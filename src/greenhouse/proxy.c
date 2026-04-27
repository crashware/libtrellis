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
#include "../transport/internal.h"

#include <stdio.h>

static void on_client_request(trellis_conn_t *conn,
                              const uint8_t *data, size_t len,
                              trellis_err_t err, void *ctx);

typedef struct raw_tcp_conn {
    uv_tcp_t             *handle;
    trellis_conn_t       *conn;
    trellis_conn_recv_cb  recv_cb;
    void                 *recv_ctx;
} raw_tcp_conn_t;

static void raw_tcp_alloc(uv_handle_t *handle, size_t suggested, uv_buf_t *buf)
{
    (void)handle;
    buf->base = malloc(suggested);
    buf->len = buf->base ? suggested : 0;
}

static void raw_tcp_read(uv_stream_t *stream, ssize_t nread, const uv_buf_t *buf)
{
    raw_tcp_conn_t *rc = stream->data;

    if (nread < 0) {
        free(buf->base);
        if (rc->recv_cb) {
            trellis_err_t err = (nread == UV_EOF) ? TRELLIS_ERR_CLOSED
                                                  : TRELLIS_ERR_TRANSPORT;
            rc->recv_cb(rc->conn, NULL, 0, err, rc->recv_ctx);
        }
        return;
    }

    if (nread == 0) {
        free(buf->base);
        return;
    }

    if (rc->recv_cb)
        rc->recv_cb(rc->conn, (const uint8_t *)buf->base, (size_t)nread,
                    TRELLIS_OK, rc->recv_ctx);
    free(buf->base);
}

typedef struct raw_write_req {
    uv_write_t           req;
    uint8_t             *data;
    trellis_conn_send_cb cb;
    void                *ctx;
} raw_write_req_t;

static void raw_tcp_on_write(uv_write_t *req, int status)
{
    raw_write_req_t *wr = req->data;
    if (wr->cb)
        wr->cb(status == 0 ? TRELLIS_OK : TRELLIS_ERR_SEND, wr->ctx);
    free(wr->data);
    free(wr);
}

static trellis_err_t raw_tcp_send(trellis_conn_t *conn,
                                  const uint8_t *data, size_t len,
                                  trellis_conn_send_cb cb, void *ctx)
{
    raw_tcp_conn_t *rc = conn->impl_data;

    raw_write_req_t *wr = malloc(sizeof(*wr));
    if (!wr)
        return TRELLIS_ERR_NOMEM;

    wr->data = malloc(len);
    if (!wr->data) {
        free(wr);
        return TRELLIS_ERR_NOMEM;
    }
    memcpy(wr->data, data, len);
    wr->cb = cb;
    wr->ctx = ctx;
    wr->req.data = wr;

    uv_buf_t buf = uv_buf_init((char *)wr->data, (unsigned int)len);
    int r = uv_write(&wr->req, (uv_stream_t *)rc->handle, &buf, 1,
                     raw_tcp_on_write);
    if (r != 0) {
        free(wr->data);
        free(wr);
        return TRELLIS_ERR_SEND;
    }

    return TRELLIS_OK;
}

static trellis_err_t raw_tcp_recv_start(trellis_conn_t *conn,
                                        trellis_conn_recv_cb cb, void *ctx)
{
    raw_tcp_conn_t *rc = conn->impl_data;
    rc->recv_cb = cb;
    rc->recv_ctx = ctx;
    int r = uv_read_start((uv_stream_t *)rc->handle, raw_tcp_alloc,
                          raw_tcp_read);
    return r == 0 ? TRELLIS_OK : TRELLIS_ERR_TRANSPORT;
}

static void raw_tcp_recv_stop(trellis_conn_t *conn)
{
    raw_tcp_conn_t *rc = conn->impl_data;
    uv_read_stop((uv_stream_t *)rc->handle);
    rc->recv_cb = NULL;
    rc->recv_ctx = NULL;
}

typedef struct raw_close_ctx {
    trellis_conn_close_cb cb;
    void                 *ctx;
    trellis_conn_t       *conn;
    raw_tcp_conn_t       *rc;
} raw_close_ctx_t;

static void raw_tcp_on_close(uv_handle_t *handle)
{
    raw_close_ctx_t *cc = handle->data;
    trellis_conn_close_cb cb = cc->cb;
    void *ctx = cc->ctx;
    trellis_conn_t *conn = cc->conn;
    raw_tcp_conn_t *rc = cc->rc;

    free(rc->handle);
    free(cc);
    free(rc);
    conn->impl_data = NULL;

    if (cb)
        cb(conn, ctx);
    trellis_conn_dealloc(conn);
}

static void raw_tcp_close(trellis_conn_t *conn,
                          trellis_conn_close_cb cb, void *ctx)
{
    raw_tcp_conn_t *rc = conn->impl_data;
    if (!rc)
        return;

    raw_close_ctx_t *cc = malloc(sizeof(*cc));
    if (!cc) {
        if (cb)
            cb(conn, ctx);
        return;
    }
    cc->cb = cb;
    cc->ctx = ctx;
    cc->conn = conn;
    cc->rc = rc;

    rc->handle->data = cc;
    uv_close((uv_handle_t *)rc->handle, raw_tcp_on_close);
}

static const trellis_conn_vtable_t raw_tcp_vtable = {
    .send       = raw_tcp_send,
    .recv_start = raw_tcp_recv_start,
    .recv_stop  = raw_tcp_recv_stop,
    .close      = raw_tcp_close,
};

static trellis_conn_t *raw_tcp_conn_create(uv_tcp_t *handle, uv_loop_t *loop)
{
    trellis_conn_t *conn = trellis_conn_alloc(&raw_tcp_vtable, "raw-tcp", loop);
    if (!conn)
        return NULL;

    raw_tcp_conn_t *rc = calloc(1, sizeof(*rc));
    if (!rc) {
        trellis_conn_dealloc(conn);
        return NULL;
    }

    rc->handle = handle;
    rc->handle->data = rc;
    rc->conn = conn;
    conn->impl_data = rc;

    struct sockaddr_storage local_ss, remote_ss;
    int namelen = sizeof(local_ss);
    if (uv_tcp_getsockname(handle, (struct sockaddr *)&local_ss, &namelen) == 0)
        trellis_conn_set_addrs(conn, (struct sockaddr *)&local_ss, NULL);

    namelen = sizeof(remote_ss);
    if (uv_tcp_getpeername(handle, (struct sockaddr *)&remote_ss, &namelen) == 0)
        trellis_conn_set_addrs(conn, NULL, (struct sockaddr *)&remote_ss);

    return conn;
}

static const char BLOOM_SCHEME[] = "bloom://";

static trellis_proxy_entry_t *proxy_cache_find(trellis_proxy_t *proxy,
                                               const char *addr)
{
    for (size_t i = 0; i < proxy->cache_count; i++) {
        if (strcmp(proxy->cache[i].bloom_addr, addr) == 0 &&
            proxy->cache[i].tunnel)
            return &proxy->cache[i];
    }
    return NULL;
}

static trellis_proxy_entry_t *proxy_cache_insert(trellis_proxy_t *proxy,
                                                 const char *addr,
                                                 trellis_conn_t *tunnel)
{
    if (proxy->cache_count >= TRELLIS_PROXY_MAX_CONNS) {
        uint64_t oldest = UINT64_MAX;
        size_t oldest_idx = 0;
        for (size_t i = 0; i < proxy->cache_count; i++) {
            if (proxy->cache[i].last_used < oldest) {
                oldest = proxy->cache[i].last_used;
                oldest_idx = i;
            }
        }
        if (proxy->cache[oldest_idx].tunnel)
            trellis_conn_close(proxy->cache[oldest_idx].tunnel, NULL, NULL);
        proxy->cache[oldest_idx].tunnel = NULL;
        proxy->cache[oldest_idx] = proxy->cache[--proxy->cache_count];
    }

    trellis_proxy_entry_t *entry = &proxy->cache[proxy->cache_count];
    size_t addr_len = strlen(addr);
    if (addr_len >= sizeof(entry->bloom_addr))
        addr_len = sizeof(entry->bloom_addr) - 1;
    memcpy(entry->bloom_addr, addr, addr_len);
    entry->bloom_addr[addr_len] = '\0';
    entry->tunnel = tunnel;
    entry->last_used = trellis_now_ms();
    proxy->cache_count++;
    return entry;
}

static bool parse_bloom_uri(const char *request_line,
                            char *host_out, size_t host_len,
                            char *path_out, size_t path_len)
{
    const char *uri = strstr(request_line, BLOOM_SCHEME);
    if (!uri)
        return false;

    uri += strlen(BLOOM_SCHEME);
    const char *slash = strchr(uri, '/');
    const char *space = strchr(uri, ' ');

    size_t host_sz;
    if (slash && (!space || slash < space))
        host_sz = (size_t)(slash - uri);
    else if (space)
        host_sz = (size_t)(space - uri);
    else
        host_sz = strlen(uri);

    if (host_sz == 0 || host_sz >= host_len)
        return false;

    memcpy(host_out, uri, host_sz);
    host_out[host_sz] = '\0';

    if (slash && (!space || slash < space)) {
        size_t path_sz = space ? (size_t)(space - slash) : strlen(slash);
        if (path_sz >= path_len)
            path_sz = path_len - 1;
        memcpy(path_out, slash, path_sz);
        path_out[path_sz] = '\0';
    } else {
        path_out[0] = '/';
        path_out[1] = '\0';
    }

    return true;
}

typedef struct {
    trellis_proxy_t *proxy;
    trellis_conn_t  *client_conn;
    uint8_t         *request_data;
    size_t           request_len;
    char             bloom_addr[128];
} proxy_request_ctx_t;

static void on_tunnel_response(trellis_conn_t *conn,
                               const uint8_t *data, size_t len,
                               trellis_err_t err, void *ctx)
{
    proxy_request_ctx_t *rc = ctx;
    (void)conn;

    if (err != TRELLIS_OK || !data || len == 0) {
        trellis_conn_close(rc->client_conn, NULL, NULL);
        free(rc->request_data);
        free(rc);
        return;
    }

    // Forward response to client.
    trellis_conn_send(rc->client_conn, data, len, NULL, NULL);
}

static void on_tunnel_connected(trellis_conn_t *tunnel,
                                trellis_err_t err, void *ctx)
{
    proxy_request_ctx_t *rc = ctx;

    if (err != TRELLIS_OK || !tunnel) {
        const char *resp = "HTTP/1.1 502 Bad Gateway\r\nContent-Length: 0\r\n\r\n";
        trellis_conn_send(rc->client_conn, (const uint8_t *)resp,
                          strlen(resp), NULL, NULL);
        trellis_conn_close(rc->client_conn, NULL, NULL);
        free(rc->request_data);
        free(rc);
        return;
    }

    proxy_cache_insert(rc->proxy, rc->bloom_addr, tunnel);

    trellis_conn_recv_start(tunnel, on_tunnel_response, rc);
    trellis_conn_send(tunnel, rc->request_data, rc->request_len, NULL, NULL);
}

static void on_client_request(trellis_conn_t *conn,
                              const uint8_t *data, size_t len,
                              trellis_err_t err, void *ctx)
{
    trellis_proxy_t *proxy = ctx;
    if (err != TRELLIS_OK || !data || len == 0)
        return;

    char *request = malloc(len + 1);
    if (!request)
        return;
    memcpy(request, data, len);
    request[len] = '\0';

    char host[128], path[256];
    if (!parse_bloom_uri(request, host, sizeof(host),
                         path, sizeof(path))) {
        free(request);
        const char *resp = "HTTP/1.1 400 Bad Request\r\nContent-Length: 0\r\n\r\n";
        trellis_conn_send(conn, (const uint8_t *)resp, strlen(resp), NULL, NULL);
        return;
    }

    char bloom_uri[128];
    snprintf(bloom_uri, sizeof(bloom_uri), "bloom:%s", host);

    free(request);

    trellis_proxy_entry_t *cached = proxy_cache_find(proxy, bloom_uri);
    if (cached) {
        cached->last_used = trellis_now_ms();
        trellis_conn_send(cached->tunnel, data, len, NULL, NULL);
        return;
    }

    trellis_bloom_addr_t addr;
    trellis_err_t perr = trellis_bloom_addr_parse(bloom_uri, &addr);
    if (perr != TRELLIS_OK) {
        const char *resp = "HTTP/1.1 502 Bad Gateway\r\nContent-Length: 0\r\n\r\n";
        trellis_conn_send(conn, (const uint8_t *)resp, strlen(resp), NULL, NULL);
        return;
    }

    proxy_request_ctx_t *rc = calloc(1, sizeof(*rc));
    if (!rc)
        return;

    rc->proxy = proxy;
    rc->client_conn = conn;
    rc->request_data = malloc(len);
    if (!rc->request_data) {
        free(rc);
        return;
    }
    memcpy(rc->request_data, data, len);
    rc->request_len = len;
    snprintf(rc->bloom_addr, sizeof(rc->bloom_addr), "%s", bloom_uri);

    trellis_greenhouse_connect(proxy->dht, &addr, on_tunnel_connected, rc);
}

static void on_proxy_accept(uv_stream_t *server, int status)
{
    trellis_proxy_t *proxy = server->data;
    if (status < 0 || !proxy->running)
        return;

    uv_tcp_t *client = malloc(sizeof(*client));
    if (!client)
        return;

    uv_tcp_init(proxy->loop, client);
    if (uv_accept(server, (uv_stream_t *)client) != 0) {
        uv_close((uv_handle_t *)client, (uv_close_cb)free);
        return;
    }

    trellis_conn_t *conn = raw_tcp_conn_create(client, proxy->loop);
    if (!conn) {
        uv_close((uv_handle_t *)client, (uv_close_cb)free);
        return;
    }

    trellis_conn_recv_start(conn, on_client_request, proxy);
}

trellis_proxy_t *trellis_proxy_new(uv_loop_t *loop, trellis_dht_t *dht,
                                   uint16_t port)
{
    if (!loop || !dht)
        return NULL;

    trellis_proxy_t *proxy = calloc(1, sizeof(*proxy));
    if (!proxy)
        return NULL;

    proxy->loop = loop;
    proxy->dht = dht;
    proxy->listen_port = port;
    return proxy;
}

trellis_err_t trellis_proxy_start(trellis_proxy_t *proxy)
{
    if (!proxy)
        return TRELLIS_ERR_INVALID_ARG;

    if (proxy->running)
        return TRELLIS_ERR_ALREADY_EXISTS;

    uv_tcp_init(proxy->loop, &proxy->listener);
    proxy->listener.data = proxy;

    struct sockaddr_in bind_addr;
    uv_ip4_addr("127.0.0.1", proxy->listen_port, &bind_addr);

    int r = uv_tcp_bind(&proxy->listener, (const struct sockaddr *)&bind_addr, 0);
    if (r != 0)
        return TRELLIS_ERR_LISTEN;

    r = uv_listen((uv_stream_t *)&proxy->listener, 128, on_proxy_accept);
    if (r != 0)
        return TRELLIS_ERR_LISTEN;

    proxy->running = true;
    return TRELLIS_OK;
}

trellis_err_t trellis_proxy_stop(trellis_proxy_t *proxy)
{
    if (!proxy)
        return TRELLIS_ERR_INVALID_ARG;

    if (!proxy->running)
        return TRELLIS_OK;

    for (size_t i = 0; i < proxy->cache_count; i++) {
        if (proxy->cache[i].tunnel) {
            trellis_conn_close(proxy->cache[i].tunnel, NULL, NULL);
            proxy->cache[i].tunnel = NULL;
        }
    }
    proxy->cache_count = 0;

    uv_close((uv_handle_t *)&proxy->listener, NULL);
    proxy->running = false;
    return TRELLIS_OK;
}

void trellis_proxy_free(trellis_proxy_t *proxy)
{
    if (!proxy)
        return;

    if (proxy->running)
        trellis_proxy_stop(proxy);

    free(proxy);
}
