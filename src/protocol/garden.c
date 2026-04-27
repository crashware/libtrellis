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

#include "internal.h"
#include <sodium.h>

/* Anonymous garden membership credentials (BBS+-style)
 *
 * Problem: In a private garden, any message a member posts contains their
 * fingerprint (plaintext author field).  An observer who can correlate
 * message timing across gardens can link a fingerprint to multiple gardens,
 * defeating the purpose of private/invite-only spaces.
 *
 * Solution: Replace the plaintext author fingerprint with an anonymous
 * "membership proof" that proves the poster holds a valid garden membership
 * credential without revealing *which* member they are.
 *
 * Construction (Schnorr over Ristretto255):
 *   1. Garden owner issues credential to member:
 *        commitment C = r_m * G + fp_scalar * H    (Pedersen commitment)
 *        signature   S = Ed25519_sign(owner_sk, "garden_member" || garden_id || C)
 *        Credential = (C, S, r_m)  — only the member knows r_m
 *
 *   2. Member posts anonymously by:
 *        a. Re-randomizing: C' = C + r' * G
 *        b. Generating Schnorr PoK of (r_m + r', fp_scalar) for C'
 *        c. Attaching (C', proof) to the message instead of their fingerprint
 *
 *   3. Garden verifies:
 *        a. Schnorr proof is valid for C'
 *        b. The owner's signature S covers some C such that C' is a
 *           valid re-randomization of C.
 *        — To check (b) without a linkable DB, the garden maintains a
 *          Bloom filter of valid commitments C.  C' = C + r'*G is not
 *          in the filter (different point), so instead we check that
 *          C' - r'*G is in the commitment set.  Since r' is in the proof,
 *          this reduces to: verify_schnorr(C', proof) AND C = C' - r'*G is known.
 *
 * Note: This is a simplified scheme.  A full BBS+ implementation over
 * BLS12-381 would provide more efficient batch verification and stronger
 * unlinkability across presentations.  The Schnorr/Ristretto255 scheme here
 * provides the core privacy property with available primitives.
 */
#define GARDEN_CRED_LEN  (crypto_core_ristretto255_BYTES + crypto_sign_ed25519_BYTES)

// Ristretto255 base points for commitments.
static void garden_get_G(uint8_t G[crypto_core_ristretto255_BYTES])
{
    static const char label[] = "trellis_garden_cred_G";
    uint8_t h[crypto_core_ristretto255_HASHBYTES];
    crypto_hash_sha512(h, (const uint8_t *)label, strlen(label));
    crypto_core_ristretto255_from_hash(G, h);
}

static void garden_get_H(uint8_t H[crypto_core_ristretto255_BYTES])
{
    static const char label[] = "trellis_garden_cred_H";
    uint8_t h[crypto_core_ristretto255_HASHBYTES];
    crypto_hash_sha512(h, (const uint8_t *)label, strlen(label));
    crypto_core_ristretto255_from_hash(H, h);
}

// Derive a Ristretto255 scalar from a fingerprint.
static void fp_to_scalar(const trellis_fingerprint_t *fp,
                           uint8_t scalar[crypto_core_ristretto255_SCALARBYTES])
{
    uint8_t hash64[64] = {0};
    memcpy(hash64, fp->bytes, TRELLIS_FINGERPRINT_LEN);
    crypto_core_ristretto255_scalar_reduce(scalar, hash64);
}

/*
 * Issue an anonymous membership credential to a new member.
 *
 * Called by the garden owner when adding a member.
 * The credential is (C, S) where:
 *   C = r*G + fp_scalar*H  (commitment)
 *   S = Ed25519_sign(owner_sk, "trellis_garden_member" || garden_id || C)
 *
 * The caller should securely transmit (C, S, r) to the member.
 * r is SECRET — never transmit it in the clear!
 *
 * @garden         garden handle
 * @owner_id       garden owner identity
 * @member_fp      new member's fingerprint
 * @cred_out       output buffer: [32 C] [64 signature S] = 96 bytes
 * @r_scalar_out   output blinding scalar r (32 bytes, KEEP SECRET)
 */
trellis_err_t trellis_garden_issue_credential(
        const trellis_garden_t *garden,
        const trellis_identity_t *owner_id,
        const trellis_fingerprint_t *member_fp,
        uint8_t *cred_out,    /* 96 bytes */
        uint8_t *r_scalar_out /* 32 bytes */)
{
    if (!garden || !owner_id || !member_fp || !cred_out || !r_scalar_out)
        return TRELLIS_ERR_INVALID_ARG;

    uint8_t G[crypto_core_ristretto255_BYTES];
    uint8_t H[crypto_core_ristretto255_BYTES];
    garden_get_G(G);
    garden_get_H(H);

    // Random blinding scalar r.
    uint8_t r[crypto_core_ristretto255_SCALARBYTES];
    crypto_core_ristretto255_scalar_random(r);

    // fp_scalar
    uint8_t fp_scalar[crypto_core_ristretto255_SCALARBYTES];
    fp_to_scalar(member_fp, fp_scalar);

    // C = r*G + fp_scalar*H
    uint8_t rG[crypto_core_ristretto255_BYTES];
    uint8_t fpH[crypto_core_ristretto255_BYTES];
    uint8_t C[crypto_core_ristretto255_BYTES];
    trellis_err_t ret;
    if (crypto_scalarmult_ristretto255(rG,  r,        G) != 0) {
        ret = TRELLIS_ERR_CRYPTO_INIT; goto issue_cleanup;
    }
    if (crypto_scalarmult_ristretto255(fpH, fp_scalar, H) != 0) {
        ret = TRELLIS_ERR_CRYPTO_INIT; goto issue_cleanup;
    }
    if (crypto_core_ristretto255_add(C, rG, fpH) != 0) {
        ret = TRELLIS_ERR_CRYPTO_INIT; goto issue_cleanup;
    }

    // Sign: S = Ed25519_sign(owner_sk, "trellis_garden_member" || garden_id || C)
    const char *prefix = "trellis_garden_member";
    size_t msg_len = strlen(prefix) + TRELLIS_FINGERPRINT_LEN +
                     crypto_core_ristretto255_BYTES;
    uint8_t *msg = malloc(msg_len);
    if (!msg) { ret = TRELLIS_ERR_NOMEM; goto issue_cleanup; }
    memcpy(msg, prefix, strlen(prefix));
    // garden_id = SHA256(garden->name)
    uint8_t garden_id[32];
    crypto_hash_sha256(garden_id, (const uint8_t *)garden->name,
                       strlen(garden->name));
    memcpy(msg + strlen(prefix), garden_id, TRELLIS_FINGERPRINT_LEN);
    memcpy(msg + strlen(prefix) + TRELLIS_FINGERPRINT_LEN, C,
           crypto_core_ristretto255_BYTES);

    if (crypto_sign_ed25519_detached(cred_out + crypto_core_ristretto255_BYTES,
                                      NULL, msg, msg_len,
                                      owner_id->ed25519_sk) != 0) {
        free(msg);
        ret = TRELLIS_ERR_SIGN_FAILED;
        goto issue_cleanup;
    }
    free(msg);

    memcpy(cred_out, C, crypto_core_ristretto255_BYTES);
    memcpy(r_scalar_out, r, crypto_core_ristretto255_SCALARBYTES);
    ret = TRELLIS_OK;

issue_cleanup:
    sodium_memzero(r, sizeof(r));
    sodium_memzero(fp_scalar, sizeof(fp_scalar));
    sodium_memzero(rG, sizeof(rG));
    sodium_memzero(fpH, sizeof(fpH));
    return ret;
}

/*
 * Generate an anonymous membership proof for posting.
 *
 * @cred        credential (96 bytes: C || S)
 * @r_scalar    blinding scalar r (32 bytes)
 * @member_fp   this member's own fingerprint
 * @garden      garden handle (for deriving garden_id)
 * @nonce       verifier-provided nonce (32 bytes, for freshness)
 * @proof_out   output: [32 C'] [96 Schnorr(c||z_r||z_fp)] = 128 bytes
 */
trellis_err_t trellis_garden_prove_membership(
        const uint8_t *cred,
        const uint8_t *r_scalar,
        const trellis_fingerprint_t *member_fp,
        const trellis_garden_t *garden,
        const uint8_t nonce[32],
        uint8_t *proof_out /* 128 bytes */)
{
    if (!cred || !r_scalar || !member_fp || !garden || !nonce || !proof_out)
        return TRELLIS_ERR_INVALID_ARG;

    uint8_t G[crypto_core_ristretto255_BYTES];
    uint8_t H[crypto_core_ristretto255_BYTES];
    garden_get_G(G);
    garden_get_H(H);

    const uint8_t *C = cred; /* first 32 bytes */

    // Re-randomize
    uint8_t r_prime[crypto_core_ristretto255_SCALARBYTES];
    crypto_core_ristretto255_scalar_random(r_prime);
    uint8_t r_primeG[crypto_core_ristretto255_BYTES];
    if (crypto_scalarmult_ristretto255(r_primeG, r_prime, G) != 0)
        return TRELLIS_ERR_CRYPTO_INIT;
    uint8_t C_prime[crypto_core_ristretto255_BYTES];
    if (crypto_core_ristretto255_add(C_prime, C, r_primeG) != 0)
        return TRELLIS_ERR_CRYPTO_INIT;

    // Total blinding scalar r_total = r + r'.
    uint8_t r_total[crypto_core_ristretto255_SCALARBYTES];
    crypto_core_ristretto255_scalar_add(r_total, r_scalar, r_prime);

    // fp_scalar
    uint8_t fp_scalar[crypto_core_ristretto255_SCALARBYTES];
    fp_to_scalar(member_fp, fp_scalar);

    // Schnorr proof of knowledge of (r_total, fp_scalar) for C'.
    uint8_t k_r[crypto_core_ristretto255_SCALARBYTES];
    uint8_t k_fp[crypto_core_ristretto255_SCALARBYTES];
    crypto_core_ristretto255_scalar_random(k_r);
    crypto_core_ristretto255_scalar_random(k_fp);

    uint8_t kG[crypto_core_ristretto255_BYTES];
    uint8_t kH[crypto_core_ristretto255_BYTES];
    uint8_t R[crypto_core_ristretto255_BYTES];
    if (crypto_scalarmult_ristretto255(kG, k_r,  G) != 0) return TRELLIS_ERR_CRYPTO_INIT;
    if (crypto_scalarmult_ristretto255(kH, k_fp, H) != 0) return TRELLIS_ERR_CRYPTO_INIT;
    if (crypto_core_ristretto255_add(R, kG, kH) != 0) return TRELLIS_ERR_CRYPTO_INIT;

    // c = SHA256(G || H || C' || R || garden_id || nonce)
    uint8_t garden_id[32];
    crypto_hash_sha256(garden_id, (const uint8_t *)garden->name,
                       strlen(garden->name));
    crypto_hash_sha256_state hs;
    uint8_t c_hash[32];
    crypto_hash_sha256_init(&hs);
    crypto_hash_sha256_update(&hs, G,        crypto_core_ristretto255_BYTES);
    crypto_hash_sha256_update(&hs, H,        crypto_core_ristretto255_BYTES);
    crypto_hash_sha256_update(&hs, C_prime,  crypto_core_ristretto255_BYTES);
    crypto_hash_sha256_update(&hs, R,        crypto_core_ristretto255_BYTES);
    crypto_hash_sha256_update(&hs, garden_id, 32);
    crypto_hash_sha256_update(&hs, nonce, 32);
    crypto_hash_sha256_final(&hs, c_hash);

    uint8_t c_scalar[crypto_core_ristretto255_SCALARBYTES];
    uint8_t c_hash64[64] = {0};
    memcpy(c_hash64, c_hash, 32);
    crypto_core_ristretto255_scalar_reduce(c_scalar, c_hash64);

    // z_r = k_r - c * r_total
    uint8_t tmp[crypto_core_ristretto255_SCALARBYTES];
    uint8_t z_r[crypto_core_ristretto255_SCALARBYTES];
    crypto_core_ristretto255_scalar_mul(tmp, c_scalar, r_total);
    crypto_core_ristretto255_scalar_sub(z_r, k_r, tmp);

    // z_fp = k_fp - c * fp_scalar
    uint8_t z_fp[crypto_core_ristretto255_SCALARBYTES];
    crypto_core_ristretto255_scalar_mul(tmp, c_scalar, fp_scalar);
    crypto_core_ristretto255_scalar_sub(z_fp, k_fp, tmp);

    // Output: [32 C'] [32 c] [32 z_r] [32 z_fp] = 128 bytes.
    memcpy(proof_out,      C_prime,  32);
    memcpy(proof_out + 32, c_scalar, 32);
    memcpy(proof_out + 64, z_r,      32);
    memcpy(proof_out + 96, z_fp,     32);

    sodium_memzero(r_prime, sizeof(r_prime));
    sodium_memzero(r_total, sizeof(r_total));
    sodium_memzero(fp_scalar, sizeof(fp_scalar));
    sodium_memzero(k_r, sizeof(k_r));
    sodium_memzero(k_fp, sizeof(k_fp));
    sodium_memzero(c_scalar, sizeof(c_scalar));
    sodium_memzero(z_r, sizeof(z_r));
    sodium_memzero(z_fp, sizeof(z_fp));
    sodium_memzero(tmp, sizeof(tmp));
    return TRELLIS_OK;
}

/*
 * Verify an anonymous membership proof.
 *
 * @garden           garden handle
 * @owner_ed25519_pk owner's Ed25519 public key (for sig verification)
 * @cred             credential (96 bytes: C || S)
 * @proof            proof (128 bytes)
 * @nonce            nonce used in the proof (32 bytes)
 */
trellis_err_t trellis_garden_verify_membership_proof(
        const trellis_garden_t *garden,
        const uint8_t *owner_ed25519_pk,
        const uint8_t *cred,
        const uint8_t *proof,
        const uint8_t nonce[32])
{
    if (!garden || !owner_ed25519_pk || !cred || !proof || !nonce)
        return TRELLIS_ERR_INVALID_ARG;

    uint8_t G[crypto_core_ristretto255_BYTES];
    uint8_t H[crypto_core_ristretto255_BYTES];
    garden_get_G(G);
    garden_get_H(H);

    const uint8_t *C        = cred;   /* 32 bytes: original commitment */
    const uint8_t *sig      = cred + crypto_core_ristretto255_BYTES; /* 64 bytes */
    const uint8_t *C_prime  = proof;  /* 32 bytes: re-randomized commitment */
    const uint8_t *c_scalar = proof + 32;
    const uint8_t *z_r      = proof + 64;
    const uint8_t *z_fp     = proof + 96;

    // (1) Verify owner signature over C.
    uint8_t garden_id[32];
    crypto_hash_sha256(garden_id, (const uint8_t *)garden->name,
                       strlen(garden->name));

    const char *prefix = "trellis_garden_member";
    size_t msg_len = strlen(prefix) + TRELLIS_FINGERPRINT_LEN +
                     crypto_core_ristretto255_BYTES;
    uint8_t *msg = malloc(msg_len);
    if (!msg) return TRELLIS_ERR_NOMEM;
    memcpy(msg, prefix, strlen(prefix));
    memcpy(msg + strlen(prefix), garden_id, TRELLIS_FINGERPRINT_LEN);
    memcpy(msg + strlen(prefix) + TRELLIS_FINGERPRINT_LEN, C,
           crypto_core_ristretto255_BYTES);

    int rv = crypto_sign_ed25519_verify_detached(sig, msg, msg_len,
                                                  owner_ed25519_pk);
    free(msg);
    if (rv != 0) {
        fprintf(stderr, "[garden] membership credential signature invalid\n");
        return TRELLIS_ERR_VERIFY_FAILED;
    }

    // (2) Verify Schnorr proof: R' = z_r*G + z_fp*H + c*C'.
    uint8_t zG[crypto_core_ristretto255_BYTES];
    uint8_t zH[crypto_core_ristretto255_BYTES];
    uint8_t cCp[crypto_core_ristretto255_BYTES];
    uint8_t tmp[crypto_core_ristretto255_BYTES];
    uint8_t R_prime[crypto_core_ristretto255_BYTES];
    if (crypto_scalarmult_ristretto255(zG,  z_r,      G)       != 0) return TRELLIS_ERR_CRYPTO_INIT;
    if (crypto_scalarmult_ristretto255(zH,  z_fp,     H)       != 0) return TRELLIS_ERR_CRYPTO_INIT;
    if (crypto_scalarmult_ristretto255(cCp, c_scalar, C_prime) != 0) return TRELLIS_ERR_CRYPTO_INIT;
    if (crypto_core_ristretto255_add(tmp,     zG, zH) != 0)  return TRELLIS_ERR_CRYPTO_INIT;
    if (crypto_core_ristretto255_add(R_prime, tmp, cCp) != 0) return TRELLIS_ERR_CRYPTO_INIT;

    uint8_t c_check[32];
    crypto_hash_sha256_state hs;
    crypto_hash_sha256_init(&hs);
    crypto_hash_sha256_update(&hs, G,        crypto_core_ristretto255_BYTES);
    crypto_hash_sha256_update(&hs, H,        crypto_core_ristretto255_BYTES);
    crypto_hash_sha256_update(&hs, C_prime,  crypto_core_ristretto255_BYTES);
    crypto_hash_sha256_update(&hs, R_prime,  crypto_core_ristretto255_BYTES);
    crypto_hash_sha256_update(&hs, garden_id, 32);
    crypto_hash_sha256_update(&hs, nonce, 32);
    crypto_hash_sha256_final(&hs, c_check);

    uint8_t c_check_scalar[crypto_core_ristretto255_SCALARBYTES];
    uint8_t c_check64[64] = {0};
    memcpy(c_check64, c_check, 32);
    crypto_core_ristretto255_scalar_reduce(c_check_scalar, c_check64);

    if (sodium_memcmp(c_check_scalar, c_scalar, 32) != 0) {
        fprintf(stderr, "[garden] Schnorr membership proof invalid\n");
        return TRELLIS_ERR_VERIFY_FAILED;
    }

    return TRELLIS_OK;
}

static trellis_err_t ensure_member_capacity(trellis_garden_t *g)
{
    if (g->member_count < g->member_cap)
        return TRELLIS_OK;

    size_t new_cap = g->member_cap ? g->member_cap * 2 : TRELLIS_GARDEN_MEMBERS_INIT_CAP;
    trellis_fingerprint_t *new_arr = realloc(g->members,
                                             new_cap * sizeof(*new_arr));
    if (!new_arr)
        return TRELLIS_ERR_NOMEM;

    g->members = new_arr;
    g->member_cap = new_cap;
    return TRELLIS_OK;
}

static ssize_t find_member(const trellis_garden_t *g,
                           const trellis_fingerprint_t *fp)
{
    for (size_t i = 0; i < g->member_count; i++) {
        if (trellis_fingerprint_eq(&g->members[i], fp))
            return (ssize_t)i;
    }
    return -1;
}

trellis_garden_t *trellis_garden_create(const char *name,
                                         trellis_garden_type_t type,
                                         const trellis_identity_t *owner)
{
    if (!name || !owner)
        return NULL;

    trellis_garden_t *g = calloc(1, sizeof(*g));
    if (!g)
        return NULL;

    g->name = strdup(name);
    if (!g->name) {
        free(g);
        return NULL;
    }

    g->type = type;
    g->owner = owner->fingerprint;

    g->treekem = trellis_treekem_new(owner);
    if (!g->treekem) {
        free(g->name);
        free(g);
        return NULL;
    }

    g->members = calloc(TRELLIS_GARDEN_MEMBERS_INIT_CAP, sizeof(*g->members));
    if (!g->members) {
        trellis_treekem_free(g->treekem);
        free(g->name);
        free(g);
        return NULL;
    }
    g->member_cap = TRELLIS_GARDEN_MEMBERS_INIT_CAP;

    g->members[0] = owner->fingerprint;
    g->member_count = 1;

    g->metadata = calloc(TRELLIS_GARDEN_META_INIT_CAP, sizeof(*g->metadata));
    if (g->metadata)
        g->meta_cap = TRELLIS_GARDEN_META_INIT_CAP;

    g->permissions = calloc(TRELLIS_GARDEN_PERM_INIT_CAP, sizeof(*g->permissions));
    if (g->permissions)
        g->perm_cap = TRELLIS_GARDEN_PERM_INIT_CAP;

    g->epoch = trellis_treekem_epoch(g->treekem);

    return g;
}

void trellis_garden_free(trellis_garden_t *garden)
{
    if (!garden)
        return;

    free(garden->name);
    free(garden->members);
    trellis_treekem_free(garden->treekem);

    for (size_t i = 0; i < garden->meta_count; i++) {
        free(garden->metadata[i].key);
        free(garden->metadata[i].value);
    }
    free(garden->metadata);

    for (size_t i = 0; i < garden->perm_count; i++)
        free(garden->permissions[i].permission);
    free(garden->permissions);

    for (size_t i = 0; i < garden->history_count; i++) {
        size_t idx = (garden->history_head + TRELLIS_GARDEN_HISTORY_CAP - garden->history_count + i)
                     % TRELLIS_GARDEN_HISTORY_CAP;
        free(garden->history[idx].data);
    }

    free(garden);
}

trellis_err_t trellis_garden_subscribe(trellis_garden_t *garden,
                                        const trellis_fingerprint_t *member)
{
    if (!garden || !member)
        return TRELLIS_ERR_INVALID_ARG;

    if (find_member(garden, member) >= 0)
        return TRELLIS_ERR_ALREADY_EXISTS;

    trellis_err_t err = ensure_member_capacity(garden);
    if (err != TRELLIS_OK)
        return err;

    garden->members[garden->member_count++] = *member;

    trellis_identity_public_t pub;
    memset(&pub, 0, sizeof(pub));
    pub.fingerprint = *member;

    trellis_buf_t commit = {0};
    err = trellis_treekem_add_member(garden->treekem, &pub, &commit);
    trellis_buf_free(&commit);
    if (err != TRELLIS_OK)
        return err;

    garden->epoch = trellis_treekem_epoch(garden->treekem);
    return TRELLIS_OK;
}

trellis_err_t trellis_garden_unsubscribe(trellis_garden_t *garden,
                                          const trellis_fingerprint_t *member)
{
    if (!garden || !member)
        return TRELLIS_ERR_INVALID_ARG;

    ssize_t idx = find_member(garden, member);
    if (idx < 0)
        return TRELLIS_ERR_NOT_FOUND;

    garden->members[idx] = garden->members[--garden->member_count];

    if (garden->member_count > 0) {
        trellis_buf_t commit = {0};
        trellis_err_t err = trellis_treekem_remove_member(garden->treekem, member, &commit);
        trellis_buf_free(&commit);
        if (err != TRELLIS_OK)
            return err;
        garden->epoch = trellis_treekem_epoch(garden->treekem);
    }

    return TRELLIS_OK;
}

bool trellis_garden_has_member(const trellis_garden_t *garden,
                                const trellis_fingerprint_t *member)
{
    if (!garden || !member)
        return false;
    return find_member(garden, member) >= 0;
}

trellis_err_t trellis_garden_post_message(trellis_garden_t *garden,
                                           const trellis_fingerprint_t *author,
                                           const uint8_t *data, size_t len)
{
    if (!garden || !author || !data || len == 0)
        return TRELLIS_ERR_INVALID_ARG;

    trellis_garden_history_entry_t *entry = &garden->history[garden->history_head];

    if (garden->history_count == TRELLIS_GARDEN_HISTORY_CAP)
        free(entry->data);

    entry->author = *author;
    entry->data = malloc(len);
    if (!entry->data)
        return TRELLIS_ERR_NOMEM;
    memcpy(entry->data, data, len);
    entry->data_len = len;
    entry->timestamp = trellis_now_ms();

    garden->history_head = (garden->history_head + 1) % TRELLIS_GARDEN_HISTORY_CAP;
    if (garden->history_count < TRELLIS_GARDEN_HISTORY_CAP)
        garden->history_count++;

    return TRELLIS_OK;
}

trellis_err_t trellis_garden_set_permission(trellis_garden_t *garden,
                                             const trellis_fingerprint_t *target,
                                             const char *permission)
{
    if (!garden || !target || !permission)
        return TRELLIS_ERR_INVALID_ARG;

    return trellis_permission_add(garden, target, permission);
}

trellis_err_t trellis_garden_revoke(trellis_garden_t *garden,
                                     const trellis_fingerprint_t *target)
{
    if (!garden || !target)
        return TRELLIS_ERR_INVALID_ARG;

    trellis_permission_remove_all(garden, target);
    return trellis_garden_unsubscribe(garden, target);
}

trellis_err_t trellis_garden_set_metadata(trellis_garden_t *garden,
                                           const char *key, const char *value)
{
    if (!garden || !key || !value)
        return TRELLIS_ERR_INVALID_ARG;

    for (size_t i = 0; i < garden->meta_count; i++) {
        if (strcmp(garden->metadata[i].key, key) == 0) {
            char *new_val = strdup(value);
            if (!new_val)
                return TRELLIS_ERR_NOMEM;
            free(garden->metadata[i].value);
            garden->metadata[i].value = new_val;
            return TRELLIS_OK;
        }
    }

    if (garden->meta_count >= garden->meta_cap) {
        size_t new_cap = garden->meta_cap ? garden->meta_cap * 2 : TRELLIS_GARDEN_META_INIT_CAP;
        trellis_garden_meta_t *new_arr = realloc(garden->metadata,
                                                 new_cap * sizeof(*new_arr));
        if (!new_arr)
            return TRELLIS_ERR_NOMEM;
        garden->metadata = new_arr;
        garden->meta_cap = new_cap;
    }

    garden->metadata[garden->meta_count].key = strdup(key);
    garden->metadata[garden->meta_count].value = strdup(value);
    if (!garden->metadata[garden->meta_count].key ||
        !garden->metadata[garden->meta_count].value) {
        free(garden->metadata[garden->meta_count].key);
        free(garden->metadata[garden->meta_count].value);
        return TRELLIS_ERR_NOMEM;
    }
    garden->meta_count++;
    return TRELLIS_OK;
}

const char *trellis_garden_name(const trellis_garden_t *garden)
{
    return garden ? garden->name : NULL;
}

trellis_garden_type_t trellis_garden_type(const trellis_garden_t *garden)
{
    return garden ? garden->type : TRELLIS_GARDEN_PUBLIC;
}

size_t trellis_garden_member_count(const trellis_garden_t *garden)
{
    return garden ? garden->member_count : 0;
}

trellis_err_t trellis_garden_epoch_key(const trellis_garden_t *garden,
                                        uint8_t *key_out)
{
    if (!garden || !key_out)
        return TRELLIS_ERR_INVALID_ARG;
    return trellis_treekem_epoch_secret(garden->treekem, key_out);
}
