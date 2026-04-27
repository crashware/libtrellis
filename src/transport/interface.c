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
#include <trellis/morph.h>
#include <trellis/camouflage.h>
#include <sodium.h>
#include <assert.h>

static void send_queue_drain(trellis_conn_t *conn);

static void send_queue_write_done(trellis_err_t err, void *ctx)
{
    trellis_conn_t *conn = (trellis_conn_t *)ctx;
    conn->send_queue_writing = false;

    if (conn->send_queue_full &&
        conn->send_queue_bytes <= TRELLIS_SEND_QUEUE_LO)
        conn->send_queue_full = false;

    send_queue_drain(conn);
}

static void send_queue_drain(trellis_conn_t *conn)
{
    assert(conn && "send_queue_drain called with NULL conn");
    if (conn->send_queue_writing || !conn->send_queue_head) {
        // Queue empty and not writing: complete graceful close if pending.
        if (!conn->send_queue_head && !conn->send_queue_writing &&
            conn->closing && conn->vtable && conn->vtable->close) {
            conn->vtable->close(conn, conn->close_cb, conn->close_ctx);
        }
        return;
    }

    trellis_send_buf_t *sb = conn->send_queue_head;
    conn->send_queue_head = sb->next;
    if (!conn->send_queue_head)
        conn->send_queue_tail = NULL;
    conn->send_queue_bytes -= sb->len;

    conn->send_queue_writing = true;

    trellis_conn_send_cb orig_cb = sb->cb;
    void *orig_ctx = sb->ctx;
    uint8_t *buf = sb->data;
    size_t buf_len = sb->len;
    free(sb);

    trellis_err_t err = conn->vtable->send(conn, buf, buf_len,
                                            send_queue_write_done, conn);
    if (err != TRELLIS_OK) {
        conn->send_queue_writing = false;
        if (orig_cb)
            orig_cb(err, orig_ctx);
    }
    free(buf);
    (void)orig_cb;
    (void)orig_ctx;
}

trellis_err_t trellis_conn_send(trellis_conn_t *conn,
                                const uint8_t *data, size_t len,
                                trellis_conn_send_cb cb, void *ctx)
{
    if (!conn || !conn->vtable || !conn->vtable->send)
        return TRELLIS_ERR_INVALID_ARG;

    if (conn->closing)
        return TRELLIS_ERR_INVALID_ARG;

    // If nothing queued and no write in-flight, send directly.
    if (!conn->send_queue_head && !conn->send_queue_writing) {
        return conn->vtable->send(conn, data, len, cb, ctx);
    }

    // Backpressure: reject when above high-water mark.
    if (conn->send_queue_bytes + len > TRELLIS_SEND_QUEUE_HI) {
        conn->send_queue_full = true;
        return TRELLIS_ERR_RATE_LIMITED;
    }

    // Enqueue
    trellis_send_buf_t *sb = malloc(sizeof(*sb));
    if (!sb)
        return TRELLIS_ERR_NOMEM;
    sb->data = malloc(len);
    if (!sb->data) {
        free(sb);
        return TRELLIS_ERR_NOMEM;
    }
    memcpy(sb->data, data, len);
    sb->len = len;
    sb->cb = cb;
    sb->ctx = ctx;
    sb->next = NULL;

    if (conn->send_queue_tail)
        conn->send_queue_tail->next = sb;
    else
        conn->send_queue_head = sb;
    conn->send_queue_tail = sb;
    conn->send_queue_bytes += len;

    send_queue_drain(conn);
    return TRELLIS_OK;
}

trellis_err_t trellis_conn_recv_start(trellis_conn_t *conn,
                                      trellis_conn_recv_cb cb, void *ctx)
{
    if (!conn || !conn->vtable || !conn->vtable->recv_start)
        return TRELLIS_ERR_INVALID_ARG;
    return conn->vtable->recv_start(conn, cb, ctx);
}

void trellis_conn_recv_stop(trellis_conn_t *conn)
{
    if (!conn || !conn->vtable || !conn->vtable->recv_stop)
        return;
    conn->vtable->recv_stop(conn);
}

void trellis_conn_close(trellis_conn_t *conn,
                        trellis_conn_close_cb cb, void *ctx)
{
    if (!conn || !conn->vtable || !conn->vtable->close)
        return;
    conn->vtable->close(conn, cb, ctx);
}

void trellis_conn_close_graceful(trellis_conn_t *conn,
                                 trellis_conn_close_cb cb, void *ctx)
{
    if (!conn || !conn->vtable || !conn->vtable->close)
        return;

    conn->closing = true;
    conn->close_cb = cb;
    conn->close_ctx = ctx;

    // If nothing queued, close immediately.
    if (!conn->send_queue_head && !conn->send_queue_writing) {
        conn->vtable->close(conn, cb, ctx);
        return;
    }

    // Otherwise, send_queue_drain will call close when the queue empties.
}

const char *trellis_conn_remote_addr(const trellis_conn_t *conn)
{
    return conn ? conn->remote_addr : NULL;
}

const char *trellis_conn_local_addr(const trellis_conn_t *conn)
{
    return conn ? conn->local_addr : NULL;
}

const char *trellis_conn_transport(const trellis_conn_t *conn)
{
    return conn ? conn->transport_name : NULL;
}

void trellis_conn_set_frame_tag(trellis_conn_t *conn,
                                const uint8_t *tag)
{
    if (!conn || !tag)
        return;
    memcpy(conn->frame_tag, tag, TRELLIS_FRAME_TAG_LEN);
    conn->has_frame_tag = true;
    conn->frame_tag_sent = false;
    conn->frame_tag_validated = false;
}

void trellis_conn_derive_frame_tag(trellis_conn_t *conn,
                                   const uint8_t *session_key,
                                   size_t key_len)
{
    if (!conn || !session_key || key_len == 0)
        return;
    crypto_auth_hmacsha256_state st;
    uint8_t digest[32];
    crypto_auth_hmacsha256_init(&st, session_key, key_len);
    crypto_auth_hmacsha256_update(&st,
        (const uint8_t *)"bloom-frame-tag", 15);
    crypto_auth_hmacsha256_final(&st, digest);
    memcpy(conn->frame_tag, digest, TRELLIS_FRAME_TAG_LEN);
    sodium_memzero(&st, sizeof(st));
    sodium_memzero(digest, sizeof(digest));
    conn->has_frame_tag = true;
    conn->frame_tag_sent = false;
    conn->frame_tag_validated = false;

    uint8_t offset_digest[32];
    crypto_auth_hmacsha256_state st2;
    crypto_auth_hmacsha256_init(&st2, session_key, key_len);
    crypto_auth_hmacsha256_update(&st2,
        (const uint8_t *)"bloom-varint-offset", 19);
    crypto_auth_hmacsha256_final(&st2, offset_digest);
    conn->frame_varint_offset = offset_digest[0] & 0x0F;
    sodium_memzero(&st2, sizeof(st2));
    sodium_memzero(offset_digest, sizeof(offset_digest));
}

void trellis_conn_set_wire_morph(trellis_conn_t *conn, trellis_morph_t *morph)
{
    if (conn) conn->wire_morph = morph;
}

void trellis_conn_set_wire_camo(trellis_conn_t *conn, trellis_camouflage_t *camo)
{
    if (conn) conn->wire_camo = camo;
}

trellis_err_t trellis_transport_connect(trellis_transport_t *t,
                                        const char *addr,
                                        trellis_connect_cb cb, void *ctx)
{
    if (!t || !t->vtable || !t->vtable->connect)
        return TRELLIS_ERR_INVALID_ARG;
    return t->vtable->connect(t, addr, cb, ctx);
}

trellis_err_t trellis_transport_listen(trellis_transport_t *t,
                                       const char *addr,
                                       trellis_accept_cb cb, void *ctx)
{
    if (!t || !t->vtable || !t->vtable->listen)
        return TRELLIS_ERR_INVALID_ARG;
    return t->vtable->listen(t, addr, cb, ctx);
}

trellis_err_t trellis_transport_stop(trellis_transport_t *t)
{
    if (!t || !t->vtable || !t->vtable->stop)
        return TRELLIS_ERR_INVALID_ARG;
    return t->vtable->stop(t);
}

const char *trellis_transport_protocol(const trellis_transport_t *t)
{
    return t ? t->protocol_name : NULL;
}

int trellis_transport_bound_port(trellis_transport_t *t)
{
    if (!t || !t->impl_data)
        return -1;
    /* The bound_port field is at a known offset in both tcp and udp
     * transport data structs.  For now, read via the struct layout
     * that stores it after the listening flag. */
    struct { char _pad[sizeof(void *)]; int port; } *p;
    (void)p;

    // TCP: bound_port is right after the 'listening' bool.
    if (strcmp(t->protocol_name, "tcp") == 0) {
        // Reach into tcp_transport_data_t which has bound_port at end.
        struct sockaddr_storage ss;
        int len = sizeof(ss);
        // The first member of tcp_transport_data_t is uv_tcp_t server.
        uv_tcp_t *server = (uv_tcp_t *)t->impl_data;
        if (uv_tcp_getsockname(server, (struct sockaddr *)&ss, &len) == 0) {
            struct sockaddr_in *s4 = (struct sockaddr_in *)&ss;
            return ntohs(s4->sin_port);
        }
    } else if (strcmp(t->protocol_name, "udp") == 0) {
        uv_udp_t *handle = (uv_udp_t *)t->impl_data;
        struct sockaddr_storage ss;
        int len = sizeof(ss);
        if (uv_udp_getsockname(handle, (struct sockaddr *)&ss, &len) == 0) {
            struct sockaddr_in *s4 = (struct sockaddr_in *)&ss;
            return ntohs(s4->sin_port);
        }
    } else if (strcmp(t->protocol_name, "tls") == 0 ||
               strcmp(t->protocol_name, "pt") == 0) {
        uv_tcp_t *server = (uv_tcp_t *)t->impl_data;
        struct sockaddr_storage ss;
        int len = sizeof(ss);
        if (uv_tcp_getsockname(server, (struct sockaddr *)&ss, &len) == 0) {
            struct sockaddr_in *s4 = (struct sockaddr_in *)&ss;
            return ntohs(s4->sin_port);
        }
    }
    return -1;
}

void trellis_transport_free(trellis_transport_t *t)
{
    if (!t)
        return;
    if (t->vtable && t->vtable->free)
        t->vtable->free(t);
    free(t);
}
