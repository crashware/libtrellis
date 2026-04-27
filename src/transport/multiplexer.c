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

#include <assert.h>
#include <sodium.h>
#include <stdlib.h>
#include <string.h>

/*
 * Stream multiplexer (Section 4.3).
 *
 * When an encryption key is set, the stream ID is encrypted inside the
 * payload rather than sent as a cleartext header:
 *
 *   Encrypted mode: [XChaCha20(stream_id || payload, nonce)] [24-byte nonce]
 *   Plaintext mode: [2-byte stream_id][payload]
 *
 * Encrypted mode hides the multiplexing structure from DPI, making it
 * impossible to distinguish individual streams or count them.
 */
#define MUX_HEADER_LEN    2
#define MUX_MAX_STREAMS 256
#define MUX_NONCE_LEN     24
#define MUX_TAG_LEN       16

struct trellis_mux_stream {
    trellis_mux_t        *mux;
    uint16_t              stream_id;
    trellis_conn_recv_cb  recv_cb;
    void                 *recv_ctx;
    bool                  open;
    bool                  paused;    /* flow control: suppress recv_cb delivery */
    // Backlog for buffering data that arrives while paused.
    uint8_t              *backlog;
    size_t                backlog_len;
    size_t                backlog_cap;
};

struct trellis_mux {
    trellis_conn_t        *conn;
    trellis_mux_stream_t  *streams[MUX_MAX_STREAMS];
    uint8_t                encrypt_key[32];
    bool                   encrypted;
};

static void mux_on_recv(trellis_conn_t *conn,
                         const uint8_t *data, size_t len,
                         trellis_err_t err, void *ctx)
{
    trellis_mux_t *mux = ctx;
    assert(mux && "mux_on_recv with NULL mux");
    (void)conn;

    if (err != TRELLIS_OK) {
        for (int i = 0; i < MUX_MAX_STREAMS; i++) {
            trellis_mux_stream_t *s = mux->streams[i];
            if (s && s->open && s->recv_cb)
                s->recv_cb(NULL, NULL, 0, err, s->recv_ctx);
        }
        return;
    }

    if (mux->encrypted) {
        // Encrypted mode: [ciphertext + tag][24-byte nonce]
        if (len < MUX_NONCE_LEN + MUX_TAG_LEN + MUX_HEADER_LEN)
            return;

        size_t ct_len = len - MUX_NONCE_LEN;
        const uint8_t *ct = data;
        const uint8_t *nonce = data + ct_len;

        uint8_t *pt = malloc(ct_len);
        if (!pt) return;

        unsigned long long pt_len;
        if (crypto_aead_xchacha20poly1305_ietf_decrypt(
                pt, &pt_len, NULL, ct, ct_len,
                NULL, 0, nonce, mux->encrypt_key) != 0) {
            free(pt);
            return;
        }

        if (pt_len < MUX_HEADER_LEN) { free(pt); return; }

        uint16_t stream_id = ((uint16_t)pt[0] << 8) | pt[1];
        if (stream_id >= MUX_MAX_STREAMS) { free(pt); return; }

        trellis_mux_stream_t *s = mux->streams[stream_id];
        if (s && s->open) {
            const uint8_t *payload = pt + MUX_HEADER_LEN;
            size_t payload_len = (size_t)pt_len - MUX_HEADER_LEN;
            if (s->paused) {
                if (payload_len <= SIZE_MAX - s->backlog_len) {
                    size_t need = s->backlog_len + payload_len;
                    if (need > s->backlog_cap) {
                        size_t new_cap = (need <= SIZE_MAX / 2) ? need * 2 : need;
                        uint8_t *nb = realloc(s->backlog, new_cap);
                        if (nb) { s->backlog = nb; s->backlog_cap = new_cap; }
                    }
                    if (s->backlog && s->backlog_len + payload_len <= s->backlog_cap) {
                        memcpy(s->backlog + s->backlog_len, payload, payload_len);
                        s->backlog_len += payload_len;
                    }
                }
            } else if (s->recv_cb) {
                s->recv_cb(mux->conn, payload, payload_len, TRELLIS_OK, s->recv_ctx);
            }
        }
        sodium_memzero(pt, (size_t)pt_len);
        free(pt);
        return;
    }

    // Plaintext mode.
    if (len < MUX_HEADER_LEN)
        return;

    uint16_t stream_id = ((uint16_t)data[0] << 8) | data[1];
    if (stream_id >= MUX_MAX_STREAMS)
        return;

    trellis_mux_stream_t *s = mux->streams[stream_id];
    if (!s || !s->open)
        return;

    if (s->paused) {
        size_t payload_len = len - MUX_HEADER_LEN;
        if (payload_len > SIZE_MAX - s->backlog_len)
            return;
        size_t need = s->backlog_len + payload_len;
        if (need > s->backlog_cap) {
            size_t new_cap = (need <= SIZE_MAX / 2) ? need * 2 : need;
            uint8_t *nb = realloc(s->backlog, new_cap);
            if (nb) { s->backlog = nb; s->backlog_cap = new_cap; }
        }
        if (s->backlog && s->backlog_len + payload_len <= s->backlog_cap) {
            memcpy(s->backlog + s->backlog_len,
                   data + MUX_HEADER_LEN, payload_len);
            s->backlog_len += payload_len;
        }
        return;
    }

    if (!s->recv_cb)
        return;

    s->recv_cb(mux->conn, data + MUX_HEADER_LEN, len - MUX_HEADER_LEN,
               TRELLIS_OK, s->recv_ctx);
}

trellis_mux_t *trellis_mux_new(trellis_conn_t *conn,
                               const uint8_t *key, size_t key_len)
{
    if (!conn)
        return NULL;

    trellis_mux_t *mux = calloc(1, sizeof(*mux));
    if (!mux)
        return NULL;

    mux->conn = conn;

    if (key && key_len > 0)
        trellis_mux_set_encrypt_key(mux, key, key_len);

    trellis_err_t rc = trellis_conn_recv_start(conn, mux_on_recv, mux);
    if (rc != TRELLIS_OK) {
        free(mux);
        return NULL;
    }

    return mux;
}

void trellis_mux_free(trellis_mux_t *mux)
{
    if (!mux)
        return;

    trellis_conn_recv_stop(mux->conn);

    for (int i = 0; i < MUX_MAX_STREAMS; i++) {
        if (mux->streams[i]) {
            mux->streams[i]->open = false;
            free(mux->streams[i]);
        }
    }

    sodium_memzero(mux->encrypt_key, sizeof(mux->encrypt_key));
    free(mux);
}

trellis_mux_stream_t *trellis_mux_open(trellis_mux_t *mux, uint16_t stream_id)
{
    if (!mux || stream_id >= MUX_MAX_STREAMS)
        return NULL;

    if (mux->streams[stream_id]) {
        if (mux->streams[stream_id]->open)
            return mux->streams[stream_id];
        free(mux->streams[stream_id]);
        mux->streams[stream_id] = NULL;
    }

    trellis_mux_stream_t *s = calloc(1, sizeof(*s));
    if (!s)
        return NULL;

    s->mux = mux;
    s->stream_id = stream_id;
    s->open = true;
    mux->streams[stream_id] = s;
    return s;
}

typedef struct mux_send_ctx {
    uint8_t              *frame;
    trellis_conn_send_cb  user_cb;
    void                 *user_ctx;
} mux_send_ctx_t;

static void mux_send_done(trellis_err_t err, void *ctx)
{
    mux_send_ctx_t *mc = ctx;
    trellis_conn_send_cb user_cb = mc->user_cb;
    void *user_ctx = mc->user_ctx;
    free(mc->frame);
    free(mc);
    if (user_cb)
        user_cb(err, user_ctx);
}

trellis_err_t trellis_mux_stream_send(trellis_mux_stream_t *stream,
                                      const uint8_t *data, size_t len,
                                      trellis_conn_send_cb cb, void *ctx)
{
    if (!stream || !stream->open)
        return TRELLIS_ERR_CLOSED;

    trellis_mux_t *mux = stream->mux;

    if (mux->encrypted) {
        size_t pt_len = MUX_HEADER_LEN + len;
        uint8_t *pt = malloc(pt_len);
        if (!pt) return TRELLIS_ERR_NOMEM;

        pt[0] = (uint8_t)(stream->stream_id >> 8);
        pt[1] = (uint8_t)(stream->stream_id);
        if (len > 0) memcpy(pt + MUX_HEADER_LEN, data, len);

        size_t frame_len = pt_len + MUX_TAG_LEN + MUX_NONCE_LEN;
        uint8_t *frame = malloc(frame_len);
        if (!frame) { free(pt); return TRELLIS_ERR_NOMEM; }

        uint8_t *nonce = frame + pt_len + MUX_TAG_LEN;
        randombytes_buf(nonce, MUX_NONCE_LEN);

        unsigned long long ct_len;
        if (crypto_aead_xchacha20poly1305_ietf_encrypt(
                frame, &ct_len, pt, pt_len, NULL, 0, NULL,
                nonce, mux->encrypt_key) != 0) {
            free(pt);
            free(frame);
            return TRELLIS_ERR_ENCRYPT;
        }
        free(pt);

        mux_send_ctx_t *mc = malloc(sizeof(*mc));
        if (!mc) { free(frame); return TRELLIS_ERR_NOMEM; }
        mc->frame = frame;   /* ownership transferred; mux_send_done frees it */
        mc->user_cb = cb;
        mc->user_ctx = ctx;

        trellis_err_t rc = trellis_conn_send(mux->conn, frame,
                                             frame_len, mux_send_done, mc);
        if (rc != TRELLIS_OK) {
            mc->frame = NULL; /* prevent double-free in mux_send_done */
            free(frame);
            free(mc);
        }
        return rc;
    }

    // Plaintext mode.
    size_t frame_len = MUX_HEADER_LEN + len;
    uint8_t *frame = malloc(frame_len);
    if (!frame)
        return TRELLIS_ERR_NOMEM;

    frame[0] = (uint8_t)(stream->stream_id >> 8);
    frame[1] = (uint8_t)(stream->stream_id);
    if (len > 0)
        memcpy(frame + MUX_HEADER_LEN, data, len);

    mux_send_ctx_t *mc = malloc(sizeof(*mc));
    if (!mc) {
        free(frame);
        return TRELLIS_ERR_NOMEM;
    }
    mc->frame = frame;
    mc->user_cb = cb;
    mc->user_ctx = ctx;

    trellis_err_t rc = trellis_conn_send(mux->conn,
                                         frame, frame_len,
                                         mux_send_done, mc);
    if (rc != TRELLIS_OK) {
        free(frame);
        free(mc);
    }
    return rc;
}

void trellis_mux_set_encrypt_key(trellis_mux_t *mux,
                                 const uint8_t *key, size_t key_len)
{
    if (!mux || !key || key_len == 0) return;
    crypto_generichash(mux->encrypt_key, sizeof(mux->encrypt_key),
                       key, key_len,
                       (const uint8_t *)"bloom-mux-key", 13);
    mux->encrypted = true;
}

void trellis_mux_stream_close(trellis_mux_stream_t *stream)
{
    if (!stream)
        return;

    stream->open = false;
    stream->recv_cb = NULL;
    stream->recv_ctx = NULL;

    free(stream->backlog);
    stream->backlog = NULL;
    stream->backlog_len = 0;
    stream->backlog_cap = 0;

    if (stream->mux)
        stream->mux->streams[stream->stream_id] = NULL;

    free(stream);
}

trellis_err_t trellis_mux_stream_recv_start(trellis_mux_stream_t *stream,
                                             trellis_conn_recv_cb cb, void *ctx)
{
    if (!stream || !cb)
        return TRELLIS_ERR_INVALID_ARG;
    if (!stream->open)
        return TRELLIS_ERR_CLOSED;

    stream->recv_cb  = cb;
    stream->recv_ctx = ctx;
    return TRELLIS_OK;
}

void trellis_mux_stream_recv_stop(trellis_mux_stream_t *stream)
{
    if (!stream)
        return;
    stream->recv_cb  = NULL;
    stream->recv_ctx = NULL;
}

/*
 * Pause delivery: incoming data is buffered until trellis_mux_stream_resume().
 * The caller should pause a stream when it is not ready to consume data (e.g.
 * back-pressure from a slow downstream sink) to avoid losing frames.
 */
trellis_err_t trellis_mux_stream_pause(trellis_mux_stream_t *stream)
{
    if (!stream)
        return TRELLIS_ERR_INVALID_ARG;
    stream->paused = true;
    return TRELLIS_OK;
}

/*
 * Resume delivery: drain any backlogged data through recv_cb, then clear
 * the paused flag so future data is delivered immediately.
 */
trellis_err_t trellis_mux_stream_resume(trellis_mux_stream_t *stream)
{
    if (!stream)
        return TRELLIS_ERR_INVALID_ARG;

    stream->paused = false;

    if (stream->backlog_len > 0 && stream->recv_cb) {
        stream->recv_cb(stream->mux->conn, stream->backlog, stream->backlog_len,
                        TRELLIS_OK, stream->recv_ctx);
        free(stream->backlog);
        stream->backlog = NULL;
        stream->backlog_len = 0;
        stream->backlog_cap = 0;
    }

    return TRELLIS_OK;
}
