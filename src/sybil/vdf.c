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
 * vdf.c — Verifiable Delay Function based on iterative SHAKE-256
 *
 * The VDF binds to an identity fingerprint: the prover computes
 * H^T(fingerprint) where H = SHAKE-256 and T is a configurable
 * iteration count. Checkpoints at evenly spaced intervals enable
 * parallel verification: each segment is verified independently
 * starting from its checkpoint, so a threaded verifier can check
 * all segments concurrently in O(T / TRELLIS_VDF_CHECKPOINTS) time.
 */

#include "internal.h"

static void hash_once(const uint8_t in[TRELLIS_VDF_HASH_LEN],
                      uint8_t out[TRELLIS_VDF_HASH_LEN])
{
    trellis_shake256(in, TRELLIS_VDF_HASH_LEN, out, TRELLIS_VDF_HASH_LEN);
}

/*
 * Verify a single segment independently. Each segment starts from
 * a known state (fingerprint for seg 0, checkpoint[seg-1] otherwise)
 * and hashes forward to the expected endpoint (checkpoint[seg] or
 * final_hash for the tail). Returns true if the segment matches.
 */
static bool verify_segment(const uint8_t start[TRELLIS_VDF_HASH_LEN],
                           uint64_t count,
                           const uint8_t expected[TRELLIS_VDF_HASH_LEN])
{
    uint8_t state[TRELLIS_VDF_HASH_LEN];
    memcpy(state, start, TRELLIS_VDF_HASH_LEN);
    for (uint64_t i = 0; i < count; i++)
        hash_once(state, state);
    bool ok = (sodium_memcmp(state, expected, TRELLIS_VDF_HASH_LEN) == 0);
    sodium_memzero(state, sizeof(state));
    return ok;
}

trellis_err_t trellis_vdf_compute(const trellis_fingerprint_t *fp,
                                  uint64_t iterations,
                                  const trellis_identity_t *signer,
                                  trellis_vdf_proof_t *proof_out)
{
    if (!fp || !signer || !proof_out || iterations == 0)
        return TRELLIS_ERR_INVALID_ARG;

    memset(proof_out, 0, sizeof(*proof_out));
    proof_out->iterations = iterations;
    proof_out->created_at = trellis_now_ms();

    uint64_t cp_interval = iterations / TRELLIS_VDF_CHECKPOINTS;
    if (cp_interval == 0)
        cp_interval = 1;

    uint8_t state[TRELLIS_VDF_HASH_LEN];
    memcpy(state, fp->bytes, TRELLIS_FINGERPRINT_LEN);

    size_t cp_idx = 0;
    for (uint64_t i = 0; i < iterations; i++) {
        hash_once(state, state);

        if (cp_idx < TRELLIS_VDF_CHECKPOINTS &&
            (i + 1) == cp_interval * (cp_idx + 1)) {
            memcpy(proof_out->checkpoints[cp_idx], state,
                   TRELLIS_VDF_HASH_LEN);
            cp_idx++;
        }
    }

    memcpy(proof_out->final_hash, state, TRELLIS_VDF_HASH_LEN);
    sodium_memzero(state, sizeof(state));

    /*
     * Sign (fingerprint || iterations || final_hash) to bind the
     * proof to this specific identity.
     */
    uint8_t msg[TRELLIS_FINGERPRINT_LEN + 8 + TRELLIS_VDF_HASH_LEN];
    memcpy(msg, fp->bytes, TRELLIS_FINGERPRINT_LEN);
    for (int b = 0; b < 8; b++)
        msg[TRELLIS_FINGERPRINT_LEN + b] =
            (uint8_t)(iterations >> (56 - 8 * b));
    memcpy(msg + TRELLIS_FINGERPRINT_LEN + 8,
           proof_out->final_hash, TRELLIS_VDF_HASH_LEN);

    trellis_err_t err = trellis_identity_sign(signer, msg, sizeof(msg),
                                              &proof_out->signature);
    sodium_memzero(msg, sizeof(msg));
    return err;
}

trellis_err_t trellis_vdf_verify(const trellis_fingerprint_t *fp,
                                 const trellis_vdf_proof_t *proof,
                                 const trellis_identity_public_t *pub)
{
    if (!fp || !proof || !pub)
        return TRELLIS_ERR_INVALID_ARG;

    if (proof->iterations == 0)
        return TRELLIS_ERR_VDF_INVALID;

    // Verify the signature over (fingerprint || iterations || final_hash)
    uint8_t msg[TRELLIS_FINGERPRINT_LEN + 8 + TRELLIS_VDF_HASH_LEN];
    memcpy(msg, fp->bytes, TRELLIS_FINGERPRINT_LEN);
    for (int b = 0; b < 8; b++)
        msg[TRELLIS_FINGERPRINT_LEN + b] =
            (uint8_t)(proof->iterations >> (56 - 8 * b));
    memcpy(msg + TRELLIS_FINGERPRINT_LEN + 8,
           proof->final_hash, TRELLIS_VDF_HASH_LEN);

    trellis_err_t err = trellis_identity_verify(pub, msg, sizeof(msg),
                                                &proof->signature);
    if (err != TRELLIS_OK)
        return TRELLIS_ERR_VDF_INVALID;

    /*
     * Verify the chain by checking each checkpoint segment independently.
     * Each segment starts from a known state and hashes forward to the
     * expected checkpoint. Segments are independent and can be verified
     * in parallel by a threaded caller.
     *
     * Segment 0:    fingerprint       -> checkpoint[0]   (cp_interval hashes)
     * Segment i:    checkpoint[i-1]   -> checkpoint[i]   (cp_interval hashes)
     * Tail segment: checkpoint[last]  -> final_hash      (remaining hashes)
     */
    uint64_t cp_interval = proof->iterations / TRELLIS_VDF_CHECKPOINTS;
    if (cp_interval == 0)
        cp_interval = 1;

    for (size_t seg = 0; seg < TRELLIS_VDF_CHECKPOINTS; seg++) {
        const uint8_t *start = (seg == 0)
            ? fp->bytes
            : proof->checkpoints[seg - 1];
        if (!verify_segment(start, cp_interval, proof->checkpoints[seg]))
            return TRELLIS_ERR_VDF_INVALID;
    }

    // Tail: iterations after the last checkpoint -> final_hash.
    uint64_t remaining = proof->iterations -
                         (uint64_t)TRELLIS_VDF_CHECKPOINTS * cp_interval;
    if (remaining > 0) {
        if (!verify_segment(proof->checkpoints[TRELLIS_VDF_CHECKPOINTS - 1],
                            remaining, proof->final_hash))
            return TRELLIS_ERR_VDF_INVALID;
    } else {
        if (sodium_memcmp(proof->checkpoints[TRELLIS_VDF_CHECKPOINTS - 1],
                          proof->final_hash, TRELLIS_VDF_HASH_LEN) != 0)
            return TRELLIS_ERR_VDF_INVALID;
    }

    return TRELLIS_OK;
}

double trellis_vdf_age_score(const trellis_vdf_proof_t *proof)
{
    if (!proof || proof->created_at == 0)
        return 0.0;

    uint64_t now = trellis_now_ms();
    if (now <= proof->created_at)
        return 0.0;

    uint64_t age_ms = now - proof->created_at;

    /*
     * Score rises from 0 toward 1 using: 1 - e^(-age / (7 days))
     * Reaches ~0.63 at 7 days, ~0.86 at 14 days, ~0.95 at 21 days.
     */
    double age_days = (double)age_ms / (24.0 * 3600.0 * 1000.0);
    double score = 1.0 - exp(-age_days / 7.0);
    if (score < 0.0) score = 0.0;
    if (score > 1.0) score = 1.0;
    return score;
}
