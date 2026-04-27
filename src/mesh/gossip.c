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

/*
 * Gossip propagation and deduplication.
 *
 * Wire format:
 *   [32 bytes] publisher fingerprint
 *   [TRELLIS_SIGNATURE_LEN bytes] dual-signature over (fingerprint || payload)
 *   [N bytes] payload
 *
 * The hybrid signature matches the rest of the library and provides
 * post-quantum authentication against a forged-gossip attack.
 *
 * Flood protection (DDoS Defense Layer 5):
 *   - Per-publisher rate limiting (10 msgs / 60s) checked BEFORE signature
 *     verification to avoid Tor-style top-half attack (expensive verification
 *     wasted on flood traffic)
 *   - Payload size cap (4 KB) checked before any crypto
 *   - Dedup set with TTL-based expiry and hard cap (16384 entries)
 *   - Content-hash deduplication prevents re-propagation of seen messages
 */
#define GOSSIP_FP_OFF      0
#define GOSSIP_SIG_OFF     TRELLIS_FINGERPRINT_LEN
#define GOSSIP_PAYLOAD_OFF (TRELLIS_FINGERPRINT_LEN + TRELLIS_SIGNATURE_LEN)
#define GOSSIP_HEADER_LEN  GOSSIP_PAYLOAD_OFF

static void compute_message_id(const uint8_t *data, size_t len,
                               uint8_t out[32])
{
    crypto_hash_sha256_state state;
    crypto_hash_sha256_init(&state);
    crypto_hash_sha256_update(&state, data, len);
    crypto_hash_sha256_final(&state, out);
}

static bool gossip_check_publisher_rate(trellis_gossip_t *gossip,
                                        const uint8_t *fp_bytes)
{
    uint64_t now = trellis_now_ms();

    // Hash the fingerprint to an 8-byte key for the rate table.
    uint8_t fp_hash[8];
    crypto_generichash(fp_hash, sizeof(fp_hash), fp_bytes,
                       TRELLIS_FINGERPRINT_LEN, NULL, 0);

    // Find or allocate a slot.
    int match = -1;
    int oldest_idx = 0;
    uint64_t oldest_ts = UINT64_MAX;

    for (int i = 0; i < GOSSIP_PUB_RATE_SLOTS; i++) {
        gossip_pub_rate_t *r = &gossip->pub_rates[i];
        if (memcmp(r->fp_hash, fp_hash, 8) == 0) {
            match = i;
            break;
        }
        if (r->window_start < oldest_ts) {
            oldest_ts = r->window_start;
            oldest_idx = i;
        }
    }

    gossip_pub_rate_t *slot;
    if (match >= 0) {
        slot = &gossip->pub_rates[match];
    } else {
        slot = &gossip->pub_rates[oldest_idx];
        memcpy(slot->fp_hash, fp_hash, 8);
        slot->count = 0;
        slot->window_start = now;
    }

    if (now - slot->window_start >= GOSSIP_PUB_RATE_WINDOW) {
        slot->window_start = now;
        slot->count = 0;
    }

    slot->count++;
    return slot->count <= GOSSIP_PUB_RATE_LIMIT;
}

static void dedup_evict_expired(dedup_set_t *s, uint64_t ttl_ms)
{
    uint64_t now = trellis_now_ms();
    size_t evicted = 0;
    for (size_t i = 0; i < s->cap; i++) {
        if (!s->entries[i].occupied)
            continue;
        if (now - s->entries[i].inserted_at > ttl_ms) {
            s->entries[i].occupied = false;
            memset(s->entries[i].digest, 0, 32);
            evicted++;
        }
    }
    if (evicted <= s->count)
        s->count -= evicted;
    else
        s->count = 0;
}

trellis_gossip_t *trellis_gossip_new(trellis_dht_t *dht,
                                      const trellis_identity_t *id)
{
    if (!dht || !id)
        return NULL;

    trellis_gossip_t *g = calloc(1, sizeof(trellis_gossip_t));
    if (!g)
        return NULL;

    g->dht = dht;
    g->identity = id;
    g->fan_out = TRELLIS_GOSSIP_FAN_OUT;
    g->ttl_ms = TRELLIS_GOSSIP_DEFAULT_TTL;
    g->subscriber_count = 0;

    if (dedup_set_init(&g->seen, DEDUP_SET_INIT_CAP) != 0) {
        free(g);
        return NULL;
    }

    return g;
}

void trellis_gossip_free(trellis_gossip_t *gossip)
{
    if (!gossip)
        return;
    dedup_set_destroy(&gossip->seen);
    free(gossip->grove_scope);
    free(gossip);
}

void trellis_gossip_set_send(trellis_gossip_t *gossip,
                              trellis_gossip_send_fn fn, void *client)
{
    if (!gossip) return;
    gossip->send_fn = fn;
    gossip->client = client;
}

static void gossip_fan_out(trellis_gossip_t *gossip,
                           const uint8_t *data, size_t len,
                           const trellis_fingerprint_t *skip)
{
    if (!gossip->send_fn || !gossip->client)
        return;

    trellis_peer_info_t *targets[TRELLIS_GOSSIP_FAN_OUT];
    size_t actual = 0;
    select_random_peers(&gossip->dht->rt, gossip->dht->sybil,
                        targets, gossip->fan_out, &actual);

    for (size_t i = 0; i < actual; i++) {
        if (skip && trellis_fingerprint_eq(&targets[i]->fingerprint, skip))
            continue;
        gossip->send_fn(gossip->client, &targets[i]->fingerprint, data, len);
    }
}

trellis_err_t trellis_gossip_publish(trellis_gossip_t *gossip,
                                      const uint8_t *data, size_t len)
{
    if (!gossip || !data || len == 0)
        return TRELLIS_ERR_INVALID_ARG;

    // Build authenticated frame: [fp][sig][payload]
    size_t frame_len = GOSSIP_HEADER_LEN + len;
    uint8_t *frame = malloc(frame_len);
    if (!frame)
        return TRELLIS_ERR_NOMEM;

    trellis_identity_public_t pub;
    trellis_identity_public_from(gossip->identity, &pub);

    memcpy(frame + GOSSIP_FP_OFF, pub.fingerprint.bytes, TRELLIS_FINGERPRINT_LEN);

    memcpy(frame + GOSSIP_PAYLOAD_OFF, data, len);

    trellis_signature_t sig;
    trellis_err_t serr = trellis_identity_sign(gossip->identity,
                                               frame + GOSSIP_FP_OFF,
                                               TRELLIS_FINGERPRINT_LEN + len,
                                               &sig);
    if (serr != TRELLIS_OK) {
        free(frame);
        return TRELLIS_ERR_SIGN_FAILED;
    }
    memcpy(frame + GOSSIP_SIG_OFF, &sig, TRELLIS_SIGNATURE_LEN);
    sodium_memzero(&sig, sizeof(sig));

    uint8_t msg_id[32];
    compute_message_id(frame, frame_len, msg_id);

    if (dedup_set_insert(&gossip->seen, msg_id) != 0) {
        free(frame);
        return TRELLIS_ERR_NOMEM;
    }

    for (size_t i = 0; i < gossip->subscriber_count; i++)
        gossip->subscribers[i].cb(data, len, &pub.fingerprint,
                                  gossip->subscribers[i].ctx);

    gossip_fan_out(gossip, frame, frame_len, NULL);
    free(frame);

    return TRELLIS_OK;
}

trellis_err_t trellis_gossip_subscribe(trellis_gossip_t *gossip,
                                        trellis_gossip_cb cb, void *ctx)
{
    if (!gossip || !cb)
        return TRELLIS_ERR_INVALID_ARG;
    if (gossip->subscriber_count >= GOSSIP_MAX_SUBSCRIBERS)
        return TRELLIS_ERR_NOMEM;

    gossip->subscribers[gossip->subscriber_count].cb = cb;
    gossip->subscribers[gossip->subscriber_count].ctx = ctx;
    gossip->subscriber_count++;
    return TRELLIS_OK;
}

trellis_err_t trellis_gossip_receive(trellis_gossip_t *gossip,
                                     const uint8_t *data, size_t len,
                                     const trellis_fingerprint_t *origin)
{
    if (!gossip || !data || !origin)
        return TRELLIS_ERR_INVALID_ARG;
    if (len <= GOSSIP_HEADER_LEN)
        return TRELLIS_ERR_DECODE;

    size_t payload_len = len - GOSSIP_HEADER_LEN;

    /* Payload size cap: reject oversized messages before any crypto work.
     * Prevents amplification attacks via gossip. */
    if (payload_len > GOSSIP_MAX_PAYLOAD)
        return TRELLIS_ERR_LIMIT_EXCEEDED;

    const uint8_t *fp_bytes  = data + GOSSIP_FP_OFF;

    /* Per-publisher rate limiting: checked BEFORE signature verification
     * to avoid the Tor-style top-half attack where expensive verification
     * is wasted on flood traffic from a single publisher. */
    if (!gossip_check_publisher_rate(gossip, fp_bytes))
        return TRELLIS_ERR_RATE_LIMITED;

    const uint8_t *sig_bytes = data + GOSSIP_SIG_OFF;
    const uint8_t *payload   = data + GOSSIP_PAYLOAD_OFF;

    trellis_fingerprint_t embedded_fp;
    memcpy(embedded_fp.bytes, fp_bytes, TRELLIS_FINGERPRINT_LEN);
    if (!trellis_fingerprint_eq(&embedded_fp, origin))
        return TRELLIS_ERR_VERIFY_FAILED;

    const trellis_peer_info_t *peer =
        trellis_dht_lookup_peer(gossip->dht, origin);
    if (!peer)
        return TRELLIS_ERR_NOT_FOUND;

    trellis_signature_t sig;
    memcpy(&sig, sig_bytes, TRELLIS_SIGNATURE_LEN);
    trellis_err_t verr = trellis_identity_verify(&peer->identity,
                                                  fp_bytes,
                                                  TRELLIS_FINGERPRINT_LEN + payload_len,
                                                  &sig);
    sodium_memzero(&sig, sizeof(sig));
    if (verr != TRELLIS_OK)
        return TRELLIS_ERR_VERIFY_FAILED;

    uint8_t msg_id[32];
    compute_message_id(data, len, msg_id);

    if (dedup_set_contains(&gossip->seen, msg_id))
        return TRELLIS_OK;

    // TTL-based eviction + hard cap enforcement.
    if (gossip->seen.count >= DEDUP_SET_MAX_CAP) {
        dedup_evict_expired(&gossip->seen, gossip->ttl_ms);
        if (gossip->seen.count >= DEDUP_SET_MAX_CAP)
            return TRELLIS_ERR_FULL;
    }

    if (dedup_set_insert(&gossip->seen, msg_id) != 0)
        return TRELLIS_ERR_NOMEM;

    for (size_t i = 0; i < gossip->subscriber_count; i++)
        gossip->subscribers[i].cb(payload, payload_len, origin,
                                  gossip->subscribers[i].ctx);

    gossip_fan_out(gossip, data, len, origin);

    return TRELLIS_OK;
}

void trellis_gossip_set_grove_scope(trellis_gossip_t *gossip,
                                    const trellis_grove_id_t *grove_id)
{
    if (!gossip)
        return;

    if (grove_id) {
        if (!gossip->grove_scope) {
            gossip->grove_scope = malloc(sizeof(trellis_grove_id_t));
            if (!gossip->grove_scope)
                return;
        }
        memcpy(gossip->grove_scope, grove_id, sizeof(trellis_grove_id_t));
    } else {
        free(gossip->grove_scope);
        gossip->grove_scope = NULL;
    }
}

const trellis_grove_id_t *trellis_gossip_grove_scope(const trellis_gossip_t *gossip)
{
    return gossip ? gossip->grove_scope : NULL;
}

/*
 * Cross-grove gossip relay.  A bridge node calls this to forward a gossip
 * message received in one grove to another grove's gossip instance, subject
 * to policy checks.
 *
 * The message is re-signed by the bridge identity for the target grove,
 * preserving the original payload but updating the publisher fingerprint
 * to the bridge's identity in the target grove.  This prevents cross-grove
 * signature verification failures while maintaining content integrity.
 *
 * Rate limiting is applied per-grove-per-publisher (the original publisher
 * fingerprint is hashed together with the source grove_id).
 */
trellis_err_t trellis_gossip_cross_grove_relay(
        trellis_gossip_t *target_gossip,
        const uint8_t *original_payload, size_t payload_len,
        const trellis_fingerprint_t *original_publisher,
        const trellis_grove_id_t *source_grove,
        const trellis_identity_t *bridge_identity)
{
    if (!target_gossip || !original_payload || !original_publisher ||
        !source_grove || !bridge_identity)
        return TRELLIS_ERR_INVALID_ARG;

    if (payload_len > GOSSIP_MAX_PAYLOAD)
        return TRELLIS_ERR_LIMIT_EXCEEDED;

    // Cross-grove rate limiting: hash publisher FP with source grove_id.
    uint8_t cross_grove_key[32];
    crypto_hash_sha256_state rate_state;
    crypto_hash_sha256_init(&rate_state);
    crypto_hash_sha256_update(&rate_state, original_publisher->bytes, 32);
    crypto_hash_sha256_update(&rate_state, source_grove->bytes, 32);
    crypto_hash_sha256_final(&rate_state, cross_grove_key);

    if (!gossip_check_publisher_rate(target_gossip, cross_grove_key))
        return TRELLIS_ERR_RATE_LIMITED;

    /* Dedup check: hash original payload with source grove to create
     * a cross-grove-unique message ID */
    uint8_t cross_msg_id[32];
    crypto_hash_sha256_state dedup_state;
    crypto_hash_sha256_init(&dedup_state);
    crypto_hash_sha256_update(&dedup_state, original_payload, payload_len);
    crypto_hash_sha256_update(&dedup_state, source_grove->bytes, 32);
    crypto_hash_sha256_final(&dedup_state, cross_msg_id);

    if (dedup_set_contains(&target_gossip->seen, cross_msg_id))
        return TRELLIS_OK;

    if (target_gossip->seen.count >= DEDUP_SET_MAX_CAP) {
        dedup_evict_expired(&target_gossip->seen, target_gossip->ttl_ms);
        if (target_gossip->seen.count >= DEDUP_SET_MAX_CAP)
            return TRELLIS_ERR_FULL;
    }

    if (dedup_set_insert(&target_gossip->seen, cross_msg_id) != 0)
        return TRELLIS_ERR_NOMEM;

    // Re-sign under bridge identity for the target grove.
    size_t frame_len = GOSSIP_HEADER_LEN + payload_len;
    uint8_t *frame = malloc(frame_len);
    if (!frame)
        return TRELLIS_ERR_NOMEM;

    memcpy(frame + GOSSIP_FP_OFF,
           bridge_identity->fingerprint.bytes,
           TRELLIS_FINGERPRINT_LEN);
    memcpy(frame + GOSSIP_PAYLOAD_OFF, original_payload, payload_len);

    trellis_signature_t sig;
    trellis_err_t err = trellis_identity_sign(
        bridge_identity,
        frame + GOSSIP_FP_OFF,
        TRELLIS_FINGERPRINT_LEN + payload_len,
        &sig);
    if (err != TRELLIS_OK) {
        free(frame);
        return err;
    }

    memcpy(frame + GOSSIP_SIG_OFF, &sig, TRELLIS_SIGNATURE_LEN);
    sodium_memzero(&sig, sizeof(sig));

    // Deliver to local subscribers.
    for (size_t i = 0; i < target_gossip->subscriber_count; i++) {
        target_gossip->subscribers[i].cb(
            original_payload, payload_len,
            &bridge_identity->fingerprint,
            target_gossip->subscribers[i].ctx);
    }

    // Fan out to target grove peers.
    gossip_fan_out(target_gossip, frame, frame_len, NULL);
    free(frame);

    return TRELLIS_OK;
}
