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

#include <stdio.h>

typedef struct {
    trellis_reverse_proxy_t *rp;
    trellis_conn_t          *tunnel;
    uv_tcp_t                 backend_conn;
    bool                     backend_connected;
} rproxy_session_t;

static void on_backend_read(uv_stream_t *stream, ssize_t nread,
                            const uv_buf_t *buf)
{
    rproxy_session_t *sess = stream->data;

    if (nread <= 0) {
        if (buf->base)
            free(buf->base);
        return;
    }

    trellis_conn_send(sess->tunnel, (const uint8_t *)buf->base,
                      (size_t)nread, NULL, NULL);
    free(buf->base);
}

static void on_backend_alloc(uv_handle_t *handle, size_t suggested,
                             uv_buf_t *buf)
{
    (void)handle;
    buf->base = malloc(suggested);
    buf->len = buf->base ? suggested : 0;
}

static void rproxy_sess_close_cb(uv_handle_t *h)
{
    rproxy_session_t *sess = h->data;
    free(sess);
}

static void on_backend_connect(uv_connect_t *req, int status)
{
    rproxy_session_t *sess = req->data;
    free(req);

    if (status < 0) {
        sess->backend_conn.data = sess;
        uv_close((uv_handle_t *)&sess->backend_conn, rproxy_sess_close_cb);
        return;
    }

    sess->backend_connected = true;
    sess->backend_conn.data = sess;
    uv_read_start((uv_stream_t *)&sess->backend_conn,
                  on_backend_alloc, on_backend_read);
}

static void rproxy_write_done_cb(uv_write_t *req, int status)
{
    (void)status;
    free(req->data);
    free(req);
}

static void on_tunnel_data(trellis_conn_t *conn, const uint8_t *data,
                           size_t len, trellis_err_t err, void *ctx)
{
    rproxy_session_t *sess = ctx;
    (void)conn;

    if (err != TRELLIS_OK || !data || len == 0) {
        if (sess->backend_connected) {
            sess->backend_conn.data = sess;
            uv_close((uv_handle_t *)&sess->backend_conn, rproxy_sess_close_cb);
        } else {
            free(sess);
        }
        return;
    }

    if (!sess->backend_connected)
        return;

    uint8_t *copy = malloc(len);
    if (!copy)
        return;
    memcpy(copy, data, len);

    uv_buf_t buf = uv_buf_init((char *)copy, (unsigned int)len);
    uv_write_t *wreq = malloc(sizeof(*wreq));
    if (!wreq) {
        free(copy);
        return;
    }
    wreq->data = copy;

    uv_write(wreq, (uv_stream_t *)&sess->backend_conn, &buf, 1,
             rproxy_write_done_cb);
}

static void rproxy_on_connection(trellis_conn_t *tunnel,
                                 const trellis_fingerprint_t *client,
                                 void *ctx)
{
    trellis_reverse_proxy_t *rp = ctx;
    (void)client;

    if (!rp->running || !tunnel)
        return;

    rproxy_session_t *sess = calloc(1, sizeof(*sess));
    if (!sess)
        return;

    sess->rp = rp;
    sess->tunnel = tunnel;

    uv_tcp_init(rp->loop, &sess->backend_conn);

    struct sockaddr_in dest;
    char host[128];
    int port = 8080;

    if (sscanf(rp->backend_addr, "%127[^:]:%d", host, &port) < 1) {
        sess->backend_conn.data = sess;
        uv_close((uv_handle_t *)&sess->backend_conn, rproxy_sess_close_cb);
        return;
    }

    uv_ip4_addr(host, port, &dest);

    uv_connect_t *creq = malloc(sizeof(*creq));
    if (!creq) {
        sess->backend_conn.data = sess;
        uv_close((uv_handle_t *)&sess->backend_conn, rproxy_sess_close_cb);
        return;
    }
    creq->data = sess;

    int r = uv_tcp_connect(creq, &sess->backend_conn,
                           (const struct sockaddr *)&dest,
                           on_backend_connect);
    if (r != 0) {
        free(creq);
        sess->backend_conn.data = sess;
        uv_close((uv_handle_t *)&sess->backend_conn, rproxy_sess_close_cb);
        return;
    }

    trellis_conn_recv_start(tunnel, on_tunnel_data, sess);
}

trellis_reverse_proxy_t *trellis_reverse_proxy_new(trellis_greenhouse_t *gh,
                                                   uv_loop_t *loop,
                                                   const char *backend_addr)
{
    if (!gh || !loop || !backend_addr)
        return NULL;

    trellis_reverse_proxy_t *rp = calloc(1, sizeof(*rp));
    if (!rp)
        return NULL;

    rp->greenhouse = gh;
    rp->loop = loop;

    size_t addr_len = strlen(backend_addr);
    if (addr_len >= sizeof(rp->backend_addr))
        addr_len = sizeof(rp->backend_addr) - 1;
    memcpy(rp->backend_addr, backend_addr, addr_len);
    rp->backend_addr[addr_len] = '\0';

    return rp;
}

trellis_err_t trellis_reverse_proxy_start(trellis_reverse_proxy_t *rp)
{
    if (!rp)
        return TRELLIS_ERR_INVALID_ARG;

    if (rp->running)
        return TRELLIS_ERR_ALREADY_EXISTS;

    trellis_err_t err = trellis_greenhouse_start(rp->greenhouse,
                                                 rproxy_on_connection, rp);
    if (err != TRELLIS_OK)
        return err;

    rp->running = true;
    return TRELLIS_OK;
}

trellis_err_t trellis_reverse_proxy_stop(trellis_reverse_proxy_t *rp)
{
    if (!rp)
        return TRELLIS_ERR_INVALID_ARG;

    if (!rp->running)
        return TRELLIS_OK;

    trellis_greenhouse_stop(rp->greenhouse);
    rp->running = false;
    return TRELLIS_OK;
}

void trellis_reverse_proxy_free(trellis_reverse_proxy_t *rp)
{
    if (!rp)
        return;

    if (rp->running)
        trellis_reverse_proxy_stop(rp);

    free(rp);
}
