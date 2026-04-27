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
 * bench_mesh.c — Performance benchmarks for Trellis mesh / DHT / onion layers.
 *
 * All operations use in-memory mocks (no network I/O).
 *
 * Outputs JSON lines to stdout:
 *   {"op": "...", "iterations": N, "total_ms": X, "per_op_us": Y}
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <uv.h>

#include "trellis/crypto.h"
#include "trellis/identity.h"
#include "trellis/mesh.h"
#include "trellis/greenhouse.h"

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

static void noop_send(void *client, const trellis_fingerprint_t *target,
                       const uint8_t *data, size_t len)
{
    (void)client; (void)target; (void)data; (void)len;
}

static void bench_dht_store(void)
{
    trellis_identity_t id;
    trellis_identity_generate(&id);

    uv_loop_t *loop = uv_loop_new();
    trellis_dht_t *dht = trellis_dht_new(&id, loop);
    trellis_dht_set_send(dht, noop_send, NULL);

    uint8_t key[32], value[256];
    memset(value, 0xCC, sizeof(value));

    BENCH_BEGIN(1000)
        memset(key, (uint8_t)_i, sizeof(key));
        key[0] = (uint8_t)(_i & 0xFF);
        key[1] = (uint8_t)((_i >> 8) & 0xFF);
        trellis_dht_store(dht, key, sizeof(key), value, sizeof(value),
                          NULL, NULL);
    BENCH_END("dht_store_local");

    trellis_dht_free(dht);
    uv_loop_delete(loop);
}

static void bench_dht_lookup(void)
{
    trellis_identity_t id;
    trellis_identity_generate(&id);

    uv_loop_t *loop = uv_loop_new();
    trellis_dht_t *dht = trellis_dht_new(&id, loop);
    trellis_dht_set_send(dht, noop_send, NULL);

    // Populate some synthetic peers so the routing table is non-trivial.
    for (int i = 0; i < 50; i++) {
        trellis_identity_t peer_id;
        trellis_identity_generate(&peer_id);

        trellis_peer_info_t peer;
        memset(&peer, 0, sizeof(peer));
        trellis_identity_public_t pub;
        trellis_identity_public_from(&peer_id, &pub);
        peer.identity = pub;
        peer.fingerprint = pub.fingerprint;
        snprintf(peer.addr, sizeof(peer.addr), "127.0.0.%d:9000", i + 1);
        peer.last_seen  = trellis_now_ms();
        peer.first_seen = peer.last_seen;
        trellis_dht_add_peer(dht, &peer);
    }

    trellis_fingerprint_t target;
    memset(&target, 0x42, sizeof(target));

    BENCH_BEGIN(1000)
        trellis_peer_info_t closest[TRELLIS_DHT_K];
        trellis_dht_find_closest(dht, &target, closest, TRELLIS_DHT_K);
    BENCH_END("dht_find_closest");

    trellis_dht_free(dht);
    uv_loop_delete(loop);
}

static void bench_onion_build(void)
{
    trellis_kem_keypair_t hops[3];
    trellis_fingerprint_t fps[3];
    for (int i = 0; i < 3; i++) {
        trellis_kem_keygen(&hops[i]);
        memset(&fps[i], (uint8_t)(i + 1), sizeof(fps[i]));
    }

    uint8_t payload[256];
    memset(payload, 0xDE, sizeof(payload));

    BENCH_BEGIN(100)
        trellis_onion_circuit_t *circuit = trellis_onion_new(3);
        for (int h = 0; h < 3; h++) {
            trellis_onion_add_hop(circuit, &fps[h],
                                  hops[h].x25519_pk, hops[h].ml_kem_pk);
        }
        trellis_buf_t wrapped = {0};
        trellis_onion_wrap(circuit, payload, sizeof(payload), &wrapped);
        trellis_buf_free(&wrapped);
        trellis_onion_free(circuit);
    BENCH_END("onion_circuit_build_3hop");
}

static void bench_dht_add_peer_subnet(void)
{
    trellis_identity_t id;
    trellis_identity_generate(&id);

    uv_loop_t *loop = uv_loop_new();
    trellis_dht_t *dht = trellis_dht_new(&id, loop);
    trellis_dht_set_send(dht, noop_send, NULL);

    BENCH_BEGIN(1000)
        trellis_identity_t peer_id;
        trellis_identity_generate(&peer_id);

        trellis_peer_info_t peer;
        memset(&peer, 0, sizeof(peer));
        trellis_identity_public_t pub;
        trellis_identity_public_from(&peer_id, &pub);
        peer.identity = pub;
        peer.fingerprint = pub.fingerprint;
        // Same /16 prefix for all -- measures rejection overhead.
        snprintf(peer.addr, sizeof(peer.addr), "10.1.%d.%d:9000",
                 (_i / 256) % 256, _i % 256);
        peer.last_seen  = trellis_now_ms();
        peer.first_seen = peer.last_seen;
        trellis_dht_add_peer(dht, &peer);
    BENCH_END("dht_add_peer_same_subnet");

    trellis_dht_free(dht);
    uv_loop_delete(loop);
}

static void bench_dht_eclipse_check(void)
{
    trellis_identity_t id;
    trellis_identity_generate(&id);

    uv_loop_t *loop = uv_loop_new();
    trellis_dht_t *dht = trellis_dht_new(&id, loop);
    trellis_dht_set_send(dht, noop_send, NULL);

    // Populate diverse peers to form a sibling table.
    for (int i = 0; i < 40; i++) {
        trellis_identity_t peer_id;
        trellis_identity_generate(&peer_id);

        trellis_peer_info_t peer;
        memset(&peer, 0, sizeof(peer));
        trellis_identity_public_t pub;
        trellis_identity_public_from(&peer_id, &pub);
        peer.identity = pub;
        peer.fingerprint = pub.fingerprint;
        snprintf(peer.addr, sizeof(peer.addr), "%d.%d.1.1:9000",
                 (i * 7) % 256, (i * 13) % 256);
        peer.last_seen  = trellis_now_ms();
        peer.first_seen = peer.last_seen;
        trellis_dht_add_peer(dht, &peer);
    }

    BENCH_BEGIN(10000)
        trellis_dht_check_eclipse(dht);
    BENCH_END("dht_eclipse_check");

    trellis_dht_free(dht);
    uv_loop_delete(loop);
}

int main(void)
{
    if (trellis_crypto_init() != TRELLIS_OK) {
        fprintf(stderr, "bench_mesh: crypto init failed\n");
        return 1;
    }

    bench_dht_store();
    bench_dht_lookup();
    bench_onion_build();
    bench_dht_add_peer_subnet();
    bench_dht_eclipse_check();

    trellis_crypto_cleanup();
    return 0;
}
