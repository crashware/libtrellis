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
 * surb.c — Single-Use Reply Blocks (SURB)
 *
 * A SURB is a pre-computed onion envelope constructed by the sender and
 * bundled with an outgoing message.  The recipient can use it to send one
 * reply message that arrives at the sender without the recipient ever
 * learning the sender's real fingerprint or location.
 *
 * Construction:
 *   1. Sender picks a return path: [hop1, hop2, ..., hopN, sender_rv]
 *      where sender_rv is a rendezvous node the sender is listening on.
 *   2. Sender pre-wraps the return-path into N+1 onion layers using
 *      trellis_onion_wrap().  The innermost layer delivers to sender_rv,
 *      who will forward to the sender's waiting connection.
 *   3. The SURB blob = [rv_fingerprint(32) || wrapped_circuit_blob].
 *
 * Use:
 *   Recipient calls trellis_surb_reply(surb, reply_data, len).
 *   Internally this extracts the RV fingerprint, wraps reply_data inside
 *   the onion, and sends it through the mesh toward hop1.
 *
 * The SURB is single-use: replay protection is enforced by including a
 * one-time nonce in the SURB blob which the rendezvous node checks.
 */

#include "internal.h"
#include <trellis/greenhouse.h>
#include <trellis/crypto.h>
#include <sodium.h>
#include <stdio.h>

// Maximum number of hops in a SURB path.
#define SURB_MAX_HOPS 4

/*
 * Wire layout of a serialised SURB blob:
 *   [32]  rv_fingerprint
 *   [32]  one-time nonce (for replay protection)
 *   [4]   hop_count (big-endian)
 *   [hop_count * 32]  hop fingerprints (entry → rv)
 *   [4]   wrapped_len
 *   [wrapped_len]  pre-built onion layers
 */
#define SURB_HEADER_LEN  (TRELLIS_FINGERPRINT_LEN + 32 + 4)

struct trellis_surb {
    trellis_fingerprint_t rv_fingerprint;
    uint8_t               nonce[32];
    /* reply_key is derived from nonce via HKDF and embedded in the innermost
     * onion layer.  The replier uses it to secretbox-encrypt the reply payload.
     * Alice retains this key locally to decrypt the reply she receives from rv. */
    uint8_t               reply_key[32];
    trellis_fingerprint_t hops[SURB_MAX_HOPS + 1]; /* hops[last] = rv */
    size_t                hop_count;
    uint8_t              *wrapped;
    size_t                wrapped_len;
    bool                  used;
};

trellis_surb_t *trellis_surb_build(
    const trellis_fingerprint_t   *rv_fp,
    const trellis_identity_public_t *rv_identity,
    const trellis_peer_info_t     **hops,
    size_t                          hop_count,
    const uint8_t                  *dummy_payload,
    size_t                          dummy_len)
{
    (void)dummy_payload; (void)dummy_len; /* reply_key is always used as inner payload */

    if (!rv_fp || !rv_identity || !hops || hop_count == 0 ||
        hop_count > SURB_MAX_HOPS)
        return NULL;

    trellis_surb_t *s = calloc(1, sizeof(*s));
    if (!s)
        return NULL;

    s->rv_fingerprint = *rv_fp;
    randombytes_buf(s->nonce, sizeof(s->nonce));

    /* Derive the reply encryption key from the nonce.
     * reply_key = HKDF-Expand(PRK=nonce, info="surb-reply-key\x01", L=32)
     * This is RFC 5869 HKDF-Expand with T(1) = HMAC(PRK, info || 0x01). */
    {
        static const uint8_t expand_label[] = "surb-reply-key\x01";
        crypto_auth_hmacsha256_state hs;
        crypto_auth_hmacsha256_init(&hs, s->nonce, sizeof(s->nonce));
        crypto_auth_hmacsha256_update(&hs, expand_label, sizeof(expand_label) - 1);
        crypto_auth_hmacsha256_final(&hs, s->reply_key);
    }

    // Build path: hops[0..hop_count-1] then rv.
    s->hop_count = hop_count + 1;
    for (size_t i = 0; i < hop_count; i++)
        s->hops[i] = hops[i]->fingerprint;
    s->hops[hop_count] = *rv_fp;

    // Build onion circuit from entry to rv.
    trellis_onion_circuit_t *circuit = trellis_onion_new(s->hop_count);
    if (!circuit) {
        free(s);
        return NULL;
    }

    for (size_t i = 0; i < hop_count; i++) {
        trellis_err_t err = trellis_onion_add_hop(
            circuit,
            &hops[i]->fingerprint,
            hops[i]->identity.x25519_pk,
            hops[i]->identity.ml_kem_pk);
        if (err != TRELLIS_OK) {
            trellis_onion_free(circuit);
            free(s);
            return NULL;
        }
    }
    /* RV node uses its own published identity keys — NOT the last relay's keys.
     * Using separate keys prevents the last relay from decrypting the RV layer
     * and learning the rendezvous point address. */
    trellis_err_t err = trellis_onion_add_hop(
        circuit, rv_fp,
        rv_identity->x25519_pk,
        rv_identity->ml_kem_pk);
    if (err != TRELLIS_OK) {
        trellis_onion_free(circuit);
        free(s);
        return NULL;
    }

    /* Wrap reply_key as the innermost payload.  When the rv node peels all
     * layers it recovers the reply_key.  The replier (Bob) uses reply_key to
     * XSalsa20-Poly1305 encrypt the actual reply before sending. */
    trellis_buf_t wrapped = {0};
    err = trellis_onion_wrap(circuit, s->reply_key, sizeof(s->reply_key),
                             &wrapped);
    trellis_onion_free(circuit);
    if (err != TRELLIS_OK) {
        free(s);
        return NULL;
    }

    s->wrapped     = wrapped.data;
    s->wrapped_len = wrapped.len;
    return s;
}

void trellis_surb_free(trellis_surb_t *s)
{
    if (!s)
        return;
    sodium_memzero(s->reply_key, sizeof(s->reply_key));
    sodium_memzero(s->nonce, sizeof(s->nonce));
    free(s->wrapped);
    free(s);
}

trellis_err_t trellis_surb_serialize(const trellis_surb_t *s, trellis_buf_t *out)
{
    if (!s || !out)
        return TRELLIS_ERR_INVALID_ARG;

    size_t total = TRELLIS_FINGERPRINT_LEN   /* rv fp */
                 + 32                         /* nonce */
                 + 32                         /* reply_key */
                 + 4                          /* hop_count */
                 + s->hop_count * TRELLIS_FINGERPRINT_LEN
                 + 4                          /* wrapped_len */
                 + s->wrapped_len;

    *out = trellis_buf_alloc(total);
    if (!out->data)
        return TRELLIS_ERR_NOMEM;

    uint8_t *p = out->data;

    memcpy(p, s->rv_fingerprint.bytes, TRELLIS_FINGERPRINT_LEN);
    p += TRELLIS_FINGERPRINT_LEN;

    memcpy(p, s->nonce, 32);
    p += 32;

    memcpy(p, s->reply_key, 32);
    p += 32;

    p[0] = (uint8_t)((s->hop_count >> 24) & 0xFF);
    p[1] = (uint8_t)((s->hop_count >> 16) & 0xFF);
    p[2] = (uint8_t)((s->hop_count >>  8) & 0xFF);
    p[3] = (uint8_t)(s->hop_count & 0xFF);
    p += 4;

    for (size_t i = 0; i < s->hop_count; i++) {
        memcpy(p, s->hops[i].bytes, TRELLIS_FINGERPRINT_LEN);
        p += TRELLIS_FINGERPRINT_LEN;
    }

    p[0] = (uint8_t)((s->wrapped_len >> 24) & 0xFF);
    p[1] = (uint8_t)((s->wrapped_len >> 16) & 0xFF);
    p[2] = (uint8_t)((s->wrapped_len >>  8) & 0xFF);
    p[3] = (uint8_t)(s->wrapped_len & 0xFF);
    p += 4;

    if (s->wrapped_len > 0)
        memcpy(p, s->wrapped, s->wrapped_len);

    out->len = total;
    return TRELLIS_OK;
}

trellis_err_t trellis_surb_deserialize(const uint8_t *data, size_t len,
                                        trellis_surb_t **out)
{
    if (!data || !out)
        return TRELLIS_ERR_INVALID_ARG;

    size_t min_len = TRELLIS_FINGERPRINT_LEN + 32 + 32 + 4; /* rv_fp + nonce + reply_key + hop_count */
    if (len < min_len)
        return TRELLIS_ERR_DECODE;

    trellis_surb_t *s = calloc(1, sizeof(*s));
    if (!s)
        return TRELLIS_ERR_NOMEM;

    const uint8_t *p = data;

    memcpy(s->rv_fingerprint.bytes, p, TRELLIS_FINGERPRINT_LEN);
    p += TRELLIS_FINGERPRINT_LEN;

    memcpy(s->nonce, p, 32);
    p += 32;

    memcpy(s->reply_key, p, 32);
    p += 32;

    size_t hc = ((size_t)p[0] << 24) | ((size_t)p[1] << 16)
              | ((size_t)p[2] <<  8) |  (size_t)p[3];
    p += 4;

    if (hc == 0 || hc > SURB_MAX_HOPS + 1 ||
        (size_t)(p - data) + hc * TRELLIS_FINGERPRINT_LEN + 4 > len) {
        free(s);
        return TRELLIS_ERR_DECODE;
    }

    s->hop_count = hc;
    for (size_t i = 0; i < hc; i++) {
        memcpy(s->hops[i].bytes, p, TRELLIS_FINGERPRINT_LEN);
        p += TRELLIS_FINGERPRINT_LEN;
    }

    size_t wl = ((size_t)p[0] << 24) | ((size_t)p[1] << 16)
              | ((size_t)p[2] <<  8) |  (size_t)p[3];
    p += 4;

    if ((size_t)(p - data) + wl > len) {
        free(s);
        return TRELLIS_ERR_DECODE;
    }

    if (wl > 0) {
        s->wrapped = malloc(wl);
        if (!s->wrapped) {
            free(s);
            return TRELLIS_ERR_NOMEM;
        }
        memcpy(s->wrapped, p, wl);
    }
    s->wrapped_len = wl;

    *out = s;
    return TRELLIS_OK;
}

/*
 * trellis_surb_reply — encrypt reply_data under the SURB's reply_key and
 * prepend it to the pre-built onion routing header.
 *
 * Wire layout of the produced blob (send toward s->hops[0]):
 *   [24]  XSalsa20-Poly1305 nonce (random, one-time)
 *   [4]   ciphertext length (reply_len + crypto_secretbox_MACBYTES), big-endian
 *   [ct_len] XSalsa20-Poly1305 ciphertext
 *   [wrapped_len] pre-built onion routing header (peeled by each relay)
 *
 * Each relay peels one onion layer from the tail; the structure of the
 * leading nonce+ct block is opaque to relays and passed through intact.
 * The rv node (last hop) delivers [nonce || ct_len || ct] to Alice.
 * Alice calls trellis_surb_decrypt_reply() to recover the plaintext.
 */
trellis_err_t trellis_surb_reply(trellis_surb_t *s,
                                  const uint8_t *reply_data, size_t reply_len,
                                  trellis_buf_t *onion_out)
{
    if (!s || !reply_data || !onion_out)
        return TRELLIS_ERR_INVALID_ARG;

    if (s->used)
        return TRELLIS_ERR_ALREADY_EXISTS; /* single-use enforcement */

    if (s->hop_count == 0 || !s->wrapped || s->wrapped_len == 0)
        return TRELLIS_ERR_INVALID_ARG;

    // Encrypt reply under reply_key.
    size_t ct_len = reply_len + crypto_secretbox_MACBYTES;
    uint8_t sb_nonce[crypto_secretbox_NONCEBYTES];
    randombytes_buf(sb_nonce, sizeof(sb_nonce));

    uint8_t *ct = malloc(ct_len);
    if (!ct)
        return TRELLIS_ERR_NOMEM;

    if (crypto_secretbox_easy(ct, reply_data, reply_len,
                              sb_nonce, s->reply_key) != 0) {
        free(ct);
        return TRELLIS_ERR_ENCRYPT;
    }

    // Build the full packet: [nonce(24) || ct_len(4) || ct || onion_header]
    size_t total = (size_t)crypto_secretbox_NONCEBYTES + 4 + ct_len + s->wrapped_len;
    *onion_out = trellis_buf_alloc(total);
    if (!onion_out->data) {
        free(ct);
        return TRELLIS_ERR_NOMEM;
    }

    uint8_t *p = onion_out->data;
    memcpy(p, sb_nonce, crypto_secretbox_NONCEBYTES);
    p += crypto_secretbox_NONCEBYTES;
    p[0] = (uint8_t)((ct_len >> 24) & 0xFF);
    p[1] = (uint8_t)((ct_len >> 16) & 0xFF);
    p[2] = (uint8_t)((ct_len >>  8) & 0xFF);
    p[3] = (uint8_t)(ct_len & 0xFF);
    p += 4;
    memcpy(p, ct, ct_len);
    p += ct_len;
    memcpy(p, s->wrapped, s->wrapped_len);

    free(ct);
    onion_out->len = total;

    s->used = true;
    fprintf(stderr, "[SURB] reply encrypted and queued toward entry hop %02x%02x...\n",
            s->hops[0].bytes[0], s->hops[0].bytes[1]);
    return TRELLIS_OK;
}

/*
 * trellis_surb_decrypt_reply — called by Alice (the SURB originator) to
 * decrypt the [nonce || ct_len || ct] blob delivered by the rv node.
 *
 * The caller must supply the original trellis_surb_t (which holds reply_key).
 * On success, *reply_out is malloc'd and must be freed by the caller.
 */
trellis_err_t trellis_surb_decrypt_reply(const trellis_surb_t *s,
                                          const uint8_t *sealed, size_t sealed_len,
                                          uint8_t **reply_out, size_t *reply_len_out)
{
    if (!s || !sealed || !reply_out || !reply_len_out)
        return TRELLIS_ERR_INVALID_ARG;

    size_t min = (size_t)crypto_secretbox_NONCEBYTES + 4 + crypto_secretbox_MACBYTES;
    if (sealed_len < min)
        return TRELLIS_ERR_DECODE;

    const uint8_t *nonce = sealed;
    const uint8_t *p     = sealed + crypto_secretbox_NONCEBYTES;
    size_t ct_len = ((size_t)p[0] << 24) | ((size_t)p[1] << 16)
                  | ((size_t)p[2] <<  8) |  (size_t)p[3];
    p += 4;

    if (ct_len < crypto_secretbox_MACBYTES ||
        (size_t)(p - sealed) + ct_len > sealed_len)
        return TRELLIS_ERR_DECODE;

    size_t plain_len = ct_len - crypto_secretbox_MACBYTES;
    uint8_t *plain = malloc(plain_len + 1);
    if (!plain)
        return TRELLIS_ERR_NOMEM;

    if (crypto_secretbox_open_easy(plain, p, ct_len, nonce, s->reply_key) != 0) {
        free(plain);
        return TRELLIS_ERR_VERIFY_FAILED;
    }

    plain[plain_len] = '\0'; /* NUL-terminate for string safety */
    *reply_out     = plain;
    *reply_len_out = plain_len;
    return TRELLIS_OK;
}

bool trellis_surb_is_used(const trellis_surb_t *s)
{
    return s && s->used;
}

const trellis_fingerprint_t *trellis_surb_entry_hop(const trellis_surb_t *s)
{
    return (s && s->hop_count > 0) ? &s->hops[0] : NULL;
}

const trellis_fingerprint_t *trellis_surb_rv_fingerprint(const trellis_surb_t *s)
{
    return s ? &s->rv_fingerprint : NULL;
}
