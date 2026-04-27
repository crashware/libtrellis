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
 * identity_recovery.c — Key Management and Identity Recovery
 *
 * Provides three mechanisms for backing up and recovering Bloom identity keys:
 *
 * 1. Encrypted export (password-based):
 *    Identity keys are serialized, encrypted with Argon2id KDF + XSalsa20Poly1305,
 *    and saved to a portable blob.  Recovery requires only the password.
 *
 * 2. k-of-n Shamir Secret Sharing:
 *    The identity key material is split into n shares using Shamir's scheme
 *    over GF(2^8).  Any k shares can reconstruct the secret.  Shares are
 *    distributed to trusted custodians; loss of up to n-k shares is survivable.
 *
 * 3. SLH-DSA emergency re-keying ceremony:
 *    When the primary Ed25519 + ML-DSA keys are compromised, the node can
 *    sign a "key rotation" record with its long-term SLH-DSA backup key.
 *    This record is published to the KT log, informing the network of the
 *    new key set.  Verifiers who trust the SLH-DSA key will update their
 *    TOFU records.
 *
 * Encrypted export format:
 *   [8]  magic "BLIDEXPT"
 *   [1]  version (1)
 *   [32] salt (random, for Argon2id)
 *   [24] nonce (XSalsa20Poly1305)
 *   [4]  plaintext_len (LE)
 *   [N+16] ciphertext (plaintext + MAC)
 *
 * Shamir share format:
 *   [4]  magic "BSHR"
 *   [1]  version (1)
 *   [1]  share_index (1-based)
 *   [1]  k (threshold)
 *   [1]  n (total shares)
 *   [32] identity fingerprint (so custodian knows whose share this is)
 *   [M]  share data (same length as secret)
 *
 * Key rotation record format:
 *   [4]  magic "KROT"
 *   [1]  version (1)
 *   [32] old fingerprint
 *   [32] new fingerprint
 *   [8]  timestamp_ms
 *   --- New identity public keys ---
 *   [32]  ed25519_pk
 *   [2592] ml_dsa_pk
 *   [32]  x25519_pk
 *   [1568] ml_kem_pk
 *   --- SLH-DSA signature over all above ---
 *   [29792] slh_dsa_sig
 */

#include "internal.h"
#include <trellis/identity.h>
#include <trellis/error.h>
#include <sodium.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <time.h>
#include <oqs/oqs.h>

#define EXPORT_MAGIC          "BLIDEXPT"
#define EXPORT_VERSION        1
#define EXPORT_SALT_LEN       32
#define EXPORT_NONCE_LEN      crypto_secretbox_NONCEBYTES  /* 24 */
#define EXPORT_MAC_LEN        crypto_secretbox_MACBYTES    /* 16 */
#define EXPORT_HEADER_LEN     (8 + 1 + EXPORT_SALT_LEN + EXPORT_NONCE_LEN + 4)

#define SHAMIR_MAGIC          "BSHR"
#define SHAMIR_SHARE_HDR_LEN  (4 + 1 + 1 + 1 + 1 + 32)

#define KROT_MAGIC            "KROT"

// Argon2id parameters for export key derivation.
#define ARGON2_OPS_LIMIT      crypto_pwhash_OPSLIMIT_SENSITIVE
#define ARGON2_MEM_LIMIT      crypto_pwhash_MEMLIMIT_SENSITIVE
#define ARGON2_ALG            crypto_pwhash_ALG_ARGON2ID13

// Maximum identity serialization size (fingerprint + all key material)
#define IDENTITY_SERIAL_MAX   (TRELLIS_FINGERPRINT_LEN + \
                               TRELLIS_ED25519_PK_LEN  + TRELLIS_ED25519_SK_LEN  + \
                               TRELLIS_ML_DSA_87_PK_LEN + TRELLIS_ML_DSA_87_SK_LEN + \
                               TRELLIS_X25519_PK_LEN   + TRELLIS_X25519_SK_LEN   + \
                               TRELLIS_ML_KEM_1024_PK_LEN + TRELLIS_ML_KEM_1024_SK_LEN + \
                               TRELLIS_SLH_DSA_PK_LEN  + TRELLIS_SLH_DSA_SK_LEN)

static size_t identity_serialize(const trellis_identity_t *id,
                                  uint8_t *buf, size_t cap)
{
    // Serialize all key material in a fixed order.
    size_t required = sizeof(id->ed25519_pk) + sizeof(id->ed25519_sk)
                    + sizeof(id->ml_dsa_pk)  + sizeof(id->ml_dsa_sk)
                    + sizeof(id->x25519_pk)  + sizeof(id->x25519_sk)
                    + sizeof(id->ml_kem_pk)  + sizeof(id->ml_kem_sk)
                    + sizeof(id->slh_dsa_pk) + sizeof(id->slh_dsa_sk)
                    + TRELLIS_FINGERPRINT_LEN;
    if (cap < required) return 0;

    size_t off = 0;
#define COPY(field) do { memcpy(buf + off, id->field, sizeof(id->field)); off += sizeof(id->field); } while(0)
    COPY(fingerprint.bytes);
    COPY(ed25519_pk); COPY(ed25519_sk);
    COPY(ml_dsa_pk);  COPY(ml_dsa_sk);
    COPY(x25519_pk);  COPY(x25519_sk);
    COPY(ml_kem_pk);  COPY(ml_kem_sk);
    COPY(slh_dsa_pk); COPY(slh_dsa_sk);
#undef COPY
    return off;
}

static bool identity_deserialize(const uint8_t *buf, size_t len,
                                   trellis_identity_t *id)
{
    size_t required = sizeof(id->ed25519_pk) + sizeof(id->ed25519_sk)
                    + sizeof(id->ml_dsa_pk)  + sizeof(id->ml_dsa_sk)
                    + sizeof(id->x25519_pk)  + sizeof(id->x25519_sk)
                    + sizeof(id->ml_kem_pk)  + sizeof(id->ml_kem_sk)
                    + sizeof(id->slh_dsa_pk) + sizeof(id->slh_dsa_sk)
                    + TRELLIS_FINGERPRINT_LEN;
    if (len < required) return false;

    size_t off = 0;
#define COPY(field) do { memcpy(id->field, buf + off, sizeof(id->field)); off += sizeof(id->field); } while(0)
    COPY(fingerprint.bytes);
    COPY(ed25519_pk); COPY(ed25519_sk);
    COPY(ml_dsa_pk);  COPY(ml_dsa_sk);
    COPY(x25519_pk);  COPY(x25519_sk);
    COPY(ml_kem_pk);  COPY(ml_kem_sk);
    COPY(slh_dsa_pk); COPY(slh_dsa_sk);
#undef COPY
    return true;
}

// --- 1. Password-protected encrypted export ---

/*
 * Export an identity to an encrypted blob.
 *
 * @id            identity to export
 * @password      password for key derivation (UTF-8)
 * @password_len  length of password
 * @out           output buffer (caller must free)
 * @out_len       output length
 */
trellis_err_t trellis_identity_export(
        const trellis_identity_t *id,
        const char *password, size_t password_len,
        uint8_t **out, size_t *out_len)
{
    if (!id || !password || !out || !out_len)
        return TRELLIS_ERR_INVALID_ARG;

    // Serialize identity.
    uint8_t plaintext[IDENTITY_SERIAL_MAX];
    size_t plain_len = identity_serialize(id, plaintext, sizeof(plaintext));
    if (plain_len == 0) return TRELLIS_ERR_CRYPTO_INIT;

    // Derive symmetric key from password.
    uint8_t salt[EXPORT_SALT_LEN];
    randombytes_buf(salt, sizeof(salt));

    uint8_t sym_key[crypto_secretbox_KEYBYTES];
    if (crypto_pwhash(sym_key, sizeof(sym_key),
                      password, password_len,
                      salt,
                      ARGON2_OPS_LIMIT, ARGON2_MEM_LIMIT, ARGON2_ALG) != 0) {
        sodium_memzero(plaintext, sizeof(plaintext));
        return TRELLIS_ERR_CRYPTO_INIT;
    }

    // Nonce
    uint8_t nonce[EXPORT_NONCE_LEN];
    randombytes_buf(nonce, sizeof(nonce));

    // Encrypt
    size_t ct_len = plain_len + EXPORT_MAC_LEN;
    uint8_t *ciphertext = malloc(ct_len);
    if (!ciphertext) {
        sodium_memzero(plaintext, sizeof(plaintext));
        return TRELLIS_ERR_NOMEM;
    }

    if (crypto_secretbox_easy(ciphertext, plaintext, plain_len,
                               nonce, sym_key) != 0) {
        free(ciphertext);
        sodium_memzero(plaintext, sizeof(plaintext));
        return TRELLIS_ERR_ENCRYPT;
    }
    sodium_memzero(plaintext, sizeof(plaintext));
    sodium_memzero(sym_key, sizeof(sym_key));

    // Build export blob.
    size_t blob_len = EXPORT_HEADER_LEN + ct_len;
    uint8_t *blob = malloc(blob_len);
    if (!blob) { free(ciphertext); return TRELLIS_ERR_NOMEM; }

    size_t off = 0;
    memcpy(blob + off, EXPORT_MAGIC, 8);  off += 8;
    blob[off++] = EXPORT_VERSION;
    memcpy(blob + off, salt, EXPORT_SALT_LEN);   off += EXPORT_SALT_LEN;
    memcpy(blob + off, nonce, EXPORT_NONCE_LEN); off += EXPORT_NONCE_LEN;
    for (int b = 0; b < 4; b++)
        blob[off++] = (uint8_t)(plain_len >> (b * 8));
    memcpy(blob + off, ciphertext, ct_len); off += ct_len;
    free(ciphertext);

    *out     = blob;
    *out_len = off;
    return TRELLIS_OK;
}

/*
 * Import an identity from an encrypted export blob.
 */
trellis_err_t trellis_identity_import(
        const uint8_t *blob, size_t blob_len,
        const char *password, size_t password_len,
        trellis_identity_t *id_out)
{
    if (!blob || !password || !id_out)
        return TRELLIS_ERR_INVALID_ARG;
    if (blob_len < EXPORT_HEADER_LEN)
        return TRELLIS_ERR_MSG_FORMAT;
    if (memcmp(blob, EXPORT_MAGIC, 8) != 0 || blob[8] != EXPORT_VERSION)
        return TRELLIS_ERR_MSG_FORMAT;

    size_t off = 9;
    const uint8_t *salt  = blob + off; off += EXPORT_SALT_LEN;
    const uint8_t *nonce = blob + off; off += EXPORT_NONCE_LEN;
    uint32_t plain_len = 0;
    for (int b = 0; b < 4; b++)
        plain_len |= ((uint32_t)blob[off + b]) << (b * 8);
    off += 4;

    if (blob_len < off + plain_len + EXPORT_MAC_LEN)
        return TRELLIS_ERR_MSG_FORMAT;

    // Re-derive key.
    uint8_t sym_key[crypto_secretbox_KEYBYTES];
    if (crypto_pwhash(sym_key, sizeof(sym_key),
                      password, password_len,
                      salt,
                      ARGON2_OPS_LIMIT, ARGON2_MEM_LIMIT, ARGON2_ALG) != 0)
        return TRELLIS_ERR_CRYPTO_INIT;

    // Decrypt
    uint8_t *plaintext = malloc(plain_len);
    if (!plaintext) return TRELLIS_ERR_NOMEM;

    if (crypto_secretbox_open_easy(plaintext, blob + off,
                                    plain_len + EXPORT_MAC_LEN,
                                    nonce, sym_key) != 0) {
        free(plaintext);
        sodium_memzero(sym_key, sizeof(sym_key));
        return TRELLIS_ERR_DECRYPT;
    }
    sodium_memzero(sym_key, sizeof(sym_key));

    bool ok = identity_deserialize(plaintext, plain_len, id_out);
    sodium_memzero(plaintext, plain_len);
    free(plaintext);

    return ok ? TRELLIS_OK : TRELLIS_ERR_MSG_FORMAT;
}

// --- 2. Shamir Secret Sharing ---

// GF(2^8) with polynomial x^8 + x^4 + x^3 + x + 1 (0x11b)
static uint8_t gf_mul(uint8_t a, uint8_t b)
{
    uint8_t p = 0;
    for (int i = 0; i < 8; i++) {
        if (b & 1) p ^= a;
        uint8_t carry = a & 0x80;
        a <<= 1;
        if (carry) a ^= 0x1b;
        b >>= 1;
    }
    return p;
}

static uint8_t gf_pow(uint8_t base, uint8_t exp)
{
    uint8_t result = 1;
    for (uint8_t i = 0; i < exp; i++) result = gf_mul(result, base);
    return result;
}

static uint8_t gf_inv(uint8_t a)
{
    // Fermat's little theorem: a^254 = a^(-1) in GF(2^8)
    return gf_pow(a, 254);
}

/*
 * Split secret into n shares, any k sufficient to reconstruct.
 *
 * @secret       secret bytes to split
 * @secret_len   length of secret
 * @k            threshold
 * @n            total shares
 * @id           identity (for fingerprint in share headers)
 * @shares_out   array of n pointers; caller allocates n pointers,
 *               function allocates each share blob
 * @share_lens   lengths of each share blob
 */
trellis_err_t trellis_identity_split_secret(
        const uint8_t *secret, size_t secret_len,
        int k, int n,
        const trellis_fingerprint_t *fp,
        uint8_t **shares_out, size_t *share_lens)
{
    if (!secret || !shares_out || !share_lens || k < 2 || k > n || n > 255)
        return TRELLIS_ERR_INVALID_ARG;

    /*
     * For each byte of secret, create a polynomial of degree k-1.
     * Evaluate at x = 1..n.
     * Share i = (i, polynomial(i)) for each byte.
     */
    for (int i = 0; i < n; i++) {
        size_t share_len = SHAMIR_SHARE_HDR_LEN + secret_len;
        shares_out[i] = malloc(share_len);
        if (!shares_out[i]) {
            for (int j = 0; j < i; j++) free(shares_out[j]);
            return TRELLIS_ERR_NOMEM;
        }
        share_lens[i] = share_len;

        uint8_t *sh = shares_out[i];
        memcpy(sh, SHAMIR_MAGIC, 4);
        sh[4] = 1;                     /* version */
        sh[5] = (uint8_t)(i + 1);      /* 1-based share index */
        sh[6] = (uint8_t)k;
        sh[7] = (uint8_t)n;
        if (fp) memcpy(sh + 8, fp->bytes, 32);
        else memset(sh + 8, 0, 32);

        uint8_t *share_data = sh + SHAMIR_SHARE_HDR_LEN;
        uint8_t x = (uint8_t)(i + 1);

        for (size_t byte_idx = 0; byte_idx < secret_len; byte_idx++) {
            // Polynomial coefficients: a[0] = secret[byte_idx], a[1..k-1] = random.
            uint8_t coeffs[255];
            coeffs[0] = secret[byte_idx];
            randombytes_buf(coeffs + 1, k - 1);

            // Evaluate polynomial at x.
            uint8_t y = 0;
            uint8_t x_pow = 1;
            for (int c = 0; c < k; c++) {
                y ^= gf_mul(coeffs[c], x_pow);
                x_pow = gf_mul(x_pow, x);
            }
            share_data[byte_idx] = y;
            sodium_memzero(coeffs, sizeof(coeffs));
        }
    }
    return TRELLIS_OK;
}

/*
 * Reconstruct the secret from k shares using Lagrange interpolation.
 *
 * @shares       array of k share blobs
 * @share_lens   lengths of each share
 * @k            number of shares provided
 * @secret_out   output buffer (must be at least secret_len bytes)
 * @secret_len   length of the secret
 */
trellis_err_t trellis_identity_reconstruct_secret(
        const uint8_t **shares, const size_t *share_lens,
        int k,
        uint8_t *secret_out, size_t secret_len)
{
    if (!shares || !share_lens || !secret_out || k < 2)
        return TRELLIS_ERR_INVALID_ARG;

    // Validate headers and collect x values and share data.
    uint8_t xs[255];
    const uint8_t *ys[255];

    for (int i = 0; i < k; i++) {
        if (share_lens[i] < SHAMIR_SHARE_HDR_LEN + secret_len)
            return TRELLIS_ERR_MSG_FORMAT;
        if (memcmp(shares[i], SHAMIR_MAGIC, 4) != 0)
            return TRELLIS_ERR_MSG_FORMAT;
        xs[i]  = shares[i][5]; /* share index */
        ys[i]  = shares[i] + SHAMIR_SHARE_HDR_LEN;
    }

    // Lagrange interpolation at x=0 (the secret)
    memset(secret_out, 0, secret_len);
    for (size_t byte_idx = 0; byte_idx < secret_len; byte_idx++) {
        uint8_t secret_byte = 0;
        for (int i = 0; i < k; i++) {
            // L_i(0) = prod_{j≠i} (0 - x_j) / (x_i - x_j)
            uint8_t num = 1, den = 1;
            for (int j = 0; j < k; j++) {
                if (j == i) continue;
                num = gf_mul(num, xs[j]);             /* 0 - x_j = x_j in GF(2^8) */
                den = gf_mul(den, xs[i] ^ xs[j]);     /* x_i - x_j in GF(2^8) */
            }
            uint8_t lagrange = gf_mul(num, gf_inv(den));
            secret_byte ^= gf_mul(ys[i][byte_idx], lagrange);
        }
        secret_out[byte_idx] = secret_byte;
    }
    return TRELLIS_OK;
}

// --- 3. SLH-DSA emergency re-keying ---

/*
 * Create an emergency key rotation record signed with the SLH-DSA backup key.
 *
 * This record, when published to the KT log, announces that:
 *   "The node with fingerprint <old_fp> has new keys <new_id>, verified
 *    by the SLH-DSA backup key."
 *
 * @old_id    the compromised identity (SLH-DSA key must be intact)
 * @new_id    the new replacement identity
 * @out       output buffer (caller must free)
 * @out_len   length of output
 */
trellis_err_t trellis_identity_emergency_rekey(
        const trellis_identity_t *old_id,
        const trellis_identity_t *new_id,
        uint8_t **out, size_t *out_len)
{
    if (!old_id || !new_id || !out || !out_len)
        return TRELLIS_ERR_INVALID_ARG;

    // Build the to-be-signed record body.
    size_t body_len = 4 + 1 + 32 + 32 + 8
                    + TRELLIS_ED25519_PK_LEN
                    + TRELLIS_ML_DSA_87_PK_LEN
                    + TRELLIS_X25519_PK_LEN
                    + TRELLIS_ML_KEM_1024_PK_LEN;

    uint8_t *body = malloc(body_len);
    if (!body) return TRELLIS_ERR_NOMEM;

    size_t off = 0;
    memcpy(body + off, KROT_MAGIC, 4);                            off += 4;
    body[off++] = 1; /* version */
    memcpy(body + off, old_id->fingerprint.bytes, 32);            off += 32;
    memcpy(body + off, new_id->fingerprint.bytes, 32);            off += 32;

    uint64_t ts_ms = (uint64_t)time(NULL) * 1000;
    for (int b = 0; b < 8; b++) body[off++] = (uint8_t)(ts_ms >> (b * 8));

    memcpy(body + off, new_id->ed25519_pk, TRELLIS_ED25519_PK_LEN);
    off += TRELLIS_ED25519_PK_LEN;
    memcpy(body + off, new_id->ml_dsa_pk, TRELLIS_ML_DSA_87_PK_LEN);
    off += TRELLIS_ML_DSA_87_PK_LEN;
    memcpy(body + off, new_id->x25519_pk, TRELLIS_X25519_PK_LEN);
    off += TRELLIS_X25519_PK_LEN;
    memcpy(body + off, new_id->ml_kem_pk, TRELLIS_ML_KEM_1024_PK_LEN);
    off += TRELLIS_ML_KEM_1024_PK_LEN;

    // Sign with SLH-DSA backup key.
    OQS_SIG *slh = OQS_SIG_new(OQS_SIG_alg_sphincs_shake_256s_simple);
    if (!slh) { free(body); return TRELLIS_ERR_CRYPTO_INIT; }

    size_t sig_len  = slh->length_signature;
    uint8_t *record = malloc(body_len + sig_len);
    if (!record) {
        OQS_SIG_free(slh);
        free(body);
        return TRELLIS_ERR_NOMEM;
    }

    memcpy(record, body, body_len);

    size_t actual_sig_len;
    if (OQS_SIG_sign(slh, record + body_len, &actual_sig_len,
                     body, body_len, old_id->slh_dsa_sk) != OQS_SUCCESS) {
        OQS_SIG_free(slh);
        free(body);
        free(record);
        return TRELLIS_ERR_SIGN_FAILED;
    }
    OQS_SIG_free(slh);
    free(body);

    *out     = record;
    *out_len = body_len + actual_sig_len;
    return TRELLIS_OK;
}

/*
 * Verify an emergency re-keying record.
 * Returns TRELLIS_OK if the SLH-DSA signature is valid.
 *
 * @record       the re-keying record bytes
 * @record_len   length
 * @slh_dsa_pk   the SLH-DSA public key of the original identity
 * @new_fp_out   (optional) fingerprint of the new identity
 */
trellis_err_t trellis_identity_verify_rekey(
        const uint8_t *record, size_t record_len,
        const uint8_t *slh_dsa_pk,
        trellis_fingerprint_t *new_fp_out)
{
    if (!record || !slh_dsa_pk)
        return TRELLIS_ERR_INVALID_ARG;

    size_t min_body = 4 + 1 + 32 + 32 + 8
                    + TRELLIS_ED25519_PK_LEN
                    + TRELLIS_ML_DSA_87_PK_LEN
                    + TRELLIS_X25519_PK_LEN
                    + TRELLIS_ML_KEM_1024_PK_LEN;

    if (record_len <= min_body)
        return TRELLIS_ERR_MSG_FORMAT;
    if (memcmp(record, KROT_MAGIC, 4) != 0 || record[4] != 1)
        return TRELLIS_ERR_MSG_FORMAT;

    if (new_fp_out)
        memcpy(new_fp_out->bytes, record + 4 + 1 + 32, 32);

    OQS_SIG *slh = OQS_SIG_new(OQS_SIG_alg_sphincs_shake_256s_simple);
    if (!slh) return TRELLIS_ERR_CRYPTO_INIT;

    size_t sig_len = record_len - min_body;
    int rv = OQS_SIG_verify(slh,
                              record, min_body,
                              record + min_body, sig_len,
                              slh_dsa_pk);
    OQS_SIG_free(slh);

    return (rv == OQS_SUCCESS) ? TRELLIS_OK : TRELLIS_ERR_VERIFY_FAILED;
}
