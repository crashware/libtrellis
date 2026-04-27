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

typedef struct udp_conn_data {
    uv_udp_t                *handle;       /* shared, owned by transport */
    struct sockaddr_storage  peer_addr;
    trellis_conn_recv_cb     recv_cb;
    void                    *recv_ctx;
    trellis_conn_t          *conn;
    bool                     receiving;
} udp_conn_data_t;

typedef struct udp_transport_data {
    uv_udp_t              handle;
    trellis_accept_cb     accept_cb;
    void                 *accept_ctx;
    trellis_transport_t  *transport;
    bool                  bound;

    udp_conn_data_t     **conns;
    size_t                conn_count;
    size_t                conn_cap;
} udp_transport_data_t;

typedef struct udp_send_req {
    uv_udp_send_t        req;
    uint8_t              *payload;
    trellis_conn_send_cb  cb;
    void                 *ctx;
} udp_send_req_t;

static const trellis_conn_vtable_t      udp_conn_vtable;
static const trellis_transport_vtable_t udp_transport_vtable;

static bool sockaddr_eq(const struct sockaddr *a, const struct sockaddr *b)
{
    if (a->sa_family != b->sa_family)
        return false;
    if (a->sa_family == AF_INET) {
        const struct sockaddr_in *a4 = (const struct sockaddr_in *)a;
        const struct sockaddr_in *b4 = (const struct sockaddr_in *)b;
        return a4->sin_port == b4->sin_port &&
               a4->sin_addr.s_addr == b4->sin_addr.s_addr;
    }
    const struct sockaddr_in6 *a6 = (const struct sockaddr_in6 *)a;
    const struct sockaddr_in6 *b6 = (const struct sockaddr_in6 *)b;
    return a6->sin6_port == b6->sin6_port &&
           memcmp(&a6->sin6_addr, &b6->sin6_addr, 16) == 0;
}

static udp_conn_data_t *udp_find_conn(udp_transport_data_t *td,
                                      const struct sockaddr *addr)
{
    for (size_t i = 0; i < td->conn_count; i++) {
        if (sockaddr_eq((struct sockaddr *)&td->conns[i]->peer_addr, addr))
            return td->conns[i];
    }
    return NULL;
}

static int udp_register_conn(udp_transport_data_t *td, udp_conn_data_t *cd)
{
    if (td->conn_count >= td->conn_cap) {
        size_t newcap = td->conn_cap ? td->conn_cap * 2 : 8;
        udp_conn_data_t **tmp = realloc(td->conns,
                                        newcap * sizeof(*tmp));
        if (!tmp)
            return -1;
        td->conns = tmp;
        td->conn_cap = newcap;
    }
    td->conns[td->conn_count++] = cd;
    return 0;
}

static void udp_unregister_conn(udp_transport_data_t *td, udp_conn_data_t *cd)
{
    for (size_t i = 0; i < td->conn_count; i++) {
        if (td->conns[i] == cd) {
            td->conns[i] = td->conns[--td->conn_count];
            return;
        }
    }
}

static trellis_conn_t *udp_conn_create(udp_transport_data_t *td,
                                       const struct sockaddr *peer)
{
    trellis_conn_t *conn = trellis_conn_alloc(&udp_conn_vtable, "udp",
                                              td->transport->loop);
    if (!conn)
        return NULL;

    udp_conn_data_t *cd = calloc(1, sizeof(*cd));
    if (!cd) {
        trellis_conn_dealloc(conn);
        return NULL;
    }

    cd->handle = &td->handle;
    memcpy(&cd->peer_addr, peer, sizeof(cd->peer_addr));
    cd->conn = conn;
    conn->impl_data = cd;

    struct sockaddr_storage local_ss;
    int namelen = sizeof(local_ss);
    if (uv_udp_getsockname(&td->handle,
                            (struct sockaddr *)&local_ss, &namelen) == 0)
        trellis_conn_set_addrs(conn, (struct sockaddr *)&local_ss, NULL);

    trellis_conn_set_addrs(conn, NULL, peer);

    if (udp_register_conn(td, cd) != 0) {
        free(cd);
        trellis_conn_dealloc(conn);
        return NULL;
    }

    return conn;
}

static void udp_alloc_cb(uv_handle_t *handle, size_t suggested,
                          uv_buf_t *buf)
{
    (void)handle;
    buf->base = malloc(suggested);
    buf->len = buf->base ? suggested : 0;
}

static void udp_recv_cb(uv_udp_t *handle, ssize_t nread,
                         const uv_buf_t *buf,
                         const struct sockaddr *addr, unsigned flags)
{
    udp_transport_data_t *td = handle->data;
    (void)flags;

    if (nread < 0) {
        free(buf->base);
        return;
    }

    if (nread == 0 || !addr) {
        free(buf->base);
        return;
    }

    udp_conn_data_t *cd = udp_find_conn(td, addr);

    if (!cd && td->accept_cb) {
        trellis_conn_t *conn = udp_conn_create(td, addr);
        if (conn) {
            td->accept_cb(conn, td->accept_ctx);
            cd = conn->impl_data;
        }
    }

    if (cd && cd->recv_cb && cd->receiving)
        cd->recv_cb(cd->conn, (const uint8_t *)buf->base, (size_t)nread,
                    TRELLIS_OK, cd->recv_ctx);

    free(buf->base);
}

static void udp_on_send(uv_udp_send_t *req, int status)
{
    udp_send_req_t *sr = (udp_send_req_t *)req;
    if (sr->cb) {
        trellis_err_t err = (status == 0) ? TRELLIS_OK : TRELLIS_ERR_SEND;
        sr->cb(err, sr->ctx);
    }
    free(sr->payload);
    free(sr);
}

static trellis_err_t udp_conn_send(trellis_conn_t *conn,
                                   const uint8_t *data, size_t len,
                                   trellis_conn_send_cb cb, void *ctx)
{
    udp_conn_data_t *cd = conn->impl_data;

    udp_send_req_t *sr = malloc(sizeof(*sr));
    if (!sr)
        return TRELLIS_ERR_NOMEM;

    sr->payload = malloc(len);
    if (!sr->payload) {
        free(sr);
        return TRELLIS_ERR_NOMEM;
    }
    memcpy(sr->payload, data, len);
    sr->cb = cb;
    sr->ctx = ctx;

    uv_buf_t uvbuf = uv_buf_init((char *)sr->payload, (unsigned int)len);

    int rc = uv_udp_send(&sr->req, cd->handle, &uvbuf, 1,
                         (const struct sockaddr *)&cd->peer_addr,
                         udp_on_send);
    if (rc != 0) {
        free(sr->payload);
        free(sr);
        return TRELLIS_ERR_SEND;
    }

    return TRELLIS_OK;
}

static trellis_err_t udp_conn_recv_start(trellis_conn_t *conn,
                                         trellis_conn_recv_cb cb, void *ctx)
{
    udp_conn_data_t *cd = conn->impl_data;
    cd->recv_cb = cb;
    cd->recv_ctx = ctx;
    cd->receiving = true;
    return TRELLIS_OK;
}

static void udp_conn_recv_stop(trellis_conn_t *conn)
{
    udp_conn_data_t *cd = conn->impl_data;
    cd->receiving = false;
    cd->recv_cb = NULL;
    cd->recv_ctx = NULL;
}

static void udp_conn_close(trellis_conn_t *conn,
                            trellis_conn_close_cb cb, void *ctx)
{
    udp_conn_data_t *cd = conn->impl_data;
    if (!cd)
        return;

    // Find and remove from transport's connection list.
    udp_transport_data_t *td = cd->handle->data;
    udp_unregister_conn(td, cd);

    free(cd);
    conn->impl_data = NULL;

    if (cb)
        cb(conn, ctx);

    trellis_conn_dealloc(conn);
}

static const trellis_conn_vtable_t udp_conn_vtable = {
    .send       = udp_conn_send,
    .recv_start = udp_conn_recv_start,
    .recv_stop  = udp_conn_recv_stop,
    .close      = udp_conn_close,
};

static trellis_err_t udp_transport_connect(trellis_transport_t *t,
                                           const char *addr,
                                           trellis_connect_cb cb, void *ctx)
{
    udp_transport_data_t *td = t->impl_data;
    struct sockaddr_storage ss;
    if (trellis_parse_addr(addr, &ss) != 0)
        return TRELLIS_ERR_INVALID_ARG;

    if (!td->bound) {
        int rc = uv_udp_init(t->loop, &td->handle);
        if (rc != 0)
            return TRELLIS_ERR_TRANSPORT;
        td->handle.data = td;

        struct sockaddr_in any;
        uv_ip4_addr("0.0.0.0", 0, &any);
        rc = uv_udp_bind(&td->handle, (const struct sockaddr *)&any, 0);
        if (rc != 0)
            return TRELLIS_ERR_TRANSPORT;

        rc = uv_udp_recv_start(&td->handle, udp_alloc_cb, udp_recv_cb);
        if (rc != 0)
            return TRELLIS_ERR_TRANSPORT;

        td->bound = true;
    }

    trellis_conn_t *conn = udp_conn_create(td,
                                           (const struct sockaddr *)&ss);
    if (!conn)
        return TRELLIS_ERR_NOMEM;

    if (cb)
        cb(conn, TRELLIS_OK, ctx);
    return TRELLIS_OK;
}

static trellis_err_t udp_transport_listen(trellis_transport_t *t,
                                          const char *addr,
                                          trellis_accept_cb cb, void *ctx)
{
    udp_transport_data_t *td = t->impl_data;
    if (td->bound)
        return TRELLIS_ERR_ALREADY_EXISTS;

    struct sockaddr_storage ss;
    if (trellis_parse_addr(addr, &ss) != 0)
        return TRELLIS_ERR_INVALID_ARG;

    int rc = uv_udp_init(t->loop, &td->handle);
    if (rc != 0)
        return TRELLIS_ERR_TRANSPORT;

    td->handle.data = td;

    rc = uv_udp_bind(&td->handle, (const struct sockaddr *)&ss, 0);
    if (rc != 0) {
        uv_close((uv_handle_t *)&td->handle, NULL);
        return TRELLIS_ERR_LISTEN;
    }

    td->accept_cb = cb;
    td->accept_ctx = ctx;

    rc = uv_udp_recv_start(&td->handle, udp_alloc_cb, udp_recv_cb);
    if (rc != 0) {
        uv_close((uv_handle_t *)&td->handle, NULL);
        return TRELLIS_ERR_LISTEN;
    }

    td->bound = true;
    return TRELLIS_OK;
}

static trellis_err_t udp_transport_stop(trellis_transport_t *t)
{
    udp_transport_data_t *td = t->impl_data;
    if (!td->bound)
        return TRELLIS_OK;

    uv_udp_recv_stop(&td->handle);
    uv_close((uv_handle_t *)&td->handle, NULL);
    td->bound = false;
    return TRELLIS_OK;
}

static void udp_transport_free(trellis_transport_t *t)
{
    udp_transport_data_t *td = t->impl_data;
    if (!td)
        return;

    if (td->bound && !uv_is_closing((uv_handle_t *)&td->handle)) {
        uv_udp_recv_stop(&td->handle);
        uv_close((uv_handle_t *)&td->handle, NULL);
    }
    free(td->conns);
    free(td);
}

static const trellis_transport_vtable_t udp_transport_vtable = {
    .connect = udp_transport_connect,
    .listen  = udp_transport_listen,
    .stop    = udp_transport_stop,
    .free    = udp_transport_free,
};

trellis_transport_t *trellis_transport_udp_new(uv_loop_t *loop)
{
    trellis_transport_t *t = trellis_transport_alloc(
        &udp_transport_vtable, "udp", loop);
    if (!t)
        return NULL;

    udp_transport_data_t *td = calloc(1, sizeof(*td));
    if (!td) {
        free(t);
        return NULL;
    }

    td->transport = t;
    t->impl_data = td;
    return t;
}
