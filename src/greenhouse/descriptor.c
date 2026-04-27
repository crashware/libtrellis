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

/*
 * Descriptor binary layout:
 *   [protocol_version:1] [created_at:8] [expires_at:8]
 *   [service_identity: serialized public identity]
 *   [intro_point_count:2]
 *   For each intro point:
 *     [node_fingerprint:32] [onion_key:ML_KEM_PK_LEN] [hint_count:1]
 *     For each hint:
 *       [hint_len:2] [hint_data:...]
 *   [signature: TRELLIS_SIGNATURE_LEN]
 */
static void write_u16_be(uint8_t *buf, uint16_t val)
{
    buf[0] = (uint8_t)(val >> 8);
    buf[1] = (uint8_t)(val);
}

static void write_u64_be(uint8_t *buf, uint64_t val)
{
    for (int i = 7; i >= 0; i--) {
        buf[i] = (uint8_t)(val & 0xFF);
        val >>= 8;
    }
}

static uint16_t read_u16_be(const uint8_t *buf)
{
    return (uint16_t)((uint16_t)buf[0] << 8 | (uint16_t)buf[1]);
}

static uint64_t read_u64_be(const uint8_t *buf)
{
    uint64_t v = 0;
    for (int i = 0; i < 8; i++)
        v = (v << 8) | buf[i];
    return v;
}

static size_t descriptor_body_size(const trellis_service_descriptor_t *desc)
{
    size_t id_ser_len = trellis_identity_public_serialize(&desc->service_identity,
                                                          NULL, 0);
    size_t sz = 1 + 8 + 8 + id_ser_len + 2;

    for (size_t i = 0; i < desc->intro_point_count; i++) {
        sz += TRELLIS_FINGERPRINT_LEN + TRELLIS_ML_KEM_1024_PK_LEN + 1;
        size_t hc = desc->intro_points[i].hint_count;
        if (hc > 8) hc = 8;
        for (size_t h = 0; h < hc; h++)
            sz += 2 + (desc->intro_points[i].address_hints[h]
                       ? strlen(desc->intro_points[i].address_hints[h]) : 0);
    }

    // Rhizome grove association: 1 byte flag + optional 32 byte grove_id.
    sz += 1;
    if (desc->has_grove)
        sz += 32;

    return sz;
}

static trellis_err_t serialize_body(const trellis_service_descriptor_t *desc,
                                    uint8_t *buf, size_t buf_len, size_t *written)
{
    uint8_t *p = buf;

    *p++ = desc->protocol_version;
    write_u64_be(p, desc->created_at); p += 8;
    write_u64_be(p, desc->expires_at); p += 8;

    size_t id_len = trellis_identity_public_serialize(&desc->service_identity,
                                                       p, buf_len - (size_t)(p - buf));
    p += id_len;

    write_u16_be(p, (uint16_t)desc->intro_point_count);
    p += 2;

    for (size_t i = 0; i < desc->intro_point_count; i++) {
        const trellis_intro_point_t *ip = &desc->intro_points[i];
        memcpy(p, ip->node_fingerprint.bytes, TRELLIS_FINGERPRINT_LEN);
        p += TRELLIS_FINGERPRINT_LEN;
        memcpy(p, ip->onion_key, TRELLIS_ML_KEM_1024_PK_LEN);
        p += TRELLIS_ML_KEM_1024_PK_LEN;
        size_t hc = ip->hint_count > 8 ? 8 : ip->hint_count;
        *p++ = (uint8_t)hc;
        for (size_t h = 0; h < hc; h++) {
            size_t hlen = ip->address_hints[h] ? strlen(ip->address_hints[h]) : 0;
            write_u16_be(p, (uint16_t)hlen);
            p += 2;
            if (hlen > 0) {
                memcpy(p, ip->address_hints[h], hlen);
                p += hlen;
            }
        }
    }

    // Rhizome grove association.
    *p++ = desc->has_grove ? 1 : 0;
    if (desc->has_grove) {
        memcpy(p, desc->grove_id.bytes, 32);
        p += 32;
    }

    *written = (size_t)(p - buf);
    return TRELLIS_OK;
}

trellis_err_t trellis_descriptor_sign(trellis_service_descriptor_t *desc,
                                       const trellis_identity_t *id)
{
    if (!desc || !id)
        return TRELLIS_ERR_INVALID_ARG;

    size_t body_sz = descriptor_body_size(desc);
    uint8_t *buf = malloc(body_sz);
    if (!buf)
        return TRELLIS_ERR_NOMEM;

    size_t written;
    trellis_err_t err = serialize_body(desc, buf, body_sz, &written);
    if (err != TRELLIS_OK) {
        free(buf);
        return err;
    }

    err = trellis_identity_sign(id, buf, written, &desc->signature);
    free(buf);
    return err;
}

trellis_err_t trellis_descriptor_verify(const trellis_service_descriptor_t *desc)
{
    if (!desc)
        return TRELLIS_ERR_INVALID_ARG;

    // Reject expired descriptors before doing any cryptographic work.
    if (trellis_descriptor_is_expired(desc))
        return TRELLIS_ERR_EXPIRED;

    size_t body_sz = descriptor_body_size(desc);
    uint8_t *buf = malloc(body_sz);
    if (!buf)
        return TRELLIS_ERR_NOMEM;

    size_t written;
    trellis_err_t err = serialize_body(desc, buf, body_sz, &written);
    if (err != TRELLIS_OK) {
        free(buf);
        return err;
    }

    err = trellis_identity_verify(&desc->service_identity, buf, written,
                                  &desc->signature);
    free(buf);
    return err;
}

trellis_err_t trellis_descriptor_serialize(const trellis_service_descriptor_t *desc,
                                            trellis_buf_t *out)
{
    if (!desc || !out)
        return TRELLIS_ERR_INVALID_ARG;

    size_t body_sz = descriptor_body_size(desc);
    size_t total = body_sz + TRELLIS_SIGNATURE_LEN;

    *out = trellis_buf_alloc(total);
    if (!out->data)
        return TRELLIS_ERR_NOMEM;

    size_t written;
    trellis_err_t err = serialize_body(desc, out->data, body_sz, &written);
    if (err != TRELLIS_OK) {
        trellis_buf_free(out);
        return err;
    }

    uint8_t *sig_p = out->data + written;
    memcpy(sig_p, desc->signature.ed25519_sig, TRELLIS_ED25519_SIG_LEN);
    memcpy(sig_p + TRELLIS_ED25519_SIG_LEN,
           desc->signature.ml_dsa_sig, TRELLIS_ML_DSA_87_SIG_LEN);

    out->len = written + TRELLIS_SIGNATURE_LEN;
    return TRELLIS_OK;
}

trellis_err_t trellis_descriptor_deserialize(const uint8_t *data, size_t len,
                                              trellis_service_descriptor_t *desc)
{
    if (!data || !desc)
        return TRELLIS_ERR_INVALID_ARG;

    if (len < 1 + 8 + 8 + 2 + TRELLIS_SIGNATURE_LEN)
        return TRELLIS_ERR_DESCRIPTOR;

    memset(desc, 0, sizeof(*desc));
    const uint8_t *p = data;

    desc->protocol_version = *p++;
    desc->created_at = read_u64_be(p); p += 8;
    desc->expires_at = read_u64_be(p); p += 8;

    size_t remaining = len - (size_t)(p - data);
    trellis_err_t err = trellis_identity_public_deserialize(p, remaining,
                                                            &desc->service_identity);
    if (err != TRELLIS_OK)
        return err;

    size_t id_len = trellis_identity_public_serialize(&desc->service_identity, NULL, 0);
    p += id_len;

    uint16_t ipc = read_u16_be(p); p += 2;
    if (ipc > 32)
        return TRELLIS_ERR_DESCRIPTOR;
    if (ipc > 0) {
        desc->intro_points = calloc(ipc, sizeof(*desc->intro_points));
        if (!desc->intro_points)
            return TRELLIS_ERR_NOMEM;
    }
    desc->intro_point_count = ipc;

    const uint8_t *end = data + len;

    for (size_t i = 0; i < ipc; i++) {
        trellis_intro_point_t *ip = &desc->intro_points[i];

        size_t ip_fixed = TRELLIS_FINGERPRINT_LEN + TRELLIS_ML_KEM_1024_PK_LEN + 1;
        if ((size_t)(end - p) < ip_fixed) {
            trellis_descriptor_free(desc);
            return TRELLIS_ERR_DESCRIPTOR;
        }

        memcpy(ip->node_fingerprint.bytes, p, TRELLIS_FINGERPRINT_LEN);
        p += TRELLIS_FINGERPRINT_LEN;
        memcpy(ip->onion_key, p, TRELLIS_ML_KEM_1024_PK_LEN);
        p += TRELLIS_ML_KEM_1024_PK_LEN;
        uint8_t wire_hint_count = *p++;

        // Parse all wire hints but only store up to 8.
        for (size_t h = 0; h < wire_hint_count; h++) {
            if ((size_t)(end - p) < 2) {
                trellis_descriptor_free(desc);
                return TRELLIS_ERR_DESCRIPTOR;
            }
            uint16_t hlen = read_u16_be(p); p += 2;
            if ((size_t)(end - p) < hlen) {
                trellis_descriptor_free(desc);
                return TRELLIS_ERR_DESCRIPTOR;
            }
            if (h < 8 && hlen > 0) {
                ip->address_hints[h] = malloc(hlen + 1);
                if (!ip->address_hints[h]) {
                    trellis_descriptor_free(desc);
                    return TRELLIS_ERR_NOMEM;
                }
                memcpy(ip->address_hints[h], p, hlen);
                ip->address_hints[h][hlen] = '\0';
            }
            p += hlen;
        }
        ip->hint_count = wire_hint_count > 8 ? 8 : wire_hint_count;
    }

    // Rhizome grove association (optional, for v2+ descriptors)
    if ((size_t)(end - p) >= 1 + TRELLIS_ED25519_SIG_LEN + TRELLIS_ML_DSA_87_SIG_LEN) {
        desc->has_grove = (*p++ != 0);
        if (desc->has_grove) {
            if ((size_t)(end - p) < 32 + TRELLIS_ED25519_SIG_LEN + TRELLIS_ML_DSA_87_SIG_LEN) {
                trellis_descriptor_free(desc);
                return TRELLIS_ERR_DESCRIPTOR;
            }
            memcpy(desc->grove_id.bytes, p, 32);
            p += 32;
        }
    }

    if ((size_t)(end - p) < TRELLIS_ED25519_SIG_LEN + TRELLIS_ML_DSA_87_SIG_LEN) {
        trellis_descriptor_free(desc);
        return TRELLIS_ERR_DESCRIPTOR;
    }
    memcpy(desc->signature.ed25519_sig, p, TRELLIS_ED25519_SIG_LEN);
    p += TRELLIS_ED25519_SIG_LEN;
    memcpy(desc->signature.ml_dsa_sig, p, TRELLIS_ML_DSA_87_SIG_LEN);

    return TRELLIS_OK;
}

bool trellis_descriptor_is_expired(const trellis_service_descriptor_t *desc)
{
    if (!desc)
        return true;
    return trellis_now_ms() > desc->expires_at;
}

void trellis_descriptor_free(trellis_service_descriptor_t *desc)
{
    if (!desc)
        return;

    if (desc->intro_points) {
        for (size_t i = 0; i < desc->intro_point_count; i++) {
            size_t hc = desc->intro_points[i].hint_count;
            if (hc > 8) hc = 8;
            for (size_t h = 0; h < hc; h++)
                free(desc->intro_points[i].address_hints[h]);
        }
        free(desc->intro_points);
        desc->intro_points = NULL;
    }
    desc->intro_point_count = 0;
}

size_t trellis_descriptor_intro_point_count(
    const trellis_service_descriptor_t *desc)
{
    return desc ? desc->intro_point_count : 0;
}

trellis_err_t trellis_descriptor_get_intro_point(
    const trellis_service_descriptor_t *desc,
    size_t index,
    trellis_fingerprint_t *fp_out,
    uint8_t *onion_key_out)
{
    if (!desc || index >= desc->intro_point_count)
        return TRELLIS_ERR_INVALID_ARG;

    const trellis_intro_point_t *ip = &desc->intro_points[index];
    if (fp_out)
        memcpy(fp_out->bytes, ip->node_fingerprint.bytes, TRELLIS_FINGERPRINT_LEN);
    if (onion_key_out)
        memcpy(onion_key_out, ip->onion_key, TRELLIS_ML_KEM_1024_PK_LEN);

    return TRELLIS_OK;
}

trellis_err_t trellis_descriptor_get_service_identity(
    const trellis_service_descriptor_t *desc,
    uint8_t *buf, size_t buf_len, size_t *written)
{
    if (!desc || !buf || !written)
        return TRELLIS_ERR_INVALID_ARG;

    size_t needed = trellis_identity_public_serialize(
        &desc->service_identity, NULL, 0);
    if (buf_len < needed)
        return TRELLIS_ERR_NOMEM;

    *written = trellis_identity_public_serialize(
        &desc->service_identity, buf, buf_len);
    return TRELLIS_OK;
}
