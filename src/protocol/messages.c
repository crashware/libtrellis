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
#include <trellis/trellis.h>
#include <trellis/crypto.h>
#include <sodium.h>

static trellis_err_t serialize_header_payload(const trellis_message_t *msg,
                                              trellis_buf_t *out)
{
    size_t total = TRELLIS_MSG_HEADER_LEN + msg->payload_len;
    *out = trellis_buf_alloc(total);
    if (!out->data)
        return TRELLIS_ERR_NOMEM;

    uint8_t *p = out->data;

    p[0] = msg->version;
    p += 1;

    trellis_write_u16_be(p, msg->type);
    p += 2;

    p[0] = msg->flags;
    p += 1;

    trellis_write_u64_be(p, msg->sequence);
    p += 8;

    trellis_write_u64_be(p, msg->timestamp);
    p += 8;

    memcpy(p, msg->source.bytes, TRELLIS_FINGERPRINT_LEN);
    p += TRELLIS_FINGERPRINT_LEN;

    memcpy(p, msg->target.bytes, TRELLIS_FINGERPRINT_LEN);
    p += TRELLIS_FINGERPRINT_LEN;

    trellis_write_u32_be(p, msg->payload_len);
    p += 4;

    if (msg->payload_len > 0 && msg->payload)
        memcpy(p, msg->payload, msg->payload_len);

    out->len = total;
    return TRELLIS_OK;
}

trellis_err_t trellis_message_serialize(const trellis_message_t *msg,
                                        trellis_buf_t *out)
{
    if (!msg || !out)
        return TRELLIS_ERR_INVALID_ARG;

    size_t total = TRELLIS_MSG_HEADER_LEN + msg->payload_len +
                   TRELLIS_SIGNATURE_LEN;
    *out = trellis_buf_alloc(total);
    if (!out->data)
        return TRELLIS_ERR_NOMEM;

    uint8_t *p = out->data;

    p[0] = msg->version;
    p += 1;

    trellis_write_u16_be(p, msg->type);
    p += 2;

    p[0] = msg->flags;
    p += 1;

    trellis_write_u64_be(p, msg->sequence);
    p += 8;

    trellis_write_u64_be(p, msg->timestamp);
    p += 8;

    memcpy(p, msg->source.bytes, TRELLIS_FINGERPRINT_LEN);
    p += TRELLIS_FINGERPRINT_LEN;

    memcpy(p, msg->target.bytes, TRELLIS_FINGERPRINT_LEN);
    p += TRELLIS_FINGERPRINT_LEN;

    trellis_write_u32_be(p, msg->payload_len);
    p += 4;

    if (msg->payload_len > 0 && msg->payload)
        memcpy(p, msg->payload, msg->payload_len);
    p += msg->payload_len;

    memcpy(p, msg->signature.ed25519_sig, TRELLIS_ED25519_SIG_LEN);
    p += TRELLIS_ED25519_SIG_LEN;
    memcpy(p, msg->signature.ml_dsa_sig, TRELLIS_ML_DSA_87_SIG_LEN);

    out->len = total;
    return TRELLIS_OK;
}

trellis_err_t trellis_message_deserialize(const uint8_t *data, size_t len,
                                          trellis_message_t *msg)
{
    if (!data || !msg)
        return TRELLIS_ERR_INVALID_ARG;

    if (len < TRELLIS_MSG_HEADER_LEN)
        return TRELLIS_ERR_MSG_FORMAT;

    memset(msg, 0, sizeof(*msg));
    const uint8_t *p = data;

    msg->version = p[0];
    if (msg->version != TRELLIS_PROTOCOL_VERSION)
        return TRELLIS_ERR_MSG_FORMAT;
    p += 1;

    msg->type = trellis_read_u16_be(p);
    p += 2;

    msg->flags = p[0];
    p += 1;

    msg->sequence = trellis_read_u64_be(p);
    p += 8;

    msg->timestamp = trellis_read_u64_be(p);
    p += 8;

    memcpy(msg->source.bytes, p, TRELLIS_FINGERPRINT_LEN);
    p += TRELLIS_FINGERPRINT_LEN;

    memcpy(msg->target.bytes, p, TRELLIS_FINGERPRINT_LEN);
    p += TRELLIS_FINGERPRINT_LEN;

    msg->payload_len = trellis_read_u32_be(p);
    p += 4;

    #define TRELLIS_MSG_MAX_PAYLOAD (4u * 1024u * 1024u)
    if (msg->payload_len > TRELLIS_MSG_MAX_PAYLOAD)
        return TRELLIS_ERR_MSG_FORMAT;

    size_t remaining = len - (size_t)(p - data);
    if (remaining < (size_t)msg->payload_len + TRELLIS_SIGNATURE_LEN)
        return TRELLIS_ERR_MSG_FORMAT;

    if (msg->payload_len > 0) {
        msg->payload = malloc(msg->payload_len);
        if (!msg->payload)
            return TRELLIS_ERR_NOMEM;
        memcpy(msg->payload, p, msg->payload_len);
    }
    p += msg->payload_len;

    memcpy(msg->signature.ed25519_sig, p, TRELLIS_ED25519_SIG_LEN);
    p += TRELLIS_ED25519_SIG_LEN;
    memcpy(msg->signature.ml_dsa_sig, p, TRELLIS_ML_DSA_87_SIG_LEN);

    return TRELLIS_OK;
}

trellis_err_t trellis_message_sign(trellis_message_t *msg,
                                   const trellis_identity_t *id)
{
    if (!msg || !id)
        return TRELLIS_ERR_INVALID_ARG;

    trellis_buf_t buf = {0};
    trellis_err_t err = serialize_header_payload(msg, &buf);
    if (err != TRELLIS_OK)
        return err;

    err = trellis_identity_sign(id, buf.data, buf.len, &msg->signature);
    trellis_buf_free(&buf);
    return err;
}

trellis_err_t trellis_message_verify(const trellis_message_t *msg,
                                     const trellis_identity_public_t *pub)
{
    if (!msg || !pub)
        return TRELLIS_ERR_INVALID_ARG;

    trellis_buf_t buf = {0};
    trellis_err_t err = serialize_header_payload(msg, &buf);
    if (err != TRELLIS_OK)
        return err;

    err = trellis_identity_verify(pub, buf.data, buf.len, &msg->signature);
    trellis_buf_free(&buf);
    return err;
}

void trellis_message_free(trellis_message_t *msg)
{
    if (!msg)
        return;
    free(msg->payload);
    msg->payload = NULL;
    msg->payload_len = 0;
}

/*
 * Sealed prefix wire layout (1660 bytes):
 *   [0  ..  31]  ephemeral X25519 public key
 *   [32 .. 1599]  ML-KEM-1024 ciphertext
 *   [1600..1611]  AES-256-GCM nonce
 *   [1612..1643]  AES-256-GCM ciphertext of sender fingerprint (32 bytes)
 *   [1644..1659]  AES-256-GCM authentication tag
 */
#define SEALED_SENDER_PREFIX_LEN \
    (TRELLIS_X25519_PK_LEN + TRELLIS_ML_KEM_1024_CT_LEN + \
     TRELLIS_AES_NONCE_LEN + TRELLIS_FINGERPRINT_LEN + TRELLIS_AES_TAG_LEN)

static const char SEALED_INFO[] = "trellis-sealed-sender-v1";

trellis_err_t trellis_message_seal_sender(trellis_message_t *msg,
                                           const trellis_fingerprint_t *sender_fp,
                                           const trellis_identity_public_t *recipient_pub,
                                           trellis_buf_t *sealed_prefix_out)
{
    if (!msg || !sender_fp || !recipient_pub || !sealed_prefix_out)
        return TRELLIS_ERR_INVALID_ARG;

    // KEM encapsulate to derive shared secret.
    trellis_kem_encaps_result_t encaps;
    trellis_err_t err = trellis_kem_encaps(
        recipient_pub->x25519_pk,
        recipient_pub->ml_kem_pk,
        &encaps);
    if (err != TRELLIS_OK)
        return err;

    // Derive AES-256 key from shared secret via HKDF-SHAKE-256.
    uint8_t aes_key[TRELLIS_AES_KEY_LEN];
    err = trellis_hkdf_shake256(
        encaps.shared_secret, TRELLIS_ML_KEM_1024_SS_LEN,
        NULL, 0,
        (const uint8_t *)SEALED_INFO, sizeof(SEALED_INFO) - 1,
        aes_key, TRELLIS_AES_KEY_LEN);
    if (err != TRELLIS_OK) {
        sodium_memzero(&encaps, sizeof(encaps));
        return err;
    }

    // Random nonce.
    uint8_t nonce[TRELLIS_AES_NONCE_LEN];
    randombytes_buf(nonce, TRELLIS_AES_NONCE_LEN);

    // Encrypt the sender fingerprint.
    uint8_t ct[TRELLIS_FINGERPRINT_LEN];
    uint8_t tag[TRELLIS_AES_TAG_LEN];
    err = trellis_aes256gcm_encrypt(
        aes_key, nonce,
        sender_fp->bytes, TRELLIS_FINGERPRINT_LEN,
        NULL, 0,
        ct, tag);
    sodium_memzero(aes_key, sizeof(aes_key));
    sodium_memzero(&encaps.shared_secret, sizeof(encaps.shared_secret));
    if (err != TRELLIS_OK)
        return err;

    // Build the prefix buffer.
    *sealed_prefix_out = trellis_buf_alloc(SEALED_SENDER_PREFIX_LEN);
    if (!sealed_prefix_out->data)
        return TRELLIS_ERR_NOMEM;

    uint8_t *p = sealed_prefix_out->data;
    memcpy(p, encaps.x25519_pk, TRELLIS_X25519_PK_LEN);
    p += TRELLIS_X25519_PK_LEN;
    memcpy(p, encaps.ml_kem_ct, TRELLIS_ML_KEM_1024_CT_LEN);
    p += TRELLIS_ML_KEM_1024_CT_LEN;
    memcpy(p, nonce, TRELLIS_AES_NONCE_LEN);
    p += TRELLIS_AES_NONCE_LEN;
    memcpy(p, ct, TRELLIS_FINGERPRINT_LEN);
    p += TRELLIS_FINGERPRINT_LEN;
    memcpy(p, tag, TRELLIS_AES_TAG_LEN);
    sealed_prefix_out->len = SEALED_SENDER_PREFIX_LEN;

    // Zero the source field in the message header so routing hops see nothing.
    memset(msg->source.bytes, 0, TRELLIS_FINGERPRINT_LEN);
    msg->flags |= TRELLIS_FLAG_SEALED_SENDER;

    return TRELLIS_OK;
}

trellis_err_t trellis_message_unseal_sender(trellis_message_t *msg,
                                             const trellis_kem_keypair_t *recipient_kp,
                                             trellis_fingerprint_t *sender_fp_out)
{
    if (!msg || !recipient_kp || !sender_fp_out)
        return TRELLIS_ERR_INVALID_ARG;

    if (!(msg->flags & TRELLIS_FLAG_SEALED_SENDER))
        return TRELLIS_ERR_INVALID_ARG;

    if (!msg->payload || msg->payload_len < SEALED_SENDER_PREFIX_LEN)
        return TRELLIS_ERR_MSG_FORMAT;

    const uint8_t *p = msg->payload;

    const uint8_t *eph_x25519_pk = p;
    p += TRELLIS_X25519_PK_LEN;
    const uint8_t *ml_kem_ct = p;
    p += TRELLIS_ML_KEM_1024_CT_LEN;
    const uint8_t *nonce = p;
    p += TRELLIS_AES_NONCE_LEN;
    const uint8_t *enc_fp = p;
    p += TRELLIS_FINGERPRINT_LEN;
    const uint8_t *tag = p;

    // KEM decapsulate.
    uint8_t shared_secret[TRELLIS_ML_KEM_1024_SS_LEN];
    trellis_err_t err = trellis_kem_decaps(
        recipient_kp, eph_x25519_pk, ml_kem_ct, shared_secret);
    if (err != TRELLIS_OK)
        return err;

    // Derive AES key.
    uint8_t aes_key[TRELLIS_AES_KEY_LEN];
    err = trellis_hkdf_shake256(
        shared_secret, TRELLIS_ML_KEM_1024_SS_LEN,
        NULL, 0,
        (const uint8_t *)SEALED_INFO, sizeof(SEALED_INFO) - 1,
        aes_key, TRELLIS_AES_KEY_LEN);
    sodium_memzero(shared_secret, sizeof(shared_secret));
    if (err != TRELLIS_OK)
        return err;

    // Decrypt the fingerprint.
    err = trellis_aes256gcm_decrypt(
        aes_key, nonce,
        enc_fp, TRELLIS_FINGERPRINT_LEN,
        NULL, 0,
        tag,
        sender_fp_out->bytes);
    sodium_memzero(aes_key, sizeof(aes_key));
    if (err != TRELLIS_OK)
        return TRELLIS_ERR_DECRYPT;

    /* Advance payload past the sealed prefix so msg->payload now points
     * at the original plaintext. Note: payload is caller-managed; we just
     * update the pointer and length.  The caller must not free msg->payload
     * directly after this call — they should free the original pointer. */
    size_t remaining = msg->payload_len - SEALED_SENDER_PREFIX_LEN;
    if (remaining > 0) {
        memmove(msg->payload, msg->payload + SEALED_SENDER_PREFIX_LEN, remaining);
    }
    msg->payload_len = (uint32_t)remaining;
    msg->flags &= ~TRELLIS_FLAG_SEALED_SENDER;

    return TRELLIS_OK;
}
