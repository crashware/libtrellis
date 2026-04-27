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
static trellis_identity_t g_member_id;
static trellis_identity_public_t g_member_pub;

static void test_message_serialize_roundtrip(void) {
    uint8_t payload[] = "hello bloom protocol";

    trellis_message_t msg;
    memset(&msg, 0, sizeof(msg));
    msg.version = TRELLIS_PROTOCOL_VERSION;
    msg.type = TRELLIS_MSG_MSG;
    msg.flags = TRELLIS_FLAG_ENCRYPTED;
    msg.sequence = 42;
    msg.timestamp = trellis_now_ms();
    msg.source = g_id.fingerprint;
    memset(&msg.target, 0xBB, sizeof(msg.target));
    msg.payload = payload;
    msg.payload_len = sizeof(payload) - 1;

    trellis_buf_t wire = {0};
    ASSERT(trellis_message_serialize(&msg, &wire) == TRELLIS_OK);
    ASSERT(wire.len > 0);

    trellis_message_t restored;
    memset(&restored, 0, sizeof(restored));
    ASSERT(trellis_message_deserialize(wire.data, wire.len, &restored) == TRELLIS_OK);

    ASSERT(restored.version == msg.version);
    ASSERT(restored.type == msg.type);
    ASSERT(restored.flags == msg.flags);
    ASSERT(restored.sequence == msg.sequence);
    ASSERT(restored.payload_len == msg.payload_len);
    ASSERT(memcmp(restored.payload, msg.payload, msg.payload_len) == 0);
    ASSERT(trellis_fingerprint_eq(&restored.source, &msg.source));
    ASSERT(trellis_fingerprint_eq(&restored.target, &msg.target));

    trellis_buf_free(&wire);
    trellis_message_free(&restored);
}

static void test_message_sign_verify(void) {
    trellis_identity_public_t pub;
    trellis_identity_public_from(&g_id, &pub);

    uint8_t payload[] = "signed message body";

    trellis_message_t msg;
    memset(&msg, 0, sizeof(msg));
    msg.version = TRELLIS_PROTOCOL_VERSION;
    msg.type = TRELLIS_MSG_MSG;
    msg.flags = 0;
    msg.sequence = 1;
    msg.timestamp = trellis_now_ms();
    msg.source = g_id.fingerprint;
    msg.payload = payload;
    msg.payload_len = sizeof(payload) - 1;

    ASSERT(trellis_message_sign(&msg, &g_id) == TRELLIS_OK);
    ASSERT(trellis_message_verify(&msg, &pub) == TRELLIS_OK);

    msg.payload[0] ^= 0xFF;
    ASSERT(trellis_message_verify(&msg, &pub) != TRELLIS_OK);
}

static void test_garden_create(void) {
    struct {
        const char *name;
        trellis_garden_type_t type;
    } cases[] = {
        {"public-garden",     TRELLIS_GARDEN_PUBLIC},
        {"private-garden",    TRELLIS_GARDEN_PRIVATE},
        {"ephemeral-garden",  TRELLIS_GARDEN_EPHEMERAL},
        {"persistent-garden", TRELLIS_GARDEN_PERSISTENT},
    };

    for (size_t i = 0; i < sizeof(cases) / sizeof(cases[0]); i++) {
        trellis_garden_t *g = trellis_garden_create(cases[i].name, cases[i].type, &g_id);
        ASSERT(g != NULL);
        ASSERT(strcmp(trellis_garden_name(g), cases[i].name) == 0);
        ASSERT(trellis_garden_type(g) == cases[i].type);
        ASSERT(trellis_garden_member_count(g) == 1);
        trellis_garden_free(g);
    }
}

static void test_garden_subscribe_unsubscribe(void) {
    trellis_garden_t *g = trellis_garden_create("sub-test", TRELLIS_GARDEN_PUBLIC, &g_id);
    ASSERT(g != NULL);

    ASSERT(!trellis_garden_has_member(g, &g_member_pub.fingerprint));
    ASSERT(trellis_garden_subscribe(g, &g_member_pub.fingerprint) == TRELLIS_OK);
    ASSERT(trellis_garden_has_member(g, &g_member_pub.fingerprint));
    ASSERT(trellis_garden_member_count(g) == 2);

    ASSERT(trellis_garden_unsubscribe(g, &g_member_pub.fingerprint) == TRELLIS_OK);
    ASSERT(!trellis_garden_has_member(g, &g_member_pub.fingerprint));
    ASSERT(trellis_garden_member_count(g) == 1);

    trellis_garden_free(g);
}

static void test_garden_post_message(void) {
    trellis_garden_t *g = trellis_garden_create("msg-test", TRELLIS_GARDEN_PUBLIC, &g_id);
    ASSERT(g != NULL);

    for (int i = 0; i < 5; i++) {
        char buf[64];
        snprintf(buf, sizeof(buf), "garden message %d", i);
        ASSERT(trellis_garden_post_message(g, &g_id.fingerprint,
                                           (const uint8_t *)buf, strlen(buf)) == TRELLIS_OK);
    }

    trellis_garden_free(g);
}

static void test_garden_permissions(void) {
    trellis_garden_t *g = trellis_garden_create("perm-test", TRELLIS_GARDEN_PRIVATE, &g_id);
    ASSERT(g != NULL);

    ASSERT(trellis_garden_subscribe(g, &g_member_pub.fingerprint) == TRELLIS_OK);
    ASSERT(trellis_garden_set_permission(g, &g_member_pub.fingerprint, "write") == TRELLIS_OK);
    ASSERT(trellis_garden_revoke(g, &g_member_pub.fingerprint) == TRELLIS_OK);

    trellis_garden_free(g);
}

static void test_capabilities_negotiate(void) {
    const char *transports_a[] = {"quic", "tcp", "ws"};
    const char *transports_b[] = {"tcp", "udp"};
    const char *features_a[]   = {"morph", "gossip", "onion"};
    const char *features_b[]   = {"morph", "relay"};

    trellis_capabilities_t local = {0};
    local.transports = transports_a;
    local.transports_count = 3;
    local.features = features_a;
    local.features_count = 3;

    trellis_capabilities_t remote = {0};
    remote.transports = transports_b;
    remote.transports_count = 2;
    remote.features = features_b;
    remote.features_count = 2;

    trellis_capabilities_t result = {0};
    ASSERT(trellis_capabilities_negotiate(&local, &remote, &result) == TRELLIS_OK);

    ASSERT(result.transports_count >= 1);
    ASSERT(result.features_count >= 1);

    int found_tcp = 0;
    for (size_t i = 0; i < result.transports_count; i++) {
        if (strcmp(result.transports[i], "tcp") == 0) found_tcp = 1;
    }
    ASSERT(found_tcp);

    int found_morph = 0;
    for (size_t i = 0; i < result.features_count; i++) {
        if (strcmp(result.features[i], "morph") == 0) found_morph = 1;
    }
    ASSERT(found_morph);

    trellis_capabilities_free(&result);
}

static void test_capabilities_add_feature(void) {
    trellis_capabilities_t caps = trellis_capabilities_default();
    size_t before = caps.features_count;

    ASSERT(trellis_capabilities_add_feature(&caps, "exit") == TRELLIS_OK);
    ASSERT(caps.features_count == before + 1);
    ASSERT(trellis_capabilities_has_feature(&caps, "exit"));

    trellis_capabilities_free(&caps);
}

static void test_capabilities_add_feature_dedup(void) {
    trellis_capabilities_t caps = trellis_capabilities_default();

    ASSERT(trellis_capabilities_add_feature(&caps, "exit") == TRELLIS_OK);
    size_t count_after_first = caps.features_count;

    ASSERT(trellis_capabilities_add_feature(&caps, "exit") == TRELLIS_OK);
    ASSERT(caps.features_count == count_after_first);

    trellis_capabilities_free(&caps);
}

static void test_capabilities_add_feature_null(void) {
    trellis_capabilities_t caps = trellis_capabilities_default();

    ASSERT(trellis_capabilities_add_feature(NULL, "exit") == TRELLIS_ERR_INVALID_ARG);
    ASSERT(trellis_capabilities_add_feature(&caps, NULL) == TRELLIS_ERR_INVALID_ARG);

    trellis_capabilities_free(&caps);
}

static void test_capabilities_has_feature_missing(void) {
    trellis_capabilities_t caps = trellis_capabilities_default();

    ASSERT(!trellis_capabilities_has_feature(&caps, "nonexistent"));
    ASSERT(!trellis_capabilities_has_feature(&caps, ""));
    ASSERT(!trellis_capabilities_has_feature(NULL, "exit"));

    trellis_capabilities_free(&caps);
}

static void test_capabilities_has_transport(void) {
    trellis_capabilities_t caps = trellis_capabilities_default();

    ASSERT(trellis_capabilities_has_transport(&caps, "tcp"));
    ASSERT(!trellis_capabilities_has_transport(&caps, "nonexistent-protocol"));
    ASSERT(!trellis_capabilities_has_transport(NULL, "tcp"));

    trellis_capabilities_free(&caps);
}

static void test_capabilities_serialize_roundtrip(void) {
    trellis_capabilities_t caps = trellis_capabilities_default();
    ASSERT(trellis_capabilities_add_feature(&caps, "exit") == TRELLIS_OK);
    ASSERT(trellis_capabilities_add_feature(&caps, "greenhouse") == TRELLIS_OK);

    trellis_buf_t wire = {0};
    ASSERT(trellis_capabilities_serialize(&caps, &wire) == TRELLIS_OK);
    ASSERT(wire.len > 0);

    trellis_capabilities_t restored = {0};
    ASSERT(trellis_capabilities_deserialize(wire.data, wire.len, &restored) == TRELLIS_OK);

    ASSERT(restored.transports_count == caps.transports_count);
    ASSERT(restored.features_count == caps.features_count);

    for (size_t i = 0; i < caps.transports_count; i++)
        ASSERT(trellis_capabilities_has_transport(&restored, caps.transports[i]));

    ASSERT(trellis_capabilities_has_feature(&restored, "exit"));
    ASSERT(trellis_capabilities_has_feature(&restored, "greenhouse"));

    trellis_buf_free(&wire);
    trellis_capabilities_free(&restored);
    trellis_capabilities_free(&caps);
}

#include "mesh/internal.h"

static void test_relay_descriptor_sign_verify(void) {
    trellis_relay_descriptor_t *d = trellis_relay_descriptor_create(
        &g_id.fingerprint, 0x0001, 10000);
    ASSERT(d != NULL);

    trellis_relay_descriptor_set_exit_policy(d, "allow:*:443");
    trellis_relay_descriptor_set_contact(d, "admin@bloom.test");

    ASSERT(trellis_relay_descriptor_sign(d, &g_id) == TRELLIS_OK);
    ASSERT(d->signed_bytes != NULL);
    ASSERT(d->signed_len > 0);

    trellis_identity_public_t pub;
    trellis_identity_public_from(&g_id, &pub);

    trellis_relay_descriptor_t *verified = NULL;
    ASSERT(trellis_relay_descriptor_verify(
        d->signed_bytes, d->signed_len, &pub, &verified) == TRELLIS_OK);
    ASSERT(verified != NULL);
    ASSERT(trellis_fingerprint_eq(&verified->fingerprint, &g_id.fingerprint));
    ASSERT(verified->bandwidth_kbps == 10000);

    trellis_relay_descriptor_free(verified);
    trellis_relay_descriptor_free(d);
}

static void test_relay_descriptor_tampered_rejected(void) {
    trellis_relay_descriptor_t *d = trellis_relay_descriptor_create(
        &g_id.fingerprint, 0x0001, 5000);
    ASSERT(d != NULL);

    ASSERT(trellis_relay_descriptor_sign(d, &g_id) == TRELLIS_OK);
    ASSERT(d->signed_bytes != NULL);

    // Tamper with a byte in the signed body (before the signature)
    d->signed_bytes[8] ^= 0xFF;

    trellis_identity_public_t pub;
    trellis_identity_public_from(&g_id, &pub);

    trellis_relay_descriptor_t *out = NULL;
    trellis_err_t err = trellis_relay_descriptor_verify(
        d->signed_bytes, d->signed_len, &pub, &out);
    ASSERT(err != TRELLIS_OK);
    ASSERT(out == NULL);

    trellis_relay_descriptor_free(d);
}

static int ext_dispatch_count = 0;

static void test_ext_handler(const trellis_message_t *msg, void *ctx) {
    (void)msg;
    (void)ctx;
    ext_dispatch_count++;
}

static void test_extensions_register(void) {
    trellis_extensions_t *ext = trellis_extensions_new();
    ASSERT(ext != NULL);

    ASSERT(trellis_extensions_register(ext, 0x8000, "custom-ext",
                                       test_ext_handler, NULL) == TRELLIS_OK);

    trellis_message_t msg;
    memset(&msg, 0, sizeof(msg));
    msg.type = 0x8000;

    ext_dispatch_count = 0;
    ASSERT(trellis_extensions_dispatch(ext, &msg) == TRELLIS_OK);
    ASSERT(ext_dispatch_count == 1);

    trellis_extensions_free(ext);
}

static void test_extensions_reject_core(void) {
    trellis_extensions_t *ext = trellis_extensions_new();
    ASSERT(ext != NULL);

    trellis_err_t err = trellis_extensions_register(ext, 0x0010, "bad-ext",
                                                    test_ext_handler, NULL);
    ASSERT(err != TRELLIS_OK);

    err = trellis_extensions_register(ext, 0x7FFF, "also-bad",
                                      test_ext_handler, NULL);
    ASSERT(err != TRELLIS_OK);

    trellis_extensions_free(ext);
}

int main(void) {
    printf("=== Test Suite: Protocol ===\n");

    ASSERT(trellis_crypto_init() == TRELLIS_OK);
    ASSERT(trellis_identity_generate(&g_id) == TRELLIS_OK);
    ASSERT(trellis_identity_generate(&g_member_id) == TRELLIS_OK);
    trellis_identity_public_from(&g_member_id, &g_member_pub);

    RUN_TEST(test_message_serialize_roundtrip);
    RUN_TEST(test_message_sign_verify);
    RUN_TEST(test_garden_create);
    RUN_TEST(test_garden_subscribe_unsubscribe);
    RUN_TEST(test_garden_post_message);
    RUN_TEST(test_garden_permissions);
    RUN_TEST(test_capabilities_negotiate);
    RUN_TEST(test_capabilities_add_feature);
    RUN_TEST(test_capabilities_add_feature_dedup);
    RUN_TEST(test_capabilities_add_feature_null);
    RUN_TEST(test_capabilities_has_feature_missing);
    RUN_TEST(test_capabilities_has_transport);
    RUN_TEST(test_capabilities_serialize_roundtrip);
    RUN_TEST(test_relay_descriptor_sign_verify);
    RUN_TEST(test_relay_descriptor_tampered_rejected);
    RUN_TEST(test_extensions_register);
    RUN_TEST(test_extensions_reject_core);

    trellis_identity_destroy(&g_member_id);
    trellis_identity_destroy(&g_id);
    trellis_crypto_cleanup();

    printf("=== All protocol tests passed ===\n");
    return 0;
}
