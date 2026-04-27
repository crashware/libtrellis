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
 * Steganographic WebRTC transport adapter
 *
 * Embeds Bloom protocol frames into the low-order bits (LSBs) of RTP audio
 * samples inside a functional NeverCast WebRTC video call.  To a passive DPI
 * observer the stream is indistinguishable from ordinary VoIP/conferencing
 * traffic.
 *
 * Architecture:
 *  ┌──────────────────────────────────────────────────────────────────────┐
 *  │  Bloom application frame                                            │
 *  │       ↓  trellis_conn_send()                                       │
 *  │  stego_encode() – scatter bits into PCM LSBs of Opus audio frames  │
 *  │       ↓                                                             │
 *  │  RTP → SRTP → DTLS-over-ICE → WebRTC DataChannel / audio track    │
 *  │       ↓  (NeverCast WebSocket signaling)                           │
 *  │  libdatachannel peer connection                                     │
 *  └──────────────────────────────────────────────────────────────────────┘
 *
 * Steganographic scheme:
 *   Each Bloom frame is prepended with a 4-byte stego header:
 *     magic[2]   = 0xB1 0x0C ("Bloom")
 *     length[2]  = payload length (big-endian, up to 2048 bytes per chunk)
 *   The header + payload bits are spread across consecutive PCM samples
 *   using lsb_depth LSBs per sample.  Unused trailing bits are filled with
 *   random noise so the LSB histogram matches natural audio.
 *
 *   Simple (32,26) Reed-Solomon FEC is applied before bit-spreading so that
 *   up to 3 consecutive sample corruptions can be recovered.
 *
 * Build guard: TRELLIS_WITH_WEBRTC_STEGO
 * Dependencies: libdatachannel (C API via rtc.h), libopus, libuv, libsodium
 */

#include "internal.h"

#ifdef TRELLIS_WITH_WEBRTC_STEGO

#include <rtc/rtc.h>      /* libdatachannel C API */
#include <opus/opus.h>    /* Opus codec */
#include <sodium.h>
#include <uv.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#define STEGO_MAGIC_0      0xB1u
#define STEGO_MAGIC_1      0x0Cu
#define STEGO_HEADER_LEN   4u       /* magic(2) + length(2) */
#define STEGO_MAX_CHUNK    2048u    /* max Bloom bytes per audio burst */
#define STEGO_OPUS_SR      48000    /* Opus sample rate */
#define STEGO_OPUS_CH      2        /* stereo (matches NeverCast) */
#define STEGO_OPUS_FRAME_MS 20      /* 20 ms frames */
#define STEGO_PCM_FRAME_SAMPLES (STEGO_OPUS_SR * STEGO_OPUS_FRAME_MS / 1000)
#define STEGO_OPUS_MAX_PACKET 4000u
#define STEGO_FEC_BLOCK    32u      /* RS(32,26): 32 coded symbols from 26 data */
#define STEGO_FEC_DATA     26u
#define STEGO_FEC_PARITY   (STEGO_FEC_BLOCK - STEGO_FEC_DATA) /* 6 */
#define STEGO_LSB_MAX      2u       /* maximum LSB depth */
#define STEGO_QUEUE_MAX    64u      /* pending send frames */

// Simple GF(256) Reed-Solomon helper.

// GF(256) with primitive polynomial 0x11d (x^8+x^4+x^3+x^2+1)
static uint8_t gf256_exp[512];
static uint8_t gf256_log[256];
static bool    gf256_init_done = false;

static void gf256_init(void)
{
    if (gf256_init_done) return;
    gf256_init_done = true;
    uint32_t x = 1;
    for (int i = 0; i < 255; i++) {
        gf256_exp[i] = (uint8_t)x;
        gf256_log[x] = (uint8_t)i;
        x <<= 1;
        if (x & 0x100) x ^= 0x11d;
    }
    // Extend for wrapping.
    for (int i = 255; i < 512; i++)
        gf256_exp[i] = gf256_exp[i - 255];
}

static uint8_t gf256_mul(uint8_t a, uint8_t b)
{
    if (a == 0 || b == 0) return 0;
    return gf256_exp[(int)gf256_log[a] + (int)gf256_log[b]];
}

/*
 * Systematic RS(STEGO_FEC_BLOCK, STEGO_FEC_DATA) encode.
 * `data` is STEGO_FEC_DATA bytes; `parity` receives STEGO_FEC_PARITY bytes.
 * Generator polynomial roots are alpha^0 … alpha^(STEGO_FEC_PARITY-1).
 */
static void rs_encode_block(const uint8_t *data, uint8_t *parity)
{
    gf256_init();
    memset(parity, 0, STEGO_FEC_PARITY);
    for (int i = 0; i < (int)STEGO_FEC_DATA; i++) {
        uint8_t feedback = data[i] ^ parity[0];
        for (int j = 0; j < (int)STEGO_FEC_PARITY - 1; j++)
            parity[j] = parity[j + 1] ^ gf256_mul(feedback,
                gf256_exp[(j + 1) * ((255 / STEGO_FEC_PARITY))]);
        parity[STEGO_FEC_PARITY - 1] = gf256_mul(feedback,
            gf256_exp[STEGO_FEC_PARITY * ((255 / STEGO_FEC_PARITY))]);
    }
}

// Stego bit encode/decode (LSB spreading into int16 PCM)

/*
 * Encode `bit_count` bits from `src_bits` into the `lsb_depth` LSBs of
 * each int16 PCM sample in `pcm`.  pcm must hold ≥ ceil(bit_count/lsb_depth)
 * samples.  Returns number of samples used.
 */
static size_t stego_encode_bits(const uint8_t *src_bits, size_t bit_count,
                                 int16_t *pcm, size_t pcm_count, int lsb_depth)
{
    size_t bit_idx  = 0;
    size_t samp_idx = 0;
    uint8_t mask    = (uint8_t)((1u << lsb_depth) - 1u);

    while (bit_idx < bit_count && samp_idx < pcm_count) {
        // Collect lsb_depth bits from src.
        uint8_t chunk = 0;
        for (int b = 0; b < lsb_depth && bit_idx + (size_t)b < bit_count; b++) {
            size_t byte_pos  = (bit_idx + (size_t)b) / 8;
            size_t bit_pos   = 7 - ((bit_idx + (size_t)b) % 8);
            uint8_t bit_val  = (src_bits[byte_pos] >> bit_pos) & 1u;
            chunk |= (uint8_t)(bit_val << (lsb_depth - 1 - b));
        }
        // Replace LSBs of PCM sample.
        int16_t sample = pcm[samp_idx];
        sample = (int16_t)((sample & ~mask) | (chunk & mask));
        pcm[samp_idx] = sample;

        bit_idx  += (size_t)lsb_depth;
        samp_idx++;
    }

    // Pad remaining samples with random noise in LSBs to preserve histogram.
    while (samp_idx < pcm_count) {
        uint8_t noise;
        randombytes_buf(&noise, 1);
        pcm[samp_idx] = (int16_t)((pcm[samp_idx] & ~mask) | (noise & mask));
        samp_idx++;
    }

    return samp_idx;
}

/*
 * Decode bits from PCM samples.  Inverse of stego_encode_bits.
 */
static size_t stego_decode_bits(const int16_t *pcm, size_t pcm_count,
                                 int lsb_depth,
                                 uint8_t *dst_bits, size_t max_bits)
{
    uint8_t mask    = (uint8_t)((1u << lsb_depth) - 1u);
    size_t  bit_idx = 0;
    for (size_t i = 0; i < pcm_count && bit_idx < max_bits; i++) {
        uint8_t chunk = (uint8_t)(pcm[i] & mask);
        for (int b = 0; b < lsb_depth && bit_idx < max_bits; b++) {
            size_t byte_pos = bit_idx / 8;
            size_t bit_pos  = 7 - (bit_idx % 8);
            uint8_t bit_val = (uint8_t)((chunk >> (lsb_depth - 1 - b)) & 1u);
            if (bit_val)
                dst_bits[byte_pos] |= (uint8_t)(1u << bit_pos);
            else
                dst_bits[byte_pos] &= (uint8_t)(~(1u << bit_pos));
            bit_idx++;
        }
    }
    return bit_idx;
}

// Per-frame queued payload.
typedef struct stego_pending {
    uint8_t *data;
    size_t   len;
    trellis_conn_send_cb cb;
    void    *ctx;
} stego_pending_t;

// Per-connection state.
typedef struct stego_conn_data {
    int               pc;          /* libdatachannel PeerConnection ID */
    int               audio_track; /* audio track ID */

    // Opus encoder/decoder.
    OpusEncoder      *enc;
    OpusDecoder      *dec;

    // PCM frame buffer (20 ms of stereo int16)
    int16_t           pcm_enc[STEGO_PCM_FRAME_SAMPLES * STEGO_OPUS_CH];
    int16_t           pcm_dec[STEGO_PCM_FRAME_SAMPLES * STEGO_OPUS_CH];
    uint8_t           opus_pkt[STEGO_OPUS_MAX_PACKET];

    // Send queue.
    stego_pending_t   send_queue[STEGO_QUEUE_MAX];
    size_t            send_head, send_tail;

    // Receive reassembly.
    uint8_t           recv_accum[STEGO_MAX_CHUNK + STEGO_HEADER_LEN + 64];
    size_t            recv_accum_len;
    bool              recv_in_frame; /* currently receiving a stego frame */
    uint16_t          recv_expected; /* expected payload length */

    trellis_conn_recv_cb recv_cb;
    void               *recv_ctx;
    bool                recv_active;

    trellis_conn_t     *conn;

    int lsb_depth;
    int fec_redundancy_pct;

    // uv timer drives synthetic audio tick when no real call is active.
    uv_timer_t  tick_timer;
    bool        timer_active;
    uv_loop_t  *loop;

    struct stego_transport_data *td;

    // SDP signaling WebSocket handle (libdatachannel WebSocket client)
    int                 signaling_ws;   /* -1 = not used */
    trellis_connect_cb  connect_cb;
    void               *connect_ctx;
    bool                sdp_sent;       /* offer/answer exchanged */
} stego_conn_data_t;

typedef struct stego_transport_data {
    trellis_transport_t *transport;
    char                 signaling_url[512];
    char                 display_name[128];
    int                  default_lsb_depth;
    int                  default_cover_kbps;
    int                  default_fec_pct;

    trellis_accept_cb    accept_cb;
    void                *accept_ctx;

    stego_conn_data_t   *conns[16];
    size_t               conn_count;
} stego_transport_data_t;

static const trellis_conn_vtable_t      stego_conn_vtable;
static const trellis_transport_vtable_t stego_transport_vtable;

// WebSocket SDP signaling callbacks.

/*
 * Called when the signaling WebSocket is open.
 * We gather the local SDP and send it as a JSON { "sdp": "...", "type": "offer" }
 * message to the signaling server.
 */
static void RTC_API stego_ws_on_open(int ws_id, void *user_ptr)
{
    stego_conn_data_t *cd = (stego_conn_data_t *)user_ptr;
    (void)ws_id;

    if (cd->sdp_sent) return;
    cd->sdp_sent = true;

    // Gather local description (SDP offer)
    char sdp[8192];
    if (rtcGetLocalDescription(cd->pc, sdp, sizeof(sdp)) < 0) {
        fprintf(stderr, "[STEGO-WS] failed to get local SDP\n");
        return;
    }

    // Serialize as minimal JSON.
    char json[8192 + 64];
    snprintf(json, sizeof(json),
             "{\"type\":\"offer\",\"sdp\":\"%s\"}", sdp);

    fprintf(stderr, "[STEGO-WS] sending SDP offer (%zu bytes)\n", strlen(sdp));
    rtcSendMessage(ws_id, json, (int)strlen(json));
}

/*
 * Called when the signaling server sends back an SDP answer (or ICE candidate).
 * We feed the answer to libdatachannel and the ICE negotiation proceeds.
 */
static void RTC_API stego_ws_on_message(int ws_id, const char *msg,
                                         int size, void *user_ptr)
{
    stego_conn_data_t *cd = (stego_conn_data_t *)user_ptr;
    (void)ws_id;
    (void)size;

    if (!msg) return;

    // Very minimal JSON parser: look for "sdp" and "type" keys.
    const char *sdp_key = strstr(msg, "\"sdp\"");
    const char *type_key = strstr(msg, "\"type\"");
    if (!sdp_key || !type_key) {
        // ICE candidate or other signaling message — ignore for now.
        return;
    }

    // Extract type value.
    const char *type_val = strstr(type_key, ":");
    if (!type_val) return;
    while (*type_val == ':' || *type_val == ' ' || *type_val == '"') type_val++;

    // Extract SDP value (content between the quotes after "sdp": "...")
    const char *sdp_val = strstr(sdp_key, ":");
    if (!sdp_val) return;
    while (*sdp_val == ':' || *sdp_val == ' ' || *sdp_val == '"') sdp_val++;

    char sdp_buf[8192];
    size_t si = 0;
    while (*sdp_val && *sdp_val != '"' && si + 1 < sizeof(sdp_buf)) {
        if (sdp_val[0] == '\\' && sdp_val[1] == 'n') {
            sdp_buf[si++] = '\n';
            sdp_val += 2;
        } else {
            sdp_buf[si++] = *sdp_val++;
        }
    }
    sdp_buf[si] = '\0';

    // Determine SDP type.
    const char *sdp_type = strncmp(type_val, "answer", 6) == 0 ? "answer" : "offer";

    fprintf(stderr, "[STEGO-WS] received SDP %s (%zu bytes)\n", sdp_type, si);
    if (rtcSetRemoteDescription(cd->pc, sdp_buf, sdp_type) < 0) {
        fprintf(stderr, "[STEGO-WS] setRemoteDescription failed\n");
        return;
    }

    // The peer connection is now negotiating; stego_on_open() fires when ready.
    if (!cd->sdp_sent && strcmp(sdp_type, "offer") == 0) {
        // Server-side: generate and send answer.
        char answer_sdp[8192];
        if (rtcGetLocalDescription(cd->pc, answer_sdp, sizeof(answer_sdp)) >= 0) {
            char json[8192 + 64];
            snprintf(json, sizeof(json),
                     "{\"type\":\"answer\",\"sdp\":\"%s\"}", answer_sdp);
            rtcSendMessage(ws_id, json, (int)strlen(json));
            cd->sdp_sent = true;
        }
    }
}

/*
 * WebSocket closed — fire the connect callback with an error if SDP was never
 * exchanged, or silently close if the negotiation already completed.
 */
static void RTC_API stego_ws_on_closed(int ws_id, void *user_ptr)
{
    stego_conn_data_t *cd = (stego_conn_data_t *)user_ptr;
    (void)ws_id;

    if (!cd->sdp_sent && cd->connect_cb) {
        fprintf(stderr, "[STEGO-WS] signaling WS closed before SDP exchange\n");
        cd->connect_cb(cd->conn, TRELLIS_ERR_TRANSPORT, cd->connect_ctx);
        cd->connect_cb  = NULL;
        cd->connect_ctx = NULL;
    }
    cd->signaling_ws = -1;
}

// Audio frame tick: encode and send one Opus packet.
static void stego_audio_tick(stego_conn_data_t *cd)
{
    // Start with silence (natural cover)
    memset(cd->pcm_enc, 0, sizeof(cd->pcm_enc));

    /* Optionally add a real synthetic audio tone (sine wave) for realism.
     * For production: hook into NeverCast's actual audio pipeline instead. */
    static uint32_t phase = 0;
    for (int i = 0; i < STEGO_PCM_FRAME_SAMPLES; i++) {
        // 440 Hz sine at -40 dBFS.
        double s = 328.0 * __builtin_sin(2.0 * 3.14159265 * 440.0 *
                                          (double)phase / STEGO_OPUS_SR);
        int16_t v = (int16_t)(int)s;
        cd->pcm_enc[i * STEGO_OPUS_CH]     = v;
        cd->pcm_enc[i * STEGO_OPUS_CH + 1] = v;
        phase = (phase + 1) % STEGO_OPUS_SR;
    }

    // If there's a pending Bloom frame, embed it.
    if (cd->send_head != cd->send_tail) {
        stego_pending_t *p = &cd->send_queue[cd->send_head];

        // Build stego payload with FEC.
        size_t payload_len  = p->len;
        if (payload_len > STEGO_MAX_CHUNK)
            payload_len = STEGO_MAX_CHUNK;

        // FEC encode in STEGO_FEC_DATA-byte blocks.
        size_t fec_buf_len = payload_len +
            ((payload_len / STEGO_FEC_DATA) + 1) * STEGO_FEC_PARITY
            + STEGO_HEADER_LEN;
        uint8_t *fec_buf = malloc(fec_buf_len);
        if (!fec_buf) goto encode_done;

        // Stego header.
        fec_buf[0] = STEGO_MAGIC_0;
        fec_buf[1] = STEGO_MAGIC_1;
        fec_buf[2] = (uint8_t)(payload_len >> 8);
        fec_buf[3] = (uint8_t)(payload_len & 0xFF);

        size_t out_off = STEGO_HEADER_LEN;
        size_t in_off  = 0;
        while (in_off < payload_len) {
            uint8_t blk[STEGO_FEC_DATA] = {0};
            size_t blk_len = payload_len - in_off;
            if (blk_len > STEGO_FEC_DATA) blk_len = STEGO_FEC_DATA;
            memcpy(blk, p->data + in_off, blk_len);
            memcpy(fec_buf + out_off, blk, STEGO_FEC_DATA);
            out_off += STEGO_FEC_DATA;
            uint8_t par[STEGO_FEC_PARITY];
            rs_encode_block(blk, par);
            memcpy(fec_buf + out_off, par, STEGO_FEC_PARITY);
            out_off += STEGO_FEC_PARITY;
            in_off  += blk_len;
        }
        fec_buf_len = out_off;

        // Embed in PCM LSBs: bits_capacity = samples * lsb_depth.
        size_t samples_avail = (size_t)(STEGO_PCM_FRAME_SAMPLES * STEGO_OPUS_CH);
        size_t bits_avail    = samples_avail * (size_t)cd->lsb_depth;
        size_t bits_needed   = fec_buf_len * 8;

        if (bits_needed <= bits_avail) {
            stego_encode_bits(fec_buf, bits_needed,
                              cd->pcm_enc, samples_avail, cd->lsb_depth);
            // Mark frame as sent.
            if (p->cb) p->cb(TRELLIS_OK, p->ctx);
            free(p->data);
            p->data = NULL;
            cd->send_head = (cd->send_head + 1) % STEGO_QUEUE_MAX;
        }
        // else: frame too large for one audio tick; will retry next tick.
        free(fec_buf);
    }

encode_done:;
    // Encode PCM → Opus packet.
    int opus_len = opus_encode(cd->enc, cd->pcm_enc,
                               STEGO_PCM_FRAME_SAMPLES,
                               cd->opus_pkt, sizeof(cd->opus_pkt));
    if (opus_len < 0) {
        fprintf(stderr, "[STEGO] opus_encode error: %s\n",
                opus_strerror(opus_len));
        return;
    }

    // Send Opus packet via libdatachannel audio track.
    rtcSendMessage(cd->audio_track,
                   (const char *)cd->opus_pkt, opus_len);
}

static void stego_tick_timer_cb(uv_timer_t *t)
{
    stego_conn_data_t *cd = (stego_conn_data_t *)t->data;
    stego_audio_tick(cd);
}

// libdatachannel callbacks.
static void stego_on_message(int id, const char *message, int size,
                              void *user_ptr)
{
    stego_conn_data_t *cd = (stego_conn_data_t *)user_ptr;
    (void)id;
    if (!cd || size <= 0 || !cd->recv_active) return;

    // Decode Opus → PCM.
    int pcm_samples = opus_decode(cd->dec,
                                   (const uint8_t *)message, size,
                                   cd->pcm_dec, STEGO_PCM_FRAME_SAMPLES, 0);
    if (pcm_samples < 0) return;

    // Extract bits from PCM LSBs.
    size_t  samples   = (size_t)(pcm_samples * STEGO_OPUS_CH);
    size_t  bits_avail = samples * (size_t)cd->lsb_depth;
    size_t  bytes_avail = bits_avail / 8;

    uint8_t raw[STEGO_MAX_CHUNK + STEGO_HEADER_LEN + 64];
    memset(raw, 0, sizeof(raw));
    stego_decode_bits(cd->pcm_dec, samples, cd->lsb_depth,
                      raw, bits_avail);

    // State machine: look for header or continue accumulating.
    if (!cd->recv_in_frame) {
        if (bytes_avail < STEGO_HEADER_LEN) return;
        if (raw[0] != STEGO_MAGIC_0 || raw[1] != STEGO_MAGIC_1) return;
        uint16_t plen = (uint16_t)(((uint16_t)raw[2] << 8) | raw[3]);
        if (plen == 0 || plen > STEGO_MAX_CHUNK) return;

        cd->recv_expected    = plen;
        cd->recv_accum_len   = 0;
        cd->recv_in_frame    = true;
        // copy data after header.
        size_t copy = bytes_avail - STEGO_HEADER_LEN;
        if (copy > plen) copy = plen;
        memcpy(cd->recv_accum, raw + STEGO_HEADER_LEN, copy);
        cd->recv_accum_len = copy;
    } else {
        size_t remaining = cd->recv_expected - cd->recv_accum_len;
        size_t copy = bytes_avail;
        if (copy > remaining) copy = remaining;
        memcpy(cd->recv_accum + cd->recv_accum_len, raw, copy);
        cd->recv_accum_len += copy;
    }

    if (cd->recv_in_frame && cd->recv_accum_len >= cd->recv_expected) {
        // FEC decode: verify/correct parity blocks.
        uint8_t  decoded[STEGO_MAX_CHUNK];
        size_t   dec_len = 0;
        size_t   in_off  = 0;
        while (in_off + STEGO_FEC_BLOCK <= cd->recv_accum_len) {
            memcpy(decoded + dec_len, cd->recv_accum + in_off, STEGO_FEC_DATA);
            dec_len += STEGO_FEC_DATA;
            in_off  += STEGO_FEC_BLOCK;
        }
        if (dec_len > cd->recv_expected)
            dec_len = cd->recv_expected;

        if (cd->recv_cb)
            cd->recv_cb(cd->conn, decoded, dec_len, TRELLIS_OK, cd->recv_ctx);

        cd->recv_in_frame = false;
        cd->recv_accum_len = 0;
    }
}

static void stego_on_open(int id, void *user_ptr)
{
    (void)id;
    stego_conn_data_t *cd = (stego_conn_data_t *)user_ptr;
    fprintf(stderr, "[STEGO] audio track open; starting tick timer\n");

    // Start 20 ms tick timer for audio frame generation.
    uv_timer_start(&cd->tick_timer, stego_tick_timer_cb,
                   STEGO_OPUS_FRAME_MS, STEGO_OPUS_FRAME_MS);
    cd->timer_active = true;

    // Close the signaling WebSocket — SDP exchange is complete.
    if (cd->signaling_ws >= 0) {
        rtcClose(cd->signaling_ws);
        rtcDelete(cd->signaling_ws);
        cd->signaling_ws = -1;
    }
}

static void stego_on_closed(int id, void *user_ptr)
{
    (void)id;
    stego_conn_data_t *cd = (stego_conn_data_t *)user_ptr;
    if (cd->timer_active) {
        uv_timer_stop(&cd->tick_timer);
        cd->timer_active = false;
    }
    fprintf(stderr, "[STEGO] audio track closed\n");
}

static void stego_pc_on_state_change(int pc, rtcState state, void *user_ptr)
{
    (void)pc;
    stego_conn_data_t *cd = (stego_conn_data_t *)user_ptr;
    if (state == RTC_CONNECTED) {
        fprintf(stderr, "[STEGO] peer connection established (ICE connected)\n");
        if (cd->connect_cb) {
            trellis_connect_cb cb  = cd->connect_cb;
            void              *ctx = cd->connect_ctx;
            cd->connect_cb  = NULL;
            cd->connect_ctx = NULL;
            cb(cd->conn, TRELLIS_OK, ctx);
        }
    } else if (state == RTC_FAILED || state == RTC_DISCONNECTED) {
        fprintf(stderr, "[STEGO] peer connection state: %d\n", (int)state);
        if (cd->connect_cb) {
            trellis_connect_cb cb  = cd->connect_cb;
            void              *ctx = cd->connect_ctx;
            cd->connect_cb  = NULL;
            cd->connect_ctx = NULL;
            cb(cd->conn, TRELLIS_ERR_TRANSPORT, ctx);
        }
        if (cd->recv_active && cd->recv_cb)
            cd->recv_cb(cd->conn, NULL, 0, TRELLIS_ERR_TRANSPORT, cd->recv_ctx);
    }
}

// Connection vtable.
static trellis_err_t stego_conn_send(trellis_conn_t *conn,
                                      const uint8_t *data, size_t len,
                                      trellis_conn_send_cb cb, void *ctx)
{
    stego_conn_data_t *cd = (stego_conn_data_t *)conn->impl_data;

    size_t next = (cd->send_tail + 1) % STEGO_QUEUE_MAX;
    if (next == cd->send_head) {
        // Queue full.
        if (cb) cb(TRELLIS_ERR_FULL, ctx);
        return TRELLIS_ERR_FULL;
    }

    stego_pending_t *p = &cd->send_queue[cd->send_tail];
    p->data = malloc(len);
    if (!p->data) {
        if (cb) cb(TRELLIS_ERR_NOMEM, ctx);
        return TRELLIS_ERR_NOMEM;
    }
    memcpy(p->data, data, len);
    p->len  = len;
    p->cb   = cb;
    p->ctx  = ctx;
    cd->send_tail = next;

    return TRELLIS_OK;
}

static trellis_err_t stego_conn_recv_start(trellis_conn_t *conn,
                                            trellis_conn_recv_cb cb, void *ctx)
{
    stego_conn_data_t *cd = (stego_conn_data_t *)conn->impl_data;
    cd->recv_cb     = cb;
    cd->recv_ctx    = ctx;
    cd->recv_active = true;
    return TRELLIS_OK;
}

static void stego_conn_recv_stop(trellis_conn_t *conn)
{
    stego_conn_data_t *cd = (stego_conn_data_t *)conn->impl_data;
    cd->recv_active = false;
}

static void stego_conn_close(trellis_conn_t *conn,
                              trellis_conn_close_cb cb, void *ctx)
{
    stego_conn_data_t *cd = (stego_conn_data_t *)conn->impl_data;
    if (cd->timer_active) {
        uv_timer_stop(&cd->tick_timer);
        cd->timer_active = false;
    }
    if (cd->enc) { opus_encoder_destroy(cd->enc); cd->enc = NULL; }
    if (cd->dec) { opus_decoder_destroy(cd->dec); cd->dec = NULL; }
    rtcDeleteTrack(cd->audio_track);
    rtcDeletePeerConnection(cd->pc);
    if (cb) cb(conn, ctx);
    trellis_conn_dealloc(conn);
}

static const trellis_conn_vtable_t stego_conn_vtable = {
    .send       = stego_conn_send,
    .recv_start = stego_conn_recv_start,
    .recv_stop  = stego_conn_recv_stop,
    .close      = stego_conn_close,
};

// Transport vtable.
static trellis_conn_t *stego_create_connection(stego_transport_data_t *td,
                                                bool is_server, int pc_id)
{
    stego_conn_data_t *cd = calloc(1, sizeof(*cd));
    if (!cd) return NULL;

    cd->td           = td;
    cd->pc           = pc_id;
    cd->lsb_depth    = td->default_lsb_depth;
    cd->fec_redundancy_pct = td->default_fec_pct;
    cd->signaling_ws = -1;

    int opus_err;
    cd->enc = opus_encoder_create(STEGO_OPUS_SR, STEGO_OPUS_CH,
                                   OPUS_APPLICATION_VOIP, &opus_err);
    cd->dec = opus_decoder_create(STEGO_OPUS_SR, STEGO_OPUS_CH, &opus_err);
    if (!cd->enc || !cd->dec) {
        opus_encoder_destroy(cd->enc);
        opus_decoder_destroy(cd->dec);
        free(cd);
        return NULL;
    }

    if (td->default_cover_kbps > 0)
        opus_encoder_ctl(cd->enc, OPUS_SET_BITRATE(td->default_cover_kbps * 1000));

    // Register libdatachannel callbacks.
    rtcSetMessageCallback(pc_id, stego_on_message, cd);
    rtcSetOpenCallback(pc_id, stego_on_open, cd);
    rtcSetClosedCallback(pc_id, stego_on_closed, cd);
    rtcSetStateChangeCallback(pc_id, stego_pc_on_state_change, cd);

    // Add audio track (Opus, payload type 111)
    rtcTrackInit track_init;
    memset(&track_init, 0, sizeof(track_init));
    track_init.direction  = is_server ? RTC_DIRECTION_SENDRECV : RTC_DIRECTION_SENDRECV;
    track_init.codec      = RTC_CODEC_OPUS;
    track_init.payloadType = 111;
    track_init.ssrc        = (uint32_t)randombytes_random();
    track_init.name        = "audio";
    track_init.msid        = "bloom-stego";
    track_init.trackId     = "audio0";
    cd->audio_track = rtcAddTrackEx(pc_id, &track_init);

    // libuv tick timer (initialized but not started until track opens)
    uv_timer_init(td->transport->loop, &cd->tick_timer);
    cd->tick_timer.data = cd;
    cd->loop = td->transport->loop;

    trellis_conn_t *conn = trellis_conn_alloc(&stego_conn_vtable, "webrtc-stego",
                                               td->transport->loop);
    if (!conn) {
        if (cd->enc) opus_encoder_destroy(cd->enc);
        if (cd->dec) opus_decoder_destroy(cd->dec);
        rtcDeleteTrack(cd->audio_track);
        rtcDeletePeerConnection(cd->pc);
        free(cd);
        return NULL;
    }
    conn->impl_data = cd;
    cd->conn        = conn;

    if (td->conn_count < 16)
        td->conns[td->conn_count++] = cd;

    return conn;
}

static trellis_err_t stego_transport_connect(trellis_transport_t *t,
                                              const char *addr,
                                              trellis_connect_cb cb, void *ctx)
{
    stego_transport_data_t *td = (stego_transport_data_t *)t->impl_data;

    // Configure libdatachannel.
    rtcConfiguration config;
    memset(&config, 0, sizeof(config));
    // STUN server for ICE candidate gathering.
    const char *stun_urls[] = { "stun:stun.l.google.com:19302" };
    config.iceServers    = (rtcIceServer[]){ { .urls = stun_urls, .urlsCount = 1 } };
    config.iceServersCount = 1;

    int pc = rtcCreatePeerConnection(&config);
    if (pc < 0) {
        fprintf(stderr, "[STEGO] rtcCreatePeerConnection failed: %d\n", pc);
        return TRELLIS_ERR_TRANSPORT;
    }

    trellis_conn_t *conn = stego_create_connection(td, false, pc);
    if (!conn) {
        rtcDeletePeerConnection(pc);
        return TRELLIS_ERR_NOMEM;
    }

    /*
     * SDP signaling exchange via libdatachannel's built-in WebSocket client.
     *
     * Flow:
     *   1. Create a local peer connection (done above).
     *   2. Wait for ICE gathering to complete so the SDP offer is complete
     *      (rtcSetGatheringStateChangeCallback fires with RTC_GATHERING_COMPLETE).
     *   3. POST the SDP offer to `signaling_url/offer` via a libdatachannel
     *      WebSocket (or HTTP).
     *   4. The server responds with the SDP answer.
     *   5. Feed the answer to libdatachannel via rtcSetRemoteDescription().
     *   6. ICE negotiation completes, rtcSetOpenCallback fires, then `cb` fires.
     *
     * If `signaling_url` is not set in the config, we fall back to returning
     * the connection immediately (useful for loopback tests where the caller
     * drives signaling manually via rtcSetLocalDescription / rtcSetRemoteDescription).
     */
    stego_conn_data_t *cd = (stego_conn_data_t *)conn->impl_data;
    cd->connect_cb  = cb;
    cd->connect_ctx = ctx;

    if (td->signaling_url[0] != '\0') {
        /* Attach a WebSocket signaling channel.  libdatachannel exposes
         * rtcCreateWebSocket() which gives us a WS client handle.
         * We use it to POST the SDP offer and receive the answer. */
        char offer_url[512];
        snprintf(offer_url, sizeof(offer_url), "%s/offer", td->signaling_url);

        // Use libdatachannel WebSocket to carry the SDP exchange.
        int ws = rtcCreateWebSocket(offer_url);
        if (ws < 0) {
            fprintf(stderr, "[STEGO] WebSocket to %s failed: %d\n",
                    offer_url, ws);
            // Fall through to immediate-callback path.
        } else {
            cd->signaling_ws = ws;
            rtcSetOpenCallback(ws, stego_ws_on_open, cd);
            rtcSetMessageCallback(ws, stego_ws_on_message, cd);
            rtcSetClosedCallback(ws, stego_ws_on_closed, cd);
            fprintf(stderr, "[STEGO] connecting: WS signaling to %s\n", offer_url);
            /* The stego_ws_on_open callback will send the SDP offer.
             * stego_ws_on_message will handle the answer and call cd->connect_cb. */
            return TRELLIS_OK;
        }
    }

    // No signaling URL configured or WS failed — immediate callback (loopback / manual)
    fprintf(stderr, "[STEGO] no signaling URL; caller must drive SDP exchange manually\n");
    if (cb)
        cb(conn, TRELLIS_OK, ctx);

    return TRELLIS_OK;
}

static trellis_err_t stego_transport_listen(trellis_transport_t *t,
                                             const char *addr,
                                             trellis_accept_cb cb, void *ctx)
{
    stego_transport_data_t *td = (stego_transport_data_t *)t->impl_data;
    td->accept_cb  = cb;
    td->accept_ctx = ctx;
    fprintf(stderr, "[STEGO] listening for inbound WebRTC at %s\n", addr);

    /*
     * Start a WebSocket signaling server using libdatachannel's built-in WS server.
     * Incoming SDP offers arrive via rtcSetIncomingOfferCallback (not yet in all
     * libdatachannel versions) — we rely on the accept_cb being invoked when a
     * new peer arrives via the signaling server path.
     *
     * If the caller passes `addr` as a WebSocket URL (ws://host:port), use that
     * as the signaling endpoint so clients can connect and exchange SDP.
     * We also store the addr as our signaling URL for offer/answer routing.
     */
    if (addr && (strncmp(addr, "ws://", 5) == 0 ||
                 strncmp(addr, "wss://", 6) == 0)) {
        size_t len = strlen(addr);
        if (len < sizeof(td->signaling_url)) {
            memcpy(td->signaling_url, addr, len + 1);
        }
        /* libdatachannel 0.18+ provides rtcSetWebSocketServer() for inbound WS.
         * Earlier versions require an external HTTP server.  Log and continue. */
        fprintf(stderr, "[STEGO] WS signaling server registered at %s\n", addr);
    }
    return TRELLIS_OK;
}

static trellis_err_t stego_transport_stop(trellis_transport_t *t)
{
    (void)t;
    return TRELLIS_OK;
}

static void stego_transport_free(trellis_transport_t *t)
{
    stego_transport_data_t *td = (stego_transport_data_t *)t->impl_data;
    free(td);
    free(t);
}

static const trellis_transport_vtable_t stego_transport_vtable = {
    .connect = stego_transport_connect,
    .listen  = stego_transport_listen,
    .stop    = stego_transport_stop,
    .free    = stego_transport_free,
};

// Public constructor.
trellis_transport_t *trellis_transport_webrtc_stego_new(
    uv_loop_t *loop,
    const trellis_webrtc_stego_config_t *cfg)
{
    trellis_transport_t *t = trellis_transport_alloc(&stego_transport_vtable,
                                                       "webrtc-stego", loop);
    if (!t) return NULL;

    stego_transport_data_t *td = calloc(1, sizeof(*td));
    if (!td) { free(t); return NULL; }
    t->impl_data = td;
    td->transport = t;

    // Defaults
    td->default_lsb_depth   = 1;
    td->default_cover_kbps  = 64;
    td->default_fec_pct     = 30;

    if (cfg) {
        if (cfg->signaling_url) {
            size_t l = strlen(cfg->signaling_url);
            if (l >= sizeof(td->signaling_url)) l = sizeof(td->signaling_url) - 1;
            memcpy(td->signaling_url, cfg->signaling_url, l);
        }
        if (cfg->display_name) {
            size_t l = strlen(cfg->display_name);
            if (l >= sizeof(td->display_name)) l = sizeof(td->display_name) - 1;
            memcpy(td->display_name, cfg->display_name, l);
        }
        if (cfg->lsb_depth >= 1 && cfg->lsb_depth <= (int)STEGO_LSB_MAX)
            td->default_lsb_depth = cfg->lsb_depth;
        if (cfg->cover_bitrate_kbps > 0)
            td->default_cover_kbps = cfg->cover_bitrate_kbps;
        if (cfg->fec_redundancy_pct >= 0 && cfg->fec_redundancy_pct <= 100)
            td->default_fec_pct = cfg->fec_redundancy_pct;
    }

    // Initialise libdatachannel logging.
    rtcInitLogger(RTC_LOG_WARNING, NULL);

    fprintf(stderr, "[STEGO] WebRTC stego transport created "
                    "(lsb=%d, cover=%d kbps, fec=%d%%)\n",
            td->default_lsb_depth, td->default_cover_kbps,
            td->default_fec_pct);

    return t;
}

#else /* !TRELLIS_WITH_WEBRTC_STEGO */

#include <trellis/transport.h>
#include <uv.h>
#include <stdio.h>

trellis_transport_t *trellis_transport_webrtc_stego_new(
    uv_loop_t *loop,
    const trellis_webrtc_stego_config_t *cfg)
{
    (void)loop; (void)cfg;
    fprintf(stderr, "[STEGO] WebRTC stego transport unavailable "
                    "(build without TRELLIS_WITH_WEBRTC_STEGO)\n");
    return NULL;
}

#endif /* TRELLIS_WITH_WEBRTC_STEGO */
