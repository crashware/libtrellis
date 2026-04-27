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

#define RUN_TEST(fn) do { printf("  %-50s", #fn "..."); fn(); printf("PASS\n"); } while(0)
#define ASSERT(cond) do { if (!(cond)) { fprintf(stderr, "FAIL at %s:%d: %s\n", __FILE__, __LINE__, #cond); abort(); } } while(0)

static trellis_identity_t g_alice;
static trellis_identity_t g_bob;

static void do_handshake(trellis_identity_t *a_id, trellis_identity_t *b_id,
                         trellis_handshake_t **a_out, trellis_handshake_t **b_out) {
    *a_out = trellis_handshake_new(a_id, TRELLIS_ROLE_INITIATOR);
    *b_out = trellis_handshake_new(b_id, TRELLIS_ROLE_RESPONDER);
    trellis_buf_t m1 = {0}, m2 = {0}, m3 = {0};
    trellis_handshake_create_msg1(*a_out, &m1);
    trellis_handshake_process_msg1(*b_out, m1.data, m1.len, &m2);
    trellis_handshake_process_msg2(*a_out, m2.data, m2.len, &m3);
    trellis_handshake_process_msg3(*b_out, m3.data, m3.len);
    trellis_buf_free(&m1);
    trellis_buf_free(&m2);
    trellis_buf_free(&m3);
}

static void test_full_handshake_and_chat(void) {
    trellis_handshake_t *hs_a, *hs_b;
    do_handshake(&g_alice, &g_bob, &hs_a, &hs_b);
    ASSERT(trellis_handshake_state(hs_a) == TRELLIS_HS_COMPLETE);
    ASSERT(trellis_handshake_state(hs_b) == TRELLIS_HS_COMPLETE);

    trellis_ratchet_t *ra = trellis_ratchet_init_from_handshake(hs_a);
    trellis_ratchet_t *rb = trellis_ratchet_init_from_handshake(hs_b);
    ASSERT(ra != NULL && rb != NULL);

    const char *alice_msgs[] = {"Hey Bob!", "How are you?", "See you later."};
    const char *bob_msgs[]   = {"Hi Alice!", "Doing great!", "Bye!"};

    for (int i = 0; i < 3; i++) {
        trellis_buf_t ct = {0}, pt = {0};

        ASSERT(trellis_ratchet_encrypt(ra, (const uint8_t *)alice_msgs[i],
                                       strlen(alice_msgs[i]), &ct) == TRELLIS_OK);
        ASSERT(trellis_ratchet_decrypt(rb, ct.data, ct.len, &pt) == TRELLIS_OK);
        ASSERT(pt.len == strlen(alice_msgs[i]));
        ASSERT(memcmp(pt.data, alice_msgs[i], pt.len) == 0);
        trellis_buf_free(&ct);
        trellis_buf_free(&pt);

        ASSERT(trellis_ratchet_encrypt(rb, (const uint8_t *)bob_msgs[i],
                                       strlen(bob_msgs[i]), &ct) == TRELLIS_OK);
        ASSERT(trellis_ratchet_decrypt(ra, ct.data, ct.len, &pt) == TRELLIS_OK);
        ASSERT(pt.len == strlen(bob_msgs[i]));
        ASSERT(memcmp(pt.data, bob_msgs[i], pt.len) == 0);
        trellis_buf_free(&ct);
        trellis_buf_free(&pt);
    }

    trellis_ratchet_stats_t sa = trellis_ratchet_stats(ra);
    trellis_ratchet_stats_t sb = trellis_ratchet_stats(rb);
    ASSERT(sa.messages_sent == 3 && sa.messages_received == 3);
    ASSERT(sb.messages_sent == 3 && sb.messages_received == 3);

    trellis_ratchet_free(ra);
    trellis_ratchet_free(rb);
    trellis_handshake_free(hs_a);
    trellis_handshake_free(hs_b);
}

static void test_garden_lifecycle(void) {
    trellis_identity_public_t member_pub;
    trellis_identity_public_from(&g_bob, &member_pub);

    trellis_garden_t *g = trellis_garden_create("integration-garden",
                                                 TRELLIS_GARDEN_PRIVATE, &g_alice);
    ASSERT(g != NULL);
    ASSERT(trellis_garden_member_count(g) == 1);

    ASSERT(trellis_garden_subscribe(g, &member_pub.fingerprint) == TRELLIS_OK);
    ASSERT(trellis_garden_member_count(g) == 2);
    ASSERT(trellis_garden_has_member(g, &member_pub.fingerprint));

    for (int i = 0; i < 3; i++) {
        char buf[64];
        snprintf(buf, sizeof(buf), "lifecycle message %d", i);
        ASSERT(trellis_garden_post_message(g, &g_alice.fingerprint,
                                           (const uint8_t *)buf, strlen(buf)) == TRELLIS_OK);
    }

    ASSERT(trellis_garden_unsubscribe(g, &member_pub.fingerprint) == TRELLIS_OK);
    ASSERT(trellis_garden_member_count(g) == 1);
    ASSERT(!trellis_garden_has_member(g, &member_pub.fingerprint));

    trellis_garden_free(g);
}

static void test_morph_encode_pipeline(void) {
    trellis_morph_config_t cfg = trellis_morph_config_default();
    cfg.metamorphic = true;
    trellis_morph_t *morph = trellis_morph_new(&cfg);
    ASSERT(morph != NULL);

    const uint8_t key[] = "test-morph-session-key-32bytes!!";
    ASSERT(trellis_morph_set_session_key(morph, key, sizeof(key) - 1) == TRELLIS_OK);

    const uint8_t payload[] = "integration morph pipeline data";
    size_t pt_len = sizeof(payload) - 1;

    trellis_buf_t encoded = {0};
    ASSERT(trellis_morph_encode(morph, payload, pt_len, &encoded) == TRELLIS_OK);
    ASSERT(encoded.len > 0);
    ASSERT(encoded.len != pt_len || memcmp(encoded.data, payload, pt_len) != 0);

    trellis_buf_t decoded = {0};
    ASSERT(trellis_morph_decode(morph, encoded.data, encoded.len, &decoded) == TRELLIS_OK);
    ASSERT(decoded.len == pt_len);
    ASSERT(memcmp(decoded.data, payload, pt_len) == 0);

    trellis_buf_free(&encoded);
    trellis_buf_free(&decoded);
    trellis_morph_free(morph);
}

int main(void) {
    printf("=== Test Suite: Integration ===\n");

    ASSERT(trellis_crypto_init() == TRELLIS_OK);
    ASSERT(trellis_identity_generate(&g_alice) == TRELLIS_OK);
    ASSERT(trellis_identity_generate(&g_bob) == TRELLIS_OK);

    RUN_TEST(test_full_handshake_and_chat);
    RUN_TEST(test_garden_lifecycle);
    RUN_TEST(test_morph_encode_pipeline);

    trellis_identity_destroy(&g_alice);
    trellis_identity_destroy(&g_bob);
    trellis_crypto_cleanup();

    printf("=== All integration tests passed ===\n");
    return 0;
}
