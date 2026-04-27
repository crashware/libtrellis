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

#ifndef TRELLIS_MORPH_H
#define TRELLIS_MORPH_H

#include "types.h"
#include "error.h"

// Forward-declare uv_loop_t to avoid pulling in all of libuv in consumer headers.
typedef struct uv_loop_s uv_loop_t;

#ifdef __cplusplus
extern "C" {
#endif

typedef enum trellis_encoding {
    TRELLIS_ENC_MSGPACK  = 0,
    TRELLIS_ENC_CBOR     = 1,
    TRELLIS_ENC_PROTOBUF = 2,
    TRELLIS_ENC_CUSTOM   = 3,
} trellis_encoding_t;

typedef struct trellis_morph_ctx {
    uint8_t            magic_bytes[4];
    trellis_encoding_t encoding;
    uint32_t           padding_target;
    uint32_t           format_index;
} trellis_morph_ctx_t;

typedef struct trellis_morph trellis_morph_t;

typedef struct trellis_morph_config {
    bool               metamorphic;
    uint32_t           min_jitter_ms;
    uint32_t           max_jitter_ms;
    uint32_t           cover_traffic_interval_ms;
    uint32_t           burst_min_chunk;
    uint32_t           burst_max_chunk;

    bool               constant_rate;
    uint32_t           constant_rate_interval_ms;
    uint32_t           constant_rate_frame_size;

    /* Fixed-size cell padding (TRELLIS_MORPH_CELL_MODE).
     * When cell_mode is true every outgoing frame is split into cells of
     * exactly cell_size bytes, providing uniform-length traffic to defeat
     * packet-length fingerprinting.  Defaults: cell_mode=false, cell_size=1024. */
    bool               cell_mode;
    uint32_t           cell_size;

    /* When true, trellis_morph_encode() returns TRELLIS_ERR_INVALID_STATE
     * unless constant_rate is also enabled.  Allows gardens and SDK config
     * to enforce constant-rate mode for all connections. */
    bool               require_constant_rate;

    /* Differential-privacy budget for cover traffic injection.  When > 0,
     * the constant-rate scheduler counts real frames within a DP interval
     * (10x the tick period), then injects Laplace(1/epsilon) noise dummies
     * at the interval boundary.  This provides epsilon-DP for the per-
     * interval message count without composition degradation across
     * individual messages.  0 = disabled (default). */
    double             dp_epsilon;
} trellis_morph_config_t;

trellis_morph_config_t trellis_morph_config_default(void);

trellis_morph_t *trellis_morph_new(const trellis_morph_config_t *cfg);
void             trellis_morph_free(trellis_morph_t *morph);

// Enable or disable constant-rate padding mode (padding negotiation).
void trellis_morph_set_constant_rate(trellis_morph_t *morph, bool enabled);

trellis_err_t trellis_morph_set_session_key(trellis_morph_t *morph,
                                            const uint8_t *key, size_t key_len);
trellis_err_t trellis_morph_encode(trellis_morph_t *morph,
                                   const uint8_t *data, size_t len,
                                   trellis_buf_t *out);
trellis_err_t trellis_morph_decode(trellis_morph_t *morph,
                                   const uint8_t *data, size_t len,
                                   trellis_buf_t *out);

typedef struct trellis_morph_inspect {
    trellis_encoding_t encoding;
    uint32_t           format_index;
    uint32_t           frame_count;
    uint8_t            magic_bytes[4];
} trellis_morph_inspect_t;

trellis_morph_inspect_t trellis_morph_inspect(const trellis_morph_t *morph);

uint32_t trellis_morph_jitter_delay(const trellis_morph_t *morph);
trellis_err_t trellis_morph_generate_cover(const trellis_morph_t *morph,
                                           trellis_buf_t *out);

typedef struct trellis_ratchet trellis_ratchet_t;
trellis_err_t trellis_morph_generate_cover_ratchet(const trellis_morph_t *morph,
                                                   trellis_ratchet_t *ratchet,
                                                   trellis_buf_t *out);

trellis_err_t trellis_morph_constant_rate_pad(trellis_morph_t *morph,
                                              const uint8_t *data, size_t len,
                                              trellis_buf_t *out);

bool          trellis_morph_is_constant_rate(const trellis_morph_t *morph);
uint32_t      trellis_morph_constant_rate_interval(const trellis_morph_t *morph);
void          trellis_morph_set_require_constant_rate(trellis_morph_t *morph, bool require);

#define TRELLIS_MORPH_CELL_SIZE_DEFAULT  1024u
#define TRELLIS_MORPH_CELL_HEADER_LEN    6u   /* flags(1) + seq(3) + payload_len(2) */
#define TRELLIS_MORPH_CELL_FLAG_LAST     0x01u

/*
 * Fragment `data` into fixed-size cells of morph->config.cell_size bytes each.
 * Each cell carries a 6-byte header followed by payload and random padding.
 * `cells` must point to an array of at least `max_cells` trellis_buf_t entries.
 * The caller must free each cell with trellis_buf_free() when done.
 */
trellis_err_t trellis_morph_cell_fragment(const trellis_morph_t *morph,
                                           const uint8_t *data, size_t len,
                                           trellis_buf_t *cells, size_t *cell_count,
                                           size_t max_cells);

/*
 * Reassemble cells produced by trellis_morph_cell_fragment() back into the
 * original plaintext.  Cells may be in any order; the LAST flag and sequence
 * numbers guide reconstruction.
 */
trellis_err_t trellis_morph_cell_reassemble(const trellis_buf_t *cells,
                                             size_t cell_count,
                                             trellis_buf_t *out);

#ifndef __EMSCRIPTEN__

/*
 * A libuv-timer-driven scheduler that emits frames at a fixed rate.
 * Real frames queued via trellis_rate_scheduler_push() are padded to the
 * configured constant_rate_frame_size and sent on the next tick.  When idle,
 * ratchet-encrypted COVER frames fill the slot so the wire shows a perfectly
 * uniform bitrate.
 *
 * Usage:
 *   s = trellis_rate_scheduler_new(morph, ratchet, loop);
 *   trellis_rate_scheduler_start(s, my_send_cb, ctx);
 *   trellis_rate_scheduler_push(s, data, len);  // from any thread calling uv_async
 *   trellis_rate_scheduler_stop(s);
 *   trellis_rate_scheduler_free(s);
 */
typedef struct trellis_rate_scheduler trellis_rate_scheduler_t;

/* Called on each scheduler tick with the padded/encrypted frame.
 * is_cover=true when this is a synthetic cover frame (no real payload). */
typedef void (*trellis_rate_send_cb)(const uint8_t *data, size_t len,
                                     bool is_cover, void *ctx);

trellis_rate_scheduler_t *trellis_rate_scheduler_new(trellis_morph_t *morph,
                                                      trellis_ratchet_t *ratchet,
                                                      uv_loop_t *loop);
void          trellis_rate_scheduler_free(trellis_rate_scheduler_t *s);
trellis_err_t trellis_rate_scheduler_start(trellis_rate_scheduler_t *s,
                                            trellis_rate_send_cb cb, void *ctx);
void          trellis_rate_scheduler_stop(trellis_rate_scheduler_t *s);

// Enqueue a real-data frame for the next available slot.
trellis_err_t trellis_rate_scheduler_push(trellis_rate_scheduler_t *s,
                                           const uint8_t *data, size_t len);

/* Enqueue multiple frames as a logical batch.  DP noise is only triggered
 * once per batch (at the interval boundary), not per fragment, avoiding
 * multiplicative overhead when a message is split into cells. */
trellis_err_t trellis_rate_scheduler_push_batch(trellis_rate_scheduler_t *s,
                                                 const trellis_buf_t *frames,
                                                 size_t count);

// How many real frames are waiting in the queue.
size_t trellis_rate_scheduler_queue_depth(const trellis_rate_scheduler_t *s);

typedef struct trellis_scheduler_stats {
    uint64_t ticks_total;
    uint64_t ticks_cover;
    uint64_t ticks_real;
    uint32_t dp_pending_dummies;
    uint64_t queue_depth;
} trellis_scheduler_stats_t;

trellis_scheduler_stats_t trellis_rate_scheduler_stats(
    const trellis_rate_scheduler_t *s);

#endif /* !__EMSCRIPTEN__ */

typedef enum trellis_timing_profile {
    TRELLIS_TIMING_DEFAULT = 0,
    TRELLIS_TIMING_HTTPS   = 1,
    TRELLIS_TIMING_HTTP2   = 2,
    TRELLIS_TIMING_DNS     = 3,
    TRELLIS_TIMING_QUIC    = 4,
    TRELLIS_TIMING_PROFILE_COUNT = 5, /* always last */
} trellis_timing_profile_t;

void     trellis_morph_set_timing_profile(trellis_morph_t *morph,
                                          trellis_timing_profile_t profile);
uint32_t trellis_morph_protocol_jitter(const trellis_morph_t *morph);
bool     trellis_morph_should_burst(const trellis_morph_t *morph,
                                    uint32_t *burst_count);

/*
 * The mimicry probe tests each camouflage profile (timing + encoding bundle)
 * by attempting TCP connections to profile-specific probe endpoints.  The
 * first reachable profile in priority order is automatically applied to the
 * attached morph engine.  Re-probing occurs:
 *   • periodically (every TRELLIS_MIMICRY_REPROBE_INTERVAL_MS milliseconds)
 *   • immediately when trellis_mimicry_probe_on_failure() is called
 *
 * Default probe endpoints are well-known CDN/DNS IPs on ports matching each
 * protocol (443 for TLS/HTTP2/QUIC, 53 for DNS, 80 for plain HTTP).
 * Custom endpoints can be added via trellis_mimicry_probe_add_endpoint().
 */
#define TRELLIS_MIMICRY_REPROBE_INTERVAL_MS  300000u /* 5 minutes */
#define TRELLIS_MIMICRY_PROBE_TIMEOUT_MS       3000u /* per-probe TCP timeout */
#define TRELLIS_MIMICRY_MAX_ENDPOINTS             4u /* per profile */

typedef struct trellis_mimicry_probe trellis_mimicry_probe_t;

// Called when a new profile is selected (including initial selection).
typedef void (*trellis_mimicry_select_cb)(trellis_timing_profile_t profile,
                                          void *ctx);

trellis_mimicry_probe_t *trellis_mimicry_probe_new(uv_loop_t *loop,
                                                    trellis_morph_t *morph);
void          trellis_mimicry_probe_free(trellis_mimicry_probe_t *p);

// Begin probing; cb is invoked on every profile change (may be NULL).
trellis_err_t trellis_mimicry_probe_start(trellis_mimicry_probe_t *p,
                                           trellis_mimicry_select_cb cb,
                                           void *ctx);
void          trellis_mimicry_probe_stop(trellis_mimicry_probe_t *p);

// Signal that the active profile has failed; triggers immediate re-probe.
void          trellis_mimicry_probe_on_failure(trellis_mimicry_probe_t *p);

// Currently selected profile (TRELLIS_TIMING_DEFAULT until first probe).
trellis_timing_profile_t
              trellis_mimicry_probe_selected(const trellis_mimicry_probe_t *p);

/* Override the default probe endpoint for a profile.
 * `ip` must be a dotted-decimal IPv4 or bracketed IPv6 address string. */
trellis_err_t trellis_mimicry_probe_add_endpoint(trellis_mimicry_probe_t *p,
                                                  trellis_timing_profile_t profile,
                                                  const char *ip,
                                                  uint16_t port);

#ifdef __cplusplus
}
#endif

#endif /* TRELLIS_MORPH_H */
