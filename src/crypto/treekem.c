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
 * treekem.c — TreeKEM group key agreement (Section 8.4, RFC 9420 style)
 *
 * Binary left-balanced tree where leaves are group members and internal
 * nodes hold path secrets.  Adding/removing a member requires O(log n)
 * KEM operations and HKDF derivations.
 */

#include "internal.h"

/*
 * Left-balanced binary tree stored in array form.
 * For n leaves, we need 2*n - 1 total nodes.
 * Leaves are at even indices: 0, 2, 4, ...
 * Internal nodes at odd indices: 1, 3, 5, ...
 *
 * parent(x) depends on level; we use the standard MLS tree math.
 */

// RFC 9420 §7.1: level of a node — 0 for leaves, trailing-ones count for internals.
static inline size_t node_level(size_t x)
{
    if ((x & 1) == 0) return 0;
    return (size_t)__builtin_ctzll(~x);
}

static inline size_t tree_left(size_t x)
{
    size_t k = node_level(x);
    if (k == 0) return x;
    return x ^ ((size_t)1 << (k - 1));
}

static inline size_t tree_right(size_t x)
{
    size_t k = node_level(x);
    if (k == 0) return x;
    return x ^ ((size_t)3 << (k - 1));
}

static inline size_t tree_parent(size_t x)
{
    size_t k = node_level(x);
    size_t b = (x >> (k + 1)) & 0x01;
    return (x | ((size_t)1 << k)) ^ (b << (k + 1));
}

static inline size_t tree_sibling(size_t x)
{
    size_t p = tree_parent(x);
    size_t l = tree_left(p);
    return (l == x) ? tree_right(p) : l;
}

static inline bool is_leaf(size_t idx) { return (idx & 1) == 0; }
static inline size_t leaf_to_node(size_t leaf_idx) { return leaf_idx * 2; }

// 1024 is arbitrary -- should be configurable per-garden.
#define MAX_MEMBERS 1024
#define MAX_NODES   (2 * MAX_MEMBERS - 1)

// Root node for a tree with n leaves.
static size_t tree_root(size_t n_leaves)
{
    if (n_leaves <= 1) return 0;
    size_t w = 2 * n_leaves - 1;
    size_t lvl = 0;
    while ((1ULL << lvl) < w) lvl++;
    return (1ULL << (lvl - 1)) - 1;
}

// -----------
// Tree node.
typedef struct treekem_node {
    bool occupied;
    bool blanked;

    // Public identity (for leaves)
    trellis_identity_public_t pub;
    trellis_fingerprint_t     fingerprint;

    // Hybrid KEM public keys for this node.
    uint8_t x25519_pk[TRELLIS_X25519_PK_LEN];
    uint8_t ml_kem_pk[TRELLIS_ML_KEM_1024_PK_LEN];

    // Path secret at this node (only known if we're on the direct path)
    uint8_t path_secret[32];
    bool    has_secret;
} treekem_node_t;

// -----------
// TreeKEM struct (opaque)
struct trellis_treekem {
    treekem_node_t nodes[MAX_NODES];
    size_t         member_count;
    uint64_t       epoch;
    uint8_t        epoch_secret[32];

    const trellis_identity_t *local_id;
    size_t                    local_leaf;

    // Local KEM keypair for decrypting path secrets directed at us.
    trellis_kem_keypair_t local_kem;
};

// -----------
// Path secret derivation.

/*
 * Seeded PRNG state for deterministic ML-KEM keygen.
 *
 * OQS_KEM_keypair uses OQS_randombytes internally.  We temporarily redirect
 * that to a deterministic stream derived from the node's path secret so that
 * any party who knows the path secret can independently reproduce the same
 * ML-KEM public key.
 *
 * The redirect is process-global, so callers on the same thread must not be
 * interrupted.  libtrellis runs its event loop on a single dedicated thread,
 * so this is safe.
 */

/* Maximum bytes OQS ML-KEM-1024 keypair draws from the PRNG.
 * ML-KEM-1024 needs 64 bytes of randomness for key generation. */
#define TREEKEM_RAND_BUF_LEN 256

typedef struct {
    uint8_t  buf[TREEKEM_RAND_BUF_LEN];
    size_t   used;
    size_t   capacity;
} seeded_rand_state_t;

static seeded_rand_state_t g_seeded_rand;

static void seeded_rand_callback(uint8_t *out, size_t n)
{
    // Fill from pre-derived deterministic buffer; wrap with SHAKE-256 if exhausted.
    size_t remaining = g_seeded_rand.capacity - g_seeded_rand.used;
    if (n <= remaining) {
        memcpy(out, g_seeded_rand.buf + g_seeded_rand.used, n);
        g_seeded_rand.used += n;
    } else {
        // Fallback: copy what we have, then extend via SHAKE-256 of the tail.
        if (remaining > 0) {
            memcpy(out, g_seeded_rand.buf + g_seeded_rand.used, remaining);
            out += remaining;
            n -= remaining;
        }
        trellis_shake256(g_seeded_rand.buf, g_seeded_rand.capacity,
                         out, n);
        g_seeded_rand.used = g_seeded_rand.capacity; /* mark exhausted */
    }
}

static trellis_err_t derive_node_keys(const uint8_t *path_secret,
                                      uint8_t *x25519_pk, uint8_t *x25519_sk,
                                      uint8_t *ml_kem_pk, uint8_t *ml_kem_sk)
{
    // Derive a deterministic seed from path secret for the node's KEM keypair.
    uint8_t seed[64];
    static const uint8_t info[] = "Bloom-TreeKEM-NodeKey";
    trellis_err_t err = trellis_hkdf_shake256(path_secret, 32,
                                              NULL, 0,
                                              info, sizeof(info) - 1,
                                              seed, sizeof(seed));
    if (err != TRELLIS_OK)
        return err;

    // X25519 keypair from first 32 bytes of seed.
    memcpy(x25519_sk, seed, TRELLIS_X25519_SK_LEN);
    if (crypto_scalarmult_curve25519_base(x25519_pk, x25519_sk) != 0) {
        sodium_memzero(seed, sizeof(seed));
        return TRELLIS_ERR_KEM_KEYGEN;
    }

    /* ML-KEM keygen using a deterministic PRNG seeded by the second 32 bytes.
     * Pre-fill the seeded buffer with SHAKE-256(seed[32..63]) so OQS draws
     * the same bytes for any given path secret. */
    trellis_shake256(seed + 32, 32,
                     g_seeded_rand.buf, TREEKEM_RAND_BUF_LEN);
    g_seeded_rand.used     = 0;
    g_seeded_rand.capacity = TREEKEM_RAND_BUF_LEN;
    sodium_memzero(seed, sizeof(seed));

    OQS_randombytes_custom_algorithm(seeded_rand_callback);

    OQS_KEM *kem = OQS_KEM_new(OQS_KEM_alg_ml_kem_1024);
    OQS_STATUS rc_kem = OQS_SUCCESS;
    if (kem) {
        rc_kem = OQS_KEM_keypair(kem, ml_kem_pk, ml_kem_sk);
        OQS_KEM_free(kem);
    } else {
        rc_kem = OQS_ERROR;
    }

    // Restore OQS to system randomness.
    OQS_randombytes_switch_algorithm("system");
    sodium_memzero(g_seeded_rand.buf, TREEKEM_RAND_BUF_LEN);

    return (rc_kem == OQS_SUCCESS) ? TRELLIS_OK : TRELLIS_ERR_KEM_KEYGEN;
}

static trellis_err_t derive_path_secret(const uint8_t *child_secret,
                                        uint8_t *parent_secret)
{
    static const uint8_t info[] = "Bloom-TreeKEM-PathSecret";
    return trellis_hkdf_shake256(child_secret, 32,
                                 NULL, 0,
                                 info, sizeof(info) - 1,
                                 parent_secret, 32);
}

// Recompute path from a leaf up to the root, setting path secrets and node keys.
static trellis_err_t recompute_path(trellis_treekem_t *tk, size_t start_node,
                                    const uint8_t *leaf_secret)
{
    size_t root = tree_root(tk->member_count);
    size_t cur = start_node;
    uint8_t secret[32];
    memcpy(secret, leaf_secret, 32);

    // Temporary storage for private keys (only for local path)
    uint8_t x25519_sk[TRELLIS_X25519_SK_LEN];
    uint8_t ml_kem_sk[TRELLIS_ML_KEM_1024_SK_LEN];

    while (1) {
        treekem_node_t *node = &tk->nodes[cur];
        memcpy(node->path_secret, secret, 32);
        node->has_secret = true;
        node->blanked = false;
        node->occupied = true;

        trellis_err_t err = derive_node_keys(secret,
                                             node->x25519_pk, x25519_sk,
                                             node->ml_kem_pk, ml_kem_sk);
        if (err != TRELLIS_OK) {
            sodium_memzero(secret, sizeof(secret));
            sodium_memzero(x25519_sk, sizeof(x25519_sk));
            sodium_memzero(ml_kem_sk, sizeof(ml_kem_sk));
            return err;
        }

        if (cur == root)
            break;

        uint8_t parent_secret[32];
        err = derive_path_secret(secret, parent_secret);
        if (err != TRELLIS_OK) {
            sodium_memzero(secret, sizeof(secret));
            sodium_memzero(x25519_sk, sizeof(x25519_sk));
            sodium_memzero(ml_kem_sk, sizeof(ml_kem_sk));
            return err;
        }
        memcpy(secret, parent_secret, 32);
        sodium_memzero(parent_secret, sizeof(parent_secret));

        cur = tree_parent(cur);
    }

    // Epoch secret = root path secret.
    memcpy(tk->epoch_secret, secret, 32);

    sodium_memzero(secret, sizeof(secret));
    sodium_memzero(x25519_sk, sizeof(x25519_sk));
    sodium_memzero(ml_kem_sk, sizeof(ml_kem_sk));

    return TRELLIS_OK;
}

// -----------
// Public API.
trellis_treekem_t *trellis_treekem_new(const trellis_identity_t *creator)
{
    if (!creator)
        return NULL;

    trellis_treekem_t *tk = calloc(1, sizeof(*tk));
    if (!tk)
        return NULL;

    tk->local_id    = creator;
    tk->local_leaf  = 0;
    tk->member_count = 1;
    tk->epoch       = 0;

    // Generate local KEM keypair.
    if (trellis_kem_keygen(&tk->local_kem) != TRELLIS_OK) {
        free(tk);
        return NULL;
    }

    // Set leaf 0.
    treekem_node_t *leaf = &tk->nodes[0];
    leaf->occupied = true;
    trellis_identity_public_from(creator, &leaf->pub);
    memcpy(&leaf->fingerprint, &creator->fingerprint, sizeof(trellis_fingerprint_t));
    memcpy(leaf->x25519_pk, tk->local_kem.x25519_pk, TRELLIS_X25519_PK_LEN);
    memcpy(leaf->ml_kem_pk, tk->local_kem.ml_kem_pk, TRELLIS_ML_KEM_1024_PK_LEN);

    // Initial epoch secret from random leaf secret.
    uint8_t leaf_secret[32];
    randombytes_buf(leaf_secret, sizeof(leaf_secret));
    if (recompute_path(tk, 0, leaf_secret) != TRELLIS_OK) {
        sodium_memzero(leaf_secret, sizeof(leaf_secret));
        free(tk);
        return NULL;
    }
    sodium_memzero(leaf_secret, sizeof(leaf_secret));

    return tk;
}

void trellis_treekem_free(trellis_treekem_t *tk)
{
    if (!tk)
        return;
    sodium_memzero(tk, sizeof(*tk));
    free(tk);
}

trellis_err_t trellis_treekem_add_member(trellis_treekem_t *tk,
                                         const trellis_identity_public_t *member,
                                         trellis_buf_t *commit_out)
{
    if (!tk || !member || !commit_out)
        return TRELLIS_ERR_INVALID_ARG;
    if (tk->member_count >= MAX_MEMBERS)
        return TRELLIS_ERR_ALREADY_EXISTS;

    size_t new_leaf_idx = tk->member_count;
    size_t new_node = leaf_to_node(new_leaf_idx);
    tk->member_count++;
    tk->epoch++;

    // Place the new member at the leaf.
    treekem_node_t *leaf = &tk->nodes[new_node];
    leaf->occupied = true;
    leaf->blanked  = false;
    memcpy(&leaf->pub, member, sizeof(*member));
    memcpy(&leaf->fingerprint, &member->fingerprint, sizeof(trellis_fingerprint_t));
    memcpy(leaf->x25519_pk, member->x25519_pk, TRELLIS_X25519_PK_LEN);
    memcpy(leaf->ml_kem_pk, member->ml_kem_pk, TRELLIS_ML_KEM_1024_PK_LEN);

    // Generate fresh leaf secret and recompute our path.
    uint8_t leaf_secret[32];
    randombytes_buf(leaf_secret, sizeof(leaf_secret));

    size_t my_node = leaf_to_node(tk->local_leaf);
    trellis_err_t err = recompute_path(tk, my_node, leaf_secret);
    if (err != TRELLIS_OK) {
        sodium_memzero(leaf_secret, sizeof(leaf_secret));
        return err;
    }

    /*
     * Build commit message: encrypt path secrets for each co-path node.
     * Format: [4: epoch][4: n_entries]
     *         for each entry:
     *           [4: node_idx][ML_KEM_CT_LEN: ciphertext][32: encrypted_secret]
     *
     * We walk from our leaf to the root. At each step, the sibling needs the
     * parent's path secret, encrypted to the sibling's KEM public key.
     */
    size_t root = tree_root(tk->member_count);
    size_t path_len = 0;
    size_t cur = my_node;
    while (cur != root) {
        cur = tree_parent(cur);
        path_len++;
    }

    // Each entry: 4 (node_idx) + X25519_PK + ML_KEM_CT + NONCE + TAG + 32 (encrypted secret)
    size_t entry_size = 4 + TRELLIS_X25519_PK_LEN + TRELLIS_ML_KEM_1024_CT_LEN
                      + TRELLIS_AES_NONCE_LEN + TRELLIS_AES_TAG_LEN + 32;
    size_t commit_len = 4 + 4 + path_len * entry_size;

    *commit_out = trellis_buf_alloc(commit_len);
    if (!commit_out->data) {
        sodium_memzero(leaf_secret, sizeof(leaf_secret));
        return TRELLIS_ERR_NOMEM;
    }

    uint8_t *p = commit_out->data;
    size_t off = 0;

    trellis_write_u32_be(p + off, (uint32_t)tk->epoch);
    off += 4;
    trellis_write_u32_be(p + off, (uint32_t)path_len);
    off += 4;

    cur = my_node;
    while (cur != root) {
        size_t par = tree_parent(cur);
        size_t sib = tree_sibling(cur);

        treekem_node_t *sib_node = &tk->nodes[sib < MAX_NODES ? sib : 0];
        treekem_node_t *par_node = &tk->nodes[par < MAX_NODES ? par : 0];

        // Node index.
        trellis_write_u32_be(p + off, (uint32_t)sib);
        off += 4;

        // Encapsulate to sibling's KEM key to get a wrapping key.
        uint8_t wrap_key[TRELLIS_ML_KEM_1024_SS_LEN];
        uint8_t eph_x25519_pk[TRELLIS_X25519_PK_LEN];
        uint8_t eph_x25519_sk[TRELLIS_X25519_SK_LEN];
        uint8_t dh_ss[TRELLIS_X25519_PK_LEN];
        uint8_t kem_ct[TRELLIS_ML_KEM_1024_CT_LEN];
        uint8_t kem_ss[TRELLIS_ML_KEM_1024_SS_LEN];

        /* Ephemeral X25519 -- DH failure is non-fatal (sibling may lack a key);
           security still rests on the ML-KEM encapsulation below. */
        randombytes_buf(eph_x25519_sk, sizeof(eph_x25519_sk));
        if (crypto_scalarmult_curve25519_base(eph_x25519_pk, eph_x25519_sk) != 0) {
            sodium_memzero(eph_x25519_sk, sizeof(eph_x25519_sk));
            sodium_memzero(leaf_secret, sizeof(leaf_secret));
            trellis_buf_free(commit_out);
            return TRELLIS_ERR_CRYPTO;
        }
        if (crypto_scalarmult_curve25519(dh_ss, eph_x25519_sk, sib_node->x25519_pk) != 0)
            sodium_memzero(dh_ss, sizeof(dh_ss));
        sodium_memzero(eph_x25519_sk, sizeof(eph_x25519_sk));

        memcpy(p + off, eph_x25519_pk, TRELLIS_X25519_PK_LEN);
        off += TRELLIS_X25519_PK_LEN;

        // ML-KEM encaps.
        OQS_KEM *kem = OQS_KEM_new(OQS_KEM_alg_ml_kem_1024);
        if (!kem) {
            sodium_memzero(dh_ss, sizeof(dh_ss));
            sodium_memzero(leaf_secret, sizeof(leaf_secret));
            trellis_buf_free(commit_out);
            return TRELLIS_ERR_NOMEM;
        }
        if (OQS_KEM_encaps(kem, kem_ct, kem_ss, sib_node->ml_kem_pk) != OQS_SUCCESS) {
            OQS_KEM_free(kem);
            sodium_memzero(dh_ss, sizeof(dh_ss));
            sodium_memzero(leaf_secret, sizeof(leaf_secret));
            trellis_buf_free(commit_out);
            return TRELLIS_ERR_KEM_ENCAPS;
        }
        OQS_KEM_free(kem);

        memcpy(p + off, kem_ct, TRELLIS_ML_KEM_1024_CT_LEN);
        off += TRELLIS_ML_KEM_1024_CT_LEN;

        // Combine DH + KEM shared secrets for wrap key.
        uint8_t combined_ss[TRELLIS_X25519_PK_LEN + TRELLIS_ML_KEM_1024_SS_LEN];
        memcpy(combined_ss, dh_ss, TRELLIS_X25519_PK_LEN);
        memcpy(combined_ss + TRELLIS_X25519_PK_LEN, kem_ss, TRELLIS_ML_KEM_1024_SS_LEN);
        sodium_memzero(dh_ss, sizeof(dh_ss));
        sodium_memzero(kem_ss, sizeof(kem_ss));

        static const uint8_t wrap_info[] = "Bloom-TreeKEM-Wrap";
        trellis_err_t hkdf_err = trellis_hkdf_shake256(
                              combined_ss, sizeof(combined_ss),
                              NULL, 0,
                              wrap_info, sizeof(wrap_info) - 1,
                              wrap_key, 32);
        sodium_memzero(combined_ss, sizeof(combined_ss));
        if (hkdf_err != TRELLIS_OK) {
            sodium_memzero(leaf_secret, sizeof(leaf_secret));
            trellis_buf_free(commit_out);
            return hkdf_err;
        }

        // Encrypt parent path secret with wrap key using AES-256-GCM.
        uint8_t enc_nonce[TRELLIS_AES_NONCE_LEN];
        randombytes_buf(enc_nonce, sizeof(enc_nonce));
        memcpy(p + off, enc_nonce, TRELLIS_AES_NONCE_LEN);
        off += TRELLIS_AES_NONCE_LEN;

        uint8_t enc_tag[TRELLIS_AES_TAG_LEN];
        uint8_t enc_secret[32];
        trellis_err_t enc_err = trellis_aes256gcm_encrypt(
            wrap_key, enc_nonce, par_node->path_secret, 32,
            NULL, 0, enc_secret, enc_tag);
        sodium_memzero(wrap_key, sizeof(wrap_key));
        if (enc_err != TRELLIS_OK) {
            sodium_memzero(enc_secret, sizeof(enc_secret));
            sodium_memzero(leaf_secret, sizeof(leaf_secret));
            trellis_buf_free(commit_out);
            return enc_err;
        }

        memcpy(p + off, enc_tag, TRELLIS_AES_TAG_LEN);
        off += TRELLIS_AES_TAG_LEN;

        memcpy(p + off, enc_secret, 32);
        off += 32;

        sodium_memzero(enc_secret, sizeof(enc_secret));

        cur = par;
    }

    commit_out->len = off;
    sodium_memzero(leaf_secret, sizeof(leaf_secret));

    return TRELLIS_OK;
}

trellis_err_t trellis_treekem_remove_member(trellis_treekem_t *tk,
                                            const trellis_fingerprint_t *member,
                                            trellis_buf_t *commit_out)
{
    if (!tk || !member || !commit_out)
        return TRELLIS_ERR_INVALID_ARG;
    if (tk->member_count <= 1)
        return TRELLIS_ERR_INVALID_ARG;

    // Find the member's leaf.
    size_t target_leaf = SIZE_MAX;
    for (size_t i = 0; i < tk->member_count; i++) {
        size_t node = leaf_to_node(i);
        if (tk->nodes[node].occupied &&
            trellis_fingerprint_eq(&tk->nodes[node].fingerprint, member)) {
            target_leaf = i;
            break;
        }
    }

    if (target_leaf == SIZE_MAX)
        return TRELLIS_ERR_NOT_FOUND;

    // Blank the leaf and its path to root.
    size_t target_node = leaf_to_node(target_leaf);
    size_t root = tree_root(tk->member_count);
    size_t cur = target_node;

    while (1) {
        tk->nodes[cur].blanked = true;
        tk->nodes[cur].has_secret = false;
        sodium_memzero(tk->nodes[cur].path_secret, 32);
        if (cur == root)
            break;
        cur = tree_parent(cur);
    }

    tk->nodes[target_node].occupied = false;
    tk->member_count--;
    tk->epoch++;

    // Recompute our path with a fresh secret.
    uint8_t leaf_secret[32];
    randombytes_buf(leaf_secret, sizeof(leaf_secret));

    size_t my_node = leaf_to_node(tk->local_leaf);
    trellis_err_t err = recompute_path(tk, my_node, leaf_secret);
    sodium_memzero(leaf_secret, sizeof(leaf_secret));
    if (err != TRELLIS_OK)
        return err;

    // Build commit (same format as add)
    size_t path_len = 0;
    cur = my_node;
    while (cur != root) {
        cur = tree_parent(cur);
        path_len++;
    }

    size_t entry_size = 4 + TRELLIS_X25519_PK_LEN + TRELLIS_ML_KEM_1024_CT_LEN
                      + TRELLIS_AES_NONCE_LEN + TRELLIS_AES_TAG_LEN + 32;
    size_t commit_len = 4 + 4 + path_len * entry_size;

    *commit_out = trellis_buf_alloc(commit_len);
    if (!commit_out->data)
        return TRELLIS_ERR_NOMEM;

    uint8_t *p = commit_out->data;
    size_t off = 0;

    trellis_write_u32_be(p + off, (uint32_t)tk->epoch);
    off += 4;
    trellis_write_u32_be(p + off, (uint32_t)path_len);
    off += 4;

    cur = my_node;
    while (cur != root) {
        size_t par = tree_parent(cur);
        size_t sib = tree_sibling(cur);
        treekem_node_t *sib_node = &tk->nodes[sib < MAX_NODES ? sib : 0];
        treekem_node_t *par_node = &tk->nodes[par < MAX_NODES ? par : 0];

        trellis_write_u32_be(p + off, (uint32_t)sib);
        off += 4;

        // Hybrid encapsulation for wrap key.
        uint8_t eph_sk[TRELLIS_X25519_SK_LEN], eph_pk[TRELLIS_X25519_PK_LEN];
        uint8_t dh_ss[TRELLIS_X25519_PK_LEN];
        randombytes_buf(eph_sk, sizeof(eph_sk));
        if (crypto_scalarmult_curve25519_base(eph_pk, eph_sk) != 0) {
            sodium_memzero(eph_sk, sizeof(eph_sk));
            trellis_buf_free(commit_out);
            return TRELLIS_ERR_CRYPTO;
        }
        if (crypto_scalarmult_curve25519(dh_ss, eph_sk, sib_node->x25519_pk) != 0)
            sodium_memzero(dh_ss, sizeof(dh_ss));
        sodium_memzero(eph_sk, sizeof(eph_sk));

        memcpy(p + off, eph_pk, TRELLIS_X25519_PK_LEN);
        off += TRELLIS_X25519_PK_LEN;

        uint8_t kem_ct[TRELLIS_ML_KEM_1024_CT_LEN];
        uint8_t kem_ss[TRELLIS_ML_KEM_1024_SS_LEN];
        OQS_KEM *kem = OQS_KEM_new(OQS_KEM_alg_ml_kem_1024);
        if (!kem) {
            sodium_memzero(dh_ss, sizeof(dh_ss));
            trellis_buf_free(commit_out);
            return TRELLIS_ERR_NOMEM;
        }
        if (OQS_KEM_encaps(kem, kem_ct, kem_ss, sib_node->ml_kem_pk) != OQS_SUCCESS) {
            OQS_KEM_free(kem);
            sodium_memzero(dh_ss, sizeof(dh_ss));
            trellis_buf_free(commit_out);
            return TRELLIS_ERR_KEM_ENCAPS;
        }
        OQS_KEM_free(kem);

        memcpy(p + off, kem_ct, TRELLIS_ML_KEM_1024_CT_LEN);
        off += TRELLIS_ML_KEM_1024_CT_LEN;

        uint8_t combined_ss[TRELLIS_X25519_PK_LEN + TRELLIS_ML_KEM_1024_SS_LEN];
        memcpy(combined_ss, dh_ss, TRELLIS_X25519_PK_LEN);
        memcpy(combined_ss + TRELLIS_X25519_PK_LEN, kem_ss, TRELLIS_ML_KEM_1024_SS_LEN);
        sodium_memzero(dh_ss, sizeof(dh_ss));
        sodium_memzero(kem_ss, sizeof(kem_ss));

        uint8_t wrap_key[32];
        static const uint8_t wrap_info[] = "Bloom-TreeKEM-Wrap";
        trellis_err_t hkdf_err = trellis_hkdf_shake256(
                              combined_ss, sizeof(combined_ss),
                              NULL, 0, wrap_info, sizeof(wrap_info) - 1,
                              wrap_key, 32);
        sodium_memzero(combined_ss, sizeof(combined_ss));
        if (hkdf_err != TRELLIS_OK) {
            trellis_buf_free(commit_out);
            return hkdf_err;
        }

        uint8_t enc_nonce[TRELLIS_AES_NONCE_LEN];
        randombytes_buf(enc_nonce, sizeof(enc_nonce));
        memcpy(p + off, enc_nonce, TRELLIS_AES_NONCE_LEN);
        off += TRELLIS_AES_NONCE_LEN;

        uint8_t enc_tag[TRELLIS_AES_TAG_LEN];
        uint8_t enc_secret[32];
        trellis_err_t enc_err = trellis_aes256gcm_encrypt(
            wrap_key, enc_nonce, par_node->path_secret, 32,
            NULL, 0, enc_secret, enc_tag);
        sodium_memzero(wrap_key, sizeof(wrap_key));
        if (enc_err != TRELLIS_OK) {
            sodium_memzero(enc_secret, sizeof(enc_secret));
            trellis_buf_free(commit_out);
            return enc_err;
        }

        memcpy(p + off, enc_tag, TRELLIS_AES_TAG_LEN);
        off += TRELLIS_AES_TAG_LEN;
        memcpy(p + off, enc_secret, 32);
        off += 32;
        sodium_memzero(enc_secret, sizeof(enc_secret));

        cur = par;
    }

    commit_out->len = off;
    return TRELLIS_OK;
}

trellis_err_t trellis_treekem_process_commit(trellis_treekem_t *tk,
                                             const uint8_t *commit, size_t len)
{
    if (!tk || !commit)
        return TRELLIS_ERR_INVALID_ARG;
    if (len < 8)
        return TRELLIS_ERR_INVALID_ARG;

    size_t off = 0;
    uint32_t epoch = trellis_read_u32_be(commit + off);
    off += 4;
    uint32_t n_entries = trellis_read_u32_be(commit + off);
    off += 4;

    /*
     * Walk through entries. If any entry targets a node on our direct path
     * (i.e., the sibling index matches a node where we can decrypt), use
     * our local KEM private key to recover the path secret and propagate.
     */
    size_t my_node = leaf_to_node(tk->local_leaf);
    bool found_our_entry = false;
    uint8_t recovered_secret[32];
    size_t recovered_node = 0;

    for (uint32_t i = 0; i < n_entries; i++) {
        if (off + 4 > len)
            return TRELLIS_ERR_INVALID_ARG;

        uint32_t target_node = trellis_read_u32_be(commit + off);
        off += 4;

        // Read ephemeral X25519 PK.
        if (off + TRELLIS_X25519_PK_LEN > len)
            return TRELLIS_ERR_INVALID_ARG;
        const uint8_t *eph_x25519_pk = commit + off;
        off += TRELLIS_X25519_PK_LEN;

        // Read ML-KEM ciphertext.
        if (off + TRELLIS_ML_KEM_1024_CT_LEN > len)
            return TRELLIS_ERR_INVALID_ARG;
        const uint8_t *kem_ct = commit + off;
        off += TRELLIS_ML_KEM_1024_CT_LEN;

        // Read nonce.
        if (off + TRELLIS_AES_NONCE_LEN > len)
            return TRELLIS_ERR_INVALID_ARG;
        const uint8_t *enc_nonce = commit + off;
        off += TRELLIS_AES_NONCE_LEN;

        // Read tag.
        if (off + TRELLIS_AES_TAG_LEN > len)
            return TRELLIS_ERR_INVALID_ARG;
        const uint8_t *enc_tag = commit + off;
        off += TRELLIS_AES_TAG_LEN;

        // Read encrypted secret.
        if (off + 32 > len)
            return TRELLIS_ERR_INVALID_ARG;
        const uint8_t *enc_secret = commit + off;
        off += 32;

        /*
         * Check if this entry is addressed to us (our node is the target,
         * or we're under the target subtree). Simple check: is target_node
         * our node or an ancestor of ours where we're the sibling?
         */
        bool is_ours = false;
        size_t root = tree_root(tk->member_count);
        size_t walk = my_node;
        while (walk != root) {
            if (tree_sibling(walk) == target_node ||
                walk == target_node) {
                is_ours = true;
                break;
            }
            walk = tree_parent(walk);
        }

        if (is_ours && !found_our_entry) {
            // Decrypt: DH + KEM decaps to get wrap key.
            uint8_t dh_ss[TRELLIS_X25519_PK_LEN];
            if (crypto_scalarmult_curve25519(dh_ss, tk->local_kem.x25519_sk,
                                             eph_x25519_pk) != 0) {
                sodium_memzero(dh_ss, sizeof(dh_ss));
                continue;
            }

            uint8_t kem_ss[TRELLIS_ML_KEM_1024_SS_LEN];
            OQS_KEM *kem = OQS_KEM_new(OQS_KEM_alg_ml_kem_1024);
            if (!kem) {
                sodium_memzero(dh_ss, sizeof(dh_ss));
                continue;
            }
            OQS_STATUS rc = OQS_KEM_decaps(kem, kem_ss, kem_ct,
                                            tk->local_kem.ml_kem_sk);
            OQS_KEM_free(kem);
            if (rc != OQS_SUCCESS) {
                sodium_memzero(dh_ss, sizeof(dh_ss));
                sodium_memzero(kem_ss, sizeof(kem_ss));
                continue;
            }

            uint8_t combined_ss[TRELLIS_X25519_PK_LEN + TRELLIS_ML_KEM_1024_SS_LEN];
            memcpy(combined_ss, dh_ss, TRELLIS_X25519_PK_LEN);
            memcpy(combined_ss + TRELLIS_X25519_PK_LEN, kem_ss,
                   TRELLIS_ML_KEM_1024_SS_LEN);
            sodium_memzero(dh_ss, sizeof(dh_ss));
            sodium_memzero(kem_ss, sizeof(kem_ss));

            uint8_t wrap_key[32];
            static const uint8_t wrap_info[] = "Bloom-TreeKEM-Wrap";
            trellis_err_t hkdf_rc = trellis_hkdf_shake256(
                                  combined_ss, sizeof(combined_ss),
                                  NULL, 0, wrap_info, sizeof(wrap_info) - 1,
                                  wrap_key, 32);
            sodium_memzero(combined_ss, sizeof(combined_ss));
            if (hkdf_rc != TRELLIS_OK) continue;

            // Decrypt the path secret.
            trellis_err_t err = trellis_aes256gcm_decrypt(
                wrap_key, enc_nonce,
                enc_secret, 32,
                NULL, 0,
                enc_tag, recovered_secret);
            sodium_memzero(wrap_key, sizeof(wrap_key));

            if (err == TRELLIS_OK) {
                found_our_entry = true;
                recovered_node = tree_parent(target_node);
            }
        }
    }

    tk->epoch = epoch;

    if (found_our_entry) {
        // Propagate recovered path secret from that node up to root.
        size_t root = tree_root(tk->member_count);
        size_t cur = recovered_node;
        uint8_t secret[32];
        memcpy(secret, recovered_secret, 32);
        sodium_memzero(recovered_secret, sizeof(recovered_secret));

        uint8_t x25519_sk[TRELLIS_X25519_SK_LEN];
        uint8_t ml_kem_sk[TRELLIS_ML_KEM_1024_SK_LEN];

        while (1) {
            treekem_node_t *node = &tk->nodes[cur];
            memcpy(node->path_secret, secret, 32);
            node->has_secret = true;
            node->blanked = false;
            node->occupied = true;

            trellis_err_t dk_err = derive_node_keys(secret,
                           node->x25519_pk, x25519_sk,
                           node->ml_kem_pk, ml_kem_sk);
            if (dk_err != TRELLIS_OK) {
                sodium_memzero(secret, sizeof(secret));
                sodium_memzero(x25519_sk, sizeof(x25519_sk));
                sodium_memzero(ml_kem_sk, sizeof(ml_kem_sk));
                return dk_err;
            }

            if (cur == root) {
                memcpy(tk->epoch_secret, secret, 32);
                break;
            }

            uint8_t parent_secret[32];
            trellis_err_t ps_err = derive_path_secret(secret, parent_secret);
            if (ps_err != TRELLIS_OK) {
                sodium_memzero(secret, sizeof(secret));
                sodium_memzero(parent_secret, sizeof(parent_secret));
                break;
            }
            memcpy(secret, parent_secret, 32);
            sodium_memzero(parent_secret, sizeof(parent_secret));
            cur = tree_parent(cur);
        }

        sodium_memzero(secret, sizeof(secret));
        sodium_memzero(x25519_sk, sizeof(x25519_sk));
        sodium_memzero(ml_kem_sk, sizeof(ml_kem_sk));
    }

    return TRELLIS_OK;
}

trellis_err_t trellis_treekem_epoch_secret(const trellis_treekem_t *tk,
                                           uint8_t *secret_out)
{
    if (!tk || !secret_out)
        return TRELLIS_ERR_INVALID_ARG;

    memcpy(secret_out, tk->epoch_secret, 32);
    return TRELLIS_OK;
}

uint64_t trellis_treekem_epoch(const trellis_treekem_t *tk)
{
    return tk ? tk->epoch : 0;
}

size_t trellis_treekem_member_count(const trellis_treekem_t *tk)
{
    return tk ? tk->member_count : 0;
}
