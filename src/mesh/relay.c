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
#include <trellis/crypto.h>
#include <sodium.h>

void trellis_flow_init_circuit(relay_circuit_t *c);

static void relay_clear_session_state(relay_circuit_t *c);

// ----

static void relay_lifecycle_cb(uv_timer_t *handle)
{
    trellis_relay_t *relay = (trellis_relay_t *)handle->data;
    uint64_t now = trellis_now_ms();

    for (size_t i = 0; i < RELAY_MAX_CIRCUITS; i++) {
        if (!relay->circuits[i].active)
            continue;

        bool expired_age = (now - relay->circuits[i].created_at >
                            RELAY_CIRCUIT_MAX_AGE_MS);
        bool expired_idle = (relay->circuits[i].last_activity_at > 0 &&
                             now - relay->circuits[i].last_activity_at >
                             RELAY_CIRCUIT_IDLE_MS);

        if (expired_age || expired_idle) {
            relay_clear_session_state(&relay->circuits[i]);
            relay->circuits[i].active = false;
            if (relay->circuit_count > 0)
                relay->circuit_count--;
        }
    }

    // Also expire onion circuit-table entries.
    trellis_circuit_table_expire(&relay->circuit_table,
                                 RELAY_CIRCUIT_MAX_AGE_MS);
}

trellis_relay_t *trellis_relay_new(trellis_dht_t *dht, size_t max_capacity)
{
    if (!dht)
        return NULL;

    trellis_relay_t *relay = calloc(1, sizeof(trellis_relay_t));
    if (!relay)
        return NULL;

    relay->dht = dht;
    relay->max_capacity = max_capacity > 0 ? max_capacity : RELAY_MAX_CIRCUITS;
    trellis_circuit_table_init(&relay->circuit_table);
    return relay;
}

static void relay_close_cb(uv_handle_t *handle)
{
    trellis_relay_t *relay = (trellis_relay_t *)handle->data;
    free(relay);
}

void trellis_relay_free(trellis_relay_t *relay)
{
    if (!relay)
        return;
    if (relay->lifecycle_timer_active) {
        uv_timer_stop(&relay->lifecycle_timer);
        relay->lifecycle_timer_active = false;
        if (!uv_is_closing((uv_handle_t *)&relay->lifecycle_timer)) {
            uv_close((uv_handle_t *)&relay->lifecycle_timer, relay_close_cb);
            return;
        }
    }
    free(relay);
}

trellis_err_t trellis_relay_start(trellis_relay_t *relay, uv_loop_t *loop)
{
    if (!relay || !loop)
        return TRELLIS_ERR_INVALID_ARG;
    relay->loop = loop;
    uv_timer_init(loop, &relay->lifecycle_timer);
    relay->lifecycle_timer.data = relay;
    uv_timer_start(&relay->lifecycle_timer, relay_lifecycle_cb,
                    RELAY_LIFECYCLE_INTERVAL_MS, RELAY_LIFECYCLE_INTERVAL_MS);
    relay->lifecycle_timer_active = true;
    return TRELLIS_OK;
}

/*
 * Check per-peer circuit creation rate.  Prevents a single authenticated
 * peer from flooding the relay with circuit requests — the same attack
 * vector that drives Tor's ongoing onion-service DDoS.
 */
static trellis_err_t relay_check_peer_rate(trellis_relay_t *relay,
                                           const trellis_fingerprint_t *peer)
{
    uint64_t now = trellis_now_ms();

    // Global rate: max RELAY_GLOBAL_CIRCUIT_RATE new circuits per second.
    if (now - relay->global_circuit_window >= 1000) {
        relay->global_circuit_window = now;
        relay->global_circuit_count = 0;
    }
    if (relay->global_circuit_count >= RELAY_GLOBAL_CIRCUIT_RATE)
        return TRELLIS_ERR_RATE_LIMITED;

    /* Per-peer rate: find or allocate a slot.
       Linear scan is fine for now (max 64 entries), revisit if relay_peer_rate_cap grows. */
    int match = -1;
    int oldest_idx = 0;
    uint64_t oldest_ts = UINT64_MAX;

    for (size_t i = 0; i < relay->peer_rate_count; i++) {
        if (trellis_fingerprint_eq(&relay->peer_rates[i].peer, peer)) {
            match = (int)i;
            break;
        }
        if (relay->peer_rates[i].window_start_ms < oldest_ts) {
            oldest_ts = relay->peer_rates[i].window_start_ms;
            oldest_idx = (int)i;
        }
    }

    relay_peer_rate_t *slot;
    if (match >= 0) {
        slot = &relay->peer_rates[match];
    } else if (relay->peer_rate_count < RELAY_PEER_RATE_SLOTS) {
        slot = &relay->peer_rates[relay->peer_rate_count++];
        slot->peer = *peer;
        slot->circuits_this_window = 0;
        slot->window_start_ms = now;
    } else {
        slot = &relay->peer_rates[oldest_idx];
        slot->peer = *peer;
        slot->circuits_this_window = 0;
        slot->window_start_ms = now;
    }

    if (now - slot->window_start_ms >= RELAY_PEER_RATE_WINDOW_MS) {
        slot->window_start_ms = now;
        slot->circuits_this_window = 0;
    }

    if (slot->circuits_this_window >= RELAY_PEER_CIRCUIT_LIMIT)
        return TRELLIS_ERR_RATE_LIMITED;

    slot->circuits_this_window++;
    relay->global_circuit_count++;
    return TRELLIS_OK;
}

static void relay_clear_session_state(relay_circuit_t *c)
{
    if (c->session_keyed) {
        sodium_memzero(c->session_key, sizeof(c->session_key));
        c->session_keyed = false;
        c->session_circuit_id = 0;
        c->session_nonce_counter = 0;
    }
}

static void relay_init_circuit_slot(relay_circuit_t *c,
                                    const trellis_fingerprint_t *from,
                                    const trellis_fingerprint_t *to)
{
    relay_clear_session_state(c);
    uint64_t now = trellis_now_ms();
    c->from = *from;
    c->to = *to;
    c->active = true;
    c->bytes_relayed = 0;
    c->created_at = now;
    c->last_activity_at = now;
    trellis_flow_init_circuit(c);
}

trellis_err_t trellis_relay_create_circuit(trellis_relay_t *relay,
                                           const trellis_fingerprint_t *from,
                                           const trellis_fingerprint_t *to)
{
    if (!relay || !from || !to)
        return TRELLIS_ERR_INVALID_ARG;

    // Per-peer and global circuit creation rate limits.
    trellis_err_t rate_err = relay_check_peer_rate(relay, from);
    if (rate_err != TRELLIS_OK)
        return rate_err;

    for (size_t i = 0; i < RELAY_MAX_CIRCUITS; i++) {
        if (relay->circuits[i].active &&
            trellis_fingerprint_eq(&relay->circuits[i].from, from) &&
            trellis_fingerprint_eq(&relay->circuits[i].to, to)) {
            return TRELLIS_ERR_ALREADY_EXISTS;
        }
    }

    // Find free slot.
    for (size_t i = 0; i < RELAY_MAX_CIRCUITS; i++) {
        if (!relay->circuits[i].active) {
            relay_init_circuit_slot(&relay->circuits[i], from, to);
            relay->circuit_count++;
            return TRELLIS_OK;
        }
    }

    // LRU eviction: if at capacity, evict oldest idle circuit.
    if (relay->circuit_count >= relay->max_capacity) {
        int oldest_idx = -1;
        uint64_t oldest_activity = UINT64_MAX;
        for (size_t i = 0; i < RELAY_MAX_CIRCUITS; i++) {
            if (relay->circuits[i].active &&
                relay->circuits[i].last_activity_at < oldest_activity) {
                oldest_activity = relay->circuits[i].last_activity_at;
                oldest_idx = (int)i;
            }
        }
        if (oldest_idx >= 0) {
            relay->circuits[oldest_idx].active = false;
            relay->circuit_count--;
            relay_init_circuit_slot(&relay->circuits[oldest_idx], from, to);
            relay->circuit_count++;
            return TRELLIS_OK;
        }
    }

    return TRELLIS_ERR_NOMEM;
}

trellis_err_t trellis_relay_forward(trellis_relay_t *relay,
                                    const trellis_fingerprint_t *from,
                                    const trellis_fingerprint_t *to,
                                    const uint8_t *data, size_t len)
{
    if (!relay || !from || !to || !data)
        return TRELLIS_ERR_INVALID_ARG;

    for (size_t i = 0; i < RELAY_MAX_CIRCUITS; i++) {
        if (!relay->circuits[i].active)
            continue;
        if (trellis_fingerprint_eq(&relay->circuits[i].from, from) &&
            trellis_fingerprint_eq(&relay->circuits[i].to, to)) {
            if (!trellis_flow_can_send(&relay->circuits[i]))
                return TRELLIS_ERR_RATE_LIMITED;

            relay->circuits[i].bytes_relayed += len;
            relay->circuits[i].last_activity_at = trellis_now_ms();
            trellis_flow_on_send(&relay->circuits[i], len);

            /* Track receive-side: decrement recv_window and flag
             * when a SENDME should be sent back to the sender */
            if (trellis_flow_on_recv(&relay->circuits[i], len))
                relay->circuits[i].recv_window = 1; /* flag: SENDME needed */
            else if (relay->circuits[i].recv_window != 1)
                relay->circuits[i].recv_window = 0;

            return TRELLIS_OK;
        }
    }

    return TRELLIS_ERR_NOT_FOUND;
}

bool trellis_relay_needs_sendme(trellis_relay_t *relay,
                                const trellis_fingerprint_t *from,
                                const trellis_fingerprint_t *to)
{
    if (!relay || !from || !to) return false;
    for (size_t i = 0; i < RELAY_MAX_CIRCUITS; i++) {
        if (!relay->circuits[i].active) continue;
        if (trellis_fingerprint_eq(&relay->circuits[i].from, from) &&
            trellis_fingerprint_eq(&relay->circuits[i].to, to)) {
            if (relay->circuits[i].recv_window == 1) {
                relay->circuits[i].recv_window = 0;
                return true;
            }
            return false;
        }
    }
    return false;
}

trellis_err_t trellis_relay_close_circuit(trellis_relay_t *relay,
                                          const trellis_fingerprint_t *from,
                                          const trellis_fingerprint_t *to)
{
    if (!relay || !from || !to)
        return TRELLIS_ERR_INVALID_ARG;

    for (size_t i = 0; i < RELAY_MAX_CIRCUITS; i++) {
        if (!relay->circuits[i].active)
            continue;
        if (trellis_fingerprint_eq(&relay->circuits[i].from, from) &&
            trellis_fingerprint_eq(&relay->circuits[i].to, to)) {
            relay_clear_session_state(&relay->circuits[i]);
            relay->circuits[i].active = false;
            relay->circuit_count--;
            return TRELLIS_OK;
        }
    }

    return TRELLIS_ERR_NOT_FOUND;
}

size_t trellis_relay_active_circuits(const trellis_relay_t *relay)
{
    return relay ? relay->circuit_count : 0;
}

size_t trellis_relay_remaining_capacity(const trellis_relay_t *relay)
{
    if (!relay || relay->circuit_count >= relay->max_capacity)
        return 0;
    return relay->max_capacity - relay->circuit_count;
}

// SENDME flow control.
void trellis_flow_on_sendme(relay_circuit_t *c);

void trellis_relay_on_sendme(trellis_relay_t *relay,
                             const trellis_fingerprint_t *from,
                             const trellis_fingerprint_t *to)
{
    if (!relay || !from || !to)
        return;
    for (size_t i = 0; i < RELAY_MAX_CIRCUITS; i++) {
        if (!relay->circuits[i].active)
            continue;
        if (trellis_fingerprint_eq(&relay->circuits[i].from, from) &&
            trellis_fingerprint_eq(&relay->circuits[i].to, to)) {
            trellis_flow_on_sendme(&relay->circuits[i]);
            relay->circuits[i].last_activity_at = trellis_now_ms();
            return;
        }
    }
}

relay_circuit_t *trellis_relay_find_circuit(trellis_relay_t *relay,
                                            const trellis_fingerprint_t *from,
                                            const trellis_fingerprint_t *to)
{
    if (!relay || !from || !to) return NULL;
    for (size_t i = 0; i < RELAY_MAX_CIRCUITS; i++) {
        if (!relay->circuits[i].active) continue;
        if (trellis_fingerprint_eq(&relay->circuits[i].from, from) &&
            trellis_fingerprint_eq(&relay->circuits[i].to, to))
            return &relay->circuits[i];
    }
    return NULL;
}

// Onion circuit reverse-path data handling.
void trellis_relay_handle_circuit_data(trellis_relay_t *relay,
                                       void *client_ptr,
                                       uint32_t stream_id,
                                       const uint8_t *data, size_t len)
{
    if (!relay || !data || len == 0)
        return;

    circuit_entry_t *ce = trellis_circuit_table_lookup(
        &relay->circuit_table, stream_id);
    if (!ce)
        return;

    /* Touch for freshness. Re-encryption and forwarding to prev_hop is
     * performed by client.c's CIRCUIT_DATA handler, which has access to
     * the peer connection table and the layer_key stored in circuit_entry_t. */
    ce->created_at = trellis_now_ms();
    (void)client_ptr;
}

// Session-keyed circuit handling.
trellis_err_t trellis_relay_handle_create_session(trellis_relay_t *relay,
                                                  const trellis_fingerprint_t *from,
                                                  const uint8_t *payload,
                                                  size_t payload_len,
                                                  uint8_t *response, size_t *resp_len)
{
    if (!relay || !from || !payload || payload_len == 0)
        return TRELLIS_ERR_INVALID_ARG;

    /* Payload: [circuit_id(4)][kem_ciphertext]
     * The KEM ciphertext contains the encapsulated shared secret. */
    if (payload_len < 4)
        return TRELLIS_ERR_INVALID_ARG;

    uint32_t circuit_id = ((uint32_t)payload[0] << 24) | ((uint32_t)payload[1] << 16) |
                          ((uint32_t)payload[2] << 8)  | (uint32_t)payload[3];

    // Rate-limit session circuit creation.
    trellis_err_t rate_err = relay_check_peer_rate(relay, from);
    if (rate_err != TRELLIS_OK)
        return rate_err;

    // Check for duplicate (retransmit) — reuse existing circuit.
    for (size_t i = 0; i < RELAY_MAX_CIRCUITS; i++) {
        if (relay->circuits[i].active && relay->circuits[i].session_keyed &&
            relay->circuits[i].session_circuit_id == circuit_id &&
            trellis_fingerprint_eq(&relay->circuits[i].from, from)) {
            if (response && resp_len && *resp_len >= 5) {
                response[0] = (uint8_t)(circuit_id >> 24);
                response[1] = (uint8_t)(circuit_id >> 16);
                response[2] = (uint8_t)(circuit_id >> 8);
                response[3] = (uint8_t)(circuit_id);
                response[4] = 0x01;
                *resp_len = 5;
            }
            return TRELLIS_OK;
        }
    }

    if (relay->circuit_count >= relay->max_capacity)
        return TRELLIS_ERR_NOMEM;

    // Find a free relay circuit slot.
    relay_circuit_t *slot = NULL;
    for (size_t i = 0; i < RELAY_MAX_CIRCUITS; i++) {
        if (!relay->circuits[i].active) {
            slot = &relay->circuits[i];
            break;
        }
    }
    if (!slot) return TRELLIS_ERR_NOMEM;

    // Decapsulate KEM to derive shared secret.
    const uint8_t *kem_ct = payload + 4;
    size_t kem_ct_len = payload_len - 4;
    size_t expected_ct = TRELLIS_X25519_PK_LEN + TRELLIS_ML_KEM_1024_CT_LEN;
    if (kem_ct_len < expected_ct)
        return TRELLIS_ERR_INVALID_ARG;

    const trellis_identity_t *id = relay->dht ? relay->dht->local_id : NULL;
    if (!id) return TRELLIS_ERR_NOT_INITIALIZED;

    trellis_kem_keypair_t kp;
    memcpy(kp.x25519_pk, id->x25519_pk, sizeof(kp.x25519_pk));
    memcpy(kp.x25519_sk, id->x25519_sk, sizeof(kp.x25519_sk));
    memcpy(kp.ml_kem_pk, id->ml_kem_pk, sizeof(kp.ml_kem_pk));
    memcpy(kp.ml_kem_sk, id->ml_kem_sk, sizeof(kp.ml_kem_sk));

    uint8_t shared_secret[TRELLIS_ML_KEM_1024_SS_LEN];
    // kem_ct contains [x25519_ephemeral_pk][ml_kem_ciphertext]
    const uint8_t *peer_x25519_pk = kem_ct;
    const uint8_t *ml_kem_ct = kem_ct + TRELLIS_X25519_PK_LEN;

    trellis_err_t err = trellis_kem_decaps(
        &kp, peer_x25519_pk, ml_kem_ct, shared_secret);
    sodium_memzero(&kp, sizeof(kp));
    if (err != TRELLIS_OK)
        return err;

    // Derive session key from KEM shared secret.
    relay_init_circuit_slot(slot, from, from);
    slot->session_keyed = true;
    slot->session_circuit_id = circuit_id;
    slot->session_nonce_counter = 0;

    trellis_hkdf_shake256(
        shared_secret, TRELLIS_ML_KEM_1024_SS_LEN,
        (const uint8_t *)"vine-session", 12,
        (const uint8_t *)"session-key-v1", 14,
        slot->session_key, 32);

    sodium_memzero(shared_secret, sizeof(shared_secret));
    relay->circuit_count++;

    // Build response: [circuit_id(4)][ok(1)]
    if (response && resp_len && *resp_len >= 5) {
        response[0] = (uint8_t)(circuit_id >> 24);
        response[1] = (uint8_t)(circuit_id >> 16);
        response[2] = (uint8_t)(circuit_id >> 8);
        response[3] = (uint8_t)(circuit_id);
        response[4] = 0x01; /* success */
        *resp_len = 5;
    }

    return TRELLIS_OK;
}

trellis_err_t trellis_relay_handle_session_data(trellis_relay_t *relay,
                                                const trellis_fingerprint_t *from,
                                                const uint8_t *payload,
                                                size_t payload_len,
                                                uint8_t *plaintext, size_t *pt_len,
                                                trellis_fingerprint_t *dest_out)
{
    if (!relay || !from || !payload || !plaintext || !pt_len || !dest_out)
        return TRELLIS_ERR_INVALID_ARG;

    // SESSION_DATA wire format: [circuit_id(4)][dest_fp(32)][nonce(12)][ciphertext][tag(16)]
    if (payload_len < 4 + TRELLIS_FINGERPRINT_LEN + TRELLIS_AES_NONCE_LEN + TRELLIS_AES_TAG_LEN)
        return TRELLIS_ERR_INVALID_ARG;

    uint32_t circuit_id = ((uint32_t)payload[0] << 24) | ((uint32_t)payload[1] << 16) |
                          ((uint32_t)payload[2] << 8)  | (uint32_t)payload[3];
    memcpy(dest_out->bytes, payload + 4, TRELLIS_FINGERPRINT_LEN);
    const uint8_t *nonce = payload + 4 + TRELLIS_FINGERPRINT_LEN;
    const uint8_t *ct = nonce + TRELLIS_AES_NONCE_LEN;
    size_t ct_len = payload_len - 4 - TRELLIS_FINGERPRINT_LEN - TRELLIS_AES_NONCE_LEN - TRELLIS_AES_TAG_LEN;
    const uint8_t *tag = ct + ct_len;

    if (*pt_len < ct_len)
        return TRELLIS_ERR_NOMEM;

    // Find the session-keyed circuit by circuit_id and peer fingerprint.
    relay_circuit_t *circ = NULL;
    for (size_t i = 0; i < RELAY_MAX_CIRCUITS; i++) {
        if (!relay->circuits[i].active || !relay->circuits[i].session_keyed)
            continue;
        if (relay->circuits[i].session_circuit_id == circuit_id &&
            trellis_fingerprint_eq(&relay->circuits[i].from, from)) {
            circ = &relay->circuits[i];
            break;
        }
    }
    if (!circ) return TRELLIS_ERR_NOT_FOUND;

    // Nonce replay protection: extract counter from big-endian nonce tail.
    uint64_t received_nc = 0;
    for (int i = 0; i < 8; i++)
        received_nc = (received_nc << 8) | nonce[TRELLIS_AES_NONCE_LEN - 8 + i];
    if (received_nc < circ->session_nonce_counter)
        return TRELLIS_ERR_INVALID_ARG;

    // Decrypt with session key.
    trellis_err_t err = trellis_aes256gcm_decrypt(
        circ->session_key, nonce, ct, ct_len, NULL, 0, tag, plaintext);
    if (err != TRELLIS_OK)
        return err;

    circ->session_nonce_counter = received_nc + 1;
    *pt_len = ct_len;
    circ->bytes_relayed += payload_len;
    circ->last_activity_at = trellis_now_ms();

    return TRELLIS_OK;
}

// ----

void trellis_relay_close_circuits_for_peer(trellis_relay_t *relay,
                                           const trellis_fingerprint_t *peer)
{
    if (!relay || !peer)
        return;
    for (size_t i = 0; i < RELAY_MAX_CIRCUITS; i++) {
        if (!relay->circuits[i].active)
            continue;
        if (trellis_fingerprint_eq(&relay->circuits[i].from, peer) ||
            trellis_fingerprint_eq(&relay->circuits[i].to, peer)) {
            relay_clear_session_state(&relay->circuits[i]);
            relay->circuits[i].active = false;
            if (relay->circuit_count > 0)
                relay->circuit_count--;
        }
    }
}
