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
#include <trellis/transport.h>
#include <trellis/protocol.h>
#include <trellis/crypto.h>
#include <uv.h>
#include <stdio.h>
#include <math.h>

typedef struct {
    uint32_t min_ms;
    uint32_t max_ms;
} jitter_range_t;

static const jitter_range_t transport_jitter[] = {
    [TRELLIS_TRANSPORT_RAW] = {  0,  10 },
    [TRELLIS_TRANSPORT_TLS] = { 20, 200 },
    [TRELLIS_TRANSPORT_PT]  = { 10, 150 },
};

#define TRANSPORT_JITTER_COUNT (sizeof(transport_jitter) / sizeof(transport_jitter[0]))

static uint32_t transport_protocol_jitter(trellis_transport_mode_t mode)
{
    if ((size_t)mode >= TRANSPORT_JITTER_COUNT)
        return 0;

    const jitter_range_t *r = &transport_jitter[mode];
    if (r->max_ms <= r->min_ms)
        return r->min_ms;

    return r->min_ms + randombytes_uniform(r->max_ms - r->min_ms + 1);
}

uint32_t trellis_morph_jitter_delay(const trellis_morph_t *morph)
{
    if (!morph)
        return 0;

    /*
     * Base jitter is now a fixed moderate range since the morph engine
     * no longer tracks transport mode. The transport-specific jitter
     * can be applied externally by callers who know the transport.
     */
    uint32_t lo = morph->config.min_jitter_ms;
    uint32_t hi = morph->config.max_jitter_ms;
    if (hi <= lo)
        return lo;

    return lo + randombytes_uniform(hi - lo + 1);
}

#define COVER_MIN_LEN  64
#define COVER_MAX_LEN  256

trellis_err_t trellis_morph_generate_cover(const trellis_morph_t *morph,
                                           trellis_buf_t *out)
{
    if (!morph || !out)
        return TRELLIS_ERR_INVALID_ARG;

    uint32_t cover_len = COVER_MIN_LEN +
                         randombytes_uniform(COVER_MAX_LEN - COVER_MIN_LEN + 1);

    *out = trellis_buf_alloc(cover_len);
    if (!out->data)
        return TRELLIS_ERR_NOMEM;

    randombytes_buf(out->data, cover_len);
    out->len = cover_len;

    return TRELLIS_OK;
}

// Protocol-aware timing.
typedef struct {
    uint32_t min_ms;
    uint32_t max_ms;
    uint32_t burst_probability;
    uint32_t burst_min;
    uint32_t burst_max;
} timing_profile_t;

static const timing_profile_t timing_profiles[] = {
    [TRELLIS_TIMING_DEFAULT] = {  5,  50, 0, 0, 0 },
    [TRELLIS_TIMING_HTTPS]   = { 20, 200, 10, 2, 6 },
    [TRELLIS_TIMING_HTTP2]   = {  5, 100, 30, 3, 10 },
    [TRELLIS_TIMING_DNS]     = { 50, 500,  5, 1, 3 },
    [TRELLIS_TIMING_QUIC]    = {  1,  40, 40, 4, 12 },
};

#define TIMING_PROFILE_COUNT (sizeof(timing_profiles) / sizeof(timing_profiles[0]))

void trellis_morph_set_timing_profile(trellis_morph_t *morph,
                                      trellis_timing_profile_t profile)
{
    if (morph) morph->timing_profile = profile;
}

uint32_t trellis_morph_protocol_jitter(const trellis_morph_t *morph)
{
    if (!morph) return 0;

    size_t idx = (size_t)morph->timing_profile;
    if (idx >= TIMING_PROFILE_COUNT)
        idx = 0;

    const timing_profile_t *tp = &timing_profiles[idx];
    if (tp->max_ms <= tp->min_ms)
        return tp->min_ms;
    return tp->min_ms + randombytes_uniform(tp->max_ms - tp->min_ms + 1);
}

bool trellis_morph_should_burst(const trellis_morph_t *morph,
                                uint32_t *burst_count)
{
    if (!morph || !burst_count) return false;

    size_t idx = (size_t)morph->timing_profile;
    if (idx >= TIMING_PROFILE_COUNT) idx = 0;

    const timing_profile_t *tp = &timing_profiles[idx];
    if (tp->burst_probability == 0) return false;

    if (randombytes_uniform(100) < tp->burst_probability) {
        uint32_t range = tp->burst_max - tp->burst_min + 1;
        *burst_count = tp->burst_min + randombytes_uniform(range);
        return true;
    }
    return false;
}

trellis_err_t trellis_morph_generate_cover_ratchet(const trellis_morph_t *morph,
                                                   trellis_ratchet_t *ratchet,
                                                   trellis_buf_t *out)
{
    if (!morph || !ratchet || !out)
        return TRELLIS_ERR_INVALID_ARG;

    /*
     * Build a COVER message and encrypt it through the ratchet so it
     * is indistinguishable from real traffic on the wire.  The receiver
     * decrypts, sees msg_type == TRELLIS_MSG_COVER, and silently discards.
     */
    uint32_t pad_len = COVER_MIN_LEN +
                       randombytes_uniform(COVER_MAX_LEN - COVER_MIN_LEN + 1);

    size_t pt_len = 2 + pad_len; /* 2-byte msg type + random payload */
    uint8_t *pt = malloc(pt_len);
    if (!pt)
        return TRELLIS_ERR_NOMEM;

    pt[0] = (uint8_t)(TRELLIS_MSG_COVER >> 8);
    pt[1] = (uint8_t)(TRELLIS_MSG_COVER);
    randombytes_buf(pt + 2, pad_len);

    trellis_err_t err = trellis_ratchet_encrypt(ratchet, pt, pt_len, out);
    sodium_memzero(pt, pt_len);
    free(pt);
    return err;
}

#ifndef __EMSCRIPTEN__
// Constant-rate scheduler.

/*
 * The scheduler fires a libuv repeating timer at morph->config.constant_rate_interval_ms.
 * On each tick it either:
 *   (a) flushes the oldest queued real-data frame through constant_rate_pad, or
 *   (b) emits a ratchet-encrypted COVER frame to fill the slot.
 *
 * This produces a constant bitrate stream that leaks no timing metadata about
 * when real messages are sent.
 */
#define SCHED_QUEUE_MAX 256

typedef struct sched_frame {
    uint8_t *data;
    size_t   len;
    uint32_t batch_id;   /* non-zero when part of a batch push */
} sched_frame_t;

struct trellis_rate_scheduler {
    trellis_morph_t    *morph;
    trellis_ratchet_t  *ratchet;
    uv_timer_t          timer;
    uv_loop_t          *loop;

    trellis_rate_send_cb send_cb;
    void                *send_ctx;

    sched_frame_t       queue[SCHED_QUEUE_MAX];
    size_t              head;
    size_t              tail;
    size_t              count;

    bool                running;
    uint64_t            ticks_total;
    uint64_t            ticks_cover;
    uint64_t            ticks_real;
    uint32_t            dp_pending_dummies;
    uint32_t            next_batch_id;

    /* Per-interval DP state: count real frames within each DP window and
     * inject noise at the window boundary instead of per-message. */
    uint64_t            dp_interval_start;
    uint32_t            dp_interval_real;
    uint32_t            dp_interval_total;
};

/*
 * Sample from Geometric(1 - exp(-epsilon)).  The cap is computed dynamically
 * so that the truncation probability is at most 1%, preserving the DP
 * guarantee across the usable epsilon range.
 */
#define DP_DUMMY_CAP_MIN  8
#define DP_DUMMY_CAP_MAX  32

static uint32_t dp_compute_cap(double epsilon)
{
    // cap = ceil(-ln(0.01) / epsilon) ≈ ceil(4.605 / epsilon)
    double raw = ceil(4.605170185988091 / epsilon);
    if (raw < DP_DUMMY_CAP_MIN) return DP_DUMMY_CAP_MIN;
    if (raw > DP_DUMMY_CAP_MAX) return DP_DUMMY_CAP_MAX;
    return (uint32_t)raw;
}

static uint32_t dp_sample_dummies(double epsilon)
{
    if (epsilon <= 0.0)
        return 0;

    double p_stop = 1.0 - exp(-epsilon);
    if (p_stop >= 1.0)
        return 0;
    if (p_stop <= 0.0)
        return DP_DUMMY_CAP_MAX;

    uint32_t cap = dp_compute_cap(epsilon);
    uint32_t count = 0;
    while (count < cap) {
        double u = (double)randombytes_uniform(UINT32_MAX) / (double)UINT32_MAX;
        if (u < p_stop)
            break;
        count++;
    }
    return count;
}

static bool sched_emit_cover(trellis_rate_scheduler_t *s)
{
    trellis_buf_t frame = {0};
    trellis_err_t err;

    if (s->ratchet)
        err = trellis_morph_generate_cover_ratchet(s->morph, s->ratchet, &frame);
    else
        err = trellis_morph_constant_rate_pad(s->morph, NULL, 0, &frame);

    if (err != TRELLIS_OK)
        return false;

    if (s->send_cb && frame.data && frame.len > 0)
        s->send_cb(frame.data, frame.len, true, s->send_ctx);

    trellis_buf_free(&frame);
    s->ticks_cover++;
    s->ticks_total++;
    return true;
}

/* Emit a random-filled fallback frame to preserve the constant-rate stream
 * when the normal cover/pad path fails (e.g. OOM, ratchet error). */
static void sched_emit_fallback(trellis_rate_scheduler_t *s)
{
    uint32_t size = s->morph->config.constant_rate_frame_size;
    if (size == 0) size = 512;
    trellis_buf_t fb = trellis_buf_alloc(size);
    if (!fb.data)
        return;
    randombytes_buf(fb.data, size);
    fb.len = size;
    if (s->send_cb)
        s->send_cb(fb.data, fb.len, true, s->send_ctx);
    trellis_buf_free(&fb);
    s->ticks_cover++;
    s->ticks_total++;
}

/* DP interval duration: 10x the tick interval (or 500ms default).
 * Noise is computed once per interval rather than per message, so
 * composition across messages within an interval is free. */
#define DP_INTERVAL_MULTIPLIER  10

static uint64_t dp_interval_ms(const trellis_rate_scheduler_t *s)
{
    uint32_t tick_ms = s->morph->config.constant_rate_interval_ms;
    if (tick_ms == 0) tick_ms = 50;
    return (uint64_t)tick_ms * DP_INTERVAL_MULTIPLIER;
}

static void dp_interval_check(trellis_rate_scheduler_t *s)
{
    double eps = s->morph->config.dp_epsilon;
    if (eps <= 0.0)
        return;

    uint64_t now = uv_now(s->loop);
    uint64_t window = dp_interval_ms(s);

    if (s->dp_interval_start == 0)
        s->dp_interval_start = now;

    if ((now - s->dp_interval_start) < window)
        return;

    /* Interval boundary reached: inject noise based on real count.
     * noisy_count = real_count + Laplace(1/epsilon).
     * We approximate Laplace by the difference of two exponential draws. */
    double scale = 1.0 / eps;
    double u1 = ((double)randombytes_uniform(UINT32_MAX) + 1.0) / ((double)UINT32_MAX + 1.0);
    double u2 = ((double)randombytes_uniform(UINT32_MAX) + 1.0) / ((double)UINT32_MAX + 1.0);
    double laplace = -scale * log(u1) + scale * log(u2);
    int noisy_count = (int)s->dp_interval_real + (int)round(laplace);
    if (noisy_count < 0) noisy_count = 0;

    int extra = noisy_count - (int)s->dp_interval_total;
    if (extra > 0) {
        uint32_t cap = dp_compute_cap(eps);
        s->dp_pending_dummies = (uint32_t)extra < cap ? (uint32_t)extra : cap;
    }

    // Reset interval.
    s->dp_interval_start = now;
    s->dp_interval_real  = 0;
    s->dp_interval_total = 0;
}

static void sched_tick(uv_timer_t *handle)
{
    trellis_rate_scheduler_t *s = (trellis_rate_scheduler_t *)handle->data;
    if (!s || !s->running)
        return;

    // Check DP interval boundary and compute noise budget.
    dp_interval_check(s);

    // Drain any pending DP dummy frames.
    if (s->dp_pending_dummies > 0) {
        if (sched_emit_cover(s))
            s->dp_pending_dummies--;
        else {
            sched_emit_fallback(s);
            s->dp_pending_dummies--;
        }
        s->dp_interval_total++;
        return;
    }

    trellis_buf_t frame = {0};
    bool is_cover = false;

    if (s->count > 0) {
        // Dequeue one real frame.
        sched_frame_t *f = &s->queue[s->head];
        trellis_err_t err = trellis_morph_constant_rate_pad(
            s->morph, f->data, f->len, &frame);

        if (err != TRELLIS_OK) {
            fprintf(stderr, "[RATE] constant_rate_pad failed: %d\n", err);
            free(f->data);
            f->data = NULL;
            f->len = 0;
            s->head = (s->head + 1) % SCHED_QUEUE_MAX;
            s->count--;
            sched_emit_fallback(s);
            s->ticks_total++;
            s->dp_interval_total++;
            return;
        }

        free(f->data);
        f->data = NULL;
        f->len = 0;
        s->head = (s->head + 1) % SCHED_QUEUE_MAX;
        s->count--;
        s->ticks_real++;
        s->ticks_total++;
        s->dp_interval_real++;
        s->dp_interval_total++;
    } else if (s->ratchet) {
        trellis_err_t err = trellis_morph_generate_cover_ratchet(
            s->morph, s->ratchet, &frame);
        if (err != TRELLIS_OK) {
            fprintf(stderr, "[RATE] cover_ratchet failed: %d\n", err);
            sched_emit_fallback(s);
            s->ticks_total++;
            s->dp_interval_total++;
            return;
        }
        is_cover = true;
        s->ticks_cover++;
        s->ticks_total++;
        s->dp_interval_total++;
    } else {
        trellis_err_t err = trellis_morph_constant_rate_pad(
            s->morph, NULL, 0, &frame);
        if (err != TRELLIS_OK) {
            sched_emit_fallback(s);
            s->ticks_total++;
            s->dp_interval_total++;
            return;
        }
        is_cover = true;
        s->ticks_cover++;
        s->ticks_total++;
        s->dp_interval_total++;
    }

    if (s->send_cb && frame.data && frame.len > 0)
        s->send_cb(frame.data, frame.len, is_cover, s->send_ctx);

    trellis_buf_free(&frame);
}

trellis_rate_scheduler_t *trellis_rate_scheduler_new(trellis_morph_t *morph,
                                                      trellis_ratchet_t *ratchet,
                                                      uv_loop_t *loop)
{
    if (!morph || !loop)
        return NULL;

    trellis_rate_scheduler_t *s = calloc(1, sizeof(*s));
    if (!s)
        return NULL;

    s->morph   = morph;
    s->ratchet = ratchet;
    s->loop    = loop;

    uv_timer_init(loop, &s->timer);
    s->timer.data = s;

    return s;
}

void trellis_rate_scheduler_free(trellis_rate_scheduler_t *s)
{
    if (!s)
        return;

    trellis_rate_scheduler_stop(s);

    // Drain the queue.
    while (s->count > 0) {
        free(s->queue[s->head].data);
        s->head = (s->head + 1) % SCHED_QUEUE_MAX;
        s->count--;
    }

    free(s);
}

trellis_err_t trellis_rate_scheduler_start(trellis_rate_scheduler_t *s,
                                            trellis_rate_send_cb cb, void *ctx)
{
    if (!s)
        return TRELLIS_ERR_INVALID_ARG;

    uint32_t interval = s->morph->config.constant_rate_interval_ms;
    if (interval == 0)
        interval = 50; /* default: 20 frames/sec = 50ms */

    s->send_cb  = cb;
    s->send_ctx = ctx;
    s->running  = true;

    uv_timer_start(&s->timer, sched_tick, interval, interval);
    return TRELLIS_OK;
}

void trellis_rate_scheduler_stop(trellis_rate_scheduler_t *s)
{
    if (!s || !s->running)
        return;

    s->running = false;
    uv_timer_stop(&s->timer);
}

trellis_err_t trellis_rate_scheduler_push(trellis_rate_scheduler_t *s,
                                           const uint8_t *data, size_t len)
{
    if (!s || !data || len == 0)
        return TRELLIS_ERR_INVALID_ARG;

    if (s->count >= SCHED_QUEUE_MAX)
        return TRELLIS_ERR_FULL;

    uint8_t *copy = malloc(len);
    if (!copy)
        return TRELLIS_ERR_NOMEM;

    memcpy(copy, data, len);
    s->queue[s->tail].data     = copy;
    s->queue[s->tail].len      = len;
    s->queue[s->tail].batch_id = 0;
    s->tail = (s->tail + 1) % SCHED_QUEUE_MAX;
    s->count++;

    return TRELLIS_OK;
}

trellis_err_t trellis_rate_scheduler_push_batch(trellis_rate_scheduler_t *s,
                                                 const trellis_buf_t *frames,
                                                 size_t count)
{
    if (!s || !frames || count == 0)
        return TRELLIS_ERR_INVALID_ARG;

    if (s->count + count > SCHED_QUEUE_MAX)
        return TRELLIS_ERR_FULL;

    uint32_t bid = ++s->next_batch_id;
    if (bid == 0) bid = ++s->next_batch_id;

    size_t enqueued   = 0;
    size_t saved_tail = s->tail;

    for (size_t i = 0; i < count; i++) {
        if (!frames[i].data || frames[i].len == 0)
            continue;

        uint8_t *copy = malloc(frames[i].len);
        if (!copy) {
            /* Roll back: walk backwards from current tail, freeing each
             * slot we wrote.  Restore tail to the saved position. */
            for (size_t r = 0; r < enqueued; r++) {
                size_t idx = (s->tail == 0)
                    ? SCHED_QUEUE_MAX - 1 : s->tail - 1;
                free(s->queue[idx].data);
                s->queue[idx].data = NULL;
                s->tail = idx;
            }
            s->count -= enqueued;
            return TRELLIS_ERR_NOMEM;
        }

        memcpy(copy, frames[i].data, frames[i].len);
        s->queue[s->tail].data     = copy;
        s->queue[s->tail].len      = frames[i].len;
        s->queue[s->tail].batch_id = bid;
        s->tail = (s->tail + 1) % SCHED_QUEUE_MAX;
        s->count++;
        enqueued++;
    }

    return TRELLIS_OK;
}

size_t trellis_rate_scheduler_queue_depth(const trellis_rate_scheduler_t *s)
{
    return s ? s->count : 0;
}

trellis_scheduler_stats_t trellis_rate_scheduler_stats(
    const trellis_rate_scheduler_t *s)
{
    trellis_scheduler_stats_t st = {0};
    if (!s)
        return st;
    st.ticks_total       = s->ticks_total;
    st.ticks_cover       = s->ticks_cover;
    st.ticks_real        = s->ticks_real;
    st.dp_pending_dummies = s->dp_pending_dummies;
    st.queue_depth       = s->count;
    return st;
}
#endif /* !__EMSCRIPTEN__ */
