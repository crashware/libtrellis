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

static int mem_is_zero(const uint8_t *p, size_t len) {
    for (size_t i = 0; i < len; i++)
        if (p[i] != 0) return 0;
    return 1;
}

static void test_shake256(void) {
    const uint8_t input[] = "trellis shake256 test vector";
    uint8_t out1[64], out2[64];

    ASSERT(trellis_shake256(input, sizeof(input) - 1, out1, sizeof(out1)) == TRELLIS_OK);
    ASSERT(!mem_is_zero(out1, sizeof(out1)));

    ASSERT(trellis_shake256(input, sizeof(input) - 1, out2, sizeof(out2)) == TRELLIS_OK);
    ASSERT(memcmp(out1, out2, sizeof(out1)) == 0);
}

static void test_hkdf_shake256(void) {
    const uint8_t ikm[]  = "input key material";
    const uint8_t salt[] = "optional salt";
    const uint8_t info[] = "context info";
    uint8_t okm1[48], okm2[48];

    ASSERT(trellis_hkdf_shake256(ikm, sizeof(ikm) - 1,
                                  salt, sizeof(salt) - 1,
                                  info, sizeof(info) - 1,
                                  okm1, sizeof(okm1)) == TRELLIS_OK);
    ASSERT(!mem_is_zero(okm1, sizeof(okm1)));

    ASSERT(trellis_hkdf_shake256(ikm, sizeof(ikm) - 1,
                                  salt, sizeof(salt) - 1,
                                  info, sizeof(info) - 1,
                                  okm2, sizeof(okm2)) == TRELLIS_OK);
    ASSERT(memcmp(okm1, okm2, sizeof(okm1)) == 0);
}

static void test_kmac256(void) {
    const uint8_t key[]    = "kmac key value";
    const uint8_t data[]   = "data to mac";
    const uint8_t custom[] = "custom string";
    uint8_t tag1[32], tag2[32];

    ASSERT(trellis_kmac256(key, sizeof(key) - 1,
                           data, sizeof(data) - 1,
                           custom, sizeof(custom) - 1,
                           tag1, sizeof(tag1)) == TRELLIS_OK);
    ASSERT(!mem_is_zero(tag1, sizeof(tag1)));

    ASSERT(trellis_kmac256(key, sizeof(key) - 1,
                           data, sizeof(data) - 1,
                           custom, sizeof(custom) - 1,
                           tag2, sizeof(tag2)) == TRELLIS_OK);
    ASSERT(memcmp(tag1, tag2, sizeof(tag1)) == 0);
}

static void test_aes256gcm_roundtrip(void) {
    uint8_t key[TRELLIS_AES_KEY_LEN];
    uint8_t nonce[TRELLIS_AES_NONCE_LEN];
    memset(key, 0xAA, sizeof(key));
    memset(nonce, 0x55, sizeof(nonce));

    const uint8_t plaintext[] = "hello trellis aes-256-gcm";
    const uint8_t ad[] = "associated data";
    size_t pt_len = sizeof(plaintext) - 1;

    uint8_t ciphertext[256];
    uint8_t tag[TRELLIS_AES_TAG_LEN];
    ASSERT(trellis_aes256gcm_encrypt(key, nonce, plaintext, pt_len,
                                     ad, sizeof(ad) - 1,
                                     ciphertext, tag) == TRELLIS_OK);

    uint8_t recovered[256];
    ASSERT(trellis_aes256gcm_decrypt(key, nonce, ciphertext, pt_len,
                                     ad, sizeof(ad) - 1,
                                     tag, recovered) == TRELLIS_OK);
    ASSERT(memcmp(recovered, plaintext, pt_len) == 0);
}

static void test_aes256gcm_tamper(void) {
    uint8_t key[TRELLIS_AES_KEY_LEN];
    uint8_t nonce[TRELLIS_AES_NONCE_LEN];
    memset(key, 0xBB, sizeof(key));
    memset(nonce, 0x66, sizeof(nonce));

    const uint8_t plaintext[] = "tamper detection test";
    size_t pt_len = sizeof(plaintext) - 1;

    uint8_t ciphertext[256];
    uint8_t tag[TRELLIS_AES_TAG_LEN];
    ASSERT(trellis_aes256gcm_encrypt(key, nonce, plaintext, pt_len,
                                     NULL, 0,
                                     ciphertext, tag) == TRELLIS_OK);

    ciphertext[0] ^= 0xFF;

    uint8_t recovered[256];
    trellis_err_t err = trellis_aes256gcm_decrypt(key, nonce, ciphertext, pt_len,
                                                  NULL, 0, tag, recovered);
    ASSERT(err != TRELLIS_OK);
}

static void test_identity_generate(void) {
    trellis_identity_t id;
    ASSERT(trellis_identity_generate(&id) == TRELLIS_OK);

    ASSERT(!mem_is_zero(id.ed25519_pk, TRELLIS_ED25519_PK_LEN));
    ASSERT(!mem_is_zero(id.ml_dsa_pk, TRELLIS_ML_DSA_87_PK_LEN));
    ASSERT(!mem_is_zero(id.x25519_pk, TRELLIS_X25519_PK_LEN));
    ASSERT(!mem_is_zero(id.ml_kem_pk, TRELLIS_ML_KEM_1024_PK_LEN));
    ASSERT(!mem_is_zero(id.slh_dsa_pk, TRELLIS_SLH_DSA_PK_LEN));
    ASSERT(!mem_is_zero(id.fingerprint.bytes, TRELLIS_FINGERPRINT_LEN));

    trellis_identity_destroy(&id);
}

static void test_identity_fingerprint(void) {
    trellis_identity_t id;
    ASSERT(trellis_identity_generate(&id) == TRELLIS_OK);

    trellis_identity_public_t pub;
    trellis_identity_public_from(&id, &pub);

    trellis_fingerprint_t fp1, fp2;
    ASSERT(trellis_identity_fingerprint(&pub, &fp1) == TRELLIS_OK);
    ASSERT(trellis_identity_fingerprint(&pub, &fp2) == TRELLIS_OK);
    ASSERT(trellis_fingerprint_eq(&fp1, &fp2));

    trellis_identity_destroy(&id);
}

static void test_identity_sign_verify(void) {
    trellis_identity_t id;
    ASSERT(trellis_identity_generate(&id) == TRELLIS_OK);

    trellis_identity_public_t pub;
    trellis_identity_public_from(&id, &pub);

    const uint8_t msg[] = "message to sign";
    trellis_signature_t sig;
    ASSERT(trellis_identity_sign(&id, msg, sizeof(msg) - 1, &sig) == TRELLIS_OK);
    ASSERT(trellis_identity_verify(&pub, msg, sizeof(msg) - 1, &sig) == TRELLIS_OK);

    const uint8_t tampered[] = "tampered message";
    ASSERT(trellis_identity_verify(&pub, tampered, sizeof(tampered) - 1, &sig) != TRELLIS_OK);

    trellis_identity_destroy(&id);
}

static void test_identity_backup_sign_verify(void) {
    trellis_identity_t id;
    ASSERT(trellis_identity_generate(&id) == TRELLIS_OK);

    trellis_identity_public_t pub;
    trellis_identity_public_from(&id, &pub);

    const uint8_t msg[] = "backup sign test payload";
    uint8_t sig[TRELLIS_SLH_DSA_SIG_LEN];
    size_t sig_len = sizeof(sig);
    ASSERT(trellis_identity_backup_sign(&id, msg, sizeof(msg) - 1,
                                        sig, &sig_len) == TRELLIS_OK);
    ASSERT(sig_len > 0);
    ASSERT(trellis_identity_backup_verify(&pub, msg, sizeof(msg) - 1,
                                          sig, sig_len) == TRELLIS_OK);

    trellis_identity_destroy(&id);
}

static void test_identity_serialize_roundtrip(void) {
    trellis_identity_t id;
    ASSERT(trellis_identity_generate(&id) == TRELLIS_OK);

    trellis_identity_public_t pub;
    trellis_identity_public_from(&id, &pub);

    uint8_t buf[8192];
    size_t written = trellis_identity_public_serialize(&pub, buf, sizeof(buf));
    ASSERT(written > 0);

    trellis_identity_public_t restored;
    ASSERT(trellis_identity_public_deserialize(buf, written, &restored) == TRELLIS_OK);

    ASSERT(memcmp(pub.ed25519_pk, restored.ed25519_pk, TRELLIS_ED25519_PK_LEN) == 0);
    ASSERT(memcmp(pub.ml_dsa_pk, restored.ml_dsa_pk, TRELLIS_ML_DSA_87_PK_LEN) == 0);
    ASSERT(memcmp(pub.x25519_pk, restored.x25519_pk, TRELLIS_X25519_PK_LEN) == 0);
    ASSERT(memcmp(pub.ml_kem_pk, restored.ml_kem_pk, TRELLIS_ML_KEM_1024_PK_LEN) == 0);
    ASSERT(memcmp(pub.slh_dsa_pk, restored.slh_dsa_pk, TRELLIS_SLH_DSA_PK_LEN) == 0);
    ASSERT(trellis_fingerprint_eq(&pub.fingerprint, &restored.fingerprint));

    trellis_identity_destroy(&id);
}

static void test_kem_roundtrip(void) {
    trellis_kem_keypair_t kp;
    ASSERT(trellis_kem_keygen(&kp) == TRELLIS_OK);

    trellis_kem_encaps_result_t enc;
    ASSERT(trellis_kem_encaps(kp.x25519_pk, kp.ml_kem_pk, &enc) == TRELLIS_OK);

    uint8_t ss_dec[TRELLIS_ML_KEM_1024_SS_LEN];
    ASSERT(trellis_kem_decaps(&kp, enc.x25519_pk, enc.ml_kem_ct, ss_dec) == TRELLIS_OK);

    ASSERT(memcmp(enc.shared_secret, ss_dec, TRELLIS_ML_KEM_1024_SS_LEN) == 0);
}

static void test_handshake_3step(void) {
    trellis_identity_t alice_id, bob_id;
    ASSERT(trellis_identity_generate(&alice_id) == TRELLIS_OK);
    ASSERT(trellis_identity_generate(&bob_id) == TRELLIS_OK);

    trellis_handshake_t *alice = trellis_handshake_new(&alice_id, TRELLIS_ROLE_INITIATOR);
    trellis_handshake_t *bob   = trellis_handshake_new(&bob_id, TRELLIS_ROLE_RESPONDER);
    ASSERT(alice != NULL);
    ASSERT(bob != NULL);

    trellis_buf_t msg1 = {0}, msg2 = {0}, msg3 = {0};
    ASSERT(trellis_handshake_create_msg1(alice, &msg1) == TRELLIS_OK);
    ASSERT(trellis_handshake_state(alice) == TRELLIS_HS_AWAITING_MSG2);

    ASSERT(trellis_handshake_process_msg1(bob, msg1.data, msg1.len, &msg2) == TRELLIS_OK);
    ASSERT(trellis_handshake_state(bob) == TRELLIS_HS_AWAITING_MSG3);

    ASSERT(trellis_handshake_process_msg2(alice, msg2.data, msg2.len, &msg3) == TRELLIS_OK);
    ASSERT(trellis_handshake_state(alice) == TRELLIS_HS_COMPLETE);

    ASSERT(trellis_handshake_process_msg3(bob, msg3.data, msg3.len) == TRELLIS_OK);
    ASSERT(trellis_handshake_state(bob) == TRELLIS_HS_COMPLETE);

    uint8_t key_a[32], key_b[32];
    ASSERT(trellis_handshake_session_key(alice, key_a) == TRELLIS_OK);
    ASSERT(trellis_handshake_session_key(bob, key_b) == TRELLIS_OK);
    ASSERT(memcmp(key_a, key_b, 32) == 0);

    trellis_buf_free(&msg1);
    trellis_buf_free(&msg2);
    trellis_buf_free(&msg3);
    trellis_handshake_free(alice);
    trellis_handshake_free(bob);
    trellis_identity_destroy(&alice_id);
    trellis_identity_destroy(&bob_id);
}

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

static void test_ratchet_roundtrip(void) {
    trellis_identity_t alice_id, bob_id;
    ASSERT(trellis_identity_generate(&alice_id) == TRELLIS_OK);
    ASSERT(trellis_identity_generate(&bob_id) == TRELLIS_OK);

    trellis_handshake_t *hs_a, *hs_b;
    do_handshake(&alice_id, &bob_id, &hs_a, &hs_b);

    trellis_ratchet_t *ra = trellis_ratchet_init_from_handshake(hs_a);
    trellis_ratchet_t *rb = trellis_ratchet_init_from_handshake(hs_b);
    ASSERT(ra != NULL);
    ASSERT(rb != NULL);

    for (int i = 0; i < 10; i++) {
        char msg[64];
        snprintf(msg, sizeof(msg), "ratchet message %d", i);
        trellis_buf_t ct = {0}, pt = {0};

        ASSERT(trellis_ratchet_encrypt(ra, (const uint8_t *)msg, strlen(msg), &ct) == TRELLIS_OK);
        ASSERT(ct.len > 0);

        ASSERT(trellis_ratchet_decrypt(rb, ct.data, ct.len, &pt) == TRELLIS_OK);
        ASSERT(pt.len == strlen(msg));
        ASSERT(memcmp(pt.data, msg, strlen(msg)) == 0);

        trellis_buf_free(&ct);
        trellis_buf_free(&pt);
    }

    trellis_ratchet_stats_t stats = trellis_ratchet_stats(ra);
    ASSERT(stats.messages_sent == 10);

    trellis_ratchet_free(ra);
    trellis_ratchet_free(rb);
    trellis_handshake_free(hs_a);
    trellis_handshake_free(hs_b);
    trellis_identity_destroy(&alice_id);
    trellis_identity_destroy(&bob_id);
}

static void test_ratchet_pqc_step(void) {
    trellis_identity_t alice_id, bob_id;
    ASSERT(trellis_identity_generate(&alice_id) == TRELLIS_OK);
    ASSERT(trellis_identity_generate(&bob_id) == TRELLIS_OK);

    trellis_handshake_t *hs_a, *hs_b;
    do_handshake(&alice_id, &bob_id, &hs_a, &hs_b);

    trellis_ratchet_t *ra = trellis_ratchet_init_from_handshake(hs_a);
    trellis_ratchet_t *rb = trellis_ratchet_init_from_handshake(hs_b);
    ASSERT(ra != NULL && rb != NULL);

    int count = TRELLIS_RATCHET_PQC_INTERVAL + 5;
    for (int i = 0; i < count; i++) {
        char msg[64];
        snprintf(msg, sizeof(msg), "pqc msg %d", i);
        trellis_buf_t ct = {0}, pt = {0};

        ASSERT(trellis_ratchet_encrypt(ra, (const uint8_t *)msg, strlen(msg), &ct) == TRELLIS_OK);
        ASSERT(trellis_ratchet_decrypt(rb, ct.data, ct.len, &pt) == TRELLIS_OK);
        ASSERT(pt.len == strlen(msg));
        ASSERT(memcmp(pt.data, msg, strlen(msg)) == 0);

        trellis_buf_free(&ct);
        trellis_buf_free(&pt);
    }

    trellis_ratchet_stats_t stats = trellis_ratchet_stats(ra);
    ASSERT(stats.messages_sent == (uint64_t)count);
    ASSERT(stats.pqc_ratchet_steps >= 1);

    trellis_ratchet_free(ra);
    trellis_ratchet_free(rb);
    trellis_handshake_free(hs_a);
    trellis_handshake_free(hs_b);
    trellis_identity_destroy(&alice_id);
    trellis_identity_destroy(&bob_id);
}

static void test_ratchet_replay_rejected(void) {
    trellis_identity_t alice_id, bob_id;
    ASSERT(trellis_identity_generate(&alice_id) == TRELLIS_OK);
    ASSERT(trellis_identity_generate(&bob_id) == TRELLIS_OK);

    trellis_handshake_t *hs_a, *hs_b;
    do_handshake(&alice_id, &bob_id, &hs_a, &hs_b);

    trellis_ratchet_t *ra = trellis_ratchet_init_from_handshake(hs_a);
    trellis_ratchet_t *rb = trellis_ratchet_init_from_handshake(hs_b);
    ASSERT(ra != NULL && rb != NULL);

    trellis_buf_t ct0 = {0}, ct1 = {0}, pt = {0};

    ASSERT(trellis_ratchet_encrypt(ra, (const uint8_t *)"msg0", 4, &ct0) == TRELLIS_OK);
    ASSERT(trellis_ratchet_encrypt(ra, (const uint8_t *)"msg1", 4, &ct1) == TRELLIS_OK);

    ASSERT(trellis_ratchet_decrypt(rb, ct0.data, ct0.len, &pt) == TRELLIS_OK);
    trellis_buf_free(&pt);

    ASSERT(trellis_ratchet_decrypt(rb, ct1.data, ct1.len, &pt) == TRELLIS_OK);
    trellis_buf_free(&pt);

    // Replaying ct0 (counter 0) must fail -- receiver expects counter >= 2.
    trellis_err_t err = trellis_ratchet_decrypt(rb, ct0.data, ct0.len, &pt);
    ASSERT(err != TRELLIS_OK);

    trellis_buf_free(&ct0);
    trellis_buf_free(&ct1);
    trellis_ratchet_free(ra);
    trellis_ratchet_free(rb);
    trellis_handshake_free(hs_a);
    trellis_handshake_free(hs_b);
    trellis_identity_destroy(&alice_id);
    trellis_identity_destroy(&bob_id);
}

static void test_ratchet_sequential_still_works(void) {
    trellis_identity_t alice_id, bob_id;
    ASSERT(trellis_identity_generate(&alice_id) == TRELLIS_OK);
    ASSERT(trellis_identity_generate(&bob_id) == TRELLIS_OK);

    trellis_handshake_t *hs_a, *hs_b;
    do_handshake(&alice_id, &bob_id, &hs_a, &hs_b);

    trellis_ratchet_t *ra = trellis_ratchet_init_from_handshake(hs_a);
    trellis_ratchet_t *rb = trellis_ratchet_init_from_handshake(hs_b);
    ASSERT(ra != NULL && rb != NULL);

    for (int i = 0; i < 50; i++) {
        char msg[64];
        snprintf(msg, sizeof(msg), "seq msg %d", i);
        trellis_buf_t ct = {0}, pt = {0};

        ASSERT(trellis_ratchet_encrypt(ra, (const uint8_t *)msg, strlen(msg), &ct) == TRELLIS_OK);
        ASSERT(trellis_ratchet_decrypt(rb, ct.data, ct.len, &pt) == TRELLIS_OK);
        ASSERT(pt.len == strlen(msg));
        ASSERT(memcmp(pt.data, msg, strlen(msg)) == 0);

        trellis_buf_free(&ct);
        trellis_buf_free(&pt);
    }

    trellis_ratchet_free(ra);
    trellis_ratchet_free(rb);
    trellis_handshake_free(hs_a);
    trellis_handshake_free(hs_b);
    trellis_identity_destroy(&alice_id);
    trellis_identity_destroy(&bob_id);
}

int main(void) {
    printf("=== Test Suite: Crypto ===\n");

    ASSERT(trellis_crypto_init() == TRELLIS_OK);

    RUN_TEST(test_shake256);
    RUN_TEST(test_hkdf_shake256);
    RUN_TEST(test_kmac256);
    RUN_TEST(test_aes256gcm_roundtrip);
    RUN_TEST(test_aes256gcm_tamper);
    RUN_TEST(test_identity_generate);
    RUN_TEST(test_identity_fingerprint);
    RUN_TEST(test_identity_sign_verify);
    RUN_TEST(test_identity_backup_sign_verify);
    RUN_TEST(test_identity_serialize_roundtrip);
    RUN_TEST(test_kem_roundtrip);
    RUN_TEST(test_handshake_3step);
    RUN_TEST(test_ratchet_roundtrip);
    RUN_TEST(test_ratchet_pqc_step);
    RUN_TEST(test_ratchet_replay_rejected);
    RUN_TEST(test_ratchet_sequential_still_works);

    trellis_crypto_cleanup();

    printf("=== All crypto tests passed ===\n");
    return 0;
}
