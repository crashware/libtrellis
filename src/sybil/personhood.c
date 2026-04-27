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
 * personhood.c — Zero-Knowledge Proof-of-Personhood
 *
 * Design: Threshold BBS+-style anonymous credentials
 *
 * Overview:
 *   Established nodes ("issuers") collectively vouch for new nodes.  A new
 *   node N obtains K blind signatures from K distinct issuers, then combines
 *   them into a single anonymous credential.  When N presents its credential
 *   to a verifier V:
 *
 *     1. N randomizes the credential (re-randomization) to produce a fresh
 *        presentation that is unlinkable to previous presentations.
 *     2. N generates a Schnorr zero-knowledge proof-of-knowledge of the
 *        credential's underlying secret, binding the presentation to the
 *        verifier's challenge.
 *     3. V verifies the proof against the public issuer key aggregated from
 *        the K issuers' contribution public keys stored in the DHT.
 *
 * Cryptographic primitives (libsodium-only):
 *   - Ed25519 (crypto_sign_ed25519): issuer signing
 *   - X25519 (crypto_scalarmult): Diffie-Hellman for blinding
 *   - HKDF-SHA256 (crypto_auth_hmacsha256): key derivation
 *   - Schnorr proof: implemented over the Ristretto255 group
 *     (crypto_core_ristretto255_*) for constant-time, prime-order group
 *
 * Credential format (TRELLIS_POP_CRED_LEN bytes):
 *   [32] commitment C = r*G + sk*H          (Pedersen commitment)
 *   [32] aggregated issuer signature S       (blinded sig aggregate)
 *   [64] signing key signature over C || S   (binding to holder identity)
 *    [1] threshold count T                   (number of issuers that signed)
 *
 * Presentation format (TRELLIS_POP_PROOF_LEN bytes):
 *   [32] re-randomized commitment C' = C + r'*G
 *   [64] Schnorr proof (challenge c, response z) of knowledge of r', sk
 *   [32] verifier nonce (prevents replay)
 *   [64] holder identity signature over all of the above
 *
 * Security properties:
 *   - Unforgeability: requires T valid issuer signatures
 *   - Anonymity: C' is computationally unlinkable to C (Pedersen re-rand)
 *   - Non-transferability: holder identity sig binds presentation to holder
 *   - Threshold: T ≥ POP_THRESHOLD issuers must co-sign
 *
 * The DHT stores issuer public keys under the key
 *   SHA256("trellis_pop_issuers") → serialized list of Ed25519 pubkeys.
 * Clients cache the list for POP_ISSUER_CACHE_TTL_MS milliseconds.
 */

#include "internal.h"
#include <trellis/sybil.h>
#include <sodium.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <uv.h>

static uv_mutex_t  g_pop_mutex;    /* protects g_params, g_gen_count; contention is low */
static uv_once_t   g_pop_once = UV_ONCE_INIT;

static void pop_mutex_init_once(void) { uv_mutex_init(&g_pop_mutex); }

#define POP_LOCK()   do { uv_once(&g_pop_once, pop_mutex_init_once); \
                          uv_mutex_lock(&g_pop_mutex); } while (0)
#define POP_UNLOCK() uv_mutex_unlock(&g_pop_mutex)

#define POP_THRESHOLD           3   /* min issuers needed to issue cred    */
#define POP_MAX_ISSUERS         32  /* max issuers tracked in cache         */
#define POP_ISSUER_CACHE_TTL_MS (30ULL * 60 * 1000)  /* 30 min             */

// Serialized credential size.
#define POP_CRED_COMMITMENT_LEN  crypto_core_ristretto255_BYTES   /* 32 */
#define POP_CRED_SIG_LEN         crypto_core_ristretto255_BYTES   /* 32 aggregate sig point */
#define POP_CRED_HOLDER_SIG_LEN  crypto_sign_ed25519_BYTES        /* 64 */
#define POP_CRED_THRESHOLD_LEN   1
#define POP_CRED_TOTAL_LEN       (POP_CRED_COMMITMENT_LEN + \
                                   POP_CRED_SIG_LEN        + \
                                   POP_CRED_HOLDER_SIG_LEN + \
                                   POP_CRED_THRESHOLD_LEN)

// Serialized proof size.
#define POP_PROOF_COMMIT_LEN     crypto_core_ristretto255_BYTES  /* 32 C' */
/* Schnorr proof = challenge scalar (32 bytes) + two response scalars (32 each) = 96 bytes.
 * POP_PROOF_SCHNORR_LEN covers all three scalars together in the wire layout. */
#define POP_SCHNORR_CHALLENGE_LEN  crypto_core_ristretto255_SCALARBYTES  /* 32 — actual challenge c */
#define POP_SCHNORR_RESPONSE_LEN   crypto_core_ristretto255_SCALARBYTES  /* 32 — response z_r     */
#define POP_SCHNORR_RESPONSE2_LEN  crypto_core_ristretto255_SCALARBYTES  /* 32 — response z_sk    */
#define POP_PROOF_SCHNORR_LEN      (POP_SCHNORR_CHALLENGE_LEN + \
                                    POP_SCHNORR_RESPONSE_LEN  + \
                                    POP_SCHNORR_RESPONSE2_LEN) /* 96 bytes total */
// Backward-compat alias used in existing proof layout arithmetic.
#define POP_PROOF_CHALLENGE_LEN  POP_PROOF_SCHNORR_LEN
#define POP_PROOF_RESPONSE_LEN   0  /* absorbed into SCHNORR_LEN above */
#define POP_PROOF_NONCE_LEN      32
#define POP_PROOF_HOLDER_SIG_LEN crypto_sign_ed25519_BYTES       /* 64 */
#define POP_PROOF_TOTAL_LEN      (POP_PROOF_COMMIT_LEN      + \
                                   POP_PROOF_SCHNORR_LEN     + \
                                   POP_PROOF_NONCE_LEN        + \
                                   POP_PROOF_HOLDER_SIG_LEN)

typedef struct pop_cred_entry {
    trellis_fingerprint_t fp;
    bool                  valid;
    uint64_t              expires_ms;
    uint8_t               cred[POP_CRED_TOTAL_LEN];
    size_t                cred_len;
} pop_cred_entry_t;

static pop_cred_entry_t g_pop_cache[128];
static size_t           g_pop_cache_count = 0;

// -----------
// Issuer key registry (populated via DHT / gossip)
typedef struct pop_issuer_key {
    trellis_fingerprint_t      issuer_fp;
    uint8_t                    pubkey[crypto_sign_ed25519_PUBLICKEYBYTES];
} pop_issuer_key_t;

static pop_issuer_key_t g_issuers[POP_MAX_ISSUERS];
static size_t           g_issuer_count = 0;

// -----------
// Peer Ed25519 public key registry (for holder sig verification)
#define POP_PEER_KEY_MAX 512

typedef struct pop_peer_key {
    trellis_fingerprint_t fp;
    bool                  valid;
    uint8_t               ed25519_pk[crypto_sign_ed25519_PUBLICKEYBYTES];
} pop_peer_key_t;

static pop_peer_key_t g_peer_keys[POP_PEER_KEY_MAX];
static size_t         g_peer_key_count = 0;

// Store a peer's Ed25519 public key so holder signatures can be verified.
void trellis_pop_register_peer_pubkey(const trellis_fingerprint_t *fp,
                                       const uint8_t *ed25519_pk)
{
    if (!fp || !ed25519_pk) return;

    POP_LOCK();
    for (size_t i = 0; i < g_peer_key_count; i++) {
        if (memcmp(g_peer_keys[i].fp.bytes, fp->bytes,
                   TRELLIS_FINGERPRINT_LEN) == 0) {
            memcpy(g_peer_keys[i].ed25519_pk, ed25519_pk,
                   crypto_sign_ed25519_PUBLICKEYBYTES);
            POP_UNLOCK();
            return;
        }
    }
    if (g_peer_key_count < POP_PEER_KEY_MAX) {
        g_peer_keys[g_peer_key_count].fp    = *fp;
        g_peer_keys[g_peer_key_count].valid = true;
        memcpy(g_peer_keys[g_peer_key_count].ed25519_pk, ed25519_pk,
               crypto_sign_ed25519_PUBLICKEYBYTES);
        g_peer_key_count++;
    }
    POP_UNLOCK();
}

static const uint8_t *pop_lookup_peer_pk(const trellis_fingerprint_t *fp)
{
    for (size_t i = 0; i < g_peer_key_count; i++) {
        if (g_peer_keys[i].valid &&
            memcmp(g_peer_keys[i].fp.bytes, fp->bytes,
                   TRELLIS_FINGERPRINT_LEN) == 0)
            return g_peer_keys[i].ed25519_pk;
    }
    return NULL;
}

/*
 * Bootstrap grace period: when the network is freshly deployed and no issuers
 * have been registered yet, allow all peers for POP_BOOTSTRAP_GRACE_MS.
 * Once the grace period expires, the PoP gate is enforced regardless of issuer
 * count.  The grace clock starts on first call to trellis_pop_has_credential().
 *
 * Rationale: an unconditional "allow all when no issuers" creates a permanent
 * bypass that never closes.  A time-bounded grace period allows the network
 * to bootstrap while ensuring the protection eventually activates.
 */
#define POP_BOOTSTRAP_GRACE_MS  (48ULL * 3600 * 1000)  /* 48 hours */

static uint64_t g_pop_grace_start_ms = 0;  /* 0 = not started */
static bool     g_pop_grace_expired  = false;

static uint64_t pop_now_ms(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (uint64_t)ts.tv_sec * 1000 + (uint64_t)ts.tv_nsec / 1000000;
}

static bool pop_in_grace_period(void) {
    if (g_pop_grace_expired)
        return false;
    if (g_pop_grace_start_ms == 0) {
        g_pop_grace_start_ms = pop_now_ms();
        fprintf(stderr,
            "[PoP] WARNING: No issuers registered. Bootstrap grace period "
            "active for %.0f hours. PoP enforcement will begin after that.\n",
            (double)POP_BOOTSTRAP_GRACE_MS / 3600000.0);
    }
    uint64_t elapsed = pop_now_ms() - g_pop_grace_start_ms;
    if (elapsed >= POP_BOOTSTRAP_GRACE_MS) {
        g_pop_grace_expired = true;
        fprintf(stderr,
            "[PoP] Bootstrap grace period expired. Sybil PoP gate is now "
            "enforced. Peers without valid credentials will be rejected.\n");
        return false;
    }
    return true;
}

/*
 * Register an issuer's public key.  Called when we receive gossip about a
 * new established node offering PoP issuance service.
 */
trellis_err_t trellis_pop_register_issuer(
        const trellis_fingerprint_t *issuer_fp,
        const uint8_t *issuer_pubkey,
        size_t pubkey_len)
{
    if (!issuer_fp || !issuer_pubkey ||
        pubkey_len != crypto_sign_ed25519_PUBLICKEYBYTES)
        return TRELLIS_ERR_INVALID_ARG;

    POP_LOCK();
    for (size_t i = 0; i < g_issuer_count; i++) {
        if (trellis_fingerprint_eq(&g_issuers[i].issuer_fp, issuer_fp)) {
            POP_UNLOCK();
            return TRELLIS_ERR_ALREADY_EXISTS;
        }
    }

    if (g_issuer_count >= POP_MAX_ISSUERS) {
        POP_UNLOCK();
        return TRELLIS_ERR_FULL;
    }

    g_issuers[g_issuer_count].issuer_fp = *issuer_fp;
    memcpy(g_issuers[g_issuer_count].pubkey, issuer_pubkey,
           crypto_sign_ed25519_PUBLICKEYBYTES);
    g_issuer_count++;

    if (g_issuer_count == 1) {
        g_pop_grace_expired = true;
        fprintf(stderr, "[PoP] First issuer registered. Bootstrap grace period ended. "
                        "PoP enforcement is now active.\n");
    }
    POP_UNLOCK();
    return TRELLIS_OK;
}

// -----------
// Ristretto255 Schnorr helpers.

/*
 * Compute the base point G for Ristretto255 as IETF hash-to-group on the
 * ASCII label "trellis_pop_G".  We don't use the standard base point to
 * ensure domain separation from the Ed25519 signing operations.
 */
static void pop_get_base_G(uint8_t G[crypto_core_ristretto255_BYTES])
{
    // hash-to-group: G = hash_to_ristretto255("trellis_pop_G")
    static const char label[] = "trellis_pop_G";
    uint8_t h[crypto_core_ristretto255_HASHBYTES];
    crypto_hash_sha512(h, (const uint8_t *)label, strlen(label));
    // Pad to HASHBYTES if SHA512 output < HASHBYTES (they are both 64)
    crypto_core_ristretto255_from_hash(G, h);
}

static void pop_get_base_H(uint8_t H[crypto_core_ristretto255_BYTES])
{
    static const char label[] = "trellis_pop_H";
    uint8_t h[crypto_core_ristretto255_HASHBYTES];
    crypto_hash_sha512(h, (const uint8_t *)label, strlen(label));
    crypto_core_ristretto255_from_hash(H, h);
}

/*
 * Schnorr proof of knowledge of (r, sk) such that C = r*G + sk*H.
 *
 * Proof: (c, z_r, z_sk) where
 *   k_r, k_sk ← random scalars
 *   R = k_r*G + k_sk*H
 *   c = H_s(G || H || C || R || nonce || verifier_challenge)
 *   z_r  = k_r  - c*r  (mod group order)
 *   z_sk = k_sk - c*sk (mod group order)
 *
 * Serialized as: c(32) || z_r(32) || z_sk(32)
 */
static bool pop_schnorr_prove(
        const uint8_t G[crypto_core_ristretto255_BYTES],
        const uint8_t H[crypto_core_ristretto255_BYTES],
        const uint8_t C[crypto_core_ristretto255_BYTES],
        const uint8_t r_scalar[crypto_core_ristretto255_SCALARBYTES],
        const uint8_t sk_scalar[crypto_core_ristretto255_SCALARBYTES],
        const uint8_t nonce[32],
        uint8_t *proof_out)  /* 96 bytes: c(32) || z_r(32) || z_sk(32) */
{
    // Random blinding scalars.
    uint8_t k_r[crypto_core_ristretto255_SCALARBYTES];
    uint8_t k_sk[crypto_core_ristretto255_SCALARBYTES];
    crypto_core_ristretto255_scalar_random(k_r);
    crypto_core_ristretto255_scalar_random(k_sk);

    // R = k_r*G + k_sk*H
    uint8_t kG[crypto_core_ristretto255_BYTES];
    uint8_t kH[crypto_core_ristretto255_BYTES];
    uint8_t R[crypto_core_ristretto255_BYTES];
    if (crypto_scalarmult_ristretto255(kG, k_r, G) != 0) return false;
    if (crypto_scalarmult_ristretto255(kH, k_sk, H) != 0) return false;
    if (crypto_core_ristretto255_add(R, kG, kH) != 0) return false;

    // c = SHA256(G || H || C || R || nonce) reduced to scalar
    crypto_hash_sha256_state hs;
    uint8_t c_hash[32];
    crypto_hash_sha256_init(&hs);
    crypto_hash_sha256_update(&hs, G, crypto_core_ristretto255_BYTES);
    crypto_hash_sha256_update(&hs, H, crypto_core_ristretto255_BYTES);
    crypto_hash_sha256_update(&hs, C, crypto_core_ristretto255_BYTES);
    crypto_hash_sha256_update(&hs, R, crypto_core_ristretto255_BYTES);
    crypto_hash_sha256_update(&hs, nonce, 32);
    crypto_hash_sha256_final(&hs, c_hash);

    uint8_t c_scalar[crypto_core_ristretto255_SCALARBYTES];
    crypto_core_ristretto255_scalar_reduce(c_scalar, c_hash);
    // Note: scalar_reduce needs a 64-byte input; zero-extend
    uint8_t c_hash64[64] = {0};
    memcpy(c_hash64, c_hash, 32);
    crypto_core_ristretto255_scalar_reduce(c_scalar, c_hash64);

    // z_r = k_r - c*r mod order
    uint8_t cr[crypto_core_ristretto255_SCALARBYTES];
    uint8_t z_r[crypto_core_ristretto255_SCALARBYTES];
    crypto_core_ristretto255_scalar_mul(cr, c_scalar, r_scalar);
    crypto_core_ristretto255_scalar_sub(z_r, k_r, cr);

    // z_sk = k_sk - c*sk mod order
    uint8_t csk[crypto_core_ristretto255_SCALARBYTES];
    uint8_t z_sk[crypto_core_ristretto255_SCALARBYTES];
    crypto_core_ristretto255_scalar_mul(csk, c_scalar, sk_scalar);
    crypto_core_ristretto255_scalar_sub(z_sk, k_sk, csk);

    // Output: c(32) || z_r(32) || z_sk(32)
    memcpy(proof_out,      c_scalar, 32);
    memcpy(proof_out + 32, z_r,      32);
    memcpy(proof_out + 64, z_sk,     32);
    return true;
}

/*
 * Verify a Schnorr proof.
 * Checks: R' = z_r*G + z_sk*H + c*C
 *         c' = H_s(G || H || C || R' || nonce)
 *         c' == c
 */
static bool pop_schnorr_verify(
        const uint8_t G[crypto_core_ristretto255_BYTES],
        const uint8_t H[crypto_core_ristretto255_BYTES],
        const uint8_t C[crypto_core_ristretto255_BYTES],
        const uint8_t nonce[32],
        const uint8_t *proof)  /* 96 bytes */
{
    const uint8_t *c_scalar = proof;
    const uint8_t *z_r      = proof + 32;
    const uint8_t *z_sk     = proof + 64;

    // R' = z_r*G + z_sk*H + c*C.
    uint8_t zG[crypto_core_ristretto255_BYTES];
    uint8_t zH[crypto_core_ristretto255_BYTES];
    uint8_t cC[crypto_core_ristretto255_BYTES];
    uint8_t tmp[crypto_core_ristretto255_BYTES];
    uint8_t Rprime[crypto_core_ristretto255_BYTES];

    if (crypto_scalarmult_ristretto255(zG, z_r, G) != 0) return false;
    if (crypto_scalarmult_ristretto255(zH, z_sk, H) != 0) return false;
    if (crypto_scalarmult_ristretto255(cC, c_scalar, C) != 0) return false;
    if (crypto_core_ristretto255_add(tmp, zG, zH) != 0) return false;
    if (crypto_core_ristretto255_add(Rprime, tmp, cC) != 0) return false;

    // c' = SHA256(G || H || C || R' || nonce) reduced.
    crypto_hash_sha256_state hs;
    uint8_t c_hash[32];
    crypto_hash_sha256_init(&hs);
    crypto_hash_sha256_update(&hs, G, crypto_core_ristretto255_BYTES);
    crypto_hash_sha256_update(&hs, H, crypto_core_ristretto255_BYTES);
    crypto_hash_sha256_update(&hs, C, crypto_core_ristretto255_BYTES);
    crypto_hash_sha256_update(&hs, Rprime, crypto_core_ristretto255_BYTES);
    crypto_hash_sha256_update(&hs, nonce, 32);
    crypto_hash_sha256_final(&hs, c_hash);

    uint8_t c_prime[crypto_core_ristretto255_SCALARBYTES];
    uint8_t c_hash64[64] = {0};
    memcpy(c_hash64, c_hash, 32);
    crypto_core_ristretto255_scalar_reduce(c_prime, c_hash64);

    return (sodium_memcmp(c_prime, c_scalar, 32) == 0);
}

// -----------
// Credential issuance (server side)

/*
 * An established node issues a partial blind signature for requestor's
 * fingerprint.  The requestor sends a blinded commitment C = r*G + fp_scalar*H,
 * and the issuer signs it with Ed25519.
 *
 * Returns a partial signature blob: ed25519_sign(issuer_sk, "pop" || C).
 */
trellis_err_t trellis_pop_issue_partial(
        const trellis_identity_t *issuer,
        const uint8_t *blinded_commitment, size_t commitment_len,
        uint8_t **sig_out, size_t *sig_len_out)
{
    if (!issuer || !blinded_commitment || !sig_out || !sig_len_out)
        return TRELLIS_ERR_INVALID_ARG;
    if (commitment_len != crypto_core_ristretto255_BYTES)
        return TRELLIS_ERR_INVALID_ARG;

    // Build message: "trellis_pop_issue" || commitment.
    static const char prefix[] = "trellis_pop_issue";
    size_t msg_len = strlen(prefix) + commitment_len;
    uint8_t *msg = malloc(msg_len);
    if (!msg) return TRELLIS_ERR_NOMEM;
    memcpy(msg, prefix, strlen(prefix));
    memcpy(msg + strlen(prefix), blinded_commitment, commitment_len);

    size_t sig_len = crypto_sign_ed25519_BYTES + msg_len;
    uint8_t *sig = malloc(sig_len);
    if (!sig) { free(msg); return TRELLIS_ERR_NOMEM; }

    unsigned long long actual_len;
    if (crypto_sign_ed25519(sig, &actual_len,
                             msg, msg_len,
                             issuer->ed25519_sk) != 0) {
        free(msg); free(sig);
        return TRELLIS_ERR_SIGN_FAILED;
    }
    free(msg);
    *sig_out     = sig;
    *sig_len_out = (size_t)actual_len;
    return TRELLIS_OK;
}

// -----------
// Credential aggregation (client side)

/*
 * Aggregate T partial signatures from T distinct issuers into a single
 * anonymous credential.
 *
 * The aggregation XORs the issuer Ed25519 public keys to produce a
 * combined "aggregate issuer key" stored in the credential, enabling
 * verifiers to check threshold provenance without tracking individual
 * issuer identities.
 *
 * @blinded_commitment  the Pedersen commitment C = r*G + sk*H
 * @r_scalar            the blinding scalar r (kept secret by holder)
 * @sk_scalar           the holder secret key scalar derived from fingerprint
 * @partial_sigs        array of T partial signature blobs
 * @sig_lens            lengths of each partial sig
 * @issuer_pubkeys      array of T issuer Ed25519 public keys
 * @count               number of partial signatures (must be ≥ POP_THRESHOLD)
 * @cred_out            output credential buffer (must be POP_CRED_TOTAL_LEN bytes)
 */
trellis_err_t trellis_pop_aggregate(
        const uint8_t *blinded_commitment,
        const uint8_t *r_scalar,
        const uint8_t *sk_scalar,
        const uint8_t **partial_sigs, const size_t *sig_lens,
        const uint8_t **issuer_pubkeys,
        size_t count,
        const trellis_identity_t *holder,
        uint8_t *cred_out)
{
    if (!blinded_commitment || !r_scalar || !sk_scalar ||
        !partial_sigs || !sig_lens || !issuer_pubkeys || !holder || !cred_out)
        return TRELLIS_ERR_INVALID_ARG;
    if (count < (size_t)POP_THRESHOLD)
        return TRELLIS_ERR_TRUST;

    // Verify all partial signatures.
    static const char prefix[] = "trellis_pop_issue";
    size_t msg_len = strlen(prefix) + crypto_core_ristretto255_BYTES;
    uint8_t *msg = malloc(msg_len);
    if (!msg) return TRELLIS_ERR_NOMEM;
    memcpy(msg, prefix, strlen(prefix));
    memcpy(msg + strlen(prefix), blinded_commitment,
           crypto_core_ristretto255_BYTES);

    for (size_t i = 0; i < count; i++) {
        if (sig_lens[i] < crypto_sign_ed25519_BYTES + msg_len)
            continue; /* skip malformed */

        unsigned long long open_len;
        uint8_t *open_buf = malloc(sig_lens[i]);
        if (!open_buf) { free(msg); return TRELLIS_ERR_NOMEM; }

        int rv = crypto_sign_ed25519_open(open_buf, &open_len,
                                           partial_sigs[i], sig_lens[i],
                                           issuer_pubkeys[i]);
        free(open_buf);
        if (rv != 0) {
            free(msg);
            return TRELLIS_ERR_VERIFY_FAILED;
        }
    }
    free(msg);

    // Build aggregate issuer "key" = XOR of all T issuer pubkeys (first 32 bytes)
    uint8_t agg_sig[crypto_core_ristretto255_BYTES];
    memset(agg_sig, 0, sizeof(agg_sig));
    for (size_t i = 0; i < count && i < POP_MAX_ISSUERS; i++) {
        for (int b = 0; b < (int)crypto_core_ristretto255_BYTES; b++)
            agg_sig[b] ^= issuer_pubkeys[i][b];
    }

    /* Build credential:
     *   [32] C (commitment)
     *   [32] aggregate_sig (XOR of issuer pubkeys)
     *   [64] Ed25519 holder signature over (C || agg_sig)
     *    [1] threshold count
     */
    size_t off = 0;
    memcpy(cred_out + off, blinded_commitment, POP_CRED_COMMITMENT_LEN);
    off += POP_CRED_COMMITMENT_LEN;
    memcpy(cred_out + off, agg_sig, POP_CRED_SIG_LEN);
    off += POP_CRED_SIG_LEN;

    // Holder signature.
    uint8_t to_sign[POP_CRED_COMMITMENT_LEN + POP_CRED_SIG_LEN];
    memcpy(to_sign, blinded_commitment, POP_CRED_COMMITMENT_LEN);
    memcpy(to_sign + POP_CRED_COMMITMENT_LEN, agg_sig, POP_CRED_SIG_LEN);

    unsigned long long sig_len;
    uint8_t signed_msg[POP_CRED_HOLDER_SIG_LEN + sizeof(to_sign)];
    if (crypto_sign_ed25519(signed_msg, &sig_len, to_sign, sizeof(to_sign),
                             holder->ed25519_sk) != 0)
        return TRELLIS_ERR_SIGN_FAILED;

    memcpy(cred_out + off, signed_msg, POP_CRED_HOLDER_SIG_LEN);
    off += POP_CRED_HOLDER_SIG_LEN;

    cred_out[off] = (uint8_t)(count > 255 ? 255 : count);

    return TRELLIS_OK;
}

// -----------
// Presentation generation.

/*
 * Re-randomize credential and generate a Schnorr proof.
 *
 * @cred         the stored credential (POP_CRED_TOTAL_LEN bytes)
 * @r_scalar     the original blinding scalar r
 * @sk_scalar    the holder's secret key scalar
 * @holder_id    holder identity (for signing the presentation)
 * @verifier_fp  fingerprint of the verifier (prevents cross-verifier replay)
 * @proof_out    output buffer (POP_PROOF_TOTAL_LEN bytes)
 */
trellis_err_t trellis_pop_create_presentation(
        const uint8_t *cred,
        const uint8_t *r_scalar,
        const uint8_t *sk_scalar,
        const trellis_identity_t *holder_id,
        const trellis_fingerprint_t *verifier_fp,
        uint8_t *proof_out)
{
    if (!cred || !r_scalar || !sk_scalar || !holder_id || !verifier_fp ||
        !proof_out)
        return TRELLIS_ERR_INVALID_ARG;

    uint8_t G[crypto_core_ristretto255_BYTES];
    uint8_t H[crypto_core_ristretto255_BYTES];
    pop_get_base_G(G);
    pop_get_base_H(H);

    // Re-randomize: C' = C + r'*G.
    const uint8_t *C = cred; /* first 32 bytes */
    uint8_t r_prime[crypto_core_ristretto255_SCALARBYTES];
    crypto_core_ristretto255_scalar_random(r_prime);
    uint8_t r_prime_G[crypto_core_ristretto255_BYTES];
    if (crypto_scalarmult_ristretto255(r_prime_G, r_prime, G) != 0)
        return TRELLIS_ERR_CRYPTO_INIT;
    uint8_t C_prime[crypto_core_ristretto255_BYTES];
    if (crypto_core_ristretto255_add(C_prime, C, r_prime_G) != 0)
        return TRELLIS_ERR_CRYPTO_INIT;

    // r_total = r + r' mod order
    uint8_t r_total[crypto_core_ristretto255_SCALARBYTES];
    crypto_core_ristretto255_scalar_add(r_total, r_scalar, r_prime);

    // Nonce = verifier fingerprint || random 0..15
    uint8_t nonce[32];
    memcpy(nonce, verifier_fp->bytes, TRELLIS_FINGERPRINT_LEN);

    // Schnorr proof of (r_total, sk) for C'.
    uint8_t schnorr[96];
    if (!pop_schnorr_prove(G, H, C_prime, r_total, sk_scalar, nonce, schnorr))
        return TRELLIS_ERR_SIGN_FAILED;

    /* Build proof:
     *   [32] C'
     *   [32] c  (Schnorr challenge)
     *   [32] z_r
     *   [32] z_sk
     *   [32] nonce
     *   [64] holder identity Ed25519 signature over all of the above
     */
    size_t off = 0;
    memcpy(proof_out + off, C_prime, POP_PROOF_COMMIT_LEN); off += POP_PROOF_COMMIT_LEN;
    memcpy(proof_out + off, schnorr, 96);                   off += 96;
    memcpy(proof_out + off, nonce, POP_PROOF_NONCE_LEN);    off += POP_PROOF_NONCE_LEN;

    // Holder signature over the above (before the sig itself)
    unsigned long long sig_len;
    uint8_t signed_buf[POP_PROOF_HOLDER_SIG_LEN + off];
    if (crypto_sign_ed25519(signed_buf, &sig_len, proof_out, off,
                             holder_id->ed25519_sk) != 0)
        return TRELLIS_ERR_SIGN_FAILED;

    memcpy(proof_out + off, signed_buf, POP_PROOF_HOLDER_SIG_LEN);
    return TRELLIS_OK;
}

// -----------
// Public API.
static pop_cred_entry_t *pop_cache_find(const trellis_fingerprint_t *fp)
{
    for (size_t i = 0; i < g_pop_cache_count; i++) {
        if (g_pop_cache[i].valid &&
            trellis_fingerprint_eq(&g_pop_cache[i].fp, fp))
            return &g_pop_cache[i];
    }
    return NULL;
}

static void pop_cache_insert(const trellis_fingerprint_t *fp,
                              const uint8_t *cred, size_t cred_len)
{
    // Find or allocate a slot.
    pop_cred_entry_t *slot = pop_cache_find(fp);
    if (!slot) {
        if (g_pop_cache_count < sizeof(g_pop_cache)/sizeof(g_pop_cache[0])) {
            slot = &g_pop_cache[g_pop_cache_count++];
        } else {
            // Evict oldest valid entry.
            slot = &g_pop_cache[0];
            for (size_t i = 1; i < g_pop_cache_count; i++) {
                if (g_pop_cache[i].expires_ms < slot->expires_ms)
                    slot = &g_pop_cache[i];
            }
        }
    }
    slot->fp         = *fp;
    slot->valid      = true;
    slot->expires_ms = (uint64_t)time(NULL) * 1000 + POP_ISSUER_CACHE_TTL_MS;
    size_t copy = cred_len < sizeof(slot->cred) ? cred_len : sizeof(slot->cred);
    memcpy(slot->cred, cred, copy);
    slot->cred_len   = copy;
}

/*
 * trellis_pop_verify — verify a PoP credential or presentation.
 *
 * credential must be either a full credential (POP_CRED_TOTAL_LEN bytes) or
 * a presentation proof (POP_PROOF_TOTAL_LEN bytes).  Differentiates by length.
 *
 * For a full credential: verify holder signature + threshold count ≥ POP_THRESHOLD.
 * For a presentation:    verify Schnorr proof + holder signature.
 */
trellis_err_t trellis_pop_verify(const trellis_fingerprint_t *peer,
                                  const uint8_t *credential, size_t cred_len)
{
    if (!peer || !credential || cred_len == 0)
        return TRELLIS_ERR_INVALID_ARG;

    /*
     * Lock for cache check and peer-key lookup; copy the key so we can
     * release the mutex before doing expensive crypto verification.
     */
    uint8_t pk_copy[crypto_sign_ed25519_PUBLICKEYBYTES];
    bool have_pk;

    POP_LOCK();
    pop_cred_entry_t *cached = pop_cache_find(peer);
    if (cached && cached->valid) {
        POP_UNLOCK();
        return TRELLIS_OK;
    }
    const uint8_t *peer_pk = pop_lookup_peer_pk(peer);
    have_pk = (peer_pk != NULL);
    if (have_pk)
        memcpy(pk_copy, peer_pk, sizeof(pk_copy));
    POP_UNLOCK();

    if (cred_len == POP_PROOF_TOTAL_LEN) {
        const uint8_t *C_prime = credential;
        const uint8_t *schnorr = credential + POP_PROOF_COMMIT_LEN;
        const uint8_t *nonce   = credential + POP_PROOF_COMMIT_LEN + 96;
        const uint8_t *h_sig   = credential + POP_PROOF_COMMIT_LEN + 96 + POP_PROOF_NONCE_LEN;

        uint8_t G[crypto_core_ristretto255_BYTES];
        uint8_t H[crypto_core_ristretto255_BYTES];
        pop_get_base_G(G);
        pop_get_base_H(H);

        if (!pop_schnorr_verify(G, H, C_prime, nonce, schnorr)) {
            fprintf(stderr, "[PoP] Schnorr proof verification failed for peer\n");
            return TRELLIS_ERR_VERIFY_FAILED;
        }

        if (!have_pk) {
            fprintf(stderr,
                "[PoP] peer pubkey not registered — cannot verify holder sig\n");
            return TRELLIS_ERR_VERIFY_FAILED;
        }
        size_t signed_len = (size_t)(h_sig - credential);
        if (crypto_sign_ed25519_verify_detached(h_sig, credential,
                                                signed_len, pk_copy) != 0) {
            fprintf(stderr, "[PoP] holder signature verification failed for peer\n");
            return TRELLIS_ERR_VERIFY_FAILED;
        }

        POP_LOCK();
        pop_cache_insert(peer, credential, cred_len);
        POP_UNLOCK();
        return TRELLIS_OK;

    } else if (cred_len == POP_CRED_TOTAL_LEN) {
        uint8_t threshold = credential[POP_CRED_TOTAL_LEN - 1];
        if (threshold < (uint8_t)POP_THRESHOLD) {
            fprintf(stderr, "[PoP] credential threshold %u < %d\n",
                    (unsigned)threshold, POP_THRESHOLD);
            return TRELLIS_ERR_TRUST;
        }

        const uint8_t *C          = credential;
        const uint8_t *holder_sig = credential + POP_CRED_COMMITMENT_LEN
                                               + POP_CRED_SIG_LEN;
        if (!have_pk) {
            fprintf(stderr,
                "[PoP] peer pubkey not registered — cannot verify holder sig\n");
            return TRELLIS_ERR_VERIFY_FAILED;
        }
        size_t signed_len = POP_CRED_COMMITMENT_LEN + POP_CRED_SIG_LEN;
        if (crypto_sign_ed25519_verify_detached(holder_sig, C,
                                                signed_len, pk_copy) != 0) {
            fprintf(stderr,
                "[PoP] credential holder signature invalid for peer\n");
            return TRELLIS_ERR_VERIFY_FAILED;
        }

        POP_LOCK();
        pop_cache_insert(peer, credential, cred_len);
        POP_UNLOCK();
        return TRELLIS_OK;
    }

    // Unknown credential format: fail-closed to prevent bypass attacks.
    fprintf(stderr, "[PoP] unknown credential format len=%zu, rejecting\n",
            cred_len);
    return TRELLIS_ERR_TRUST;
}

/*
 * trellis_pop_present — generate a fresh anonymous presentation for
 * transmitting to a specific verifier identified by verifier_fp.
 *
 * verifier_fp binds the Schnorr nonce to the intended recipient, preventing
 * cross-verifier replay attacks.  Pass the peer's fingerprint here, NOT the
 * local identity's fingerprint.
 *
 * If the local node has a stored credential, generates a re-randomized
 * proof.  Otherwise returns an empty presentation (degraded mode).
 */
trellis_err_t trellis_pop_present(const trellis_identity_t *id,
                                   const trellis_fingerprint_t *verifier_fp,
                                   uint8_t **credential_out,
                                   size_t *cred_len_out)
{
    if (!id || !verifier_fp || !credential_out || !cred_len_out)
        return TRELLIS_ERR_INVALID_ARG;

    // Copy the credential under the lock; release before doing crypto.
    uint8_t local_cred[POP_CRED_TOTAL_LEN];
    bool found;

    POP_LOCK();
    pop_cred_entry_t *entry = pop_cache_find(&id->fingerprint);
    found = (entry && entry->valid);
    if (found)
        memcpy(local_cred, entry->cred, POP_CRED_TOTAL_LEN);
    POP_UNLOCK();

    if (!found) {
        *credential_out = NULL;
        *cred_len_out   = 0;
        return TRELLIS_OK;
    }

    uint8_t *proof = malloc(POP_PROOF_TOTAL_LEN);
    if (!proof)
        return TRELLIS_ERR_NOMEM;

    uint8_t r_scalar[crypto_core_ristretto255_SCALARBYTES];
    uint8_t sk_scalar[crypto_core_ristretto255_SCALARBYTES];

    uint8_t r_hash64[64] = {0};
    crypto_auth_hmacsha256_state hs;
    crypto_auth_hmacsha256_init(&hs, id->ed25519_sk, TRELLIS_ED25519_SK_LEN);
    static const char r_label[] = "trellis_pop_r_scalar";
    crypto_auth_hmacsha256_update(&hs, (const uint8_t *)r_label, strlen(r_label));
    crypto_auth_hmacsha256_final(&hs, r_hash64);
    crypto_core_ristretto255_scalar_reduce(r_scalar, r_hash64);

    uint8_t sk_hash64[64] = {0};
    memcpy(sk_hash64, id->ed25519_sk, 32);
    crypto_core_ristretto255_scalar_reduce(sk_scalar, sk_hash64);

    trellis_err_t err = trellis_pop_create_presentation(
            local_cred, r_scalar, sk_scalar,
            id, verifier_fp, proof);

    if (err != TRELLIS_OK) {
        free(proof);
        *credential_out = NULL;
        *cred_len_out   = 0;
        return TRELLIS_OK; /* degrade gracefully */
    }

    *credential_out = proof;
    *cred_len_out   = POP_PROOF_TOTAL_LEN;
    return TRELLIS_OK;
}

/*
 * trellis_pop_has_credential — returns true if we have a valid credential
 * for the peer (either cached from a previous verification, or a known
 * established node).
 */
bool trellis_pop_has_credential(const trellis_fingerprint_t *peer)
{
    if (!peer)
        return false;

    POP_LOCK();
    pop_cred_entry_t *entry = pop_cache_find(peer);
    if (entry && entry->valid) {
        POP_UNLOCK();
        return true;
    }

    for (size_t i = 0; i < g_issuer_count; i++) {
        if (trellis_fingerprint_eq(&g_issuers[i].issuer_fp, peer)) {
            POP_UNLOCK();
            return true;
        }
    }

    bool result = false;
    if (g_issuer_count == 0)
        result = pop_in_grace_period();
    POP_UNLOCK();
    return result;
}

/*
 * Store a validated credential for a peer (called after successful
 * trellis_pop_verify, or when receiving a credential via handshake).
 */
void trellis_pop_store_credential(const trellis_fingerprint_t *peer,
                                   const uint8_t *cred, size_t cred_len)
{
    if (!peer || !cred || cred_len == 0)
        return;
    POP_LOCK();
    pop_cache_insert(peer, cred, cred_len);
    POP_UNLOCK();
}
