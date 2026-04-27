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
 * test_rhizome.c — Multi-grove integration tests for Rhizome
 *
 * Tests grove creation, membership, DHT namespace isolation, bridge
 * peering, canopy routing, cross-grove Greenhouse resolution, bridge
 * trust scoring, and cross-grove gossip relay.
 */

#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <uv.h>
#include <trellis/trellis.h>
#include <trellis/rhizome.h>
#include <trellis/greenhouse.h>

#define RUN_TEST(fn) do { printf("  %-60s", #fn "..."); fn(); printf("PASS\n"); } while(0)
#define ASSERT(cond) do { if (!(cond)) { fprintf(stderr, "FAIL at %s:%d: %s\n", __FILE__, __LINE__, #cond); abort(); } } while(0)

static trellis_identity_t g_id_alpha;
static trellis_identity_t g_id_beta;
static trellis_identity_t g_id_bridge;
static trellis_identity_t g_id_member;
static uv_loop_t *g_loop;

static void drain_loop(void) {
    while (uv_loop_alive(g_loop))
        uv_run(g_loop, UV_RUN_ONCE);
    uv_run(g_loop, UV_RUN_NOWAIT);
}

// --- Grove Identity Tests ---
static void test_grove_create(void) {
    trellis_grove_policy_t policy = trellis_grove_policy_default();
    trellis_grove_t *grove = trellis_grove_create("Test Grove Alpha",
                                                   &g_id_alpha, &policy);
    ASSERT(grove != NULL);

    const trellis_grove_id_t *gid = trellis_grove_id(grove);
    ASSERT(gid != NULL);

    // grove_id should be non-zero.
    int all_zero = 1;
    for (int i = 0; i < TRELLIS_GROVE_ID_LEN; i++) {
        if (gid->bytes[i] != 0) { all_zero = 0; break; }
    }
    ASSERT(!all_zero);

    // Creator should be an admin and member.
    ASSERT(trellis_grove_is_admin(grove, &g_id_alpha.fingerprint));
    ASSERT(trellis_grove_is_member(grove, &g_id_alpha.fingerprint));

    trellis_grove_free(grove);
}

static void test_grove_genesis_verify(void) {
    trellis_grove_policy_t policy = trellis_grove_policy_default();
    trellis_grove_t *grove = trellis_grove_create("Verified Grove",
                                                   &g_id_alpha, &policy);
    ASSERT(grove != NULL);

    const trellis_grove_genesis_t *gen = trellis_grove_genesis(grove);
    ASSERT(gen != NULL);

    // Genesis should verify successfully.
    ASSERT(trellis_grove_genesis_verify(gen) == TRELLIS_OK);

    // Tampered genesis should fail.
    trellis_grove_genesis_t tampered = *gen;
    tampered.name[0] ^= 0xFF;
    ASSERT(trellis_grove_genesis_verify(&tampered) != TRELLIS_OK);

    trellis_grove_free(grove);
}

static void test_grove_genesis_serialize_roundtrip(void) {
    trellis_grove_policy_t policy = trellis_grove_policy_default();
    trellis_grove_t *grove = trellis_grove_create("Serializable Grove",
                                                   &g_id_alpha, &policy);
    ASSERT(grove != NULL);

    const trellis_grove_genesis_t *gen = trellis_grove_genesis(grove);
    trellis_buf_t buf;
    ASSERT(trellis_grove_genesis_serialize(gen, &buf) == TRELLIS_OK);
    ASSERT(buf.len > 0);

    trellis_grove_genesis_t decoded;
    ASSERT(trellis_grove_genesis_deserialize(buf.data, buf.len, &decoded) == TRELLIS_OK);
    ASSERT(trellis_grove_id_eq(&decoded.grove_id, &gen->grove_id));
    ASSERT(strcmp(decoded.name, gen->name) == 0);

    trellis_buf_free(&buf);
    trellis_grove_free(grove);
}

static void test_grove_id_short(void) {
    trellis_grove_policy_t policy = trellis_grove_policy_default();
    trellis_grove_t *grove = trellis_grove_create("Short ID Grove",
                                                   &g_id_alpha, &policy);
    ASSERT(grove != NULL);

    const trellis_grove_id_t *gid = trellis_grove_id(grove);
    char short_id[17];
    ASSERT(trellis_grove_id_short(gid, short_id, sizeof(short_id)) == TRELLIS_OK);
    ASSERT(strlen(short_id) == 16);

    // Short id should be lowercase alphanumeric (base32)
    for (size_t i = 0; i < strlen(short_id); i++) {
        char c = short_id[i];
        ASSERT((c >= 'a' && c <= 'z') || (c >= '2' && c <= '7'));
    }

    trellis_grove_free(grove);
}

static void test_grove_id_eq(void) {
    trellis_grove_id_t a, b;
    memset(&a, 0xAA, sizeof(a));
    memset(&b, 0xAA, sizeof(b));
    ASSERT(trellis_grove_id_eq(&a, &b));

    b.bytes[0] ^= 1;
    ASSERT(!trellis_grove_id_eq(&a, &b));
}

// --- Membership Tests ---
static void test_grove_membership_issue(void) {
    trellis_grove_policy_t policy = trellis_grove_policy_default();
    trellis_grove_t *grove = trellis_grove_create("Membership Grove",
                                                   &g_id_alpha, &policy);
    ASSERT(grove != NULL);

    trellis_grove_membership_t mem;
    ASSERT(trellis_grove_issue_membership(grove, &g_id_alpha,
                                          &g_id_member.fingerprint,
                                          false, 0, &mem) == TRELLIS_OK);

    // Verify the attestation.
    trellis_identity_public_t admin_pub;
    trellis_identity_public_from(&g_id_alpha, &admin_pub);
    ASSERT(trellis_grove_membership_verify(&mem, &admin_pub) == TRELLIS_OK);

    // Add member to grove.
    ASSERT(trellis_grove_add_member(grove, &mem, &admin_pub) == TRELLIS_OK);
    ASSERT(trellis_grove_is_member(grove, &g_id_member.fingerprint));

    trellis_grove_free(grove);
}

static void test_grove_membership_serialize_roundtrip(void) {
    trellis_grove_policy_t policy = trellis_grove_policy_default();
    trellis_grove_t *grove = trellis_grove_create("Serialize Membership",
                                                   &g_id_alpha, &policy);
    ASSERT(grove != NULL);

    trellis_grove_membership_t mem;
    ASSERT(trellis_grove_issue_membership(grove, &g_id_alpha,
                                          &g_id_member.fingerprint,
                                          true, 0, &mem) == TRELLIS_OK);

    trellis_buf_t buf;
    ASSERT(trellis_grove_membership_serialize(&mem, &buf) == TRELLIS_OK);

    trellis_grove_membership_t decoded;
    ASSERT(trellis_grove_membership_deserialize(buf.data, buf.len, &decoded) == TRELLIS_OK);

    ASSERT(trellis_grove_id_eq(&decoded.grove_id, &mem.grove_id));
    ASSERT(trellis_fingerprint_eq(&decoded.member_fp, &mem.member_fp));
    ASSERT(decoded.is_bridge == true);

    trellis_buf_free(&buf);
    trellis_grove_free(grove);
}

static void test_grove_non_admin_cannot_issue(void) {
    trellis_grove_policy_t policy = trellis_grove_policy_default();
    trellis_grove_t *grove = trellis_grove_create("Admin Test",
                                                   &g_id_alpha, &policy);
    ASSERT(grove != NULL);

    // g_id_beta is not an admin.
    trellis_grove_membership_t mem;
    ASSERT(trellis_grove_issue_membership(grove, &g_id_beta,
                                          &g_id_member.fingerprint,
                                          false, 0, &mem) == TRELLIS_ERR_PERMISSION);

    trellis_grove_free(grove);
}

static void test_grove_add_admin(void) {
    trellis_grove_policy_t policy = trellis_grove_policy_default();
    trellis_grove_t *grove = trellis_grove_create("Admin Grove",
                                                   &g_id_alpha, &policy);
    ASSERT(grove != NULL);

    ASSERT(!trellis_grove_is_admin(grove, &g_id_beta.fingerprint));
    ASSERT(trellis_grove_add_admin(grove, &g_id_beta.fingerprint) == TRELLIS_OK);
    ASSERT(trellis_grove_is_admin(grove, &g_id_beta.fingerprint));

    // Duplicate add.
    ASSERT(trellis_grove_add_admin(grove, &g_id_beta.fingerprint) == TRELLIS_ERR_ALREADY_EXISTS);

    trellis_grove_free(grove);
}

// --- DHT Namespace Isolation Tests ---
static void test_dht_grove_namespace_isolation(void) {
    trellis_dht_t *dht_alpha = trellis_dht_new(&g_id_alpha, g_loop);
    trellis_dht_t *dht_beta = trellis_dht_new(&g_id_beta, g_loop);
    ASSERT(dht_alpha != NULL);
    ASSERT(dht_beta != NULL);

    // Create two different groves.
    trellis_grove_policy_t policy = trellis_grove_policy_default();
    trellis_grove_t *grove_alpha = trellis_grove_create("Alpha",
                                                         &g_id_alpha, &policy);
    trellis_grove_t *grove_beta = trellis_grove_create("Beta",
                                                        &g_id_beta, &policy);
    ASSERT(grove_alpha != NULL);
    ASSERT(grove_beta != NULL);

    // Bind each DHT to its grove.
    ASSERT(trellis_dht_set_grove(dht_alpha, trellis_grove_id(grove_alpha)) == TRELLIS_OK);
    ASSERT(trellis_dht_set_grove(dht_beta, trellis_grove_id(grove_beta)) == TRELLIS_OK);

    // Verify grove bindings.
    ASSERT(trellis_dht_grove(dht_alpha) != NULL);
    ASSERT(trellis_dht_grove(dht_beta) != NULL);
    ASSERT(!trellis_grove_id_eq(trellis_dht_grove(dht_alpha),
                                trellis_dht_grove(dht_beta)));

    // Scoped keys should differ for the same raw key.
    const uint8_t raw_key[] = "shared-key";
    uint8_t scoped_alpha[32], scoped_beta[32];
    trellis_grove_scope_key(trellis_grove_id(grove_alpha),
                            raw_key, sizeof(raw_key) - 1, scoped_alpha);
    trellis_grove_scope_key(trellis_grove_id(grove_beta),
                            raw_key, sizeof(raw_key) - 1, scoped_beta);

    ASSERT(memcmp(scoped_alpha, scoped_beta, 32) != 0);

    // NULL grove_id should pass through unchanged
    uint8_t unscoped[32];
    memset(unscoped, 0, 32);
    trellis_grove_scope_key(NULL, raw_key, sizeof(raw_key) - 1, unscoped);
    ASSERT(memcmp(unscoped, raw_key, sizeof(raw_key) - 1) == 0);

    // Unbind grove from dht.
    ASSERT(trellis_dht_set_grove(dht_alpha, NULL) == TRELLIS_OK);
    ASSERT(trellis_dht_grove(dht_alpha) == NULL);

    trellis_grove_free(grove_alpha);
    trellis_grove_free(grove_beta);
    trellis_dht_free(dht_alpha);
    trellis_dht_free(dht_beta);
    drain_loop();
}

// --- Grove Policy Tests ---
static void test_grove_peering_policy_open(void) {
    trellis_grove_policy_t policy = trellis_grove_policy_default();
    policy.peer_mode = TRELLIS_GROVE_PEER_OPEN;

    trellis_grove_t *grove = trellis_grove_create("Open Grove",
                                                   &g_id_alpha, &policy);
    ASSERT(grove != NULL);

    trellis_grove_id_t random_grove;
    memset(&random_grove, 0x42, sizeof(random_grove));
    ASSERT(trellis_grove_peering_allowed(grove, &random_grove));

    trellis_grove_free(grove);
}

static void test_grove_peering_policy_whitelist(void) {
    trellis_grove_policy_t policy = trellis_grove_policy_default();
    policy.peer_mode = TRELLIS_GROVE_PEER_WHITELIST;

    trellis_grove_id_t allowed_grove;
    memset(&allowed_grove, 0x11, sizeof(allowed_grove));
    policy.peer_list[0] = allowed_grove;
    policy.peer_list_count = 1;

    trellis_grove_t *grove = trellis_grove_create("Whitelist Grove",
                                                   &g_id_alpha, &policy);
    ASSERT(grove != NULL);

    ASSERT(trellis_grove_peering_allowed(grove, &allowed_grove));

    trellis_grove_id_t blocked_grove;
    memset(&blocked_grove, 0x22, sizeof(blocked_grove));
    ASSERT(!trellis_grove_peering_allowed(grove, &blocked_grove));

    trellis_grove_free(grove);
}

static void test_grove_peering_policy_blacklist(void) {
    trellis_grove_policy_t policy = trellis_grove_policy_default();
    policy.peer_mode = TRELLIS_GROVE_PEER_BLACKLIST;

    trellis_grove_id_t blocked_grove;
    memset(&blocked_grove, 0x33, sizeof(blocked_grove));
    policy.peer_list[0] = blocked_grove;
    policy.peer_list_count = 1;

    trellis_grove_t *grove = trellis_grove_create("Blacklist Grove",
                                                   &g_id_alpha, &policy);
    ASSERT(grove != NULL);

    ASSERT(!trellis_grove_peering_allowed(grove, &blocked_grove));

    trellis_grove_id_t allowed_grove;
    memset(&allowed_grove, 0x44, sizeof(allowed_grove));
    ASSERT(trellis_grove_peering_allowed(grove, &allowed_grove));

    trellis_grove_free(grove);
}

// --- Bridge Node Tests ---
static void test_bridge_create(void) {
    trellis_dht_t *home_dht = trellis_dht_new(&g_id_bridge, g_loop);
    ASSERT(home_dht != NULL);

    trellis_router_t *router = trellis_router_new(home_dht, NULL);
    ASSERT(router != NULL);

    trellis_bridge_t *bridge = trellis_bridge_new(home_dht, router,
                                                   &g_id_bridge, g_loop);
    ASSERT(bridge != NULL);
    ASSERT(trellis_bridge_grove_count(bridge) == 1);

    trellis_bridge_free(bridge);
    trellis_router_free(router);
    trellis_dht_free(home_dht);
    drain_loop();
}

static void test_bridge_grove_count(void) {
    trellis_dht_t *home_dht = trellis_dht_new(&g_id_bridge, g_loop);
    trellis_router_t *router = trellis_router_new(home_dht, NULL);

    trellis_bridge_t *bridge = trellis_bridge_new(home_dht, router,
                                                   &g_id_bridge, g_loop);
    ASSERT(bridge != NULL);
    ASSERT(trellis_bridge_grove_count(bridge) == 1);

    // Slot 0 should return a DHT.
    ASSERT(trellis_bridge_grove_dht(bridge, 0) == home_dht);

    // Invalid index should return NULL.
    ASSERT(trellis_bridge_grove_dht(bridge, 99) == NULL);

    trellis_bridge_free(bridge);
    trellis_router_free(router);
    trellis_dht_free(home_dht);
    drain_loop();
}

// --- Bridge Attestation Tests ---
static void test_bridge_attestation_serialize_roundtrip(void) {
    trellis_bridge_attestation_t att;
    memset(&att, 0, sizeof(att));

    memset(att.grove_a.bytes, 0xAA, 32);
    memset(att.grove_b.bytes, 0xBB, 32);
    att.fp_in_a = g_id_alpha.fingerprint;
    att.fp_in_b = g_id_beta.fingerprint;
    att.issued_at = trellis_now_ms();
    att.expires_at = 0;

    // Sign with both identities.
    uint8_t presig[4 + 1 + 32 + 32 + 32 + 32 + 8 + 8];
    size_t off = 0;
    memcpy(presig + off, "BATT", 4); off += 4;
    presig[off++] = 1;
    memcpy(presig + off, att.grove_a.bytes, 32); off += 32;
    memcpy(presig + off, att.grove_b.bytes, 32); off += 32;
    memcpy(presig + off, att.fp_in_a.bytes, 32); off += 32;
    memcpy(presig + off, att.fp_in_b.bytes, 32); off += 32;
    for (int i = 0; i < 8; i++) presig[off++] = (uint8_t)(att.issued_at >> ((7 - i) * 8));
    for (int i = 0; i < 8; i++) presig[off++] = (uint8_t)(att.expires_at >> ((7 - i) * 8));

    ASSERT(trellis_identity_sign(&g_id_alpha, presig, off, &att.sig_a) == TRELLIS_OK);
    ASSERT(trellis_identity_sign(&g_id_beta, presig, off, &att.sig_b) == TRELLIS_OK);

    trellis_buf_t buf;
    ASSERT(trellis_bridge_attestation_serialize(&att, &buf) == TRELLIS_OK);

    trellis_bridge_attestation_t decoded;
    ASSERT(trellis_bridge_attestation_deserialize(buf.data, buf.len, &decoded) == TRELLIS_OK);
    ASSERT(trellis_grove_id_eq(&decoded.grove_a, &att.grove_a));
    ASSERT(trellis_grove_id_eq(&decoded.grove_b, &att.grove_b));
    ASSERT(trellis_fingerprint_eq(&decoded.fp_in_a, &att.fp_in_a));
    ASSERT(trellis_fingerprint_eq(&decoded.fp_in_b, &att.fp_in_b));

    // Verify the attestation.
    trellis_identity_public_t pub_a, pub_b;
    trellis_identity_public_from(&g_id_alpha, &pub_a);
    trellis_identity_public_from(&g_id_beta, &pub_b);
    ASSERT(trellis_bridge_attestation_verify(&decoded, &pub_a, &pub_b) == TRELLIS_OK);

    trellis_buf_free(&buf);
}

// --- Canopy Routing Tests ---
static void test_canopy_create(void) {
    trellis_dht_t *dht = trellis_dht_new(&g_id_bridge, g_loop);
    trellis_router_t *router = trellis_router_new(dht, NULL);
    trellis_bridge_t *bridge = trellis_bridge_new(dht, router,
                                                   &g_id_bridge, g_loop);
    ASSERT(bridge != NULL);

    trellis_canopy_t *canopy = trellis_canopy_new(bridge, g_loop);
    ASSERT(canopy != NULL);
    ASSERT(trellis_canopy_route_count(canopy) == 0);

    trellis_canopy_free(canopy);
    trellis_bridge_free(bridge);
    trellis_router_free(router);
    trellis_dht_free(dht);
    drain_loop();
}

static void test_canopy_local_grove_reachable(void) {
    trellis_dht_t *dht = trellis_dht_new(&g_id_bridge, g_loop);
    trellis_grove_id_t home_grove;
    memset(&home_grove, 0xAB, sizeof(home_grove));
    trellis_dht_set_grove(dht, &home_grove);

    trellis_router_t *router = trellis_router_new(dht, NULL);
    trellis_bridge_t *bridge = trellis_bridge_new(dht, router,
                                                   &g_id_bridge, g_loop);
    trellis_canopy_t *canopy = trellis_canopy_new(bridge, g_loop);

    // Our own grove should always be reachable.
    ASSERT(trellis_canopy_grove_reachable(canopy, &home_grove));

    // Unknown grove should not be reachable.
    trellis_grove_id_t unknown;
    memset(&unknown, 0xCD, sizeof(unknown));
    ASSERT(!trellis_canopy_grove_reachable(canopy, &unknown));

    trellis_canopy_free(canopy);
    trellis_bridge_free(bridge);
    trellis_router_free(router);
    trellis_dht_free(dht);
    drain_loop();
}

static void test_canopy_route_direct_grove(void) {
    trellis_dht_t *dht = trellis_dht_new(&g_id_bridge, g_loop);
    trellis_grove_id_t home_grove;
    memset(&home_grove, 0xAB, sizeof(home_grove));
    trellis_dht_set_grove(dht, &home_grove);

    trellis_router_t *router = trellis_router_new(dht, NULL);
    trellis_bridge_t *bridge = trellis_bridge_new(dht, router,
                                                   &g_id_bridge, g_loop);
    trellis_canopy_t *canopy = trellis_canopy_new(bridge, g_loop);

    // Routing to own grove should return direct path (len=0)
    trellis_canopy_route_t route;
    ASSERT(trellis_canopy_route(canopy, &home_grove, &route) == TRELLIS_OK);
    ASSERT(route.path_len == 0);
    ASSERT(route.score == 1.0);

    trellis_canopy_free(canopy);
    trellis_bridge_free(bridge);
    trellis_router_free(router);
    trellis_dht_free(dht);
    drain_loop();
}

static void test_bloom_addr_cross_grove_roundtrip(void) {
    trellis_grove_id_t grove;
    memset(&grove, 0x42, sizeof(grove));

    trellis_bloom_addr_t addr;
    ASSERT(trellis_bloom_addr_from_fingerprint_grove(
        &g_id_alpha.fingerprint, &grove, &addr) == TRELLIS_OK);
    ASSERT(addr.has_grove);
    ASSERT(memcmp(addr.grove_id_short, grove.bytes, 10) == 0);

    // URI should contain '@'.
    ASSERT(strchr(addr.uri, '@') != NULL);

    // Parse it back.
    trellis_bloom_addr_t parsed;
    ASSERT(trellis_bloom_addr_parse(addr.uri, &parsed) == TRELLIS_OK);
    ASSERT(parsed.has_grove);
    ASSERT(memcmp(parsed.grove_id_short, grove.bytes, 10) == 0);
    ASSERT(trellis_fingerprint_eq(&parsed.fingerprint, &g_id_alpha.fingerprint));
}

static void test_bloom_addr_no_grove_backward_compat(void) {
    trellis_bloom_addr_t addr;
    ASSERT(trellis_bloom_addr_from_fingerprint(&g_id_alpha.fingerprint,
                                               &addr) == TRELLIS_OK);
    ASSERT(!addr.has_grove);
    ASSERT(strchr(addr.uri, '@') == NULL);

    trellis_bloom_addr_t parsed;
    ASSERT(trellis_bloom_addr_parse(addr.uri, &parsed) == TRELLIS_OK);
    ASSERT(!parsed.has_grove);
    ASSERT(trellis_fingerprint_eq(&parsed.fingerprint, &g_id_alpha.fingerprint));
}

// --- Bridge Trust Tests ---
static void test_bridge_trust_scoring(void) {
    trellis_bridge_trust_t *bt = trellis_bridge_trust_new(NULL);
    ASSERT(bt != NULL);

    // Initial score should be 0.5 (BRIDGE_TRUST_INITIAL)
    double initial = trellis_bridge_trust_score(bt, &g_id_bridge.fingerprint);
    ASSERT(initial >= 0.49 && initial <= 0.51);

    // Record successes - score should increase.
    for (int i = 0; i < 10; i++)
        trellis_bridge_trust_record_success(bt, &g_id_bridge.fingerprint);

    double after_success = trellis_bridge_trust_score(bt, &g_id_bridge.fingerprint);
    ASSERT(after_success > initial);

    // Record failures - score should decrease.
    for (int i = 0; i < 20; i++)
        trellis_bridge_trust_record_failure(bt, &g_id_bridge.fingerprint);

    double after_failure = trellis_bridge_trust_score(bt, &g_id_bridge.fingerprint);
    ASSERT(after_failure < after_success);

    // Score should be bounded [0, 1]
    ASSERT(after_failure >= 0.0);
    ASSERT(after_failure <= 1.0);

    trellis_bridge_trust_free(bt);
}

static void test_bridge_trust_register_attestation(void) {
    trellis_bridge_trust_t *bt = trellis_bridge_trust_new(NULL);
    ASSERT(bt != NULL);

    trellis_bridge_attestation_t att;
    memset(&att, 0, sizeof(att));
    memset(att.grove_a.bytes, 0xAA, 32);
    memset(att.grove_b.bytes, 0xBB, 32);
    att.fp_in_a = g_id_alpha.fingerprint;
    att.fp_in_b = g_id_beta.fingerprint;
    att.issued_at = trellis_now_ms();
    att.expires_at = 0;

    // Build presig for signing.
    uint8_t presig[4 + 1 + 32 + 32 + 32 + 32 + 8 + 8];
    size_t off = 0;
    memcpy(presig + off, "BATT", 4); off += 4;
    presig[off++] = 1;
    memcpy(presig + off, att.grove_a.bytes, 32); off += 32;
    memcpy(presig + off, att.grove_b.bytes, 32); off += 32;
    memcpy(presig + off, att.fp_in_a.bytes, 32); off += 32;
    memcpy(presig + off, att.fp_in_b.bytes, 32); off += 32;
    for (int i = 0; i < 8; i++) presig[off++] = (uint8_t)(att.issued_at >> ((7 - i) * 8));
    for (int i = 0; i < 8; i++) presig[off++] = (uint8_t)(att.expires_at >> ((7 - i) * 8));

    ASSERT(trellis_identity_sign(&g_id_alpha, presig, off, &att.sig_a) == TRELLIS_OK);
    ASSERT(trellis_identity_sign(&g_id_beta, presig, off, &att.sig_b) == TRELLIS_OK);

    trellis_identity_public_t pub_a, pub_b;
    trellis_identity_public_from(&g_id_alpha, &pub_a);
    trellis_identity_public_from(&g_id_beta, &pub_b);

    ASSERT(trellis_bridge_trust_register(bt, &att, &pub_a, &pub_b) == TRELLIS_OK);

    // After attestation, score should be at least 0.6 (baseline boost)
    double score = trellis_bridge_trust_score(bt, &att.fp_in_a);
    ASSERT(score >= 0.59);

    trellis_bridge_trust_free(bt);
}

static void test_gossip_grove_scope(void) {
    trellis_dht_t *dht = trellis_dht_new(&g_id_alpha, g_loop);
    trellis_gossip_t *gossip = trellis_gossip_new(dht, &g_id_alpha);
    ASSERT(gossip != NULL);

    // No scope by default.
    ASSERT(trellis_gossip_grove_scope(gossip) == NULL);

    // Set scope.
    trellis_grove_id_t grove;
    memset(&grove, 0x55, sizeof(grove));
    trellis_gossip_set_grove_scope(gossip, &grove);
    ASSERT(trellis_gossip_grove_scope(gossip) != NULL);
    ASSERT(trellis_grove_id_eq(trellis_gossip_grove_scope(gossip), &grove));

    // Clear scope.
    trellis_gossip_set_grove_scope(gossip, NULL);
    ASSERT(trellis_gossip_grove_scope(gossip) == NULL);

    trellis_gossip_free(gossip);
    trellis_dht_free(dht);
    drain_loop();
}

static void test_canopy_message_types_exist(void) {
    ASSERT(TRELLIS_MSG_CANOPY_ANNOUNCE == 0x9001);
    ASSERT(TRELLIS_MSG_CANOPY_WITHDRAW == 0x9002);
    ASSERT(TRELLIS_MSG_CANOPY_CIRCUIT_CREATE == 0x9003);
    ASSERT(TRELLIS_MSG_CANOPY_CIRCUIT_DATA == 0x9004);
    ASSERT(TRELLIS_MSG_CANOPY_CIRCUIT_DESTROY == 0x9005);
}

static void test_rhizome_error_codes(void) {
    ASSERT(TRELLIS_ERR_GROVE == -800);
    ASSERT(TRELLIS_ERR_GROVE_UNKNOWN == -801);
    ASSERT(TRELLIS_ERR_GROVE_POLICY == -802);
    ASSERT(TRELLIS_ERR_BRIDGE == -803);
    ASSERT(TRELLIS_ERR_CANOPY == -804);
    ASSERT(TRELLIS_ERR_CANOPY_LOOP == -805);
    ASSERT(TRELLIS_ERR_CANOPY_EXPIRED == -806);
}

// --- Test Runner ---
int main(void) {
    printf("Initializing crypto...\n");
    trellis_crypto_init();

    printf("Generating test identities...\n");
    ASSERT(trellis_identity_generate(&g_id_alpha) == TRELLIS_OK);
    ASSERT(trellis_identity_generate(&g_id_beta) == TRELLIS_OK);
    ASSERT(trellis_identity_generate(&g_id_bridge) == TRELLIS_OK);
    ASSERT(trellis_identity_generate(&g_id_member) == TRELLIS_OK);

    g_loop = uv_default_loop();

    printf("\n── Rhizome Integration Tests ───────────────────────────────────\n\n");

    printf("Grove Identity:\n");
    RUN_TEST(test_grove_create);
    RUN_TEST(test_grove_genesis_verify);
    RUN_TEST(test_grove_genesis_serialize_roundtrip);
    RUN_TEST(test_grove_id_short);
    RUN_TEST(test_grove_id_eq);

    printf("\nMembership:\n");
    RUN_TEST(test_grove_membership_issue);
    RUN_TEST(test_grove_membership_serialize_roundtrip);
    RUN_TEST(test_grove_non_admin_cannot_issue);
    RUN_TEST(test_grove_add_admin);

    printf("\nDHT Namespace Isolation:\n");
    RUN_TEST(test_dht_grove_namespace_isolation);

    printf("\nGrove Policy:\n");
    RUN_TEST(test_grove_peering_policy_open);
    RUN_TEST(test_grove_peering_policy_whitelist);
    RUN_TEST(test_grove_peering_policy_blacklist);

    printf("\nBridge Node:\n");
    RUN_TEST(test_bridge_create);
    RUN_TEST(test_bridge_grove_count);

    printf("\nBridge Attestation:\n");
    RUN_TEST(test_bridge_attestation_serialize_roundtrip);

    printf("\nCanopy Routing:\n");
    RUN_TEST(test_canopy_create);
    RUN_TEST(test_canopy_local_grove_reachable);
    RUN_TEST(test_canopy_route_direct_grove);

    printf("\nCross-Grove Greenhouse Addressing:\n");
    RUN_TEST(test_bloom_addr_cross_grove_roundtrip);
    RUN_TEST(test_bloom_addr_no_grove_backward_compat);

    printf("\nBridge Trust:\n");
    RUN_TEST(test_bridge_trust_scoring);
    RUN_TEST(test_bridge_trust_register_attestation);

    printf("\nCross-Grove Gossip:\n");
    RUN_TEST(test_gossip_grove_scope);

    printf("\nProtocol Constants:\n");
    RUN_TEST(test_canopy_message_types_exist);
    RUN_TEST(test_rhizome_error_codes);

    printf("\n── All Rhizome tests passed! ───────────────────────────────────\n\n");

    trellis_identity_destroy(&g_id_alpha);
    trellis_identity_destroy(&g_id_beta);
    trellis_identity_destroy(&g_id_bridge);
    trellis_identity_destroy(&g_id_member);

    uv_loop_close(g_loop);
    trellis_crypto_cleanup();

    return 0;
}
