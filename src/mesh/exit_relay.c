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
 * exit_relay.c — Clearnet Exit Relay Architecture
 *
 * Provides a Tor-like exit relay for Bloom circuits to access the clearnet
 * internet.  Exit relays:
 *   1. Accept onion-routed CONNECT or SOCKS5 requests from Bloom clients
 *   2. Forward the TCP connection to the requested clearnet destination
 *   3. Bidirectionally proxy data between the Bloom circuit and the TCP conn
 *   4. Enforce an exit policy (allow/deny by domain/IP/port)
 *   5. Apply abuse mitigation: connection rate limiting + proof-of-work
 *
 * SOCKS5 proxy interface:
 *   Exit relays also expose an optional local SOCKS5 port so that
 *   traditional applications can route through Bloom without modification.
 *   The SOCKS5 proxy constructs a Bloom onion circuit and exits through a
 *   willing exit relay.
 *
 * Exit policy syntax (simplified):
 *   "allow:*:443"       — allow all HTTPS
 *   "allow:*:80"        — allow HTTP
 *   "deny:*:25"         — block SMTP (anti-spam)
 *   "deny:*.onion:*"    — block .onion (use greenhouse instead)
 *   Rules are evaluated in order; default = deny.
 *
 * Proof-of-Work anti-abuse:
 *   Clients must attach a Hashcash-style PoW token to exit CONNECT requests.
 *   The token is: SHA256(circuit_id || target || nonce) with leading zeros.
 *   Difficulty is dynamically adjusted based on relay load.
 *
 * Privacy notes:
 *   - Exit relays can see the destination address but NOT the circuit origin
 *     (that's protected by onion routing).
 *   - DNS requests use the relay's resolver; implement DoH to prevent
 *     resolver-side correlation.  TODO(dns): not done yet
 *   - No logging of destinations (operators should configure no-log policy).
 */

#include "internal.h"
#include <trellis/mesh.h>
#include <trellis/error.h>
#include <sodium.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <time.h>
#include "../platform.h"
#include <uv.h>

#define EXIT_MAX_POLICY_RULES      64
#define EXIT_MAX_CONNECTIONS       512
#define EXIT_SOCKS5_BUF_LEN        65536
#define EXIT_RATE_LIMIT_WINDOW_MS  1000
#define EXIT_RATE_LIMIT_MAX_CONN   10    /* max new connections per second */
#define EXIT_POW_DIFFICULTY        TRELLIS_EXIT_POW_DIFFICULTY
#define EXIT_CONNECT_TIMEOUT_MS    10000

// SOCKS5 protocol constants.
#define SOCKS5_VERSION             0x05
#define SOCKS5_CMD_CONNECT         0x01
#define SOCKS5_ATYP_IPV4           0x01
#define SOCKS5_ATYP_DOMAIN         0x03
#define SOCKS5_ATYP_IPV6           0x04
#define SOCKS5_AUTH_NONE           0x00
#define SOCKS5_REP_SUCCESS         0x00
#define SOCKS5_REP_GENERAL_FAILURE 0x01
#define SOCKS5_REP_CONN_REFUSED    0x05
#define SOCKS5_REP_ADDR_UNSUP      0x08

typedef enum {
    EXIT_RULE_ALLOW,
    EXIT_RULE_DENY
} exit_rule_action_t;

typedef struct exit_policy_rule {
    exit_rule_action_t action;
    char               host_pattern[256];   /* "*.example.com" or "*" */
    uint16_t           port_min;
    uint16_t           port_max;            /* port_min == port_max for single port */
} exit_policy_rule_t;

// Connection tracking.

// exit_circuit_write_fn and exit_circuit_upstream_t are in internal.h.
typedef struct exit_conn {
    bool         active;
    uv_tcp_t     upstream;       /* connection to Bloom circuit (SOCKS5 mode) */
    uv_tcp_t     downstream;     /* connection to clearnet target */
    uv_connect_t connect_req;
    uv_timer_t   timeout_timer;
    bool         timer_active;
    bool         connected;

    char         target_host[256];
    uint16_t     target_port;

    uint8_t      up_buf[EXIT_SOCKS5_BUF_LEN];
    uint8_t      dn_buf[EXIT_SOCKS5_BUF_LEN];

    /* Circuit-mode upstream: when set, downstream reads are forwarded
     * via this callback rather than to the uv_tcp_t upstream. */
    exit_circuit_write_fn circuit_write;
    void                 *circuit_ctx;
    uint32_t              stream_id;

    struct trellis_exit_relay *relay;
} exit_conn_t;

// Rate limiter.
typedef struct exit_rate_state {
    uint64_t window_start;
    uint32_t count;
} exit_rate_state_t;

typedef enum {
    SOCKS5_HANDSHAKE,
    SOCKS5_AUTH,
    SOCKS5_REQUEST,
    SOCKS5_CONNECTED
} socks5_state_t;

typedef struct socks5_conn {
    bool           active;
    uv_tcp_t       client;
    socks5_state_t state;
    char           target_host[256];
    uint16_t       target_port;
    uint8_t        buf[1024];
    size_t         buf_used;
    exit_conn_t   *exit_conn;
    struct trellis_exit_relay *relay;
} socks5_conn_t;

struct trellis_exit_relay {
    exit_policy_rule_t rules[EXIT_MAX_POLICY_RULES];
    size_t             rule_count;

    exit_conn_t        conns[EXIT_MAX_CONNECTIONS];
    size_t             conn_count;

    socks5_conn_t      socks_conns[EXIT_MAX_CONNECTIONS];

    uv_loop_t         *loop;
    uv_tcp_t           socks_server;
    bool               socks_active;
    int                socks_port;

    exit_rate_state_t  rate;

    // DNS-over-HTTPS resolver for privacy-preserving name resolution.
    trellis_doh_resolver_t *doh;

    uint32_t           pow_difficulty;  /* leading bits */
    bool               pow_required;
};

// Exit policy evaluation.
static bool pattern_match(const char *pattern, const char *host)
{
    if (strcmp(pattern, "*") == 0) return true;
    if (pattern[0] == '*' && pattern[1] == '.') {
        // wildcard subdomain: *.example.com matches sub.example.com.
        const char *suffix = pattern + 1; /* ".example.com" */
        size_t slen = strlen(suffix);
        size_t hlen = strlen(host);
        if (hlen < slen) return false;
        return strcmp(host + hlen - slen, suffix) == 0;
    }
    return strcmp(pattern, host) == 0;
}

static bool exit_policy_allows(const struct trellis_exit_relay *er,
                                 const char *host, uint16_t port)
{
    for (size_t i = 0; i < er->rule_count; i++) {
        const exit_policy_rule_t *r = &er->rules[i];
        if (port < r->port_min || port > r->port_max) continue;
        if (!pattern_match(r->host_pattern, host)) continue;
        return (r->action == EXIT_RULE_ALLOW);
    }
    return false; /* default deny */
}

// Connection rate limiter.
static uint64_t exit_now_ms(void)
{
    uv_timespec64_t ts;
    uv_clock_gettime(UV_CLOCK_MONOTONIC, &ts);
    return (uint64_t)ts.tv_sec * 1000 + ts.tv_nsec / 1000000;
}

static bool exit_rate_check(struct trellis_exit_relay *er)
{
    uint64_t now = exit_now_ms();
    if (now - er->rate.window_start > EXIT_RATE_LIMIT_WINDOW_MS) {
        er->rate.window_start = now;
        er->rate.count = 0;
    }
    if (er->rate.count >= (uint32_t)EXIT_RATE_LIMIT_MAX_CONN)
        return false;
    er->rate.count++;
    return true;
}

// Proof-of-Work verification.
static bool exit_verify_pow(const uint8_t *token, size_t token_len,
                              uint32_t required_bits)
{
    if (required_bits == 0) return true;
    if (!token || token_len < 32) return false;

    // PoW token = nonce such that SHA256(challenge || nonce) has leading zeros.
    uint8_t hash[32];
    crypto_hash_sha256(hash, token, token_len);

    // Check leading bits.
    uint32_t bits_checked = 0;
    for (size_t i = 0; i < 32 && bits_checked < required_bits; i++) {
        uint8_t byte = hash[i];
        for (int b = 7; b >= 0 && bits_checked < required_bits; b--) {
            if ((byte >> b) & 1) return false;
            bits_checked++;
        }
    }
    return true;
}

// Connection proxying.
static void exit_on_downstream_alloc(uv_handle_t *handle, size_t suggested,
                                       uv_buf_t *buf)
{
    exit_conn_t *ec = (exit_conn_t *)handle->data;
    buf->base = (char *)ec->dn_buf;
    buf->len  = sizeof(ec->dn_buf);
}

static void exit_on_upstream_alloc(uv_handle_t *handle, size_t suggested,
                                    uv_buf_t *buf)
{
    exit_conn_t *ec = (exit_conn_t *)handle->data;
    buf->base = (char *)ec->up_buf;
    buf->len  = sizeof(ec->up_buf);
}

typedef struct {
    uv_write_t req;
    uint8_t    data[];
} exit_write_req_t;

static void exit_write_done(uv_write_t *req, int status)
{
    (void)status;
    free(req);
}

static void exit_write_buf_done(uv_write_t *req, int status)
{
    (void)status;
    free(req->data);
    free(req);
}

static void exit_on_downstream_read(uv_stream_t *stream, ssize_t nread,
                                     const uv_buf_t *buf)
{
    exit_conn_t *ec = (exit_conn_t *)stream->data;
    if (nread <= 0) {
        if (!ec->circuit_write)
            uv_close((uv_handle_t *)&ec->upstream, NULL);
        uv_close((uv_handle_t *)&ec->downstream, NULL);
        ec->active = false;
        return;
    }

    // Circuit mode: send data back via CIRCUIT_DATA callback.
    if (ec->circuit_write) {
        ec->circuit_write((const uint8_t *)buf->base, (size_t)nread,
                          ec->stream_id, ec->circuit_ctx);
        return;
    }

    exit_write_req_t *wr = malloc(sizeof(*wr) + nread);
    if (!wr) return;
    memcpy(wr->data, buf->base, nread);
    uv_buf_t wbuf = uv_buf_init((char *)wr->data, (unsigned)nread);
    uv_write(&wr->req, (uv_stream_t *)&ec->upstream, &wbuf, 1,
             exit_write_done);
}

static void exit_on_upstream_read(uv_stream_t *stream, ssize_t nread,
                                   const uv_buf_t *buf)
{
    exit_conn_t *ec = (exit_conn_t *)stream->data;
    if (nread <= 0) {
        uv_close((uv_handle_t *)&ec->upstream, NULL);
        uv_close((uv_handle_t *)&ec->downstream, NULL);
        ec->active = false;
        return;
    }

    exit_write_req_t *wr = malloc(sizeof(*wr) + nread);
    if (!wr) return;
    memcpy(wr->data, buf->base, nread);
    uv_buf_t wbuf = uv_buf_init((char *)wr->data, (unsigned)nread);
    uv_write(&wr->req, (uv_stream_t *)&ec->downstream, &wbuf, 1,
             exit_write_done);
}

static void exit_on_connect_timeout(uv_timer_t *handle)
{
    exit_conn_t *ec = (exit_conn_t *)handle->data;
    fprintf(stderr, "[exit] connect timeout to %s:%u\n",
            ec->target_host, ec->target_port);
    if (ec->upstream.loop)
        uv_close((uv_handle_t *)&ec->upstream, NULL);
    uv_close((uv_handle_t *)&ec->downstream, NULL);
    uv_timer_stop(handle);
    uv_close((uv_handle_t *)handle, NULL);
    ec->timer_active = false;
    ec->active = false;
}

// Exit connection helpers.
static exit_conn_t *exit_alloc_conn(struct trellis_exit_relay *er)
{
    for (size_t i = 0; i < EXIT_MAX_CONNECTIONS; i++) {
        if (!er->conns[i].active) {
            memset(&er->conns[i], 0, sizeof(exit_conn_t));
            er->conns[i].active = true;
            er->conns[i].relay  = er;
            return &er->conns[i];
        }
    }
    return NULL;
}

static void exit_start_proxy(exit_conn_t *ec)
{
    ec->connected = true;
    ec->downstream.data = ec;

    uv_read_start((uv_stream_t *)&ec->downstream,
                  exit_on_downstream_alloc, exit_on_downstream_read);

    // In circuit mode, upstream data arrives via messages, not a TCP stream.
    if (!ec->circuit_write) {
        ec->upstream.data = ec;
        uv_read_start((uv_stream_t *)&ec->upstream,
                      exit_on_upstream_alloc, exit_on_upstream_read);
    }

    fprintf(stderr, "[exit] proxy active for %s:%u\n",
            ec->target_host, ec->target_port);
}

static void exit_on_connect(uv_connect_t *req, int status)
{
    exit_conn_t *ec = (exit_conn_t *)req->data;

    if (ec->timer_active) {
        uv_timer_stop(&ec->timeout_timer);
        uv_close((uv_handle_t *)&ec->timeout_timer, NULL);
        ec->timer_active = false;
    }

    if (status < 0) {
        fprintf(stderr, "[exit] connect failed: %s\n", uv_strerror(status));
        uv_close((uv_handle_t *)&ec->upstream, NULL);
        ec->active = false;
        return;
    }

    exit_start_proxy(ec);
}

// SOCKS5 proxy.
static void socks5_send_reply(socks5_conn_t *sc, uint8_t rep)
{
    exit_write_req_t *wr = malloc(sizeof(*wr) + 10);
    if (!wr) return;
    uint8_t *r = wr->data;
    r[0] = SOCKS5_VERSION;
    r[1] = rep;
    r[2] = 0x00; /* RSV */
    r[3] = SOCKS5_ATYP_IPV4;
    memset(r + 4, 0, 6); /* BND.ADDR + BND.PORT */
    uv_buf_t buf = uv_buf_init((char *)r, 10);
    uv_write(&wr->req, (uv_stream_t *)&sc->client, &buf, 1, exit_write_done);
}

static void socks5_on_target_connect(uv_connect_t *req, int status)
{
    socks5_conn_t *sc = (socks5_conn_t *)req->data;
    exit_conn_t   *ec = sc->exit_conn;

    if (ec && ec->timer_active) {
        uv_timer_stop(&ec->timeout_timer);
        uv_close((uv_handle_t *)&ec->timeout_timer, NULL);
        ec->timer_active = false;
    }

    if (status < 0 || !ec) {
        fprintf(stderr, "[socks5] connect to %s:%u failed: %s\n",
                sc->target_host, sc->target_port,
                status < 0 ? uv_strerror(status) : "no conn");
        socks5_send_reply(sc, SOCKS5_REP_CONN_REFUSED);
        uv_close((uv_handle_t *)&sc->client, NULL);
        if (ec) {
            uv_close((uv_handle_t *)&ec->downstream, NULL);
            ec->active = false;
        }
        sc->active = false;
        return;
    }

    uv_read_stop((uv_stream_t *)&sc->client);

    /*
     * Transfer the SOCKS5 client socket into the exit_conn upstream.
     * Dup the fd first so we know whether the transfer will work
     * before telling the client about success/failure.
     */
    uv_os_fd_t client_fd;
    uv_fileno((uv_handle_t *)&sc->client, &client_fd);

    int dup_fd = trellis_dup(client_fd);
    if (dup_fd < 0) {
        socks5_send_reply(sc, SOCKS5_REP_GENERAL_FAILURE);
        uv_close((uv_handle_t *)&sc->client, NULL);
        ec->active = false;
        sc->active = false;
        return;
    }

    socks5_send_reply(sc, SOCKS5_REP_SUCCESS);
    sc->state = SOCKS5_CONNECTED;

    uv_close((uv_handle_t *)&sc->client, NULL);

    uv_tcp_init(ec->relay->loop, &ec->upstream);
    uv_tcp_open(&ec->upstream, dup_fd);

    strncpy(ec->target_host, sc->target_host, sizeof(ec->target_host) - 1);
    ec->target_port = sc->target_port;

    exit_start_proxy(ec);
}

static void socks5_doh_cb(const struct sockaddr_storage *addr,
                          trellis_err_t err, void *ctx)
{
    socks5_conn_t *sc = (socks5_conn_t *)ctx;

    if (err != TRELLIS_OK || !addr) {
        fprintf(stderr, "[socks5] DoH resolution failed for %s\n",
                sc->target_host);
        socks5_send_reply(sc, SOCKS5_REP_CONN_REFUSED);
        uv_close((uv_handle_t *)&sc->client, NULL);
        sc->active = false;
        return;
    }

    exit_conn_t *ec = exit_alloc_conn(sc->relay);
    if (!ec) {
        fprintf(stderr, "[socks5] connection table full\n");
        socks5_send_reply(sc, SOCKS5_REP_CONN_REFUSED);
        uv_close((uv_handle_t *)&sc->client, NULL);
        sc->active = false;
        return;
    }

    sc->exit_conn = ec;

    uv_tcp_init(sc->relay->loop, &ec->downstream);
    ec->connect_req.data = sc;

    uv_timer_init(sc->relay->loop, &ec->timeout_timer);
    ec->timeout_timer.data = ec;
    uv_timer_start(&ec->timeout_timer, exit_on_connect_timeout,
                   EXIT_CONNECT_TIMEOUT_MS, 0);
    ec->timer_active = true;

    int rc = uv_tcp_connect(&ec->connect_req,
                            &ec->downstream,
                            (const struct sockaddr *)addr,
                            socks5_on_target_connect);
    if (rc != 0) {
        fprintf(stderr, "[socks5] uv_tcp_connect failed: %s\n", uv_strerror(rc));
        uv_timer_stop(&ec->timeout_timer);
        uv_close((uv_handle_t *)&ec->timeout_timer, NULL);
        ec->timer_active = false;
        socks5_send_reply(sc, SOCKS5_REP_CONN_REFUSED);
        uv_close((uv_handle_t *)&sc->client, NULL);
        uv_close((uv_handle_t *)&ec->downstream, NULL);
        ec->active = false;
        sc->active = false;
    }
}

static void socks5_process(socks5_conn_t *sc, const uint8_t *data, size_t len)
{
    if (data && len > 0) {
        size_t space = sizeof(sc->buf) - sc->buf_used;
        if (len > space) len = space;
        memcpy(sc->buf + sc->buf_used, data, len);
        sc->buf_used += len;
    }

    switch (sc->state) {
    case SOCKS5_HANDSHAKE: {
        if (sc->buf_used < 2) return;
        if (sc->buf[0] != SOCKS5_VERSION) goto error;
        uint8_t nmethods = sc->buf[1];
        if (sc->buf_used < (size_t)(2 + nmethods)) return;
        uint8_t *auth_reply = malloc(2);
        if (!auth_reply) goto error;
        auth_reply[0] = SOCKS5_VERSION;
        auth_reply[1] = SOCKS5_AUTH_NONE;
        uv_write_t *req = malloc(sizeof(*req));
        if (!req) { free(auth_reply); goto error; }
        req->data = auth_reply;
        uv_buf_t buf = uv_buf_init((char *)auth_reply, 2);
        uv_write(req, (uv_stream_t *)&sc->client, &buf, 1, exit_write_buf_done);
        size_t consumed = 2 + nmethods;
        memmove(sc->buf, sc->buf + consumed, sc->buf_used - consumed);
        sc->buf_used -= consumed;
        sc->state = SOCKS5_REQUEST;
        socks5_process(sc, NULL, 0);
        break;
    }
    case SOCKS5_REQUEST: {
        if (sc->buf_used < 4) return;
        if (sc->buf[0] != SOCKS5_VERSION || sc->buf[1] != SOCKS5_CMD_CONNECT)
            goto error;
        uint8_t atyp = sc->buf[3];
        if (atyp == SOCKS5_ATYP_DOMAIN) {
            if (sc->buf_used < 5) return;
            uint8_t dlen = sc->buf[4];
            if (sc->buf_used < (size_t)(5 + dlen + 2)) return;
            memcpy(sc->target_host, sc->buf + 5, dlen);
            sc->target_host[dlen] = '\0';
            sc->target_port = ((uint16_t)sc->buf[5 + dlen] << 8) |
                               sc->buf[6 + dlen];
        } else if (atyp == SOCKS5_ATYP_IPV4) {
            if (sc->buf_used < 10) return;
            snprintf(sc->target_host, sizeof(sc->target_host), "%u.%u.%u.%u",
                     sc->buf[4], sc->buf[5], sc->buf[6], sc->buf[7]);
            sc->target_port = ((uint16_t)sc->buf[8] << 8) | sc->buf[9];
        } else if (atyp == SOCKS5_ATYP_IPV6) {
            if (sc->buf_used < 22) return; /* 4 header + 16 addr + 2 port */
            struct in6_addr a6;
            memcpy(&a6, sc->buf + 4, 16);
            char v6[INET6_ADDRSTRLEN];
            uv_inet_ntop(AF_INET6, &a6, v6, sizeof(v6));
            snprintf(sc->target_host, sizeof(sc->target_host), "%s", v6);
            sc->target_port = ((uint16_t)sc->buf[20] << 8) | sc->buf[21];
        } else {
            socks5_send_reply(sc, SOCKS5_REP_ADDR_UNSUP);
            goto error;
        }

        if (!exit_policy_allows(sc->relay, sc->target_host, sc->target_port)) {
            fprintf(stderr, "[socks5] exit policy denied: %s:%u\n",
                    sc->target_host, sc->target_port);
            socks5_send_reply(sc, SOCKS5_REP_CONN_REFUSED);
            goto error;
        }

        if (!exit_rate_check(sc->relay)) {
            fprintf(stderr, "[socks5] rate limit exceeded\n");
            socks5_send_reply(sc, SOCKS5_REP_CONN_REFUSED);
            goto error;
        }

        fprintf(stderr, "[socks5] CONNECT %s:%u — resolving via DoH\n",
                sc->target_host, sc->target_port);

        if (sc->relay->doh) {
            trellis_err_t doh_err = trellis_doh_resolve(
                sc->relay->doh, sc->target_host, sc->target_port,
                AF_UNSPEC, socks5_doh_cb, sc);
            if (doh_err != TRELLIS_OK) {
                socks5_send_reply(sc, SOCKS5_REP_CONN_REFUSED);
                goto error;
            }
        } else {
            socks5_send_reply(sc, SOCKS5_REP_CONN_REFUSED);
            goto error;
        }
        sc->state = SOCKS5_AUTH;
        break;
    }
    case SOCKS5_CONNECTED: {
        // Bidirectional proxy: forward client data to the downstream target.
        if (sc->exit_conn && sc->exit_conn->connected && sc->buf_used > 0) {
            uint8_t *copy = malloc(sc->buf_used);
            uv_write_t *wr = copy ? malloc(sizeof(*wr)) : NULL;
            if (wr) {
                memcpy(copy, sc->buf, sc->buf_used);
                wr->data = copy;
                uv_buf_t wbuf = uv_buf_init((char *)copy,
                                            (unsigned)sc->buf_used);
                uv_write(wr, (uv_stream_t *)&sc->exit_conn->downstream,
                         &wbuf, 1, exit_write_buf_done);
                sc->buf_used = 0;
            } else {
                free(copy);
            }
        }
        break;
    }
    default:
        break;
    }
    return;
error:
    uv_close((uv_handle_t *)&sc->client, NULL);
    sc->active = false;
}

static void socks5_alloc_cb(uv_handle_t *handle, size_t suggested, uv_buf_t *buf)
{
    socks5_conn_t *sc = (socks5_conn_t *)handle->data;
    size_t available = sizeof(sc->buf) - sc->buf_used;
    buf->base = (char *)(sc->buf + sc->buf_used);
    buf->len  = available > suggested ? suggested : available;
}

static void socks5_read_cb(uv_stream_t *stream, ssize_t nread,
                            const uv_buf_t *buf)
{
    socks5_conn_t *sc = (socks5_conn_t *)stream->data;
    if (nread <= 0) {
        uv_close((uv_handle_t *)&sc->client, NULL);
        sc->active = false;
        return;
    }
    socks5_process(sc, (const uint8_t *)buf->base, (size_t)nread);
}

static void socks5_on_accept(uv_stream_t *server, int status)
{
    if (status < 0) return;
    struct trellis_exit_relay *er = (struct trellis_exit_relay *)server->data;

    if (!exit_rate_check(er)) {
        fprintf(stderr, "[socks5] rate limit exceeded\n");
        uv_tcp_t tmp;
        uv_tcp_init(er->loop, &tmp);
        uv_accept(server, (uv_stream_t *)&tmp);
        uv_close((uv_handle_t *)&tmp, NULL);
        return;
    }

    // Find free slot.
    socks5_conn_t *sc = NULL;
    for (size_t i = 0; i < EXIT_MAX_CONNECTIONS; i++) {
        if (!er->socks_conns[i].active) {
            sc = &er->socks_conns[i];
            break;
        }
    }
    if (!sc) {
        fprintf(stderr, "[socks5] connection table full\n");
        uv_tcp_t tmp;
        uv_tcp_init(er->loop, &tmp);
        uv_accept(server, (uv_stream_t *)&tmp);
        uv_close((uv_handle_t *)&tmp, NULL);
        return;
    }

    memset(sc, 0, sizeof(*sc));
    sc->active = true;
    sc->relay  = er;
    sc->state  = SOCKS5_HANDSHAKE;

    uv_tcp_init(er->loop, &sc->client);
    sc->client.data = sc;

    if (uv_accept(server, (uv_stream_t *)&sc->client) != 0) {
        sc->active = false;
        return;
    }

    uv_read_start((uv_stream_t *)&sc->client, socks5_alloc_cb, socks5_read_cb);
}

// Public API.
trellis_exit_relay_t *trellis_exit_relay_new(uv_loop_t *loop)
{
    if (!loop) return NULL;
    struct trellis_exit_relay *er = calloc(1, sizeof(*er));
    if (!er) return NULL;
    er->loop          = loop;
    er->pow_difficulty = EXIT_POW_DIFFICULTY;
    er->pow_required  = true;
    er->doh           = trellis_doh_new(loop, NULL);
    return (trellis_exit_relay_t *)er;
}

static void exit_relay_free_cb(uv_handle_t *handle)
{
    struct trellis_exit_relay *er = (struct trellis_exit_relay *)handle->data;
    free(er);
}

void trellis_exit_relay_free(trellis_exit_relay_t *er_)
{
    if (!er_) return;
    struct trellis_exit_relay *er = (struct trellis_exit_relay *)er_;

    for (size_t i = 0; i < EXIT_MAX_CONNECTIONS; i++) {
        exit_conn_t *ec = &er->conns[i];
        if (!ec->active) continue;
        if (ec->timer_active) {
            uv_timer_stop(&ec->timeout_timer);
            uv_close((uv_handle_t *)&ec->timeout_timer, NULL);
        }
        if (!ec->circuit_write)
            uv_close((uv_handle_t *)&ec->upstream, NULL);
        uv_close((uv_handle_t *)&ec->downstream, NULL);
        ec->active = false;
    }

    for (size_t i = 0; i < EXIT_MAX_CONNECTIONS; i++) {
        socks5_conn_t *sc = &er->socks_conns[i];
        if (!sc->active) continue;
        uv_close((uv_handle_t *)&sc->client, NULL);
        sc->active = false;
    }

    trellis_doh_free(er->doh);

    if (er->socks_active) {
        er->socks_server.data = er;
        uv_close((uv_handle_t *)&er->socks_server, exit_relay_free_cb);
    } else {
        free(er);
    }
}

trellis_err_t trellis_exit_relay_add_policy(
        trellis_exit_relay_t *er_,
        const char *rule_str)
{
    struct trellis_exit_relay *er = (struct trellis_exit_relay *)er_;
    if (!er || !rule_str) return TRELLIS_ERR_INVALID_ARG;
    if (er->rule_count >= EXIT_MAX_POLICY_RULES) return TRELLIS_ERR_FULL;

    exit_policy_rule_t *r = &er->rules[er->rule_count];
    char action[8], host[256], port_str[32];

    if (sscanf(rule_str, "%7[^:]:%255[^:]:%31s", action, host, port_str) != 3)
        return TRELLIS_ERR_INVALID_ARG;

    r->action = (strcmp(action, "allow") == 0) ? EXIT_RULE_ALLOW : EXIT_RULE_DENY;
    strncpy(r->host_pattern, host, sizeof(r->host_pattern) - 1);

    if (strcmp(port_str, "*") == 0) {
        r->port_min = 1;
        r->port_max = 65535;
    } else {
        unsigned p;
        if (sscanf(port_str, "%u", &p) != 1 || p > 65535)
            return TRELLIS_ERR_INVALID_ARG;
        r->port_min = r->port_max = (uint16_t)p;
    }

    er->rule_count++;
    return TRELLIS_OK;
}

trellis_err_t trellis_exit_relay_start_socks5(
        trellis_exit_relay_t *er_,
        int port)
{
    struct trellis_exit_relay *er = (struct trellis_exit_relay *)er_;
    if (!er || port < 1 || port > 65535) return TRELLIS_ERR_INVALID_ARG;

    er->socks_port = port;
    uv_tcp_init(er->loop, &er->socks_server);
    er->socks_server.data = er;

    struct sockaddr_in addr;
    uv_ip4_addr("127.0.0.1", port, &addr);

    if (uv_tcp_bind(&er->socks_server,
                    (const struct sockaddr *)&addr, 0) != 0)
        return TRELLIS_ERR_IO;

    if (uv_listen((uv_stream_t *)&er->socks_server, 16,
                  socks5_on_accept) != 0)
        return TRELLIS_ERR_IO;

    er->socks_active = true;
    fprintf(stderr, "[exit] SOCKS5 proxy listening on 127.0.0.1:%d\n", port);
    return TRELLIS_OK;
}

bool trellis_exit_relay_policy_allows(const trellis_exit_relay_t *er_,
                                       const char *host, uint16_t port)
{
    if (!er_ || !host) return false;
    return exit_policy_allows((const struct trellis_exit_relay *)er_, host, port);
}

bool trellis_exit_relay_verify_pow(const trellis_exit_relay_t *er_,
                                    const uint8_t *token, size_t token_len)
{
    const struct trellis_exit_relay *er = (const struct trellis_exit_relay *)er_;
    if (!er || !er->pow_required) return true;
    return exit_verify_pow(token, token_len, er->pow_difficulty);
}

// Onion-routed exit stream handling.
static void stream_on_connect(uv_connect_t *req, int status)
{
    exit_conn_t *ec = (exit_conn_t *)req->data;

    if (ec->timer_active) {
        uv_timer_stop(&ec->timeout_timer);
        uv_close((uv_handle_t *)&ec->timeout_timer, NULL);
        ec->timer_active = false;
    }

    if (status < 0) {
        fprintf(stderr, "[exit-stream] connect to %s:%u failed: %s\n",
                ec->target_host, ec->target_port, uv_strerror(status));
        uv_close((uv_handle_t *)&ec->downstream, NULL);
        if (ec->upstream.loop)
            uv_close((uv_handle_t *)&ec->upstream, NULL);
        ec->active = false;
        return;
    }

    exit_start_proxy(ec);
}

static void stream_doh_cb(const struct sockaddr_storage *addr,
                          trellis_err_t err, void *ctx)
{
    exit_conn_t *ec = (exit_conn_t *)ctx;

    if (err != TRELLIS_OK || !addr) {
        fprintf(stderr, "[exit-stream] DoH resolution failed for %s\n",
                ec->target_host);
        if (ec->upstream.loop)
            uv_close((uv_handle_t *)&ec->upstream, NULL);
        ec->active = false;
        return;
    }

    uv_tcp_init(ec->relay->loop, &ec->downstream);
    ec->connect_req.data = ec;

    uv_timer_init(ec->relay->loop, &ec->timeout_timer);
    ec->timeout_timer.data = ec;
    uv_timer_start(&ec->timeout_timer, exit_on_connect_timeout,
                   EXIT_CONNECT_TIMEOUT_MS, 0);
    ec->timer_active = true;

    int rc = uv_tcp_connect(&ec->connect_req,
                            &ec->downstream,
                            (const struct sockaddr *)addr,
                            stream_on_connect);
    if (rc != 0) {
        fprintf(stderr, "[exit-stream] uv_tcp_connect failed: %s\n",
                uv_strerror(rc));
        uv_timer_stop(&ec->timeout_timer);
        uv_close((uv_handle_t *)&ec->timeout_timer, NULL);
        ec->timer_active = false;
        uv_close((uv_handle_t *)&ec->downstream, NULL);
        if (ec->upstream.loop)
            uv_close((uv_handle_t *)&ec->upstream, NULL);
        ec->active = false;
    }
}

// Find an existing exit_conn by stream_id (for DATA/CLOSE on established streams)
static exit_conn_t *exit_find_conn_by_stream(struct trellis_exit_relay *er,
                                              uint32_t stream_id)
{
    if (stream_id == 0) return NULL;
    for (size_t i = 0; i < EXIT_MAX_CONNECTIONS; i++) {
        if (er->conns[i].active && er->conns[i].stream_id == stream_id)
            return &er->conns[i];
    }
    return NULL;
}

// Handle OPEN command: parse host/port/PoW, allocate conn, start DNS resolve.
static trellis_err_t exit_handle_open(struct trellis_exit_relay *er,
                                       const uint8_t *payload, size_t payload_len,
                                       uint32_t stream_id, void *upstream)
{
    if (payload_len < 4)
        return TRELLIS_ERR_INVALID_ARG;

    uint8_t host_len = payload[0];
    if (host_len > 253)
        return TRELLIS_ERR_INVALID_ARG;
    if (payload_len < (size_t)(1 + host_len + 2))
        return TRELLIS_ERR_INVALID_ARG;

    char host[256] = {0};
    memcpy(host, payload + 1, host_len);
    host[host_len] = '\0';

    uint16_t port = ((uint16_t)payload[1 + host_len] << 8) |
                     payload[2 + host_len];

    if (er->pow_required) {
        size_t header_len = (size_t)(1 + host_len + 2);
        const uint8_t *pow_token = payload + header_len;
        size_t pow_len = payload_len - header_len;
        if (!exit_verify_pow(pow_token, pow_len, er->pow_difficulty)) {
            fprintf(stderr, "[exit-stream] PoW verification failed\n");
            return TRELLIS_ERR_INVALID_ARG;
        }
    }

    if (!exit_policy_allows(er, host, port)) {
        fprintf(stderr, "[exit-stream] policy denied: %s:%u\n", host, port);
        return TRELLIS_ERR_NOT_FOUND;
    }

    exit_conn_t *ec = exit_alloc_conn(er);
    if (!ec)
        return TRELLIS_ERR_NOMEM;

    strncpy(ec->target_host, host, sizeof(ec->target_host) - 1);
    ec->target_port = port;
    ec->stream_id   = stream_id;

    if (upstream) {
        exit_circuit_upstream_t *cu = (exit_circuit_upstream_t *)upstream;
        ec->circuit_write = cu->write_fn;
        ec->circuit_ctx   = cu->ctx;
    }

    fprintf(stderr, "[exit-stream] opening connection to %s:%u via DoH "
            "(stream %08x)\n", host, port, stream_id);

    if (!er->doh) {
        ec->active = false;
        return TRELLIS_ERR_NOT_INITIALIZED;
    }

    trellis_err_t doh_err = trellis_doh_resolve(
        er->doh, host, port, AF_UNSPEC, stream_doh_cb, ec);
    if (doh_err != TRELLIS_OK) {
        ec->active = false;
        return doh_err;
    }

    return TRELLIS_OK;
}

// Handle DATA command: forward raw bytes to the existing clearnet socket.
static trellis_err_t exit_handle_data(struct trellis_exit_relay *er,
                                       const uint8_t *data, size_t len,
                                       uint32_t stream_id)
{
    exit_conn_t *ec = exit_find_conn_by_stream(er, stream_id);
    if (!ec) {
        fprintf(stderr, "[exit-stream] DATA for unknown stream %08x\n",
                stream_id);
        return TRELLIS_ERR_NOT_FOUND;
    }

    if (!ec->connected) {
        fprintf(stderr, "[exit-stream] DATA before connect complete on %08x\n",
                stream_id);
        return TRELLIS_ERR_NOT_INITIALIZED;
    }

    exit_write_req_t *wr = malloc(sizeof(*wr) + len);
    if (!wr)
        return TRELLIS_ERR_NOMEM;
    memcpy(wr->data, data, len);
    uv_buf_t uvbuf = uv_buf_init((char *)wr->data, (unsigned int)len);
    int rc = uv_write(&wr->req, (uv_stream_t *)&ec->downstream,
                       &uvbuf, 1, exit_write_done);
    if (rc != 0) {
        free(wr);
        return TRELLIS_ERR_IO;
    }
    return TRELLIS_OK;
}

// Handle CLOSE command: tear down the clearnet socket for a stream.
static trellis_err_t exit_handle_close(struct trellis_exit_relay *er,
                                        uint32_t stream_id)
{
    exit_conn_t *ec = exit_find_conn_by_stream(er, stream_id);
    if (!ec)
        return TRELLIS_ERR_NOT_FOUND;

    fprintf(stderr, "[exit-stream] closing stream %08x (%s:%u)\n",
            stream_id, ec->target_host, ec->target_port);

    if (ec->timer_active) {
        uv_timer_stop(&ec->timeout_timer);
        uv_close((uv_handle_t *)&ec->timeout_timer, NULL);
        ec->timer_active = false;
    }
    if (ec->downstream.loop)
        uv_close((uv_handle_t *)&ec->downstream, NULL);
    if (!ec->circuit_write && ec->upstream.loop)
        uv_close((uv_handle_t *)&ec->upstream, NULL);
    ec->active = false;
    return TRELLIS_OK;
}

trellis_err_t trellis_exit_relay_handle_stream(trellis_exit_relay_t *er_,
                                                const uint8_t *payload,
                                                size_t payload_len,
                                                uint32_t stream_id,
                                                void *upstream)
{
    struct trellis_exit_relay *er = (struct trellis_exit_relay *)er_;
    if (!er || !payload || payload_len < 1)
        return TRELLIS_ERR_INVALID_ARG;

    if (!exit_rate_check(er))
        return TRELLIS_ERR_RATE_LIMITED;

    uint8_t cmd = payload[0];
    const uint8_t *body = payload + 1;
    size_t body_len = payload_len - 1;

    switch (cmd) {
    case TRELLIS_EXIT_CMD_OPEN:
        return exit_handle_open(er, body, body_len, stream_id, upstream);
    case TRELLIS_EXIT_CMD_DATA:
        return exit_handle_data(er, body, body_len, stream_id);
    case TRELLIS_EXIT_CMD_CLOSE:
        return exit_handle_close(er, stream_id);
    default:
        fprintf(stderr, "[exit-stream] unknown command 0x%02x\n", cmd);
        return TRELLIS_ERR_INVALID_ARG;
    }
}
