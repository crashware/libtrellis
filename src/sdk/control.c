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
 * Control interface — Unix domain socket with JSON-RPC 2.0 protocol.
 *
 * Allows external tooling (CLIs, monitoring daemons, orchestrators) to
 * introspect and command a running trellis client without linking against
 * libtrellis directly.
 *
 * Wire protocol: newline-delimited JSON-RPC 2.0 over a stream socket.
 * Authentication: optional shared-secret cookie file (like Tor's CookieAuth).
 *
 * Read commands:  getInfo, getPeers, getCircuits, getGardens, getMetrics
 * Write commands: connect, disconnect, setRouting, setAlias, stop
 */

#include "internal.h"
#include "../mesh/internal.h"
#include <uv.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <inttypes.h>
#include "../platform.h"

#define CTRL_MAX_CLIENTS       8
#define CTRL_RECV_BUF          (16u * 1024u)
#define CTRL_COOKIE_LEN        32
#define CTRL_MAX_MSG           (64u * 1024u)

typedef struct trellis_ctrl       trellis_ctrl_t;
typedef struct ctrl_client        ctrl_client_t;

static void ctrl_on_connection(uv_stream_t *server, int status);
static void ctrl_on_alloc(uv_handle_t *handle, size_t suggested, uv_buf_t *buf);
static void ctrl_on_read(uv_stream_t *stream, ssize_t nread, const uv_buf_t *buf);
static void ctrl_process_line(ctrl_client_t *cc, const char *line, size_t len);
static void ctrl_send_response(ctrl_client_t *cc, const char *json);
static void ctrl_close_client(ctrl_client_t *cc);

struct ctrl_client {
    uv_pipe_t        pipe;
    trellis_ctrl_t  *ctrl;
    char            *recv_buf;
    size_t           recv_len;
    size_t           recv_cap;
    bool             authenticated;
    bool             closing;
};

struct trellis_ctrl {
    uv_pipe_t             server;
    trellis_client_t     *client;
    char                 *socket_path;
    bool                  running;

    // Cookie auth — if cookie is set, clients must send {"method":"auth","params":{"cookie":"<hex>"}} first.
    uint8_t               cookie[CTRL_COOKIE_LEN];
    char                 *cookie_path;
    bool                  auth_required;

    ctrl_client_t        *clients[CTRL_MAX_CLIENTS];
    size_t                client_count;
};

// JSON helpers (minimal — no dependency)
static int json_find_string(const char *json, const char *key,
                            char *out, size_t out_cap)
{
    char needle[128];
    int nlen = snprintf(needle, sizeof(needle), "\"%s\"", key);
    if (nlen < 0 || (size_t)nlen >= sizeof(needle)) return -1;

    const char *p = strstr(json, needle);
    if (!p) return -1;
    p += nlen;
    while (*p == ' ' || *p == ':' || *p == '\t') p++;
    if (*p != '"') return -1;
    p++;
    const char *end = strchr(p, '"');
    if (!end) return -1;
    size_t slen = (size_t)(end - p);
    if (slen >= out_cap) slen = out_cap - 1;
    memcpy(out, p, slen);
    out[slen] = '\0';
    return (int)slen;
}

static int json_find_id(const char *json, char *out, size_t out_cap)
{
    const char *p = strstr(json, "\"id\"");
    if (!p) { out[0] = '\0'; return -1; }
    p += 4;
    while (*p == ' ' || *p == ':' || *p == '\t') p++;
    if (*p == '"') {
        p++;
        const char *end = strchr(p, '"');
        if (!end) return -1;
        size_t slen = (size_t)(end - p);
        if (slen >= out_cap) slen = out_cap - 1;
        int n = snprintf(out, out_cap, "\"%.*s\"", (int)slen, p);
        return n > 0 ? n : -1;
    }
    // Numeric id.
    const char *start = p;
    while (*p >= '0' && *p <= '9') p++;
    size_t slen = (size_t)(p - start);
    if (slen == 0 || slen >= out_cap) return -1;
    memcpy(out, start, slen);
    out[slen] = '\0';
    return (int)slen;
}

// Cookie auth.
static trellis_err_t ctrl_generate_cookie(trellis_ctrl_t *ctrl,
                                          const char *cookie_path)
{
    randombytes_buf(ctrl->cookie, CTRL_COOKIE_LEN);

    if (cookie_path) {
        FILE *f = fopen(cookie_path, "w");
        if (!f) return TRELLIS_ERR_IO;
        for (size_t i = 0; i < CTRL_COOKIE_LEN; i++)
            fprintf(f, "%02x", ctrl->cookie[i]);
        fprintf(f, "\n");
        fclose(f);
        trellis_chmod(cookie_path, 0600);
        ctrl->cookie_path = strdup(cookie_path);
    }

    ctrl->auth_required = true;
    return TRELLIS_OK;
}

static bool ctrl_verify_cookie(trellis_ctrl_t *ctrl, const char *hex)
{
    if (strlen(hex) != CTRL_COOKIE_LEN * 2)
        return false;
    uint8_t provided[CTRL_COOKIE_LEN];
    for (size_t i = 0; i < CTRL_COOKIE_LEN; i++) {
        unsigned int b;
        if (sscanf(hex + i * 2, "%02x", &b) != 1)
            return false;
        provided[i] = (uint8_t)b;
    }
    return sodium_memcmp(ctrl->cookie, provided, CTRL_COOKIE_LEN) == 0;
}

// Client management.
static ctrl_client_t *ctrl_accept_client(trellis_ctrl_t *ctrl)
{
    if (ctrl->client_count >= CTRL_MAX_CLIENTS)
        return NULL;

    ctrl_client_t *cc = calloc(1, sizeof(*cc));
    if (!cc) return NULL;

    cc->ctrl = ctrl;
    cc->recv_cap = CTRL_RECV_BUF;
    cc->recv_buf = malloc(cc->recv_cap);
    if (!cc->recv_buf) { free(cc); return NULL; }
    cc->authenticated = !ctrl->auth_required;

    ctrl->clients[ctrl->client_count++] = cc;
    return cc;
}

static void ctrl_on_close(uv_handle_t *handle)
{
    ctrl_client_t *cc = (ctrl_client_t *)handle;
    trellis_ctrl_t *ctrl = cc->ctrl;
    for (size_t i = 0; i < ctrl->client_count; i++) {
        if (ctrl->clients[i] == cc) {
            ctrl->clients[i] = ctrl->clients[--ctrl->client_count];
            break;
        }
    }
    free(cc->recv_buf);
    free(cc);
}

static void ctrl_close_client(ctrl_client_t *cc)
{
    if (cc->closing) return;
    cc->closing = true;
    uv_close((uv_handle_t *)&cc->pipe, ctrl_on_close);
}

// Response building.
static void ctrl_write_done(uv_write_t *req, int status)
{
    (void)status;
    free(req->data);
    free(req);
}

static void ctrl_send_response(ctrl_client_t *cc, const char *json)
{
    size_t len = strlen(json);
    char *buf = malloc(len + 1);
    if (!buf) return;
    memcpy(buf, json, len);
    buf[len] = '\n';

    uv_write_t *req = malloc(sizeof(*req));
    if (!req) { free(buf); return; }
    req->data = buf;

    uv_buf_t wbuf = uv_buf_init(buf, (unsigned int)(len + 1));
    if (uv_write(req, (uv_stream_t *)&cc->pipe, &wbuf, 1, ctrl_write_done) != 0) {
        free(buf);
        free(req);
    }
}

static void ctrl_send_error(ctrl_client_t *cc, const char *id,
                            int code, const char *message)
{
    char resp[512];
    snprintf(resp, sizeof(resp),
        "{\"jsonrpc\":\"2.0\",\"id\":%s,\"error\":{\"code\":%d,\"message\":\"%s\"}}",
        id[0] ? id : "null", code, message);
    ctrl_send_response(cc, resp);
}

static void ctrl_send_result(ctrl_client_t *cc, const char *id,
                             const char *result_json)
{
    size_t rlen = strlen(result_json);
    size_t need = 64 + strlen(id) + rlen;
    char *resp = malloc(need);
    if (!resp) return;
    snprintf(resp, need,
        "{\"jsonrpc\":\"2.0\",\"id\":%s,\"result\":%s}",
        id, result_json);
    ctrl_send_response(cc, resp);
    free(resp);
}

// Command handlers.
static void cmd_get_info(ctrl_client_t *cc, const char *id)
{
    trellis_client_t *client = cc->ctrl->client;

    char fp_hex[TRELLIS_FINGERPRINT_LEN * 2 + 1];
    const trellis_identity_t *ident = trellis_client_identity(client);
    trellis_fingerprint_t fp;
    trellis_identity_fingerprint(ident, &fp);
    for (size_t i = 0; i < TRELLIS_FINGERPRINT_LEN; i++)
        snprintf(fp_hex + i * 2, 3, "%02x", fp.bytes[i]);

    const char *alias = trellis_client_alias(client);
    size_t peers = trellis_client_peer_count(client);
    size_t conns = trellis_client_conn_count(client);
    const char *addr = trellis_client_listen_addr(client);

    char result[1024];
    snprintf(result, sizeof(result),
        "{\"fingerprint\":\"%s\",\"alias\":\"%s\","
        "\"peers\":%zu,\"connections\":%zu,"
        "\"listen_addr\":\"%s\",\"running\":%s}",
        fp_hex, alias ? alias : "",
        peers, conns,
        addr ? addr : "",
        client->running ? "true" : "false");

    ctrl_send_result(cc, id, result);
}

static void cmd_get_peers(ctrl_client_t *cc, const char *id)
{
    trellis_client_t *client = cc->ctrl->client;

    size_t cap = 64 + client->peer_conn_count * 384;
    char *result = malloc(cap);
    if (!result) { ctrl_send_error(cc, id, -32603, "out of memory"); return; }

    size_t off = 0;
    off += (size_t)snprintf(result + off, cap - off, "[");

    for (size_t i = 0; i < client->peer_conn_count; i++) {
        trellis_peer_conn_t *pc = &client->peers[i];
        if (!pc->conn) continue;

        char fp_hex[TRELLIS_FINGERPRINT_LEN * 2 + 1];
        for (size_t j = 0; j < TRELLIS_FINGERPRINT_LEN; j++)
            snprintf(fp_hex + j * 2, 3, "%02x", pc->remote_fp.bytes[j]);

        if (off > 1) off += (size_t)snprintf(result + off, cap - off, ",");
        off += (size_t)snprintf(result + off, cap - off,
            "{\"fingerprint\":\"%s\",\"addr\":\"%s\","
            "\"handshake_complete\":%s,"
            "\"relay\":%s,\"exit\":%s,\"greenhouse\":%s}",
            fp_hex, pc->addr,
            pc->handshake_complete ? "true" : "false",
            pc->relay_eligible ? "true" : "false",
            pc->exit_eligible ? "true" : "false",
            pc->greenhouse_eligible ? "true" : "false");
    }

    off += (size_t)snprintf(result + off, cap - off, "]");
    ctrl_send_result(cc, id, result);
    free(result);
}

static void cmd_get_circuits(ctrl_client_t *cc, const char *id)
{
    trellis_client_t *client = cc->ctrl->client;
    if (!client->relay) {
        ctrl_send_result(cc, id, "[]");
        return;
    }

    // Build JSON array of active relay circuits.
    size_t cap = 8192;
    char *result = malloc(cap);
    if (!result) { ctrl_send_error(cc, id, -32603, "OOM"); return; }

    size_t off = 0;
    off += (size_t)snprintf(result + off, cap - off, "[");

    trellis_relay_t *relay = client->relay;
    bool first = true;
    for (size_t i = 0; i < RELAY_MAX_CIRCUITS; i++) {
        if (!relay->circuits[i].active) continue;
        if (!first) off += (size_t)snprintf(result + off, cap - off, ",");
        first = false;

        char from_hex[65], to_hex[65];
        for (int b = 0; b < TRELLIS_FINGERPRINT_LEN && b < 32; b++) {
            snprintf(from_hex + b * 2, 3, "%02x",
                     relay->circuits[i].from.bytes[b]);
            snprintf(to_hex + b * 2, 3, "%02x",
                     relay->circuits[i].to.bytes[b]);
        }
        uint64_t age = trellis_now_ms() - relay->circuits[i].created_at;
        off += (size_t)snprintf(result + off, cap - off,
            "{\"from\":\"%s\",\"to\":\"%s\","
            "\"bytes_relayed\":%" PRIu64 ","
            "\"age_ms\":%" PRIu64 ","
            "\"send_window\":%u,"
            "\"cwnd\":%u}",
            from_hex, to_hex,
            relay->circuits[i].bytes_relayed,
            age,
            relay->circuits[i].send_window,
            relay->circuits[i].cwnd);
    }

    // Also include circuit_table entries.
    circuit_table_t *ct = &relay->circuit_table;
    for (size_t i = 0; i < CIRCUIT_TABLE_MAX; i++) {
        if (!ct->entries[i].active) continue;
        if (!first) off += (size_t)snprintf(result + off, cap - off, ",");
        first = false;

        char prev_hex[65];
        for (int b = 0; b < TRELLIS_FINGERPRINT_LEN && b < 32; b++)
            snprintf(prev_hex + b * 2, 3, "%02x",
                     ct->entries[i].prev_hop.bytes[b]);

        off += (size_t)snprintf(result + off, cap - off,
            "{\"type\":\"onion_circuit\",\"stream_id\":%u,"
            "\"prev_hop\":\"%s\",\"hop_count\":%u}",
            ct->entries[i].stream_id, prev_hex,
            ct->entries[i].hop_count);
    }

    off += (size_t)snprintf(result + off, cap - off, "]");
    ctrl_send_result(cc, id, result);
    free(result);
}

static void cmd_get_gardens(ctrl_client_t *cc, const char *id)
{
    trellis_client_t *client = cc->ctrl->client;

    size_t cap = 64 + client->garden_count * 256;
    char *result = malloc(cap);
    if (!result) { ctrl_send_error(cc, id, -32603, "out of memory"); return; }

    size_t off = 0;
    off += (size_t)snprintf(result + off, cap - off, "[");

    for (size_t i = 0; i < client->garden_count; i++) {
        if (!client->gardens[i].name) continue;
        if (off > 1) off += (size_t)snprintf(result + off, cap - off, ",");
        off += (size_t)snprintf(result + off, cap - off,
            "{\"name\":\"%s\"}", client->gardens[i].name);
    }

    off += (size_t)snprintf(result + off, cap - off, "]");
    ctrl_send_result(cc, id, result);
    free(result);
}

static void cmd_connect(ctrl_client_t *cc, const char *id, const char *json)
{
    char addr[256];
    if (json_find_string(json, "addr", addr, sizeof(addr)) < 0) {
        ctrl_send_error(cc, id, -32602, "missing params.addr");
        return;
    }

    trellis_err_t err = trellis_client_connect_addr(cc->ctrl->client, addr);
    if (err != TRELLIS_OK) {
        ctrl_send_error(cc, id, -32000, trellis_err_str(err));
        return;
    }
    ctrl_send_result(cc, id, "true");
}

static void cmd_disconnect(ctrl_client_t *cc, const char *id, const char *json)
{
    char fp_hex[65];
    if (json_find_string(json, "fingerprint", fp_hex, sizeof(fp_hex)) < 0) {
        ctrl_send_error(cc, id, -32602, "missing params.fingerprint");
        return;
    }

    trellis_fingerprint_t fp;
    if (strlen(fp_hex) != TRELLIS_FINGERPRINT_LEN * 2) {
        ctrl_send_error(cc, id, -32602, "invalid fingerprint length");
        return;
    }
    for (size_t i = 0; i < TRELLIS_FINGERPRINT_LEN; i++) {
        unsigned int b;
        if (sscanf(fp_hex + i * 2, "%02x", &b) != 1) {
            ctrl_send_error(cc, id, -32602, "invalid fingerprint hex");
            return;
        }
        fp.bytes[i] = (uint8_t)b;
    }

    trellis_client_remove_peer_conn(cc->ctrl->client, &fp);
    ctrl_send_result(cc, id, "true");
}

static void cmd_set_routing(ctrl_client_t *cc, const char *id, const char *json)
{
    char mode[32];
    if (json_find_string(json, "mode", mode, sizeof(mode)) < 0) {
        ctrl_send_error(cc, id, -32602, "missing params.mode");
        return;
    }

    trellis_routing_mode_t rm;
    if (strcmp(mode, "direct") == 0)         rm = TRELLIS_ROUTE_DIRECT;
    else if (strcmp(mode, "relay") == 0)     rm = TRELLIS_ROUTE_RELAY;
    else if (strcmp(mode, "multipath") == 0) rm = TRELLIS_ROUTE_MULTIPATH;
    else if (strcmp(mode, "onion") == 0)     rm = TRELLIS_ROUTE_ONION;
    else {
        ctrl_send_error(cc, id, -32602, "unknown routing mode");
        return;
    }

    trellis_err_t err = trellis_client_set_routing(cc->ctrl->client, rm);
    if (err != TRELLIS_OK) {
        ctrl_send_error(cc, id, -32000, trellis_err_str(err));
        return;
    }
    ctrl_send_result(cc, id, "true");
}

static void cmd_set_alias(ctrl_client_t *cc, const char *id, const char *json)
{
    char alias[128];
    if (json_find_string(json, "alias", alias, sizeof(alias)) < 0) {
        ctrl_send_error(cc, id, -32602, "missing params.alias");
        return;
    }

    trellis_err_t err = trellis_client_set_alias(cc->ctrl->client, alias);
    if (err != TRELLIS_OK) {
        ctrl_send_error(cc, id, -32000, trellis_err_str(err));
        return;
    }
    ctrl_send_result(cc, id, "true");
}

static void cmd_stop(ctrl_client_t *cc, const char *id)
{
    ctrl_send_result(cc, id, "true");
    trellis_client_stop(cc->ctrl->client);
}

// RPC dispatch.
static void ctrl_process_line(ctrl_client_t *cc, const char *line, size_t len)
{
    if (len == 0) return;

    (void)len;

    char method[64];
    char id[64];

    if (json_find_string(line, "method", method, sizeof(method)) < 0) {
        ctrl_send_error(cc, "null", -32600, "missing method");
        return;
    }
    json_find_id(line, id, sizeof(id));
    if (id[0] == '\0') snprintf(id, sizeof(id), "null");

    // Auth gate.
    if (!cc->authenticated) {
        if (strcmp(method, "auth") == 0) {
            char cookie_hex[128];
            if (json_find_string(line, "cookie", cookie_hex, sizeof(cookie_hex)) >= 0
                && ctrl_verify_cookie(cc->ctrl, cookie_hex))
            {
                cc->authenticated = true;
                ctrl_send_result(cc, id, "true");
            } else {
                ctrl_send_error(cc, id, -32001, "authentication failed");
                ctrl_close_client(cc);
            }
            return;
        }
        ctrl_send_error(cc, id, -32001, "not authenticated");
        return;
    }

    // Read commands.
    if (strcmp(method, "getInfo") == 0)          { cmd_get_info(cc, id); return; }
    if (strcmp(method, "getPeers") == 0)         { cmd_get_peers(cc, id); return; }
    if (strcmp(method, "getCircuits") == 0)       { cmd_get_circuits(cc, id); return; }
    if (strcmp(method, "getGardens") == 0)        { cmd_get_gardens(cc, id); return; }

    // Write commands.
    if (strcmp(method, "connect") == 0)           { cmd_connect(cc, id, line); return; }
    if (strcmp(method, "disconnect") == 0)        { cmd_disconnect(cc, id, line); return; }
    if (strcmp(method, "setRouting") == 0)        { cmd_set_routing(cc, id, line); return; }
    if (strcmp(method, "setAlias") == 0)          { cmd_set_alias(cc, id, line); return; }
    if (strcmp(method, "stop") == 0)              { cmd_stop(cc, id); return; }

    ctrl_send_error(cc, id, -32601, "method not found");
}

// UV callbacks.
static void ctrl_on_alloc(uv_handle_t *handle, size_t suggested, uv_buf_t *buf)
{
    (void)suggested;
    ctrl_client_t *cc = (ctrl_client_t *)handle;
    size_t avail = cc->recv_cap - cc->recv_len;
    buf->base = cc->recv_buf + cc->recv_len;
    buf->len = (unsigned int)avail;
}

static void ctrl_on_read(uv_stream_t *stream, ssize_t nread, const uv_buf_t *buf)
{
    (void)buf;
    ctrl_client_t *cc = (ctrl_client_t *)stream;
    if (nread <= 0) {
        ctrl_close_client(cc);
        return;
    }

    cc->recv_len += (size_t)nread;
    if (cc->recv_len > CTRL_MAX_MSG) {
        ctrl_close_client(cc);
        return;
    }

    // Process complete newline-delimited messages.
    char *base = cc->recv_buf;
    size_t remaining = cc->recv_len;
    while (remaining > 0) {
        char *nl = memchr(base, '\n', remaining);
        if (!nl) break;

        size_t line_len = (size_t)(nl - base);
        *nl = '\0';
        ctrl_process_line(cc, base, line_len);

        line_len++;
        base += line_len;
        remaining -= line_len;
    }

    if (remaining > 0 && base != cc->recv_buf)
        memmove(cc->recv_buf, base, remaining);
    cc->recv_len = remaining;
}

static void ctrl_on_connection(uv_stream_t *server, int status)
{
    if (status < 0) return;

    trellis_ctrl_t *ctrl = (trellis_ctrl_t *)server->data;
    ctrl_client_t *cc = ctrl_accept_client(ctrl);
    if (!cc) return;

    uv_pipe_init(server->loop, &cc->pipe, 0);

    if (uv_accept(server, (uv_stream_t *)&cc->pipe) != 0) {
        ctrl_close_client(cc);
        return;
    }

    uv_read_start((uv_stream_t *)&cc->pipe, ctrl_on_alloc, ctrl_on_read);
}

// Public API.
trellis_ctrl_t *trellis_ctrl_new(trellis_client_t *client,
                                 uv_loop_t *loop,
                                 const char *socket_path,
                                 const char *cookie_path)
{
    if (!client || !loop || !socket_path)
        return NULL;

    trellis_ctrl_t *ctrl = calloc(1, sizeof(*ctrl));
    if (!ctrl) return NULL;

    ctrl->client = client;
    ctrl->socket_path = strdup(socket_path);
    if (!ctrl->socket_path) { free(ctrl); return NULL; }

    uv_pipe_init(loop, &ctrl->server, 0);
    ctrl->server.data = ctrl;

    // Remove stale socket file.
    trellis_unlink(socket_path);

    int r = uv_pipe_bind(&ctrl->server, socket_path);
    if (r != 0) {
        free(ctrl->socket_path);
        free(ctrl);
        return NULL;
    }

    // Restrict socket permissions.
    trellis_chmod(socket_path, 0700);

    r = uv_listen((uv_stream_t *)&ctrl->server, 4, ctrl_on_connection);
    if (r != 0) {
        uv_close((uv_handle_t *)&ctrl->server, NULL);
        free(ctrl->socket_path);
        free(ctrl);
        return NULL;
    }

    ctrl->running = true;

    if (cookie_path)
        ctrl_generate_cookie(ctrl, cookie_path);

    return ctrl;
}

static void ctrl_server_closed(uv_handle_t *handle)
{
    (void)handle;
}

void trellis_ctrl_free(trellis_ctrl_t *ctrl)
{
    if (!ctrl) return;

    for (size_t i = 0; i < ctrl->client_count; i++) {
        if (ctrl->clients[i] && !ctrl->clients[i]->closing)
            ctrl_close_client(ctrl->clients[i]);
    }

    if (ctrl->running) {
        uv_close((uv_handle_t *)&ctrl->server, ctrl_server_closed);
        ctrl->running = false;
    }

    if (ctrl->socket_path) {
        trellis_unlink(ctrl->socket_path);
        free(ctrl->socket_path);
    }
    if (ctrl->cookie_path) {
        trellis_unlink(ctrl->cookie_path);
        free(ctrl->cookie_path);
    }

    sodium_memzero(ctrl->cookie, CTRL_COOKIE_LEN);
    free(ctrl);
}

const char *trellis_ctrl_socket_path(const trellis_ctrl_t *ctrl)
{
    return ctrl ? ctrl->socket_path : NULL;
}
