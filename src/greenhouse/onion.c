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

trellis_onion_circuit_t *trellis_onion_new(size_t hop_count)
{
    if (hop_count == 0)
        hop_count = TRELLIS_ONION_DEFAULT_HOPS;

    trellis_onion_circuit_t *c = calloc(1, sizeof(*c));
    if (!c)
        return NULL;

    c->hops = calloc(hop_count, sizeof(*c->hops));
    if (!c->hops) {
        free(c);
        return NULL;
    }
    c->max_hops = hop_count;
    return c;
}

void trellis_onion_free(trellis_onion_circuit_t *circuit)
{
    if (!circuit)
        return;

    for (size_t i = 0; i < circuit->hop_count; i++) {
        sodium_memzero(circuit->hops[i].shared_secret, 32);
        free(circuit->hops[i].ml_kem_ct);
    }
    free(circuit->hops);
    free(circuit);
}

trellis_err_t trellis_onion_add_hop(trellis_onion_circuit_t *circuit,
                                     const trellis_fingerprint_t *relay,
                                     const uint8_t *x25519_pk,
                                     const uint8_t *ml_kem_pk)
{
    if (!circuit || !relay || !ml_kem_pk)
        return TRELLIS_ERR_INVALID_ARG;

    if (circuit->hop_count >= circuit->max_hops)
        return TRELLIS_ERR_ONION;

    trellis_onion_hop_t *hop = &circuit->hops[circuit->hop_count];
    hop->fingerprint = *relay;

    trellis_kem_encaps_result_t encaps;
    trellis_err_t err = trellis_kem_encaps(x25519_pk, ml_kem_pk, &encaps);
    if (err != TRELLIS_OK)
        return err;

    memcpy(hop->shared_secret, encaps.shared_secret, 32);
    memcpy(hop->x25519_eph_pk, encaps.x25519_pk, TRELLIS_X25519_PK_LEN);

    hop->ml_kem_ct = malloc(TRELLIS_ML_KEM_1024_CT_LEN);
    if (!hop->ml_kem_ct) {
        sodium_memzero(hop->shared_secret, 32);
        return TRELLIS_ERR_NOMEM;
    }
    memcpy(hop->ml_kem_ct, encaps.ml_kem_ct, TRELLIS_ML_KEM_1024_CT_LEN);
    hop->ml_kem_ct_len = TRELLIS_ML_KEM_1024_CT_LEN;

    sodium_memzero(&encaps, sizeof(encaps));
    circuit->hop_count++;
    return TRELLIS_OK;
}

trellis_err_t trellis_onion_wrap(trellis_onion_circuit_t *circuit,
                                  const uint8_t *data, size_t len,
                                  trellis_buf_t *out)
{
    if (!circuit || !data || !out)
        return TRELLIS_ERR_INVALID_ARG;

    if (circuit->hop_count == 0)
        return TRELLIS_ERR_ONION;

    /*
     * Wrap from innermost hop to outermost.  Each layer:
     *   [next_hop_fingerprint (32)] [inner_payload (...)]
     * encrypted with AES-256-GCM using the hop's shared secret.
     */
    uint8_t *current = malloc(len);
    if (!current)
        return TRELLIS_ERR_NOMEM;
    memcpy(current, data, len);
    size_t current_len = len;

    for (size_t i = circuit->hop_count; i > 0; i--) {
        size_t idx = i - 1;
        trellis_onion_hop_t *hop = &circuit->hops[idx];

        size_t plain_len = TRELLIS_FINGERPRINT_LEN + current_len;
        uint8_t *plaintext = malloc(plain_len);
        if (!plaintext) {
            free(current);
            return TRELLIS_ERR_NOMEM;
        }

        if (idx + 1 < circuit->hop_count) {
            memcpy(plaintext, circuit->hops[idx + 1].fingerprint.bytes,
                   TRELLIS_FINGERPRINT_LEN);
        } else {
            memset(plaintext, 0, TRELLIS_FINGERPRINT_LEN);
        }
        memcpy(plaintext + TRELLIS_FINGERPRINT_LEN, current, current_len);
        free(current);

        /*
         * Layer format: [X25519 eph PK (32)] [ML-KEM CT] [nonce] [encrypted] [tag]
         * Both keys are needed so each relay can do hybrid decapsulation.
         */
        size_t header_len = TRELLIS_X25519_PK_LEN + TRELLIS_ML_KEM_1024_CT_LEN;
        size_t layer_len = header_len
                         + TRELLIS_AES_NONCE_LEN
                         + plain_len
                         + TRELLIS_AES_TAG_LEN;
        current = malloc(layer_len);
        if (!current) {
            free(plaintext);
            return TRELLIS_ERR_NOMEM;
        }

        memcpy(current, hop->x25519_eph_pk, TRELLIS_X25519_PK_LEN);
        memcpy(current + TRELLIS_X25519_PK_LEN, hop->ml_kem_ct,
               TRELLIS_ML_KEM_1024_CT_LEN);

        uint8_t nonce_input[37];
        memcpy(nonce_input, hop->shared_secret, 32);
        nonce_input[32] = (uint8_t)idx;
        uint32_t wc = circuit->wrap_counter;
        nonce_input[33] = (uint8_t)(wc);
        nonce_input[34] = (uint8_t)(wc >> 8);
        nonce_input[35] = (uint8_t)(wc >> 16);
        nonce_input[36] = (uint8_t)(wc >> 24);

        uint8_t nonce[TRELLIS_AES_NONCE_LEN];
        trellis_shake256(nonce_input, sizeof(nonce_input),
                         nonce, TRELLIS_AES_NONCE_LEN);

        memcpy(current + header_len, nonce, TRELLIS_AES_NONCE_LEN);

        uint8_t tag[TRELLIS_AES_TAG_LEN];
        trellis_err_t err = trellis_aes256gcm_encrypt(
            hop->shared_secret, nonce,
            plaintext, plain_len,
            NULL, 0,
            current + header_len + TRELLIS_AES_NONCE_LEN,
            tag);
        free(plaintext);

        if (err != TRELLIS_OK) {
            free(current);
            return err;
        }

        memcpy(current + header_len + TRELLIS_AES_NONCE_LEN + plain_len,
               tag, TRELLIS_AES_TAG_LEN);
        current_len = layer_len;
    }

    *out = trellis_buf_from(current, current_len);
    free(current);
    if (!out->data)
        return TRELLIS_ERR_NOMEM;

    circuit->wrap_counter++;
    return TRELLIS_OK;
}

trellis_err_t trellis_onion_peel(const trellis_kem_keypair_t *kp,
                                  const uint8_t *data, size_t len,
                                  trellis_buf_t *out,
                                  trellis_fingerprint_t *next_hop)
{
    if (!kp || !data || !out || !next_hop)
        return TRELLIS_ERR_INVALID_ARG;

    size_t header_len = TRELLIS_X25519_PK_LEN + TRELLIS_ML_KEM_1024_CT_LEN;
    size_t min_len = header_len
                   + TRELLIS_AES_NONCE_LEN
                   + TRELLIS_FINGERPRINT_LEN
                   + TRELLIS_AES_TAG_LEN;
    if (len < min_len)
        return TRELLIS_ERR_ONION;

    /*
     * Layer format: [X25519 eph PK (32)] [ML-KEM CT] [nonce] [encrypted] [tag]
     * 1. Hybrid decapsulate using both X25519 DH and ML-KEM.
     * 2. AES-256-GCM decrypt to get [next_hop_fingerprint || inner_payload].
     */
    const uint8_t *x25519_eph_pk = data;
    const uint8_t *kem_ct = data + TRELLIS_X25519_PK_LEN;
    const uint8_t *nonce = data + header_len;
    size_t enc_region = len - header_len
                            - TRELLIS_AES_NONCE_LEN
                            - TRELLIS_AES_TAG_LEN;
    const uint8_t *ct = nonce + TRELLIS_AES_NONCE_LEN;
    const uint8_t *tag = ct + enc_region;

    uint8_t shared_secret[32];
    trellis_err_t err = trellis_kem_decaps(kp, x25519_eph_pk, kem_ct,
                                            shared_secret);
    if (err != TRELLIS_OK)
        return TRELLIS_ERR_ONION;

    uint8_t *plaintext = malloc(enc_region);
    if (!plaintext) {
        sodium_memzero(shared_secret, sizeof(shared_secret));
        return TRELLIS_ERR_NOMEM;
    }

    err = trellis_aes256gcm_decrypt(
        shared_secret, nonce,
        ct, enc_region,
        NULL, 0,
        tag,
        plaintext);
    sodium_memzero(shared_secret, sizeof(shared_secret));

    if (err != TRELLIS_OK) {
        free(plaintext);
        return err;
    }

    if (enc_region < TRELLIS_FINGERPRINT_LEN) {
        free(plaintext);
        return TRELLIS_ERR_ONION;
    }

    memcpy(next_hop->bytes, plaintext, TRELLIS_FINGERPRINT_LEN);

    size_t payload_len = enc_region - TRELLIS_FINGERPRINT_LEN;
    *out = trellis_buf_from(plaintext + TRELLIS_FINGERPRINT_LEN, payload_len);
    free(plaintext);

    if (payload_len > 0 && !out->data)
        return TRELLIS_ERR_NOMEM;

    return TRELLIS_OK;
}

trellis_err_t trellis_onion_peel_key(const trellis_kem_keypair_t *kp,
                                      const uint8_t *data, size_t len,
                                      trellis_buf_t *out,
                                      trellis_fingerprint_t *next_hop,
                                      uint8_t layer_key_out[32])
{
    if (!kp || !data || !out || !next_hop)
        return TRELLIS_ERR_INVALID_ARG;

    size_t header_len = TRELLIS_X25519_PK_LEN + TRELLIS_ML_KEM_1024_CT_LEN;
    size_t min_len = header_len
                   + TRELLIS_AES_NONCE_LEN
                   + TRELLIS_FINGERPRINT_LEN
                   + TRELLIS_AES_TAG_LEN;
    if (len < min_len)
        return TRELLIS_ERR_ONION;

    const uint8_t *x25519_eph_pk = data;
    const uint8_t *kem_ct = data + TRELLIS_X25519_PK_LEN;
    const uint8_t *nonce = data + header_len;
    size_t enc_region = len - header_len
                            - TRELLIS_AES_NONCE_LEN
                            - TRELLIS_AES_TAG_LEN;
    const uint8_t *ct = nonce + TRELLIS_AES_NONCE_LEN;
    const uint8_t *tag = ct + enc_region;

    uint8_t shared_secret[32];
    trellis_err_t err = trellis_kem_decaps(kp, x25519_eph_pk, kem_ct,
                                            shared_secret);
    if (err != TRELLIS_OK)
        return TRELLIS_ERR_ONION;

    uint8_t *plaintext = malloc(enc_region);
    if (!plaintext) {
        sodium_memzero(shared_secret, sizeof(shared_secret));
        return TRELLIS_ERR_NOMEM;
    }

    err = trellis_aes256gcm_decrypt(
        shared_secret, nonce,
        ct, enc_region,
        NULL, 0,
        tag,
        plaintext);

    if (err != TRELLIS_OK) {
        sodium_memzero(shared_secret, sizeof(shared_secret));
        free(plaintext);
        return err;
    }

    // Output the layer key before zeroing.
    if (layer_key_out)
        memcpy(layer_key_out, shared_secret, 32);
    sodium_memzero(shared_secret, sizeof(shared_secret));

    if (enc_region < TRELLIS_FINGERPRINT_LEN) {
        free(plaintext);
        return TRELLIS_ERR_ONION;
    }

    memcpy(next_hop->bytes, plaintext, TRELLIS_FINGERPRINT_LEN);

    size_t payload_len = enc_region - TRELLIS_FINGERPRINT_LEN;
    *out = trellis_buf_from(plaintext + TRELLIS_FINGERPRINT_LEN, payload_len);
    free(plaintext);

    if (payload_len > 0 && !out->data)
        return TRELLIS_ERR_NOMEM;

    return TRELLIS_OK;
}
