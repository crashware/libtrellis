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
#include <math.h>

trellis_morph_config_t trellis_morph_config_default(void)
{
    trellis_morph_config_t cfg = {
        .metamorphic               = false,
        .min_jitter_ms             = 5,
        .max_jitter_ms             = 50,
        .cover_traffic_interval_ms = 0,
        .burst_min_chunk           = 64,
        .burst_max_chunk           = 512,
        .constant_rate             = false,
        .constant_rate_interval_ms = 0,
        .constant_rate_frame_size  = 0,
        .cell_mode                 = false,
        .cell_size                 = TRELLIS_MORPH_CELL_SIZE_DEFAULT,
        .require_constant_rate     = false,
        .dp_epsilon                = 0.0,
    };
    return cfg;
}

trellis_morph_t *trellis_morph_new(const trellis_morph_config_t *cfg)
{
    if (!cfg)
        return NULL;

    // Reject pathological dp_epsilon values that would silently disable DP.
    if (cfg->dp_epsilon != 0.0 &&
        (cfg->dp_epsilon < 0.0 || isnan(cfg->dp_epsilon) ||
         isinf(cfg->dp_epsilon)))
        return NULL;

    // Validate cell_mode + constant_rate size compatibility.
    if (cfg->cell_mode && cfg->constant_rate &&
        cfg->constant_rate_frame_size != 0 &&
        cfg->constant_rate_frame_size != cfg->cell_size)
        return NULL;

    trellis_morph_t *m = calloc(1, sizeof(trellis_morph_t));
    if (!m)
        return NULL;

    m->config = *cfg;
    m->frame_count = 0;
    m->session_key_set = false;

    // Align constant_rate_frame_size to cell_size when both modes active.
    if (m->config.cell_mode && m->config.constant_rate &&
        m->config.constant_rate_frame_size == 0)
        m->config.constant_rate_frame_size = m->config.cell_size;

    return m;
}

void trellis_morph_free(trellis_morph_t *morph)
{
    if (!morph)
        return;
    sodium_memzero(morph->session_key, sizeof(morph->session_key));
    sodium_memzero(&morph->ctx, sizeof(morph->ctx));
    free(morph);
}

void trellis_morph_set_constant_rate(trellis_morph_t *morph, bool enabled)
{
    if (morph)
        morph->config.constant_rate = enabled;
}

trellis_err_t trellis_morph_set_session_key(trellis_morph_t *morph,
                                            const uint8_t *key, size_t key_len)
{
    if (!morph || !key || key_len == 0)
        return TRELLIS_ERR_INVALID_ARG;

    size_t copy_len = key_len < sizeof(morph->session_key)
                      ? key_len : sizeof(morph->session_key);
    sodium_memzero(morph->session_key, sizeof(morph->session_key));
    memcpy(morph->session_key, key, copy_len);
    morph->session_key_len = copy_len;
    morph->session_key_set = true;

    /* magic_bytes are re-derived each rotation epoch; store the base key
     * rather than a single precomputed value. The base is SHA-256 of the
     * session key, used as input to the per-epoch derivation. */
    crypto_hash_sha256_state st;
    uint8_t digest[32];
    crypto_hash_sha256_init(&st);
    crypto_hash_sha256_update(&st, (const uint8_t *)"bloom-morph-magic", 17);
    crypto_hash_sha256_update(&st, key, key_len);
    crypto_hash_sha256_final(&st, digest);
    memcpy(morph->ctx.magic_bytes, digest, 4);

    morph->ctx.padding_target = 128 + (key[4 % key_len] % 128);
    morph->ctx.encoding = (trellis_encoding_t)(key[5 % key_len] % 4);
    morph->ctx.format_index = 0;

    morph->frame_count = 0;

    return TRELLIS_OK;
}

/*
 * Derive the 4-byte magic prefix for a given rotation epoch.
 * epoch = frame_count / MORPH_ROTATION_INTERVAL
 * This prevents the prefix from being a stable per-session DPI fingerprint.
 */
static void morph_derive_magic(const trellis_morph_t *morph,
                                uint32_t epoch, uint8_t magic[4])
{
    crypto_hash_sha256_state st;
    uint8_t digest[32];
    uint8_t epoch_le[4] = {
        (uint8_t)(epoch),
        (uint8_t)(epoch >> 8),
        (uint8_t)(epoch >> 16),
        (uint8_t)(epoch >> 24),
    };
    crypto_hash_sha256_init(&st);
    crypto_hash_sha256_update(&st, morph->ctx.magic_bytes, 4);
    crypto_hash_sha256_update(&st, epoch_le, 4);
    crypto_hash_sha256_final(&st, digest);
    memcpy(magic, digest, 4);
}

trellis_err_t trellis_morph_encode(trellis_morph_t *morph,
                                   const uint8_t *data, size_t len,
                                   trellis_buf_t *out)
{
    if (!morph || !data || !out)
        return TRELLIS_ERR_INVALID_ARG;

    if (morph->config.require_constant_rate && !morph->config.constant_rate)
        return TRELLIS_ERR_INVALID_STATE;

    if (morph->config.metamorphic && morph->session_key_set) {
        // Step 1: pad.
        trellis_buf_t padded = {0};
        trellis_err_t err = morph_pad(morph, data, len, &padded);
        if (err != TRELLIS_OK)
            return err;

        // Step 2: mutate (wrap in the active encoding format)
        trellis_buf_t mutated = {0};
        err = morph_mutate_encode(morph, padded.data, padded.len, &mutated);
        trellis_buf_free(&padded);
        if (err != TRELLIS_OK)
            return err;

        // Step 3: prepend epoch-derived magic prefix.
        uint32_t epoch = morph->frame_count / MORPH_ROTATION_INTERVAL;
        uint8_t magic[4];
        morph_derive_magic(morph, epoch, magic);

        *out = trellis_buf_alloc(4 + mutated.len);
        if (!out->data) {
            trellis_buf_free(&mutated);
            return TRELLIS_ERR_NOMEM;
        }

        memcpy(out->data, magic, 4);
        memcpy(out->data + 4, mutated.data, mutated.len);
        out->len = 4 + mutated.len;
        trellis_buf_free(&mutated);

        if (morph->frame_count < UINT32_MAX)
            morph->frame_count++;

        // Rotate encoding and format index at each interval boundary.
        if (morph->frame_count % MORPH_ROTATION_INTERVAL == 0) {
            morph->ctx.encoding = (trellis_encoding_t)
                ((morph->ctx.encoding + 1) % 4);
            morph->ctx.format_index =
                (morph->ctx.format_index + 1) % 4;
        }

        return TRELLIS_OK;
    }

    // Non-metamorphic: pass through unchanged.
    *out = trellis_buf_from(data, len);
    if (!out->data && len > 0)
        return TRELLIS_ERR_NOMEM;

    if (morph->frame_count < UINT32_MAX)
        morph->frame_count++;
    return TRELLIS_OK;
}

trellis_err_t trellis_morph_decode(trellis_morph_t *morph,
                                   const uint8_t *data, size_t len,
                                   trellis_buf_t *out)
{
    if (!morph || !data || !out)
        return TRELLIS_ERR_INVALID_ARG;

    if (morph->config.metamorphic && morph->session_key_set) {
        if (len < 4)
            return TRELLIS_ERR_DECODE;

        /* Verify epoch-derived magic prefix.  Allow ±1 epoch tolerance so
         * that brief frame-count divergence (e.g. on transport reconnect or
         * reordering) does not permanently break the morph stream. */
        uint32_t base_epoch = morph->recv_frame_count / MORPH_ROTATION_INTERVAL;
        bool magic_ok = false;
        int32_t epoch_offset = 0; /* the matched offset relative to base */
        for (int32_t delta = 0; delta <= 1 && !magic_ok; delta++) {
            for (int32_t sign = 0; sign <= 1 && !magic_ok; sign++) {
                int32_t d = (sign == 0) ? delta : -delta;
                if (d == 0 && sign == 1) continue; /* avoid duplicate 0 */
                uint32_t candidate = (uint32_t)((int32_t)base_epoch + d);
                uint8_t m[4];
                morph_derive_magic(morph, candidate, m);
                if (sodium_memcmp(data, m, 4) == 0) {
                    magic_ok = true;
                    epoch_offset = d;
                }
            }
        }
        if (!magic_ok)
            return TRELLIS_ERR_DECODE;

        // If a skew was detected, resync recv_frame_count to the matched epoch.
        if (epoch_offset != 0) {
            int64_t adjusted = (int64_t)morph->recv_frame_count
                               + (int64_t)epoch_offset * MORPH_ROTATION_INTERVAL;
            morph->recv_frame_count = adjusted >= 0 ? (uint32_t)adjusted : 0;
        }

        // Demutate
        trellis_buf_t demutated = {0};
        trellis_err_t err = morph_mutate_decode(morph, data + 4, len - 4, &demutated);
        if (err != TRELLIS_OK)
            return err;

        // Unpad
        trellis_buf_t unpacked = {0};
        err = morph_unpad(demutated.data, demutated.len, &unpacked);
        trellis_buf_free(&demutated);
        if (err != TRELLIS_OK)
            return err;

        if (morph->recv_frame_count < UINT32_MAX)
            morph->recv_frame_count++;
        *out = unpacked;
        return TRELLIS_OK;
    }

    // Non-metamorphic: pass through unchanged.
    *out = trellis_buf_from(data, len);
    if (!out->data && len > 0)
        return TRELLIS_ERR_NOMEM;

    return TRELLIS_OK;
}

trellis_morph_inspect_t trellis_morph_inspect(const trellis_morph_t *morph)
{
    trellis_morph_inspect_t info = {0};
    if (!morph)
        return info;

    info.encoding = morph->ctx.encoding;
    info.format_index = morph->ctx.format_index;
    info.frame_count = morph->frame_count;
    memcpy(info.magic_bytes, morph->ctx.magic_bytes, 4);
    return info;
}

// Constant-rate padding.
trellis_err_t trellis_morph_constant_rate_pad(trellis_morph_t *morph,
                                              const uint8_t *data, size_t len,
                                              trellis_buf_t *out)
{
    if (!morph || !out)
        return TRELLIS_ERR_INVALID_ARG;

    uint32_t target = morph->config.constant_rate_frame_size;
    if (target == 0) target = 512;

    if (!data || len == 0) {
        *out = trellis_buf_alloc(target);
        if (!out->data)
            return TRELLIS_ERR_NOMEM;
        // Pure cover frame: 4-byte zero length + random fill.
        memset(out->data, 0, 4);
        randombytes_buf(out->data + 4, target - 4);
        out->len = target;
        return TRELLIS_OK;
    }

    size_t needed = 4 + len;
    if (needed > target)
        return TRELLIS_ERR_OVERFLOW;

    *out = trellis_buf_alloc(target);
    if (!out->data)
        return TRELLIS_ERR_NOMEM;

    uint32_t len32 = (uint32_t)len;
    out->data[0] = (uint8_t)(len32 >> 24);
    out->data[1] = (uint8_t)(len32 >> 16);
    out->data[2] = (uint8_t)(len32 >> 8);
    out->data[3] = (uint8_t)(len32);
    memcpy(out->data + 4, data, len);
    if (target > needed)
        randombytes_buf(out->data + needed, target - needed);
    out->len = target;
    return TRELLIS_OK;
}

bool trellis_morph_is_constant_rate(const trellis_morph_t *morph)
{
    return morph && morph->config.constant_rate;
}

uint32_t trellis_morph_constant_rate_interval(const trellis_morph_t *morph)
{
    if (!morph) return 0;
    return morph->config.constant_rate_interval_ms;
}

void trellis_morph_set_require_constant_rate(trellis_morph_t *morph, bool require)
{
    if (!morph) return;
    morph->config.require_constant_rate = require;
}

#ifndef __EMSCRIPTEN__
// Adaptive mimicry probe.

// Default probe endpoints per profile (well-known CDN/DNS IPs)
static const probe_endpoint_t default_endpoints[TRELLIS_TIMING_PROFILE_COUNT][2] = {
    /* DEFAULT */  { {"1.1.1.1", 80}, {"8.8.8.8", 80} },
    /* HTTPS   */  { {"1.1.1.1", 443}, {"8.8.8.8", 443} },
    /* HTTP2   */  { {"1.1.1.1", 443}, {"104.16.0.0", 443} },
    /* DNS     */  { {"8.8.8.8", 53},  {"1.1.1.1", 53} },
    /* QUIC    */  { {"8.8.8.8", 443}, {"1.1.1.1", 443} },
};

// Per-attempt context passed to the libuv TCP connect callback.
typedef struct probe_attempt {
    uv_tcp_t                 handle;
    uv_connect_t             connect_req;
    uv_timer_t               timeout;
    trellis_mimicry_probe_t *probe;
    trellis_timing_profile_t profile;
    bool                     done;
} probe_attempt_t;

static void probe_attempt_close_handle(uv_handle_t *h)
{
    (void)h;
}

static void probe_attempt_close_and_free(uv_handle_t *h)
{
    probe_attempt_t *a = (probe_attempt_t *)h->data;
    if (a) free(a);
}

static void probe_attempt_free_all(probe_attempt_t *a)
{
    if (!a) return;
    // handles were already closed via uv_close before we get here.
    free(a);
}

static void probe_attempt_finish(probe_attempt_t *a, bool ok)
{
    if (a->done) return;
    a->done = true;

    trellis_mimicry_probe_t *p = a->probe;
    if (ok)
        p->probe_ok[a->profile] = true;

    // Stop the timeout timer.
    uv_timer_stop(&a->timeout);

    // Close both handles; free on last close.
    uv_close((uv_handle_t *)&a->timeout, probe_attempt_close_handle);
    uv_close((uv_handle_t *)&a->handle, (uv_close_cb)probe_attempt_free_all);

    // Decrement pending count; when all done apply selection.
    if (p->probe_pending > 0)
        p->probe_pending--;

    if (p->probe_pending == 0 && p->probe_round_active) {
        p->probe_round_active = false;

        if (!p->running) return;

        // Priority order: HTTPS > HTTP2 > QUIC > DNS > DEFAULT.
        static const trellis_timing_profile_t priority[] = {
            TRELLIS_TIMING_HTTPS,
            TRELLIS_TIMING_HTTP2,
            TRELLIS_TIMING_QUIC,
            TRELLIS_TIMING_DNS,
            TRELLIS_TIMING_DEFAULT,
        };

        uint64_t now_ms = (uint64_t)(uv_now(p->loop));
        trellis_timing_profile_t chosen = TRELLIS_TIMING_DEFAULT;
        for (size_t i = 0; i < sizeof(priority)/sizeof(priority[0]); i++) {
            trellis_timing_profile_t prof = priority[i];
            if (!p->probe_ok[prof]) continue;
            if (now_ms < p->blocked_until_ms[prof]) continue;
            chosen = prof;
            break;
        }

        if (chosen != p->selected) {
            p->selected = chosen;
            trellis_morph_set_timing_profile(p->morph, chosen);
            fprintf(stderr, "[MIMICRY] selected profile %d\n", (int)chosen);
            if (p->select_cb)
                p->select_cb(chosen, p->select_ctx);
        }
    }
}

static void probe_timeout_cb(uv_timer_t *t)
{
    probe_attempt_t *a = (probe_attempt_t *)t->data;
    probe_attempt_finish(a, false);
}

static void probe_connect_cb(uv_connect_t *req, int status)
{
    probe_attempt_t *a = (probe_attempt_t *)req->data;
    probe_attempt_finish(a, status == 0);
}

static void mimicry_run_probe_round(trellis_mimicry_probe_t *p)
{
    if (p->probe_round_active) return; /* already running */

    p->probe_round_active = true;
    memset(p->probe_ok, 0, sizeof(p->probe_ok));
    p->probe_pending = 0;

    uint64_t now_ms = (uint64_t)(uv_now(p->loop));

    for (int pr = 0; pr < TRELLIS_TIMING_PROFILE_COUNT; pr++) {
        // Pick the first configured endpoint for this profile.
        size_t ep_cnt = p->ep_count[pr];
        if (ep_cnt == 0) {
            // DEFAULT and HTTPS always have built-in probes.
            if (pr < 2) {
                // reachable assumed: DEFAULT always reachable.
                if (pr == TRELLIS_TIMING_DEFAULT)
                    p->probe_ok[pr] = true;
            }
            continue;
        }

        if (now_ms < p->blocked_until_ms[pr])
            continue;

        /* Probe ALL configured endpoints for this profile.  A profile is
         * considered reachable if ANY endpoint succeeds.  This prevents a
         * single unreachable IP from disabling the entire profile. */
        for (size_t ei = 0; ei < ep_cnt; ei++) {
        const probe_endpoint_t *ep = &p->endpoints[pr][ei];

        probe_attempt_t *a = calloc(1, sizeof(*a));
        if (!a) continue;

        a->probe   = p;
        a->profile = (trellis_timing_profile_t)pr;
        a->done    = false;

        uv_tcp_init(p->loop, &a->handle);
        uv_timer_init(p->loop, &a->timeout);
        a->timeout.data = a;
        a->connect_req.data = a;

        // Build sockaddr.
        struct sockaddr_storage ss;
        memset(&ss, 0, sizeof(ss));
        int rc;
        if (strchr(ep->ip, ':')) {
            struct sockaddr_in6 *sa6 = (struct sockaddr_in6 *)&ss;
            rc = uv_ip6_addr(ep->ip, ep->port, sa6);
        } else {
            struct sockaddr_in *sa4 = (struct sockaddr_in *)&ss;
            rc = uv_ip4_addr(ep->ip, ep->port, sa4);
        }

        if (rc != 0) {
            a->handle.data = NULL;
            a->timeout.data = a;
            uv_close((uv_handle_t *)&a->handle, probe_attempt_close_handle);
            uv_close((uv_handle_t *)&a->timeout, probe_attempt_close_and_free);
            continue;
        }

        uv_timer_start(&a->timeout, probe_timeout_cb,
                       TRELLIS_MIMICRY_PROBE_TIMEOUT_MS, 0);

        rc = uv_tcp_connect(&a->connect_req, &a->handle,
                            (const struct sockaddr *)&ss, probe_connect_cb);
        if (rc != 0) {
            uv_timer_stop(&a->timeout);
            a->timeout.data = NULL;
            a->handle.data = a;
            uv_close((uv_handle_t *)&a->timeout, probe_attempt_close_handle);
            uv_close((uv_handle_t *)&a->handle, probe_attempt_close_and_free);
            continue;
        }

        p->probe_pending++;
        } /* for each endpoint */
    }

    // DEFAULT is always reachable (no network dependency)
    p->probe_ok[TRELLIS_TIMING_DEFAULT] = true;

    if (p->probe_pending == 0) {
        // No async probes launched: apply synchronously.
        p->probe_round_active = false;
        trellis_morph_set_timing_profile(p->morph, TRELLIS_TIMING_DEFAULT);
        if (p->selected != TRELLIS_TIMING_DEFAULT) {
            p->selected = TRELLIS_TIMING_DEFAULT;
            if (p->select_cb)
                p->select_cb(TRELLIS_TIMING_DEFAULT, p->select_ctx);
        }
    }
}

static void mimicry_reprobe_timer_cb(uv_timer_t *t)
{
    trellis_mimicry_probe_t *p = (trellis_mimicry_probe_t *)t->data;
    fprintf(stderr, "[MIMICRY] scheduled re-probe\n");
    mimicry_run_probe_round(p);
}

trellis_mimicry_probe_t *trellis_mimicry_probe_new(uv_loop_t *loop,
                                                    trellis_morph_t *morph)
{
    if (!loop || !morph) return NULL;

    trellis_mimicry_probe_t *p = calloc(1, sizeof(*p));
    if (!p) return NULL;

    p->loop     = loop;
    p->morph    = morph;
    p->selected = TRELLIS_TIMING_DEFAULT;

    // Populate default probe endpoints.
    for (int pr = 0; pr < TRELLIS_TIMING_PROFILE_COUNT; pr++) {
        size_t cnt = sizeof(default_endpoints[pr]) / sizeof(default_endpoints[pr][0]);
        for (size_t i = 0; i < cnt && i < TRELLIS_MIMICRY_MAX_ENDPOINTS; i++) {
            if (!default_endpoints[pr][i].ip[0]) break;
            p->endpoints[pr][i] = default_endpoints[pr][i];
            p->ep_count[pr]++;
        }
    }

    uv_timer_init(loop, &p->reprobe_timer);
    p->reprobe_timer.data = p;

    return p;
}

static void mimicry_probe_close_cb(uv_handle_t *handle)
{
    trellis_mimicry_probe_t *p = (trellis_mimicry_probe_t *)handle->data;
    free(p);
}

void trellis_mimicry_probe_free(trellis_mimicry_probe_t *p)
{
    if (!p) return;
    trellis_mimicry_probe_stop(p);
    if (!uv_is_closing((uv_handle_t *)&p->reprobe_timer)) {
        uv_close((uv_handle_t *)&p->reprobe_timer, mimicry_probe_close_cb);
    } else {
        free(p);
    }
}

trellis_err_t trellis_mimicry_probe_start(trellis_mimicry_probe_t *p,
                                           trellis_mimicry_select_cb cb,
                                           void *ctx)
{
    if (!p) return TRELLIS_ERR_INVALID_ARG;

    p->select_cb  = cb;
    p->select_ctx = ctx;
    p->running    = true;

    // Schedule periodic re-probe.
    uv_timer_start(&p->reprobe_timer, mimicry_reprobe_timer_cb,
                   TRELLIS_MIMICRY_REPROBE_INTERVAL_MS,
                   TRELLIS_MIMICRY_REPROBE_INTERVAL_MS);
    p->timer_active = true;

    // Run initial probe immediately.
    mimicry_run_probe_round(p);

    return TRELLIS_OK;
}

void trellis_mimicry_probe_stop(trellis_mimicry_probe_t *p)
{
    if (!p || !p->running) return;
    p->running = false;
    if (p->timer_active) {
        uv_timer_stop(&p->reprobe_timer);
        p->timer_active = false;
    }
}

void trellis_mimicry_probe_on_failure(trellis_mimicry_probe_t *p)
{
    if (!p) return;

    uint64_t now_ms = (uint64_t)(uv_now(p->loop));

    // Block the current profile for 5 minutes and re-probe immediately.
    if (p->selected < TRELLIS_TIMING_PROFILE_COUNT)
        p->blocked_until_ms[p->selected] = now_ms + 5 * 60 * 1000ULL;

    fprintf(stderr, "[MIMICRY] profile %d failed; re-probing\n", (int)p->selected);
    mimicry_run_probe_round(p);
}

trellis_timing_profile_t
trellis_mimicry_probe_selected(const trellis_mimicry_probe_t *p)
{
    if (!p) return TRELLIS_TIMING_DEFAULT;
    return p->selected;
}

trellis_err_t trellis_mimicry_probe_add_endpoint(trellis_mimicry_probe_t *p,
                                                  trellis_timing_profile_t profile,
                                                  const char *ip,
                                                  uint16_t port)
{
    if (!p || !ip || profile >= TRELLIS_TIMING_PROFILE_COUNT)
        return TRELLIS_ERR_INVALID_ARG;

    if (p->ep_count[profile] >= TRELLIS_MIMICRY_MAX_ENDPOINTS)
        return TRELLIS_ERR_FULL;

    probe_endpoint_t *ep = &p->endpoints[profile][p->ep_count[profile]];
    size_t ip_len = strlen(ip);
    if (ip_len >= sizeof(ep->ip))
        ip_len = sizeof(ep->ip) - 1;
    memcpy(ep->ip, ip, ip_len);
    ep->ip[ip_len] = '\0';
    ep->port = port;
    p->ep_count[profile]++;

    return TRELLIS_OK;
}

#endif /* !__EMSCRIPTEN__ */

trellis_err_t trellis_morph_cell_fragment(const trellis_morph_t *morph,
                                           const uint8_t *data, size_t len,
                                           trellis_buf_t *cells, size_t *cell_count,
                                           size_t max_cells)
{
    return morph_cell_fragment(morph, data, len, cells, cell_count, max_cells);
}

trellis_err_t trellis_morph_cell_reassemble(const trellis_buf_t *cells,
                                             size_t cell_count,
                                             trellis_buf_t *out)
{
    return morph_cell_reassemble(cells, cell_count, out);
}
