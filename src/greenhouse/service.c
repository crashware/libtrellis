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

#include <stdio.h>
#include <trellis/trellis.h>
#include <trellis/types.h>
#include <trellis/mesh.h>
#include <trellis/rhizome.h>

trellis_greenhouse_t *trellis_greenhouse_new(const trellis_identity_t *service_id,
                                              trellis_greenhouse_type_t type,
                                              trellis_dht_t *dht)
{
    if (!service_id || !dht)
        return NULL;

    trellis_greenhouse_t *gh = calloc(1, sizeof(*gh));
    if (!gh)
        return NULL;

    gh->service_id = *service_id;
    gh->type = type;
    gh->dht = dht;
    return gh;
}

void trellis_greenhouse_free(trellis_greenhouse_t *gh)
{
    if (!gh)
        return;

    if (gh->running)
        trellis_greenhouse_stop(gh);

    if (gh->federation.local_share) {
        sodium_memzero(gh->federation.local_share,
                       gh->federation.local_share_len);
        free(gh->federation.local_share);
    }

    trellis_descriptor_free(&gh->descriptor);
    sodium_memzero(&gh->service_id, sizeof(gh->service_id));
    free(gh);
}

void trellis_greenhouse_set_router(trellis_greenhouse_t *gh,
                                    trellis_router_t *router)
{
    if (gh)
        gh->router = router;
}

trellis_err_t trellis_greenhouse_acl_add(trellis_greenhouse_t *gh,
                                          const trellis_fingerprint_t *fp)
{
    if (!gh || !fp)
        return TRELLIS_ERR_INVALID_ARG;
    if (gh->acl_count >= TRELLIS_GH_MAX_ACL)
        return TRELLIS_ERR_FULL;
    for (size_t i = 0; i < gh->acl_count; i++) {
        if (trellis_fingerprint_eq(&gh->acl[i], fp))
            return TRELLIS_ERR_ALREADY_EXISTS;
    }
    gh->acl[gh->acl_count++] = *fp;
    return TRELLIS_OK;
}

trellis_err_t trellis_greenhouse_acl_remove(trellis_greenhouse_t *gh,
                                             const trellis_fingerprint_t *fp)
{
    if (!gh || !fp)
        return TRELLIS_ERR_INVALID_ARG;
    for (size_t i = 0; i < gh->acl_count; i++) {
        if (trellis_fingerprint_eq(&gh->acl[i], fp)) {
            gh->acl[i] = gh->acl[--gh->acl_count];
            return TRELLIS_OK;
        }
    }
    return TRELLIS_ERR_NOT_FOUND;
}

trellis_err_t trellis_greenhouse_set_federation(
                    trellis_greenhouse_t *gh,
                    const uint8_t *share, size_t share_len,
                    int share_idx,
                    int threshold, int total,
                    const trellis_fingerprint_t *peer_fps,
                    size_t peer_count)
{
    if (!gh || !share || share_len == 0 || !peer_fps)
        return TRELLIS_ERR_INVALID_ARG;
    if (gh->type != TRELLIS_GH_FEDERATED)
        return TRELLIS_ERR_INVALID_ARG;
    if (threshold < 1 || total < threshold || total > 255)
        return TRELLIS_ERR_INVALID_ARG;
    if (share_idx < 1 || share_idx > total)
        return TRELLIS_ERR_INVALID_ARG;
    if (peer_count == 0 || peer_count > TRELLIS_MAX_INTRO_POINTS)
        return TRELLIS_ERR_INVALID_ARG;
    if (gh->running)
        return TRELLIS_ERR_ALREADY_EXISTS;

    gh->federation.local_share = malloc(share_len);
    if (!gh->federation.local_share)
        return TRELLIS_ERR_NOMEM;

    memcpy(gh->federation.local_share, share, share_len);
    gh->federation.local_share_len = share_len;
    gh->federation.local_share_idx = share_idx;
    gh->federation.threshold = threshold;
    gh->federation.total = total;
    gh->federation.peer_count = peer_count;
    for (size_t i = 0; i < peer_count; i++)
        gh->federation.peers[i] = peer_fps[i];

    return TRELLIS_OK;
}

// --- ACL helpers ---
static bool acl_check(const trellis_greenhouse_t *gh,
                      const trellis_fingerprint_t *client)
{
    if (gh->type != TRELLIS_GH_AUTHENTICATED)
        return true; /* no ACL enforcement for other types */
    if (!client)
        return false;
    for (size_t i = 0; i < gh->acl_count; i++) {
        if (trellis_fingerprint_eq(&gh->acl[i], client))
            return true;
    }
    return false;
}

static trellis_err_t establish_intro_points(trellis_greenhouse_t *gh)
{
    gh->descriptor.intro_points = calloc(TRELLIS_MAX_INTRO_POINTS,
                                         sizeof(*gh->descriptor.intro_points));
    if (!gh->descriptor.intro_points)
        return TRELLIS_ERR_NOMEM;

    /*
     * Select relay nodes from the DHT as introduction points.
     * Build an onion circuit to each and register as ESTABLISH_INTRO.
     * Clients will contact these relays to initiate rendezvous.
     */
    size_t target_count = TRELLIS_MAX_INTRO_POINTS;
    trellis_peer_info_t peers[TRELLIS_MAX_INTRO_POINTS * 2];
    size_t available = trellis_dht_get_peers(gh->dht, peers,
                                             target_count * 2);

    if (available == 0) {
        /*
         * No peers in DHT yet -- fall back to advertising local identity
         * as intro point so the descriptor is still structurally valid.
         */
        trellis_intro_point_t *ip = &gh->descriptor.intro_points[0];
        ip->node_fingerprint = gh->service_id.fingerprint;
        memcpy(ip->onion_key, gh->service_id.ml_kem_pk,
               TRELLIS_ML_KEM_1024_PK_LEN);
        ip->hint_count = 0;
        gh->descriptor.intro_point_count = 1;
        gh->intro_count = 1;
        gh->intro_points[0].point = *ip;
        gh->intro_points[0].active = true;
        return TRELLIS_OK;
    }

    size_t established = 0;
    for (size_t i = 0; i < available && established < target_count; i++) {
        if (peers[i].quarantined)
            continue;
        if (!peers[i].greenhouse_capable)
            continue;
        if (trellis_fingerprint_eq(&peers[i].fingerprint,
                                   &gh->service_id.fingerprint))
            continue;

        static const uint8_t zero_mlkem[TRELLIS_ML_KEM_1024_PK_LEN] = {0};
        if (memcmp(peers[i].identity.ml_kem_pk, zero_mlkem,
                   TRELLIS_ML_KEM_1024_PK_LEN) == 0)
            continue;

        // Build onion circuit to this relay.
        trellis_onion_circuit_t *circuit = trellis_onion_new(1);
        if (!circuit)
            continue;

        trellis_err_t err = trellis_onion_add_hop(
            circuit, &peers[i].fingerprint,
            peers[i].identity.x25519_pk,
            peers[i].identity.ml_kem_pk);
        if (err != TRELLIS_OK) {
            trellis_onion_free(circuit);
            continue;
        }

        // Register intro point in the descriptor.
        trellis_intro_point_t *ip =
            &gh->descriptor.intro_points[established];
        ip->node_fingerprint = peers[i].fingerprint;
        memcpy(ip->onion_key, peers[i].identity.ml_kem_pk,
               TRELLIS_ML_KEM_1024_PK_LEN);
        ip->hint_count = 0;

        gh->intro_points[established].point = *ip;

        /*
         * Send ESTABLISH_INTRO to the relay via the router.
         *
         * Payload format:
         *   [4]  magic  "ESTI"
         *   [1]  version = 1
         *   [32] service fingerprint
         *   [TRELLIS_ML_KEM_1024_PK_LEN] service onion key (ML-KEM-1024)
         *   [32] intro point index (LE u32)
         *
         * The relay stores the service fingerprint + onion key, then
         * forwards INTRODUCE cells from clients to our intro-message handler.
         *
         * When a router is not set (e.g., loopback test), we skip the send
         * but still mark the point active so the descriptor is valid.
         */
        if (gh->router) {
            static const uint8_t ESTI_MAGIC[4] = {'E','S','T','I'};
            uint8_t estab_payload[4 + 1 + 32 + TRELLIS_ML_KEM_1024_PK_LEN + 4];
            size_t off = 0;

            memcpy(estab_payload + off, ESTI_MAGIC, 4); off += 4;
            estab_payload[off++] = 1; /* version */
            memcpy(estab_payload + off, &gh->service_id.fingerprint,
                   sizeof(gh->service_id.fingerprint)); off += 32;
            memcpy(estab_payload + off, gh->service_id.ml_kem_pk,
                   TRELLIS_ML_KEM_1024_PK_LEN); off += TRELLIS_ML_KEM_1024_PK_LEN;
            // BE u32 index.
            estab_payload[off+0] = 0; estab_payload[off+1] = 0;
            estab_payload[off+2] = (uint8_t)((established >> 8) & 0xFF);
            estab_payload[off+3] = (uint8_t)(established & 0xFF); off += 4;

            /*
             * Onion-wrap the payload for delivery through the circuit.
             * The relay is the single hop, so this just adds the KEM
             * layer for the relay to decrypt.
             */
            trellis_buf_t wrapped = {0};
            trellis_err_t werr = trellis_onion_wrap(
                circuit, estab_payload, off, &wrapped);
            if (werr == TRELLIS_OK && wrapped.data) {
                trellis_router_send(gh->router, &peers[i].fingerprint,
                                    wrapped.data, wrapped.len, NULL, NULL);
                trellis_buf_free(&wrapped);
                gh->intro_points[established].active = true;
                fprintf(stderr,
                    "[GH] ESTABLISH_INTRO sent to relay %02x%02x… (slot %zu)\n",
                    ((const uint8_t *)&peers[i].fingerprint)[0],
                    ((const uint8_t *)&peers[i].fingerprint)[1],
                    established);
            } else {
                fprintf(stderr,
                    "[GH] onion wrap failed for ESTABLISH_INTRO (slot %zu)\n",
                    established);
                gh->intro_points[established].active = false;
                trellis_onion_free(circuit);
                continue;
            }
        } else {
            // No router — mark active for loopback/test mode.
            gh->intro_points[established].active = true;
        }

        trellis_onion_free(circuit);

        established++;
    }

    if (established == 0) {
        // Fall back to local identity.
        trellis_intro_point_t *ip = &gh->descriptor.intro_points[0];
        ip->node_fingerprint = gh->service_id.fingerprint;
        memcpy(ip->onion_key, gh->service_id.ml_kem_pk,
               TRELLIS_ML_KEM_1024_PK_LEN);
        ip->hint_count = 0;
        established = 1;
        gh->intro_points[0].point = *ip;
        gh->intro_points[0].active = true;
    }

    gh->descriptor.intro_point_count = established;
    gh->intro_count = established;
    return TRELLIS_OK;
}

// --- Long-lived intro circuit maintenance ---
static void intro_keepalive_cb(uv_timer_t *handle)
{
    trellis_greenhouse_t *gh = handle->data;
    if (!gh || !gh->running)
        return;

    uint64_t now = trellis_now_ms();

    // Re-establish any dead intro circuits.
    for (size_t i = 0; i < gh->intro_count; i++) {
        if (!gh->intro_points[i].active || gh->intro_points[i].conn)
            continue;
        // Circuit went dead — mark and will be rebuilt on next establish.
        gh->intro_points[i].active = false;
        fprintf(stderr, "[GH] intro point %zu lost, will re-establish\n", i);
    }

    // Rebuild intro points if we lost too many.
    size_t active_count = 0;
    for (size_t i = 0; i < gh->intro_count; i++)
        if (gh->intro_points[i].active) active_count++;

    if (active_count < (gh->intro_count + 1) / 2) {
        // Lost majority of intro points — re-establish full set.
        for (size_t i = 0; i < gh->intro_count; i++) {
            if (gh->intro_points[i].recv_ctx) {
                free(gh->intro_points[i].recv_ctx);
                gh->intro_points[i].recv_ctx = NULL;
            }
            gh->intro_points[i].active = false;
            gh->intro_points[i].conn = NULL;
        }
        gh->intro_count = 0;
        establish_intro_points(gh);

        // Re-sign and re-publish descriptor with fresh expiry.
        gh->descriptor.created_at = now;
        gh->descriptor.expires_at = now + TRELLIS_GH_DESCRIPTOR_TTL_MS;
        trellis_descriptor_sign(&gh->descriptor, &gh->service_id);

        uint8_t desc_key[32];
        if (trellis_bloom_addr_descriptor_key(&gh->service_id.fingerprint,
                                              desc_key) == TRELLIS_OK) {
            trellis_buf_t desc_buf = {0};
            if (trellis_descriptor_serialize(&gh->descriptor, &desc_buf) == TRELLIS_OK) {
                trellis_dht_store_redundant(gh->dht, desc_key, sizeof(desc_key),
                                            desc_buf.data, desc_buf.len,
                                            &gh->service_id.fingerprint,
                                            NULL, NULL);
                trellis_buf_free(&desc_buf);
            }
        }
    }

    // Refresh descriptor ~30 min before expiry.
    if (gh->descriptor.expires_at > now &&
        gh->descriptor.expires_at - now < 30ULL * 60 * 1000) {
        gh->descriptor.created_at = now;
        gh->descriptor.expires_at = now + TRELLIS_GH_DESCRIPTOR_TTL_MS;
        trellis_descriptor_sign(&gh->descriptor, &gh->service_id);

        uint8_t desc_key[32];
        if (trellis_bloom_addr_descriptor_key(&gh->service_id.fingerprint,
                                              desc_key) == TRELLIS_OK) {
            trellis_buf_t desc_buf = {0};
            if (trellis_descriptor_serialize(&gh->descriptor, &desc_buf) == TRELLIS_OK) {
                trellis_dht_store_redundant(gh->dht, desc_key, sizeof(desc_key),
                                            desc_buf.data, desc_buf.len,
                                            &gh->service_id.fingerprint,
                                            NULL, NULL);
                trellis_buf_free(&desc_buf);
            }
        }
    }
}

static trellis_err_t do_greenhouse_start(trellis_greenhouse_t *gh,
                                          trellis_greenhouse_conn_cb cb,
                                          void *ctx, uv_loop_t *loop);

trellis_err_t trellis_greenhouse_start_with_loop(trellis_greenhouse_t *gh,
                                                  trellis_greenhouse_conn_cb cb,
                                                  void *ctx, uv_loop_t *loop)
{
    return do_greenhouse_start(gh, cb, ctx, loop);
}

trellis_err_t trellis_greenhouse_start(trellis_greenhouse_t *gh,
                                        trellis_greenhouse_conn_cb cb, void *ctx)
{
    return do_greenhouse_start(gh, cb, ctx, NULL);
}

static trellis_err_t do_greenhouse_start(trellis_greenhouse_t *gh,
                                          trellis_greenhouse_conn_cb cb,
                                          void *ctx, uv_loop_t *loop)
{
    if (!gh || !cb)
        return TRELLIS_ERR_INVALID_ARG;

    if (gh->running)
        return TRELLIS_ERR_ALREADY_EXISTS;

    if (gh->type == TRELLIS_GH_FEDERATED &&
        gh->federation.peer_count == 0) {
        return TRELLIS_ERR_INVALID_ARG;
    }

    gh->conn_cb = cb;
    gh->conn_ctx = ctx;

    // Build service descriptor.
    memset(&gh->descriptor, 0, sizeof(gh->descriptor));
    trellis_identity_public_from(&gh->service_id,
                                 &gh->descriptor.service_identity);
    gh->descriptor.protocol_version = 1;
    gh->descriptor.created_at = trellis_now_ms();
    gh->descriptor.expires_at = gh->descriptor.created_at + TRELLIS_GH_DESCRIPTOR_TTL_MS;

    /* Establish introduction points via DHT-selected relays.
     * Federated mode: each node establishes its own subset of intro points.
     * The shared service identity means any node can accept the introduction
     * and perform the rendezvous handshake. */
    trellis_err_t err = establish_intro_points(gh);
    if (err != TRELLIS_OK)
        return err;

    err = trellis_descriptor_sign(&gh->descriptor, &gh->service_id);
    if (err != TRELLIS_OK) {
        free(gh->descriptor.intro_points);
        gh->descriptor.intro_points = NULL;
        return err;
    }

    // Publish descriptor to DHT.
    uint8_t desc_key[32];
    err = trellis_bloom_addr_descriptor_key(&gh->service_id.fingerprint,
                                            desc_key);
    if (err != TRELLIS_OK)
        return err;

    trellis_buf_t desc_buf = {0};
    err = trellis_descriptor_serialize(&gh->descriptor, &desc_buf);
    if (err != TRELLIS_OK)
        return err;

    err = trellis_dht_store_redundant(gh->dht, desc_key, sizeof(desc_key),
                                       desc_buf.data, desc_buf.len,
                                       &gh->service_id.fingerprint,
                                       NULL, NULL);
    trellis_buf_free(&desc_buf);
    if (err != TRELLIS_OK && err != TRELLIS_ERR_RATE_LIMITED)
        return err;

    /* Federated mode: also publish this node's intro points under a
     * per-federation-peer descriptor key so other peers can merge them.
     * Key = SHAKE-256("bloom-federation-v1" || service_fp || peer_idx). */
    if (gh->type == TRELLIS_GH_FEDERATED &&
        gh->federation.local_share_idx > 0) {
        uint8_t fed_ikm[TRELLIS_FINGERPRINT_LEN + 1];
        memcpy(fed_ikm, gh->service_id.fingerprint.bytes, TRELLIS_FINGERPRINT_LEN);
        fed_ikm[TRELLIS_FINGERPRINT_LEN] = (uint8_t)gh->federation.local_share_idx;
        static const uint8_t fed_info[] = "bloom-federation-v1";
        uint8_t fed_key[32];
        trellis_hkdf_shake256(fed_ikm, sizeof(fed_ikm),
                              NULL, 0,
                              fed_info, sizeof(fed_info) - 1,
                              fed_key, 32);

        trellis_buf_t fed_buf = {0};
        trellis_descriptor_serialize(&gh->descriptor, &fed_buf);
        if (fed_buf.data) {
            trellis_dht_store(gh->dht, fed_key, 32,
                              fed_buf.data, fed_buf.len, NULL, NULL);
            trellis_buf_free(&fed_buf);
        }
        sodium_memzero(fed_key, sizeof(fed_key));
        sodium_memzero(fed_ikm, sizeof(fed_ikm));
    }

    // Start long-lived intro circuit keepalive timer if loop is provided.
    if (loop) {
        gh->loop = loop;
        uv_timer_init(loop, &gh->intro_keepalive);
        gh->intro_keepalive.data = gh;
        uv_timer_start(&gh->intro_keepalive, intro_keepalive_cb,
                       TRELLIS_GH_INTRO_KEEPALIVE_MS,
                       TRELLIS_GH_INTRO_KEEPALIVE_MS);
        gh->intro_timer_active = true;
    }

    gh->running = true;
    return TRELLIS_OK;
}

trellis_err_t trellis_greenhouse_stop(trellis_greenhouse_t *gh)
{
    if (!gh)
        return TRELLIS_ERR_INVALID_ARG;

    if (!gh->running)
        return TRELLIS_OK;

    // Stop keepalive timer.
    if (gh->intro_timer_active) {
        uv_timer_stop(&gh->intro_keepalive);
        gh->intro_timer_active = false;
    }

    // Remove all descriptor replicas from DHT (best-effort)
    uint8_t desc_key[32];
    trellis_bloom_addr_descriptor_key(&gh->service_id.fingerprint, desc_key);
    trellis_dht_store_redundant(gh->dht, desc_key, sizeof(desc_key),
                                 NULL, 0, &gh->service_id.fingerprint,
                                 NULL, NULL);

    for (size_t i = 0; i < gh->intro_count; i++) {
        if (gh->intro_points[i].conn) {
            trellis_conn_close(gh->intro_points[i].conn, NULL, NULL);
            gh->intro_points[i].conn = NULL;
        }
        if (gh->intro_points[i].recv_ctx) {
            free(gh->intro_points[i].recv_ctx);
            gh->intro_points[i].recv_ctx = NULL;
        }
        gh->intro_points[i].active = false;
    }
    gh->intro_count = 0;

    gh->running = false;
    gh->conn_cb = NULL;
    gh->conn_ctx = NULL;
    return TRELLIS_OK;
}

typedef struct {
    trellis_connect_cb   cb;
    void                *ctx;
    trellis_dht_t       *dht;
    trellis_router_t    *router;     /* optional: enables cross-node rendezvous */
    trellis_rendezvous_t *rv;
    trellis_surb_t      *surb;      /* client-side SURB for decrypting reply */
} greenhouse_connect_ctx_t;

static void greenhouse_connect_ctx_free(greenhouse_connect_ctx_t *gc)
{
    if (!gc) return;
    if (gc->surb) trellis_surb_free(gc->surb);
    free(gc);
}

static void on_descriptor_found(const uint8_t *value, size_t len,
                                trellis_err_t err, void *ctx)
{
    greenhouse_connect_ctx_t *gc = ctx;
    if (err != TRELLIS_OK || !value || len == 0) {
        if (gc->cb)
            gc->cb(NULL, err != TRELLIS_OK ? err : TRELLIS_ERR_NOT_FOUND,
                   gc->ctx);
        greenhouse_connect_ctx_free(gc);
        return;
    }

    trellis_service_descriptor_t desc;
    err = trellis_descriptor_deserialize(value, len, &desc);
    if (err != TRELLIS_OK) {
        if (gc->cb)
            gc->cb(NULL, err, gc->ctx);
        greenhouse_connect_ctx_free(gc);
        return;
    }

    if (trellis_descriptor_is_expired(&desc)) {
        trellis_descriptor_free(&desc);
        if (gc->cb)
            gc->cb(NULL, TRELLIS_ERR_DESCRIPTOR, gc->ctx);
        greenhouse_connect_ctx_free(gc);
        return;
    }

    err = trellis_descriptor_verify(&desc);
    if (err != TRELLIS_OK) {
        trellis_descriptor_free(&desc);
        if (gc->cb)
            gc->cb(NULL, err, gc->ctx);
        greenhouse_connect_ctx_free(gc);
        return;
    }

    /*
     * Rendezvous protocol (RFC Bloom §9.4):
     *
     *  CLIENT                INTRO RELAY              SERVICE
     *    |---INTRODUCE1(cookie,rv_relay)---->|           |
     *    |                   |---INTRODUCE2(cookie)----->|
     *    |<---(async)---service joins rendezvous relay---|
     *    |<============RENDEZVOUS_JOINED=channel=========|
     *
     *  1. Pick a rendezvous relay from the DHT (or use intro node itself)
     *  2. Create a rendezvous session with a random cookie
     *  3. Build INTRODUCE1 payload:
     *       [4]  magic "INT1"
     *       [1]  version = 1
     *       [32] rendezvous relay fingerprint
     *       [32] rendezvous cookie
     *       [TRELLIS_ML_KEM_1024_PK_LEN] client half-key (ML-KEM-1024 pk)
     *  4. Onion-wrap for the intro point and send via router
     *  5. The intro relay decrypts and forwards INTRODUCE2 to the service
     *  6. Service builds a circuit to the rendezvous relay and sends
     *     RENDEZVOUS_JOIN with the same cookie
     *  7. Client polls / awaits the RENDEZVOUS_CONN message
     *  8. Bridge() creates the bidirectional tunnel
     *
     * When no router is configured (in-process / test mode), skip steps 3-6
     * and directly bridge the connection pair.
     */
    if (desc.intro_point_count == 0) {
        trellis_descriptor_free(&desc);
        if (gc->cb)
            gc->cb(NULL, TRELLIS_ERR_NOT_FOUND, gc->ctx);
        greenhouse_connect_ctx_free(gc);
        return;
    }

    // Select first intro point.
    trellis_intro_point_t *ip = &desc.intro_points[0];

    // Create rendezvous session and generate a cookie.
    trellis_rendezvous_t *rv = trellis_rendezvous_new();
    if (!rv) {
        trellis_descriptor_free(&desc);
        if (gc->cb)
            gc->cb(NULL, TRELLIS_ERR_NOMEM, gc->ctx);
        greenhouse_connect_ctx_free(gc);
        return;
    }

    uint8_t cookie[TRELLIS_RENDEZVOUS_COOKIE_LEN];
    trellis_rendezvous_establish(rv, cookie);

    // Build onion circuit to the intro point.
    trellis_onion_circuit_t *circuit = trellis_onion_new(1);
    if (circuit) {
        trellis_onion_add_hop(circuit, &ip->node_fingerprint,
                              desc.service_identity.x25519_pk,
                              ip->onion_key);
        rv->client_circuit = circuit;
    }

    if (gc->router && circuit) {
        /*
         * Cross-node path: send INTRODUCE1 to the intro relay.
         * Pick a rendezvous relay (use first available DHT peer, or the
         * intro node itself if no other peers are available).
         */
        trellis_peer_info_t rv_relay_peer = {0};
        trellis_fingerprint_t rv_relay_fp = ip->node_fingerprint; /* default */
        trellis_peer_info_t peers_buf[4];
        size_t npeers = trellis_dht_get_peers(gc->dht, peers_buf, 4);
        for (size_t pi = 0; pi < npeers; pi++) {
            if (!trellis_fingerprint_eq(&peers_buf[pi].fingerprint,
                                        &ip->node_fingerprint) &&
                !peers_buf[pi].quarantined &&
                peers_buf[pi].greenhouse_capable) {
                rv_relay_fp = peers_buf[pi].fingerprint;
                rv_relay_peer = peers_buf[pi];
                break;
            }
        }
        /*
         * If a distinct rendezvous relay was found, add it as a second hop on
         * the onion circuit so the intro relay cannot correlate the two roles.
         */
        if (circuit && !trellis_fingerprint_eq(&rv_relay_fp,
                                                &ip->node_fingerprint)) {
            trellis_onion_add_hop(circuit, &rv_relay_peer.fingerprint,
                                  rv_relay_peer.identity.x25519_pk,
                                  rv_relay_peer.identity.ml_kem_pk);
        }

        /*
         * Build INTRODUCE1 payload v2 (encrypted for the service via the
         * intro node's onion layer):
         *   [4]  magic "INT1"
         *   [1]  version (2)
         *   [32] rendezvous relay fingerprint
         *   [RENDEZVOUS_COOKIE_LEN] rendezvous cookie
         *   [4]  surb_len (BE)
         *   [N]  serialized SURB
         */
        static const uint8_t INT1_MAGIC[4] = {'I','N','T','1'};
        size_t intro1_base = 4 + 1 + 32 + TRELLIS_RENDEZVOUS_COOKIE_LEN;

        // Build a SURB for unlinkable service replies.
        trellis_surb_t *client_surb = NULL;
        trellis_buf_t surb_wire = {0};
        {
            trellis_peer_info_t surb_peers[3];
            size_t surb_npeers = trellis_dht_get_peers(gc->dht, surb_peers, 3);
            if (surb_npeers >= 1) {
                const trellis_peer_info_t *surb_hop_ptrs[3];
                for (size_t si = 0; si < surb_npeers; si++)
                    surb_hop_ptrs[si] = &surb_peers[si];
                client_surb = trellis_surb_build(
                    &rv_relay_fp, &rv_relay_peer.identity,
                    surb_hop_ptrs, surb_npeers, NULL, 0);
                if (client_surb)
                    trellis_surb_serialize(client_surb, &surb_wire);
            }
        }

        size_t intro1_len = intro1_base + 4 + surb_wire.len;
        uint8_t *intro1_payload = malloc(intro1_len);
        if (!intro1_payload) {
            if (client_surb) trellis_surb_free(client_surb);
            trellis_buf_free(&surb_wire);
            trellis_rendezvous_free(rv);
            trellis_descriptor_free(&desc);
            if (gc->cb) gc->cb(NULL, TRELLIS_ERR_NOMEM, gc->ctx);
            greenhouse_connect_ctx_free(gc);
            return;
        }

        size_t poff = 0;
        memcpy(intro1_payload + poff, INT1_MAGIC, 4); poff += 4;
        intro1_payload[poff++] = 2;
        memcpy(intro1_payload + poff, rv_relay_fp.bytes, 32); poff += 32;
        memcpy(intro1_payload + poff, cookie, TRELLIS_RENDEZVOUS_COOKIE_LEN);
        poff += TRELLIS_RENDEZVOUS_COOKIE_LEN;

        // SURB length + data.
        uint32_t surb_len_be = (uint32_t)surb_wire.len;
        intro1_payload[poff++] = (uint8_t)(surb_len_be >> 24);
        intro1_payload[poff++] = (uint8_t)(surb_len_be >> 16);
        intro1_payload[poff++] = (uint8_t)(surb_len_be >> 8);
        intro1_payload[poff++] = (uint8_t)(surb_len_be);
        if (surb_wire.len > 0) {
            memcpy(intro1_payload + poff, surb_wire.data, surb_wire.len);
            poff += surb_wire.len;
        }
        trellis_buf_free(&surb_wire);
        gc->surb = client_surb;

        trellis_buf_t wrapped = {0};
        err = trellis_onion_wrap(circuit, intro1_payload, poff, &wrapped);
        free(intro1_payload);
        if (err == TRELLIS_OK && wrapped.data) {
            trellis_router_send(gc->router, &ip->node_fingerprint,
                                wrapped.data, wrapped.len, NULL, NULL);
            trellis_buf_free(&wrapped);
            fprintf(stderr,
                "[GH] INTRODUCE1 sent to intro relay %02x%02x… "
                "(rv relay %02x%02x…)\n",
                ip->node_fingerprint.bytes[0], ip->node_fingerprint.bytes[1],
                rv_relay_fp.bytes[0], rv_relay_fp.bytes[1]);
        } else {
            fprintf(stderr, "[GH] INTRODUCE1 onion wrap failed (%d)\n", err);
        }

        /*
         * Asynchronous path: the tunnel conn is returned to the caller once
         * the service responds and the rendezvous relay delivers
         * RENDEZVOUS_CONN.  For now we create the pipe pair and return the
         * client end; the service end will be activated by trellis_rendezvous_bridge()
         * when the async join arrives.
         */
        trellis_conn_t *client_end = NULL;
        trellis_conn_t *service_end = NULL;
        err = trellis_rendezvous_conn_pair(&client_end, &service_end);
        if (err != TRELLIS_OK) {
            trellis_rendezvous_free(rv);
            trellis_descriptor_free(&desc);
            if (gc->cb)
                gc->cb(NULL, err, gc->ctx);
            greenhouse_connect_ctx_free(gc);
            return;
        }
        rv->client_conn  = client_end;
        rv->service_conn = service_end;
        gc->rv = rv;

        trellis_descriptor_free(&desc);
        if (gc->cb)
            gc->cb(client_end, TRELLIS_OK, gc->ctx);
        greenhouse_connect_ctx_free(gc);
        return;
    }

    /*
     * In-process / test fallback: directly bridge since both client and
     * service share the same DHT and rendezvous state.
     */
    trellis_conn_t *client_end = NULL;
    trellis_conn_t *service_end = NULL;
    err = trellis_rendezvous_conn_pair(&client_end, &service_end);
    if (err != TRELLIS_OK) {
        trellis_rendezvous_free(rv);
        trellis_descriptor_free(&desc);
        if (gc->cb)
            gc->cb(NULL, err, gc->ctx);
        greenhouse_connect_ctx_free(gc);
        return;
    }

    rv->client_conn  = client_end;
    rv->service_conn = service_end;
    gc->rv = rv;

    trellis_rendezvous_join(rv, cookie);
    trellis_rendezvous_bridge(rv);

    fprintf(stderr, "[GH] rendezvous bridged (in-process) for intro %02x%02x…\n",
            ip->node_fingerprint.bytes[0], ip->node_fingerprint.bytes[1]);

    trellis_descriptor_free(&desc);

    if (gc->cb)
        gc->cb(client_end, TRELLIS_OK, gc->ctx);
    greenhouse_connect_ctx_free(gc);
}

trellis_err_t trellis_greenhouse_connect(trellis_dht_t *dht,
                                          const trellis_bloom_addr_t *addr,
                                          trellis_connect_cb cb, void *ctx)
{
    return trellis_greenhouse_connect_with_router(dht, NULL, addr, cb, ctx);
}

trellis_err_t trellis_greenhouse_connect_with_router(
                                          trellis_dht_t *dht,
                                          trellis_router_t *router,
                                          const trellis_bloom_addr_t *addr,
                                          trellis_connect_cb cb, void *ctx)
{
    if (!dht || !addr)
        return TRELLIS_ERR_INVALID_ARG;

    uint8_t desc_key[32];
    trellis_err_t err = trellis_bloom_addr_descriptor_key(&addr->fingerprint,
                                                          desc_key);
    if (err != TRELLIS_OK)
        return err;

    greenhouse_connect_ctx_t *gc = calloc(1, sizeof(*gc));
    if (!gc)
        return TRELLIS_ERR_NOMEM;
    gc->cb     = cb;
    gc->ctx    = ctx;
    gc->dht    = dht;
    gc->router = router;   /* NULL → in-process fallback; set → cross-node INTRODUCE1 */

    /* PIR + S/Kademlia: fire real + cover queries across disjoint paths so
     * neither queried DHT nodes nor any single path learns the real key. */
    return trellis_pir_find_value_skad(dht, desc_key, sizeof(desc_key),
                                        on_descriptor_found, gc);
}

// --- Cross-grove Greenhouse connect ---
typedef struct {
    trellis_connect_cb    cb;
    void                 *ctx;
    trellis_dht_t        *dht;
    trellis_router_t     *router;
    trellis_canopy_t     *canopy;
    trellis_bloom_addr_t  addr;
    trellis_grove_id_t    target_grove;
    uint32_t              circuit_id;
    uint32_t              cdht_request_id;
} cross_grove_connect_ctx_t;

#define CDHT_MAGIC_RSP "CDFR"
#define CDHT_RSP_HDR   (4 + 4 + 1 + 4)

static void cross_grove_cleanup(cross_grove_connect_ctx_t *cgc,
                                trellis_err_t err)
{
    if (cgc->circuit_id && cgc->canopy) {
        trellis_canopy_circuit_set_app_recv(cgc->canopy, cgc->circuit_id,
                                            NULL, NULL);
        trellis_canopy_circuit_destroy(cgc->canopy, cgc->circuit_id);
    }
    if (cgc->cb)
        cgc->cb(NULL, err, cgc->ctx);
    free(cgc);
}

static void on_cross_grove_descriptor_found(const uint8_t *value, size_t len,
                                             trellis_err_t err, void *ctx)
{
    cross_grove_connect_ctx_t *cgc = (cross_grove_connect_ctx_t *)ctx;
    if (err != TRELLIS_OK || !value) {
        cross_grove_cleanup(cgc, err != TRELLIS_OK ? err : TRELLIS_ERR_NOT_FOUND);
        return;
    }

    trellis_service_descriptor_t desc;
    err = trellis_descriptor_deserialize(value, len, &desc);
    if (err != TRELLIS_OK) {
        cross_grove_cleanup(cgc, err);
        return;
    }

    err = trellis_descriptor_verify(&desc);
    if (err != TRELLIS_OK) {
        trellis_descriptor_free(&desc);
        cross_grove_cleanup(cgc, err);
        return;
    }

    if (trellis_descriptor_is_expired(&desc)) {
        trellis_descriptor_free(&desc);
        cross_grove_cleanup(cgc, TRELLIS_ERR_DESCRIPTOR);
        return;
    }

    trellis_descriptor_free(&desc);

    /* The canopy circuit is no longer needed for descriptor fetch.
     * Destroy it before handing off to the standard connect path,
     * which will establish its own transport-layer connection. */
    if (cgc->circuit_id && cgc->canopy) {
        trellis_canopy_circuit_set_app_recv(cgc->canopy, cgc->circuit_id,
                                            NULL, NULL);
        trellis_canopy_circuit_destroy(cgc->canopy, cgc->circuit_id);
        cgc->circuit_id = 0;
    }

    trellis_err_t conn_err = trellis_greenhouse_connect_with_router(
        cgc->dht, cgc->router, &cgc->addr, cgc->cb, cgc->ctx);
    if (conn_err != TRELLIS_OK) {
        cross_grove_cleanup(cgc, conn_err);
        return;
    }
    free(cgc);
}

static void on_cross_grove_cdht_recv(uint32_t circuit_id,
                                     const uint8_t *payload, size_t len,
                                     void *ctx)
{
    cross_grove_connect_ctx_t *cgc = (cross_grove_connect_ctx_t *)ctx;
    if (!cgc || circuit_id != cgc->circuit_id || !payload ||
        len < CDHT_RSP_HDR)
        return;

    if (memcmp(payload, CDHT_MAGIC_RSP, 4) != 0)
        return;

    uint32_t rid = ((uint32_t)payload[4] << 24) | ((uint32_t)payload[5] << 16) |
                   ((uint32_t)payload[6] << 8) | (uint32_t)payload[7];
    if (rid != cgc->cdht_request_id)
        return;

    uint8_t status = payload[8];
    uint32_t vlen = ((uint32_t)payload[9] << 24) | ((uint32_t)payload[10] << 16) |
                    ((uint32_t)payload[11] << 8) | (uint32_t)payload[12];
    if (CDHT_RSP_HDR + vlen > len) {
        cross_grove_cleanup(cgc, TRELLIS_ERR_MSG_FORMAT);
        return;
    }

    trellis_canopy_circuit_set_app_recv(cgc->canopy, cgc->circuit_id,
                                        NULL, NULL);

    if (status != 0 || vlen == 0) {
        cross_grove_cleanup(cgc, status == 1 ? TRELLIS_ERR_NOT_FOUND
                                              : TRELLIS_ERR_CANOPY);
        return;
    }

    on_cross_grove_descriptor_found(payload + CDHT_RSP_HDR, vlen,
                                    TRELLIS_OK, cgc);
}

static void on_cross_grove_circuit(trellis_err_t err, uint32_t circuit_id,
                                    void *ctx)
{
    cross_grove_connect_ctx_t *cgc = (cross_grove_connect_ctx_t *)ctx;
    if (err != TRELLIS_OK) {
        cross_grove_cleanup(cgc, err);
        return;
    }
    cgc->circuit_id = circuit_id;

    /* Derive the raw descriptor key — do NOT pre-scope it here.
     * The terminal bridge's DHT (bound to grove γ) applies
     * grove_scope_key transparently inside trellis_dht_find_value_skad. */
    uint8_t desc_key[32];
    err = trellis_bloom_addr_descriptor_key(&cgc->addr.fingerprint, desc_key);
    if (err != TRELLIS_OK) {
        cross_grove_cleanup(cgc, err);
        return;
    }

    randombytes_buf(&cgc->cdht_request_id, sizeof(cgc->cdht_request_id));
    if (cgc->cdht_request_id == 0)
        cgc->cdht_request_id = 1;

    trellis_err_t reg_err = trellis_canopy_circuit_set_app_recv(
        cgc->canopy, circuit_id, on_cross_grove_cdht_recv, cgc);
    if (reg_err != TRELLIS_OK) {
        cross_grove_cleanup(cgc, reg_err);
        return;
    }

    // CDFN || request_id(4) || key_len(2 BE) || raw_descriptor_key.
    uint8_t req[4 + 4 + 2 + 32];
    memcpy(req, "CDFN", 4);
    req[4] = (uint8_t)(cgc->cdht_request_id >> 24);
    req[5] = (uint8_t)(cgc->cdht_request_id >> 16);
    req[6] = (uint8_t)(cgc->cdht_request_id >> 8);
    req[7] = (uint8_t)(cgc->cdht_request_id);
    req[8] = 0;
    req[9] = 32;
    memcpy(req + 10, desc_key, 32);

    trellis_err_t send_err = trellis_canopy_circuit_send(
        cgc->canopy, circuit_id, req, sizeof(req));
    if (send_err != TRELLIS_OK) {
        cross_grove_cleanup(cgc, send_err);
        return;
    }
}

trellis_err_t trellis_greenhouse_connect_cross_grove(
                                          trellis_dht_t *dht,
                                          trellis_router_t *router,
                                          trellis_canopy_t *canopy,
                                          const trellis_bloom_addr_t *addr,
                                          trellis_connect_cb cb, void *ctx)
{
    if (!dht || !canopy || !addr || !addr->has_grove)
        return TRELLIS_ERR_INVALID_ARG;

    // Resolve grove-id-short to full grove_id.
    trellis_grove_id_t target_grove;
    char short_id[17];
    size_t short_len = 16;

    /* The addr->grove_id_short is raw bytes (10 bytes); we need the base32
     * representation for grove_id_from_short.  For simplicity, construct a
     * temporary grove_id using the short bytes. */
    trellis_grove_id_t candidate = {0};
    memcpy(candidate.bytes, addr->grove_id_short, 10);
    trellis_err_t err = trellis_grove_id_short(&candidate, short_id, sizeof(short_id));
    if (err != TRELLIS_OK)
        return err;

    err = trellis_grove_id_from_short(short_id, canopy, &target_grove);
    if (err != TRELLIS_OK)
        return err;

    // Verify the target grove is reachable.
    if (!trellis_canopy_grove_reachable(canopy, &target_grove))
        return TRELLIS_ERR_GROVE_UNKNOWN;

    trellis_canopy_route_t route;
    err = trellis_canopy_route(canopy, &target_grove, &route);
    if (err != TRELLIS_OK)
        return err;

    cross_grove_connect_ctx_t *cgc = calloc(1, sizeof(*cgc));
    if (!cgc)
        return TRELLIS_ERR_NOMEM;

    cgc->cb = cb;
    cgc->ctx = ctx;
    cgc->dht = dht;
    cgc->router = router;
    cgc->canopy = canopy;
    cgc->addr = *addr;
    cgc->target_grove = target_grove;

    /* Target grove is local to this bridge: PIR on γ's DHT with raw key.
     * DHT layer applies grove namespace scoping transparently (dht.c). */
    if (route.path_len == 0) {
        trellis_bridge_t *bridge = trellis_canopy_bridge(canopy);
        if (!bridge) {
            free(cgc);
            return TRELLIS_ERR_INVALID_STATE;
        }

        size_t gslot = 0;
        bool have_slot = false;
        for (size_t i = 0; i < TRELLIS_BRIDGE_MAX_GROVES; i++) {
            const trellis_grove_id_t *g =
                trellis_bridge_grove_at(bridge, i);
            if (g && trellis_grove_id_eq(g, &target_grove)) {
                gslot = i;
                have_slot = true;
                break;
            }
        }
        if (!have_slot) {
            free(cgc);
            return TRELLIS_ERR_GROVE_UNKNOWN;
        }

        const trellis_grove_policy_t *lpol =
            trellis_bridge_grove_policy(bridge, gslot);
        if (lpol && !lpol->descriptors_visible) {
            free(cgc);
            return TRELLIS_ERR_GROVE_POLICY;
        }

        trellis_dht_t *grove_dht = trellis_bridge_grove_dht(bridge, gslot);
        if (!grove_dht) {
            free(cgc);
            return TRELLIS_ERR_INVALID_STATE;
        }

        uint8_t desc_key[32];
        err = trellis_bloom_addr_descriptor_key(&addr->fingerprint, desc_key);
        if (err != TRELLIS_OK) {
            free(cgc);
            return err;
        }
        return trellis_pir_find_value_skad(grove_dht, desc_key, sizeof(desc_key),
                                           on_cross_grove_descriptor_found, cgc);
    }

    /* Remote grove: circuit terminates at the egress bridge into γ (first hop
     * from this node along the selected path; see whitepaper §18.5 / §10.9). */
    return trellis_canopy_circuit_create(canopy, &target_grove,
                                          &route.next_bridge_fp,
                                          on_cross_grove_circuit, cgc);
}

// --- Global service registry ---
#define GH_REGISTRY_MAX 32

typedef struct {
    trellis_fingerprint_t service_fp;
    trellis_greenhouse_t *gh;
    bool                  occupied;
} gh_registry_entry_t;

static gh_registry_entry_t g_gh_registry[GH_REGISTRY_MAX];

void trellis_greenhouse_register(trellis_greenhouse_t *gh)
{
    if (!gh) return;
    for (size_t i = 0; i < GH_REGISTRY_MAX; i++) {
        if (!g_gh_registry[i].occupied) {
            g_gh_registry[i].service_fp = gh->service_id.fingerprint;
            g_gh_registry[i].gh = gh;
            g_gh_registry[i].occupied = true;
            return;
        }
    }
}

void trellis_greenhouse_unregister(trellis_greenhouse_t *gh)
{
    if (!gh) return;
    for (size_t i = 0; i < GH_REGISTRY_MAX; i++) {
        if (g_gh_registry[i].occupied && g_gh_registry[i].gh == gh) {
            g_gh_registry[i].occupied = false;
            g_gh_registry[i].gh = NULL;
            return;
        }
    }
}

static trellis_greenhouse_t *gh_registry_lookup(
    const trellis_fingerprint_t *fp)
{
    for (size_t i = 0; i < GH_REGISTRY_MAX; i++) {
        if (g_gh_registry[i].occupied &&
            trellis_fingerprint_eq(&g_gh_registry[i].service_fp, fp))
            return g_gh_registry[i].gh;
    }
    return NULL;
}

// --- Relay-side intro point table ---
#define INTRO_TABLE_MAX 64

typedef struct {
    trellis_fingerprint_t service_fp;
    trellis_fingerprint_t relay_fp;   /* peer that registered this intro point */
    bool                  occupied;
} intro_table_entry_t;

static intro_table_entry_t g_intro_table[INTRO_TABLE_MAX];

static void intro_table_register(const trellis_fingerprint_t *service_fp,
                                 const trellis_fingerprint_t *relay_fp)
{
    for (size_t i = 0; i < INTRO_TABLE_MAX; i++) {
        if (!g_intro_table[i].occupied) {
            g_intro_table[i].service_fp = *service_fp;
            g_intro_table[i].relay_fp = *relay_fp;
            g_intro_table[i].occupied = true;
            return;
        }
    }
}

static const trellis_fingerprint_t *intro_table_lookup(
    const trellis_fingerprint_t *service_fp)
{
    for (size_t i = 0; i < INTRO_TABLE_MAX; i++) {
        if (g_intro_table[i].occupied &&
            trellis_fingerprint_eq(&g_intro_table[i].service_fp, service_fp))
            return &g_intro_table[i].relay_fp;
    }
    return NULL;
}

// --- Pending rendezvous table (relay-side) ---
#define RV_PENDING_MAX 64

typedef struct {
    uint8_t               cookie[TRELLIS_RENDEZVOUS_COOKIE_LEN];
    trellis_rendezvous_t *rv;
    trellis_surb_t       *surb;    /* SURB from client for unlinkable reply */
    bool                  occupied;
} rv_pending_entry_t;

static rv_pending_entry_t g_rv_pending[RV_PENDING_MAX];

static void rv_pending_insert(const uint8_t cookie[TRELLIS_RENDEZVOUS_COOKIE_LEN],
                              trellis_rendezvous_t *rv)
{
    for (size_t i = 0; i < RV_PENDING_MAX; i++) {
        if (!g_rv_pending[i].occupied) {
            memcpy(g_rv_pending[i].cookie, cookie,
                   TRELLIS_RENDEZVOUS_COOKIE_LEN);
            g_rv_pending[i].rv = rv;
            g_rv_pending[i].occupied = true;
            return;
        }
    }
}

static trellis_rendezvous_t *rv_pending_lookup(
    const uint8_t cookie[TRELLIS_RENDEZVOUS_COOKIE_LEN],
    trellis_surb_t **surb_out)
{
    for (size_t i = 0; i < RV_PENDING_MAX; i++) {
        if (g_rv_pending[i].occupied &&
            sodium_memcmp(g_rv_pending[i].cookie, cookie,
                          TRELLIS_RENDEZVOUS_COOKIE_LEN) == 0) {
            trellis_rendezvous_t *rv = g_rv_pending[i].rv;
            trellis_surb_t *surb = g_rv_pending[i].surb;
            g_rv_pending[i].surb = NULL;
            g_rv_pending[i].occupied = false;
            if (surb_out)
                *surb_out = surb;
            else if (surb)
                trellis_surb_free(surb);
            return rv;
        }
    }
    return NULL;
}

// --- Protocol message dispatcher ---

// Find any registered greenhouse with a non-NULL router for error sending.
static trellis_router_t *gh_find_any_router(void)
{
    for (size_t i = 0; i < GH_REGISTRY_MAX; i++) {
        if (g_gh_registry[i].occupied && g_gh_registry[i].gh &&
            g_gh_registry[i].gh->router)
            return g_gh_registry[i].gh->router;
    }
    return NULL;
}

/*
 * Send a TRELLIS_MSG_GH_ERROR back to msg->source.
 * Payload: [4] error_code (BE) + [32] cookie (or zeros if NULL).
 */
static void send_gh_error(const trellis_message_t *msg,
                          trellis_gh_error_code_t code,
                          const uint8_t *cookie)
{
    trellis_router_t *router = gh_find_any_router();
    if (!router) return;

    uint8_t payload[4 + TRELLIS_RENDEZVOUS_COOKIE_LEN];
    payload[0] = (uint8_t)(code >> 24);
    payload[1] = (uint8_t)(code >> 16);
    payload[2] = (uint8_t)(code >> 8);
    payload[3] = (uint8_t)(code);
    if (cookie)
        memcpy(payload + 4, cookie, TRELLIS_RENDEZVOUS_COOKIE_LEN);
    else
        memset(payload + 4, 0, TRELLIS_RENDEZVOUS_COOKIE_LEN);

    trellis_message_t err_msg = {0};
    err_msg.version     = TRELLIS_PROTOCOL_VERSION;
    err_msg.type        = TRELLIS_MSG_GH_ERROR;
    err_msg.timestamp   = trellis_now_ms();
    err_msg.target      = msg->source;
    err_msg.payload     = payload;
    err_msg.payload_len = sizeof(payload);

    trellis_buf_t wire = {0};
    if (trellis_message_serialize(&err_msg, &wire) == TRELLIS_OK && wire.data) {
        trellis_router_send(router, &msg->source,
                            wire.data, wire.len, NULL, NULL);
        trellis_buf_free(&wire);
    }
}

static void dispatch_gh_introduce(const trellis_message_t *msg)
{
    /* INTRODUCE2 payload from intro relay to service:
     *   [32] rendezvous relay fingerprint
     *   [RENDEZVOUS_COOKIE_LEN] rendezvous cookie
     *   [4]  surb_len (BE) -- optional, v2+
     *   [N]  serialized SURB -- optional, v2+ */
    if (!msg->payload || msg->payload_len < 32 + TRELLIS_RENDEZVOUS_COOKIE_LEN) {
        send_gh_error(msg, TRELLIS_GH_ERR_BAD_PAYLOAD, NULL);
        return;
    }

    trellis_fingerprint_t rv_relay_fp;
    memcpy(rv_relay_fp.bytes, msg->payload, 32);
    const uint8_t *cookie = msg->payload + 32;

    // Parse optional SURB from v2 payload.
    trellis_surb_t *intro_surb = NULL;
    size_t surb_offset = 32 + TRELLIS_RENDEZVOUS_COOKIE_LEN;
    if (msg->payload_len >= surb_offset + 4) {
        uint32_t surb_len = ((uint32_t)msg->payload[surb_offset] << 24) |
                            ((uint32_t)msg->payload[surb_offset + 1] << 16) |
                            ((uint32_t)msg->payload[surb_offset + 2] << 8) |
                            (uint32_t)msg->payload[surb_offset + 3];
        surb_offset += 4;
        if (surb_len > 0 && surb_len <= 16384 &&
            msg->payload_len >= surb_offset + surb_len) {
            trellis_surb_deserialize(msg->payload + surb_offset,
                                     surb_len, &intro_surb);
        }
    }

    trellis_greenhouse_t *gh = gh_registry_lookup(&msg->target);
    if (!gh || !gh->running) {
        fprintf(stderr, "[GH] INTRODUCE2: no registered service for target\n");
        send_gh_error(msg, TRELLIS_GH_ERR_NO_SERVICE, NULL);
        if (intro_surb) trellis_surb_free(intro_surb);
        return;
    }

    trellis_rendezvous_t *rv = trellis_rendezvous_new();
    if (!rv) {
        if (intro_surb) trellis_surb_free(intro_surb);
        return;
    }

    trellis_rendezvous_join(rv, cookie);

    // Store in pending table with the SURB for later reply.
    rv_pending_insert(cookie, rv);
    // Attach SURB to the pending entry.
    for (size_t i = 0; i < RV_PENDING_MAX; i++) {
        if (g_rv_pending[i].occupied &&
            sodium_memcmp(g_rv_pending[i].cookie, cookie,
                          TRELLIS_RENDEZVOUS_COOKIE_LEN) == 0) {
            g_rv_pending[i].surb = intro_surb;
            intro_surb = NULL;
            break;
        }
    }
    if (intro_surb) trellis_surb_free(intro_surb);

    if (gh->router) {
        uint8_t join_payload[TRELLIS_RENDEZVOUS_COOKIE_LEN];
        memcpy(join_payload, cookie, TRELLIS_RENDEZVOUS_COOKIE_LEN);
        trellis_router_send(gh->router, &rv_relay_fp,
                            join_payload, sizeof(join_payload), NULL, NULL);
        fprintf(stderr, "[GH] RENDEZVOUS_JOIN sent to relay %02x%02x...\n",
                rv_relay_fp.bytes[0], rv_relay_fp.bytes[1]);

        // Send an acknowledgment via the SURB if one was provided.
        trellis_surb_t *ack_surb = NULL;
        for (size_t i = 0; i < RV_PENDING_MAX; i++) {
            if (g_rv_pending[i].occupied &&
                sodium_memcmp(g_rv_pending[i].cookie, cookie,
                              TRELLIS_RENDEZVOUS_COOKIE_LEN) == 0 &&
                g_rv_pending[i].surb) {
                ack_surb = g_rv_pending[i].surb;
                g_rv_pending[i].surb = NULL;
                break;
            }
        }
        if (ack_surb) {
            static const uint8_t ack[] = "GH_ACK";
            trellis_greenhouse_reply_via_surb(gh, ack_surb, ack, sizeof(ack));
            trellis_surb_free(ack_surb);
        }
    }
}

static void dispatch_gh_rendezvous_join(const trellis_message_t *msg)
{
    /* RENDEZVOUS_JOIN: service has connected to the rendezvous relay.
     * Payload: [RENDEZVOUS_COOKIE_LEN] cookie
     * The relay matches the cookie to a pending rendezvous from the
     * client and completes the bridge. */
    if (!msg->payload || msg->payload_len < TRELLIS_RENDEZVOUS_COOKIE_LEN) {
        send_gh_error(msg, TRELLIS_GH_ERR_BAD_PAYLOAD, NULL);
        return;
    }

    const uint8_t *cookie = msg->payload;
    trellis_surb_t *pending_surb = NULL;
    trellis_rendezvous_t *rv = rv_pending_lookup(cookie, &pending_surb);
    if (rv) {
        trellis_rendezvous_bridge(rv);
        if (pending_surb) trellis_surb_free(pending_surb);
        fprintf(stderr, "[GH] rendezvous bridge completed for cookie "
                "%02x%02x...\n", cookie[0], cookie[1]);
    } else {
        fprintf(stderr, "[GH] RENDEZVOUS_JOIN: no pending session for "
                "cookie %02x%02x...\n", cookie[0], cookie[1]);
        send_gh_error(msg, TRELLIS_GH_ERR_NO_SESSION, cookie);
    }
}

static void dispatch_gh_rendezvous_conn(const trellis_message_t *msg)
{
    /* RENDEZVOUS_CONN: the relay confirms that both sides are connected.
     * Payload: [RENDEZVOUS_COOKIE_LEN] cookie
     * Activates the dormant service-side pipe. */
    if (!msg->payload || msg->payload_len < TRELLIS_RENDEZVOUS_COOKIE_LEN) {
        send_gh_error(msg, TRELLIS_GH_ERR_BAD_PAYLOAD, NULL);
        return;
    }

    const uint8_t *cookie = msg->payload;
    trellis_rendezvous_t *rv = rv_pending_lookup(cookie, NULL);
    if (rv) {
        trellis_rendezvous_bridge(rv);
        fprintf(stderr, "[GH] RENDEZVOUS_CONN activated for cookie "
                "%02x%02x...\n", cookie[0], cookie[1]);
    } else {
        send_gh_error(msg, TRELLIS_GH_ERR_NO_SESSION, cookie);
    }
}

static void dispatch_gh_establish_intro(const trellis_message_t *msg)
{
    /* ESTABLISH_INTRO (from service to relay): register this relay as an
     * introduction point for the service.
     * Payload: [4] magic "ESTI" [1] version [32] service fingerprint ... */
    if (!msg->payload || msg->payload_len < 4 + 1 + 32) {
        send_gh_error(msg, TRELLIS_GH_ERR_BAD_PAYLOAD, NULL);
        return;
    }
    if (memcmp(msg->payload, "ESTI", 4) != 0) {
        send_gh_error(msg, TRELLIS_GH_ERR_BAD_PAYLOAD, NULL);
        return;
    }

    trellis_fingerprint_t service_fp;
    memcpy(service_fp.bytes, msg->payload + 5, 32);

    intro_table_register(&service_fp, &msg->source);
    fprintf(stderr, "[GH] ESTABLISH_INTRO registered for service %02x%02x...\n",
            service_fp.bytes[0], service_fp.bytes[1]);
}

trellis_err_t trellis_greenhouse_reply_via_surb(trellis_greenhouse_t *gh,
                                                trellis_surb_t *surb,
                                                const uint8_t *data, size_t len)
{
    if (!gh || !surb || !data || len == 0)
        return TRELLIS_ERR_INVALID_ARG;
    if (trellis_surb_is_used(surb))
        return TRELLIS_ERR_INVALID_STATE;

    trellis_buf_t onion_out = {0};
    trellis_err_t err = trellis_surb_reply(surb, data, len, &onion_out);
    if (err != TRELLIS_OK)
        return err;

    const trellis_fingerprint_t *entry = trellis_surb_entry_hop(surb);
    if (!entry || !gh->router) {
        trellis_buf_free(&onion_out);
        return TRELLIS_ERR_NOT_INITIALIZED;
    }

    err = trellis_router_send(gh->router, entry,
                               onion_out.data, onion_out.len, NULL, NULL);
    trellis_buf_free(&onion_out);
    return err;
}

void trellis_greenhouse_dispatch_msg(const trellis_message_t *msg)
{
    if (!msg)
        return;

    switch ((int)msg->type) {
    case TRELLIS_MSG_GH_DESCRIPTOR:
        // Descriptor publication; handled by DHT store.
        break;

    case TRELLIS_MSG_GH_INTRODUCE:
        dispatch_gh_introduce(msg);
        break;

    case TRELLIS_MSG_GH_RENDEZVOUS_EST:
        dispatch_gh_establish_intro(msg);
        break;

    case TRELLIS_MSG_GH_RENDEZVOUS_JOIN:
        dispatch_gh_rendezvous_join(msg);
        break;

    case TRELLIS_MSG_GH_RENDEZVOUS_CONN:
        dispatch_gh_rendezvous_conn(msg);
        break;

    case TRELLIS_MSG_GH_ERROR:
        // Error responses are handled on the client side (see client.c).
        break;

    default:
        break;
    }
}
