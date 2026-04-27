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
 * metrics.c — Privacy-Preserving Operator Monitoring
 *
 * Provides per-node operational metrics for relay operators, with strict
 * privacy guarantees:
 *
 *   NEVER logged:
 *     - Circuit endpoints (source or destination fingerprints)
 *     - Greenhouse service identities
 *     - Garden membership
 *     - Message content or length distributions that could fingerprint users
 *     - Timing of individual connections
 *
 *   Safe to expose:
 *     - Aggregate bandwidth (bytes relayed, bytes sent/received by transport)
 *     - Peer count (active connections)
 *     - DHT routing table size
 *     - Cover traffic ratio (cover bytes / total bytes)
 *     - Uptime
 *     - Relay circuit count (no endpoints)
 *     - Error counts by category (not by peer)
 *     - Queue depths
 *
 * Prometheus format:
 *   Metrics are exposed over an optional HTTP endpoint (localhost only,
 *   never on a public interface).  The endpoint format is plain-text
 *   Prometheus exposition format (text/plain; version=0.0.4).
 *
 * The metrics HTTP listener binds ONLY to 127.0.0.1 by default and
 * MUST NOT be exposed publicly.
 */

#include "internal.h"
#include <trellis/error.h>
#include <uv.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdatomic.h>
#include <time.h>

#define METRICS_SCRAPE_BUF_MAX  (64 * 1024)   /* 64 KB max Prometheus output */
#define METRICS_LISTEN_HOST     "127.0.0.1"

// --- Counters (lock-free atomics) ---
typedef struct {
    _Atomic uint64_t bytes_relayed;         /* bytes forwarded as relay */
    _Atomic uint64_t bytes_sent;            /* bytes sent to network */
    _Atomic uint64_t bytes_received;        /* bytes received from network */
    _Atomic uint64_t cover_bytes_sent;      /* cover traffic bytes */

    _Atomic uint64_t messages_sent;
    _Atomic uint64_t messages_received;
    _Atomic uint64_t messages_dropped;

    _Atomic uint64_t relay_circuits_created;
    _Atomic uint64_t relay_circuits_active;

    _Atomic uint64_t dht_stores;
    _Atomic uint64_t dht_lookups;
    _Atomic uint64_t dht_peer_count;

    _Atomic uint64_t peer_connections;
    _Atomic uint64_t peer_handshakes_ok;
    _Atomic uint64_t peer_handshakes_fail;

    _Atomic uint64_t errors_crypto;
    _Atomic uint64_t errors_transport;
    _Atomic uint64_t errors_routing;
    _Atomic uint64_t errors_other;

    _Atomic uint64_t greenhouse_connections; /* count only, no endpoints */
    _Atomic uint64_t garden_messages;        /* aggregate count only */

    uint64_t         uptime_start_sec;       /* set once at startup */
} bloom_metrics_t;

typedef struct metrics_client {
    uv_tcp_t              handle;
    trellis_metrics_t    *metrics;
    char                  recv_buf[256];
    size_t                recv_len;
} metrics_client_t;

// --- Metrics handle ---
struct trellis_metrics {
    bloom_metrics_t    counters;
    uv_loop_t         *loop;
    uv_tcp_t           server;
    bool               server_active;
    int                port;
};

// --- Counter API ---
trellis_metrics_t *trellis_metrics_new(uv_loop_t *loop)
{
    if (!loop) return NULL;
    trellis_metrics_t *m = calloc(1, sizeof(*m));
    if (!m) return NULL;
    m->loop = loop;

    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    m->counters.uptime_start_sec = (uint64_t)ts.tv_sec;
    return m;
}

void trellis_metrics_free(trellis_metrics_t *m)
{
    if (!m) return;
    if (m->server_active) {
        uv_close((uv_handle_t *)&m->server, NULL);
    }
    free(m);
}

void trellis_metrics_inc_bytes_relayed(trellis_metrics_t *m, size_t n)
{
    if (!m) return;
    atomic_fetch_add(&m->counters.bytes_relayed, n);
}

void trellis_metrics_inc_bytes_sent(trellis_metrics_t *m, size_t n)
{
    if (!m) return;
    atomic_fetch_add(&m->counters.bytes_sent, n);
}

void trellis_metrics_inc_bytes_received(trellis_metrics_t *m, size_t n)
{
    if (!m) return;
    atomic_fetch_add(&m->counters.bytes_received, n);
}

void trellis_metrics_inc_cover_bytes(trellis_metrics_t *m, size_t n)
{
    if (!m) return;
    atomic_fetch_add(&m->counters.cover_bytes_sent, n);
}

void trellis_metrics_inc_messages_sent(trellis_metrics_t *m)
{
    if (!m) return;
    atomic_fetch_add(&m->counters.messages_sent, 1);
}

void trellis_metrics_inc_messages_received(trellis_metrics_t *m)
{
    if (!m) return;
    atomic_fetch_add(&m->counters.messages_received, 1);
}

void trellis_metrics_inc_messages_dropped(trellis_metrics_t *m)
{
    if (!m) return;
    atomic_fetch_add(&m->counters.messages_dropped, 1);
}

void trellis_metrics_set_relay_circuits(trellis_metrics_t *m, uint64_t count)
{
    if (!m) return;
    atomic_store(&m->counters.relay_circuits_active, count);
}

void trellis_metrics_inc_relay_circuit(trellis_metrics_t *m)
{
    if (!m) return;
    atomic_fetch_add(&m->counters.relay_circuits_created, 1);
    atomic_fetch_add(&m->counters.relay_circuits_active, 1);
}

void trellis_metrics_dec_relay_circuit(trellis_metrics_t *m)
{
    if (!m) return;
    uint64_t cur = atomic_load(&m->counters.relay_circuits_active);
    if (cur > 0) atomic_fetch_sub(&m->counters.relay_circuits_active, 1);
}

void trellis_metrics_inc_dht_store(trellis_metrics_t *m)
{
    if (!m) return;
    atomic_fetch_add(&m->counters.dht_stores, 1);
}

void trellis_metrics_inc_dht_lookup(trellis_metrics_t *m)
{
    if (!m) return;
    atomic_fetch_add(&m->counters.dht_lookups, 1);
}

void trellis_metrics_set_dht_peers(trellis_metrics_t *m, uint64_t count)
{
    if (!m) return;
    atomic_store(&m->counters.dht_peer_count, count);
}

void trellis_metrics_set_peer_connections(trellis_metrics_t *m, uint64_t count)
{
    if (!m) return;
    atomic_store(&m->counters.peer_connections, count);
}

void trellis_metrics_inc_handshake_ok(trellis_metrics_t *m)
{
    if (!m) return;
    atomic_fetch_add(&m->counters.peer_handshakes_ok, 1);
}

void trellis_metrics_inc_handshake_fail(trellis_metrics_t *m)
{
    if (!m) return;
    atomic_fetch_add(&m->counters.peer_handshakes_fail, 1);
}

void trellis_metrics_inc_error(trellis_metrics_t *m, trellis_err_t err)
{
    if (!m) return;
    if (err >= -199 && err <= -100)
        atomic_fetch_add(&m->counters.errors_crypto, 1);
    else if (err >= -299 && err <= -200)
        atomic_fetch_add(&m->counters.errors_transport, 1);
    else if (err >= -399 && err <= -300)
        atomic_fetch_add(&m->counters.errors_routing, 1);
    else
        atomic_fetch_add(&m->counters.errors_other, 1);
}

void trellis_metrics_inc_greenhouse_conn(trellis_metrics_t *m)
{
    if (!m) return;
    atomic_fetch_add(&m->counters.greenhouse_connections, 1);
}

void trellis_metrics_inc_garden_message(trellis_metrics_t *m)
{
    if (!m) return;
    atomic_fetch_add(&m->counters.garden_messages, 1);
}

// --- Prometheus text generation ---
static size_t write_counter(char *buf, size_t cap, size_t off,
                              const char *name, const char *help,
                              uint64_t value)
{
    off += (size_t)snprintf(buf + off, cap - off,
        "# HELP %s %s\n"
        "# TYPE %s counter\n"
        "%s %" PRIu64 "\n",
        name, help, name, name, value);
    return off;
}

static size_t write_gauge(char *buf, size_t cap, size_t off,
                           const char *name, const char *help,
                           uint64_t value)
{
    off += (size_t)snprintf(buf + off, cap - off,
        "# HELP %s %s\n"
        "# TYPE %s gauge\n"
        "%s %" PRIu64 "\n",
        name, help, name, name, value);
    return off;
}

/*
 * Generate a Prometheus-format metrics scrape payload.
 * Returns the number of bytes written, or 0 on error.
 */
size_t trellis_metrics_scrape(const trellis_metrics_t *m,
                               char *buf, size_t buf_cap)
{
    if (!m || !buf || buf_cap < 256) return 0;

    const bloom_metrics_t *c = &m->counters;
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    uint64_t uptime_sec = (uint64_t)ts.tv_sec - c->uptime_start_sec;

    size_t off = 0;

#define COUNTER(name, help, val) \
    off = write_counter(buf, buf_cap, off, name, help, val)
#define GAUGE(name, help, val) \
    off = write_gauge(buf, buf_cap, off, name, help, val)

    GAUGE("bloom_uptime_seconds",
          "Seconds since the Bloom node started",
          uptime_sec);

    GAUGE("process_start_time_seconds",
          "Unix timestamp when the process started",
          c->uptime_start_sec);

    COUNTER("bloom_bytes_relayed_total",
            "Total bytes relayed for other nodes (no endpoint info)",
            atomic_load(&c->bytes_relayed));

    COUNTER("bloom_bytes_sent_total",
            "Total bytes sent to the network",
            atomic_load(&c->bytes_sent));

    COUNTER("bloom_bytes_received_total",
            "Total bytes received from the network",
            atomic_load(&c->bytes_received));

    COUNTER("bloom_cover_bytes_sent_total",
            "Total cover traffic bytes sent",
            atomic_load(&c->cover_bytes_sent));

    // Cover ratio as a gauge (0.0 - 1.0 * 1000 for integer metric)
    uint64_t total_tx = atomic_load(&c->bytes_sent);
    uint64_t cover_tx = atomic_load(&c->cover_bytes_sent);
    uint64_t cover_ratio_permille = total_tx > 0
                                  ? (cover_tx * 1000 / total_tx) : 0;
    GAUGE("bloom_cover_ratio_permille",
          "Cover traffic as permille (0-1000) of total outbound bytes",
          cover_ratio_permille);

    COUNTER("bloom_messages_sent_total",
            "Total Bloom protocol messages sent",
            atomic_load(&c->messages_sent));

    COUNTER("bloom_messages_received_total",
            "Total Bloom protocol messages received",
            atomic_load(&c->messages_received));

    COUNTER("bloom_messages_dropped_total",
            "Total messages dropped (queue full, crypto error, etc.)",
            atomic_load(&c->messages_dropped));

    GAUGE("bloom_relay_circuits_active",
          "Number of active relay circuits (no endpoint info)",
          atomic_load(&c->relay_circuits_active));

    COUNTER("bloom_relay_circuits_created_total",
            "Total relay circuits created since startup",
            atomic_load(&c->relay_circuits_created));

    COUNTER("bloom_dht_stores_total",
            "Total DHT STORE operations performed",
            atomic_load(&c->dht_stores));

    COUNTER("bloom_dht_lookups_total",
            "Total DHT FIND_VALUE operations performed",
            atomic_load(&c->dht_lookups));

    GAUGE("bloom_dht_peers",
          "Number of peers in the DHT routing table",
          atomic_load(&c->dht_peer_count));

    GAUGE("bloom_peer_connections",
          "Number of active peer connections",
          atomic_load(&c->peer_connections));

    COUNTER("bloom_handshakes_ok_total",
            "Total successful handshakes",
            atomic_load(&c->peer_handshakes_ok));

    COUNTER("bloom_handshakes_fail_total",
            "Total failed handshakes",
            atomic_load(&c->peer_handshakes_fail));

    COUNTER("bloom_errors_crypto_total",
            "Cryptographic errors (no identifying info)",
            atomic_load(&c->errors_crypto));

    COUNTER("bloom_errors_transport_total",
            "Transport errors",
            atomic_load(&c->errors_transport));

    COUNTER("bloom_errors_routing_total",
            "Routing errors",
            atomic_load(&c->errors_routing));

    COUNTER("bloom_errors_other_total",
            "Other errors",
            atomic_load(&c->errors_other));

    COUNTER("bloom_greenhouse_connections_total",
            "Total greenhouse connections served (no service identity logged)",
            atomic_load(&c->greenhouse_connections));

    COUNTER("bloom_garden_messages_total",
            "Total garden messages relayed/posted (aggregate only)",
            atomic_load(&c->garden_messages));

#undef COUNTER
#undef GAUGE

    return off;
}

// --- HTTP server for Prometheus scraping ---
static void metrics_client_close_cb(uv_handle_t *h)
{
    free(h->data);
}

static void metrics_write_done_cb(uv_write_t *req, int status)
{
    (void)status;
    uv_buf_t *buf = req->data;
    if (buf) { free(buf->base); free(buf); }
    free(req);
}

static void metrics_on_read(uv_stream_t *stream, ssize_t nread, const uv_buf_t *buf)
{
    metrics_client_t *client = stream->data;

    if (nread <= 0) {
        if (buf->base) free(buf->base);
        uv_close((uv_handle_t *)stream, metrics_client_close_cb);
        return;
    }

    // Accumulate into request buffer; we only need the first line.
    size_t to_copy = (size_t)nread;
    if (client->recv_len + to_copy > sizeof(client->recv_buf) - 1)
        to_copy = sizeof(client->recv_buf) - 1 - client->recv_len;
    memcpy(client->recv_buf + client->recv_len, buf->base, to_copy);
    client->recv_len += to_copy;
    client->recv_buf[client->recv_len] = '\0';
    if (buf->base) free(buf->base);

    // Check if we have a complete HTTP request line.
    if (!strstr(client->recv_buf, "\r\n\r\n") &&
        !strstr(client->recv_buf, "\n\n"))
        return;

    // Parse request path from "GET /path HTTP/1.x".
    const char *content_type = "text/plain; version=0.0.4";
    char *body = NULL;
    size_t body_len = 0;

    if (strncmp(client->recv_buf, "GET /health", 11) == 0) {
        static const char health_body[] = "{\"status\":\"ok\"}\n";
        body = malloc(sizeof(health_body));
        if (!body) {
            uv_close((uv_handle_t *)stream, metrics_client_close_cb);
            return;
        }
        memcpy(body, health_body, sizeof(health_body) - 1);
        body_len = sizeof(health_body) - 1;
        content_type = "application/json";
    } else {
        body = malloc(METRICS_SCRAPE_BUF_MAX);  /* FIXME: streaming write instead of malloc-all */
        if (!body) {
            uv_close((uv_handle_t *)stream, metrics_client_close_cb);
            return;
        }
        body_len = trellis_metrics_scrape(client->metrics, body,
                                           METRICS_SCRAPE_BUF_MAX);
    }

    // Build HTTP response.
    char header[256];
    int hlen = snprintf(header, sizeof(header),
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: %s\r\n"
        "Content-Length: %zu\r\n"
        "Connection: close\r\n"
        "\r\n",
        content_type, body_len);

    size_t resp_len = (size_t)hlen + body_len;
    char *resp = malloc(resp_len);
    if (!resp) { free(body); uv_close((uv_handle_t *)stream, metrics_client_close_cb); return; }
    memcpy(resp, header, (size_t)hlen);
    memcpy(resp + hlen, body, body_len);
    free(body);

    uv_write_t *req = malloc(sizeof(*req));
    uv_buf_t   *rbuf = malloc(sizeof(*rbuf));
    if (!req || !rbuf) {
        free(req); free(rbuf); free(resp);
        uv_close((uv_handle_t *)stream, metrics_client_close_cb);
        return;
    }
    *rbuf = uv_buf_init(resp, (unsigned int)resp_len);
    req->data = rbuf;
    uv_write(req, stream, rbuf, 1, metrics_write_done_cb);
}

static void metrics_alloc_cb(uv_handle_t *h, size_t suggested, uv_buf_t *buf)
{
    (void)h; (void)suggested;
    buf->base = malloc(256);
    buf->len  = buf->base ? 256 : 0;
}

static void metrics_on_connection(uv_stream_t *server, int status)
{
    if (status < 0) return;

    trellis_metrics_t *m = server->data;

    metrics_client_t *client = calloc(1, sizeof(*client));
    if (!client) return;
    client->metrics = m;

    uv_tcp_init(m->loop, &client->handle);
    client->handle.data = client;

    if (uv_accept(server, (uv_stream_t *)&client->handle) != 0) {
        uv_close((uv_handle_t *)&client->handle, metrics_client_close_cb);
        return;
    }
    uv_read_start((uv_stream_t *)&client->handle,
                   metrics_alloc_cb, metrics_on_read);
}

/*
 * Start the Prometheus HTTP metrics endpoint on localhost:port.
 * Binds ONLY to 127.0.0.1 — never exposes to the network.
 */
trellis_err_t trellis_metrics_start_http(trellis_metrics_t *m, int port)
{
    if (!m || port <= 0 || port > 65535) return TRELLIS_ERR_INVALID_ARG;
    if (m->server_active) return TRELLIS_ERR_ALREADY_EXISTS;

    uv_tcp_init(m->loop, &m->server);
    m->server.data = m;

    struct sockaddr_in addr;
    uv_ip4_addr(METRICS_LISTEN_HOST, port, &addr);

    if (uv_tcp_bind(&m->server, (const struct sockaddr *)&addr, 0) != 0) {
        uv_close((uv_handle_t *)&m->server, NULL);
        return TRELLIS_ERR_LISTEN;
    }

    if (uv_listen((uv_stream_t *)&m->server, 4, metrics_on_connection) != 0) {
        uv_close((uv_handle_t *)&m->server, NULL);
        return TRELLIS_ERR_LISTEN;
    }

    m->server_active = true;
    m->port = port;
    return TRELLIS_OK;
}

void trellis_metrics_stop_http(trellis_metrics_t *m)
{
    if (!m || !m->server_active) return;
    uv_close((uv_handle_t *)&m->server, NULL);
    m->server_active = false;
}
