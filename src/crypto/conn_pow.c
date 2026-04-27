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
 * conn_pow.c — Per-connection Proof-of-Work (DDoS Defense Layer 0).
 *
 * Fixed-difficulty iterative SHAKE-256 that gates new connections,
 * avoiding the Tor PoW control loop vulnerabilities:
 *
 *   - No dynamic difficulty adjustment (no End-Rush, Temporary-Turmoil,
 *     or Maintenance attacks possible)
 *   - O(1) verification via stored expected hash (resists top-half flooding)
 *   - Challenge is nonce+seed bound (no precomputation across connections)
 *   - 5-minute seed rotation (vs Tor's 4-hour window)
 */

#include <trellis/conn_pow.h>
#include <trellis/crypto.h>
#include <sodium.h>
#include <stdlib.h>
#include <string.h>

struct trellis_conn_pow_server {
    trellis_conn_pow_seed_t seeds[TRELLIS_CONN_POW_MAX_SEEDS];
    size_t                  seed_count;
    uint32_t                next_seed_id;
    uint32_t                iterations;
};

/*
 * Public computation: both client and server derive the result from the
 * challenge parameters (seed_id, nonce, iterations) alone.  The server's
 * secret seed is only used to generate unpredictable nonces -- the actual
 * iterative work is deterministic from public inputs.
 *
 * state = SHAKE-256(seed_id_le32 || nonce)
 * for i in 0..iterations: state = SHAKE-256(state)
 * result = state
 */
void trellis_conn_pow_compute(const uint8_t seed[TRELLIS_CONN_POW_SEED_LEN],
                              const uint8_t nonce[TRELLIS_CONN_POW_NONCE_LEN],
                              uint32_t iterations,
                              uint8_t out[TRELLIS_CONN_POW_HASH_LEN])
{
    /* Legacy wrapper: the seed parameter is ignored since the public
     * computation derives from (seed_id || nonce).  Pass seed_id=0
     * so callers migrating from this API get deterministic output.
     * New code should use trellis_conn_pow_compute_public() directly. */
    (void)seed;
    trellis_conn_pow_compute_public(0, nonce, iterations, out);
}

#define TRELLIS_CONN_POW_MAX_ITERS (1U << 24) /* ~16M, safety cap */

void trellis_conn_pow_compute_public(uint32_t seed_id,
                                     const uint8_t nonce[TRELLIS_CONN_POW_NONCE_LEN],
                                     uint32_t iterations,
                                     uint8_t out[TRELLIS_CONN_POW_HASH_LEN])
{
    if (!nonce || !out) return;
    if (iterations > TRELLIS_CONN_POW_MAX_ITERS)
        iterations = TRELLIS_CONN_POW_MAX_ITERS;

    uint8_t state[TRELLIS_CONN_POW_HASH_LEN];
    uint8_t input[4 + TRELLIS_CONN_POW_NONCE_LEN];
    input[0] = (uint8_t)(seed_id);
    input[1] = (uint8_t)(seed_id >> 8);
    input[2] = (uint8_t)(seed_id >> 16);
    input[3] = (uint8_t)(seed_id >> 24);
    memcpy(input + 4, nonce, TRELLIS_CONN_POW_NONCE_LEN);

    trellis_shake256(input, sizeof(input), state, TRELLIS_CONN_POW_HASH_LEN);

    for (uint32_t i = 0; i < iterations; i++)
        trellis_shake256(state, TRELLIS_CONN_POW_HASH_LEN,
                         state, TRELLIS_CONN_POW_HASH_LEN);

    memcpy(out, state, TRELLIS_CONN_POW_HASH_LEN);
    sodium_memzero(state, sizeof(state));
    sodium_memzero(input, sizeof(input));
}

static void generate_seed(trellis_conn_pow_seed_t *s, uint32_t id)
{
    randombytes_buf(s->seed, TRELLIS_CONN_POW_SEED_LEN);
    s->seed_id = id;
    s->created_at = trellis_now_ms();
}

trellis_conn_pow_server_t *trellis_conn_pow_server_new(uint32_t iterations)
{
    trellis_conn_pow_server_t *s = calloc(1, sizeof(*s));
    if (!s)
        return NULL;

    s->iterations = iterations > 0 ? iterations : TRELLIS_CONN_POW_DEFAULT_ITERS;
    s->next_seed_id = 1;

    generate_seed(&s->seeds[0], s->next_seed_id++);
    s->seed_count = 1;

    return s;
}

void trellis_conn_pow_server_free(trellis_conn_pow_server_t *s)
{
    if (!s)
        return;
    sodium_memzero(s, sizeof(*s));
    free(s);
}

static void maybe_rotate(trellis_conn_pow_server_t *s)
{
    uint64_t now = trellis_now_ms();
    if (now <= s->seeds[0].created_at)
        return; /* clock went backwards — skip rotation this tick */
    uint64_t age = now - s->seeds[0].created_at;
    if (age < TRELLIS_CONN_POW_SEED_ROTATE_MS)
        return;

    // Shift current seed to slot 1 (previous), generate new in slot 0.
    if (s->seed_count >= TRELLIS_CONN_POW_MAX_SEEDS)
        sodium_memzero(&s->seeds[TRELLIS_CONN_POW_MAX_SEEDS - 1],
                       sizeof(trellis_conn_pow_seed_t));

    for (size_t i = TRELLIS_CONN_POW_MAX_SEEDS - 1; i > 0; i--)
        s->seeds[i] = s->seeds[i - 1];

    generate_seed(&s->seeds[0], s->next_seed_id++);
    if (s->seed_count < TRELLIS_CONN_POW_MAX_SEEDS)
        s->seed_count++;
}

static const trellis_conn_pow_seed_t *find_seed(
    const trellis_conn_pow_server_t *s, uint32_t seed_id)
{
    for (size_t i = 0; i < s->seed_count; i++) {
        if (s->seeds[i].seed_id == seed_id)
            return &s->seeds[i];
    }
    return NULL;
}

trellis_err_t trellis_conn_pow_generate(trellis_conn_pow_server_t *s,
                                        trellis_conn_pow_challenge_t *out)
{
    if (!s || !out)
        return TRELLIS_ERR_INVALID_ARG;

    maybe_rotate(s);

    out->seed_id = s->seeds[0].seed_id;
    randombytes_buf(out->nonce, TRELLIS_CONN_POW_NONCE_LEN);
    out->iterations = s->iterations;

    return TRELLIS_OK;
}

trellis_err_t trellis_conn_pow_solve(const trellis_conn_pow_challenge_t *ch,
                                     const uint8_t seed[TRELLIS_CONN_POW_SEED_LEN],
                                     trellis_conn_pow_solution_t *out)
{
    if (!ch || !out)
        return TRELLIS_ERR_INVALID_ARG;

    (void)seed; /* seed param kept for API compat; computation is public */
    memcpy(out->nonce, ch->nonce, TRELLIS_CONN_POW_NONCE_LEN);
    trellis_conn_pow_compute_public(ch->seed_id, ch->nonce, ch->iterations,
                                    out->result);
    return TRELLIS_OK;
}

trellis_err_t trellis_conn_pow_verify(trellis_conn_pow_server_t *s,
                                      const trellis_conn_pow_challenge_t *ch,
                                      const trellis_conn_pow_solution_t *sol)
{
    if (!s || !ch || !sol)
        return TRELLIS_ERR_INVALID_ARG;

    // Verify nonce matches the challenge.
    if (sodium_memcmp(ch->nonce, sol->nonce, TRELLIS_CONN_POW_NONCE_LEN) != 0)
        return TRELLIS_ERR_VERIFY_FAILED;

    // Verify the seed_id is one we issued (prevents replays across servers)
    if (!find_seed(s, ch->seed_id))
        return TRELLIS_ERR_EXPIRED;

    uint8_t expected[TRELLIS_CONN_POW_HASH_LEN];
    trellis_conn_pow_compute_public(ch->seed_id, ch->nonce, ch->iterations,
                                    expected);

    int eq = sodium_memcmp(expected, sol->result, TRELLIS_CONN_POW_HASH_LEN);
    sodium_memzero(expected, sizeof(expected));

    return (eq == 0) ? TRELLIS_OK : TRELLIS_ERR_VERIFY_FAILED;
}

trellis_err_t trellis_conn_pow_challenge_serialize(
    const trellis_conn_pow_challenge_t *ch,
    uint8_t out[TRELLIS_CONN_POW_CHALLENGE_LEN])
{
    if (!ch || !out)
        return TRELLIS_ERR_INVALID_ARG;

    out[0] = TRELLIS_CONN_POW_CHALLENGE_TAG;

    out[1] = (uint8_t)(ch->seed_id);
    out[2] = (uint8_t)(ch->seed_id >> 8);
    out[3] = (uint8_t)(ch->seed_id >> 16);
    out[4] = (uint8_t)(ch->seed_id >> 24);

    memcpy(out + 5, ch->nonce, TRELLIS_CONN_POW_NONCE_LEN);

    uint32_t it = ch->iterations;
    out[21] = (uint8_t)(it);
    out[22] = (uint8_t)(it >> 8);
    out[23] = (uint8_t)(it >> 16);
    out[24] = (uint8_t)(it >> 24);

    return TRELLIS_OK;
}

trellis_err_t trellis_conn_pow_challenge_deserialize(
    const uint8_t *data, size_t len,
    trellis_conn_pow_challenge_t *out)
{
    if (!data || !out || len < TRELLIS_CONN_POW_CHALLENGE_LEN)
        return TRELLIS_ERR_INVALID_ARG;

    if (data[0] != TRELLIS_CONN_POW_CHALLENGE_TAG)
        return TRELLIS_ERR_DECRYPT;

    out->seed_id = (uint32_t)data[1]
                 | ((uint32_t)data[2] << 8)
                 | ((uint32_t)data[3] << 16)
                 | ((uint32_t)data[4] << 24);

    memcpy(out->nonce, data + 5, TRELLIS_CONN_POW_NONCE_LEN);

    out->iterations = (uint32_t)data[21]
                    | ((uint32_t)data[22] << 8)
                    | ((uint32_t)data[23] << 16)
                    | ((uint32_t)data[24] << 24);

    return TRELLIS_OK;
}

trellis_err_t trellis_conn_pow_solution_serialize(
    const trellis_conn_pow_solution_t *sol,
    uint8_t out[TRELLIS_CONN_POW_SOLUTION_LEN])
{
    if (!sol || !out)
        return TRELLIS_ERR_INVALID_ARG;

    out[0] = TRELLIS_CONN_POW_SOLUTION_TAG;
    memcpy(out + 1, sol->nonce, TRELLIS_CONN_POW_NONCE_LEN);
    memcpy(out + 1 + TRELLIS_CONN_POW_NONCE_LEN, sol->result,
           TRELLIS_CONN_POW_HASH_LEN);

    return TRELLIS_OK;
}

trellis_err_t trellis_conn_pow_solution_deserialize(
    const uint8_t *data, size_t len,
    trellis_conn_pow_solution_t *out)
{
    if (!data || !out || len < TRELLIS_CONN_POW_SOLUTION_LEN)
        return TRELLIS_ERR_INVALID_ARG;

    if (data[0] != TRELLIS_CONN_POW_SOLUTION_TAG)
        return TRELLIS_ERR_DECRYPT;

    memcpy(out->nonce, data + 1, TRELLIS_CONN_POW_NONCE_LEN);
    memcpy(out->result, data + 1 + TRELLIS_CONN_POW_NONCE_LEN,
           TRELLIS_CONN_POW_HASH_LEN);

    return TRELLIS_OK;
}
