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
 * handshake.c — 3-message Noise-adapted hybrid handshake (Section 5.3)
 *
 * Message flow:
 *   Msg1 (I→R): eph_x25519_pk || eph_ml_kem_pk || identity_bundle_I
 *   Msg2 (R→I): eph_x25519_pk || eph_ml_kem_pk || kem_ciphertext
 *               || identity_bundle_R || transcript_sig_R
 *   Msg3 (I→R): identity_bundle_I || transcript_sig_I
 *
 * Transcript authentication:
 *   sig_R = DualSign(R.sk, SHAKE256(prologue||Msg1||Msg2_body))
 *   sig_I = DualSign(I.sk, SHAKE256(prologue||Msg1||Msg2||Msg3_body))
 *
 * After Msg2 the initiator has verified R's identity.
 * After Msg3 the responder has verified I's identity.
 * Both sides derive the session key from the same transcript.
 */

#include "internal.h"

// Forward declarations for dual sign/verify from sign.c.
extern trellis_err_t trellis_dual_sign(const uint8_t *ed25519_sk,
                                       const uint8_t *ml_dsa_sk,
                                       const uint8_t *msg, size_t msg_len,
                                       trellis_signature_t *sig);
extern trellis_err_t trellis_dual_verify(const uint8_t *ed25519_pk,
                                         const uint8_t *ml_dsa_pk,
                                         const uint8_t *msg, size_t msg_len,
                                         const trellis_signature_t *sig);

/*
 * The transcript accumulates: prologue + msg1 + msg2_body + msg2_sig +
 * msg3_body + msg3_sig.  All sizes are protocol-fixed, so we pre-allocate
 * the exact worst-case at handshake creation time — no realloc during the
 * exchange, no silent OOM corruption.
 */
#define HS_PROLOGUE_LEN         27  /* why 27? see protocol spec v1 prologue definition */
#define HS_FIELD(x)             (4 + (size_t)(x))
#define HS_ID_BUNDLE_MAX        (TRELLIS_IDENTITY_PUBLIC_SERIAL_LEN + \
                                 TRELLIS_FINGERPRINT_LEN)

#define HS_MSG1_MAX             (HS_FIELD(TRELLIS_X25519_PK_LEN) +     \
                                 HS_FIELD(TRELLIS_ML_KEM_1024_PK_LEN) + \
                                 HS_FIELD(HS_ID_BUNDLE_MAX))

#define HS_MSG2_BODY_MAX        (HS_FIELD(TRELLIS_X25519_PK_LEN) +     \
                                 HS_FIELD(TRELLIS_ML_KEM_1024_PK_LEN) + \
                                 HS_FIELD(TRELLIS_ML_KEM_1024_CT_LEN) + \
                                 HS_FIELD(HS_ID_BUNDLE_MAX))

#define HS_SIG_FIELD            HS_FIELD(TRELLIS_SIGNATURE_LEN)
#define HS_MSG3_BODY_MAX        HS_FIELD(HS_ID_BUNDLE_MAX)

#define HS_TRANSCRIPT_CAP       (HS_PROLOGUE_LEN + HS_MSG1_MAX +        \
                                 HS_MSG2_BODY_MAX + HS_SIG_FIELD +      \
                                 HS_MSG3_BODY_MAX + HS_SIG_FIELD +      \
                                 256 /* headroom for future fields */)

// Handshake struct (opaque)
struct trellis_handshake {
    trellis_handshake_role_t  role;
    trellis_handshake_state_t state;

    const trellis_identity_t *local_id;
    trellis_identity_public_t remote_pub;
    bool                      remote_pub_set;

    trellis_kem_keypair_t     local_eph;
    trellis_kem_keypair_t     remote_eph;

    /*
     * Fixed-capacity transcript buffer.  All bytes absorbed during the
     * 3-message exchange fit within HS_TRANSCRIPT_CAP, which is derived
     * from the protocol field sizes at compile time.  No dynamic
     * reallocation occurs, eliminating any OOM risk during the handshake.
     */
    trellis_buf_t             transcript_buf;

    uint8_t session_key[32];
};

static trellis_err_t hs_absorb(trellis_handshake_t *hs,
                                const uint8_t *data, size_t len)
{
    if (!len) return TRELLIS_OK;
    size_t need = hs->transcript_buf.len + len;
    if (need > hs->transcript_buf.cap)
        return TRELLIS_ERR_NOMEM;
    memcpy(hs->transcript_buf.data + hs->transcript_buf.len, data, len);
    hs->transcript_buf.len += len;
    return TRELLIS_OK;
}

static trellis_err_t hs_transcript_hash(trellis_handshake_t *hs,
                                        uint8_t *out, size_t out_len)
{
    return trellis_shake256(hs->transcript_buf.data,
                            hs->transcript_buf.len,
                            out, out_len);
}

// Serialize identity_public into a temp buffer for wire format.
static size_t serialize_identity_bundle(const trellis_identity_public_t *pub,
                                        uint8_t *buf, size_t cap)
{
    return trellis_identity_public_serialize(pub, buf, cap);
}

/*
 * Sign the current transcript hash and append the signature as a
 * length-prefixed field to *buf (which must have at least
 * (4 + TRELLIS_SIGNATURE_LEN) bytes of capacity beyond *off).
 * Also absorbs the sig field into the transcript so subsequent
 * signing/verification operates on the same state.
 */
static trellis_err_t hs_sign_transcript(trellis_handshake_t *hs,
                                        trellis_buf_t *buf, size_t *off)
{
    if (*off + 4 + TRELLIS_SIGNATURE_LEN > buf->cap)
        return TRELLIS_ERR_NOMEM;

    uint8_t th[32];
    trellis_err_t err = hs_transcript_hash(hs, th, sizeof(th));
    if (err != TRELLIS_OK)
        return err;

    trellis_signature_t sig;
    err = trellis_dual_sign(hs->local_id->ed25519_sk,
                            hs->local_id->ml_dsa_sk,
                            th, sizeof(th), &sig);
    sodium_memzero(th, sizeof(th));
    if (err != TRELLIS_OK)
        return err;

    size_t sig_field_start = *off;
    trellis_write_u32_be(buf->data + *off, (uint32_t)TRELLIS_SIGNATURE_LEN);
    *off += 4;
    memcpy(buf->data + *off, &sig, TRELLIS_SIGNATURE_LEN);
    *off += TRELLIS_SIGNATURE_LEN;
    buf->len = *off;

    trellis_err_t aerr = hs_absorb(hs, buf->data + sig_field_start,
                                    4 + TRELLIS_SIGNATURE_LEN);
    sodium_memzero(&sig, sizeof(sig));
    return aerr;
}

/*
 * Extract a signature field at the current parse position, verify it
 * against the transcript hash (computed BEFORE absorbing msg bytes
 * past body_end) and absorb the sig field into the transcript.
 *
 * msg      : full received message buffer
 * body_end : offset where the body ends (sig field starts)
 * off      : updated to point past the sig field on success
 * remote   : the sender's public identity for verification
 */
static trellis_err_t hs_verify_transcript_sig(trellis_handshake_t *hs,
                                              const uint8_t *msg,
                                              size_t msg_len,
                                              size_t body_end,
                                              size_t *off,
                                              const trellis_identity_public_t *remote)
{
    // Parse sig field starting at body_end.
    *off = body_end;
    const uint8_t *sig_bytes;
    size_t sig_len;
    if (trellis_buf_read_field(msg, msg_len, off,
                               &sig_bytes, &sig_len) != 0)
        return TRELLIS_ERR_HANDSHAKE;
    if (sig_len != TRELLIS_SIGNATURE_LEN)
        return TRELLIS_ERR_HANDSHAKE;

    // Compute transcript hash over everything absorbed so far (body only)
    uint8_t th[32];
    trellis_err_t err = hs_transcript_hash(hs, th, sizeof(th));
    if (err != TRELLIS_OK)
        return err;

    // Verify dual signature.
    trellis_signature_t sig;
    memcpy(&sig, sig_bytes, TRELLIS_SIGNATURE_LEN);
    err = trellis_dual_verify(remote->ed25519_pk, remote->ml_dsa_pk,
                              th, sizeof(th), &sig);
    sodium_memzero(th, sizeof(th));
    sodium_memzero(&sig, sizeof(sig));
    if (err != TRELLIS_OK)
        return TRELLIS_ERR_VERIFY_FAILED;

    // Absorb sig field (length prefix + bytes) into transcript.
    return hs_absorb(hs, msg + body_end, 4 + TRELLIS_SIGNATURE_LEN);
}

// Public API.
trellis_handshake_t *trellis_handshake_new(const trellis_identity_t *local_id,
                                           trellis_handshake_role_t role)
{
    if (!local_id)
        return NULL;

    trellis_handshake_t *hs = calloc(1, sizeof(*hs));
    if (!hs)
        return NULL;

    hs->role     = role;
    hs->state    = TRELLIS_HS_INIT;
    hs->local_id = local_id;
    hs->remote_pub_set = false;

    hs->transcript_buf = trellis_buf_alloc(HS_TRANSCRIPT_CAP);
    if (!hs->transcript_buf.data) {
        free(hs);
        return NULL;
    }
    hs->transcript_buf.len = 0;

    // Domain separation.
    static const uint8_t prologue[] = "Bloom-Protocol-v1-Handshake";
    if (hs_absorb(hs, prologue, sizeof(prologue) - 1) != TRELLIS_OK) {
        free(hs->transcript_buf.data);
        free(hs);
        return NULL;
    }

    return hs;
}

void trellis_handshake_free(trellis_handshake_t *hs)
{
    if (!hs)
        return;

    sodium_memzero(&hs->local_eph, sizeof(hs->local_eph));
    sodium_memzero(&hs->remote_eph, sizeof(hs->remote_eph));
    sodium_memzero(hs->session_key, sizeof(hs->session_key));

    if (hs->transcript_buf.data) {
        sodium_memzero(hs->transcript_buf.data, hs->transcript_buf.len);
        free(hs->transcript_buf.data);
    }

    sodium_memzero(hs, sizeof(*hs));
    free(hs);
}

// Message 1: Initiator → Responder.
trellis_err_t trellis_handshake_create_msg1(trellis_handshake_t *hs,
                                            trellis_buf_t *out)
{
    if (!hs || !out)
        return TRELLIS_ERR_INVALID_ARG;
    if (hs->role != TRELLIS_ROLE_INITIATOR || hs->state != TRELLIS_HS_INIT)
        return TRELLIS_ERR_HANDSHAKE;

    /*
     * Generate ephemeral KEM keypair.  For the X25519 component, use
     * Elligator2-representable keys so the public key bytes appear
     * uniformly random on the wire (DPI resistance Layer 1C).
     * Falls back to standard keygen if Elligator2 fails.
     */
    trellis_err_t err = trellis_kem_keygen(&hs->local_eph);
    if (err != TRELLIS_OK)
        return err;

    uint8_t x25519_repr[TRELLIS_X25519_PK_LEN];
    {
        bool have_repr = false;
        for (int _elig_try = 0; _elig_try < 64 && !have_repr; _elig_try++) {
            uint8_t elig_sk[32], elig_pk[32], elig_repr[32];
            if (trellis_elligator2_keygen(elig_sk, elig_pk, elig_repr) == TRELLIS_OK) {
                memcpy(hs->local_eph.x25519_sk, elig_sk, 32);
                memcpy(hs->local_eph.x25519_pk, elig_pk, 32);
                memcpy(x25519_repr, elig_repr, 32);
                sodium_memzero(elig_sk, 32);
                have_repr = true;
            }
            sodium_memzero(elig_sk, sizeof(elig_sk));
        }
        if (!have_repr)
            return TRELLIS_ERR_HANDSHAKE;
    }

    // Serialize identity bundle.
    uint8_t id_buf[TRELLIS_IDENTITY_PUBLIC_SERIAL_LEN + TRELLIS_FINGERPRINT_LEN];
    trellis_identity_public_t pub;
    trellis_identity_public_from(hs->local_id, &pub);
    size_t id_len = serialize_identity_bundle(&pub, id_buf, sizeof(id_buf));
    if (id_len == 0)
        return TRELLIS_ERR_HANDSHAKE;

    // Wire format: [x25519_repr][ml_kem_pk][identity_bundle] all length-prefixed.
    size_t max_len = (4 + TRELLIS_X25519_PK_LEN) +
                     (4 + TRELLIS_ML_KEM_1024_PK_LEN) +
                     (4 + id_len);

    *out = trellis_buf_alloc(max_len);
    if (!out->data)
        return TRELLIS_ERR_NOMEM;

    size_t off = 0;
    if (trellis_buf_append_field(out->data, out->cap, &off,
                                 x25519_repr, TRELLIS_X25519_PK_LEN) != 0)
        goto fail;
    if (trellis_buf_append_field(out->data, out->cap, &off,
                                 hs->local_eph.ml_kem_pk, TRELLIS_ML_KEM_1024_PK_LEN) != 0)
        goto fail;
    if (trellis_buf_append_field(out->data, out->cap, &off,
                                 id_buf, id_len) != 0)
        goto fail;

    out->len = off;

    // Absorb msg1 into transcript.
    err = hs_absorb(hs, out->data, out->len);
    if (err != TRELLIS_OK)
        goto fail;

    hs->state = TRELLIS_HS_AWAITING_MSG2;
    return TRELLIS_OK;

fail:
    trellis_buf_free(out);
    return TRELLIS_ERR_HANDSHAKE;
}

// Process Message 1 (Responder receives) → produce Message 2.
trellis_err_t trellis_handshake_process_msg1(trellis_handshake_t *hs,
                                             const uint8_t *msg, size_t len,
                                             trellis_buf_t *out)
{
    if (!hs || !msg || !out)
        return TRELLIS_ERR_INVALID_ARG;
    if (hs->role != TRELLIS_ROLE_RESPONDER || hs->state != TRELLIS_HS_INIT)
        return TRELLIS_ERR_HANDSHAKE;

    // Absorb msg1 into transcript.
    trellis_err_t err = hs_absorb(hs, msg, len);
    if (err != TRELLIS_OK)
        return err;

    // Parse msg1 fields.
    size_t off = 0;
    const uint8_t *peer_x25519_repr, *peer_ml_kem_pk, *id_data;
    size_t peer_x25519_len, peer_ml_kem_len, id_data_len;

    if (trellis_buf_read_field(msg, len, &off, &peer_x25519_repr, &peer_x25519_len) != 0 ||
        peer_x25519_len != TRELLIS_X25519_PK_LEN)
        return TRELLIS_ERR_HANDSHAKE;

    if (trellis_buf_read_field(msg, len, &off, &peer_ml_kem_pk, &peer_ml_kem_len) != 0 ||
        peer_ml_kem_len != TRELLIS_ML_KEM_1024_PK_LEN)
        return TRELLIS_ERR_HANDSHAKE;

    if (trellis_buf_read_field(msg, len, &off, &id_data, &id_data_len) != 0)
        return TRELLIS_ERR_HANDSHAKE;

    // Deserialize remote identity.
    err = trellis_identity_public_deserialize(id_data, id_data_len,
                                              &hs->remote_pub);
    if (err != TRELLIS_OK)
        return err;
    hs->remote_pub_set = true;

    /* Decode Elligator2 representative to recover X25519 public key.
       Strip the high bit (randomized for entropy) before decoding. */
    uint8_t repr_clean[TRELLIS_X25519_PK_LEN];
    memcpy(repr_clean, peer_x25519_repr, TRELLIS_X25519_PK_LEN);
    repr_clean[31] &= 0x7F;

    uint8_t peer_x25519_pk_decoded[TRELLIS_X25519_PK_LEN];
    if (trellis_elligator2_decode(repr_clean, peer_x25519_pk_decoded) != TRELLIS_OK)
        return TRELLIS_ERR_HANDSHAKE;

    // Store remote ephemeral public keys.
    memcpy(hs->remote_eph.x25519_pk, peer_x25519_pk_decoded, TRELLIS_X25519_PK_LEN);
    memcpy(hs->remote_eph.ml_kem_pk, peer_ml_kem_pk, TRELLIS_ML_KEM_1024_PK_LEN);

    /*
     * Generate our ephemeral KEM keypair.  Use Elligator2-representable
     * X25519 keys so our public key also looks random on the wire.
     */
    err = trellis_kem_keygen(&hs->local_eph);
    if (err != TRELLIS_OK)
        return err;

    uint8_t resp_x25519_repr[TRELLIS_X25519_PK_LEN];
    {
        bool have_repr = false;
        for (int _elig_try = 0; _elig_try < 64 && !have_repr; _elig_try++) {
            uint8_t elig_sk[32], elig_pk[32], elig_repr[32];
            if (trellis_elligator2_keygen(elig_sk, elig_pk, elig_repr) == TRELLIS_OK) {
                memcpy(hs->local_eph.x25519_sk, elig_sk, 32);
                memcpy(hs->local_eph.x25519_pk, elig_pk, 32);
                memcpy(resp_x25519_repr, elig_repr, 32);
                sodium_memzero(elig_sk, 32);
                have_repr = true;
            }
            sodium_memzero(elig_sk, sizeof(elig_sk));
        }
        if (!have_repr)
            return TRELLIS_ERR_HANDSHAKE;
    }

    // Perform X25519 DH with remote ephemeral.
    uint8_t dh_secret[TRELLIS_X25519_PK_LEN];
    if (crypto_scalarmult_curve25519(dh_secret, hs->local_eph.x25519_sk,
                                     peer_x25519_pk_decoded) != 0) {
        sodium_memzero(dh_secret, sizeof(dh_secret));
        return TRELLIS_ERR_HANDSHAKE;
    }

    // ML-KEM encapsulation against remote ephemeral PQC key.
    uint8_t kem_ct[TRELLIS_ML_KEM_1024_CT_LEN];
    uint8_t kem_ss[TRELLIS_ML_KEM_1024_SS_LEN];
    uint8_t combined[TRELLIS_X25519_PK_LEN + TRELLIS_ML_KEM_1024_SS_LEN];
    uint8_t th[32];
    trellis_err_t ret = TRELLIS_ERR_HANDSHAKE;

    OQS_KEM *kem = OQS_KEM_new(OQS_KEM_alg_ml_kem_1024);
    if (!kem)
        goto cleanup;
    OQS_STATUS rc = OQS_KEM_encaps(kem, kem_ct, kem_ss, peer_ml_kem_pk);
    OQS_KEM_free(kem);
    if (rc != OQS_SUCCESS)
        goto cleanup;

    // Build responder identity bundle.
    uint8_t id_buf[TRELLIS_IDENTITY_PUBLIC_SERIAL_LEN + TRELLIS_FINGERPRINT_LEN];
    trellis_identity_public_t pub;
    trellis_identity_public_from(hs->local_id, &pub);
    size_t id_len = serialize_identity_bundle(&pub, id_buf, sizeof(id_buf));
    if (id_len == 0)
        goto cleanup;

    // Build msg2: [x25519_repr][ml_kem_pk][kem_ct][identity_bundle][transcript_sig_R]
    size_t max_len = (4 + TRELLIS_X25519_PK_LEN) +
                     (4 + TRELLIS_ML_KEM_1024_PK_LEN) +
                     (4 + TRELLIS_ML_KEM_1024_CT_LEN) +
                     (4 + id_len) +
                     (4 + TRELLIS_SIGNATURE_LEN);

    *out = trellis_buf_alloc(max_len);
    if (!out->data) {
        ret = TRELLIS_ERR_NOMEM;
        goto cleanup;
    }

    size_t woff = 0;
    if (trellis_buf_append_field(out->data, out->cap, &woff,
                                 resp_x25519_repr, TRELLIS_X25519_PK_LEN) != 0 ||
        trellis_buf_append_field(out->data, out->cap, &woff,
                                 hs->local_eph.ml_kem_pk, TRELLIS_ML_KEM_1024_PK_LEN) != 0 ||
        trellis_buf_append_field(out->data, out->cap, &woff,
                                 kem_ct, TRELLIS_ML_KEM_1024_CT_LEN) != 0 ||
        trellis_buf_append_field(out->data, out->cap, &woff,
                                 id_buf, id_len) != 0)
        goto cleanup_buf;

    err = hs_absorb(hs, out->data, woff);
    if (err != TRELLIS_OK) { ret = err; goto cleanup_buf; }

    err = hs_sign_transcript(hs, out, &woff);
    if (err != TRELLIS_OK) { ret = err; goto cleanup_buf; }

    // Derive session_key = HKDF-SHAKE-256(dh || kem_ss, TH, INFO_FINAL)
    memcpy(combined, dh_secret, TRELLIS_X25519_PK_LEN);
    memcpy(combined + TRELLIS_X25519_PK_LEN, kem_ss, TRELLIS_ML_KEM_1024_SS_LEN);

    err = hs_transcript_hash(hs, th, sizeof(th));
    if (err != TRELLIS_OK) { ret = err; goto cleanup_buf; }

    err = trellis_hkdf_shake256(combined, sizeof(combined),
                                th, sizeof(th),
                                (const uint8_t *)TRELLIS_HS_INFO_FINAL,
                                strlen(TRELLIS_HS_INFO_FINAL),
                                hs->session_key, 32);
    if (err != TRELLIS_OK) { ret = err; goto cleanup_buf; }

    hs->state = TRELLIS_HS_AWAITING_MSG3;
    ret = TRELLIS_OK;
    goto cleanup;

cleanup_buf:
    trellis_buf_free(out);
cleanup:
    sodium_memzero(dh_secret, sizeof(dh_secret));
    sodium_memzero(kem_ss, sizeof(kem_ss));
    sodium_memzero(combined, sizeof(combined));
    sodium_memzero(th, sizeof(th));
    return ret;
}

// Process Message 2 (Initiator receives) → produce Message 3.
trellis_err_t trellis_handshake_process_msg2(trellis_handshake_t *hs,
                                             const uint8_t *msg, size_t len,
                                             trellis_buf_t *out)
{
    if (!hs || !msg || !out)
        return TRELLIS_ERR_INVALID_ARG;
    if (hs->role != TRELLIS_ROLE_INITIATOR || hs->state != TRELLIS_HS_AWAITING_MSG2)
        return TRELLIS_ERR_HANDSHAKE;

    /* Parse msg2 body fields first (do NOT absorb the whole message at once;
     * we need to absorb body and sig separately to verify the signature). */
    size_t off = 0;
    const uint8_t *peer_x25519_repr, *peer_ml_kem_pk, *kem_ct_data, *id_data;
    size_t peer_x25519_len, peer_ml_kem_len, kem_ct_len, id_data_len;

    if (trellis_buf_read_field(msg, len, &off, &peer_x25519_repr, &peer_x25519_len) != 0 ||
        peer_x25519_len != TRELLIS_X25519_PK_LEN)
        return TRELLIS_ERR_HANDSHAKE;

    if (trellis_buf_read_field(msg, len, &off, &peer_ml_kem_pk, &peer_ml_kem_len) != 0 ||
        peer_ml_kem_len != TRELLIS_ML_KEM_1024_PK_LEN)
        return TRELLIS_ERR_HANDSHAKE;

    if (trellis_buf_read_field(msg, len, &off, &kem_ct_data, &kem_ct_len) != 0 ||
        kem_ct_len != TRELLIS_ML_KEM_1024_CT_LEN)
        return TRELLIS_ERR_HANDSHAKE;

    if (trellis_buf_read_field(msg, len, &off, &id_data, &id_data_len) != 0)
        return TRELLIS_ERR_HANDSHAKE;

    // Deserialize remote identity from the body.
    trellis_err_t err = trellis_identity_public_deserialize(id_data, id_data_len,
                                                            &hs->remote_pub);
    if (err != TRELLIS_OK)
        return err;
    hs->remote_pub_set = true;

    /* `off` now points to the start of the signature field.
     * Absorb the body bytes into the transcript, then verify the sig. */
    size_t body_end = off;
    err = hs_absorb(hs, msg, body_end);
    if (err != TRELLIS_OK)
        return err;

    err = hs_verify_transcript_sig(hs, msg, len, body_end, &off,
                                   &hs->remote_pub);
    if (err != TRELLIS_OK)
        return err;

    // Decode Elligator2 representative from msg2.
    uint8_t msg2_repr_clean[TRELLIS_X25519_PK_LEN];
    memcpy(msg2_repr_clean, peer_x25519_repr, TRELLIS_X25519_PK_LEN);
    msg2_repr_clean[31] &= 0x7F;

    uint8_t peer_x25519_pk[TRELLIS_X25519_PK_LEN];
    if (trellis_elligator2_decode(msg2_repr_clean, peer_x25519_pk) != TRELLIS_OK)
        return TRELLIS_ERR_HANDSHAKE;

    // Store remote ephemeral public keys for later use by the ratchet.
    memcpy(hs->remote_eph.x25519_pk, peer_x25519_pk, TRELLIS_X25519_PK_LEN);
    memcpy(hs->remote_eph.ml_kem_pk, peer_ml_kem_pk, TRELLIS_ML_KEM_1024_PK_LEN);

    // X25519 DH with our ephemeral SK and remote ephemeral PK.
    uint8_t dh_secret[TRELLIS_X25519_PK_LEN];
    if (crypto_scalarmult_curve25519(dh_secret, hs->local_eph.x25519_sk,
                                     peer_x25519_pk) != 0) {
        sodium_memzero(dh_secret, sizeof(dh_secret));
        return TRELLIS_ERR_HANDSHAKE;
    }

    // ML-KEM decapsulation.
    uint8_t kem_ss[TRELLIS_ML_KEM_1024_SS_LEN];
    OQS_KEM *kem = OQS_KEM_new(OQS_KEM_alg_ml_kem_1024);
    if (!kem) {
        sodium_memzero(dh_secret, sizeof(dh_secret));
        return TRELLIS_ERR_HANDSHAKE;
    }
    OQS_STATUS rc = OQS_KEM_decaps(kem, kem_ss, kem_ct_data, hs->local_eph.ml_kem_sk);
    OQS_KEM_free(kem);
    if (rc != OQS_SUCCESS) {
        sodium_memzero(dh_secret, sizeof(dh_secret));
        sodium_memzero(kem_ss, sizeof(kem_ss));
        return TRELLIS_ERR_HANDSHAKE;
    }

    // Derive session_key = HKDF-SHAKE-256(dh || kem_ss, transcript, info_final)
    uint8_t combined[TRELLIS_X25519_PK_LEN + TRELLIS_ML_KEM_1024_SS_LEN];
    memcpy(combined, dh_secret, TRELLIS_X25519_PK_LEN);
    memcpy(combined + TRELLIS_X25519_PK_LEN, kem_ss, TRELLIS_ML_KEM_1024_SS_LEN);

    uint8_t th[32];
    err = hs_transcript_hash(hs, th, sizeof(th));
    if (err != TRELLIS_OK) {
        sodium_memzero(dh_secret, sizeof(dh_secret));
        sodium_memzero(kem_ss, sizeof(kem_ss));
        sodium_memzero(combined, sizeof(combined));
        return err;
    }

    err = trellis_hkdf_shake256(combined, sizeof(combined),
                                th, sizeof(th),
                                (const uint8_t *)TRELLIS_HS_INFO_FINAL,
                                strlen(TRELLIS_HS_INFO_FINAL),
                                hs->session_key, 32);

    sodium_memzero(dh_secret, sizeof(dh_secret));
    sodium_memzero(kem_ss, sizeof(kem_ss));
    sodium_memzero(combined, sizeof(combined));
    sodium_memzero(th, sizeof(th));

    if (err != TRELLIS_OK)
        return err;

    /* Build msg3: initiator's identity bundle + transcript_sig_I
     * Proves the initiator owns its static identity, completing mutual auth. */
    uint8_t id_buf[TRELLIS_IDENTITY_PUBLIC_SERIAL_LEN + TRELLIS_FINGERPRINT_LEN];
    trellis_identity_public_t pub;
    trellis_identity_public_from(hs->local_id, &pub);
    size_t id_len = serialize_identity_bundle(&pub, id_buf, sizeof(id_buf));
    if (id_len == 0)
        return TRELLIS_ERR_HANDSHAKE;

    size_t max_len = (4 + id_len) + (4 + TRELLIS_SIGNATURE_LEN);
    *out = trellis_buf_alloc(max_len);
    if (!out->data)
        return TRELLIS_ERR_NOMEM;

    size_t woff = 0;
    if (trellis_buf_append_field(out->data, out->cap, &woff,
                                 id_buf, id_len) != 0) {
        trellis_buf_free(out);
        return TRELLIS_ERR_HANDSHAKE;
    }

    // Absorb msg3 body before signing.
    err = hs_absorb(hs, out->data, woff);
    if (err != TRELLIS_OK) {
        trellis_buf_free(out);
        return err;
    }

    // Append initiator's transcript signature.
    err = hs_sign_transcript(hs, out, &woff);
    if (err != TRELLIS_OK) {
        trellis_buf_free(out);
        return err;
    }

    hs->state = TRELLIS_HS_COMPLETE;
    return TRELLIS_OK;
}

trellis_err_t trellis_handshake_process_msg3(trellis_handshake_t *hs,
                                             const uint8_t *msg, size_t len)
{
    if (!hs || !msg)
        return TRELLIS_ERR_INVALID_ARG;
    if (hs->role != TRELLIS_ROLE_RESPONDER || hs->state != TRELLIS_HS_AWAITING_MSG3)
        return TRELLIS_ERR_HANDSHAKE;

    // Parse msg3 body (identity bundle) without absorbing signature yet.
    size_t off = 0;
    const uint8_t *id_data;
    size_t id_data_len;
    if (trellis_buf_read_field(msg, len, &off, &id_data, &id_data_len) != 0)
        return TRELLIS_ERR_HANDSHAKE;

    // Verify that msg3 identity matches msg1 identity.
    trellis_identity_public_t confirm_pub;
    trellis_err_t err = trellis_identity_public_deserialize(id_data, id_data_len,
                                                            &confirm_pub);
    if (err != TRELLIS_OK)
        return err;

    if (!trellis_fingerprint_eq(&confirm_pub.fingerprint, &hs->remote_pub.fingerprint))
        return TRELLIS_ERR_VERIFY_FAILED;

    // Absorb msg3 body, then verify initiator's transcript signature.
    size_t body_end = off;
    err = hs_absorb(hs, msg, body_end);
    if (err != TRELLIS_OK)
        return err;

    err = hs_verify_transcript_sig(hs, msg, len, body_end, &off,
                                   &hs->remote_pub);
    if (err != TRELLIS_OK)
        return err;

    // session_key was already derived in process_msg1.
    hs->state = TRELLIS_HS_COMPLETE;
    return TRELLIS_OK;
}

// Accessors
trellis_handshake_state_t trellis_handshake_state(const trellis_handshake_t *hs)
{
    return hs ? hs->state : TRELLIS_HS_INIT;
}

trellis_handshake_role_t trellis_handshake_role(const trellis_handshake_t *hs)
{
    return hs ? hs->role : TRELLIS_ROLE_INITIATOR;
}

trellis_err_t trellis_handshake_session_key(const trellis_handshake_t *hs,
                                            uint8_t *key_out)
{
    if (!hs || !key_out)
        return TRELLIS_ERR_INVALID_ARG;
    if (hs->state != TRELLIS_HS_COMPLETE)
        return TRELLIS_ERR_HANDSHAKE;

    memcpy(key_out, hs->session_key, 32);
    return TRELLIS_OK;
}

trellis_err_t trellis_handshake_remote_identity(const trellis_handshake_t *hs,
                                                trellis_identity_public_t *out)
{
    if (!hs || !out)
        return TRELLIS_ERR_INVALID_ARG;
    if (!hs->remote_pub_set)
        return TRELLIS_ERR_HANDSHAKE;

    memcpy(out, &hs->remote_pub, sizeof(*out));
    return TRELLIS_OK;
}

trellis_err_t trellis_handshake_remote_eph_kem(const trellis_handshake_t *hs,
                                               uint8_t *x25519_pk,
                                               uint8_t *ml_kem_pk)
{
    if (!hs || !x25519_pk || !ml_kem_pk)
        return TRELLIS_ERR_INVALID_ARG;
    /* Remote ephemeral keys are available after msg1 is processed (responder)
     * or after msg2 is processed (initiator, state >= AWAITING_MSG3). */
    if (hs->role == TRELLIS_ROLE_INITIATOR) {
        if (hs->state < TRELLIS_HS_AWAITING_MSG3)
            return TRELLIS_ERR_HANDSHAKE;
    } else {
        if (hs->state < TRELLIS_HS_AWAITING_MSG3)
            return TRELLIS_ERR_HANDSHAKE;
    }

    memcpy(x25519_pk, hs->remote_eph.x25519_pk, TRELLIS_X25519_PK_LEN);
    memcpy(ml_kem_pk, hs->remote_eph.ml_kem_pk, TRELLIS_ML_KEM_1024_PK_LEN);
    return TRELLIS_OK;
}

trellis_err_t trellis_handshake_local_eph_kem(const trellis_handshake_t *hs,
                                              trellis_kem_keypair_t *kp)
{
    if (!hs || !kp)
        return TRELLIS_ERR_INVALID_ARG;
    if (hs->state < TRELLIS_HS_AWAITING_MSG2)
        return TRELLIS_ERR_HANDSHAKE;
    memcpy(kp, &hs->local_eph, sizeof(*kp));
    return TRELLIS_OK;
}

// Handshake obfuscation.

/*
 * Wire format: [24-byte nonce][encrypted plaintext][16-byte tag]
 * Plaintext:   [4-byte LE original_len][original_data][random_padding]
 * Total output is always TRELLIS_OBFS_PAD_SIZE bytes.
 */
#define OBFS_OVERHEAD (TRELLIS_OBFS_NONCE_LEN + TRELLIS_OBFS_TAG_LEN)
#define OBFS_PT_CAP   (TRELLIS_OBFS_PAD_SIZE - OBFS_OVERHEAD)
#define OBFS_LEN_HDR  4

trellis_err_t trellis_handshake_obfuscate(const uint8_t *msg, size_t msg_len,
                                          const uint8_t *obfs_key,
                                          trellis_buf_t *out)
{
    if (!msg || !obfs_key || !out)
        return TRELLIS_ERR_INVALID_ARG;
    if (msg_len + OBFS_LEN_HDR > OBFS_PT_CAP)
        return TRELLIS_ERR_INVALID_ARG;

    *out = trellis_buf_alloc(TRELLIS_OBFS_PAD_SIZE);
    if (!out->data)
        return TRELLIS_ERR_NOMEM;
    out->len = TRELLIS_OBFS_PAD_SIZE;

    uint8_t *nonce = out->data;
    randombytes_buf(nonce, TRELLIS_OBFS_NONCE_LEN);

    uint8_t *pt = malloc(OBFS_PT_CAP);
    if (!pt) { trellis_buf_free(out); return TRELLIS_ERR_NOMEM; }

    trellis_write_u32_be(pt, (uint32_t)msg_len);
    memcpy(pt + OBFS_LEN_HDR, msg, msg_len);
    randombytes_buf(pt + OBFS_LEN_HDR + msg_len,
                    OBFS_PT_CAP - OBFS_LEN_HDR - msg_len);

    unsigned long long ct_len;
    if (crypto_aead_xchacha20poly1305_ietf_encrypt(
            out->data + TRELLIS_OBFS_NONCE_LEN, &ct_len,
            pt, OBFS_PT_CAP,
            NULL, 0,
            NULL,
            nonce, obfs_key) != 0) {
        sodium_memzero(pt, OBFS_PT_CAP);
        free(pt);
        trellis_buf_free(out);
        return TRELLIS_ERR_ENCRYPT;
    }

    sodium_memzero(pt, OBFS_PT_CAP);
    free(pt);
    return TRELLIS_OK;
}

trellis_err_t trellis_handshake_deobfuscate(const uint8_t *data, size_t data_len,
                                            const uint8_t *obfs_key,
                                            trellis_buf_t *out)
{
    if (!data || !obfs_key || !out)
        return TRELLIS_ERR_INVALID_ARG;
    if (data_len != TRELLIS_OBFS_PAD_SIZE)
        return TRELLIS_ERR_INVALID_ARG;

    const uint8_t *nonce = data;
    const uint8_t *ct = data + TRELLIS_OBFS_NONCE_LEN;
    size_t ct_len = data_len - TRELLIS_OBFS_NONCE_LEN;

    uint8_t *pt = malloc(OBFS_PT_CAP);
    if (!pt) return TRELLIS_ERR_NOMEM;

    unsigned long long pt_len;
    if (crypto_aead_xchacha20poly1305_ietf_decrypt(
            pt, &pt_len,
            NULL,
            ct, ct_len,
            NULL, 0,
            nonce, obfs_key) != 0) {
        sodium_memzero(pt, OBFS_PT_CAP);
        free(pt);
        return TRELLIS_ERR_DECRYPT;
    }

    if (pt_len < OBFS_LEN_HDR) {
        sodium_memzero(pt, OBFS_PT_CAP);
        free(pt);
        return TRELLIS_ERR_DECODE;
    }

    uint32_t msg_len = trellis_read_u32_be(pt);
    if (OBFS_LEN_HDR + (size_t)msg_len > pt_len) {
        sodium_memzero(pt, OBFS_PT_CAP);
        free(pt);
        return TRELLIS_ERR_DECODE;
    }

    *out = trellis_buf_from(pt + OBFS_LEN_HDR, msg_len);
    sodium_memzero(pt, OBFS_PT_CAP);
    free(pt);
    if (!out->data && msg_len > 0)
        return TRELLIS_ERR_NOMEM;

    return TRELLIS_OK;
}

trellis_err_t trellis_derive_obfs_key_epoch(const uint8_t *server_pk,
                                             size_t pk_len,
                                             uint64_t epoch,
                                             uint8_t *obfs_key_out)
{
    if (!server_pk || !obfs_key_out || pk_len == 0 || pk_len > 4096)
        return TRELLIS_ERR_INVALID_ARG;

    uint8_t epoch_le[8];
    for (int i = 0; i < 8; i++)
        epoch_le[i] = (uint8_t)(epoch >> (i * 8));

    size_t ikm_len = pk_len + sizeof(epoch_le);
    uint8_t *ikm = malloc(ikm_len);
    if (!ikm)
        return TRELLIS_ERR_NOMEM;
    memcpy(ikm, server_pk, pk_len);
    memcpy(ikm + pk_len, epoch_le, sizeof(epoch_le));

    trellis_err_t err = trellis_hkdf_shake256(
        ikm, ikm_len,
        (const uint8_t *)"bloom-obfs-key", 14,
        (const uint8_t *)"handshake-obfuscation-v1", 24,
        obfs_key_out, TRELLIS_OBFS_KEY_LEN);

    sodium_memzero(ikm, ikm_len);
    free(ikm);
    return err;
}

trellis_err_t trellis_derive_obfs_key(const uint8_t *server_pk,
                                      size_t pk_len,
                                      uint8_t *obfs_key_out)
{
    /*
     * Include the current UTC hour as an 8-byte little-endian epoch so the
     * obfuscation key rotates hourly.  This prevents the same HMAC key from
     * applying to every connection to a server across its entire lifetime,
     * making long-term traffic correlation via the obfs key infeasible.
     */
    uint64_t epoch = trellis_now_ms() / 3600000ULL;
    return trellis_derive_obfs_key_epoch(server_pk, pk_len, epoch, obfs_key_out);
}
