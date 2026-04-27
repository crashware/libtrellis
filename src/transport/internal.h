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

#ifndef TRELLIS_TRANSPORT_INTERNAL_H
#define TRELLIS_TRANSPORT_INTERNAL_H

#include <trellis/transport.h>
#include <trellis/morph.h>
#include <trellis/camouflage.h>
#include <uv.h>
#include <sodium.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

// Connection vtable.
typedef struct trellis_conn_vtable {
    trellis_err_t (*send)(trellis_conn_t *conn,
                          const uint8_t *data, size_t len,
                          trellis_conn_send_cb cb, void *ctx);
    trellis_err_t (*recv_start)(trellis_conn_t *conn,
                                trellis_conn_recv_cb cb, void *ctx);
    void          (*recv_stop)(trellis_conn_t *conn);
    void          (*close)(trellis_conn_t *conn,
                           trellis_conn_close_cb cb, void *ctx);
} trellis_conn_vtable_t;

// Per-connection rate limiting (token bucket + bandwidth tracking)
typedef struct trellis_conn_rate_limit {
    uint32_t tokens;           /* current token count */
    uint32_t max_tokens;       /* bucket capacity (burst) */
    uint32_t refill_rate;      /* tokens per second (sustained) */
    uint64_t last_refill_ms;   /* timestamp of last refill */
    uint64_t bytes_recv;       /* bytes received this second */
    uint64_t bytes_recv_window; /* start of current byte-counting window */
    uint64_t max_bytes_per_sec; /* bandwidth cap (0 = unlimited) */
} trellis_conn_rate_limit_t;

#define TRELLIS_CONN_RATE_DEFAULT_BURST    100  /* frames/sec burst */
#define TRELLIS_CONN_RATE_DEFAULT_SUSTAIN  50   /* frames/sec sustained */
#define TRELLIS_CONN_RATE_DEFAULT_BW       (1024 * 1024) /* 1 MB/s */

// Per-connection send queue entry.
typedef struct trellis_send_buf {
    uint8_t                  *data;
    size_t                    len;
    trellis_conn_send_cb      cb;
    void                     *ctx;
    struct trellis_send_buf  *next;
} trellis_send_buf_t;

#define TRELLIS_SEND_QUEUE_HI  (256u * 1024u)  /* 256 KB: stop accepting */
#define TRELLIS_SEND_QUEUE_LO  (64u * 1024u)   /* 64 KB: resume accepting */

struct trellis_conn {
    const trellis_conn_vtable_t *vtable;
    char                        *remote_addr;
    char                        *local_addr;
    const char                  *transport_name;
    uv_loop_t                   *loop;
    void                        *impl_data;

    uint8_t                      frame_tag[TRELLIS_FRAME_TAG_LEN];
    bool                         has_frame_tag;
    bool                         frame_tag_sent;
    bool                         frame_tag_validated;

    uint8_t                      frame_varint_offset;

    trellis_morph_t             *wire_morph;
    trellis_camouflage_t        *wire_camo;

    // Peer's authenticated Bloom fingerprint (set by handshake layer)
    trellis_fingerprint_t        peer_fingerprint;
    bool                         peer_fingerprint_valid;

    trellis_conn_rate_limit_t    rate_limit;

    // Send queue with backpressure.
    trellis_send_buf_t          *send_queue_head;
    trellis_send_buf_t          *send_queue_tail;
    size_t                       send_queue_bytes;
    bool                         send_queue_writing; /* vtable write in-flight */
    bool                         send_queue_full;    /* above high-water mark */

    // Graceful close state.
    bool                         closing;
    trellis_conn_close_cb        close_cb;
    void                        *close_ctx;
};

// Transport vtable.
typedef struct trellis_transport_vtable {
    trellis_err_t (*connect)(trellis_transport_t *t, const char *addr,
                             trellis_connect_cb cb, void *ctx);
    trellis_err_t (*listen)(trellis_transport_t *t, const char *addr,
                            trellis_accept_cb cb, void *ctx);
    trellis_err_t (*stop)(trellis_transport_t *t);
    void          (*free)(trellis_transport_t *t);
} trellis_transport_vtable_t;

struct trellis_transport {
    const trellis_transport_vtable_t *vtable;
    const char                       *protocol_name;
    uv_loop_t                        *loop;
    void                             *impl_data;
};

#define TRELLIS_ADDR_MAX 256

static inline trellis_conn_t *trellis_conn_alloc(
    const trellis_conn_vtable_t *vtable,
    const char *transport_name,
    uv_loop_t *loop)
{
    trellis_conn_t *conn = calloc(1, sizeof(*conn));
    if (!conn)
        return NULL;
    conn->vtable = vtable;
    conn->transport_name = transport_name;
    conn->loop = loop;
    conn->rate_limit.max_tokens = TRELLIS_CONN_RATE_DEFAULT_BURST;
    conn->rate_limit.refill_rate = TRELLIS_CONN_RATE_DEFAULT_SUSTAIN;
    conn->rate_limit.tokens = TRELLIS_CONN_RATE_DEFAULT_BURST;
    conn->rate_limit.max_bytes_per_sec = TRELLIS_CONN_RATE_DEFAULT_BW;
    conn->rate_limit.last_refill_ms = trellis_now_ms();
    conn->rate_limit.bytes_recv_window = trellis_now_ms();
    return conn;
}

/* Token bucket check: refills tokens, returns TRELLIS_OK if a frame is
 * allowed, TRELLIS_ERR_RATE_LIMITED otherwise. */
static inline trellis_err_t trellis_conn_rate_check_frame(
    trellis_conn_t *conn, size_t frame_bytes)
{
    trellis_conn_rate_limit_t *rl = &conn->rate_limit;
    if (rl->max_tokens == 0)
        return TRELLIS_OK;

    uint64_t now = trellis_now_ms();

    /* Refill tokens — advance by whole seconds only to avoid losing
     * fractional-second credit from integer division. */
    uint64_t elapsed = now - rl->last_refill_ms;
    if (elapsed >= 1000) {
        uint64_t full_secs = elapsed / 1000;
        uint64_t refill = full_secs * (uint64_t)rl->refill_rate;
        if (refill > rl->max_tokens)
            refill = rl->max_tokens;
        rl->tokens += (uint32_t)refill;
        if (rl->tokens > rl->max_tokens)
            rl->tokens = rl->max_tokens;
        rl->last_refill_ms += full_secs * 1000;
    }

    // Bandwidth check.
    if (rl->max_bytes_per_sec > 0) {
        if (now - rl->bytes_recv_window >= 1000) {
            rl->bytes_recv = 0;
            rl->bytes_recv_window = now;
        }
        if (rl->bytes_recv + frame_bytes > rl->max_bytes_per_sec)
            return TRELLIS_ERR_RATE_LIMITED;
        rl->bytes_recv += frame_bytes;
    }

    // Token check.
    if (rl->tokens == 0)
        return TRELLIS_ERR_RATE_LIMITED;
    rl->tokens--;
    return TRELLIS_OK;
}

static inline void trellis_conn_set_addrs(trellis_conn_t *conn,
                                          const struct sockaddr *local,
                                          const struct sockaddr *remote)
{
    char buf[TRELLIS_ADDR_MAX];
    int port;

    if (local) {
        if (local->sa_family == AF_INET) {
            const struct sockaddr_in *s = (const struct sockaddr_in *)local;
            uv_ip4_name(s, buf, sizeof(buf));
            port = ntohs(s->sin_port);
        } else {
            const struct sockaddr_in6 *s = (const struct sockaddr_in6 *)local;
            uv_ip6_name(s, buf, sizeof(buf));
            port = ntohs(s->sin6_port);
        }
        size_t len = strlen(buf);
        snprintf(buf + len, sizeof(buf) - len, ":%d", port);
        conn->local_addr = strdup(buf);
    }

    if (remote) {
        if (remote->sa_family == AF_INET) {
            const struct sockaddr_in *s = (const struct sockaddr_in *)remote;
            uv_ip4_name(s, buf, sizeof(buf));
            port = ntohs(s->sin_port);
        } else {
            const struct sockaddr_in6 *s = (const struct sockaddr_in6 *)remote;
            uv_ip6_name(s, buf, sizeof(buf));
            port = ntohs(s->sin6_port);
        }
        size_t len = strlen(buf);
        snprintf(buf + len, sizeof(buf) - len, ":%d", port);
        conn->remote_addr = strdup(buf);
    }
}

static inline void trellis_conn_dealloc(trellis_conn_t *conn)
{
    if (!conn)
        return;
    // Drain send queue.
    trellis_send_buf_t *sb = conn->send_queue_head;
    while (sb) {
        trellis_send_buf_t *next = sb->next;
        free(sb->data);
        free(sb);
        sb = next;
    }
    conn->send_queue_head = conn->send_queue_tail = NULL;
    conn->send_queue_bytes = 0;

    if (conn->wire_morph)
        trellis_morph_free(conn->wire_morph);
    if (conn->wire_camo)
        trellis_camouflage_free(conn->wire_camo);
    free(conn->remote_addr);
    free(conn->local_addr);
    free(conn);
}

static inline trellis_transport_t *trellis_transport_alloc(
    const trellis_transport_vtable_t *vtable,
    const char *protocol_name,
    uv_loop_t *loop)
{
    trellis_transport_t *t = calloc(1, sizeof(*t));
    if (!t)
        return NULL;
    t->vtable = vtable;
    t->protocol_name = protocol_name;
    t->loop = loop;
    return t;
}

// Variable-length frame encoding (QUIC-style varint)
static inline size_t trellis_varint_size(uint64_t v)
{
    if (v < 0x40)        return 1;
    if (v < 0x4000)      return 2;
    if (v < 0x40000000)  return 4;
    return 8;
}

static inline size_t trellis_varint_encode(uint8_t *buf, uint64_t v)
{
    if (v < 0x40) {
        buf[0] = (uint8_t)v;
        return 1;
    }
    if (v < 0x4000) {
        buf[0] = (uint8_t)(0x40 | (v >> 8));
        buf[1] = (uint8_t)(v);
        return 2;
    }
    if (v < 0x40000000) {
        buf[0] = (uint8_t)(0x80 | (v >> 24));
        buf[1] = (uint8_t)(v >> 16);
        buf[2] = (uint8_t)(v >> 8);
        buf[3] = (uint8_t)(v);
        return 4;
    }
    buf[0] = (uint8_t)(0xC0 | (v >> 56));
    buf[1] = (uint8_t)(v >> 48);
    buf[2] = (uint8_t)(v >> 40);
    buf[3] = (uint8_t)(v >> 32);
    buf[4] = (uint8_t)(v >> 24);
    buf[5] = (uint8_t)(v >> 16);
    buf[6] = (uint8_t)(v >> 8);
    buf[7] = (uint8_t)(v);
    return 8;
}

static inline size_t trellis_varint_decode(const uint8_t *buf, size_t buf_len,
                                           uint64_t *out)
{
    if (buf_len < 1) return 0;
    uint8_t prefix = buf[0] >> 6;
    size_t len = (size_t)(1 << prefix);
    if (buf_len < len) return 0;
    *out = buf[0] & 0x3F;
    for (size_t i = 1; i < len; i++)
        *out = (*out << 8) | buf[i];
    return len;
}

// Common frame delivery for length-prefixed transports.
typedef struct trellis_frame_parser {
    trellis_conn_t       *conn;
    uint8_t              *buf;
    size_t                buf_len;
    size_t                buf_cap;
    trellis_conn_recv_cb  recv_cb;
    void                 *recv_ctx;
} trellis_frame_parser_t;

/*
 * Determine the size of the next complete camouflage protocol unit in the
 * buffer.  Returns 0 when there isn't enough data for a complete unit.
 */
static inline size_t trellis_camo_unit_size(trellis_camouflage_type_t type,
                                            const uint8_t *buf, size_t len)
{
    switch (type) {
    case TRELLIS_CAMO_TLS13:
        if (len < 5) return 0;
        return 5 + (((size_t)buf[3] << 8) | buf[4]);
    case TRELLIS_CAMO_HTTP2: {
        const uint8_t *p = buf;
        size_t l = len;
        if (l >= 24 && memcmp(p, "PRI * HTTP/2.0\r\n\r\nSM\r\n\r\n", 24) == 0) {
            p += 24; l -= 24;
        }
        if (l < 9) return 0;
        uint32_t pl = ((uint32_t)p[0] << 16) | ((uint32_t)p[1] << 8) | p[2];
        return (size_t)(p - buf) + 9 + pl;
    }
    case TRELLIS_CAMO_DNS:
        if (len < 2) return 0;
        return 2 + (((size_t)buf[0] << 8) | buf[1]);
    case TRELLIS_CAMO_QUIC: {
        if (len < 7) return 0;
        const uint8_t *p = buf + 5;
        size_t l = len - 5;
        if (l < 1) return 0; uint8_t d = *p++; l--;
        if (l < d) return 0; p += d; l -= d;
        if (l < 1) return 0; uint8_t s = *p++; l--;
        if (l < s) return 0; p += s; l -= s;
        if (l < 1) return 0; uint8_t t = *p++; l--;
        if (l < t) return 0; p += t; l -= t;
        if (l < 2) return 0;
        uint16_t payload = (uint16_t)((p[0] & 0x3F) << 8) | p[1];
        p += 2;
        return (size_t)(p - buf) + payload;
    }
    default:
        return len;
    }
}

/*
 * Deliver one inner frame (after camo unwrap + morph decode) to the
 * application callback.  Handles tag validation and varint parsing.
 */
static inline void trellis_frame_deliver_inner(trellis_frame_parser_t *fp,
                                               const uint8_t *data, size_t len)
{
    trellis_conn_t *conn = fp->conn;

    if (conn->has_frame_tag && !conn->frame_tag_validated) {
        if (len < TRELLIS_FRAME_TAG_LEN)
            return;
        if (memcmp(data, conn->frame_tag, TRELLIS_FRAME_TAG_LEN) != 0) {
            if (fp->recv_cb)
                fp->recv_cb(conn, NULL, 0, TRELLIS_ERR_TRANSPORT,
                            fp->recv_ctx);
            return;
        }
        conn->frame_tag_validated = true;
        data += TRELLIS_FRAME_TAG_LEN;
        len  -= TRELLIS_FRAME_TAG_LEN;
    }

    size_t vi_off = (size_t)conn->frame_varint_offset;
    if (len < vi_off)
        return;
    data += vi_off;
    len  -= vi_off;

    uint64_t frame_len;
    size_t hdr_len = trellis_varint_decode(data, len, &frame_len);
    if (hdr_len > 0 && frame_len <= TRELLIS_MAX_FRAME_LEN &&
        hdr_len + (size_t)frame_len <= len) {
        if (trellis_conn_rate_check_frame(conn, (size_t)frame_len)
                != TRELLIS_OK)
            return; /* silently drop rate-limited frames */
        if (fp->recv_cb)
            fp->recv_cb(conn, data + hdr_len, (size_t)frame_len,
                        TRELLIS_OK, fp->recv_ctx);
    }
}

static inline void trellis_frame_deliver(trellis_frame_parser_t *fp)
{
    trellis_conn_t *conn = fp->conn;

    /*
     * Protocol-mimic camouflage (TLS13, HTTP2, DNS, QUIC) wraps each frame
     * as an individual protocol unit with its own header.  We extract and
     * unwrap each unit independently, then morph-decode and parse inside.
     * This avoids concatenating camo padding across frame boundaries.
     */
    if (conn->wire_camo &&
        conn->wire_camo->type != TRELLIS_CAMO_NONE &&
        conn->wire_camo->type != TRELLIS_CAMO_RAW_OBFS) {

        size_t off = 0;
        while (off < fp->buf_len) {
            size_t usize = trellis_camo_unit_size(
                conn->wire_camo->type,
                fp->buf + off, fp->buf_len - off);
            if (usize == 0 || off + usize > fp->buf_len)
                break;

            trellis_buf_t uw = {0};
            if (trellis_camouflage_unwrap(conn->wire_camo,
                    fp->buf + off, usize, &uw) != TRELLIS_OK || !uw.data) {
                off += usize;
                continue;
            }

            const uint8_t *fdata = uw.data;
            size_t flen = uw.len;
            trellis_buf_t dm = {0};

            if (conn->wire_morph) {
                if (trellis_morph_decode(conn->wire_morph, uw.data, uw.len,
                                         &dm) == TRELLIS_OK && dm.data) {
                    fdata = dm.data;
                    flen = dm.len;
                }
            }

            trellis_frame_deliver_inner(fp, fdata, flen);

            trellis_buf_free(&uw);
            if (dm.data) trellis_buf_free(&dm);
            off += usize;
        }

        if (off > 0) {
            fp->buf_len -= off;
            if (fp->buf_len > 0)
                memmove(fp->buf, fp->buf + off, fp->buf_len);
        }
        return;
    }

    /*
     * RAW_OBFS: XOR stream cipher operates on the full buffer (same
     * length in, same length out).  After unwrap we have the original
     * framed bytes.
     */
    if (conn->wire_camo &&
        conn->wire_camo->type == TRELLIS_CAMO_RAW_OBFS &&
        fp->buf_len > 0) {
        trellis_buf_t dec = {0};
        if (trellis_camouflage_unwrap(conn->wire_camo,
                fp->buf, fp->buf_len, &dec) == TRELLIS_OK && dec.data) {
            memcpy(fp->buf, dec.data, dec.len);
            fp->buf_len = dec.len;
            trellis_buf_free(&dec);
        }
    }

    /*
     * Wire morph without protocol-level camo: best-effort decode of the
     * buffer.  Works correctly when each TCP read delivers at least one
     * complete morph frame.
     */
    if (conn->wire_morph && fp->buf_len > 0) {
        trellis_buf_t dm = {0};
        if (trellis_morph_decode(conn->wire_morph, fp->buf, fp->buf_len,
                                 &dm) == TRELLIS_OK && dm.data) {
            if (dm.len > fp->buf_cap) {
                uint8_t *nb = realloc(fp->buf, dm.len);
                if (nb) { fp->buf = nb; fp->buf_cap = dm.len; }
            }
            memcpy(fp->buf, dm.data, dm.len);
            fp->buf_len = dm.len;
            trellis_buf_free(&dm);
        }
    }

    // ---- Tag validation + varint frame parsing (stream path) ----
    if (conn->has_frame_tag && !conn->frame_tag_validated) {
        if (fp->buf_len < TRELLIS_FRAME_TAG_LEN)
            return;
        if (memcmp(fp->buf, conn->frame_tag, TRELLIS_FRAME_TAG_LEN) != 0) {
            if (fp->recv_cb)
                fp->recv_cb(conn, NULL, 0, TRELLIS_ERR_TRANSPORT,
                            fp->recv_ctx);
            return;
        }
        conn->frame_tag_validated = true;
        fp->buf_len -= TRELLIS_FRAME_TAG_LEN;
        if (fp->buf_len > 0)
            memmove(fp->buf, fp->buf + TRELLIS_FRAME_TAG_LEN, fp->buf_len);
    }

    size_t vi_off = (size_t)conn->frame_varint_offset;

    while (fp->buf_len > vi_off) {
        uint64_t frame_len;
        size_t hdr_len = trellis_varint_decode(fp->buf + vi_off,
                                               fp->buf_len - vi_off,
                                               &frame_len);
        if (hdr_len == 0)
            break;

        if (frame_len > TRELLIS_MAX_FRAME_LEN) {
            if (fp->recv_cb)
                fp->recv_cb(conn, NULL, 0, TRELLIS_ERR_TRANSPORT,
                            fp->recv_ctx);
            return;
        }

        size_t total = vi_off + hdr_len + (size_t)frame_len;
        if (fp->buf_len < total)
            break;

        if (trellis_conn_rate_check_frame(conn, (size_t)frame_len)
                != TRELLIS_OK) {
            fp->buf_len -= total;
            if (fp->buf_len > 0)
                memmove(fp->buf, fp->buf + total, fp->buf_len);
            vi_off = (size_t)conn->frame_varint_offset;
            continue;
        }

        if (fp->recv_cb)
            fp->recv_cb(conn, fp->buf + vi_off + hdr_len, (size_t)frame_len,
                        TRELLIS_OK, fp->recv_ctx);

        fp->buf_len -= total;
        if (fp->buf_len > 0)
            memmove(fp->buf, fp->buf + total, fp->buf_len);

        /* The delivery callback may have changed connection state
           (e.g. finalize_handshake set frame_tag / morph / camo).
           Re-enter from the top to pick up the new settings. */
        if (conn->wire_camo || conn->wire_morph ||
            (conn->has_frame_tag && !conn->frame_tag_validated)) {
            trellis_frame_deliver(fp);
            return;
        }
        vi_off = (size_t)conn->frame_varint_offset;
    }
}

static inline size_t trellis_frame_build(trellis_conn_t *conn,
                                         const uint8_t *data, size_t len,
                                         uint8_t **out)
{
    size_t tag_len = (conn->has_frame_tag && !conn->frame_tag_sent)
                     ? TRELLIS_FRAME_TAG_LEN : 0;
    size_t vi_off = (size_t)conn->frame_varint_offset;
    size_t hdr_len = trellis_varint_size((uint64_t)len);
    size_t raw_total = tag_len + vi_off + hdr_len + len;

    uint8_t *raw = malloc(raw_total);
    if (!raw) { *out = NULL; return 0; }

    uint8_t *p = raw;
    if (tag_len > 0) {
        memcpy(p, conn->frame_tag, TRELLIS_FRAME_TAG_LEN);
        p += TRELLIS_FRAME_TAG_LEN;
    }
    if (vi_off > 0)
        randombytes_buf(p, vi_off);
    p += vi_off;
    trellis_varint_encode(p, (uint64_t)len);
    p += hdr_len;
    memcpy(p, data, len);

    // Apply wire morph (metamorphic encoding over the frame)
    if (conn->wire_morph) {
        trellis_buf_t morphed = {0};
        if (trellis_morph_encode(conn->wire_morph, raw, raw_total,
                                 &morphed) == TRELLIS_OK && morphed.data) {
            free(raw);
            raw = morphed.data;
            raw_total = morphed.len;
        }
    }

    // Apply wire camouflage (protocol mimicry wrapping)
    if (conn->wire_camo) {
        trellis_buf_t camoed = {0};
        if (trellis_camouflage_wrap(conn->wire_camo, raw, raw_total,
                                    &camoed) == TRELLIS_OK && camoed.data) {
            free(raw);
            raw = camoed.data;
            raw_total = camoed.len;
        }
    }

    *out = raw;
    return raw_total;
}

// Parse "host:port" or bare "port" into sockaddr. Returns 0 on success.
static inline int trellis_parse_addr(const char *addr,
                                     struct sockaddr_storage *out)
{
    if (!addr || !out)
        return -1;

    char host[TRELLIS_ADDR_MAX];
    int port;
    const char *colon = strrchr(addr, ':');

    if (!colon) {
        // Bare port number -- bind to all interfaces.
        port = atoi(addr);
        if (port < 0 || port > 65535)
            return -1;
        memcpy(host, "0.0.0.0", 8);
    } else {
        size_t host_len = (size_t)(colon - addr);
        if (host_len >= sizeof(host))
            return -1;
        memcpy(host, addr, host_len);
        host[host_len] = '\0';
        port = atoi(colon + 1);
        if (port < 0 || port > 65535)
            return -1;
    }

    struct sockaddr_in addr4;
    if (uv_ip4_addr(host, port, &addr4) == 0) {
        memcpy(out, &addr4, sizeof(addr4));
        return 0;
    }

    struct sockaddr_in6 addr6;
    if (uv_ip6_addr(host, port, &addr6) == 0) {
        memcpy(out, &addr6, sizeof(addr6));
        return 0;
    }

    return -1;
}

#ifdef __cplusplus
}
#endif

#endif /* TRELLIS_TRANSPORT_INTERNAL_H */
