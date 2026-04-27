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
 * bench_morph.c — Performance benchmarks for Trellis morph engine encode/decode.
 *
 * Tests each encoding mode with a 1 KB payload.
 *
 * Outputs JSON lines to stdout:
 *   {"op": "...", "iterations": N, "total_ms": X, "per_op_us": Y}
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "trellis/crypto.h"
#include "trellis/morph.h"

static double timespec_diff_ms(struct timespec *start, struct timespec *end)
{
    double sec  = (double)(end->tv_sec  - start->tv_sec);
    double nsec = (double)(end->tv_nsec - start->tv_nsec);
    return sec * 1000.0 + nsec / 1e6;
}

#define BENCH_BEGIN(n)                                          \
    do {                                                        \
        struct timespec _ts_start, _ts_end;                     \
        int _iters = (n);                                       \
        clock_gettime(CLOCK_MONOTONIC, &_ts_start);             \
        for (int _i = 0; _i < _iters; _i++) {

#define BENCH_END(label)                                        \
        }                                                       \
        clock_gettime(CLOCK_MONOTONIC, &_ts_end);               \
        double _total = timespec_diff_ms(&_ts_start, &_ts_end); \
        double _per   = (_total / _iters) * 1000.0;            \
        printf("{\"op\": \"%s\", \"iterations\": %d, "          \
               "\"total_ms\": %.3f, \"per_op_us\": %.3f}\n",   \
               (label), _iters, _total, _per);                  \
    } while (0)

static const char *encoding_name(trellis_encoding_t enc)
{
    switch (enc) {
    case TRELLIS_ENC_MSGPACK:  return "msgpack";
    case TRELLIS_ENC_CBOR:     return "cbor";
    case TRELLIS_ENC_PROTOBUF: return "protobuf";
    case TRELLIS_ENC_CUSTOM:   return "custom";
    default:                   return "unknown";
    }
}

static void bench_morph_encoding(trellis_encoding_t enc)
{
    trellis_morph_config_t cfg = trellis_morph_config_default();
    trellis_morph_t *morph = trellis_morph_new(&cfg);
    if (!morph) {
        fprintf(stderr, "bench_morph: failed to create morph ctx for %s\n",
                encoding_name(enc));
        return;
    }

    uint8_t session_key[32];
    memset(session_key, 0x55, sizeof(session_key));
    trellis_morph_set_session_key(morph, session_key, sizeof(session_key));

    uint8_t payload[1024];
    memset(payload, 0xAA, sizeof(payload));

    char label_enc[64], label_dec[64];
    snprintf(label_enc, sizeof(label_enc), "morph_encode_%s_1kb",
             encoding_name(enc));
    snprintf(label_dec, sizeof(label_dec), "morph_decode_%s_1kb",
             encoding_name(enc));

    // Encode benchmark.
    BENCH_BEGIN(1000)
        trellis_buf_t out = {0};
        trellis_morph_encode(morph, payload, sizeof(payload), &out);
        trellis_buf_free(&out);
    BENCH_END(label_enc);

    // Prepare a single encoded blob for decode benchmarking.
    trellis_buf_t encoded = {0};
    trellis_morph_encode(morph, payload, sizeof(payload), &encoded);

    // Decode benchmark.
    if (encoded.data && encoded.len > 0) {
        BENCH_BEGIN(1000)
            trellis_buf_t dec = {0};
            trellis_morph_decode(morph, encoded.data, encoded.len, &dec);
            trellis_buf_free(&dec);
        BENCH_END(label_dec);
    }

    trellis_buf_free(&encoded);
    trellis_morph_free(morph);
}

static void bench_morph_cell(void)
{
    trellis_morph_config_t cfg = trellis_morph_config_default();
    cfg.cell_mode = true;
    cfg.cell_size = TRELLIS_MORPH_CELL_SIZE_DEFAULT;

    trellis_morph_t *morph = trellis_morph_new(&cfg);
    if (!morph) return;

    uint8_t payload[1024];
    memset(payload, 0xBB, sizeof(payload));

    BENCH_BEGIN(1000)
        trellis_buf_t cells[16];
        size_t cell_count = 0;
        trellis_morph_cell_fragment(morph, payload, sizeof(payload),
                                    cells, &cell_count, 16);
        for (size_t c = 0; c < cell_count; c++)
            trellis_buf_free(&cells[c]);
    BENCH_END("morph_cell_fragment_1kb");

    // Prepare cells for reassembly benchmark.
    trellis_buf_t cells[16];
    size_t cell_count = 0;
    trellis_morph_cell_fragment(morph, payload, sizeof(payload),
                                cells, &cell_count, 16);

    if (cell_count > 0) {
        BENCH_BEGIN(1000)
            trellis_buf_t reassembled = {0};
            trellis_morph_cell_reassemble(cells, cell_count, &reassembled);
            trellis_buf_free(&reassembled);
        BENCH_END("morph_cell_reassemble_1kb");
    }

    for (size_t c = 0; c < cell_count; c++)
        trellis_buf_free(&cells[c]);

    trellis_morph_free(morph);
}

static void bench_constant_rate_pad(void)
{
    trellis_morph_config_t cfg = trellis_morph_config_default();
    cfg.constant_rate = true;
    cfg.constant_rate_frame_size = 512;

    trellis_morph_t *morph = trellis_morph_new(&cfg);
    if (!morph) return;

    uint8_t payload[256];
    memset(payload, 0xDD, sizeof(payload));

    BENCH_BEGIN(100000)
        trellis_buf_t out = {0};
        trellis_morph_constant_rate_pad(morph, payload, sizeof(payload), &out);
        trellis_buf_free(&out);
    BENCH_END("constant_rate_pad_256b_payload");

    BENCH_BEGIN(100000)
        trellis_buf_t out = {0};
        trellis_morph_constant_rate_pad(morph, NULL, 0, &out);
        trellis_buf_free(&out);
    BENCH_END("constant_rate_pad_cover");

    trellis_morph_free(morph);
}

int main(void)
{
    if (trellis_crypto_init() != TRELLIS_OK) {
        fprintf(stderr, "bench_morph: crypto init failed\n");
        return 1;
    }

    bench_morph_encoding(TRELLIS_ENC_MSGPACK);
    bench_morph_encoding(TRELLIS_ENC_CBOR);
    bench_morph_encoding(TRELLIS_ENC_PROTOBUF);
    bench_morph_encoding(TRELLIS_ENC_CUSTOM);
    bench_morph_cell();
    bench_constant_rate_pad();

    trellis_crypto_cleanup();
    return 0;
}
