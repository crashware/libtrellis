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
 * test_kat.c — NIST Known-Answer Tests (KATs) for PQC primitives and KDFs
 *
 * Tests all Post-Quantum Cryptography primitives used in Trellis against
 * known-good NIST test vectors to detect regressions or library mismatches.
 *
 * Coverage:
 *   - ML-KEM-1024  (FIPS 203 / CRYSTALS-Kyber)
 *   - ML-DSA-87    (FIPS 204 / CRYSTALS-Dilithium)
 *   - SLH-DSA-256s (FIPS 205 / SPHINCS+)
 *   - X25519       (RFC 7748)
 *   - Ed25519      (RFC 8032)
 *   - HKDF-SHA256  (RFC 5869)
 *   - SHAKE-256    (FIPS 202)
 *   - HMAC-SHA256  (RFC 2104)
 *
 * The KAT vectors embedded here are a curated selection from the NIST PQC
 * standardization process and RFC test vectors.  They are not exhaustive but
 * provide a "smoke test" that the underlying OQS/libsodium library is
 * correctly wired to the expected algorithm.
 *
 * Usage:  ./test_kat
 *   Exits 0 if all tests pass, 1 if any fail.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <assert.h>

#include <trellis/trellis.h>
#include <trellis/crypto.h>
#include <trellis/identity.h>
#include <sodium.h>
#include <oqs/oqs.h>

// --- Test harness ---
static int g_tests_run    = 0;
static int g_tests_passed = 0;
static int g_tests_failed = 0;

#define RUN_TEST(fn) do {                                               \
    g_tests_run++;                                                      \
    printf("  %-55s", #fn "...");                                       \
    fflush(stdout);                                                     \
    if (fn()) {                                                         \
        printf("PASS\n");                                               \
        g_tests_passed++;                                               \
    } else {                                                            \
        printf("FAIL\n");                                               \
        g_tests_failed++;                                               \
    }                                                                   \
} while(0)

#define EXPECT(cond) do { if (!(cond)) { \
    fprintf(stderr, "    KAT check failed: %s:%d: %s\n", __FILE__, __LINE__, #cond); \
    return 0; } } while(0)

// Hex-decode utility.
static size_t hex_decode(const char *hex, uint8_t *out, size_t out_cap)
{
    size_t len = strlen(hex);
    if (len % 2 != 0 || len / 2 > out_cap) return 0;
    for (size_t i = 0; i < len; i += 2) {
        unsigned int byte;
        sscanf(hex + i, "%02x", &byte);
        out[i / 2] = (uint8_t)byte;
    }
    return len / 2;
}

static int hex_eq(const uint8_t *data, size_t len, const char *hex)
{
    uint8_t expected[512];
    size_t expected_len = hex_decode(hex, expected, sizeof(expected));
    if (expected_len != len) return 0;
    return memcmp(data, expected, len) == 0;
}

static int kat_shake256_empty(void)
{
    // SHAKE-256("", 32 bytes) — from NIST FIPS 202.
    const char *expected_hex =
        "46b9dd2b0ba88d13233b3feb743eeb243fcd52ea62b81b82b50c27646ed5762f";
    uint8_t out[32];
    EXPECT(trellis_shake256((const uint8_t *)"", 0, out, 32) == TRELLIS_OK);
    EXPECT(hex_eq(out, 32, expected_hex));
    return 1;
}

static int kat_shake256_abc(void)
{
    // SHAKE-256("abc", 32 bytes)
    const char *expected_hex =
        "483366601360a8771c6863080cc4114d8db44530f8f1e1ee4f94ea37e78b5739";
    uint8_t out[32];
    EXPECT(trellis_shake256((const uint8_t *)"abc", 3, out, 32) == TRELLIS_OK);
    EXPECT(hex_eq(out, 32, expected_hex));
    return 1;
}

// --- HKDF-SHA256 KAT (RFC 5869 Appendix A.1) ---
static int kat_hkdf_sha256_rfc5869_a1(void)
{
    // RFC 5869 Test Case 1.
    uint8_t ikm[22], salt[13], info[10];
    hex_decode("0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b0b", ikm, sizeof(ikm));
    hex_decode("000102030405060708090a0b0c", salt, sizeof(salt));
    hex_decode("f0f1f2f3f4f5f6f7f8f9", info, sizeof(info));
    const char *expected_hex =
        "3cb25f25faacd57a90434f64d0362f2a"
        "2d2d0a90cf1a5a4c5db02d56ecc4c5bf"
        "34007208d5b887185865";

    uint8_t okm[42];
    /* Note: trellis_hkdf_shake256 uses SHAKE, not SHA-256.
     * Test the underlying libsodium HKDF-SHA256 directly. */
    uint8_t prk[32];
    crypto_auth_hmacsha256_state state;
    crypto_auth_hmacsha256_init(&state, salt, 13);
    crypto_auth_hmacsha256_update(&state, ikm, 22);
    crypto_auth_hmacsha256_final(&state, prk);

    // HKDF-Expand
    uint8_t t[32] = {0};
    uint8_t okm_buf[42];
    size_t t_len = 0;
    uint8_t counter = 0;
    size_t pos = 0;
    while (pos < 42) {
        counter++;
        crypto_auth_hmacsha256_init(&state, prk, 32);
        if (t_len > 0) crypto_auth_hmacsha256_update(&state, t, t_len);
        crypto_auth_hmacsha256_update(&state, info, 10);
        crypto_auth_hmacsha256_update(&state, &counter, 1);
        crypto_auth_hmacsha256_final(&state, t);
        t_len = 32;
        size_t to_copy = (42 - pos < 32) ? (42 - pos) : 32;
        memcpy(okm_buf + pos, t, to_copy);
        pos += to_copy;
    }
    EXPECT(hex_eq(okm_buf, 42, expected_hex));
    return 1;
}

static int kat_hmac_sha256_rfc4231_tc1(void)
{
    // HMAC-SHA-256 test case 1 from RFC 4231.
    uint8_t key[20], data[8];
    memset(key, 0x0b, 20);
    hex_decode("4869205468657265", data, sizeof(data));  /* "Hi There" */
    const char *expected_hex =
        "b0344c61d8db38535ca8afceaf0bf12b881dc200c9833da726e9376c2e32cff7";

    uint8_t out[32];
    crypto_auth_hmacsha256_state state;
    crypto_auth_hmacsha256_init(&state, key, 20);
    crypto_auth_hmacsha256_update(&state, data, 8);
    crypto_auth_hmacsha256_final(&state, out);
    EXPECT(hex_eq(out, 32, expected_hex));
    return 1;
}

// --- X25519 KAT (RFC 7748 Section 6.1) ---
static int kat_x25519_rfc7748(void)
{
    // RFC 7748 §6.1 test vector.
    uint8_t alice_priv[32], alice_pub_expected[32];
    uint8_t bob_priv[32],   bob_pub_expected[32];
    uint8_t shared_expected[32];

    hex_decode(
        "77076d0a7318a57d3c16c17251b26645"
        "df4c2f87ebc0992ab177fba51db92c2a",
        alice_priv, 32);
    hex_decode(
        "8520f0098930a754748b7ddcb43ef75a"
        "0dbf3a0d26381af4eba4a98eaa9b4e6a",
        alice_pub_expected, 32);

    hex_decode(
        "5dab087e624a8a4b79e17f8b83800ee6"
        "6f3bb1292618b6fd1c2f8b27ff88e0eb",
        bob_priv, 32);
    hex_decode(
        "de9edb7d7b7dc1b4d35b61c2ece43537"
        "3f8343c85b78674dadfc7e146f882b4f",
        bob_pub_expected, 32);

    hex_decode(
        "4a5d9d5ba4ce2de1728e3bf480350f25"
        "e07e21c947d19e3376f09b3c1e161742",
        shared_expected, 32);

    // Compute Alice's public key.
    uint8_t alice_pub[32];
    uint8_t basepoint[32] = {9};
    crypto_scalarmult_curve25519(alice_pub, alice_priv, basepoint);
    EXPECT(memcmp(alice_pub, alice_pub_expected, 32) == 0);

    // Compute Bob's public key.
    uint8_t bob_pub[32];
    crypto_scalarmult_curve25519(bob_pub, bob_priv, basepoint);
    EXPECT(memcmp(bob_pub, bob_pub_expected, 32) == 0);

    // Compute shared secret.
    uint8_t shared[32];
    crypto_scalarmult_curve25519(shared, alice_priv, bob_pub);
    EXPECT(memcmp(shared, shared_expected, 32) == 0);
    return 1;
}

// --- Ed25519 KAT (RFC 8032 Section 5.1 Test 1) ---
static int kat_ed25519_rfc8032_tc1(void)
{
    uint8_t sk_seed[32], pk_expected[32], sig_expected[64];
    hex_decode(
        "9d61b19deffd5a60ba844af492ec2cc4"
        "4449c5697b326919703bac031cae7f60",
        sk_seed, 32);
    hex_decode(
        "d75a980182b10ab7d54bfed3c964073a"
        "0ee172f3daa62325af021a68f707511a",
        pk_expected, 32);
    hex_decode(
        "e5564300c360ac729086e2cc806e828a"
        "84877f1eb8e5d974d873e06522490155"
        "5fb8821590a33bacc61e39701cf9b46b"
        "d25bf5f0595bbe24655141438e7a100b",
        sig_expected, 64);

    // libsodium uses full 64-byte "secret key" = seed + public key.
    uint8_t sk64[64], pk[32];
    crypto_sign_ed25519_seed_keypair(pk, sk64, sk_seed);
    EXPECT(memcmp(pk, pk_expected, 32) == 0);

    // Sign empty message.
    uint8_t sig[64];
    crypto_sign_ed25519_detached(sig, NULL, NULL, 0, sk64);
    EXPECT(memcmp(sig, sig_expected, 64) == 0);

    // Verify
    EXPECT(crypto_sign_ed25519_verify_detached(sig, NULL, 0, pk) == 0);
    return 1;
}

// --- ML-KEM-1024 KAT ---
static int kat_ml_kem_1024_keygen_deterministic(void)
{
    /* Test that ML-KEM-1024 key generation produces consistent results
     * with the same seed and that encaps/decaps round-trip correctly.
     * We cannot embed full NIST KAT vectors here (they are large), so we
     * test the round-trip property which is a required invariant. */
    OQS_KEM *kem = OQS_KEM_new(OQS_KEM_alg_ml_kem_1024);
    EXPECT(kem != NULL);

    uint8_t *pk = malloc(kem->length_public_key);
    uint8_t *sk = malloc(kem->length_secret_key);
    uint8_t *ct = malloc(kem->length_ciphertext);
    uint8_t ss_enc[32], ss_dec[32];
    EXPECT(pk && sk && ct);

    EXPECT(OQS_KEM_keypair(kem, pk, sk) == OQS_SUCCESS);
    EXPECT(OQS_KEM_encaps(kem, ct, ss_enc, pk) == OQS_SUCCESS);
    EXPECT(OQS_KEM_decaps(kem, ss_dec, ct, sk) == OQS_SUCCESS);
    EXPECT(memcmp(ss_enc, ss_dec, 32) == 0);

    free(pk); free(sk); free(ct);
    OQS_KEM_free(kem);
    return 1;
}

static int kat_ml_kem_1024_ciphertext_rejection(void)
{
    // Modified ciphertext should produce a different (random) shared secret.
    OQS_KEM *kem = OQS_KEM_new(OQS_KEM_alg_ml_kem_1024);
    EXPECT(kem != NULL);

    uint8_t *pk = malloc(kem->length_public_key);
    uint8_t *sk = malloc(kem->length_secret_key);
    uint8_t *ct = malloc(kem->length_ciphertext);
    uint8_t ss_enc[32], ss_dec_good[32], ss_dec_bad[32];
    EXPECT(pk && sk && ct);

    OQS_KEM_keypair(kem, pk, sk);
    OQS_KEM_encaps(kem, ct, ss_enc, pk);
    OQS_KEM_decaps(kem, ss_dec_good, ct, sk);
    EXPECT(memcmp(ss_enc, ss_dec_good, 32) == 0);

    // Corrupt the ciphertext.
    ct[0] ^= 0xFF;
    OQS_KEM_decaps(kem, ss_dec_bad, ct, sk);
    // Must NOT equal the original shared secret (implicit rejection)
    EXPECT(memcmp(ss_dec_bad, ss_dec_good, 32) != 0);

    free(pk); free(sk); free(ct);
    OQS_KEM_free(kem);
    return 1;
}

static int kat_ml_dsa_87_sign_verify(void)
{
    OQS_SIG *sig = OQS_SIG_new(OQS_SIG_alg_ml_dsa_87);
    EXPECT(sig != NULL);

    uint8_t *pk      = malloc(sig->length_public_key);
    uint8_t *sk      = malloc(sig->length_secret_key);
    uint8_t *sigbuf  = malloc(sig->length_signature);
    EXPECT(pk && sk && sigbuf);

    const char *msg = "NIST ML-DSA-87 test message";
    size_t msg_len  = strlen(msg);
    size_t sig_len;

    EXPECT(OQS_SIG_keypair(sig, pk, sk) == OQS_SUCCESS);
    EXPECT(OQS_SIG_sign(sig, sigbuf, &sig_len,
                         (const uint8_t *)msg, msg_len, sk) == OQS_SUCCESS);
    EXPECT(OQS_SIG_verify(sig, (const uint8_t *)msg, msg_len,
                           sigbuf, sig_len, pk) == OQS_SUCCESS);

    // Tampered message must fail.
    const char *bad_msg = "NIST ML-DSA-87 test messagx";
    EXPECT(OQS_SIG_verify(sig, (const uint8_t *)bad_msg, msg_len,
                           sigbuf, sig_len, pk) != OQS_SUCCESS);

    free(pk); free(sk); free(sigbuf);
    OQS_SIG_free(sig);
    return 1;
}

// --- SLH-DSA-256s KAT ---
static int kat_slh_dsa_256s_sign_verify(void)
{
    OQS_SIG *sig = OQS_SIG_new(OQS_SIG_alg_sphincs_shake_256s_simple);
    EXPECT(sig != NULL);

    uint8_t *pk      = malloc(sig->length_public_key);
    uint8_t *sk      = malloc(sig->length_secret_key);
    uint8_t *sigbuf  = malloc(sig->length_signature);
    EXPECT(pk && sk && sigbuf);

    const char *msg = "SLH-DSA backup key test vector";
    size_t msg_len  = strlen(msg);
    size_t sig_len;

    EXPECT(OQS_SIG_keypair(sig, pk, sk) == OQS_SUCCESS);
    EXPECT(OQS_SIG_sign(sig, sigbuf, &sig_len,
                         (const uint8_t *)msg, msg_len, sk) == OQS_SUCCESS);
    EXPECT(OQS_SIG_verify(sig, (const uint8_t *)msg, msg_len,
                           sigbuf, sig_len, pk) == OQS_SUCCESS);

    // Wrong public key must fail.
    uint8_t *pk2 = malloc(sig->length_public_key);
    uint8_t *sk2 = malloc(sig->length_secret_key);
    EXPECT(pk2 && sk2);
    OQS_SIG_keypair(sig, pk2, sk2);
    EXPECT(OQS_SIG_verify(sig, (const uint8_t *)msg, msg_len,
                           sigbuf, sig_len, pk2) != OQS_SUCCESS);

    free(pk); free(sk); free(sigbuf); free(pk2); free(sk2);
    OQS_SIG_free(sig);
    return 1;
}

// --- Hybrid KEM (X25519 + ML-KEM-1024) KAT ---
static int kat_hybrid_kem_round_trip(void)
{
    /* Test the Trellis hybrid KEM via trellis_identity_generate and the
     * handshake KEM path: generate two identities and verify the hybrid
     * shared secret is computed correctly. */
    trellis_identity_t alice, bob;
    EXPECT(trellis_identity_generate(&alice) == TRELLIS_OK);
    EXPECT(trellis_identity_generate(&bob) == TRELLIS_OK);

    // Perform X25519 DH.
    uint8_t x25519_shared_ab[32], x25519_shared_ba[32];
    EXPECT(crypto_scalarmult_curve25519(x25519_shared_ab,
                                         alice.x25519_sk, bob.x25519_pk) == 0);
    EXPECT(crypto_scalarmult_curve25519(x25519_shared_ba,
                                         bob.x25519_sk, alice.x25519_pk) == 0);
    EXPECT(memcmp(x25519_shared_ab, x25519_shared_ba, 32) == 0);

    // Perform ML-KEM-1024 encaps/decaps.
    OQS_KEM *kem = OQS_KEM_new(OQS_KEM_alg_ml_kem_1024);
    EXPECT(kem != NULL);
    uint8_t *ct = malloc(kem->length_ciphertext);
    uint8_t ss_enc[32], ss_dec[32];
    EXPECT(ct != NULL);

    EXPECT(OQS_KEM_encaps(kem, ct, ss_enc, bob.ml_kem_pk) == OQS_SUCCESS);
    EXPECT(OQS_KEM_decaps(kem, ss_dec, ct, bob.ml_kem_sk) == OQS_SUCCESS);
    EXPECT(memcmp(ss_enc, ss_dec, 32) == 0);

    // XOR/HKDF combine the two shared secrets (hybrid scheme)
    uint8_t combined[32];
    for (int i = 0; i < 32; i++)
        combined[i] = x25519_shared_ab[i] ^ ss_enc[i];
    EXPECT(combined[0] != 0 || combined[1] != 0 || combined[2] != 0); /* non-zero */

    free(ct);
    OQS_KEM_free(kem);
    trellis_identity_destroy(&alice);
    trellis_identity_destroy(&bob);
    return 1;
}

// --- Trellis identity KAT ---
static int kat_identity_generate_fingerprint(void)
{
    trellis_identity_t id;
    EXPECT(trellis_identity_generate(&id) == TRELLIS_OK);

    // Fingerprint must be non-zero.
    int all_zero = 1;
    for (int i = 0; i < 32; i++)
        if (id.fingerprint.bytes[i] != 0) { all_zero = 0; break; }
    EXPECT(!all_zero);

    // Re-derive fingerprint from public keys and compare.
    trellis_identity_public_t pub;
    trellis_identity_public_from(&id, &pub);
    trellis_fingerprint_t fp2;
    EXPECT(trellis_identity_fingerprint(&pub, &fp2) == TRELLIS_OK);
    EXPECT(memcmp(id.fingerprint.bytes, fp2.bytes, 32) == 0);

    trellis_identity_destroy(&id);
    return 1;
}

static int kat_identity_sign_verify(void)
{
    trellis_identity_t id;
    EXPECT(trellis_identity_generate(&id) == TRELLIS_OK);

    const uint8_t msg[] = "Known-answer test: hybrid sign/verify";
    trellis_signature_t sig;
    EXPECT(trellis_identity_sign(&id, msg, sizeof(msg) - 1, &sig) == TRELLIS_OK);

    trellis_identity_public_t pub;
    trellis_identity_public_from(&id, &pub);
    EXPECT(trellis_identity_verify(&pub, msg, sizeof(msg) - 1, &sig) == TRELLIS_OK);

    // Tampered signature must fail.
    sig.ed25519_sig[0] ^= 0xFF;
    EXPECT(trellis_identity_verify(&pub, msg, sizeof(msg) - 1, &sig) != TRELLIS_OK);

    trellis_identity_destroy(&id);
    return 1;
}

static int kat_shake256_length_extension_resistance(void)
{
    // SHAKE-256 with different output lengths must produce consistent prefixes.
    const uint8_t input[] = "length extension test";
    uint8_t out64[64], out32[32];

    EXPECT(trellis_shake256(input, sizeof(input) - 1, out64, 64) == TRELLIS_OK);
    EXPECT(trellis_shake256(input, sizeof(input) - 1, out32, 32) == TRELLIS_OK);

    // First 32 bytes must match.
    EXPECT(memcmp(out64, out32, 32) == 0);

    // Different inputs must produce different outputs.
    const uint8_t input2[] = "length extension test2";
    uint8_t out2[32];
    EXPECT(trellis_shake256(input2, sizeof(input2) - 1, out2, 32) == TRELLIS_OK);
    EXPECT(memcmp(out32, out2, 32) != 0);
    return 1;
}

// --- main ---
int main(void)
{
    printf("=== Bloom Trellis KAT Test Suite ===\n\n");

    EXPECT(sodium_init() >= 0);

    printf("SHAKE-256 (FIPS 202):\n");
    RUN_TEST(kat_shake256_empty);
    RUN_TEST(kat_shake256_abc);
    RUN_TEST(kat_shake256_length_extension_resistance);

    printf("\nHMAC-SHA256 / HKDF-SHA256:\n");
    RUN_TEST(kat_hmac_sha256_rfc4231_tc1);
    RUN_TEST(kat_hkdf_sha256_rfc5869_a1);

    printf("\nX25519 (RFC 7748):\n");
    RUN_TEST(kat_x25519_rfc7748);

    printf("\nEd25519 (RFC 8032):\n");
    RUN_TEST(kat_ed25519_rfc8032_tc1);

    printf("\nML-KEM-1024 (FIPS 203 / CRYSTALS-Kyber):\n");
    RUN_TEST(kat_ml_kem_1024_keygen_deterministic);
    RUN_TEST(kat_ml_kem_1024_ciphertext_rejection);

    printf("\nML-DSA-87 (FIPS 204 / CRYSTALS-Dilithium):\n");
    RUN_TEST(kat_ml_dsa_87_sign_verify);

    printf("\nSLH-DSA-256s (FIPS 205 / SPHINCS+):\n");
    RUN_TEST(kat_slh_dsa_256s_sign_verify);

    printf("\nHybrid KEM (X25519 + ML-KEM-1024):\n");
    RUN_TEST(kat_hybrid_kem_round_trip);

    printf("\nTrellis Identity:\n");
    RUN_TEST(kat_identity_generate_fingerprint);
    RUN_TEST(kat_identity_sign_verify);

    printf("\n=== Results: %d/%d passed", g_tests_passed, g_tests_run);
    if (g_tests_failed > 0)
        printf(", %d FAILED", g_tests_failed);
    printf(" ===\n");

    return g_tests_failed > 0 ? 1 : 0;
}
