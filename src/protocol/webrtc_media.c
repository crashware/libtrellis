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
 * webrtc_media.c — WebRTC Media Session and Signaling
 *
 * Implements the Bloom-native WebRTC signaling layer (Section 8.7).
 *
 * SDP offer/answer and ICE candidates are exchanged as encrypted Bloom
 * protocol messages (TRELLIS_MSG_WEBRTC_*).  The actual RTP/SRTP media
 * flow uses the standard WebRTC path (via libdatachannel / browser WebRTC)
 * and is NOT routed through the Bloom overlay (it uses the morph-bypass
 * flag to indicate this).
 *
 * The webrtc_stego transport (Phase 4) provides an alternative "full
 * camouflage" mode where media is actually steganographically embedded;
 * this module handles the normal "NeverCast call over Bloom signaling" path.
 *
 * Message payload formats:
 *
 * WEBRTC_OFFER / WEBRTC_ANSWER / WEBRTC_RENEGOTIATE:
 *   [4]  magic "WSDP"
 *   [1]  version (1)
 *   [1]  media_type_flags (bitmask of trellis_webrtc_media_type_t)
 *   [1]  morph_bypass (bool: 1 = peer requests morph bypass for media)
 *   [4]  sdp_len (LE)
 *   [N]  sdp (UTF-8 SDP string)
 *
 * WEBRTC_ICE:
 *   [4]  magic "WICE"
 *   [1]  version (1)
 *   [4]  sdp_mline_index (LE)
 *   [1]  sdp_mid_len
 *   [M]  sdp_mid
 *   [4]  candidate_len (LE)
 *   [N]  candidate (UTF-8 ICE candidate string)
 *
 * WEBRTC_HANGUP:
 *   [4]  magic "WHUP"
 *   [1]  version (1)
 *   [1]  reason (0=normal, 1=error, 2=rejected)
 */

#include "internal.h"
#include <trellis/protocol.h>
#include <trellis/error.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#define WSDP_MAGIC  "WSDP"
#define WICE_MAGIC  "WICE"
#define WHUP_MAGIC  "WHUP"
#define WM_VERSION  1
#define MAX_SDP_LEN (64 * 1024)   /* 64 KB max SDP */
#define MAX_CAND_LEN 1024

struct trellis_webrtc_session {
    trellis_fingerprint_t    remote_fp;
    bool                     is_offerer;

    trellis_webrtc_offer_cb  on_offer;
    void                    *on_offer_ctx;

    trellis_webrtc_offer_cb  on_answer;
    void                    *on_answer_ctx;

    trellis_webrtc_ice_cb    on_ice;
    void                    *on_ice_ctx;

    trellis_webrtc_hangup_cb on_hangup;
    void                    *on_hangup_ctx;
};

static void write_u32_le(uint8_t *buf, uint32_t v)
{
    buf[0] = (uint8_t)(v);
    buf[1] = (uint8_t)(v >> 8);
    buf[2] = (uint8_t)(v >> 16);
    buf[3] = (uint8_t)(v >> 24);
}

static uint32_t read_u32_le(const uint8_t *buf)
{
    return (uint32_t)buf[0]
         | ((uint32_t)buf[1] << 8)
         | ((uint32_t)buf[2] << 16)
         | ((uint32_t)buf[3] << 24);
}

trellis_webrtc_session_t *trellis_webrtc_session_new(
        const trellis_fingerprint_t *remote_fp,
        bool is_offerer)
{
    if (!remote_fp) return NULL;
    trellis_webrtc_session_t *s = calloc(1, sizeof(*s));
    if (!s) return NULL;
    s->remote_fp  = *remote_fp;
    s->is_offerer = is_offerer;
    return s;
}

void trellis_webrtc_session_free(trellis_webrtc_session_t *s)
{
    if (!s) return;
    free(s);
}

void trellis_webrtc_session_on_offer(trellis_webrtc_session_t *s,
                                      trellis_webrtc_offer_cb cb, void *ctx)
{
    if (!s) return;
    s->on_offer     = cb;
    s->on_offer_ctx = ctx;
}

void trellis_webrtc_session_on_answer(trellis_webrtc_session_t *s,
                                       trellis_webrtc_offer_cb cb, void *ctx)
{
    if (!s) return;
    s->on_answer     = cb;
    s->on_answer_ctx = ctx;
}

void trellis_webrtc_session_on_ice(trellis_webrtc_session_t *s,
                                    trellis_webrtc_ice_cb cb, void *ctx)
{
    if (!s) return;
    s->on_ice     = cb;
    s->on_ice_ctx = ctx;
}

void trellis_webrtc_session_on_hangup(trellis_webrtc_session_t *s,
                                       trellis_webrtc_hangup_cb cb, void *ctx)
{
    if (!s) return;
    s->on_hangup     = cb;
    s->on_hangup_ctx = ctx;
}

trellis_err_t trellis_webrtc_session_dispatch(trellis_webrtc_session_t *s,
                                               const trellis_message_t *msg)
{
    if (!s || !msg || !msg->payload) return TRELLIS_ERR_INVALID_ARG;

    const uint8_t *p   = msg->payload;
    size_t         len = msg->payload_len;

    switch ((trellis_msg_type_t)msg->type) {
    case TRELLIS_MSG_WEBRTC_OFFER:
    case TRELLIS_MSG_WEBRTC_ANSWER:
    case TRELLIS_MSG_WEBRTC_RENEGOTIATE: {
        if (len < 4 + 1 + 1 + 1 + 4) return TRELLIS_ERR_MSG_FORMAT;
        if (memcmp(p, WSDP_MAGIC, 4) != 0) return TRELLIS_ERR_MSG_FORMAT;
        if (p[4] != WM_VERSION) return TRELLIS_ERR_MSG_FORMAT;
        // p[5] = media_type_flags, p[6] = morph_bypass.
        uint32_t sdp_len = read_u32_le(p + 7);
        if (sdp_len > MAX_SDP_LEN || len < 4 + 1 + 1 + 1 + 4 + sdp_len)
            return TRELLIS_ERR_MSG_FORMAT;

        // Null-terminate the SDP string for the callback.
        char *sdp = malloc(sdp_len + 1);
        if (!sdp) return TRELLIS_ERR_NOMEM;
        memcpy(sdp, p + 11, sdp_len);
        sdp[sdp_len] = '\0';

        trellis_webrtc_offer_cb cb  = (msg->type == TRELLIS_MSG_WEBRTC_OFFER ||
                                        msg->type == TRELLIS_MSG_WEBRTC_RENEGOTIATE)
                                      ? s->on_offer  : s->on_answer;
        void                  *ctx = (msg->type == TRELLIS_MSG_WEBRTC_OFFER ||
                                        msg->type == TRELLIS_MSG_WEBRTC_RENEGOTIATE)
                                      ? s->on_offer_ctx : s->on_answer_ctx;
        if (cb) cb(sdp, ctx);
        free(sdp);
        break;
    }

    case TRELLIS_MSG_WEBRTC_ICE: {
        if (len < 4 + 1 + 4 + 1) return TRELLIS_ERR_MSG_FORMAT;
        if (memcmp(p, WICE_MAGIC, 4) != 0) return TRELLIS_ERR_MSG_FORMAT;
        if (p[4] != WM_VERSION) return TRELLIS_ERR_MSG_FORMAT;

        size_t off = 5;
        uint32_t mline_index = read_u32_le(p + off); off += 4;
        if (len < off + 1) return TRELLIS_ERR_MSG_FORMAT;
        uint8_t mid_len = p[off++];
        if (len < off + mid_len + 4) return TRELLIS_ERR_MSG_FORMAT;
        char *sdp_mid = malloc(mid_len + 1);
        if (!sdp_mid) return TRELLIS_ERR_NOMEM;
        memcpy(sdp_mid, p + off, mid_len);
        sdp_mid[mid_len] = '\0';
        off += mid_len;

        uint32_t cand_len = read_u32_le(p + off); off += 4;
        if (cand_len > MAX_CAND_LEN || len < off + cand_len) {
            free(sdp_mid); return TRELLIS_ERR_MSG_FORMAT;
        }
        char *candidate = malloc(cand_len + 1);
        if (!candidate) { free(sdp_mid); return TRELLIS_ERR_NOMEM; }
        memcpy(candidate, p + off, cand_len);
        candidate[cand_len] = '\0';

        if (s->on_ice)
            s->on_ice(candidate, sdp_mid, (int)mline_index, s->on_ice_ctx);
        free(sdp_mid);
        free(candidate);
        break;
    }

    case TRELLIS_MSG_WEBRTC_HANGUP:
        if (len >= 5 && memcmp(p, WHUP_MAGIC, 4) == 0) {
            if (s->on_hangup) s->on_hangup(s->on_hangup_ctx);
        }
        break;

    default:
        return TRELLIS_ERR_MSG_FORMAT;
    }
    return TRELLIS_OK;
}

static trellis_err_t msg_alloc_payload(trellis_message_t *msg, size_t len)
{
    msg->payload = malloc(len);
    if (!msg->payload) return TRELLIS_ERR_NOMEM;
    msg->payload_len = (uint32_t)len;
    return TRELLIS_OK;
}

static void fill_header(trellis_message_t *msg,
                          uint16_t type,
                          const trellis_fingerprint_t *src,
                          const trellis_fingerprint_t *dst,
                          uint8_t flags)
{
    msg->version  = 1;
    msg->type     = type;
    msg->flags    = flags;
    msg->sequence = 0; /* caller sets */
    msg->timestamp = 0;
    msg->source   = *src;
    msg->target   = *dst;
}

trellis_err_t trellis_webrtc_build_offer(const trellis_fingerprint_t *src,
                                          const trellis_fingerprint_t *dst,
                                          const char *sdp,
                                          trellis_webrtc_media_type_t types,
                                          bool morph_bypass,
                                          trellis_message_t *msg_out)
{
    if (!src || !dst || !sdp || !msg_out) return TRELLIS_ERR_INVALID_ARG;
    size_t sdp_len = strlen(sdp);
    if (sdp_len > MAX_SDP_LEN) return TRELLIS_ERR_INVALID_ARG;

    size_t payload_len = 4 + 1 + 1 + 1 + 4 + sdp_len;
    trellis_err_t err = msg_alloc_payload(msg_out, payload_len);
    if (err != TRELLIS_OK) return err;

    uint8_t *p = msg_out->payload;
    memcpy(p, WSDP_MAGIC, 4);      p += 4;
    *p++ = WM_VERSION;
    *p++ = (uint8_t)types;
    *p++ = morph_bypass ? 1 : 0;
    write_u32_le(p, (uint32_t)sdp_len); p += 4;
    memcpy(p, sdp, sdp_len);

    uint8_t flags = morph_bypass ? TRELLIS_FLAG_MORPH_BYPASS : 0;
    fill_header(msg_out, TRELLIS_MSG_WEBRTC_OFFER, src, dst, flags);
    return TRELLIS_OK;
}

trellis_err_t trellis_webrtc_build_answer(const trellis_fingerprint_t *src,
                                           const trellis_fingerprint_t *dst,
                                           const char *sdp,
                                           trellis_message_t *msg_out)
{
    if (!src || !dst || !sdp || !msg_out) return TRELLIS_ERR_INVALID_ARG;
    size_t sdp_len = strlen(sdp);
    if (sdp_len > MAX_SDP_LEN) return TRELLIS_ERR_INVALID_ARG;

    size_t payload_len = 4 + 1 + 1 + 1 + 4 + sdp_len;
    trellis_err_t err = msg_alloc_payload(msg_out, payload_len);
    if (err != TRELLIS_OK) return err;

    uint8_t *p = msg_out->payload;
    memcpy(p, WSDP_MAGIC, 4);      p += 4;
    *p++ = WM_VERSION;
    *p++ = 0; /* media_type_flags = 0 in answer */
    *p++ = 0; /* morph_bypass = follow offerer's preference */
    write_u32_le(p, (uint32_t)sdp_len); p += 4;
    memcpy(p, sdp, sdp_len);

    fill_header(msg_out, TRELLIS_MSG_WEBRTC_ANSWER, src, dst, 0);
    return TRELLIS_OK;
}

trellis_err_t trellis_webrtc_build_ice(const trellis_fingerprint_t *src,
                                        const trellis_fingerprint_t *dst,
                                        const char *candidate,
                                        const char *sdp_mid,
                                        int sdp_mline_index,
                                        trellis_message_t *msg_out)
{
    if (!src || !dst || !candidate || !sdp_mid || !msg_out)
        return TRELLIS_ERR_INVALID_ARG;

    size_t cand_len = strlen(candidate);
    size_t mid_len  = strlen(sdp_mid);
    if (cand_len > MAX_CAND_LEN || mid_len > 255)
        return TRELLIS_ERR_INVALID_ARG;

    size_t payload_len = 4 + 1 + 4 + 1 + mid_len + 4 + cand_len;
    trellis_err_t err = msg_alloc_payload(msg_out, payload_len);
    if (err != TRELLIS_OK) return err;

    uint8_t *p = msg_out->payload;
    memcpy(p, WICE_MAGIC, 4);        p += 4;
    *p++ = WM_VERSION;
    write_u32_le(p, (uint32_t)sdp_mline_index); p += 4;
    *p++ = (uint8_t)mid_len;
    memcpy(p, sdp_mid, mid_len);     p += mid_len;
    write_u32_le(p, (uint32_t)cand_len); p += 4;
    memcpy(p, candidate, cand_len);

    fill_header(msg_out, TRELLIS_MSG_WEBRTC_ICE, src, dst, 0);
    return TRELLIS_OK;
}

trellis_err_t trellis_webrtc_build_hangup(const trellis_fingerprint_t *src,
                                           const trellis_fingerprint_t *dst,
                                           trellis_message_t *msg_out)
{
    if (!src || !dst || !msg_out) return TRELLIS_ERR_INVALID_ARG;

    size_t payload_len = 4 + 1 + 1;
    trellis_err_t err = msg_alloc_payload(msg_out, payload_len);
    if (err != TRELLIS_OK) return err;

    uint8_t *p = msg_out->payload;
    memcpy(p, WHUP_MAGIC, 4);  p += 4;
    *p++ = WM_VERSION;
    *p++ = 0; /* reason: normal */

    fill_header(msg_out, TRELLIS_MSG_WEBRTC_HANGUP, src, dst, 0);
    return TRELLIS_OK;
}
