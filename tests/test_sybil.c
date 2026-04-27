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
#include <math.h>
#include <uv.h>
#include <trellis/trellis.h>
#include <trellis/sybil.h>

#define RUN_TEST(fn) do { printf("  %-50s", #fn "..."); fn(); printf("PASS\n"); } while(0)
#define ASSERT(cond) do { if (!(cond)) { fprintf(stderr, "FAIL at %s:%d: %s\n", __FILE__, __LINE__, #cond); abort(); } } while(0)

static trellis_identity_t g_id;
static uv_loop_t *g_loop;

// --- VDF tests ---
static void test_vdf_compute_verify(void) {
    trellis_vdf_proof_t proof;
    memset(&proof, 0, sizeof(proof));

    // Use a small iteration count for test speed.
    trellis_err_t err = trellis_vdf_compute(
        &g_id.fingerprint, 256, &g_id, &proof);
    ASSERT(err == TRELLIS_OK);
    ASSERT(proof.iterations == 256);
    ASSERT(proof.created_at > 0);

    // Verify should pass.
    trellis_identity_public_t pub;
    trellis_identity_public_from(&g_id, &pub);
    err = trellis_vdf_verify(&g_id.fingerprint, &proof, &pub);
    ASSERT(err == TRELLIS_OK);
}

static void test_vdf_reject_tampered(void) {
    trellis_vdf_proof_t proof;
    memset(&proof, 0, sizeof(proof));

    trellis_err_t err = trellis_vdf_compute(
        &g_id.fingerprint, 256, &g_id, &proof);
    ASSERT(err == TRELLIS_OK);

    // Tamper with the final hash.
    proof.final_hash[0] ^= 0xFF;

    trellis_identity_public_t pub;
    trellis_identity_public_from(&g_id, &pub);
    err = trellis_vdf_verify(&g_id.fingerprint, &proof, &pub);
    ASSERT(err == TRELLIS_ERR_VDF_INVALID);
}

static void test_vdf_reject_wrong_identity(void) {
    trellis_vdf_proof_t proof;
    memset(&proof, 0, sizeof(proof));

    trellis_err_t err = trellis_vdf_compute(
        &g_id.fingerprint, 256, &g_id, &proof);
    ASSERT(err == TRELLIS_OK);

    // Verify with a different identity's fingerprint.
    trellis_identity_t other_id;
    ASSERT(trellis_identity_generate(&other_id) == TRELLIS_OK);

    trellis_identity_public_t pub;
    trellis_identity_public_from(&g_id, &pub);
    err = trellis_vdf_verify(&other_id.fingerprint, &proof, &pub);
    ASSERT(err == TRELLIS_ERR_VDF_INVALID);

    trellis_identity_destroy(&other_id);
}

static void test_vdf_age_score(void) {
    trellis_vdf_proof_t proof;
    memset(&proof, 0, sizeof(proof));

    // Zero created_at -> score 0.
    ASSERT(trellis_vdf_age_score(&proof) == 0.0);

    // Recent proof -> low but positive score.
    proof.created_at = trellis_now_ms() - 1000;
    double score = trellis_vdf_age_score(&proof);
    ASSERT(score >= 0.0 && score < 0.01);

    // NULL proof -> 0
    ASSERT(trellis_vdf_age_score(NULL) == 0.0);
}

// --- Trust graph tests ---
static void test_trust_graph_lifecycle(void) {
    trellis_trust_graph_t *tg = trellis_trust_graph_new();
    ASSERT(tg != NULL);
    trellis_trust_graph_free(tg);
}

static void test_trust_observe(void) {
    trellis_trust_graph_t *tg = trellis_trust_graph_new();
    ASSERT(tg != NULL);

    trellis_fingerprint_t fp;
    randombytes_buf(fp.bytes, sizeof(fp.bytes));

    ASSERT(trellis_trust_observe(tg, &fp, 0.5) == TRELLIS_OK);

    const trellis_trust_entry_t *e = trellis_trust_lookup(tg, &fp);
    ASSERT(e != NULL);
    ASSERT(fabs(e->direct_trust - 0.5) < 0.001);

    // Observe again -> cumulative, clamped at 1.0.
    ASSERT(trellis_trust_observe(tg, &fp, 0.8) == TRELLIS_OK);
    e = trellis_trust_lookup(tg, &fp);
    ASSERT(fabs(e->direct_trust - 1.0) < 0.001);

    trellis_trust_graph_free(tg);
}

static void test_trust_vouch_and_recompute(void) {
    trellis_trust_graph_t *tg = trellis_trust_graph_new();
    ASSERT(tg != NULL);

    // Create two identities: A vouches for B.
    trellis_identity_t id_a, id_b;
    ASSERT(trellis_identity_generate(&id_a) == TRELLIS_OK);
    ASSERT(trellis_identity_generate(&id_b) == TRELLIS_OK);

    // Give A some direct trust.
    ASSERT(trellis_trust_observe(tg, &id_a.fingerprint, 0.9) == TRELLIS_OK);

    // Create and add vouch.
    trellis_vouch_t vouch;
    ASSERT(trellis_vouch_create(&id_a, &id_b.fingerprint, &vouch) == TRELLIS_OK);

    trellis_identity_public_t pub_a;
    trellis_identity_public_from(&id_a, &pub_a);
    ASSERT(trellis_trust_add_vouch(tg, &vouch, &pub_a) == TRELLIS_OK);

    // B should have a vouch count.
    const trellis_trust_entry_t *eb = trellis_trust_lookup(tg, &id_b.fingerprint);
    ASSERT(eb != NULL);
    ASSERT(eb->vouch_count == 1);

    // Recompute should propagate transitive trust.
    trellis_trust_recompute(tg);

    double score_b = trellis_trust_score(tg, &id_b.fingerprint);
    ASSERT(score_b > 0.0);

    trellis_identity_destroy(&id_a);
    trellis_identity_destroy(&id_b);
    trellis_trust_graph_free(tg);
}

static void test_trust_invalid_vouch(void) {
    trellis_trust_graph_t *tg = trellis_trust_graph_new();
    ASSERT(tg != NULL);

    trellis_identity_t id_a, id_b, id_fake;
    ASSERT(trellis_identity_generate(&id_a) == TRELLIS_OK);
    ASSERT(trellis_identity_generate(&id_b) == TRELLIS_OK);
    ASSERT(trellis_identity_generate(&id_fake) == TRELLIS_OK);

    trellis_vouch_t vouch;
    ASSERT(trellis_vouch_create(&id_a, &id_b.fingerprint, &vouch) == TRELLIS_OK);

    // Verify with wrong public key should fail.
    trellis_identity_public_t pub_fake;
    trellis_identity_public_from(&id_fake, &pub_fake);
    trellis_err_t err = trellis_trust_add_vouch(tg, &vouch, &pub_fake);
    ASSERT(err == TRELLIS_ERR_VOUCH_INVALID);

    trellis_identity_destroy(&id_a);
    trellis_identity_destroy(&id_b);
    trellis_identity_destroy(&id_fake);
    trellis_trust_graph_free(tg);
}

// --- Behavioral scoring tests ---
static void test_behavior_lifecycle(void) {
    trellis_behavior_tracker_t *bt = trellis_behavior_new();
    ASSERT(bt != NULL);
    trellis_behavior_free(bt);
}

static void test_behavior_circuit_scoring(void) {
    trellis_behavior_tracker_t *bt = trellis_behavior_new();
    ASSERT(bt != NULL);

    trellis_fingerprint_t fp;
    randombytes_buf(fp.bytes, sizeof(fp.bytes));

    // Record 10 attempts, 8 completions -> fidelity = 0.8.
    for (int i = 0; i < 10; i++)
        ASSERT(trellis_behavior_record_circuit(bt, &fp, i < 8) == TRELLIS_OK);

    double score = trellis_behavior_score(bt, &fp);
    ASSERT(score > 0.0 && score <= 1.0);

    // Unknown peer -> neutral 0.5.
    trellis_fingerprint_t unknown;
    randombytes_buf(unknown.bytes, sizeof(unknown.bytes));
    ASSERT(fabs(trellis_behavior_score(bt, &unknown) - 0.5) < 0.001);

    trellis_behavior_free(bt);
}

static void test_behavior_bandwidth_honesty(void) {
    trellis_behavior_tracker_t *bt = trellis_behavior_new();
    ASSERT(bt != NULL);

    trellis_fingerprint_t fp;
    randombytes_buf(fp.bytes, sizeof(fp.bytes));

    // Liar: claims 1000, measures 200.
    ASSERT(trellis_behavior_record_bandwidth(bt, &fp, 1000.0, 200.0) == TRELLIS_OK);

    // Also record a circuit so fidelity contributes.
    ASSERT(trellis_behavior_record_circuit(bt, &fp, true) == TRELLIS_OK);

    double score = trellis_behavior_score(bt, &fp);
    // Should be penalized: fidelity=1.0, bw_honesty=0.2 -> 0.5*1+0.5*0.2=0.6.
    ASSERT(score < 0.7);

    trellis_behavior_free(bt);
}

static void test_behavior_subnet_detection(void) {
    trellis_behavior_tracker_t *bt = trellis_behavior_new();
    ASSERT(bt != NULL);

    trellis_fingerprint_t fp_a, fp_b, fp_c;
    randombytes_buf(fp_a.bytes, sizeof(fp_a.bytes));
    randombytes_buf(fp_b.bytes, sizeof(fp_b.bytes));
    randombytes_buf(fp_c.bytes, sizeof(fp_c.bytes));

    // A and B share a /24.
    ASSERT(trellis_behavior_record_seen(bt, &fp_a, "192.168.1.10:8000") == TRELLIS_OK);
    ASSERT(trellis_behavior_record_seen(bt, &fp_b, "192.168.1.20:8001") == TRELLIS_OK);
    // C is on a different subnet.
    ASSERT(trellis_behavior_record_seen(bt, &fp_c, "10.0.0.5:8002") == TRELLIS_OK);

    ASSERT(trellis_behavior_same_subnet(bt, &fp_a, &fp_b) == true);
    ASSERT(trellis_behavior_same_subnet(bt, &fp_a, &fp_c) == false);

    trellis_behavior_free(bt);
}

static void test_behavior_uptime_correlation(void) {
    trellis_behavior_tracker_t *bt = trellis_behavior_new();
    ASSERT(bt != NULL);

    trellis_fingerprint_t fp_a, fp_b;
    randombytes_buf(fp_a.bytes, sizeof(fp_a.bytes));
    randombytes_buf(fp_b.bytes, sizeof(fp_b.bytes));

    // Simulate identical uptime patterns -> high correlation.
    for (int i = 0; i < 48; i++) {
        trellis_behavior_record_seen(bt, &fp_a, "1.2.3.4:80");
        trellis_behavior_record_seen(bt, &fp_b, "5.6.7.8:80");
        trellis_behavior_advance_bucket(bt);
    }

    double corr = trellis_behavior_uptime_correlation(bt, &fp_a, &fp_b);
    // Should be very high (near 1.0) since patterns are identical.
    ASSERT(corr > 0.9);

    trellis_behavior_free(bt);
}

static void test_pop_stub_passes(void) {
    trellis_fingerprint_t fp;
    randombytes_buf(fp.bytes, sizeof(fp.bytes));

    ASSERT(trellis_pop_has_credential(&fp) == true);
    ASSERT(trellis_pop_verify(&fp, NULL, 0) == TRELLIS_ERR_INVALID_ARG);

    uint8_t *cred = NULL;
    size_t cred_len = 0;
    trellis_fingerprint_t verifier_fp;
    randombytes_buf(verifier_fp.bytes, sizeof(verifier_fp.bytes));
    ASSERT(trellis_pop_present(&g_id, &verifier_fp, &cred, &cred_len) == TRELLIS_OK);
}

static void test_hco_stub_passes(void) {
    trellis_fingerprint_t fp;
    randombytes_buf(fp.bytes, sizeof(fp.bytes));

    ASSERT(trellis_hco_is_current(&fp) == true);
    ASSERT(fabs(trellis_hco_freshness(&fp) - 1.0) < 0.001);

    trellis_hco_challenge_t challenge;
    ASSERT(trellis_hco_issue(&challenge) == TRELLIS_OK);
    ASSERT(challenge.issued_at > 0);
    ASSERT(challenge.expires_at > challenge.issued_at);

    ASSERT(trellis_hco_verify(&challenge, NULL, 0) == TRELLIS_OK);
}

// --- Composite scorer tests ---
static void test_composite_lifecycle(void) {
    trellis_sybil_t *sybil = trellis_sybil_new();
    ASSERT(sybil != NULL);
    ASSERT(trellis_sybil_trust(sybil) != NULL);
    ASSERT(trellis_sybil_behavior(sybil) != NULL);
    trellis_sybil_free(sybil);
}

static void test_composite_score_range(void) {
    trellis_sybil_t *sybil = trellis_sybil_new();
    ASSERT(sybil != NULL);

    trellis_fingerprint_t fp;
    randombytes_buf(fp.bytes, sizeof(fp.bytes));

    double score = trellis_sybil_composite_score(
        sybil, &fp, 50.0, 0.8, 0.9, NULL);
    ASSERT(score >= 0.0 && score <= 1.0);

    trellis_sybil_free(sybil);
}

static void test_composite_admission_no_proof(void) {
    trellis_sybil_t *sybil = trellis_sybil_new();
    ASSERT(sybil != NULL);

    trellis_fingerprint_t fp;
    randombytes_buf(fp.bytes, sizeof(fp.bytes));

    // NULL VDF proof -> admission fails, peer will be quarantined
    trellis_err_t err = trellis_sybil_check_admission(
        sybil, &fp, NULL, NULL);
    ASSERT(err == TRELLIS_ERR_VDF_INVALID);

    trellis_sybil_free(sybil);
}

static void test_composite_admission_with_valid_proof(void) {
    trellis_sybil_t *sybil = trellis_sybil_new();
    ASSERT(sybil != NULL);

    trellis_vdf_proof_t proof;
    ASSERT(trellis_vdf_compute(&g_id.fingerprint, 64, &g_id, &proof) == TRELLIS_OK);

    trellis_identity_public_t pub;
    trellis_identity_public_from(&g_id, &pub);

    trellis_err_t err = trellis_sybil_check_admission(
        sybil, &g_id.fingerprint, &proof, &pub);
    ASSERT(err == TRELLIS_OK);

    trellis_sybil_free(sybil);
}

static void test_composite_hop_selection(void) {
    trellis_sybil_t *sybil = trellis_sybil_new();
    ASSERT(sybil != NULL);

    // Build some candidate peers.
    trellis_peer_info_t peers[6];
    trellis_peer_info_t *ptrs[6];
    for (int i = 0; i < 6; i++) {
        memset(&peers[i], 0, sizeof(peers[i]));
        randombytes_buf(peers[i].fingerprint.bytes,
                        sizeof(peers[i].fingerprint.bytes));
        peers[i].latency_ms = 20.0 + i * 10;
        peers[i].bandwidth = 0.8;
        peers[i].reliability = 0.9;
        snprintf(peers[i].addr, sizeof(peers[i].addr),
                 "%d.%d.%d.1:8000", 10 + i, i, i);
        ptrs[i] = &peers[i];

        // Seed behavioral data so scoring has something to work with.
        trellis_behavior_record_seen(trellis_sybil_behavior(sybil),
                                     &peers[i].fingerprint, peers[i].addr);
        trellis_behavior_record_circuit(trellis_sybil_behavior(sybil),
                                        &peers[i].fingerprint, true);
    }

    trellis_fingerprint_t exclude;
    randombytes_buf(exclude.bytes, sizeof(exclude.bytes));

    trellis_peer_info_t *hops[3];
    size_t got = trellis_sybil_select_hops(
        sybil, ptrs, 6, &exclude, hops, 3);
    ASSERT(got == 3);

    // Ensure all 3 hops are distinct.
    ASSERT(!trellis_fingerprint_eq(&hops[0]->fingerprint,
                                   &hops[1]->fingerprint));
    ASSERT(!trellis_fingerprint_eq(&hops[1]->fingerprint,
                                   &hops[2]->fingerprint));
    ASSERT(!trellis_fingerprint_eq(&hops[0]->fingerprint,
                                   &hops[2]->fingerprint));

    trellis_sybil_free(sybil);
}

static void test_dht_quarantine_no_vdf(void) {
    trellis_dht_t *dht = trellis_dht_new(&g_id, g_loop);
    ASSERT(dht != NULL);

    trellis_identity_t peer_id;
    ASSERT(trellis_identity_generate(&peer_id) == TRELLIS_OK);

    trellis_peer_info_t info;
    memset(&info, 0, sizeof(info));
    trellis_identity_public_t pub;
    trellis_identity_public_from(&peer_id, &pub);
    info.identity = pub;
    info.fingerprint = pub.fingerprint;
    snprintf(info.addr, sizeof(info.addr), "127.0.0.1:9000");
    info.latency_ms = 10.0;
    info.bandwidth = 1000.0;
    info.reliability = 0.99;

    ASSERT(trellis_dht_add_peer(dht, &info) == TRELLIS_OK);

    // Peer should be in the DHT.
    ASSERT(trellis_dht_peer_count(dht) == 1);

    // Peer should be quarantined since no VDF proof was provided.
    const trellis_peer_info_t *looked_up = trellis_dht_lookup_peer(dht, &info.fingerprint);
    ASSERT(looked_up != NULL);
    ASSERT(looked_up->quarantined == true);

    trellis_identity_destroy(&peer_id);
    trellis_dht_free(dht);
}

// --- Vouch gossip integration tests ---
static trellis_fingerprint_t g_last_gossip_target;
static const uint8_t *g_last_gossip_data;
static size_t g_last_gossip_len;

static void mock_gossip_send(void *client,
                             const trellis_fingerprint_t *target,
                             const uint8_t *data, size_t len)
{
    (void)client;
    g_last_gossip_target = *target;
    g_last_gossip_data = data;
    g_last_gossip_len = len;
}

static void test_vouch_gossip_roundtrip(void) {
    trellis_sybil_t *sybil = trellis_sybil_new();
    ASSERT(sybil != NULL);

    trellis_dht_t *dht = trellis_dht_new(&g_id, g_loop);
    ASSERT(dht != NULL);

    trellis_gossip_t *gossip = trellis_gossip_new(dht, &g_id);
    ASSERT(gossip != NULL);
    trellis_gossip_set_send(gossip, mock_gossip_send, NULL);

    // Subscribe to vouch messages.
    ASSERT(trellis_sybil_subscribe_vouches(sybil, gossip) == TRELLIS_OK);

    // Create a vouch: g_id vouches for some peer.
    trellis_identity_t peer_id;
    ASSERT(trellis_identity_generate(&peer_id) == TRELLIS_OK);

    trellis_vouch_t vouch;
    ASSERT(trellis_vouch_create(&g_id, &peer_id.fingerprint, &vouch) == TRELLIS_OK);

    trellis_identity_public_t pub;
    trellis_identity_public_from(&g_id, &pub);

    // Publish the vouch through gossip.
    ASSERT(trellis_sybil_publish_vouch(sybil, gossip, &vouch, &pub) == TRELLIS_OK);

    // The vouch should now be in the trust graph.
    const trellis_trust_entry_t *e =
        trellis_trust_lookup(trellis_sybil_trust(sybil), &peer_id.fingerprint);
    ASSERT(e != NULL);
    ASSERT(e->vouch_count >= 1);

    trellis_identity_destroy(&peer_id);
    trellis_gossip_free(gossip);
    trellis_dht_free(dht);
    trellis_sybil_free(sybil);
}

// --- Error string tests ---
static void test_sybil_error_strings(void) {
    ASSERT(strcmp(trellis_err_str(TRELLIS_ERR_SYBIL), "sybil resistance error") == 0);
    ASSERT(strcmp(trellis_err_str(TRELLIS_ERR_VDF_INVALID), "VDF proof invalid") == 0);
    ASSERT(strcmp(trellis_err_str(TRELLIS_ERR_VDF_TOO_YOUNG),
                  "VDF proof too young for relay duty") == 0);
    ASSERT(strcmp(trellis_err_str(TRELLIS_ERR_VOUCH_INVALID),
                  "vouch attestation invalid") == 0);
    ASSERT(strcmp(trellis_err_str(TRELLIS_ERR_QUARANTINED),
                  "peer is quarantined") == 0);
    ASSERT(strcmp(trellis_err_str(TRELLIS_ERR_BEHAVIOR),
                  "behavioral scoring error") == 0);
}

int main(void) {
    printf("=== Test Suite: Sybil Resistance ===\n");

    ASSERT(trellis_crypto_init() == TRELLIS_OK);
    ASSERT(trellis_identity_generate(&g_id) == TRELLIS_OK);
    g_loop = uv_default_loop();
    ASSERT(g_loop != NULL);

    // VDF
    RUN_TEST(test_vdf_compute_verify);
    RUN_TEST(test_vdf_reject_tampered);
    RUN_TEST(test_vdf_reject_wrong_identity);
    RUN_TEST(test_vdf_age_score);

    // Trust graph.
    RUN_TEST(test_trust_graph_lifecycle);
    RUN_TEST(test_trust_observe);
    RUN_TEST(test_trust_vouch_and_recompute);
    RUN_TEST(test_trust_invalid_vouch);

    // Behavioral scoring.
    RUN_TEST(test_behavior_lifecycle);
    RUN_TEST(test_behavior_circuit_scoring);
    RUN_TEST(test_behavior_bandwidth_honesty);
    RUN_TEST(test_behavior_subnet_detection);
    RUN_TEST(test_behavior_uptime_correlation);

    // Stubs
    RUN_TEST(test_pop_stub_passes);
    RUN_TEST(test_hco_stub_passes);

    // Composite
    RUN_TEST(test_composite_lifecycle);
    RUN_TEST(test_composite_score_range);
    RUN_TEST(test_composite_admission_no_proof);
    RUN_TEST(test_composite_admission_with_valid_proof);
    RUN_TEST(test_composite_hop_selection);

    // Integration
    RUN_TEST(test_dht_quarantine_no_vdf);

    // Vouch gossip.
    RUN_TEST(test_vouch_gossip_roundtrip);

    // Error strings.
    RUN_TEST(test_sybil_error_strings);

    trellis_identity_destroy(&g_id);
    trellis_crypto_cleanup();

    printf("=== All sybil resistance tests passed ===\n");
    return 0;
}
