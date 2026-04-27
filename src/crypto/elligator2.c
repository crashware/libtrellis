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
 * elligator2.c — Elligator2 encoding for Curve25519 public keys
 *
 * Maps X25519 public keys to uniformly random 32-byte representatives and
 * back, so that ephemeral public keys are indistinguishable from random
 * noise on the wire.
 *
 * Field: GF(p), p = 2^255 - 19
 * Curve: v^2 = u^3 + A*u^2 + u, A = 486662
 * Non-square: n = 2 (since p ≡ 5 mod 8, 2 is a QNR)
 */

#include "internal.h"
#include <sodium.h>

#ifdef __SIZEOF_INT128__
typedef __int128 int128_t;
typedef unsigned __int128 uint128_t;
#else
#error "Elligator2 requires __int128 support"
#endif

typedef int64_t fe[5];

#define LIMB_BITS 51
#define LIMB_MASK ((INT64_C(1) << 51) - 1)

static void fe_zero(fe r) { r[0]=r[1]=r[2]=r[3]=r[4]=0; }
static void fe_one(fe r)  { r[0]=1; r[1]=r[2]=r[3]=r[4]=0; }

static void fe_copy(fe r, const fe a)
{
    for (int i = 0; i < 5; i++) r[i] = a[i];
}

static void fe_from_bytes(fe r, const uint8_t b[32])
{
    uint64_t lo = 0;
    int bits = 0, limb = 0;
    fe_zero(r);
    for (int i = 0; i < 32; i++) {
        lo |= (uint64_t)b[i] << bits;
        bits += 8;
        if (bits >= LIMB_BITS && limb < 5) {
            r[limb++] = (int64_t)(lo & (uint64_t)LIMB_MASK);
            lo >>= LIMB_BITS;
            bits -= LIMB_BITS;
        }
    }
    if (limb < 5)
        r[limb] = (int64_t)(lo & (uint64_t)LIMB_MASK);
    r[4] &= LIMB_MASK;
}

static void fe_carry(fe r)
{
    for (int i = 0; i < 4; i++) {
        r[i+1] += r[i] >> LIMB_BITS;
        r[i] &= LIMB_MASK;
    }
    int64_t carry = r[4] >> LIMB_BITS;
    r[4] &= LIMB_MASK;
    r[0] += carry * 19;
}

static void fe_to_bytes(uint8_t b[32], const fe a)
{
    int64_t t[5];
    fe_copy(t, a);

    for (int pass = 0; pass < 3; pass++) {
        for (int i = 0; i < 4; i++) {
            t[i+1] += t[i] >> LIMB_BITS;
            t[i] &= LIMB_MASK;
        }
        int64_t carry = t[4] >> LIMB_BITS;
        t[4] &= LIMB_MASK;
        t[0] += carry * 19;
    }

    int64_t c[5];
    c[0] = t[0] + 19;
    for (int i = 0; i < 4; i++) {
        c[i+1] = t[i+1] + (c[i] >> LIMB_BITS);
        c[i] &= LIMB_MASK;
    }
    int64_t top = c[4] >> LIMB_BITS;
    c[4] &= LIMB_MASK;

    int64_t mask = -(top & 1);
    for (int i = 0; i < 5; i++)
        t[i] = (c[i] & mask) | (t[i] & ~mask);

    memset(b, 0, 32);
    uint64_t v;

    v = (uint64_t)t[0];
    b[0]  = (uint8_t)(v);       b[1]  = (uint8_t)(v >> 8);
    b[2]  = (uint8_t)(v >> 16); b[3]  = (uint8_t)(v >> 24);
    b[4]  = (uint8_t)(v >> 32); b[5]  = (uint8_t)(v >> 40);
    b[6]  = (uint8_t)(v >> 48);

    v = (uint64_t)t[1];
    b[6] |= (uint8_t)(v << 3);
    b[7]  = (uint8_t)(v >> 5);  b[8]  = (uint8_t)(v >> 13);
    b[9]  = (uint8_t)(v >> 21); b[10] = (uint8_t)(v >> 29);
    b[11] = (uint8_t)(v >> 37); b[12] = (uint8_t)(v >> 45);

    v = (uint64_t)t[2];
    b[12] |= (uint8_t)(v << 6);
    b[13]  = (uint8_t)(v >> 2);  b[14] = (uint8_t)(v >> 10);
    b[15]  = (uint8_t)(v >> 18); b[16] = (uint8_t)(v >> 26);
    b[17]  = (uint8_t)(v >> 34); b[18] = (uint8_t)(v >> 42);
    b[19]  = (uint8_t)(v >> 50);

    v = (uint64_t)t[3];
    b[19] |= (uint8_t)(v << 1);
    b[20]  = (uint8_t)(v >> 7);  b[21] = (uint8_t)(v >> 15);
    b[22]  = (uint8_t)(v >> 23); b[23] = (uint8_t)(v >> 31);
    b[24]  = (uint8_t)(v >> 39); b[25] = (uint8_t)(v >> 47);

    v = (uint64_t)t[4];
    b[25] |= (uint8_t)(v << 4);
    b[26]  = (uint8_t)(v >> 4);  b[27] = (uint8_t)(v >> 12);
    b[28]  = (uint8_t)(v >> 20); b[29] = (uint8_t)(v >> 28);
    b[30]  = (uint8_t)(v >> 36); b[31] = (uint8_t)(v >> 44);
}

static void fe_add(fe r, const fe a, const fe b)
{
    for (int i = 0; i < 5; i++) r[i] = a[i] + b[i];
}

static void fe_sub(fe r, const fe a, const fe b)
{
    static const int64_t bias[5] = {
        INT64_C(0xFFFFFFFFFFFDA), /* 2*(2^51-19) = 2^52 - 38 */
        INT64_C(0xFFFFFFFFFFFFE), /* 2*(2^51-1)  = 2^52 - 2 */
        INT64_C(0xFFFFFFFFFFFFE),
        INT64_C(0xFFFFFFFFFFFFE),
        INT64_C(0xFFFFFFFFFFFFE), /* 2*(2^51-1)  = 2^52 - 2 */
    };
    for (int i = 0; i < 5; i++)
        r[i] = a[i] + bias[i] - b[i];
}

static void fe_mul(fe r, const fe a, const fe b)
{
    int128_t t[5];
    for (int i = 0; i < 5; i++) t[i] = 0;

    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 5; j++) {
            int k = i + j;
            if (k < 5)
                t[k] += (int128_t)a[i] * b[j];
            else
                t[k - 5] += (int128_t)a[i] * b[j] * 19;
        }
    }

    for (int i = 0; i < 4; i++) {
        t[i+1] += t[i] >> LIMB_BITS;
        t[i] &= LIMB_MASK;
    }

    int128_t hi = t[4] >> LIMB_BITS;
    t[4] &= LIMB_MASK;
    t[0] += hi * 19;

    for (int i = 0; i < 4; i++) {
        t[i+1] += t[i] >> LIMB_BITS;
        t[i] &= LIMB_MASK;
    }

    for (int i = 0; i < 5; i++) r[i] = (int64_t)t[i];
}

// XXX: could use libsodium's internal fe_sq if they ever expose it
static void fe_sq(fe r, const fe a) { fe_mul(r, a, a); }

static void fe_neg(fe r, const fe a)
{
    fe z;
    fe_zero(z);
    fe_sub(r, z, a);
    fe_carry(r);
}

static int fe_is_zero(const fe a)
{
    uint8_t b[32];
    fe t;
    fe_copy(t, a);
    fe_carry(t);
    fe_to_bytes(b, t);
    return sodium_is_zero(b, 32);
}

static int fe_eq(const fe a, const fe b)
{
    fe d;
    fe_sub(d, a, b);
    fe_carry(d);
    return fe_is_zero(d);
}

// a^(p-2) mod p via addition chain for 2^255 - 21.
static void fe_inv(fe r, const fe a)
{
    fe t0, t1, t2, t3;

    fe_sq(t0, a);                                        /* a^2 */
    fe_sq(t1, t0); fe_sq(t1, t1);                       /* a^8 */
    fe_mul(t1, t1, a);                                   /* a^9 */
    fe_mul(t0, t0, t1);                                  /* a^11 */
    fe_sq(t2, t0);                                       /* a^22 */
    fe_mul(t1, t1, t2);                                  /* a^(2^5-1) */

    fe_sq(t2, t1);
    for (int i = 0; i < 4; i++) fe_sq(t2, t2);
    fe_mul(t1, t2, t1);                                  /* a^(2^10-1) */

    fe_sq(t2, t1);
    for (int i = 0; i < 9; i++) fe_sq(t2, t2);
    fe_mul(t2, t2, t1);                                  /* a^(2^20-1) */

    fe_sq(t3, t2);
    for (int i = 0; i < 19; i++) fe_sq(t3, t3);
    fe_mul(t2, t3, t2);                                  /* a^(2^40-1) */

    for (int i = 0; i < 10; i++) fe_sq(t2, t2);
    fe_mul(t1, t2, t1);                                  /* a^(2^50-1) */

    fe_sq(t2, t1);
    for (int i = 0; i < 49; i++) fe_sq(t2, t2);
    fe_mul(t2, t2, t1);                                  /* a^(2^100-1) */

    fe_sq(t3, t2);
    for (int i = 0; i < 99; i++) fe_sq(t3, t3);
    fe_mul(t2, t3, t2);                                  /* a^(2^200-1) */

    for (int i = 0; i < 50; i++) fe_sq(t2, t2);
    fe_mul(t1, t2, t1);                                  /* a^(2^250-1) */

    for (int i = 0; i < 5; i++) fe_sq(t1, t1);          /* a^(2^255-32) */
    fe_mul(r, t1, t0);                                   /* a^(2^255-21) */
}

/*
 * Compute a^((p+3)/8) = a^(2^252 - 2) using an efficient addition chain.
 * This is the candidate square root for p ≡ 5 (mod 8).
 */
static void fe_pow_2_252m2(fe r, const fe a)
{
    fe a2, t0, t1, t2, t3;

    fe_sq(a2, a);                                        /* a^2 (saved) */
    fe_copy(t0, a2);
    fe_sq(t1, t0); fe_sq(t1, t1);                       /* a^8 */
    fe_mul(t1, t1, a);                                   /* a^9 */
    fe_mul(t0, t0, t1);                                  /* a^11 */
    fe_sq(t2, t0);                                       /* a^22 */
    fe_mul(t1, t1, t2);                                  /* a^(2^5-1) */

    fe_sq(t2, t1);
    for (int i = 0; i < 4; i++) fe_sq(t2, t2);
    fe_mul(t1, t2, t1);                                  /* a^(2^10-1) */

    fe_sq(t2, t1);
    for (int i = 0; i < 9; i++) fe_sq(t2, t2);
    fe_mul(t2, t2, t1);                                  /* a^(2^20-1) */

    fe_sq(t3, t2);
    for (int i = 0; i < 19; i++) fe_sq(t3, t3);
    fe_mul(t2, t3, t2);                                  /* a^(2^40-1) */

    for (int i = 0; i < 10; i++) fe_sq(t2, t2);
    fe_mul(t1, t2, t1);                                  /* a^(2^50-1) */

    fe_sq(t2, t1);
    for (int i = 0; i < 49; i++) fe_sq(t2, t2);
    fe_mul(t2, t2, t1);                                  /* a^(2^100-1) */

    fe_sq(t3, t2);
    for (int i = 0; i < 99; i++) fe_sq(t3, t3);
    fe_mul(t2, t3, t2);                                  /* a^(2^200-1) */

    for (int i = 0; i < 50; i++) fe_sq(t2, t2);
    fe_mul(t1, t2, t1);                                  /* a^(2^250-1) */

    fe_sq(t1, t1);                                       /* a^(2^251-2) */
    fe_sq(t1, t1);                                       /* a^(2^252-4) */
    fe_mul(r, t1, a2);                                   /* a^(2^252-4+2) = a^(2^252-2) */
}

static const uint8_t sqrtm1_bytes[32] = {
    0xb0, 0xa0, 0x0e, 0x4a, 0x27, 0x1b, 0xee, 0xc4,
    0x78, 0xe4, 0x2f, 0xad, 0x06, 0x18, 0x43, 0x2f,
    0xa7, 0xd7, 0xfb, 0x3d, 0x99, 0x00, 0x4d, 0x2b,
    0x0b, 0xdf, 0xc1, 0x4f, 0x80, 0x24, 0x83, 0x2b
};

/*
 * Try to compute sqrt(a) mod p.
 * Returns 1 on success (r contains the square root), 0 if a is a QNR.
 */
static int fe_sqrt_check(fe r, const fe a)
{
    fe beta, check;
    fe_pow_2_252m2(beta, a);

    // Check beta^2 == a.
    fe_sq(check, beta);
    if (fe_eq(check, a)) {
        fe_copy(r, beta);
        return 1;
    }

    // Check (beta * sqrt(-1))^2 == a
    fe sqrtm1;
    fe_from_bytes(sqrtm1, sqrtm1_bytes);
    fe_mul(beta, beta, sqrtm1);

    fe_sq(check, beta);
    if (fe_eq(check, a)) {
        fe_copy(r, beta);
        return 1;
    }

    return 0;
}

static void fe_set_A(fe r) { fe_zero(r); r[0] = 486662; }
static void fe_set_n(fe r) { fe_zero(r); r[0] = 2; }

trellis_err_t trellis_elligator2_decode(const uint8_t repr[32],
                                        uint8_t pk_out[32])
{
    fe r, A, n;
    fe_from_bytes(r, repr);
    fe_set_A(A);
    fe_set_n(n);

    // w = -A / (1 + n·r²)
    fe r2, nr2, denom, w;
    fe_sq(r2, r);
    fe_mul(nr2, n, r2);

    fe one;
    fe_one(one);
    fe_add(denom, one, nr2);
    fe_carry(denom);

    if (fe_is_zero(denom))
        return TRELLIS_ERR_CRYPTO;

    fe denom_inv, neg_A;
    fe_inv(denom_inv, denom);
    fe_neg(neg_A, A);
    fe_mul(w, neg_A, denom_inv);

    // g(w) = w³ + A·w² + w
    fe w2, w3, Aw2, gw;
    fe_sq(w2, w);
    fe_mul(w3, w2, w);
    fe_mul(Aw2, A, w2);
    fe_add(gw, w3, Aw2);
    fe_add(gw, gw, w);
    fe_carry(gw);

    // Check if g(w) is a QR by trying to compute its sqrt.
    fe dummy;
    int is_qr = fe_sqrt_check(dummy, gw);

    fe x;
    if (is_qr) {
        fe_copy(x, w);
    } else {
        fe_neg(x, w);
        fe_sub(x, x, A);
        fe_carry(x);
    }

    fe_to_bytes(pk_out, x);
    return TRELLIS_OK;
}

trellis_err_t trellis_elligator2_encode(const uint8_t pk[32],
                                        uint8_t repr_out[32])
{
    fe x, A, n;
    fe_from_bytes(x, pk);
    fe_set_A(A);
    fe_set_n(n);

    if (fe_is_zero(x))
        return TRELLIS_ERR_INVALID_ARG;

    fe x_plus_A;
    fe_add(x_plus_A, x, A);
    fe_carry(x_plus_A);
    if (fe_is_zero(x_plus_A))
        return TRELLIS_ERR_INVALID_ARG;

    /*
     * Case 1: x = w, so  r² = -(x + A) / (n·x)
     * If this is a QR, r = √(d) is the representative.
     */
    fe neg_xpA, nx, nx_inv, d;
    fe_neg(neg_xpA, x_plus_A);
    fe_mul(nx, n, x);
    fe_inv(nx_inv, nx);
    fe_mul(d, neg_xpA, nx_inv);

    fe r;
    if (fe_sqrt_check(r, d)) {
        fe_to_bytes(repr_out, r);
        return TRELLIS_OK;
    }

    /*
     * Case 2: x = -w - A, so  r² = -x / (n·(x + A))
     */
    fe neg_x, nxpA, nxpA_inv, d2;
    fe_neg(neg_x, x);
    fe_mul(nxpA, n, x_plus_A);
    fe_inv(nxpA_inv, nxpA);
    fe_mul(d2, neg_x, nxpA_inv);

    if (fe_sqrt_check(r, d2)) {
        fe_to_bytes(repr_out, r);
        return TRELLIS_OK;
    }

    return TRELLIS_ERR_INVALID_ARG;
}

trellis_err_t trellis_elligator2_keygen(uint8_t sk[32], uint8_t pk[32],
                                        uint8_t repr[32])
{
    for (int attempts = 0; attempts < 256; attempts++) {
        randombytes_buf(sk, 32);
        sk[0]  &= 248;
        sk[31] &= 127;
        sk[31] |= 64;

        if (crypto_scalarmult_curve25519_base(pk, sk) != 0)
            continue;

        if (trellis_elligator2_encode(pk, repr) == TRELLIS_OK) {
            uint8_t flip;
            randombytes_buf(&flip, 1);
            repr[31] |= (flip & 0x80);
            return TRELLIS_OK;
        }
    }

    sodium_memzero(sk, 32);
    sodium_memzero(pk, 32);
    return TRELLIS_ERR_HANDSHAKE;
}
