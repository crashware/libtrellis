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
#include <trellis/trellis.h>
#include "../src/platform.h"

#define RUN_TEST(fn) do { printf("  %-50s", #fn "..."); fn(); printf("PASS\n"); } while(0)
#define ASSERT(cond) do { if (!(cond)) { fprintf(stderr, "FAIL at %s:%d: %s\n", __FILE__, __LINE__, #cond); abort(); } } while(0)

static volatile int g_dm_received = 0;
static char g_dm_payload[256] = {0};
static volatile int g_handshakes[5] = {0};

static void on_message(const trellis_event_data_t *ev, void *ctx)
{
    (void)ctx;
    if (ev->event == TRELLIS_EV_MESSAGE && ev->data && ev->data_len > 0) {
        size_t copy = ev->data_len < sizeof(g_dm_payload) - 1
                      ? ev->data_len : sizeof(g_dm_payload) - 1;
        memcpy(g_dm_payload, ev->data, copy);
        g_dm_payload[copy] = '\0';
        g_dm_received = 1;
    }
}

static void on_handshake(const trellis_event_data_t *ev, void *ctx)
{
    int idx = *(int *)ctx;
    if (ev->event == TRELLIS_EV_HANDSHAKE_COMPLETE)
        g_handshakes[idx]++;
}

static void msleep(int ms)
{
    trellis_sleep_ms((unsigned int)ms);
}

static int wait_for(volatile int *flag, int target, int timeout_ms)
{
    for (int elapsed = 0; elapsed < timeout_ms; elapsed += 50) {
        if (*flag >= target)
            return 1;
        msleep(50);
    }
    return *flag >= target;
}

static void test_onion_routing_5_nodes(void)
{
    /*
     * 5-node fully meshed topology.  node0 is the sender (onion
     * mode), node4 is the destination.  Nodes 1-3 are available as
     * relay hops, giving route_onion() the 3 eligible peers it needs.
     */
    #define N 5
    static int indices[N] = {0, 1, 2, 3, 4};
    trellis_client_t *nodes[N] = {0};
    char listen_addrs[N][32];
    char id_paths[N][128];

    for (int i = 0; i < N; i++) {
        snprintf(id_paths[i], sizeof(id_paths[i]),
                 "/tmp/test_onion_node%d.key", i);
        unlink(id_paths[i]);
        memset(listen_addrs[i], 0, sizeof(listen_addrs[i]));
    }

    // Node 1 is the seed.
    trellis_client_config_t cfg_seed = trellis_client_config_default();
    cfg_seed.listen_addr = "127.0.0.1:0";
    cfg_seed.disable_discovery = true;
    cfg_seed.identity_path = id_paths[1];
    cfg_seed.conn_pow_iterations = UINT32_MAX;
    cfg_seed.decoy_circuit_interval_ms = 0;
    nodes[1] = trellis_client_new(&cfg_seed);
    ASSERT(nodes[1] != NULL);
    trellis_client_on(nodes[1], TRELLIS_EV_HANDSHAKE_COMPLETE, on_handshake, &indices[1]);
    ASSERT(trellis_client_start(nodes[1]) == TRELLIS_OK);
    trellis_dht_set_eclipse_enabled(trellis_client_get_dht(nodes[1]), false);
    trellis_dht_disable_sybil(trellis_client_get_dht(nodes[1]));
    snprintf(listen_addrs[1], sizeof(listen_addrs[1]), "%s",
             trellis_client_listen_addr(nodes[1]));
    fprintf(stderr, "[TEST] node1 (seed) listening on %s\n", listen_addrs[1]);

    // Start remaining nodes, all seeded to node1.
    for (int i = 0; i < N; i++) {
        if (i == 1) continue;
        trellis_client_config_t cfg = trellis_client_config_default();
        cfg.listen_addr = "127.0.0.1:0";
        cfg.seed_addr = listen_addrs[1];
        cfg.disable_discovery = true;
        cfg.identity_path = id_paths[i];
        cfg.conn_pow_iterations = UINT32_MAX;
        cfg.decoy_circuit_interval_ms = 0;
        nodes[i] = trellis_client_new(&cfg);
        ASSERT(nodes[i] != NULL);
        trellis_client_on(nodes[i], TRELLIS_EV_HANDSHAKE_COMPLETE, on_handshake, &indices[i]);
        if (i == 4)
            trellis_client_on(nodes[i], TRELLIS_EV_MESSAGE, on_message, &indices[i]);
        ASSERT(trellis_client_start(nodes[i]) == TRELLIS_OK);
        trellis_dht_set_eclipse_enabled(trellis_client_get_dht(nodes[i]), false);
        trellis_dht_disable_sybil(trellis_client_get_dht(nodes[i]));
        const char *a = trellis_client_listen_addr(nodes[i]);
        if (a) snprintf(listen_addrs[i], sizeof(listen_addrs[i]), "%s", a);
        fprintf(stderr, "[TEST] node%d listening on %s\n", i, a ? a : "(none)");
    }

    // Wait for seed to complete handshakes with all 4 others.
    fprintf(stderr, "[TEST] waiting for seed handshakes...\n");
    ASSERT(wait_for(&g_handshakes[1], 4, 30000));
    fprintf(stderr, "[TEST] node1 completed %d handshakes\n", g_handshakes[1]);

    /* Fully mesh: each node connects to every other node it isn't
       already connected to (seed connections handled above). */
    for (int i = 0; i < N; i++) {
        for (int j = i + 1; j < N; j++) {
            if (i == 1 || j == 1) continue; /* already connected via seed */
            if (listen_addrs[j][0])
                trellis_client_connect_addr(nodes[i], listen_addrs[j]);
        }
    }

    // Wait for every node to handshake with all N-1 peers.
    for (int i = 0; i < N; i++) {
        fprintf(stderr, "[TEST] waiting for node%d to reach %d handshakes...\n",
                i, N - 1);
        ASSERT(wait_for(&g_handshakes[i], N - 1, 20000));
    }
    fprintf(stderr, "[TEST] all nodes fully meshed\n");

    /* Allow extra time for the routing table to mark peers as eligible
     * onion relays after handshake events propagate.  Retry up to 10s
     * until the sender sees >=3 eligible peers (needed for onion). */
    for (int w = 0; w < 20; w++) {
        msleep(500);
        if (trellis_client_peer_count(nodes[0]) >= 4)
            break;
    }

    for (int i = 0; i < N; i++)
        fprintf(stderr, "[TEST] node%d: peers=%zu conns=%zu\n", i,
                trellis_client_peer_count(nodes[i]),
                trellis_client_conn_count(nodes[i]));

    // Set sender to onion routing.
    ASSERT(trellis_client_set_routing(nodes[0], TRELLIS_ROUTE_ONION) == TRELLIS_OK);
    fprintf(stderr, "[TEST] node0 routing mode set to ONION\n");
    msleep(1000);

    // Send DM from node0 to node4.
    const trellis_identity_t *id4 = trellis_client_identity(nodes[4]);
    ASSERT(id4 != NULL);

    const char *test_msg = "onion-routed-message-12345";
    g_dm_received = 0;
    memset(g_dm_payload, 0, sizeof(g_dm_payload));

    fprintf(stderr, "[TEST] sending DM from node0 to node4 (fp=%02x%02x...)\n",
            id4->fingerprint.bytes[0], id4->fingerprint.bytes[1]);

    trellis_err_t err = trellis_client_send_dm(
        nodes[0], &id4->fingerprint,
        (const uint8_t *)test_msg, strlen(test_msg));
    fprintf(stderr, "[TEST] send_dm returned %d\n", err);

    int delivered = wait_for(&g_dm_received, 1, 10000);

    ASSERT(delivered && "DM must be delivered via onion within timeout");
    fprintf(stderr, "[TEST] DM delivered via onion! payload='%s'\n", g_dm_payload);
    ASSERT(strcmp(g_dm_payload, test_msg) == 0);

    for (int i = 0; i < N; i++) {
        trellis_client_stop(nodes[i]);
        trellis_client_free(nodes[i]);
        unlink(id_paths[i]);
    }
    #undef N
}

static void test_onion_fallback_under_3_peers(void)
{
    const char *id_path = "/tmp/test_onion_fallback.key";
    unlink(id_path);

    trellis_client_config_t cfg = trellis_client_config_default();
    cfg.listen_addr = "127.0.0.1:0";
    cfg.disable_discovery = true;
    cfg.identity_path = id_path;
    cfg.conn_pow_iterations = UINT32_MAX;
    cfg.decoy_circuit_interval_ms = 0;
    trellis_client_t *c = trellis_client_new(&cfg);
    ASSERT(c != NULL);
    ASSERT(trellis_client_start(c) == TRELLIS_OK);
    ASSERT(trellis_client_set_routing(c, TRELLIS_ROUTE_ONION) == TRELLIS_OK);

    // With 0 peers, send_dm should fail gracefully (no crash)
    trellis_fingerprint_t fake_target = {0};
    fake_target.bytes[0] = 0xDE;
    fake_target.bytes[1] = 0xAD;

    trellis_err_t err = trellis_client_send_dm(
        c, &fake_target, (const uint8_t *)"test", 4);
    fprintf(stderr, "[TEST] onion with 0 peers returned %d (expected non-zero)\n", err);

    trellis_client_stop(c);
    trellis_client_free(c);
    unlink(id_path);
}

static void test_onion_wrap_peel_1_hop(void)
{
    trellis_kem_keypair_t hop;
    trellis_fingerprint_t relay_fp;

    ASSERT(trellis_kem_keygen(&hop) == TRELLIS_OK);
    memset(&relay_fp, 0xAA, sizeof(relay_fp));

    trellis_onion_circuit_t *circuit = trellis_onion_new(1);
    ASSERT(circuit != NULL);
    ASSERT(trellis_onion_add_hop(circuit, &relay_fp, hop.x25519_pk, hop.ml_kem_pk) == TRELLIS_OK);

    const uint8_t payload[] = "single-hop-payload";
    trellis_buf_t wrapped = {0};
    ASSERT(trellis_onion_wrap(circuit, payload, sizeof(payload) - 1, &wrapped) == TRELLIS_OK);
    ASSERT(wrapped.len > 0);

    trellis_buf_t peeled = {0};
    trellis_fingerprint_t next = {0};
    ASSERT(trellis_onion_peel(&hop, wrapped.data, wrapped.len,
                              &peeled, &next) == TRELLIS_OK);

    ASSERT(peeled.len == sizeof(payload) - 1);
    ASSERT(memcmp(peeled.data, payload, peeled.len) == 0);

    trellis_buf_free(&peeled);
    trellis_buf_free(&wrapped);
    trellis_onion_free(circuit);
}

static void test_onion_wrap_peel_max_hops(void)
{
    #define MAX_HOPS 7
    trellis_kem_keypair_t hops[MAX_HOPS];
    trellis_fingerprint_t relay_fps[MAX_HOPS];

    for (int i = 0; i < MAX_HOPS; i++) {
        ASSERT(trellis_kem_keygen(&hops[i]) == TRELLIS_OK);
        memset(&relay_fps[i], 0x20 + i, sizeof(trellis_fingerprint_t));
    }

    trellis_onion_circuit_t *circuit = trellis_onion_new(MAX_HOPS);
    ASSERT(circuit != NULL);

    for (int i = 0; i < MAX_HOPS; i++) {
        ASSERT(trellis_onion_add_hop(circuit, &relay_fps[i],
                                     hops[i].x25519_pk,
                                     hops[i].ml_kem_pk) == TRELLIS_OK);
    }

    const uint8_t payload[] = "deep-nested-onion-payload-7-hops";
    trellis_buf_t wrapped = {0};
    ASSERT(trellis_onion_wrap(circuit, payload, sizeof(payload) - 1, &wrapped) == TRELLIS_OK);
    ASSERT(wrapped.len > 0);

    trellis_buf_t current = trellis_buf_dup(wrapped);
    for (int i = 0; i < MAX_HOPS; i++) {
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
    #undef MAX_HOPS
}

static volatile int g_multi_dm_count = 0;

static void on_multi_message(const trellis_event_data_t *ev, void *ctx)
{
    (void)ctx;
    if (ev->event == TRELLIS_EV_MESSAGE && ev->data && ev->data_len > 0)
        g_multi_dm_count++;
}

static void test_onion_multi_message_delivery(void)
{
    #define RN 5
    static int r_indices[RN] = {0, 1, 2, 3, 4};
    trellis_client_t *rn[RN] = {0};
    char r_addrs[RN][32];
    char r_ids[RN][128];

    for (int i = 0; i < RN; i++) {
        snprintf(r_ids[i], sizeof(r_ids[i]), "/tmp/test_multi_node%d.key", i);
        unlink(r_ids[i]);
        memset(r_addrs[i], 0, sizeof(r_addrs[i]));
    }

    for (int i = 0; i < 5; i++) g_handshakes[i] = 0;

    trellis_client_config_t cfg0 = trellis_client_config_default();
    cfg0.listen_addr = "127.0.0.1:0";
    cfg0.disable_discovery = true;
    cfg0.identity_path = r_ids[1];
    cfg0.conn_pow_iterations = UINT32_MAX;
    cfg0.decoy_circuit_interval_ms = 0;
    rn[1] = trellis_client_new(&cfg0);
    ASSERT(rn[1] != NULL);
    trellis_client_on(rn[1], TRELLIS_EV_HANDSHAKE_COMPLETE, on_handshake, &r_indices[1]);
    ASSERT(trellis_client_start(rn[1]) == TRELLIS_OK);
    trellis_dht_set_eclipse_enabled(trellis_client_get_dht(rn[1]), false);
    trellis_dht_disable_sybil(trellis_client_get_dht(rn[1]));
    snprintf(r_addrs[1], sizeof(r_addrs[1]), "%s", trellis_client_listen_addr(rn[1]));

    for (int i = 0; i < RN; i++) {
        if (i == 1) continue;
        trellis_client_config_t cfg = trellis_client_config_default();
        cfg.listen_addr = "127.0.0.1:0";
        cfg.seed_addr = r_addrs[1];
        cfg.disable_discovery = true;
        cfg.identity_path = r_ids[i];
        cfg.conn_pow_iterations = UINT32_MAX;
        cfg.decoy_circuit_interval_ms = 0;
        rn[i] = trellis_client_new(&cfg);
        ASSERT(rn[i] != NULL);
        trellis_client_on(rn[i], TRELLIS_EV_HANDSHAKE_COMPLETE, on_handshake, &r_indices[i]);
        if (i == 4)
            trellis_client_on(rn[i], TRELLIS_EV_MESSAGE, on_multi_message, NULL);
        ASSERT(trellis_client_start(rn[i]) == TRELLIS_OK);
        trellis_dht_set_eclipse_enabled(trellis_client_get_dht(rn[i]), false);
        trellis_dht_disable_sybil(trellis_client_get_dht(rn[i]));
        const char *a = trellis_client_listen_addr(rn[i]);
        if (a) snprintf(r_addrs[i], sizeof(r_addrs[i]), "%s", a);
    }

    ASSERT(wait_for(&g_handshakes[1], 4, 30000));

    for (int i = 0; i < RN; i++)
        for (int j = i + 1; j < RN; j++) {
            if (i == 1 || j == 1) continue;
            if (r_addrs[j][0])
                trellis_client_connect_addr(rn[i], r_addrs[j]);
        }

    for (int i = 0; i < RN; i++)
        ASSERT(wait_for(&g_handshakes[i], RN - 1, 20000));

    for (int w = 0; w < 30; w++) {
        msleep(500);
        if (trellis_client_peer_count(rn[0]) >= 4)
            break;
    }
    fprintf(stderr, "[TEST] replay: node0 peers=%zu\n",
            trellis_client_peer_count(rn[0]));

    ASSERT(trellis_client_set_routing(rn[0], TRELLIS_ROUTE_ONION) == TRELLIS_OK);
    msleep(2000);

    const trellis_identity_t *id4 = trellis_client_identity(rn[4]);
    ASSERT(id4 != NULL);

    g_multi_dm_count = 0;

    /* Send two distinct DMs — both should be delivered since they are
     * independent messages (not onion-layer replays). */
    trellis_err_t err1 = trellis_client_send_dm(rn[0], &id4->fingerprint,
                           (const uint8_t *)"msg-one", 7);
    trellis_err_t err2 = trellis_client_send_dm(rn[0], &id4->fingerprint,
                           (const uint8_t *)"msg-two", 7);
    fprintf(stderr, "[TEST] multi-msg sends: err1=%d err2=%d\n", err1, err2);

    ASSERT(wait_for(&g_multi_dm_count, 2, 20000));
    fprintf(stderr, "[TEST] multi-message DM count = %d (expect 2)\n",
            g_multi_dm_count);

    for (int i = 0; i < RN; i++) {
        trellis_client_stop(rn[i]);
        trellis_client_free(rn[i]);
        unlink(r_ids[i]);
    }
    #undef RN
}

static void test_onion_routing_mode_switch(void)
{
    const char *id_path = "/tmp/test_onion_mode_switch.key";
    unlink(id_path);

    trellis_client_config_t cfg = trellis_client_config_default();
    cfg.listen_addr = "127.0.0.1:0";
    cfg.disable_discovery = true;
    cfg.identity_path = id_path;
    cfg.conn_pow_iterations = UINT32_MAX;
    cfg.decoy_circuit_interval_ms = 0;
    trellis_client_t *c = trellis_client_new(&cfg);
    ASSERT(c != NULL);
    ASSERT(trellis_client_start(c) == TRELLIS_OK);

    ASSERT(trellis_client_set_routing(c, TRELLIS_ROUTE_DIRECT) == TRELLIS_OK);
    ASSERT(trellis_client_set_routing(c, TRELLIS_ROUTE_ONION) == TRELLIS_OK);
    ASSERT(trellis_client_set_routing(c, TRELLIS_ROUTE_DIRECT) == TRELLIS_OK);
    ASSERT(trellis_client_set_routing(c, TRELLIS_ROUTE_RELAY) == TRELLIS_OK);
    ASSERT(trellis_client_set_routing(c, TRELLIS_ROUTE_ONION) == TRELLIS_OK);

    // Verify client is still operational after mode switches.
    ASSERT(trellis_client_peer_count(c) == 0);

    trellis_client_stop(c);
    trellis_client_free(c);
    unlink(id_path);
}

int main(void)
{
    printf("=== Test Suite: Onion Routing ===\n");

    ASSERT(trellis_crypto_init() == TRELLIS_OK);

    RUN_TEST(test_onion_wrap_peel_1_hop);
    RUN_TEST(test_onion_wrap_peel_max_hops);
    RUN_TEST(test_onion_routing_mode_switch);
    RUN_TEST(test_onion_fallback_under_3_peers);
    RUN_TEST(test_onion_routing_5_nodes);
    RUN_TEST(test_onion_multi_message_delivery);

    trellis_crypto_cleanup();

    printf("=== All onion routing tests passed ===\n");
    return 0;
}
