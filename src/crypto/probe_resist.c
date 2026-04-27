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

#include <trellis/probe_resist.h>
#include <trellis/types.h>
#include <sodium.h>
#include <stdlib.h>
#include <string.h>

/*
 * Active probe resistance (DPI Defense Layer 1B).
 *
 * Protects Trellis servers from identification by censors who send
 * probe connections.  The guard wraps outgoing handshake messages with
 * an HMAC + timestamp + nonce, and validates incoming messages:
 *
 * - No response without valid HMAC derived from the obfuscation key
 * - Replay protection via nonce tracking with a circular buffer
 * - Timestamp freshness check (reject > 60s old)
 * - Rate limiting on unknown connections
 *
 * Wire format: [32-byte HMAC][8-byte timestamp LE][16-byte nonce][message]
 *
 * Scaled from 32 to 1024 IP buckets with hash-indexed O(1) lookup and
 * a bloom filter fast-path for known-rate-limited IPs (avoids touching
 * the LRU for repeated offenders).  Designed to resist distributed
 * attacks (Kimwolf-scale: 700K+ IPs).
 */
#define PROBE_IP_BUCKETS_DEFAULT 1024    /* sized for ~700k IPs at 70% load factor */
#define PROBE_IP_LINEAR_PROBE    4

/* Bloom filter for recently rate-limited IPs: avoids LRU churn from
 * known-bad IPs hammering the table.  2048 bits, 3 hash functions. */
#define PROBE_BLOOM_BITS  2048
#define PROBE_BLOOM_BYTES (PROBE_BLOOM_BITS / 8)
#define PROBE_BLOOM_K     3
#define PROBE_BLOOM_RESET_MS 60000  /* reset bloom filter every 60s */

typedef struct probe_ip_bucket {
    uint64_t ip_hash;
    uint32_t count;
    uint64_t window_start;
    uint64_t last_used;
} probe_ip_bucket_t;

typedef struct probe_nonce_slot {
    uint8_t  nonce[TRELLIS_PROBE_NONCE_LEN];
    uint64_t recorded_ms;  /* 0 = empty */
} probe_nonce_slot_t;

struct trellis_probe_guard {
    uint8_t  key[32];

    probe_nonce_slot_t replay_slots[TRELLIS_PROBE_REPLAY_SLOTS];
    size_t             replay_head;

    uint32_t rate_limit;
    uint32_t per_ip_rate_limit;
    uint32_t rate_count;
    uint64_t rate_window_start;

    probe_ip_bucket_t *ip_buckets;
    size_t             ip_bucket_count;

    // Bloom filter for recently rate-limited IPs.
    uint8_t  bloom[PROBE_BLOOM_BYTES];
    uint64_t bloom_reset_at;

    // Clock skew offset for timestamp validation (set from DHT clock sync)
    int64_t  clock_offset_ms;
};

static void bloom_set(uint8_t *bloom, uint64_t ip_hash)
{
    for (int k = 0; k < PROBE_BLOOM_K; k++) {
        uint32_t bit = (uint32_t)((ip_hash >> (k * 11)) % PROBE_BLOOM_BITS);
        bloom[bit / 8] |= (uint8_t)(1 << (bit % 8));
    }
}

static bool bloom_test(const uint8_t *bloom, uint64_t ip_hash)
{
    for (int k = 0; k < PROBE_BLOOM_K; k++) {
        uint32_t bit = (uint32_t)((ip_hash >> (k * 11)) % PROBE_BLOOM_BITS);
        if (!(bloom[bit / 8] & (1 << (bit % 8))))
            return false;
    }
    return true;
}

static void bloom_maybe_reset(trellis_probe_guard_t *pg, uint64_t now)
{
    if (now - pg->bloom_reset_at >= PROBE_BLOOM_RESET_MS) {
        memset(pg->bloom, 0, PROBE_BLOOM_BYTES);
        pg->bloom_reset_at = now;
    }
}

static trellis_probe_guard_t *probe_guard_new_impl(const uint8_t *obfs_key,
                                                    size_t key_len,
                                                    size_t ip_buckets)
{
    if (!obfs_key || key_len == 0)
        return NULL;

    trellis_probe_guard_t *pg = calloc(1, sizeof(*pg));
    if (!pg) return NULL;

    pg->ip_bucket_count = ip_buckets > 0 ? ip_buckets : PROBE_IP_BUCKETS_DEFAULT;
    pg->ip_buckets = calloc(pg->ip_bucket_count, sizeof(probe_ip_bucket_t));
    if (!pg->ip_buckets) {
        free(pg);
        return NULL;
    }

    crypto_generichash(pg->key, sizeof(pg->key),
                       obfs_key, key_len,
                       (const uint8_t *)"bloom-probe-guard", 17);
    pg->rate_limit = 500;
    pg->per_ip_rate_limit = 10;
    pg->bloom_reset_at = trellis_now_ms();
    return pg;
}

trellis_probe_guard_t *trellis_probe_guard_new(const uint8_t *obfs_key,
                                               size_t key_len)
{
    return probe_guard_new_impl(obfs_key, key_len, PROBE_IP_BUCKETS_DEFAULT);
}

trellis_probe_guard_t *trellis_probe_guard_new_sized(const uint8_t *obfs_key,
                                                     size_t key_len,
                                                     size_t ip_buckets)
{
    return probe_guard_new_impl(obfs_key, key_len, ip_buckets);
}

void trellis_probe_guard_free(trellis_probe_guard_t *pg)
{
    if (!pg) return;
    if (pg->ip_buckets) {
        sodium_memzero(pg->ip_buckets,
                       pg->ip_bucket_count * sizeof(probe_ip_bucket_t));
        free(pg->ip_buckets);
    }
    sodium_memzero(pg, sizeof(*pg));
    free(pg);
}

trellis_err_t trellis_probe_guard_wrap(trellis_probe_guard_t *pg,
                                       const uint8_t *msg, size_t msg_len,
                                       trellis_buf_t *out)
{
    if (!pg || !msg || !out)
        return TRELLIS_ERR_INVALID_ARG;

    *out = trellis_buf_alloc(TRELLIS_PROBE_HEADER_LEN + msg_len);
    if (!out->data)
        return TRELLIS_ERR_NOMEM;

    uint8_t *p = out->data;

    uint64_t now = trellis_now_ms();
    uint8_t ts[TRELLIS_PROBE_TIMESTAMP_LEN];
    for (int i = 0; i < 8; i++)
        ts[i] = (uint8_t)(now >> ((7 - i) * 8));

    uint8_t nonce[TRELLIS_PROBE_NONCE_LEN];
    randombytes_buf(nonce, sizeof(nonce));

    crypto_auth_hmacsha256_state st;
    crypto_auth_hmacsha256_init(&st, pg->key, sizeof(pg->key));
    crypto_auth_hmacsha256_update(&st, ts, sizeof(ts));
    crypto_auth_hmacsha256_update(&st, nonce, sizeof(nonce));
    crypto_auth_hmacsha256_update(&st, msg, msg_len);
    crypto_auth_hmacsha256_final(&st, p);
    p += TRELLIS_PROBE_HMAC_LEN;

    memcpy(p, ts, sizeof(ts));
    p += TRELLIS_PROBE_TIMESTAMP_LEN;

    memcpy(p, nonce, sizeof(nonce));
    p += TRELLIS_PROBE_NONCE_LEN;

    memcpy(p, msg, msg_len);
    out->len = TRELLIS_PROBE_HEADER_LEN + msg_len;
    return TRELLIS_OK;
}

static bool check_replay(trellis_probe_guard_t *pg,
                          const uint8_t *nonce, uint64_t now)
{
    for (size_t i = 0; i < TRELLIS_PROBE_REPLAY_SLOTS; i++) {
        probe_nonce_slot_t *s = &pg->replay_slots[i];
        if (s->recorded_ms == 0)
            continue;
        if (now > s->recorded_ms &&
            (now - s->recorded_ms) > TRELLIS_PROBE_MAX_AGE_MS)
            continue;
        if (sodium_memcmp(s->nonce, nonce, TRELLIS_PROBE_NONCE_LEN) == 0)
            return true;
    }
    return false;
}

static void record_nonce(trellis_probe_guard_t *pg,
                          const uint8_t *nonce, uint64_t now)
{
    size_t start = pg->replay_head;
    for (size_t i = 0; i < TRELLIS_PROBE_REPLAY_SLOTS; i++) {
        size_t idx = (start + i) % TRELLIS_PROBE_REPLAY_SLOTS;
        probe_nonce_slot_t *s = &pg->replay_slots[idx];
        bool expired = (s->recorded_ms == 0) ||
                       (now > s->recorded_ms &&
                        (now - s->recorded_ms) > TRELLIS_PROBE_MAX_AGE_MS);
        if (expired) {
            memcpy(s->nonce, nonce, TRELLIS_PROBE_NONCE_LEN);
            s->recorded_ms = now;
            pg->replay_head = (idx + 1) % TRELLIS_PROBE_REPLAY_SLOTS;
            return;
        }
    }
    probe_nonce_slot_t *s = &pg->replay_slots[pg->replay_head];
    memcpy(s->nonce, nonce, TRELLIS_PROBE_NONCE_LEN);
    s->recorded_ms = now;
    pg->replay_head = (pg->replay_head + 1) % TRELLIS_PROBE_REPLAY_SLOTS;
}

trellis_err_t trellis_probe_guard_validate(trellis_probe_guard_t *pg,
                                           const uint8_t *data, size_t data_len,
                                           const uint8_t **msg_out,
                                           size_t *msg_len_out)
{
    if (!pg || !data || !msg_out || !msg_len_out)
        return TRELLIS_ERR_INVALID_ARG;

    if (data_len < TRELLIS_PROBE_HEADER_LEN)
        return TRELLIS_ERR_DECRYPT;

    uint64_t now = trellis_now_ms();
    if (now - pg->rate_window_start >= 1000) {
        pg->rate_window_start = now;
        pg->rate_count = 0;
    }
    pg->rate_count++;
    if (pg->rate_limit > 0 && pg->rate_count > pg->rate_limit)
        return TRELLIS_ERR_DECRYPT;

    const uint8_t *hmac  = data;
    const uint8_t *ts    = data + TRELLIS_PROBE_HMAC_LEN;
    const uint8_t *nonce = ts + TRELLIS_PROBE_TIMESTAMP_LEN;
    const uint8_t *msg   = nonce + TRELLIS_PROBE_NONCE_LEN;
    size_t msg_len = data_len - TRELLIS_PROBE_HEADER_LEN;

    uint8_t expected[TRELLIS_PROBE_HMAC_LEN];
    crypto_auth_hmacsha256_state st;
    crypto_auth_hmacsha256_init(&st, pg->key, sizeof(pg->key));
    crypto_auth_hmacsha256_update(&st, ts, TRELLIS_PROBE_TIMESTAMP_LEN);
    crypto_auth_hmacsha256_update(&st, nonce, TRELLIS_PROBE_NONCE_LEN);
    crypto_auth_hmacsha256_update(&st, msg, msg_len);
    crypto_auth_hmacsha256_final(&st, expected);

    if (sodium_memcmp(hmac, expected, TRELLIS_PROBE_HMAC_LEN) != 0)
        return TRELLIS_ERR_DECRYPT;

    uint64_t msg_time = 0;
    for (int i = 0; i < 8; i++)
        msg_time |= (uint64_t)ts[i] << ((7 - i) * 8);

    // Adjust for clock skew between peers.
    uint64_t adj_now = now;
    if (pg->clock_offset_ms > 0)
        adj_now += (uint64_t)pg->clock_offset_ms;
    else if ((uint64_t)(-pg->clock_offset_ms) < adj_now)
        adj_now -= (uint64_t)(-pg->clock_offset_ms);

    if (adj_now > msg_time && (adj_now - msg_time) > TRELLIS_PROBE_MAX_AGE_MS)
        return TRELLIS_ERR_DECRYPT;
    if (msg_time > adj_now && (msg_time - adj_now) > TRELLIS_PROBE_MAX_AGE_MS)
        return TRELLIS_ERR_DECRYPT;

    if (check_replay(pg, nonce, now))
        return TRELLIS_ERR_DECRYPT;
    record_nonce(pg, nonce, now);

    *msg_out = msg;
    *msg_len_out = msg_len;
    return TRELLIS_OK;
}

void trellis_probe_guard_set_rate_limit(trellis_probe_guard_t *pg,
                                        uint32_t max_per_second)
{
    if (pg) pg->rate_limit = max_per_second;
}

void trellis_probe_guard_set_per_ip_rate_limit(trellis_probe_guard_t *pg,
                                               uint32_t max_per_second)
{
    if (pg) pg->per_ip_rate_limit = max_per_second;
}

void trellis_probe_guard_set_clock_offset(trellis_probe_guard_t *pg,
                                          int64_t offset_ms)
{
    if (pg) pg->clock_offset_ms = offset_ms;
}

void trellis_probe_guard_rekey(trellis_probe_guard_t *pg,
                               const uint8_t *obfs_key, size_t key_len)
{
    if (!pg || !obfs_key || key_len == 0)
        return;
    sodium_memzero(pg->key, sizeof(pg->key));
    crypto_generichash(pg->key, sizeof(pg->key),
                       obfs_key, key_len,
                       (const uint8_t *)"bloom-probe-guard", 17);
}

/*
 * Per-IP rate limiting with hash-indexed 1024-slot table and bloom
 * filter fast-path for known-rate-limited IPs.
 *
 * Hash-indexed lookup: idx = ip_hash % bucket_count, then linear
 * probe up to PROBE_IP_LINEAR_PROBE slots.  Falls back to LRU
 * eviction if no match found within probe distance.
 */
static trellis_err_t probe_check_ip_rate(trellis_probe_guard_t *pg,
                                         const char *ip_str)
{
    if (!ip_str)
        return TRELLIS_OK;

    uint64_t now = trellis_now_ms();
    bloom_maybe_reset(pg, now);

    uint8_t hash_out[8];
    crypto_generichash(hash_out, sizeof(hash_out),
                       (const uint8_t *)ip_str, strlen(ip_str),
                       pg->key, sizeof(pg->key));
    uint64_t ip_hash = 0;
    for (int i = 0; i < 8; i++)
        ip_hash |= (uint64_t)hash_out[i] << (i * 8);

    /* Bloom filter fast-path: if this IP was recently rate-limited,
     * reject immediately without touching the bucket table. */
    if (bloom_test(pg->bloom, ip_hash))
        return TRELLIS_ERR_DECRYPT;

    // Hash-indexed lookup with bounded linear probing.
    size_t start = (size_t)(ip_hash % pg->ip_bucket_count);
    int match_idx = -1;
    int empty_idx = -1;

    for (int p = 0; p < PROBE_IP_LINEAR_PROBE; p++) {
        size_t idx = (start + (size_t)p) % pg->ip_bucket_count;
        probe_ip_bucket_t *b = &pg->ip_buckets[idx];

        if (b->last_used == 0 && empty_idx < 0) {
            empty_idx = (int)idx;
            continue;
        }
        if (b->ip_hash == ip_hash) {
            match_idx = (int)idx;
            break;
        }
    }

    probe_ip_bucket_t *bucket;

    if (match_idx >= 0) {
        bucket = &pg->ip_buckets[match_idx];
    } else if (empty_idx >= 0) {
        bucket = &pg->ip_buckets[empty_idx];
        bucket->ip_hash = ip_hash;
        bucket->count = 0;
        bucket->window_start = now;
    } else {
        // All probe slots occupied: find LRU within probe distance.
        size_t lru_idx = start;
        uint64_t oldest = UINT64_MAX;
        for (int p = 0; p < PROBE_IP_LINEAR_PROBE; p++) {
            size_t idx = (start + (size_t)p) % pg->ip_bucket_count;
            if (pg->ip_buckets[idx].last_used < oldest) {
                oldest = pg->ip_buckets[idx].last_used;
                lru_idx = idx;
            }
        }
        bucket = &pg->ip_buckets[lru_idx];
        bucket->ip_hash = ip_hash;
        bucket->count = 0;
        bucket->window_start = now;
    }

    bucket->last_used = now;

    if (now - bucket->window_start >= 1000) {
        bucket->window_start = now;
        bucket->count = 0;
    }

    bucket->count++;
    if (pg->per_ip_rate_limit > 0 && bucket->count > pg->per_ip_rate_limit) {
        bloom_set(pg->bloom, ip_hash);
        return TRELLIS_ERR_DECRYPT;
    }

    return TRELLIS_OK;
}

trellis_err_t trellis_probe_guard_validate_addr(trellis_probe_guard_t *pg,
                                                const uint8_t *data,
                                                size_t data_len,
                                                const char *remote_ip,
                                                const uint8_t **msg_out,
                                                size_t *msg_len_out)
{
    if (!pg || !data || !msg_out || !msg_len_out)
        return TRELLIS_ERR_INVALID_ARG;

    trellis_err_t ip_err = probe_check_ip_rate(pg, remote_ip);
    if (ip_err != TRELLIS_OK)
        return ip_err;

    return trellis_probe_guard_validate(pg, data, data_len, msg_out, msg_len_out);
}
