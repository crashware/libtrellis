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
 * naming.c — Bloom Naming Ecosystem
 *
 * Provides a three-layer naming system:
 *
 * 1. DHT Alias Registration:
 *    A human-readable name (e.g. "alice@bloom") is mapped to an identity
 *    fingerprint via a signed record stored in the Kademlia DHT.  The record
 *    is authenticated by the Ed25519 key of the fingerprint it points to,
 *    preventing squatting by third parties.  First-registration wins; updates
 *    require a valid signature from the registered key.
 *
 *    DHT key = SHAKE-256("bloom:name:v1:" || alias)[:32]
 *    Record  = [8 magic][1 version][alias_len][alias][32 fingerprint][8 ts]
 *              [64 ed25519_sig]
 *
 * 2. Local Petname System:
 *    Each node maintains a local mapping of fingerprint -> human label.
 *    Petnames are purely local, never broadcast, and take precedence over
 *    DHT aliases for display.  They are persisted in an encrypted local store.
 *
 * 3. Conflict Resolution:
 *    If two registrations compete for the same alias, the one with the lower
 *    timestamp wins (first-registration).  Ties (identical timestamps, different
 *    keys) are broken deterministically by the lexicographic minimum fingerprint.
 *    Clients always verify the incumbent record before accepting an update.
 *
 * 4. DNS TXT Bridge:
 *    Bloom aliases can be published as DNS TXT records in the format:
 *      bloom-alias=<base64(fingerprint)>
 *    at domain  <alias>.users.bloom.  Clients resolve these when bootstrapping
 *    and add them to the DHT, providing DNS-to-Bloom name bridge.
 *
 * Alias format:
 *   Up to 63 bytes of lowercase ASCII [a-z0-9_.-] with optional "@bloom" suffix.
 *   The "@bloom" suffix is stripped before hashing.
 */

#include "internal.h"
#include <trellis/mesh.h>
#include <trellis/error.h>
#include <sodium.h>
#include <uv.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <ctype.h>
#include <time.h>

// ----

#define NAMING_MAGIC            "BLNMV01\0"
#define NAMING_MAGIC_LEN        8
#define NAMING_VERSION          1
#define NAMING_MAX_ALIAS_LEN    63
#define NAMING_RECORD_MAX       (NAMING_MAGIC_LEN + 1 + 1 + NAMING_MAX_ALIAS_LEN \
                                  + 32 + 8 + 64)
#define NAMING_DHT_KEY_LEN      32
#define NAMING_DHT_PREFIX       "bloom:name:v1:"
#define NAMING_PUBLISH_TTL_MS   (24ULL * 3600 * 1000)   /* republish every 24h */
#define NAMING_DNS_BRIDGE_DOMAIN "users.bloom"

// Local petname store: kept in-memory, caller responsible for persistence.
#define PETNAME_MAX             256

typedef struct {
    char                alias[NAMING_MAX_ALIAS_LEN + 1];
    trellis_fingerprint_t fingerprint;
    uint64_t            registered_ms;
} naming_cache_entry_t;

typedef struct {
    trellis_fingerprint_t fingerprint;
    char                label[NAMING_MAX_ALIAS_LEN + 1];
} petname_entry_t;

struct trellis_naming {
    trellis_dht_t      *dht;
    trellis_identity_t *self_id;
    uv_loop_t          *loop;

    // In-memory cache of resolved aliases.
    naming_cache_entry_t  *cache;
    size_t                 cache_count;
    size_t                 cache_cap;

    // Local petname store.
    petname_entry_t   petnames[PETNAME_MAX];
    size_t            petname_count;

    // Registered own aliases and their republish timer.
    char            own_aliases[8][NAMING_MAX_ALIAS_LEN + 1];
    size_t          own_alias_count;
    uv_timer_t      publish_timer;
    bool            timer_active;
};

static uint64_t naming_now_ms(void)
{
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    return (uint64_t)ts.tv_sec * 1000 + ts.tv_nsec / 1000000;
}

// Normalize an alias: strip "@bloom" suffix, lowercase, validate charset.
static trellis_err_t alias_normalize(const char *raw, char *out, size_t out_cap)
{
    if (!raw || !out) return TRELLIS_ERR_INVALID_ARG;

    size_t len = strlen(raw);

    // Strip "@bloom" suffix.
    if (len > 6 && strcasecmp(raw + len - 6, "@bloom") == 0)
        len -= 6;

    if (len == 0 || len > NAMING_MAX_ALIAS_LEN)
        return TRELLIS_ERR_INVALID_ARG;
    if (out_cap < len + 1)
        return TRELLIS_ERR_INVALID_ARG;

    for (size_t i = 0; i < len; i++) {
        char c = raw[i];
        if (!isalnum((unsigned char)c) && c != '_' && c != '.' && c != '-')
            return TRELLIS_ERR_INVALID_ARG;
        out[i] = (char)tolower((unsigned char)c);
    }
    out[len] = '\0';
    return TRELLIS_OK;
}

/* Compute the DHT key for an alias using SHA-256 (consistent with all other
 * Trellis DHT key derivations: canary, KT log, incentive).
 * Key = SHA256(NAMING_DHT_PREFIX || normalized_alias) */
static void alias_dht_key(const char *alias, uint8_t key_out[NAMING_DHT_KEY_LEN])
{
    char prefixed[128];
    int n = snprintf(prefixed, sizeof(prefixed), NAMING_DHT_PREFIX "%s", alias);
    crypto_hash_sha256(key_out, (const uint8_t *)prefixed,
                       (n > 0 && (size_t)n < sizeof(prefixed)) ? (size_t)n
                                                                : strlen(prefixed));
}

// Serialize a naming record for signing/storage.
static size_t naming_record_serialize(const char *alias,
                                       const trellis_fingerprint_t *fp,
                                       uint64_t ts_ms,
                                       const uint8_t *sig, /* may be NULL */
                                       uint8_t *out, size_t out_cap)
{
    uint8_t alias_len = (uint8_t)strlen(alias);
    size_t needed = NAMING_MAGIC_LEN + 1 + 1 + alias_len + 32 + 8
                  + (sig ? 64 : 0);
    if (out_cap < needed) return 0;

    size_t off = 0;
    memcpy(out + off, NAMING_MAGIC, NAMING_MAGIC_LEN); off += NAMING_MAGIC_LEN;
    out[off++] = NAMING_VERSION;
    out[off++] = alias_len;
    memcpy(out + off, alias, alias_len); off += alias_len;
    memcpy(out + off, fp->bytes, 32); off += 32;
    for (int b = 0; b < 8; b++) out[off++] = (uint8_t)(ts_ms >> (b * 8));
    if (sig) {
        memcpy(out + off, sig, 64);
        off += 64;
    }
    return off;
}

// Parse a naming record; fills alias, fingerprint, ts_ms.
static bool naming_record_parse(const uint8_t *data, size_t len,
                                  char alias_out[NAMING_MAX_ALIAS_LEN + 1],
                                  trellis_fingerprint_t *fp_out,
                                  uint64_t *ts_ms_out,
                                  const uint8_t **sig_out)
{
    if (len < NAMING_MAGIC_LEN + 1 + 1 + 32 + 8 + 64)
        return false;
    if (memcmp(data, NAMING_MAGIC, NAMING_MAGIC_LEN) != 0)
        return false;
    if (data[NAMING_MAGIC_LEN] != NAMING_VERSION)
        return false;

    size_t off = NAMING_MAGIC_LEN + 1;
    uint8_t alias_len = data[off++];
    if (alias_len == 0 || alias_len > NAMING_MAX_ALIAS_LEN)
        return false;
    if (len < off + alias_len + 32 + 8 + 64)
        return false;

    memcpy(alias_out, data + off, alias_len);
    alias_out[alias_len] = '\0';
    off += alias_len;

    memcpy(fp_out->bytes, data + off, 32); off += 32;

    uint64_t ts = 0;
    for (int b = 0; b < 8; b++) ts |= ((uint64_t)data[off + b]) << (b * 8);
    *ts_ms_out = ts;
    off += 8;

    *sig_out = data + off;
    return true;
}

/* Verify the Ed25519 signature in a naming record.
 * We need the ed25519_pk of the fingerprint to verify; callers supply it. */
static bool naming_record_verify(const uint8_t *data, size_t len,
                                   const uint8_t *ed25519_pk)
{
    if (len < NAMING_MAGIC_LEN + 2 + 32 + 8 + 64)
        return false;

    // Signature covers everything except the trailing 64-byte sig itself.
    size_t body_len = len - 64;
    const uint8_t *sig = data + body_len;

    return crypto_sign_ed25519_verify_detached(sig, data, body_len,
                                                ed25519_pk) == 0;
}

static naming_cache_entry_t *cache_find(trellis_naming_t *n, const char *alias)
{
    for (size_t i = 0; i < n->cache_count; i++)
        if (strcmp(n->cache[i].alias, alias) == 0) return &n->cache[i];
    return NULL;
}

static naming_cache_entry_t *cache_alloc(trellis_naming_t *n)
{
    if (n->cache_count == n->cache_cap) {
        size_t new_cap = n->cache_cap ? n->cache_cap * 2 : 16;
        naming_cache_entry_t *tmp = realloc(n->cache,
                                             new_cap * sizeof(*tmp));
        if (!tmp) return NULL;
        n->cache = tmp;
        n->cache_cap = new_cap;
    }
    return &n->cache[n->cache_count++];
}

// DHT lookup callback.
typedef struct {
    trellis_naming_t        *naming;
    char                     alias[NAMING_MAX_ALIAS_LEN + 1];
    trellis_naming_resolve_cb cb;
    void                    *ctx;
} name_lookup_ctx_t;

static void on_name_dht_result(const uint8_t *value, size_t len,
                                 trellis_err_t err, void *ctx)
{
    name_lookup_ctx_t *lctx = ctx;

    if (err != TRELLIS_OK || !value || len == 0) {
        lctx->cb(NULL, NULL, err, lctx->ctx);
        free(lctx);
        return;
    }

    char alias[NAMING_MAX_ALIAS_LEN + 1];
    trellis_fingerprint_t fp;
    uint64_t ts_ms;
    const uint8_t *sig;

    if (!naming_record_parse(value, len, alias, &fp, &ts_ms, &sig)) {
        lctx->cb(NULL, NULL, TRELLIS_ERR_MSG_FORMAT, lctx->ctx);
        free(lctx);
        return;
    }

    // Cache the result.
    naming_cache_entry_t *entry = cache_find(lctx->naming, alias);
    if (!entry) {
        entry = cache_alloc(lctx->naming);
        if (entry) {
            strncpy(entry->alias, alias, NAMING_MAX_ALIAS_LEN);
            entry->alias[NAMING_MAX_ALIAS_LEN] = '\0';
        }
    }
    if (entry) {
        entry->fingerprint = fp;
        entry->registered_ms = ts_ms;
    }

    lctx->cb(alias, &fp, TRELLIS_OK, lctx->ctx);
    free(lctx);
}

// ----

static void naming_publish_one(trellis_naming_t *n, const char *alias);

static void naming_publish_timer_cb(uv_timer_t *timer)
{
    trellis_naming_t *n = timer->data;
    for (size_t i = 0; i < n->own_alias_count; i++)
        naming_publish_one(n, n->own_aliases[i]);
}

static void naming_publish_one(trellis_naming_t *n, const char *alias)
{
    if (!n->self_id) return;

    uint64_t ts_ms = naming_now_ms();

    // Build record body (without sig)
    uint8_t body[NAMING_RECORD_MAX];
    size_t body_len = naming_record_serialize(alias, &n->self_id->fingerprint,
                                               ts_ms, NULL, body, sizeof(body));
    if (body_len == 0) return;

    // Sign with Ed25519.
    uint8_t sig[64];
    crypto_sign_ed25519_detached(sig, NULL,
                                  body, body_len,
                                  n->self_id->ed25519_sk);

    // Full record with signature.
    uint8_t record[NAMING_RECORD_MAX];
    size_t record_len = naming_record_serialize(alias, &n->self_id->fingerprint,
                                                 ts_ms, sig, record, sizeof(record));
    if (record_len == 0) return;

    // DHT key.
    uint8_t dht_key[NAMING_DHT_KEY_LEN];
    alias_dht_key(alias, dht_key);

    trellis_dht_store_redundant(n->dht, dht_key, NAMING_DHT_KEY_LEN,
                                 record, record_len,
                                 &n->self_id->fingerprint, NULL, NULL);
}

// Public API.
trellis_naming_t *trellis_naming_new(trellis_dht_t *dht,
                                      trellis_identity_t *self_id,
                                      uv_loop_t *loop)
{
    if (!dht || !loop) return NULL;

    trellis_naming_t *n = calloc(1, sizeof(*n));
    if (!n) return NULL;

    n->dht     = dht;
    n->self_id = self_id;
    n->loop    = loop;

    uv_timer_init(loop, &n->publish_timer);
    n->publish_timer.data = n;

    return n;
}

void trellis_naming_free(trellis_naming_t *n)
{
    if (!n) return;
    if (n->timer_active) {
        uv_timer_stop(&n->publish_timer);
        uv_close((uv_handle_t *)&n->publish_timer, NULL);
    }
    free(n->cache);
    free(n);
}

/*
 * Register an alias pointing to our own identity.
 * This publishes a signed record to the DHT and starts periodic re-publishing.
 */
trellis_err_t trellis_naming_register(trellis_naming_t *n, const char *raw_alias)
{
    if (!n || !raw_alias || !n->self_id) return TRELLIS_ERR_INVALID_ARG;
    if (n->own_alias_count >= 8) return TRELLIS_ERR_LIMIT_EXCEEDED;

    char alias[NAMING_MAX_ALIAS_LEN + 1];
    trellis_err_t err = alias_normalize(raw_alias, alias, sizeof(alias));
    if (err != TRELLIS_OK) return err;

    // Check for duplicate.
    for (size_t i = 0; i < n->own_alias_count; i++)
        if (strcmp(n->own_aliases[i], alias) == 0) return TRELLIS_OK;

    strncpy(n->own_aliases[n->own_alias_count++], alias, NAMING_MAX_ALIAS_LEN);

    // Publish immediately.
    naming_publish_one(n, alias);

    // Start republish timer if not already running.
    if (!n->timer_active) {
        uv_timer_start(&n->publish_timer, naming_publish_timer_cb,
                        NAMING_PUBLISH_TTL_MS, NAMING_PUBLISH_TTL_MS);
        n->timer_active = true;
    }
    return TRELLIS_OK;
}

/*
 * Resolve an alias to a fingerprint via the DHT.
 * Result delivered asynchronously via callback.
 */
trellis_err_t trellis_naming_resolve(trellis_naming_t *n,
                                      const char *raw_alias,
                                      trellis_naming_resolve_cb cb,
                                      void *ctx)
{
    if (!n || !raw_alias || !cb) return TRELLIS_ERR_INVALID_ARG;

    char alias[NAMING_MAX_ALIAS_LEN + 1];
    trellis_err_t err = alias_normalize(raw_alias, alias, sizeof(alias));
    if (err != TRELLIS_OK) return err;

    // Check local cache first.
    naming_cache_entry_t *cached = cache_find(n, alias);
    if (cached) {
        cb(alias, &cached->fingerprint, TRELLIS_OK, ctx);
        return TRELLIS_OK;
    }

    // Async DHT lookup.
    name_lookup_ctx_t *lctx = malloc(sizeof(*lctx));
    if (!lctx) return TRELLIS_ERR_NOMEM;

    lctx->naming = n;
    strncpy(lctx->alias, alias, NAMING_MAX_ALIAS_LEN);
    lctx->alias[NAMING_MAX_ALIAS_LEN] = '\0';
    lctx->cb  = cb;
    lctx->ctx = ctx;

    uint8_t dht_key[NAMING_DHT_KEY_LEN];
    alias_dht_key(alias, dht_key);

    return trellis_dht_find_value_redundant(n->dht, dht_key, NAMING_DHT_KEY_LEN,
                                             on_name_dht_result, lctx);
}

/*
 * Verify a raw naming record (from DHT) and update the local cache.
 * The ed25519_pk must be the public key of the fingerprint in the record.
 * Returns TRELLIS_OK if valid, updates cache.
 */
trellis_err_t trellis_naming_verify_record(trellis_naming_t *n,
                                            const uint8_t *record,
                                            size_t record_len,
                                            const uint8_t *ed25519_pk)
{
    if (!n || !record || !ed25519_pk) return TRELLIS_ERR_INVALID_ARG;

    char alias[NAMING_MAX_ALIAS_LEN + 1];
    trellis_fingerprint_t fp;
    uint64_t ts_ms;
    const uint8_t *sig;

    if (!naming_record_parse(record, record_len, alias, &fp, &ts_ms, &sig))
        return TRELLIS_ERR_MSG_FORMAT;

    if (!naming_record_verify(record, record_len, ed25519_pk))
        return TRELLIS_ERR_VERIFY_FAILED;

    // Conflict resolution: first-registration wins.
    naming_cache_entry_t *entry = cache_find(n, alias);
    if (entry) {
        if (ts_ms > entry->registered_ms) return TRELLIS_ERR_CONFLICT;
        if (ts_ms == entry->registered_ms) {
            // Tie-break: lexicographic minimum fingerprint wins.
            if (memcmp(fp.bytes, entry->fingerprint.bytes, 32) > 0)
                return TRELLIS_ERR_CONFLICT;
        }
        entry->fingerprint    = fp;
        entry->registered_ms  = ts_ms;
    } else {
        entry = cache_alloc(n);
        if (!entry) return TRELLIS_ERR_NOMEM;
        strncpy(entry->alias, alias, NAMING_MAX_ALIAS_LEN);
        entry->alias[NAMING_MAX_ALIAS_LEN] = '\0';
        entry->fingerprint   = fp;
        entry->registered_ms = ts_ms;
    }
    return TRELLIS_OK;
}

// Petname system.

/*
 * Set a local petname for a fingerprint.
 * Petnames are purely local and never shared over the network.
 */
trellis_err_t trellis_naming_set_petname(trellis_naming_t *n,
                                          const trellis_fingerprint_t *fp,
                                          const char *label)
{
    if (!n || !fp || !label) return TRELLIS_ERR_INVALID_ARG;
    if (strlen(label) > NAMING_MAX_ALIAS_LEN) return TRELLIS_ERR_INVALID_ARG;

    // Update existing.
    for (size_t i = 0; i < n->petname_count; i++) {
        if (memcmp(n->petnames[i].fingerprint.bytes, fp->bytes, 32) == 0) {
            strncpy(n->petnames[i].label, label, NAMING_MAX_ALIAS_LEN);
            n->petnames[i].label[NAMING_MAX_ALIAS_LEN] = '\0';
            return TRELLIS_OK;
        }
    }

    if (n->petname_count >= PETNAME_MAX) return TRELLIS_ERR_LIMIT_EXCEEDED;

    petname_entry_t *pe = &n->petnames[n->petname_count++];
    pe->fingerprint = *fp;
    strncpy(pe->label, label, NAMING_MAX_ALIAS_LEN);
    pe->label[NAMING_MAX_ALIAS_LEN] = '\0';
    return TRELLIS_OK;
}

/*
 * Look up the local petname for a fingerprint.
 * Returns NULL if no petname is set.
 */
const char *trellis_naming_get_petname(const trellis_naming_t *n,
                                        const trellis_fingerprint_t *fp)
{
    if (!n || !fp) return NULL;
    for (size_t i = 0; i < n->petname_count; i++)
        if (memcmp(n->petnames[i].fingerprint.bytes, fp->bytes, 32) == 0)
            return n->petnames[i].label;
    return NULL;
}

/*
 * Remove a petname.
 */
trellis_err_t trellis_naming_remove_petname(trellis_naming_t *n,
                                             const trellis_fingerprint_t *fp)
{
    if (!n || !fp) return TRELLIS_ERR_INVALID_ARG;
    for (size_t i = 0; i < n->petname_count; i++) {
        if (memcmp(n->petnames[i].fingerprint.bytes, fp->bytes, 32) == 0) {
            n->petnames[i] = n->petnames[--n->petname_count];
            return TRELLIS_OK;
        }
    }
    return TRELLIS_ERR_NOT_FOUND;
}

// DNS TXT bridge.

/*
 * Parse a DNS TXT record in Bloom bridge format.
 * Format: "bloom-alias=<base64url(fingerprint)>"
 *
 * If valid, inserts the alias -> fingerprint mapping into the DHT cache.
 */
trellis_err_t trellis_naming_import_dns_txt(trellis_naming_t *n,
                                             const char *alias,
                                             const char *txt_record)
{
    if (!n || !alias || !txt_record) return TRELLIS_ERR_INVALID_ARG;

    static const char prefix[] = "bloom-fp=";
    if (strncmp(txt_record, prefix, sizeof(prefix) - 1) != 0)
        return TRELLIS_ERR_MSG_FORMAT;

    const char *b64 = txt_record + sizeof(prefix) - 1;
    size_t b64_len  = strlen(b64);

    // Base64 decode fingerprint (44 chars for 32 bytes in standard base64)
    uint8_t fp_bytes[32];
    size_t decoded_len = 0;
    if (sodium_base642bin(fp_bytes, sizeof(fp_bytes),
                           b64, b64_len,
                           NULL, &decoded_len, NULL,
                           sodium_base64_VARIANT_URLSAFE_NO_PADDING) != 0
        || decoded_len != 32)
        return TRELLIS_ERR_MSG_FORMAT;

    char normalized[NAMING_MAX_ALIAS_LEN + 1];
    trellis_err_t err = alias_normalize(alias, normalized, sizeof(normalized));
    if (err != TRELLIS_OK) return err;

    // Insert into cache (without full signature verification — DNS is advisory)
    naming_cache_entry_t *entry = cache_find(n, normalized);
    if (!entry) {
        entry = cache_alloc(n);
        if (!entry) return TRELLIS_ERR_NOMEM;
        strncpy(entry->alias, normalized, NAMING_MAX_ALIAS_LEN);
        entry->alias[NAMING_MAX_ALIAS_LEN] = '\0';
        entry->registered_ms = 0;
    }
    memcpy(entry->fingerprint.bytes, fp_bytes, 32);
    return TRELLIS_OK;
}

/*
 * Generate a DNS TXT record string for publishing our own alias via DNS.
 * The caller should publish this as a TXT record at <alias>.users.bloom.
 *
 * Output format: "bloom-fp=<base64url(fingerprint)>"
 */
trellis_err_t trellis_naming_export_dns_txt(const trellis_naming_t *n,
                                             const char *alias,
                                             char *out, size_t out_cap)
{
    if (!n || !alias || !out || !n->self_id) return TRELLIS_ERR_INVALID_ARG;

    char b64[64];
    sodium_bin2base64(b64, sizeof(b64),
                      n->self_id->fingerprint.bytes, 32,
                      sodium_base64_VARIANT_URLSAFE_NO_PADDING);

    int written = snprintf(out, out_cap, "bloom-fp=%s", b64);
    if (written < 0 || (size_t)written >= out_cap)
        return TRELLIS_ERR_INVALID_ARG;
    return TRELLIS_OK;
}
