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

#include <trellis/camouflage.h>
#include <sodium.h>
#include <stdlib.h>
#include <string.h>

/*
 * Protocol camouflage engine (DPI Defense Layer 0).
 *
 * Each disguise wraps Trellis frames in a format that mimics an allowed
 * protocol.  Disguises maintain per-connection state machines so the
 * connection lifecycle (handshake → data) matches the real protocol,
 * not just individual frames.
 */

/* HACK: coturn expects the SNI in a specific format when proxying;
   if you change this pool make sure to test against it */
static const char *mimic_sni_pool[] = {
    "www.google.com",
    "cdn.cloudflare.com",
    "api.github.com",
    "assets.amazonaws.com",
    "static.fastly.net",
    "fonts.googleapis.com",
};
#define MIMIC_SNI_COUNT (sizeof(mimic_sni_pool) / sizeof(mimic_sni_pool[0]))

static const char *random_sni(void)
{
    return mimic_sni_pool[randombytes_uniform(MIMIC_SNI_COUNT)];
}

// --- TLS 1.3 Application Data mimic ---
typedef enum tls13_phase { TLS13_HELLO = 0, TLS13_DATA = 1 } tls13_phase_t;

typedef struct tls13_state {
    uint64_t      seq;
    tls13_phase_t phase;
    bool          is_initiator;
} tls13_state_t;

/*
 * Builds a plausible TLS 1.3 ClientHello record.
 * The content is realistic enough to pass regex-based passive DPI.
 */
static trellis_err_t tls13_build_client_hello(trellis_buf_t *out,
                                               const char *sni)
{
    // Randomise the 32-byte random field and session_id.
    uint8_t tls_random[32];
    uint8_t session_id[32];
    randombytes_buf(tls_random, sizeof(tls_random));
    randombytes_buf(session_id, sizeof(session_id));

    size_t sni_len = sni ? strlen(sni) : 0;

    /*
     * ClientHello extensions (minimal but convincing):
     *   supported_versions: TLS 1.3 (0x0304)
     *   supported_groups:   x25519 (0x001d), secp256r1 (0x0017)
     *   signature_algorithms: ecdsa_secp256r1_sha256 (0x0403)
     *   ALPN: h2
     *   SNI: the supplied hostname
     */
    uint8_t exts[256];
    size_t ep = 0;

    // supported_versions
    exts[ep++] = 0x00; exts[ep++] = 0x2b; /* type */
    exts[ep++] = 0x00; exts[ep++] = 0x03; /* length */
    exts[ep++] = 0x02;                     /* list length */
    exts[ep++] = 0x03; exts[ep++] = 0x04; /* TLS 1.3 */

    // supported_groups
    exts[ep++] = 0x00; exts[ep++] = 0x0a;
    exts[ep++] = 0x00; exts[ep++] = 0x06;
    exts[ep++] = 0x00; exts[ep++] = 0x04;
    exts[ep++] = 0x00; exts[ep++] = 0x1d; /* x25519 */
    exts[ep++] = 0x00; exts[ep++] = 0x17; /* secp256r1 */

    // signature_algorithms
    exts[ep++] = 0x00; exts[ep++] = 0x0d;
    exts[ep++] = 0x00; exts[ep++] = 0x04;
    exts[ep++] = 0x00; exts[ep++] = 0x02;
    exts[ep++] = 0x04; exts[ep++] = 0x03; /* ecdsa_secp256r1_sha256 */

    // ALPN: h2.
    exts[ep++] = 0x00; exts[ep++] = 0x10;
    exts[ep++] = 0x00; exts[ep++] = 0x05;
    exts[ep++] = 0x00; exts[ep++] = 0x03;
    exts[ep++] = 0x02; exts[ep++] = 'h'; exts[ep++] = '2';

    // SNI
    if (sni_len > 0 && sni_len < 128) {
        exts[ep++] = 0x00; exts[ep++] = 0x00; /* SNI type */
        uint16_t sni_ext_len = (uint16_t)(5 + sni_len);
        exts[ep++] = (uint8_t)(sni_ext_len >> 8);
        exts[ep++] = (uint8_t)(sni_ext_len);
        uint16_t list_len = (uint16_t)(3 + sni_len);
        exts[ep++] = (uint8_t)(list_len >> 8);
        exts[ep++] = (uint8_t)(list_len);
        exts[ep++] = 0x00; /* host_name type */
        exts[ep++] = (uint8_t)(sni_len >> 8);
        exts[ep++] = (uint8_t)(sni_len);
        memcpy(exts + ep, sni, sni_len);
        ep += sni_len;
    }

    // ClientHello body.
    size_t ch_body_len = 2 + 32 + 1 + 32 + 2 + 2 + 2 + 1 + 2 + ep;
    uint8_t cipher_suites[4] = { 0x13, 0x01, 0x13, 0x03 }; /* AES_128_GCM_SHA256, CHACHA20 */

    size_t hs_len = 4 + ch_body_len;
    size_t record_len = hs_len;

    *out = trellis_buf_alloc(5 + record_len);
    if (!out->data) return TRELLIS_ERR_NOMEM;

    uint8_t *p = out->data;
    // TLS record header: Handshake (0x16), TLS 1.0 compat (0x03 0x01)
    *p++ = 0x16; *p++ = 0x03; *p++ = 0x01;
    *p++ = (uint8_t)(record_len >> 8);
    *p++ = (uint8_t)(record_len);

    // Handshake header: ClientHello (1)
    *p++ = 0x01;
    *p++ = (uint8_t)(ch_body_len >> 16);
    *p++ = (uint8_t)(ch_body_len >> 8);
    *p++ = (uint8_t)(ch_body_len);

    // client_version: TLS 1.2 compat.
    *p++ = 0x03; *p++ = 0x03;

    memcpy(p, tls_random, 32); p += 32;

    // session_id (32 bytes)
    *p++ = 32;
    memcpy(p, session_id, 32); p += 32;

    // cipher suites.
    *p++ = 0x00; *p++ = (uint8_t)sizeof(cipher_suites);
    memcpy(p, cipher_suites, sizeof(cipher_suites)); p += sizeof(cipher_suites);

    // compression methods.
    *p++ = 0x01; *p++ = 0x00;

    // extensions length.
    *p++ = (uint8_t)(ep >> 8); *p++ = (uint8_t)(ep);
    memcpy(p, exts, ep); p += ep;

    out->len = (size_t)(p - out->data);
    return TRELLIS_OK;
}

/*
 * Build a fake TLS 1.3 server response: ChangeCipherSpec + one
 * EncryptedExtensions Application Data record.
 */
static trellis_err_t tls13_build_server_hello(trellis_buf_t *out)
{
    uint8_t srv_random[32];
    randombytes_buf(srv_random, sizeof(srv_random));

    // ServerHello record.
    uint8_t sh_body[128];
    size_t sp = 0;
    sh_body[sp++] = 0x03; sh_body[sp++] = 0x03; /* version */
    memcpy(sh_body + sp, srv_random, 32); sp += 32;
    sh_body[sp++] = 0x00; /* session_id length */
    sh_body[sp++] = 0x13; sh_body[sp++] = 0x01; /* cipher TLS_AES_128_GCM_SHA256 */
    sh_body[sp++] = 0x00; /* compression */
    // supported_versions extension = TLS1.3.
    uint8_t sv_ext[6] = { 0x00, 0x2b, 0x00, 0x02, 0x03, 0x04 };
    sh_body[sp++] = 0x00; sh_body[sp++] = (uint8_t)sizeof(sv_ext);
    memcpy(sh_body + sp, sv_ext, sizeof(sv_ext)); sp += sizeof(sv_ext);

    size_t sh_hs_len = 4 + sp;
    size_t sh_rec_len = sh_hs_len;

    // ChangeCipherSpec record (1 byte payload = 0x01)
    size_t ccs_len = 5 + 1;
    // Fake EncryptedExtensions as Application Data.
    uint8_t ee_payload[32];
    randombytes_buf(ee_payload, sizeof(ee_payload));
    size_t ee_rec_len = 5 + sizeof(ee_payload);

    size_t total = 5 + sh_rec_len + ccs_len + ee_rec_len;
    *out = trellis_buf_alloc(total);
    if (!out->data) return TRELLIS_ERR_NOMEM;

    uint8_t *p = out->data;

    // ServerHello record.
    *p++ = 0x16; *p++ = 0x03; *p++ = 0x03;
    *p++ = (uint8_t)(sh_rec_len >> 8); *p++ = (uint8_t)(sh_rec_len);
    *p++ = 0x02; /* ServerHello */
    *p++ = (uint8_t)(sp >> 16); *p++ = (uint8_t)(sp >> 8); *p++ = (uint8_t)(sp);
    memcpy(p, sh_body, sp); p += sp;

    // ChangeCipherSpec
    *p++ = 0x14; *p++ = 0x03; *p++ = 0x03; *p++ = 0x00; *p++ = 0x01;
    *p++ = 0x01;

    // Fake EncryptedExtensions as Application Data record.
    *p++ = 0x17; *p++ = 0x03; *p++ = 0x03;
    *p++ = (uint8_t)(sizeof(ee_payload) >> 8);
    *p++ = (uint8_t)(sizeof(ee_payload));
    memcpy(p, ee_payload, sizeof(ee_payload)); p += sizeof(ee_payload);

    out->len = (size_t)(p - out->data);
    return TRELLIS_OK;
}

static trellis_err_t tls13_wrap(const uint8_t *in, size_t in_len,
                                trellis_buf_t *out, void *state)
{
    tls13_state_t *st = state;

    if (st->phase == TLS13_HELLO) {
        // Emit protocol handshake records first, then the data record.
        trellis_buf_t hs = {0};
        trellis_err_t err;
        if (st->is_initiator)
            err = tls13_build_client_hello(&hs, random_sni());
        else
            err = tls13_build_server_hello(&hs);

        if (err != TRELLIS_OK)
            return err;

        // Padding inside the length field (TLS 1.3 style)
        uint32_t pad = randombytes_uniform(32);
        size_t record_len = in_len + pad + 1; /* +1 for inner content type */
        if (record_len > 16384) record_len = in_len + 1;

        size_t total = hs.len + 5 + record_len;
        *out = trellis_buf_alloc(total);
        if (!out->data) { trellis_buf_free(&hs); return TRELLIS_ERR_NOMEM; }

        memcpy(out->data, hs.data, hs.len);
        trellis_buf_free(&hs);

        uint8_t *p = out->data + (total - 5 - record_len);
        *p++ = 0x17; *p++ = 0x03; *p++ = 0x03;
        *p++ = (uint8_t)(record_len >> 8); *p++ = (uint8_t)(record_len);
        memcpy(p, in, in_len); p += in_len;
        *p++ = 0x17; /* inner content type: Application Data */
        if (record_len > in_len + 1)
            memset(p, 0, record_len - in_len - 1);

        out->len = total;
        st->phase = TLS13_DATA;
        st->seq++;
        return TRELLIS_OK;
    }

    // DATA phase: pure Application Data records.
    uint32_t pad = randombytes_uniform(32);
    size_t record_len = in_len + pad + 1;
    if (record_len > 16384) record_len = in_len + 1;

    *out = trellis_buf_alloc(5 + record_len);
    if (!out->data) return TRELLIS_ERR_NOMEM;

    out->data[0] = 0x17; out->data[1] = 0x03; out->data[2] = 0x03;
    out->data[3] = (uint8_t)(record_len >> 8);
    out->data[4] = (uint8_t)(record_len);
    memcpy(out->data + 5, in, in_len);
    out->data[5 + in_len] = 0x17; /* inner content type */
    if (record_len > in_len + 1)
        memset(out->data + 5 + in_len + 1, 0, record_len - in_len - 1);
    out->len = 5 + record_len;

    st->seq++;
    return TRELLIS_OK;
}

static trellis_err_t tls13_unwrap(const uint8_t *in, size_t in_len,
                                  trellis_buf_t *out, void *state)
{
    tls13_state_t *st = state;
    const uint8_t *p = in;
    size_t left = in_len;

    // Skip handshake and ChangeCipherSpec records until we hit AppData.
    while (left >= 5) {
        uint8_t rec_type = p[0];
        uint16_t rec_len = ((uint16_t)p[3] << 8) | p[4];

        if (5 + (size_t)rec_len > left)
            return TRELLIS_ERR_DECODE;

        if (rec_type == 0x17) { /* Application Data */
            st->phase = TLS13_DATA;
            // Payload minus the inner content-type byte and optional padding.
            if (rec_len < 1)
                return TRELLIS_ERR_DECODE;
            /* RFC 8446 §5.4: scan backwards past zero-valued padding bytes
             * to find the inner content-type byte, then strip it.  The actual
             * plaintext precedes the content-type byte. */
            const uint8_t *payload = p + 5;
            size_t pos = rec_len;
            while (pos > 0 && payload[pos - 1] == 0)
                pos--;
            if (pos == 0)
                return TRELLIS_ERR_DECODE;
            pos--; /* skip the inner content-type byte itself */
            size_t data_len = pos;

            *out = trellis_buf_from(payload, data_len);
            if (!out->data && data_len > 0) return TRELLIS_ERR_NOMEM;
            return TRELLIS_OK;
        }

        p += 5 + rec_len;
        left -= 5 + rec_len;
    }

    return TRELLIS_ERR_DECODE;
}

static void tls13_free(void *state) { free(state); }

static const trellis_camouflage_vtable_t tls13_vtable = {
    .wrap          = tls13_wrap,
    .unwrap        = tls13_unwrap,
    .free_state    = tls13_free,
    .protocol_name = "tls13-mimic",
};

// --- HTTP/2 DATA frame mimic ---
typedef enum http2_phase {
    HTTP2_PREFACE  = 0,
    HTTP2_SETTINGS = 1,
    HTTP2_DATA     = 2,
} http2_phase_t;

/* Number of DATA frames to send on one stream before opening a new one.
 * Mirrors realistic long-poll / streaming POST behaviour. */
#define HTTP2_FRAMES_PER_STREAM 20

typedef struct http2_state {
    uint32_t    stream_id;
    uint32_t    next_stream_id;
    uint32_t    frames_on_current_stream; /* DATA frames sent on active stream */
    bool        preface_sent;
    bool        is_initiator;
    http2_phase_t phase;
} http2_state_t;

// Write a 9-byte HTTP/2 frame header into buf. Returns buf + 9.
static uint8_t *http2_frame_hdr(uint8_t *buf, uint32_t length,
                                uint8_t type, uint8_t flags, uint32_t stream_id)
{
    buf[0] = (uint8_t)(length >> 16);
    buf[1] = (uint8_t)(length >> 8);
    buf[2] = (uint8_t)(length);
    buf[3] = type;
    buf[4] = flags;
    buf[5] = (uint8_t)((stream_id >> 24) & 0x7F);
    buf[6] = (uint8_t)(stream_id >> 16);
    buf[7] = (uint8_t)(stream_id >> 8);
    buf[8] = (uint8_t)(stream_id);
    return buf + 9;
}

// Minimal HPACK literal header block: POST / HTTP/2 with authority.
static size_t http2_build_headers_block(uint8_t *buf, size_t cap,
                                        const char *authority)
{
    size_t p = 0;
    // :method POST - indexed (3)
    if (p < cap) buf[p++] = 0x83;
    // :scheme https - indexed (7)
    if (p < cap) buf[p++] = 0x87;
    // :path / - indexed (4)
    if (p < cap) buf[p++] = 0x84;
    // :authority - literal.
    if (authority && p + 3 + strlen(authority) < cap) {
        buf[p++] = 0x41; /* literal with incremental indexing, index 1 */
        size_t alen = strlen(authority);
        buf[p++] = (uint8_t)alen;
        memcpy(buf + p, authority, alen);
        p += alen;
    }
    return p;
}

static trellis_err_t http2_wrap(const uint8_t *in, size_t in_len,
                                trellis_buf_t *out, void *state)
{
    http2_state_t *st = state;
    const char *sni = random_sni();

    if (st->phase == HTTP2_PREFACE) {
        /*
         * Connection preface + SETTINGS + HEADERS + DATA in one buffer.
         * Both sides send the preface and SETTINGS.
         */
        uint8_t settings_payload[18] = {
            // HEADER_TABLE_SIZE = 4096
            0x00, 0x01, 0x00, 0x00, 0x10, 0x00,
            // MAX_CONCURRENT_STREAMS = 100
            0x00, 0x03, 0x00, 0x00, 0x00, 0x64,
            // INITIAL_WINDOW_SIZE = 65535
            0x00, 0x04, 0x00, 0x00, 0xFF, 0xFF,
        };

        uint8_t hdr_block[256];
        size_t hdr_len = http2_build_headers_block(hdr_block, sizeof(hdr_block), sni);

        size_t preface_len = st->is_initiator ? 24 : 0;
        // SETTINGS frame: 9-byte hdr + 18-byte payload.
        size_t settings_frame_len = 9 + sizeof(settings_payload);
        // HEADERS frame.
        size_t headers_frame_len = 9 + hdr_len;
        // DATA frame.
        size_t data_frame_len = 9 + in_len;

        size_t total = preface_len + settings_frame_len + headers_frame_len + data_frame_len;
        *out = trellis_buf_alloc(total);
        if (!out->data) return TRELLIS_ERR_NOMEM;

        uint8_t *p = out->data;
        if (st->is_initiator) {
            memcpy(p, "PRI * HTTP/2.0\r\n\r\nSM\r\n\r\n", 24);
            p += 24;
        }

        p = http2_frame_hdr(p, sizeof(settings_payload), 0x04, 0x00, 0);
        memcpy(p, settings_payload, sizeof(settings_payload)); p += sizeof(settings_payload);

        p = http2_frame_hdr(p, (uint32_t)hdr_len, 0x01, 0x04 /* END_HEADERS */, st->next_stream_id);
        memcpy(p, hdr_block, hdr_len); p += hdr_len;

        p = http2_frame_hdr(p, (uint32_t)in_len, 0x00, 0x00, st->next_stream_id);
        memcpy(p, in, in_len); p += in_len;

        out->len = (size_t)(p - out->data);
        st->stream_id = st->next_stream_id;
        st->next_stream_id += 2;
        st->frames_on_current_stream = 1;
        st->phase = HTTP2_DATA;
        st->preface_sent = true;
        return TRELLIS_OK;
    }

    /* DATA phase.
     *
     * Real HTTP/2 connections multiplex many DATA frames on the same stream
     * before ending it.  Opening a new stream for every frame is a DPI
     * fingerprint.  Reuse the active stream for HTTP2_FRAMES_PER_STREAM
     * frames, then open a new one (simulating a new logical POST request). */
    bool need_new_stream = (st->frames_on_current_stream >= HTTP2_FRAMES_PER_STREAM);

    size_t hdr_overhead = 0;
    uint8_t hdr_block_data[256];
    size_t hdr_len_data = 0;

    if (need_new_stream) {
        hdr_len_data = http2_build_headers_block(hdr_block_data, sizeof(hdr_block_data), sni);
        hdr_overhead = 9 + hdr_len_data;
    }

    size_t total = hdr_overhead + 9 + in_len;
    *out = trellis_buf_alloc(total);
    if (!out->data) return TRELLIS_ERR_NOMEM;

    uint8_t *p = out->data;

    if (need_new_stream) {
        st->stream_id = st->next_stream_id;
        st->next_stream_id += 2;
        st->frames_on_current_stream = 0;
        p = http2_frame_hdr(p, (uint32_t)hdr_len_data, 0x01, 0x04, st->stream_id);
        memcpy(p, hdr_block_data, hdr_len_data); p += hdr_len_data;
    }

    p = http2_frame_hdr(p, (uint32_t)in_len, 0x00, 0x00, st->stream_id);
    memcpy(p, in, in_len); p += in_len;

    out->len = (size_t)(p - out->data);
    st->frames_on_current_stream++;
    return TRELLIS_OK;
}

static trellis_err_t http2_unwrap(const uint8_t *in, size_t in_len,
                                  trellis_buf_t *out, void *state)
{
    http2_state_t *st = state;
    const uint8_t *p = in;
    size_t remaining = in_len;

    // Skip preface.
    if (!st->preface_sent && remaining >= 24 &&
        memcmp(p, "PRI * HTTP/2.0\r\n\r\nSM\r\n\r\n", 24) == 0) {
        p += 24; remaining -= 24;
        st->preface_sent = true;
    }

    // Scan frames to find DATA (type 0x00)
    while (remaining >= 9) {
        uint32_t frame_len = ((uint32_t)p[0] << 16) |
                             ((uint32_t)p[1] << 8) | p[2];
        uint8_t frame_type = p[3];
        // uint8_t flags = p[4];
        // uint32_t stream_id = ... p[5..8]
        if (9 + (size_t)frame_len > remaining)
            return TRELLIS_ERR_DECODE;

        if (frame_type == 0x00) { /* DATA */
            *out = trellis_buf_from(p + 9, frame_len);
            if (!out->data && frame_len > 0) return TRELLIS_ERR_NOMEM;
            st->phase = HTTP2_DATA;
            return TRELLIS_OK;
        }

        p += 9 + frame_len;
        remaining -= 9 + frame_len;
    }

    return TRELLIS_ERR_DECODE;
}

static void http2_free(void *state) { free(state); }

static const trellis_camouflage_vtable_t http2_vtable = {
    .wrap          = http2_wrap,
    .unwrap        = http2_unwrap,
    .free_state    = http2_free,
    .protocol_name = "http2-mimic",
};

// --- DNS TXT response mimic ---
#define DNS_QUERY_QUEUE 16

typedef struct dns_state {
    uint16_t tx_id;
    uint16_t pending_ids[DNS_QUERY_QUEUE];
    size_t   pending_count;
    bool     is_initiator;
} dns_state_t;

static const char *dns_fake_labels[] = {
    "_relay", "_node", "_bridge", "_peer", "_gate",
};
#define DNS_LABEL_COUNT (sizeof(dns_fake_labels)/sizeof(dns_fake_labels[0]))


/*
 * Encode a domain name (e.g. "_relay01ab.trellis.invalid") into DNS wire
 * format.  Returns the number of bytes written to buf.
 */
static size_t dns_encode_name(uint8_t *buf, size_t cap, const char *name)
{
    size_t p = 0;
    const char *tok = name;
    const char *dot;
    while ((dot = strchr(tok, '.')) != NULL) {
        size_t llen = (size_t)(dot - tok);
        if (p + 1 + llen >= cap) return 0;
        buf[p++] = (uint8_t)llen;
        memcpy(buf + p, tok, llen); p += llen;
        tok = dot + 1;
    }
    size_t last = strlen(tok);
    if (p + 1 + last + 1 >= cap) return 0;
    buf[p++] = (uint8_t)last;
    memcpy(buf + p, tok, last); p += last;
    buf[p++] = 0x00;
    return p;
}

static trellis_err_t dns_wrap(const uint8_t *in, size_t in_len,
                              trellis_buf_t *out, void *state)
{
    dns_state_t *st = state;

    // Build a query name and emit a query before the response (initiator)
    uint8_t query_buf[128];
    size_t query_len = 0;

    // Generate the QNAME string so the response can include it verbatim.
    const char *lbl = dns_fake_labels[randombytes_uniform(DNS_LABEL_COUNT)];
    uint8_t rnd[4];
    randombytes_buf(rnd, sizeof(rnd));
    char qname_str[64];
    snprintf(qname_str, sizeof(qname_str), "%s%02x%02x%02x%02x.trellis.invalid",
             lbl, rnd[0], rnd[1], rnd[2], rnd[3]);

    // Encode qname into wire format (used in both query and response)
    uint8_t qname_wire[64];
    size_t qname_wire_len = dns_encode_name(qname_wire, sizeof(qname_wire), qname_str);
    if (qname_wire_len == 0) return TRELLIS_ERR_DECODE;

    if (st->is_initiator) {
        // Re-use dns_build_query logic but with our pre-chosen name.
        uint16_t tx = st->tx_id++;
        if (st->pending_count < DNS_QUERY_QUEUE)
            st->pending_ids[st->pending_count++] = tx;

        size_t qp = 0;
        query_buf[qp++] = (uint8_t)(tx >> 8); query_buf[qp++] = (uint8_t)(tx);
        query_buf[qp++] = 0x01; query_buf[qp++] = 0x00;
        query_buf[qp++] = 0x00; query_buf[qp++] = 0x01;
        query_buf[qp++] = 0x00; query_buf[qp++] = 0x00;
        query_buf[qp++] = 0x00; query_buf[qp++] = 0x00;
        query_buf[qp++] = 0x00; query_buf[qp++] = 0x00;
        if (qp + qname_wire_len + 4 <= sizeof(query_buf)) {
            memcpy(query_buf + qp, qname_wire, qname_wire_len); qp += qname_wire_len;
            query_buf[qp++] = 0x00; query_buf[qp++] = 0x10; /* QTYPE=TXT */
            query_buf[qp++] = 0x00; query_buf[qp++] = 0x01; /* QCLASS=IN */
        }
        query_len = qp;
    }

    uint16_t resp_tx = (st->pending_count > 0)
                       ? st->pending_ids[--st->pending_count]
                       : st->tx_id++;

    // Vary TTL: 30 to 629 seconds.
    uint32_t ttl = 30 + randombytes_uniform(600);

    size_t dns_hdr = 12;
    size_t ans_hdr = 2 + 2 + 2 + 4 + 2; /* NAME + TYPE + CLASS + TTL + RDLENGTH */

    /*
     * Question section: real wire-format QNAME + QTYPE + QCLASS.
     * Using 0xC00C (pointer to offset 12) here would be invalid because offset
     * 12 inside this response message is the question QNAME itself — not a
     * previously encoded label.  Write the full wire label instead.
     */
    size_t question_section_len = qname_wire_len + 4; /* QNAME + QTYPE + QCLASS */

    /*
     * Answer NAME: use a compression pointer back to the Question's QNAME
     * which starts at byte 12 of the response (immediately after the header).
     */
    size_t txt_chunks = (in_len + 254) / 255;
    size_t txt_rdata_len = in_len + txt_chunks;
    size_t dns_body = dns_hdr + question_section_len + ans_hdr + txt_rdata_len;
    size_t query_frame = query_len > 0 ? 2 + query_len : 0;
    size_t resp_frame = 2 + dns_body;

    *out = trellis_buf_alloc(query_frame + resp_frame);
    if (!out->data) return TRELLIS_ERR_NOMEM;

    uint8_t *p = out->data;

    if (query_len > 0) {
        p[0] = (uint8_t)(query_len >> 8); p[1] = (uint8_t)(query_len);
        p += 2;
        memcpy(p, query_buf, query_len); p += query_len;
    }

    // DNS-over-TCP length prefix for response.
    p[0] = (uint8_t)(dns_body >> 8); p[1] = (uint8_t)(dns_body); p += 2;

    // Response DNS header.
    p[0] = (uint8_t)(resp_tx >> 8); p[1] = (uint8_t)(resp_tx);
    p[2] = 0x81; p[3] = 0x80; /* QR=1, AA=0, RD=1, RA=1 */
    p[4] = 0x00; p[5] = 0x01; /* QDCOUNT=1 */
    p[6] = 0x00; p[7] = 0x01; /* ANCOUNT=1 */
    p[8] = 0x00; p[9] = 0x00; /* NSCOUNT */
    p[10] = 0x00; p[11] = 0x00; /* ARCOUNT */
    p += dns_hdr;

    // Question section: full wire-format QNAME (not a back-pointer)
    memcpy(p, qname_wire, qname_wire_len); p += qname_wire_len;
    p[0] = 0x00; p[1] = 0x10; /* QTYPE=TXT */
    p[2] = 0x00; p[3] = 0x01; /* QCLASS=IN */
    p += 4;

    // Answer RR: NAME is a compression pointer back to offset 12 (start of question QNAME)
    p[0] = 0xC0; p[1] = 0x0C; /* pointer to byte 12 — now valid */
    p[2] = 0x00; p[3] = 0x10; /* TYPE=TXT */
    p[4] = 0x00; p[5] = 0x01; /* CLASS=IN */
    p[6] = (uint8_t)(ttl >> 24); p[7] = (uint8_t)(ttl >> 16);
    p[8] = (uint8_t)(ttl >> 8); p[9] = (uint8_t)(ttl);
    p[10] = (uint8_t)(txt_rdata_len >> 8);
    p[11] = (uint8_t)(txt_rdata_len);
    p += ans_hdr;

    // TXT RDATA: length-prefixed chunks.
    size_t off = 0;
    while (off < in_len) {
        size_t chunk = in_len - off;
        if (chunk > 255) chunk = 255;
        *p++ = (uint8_t)chunk;
        memcpy(p, in + off, chunk); p += chunk;
        off += chunk;
    }

    out->len = (size_t)(p - out->data);
    return TRELLIS_OK;
}

static trellis_err_t dns_unwrap(const uint8_t *in, size_t in_len,
                                trellis_buf_t *out, void *state)
{
    (void)state;
    const uint8_t *p = in;
    size_t left = in_len;

    // Skip any query frames (QR=0), stop at the first response frame (QR=1)
    while (left >= 2) {
        if (left < 2) return TRELLIS_ERR_DECODE;
        uint16_t dns_len = ((uint16_t)p[0] << 8) | p[1];
        if (2 + (size_t)dns_len > left) return TRELLIS_ERR_DECODE;

        const uint8_t *dns = p + 2;
        if (dns_len < 12) { p += 2 + dns_len; left -= 2 + dns_len; continue; }

        bool is_response = (dns[2] & 0x80) != 0;
        if (!is_response) { p += 2 + dns_len; left -= 2 + dns_len; continue; }

        // Response: skip header + question section to reach answer RR.
        size_t dns_hdr = 12;
        // Skip question section: scan for root label terminator.
        size_t q_off = dns_hdr;
        uint16_t qdcount = ((uint16_t)dns[4] << 8) | dns[5];
        if (qdcount > 0) {
            while (q_off < dns_len) {
                uint8_t label_len = dns[q_off];
                if (label_len == 0) { q_off++; break; }
                if ((label_len & 0xC0) == 0xC0) { q_off += 2; break; }
                q_off += 1 + label_len;
            }
            q_off += 4; /* QTYPE + QCLASS */
        }

        // Answer RR.
        size_t ans_hdr = 2 + 2 + 2 + 4 + 2;
        if (q_off + ans_hdr > dns_len) return TRELLIS_ERR_DECODE;

        uint16_t rdlength = ((uint16_t)dns[q_off + 10] << 8) | dns[q_off + 11];
        const uint8_t *rdata = dns + q_off + ans_hdr;
        if (q_off + ans_hdr + rdlength > dns_len) return TRELLIS_ERR_DECODE;

        // Decode TXT chunks.
        size_t total = 0;
        const uint8_t *scan = rdata;
        size_t scan_left = rdlength;
        while (scan_left > 0) {
            uint8_t cl = *scan++; scan_left--;
            if (cl > scan_left) return TRELLIS_ERR_DECODE;
            total += cl; scan += cl; scan_left -= cl;
        }

        *out = trellis_buf_alloc(total);
        if (!out->data && total > 0) return TRELLIS_ERR_NOMEM;

        size_t woff = 0;
        scan = rdata; scan_left = rdlength;
        while (scan_left > 0) {
            uint8_t cl = *scan++; scan_left--;
            memcpy(out->data + woff, scan, cl);
            woff += cl; scan += cl; scan_left -= cl;
        }
        out->len = total;
        return TRELLIS_OK;
    }

    return TRELLIS_ERR_DECODE;
}

static void dns_free(void *state) { free(state); }

static const trellis_camouflage_vtable_t dns_vtable = {
    .wrap          = dns_wrap,
    .unwrap        = dns_unwrap,
    .free_state    = dns_free,
    .protocol_name = "dns-tunnel",
};

// --- QUIC Initial packet mimic ---
typedef enum quic_phase { QUIC_INITIAL = 0, QUIC_ONE_RTT = 1 } quic_phase_t;

typedef struct quic_state {
    uint8_t    dcid[8];
    uint8_t    scid[8];
    uint32_t   pkt_num;
    uint32_t   phase_pkt_count;  /* packets sent in current phase */
    quic_phase_t phase;
    bool       is_initiator;
    // 32-byte 1-RTT key derived from the mux session key at init time.
    uint8_t    rtt1_key[32];
    bool       rtt1_key_set;
} quic_state_t;

/*
 * Derive QUIC Initial secrets per RFC 9001 §5.2:
 *   initial_salt = 0x38762cf7f55934b34d179ae6a4c80cadccbb7f0a
 *   secret = HKDF-Extract(initial_salt, dcid)
 *   client_in = HKDF-Expand-Label(secret, "client in", "", 32)
 *   server_in = HKDF-Expand-Label(secret, "server in", "", 32)
 *
 * We derive a 32-byte key + 12-byte IV using the label mechanism.
 */
static void quic_derive_initial_keys(const uint8_t *dcid, size_t dcid_len,
                                     bool is_client,
                                     uint8_t key_out[16], uint8_t iv_out[12],
                                     uint8_t hp_out[16])
{
    static const uint8_t initial_salt[20] = {
        0x38, 0x76, 0x2c, 0xf7, 0xf5, 0x59, 0x34, 0xb3, 0x4d, 0x17,
        0x9a, 0xe6, 0xa4, 0xc8, 0x0c, 0xad, 0xcc, 0xbb, 0x7f, 0x0a,
    };

    // HKDF-Extract
    uint8_t initial_secret[32];
    crypto_auth_hmacsha256_state st;
    crypto_auth_hmacsha256_init(&st, initial_salt, sizeof(initial_salt));
    crypto_auth_hmacsha256_update(&st, dcid, dcid_len);
    crypto_auth_hmacsha256_final(&st, initial_secret);

    // HKDF-Expand for client or server in (using a simplified label)
    const char *label = is_client ? "client in" : "server in";
    uint8_t label_info[64];
    size_t info_len = 0;
    label_info[info_len++] = 0x00; label_info[info_len++] = 0x20; /* length=32 */
    label_info[info_len++] = (uint8_t)(6 + strlen(label));
    memcpy(label_info + info_len, "tls13 ", 6); info_len += 6;
    size_t llen = strlen(label);
    memcpy(label_info + info_len, label, llen); info_len += llen;
    label_info[info_len++] = 0x00; /* context len */
    label_info[info_len++] = 0x01; /* HKDF counter */

    uint8_t handshake_secret[32];
    crypto_auth_hmacsha256_init(&st, initial_secret, sizeof(initial_secret));
    crypto_auth_hmacsha256_update(&st, label_info, info_len);
    crypto_auth_hmacsha256_final(&st, handshake_secret);

    memcpy(key_out, handshake_secret, 16);
    memcpy(iv_out,  handshake_secret + 16, 12);

    // Header protection key (simple derivation: hash of key)
    uint8_t hp_digest[32];
    crypto_hash_sha256(hp_digest, key_out, 16);
    memcpy(hp_out, hp_digest, 16);

    sodium_memzero(initial_secret, sizeof(initial_secret));
    sodium_memzero(handshake_secret, sizeof(handshake_secret));
    sodium_memzero(hp_digest, sizeof(hp_digest));
    sodium_memzero(&st, sizeof(st));
}

/*
 * Apply QUIC header protection mask to the first byte and packet number.
 * Uses AES-128-ECB on the sample (simplified: we use BLAKE2b as a
 * substitute since libsodium does not expose raw AES-ECB).
 */
static void quic_apply_header_protection(uint8_t *first_byte,
                                          uint8_t *pkt_num_bytes,
                                          size_t pkt_num_len,
                                          const uint8_t *hp_key,
                                          const uint8_t *sample)
{
    uint8_t mask[5];
    // Use crypto_generichash as a keyed PRF on the sample.
    uint8_t hp_digest[32];
    crypto_generichash(hp_digest, sizeof(hp_digest),
                       sample, 16, hp_key, 16);
    memcpy(mask, hp_digest, 5);

    *first_byte ^= (mask[0] & 0x0F);
    for (size_t i = 0; i < pkt_num_len; i++)
        pkt_num_bytes[i] ^= mask[1 + i];
}

static trellis_err_t quic_wrap(const uint8_t *in, size_t in_len,
                               trellis_buf_t *out, void *state)
{
    quic_state_t *st = state;

    // Transition to 1-RTT after 3 Initial packets (client) or 2 (server)
    uint32_t transition = st->is_initiator ? 3 : 2;
    if (st->phase == QUIC_INITIAL && st->phase_pkt_count >= transition) {
        st->phase = QUIC_ONE_RTT;
        // Rotate DCID on phase transition per RFC 9000 §5.1.
        randombytes_buf(st->dcid, sizeof(st->dcid));
        st->phase_pkt_count = 0;
    }

    if (st->phase == QUIC_INITIAL) {
        /* Long Header Initial packet with AEAD-encrypted payload.
         *
         * RFC 9001 §5 uses AES-128-GCM.  libsodium does not expose
         * AES-128-GCM directly, so we use ChaCha20-Poly1305-IETF with
         * the derived key (padded to 32 bytes) and the packet-number-based
         * nonce.  The result is opaque binary indistinguishable from
         * encrypted ciphertext. */
        uint8_t key[16], iv_bytes[12], hp[16];
        quic_derive_initial_keys(st->dcid, sizeof(st->dcid),
                                 st->is_initiator, key, iv_bytes, hp);

        // Build header.
        size_t hdr_len = 1 + 4 + 1 + 8 + 1 + 8 + 1 + 2 + 4;
        // AEAD adds a 16-byte Poly1305 tag.
        size_t ct_len = in_len + 16;
        size_t payload_len = 4 + ct_len; /* pkt_num + ciphertext */
        size_t total = hdr_len + payload_len;

        *out = trellis_buf_alloc(total);
        if (!out->data) return TRELLIS_ERR_NOMEM;

        uint8_t *p = out->data;
        uint8_t *first_byte = p++;
        *first_byte = 0xC3; /* Long header, Initial, 4-byte pkt num */

        *p++ = 0x00; *p++ = 0x00; *p++ = 0x00; *p++ = 0x01; /* version 1 */
        *p++ = 8;  memcpy(p, st->dcid, 8); p += 8;
        *p++ = 8;  memcpy(p, st->scid, 8); p += 8;
        *p++ = 0x00; /* token length = 0 */

        // Payload length as 2-byte QUIC varint.
        *p++ = (uint8_t)(0x40 | ((payload_len >> 8) & 0x3F));
        *p++ = (uint8_t)(payload_len);

        // Packet number (4 bytes)
        uint8_t *pkt_num_start = p;
        *p++ = (uint8_t)(st->pkt_num >> 24);
        *p++ = (uint8_t)(st->pkt_num >> 16);
        *p++ = (uint8_t)(st->pkt_num >> 8);
        *p++ = (uint8_t)(st->pkt_num);

        // Construct AEAD nonce: XOR pkt_num into the last 4 bytes of iv_bytes.
        uint8_t nonce[12];
        memcpy(nonce, iv_bytes, 12);
        nonce[8]  ^= (uint8_t)(st->pkt_num >> 24);
        nonce[9]  ^= (uint8_t)(st->pkt_num >> 16);
        nonce[10] ^= (uint8_t)(st->pkt_num >> 8);
        nonce[11] ^= (uint8_t)(st->pkt_num);

        // Expand 16-byte key to 32 bytes for ChaCha20 by hashing.
        uint8_t key32[32];
        crypto_hash_sha256(key32, key, 16);

        // AEAD-encrypt payload; the header bytes up to pkt_num serve as AAD.
        unsigned long long written_ct_len;
        if (crypto_aead_chacha20poly1305_ietf_encrypt(
                p, &written_ct_len,
                in, in_len,
                out->data, (size_t)(p - out->data), /* AAD = header + pkt_num */
                NULL, nonce, key32) != 0) {
            trellis_buf_free(out);
            sodium_memzero(key32, sizeof(key32));
            return TRELLIS_ERR_ENCRYPT;
        }
        p += (size_t)written_ct_len;
        sodium_memzero(key32, sizeof(key32));

        out->len = (size_t)(p - out->data);

        // Apply header protection using sample = first 16 bytes of ciphertext.
        const uint8_t *sample = pkt_num_start + 4;
        if ((size_t)(sample - out->data) + 16 <= out->len)
            quic_apply_header_protection(first_byte, pkt_num_start, 4, hp, sample);
    } else {
        // Short Header (1-RTT) packet with AEAD payload.
        size_t hdr_len = 1 + 8 + 4; /* form byte + dcid + 4-byte pkt_num */
        size_t ct_len = in_len + 16; /* ChaCha20-Poly1305 tag */
        size_t total = hdr_len + ct_len;

        *out = trellis_buf_alloc(total);
        if (!out->data) return TRELLIS_ERR_NOMEM;

        uint8_t *p = out->data;
        *p++ = (uint8_t)(0x40 | (st->pkt_num & 0x3F)); /* Short header, key phase 0 */
        memcpy(p, st->dcid, 8); p += 8;
        *p++ = (uint8_t)(st->pkt_num >> 24);
        *p++ = (uint8_t)(st->pkt_num >> 16);
        *p++ = (uint8_t)(st->pkt_num >> 8);
        *p++ = (uint8_t)(st->pkt_num);

        uint8_t nonce1[12] = {0};
        nonce1[8]  = (uint8_t)(st->pkt_num >> 24);
        nonce1[9]  = (uint8_t)(st->pkt_num >> 16);
        nonce1[10] = (uint8_t)(st->pkt_num >> 8);
        nonce1[11] = (uint8_t)(st->pkt_num);

        if (st->rtt1_key_set) {
            unsigned long long written;
            if (crypto_aead_chacha20poly1305_ietf_encrypt(
                    p, &written,
                    in, in_len,
                    out->data, hdr_len, /* AAD = short header bytes */
                    NULL, nonce1, st->rtt1_key) != 0) {
                trellis_buf_free(out);
                return TRELLIS_ERR_ENCRYPT;
            }
            p += (size_t)written;
        } else {
            trellis_buf_free(out);
            return TRELLIS_ERR_ENCRYPT;
        }
        out->len = (size_t)(p - out->data);
    }

    if (st->pkt_num == UINT32_MAX) {
        // Nonce space exhausted; caller must re-key before the next packet.
        trellis_buf_free(out);
        return TRELLIS_ERR_ENCRYPT;
    }
    st->pkt_num++;
    st->phase_pkt_count++;
    return TRELLIS_OK;
}

static trellis_err_t quic_unwrap(const uint8_t *in, size_t in_len,
                                 trellis_buf_t *out, void *state)
{
    quic_state_t *st = state;
    if (in_len < 2) return TRELLIS_ERR_DECODE;

    bool is_long = (in[0] & 0x80) != 0;

    if (is_long) {
        /* Long header: parse form+version+dcid+scid+token+length+pkt_num,
         * then AEAD-decrypt the ciphertext payload. */
        if (in_len < 7) return TRELLIS_ERR_DECODE;
        const uint8_t *p = in + 5;
        size_t left = in_len - 5;

        if (left < 1) return TRELLIS_ERR_DECODE;
        uint8_t dcid_len = *p++; left--;
        if (left < dcid_len) return TRELLIS_ERR_DECODE;
        const uint8_t *dcid_ptr = p;
        p += dcid_len; left -= dcid_len;

        if (left < 1) return TRELLIS_ERR_DECODE;
        uint8_t scid_len = *p++; left--;
        if (left < scid_len) return TRELLIS_ERR_DECODE;
        p += scid_len; left -= scid_len;

        // token
        if (left < 1) return TRELLIS_ERR_DECODE;
        uint8_t tok_len = *p++; left--;
        if (left < tok_len) return TRELLIS_ERR_DECODE;
        p += tok_len; left -= tok_len;

        // length (2-byte varint)
        if (left < 2) return TRELLIS_ERR_DECODE;
        uint16_t payload_len = (uint16_t)((p[0] & 0x3F) << 8) | p[1];
        p += 2; left -= 2;

        // pkt_num (4 bytes)
        if (left < 4) return TRELLIS_ERR_DECODE;
        uint32_t pkt_num = ((uint32_t)p[0] << 24) | ((uint32_t)p[1] << 16)
                         | ((uint32_t)p[2] << 8) | p[3];
        const uint8_t *hdr_start = in;
        size_t hdr_end = (size_t)(p + 4 - in); /* AAD length = header + pkt_num */
        p += 4; left -= 4;

        size_t ct_len = payload_len >= 4 ? payload_len - 4 : 0;
        if (left < ct_len) ct_len = left;
        if (ct_len < 16) return TRELLIS_ERR_DECODE; /* too short for Poly1305 tag */

        // Derive same keys the sender used (receiver is the opposite role)
        uint8_t key[16], iv_bytes[12], hp[16];
        bool is_client_role = !st->is_initiator; /* we receive from the opposite side */
        quic_derive_initial_keys(dcid_ptr, dcid_len, is_client_role,
                                 key, iv_bytes, hp);
        (void)hp;

        uint8_t nonce[12];
        memcpy(nonce, iv_bytes, 12);
        nonce[8]  ^= (uint8_t)(pkt_num >> 24);
        nonce[9]  ^= (uint8_t)(pkt_num >> 16);
        nonce[10] ^= (uint8_t)(pkt_num >> 8);
        nonce[11] ^= (uint8_t)(pkt_num);

        uint8_t key32[32];
        crypto_hash_sha256(key32, key, 16);

        *out = trellis_buf_alloc(ct_len);
        if (!out->data) { sodium_memzero(key32, 32); return TRELLIS_ERR_NOMEM; }

        unsigned long long pt_len;
        int rc = crypto_aead_chacha20poly1305_ietf_decrypt(
            out->data, &pt_len,
            NULL,
            p, ct_len,
            hdr_start, hdr_end,
            nonce, key32);
        sodium_memzero(key32, sizeof(key32));

        if (rc != 0) { trellis_buf_free(out); return TRELLIS_ERR_DECRYPT; }
        out->len = (size_t)pt_len;
        st->phase = QUIC_INITIAL;
        return TRELLIS_OK;
    }

    // Short header: 1-byte form + 8-byte dcid + 4-byte pkt_num + [encrypted payload]
    if (in_len < 13) return TRELLIS_ERR_DECODE;

    uint32_t pkt_num = ((uint32_t)in[9] << 24)
                     | ((uint32_t)in[10] << 16)
                     | ((uint32_t)in[11] << 8)
                     | (uint32_t)in[12];
    size_t hdr_end = 13;
    size_t ct_len = in_len - hdr_end;
    const uint8_t *ct_ptr = in + hdr_end;

    if (!st->rtt1_key_set || ct_len < 16)
        return TRELLIS_ERR_DECRYPT;

    uint8_t nonce1[12] = {0};
    nonce1[8]  = (uint8_t)(pkt_num >> 24);
    nonce1[9]  = (uint8_t)(pkt_num >> 16);
    nonce1[10] = (uint8_t)(pkt_num >> 8);
    nonce1[11] = (uint8_t)(pkt_num);

    *out = trellis_buf_alloc(ct_len);
    if (!out->data) return TRELLIS_ERR_NOMEM;

    unsigned long long pt_len;
    if (crypto_aead_chacha20poly1305_ietf_decrypt(
            out->data, &pt_len,
            NULL,
            ct_ptr, ct_len,
            in, hdr_end,
            nonce1, st->rtt1_key) != 0) {
        trellis_buf_free(out);
        return TRELLIS_ERR_DECRYPT;
    }
    out->len = (size_t)pt_len;

    st->phase = QUIC_ONE_RTT;
    return TRELLIS_OK;
}

static void quic_free(void *state) {
    if (state) {
        sodium_memzero(state, sizeof(quic_state_t));
        free(state);
    }
}

static const trellis_camouflage_vtable_t quic_vtable = {
    .wrap          = quic_wrap,
    .unwrap        = quic_unwrap,
    .free_state    = quic_free,
    .protocol_name = "quic-mimic",
};

// --- Raw obfuscation (ChaCha20 stream cipher) ---
#define RAW_OBFS_NONCE_LEN 8  /* 64-bit nonce for chacha20 */

typedef struct raw_obfs_state {
    uint8_t key[32];
    uint64_t counter;
} raw_obfs_state_t;

static trellis_err_t raw_obfs_wrap(const uint8_t *in, size_t in_len,
                                   trellis_buf_t *out, void *state)
{
    raw_obfs_state_t *st = state;

    // Frame: [8-byte nonce][ChaCha20(payload)]
    *out = trellis_buf_alloc(RAW_OBFS_NONCE_LEN + in_len);
    if (!out->data) return TRELLIS_ERR_NOMEM;

    uint8_t nonce[8];
    uint64_t ctr = st->counter++;
    for (int i = 0; i < 8; i++)
        nonce[i] = (uint8_t)(ctr >> (i * 8));

    memcpy(out->data, nonce, RAW_OBFS_NONCE_LEN);

    // Use chacha20 with 64-bit nonce + 64-bit IC=0 (libsodium: nonce=8 bytes)
    crypto_stream_chacha20_xor(out->data + RAW_OBFS_NONCE_LEN,
                               in, in_len,
                               nonce, st->key);
    out->len = RAW_OBFS_NONCE_LEN + in_len;
    return TRELLIS_OK;
}

static trellis_err_t raw_obfs_unwrap(const uint8_t *in, size_t in_len,
                                     trellis_buf_t *out, void *state)
{
    raw_obfs_state_t *st = state;

    if (in_len < RAW_OBFS_NONCE_LEN) return TRELLIS_ERR_DECODE;

    const uint8_t *nonce = in;
    size_t payload_len = in_len - RAW_OBFS_NONCE_LEN;

    *out = trellis_buf_alloc(payload_len);
    if (!out->data && payload_len > 0) return TRELLIS_ERR_NOMEM;

    crypto_stream_chacha20_xor(out->data,
                               in + RAW_OBFS_NONCE_LEN, payload_len,
                               nonce, st->key);
    out->len = payload_len;
    (void)st;
    return TRELLIS_OK;
}

static void raw_obfs_free(void *state)
{
    if (state) {
        sodium_memzero(state, sizeof(raw_obfs_state_t));
        free(state);
    }
}

static const trellis_camouflage_vtable_t raw_obfs_vtable = {
    .wrap          = raw_obfs_wrap,
    .unwrap        = raw_obfs_unwrap,
    .free_state    = raw_obfs_free,
    .protocol_name = "raw-obfs",
};

static trellis_err_t noop_wrap(const uint8_t *in, size_t in_len,
                               trellis_buf_t *out, void *state)
{
    (void)state;
    *out = trellis_buf_from(in, in_len);
    if (!out->data && in_len > 0) return TRELLIS_ERR_NOMEM;
    return TRELLIS_OK;
}

static const trellis_camouflage_vtable_t noop_vtable = {
    .wrap          = noop_wrap,
    .unwrap        = noop_wrap,
    .free_state    = NULL,
    .protocol_name = "none",
};

// --- Public API ---
trellis_camouflage_t *trellis_camouflage_new(trellis_camouflage_type_t type,
                                             const uint8_t *session_key,
                                             size_t key_len,
                                             bool is_initiator)
{
    trellis_camouflage_t *camo = calloc(1, sizeof(*camo));
    if (!camo) return NULL;
    camo->type = type;

    switch (type) {
    case TRELLIS_CAMO_TLS13: {
        tls13_state_t *st = calloc(1, sizeof(*st));
        if (!st) { free(camo); return NULL; }
        st->is_initiator = is_initiator;
        st->phase = TLS13_HELLO;
        camo->vtable = &tls13_vtable;
        camo->state = st;
        break;
    }
    case TRELLIS_CAMO_HTTP2: {
        http2_state_t *st = calloc(1, sizeof(*st));
        if (!st) { free(camo); return NULL; }
        st->is_initiator = is_initiator;
        st->phase = HTTP2_PREFACE;
        // Client streams start at 1, server at 2, both increment by 2.
        st->next_stream_id = is_initiator ? 1 : 2;
        st->stream_id = 0;
        st->frames_on_current_stream = 0;
        camo->vtable = &http2_vtable;
        camo->state = st;
        break;
    }
    case TRELLIS_CAMO_DNS: {
        dns_state_t *st = calloc(1, sizeof(*st));
        if (!st) { free(camo); return NULL; }
        randombytes_buf(&st->tx_id, 2);
        st->is_initiator = is_initiator;
        camo->vtable = &dns_vtable;
        camo->state = st;
        break;
    }
    case TRELLIS_CAMO_QUIC: {
        quic_state_t *st = calloc(1, sizeof(*st));
        if (!st) { free(camo); return NULL; }
        randombytes_buf(st->dcid, sizeof(st->dcid));
        randombytes_buf(st->scid, sizeof(st->scid));
        st->is_initiator = is_initiator;
        st->phase = QUIC_INITIAL;
        if (session_key && key_len > 0) {
            crypto_generichash(st->rtt1_key, sizeof(st->rtt1_key),
                               session_key, key_len,
                               (const uint8_t *)"bloom-quic-1rtt", 15);
            st->rtt1_key_set = true;
        }
        camo->vtable = &quic_vtable;
        camo->state = st;
        break;
    }
    case TRELLIS_CAMO_RAW_OBFS: {
        raw_obfs_state_t *st = calloc(1, sizeof(*st));
        if (!st) { free(camo); return NULL; }
        if (session_key && key_len > 0) {
            crypto_generichash(st->key, sizeof(st->key),
                               session_key, key_len,
                               (const uint8_t *)"bloom-raw-obfs", 14);
        } else {
            randombytes_buf(st->key, sizeof(st->key));
        }
        camo->vtable = &raw_obfs_vtable;
        camo->state = st;
        break;
    }
    default:
        camo->vtable = &noop_vtable;
        camo->state = NULL;
        break;
    }

    return camo;
}

void trellis_camouflage_free(trellis_camouflage_t *camo)
{
    if (!camo) return;
    if (camo->vtable && camo->vtable->free_state && camo->state)
        camo->vtable->free_state(camo->state);
    free(camo);
}

trellis_err_t trellis_camouflage_wrap(trellis_camouflage_t *camo,
                                      const uint8_t *in, size_t in_len,
                                      trellis_buf_t *out)
{
    if (!camo || !camo->vtable || !camo->vtable->wrap || !out)
        return TRELLIS_ERR_INVALID_ARG;
    return camo->vtable->wrap(in, in_len, out, camo->state);
}

trellis_err_t trellis_camouflage_unwrap(trellis_camouflage_t *camo,
                                        const uint8_t *in, size_t in_len,
                                        trellis_buf_t *out)
{
    if (!camo || !camo->vtable || !camo->vtable->unwrap || !out)
        return TRELLIS_ERR_INVALID_ARG;
    return camo->vtable->unwrap(in, in_len, out, camo->state);
}

const char *trellis_camouflage_protocol_name(const trellis_camouflage_t *camo)
{
    if (!camo || !camo->vtable) return "unknown";
    return camo->vtable->protocol_name;
}
