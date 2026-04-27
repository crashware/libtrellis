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
#include "../sdk/internal.h"
#include <stdio.h>

void trellis_circuit_pool_stop(trellis_router_t *router);

trellis_router_t *trellis_router_new(trellis_dht_t *dht, void *client)
{
    if (!dht)
        return NULL;

    trellis_router_t *router = calloc(1, sizeof(trellis_router_t));
    if (!router)
        return NULL;

    router->dht = dht;
    router->client = client;
    router->mode = TRELLIS_ROUTE_DIRECT;
    return router;
}

void trellis_router_free(trellis_router_t *router)
{
    if (!router) return;
    trellis_circuit_pool_stop(router);
    free(router);
}

trellis_routing_mode_t trellis_router_mode(const trellis_router_t *router)
{
    return router ? router->mode : TRELLIS_ROUTE_DIRECT;
}

trellis_err_t trellis_router_set_mode(trellis_router_t *router,
                                       trellis_routing_mode_t mode)
{
    if (!router)
        return TRELLIS_ERR_INVALID_ARG;
    if (mode < TRELLIS_ROUTE_DIRECT || mode > TRELLIS_ROUTE_ADAPTIVE)
        return TRELLIS_ERR_INVALID_ARG;

    router->mode = mode;
    return TRELLIS_OK;
}

void trellis_router_set_guard(trellis_router_t *router, trellis_guard_t *guard)
{
    if (router)
        router->guard = guard;
}

trellis_err_t trellis_route_exclude_jurisdictions(trellis_router_t *router,
                                                   const char **codes)
{
    if (!router)
        return TRELLIS_ERR_INVALID_ARG;

    router->excluded_jur_count = 0;

    if (!codes)
        return TRELLIS_OK;

    for (size_t i = 0; i < ROUTER_MAX_EXCLUDED_JURISDICTIONS && codes[i]; i++) {
        size_t clen = strlen(codes[i]);
        if (clen > 3) clen = 3; /* trim to alpha-2 (2 chars) or alpha-3 (3 chars) */
        memcpy(router->excluded_jurs[router->excluded_jur_count], codes[i], clen);
        router->excluded_jurs[router->excluded_jur_count][clen] = '\0';
        router->excluded_jur_count++;
    }

    return TRELLIS_OK;
}

// Returns true if the peer's jurisdiction is in the exclusion list.
static bool peer_jurisdiction_excluded(const trellis_router_t *router,
                                        const trellis_peer_info_t *peer)
{
    if (router->excluded_jur_count == 0)
        return false;
    if (!peer->jurisdiction[0])
        return false; /* unknown jurisdiction: allow through */
    for (size_t i = 0; i < router->excluded_jur_count; i++) {
        if (strcasecmp(peer->jurisdiction, router->excluded_jurs[i]) == 0)
            return true;
    }
    return false;
}

static trellis_peer_info_t *find_peer_in_dht(trellis_router_t *router,
                                             const trellis_fingerprint_t *dest)
{
    for (int b = 0; b < TRELLIS_DHT_BUCKETS; b++) {
        for (size_t p = 0; p < router->dht->rt.buckets[b].count; p++) {
            if (trellis_fingerprint_eq(
                    &router->dht->rt.buckets[b].peers[p]->fingerprint, dest))
                return router->dht->rt.buckets[b].peers[p];
        }
    }
    return NULL;
}

// --- Direct send ---
static trellis_err_t route_direct(trellis_router_t *router,
                                  const trellis_fingerprint_t *dest,
                                  const uint8_t *data, size_t len,
                                  trellis_conn_send_cb cb, void *ctx)
{
    trellis_client_t *client = (trellis_client_t *)router->client;
    if (!client) {
        if (cb) cb(TRELLIS_ERR_NOT_INITIALIZED, ctx);
        return TRELLIS_ERR_NOT_INITIALIZED;
    }

    trellis_peer_conn_t *pc = trellis_client_find_peer(client, dest);
    if (pc && pc->conn) {
        trellis_err_t err = trellis_conn_send(pc->conn, data, len, cb, ctx);
        return err;
    }

    trellis_peer_info_t *peer = find_peer_in_dht(router, dest);
    if (!peer) {
        if (cb) cb(TRELLIS_ERR_NOT_FOUND, ctx);
        return TRELLIS_ERR_NOT_FOUND;
    }

    /*
     * Peer exists in DHT but we have no active connection.
     * Attempt to connect using the stored address.
     */
    if (peer->addr[0] && client->transport) {
        trellis_transport_connect(client->transport, peer->addr, NULL, client);
        if (cb) cb(TRELLIS_ERR_NOT_FOUND, ctx);
        return TRELLIS_ERR_NOT_FOUND;
    }

    if (cb) cb(TRELLIS_ERR_NOT_FOUND, ctx);
    return TRELLIS_ERR_NOT_FOUND;
}

// --- Relay send ---
static trellis_err_t route_relay(trellis_router_t *router,
                                 const trellis_fingerprint_t *dest,
                                 const uint8_t *data, size_t len,
                                 trellis_conn_send_cb cb, void *ctx)
{
    trellis_client_t *client = (trellis_client_t *)router->client;
    if (!client || !client->relay) {
        if (cb) cb(TRELLIS_ERR_NOT_INITIALIZED, ctx);
        return TRELLIS_ERR_NOT_INITIALIZED;
    }

    /* Find a relay-eligible peer that isn't the destination.
     * Prefer peers that have completed capability negotiation and advertised
     * the "relay" feature. Fall back to any connected peer if none qualify.
     * Skip peers in excluded jurisdictions. */
    trellis_peer_conn_t *relay_pc = NULL;
    trellis_peer_conn_t *fallback_pc = NULL;
    for (size_t i = 0; i < client->peer_conn_count; i++) {
        trellis_peer_conn_t *pc = &client->peers[i];
        if (!pc->conn || !pc->handshake_complete)
            continue;
        if (trellis_fingerprint_eq(&pc->remote_fp, dest))
            continue;
        // Jurisdiction exclusion: look up peer in DHT to get jurisdiction.
        const trellis_peer_info_t *pi =
            trellis_dht_lookup_peer(router->dht, &pc->remote_fp);
        if (pi && peer_jurisdiction_excluded(router, pi))
            continue;
        // Skip peers with expired or invalid canaries.
        trellis_err_t canary_err = trellis_canary_check_peer(&pc->remote_fp);
        if (canary_err == TRELLIS_ERR_EXPIRED ||
            canary_err == TRELLIS_ERR_VERIFY_FAILED)
            continue;
        if (pc->caps_received && pc->relay_eligible) {
            relay_pc = pc;
            break;
        }
        if (!fallback_pc)
            fallback_pc = pc;
    }
    if (!relay_pc)
        relay_pc = fallback_pc;

    if (!relay_pc)
        return route_direct(router, dest, data, len, cb, ctx);

    // Build relay envelope: [from_fp][to_fp][inner_data]
    if (len > UINT32_MAX - 2 * TRELLIS_FINGERPRINT_LEN) {
        if (cb) cb(TRELLIS_ERR_INVALID_ARG, ctx);
        return TRELLIS_ERR_INVALID_ARG;
    }
    size_t env_len = 2 * TRELLIS_FINGERPRINT_LEN + len;
    uint8_t *envelope = malloc(env_len);
    if (!envelope) {
        if (cb) cb(TRELLIS_ERR_NOMEM, ctx);
        return TRELLIS_ERR_NOMEM;
    }
    memcpy(envelope, client->identity.fingerprint.bytes, TRELLIS_FINGERPRINT_LEN);
    memcpy(envelope + TRELLIS_FINGERPRINT_LEN, dest->bytes, TRELLIS_FINGERPRINT_LEN);
    memcpy(envelope + 2 * TRELLIS_FINGERPRINT_LEN, data, len);

    // Wrap in a TRELLIS_MSG_RELAY message.
    trellis_message_t rmsg = {0};
    rmsg.version = TRELLIS_PROTOCOL_VERSION;
    rmsg.type = TRELLIS_MSG_RELAY;
    rmsg.flags = TRELLIS_FLAG_RELAYED;
    rmsg.source = client->identity.fingerprint;
    rmsg.target = relay_pc->remote_fp;
    rmsg.payload_len = (uint32_t)env_len;
    rmsg.payload = envelope;

    trellis_err_t err = trellis_message_sign(&rmsg, &client->identity);
    if (err != TRELLIS_OK) {
        free(envelope);
        if (cb) cb(err, ctx);
        return err;
    }

    trellis_buf_t wire = {0};
    err = trellis_message_serialize(&rmsg, &wire);
    free(envelope);
    if (err != TRELLIS_OK) {
        if (cb) cb(err, ctx);
        return err;
    }

    err = trellis_conn_send(relay_pc->conn, wire.data, wire.len, cb, ctx);
    trellis_buf_free(&wire);
    return err;
}

// --- Multipath send ---
static trellis_err_t route_multipath(trellis_router_t *router,
                                     const trellis_fingerprint_t *dest,
                                     const uint8_t *data, size_t len,
                                     trellis_conn_send_cb cb, void *ctx)
{
    trellis_client_t *client = (trellis_client_t *)router->client;
    if (!client) {
        if (cb) cb(TRELLIS_ERR_NOT_INITIALIZED, ctx);
        return TRELLIS_ERR_NOT_INITIALIZED;
    }

    trellis_peer_info_t *candidates[TRELLIS_DHT_K];
    size_t actual = 0;
    select_random_peers(&router->dht->rt, router->dht->sybil,
                        candidates, TRELLIS_DHT_K, &actual);

    if (actual < 2)
        return route_direct(router, dest, data, len, cb, ctx);

    // Select the two most distinct relay peers by XOR distance.
    trellis_peer_info_t *path_a = candidates[0];
    trellis_peer_info_t *path_b = candidates[1];

    uint8_t max_dist[TRELLIS_FINGERPRINT_LEN] = {0};
    for (size_t i = 0; i < actual; i++) {
        for (size_t j = i + 1; j < actual; j++) {
            uint8_t dist[TRELLIS_FINGERPRINT_LEN];
            xor_distance(&candidates[i]->fingerprint,
                         &candidates[j]->fingerprint, dist);
            if (memcmp(dist, max_dist, TRELLIS_FINGERPRINT_LEN) > 0) {
                memcpy(max_dist, dist, TRELLIS_FINGERPRINT_LEN);
                path_a = candidates[i];
                path_b = candidates[j];
            }
        }
    }

    // Build relay envelope: [from_fp][to_fp][inner_data]
    if (len > UINT32_MAX - 2 * TRELLIS_FINGERPRINT_LEN) {
        if (cb) cb(TRELLIS_ERR_INVALID_ARG, ctx);
        return TRELLIS_ERR_INVALID_ARG;
    }
    size_t env_len = 2 * TRELLIS_FINGERPRINT_LEN + len;
    uint8_t *envelope = malloc(env_len);
    if (!envelope) {
        if (cb) cb(TRELLIS_ERR_NOMEM, ctx);
        return TRELLIS_ERR_NOMEM;
    }
    memcpy(envelope, client->identity.fingerprint.bytes, TRELLIS_FINGERPRINT_LEN);
    memcpy(envelope + TRELLIS_FINGERPRINT_LEN, dest->bytes, TRELLIS_FINGERPRINT_LEN);
    memcpy(envelope + 2 * TRELLIS_FINGERPRINT_LEN, data, len);

    trellis_peer_conn_t *pc_a = trellis_client_find_peer(client,
                                                         &path_a->fingerprint);
    trellis_peer_conn_t *pc_b = trellis_client_find_peer(client,
                                                         &path_b->fingerprint);
    trellis_err_t err_a = TRELLIS_ERR_NOT_FOUND;
    trellis_err_t err_b = TRELLIS_ERR_NOT_FOUND;

    trellis_message_t rmsg = {0};
    rmsg.version = TRELLIS_PROTOCOL_VERSION;
    rmsg.type = TRELLIS_MSG_RELAY;
    rmsg.flags = TRELLIS_FLAG_RELAYED;
    rmsg.source = client->identity.fingerprint;
    rmsg.payload_len = (uint32_t)env_len;
    rmsg.payload = envelope;

    // Send via path A.
    if (pc_a && pc_a->conn) {
        rmsg.target = pc_a->remote_fp;
        trellis_err_t serr = trellis_message_sign(&rmsg, &client->identity);
        if (serr == TRELLIS_OK) {
            trellis_buf_t wire = {0};
            serr = trellis_message_serialize(&rmsg, &wire);
            if (serr == TRELLIS_OK) {
                err_a = trellis_conn_send(pc_a->conn, wire.data, wire.len,
                                          NULL, NULL);
                trellis_buf_free(&wire);
            }
        }
    }

    // Send via path B.
    if (pc_b && pc_b->conn) {
        rmsg.target = pc_b->remote_fp;
        trellis_err_t serr = trellis_message_sign(&rmsg, &client->identity);
        if (serr == TRELLIS_OK) {
            trellis_buf_t wire = {0};
            serr = trellis_message_serialize(&rmsg, &wire);
            if (serr == TRELLIS_OK) {
                err_b = trellis_conn_send(pc_b->conn, wire.data, wire.len,
                                          NULL, NULL);
                trellis_buf_free(&wire);
            }
        }
    }

    free(envelope);

    trellis_err_t err = (err_a == TRELLIS_OK || err_b == TRELLIS_OK)
                        ? TRELLIS_OK : err_a;
    if (cb) cb(err, ctx);
    return err;
}

// --- Onion send ---

/*
 * Per-layer wire format:
 *   [X25519 eph_pk][ML-KEM-1024 CT][nonce][ciphertext][tag]
 *
 * Each layer's AES key is derived via hybrid KEM: X25519 ECDH combined
 * with ML-KEM-1024 encapsulation through HKDF-SHAKE-256.
 * Only the hop possessing both secret keys can peel.
 */
#define ONION_HYBRID_OVERHEAD \
    (TRELLIS_X25519_PK_LEN + TRELLIS_ML_KEM_1024_CT_LEN \
     + TRELLIS_AES_NONCE_LEN + TRELLIS_AES_TAG_LEN)

/*
 * stream_id > 0 enables circuit mode: plaintext = [next_fp][stream_id(4)][inner]
 * stream_id == 0: legacy mode: plaintext = [next_fp][inner]
 */
static trellis_err_t onion_wrap_layer(const trellis_fingerprint_t *next_fp,
                                      const uint8_t *inner, size_t inner_len,
                                      const uint8_t *hop_x25519_pk,
                                      const uint8_t *hop_ml_kem_pk,
                                      uint8_t **blob_out, size_t *blob_len_out,
                                      uint32_t stream_id)
{
    size_t sid_len = stream_id ? 4 : 0;
    if (inner_len > SIZE_MAX - TRELLIS_FINGERPRINT_LEN - sid_len)
        return TRELLIS_ERR_INVALID_ARG;
    size_t plain_len = TRELLIS_FINGERPRINT_LEN + sid_len + inner_len;
    if (plain_len > SIZE_MAX - ONION_HYBRID_OVERHEAD)
        return TRELLIS_ERR_INVALID_ARG;
    uint8_t *plain = malloc(plain_len);
    if (!plain)
        return TRELLIS_ERR_NOMEM;
    size_t off = 0;
    memcpy(plain + off, next_fp->bytes, TRELLIS_FINGERPRINT_LEN);
    off += TRELLIS_FINGERPRINT_LEN;
    if (stream_id) {
        plain[off++] = (uint8_t)(stream_id >> 24);
        plain[off++] = (uint8_t)(stream_id >> 16);
        plain[off++] = (uint8_t)(stream_id >> 8);
        plain[off++] = (uint8_t)(stream_id);
    }
    memcpy(plain + off, inner, inner_len);

    trellis_kem_encaps_result_t encaps;
    trellis_err_t err = trellis_kem_encaps(hop_x25519_pk, hop_ml_kem_pk,
                                            &encaps);
    if (err != TRELLIS_OK) {
        free(plain);
        return err;
    }

    uint8_t nonce[TRELLIS_AES_NONCE_LEN];
    randombytes_buf(nonce, TRELLIS_AES_NONCE_LEN);

    size_t blob_len = ONION_HYBRID_OVERHEAD + plain_len;
    uint8_t *blob = malloc(blob_len);
    if (!blob) {
        sodium_memzero(&encaps, sizeof(encaps));
        free(plain);
        return TRELLIS_ERR_NOMEM;
    }

    size_t boff = 0;
    memcpy(blob + boff, encaps.x25519_pk, TRELLIS_X25519_PK_LEN);
    boff += TRELLIS_X25519_PK_LEN;
    memcpy(blob + boff, encaps.ml_kem_ct, TRELLIS_ML_KEM_1024_CT_LEN);
    boff += TRELLIS_ML_KEM_1024_CT_LEN;
    memcpy(blob + boff, nonce, TRELLIS_AES_NONCE_LEN);
    boff += TRELLIS_AES_NONCE_LEN;

    uint8_t tag[TRELLIS_AES_TAG_LEN];
    err = trellis_aes256gcm_encrypt(
        encaps.shared_secret, nonce, plain, plain_len, NULL, 0,
        blob + boff, tag);
    sodium_memzero(&encaps, sizeof(encaps));
    sodium_memzero(plain, plain_len);
    free(plain);

    if (err != TRELLIS_OK) {
        free(blob);
        return err;
    }

    memcpy(blob + boff + plain_len, tag, TRELLIS_AES_TAG_LEN);

    *blob_out = blob;
    *blob_len_out = blob_len;
    return TRELLIS_OK;
}

// Variant that uses a pre-computed KEM encapsulation (from circuit pool).
static trellis_err_t onion_wrap_layer_precomputed(
    const trellis_fingerprint_t *next_fp,
    const uint8_t *inner, size_t inner_len,
    const trellis_kem_encaps_result_t *pre,
    uint8_t **blob_out, size_t *blob_len_out,
    uint32_t stream_id)
{
    size_t sid_len = stream_id ? 4 : 0;
    if (inner_len > SIZE_MAX - TRELLIS_FINGERPRINT_LEN - sid_len)
        return TRELLIS_ERR_INVALID_ARG;
    size_t plain_len = TRELLIS_FINGERPRINT_LEN + sid_len + inner_len;
    if (plain_len > SIZE_MAX - ONION_HYBRID_OVERHEAD)
        return TRELLIS_ERR_INVALID_ARG;
    uint8_t *plain = malloc(plain_len);
    if (!plain)
        return TRELLIS_ERR_NOMEM;
    size_t off = 0;
    memcpy(plain + off, next_fp->bytes, TRELLIS_FINGERPRINT_LEN);
    off += TRELLIS_FINGERPRINT_LEN;
    if (stream_id) {
        plain[off++] = (uint8_t)(stream_id >> 24);
        plain[off++] = (uint8_t)(stream_id >> 16);
        plain[off++] = (uint8_t)(stream_id >> 8);
        plain[off++] = (uint8_t)(stream_id);
    }
    memcpy(plain + off, inner, inner_len);

    uint8_t nonce[TRELLIS_AES_NONCE_LEN];
    randombytes_buf(nonce, TRELLIS_AES_NONCE_LEN);

    size_t blob_len = ONION_HYBRID_OVERHEAD + plain_len;
    uint8_t *blob = malloc(blob_len);
    if (!blob) {
        free(plain);
        return TRELLIS_ERR_NOMEM;
    }

    size_t boff = 0;
    memcpy(blob + boff, pre->x25519_pk, TRELLIS_X25519_PK_LEN);
    boff += TRELLIS_X25519_PK_LEN;
    memcpy(blob + boff, pre->ml_kem_ct, TRELLIS_ML_KEM_1024_CT_LEN);
    boff += TRELLIS_ML_KEM_1024_CT_LEN;
    memcpy(blob + boff, nonce, TRELLIS_AES_NONCE_LEN);
    boff += TRELLIS_AES_NONCE_LEN;

    uint8_t tag[TRELLIS_AES_TAG_LEN];
    trellis_err_t err = trellis_aes256gcm_encrypt(
        pre->shared_secret, nonce, plain, plain_len, NULL, 0,
        blob + boff, tag);
    sodium_memzero(plain, plain_len);
    free(plain);

    if (err != TRELLIS_OK) {
        free(blob);
        return err;
    }

    memcpy(blob + boff + plain_len, tag, TRELLIS_AES_TAG_LEN);

    *blob_out = blob;
    *blob_len_out = blob_len;
    return TRELLIS_OK;
}

// Forward declarations for circuit pool.
void trellis_circuit_pool_init(trellis_router_t *router);
prebuilt_circuit_t *trellis_circuit_pool_acquire(trellis_router_t *router,
                                                  const trellis_fingerprint_t *dest);
uint32_t trellis_circuit_pool_alloc_stream(prebuilt_circuit_t *circuit);
uint32_t trellis_circuit_pool_alloc_stream_with_id(prebuilt_circuit_t *circuit,
                                                    uint32_t forced_id);
void trellis_circuit_pool_close_stream(prebuilt_circuit_t *circuit, uint32_t stream_id);
void trellis_circuit_pool_release(trellis_router_t *router, prebuilt_circuit_t *circuit);

static trellis_err_t route_onion_ex(trellis_router_t *router,
                                    const trellis_fingerprint_t *dest,
                                    const uint8_t *data, size_t len,
                                    uint32_t force_stream_id,
                                    uint32_t *stream_id_out,
                                    trellis_conn_send_cb cb, void *ctx)
{
    trellis_client_t *client = (trellis_client_t *)router->client;
    if (!client) {
        if (cb) cb(TRELLIS_ERR_NOT_INITIALIZED, ctx);
        return TRELLIS_ERR_NOT_INITIALIZED;
    }

    static const uint8_t zero_x25519[TRELLIS_X25519_PK_LEN] = {0};
    static const uint8_t zero_mlkem[TRELLIS_ML_KEM_1024_PK_LEN] = {0};

    /* Try to reuse a pre-built circuit from the pool.
     * If successful, skip the expensive hop-selection / DHT lookup path
     * and jump straight to onion wrapping with the pooled hops. */
    prebuilt_circuit_t *pooled = trellis_circuit_pool_acquire(router, dest);
    uint32_t pooled_sid = 0;
    if (pooled && pooled->hop_depth == 3) {
        pooled_sid = trellis_circuit_pool_alloc_stream_with_id(
            pooled, force_stream_id);
        if (pooled_sid == 0)
            pooled = NULL;
    } else {
        pooled = NULL;
    }

    trellis_peer_info_t *hop1 = NULL;
    trellis_peer_info_t *hop2 = NULL;
    trellis_peer_info_t *hop3 = NULL;
    uint32_t stream_id;

    if (pooled) {
        // Resolve pooled fingerprints back to peer_info for KEM keys.
        hop1 = (trellis_peer_info_t *)trellis_dht_lookup_peer(router->dht, &pooled->hops[0]);
        hop2 = (trellis_peer_info_t *)trellis_dht_lookup_peer(router->dht, &pooled->hops[1]);
        hop3 = (trellis_peer_info_t *)trellis_dht_lookup_peer(router->dht, &pooled->hops[2]);
        stream_id = pooled_sid;

        if (!hop1 || !hop2 || !hop3 ||
            memcmp(hop1->identity.x25519_pk, zero_x25519, TRELLIS_X25519_PK_LEN) == 0 ||
            memcmp(hop2->identity.x25519_pk, zero_x25519, TRELLIS_X25519_PK_LEN) == 0 ||
            memcmp(hop3->identity.x25519_pk, zero_x25519, TRELLIS_X25519_PK_LEN) == 0) {
            trellis_circuit_pool_close_stream(pooled, pooled_sid);
            trellis_circuit_pool_release(router, pooled);
            pooled = NULL;
        } else {
            // Invalidate pre-computed encaps if any hop key rotated.
            if (pooled->encaps_valid) {
                const trellis_peer_info_t *ph[3] = {hop1, hop2, hop3};
                for (int ki = 0; ki < 3; ki++) {
                    if (memcmp(ph[ki]->identity.x25519_pk,
                               pooled->hop_x25519_snapshot[ki],
                               TRELLIS_X25519_PK_LEN) != 0) {
                        pooled->encaps_valid = false;
                        break;
                    }
                }
            }
            fprintf(stderr, "[ONION] reusing pooled circuit %08x (encaps=%s)\n",
                    stream_id, pooled->encaps_valid ? "pre" : "fresh");
        }
    }

    if (!pooled) {
        trellis_peer_info_t *candidates[TRELLIS_DHT_K];
        size_t actual = 0;
        select_random_peers(&router->dht->rt, router->dht->sybil,
                            candidates, TRELLIS_DHT_K, &actual);

        trellis_peer_info_t *relays[TRELLIS_DHT_K];
        size_t relay_count = 0;
        for (size_t i = 0; i < actual && relay_count < TRELLIS_DHT_K; i++) {
            if (trellis_fingerprint_eq(&candidates[i]->fingerprint, dest))
                continue;
            if (memcmp(candidates[i]->identity.x25519_pk, zero_x25519,
                       TRELLIS_X25519_PK_LEN) == 0)
                continue;
            if (memcmp(candidates[i]->identity.ml_kem_pk, zero_mlkem,
                       TRELLIS_ML_KEM_1024_PK_LEN) == 0)
                continue;
            trellis_peer_conn_t *hop_pc = trellis_client_find_peer(
                client, &candidates[i]->fingerprint);
            if (hop_pc && hop_pc->caps_received && !hop_pc->relay_eligible)
                continue;
            if (peer_jurisdiction_excluded(router, candidates[i]))
                continue;
            trellis_err_t canary_err =
                trellis_canary_check_peer(&candidates[i]->fingerprint);
            if (canary_err == TRELLIS_ERR_EXPIRED ||
                canary_err == TRELLIS_ERR_VERIFY_FAILED)
                continue;

            /* Enrich candidate bandwidth from descriptor cache so the
             * sybil weighted selection picks up measured values. */
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

        if (relay_count < 3) {
            fprintf(stderr, "[ONION] only %zu eligible relay peers (need 3) "
                    "— falling back to relay\n", relay_count);
            return route_relay(router, dest, data, len, cb, ctx);
        }

        trellis_peer_info_t *hops[3] = {NULL, NULL, NULL};
        if (router->dht->sybil) {
            size_t got = trellis_sybil_select_hops(
                router->dht->sybil,
                relays, relay_count,
                dest, hops, 3);
            if (got < 3) {
                fprintf(stderr, "[ONION] sybil hop selection returned %zu "
                        "(need 3) — falling back to relay\n", got);
                return route_relay(router, dest, data, len, cb, ctx);
            }
        } else {
            hops[0] = relays[0];
            hops[1] = relays[1];
            hops[2] = relays[2];
        }

        if (router->guard) {
            for (int gi = 0; gi < 3; gi++) {
                const trellis_fingerprint_t *gfp = trellis_guard_get(router->guard, gi);
                if (!gfp)
                    continue;
                for (size_t ri = 0; ri < relay_count; ri++) {
                    if (trellis_fingerprint_eq(&relays[ri]->fingerprint, gfp)) {
                        trellis_peer_info_t *tmp = relays[0];
                        relays[0] = relays[ri];
                        relays[ri] = tmp;
                        hops[0] = relays[0];
                        trellis_guard_mark_used(router->guard, gfp);
                        fprintf(stderr, "[ONION] using pinned guard %02x%02x... "
                                "as entry hop\n", gfp->bytes[0], gfp->bytes[1]);
                        goto guard_selected;
                    }
                }
            }
        }
    guard_selected:;

        /* After guard override, ensure hops[1] and hops[2] differ from
         * hops[0].  The L2/L3 vanguard blocks below handle the case when
         * vanguards are configured; this covers the un-vanguarded path. */
        for (int hi = 1; hi <= 2; hi++) {
            if (trellis_fingerprint_eq(&hops[hi]->fingerprint,
                                       &hops[0]->fingerprint)) {
                for (size_t ri = 0; ri < relay_count; ri++) {
                    bool dup = false;
                    for (int hj = 0; hj < 3; hj++) {
                        if (hj == hi) continue;
                        if (trellis_fingerprint_eq(&relays[ri]->fingerprint,
                                                   &hops[hj]->fingerprint)) {
                            dup = true;
                            break;
                        }
                    }
                    if (!dup) { hops[hi] = relays[ri]; break; }
                }
            }
        }

        // L2 vanguard: pin hops[1] to a known L2 vanguard if available.
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
                        goto l2_selected;
                    }
                }
            }
        }
    l2_selected:;

        // L3 vanguard: pin hops[2] to a known L3 vanguard if available.
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
                        goto l3_selected;
                    }
                }
            }
        }
    l3_selected:;

        hop1 = hops[0];
        hop2 = hops[1];
        hop3 = hops[2];

        if (force_stream_id > 0) {
            stream_id = force_stream_id;
        } else {
            randombytes_buf(&stream_id, sizeof(stream_id));
            if (stream_id == 0) stream_id = 1;
        }
    }

    // Look up the destination's hybrid keys for the innermost layer.
    const trellis_peer_info_t *dest_info =
        trellis_dht_lookup_peer(router->dht, dest);
    if (!dest_info ||
        memcmp(dest_info->identity.x25519_pk, zero_x25519,
               TRELLIS_X25519_PK_LEN) == 0 ||
        memcmp(dest_info->identity.ml_kem_pk, zero_mlkem,
               TRELLIS_ML_KEM_1024_PK_LEN) == 0) {
        fprintf(stderr, "[ONION] destination %02x%02x... missing hybrid keys "
                "in DHT — falling back to relay\n",
                dest->bytes[0], dest->bytes[1]);
        if (pooled) {
            trellis_circuit_pool_close_stream(pooled, pooled_sid);
            trellis_circuit_pool_release(router, pooled);
        }
        return route_relay(router, dest, data, len, cb, ctx);
    }

    fprintf(stderr, "[ONION] building 4-layer circuit: hop1=%02x%02x... "
            "hop2=%02x%02x... hop3=%02x%02x... dest=%02x%02x...\n",
            hop1->fingerprint.bytes[0], hop1->fingerprint.bytes[1],
            hop2->fingerprint.bytes[0], hop2->fingerprint.bytes[1],
            hop3->fingerprint.bytes[0], hop3->fingerprint.bytes[1],
            dest->bytes[0], dest->bytes[1]);

    trellis_err_t err;

    bool use_pre = pooled && pooled->encaps_valid;

    // Layer 4: destination — always fresh KEM.
    uint8_t *l4_blob = NULL;
    size_t l4_len = 0;
    err = onion_wrap_layer(dest, data, len,
                           dest_info->identity.x25519_pk,
                           dest_info->identity.ml_kem_pk,
                           &l4_blob, &l4_len, stream_id);
    if (err != TRELLIS_OK) {
        if (pooled) trellis_circuit_pool_close_stream(pooled, pooled_sid);
        if (cb) cb(err, ctx);
        return err;
    }

    // Layer 3: hop3 — use precomputed encaps[2] if available.
    uint8_t *l3_blob = NULL;
    size_t l3_len = 0;
    if (use_pre)
        err = onion_wrap_layer_precomputed(dest, l4_blob, l4_len,
                                           &pooled->hop_encaps[2],
                                           &l3_blob, &l3_len, stream_id);
    else
        err = onion_wrap_layer(dest, l4_blob, l4_len,
                               hop3->identity.x25519_pk,
                               hop3->identity.ml_kem_pk,
                               &l3_blob, &l3_len, stream_id);
    free(l4_blob);
    if (err != TRELLIS_OK) {
        if (pooled) trellis_circuit_pool_close_stream(pooled, pooled_sid);
        if (cb) cb(err, ctx);
        return err;
    }

    // Layer 2: hop2 — use precomputed encaps[1] if available.
    uint8_t *l2_blob = NULL;
    size_t l2_len = 0;
    if (use_pre)
        err = onion_wrap_layer_precomputed(&hop3->fingerprint, l3_blob, l3_len,
                                           &pooled->hop_encaps[1],
                                           &l2_blob, &l2_len, stream_id);
    else
        err = onion_wrap_layer(&hop3->fingerprint, l3_blob, l3_len,
                               hop2->identity.x25519_pk,
                               hop2->identity.ml_kem_pk,
                               &l2_blob, &l2_len, stream_id);
    free(l3_blob);
    if (err != TRELLIS_OK) {
        if (pooled) trellis_circuit_pool_close_stream(pooled, pooled_sid);
        if (cb) cb(err, ctx);
        return err;
    }

    // Layer 1: hop1 — use precomputed encaps[0] if available.
    uint8_t *l1_blob = NULL;
    size_t l1_len = 0;
    if (use_pre)
        err = onion_wrap_layer_precomputed(&hop2->fingerprint, l2_blob, l2_len,
                                           &pooled->hop_encaps[0],
                                           &l1_blob, &l1_len, stream_id);
    else
        err = onion_wrap_layer(&hop2->fingerprint, l2_blob, l2_len,
                               hop1->identity.x25519_pk,
                               hop1->identity.ml_kem_pk,
                               &l1_blob, &l1_len, stream_id);
    free(l2_blob);
    if (err != TRELLIS_OK) {
        if (pooled) trellis_circuit_pool_close_stream(pooled, pooled_sid);
        if (cb) cb(err, ctx);
        return err;
    }

    // Wrap in TRELLIS_MSG_ONION and send to hop1.
    if (l1_len > UINT32_MAX) {
        free(l1_blob);
        if (pooled) trellis_circuit_pool_close_stream(pooled, pooled_sid);
        if (cb) cb(TRELLIS_ERR_INVALID_ARG, ctx);
        return TRELLIS_ERR_INVALID_ARG;
    }
    trellis_message_t omsg = {0};
    omsg.version = TRELLIS_PROTOCOL_VERSION;
    omsg.type = TRELLIS_MSG_ONION;
    omsg.timestamp = trellis_now_ms();
    omsg.target = hop1->fingerprint;
    omsg.payload_len = (uint32_t)l1_len;
    omsg.payload = l1_blob;

    trellis_buf_t wire = {0};
    err = trellis_message_serialize(&omsg, &wire);
    free(l1_blob);
    if (err != TRELLIS_OK) {
        if (pooled) trellis_circuit_pool_close_stream(pooled, pooled_sid);
        if (cb) cb(err, ctx);
        return err;
    }

    trellis_peer_conn_t *pc = trellis_client_find_peer(client,
                                                       &hop1->fingerprint);
    if (pc && pc->conn) {
        fprintf(stderr, "[ONION] sent %zu-byte onion packet to hop1 %02x%02x...\n",
                wire.len, hop1->fingerprint.bytes[0], hop1->fingerprint.bytes[1]);
        err = trellis_conn_send(pc->conn, wire.data, wire.len, cb, ctx);
        if (err == TRELLIS_OK && router->guard)
            trellis_guard_path_bias_attempt(router->guard,
                                            &hop1->fingerprint);
        if (err != TRELLIS_OK && pooled)
            trellis_circuit_pool_close_stream(pooled, pooled_sid);
    } else {
        fprintf(stderr, "[ONION] no connection to hop1 %02x%02x...\n",
                hop1->fingerprint.bytes[0], hop1->fingerprint.bytes[1]);
        if (pooled) trellis_circuit_pool_close_stream(pooled, pooled_sid);
        err = TRELLIS_ERR_NOT_FOUND;
        if (cb) cb(err, ctx);
    }

    trellis_buf_free(&wire);

    if (err == TRELLIS_OK && stream_id_out)
        *stream_id_out = stream_id;

    return err;
}

static trellis_err_t route_onion(trellis_router_t *router,
                                 const trellis_fingerprint_t *dest,
                                 const uint8_t *data, size_t len,
                                 trellis_conn_send_cb cb, void *ctx)
{
    return route_onion_ex(router, dest, data, len, 0, NULL, cb, ctx);
}

// --- Session-keyed relay send (low-overhead media path) ---
static trellis_err_t route_session_relay(trellis_router_t *router,
                                         const trellis_fingerprint_t *dest,
                                         const uint8_t *data, size_t len,
                                         trellis_conn_send_cb cb, void *ctx)
{
    trellis_client_t *client = (trellis_client_t *)router->client;
    if (!client) {
        if (cb) cb(TRELLIS_ERR_NOT_INITIALIZED, ctx);
        return TRELLIS_ERR_NOT_INITIALIZED;
    }

    session_circuit_t *sc = trellis_session_pool_acquire(router, dest);
    if (!sc) {
        fprintf(stderr, "[SESSION] no session circuit available, falling back to relay\n");
        return route_relay(router, dest, data, len, cb, ctx);
    }

    /* If this session circuit has KEM encapsulation that hasn't been sent
     * to the relay yet, send CIRCUIT_CREATE_SESSION first. Note: we send
     * SESSION_DATA immediately without waiting for CREATED_SESSION. The
     * relay may drop the first few packets if it hasn't finished key
     * derivation yet. This is acceptable for media (WebRTC tolerates loss)
     * and avoids an extra RTT on the critical path. */
    if (sc->encaps_valid) {
        size_t ct_len = TRELLIS_X25519_PK_LEN + TRELLIS_ML_KEM_1024_CT_LEN;
        size_t setup_len = 4 + ct_len;
        uint8_t *setup = malloc(setup_len);
        if (!setup) {
            fprintf(stderr, "[SESSION] CREATE_SESSION alloc failed, falling back to relay\n");
            return route_relay(router, dest, data, len, cb, ctx);
        }
        {
            setup[0] = (uint8_t)(sc->circuit_id >> 24);
            setup[1] = (uint8_t)(sc->circuit_id >> 16);
            setup[2] = (uint8_t)(sc->circuit_id >> 8);
            setup[3] = (uint8_t)(sc->circuit_id);
            memcpy(setup + 4, sc->encaps.x25519_pk, TRELLIS_X25519_PK_LEN);
            memcpy(setup + 4 + TRELLIS_X25519_PK_LEN,
                   sc->encaps.ml_kem_ct, TRELLIS_ML_KEM_1024_CT_LEN);

            trellis_message_t setup_msg = {0};
            setup_msg.version = TRELLIS_PROTOCOL_VERSION;
            setup_msg.type = TRELLIS_MSG_CIRCUIT_CREATE_SESSION;
            setup_msg.source = client->identity.fingerprint;
            setup_msg.target = sc->relay;
            setup_msg.payload_len = (uint32_t)setup_len;
            setup_msg.payload = setup;

            trellis_err_t serr = trellis_message_sign(&setup_msg, &client->identity);
            if (serr == TRELLIS_OK) {
                trellis_buf_t wire = {0};
                serr = trellis_message_serialize(&setup_msg, &wire);
                if (serr == TRELLIS_OK) {
                    trellis_peer_conn_t *rpc = trellis_client_find_peer(client, &sc->relay);
                    if (rpc && rpc->conn)
                        trellis_conn_send(rpc->conn, wire.data, wire.len, NULL, NULL);
                    trellis_buf_free(&wire);
                }
            }
            free(setup);
        }
        sc->encaps_valid = false; /* only send once */
        sodium_memzero(&sc->encaps, sizeof(sc->encaps));
    }

    // AES-256-GCM encrypt with session key, monotonic nonce.
    uint8_t nonce[TRELLIS_AES_NONCE_LEN];
    memset(nonce, 0, TRELLIS_AES_NONCE_LEN);
    uint64_t nc = sc->nonce_counter++;
    for (int i = 0; i < 8; i++)
        nonce[TRELLIS_AES_NONCE_LEN - 1 - i] = (uint8_t)(nc >> (i * 8));

    // SESSION_DATA wire format: [circuit_id(4)][dest_fp(32)][nonce(12)][ciphertext(len)][tag(16)]
    size_t frame_len = 4 + TRELLIS_FINGERPRINT_LEN + TRELLIS_AES_NONCE_LEN + len + TRELLIS_AES_TAG_LEN;
    uint8_t *frame = malloc(frame_len);
    if (!frame) {
        if (cb) cb(TRELLIS_ERR_NOMEM, ctx);
        return TRELLIS_ERR_NOMEM;
    }

    size_t off = 0;
    frame[off++] = (uint8_t)(sc->circuit_id >> 24);
    frame[off++] = (uint8_t)(sc->circuit_id >> 16);
    frame[off++] = (uint8_t)(sc->circuit_id >> 8);
    frame[off++] = (uint8_t)(sc->circuit_id);
    memcpy(frame + off, dest->bytes, TRELLIS_FINGERPRINT_LEN);
    off += TRELLIS_FINGERPRINT_LEN;
    memcpy(frame + off, nonce, TRELLIS_AES_NONCE_LEN);
    off += TRELLIS_AES_NONCE_LEN;

    uint8_t tag[TRELLIS_AES_TAG_LEN];
    trellis_err_t err = trellis_aes256gcm_encrypt(
        sc->session_key, nonce, data, len, NULL, 0,
        frame + off, tag);
    if (err != TRELLIS_OK) {
        free(frame);
        if (cb) cb(err, ctx);
        return err;
    }
    off += len;
    memcpy(frame + off, tag, TRELLIS_AES_TAG_LEN);

    // Wrap in TRELLIS_MSG_SESSION_DATA and send to relay.
    trellis_message_t smsg = {0};
    smsg.version = TRELLIS_PROTOCOL_VERSION;
    smsg.type = TRELLIS_MSG_SESSION_DATA;
    smsg.flags = TRELLIS_FLAG_MORPH_BYPASS;
    smsg.source = client->identity.fingerprint;
    smsg.target = sc->relay;
    smsg.payload_len = (uint32_t)frame_len;
    smsg.payload = frame;

    /* TODO(perf): hybrid signing (Ed25519 + ML-DSA-87) adds ~4.7KB per
     * packet. For high-rate media (50 pkt/s), consider Ed25519-only or a
     * MAC-based scheme since AES-GCM already authenticates the payload. */
    err = trellis_message_sign(&smsg, &client->identity);
    if (err != TRELLIS_OK) {
        free(frame);
        if (cb) cb(err, ctx);
        return err;
    }

    trellis_buf_t wire = {0};
    err = trellis_message_serialize(&smsg, &wire);
    free(frame);
    if (err != TRELLIS_OK) {
        if (cb) cb(err, ctx);
        return err;
    }

    trellis_peer_conn_t *pc = trellis_client_find_peer(client, &sc->relay);
    if (pc && pc->conn) {
        err = trellis_conn_send(pc->conn, wire.data, wire.len, cb, ctx);
    } else {
        err = TRELLIS_ERR_NOT_FOUND;
        if (cb) cb(err, ctx);
    }

    trellis_buf_free(&wire);
    return err;
}

static int select_hop_count(uint8_t msg_flags,
                            trellis_privacy_level_t pref)
{
    if (msg_flags & TRELLIS_FLAG_MORPH_BYPASS) /* media stream */
        return (pref == TRELLIS_PRIVACY_MAX) ? 3 : 1;
    return 3; /* signaling always full onion */
}

static trellis_err_t route_adaptive(trellis_router_t *router,
                                    const trellis_fingerprint_t *dest,
                                    const uint8_t *data, size_t len,
                                    uint8_t msg_flags,
                                    trellis_conn_send_cb cb, void *ctx)
{
    int hops = select_hop_count(msg_flags, router->privacy_level);

    if (hops == 1)
        return route_session_relay(router, dest, data, len, cb, ctx);

    return route_onion(router, dest, data, len, cb, ctx);
}

// --- Dispatch ---
trellis_err_t trellis_router_send(trellis_router_t *router,
                                   const trellis_fingerprint_t *dest,
                                   const uint8_t *data, size_t len,
                                   trellis_conn_send_cb cb, void *ctx)
{
    if (!router || !dest || !data)
        return TRELLIS_ERR_INVALID_ARG;

    switch (router->mode) {
    case TRELLIS_ROUTE_DIRECT:
        return route_direct(router, dest, data, len, cb, ctx);
    case TRELLIS_ROUTE_RELAY:
        return route_relay(router, dest, data, len, cb, ctx);
    case TRELLIS_ROUTE_MULTIPATH:
        return route_multipath(router, dest, data, len, cb, ctx);
    case TRELLIS_ROUTE_ONION:
        return route_onion(router, dest, data, len, cb, ctx);
    case TRELLIS_ROUTE_AUTO: {
        trellis_err_t err = route_direct(router, dest, data, len, NULL, NULL);
        if (err != TRELLIS_OK)
            err = route_relay(router, dest, data, len, cb, ctx);
        else if (cb)
            cb(TRELLIS_OK, ctx);
        return err;
    }
    case TRELLIS_ROUTE_ADAPTIVE:
        return route_adaptive(router, dest, data, len, 0, cb, ctx);
    }

    return TRELLIS_ERR_ROUTING;
}

trellis_err_t trellis_router_send_ex(trellis_router_t *router,
                                      const trellis_fingerprint_t *dest,
                                      const uint8_t *data, size_t len,
                                      uint32_t force_stream_id,
                                      uint32_t *stream_id_out,
                                      trellis_conn_send_cb cb, void *ctx)
{
    if (!router || !dest || !data)
        return TRELLIS_ERR_INVALID_ARG;

    return route_onion_ex(router, dest, data, len,
                          force_stream_id, stream_id_out, cb, ctx);
}

trellis_err_t trellis_router_send_with_flags(trellis_router_t *router,
                                              const trellis_fingerprint_t *dest,
                                              const uint8_t *data, size_t len,
                                              uint8_t flags,
                                              trellis_conn_send_cb cb, void *ctx)
{
    if (!router || !dest || !data)
        return TRELLIS_ERR_INVALID_ARG;

    if (router->mode == TRELLIS_ROUTE_ADAPTIVE)
        return route_adaptive(router, dest, data, len, flags, cb, ctx);

    return trellis_router_send(router, dest, data, len, cb, ctx);
}

void trellis_router_set_privacy_level(trellis_router_t *router,
                                       trellis_privacy_level_t level)
{
    if (router)
        router->privacy_level = level;
}
