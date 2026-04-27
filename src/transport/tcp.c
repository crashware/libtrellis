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

typedef struct tcp_conn_data {
    uv_tcp_t            *handle;
    trellis_conn_t      *conn;

    trellis_conn_recv_cb  recv_cb;
    void                 *recv_ctx;

    uint8_t             *recv_buf;
    size_t               recv_buf_len;
    size_t               recv_buf_cap;
} tcp_conn_data_t;

typedef struct tcp_transport_data {
    uv_tcp_t          server;
    trellis_accept_cb accept_cb;
    void             *accept_ctx;
    trellis_transport_t *transport;
    bool               listening;
    int                bound_port;
} tcp_transport_data_t;

typedef struct tcp_write_req {
    uv_write_t          req;
    uint8_t            *frame;
    trellis_conn_send_cb cb;
    void               *ctx;
} tcp_write_req_t;

typedef struct tcp_connect_req {
    uv_connect_t       req;
    uv_tcp_t          *handle;
    trellis_connect_cb cb;
    void              *ctx;
    trellis_transport_t *transport;
} tcp_connect_req_t;

static const trellis_conn_vtable_t      tcp_conn_vtable;
static const trellis_transport_vtable_t tcp_transport_vtable;

static trellis_conn_t *tcp_conn_create(uv_tcp_t *handle,
                                       trellis_transport_t *t)
{
    trellis_conn_t *conn = trellis_conn_alloc(&tcp_conn_vtable, "tcp", t->loop);
    if (!conn)
        return NULL;

    tcp_conn_data_t *cd = calloc(1, sizeof(*cd));
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

static void tcp_deliver_frames(tcp_conn_data_t *cd)
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

static void tcp_alloc_cb(uv_handle_t *handle, size_t suggested,
                          uv_buf_t *buf)
{
    tcp_conn_data_t *cd = handle->data;
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

static void tcp_read_cb(uv_stream_t *stream, ssize_t nread,
                         const uv_buf_t *buf)
{
    tcp_conn_data_t *cd = stream->data;
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
    tcp_deliver_frames(cd);
}

static void tcp_on_write(uv_write_t *req, int status)
{
    tcp_write_req_t *wr = req->data;
    if (wr->cb) {
        trellis_err_t err = (status == 0) ? TRELLIS_OK : TRELLIS_ERR_SEND;
        wr->cb(err, wr->ctx);
    }
    free(wr->frame);
    free(wr);
}

static trellis_err_t tcp_conn_send(trellis_conn_t *conn,
                                   const uint8_t *data, size_t len,
                                   trellis_conn_send_cb cb, void *ctx)
{
    tcp_conn_data_t *cd = conn->impl_data;

    tcp_write_req_t *wr = malloc(sizeof(*wr));
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

    uv_buf_t uvbuf = uv_buf_init((char *)wr->frame, (unsigned int)frame_len);

    int rc = uv_write(&wr->req, (uv_stream_t *)cd->handle,
                      &uvbuf, 1, tcp_on_write);
    if (rc != 0) {
        free(wr->frame);
        free(wr);
        return TRELLIS_ERR_SEND;
    }

    return TRELLIS_OK;
}

static trellis_err_t tcp_conn_recv_start(trellis_conn_t *conn,
                                         trellis_conn_recv_cb cb, void *ctx)
{
    tcp_conn_data_t *cd = conn->impl_data;
    cd->recv_cb = cb;
    cd->recv_ctx = ctx;

    int rc = uv_read_start((uv_stream_t *)cd->handle,
                           tcp_alloc_cb, tcp_read_cb);
    return (rc == 0) ? TRELLIS_OK : TRELLIS_ERR_TRANSPORT;
}

static void tcp_conn_recv_stop(trellis_conn_t *conn)
{
    tcp_conn_data_t *cd = conn->impl_data;
    uv_read_stop((uv_stream_t *)cd->handle);
    cd->recv_cb = NULL;
    cd->recv_ctx = NULL;
}

typedef struct tcp_close_ctx {
    trellis_conn_close_cb cb;
    void                 *ctx;
    trellis_conn_t       *conn;
    tcp_conn_data_t      *cd;
} tcp_close_ctx_t;

static void tcp_on_close(uv_handle_t *handle)
{
    tcp_close_ctx_t *cc = handle->data;

    trellis_conn_close_cb cb = cc->cb;
    void *ctx = cc->ctx;
    trellis_conn_t *conn = cc->conn;
    tcp_conn_data_t *cd = cc->cd;

    free(cd->recv_buf);
    free(cd->handle);
    free(cc);
    free(cd);
    conn->impl_data = NULL;

    if (cb)
        cb(conn, ctx);

    trellis_conn_dealloc(conn);
}

static void tcp_conn_close(trellis_conn_t *conn,
                            trellis_conn_close_cb cb, void *ctx)
{
    tcp_conn_data_t *cd = conn->impl_data;
    if (!cd)
        return;

    tcp_close_ctx_t *cc = malloc(sizeof(*cc));
    if (!cc) {
        if (cb)
            cb(conn, ctx);
        return;
    }
    cc->cb = cb;
    cc->ctx = ctx;
    cc->conn = conn;
    cc->cd = cd;

    cd->handle->data = cc;
    uv_close((uv_handle_t *)cd->handle, tcp_on_close);
}

static const trellis_conn_vtable_t tcp_conn_vtable = {
    .send       = tcp_conn_send,
    .recv_start = tcp_conn_recv_start,
    .recv_stop  = tcp_conn_recv_stop,
    .close      = tcp_conn_close,
};

static void tcp_on_connect(uv_connect_t *req, int status)
{
    tcp_connect_req_t *cr = (tcp_connect_req_t *)req;

    if (status != 0) {
        uv_close((uv_handle_t *)cr->handle, (uv_close_cb)free);
        if (cr->cb)
            cr->cb(NULL, TRELLIS_ERR_CONNECT, cr->ctx);
        free(cr);
        return;
    }

    trellis_conn_t *conn = tcp_conn_create(cr->handle, cr->transport);
    if (!conn) {
        uv_close((uv_handle_t *)cr->handle, (uv_close_cb)free);
        if (cr->cb)
            cr->cb(NULL, TRELLIS_ERR_NOMEM, cr->ctx);
        free(cr);
        return;
    }

    if (cr->cb)
        cr->cb(conn, TRELLIS_OK, cr->ctx);
    free(cr);
}

static trellis_err_t tcp_transport_connect(trellis_transport_t *t,
                                           const char *addr,
                                           trellis_connect_cb cb, void *ctx)
{
    struct sockaddr_storage ss;
    if (trellis_parse_addr(addr, &ss) != 0)
        return TRELLIS_ERR_INVALID_ARG;

    tcp_connect_req_t *cr = calloc(1, sizeof(*cr));
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

    int rc = uv_tcp_init(t->loop, cr->handle);
    if (rc != 0) {
        free(cr->handle);
        free(cr);
        return TRELLIS_ERR_TRANSPORT;
    }

    rc = uv_tcp_connect(&cr->req, cr->handle,
                        (const struct sockaddr *)&ss, tcp_on_connect);
    if (rc != 0) {
        uv_close((uv_handle_t *)cr->handle, (uv_close_cb)free);
        free(cr);
        return TRELLIS_ERR_CONNECT;
    }

    return TRELLIS_OK;
}

static void tcp_on_connection(uv_stream_t *server, int status)
{
    tcp_transport_data_t *td = server->data;
    if (status < 0)
        return;

    uv_tcp_t *client = malloc(sizeof(*client));
    if (!client)
        return;

    int rc = uv_tcp_init(td->transport->loop, client);
    if (rc != 0) {
        free(client);
        return;
    }

    rc = uv_accept(server, (uv_stream_t *)client);
    if (rc != 0) {
        uv_close((uv_handle_t *)client, (uv_close_cb)free);
        return;
    }

    trellis_conn_t *conn = tcp_conn_create(client, td->transport);
    if (!conn) {
        uv_close((uv_handle_t *)client, (uv_close_cb)free);
        return;
    }

    if (td->accept_cb)
        td->accept_cb(conn, td->accept_ctx);
}

static trellis_err_t tcp_transport_listen(trellis_transport_t *t,
                                          const char *addr,
                                          trellis_accept_cb cb, void *ctx)
{
    tcp_transport_data_t *td = t->impl_data;
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

    rc = uv_listen((uv_stream_t *)&td->server, 128, tcp_on_connection);
    if (rc != 0) {
        uv_close((uv_handle_t *)&td->server, NULL);
        return TRELLIS_ERR_LISTEN;
    }

    struct sockaddr_storage bound;
    int namelen = sizeof(bound);
    if (uv_tcp_getsockname(&td->server, (struct sockaddr *)&bound, &namelen) == 0) {
        struct sockaddr_in *s4 = (struct sockaddr_in *)&bound;
        td->bound_port = ntohs(s4->sin_port);
    }

    td->listening = true;
    return TRELLIS_OK;
}

static trellis_err_t tcp_transport_stop(trellis_transport_t *t)
{
    tcp_transport_data_t *td = t->impl_data;
    if (!td->listening)
        return TRELLIS_OK;

    uv_close((uv_handle_t *)&td->server, NULL);
    td->listening = false;
    return TRELLIS_OK;
}

static void tcp_transport_free(trellis_transport_t *t)
{
    tcp_transport_data_t *td = t->impl_data;
    if (td) {
        if (td->listening && !uv_is_closing((uv_handle_t *)&td->server))
            uv_close((uv_handle_t *)&td->server, NULL);
        free(td);
    }
}

static const trellis_transport_vtable_t tcp_transport_vtable = {
    .connect = tcp_transport_connect,
    .listen  = tcp_transport_listen,
    .stop    = tcp_transport_stop,
    .free    = tcp_transport_free,
};

trellis_transport_t *trellis_transport_tcp_new(uv_loop_t *loop)
{
    trellis_transport_t *t = trellis_transport_alloc(
        &tcp_transport_vtable, "tcp", loop);
    if (!t)
        return NULL;

    tcp_transport_data_t *td = calloc(1, sizeof(*td));
    if (!td) {
        free(t);
        return NULL;
    }

    td->transport = t;
    t->impl_data = td;
    return t;
}
