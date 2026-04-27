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

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sodium.h>

/*
 * Minimal WebSocket transport (RFC 6455) over libuv TCP.
 *
 * Handshake is HTTP/1.1 Upgrade. After upgrade, binary frames (opcode 0x2)
 * carry payload. Client-to-server frames are masked; server-to-client are not.
 */
#define WS_GUID "258EAFA5-E914-47DA-95CA-C5AB0DC85B11"  /* RFC 6455 magic */
#define WS_OPCODE_BINARY 0x02
#define WS_OPCODE_CLOSE  0x08

// -----------
// State machine for handshake + framed I/O.
typedef enum {
    WS_STATE_CONNECTING,
    WS_STATE_ACCEPTING,
    WS_STATE_OPEN,
    WS_STATE_CLOSED,
} ws_state_t;

typedef struct ws_conn_data {
    uv_tcp_t              handle;
    trellis_conn_t       *conn;
    ws_state_t            state;
    bool                  is_client;

    trellis_conn_recv_cb  recv_cb;
    void                 *recv_ctx;

    uint8_t              *buf;
    size_t                buf_len;
    size_t                buf_cap;

    trellis_frame_parser_t frame_parser;

    // Client handshake: stash callbacks until upgrade completes.
    trellis_connect_cb    connect_cb;
    void                 *connect_ctx;

    // Server handshake: stash accept callback context.
    trellis_accept_cb     accept_cb;
    void                 *accept_ctx;

    char                  ws_key[32];
} ws_conn_data_t;

typedef struct ws_transport_data {
    uv_tcp_t              server;
    trellis_accept_cb     accept_cb;
    void                 *accept_ctx;
    trellis_transport_t  *transport;
    bool                  listening;
} ws_transport_data_t;

typedef struct ws_write_req {
    uv_write_t            req;
    uint8_t              *frame;
    trellis_conn_send_cb  cb;
    void                 *ctx;
} ws_write_req_t;

static const trellis_conn_vtable_t      ws_conn_vtable;
static const trellis_transport_vtable_t ws_transport_vtable;
static void ws_start_read(ws_conn_data_t *wd);

// -----------
// SHA-1 (RFC 3174) — needed for Sec-WebSocket-Accept.
static void sha1(const uint8_t *data, size_t len, uint8_t out[20])
{
    uint32_t h0 = 0x67452301, h1 = 0xEFCDAB89;
    uint32_t h2 = 0x98BADCFE, h3 = 0x10325476, h4 = 0xC3D2E1F0;

    uint64_t total_bits = (uint64_t)len * 8;
    size_t padded = ((len + 8) / 64 + 1) * 64;
    uint8_t *msg = calloc(1, padded);
    if (!msg)
        return;
    memcpy(msg, data, len);
    msg[len] = 0x80;
    for (int i = 0; i < 8; i++)
        msg[padded - 1 - i] = (uint8_t)(total_bits >> (i * 8));

    for (size_t offset = 0; offset < padded; offset += 64) {
        uint32_t w[80];
        for (int i = 0; i < 16; i++)
            w[i] = ((uint32_t)msg[offset + i * 4] << 24) |
                    ((uint32_t)msg[offset + i * 4 + 1] << 16) |
                    ((uint32_t)msg[offset + i * 4 + 2] << 8) |
                    ((uint32_t)msg[offset + i * 4 + 3]);
        for (int i = 16; i < 80; i++) {
            uint32_t v = w[i-3] ^ w[i-8] ^ w[i-14] ^ w[i-16];
            w[i] = (v << 1) | (v >> 31);
        }

        uint32_t a = h0, b = h1, c = h2, d = h3, e = h4;
        for (int i = 0; i < 80; i++) {
            uint32_t f, k;
            if (i < 20)      { f = (b & c) | (~b & d);        k = 0x5A827999; }
            else if (i < 40) { f = b ^ c ^ d;                 k = 0x6ED9EBA1; }
            else if (i < 60) { f = (b & c) | (b & d) | (c & d); k = 0x8F1BBCDC; }
            else              { f = b ^ c ^ d;                 k = 0xCA62C1D6; }
            uint32_t tmp = ((a << 5) | (a >> 27)) + f + e + k + w[i];
            e = d; d = c; c = (b << 30) | (b >> 2); b = a; a = tmp;
        }
        h0 += a; h1 += b; h2 += c; h3 += d; h4 += e;
    }
    free(msg);

    uint32_t h[5] = {h0, h1, h2, h3, h4};
    for (int i = 0; i < 5; i++) {
        out[i * 4]     = (uint8_t)(h[i] >> 24);
        out[i * 4 + 1] = (uint8_t)(h[i] >> 16);
        out[i * 4 + 2] = (uint8_t)(h[i] >> 8);
        out[i * 4 + 3] = (uint8_t)(h[i]);
    }
}

// -----------
// Base64 encode (for Sec-WebSocket-Accept)
static const char b64_table[] =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

static void base64_encode(const uint8_t *in, size_t len, char *out)
{
    size_t i = 0, j = 0;
    while (i + 2 < len) {
        uint32_t n = ((uint32_t)in[i] << 16) | ((uint32_t)in[i+1] << 8) | in[i+2];
        out[j++] = b64_table[(n >> 18) & 0x3f];
        out[j++] = b64_table[(n >> 12) & 0x3f];
        out[j++] = b64_table[(n >> 6) & 0x3f];
        out[j++] = b64_table[n & 0x3f];
        i += 3;
    }
    if (len - i == 2) {
        uint32_t n = ((uint32_t)in[i] << 16) | ((uint32_t)in[i+1] << 8);
        out[j++] = b64_table[(n >> 18) & 0x3f];
        out[j++] = b64_table[(n >> 12) & 0x3f];
        out[j++] = b64_table[(n >> 6) & 0x3f];
        out[j++] = '=';
    } else if (len - i == 1) {
        uint32_t n = (uint32_t)in[i] << 16;
        out[j++] = b64_table[(n >> 18) & 0x3f];
        out[j++] = b64_table[(n >> 12) & 0x3f];
        out[j++] = '=';
        out[j++] = '=';
    }
    out[j] = '\0';
}

static void ws_compute_accept(const char *key, char accept_out[29])
{
    char concat[128];
    snprintf(concat, sizeof(concat), "%s%s", key, WS_GUID);

    uint8_t digest[20];
    sha1((const uint8_t *)concat, strlen(concat), digest);
    base64_encode(digest, 20, accept_out);
}

static void ws_generate_key(char out[25])
{
    uint8_t raw[16];
    randombytes_buf(raw, sizeof(raw));
    base64_encode(raw, 16, out);
}

static int ws_buf_ensure(ws_conn_data_t *wd, size_t additional)
{
    size_t needed = wd->buf_len + additional;
    if (needed <= wd->buf_cap)
        return 0;
    size_t newcap = wd->buf_cap ? wd->buf_cap * 2 : 4096;
    while (newcap < needed)
        newcap *= 2;
    uint8_t *tmp = realloc(wd->buf, newcap);
    if (!tmp)
        return -1;
    wd->buf = tmp;
    wd->buf_cap = newcap;
    return 0;
}

// -----------
// Handshake parsing.
static const char *ws_find_header_end(const char *buf, size_t len)
{
    for (size_t i = 0; i + 3 < len; i++) {
        if (buf[i] == '\r' && buf[i+1] == '\n' &&
            buf[i+2] == '\r' && buf[i+3] == '\n')
            return buf + i + 4;
    }
    return NULL;
}

static bool ws_extract_header(const char *headers, const char *name,
                               char *out, size_t out_sz)
{
    size_t nlen = strlen(name);
    const char *p = headers;
    while (*p) {
        if (strncasecmp(p, name, nlen) == 0 && p[nlen] == ':') {
            p += nlen + 1;
            while (*p == ' ')
                p++;
            const char *end = strstr(p, "\r\n");
            if (!end)
                end = p + strlen(p);
            size_t vlen = (size_t)(end - p);
            if (vlen >= out_sz)
                vlen = out_sz - 1;
            memcpy(out, p, vlen);
            out[vlen] = '\0';
            return true;
        }
        const char *nl = strstr(p, "\r\n");
        if (!nl)
            break;
        p = nl + 2;
    }
    return false;
}

// -----------
// Process client handshake response.
static bool ws_process_client_handshake(ws_conn_data_t *wd)
{
    const char *end = ws_find_header_end((char *)wd->buf, wd->buf_len);
    if (!end)
        return false;

    char accept_expected[29];
    ws_compute_accept(wd->ws_key, accept_expected);

    char accept_received[64] = {0};
    ws_extract_header((char *)wd->buf, "Sec-WebSocket-Accept",
                      accept_received, sizeof(accept_received));

    size_t consumed = (size_t)(end - (char *)wd->buf);
    wd->buf_len -= consumed;
    if (wd->buf_len > 0)
        memmove(wd->buf, wd->buf + consumed, wd->buf_len);

    if (strcmp(accept_expected, accept_received) != 0)
        return false;

    wd->state = WS_STATE_OPEN;
    return true;
}

// -----------
// Process server handshake request.
static bool ws_process_server_handshake(ws_conn_data_t *wd)
{
    const char *end = ws_find_header_end((char *)wd->buf, wd->buf_len);
    if (!end)
        return false;

    char key[64] = {0};
    if (!ws_extract_header((char *)wd->buf, "Sec-WebSocket-Key",
                           key, sizeof(key)))
        return false;

    char accept[29];
    ws_compute_accept(key, accept);

    char response[512];
    int rlen = snprintf(response, sizeof(response),
        "HTTP/1.1 101 Switching Protocols\r\n"
        "Upgrade: websocket\r\n"
        "Connection: Upgrade\r\n"
        "Sec-WebSocket-Accept: %s\r\n\r\n",
        accept);

    uv_buf_t uvbuf = uv_buf_init(response, (unsigned int)rlen);
    // Synchronous write for handshake — acceptable during setup.
    uv_try_write((uv_stream_t *)&wd->handle, &uvbuf, 1);

    size_t consumed = (size_t)(end - (char *)wd->buf);
    wd->buf_len -= consumed;
    if (wd->buf_len > 0)
        memmove(wd->buf, wd->buf + consumed, wd->buf_len);

    wd->state = WS_STATE_OPEN;
    return true;
}

// -----------
// WebSocket frame parsing.
static void ws_deliver_frames(ws_conn_data_t *wd)
{
    while (wd->buf_len >= 2) {
        uint8_t *p = wd->buf;
        uint8_t opcode = p[0] & 0x0F;
        bool masked = (p[1] & 0x80) != 0;
        uint64_t payload_len = p[1] & 0x7F;
        size_t header_len = 2;

        if (payload_len == 126) {
            if (wd->buf_len < 4)
                return;
            payload_len = ((uint64_t)p[2] << 8) | p[3];
            header_len = 4;
        } else if (payload_len == 127) {
            if (wd->buf_len < 10)
                return;
            payload_len = 0;
            for (int i = 0; i < 8; i++)
                payload_len = (payload_len << 8) | p[2 + i];
            header_len = 10;
        }

        if (masked)
            header_len += 4;

        size_t frame_total = header_len + (size_t)payload_len;
        if (wd->buf_len < frame_total)
            return;

        uint8_t *payload = p + header_len;

        if (masked) {
            uint8_t *mask_key = p + header_len - 4;
            for (uint64_t i = 0; i < payload_len; i++)
                payload[i] ^= mask_key[i % 4];
        }

        if (opcode == WS_OPCODE_CLOSE) {
            wd->state = WS_STATE_CLOSED;
            if (wd->recv_cb)
                wd->recv_cb(wd->conn, NULL, 0, TRELLIS_ERR_CLOSED,
                            wd->recv_ctx);
            return;
        }

        if (opcode == WS_OPCODE_BINARY) {
            trellis_frame_parser_t *fp = &wd->frame_parser;
            fp->conn = wd->conn;
            fp->recv_cb = wd->recv_cb;
            fp->recv_ctx = wd->recv_ctx;

            size_t need = fp->buf_len + (size_t)payload_len;
            if (need > fp->buf_cap) {
                size_t nc = fp->buf_cap ? fp->buf_cap * 2 : 4096;
                while (nc < need) nc *= 2;
                uint8_t *nb = realloc(fp->buf, nc);
                if (nb) { fp->buf = nb; fp->buf_cap = nc; }
            }
            if (fp->buf) {
                memcpy(fp->buf + fp->buf_len, payload, (size_t)payload_len);
                fp->buf_len += (size_t)payload_len;
                trellis_frame_deliver(fp);
            }
        }

        wd->buf_len -= frame_total;
        if (wd->buf_len > 0)
            memmove(wd->buf, wd->buf + frame_total, wd->buf_len);
    }
}

// -----------
// libuv read callbacks.
static void ws_alloc_cb(uv_handle_t *handle, size_t suggested,
                         uv_buf_t *buf)
{
    ws_conn_data_t *wd = handle->data;
    if (ws_buf_ensure(wd, suggested) != 0) {
        buf->base = NULL;
        buf->len = 0;
        return;
    }
    buf->base = (char *)(wd->buf + wd->buf_len);
    buf->len = wd->buf_cap - wd->buf_len;
}

static void ws_read_cb(uv_stream_t *stream, ssize_t nread,
                        const uv_buf_t *buf)
{
    ws_conn_data_t *wd = stream->data;
    (void)buf;

    if (nread < 0) {
        ws_state_t prev = wd->state;
        wd->state = WS_STATE_CLOSED;
        if (prev == WS_STATE_CONNECTING && wd->connect_cb) {
            wd->connect_cb(NULL, TRELLIS_ERR_CONNECT, wd->connect_ctx);
        } else if (wd->recv_cb) {
            wd->recv_cb(wd->conn, NULL, 0, TRELLIS_ERR_CLOSED, wd->recv_ctx);
        }
        return;
    }

    if (nread == 0)
        return;

    wd->buf_len += (size_t)nread;

    switch (wd->state) {
    case WS_STATE_CONNECTING:
        if (ws_process_client_handshake(wd)) {
            if (wd->connect_cb)
                wd->connect_cb(wd->conn, TRELLIS_OK, wd->connect_ctx);
        }
        break;

    case WS_STATE_ACCEPTING:
        if (ws_process_server_handshake(wd)) {
            if (wd->accept_cb)
                wd->accept_cb(wd->conn, wd->accept_ctx);
            if (wd->buf_len > 0)
                ws_deliver_frames(wd);
        }
        break;

    case WS_STATE_OPEN:
        ws_deliver_frames(wd);
        break;

    case WS_STATE_CLOSED:
        break;
    }
}

static void ws_start_read(ws_conn_data_t *wd)
{
    uv_read_start((uv_stream_t *)&wd->handle, ws_alloc_cb, ws_read_cb);
}

static ws_conn_data_t *ws_conn_data_new(trellis_transport_t *t, bool is_client)
{
    trellis_conn_t *conn = trellis_conn_alloc(&ws_conn_vtable, "websocket",
                                              t->loop);
    if (!conn)
        return NULL;

    ws_conn_data_t *wd = calloc(1, sizeof(*wd));
    if (!wd) {
        trellis_conn_dealloc(conn);
        return NULL;
    }

    wd->conn = conn;
    wd->is_client = is_client;
    wd->handle.data = wd;
    conn->impl_data = wd;
    return wd;
}

// -----------
// Connection vtable.
static void ws_on_write(uv_write_t *req, int status)
{
    ws_write_req_t *wr = req->data;
    if (wr->cb) {
        trellis_err_t err = (status == 0) ? TRELLIS_OK : TRELLIS_ERR_SEND;
        wr->cb(err, wr->ctx);
    }
    free(wr->frame);
    free(wr);
}

static trellis_err_t ws_conn_send(trellis_conn_t *conn,
                                  const uint8_t *data, size_t len,
                                  trellis_conn_send_cb cb, void *ctx)
{
    ws_conn_data_t *wd = conn->impl_data;
    if (wd->state != WS_STATE_OPEN)
        return TRELLIS_ERR_CLOSED;

    // Apply wire-level framing (varint length, morph, camouflage)
    uint8_t *framed = NULL;
    size_t framed_len = trellis_frame_build(conn, data, len, &framed);
    if (!framed)
        return TRELLIS_ERR_NOMEM;

    if (conn->has_frame_tag && !conn->frame_tag_sent)
        conn->frame_tag_sent = true;

    const uint8_t *payload = framed;
    size_t payload_len = framed_len;

    bool mask = wd->is_client;
    size_t header_sz = 2;
    if (payload_len >= 126 && payload_len <= 0xFFFF)
        header_sz = 4;
    else if (payload_len > 0xFFFF)
        header_sz = 10;
    if (mask)
        header_sz += 4;

    size_t total = header_sz + payload_len;
    ws_write_req_t *wr = malloc(sizeof(*wr));
    if (!wr) {
        free(framed);
        return TRELLIS_ERR_NOMEM;
    }

    wr->frame = malloc(total);
    if (!wr->frame) {
        free(wr);
        free(framed);
        return TRELLIS_ERR_NOMEM;
    }

    uint8_t *p = wr->frame;
    p[0] = 0x80 | WS_OPCODE_BINARY;

    size_t off = 1;
    uint8_t mask_bit = mask ? 0x80 : 0x00;
    if (payload_len < 126) {
        p[off++] = mask_bit | (uint8_t)payload_len;
    } else if (payload_len <= 0xFFFF) {
        p[off++] = mask_bit | 126;
        p[off++] = (uint8_t)(payload_len >> 8);
        p[off++] = (uint8_t)(payload_len);
    } else {
        p[off++] = mask_bit | 127;
        for (int i = 7; i >= 0; i--)
            p[off++] = (uint8_t)(payload_len >> (i * 8));
    }

    if (mask) {
        uint8_t mask_key[4];
        randombytes_buf(mask_key, 4);
        memcpy(p + off, mask_key, 4);
        off += 4;
        for (size_t i = 0; i < payload_len; i++)
            p[off + i] = payload[i] ^ mask_key[i % 4];
    } else {
        memcpy(p + off, payload, payload_len);
    }

    free(framed);

    wr->cb = cb;
    wr->ctx = ctx;
    wr->req.data = wr;

    uv_buf_t uvbuf = uv_buf_init((char *)wr->frame, (unsigned int)total);
    int rc = uv_write(&wr->req, (uv_stream_t *)&wd->handle,
                      &uvbuf, 1, ws_on_write);
    if (rc != 0) {
        free(wr->frame);
        free(wr);
        return TRELLIS_ERR_SEND;
    }

    return TRELLIS_OK;
}

static trellis_err_t ws_conn_recv_start(trellis_conn_t *conn,
                                        trellis_conn_recv_cb cb, void *ctx)
{
    ws_conn_data_t *wd = conn->impl_data;
    wd->recv_cb = cb;
    wd->recv_ctx = ctx;
    wd->frame_parser.conn = conn;
    wd->frame_parser.recv_cb = cb;
    wd->frame_parser.recv_ctx = ctx;
    return TRELLIS_OK;
}

static void ws_conn_recv_stop(trellis_conn_t *conn)
{
    ws_conn_data_t *wd = conn->impl_data;
    wd->recv_cb = NULL;
    wd->recv_ctx = NULL;
}

typedef struct ws_close_ctx {
    trellis_conn_close_cb cb;
    void                 *ctx;
    trellis_conn_t       *conn;
    ws_conn_data_t       *wd;
} ws_close_ctx_t;

static void ws_on_close(uv_handle_t *handle)
{
    ws_close_ctx_t *cc = handle->data;
    trellis_conn_close_cb cb = cc->cb;
    void *ctx = cc->ctx;
    trellis_conn_t *conn = cc->conn;

    free(cc->wd->buf);
    free(cc->wd->frame_parser.buf);
    free(cc->wd);
    free(cc);
    conn->impl_data = NULL;

    if (cb)
        cb(conn, ctx);

    trellis_conn_dealloc(conn);
}

static void ws_conn_close(trellis_conn_t *conn,
                           trellis_conn_close_cb cb, void *ctx)
{
    ws_conn_data_t *wd = conn->impl_data;
    if (!wd)
        return;

    wd->state = WS_STATE_CLOSED;
    uv_read_stop((uv_stream_t *)&wd->handle);

    ws_close_ctx_t *cc = malloc(sizeof(*cc));
    if (!cc) {
        if (cb) cb(conn, ctx);
        return;
    }
    cc->cb = cb;
    cc->ctx = ctx;
    cc->conn = conn;
    cc->wd = wd;

    wd->handle.data = cc;
    uv_close((uv_handle_t *)&wd->handle, ws_on_close);
}

static const trellis_conn_vtable_t ws_conn_vtable = {
    .send       = ws_conn_send,
    .recv_start = ws_conn_recv_start,
    .recv_stop  = ws_conn_recv_stop,
    .close      = ws_conn_close,
};

// -----------
// Transport: connect (client side)
typedef struct ws_connect_req {
    uv_connect_t          req;
    ws_conn_data_t       *wd;
    trellis_connect_cb    cb;
    void                 *ctx;
    trellis_transport_t  *transport;
    char                  host[TRELLIS_ADDR_MAX];
} ws_connect_req_t;

static void ws_on_tcp_connect(uv_connect_t *req, int status)
{
    ws_connect_req_t *cr = (ws_connect_req_t *)req;
    ws_conn_data_t *wd = cr->wd;

    if (status != 0) {
        uv_close((uv_handle_t *)&wd->handle, NULL);
        if (cr->cb)
            cr->cb(NULL, TRELLIS_ERR_CONNECT, cr->ctx);
        free(wd);
        free(cr);
        return;
    }

    struct sockaddr_storage local_ss, remote_ss;
    int namelen = sizeof(local_ss);
    if (uv_tcp_getsockname(&wd->handle,
                            (struct sockaddr *)&local_ss, &namelen) == 0)
        trellis_conn_set_addrs(wd->conn, (struct sockaddr *)&local_ss, NULL);
    namelen = sizeof(remote_ss);
    if (uv_tcp_getpeername(&wd->handle,
                            (struct sockaddr *)&remote_ss, &namelen) == 0)
        trellis_conn_set_addrs(wd->conn, NULL, (struct sockaddr *)&remote_ss);

    wd->state = WS_STATE_CONNECTING;
    wd->connect_cb = cr->cb;
    wd->connect_ctx = cr->ctx;

    ws_generate_key(wd->ws_key);

    char request[512];
    int rlen = snprintf(request, sizeof(request),
        "GET / HTTP/1.1\r\n"
        "Host: %s\r\n"
        "Upgrade: websocket\r\n"
        "Connection: Upgrade\r\n"
        "Sec-WebSocket-Key: %s\r\n"
        "Sec-WebSocket-Version: 13\r\n\r\n",
        cr->host, wd->ws_key);

    uv_buf_t uvbuf = uv_buf_init(request, (unsigned int)rlen);
    uv_try_write((uv_stream_t *)&wd->handle, &uvbuf, 1);

    ws_start_read(wd);
    free(cr);
}

static trellis_err_t ws_transport_connect(trellis_transport_t *t,
                                          const char *addr,
                                          trellis_connect_cb cb, void *ctx)
{
    struct sockaddr_storage ss;
    if (trellis_parse_addr(addr, &ss) != 0)
        return TRELLIS_ERR_INVALID_ARG;

    ws_conn_data_t *wd = ws_conn_data_new(t, true);
    if (!wd)
        return TRELLIS_ERR_NOMEM;

    ws_connect_req_t *cr = calloc(1, sizeof(*cr));
    if (!cr) {
        trellis_conn_dealloc(wd->conn);
        free(wd);
        return TRELLIS_ERR_NOMEM;
    }

    int rc = uv_tcp_init(t->loop, &wd->handle);
    if (rc != 0) {
        trellis_conn_dealloc(wd->conn);
        free(wd);
        free(cr);
        return TRELLIS_ERR_TRANSPORT;
    }

    cr->wd = wd;
    cr->cb = cb;
    cr->ctx = ctx;
    cr->transport = t;
    snprintf(cr->host, sizeof(cr->host), "%s", addr);

    rc = uv_tcp_connect(&cr->req, &wd->handle,
                        (const struct sockaddr *)&ss, ws_on_tcp_connect);
    if (rc != 0) {
        uv_close((uv_handle_t *)&wd->handle, NULL);
        trellis_conn_dealloc(wd->conn);
        free(wd);
        free(cr);
        return TRELLIS_ERR_CONNECT;
    }

    return TRELLIS_OK;
}

// -----------
// Transport: listen (server side)
static void ws_on_connection(uv_stream_t *server, int status)
{
    ws_transport_data_t *td = server->data;
    if (status < 0)
        return;

    ws_conn_data_t *wd = ws_conn_data_new(td->transport, false);
    if (!wd)
        return;

    int rc = uv_tcp_init(td->transport->loop, &wd->handle);
    if (rc != 0) {
        trellis_conn_dealloc(wd->conn);
        free(wd);
        return;
    }
    wd->handle.data = wd;

    rc = uv_accept(server, (uv_stream_t *)&wd->handle);
    if (rc != 0) {
        uv_close((uv_handle_t *)&wd->handle, NULL);
        trellis_conn_dealloc(wd->conn);
        free(wd);
        return;
    }

    struct sockaddr_storage local_ss, remote_ss;
    int namelen = sizeof(local_ss);
    if (uv_tcp_getsockname(&wd->handle,
                            (struct sockaddr *)&local_ss, &namelen) == 0)
        trellis_conn_set_addrs(wd->conn, (struct sockaddr *)&local_ss, NULL);
    namelen = sizeof(remote_ss);
    if (uv_tcp_getpeername(&wd->handle,
                            (struct sockaddr *)&remote_ss, &namelen) == 0)
        trellis_conn_set_addrs(wd->conn, NULL, (struct sockaddr *)&remote_ss);

    wd->state = WS_STATE_ACCEPTING;
    wd->accept_cb = td->accept_cb;
    wd->accept_ctx = td->accept_ctx;

    ws_start_read(wd);
}

static trellis_err_t ws_transport_listen(trellis_transport_t *t,
                                         const char *addr,
                                         trellis_accept_cb cb, void *ctx)
{
    ws_transport_data_t *td = t->impl_data;
    if (td->listening)
        return TRELLIS_ERR_ALREADY_EXISTS;

    struct sockaddr_storage ss;
    if (trellis_parse_addr(addr, &ss) != 0)
        return TRELLIS_ERR_INVALID_ARG;

    int rc = uv_tcp_init(t->loop, &td->server);
    if (rc != 0)
        return TRELLIS_ERR_TRANSPORT;

    td->server.data = td;
    td->accept_cb = cb;
    td->accept_ctx = ctx;

    rc = uv_tcp_bind(&td->server, (const struct sockaddr *)&ss, 0);
    if (rc != 0) {
        uv_close((uv_handle_t *)&td->server, NULL);
        return TRELLIS_ERR_LISTEN;
    }

    rc = uv_listen((uv_stream_t *)&td->server, 128, ws_on_connection);
    if (rc != 0) {
        uv_close((uv_handle_t *)&td->server, NULL);
        return TRELLIS_ERR_LISTEN;
    }

    td->listening = true;
    return TRELLIS_OK;
}

// -----------
// Transport: stop / free.
static trellis_err_t ws_transport_stop(trellis_transport_t *t)
{
    ws_transport_data_t *td = t->impl_data;
    if (!td->listening)
        return TRELLIS_OK;

    uv_close((uv_handle_t *)&td->server, NULL);
    td->listening = false;
    return TRELLIS_OK;
}

static void ws_transport_free(trellis_transport_t *t)
{
    ws_transport_data_t *td = t->impl_data;
    if (td) {
        if (td->listening)
            uv_close((uv_handle_t *)&td->server, NULL);
        free(td);
    }
}

static const trellis_transport_vtable_t ws_transport_vtable = {
    .connect = ws_transport_connect,
    .listen  = ws_transport_listen,
    .stop    = ws_transport_stop,
    .free    = ws_transport_free,
};

// -----------
// Public constructor.
trellis_transport_t *trellis_transport_ws_new(uv_loop_t *loop)
{
    trellis_transport_t *t = trellis_transport_alloc(
        &ws_transport_vtable, "websocket", loop);
    if (!t)
        return NULL;

    ws_transport_data_t *td = calloc(1, sizeof(*td));
    if (!td) {
        free(t);
        return NULL;
    }

    td->transport = t;
    t->impl_data = td;
    return t;
}
