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
 * key_transparency.c — Append-Only Key Transparency Log
 *
 * Provides a distributed key transparency (KT) log for Bloom identity keys,
 * inspired by Google's Key Transparency and Signal's PQXDH key change
 * notification.
 *
 * Problem: If an attacker compromises a node and replaces its public keys,
 * other nodes may be tricked into encrypting to the attacker's keys instead.
 * Classic TOFU (trust-on-first-use) catches this on the initial key exchange,
 * but not on key changes during an established relationship.
 *
 * Solution: Each identity publishes a signed, chained log of key updates to
 * the DHT.  Each log entry commits to the previous entry's hash (Merkle chain),
 * making retroactive modification detectable.  Clients that encounter a
 * different key than previously seen MUST check the log before proceeding.
 *
 * Log entry format:
 *   struct trellis_kt_entry {
 *       uint8_t  magic[4];              // "KTL1"
 *       uint8_t  prev_hash[32];         // SHA256 of previous entry (0 = genesis)
 *       uint32_t sequence;              // monotonically increasing
 *       uint64_t timestamp_ms;          // unix epoch ms
 *       uint8_t  fingerprint[32];       // identity fingerprint (static)
 *       uint8_t  ml_kem_pk[1568];       // ML-KEM-1024 public key (TRELLIS_ML_KEM_1024_PK_LEN)
 *       uint8_t  x25519_pk[32];         // X25519 ephemeral key
 *       uint8_t  ml_dsa_pk[2592];       // ML-DSA-87 signing key (TRELLIS_ML_DSA_87_PK_LEN)
 *       uint8_t  ed25519_pk[32];        // Ed25519 signing key
 *       uint8_t  sig[64];               // Ed25519 sig over all prior fields
 *   };
 *
 * DHT storage:
 *   Key = SHA256("trellis_kt_log" || fingerprint || uint32_le(sequence))
 *   Value = serialized entry
 *
 *   The "head" pointer:
 *   Key = SHA256("trellis_kt_head" || fingerprint)
 *   Value = uint32_le(latest_sequence)
 *
 * Client-side TOFU:
 *   On first contact: record (fingerprint, current_keys, sequence) locally.
 *   On subsequent contact: compare received keys with stored.
 *   If they differ: fetch the KT log from the DHT and verify:
 *     (a) The chain from stored_sequence to current_sequence is valid.
 *     (b) Each entry is properly signed.
 *     (c) The final entry's keys match what was presented.
 *   If verification fails: flag the peer as potentially compromised.
 */

#include "internal.h"
#include <trellis/crypto.h>
#include <trellis/mesh.h>
#include <trellis/error.h>
#include <sodium.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <time.h>
#include <inttypes.h>

#define KT_MAGIC              "KTL1"
#define KT_ENTRY_FIXED_LEN    (4 + 32 + 4 + 8 + 32)  /* magic+prev+seq+ts+fp */
#define KT_ED25519_SIG_LEN    crypto_sign_ed25519_BYTES   /* 64 */

// Maximum number of log entries we'll fetch during a chain verify.
#define KT_MAX_CHAIN_LEN      64

// TOFU cache: how many peers to track.
#define KT_TOFU_CACHE_MAX     1024

// ----

typedef struct kt_tofu_entry {
    trellis_fingerprint_t fp;
    bool                  seen;
    uint32_t              sequence;
    uint64_t              last_access; /* LRU timestamp (kt_now_ms()) */
    uint8_t               ed25519_pk[crypto_sign_ed25519_PUBLICKEYBYTES];
    uint8_t               x25519_pk[TRELLIS_X25519_PK_LEN];
    /* We don't cache the large ML-KEM and ML-DSA keys — only the Ed25519
     * key which is the primary binding for TOFU purposes. */
} kt_tofu_entry_t;

static kt_tofu_entry_t g_kt_tofu[KT_TOFU_CACHE_MAX];
static size_t          g_kt_tofu_count = 0;

typedef struct kt_own_chain {
    bool     initialized;
    uint32_t head_sequence;
    uint8_t  head_hash[32];  /* SHA256 of the last entry we published */
} kt_own_chain_t;

static kt_own_chain_t g_own_chain = {0};

static uint64_t kt_now_ms(void)
{
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    return (uint64_t)ts.tv_sec * 1000 + (uint64_t)ts.tv_nsec / 1000000;
}

static void kt_dht_entry_key(const trellis_fingerprint_t *fp,
                               uint32_t sequence,
                               uint8_t *key_out)
{
    static const char prefix[] = "trellis_kt_log";
    crypto_hash_sha256_state hs;
    crypto_hash_sha256_init(&hs);
    crypto_hash_sha256_update(&hs, (const uint8_t *)prefix, strlen(prefix));
    crypto_hash_sha256_update(&hs, fp->bytes, TRELLIS_FINGERPRINT_LEN);
    uint8_t seq_le[4];
    seq_le[0] = (uint8_t)(sequence);
    seq_le[1] = (uint8_t)(sequence >> 8);
    seq_le[2] = (uint8_t)(sequence >> 16);
    seq_le[3] = (uint8_t)(sequence >> 24);
    crypto_hash_sha256_update(&hs, seq_le, 4);
    crypto_hash_sha256_final(&hs, key_out);
}

static void kt_dht_head_key(const trellis_fingerprint_t *fp, uint8_t *key_out)
{
    static const char prefix[] = "trellis_kt_head";
    crypto_hash_sha256_state hs;
    crypto_hash_sha256_init(&hs);
    crypto_hash_sha256_update(&hs, (const uint8_t *)prefix, strlen(prefix));
    crypto_hash_sha256_update(&hs, fp->bytes, TRELLIS_FINGERPRINT_LEN);
    crypto_hash_sha256_final(&hs, key_out);
}

static kt_tofu_entry_t *kt_tofu_find(const trellis_fingerprint_t *fp)
{
    for (size_t i = 0; i < g_kt_tofu_count; i++) {
        if (trellis_fingerprint_eq(&g_kt_tofu[i].fp, fp)) {
            g_kt_tofu[i].last_access = kt_now_ms();
            return &g_kt_tofu[i];
        }
    }
    return NULL;
}

static kt_tofu_entry_t *kt_tofu_alloc(const trellis_fingerprint_t *fp)
{
    kt_tofu_entry_t *e = kt_tofu_find(fp);
    if (e) return e;

    if (g_kt_tofu_count < KT_TOFU_CACHE_MAX) {
        e = &g_kt_tofu[g_kt_tofu_count++];
        memset(e, 0, sizeof(*e));
        e->fp          = *fp;
        e->seen        = false;
        e->last_access = kt_now_ms();
        return e;
    }

    // LRU eviction: find the entry with the smallest last_access timestamp.
    size_t lru_idx = 0;
    uint64_t lru_time = g_kt_tofu[0].last_access;
    for (size_t i = 1; i < g_kt_tofu_count; i++) {
        if (g_kt_tofu[i].last_access < lru_time) {
            lru_time = g_kt_tofu[i].last_access;
            lru_idx  = i;
        }
    }
    e = &g_kt_tofu[lru_idx];
    memset(e, 0, sizeof(*e));
    e->fp          = *fp;
    e->last_access = kt_now_ms();
    return e;
}

// Entry construction.

/*
 * Serialize a KT log entry into buf.
 * buf must be large enough — minimum:
 *   KT_ENTRY_FIXED_LEN + ML_KEM_PK_LEN + X25519_PK_LEN + ML_DSA_PK_LEN +
 *   ED25519_PK_LEN + KT_ED25519_SIG_LEN
 *
 * Returns the number of bytes written, or 0 on error.
 */
size_t trellis_kt_serialize_entry(
        const trellis_identity_t *id,
        uint32_t sequence,
        const uint8_t prev_hash[32],
        uint8_t *buf, size_t buf_cap)
{
    if (!id || !buf)
        return 0;

    // Calculate required size.
    size_t required = KT_ENTRY_FIXED_LEN
                    + TRELLIS_ML_KEM_1024_PK_LEN
                    + TRELLIS_X25519_PK_LEN
                    + TRELLIS_ML_DSA_87_PK_LEN
                    + crypto_sign_ed25519_PUBLICKEYBYTES
                    + KT_ED25519_SIG_LEN;
    if (buf_cap < required)
        return 0;

    memset(buf, 0, required);
    size_t off = 0;

    // magic
    memcpy(buf + off, KT_MAGIC, 4); off += 4;

    // prev_hash
    if (prev_hash)
        memcpy(buf + off, prev_hash, 32);
    // else: zeros for genesis.
    off += 32;

    // sequence (little-endian 32-bit)
    buf[off++] = (uint8_t)(sequence);
    buf[off++] = (uint8_t)(sequence >> 8);
    buf[off++] = (uint8_t)(sequence >> 16);
    buf[off++] = (uint8_t)(sequence >> 24);

    // timestamp_ms
    uint64_t ts = kt_now_ms();
    for (int b = 0; b < 8; b++)
        buf[off++] = (uint8_t)(ts >> (b * 8));

    // fingerprint
    memcpy(buf + off, id->fingerprint.bytes, TRELLIS_FINGERPRINT_LEN);
    off += TRELLIS_FINGERPRINT_LEN;

    // ML-KEM-1024 public key.
    memcpy(buf + off, id->ml_kem_pk, TRELLIS_ML_KEM_1024_PK_LEN);
    off += TRELLIS_ML_KEM_1024_PK_LEN;

    // X25519 public key.
    memcpy(buf + off, id->x25519_pk, TRELLIS_X25519_PK_LEN);
    off += TRELLIS_X25519_PK_LEN;

    // ML-DSA-87 public key.
    memcpy(buf + off, id->ml_dsa_pk, TRELLIS_ML_DSA_87_PK_LEN);
    off += TRELLIS_ML_DSA_87_PK_LEN;

    // Ed25519 public key.
    memcpy(buf + off, id->ed25519_pk,
           crypto_sign_ed25519_PUBLICKEYBYTES);
    off += crypto_sign_ed25519_PUBLICKEYBYTES;

    // Ed25519 signature over all prior bytes.
    if (crypto_sign_ed25519_detached(buf + off, NULL, buf, off,
                                           id->ed25519_sk) != 0)
        return 0;
    off += KT_ED25519_SIG_LEN;

    return off;
}

// Entry verification.

/*
 * Verify a KT log entry.
 *
 * @data           raw entry bytes
 * @data_len       length
 * @expected_prev  expected prev_hash (NULL = any)
 * @ed25519_pk     the Ed25519 public key to verify with (from the entry itself
 *                 on genesis, or from the preceding entry on subsequent)
 * @entry_hash_out SHA256 of this entry (for chaining)
 *
 * Returns TRELLIS_OK if the entry is well-formed and signature valid.
 */
trellis_err_t trellis_kt_verify_entry(
        const uint8_t *data, size_t data_len,
        const uint8_t *expected_prev,
        const uint8_t *ed25519_pk,
        uint8_t *entry_hash_out)
{
    if (!data || !ed25519_pk)
        return TRELLIS_ERR_INVALID_ARG;

    size_t min_len = KT_ENTRY_FIXED_LEN
                   + TRELLIS_ML_KEM_1024_PK_LEN
                   + TRELLIS_X25519_PK_LEN
                   + TRELLIS_ML_DSA_87_PK_LEN
                   + crypto_sign_ed25519_PUBLICKEYBYTES
                   + KT_ED25519_SIG_LEN;
    if (data_len < min_len)
        return TRELLIS_ERR_MSG_FORMAT;

    // Check magic.
    if (memcmp(data, KT_MAGIC, 4) != 0)
        return TRELLIS_ERR_MSG_FORMAT;

    // Check prev_hash (constant-time to prevent timing oracle)
    if (expected_prev) {
        if (sodium_memcmp(data + 4, expected_prev, 32) != 0) {
            fprintf(stderr, "[KT] chain break: prev_hash mismatch\n");
            return TRELLIS_ERR_VERIFY_FAILED;
        }
    }

    // Verify signature (over everything except the last KT_ED25519_SIG_LEN bytes)
    size_t signed_len = data_len - KT_ED25519_SIG_LEN;
    const uint8_t *sig = data + signed_len;

    if (crypto_sign_ed25519_verify_detached(sig, data, signed_len,
                                             ed25519_pk) != 0) {
        fprintf(stderr, "[KT] signature verification failed\n");
        return TRELLIS_ERR_VERIFY_FAILED;
    }

    // Compute this entry's hash for chaining.
    if (entry_hash_out)
        crypto_hash_sha256(entry_hash_out, data, data_len);

    return TRELLIS_OK;
}

// Publishing

/*
 * Publish the current node's public keys to the KT log.
 * Creates a new log entry chained from the previous one.
 */
trellis_err_t trellis_kt_publish(trellis_dht_t *dht,
                                  const trellis_identity_t *id)
{
    if (!dht || !id)
        return TRELLIS_ERR_INVALID_ARG;

    uint32_t new_seq;
    uint8_t  prev_hash[32];

    if (!g_own_chain.initialized) {
        new_seq = 0;
        memset(prev_hash, 0, 32);
    } else {
        new_seq = g_own_chain.head_sequence + 1;
        memcpy(prev_hash, g_own_chain.head_hash, 32);
    }

    // Allocate buffer for the entry.
    size_t buf_cap = KT_ENTRY_FIXED_LEN
                   + TRELLIS_ML_KEM_1024_PK_LEN
                   + TRELLIS_X25519_PK_LEN
                   + TRELLIS_ML_DSA_87_PK_LEN
                   + crypto_sign_ed25519_PUBLICKEYBYTES
                   + KT_ED25519_SIG_LEN;
    uint8_t *buf = malloc(buf_cap);
    if (!buf) return TRELLIS_ERR_NOMEM;

    size_t entry_len = trellis_kt_serialize_entry(id, new_seq,
                                                   new_seq > 0 ? prev_hash : NULL,
                                                   buf, buf_cap);
    if (entry_len == 0) {
        free(buf);
        return TRELLIS_ERR_SIGN_FAILED;
    }

    // Store to DHT.
    uint8_t dht_key[32];
    kt_dht_entry_key(&id->fingerprint, new_seq, dht_key);

    trellis_err_t err = trellis_dht_store(dht, dht_key, sizeof(dht_key),
                                           buf, entry_len, NULL, NULL);
    if (err != TRELLIS_OK) {
        free(buf);
        return err;
    }

    // Update head pointer.
    uint8_t head_key[32];
    kt_dht_head_key(&id->fingerprint, head_key);
    uint8_t head_val[4];
    head_val[0] = (uint8_t)(new_seq);
    head_val[1] = (uint8_t)(new_seq >> 8);
    head_val[2] = (uint8_t)(new_seq >> 16);
    head_val[3] = (uint8_t)(new_seq >> 24);
    err = trellis_dht_store(dht, head_key, sizeof(head_key),
                            head_val, sizeof(head_val), NULL, NULL);
    if (err != TRELLIS_OK) {
        free(buf);
        return err;
    }

    // Update local chain state.
    crypto_hash_sha256(g_own_chain.head_hash, buf, entry_len);
    g_own_chain.head_sequence = new_seq;
    g_own_chain.initialized   = true;

    fprintf(stderr, "[KT] published key log entry seq=%" PRIu32 "\n", new_seq);
    free(buf);
    return TRELLIS_OK;
}

// TOFU + chain verification.
typedef struct kt_verify_ctx {
    trellis_fingerprint_t     fp;
    uint32_t                  from_seq;
    uint32_t                  to_seq;
    uint8_t                   expected_prev[32];
    uint8_t                   current_ed25519_pk[crypto_sign_ed25519_PUBLICKEYBYTES];
    uint32_t                  current_seq;
    trellis_dht_t            *dht;
    trellis_kt_verify_cb      cb;
    void                     *ctx;
} kt_verify_ctx_t;

static void kt_chain_step(kt_verify_ctx_t *vc);

static void on_kt_entry_fetched(const uint8_t *value, size_t value_len,
                                 trellis_err_t err, void *raw_ctx)
{
    kt_verify_ctx_t *vc = (kt_verify_ctx_t *)raw_ctx;

    if (err != TRELLIS_OK || !value) {
        fprintf(stderr, "[KT] failed to fetch seq %" PRIu32 " for peer\n",
                vc->current_seq);
        if (vc->cb) vc->cb(&vc->fp, false, vc->ctx);
        free(vc);
        return;
    }

    // Verify this entry.
    uint8_t entry_hash[32];
    trellis_err_t verr = trellis_kt_verify_entry(
            value, value_len,
            vc->current_seq > vc->from_seq ? vc->expected_prev : NULL,
            vc->current_ed25519_pk,
            entry_hash);

    if (verr != TRELLIS_OK) {
        fprintf(stderr, "[KT] chain verification failed at seq %" PRIu32 "\n",
                vc->current_seq);
        if (vc->cb) vc->cb(&vc->fp, false, vc->ctx);
        free(vc);
        return;
    }

    // Update chain state.
    memcpy(vc->expected_prev, entry_hash, 32);

    // Extract the new Ed25519 key from this entry for the next step.
    size_t ed_offset = KT_ENTRY_FIXED_LEN
                     + TRELLIS_ML_KEM_1024_PK_LEN
                     + TRELLIS_X25519_PK_LEN
                     + TRELLIS_ML_DSA_87_PK_LEN;
    if (value_len >= ed_offset + crypto_sign_ed25519_PUBLICKEYBYTES) {
        memcpy(vc->current_ed25519_pk, value + ed_offset,
               crypto_sign_ed25519_PUBLICKEYBYTES);
    }

    vc->current_seq++;

    if (vc->current_seq > vc->to_seq) {
        // Chain fully verified — update TOFU cache.
        kt_tofu_entry_t *tofu = kt_tofu_find(&vc->fp);
        if (tofu) {
            tofu->sequence = vc->to_seq;
            memcpy(tofu->ed25519_pk, vc->current_ed25519_pk,
                   crypto_sign_ed25519_PUBLICKEYBYTES);
        }
        if (vc->cb) vc->cb(&vc->fp, true, vc->ctx);
        free(vc);
        return;
    }

    // Fetch next entry.
    kt_chain_step(vc);
}

static void kt_chain_step(kt_verify_ctx_t *vc)
{
    uint8_t dht_key[32];
    kt_dht_entry_key(&vc->fp, vc->current_seq, dht_key);
    trellis_err_t err = trellis_dht_find_value(vc->dht, dht_key, sizeof(dht_key),
                                                on_kt_entry_fetched, vc);
    if (err != TRELLIS_OK) {
        if (vc->cb) vc->cb(&vc->fp, false, vc->ctx);
        free(vc);
    }
}

// Public API.

/*
 * Record first contact with a peer.  Should be called when we first exchange
 * keys with a new peer.
 */
void trellis_kt_record_first_contact(
        const trellis_fingerprint_t *fp,
        const uint8_t *ed25519_pk,
        const uint8_t *x25519_pk,
        uint32_t sequence)
{
    if (!fp || !ed25519_pk) return;

    kt_tofu_entry_t *e = kt_tofu_alloc(fp);
    if (!e) return;

    e->seen     = true;
    e->sequence = sequence;
    memcpy(e->ed25519_pk, ed25519_pk, crypto_sign_ed25519_PUBLICKEYBYTES);
    if (x25519_pk)
        memcpy(e->x25519_pk, x25519_pk, TRELLIS_X25519_PK_LEN);
}

/*
 * Check if a peer's presented keys are consistent with our TOFU record.
 *
 * Returns TRELLIS_OK if:
 *   - Peer is not yet seen (first contact, no issue)
 *   - Keys match what we have on file
 *
 * Returns TRELLIS_ERR_VERIFY_FAILED if keys differ (key change detected).
 * In that case, caller should call trellis_kt_verify_chain() to validate
 * the change before proceeding.
 */
trellis_err_t trellis_kt_check_peer(
        const trellis_fingerprint_t *fp,
        const uint8_t *ed25519_pk)
{
    if (!fp || !ed25519_pk)
        return TRELLIS_ERR_INVALID_ARG;

    kt_tofu_entry_t *e = kt_tofu_find(fp);
    if (!e || !e->seen)
        return TRELLIS_OK;  /* first contact */

    if (sodium_memcmp(e->ed25519_pk, ed25519_pk,
                      crypto_sign_ed25519_PUBLICKEYBYTES) != 0) {
        fprintf(stderr, "[KT] key change detected for peer — must verify chain\n");
        return TRELLIS_ERR_VERIFY_FAILED;
    }

    return TRELLIS_OK;
}

/*
 * Asynchronously verify the KT chain from the locally-stored sequence to
 * to_seq.  This downloads and verifies each log entry in order.
 *
 * cb(fp, chain_valid, ctx) is called when done.
 *
 * Use after trellis_kt_check_peer() returns VERIFY_FAILED to determine
 * whether the key change is legitimate.
 */
trellis_err_t trellis_kt_verify_chain(
        trellis_dht_t *dht,
        const trellis_fingerprint_t *fp,
        uint32_t to_seq,
        trellis_kt_verify_cb cb, void *ctx)
{
    if (!dht || !fp)
        return TRELLIS_ERR_INVALID_ARG;

    kt_tofu_entry_t *tofu = kt_tofu_find(fp);
    uint32_t from_seq = tofu ? tofu->sequence : 0;

    if (to_seq - from_seq > KT_MAX_CHAIN_LEN) {
        fprintf(stderr, "[KT] chain too long (%" PRIu32 " entries), refusing\n",
                to_seq - from_seq);
        if (cb) cb(fp, false, ctx);
        return TRELLIS_OK;
    }

    kt_verify_ctx_t *vc = calloc(1, sizeof(*vc));
    if (!vc) return TRELLIS_ERR_NOMEM;

    vc->fp         = *fp;
    vc->from_seq   = from_seq;
    vc->to_seq     = to_seq;
    vc->current_seq = from_seq;
    vc->dht        = dht;
    vc->cb         = cb;
    vc->ctx        = ctx;

    // Seed the verify context with the TOFU key (or zeros for genesis)
    if (tofu && tofu->seen)
        memcpy(vc->current_ed25519_pk, tofu->ed25519_pk,
               crypto_sign_ed25519_PUBLICKEYBYTES);

    kt_chain_step(vc);
    return TRELLIS_OK;
}
