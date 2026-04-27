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
 * ratchet.c — Post-Quantum Double Ratchet (Section 5.4)
 *
 * Symmetric ratchet advances a chain key with each message.
 * Every TRELLIS_RATCHET_PQC_INTERVAL messages, a PQC ratchet step
 * replaces the chain keys via a fresh ML-KEM-1024 encaps/decaps.
 */

#include "internal.h"

#define CHAIN_KEY_LEN 32
#define MSG_KEY_LEN   32
#define NONCE_LEN     TRELLIS_AES_NONCE_LEN

// ----

/*
 * Wire format per message:
 *   [4] flags (bit 0 = has_pqc_ratchet)
 *   [4] message counter (LE)
 *   [NONCE_LEN] nonce
 *   [TAG_LEN] AES-GCM tag
 *   if has_pqc_ratchet:
 *     [4] pqc_payload_len
 *     [ML_KEM_PK_LEN] new KEM public key   (sender's new pk)
 *     [ML_KEM_CT_LEN] KEM ciphertext        (encaps to receiver's pk)
 *   [ct_len] ciphertext
 */
#define FLAG_PQC_RATCHET 0x01
#define HEADER_BASE_LEN  (4 + 4 + NONCE_LEN + TRELLIS_AES_TAG_LEN)
#define MAX_COUNTER_SKIP 256

struct trellis_ratchet {
    uint8_t send_chain_key[CHAIN_KEY_LEN];
    uint8_t recv_chain_key[CHAIN_KEY_LEN];

    uint32_t send_counter;
    uint32_t recv_counter;

    uint32_t pqc_step_counter;
    uint32_t pqc_interval;

    /*
     * Session-derived nonce prefix (8 bytes).  Mixed into every AES-GCM nonce
     * to ensure nonces are unique across sessions even when the counter starts
     * at zero.  Without this prefix the upper 8 of 12 nonce bytes would be
     * zero in every session, risking nonce reuse when two sessions encrypt
     * their first message under distinct keys derived from the same IV space.
     */
    uint8_t nonce_base[8];

    // Current KEM keypair for PQC ratchet steps.
    trellis_kem_keypair_t local_kem;
    // Remote's current KEM public key.
    uint8_t remote_x25519_pk[TRELLIS_X25519_PK_LEN];
    uint8_t remote_ml_kem_pk[TRELLIS_ML_KEM_1024_PK_LEN];

    trellis_ratchet_stats_t stats;
};

// ----

static trellis_err_t chain_advance(uint8_t *chain_key, uint8_t *msg_key)
{
    static const uint8_t ck_info[] = "Bloom-Chain-Key";
    static const uint8_t mk_info[] = "Bloom-Message-Key";

    // Derive message key.
    trellis_err_t err = trellis_hkdf_shake256(
        chain_key, CHAIN_KEY_LEN,
        NULL, 0,
        mk_info, sizeof(mk_info) - 1,
        msg_key, MSG_KEY_LEN);
    if (err != TRELLIS_OK)
        return err;

    // Advance chain key in place.
    uint8_t next_ck[CHAIN_KEY_LEN];
    err = trellis_hkdf_shake256(
        chain_key, CHAIN_KEY_LEN,
        NULL, 0,
        ck_info, sizeof(ck_info) - 1,
        next_ck, CHAIN_KEY_LEN);
    if (err != TRELLIS_OK) {
        sodium_memzero(msg_key, MSG_KEY_LEN);
        return err;
    }

    memcpy(chain_key, next_ck, CHAIN_KEY_LEN);
    sodium_memzero(next_ck, sizeof(next_ck));
    return TRELLIS_OK;
}

/* Derive a nonce from the session nonce_base (8 bytes) XOR'd with the counter.
 * Layout: [base[0]^ctr[0], base[1]^ctr[1], base[2]^ctr[2], base[3]^ctr[3],
 *          base[4], base[5], base[6], base[7], 0, 0, 0, 0]
 * The counter occupies the low 4 bytes (LE), giving 2^32 unique nonces per
 * session; the upper 8 bytes are session-specific ensuring cross-session
 * uniqueness even at counter == 0. */
static void derive_nonce(const uint8_t nonce_base[8],
                          uint32_t counter, uint8_t *nonce)
{
    uint8_t ctr_le[4];
    trellis_write_u32_be(ctr_le, counter);
    nonce[0] = nonce_base[0] ^ ctr_le[0];
    nonce[1] = nonce_base[1] ^ ctr_le[1];
    nonce[2] = nonce_base[2] ^ ctr_le[2];
    nonce[3] = nonce_base[3] ^ ctr_le[3];
    nonce[4] = nonce_base[4];
    nonce[5] = nonce_base[5];
    nonce[6] = nonce_base[6];
    nonce[7] = nonce_base[7];
    // upper 4 bytes reserved / zero.
    nonce[8] = nonce[9] = nonce[10] = nonce[11] = 0;
}

/*
 * Init from completed handshake with optional domain separation.
 *
 * When domain is non-NULL, a domain-specific session key is derived via
 * HKDF-SHAKE256(session_key, info=domain) before the chain key / nonce-base
 * derivations.  This produces a ratchet whose key schedule is cryptographically
 * independent from one created with a different (or no) domain label, even
 * though both originate from the same handshake shared secret.
 */
trellis_ratchet_t *trellis_ratchet_init_from_handshake_ex(
    const trellis_handshake_t *hs,
    const uint8_t *domain, size_t domain_len)
{
    if (!hs)
        return NULL;

    uint8_t session_key[32];
    if (trellis_handshake_session_key(hs, session_key) != TRELLIS_OK)
        return NULL;

    /* Domain separation: derive a wholly independent key when a label is
     * provided, so two ratchets from the same handshake share no state. */
    if (domain && domain_len > 0) {
        uint8_t domain_key[32];
        trellis_err_t derr = trellis_hkdf_shake256(
            session_key, 32, NULL, 0,
            domain, domain_len,
            domain_key, sizeof(domain_key));
        if (derr != TRELLIS_OK) {
            sodium_memzero(session_key, sizeof(session_key));
            return NULL;
        }
        memcpy(session_key, domain_key, 32);
        sodium_memzero(domain_key, sizeof(domain_key));
    }

    trellis_ratchet_t *r = calloc(1, sizeof(*r));
    if (!r) {
        sodium_memzero(session_key, sizeof(session_key));
        return NULL;
    }

    /*
     * Derive directional chain keys so that one side's send chain
     * matches the other side's recv chain.  The "I2R" key is used
     * by the initiator to send (and by the responder to receive),
     * and "R2I" is the opposite direction.
     */
    static const uint8_t i2r_info[] = "Bloom-Ratchet-I2R";
    static const uint8_t r2i_info[] = "Bloom-Ratchet-R2I";

    const uint8_t *send_info, *recv_info;
    size_t send_info_len, recv_info_len;

    if (trellis_handshake_role(hs) == TRELLIS_ROLE_INITIATOR) {
        send_info = i2r_info;  send_info_len = sizeof(i2r_info) - 1;
        recv_info = r2i_info;  recv_info_len = sizeof(r2i_info) - 1;
    } else {
        send_info = r2i_info;  send_info_len = sizeof(r2i_info) - 1;
        recv_info = i2r_info;  recv_info_len = sizeof(i2r_info) - 1;
    }

    trellis_err_t err;
    err = trellis_hkdf_shake256(session_key, 32, NULL, 0,
                                send_info, send_info_len,
                                r->send_chain_key, CHAIN_KEY_LEN);
    if (err != TRELLIS_OK)
        goto fail;

    err = trellis_hkdf_shake256(session_key, 32, NULL, 0,
                                recv_info, recv_info_len,
                                r->recv_chain_key, CHAIN_KEY_LEN);
    if (err != TRELLIS_OK)
        goto fail;

    /* Derive session-unique nonce_base (8 bytes) so AES-GCM IVs are unique
     * across sessions even at counter == 0. */
    static const uint8_t nb_info[] = "Bloom-Ratchet-NonceBase";
    uint8_t nonce_base_buf[32];
    err = trellis_hkdf_shake256(session_key, 32, NULL, 0,
                                nb_info, sizeof(nb_info) - 1,
                                nonce_base_buf, sizeof(nonce_base_buf));
    if (err != TRELLIS_OK)
        goto fail;
    memcpy(r->nonce_base, nonce_base_buf, sizeof(r->nonce_base));
    sodium_memzero(nonce_base_buf, sizeof(nonce_base_buf));

    sodium_memzero(session_key, sizeof(session_key));

    r->pqc_interval = TRELLIS_RATCHET_PQC_INTERVAL;

    /*
     * Reuse the handshake's own ephemeral KEM keypair so each side's
     * local_kem matches the other side's remote_ml_kem_pk.  Both ratchets
     * get independent memcpy'd copies; their KEM state will diverge after
     * each one's first PQC ratchet step.
     */
    err = trellis_handshake_local_eph_kem(hs, &r->local_kem);
    if (err != TRELLIS_OK)
        goto fail;

    err = trellis_handshake_remote_eph_kem(hs, r->remote_x25519_pk, r->remote_ml_kem_pk);
    if (err != TRELLIS_OK)
        goto fail;

    return r;

fail:
    sodium_memzero(session_key, sizeof(session_key));
    if (r) {
        sodium_memzero(r, sizeof(*r));
        free(r);
    }
    return NULL;
}

trellis_ratchet_t *trellis_ratchet_init_from_handshake(const trellis_handshake_t *hs)
{
    return trellis_ratchet_init_from_handshake_ex(hs, NULL, 0);
}

void trellis_ratchet_free(trellis_ratchet_t *r)
{
    if (!r)
        return;

    sodium_memzero(r, sizeof(*r));
    free(r);
}

// Encrypt
trellis_err_t trellis_ratchet_encrypt(trellis_ratchet_t *r,
                                      const uint8_t *plaintext, size_t pt_len,
                                      trellis_buf_t *out)
{
    if (!r || !out)
        return TRELLIS_ERR_INVALID_ARG;
    if (!plaintext && pt_len > 0)
        return TRELLIS_ERR_INVALID_ARG;

    bool do_pqc = (r->pqc_interval > 0) &&
                  (r->send_counter > 0) &&
                  (r->send_counter % r->pqc_interval == 0);

    /* PQC ratchet step must happen BEFORE chain_advance so the
     * receiver (which also PQC-mixes before chain_advance) derives
     * the same message key. */
    size_t pqc_extra = 0;
    uint8_t kem_ct[TRELLIS_ML_KEM_1024_CT_LEN];
    trellis_kem_keypair_t new_kem;
    trellis_err_t err;

    if (do_pqc) {
        pqc_extra = 4 + TRELLIS_X25519_PK_LEN + TRELLIS_ML_KEM_1024_PK_LEN
                      + TRELLIS_ML_KEM_1024_CT_LEN;

        err = trellis_kem_keygen(&new_kem);
        if (err != TRELLIS_OK)
            return err;

        uint8_t pqc_ss[TRELLIS_ML_KEM_1024_SS_LEN];
        OQS_KEM *kem_obj = OQS_KEM_new(OQS_KEM_alg_ml_kem_1024);
        if (!kem_obj) {
            sodium_memzero(&new_kem, sizeof(new_kem));
            return TRELLIS_ERR_RATCHET;
        }
        OQS_STATUS rc = OQS_KEM_encaps(kem_obj, kem_ct, pqc_ss,
                                        r->remote_ml_kem_pk);
        OQS_KEM_free(kem_obj);
        if (rc != OQS_SUCCESS) {
            sodium_memzero(&new_kem, sizeof(new_kem));
            sodium_memzero(pqc_ss, sizeof(pqc_ss));
            return TRELLIS_ERR_RATCHET;
        }

        static const uint8_t pqc_info[] = "Bloom-PQC-Ratchet";
        err = trellis_hkdf_shake256(pqc_ss, sizeof(pqc_ss),
                                    r->send_chain_key, CHAIN_KEY_LEN,
                                    pqc_info, sizeof(pqc_info) - 1,
                                    r->send_chain_key, CHAIN_KEY_LEN);
        sodium_memzero(pqc_ss, sizeof(pqc_ss));
        if (err != TRELLIS_OK) {
            sodium_memzero(&new_kem, sizeof(new_kem));
            return err;
        }

        sodium_memzero(&r->local_kem, sizeof(r->local_kem));
        memcpy(&r->local_kem, &new_kem, sizeof(new_kem));
        sodium_memzero(&new_kem, sizeof(new_kem));

        r->pqc_step_counter++;
        r->stats.pqc_ratchet_steps++;
    }

    // Guard counter exhaustion BEFORE allocating or encrypting anything.
    if (r->send_counter == UINT32_MAX)
        return TRELLIS_ERR_ENCRYPT;

    // Advance symmetric chain and get message key.
    uint8_t msg_key[MSG_KEY_LEN];
    err = chain_advance(r->send_chain_key, msg_key);
    if (err != TRELLIS_OK)
        return err;

    uint8_t nonce[NONCE_LEN];
    derive_nonce(r->nonce_base, r->send_counter, nonce);

    size_t total = HEADER_BASE_LEN + pqc_extra + pt_len;
    *out = trellis_buf_alloc(total);
    if (!out->data) {
        sodium_memzero(msg_key, sizeof(msg_key));
        return TRELLIS_ERR_NOMEM;
    }

    uint8_t *p = out->data;
    size_t off = 0;

    // Flags
    uint32_t flags = do_pqc ? FLAG_PQC_RATCHET : 0;
    trellis_write_u32_be(p + off, flags);
    off += 4;

    // Counter
    trellis_write_u32_be(p + off, r->send_counter);
    off += 4;

    // Nonce
    memcpy(p + off, nonce, NONCE_LEN);
    off += NONCE_LEN;

    // PQC ratchet payload (placed before tag so it is covered by AEAD AD)
    if (do_pqc) {
        uint32_t pqc_len = TRELLIS_X25519_PK_LEN + TRELLIS_ML_KEM_1024_PK_LEN
                         + TRELLIS_ML_KEM_1024_CT_LEN;
        trellis_write_u32_be(p + off, pqc_len);
        off += 4;

        memcpy(p + off, r->local_kem.x25519_pk, TRELLIS_X25519_PK_LEN);
        off += TRELLIS_X25519_PK_LEN;

        memcpy(p + off, r->local_kem.ml_kem_pk, TRELLIS_ML_KEM_1024_PK_LEN);
        off += TRELLIS_ML_KEM_1024_PK_LEN;

        memcpy(p + off, kem_ct, TRELLIS_ML_KEM_1024_CT_LEN);
        off += TRELLIS_ML_KEM_1024_CT_LEN;
    }

    // Tag immediately after AD region.
    size_t tag_off = off;
    off += TRELLIS_AES_TAG_LEN;

    // Encrypt plaintext; AD covers flags + counter + nonce + PQC payload.
    size_t ct_off = off;
    err = trellis_aes256gcm_encrypt(msg_key, nonce,
                                    plaintext, pt_len,
                                    out->data, tag_off,
                                    p + ct_off,
                                    p + tag_off);

    sodium_memzero(msg_key, sizeof(msg_key));

    if (err != TRELLIS_OK) {
        trellis_buf_free(out);
        return err;
    }

    out->len = ct_off + pt_len;

    r->send_counter++;
    r->stats.messages_sent++;
    r->stats.sym_ratchet_steps++;

    return TRELLIS_OK;
}

// Decrypt
trellis_err_t trellis_ratchet_decrypt(trellis_ratchet_t *r,
                                      const uint8_t *ciphertext, size_t ct_len,
                                      trellis_buf_t *out)
{
    if (!r || !ciphertext || !out)
        return TRELLIS_ERR_INVALID_ARG;
    if (ct_len < HEADER_BASE_LEN)
        return TRELLIS_ERR_DECRYPT;

    const uint8_t *p = ciphertext;
    size_t off = 0;

    uint32_t flags = trellis_read_u32_be(p + off);
    off += 4;

    uint32_t counter = trellis_read_u32_be(p + off);
    off += 4;

    if (counter < r->recv_counter)
        return TRELLIS_ERR_DECRYPT;
    if (counter > r->recv_counter + MAX_COUNTER_SKIP)
        return TRELLIS_ERR_DECRYPT;

    uint32_t skip_count = counter - r->recv_counter;

    const uint8_t *nonce = p + off;
    off += NONCE_LEN;

    bool has_pqc = (flags & FLAG_PQC_RATCHET) != 0;

    /*
     * Snapshot all mutable ratchet state before we begin advancing the chain
     * or mixing PQC material.  If AES-GCM decryption fails (including due to
     * an adversarial message with a crafted high counter), we restore the
     * snapshot so the chain key is not permanently corrupted and future
     * legitimate messages remain decryptable.
     */
    uint8_t saved_recv_chain_key[CHAIN_KEY_LEN];
    uint8_t saved_remote_x25519_pk[TRELLIS_X25519_PK_LEN];
    uint8_t saved_remote_ml_kem_pk[TRELLIS_ML_KEM_1024_PK_LEN];
    memcpy(saved_recv_chain_key, r->recv_chain_key, CHAIN_KEY_LEN);
    memcpy(saved_remote_x25519_pk, r->remote_x25519_pk, TRELLIS_X25519_PK_LEN);
    memcpy(saved_remote_ml_kem_pk, r->remote_ml_kem_pk, TRELLIS_ML_KEM_1024_PK_LEN);

#define RESTORE_CHAIN_SNAPSHOT() do {                                          \
    memcpy(r->recv_chain_key, saved_recv_chain_key, CHAIN_KEY_LEN);            \
    memcpy(r->remote_x25519_pk, saved_remote_x25519_pk, TRELLIS_X25519_PK_LEN); \
    memcpy(r->remote_ml_kem_pk, saved_remote_ml_kem_pk, TRELLIS_ML_KEM_1024_PK_LEN); \
    sodium_memzero(saved_recv_chain_key, sizeof(saved_recv_chain_key));        \
    sodium_memzero(saved_remote_x25519_pk, sizeof(saved_remote_x25519_pk));   \
    sodium_memzero(saved_remote_ml_kem_pk, sizeof(saved_remote_ml_kem_pk));   \
} while (0)

#define CLEANUP_SNAPSHOT() do {                                                \
    sodium_memzero(saved_recv_chain_key, sizeof(saved_recv_chain_key));        \
    sodium_memzero(saved_remote_x25519_pk, sizeof(saved_remote_x25519_pk));   \
    sodium_memzero(saved_remote_ml_kem_pk, sizeof(saved_remote_ml_kem_pk));   \
} while (0)

    // PQC payload sits before the tag (authenticated by AEAD AD)
    if (has_pqc) {
        if (off + 4 > ct_len) {
            CLEANUP_SNAPSHOT();
            return TRELLIS_ERR_DECRYPT;
        }

        uint32_t pqc_len = trellis_read_u32_be(p + off);
        off += 4;

        size_t expected_pqc = TRELLIS_X25519_PK_LEN + TRELLIS_ML_KEM_1024_PK_LEN
                            + TRELLIS_ML_KEM_1024_CT_LEN;
        if (pqc_len != expected_pqc || off + pqc_len > ct_len) {
            CLEANUP_SNAPSHOT();
            return TRELLIS_ERR_DECRYPT;
        }

        // Extract new remote KEM public keys.
        const uint8_t *new_x25519_pk = p + off;
        off += TRELLIS_X25519_PK_LEN;

        const uint8_t *new_ml_kem_pk = p + off;
        off += TRELLIS_ML_KEM_1024_PK_LEN;

        const uint8_t *kem_ct_data = p + off;
        off += TRELLIS_ML_KEM_1024_CT_LEN;

        // Decapsulate PQC shared secret.
        uint8_t pqc_ss[TRELLIS_ML_KEM_1024_SS_LEN];
        OQS_KEM *kem_obj = OQS_KEM_new(OQS_KEM_alg_ml_kem_1024);
        if (!kem_obj) {
            CLEANUP_SNAPSHOT();
            return TRELLIS_ERR_RATCHET;
        }

        OQS_STATUS rc = OQS_KEM_decaps(kem_obj, pqc_ss, kem_ct_data,
                                        r->local_kem.ml_kem_sk);
        OQS_KEM_free(kem_obj);
        if (rc != OQS_SUCCESS) {
            sodium_memzero(pqc_ss, sizeof(pqc_ss));
            CLEANUP_SNAPSHOT();
            return TRELLIS_ERR_RATCHET;
        }

        // Mix PQC shared secret into receiving chain key.
        static const uint8_t pqc_info[] = "Bloom-PQC-Ratchet";
        trellis_err_t err = trellis_hkdf_shake256(
            pqc_ss, sizeof(pqc_ss),
            r->recv_chain_key, CHAIN_KEY_LEN,
            pqc_info, sizeof(pqc_info) - 1,
            r->recv_chain_key, CHAIN_KEY_LEN);
        sodium_memzero(pqc_ss, sizeof(pqc_ss));
        if (err != TRELLIS_OK) {
            RESTORE_CHAIN_SNAPSHOT();
            return err;
        }

        // Tentatively update remote's KEM public keys; rolled back on failure.
        memcpy(r->remote_x25519_pk, new_x25519_pk, TRELLIS_X25519_PK_LEN);
        memcpy(r->remote_ml_kem_pk, new_ml_kem_pk, TRELLIS_ML_KEM_1024_PK_LEN);
    }

    // Tag follows the PQC payload; AD covers everything before the tag.
    const size_t ad_len = off;
    if (off + TRELLIS_AES_TAG_LEN > ct_len) {
        RESTORE_CHAIN_SNAPSHOT();
        return TRELLIS_ERR_DECRYPT;
    }
    const uint8_t *tag = p + off;
    off += TRELLIS_AES_TAG_LEN;

    /* Advance past skipped messages (discard their keys).
     * This mutates recv_chain_key; it will be rolled back via snapshot if
     * the subsequent decryption fails. */
    for (uint32_t s = 0; s < skip_count; s++) {
        uint8_t discard[MSG_KEY_LEN];
        trellis_err_t serr = chain_advance(r->recv_chain_key, discard);
        sodium_memzero(discard, sizeof(discard));
        if (serr != TRELLIS_OK) {
            RESTORE_CHAIN_SNAPSHOT();
            return serr;
        }
    }

    // Advance receiving chain to derive message key.
    uint8_t msg_key[MSG_KEY_LEN];
    trellis_err_t err = chain_advance(r->recv_chain_key, msg_key);
    if (err != TRELLIS_OK) {
        RESTORE_CHAIN_SNAPSHOT();
        return err;
    }

    // Decrypt
    size_t payload_len = ct_len - off;
    *out = trellis_buf_alloc(payload_len > 0 ? payload_len : 1);
    if (!out->data) {
        sodium_memzero(msg_key, sizeof(msg_key));
        RESTORE_CHAIN_SNAPSHOT();
        return TRELLIS_ERR_NOMEM;
    }

    err = trellis_aes256gcm_decrypt(msg_key, nonce,
                                    p + off, payload_len,
                                    p, ad_len,
                                    tag, out->data);
    sodium_memzero(msg_key, sizeof(msg_key));

    if (err != TRELLIS_OK) {
        /* Decryption failed: restore chain key so a crafted bad message cannot
         * permanently corrupt the receiver state and deny future messages. */
        trellis_buf_free(out);
        RESTORE_CHAIN_SNAPSHOT();
        return err;
    }

    // Decryption succeeded — discard the snapshot.
    CLEANUP_SNAPSHOT();

#undef RESTORE_CHAIN_SNAPSHOT
#undef CLEANUP_SNAPSHOT

    out->len = payload_len;
    r->recv_counter = (counter < UINT32_MAX) ? counter + 1 : UINT32_MAX;
    r->stats.messages_received++;
    r->stats.sym_ratchet_steps++;
    if (has_pqc)
        r->stats.pqc_ratchet_steps++;

    return TRELLIS_OK;
}

trellis_ratchet_stats_t trellis_ratchet_stats(const trellis_ratchet_t *r)
{
    if (!r) {
        trellis_ratchet_stats_t empty = {0};
        return empty;
    }
    return r->stats;
}
