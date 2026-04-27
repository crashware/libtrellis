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
 * STUN message parser/serializer (RFC 5389)
 *
 * Handles the 20-byte STUN header, TLV attribute parsing, MESSAGE-INTEGRITY
 * (HMAC-SHA1), and FINGERPRINT (CRC-32 XOR). Supports both STUN and TURN
 * message types.
 */

#include <trellis/turn.h>
#include <trellis/types.h>
#include <trellis/error.h>

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sodium.h>

static inline uint16_t rd16(const uint8_t *p)
{
    return (uint16_t)((p[0] << 8) | p[1]);
}

static inline uint32_t rd32(const uint8_t *p)
{
    return ((uint32_t)p[0] << 24) | ((uint32_t)p[1] << 16) |
           ((uint32_t)p[2] << 8)  | (uint32_t)p[3];
}

static inline void wr16(uint8_t *p, uint16_t v)
{
    p[0] = (uint8_t)(v >> 8);
    p[1] = (uint8_t)(v);
}

static inline void wr32(uint8_t *p, uint32_t v)
{
    p[0] = (uint8_t)(v >> 24);
    p[1] = (uint8_t)(v >> 16);
    p[2] = (uint8_t)(v >> 8);
    p[3] = (uint8_t)(v);
}

// --- CRC-32 for STUN FINGERPRINT (ISO 3309 / ITU-T V.42) ---
static uint32_t crc32_table[256];
static bool crc32_init_done = false;  /* XXX: not thread-safe, fine for single-threaded relay */

static void crc32_init(void)
{
    if (crc32_init_done) return;
    for (uint32_t i = 0; i < 256; i++) {
        uint32_t c = i;
        for (int j = 0; j < 8; j++)
            c = (c & 1) ? (0xEDB88320u ^ (c >> 1)) : (c >> 1);
        crc32_table[i] = c;
    }
    crc32_init_done = true;
}

static uint32_t crc32_compute(const uint8_t *data, size_t len)
{
    crc32_init();
    uint32_t crc = 0xFFFFFFFFu;
    for (size_t i = 0; i < len; i++)
        crc = crc32_table[(crc ^ data[i]) & 0xFF] ^ (crc >> 8);
    return crc ^ 0xFFFFFFFFu;
}

// --- HMAC-SHA1 (RFC 2104) for MESSAGE-INTEGRITY ---
static void sha1_hash(const uint8_t *data, size_t len, uint8_t out[20]);

void hmac_sha1(const uint8_t *key, size_t key_len,
               const uint8_t *data, size_t data_len,
               uint8_t out[20])
{
    uint8_t k_pad[64];
    uint8_t k_ipad[64], k_opad[64];

    memset(k_pad, 0, 64);
    if (key_len > 64) {
        sha1_hash(key, key_len, k_pad);
    } else {
        memcpy(k_pad, key, key_len);
    }

    for (int i = 0; i < 64; i++) {
        k_ipad[i] = k_pad[i] ^ 0x36;
        k_opad[i] = k_pad[i] ^ 0x5C;
    }

    // Inner: SHA-1(k_ipad || data)
    // SHA-1 implementation (FIPS 180-4)
    uint32_t h0 = 0x67452301, h1 = 0xEFCDAB89, h2 = 0x98BADCFE;
    uint32_t h3 = 0x10325476, h4 = 0xC3D2E1F0;

    // Process in 64-byte blocks.
    size_t total_inner = 64 + data_len;
    size_t padded_inner = ((total_inner + 8) / 64 + 1) * 64;
    uint8_t *inner_buf = calloc(1, padded_inner);
    if (!inner_buf) { memset(out, 0, 20); return; }
    memcpy(inner_buf, k_ipad, 64);
    memcpy(inner_buf + 64, data, data_len);
    inner_buf[total_inner] = 0x80;
    uint64_t bit_len = (uint64_t)total_inner * 8;
    inner_buf[padded_inner - 8] = (uint8_t)(bit_len >> 56);
    inner_buf[padded_inner - 7] = (uint8_t)(bit_len >> 48);
    inner_buf[padded_inner - 6] = (uint8_t)(bit_len >> 40);
    inner_buf[padded_inner - 5] = (uint8_t)(bit_len >> 32);
    inner_buf[padded_inner - 4] = (uint8_t)(bit_len >> 24);
    inner_buf[padded_inner - 3] = (uint8_t)(bit_len >> 16);
    inner_buf[padded_inner - 2] = (uint8_t)(bit_len >> 8);
    inner_buf[padded_inner - 1] = (uint8_t)(bit_len);

    for (size_t blk = 0; blk < padded_inner; blk += 64) {
        uint32_t w[80];
        for (int i = 0; i < 16; i++)
            w[i] = rd32(inner_buf + blk + i * 4);
        for (int i = 16; i < 80; i++) {
            uint32_t t = w[i-3] ^ w[i-8] ^ w[i-14] ^ w[i-16];
            w[i] = (t << 1) | (t >> 31);
        }
        uint32_t a = h0, b = h1, c = h2, d = h3, e = h4;
        for (int i = 0; i < 80; i++) {
            uint32_t f, k;
            if (i < 20)      { f = (b & c) | (~b & d);           k = 0x5A827999; }
            else if (i < 40) { f = b ^ c ^ d;                    k = 0x6ED9EBA1; }
            else if (i < 60) { f = (b & c) | (b & d) | (c & d);  k = 0x8F1BBCDC; }
            else              { f = b ^ c ^ d;                    k = 0xCA62C1D6; }
            uint32_t temp = ((a << 5) | (a >> 27)) + f + e + k + w[i];
            e = d; d = c; c = (b << 30) | (b >> 2); b = a; a = temp;
        }
        h0 += a; h1 += b; h2 += c; h3 += d; h4 += e;
    }
    free(inner_buf);

    uint8_t inner_hash[20];
    wr32(inner_hash,      h0);
    wr32(inner_hash + 4,  h1);
    wr32(inner_hash + 8,  h2);
    wr32(inner_hash + 12, h3);
    wr32(inner_hash + 16, h4);

    // Outer: SHA-1(k_opad || inner_hash)
    h0 = 0x67452301; h1 = 0xEFCDAB89; h2 = 0x98BADCFE;
    h3 = 0x10325476; h4 = 0xC3D2E1F0;
    size_t total_outer = 64 + 20;
    size_t padded_outer = ((total_outer + 8) / 64 + 1) * 64;
    uint8_t *outer_buf = calloc(1, padded_outer);
    if (!outer_buf) { memset(out, 0, 20); return; }
    memcpy(outer_buf, k_opad, 64);
    memcpy(outer_buf + 64, inner_hash, 20);
    outer_buf[total_outer] = 0x80;
    bit_len = (uint64_t)total_outer * 8;
    outer_buf[padded_outer - 8] = (uint8_t)(bit_len >> 56);
    outer_buf[padded_outer - 7] = (uint8_t)(bit_len >> 48);
    outer_buf[padded_outer - 6] = (uint8_t)(bit_len >> 40);
    outer_buf[padded_outer - 5] = (uint8_t)(bit_len >> 32);
    outer_buf[padded_outer - 4] = (uint8_t)(bit_len >> 24);
    outer_buf[padded_outer - 3] = (uint8_t)(bit_len >> 16);
    outer_buf[padded_outer - 2] = (uint8_t)(bit_len >> 8);
    outer_buf[padded_outer - 1] = (uint8_t)(bit_len);

    for (size_t blk = 0; blk < padded_outer; blk += 64) {
        uint32_t w[80];
        for (int i = 0; i < 16; i++)
            w[i] = rd32(outer_buf + blk + i * 4);
        for (int i = 16; i < 80; i++) {
            uint32_t t = w[i-3] ^ w[i-8] ^ w[i-14] ^ w[i-16];
            w[i] = (t << 1) | (t >> 31);
        }
        uint32_t a = h0, b = h1, c = h2, d = h3, e = h4;
        for (int i = 0; i < 80; i++) {
            uint32_t f, k;
            if (i < 20)      { f = (b & c) | (~b & d);           k = 0x5A827999; }
            else if (i < 40) { f = b ^ c ^ d;                    k = 0x6ED9EBA1; }
            else if (i < 60) { f = (b & c) | (b & d) | (c & d);  k = 0x8F1BBCDC; }
            else              { f = b ^ c ^ d;                    k = 0xCA62C1D6; }
            uint32_t temp = ((a << 5) | (a >> 27)) + f + e + k + w[i];
            e = d; d = c; c = (b << 30) | (b >> 2); b = a; a = temp;
        }
        h0 += a; h1 += b; h2 += c; h3 += d; h4 += e;
    }
    free(outer_buf);

    wr32(out,      h0);
    wr32(out + 4,  h1);
    wr32(out + 8,  h2);
    wr32(out + 12, h3);
    wr32(out + 16, h4);
}

static void sha1_hash(const uint8_t *data, size_t len, uint8_t out[20])
{
    uint32_t h0 = 0x67452301, h1 = 0xEFCDAB89, h2 = 0x98BADCFE;
    uint32_t h3 = 0x10325476, h4 = 0xC3D2E1F0;
    size_t padded = ((len + 8) / 64 + 1) * 64;
    uint8_t *buf = calloc(1, padded);
    if (!buf) { memset(out, 0, 20); return; }
    memcpy(buf, data, len);
    buf[len] = 0x80;
    uint64_t bit_len = (uint64_t)len * 8;
    for (int i = 0; i < 8; i++)
        buf[padded - 1 - i] = (uint8_t)(bit_len >> (i * 8));

    for (size_t blk = 0; blk < padded; blk += 64) {
        uint32_t w[80];
        for (int i = 0; i < 16; i++)
            w[i] = rd32(buf + blk + i * 4);
        for (int i = 16; i < 80; i++) {
            uint32_t t = w[i-3] ^ w[i-8] ^ w[i-14] ^ w[i-16];
            w[i] = (t << 1) | (t >> 31);
        }
        uint32_t a = h0, b = h1, c = h2, d = h3, e = h4;
        for (int i = 0; i < 80; i++) {
            uint32_t f, k;
            if (i < 20)      { f = (b & c) | (~b & d);           k = 0x5A827999; }
            else if (i < 40) { f = b ^ c ^ d;                    k = 0x6ED9EBA1; }
            else if (i < 60) { f = (b & c) | (b & d) | (c & d);  k = 0x8F1BBCDC; }
            else              { f = b ^ c ^ d;                    k = 0xCA62C1D6; }
            uint32_t temp = ((a << 5) | (a >> 27)) + f + e + k + w[i];
            e = d; d = c; c = (b << 30) | (b >> 2); b = a; a = temp;
        }
        h0 += a; h1 += b; h2 += c; h3 += d; h4 += e;
    }
    free(buf);
    wr32(out, h0); wr32(out+4, h1); wr32(out+8, h2);
    wr32(out+12, h3); wr32(out+16, h4);
}

// --- MD5 (RFC 1321) for long-term credential key derivation ---
void md5_hash(const uint8_t *data, size_t len, uint8_t out[16])
{
    // Minimal MD5 for STUN long-term credential key = MD5(user:realm:pass)
    static const uint32_t s[] = {
        7,12,17,22, 7,12,17,22, 7,12,17,22, 7,12,17,22,
        5, 9,14,20, 5, 9,14,20, 5, 9,14,20, 5, 9,14,20,
        4,11,16,23, 4,11,16,23, 4,11,16,23, 4,11,16,23,
        6,10,15,21, 6,10,15,21, 6,10,15,21, 6,10,15,21
    };
    static const uint32_t K[] = {
        0xd76aa478,0xe8c7b756,0x242070db,0xc1bdceee,
        0xf57c0faf,0x4787c62a,0xa8304613,0xfd469501,
        0x698098d8,0x8b44f7af,0xffff5bb1,0x895cd7be,
        0x6b901122,0xfd987193,0xa679438e,0x49b40821,
        0xf61e2562,0xc040b340,0x265e5a51,0xe9b6c7aa,
        0xd62f105d,0x02441453,0xd8a1e681,0xe7d3fbc8,
        0x21e1cde6,0xc33707d6,0xf4d50d87,0x455a14ed,
        0xa9e3e905,0xfcefa3f8,0x676f02d9,0x8d2a4c8a,
        0xfffa3942,0x8771f681,0x6d9d6122,0xfde5380c,
        0xa4beea44,0x4bdecfa9,0xf6bb4b60,0xbebfbc70,
        0x289b7ec6,0xeaa127fa,0xd4ef3085,0x04881d05,
        0xd9d4d039,0xe6db99e5,0x1fa27cf8,0xc4ac5665,
        0xf4292244,0x432aff97,0xab9423a7,0xfc93a039,
        0x655b59c3,0x8f0ccc92,0xffeff47d,0x85845dd1,
        0x6fa87e4f,0xfe2ce6e0,0xa3014314,0x4e0811a1,
        0xf7537e82,0xbd3af235,0x2ad7d2bb,0xeb86d391
    };

    uint32_t a0 = 0x67452301, b0 = 0xefcdab89;
    uint32_t c0 = 0x98badcfe, d0 = 0x10325476;

    size_t padded = ((len + 8) / 64 + 1) * 64;
    uint8_t *buf = calloc(1, padded);
    if (!buf) { memset(out, 0, 16); return; }
    memcpy(buf, data, len);
    buf[len] = 0x80;
    uint64_t bit_len = (uint64_t)len * 8;
    memcpy(buf + padded - 8, &bit_len, 8); /* little-endian */

    for (size_t off = 0; off < padded; off += 64) {
        uint32_t M[16];
        for (int i = 0; i < 16; i++)
            memcpy(&M[i], buf + off + i * 4, 4); /* little-endian */

        uint32_t A = a0, B = b0, C = c0, D = d0;
        for (int i = 0; i < 64; i++) {
            uint32_t F, g;
            if (i < 16)      { F = (B & C) | (~B & D);  g = i; }
            else if (i < 32) { F = (D & B) | (~D & C);  g = (5*i+1) % 16; }
            else if (i < 48) { F = B ^ C ^ D;           g = (3*i+5) % 16; }
            else              { F = C ^ (B | ~D);        g = (7*i) % 16; }
            F = F + A + K[i] + M[g];
            A = D; D = C; C = B;
            B = B + ((F << s[i]) | (F >> (32 - s[i])));
        }
        a0 += A; b0 += B; c0 += C; d0 += D;
    }
    free(buf);

    memcpy(out,      &a0, 4);
    memcpy(out + 4,  &b0, 4);
    memcpy(out + 8,  &c0, 4);
    memcpy(out + 12, &d0, 4);
}

// --- XOR address encoding/decoding ---
static void xor_addr_decode(const uint8_t *p, size_t len,
                            const uint8_t txn_id[STUN_TXN_ID_LEN],
                            stun_addr_t *addr)
{
    if (len < 4) return;
    addr->family = p[1];
    uint16_t xport = rd16(p + 2);
    addr->port = xport ^ (uint16_t)(STUN_MAGIC_COOKIE >> 16);

    if (addr->family == 0x01 && len >= 8) {
        uint32_t xip = rd32(p + 4);
        uint32_t ip = xip ^ STUN_MAGIC_COOKIE;
        addr->ip.v4[0] = (uint8_t)(ip >> 24);
        addr->ip.v4[1] = (uint8_t)(ip >> 16);
        addr->ip.v4[2] = (uint8_t)(ip >> 8);
        addr->ip.v4[3] = (uint8_t)(ip);
    } else if (addr->family == 0x02 && len >= 20) {
        uint8_t xor_pad[16];
        wr32(xor_pad, STUN_MAGIC_COOKIE);
        memcpy(xor_pad + 4, txn_id, 12);
        for (int i = 0; i < 16; i++)
            addr->ip.v6[i] = p[4 + i] ^ xor_pad[i];
    }
}

static size_t xor_addr_encode(uint16_t attr_type, const stun_addr_t *addr,
                              const uint8_t txn_id[STUN_TXN_ID_LEN],
                              uint8_t *buf, size_t cap)
{
    size_t val_len = (addr->family == 0x01) ? 8 : 20;
    size_t total = 4 + val_len;
    size_t padded = total + ((4 - (val_len % 4)) % 4);
    if (cap < padded) return 0;

    memset(buf, 0, padded);
    wr16(buf, attr_type);
    wr16(buf + 2, (uint16_t)val_len);
    buf[4] = 0; /* reserved */
    buf[5] = addr->family;

    uint16_t xport = addr->port ^ (uint16_t)(STUN_MAGIC_COOKIE >> 16);
    wr16(buf + 6, xport);

    if (addr->family == 0x01) {
        uint32_t ip = ((uint32_t)addr->ip.v4[0] << 24) |
                      ((uint32_t)addr->ip.v4[1] << 16) |
                      ((uint32_t)addr->ip.v4[2] << 8)  |
                      (uint32_t)addr->ip.v4[3];
        wr32(buf + 8, ip ^ STUN_MAGIC_COOKIE);
    } else {
        uint8_t xor_pad[16];
        wr32(xor_pad, STUN_MAGIC_COOKIE);
        memcpy(xor_pad + 4, txn_id, 12);
        for (int i = 0; i < 16; i++)
            buf[8 + i] = addr->ip.v6[i] ^ xor_pad[i];
    }
    return padded;
}

/* ══════════════════════════════════════════════════════════════════════════
 *  Public API
 * ══════════════════════════════════════════════════════════════════════════ */
bool stun_is_stun(const uint8_t *buf, size_t len)
{
    if (len < STUN_HEADER_LEN) return false;
    // First two bits must be 00 (RFC 5389 §6)
    if ((buf[0] & 0xC0) != 0x00) return false;
    // Magic cookie check.
    return rd32(buf + 4) == STUN_MAGIC_COOKIE;
}

bool stun_is_channel_data(const uint8_t *buf, size_t len)
{
    if (len < TURN_CHANNEL_HEADER_LEN) return false;
    uint16_t ch = rd16(buf);
    return ch >= TURN_CHANNEL_MIN && ch <= TURN_CHANNEL_MAX;
}

trellis_err_t stun_parse(const uint8_t *buf, size_t len, stun_msg_t *out)
{
    if (!buf || !out || len < STUN_HEADER_LEN)
        return TRELLIS_ERR_INVALID_ARG;

    memset(out, 0, sizeof(*out));
    out->raw = buf;
    out->raw_len = len;

    if ((buf[0] & 0xC0) != 0x00)
        return TRELLIS_ERR_MSG_FORMAT;

    out->type = rd16(buf);
    out->length = rd16(buf + 2);
    memcpy(out->txn_id, buf + 8, STUN_TXN_ID_LEN);

    if (rd32(buf + 4) != STUN_MAGIC_COOKIE)
        return TRELLIS_ERR_MSG_FORMAT;

    if (STUN_HEADER_LEN + (size_t)out->length > len)
        return TRELLIS_ERR_MSG_FORMAT;

    // Walk TLV attributes.
    size_t pos = STUN_HEADER_LEN;
    size_t end = STUN_HEADER_LEN + out->length;

    while (pos + 4 <= end) {
        uint16_t atype = rd16(buf + pos);
        uint16_t alen  = rd16(buf + pos + 2);
        size_t padded_len = alen + ((4 - (alen % 4)) % 4);

        if (pos + 4 + alen > end)
            break;

        const uint8_t *aval = buf + pos + 4;

        switch (atype) {
        case STUN_ATTR_USERNAME:
            out->username = aval;
            out->username_len = alen;
            break;
        case STUN_ATTR_REALM:
            out->realm = aval;
            out->realm_len = alen;
            break;
        case STUN_ATTR_NONCE:
            out->nonce = aval;
            out->nonce_len = alen;
            break;
        case STUN_ATTR_MESSAGE_INTEGRITY:
            out->has_integrity = true;
            out->integrity_offset = (uint16_t)pos;
            break;
        case STUN_ATTR_FINGERPRINT:
            out->has_fingerprint = true;
            break;
        case STUN_ATTR_ERROR_CODE:
            if (alen >= 4) {
                out->has_error = true;
                out->error_code = (aval[2] & 0x07) * 100 + aval[3];
            }
            break;
        case TURN_ATTR_LIFETIME:
            if (alen >= 4) {
                out->has_lifetime = true;
                out->lifetime = rd32(aval);
            }
            break;
        case TURN_ATTR_CHANNEL_NUMBER:
            if (alen >= 4) {
                out->has_channel_number = true;
                out->channel_number = rd16(aval);
            }
            break;
        case TURN_ATTR_REQUESTED_TRANSPORT:
            if (alen >= 4) {
                out->has_requested_transport = true;
                out->requested_transport = aval[0];
            }
            break;
        case TURN_ATTR_XOR_PEER_ADDRESS:
            out->has_xor_peer_addr = true;
            xor_addr_decode(aval, alen, out->txn_id, &out->xor_peer_addr);
            break;
        case TURN_ATTR_XOR_RELAYED_ADDRESS:
            out->has_xor_relayed_addr = true;
            xor_addr_decode(aval, alen, out->txn_id, &out->xor_relayed_addr);
            break;
        case STUN_ATTR_XOR_MAPPED_ADDRESS:
            out->has_xor_mapped_addr = true;
            xor_addr_decode(aval, alen, out->txn_id, &out->xor_mapped_addr);
            break;
        case TURN_ATTR_DATA:
            out->data_attr = aval;
            out->data_attr_len = alen;
            break;
        default:
            break;
        }

        pos += 4 + padded_len;
    }

    return TRELLIS_OK;
}

bool stun_verify_integrity(const stun_msg_t *msg, const uint8_t *key, size_t key_len)
{
    if (!msg || !msg->has_integrity || !key)
        return false;

    /* MESSAGE-INTEGRITY is computed over the STUN message up to and
     * including the attribute header of MESSAGE-INTEGRITY, but with
     * the message length adjusted to cover only up to MESSAGE-INTEGRITY. */
    size_t mi_offset = msg->integrity_offset;
    // Adjusted length = offset of MI attr - header + MI attr total (4+20)
    uint16_t adjusted_len = (uint16_t)(mi_offset - STUN_HEADER_LEN + 4 + 20);

    uint8_t header_copy[STUN_HEADER_LEN];
    memcpy(header_copy, msg->raw, STUN_HEADER_LEN);
    wr16(header_copy + 2, adjusted_len);

    // Compute HMAC-SHA1 over: adjusted_header || attributes_before_MI.
    size_t data_len = mi_offset;
    uint8_t *data = malloc(data_len);
    if (!data) return false;
    memcpy(data, header_copy, STUN_HEADER_LEN);
    if (data_len > STUN_HEADER_LEN)
        memcpy(data + STUN_HEADER_LEN, msg->raw + STUN_HEADER_LEN,
               data_len - STUN_HEADER_LEN);

    uint8_t computed[20];
    hmac_sha1(key, key_len, data, data_len, computed);
    free(data);

    return sodium_memcmp(computed, msg->raw + mi_offset + 4, 20) == 0;
}

bool stun_verify_auth_secret(const stun_msg_t *msg,
                             const char *secret, const char *realm)
{
    if (!msg || !msg->username || !secret || !realm)
        return false;

    // Derive password: Base64(HMAC-SHA1(secret, username))
    uint8_t raw_mac[20];
    hmac_sha1((const uint8_t *)secret, strlen(secret),
              msg->username, msg->username_len, raw_mac);

    // Base64 encode (manual, no external dep)
    static const char b64[] =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    char password[32]; /* ceil(20/3)*4 + 1 = 29 */
    size_t pi = 0;
    for (size_t i = 0; i < 20; i += 3) {
        uint32_t n = ((uint32_t)raw_mac[i] << 16);
        if (i + 1 < 20) n |= ((uint32_t)raw_mac[i+1] << 8);
        if (i + 2 < 20) n |= (uint32_t)raw_mac[i+2];
        password[pi++] = b64[(n >> 18) & 0x3F];
        password[pi++] = b64[(n >> 12) & 0x3F];
        password[pi++] = (i + 1 < 20) ? b64[(n >> 6) & 0x3F] : '=';
        password[pi++] = (i + 2 < 20) ? b64[n & 0x3F] : '=';
    }
    password[pi] = '\0';

    // Check timestamp expiry: username = "timestamp:label".
    char username_str[256];
    size_t ulen = msg->username_len;
    if (ulen >= sizeof(username_str)) ulen = sizeof(username_str) - 1;
    memcpy(username_str, msg->username, ulen);
    username_str[ulen] = '\0';

    char *colon = strchr(username_str, ':');
    (void)colon;
    size_t digit_count = 0;
    while (username_str[digit_count] >= '0' && username_str[digit_count] <= '9')
        digit_count++;
    if (digit_count > 0 && digit_count <= 10) {
        uint64_t ts = 0;
        for (size_t i = 0; i < digit_count; i++)
            ts = ts * 10 + (uint64_t)(username_str[i] - '0');
        uint64_t now_s = trellis_now_ms() / 1000;
        if (ts > 0 && ts < now_s)
            return false; /* expired */
    }

    // Compute long-term key: MD5(username:realm:password)
    size_t realm_len = strlen(realm);
    size_t key_input_len = ulen + 1 + realm_len + 1 + pi;
    uint8_t *key_input = malloc(key_input_len);
    if (!key_input) return false;

    memcpy(key_input, username_str, ulen);
    key_input[ulen] = ':';
    memcpy(key_input + ulen + 1, realm, realm_len);
    key_input[ulen + 1 + realm_len] = ':';
    memcpy(key_input + ulen + 1 + realm_len + 1, password, pi);

    uint8_t lt_key[16];
    md5_hash(key_input, key_input_len, lt_key);
    free(key_input);

    return stun_verify_integrity(msg, lt_key, 16);
}

// --- Message builder ---
size_t stun_build_response(uint16_t type, const uint8_t txn_id[STUN_TXN_ID_LEN],
                           const uint8_t *attrs, size_t attrs_len,
                           const uint8_t *key, size_t key_len,
                           uint8_t *buf, size_t buf_cap)
{
    // Total: header + attrs + MI(24) + FP(8)
    size_t mi_len = key ? 24 : 0;
    size_t fp_len = 8;
    size_t total = STUN_HEADER_LEN + attrs_len + mi_len + fp_len;
    if (buf_cap < total) return 0;

    memset(buf, 0, total);

    // Header
    wr16(buf, type);
    wr16(buf + 2, (uint16_t)(attrs_len + mi_len + fp_len));
    wr32(buf + 4, STUN_MAGIC_COOKIE);
    memcpy(buf + 8, txn_id, STUN_TXN_ID_LEN);

    // Attributes
    if (attrs && attrs_len)
        memcpy(buf + STUN_HEADER_LEN, attrs, attrs_len);

    size_t pos = STUN_HEADER_LEN + attrs_len;

    // MESSAGE-INTEGRITY
    if (key) {
        // Adjust length to include MI but not FP.
        wr16(buf + 2, (uint16_t)(attrs_len + mi_len));

        uint8_t hmac_out[20];
        hmac_sha1(key, key_len, buf, pos, hmac_out);

        wr16(buf + pos, STUN_ATTR_MESSAGE_INTEGRITY);
        wr16(buf + pos + 2, 20);
        memcpy(buf + pos + 4, hmac_out, 20);
        pos += 24;

        // Restore full length including FP.
        wr16(buf + 2, (uint16_t)(attrs_len + mi_len + fp_len));
    }

    // FINGERPRINT
    uint32_t crc = crc32_compute(buf, pos) ^ STUN_FINGERPRINT_XOR;
    wr16(buf + pos, STUN_ATTR_FINGERPRINT);
    wr16(buf + pos + 2, 4);
    wr32(buf + pos + 4, crc);
    pos += 8;

    return pos;
}

size_t stun_attr_xor_mapped_address(const stun_addr_t *addr,
                                    const uint8_t txn_id[STUN_TXN_ID_LEN],
                                    uint8_t *buf, size_t cap)
{
    return xor_addr_encode(STUN_ATTR_XOR_MAPPED_ADDRESS, addr, txn_id, buf, cap);
}

size_t stun_attr_xor_relayed_address(const stun_addr_t *addr,
                                     const uint8_t txn_id[STUN_TXN_ID_LEN],
                                     uint8_t *buf, size_t cap)
{
    return xor_addr_encode(TURN_ATTR_XOR_RELAYED_ADDRESS, addr, txn_id, buf, cap);
}

size_t stun_attr_xor_peer_address(const stun_addr_t *addr,
                                  const uint8_t txn_id[STUN_TXN_ID_LEN],
                                  uint8_t *buf, size_t cap)
{
    return xor_addr_encode(TURN_ATTR_XOR_PEER_ADDRESS, addr, txn_id, buf, cap);
}

size_t stun_attr_error_code(uint16_t code, const char *reason,
                            uint8_t *buf, size_t cap)
{
    size_t rlen = reason ? strlen(reason) : 0;
    size_t val_len = 4 + rlen;
    size_t padded = val_len + ((4 - (val_len % 4)) % 4);
    if (cap < 4 + padded) return 0;

    memset(buf, 0, 4 + padded);
    wr16(buf, STUN_ATTR_ERROR_CODE);
    wr16(buf + 2, (uint16_t)val_len);
    buf[6] = (uint8_t)(code / 100);
    buf[7] = (uint8_t)(code % 100);
    if (reason && rlen)
        memcpy(buf + 8, reason, rlen);
    return 4 + padded;
}

size_t stun_attr_lifetime(uint32_t seconds, uint8_t *buf, size_t cap)
{
    if (cap < 8) return 0;
    wr16(buf, TURN_ATTR_LIFETIME);
    wr16(buf + 2, 4);
    wr32(buf + 4, seconds);
    return 8;
}

size_t stun_attr_software(const char *name, uint8_t *buf, size_t cap)
{
    size_t nlen = strlen(name);
    size_t padded = nlen + ((4 - (nlen % 4)) % 4);
    if (cap < 4 + padded) return 0;
    memset(buf, 0, 4 + padded);
    wr16(buf, STUN_ATTR_SOFTWARE);
    wr16(buf + 2, (uint16_t)nlen);
    memcpy(buf + 4, name, nlen);
    return 4 + padded;
}

size_t stun_attr_nonce(const uint8_t *nonce, size_t nonce_len,
                       uint8_t *buf, size_t cap)
{
    size_t padded = nonce_len + ((4 - (nonce_len % 4)) % 4);
    if (cap < 4 + padded) return 0;
    memset(buf, 0, 4 + padded);
    wr16(buf, STUN_ATTR_NONCE);
    wr16(buf + 2, (uint16_t)nonce_len);
    memcpy(buf + 4, nonce, nonce_len);
    return 4 + padded;
}

size_t stun_attr_realm(const char *realm, uint8_t *buf, size_t cap)
{
    size_t rlen = strlen(realm);
    size_t padded = rlen + ((4 - (rlen % 4)) % 4);
    if (cap < 4 + padded) return 0;
    memset(buf, 0, 4 + padded);
    wr16(buf, STUN_ATTR_REALM);
    wr16(buf + 2, (uint16_t)rlen);
    memcpy(buf + 4, realm, rlen);
    return 4 + padded;
}
