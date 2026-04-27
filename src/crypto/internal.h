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

#ifndef TRELLIS_CRYPTO_INTERNAL_H
#define TRELLIS_CRYPTO_INTERNAL_H

#include <trellis/types.h>
#include <trellis/error.h>
#include <trellis/identity.h>
#include <trellis/crypto.h>

#include <oqs/oqs.h>
#include <oqs/sha3.h>
#include <sodium.h>

#include <string.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

#define TRELLIS_HS_INFO_STEP2  "Bloom-Handshake-v1-step2"
#define TRELLIS_HS_INFO_FINAL  "Bloom-Handshake-v1-final"

#define TRELLIS_IDENTITY_PUBLIC_SERIAL_LEN \
    (TRELLIS_ED25519_PK_LEN +              \
     TRELLIS_ML_DSA_87_PK_LEN +           \
     TRELLIS_X25519_PK_LEN +              \
     TRELLIS_ML_KEM_1024_PK_LEN +         \
     TRELLIS_SLH_DSA_PK_LEN)

typedef struct trellis_shake256_state {
    OQS_SHA3_shake256_inc_ctx ctx;
} trellis_shake256_state_t;

static inline void trellis_shake256_inc_init(trellis_shake256_state_t *st)
{
    OQS_SHA3_shake256_inc_init(&st->ctx);
}

static inline void trellis_shake256_inc_absorb(trellis_shake256_state_t *st,
                                               const uint8_t *data, size_t len)
{
    OQS_SHA3_shake256_inc_absorb(&st->ctx, data, len);
}

static inline void trellis_shake256_inc_finalize(trellis_shake256_state_t *st)
{
    OQS_SHA3_shake256_inc_finalize(&st->ctx);
}

static inline void trellis_shake256_inc_squeeze(trellis_shake256_state_t *st,
                                                uint8_t *out, size_t out_len)
{
    OQS_SHA3_shake256_inc_squeeze(out, out_len, &st->ctx);
}

static inline void trellis_shake256_inc_ctx_release(trellis_shake256_state_t *st)
{
    OQS_SHA3_shake256_inc_ctx_release(&st->ctx);
}

/*
 * left_encode(x): encode integer x as byte string with prepended length byte.
 * Returns number of bytes written (max 9).
 */
static inline size_t trellis_left_encode(uint64_t x, uint8_t *buf)
{
    uint8_t tmp[8];
    size_t n = 0;

    if (x == 0) {
        buf[0] = 1;
        buf[1] = 0;
        return 2;
    }

    uint64_t v = x;
    while (v > 0) {
        tmp[n++] = (uint8_t)(v & 0xFF);
        v >>= 8;
    }

    buf[0] = (uint8_t)n;
    for (size_t i = 0; i < n; i++)
        buf[i + 1] = tmp[n - 1 - i];

    return n + 1;
}

/*
 * right_encode(x): encode integer x as byte string with appended length byte.
 * Returns number of bytes written (max 9).
 */
static inline size_t trellis_right_encode(uint64_t x, uint8_t *buf)
{
    uint8_t tmp[8];
    size_t n = 0;

    if (x == 0) {
        buf[0] = 0;
        buf[1] = 1;
        return 2;
    }

    uint64_t v = x;
    while (v > 0) {
        tmp[n++] = (uint8_t)(v & 0xFF);
        v >>= 8;
    }

    for (size_t i = 0; i < n; i++)
        buf[i] = tmp[n - 1 - i];
    buf[n] = (uint8_t)n;

    return n + 1;
}

/*
 * encode_string(S, len): left_encode(len*8) || S
 * Returns number of bytes written.
 */
static inline size_t trellis_encode_string(const uint8_t *s, size_t slen,
                                           uint8_t *buf, size_t buf_cap)
{
    uint8_t len_enc[9];
    size_t le_len = trellis_left_encode(slen * 8, len_enc);

    if (le_len + slen > buf_cap)
        return 0;

    memcpy(buf, len_enc, le_len);
    if (slen > 0)
        memcpy(buf + le_len, s, slen);

    return le_len + slen;
}

// cSHAKE-256 rate in bytes (1088 bits / 8)
#define TRELLIS_CSHAKE256_RATE 136

static inline void trellis_write_u32_be(uint8_t *buf, uint32_t v)
{
    buf[0] = (uint8_t)(v >> 24);
    buf[1] = (uint8_t)(v >> 16);
    buf[2] = (uint8_t)(v >> 8);
    buf[3] = (uint8_t)(v);
}

static inline uint32_t trellis_read_u32_be(const uint8_t *buf)
{
    return (uint32_t)buf[0] << 24
         | (uint32_t)buf[1] << 16
         | (uint32_t)buf[2] << 8
         | (uint32_t)buf[3];
}

// Append a length-prefixed field to a buffer at *offset. Returns 0 on success.
static inline int trellis_buf_append_field(uint8_t *buf, size_t buf_cap,
                                           size_t *offset,
                                           const uint8_t *data, size_t data_len)
{
    if (data_len > buf_cap || *offset > buf_cap - 4 || data_len > buf_cap - 4 - *offset)
        return -1;
    trellis_write_u32_be(buf + *offset, (uint32_t)data_len);
    *offset += 4;
    memcpy(buf + *offset, data, data_len);
    *offset += data_len;
    return 0;
}

// Read a length-prefixed field from buf at *offset. Returns 0 on success.
static inline int trellis_buf_read_field(const uint8_t *buf, size_t buf_len,
                                         size_t *offset,
                                         const uint8_t **data_out,
                                         size_t *data_len_out)
{
    if (*offset > buf_len || buf_len - *offset < 4)
        return -1;
    uint32_t flen = trellis_read_u32_be(buf + *offset);
    *offset += 4;
    if (flen > buf_len - *offset)
        return -1;
    *data_out = buf + *offset;
    *data_len_out = flen;
    *offset += flen;
    return 0;
}

#ifdef __cplusplus
}
#endif

#endif /* TRELLIS_CRYPTO_INTERNAL_H */
