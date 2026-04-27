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
 * pir.c — Private Information Retrieval for DHT key lookups
 *
 * Threat model: A DHT node that receives a FIND_VALUE request learns that
 * *some* client (at the Bloom level, pseudonymous) is interested in key K.
 * If K is a greenhouse service descriptor key, this reveals which service
 * the client wants to connect to.
 *
 * Mitigation — Cover-Query PIR:
 *   For each real lookup of key K, the PIR layer fires PIR_COVER_COUNT
 *   additional FIND_VALUE requests for pseudorandom "cover keys" derived
 *   as HKDF-SHAKE-256(K, salt, "trellis_pir_cover" || i).  These cover keys do not correspond to
 *   any real service, but they are indistinguishable from real service keys
 *   to any DHT node that sees them.  The real response is held by the
 *   client; cover responses are discarded.
 *
 *   Because the cover keys are derived deterministically from a per-session
 *   secret (never shared), an adversary who controls up to
 *   PIR_COVER_COUNT DHT nodes, each seeing at most one query per lookup,
 *   cannot determine which of the PIR_COVER_COUNT+1 keys was real with
 *   probability better than 1/(PIR_COVER_COUNT+1).
 *
 *   The cover keys are regenerated with a fresh random salt per lookup,
 *   preventing correlation between lookups for the same real key.
 *
 * Integration: Replace calls to trellis_dht_find_value() for greenhouse
 * descriptor lookups with trellis_pir_find_value().  The callback
 * signature is identical to trellis_dht_find_cb.
 *
 * Optional upgrade path: When the DHT has ≥ PIR_COVER_COUNT+1 disjoint
 * nodes available, combine with S/Kademlia (trellis_dht_find_value_skad)
 * so each cover query goes to a different lookup path.
 */

#include "internal.h"
#include "trellis/mesh.h"
#include "trellis/error.h"
#include "trellis/crypto.h"

#include <sodium.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <uv.h>

/* Number of cover queries per real lookup.  Higher = more privacy, more
 * bandwidth.  4 means an observer needs to control all 5 queried keys'
 * DHT neighbourhoods to identify the real one. */
#define PIR_COVER_COUNT        4

// Maximum key length we support.
#define PIR_MAX_KEY_LEN        64

// PIR context for one in-flight lookup.
typedef struct pir_lookup {
    bool            active;

    // Real key.
    uint8_t         real_key[PIR_MAX_KEY_LEN];
    size_t          real_key_len;

    // User callback.
    trellis_dht_find_cb  cb;
    void                *ctx;

    // Tracking: which sub-queries are still outstanding.
    int             total;        /* = PIR_COVER_COUNT + 1            */
    int             done;         /* how many have completed           */
    bool            real_found;   /* did the real key return a result? */
    uint8_t        *real_value;
    size_t          real_value_len;

    struct pir_lookup *self;      /* pointer back for cover callbacks  */
    trellis_dht_t  *dht;
} pir_lookup_t;

// Per-cover callback context.
typedef struct pir_sub_ctx {
    pir_lookup_t *lookup;
    int           idx; /* 0 = real, 1..PIR_COVER_COUNT = cover */
} pir_sub_ctx_t;

#define PIR_MAX_CONCURRENT  16

static pir_lookup_t g_pir_lookups[PIR_MAX_CONCURRENT];

/* Mutex protecting the global lookup table against concurrent access from
 * multiple threads or multiple trellis_dht_t instances in the same process. */
static uv_mutex_t g_pir_mutex;
static uv_once_t  g_pir_mutex_once = UV_ONCE_INIT;

static void pir_mutex_init_once(void)
{
    uv_mutex_init(&g_pir_mutex);
}

static pir_lookup_t *pir_alloc(void)
{
    uv_once(&g_pir_mutex_once, pir_mutex_init_once);
    uv_mutex_lock(&g_pir_mutex);
    for (int i = 0; i < PIR_MAX_CONCURRENT; i++) {
        if (!g_pir_lookups[i].active) {
            memset(&g_pir_lookups[i], 0, sizeof(pir_lookup_t));
            g_pir_lookups[i].active = true;
            g_pir_lookups[i].self   = &g_pir_lookups[i];
            uv_mutex_unlock(&g_pir_mutex);
            return &g_pir_lookups[i];
        }
    }
    uv_mutex_unlock(&g_pir_mutex);
    return NULL;
}

static void pir_free(pir_lookup_t *p)
{
    uv_once(&g_pir_mutex_once, pir_mutex_init_once);
    uv_mutex_lock(&g_pir_mutex);
    free(p->real_value);
    p->real_value     = NULL;
    p->real_value_len = 0;
    sodium_memzero(p->real_key, sizeof(p->real_key));
    p->real_key_len   = 0;
    p->active         = false;
    uv_mutex_unlock(&g_pir_mutex);
}

/*
 * Called when any sub-query (real or cover) completes.
 */
static void pir_sub_cb(const uint8_t *value, size_t value_len,
                        trellis_err_t err, void *raw_ctx)
{
    pir_sub_ctx_t *sc = (pir_sub_ctx_t *)raw_ctx;
    pir_lookup_t  *p  = sc->lookup;
    bool           is_real = sc->idx == 0;
    free(sc);

    if (!p || !p->active)
        return;

    if (is_real && err == TRELLIS_OK && value && value_len > 0) {
        p->real_found = true;
        p->real_value = malloc(value_len);
        if (p->real_value) {
            memcpy(p->real_value, value, value_len);
            p->real_value_len = value_len;
        }
    }

    p->done++;

    if (p->done < p->total)
        return;

    // All sub-queries done — deliver result to caller.
    trellis_dht_find_cb user_cb  = p->cb;
    void               *user_ctx = p->ctx;

    if (p->real_found) {
        uint8_t *v   = p->real_value;
        size_t   vl  = p->real_value_len;
        pir_free(p);
        if (user_cb) user_cb(v, vl, TRELLIS_OK, user_ctx);
        free(v);
    } else {
        pir_free(p);
        if (user_cb) user_cb(NULL, 0, TRELLIS_ERR_NOT_FOUND, user_ctx);
    }
}

/*
 * Derive a cover key from the real key using a per-lookup random salt and
 * an index via HKDF-SHAKE-256 (matching the core spec's KDF choice).
 *
 * cover_key = HKDF-SHAKE-256(ikm=real_key, salt=random_salt,
 *                            info="trellis_pir_cover" || index_byte)
 */
static void pir_derive_cover_key(const uint8_t *real_key, size_t real_key_len,
                                  const uint8_t salt[32],
                                  int idx,
                                  uint8_t *out, size_t out_len)
{
    static const char label[] = "trellis_pir_cover";
    uint8_t info[sizeof(label) - 1 + 1];
    memcpy(info, label, sizeof(label) - 1);
    info[sizeof(label) - 1] = (uint8_t)idx;

    trellis_hkdf_shake256(real_key, real_key_len,
                          salt, 32,
                          info, sizeof(info),
                          out, out_len);
}

/*
 * trellis_pir_find_value — drop-in replacement for trellis_dht_find_value
 * for privacy-sensitive lookups (greenhouse descriptor keys).
 *
 * Fires PIR_COVER_COUNT+1 parallel FIND_VALUE queries: the real one plus
 * PIR_COVER_COUNT cover queries for pseudorandom keys.  The callback
 * receives the real value (or NOT_FOUND) only after all queries complete.
 *
 * @dht       DHT handle
 * @key       real lookup key
 * @key_len   key length (≤ PIR_MAX_KEY_LEN)
 * @cb        result callback (same signature as trellis_dht_find_cb)
 * @ctx       user context
 */
trellis_err_t trellis_pir_find_value(trellis_dht_t *dht,
                                      const uint8_t *key, size_t key_len,
                                      trellis_dht_find_cb cb, void *ctx)
{
    if (!dht || !key || key_len == 0 || key_len > PIR_MAX_KEY_LEN)
        return TRELLIS_ERR_INVALID_ARG;

    pir_lookup_t *p = pir_alloc();
    if (!p) {
        // Fallback to unprotected lookup if no slots available.
        fprintf(stderr, "[PIR] no free slots, falling back to plain lookup\n");
        return trellis_dht_find_value(dht, key, key_len, cb, ctx);
    }

    memcpy(p->real_key, key, key_len);
    p->real_key_len = key_len;
    p->cb           = cb;
    p->ctx          = ctx;
    p->dht          = dht;
    p->total        = PIR_COVER_COUNT + 1;
    p->done         = 0;
    p->real_found   = false;

    // Random salt for this lookup's cover key derivation.
    uint8_t salt[32];
    randombytes_buf(salt, sizeof(salt));

    /* Fire all sub-queries.  If any allocation fails, mark that sub-query
     * as instantly done with NOT_FOUND so the count still closes properly. */
    for (int i = 0; i <= PIR_COVER_COUNT; i++) {
        pir_sub_ctx_t *sc = malloc(sizeof(*sc));
        if (!sc) {
            p->done++;
            continue;
        }
        sc->lookup = p;
        sc->idx    = i;

        size_t done_before = p->done;
        trellis_err_t err;
        if (i == 0) {
            err = trellis_dht_find_value(dht, key, key_len,
                                          pir_sub_cb, sc);
        } else {
            uint8_t cover_key[PIR_MAX_KEY_LEN];
            pir_derive_cover_key(key, key_len, salt, i,
                                  cover_key, key_len);
            err = trellis_dht_find_value(dht, cover_key, key_len,
                                          pir_sub_cb, sc);
            sodium_memzero(cover_key, sizeof(cover_key));
        }

        if (err != TRELLIS_OK && p->done == done_before) {
            free(sc);
            p->done++;
        }
    }
    sodium_memzero(salt, sizeof(salt));

    // If every sub-query failed synchronously, deliver NOT_FOUND now.
    if (p->done == p->total) {
        pir_free(p);
        if (cb) cb(NULL, 0, TRELLIS_ERR_NOT_FOUND, ctx);
    }

    return TRELLIS_OK;
}

/*
 * trellis_pir_find_value_skad — same as trellis_pir_find_value but uses
 * the S/Kademlia disjoint-path variant for each sub-query, giving both
 * PIR privacy and Sybil resistance.
 */
trellis_err_t trellis_pir_find_value_skad(trellis_dht_t *dht,
                                           const uint8_t *key, size_t key_len,
                                           trellis_dht_find_cb cb, void *ctx)
{
    if (!dht || !key || key_len == 0 || key_len > PIR_MAX_KEY_LEN)
        return TRELLIS_ERR_INVALID_ARG;

    pir_lookup_t *p = pir_alloc();
    if (!p) {
        fprintf(stderr, "[PIR/skad] no free slots, falling back\n");
        return trellis_dht_find_value_skad(dht, key, key_len, cb, ctx);
    }

    memcpy(p->real_key, key, key_len);
    p->real_key_len = key_len;
    p->cb           = cb;
    p->ctx          = ctx;
    p->dht          = dht;
    p->total        = PIR_COVER_COUNT + 1;
    p->done         = 0;
    p->real_found   = false;

    uint8_t salt[32];
    randombytes_buf(salt, sizeof(salt));

    for (int i = 0; i <= PIR_COVER_COUNT; i++) {
        pir_sub_ctx_t *sc = malloc(sizeof(*sc));
        if (!sc) {
            p->done++;
            continue;
        }
        sc->lookup = p;
        sc->idx    = i;

        size_t done_before = p->done;
        trellis_err_t err;
        if (i == 0) {
            err = trellis_dht_find_value_skad(dht, key, key_len,
                                               pir_sub_cb, sc);
        } else {
            uint8_t cover_key[PIR_MAX_KEY_LEN];
            pir_derive_cover_key(key, key_len, salt, i,
                                  cover_key, key_len);
            err = trellis_dht_find_value_skad(dht, cover_key, key_len,
                                               pir_sub_cb, sc);
            sodium_memzero(cover_key, sizeof(cover_key));
        }

        if (err != TRELLIS_OK && p->done == done_before) {
            free(sc);
            p->done++;
        }
    }
    sodium_memzero(salt, sizeof(salt));

    if (p->done == p->total) {
        pir_free(p);
        if (cb) cb(NULL, 0, TRELLIS_ERR_NOT_FOUND, ctx);
    }

    return TRELLIS_OK;
}
