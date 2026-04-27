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
 * bench_crypto.c — Performance benchmarks for Trellis cryptographic primitives.
 *
 * Outputs JSON lines to stdout:
 *   {"op": "...", "iterations": N, "total_ms": X, "per_op_us": Y}
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "trellis/crypto.h"
#include "trellis/identity.h"

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

static void bench_kem_keygen(void)
{
    trellis_kem_keypair_t kp;
    BENCH_BEGIN(100)
        trellis_kem_keygen(&kp);
    BENCH_END("kem_keygen");
}

static void bench_kem_encaps(void)
{
    trellis_kem_keypair_t kp;
    trellis_kem_encaps_result_t res;
    trellis_kem_keygen(&kp);

    BENCH_BEGIN(1000)
        trellis_kem_encaps(kp.x25519_pk, kp.ml_kem_pk, &res);
    BENCH_END("kem_encaps");
}

static void bench_kem_decaps(void)
{
    trellis_kem_keypair_t kp;
    trellis_kem_encaps_result_t enc;
    uint8_t ss[TRELLIS_ML_KEM_1024_SS_LEN];
    trellis_kem_keygen(&kp);
    trellis_kem_encaps(kp.x25519_pk, kp.ml_kem_pk, &enc);

    BENCH_BEGIN(1000)
        trellis_kem_decaps(&kp, enc.x25519_pk, enc.ml_kem_ct, ss);
    BENCH_END("kem_decaps");
}

static void bench_sign(void)
{
    trellis_identity_t id;
    trellis_identity_generate(&id);

    uint8_t msg[64];
    memset(msg, 0xAB, sizeof(msg));
    trellis_signature_t sig;

    BENCH_BEGIN(100)
        trellis_identity_sign(&id, msg, sizeof(msg), &sig);
    BENCH_END("ml_dsa_87_sign");
}

static void bench_verify(void)
{
    trellis_identity_t id;
    trellis_identity_generate(&id);

    trellis_identity_public_t pub;
    trellis_identity_public_from(&id, &pub);

    uint8_t msg[64];
    memset(msg, 0xAB, sizeof(msg));
    trellis_signature_t sig;
    trellis_identity_sign(&id, msg, sizeof(msg), &sig);

    BENCH_BEGIN(100)
        trellis_identity_verify(&pub, msg, sizeof(msg), &sig);
    BENCH_END("ml_dsa_87_verify");
}

static void bench_hkdf(void)
{
    uint8_t ikm[32], salt[32], info[16], okm[32];
    memset(ikm,  0x01, sizeof(ikm));
    memset(salt, 0x02, sizeof(salt));
    memset(info, 0x03, sizeof(info));

    BENCH_BEGIN(1000)
        trellis_hkdf_shake256(ikm, sizeof(ikm), salt, sizeof(salt),
                              info, sizeof(info), okm, sizeof(okm));
    BENCH_END("hkdf_shake256");
}

static void bench_handshake(void)
{
    trellis_identity_t id_a, id_b;
    trellis_identity_generate(&id_a);
    trellis_identity_generate(&id_b);

    BENCH_BEGIN(100)
        trellis_handshake_t *hs_a = trellis_handshake_new(&id_a, TRELLIS_ROLE_INITIATOR);
        trellis_handshake_t *hs_b = trellis_handshake_new(&id_b, TRELLIS_ROLE_RESPONDER);

        trellis_buf_t msg1 = {0}, msg2 = {0}, msg3 = {0};

        trellis_handshake_create_msg1(hs_a, &msg1);
        trellis_handshake_process_msg1(hs_b, msg1.data, msg1.len, &msg2);
        trellis_handshake_process_msg2(hs_a, msg2.data, msg2.len, &msg3);
        trellis_handshake_process_msg3(hs_b, msg3.data, msg3.len);

        trellis_buf_free(&msg1);
        trellis_buf_free(&msg2);
        trellis_buf_free(&msg3);
        trellis_handshake_free(hs_a);
        trellis_handshake_free(hs_b);
    BENCH_END("handshake_3msg");
}

static void bench_ratchet_step(void)
{
    trellis_identity_t id_a, id_b;
    trellis_identity_generate(&id_a);
    trellis_identity_generate(&id_b);

    trellis_handshake_t *hs_a = trellis_handshake_new(&id_a, TRELLIS_ROLE_INITIATOR);
    trellis_handshake_t *hs_b = trellis_handshake_new(&id_b, TRELLIS_ROLE_RESPONDER);

    trellis_buf_t m1 = {0}, m2 = {0}, m3 = {0};
    trellis_handshake_create_msg1(hs_a, &m1);
    trellis_handshake_process_msg1(hs_b, m1.data, m1.len, &m2);
    trellis_handshake_process_msg2(hs_a, m2.data, m2.len, &m3);
    trellis_handshake_process_msg3(hs_b, m3.data, m3.len);
    trellis_buf_free(&m1);
    trellis_buf_free(&m2);
    trellis_buf_free(&m3);

    trellis_ratchet_t *r_send = trellis_ratchet_init_from_handshake(hs_a);
    trellis_ratchet_t *r_recv = trellis_ratchet_init_from_handshake(hs_b);

    uint8_t pt[] = "benchmark ratchet payload data";

    BENCH_BEGIN(1000)
        trellis_buf_t ct = {0};
        trellis_ratchet_encrypt(r_send, pt, sizeof(pt), &ct);

        trellis_buf_t dec = {0};
        trellis_ratchet_decrypt(r_recv, ct.data, ct.len, &dec);

        trellis_buf_free(&ct);
        trellis_buf_free(&dec);
    BENCH_END("ratchet_step");

    trellis_ratchet_free(r_send);
    trellis_ratchet_free(r_recv);
    trellis_handshake_free(hs_a);
    trellis_handshake_free(hs_b);
}

int main(void)
{
    if (trellis_crypto_init() != TRELLIS_OK) {
        fprintf(stderr, "bench_crypto: crypto init failed\n");
        return 1;
    }

    bench_kem_keygen();
    bench_kem_encaps();
    bench_kem_decaps();
    bench_sign();
    bench_verify();
    bench_hkdf();
    bench_handshake();
    bench_ratchet_step();

    trellis_crypto_cleanup();
    return 0;
}
