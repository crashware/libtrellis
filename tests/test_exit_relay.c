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
 * test_exit_relay.c — Unit tests for the clearnet exit relay subsystem
 *
 * Tests exit policy evaluation, proof-of-work verification, and SOCKS5
 * listener binding in isolation (no full client or network required).
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <uv.h>
#include <trellis/trellis.h>
#include <sodium.h>

#define RUN_TEST(fn) do { printf("  %-50s", #fn "..."); fn(); printf("PASS\n"); } while(0)
#define ASSERT(cond) do { if (!(cond)) { fprintf(stderr, "FAIL at %s:%d: %s\n", __FILE__, __LINE__, #cond); abort(); } } while(0)

static uv_loop_t *g_loop;

static void test_exit_relay_create_free(void)
{
    trellis_exit_relay_t *er = trellis_exit_relay_new(g_loop);
    ASSERT(er != NULL);
    trellis_exit_relay_free(er);
}

static void test_exit_policy_default_deny(void)
{
    trellis_exit_relay_t *er = trellis_exit_relay_new(g_loop);
    ASSERT(er != NULL);

    ASSERT(!trellis_exit_relay_policy_allows(er, "example.com", 443));
    ASSERT(!trellis_exit_relay_policy_allows(er, "evil.org", 80));
    ASSERT(!trellis_exit_relay_policy_allows(er, "localhost", 22));

    trellis_exit_relay_free(er);
}

static void test_exit_policy_allow_rule(void)
{
    trellis_exit_relay_t *er = trellis_exit_relay_new(g_loop);
    ASSERT(er != NULL);

    ASSERT(trellis_exit_relay_add_policy(er, "allow:*.example.com:443") == TRELLIS_OK);

    ASSERT(trellis_exit_relay_policy_allows(er, "foo.example.com", 443));
    ASSERT(trellis_exit_relay_policy_allows(er, "bar.example.com", 443));
    ASSERT(!trellis_exit_relay_policy_allows(er, "evil.com", 443));
    ASSERT(!trellis_exit_relay_policy_allows(er, "foo.example.com", 80));

    trellis_exit_relay_free(er);
}

static void test_exit_policy_reject_rule(void)
{
    trellis_exit_relay_t *er = trellis_exit_relay_new(g_loop);
    ASSERT(er != NULL);

    // First-match-wins: reject port 25 before the allow-all catches it.
    ASSERT(trellis_exit_relay_add_policy(er, "reject:*:25") == TRELLIS_OK);
    ASSERT(trellis_exit_relay_add_policy(er, "allow:*:*") == TRELLIS_OK);

    ASSERT(!trellis_exit_relay_policy_allows(er, "spammer.net", 25));
    ASSERT(trellis_exit_relay_policy_allows(er, "example.com", 443));
    ASSERT(trellis_exit_relay_policy_allows(er, "example.com", 80));

    trellis_exit_relay_free(er);
}

static void test_exit_policy_wildcard_port(void)
{
    trellis_exit_relay_t *er = trellis_exit_relay_new(g_loop);
    ASSERT(er != NULL);

    ASSERT(trellis_exit_relay_add_policy(er, "allow:example.com:*") == TRELLIS_OK);

    ASSERT(trellis_exit_relay_policy_allows(er, "example.com", 80));
    ASSERT(trellis_exit_relay_policy_allows(er, "example.com", 443));
    ASSERT(trellis_exit_relay_policy_allows(er, "example.com", 22));
    ASSERT(!trellis_exit_relay_policy_allows(er, "other.com", 80));

    trellis_exit_relay_free(er);
}

static void generate_pow_token(uint8_t *token_out, size_t token_len,
                               uint32_t difficulty)
{
    ASSERT(token_len >= 32);
    uint8_t digest[32];
    uint64_t nonce = 0;

    for (;;) {
        memset(token_out, 0, token_len);
        memcpy(token_out, &nonce, sizeof(nonce));
        crypto_hash_sha256(digest, token_out, token_len);

        // Check leading zero bits.
        uint32_t zeros = 0;
        for (uint32_t i = 0; i < difficulty; i++) {
            uint32_t byte_idx = i / 8;
            uint32_t bit_idx  = 7 - (i % 8);
            if ((digest[byte_idx] >> bit_idx) & 1)
                break;
            zeros++;
        }
        if (zeros >= difficulty)
            return;
        nonce++;
    }
}

static void test_exit_pow_verify_valid(void)
{
    trellis_exit_relay_t *er = trellis_exit_relay_new(g_loop);
    ASSERT(er != NULL);

    uint8_t token[32];
    generate_pow_token(token, sizeof(token), TRELLIS_EXIT_POW_DIFFICULTY);

    ASSERT(trellis_exit_relay_verify_pow(er, token, sizeof(token)));

    trellis_exit_relay_free(er);
}

static void test_exit_pow_verify_invalid(void)
{
    trellis_exit_relay_t *er = trellis_exit_relay_new(g_loop);
    ASSERT(er != NULL);

    uint8_t fake_token[32];
    memset(fake_token, 0xFF, sizeof(fake_token));

    ASSERT(!trellis_exit_relay_verify_pow(er, fake_token, sizeof(fake_token)));

    // Token too short.
    uint8_t short_token[4] = {0};
    ASSERT(!trellis_exit_relay_verify_pow(er, short_token, sizeof(short_token)));

    trellis_exit_relay_free(er);
}

static void test_exit_socks5_bind(void)
{
    trellis_exit_relay_t *er = trellis_exit_relay_new(g_loop);
    ASSERT(er != NULL);

    // Port must be 1-65535; use a high ephemeral port to avoid conflicts.
    trellis_err_t err = trellis_exit_relay_start_socks5(er, 19199);
    ASSERT(err == TRELLIS_OK);

    trellis_exit_relay_free(er);
}

int main(void)
{
    printf("=== Test Suite: Exit Relay ===\n");

    ASSERT(sodium_init() >= 0);
    ASSERT(trellis_crypto_init() == TRELLIS_OK);

    g_loop = uv_default_loop();
    ASSERT(g_loop != NULL);

    RUN_TEST(test_exit_relay_create_free);
    RUN_TEST(test_exit_policy_default_deny);
    RUN_TEST(test_exit_policy_allow_rule);
    RUN_TEST(test_exit_policy_reject_rule);
    RUN_TEST(test_exit_policy_wildcard_port);
    RUN_TEST(test_exit_pow_verify_valid);
    RUN_TEST(test_exit_pow_verify_invalid);
    RUN_TEST(test_exit_socks5_bind);

    uv_loop_close(g_loop);
    trellis_crypto_cleanup();

    printf("=== All exit relay tests passed ===\n");
    return 0;
}
