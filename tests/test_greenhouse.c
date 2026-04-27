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

static trellis_identity_t g_id;

static void test_bloom_addr_roundtrip(void) {
    trellis_identity_public_t pub;
    trellis_identity_public_from(&g_id, &pub);

    trellis_bloom_addr_t addr;
    ASSERT(trellis_bloom_addr_from_fingerprint(&pub.fingerprint, &addr) == TRELLIS_OK);
    ASSERT(strlen(addr.uri) > 0);
    ASSERT(strncmp(addr.uri, TRELLIS_BLOOM_ADDR_PREFIX, strlen(TRELLIS_BLOOM_ADDR_PREFIX)) == 0);

    trellis_bloom_addr_t parsed;
    ASSERT(trellis_bloom_addr_parse(addr.uri, &parsed) == TRELLIS_OK);
    ASSERT(trellis_fingerprint_eq(&addr.fingerprint, &parsed.fingerprint));
}

static void test_bloom_addr_descriptor_key(void) {
    trellis_identity_public_t pub;
    trellis_identity_public_from(&g_id, &pub);

    uint8_t key1[32], key2[32];
    ASSERT(trellis_bloom_addr_descriptor_key(&pub.fingerprint, key1) == TRELLIS_OK);
    ASSERT(trellis_bloom_addr_descriptor_key(&pub.fingerprint, key2) == TRELLIS_OK);
    ASSERT(memcmp(key1, key2, 32) == 0);
}

static void test_descriptor_sign_verify(void) {
    trellis_identity_public_t pub;
    trellis_identity_public_from(&g_id, &pub);

    trellis_service_descriptor_t desc;
    memset(&desc, 0, sizeof(desc));
    desc.service_identity = pub;
    desc.protocol_version = TRELLIS_PROTOCOL_VERSION;
    desc.created_at = trellis_now_ms();
    desc.expires_at = desc.created_at + 3600000;
    desc.intro_points = NULL;
    desc.intro_point_count = 0;

    ASSERT(trellis_descriptor_sign(&desc, &g_id) == TRELLIS_OK);
    ASSERT(trellis_descriptor_verify(&desc) == TRELLIS_OK);

    trellis_descriptor_free(&desc);
}

static void test_descriptor_serialize_roundtrip(void) {
    trellis_identity_public_t pub;
    trellis_identity_public_from(&g_id, &pub);

    trellis_service_descriptor_t desc;
    memset(&desc, 0, sizeof(desc));
    desc.service_identity = pub;
    desc.protocol_version = TRELLIS_PROTOCOL_VERSION;
    desc.created_at = trellis_now_ms();
    desc.expires_at = desc.created_at + 7200000;
    desc.intro_points = NULL;
    desc.intro_point_count = 0;

    ASSERT(trellis_descriptor_sign(&desc, &g_id) == TRELLIS_OK);

    trellis_buf_t wire = {0};
    ASSERT(trellis_descriptor_serialize(&desc, &wire) == TRELLIS_OK);
    ASSERT(wire.len > 0);

    trellis_service_descriptor_t restored;
    memset(&restored, 0, sizeof(restored));
    ASSERT(trellis_descriptor_deserialize(wire.data, wire.len, &restored) == TRELLIS_OK);

    ASSERT(restored.protocol_version == desc.protocol_version);
    ASSERT(restored.created_at == desc.created_at);
    ASSERT(restored.expires_at == desc.expires_at);
    ASSERT(trellis_fingerprint_eq(&restored.service_identity.fingerprint,
                                  &desc.service_identity.fingerprint));

    trellis_buf_free(&wire);
    trellis_descriptor_free(&desc);
    trellis_descriptor_free(&restored);
}

static void test_descriptor_expiry(void) {
    trellis_identity_public_t pub;
    trellis_identity_public_from(&g_id, &pub);

    trellis_service_descriptor_t desc;
    memset(&desc, 0, sizeof(desc));
    desc.service_identity = pub;
    desc.protocol_version = TRELLIS_PROTOCOL_VERSION;
    desc.created_at = 1000;
    desc.expires_at = 2000;

    ASSERT(trellis_descriptor_is_expired(&desc) == true);

    trellis_descriptor_free(&desc);
}

static void test_onion_wrap_peel(void) {
    trellis_kem_keypair_t hops[3];
    trellis_fingerprint_t relay_fps[3];

    for (int i = 0; i < 3; i++) {
        ASSERT(trellis_kem_keygen(&hops[i]) == TRELLIS_OK);
        memset(&relay_fps[i], 0x10 + i, sizeof(trellis_fingerprint_t));
    }

    trellis_onion_circuit_t *circuit = trellis_onion_new(3);
    ASSERT(circuit != NULL);

    for (int i = 0; i < 3; i++) {
        ASSERT(trellis_onion_add_hop(circuit, &relay_fps[i], hops[i].x25519_pk, hops[i].ml_kem_pk) == TRELLIS_OK);
    }

    const uint8_t payload[] = "secret onion payload at the core";
    trellis_buf_t wrapped = {0};
    ASSERT(trellis_onion_wrap(circuit, payload, sizeof(payload) - 1, &wrapped) == TRELLIS_OK);
    ASSERT(wrapped.len > 0);

    trellis_buf_t current = trellis_buf_dup(wrapped);
    for (int i = 0; i < 3; i++) {
        trellis_buf_t peeled = {0};
        trellis_fingerprint_t next = {0};
        ASSERT(trellis_onion_peel(&hops[i], current.data, current.len,
                                  &peeled, &next) == TRELLIS_OK);
        trellis_buf_free(&current);
        current = peeled;
    }

    ASSERT(current.len == sizeof(payload) - 1);
    ASSERT(memcmp(current.data, payload, current.len) == 0);

    trellis_buf_free(&current);
    trellis_buf_free(&wrapped);
    trellis_onion_free(circuit);
}

static void test_rendezvous_flow(void) {
    trellis_rendezvous_t *rv1 = trellis_rendezvous_new();
    trellis_rendezvous_t *rv2 = trellis_rendezvous_new();
    ASSERT(rv1 != NULL && rv2 != NULL);

    uint8_t cookie[TRELLIS_RENDEZVOUS_COOKIE_LEN];
    ASSERT(trellis_rendezvous_establish(rv1, cookie) == TRELLIS_OK);
    ASSERT(trellis_rendezvous_join(rv2, cookie) == TRELLIS_OK);
    ASSERT(trellis_rendezvous_bridge(rv1) == TRELLIS_OK);

    trellis_rendezvous_free(rv1);
    trellis_rendezvous_free(rv2);
}

int main(void) {
    printf("=== Test Suite: Greenhouse ===\n");

    ASSERT(trellis_crypto_init() == TRELLIS_OK);
    ASSERT(trellis_identity_generate(&g_id) == TRELLIS_OK);

    RUN_TEST(test_bloom_addr_roundtrip);
    RUN_TEST(test_bloom_addr_descriptor_key);
    RUN_TEST(test_descriptor_sign_verify);
    RUN_TEST(test_descriptor_serialize_roundtrip);
    RUN_TEST(test_descriptor_expiry);
    RUN_TEST(test_onion_wrap_peel);
    RUN_TEST(test_rendezvous_flow);

    trellis_identity_destroy(&g_id);
    trellis_crypto_cleanup();

    printf("=== All greenhouse tests passed ===\n");
    return 0;
}
