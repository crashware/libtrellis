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
 * kdf.c — SHAKE-256, KMAC-256 (NIST SP 800-185), and HKDF-SHAKE-256
 *
 */

#include "internal.h"

trellis_err_t trellis_shake256(const uint8_t *in, size_t in_len,
                               uint8_t *out, size_t out_len)
{
    if (!out || out_len == 0)
        return TRELLIS_ERR_INVALID_ARG;

    if (!in && in_len > 0)
        return TRELLIS_ERR_INVALID_ARG;

    OQS_SHA3_shake256(out, out_len, in, in_len);
    return TRELLIS_OK;
}

/*
 * cSHAKE-256(X, L, N, S):
 *   If N == "" and S == "": equivalent to SHAKE-256(X, L)
 *   Else: bytepad(encode_string(N) || encode_string(S), 136) || X || right_encode(L)
 *   then absorb into Keccak[512] and squeeze L bits.
 */
static trellis_err_t cshake256(const uint8_t *x, size_t x_len,
                               size_t out_bits,
                               const uint8_t *n, size_t n_len,
                               const uint8_t *s, size_t s_len,
                               uint8_t *out)
{
    if (n_len == 0 && s_len == 0) {
        OQS_SHA3_shake256(out, out_bits / 8, x, x_len);
        return TRELLIS_OK;
    }

    trellis_shake256_state_t st;
    trellis_shake256_inc_init(&st);

    /*
     * bytepad(encode_string(N) || encode_string(S), 136):
     *   left_encode(136) || encode_string(N) || encode_string(S) || zero-pad to multiple of 136
     */
    uint8_t scratch[512];
    size_t pos = 0;

    // left_encode(w=136)
    pos += trellis_left_encode(TRELLIS_CSHAKE256_RATE, scratch + pos);

    // encode_string(N)
    pos += trellis_encode_string(n, n_len, scratch + pos, sizeof(scratch) - pos);

    // encode_string(S)
    pos += trellis_encode_string(s, s_len, scratch + pos, sizeof(scratch) - pos);

    // pad to multiple of rate.
    size_t pad_len = TRELLIS_CSHAKE256_RATE - (pos % TRELLIS_CSHAKE256_RATE);
    if (pad_len < TRELLIS_CSHAKE256_RATE) {
        memset(scratch + pos, 0, pad_len);
        pos += pad_len;
    }

    trellis_shake256_inc_absorb(&st, scratch, pos);

    // absorb X.
    trellis_shake256_inc_absorb(&st, x, x_len);

    // absorb right_encode(L) where L = output length in bits.
    uint8_t re[9];
    size_t re_len = trellis_right_encode(out_bits, re);
    trellis_shake256_inc_absorb(&st, re, re_len);

    trellis_shake256_inc_finalize(&st);
    trellis_shake256_inc_squeeze(&st, out, out_bits / 8);
    trellis_shake256_inc_ctx_release(&st);

    sodium_memzero(scratch, sizeof(scratch));
    return TRELLIS_OK;
}

trellis_err_t trellis_kmac256(const uint8_t *key, size_t key_len,
                              const uint8_t *data, size_t data_len,
                              const uint8_t *custom, size_t custom_len,
                              uint8_t *out, size_t out_len)
{
    if (!key || !out || out_len == 0)
        return TRELLIS_ERR_INVALID_ARG;

    /*
     * KMAC256(K, X, L, S) = cSHAKE256(newX, L, "KMAC", S)
     * where newX = bytepad(encode_string(K), 136) || X || right_encode(L)
     *
     * We build newX on the fly via incremental hashing in cSHAKE256.
     * For simplicity, construct the full input to pass to our cSHAKE256.
     */

    // Build: bytepad(encode_string(K), 136) || X || right_encode(L)
    uint8_t key_header[512];
    size_t hdr_pos = 0;

    // left_encode(136) for bytepad
    hdr_pos += trellis_left_encode(TRELLIS_CSHAKE256_RATE, key_header + hdr_pos);

    // encode_string(K)
    hdr_pos += trellis_encode_string(key, key_len, key_header + hdr_pos,
                                     sizeof(key_header) - hdr_pos);

    // pad to multiple of rate.
    size_t pad_needed = TRELLIS_CSHAKE256_RATE - (hdr_pos % TRELLIS_CSHAKE256_RATE);
    if (pad_needed < TRELLIS_CSHAKE256_RATE) {
        memset(key_header + hdr_pos, 0, pad_needed);
        hdr_pos += pad_needed;
    }

    // right_encode(L) where L = out_len * 8 bits
    uint8_t re[9];
    size_t re_len = trellis_right_encode(out_len * 8, re);

    // Total newX = key_header || data || re.
    size_t total = hdr_pos + data_len + re_len;
    uint8_t *newx = malloc(total);
    if (!newx)
        return TRELLIS_ERR_NOMEM;

    memcpy(newx, key_header, hdr_pos);
    if (data && data_len > 0)
        memcpy(newx + hdr_pos, data, data_len);
    memcpy(newx + hdr_pos + data_len, re, re_len);

    static const uint8_t kmac_name[] = "KMAC";
    trellis_err_t err = cshake256(newx, total, out_len * 8,
                                  kmac_name, 4,
                                  custom, custom_len, out);

    sodium_memzero(newx, total);
    sodium_memzero(key_header, sizeof(key_header));
    free(newx);

    return err;
}

trellis_err_t trellis_hkdf_shake256(const uint8_t *ikm, size_t ikm_len,
                                    const uint8_t *salt, size_t salt_len,
                                    const uint8_t *info, size_t info_len,
                                    uint8_t *okm, size_t okm_len)
{
    if (!ikm || !okm || okm_len == 0)
        return TRELLIS_ERR_INVALID_ARG;
    if (okm_len > 255 * 32)
        return TRELLIS_ERR_INVALID_ARG;

    // Use a default salt of 32 zero bytes if none provided.
    uint8_t default_salt[32];
    if (!salt || salt_len == 0) {
        memset(default_salt, 0, sizeof(default_salt));
        salt = default_salt;
        salt_len = sizeof(default_salt);
    }

    // Extract: PRK = KMAC-256(salt, ikm, "HKDF-Extract")
    uint8_t prk[32];
    static const uint8_t extract_custom[] = "HKDF-Extract";
    trellis_err_t err = trellis_kmac256(salt, salt_len,
                                        ikm, ikm_len,
                                        extract_custom, sizeof(extract_custom) - 1,
                                        prk, sizeof(prk));
    if (err != TRELLIS_OK)
        return err;

    /*
     * Expand: T(i) = KMAC-256(PRK, T(i-1) || info || counter, "HKDF-Expand")
     * Output first okm_len bytes of T(1) || T(2) || ...
     */
    static const uint8_t expand_custom[] = "HKDF-Expand";
    uint8_t t_prev[32];
    memset(t_prev, 0, sizeof(t_prev));

    size_t offset = 0;
    uint8_t counter = 1;
    bool first = true;

    while (offset < okm_len) {
        // Build input: T(i-1) || info || counter.
        size_t input_len = (first ? 0 : 32) + info_len + 1;
        uint8_t *input = malloc(input_len);
        if (!input) {
            sodium_memzero(prk, sizeof(prk));
            sodium_memzero(t_prev, sizeof(t_prev));
            return TRELLIS_ERR_NOMEM;
        }

        size_t ipos = 0;
        if (!first) {
            memcpy(input, t_prev, 32);
            ipos = 32;
        }
        if (info && info_len > 0) {
            memcpy(input + ipos, info, info_len);
            ipos += info_len;
        }
        input[ipos] = counter;

        uint8_t t_cur[32];
        err = trellis_kmac256(prk, sizeof(prk),
                              input, input_len,
                              expand_custom, sizeof(expand_custom) - 1,
                              t_cur, sizeof(t_cur));

        sodium_memzero(input, input_len);
        free(input);

        if (err != TRELLIS_OK) {
            sodium_memzero(prk, sizeof(prk));
            sodium_memzero(t_prev, sizeof(t_prev));
            return err;
        }

        size_t to_copy = okm_len - offset;
        if (to_copy > 32)
            to_copy = 32;
        memcpy(okm + offset, t_cur, to_copy);

        memcpy(t_prev, t_cur, 32);
        sodium_memzero(t_cur, sizeof(t_cur));

        offset += to_copy;
        counter++;
        first = false;
    }

    sodium_memzero(prk, sizeof(prk));
    sodium_memzero(t_prev, sizeof(t_prev));

    return TRELLIS_OK;
}
