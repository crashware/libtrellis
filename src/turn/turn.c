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
 * TURN protocol handler (RFC 5766 subset)
 *
 * Implements the server side of TURN: Allocate, Refresh, CreatePermission,
 * ChannelBind, Send/Data, and ChannelData. Authentication uses the
 * coturn-compatible "use-auth-secret" HMAC-SHA1 scheme.
 */

#include <trellis/turn.h>
#include <trellis/types.h>
#include <trellis/error.h>

#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sodium.h>

#define TURN_LOG_DEBUG  0
#define TURN_LOG_INFO   1
#define TURN_LOG_WARN   2
#define TURN_LOG_ERROR  3

static void turn_log(const turn_server_t *srv, int level, const char *fmt, ...)
{
    char buf[512];
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(buf, sizeof(buf), fmt, ap);
    va_end(ap);
    if (srv->log_cb)
        srv->log_cb(level, buf, srv->log_ctx);
    else
        fprintf(stderr, "[TURN] %s\n", buf);
}

static inline void wr16(uint8_t *p, uint16_t v)
{
    p[0] = (uint8_t)(v >> 8);
    p[1] = (uint8_t)(v);
}

static inline void wr32(uint8_t *p, uint32_t v)
{
    p[0] = (uint8_t)(v >> 24);
    p[1] = (uint8_t)(v >> 16);
    p[2] = (uint8_t)(v >> 8);
    p[3] = (uint8_t)(v);
}

static inline uint16_t rd16(const uint8_t *p)
{
    return (uint16_t)((p[0] << 8) | p[1]);
}

static bool addr_eq(const stun_addr_t *a, const stun_addr_t *b)
{
    if (a->family != b->family || a->port != b->port) return false;
    if (a->family == 0x01)
        return memcmp(a->ip.v4, b->ip.v4, 4) == 0;
    if (a->family == 0x02)
        return memcmp(a->ip.v6, b->ip.v6, 16) == 0;
    return false;
}

static bool addr_ip_eq(const stun_addr_t *a, const stun_addr_t *b)
{
    if (a->family != b->family) return false;
    if (a->family == 0x01)
        return memcmp(a->ip.v4, b->ip.v4, 4) == 0;
    if (a->family == 0x02)
        return memcmp(a->ip.v6, b->ip.v6, 16) == 0;
    return false;
}

/* Derive the auth key for a STUN message given the shared secret.
 * password = Base64(HMAC-SHA1(secret, username))
 * key = MD5(username:realm:password) */
static bool derive_auth_key(const stun_msg_t *msg, const char *secret,
                            const char *realm, uint8_t key_out[16])
{
    if (!msg->username || !secret || !realm) return false;

    // HMAC-SHA1(secret, username)
    uint8_t raw_mac[20];
    /* Use stun_verify_auth_secret's approach: we need the internal hmac_sha1.
     * Instead, replicate the key derivation here. */
    extern void hmac_sha1(const uint8_t *, size_t, const uint8_t *, size_t, uint8_t[20]);
    extern void md5_hash(const uint8_t *, size_t, uint8_t[16]);

    hmac_sha1((const uint8_t *)secret, strlen(secret),
              msg->username, msg->username_len, raw_mac);

    // FIXME: use sodium_bin2base64 instead of hand-rolling this
    static const char b64[] =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    char password[32];
    size_t pi = 0;
    for (size_t i = 0; i < 20; i += 3) {
        uint32_t n = ((uint32_t)raw_mac[i] << 16);
        if (i + 1 < 20) n |= ((uint32_t)raw_mac[i+1] << 8);
        if (i + 2 < 20) n |= (uint32_t)raw_mac[i+2];
        password[pi++] = b64[(n >> 18) & 0x3F];
        password[pi++] = b64[(n >> 12) & 0x3F];
        password[pi++] = (i + 1 < 20) ? b64[(n >> 6) & 0x3F] : '=';
        password[pi++] = (i + 2 < 20) ? b64[n & 0x3F] : '=';
    }
    password[pi] = '\0';

    // MD5(username:realm:password)
    size_t ulen = msg->username_len;
    size_t rlen = strlen(realm);
    size_t total = ulen + 1 + rlen + 1 + pi;
    uint8_t *input = malloc(total);
    if (!input) return false;

    memcpy(input, msg->username, ulen);
    input[ulen] = ':';
    memcpy(input + ulen + 1, realm, rlen);
    input[ulen + 1 + rlen] = ':';
    memcpy(input + ulen + 1 + rlen + 1, password, pi);

    md5_hash(input, total, key_out);
    free(input);
    return true;
}

/* Nonces are derived from nonce_key + epoch so the server can validate
 * them statelessly. Epoch rotates every 5 minutes; both current and
 * previous epoch are accepted to avoid races. */
#define TURN_NONCE_EPOCH_SECS 300

static void generate_nonce(turn_server_t *srv, uint8_t nonce[TURN_NONCE_LEN])
{
    uint64_t epoch = trellis_now_ms() / 1000 / TURN_NONCE_EPOCH_SECS;
    srv->nonce_epoch = epoch;

    uint8_t input[40]; /* 32 (key) + 8 (epoch) */
    memcpy(input, srv->nonce_key, 32);
    for (int i = 0; i < 8; i++)
        input[32 + i] = (uint8_t)(epoch >> (56 - i * 8));

    uint8_t hash[crypto_generichash_BYTES];
    crypto_generichash(hash, sizeof(hash), input, sizeof(input), NULL, 0);
    memcpy(nonce, hash, TURN_NONCE_LEN);
}

static bool verify_nonce(const turn_server_t *srv, const stun_msg_t *msg)
{
    if (!msg->nonce || msg->nonce_len != TURN_NONCE_LEN)
        return false;

    uint64_t now_epoch = trellis_now_ms() / 1000 / TURN_NONCE_EPOCH_SECS;

    for (int delta = 0; delta <= 1; delta++) {
        uint64_t epoch = now_epoch - (uint64_t)delta;
        uint8_t input[40];
        memcpy(input, srv->nonce_key, 32);
        for (int i = 0; i < 8; i++)
            input[32 + i] = (uint8_t)(epoch >> (56 - i * 8));

        uint8_t hash[crypto_generichash_BYTES];
        crypto_generichash(hash, sizeof(hash), input, sizeof(input), NULL, 0);

        if (sodium_memcmp(msg->nonce, hash, TURN_NONCE_LEN) == 0)
            return true;
    }
    return false;
}

// Build and send a 401 Unauthorized error with realm + nonce.
static void send_unauth(turn_server_t *srv, const stun_msg_t *req,
                        uint16_t resp_type, const stun_addr_t *dst)
{
    uint8_t attrs[512];
    size_t apos = 0;

    uint8_t nonce[TURN_NONCE_LEN];
    generate_nonce(srv, nonce);

    apos += stun_attr_error_code(STUN_ERR_UNAUTHORIZED, "Unauthorized",
                                 attrs + apos, sizeof(attrs) - apos);
    apos += stun_attr_realm(srv->realm, attrs + apos, sizeof(attrs) - apos);
    apos += stun_attr_nonce(nonce, TURN_NONCE_LEN,
                            attrs + apos, sizeof(attrs) - apos);

    uint8_t buf[1024];
    size_t blen = stun_build_response(resp_type, req->txn_id,
                                      attrs, apos, NULL, 0,
                                      buf, sizeof(buf));
    if (blen > 0 && srv->send_cb)
        srv->send_cb(buf, blen, dst, srv->send_ctx);
}

static void send_error(turn_server_t *srv, const stun_msg_t *req,
                       uint16_t resp_type, uint16_t code, const char *reason,
                       const uint8_t *key, size_t key_len,
                       const stun_addr_t *dst)
{
    uint8_t attrs[512];
    size_t apos = 0;
    apos += stun_attr_error_code(code, reason,
                                 attrs + apos, sizeof(attrs) - apos);

    if (code == STUN_ERR_STALE_NONCE) {
        uint8_t nonce[TURN_NONCE_LEN];
        generate_nonce(srv, nonce);
        apos += stun_attr_realm(srv->realm, attrs + apos, sizeof(attrs) - apos);
        apos += stun_attr_nonce(nonce, TURN_NONCE_LEN,
                                attrs + apos, sizeof(attrs) - apos);
    }

    uint8_t buf[1024];
    size_t blen = stun_build_response(resp_type, req->txn_id,
                                      attrs, apos, key, key_len,
                                      buf, sizeof(buf));
    if (blen > 0 && srv->send_cb)
        srv->send_cb(buf, blen, dst, srv->send_ctx);
}

static void handle_binding(turn_server_t *srv, const stun_msg_t *req,
                           const stun_addr_t *client)
{
    uint16_t resp_type = STUN_METHOD_BINDING | STUN_CLASS_SUCCESS;
    uint8_t attrs[128];
    size_t apos = 0;

    apos += stun_attr_xor_mapped_address(client, req->txn_id,
                                         attrs + apos, sizeof(attrs) - apos);
    apos += stun_attr_software("vine-relay/1.0",
                               attrs + apos, sizeof(attrs) - apos);

    uint8_t buf[1024];
    size_t blen = stun_build_response(resp_type, req->txn_id,
                                      attrs, apos, NULL, 0,
                                      buf, sizeof(buf));
    if (blen > 0 && srv->send_cb)
        srv->send_cb(buf, blen, client, srv->send_ctx);
}

// --- Authentication check ---

/* Returns true if auth passes, false otherwise (sends 401 if needed).
 * On success, populates key_out with the long-term auth key. */
static bool check_auth(turn_server_t *srv, const stun_msg_t *req,
                       uint16_t error_type, const stun_addr_t *client,
                       uint8_t key_out[16])
{
    if (!req->username || !req->has_integrity) {
        send_unauth(srv, req, error_type, client);
        return false;
    }

    if (!verify_nonce(srv, req)) {
        send_error(srv, req, error_type,
                   STUN_ERR_STALE_NONCE, "Stale Nonce",
                   NULL, 0, client);
        return false;
    }

    /* Validate credential expiry: username format is "timestamp:name".
     * The timestamp is a "valid until" Unix epoch (set to now + TTL by
     * the signaling server). Reject if expired. */
    if (req->username_len > 0) {
        char ts_buf[21] = {0}; /* max 20 digits for uint64 */
        size_t ts_len = 0;
        while (ts_len < req->username_len && ts_len < sizeof(ts_buf) - 1 &&
               req->username[ts_len] != ':')
            ts_buf[ts_len] = (char)req->username[ts_len], ts_len++;
        if (ts_len > 0 && ts_len <= 10) { /* Unix epoch fits in 10 digits until 2286 */
            uint64_t ts = 0;
            bool valid_digits = true;
            for (size_t i = 0; i < ts_len; i++) {
                if (ts_buf[i] < '0' || ts_buf[i] > '9') { valid_digits = false; break; }
                ts = ts * 10 + (uint64_t)(ts_buf[i] - '0');
            }
            if (valid_digits && ts > 0) {
                uint64_t now_s = trellis_now_ms() / 1000;
                if (ts < now_s) {
                    send_unauth(srv, req, error_type, client);
                    return false;
                }
            }
        }
    }

    if (!derive_auth_key(req, srv->secret, srv->realm, key_out)) {
        send_unauth(srv, req, error_type, client);
        return false;
    }
    if (!stun_verify_integrity(req, key_out, 16)) {
        send_error(srv, req, error_type,
                   STUN_ERR_WRONG_CREDENTIALS, "Wrong Credentials",
                   NULL, 0, client);
        return false;
    }
    return true;
}

// --- Allocate ---
static void handle_allocate(turn_server_t *srv, const stun_msg_t *req,
                            const stun_addr_t *client, uint8_t transport)
{
    uint16_t err_type = TURN_METHOD_ALLOCATE | STUN_CLASS_ERROR;
    uint8_t auth_key[16];

    if (!check_auth(srv, req, err_type, client, auth_key))
        return;

    // Must request UDP transport.
    if (!req->has_requested_transport || req->requested_transport != 17) {
        send_error(srv, req, err_type,
                   STUN_ERR_UNSUPPORTED_PROTO, "UDP only",
                   auth_key, 16, client);
        return;
    }

    // Check for existing allocation from this client.
    turn_allocation_t *existing = turn_server_find_alloc_by_client(srv, client);
    if (existing) {
        send_error(srv, req, err_type,
                   STUN_ERR_ALLOCATION_MISMATCH, "Allocation exists",
                   auth_key, 16, client);
        return;
    }

    // Check quota.
    if (srv->alloc_count >= srv->max_allocations) {
        send_error(srv, req, err_type,
                   STUN_ERR_ALLOCATION_QUOTA, "Quota exceeded",
                   auth_key, 16, client);
        return;
    }

    // Find free slot.
    turn_allocation_t *alloc = NULL;
    for (size_t i = 0; i < TURN_MAX_ALLOCATIONS; i++) {
        if (!srv->allocations[i].active) {
            alloc = &srv->allocations[i];
            break;
        }
    }
    if (!alloc) {
        send_error(srv, req, err_type,
                   STUN_ERR_INSUFFICIENT_CAP, "No capacity",
                   auth_key, 16, client);
        return;
    }

    // Initialize allocation.
    memset(alloc, 0, sizeof(*alloc));
    alloc->active = true;
    alloc->id = srv->next_alloc_id++;
    alloc->client_addr = *client;
    alloc->client_transport = transport;
    memcpy(alloc->auth_key, auth_key, 16);

    size_t ulen = req->username_len;
    if (ulen >= sizeof(alloc->username)) ulen = sizeof(alloc->username) - 1;
    memcpy(alloc->username, req->username, ulen);
    alloc->username[ulen] = '\0';

    generate_nonce(srv, alloc->nonce);

    uint32_t lifetime = srv->default_lifetime;
    if (req->has_lifetime && req->lifetime < lifetime)
        lifetime = req->lifetime;
    if (lifetime > srv->max_lifetime)
        lifetime = srv->max_lifetime;

    uint64_t now = trellis_now_ms();
    alloc->created_at_ms = now;
    alloc->expires_at_ms = now + (uint64_t)lifetime * 1000;
    alloc->max_bps = srv->max_bps;

    /* Assign relay address (virtual, based on external IP + incremented port).
     * Scan forward from next_relay_port to find a port not already in use. */
    alloc->relay_addr = srv->external_addr;
    {
        uint32_t start = srv->next_relay_port;
        uint32_t candidate = start;
        bool found = false;
        do {
            bool in_use = false;
            for (size_t j = 0; j < TURN_MAX_ALLOCATIONS; j++) {
                if (&srv->allocations[j] == alloc)
                    continue;
                if (srv->allocations[j].active &&
                    srv->allocations[j].relay_addr.port == (uint16_t)candidate) {
                    in_use = true;
                    break;
                }
            }
            if (!in_use) { found = true; break; }
            candidate++;
            if (candidate > srv->relay_port_max)
                candidate = srv->relay_port_min;
        } while (candidate != start);

        if (!found) {
            alloc->active = false;
            send_error(srv, req, err_type,
                       STUN_ERR_INSUFFICIENT_CAP, "No relay ports available",
                       auth_key, 16, client);
            return;
        }
        alloc->relay_addr.port = (uint16_t)candidate;
        uint32_t next = candidate + 1;
        if (next > srv->relay_port_max)
            next = srv->relay_port_min;
        srv->next_relay_port = (uint16_t)next;
    }

    srv->alloc_count++;

    turn_log(srv, TURN_LOG_INFO, "Allocate: %s → relay port %u (lifetime %us, id=%lu)",
             alloc->username, alloc->relay_addr.port, lifetime,
             (unsigned long)alloc->id);

    // Send success response.
    uint16_t ok_type = TURN_METHOD_ALLOCATE | STUN_CLASS_SUCCESS;
    uint8_t attrs[256];
    size_t apos = 0;

    apos += stun_attr_xor_relayed_address(&alloc->relay_addr, req->txn_id,
                                          attrs + apos, sizeof(attrs) - apos);
    apos += stun_attr_lifetime(lifetime, attrs + apos, sizeof(attrs) - apos);
    apos += stun_attr_xor_mapped_address(client, req->txn_id,
                                         attrs + apos, sizeof(attrs) - apos);
    apos += stun_attr_software("vine-relay/1.0",
                               attrs + apos, sizeof(attrs) - apos);

    uint8_t buf[1024];
    size_t blen = stun_build_response(ok_type, req->txn_id,
                                      attrs, apos, auth_key, 16,
                                      buf, sizeof(buf));
    if (blen > 0 && srv->send_cb)
        srv->send_cb(buf, blen, client, srv->send_ctx);
}

// --- Refresh ---
static void handle_refresh(turn_server_t *srv, const stun_msg_t *req,
                           const stun_addr_t *client)
{
    uint16_t err_type = TURN_METHOD_REFRESH | STUN_CLASS_ERROR;
    uint8_t auth_key[16];

    if (!check_auth(srv, req, err_type, client, auth_key))
        return;

    turn_allocation_t *alloc = turn_server_find_alloc_by_client(srv, client);
    if (!alloc) {
        send_error(srv, req, err_type,
                   STUN_ERR_ALLOCATION_MISMATCH, "No allocation",
                   auth_key, 16, client);
        return;
    }

    uint32_t lifetime = 0;
    if (req->has_lifetime)
        lifetime = req->lifetime;
    else
        lifetime = srv->default_lifetime;

    if (lifetime > srv->max_lifetime)
        lifetime = srv->max_lifetime;

    if (lifetime == 0) {
        // Delete allocation and clear sensitive fields.
        turn_log(srv, TURN_LOG_INFO, "Refresh: %s deallocated", alloc->username);
        if (srv->alloc_count > 0) srv->alloc_count--;
        memset(alloc->username, 0, sizeof(alloc->username));
        memset(alloc->auth_key, 0, sizeof(alloc->auth_key));
        memset(alloc->permissions, 0, sizeof(alloc->permissions));
        memset(alloc->channels, 0, sizeof(alloc->channels));
        alloc->active = false;
    } else {
        alloc->expires_at_ms = trellis_now_ms() + (uint64_t)lifetime * 1000;
        turn_log(srv, TURN_LOG_INFO, "Refresh: %s → %us", alloc->username, lifetime);
    }

    uint16_t ok_type = TURN_METHOD_REFRESH | STUN_CLASS_SUCCESS;
    uint8_t attrs[64];
    size_t apos = 0;
    apos += stun_attr_lifetime(lifetime, attrs + apos, sizeof(attrs) - apos);

    uint8_t buf[1024];
    size_t blen = stun_build_response(ok_type, req->txn_id,
                                      attrs, apos, auth_key, 16,
                                      buf, sizeof(buf));
    if (blen > 0 && srv->send_cb)
        srv->send_cb(buf, blen, client, srv->send_ctx);
}

// --- CreatePermission ---
static void handle_create_permission(turn_server_t *srv, const stun_msg_t *req,
                                     const stun_addr_t *client)
{
    uint16_t err_type = TURN_METHOD_CREATE_PERM | STUN_CLASS_ERROR;
    uint8_t auth_key[16];

    if (!check_auth(srv, req, err_type, client, auth_key))
        return;

    turn_allocation_t *alloc = turn_server_find_alloc_by_client(srv, client);
    if (!alloc) {
        send_error(srv, req, err_type,
                   STUN_ERR_ALLOCATION_MISMATCH, "No allocation",
                   auth_key, 16, client);
        return;
    }

    if (!req->has_xor_peer_addr) {
        send_error(srv, req, err_type,
                   STUN_ERR_BAD_REQUEST, "Missing peer address",
                   auth_key, 16, client);
        return;
    }

    // Find or create permission slot.
    uint64_t now = trellis_now_ms();
    uint64_t expires = now + (uint64_t)TURN_PERMISSION_LIFETIME * 1000;
    turn_permission_t *perm = NULL;

    for (size_t i = 0; i < TURN_MAX_PERMISSIONS; i++) {
        if (alloc->permissions[i].active &&
            addr_ip_eq(&alloc->permissions[i].peer_addr, &req->xor_peer_addr)) {
            perm = &alloc->permissions[i];
            break;
        }
    }
    if (!perm) {
        for (size_t i = 0; i < TURN_MAX_PERMISSIONS; i++) {
            if (!alloc->permissions[i].active) {
                perm = &alloc->permissions[i];
                break;
            }
        }
    }
    if (!perm) {
        // Evict oldest.
        uint64_t oldest = UINT64_MAX;
        for (size_t i = 0; i < TURN_MAX_PERMISSIONS; i++) {
            if (alloc->permissions[i].expires_at_ms < oldest) {
                oldest = alloc->permissions[i].expires_at_ms;
                perm = &alloc->permissions[i];
            }
        }
    }

    perm->active = true;
    perm->peer_addr = req->xor_peer_addr;
    perm->expires_at_ms = expires;

    uint16_t ok_type = TURN_METHOD_CREATE_PERM | STUN_CLASS_SUCCESS;
    uint8_t buf[1024];
    size_t blen = stun_build_response(ok_type, req->txn_id,
                                      NULL, 0, auth_key, 16,
                                      buf, sizeof(buf));
    if (blen > 0 && srv->send_cb)
        srv->send_cb(buf, blen, client, srv->send_ctx);
}

// --- ChannelBind ---
static void handle_channel_bind(turn_server_t *srv, const stun_msg_t *req,
                                const stun_addr_t *client)
{
    uint16_t err_type = TURN_METHOD_CHANNEL_BIND | STUN_CLASS_ERROR;
    uint8_t auth_key[16];

    if (!check_auth(srv, req, err_type, client, auth_key))
        return;

    turn_allocation_t *alloc = turn_server_find_alloc_by_client(srv, client);
    if (!alloc) {
        send_error(srv, req, err_type,
                   STUN_ERR_ALLOCATION_MISMATCH, "No allocation",
                   auth_key, 16, client);
        return;
    }

    if (!req->has_channel_number || !req->has_xor_peer_addr) {
        send_error(srv, req, err_type,
                   STUN_ERR_BAD_REQUEST, "Missing channel/peer",
                   auth_key, 16, client);
        return;
    }

    if (req->channel_number < TURN_CHANNEL_MIN ||
        req->channel_number > TURN_CHANNEL_MAX) {
        send_error(srv, req, err_type,
                   STUN_ERR_BAD_REQUEST, "Invalid channel number",
                   auth_key, 16, client);
        return;
    }

    // Check for conflicting bindings.
    for (size_t i = 0; i < TURN_MAX_CHANNELS; i++) {
        if (!alloc->channels[i].active) continue;
        if (alloc->channels[i].number == req->channel_number &&
            !addr_eq(&alloc->channels[i].peer_addr, &req->xor_peer_addr)) {
            send_error(srv, req, err_type,
                       STUN_ERR_BAD_REQUEST, "Channel conflict",
                       auth_key, 16, client);
            return;
        }
        if (addr_eq(&alloc->channels[i].peer_addr, &req->xor_peer_addr) &&
            alloc->channels[i].number != req->channel_number) {
            send_error(srv, req, err_type,
                       STUN_ERR_BAD_REQUEST, "Peer already bound",
                       auth_key, 16, client);
            return;
        }
    }

    // Find or create channel slot.
    uint64_t now = trellis_now_ms();
    uint64_t expires = now + (uint64_t)TURN_CHANNEL_LIFETIME * 1000;
    turn_channel_t *ch = NULL;

    for (size_t i = 0; i < TURN_MAX_CHANNELS; i++) {
        if (alloc->channels[i].active &&
            alloc->channels[i].number == req->channel_number) {
            ch = &alloc->channels[i];
            break;
        }
    }
    if (!ch) {
        for (size_t i = 0; i < TURN_MAX_CHANNELS; i++) {
            if (!alloc->channels[i].active) {
                ch = &alloc->channels[i];
                break;
            }
        }
    }
    if (!ch) {
        send_error(srv, req, err_type,
                   STUN_ERR_INSUFFICIENT_CAP, "No channel slots",
                   auth_key, 16, client);
        return;
    }

    ch->active = true;
    ch->number = req->channel_number;
    ch->peer_addr = req->xor_peer_addr;
    ch->expires_at_ms = expires;

    // Also install/refresh permission for this peer.
    turn_permission_t *perm = NULL;
    for (size_t i = 0; i < TURN_MAX_PERMISSIONS; i++) {
        if (alloc->permissions[i].active &&
            addr_ip_eq(&alloc->permissions[i].peer_addr, &req->xor_peer_addr)) {
            perm = &alloc->permissions[i];
            break;
        }
    }
    if (!perm) {
        for (size_t i = 0; i < TURN_MAX_PERMISSIONS; i++) {
            if (!alloc->permissions[i].active) {
                perm = &alloc->permissions[i];
                break;
            }
        }
    }
    if (!perm) {
        // Evict oldest permission (RFC 5766 §11: ChannelBind implies permission)
        uint64_t oldest = UINT64_MAX;
        for (size_t i = 0; i < TURN_MAX_PERMISSIONS; i++) {
            if (alloc->permissions[i].expires_at_ms < oldest) {
                oldest = alloc->permissions[i].expires_at_ms;
                perm = &alloc->permissions[i];
            }
        }
    }
    if (perm) {
        perm->active = true;
        perm->peer_addr = req->xor_peer_addr;
        perm->expires_at_ms = expires;
    }

    uint16_t ok_type = TURN_METHOD_CHANNEL_BIND | STUN_CLASS_SUCCESS;
    uint8_t buf[1024];
    size_t blen = stun_build_response(ok_type, req->txn_id,
                                      NULL, 0, auth_key, 16,
                                      buf, sizeof(buf));
    if (blen > 0 && srv->send_cb)
        srv->send_cb(buf, blen, client, srv->send_ctx);
}

// --- Send Indication ---
static bool check_bps(turn_allocation_t *alloc, size_t len)
{
    if (alloc->max_bps == 0)
        return true;

    uint64_t now = trellis_now_ms();
    if (now - alloc->bytes_window_start >= 1000) {
        alloc->bytes_window_start = now;
        alloc->bytes_this_window = 0;
    }

    if (alloc->bytes_this_window + len > alloc->max_bps)
        return false;

    alloc->bytes_this_window += len;
    return true;
}

static void handle_send(turn_server_t *srv, const stun_msg_t *req,
                        const stun_addr_t *client)
{
    turn_allocation_t *alloc = turn_server_find_alloc_by_client(srv, client);
    if (!alloc) return;

    if (!req->has_xor_peer_addr || !req->data_attr || req->data_attr_len == 0)
        return;

    if (!turn_alloc_has_permission(alloc, &req->xor_peer_addr))
        return;

    if (!check_bps(alloc, req->data_attr_len))
        return;

    alloc->bytes_relayed += req->data_attr_len;

    if (srv->relay_cb)
        srv->relay_cb(alloc->id, &req->xor_peer_addr,
                      req->data_attr, req->data_attr_len, srv->relay_ctx);
}

// --- ChannelData ---
static void handle_channel_data(turn_server_t *srv,
                                const uint8_t *data, size_t len,
                                const stun_addr_t *client)
{
    if (len < TURN_CHANNEL_HEADER_LEN) return;
    uint16_t ch_num = rd16(data);
    uint16_t ch_len = rd16(data + 2);

    if (ch_num < TURN_CHANNEL_MIN || ch_num > TURN_CHANNEL_MAX) return;
    if (TURN_CHANNEL_HEADER_LEN + (size_t)ch_len > len) return;

    turn_allocation_t *alloc = turn_server_find_alloc_by_client(srv, client);
    if (!alloc) return;

    stun_addr_t peer;
    if (!turn_alloc_find_peer_by_channel(alloc, ch_num, &peer))
        return;

    // RFC 5766 §11.7: verify permission before relaying.
    if (!turn_alloc_has_permission(alloc, &peer))
        return;

    if (!check_bps(alloc, ch_len))
        return;

    const uint8_t *payload = data + TURN_CHANNEL_HEADER_LEN;
    alloc->bytes_relayed += ch_len;

    if (srv->relay_cb)
        srv->relay_cb(alloc->id, &peer, payload, ch_len, srv->relay_ctx);
}

/* ══════════════════════════════════════════════════════════════════════════
 *  Public API
 * ══════════════════════════════════════════════════════════════════════════ */
trellis_err_t turn_server_init(turn_server_t *srv,
                               const char *realm,
                               const char *secret,
                               const stun_addr_t *external_addr)
{
    if (!srv || !realm || !secret || !external_addr)
        return TRELLIS_ERR_INVALID_ARG;

    memset(srv, 0, sizeof(*srv));

    size_t rlen = strlen(realm);
    if (rlen >= sizeof(srv->realm)) rlen = sizeof(srv->realm) - 1;
    memcpy(srv->realm, realm, rlen);

    size_t slen = strlen(secret);
    srv->secret = malloc(slen + 1);
    if (!srv->secret) return TRELLIS_ERR_NOMEM;
    memcpy(srv->secret, secret, slen);
    srv->secret[slen] = '\0';
    srv->secret_len = slen;

    srv->external_addr = *external_addr;
    srv->default_lifetime = TURN_DEFAULT_LIFETIME;
    srv->max_lifetime = TURN_MAX_LIFETIME;
    srv->max_allocations = TURN_MAX_ALLOCATIONS;
    srv->max_bps = 500000; /* 4 Mbps default */
    srv->relay_port_min = 49152;
    srv->relay_port_max = 50152;
    srv->next_relay_port = srv->relay_port_min;
    srv->next_alloc_id = 1;

    randombytes_buf(srv->nonce_key, sizeof(srv->nonce_key));

    return TRELLIS_OK;
}

void turn_server_destroy(turn_server_t *srv)
{
    if (!srv) return;
    if (srv->secret) {
        sodium_memzero(srv->secret, srv->secret_len);
        free(srv->secret);
        srv->secret = NULL;
    }
    sodium_memzero(srv->nonce_key, sizeof(srv->nonce_key));
    memset(srv, 0, sizeof(*srv));
}

trellis_err_t turn_server_process(turn_server_t *srv,
                                  const uint8_t *data, size_t len,
                                  const stun_addr_t *client_addr,
                                  uint8_t client_transport)
{
    if (!srv || !data || !client_addr)
        return TRELLIS_ERR_INVALID_ARG;

    // ChannelData (first two bytes in 0x4000-0x7FFF range)
    if (stun_is_channel_data(data, len)) {
        handle_channel_data(srv, data, len, client_addr);
        return TRELLIS_OK;
    }

    // STUN message.
    if (!stun_is_stun(data, len))
        return TRELLIS_ERR_MSG_FORMAT;

    stun_msg_t msg;
    trellis_err_t err = stun_parse(data, len, &msg);
    if (err != TRELLIS_OK) return err;

    // Extract method.
    uint16_t method = msg.type & 0x3EEF; /* mask out class bits */
    uint16_t cls    = msg.type & 0x0110;

    // Only handle requests and indications.
    if (cls != STUN_CLASS_REQUEST && cls != STUN_CLASS_INDICATION)
        return TRELLIS_OK;

    switch (method) {
    case STUN_METHOD_BINDING:
        handle_binding(srv, &msg, client_addr);
        break;
    case TURN_METHOD_ALLOCATE:
        handle_allocate(srv, &msg, client_addr, client_transport);
        break;
    case TURN_METHOD_REFRESH:
        handle_refresh(srv, &msg, client_addr);
        break;
    case TURN_METHOD_CREATE_PERM:
        handle_create_permission(srv, &msg, client_addr);
        break;
    case TURN_METHOD_CHANNEL_BIND:
        handle_channel_bind(srv, &msg, client_addr);
        break;
    case TURN_METHOD_SEND:
        handle_send(srv, &msg, client_addr);
        break;
    default:
        break;
    }

    return TRELLIS_OK;
}

trellis_err_t turn_server_deliver(turn_server_t *srv,
                                  uint64_t alloc_id,
                                  const stun_addr_t *peer_addr,
                                  const uint8_t *data, size_t len)
{
    if (!srv || !peer_addr || !data)
        return TRELLIS_ERR_INVALID_ARG;
    if (len > UINT16_MAX)
        return TRELLIS_ERR_OVERFLOW;

    turn_allocation_t *alloc = turn_server_find_alloc_by_id(srv, alloc_id);
    if (!alloc)
        return TRELLIS_ERR_NOT_FOUND;

    // Check permission.
    if (!turn_alloc_has_permission(alloc, peer_addr))
        return TRELLIS_ERR_PERMISSION;

    // Try ChannelData first (fast path)
    uint16_t ch = turn_alloc_find_channel(alloc, peer_addr);
    if (ch != 0) {
        size_t frame_len = TURN_CHANNEL_HEADER_LEN + len;
        // Pad to 4-byte boundary.
        size_t padded = frame_len + ((4 - (len % 4)) % 4);
        uint8_t *frame = calloc(1, padded);
        if (!frame) return TRELLIS_ERR_NOMEM;

        wr16(frame, ch);
        wr16(frame + 2, (uint16_t)len);
        memcpy(frame + TURN_CHANNEL_HEADER_LEN, data, len);

        if (srv->send_cb)
            srv->send_cb(frame, padded, &alloc->client_addr, srv->send_ctx);
        free(frame);
        return TRELLIS_OK;
    }

    /* Fall back to Data indication (slow path).
     * Max data = 65535, attrs overhead = ~16, header = 20, MI+FP = 32.
     * Use heap allocation for large payloads. */
    size_t padded_data = len + ((4 - (len % 4)) % 4);
    size_t attrs_cap = 32 + padded_data;
    uint8_t *attrs = malloc(attrs_cap);
    if (!attrs) return TRELLIS_ERR_NOMEM;

    /* Generate txn_id first so it's shared between XOR-PEER-ADDRESS
     * encoding and the STUN header (required for correct IPv6 XOR). */
    uint8_t ind_txn_id[STUN_TXN_ID_LEN];
    randombytes_buf(ind_txn_id, STUN_TXN_ID_LEN);

    size_t apos = 0;
    apos += stun_attr_xor_peer_address(peer_addr, ind_txn_id,
                                       attrs + apos, attrs_cap - apos);
    // DATA attribute.
    if (apos + 4 + padded_data > attrs_cap) {
        free(attrs);
        return TRELLIS_ERR_OVERFLOW;
    }
    wr16(attrs + apos, TURN_ATTR_DATA);
    wr16(attrs + apos + 2, (uint16_t)len);
    memcpy(attrs + apos + 4, data, len);
    if (padded_data > len)
        memset(attrs + apos + 4 + len, 0, padded_data - len);
    apos += 4 + padded_data;

    uint16_t ind_type = TURN_METHOD_DATA | STUN_CLASS_INDICATION;

    size_t buf_cap = 20 + apos + 32;
    uint8_t *buf = malloc(buf_cap);
    if (!buf) { free(attrs); return TRELLIS_ERR_NOMEM; }

    size_t blen = stun_build_response(ind_type, ind_txn_id,
                                      attrs, apos, NULL, 0,
                                      buf, buf_cap);
    if (blen > 0 && srv->send_cb)
        srv->send_cb(buf, blen, &alloc->client_addr, srv->send_ctx);

    free(attrs);
    free(buf);
    return TRELLIS_OK;
}

void turn_server_tick(turn_server_t *srv)
{
    if (!srv) return;
    uint64_t now = trellis_now_ms();

    for (size_t i = 0; i < TURN_MAX_ALLOCATIONS; i++) {
        turn_allocation_t *a = &srv->allocations[i];
        if (!a->active) continue;

        if (now >= a->expires_at_ms) {
            turn_log(srv, TURN_LOG_INFO, "Allocation expired: %s (id=%lu)",
                     a->username, (unsigned long)a->id);
            a->active = false;
            if (srv->alloc_count > 0) srv->alloc_count--;
            continue;
        }

        // Expire permissions.
        for (size_t j = 0; j < TURN_MAX_PERMISSIONS; j++) {
            if (a->permissions[j].active && now >= a->permissions[j].expires_at_ms)
                a->permissions[j].active = false;
        }

        // Expire channels.
        for (size_t j = 0; j < TURN_MAX_CHANNELS; j++) {
            if (a->channels[j].active && now >= a->channels[j].expires_at_ms)
                a->channels[j].active = false;
        }
    }
}

// --- Lookup helpers ---
turn_allocation_t *turn_server_find_alloc_by_client(turn_server_t *srv,
                                                     const stun_addr_t *client)
{
    if (!srv || !client) return NULL;
    for (size_t i = 0; i < TURN_MAX_ALLOCATIONS; i++) {
        if (srv->allocations[i].active &&
            addr_eq(&srv->allocations[i].client_addr, client))
            return &srv->allocations[i];
    }
    return NULL;
}

turn_allocation_t *turn_server_find_alloc_by_relay(turn_server_t *srv,
                                                    const stun_addr_t *relay)
{
    if (!srv || !relay) return NULL;
    for (size_t i = 0; i < TURN_MAX_ALLOCATIONS; i++) {
        if (srv->allocations[i].active &&
            addr_eq(&srv->allocations[i].relay_addr, relay))
            return &srv->allocations[i];
    }
    return NULL;
}

turn_allocation_t *turn_server_find_alloc_by_id(turn_server_t *srv,
                                                 uint64_t alloc_id)
{
    if (!srv) return NULL;
    for (size_t i = 0; i < TURN_MAX_ALLOCATIONS; i++) {
        if (srv->allocations[i].active && srv->allocations[i].id == alloc_id)
            return &srv->allocations[i];
    }
    return NULL;
}

bool turn_alloc_has_permission(const turn_allocation_t *alloc,
                               const stun_addr_t *peer)
{
    if (!alloc || !peer) return false;
    uint64_t now = trellis_now_ms();
    for (size_t i = 0; i < TURN_MAX_PERMISSIONS; i++) {
        if (alloc->permissions[i].active &&
            now < alloc->permissions[i].expires_at_ms &&
            addr_ip_eq(&alloc->permissions[i].peer_addr, peer))
            return true;
    }
    return false;
}

uint16_t turn_alloc_find_channel(const turn_allocation_t *alloc,
                                 const stun_addr_t *peer)
{
    if (!alloc || !peer) return 0;
    uint64_t now = trellis_now_ms();
    for (size_t i = 0; i < TURN_MAX_CHANNELS; i++) {
        if (alloc->channels[i].active &&
            now < alloc->channels[i].expires_at_ms &&
            addr_eq(&alloc->channels[i].peer_addr, peer))
            return alloc->channels[i].number;
    }
    return 0;
}

bool turn_alloc_find_peer_by_channel(const turn_allocation_t *alloc,
                                     uint16_t channel,
                                     stun_addr_t *peer_out)
{
    if (!alloc || !peer_out) return false;
    uint64_t now = trellis_now_ms();
    for (size_t i = 0; i < TURN_MAX_CHANNELS; i++) {
        if (alloc->channels[i].active &&
            alloc->channels[i].number == channel &&
            now < alloc->channels[i].expires_at_ms) {
            *peer_out = alloc->channels[i].peer_addr;
            return true;
        }
    }
    return false;
}
