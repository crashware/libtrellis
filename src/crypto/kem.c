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
 * kem.c — Hybrid KEM: X25519 + ML-KEM-1024 (Section 5.1)
 *
 */

#include "internal.h"

// Combine two shared secrets via HKDF-SHAKE-256.
static trellis_err_t combine_secrets(const uint8_t *dh_ss, size_t dh_len,
                                     const uint8_t *kem_ss, size_t kem_len,
                                     uint8_t *out)
{
    uint8_t combined[64];
    if (dh_len + kem_len > sizeof(combined))
        return TRELLIS_ERR_INVALID_ARG;

    memcpy(combined, dh_ss, dh_len);
    memcpy(combined + dh_len, kem_ss, kem_len);

    static const uint8_t info[] = "Bloom-HybridKEM-v1";
    trellis_err_t err = trellis_hkdf_shake256(
        combined, dh_len + kem_len,
        NULL, 0,
        info, sizeof(info) - 1,
        out, TRELLIS_ML_KEM_1024_SS_LEN);

    sodium_memzero(combined, sizeof(combined));
    return err;
}

trellis_err_t trellis_kem_keygen(trellis_kem_keypair_t *kp)
{
    if (!kp)
        return TRELLIS_ERR_INVALID_ARG;

    // X25519: random secret key, derive public key.
    randombytes_buf(kp->x25519_sk, TRELLIS_X25519_SK_LEN);
    if (crypto_scalarmult_curve25519_base(kp->x25519_pk, kp->x25519_sk) != 0) {
        sodium_memzero(kp, sizeof(*kp));
        return TRELLIS_ERR_KEM_KEYGEN;
    }

    // ML-KEM-1024
    OQS_KEM *kem = OQS_KEM_new(OQS_KEM_alg_ml_kem_1024);
    if (!kem) {
        sodium_memzero(kp, sizeof(*kp));
        return TRELLIS_ERR_KEM_KEYGEN;
    }

    OQS_STATUS rc = OQS_KEM_keypair(kem, kp->ml_kem_pk, kp->ml_kem_sk);
    OQS_KEM_free(kem);

    if (rc != OQS_SUCCESS) {
        sodium_memzero(kp, sizeof(*kp));
        return TRELLIS_ERR_KEM_KEYGEN;
    }

    return TRELLIS_OK;
}

trellis_err_t trellis_kem_encaps(const uint8_t *peer_x25519_pk,
                                 const uint8_t *peer_ml_kem_pk,
                                 trellis_kem_encaps_result_t *result)
{
    if (!peer_ml_kem_pk || !result)
        return TRELLIS_ERR_INVALID_ARG;

    uint8_t dh_ss[TRELLIS_X25519_PK_LEN];
    bool have_dh = false;

    if (peer_x25519_pk) {
        uint8_t eph_sk[TRELLIS_X25519_SK_LEN];
        randombytes_buf(eph_sk, sizeof(eph_sk));
        if (crypto_scalarmult_curve25519_base(result->x25519_pk, eph_sk) != 0) {
            sodium_memzero(eph_sk, sizeof(eph_sk));
            return TRELLIS_ERR_KEM_ENCAPS;
        }
        if (crypto_scalarmult_curve25519(dh_ss, eph_sk, peer_x25519_pk) != 0) {
            sodium_memzero(eph_sk, sizeof(eph_sk));
            return TRELLIS_ERR_KEM_ENCAPS;
        }
        sodium_memzero(eph_sk, sizeof(eph_sk));
        have_dh = true;
    } else {
        memset(result->x25519_pk, 0, TRELLIS_X25519_PK_LEN);
    }

    uint8_t kem_ss[TRELLIS_ML_KEM_1024_SS_LEN];
    OQS_KEM *kem = OQS_KEM_new(OQS_KEM_alg_ml_kem_1024);
    if (!kem) {
        sodium_memzero(dh_ss, sizeof(dh_ss));
        return TRELLIS_ERR_KEM_ENCAPS;
    }

    OQS_STATUS rc = OQS_KEM_encaps(kem, result->ml_kem_ct, kem_ss, peer_ml_kem_pk);
    OQS_KEM_free(kem);

    if (rc != OQS_SUCCESS) {
        sodium_memzero(dh_ss, sizeof(dh_ss));
        sodium_memzero(kem_ss, sizeof(kem_ss));
        return TRELLIS_ERR_KEM_ENCAPS;
    }

    trellis_err_t err;
    if (have_dh) {
        err = combine_secrets(dh_ss, sizeof(dh_ss),
                              kem_ss, sizeof(kem_ss),
                              result->shared_secret);
    } else {
        memcpy(result->shared_secret, kem_ss, TRELLIS_ML_KEM_1024_SS_LEN);
        err = TRELLIS_OK;
    }

    sodium_memzero(dh_ss, sizeof(dh_ss));
    sodium_memzero(kem_ss, sizeof(kem_ss));

    return err;
}

trellis_err_t trellis_kem_decaps(const trellis_kem_keypair_t *kp,
                                 const uint8_t *peer_x25519_pk,
                                 const uint8_t *ml_kem_ct,
                                 uint8_t *shared_secret)
{
    if (!kp || !peer_x25519_pk || !ml_kem_ct || !shared_secret)
        return TRELLIS_ERR_INVALID_ARG;

    // X25519 DH.
    uint8_t dh_ss[TRELLIS_X25519_PK_LEN];
    if (crypto_scalarmult_curve25519(dh_ss, kp->x25519_sk, peer_x25519_pk) != 0) {
        sodium_memzero(dh_ss, sizeof(dh_ss));
        return TRELLIS_ERR_KEM_DECAPS;
    }

    // ML-KEM-1024 decaps.
    uint8_t kem_ss[TRELLIS_ML_KEM_1024_SS_LEN];
    OQS_KEM *kem = OQS_KEM_new(OQS_KEM_alg_ml_kem_1024);
    if (!kem) {
        sodium_memzero(dh_ss, sizeof(dh_ss));
        return TRELLIS_ERR_KEM_DECAPS;
    }

    OQS_STATUS rc = OQS_KEM_decaps(kem, kem_ss, ml_kem_ct, kp->ml_kem_sk);
    OQS_KEM_free(kem);

    if (rc != OQS_SUCCESS) {
        sodium_memzero(dh_ss, sizeof(dh_ss));
        sodium_memzero(kem_ss, sizeof(kem_ss));
        return TRELLIS_ERR_KEM_DECAPS;
    }

    trellis_err_t err = combine_secrets(dh_ss, sizeof(dh_ss),
                                        kem_ss, sizeof(kem_ss),
                                        shared_secret);

    sodium_memzero(dh_ss, sizeof(dh_ss));
    sodium_memzero(kem_ss, sizeof(kem_ss));

    return err;
}
