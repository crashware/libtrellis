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
#include <msgpack.h>
#include <cbor.h>

/*
 * Metamorphic mutation (Section 7.2).
 *
 * Encoding rotation cycles through MSGPACK, CBOR, PROTOBUF, CUSTOM
 * every MORPH_ROTATION_INTERVAL frames. Frame format rotation selects
 * one of 4 header field orderings.
 *
 * Wire layout produced by morph_mutate_encode:
 *   [1-byte encoding tag][re-encoded payload]
 *
 * The encoding tag lets the decoder know which format was used
 * without relying on frame_count synchronisation.
 */
#define MUTATION_TAG_LEN 1

static size_t encode_varint(uint8_t *buf, uint64_t v)
{
    size_t n = 0;
    while (v > 0x7F) {
        buf[n++] = (uint8_t)(v & 0x7F) | 0x80;
        v >>= 7;
    }
    buf[n++] = (uint8_t)v;
    return n;
}

static size_t decode_varint(const uint8_t *buf, size_t buf_len, uint64_t *v)
{
    *v = 0;
    size_t n = 0;
    unsigned shift = 0;
    while (n < buf_len && n < 10) {
        uint64_t byte = buf[n];
        *v |= (byte & 0x7F) << shift;
        n++;
        if (!(byte & 0x80))
            return n;
        shift += 7;
    }
    return 0;
}

static trellis_err_t encode_protobuf(const uint8_t *in, size_t in_len,
                                     trellis_buf_t *out)
{
    uint8_t field_tag = (1 << 3) | 2; /* field 1, wire type 2 (length-delimited) */
    uint8_t len_buf[10];
    size_t len_n = encode_varint(len_buf, (uint64_t)in_len);

    *out = trellis_buf_alloc(1 + len_n + in_len);
    if (!out->data)
        return TRELLIS_ERR_NOMEM;

    out->data[0] = field_tag;
    memcpy(out->data + 1, len_buf, len_n);
    memcpy(out->data + 1 + len_n, in, in_len);
    out->len = 1 + len_n + in_len;
    return TRELLIS_OK;
}

static trellis_err_t decode_protobuf(const uint8_t *in, size_t in_len,
                                     trellis_buf_t *out)
{
    if (in_len < 2)
        return TRELLIS_ERR_DECODE;

    uint8_t expected_tag = (1 << 3) | 2;
    if (in[0] != expected_tag)
        return TRELLIS_ERR_DECODE;

    uint64_t payload_len = 0;
    size_t len_n = decode_varint(in + 1, in_len - 1, &payload_len);
    if (len_n == 0)
        return TRELLIS_ERR_DECODE;

    if (1 + len_n + (size_t)payload_len > in_len)
        return TRELLIS_ERR_DECODE;

    *out = trellis_buf_from(in + 1 + len_n, (size_t)payload_len);
    if (!out->data && payload_len > 0)
        return TRELLIS_ERR_NOMEM;
    return TRELLIS_OK;
}

static trellis_err_t encode_msgpack(const uint8_t *in, size_t in_len,
                                    trellis_buf_t *out)
{
    msgpack_sbuffer sbuf;
    msgpack_sbuffer_init(&sbuf);

    msgpack_packer pk;
    msgpack_packer_init(&pk, &sbuf, msgpack_sbuffer_write);
    msgpack_pack_bin(&pk, in_len);
    msgpack_pack_bin_body(&pk, in, in_len);

    *out = trellis_buf_alloc(sbuf.size);
    if (!out->data) {
        msgpack_sbuffer_destroy(&sbuf);
        return TRELLIS_ERR_NOMEM;
    }
    memcpy(out->data, sbuf.data, sbuf.size);
    out->len = sbuf.size;
    msgpack_sbuffer_destroy(&sbuf);
    return TRELLIS_OK;
}

static trellis_err_t decode_msgpack(const uint8_t *in, size_t in_len,
                                    trellis_buf_t *out)
{
    msgpack_unpacked result;
    msgpack_unpacked_init(&result);

    size_t off = 0;
    msgpack_unpack_return rc = msgpack_unpack_next(&result, (const char *)in,
                                                    in_len, &off);
    if (rc != MSGPACK_UNPACK_SUCCESS) {
        msgpack_unpacked_destroy(&result);
        return TRELLIS_ERR_DECODE;
    }

    if (result.data.type != MSGPACK_OBJECT_BIN) {
        msgpack_unpacked_destroy(&result);
        return TRELLIS_ERR_DECODE;
    }

    *out = trellis_buf_from((const uint8_t *)result.data.via.bin.ptr,
                            result.data.via.bin.size);
    msgpack_unpacked_destroy(&result);
    if (!out->data && result.data.via.bin.size > 0)
        return TRELLIS_ERR_NOMEM;
    return TRELLIS_OK;
}

static trellis_err_t encode_cbor(const uint8_t *in, size_t in_len,
                                 trellis_buf_t *out)
{
    size_t buf_cap = in_len + 16;
    *out = trellis_buf_alloc(buf_cap);
    if (!out->data)
        return TRELLIS_ERR_NOMEM;

    CborEncoder encoder;
    cbor_encoder_init(&encoder, out->data, buf_cap, 0);
    CborError err = cbor_encode_byte_string(&encoder, in, in_len);
    if (err != CborNoError) {
        trellis_buf_free(out);
        return TRELLIS_ERR_MORPH;
    }

    out->len = cbor_encoder_get_buffer_size(&encoder, out->data);
    return TRELLIS_OK;
}

static trellis_err_t decode_cbor(const uint8_t *in, size_t in_len,
                                 trellis_buf_t *out)
{
    CborParser parser;
    CborValue value;
    CborError err = cbor_parser_init(in, in_len, 0, &parser, &value);
    if (err != CborNoError)
        return TRELLIS_ERR_DECODE;

    if (!cbor_value_is_byte_string(&value))
        return TRELLIS_ERR_DECODE;

    uint8_t *buf = NULL;
    size_t buf_len = 0;
    err = cbor_value_dup_byte_string(&value, &buf, &buf_len, NULL);
    if (err != CborNoError)
        return TRELLIS_ERR_DECODE;

    out->data = buf;
    out->len = buf_len;
    out->cap = buf_len;
    return TRELLIS_OK;
}

/*
 * Wire layout: [4-byte LE length][8-byte counter nonce][ChaCha20(payload)]
 *
 * The 8-byte nonce is a little-endian counter derived from frame_count so
 * each frame uses a distinct keystream.  The 32-byte ChaCha20 key is derived
 * from the morph session key via BLAKE2b, ensuring no two sessions share a
 * keystream.
 */
static trellis_err_t encode_custom(const uint8_t *in, size_t in_len,
                                   trellis_buf_t *out,
                                   const trellis_morph_t *morph)
{
    // 8-byte nonce = little-endian frame_count.
    uint8_t nonce[8];
    uint64_t fc = (uint64_t)morph->frame_count;
    for (int i = 0; i < 8; i++)
        nonce[i] = (uint8_t)(fc >> (i * 8));

    // 32-byte key = BLAKE2b(session_key, "bloom-custom-enc")
    uint8_t key32[32];
    crypto_generichash(key32, sizeof(key32),
                       morph->session_key, morph->session_key_len,
                       (const uint8_t *)"bloom-custom-enc", 16);

    size_t total = 4 + 8 + in_len;
    *out = trellis_buf_alloc(total);
    if (!out->data) { sodium_memzero(key32, 32); return TRELLIS_ERR_NOMEM; }

    uint32_t len32 = (uint32_t)in_len;
    out->data[0] = (uint8_t)((len32 >> 24) & 0xFF);
    out->data[1] = (uint8_t)((len32 >> 16) & 0xFF);
    out->data[2] = (uint8_t)((len32 >> 8) & 0xFF);
    out->data[3] = (uint8_t)(len32 & 0xFF);
    memcpy(out->data + 4, nonce, 8);

    crypto_stream_chacha20_xor(out->data + 12, in, in_len, nonce, key32);
    sodium_memzero(key32, sizeof(key32));

    out->len = total;
    return TRELLIS_OK;
}

static trellis_err_t decode_custom(const uint8_t *in, size_t in_len,
                                   trellis_buf_t *out,
                                   const trellis_morph_t *morph)
{
    if (in_len < 12) /* 4-byte len + 8-byte nonce */
        return TRELLIS_ERR_DECODE;

    uint32_t payload_len = ((uint32_t)in[0] << 24) |
                           ((uint32_t)in[1] << 16) |
                           ((uint32_t)in[2] << 8) |
                           (uint32_t)in[3];

    if (12 + (size_t)payload_len > in_len)
        return TRELLIS_ERR_DECODE;

    const uint8_t *nonce = in + 4;

    uint8_t key32[32];
    crypto_generichash(key32, sizeof(key32),
                       morph->session_key, morph->session_key_len,
                       (const uint8_t *)"bloom-custom-enc", 16);

    *out = trellis_buf_alloc(payload_len);
    if (!out->data && payload_len > 0) {
        sodium_memzero(key32, 32);
        return TRELLIS_ERR_NOMEM;
    }

    if (payload_len > 0)
        crypto_stream_chacha20_xor(out->data, in + 12, payload_len, nonce, key32);
    sodium_memzero(key32, sizeof(key32));

    out->len = payload_len;
    return TRELLIS_OK;
}

trellis_err_t morph_mutate_encode(trellis_morph_t *morph,
                                  const uint8_t *in, size_t in_len,
                                  trellis_buf_t *out)
{
    if (!morph || !in || !out)
        return TRELLIS_ERR_INVALID_ARG;

    trellis_encoding_t enc = morph->ctx.encoding;
    trellis_buf_t encoded = {0};
    trellis_err_t err;

    switch (enc) {
    case TRELLIS_ENC_MSGPACK:
        err = encode_msgpack(in, in_len, &encoded);
        break;
    case TRELLIS_ENC_CBOR:
        err = encode_cbor(in, in_len, &encoded);
        break;
    case TRELLIS_ENC_PROTOBUF:
        err = encode_protobuf(in, in_len, &encoded);
        break;
    case TRELLIS_ENC_CUSTOM:
        err = encode_custom(in, in_len, &encoded, morph);
        break;
    default:
        return TRELLIS_ERR_MORPH;
    }

    if (err != TRELLIS_OK)
        return err;

    *out = trellis_buf_alloc(MUTATION_TAG_LEN + encoded.len);
    if (!out->data) {
        trellis_buf_free(&encoded);
        return TRELLIS_ERR_NOMEM;
    }

    out->data[0] = (uint8_t)enc;
    memcpy(out->data + MUTATION_TAG_LEN, encoded.data, encoded.len);
    out->len = MUTATION_TAG_LEN + encoded.len;

    trellis_buf_free(&encoded);
    return TRELLIS_OK;
}

trellis_err_t morph_mutate_decode(trellis_morph_t *morph,
                                  const uint8_t *in, size_t in_len,
                                  trellis_buf_t *out)
{
    (void)morph;

    if (!in || in_len < MUTATION_TAG_LEN || !out)
        return TRELLIS_ERR_INVALID_ARG;

    trellis_encoding_t enc = (trellis_encoding_t)in[0];
    const uint8_t *payload = in + MUTATION_TAG_LEN;
    size_t payload_len = in_len - MUTATION_TAG_LEN;

    switch (enc) {
    case TRELLIS_ENC_MSGPACK:
        return decode_msgpack(payload, payload_len, out);
    case TRELLIS_ENC_CBOR:
        return decode_cbor(payload, payload_len, out);
    case TRELLIS_ENC_PROTOBUF:
        return decode_protobuf(payload, payload_len, out);
    case TRELLIS_ENC_CUSTOM:
        return decode_custom(payload, payload_len, out, morph);
    }

    return TRELLIS_ERR_DECODE;
}
