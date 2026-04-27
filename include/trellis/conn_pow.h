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

#ifndef TRELLIS_CONN_POW_H
#define TRELLIS_CONN_POW_H

#include "types.h"
#include "error.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * Per-connection Proof-of-Work (DDoS Defense Layer 0).
 *
 * Fixed-difficulty iterative SHAKE-256 challenge that gates new connections.
 * Designed to avoid the Tor PoW control loop exploits (End-Rush,
 * Temporary-Turmoil, Maintenance, Hybrid):
 *
 *   - Fixed difficulty: no feedback loop to game
 *   - Challenge bound to per-connection nonce + rotating server seed
 *   - Server seed rotates every 5 minutes (vs Tor's 4-hour window)
 *   - Verification is O(1) hash comparison, not iterative re-solve
 *   - Separated from post-handshake processing (resists hybrid attack)
 *
 * Protocol:
 *   Server -> Client: CHALLENGE(seed_id, nonce, iterations)
 *   Client -> Server: SOLUTION(nonce, result_hash)
 *   Server verifies:  SHAKE-256^iterations(seed || nonce) == result_hash
 *
 * Wire format:
 *   Challenge: [1 tag][4 seed_id LE][16 nonce][4 iterations LE] = 25 bytes
 *   Solution:  [1 tag][16 nonce][32 result_hash]                = 49 bytes
 */
#define TRELLIS_CONN_POW_SEED_LEN       32
#define TRELLIS_CONN_POW_NONCE_LEN      16
#define TRELLIS_CONN_POW_HASH_LEN       32
#define TRELLIS_CONN_POW_DEFAULT_ITERS  (1U << 16) /* ~65K, targeting ~50ms */
#define TRELLIS_CONN_POW_SEED_ROTATE_MS 300000     /* 5 minutes */
#define TRELLIS_CONN_POW_MAX_SEEDS      2          /* current + previous */

#define TRELLIS_CONN_POW_CHALLENGE_TAG  0xC0
#define TRELLIS_CONN_POW_SOLUTION_TAG   0xC1
#define TRELLIS_CONN_POW_CHALLENGE_LEN  25 /* tag(1) + seed_id(4) + nonce(16) + iters(4) */
#define TRELLIS_CONN_POW_SOLUTION_LEN   49 /* tag(1) + nonce(16) + hash(32) */

typedef struct trellis_conn_pow_seed {
    uint8_t  seed[TRELLIS_CONN_POW_SEED_LEN];
    uint32_t seed_id;
    uint64_t created_at;
} trellis_conn_pow_seed_t;

typedef struct trellis_conn_pow_challenge {
    uint32_t seed_id;
    uint8_t  nonce[TRELLIS_CONN_POW_NONCE_LEN];
    uint32_t iterations;
} trellis_conn_pow_challenge_t;

typedef struct trellis_conn_pow_solution {
    uint8_t nonce[TRELLIS_CONN_POW_NONCE_LEN];
    uint8_t result[TRELLIS_CONN_POW_HASH_LEN];
} trellis_conn_pow_solution_t;

/*
 * Server-side seed manager: maintains current and previous seeds,
 * auto-rotates on TRELLIS_CONN_POW_SEED_ROTATE_MS intervals.
 */
typedef struct trellis_conn_pow_server trellis_conn_pow_server_t;

trellis_conn_pow_server_t *trellis_conn_pow_server_new(uint32_t iterations);
void                       trellis_conn_pow_server_free(trellis_conn_pow_server_t *s);

// Generate a challenge for a new connection. Thread-safe.
trellis_err_t trellis_conn_pow_generate(trellis_conn_pow_server_t *s,
                                        trellis_conn_pow_challenge_t *out);

// Serialize a challenge into wire format.
trellis_err_t trellis_conn_pow_challenge_serialize(
    const trellis_conn_pow_challenge_t *ch,
    uint8_t out[TRELLIS_CONN_POW_CHALLENGE_LEN]);

// Deserialize a challenge from wire format.
trellis_err_t trellis_conn_pow_challenge_deserialize(
    const uint8_t *data, size_t len,
    trellis_conn_pow_challenge_t *out);

// Verify a solution against one of the active seeds. O(1) verification.
trellis_err_t trellis_conn_pow_verify(trellis_conn_pow_server_t *s,
                                      const trellis_conn_pow_challenge_t *ch,
                                      const trellis_conn_pow_solution_t *sol);

/*
 * Client-side: solve a challenge by iterating SHAKE-256.
 * Runs synchronously; takes ~50ms at default difficulty.
 */
trellis_err_t trellis_conn_pow_solve(const trellis_conn_pow_challenge_t *ch,
                                     const uint8_t seed[TRELLIS_CONN_POW_SEED_LEN],
                                     trellis_conn_pow_solution_t *out);

/*
 * Compute the expected hash for a given seed + nonce + iterations.
 * Used internally by both solve (client) and verify (server).
 * Kept for API compatibility; delegates to compute_public.
 */
void trellis_conn_pow_compute(const uint8_t seed[TRELLIS_CONN_POW_SEED_LEN],
                              const uint8_t nonce[TRELLIS_CONN_POW_NONCE_LEN],
                              uint32_t iterations,
                              uint8_t out[TRELLIS_CONN_POW_HASH_LEN]);

/*
 * Public computation from challenge parameters only.
 * Both client and server use this to derive the result deterministically.
 */
void trellis_conn_pow_compute_public(uint32_t seed_id,
                                     const uint8_t nonce[TRELLIS_CONN_POW_NONCE_LEN],
                                     uint32_t iterations,
                                     uint8_t out[TRELLIS_CONN_POW_HASH_LEN]);

// Serialize/deserialize a solution.
trellis_err_t trellis_conn_pow_solution_serialize(
    const trellis_conn_pow_solution_t *sol,
    uint8_t out[TRELLIS_CONN_POW_SOLUTION_LEN]);

trellis_err_t trellis_conn_pow_solution_deserialize(
    const uint8_t *data, size_t len,
    trellis_conn_pow_solution_t *out);

#ifdef __cplusplus
}
#endif

#endif /* TRELLIS_CONN_POW_H */
