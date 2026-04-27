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
 * LoRa transport adapter with DTN store-and-forward (RFC 9171 BPv7-inspired)
 *
 * Operates a LoRa radio modem over a serial UART connection (e.g. SX1276 /
 * SX1278 connected via USB-to-serial, or a Meshtastic-compatible node).
 * Bloom frames are chunked to fit within the LoRa payload limit, bundled with
 * destination fingerprint + TTL, persisted to disk, and forwarded when a
 * relay comes in range.
 *
 * Frame format on-air (after AT/modem framing):
 *   [4B bloom_magic][1B chunk_idx][1B total_chunks][2B bundle_id][1B ttl_hops]
 *   [32B dest_fp][N bytes payload]   (max N = lora_max_payload - 40)
 *
 * DTN bundle store (on-disk):
 *   Each bundle is stored as a fixed-header binary file:
 *     magic[4]  = "DTNN"
 *     bundle_id[2]
 *     dest_fp[32]
 *     ttl_s[4]
 *     created[8] (unix ms)
 *     retry_count[4]
 *     payload_len[4]
 *     payload[...]
 *
 * Build guard: TRELLIS_WITH_DTN
 * Dependencies: libuv (for serial I/O via uv_pipe_t), libsodium
 */

#include "internal.h"

#ifdef TRELLIS_WITH_DTN

#include <sodium.h>
#include <uv.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <time.h>
#include <errno.h>
#include "../platform.h"

#define LORA_BLOOM_MAGIC       0x424C4F4D  /* "BLOM" */
/* Header layout: magic(4) + chunk_idx(1) + total_chunks(1) + bundle_id(2)
 *                + ttl(1) + dest_fp(32) = 41 bytes total */
#define LORA_CHUNK_HEADER_LEN  41u
#define LORA_MAX_PAYLOAD       240u        /* raw LoRa max payload */
#define LORA_CHUNK_DATA_MAX    (LORA_MAX_PAYLOAD - LORA_CHUNK_HEADER_LEN)
#define LORA_MAX_REASSEMBLY    32u         /* max in-flight reassembly contexts */
#define DTN_BUNDLE_MAGIC       "DTNN"
#define DTN_BUNDLE_MAGIC_LEN   4u
#define DTN_MAX_BUNDLES_MEM    128u        /* max in-memory bundle list */
#define DTN_SCAN_INTERVAL_MS   60000u      /* scan store every 60 s */
#define DTN_DEFAULT_TTL_S      86400u      /* 24 h */
#define DTN_DEFAULT_MAX_STORE  (64u * 1024u * 1024u)
#define DTN_DEFAULT_MAX_BUNDLES 1024u
#define DTN_BUNDLE_HEADER_LEN  (DTN_BUNDLE_MAGIC_LEN + 2 + 32 + 4 + 8 + 4 + 4)

// -----------
// DTN bundle store.
typedef struct dtn_bundle {
    uint16_t bundle_id;
    uint8_t  dest_fp[32];
    uint32_t ttl_s;
    uint64_t created_ms;
    uint32_t retry_count;
    uint8_t *payload;
    size_t   payload_len;
    char     file_path[512];
} dtn_bundle_t;

typedef struct dtn_store {
    char          path[512];
    uint32_t      bundle_ttl_s;
    uint64_t      max_store_bytes;
    size_t        max_bundles;
    uint64_t      used_bytes;

    dtn_bundle_t *bundles[DTN_MAX_BUNDLES_MEM];
    size_t        bundle_count;

    uv_loop_t    *loop;
    uv_timer_t    scan_timer;
    bool          timer_active;
} dtn_store_t;

static uint64_t dtn_now_ms(void)
{
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    return (uint64_t)ts.tv_sec * 1000 + (uint64_t)ts.tv_nsec / 1000000;
}

static void dtn_bundle_write(const dtn_store_t *store, dtn_bundle_t *b)
{
    if (!store->path[0]) return;

    char path[512];
    snprintf(path, sizeof(path), "%s/bundle_%04x.dtn", store->path, b->bundle_id);
    strncpy(b->file_path, path, sizeof(b->file_path) - 1);

    FILE *f = fopen(path, "wb");
    if (!f) return;

    uint8_t hdr[DTN_BUNDLE_HEADER_LEN];
    memcpy(hdr, DTN_BUNDLE_MAGIC, 4);
    hdr[4] = (uint8_t)(b->bundle_id >> 8);
    hdr[5] = (uint8_t)(b->bundle_id & 0xFF);
    memcpy(hdr + 6, b->dest_fp, 32);
    hdr[38] = (uint8_t)(b->ttl_s >> 24);
    hdr[39] = (uint8_t)(b->ttl_s >> 16);
    hdr[40] = (uint8_t)(b->ttl_s >> 8);
    hdr[41] = (uint8_t)(b->ttl_s & 0xFF);
    for (int i = 0; i < 8; i++)
        hdr[42 + i] = (uint8_t)(b->created_ms >> ((7 - i) * 8));
    hdr[50] = (uint8_t)(b->retry_count >> 24);
    hdr[51] = (uint8_t)(b->retry_count >> 16);
    hdr[52] = (uint8_t)(b->retry_count >> 8);
    hdr[53] = (uint8_t)(b->retry_count & 0xFF);
    hdr[54] = (uint8_t)(b->payload_len >> 24);
    hdr[55] = (uint8_t)(b->payload_len >> 16);
    hdr[56] = (uint8_t)(b->payload_len >> 8);
    hdr[57] = (uint8_t)(b->payload_len & 0xFF);

    fwrite(hdr, sizeof(hdr), 1, f);
    if (b->payload && b->payload_len)
        fwrite(b->payload, b->payload_len, 1, f);
    fclose(f);
}

static dtn_bundle_t *dtn_bundle_read(const char *path)
{
    FILE *f = fopen(path, "rb");
    if (!f) return NULL;

    uint8_t hdr[DTN_BUNDLE_HEADER_LEN];
    if (fread(hdr, sizeof(hdr), 1, f) != 1) {
        fclose(f); return NULL;
    }

    if (memcmp(hdr, DTN_BUNDLE_MAGIC, 4) != 0) {
        fclose(f); return NULL;
    }

    dtn_bundle_t *b = calloc(1, sizeof(*b));
    if (!b) { fclose(f); return NULL; }

    b->bundle_id = (uint16_t)(((uint16_t)hdr[4] << 8) | hdr[5]);
    memcpy(b->dest_fp, hdr + 6, 32);
    b->ttl_s = ((uint32_t)hdr[38] << 24) | ((uint32_t)hdr[39] << 16) |
               ((uint32_t)hdr[40] << 8)  | hdr[41];
    b->created_ms = 0;
    for (int i = 0; i < 8; i++)
        b->created_ms = (b->created_ms << 8) | hdr[42 + i];
    b->retry_count = ((uint32_t)hdr[50] << 24) | ((uint32_t)hdr[51] << 16) |
                     ((uint32_t)hdr[52] << 8)  | hdr[53];
    b->payload_len = ((uint32_t)hdr[54] << 24) | ((uint32_t)hdr[55] << 16) |
                     ((uint32_t)hdr[56] << 8)  | hdr[57];

    if (b->payload_len > 0 && b->payload_len <= 1024 * 1024) {
        b->payload = malloc(b->payload_len);
        if (!b->payload || fread(b->payload, b->payload_len, 1, f) != 1) {
            free(b->payload); free(b); fclose(f);
            return NULL;
        }
    }

    strncpy(b->file_path, path, sizeof(b->file_path) - 1);
    fclose(f);
    return b;
}

static void dtn_store_init(dtn_store_t *store, const trellis_dtn_config_t *cfg,
                            uv_loop_t *loop)
{
    if (cfg && cfg->dtn_store_path) {
        size_t l = strlen(cfg->dtn_store_path);
        if (l >= sizeof(store->path)) l = sizeof(store->path) - 1;
        memcpy(store->path, cfg->dtn_store_path, l);
    }
    store->bundle_ttl_s   = (cfg && cfg->bundle_ttl_s)   ? cfg->bundle_ttl_s   : DTN_DEFAULT_TTL_S;
    store->max_store_bytes = (cfg && cfg->max_store_bytes) ? cfg->max_store_bytes : DTN_DEFAULT_MAX_STORE;
    store->max_bundles    = (cfg && cfg->max_bundles)    ? cfg->max_bundles    : DTN_DEFAULT_MAX_BUNDLES;
    store->loop           = loop;

    // Create store directory.
    if (store->path[0])
        trellis_mkdir(store->path, 0700);
}

static trellis_err_t dtn_store_put(dtn_store_t *store,
                                    const uint8_t *dest_fp,
                                    const uint8_t *data, size_t len)
{
    if (store->bundle_count >= DTN_MAX_BUNDLES_MEM)
        return TRELLIS_ERR_FULL;

    dtn_bundle_t *b = calloc(1, sizeof(*b));
    if (!b) return TRELLIS_ERR_NOMEM;

    randombytes_buf(&b->bundle_id, 2);
    memcpy(b->dest_fp, dest_fp, 32);
    b->ttl_s      = store->bundle_ttl_s;
    b->created_ms = dtn_now_ms();

    b->payload = malloc(len);
    if (!b->payload) { free(b); return TRELLIS_ERR_NOMEM; }
    memcpy(b->payload, data, len);
    b->payload_len = len;

    dtn_bundle_write(store, b);
    store->used_bytes += len;
    store->bundles[store->bundle_count++] = b;

    return TRELLIS_OK;
}

static void dtn_store_expire(dtn_store_t *store)
{
    uint64_t now_ms = dtn_now_ms();
    size_t write_idx = 0;
    for (size_t i = 0; i < store->bundle_count; i++) {
        dtn_bundle_t *b = store->bundles[i];
        uint64_t age_s = (now_ms - b->created_ms) / 1000;
        if (age_s > b->ttl_s) {
            if (b->file_path[0]) remove(b->file_path);
            free(b->payload);
            free(b);
        } else {
            store->bundles[write_idx++] = b;
        }
    }
    store->bundle_count = write_idx;
}

static void dtn_scan_timer_cb(uv_timer_t *t)
{
    dtn_store_t *store = (dtn_store_t *)t->data;
    dtn_store_expire(store);
}

static void dtn_store_start_timer(dtn_store_t *store)
{
    if (!store->loop) return;
    uv_timer_init(store->loop, &store->scan_timer);
    store->scan_timer.data = store;
    uv_timer_start(&store->scan_timer, dtn_scan_timer_cb,
                   DTN_SCAN_INTERVAL_MS, DTN_SCAN_INTERVAL_MS);
    store->timer_active = true;
}

/* AT-command write callback: frees the heap-allocated data buffer and the
 * write request itself once the async write has completed. */
static void lora_at_write_cb(uv_write_t *req, int status)
{
    (void)status;
    free(req->data); /* at_config buffer */
    free(req);       /* uv_write_t */
}

typedef struct lora_reassembly {
    uint16_t  bundle_id;
    uint8_t   total_chunks;
    uint8_t  *chunks[32];   /* pointers to received chunks (NULL if missing) */
    size_t    chunk_lens[32];
    uint32_t  received;     /* bitmask of received chunk indices (supports ≤32 chunks) */
    uint64_t  first_seen_ms;
    uint8_t   dest_fp[32];
} lora_reassembly_t;

typedef struct lora_conn_data {
    trellis_conn_t       *conn;
    struct lora_transport_data *td;

    trellis_conn_recv_cb  recv_cb;
    void                 *recv_ctx;
    bool                  recv_active;
    bool                  closed;
} lora_conn_data_t;

// -----------
// Per-transport state.
typedef struct lora_transport_data {
    trellis_transport_t *transport;

    // Serial port via libuv pipe.
    uv_pipe_t            serial;
    bool                 serial_open;
    char                 serial_path[256];
    uint32_t             baud_rate;

    // Config
    uint32_t frequency_hz;
    uint8_t  spreading_factor;
    uint8_t  bandwidth_khz;
    uint8_t  coding_rate;
    int8_t   tx_power_dbm;
    bool     meshtastic_compat;

    // DTN store.
    dtn_store_t dtn_store;

    // Reassembly contexts.
    lora_reassembly_t reassembly[LORA_MAX_REASSEMBLY];
    size_t            reassembly_count;

    // Active logical connections.
    lora_conn_data_t *conns[16];
    size_t            conn_count;

    trellis_accept_cb  accept_cb;
    void              *accept_ctx;

    // Serial receive buffer.
    uint8_t  rx_buf[1024];
    size_t   rx_buf_len;
} lora_transport_data_t;

// -----------
// LoRa chunk helpers.

/*
 * Build a LoRa chunk on-air frame.
 * Returns length of encoded frame, or 0 on failure.
 */
static size_t lora_build_chunk(const uint8_t *dest_fp,
                                uint16_t bundle_id, uint8_t chunk_idx,
                                uint8_t total_chunks, uint8_t ttl_hops,
                                const uint8_t *payload, size_t payload_len,
                                uint8_t *out, size_t out_cap)
{
    if (payload_len > LORA_CHUNK_DATA_MAX)
        payload_len = LORA_CHUNK_DATA_MAX;
    if (out_cap < 41 + payload_len)
        return 0;

    size_t off = 0;
    out[off++] = (uint8_t)(LORA_BLOOM_MAGIC >> 24);
    out[off++] = (uint8_t)(LORA_BLOOM_MAGIC >> 16);
    out[off++] = (uint8_t)(LORA_BLOOM_MAGIC >> 8);
    out[off++] = (uint8_t)(LORA_BLOOM_MAGIC & 0xFF);
    out[off++] = chunk_idx;
    out[off++] = total_chunks;
    out[off++] = (uint8_t)(bundle_id >> 8);
    out[off++] = (uint8_t)(bundle_id & 0xFF);
    out[off++] = ttl_hops;
    memcpy(out + off, dest_fp, 32); off += 32;
    memcpy(out + off, payload, payload_len); off += payload_len;
    return off;
}

/*
 * Parse an incoming LoRa frame.
 * Returns true if the frame is a valid Bloom LoRa chunk.
 */
static bool lora_parse_chunk(const uint8_t *frame, size_t frame_len,
                              uint8_t *out_dest_fp,
                              uint16_t *out_bundle_id,
                              uint8_t *out_chunk_idx,
                              uint8_t *out_total_chunks,
                              uint8_t *out_ttl_hops,
                              const uint8_t **out_payload,
                              size_t *out_payload_len)
{
    if (frame_len < 41) return false;
    uint32_t magic = ((uint32_t)frame[0] << 24) | ((uint32_t)frame[1] << 16) |
                     ((uint32_t)frame[2] << 8)  | frame[3];
    if (magic != LORA_BLOOM_MAGIC) return false;

    *out_chunk_idx   = frame[4];
    *out_total_chunks = frame[5];
    *out_bundle_id   = (uint16_t)(((uint16_t)frame[6] << 8) | frame[7]);
    *out_ttl_hops    = frame[8];
    memcpy(out_dest_fp, frame + 9, 32);
    *out_payload     = frame + 41;
    *out_payload_len = frame_len - 41;
    return true;
}

// -----------
// Serial I/O.
static void lora_serial_alloc_cb(uv_handle_t *handle, size_t suggested_size,
                                  uv_buf_t *buf)
{
    (void)handle;
    buf->base = malloc(suggested_size > 512 ? 512 : suggested_size);
    buf->len  = buf->base ? (unsigned)(suggested_size > 512 ? 512 : suggested_size) : 0;
}

static void lora_serial_read_cb(uv_stream_t *stream, ssize_t nread,
                                 const uv_buf_t *buf)
{
    lora_transport_data_t *td = (lora_transport_data_t *)stream->data;

    if (nread <= 0) {
        free(buf->base);
        if (nread == UV_EOF)
            fprintf(stderr, "[LoRa] serial port closed\n");
        return;
    }

    // Accumulate into rx_buf.
    size_t space = sizeof(td->rx_buf) - td->rx_buf_len;
    size_t copy  = (size_t)nread < space ? (size_t)nread : space;
    memcpy(td->rx_buf + td->rx_buf_len, buf->base, copy);
    td->rx_buf_len += copy;
    free(buf->base);

    /* For AT+receive-mode modems, each received LoRa packet arrives as a
     * line like "+RCV=<len>,<payload_hex>\r\n".
     * Search for this pattern and extract the hex payload. */
    char *line_end;
    while ((line_end = memchr(td->rx_buf, '\n', td->rx_buf_len)) != NULL) {
        size_t line_len = (size_t)(line_end - (char *)td->rx_buf) + 1;
        char line[512];
        if (line_len < sizeof(line)) {
            memcpy(line, td->rx_buf, line_len);
            line[line_len] = '\0';

            // Parse AT+RCV= response.
            if (strncmp(line, "+RCV=", 5) == 0) {
                int pkt_len;
                char hex_payload[512];
                if (sscanf(line + 5, "%d,%511s", &pkt_len, hex_payload) == 2
                    && pkt_len > 0 && pkt_len <= (int)LORA_MAX_PAYLOAD) {
                    // Decode hex payload.
                    uint8_t pkt[LORA_MAX_PAYLOAD];
                    for (int i = 0; i < pkt_len && 2 * i + 1 < (int)strlen(hex_payload); i++) {
                        unsigned byte;
                        sscanf(hex_payload + 2 * i, "%02x", &byte);
                        pkt[i] = (uint8_t)byte;
                    }

                    // Parse Bloom LoRa chunk.
                    uint8_t  dest_fp[32];
                    uint16_t bundle_id;
                    uint8_t  chunk_idx, total_chunks, ttl_hops;
                    const uint8_t *payload;
                    size_t payload_len;

                    if (lora_parse_chunk(pkt, (size_t)pkt_len,
                                         dest_fp, &bundle_id,
                                         &chunk_idx, &total_chunks, &ttl_hops,
                                         &payload, &payload_len)
                        && total_chunks > 0 && total_chunks <= 32) {
                        // Find or create reassembly context.
                        lora_reassembly_t *ra = NULL;
                        for (size_t i = 0; i < td->reassembly_count; i++) {
                            if (td->reassembly[i].bundle_id == bundle_id) {
                                ra = &td->reassembly[i];
                                break;
                            }
                        }
                        if (!ra && td->reassembly_count < LORA_MAX_REASSEMBLY) {
                            ra = &td->reassembly[td->reassembly_count++];
                            memset(ra, 0, sizeof(*ra));
                            ra->bundle_id     = bundle_id;
                            ra->total_chunks  = total_chunks;
                            ra->first_seen_ms = dtn_now_ms();
                            memcpy(ra->dest_fp, dest_fp, 32);
                        }
                        if (ra && chunk_idx < 32 && !ra->chunks[chunk_idx]) {
                            ra->chunks[chunk_idx] = malloc(payload_len);
                            if (ra->chunks[chunk_idx]) {
                                memcpy(ra->chunks[chunk_idx], payload, payload_len);
                                ra->chunk_lens[chunk_idx] = payload_len;
                                ra->received |= (1u << chunk_idx);
                            }
                        }

                        // Check if complete.
                        if (ra && ra->received == ((1u << total_chunks) - 1u)) {
                            // Reassemble
                            size_t total = 0;
                            for (uint8_t ci = 0; ci < total_chunks; ci++)
                                total += ra->chunk_lens[ci];

                            uint8_t *full = malloc(total);
                            if (full) {
                                size_t off = 0;
                                for (uint8_t ci = 0; ci < total_chunks; ci++) {
                                    memcpy(full + off, ra->chunks[ci], ra->chunk_lens[ci]);
                                    off += ra->chunk_lens[ci];
                                    free(ra->chunks[ci]);
                                    ra->chunks[ci] = NULL;
                                }

                                // Deliver to all active recv connections.
                                for (size_t ci2 = 0; ci2 < td->conn_count; ci2++) {
                                    lora_conn_data_t *cd = td->conns[ci2];
                                    if (cd && cd->recv_active && cd->recv_cb)
                                        cd->recv_cb(cd->conn, full, total,
                                                    TRELLIS_OK, cd->recv_ctx);
                                }
                                free(full);
                            } else {
                                for (uint8_t ci = 0; ci < total_chunks; ci++) {
                                    free(ra->chunks[ci]);
                                    ra->chunks[ci] = NULL;
                                }
                            }

                            // Remove reassembly entry.
                            for (size_t ri = 0; ri < td->reassembly_count; ri++) {
                                if (&td->reassembly[ri] == ra) {
                                    td->reassembly[ri] = td->reassembly[--td->reassembly_count];
                                    break;
                                }
                            }
                        }
                    }
                }
            }
        }

        td->rx_buf_len -= line_len;
        if (td->rx_buf_len > 0)
            memmove(td->rx_buf, td->rx_buf + line_len, td->rx_buf_len);
    }
}

// -----------
// Send a frame over LoRa serial.
typedef struct lora_write_req {
    uv_write_t     req;
    uint8_t       *data;
    trellis_conn_send_cb cb;
    void          *ctx;
} lora_write_req_t;

static void lora_write_done_cb(uv_write_t *req, int status)
{
    lora_write_req_t *wr = (lora_write_req_t *)req;
    trellis_err_t err = (status == 0) ? TRELLIS_OK : TRELLIS_ERR_TRANSPORT;
    if (wr->cb) wr->cb(err, wr->ctx);
    free(wr->data);
    free(wr);
}

static trellis_err_t lora_send_frame(lora_transport_data_t *td,
                                      const uint8_t *dest_fp,
                                      const uint8_t *data, size_t len,
                                      trellis_conn_send_cb cb, void *ctx)
{
    if (!td->serial_open) {
        // Store in DTN bundle for later delivery.
        trellis_err_t err = dtn_store_put(&td->dtn_store, dest_fp, data, len);
        if (err == TRELLIS_OK)
            fprintf(stderr, "[LoRa/DTN] frame queued (serial not available)\n");
        if (cb) cb(err, ctx);
        return err;
    }

    // Fragment into chunks.
    uint16_t bundle_id;
    randombytes_buf(&bundle_id, 2);

    uint8_t total_chunks = (uint8_t)((len + LORA_CHUNK_DATA_MAX - 1) / LORA_CHUNK_DATA_MAX);
    if (total_chunks == 0) total_chunks = 1;

    for (uint8_t ci = 0; ci < total_chunks; ci++) {
        size_t chunk_offset  = (size_t)ci * LORA_CHUNK_DATA_MAX;
        size_t chunk_len     = len - chunk_offset;
        if (chunk_len > LORA_CHUNK_DATA_MAX) chunk_len = LORA_CHUNK_DATA_MAX;

        uint8_t frame[LORA_MAX_PAYLOAD + 4];  /* +4 for AT command overhead */
        size_t frame_len = lora_build_chunk(
            dest_fp, bundle_id, ci, total_chunks, 8,
            data + chunk_offset, chunk_len,
            frame, sizeof(frame));
        if (frame_len == 0) continue;

        /* Format AT+SEND command: "AT+SEND=0,<len>,<hex>\r\n"
         * Need: 10 + 3 + 1 + (frame_len*2) + 2; frame_len <= LORA_MAX_PAYLOAD */
        char at_cmd[16 + (LORA_MAX_PAYLOAD * 2) + 4];
        size_t at_len = 0;
        at_len += (size_t)snprintf(at_cmd, sizeof(at_cmd),
                                   "AT+SEND=0,%zu,", frame_len);
        for (size_t bi = 0; bi < frame_len; bi++)
            at_len += (size_t)snprintf(at_cmd + at_len, sizeof(at_cmd) - at_len,
                                       "%02X", frame[bi]);
        at_cmd[at_len++] = '\r';
        at_cmd[at_len++] = '\n';

        lora_write_req_t *wr = calloc(1, sizeof(*wr));
        uint8_t *buf = malloc(at_len);
        if (!wr || !buf) {
            free(wr); free(buf);
            if (cb) cb(TRELLIS_ERR_NOMEM, ctx);
            return TRELLIS_ERR_NOMEM;
        }
        memcpy(buf, at_cmd, at_len);
        wr->data = buf;
        // Only fire callback on the last chunk.
        wr->cb  = (ci == total_chunks - 1) ? cb : NULL;
        wr->ctx = ctx;

        uv_buf_t uv_buf = uv_buf_init((char *)buf, (unsigned)at_len);
        uv_write((uv_write_t *)wr, (uv_stream_t *)&td->serial, &uv_buf, 1,
                 lora_write_done_cb);
    }

    return TRELLIS_OK;
}

// -----------
// Connection vtable.
static trellis_err_t lora_conn_send(trellis_conn_t *conn,
                                     const uint8_t *data, size_t len,
                                     trellis_conn_send_cb cb, void *ctx)
{
    lora_conn_data_t      *cd = (lora_conn_data_t *)conn->impl_data;
    lora_transport_data_t *td = cd->td;
    // Broadcast destination (all zeros = any relay)
    uint8_t dest_fp[32] = {0};
    return lora_send_frame(td, dest_fp, data, len, cb, ctx);
}

static trellis_err_t lora_conn_recv_start(trellis_conn_t *conn,
                                           trellis_conn_recv_cb cb, void *ctx)
{
    lora_conn_data_t *cd = (lora_conn_data_t *)conn->impl_data;
    cd->recv_cb     = cb;
    cd->recv_ctx    = ctx;
    cd->recv_active = true;
    return TRELLIS_OK;
}

static void lora_conn_recv_stop(trellis_conn_t *conn)
{
    lora_conn_data_t *cd = (lora_conn_data_t *)conn->impl_data;
    cd->recv_active = false;
}

static void lora_conn_close(trellis_conn_t *conn,
                             trellis_conn_close_cb cb, void *ctx)
{
    lora_conn_data_t *cd = (lora_conn_data_t *)conn->impl_data;
    cd->closed = true;
    if (cb) cb(conn, ctx);
    trellis_conn_dealloc(conn);
}

static const trellis_conn_vtable_t lora_conn_vtable = {
    .send       = lora_conn_send,
    .recv_start = lora_conn_recv_start,
    .recv_stop  = lora_conn_recv_stop,
    .close      = lora_conn_close,
};

// -----------
// Transport vtable.
static trellis_err_t lora_transport_connect(trellis_transport_t *t,
                                             const char *addr,
                                             trellis_connect_cb cb, void *ctx)
{
    lora_transport_data_t *td = (lora_transport_data_t *)t->impl_data;
    (void)addr; /* LoRa is broadcast; addr is used as bundle destination */

    trellis_conn_t *conn = trellis_conn_alloc(&lora_conn_vtable, "lora",
                                               t->loop);
    if (!conn) {
        if (cb) cb(NULL, TRELLIS_ERR_NOMEM, ctx);
        return TRELLIS_ERR_NOMEM;
    }

    lora_conn_data_t *cd = calloc(1, sizeof(*cd));
    if (!cd) {
        trellis_conn_dealloc(conn);
        if (cb) cb(NULL, TRELLIS_ERR_NOMEM, ctx);
        return TRELLIS_ERR_NOMEM;
    }
    cd->conn = conn;
    cd->td   = td;
    conn->impl_data = cd;

    if (td->conn_count < 16)
        td->conns[td->conn_count++] = cd;

    if (cb) cb(conn, TRELLIS_OK, ctx);
    return TRELLIS_OK;
}

static trellis_err_t lora_transport_listen(trellis_transport_t *t,
                                            const char *addr,
                                            trellis_accept_cb cb, void *ctx)
{
    lora_transport_data_t *td = (lora_transport_data_t *)t->impl_data;
    (void)addr;
    td->accept_cb  = cb;
    td->accept_ctx = ctx;

    // Open serial port.
    if (td->serial_path[0]) {
        uv_pipe_init(t->loop, &td->serial, 0);
        td->serial.data = td;

        int fd = open(td->serial_path,
                      O_RDWR | O_NOCTTY | O_NONBLOCK);
        if (fd >= 0) {
            uv_pipe_open(&td->serial, fd);
            uv_read_start((uv_stream_t *)&td->serial,
                          lora_serial_alloc_cb,
                          lora_serial_read_cb);
            td->serial_open = true;

            /* Send AT commands to configure the modem.
             * Both the write-request and the data buffer are heap-allocated so
             * they remain valid until the async write callback fires. */
            char *at_config = malloc(256);
            uv_write_t *wr = at_config
                             ? calloc(1, sizeof(*wr)) : NULL;
            if (at_config && wr) {
                snprintf(at_config, 256,
                         "AT+PARAMETER=%u,%u,%u,%u\r\n"
                         "AT+BAND=%u\r\n"
                         "AT+CRFOP=%d\r\n",
                         td->spreading_factor, td->bandwidth_khz,
                         td->coding_rate, 8,
                         td->frequency_hz,
                         (int)td->tx_power_dbm);

                // Store the data pointer so the callback can free it.
                wr->data = at_config;

                uv_buf_t buf = uv_buf_init(at_config,
                                           (unsigned)strlen(at_config));
                uv_write(wr, (uv_stream_t *)&td->serial, &buf, 1,
                         lora_at_write_cb);
            } else {
                free(at_config);
                free(wr);
            }

            fprintf(stderr, "[LoRa] serial %s opened, modem configured\n",
                    td->serial_path);
        } else {
            fprintf(stderr, "[LoRa] cannot open serial %s: %s; "
                            "DTN-only mode\n",
                    td->serial_path, strerror(errno));
        }
    }

    return TRELLIS_OK;
}

static trellis_err_t lora_transport_stop(trellis_transport_t *t)
{
    lora_transport_data_t *td = (lora_transport_data_t *)t->impl_data;
    if (td->serial_open) {
        uv_read_stop((uv_stream_t *)&td->serial);
        uv_close((uv_handle_t *)&td->serial, NULL);
        td->serial_open = false;
    }
    if (td->dtn_store.timer_active) {
        uv_timer_stop(&td->dtn_store.scan_timer);
        td->dtn_store.timer_active = false;
    }
    return TRELLIS_OK;
}

static void lora_transport_free(trellis_transport_t *t)
{
    lora_transport_data_t *td = (lora_transport_data_t *)t->impl_data;
    for (size_t i = 0; i < td->dtn_store.bundle_count; i++) {
        free(td->dtn_store.bundles[i]->payload);
        free(td->dtn_store.bundles[i]);
    }
    free(td);
    free(t);
}

static const trellis_transport_vtable_t lora_transport_vtable = {
    .connect = lora_transport_connect,
    .listen  = lora_transport_listen,
    .stop    = lora_transport_stop,
    .free    = lora_transport_free,
};

// -----------
// Public constructor.
trellis_transport_t *trellis_transport_lora_new(uv_loop_t *loop,
                                                const trellis_lora_config_t *cfg)
{
    trellis_transport_t *t = trellis_transport_alloc(&lora_transport_vtable,
                                                       "lora", loop);
    if (!t) return NULL;

    lora_transport_data_t *td = calloc(1, sizeof(*td));
    if (!td) { free(t); return NULL; }
    t->impl_data  = td;
    td->transport = t;

    // Defaults
    td->spreading_factor = 9;
    td->bandwidth_khz    = 125;
    td->coding_rate      = 5;
    td->tx_power_dbm     = 14;
    td->frequency_hz     = 915000000;
    td->baud_rate        = 115200;

    if (cfg) {
        if (cfg->serial_port) {
            size_t l = strlen(cfg->serial_port);
            if (l >= sizeof(td->serial_path)) l = sizeof(td->serial_path) - 1;
            memcpy(td->serial_path, cfg->serial_port, l);
        }
        if (cfg->baud_rate)        td->baud_rate        = cfg->baud_rate;
        if (cfg->frequency_hz)     td->frequency_hz     = cfg->frequency_hz;
        if (cfg->spreading_factor) td->spreading_factor = cfg->spreading_factor;
        if (cfg->bandwidth_khz)    td->bandwidth_khz    = cfg->bandwidth_khz;
        if (cfg->coding_rate)      td->coding_rate      = cfg->coding_rate;
        if (cfg->tx_power_dbm)     td->tx_power_dbm     = cfg->tx_power_dbm;
        td->meshtastic_compat = cfg->meshtastic_compat;
        dtn_store_init(&td->dtn_store, &cfg->dtn, loop);
    } else {
        dtn_store_init(&td->dtn_store, NULL, loop);
    }

    dtn_store_start_timer(&td->dtn_store);

    fprintf(stderr, "[LoRa] transport created (%.1f MHz SF%u BW%u CR4/%u "
                    "txp=%ddBm DTN=%s)\n",
            td->frequency_hz / 1e6f, td->spreading_factor,
            td->bandwidth_khz, td->coding_rate, (int)td->tx_power_dbm,
            td->dtn_store.path[0] ? td->dtn_store.path : "(memory-only)");

    return t;
}

#else /* !TRELLIS_WITH_DTN */

#include <trellis/transport.h>
#include <uv.h>
#include <stdio.h>

trellis_transport_t *trellis_transport_lora_new(uv_loop_t *loop,
                                                const trellis_lora_config_t *cfg)
{
    (void)loop; (void)cfg;
    fprintf(stderr, "[LoRa] transport unavailable "
                    "(build without TRELLIS_WITH_DTN)\n");
    return NULL;
}

#endif /* TRELLIS_WITH_DTN */
