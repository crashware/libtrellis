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
 * identity.c — Composite identity generation and management (Section 5.2)
 *
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

trellis_err_t trellis_identity_fingerprint(const trellis_identity_public_t *pub,
                                           trellis_fingerprint_t *fp)
{
    if (!pub || !fp)
        return TRELLIS_ERR_INVALID_ARG;

    /*
     * Canonical serialization: concat all public keys in order:
     * ed25519 || ml_dsa || x25519 || ml_kem || slh_dsa
     */
    trellis_shake256_state_t st;
    trellis_shake256_inc_init(&st);

    trellis_shake256_inc_absorb(&st, pub->ed25519_pk, TRELLIS_ED25519_PK_LEN);
    trellis_shake256_inc_absorb(&st, pub->ml_dsa_pk,  TRELLIS_ML_DSA_87_PK_LEN);
    trellis_shake256_inc_absorb(&st, pub->x25519_pk,  TRELLIS_X25519_PK_LEN);
    trellis_shake256_inc_absorb(&st, pub->ml_kem_pk,  TRELLIS_ML_KEM_1024_PK_LEN);
    trellis_shake256_inc_absorb(&st, pub->slh_dsa_pk, TRELLIS_SLH_DSA_PK_LEN);

    trellis_shake256_inc_finalize(&st);
    trellis_shake256_inc_squeeze(&st, fp->bytes, TRELLIS_FINGERPRINT_LEN);
    trellis_shake256_inc_ctx_release(&st);

    return TRELLIS_OK;
}

trellis_err_t trellis_identity_generate(trellis_identity_t *id)
{
    if (!id)
        return TRELLIS_ERR_INVALID_ARG;

    memset(id, 0, sizeof(*id));

    // 1. Ed25519 signing keypair.
    if (crypto_sign_ed25519_keypair(id->ed25519_pk, id->ed25519_sk) != 0)
        goto fail_keygen;

    // 2. ML-DSA-87 signing keypair.
    OQS_SIG *ml_dsa = OQS_SIG_new(OQS_SIG_alg_ml_dsa_87);
    if (!ml_dsa)
        goto fail_keygen;

    OQS_STATUS rc = OQS_SIG_keypair(ml_dsa, id->ml_dsa_pk, id->ml_dsa_sk);
    OQS_SIG_free(ml_dsa);
    if (rc != OQS_SUCCESS)
        goto fail_keygen;

    // 3. X25519 KEM keypair.
    randombytes_buf(id->x25519_sk, TRELLIS_X25519_SK_LEN);
    if (crypto_scalarmult_curve25519_base(id->x25519_pk, id->x25519_sk) != 0)
        goto fail_keygen;

    // 4. ML-KEM-1024 keypair.
    OQS_KEM *ml_kem = OQS_KEM_new(OQS_KEM_alg_ml_kem_1024);
    if (!ml_kem)
        goto fail_keygen;

    rc = OQS_KEM_keypair(ml_kem, id->ml_kem_pk, id->ml_kem_sk);
    OQS_KEM_free(ml_kem);
    if (rc != OQS_SUCCESS)
        goto fail_keygen;

    // 5. SLH-DSA-256s backup signing keypair.
    OQS_SIG *slh = OQS_SIG_new(OQS_SIG_alg_sphincs_sha2_256s_simple);
    if (!slh)
        goto fail_keygen;

    rc = OQS_SIG_keypair(slh, id->slh_dsa_pk, id->slh_dsa_sk);
    OQS_SIG_free(slh);
    if (rc != OQS_SUCCESS)
        goto fail_keygen;

    // Compute fingerprint from public keys.
    trellis_identity_public_t pub;
    trellis_identity_public_from(id, &pub);
    trellis_err_t err = trellis_identity_fingerprint(&pub, &id->fingerprint);
    sodium_memzero(&pub, sizeof(pub));

    return err;

fail_keygen:
    sodium_memzero(id, sizeof(*id));
    return TRELLIS_ERR_SIGN_KEYGEN;
}

void trellis_identity_public_from(const trellis_identity_t *id,
                                  trellis_identity_public_t *pub)
{
    if (!id || !pub)
        return;

    memcpy(pub->ed25519_pk, id->ed25519_pk, TRELLIS_ED25519_PK_LEN);
    memcpy(pub->ml_dsa_pk,  id->ml_dsa_pk,  TRELLIS_ML_DSA_87_PK_LEN);
    memcpy(pub->x25519_pk,  id->x25519_pk,  TRELLIS_X25519_PK_LEN);
    memcpy(pub->ml_kem_pk,  id->ml_kem_pk,  TRELLIS_ML_KEM_1024_PK_LEN);
    memcpy(pub->slh_dsa_pk, id->slh_dsa_pk, TRELLIS_SLH_DSA_PK_LEN);
    memcpy(&pub->fingerprint, &id->fingerprint, sizeof(trellis_fingerprint_t));
}

trellis_err_t trellis_identity_sign(const trellis_identity_t *id,
                                    const uint8_t *msg, size_t msg_len,
                                    trellis_signature_t *sig)
{
    if (!id)
        return TRELLIS_ERR_INVALID_ARG;

    return trellis_dual_sign(id->ed25519_sk, id->ml_dsa_sk, msg, msg_len, sig);
}

trellis_err_t trellis_identity_verify(const trellis_identity_public_t *pub,
                                      const uint8_t *msg, size_t msg_len,
                                      const trellis_signature_t *sig)
{
    if (!pub)
        return TRELLIS_ERR_INVALID_ARG;

    return trellis_dual_verify(pub->ed25519_pk, pub->ml_dsa_pk, msg, msg_len, sig);
}

trellis_err_t trellis_identity_backup_sign(const trellis_identity_t *id,
                                           const uint8_t *msg, size_t msg_len,
                                           uint8_t *sig_out, size_t *sig_len)
{
    if (!id || !sig_out || !sig_len)
        return TRELLIS_ERR_INVALID_ARG;
    if (!msg && msg_len > 0)
        return TRELLIS_ERR_INVALID_ARG;

    OQS_SIG *slh = OQS_SIG_new(OQS_SIG_alg_sphincs_sha2_256s_simple);
    if (!slh)
        return TRELLIS_ERR_SIGN_FAILED;

    OQS_STATUS rc = OQS_SIG_sign(slh, sig_out, sig_len,
                                 msg, msg_len, id->slh_dsa_sk);
    OQS_SIG_free(slh);

    return (rc == OQS_SUCCESS) ? TRELLIS_OK : TRELLIS_ERR_SIGN_FAILED;
}

trellis_err_t trellis_identity_backup_verify(const trellis_identity_public_t *pub,
                                             const uint8_t *msg, size_t msg_len,
                                             const uint8_t *sig, size_t sig_len)
{
    if (!pub || !sig)
        return TRELLIS_ERR_INVALID_ARG;
    if (!msg && msg_len > 0)
        return TRELLIS_ERR_INVALID_ARG;

    OQS_SIG *slh = OQS_SIG_new(OQS_SIG_alg_sphincs_sha2_256s_simple);
    if (!slh)
        return TRELLIS_ERR_VERIFY_FAILED;

    OQS_STATUS rc = OQS_SIG_verify(slh, msg, msg_len, sig, sig_len,
                                   pub->slh_dsa_pk);
    OQS_SIG_free(slh);

    return (rc == OQS_SUCCESS) ? TRELLIS_OK : TRELLIS_ERR_VERIFY_FAILED;
}

size_t trellis_identity_public_serialize(const trellis_identity_public_t *pub,
                                         uint8_t *buf, size_t buf_len)
{
    const size_t needed = TRELLIS_IDENTITY_PUBLIC_SERIAL_LEN
                        + TRELLIS_FINGERPRINT_LEN;

    if (!pub)
        return 0;
    if (!buf || buf_len < needed)
        return needed;

    size_t off = 0;
    memcpy(buf + off, pub->ed25519_pk, TRELLIS_ED25519_PK_LEN);
    off += TRELLIS_ED25519_PK_LEN;

    memcpy(buf + off, pub->ml_dsa_pk, TRELLIS_ML_DSA_87_PK_LEN);
    off += TRELLIS_ML_DSA_87_PK_LEN;

    memcpy(buf + off, pub->x25519_pk, TRELLIS_X25519_PK_LEN);
    off += TRELLIS_X25519_PK_LEN;

    memcpy(buf + off, pub->ml_kem_pk, TRELLIS_ML_KEM_1024_PK_LEN);
    off += TRELLIS_ML_KEM_1024_PK_LEN;

    memcpy(buf + off, pub->slh_dsa_pk, TRELLIS_SLH_DSA_PK_LEN);
    off += TRELLIS_SLH_DSA_PK_LEN;

    memcpy(buf + off, pub->fingerprint.bytes, TRELLIS_FINGERPRINT_LEN);
    off += TRELLIS_FINGERPRINT_LEN;

    return off;
}

trellis_err_t trellis_identity_public_deserialize(const uint8_t *buf, size_t len,
                                                  trellis_identity_public_t *pub)
{
    const size_t needed = TRELLIS_IDENTITY_PUBLIC_SERIAL_LEN
                        + TRELLIS_FINGERPRINT_LEN;

    if (!buf || !pub || len < needed)
        return TRELLIS_ERR_INVALID_ARG;

    size_t off = 0;
    memcpy(pub->ed25519_pk, buf + off, TRELLIS_ED25519_PK_LEN);
    off += TRELLIS_ED25519_PK_LEN;

    memcpy(pub->ml_dsa_pk, buf + off, TRELLIS_ML_DSA_87_PK_LEN);
    off += TRELLIS_ML_DSA_87_PK_LEN;

    memcpy(pub->x25519_pk, buf + off, TRELLIS_X25519_PK_LEN);
    off += TRELLIS_X25519_PK_LEN;

    memcpy(pub->ml_kem_pk, buf + off, TRELLIS_ML_KEM_1024_PK_LEN);
    off += TRELLIS_ML_KEM_1024_PK_LEN;

    memcpy(pub->slh_dsa_pk, buf + off, TRELLIS_SLH_DSA_PK_LEN);
    off += TRELLIS_SLH_DSA_PK_LEN;

    memcpy(pub->fingerprint.bytes, buf + off, TRELLIS_FINGERPRINT_LEN);

    // Verify fingerprint matches the public keys.
    trellis_fingerprint_t computed;
    trellis_err_t err = trellis_identity_fingerprint(pub, &computed);
    if (err != TRELLIS_OK)
        return err;

    if (!trellis_fingerprint_eq(&computed, &pub->fingerprint))
        return TRELLIS_ERR_VERIFY_FAILED;

    // Reject all-zero X25519 public key (small-subgroup / low-order point)
    static const uint8_t zero_pk[TRELLIS_X25519_PK_LEN] = {0};
    if (sodium_memcmp(pub->x25519_pk, zero_pk, TRELLIS_X25519_PK_LEN) == 0)
        return TRELLIS_ERR_INVALID_ARG;

    // Reject all-zero Ed25519 public key.
    static const uint8_t zero_ed[TRELLIS_ED25519_PK_LEN] = {0};
    if (sodium_memcmp(pub->ed25519_pk, zero_ed, TRELLIS_ED25519_PK_LEN) == 0)
        return TRELLIS_ERR_INVALID_ARG;

    return TRELLIS_OK;
}

trellis_err_t trellis_identity_kem_keypair(const trellis_identity_t *id,
                                            uint8_t *pk_out, size_t pk_cap,
                                            uint8_t *sk_out, size_t sk_cap)
{
    if (!id || !pk_out || !sk_out)
        return TRELLIS_ERR_INVALID_ARG;

    const size_t pk_needed = TRELLIS_X25519_PK_LEN + TRELLIS_ML_KEM_1024_PK_LEN;
    const size_t sk_needed = TRELLIS_X25519_SK_LEN + TRELLIS_ML_KEM_1024_SK_LEN;

    if (pk_cap < pk_needed || sk_cap < sk_needed)
        return TRELLIS_ERR_INVALID_ARG;

    size_t off = 0;
    memcpy(pk_out + off, id->x25519_pk, TRELLIS_X25519_PK_LEN);
    off += TRELLIS_X25519_PK_LEN;
    memcpy(pk_out + off, id->ml_kem_pk, TRELLIS_ML_KEM_1024_PK_LEN);

    off = 0;
    memcpy(sk_out + off, id->x25519_sk, TRELLIS_X25519_SK_LEN);
    off += TRELLIS_X25519_SK_LEN;
    memcpy(sk_out + off, id->ml_kem_sk, TRELLIS_ML_KEM_1024_SK_LEN);

    return TRELLIS_OK;
}

void trellis_identity_destroy(trellis_identity_t *id)
{
    if (!id)
        return;

    sodium_memzero(id->ed25519_sk, TRELLIS_ED25519_SK_LEN);
    sodium_memzero(id->ml_dsa_sk,  TRELLIS_ML_DSA_87_SK_LEN);
    sodium_memzero(id->x25519_sk,  TRELLIS_X25519_SK_LEN);
    sodium_memzero(id->ml_kem_sk,  TRELLIS_ML_KEM_1024_SK_LEN);
    sodium_memzero(id->slh_dsa_sk, TRELLIS_SLH_DSA_SK_LEN);
    sodium_memzero(id, sizeof(*id));
}
