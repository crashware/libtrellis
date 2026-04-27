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
 * canopy.c — Canopy Inter-Grove Routing Protocol
 *
 * The Canopy protocol enables path-vector routing between groves.  Bridge
 * nodes exchange reachability announcements that propagate across grove
 * boundaries, building a routing table of grove_id -> next_bridge_fp paths.
 *
 * Announcement format (normative: bloom_protocol_whitepaper.md Appendix B.6):
 *   "CANO" magic(4) || version(1) || origin_grove_id(32) ||
 *   path_len(1) || grove_id_path(32 * path_len) || bridge_fp(32) ||
 *   bridge_ed25519_pk(32) || capabilities(4) || ttl(1) ||
 *   timestamp(8) || ed25519_sig(64)
 *
 * Cross-grove circuits are constructed as nested onion tunnels that
 * traverse bridge chains.  Each grove boundary adds one encryption
 * layer; the client's inner onion layers remain opaque to bridges.
 */

#include "internal.h"
#include <trellis/rhizome.h>
#include <sodium.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>

#define CANOPY_MAGIC_LEN      4
#define CANOPY_HEADER_LEN     (CANOPY_MAGIC_LEN + 1 + 32 + 1) /* magic+ver+origin+pathlen */
#define CANOPY_TRAILER_LEN    (32 + 32 + 4 + 1 + 8 + 64) /* bridge_fp+ed25519_pk+caps+ttl+ts+sig */
#define CANOPY_MAX_RAW        (CANOPY_HEADER_LEN + 32 * TRELLIS_CANOPY_MAX_PATH_LEN + CANOPY_TRAILER_LEN)
#define CANOPY_DEFAULT_TTL    8

// Per-bridge announcement rate limiting.
#define CANOPY_RATE_LIMIT_SLOTS  64
#define CANOPY_RATE_LIMIT_MAX    5   /* max announcements per window */
#define CANOPY_RATE_LIMIT_WINDOW 60000 /* 60 seconds */

typedef struct canopy_rate_entry {
    trellis_fingerprint_t bridge_fp;
    uint32_t              count;
    uint64_t              window_start;
} canopy_rate_entry_t;

#define CANOPY_MAX_CIRCUIT_PAYLOAD  (64 * 1024) /* 64 KiB per relay hop */

#define CANOPY_MAX_CIRCUITS 64

typedef struct canopy_circuit {
    bool                  active;
    uint32_t              circuit_id;
    trellis_grove_id_t    target_grove;
    trellis_fingerprint_t dest;
    trellis_fingerprint_t next_bridge;
    trellis_fingerprint_t source;
    size_t                grove_index;
    uint64_t              created_at;
    trellis_canopy_app_recv_fn app_recv;
    void                 *app_recv_ctx;
} canopy_circuit_t;

// --- Canopy Manager ---
struct trellis_canopy {
    trellis_bridge_t     *bridge;
    uv_loop_t            *loop;
    uv_timer_t            announce_timer;
    bool                  running;
    bool                  timer_initialized;

    trellis_canopy_route_t routes[TRELLIS_CANOPY_MAX_ROUTES];
    size_t                 route_count;

    canopy_circuit_t       circuits[CANOPY_MAX_CIRCUITS];
    uint32_t               next_circuit_id;

    canopy_rate_entry_t    rate_limits[CANOPY_RATE_LIMIT_SLOTS];

    trellis_bridge_trust_t *trust;
};

// --- Announcement Serialization ---
static size_t serialize_announcement(const trellis_grove_id_t *origin_grove,
                                     const trellis_grove_id_t *path,
                                     size_t path_len,
                                     const trellis_fingerprint_t *bridge_fp,
                                     const uint8_t *ed25519_pk,
                                     uint32_t capabilities,
                                     uint8_t ttl,
                                     uint64_t timestamp,
                                     uint8_t *buf, size_t cap)
{
    size_t need = CANOPY_HEADER_LEN + 32 * path_len + CANOPY_TRAILER_LEN;
    if (cap < need)
        return 0;

    size_t off = 0;
    memcpy(buf + off, TRELLIS_CANOPY_ANNOUNCE_MAGIC, 4); off += 4;
    buf[off++] = TRELLIS_CANOPY_ANNOUNCE_VERSION;
    memcpy(buf + off, origin_grove->bytes, 32); off += 32;
    buf[off++] = (uint8_t)path_len;

    for (size_t i = 0; i < path_len; i++) {
        memcpy(buf + off, path[i].bytes, 32);
        off += 32;
    }

    memcpy(buf + off, bridge_fp->bytes, 32); off += 32;
    memcpy(buf + off, ed25519_pk, 32); off += 32;

    buf[off++] = (uint8_t)(capabilities >> 24);
    buf[off++] = (uint8_t)(capabilities >> 16);
    buf[off++] = (uint8_t)(capabilities >> 8);
    buf[off++] = (uint8_t)(capabilities);

    buf[off++] = ttl;

    for (int i = 0; i < 8; i++)
        buf[off++] = (uint8_t)(timestamp >> ((7 - i) * 8));

    // Signature placeholder (64 bytes of zero, to be filled by caller)
    memset(buf + off, 0, 64);
    off += 64;

    return off;
}

static trellis_err_t deserialize_announcement(
    const uint8_t *data, size_t len,
    trellis_grove_id_t *origin_out,
    trellis_grove_id_t *path_out, size_t path_cap, size_t *path_len_out,
    trellis_fingerprint_t *bridge_fp_out,
    uint8_t *ed25519_pk_out,
    uint32_t *capabilities_out,
    uint8_t *ttl_out,
    uint64_t *timestamp_out,
    uint8_t *sig_out /* 64 bytes */)
{
    if (len < CANOPY_HEADER_LEN + CANOPY_TRAILER_LEN)
        return TRELLIS_ERR_MSG_FORMAT;

    size_t off = 0;
    if (memcmp(data + off, TRELLIS_CANOPY_ANNOUNCE_MAGIC, 4) != 0)
        return TRELLIS_ERR_MSG_FORMAT;
    off += 4;

    if (data[off++] != TRELLIS_CANOPY_ANNOUNCE_VERSION)
        return TRELLIS_ERR_MSG_FORMAT;

    memcpy(origin_out->bytes, data + off, 32); off += 32;

    size_t plen = data[off++];
    if (plen > TRELLIS_CANOPY_MAX_PATH_LEN || plen > path_cap)
        return TRELLIS_ERR_MSG_FORMAT;

    if (off + 32 * plen + CANOPY_TRAILER_LEN > len)
        return TRELLIS_ERR_MSG_FORMAT;

    for (size_t i = 0; i < plen; i++) {
        memcpy(path_out[i].bytes, data + off, 32);
        off += 32;
    }
    *path_len_out = plen;

    memcpy(bridge_fp_out->bytes, data + off, 32); off += 32;
    memcpy(ed25519_pk_out, data + off, 32); off += 32;

    *capabilities_out = ((uint32_t)data[off] << 24) | ((uint32_t)data[off+1] << 16) |
                        ((uint32_t)data[off+2] << 8) | (uint32_t)data[off+3];
    off += 4;

    *ttl_out = data[off++];

    *timestamp_out = 0;
    for (int i = 0; i < 8; i++)
        *timestamp_out |= (uint64_t)data[off++] << ((7 - i) * 8);

    memcpy(sig_out, data + off, 64);

    return TRELLIS_OK;
}

// --- Rate Limiting ---
static bool check_announce_rate(trellis_canopy_t *canopy,
                                const trellis_fingerprint_t *bridge_fp)
{
    uint64_t now = trellis_now_ms();
    int match = -1;
    int oldest_idx = 0;
    uint64_t oldest_ts = UINT64_MAX;

    for (int i = 0; i < CANOPY_RATE_LIMIT_SLOTS; i++) {
        canopy_rate_entry_t *r = &canopy->rate_limits[i];
        if (trellis_fingerprint_eq(&r->bridge_fp, bridge_fp)) {
            match = i;
            break;
        }
        if (r->window_start < oldest_ts) {
            oldest_ts = r->window_start;
            oldest_idx = i;
        }
    }

    canopy_rate_entry_t *slot;
    if (match >= 0) {
        slot = &canopy->rate_limits[match];
    } else {
        slot = &canopy->rate_limits[oldest_idx];
        slot->bridge_fp = *bridge_fp;
        slot->count = 0;
        slot->window_start = now;
    }

    if (now - slot->window_start >= CANOPY_RATE_LIMIT_WINDOW) {
        slot->window_start = now;
        slot->count = 0;
    }

    if (slot->count >= CANOPY_RATE_LIMIT_MAX)
        return false;

    slot->count++;
    return true;
}

// --- Loop Detection ---
static bool path_contains_grove(const trellis_grove_id_t *path, size_t path_len,
                                const trellis_grove_id_t *grove_id)
{
    for (size_t i = 0; i < path_len; i++) {
        if (trellis_grove_id_eq(&path[i], grove_id))
            return true;
    }
    return false;
}

// --- Periodic Announcement Timer ---
static void announce_timer_cb(uv_timer_t *handle)
{
    trellis_canopy_t *canopy = handle->data;
    trellis_canopy_announce(canopy);
}

/* Resolve which local grove index can reach a given target grove.
 * Returns 0 (home grove) if no specific match is found. */
static size_t resolve_grove_index(const trellis_canopy_t *canopy,
                                  const trellis_grove_id_t *grove_id)
{
    if (!canopy->bridge || !grove_id)
        return 0;
    for (size_t i = 0; i < TRELLIS_BRIDGE_MAX_GROVES; i++) {
        const trellis_grove_id_t *g = trellis_bridge_grove_at(canopy->bridge, i);
        if (g && trellis_grove_id_eq(g, grove_id))
            return i;
    }
    return 0;
}

static trellis_canopy_route_t *find_route(trellis_canopy_t *canopy,
                                           const trellis_grove_id_t *grove_id)
{
    for (size_t i = 0; i < canopy->route_count; i++) {
        if (trellis_grove_id_eq(&canopy->routes[i].target_grove, grove_id))
            return &canopy->routes[i];
    }
    return NULL;
}

static void expire_routes(trellis_canopy_t *canopy)
{
    uint64_t now = trellis_now_ms();
    size_t write = 0;
    for (size_t read = 0; read < canopy->route_count; read++) {
        if (now - canopy->routes[read].last_seen < TRELLIS_CANOPY_ROUTE_EXPIRE_MS) {
            if (write != read)
                canopy->routes[write] = canopy->routes[read];
            write++;
        }
    }
    canopy->route_count = write;
}

// --- Public API ---
trellis_canopy_t *trellis_canopy_new(trellis_bridge_t *bridge, uv_loop_t *loop)
{
    if (!bridge || !loop)
        return NULL;

    trellis_canopy_t *canopy = calloc(1, sizeof(*canopy));
    if (!canopy)
        return NULL;

    canopy->bridge = bridge;
    canopy->loop = loop;
    randombytes_buf(&canopy->next_circuit_id, sizeof(canopy->next_circuit_id));
    if (canopy->next_circuit_id == 0)
        canopy->next_circuit_id = 1;

    return canopy;
}

trellis_bridge_t *trellis_canopy_bridge(const trellis_canopy_t *canopy)
{
    return canopy ? canopy->bridge : NULL;
}

void trellis_canopy_set_trust(trellis_canopy_t *canopy,
                               trellis_bridge_trust_t *bt)
{
    if (canopy)
        canopy->trust = bt;
}

static void canopy_close_cb(uv_handle_t *handle)
{
    trellis_canopy_t *canopy = (trellis_canopy_t *)handle->data;
    free(canopy);
}

void trellis_canopy_free(trellis_canopy_t *canopy)
{
    if (!canopy)
        return;

    if (canopy->running)
        trellis_canopy_stop(canopy);

    // Tear down active circuits so remote bridges release resources.
    for (int i = 0; i < CANOPY_MAX_CIRCUITS; i++) {
        if (canopy->circuits[i].active) {
            canopy_circuit_t *circ = &canopy->circuits[i];
            uint32_t cid = circ->circuit_id;
            uint8_t destroy_msg[4] = {
                (uint8_t)(cid), (uint8_t)(cid >> 8),
                (uint8_t)(cid >> 16), (uint8_t)(cid >> 24)
            };
            const trellis_fingerprint_t *self_fp =
                trellis_bridge_fingerprint(canopy->bridge);
            const trellis_fingerprint_t *dest_peer = &circ->next_bridge;
            if (self_fp && trellis_fingerprint_eq(&circ->next_bridge, self_fp))
                dest_peer = &circ->source;
            trellis_bridge_dht_send(canopy->bridge,
                                    circ->grove_index,
                                    dest_peer,
                                    destroy_msg, sizeof(destroy_msg));
            circ->active = false;
        }
    }

    /* Close the timer handle properly via libuv before freeing.
     * Only attempt if the timer was initialized via uv_timer_init.
     * If the timer is already closing, canopy_close_cb will free. */
    if (canopy->timer_initialized) {
        if (!uv_is_closing((uv_handle_t *)&canopy->announce_timer))
            uv_close((uv_handle_t *)&canopy->announce_timer, canopy_close_cb);
        // else: close already in flight — canopy_close_cb will free.
    } else {
        free(canopy);
    }
}

trellis_err_t trellis_canopy_start(trellis_canopy_t *canopy)
{
    if (!canopy || canopy->running)
        return TRELLIS_ERR_INVALID_STATE;

    if (!canopy->timer_initialized) {
        uv_timer_init(canopy->loop, &canopy->announce_timer);
        canopy->announce_timer.data = canopy;
        canopy->timer_initialized = true;
    }
    uv_timer_start(&canopy->announce_timer, announce_timer_cb,
                    TRELLIS_CANOPY_ANNOUNCE_INTERVAL_MS,
                    TRELLIS_CANOPY_ANNOUNCE_INTERVAL_MS);
    canopy->running = true;

    // Initial announcement.
    trellis_canopy_announce(canopy);

    return TRELLIS_OK;
}

void trellis_canopy_stop(trellis_canopy_t *canopy)
{
    if (!canopy || !canopy->running)
        return;

    if (canopy->timer_initialized)
        uv_timer_stop(&canopy->announce_timer);
    canopy->running = false;
}

trellis_err_t trellis_canopy_announce(trellis_canopy_t *canopy)
{
    if (!canopy || !canopy->bridge)
        return TRELLIS_ERR_INVALID_ARG;

    trellis_bridge_t *bridge = canopy->bridge;

    /* For each pair of groves we participate in, announce reachability
     * of each grove to all other groves */
    for (size_t i = 0; i < TRELLIS_BRIDGE_MAX_GROVES; i++) {
        const trellis_grove_id_t *src_grove = trellis_bridge_grove_at(bridge, i);
        if (!src_grove)
            continue;

        for (size_t j = 0; j < TRELLIS_BRIDGE_MAX_GROVES; j++) {
            if (i == j)
                continue;

            const trellis_grove_id_t *dest_grove = trellis_bridge_grove_at(bridge, j);
            if (!dest_grove)
                continue;

            // Build announcement: "grove j is reachable through me, from grove i".
            uint8_t buf[CANOPY_MAX_RAW];
            trellis_grove_id_t path[1];
            path[0] = *src_grove;

            const trellis_fingerprint_t *our_fp =
                trellis_bridge_fingerprint(bridge);
            const trellis_identity_t *our_id =
                trellis_bridge_identity(bridge);
            if (!our_fp || !our_id)
                continue;

            size_t ann_len = serialize_announcement(
                dest_grove, path, 1, our_fp,
                our_id->ed25519_pk,
                0, /* capabilities */
                CANOPY_DEFAULT_TTL,
                trellis_now_ms(),
                buf, sizeof(buf));

            if (ann_len == 0)
                continue;

            // Sign the announcement (Ed25519 only for efficiency)
            size_t sig_off = ann_len - 64;
            crypto_sign_ed25519_detached(
                buf + sig_off, NULL,
                buf, sig_off,
                our_id->ed25519_sk);

            // Gossip the announcement within grove i.
            trellis_peer_info_t *peers[TRELLIS_GOSSIP_FAN_OUT];
            size_t peer_count = 0;
            trellis_bridge_select_peers(bridge, i,
                                        peers, TRELLIS_GOSSIP_FAN_OUT,
                                        &peer_count);

            for (size_t p = 0; p < peer_count; p++) {
                trellis_bridge_dht_send(bridge, i,
                                        &peers[p]->fingerprint,
                                        buf, ann_len);
            }
        }
    }

    // Expire old routes.
    expire_routes(canopy);

    return TRELLIS_OK;
}

trellis_err_t trellis_canopy_on_announce(trellis_canopy_t *canopy,
                                         const uint8_t *data, size_t len,
                                         const trellis_fingerprint_t *sender)
{
    if (!canopy || !data || !sender)
        return TRELLIS_ERR_INVALID_ARG;

    trellis_grove_id_t origin;
    trellis_grove_id_t path[TRELLIS_CANOPY_MAX_PATH_LEN];
    size_t path_len;
    trellis_fingerprint_t bridge_fp;
    uint8_t ed25519_pk[32];
    uint32_t capabilities;
    uint8_t ttl;
    uint64_t timestamp;
    uint8_t sig[64];

    trellis_err_t err = deserialize_announcement(
        data, len, &origin, path, TRELLIS_CANOPY_MAX_PATH_LEN, &path_len,
        &bridge_fp, ed25519_pk, &capabilities, &ttl, &timestamp, sig);
    if (err != TRELLIS_OK)
        return err;

    // Verify Ed25519 signature using the embedded public key.
    size_t sig_off = len - 64;
    if (crypto_sign_ed25519_verify_detached(sig, data, sig_off,
            ed25519_pk) != 0)
        return TRELLIS_ERR_VERIFY_FAILED;

    // Rate limit per bridge.
    if (!check_announce_rate(canopy, &bridge_fp))
        return TRELLIS_ERR_RATE_LIMITED;

    // Policy check: reject routes from groves our policy disallows.
    if (!trellis_bridge_peering_allowed(canopy->bridge, &origin))
        return TRELLIS_ERR_GROVE_POLICY;

    // Check TTL.
    if (ttl == 0)
        return TRELLIS_ERR_CANOPY_EXPIRED;

    // Check timestamp freshness (reject stale or far-future announcements)
    uint64_t now = trellis_now_ms();
    if (now > timestamp && now - timestamp > TRELLIS_CANOPY_ROUTE_EXPIRE_MS)
        return TRELLIS_ERR_CANOPY_EXPIRED;
    if (timestamp > now && timestamp - now > TRELLIS_CANOPY_ANNOUNCE_INTERVAL_MS * 2)
        return TRELLIS_ERR_CANOPY_EXPIRED;

    /* Loop detection: check if any of our groves appear in the path,
     * excluding the last entry which is the grove the announcement is
     * currently being gossiped in (the receiver is necessarily a member). */
    if (path_len > 1) {
        for (size_t i = 0; i < TRELLIS_BRIDGE_MAX_GROVES; i++) {
            const trellis_grove_id_t *our_grove =
                trellis_bridge_grove_at(canopy->bridge, i);
            if (our_grove && path_contains_grove(path, path_len - 1, our_grove))
                return TRELLIS_ERR_CANOPY_LOOP;
        }
    }

    // Update or insert route — score factors: path length + bridge trust.
    double score = 1.0 / (double)(path_len + 1);
    if (canopy->trust) {
        double trust = trellis_bridge_trust_score(canopy->trust, &bridge_fp);
        score *= trust;
    }

    now = trellis_now_ms();
    trellis_canopy_route_t *existing = find_route(canopy, &origin);
    if (existing) {
        /* TOFU identity binding: reject if the bridge_fp re-announces with
         * a different ed25519 key than previously pinned */
        if (trellis_fingerprint_eq(&existing->next_bridge_fp, &bridge_fp) &&
            memcmp(existing->bridge_ed25519_pk, ed25519_pk, 32) != 0) {
            static const uint8_t zero[32] = {0};
            if (memcmp(existing->bridge_ed25519_pk, zero, 32) != 0)
                return TRELLIS_ERR_VERIFY_FAILED;
        }

        // Replace if the new route has a higher composite score.
        if (score > existing->score ||
            (score == existing->score && timestamp > existing->remote_ts)) {
            existing->path_len = path_len;
            memcpy(existing->path, path, path_len * sizeof(trellis_grove_id_t));
            existing->next_bridge_fp = bridge_fp;
            memcpy(existing->bridge_ed25519_pk, ed25519_pk, 32);
            existing->capabilities = capabilities;
            existing->last_seen = now;
            existing->remote_ts = timestamp;
            existing->score = score;
        }
    } else if (canopy->route_count < TRELLIS_CANOPY_MAX_ROUTES) {
        trellis_canopy_route_t *r = &canopy->routes[canopy->route_count++];
        r->target_grove = origin;
        r->path_len = path_len;
        memcpy(r->path, path, path_len * sizeof(trellis_grove_id_t));
        r->next_bridge_fp = bridge_fp;
        memcpy(r->bridge_ed25519_pk, ed25519_pk, 32);
        r->capabilities = capabilities;
        r->last_seen = now;
        r->remote_ts = timestamp;
        r->score = score;
    } else {
        // Table full — evict lowest-scoring route if new route is better.
        size_t worst = 0;
        for (size_t i = 1; i < canopy->route_count; i++) {
            if (canopy->routes[i].score < canopy->routes[worst].score)
                worst = i;
        }
        if (score > canopy->routes[worst].score) {
            trellis_canopy_route_t *r = &canopy->routes[worst];
            r->target_grove = origin;
            r->path_len = path_len;
            memcpy(r->path, path, path_len * sizeof(trellis_grove_id_t));
            r->next_bridge_fp = bridge_fp;
            memcpy(r->bridge_ed25519_pk, ed25519_pk, 32);
            r->capabilities = capabilities;
            r->last_seen = now;
            r->remote_ts = timestamp;
            r->score = score;
        }
    }

    // Forward with decremented TTL if we're a bridge to other groves.
    if (ttl > 1 && canopy->bridge) {
        for (size_t i = 0; i < TRELLIS_BRIDGE_MAX_GROVES; i++) {
            const trellis_grove_id_t *our_grove =
                trellis_bridge_grove_at(canopy->bridge, i);
            if (!our_grove)
                continue;

            // Don't forward back to the grove it came from.
            if (path_len > 0 && trellis_grove_id_eq(our_grove, &path[path_len - 1]))
                continue;

            // Don't forward if our grove is the origin.
            if (trellis_grove_id_eq(our_grove, &origin))
                continue;

            // Respect per-grove gossip relay policy.
            const trellis_grove_policy_t *gp =
                trellis_bridge_grove_policy(canopy->bridge, i);
            if (gp && !gp->gossip_relay_allowed)
                continue;

            // Append our grove to path and re-announce.
            if (path_len >= TRELLIS_CANOPY_MAX_PATH_LEN)
                continue;

            trellis_grove_id_t new_path[TRELLIS_CANOPY_MAX_PATH_LEN];
            memcpy(new_path, path, path_len * sizeof(trellis_grove_id_t));
            new_path[path_len] = *our_grove;

            const trellis_fingerprint_t *our_fp =
                trellis_bridge_fingerprint(canopy->bridge);
            const trellis_identity_t *our_id =
                trellis_bridge_identity(canopy->bridge);
            if (!our_fp || !our_id)
                continue;

            uint8_t fwd_buf[CANOPY_MAX_RAW];
            size_t fwd_len = serialize_announcement(
                &origin, new_path, path_len + 1,
                our_fp,
                our_id->ed25519_pk,
                capabilities, ttl - 1, timestamp,
                fwd_buf, sizeof(fwd_buf));

            if (fwd_len == 0)
                continue;

            // Re-sign with our key.
            size_t fwd_sig_off = fwd_len - 64;
            crypto_sign_ed25519_detached(
                fwd_buf + fwd_sig_off, NULL,
                fwd_buf, fwd_sig_off,
                our_id->ed25519_sk);

            // Send to random peers in this grove.
            trellis_peer_info_t *fwd_peers[TRELLIS_GOSSIP_FAN_OUT];
            size_t fwd_peer_count = 0;
            trellis_bridge_select_peers(canopy->bridge, i,
                                        fwd_peers, TRELLIS_GOSSIP_FAN_OUT,
                                        &fwd_peer_count);
            for (size_t p = 0; p < fwd_peer_count; p++) {
                trellis_bridge_dht_send(canopy->bridge, i,
                                        &fwd_peers[p]->fingerprint,
                                        fwd_buf, fwd_len);
            }
        }
    }

    return TRELLIS_OK;
}

trellis_err_t trellis_canopy_route(const trellis_canopy_t *canopy,
                                   const trellis_grove_id_t *target_grove,
                                   trellis_canopy_route_t *route_out)
{
    if (!canopy || !target_grove || !route_out)
        return TRELLIS_ERR_INVALID_ARG;

    // Check if target is one of our own groves.
    for (size_t i = 0; i < TRELLIS_BRIDGE_MAX_GROVES; i++) {
        const trellis_grove_id_t *g = trellis_bridge_grove_at(canopy->bridge, i);
        if (g && trellis_grove_id_eq(g, target_grove)) {
            memset(route_out, 0, sizeof(*route_out));
            route_out->target_grove = *target_grove;
            route_out->path_len = 0; /* direct */
            route_out->score = 1.0;
            return TRELLIS_OK;
        }
    }

    // Search route table for best route.
    const trellis_canopy_route_t *best = NULL;
    for (size_t i = 0; i < canopy->route_count; i++) {
        if (!trellis_grove_id_eq(&canopy->routes[i].target_grove, target_grove))
            continue;
        if (!best || canopy->routes[i].score > best->score)
            best = &canopy->routes[i];
    }

    if (!best)
        return TRELLIS_ERR_GROVE_UNKNOWN;

    *route_out = *best;
    return TRELLIS_OK;
}

size_t trellis_canopy_route_count(const trellis_canopy_t *canopy)
{
    return canopy ? canopy->route_count : 0;
}

bool trellis_canopy_grove_reachable(const trellis_canopy_t *canopy,
                                    const trellis_grove_id_t *grove_id)
{
    if (!canopy || !grove_id)
        return false;

    // Direct membership?
    for (size_t i = 0; i < TRELLIS_BRIDGE_MAX_GROVES; i++) {
        const trellis_grove_id_t *g = trellis_bridge_grove_at(canopy->bridge, i);
        if (g && trellis_grove_id_eq(g, grove_id))
            return true;
    }

    return find_route((trellis_canopy_t *)canopy, grove_id) != NULL;
}

static uint32_t count_active_circuits(const trellis_canopy_t *canopy)
{
    uint32_t n = 0;
    for (int i = 0; i < CANOPY_MAX_CIRCUITS; i++)
        if (canopy->circuits[i].active)
            n++;
    return n;
}

static bool find_circuit_by_id(const trellis_canopy_t *canopy, uint32_t cid)
{
    for (int i = 0; i < CANOPY_MAX_CIRCUITS; i++)
        if (canopy->circuits[i].active && canopy->circuits[i].circuit_id == cid)
            return true;
    return false;
}

static int find_circuit_index_by_id(trellis_canopy_t *canopy, uint32_t cid)
{
    for (int i = 0; i < CANOPY_MAX_CIRCUITS; i++)
        if (canopy->circuits[i].active && canopy->circuits[i].circuit_id == cid)
            return i;
    return -1;
}

/* --- Cross-grove DHT fetch over Canopy (CDHT) ---
 * Request/response carried inside CANOPY_CIRCUIT_DATA payloads after the
 * circuit_id + length wrapper.  Terminal bridge in γ runs PIR FIND_VALUE on
 * its local (grove-scoped) DHT; cover queries hit γ's DHT as well.
 */
#define CDHT_MAGIC_REQ "CDFN"
#define CDHT_MAGIC_RSP "CDFR"
#define CDHT_REQ_MIN   (4 + 4 + 2) /* magic || request_id || key_len */
#define CDHT_RSP_HDR   (4 + 4 + 1 + 4) /* magic || request_id || status || val_len */

typedef struct {
    trellis_canopy_t *canopy;
    uint32_t          circuit_id;
    uint32_t          request_id;
} cdht_pir_ctx_t;

static void cdht_send_response(trellis_canopy_t *canopy, uint32_t circuit_id,
                                uint32_t request_id, uint8_t status,
                                const uint8_t *value, size_t value_len)
{
    if (!canopy || value_len > CANOPY_MAX_CIRCUIT_PAYLOAD - CDHT_RSP_HDR)
        return;

    size_t total = CDHT_RSP_HDR + value_len;
    uint8_t *buf = malloc(total);
    if (!buf)
        return;

    memcpy(buf, CDHT_MAGIC_RSP, 4);
    trellis_write_u32_be(buf + 4, request_id);
    buf[8] = status;
    trellis_write_u32_be(buf + 9, (uint32_t)value_len);
    if (value_len > 0 && value)
        memcpy(buf + 13, value, value_len);

    trellis_canopy_circuit_send(canopy, circuit_id, buf, total);
    free(buf);
}

static void cdht_pir_done(const uint8_t *value, size_t value_len,
                          trellis_err_t err, void *ctx)
{
    cdht_pir_ctx_t *pc = (cdht_pir_ctx_t *)ctx;
    if (!pc)
        return;

    if (err == TRELLIS_OK && value && value_len > 0)
        cdht_send_response(pc->canopy, pc->circuit_id, pc->request_id,
                           0, value, value_len);
    else
        cdht_send_response(pc->canopy, pc->circuit_id, pc->request_id,
                           1, NULL, 0);

    free(pc);
}

static size_t bridge_slot_for_grove(trellis_bridge_t *bridge,
                                    const trellis_grove_id_t *grove_id)
{
    if (!bridge || !grove_id)
        return SIZE_MAX;
    for (size_t i = 0; i < TRELLIS_BRIDGE_MAX_GROVES; i++) {
        const trellis_grove_id_t *g = trellis_bridge_grove_at(bridge, i);
        if (g && trellis_grove_id_eq(g, grove_id))
            return i;
    }
    return SIZE_MAX;
}

static void canopy_handle_cdht_request(trellis_canopy_t *canopy,
                                       canopy_circuit_t *circ,
                                       const uint8_t *payload, size_t payload_len)
{
    if (!canopy || !canopy->bridge || !circ || !payload ||
        payload_len < CDHT_REQ_MIN)
        return;

    if (memcmp(payload, CDHT_MAGIC_REQ, 4) != 0)
        return;

    uint32_t request_id = trellis_read_u32_be(payload + 4);
    uint16_t key_len = ((uint16_t)payload[8] << 8) | (uint16_t)payload[9];
    if (key_len == 0 || key_len > 64 || CDHT_REQ_MIN + key_len > payload_len)
        return;

    size_t gslot = bridge_slot_for_grove(canopy->bridge, &circ->target_grove);
    if (gslot == SIZE_MAX) {
        cdht_send_response(canopy, circ->circuit_id, request_id, 2, NULL, 0);
        return;
    }

    const trellis_grove_policy_t *pol =
        trellis_bridge_grove_policy(canopy->bridge, gslot);
    if (pol && !pol->descriptors_visible) {
        cdht_send_response(canopy, circ->circuit_id, request_id, 2, NULL, 0);
        return;
    }

    trellis_dht_t *dht = trellis_bridge_grove_dht(canopy->bridge, gslot);
    if (!dht) {
        cdht_send_response(canopy, circ->circuit_id, request_id, 2, NULL, 0);
        return;
    }

    cdht_pir_ctx_t *pctx = malloc(sizeof(*pctx));
    if (!pctx) {
        cdht_send_response(canopy, circ->circuit_id, request_id, 2, NULL, 0);
        return;
    }
    pctx->canopy     = canopy;
    pctx->circuit_id = circ->circuit_id;
    pctx->request_id = request_id;

    const uint8_t *key = payload + CDHT_REQ_MIN;
    trellis_err_t perr = trellis_pir_find_value_skad(
        dht, key, key_len, cdht_pir_done, pctx);
    if (perr != TRELLIS_OK) {
        free(pctx);
        cdht_send_response(canopy, circ->circuit_id, request_id, 2, NULL, 0);
    }
}

static void canopy_deliver_local_payload(trellis_canopy_t *canopy,
                                         canopy_circuit_t *circ,
                                         const uint8_t *payload, size_t payload_len)
{
    if (!canopy || !circ || !payload || payload_len == 0)
        return;

    const trellis_fingerprint_t *self_fp =
        trellis_bridge_fingerprint(canopy->bridge);
    if (!self_fp)
        return;

    // Terminal bridge: run CDHT when payload requests a DHT lookup.
    if (trellis_fingerprint_eq(&circ->next_bridge, self_fp) &&
        payload_len >= 4 && memcmp(payload, CDHT_MAGIC_REQ, 4) == 0) {
        canopy_handle_cdht_request(canopy, circ, payload, payload_len);
        return;
    }

    if (circ->app_recv)
        circ->app_recv(circ->circuit_id, payload, payload_len, circ->app_recv_ctx);
}

trellis_err_t trellis_canopy_circuit_set_app_recv(trellis_canopy_t *canopy,
                                                  uint32_t circuit_id,
                                                  trellis_canopy_app_recv_fn fn,
                                                  void *ctx)
{
    if (!canopy)
        return TRELLIS_ERR_INVALID_ARG;

    int idx = find_circuit_index_by_id(canopy, circuit_id);
    if (idx < 0)
        return TRELLIS_ERR_NOT_FOUND;

    canopy->circuits[idx].app_recv     = fn;
    canopy->circuits[idx].app_recv_ctx = ctx;
    return TRELLIS_OK;
}

trellis_err_t trellis_canopy_circuit_create(trellis_canopy_t *canopy,
                                            const trellis_grove_id_t *target_grove,
                                            const trellis_fingerprint_t *dest,
                                            trellis_canopy_circuit_cb cb,
                                            void *ctx)
{
    if (!canopy || !target_grove || !dest)
        return TRELLIS_ERR_INVALID_ARG;

    // Policy: check peering and circuit limits.
    if (!trellis_bridge_peering_allowed(canopy->bridge, target_grove))
        return TRELLIS_ERR_GROVE_POLICY;

    const trellis_grove_policy_t *policy =
        trellis_bridge_grove_policy(canopy->bridge, 0);
    if (policy && policy->max_cross_grove_circuits > 0 &&
        count_active_circuits(canopy) >= policy->max_cross_grove_circuits)
        return TRELLIS_ERR_LIMIT_EXCEEDED;

    // Find route to target grove.
    trellis_canopy_route_t route;
    trellis_err_t err = trellis_canopy_route(canopy, target_grove, &route);
    if (err != TRELLIS_OK)
        return err;

    // Find free circuit slot.
    int idx = -1;
    for (int i = 0; i < CANOPY_MAX_CIRCUITS; i++) {
        if (!canopy->circuits[i].active) {
            idx = i;
            break;
        }
    }
    if (idx < 0)
        return TRELLIS_ERR_LIMIT_EXCEEDED;

    // Determine which local grove to use for reaching the next bridge.
    size_t gidx = (route.path_len > 0)
        ? resolve_grove_index(canopy, &route.path[0])
        : resolve_grove_index(canopy, target_grove);

    // Allocate circuit with collision-free ID.
    canopy_circuit_t *circ = &canopy->circuits[idx];
    circ->active = true;
    circ->app_recv     = NULL;
    circ->app_recv_ctx = NULL;
    uint32_t cid;
    do {
        cid = canopy->next_circuit_id++;
        if (cid == 0) cid = canopy->next_circuit_id++;
    } while (find_circuit_by_id(canopy, cid));
    circ->circuit_id = cid;
    circ->target_grove = *target_grove;
    circ->dest = *dest;
    circ->grove_index = gidx;
    circ->created_at = trellis_now_ms();

    const trellis_fingerprint_t *origin_fp =
        trellis_bridge_fingerprint(canopy->bridge);
    if (!origin_fp) {
        circ->active = false;
        return TRELLIS_ERR_INVALID_STATE;
    }
    circ->source = *origin_fp;

    if (route.path_len == 0) {
        /* Target grove is local — no cross-grove relay needed.
         * Store the destination as the "next bridge" for local delivery. */
        circ->next_bridge = *dest;
    } else {
        circ->next_bridge = route.next_bridge_fp;

        uint8_t create_msg[4 + 32 + 32];
        create_msg[0] = (uint8_t)(circ->circuit_id >> 24);
        create_msg[1] = (uint8_t)(circ->circuit_id >> 16);
        create_msg[2] = (uint8_t)(circ->circuit_id >> 8);
        create_msg[3] = (uint8_t)(circ->circuit_id);
        memcpy(create_msg + 4, target_grove->bytes, 32);
        memcpy(create_msg + 36, dest->bytes, 32);

        trellis_err_t send_err = trellis_bridge_dht_send(
            canopy->bridge, gidx, &route.next_bridge_fp,
            create_msg, sizeof(create_msg));
        if (send_err != TRELLIS_OK) {
            circ->active = false;
            return send_err;
        }
    }

    if (cb)
        cb(TRELLIS_OK, circ->circuit_id, ctx);

    return TRELLIS_OK;
}

trellis_err_t trellis_canopy_circuit_send(trellis_canopy_t *canopy,
                                          uint32_t circuit_id,
                                          const uint8_t *data, size_t len)
{
    if (!canopy || !data)
        return TRELLIS_ERR_INVALID_ARG;

    canopy_circuit_t *circ = NULL;
    for (int i = 0; i < CANOPY_MAX_CIRCUITS; i++) {
        if (canopy->circuits[i].active &&
            canopy->circuits[i].circuit_id == circuit_id) {
            circ = &canopy->circuits[i];
            break;
        }
    }
    if (!circ)
        return TRELLIS_ERR_NOT_FOUND;

    const trellis_fingerprint_t *self_fp =
        trellis_bridge_fingerprint(canopy->bridge);
    if (!self_fp)
        return TRELLIS_ERR_INVALID_STATE;

    // Origin sends toward next_bridge; terminal sends back toward source.
    const trellis_fingerprint_t *dest_peer;
    if (trellis_fingerprint_eq(&circ->next_bridge, self_fp))
        dest_peer = &circ->source;
    else
        dest_peer = &circ->next_bridge;

    // Wire format: circuit_id(4) || payload_len(4) || payload(len)
    if (len > CANOPY_MAX_CIRCUIT_PAYLOAD)
        return TRELLIS_ERR_INVALID_ARG;
    size_t msg_len = 4 + 4 + len;
    uint8_t *msg = malloc(msg_len);
    if (!msg)
        return TRELLIS_ERR_NOMEM;

    msg[0] = (uint8_t)(circuit_id >> 24);
    msg[1] = (uint8_t)(circuit_id >> 16);
    msg[2] = (uint8_t)(circuit_id >> 8);
    msg[3] = (uint8_t)(circuit_id);
    msg[4] = (uint8_t)(len >> 24);
    msg[5] = (uint8_t)(len >> 16);
    msg[6] = (uint8_t)(len >> 8);
    msg[7] = (uint8_t)(len);
    memcpy(msg + 8, data, len);

    trellis_err_t send_err = trellis_bridge_dht_send(
        canopy->bridge, circ->grove_index, dest_peer, msg, msg_len);
    free(msg);
    return send_err;
}

trellis_err_t trellis_canopy_circuit_destroy(trellis_canopy_t *canopy,
                                             uint32_t circuit_id)
{
    if (!canopy)
        return TRELLIS_ERR_INVALID_ARG;

    for (int i = 0; i < CANOPY_MAX_CIRCUITS; i++) {
        if (canopy->circuits[i].active &&
            canopy->circuits[i].circuit_id == circuit_id) {

            canopy_circuit_t *circ = &canopy->circuits[i];

            // Send destroy message to the remote circuit endpoint.
            uint8_t destroy_msg[4];
            destroy_msg[0] = (uint8_t)(circuit_id >> 24);
            destroy_msg[1] = (uint8_t)(circuit_id >> 16);
            destroy_msg[2] = (uint8_t)(circuit_id >> 8);
            destroy_msg[3] = (uint8_t)(circuit_id);

            const trellis_fingerprint_t *self_fp =
                trellis_bridge_fingerprint(canopy->bridge);
            const trellis_fingerprint_t *dest_peer = &circ->next_bridge;
            if (self_fp && trellis_fingerprint_eq(&circ->next_bridge, self_fp))
                dest_peer = &circ->source;

            trellis_bridge_dht_send(canopy->bridge,
                                    circ->grove_index,
                                    dest_peer,
                                    destroy_msg, sizeof(destroy_msg));

            memset(circ, 0, sizeof(canopy_circuit_t));
            return TRELLIS_OK;
        }
    }

    return TRELLIS_ERR_NOT_FOUND;
}

// --- Incoming Circuit Message Handlers (bridge-side) ---
trellis_err_t trellis_canopy_on_circuit_create(trellis_canopy_t *canopy,
                                               const uint8_t *data, size_t len,
                                               const trellis_fingerprint_t *sender)
{
    if (!canopy || !data || !sender)
        return TRELLIS_ERR_INVALID_ARG;

    // Wire: circuit_id(4) || target_grove(32) || dest_fp(32)
    if (len < 4 + 32 + 32)
        return TRELLIS_ERR_MSG_FORMAT;

    uint32_t circuit_id = ((uint32_t)data[0] << 24) | ((uint32_t)data[1] << 16) |
                          ((uint32_t)data[2] << 8) | (uint32_t)data[3];

    trellis_grove_id_t target_grove;
    trellis_fingerprint_t dest_fp;
    memcpy(target_grove.bytes, data + 4, 32);
    memcpy(dest_fp.bytes, data + 36, 32);

    // Policy: check peering and circuit limits.
    if (!trellis_bridge_peering_allowed(canopy->bridge, &target_grove))
        return TRELLIS_ERR_GROVE_POLICY;

    // Check if target grove is one we participate in (terminal hop)
    bool is_terminal = false;
    size_t target_slot = 0;
    for (size_t i = 0; i < TRELLIS_BRIDGE_MAX_GROVES; i++) {
        const trellis_grove_id_t *g = trellis_bridge_grove_at(canopy->bridge, i);
        if (g && trellis_grove_id_eq(g, &target_grove)) {
            is_terminal = true;
            target_slot = i;
            break;
        }
    }

    /* Policy: check circuit limits.
     * For terminal hops, use the target grove's policy. For relay hops,
     * use the home grove's policy (slot 0) since we don't own the target. */
    size_t policy_slot = is_terminal ? target_slot : 0;
    const trellis_grove_policy_t *policy =
        trellis_bridge_grove_policy(canopy->bridge, policy_slot);
    if (policy && policy->max_cross_grove_circuits > 0 &&
        count_active_circuits(canopy) >= policy->max_cross_grove_circuits)
        return TRELLIS_ERR_LIMIT_EXCEEDED;

    // Reject duplicate circuit IDs to prevent hijacking.
    for (int i = 0; i < CANOPY_MAX_CIRCUITS; i++) {
        if (canopy->circuits[i].active &&
            canopy->circuits[i].circuit_id == circuit_id)
            return TRELLIS_ERR_INVALID_ARG;
    }

    // Find free circuit slot.
    int idx = -1;
    for (int i = 0; i < CANOPY_MAX_CIRCUITS; i++) {
        if (!canopy->circuits[i].active) {
            idx = i;
            break;
        }
    }
    if (idx < 0)
        return TRELLIS_ERR_LIMIT_EXCEEDED;

    canopy_circuit_t *circ = &canopy->circuits[idx];
    circ->active = true;
    circ->circuit_id = circuit_id;
    circ->target_grove = target_grove;
    circ->dest = dest_fp;
    circ->source = *sender;
    circ->created_at = trellis_now_ms();
    circ->app_recv     = NULL;
    circ->app_recv_ctx = NULL;

    if (is_terminal) {
        circ->next_bridge = dest_fp;
        circ->grove_index = target_slot;
    } else {
        trellis_canopy_route_t route;
        trellis_err_t err = trellis_canopy_route(canopy, &target_grove, &route);
        if (err != TRELLIS_OK) {
            memset(circ, 0, sizeof(*circ));
            return err;
        }
        circ->next_bridge = route.next_bridge_fp;
        circ->grove_index = (route.path_len > 0)
            ? resolve_grove_index(canopy, &route.path[0])
            : target_slot;

        trellis_err_t send_err = trellis_bridge_dht_send(
            canopy->bridge, circ->grove_index,
            &route.next_bridge_fp, data, len);
        if (send_err != TRELLIS_OK) {
            memset(circ, 0, sizeof(*circ));
            return send_err;
        }
    }

    return TRELLIS_OK;
}

trellis_err_t trellis_canopy_on_circuit_data(trellis_canopy_t *canopy,
                                             const uint8_t *data, size_t len,
                                             const trellis_fingerprint_t *sender)
{
    if (!canopy || !data || !sender)
        return TRELLIS_ERR_INVALID_ARG;

    // Wire: circuit_id(4) || payload_len(4) || payload(N)
    if (len < 8)
        return TRELLIS_ERR_MSG_FORMAT;

    uint32_t circuit_id = ((uint32_t)data[0] << 24) | ((uint32_t)data[1] << 16) |
                          ((uint32_t)data[2] << 8) | (uint32_t)data[3];

    /* Validate embedded payload_len matches actual data.
     * Compare without addition to avoid 32-bit size_t wrap. */
    uint32_t payload_len = ((uint32_t)data[4] << 24) | ((uint32_t)data[5] << 16) |
                           ((uint32_t)data[6] << 8) | (uint32_t)data[7];
    if (payload_len > CANOPY_MAX_CIRCUIT_PAYLOAD || payload_len > len - 8)
        return TRELLIS_ERR_MSG_FORMAT;

    canopy_circuit_t *circ = NULL;
    for (int i = 0; i < CANOPY_MAX_CIRCUITS; i++) {
        if (canopy->circuits[i].active &&
            canopy->circuits[i].circuit_id == circuit_id) {
            circ = &canopy->circuits[i];
            break;
        }
    }
    if (!circ)
        return TRELLIS_ERR_NOT_FOUND;

    const trellis_fingerprint_t *self_fp =
        trellis_bridge_fingerprint(canopy->bridge);
    if (!self_fp)
        return TRELLIS_ERR_INVALID_STATE;

    const trellis_fingerprint_t *fwd_peer;
    if (trellis_fingerprint_eq(sender, &circ->source))
        fwd_peer = &circ->next_bridge;
    else if (trellis_fingerprint_eq(sender, &circ->next_bridge))
        fwd_peer = &circ->source;
    else
        return TRELLIS_ERR_VERIFY_FAILED;

    if (trellis_fingerprint_eq(fwd_peer, self_fp)) {
        canopy_deliver_local_payload(canopy, circ, data + 8, payload_len);
        return TRELLIS_OK;
    }

    return trellis_bridge_dht_send(canopy->bridge, circ->grove_index,
                                   fwd_peer, data, 8 + payload_len);
}

trellis_err_t trellis_canopy_on_circuit_destroy(trellis_canopy_t *canopy,
                                                const uint8_t *data, size_t len,
                                                const trellis_fingerprint_t *sender)
{
    if (!canopy || !data || !sender)
        return TRELLIS_ERR_INVALID_ARG;

    // Wire: circuit_id(4)
    if (len < 4)
        return TRELLIS_ERR_MSG_FORMAT;

    uint32_t circuit_id = ((uint32_t)data[0] << 24) | ((uint32_t)data[1] << 16) |
                          ((uint32_t)data[2] << 8) | (uint32_t)data[3];

    for (int i = 0; i < CANOPY_MAX_CIRCUITS; i++) {
        if (canopy->circuits[i].active &&
            canopy->circuits[i].circuit_id == circuit_id) {

            canopy_circuit_t *circ = &canopy->circuits[i];

            const trellis_fingerprint_t *self_fp =
                trellis_bridge_fingerprint(canopy->bridge);
            if (!self_fp)
                return TRELLIS_ERR_INVALID_STATE;

            const trellis_fingerprint_t *fwd_peer;
            if (trellis_fingerprint_eq(sender, &circ->source))
                fwd_peer = &circ->next_bridge;
            else if (trellis_fingerprint_eq(sender, &circ->next_bridge))
                fwd_peer = &circ->source;
            else
                return TRELLIS_ERR_VERIFY_FAILED;

            if (!trellis_fingerprint_eq(fwd_peer, self_fp)) {
                trellis_bridge_dht_send(canopy->bridge, circ->grove_index,
                                        fwd_peer, data, len);
            }

            memset(circ, 0, sizeof(canopy_circuit_t));
            return TRELLIS_OK;
        }
    }

    return TRELLIS_ERR_NOT_FOUND;
}

// --- Canopy Withdrawal Handling ---
#define CANOPY_WITHDRAW_MAGIC "CANW"
#define CANOPY_WITHDRAW_LEN   (4 + 32 + 32 + 32 + 8 + 64)

trellis_err_t trellis_canopy_on_withdraw(trellis_canopy_t *canopy,
                                          const uint8_t *data, size_t len,
                                          const trellis_fingerprint_t *sender)
{
    if (!canopy || !data || !sender)
        return TRELLIS_ERR_INVALID_ARG;

    if (len < CANOPY_WITHDRAW_LEN)
        return TRELLIS_ERR_MSG_FORMAT;

    if (memcmp(data, CANOPY_WITHDRAW_MAGIC, 4) != 0)
        return TRELLIS_ERR_MSG_FORMAT;

    trellis_grove_id_t grove_id;
    trellis_fingerprint_t bridge_fp;
    memcpy(grove_id.bytes, data + 4, 32);
    memcpy(bridge_fp.bytes, data + 36, 32);

    const uint8_t *ed25519_pk = data + 68;

    uint64_t timestamp = 0;
    for (int i = 0; i < 8; i++)
        timestamp |= (uint64_t)data[100 + i] << ((7 - i) * 8);

    // Verify Ed25519 signature over everything before the sig.
    const uint8_t *sig = data + 4 + 32 + 32 + 32 + 8;
    if (crypto_sign_ed25519_verify_detached(sig, data, 4 + 32 + 32 + 32 + 8,
            ed25519_pk) != 0)
        return TRELLIS_ERR_VERIFY_FAILED;

    // Reject stale withdrawals to prevent replay attacks.
    uint64_t now = trellis_now_ms();
    if (now > timestamp && now - timestamp > TRELLIS_CANOPY_ROUTE_EXPIRE_MS)
        return TRELLIS_ERR_CANOPY_EXPIRED;

    // Rate limit per bridge.
    if (!check_announce_rate(canopy, &bridge_fp))
        return TRELLIS_ERR_RATE_LIMITED;

    /* Remove matching routes from this bridge.
     * TOFU: only withdraw when the embedded PK matches the pinned key
     * to prevent forged withdrawals from unrelated keypairs. */
    size_t old_count = canopy->route_count;
    size_t write = 0;
    for (size_t read = 0; read < canopy->route_count; read++) {
        bool match_grove = trellis_grove_id_eq(
            &canopy->routes[read].target_grove, &grove_id);
        bool match_bridge = trellis_fingerprint_eq(
            &canopy->routes[read].next_bridge_fp, &bridge_fp);
        static const uint8_t zero_pk[32] = {0};
        bool pk_ok = memcmp(canopy->routes[read].bridge_ed25519_pk,
                            zero_pk, 32) == 0 ||
                     memcmp(canopy->routes[read].bridge_ed25519_pk,
                            ed25519_pk, 32) == 0;
        bool remove = match_grove && match_bridge && pk_ok;
        if (!remove) {
            if (write != read)
                canopy->routes[write] = canopy->routes[read];
            write++;
        }
    }
    canopy->route_count = write;

    /* Forward withdrawal to other groves.  Two guards prevent amplification
     * loops: (1) skip the grove the withdrawal originated from, and
     * (2) only forward if we actually removed routes — if nothing was
     * removed, we already processed this withdrawal and shouldn't relay. */
    if (canopy->bridge && write < old_count) {
        for (size_t i = 0; i < TRELLIS_BRIDGE_MAX_GROVES; i++) {
            const trellis_grove_id_t *our_grove =
                trellis_bridge_grove_at(canopy->bridge, i);
            if (!our_grove)
                continue;
            if (trellis_grove_id_eq(our_grove, &grove_id))
                continue;

            trellis_peer_info_t *fwd_peers[TRELLIS_GOSSIP_FAN_OUT];
            size_t fwd_peer_count = 0;
            trellis_bridge_select_peers(canopy->bridge, i,
                                        fwd_peers, TRELLIS_GOSSIP_FAN_OUT,
                                        &fwd_peer_count);
            for (size_t p = 0; p < fwd_peer_count; p++) {
                trellis_bridge_dht_send(canopy->bridge, i,
                                        &fwd_peers[p]->fingerprint,
                                        data, len);
            }
        }
    }

    return TRELLIS_OK;
}

// --- Grove ID short resolution via canopy table ---
trellis_err_t trellis_grove_id_from_short(const char *short_id,
                                          const struct trellis_canopy *canopy,
                                          trellis_grove_id_t *out)
{
    if (!short_id || !canopy || !out)
        return TRELLIS_ERR_INVALID_ARG;

    size_t slen = strlen(short_id);
    if (slen < 2 || slen > 16)
        return TRELLIS_ERR_INVALID_ARG;

    // Check all known routes for a matching short id.
    for (size_t i = 0; i < canopy->route_count; i++) {
        char candidate[17];
        trellis_err_t err = trellis_grove_id_short(
            &canopy->routes[i].target_grove, candidate, sizeof(candidate));
        if (err != TRELLIS_OK)
            continue;

        if (strncmp(candidate, short_id, slen) == 0) {
            *out = canopy->routes[i].target_grove;
            return TRELLIS_OK;
        }
    }

    // Also check our own groves.
    for (size_t i = 0; i < TRELLIS_BRIDGE_MAX_GROVES; i++) {
        const trellis_grove_id_t *g = trellis_bridge_grove_at(canopy->bridge, i);
        if (!g)
            continue;

        char candidate[17];
        trellis_err_t err = trellis_grove_id_short(g, candidate, sizeof(candidate));
        if (err != TRELLIS_OK)
            continue;

        if (strncmp(candidate, short_id, slen) == 0) {
            *out = *g;
            return TRELLIS_OK;
        }
    }

    return TRELLIS_ERR_NOT_FOUND;
}
