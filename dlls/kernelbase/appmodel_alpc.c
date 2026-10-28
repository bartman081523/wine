/* ALPC port facade: a stand-in for kernelbase/@ and ntdll/@ exports.
 * Ported from the test_h kernelbase appmodel_fake.c ALPC section; the
 * package-identity half of that work lives in appmodel_fake.c and
 * appmodel_registry.c (appmodel-package-registry patch). */

/* --- ALPC port facade ---
 * CoreMessagingXP, the dispatcher core behind every WinUI window,
 * speaks real ALPC through the ntdll.dll!NtAlpc* family: it creates
 * its message port and reaches the runtime's broker port, neither of
 * which wine implements, so the loader's stub raises on the first
 * call and takes the whole process down. A stand-in port is one named
 * object with two direction queues: NtAlpcCreatePort makes it and
 * binds a server-side handle, NtAlpcConnectPort joins it with a
 * client-side handle under the same name, and a send on either end
 * drops the message copy on the opposite direction's queue so a
 * client send lands on the dispatcher's parked receive while a server
 * send reaches the client's pending receive. Receiving dequeues one
 * message and parks on the caller's timeout while the owning
 * direction is empty, and connecting with no port under the name
 * reports OBJECT_NAME_NOT_FOUND like an absent broker would, with
 * everything beyond the queues answering the precise failure NTSTATUS
 * so callers keep their documented fallback paths. */

#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

#include "ntstatus.h"
#define WIN32_NO_STATUS
#include "windef.h"
#include "winbase.h"
#include "winreg.h"
#include "winerror.h"
#include "winternl.h"
#include "appmodel.h"

#include "wine/debug.h"

WINE_DEFAULT_DEBUG_CHANNEL(appmodel);

struct alpc_queue_node
{
    struct alpc_queue_node *next;
    SIZE_T size;
    void *context;                /* sender-attached context record copy, or
                                   * NULL to have delivery stamp a fresh one */
    /* message data follows */
};

#define ALPC_END_SERVER     0
#define ALPC_END_CLIENT     1

/* Message attribute flag CoreMessagingXP's dispatch requests from every
 * delivered message (AlpcGetMessageAttribute 0x20000000). */
#define ALPC_MESSAGE_CONTEXT_FLAG 0x20000000

/* Broker-side message context handed out through the context attribute.
 * CoreMessagingXP's dispatch takes the first pointer of the attribute
 * block as the message's context object, and the accept path consumes the
 * fields in place: payload pointer at 0x10, payload size at 0x18, presence
 * flag at 0x1c, with 0x08 and 0x20 reserved for the sender identity only
 * the failure paths read. Delivery stamps a per-message record describing
 * the delivered message (see alpc_context_stamp); this zeroed one only
 * covers the out-of-memory fallback, where the consumer sees the zero
 * presence flag and skips the payload copies. */
static BYTE alpc_msg_context[0x28];
static unsigned char alpc_saved_session_guid[0x10];
static BOOL alpc_have_session_guid;
static volatile LONG alpc_cmxp_client_id = 0;

static const unsigned char cmxp_reply_body[0x20] =
{
    0x0c, 0x00, 0x00, 0x00,         /* dword0: body length / 4 = 12 (48 bytes) */
    0x00, 0x00, 0x04, 0x00,         /* dword1: u16@+4 key=0, u16@+6 counter=4 */
    0x04, 0x00, 0x00, 0x00,         /* element 0 header: 4 bytes */
    0x00, 0x00, 0x00, 0x00,         /*   status = S_OK */
    0x08, 0x00, 0x00, 0x00,         /* element 1 header: 8 bytes */
    0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                                    /*   RegistrarClientId = 1 */
    0x10, 0x00, 0x00, 0x00          /* element 2 header: 16 bytes (GUID copied below) */
};

/* Schema counter 20 (0x14) reply for RegisterEndpoint (action 14 = 0x000e)
 * Type stream: [status u32][u32 param1][u32 param2][u64 param3 / client_id].
 * Handler 0x180073940 builds a 3-element Array [UInt32, UInt32, UInt64]
 * which FUN_18002b868 consumes: node0=UInt32, node1=UInt32 (param2 != 0), node2=UInt64.
 * Body size: 11 dwords = 44 bytes (0x2c). */
static const unsigned char cmxp_reply_body_counter20[0x24] =
{
    0x0b, 0x00, 0x00, 0x00,         /* dword0: body length / 4 = 11 (44 bytes total) */
    0x00, 0x00, 0x14, 0x00,         /* dword1: u16@+4 key=0, u16@+6 counter=0x14 (20) */
    0x04, 0x00, 0x00, 0x00,         /* element 0 header: 4 bytes */
    0x00, 0x00, 0x00, 0x00,         /*   status = S_OK (0) */
    0x04, 0x00, 0x00, 0x00,         /* element 1 header: 4 bytes */
    0x01, 0x00, 0x00, 0x00,         /*   param 1: uint32 = 1 */
    0x04, 0x00, 0x00, 0x00,         /* element 2 header: 4 bytes */
    0x01, 0x00, 0x00, 0x00,         /*   param 2: uint32 = 1 (MUST NOT BE 0!) */
    0x08, 0x00, 0x00, 0x00          /* element 3 header: 8 bytes (client_id copied below) */
};

/* Schema counter 23 (0x17) reply for ConnectToEndpoint (action 17 = 0x0011)
 * Type stream: [status u32][u32 param1][u64 client_id][guid conversation_id][array endpoints].
 * Handler 0x180073720 builds an Array [UInt32, UInt64, ConversationInfo, Array]
 * which FUN_18002bae8 consumes: node0=UInt32, node1=UInt64, node2=ConversationInfo, node3=Array.
 * Body size: 15 dwords = 60 bytes (0x3c). */
static const unsigned char cmxp_reply_body_counter23[0x3c] =
{
    0x0f, 0x00, 0x00, 0x00,         /* dword0: body length / 4 = 15 (60 bytes total) */
    0x00, 0x00, 0x17, 0x00,         /* dword1: u16@+4 key=0, u16@+6 counter=0x17 (23) */
    0x04, 0x00, 0x00, 0x00,         /* element 0 header: 4 bytes */
    0x00, 0x00, 0x00, 0x00,         /*   status = S_OK (0) */
    0x04, 0x00, 0x00, 0x00,         /* element 1 header: 4 bytes */
    0x01, 0x00, 0x00, 0x00,         /*   param 1: uint32 = 1 */
    0x08, 0x00, 0x00, 0x00,         /* element 2 header: 8 bytes */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* client_id copied below */
    0x10, 0x00, 0x00, 0x00,         /* element 3 header: 16 bytes (GUID copied below) */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00          /* element 4 header: 0 bytes (empty endpoints array) */
};

/* Tag inside the payload-marker word (0x24) of every context record the
 * stand-in stamped itself, so a later send never propagates one of our
 * records back as a sender-owned context. */
#define ALPC_CONTEXT_TAG 0x584f4441u    /* 'XODA' */

struct alpc_port_object
{
    SRWLOCK queue_lock;
    struct alpc_queue_node *queue_head[2], *queue_tail[2];
    ULONG queue_count[2];         /* messages our own puts queued per direction */
    HANDLE signal[2];             /* one auto-reset event per direction */
    WCHAR *name;                  /* bytes the server end was created under */
    USHORT name_len;              /* name length in bytes */
};

static struct alpc_port_object *alpc_port_object_create( const UNICODE_STRING *name )
{
    struct alpc_port_object *port = RtlAllocateHeap( GetProcessHeap(), 0, sizeof(*port) );

    if (!port)
        return NULL;
    RtlInitializeSRWLock( &port->queue_lock );
    port->queue_head[ALPC_END_SERVER] = NULL;
    port->queue_head[ALPC_END_CLIENT] = NULL;
    port->queue_tail[ALPC_END_SERVER] = NULL;
    port->queue_tail[ALPC_END_CLIENT] = NULL;
    port->queue_count[ALPC_END_SERVER] = 0;
    port->queue_count[ALPC_END_CLIENT] = 0;
    port->signal[ALPC_END_SERVER] = NULL;
    port->signal[ALPC_END_CLIENT] = NULL;
    port->name = NULL;
    port->name_len = 0;

    if (name && name->Length)
    {
        if (!(port->name = RtlAllocateHeap( GetProcessHeap(), HEAP_ZERO_MEMORY, name->Length + sizeof(WCHAR) )))
        {
            RtlFreeHeap( GetProcessHeap(), 0, port );
            return NULL;
        }
        memcpy( port->name, name->Buffer, name->Length );
        port->name[name->Length / sizeof(WCHAR)] = 0;
        port->name_len = name->Length;
    }
    return port;
}

static void alpc_port_object_free( struct alpc_port_object *port )
{
    if (port->name)
        RtlFreeHeap( GetProcessHeap(), 0, port->name );
    RtlFreeHeap( GetProcessHeap(), 0, port );
}

static BOOL alpc_port_name_equal( const struct alpc_port_object *port, const UNICODE_STRING *name )
{
    UNICODE_STRING stored;

    if (port->name_len != name->Length)
        return FALSE;
    stored.Buffer = port->name;
    stored.Length = stored.MaximumLength = port->name_len;
    return RtlEqualUnicodeString( &stored, name, TRUE );
}

/* The stand-in handles are real events, so closing one is safe; each
 * maps to its port object plus the direction it serves through this
 * table. */
#define ALPC_PORT_HANDLE_MAX    16

static struct
{
    HANDLE event;
    struct alpc_port_object *port;
    unsigned int end;
} alpc_handles[ALPC_PORT_HANDLE_MAX];
static SRWLOCK alpc_port_lock = SRWLOCK_INIT;

static void alpc_queue_put( struct alpc_port_object *port, unsigned int end, const PORT_MESSAGE *msg,
                            USHORT type_stamp, const void *context );
static BOOL alpc_queue_get( struct alpc_port_object *port, unsigned int end,
                            PORT_MESSAGE *buffer, SIZE_T buffer_size, SIZE_T *received,
                            void **context );
static BYTE *alpc_attr_block( void *header, ULONG attribute_flag );

/* appmodel_fake.c: posts to the IO completion port(s) the packet that was
 * associated with target stands in for, keyed as the association gave it. */
void wait_packet_post_completions( void *target );

static LONG alpc_handle_register( HANDLE event, struct alpc_port_object *port, unsigned int end )
{
    unsigned int slot;

    RtlAcquireSRWLockExclusive( &alpc_port_lock );
    for (slot = 0; slot < ALPC_PORT_HANDLE_MAX; ++slot)
        if (!alpc_handles[slot].port)
            break;
    if (slot != ALPC_PORT_HANDLE_MAX)
    {
        alpc_handles[slot].event = event;
        alpc_handles[slot].port = port;
        alpc_handles[slot].end = end;
    }
    RtlReleaseSRWLockExclusive( &alpc_port_lock );

    return slot == ALPC_PORT_HANDLE_MAX ? STATUS_NO_MEMORY : STATUS_SUCCESS;
}

static LONG alpc_port_from_handle( void *port_handle, struct alpc_port_object **port,
                                   unsigned int *end )
{
    unsigned int slot;

    RtlAcquireSRWLockShared( &alpc_port_lock );
    for (slot = 0; slot < ALPC_PORT_HANDLE_MAX; ++slot)
        if (alpc_handles[slot].port && alpc_handles[slot].event == (HANDLE)(UINT_PTR)port_handle)
        {
            *port = alpc_handles[slot].port;
            *end = alpc_handles[slot].end;
            break;
        }
    RtlReleaseSRWLockShared( &alpc_port_lock );

    return slot == ALPC_PORT_HANDLE_MAX ? STATUS_INVALID_HANDLE : STATUS_SUCCESS;
}

LONG WINAPI NtAlpcCreatePort( void *port_handle, const OBJECT_ATTRIBUTES *object_attributes,
                              void *port_attributes )
{
    OBJECT_ATTRIBUTES attrs;
    struct alpc_port_object *port;
    HANDLE event;
    LONG status;

    FIXME( "(%p, %p [%s], %p): event object stand-in for the port\n",
           port_handle, object_attributes,
           (object_attributes && object_attributes->ObjectName)
               ? debugstr_wn( object_attributes->ObjectName->Buffer,
                              object_attributes->ObjectName->Length / sizeof(WCHAR) ) : "-",
           port_attributes );

    if (!port_handle)
        return STATUS_INVALID_PARAMETER;

    if (!(port = alpc_port_object_create( object_attributes ? object_attributes->ObjectName : NULL )))
        return STATUS_NO_MEMORY;

    InitializeObjectAttributes( &attrs, NULL, 0, 0, NULL );

    /* port access masks carry bits the event type does not know;
     * use anonymous event so wineserver path resolution is not invoked */
    status = NtCreateEvent( &event, EVENT_ALL_ACCESS, &attrs, NotificationEvent, FALSE );
    if (status)
    {
        WARN( "NtAlpcCreatePort: NtCreateEvent failed: %08lx\n", status );
        alpc_port_object_free( port );
        return status;
    }
    status = alpc_handle_register( event, port, ALPC_END_SERVER );
    if (status)
    {
        NtClose( event );
        alpc_port_object_free( port );
        return status;
    }

    *(HANDLE *)port_handle = event;
    return STATUS_SUCCESS;
}

/* Server-side accept of a handshake that in our single-world stand-in
 * never arrives. */
LONG WINAPI NtAlpcAcceptConnectPort( void *port_handle, void *connection_port, ULONG flags,
                                     void *object_attributes, void *port_attributes,
                                     void *port_context, void *connection_request,
                                     void *connection_attributes, BOOLEAN accept_connection )
{
    FIXME( "(%p, %p, %#lx, %p, %p, %p, %p, %p, %u): no pending connection\n",
           port_handle, connection_port, (unsigned long)flags, object_attributes,
           port_attributes, port_context, connection_request, connection_attributes,
           accept_connection );
    return STATUS_NOT_IMPLEMENTED;
}

/* A connection exists as long as some stand-in port owns the requested
 * name; without it the absent-broker answer is the honest one. */
#define ALPC_CONN_REQUEST_SIZE 0x50

LONG WINAPI NtAlpcConnectPort( void *port_handle, const UNICODE_STRING *server_name,
                               void *object_attributes, void *port_attributes, ULONG flags,
                               void *required_server_sid, void *connection_message,
                               void *buffer_length, void *out_message_attributes,
                               void *in_message_attributes, void *timeout )
{
    OBJECT_ATTRIBUTES attrs;
    struct alpc_port_object *port = NULL;
    unsigned int slot;
    HANDLE event;
    PORT_MESSAGE *conn;
    LONG status;

    FIXME( "(%p, %s, %p, %p, %#lx, %p, %p, %p, %p, %p, %p): join the named server stand-in\n",
           port_handle, debugstr_wn( server_name->Buffer, server_name->Length / sizeof(WCHAR) ),
           object_attributes, port_attributes, (unsigned long)flags, required_server_sid,
           connection_message, buffer_length, out_message_attributes, in_message_attributes,
           timeout );

    if (!port_handle || !server_name)
        return STATUS_INVALID_PARAMETER;

    RtlAcquireSRWLockShared( &alpc_port_lock );
    for (slot = 0; slot < ALPC_PORT_HANDLE_MAX; ++slot)
        if (alpc_handles[slot].port && alpc_handles[slot].end == ALPC_END_SERVER
            && alpc_port_name_equal( alpc_handles[slot].port, server_name ))
        {
            port = alpc_handles[slot].port;
            break;
        }
    RtlReleaseSRWLockShared( &alpc_port_lock );

    if (!port)
        return STATUS_OBJECT_NAME_NOT_FOUND;

    InitializeObjectAttributes( &attrs, NULL, 0, 0, NULL );
    status = NtCreateEvent( &event, EVENT_ALL_ACCESS, &attrs, NotificationEvent, FALSE );
    if (status)
        return status;

    status = alpc_handle_register( event, port, ALPC_END_CLIENT );
    if (status)
    {
        NtClose( event );
        return status;
    }

    /* real LPC delivers the connection request to the server end's first
     * receive. The stand-in takes the OS broker's delivery role, and that
     * role stamps the request into the wire form the receiving server
     * expects: the parser CoreMessagingXP runs over the request demands a
     * 0x50-byte header with the total size, zeroed control words and the
     * request-info bookkeeping at 0x38/0x40/0x42/0x48; the connecting
     * client writes neither, so synthesize the delivered copy. */
    if (connection_message)
    {
        unsigned char *raw;
        unsigned int body_len;
        unsigned long total_size;

        /* DIAG: the connecting client's own request buffer, forwarded
         * below; the dump shows what the client prepared. */
        {
            char hex[0xf1];
            unsigned int i;
            const PORT_MESSAGE *cm = (const PORT_MESSAGE *)connection_message;

            TRACE( "client connection message %p: data %#x total %#x type %#x\n",
                   connection_message, cm->DataSize, cm->MessageSize, cm->MessageType );
            hex[0] = 0;
            for (i = 0; i < 0x78; ++i)
                sprintf( hex + i * 2, "%02x", ((const unsigned char *)connection_message)[i] );
            hex[0xf0] = 0;
            TRACE( "client connection message bytes: %s\n", hex );
        }

        /* forward the client's own body: the broker's connection handler
         * builds the per-session registry entries from the credentials
         * the client wrote at 0x28, and an empty-body request leaves
         * those entries absent — the dispatcher then dies on its first
         * delivery lookup */
        body_len = ((const PORT_MESSAGE *)connection_message)->DataSize;
        if (body_len >= 0x10)
        {
            memcpy( alpc_saved_session_guid, (const unsigned char *)connection_message + 0x28, 0x10 );
            alpc_have_session_guid = TRUE;
        }
        total_size = ALPC_CONN_REQUEST_SIZE + body_len;
        if ((conn = RtlAllocateHeap( GetProcessHeap(), 0, total_size )))
        {
            memset( conn, 0, total_size );
            raw = (unsigned char *)conn;
            conn->MessageSize = total_size;
            conn->DataSize = total_size - 0x28;
            conn->MessageType = 1;              /* LPC_CONNECTION_REQUEST */
            *(unsigned int *)(raw + 0x38) = 4;  /* request-info: parsed-request form */
            *(unsigned short *)(raw + 0x42) = 1;
            *(unsigned int *)(raw + 0x48) = body_len;
            if (body_len)
                memcpy( raw + 0x50, (const unsigned char *)connection_message + 0x28,
                        body_len );
            alpc_queue_put( port, ALPC_END_SERVER, conn, 0, NULL );
            RtlFreeHeap( GetProcessHeap(), 0, conn );
        }
        else
            alpc_queue_put( port, ALPC_END_SERVER, (const PORT_MESSAGE *)connection_message,
                            1 /* LPC_CONNECTION_REQUEST */, NULL );
    }

    *(HANDLE *)port_handle = event;
    return STATUS_SUCCESS;
}

LONG WINAPI NtAlpcDisconnectPort( void *port_handle, ULONG flags )
{
    FIXME( "(%p, %#lx): nothing to tear down\n", port_handle, (unsigned long)flags );
    return STATUS_SUCCESS;
}

LONG WINAPI NtAlpcQueryInformation( void *port_handle, ULONG information_class, void *information,
                                    ULONG length, ULONG *return_length )
{
    FIXME( "(%p, %#lx, %p, %#lx, %p): no port information behind the stand-in\n",
           port_handle, (unsigned long)information_class, information,
           (unsigned long)length, return_length );
    return STATUS_NOT_IMPLEMENTED;
}

/* The direction queues behind every stand-in port. Nodes carry a full
 * port message (header with its sizes plus payload); each of the two
 * queues has its own auto-reset event waking one receiver per arriving
 * message, and a receive parks on its direction's timeout while that
 * queue is empty. */
static HANDLE alpc_queue_ensure_signal( struct alpc_port_object *port, unsigned int end )
{
    HANDLE event = NULL;

    RtlAcquireSRWLockExclusive( &port->queue_lock );
    if (!port->signal[end])
    {
        OBJECT_ATTRIBUTES attrs;

        InitializeObjectAttributes( &attrs, NULL, 0, 0, NULL );
        NtCreateEvent( &event, EVENT_ALL_ACCESS, &attrs, SynchronizationEvent, FALSE );
        port->signal[end] = event;
    }
    else
        event = port->signal[end];
    RtlReleaseSRWLockExclusive( &port->queue_lock );

    return event;
}

/* Queues a copy of msg on the given end. The kernel stamps the message
 * type it delivers rather than trusting the sender's buffer; type_stamp
 * mirrors that on the copy (0 keeps the sender's value). The context field
 * carries a sender-attached record copy (see NtAlpcSendWaitReceivePort),
 * or NULL to let delivery stamp a fresh record describing the message. */
static void alpc_queue_put( struct alpc_port_object *port, unsigned int end, const PORT_MESSAGE *msg,
                            USHORT type_stamp, const void *context )
{
    struct alpc_queue_node *node;
    HANDLE signal;
    unsigned int slot;

    /* The reply normalizer below may rewrite a message into a longer wire
     * form than the sender supplied (0x70 -> 0x80 connect reply, or 0x50 -> 0x80 thread reply);
     * the slack keeps that in-place rewrite inside the allocation. */
    if (!(node = RtlAllocateHeap( GetProcessHeap(), 0, sizeof(*node) + (msg->MessageSize < 0x80 ? 0x80 : msg->MessageSize) + 0x40 )))
    {
        FIXME( "out of memory for a %u-byte message\n", msg->MessageSize );
        return;
    }
    node->size = msg->MessageSize;
    node->context = (void *)context;
    memcpy( node + 1, msg, msg->MessageSize );
    /* DIAG: every byte of the queued copy before the length normalizer
     * touches it, so runs show what the sender really wrote at 0x50+. */
    {
        char hex[0x401];
        unsigned int i, n = node->size < 0x200 ? node->size : 0x200;

        hex[0] = 0;
        for (i = 0; i < n; ++i)
            sprintf( hex + i * 2, "%02x", ((const unsigned char *)(node + 1))[i] );
        hex[2 * n] = 0;
        TRACE( "queued %u-byte message from %p on end %u: %s\n", (unsigned int)node->size,
               msg, end, hex );
    }
    if (type_stamp)
        ((PORT_MESSAGE *)(node + 1))->MessageType = type_stamp;

    /* The connect-request form the broker's request parser demands is the
     * 0x50-byte header with the body length at 0x48 (see the synthesis in
     * NtAlpcConnectPort); raw client requests carrying only the 0x28-byte
     * port-message header get their total stamped to that form. Every
     * other message is the broker's or the client's own: both sides ship
     * standard port messages (MessageSize = DataSize + 0x28) whose 0x48
     * word is the envelope's own bookkeeping, not a body length —
     * rewriting their total here truncated real payload off the broker's
     * 0x70-byte connect reply and zeroed the dispatch length (run20e's
     * 0x1800a3390 dispatch AV). */
    {
        PORT_MESSAGE *copy = (PORT_MESSAGE *)(node + 1);
        if (copy->MessageType == 1 /* LPC_CONNECTION_REQUEST */)
        {
            unsigned int body_size = *(const unsigned int *)((const char *)copy + 0x48);
            /* the queue copy was sized from the sender's original total;
             * never stamp a header that claims bytes past the block */
            if (!body_size || body_size + 0x50 > node->size) { /* not the 0x50-header form */ }
            else if (copy->MessageSize != body_size + 0x50)
            {
                FIXME( "normalized connect request: total %#x -> %#x (body %#x)\n",
                       copy->MessageSize, body_size + 0x50, body_size );
                copy->MessageSize = body_size + 0x50;
            }
        }
        else if (node->size >= 0x4c)
        {
            unsigned int body_size = *(const unsigned int *)((const char *)copy + 0x48);
            /* The receiver-side contract (CoreMessagingXP's message validator
             * at CMXP+0x18007cae4, reached through the LPC_REPLY dispatch of
             * CMXP+0x18007b33c) demands MessageSize == [0x48] + 0x50 of every
             * non-bare message: 0x18007cae4 fails on the reply of run20g's
             * dance if the word disagrees, and the dispatch FailFasts.  The
             * broker ships its replies as the standard envelope
             * (MessageSize = DataSize + 0x28) without the word, so refill it
             * from the total here — that form, body length = total - 0x50
             * with the body at 0x50, is exactly what the dispatch then reads
             * (CMXP+0x18009430: 0x48(%r8) -> length, 0x50(%r8) -> body). */
            /* The broker's 0x70-byte type-2 session reply carries a wire
             * body its own client fails to parse (run48 Exit-64): [dword0
             * = 1][dword1 = 0] plus the raw 16-byte session GUID and 8
             * zero bytes — the deserializer 0x180090364 reads the
             * element-size word at body+8 and dies on the GUID's first
             * dword 0x29324ee4 (error 2 -> HCALL -> ProtocolException ->
             * FailFast). run49 proved the reply pump uses the OTHER schema
             * static: schema = CMXP+0x1800b7770 = {count 1, keytab
             * 0x1800b7760}, key entry {key 0, max 0x1d, desc 0x1800adba0},
             * argBuf = CMXP+0x1800bf120. run50 (16-byte body, key 0,
             * counter 0, one 4-byte element) parsed GREEN and still
             * Exit-64'd: counter 0's element handler 0x180072eb0 is
             * CMXP's protocol-assertion raiser — after a green parse the
             * handler application turns the parsed element value into
             * RaiseFailFastException via 0x180072ad8 -> 0x18002e8f4 ->
             * 0x18007298c (code 0xe0464645 'FF'). The schema's zero-
             * element variants (counter 3/9/10/14/15/21/22, all with the
             * benign handler 0x180072f30 and argBuf tag byte 0 at offset
             * 19) are the parseable-and-safe forms: mirror the broker's
             * own 88-byte request grammar (d0 = body/4 = 2, key, counter,
             * no elements, full consume). Rewrite the reply into the
             * 8-byte key-0 counter-3 empty form. */
            if (copy->MessageType == 2 /* LPC_REPLY */ &&
                ((copy->MessageSize == 0x70 &&
                  *(const unsigned int *)((const char *)copy + 0x50) == 1 &&
                  *(const unsigned int *)((const char *)copy + 0x54) == 0) ||
                 (copy->MessageSize == 0x50 &&
                  *(const unsigned int *)((const char *)copy + 0x48) == 0)))
            {
                /* Reply schema side (run49: schema 0x1800b7770, key tab
                 * {key 0, max 29, desc 0x1800adba0}, argBuf 0x1800bf120)
                 * variant counter 4 = desc {fn 0x180073aa0, off 21, tag 3}
                 * with type stream [u32 status][8-B RegistrarClientId]
                 * [16-B Guid]. Handler 0x180073aa0 only completes the
                 * pending call when the status element is 0: it then builds
                 * the result record from FUN_18002a480 (typed node, type
                 * desc 0x1800b5550 "RegistrarClientId", value = element 1)
                 * and FUN_18002a34c (typed node, type desc 0x1800b5890
                 * "Guid", value = element 2 as two qwords) and hands the
                 * tuple to the completion. Validation 0x18002be18 (run51's
                 * AV site) then demands node0 = RegistrarClientId and
                 * node1 = Guid with count >= 2.
                 * In run88, thread 019c called RegisterThread and received an
                 * unformatted 0x50-byte reply with [0x48]==0; HandleSynchronousReply
                 * treated it as non-bare, calling ProcessReply(NULL, 0) ->
                 * CoreUICallReceive -> UnmarshalReceive which returned S_OK without
                 * setting method_descriptor, crashing with AV at 0x18008f40a.
                 * Rewriting both 0x70 session replies and 0x50 thread replies
                 * into the counter-4 wire form completes both call sites. */
                if (copy->MessageSize == 0x70)
                {
                    memcpy( alpc_saved_session_guid, (const char *)copy + 0x58, sizeof(alpc_saved_session_guid) );
                    alpc_have_session_guid = TRUE;
                }

                if (alpc_have_session_guid)
                {
                    ULONG64 client_id = (ULONG64)InterlockedIncrement( &alpc_cmxp_client_id );
                    TRACE( "rewrote %#x-byte broker reply into the parseable 48-byte counter-4 wire form (client_id=%I64u)\n",
                           copy->MessageSize, client_id );
                    memcpy( (char *)copy + 0x50, cmxp_reply_body, sizeof(cmxp_reply_body) );
                    memcpy( (char *)copy + 0x64, &client_id, sizeof(client_id) );
                    memcpy( (char *)copy + 0x70, alpc_saved_session_guid, sizeof(alpc_saved_session_guid) );
                    node->size = copy->MessageSize = 0x80;
                    copy->DataSize = 0x58;
                }
                else
                {
                    TRACE( "no session guid; rewriting %#x-byte empty reply to 0x28 bare form\n",
                           copy->MessageSize );
                    node->size = copy->MessageSize = 0x28;
                    copy->DataSize = 0;
                }
            }
            if (copy->MessageSize >= 0x50 && copy->MessageSize != body_size + 0x50)
            {
                TRACE( "stamped envelope body length: %#x -> %#x (total %#x)\n",
                       body_size, copy->MessageSize - 0x50, copy->MessageSize );
                *(unsigned int *)((char *)copy + 0x48) = copy->MessageSize - 0x50;
            }
        }
    }

    RtlAcquireSRWLockExclusive( &port->queue_lock );
    if (port->queue_tail[end] && !port->queue_count[end])
    {
        FIXME( "queue %u of port %p corrupt: tail %p without a queued message, resetting\n",
               end, port, port->queue_tail[end] );
        port->queue_head[end] = NULL;
        port->queue_tail[end] = NULL;
    }
    node->next = NULL;
    if (port->queue_tail[end])
        port->queue_tail[end]->next = node;
    else
        port->queue_head[end] = node;
    port->queue_tail[end] = node;
    ++port->queue_count[end];
    RtlReleaseSRWLockExclusive( &port->queue_lock );

    TRACE( "queued %u-byte message %p on %s end of port %p (head %p, type %#x)\n", (unsigned)msg->MessageSize,
           node, end == ALPC_END_SERVER ? "server" : "client", port, node,
           (unsigned)((const PORT_MESSAGE *)(node + 1))->MessageType );
    if ((signal = alpc_queue_ensure_signal( port, end )))
        NtSetEvent( signal, NULL );
    /* the other side may wait on the port's own handle rather than a
     * SendWait receive park: completion-packet driven pumping (CoreMessagingXP's
     * pump associates the CreatePort event with its NtCreateIoCompletion port
     * via NtAssociateWaitCompletionPacket and parks on
     * NtWaitForMultipleObjects over that event) needs the port handle event
     * itself signaled, not only the lazy queue signal. */
    RtlAcquireSRWLockShared( &alpc_port_lock );
    for (slot = 0; slot < ALPC_PORT_HANDLE_MAX; ++slot)
    {
        if (alpc_handles[slot].port == port && alpc_handles[slot].end == end)
        {
            NtSetEvent( alpc_handles[slot].event, NULL );
            /* and the pump's actual wake comes through the completion the
             * associated packet posts on the pump's io port (run47: the
             * port event alone left it parked) */
            wait_packet_post_completions( alpc_handles[slot].event );
        }
    }
    RtlReleaseSRWLockShared( &alpc_port_lock );
}

static void *alpc_context_stamp( PORT_MESSAGE *msg );

static BOOL alpc_fulfill_cmxp_reply( struct alpc_port_object *port, const PORT_MESSAGE *send_msg,
                                     USHORT action, PORT_MESSAGE *receive_message,
                                     SIZE_T *buffer_length, void *receive_attributes )
{
    char reply_buf[0x100];
    PORT_MESSAGE *reply = (PORT_MESSAGE *)reply_buf;
    ULONG64 client_id = (ULONG64)InterlockedIncrement( &alpc_cmxp_client_id );
    SIZE_T copy_len;

    if (!receive_message || !buffer_length)
        return FALSE;

    memset( reply_buf, 0, sizeof(reply_buf) );
    reply->MessageType = 2; /* LPC_REPLY */
    if (send_msg)
    {
        reply->MessageId = send_msg->MessageId;
        reply->SectionSize = send_msg->SectionSize;
    }
    *(unsigned short *)(reply_buf + 0x42) = 1;

    if (action == 0x000e)
    {
        /* RegisterEndpoint: schema counter 20 (0x14) */
        reply->MessageSize = 0x7c;
        reply->DataSize = 0x54;
        *(unsigned int *)(reply_buf + 0x48) = 0x2c;
        memcpy( reply_buf + 0x50, cmxp_reply_body_counter20, sizeof(cmxp_reply_body_counter20) );
        memcpy( reply_buf + 0x50 + sizeof(cmxp_reply_body_counter20), &client_id, sizeof(client_id) );
        TRACE( "directly fulfilled CMXP counter-20 reply (action %u, MessageId=%lu, client_id=%I64u)\n",
               action, reply->MessageId, client_id );
    }
    else if (action == 0x0011)
    {
        /* ConnectToEndpoint (e.g. System\CompositionEngine): schema counter 23 (0x17) */
        reply->MessageSize = 0x8c;
        reply->DataSize = 0x64;
        *(unsigned int *)(reply_buf + 0x48) = 0x3c;
        memcpy( reply_buf + 0x50, cmxp_reply_body_counter23, sizeof(cmxp_reply_body_counter23) );
        memcpy( reply_buf + 0x6c, &client_id, sizeof(client_id) );
        if (alpc_have_session_guid)
            memcpy( reply_buf + 0x78, alpc_saved_session_guid, sizeof(alpc_saved_session_guid) );
        TRACE( "directly fulfilled CMXP counter-23 reply (action 17, MessageId=%lu, client_id=%I64u)\n",
               reply->MessageId, client_id );
    }
    else
    {
        /* RegisterNotificationEndpoint or thread registration: schema counter 4 */
        reply->MessageSize = 0x80;
        reply->DataSize = 0x58;
        *(unsigned int *)(reply_buf + 0x48) = 0x30;
        memcpy( reply_buf + 0x50, cmxp_reply_body, sizeof(cmxp_reply_body) );
        memcpy( reply_buf + 0x64, &client_id, sizeof(client_id) );
        if (alpc_have_session_guid)
            memcpy( reply_buf + 0x70, alpc_saved_session_guid, sizeof(alpc_saved_session_guid) );
        TRACE( "directly fulfilled CMXP counter-4 reply (action %u, MessageId=%lu, client_id=%I64u)\n",
               action, reply->MessageId, client_id );
    }

    copy_len = min( *buffer_length, reply->MessageSize );
    memcpy( receive_message, reply, copy_len );
    *buffer_length = reply->MessageSize;

    if (receive_attributes)
    {
        BYTE *context_block = alpc_attr_block( receive_attributes, ALPC_MESSAGE_CONTEXT_FLAG );
        if (context_block)
        {
            void *received_context = alpc_context_stamp( receive_message );
            if (!received_context)
                received_context = alpc_msg_context;
            *(void **)context_block = received_context;
            ((ULONG *)receive_attributes)[1] |= ALPC_MESSAGE_CONTEXT_FLAG;
        }
    }

    return TRUE;
}

/* Copies the oldest queued message in the given direction into the
 * receive buffer; returns TRUE, the message size and the message's
 * round-tripped context on success. Oversized messages are dropped
 * rather than corrupting the caller's parse. */
static BOOL alpc_queue_get( struct alpc_port_object *port, unsigned int end,
                            PORT_MESSAGE *buffer, SIZE_T buffer_size, SIZE_T *received,
                            void **context )
{
    struct alpc_queue_node *node;
    BOOL empty;
    unsigned int slot;

    RtlAcquireSRWLockExclusive( &port->queue_lock );
    node = port->queue_head[end];
    if (node && !port->queue_count[end])
    {
        FIXME( "queue %u of port %p corrupt: head %p without a queued message, ignoring it\n",
               end, port, node );
        port->queue_head[end] = NULL;
        port->queue_tail[end] = NULL;
        node = NULL;
    }
    if (node)
    {
        port->queue_head[end] = node->next;
        if (!port->queue_head[end])
            port->queue_tail[end] = NULL;
        --port->queue_count[end];
    }
    empty = !port->queue_count[end];
    RtlReleaseSRWLockExclusive( &port->queue_lock );

    if (empty)
    {
        RtlAcquireSRWLockShared( &alpc_port_lock );
        for (slot = 0; slot < ALPC_PORT_HANDLE_MAX; ++slot)
        {
            if (alpc_handles[slot].port == port && alpc_handles[slot].end == end)
                NtResetEvent( alpc_handles[slot].event, NULL );
        }
        RtlReleaseSRWLockShared( &alpc_port_lock );
    }

    if (!node)
        return FALSE;

    if (node->size > buffer_size)
    {
        FIXME( "message of %u bytes exceeds the %u-byte receive buffer, dropping it\n",
               (unsigned)node->size, (unsigned)buffer_size );
        RtlFreeHeap( GetProcessHeap(), 0, node );
        return FALSE;
    }

    memcpy( buffer, node + 1, node->size );
    *received = node->size;
    *context = node->context;
    TRACE( "delivered %u-byte message %p to %s end of port %p (type %#x, context %p)\n", (unsigned)node->size,
           buffer, end == ALPC_END_SERVER ? "server" : "client", port, (unsigned)buffer->MessageType,
           node->context );
    /* DIAG(nostic-only, not mirrored): dump the delivered message bytes
     * so the log shows what the dispatch actually receives. */
    {
        char hex[0x401];
        unsigned int i;
        hex[0] = 0;
        for (i = 0; i < node->size && i < 0x200; ++i)
            sprintf( hex + i * 2, "%02x", ((const unsigned char *)(node + 1))[i] );
        TRACE( "delivered bytes: %s\n", hex );
    }
    RtlFreeHeap( GetProcessHeap(), 0, node );
    return TRUE;
}

/* Delivery-side context record. A sender that attaches no context of its
 * own (every message today) still reaches its consumer with a record the
 * accept and dispatch paths can consume: payload pointer (0x10), payload
 * size (0x18) and presence flag (0x1c) describe the consumer layout run13
 * mapped. Connection requests keep the request-info form with the client
 * body at 0x50 behind its length at 0x48; other messages carry the
 * port-message payload at 0x28 sized by DataSize. The record points into
 * the receiver's own receive buffer, so it outlives the queue node and is
 * intentionally never freed. The tag at 0x24 marks records the stand-in
 * authored: only a pristine stamp (tail 0x28..0x3f still zero) is refused
 * on a later send — a consumer that writes into the record it received
 * through AlpcGetMessageAttribute owns the result and it propagates.
 * The record is 0x40 bytes: CMXP appends per-message state at 0x28..0x3f
 * (run20g record 0x437daa0: type/count pair 0x81460009 at 0x2c plus two
 * module data pointers), so a 0x28 block would be overrun. */
static void *alpc_context_stamp( PORT_MESSAGE *msg )
{
    BYTE *record;

    if (!(record = RtlAllocateHeap( GetProcessHeap(), 0, 0x40 )))
        return NULL;
    memset( record, 0, 0x40 );
    if (msg->MessageType == 1)      /* LPC_CONNECTION_REQUEST */
    {
        *(void **)(record + 0x10) = (char *)msg + 0x50;
        *(ULONG *)(record + 0x18) = *(const ULONG *)((const char *)msg + 0x48);
    }
    else
    {
        *(void **)(record + 0x10) = (char *)msg + 0x28;
        *(ULONG *)(record + 0x18) = msg->DataSize;
    }
    *(ULONG *)(record + 0x1c) = 1;  /* payload present */
    *(ULONG *)(record + 0x24) = ALPC_CONTEXT_TAG;
    return record;
}

/* CMXP probe readout. The patched CoreMessagingXP.dll plants trampolines in
 * its .text-tail code cave that stash live register snapshots into
 * xref-free .didat slots (scratches/proton/cmxp_probe_patch.py; the capture
 * bases moved off the original .data-tail plan, so these follow the patch
 * script — run48 read the pre-move addresses and voided its dump):
 *   0x1800f4a20: pump 0x18007b760 entry — endpoint | service | [service]
 *   0x1800f4a00: resolver 0x180019220 entry — holder | id
 *   0x1800f4a40: Manager::Register 0x18005e424 entry — this | rdx | r8
 *   0x1800f4a60: _CxxThrowException 0x180045c0 entry — exception obj | ThrowInfo
 *                (ProtocolException keeps its HRESULT at obj+0x28)
 *   0x1800f4a80: throw-helper 0x1800112b0 entry — caller RA | exception instance
 *   0x1800f4aa0: async task entry 0x1800729f0 — rcx | rdx | r8 | r9 |
 *                ra1 | ra2 | ra3 (the code-101 ProtocolException thrower;
 *                zero static xrefs, invoked by the ring dispatcher)
 *   0x1800f4b00: deserializer 0x180009034c entry — rcx | rdx (schema desc) |
 *                r8 | r9 | arg5..arg12 | payload head behind arg5
 *   0x1800f4ae0: handler-dispatch call site 0x180009064c — status u32 |
 *                &status | task obj (parse failed; the deserializer then
 *                dispatches [task]->vt[0x20] = 0x1800729f0 and FF's)
 * CMXP is loaded in this process, so walk the PEB module list for its base
 * and log the slots alongside the SendWait trace: every ALPC syscall gets a
 * snapshot whenever any probe value moved. Also drained from
 * RaiseFailFastException — a CMXP client that throws ProtocolException
 * fail-fasts before the next ALPC syscall would flush the throw slots. */
void cmxp_probe_dump( void )
{
    static void *cmxp_base;
    static UINT64 s2_ep, s2_svc, s2_vt, s1_this, s1_id;
    static UINT64 s3_a, s3_b, s3_c, s4_obj, s4_ti, s5_ra, s5_inst;
    static UINT64 s6_rcx, s6_rdx, s6_r8, s6_r9, s6_ra1, s6_ra2, s6_ra3;
    static UINT64 s8o[15];                      /* deser entry: regs + args + buf */
    static UINT64 s7_status, s7_stptr, s7_task; /* handler call: parse failure */
    static UINT64 s9_rcx, s9_rdx, s9_ra, s9_peek; /* vt1 getter: caller + obj */
    static UINT64 s10_out, s10_a, s10_h1, s10_h2, s10_h1cnt, s10_h2cnt, s10_e0,
                  s10_e1;                          /* text evaluator: holders */
    static UINT64 s11_ra, s11_rcx, s11_rdx, s11_head, s11_p0;
    static UINT32 s11_code;
    static const WCHAR probe_name[] = L"CoreMessagingXP.dll";
    UNICODE_STRING name = { sizeof(probe_name) - sizeof(WCHAR), sizeof(probe_name), (PWSTR)probe_name };
    PEB_LDR_DATA *ldr = RtlGetCurrentPeb()->LdrData;
    LIST_ENTRY *cur, *head = &ldr->InLoadOrderModuleList;
    UINT64 *s2, *s1, *s3, *s4, *s5, *s6, *s7, *s8p, *s9, *s10;
    UINT64 ff_ra, ff_rcx, ff_rdx, ff_head, ff_p0, s5_wrap;
    UINT32 ff_code;

    if (!cmxp_base)
    {
        for (cur = head->Flink; cur != head; cur = cur->Flink)
        {
            LDR_DATA_TABLE_ENTRY *entry = (LDR_DATA_TABLE_ENTRY *)
                ((char *)cur - offsetof( LDR_DATA_TABLE_ENTRY, InLoadOrderLinks ));
            UNICODE_STRING *base_name = &entry->BaseDllName;

            if (base_name->Length == name.Length &&
                !wcsicmp( base_name->Buffer, probe_name ))
            {
                cmxp_base = entry->DllBase;
                break;
            }
        }
        if (!cmxp_base) return;
    }
    s2 = (UINT64 *)((char *)cmxp_base + 0xf4a20);
    s1 = (UINT64 *)((char *)cmxp_base + 0xf4a00);
    s3 = (UINT64 *)((char *)cmxp_base + 0xf4a40);
    s4 = (UINT64 *)((char *)cmxp_base + 0xf4a60);
    s5 = (UINT64 *)((char *)cmxp_base + 0xf4a80);
    s6 = (UINT64 *)((char *)cmxp_base + 0xf4aa0);
    s8p = (UINT64 *)((char *)cmxp_base + 0xf4b00);
    s7 = (UINT64 *)((char *)cmxp_base + 0xf4ae0);
    s9 = (UINT64 *)((char *)cmxp_base + 0xf4b80);
    s10 = (UINT64 *)((char *)cmxp_base + 0xf4ba0);
    ff_ra = *(UINT64 *)((char *)cmxp_base + 0xf4c60);
    ff_rcx = *(UINT64 *)((char *)cmxp_base + 0xf4c68);
    ff_rdx = *(UINT64 *)((char *)cmxp_base + 0xf4c70);
    ff_code = *(UINT32 *)((char *)cmxp_base + 0xf4c78);
    ff_head = *(UINT64 *)((char *)cmxp_base + 0xf4c80);
    ff_p0 = *(UINT64 *)((char *)cmxp_base + 0xf4c88);
    if (s2[0] == s2_ep && s2[1] == s2_svc && s2[2] == s2_vt &&
        s1[0] == s1_this && s1[1] == s1_id &&
        s3[0] == s3_a && s3[1] == s3_b && s3[2] == s3_c &&
        s4[0] == s4_obj && s4[1] == s4_ti &&
        s5[0] == s5_ra && s5[1] == s5_inst &&
        s6[0] == s6_rcx && s6[1] == s6_rdx && s6[2] == s6_r8 &&
        s6[3] == s6_r9 && s6[4] == s6_ra1 && s6[5] == s6_ra2 &&
        s6[6] == s6_ra3 &&
        s7[0] == s7_status && s7[1] == s7_stptr && s7[2] == s7_task &&
        s8p[0] == s8o[0] && s8p[1] == s8o[1] && s8p[2] == s8o[2] &&
        s8p[3] == s8o[3] && s8p[4] == s8o[4] && s8p[5] == s8o[5] &&
        s8p[6] == s8o[6] && s8p[7] == s8o[7] && s8p[8] == s8o[8] &&
        s8p[9] == s8o[9] && s8p[10] == s8o[10] && s8p[11] == s8o[11] &&
        s8p[12] == s8o[12] && s8p[13] == s8o[13] && s8p[14] == s8o[14] &&
        s9[0] == s9_rcx && s9[1] == s9_rdx && s9[2] == s9_ra &&
        s9[3] == s9_peek &&
        s10[0] == s10_out && s10[1] == s10_a && s10[2] == s10_h1 &&
        s10[3] == s10_h2 && s10[4] == s10_h1cnt && s10[5] == s10_h2cnt &&
        s10[6] == s10_e0 && s10[7] == s10_e1 &&
        ff_ra == s11_ra && ff_rcx == s11_rcx && ff_rdx == s11_rdx &&
        ff_code == s11_code && ff_head == s11_head && ff_p0 == s11_p0)
        return;
    s2_ep = s2[0]; s2_svc = s2[1]; s2_vt = s2[2];
    s1_this = s1[0]; s1_id = s1[1];
    s3_a = s3[0]; s3_b = s3[1]; s3_c = s3[2];
    s4_obj = s4[0]; s4_ti = s4[1];
    s5_ra = s5[0]; s5_inst = s5[1];
    s6_rcx = s6[0]; s6_rdx = s6[1]; s6_r8 = s6[2]; s6_r9 = s6[3];
    s6_ra1 = s6[4]; s6_ra2 = s6[5]; s6_ra3 = s6[6];
    s7_status = s7[0]; s7_stptr = s7[1]; s7_task = s7[2];
    memcpy( s8o, s8p, sizeof(s8o) );
    s9_rcx = s9[0]; s9_rdx = s9[1]; s9_ra = s9[2]; s9_peek = s9[3];
    s10_out = s10[0]; s10_a = s10[1]; s10_h1 = s10[2]; s10_h2 = s10[3];
    s10_h1cnt = s10[4]; s10_h2cnt = s10[5]; s10_e0 = s10[6]; s10_e1 = s10[7];
    s11_ra = ff_ra; s11_rcx = ff_rcx; s11_rdx = ff_rdx;
    s11_code = ff_code; s11_head = ff_head; s11_p0 = ff_p0;
    /* the thrower's rcx is the stack-allocated Holder$@ProtocolException;
     * its HRESULT lives at +0x28 (getter 0x1800a2b18). The helper's rcx is
     * the refcounted ProtocolException instance — also read +0x28 and the
     * wrapped object at +0x18 (assigned by refactorable ctor 0x1800011240),
     * and the captured return address names the code that threw. */
    s5_wrap = s5_inst ? *(UINT64 *)((char *)s5_inst + 0x18) : 0;
    /* Keep every line short: run20q showed a single ~1.5KB TRACE blowing the
     * per-thread debug line buffer ("append_output debug buffer overflow")
     * and raising STATUS_BREAKPOINT inside ntdll's debug-append path, which
     * killed the pump thread during EH unwind. */
    TRACE( "cmxp probe: pump ep=%016I64x svc=%016I64x vt=%016I64x\n",
           s2_ep, s2_svc, s2_vt );
    TRACE( "cmxp probe: resolver this=%016I64x id=%016I64x\n", s1_this, s1_id );
    TRACE( "cmxp probe: register this=%016I64x a=%016I64x b=%016I64x\n",
           s3_a, s3_b, s3_c );
    TRACE( "cmxp probe: throw obj=%016I64x ti=%016I64x hr=%08x\n",
           s4_obj, s4_ti, s4_obj ? *(UINT32 *)((char *)s4_obj + 0x28) : 0 );
    TRACE( "cmxp probe: helper ra=%016I64x inst=%016I64x hr=%08x wrap=%016I64x\n",
           s5_ra, s5_inst,
           s5_inst ? *(UINT32 *)((char *)s5_inst + 0x28) : 0, s5_wrap );
    TRACE( "cmxp probe: task rcx=%016I64x rdx=%016I64x r8=%016I64x r9=%016I64x\n",
           s6_rcx, s6_rdx, s6_r8, s6_r9 );
    TRACE( "cmxp probe: task ra1=%016I64x ra2=%016I64x ra3=%016I64x\n",
           s6_ra1, s6_ra2, s6_ra3 );
    TRACE( "cmxp probe: deser ctx=%016I64x desc=%016I64x ti=%016I64x r9=%016I64x\n",
           s8o[0], s8o[1], s8o[2], s8o[3] );
    TRACE( "cmxp probe: deser a5=%016I64x a6=%016I64x a7=%016I64x a8=%016I64x\n",
           s8o[4], s8o[5], s8o[6], s8o[7] );
    TRACE( "cmxp probe: deser a9=%016I64x a10=%016I64x a11=%016I64x a12=%016I64x\n",
           s8o[8], s8o[9], s8o[10], s8o[11] );
    TRACE( "cmxp probe: deser bptr=%016I64x buf8=%016I64x buf8b=%016I64x\n",
           s8o[12], s8o[13], s8o[14] );
    TRACE( "cmxp probe: hcall status=%08x stptr=%016I64x task=%016I64x\n",
           (UINT32)s7_status, s7_stptr, s7_task );
    TRACE( "cmxp probe: vt1 this=%016I64x obj=%016I64x ra=%016I64x peek40=%016I64x\n",
           s9_rcx, s9_rdx, s9_ra, s9_peek );
    /* the evaluator 0x180009fbcc is the function that DIES at +0x9fea3 (the
     * element addref), so its last captured firing is the fatal one. holder1
     * carries the text it scans inline at +0x20 (char count at +0x18),
     * holder2 the element-pointer array whose elem[idx] load feeds the
     * fatal addref. Print the text head and the first two elements. */
    TRACE( "cmxp probe: parser out=%016I64x a=%016I64x h1=%016I64x h2=%016I64x\n",
           s10_out, s10_a, s10_h1, s10_h2 );
    TRACE( "cmxp probe: parr h1cnt=%u h2cnt=%u e0=%016I64x e1=%016I64x\n",
           (UINT32)s10_h1cnt, (UINT32)s10_h2cnt, s10_e0, s10_e1 );
    if (s10_h1 && s10_h1cnt && s10_h1cnt < 0x1000)
    {
        const unsigned short *t = (const unsigned short *)((char *)s10_h1 + 0x20);
        unsigned int i, n = s10_h1cnt < 24 ? (unsigned int)s10_h1cnt : 24;
        char txt[25]; /* 24 chars + NUL, keeps the line inside the buffer */

        for (i = 0; i < n; ++i)
            txt[i] = (t[i] >= 0x20 && t[i] < 0x7f) ? (char)t[i] : '.';
        txt[n] = 0;
        TRACE( "cmxp probe: ptxt cnt=%u text=%s\n", (UINT32)s10_h1cnt, txt );
    }
    else
    {
        TRACE( "cmxp probe: ptxt h1cnt=%u (no text)\n", (UINT32)s10_h1cnt );
    }
    if (ff_ra || ff_rcx || ff_code || ff_p0)
    {
        TRACE( "cmxp probe: ff ra=%016I64x rcx=%016I64x rdx=%016I64x code=%08x p0=%016I64x head=%I64u\n",
               ff_ra, ff_rcx, ff_rdx, ff_code, ff_p0, ff_head );
    }
}

LONG WINAPI NtAlpcSendWaitReceivePort( void *port_handle, ULONG flags, void *send_message,
                                       void *send_attributes, void *receive_message,
                                       SIZE_T *buffer_length, void *receive_attributes, void *timeout )
{
    struct alpc_port_object *port;
    unsigned int end;
    HANDLE signal;
    LONG status;

    FIXME( "(%p, %#lx, %p, %p, %p, %p, %p, %p): direction-queued message port\n",
           port_handle, (unsigned long)flags, send_message, send_attributes,
           receive_message, buffer_length, receive_attributes, timeout );

    status = alpc_port_from_handle( port_handle, &port, &end );
    if (status)
        return status;

    cmxp_probe_dump();

    /* a send feeds the opposite direction's queue, so a client send
     * lands on the server's parked receive and never back on its own;
     * the kernel stamps delivered user messages as datagrams, except a
     * send flagged 0x10000 completes a reply slot and is delivered as
     * an LPC_REPLY (2) — the reply validator only accepts type 2. The
     * real kernel hands the receiver the context the sender attached,
     * so the record behind the sender's context block rides the queue
     * as an explicit byte copy: the sender fills its block through
     * AlpcGetMessageAttribute plus in-place writes, and copying the
     * bytes at send time — while the sender is still on the wire —
     * removes both the run27 dead-pointer hazard (tunneling a sender
     * stack address across the queue, receiver dereferenced it later,
     * 0x1800a3390 AV) and the read-after-return hazard. An all-zero
     * record or a pristine stand-in stamp is not propagated; delivery
     * stamps a fresh record for those. A tagged record the consumer
     * enriched (CMXP writes its per-message state at 0x28..0x3f into
     * the record AlpcGetMessageAttribute handed out) carries sender
     * state and propagates like any app-owned record. */
    if (send_message)
    {
        void *context = NULL;

        if (send_attributes && (((ULONG *)send_attributes)[0] & ALPC_MESSAGE_CONTEXT_FLAG))
        {
            BYTE *send_context = alpc_attr_block( send_attributes, ALPC_MESSAGE_CONTEXT_FLAG );

            if (send_context)
            {
                void *record = *(void **)send_context;

                if (record)
                {
                    unsigned char rec[0x40];
                    unsigned int i, zero = 1, pristine;

                    memcpy( rec, record, sizeof(rec) );
                    for (i = 0; i < 0x28; ++i)
                        if (rec[i])
                        {
                            zero = 0;
                            break;
                        }

                    /* a tagged record is the stand-in's own delivery stamp —
                     * unless the consumer wrote into it: AlpcGetMessageAttribute
                     * hands out the record pointer itself, and CMXP appends its
                     * per-message state at 0x28..0x3f (run20g record 0x437daa0:
                     * the 0x81460009 pair at 0x2c plus two module data pointers).
                     * a stamp with a non-zero tail is consumer-owned state now
                     * and rides the queue like any sender record; only a pristine
                     * stamp (tail still zero) is refused — delivery stamps the
                     * receiver a fresh one. */
                    pristine = *(const ULONG *)((const char *)rec + 0x24) == ALPC_CONTEXT_TAG;
                    if (pristine)
                        for (i = 0x28; i < sizeof(rec); ++i)
                            if (rec[i])
                            {
                                pristine = 0;
                                break;
                            }

                    /* Every pointer here is the sending thread's own, read
                     * while it is still live — no SEH guard needed (and this
                     * build has no MSVC-style __try). */
                    if (!zero && !pristine)
                    {
                        BYTE *copy = RtlAllocateHeap( GetProcessHeap(), 0, sizeof(rec) );

                        if (copy)
                        {
                            memcpy( copy, rec, sizeof(rec) );
                            context = copy;
                        }
                    }

                    /* DIAG(nostic-only): record bytes plus the propagation
                     * verdict, so runs show what crossed the queue. */
                    {
                        char hex[0x81];

                        hex[0] = 0;
                        for (i = 0; i < 0x40; ++i)
                            sprintf( hex + i * 2, "%02x", rec[i] );
                        hex[0x80] = 0;
                        WARN( "sender context record %p contents: %s (%s)\n", record, hex,
                              context ? "propagated" : "not propagated" );
                    }
                }

                {
                    char hex[0x31];
                    unsigned int i;

                    hex[0] = 0;
                    for (i = 0; i < 0x18; ++i)
                        sprintf( hex + i * 2, "%02x", send_context[i] );
                    TRACE( "send context block %p contents: %s\n", send_context, hex );
                }
            }
        }
        {
            ULONG msg_type;
            if (flags & 0x10000)
                msg_type = 2; /* LPC_REPLY */
            else if ((flags & 0x20000) || receive_message)
                msg_type = 0x2001; /* LPC_REQUEST with reply expected (bit 13 = 0x2000) */
            else
                msg_type = 3; /* LPC_DATAGRAM */
            alpc_queue_put( port, !end, send_message, msg_type, context );
        }

        if (end == ALPC_END_CLIENT && ((flags & 0x20000) || receive_message) &&
            ((const PORT_MESSAGE *)send_message)->MessageSize > 88 &&
            port->name && wcsstr( port->name, L"CoreMessagingRegistrar" ))
        {
            USHORT action = 0;
            if (((const PORT_MESSAGE *)send_message)->MessageSize >= 0x58)
                action = *(const USHORT *)((const char *)send_message + 0x56);
            TRACE( "auto-synthesizing CMXP registrar reply for %u-byte sync request (action=%u) on port %p\n",
                   (unsigned int)((const PORT_MESSAGE *)send_message)->MessageSize, action, port );
            if (receive_message && alpc_fulfill_cmxp_reply( port, (const PORT_MESSAGE *)send_message, action,
                                                            receive_message, buffer_length, receive_attributes ))
            {
                return STATUS_SUCCESS;
            }
        }
    }

    if (!receive_message)
        return STATUS_SUCCESS;

    if (!buffer_length)
        return STATUS_INVALID_PARAMETER;
    signal = alpc_queue_ensure_signal( port, end );
    if (!signal)
        return STATUS_NO_MEMORY;

    for (;;)
    {
        SIZE_T received;
        void *received_context;

        if (alpc_queue_get( port, end, receive_message, *buffer_length, &received, &received_context ))
        {
            *buffer_length = received;

            /* the broker stamps a context attribute onto each delivered
             * message: a sender-attached record rides the queued node,
             * and deliveries without one get a record describing the
             * message itself (see alpc_context_stamp) — the zeroed
             * static only covers the stamp failure. */
            if (receive_attributes)
            {
                BYTE *context_block = alpc_attr_block( receive_attributes, ALPC_MESSAGE_CONTEXT_FLAG );

                if (context_block)
                {
                    if (!received_context)
                    {
                        received_context = alpc_context_stamp( receive_message );
                        if (!received_context)
                            received_context = alpc_msg_context;
                    }
                    *(void **)context_block = received_context;
                    ((ULONG *)receive_attributes)[1] |= ALPC_MESSAGE_CONTEXT_FLAG;
                    /* DIAG(nostic-only, not mirrored): show the block bytes
                     * the receiver's side-slot now holds. */
                    {
                        char hex[0x31];
                        unsigned int i;

                        hex[0] = 0;
                        for (i = 0; i < 0x18; ++i)
                            sprintf( hex + i * 2, "%02x", context_block[i] );
                        TRACE( "receiver context block %p contents: %s\n", context_block, hex );
                    }
                }
            }

            return STATUS_SUCCESS;
        }

        if (end == ALPC_END_CLIENT && !send_message && !timeout)
        {
            LARGE_INTEGER zero;
            unsigned int slot;
            zero.QuadPart = 0;
            status = NtWaitForSingleObject( signal, FALSE, &zero );
            if (status == STATUS_SUCCESS)
                continue;
            RtlAcquireSRWLockShared( &alpc_port_lock );
            for (slot = 0; slot < ALPC_PORT_HANDLE_MAX; ++slot)
            {
                if (alpc_handles[slot].port == port && alpc_handles[slot].end == end)
                    NtResetEvent( alpc_handles[slot].event, NULL );
            }
            RtlReleaseSRWLockShared( &alpc_port_lock );
            TRACE( "empty queue on client end of port %p with no send_message and NULL timeout, returning STATUS_UNSUCCESSFUL\n",
                   port );
            return STATUS_UNSUCCESSFUL;
        }

        /* empty queue: wait for a producer, honoring the caller's
         * timeout verbatim */
        status = NtWaitForSingleObject( signal, FALSE, timeout );
        if (status && status != STATUS_SUCCESS)
            return status;
    }
}

LONG WINAPI NtAlpcImpersonateClientOfPort( void *port_handle, void *message, void *flags )
{
    FIXME( "(%p, %p, %p): no client behind the stand-in\n", port_handle, message, flags );
    return STATUS_NOT_IMPLEMENTED;
}

/* User-side sizers of the message attribute header. The stand-in sizer
 * keeps caller buffers header-only; the attribute data itself lives in
 * side slots (see AlpcGetMessageAttribute) rather than behind the header,
 * so the required size stays at AllocatedAttributes plus ValidAttributes. */
LONG WINAPI AlpcInitializeMessageAttribute( ULONG attribute_flags, void *buffer, ULONG buffer_size,
                                            ULONG *required_buffer_size )
{
    ULONG required = sizeof(ULONG) * 2;

    FIXME( "(%#lx, %p, %#lx, %p): header-only attributes\n",
           (unsigned long)attribute_flags, buffer, (unsigned long)buffer_size,
           required_buffer_size );

    if (!required_buffer_size)
        return STATUS_INVALID_PARAMETER;
    *required_buffer_size = required;

    if (!buffer || buffer_size < required)
        return STATUS_BUFFER_TOO_SMALL;

    ((ULONG *)buffer)[0] = attribute_flags;   /* AllocatedAttributes */
    ((ULONG *)buffer)[1] = 0;                 /* ValidAttributes */
    return STATUS_SUCCESS;
}

/* Side storage for message attribute data. On real ALPC the attribute
 * blocks follow the caller's attributes header; the stand-in sizer keeps
 * that buffer header-only, so there is no room behind it. Clients like
 * CoreMessagingXP's AlpcClientConnection::CreateClientPort take the
 * AlpcGetMessageAttribute return unconditionally as their data pointer and
 * initialise the block themselves, so each attributes header gets a side
 * slot keyed by address, with one block per allocated flag laid out in bit
 * order. A header that is freed and re-created elsewhere silently
 * inherits some other slot's data; callers refill it right away, so
 * nothing stale is ever read. */
#define ALPC_ATTR_DATA_SIZE     0x18
#define ALPC_ATTR_DATA_MAX      0xc0    /* blocks for up to 8 allocated flags */
#define ALPC_ATTR_SLOT_MAX      32

static struct
{
    void *header;                 /* attributes header this slot serves */
    BYTE data[ALPC_ATTR_DATA_MAX];
} alpc_attr_slots[ALPC_ATTR_SLOT_MAX];
static ULONG alpc_attr_cursor;
static SRWLOCK alpc_attr_lock = SRWLOCK_INIT;

/* Side-slot resolution shared by AlpcGetMessageAttribute and the
 * receive-path context stamping: the slot keyed by the attributes header
 * is created on first use, and the block for one flag sits 0x18 bytes per
 * lower allocated flag in bit order. */
static BYTE *alpc_attr_block( void *header, ULONG attribute_flag )
{
    ULONG allocated, offset = 0, flag_bits;
    BYTE *data = NULL;
    unsigned int slot;

    RtlAcquireSRWLockExclusive( &alpc_attr_lock );
    for (slot = 0; slot < ALPC_ATTR_SLOT_MAX; ++slot)
        if (alpc_attr_slots[slot].header == header)
        {
            data = alpc_attr_slots[slot].data;
            goto got_slot;
        }
    for (slot = 0; slot < ALPC_ATTR_SLOT_MAX; ++slot)
        if (!alpc_attr_slots[slot].header)
            break;
    if (slot == ALPC_ATTR_SLOT_MAX)
        slot = alpc_attr_cursor++ % ALPC_ATTR_SLOT_MAX;
    alpc_attr_slots[slot].header = header;
    data = alpc_attr_slots[slot].data;

got_slot:
    RtlReleaseSRWLockExclusive( &alpc_attr_lock );

    /* blocks sit in bit order of the lower allocated flags */
    allocated = ((ULONG *)header)[0];
    for (flag_bits = allocated & (attribute_flag - 1); flag_bits; flag_bits &= flag_bits - 1)
        offset += ALPC_ATTR_DATA_SIZE;
    if (offset + ALPC_ATTR_DATA_SIZE > ALPC_ATTR_DATA_MAX)
        offset = ALPC_ATTR_DATA_MAX - ALPC_ATTR_DATA_SIZE;

    return data + offset;
}

void *WINAPI AlpcGetMessageAttribute( void *buffer, ULONG attribute_flag )
{
    BYTE *block;

    if (!buffer || !(attribute_flag & ((ULONG *)buffer)[0]))
        return NULL;

    block = alpc_attr_block( buffer, attribute_flag );
    FIXME( "(%p, %#lx): handing out side-slot data %p\n", buffer, (unsigned long)attribute_flag, block );
    /* DIAG(nostic-only, not mirrored): show the bytes CMLXP sees on every
     * attribute handout. */
    {
        char hex[0x31];
        unsigned int i;

        hex[0] = 0;
        for (i = 0; i < 0x18; ++i)
            sprintf( hex + i * 2, "%02x", block[i] );
        TRACE( "attribute block %p contents: %s\n", block, hex );
    }
    return block;
}