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
 * canary.c — Warrant Canary Infrastructure
 *
 * Operators publish signed, timestamped attestations ("canaries") to the
 * DHT.  A canary states: "As of timestamp T, this node has not received a
 * legal demand (NSL, gag order, warrant) forcing it to betray its users."
 *
 * Clients that route through a node MUST verify its canary is:
 *   (a) present in the DHT
 *   (b) signed with the node's identity key
 *   (c) within CANARY_MAX_AGE_MS of the current time
 *
 * If a node fails to publish a fresh canary (i.e. it has been silenced),
 * clients automatically refuse to route traffic through it.  This is the
 * "dead canary" property: the *absence* of an attestation signals danger.
 *
 * Canary format (signed with SLH-DSA / ML-DSA or Ed25519):
 *
 *   struct trellis_canary {
 *       uint8_t  magic[4];           // "CNAR"
 *       uint8_t  version;            // 1
 *       uint8_t  node_fp[32];        // publisher fingerprint
 *       uint64_t issued_at_ms;       // unix epoch ms
 *       uint64_t next_publish_ms;    // expected next publication
 *       uint8_t  statement[256];     // human-readable statement (UTF-8)
 *       uint8_t  statement_len;      // actual statement length
 *       uint8_t  signature[64];      // Ed25519 signature over all prior fields
 *   };
 *
 * DHT key: SHA256("trellis_canary" || node_fingerprint)
 * (consistent with all other Trellis DHT key derivations)
 * TTL:     CANARY_PUBLISH_INTERVAL_MS * 2 (nodes must refresh regularly)
 *
 * Routing integration:
 *   trellis_canary_check_peer() is called by routing.c before selecting
 *   any relay hop.  It returns TRELLIS_ERR_CANARY_EXPIRED if the peer's
 *   canary is absent or stale, causing the hop to be skipped.
 */

#include "internal.h"
#include <trellis/mesh.h>
#include <trellis/error.h>
#include <sodium.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>
#include <stdbool.h>
#include <stdint.h>
#include <inttypes.h>
#include <uv.h>

#define CANARY_MAGIC              "CNAR"
#define CANARY_VERSION            1
#define CANARY_HEADER_LEN         (4 + 1 + 32 + 8 + 8) /* magic+ver+fp+ts+next */
#define CANARY_STATEMENT_MAX      256
#define CANARY_SIG_LEN            crypto_sign_ed25519_BYTES  /* 64 */
#define CANARY_TOTAL_LEN          (CANARY_HEADER_LEN + \
                                    CANARY_STATEMENT_MAX + 1 + \
                                    CANARY_SIG_LEN)

// How long a canary is valid: 72 hours (spec default)
#define CANARY_MAX_AGE_MS         (72ULL * 60 * 60 * 1000)

// How often a node should re-publish: every 24 hours (well within the 72h window)
#define CANARY_PUBLISH_INTERVAL_MS (24ULL * 60 * 60 * 1000)

typedef struct canary_entry {
    trellis_fingerprint_t fp;
    bool                  valid;
    uint64_t              issued_at_ms;
    uint64_t              next_publish_ms;
    uint64_t              verified_at_ms;  /* when we verified this locally */
} canary_entry_t;

#define CANARY_CACHE_MAX  512

static canary_entry_t g_canary_cache[CANARY_CACHE_MAX];
static size_t         g_canary_cache_count = 0;

static uint64_t canary_now_ms(void)
{
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    return (uint64_t)ts.tv_sec * 1000 + (uint64_t)ts.tv_nsec / 1000000;
}

static canary_entry_t *canary_cache_find(const trellis_fingerprint_t *fp)
{
    for (size_t i = 0; i < g_canary_cache_count; i++) {
        if (g_canary_cache[i].valid &&
            trellis_fingerprint_eq(&g_canary_cache[i].fp, fp))
            return &g_canary_cache[i];
    }
    return NULL;
}

static canary_entry_t *canary_cache_alloc(const trellis_fingerprint_t *fp)
{
    // Find existing or allocate new.
    canary_entry_t *e = canary_cache_find(fp);
    if (e) return e;

    if (g_canary_cache_count < CANARY_CACHE_MAX) {
        e = &g_canary_cache[g_canary_cache_count++];
        memset(e, 0, sizeof(*e));
        e->fp = *fp;
        return e;
    }

    // Evict oldest entry.
    e = &g_canary_cache[0];
    for (size_t i = 1; i < g_canary_cache_count; i++) {
        if (g_canary_cache[i].verified_at_ms < e->verified_at_ms)
            e = &g_canary_cache[i];
    }
    memset(e, 0, sizeof(*e));
    e->fp = *fp;
    return e;
}

void trellis_canary_dht_key(const trellis_fingerprint_t *fp,
                              uint8_t *key_out)
{
    static const char prefix[] = "trellis_canary";
    crypto_hash_sha256_state hs;
    crypto_hash_sha256_init(&hs);
    crypto_hash_sha256_update(&hs, (const uint8_t *)prefix, strlen(prefix));
    crypto_hash_sha256_update(&hs, fp->bytes, TRELLIS_FINGERPRINT_LEN);
    crypto_hash_sha256_final(&hs, key_out);
}

/*
 * Serialize a canary record into buf (must be at least CANARY_TOTAL_LEN bytes).
 * Signs with the node's Ed25519 signing key.
 * Returns the actual number of bytes written, or 0 on error.
 */
size_t trellis_canary_serialize(
        const trellis_identity_t *id,
        const char *statement,  /* human-readable attestation text */
        uint8_t *buf, size_t buf_cap)
{
    if (!id || !buf || buf_cap < CANARY_TOTAL_LEN)
        return 0;

    memset(buf, 0, CANARY_TOTAL_LEN);

    size_t off = 0;
    memcpy(buf + off, CANARY_MAGIC, 4);            off += 4;
    buf[off++] = CANARY_VERSION;
    memcpy(buf + off, id->fingerprint.bytes, 32);  off += 32;

    uint64_t now_ms  = canary_now_ms();
    uint64_t next_ms = now_ms + CANARY_PUBLISH_INTERVAL_MS;

    // issued_at_ms (little-endian 64-bit)
    for (int b = 0; b < 8; b++)
        buf[off++] = (uint8_t)(now_ms >> (b * 8));

    // next_publish_ms
    for (int b = 0; b < 8; b++)
        buf[off++] = (uint8_t)(next_ms >> (b * 8));

    // statement
    size_t stmt_len = 0;
    if (statement) {
        stmt_len = strlen(statement);
        if (stmt_len > CANARY_STATEMENT_MAX)
            stmt_len = CANARY_STATEMENT_MAX;
        memcpy(buf + off, statement, stmt_len);
    }
    off += CANARY_STATEMENT_MAX;
    buf[off++] = (uint8_t)stmt_len;

    // Signature over everything so far.
    unsigned long long sig_buf_len;
    uint8_t signed_msg[CANARY_SIG_LEN + CANARY_HEADER_LEN +
                        CANARY_STATEMENT_MAX + 1];
    if (crypto_sign_ed25519(signed_msg, &sig_buf_len,
                             buf, off,
                             id->ed25519_sk) != 0)
        return 0;

    memcpy(buf + off, signed_msg, CANARY_SIG_LEN);
    off += CANARY_SIG_LEN;

    return off;
}

/*
 * Deserialize and verify a canary record.
 *
 * @data           raw canary bytes from DHT
 * @data_len       length of data
 * @issuer_pubkey  Ed25519 public key of the node that supposedly signed this
 * @issued_at_ms_out  (optional) output: when this canary was issued
 *
 * Returns TRELLIS_OK if valid and fresh.
 * Returns TRELLIS_ERR_VERIFY_FAILED if signature invalid.
 * Returns TRELLIS_ERR_EXPIRED if canary is too old.
 */
trellis_err_t trellis_canary_verify(
        const uint8_t *data, size_t data_len,
        const uint8_t *issuer_pubkey,
        const trellis_fingerprint_t *expected_fp,
        uint64_t *issued_at_ms_out)
{
    if (!data || !issuer_pubkey)
        return TRELLIS_ERR_INVALID_ARG;
    if (data_len < CANARY_TOTAL_LEN)
        return TRELLIS_ERR_MSG_FORMAT;

    // Check magic.
    if (memcmp(data, CANARY_MAGIC, 4) != 0)
        return TRELLIS_ERR_MSG_FORMAT;
    if (data[4] != CANARY_VERSION)
        return TRELLIS_ERR_MSG_FORMAT;

    size_t signed_len = CANARY_TOTAL_LEN - CANARY_SIG_LEN;
    const uint8_t *sig = data + signed_len;

    // Verify Ed25519 signature (detached: sig is appended, message is the body)
    if (crypto_sign_ed25519_verify_detached(sig, data, signed_len,
                                             issuer_pubkey) != 0) {
        fprintf(stderr, "[canary] signature verification failed\n");
        return TRELLIS_ERR_VERIFY_FAILED;
    }

    // Verify embedded node_fp matches the expected peer identity.
    if (expected_fp &&
        memcmp(data + 5, expected_fp->bytes, TRELLIS_FINGERPRINT_LEN) != 0) {
        fprintf(stderr, "[canary] embedded fingerprint mismatch\n");
        return TRELLIS_ERR_VERIFY_FAILED;
    }

    // Extract issued_at_ms.
    size_t off = 4 + 1 + 32;  /* magic + version + fp */
    uint64_t issued_ms = 0;
    for (int b = 0; b < 8; b++)
        issued_ms |= ((uint64_t)data[off + b]) << (b * 8);

    if (issued_at_ms_out)
        *issued_at_ms_out = issued_ms;

    // Check freshness (use adjusted time if available for clock skew tolerance)
    uint64_t now_ms = canary_now_ms();
    uint64_t adjusted = now_ms;
    // Allow +-30s extra tolerance for clock skew between peers.
    uint64_t tolerance = 30000;
    if (adjusted > issued_ms + CANARY_MAX_AGE_MS + tolerance) {
        fprintf(stderr, "[canary] expired: issued %" PRIu64 " ms ago, max %"
                PRIu64 " ms\n", adjusted - issued_ms, CANARY_MAX_AGE_MS);
        return TRELLIS_ERR_EXPIRED;
    }

    return TRELLIS_OK;
}

typedef struct canary_fetch_ctx {
    trellis_fingerprint_t fp;
    uint8_t               pubkey[crypto_sign_ed25519_PUBLICKEYBYTES];
    trellis_canary_cb     cb;
    void                 *ctx;
} canary_fetch_ctx_t;

static void on_canary_dht_result(const uint8_t *value, size_t value_len,
                                  trellis_err_t err, void *raw_ctx)
{
    canary_fetch_ctx_t *fc = (canary_fetch_ctx_t *)raw_ctx;

    if (err != TRELLIS_OK || !value) {
        fprintf(stderr, "[canary] DHT lookup failed for peer, assuming stale\n");
        if (fc->cb) fc->cb(&fc->fp, false, fc->ctx);
        free(fc);
        return;
    }

    uint64_t issued_ms = 0;
    trellis_err_t verr = trellis_canary_verify(value, value_len,
                                                fc->pubkey, &fc->fp,
                                                &issued_ms);
    bool fresh = (verr == TRELLIS_OK);

    if (fresh) {
        canary_entry_t *e = canary_cache_alloc(&fc->fp);
        if (e) {
            e->valid          = true;
            e->issued_at_ms   = issued_ms;
            e->verified_at_ms = canary_now_ms();
            // next_publish_ms from canary body.
            size_t off = 4 + 1 + 32 + 8;
            e->next_publish_ms = 0;
            for (int b = 0; b < 8; b++)
                e->next_publish_ms |= ((uint64_t)value[off + b]) << (b * 8);
        }
    }

    if (fc->cb) fc->cb(&fc->fp, fresh, fc->ctx);
    free(fc);
}

/*
 * Asynchronously fetch and verify a node's canary from the DHT.
 * Calls cb(fp, is_fresh, ctx) when done.
 */
trellis_err_t trellis_canary_fetch(
        trellis_dht_t *dht,
        const trellis_fingerprint_t *fp,
        const uint8_t *node_pubkey,       /* Ed25519 public key, 32 bytes */
        trellis_canary_cb cb, void *ctx)
{
    if (!dht || !fp || !node_pubkey)
        return TRELLIS_ERR_INVALID_ARG;

    // Check cache first.
    canary_entry_t *e = canary_cache_find(fp);
    if (e && e->valid) {
        uint64_t age_ms = canary_now_ms() - e->issued_at_ms;
        if (age_ms < CANARY_MAX_AGE_MS) {
            if (cb) cb(fp, true, ctx);
            return TRELLIS_OK;
        }
        e->valid = false;  /* expired */
    }

    canary_fetch_ctx_t *fc = malloc(sizeof(*fc));
    if (!fc) return TRELLIS_ERR_NOMEM;
    fc->fp  = *fp;
    memcpy(fc->pubkey, node_pubkey, crypto_sign_ed25519_PUBLICKEYBYTES);
    fc->cb  = cb;
    fc->ctx = ctx;

    uint8_t dht_key[32];
    trellis_canary_dht_key(fp, dht_key);

    return trellis_dht_find_value(dht, dht_key, sizeof(dht_key),
                                   on_canary_dht_result, fc);
}

trellis_err_t trellis_canary_check_peer_adjusted(
    const trellis_fingerprint_t *fp, const trellis_dht_t *dht);

/*
 * Synchronous check: is the cached canary for this peer fresh?
 *
 * Returns TRELLIS_OK if the cache says the peer has a valid canary.
 * Returns TRELLIS_ERR_NOT_FOUND if not cached (caller should call
 * trellis_canary_fetch first).
 * Returns TRELLIS_ERR_EXPIRED if the cached canary has aged out.
 */
trellis_err_t trellis_canary_check_peer(const trellis_fingerprint_t *fp)
{
    return trellis_canary_check_peer_adjusted(fp, NULL);
}

trellis_err_t trellis_canary_check_peer_adjusted(const trellis_fingerprint_t *fp,
                                                  const trellis_dht_t *dht)
{
    if (!fp) return TRELLIS_ERR_INVALID_ARG;

    canary_entry_t *e = canary_cache_find(fp);
    if (!e || !e->valid)
        return TRELLIS_ERR_NOT_FOUND;

    uint64_t now = dht ? trellis_adjusted_now_ms(dht) : canary_now_ms();
    uint64_t age_ms = now - e->issued_at_ms;
    if (age_ms > CANARY_MAX_AGE_MS) {
        e->valid = false;
        return TRELLIS_ERR_EXPIRED;
    }

    return TRELLIS_OK;
}

/*
 * Publish this node's canary to the DHT.
 * Should be called periodically (every CANARY_PUBLISH_INTERVAL_MS).
 */
trellis_err_t trellis_canary_publish(
        trellis_dht_t *dht,
        const trellis_identity_t *id,
        const char *statement)
{
    if (!dht || !id)
        return TRELLIS_ERR_INVALID_ARG;

    uint8_t canary_buf[CANARY_TOTAL_LEN];
    size_t  canary_len = trellis_canary_serialize(id, statement,
                                                   canary_buf, sizeof(canary_buf));
    if (canary_len == 0)
        return TRELLIS_ERR_SIGN_FAILED;

    uint8_t dht_key[32];
    trellis_canary_dht_key(&id->fingerprint, dht_key);

    fprintf(stderr, "[canary] publishing canary for this node\n");
    return trellis_dht_store(dht, dht_key, sizeof(dht_key),
                              canary_buf, canary_len, NULL, NULL);
}

typedef struct canary_timer_ctx {
    uv_timer_t            timer;
    trellis_dht_t        *dht;
    const trellis_identity_t *id;
    char                  statement[512];
} canary_timer_ctx_t;

static void canary_publish_timer_cb(uv_timer_t *handle)
{
    canary_timer_ctx_t *c = (canary_timer_ctx_t *)handle->data;
    trellis_canary_publish(c->dht, c->id, c->statement);
}

/*
 * Start the periodic canary publication timer.
 *
 * @loop       libuv event loop
 * @dht        DHT handle
 * @id         this node's identity
 * @statement  attestation text (e.g. "No warrants received as of this date")
 *
 * Returns a heap-allocated timer context; caller must call
 * trellis_canary_timer_stop() to clean up.
 */
trellis_canary_timer_t *trellis_canary_timer_start(
        uv_loop_t *loop,
        trellis_dht_t *dht,
        const trellis_identity_t *id,
        const char *statement)
{
    if (!loop || !dht || !id)
        return NULL;

    canary_timer_ctx_t *c = calloc(1, sizeof(*c));
    if (!c) return NULL;

    c->dht = dht;
    c->id  = id;
    if (statement) {
        strncpy(c->statement, statement, sizeof(c->statement) - 1);
        c->statement[sizeof(c->statement) - 1] = '\0';
    } else {
        strncpy(c->statement,
                "No warrants, NSLs, or gag orders have been received as of this publication.",
                sizeof(c->statement) - 1);
    }

    uv_timer_init(loop, &c->timer);
    c->timer.data = c;

    // Publish immediately, then every CANARY_PUBLISH_INTERVAL_MS.
    trellis_canary_publish(dht, id, c->statement);
    uv_timer_start(&c->timer, canary_publish_timer_cb,
                   CANARY_PUBLISH_INTERVAL_MS, CANARY_PUBLISH_INTERVAL_MS);

    return (trellis_canary_timer_t *)c;
}

static void canary_timer_close_cb(uv_handle_t *handle)
{
    canary_timer_ctx_t *c = (canary_timer_ctx_t *)handle->data;
    free(c);
}

void trellis_canary_timer_stop(trellis_canary_timer_t *timer)
{
    if (!timer) return;
    canary_timer_ctx_t *c = (canary_timer_ctx_t *)timer;
    uv_timer_stop(&c->timer);
    c->timer.data = c;
    uv_close((uv_handle_t *)&c->timer, canary_timer_close_cb);
}

/*
 * Invalidate the cached canary for a peer (e.g. if we learn it has been
 * silenced or its canary was revoked via gossip).
 */
void trellis_canary_revoke(const trellis_fingerprint_t *fp)
{
    canary_entry_t *e = canary_cache_find(fp);
    if (e)
        e->valid = false;
}
