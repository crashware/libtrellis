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

#ifdef TRELLIS_WITH_TLS

#include <mbedtls/ssl.h>
#include <mbedtls/entropy.h>
#include <mbedtls/ctr_drbg.h>
#include <mbedtls/x509_crt.h>
#include <mbedtls/pk.h>
#include <mbedtls/error.h>
#include <mbedtls/net_sockets.h>
#include <mbedtls/version.h>

#include <sodium.h>

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define ECH_EXT_TYPE           0xFE0Du   /* draft value; track the final RFC assignment */
#define ECH_TYPE_OUTER         0u
#define ECH_KEM_ID             0x0020u   /* DHKEM(X25519, HKDF-SHA256) */
#define ECH_KDF_ID             0x0001u   /* HKDF-SHA256 */
#define ECH_AEAD_ID            0x0003u   /* ChaCha20Poly1305 */
#define ECH_ENC_LEN            32u       /* X25519 public key length */
#define ECH_PAYLOAD_MIN_LEN    64u       /* minimum encrypted inner CH size */
// Maximum ECH extension payload stored per-connection.
#define ECH_EXT_BUF_MAX        512u

// --- Minimal HPKE for ECH (DHKEM-X25519-HKDF-SHA256-ChaCha20Poly1305) ---

/*
 * HKDF-SHA256 Extract: PRK = HMAC-SHA256(salt, ikm)
 * salt = 0-length means use SHA256-hash-length zero bytes per RFC 5869.
 */
static void ech_hkdf_extract(const uint8_t *salt, size_t salt_len,
                              const uint8_t *ikm,  size_t ikm_len,
                              uint8_t prk[32])
{
    uint8_t zero_salt[32] = {0};
    if (!salt || salt_len == 0) {
        salt     = zero_salt;
        salt_len = 32;
    }
    crypto_auth_hmacsha256_state st;
    crypto_auth_hmacsha256_init(&st, salt, salt_len);
    crypto_auth_hmacsha256_update(&st, ikm, ikm_len);
    crypto_auth_hmacsha256_final(&st, prk);
}

/*
 * HKDF-SHA256 Expand: OKM of length `len` bytes
 * Uses T(1) = HMAC-SHA256(PRK, info || 0x01)
 * T(2) = HMAC-SHA256(PRK, T(1) || info || 0x02) etc.
 * Only outputs up to 32 bytes (single T(1) block) for our KEM usage.
 */
static void ech_hkdf_expand(const uint8_t prk[32],
                             const uint8_t *info, size_t info_len,
                             uint8_t *okm,  size_t len)
{
    uint8_t counter = 1;
    uint8_t t[32];
    uint8_t prev_len = 0;

    for (size_t offset = 0; offset < len; ) {
        crypto_auth_hmacsha256_state st;
        crypto_auth_hmacsha256_init(&st, prk, 32);
        if (prev_len > 0)
            crypto_auth_hmacsha256_update(&st, t, prev_len);
        crypto_auth_hmacsha256_update(&st, info, info_len);
        crypto_auth_hmacsha256_update(&st, &counter, 1);
        crypto_auth_hmacsha256_final(&st, t);
        prev_len = 32;

        size_t copy = len - offset;
        if (copy > 32) copy = 32;
        memcpy(okm + offset, t, copy);
        offset += copy;
        counter++;
    }
    sodium_memzero(t, sizeof(t));
}

/*
 * HPKE LabeledExtract:
 *   HKDF-Extract(salt, "HPKE-v1" || suite_id || label || ikm)
 */
static void ech_labeled_extract(const uint8_t *salt,    size_t salt_len,
                                 const uint8_t *suite_id, size_t suite_id_len,
                                 const char    *label,
                                 const uint8_t *ikm,     size_t ikm_len,
                                 uint8_t prk[32])
{
    static const uint8_t hpkev1[] = "HPKE-v1";
    uint8_t buf[256];
    size_t  off = 0;
    memcpy(buf + off, hpkev1, 7);    off += 7;
    memcpy(buf + off, suite_id, suite_id_len); off += suite_id_len;
    size_t llen = strlen(label);
    memcpy(buf + off, label, llen);  off += llen;
    memcpy(buf + off, ikm, ikm_len); off += ikm_len;
    ech_hkdf_extract(salt, salt_len, buf, off, prk);
    sodium_memzero(buf, sizeof(buf));
}

/*
 * HPKE LabeledExpand:
 *   HKDF-Expand(prk, "HPKE-v1" || suite_id || label || info, len)
 */
static void ech_labeled_expand(const uint8_t *prk,
                                const uint8_t *suite_id, size_t suite_id_len,
                                const char    *label,
                                const uint8_t *info,     size_t info_len,
                                uint8_t *okm, size_t len)
{
    static const uint8_t hpkev1[] = "HPKE-v1";
    uint8_t buf[512];
    size_t  off = 0;
    // 2-byte big-endian length prefix.
    buf[off++] = (uint8_t)(len >> 8);
    buf[off++] = (uint8_t)(len & 0xFF);
    memcpy(buf + off, hpkev1, 7);    off += 7;
    memcpy(buf + off, suite_id, suite_id_len); off += suite_id_len;
    size_t llen = strlen(label);
    memcpy(buf + off, label, llen);  off += llen;
    if (info && info_len)
        memcpy(buf + off, info, info_len);
    off += info_len;
    ech_hkdf_expand(prk, buf, off, okm, len);
    sodium_memzero(buf, sizeof(buf));
}

/*
 * DHKEM(X25519, HKDF-SHA256): encapsulate.
 * Given server public key `pk_r` (32 bytes), outputs:
 *   enc    (32 bytes) – ephemeral sender public key
 *   ss     (32 bytes) – HPKE shared secret
 *
 * Returns 0 on success, -1 on failure.
 */
static int ech_kem_encap(mbedtls_ctr_drbg_context *drbg,
                          const uint8_t  pk_r[32],
                          uint8_t        enc[32],   /* out: eph pubkey */
                          uint8_t        ss[32])    /* out: shared secret */
{
    // KEM suite_id = "KEM" || I2OSP(0x0020, 2)
    static const uint8_t kem_suite[] = {
        'K','E','M', 0x00, 0x20
    };

    // Generate ephemeral keypair.
    uint8_t eph_sk[32];
    if (mbedtls_ctr_drbg_random(drbg, eph_sk, 32) != 0)
        return -1;
    crypto_scalarmult_curve25519_base(enc, eph_sk); /* enc = eph_pk */

    // DH shared value: dh = X25519(eph_sk, pk_r)
    uint8_t dh[32];
    if (crypto_scalarmult_curve25519(dh, eph_sk, pk_r) != 0) {
        sodium_memzero(eph_sk, 32);
        return -1;
    }

    // kem_context = enc || pk_r
    uint8_t kem_ctx[64];
    memcpy(kem_ctx,      enc,  32);
    memcpy(kem_ctx + 32, pk_r, 32);

    // ExtractAndExpand
    uint8_t prk[32];
    ech_labeled_extract(NULL, 0, kem_suite, sizeof(kem_suite),
                        "shared_secret", dh, 32, prk);
    ech_labeled_expand(prk, kem_suite, sizeof(kem_suite),
                       "shared_secret", kem_ctx, 64, ss, 32);

    sodium_memzero(eph_sk, 32);
    sodium_memzero(dh,     32);
    sodium_memzero(prk,    32);
    return 0;
}

/*
 * HPKE Setup Base sender:
 * Returns key (32 bytes) and nonce (12 bytes) for ChaCha20Poly1305 AEAD.
 *
 * info = the HPKE "info" string (for ECH: "tls ech\0" || config_id)
 */
static void ech_hpke_setup_base(const uint8_t *ss, size_t ss_len,
                                 const uint8_t *enc, size_t enc_len,
                                 const uint8_t *info, size_t info_len,
                                 uint8_t key[32], uint8_t nonce[12])
{
    // HPKE full suite_id: "HPKE" || kem || kdf || aead.
    static const uint8_t suite_id[] = {
        'H','P','K','E',
        0x00, 0x20,  /* KEM = DHKEM-X25519 */
        0x00, 0x01,  /* KDF = HKDF-SHA256 */
        0x00, 0x03   /* AEAD = ChaCha20Poly1305 */
    };

    // mode_id = 0 (Base)
    static const uint8_t mode_base[1] = {0};

    // ks_context = mode_id || KEM-extracted shared_secret || psk_id_hash || info_hash
    // For Base mode: psk = "", psk_id = "".
    uint8_t prk[32], psk_id_hash[32], info_hash[32];

    // psk_id_hash = LabeledExtract("", "psk_id_hash", "")
    uint8_t empty = 0;
    ech_labeled_extract(NULL, 0, suite_id, sizeof(suite_id),
                        "psk_id_hash", &empty, 0, psk_id_hash);
    // info_hash = LabeledExtract("", "info_hash", info)
    ech_labeled_extract(NULL, 0, suite_id, sizeof(suite_id),
                        "info_hash", info, info_len, info_hash);

    // ks_context = mode_id || ss || psk_id_hash || info_hash
    uint8_t ks_ctx[1 + 32 + 32 + 32];
    memcpy(ks_ctx,      mode_base,   1);
    memcpy(ks_ctx + 1,  ss,          ss_len < 32 ? ss_len : 32);
    memcpy(ks_ctx + 33, psk_id_hash, 32);
    memcpy(ks_ctx + 65, info_hash,   32);

    // secret = LabeledExtract(ss, "secret", "")
    ech_labeled_extract(ss, ss_len, suite_id, sizeof(suite_id),
                        "secret", &empty, 0, prk);

    // key = LabeledExpand(prk, "key", ks_ctx, Nk=32)
    ech_labeled_expand(prk, suite_id, sizeof(suite_id),
                       "key", ks_ctx, sizeof(ks_ctx), key, 32);
    // nonce = LabeledExpand(prk, "base_nonce", ks_ctx, Nn=12)
    ech_labeled_expand(prk, suite_id, sizeof(suite_id),
                       "base_nonce", ks_ctx, sizeof(ks_ctx), nonce, 12);

    sodium_memzero(prk,         32);
    sodium_memzero(psk_id_hash, 32);
    sodium_memzero(info_hash,   32);
    sodium_memzero(ks_ctx,      sizeof(ks_ctx));
}

/*
 * Parse an ECHConfigList to find the first supported entry.
 * ECHConfigList = opaque<1..2^16-1> of ECHConfig entries.
 * ECHConfig = { version: uint16, contents: opaque<1..2^16-1> }
 * ECHConfigContents = { kem_id, kdf_id, aead_id, config_id, pk<1..2^16-1>,
 *                        max_name_len, public_name<1..255>, extensions<0..2^16-1> }
 *
 * Outputs config_id (1 byte) and pk (32 bytes for X25519).
 * Returns 0 on success, -1 if no supported entry found.
 */
static int ech_parse_config_list(const uint8_t *cl, size_t cl_len,
                                  uint8_t *out_config_id,
                                  uint8_t out_pk[32])
{
    if (cl_len < 2) return -1;
    size_t list_len = ((size_t)cl[0] << 8) | cl[1];
    if (list_len + 2 > cl_len) return -1;
    size_t off = 2;
    size_t end = off + list_len;

    while (off + 4 <= end) {
        uint16_t ver = ((uint16_t)cl[off] << 8) | cl[off + 1];
        off += 2;
        uint16_t clen = ((uint16_t)cl[off] << 8) | cl[off + 1];
        off += 2;
        size_t entry_end = off + clen;
        if (entry_end > end) break;

        if (ver != 0xFE0Du || clen < 12) { /* 0xFE0D = ECH draft-18 */
            off = entry_end;
            continue;
        }

        /* contents: kem_id(2) kdf_id(2) aead_id(2) config_id(1)
         *           pk_len(2) pk(...)  max_name_len(1)  public_name(...)  exts(...) */
        uint16_t kem  = ((uint16_t)cl[off] << 8) | cl[off + 1]; off += 2;
        uint16_t kdf  = ((uint16_t)cl[off] << 8) | cl[off + 1]; off += 2;
        uint16_t aead = ((uint16_t)cl[off] << 8) | cl[off + 1]; off += 2;
        uint8_t  cid  = cl[off++];

        if (kem != ECH_KEM_ID || kdf != ECH_KDF_ID ||
            (aead != ECH_AEAD_ID && aead != 0x0001u)) {
            off = entry_end;
            continue;
        }

        uint16_t pk_len = ((uint16_t)cl[off] << 8) | cl[off + 1]; off += 2;
        if (pk_len != 32 || off + 32 > entry_end) {
            off = entry_end;
            continue;
        }

        *out_config_id = cid;
        memcpy(out_pk, cl + off, 32);
        return 0;
    }
    return -1;
}

/*
 * Build an ECH outer ClientHello extension payload into `buf` (max buf_cap bytes).
 * Returns number of bytes written, or 0 on failure.
 *
 * If `server_pk` is non-NULL (32 bytes), performs HPKE encryption.
 * Otherwise uses ECH GREASE (random bytes for enc and payload).
 *
 * `inner_sni` is the real destination hostname to encrypt into the payload.
 * `config_id` is used when doing real ECH; random when GREASE.
 */
static size_t ech_build_ext(mbedtls_ctr_drbg_context *drbg,
                             const uint8_t *server_pk,   /* NULL = GREASE */
                             uint8_t        config_id,
                             const char    *inner_sni,
                             uint8_t       *buf,
                             size_t         buf_cap)
{
    /* Inner CH: minimal TLS server_name extension containing inner_sni
     * Format: ext_type(2) ext_len(2) list_len(2) name_type(1) name_len(2) name(...) */
    uint8_t inner_sni_len = inner_sni ? (uint8_t)strnlen(inner_sni, 253) : 0;
    size_t  sni_ext_len   = inner_sni_len ? (9 + inner_sni_len) : 0;

    // Pad to ECH_PAYLOAD_MIN_LEN for traffic analysis resistance.
    size_t plaintext_len = sni_ext_len > ECH_PAYLOAD_MIN_LEN
                           ? sni_ext_len : ECH_PAYLOAD_MIN_LEN;
    // ciphertext len = plaintext + 16 (Poly1305 tag)
    size_t ciphertext_len = plaintext_len + 16;

    /* Total ext payload:
     * type(1) + kem(2) + kdf(2) + aead(2) + config_id(1)
     * + enc_len(2) + enc(32) + payload_len(2) + payload(ciphertext_len) */
    size_t ext_payload_len = 1 + 2 + 2 + 2 + 1 + 2 + 32 + 2 + ciphertext_len;
    if (ext_payload_len > buf_cap)
        return 0;

    uint8_t enc[32]  = {0};
    uint8_t key[32]  = {0};
    uint8_t nonce[12] = {0};
    bool    do_hpke  = (server_pk != NULL);

    if (do_hpke) {
        uint8_t ss[32];
        if (ech_kem_encap(drbg, server_pk, enc, ss) != 0)
            do_hpke = false;
        else {
            // info = "tls ech\0" || config_id
            uint8_t hpke_info[9] = {
                't','l','s',' ','e','c','h','\0', config_id
            };
            ech_hpke_setup_base(ss, 32, enc, 32,
                                hpke_info, sizeof(hpke_info),
                                key, nonce);
            sodium_memzero(ss, 32);
        }
    }

    if (!do_hpke) {
        // GREASE: random enc and config_id.
        mbedtls_ctr_drbg_random(drbg, enc,       32);
        mbedtls_ctr_drbg_random(drbg, &config_id, 1);
    }

    size_t off = 0;
    buf[off++] = ECH_TYPE_OUTER;                      /* type */
    buf[off++] = (uint8_t)(ECH_KEM_ID  >> 8);
    buf[off++] = (uint8_t)(ECH_KEM_ID  & 0xFF);       /* kem_id */
    buf[off++] = (uint8_t)(ECH_KDF_ID  >> 8);
    buf[off++] = (uint8_t)(ECH_KDF_ID  & 0xFF);       /* kdf_id */
    buf[off++] = (uint8_t)(ECH_AEAD_ID >> 8);
    buf[off++] = (uint8_t)(ECH_AEAD_ID & 0xFF);       /* aead_id */
    buf[off++] = config_id;
    buf[off++] = 0x00;
    buf[off++] = ECH_ENC_LEN;                         /* enc length (2 bytes) */
    memcpy(buf + off, enc, 32); off += 32;            /* enc */
    buf[off++] = (uint8_t)(ciphertext_len >> 8);
    buf[off++] = (uint8_t)(ciphertext_len & 0xFF);    /* payload length (2 bytes) */

    uint8_t *payload_dst = buf + off;
    off += ciphertext_len;

    if (do_hpke && inner_sni_len) {
        // Build plaintext: SNI extension + zero padding.
        uint8_t pt[512] = {0};
        size_t  pt_off  = 0;
        // server_name extension: type=0x0000, len, list_len, type=0, name_len, name.
        pt[pt_off++] = 0x00; pt[pt_off++] = 0x00; /* ext type: server_name */
        uint16_t ext_content_len = 3 + inner_sni_len;
        pt[pt_off++] = (uint8_t)(ext_content_len >> 8);
        pt[pt_off++] = (uint8_t)(ext_content_len & 0xFF);
        uint16_t list_len_val = 1 + 2 + inner_sni_len;
        pt[pt_off++] = (uint8_t)(list_len_val >> 8);
        pt[pt_off++] = (uint8_t)(list_len_val & 0xFF);
        pt[pt_off++] = 0x00; /* name_type = host_name */
        pt[pt_off++] = 0x00;
        pt[pt_off++] = inner_sni_len;
        memcpy(pt + pt_off, inner_sni, inner_sni_len);
        pt_off += inner_sni_len;
        // zero-pad to plaintext_len.
        // already zeroed via = {0}

        // AEAD = ChaCha20Poly1305-IETF (nonce 12 bytes)
        // aad: outer ECH header up to (not including) payload.
        unsigned long long ct_len = 0;
        crypto_aead_chacha20poly1305_ietf_encrypt(
            payload_dst, &ct_len,
            pt, plaintext_len,
            buf, off - ciphertext_len,  /* aad = header bytes */
            NULL, nonce, key);
        sodium_memzero(pt,    sizeof(pt));
    } else {
        // GREASE or no inner SNI: random payload.
        mbedtls_ctr_drbg_random(drbg, payload_dst, ciphertext_len);
    }

    sodium_memzero(key,   32);
    sodium_memzero(nonce, 12);
    sodium_memzero(enc,   32);
    return off;
}

static const char *default_sni_pool[] = {
    "www.google.com",
    "cdn.cloudflare.com",
    "api.github.com",
    "assets.amazonaws.com",
    "static.fastly.net",
    "media.cloudfront.net",
    "fonts.googleapis.com",
    "ajax.aspnetcdn.com",
};
#define DEFAULT_SNI_POOL_SIZE (sizeof(default_sni_pool) / sizeof(default_sni_pool[0]))

typedef struct tls_shared_ctx {
    mbedtls_entropy_context  entropy;
    mbedtls_ctr_drbg_context drbg;
    mbedtls_x509_crt         own_cert;
    mbedtls_pk_context       own_key;
    bool                     certs_loaded;
    mbedtls_x509_crt         ca_chain;
    bool                     ca_chain_loaded;
} tls_shared_ctx_t;

typedef enum tls_fronting_state {
    FRONTING_NONE        = 0,  /* No domain fronting */
    FRONTING_AWAIT_200   = 1,  /* Sent CONNECT, waiting for 200 response */
    FRONTING_ESTABLISHED = 2,  /* Tunnel established, pass data through */
} tls_fronting_state_t;

// --- Per-connection state ---
typedef struct tls_conn_data {
    uv_tcp_t              *handle;
    trellis_conn_t        *conn;

    trellis_conn_recv_cb   recv_cb;
    void                  *recv_ctx;

    uint8_t               *recv_buf;
    size_t                 recv_buf_len;
    size_t                 recv_buf_cap;

    // TLS state.
    mbedtls_ssl_context    ssl;
    mbedtls_ssl_config     ssl_conf;
    bool                   tls_established;

    // BIO ring buffer: raw TCP bytes flow in here, mbedTLS reads from here.
    uint8_t               *bio_in;
    size_t                 bio_in_len;
    size_t                 bio_in_cap;

    // BIO output: mbedTLS writes here, we flush to TCP.
    uint8_t               *bio_out;
    size_t                 bio_out_len;
    size_t                 bio_out_cap;

    tls_shared_ctx_t      *shared;
    bool                   is_server;

    // Handshake completion callback (used during connect/accept)
    trellis_connect_cb     hs_connect_cb;
    void                  *hs_connect_ctx;
    trellis_transport_t   *hs_transport;

    trellis_accept_cb      hs_accept_cb;
    void                  *hs_accept_ctx;

    // Domain fronting (HTTP CONNECT tunnel)
    tls_fronting_state_t   fronting_state;
    char                   relay_target[256];  /* host:port for CONNECT */

    // Encrypted Client Hello (ECH)
    bool    ech_enabled;
    char    ech_inner_sni[256];       /* real destination SNI (encrypted) */
    uint8_t ech_ext_buf[ECH_EXT_BUF_MAX]; /* pre-built ECH extension payload */
    size_t  ech_ext_len;
} tls_conn_data_t;

typedef struct tls_transport_data {
    uv_tcp_t             server;
    trellis_accept_cb    accept_cb;
    void                *accept_ctx;
    trellis_transport_t *transport;
    bool                 listening;
    int                  bound_port;
    tls_shared_ctx_t     shared;
    char                 sni[64];
    char                 front_host[128];
    char                 relay_target[256];
    /* When true, outgoing connections verify the peer certificate.
     * Set to false when domain fronting is enabled (CDN cert != relay cert). */
    bool                 verify_peer;

    const char         **custom_sni_pool;
    size_t               custom_sni_pool_count;

    // ECH config (populated from trellis_tls_config_t)
    char     ech_public_name[256]; /* outer SNI (CDN / public name) */
    char     ech_inner_sni[256];   /* real destination (hidden from DPI) */
    uint8_t  ech_config_list[1024]; /* raw ECHConfigList bytes from DNS HTTPS RR */
    size_t   ech_config_list_len;
    bool     ech_enabled;          /* true when ech_public_name is set */
} tls_transport_data_t;

static const trellis_conn_vtable_t      tls_conn_vtable;
static const trellis_transport_vtable_t tls_transport_vtable;
static void tls_flush_bio_out(tls_conn_data_t *cd);
static void tls_continue_handshake(tls_conn_data_t *cd);

#if MBEDTLS_VERSION_NUMBER >= 0x03040000
/*
 * mbedTLS will call this once when building the ClientHello.
 * We fill in the pre-built ECH outer extension bytes stored in cd->ech_ext_buf.
 */
static int tls_ech_ext_write_cb(mbedtls_ssl_context *ssl,
                                 unsigned int         ext_type,
                                 const unsigned char **buf,
                                 size_t              *len,
                                 void                *user_data)
{
    (void)ssl;
    (void)ext_type;
    tls_conn_data_t *cd = (tls_conn_data_t *)user_data;
    if (!cd->ech_enabled || cd->ech_ext_len == 0)
        return 0; /* no data: skip extension */
    *buf = cd->ech_ext_buf;
    *len = cd->ech_ext_len;
    return 0;
}

static int tls_ech_ext_parse_cb(mbedtls_ssl_context *ssl,
                                  unsigned int         ext_type,
                                  const unsigned char *buf,
                                  size_t               len,
                                  void                *user_data)
{
    // Server may echo back a retry ECHConfigList – log and ignore for now.
    (void)ssl; (void)ext_type; (void)buf; (void)len; (void)user_data;
    fprintf(stderr, "[ECH] server returned ECH retry-config (%zu bytes)\n", len);
    return 0;
}
#endif /* MBEDTLS_VERSION_NUMBER >= 0x03040000 */

// --- BIO callbacks for mbedTLS ---
static int tls_bio_send(void *ctx, const unsigned char *buf, size_t len)
{
    tls_conn_data_t *cd = ctx;

    size_t needed = cd->bio_out_len + len;
    if (needed > cd->bio_out_cap) {
        size_t newcap = cd->bio_out_cap ? cd->bio_out_cap * 2 : 4096;
        while (newcap < needed)
            newcap *= 2;
        uint8_t *tmp = realloc(cd->bio_out, newcap);
        if (!tmp)
            return MBEDTLS_ERR_SSL_ALLOC_FAILED;
        cd->bio_out = tmp;
        cd->bio_out_cap = newcap;
    }

    memcpy(cd->bio_out + cd->bio_out_len, buf, len);
    cd->bio_out_len += len;
    return (int)len;
}

static int tls_bio_recv(void *ctx, unsigned char *buf, size_t len)
{
    tls_conn_data_t *cd = ctx;

    if (cd->bio_in_len == 0)
        return MBEDTLS_ERR_SSL_WANT_READ;

    size_t to_copy = len < cd->bio_in_len ? len : cd->bio_in_len;
    memcpy(buf, cd->bio_in, to_copy);

    cd->bio_in_len -= to_copy;
    if (cd->bio_in_len > 0)
        memmove(cd->bio_in, cd->bio_in + to_copy, cd->bio_in_len);

    return (int)to_copy;
}

// --- Write flush: push bio_out bytes to TCP ---
typedef struct tls_flush_req {
    uv_write_t req;
    uint8_t   *data;
} tls_flush_req_t;

static void tls_on_flush(uv_write_t *req, int status)
{
    tls_flush_req_t *fr = req->data;
    if (status != 0) {
        tls_conn_data_t *cd = (tls_conn_data_t *)req->handle->data;
        if (cd && cd->recv_cb)
            cd->recv_cb(cd->conn, NULL, 0, TRELLIS_ERR_SEND, cd->recv_ctx);
    }
    free(fr->data);
    free(fr);
}

static void tls_flush_bio_out(tls_conn_data_t *cd)
{
    if (cd->bio_out_len == 0)
        return;

    tls_flush_req_t *fr = malloc(sizeof(*fr));
    if (!fr)
        return;

    fr->data = malloc(cd->bio_out_len);
    if (!fr->data) {
        free(fr);
        return;
    }

    memcpy(fr->data, cd->bio_out, cd->bio_out_len);
    size_t len = cd->bio_out_len;
    cd->bio_out_len = 0;

    fr->req.data = fr;

    uv_buf_t uvbuf = uv_buf_init((char *)fr->data, (unsigned int)len);
    int rc = uv_write(&fr->req, (uv_stream_t *)cd->handle, &uvbuf, 1,
                      tls_on_flush);
    if (rc != 0) {
        free(fr->data);
        free(fr);
    }
}

static void tls_deliver_frames(tls_conn_data_t *cd)
{
    trellis_frame_parser_t fp = {
        .conn     = cd->conn,
        .buf      = cd->recv_buf,
        .buf_len  = cd->recv_buf_len,
        .buf_cap  = cd->recv_buf_cap,
        .recv_cb  = cd->recv_cb,
        .recv_ctx = cd->recv_ctx,
    };
    trellis_frame_deliver(&fp);
    cd->recv_buf_len = fp.buf_len;
}

// --- Drain decrypted data from mbedTLS and deliver frames ---

/*
 * Check incoming data for an HTTP CONNECT 200 response.
 * The response looks like: "HTTP/1.1 200 Connection established\r\n\r\n"
 * We scan the buffer for "\r\n\r\n" and check that "200" appears in the
 * first line.  Returns true and advances past the headers if found.
 */
static bool fronting_check_200(tls_conn_data_t *cd,
                               const uint8_t *data, size_t len,
                               size_t *header_end_out)
{
    // Find end of HTTP header block.
    for (size_t i = 0; i + 3 < len; i++) {
        if (data[i] == '\r' && data[i+1] == '\n' &&
            data[i+2] == '\r' && data[i+3] == '\n') {
            // Check that the status line contains "200".
            bool has_200 = false;
            for (size_t j = 0; j + 2 < i; j++) {
                if (data[j] == '2' && data[j+1] == '0' && data[j+2] == '0') {
                    has_200 = true;
                    break;
                }
            }
            if (has_200) {
                *header_end_out = i + 4;
                return true;
            }
            return false;
        }
    }
    return false; /* incomplete response */
}

static void tls_drain_ssl_read(tls_conn_data_t *cd)
{
    uint8_t tmp[4096];
    for (;;) {
        int n = mbedtls_ssl_read(&cd->ssl, tmp, sizeof(tmp));
        if (n == MBEDTLS_ERR_SSL_WANT_READ || n == MBEDTLS_ERR_SSL_WANT_WRITE)
            break;
        if (n <= 0) {
            if (cd->recv_cb) {
                trellis_err_t err = (n == MBEDTLS_ERR_SSL_PEER_CLOSE_NOTIFY)
                                    ? TRELLIS_ERR_CLOSED : TRELLIS_ERR_TRANSPORT;
                cd->recv_cb(cd->conn, NULL, 0, err, cd->recv_ctx);
            }
            break;
        }

        /*
         * Domain fronting: while waiting for the HTTP 200, buffer data and
         * scan for the end of the CONNECT response headers.
         */
        if (cd->fronting_state == FRONTING_AWAIT_200) {
            // Append to recv_buf for header scanning.
            size_t needed = cd->recv_buf_len + (size_t)n;
            if (needed > cd->recv_buf_cap) {
                size_t nc = cd->recv_buf_cap ? cd->recv_buf_cap * 2 : 4096;
                while (nc < needed) nc *= 2;
                uint8_t *nb = realloc(cd->recv_buf, nc);
                if (!nb) break;
                cd->recv_buf = nb;
                cd->recv_buf_cap = nc;
            }
            memcpy(cd->recv_buf + cd->recv_buf_len, tmp, (size_t)n);
            cd->recv_buf_len += (size_t)n;

            size_t hdr_end = 0;
            if (fronting_check_200(cd, cd->recv_buf, cd->recv_buf_len, &hdr_end)) {
                // Tunnel established — shift any post-header bytes to start.
                size_t leftover = cd->recv_buf_len - hdr_end;
                if (leftover > 0)
                    memmove(cd->recv_buf, cd->recv_buf + hdr_end, leftover);
                cd->recv_buf_len = leftover;

                cd->fronting_state = FRONTING_ESTABLISHED;

                // Now fire the deferred connect callback.
                if (cd->hs_connect_cb) {
                    cd->hs_connect_cb(cd->conn, TRELLIS_OK, cd->hs_connect_ctx);
                    cd->hs_connect_cb = NULL;
                }

                // Deliver any payload bytes that followed the headers.
                if (leftover > 0)
                    tls_deliver_frames(cd);
            }
            continue;
        }

        size_t needed = cd->recv_buf_len + (size_t)n;
        if (needed > cd->recv_buf_cap) {
            size_t newcap = cd->recv_buf_cap ? cd->recv_buf_cap * 2 : 4096;
            while (newcap < needed)
                newcap *= 2;
            uint8_t *nbuf = realloc(cd->recv_buf, newcap);
            if (!nbuf)
                break;
            cd->recv_buf = nbuf;
            cd->recv_buf_cap = newcap;
        }
        memcpy(cd->recv_buf + cd->recv_buf_len, tmp, (size_t)n);
        cd->recv_buf_len += (size_t)n;
    }

    if (cd->fronting_state != FRONTING_AWAIT_200)
        tls_deliver_frames(cd);
}

// --- Raw TCP read callback ---
static void tls_raw_alloc_cb(uv_handle_t *handle, size_t suggested,
                              uv_buf_t *buf)
{
    tls_conn_data_t *cd = handle->data;

    size_t needed = cd->bio_in_len + suggested;
    if (needed > cd->bio_in_cap) {
        size_t newcap = cd->bio_in_cap ? cd->bio_in_cap * 2 : 4096;
        while (newcap < needed)
            newcap *= 2;
        uint8_t *tmp = realloc(cd->bio_in, newcap);
        if (!tmp) {
            buf->base = NULL;
            buf->len = 0;
            return;
        }
        cd->bio_in = tmp;
        cd->bio_in_cap = newcap;
    }

    buf->base = (char *)(cd->bio_in + cd->bio_in_len);
    buf->len = cd->bio_in_cap - cd->bio_in_len;
}

static void tls_raw_read_cb(uv_stream_t *stream, ssize_t nread,
                             const uv_buf_t *buf)
{
    tls_conn_data_t *cd = stream->data;
    (void)buf;

    if (nread < 0) {
        if (cd->recv_cb) {
            trellis_err_t err = (nread == UV_EOF) ? TRELLIS_ERR_CLOSED
                                                  : TRELLIS_ERR_TRANSPORT;
            cd->recv_cb(cd->conn, NULL, 0, err, cd->recv_ctx);
        }
        return;
    }
    if (nread == 0)
        return;

    cd->bio_in_len += (size_t)nread;

    if (!cd->tls_established) {
        tls_continue_handshake(cd);
        return;
    }

    tls_drain_ssl_read(cd);
    tls_flush_bio_out(cd);
}

// --- TLS handshake continuation ---
static void tls_continue_handshake(tls_conn_data_t *cd)
{
    int ret = mbedtls_ssl_handshake(&cd->ssl);
    tls_flush_bio_out(cd);

    if (ret == MBEDTLS_ERR_SSL_WANT_READ || ret == MBEDTLS_ERR_SSL_WANT_WRITE)
        return;

    if (ret != 0) {
        if (cd->hs_connect_cb) {
            cd->hs_connect_cb(NULL, TRELLIS_ERR_HANDSHAKE, cd->hs_connect_ctx);
            cd->hs_connect_cb = NULL;
        }
        return;
    }

    cd->tls_established = true;

    // Domain fronting: after TLS, send HTTP CONNECT to tunnel through the CDN.
    if (cd->fronting_state == FRONTING_AWAIT_200 && cd->relay_target[0]) {
        char connect_req[512];
        int rlen = snprintf(connect_req, sizeof(connect_req),
                            "CONNECT %s HTTP/1.1\r\nHost: %s\r\n\r\n",
                            cd->relay_target, cd->relay_target);
        if (rlen > 0 && (size_t)rlen < sizeof(connect_req)) {
            mbedtls_ssl_write(&cd->ssl,
                              (const unsigned char *)connect_req, (size_t)rlen);
            tls_flush_bio_out(cd);
        }
        // hs_connect_cb is called once we see the 200 response.
        return;
    }

    if (cd->hs_connect_cb) {
        cd->hs_connect_cb(cd->conn, TRELLIS_OK, cd->hs_connect_ctx);
        cd->hs_connect_cb = NULL;
    }

    if (cd->hs_accept_cb) {
        cd->hs_accept_cb(cd->conn, cd->hs_accept_ctx);
        cd->hs_accept_cb = NULL;
    }
}

// --- Connection helper ---
static trellis_conn_t *tls_conn_create(uv_tcp_t *handle,
                                       trellis_transport_t *t,
                                       tls_shared_ctx_t *shared,
                                       bool is_server,
                                       const char *sni,
                                       bool verify_peer,
                                       const tls_transport_data_t *td)
{
    trellis_conn_t *conn = trellis_conn_alloc(&tls_conn_vtable, "tls", t->loop);
    if (!conn)
        return NULL;

    tls_conn_data_t *cd = calloc(1, sizeof(*cd));
    if (!cd) {
        trellis_conn_dealloc(conn);
        return NULL;
    }

    cd->handle = handle;
    cd->handle->data = cd;
    cd->conn = conn;
    cd->shared = shared;
    cd->is_server = is_server;
    conn->impl_data = cd;

    struct sockaddr_storage local_ss, remote_ss;
    int namelen = sizeof(local_ss);
    if (uv_tcp_getsockname(cd->handle,
                           (struct sockaddr *)&local_ss, &namelen) == 0)
        trellis_conn_set_addrs(conn, (struct sockaddr *)&local_ss, NULL);

    namelen = sizeof(remote_ss);
    if (uv_tcp_getpeername(cd->handle,
                           (struct sockaddr *)&remote_ss, &namelen) == 0)
        trellis_conn_set_addrs(conn, NULL, (struct sockaddr *)&remote_ss);

    mbedtls_ssl_init(&cd->ssl);
    mbedtls_ssl_config_init(&cd->ssl_conf);

    int endpoint = is_server ? MBEDTLS_SSL_IS_SERVER : MBEDTLS_SSL_IS_CLIENT;
    mbedtls_ssl_config_defaults(&cd->ssl_conf, endpoint,
                                MBEDTLS_SSL_TRANSPORT_STREAM,
                                MBEDTLS_SSL_PRESET_DEFAULT);

    mbedtls_ssl_conf_rng(&cd->ssl_conf, mbedtls_ctr_drbg_random, &shared->drbg);
    /* Server-side connections never verify the client; client-side connections
     * verify the peer certificate unless the caller opted out (e.g. domain
     * fronting where the CDN cert does not match the relay identity).
     *
     * If verify_peer is true but no CA chain was loaded, refuse to create the
     * connection — silently downgrading to VERIFY_OPTIONAL would make MITM
     * attacks undetectable.  Callers that need no-cert connections must
     * explicitly pass verify_peer=false. */
    int authmode;
    if (!is_server && verify_peer) {
        if (!shared->ca_chain_loaded) {
            // Cannot satisfy verify_peer without a CA bundle.
            mbedtls_ssl_config_free(&cd->ssl_conf);
            free(cd);
            return NULL;
        }
        authmode = MBEDTLS_SSL_VERIFY_REQUIRED;
        mbedtls_ssl_conf_ca_chain(&cd->ssl_conf, &shared->ca_chain, NULL);
    } else {
        authmode = MBEDTLS_SSL_VERIFY_NONE;
    }
    mbedtls_ssl_conf_authmode(&cd->ssl_conf, authmode);

    if (shared->certs_loaded) {
        mbedtls_ssl_conf_own_cert(&cd->ssl_conf, &shared->own_cert,
                                  &shared->own_key);
    }

    mbedtls_ssl_conf_min_tls_version(&cd->ssl_conf,
                                     MBEDTLS_SSL_VERSION_TLS1_3);
    mbedtls_ssl_conf_max_tls_version(&cd->ssl_conf,
                                     MBEDTLS_SSL_VERSION_TLS1_3);

    mbedtls_ssl_setup(&cd->ssl, &cd->ssl_conf);

    if (!is_server && sni && sni[0])
        mbedtls_ssl_set_hostname(&cd->ssl, sni);

    /* ECH: build extension payload and register the custom-extension callback.
     * When ech_public_name is set, `sni` has already been overridden to the
     * outer (CDN) name by the caller; here we build and register the ECH
     * ClientHello extension that hides the real destination from DPI. */
    if (!is_server && td && td->ech_enabled) {
        cd->ech_enabled = true;
        if (td->ech_inner_sni[0]) {
            size_t isni_len = strlen(td->ech_inner_sni);
            if (isni_len >= sizeof(cd->ech_inner_sni))
                isni_len = sizeof(cd->ech_inner_sni) - 1;
            memcpy(cd->ech_inner_sni, td->ech_inner_sni, isni_len);
            cd->ech_inner_sni[isni_len] = '\0';
        }

        // Parse ECHConfigList (if available) for server pk and config_id.
        uint8_t server_pk[32] = {0};
        uint8_t config_id     = 0;
        bool    have_pk       = false;

        if (td->ech_config_list_len > 0) {
            have_pk = (ech_parse_config_list(
                           td->ech_config_list, td->ech_config_list_len,
                           &config_id, server_pk) == 0);
        }

        cd->ech_ext_len = ech_build_ext(
            &shared->drbg,
            have_pk ? server_pk : NULL,
            config_id,
            cd->ech_inner_sni[0] ? cd->ech_inner_sni : NULL,
            cd->ech_ext_buf, sizeof(cd->ech_ext_buf));

        fprintf(stderr, "[ECH] %s outer_sni=%s inner_sni=%s ext_len=%zu\n",
                have_pk ? "HPKE" : "GREASE",
                sni ? sni : "(none)",
                cd->ech_inner_sni[0] ? cd->ech_inner_sni : "(none)",
                cd->ech_ext_len);

#if defined(MBEDTLS_SSL_CUSTOM_TLS_EXT)
        if (cd->ech_ext_len > 0) {
            mbedtls_ssl_conf_custom_tls_ext(
                &cd->ssl_conf,
                ECH_EXT_TYPE,
                tls_ech_ext_write_cb,
                tls_ech_ext_parse_cb,
                cd);
        }
#endif
    }

    mbedtls_ssl_set_bio(&cd->ssl, cd, tls_bio_send, tls_bio_recv, NULL);

    return conn;
}

// --- Connection vtable implementations ---
typedef struct tls_write_req {
    uv_write_t           req;
    uint8_t             *frame;
    trellis_conn_send_cb cb;
    void                *ctx;
} tls_write_req_t;

static void tls_on_write(uv_write_t *req, int status)
{
    tls_write_req_t *wr = req->data;
    if (wr->cb) {
        trellis_err_t err = (status == 0) ? TRELLIS_OK : TRELLIS_ERR_SEND;
        wr->cb(err, wr->ctx);
    }
    free(wr->frame);
    free(wr);
}

static trellis_err_t tls_conn_send(trellis_conn_t *conn,
                                   const uint8_t *data, size_t len,
                                   trellis_conn_send_cb cb, void *ctx)
{
    tls_conn_data_t *cd = conn->impl_data;

    if (!cd->tls_established)
        return TRELLIS_ERR_TRANSPORT;

    uint8_t *frame = NULL;
    size_t frame_len = trellis_frame_build(conn, data, len, &frame);
    if (!frame)
        return TRELLIS_ERR_NOMEM;
    if (conn->has_frame_tag)
        conn->frame_tag_sent = true;

    size_t written = 0;
    while (written < frame_len) {
        int ret = mbedtls_ssl_write(&cd->ssl, frame + written,
                                    frame_len - written);
        if (ret < 0) {
            if (ret == MBEDTLS_ERR_SSL_WANT_WRITE ||
                ret == MBEDTLS_ERR_SSL_WANT_READ) {
                tls_flush_bio_out(cd);
                continue;
            }
            free(frame);
            return TRELLIS_ERR_SEND;
        }
        written += (size_t)ret;
    }
    free(frame);

    // Flush the encrypted output to TCP.
    if (cd->bio_out_len == 0) {
        if (cb) cb(TRELLIS_OK, ctx);
        return TRELLIS_OK;
    }

    tls_write_req_t *wr = malloc(sizeof(*wr));
    if (!wr)
        return TRELLIS_ERR_NOMEM;

    wr->frame = malloc(cd->bio_out_len);
    if (!wr->frame) {
        free(wr);
        return TRELLIS_ERR_NOMEM;
    }

    memcpy(wr->frame, cd->bio_out, cd->bio_out_len);
    size_t flush_len = cd->bio_out_len;
    cd->bio_out_len = 0;

    wr->cb = cb;
    wr->ctx = ctx;
    wr->req.data = wr;

    uv_buf_t uvbuf = uv_buf_init((char *)wr->frame, (unsigned int)flush_len);
    int rc = uv_write(&wr->req, (uv_stream_t *)cd->handle,
                      &uvbuf, 1, tls_on_write);
    if (rc != 0) {
        free(wr->frame);
        free(wr);
        return TRELLIS_ERR_SEND;
    }

    return TRELLIS_OK;
}

static trellis_err_t tls_conn_recv_start(trellis_conn_t *conn,
                                         trellis_conn_recv_cb cb, void *ctx)
{
    tls_conn_data_t *cd = conn->impl_data;
    cd->recv_cb = cb;
    cd->recv_ctx = ctx;

    int rc = uv_read_start((uv_stream_t *)cd->handle,
                           tls_raw_alloc_cb, tls_raw_read_cb);
    return (rc == 0) ? TRELLIS_OK : TRELLIS_ERR_TRANSPORT;
}

static void tls_conn_recv_stop(trellis_conn_t *conn)
{
    tls_conn_data_t *cd = conn->impl_data;
    uv_read_stop((uv_stream_t *)cd->handle);
    cd->recv_cb = NULL;
    cd->recv_ctx = NULL;
}

typedef struct tls_close_ctx {
    trellis_conn_close_cb cb;
    void                 *ctx;
    trellis_conn_t       *conn;
    tls_conn_data_t      *cd;
} tls_close_ctx_t;

static void tls_on_close(uv_handle_t *handle)
{
    tls_close_ctx_t *cc = handle->data;

    trellis_conn_close_cb cb = cc->cb;
    void *ctx = cc->ctx;
    trellis_conn_t *conn = cc->conn;
    tls_conn_data_t *cd = cc->cd;

    mbedtls_ssl_free(&cd->ssl);
    mbedtls_ssl_config_free(&cd->ssl_conf);
    free(cd->recv_buf);
    free(cd->bio_in);
    free(cd->bio_out);
    free(cd->handle);
    free(cc);
    free(cd);
    conn->impl_data = NULL;

    if (cb)
        cb(conn, ctx);

    trellis_conn_dealloc(conn);
}

static void tls_conn_close(trellis_conn_t *conn,
                            trellis_conn_close_cb cb, void *ctx)
{
    tls_conn_data_t *cd = conn->impl_data;
    if (!cd)
        return;

    // Send TLS close_notify (best-effort)
    if (cd->tls_established) {
        mbedtls_ssl_close_notify(&cd->ssl);
        tls_flush_bio_out(cd);
    }

    tls_close_ctx_t *cc = malloc(sizeof(*cc));
    if (!cc) {
        if (cb) cb(conn, ctx);
        return;
    }
    cc->cb = cb;
    cc->ctx = ctx;
    cc->conn = conn;
    cc->cd = cd;

    cd->handle->data = cc;
    uv_close((uv_handle_t *)cd->handle, tls_on_close);
}

static const trellis_conn_vtable_t tls_conn_vtable = {
    .send       = tls_conn_send,
    .recv_start = tls_conn_recv_start,
    .recv_stop  = tls_conn_recv_stop,
    .close      = tls_conn_close,
};

// --- Transport: connect ---
typedef struct tls_connect_req {
    uv_connect_t        req;
    uv_tcp_t           *handle;
    trellis_connect_cb  cb;
    void               *ctx;
    trellis_transport_t *transport;
    tls_transport_data_t *td;
} tls_connect_req_t;

static void tls_on_tcp_connect(uv_connect_t *req, int status)
{
    tls_connect_req_t *cr = (tls_connect_req_t *)req;

    if (status != 0) {
        uv_close((uv_handle_t *)cr->handle, (uv_close_cb)free);
        if (cr->cb)
            cr->cb(NULL, TRELLIS_ERR_CONNECT, cr->ctx);
        free(cr);
        return;
    }

    /* SNI selection priority:
     *  1. ECH public_name (outer SNI hides real destination from DPI)
     *  2. Domain fronting front_host (CDN CONNECT tunnel)
     *  3. Configured sni / random pool entry */
    const char *sni;
    if (cr->td->ech_enabled && cr->td->ech_public_name[0])
        sni = cr->td->ech_public_name;
    else if (cr->td->front_host[0])
        sni = cr->td->front_host;
    else
        sni = cr->td->sni;

    trellis_conn_t *conn = tls_conn_create(cr->handle, cr->transport,
                                           &cr->td->shared, false, sni,
                                           cr->td->verify_peer, cr->td);
    if (!conn) {
        uv_close((uv_handle_t *)cr->handle, (uv_close_cb)free);
        if (cr->cb)
            cr->cb(NULL, TRELLIS_ERR_NOMEM, cr->ctx);
        free(cr);
        return;
    }

    tls_conn_data_t *cd = conn->impl_data;
    cd->hs_connect_cb = cr->cb;
    cd->hs_connect_ctx = cr->ctx;
    cd->hs_transport = cr->transport;

    /* If domain fronting is configured, defer hs_connect_cb until the
     * HTTP CONNECT tunnel is established. */
    if (cr->td->front_host[0] && cr->td->relay_target[0]) {
        strncpy(cd->relay_target, cr->td->relay_target,
                sizeof(cd->relay_target) - 1);
        cd->fronting_state = FRONTING_AWAIT_200;
        // hs_connect_cb is called after tunnel established, not here.
    }

    free(cr);

    // Start reading raw TCP for the handshake.
    uv_read_start((uv_stream_t *)cd->handle, tls_raw_alloc_cb, tls_raw_read_cb);

    // Kick off the TLS handshake.
    tls_continue_handshake(cd);
}

static trellis_err_t tls_transport_connect(trellis_transport_t *t,
                                           const char *addr,
                                           trellis_connect_cb cb, void *ctx)
{
    tls_transport_data_t *td = t->impl_data;

    struct sockaddr_storage ss;
    if (trellis_parse_addr(addr, &ss) != 0)
        return TRELLIS_ERR_INVALID_ARG;

    tls_connect_req_t *cr = calloc(1, sizeof(*cr));
    if (!cr)
        return TRELLIS_ERR_NOMEM;

    cr->handle = malloc(sizeof(uv_tcp_t));
    if (!cr->handle) {
        free(cr);
        return TRELLIS_ERR_NOMEM;
    }

    cr->cb = cb;
    cr->ctx = ctx;
    cr->transport = t;
    cr->td = td;

    int rc = uv_tcp_init(t->loop, cr->handle);
    if (rc != 0) {
        free(cr->handle);
        free(cr);
        return TRELLIS_ERR_TRANSPORT;
    }

    rc = uv_tcp_connect(&cr->req, cr->handle,
                        (const struct sockaddr *)&ss, tls_on_tcp_connect);
    if (rc != 0) {
        uv_close((uv_handle_t *)cr->handle, (uv_close_cb)free);
        free(cr);
        return TRELLIS_ERR_CONNECT;
    }

    return TRELLIS_OK;
}

// --- Transport: listen ---
static void tls_on_connection(uv_stream_t *server, int status)
{
    tls_transport_data_t *td = server->data;
    if (status < 0)
        return;

    uv_tcp_t *client = malloc(sizeof(*client));
    if (!client)
        return;

    int rc = uv_tcp_init(td->transport->loop, client);
    if (rc != 0) {
        free(client);
        return;
    }

    rc = uv_accept(server, (uv_stream_t *)client);
    if (rc != 0) {
        uv_close((uv_handle_t *)client, (uv_close_cb)free);
        return;
    }

    trellis_conn_t *conn = tls_conn_create(client, td->transport,
                                           &td->shared, true,
                                           td->sni, false /* server never verifies client */,
                                           NULL /* ECH is client-only */);
    if (!conn) {
        uv_close((uv_handle_t *)client, (uv_close_cb)free);
        return;
    }

    tls_conn_data_t *cd = conn->impl_data;
    cd->hs_accept_cb = td->accept_cb;
    cd->hs_accept_ctx = td->accept_ctx;

    // Start reading raw TCP for the server-side handshake.
    uv_read_start((uv_stream_t *)cd->handle, tls_raw_alloc_cb, tls_raw_read_cb);
}

static trellis_err_t tls_transport_listen(trellis_transport_t *t,
                                          const char *addr,
                                          trellis_accept_cb cb, void *ctx)
{
    tls_transport_data_t *td = t->impl_data;
    if (td->listening)
        return TRELLIS_ERR_ALREADY_EXISTS;

    struct sockaddr_storage ss;
    if (trellis_parse_addr(addr, &ss) != 0)
        return TRELLIS_ERR_INVALID_ARG;

    int rc = uv_tcp_init(t->loop, &td->server);
    if (rc != 0)
        return TRELLIS_ERR_TRANSPORT;

    td->server.data = td;
    td->accept_cb = cb;
    td->accept_ctx = ctx;

    rc = uv_tcp_bind(&td->server, (const struct sockaddr *)&ss, 0);
    if (rc != 0) {
        uv_close((uv_handle_t *)&td->server, NULL);
        return TRELLIS_ERR_LISTEN;
    }

    rc = uv_listen((uv_stream_t *)&td->server, 128, tls_on_connection);
    if (rc != 0) {
        uv_close((uv_handle_t *)&td->server, NULL);
        return TRELLIS_ERR_LISTEN;
    }

    struct sockaddr_storage bound;
    int namelen = sizeof(bound);
    if (uv_tcp_getsockname(&td->server, (struct sockaddr *)&bound,
                           &namelen) == 0) {
        struct sockaddr_in *s4 = (struct sockaddr_in *)&bound;
        td->bound_port = ntohs(s4->sin_port);
    }

    td->listening = true;
    return TRELLIS_OK;
}

// --- Transport: stop / free ---
static trellis_err_t tls_transport_stop(trellis_transport_t *t)
{
    tls_transport_data_t *td = t->impl_data;
    if (!td->listening)
        return TRELLIS_OK;

    uv_close((uv_handle_t *)&td->server, NULL);
    td->listening = false;
    return TRELLIS_OK;
}

static void tls_transport_free(trellis_transport_t *t)
{
    tls_transport_data_t *td = t->impl_data;
    if (td) {
        if (td->listening && !uv_is_closing((uv_handle_t *)&td->server))
            uv_close((uv_handle_t *)&td->server, NULL);

        if (td->shared.certs_loaded) {
            mbedtls_x509_crt_free(&td->shared.own_cert);
            mbedtls_pk_free(&td->shared.own_key);
        }
        mbedtls_x509_crt_free(&td->shared.ca_chain);
        mbedtls_ctr_drbg_free(&td->shared.drbg);
        mbedtls_entropy_free(&td->shared.entropy);
        free(td);
    }
}

static const trellis_transport_vtable_t tls_transport_vtable = {
    .connect = tls_transport_connect,
    .listen  = tls_transport_listen,
    .stop    = tls_transport_stop,
    .free    = tls_transport_free,
};

// --- Public constructor ---
trellis_transport_t *trellis_transport_tls_new(uv_loop_t *loop,
                                               const trellis_tls_config_t *cfg)
{
    trellis_transport_t *t = trellis_transport_alloc(
        &tls_transport_vtable, "tls", loop);
    if (!t)
        return NULL;

    tls_transport_data_t *td = calloc(1, sizeof(*td));
    if (!td) {
        free(t);
        return NULL;
    }

    td->transport = t;
    t->impl_data = td;

    mbedtls_entropy_init(&td->shared.entropy);
    mbedtls_ctr_drbg_init(&td->shared.drbg);

    const char *pers = "trellis-tls";
    if (mbedtls_ctr_drbg_seed(&td->shared.drbg, mbedtls_entropy_func,
                              &td->shared.entropy,
                              (const unsigned char *)pers,
                              strlen(pers)) != 0) {
        free(td);
        free(t);
        return NULL;
    }

    // Load own certificate / key if provided.
    mbedtls_x509_crt_init(&td->shared.own_cert);
    mbedtls_pk_init(&td->shared.own_key);

    if (cfg && cfg->cert_pem && cfg->key_pem) {
        int ret = mbedtls_x509_crt_parse(&td->shared.own_cert,
                                         (const unsigned char *)cfg->cert_pem,
                                         strlen(cfg->cert_pem) + 1);
        if (ret == 0) {
            ret = mbedtls_pk_parse_key(&td->shared.own_key,
                                       (const unsigned char *)cfg->key_pem,
                                       strlen(cfg->key_pem) + 1,
                                       NULL, 0,
                                       mbedtls_ctr_drbg_random,
                                       &td->shared.drbg);
        }
        td->shared.certs_loaded = (ret == 0);
    }

    // Load CA certificate chain for peer verification.
    mbedtls_x509_crt_init(&td->shared.ca_chain);
    if (cfg && cfg->ca_pem) {
        int ret = mbedtls_x509_crt_parse(&td->shared.ca_chain,
                                          (const unsigned char *)cfg->ca_pem,
                                          strlen(cfg->ca_pem) + 1);
        td->shared.ca_chain_loaded = (ret == 0);
    }

    // Store custom SNI pool if provided.
    if (cfg && cfg->sni_pool && cfg->sni_pool_count > 0) {
        td->custom_sni_pool = cfg->sni_pool;
        td->custom_sni_pool_count = cfg->sni_pool_count;
    }

    // Store domain fronting host and relay target.
    if (cfg && cfg->front_host) {
        size_t fh_len = strlen(cfg->front_host);
        if (fh_len >= sizeof(td->front_host))
            fh_len = sizeof(td->front_host) - 1;
        memcpy(td->front_host, cfg->front_host, fh_len);
        td->front_host[fh_len] = '\0';
    }
    if (cfg && cfg->relay_target) {
        size_t rt_len = strlen(cfg->relay_target);
        if (rt_len >= sizeof(td->relay_target))
            rt_len = sizeof(td->relay_target) - 1;
        memcpy(td->relay_target, cfg->relay_target, rt_len);
        td->relay_target[rt_len] = '\0';
    }

    // Store ECH config.
    if (cfg && cfg->ech_public_name && cfg->ech_public_name[0]) {
        size_t pn_len = strlen(cfg->ech_public_name);
        if (pn_len >= sizeof(td->ech_public_name))
            pn_len = sizeof(td->ech_public_name) - 1;
        memcpy(td->ech_public_name, cfg->ech_public_name, pn_len);
        td->ech_public_name[pn_len] = '\0';
        td->ech_enabled = true;
    }
    if (cfg && cfg->ech_inner_sni && cfg->ech_inner_sni[0]) {
        size_t is_len = strlen(cfg->ech_inner_sni);
        if (is_len >= sizeof(td->ech_inner_sni))
            is_len = sizeof(td->ech_inner_sni) - 1;
        memcpy(td->ech_inner_sni, cfg->ech_inner_sni, is_len);
        td->ech_inner_sni[is_len] = '\0';
    }
    if (cfg && cfg->ech_config_list && cfg->ech_config_list_len > 0) {
        size_t cl_len = cfg->ech_config_list_len;
        if (cl_len > sizeof(td->ech_config_list))
            cl_len = sizeof(td->ech_config_list);
        memcpy(td->ech_config_list, cfg->ech_config_list, cl_len);
        td->ech_config_list_len = cl_len;
    }

    /* Certificate verification:
     *   - Enabled by default for non-fronted connections (cfg->verify_peer true
     *     or unset when front_host is absent).
     *   - Disabled when domain fronting or ECH is active (CDN cert != relay).
     *   - Callers may also set verify_peer = false explicitly.
     */
    if ((cfg && cfg->front_host && cfg->front_host[0]) || td->ech_enabled) {
        td->verify_peer = false;
    } else if (cfg) {
        td->verify_peer = cfg->verify_peer;
    } else {
        td->verify_peer = true;
    }

    // Select SNI from pool or explicit config.
    if (cfg && cfg->sni) {
        size_t sni_len = strlen(cfg->sni);
        if (sni_len >= sizeof(td->sni))
            sni_len = sizeof(td->sni) - 1;
        memcpy(td->sni, cfg->sni, sni_len);
        td->sni[sni_len] = '\0';
    } else {
        const char **pool = td->custom_sni_pool
                            ? td->custom_sni_pool : default_sni_pool;
        size_t pool_size = td->custom_sni_pool
                           ? td->custom_sni_pool_count : DEFAULT_SNI_POOL_SIZE;

        unsigned char rnd;
        mbedtls_ctr_drbg_random(&td->shared.drbg, &rnd, 1);
        size_t idx = rnd % pool_size;
        size_t sni_len = strlen(pool[idx]);
        if (sni_len >= sizeof(td->sni))
            sni_len = sizeof(td->sni) - 1;
        memcpy(td->sni, pool[idx], sni_len);
        td->sni[sni_len] = '\0';
    }

    return t;
}

#endif /* TRELLIS_WITH_TLS */
