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

// RFC 4648 Base32 (A-Z, 2-7, no padding)
static const char BASE32_ALPHA[] = "abcdefghijklmnopqrstuvwxyz234567";

static size_t base32_encode(const uint8_t *in, size_t in_len,
                            char *out, size_t out_len)
{
    size_t needed = (in_len * 8 + 4) / 5;
    if (out_len < needed + 1)
        return 0;

    size_t bits = 0;
    uint32_t acc = 0;
    size_t pos = 0;

    for (size_t i = 0; i < in_len; i++) {
        acc = (acc << 8) | in[i];
        bits += 8;
        while (bits >= 5) {
            bits -= 5;
            out[pos++] = BASE32_ALPHA[(acc >> bits) & 0x1F];
        }
    }
    if (bits > 0)
        out[pos++] = BASE32_ALPHA[(acc << (5 - bits)) & 0x1F];

    out[pos] = '\0';
    return pos;
}

static int base32_decode_char(char c)
{
    if (c >= 'a' && c <= 'z')
        return c - 'a';
    if (c >= 'A' && c <= 'Z')
        return c - 'A';
    if (c >= '2' && c <= '7')
        return c - '2' + 26;
    return -1;
}

static size_t base32_decode(const char *in, size_t in_len,
                            uint8_t *out, size_t out_len)
{
    size_t needed = in_len * 5 / 8;
    if (out_len < needed)
        return 0;

    uint32_t acc = 0;
    size_t bits = 0;
    size_t pos = 0;

    for (size_t i = 0; i < in_len; i++) {
        int val = base32_decode_char(in[i]);
        if (val < 0)
            return 0;
        acc = (acc << 5) | (uint32_t)val;
        bits += 5;
        if (bits >= 8) {
            bits -= 8;
            out[pos++] = (uint8_t)((acc >> bits) & 0xFF);
        }
    }
    return pos;
}

trellis_err_t trellis_bloom_addr_from_fingerprint(const trellis_fingerprint_t *fp,
                                                   trellis_bloom_addr_t *addr)
{
    if (!fp || !addr)
        return TRELLIS_ERR_INVALID_ARG;

    memset(addr, 0, sizeof(*addr));
    addr->fingerprint = *fp;
    addr->has_grove = false;

    char b32[64];
    size_t b32_len = base32_encode(fp->bytes, TRELLIS_FINGERPRINT_LEN,
                                   b32, sizeof(b32));
    if (b32_len == 0)
        return TRELLIS_ERR_GREENHOUSE;

    int n = snprintf(addr->uri, sizeof(addr->uri), "%s%s",
                     TRELLIS_BLOOM_ADDR_PREFIX, b32);
    if (n < 0 || (size_t)n >= sizeof(addr->uri))
        return TRELLIS_ERR_GREENHOUSE;

    return TRELLIS_OK;
}

trellis_err_t trellis_bloom_addr_from_fingerprint_grove(
        const trellis_fingerprint_t *fp,
        const trellis_grove_id_t *grove_id,
        trellis_bloom_addr_t *addr)
{
    if (!fp || !grove_id || !addr)
        return TRELLIS_ERR_INVALID_ARG;

    memset(addr, 0, sizeof(*addr));
    addr->fingerprint = *fp;
    addr->has_grove = true;
    memcpy(addr->grove_id_short, grove_id->bytes, 10);

    char fp_b32[64];
    size_t fp_len = base32_encode(fp->bytes, TRELLIS_FINGERPRINT_LEN,
                                  fp_b32, sizeof(fp_b32));
    if (fp_len == 0)
        return TRELLIS_ERR_GREENHOUSE;

    char grove_b32[24];
    size_t grove_len = base32_encode(grove_id->bytes, 10,
                                     grove_b32, sizeof(grove_b32));
    if (grove_len == 0)
        return TRELLIS_ERR_GREENHOUSE;

    int n = snprintf(addr->uri, sizeof(addr->uri), "%s%s@%s",
                     TRELLIS_BLOOM_ADDR_PREFIX, fp_b32, grove_b32);
    if (n < 0 || (size_t)n >= sizeof(addr->uri))
        return TRELLIS_ERR_GREENHOUSE;

    return TRELLIS_OK;
}

trellis_err_t trellis_bloom_addr_parse(const char *uri,
                                        trellis_bloom_addr_t *addr)
{
    if (!uri || !addr)
        return TRELLIS_ERR_INVALID_ARG;

    size_t prefix_len = strlen(TRELLIS_BLOOM_ADDR_PREFIX);
    if (strncmp(uri, TRELLIS_BLOOM_ADDR_PREFIX, prefix_len) != 0)
        return TRELLIS_ERR_GREENHOUSE;

    const char *b32 = uri + prefix_len;
    size_t b32_len = strlen(b32);
    if (b32_len == 0)
        return TRELLIS_ERR_GREENHOUSE;

    memset(addr, 0, sizeof(*addr));

    // Check for @grove-id-short suffix.
    const char *at = strchr(b32, '@');
    size_t fp_b32_len;
    if (at) {
        fp_b32_len = (size_t)(at - b32);
        const char *grove_b32 = at + 1;
        size_t grove_b32_len = strlen(grove_b32);
        if (grove_b32_len == 0 || grove_b32_len > 20)
            return TRELLIS_ERR_GREENHOUSE;

        size_t grove_decoded = base32_decode(grove_b32, grove_b32_len,
                                             addr->grove_id_short, 10);
        if (grove_decoded == 0)
            return TRELLIS_ERR_GREENHOUSE;
        addr->has_grove = true;
    } else {
        fp_b32_len = b32_len;
        addr->has_grove = false;
    }

    size_t decoded = base32_decode(b32, fp_b32_len,
                                   addr->fingerprint.bytes,
                                   TRELLIS_FINGERPRINT_LEN);
    if (decoded != TRELLIS_FINGERPRINT_LEN)
        return TRELLIS_ERR_GREENHOUSE;

    size_t uri_len = strlen(uri);
    if (uri_len >= sizeof(addr->uri))
        return TRELLIS_ERR_GREENHOUSE;
    memcpy(addr->uri, uri, uri_len + 1);

    return TRELLIS_OK;
}

trellis_err_t trellis_bloom_addr_descriptor_key(const trellis_fingerprint_t *fp,
                                                 uint8_t *key_out)
{
    if (!fp || !key_out)
        return TRELLIS_ERR_INVALID_ARG;

    static const uint8_t info[] = "bloom-greenhouse-v1";
    return trellis_hkdf_shake256(
        fp->bytes, TRELLIS_FINGERPRINT_LEN,
        NULL, 0,
        info, sizeof(info) - 1,
        key_out, 32);
}
