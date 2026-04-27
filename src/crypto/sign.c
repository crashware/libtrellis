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

trellis_err_t trellis_dual_sign(const uint8_t *ed25519_sk,
                                const uint8_t *ml_dsa_sk,
                                const uint8_t *msg, size_t msg_len,
                                trellis_signature_t *sig)
{
    if (!ed25519_sk || !ml_dsa_sk || !sig)
        return TRELLIS_ERR_INVALID_ARG;
    if (!msg && msg_len > 0)
        return TRELLIS_ERR_INVALID_ARG;

    // Ed25519 detached signature.
    unsigned long long ed_sig_len;
    if (crypto_sign_ed25519_detached(sig->ed25519_sig, &ed_sig_len,
                                     msg, (unsigned long long)msg_len,
                                     ed25519_sk) != 0)
        return TRELLIS_ERR_SIGN_FAILED;

    // ML-DSA-87 signature.
    OQS_SIG *ml_dsa = OQS_SIG_new(OQS_SIG_alg_ml_dsa_87);
    if (!ml_dsa)
        return TRELLIS_ERR_SIGN_FAILED;

    size_t ml_sig_len = 0;
    OQS_STATUS rc = OQS_SIG_sign(ml_dsa, sig->ml_dsa_sig, &ml_sig_len,
                                 msg, msg_len, ml_dsa_sk);
    OQS_SIG_free(ml_dsa);

    return (rc == OQS_SUCCESS) ? TRELLIS_OK : TRELLIS_ERR_SIGN_FAILED;
}

trellis_err_t trellis_dual_verify(const uint8_t *ed25519_pk,
                                  const uint8_t *ml_dsa_pk,
                                  const uint8_t *msg, size_t msg_len,
                                  const trellis_signature_t *sig)
{
    if (!ed25519_pk || !ml_dsa_pk || !sig)
        return TRELLIS_ERR_INVALID_ARG;
    if (!msg && msg_len > 0)
        return TRELLIS_ERR_INVALID_ARG;

    // Both must pass — no short-circuit on first failure for constant-time.
    int ed_ok = crypto_sign_ed25519_verify_detached(
        sig->ed25519_sig, msg, (unsigned long long)msg_len, ed25519_pk);

    OQS_SIG *ml_dsa = OQS_SIG_new(OQS_SIG_alg_ml_dsa_87);
    if (!ml_dsa)
        return TRELLIS_ERR_VERIFY_FAILED;

    OQS_STATUS ml_ok = OQS_SIG_verify(ml_dsa, msg, msg_len,
                                       sig->ml_dsa_sig,
                                       TRELLIS_ML_DSA_87_SIG_LEN,
                                       ml_dsa_pk);
    OQS_SIG_free(ml_dsa);

    if (ed_ok != 0 || ml_ok != OQS_SUCCESS)
        return TRELLIS_ERR_VERIFY_FAILED;

    return TRELLIS_OK;
}
