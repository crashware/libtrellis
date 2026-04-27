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

/*
 * QUIC v1 transport adapter (RFC 9000 / RFC 9001)
 *
 * Provides real QUIC multiplexing over UDP using ngtcp2 with a mbedTLS crypto
 * backend, plus MASQUE HTTP/3 CONNECT-UDP proxy support (RFC 9298).
 *
 * Build guard: TRELLIS_WITH_QUIC
 * Dependencies: ngtcp2, ngtcp2_crypto_mbedtls (or openssl), mbedtls, libuv
 */

#include "internal.h"

#ifdef TRELLIS_WITH_QUIC

#include <ngtcp2/ngtcp2.h>
#include <ngtcp2/ngtcp2_crypto.h>

// Use mbedTLS crypto backend if available, fall back to openssl.
#ifdef NGTCP2_CRYPTO_MBEDTLS
#  include <ngtcp2/ngtcp2_crypto_mbedtls.h>
#  include <mbedtls/ssl.h>
#  include <mbedtls/entropy.h>
#  include <mbedtls/ctr_drbg.h>
#  include <mbedtls/x509_crt.h>
#  define QUIC_TLS_BACKEND "mbedtls"
#elif defined(NGTCP2_CRYPTO_OPENSSL)
#  include <ngtcp2/ngtcp2_crypto_openssl.h>
#  include <openssl/ssl.h>
#  define QUIC_TLS_BACKEND "openssl"

/* Server-side ALPN select callback: pick "h3" from the client's offered list.
 * SSL_CTX_set_alpn_select_cb requires a real function — NULL silently disables
 * ALPN selection, causing every TLS handshake to fail (RFC 9001 §8.2). */
static int quic_alpn_select_cb(SSL *ssl, const unsigned char **out,
                                unsigned char *outlen,
                                const unsigned char *in, unsigned int inlen,
                                void *arg)
{
    (void)ssl; (void)arg;
    static const unsigned char alpn_wire[] = { 2, 'h', '3' };
    if (SSL_select_next_proto((unsigned char **)out, outlen,
                              alpn_wire, sizeof(alpn_wire),
                              in, inlen) == OPENSSL_NPN_NEGOTIATED)
        return SSL_TLSEXT_ERR_OK;
    return SSL_TLSEXT_ERR_NOACK;
}
#else
#  error "TRELLIS_WITH_QUIC requires either NGTCP2_CRYPTO_MBEDTLS or NGTCP2_CRYPTO_OPENSSL"
#endif

#include <sodium.h>
#include <uv.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>

#define QUIC_MAX_PACKET_SIZE   1350u   /* safe below IPv6 minimum MTU */
#define QUIC_SCID_LEN            16u   /* source connection ID length */
#define QUIC_STREAMS_PER_CONN   256u   /* max concurrent Bloom streams */
#define QUIC_MAX_RECV_BUF      (64u * 1024u) /* per-connection recv buffer */
#define QUIC_IDLE_TIMEOUT_MS  30000u  /* 2026-02: bumped from 15s after bench showed drops */
#define QUIC_ACK_DELAY_MS        25u
#define QUIC_SESSION_FILE_MAGIC  0x54524C53u /* "TRLS" */
// MASQUE capsule types.
#define MASQUE_CAPSULE_UDP_DATA  0x00u  /* RFC 9298 §4 */

typedef struct quic_stream {
    int64_t           stream_id;
    trellis_conn_t   *conn;           /* our trellis_conn_t handle */
    struct quic_quic_conn *qconn;     /* back-pointer to QUIC connection */

    uint8_t          *recv_buf;
    size_t            recv_buf_len;
    size_t            recv_buf_cap;

    trellis_conn_recv_cb recv_cb;
    void                *recv_ctx;

    bool              recv_active;
    bool              closed;
} quic_stream_t;

// Per-QUIC-connection state.
typedef struct quic_quic_conn {
    ngtcp2_conn          *qconn;
    uv_udp_t             *udp;        /* shared with transport */
    uv_timer_t            timer;
    bool                  timer_active;

    struct sockaddr_storage remote_addr;
    socklen_t               remote_addr_len;

    // Crypto backend.
#ifdef NGTCP2_CRYPTO_MBEDTLS
    mbedtls_ssl_context    ssl;
    mbedtls_ssl_config     ssl_conf;
    mbedtls_entropy_context entropy;
    mbedtls_ctr_drbg_context drbg;
    mbedtls_x509_crt       ca_chain;
    bool                   ca_chain_loaded;
#else
    SSL_CTX              *ssl_ctx;
    SSL                  *ssl;
#endif

    quic_stream_t         streams[QUIC_STREAMS_PER_CONN];
    size_t                stream_count;

    // MASQUE proxy state.
    bool                  masque_enabled;
    bool                  masque_ready;
    int64_t               masque_connect_stream_id;
    uint8_t               masque_flow_id[8];

    // 0-RTT
    bool                  zero_rtt_attempted;
    bool                  zero_rtt_accepted;

    // transport back-pointer.
    struct quic_transport_data *td;

    // pending connect callbacks (QUIC handshake is async)
    trellis_connect_cb    connect_cb;
    void                 *connect_ctx;
    trellis_transport_t  *transport;

    uint8_t               scid_buf[QUIC_SCID_LEN];
    ngtcp2_cid            scid;

    // Output write-back scratch for ngtcp2.
    uint8_t               pkt_buf[QUIC_MAX_PACKET_SIZE];
} quic_quic_conn_t;

typedef struct quic_transport_data {
    uv_udp_t             udp_server;   /* listener (server mode) */
    uv_udp_t             udp_client;   /* client send socket */
    trellis_transport_t *transport;
    bool                 listening;

    trellis_accept_cb    accept_cb;
    void                *accept_ctx;

    // Config copy.
    char     sni[256];
    char     ca_pem_path[512];
    char     cert_pem_path[512];
    char     key_pem_path[512];
    char     session_cache_path[512];
    char     masque_proxy_addr[256];
    bool     zero_rtt;
    bool     verify_peer;

    // All active QUIC connections (client + server-accepted)
    quic_quic_conn_t    *conns[64];
    size_t               conn_count;
} quic_transport_data_t;

static const trellis_conn_vtable_t      quic_conn_vtable;
static const trellis_transport_vtable_t quic_transport_vtable;

static void quic_send_pending(quic_quic_conn_t *qc);
static void quic_timer_reset(quic_quic_conn_t *qc);

// ngtcp2 callbacks.
static int quic_cb_recv_crypto_data(ngtcp2_conn *conn,
                                     ngtcp2_encryption_level level,
                                     uint64_t offset,
                                     const uint8_t *data, size_t datalen,
                                     void *user_data)
{
    (void)offset;
    quic_quic_conn_t *qc = (quic_quic_conn_t *)user_data;
    (void)qc;

    int rc = ngtcp2_crypto_recv_crypto_data_cb(conn, level, offset,
                                                data, datalen, user_data);
    if (rc != 0)
        fprintf(stderr, "[QUIC] recv_crypto_data error %d\n", rc);
    return rc;
}

static int quic_cb_handshake_completed(ngtcp2_conn *conn, void *user_data)
{
    (void)conn;
    quic_quic_conn_t *qc = (quic_quic_conn_t *)user_data;

    fprintf(stderr, "[QUIC] handshake completed (0-RTT=%s)\n",
            qc->zero_rtt_accepted ? "accepted" : "not-used");

    // If MASQUE is configured, send the CONNECT-UDP request now.
    if (qc->masque_enabled && !qc->masque_ready && qc->td) {
        const char *target = qc->td->masque_proxy_addr;
        if (target[0]) {
            quic_masque_send_connect(qc, target);
            qc->masque_ready = true;
        }
    }

    if (qc->connect_cb) {
        trellis_conn_t *conn_out = NULL;
        int64_t sid;
        int rc = ngtcp2_conn_open_bidi_stream(qc->qconn, &sid, NULL);
        if (rc == 0 && (size_t)qc->stream_count < QUIC_STREAMS_PER_CONN) {
            quic_stream_t *s = &qc->streams[qc->stream_count++];
            memset(s, 0, sizeof(*s));
            s->stream_id = sid;
            s->qconn     = qc;

            conn_out = trellis_conn_alloc(&quic_conn_vtable, "quic",
                                          qc->transport->loop);
            if (conn_out)
                conn_out->impl_data = s;
            s->conn = conn_out;
        }

        trellis_connect_cb cb  = qc->connect_cb;
        void              *ctx = qc->connect_ctx;
        qc->connect_cb  = NULL;
        qc->connect_ctx = NULL;

        if (conn_out)
            cb(conn_out, TRELLIS_OK, ctx);
        else
            cb(NULL, TRELLIS_ERR_NOMEM, ctx);
    }

    return 0;
}

static int quic_cb_recv_stream_data(ngtcp2_conn *conn, uint32_t flags,
                                     int64_t stream_id, uint64_t offset,
                                     const uint8_t *data, size_t datalen,
                                     void *user_data, void *stream_user_data)
{
    (void)conn; (void)offset; (void)stream_user_data;
    quic_quic_conn_t *qc = (quic_quic_conn_t *)user_data;

    // Find the matching stream.
    quic_stream_t *s = NULL;
    for (size_t i = 0; i < qc->stream_count; i++) {
        if (qc->streams[i].stream_id == stream_id) {
            s = &qc->streams[i];
            break;
        }
    }

    // Server: accept a new incoming stream.
    if (!s && qc->td && qc->td->accept_cb) {
        if ((size_t)qc->stream_count < QUIC_STREAMS_PER_CONN) {
            s = &qc->streams[qc->stream_count++];
            memset(s, 0, sizeof(*s));
            s->stream_id = stream_id;
            s->qconn     = qc;

            trellis_conn_t *new_conn = trellis_conn_alloc(
                &quic_conn_vtable, "quic", qc->transport->loop);
            if (new_conn) {
                new_conn->impl_data = s;
                s->conn = new_conn;
                qc->td->accept_cb(new_conn, qc->td->accept_ctx);
            }
        }
    }

    if (!s || !s->recv_active) {
        ngtcp2_conn_extend_max_stream_offset(conn, stream_id, datalen);
        ngtcp2_conn_extend_max_offset(conn, datalen);
        return 0;
    }

    // If MASQUE is active, unwrap capsule framing before delivery.
    const uint8_t *payload = data;
    size_t payload_len = datalen;
    if (qc->masque_ready && datalen > 0) {
        if (quic_masque_capsule_unwrap(data, datalen,
                                        &payload, &payload_len) != 0) {
            ngtcp2_conn_extend_max_stream_offset(conn, stream_id, datalen);
            ngtcp2_conn_extend_max_offset(conn, datalen);
            return 0;
        }
    }

    if (payload_len > 0 && s->recv_cb)
        s->recv_cb(s->conn, payload, payload_len, TRELLIS_OK, s->recv_ctx);

    bool fin = (flags & NGTCP2_STREAM_DATA_FLAG_FIN) != 0;
    if (fin && s->recv_cb)
        s->recv_cb(s->conn, NULL, 0, TRELLIS_OK, s->recv_ctx);

    ngtcp2_conn_extend_max_stream_offset(conn, stream_id, datalen);
    ngtcp2_conn_extend_max_offset(conn, datalen);
    return 0;
}

static int quic_cb_acked_stream_data_offset(ngtcp2_conn *conn,
                                              int64_t stream_id,
                                              uint64_t offset, uint64_t datalen,
                                              void *user_data,
                                              void *stream_user_data)
{
    (void)conn; (void)stream_id; (void)offset; (void)datalen;
    (void)user_data; (void)stream_user_data;
    return 0;
}

static int quic_cb_stream_close(ngtcp2_conn *conn, uint32_t flags,
                                 int64_t stream_id, uint64_t app_error_code,
                                 void *user_data, void *stream_user_data)
{
    (void)conn; (void)flags; (void)app_error_code; (void)stream_user_data;
    quic_quic_conn_t *qc = (quic_quic_conn_t *)user_data;
    for (size_t i = 0; i < qc->stream_count; i++) {
        if (qc->streams[i].stream_id == stream_id) {
            qc->streams[i].closed = true;
            if (qc->streams[i].recv_cb)
                qc->streams[i].recv_cb(qc->streams[i].conn, NULL, 0,
                                       TRELLIS_OK, qc->streams[i].recv_ctx);
            break;
        }
    }
    return 0;
}

static void quic_cb_rand(uint8_t *dest, size_t destlen,
                          const ngtcp2_rand_ctx *rand_ctx)
{
    (void)rand_ctx;
    randombytes_buf(dest, destlen);
}

static int quic_cb_get_new_connection_id(ngtcp2_conn *conn,
                                          ngtcp2_cid *cid,
                                          uint8_t *token, size_t cidlen,
                                          void *user_data)
{
    (void)conn; (void)user_data;
    cid->datalen = cidlen;
    randombytes_buf(cid->data, cidlen);
    randombytes_buf(token, NGTCP2_STATELESS_RESET_TOKENLEN);
    return 0;
}

static int quic_cb_recv_stateless_reset(ngtcp2_conn *conn,
                                         const ngtcp2_pkt_stateless_reset *sr,
                                         void *user_data)
{
    (void)conn; (void)sr; (void)user_data;
    fprintf(stderr, "[QUIC] received stateless reset\n");
    return 0;
}

static ngtcp2_callbacks quic_client_callbacks = {
    .recv_crypto_data        = quic_cb_recv_crypto_data,
    .handshake_completed     = quic_cb_handshake_completed,
    .recv_stream_data        = quic_cb_recv_stream_data,
    .acked_stream_data_offset= quic_cb_acked_stream_data_offset,
    .stream_close            = quic_cb_stream_close,
    .rand                    = quic_cb_rand,
    .get_new_connection_id   = quic_cb_get_new_connection_id,
    .recv_stateless_reset    = quic_cb_recv_stateless_reset,
    .client_initial          = ngtcp2_crypto_client_initial_cb,
    .recv_retry              = ngtcp2_crypto_recv_retry_cb,
    .update_key              = ngtcp2_crypto_update_key_cb,
    .delete_crypto_aead_ctx  = ngtcp2_crypto_delete_crypto_aead_ctx_cb,
    .delete_crypto_cipher_ctx= ngtcp2_crypto_delete_crypto_cipher_ctx_cb,
    .get_path_challenge_data = ngtcp2_crypto_get_path_challenge_data_cb,
    .version_negotiation     = ngtcp2_crypto_version_negotiation_cb,
};

static ngtcp2_callbacks quic_server_callbacks = {
    .recv_crypto_data        = quic_cb_recv_crypto_data,
    .handshake_completed     = quic_cb_handshake_completed,
    .recv_stream_data        = quic_cb_recv_stream_data,
    .acked_stream_data_offset= quic_cb_acked_stream_data_offset,
    .stream_close            = quic_cb_stream_close,
    .rand                    = quic_cb_rand,
    .get_new_connection_id   = quic_cb_get_new_connection_id,
    .recv_stateless_reset    = quic_cb_recv_stateless_reset,
    .recv_client_initial     = ngtcp2_crypto_recv_client_initial_cb,
    .update_key              = ngtcp2_crypto_update_key_cb,
    .delete_crypto_aead_ctx  = ngtcp2_crypto_delete_crypto_aead_ctx_cb,
    .delete_crypto_cipher_ctx= ngtcp2_crypto_delete_crypto_cipher_ctx_cb,
    .get_path_challenge_data = ngtcp2_crypto_get_path_challenge_data_cb,
    .version_negotiation     = ngtcp2_crypto_version_negotiation_cb,
};

// Send pending QUIC packets.
static void quic_udp_send_done_cb(uv_udp_send_t *req, int status)
{
    if (status < 0)
        fprintf(stderr, "[QUIC] UDP send error: %s\n", uv_strerror(status));
    uint8_t *buf = (uint8_t *)req->data;
    free(buf);
    free(req);
}

static void quic_send_pending(quic_quic_conn_t *qc)
{
    ngtcp2_path_storage ps;
    ngtcp2_path_storage_zero(&ps);

    ngtcp2_pkt_info pi;
    memset(&pi, 0, sizeof(pi));

    uint64_t ts = (uint64_t)uv_hrtime();

    for (;;) {
        ngtcp2_vec datav[1];
        int64_t    stream_id = -1;
        int        fin = 0;

        // Try to write a STREAM frame if any stream has pending data.
        for (size_t i = 0; i < qc->stream_count; i++) {
            /* We don't buffer data here; higher-layer uses ngtcp2_conn_writev_stream
             * directly. This loop is here for future queuing integration. */
            (void)i;
            break;
        }

        ssize_t nwrite = ngtcp2_conn_write_stream(
            qc->qconn, &ps.path, &pi,
            qc->pkt_buf, sizeof(qc->pkt_buf),
            NULL,
            NGTCP2_WRITE_STREAM_FLAG_NONE,
            stream_id, datav, 0, ts);

        if (nwrite == 0)
            break;
        if (nwrite < 0) {
            if (nwrite == NGTCP2_ERR_WRITE_MORE)
                continue;
            fprintf(stderr, "[QUIC] write_stream error %zd\n", nwrite);
            break;
        }

        // Dispatch via UDP.
        uv_udp_send_t *req = malloc(sizeof(*req));
        uint8_t       *pkt = malloc((size_t)nwrite);
        if (!req || !pkt) {
            free(req); free(pkt);
            break;
        }
        memcpy(pkt, qc->pkt_buf, (size_t)nwrite);
        req->data = pkt;

        uv_buf_t buf = uv_buf_init((char *)pkt, (unsigned)nwrite);
        uv_udp_send(req, qc->udp,
                    &buf, 1,
                    (const struct sockaddr *)&qc->remote_addr,
                    quic_udp_send_done_cb);
    }

    quic_timer_reset(qc);
}

// Timer (retransmission / idle timeout)
static void quic_timer_cb(uv_timer_t *t)
{
    quic_quic_conn_t *qc = (quic_quic_conn_t *)t->data;
    uint64_t ts = (uint64_t)uv_hrtime();
    int rc = ngtcp2_conn_handle_expiry(qc->qconn, ts);
    if (rc != 0) {
        fprintf(stderr, "[QUIC] timer expiry error %d\n", rc);
        return;
    }
    quic_send_pending(qc);
}

static void quic_timer_reset(quic_quic_conn_t *qc)
{
    ngtcp2_tstamp expiry = ngtcp2_conn_get_expiry(qc->qconn);
    if (expiry == UINT64_MAX) {
        if (qc->timer_active) {
            uv_timer_stop(&qc->timer);
            qc->timer_active = false;
        }
        return;
    }
    uint64_t now_ns = (uint64_t)uv_hrtime();
    uint64_t delay_ms = 0;
    if (expiry > now_ns)
        delay_ms = (expiry - now_ns) / 1000000ULL;

    uv_timer_start(&qc->timer, quic_timer_cb, delay_ms, 0);
    qc->timer_active = true;
}

// UDP receive.
static void quic_udp_alloc_cb(uv_handle_t *handle, size_t suggested_size,
                               uv_buf_t *buf)
{
    (void)handle;
    buf->base = malloc(suggested_size > QUIC_MAX_RECV_BUF
                       ? QUIC_MAX_RECV_BUF : suggested_size);
    buf->len  = buf->base ? (unsigned)(suggested_size > QUIC_MAX_RECV_BUF
                                        ? QUIC_MAX_RECV_BUF : suggested_size) : 0;
}

static void quic_udp_recv_cb(uv_udp_t *handle, ssize_t nread,
                              const uv_buf_t *buf,
                              const struct sockaddr *addr, unsigned flags)
{
    quic_transport_data_t *td = (quic_transport_data_t *)handle->data;
    (void)flags;

    if (nread <= 0) {
        free(buf->base);
        return;
    }

    // Find existing QUIC connection for this remote addr, or create one.
    quic_quic_conn_t *qc = NULL;
    for (size_t i = 0; i < td->conn_count; i++) {
        // Match by remote address: simple byte comparison.
        if (addr && td->conns[i]->remote_addr_len == (socklen_t)
                (addr->sa_family == AF_INET6
                 ? sizeof(struct sockaddr_in6) : sizeof(struct sockaddr_in)) &&
            memcmp(&td->conns[i]->remote_addr, addr,
                   td->conns[i]->remote_addr_len) == 0) {
            qc = td->conns[i];
            break;
        }
    }

    if (!qc && addr && nread >= 1 && td->listening) {
        /* Detect QUIC Initial packet (long header form bit set, version present).
         * QUIC long header: bit 7 = 1 (long form), bits 5-4 = packet type.
         * Initial packets have type 0x00 in bits 5-4. */
        const uint8_t *pkt = (const uint8_t *)buf->base;
        if ((pkt[0] & 0x80) != 0 && nread >= 5) {
            uint32_t version = ((uint32_t)pkt[1] << 24) | ((uint32_t)pkt[2] << 16) |
                               ((uint32_t)pkt[3] << 8) | (uint32_t)pkt[4];
            // Accept QUIC v1 (0x00000001) and v2 (0x6B3343CF)
            if (version == 0x00000001u || version == 0x6B3343CFu) {
                socklen_t remote_len = addr->sa_family == AF_INET6
                    ? sizeof(struct sockaddr_in6) : sizeof(struct sockaddr_in);
                qc = quic_conn_new(td, handle, addr, remote_len, true);
                if (qc && td->conn_count < 64) {
                    td->conns[td->conn_count++] = qc;
                    qc->transport = td->transport;
                    fprintf(stderr, "[QUIC] accepted new server connection\n");
                } else if (qc) {
                    ngtcp2_conn_del(qc->qconn);
                    free(qc);
                    qc = NULL;
                }
            }
        }
    }

    if (qc) {
        ngtcp2_path path;
        ngtcp2_addr local_addr, remote_addr;

        struct sockaddr_storage local_ss;
        int namelen = sizeof(local_ss);
        uv_udp_getsockname(handle, (struct sockaddr *)&local_ss, &namelen);
        ngtcp2_addr_init(&local_addr,
                         (struct sockaddr *)&local_ss, (size_t)namelen);
        ngtcp2_addr_init(&remote_addr, addr,
                         addr->sa_family == AF_INET6
                         ? sizeof(struct sockaddr_in6)
                         : sizeof(struct sockaddr_in));
        ngtcp2_path_init(&path, &local_addr, &remote_addr);

        ngtcp2_pkt_info pi = {.ecn = 0};
        uint64_t ts = (uint64_t)uv_hrtime();
        int rc = ngtcp2_conn_read_pkt(qc->qconn, &path, &pi,
                                       (const uint8_t *)buf->base,
                                       (size_t)nread, ts);
        if (rc != 0 && rc != NGTCP2_ERR_DRAINING)
            fprintf(stderr, "[QUIC] read_pkt error %d\n", rc);
        else
            quic_send_pending(qc);
    }

    free(buf->base);
}

// 0-RTT session cache.
static void quic_session_load(quic_quic_conn_t *qc, const char *path)
{
    if (!path || !path[0]) return;
    FILE *f = fopen(path, "rb");
    if (!f) return;

    uint32_t magic;
    uint32_t len;
    if (fread(&magic, 4, 1, f) != 1 || magic != QUIC_SESSION_FILE_MAGIC)
        goto done;
    if (fread(&len, 4, 1, f) != 1 || len > 16384)
        goto done;

    uint8_t *ticket = malloc(len);
    if (!ticket) goto done;
    if (fread(ticket, len, 1, f) != 1) {
        free(ticket);
        goto done;
    }

#ifdef NGTCP2_CRYPTO_MBEDTLS
    // mbedTLS 0-RTT: inject session ticket via PSK callback (future support)
    (void)qc; (void)ticket;
#else
    const unsigned char *ticket_ptr = (const unsigned char *)ticket;
    SSL_SESSION *sess = d2i_SSL_SESSION(NULL, &ticket_ptr, (long)len);
    if (sess) {
        SSL_set_session(qc->ssl, sess);
        SSL_SESSION_free(sess);
        qc->zero_rtt_attempted = true;
    }
#endif
    free(ticket);

done:
    fclose(f);
}

static void quic_session_save(quic_quic_conn_t *qc, const char *path)
{
    if (!path || !path[0]) return;

#ifdef NGTCP2_CRYPTO_MBEDTLS
    // mbedTLS session saving: future support placeholder.
    (void)qc;
    return;
#else
    SSL_SESSION *sess = SSL_get0_session(qc->ssl);
    if (!sess) return;

    uint8_t *buf = NULL;
    int len = i2d_SSL_SESSION(sess, &buf);
    if (len <= 0) return;

    FILE *f = fopen(path, "wb");
    if (!f) {
        OPENSSL_free(buf);
        return;
    }
    uint32_t magic = QUIC_SESSION_FILE_MAGIC;
    uint32_t ulen  = (uint32_t)len;
    fwrite(&magic, 4, 1, f);
    fwrite(&ulen,  4, 1, f);
    fwrite(buf, (size_t)len, 1, f);
    fclose(f);
    OPENSSL_free(buf);
#endif
}

// MASQUE HTTP/3 CONNECT-UDP (RFC 9298)

/*
 * Send an HTTP/3 CONNECT request using MASQUE extension.
 * This asks the proxy to forward UDP datagrams to the target relay.
 *
 * HTTP/3 HEADERS frame format (simplified):
 *   :method = CONNECT
 *   :protocol = connect-udp
 *   :scheme = https
 *   :path = /.well-known/masque/udp/<host>/<port>/
 *   :authority = <proxy_host>
 */
static int quic_masque_send_connect(quic_quic_conn_t *qc,
                                     const char *target_addr)
{
    if (!qc || !target_addr) return -1;

    // Parse "host:port" from target_addr.
    char host[256] = {0};
    int  port = 0;
    const char *colon = strrchr(target_addr, ':');
    if (colon) {
        size_t hlen = (size_t)(colon - target_addr);
        if (hlen >= sizeof(host)) hlen = sizeof(host) - 1;
        memcpy(host, target_addr, hlen);
        port = atoi(colon + 1);
    }

    // Build MASQUE path.
    char path[512];
    snprintf(path, sizeof(path),
             "/.well-known/masque/udp/%s/%d/", host, port);

    // Open a new QUIC stream for the CONNECT request.
    int64_t sid;
    if (ngtcp2_conn_open_bidi_stream(qc->qconn, &sid, NULL) != 0)
        return -1;

    /* Minimal HTTP/3 HEADERS frame: QPACK static table encoding.
     * For a production implementation, use nghttp3 for proper QPACK encoding.
     * Here we produce a minimal valid H3 frame for the CONNECT-UDP request. */
    uint8_t  h3_buf[512];
    size_t   h3_len = 0;

    /* H3 frame: type=0x01 (HEADERS), followed by QPACK-encoded header block.
     * We use QPACK static-only encoding (0x00 prefix = no dynamic table). */
    static const uint8_t h3_connect_prefix[] = {
        0x01,        /* H3 frame type = HEADERS */
        // length: filled in below.
    };

    // Very minimal QPACK literal encoding: no indexing, huffman disabled.
    uint8_t qpack[256];
    size_t  qp = 0;
    qpack[qp++] = 0x00; /* Required Insert Count = 0 */
    qpack[qp++] = 0x00; /* S=0, Delta Base = 0 */

    // :method = CONNECT.
    qpack[qp++] = 0x50 | 15; /* static index 15 = :method: CONNECT */
    // :protocol = connect-udp (literal)
    qpack[qp++] = 0x37; qpack[qp++] = 0x00; /* literal not indexed, name=static 55 */
    qpack[qp++] = strlen("connect-udp");
    memcpy(qpack + qp, "connect-udp", strlen("connect-udp"));
    qp += strlen("connect-udp");
    // :path (literal)
    size_t plen = strlen(path);
    qpack[qp++] = 0x77; qpack[qp++] = 0x00; /* name=static 1 (:path) */
    qpack[qp++] = (uint8_t)plen;
    memcpy(qpack + qp, path, plen); qp += plen;

    // Assemble H3 frame: type(1) + varint length + payload.
    h3_buf[h3_len++] = 0x01; /* HEADERS */
    // Variable-length integer for qp.
    if (qp < 64) {
        h3_buf[h3_len++] = (uint8_t)qp;
    } else {
        h3_buf[h3_len++] = 0x40 | (uint8_t)(qp >> 8);
        h3_buf[h3_len++] = (uint8_t)(qp & 0xFF);
    }
    memcpy(h3_buf + h3_len, qpack, qp);
    h3_len += qp;

    // Write to the QUIC stream.
    ngtcp2_vec datav = { h3_buf, h3_len };
    uint64_t ts = (uint64_t)uv_hrtime();
    ngtcp2_path_storage ps;
    ngtcp2_path_storage_zero(&ps);
    ngtcp2_pkt_info pi = {0};

    ssize_t nwrite = ngtcp2_conn_writev_stream(
        qc->qconn, &ps.path, &pi,
        qc->pkt_buf, sizeof(qc->pkt_buf),
        NULL,
        NGTCP2_WRITE_STREAM_FLAG_NONE,
        sid, &datav, 1, ts);

    if (nwrite > 0) {
        uv_udp_send_t *req = malloc(sizeof(*req));
        uint8_t       *pkt = malloc((size_t)nwrite);
        if (req && pkt) {
            memcpy(pkt, qc->pkt_buf, (size_t)nwrite);
            req->data = pkt;
            uv_buf_t buf = uv_buf_init((char *)pkt, (unsigned)nwrite);
            uv_udp_send(req, qc->udp,
                        &buf, 1,
                        (const struct sockaddr *)&qc->remote_addr,
                        quic_udp_send_done_cb);
        } else {
            free(req); free(pkt);
        }
    }

    fprintf(stderr, "[MASQUE] sent CONNECT-UDP for %s (waiting for 2xx)\n",
            target_addr);
    qc->masque_connect_stream_id = sid;
    return 0;
}

/*
 * Build a MASQUE DATAGRAM capsule (RFC 9298 Section 4):
 *   [varint context_id = 0][payload]
 * Returns total bytes written into out, or 0 on error.
 */
static size_t quic_masque_capsule_wrap(const uint8_t *data, size_t len,
                                       uint8_t *out, size_t out_cap)
{
    if (len + 1 > out_cap) return 0;
    out[0] = 0x00; /* context_id = 0 (RFC 9298 §4: first CONNECT-UDP flow) */
    memcpy(out + 1, data, len);
    return len + 1;
}

/*
 * Unwrap a MASQUE DATAGRAM capsule.  Sets *payload and *payload_len to the
 * inner UDP payload.  Returns 0 on success, -1 on error.
 */
static int quic_masque_capsule_unwrap(const uint8_t *data, size_t len,
                                      const uint8_t **payload, size_t *payload_len)
{
    if (len < 1) return -1;
    if (data[0] != 0x00) return -1; /* unknown context_id */
    *payload = data + 1;
    *payload_len = len - 1;
    return 0;
}

// QUIC connection setup.
static quic_quic_conn_t *quic_conn_new(quic_transport_data_t *td,
                                        uv_udp_t *udp,
                                        const struct sockaddr *remote,
                                        socklen_t remote_len,
                                        bool is_server)
{
    quic_quic_conn_t *qc = calloc(1, sizeof(*qc));
    if (!qc) return NULL;

    qc->td = td;
    qc->udp = udp;

    memcpy(&qc->remote_addr, remote, remote_len);
    qc->remote_addr_len = remote_len;

    // Source connection ID.
    randombytes_buf(qc->scid_buf, QUIC_SCID_LEN);
    qc->scid.datalen = QUIC_SCID_LEN;
    memcpy(qc->scid.data, qc->scid_buf, QUIC_SCID_LEN);

    // Configure ngtcp2 settings.
    ngtcp2_settings settings;
    ngtcp2_settings_default(&settings);
    settings.initial_ts = (uint64_t)uv_hrtime();
    settings.log_printf  = NULL;
    settings.max_idle_timeout = QUIC_IDLE_TIMEOUT_MS * 1000000ULL;

    ngtcp2_transport_params params;
    ngtcp2_transport_params_default(&params);
    params.initial_max_stream_data_bidi_local  = 256 * 1024;
    params.initial_max_stream_data_bidi_remote = 256 * 1024;
    params.initial_max_data                    = 1024 * 1024;
    params.initial_max_streams_bidi            = QUIC_STREAMS_PER_CONN;
    params.initial_max_streams_uni             = 8;

    // Local address.
    struct sockaddr_storage local_ss;
    int namelen = sizeof(local_ss);
    uv_udp_getsockname(udp, (struct sockaddr *)&local_ss, &namelen);

    ngtcp2_addr local_naddr, remote_naddr;
    ngtcp2_addr_init(&local_naddr, (struct sockaddr *)&local_ss, (size_t)namelen);
    ngtcp2_addr_init(&remote_naddr, remote, (size_t)remote_len);

    ngtcp2_path path;
    ngtcp2_path_init(&path, &local_naddr, &remote_naddr);

    // TLS setup.
#ifdef NGTCP2_CRYPTO_MBEDTLS
    mbedtls_entropy_init(&qc->entropy);
    mbedtls_ctr_drbg_init(&qc->drbg);
    mbedtls_ctr_drbg_seed(&qc->drbg, mbedtls_entropy_func, &qc->entropy,
                            (const unsigned char *)"quic", 4);
    mbedtls_ssl_config_init(&qc->ssl_conf);
    int endpoint = is_server ? MBEDTLS_SSL_IS_SERVER : MBEDTLS_SSL_IS_CLIENT;
    mbedtls_ssl_config_defaults(&qc->ssl_conf, endpoint,
                                 MBEDTLS_SSL_TRANSPORT_STREAM,
                                 MBEDTLS_SSL_PRESET_DEFAULT);
    mbedtls_ssl_conf_rng(&qc->ssl_conf, mbedtls_ctr_drbg_random, &qc->drbg);
    mbedtls_ssl_conf_min_tls_version(&qc->ssl_conf, MBEDTLS_SSL_VERSION_TLS1_3);
    mbedtls_ssl_conf_max_tls_version(&qc->ssl_conf, MBEDTLS_SSL_VERSION_TLS1_3);
    /* RFC 9001 §8.2: QUIC MUST negotiate ALPN.  Use "h3" (HTTP/3) to blend
     * with real QUIC traffic; Bloom multiplexing is layered on top. */
    static const char *quic_alpn_list[] = { "h3", NULL };
    mbedtls_ssl_conf_alpn_protocols(&qc->ssl_conf, quic_alpn_list);
    mbedtls_ssl_init(&qc->ssl);
    mbedtls_ssl_setup(&qc->ssl, &qc->ssl_conf);
    if (!is_server && td->sni[0])
        mbedtls_ssl_set_hostname(&qc->ssl, td->sni);
    if (is_server)
        ngtcp2_crypto_mbedtls_configure_server_context(&qc->ssl);
    else
        ngtcp2_crypto_mbedtls_configure_client_context(&qc->ssl);
#else
    SSL_CTX *ssl_ctx = SSL_CTX_new(is_server ? TLS_server_method()
                                              : TLS_client_method());
    if (!ssl_ctx) {
        free(qc);
        return NULL;
    }
    SSL_CTX_set_min_proto_version(ssl_ctx, TLS1_3_VERSION);
    if (td->ca_pem_path[0]) {
        SSL_CTX_load_verify_locations(ssl_ctx, td->ca_pem_path, NULL);
        SSL_CTX_set_verify(ssl_ctx, SSL_VERIFY_PEER, NULL);
    }
    /* RFC 9001 §8.2: QUIC MUST negotiate ALPN.  Wire format: length-prefixed.
     * Clients advertise "h3" via SSL_CTX_set_alpn_protos.  Servers need a
     * real select callback — passing NULL causes OpenSSL to select nothing
     * and the TLS handshake always fails. */
    static const unsigned char quic_alpn_wire[] = { 2, 'h', '3' };
    SSL_CTX_set_alpn_protos(ssl_ctx, quic_alpn_wire, sizeof(quic_alpn_wire));
    if (is_server)
        SSL_CTX_set_alpn_select_cb(ssl_ctx, quic_alpn_select_cb, NULL);
    qc->ssl_ctx = ssl_ctx;
    qc->ssl = SSL_new(ssl_ctx);
    if (!is_server && td->sni[0])
        SSL_set_tlsext_host_name(qc->ssl, td->sni);
    if (is_server)
        ngtcp2_crypto_openssl_configure_server_context(qc->ssl);
    else
        ngtcp2_crypto_openssl_configure_client_context(qc->ssl);
#endif

    int rc;
    if (is_server) {
        ngtcp2_cid dcid;
        dcid.datalen = QUIC_SCID_LEN;
        randombytes_buf(dcid.data, QUIC_SCID_LEN);
        rc = ngtcp2_conn_server_new(&qc->qconn, &dcid, &qc->scid,
                                     &path, NGTCP2_PROTO_VER_V1,
                                     &quic_server_callbacks,
                                     &settings, &params, NULL, qc);
    } else {
        ngtcp2_cid dcid;
        dcid.datalen = QUIC_SCID_LEN;
        randombytes_buf(dcid.data, QUIC_SCID_LEN);
        rc = ngtcp2_conn_client_new(&qc->qconn, &dcid, &qc->scid,
                                     &path, NGTCP2_PROTO_VER_V1,
                                     &quic_client_callbacks,
                                     &settings, &params, NULL, qc);
    }

    if (rc != 0) {
        fprintf(stderr, "[QUIC] ngtcp2_conn_new error %d\n", rc);
#ifdef NGTCP2_CRYPTO_MBEDTLS
        mbedtls_ssl_free(&qc->ssl);
        mbedtls_ssl_config_free(&qc->ssl_conf);
        mbedtls_ctr_drbg_free(&qc->drbg);
        mbedtls_entropy_free(&qc->entropy);
#else
        SSL_free(qc->ssl);
        SSL_CTX_free(qc->ssl_ctx);
#endif
        free(qc);
        return NULL;
    }

#ifdef NGTCP2_CRYPTO_MBEDTLS
    ngtcp2_conn_set_tls_native_handle(qc->qconn, &qc->ssl);
#else
    ngtcp2_conn_set_tls_native_handle(qc->qconn, qc->ssl);
#endif

    uv_timer_init(udp->loop, &qc->timer);
    qc->timer.data = qc;

    return qc;
}

// Connection vtable.
typedef struct quic_send_req {
    uv_udp_send_t     req;
    uint8_t          *pkt;
    trellis_conn_send_cb cb;
    void             *ctx;
} quic_send_req_t;

static void quic_send_req_done(uv_udp_send_t *req, int status)
{
    quic_send_req_t *sr = (quic_send_req_t *)req;
    trellis_err_t err = (status == 0) ? TRELLIS_OK : TRELLIS_ERR_TRANSPORT;
    if (sr->cb)
        sr->cb(err, sr->ctx);
    free(sr->pkt);
    free(sr);
}

static trellis_err_t quic_conn_send(trellis_conn_t *conn,
                                     const uint8_t *data, size_t len,
                                     trellis_conn_send_cb cb, void *ctx)
{
    quic_stream_t    *s  = (quic_stream_t *)conn->impl_data;
    quic_quic_conn_t *qc = s->qconn;

    if (s->closed)
        return TRELLIS_ERR_CLOSED;

    const uint8_t *send_data = data;
    size_t         send_len  = len;
    uint8_t        capsule_buf[QUIC_MAX_PACKET_SIZE];

    if (qc->masque_ready) {
        send_len = quic_masque_capsule_wrap(data, len,
                                             capsule_buf, sizeof(capsule_buf));
        if (send_len == 0) {
            if (cb) cb(TRELLIS_ERR_TRANSPORT, ctx);
            return TRELLIS_ERR_TRANSPORT;
        }
        send_data = capsule_buf;
    }

    ngtcp2_vec datav = { (uint8_t *)send_data, send_len };
    ngtcp2_path_storage ps;
    ngtcp2_path_storage_zero(&ps);
    ngtcp2_pkt_info pi = {0};
    uint64_t ts = (uint64_t)uv_hrtime();

    ssize_t nwrite = ngtcp2_conn_writev_stream(
        qc->qconn, &ps.path, &pi,
        qc->pkt_buf, sizeof(qc->pkt_buf),
        NULL,
        NGTCP2_WRITE_STREAM_FLAG_NONE,
        s->stream_id, &datav, 1, ts);

    if (nwrite <= 0) {
        if (cb) cb(TRELLIS_ERR_TRANSPORT, ctx);
        return TRELLIS_ERR_TRANSPORT;
    }

    quic_send_req_t *sr = malloc(sizeof(*sr));
    uint8_t         *pkt = malloc((size_t)nwrite);
    if (!sr || !pkt) {
        free(sr); free(pkt);
        if (cb) cb(TRELLIS_ERR_NOMEM, ctx);
        return TRELLIS_ERR_NOMEM;
    }
    memcpy(pkt, qc->pkt_buf, (size_t)nwrite);
    sr->pkt = pkt;
    sr->cb  = cb;
    sr->ctx = ctx;

    uv_buf_t buf = uv_buf_init((char *)pkt, (unsigned)nwrite);
    uv_udp_send((uv_udp_send_t *)sr, qc->udp,
                &buf, 1,
                (const struct sockaddr *)&qc->remote_addr,
                quic_send_req_done);

    quic_timer_reset(qc);
    return TRELLIS_OK;
}

static trellis_err_t quic_conn_recv_start(trellis_conn_t *conn,
                                           trellis_conn_recv_cb cb, void *ctx)
{
    quic_stream_t *s = (quic_stream_t *)conn->impl_data;
    s->recv_cb      = cb;
    s->recv_ctx     = ctx;
    s->recv_active  = true;
    return TRELLIS_OK;
}

static void quic_conn_recv_stop(trellis_conn_t *conn)
{
    quic_stream_t *s = (quic_stream_t *)conn->impl_data;
    s->recv_active = false;
}

static void quic_conn_close(trellis_conn_t *conn,
                             trellis_conn_close_cb cb, void *ctx)
{
    quic_stream_t *s = (quic_stream_t *)conn->impl_data;
    if (!s->closed) {
        s->closed = true;
        ngtcp2_conn_shutdown_stream(s->qconn->qconn, 0,
                                    s->stream_id, NGTCP2_APP_NOERROR);
        quic_send_pending(s->qconn);
    }
    if (cb) cb(conn, ctx);
    trellis_conn_dealloc(conn);
}

static const trellis_conn_vtable_t quic_conn_vtable = {
    .send       = quic_conn_send,
    .recv_start = quic_conn_recv_start,
    .recv_stop  = quic_conn_recv_stop,
    .close      = quic_conn_close,
};

// Transport vtable.
static trellis_err_t quic_transport_connect(trellis_transport_t *t,
                                             const char *addr,
                                             trellis_connect_cb cb, void *ctx)
{
    quic_transport_data_t *td = (quic_transport_data_t *)t->impl_data;

    struct sockaddr_storage ss;
    if (trellis_parse_addr(addr, &ss) != 0)
        return TRELLIS_ERR_INVALID_ARG;

    socklen_t remote_len = (ss.ss_family == AF_INET6)
                           ? sizeof(struct sockaddr_in6)
                           : sizeof(struct sockaddr_in);

    // Bind client UDP socket to an ephemeral port if not already done.
    if (uv_udp_getsockname(&td->udp_client, NULL, &(int){0}) != 0) {
        struct sockaddr_in bind_addr;
        uv_ip4_addr("0.0.0.0", 0, &bind_addr);
        uv_udp_bind(&td->udp_client, (const struct sockaddr *)&bind_addr, 0);
        td->udp_client.data = td;
        uv_udp_recv_start(&td->udp_client, quic_udp_alloc_cb, quic_udp_recv_cb);
    }

    quic_quic_conn_t *qc = quic_conn_new(td, &td->udp_client,
                                          (const struct sockaddr *)&ss,
                                          remote_len, false);
    if (!qc)
        return TRELLIS_ERR_NOMEM;

    // Store connect callback; will be invoked after handshake.
    qc->connect_cb  = cb;
    qc->connect_ctx = ctx;
    qc->transport   = t;

    // 0-RTT session resumption.
    if (td->zero_rtt && td->session_cache_path[0])
        quic_session_load(qc, td->session_cache_path);

    // Register in transport connection table.
    if (td->conn_count < 64)
        td->conns[td->conn_count++] = qc;

    // Kick off the initial QUIC handshake (send Initial packet)
    quic_send_pending(qc);

    // MASQUE: after handshake completes, send CONNECT-UDP.
    if (td->masque_proxy_addr[0])
        qc->masque_enabled = true;

    return TRELLIS_OK;
}

static trellis_err_t quic_transport_listen(trellis_transport_t *t,
                                            const char *addr,
                                            trellis_accept_cb cb, void *ctx)
{
    quic_transport_data_t *td = (quic_transport_data_t *)t->impl_data;

    struct sockaddr_storage ss;
    if (trellis_parse_addr(addr, &ss) != 0)
        return TRELLIS_ERR_INVALID_ARG;

    td->accept_cb  = cb;
    td->accept_ctx = ctx;

    uv_udp_init(t->loop, &td->udp_server);
    td->udp_server.data = td;

    uv_udp_bind(&td->udp_server, (const struct sockaddr *)&ss, UV_UDP_REUSEADDR);
    uv_udp_recv_start(&td->udp_server, quic_udp_alloc_cb, quic_udp_recv_cb);
    td->listening = true;

    return TRELLIS_OK;
}

static trellis_err_t quic_transport_stop(trellis_transport_t *t)
{
    quic_transport_data_t *td = (quic_transport_data_t *)t->impl_data;
    if (td->listening) {
        uv_udp_recv_stop(&td->udp_server);
        td->listening = false;
    }
    uv_udp_recv_stop(&td->udp_client);
    return TRELLIS_OK;
}

static void quic_transport_free(trellis_transport_t *t)
{
    quic_transport_data_t *td = (quic_transport_data_t *)t->impl_data;
    for (size_t i = 0; i < td->conn_count; i++) {
        quic_quic_conn_t *qc = td->conns[i];
        if (!qc) continue;
        if (qc->timer_active)
            uv_timer_stop(&qc->timer);
        ngtcp2_conn_del(qc->qconn);
#ifdef NGTCP2_CRYPTO_MBEDTLS
        mbedtls_ssl_free(&qc->ssl);
        mbedtls_ssl_config_free(&qc->ssl_conf);
        mbedtls_ctr_drbg_free(&qc->drbg);
        mbedtls_entropy_free(&qc->entropy);
#else
        SSL_free(qc->ssl);
        SSL_CTX_free(qc->ssl_ctx);
#endif
        free(qc);
    }
    free(td);
    free(t);
}

static const trellis_transport_vtable_t quic_transport_vtable = {
    .connect = quic_transport_connect,
    .listen  = quic_transport_listen,
    .stop    = quic_transport_stop,
    .free    = quic_transport_free,
};

// Public constructor.
trellis_transport_t *trellis_transport_quic_new(uv_loop_t *loop,
                                                const trellis_quic_config_t *cfg)
{
    trellis_transport_t *t = trellis_transport_alloc(&quic_transport_vtable,
                                                       "quic", loop);
    if (!t) return NULL;

    quic_transport_data_t *td = calloc(1, sizeof(*td));
    if (!td) {
        free(t);
        return NULL;
    }
    t->impl_data = td;
    td->transport = t;

    uv_udp_init(loop, &td->udp_client);

    if (cfg) {
        if (cfg->sni) {
            size_t l = strlen(cfg->sni);
            if (l >= sizeof(td->sni)) l = sizeof(td->sni) - 1;
            memcpy(td->sni, cfg->sni, l);
        }
        if (cfg->ca_pem) {
            size_t l = strlen(cfg->ca_pem);
            if (l >= sizeof(td->ca_pem_path)) l = sizeof(td->ca_pem_path) - 1;
            memcpy(td->ca_pem_path, cfg->ca_pem, l);
        }
        if (cfg->cert_pem) {
            size_t l = strlen(cfg->cert_pem);
            if (l >= sizeof(td->cert_pem_path)) l = sizeof(td->cert_pem_path) - 1;
            memcpy(td->cert_pem_path, cfg->cert_pem, l);
        }
        if (cfg->key_pem) {
            size_t l = strlen(cfg->key_pem);
            if (l >= sizeof(td->key_pem_path)) l = sizeof(td->key_pem_path) - 1;
            memcpy(td->key_pem_path, cfg->key_pem, l);
        }
        if (cfg->session_cache_path) {
            size_t l = strlen(cfg->session_cache_path);
            if (l >= sizeof(td->session_cache_path)) l = sizeof(td->session_cache_path) - 1;
            memcpy(td->session_cache_path, cfg->session_cache_path, l);
        }
        if (cfg->masque_proxy_addr) {
            size_t l = strlen(cfg->masque_proxy_addr);
            if (l >= sizeof(td->masque_proxy_addr)) l = sizeof(td->masque_proxy_addr) - 1;
            memcpy(td->masque_proxy_addr, cfg->masque_proxy_addr, l);
        }
        td->zero_rtt    = cfg->zero_rtt;
        td->verify_peer = cfg->verify_peer;
    } else {
        td->verify_peer = true;
    }

    fprintf(stderr, "[QUIC] transport created (backend=%s, 0-RTT=%s, MASQUE=%s)\n",
            QUIC_TLS_BACKEND,
            td->zero_rtt ? "yes" : "no",
            td->masque_proxy_addr[0] ? td->masque_proxy_addr : "disabled");

    return t;
}

/* Silence unused-function warning for quic_session_save on platforms
 * where 0-RTT save is not yet wired into a callback. */
static void __attribute__((unused)) _quic_session_save_ref(void)
{
    quic_quic_conn_t *qc = NULL;
    quic_session_save(qc, NULL);
}

#else /* !TRELLIS_WITH_QUIC */

#include <trellis/transport.h>
#include <uv.h>
#include <stdio.h>

trellis_transport_t *trellis_transport_quic_new(uv_loop_t *loop,
                                                const trellis_quic_config_t *cfg)
{
    (void)loop; (void)cfg;
    fprintf(stderr, "[QUIC] QUIC transport unavailable "
                    "(build without TRELLIS_WITH_QUIC)\n");
    return NULL;
}

#endif /* TRELLIS_WITH_QUIC */
