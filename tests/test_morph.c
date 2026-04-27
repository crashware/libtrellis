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

#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <trellis/trellis.h>
#include <uv.h>

#define RUN_TEST(fn) do { printf("  %-50s", #fn "..."); fn(); printf("PASS\n"); } while(0)
#define ASSERT(cond) do { if (!(cond)) { fprintf(stderr, "FAIL at %s:%d: %s\n", __FILE__, __LINE__, #cond); abort(); } } while(0)

static int mem_is_zero(const uint8_t *p, size_t len) {
    for (size_t i = 0; i < len; i++)
        if (p[i] != 0) return 0;
    return 1;
}

static void test_morph_defaults(void) {
    trellis_morph_config_t cfg = trellis_morph_config_default();
    ASSERT(cfg.metamorphic == false);
    ASSERT(cfg.min_jitter_ms >= 0);
    ASSERT(cfg.max_jitter_ms >= cfg.min_jitter_ms);
    ASSERT(cfg.burst_min_chunk > 0);
    ASSERT(cfg.burst_max_chunk >= cfg.burst_min_chunk);
}

static void test_morph_passthrough_roundtrip(void) {
    trellis_morph_config_t cfg = trellis_morph_config_default();

    trellis_morph_t *morph = trellis_morph_new(&cfg);
    ASSERT(morph != NULL);

    const uint8_t payload[] = "morph roundtrip test payload data";
    trellis_buf_t encoded = {0}, decoded = {0};

    ASSERT(trellis_morph_encode(morph, payload, sizeof(payload) - 1, &encoded) == TRELLIS_OK);
    ASSERT(encoded.len > 0);
    ASSERT(encoded.len == sizeof(payload) - 1);
    ASSERT(memcmp(encoded.data, payload, encoded.len) == 0);

    ASSERT(trellis_morph_decode(morph, encoded.data, encoded.len, &decoded) == TRELLIS_OK);
    ASSERT(decoded.len == sizeof(payload) - 1);
    ASSERT(memcmp(decoded.data, payload, decoded.len) == 0);

    trellis_buf_free(&encoded);
    trellis_buf_free(&decoded);
    trellis_morph_free(morph);
}

static void test_morph_metamorphic(void) {
    trellis_morph_config_t cfg = trellis_morph_config_default();
    cfg.metamorphic = true;

    trellis_morph_t *morph = trellis_morph_new(&cfg);
    ASSERT(morph != NULL);

    uint8_t session_key[32];
    memset(session_key, 0xCC, sizeof(session_key));
    ASSERT(trellis_morph_set_session_key(morph, session_key, sizeof(session_key)) == TRELLIS_OK);

    trellis_morph_inspect_t snap = trellis_morph_inspect(morph);
    ASSERT(!mem_is_zero(snap.magic_bytes, 4));

    const uint8_t payload[] = "metamorphic encoding test";

    trellis_buf_t enc = {0}, dec = {0};
    ASSERT(trellis_morph_encode(morph, payload, sizeof(payload) - 1, &enc) == TRELLIS_OK);
    ASSERT(enc.len > sizeof(payload) - 1);

    // Create a second morph with the same key to decode.
    trellis_morph_t *morph2 = trellis_morph_new(&cfg);
    ASSERT(morph2 != NULL);
    ASSERT(trellis_morph_set_session_key(morph2, session_key, sizeof(session_key)) == TRELLIS_OK);

    ASSERT(trellis_morph_decode(morph2, enc.data, enc.len, &dec) == TRELLIS_OK);
    ASSERT(dec.len == sizeof(payload) - 1);
    ASSERT(memcmp(dec.data, payload, dec.len) == 0);

    trellis_buf_free(&enc);
    trellis_buf_free(&dec);
    trellis_morph_free(morph);
    trellis_morph_free(morph2);
}

static void test_morph_metamorphic_padding(void) {
    trellis_morph_config_t cfg = trellis_morph_config_default();
    cfg.metamorphic = true;

    trellis_morph_t *morph = trellis_morph_new(&cfg);
    ASSERT(morph != NULL);

    uint8_t session_key[32];
    memset(session_key, 0xAA, sizeof(session_key));
    ASSERT(trellis_morph_set_session_key(morph, session_key, sizeof(session_key)) == TRELLIS_OK);

    const uint8_t small[] = "hi";
    const uint8_t medium[] = "a somewhat longer message for padding test purposes";

    trellis_buf_t enc_small = {0}, enc_medium = {0};
    ASSERT(trellis_morph_encode(morph, small, sizeof(small) - 1, &enc_small) == TRELLIS_OK);
    ASSERT(trellis_morph_encode(morph, medium, sizeof(medium) - 1, &enc_medium) == TRELLIS_OK);

    // Encoded sizes should be padded to the same target boundary.
    ASSERT(enc_small.len > sizeof(small) - 1);
    ASSERT(enc_medium.len > sizeof(medium) - 1);

    trellis_buf_free(&enc_small);
    trellis_buf_free(&enc_medium);
    trellis_morph_free(morph);
}

static void test_morph_cover_traffic(void) {
    trellis_morph_config_t cfg = trellis_morph_config_default();
    trellis_morph_t *morph = trellis_morph_new(&cfg);
    ASSERT(morph != NULL);

    trellis_buf_t cover = {0};
    ASSERT(trellis_morph_generate_cover(morph, &cover) == TRELLIS_OK);
    ASSERT(cover.len > 0);
    ASSERT(!mem_is_zero(cover.data, cover.len));

    trellis_buf_free(&cover);
    trellis_morph_free(morph);
}

static void test_morph_jitter(void) {
    trellis_morph_config_t cfg = trellis_morph_config_default();
    cfg.min_jitter_ms = 10;
    cfg.max_jitter_ms = 100;

    trellis_morph_t *morph = trellis_morph_new(&cfg);
    ASSERT(morph != NULL);

    uint32_t delay = trellis_morph_jitter_delay(morph);
    ASSERT(delay >= 10);
    ASSERT(delay <= 100);

    trellis_morph_free(morph);
}

static void test_morph_require_constant_rate(void) {
    trellis_morph_config_t cfg = trellis_morph_config_default();
    cfg.require_constant_rate = true;
    cfg.constant_rate = false;

    trellis_morph_t *morph = trellis_morph_new(&cfg);
    ASSERT(morph != NULL);

    const uint8_t payload[] = "should fail without constant rate";
    trellis_buf_t out = {0};

    // Encode must fail when require_constant_rate is set but constant_rate is off.
    ASSERT(trellis_morph_encode(morph, payload, sizeof(payload) - 1, &out)
           == TRELLIS_ERR_INVALID_STATE);
    ASSERT(out.data == NULL);

    trellis_morph_free(morph);

    // With constant_rate enabled, encode should succeed.
    cfg.constant_rate = true;
    morph = trellis_morph_new(&cfg);
    ASSERT(morph != NULL);

    ASSERT(trellis_morph_encode(morph, payload, sizeof(payload) - 1, &out)
           == TRELLIS_OK);
    ASSERT(out.len > 0);

    trellis_buf_free(&out);
    trellis_morph_free(morph);
}

static void test_morph_dp_epsilon_config(void) {
    trellis_morph_config_t cfg = trellis_morph_config_default();

    // Default dp_epsilon should be 0 (disabled)
    ASSERT(cfg.dp_epsilon == 0.0);

    // Setting it should be preserved through morph_new.
    cfg.dp_epsilon = 1.0;
    cfg.constant_rate = true;
    cfg.constant_rate_interval_ms = 50;

    trellis_morph_t *morph = trellis_morph_new(&cfg);
    ASSERT(morph != NULL);
    ASSERT(trellis_morph_is_constant_rate(morph));

    trellis_morph_free(morph);
}

static void test_morph_cell_rate_validation(void) {
    trellis_morph_config_t cfg = trellis_morph_config_default();
    cfg.cell_mode = true;
    cfg.constant_rate = true;
    cfg.cell_size = 1024;

    // Mismatched sizes should be rejected.
    cfg.constant_rate_frame_size = 512;
    trellis_morph_t *morph = trellis_morph_new(&cfg);
    ASSERT(morph == NULL);

    // Matching sizes should be accepted.
    cfg.constant_rate_frame_size = 1024;
    morph = trellis_morph_new(&cfg);
    ASSERT(morph != NULL);
    trellis_morph_free(morph);

    // Zero frame_size should default to cell_size.
    cfg.constant_rate_frame_size = 0;
    morph = trellis_morph_new(&cfg);
    ASSERT(morph != NULL);
    ASSERT(trellis_morph_constant_rate_interval(morph) == 0 ||
           trellis_morph_is_constant_rate(morph));
    trellis_morph_free(morph);
}

typedef struct {
    int  total_frames;
    int  cover_frames;
    int  real_frames;
} sched_test_ctx_t;

static void sched_test_send_cb(const uint8_t *data, size_t len,
                                bool is_cover, void *ctx) {
    sched_test_ctx_t *tc = (sched_test_ctx_t *)ctx;
    (void)data; (void)len;
    tc->total_frames++;
    if (is_cover) tc->cover_frames++;
    else tc->real_frames++;
}

static void test_scheduler_basic(void) {
    trellis_morph_config_t cfg = trellis_morph_config_default();
    cfg.constant_rate = true;
    cfg.constant_rate_interval_ms = 10;
    cfg.constant_rate_frame_size = 512;

    trellis_morph_t *morph = trellis_morph_new(&cfg);
    ASSERT(morph != NULL);

    uv_loop_t loop;
    uv_loop_init(&loop);

    trellis_rate_scheduler_t *s = trellis_rate_scheduler_new(morph, NULL, &loop);
    ASSERT(s != NULL);

    sched_test_ctx_t ctx = {0};
    ASSERT(trellis_rate_scheduler_start(s, sched_test_send_cb, &ctx) == TRELLIS_OK);

    // Push one real frame.
    uint8_t payload[100];
    memset(payload, 0x42, sizeof(payload));
    ASSERT(trellis_rate_scheduler_push(s, payload, sizeof(payload)) == TRELLIS_OK);
    ASSERT(trellis_rate_scheduler_queue_depth(s) == 1);

    // Run the loop for a few ticks.
    for (int i = 0; i < 5; i++)
        uv_run(&loop, UV_RUN_ONCE);

    ASSERT(ctx.total_frames > 0);
    ASSERT(ctx.real_frames >= 1);
    ASSERT(trellis_rate_scheduler_queue_depth(s) == 0);

    trellis_rate_scheduler_stop(s);
    trellis_rate_scheduler_free(s);
    uv_loop_close(&loop);
    trellis_morph_free(morph);
}

static void test_scheduler_cover_when_idle(void) {
    trellis_morph_config_t cfg = trellis_morph_config_default();
    cfg.constant_rate = true;
    cfg.constant_rate_interval_ms = 10;
    cfg.constant_rate_frame_size = 256;

    trellis_morph_t *morph = trellis_morph_new(&cfg);
    ASSERT(morph != NULL);

    uv_loop_t loop;
    uv_loop_init(&loop);

    trellis_rate_scheduler_t *s = trellis_rate_scheduler_new(morph, NULL, &loop);
    ASSERT(s != NULL);

    sched_test_ctx_t ctx = {0};
    ASSERT(trellis_rate_scheduler_start(s, sched_test_send_cb, &ctx) == TRELLIS_OK);

    // Run without pushing any data -- should get cover frames.
    for (int i = 0; i < 10; i++)
        uv_run(&loop, UV_RUN_ONCE);

    ASSERT(ctx.cover_frames > 0);
    ASSERT(ctx.real_frames == 0);

    trellis_rate_scheduler_stop(s);
    trellis_rate_scheduler_free(s);
    uv_loop_close(&loop);
    trellis_morph_free(morph);
}

static void test_scheduler_dp_dummies(void) {
    trellis_morph_config_t cfg = trellis_morph_config_default();
    cfg.constant_rate = true;
    cfg.constant_rate_interval_ms = 10;
    cfg.constant_rate_frame_size = 256;
    cfg.dp_epsilon = 1.0;

    trellis_morph_t *morph = trellis_morph_new(&cfg);
    ASSERT(morph != NULL);

    uv_loop_t loop;
    uv_loop_init(&loop);

    trellis_rate_scheduler_t *s = trellis_rate_scheduler_new(morph, NULL, &loop);
    ASSERT(s != NULL);

    sched_test_ctx_t ctx = {0};
    ASSERT(trellis_rate_scheduler_start(s, sched_test_send_cb, &ctx) == TRELLIS_OK);

    // Push a real frame to trigger DP noise.
    uint8_t payload[64];
    memset(payload, 0xAA, sizeof(payload));
    ASSERT(trellis_rate_scheduler_push(s, payload, sizeof(payload)) == TRELLIS_OK);

    // Run for enough ticks to process the real frame and any DP interval noise.
    for (int i = 0; i < 30; i++)
        uv_run(&loop, UV_RUN_ONCE);

    // Should have at least one real frame and some cover (DP or idle)
    ASSERT(ctx.real_frames >= 1);
    ASSERT(ctx.total_frames > ctx.real_frames);

    trellis_rate_scheduler_stop(s);
    trellis_rate_scheduler_free(s);
    uv_loop_close(&loop);
    trellis_morph_free(morph);
}

static void test_scheduler_stats(void) {
    trellis_morph_config_t cfg = trellis_morph_config_default();
    cfg.constant_rate = true;
    cfg.constant_rate_interval_ms = 10;
    cfg.constant_rate_frame_size = 256;

    trellis_morph_t *morph = trellis_morph_new(&cfg);
    ASSERT(morph != NULL);

    uv_loop_t loop;
    uv_loop_init(&loop);

    trellis_rate_scheduler_t *s = trellis_rate_scheduler_new(morph, NULL, &loop);
    ASSERT(s != NULL);

    sched_test_ctx_t ctx = {0};
    ASSERT(trellis_rate_scheduler_start(s, sched_test_send_cb, &ctx) == TRELLIS_OK);

    uint8_t payload[100];
    memset(payload, 0xBB, sizeof(payload));
    trellis_rate_scheduler_push(s, payload, sizeof(payload));

    for (int i = 0; i < 10; i++)
        uv_run(&loop, UV_RUN_ONCE);

    trellis_scheduler_stats_t st = trellis_rate_scheduler_stats(s);
    ASSERT(st.ticks_total == st.ticks_real + st.ticks_cover);
    ASSERT(st.ticks_real >= 1);
    ASSERT(st.queue_depth == 0);

    trellis_rate_scheduler_stop(s);
    trellis_rate_scheduler_free(s);
    uv_loop_close(&loop);
    trellis_morph_free(morph);
}

static void test_scheduler_batch_push(void) {
    trellis_morph_config_t cfg = trellis_morph_config_default();
    cfg.constant_rate = true;
    cfg.constant_rate_interval_ms = 10;
    cfg.constant_rate_frame_size = 256;

    trellis_morph_t *morph = trellis_morph_new(&cfg);
    ASSERT(morph != NULL);

    uv_loop_t loop;
    uv_loop_init(&loop);

    trellis_rate_scheduler_t *s = trellis_rate_scheduler_new(morph, NULL, &loop);
    ASSERT(s != NULL);

    sched_test_ctx_t ctx = {0};
    ASSERT(trellis_rate_scheduler_start(s, sched_test_send_cb, &ctx) == TRELLIS_OK);

    // Push a batch of 3 frames.
    uint8_t data1[64], data2[64], data3[64];
    memset(data1, 0x11, sizeof(data1));
    memset(data2, 0x22, sizeof(data2));
    memset(data3, 0x33, sizeof(data3));

    trellis_buf_t frames[3] = {
        { .data = data1, .len = sizeof(data1) },
        { .data = data2, .len = sizeof(data2) },
        { .data = data3, .len = sizeof(data3) },
    };

    ASSERT(trellis_rate_scheduler_push_batch(s, frames, 3) == TRELLIS_OK);
    ASSERT(trellis_rate_scheduler_queue_depth(s) == 3);

    for (int i = 0; i < 20; i++)
        uv_run(&loop, UV_RUN_ONCE);

    ASSERT(ctx.real_frames >= 3);
    ASSERT(trellis_rate_scheduler_queue_depth(s) == 0);

    trellis_rate_scheduler_stop(s);
    trellis_rate_scheduler_free(s);
    uv_loop_close(&loop);
    trellis_morph_free(morph);
}

int main(void) {
    printf("=== Test Suite: Morph ===\n");

    ASSERT(trellis_crypto_init() == TRELLIS_OK);

    RUN_TEST(test_morph_defaults);
    RUN_TEST(test_morph_passthrough_roundtrip);
    RUN_TEST(test_morph_metamorphic);
    RUN_TEST(test_morph_metamorphic_padding);
    RUN_TEST(test_morph_cover_traffic);
    RUN_TEST(test_morph_jitter);
    RUN_TEST(test_morph_require_constant_rate);
    RUN_TEST(test_morph_dp_epsilon_config);
    RUN_TEST(test_morph_cell_rate_validation);
    RUN_TEST(test_scheduler_basic);
    RUN_TEST(test_scheduler_cover_when_idle);
    RUN_TEST(test_scheduler_dp_dummies);
    RUN_TEST(test_scheduler_stats);
    RUN_TEST(test_scheduler_batch_push);

    trellis_crypto_cleanup();

    printf("=== All morph tests passed ===\n");
    return 0;
}
