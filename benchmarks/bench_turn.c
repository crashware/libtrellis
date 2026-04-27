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
 * bench_turn.c — Performance benchmarks for STUN/TURN message processing.
 *
 * Measures parsing/serialization throughput and TURN server processing
 * overhead to quantify the cost of replacing coturn with vine-relay.
 *
 * Outputs JSON lines to stdout:
 *   {"op": "...", "iterations": N, "total_ms": X, "per_op_us": Y}
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "trellis/turn.h"
#include <sodium.h>

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

static uint8_t g_binding_req[20];
static uint8_t g_binding_resp[128];
static size_t  g_binding_resp_len;

static void init_test_packets(void)
{
    // Minimal STUN Binding Request.
    g_binding_req[0] = 0x00; g_binding_req[1] = 0x01;
    g_binding_req[2] = 0x00; g_binding_req[3] = 0x00;
    g_binding_req[4] = 0x21; g_binding_req[5] = 0x12;
    g_binding_req[6] = 0xA4; g_binding_req[7] = 0x42;
    randombytes_buf(g_binding_req + 8, 12);

    // Build a Binding Success Response with XOR-MAPPED-ADDRESS.
    stun_addr_t addr = {0};
    addr.family = 0x01;
    addr.port = 50000;
    addr.ip.v4[0] = 192; addr.ip.v4[1] = 168;
    addr.ip.v4[2] = 1; addr.ip.v4[3] = 100;

    uint8_t attr_buf[64];
    size_t attr_len = stun_attr_xor_mapped_address(&addr, g_binding_req + 8,
                                                    attr_buf, sizeof(attr_buf));

    g_binding_resp_len = stun_build_response(
        STUN_METHOD_BINDING | STUN_CLASS_SUCCESS,
        g_binding_req + 8, attr_buf, attr_len,
        NULL, 0, g_binding_resp, sizeof(g_binding_resp));
}

static void bench_stun_is_stun(void)
{
    volatile bool result;
    BENCH_BEGIN(1000000)
        result = stun_is_stun(g_binding_req, 20);
    BENCH_END("stun_is_stun");
    (void)result;
}

static void bench_stun_parse_binding(void)
{
    stun_msg_t msg;
    BENCH_BEGIN(500000)
        memset(&msg, 0, sizeof(msg));
        stun_parse(g_binding_req, 20, &msg);
    BENCH_END("stun_parse_binding_req");
}

static void bench_stun_parse_response(void)
{
    stun_msg_t msg;
    BENCH_BEGIN(500000)
        memset(&msg, 0, sizeof(msg));
        stun_parse(g_binding_resp, g_binding_resp_len, &msg);
    BENCH_END("stun_parse_binding_resp");
}

static void bench_stun_build_response(void)
{
    uint8_t txn[STUN_TXN_ID_LEN] = {0};
    uint8_t buf[128];

    stun_addr_t addr = {0};
    addr.family = 0x01; addr.port = 50000;
    uint8_t attr_buf[64];
    size_t attr_len = stun_attr_xor_mapped_address(&addr, txn, attr_buf, sizeof(attr_buf));

    BENCH_BEGIN(500000)
        stun_build_response(
            STUN_METHOD_BINDING | STUN_CLASS_SUCCESS,
            txn, attr_buf, attr_len,
            NULL, 0, buf, sizeof(buf));
    BENCH_END("stun_build_response");
}

static void bench_stun_build_response_with_integrity(void)
{
    uint8_t txn[STUN_TXN_ID_LEN] = {0};
    uint8_t key[16] = {0};
    uint8_t buf[256];

    stun_addr_t addr = {0};
    addr.family = 0x01; addr.port = 50000;
    uint8_t attr_buf[64];
    size_t attr_len = stun_attr_xor_mapped_address(&addr, txn, attr_buf, sizeof(attr_buf));

    BENCH_BEGIN(100000)
        stun_build_response(
            STUN_METHOD_BINDING | STUN_CLASS_SUCCESS,
            txn, attr_buf, attr_len,
            key, sizeof(key), buf, sizeof(buf));
    BENCH_END("stun_build_response_hmac");
}

static void bench_turn_server_process_binding(void)
{
    turn_server_t srv;
    stun_addr_t ext = {0};
    ext.family = 0x01; ext.port = 443;
    turn_server_init(&srv, "bench.realm", "bench-secret", &ext);

    // no-op send callback to avoid I/O in the benchmark.
    srv.send_cb = NULL;

    stun_addr_t client = {0};
    client.family = 0x01; client.port = 50000;

    BENCH_BEGIN(100000)
        turn_server_process(&srv, g_binding_req, 20, &client, 17);
    BENCH_END("turn_server_process_binding");

    turn_server_destroy(&srv);
}

static void bench_channel_data_detect(void)
{
    uint8_t pkt[1204];
    pkt[0] = 0x40; pkt[1] = 0x00;
    pkt[2] = 0x04; pkt[3] = 0xB0; /* 1200 bytes payload */
    memset(pkt + 4, 0xAA, 1200);

    volatile bool result;
    BENCH_BEGIN(1000000)
        result = stun_is_channel_data(pkt, sizeof(pkt));
    BENCH_END("stun_is_channel_data");
    (void)result;
}

static void bench_overhead_summary(void)
{
    /* Print bandwidth overhead comparison.
     * SESSION_DATA wire format: trellis_msg header (88) + payload + hybrid signature (~4691).
     * Payload = circuit_id(4) + dest_fp(32) + nonce(12) + ciphertext(1200) + tag(16) = 1264.
     * Total wire size per packet ≈ 88 + 1264 + 4691 = 6043 bytes. */
    printf("\n--- Bandwidth Overhead Summary ---\n");
    printf("Media packet size:            1200 bytes (typical WebRTC audio/video)\n");
    printf("coturn ChannelData:           +4 bytes    = 1204 bytes (0.3%% overhead)\n");
    printf("vine-relay session-key:\n");
    printf("  AES-GCM envelope:           +64 bytes   = 1264 bytes payload\n");
    printf("  + trellis msg header:       +88 bytes\n");
    printf("  + hybrid sig (Ed25519+ML-DSA): ~+4691 bytes\n");
    printf("  Total per-packet wire size:  ~6043 bytes (403%% overhead)\n");
    printf("  NOTE: use Ed25519-only signing to reduce to ~1416 bytes (18%% overhead)\n");
    printf("vine-relay 3-hop onion:       ~+1664 bytes = 2864 bytes (138%% overhead)\n");
    printf("Adaptive mode (media):        session-key path (see above)\n");
    printf("Adaptive mode (signaling):    3-hop onion path\n");
    printf("---------------------------------\n");
}

int main(void)
{
    if (sodium_init() < 0) {
        fprintf(stderr, "sodium_init() failed\n");
        return 1;
    }

    init_test_packets();

    printf("--- STUN/TURN Benchmarks ---\n");
    bench_stun_is_stun();
    bench_stun_parse_binding();
    bench_stun_parse_response();
    bench_stun_build_response();
    bench_stun_build_response_with_integrity();
    bench_channel_data_detect();
    bench_turn_server_process_binding();
    bench_overhead_summary();

    return 0;
}
