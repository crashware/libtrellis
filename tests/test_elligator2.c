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
#include <string.h>
#include <stdlib.h>
#include <sodium.h>
#include <trellis/crypto.h>

#define ASSERT(cond) do { \
    if (!(cond)) { \
        fprintf(stderr, "FAIL at %s:%d: %s\n", __FILE__, __LINE__, #cond); \
        abort(); \
    } \
} while (0)

static void test_encode_decode_roundtrip(void)
{
    int successes = 0;
    for (int i = 0; i < 100; i++) {
        uint8_t sk[32], pk[32], repr[32];
        randombytes_buf(sk, 32);
        sk[0]  &= 248;
        sk[31] &= 127;
        sk[31] |= 64;
        ASSERT(crypto_scalarmult_curve25519_base(pk, sk) == 0);

        if (trellis_elligator2_encode(pk, repr) != TRELLIS_OK)
            continue;

        successes++;

        uint8_t pk_back[32];
        ASSERT(trellis_elligator2_decode(repr, pk_back) == TRELLIS_OK);
        ASSERT(memcmp(pk, pk_back, 32) == 0);
    }
    ASSERT(successes > 0);
}

static void test_keygen_and_dh(void)
{
    uint8_t sk_a[32], pk_a[32], repr_a[32];
    uint8_t sk_b[32], pk_b[32], repr_b[32];

    ASSERT(trellis_elligator2_keygen(sk_a, pk_a, repr_a) == TRELLIS_OK);
    ASSERT(trellis_elligator2_keygen(sk_b, pk_b, repr_b) == TRELLIS_OK);

    uint8_t repr_a_clean[32], repr_b_clean[32];
    memcpy(repr_a_clean, repr_a, 32);
    repr_a_clean[31] &= 0x7F;
    memcpy(repr_b_clean, repr_b, 32);
    repr_b_clean[31] &= 0x7F;

    uint8_t pk_a_recv[32], pk_b_recv[32];
    ASSERT(trellis_elligator2_decode(repr_a_clean, pk_a_recv) == TRELLIS_OK);
    ASSERT(trellis_elligator2_decode(repr_b_clean, pk_b_recv) == TRELLIS_OK);

    ASSERT(memcmp(pk_a, pk_a_recv, 32) == 0);
    ASSERT(memcmp(pk_b, pk_b_recv, 32) == 0);

    uint8_t dh_a[32], dh_b[32];
    ASSERT(crypto_scalarmult_curve25519(dh_a, sk_a, pk_b_recv) == 0);
    ASSERT(crypto_scalarmult_curve25519(dh_b, sk_b, pk_a_recv) == 0);
    ASSERT(memcmp(dh_a, dh_b, 32) == 0);
}

int main(void)
{
    ASSERT(sodium_init() >= 0);
    test_encode_decode_roundtrip();
    test_keygen_and_dh();
    return 0;
}
