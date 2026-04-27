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

// Pre-built circuit pool for latency hiding.
/* Maintains a pool of pre-built onion circuits so the first request to
 * any destination doesn't pay the full 4-hop KEM latency. Circuits are
 * reused for multiple streams (stream multiplexing within a circuit).
 *
 * Pool lifecycle:
 *   - On router start, begin building CIRCUIT_POOL_SIZE circuits
 *   - When a circuit is consumed, start building a replacement
 *   - Circuits older than CIRCUIT_POOL_MAX_AGE_MS are retired
 *   - Each circuit supports up to CIRCUIT_MAX_STREAMS concurrent streams
 */

#include "internal.h"
#include "../sdk/internal.h"
#include <stdio.h>

void trellis_circuit_pool_init(trellis_router_t *router)
{
    if (!router) return;
    memset(router->circuit_pool, 0, sizeof(router->circuit_pool));
}

static prebuilt_circuit_t *pool_find_free(trellis_router_t *router)
{
    for (size_t i = 0; i < CIRCUIT_POOL_SIZE; i++) {
        if (!router->circuit_pool[i].valid)
            return &router->circuit_pool[i];
    }
    return NULL;
}

prebuilt_circuit_t *trellis_circuit_pool_acquire(trellis_router_t *router,
                                                  const trellis_fingerprint_t *dest)
{
    if (!router) return NULL;
    uint64_t now = trellis_now_ms();

    // Prefer a circuit already built for this destination.
    if (dest) {
        for (size_t i = 0; i < CIRCUIT_POOL_SIZE; i++) {
            prebuilt_circuit_t *c = &router->circuit_pool[i];
            if (!c->valid) continue;
            if (now - c->created_at > CIRCUIT_POOL_MAX_AGE_MS) {
                c->valid = false;
                continue;
            }
            if (trellis_fingerprint_eq(&c->dest, dest) &&
                c->stream_count < CIRCUIT_MAX_STREAMS)
                return c;
        }
    }

    // Take any general-purpose circuit with capacity.
    static const trellis_fingerprint_t zero_fp = {0};
    for (size_t i = 0; i < CIRCUIT_POOL_SIZE; i++) {
        prebuilt_circuit_t *c = &router->circuit_pool[i];
        if (!c->valid) continue;
        if (now - c->created_at > CIRCUIT_POOL_MAX_AGE_MS) {
            c->valid = false;
            continue;
        }
        if (trellis_fingerprint_eq(&c->dest, &zero_fp) &&
            c->stream_count < CIRCUIT_MAX_STREAMS) {
            if (dest) c->dest = *dest;
            return c;
        }
    }

    return NULL;
}

uint32_t trellis_circuit_pool_alloc_stream_with_id(prebuilt_circuit_t *circuit,
                                                    uint32_t forced_id);

uint32_t trellis_circuit_pool_alloc_stream(prebuilt_circuit_t *circuit)
{
    return trellis_circuit_pool_alloc_stream_with_id(circuit, 0);
}

uint32_t trellis_circuit_pool_alloc_stream_with_id(prebuilt_circuit_t *circuit,
                                                    uint32_t forced_id)
{
    if (!circuit) return 0;

    /* When reusing a stream_id, check if it's already active on this circuit
     * (legitimate for subsequent sends on the same exit stream). */
    if (forced_id > 0) {
        for (size_t i = 0; i < CIRCUIT_MAX_STREAMS; i++) {
            if (circuit->streams[i].active &&
                circuit->streams[i].stream_id == forced_id)
                return forced_id;
        }
    }

    for (size_t i = 0; i < CIRCUIT_MAX_STREAMS; i++) {
        if (!circuit->streams[i].active) {
            uint32_t sid;
            if (forced_id > 0) {
                sid = forced_id;
            } else {
                randombytes_buf(&sid, sizeof(sid));
                if (sid == 0) sid = 1;
            }
            circuit->streams[i].active = true;
            circuit->streams[i].half_closed = false;
            circuit->streams[i].stream_id = sid;
            circuit->stream_count++;
            return sid;
        }
    }
    return 0;
}

void trellis_circuit_pool_close_stream(prebuilt_circuit_t *circuit, uint32_t stream_id)
{
    if (!circuit || stream_id == 0) return;
    for (size_t i = 0; i < CIRCUIT_MAX_STREAMS; i++) {
        if (circuit->streams[i].active && circuit->streams[i].stream_id == stream_id) {
            circuit->streams[i].active = false;
            if (circuit->stream_count > 0)
                circuit->stream_count--;
            return;
        }
    }
}

void trellis_circuit_pool_release(trellis_router_t *router,
                                   prebuilt_circuit_t *circuit)
{
    if (!circuit) return;
    if (circuit->stream_count == 0) {
        sodium_memzero(circuit->hop_encaps, sizeof(circuit->hop_encaps));
        circuit->encaps_valid = false;
        circuit->valid = false;
    }

    (void)router;
}

void trellis_circuit_pool_expire(trellis_router_t *router)
{
    if (!router) return;
    uint64_t now = trellis_now_ms();
    for (size_t i = 0; i < CIRCUIT_POOL_SIZE; i++) {
        prebuilt_circuit_t *c = &router->circuit_pool[i];
        if (c->valid && (now - c->created_at > CIRCUIT_POOL_MAX_AGE_MS)) {
            sodium_memzero(c->hop_encaps, sizeof(c->hop_encaps));
            c->encaps_valid = false;
            c->valid = false;
            c->stream_count = 0;
        }
    }
}

static void pool_build_timer_cb(uv_timer_t *handle)
{
    trellis_router_t *router = (trellis_router_t *)handle->data;
    if (!router) return;

    trellis_circuit_pool_expire(router);
    trellis_session_pool_expire(router);

    // Count how many valid circuits we have.
    size_t valid = 0;
    for (size_t i = 0; i < CIRCUIT_POOL_SIZE; i++) {
        if (router->circuit_pool[i].valid) valid++;
    }

    if (valid >= CIRCUIT_POOL_SIZE) return;

    /* Build new circuits to fill the pool.
     * We register a prebuilt circuit with the hop fingerprints so that
     * when route_onion runs, it can check the pool first. */
    prebuilt_circuit_t *slot = pool_find_free(router);
    if (!slot) return;

    memset(slot, 0, sizeof(*slot));
    slot->created_at = trellis_now_ms();

    // Select 3 hops using the same logic as route_onion.
    trellis_peer_info_t *candidates[TRELLIS_DHT_K];
    size_t actual = 0;
    select_random_peers(&router->dht->rt, router->dht->sybil,
                        candidates, TRELLIS_DHT_K, &actual);

    static const uint8_t zero_x25519[TRELLIS_X25519_PK_LEN] = {0};
    static const uint8_t zero_mlkem[TRELLIS_ML_KEM_1024_PK_LEN] = {0};
    trellis_peer_info_t *relays[TRELLIS_DHT_K];
    size_t relay_count = 0;
    for (size_t i = 0; i < actual && relay_count < TRELLIS_DHT_K; i++) {
        if (memcmp(candidates[i]->identity.x25519_pk, zero_x25519,
                   TRELLIS_X25519_PK_LEN) == 0)
            continue;
        if (memcmp(candidates[i]->identity.ml_kem_pk, zero_mlkem,
                   TRELLIS_ML_KEM_1024_PK_LEN) == 0)
            continue;

        // Enrich bandwidth from descriptor cache for weighted selection.
        trellis_relay_descriptor_t *desc =
            trellis_relay_descriptor_cached(&candidates[i]->fingerprint);
        if (desc) {
            if (desc->bandwidth_kbps > 0)
                candidates[i]->bandwidth = desc->bandwidth_kbps;
        } else {
            trellis_relay_descriptor_fetch(router->dht,
                &candidates[i]->fingerprint, NULL, NULL, NULL);
        }
        relays[relay_count++] = candidates[i];
    }

    if (relay_count < 3) return;

    trellis_peer_info_t *hops[3] = {relays[0], relays[1], relays[2]};
    if (router->dht->sybil) {
        static const trellis_fingerprint_t zero_fp = {0};
        size_t got = trellis_sybil_select_hops(
            router->dht->sybil, relays, relay_count,
            &zero_fp, hops, 3);
        if (got < 3) return;
    }

    // Guard pinning for pre-built circuits (swap to avoid duplicate hops)
    if (router->guard) {
        for (int gi = 0; gi < 3; gi++) {
            const trellis_fingerprint_t *gfp = trellis_guard_get(router->guard, gi);
            if (!gfp) continue;
            for (size_t ri = 0; ri < relay_count; ri++) {
                if (trellis_fingerprint_eq(&relays[ri]->fingerprint, gfp)) {
                    trellis_peer_info_t *tmp = relays[0];
                    relays[0] = relays[ri];
                    relays[ri] = tmp;
                    hops[0] = relays[0];
                    trellis_guard_mark_used(router->guard, gfp);
                    goto pool_guard_ok;
                }
            }
        }
    }
pool_guard_ok:;

    // L2 vanguard: pin hops[1]
    if (router->guard) {
        for (int li = 0; li < 2; li++) {
            const trellis_fingerprint_t *l2fp =
                trellis_guard_get_l2(router->guard, li);
            if (!l2fp) continue;
            for (size_t ri = 1; ri < relay_count; ri++) {
                if (trellis_fingerprint_eq(&relays[ri]->fingerprint, l2fp) &&
                    !trellis_fingerprint_eq(&relays[ri]->fingerprint,
                                            &hops[0]->fingerprint)) {
                    hops[1] = relays[ri];
                    goto pool_l2_ok;
                }
            }
        }
    }
pool_l2_ok:;

    // L3 vanguard: pin hops[2]
    if (router->guard) {
        for (int li = 0; li < 2; li++) {
            const trellis_fingerprint_t *l3fp =
                trellis_guard_get_l3(router->guard, li);
            if (!l3fp) continue;
            for (size_t ri = 1; ri < relay_count; ri++) {
                if (trellis_fingerprint_eq(&relays[ri]->fingerprint, l3fp) &&
                    !trellis_fingerprint_eq(&relays[ri]->fingerprint,
                                            &hops[0]->fingerprint) &&
                    !trellis_fingerprint_eq(&relays[ri]->fingerprint,
                                            &hops[1]->fingerprint)) {
                    hops[2] = relays[ri];
                    goto pool_l3_ok;
                }
            }
        }
    }
pool_l3_ok:;

    slot->hops[0] = hops[0]->fingerprint;
    slot->hops[1] = hops[1]->fingerprint;
    slot->hops[2] = hops[2]->fingerprint;
    memset(&slot->dest, 0, sizeof(slot->dest));
    randombytes_buf(&slot->circuit_id, sizeof(slot->circuit_id));
    if (slot->circuit_id == 0) slot->circuit_id = 1;
    slot->hop_depth = 3;

    // Pre-compute KEM encapsulation for each relay hop.
    slot->encaps_valid = true;
    bool kem_ok = true;
    for (int hi = 0; hi < 3; hi++) {
        memcpy(slot->hop_x25519_snapshot[hi],
               hops[hi]->identity.x25519_pk, TRELLIS_X25519_PK_LEN);
        trellis_err_t kem_err = trellis_kem_encaps(
            hops[hi]->identity.x25519_pk,
            hops[hi]->identity.ml_kem_pk,
            &slot->hop_encaps[hi]);
        if (kem_err != TRELLIS_OK) {
            slot->encaps_valid = false;
            sodium_memzero(slot->hop_encaps, sizeof(slot->hop_encaps));
            kem_ok = false;
            break;
        }
    }

    if (!kem_ok) {
        fprintf(stderr, "[circuit-pool] KEM encaps failed, discarding slot\n");
        return;
    }

    slot->valid = true;

    fprintf(stderr, "[circuit-pool] pre-built circuit %08x: "
            "%02x%02x→%02x%02x→%02x%02x\n",
            slot->circuit_id,
            slot->hops[0].bytes[0], slot->hops[0].bytes[1],
            slot->hops[1].bytes[0], slot->hops[1].bytes[1],
            slot->hops[2].bytes[0], slot->hops[2].bytes[1]);
}

trellis_err_t trellis_circuit_pool_start(trellis_router_t *router, uv_loop_t *loop)
{
    if (!router || !loop) return TRELLIS_ERR_INVALID_ARG;

    trellis_circuit_pool_init(router);
    trellis_session_pool_init(router);

    uv_timer_init(loop, &router->pool_timer);
    router->pool_timer.data = router;
    uv_timer_start(&router->pool_timer, pool_build_timer_cb,
                   5000, 30000); /* first build after 5s, then every 30s */
    router->pool_timer_active = true;

    return TRELLIS_OK;
}

void trellis_circuit_pool_stop(trellis_router_t *router)
{
    if (!router || !router->pool_timer_active) return;
    uv_timer_stop(&router->pool_timer);
    router->pool_timer_active = false;
}

/* ══════════════════════════════════════════════════════════════════════════
 *  Session circuit pool (1-hop session-keyed circuits for media relay)
 *
 *  Each session circuit performs a single KEM exchange at setup time and
 *  then uses AES-256-GCM with a session key for all subsequent packets
 *  (28 bytes overhead per packet vs ~1660 for per-packet KEM).
 * ══════════════════════════════════════════════════════════════════════════ */
void trellis_session_pool_init(trellis_router_t *router)
{
    if (!router) return;
    memset(router->session_pool, 0, sizeof(router->session_pool));
}

session_circuit_t *trellis_session_pool_acquire(trellis_router_t *router,
                                                const trellis_fingerprint_t *dest)
{
    if (!router) return NULL;
    uint64_t now = trellis_now_ms();

    // Find a valid session circuit to any relay with capacity.
    for (size_t i = 0; i < SESSION_POOL_SIZE; i++) {
        session_circuit_t *sc = &router->session_pool[i];
        if (!sc->valid) continue;
        if (now - sc->created_at > CIRCUIT_POOL_MAX_AGE_MS) {
            sodium_memzero(sc->session_key, SESSION_KEY_LEN);
            sc->valid = false;
            continue;
        }
        return sc;
    }

    /* No pre-built session circuits available; build one on demand.
     * Select a relay-eligible peer for 1-hop relay. */
    trellis_client_t *client = (trellis_client_t *)router->client;
    if (!client) return NULL;

    static const uint8_t zero_x25519[TRELLIS_X25519_PK_LEN] = {0};
    static const uint8_t zero_mlkem[TRELLIS_ML_KEM_1024_PK_LEN] = {0};

    trellis_peer_conn_t *relay_pc = NULL;
    for (size_t i = 0; i < client->peer_conn_count; i++) {
        trellis_peer_conn_t *pc = &client->peers[i];
        if (!pc->conn || !pc->handshake_complete) continue;
        if (dest && trellis_fingerprint_eq(&pc->remote_fp, dest)) continue;
        if (pc->caps_received && pc->relay_eligible) {
            relay_pc = pc;
            break;
        }
    }
    if (!relay_pc) return NULL;

    // Find a free slot.
    session_circuit_t *slot = NULL;
    for (size_t i = 0; i < SESSION_POOL_SIZE; i++) {
        if (!router->session_pool[i].valid) {
            slot = &router->session_pool[i];
            break;
        }
    }
    if (!slot) return NULL;

    // Perform KEM encapsulation to establish session key.
    const trellis_peer_info_t *relay_info =
        trellis_dht_lookup_peer(router->dht, &relay_pc->remote_fp);
    if (!relay_info) return NULL;
    if (memcmp(relay_info->identity.x25519_pk, zero_x25519,
               TRELLIS_X25519_PK_LEN) == 0) return NULL;
    if (memcmp(relay_info->identity.ml_kem_pk, zero_mlkem,
               TRELLIS_ML_KEM_1024_PK_LEN) == 0) return NULL;

    trellis_kem_encaps_result_t encaps;
    trellis_err_t err = trellis_kem_encaps(
        relay_info->identity.x25519_pk,
        relay_info->identity.ml_kem_pk,
        &encaps);
    if (err != TRELLIS_OK) return NULL;

    // Derive session key from KEM shared secret.
    memset(slot, 0, sizeof(*slot));
    slot->valid = true;
    slot->relay = relay_pc->remote_fp;
    slot->created_at = now;
    slot->nonce_counter = 0;
    slot->encaps = encaps;
    slot->encaps_valid = true;

    trellis_hkdf_shake256(
        encaps.shared_secret, sizeof(encaps.shared_secret),
        (const uint8_t *)"vine-session", 12,
        (const uint8_t *)"session-key-v1", 14,
        slot->session_key, SESSION_KEY_LEN);

    sodium_memzero(&encaps, sizeof(encaps));
    randombytes_buf(&slot->circuit_id, sizeof(slot->circuit_id));
    if (slot->circuit_id == 0) slot->circuit_id = 1;

    fprintf(stderr, "[session-pool] built session circuit %08x via relay %02x%02x…\n",
            slot->circuit_id,
            slot->relay.bytes[0], slot->relay.bytes[1]);

    (void)dest;
    return slot;
}

void trellis_session_pool_expire(trellis_router_t *router)
{
    if (!router) return;
    uint64_t now = trellis_now_ms();
    for (size_t i = 0; i < SESSION_POOL_SIZE; i++) {
        session_circuit_t *sc = &router->session_pool[i];
        if (sc->valid && (now - sc->created_at > CIRCUIT_POOL_MAX_AGE_MS)) {
            sodium_memzero(sc->session_key, SESSION_KEY_LEN);
            sodium_memzero(&sc->encaps, sizeof(sc->encaps));
            sc->valid = false;
        }
    }
}
