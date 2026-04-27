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
#include <trellis/crypto.h>
#include <sodium.h>

/*
 * Signed relay descriptors — verifiable self-published relay metadata.
 *
 * Each relay creates a descriptor advertising its capabilities (relay, exit,
 * guard), exit policy, claimed bandwidth, and contact info. The descriptor
 * is signed with the relay's hybrid Ed25519 + ML-DSA identity keys.
 *
 * Descriptors are stored in the DHT keyed by the relay's fingerprint.
 * Clients fetch and verify descriptors before selecting relays for circuits.
 *
 * Wire format (flat binary, not CBOR — avoids dependency):
 *   [fingerprint(32)][version(4)][published_at(8)][capabilities(4)]
 *   [exit_policy_len(2)][exit_policy(N)][bandwidth_claimed(4)]
 *   [contact_len(1)][contact(M)]
 *   [ed25519_sig(64)][ml_dsa_sig(4627)]
 */
#define DESC_HEADER_LEN (TRELLIS_FINGERPRINT_LEN + 4 + 8 + 4 + 2)

// Capability flags.
#define RELAY_CAP_RELAY    0x01
#define RELAY_CAP_EXIT     0x02
#define RELAY_CAP_GUARD    0x04
#define RELAY_CAP_GH       0x08  /* greenhouse capable */

trellis_relay_descriptor_t *trellis_relay_descriptor_create(
    const trellis_fingerprint_t *fp, uint32_t capabilities,
    uint32_t bandwidth_kbps)
{
    trellis_relay_descriptor_t *d = calloc(1, sizeof(*d));
    if (!d) return NULL;
    d->fingerprint = *fp;
    d->version = 1;
    d->published_at = trellis_now_ms();
    d->capabilities = capabilities;
    d->bandwidth_kbps = bandwidth_kbps;
    return d;
}

void trellis_relay_descriptor_free(trellis_relay_descriptor_t *d)
{
    if (!d) return;
    free(d->exit_policy);
    free(d->contact);
    free(d->signed_bytes);
    free(d);
}

void trellis_relay_descriptor_set_exit_policy(
    trellis_relay_descriptor_t *d, const char *policy)
{
    if (!d) return;
    free(d->exit_policy);
    d->exit_policy = policy ? strdup(policy) : NULL;
    d->exit_policy_len = d->exit_policy ? (uint16_t)strlen(d->exit_policy) : 0;
}

void trellis_relay_descriptor_set_contact(
    trellis_relay_descriptor_t *d, const char *contact)
{
    if (!d) return;
    free(d->contact);
    d->contact = contact ? strdup(contact) : NULL;
    size_t clen = d->contact ? strlen(d->contact) : 0;
    if (clen > UINT8_MAX) clen = UINT8_MAX;
    d->contact_len = (uint8_t)clen;
}

static size_t descriptor_body_len(const trellis_relay_descriptor_t *d)
{
    return DESC_HEADER_LEN + d->exit_policy_len + 4 + 1 + d->contact_len;
}

static size_t descriptor_serialize_body(const trellis_relay_descriptor_t *d,
                                        uint8_t *out)
{
    size_t off = 0;
    memcpy(out + off, d->fingerprint.bytes, TRELLIS_FINGERPRINT_LEN);
    off += TRELLIS_FINGERPRINT_LEN;

    out[off++] = (uint8_t)(d->version >> 24);
    out[off++] = (uint8_t)(d->version >> 16);
    out[off++] = (uint8_t)(d->version >> 8);
    out[off++] = (uint8_t)(d->version);

    for (int i = 7; i >= 0; i--)
        out[off++] = (uint8_t)(d->published_at >> (i * 8));

    out[off++] = (uint8_t)(d->capabilities >> 24);
    out[off++] = (uint8_t)(d->capabilities >> 16);
    out[off++] = (uint8_t)(d->capabilities >> 8);
    out[off++] = (uint8_t)(d->capabilities);

    out[off++] = (uint8_t)(d->exit_policy_len >> 8);
    out[off++] = (uint8_t)(d->exit_policy_len);
    if (d->exit_policy_len > 0 && d->exit_policy)
        memcpy(out + off, d->exit_policy, d->exit_policy_len);
    off += d->exit_policy_len;

    out[off++] = (uint8_t)(d->bandwidth_kbps >> 24);
    out[off++] = (uint8_t)(d->bandwidth_kbps >> 16);
    out[off++] = (uint8_t)(d->bandwidth_kbps >> 8);
    out[off++] = (uint8_t)(d->bandwidth_kbps);

    out[off++] = d->contact_len;
    if (d->contact_len > 0 && d->contact)
        memcpy(out + off, d->contact, d->contact_len);
    off += d->contact_len;

    return off;
}

trellis_err_t trellis_relay_descriptor_sign(
    trellis_relay_descriptor_t *d, const trellis_identity_t *id)
{
    if (!d || !id)
        return TRELLIS_ERR_INVALID_ARG;

    size_t body_len = descriptor_body_len(d);
    size_t total = body_len + TRELLIS_SIGNATURE_LEN;
    uint8_t *buf = malloc(total);
    if (!buf) return TRELLIS_ERR_NOMEM;

    size_t actual_body = descriptor_serialize_body(d, buf);

    trellis_signature_t sig;
    trellis_err_t err = trellis_identity_sign(id, buf, actual_body, &sig);
    if (err != TRELLIS_OK) {
        free(buf);
        return err;
    }

    memcpy(buf + actual_body, sig.ed25519_sig, TRELLIS_ED25519_SIG_LEN);
    memcpy(buf + actual_body + TRELLIS_ED25519_SIG_LEN,
           sig.ml_dsa_sig, TRELLIS_ML_DSA_87_SIG_LEN);

    free(d->signed_bytes);
    d->signed_bytes = buf;
    d->signed_len = total;

    return TRELLIS_OK;
}

trellis_err_t trellis_relay_descriptor_verify(
    const uint8_t *data, size_t len,
    const trellis_identity_public_t *pub,
    trellis_relay_descriptor_t **out)
{
    if (!data || !pub || !out)
        return TRELLIS_ERR_INVALID_ARG;
    if (len < DESC_HEADER_LEN + TRELLIS_SIGNATURE_LEN)
        return TRELLIS_ERR_MSG_FORMAT;

    // Parse enough to know the body length.
    size_t off = TRELLIS_FINGERPRINT_LEN + 4 + 8 + 4; /* fp + ver + ts + caps */
    if (off + 2 > len) return TRELLIS_ERR_MSG_FORMAT;
    uint16_t policy_len = ((uint16_t)data[off] << 8) | data[off + 1];
    off += 2 + policy_len;
    if (off + 4 > len) return TRELLIS_ERR_MSG_FORMAT;
    off += 4; /* bandwidth */
    if (off + 1 > len) return TRELLIS_ERR_MSG_FORMAT;
    uint8_t contact_len = data[off];
    off += 1 + contact_len;

    size_t body_len = off;
    if (body_len + TRELLIS_SIGNATURE_LEN > len)
        return TRELLIS_ERR_MSG_FORMAT;

    // Verify hybrid signature.
    trellis_signature_t sig;
    memcpy(sig.ed25519_sig, data + body_len, TRELLIS_ED25519_SIG_LEN);
    memcpy(sig.ml_dsa_sig, data + body_len + TRELLIS_ED25519_SIG_LEN,
           TRELLIS_ML_DSA_87_SIG_LEN);

    trellis_err_t err = trellis_identity_verify(pub, data, body_len, &sig);
    if (err != TRELLIS_OK)
        return TRELLIS_ERR_VERIFY_FAILED;

    // Parse descriptor body.
    trellis_relay_descriptor_t *d = calloc(1, sizeof(*d));
    if (!d) return TRELLIS_ERR_NOMEM;

    off = 0;
    memcpy(d->fingerprint.bytes, data + off, TRELLIS_FINGERPRINT_LEN);
    off += TRELLIS_FINGERPRINT_LEN;

    d->version = ((uint32_t)data[off] << 24) | ((uint32_t)data[off+1] << 16) |
                 ((uint32_t)data[off+2] << 8) | data[off+3];
    off += 4;

    d->published_at = 0;
    for (int i = 0; i < 8; i++)
        d->published_at = (d->published_at << 8) | data[off++];

    d->capabilities = ((uint32_t)data[off] << 24) | ((uint32_t)data[off+1] << 16) |
                      ((uint32_t)data[off+2] << 8) | data[off+3];
    off += 4;

    d->exit_policy_len = ((uint16_t)data[off] << 8) | data[off + 1];
    off += 2;
    if (d->exit_policy_len > 0) {
        d->exit_policy = malloc(d->exit_policy_len + 1);
        if (d->exit_policy) {
            memcpy(d->exit_policy, data + off, d->exit_policy_len);
            d->exit_policy[d->exit_policy_len] = '\0';
        }
    }
    off += d->exit_policy_len;

    d->bandwidth_kbps = ((uint32_t)data[off] << 24) | ((uint32_t)data[off+1] << 16) |
                        ((uint32_t)data[off+2] << 8) | data[off+3];
    off += 4;

    d->contact_len = data[off++];
    if (d->contact_len > 0) {
        d->contact = malloc(d->contact_len + 1);
        if (d->contact) {
            memcpy(d->contact, data + off, d->contact_len);
            d->contact[d->contact_len] = '\0';
        }
    }

    // Store raw signed bytes for republishing.
    d->signed_bytes = malloc(len);
    if (d->signed_bytes) {
        memcpy(d->signed_bytes, data, len);
        d->signed_len = len;
    }

    *out = d;
    return TRELLIS_OK;
}

trellis_err_t trellis_relay_descriptor_publish(trellis_dht_t *dht,
                                               trellis_relay_descriptor_t *d)
{
    if (!dht || !d || !d->signed_bytes)
        return TRELLIS_ERR_INVALID_ARG;

    uint8_t key[4 + TRELLIS_FINGERPRINT_LEN];
    memcpy(key, "desc", 4);
    memcpy(key + 4, d->fingerprint.bytes, TRELLIS_FINGERPRINT_LEN);

    return trellis_dht_store(dht, key, sizeof(key),
                             d->signed_bytes, d->signed_len,
                             NULL, NULL);
}

#define DESC_CACHE_MAX      256
#define DESC_CACHE_TTL_MS   (30 * 60 * 1000)  /* 30 minutes */

typedef struct desc_cache_entry {
    trellis_relay_descriptor_t *desc;
    uint64_t                    fetched_at;
    bool                        occupied;
} desc_cache_entry_t;

static desc_cache_entry_t g_desc_cache[DESC_CACHE_MAX];

static trellis_relay_descriptor_t *desc_cache_lookup(
    const trellis_fingerprint_t *fp)
{
    uint64_t now = trellis_now_ms();
    for (size_t i = 0; i < DESC_CACHE_MAX; i++) {
        if (!g_desc_cache[i].occupied) continue;
        if (now - g_desc_cache[i].fetched_at > DESC_CACHE_TTL_MS) {
            trellis_relay_descriptor_free(g_desc_cache[i].desc);
            g_desc_cache[i].occupied = false;
            continue;
        }
        if (trellis_fingerprint_eq(&g_desc_cache[i].desc->fingerprint, fp))
            return g_desc_cache[i].desc;
    }
    return NULL;
}

static void desc_cache_insert(trellis_relay_descriptor_t *d)
{
    uint64_t now = trellis_now_ms();
    int slot = -1;
    for (size_t i = 0; i < DESC_CACHE_MAX; i++) {
        if (!g_desc_cache[i].occupied ||
            now - g_desc_cache[i].fetched_at > DESC_CACHE_TTL_MS) {
            if (g_desc_cache[i].occupied)
                trellis_relay_descriptor_free(g_desc_cache[i].desc);
            slot = (int)i;
            break;
        }
    }
    if (slot < 0) {
        // Evict oldest.
        uint64_t oldest = UINT64_MAX;
        for (size_t i = 0; i < DESC_CACHE_MAX; i++) {
            if (g_desc_cache[i].fetched_at < oldest) {
                oldest = g_desc_cache[i].fetched_at;
                slot = (int)i;
            }
        }
        if (slot >= 0 && g_desc_cache[slot].occupied)
            trellis_relay_descriptor_free(g_desc_cache[slot].desc);
    }
    if (slot >= 0) {
        g_desc_cache[slot].desc = d;
        g_desc_cache[slot].fetched_at = now;
        g_desc_cache[slot].occupied = true;
    }
}

typedef struct {
    trellis_fingerprint_t        fp;
    trellis_identity_public_t    pub;
    trellis_relay_descriptor_cb  cb;
    void                        *ctx;
} desc_fetch_ctx_t;

static void on_desc_dht_result(const uint8_t *value, size_t value_len,
                                trellis_err_t err, void *raw_ctx)
{
    desc_fetch_ctx_t *fc = (desc_fetch_ctx_t *)raw_ctx;
    if (err != TRELLIS_OK || !value || value_len == 0) {
        if (fc->cb) fc->cb(NULL, TRELLIS_ERR_NOT_FOUND, fc->ctx);
        free(fc);
        return;
    }

    trellis_relay_descriptor_t *d = NULL;
    trellis_err_t ve = trellis_relay_descriptor_verify(
        value, value_len, &fc->pub, &d);
    if (ve != TRELLIS_OK || !d) {
        if (fc->cb) fc->cb(NULL, ve, fc->ctx);
        free(fc);
        return;
    }

    // Verify the descriptor matches the expected fingerprint.
    if (!trellis_fingerprint_eq(&d->fingerprint, &fc->fp)) {
        trellis_relay_descriptor_free(d);
        if (fc->cb) fc->cb(NULL, TRELLIS_ERR_VERIFY_FAILED, fc->ctx);
        free(fc);
        return;
    }

    // Check freshness with clock skew tolerance.
    uint64_t now = trellis_now_ms();
    uint64_t max_age = 24 * 60 * 60 * 1000ULL; /* 24h */
    if (now > d->published_at + max_age + 30000) {
        trellis_relay_descriptor_free(d);
        if (fc->cb) fc->cb(NULL, TRELLIS_ERR_EXPIRED, fc->ctx);
        free(fc);
        return;
    }

    desc_cache_insert(d);
    if (fc->cb) fc->cb(d, TRELLIS_OK, fc->ctx);
    free(fc);
}

trellis_err_t trellis_relay_descriptor_fetch(
    trellis_dht_t *dht,
    const trellis_fingerprint_t *fp,
    const trellis_identity_public_t *pub,
    trellis_relay_descriptor_cb cb,
    void *ctx)
{
    if (!dht || !fp) return TRELLIS_ERR_INVALID_ARG;

    // Check cache.
    trellis_relay_descriptor_t *cached = desc_cache_lookup(fp);
    if (cached) {
        if (cb) cb(cached, TRELLIS_OK, ctx);
        return TRELLIS_OK;
    }

    if (!pub) return TRELLIS_ERR_INVALID_ARG;

    desc_fetch_ctx_t *fc = calloc(1, sizeof(*fc));
    if (!fc) return TRELLIS_ERR_NOMEM;
    fc->fp = *fp;
    fc->pub = *pub;
    fc->cb = cb;
    fc->ctx = ctx;

    uint8_t key[4 + TRELLIS_FINGERPRINT_LEN];
    memcpy(key, "desc", 4);
    memcpy(key + 4, fp->bytes, TRELLIS_FINGERPRINT_LEN);

    return trellis_pir_find_value(dht, key, sizeof(key),
                                   on_desc_dht_result, fc);
}

trellis_relay_descriptor_t *trellis_relay_descriptor_cached(
    const trellis_fingerprint_t *fp)
{
    return fp ? desc_cache_lookup(fp) : NULL;
}
