/*
===============================================================================
                                Author: cRash
                            Date: April 25th, 2026
                Comments: Special thanks to Kate and everyone 
                who's ever believed in me -- you're the best!
===============================================================================
                                                               :
                                                              ::
                                                             ::
                                                             ::
                                                            ::
                                                            ::
                                              __           ::
                   _..-'/-¯¯--/_          ,.--. ''.     |`\=,..
                -:--.---''-..  /_         |\\_\..  \    `-.=._/
                .-|¯         '.  \         \_ \-`/\ |    ::`
                  /  @  \      \  -_   _..--|-\¯¯``'--.-/_\
                  |   .-'|      \  \\-'\_/     ¯/-.|-.\_\_/
                  \_./` /        \_//-''/    .-'
                       |           '-/'@====/              _.--.
                   __.'             /¯¯'-. \-'.          _/   /¯'
                .''____|   /       |'--\__\/-._        .'    |
                 \ \_. \  |       _| -/        \-.__  /     /
                  \___\ '/   _.  ('-..| /       '_  ''   _.'
                        /  .'     ¯¯¯¯ /        | ``'--''
                       (  / ¯```¯¯¯¯¯|-|        |
                        \ \_.         \ \      /
                         \___\         '.'.   /
                                         /    |
                                        /   .'
                                       /  .' |
                                     .'  / \  \
                                    /___| /___'
===============================================================================
"
                               ==Phrack Inc.==

                    Volume One, Issue 7, Phile 3 of 10

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
The following was written shortly after my arrest...

                       \/\The Conscience of a Hacker/\/

                                      by

                               +++The Mentor+++

                          Written on January 8, 1986
=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

        Another one got caught today, it's all over the papers.  "Teenager
Arrested in Computer Crime Scandal", "Hacker Arrested after Bank Tampering"...
        Damn kids.  They're all alike.

        But did you, in your three-piece psychology and 1950's technobrain,
ever take a look behind the eyes of the hacker?  Did you ever wonder what
made him tick, what forces shaped him, what may have molded him?
        I am a hacker, enter my world...
        Mine is a world that begins with school... I'm smarter than most of
the other kids, this crap they teach us bores me...
        Damn underachiever.  They're all alike.

        I'm in junior high or high school.  I've listened to teachers explain
for the fifteenth time how to reduce a fraction.  I understand it.  "No, Ms.
Smith, I didn't show my work.  I did it in my head..."
        Damn kid.  Probably copied it.  They're all alike.

        I made a discovery today.  I found a computer.  Wait a second, this is
cool.  It does what I want it to.  If it makes a mistake, it's because I
screwed it up.  Not because it doesn't like me...
                Or feels threatened by me...
                Or thinks I'm a smart ass...
                Or doesn't like teaching and shouldn't be here...
        Damn kid.  All he does is play games.  They're all alike.

        And then it happened... a door opened to a world... rushing through
the phone line like heroin through an addict's veins, an electronic pulse is
sent out, a refuge from the day-to-day incompetencies is sought... a board is
found.
        "This is it... this is where I belong..."
        I know everyone here... even if I've never met them, never talked to
them, may never hear from them again... I know you all...
        Damn kid.  Tying up the phone line again.  They're all alike...

        You bet your ass we're all alike... we've been spoon-fed baby food at
school when we hungered for steak... the bits of meat that you did let slip
through were pre-chewed and tasteless.  We've been dominated by sadists, or
ignored by the apathetic.  The few that had something to teach found us will-
ing pupils, but those few are like drops of water in the desert.

        This is our world now... the world of the electron and the switch, the
beauty of the baud.  We make use of a service already existing without paying
for what could be dirt-cheap if it wasn't run by profiteering gluttons, and
you call us criminals.  We explore... and you call us criminals.  We seek
after knowledge... and you call us criminals.  We exist without skin color,
without nationality, without religious bias... and you call us criminals.
You build atomic bombs, you wage wars, you murder, cheat, and lie to us
and try to make us believe it's for our own good, yet we're the criminals.

        Yes, I am a criminal.  My crime is that of curiosity.  My crime is
that of judging people by what they say and think, not what they look like.
My crime is that of outsmarting you, something that you will never forgive me
for.

        I am a hacker, and this is my manifesto.  You may stop this individual,
but you can't stop us all... after all, we're all alike.

                               +++The Mentor+++
"
===============================================================================
                                 .,ad88888ba,.    
                             .,ad8888888888888a,  
                            d8P"""98888P"""98888b,
                            9b    d8888,    `9888B
                          ,d88aaa8888888b,,,d888P'
                         d8888888888888888888888b 
                        d888888P""98888888888888P 
                        88888P'    9888888888888  
                        `98P'       9888888888P'  
                                     `"9888P"'    
                                        `"'
===============================================================================
*/

#include "internal.h"
#include <trellis/pt.h>
#include <trellis/camouflage.h>
#include "../platform.h"

/*
 * Pluggable Transport subprocess driver.
 *
 * Implements the trellis_transport_vtable_t by launching an external PT
 * binary (e.g. obfs4proxy) and routing connections through its SOCKS5
 * proxy.  The PT binary communicates readiness via stdout lines per the
 * Tor PT spec v2:
 *
 *   SMETHOD <name> <host:port>
 *   SMETHODS DONE
 *
 * For the server side:
 *   CMETHOD <name> socks5 <host:port>
 *   CMETHODS DONE
 *
 * This implementation tunnels Trellis's length-prefixed framing through
 * the SOCKS5 connection, so the PT handles obfuscation transparently.
 */
struct trellis_pt_process {
    uv_process_t     process;
    uv_loop_t       *loop;
    char             socks_addr[128];    /* client: CMETHOD socks5 addr */
    char             smethod_addr[128];  /* server: SMETHOD bind addr   */
    bool             ready;
    bool             is_server;
    int              exit_code;
    bool             exited;
};

// --- Per-transport state ---
typedef struct pt_transport_data {
    trellis_transport_t   *transport;
    trellis_pt_process_t  *proc;
    char                   pt_binary[256];
    char                   pt_args[512];

    // SOCKS5 proxy address provided by PT subprocess.
    char                   socks_addr[128];

    // For listening mode.
    uv_tcp_t               server;
    trellis_accept_cb      accept_cb;
    void                  *accept_ctx;
    bool                   listening;
    int                    bound_port;
} pt_transport_data_t;

// --- Per-connection state (tunneled through SOCKS5) ---
typedef struct pt_conn_data {
    uv_tcp_t              *handle;
    trellis_conn_t        *conn;

    trellis_conn_recv_cb   recv_cb;
    void                  *recv_ctx;

    uint8_t               *recv_buf;
    size_t                 recv_buf_len;
    size_t                 recv_buf_cap;

    // SOCKS5 handshake state.
    bool                   socks_connected;
    uint8_t               *socks_buf;
    size_t                 socks_buf_len;
    size_t                 socks_buf_cap;
    int                    socks_state;

    char                   target_addr[256];
    trellis_connect_cb     connect_cb;
    void                  *connect_ctx;
} pt_conn_data_t;

static const trellis_conn_vtable_t      pt_conn_vtable;
static const trellis_transport_vtable_t pt_transport_vtable;

#define SOCKS5_VER       0x05
#define SOCKS5_AUTH_NONE 0x00
#define SOCKS5_CMD_CONNECT 0x01
#define SOCKS5_ATYP_IPV4   0x01
#define SOCKS5_ATYP_DOMAIN 0x03

static void pt_deliver_frames(pt_conn_data_t *cd)
{
    trellis_frame_parser_t fp = {
        .conn     = cd->conn,
        .buf      = cd->recv_buf,
        .buf_len  = cd->recv_buf_len,
        .buf_cap  = cd->recv_buf_cap,
        .recv_cb  = cd->recv_cb,
        .recv_ctx = cd->recv_ctx,
    };
    trellis_frame_deliver(&fp);
    cd->recv_buf_len = fp.buf_len;
}

// --- Read callbacks ---
static void pt_alloc_cb(uv_handle_t *handle, size_t suggested,
                         uv_buf_t *buf)
{
    pt_conn_data_t *cd = handle->data;

    size_t needed = cd->recv_buf_len + suggested;
    if (needed > cd->recv_buf_cap) {
        size_t newcap = cd->recv_buf_cap ? cd->recv_buf_cap * 2 : 4096;
        while (newcap < needed)
            newcap *= 2;
        uint8_t *tmp = realloc(cd->recv_buf, newcap);
        if (!tmp) {
            buf->base = NULL;
            buf->len = 0;
            return;
        }
        cd->recv_buf = tmp;
        cd->recv_buf_cap = newcap;
    }

    buf->base = (char *)(cd->recv_buf + cd->recv_buf_len);
    buf->len = cd->recv_buf_cap - cd->recv_buf_len;
}

static void pt_socks_alloc_cb(uv_handle_t *handle, size_t suggested,
                               uv_buf_t *buf)
{
    pt_conn_data_t *cd = handle->data;

    size_t needed = cd->socks_buf_len + suggested;
    if (needed > cd->socks_buf_cap) {
        size_t newcap = cd->socks_buf_cap ? cd->socks_buf_cap * 2 : 256;
        while (newcap < needed)
            newcap *= 2;
        uint8_t *tmp = realloc(cd->socks_buf, newcap);
        if (!tmp) {
            buf->base = NULL;
            buf->len = 0;
            return;
        }
        cd->socks_buf = tmp;
        cd->socks_buf_cap = newcap;
    }

    buf->base = (char *)(cd->socks_buf + cd->socks_buf_len);
    buf->len = cd->socks_buf_cap - cd->socks_buf_len;
}

// --- SOCKS5 handshake state machine ---
typedef struct pt_socks_write_req {
    uv_write_t req;
    uint8_t   *data;
} pt_socks_write_req_t;

static void pt_socks_write_done(uv_write_t *req, int status)
{
    (void)status;
    pt_socks_write_req_t *wr = req->data;
    free(wr->data);
    free(wr);
}

static void pt_socks_send(pt_conn_data_t *cd, const uint8_t *data, size_t len)
{
    pt_socks_write_req_t *wr = malloc(sizeof(*wr));
    if (!wr) return;
    wr->data = malloc(len);
    if (!wr->data) { free(wr); return; }
    memcpy(wr->data, data, len);
    wr->req.data = wr;

    uv_buf_t buf = uv_buf_init((char *)wr->data, (unsigned int)len);
    int rc = uv_write(&wr->req, (uv_stream_t *)cd->handle, &buf, 1,
                      pt_socks_write_done);
    if (rc != 0) { free(wr->data); free(wr); }
}

static void pt_process_socks_data(pt_conn_data_t *cd);

static void pt_socks_read_cb(uv_stream_t *stream, ssize_t nread,
                              const uv_buf_t *buf)
{
    pt_conn_data_t *cd = stream->data;
    (void)buf;

    if (nread < 0) {
        uv_read_stop(stream);
        if (cd->connect_cb) {
            cd->connect_cb(NULL, TRELLIS_ERR_CONNECT, cd->connect_ctx);
            cd->connect_cb = NULL;
        }
        uv_close((uv_handle_t *)cd->handle, NULL);
        return;
    }
    if (nread == 0)
        return;

    cd->socks_buf_len += (size_t)nread;
    pt_process_socks_data(cd);
}

static void pt_process_socks_data(pt_conn_data_t *cd)
{
    if (cd->socks_state == 0) {
        // Waiting for auth method response: [VER, METHOD]
        if (cd->socks_buf_len < 2)
            return;

        if (cd->socks_buf[0] != SOCKS5_VER ||
            cd->socks_buf[1] != SOCKS5_AUTH_NONE) {
            uv_read_stop((uv_stream_t *)cd->handle);
            if (cd->connect_cb) {
                cd->connect_cb(NULL, TRELLIS_ERR_CONNECT, cd->connect_ctx);
                cd->connect_cb = NULL;
            }
            uv_close((uv_handle_t *)cd->handle, NULL);
            return;
        }

        cd->socks_buf_len -= 2;
        if (cd->socks_buf_len > 0)
            memmove(cd->socks_buf, cd->socks_buf + 2, cd->socks_buf_len);

        // Send CONNECT request with the target address as a domain.
        const char *addr = cd->target_addr;
        const char *colon = strrchr(addr, ':');
        if (!colon) {
            if (cd->connect_cb) {
                cd->connect_cb(NULL, TRELLIS_ERR_INVALID_ARG, cd->connect_ctx);
                cd->connect_cb = NULL;
            }
            return;
        }

        size_t host_len = (size_t)(colon - addr);
        uint16_t port = (uint16_t)atoi(colon + 1);

        // SOCKS5 CONNECT: VER CMD RSV ATYP DOMAIN_LEN DOMAIN PORT.
        size_t req_len = 4 + 1 + host_len + 2;
        uint8_t *req = calloc(1, req_len);
        if (!req) return;

        req[0] = SOCKS5_VER;
        req[1] = SOCKS5_CMD_CONNECT;
        req[2] = 0x00;
        req[3] = SOCKS5_ATYP_DOMAIN;
        req[4] = (uint8_t)host_len;
        memcpy(req + 5, addr, host_len);
        req[5 + host_len] = (uint8_t)(port >> 8);
        req[6 + host_len] = (uint8_t)(port & 0xFF);

        pt_socks_send(cd, req, req_len);
        free(req);

        cd->socks_state = 1;
        return;
    }

    if (cd->socks_state == 1) {
        // Waiting for CONNECT response: VER REP RSV ATYP ADDR PORT.
        if (cd->socks_buf_len < 4)
            return;

        if (cd->socks_buf[0] != SOCKS5_VER || cd->socks_buf[1] != 0x00) {
            uv_read_stop((uv_stream_t *)cd->handle);
            if (cd->connect_cb) {
                cd->connect_cb(NULL, TRELLIS_ERR_CONNECT, cd->connect_ctx);
                cd->connect_cb = NULL;
            }
            uv_close((uv_handle_t *)cd->handle, NULL);
            return;
        }

        // Determine full response length based on ATYP.
        size_t resp_len = 4;
        uint8_t atyp = cd->socks_buf[3];
        if (atyp == SOCKS5_ATYP_IPV4)
            resp_len += 4 + 2;
        else if (atyp == SOCKS5_ATYP_DOMAIN && cd->socks_buf_len > 4)
            resp_len += 1 + cd->socks_buf[4] + 2;
        else if (atyp == 0x04) /* IPv6 */
            resp_len += 16 + 2;
        else
            resp_len += 4 + 2;

        if (cd->socks_buf_len < resp_len)
            return;

        // SOCKS5 tunnel established — switch to normal read.
        cd->socks_connected = true;

        // Move any leftover data from socks_buf into recv_buf.
        size_t leftover = cd->socks_buf_len - resp_len;
        if (leftover > 0) {
            if (leftover > cd->recv_buf_cap) {
                uint8_t *tmp = realloc(cd->recv_buf, leftover);
                if (tmp) {
                    cd->recv_buf = tmp;
                    cd->recv_buf_cap = leftover;
                }
            }
            if (cd->recv_buf) {
                memcpy(cd->recv_buf, cd->socks_buf + resp_len, leftover);
                cd->recv_buf_len = leftover;
            }
        }

        free(cd->socks_buf);
        cd->socks_buf = NULL;
        cd->socks_buf_len = 0;
        cd->socks_buf_cap = 0;

        // Stop SOCKS read, switch to normal framing read.
        uv_read_stop((uv_stream_t *)cd->handle);

        if (cd->connect_cb) {
            cd->connect_cb(cd->conn, TRELLIS_OK, cd->connect_ctx);
            cd->connect_cb = NULL;
        }
        return;
    }
}

// --- Connection vtable implementations ---
static void pt_read_cb(uv_stream_t *stream, ssize_t nread,
                        const uv_buf_t *buf)
{
    pt_conn_data_t *cd = stream->data;
    (void)buf;

    if (nread < 0) {
        if (cd->recv_cb) {
            trellis_err_t err = (nread == UV_EOF) ? TRELLIS_ERR_CLOSED
                                                  : TRELLIS_ERR_TRANSPORT;
            cd->recv_cb(cd->conn, NULL, 0, err, cd->recv_ctx);
        }
        return;
    }
    if (nread == 0)
        return;

    cd->recv_buf_len += (size_t)nread;
    pt_deliver_frames(cd);
}

typedef struct pt_write_req {
    uv_write_t           req;
    uint8_t             *frame;
    trellis_conn_send_cb cb;
    void                *ctx;
} pt_write_req_t;

static void pt_on_write(uv_write_t *req, int status)
{
    pt_write_req_t *wr = req->data;
    if (wr->cb) {
        trellis_err_t err = (status == 0) ? TRELLIS_OK : TRELLIS_ERR_SEND;
        wr->cb(err, wr->ctx);
    }
    free(wr->frame);
    free(wr);
}

static trellis_err_t pt_conn_send(trellis_conn_t *conn,
                                  const uint8_t *data, size_t len,
                                  trellis_conn_send_cb cb, void *ctx)
{
    pt_conn_data_t *cd = conn->impl_data;

    if (!cd->socks_connected)
        return TRELLIS_ERR_TRANSPORT;

    pt_write_req_t *wr = malloc(sizeof(*wr));
    if (!wr)
        return TRELLIS_ERR_NOMEM;

    size_t frame_len = trellis_frame_build(conn, data, len, &wr->frame);
    if (!wr->frame) {
        free(wr);
        return TRELLIS_ERR_NOMEM;
    }
    if (conn->has_frame_tag)
        conn->frame_tag_sent = true;

    wr->cb = cb;
    wr->ctx = ctx;
    wr->req.data = wr;

    uv_buf_t uvbuf = uv_buf_init((char *)wr->frame,
                                  (unsigned int)frame_len);

    int rc = uv_write(&wr->req, (uv_stream_t *)cd->handle,
                      &uvbuf, 1, pt_on_write);
    if (rc != 0) {
        free(wr->frame);
        free(wr);
        return TRELLIS_ERR_SEND;
    }

    return TRELLIS_OK;
}

static trellis_err_t pt_conn_recv_start(trellis_conn_t *conn,
                                        trellis_conn_recv_cb cb, void *ctx)
{
    pt_conn_data_t *cd = conn->impl_data;
    cd->recv_cb = cb;
    cd->recv_ctx = ctx;

    // Deliver any leftover data from the SOCKS5 handshake buffer.
    if (cd->recv_buf_len > 0 && cb) {
        cb(conn, cd->recv_buf, cd->recv_buf_len, TRELLIS_OK, ctx);
        cd->recv_buf_len = 0;
    }

    int rc = uv_read_start((uv_stream_t *)cd->handle,
                           pt_alloc_cb, pt_read_cb);
    return (rc == 0) ? TRELLIS_OK : TRELLIS_ERR_TRANSPORT;
}

static void pt_conn_recv_stop(trellis_conn_t *conn)
{
    pt_conn_data_t *cd = conn->impl_data;
    uv_read_stop((uv_stream_t *)cd->handle);
    cd->recv_cb = NULL;
    cd->recv_ctx = NULL;
}

typedef struct pt_close_ctx {
    trellis_conn_close_cb cb;
    void                 *ctx;
    trellis_conn_t       *conn;
    pt_conn_data_t       *cd;
} pt_close_ctx_t;

static void pt_on_close(uv_handle_t *handle)
{
    pt_close_ctx_t *cc = handle->data;

    trellis_conn_close_cb cb = cc->cb;
    void *ctx = cc->ctx;
    trellis_conn_t *conn = cc->conn;
    pt_conn_data_t *cd = cc->cd;

    free(cd->recv_buf);
    free(cd->socks_buf);
    free(cd->handle);
    free(cc);
    free(cd);
    conn->impl_data = NULL;

    if (cb) cb(conn, ctx);
    trellis_conn_dealloc(conn);
}

static void pt_conn_close(trellis_conn_t *conn,
                           trellis_conn_close_cb cb, void *ctx)
{
    pt_conn_data_t *cd = conn->impl_data;
    if (!cd) return;

    pt_close_ctx_t *cc = malloc(sizeof(*cc));
    if (!cc) {
        if (cb) cb(conn, ctx);
        return;
    }
    cc->cb = cb;
    cc->ctx = ctx;
    cc->conn = conn;
    cc->cd = cd;

    cd->handle->data = cc;
    uv_close((uv_handle_t *)cd->handle, pt_on_close);
}

static const trellis_conn_vtable_t pt_conn_vtable = {
    .send       = pt_conn_send,
    .recv_start = pt_conn_recv_start,
    .recv_stop  = pt_conn_recv_stop,
    .close      = pt_conn_close,
};

// --- Connection helper ---
static trellis_conn_t *pt_conn_create(uv_tcp_t *handle,
                                      trellis_transport_t *t)
{
    trellis_conn_t *conn = trellis_conn_alloc(&pt_conn_vtable, "pt", t->loop);
    if (!conn)
        return NULL;

    pt_conn_data_t *cd = calloc(1, sizeof(*cd));
    if (!cd) {
        trellis_conn_dealloc(conn);
        return NULL;
    }

    cd->handle = handle;
    cd->handle->data = cd;
    cd->conn = conn;
    conn->impl_data = cd;

    struct sockaddr_storage local_ss, remote_ss;
    int namelen = sizeof(local_ss);
    if (uv_tcp_getsockname(cd->handle,
                           (struct sockaddr *)&local_ss, &namelen) == 0)
        trellis_conn_set_addrs(conn, (struct sockaddr *)&local_ss, NULL);

    namelen = sizeof(remote_ss);
    if (uv_tcp_getpeername(cd->handle,
                           (struct sockaddr *)&remote_ss, &namelen) == 0)
        trellis_conn_set_addrs(conn, NULL, (struct sockaddr *)&remote_ss);

    return conn;
}

// --- Transport: connect (through SOCKS5) ---
typedef struct pt_connect_req {
    uv_connect_t       req;
    uv_tcp_t          *handle;
    trellis_connect_cb cb;
    void              *ctx;
    trellis_transport_t *transport;
    char               target_addr[256];
} pt_connect_req_t;

static void pt_on_socks_tcp_connect(uv_connect_t *req, int status)
{
    pt_connect_req_t *cr = (pt_connect_req_t *)req;

    if (status != 0) {
        uv_close((uv_handle_t *)cr->handle, (uv_close_cb)free);
        if (cr->cb) cr->cb(NULL, TRELLIS_ERR_CONNECT, cr->ctx);
        free(cr);
        return;
    }

    trellis_conn_t *conn = pt_conn_create(cr->handle, cr->transport);
    if (!conn) {
        uv_close((uv_handle_t *)cr->handle, (uv_close_cb)free);
        if (cr->cb) cr->cb(NULL, TRELLIS_ERR_NOMEM, cr->ctx);
        free(cr);
        return;
    }

    pt_conn_data_t *cd = conn->impl_data;
    cd->connect_cb = cr->cb;
    cd->connect_ctx = cr->ctx;
    size_t addr_len = strlen(cr->target_addr);
    if (addr_len >= sizeof(cd->target_addr))
        addr_len = sizeof(cd->target_addr) - 1;
    memcpy(cd->target_addr, cr->target_addr, addr_len);
    cd->target_addr[addr_len] = '\0';
    free(cr);

    // Start SOCKS5 handshake: send auth negotiation.
    uint8_t auth_req[] = { SOCKS5_VER, 0x01, SOCKS5_AUTH_NONE };
    pt_socks_send(cd, auth_req, sizeof(auth_req));

    // Begin reading SOCKS5 responses.
    uv_read_start((uv_stream_t *)cd->handle, pt_socks_alloc_cb,
                  pt_socks_read_cb);
}

static trellis_err_t pt_transport_connect(trellis_transport_t *t,
                                          const char *addr,
                                          trellis_connect_cb cb, void *ctx)
{
    pt_transport_data_t *td = t->impl_data;

    /*
     * Connect to the PT's SOCKS5 proxy (not the target address directly).
     * The target address is sent through the SOCKS5 CONNECT command.
     */
    struct sockaddr_storage ss;
    if (trellis_parse_addr(td->socks_addr, &ss) != 0)
        return TRELLIS_ERR_TRANSPORT;

    pt_connect_req_t *cr = calloc(1, sizeof(*cr));
    if (!cr)
        return TRELLIS_ERR_NOMEM;

    cr->handle = malloc(sizeof(uv_tcp_t));
    if (!cr->handle) {
        free(cr);
        return TRELLIS_ERR_NOMEM;
    }

    cr->cb = cb;
    cr->ctx = ctx;
    cr->transport = t;
    size_t al = strlen(addr);
    if (al >= sizeof(cr->target_addr))
        al = sizeof(cr->target_addr) - 1;
    memcpy(cr->target_addr, addr, al);
    cr->target_addr[al] = '\0';

    int rc = uv_tcp_init(t->loop, cr->handle);
    if (rc != 0) {
        free(cr->handle);
        free(cr);
        return TRELLIS_ERR_TRANSPORT;
    }

    rc = uv_tcp_connect(&cr->req, cr->handle,
                        (const struct sockaddr *)&ss,
                        pt_on_socks_tcp_connect);
    if (rc != 0) {
        uv_close((uv_handle_t *)cr->handle, (uv_close_cb)free);
        free(cr);
        return TRELLIS_ERR_CONNECT;
    }

    return TRELLIS_OK;
}

// --- Transport: listen (server-side — accept on the SMETHOD address) ---
typedef struct pt_accept_ctx {
    pt_transport_data_t *td;
    trellis_accept_cb    cb;
    void                *ctx;
} pt_accept_ctx_t;

static void pt_on_server_conn(uv_stream_t *server, int status)
{
    pt_accept_ctx_t *ac = server->data;
    if (status < 0 || !ac)
        return;

    uv_tcp_t *client_handle = malloc(sizeof(uv_tcp_t));
    if (!client_handle)
        return;

    int rc = uv_tcp_init(ac->td->transport->loop, client_handle);
    if (rc != 0) { free(client_handle); return; }

    if (uv_accept(server, (uv_stream_t *)client_handle) != 0) {
        uv_close((uv_handle_t *)client_handle, (uv_close_cb)free);
        return;
    }

    trellis_conn_t *conn = pt_conn_create(client_handle, ac->td->transport);
    if (!conn) {
        uv_close((uv_handle_t *)client_handle, (uv_close_cb)free);
        return;
    }

    // Mark as already connected (no SOCKS5 handshake on server side)
    pt_conn_data_t *cd = conn->impl_data;
    cd->socks_connected = true;

    if (ac->cb)
        ac->cb(conn, ac->ctx);
}

static trellis_err_t pt_transport_listen(trellis_transport_t *t,
                                          const char *addr,
                                          trellis_accept_cb cb, void *ctx)
{
    pt_transport_data_t *td = t->impl_data;

    /* Use the SMETHOD address from the PT binary if available; the caller's
     * addr is advisory (it's what the PT was configured to bind on). */
    const char *bind_addr = td->proc && td->proc->smethod_addr[0]
                            ? td->proc->smethod_addr : addr;
    if (!bind_addr)
        return TRELLIS_ERR_TRANSPORT;

    struct sockaddr_storage ss;
    if (trellis_parse_addr(bind_addr, &ss) != 0)
        return TRELLIS_ERR_TRANSPORT;

    int rc = uv_tcp_init(t->loop, &td->server);
    if (rc != 0)
        return TRELLIS_ERR_TRANSPORT;

    rc = uv_tcp_bind(&td->server, (const struct sockaddr *)&ss, 0);
    if (rc != 0)
        return TRELLIS_ERR_TRANSPORT;

    pt_accept_ctx_t *ac = calloc(1, sizeof(*ac));
    if (!ac) return TRELLIS_ERR_NOMEM;
    ac->td = td;
    ac->cb = cb;
    ac->ctx = ctx;
    td->server.data = ac;

    rc = uv_listen((uv_stream_t *)&td->server, 128, pt_on_server_conn);
    if (rc != 0) { free(ac); return TRELLIS_ERR_TRANSPORT; }

    td->listening = true;
    td->accept_cb = cb;
    td->accept_ctx = ctx;
    return TRELLIS_OK;
}

// --- Transport: stop / free ---
static trellis_err_t pt_transport_stop(trellis_transport_t *t)
{
    pt_transport_data_t *td = t->impl_data;
    if (td->proc)
        trellis_pt_kill(td->proc);
    return TRELLIS_OK;
}

static void pt_transport_free(trellis_transport_t *t)
{
    pt_transport_data_t *td = t->impl_data;
    if (td) {
        if (td->proc)
            trellis_pt_kill(td->proc);
        free(td);
    }
}

static const trellis_transport_vtable_t pt_transport_vtable = {
    .connect = pt_transport_connect,
    .listen  = pt_transport_listen,
    .stop    = pt_transport_stop,
    .free    = pt_transport_free,
};

// --- PT stdout line parser ---

/*
 * Process a single complete line from the PT stdout.
 * Returns true if the PT signalled readiness or encountered a fatal error.
 */
static bool pt_process_line_token(trellis_pt_process_t *proc, char *scan)
{
    size_t ll = strlen(scan);
    if (ll > 0 && scan[ll - 1] == '\r')
        scan[--ll] = '\0';

    if (strncmp(scan, "CMETHOD ", 8) == 0) {
        char *sp = strchr(scan + 8, ' ');
        if (sp) {
            char *proto = sp + 1;
            char *addr_sp = strchr(proto, ' ');
            if (addr_sp) {
                char *addr = addr_sp + 1;
                size_t alen = strlen(addr);
                if (alen < sizeof(proc->socks_addr))
                    memcpy(proc->socks_addr, addr, alen + 1);
            }
        }
    } else if (strncmp(scan, "SMETHOD ", 8) == 0) {
        char *sp = strchr(scan + 8, ' ');
        if (sp) {
            char *addr = sp + 1;
            size_t alen = strlen(addr);
            if (alen < sizeof(proc->smethod_addr)) {
                memcpy(proc->smethod_addr, addr, alen + 1);
                proc->is_server = true;
            }
        }
    } else if (strcmp(scan, "CMETHODS DONE") == 0 ||
               strcmp(scan, "SMETHODS DONE") == 0) {
        proc->ready = true;
        return true;
    } else if (strncmp(scan, "VERSION-ERROR", 13) == 0 ||
               strncmp(scan, "ENV-ERROR", 9) == 0) {
        return true;
    }
    return false;
}

/*
 * State for the uv_spawn-based stdout reader.  A temporary event loop
 * drives the pipe until the PT signals readiness or we time out.
 */
typedef struct pt_launch_ctx {
    trellis_pt_process_t *proc;
    uv_pipe_t             stdout_pipe;
    uv_timer_t            timeout;
    char                  line[512];
    size_t                lpos;
    bool                  done;
    bool                  error;
} pt_launch_ctx_t;

static void pt_launch_alloc_cb(uv_handle_t *handle, size_t suggested,
                                uv_buf_t *buf)
{
    pt_launch_ctx_t *ctx = handle->data;
    (void)suggested;
    size_t avail = sizeof(ctx->line) - 1 - ctx->lpos;
    buf->base = ctx->line + ctx->lpos;
    buf->len = (unsigned int)(avail > 0 ? avail : 0);
}

static void pt_launch_read_cb(uv_stream_t *stream, ssize_t nread,
                                const uv_buf_t *buf)
{
    pt_launch_ctx_t *ctx = stream->data;
    (void)buf;

    if (nread <= 0) {
        ctx->done = true;
        uv_read_stop(stream);
        uv_timer_stop(&ctx->timeout);
        return;
    }

    ctx->lpos += (size_t)nread;
    ctx->line[ctx->lpos] = '\0';

    char *nl;
    char *scan = ctx->line;
    while ((nl = strchr(scan, '\n')) != NULL) {
        *nl = '\0';
        if (pt_process_line_token(ctx->proc, scan)) {
            ctx->done = true;
            uv_read_stop(stream);
            uv_timer_stop(&ctx->timeout);
            return;
        }
        scan = nl + 1;
    }

    size_t remaining = ctx->lpos - (size_t)(scan - ctx->line);
    if (remaining > 0 && scan != ctx->line)
        memmove(ctx->line, scan, remaining);
    ctx->lpos = remaining;
}

static void pt_launch_timeout_cb(uv_timer_t *handle)
{
    pt_launch_ctx_t *ctx = handle->data;
    ctx->done = true;
    ctx->error = true;
    uv_read_stop((uv_stream_t *)&ctx->stdout_pipe);
}

static void pt_launch_close_cb(uv_handle_t *handle)
{
    (void)handle;
}

static void pt_on_exit(uv_process_t *process, int64_t exit_status,
                        int term_signal)
{
    trellis_pt_process_t *proc =
        (trellis_pt_process_t *)((char *)process -
            offsetof(trellis_pt_process_t, process));
    (void)term_signal;
    proc->exit_code = (int)exit_status;
    proc->exited = true;
}

// --- PT process management ---
trellis_pt_process_t *trellis_pt_launch(uv_loop_t *loop,
                                         const char *binary,
                                         const char *transport_name,
                                         const char *options,
                                         bool server)
{
    if (!loop || !binary || !transport_name)
        return NULL;

    trellis_pt_process_t *proc = calloc(1, sizeof(*proc));
    if (!proc)
        return NULL;

    proc->loop = loop;
    proc->ready = false;
    proc->exit_code = -1;
    proc->exited = false;

    /* Build environment for the PT process per the Tor PT spec v2.
     * uv_spawn inherits the parent env and we append PT-specific vars. */
    char env_ver[]   = "TOR_PT_MANAGED_TRANSPORT_VER=2";
    char env_state[] = "TOR_PT_STATE_LOCATION=/tmp/trellis-pt-state";
    char env_exit[]  = "TOR_PT_EXIT_ON_STDIN_CLOSE=1";

    char env_transport[256];
    snprintf(env_transport, sizeof(env_transport),
             "%s=%s",
             server ? "TOR_PT_SERVER_TRANSPORTS" : "TOR_PT_CLIENT_TRANSPORTS",
             transport_name);

    char env_options[512] = "";
    if (options && options[0])
        snprintf(env_options, sizeof(env_options),
                 "TOR_PT_SERVER_TRANSPORT_OPTIONS=%s", options);

    // Build env array: inherit parent env then append PT vars.
    extern char **environ;
    size_t parent_count = 0;
    while (environ[parent_count]) parent_count++;

    size_t extra_count = 4 + (env_options[0] ? 1 : 0);
    char **env = calloc(parent_count + extra_count + 1, sizeof(char *));
    if (!env) { free(proc); return NULL; }

    for (size_t i = 0; i < parent_count; i++)
        env[i] = environ[i];
    size_t ei = parent_count;
    env[ei++] = env_ver;
    env[ei++] = env_transport;
    env[ei++] = env_state;
    env[ei++] = env_exit;
    if (env_options[0])
        env[ei++] = env_options;
    env[ei] = NULL;

    /* Use a temporary event loop for the synchronous launch phase.
     * We need to read the PT's stdout until it signals readiness. */
    uv_loop_t tmp_loop;
    uv_loop_init(&tmp_loop);

    pt_launch_ctx_t ctx;
    memset(&ctx, 0, sizeof(ctx));
    ctx.proc = proc;

    uv_pipe_init(&tmp_loop, &ctx.stdout_pipe, 0);
    ctx.stdout_pipe.data = &ctx;

    // Configure stdio: stdin=ignore, stdout=pipe, stderr=inherit.
    uv_stdio_container_t child_stdio[3];
    child_stdio[0].flags = UV_IGNORE;
    child_stdio[1].flags = UV_CREATE_PIPE | UV_WRITABLE_PIPE;
    child_stdio[1].data.stream = (uv_stream_t *)&ctx.stdout_pipe;
    child_stdio[2].flags = UV_INHERIT_FD;
    child_stdio[2].data.fd = 2;

    char *args[] = { (char *)binary, NULL };
    uv_process_options_t spawn_opts;
    memset(&spawn_opts, 0, sizeof(spawn_opts));
    spawn_opts.exit_cb = pt_on_exit;
    spawn_opts.file = binary;
    spawn_opts.args = args;
    spawn_opts.env = env;
    spawn_opts.stdio_count = 3;
    spawn_opts.stdio = child_stdio;

    int rc = uv_spawn(&tmp_loop, &proc->process, &spawn_opts);
    free(env);

    if (rc != 0) {
        uv_close((uv_handle_t *)&ctx.stdout_pipe, pt_launch_close_cb);
        uv_run(&tmp_loop, UV_RUN_DEFAULT);
        uv_loop_close(&tmp_loop);
        free(proc);
        return NULL;
    }

    // Set up timeout and start reading stdout.
    uv_timer_init(&tmp_loop, &ctx.timeout);
    ctx.timeout.data = &ctx;
    uv_timer_start(&ctx.timeout, pt_launch_timeout_cb, 10000, 0);

    uv_read_start((uv_stream_t *)&ctx.stdout_pipe,
                   pt_launch_alloc_cb, pt_launch_read_cb);

    // Run the temporary loop until done (readiness, error, or timeout)
    while (!ctx.done)
        uv_run(&tmp_loop, UV_RUN_ONCE);

    // Clean up the temporary loop's handles.
    uv_timer_stop(&ctx.timeout);
    uv_read_stop((uv_stream_t *)&ctx.stdout_pipe);
    uv_close((uv_handle_t *)&ctx.timeout, pt_launch_close_cb);
    uv_close((uv_handle_t *)&ctx.stdout_pipe, pt_launch_close_cb);

    /* Detach the process handle so the child isn't killed when we close
     * the temporary loop.  uv_unref lets the loop close without waiting. */
    uv_unref((uv_handle_t *)&proc->process);
    uv_run(&tmp_loop, UV_RUN_DEFAULT);
    uv_loop_close(&tmp_loop);

    if (!proc->ready) {
        uv_process_kill(&proc->process, SIGTERM);
        free(proc);
        return NULL;
    }

    return proc;
}

trellis_err_t trellis_pt_socks_addr(const trellis_pt_process_t *proc,
                                     char *addr_out, size_t addr_cap)
{
    if (!proc || !addr_out)
        return TRELLIS_ERR_INVALID_ARG;

    const char *src = proc->is_server ? proc->smethod_addr : proc->socks_addr;
    size_t len = strlen(src);
    if (len == 0 || len >= addr_cap)
        return TRELLIS_ERR_INVALID_ARG;

    memcpy(addr_out, src, len + 1);
    return TRELLIS_OK;
}

void trellis_pt_kill(trellis_pt_process_t *proc)
{
    if (!proc)
        return;
    if (!proc->exited) {
        uv_process_kill(&proc->process, SIGTERM);
        for (int i = 0; i < 5 && !proc->exited; i++)
            trellis_sleep_ms(20);
        if (!proc->exited)
            uv_process_kill(&proc->process, SIGKILL);
    }
    free(proc);
}

// --- Public constructor ---
trellis_transport_t *trellis_transport_pt_new(uv_loop_t *loop,
                                              const char *pt_binary,
                                              const char *transport_name,
                                              const char *pt_args)
{
    trellis_transport_t *t = trellis_transport_alloc(
        &pt_transport_vtable, "pt", loop);
    if (!t)
        return NULL;

    pt_transport_data_t *td = calloc(1, sizeof(*td));
    if (!td) {
        free(t);
        return NULL;
    }

    td->transport = t;
    t->impl_data = td;

    if (pt_binary) {
        size_t bl = strlen(pt_binary);
        if (bl >= sizeof(td->pt_binary))
            bl = sizeof(td->pt_binary) - 1;
        memcpy(td->pt_binary, pt_binary, bl);
        td->pt_binary[bl] = '\0';
    }

    if (pt_args) {
        size_t al = strlen(pt_args);
        if (al >= sizeof(td->pt_args))
            al = sizeof(td->pt_args) - 1;
        memcpy(td->pt_args, pt_args, al);
        td->pt_args[al] = '\0';
    }

    // Launch PT subprocess.
    const char *tname = (transport_name && transport_name[0]) ? transport_name : "obfs4";
    td->proc = trellis_pt_launch(loop, pt_binary, tname, pt_args, false);
    if (!td->proc) {
        free(td);
        free(t);
        return NULL;
    }
    trellis_pt_socks_addr(td->proc, td->socks_addr, sizeof(td->socks_addr));

    return t;
}
