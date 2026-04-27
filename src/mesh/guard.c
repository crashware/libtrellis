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
 * guard.c — Guard Node Pinning (Tor Proposal 292 variant)
 *
 * Maintains a stable set of 3 entry guards selected from high-reputation
 * DHT peers, persisted across restarts and rotated only on failure or
 * after a configurable lifetime (60–90 days).
 *
 * Additionally maintains vanguard L2 and L3 pools that rotate on shorter
 * schedules to prevent an adversary from inferring the entry guard by
 * watching which nodes appear as middle and exit hops in circuits.
 *
 * Guard selection criteria:
 *   - Non-quarantined (passed Sybil admission gate)
 *   - Both hybrid KEM keys present (required for onion routing)
 *   - Age in DHT ≥ GUARD_MIN_AGE_MS (new peers are not immediately trusted)
 *   - AS-diverse relative to other selected guards (different /16 subnet)
 *
 * Vanguard schedule (Tor Proposal 292):
 *   L2: 2 positions, rotate each position uniformly in [1,4] days
 *   L3: 2 positions, rotate each position uniformly in [1,4] hours
 */

#include "internal.h"
#include <sodium.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// --- Configuration constants ---
#define GUARD_COUNT             3
#define GUARD_LIFETIME_MIN_MS   (60LL * 24 * 3600 * 1000)   /* 60 days  */
#define GUARD_LIFETIME_MAX_MS   (90LL * 24 * 3600 * 1000)   /* 90 days  */
#define GUARD_MIN_AGE_MS        (3600 * 1000)                /* 1 hour; may be too low for high-churn networks */
#define GUARD_FAIL_THRESHOLD    3                            /* strikes  */

#define VANGUARD_L2_COUNT       2
#define VANGUARD_L3_COUNT       2
#define L2_ROTATION_MIN_MS      (1LL * 24 * 3600 * 1000)    /* 1 day    */
#define L2_ROTATION_MAX_MS      (4LL * 24 * 3600 * 1000)    /* 4 days   */
#define L3_ROTATION_MIN_MS      (1LL * 3600 * 1000)         /* 1 hour   */
#define L3_ROTATION_MAX_MS      (4LL * 3600 * 1000)         /* 4 hours  */

typedef struct guard_entry {
    trellis_fingerprint_t fp;
    char                  addr[256];
    uint64_t              added_at;      /* wall-clock ms when selected */
    uint64_t              expires_at;    /* added_at + random [60,90] days */
    uint64_t              last_used;
    int                   failure_count;
    bool                  active;
    bool                  valid;         /* slot populated */

    /* Path bias tracking: monitor circuit success/failure rate per guard.
     * If the ratio drops below PATH_BIAS_THRESHOLD, the guard is suspect
     * (possible malicious guard dropping circuits selectively). */
    uint32_t              circuits_attempted;
    uint32_t              circuits_succeeded;
    uint32_t              circuits_collapsed;   /* circuits failed mid-use */
} guard_entry_t;

#define PATH_BIAS_MIN_CIRCUITS  20     /* minimum before we judge */
#define PATH_BIAS_WARN_RATIO    0.70   /* warn if success < 70% */
#define PATH_BIAS_DISABLE_RATIO 0.30   /* disable guard if success < 30% */

typedef struct vanguard_entry {
    trellis_fingerprint_t fp;
    char                  addr[256];
    uint64_t              rotates_at;   /* time to rotate */
    bool                  valid;
} vanguard_entry_t;

struct trellis_guard {
    guard_entry_t    guards[GUARD_COUNT];
    vanguard_entry_t l2[VANGUARD_L2_COUNT];
    vanguard_entry_t l3[VANGUARD_L3_COUNT];

    trellis_dht_t   *dht;
    trellis_sybil_t *sybil;
    char            *persist_path;
    uint8_t          hmac_key[32];

    uint64_t         last_selection_ms;
};

// --- Persist format ---

/*
 * File layout v2 (binary, little-endian):
 *   [4] magic = 0x424C4755 ('BLGU')
 *   [4] version = 2
 *   [32] hmac_key
 *   [GUARD_COUNT] guard records:
 *     [32] fp  [256] addr  [8] added_at  [8] expires_at  [8] last_used
 *     [4] failure_count  [1] active  [1] valid
 *   [VANGUARD_L2_COUNT] L2 records:
 *     [32] fp  [256] addr  [8] rotates_at  [1] valid
 *   [VANGUARD_L3_COUNT] L3 records: same format as L2
 *   [32] crypto_auth tag over everything from hmac_key onwards (excl tag)
 */
#define PERSIST_MAGIC   0x424C4755u
#define PERSIST_VERSION 2u

/*
 * Compute the size of the authenticated body (hmac_key + records), excluding
 * the 8-byte header and the 32-byte trailing tag.
 */
#define GUARD_REC_SIZE  (32 + 256 + 8 + 8 + 8 + 4 + 1 + 1)   /* 318 */
#define VANGUARD_REC_SIZE (32 + 256 + 8 + 1)                   /* 297 */
#define PERSIST_BODY_LEN  (32 /* hmac_key */                               \
    + GUARD_COUNT * GUARD_REC_SIZE                                         \
    + (VANGUARD_L2_COUNT + VANGUARD_L3_COUNT) * VANGUARD_REC_SIZE)

static void guard_save(const trellis_guard_t *g)
{
    if (!g->persist_path)
        return;

    FILE *f = fopen(g->persist_path, "wb");
    if (!f)
        return;

    /* Accumulate the authenticated body in a buffer so we can compute
     * the HMAC tag in a single pass. */
    uint8_t *body = malloc(PERSIST_BODY_LEN);
    if (!body) { fclose(f); return; }
    size_t boff = 0;

    memcpy(body + boff, g->hmac_key, 32); boff += 32;

    for (int i = 0; i < GUARD_COUNT; i++) {
        const guard_entry_t *ge = &g->guards[i];
        uint8_t *rec = body + boff;
        size_t off = 0;
        memcpy(rec + off, ge->fp.bytes, 32);       off += 32;
        memcpy(rec + off, ge->addr, 256);           off += 256;
        trellis_write_u32_le(rec + off, (uint32_t)(ge->added_at & 0xFFFFFFFF));
        trellis_write_u32_le(rec + off + 4, (uint32_t)(ge->added_at >> 32));
        off += 8;
        trellis_write_u32_le(rec + off, (uint32_t)(ge->expires_at & 0xFFFFFFFF));
        trellis_write_u32_le(rec + off + 4, (uint32_t)(ge->expires_at >> 32));
        off += 8;
        trellis_write_u32_le(rec + off, (uint32_t)(ge->last_used & 0xFFFFFFFF));
        trellis_write_u32_le(rec + off + 4, (uint32_t)(ge->last_used >> 32));
        off += 8;
        trellis_write_u32_le(rec + off, (uint32_t)ge->failure_count); off += 4;
        rec[off++] = ge->active ? 1 : 0;
        rec[off++] = ge->valid  ? 1 : 0;
        boff += off;
    }

    for (int pool = 0; pool < 2; pool++) {
        int count = (pool == 0) ? VANGUARD_L2_COUNT : VANGUARD_L3_COUNT;
        const vanguard_entry_t *entries = (pool == 0) ? g->l2 : g->l3;
        for (int i = 0; i < count; i++) {
            const vanguard_entry_t *ve = &entries[i];
            uint8_t *rec = body + boff;
            size_t off = 0;
            memcpy(rec + off, ve->fp.bytes, 32); off += 32;
            memcpy(rec + off, ve->addr, 256);    off += 256;
            trellis_write_u32_le(rec + off,
                (uint32_t)(ve->rotates_at & 0xFFFFFFFF));
            trellis_write_u32_le(rec + off + 4,
                (uint32_t)(ve->rotates_at >> 32));
            off += 8;
            rec[off++] = ve->valid ? 1 : 0;
            boff += off;
        }
    }

    uint8_t hdr[8];
    trellis_write_u32_le(hdr, PERSIST_MAGIC);
    trellis_write_u32_le(hdr + 4, PERSIST_VERSION);
    fwrite(hdr, 1, 8, f);

    fwrite(body, 1, boff, f);

    // Compute and write HMAC tag over the body.
    uint8_t tag[crypto_auth_BYTES];
    crypto_auth(tag, body, boff, g->hmac_key);
    fwrite(tag, 1, sizeof(tag), f);

    free(body);
    fclose(f);
}

static bool guard_load(trellis_guard_t *g)
{
    if (!g->persist_path)
        return false;

    FILE *f = fopen(g->persist_path, "rb");
    if (!f)
        return false;

    uint8_t hdr[8];
    if (fread(hdr, 1, 8, f) != 8) { fclose(f); return false; }
    if (trellis_read_u32_le(hdr) != PERSIST_MAGIC) { fclose(f); return false; }
    if (trellis_read_u32_le(hdr + 4) != PERSIST_VERSION) { fclose(f); return false; }

    uint8_t *body = malloc(PERSIST_BODY_LEN);
    if (!body) { fclose(f); return false; }
    if (fread(body, 1, PERSIST_BODY_LEN, f) != PERSIST_BODY_LEN) {
        free(body); fclose(f); return false;
    }

    uint8_t stored_tag[crypto_auth_BYTES];
    if (fread(stored_tag, 1, sizeof(stored_tag), f) != sizeof(stored_tag)) {
        free(body); fclose(f); return false;
    }
    fclose(f);

    // Extract the HMAC key (first 32 bytes of body) and verify tag.
    memcpy(g->hmac_key, body, 32);
    if (crypto_auth_verify(stored_tag, body, PERSIST_BODY_LEN,
                           g->hmac_key) != 0) {
        sodium_memzero(g->hmac_key, sizeof(g->hmac_key));
        free(body);
        return false;
    }

    // Deserialize records from body, starting after the 32-byte HMAC key.
    size_t boff = 32;

    for (int i = 0; i < GUARD_COUNT; i++) {
        guard_entry_t *ge = &g->guards[i];
        const uint8_t *rec = body + boff;
        size_t off = 0;
        memcpy(ge->fp.bytes, rec + off, 32);  off += 32;
        memcpy(ge->addr, rec + off, 256);      off += 256;
        ge->added_at  = (uint64_t)trellis_read_u32_le(rec + off)
                      | ((uint64_t)trellis_read_u32_le(rec + off + 4) << 32);
        off += 8;
        ge->expires_at = (uint64_t)trellis_read_u32_le(rec + off)
                       | ((uint64_t)trellis_read_u32_le(rec + off + 4) << 32);
        off += 8;
        ge->last_used  = (uint64_t)trellis_read_u32_le(rec + off)
                       | ((uint64_t)trellis_read_u32_le(rec + off + 4) << 32);
        off += 8;
        ge->failure_count = (int)trellis_read_u32_le(rec + off); off += 4;
        ge->active = rec[off++] != 0;
        ge->valid  = rec[off++] != 0;
        boff += off;
    }

    for (int pool = 0; pool < 2; pool++) {
        int count = (pool == 0) ? VANGUARD_L2_COUNT : VANGUARD_L3_COUNT;
        vanguard_entry_t *entries = (pool == 0) ? g->l2 : g->l3;
        for (int i = 0; i < count; i++) {
            vanguard_entry_t *ve = &entries[i];
            const uint8_t *rec = body + boff;
            size_t off = 0;
            memcpy(ve->fp.bytes, rec + off, 32); off += 32;
            memcpy(ve->addr, rec + off, 256);    off += 256;
            ve->rotates_at = (uint64_t)trellis_read_u32_le(rec + off)
                           | ((uint64_t)trellis_read_u32_le(rec + off + 4) << 32);
            off += 8;
            ve->valid = rec[off++] != 0;
            boff += off;
        }
    }

    free(body);
    return true;
}

// --- Candidate filtering ---
static bool peer_has_hybrid_keys(const trellis_peer_info_t *p)
{
    static const uint8_t zero_x25519[TRELLIS_X25519_PK_LEN] = {0};
    static const uint8_t zero_mlkem[TRELLIS_ML_KEM_1024_PK_LEN] = {0};
    return memcmp(p->identity.x25519_pk, zero_x25519, TRELLIS_X25519_PK_LEN) != 0
        && memcmp(p->identity.ml_kem_pk, zero_mlkem, TRELLIS_ML_KEM_1024_PK_LEN) != 0;
}

/* Extract subnet prefix from an address string.
 * IPv4 "A.B.C.D:port" → /16 (upper 16 bits).
 * IPv6 "[xxxx:yyyy:zzzz:...]:port" → /48 hashed to 16 bits.
 * Returns 0 if the address format is unrecognized. */
static uint32_t addr_subnet16(const char *addr)
{
    if (!addr) return 0;
    unsigned a = 0, b = 0;
    if (sscanf(addr, "%u.%u.", &a, &b) == 2)
        return (a << 8) | b;

    // IPv6: extract up to the first 3 groups (48-bit prefix) and hash.
    const char *v6 = addr;
    if (*v6 == '[') v6++;
    unsigned g1 = 0, g2 = 0, g3 = 0;
    if (sscanf(v6, "%x:%x:%x:", &g1, &g2, &g3) == 3) {
        uint32_t h = (g1 * 2654435761u) ^ (g2 * 40503u) ^ (g3 * 65599u);
        return (h & 0xFFFF) | 0x10000; /* set bit 16 to distinguish from IPv4 */
    }
    return 0;
}

static bool already_used_subnet(const trellis_guard_t *g,
                                 const char *candidate_addr,
                                 int skip_idx)
{
    uint32_t s = addr_subnet16(candidate_addr);
    if (s == 0)
        return false;
    for (int i = 0; i < GUARD_COUNT; i++) {
        if (i == skip_idx || !g->guards[i].valid)
            continue;
        if (addr_subnet16(g->guards[i].addr) == s)
            return true;
    }
    return false;
}

static uint64_t random_guard_lifetime(void)
{
    uint32_t r = randombytes_uniform((uint32_t)(GUARD_LIFETIME_MAX_MS
                                              - GUARD_LIFETIME_MIN_MS));
    return GUARD_LIFETIME_MIN_MS + (uint64_t)r;
}

static uint64_t random_l2_rotation(void)
{
    uint32_t r = randombytes_uniform(
        (uint32_t)(L2_ROTATION_MAX_MS - L2_ROTATION_MIN_MS));
    return L2_ROTATION_MIN_MS + (uint64_t)r;
}

static uint64_t random_l3_rotation(void)
{
    uint32_t r = randombytes_uniform(
        (uint32_t)(L3_ROTATION_MAX_MS - L3_ROTATION_MIN_MS));
    return L3_ROTATION_MIN_MS + (uint64_t)r;
}

// --- Select a single guard/vanguard from the DHT ---
static bool select_peer_for_slot(trellis_guard_t *g,
                                  uint64_t now_ms,
                                  const trellis_fingerprint_t *exclude_fps,
                                  size_t exclude_count,
                                  int skip_subnet_guard_idx,
                                  trellis_fingerprint_t *out_fp,
                                  char *out_addr)
{
    if (!g->dht)
        return false; /* no-peers mode: no candidates available */

    trellis_peer_info_t *candidates[TRELLIS_DHT_K * 4];
    size_t actual = 0;
    select_random_peers(&g->dht->rt, g->sybil,
                        candidates, TRELLIS_DHT_K * 4, &actual);

    if (actual == 0) return false;

    // Fisher-Yates shuffle for randomness.
    for (size_t i = actual - 1; i > 0; i--) {
        uint32_t j = randombytes_uniform((uint32_t)(i + 1));
        trellis_peer_info_t *tmp = candidates[i];
        candidates[i] = candidates[j];
        candidates[j] = tmp;
    }

    for (size_t i = 0; i < actual; i++) {
        trellis_peer_info_t *p = candidates[i];
        if (p->quarantined)
            continue;
        if (!peer_has_hybrid_keys(p))
            continue;

        /* Must have been in our routing table for at least GUARD_MIN_AGE_MS.
         * first_seen == 0 means the field pre-dates this version; treat as
         * old enough to avoid breaking bootstrap. */
        if (p->first_seen > 0 &&
            now_ms - p->first_seen < GUARD_MIN_AGE_MS)
            continue;

        // Exclude specific fingerprints (already selected guards/vanguards)
        bool excluded = false;
        for (size_t e = 0; e < exclude_count; e++) {
            if (trellis_fingerprint_eq(&p->fingerprint, &exclude_fps[e])) {
                excluded = true;
                break;
            }
        }
        if (excluded)
            continue;

        // AS diversity for guard slots: prefer different /16 subnet.
        if (skip_subnet_guard_idx >= 0 &&
            already_used_subnet(g, p->addr, skip_subnet_guard_idx))
            continue;

        *out_fp = p->fingerprint;
        if (p->addr[0])
            memcpy(out_addr, p->addr, 256);
        else
            out_addr[0] = '\0';
        return true;
    }
    return false;
}

// --- Public API ---
trellis_guard_t *trellis_guard_new(trellis_dht_t *dht,
                                    trellis_sybil_t *sybil,
                                    const char *persist_path)
{
    /* dht may be NULL; guard then operates in no-peers mode (useful for
     * testing and bootstrap scenarios without a running DHT). */
    trellis_guard_t *g = calloc(1, sizeof(*g));
    if (!g)
        return NULL;

    g->dht   = dht;
    g->sybil = sybil;

    if (persist_path) {
        g->persist_path = strdup(persist_path);
        if (!g->persist_path) {
            free(g);
            return NULL;
        }
        if (!guard_load(g))
            randombytes_buf(g->hmac_key, sizeof(g->hmac_key));
    } else {
        randombytes_buf(g->hmac_key, sizeof(g->hmac_key));
    }

    return g;
}

void trellis_guard_free(trellis_guard_t *g)
{
    if (!g)
        return;
    sodium_memzero(g->hmac_key, sizeof(g->hmac_key));
    free(g->persist_path);
    free(g);
}

void trellis_guard_select(trellis_guard_t *g, uint64_t now_ms)
{
    if (!g)
        return;

    // Expire old guards.
    for (int i = 0; i < GUARD_COUNT; i++) {
        if (g->guards[i].valid && now_ms > g->guards[i].expires_at)
            g->guards[i].valid = false;
    }

    // Fill any empty guard slots.
    trellis_fingerprint_t used[GUARD_COUNT];
    size_t used_count = 0;
    for (int i = 0; i < GUARD_COUNT; i++) {
        if (g->guards[i].valid)
            used[used_count++] = g->guards[i].fp;
    }

    bool changed = false;
    for (int i = 0; i < GUARD_COUNT; i++) {
        if (g->guards[i].valid)
            continue;

        trellis_fingerprint_t fp = {0};
        char addr[256] = {0};
        if (select_peer_for_slot(g, now_ms, used, used_count,
                                  i, &fp, addr)) {
            g->guards[i].fp         = fp;
            memcpy(g->guards[i].addr, addr, 256);
            g->guards[i].added_at   = now_ms;
            g->guards[i].expires_at = now_ms + random_guard_lifetime();
            g->guards[i].last_used  = 0;
            g->guards[i].failure_count = 0;
            g->guards[i].active     = true;
            g->guards[i].valid      = true;
            used[used_count++] = fp;
            changed = true;
        }
    }

    // Select vanguard L2 slots.
    trellis_fingerprint_t all_used[GUARD_COUNT + VANGUARD_L2_COUNT + VANGUARD_L3_COUNT];
    size_t all_used_count = 0;
    for (int i = 0; i < GUARD_COUNT; i++)
        if (g->guards[i].valid) all_used[all_used_count++] = g->guards[i].fp;

    for (int i = 0; i < VANGUARD_L2_COUNT; i++) {
        if (g->l2[i].valid && now_ms < g->l2[i].rotates_at) {
            all_used[all_used_count++] = g->l2[i].fp;
            continue;
        }
        trellis_fingerprint_t fp = {0};
        char addr[256] = {0};
        if (select_peer_for_slot(g, now_ms, all_used, all_used_count,
                                  -1, &fp, addr)) {
            g->l2[i].fp = fp;
            memcpy(g->l2[i].addr, addr, 256);
            g->l2[i].rotates_at = now_ms + random_l2_rotation();
            g->l2[i].valid = true;
            all_used[all_used_count++] = fp;
            changed = true;
        }
    }

    // Select vanguard L3 slots.
    for (int i = 0; i < VANGUARD_L3_COUNT; i++) {
        if (g->l3[i].valid && now_ms < g->l3[i].rotates_at)
            continue;
        trellis_fingerprint_t fp = {0};
        char addr[256] = {0};
        if (select_peer_for_slot(g, now_ms, all_used, all_used_count,
                                  -1, &fp, addr)) {
            g->l3[i].fp = fp;
            memcpy(g->l3[i].addr, addr, 256);
            g->l3[i].rotates_at = now_ms + random_l3_rotation();
            g->l3[i].valid = true;
            all_used[all_used_count++] = fp;
            changed = true;
        }
    }

    if (changed)
        guard_save(g);

    g->last_selection_ms = now_ms;
}

const trellis_fingerprint_t *trellis_guard_get(const trellis_guard_t *g,
                                                 int idx)
{
    if (!g || idx < 0 || idx >= GUARD_COUNT)
        return NULL;
    return g->guards[idx].valid ? &g->guards[idx].fp : NULL;
}

const trellis_fingerprint_t *trellis_guard_get_l2(const trellis_guard_t *g,
                                                    int idx)
{
    if (!g || idx < 0 || idx >= VANGUARD_L2_COUNT)
        return NULL;
    return g->l2[idx].valid ? &g->l2[idx].fp : NULL;
}

const trellis_fingerprint_t *trellis_guard_get_l3(const trellis_guard_t *g,
                                                    int idx)
{
    if (!g || idx < 0 || idx >= VANGUARD_L3_COUNT)
        return NULL;
    return g->l3[idx].valid ? &g->l3[idx].fp : NULL;
}

void trellis_guard_on_failure(trellis_guard_t *g,
                               const trellis_fingerprint_t *fp)
{
    if (!g || !fp)
        return;

    for (int i = 0; i < GUARD_COUNT; i++) {
        if (!g->guards[i].valid)
            continue;
        if (trellis_fingerprint_eq(&g->guards[i].fp, fp)) {
            g->guards[i].failure_count++;
            if (g->guards[i].failure_count >= GUARD_FAIL_THRESHOLD) {
                g->guards[i].active = false;
                /* Keep the slot populated but mark inactive; replacement
                 * selection happens on next trellis_guard_select() call with
                 * an overlap period to avoid churning all guards at once. */
                uint64_t now_ms = trellis_now_ms();
                // Invalidate immediately so select() fills it.
                g->guards[i].valid = false;
                trellis_guard_select(g, now_ms);
            }
            return;
        }
    }

    // Check vanguard positions as well.
    for (int i = 0; i < VANGUARD_L2_COUNT; i++) {
        if (g->l2[i].valid && trellis_fingerprint_eq(&g->l2[i].fp, fp)) {
            g->l2[i].rotates_at = 0; /* force rotation on next select() */
        }
    }
    for (int i = 0; i < VANGUARD_L3_COUNT; i++) {
        if (g->l3[i].valid && trellis_fingerprint_eq(&g->l3[i].fp, fp)) {
            g->l3[i].rotates_at = 0;
        }
    }
}

void trellis_guard_tick(trellis_guard_t *g, uint64_t now_ms)
{
    if (!g)
        return;

    bool need_select = false;

    for (int i = 0; i < GUARD_COUNT; i++) {
        if (!g->guards[i].valid) { need_select = true; break; }
        if (now_ms > g->guards[i].expires_at) { need_select = true; break; }
    }
    for (int i = 0; i < VANGUARD_L2_COUNT && !need_select; i++) {
        if (!g->l2[i].valid || now_ms >= g->l2[i].rotates_at)
            need_select = true;
    }
    for (int i = 0; i < VANGUARD_L3_COUNT && !need_select; i++) {
        if (!g->l3[i].valid || now_ms >= g->l3[i].rotates_at)
            need_select = true;
    }

    if (need_select)
        trellis_guard_select(g, now_ms);
}

void trellis_guard_mark_used(trellis_guard_t *g,
                              const trellis_fingerprint_t *fp)
{
    if (!g || !fp)
        return;
    for (int i = 0; i < GUARD_COUNT; i++) {
        if (g->guards[i].valid &&
            trellis_fingerprint_eq(&g->guards[i].fp, fp)) {
            g->guards[i].last_used   = trellis_now_ms();
            g->guards[i].active      = true;
            g->guards[i].failure_count = 0;
            return;
        }
    }
}

// --- Path bias tracking ---
void trellis_guard_path_bias_attempt(trellis_guard_t *g,
                                      const trellis_fingerprint_t *guard_fp)
{
    if (!g || !guard_fp) return;
    for (int i = 0; i < GUARD_COUNT; i++) {
        if (g->guards[i].valid &&
            trellis_fingerprint_eq(&g->guards[i].fp, guard_fp)) {
            g->guards[i].circuits_attempted++;
            return;
        }
    }
}

void trellis_guard_path_bias_success(trellis_guard_t *g,
                                      const trellis_fingerprint_t *guard_fp)
{
    if (!g || !guard_fp) return;
    for (int i = 0; i < GUARD_COUNT; i++) {
        if (g->guards[i].valid &&
            trellis_fingerprint_eq(&g->guards[i].fp, guard_fp)) {
            g->guards[i].circuits_succeeded++;
            return;
        }
    }
}

void trellis_guard_path_bias_collapse(trellis_guard_t *g,
                                       const trellis_fingerprint_t *guard_fp)
{
    if (!g || !guard_fp) return;
    for (int i = 0; i < GUARD_COUNT; i++) {
        if (g->guards[i].valid &&
            trellis_fingerprint_eq(&g->guards[i].fp, guard_fp)) {
            g->guards[i].circuits_collapsed++;
            return;
        }
    }
}

trellis_err_t trellis_guard_path_bias_check(trellis_guard_t *g)
{
    if (!g) return TRELLIS_ERR_INVALID_ARG;

    for (int i = 0; i < GUARD_COUNT; i++) {
        guard_entry_t *ge = &g->guards[i];
        if (!ge->valid || !ge->active) continue;
        if (ge->circuits_attempted < PATH_BIAS_MIN_CIRCUITS) continue;

        double ratio = (double)ge->circuits_succeeded /
                       (double)ge->circuits_attempted;

        if (ratio < PATH_BIAS_DISABLE_RATIO) {
            fprintf(stderr, "[guard] path bias CRITICAL for %02x%02x...: "
                    "%.1f%% success (%u/%u) -- disabling guard\n",
                    ge->fp.bytes[0], ge->fp.bytes[1],
                    ratio * 100.0, ge->circuits_succeeded,
                    ge->circuits_attempted);
            ge->active = false;
            guard_save(g);
        } else if (ratio < PATH_BIAS_WARN_RATIO) {
            fprintf(stderr, "[guard] path bias WARNING for %02x%02x...: "
                    "%.1f%% success (%u/%u)\n",
                    ge->fp.bytes[0], ge->fp.bytes[1],
                    ratio * 100.0, ge->circuits_succeeded,
                    ge->circuits_attempted);
        }
    }
    return TRELLIS_OK;
}
