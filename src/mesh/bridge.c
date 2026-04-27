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
 * bridge.c — Bridge Node Multi-Grove Management
 *
 * A bridge node participates in multiple groves simultaneously, maintaining
 * separate DHT instances per grove.  The bridge policy engine controls what
 * traffic may cross grove boundaries.
 *
 * Architecture:
 *   - Each grove slot holds a (grove_id, dht, router, topology) tuple
 *   - The home grove (slot 0) is the node's primary identity
 *   - Additional groves are joined via trellis_bridge_add_grove()
 *   - Cross-grove forwarding is policy-checked before execution
 *
 * Bridge attestations prove a bridge controls identities in two groves
 * without revealing cross-grove linkability to non-bridge peers.
 */

#include "internal.h"
#include <trellis/rhizome.h>
#include <sodium.h>
#include <stdlib.h>
#include <string.h>

// --- Bridge Attestation ---

/*
 * Attestation pre-signature format:
 *   "BATT" magic(4) || version(1) || grove_a(32) || grove_b(32) ||
 *   fp_in_a(32) || fp_in_b(32) || issued_at(8) || expires_at(8)
 */
#define BRIDGE_ATT_MAGIC   "BATT"
#define BRIDGE_ATT_VERSION 1
#define BRIDGE_ATT_PRESIG_LEN (4 + 1 + 32 + 32 + 32 + 32 + 8 + 8)

static void serialize_att_presig(const trellis_bridge_attestation_t *att,
                                 uint8_t buf[BRIDGE_ATT_PRESIG_LEN])
{
    size_t off = 0;
    memcpy(buf + off, BRIDGE_ATT_MAGIC, 4); off += 4;
    buf[off++] = BRIDGE_ATT_VERSION;
    memcpy(buf + off, att->grove_a.bytes, 32); off += 32;
    memcpy(buf + off, att->grove_b.bytes, 32); off += 32;
    memcpy(buf + off, att->fp_in_a.bytes, 32); off += 32;
    memcpy(buf + off, att->fp_in_b.bytes, 32); off += 32;

    for (int i = 0; i < 8; i++)
        buf[off++] = (uint8_t)(att->issued_at >> ((7 - i) * 8));
    for (int i = 0; i < 8; i++)
        buf[off++] = (uint8_t)(att->expires_at >> ((7 - i) * 8));
}

trellis_err_t trellis_bridge_attestation_serialize(
        const trellis_bridge_attestation_t *att,
        trellis_buf_t *out)
{
    if (!att || !out)
        return TRELLIS_ERR_INVALID_ARG;

    size_t total = BRIDGE_ATT_PRESIG_LEN + 2 * TRELLIS_SIGNATURE_LEN;
    *out = trellis_buf_alloc(total);
    if (!out->data)
        return TRELLIS_ERR_NOMEM;

    serialize_att_presig(att, out->data);
    size_t off = BRIDGE_ATT_PRESIG_LEN;

    memcpy(out->data + off, att->sig_a.ed25519_sig, TRELLIS_ED25519_SIG_LEN);
    off += TRELLIS_ED25519_SIG_LEN;
    memcpy(out->data + off, att->sig_a.ml_dsa_sig, TRELLIS_ML_DSA_87_SIG_LEN);
    off += TRELLIS_ML_DSA_87_SIG_LEN;

    memcpy(out->data + off, att->sig_b.ed25519_sig, TRELLIS_ED25519_SIG_LEN);
    off += TRELLIS_ED25519_SIG_LEN;
    memcpy(out->data + off, att->sig_b.ml_dsa_sig, TRELLIS_ML_DSA_87_SIG_LEN);
    off += TRELLIS_ML_DSA_87_SIG_LEN;

    out->len = off;
    return TRELLIS_OK;
}

trellis_err_t trellis_bridge_attestation_deserialize(
        const uint8_t *data, size_t len,
        trellis_bridge_attestation_t *att)
{
    if (!data || !att)
        return TRELLIS_ERR_INVALID_ARG;

    size_t total = BRIDGE_ATT_PRESIG_LEN + 2 * TRELLIS_SIGNATURE_LEN;
    if (len < total)
        return TRELLIS_ERR_MSG_FORMAT;

    memset(att, 0, sizeof(*att));

    size_t off = 0;
    if (memcmp(data + off, BRIDGE_ATT_MAGIC, 4) != 0)
        return TRELLIS_ERR_MSG_FORMAT;
    off += 4;

    if (data[off++] != BRIDGE_ATT_VERSION)
        return TRELLIS_ERR_MSG_FORMAT;

    memcpy(att->grove_a.bytes, data + off, 32); off += 32;
    memcpy(att->grove_b.bytes, data + off, 32); off += 32;
    memcpy(att->fp_in_a.bytes, data + off, 32); off += 32;
    memcpy(att->fp_in_b.bytes, data + off, 32); off += 32;

    att->issued_at = 0;
    for (int i = 0; i < 8; i++)
        att->issued_at |= (uint64_t)data[off++] << ((7 - i) * 8);

    att->expires_at = 0;
    for (int i = 0; i < 8; i++)
        att->expires_at |= (uint64_t)data[off++] << ((7 - i) * 8);

    memcpy(att->sig_a.ed25519_sig, data + off, TRELLIS_ED25519_SIG_LEN);
    off += TRELLIS_ED25519_SIG_LEN;
    memcpy(att->sig_a.ml_dsa_sig, data + off, TRELLIS_ML_DSA_87_SIG_LEN);
    off += TRELLIS_ML_DSA_87_SIG_LEN;

    memcpy(att->sig_b.ed25519_sig, data + off, TRELLIS_ED25519_SIG_LEN);
    off += TRELLIS_ED25519_SIG_LEN;
    memcpy(att->sig_b.ml_dsa_sig, data + off, TRELLIS_ML_DSA_87_SIG_LEN);

    return TRELLIS_OK;
}

trellis_err_t trellis_bridge_attestation_verify(
        const trellis_bridge_attestation_t *att,
        const trellis_identity_public_t *pub_a,
        const trellis_identity_public_t *pub_b)
{
    if (!att || !pub_a || !pub_b)
        return TRELLIS_ERR_INVALID_ARG;

    // Check expiry.
    if (att->expires_at > 0 && att->expires_at < trellis_now_ms())
        return TRELLIS_ERR_CANOPY_EXPIRED;

    uint8_t presig[BRIDGE_ATT_PRESIG_LEN];
    serialize_att_presig(att, presig);

    // Verify signature from identity in grove A.
    trellis_err_t err = trellis_identity_verify(pub_a, presig,
                                                BRIDGE_ATT_PRESIG_LEN,
                                                &att->sig_a);
    if (err != TRELLIS_OK)
        return err;

    // Verify signature from identity in grove B.
    return trellis_identity_verify(pub_b, presig, BRIDGE_ATT_PRESIG_LEN,
                                   &att->sig_b);
}

// --- Bridge Node ---
typedef struct bridge_grove_slot {
    bool                  active;
    trellis_grove_id_t    grove_id;
    trellis_dht_t        *dht;
    trellis_router_t     *router;
    trellis_topology_t   *topology;
    trellis_grove_t      *grove;     /* grove manager (may be NULL for remote joins) */
} bridge_grove_slot_t;

struct trellis_bridge {
    bridge_grove_slot_t   groves[TRELLIS_BRIDGE_MAX_GROVES];
    size_t                grove_count;
    const trellis_identity_t *identity;
    uv_loop_t            *loop;
};

trellis_bridge_t *trellis_bridge_new(trellis_dht_t *home_dht,
                                     trellis_router_t *home_router,
                                     const trellis_identity_t *identity,
                                     uv_loop_t *loop)
{
    if (!home_dht || !identity || !loop)
        return NULL;

    trellis_bridge_t *bridge = calloc(1, sizeof(*bridge));
    if (!bridge)
        return NULL;

    bridge->identity = identity;
    bridge->loop = loop;

    // Slot 0 is the home grove.
    bridge_grove_slot_t *slot = &bridge->groves[0];
    slot->active = true;
    slot->dht = home_dht;
    slot->router = home_router;
    slot->topology = NULL;
    slot->grove = NULL;

    if (home_dht->grove_id)
        slot->grove_id = *home_dht->grove_id;

    bridge->grove_count = 1;

    return bridge;
}

void trellis_bridge_free(trellis_bridge_t *bridge)
{
    if (!bridge)
        return;

    // Free additional grove slots (not slot 0 which is caller-owned)
    for (size_t i = 1; i < TRELLIS_BRIDGE_MAX_GROVES; i++) {
        bridge_grove_slot_t *slot = &bridge->groves[i];
        if (!slot->active)
            continue;

        if (slot->topology) {
            trellis_topology_stop(slot->topology);
            trellis_topology_free(slot->topology);
        }
        if (slot->router)
            trellis_router_free(slot->router);
        if (slot->dht)
            trellis_dht_free(slot->dht);
        if (slot->grove)
            trellis_grove_free(slot->grove);
    }

    free(bridge);
}

trellis_err_t trellis_bridge_add_grove(trellis_bridge_t *bridge,
                                       const trellis_grove_id_t *grove_id,
                                       const char *seed_addr,
                                       int *slot_out)
{
    if (!bridge || !grove_id || !seed_addr)
        return TRELLIS_ERR_INVALID_ARG;

    if (bridge->grove_count >= TRELLIS_BRIDGE_MAX_GROVES)
        return TRELLIS_ERR_LIMIT_EXCEEDED;

    // Check for duplicate.
    for (size_t i = 0; i < TRELLIS_BRIDGE_MAX_GROVES; i++) {
        if (bridge->groves[i].active &&
            trellis_grove_id_eq(&bridge->groves[i].grove_id, grove_id))
            return TRELLIS_ERR_ALREADY_EXISTS;
    }

    // Find free slot.
    int idx = -1;
    for (size_t i = 1; i < TRELLIS_BRIDGE_MAX_GROVES; i++) {
        if (!bridge->groves[i].active) {
            idx = (int)i;
            break;
        }
    }
    if (idx < 0)
        return TRELLIS_ERR_LIMIT_EXCEEDED;

    // Create new DHT instance for this grove.
    trellis_dht_t *dht = trellis_dht_new(bridge->identity, bridge->loop);
    if (!dht)
        return TRELLIS_ERR_NOMEM;

    // Bind the grove namespace.
    trellis_err_t err = trellis_dht_set_grove(dht, grove_id);
    if (err != TRELLIS_OK) {
        trellis_dht_free(dht);
        return err;
    }

    // Bootstrap into the remote grove.
    err = trellis_dht_bootstrap(dht, seed_addr);
    if (err != TRELLIS_OK) {
        trellis_dht_free(dht);
        return err;
    }

    // Create router for this grove.
    trellis_router_t *router = trellis_router_new(dht, NULL);
    if (!router) {
        trellis_dht_free(dht);
        return TRELLIS_ERR_NOMEM;
    }

    // Create topology manager.
    trellis_topology_t *topo = trellis_topology_new(dht, bridge->loop);
    if (!topo) {
        trellis_router_free(router);
        trellis_dht_free(dht);
        return TRELLIS_ERR_NOMEM;
    }

    bridge_grove_slot_t *slot = &bridge->groves[idx];
    slot->active = true;
    slot->grove_id = *grove_id;
    slot->dht = dht;
    slot->router = router;
    slot->topology = topo;
    slot->grove = NULL;

    bridge->grove_count++;

    trellis_err_t start_err = trellis_topology_start(topo);
    if (start_err != TRELLIS_OK) {
        trellis_topology_free(topo);
        trellis_router_free(router);
        trellis_dht_free(dht);
        memset(slot, 0, sizeof(*slot));
        bridge->grove_count--;
        return start_err;
    }

    if (slot_out)
        *slot_out = idx;

    return TRELLIS_OK;
}

trellis_err_t trellis_bridge_remove_grove(trellis_bridge_t *bridge,
                                          const trellis_grove_id_t *grove_id)
{
    if (!bridge || !grove_id)
        return TRELLIS_ERR_INVALID_ARG;

    for (size_t i = 1; i < TRELLIS_BRIDGE_MAX_GROVES; i++) {
        bridge_grove_slot_t *slot = &bridge->groves[i];
        if (!slot->active)
            continue;
        if (!trellis_grove_id_eq(&slot->grove_id, grove_id))
            continue;

        if (slot->topology) {
            trellis_topology_stop(slot->topology);
            trellis_topology_free(slot->topology);
        }
        if (slot->router)
            trellis_router_free(slot->router);
        if (slot->dht)
            trellis_dht_free(slot->dht);
        if (slot->grove)
            trellis_grove_free(slot->grove);

        memset(slot, 0, sizeof(*slot));
        bridge->grove_count--;
        return TRELLIS_OK;
    }

    return TRELLIS_ERR_NOT_FOUND;
}

static bridge_grove_slot_t *find_grove_slot(trellis_bridge_t *bridge,
                                             const trellis_grove_id_t *grove_id)
{
    for (size_t i = 0; i < TRELLIS_BRIDGE_MAX_GROVES; i++) {
        if (bridge->groves[i].active &&
            trellis_grove_id_eq(&bridge->groves[i].grove_id, grove_id))
            return &bridge->groves[i];
    }
    return NULL;
}

trellis_err_t trellis_bridge_forward_find(trellis_bridge_t *bridge,
                                          const trellis_grove_id_t *target_grove,
                                          const uint8_t *key, size_t key_len,
                                          trellis_dht_find_cb cb, void *ctx)
{
    if (!bridge || !target_grove || !key)
        return TRELLIS_ERR_INVALID_ARG;

    bridge_grove_slot_t *slot = find_grove_slot(bridge, target_grove);
    if (!slot)
        return TRELLIS_ERR_GROVE_UNKNOWN;

    // Policy check: if home grove has a grove manager, verify peering.
    if (bridge->groves[0].grove &&
        !trellis_grove_peering_allowed(bridge->groves[0].grove, target_grove))
        return TRELLIS_ERR_GROVE_POLICY;

    return trellis_dht_find_value(slot->dht, key, key_len, cb, ctx);
}

trellis_err_t trellis_bridge_forward_store(trellis_bridge_t *bridge,
                                           const trellis_grove_id_t *target_grove,
                                           const uint8_t *key, size_t key_len,
                                           const uint8_t *value, size_t value_len,
                                           trellis_dht_store_cb cb, void *ctx)
{
    if (!bridge || !target_grove || !key || !value)
        return TRELLIS_ERR_INVALID_ARG;

    bridge_grove_slot_t *slot = find_grove_slot(bridge, target_grove);
    if (!slot)
        return TRELLIS_ERR_GROVE_UNKNOWN;

    if (bridge->groves[0].grove &&
        !trellis_grove_peering_allowed(bridge->groves[0].grove, target_grove))
        return TRELLIS_ERR_GROVE_POLICY;

    return trellis_dht_store(slot->dht, key, key_len, value, value_len,
                             cb, ctx);
}

trellis_err_t trellis_bridge_advertise(trellis_bridge_t *bridge)
{
    if (!bridge)
        return TRELLIS_ERR_INVALID_ARG;

    /* Publish bridge attestation records in each grove's DHT.
     * The canopy layer handles the actual reachability announcements. */
    for (size_t i = 0; i < TRELLIS_BRIDGE_MAX_GROVES; i++) {
        bridge_grove_slot_t *slot = &bridge->groves[i];
        if (!slot->active || !slot->dht)
            continue;

        /* Store a "bridge-present" marker at a well-known DHT key
         * derived from the bridge's fingerprint in this grove via
         * HKDF-SHAKE-256 for proper domain separation. */
        uint8_t marker_key[32];
        uint8_t ikm[64];
        memcpy(ikm, bridge->identity->fingerprint.bytes, 32);
        memcpy(ikm + 32, slot->grove_id.bytes, 32);
        static const uint8_t info[] = "bloom-bridge-v1";
        trellis_hkdf_shake256(ikm, sizeof(ikm),
                              NULL, 0,
                              info, sizeof(info) - 1,
                              marker_key, 32);

        uint8_t marker_val[1] = { 1 };
        trellis_dht_store(slot->dht, marker_key, 32,
                          marker_val, 1, NULL, NULL);
    }

    return TRELLIS_OK;
}

size_t trellis_bridge_grove_count(const trellis_bridge_t *bridge)
{
    return bridge ? bridge->grove_count : 0;
}

trellis_dht_t *trellis_bridge_grove_dht(const trellis_bridge_t *bridge,
                                        size_t index)
{
    if (!bridge || index >= TRELLIS_BRIDGE_MAX_GROVES)
        return NULL;
    if (!bridge->groves[index].active)
        return NULL;
    return bridge->groves[index].dht;
}

const trellis_grove_id_t *trellis_bridge_grove_at(const trellis_bridge_t *bridge,
                                                   size_t index)
{
    if (!bridge || index >= TRELLIS_BRIDGE_MAX_GROVES)
        return NULL;
    if (!bridge->groves[index].active)
        return NULL;
    return &bridge->groves[index].grove_id;
}

const trellis_identity_t *trellis_bridge_identity(const trellis_bridge_t *bridge)
{
    return bridge ? bridge->identity : NULL;
}

const trellis_fingerprint_t *trellis_bridge_fingerprint(const trellis_bridge_t *bridge)
{
    return bridge ? &bridge->identity->fingerprint : NULL;
}

trellis_err_t trellis_bridge_dht_send(const trellis_bridge_t *bridge,
                                      size_t grove_index,
                                      const trellis_fingerprint_t *target,
                                      const uint8_t *data, size_t len)
{
    if (!bridge || !target || !data)
        return TRELLIS_ERR_INVALID_ARG;
    if (grove_index >= TRELLIS_BRIDGE_MAX_GROVES ||
        !bridge->groves[grove_index].active)
        return TRELLIS_ERR_GROVE_UNKNOWN;

    trellis_dht_t *dht = bridge->groves[grove_index].dht;
    if (!dht || !dht->send_fn)
        return TRELLIS_ERR_INVALID_STATE;

    dht->send_fn(dht->send_client, target, data, len);
    return TRELLIS_OK;
}

trellis_err_t trellis_bridge_select_peers(const trellis_bridge_t *bridge,
                                          size_t grove_index,
                                          trellis_peer_info_t **out,
                                          size_t count, size_t *actual)
{
    if (!bridge || !out || !actual)
        return TRELLIS_ERR_INVALID_ARG;
    if (grove_index >= TRELLIS_BRIDGE_MAX_GROVES ||
        !bridge->groves[grove_index].active)
        return TRELLIS_ERR_GROVE_UNKNOWN;

    trellis_dht_t *dht = bridge->groves[grove_index].dht;
    if (!dht)
        return TRELLIS_ERR_INVALID_STATE;

    select_random_peers(&dht->rt, dht->sybil, out, count, actual);
    return TRELLIS_OK;
}

const trellis_grove_policy_t *trellis_bridge_grove_policy(
        const trellis_bridge_t *bridge, size_t index)
{
    if (!bridge || index >= TRELLIS_BRIDGE_MAX_GROVES)
        return NULL;
    if (!bridge->groves[index].active || !bridge->groves[index].grove)
        return NULL;
    return trellis_grove_policy(bridge->groves[index].grove);
}

bool trellis_bridge_peering_allowed(const trellis_bridge_t *bridge,
                                    const trellis_grove_id_t *remote_grove)
{
    if (!bridge || !remote_grove)
        return false;
    if (!bridge->groves[0].grove)
        return true; /* no grove manager = permissive */
    return trellis_grove_peering_allowed(bridge->groves[0].grove, remote_grove);
}

