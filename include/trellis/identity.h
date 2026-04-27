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

#ifndef TRELLIS_IDENTITY_H
#define TRELLIS_IDENTITY_H

#include "types.h"
#include "error.h"

#ifdef __cplusplus
extern "C" {
#endif

// Key sizes per NIST FIPS 203/204/205 + classical.
#define TRELLIS_ED25519_PK_LEN     32
#define TRELLIS_ED25519_SK_LEN     64
#define TRELLIS_ED25519_SIG_LEN    64

#define TRELLIS_X25519_PK_LEN      32
#define TRELLIS_X25519_SK_LEN      32

#define TRELLIS_ML_KEM_1024_PK_LEN 1568
#define TRELLIS_ML_KEM_1024_SK_LEN 3168
#define TRELLIS_ML_KEM_1024_CT_LEN 1568
#define TRELLIS_ML_KEM_1024_SS_LEN 32

#define TRELLIS_ML_DSA_87_PK_LEN   2592
#define TRELLIS_ML_DSA_87_SK_LEN   4896
#define TRELLIS_ML_DSA_87_SIG_LEN  4627

#define TRELLIS_SLH_DSA_PK_LEN     64
#define TRELLIS_SLH_DSA_SK_LEN     128
#define TRELLIS_SLH_DSA_SIG_LEN    29792

// Composite identity bundle (Section 5.2)
typedef struct trellis_identity {
    // Classical signing.
    uint8_t ed25519_pk[TRELLIS_ED25519_PK_LEN];
    uint8_t ed25519_sk[TRELLIS_ED25519_SK_LEN];

    // PQC signing (ML-DSA-87 / FIPS 204)
    uint8_t ml_dsa_pk[TRELLIS_ML_DSA_87_PK_LEN];
    uint8_t ml_dsa_sk[TRELLIS_ML_DSA_87_SK_LEN];

    // Classical KEM (X25519)
    uint8_t x25519_pk[TRELLIS_X25519_PK_LEN];
    uint8_t x25519_sk[TRELLIS_X25519_SK_LEN];

    // PQC KEM (ML-KEM-1024 / FIPS 203)
    uint8_t ml_kem_pk[TRELLIS_ML_KEM_1024_PK_LEN];
    uint8_t ml_kem_sk[TRELLIS_ML_KEM_1024_SK_LEN];

    // Long-term backup signing (SLH-DSA-256s / FIPS 205)
    uint8_t slh_dsa_pk[TRELLIS_SLH_DSA_PK_LEN];
    uint8_t slh_dsa_sk[TRELLIS_SLH_DSA_SK_LEN];

    // Derived fingerprint: SHAKE-256(canonical_serialize(public_keys))[:32]
    trellis_fingerprint_t fingerprint;
} trellis_identity_t;

// Public-only bundle for wire transfer.
typedef struct trellis_identity_public {
    uint8_t ed25519_pk[TRELLIS_ED25519_PK_LEN];
    uint8_t ml_dsa_pk[TRELLIS_ML_DSA_87_PK_LEN];
    uint8_t x25519_pk[TRELLIS_X25519_PK_LEN];
    uint8_t ml_kem_pk[TRELLIS_ML_KEM_1024_PK_LEN];
    uint8_t slh_dsa_pk[TRELLIS_SLH_DSA_PK_LEN];
    trellis_fingerprint_t fingerprint;
} trellis_identity_public_t;

// Hybrid signature: Ed25519 + ML-DSA-87.
typedef struct trellis_signature {
    uint8_t ed25519_sig[TRELLIS_ED25519_SIG_LEN];
    uint8_t ml_dsa_sig[TRELLIS_ML_DSA_87_SIG_LEN];
} trellis_signature_t;

#define TRELLIS_SIGNATURE_LEN (TRELLIS_ED25519_SIG_LEN + TRELLIS_ML_DSA_87_SIG_LEN)

trellis_err_t trellis_identity_generate(trellis_identity_t *id);
void          trellis_identity_public_from(const trellis_identity_t *id,
                                           trellis_identity_public_t *pub);
trellis_err_t trellis_identity_fingerprint(const trellis_identity_public_t *pub,
                                           trellis_fingerprint_t *fp);

trellis_err_t trellis_identity_sign(const trellis_identity_t *id,
                                    const uint8_t *msg, size_t msg_len,
                                    trellis_signature_t *sig);
trellis_err_t trellis_identity_verify(const trellis_identity_public_t *pub,
                                      const uint8_t *msg, size_t msg_len,
                                      const trellis_signature_t *sig);

trellis_err_t trellis_identity_backup_sign(const trellis_identity_t *id,
                                           const uint8_t *msg, size_t msg_len,
                                           uint8_t *sig_out, size_t *sig_len);
trellis_err_t trellis_identity_backup_verify(const trellis_identity_public_t *pub,
                                             const uint8_t *msg, size_t msg_len,
                                             const uint8_t *sig, size_t sig_len);

size_t trellis_identity_public_serialize(const trellis_identity_public_t *pub,
                                         uint8_t *buf, size_t buf_len);
trellis_err_t trellis_identity_public_deserialize(const uint8_t *buf, size_t len,
                                                  trellis_identity_public_t *pub);

void trellis_identity_destroy(trellis_identity_t *id);

// Extract the KEM keypair (X25519 + ML-KEM-1024) from a full identity.
trellis_err_t trellis_identity_kem_keypair(const trellis_identity_t *id,
                                            uint8_t *pk_out, size_t pk_cap,
                                            uint8_t *sk_out, size_t sk_cap);

// Password-protected encrypted export / import.
trellis_err_t trellis_identity_export(const trellis_identity_t *id,
                                       const char *password,
                                       size_t password_len,
                                       uint8_t **out, size_t *out_len);

trellis_err_t trellis_identity_import(const uint8_t *blob, size_t blob_len,
                                       const char *password,
                                       size_t password_len,
                                       trellis_identity_t *id_out);

// Shamir Secret Sharing: split and reconstruct identity key material.
trellis_err_t trellis_identity_split_secret(const uint8_t *secret,
                                             size_t secret_len,
                                             int k, int n,
                                             const trellis_fingerprint_t *fp,
                                             uint8_t **shares_out,
                                             size_t *share_lens);

trellis_err_t trellis_identity_reconstruct_secret(const uint8_t **shares,
                                                   const size_t *share_lens,
                                                   int k,
                                                   uint8_t *secret_out,
                                                   size_t secret_len);

// SLH-DSA emergency re-keying ceremony.
trellis_err_t trellis_identity_emergency_rekey(const trellis_identity_t *old_id,
                                                const trellis_identity_t *new_id,
                                                uint8_t **out, size_t *out_len);

trellis_err_t trellis_identity_verify_rekey(const uint8_t *record,
                                             size_t record_len,
                                             const uint8_t *slh_dsa_pk,
                                             trellis_fingerprint_t *new_fp_out);

#ifdef __cplusplus
}
#endif

#endif /* TRELLIS_IDENTITY_H */
