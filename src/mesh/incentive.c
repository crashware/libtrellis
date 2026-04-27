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
 * incentive.c — Relay Incentive System
 *
 * Provides a voluntary, privacy-preserving bandwidth accounting system
 * for Bloom relay operators.  Relays that forward traffic earn signed
 * "bandwidth receipts" from upstream nodes.  These receipts can be
 * optionally redeemed for Lightning micropayments via BTCPay Server.
 *
 * Design goals:
 *   - Privacy-first: receipts do not reveal circuit endpoints
 *   - Voluntary: nodes can run without Lightning integration
 *   - Anti-fraud: receipts are signed by the traffic source with a
 *     cryptographic commitment to the bytes relayed
 *   - Anti-double-spend: receipt serial numbers are monotonic and
 *     gossiped to prevent reuse across nodes
 *
 * Receipt format:
 *   struct bw_receipt {
 *       uint8_t  magic[4];       // "BWRC"
 *       uint8_t  version;        // 1
 *       uint8_t  relay_fp[32];   // fingerprint of the relay being paid
 *       uint64_t bytes_fwd;      // bytes forwarded this window
 *       uint64_t window_start;   // unix ms
 *       uint64_t window_end;     // unix ms
 *       uint32_t serial;         // monotonic receipt number
 *       uint8_t  commitment[32]; // SHA256(relay_fp || bytes || window || rand)
 *       uint8_t  sig[64];        // Ed25519 sig over all above by the payer
 *   };
 *   Total: 4+1+32+8+8+8+4+32+64 = 161 bytes
 *
 * Lightning redemption (optional, compile with -DTRELLIS_WITH_LIGHTNING):
 *   The relay collects receipts over a settlement period, batches them,
 *   and submits to a BTCPay Server instance via HTTP to claim payment.
 *   The relay generates its own BOLT11 invoice via lncli addinvoice and
 *   includes it in the redemption request.  BTCPay validates the receipt
 *   signatures, confirms the claimed amount is fair, and pays the relay's
 *   invoice.  Configuration: --btcpay-url, --btcpay-key, --lncli-path,
 *   --sats-per-mb.
 */

#include "internal.h"
#include <trellis/mesh.h>
#include <trellis/error.h>
#include <sodium.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <errno.h>
#include <stdbool.h>
#include <inttypes.h>
#include <uv.h>
#include "../platform.h"

#define BW_RECEIPT_MAGIC         "BWRC"
#define BW_RECEIPT_VERSION       1
#define BW_RECEIPT_FIXED_LEN     (4 + 1 + 32 + 8 + 8 + 8 + 4 + 32)
#define BW_RECEIPT_SIG_LEN       crypto_sign_ed25519_BYTES  /* 64 */
#define BW_RECEIPT_TOTAL_LEN     (BW_RECEIPT_FIXED_LEN + BW_RECEIPT_SIG_LEN)

// Accounting window: 1 hour.
#define BW_WINDOW_MS             (3600ULL * 1000)

// Minimum bytes to issue a receipt (avoid spam for tiny amounts)
#define BW_MIN_BYTES_FOR_RECEIPT (1024ULL * 1024)  /* 1 MB */

// Maximum receipts to buffer before settlement.
#define BW_MAX_RECEIPTS_BUFFERED 1024

// Maximum peers to track bandwidth for.
#define BW_MAX_TRACKED_PEERS     256

// -----------
// Internal state.
typedef struct bw_peer_entry {
    trellis_fingerprint_t fp;
    bool                  active;
    uint64_t              bytes_recv;    /* bytes received from this peer (we relay for them) */
    uint64_t              bytes_sent;    /* bytes we sent to this peer */
    uint64_t              window_start;
    uint32_t              next_serial;
    uint64_t              last_serial;   /* last accepted receipt serial (anti-double-spend) */
} bw_peer_entry_t;

typedef struct bw_receipt_entry {
    bool    valid;
    uint8_t data[BW_RECEIPT_TOTAL_LEN];
    size_t  data_len;
} bw_receipt_entry_t;

struct trellis_incentive {
    // Identity of this node.
    const trellis_identity_t *local_id;

    // Per-peer bandwidth tracking.
    bw_peer_entry_t  peers[BW_MAX_TRACKED_PEERS];
    size_t           peer_count;

    // Receipts we have issued (as payer)
    bw_receipt_entry_t issued[BW_MAX_RECEIPTS_BUFFERED];
    size_t             issued_count;

    // Receipts we have received (as relay)
    bw_receipt_entry_t received[BW_MAX_RECEIPTS_BUFFERED];
    size_t             received_count;

    // Window settlement timer.
    uv_timer_t   settlement_timer;
    uv_loop_t   *loop;
    bool         timer_active;

    // Lightning BTCPay config (optional)
    char     btcpay_url[512];
    char     btcpay_api_key[256];
    char     lncli_path[512];       /* path to lncli binary */
    uint64_t sats_per_mb;           /* payment rate: sats per megabyte */
    bool     lightning_enabled;

    // Callback when we receive a new receipt.
    trellis_incentive_receipt_cb  receipt_cb;
    void                         *receipt_ctx;

    // Send callback for delivering receipts to peers over the wire.
    trellis_incentive_send_fn     send_fn;
    void                         *send_ctx;
};

static uint64_t incentive_now_ms(void)
{
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    return (uint64_t)ts.tv_sec * 1000 + (uint64_t)ts.tv_nsec / 1000000;
}

static bw_peer_entry_t *incentive_find_peer(trellis_incentive_t *inc,
                                              const trellis_fingerprint_t *fp)
{
    for (size_t i = 0; i < inc->peer_count; i++) {
        if (inc->peers[i].active &&
            trellis_fingerprint_eq(&inc->peers[i].fp, fp))
            return &inc->peers[i];
    }
    return NULL;
}

static bw_peer_entry_t *incentive_get_or_create_peer(
        trellis_incentive_t *inc,
        const trellis_fingerprint_t *fp)
{
    bw_peer_entry_t *e = incentive_find_peer(inc, fp);
    if (e) return e;

    if (inc->peer_count < BW_MAX_TRACKED_PEERS) {
        e = &inc->peers[inc->peer_count++];
        memset(e, 0, sizeof(*e));
        e->fp           = *fp;
        e->active       = true;
        e->window_start = incentive_now_ms();
        e->next_serial  = 1;
        return e;
    }

    // Evict least-recently-used (oldest window_start)
    e = &inc->peers[0];
    for (size_t i = 1; i < inc->peer_count; i++) {
        if (inc->peers[i].window_start < e->window_start)
            e = &inc->peers[i];
    }
    memset(e, 0, sizeof(*e));
    e->fp           = *fp;
    e->active       = true;
    e->window_start = incentive_now_ms();
    e->next_serial  = 1;
    return e;
}

// -----------
// Receipt construction.
static trellis_err_t incentive_build_receipt(
        const trellis_identity_t *payer,
        const trellis_fingerprint_t *relay_fp,
        uint64_t bytes_fwd,
        uint64_t window_start,
        uint64_t window_end,
        uint32_t serial,
        uint8_t *buf_out)
{
    size_t off = 0;

    memcpy(buf_out + off, BW_RECEIPT_MAGIC, 4); off += 4;
    buf_out[off++] = BW_RECEIPT_VERSION;
    memcpy(buf_out + off, relay_fp->bytes, 32); off += 32;

    for (int b = 0; b < 8; b++) buf_out[off++] = (uint8_t)(bytes_fwd    >> (b*8));
    for (int b = 0; b < 8; b++) buf_out[off++] = (uint8_t)(window_start >> (b*8));
    for (int b = 0; b < 8; b++) buf_out[off++] = (uint8_t)(window_end   >> (b*8));
    for (int b = 0; b < 4; b++) buf_out[off++] = (uint8_t)(serial       >> (b*8));

    // Deterministic commitment: SHA256(relay_fp || bytes_fwd || window_start || window_end || serial)
    crypto_hash_sha256_state hs;
    uint8_t commitment[32];
    crypto_hash_sha256_init(&hs);
    crypto_hash_sha256_update(&hs, relay_fp->bytes, 32);
    crypto_hash_sha256_update(&hs, buf_out + 4 + 1 + 32, 8);      /* bytes_fwd */
    crypto_hash_sha256_update(&hs, buf_out + 4 + 1 + 32 + 8, 8);  /* window_start */
    crypto_hash_sha256_update(&hs, buf_out + 4 + 1 + 32 + 16, 8); /* window_end */
    crypto_hash_sha256_update(&hs, buf_out + 4 + 1 + 32 + 24, 4); /* serial */
    crypto_hash_sha256_final(&hs, commitment);
    memcpy(buf_out + off, commitment, 32); off += 32;

    // Signature
    if (crypto_sign_ed25519_detached(buf_out + off, NULL, buf_out, off,
                                      payer->ed25519_sk) != 0)
        return TRELLIS_ERR_SIGN_FAILED;
    off += BW_RECEIPT_SIG_LEN;

    (void)off;
    return TRELLIS_OK;
}

// -----------
// Receipt verification.
trellis_err_t trellis_incentive_verify_receipt(
        const uint8_t *receipt, size_t receipt_len,
        const uint8_t *payer_ed25519_pk,
        uint64_t *bytes_fwd_out,
        trellis_fingerprint_t *relay_fp_out)
{
    if (!receipt || !payer_ed25519_pk)
        return TRELLIS_ERR_INVALID_ARG;
    if (receipt_len < BW_RECEIPT_TOTAL_LEN)
        return TRELLIS_ERR_MSG_FORMAT;
    if (memcmp(receipt, BW_RECEIPT_MAGIC, 4) != 0)
        return TRELLIS_ERR_MSG_FORMAT;
    if (receipt[4] != BW_RECEIPT_VERSION)
        return TRELLIS_ERR_MSG_FORMAT;

    size_t signed_len = BW_RECEIPT_FIXED_LEN;
    const uint8_t *sig = receipt + signed_len;

    if (crypto_sign_ed25519_verify_detached(sig, receipt, signed_len,
                                             payer_ed25519_pk) != 0)
        return TRELLIS_ERR_VERIFY_FAILED;

    // Recompute and verify the deterministic commitment.
    {
        crypto_hash_sha256_state hs;
        uint8_t expected[32];
        crypto_hash_sha256_init(&hs);
        crypto_hash_sha256_update(&hs, receipt + 5, 32);               /* relay_fp */
        crypto_hash_sha256_update(&hs, receipt + 4 + 1 + 32, 8);      /* bytes_fwd */
        crypto_hash_sha256_update(&hs, receipt + 4 + 1 + 32 + 8, 8);  /* window_start */
        crypto_hash_sha256_update(&hs, receipt + 4 + 1 + 32 + 16, 8); /* window_end */
        crypto_hash_sha256_update(&hs, receipt + 4 + 1 + 32 + 24, 4); /* serial */
        crypto_hash_sha256_final(&hs, expected);

        const uint8_t *embedded = receipt + 4 + 1 + 32 + 8 + 8 + 8 + 4;
        if (sodium_memcmp(expected, embedded, 32) != 0)
            return TRELLIS_ERR_VERIFY_FAILED;
    }

    if (relay_fp_out)
        memcpy(relay_fp_out->bytes, receipt + 5, 32);

    if (bytes_fwd_out) {
        uint64_t bfwd = 0;
        size_t off = 4 + 1 + 32;
        for (int b = 0; b < 8; b++)
            bfwd |= ((uint64_t)receipt[off + b]) << (b * 8);
        *bytes_fwd_out = bfwd;
    }

    return TRELLIS_OK;
}

// -----------
// Forward declarations.
trellis_err_t trellis_incentive_redeem(trellis_incentive_t *inc);

// -----------
// Window settlement.
static void incentive_settle_peer(trellis_incentive_t *inc,
                                   bw_peer_entry_t *pe)
{
    if (pe->bytes_recv < BW_MIN_BYTES_FOR_RECEIPT)
        return;

    uint64_t now = incentive_now_ms();

    // Issue a receipt to the peer for bytes we relayed on their behalf.
    if (inc->issued_count >= BW_MAX_RECEIPTS_BUFFERED) {
        fprintf(stderr, "[incentive] receipt buffer full, dropping\n");
        return;
    }

    bw_receipt_entry_t *re = &inc->issued[inc->issued_count];
    trellis_err_t err = incentive_build_receipt(
            inc->local_id, &pe->fp,
            pe->bytes_recv,
            pe->window_start, now,
            pe->next_serial++,
            re->data);
    if (err == TRELLIS_OK) {
        re->valid    = true;
        re->data_len = BW_RECEIPT_TOTAL_LEN;
        inc->issued_count++;
        fprintf(stderr, "[incentive] issued receipt for %" PRIu64 " bytes to peer\n",
                pe->bytes_recv);

        if (inc->send_fn)
            inc->send_fn(inc->send_ctx, &pe->fp, re->data, re->data_len);
    }

    // Reset window.
    pe->bytes_recv    = 0;
    pe->bytes_sent    = 0;
    pe->window_start  = now;
}

static void incentive_settlement_cb(uv_timer_t *handle)
{
    trellis_incentive_t *inc = (trellis_incentive_t *)handle->data;

    for (size_t i = 0; i < inc->peer_count; i++) {
        if (inc->peers[i].active)
            incentive_settle_peer(inc, &inc->peers[i]);
    }

    if (inc->lightning_enabled && inc->received_count > 0)
        trellis_incentive_redeem(inc);
}

// -----------
// Public API.
trellis_incentive_t *trellis_incentive_new(const trellis_identity_t *local_id,
                                            uv_loop_t *loop)
{
    if (!local_id || !loop)
        return NULL;

    trellis_incentive_t *inc = calloc(1, sizeof(*inc));
    if (!inc) return NULL;

    inc->local_id = local_id;
    inc->loop     = loop;

    uv_timer_init(loop, &inc->settlement_timer);
    inc->settlement_timer.data = inc;
    uv_timer_start(&inc->settlement_timer, incentive_settlement_cb,
                   BW_WINDOW_MS, BW_WINDOW_MS);
    inc->timer_active = true;

    return inc;
}

void trellis_incentive_free(trellis_incentive_t *inc)
{
    if (!inc) return;
    if (inc->timer_active) {
        uv_timer_stop(&inc->settlement_timer);
        inc->timer_active = false;
    }
    free(inc);
}

/*
 * Record bytes relayed on behalf of a peer (called by relay logic).
 */
void trellis_incentive_record_relay(trellis_incentive_t *inc,
                                     const trellis_fingerprint_t *peer_fp,
                                     uint64_t bytes)
{
    if (!inc || !peer_fp) return;
    bw_peer_entry_t *pe = incentive_get_or_create_peer(inc, peer_fp);
    if (pe) pe->bytes_recv += bytes;
}

/*
 * Record bytes we consumed through a relay (called by client logic).
 */
void trellis_incentive_record_consume(trellis_incentive_t *inc,
                                       const trellis_fingerprint_t *relay_fp,
                                       uint64_t bytes)
{
    if (!inc || !relay_fp) return;
    bw_peer_entry_t *pe = incentive_get_or_create_peer(inc, relay_fp);
    if (pe) pe->bytes_sent += bytes;
}

/*
 * Accept an incoming bandwidth receipt from a payer.
 * Verifies the receipt and stores it for potential redemption.
 */
trellis_err_t trellis_incentive_accept_receipt(
        trellis_incentive_t *inc,
        const uint8_t *receipt, size_t receipt_len,
        const uint8_t *payer_ed25519_pk)
{
    if (!inc || !receipt || !payer_ed25519_pk)
        return TRELLIS_ERR_INVALID_ARG;

    uint64_t bytes_fwd;
    trellis_fingerprint_t relay_fp;
    trellis_err_t err = trellis_incentive_verify_receipt(
            receipt, receipt_len, payer_ed25519_pk,
            &bytes_fwd, &relay_fp);
    if (err != TRELLIS_OK)
        return err;

    // Verify it's addressed to us.
    if (!trellis_fingerprint_eq(&relay_fp, &inc->local_id->fingerprint)) {
        fprintf(stderr, "[incentive] receipt not addressed to us\n");
        return TRELLIS_ERR_PERMISSION;
    }

    // Extract serial and check for double-spend (monotonic per payer)
    uint32_t serial = 0;
    {
        size_t soff = 4 + 1 + 32 + 8 + 8 + 8; /* offset of serial field */
        for (int b = 0; b < 4; b++)
            serial |= ((uint32_t)receipt[soff + b]) << (b * 8);
    }

    // Derive payer fingerprint from their ed25519 public key.
    trellis_fingerprint_t payer_fp;
    crypto_hash_sha256(payer_fp.bytes, payer_ed25519_pk, 32);

    bw_peer_entry_t *payer_pe = incentive_get_or_create_peer(inc, &payer_fp);
    if (payer_pe) {
        if ((uint64_t)serial <= payer_pe->last_serial) {
            fprintf(stderr, "[incentive] receipt serial %" PRIu32 " <= last %" PRIu64
                    ", rejecting (double-spend)\n", serial, payer_pe->last_serial);
            return TRELLIS_ERR_PERMISSION;
        }
        payer_pe->last_serial = (uint64_t)serial;
    }

    if (inc->received_count >= BW_MAX_RECEIPTS_BUFFERED) {
        fprintf(stderr, "[incentive] received receipt buffer full\n");
        return TRELLIS_ERR_FULL;
    }

    bw_receipt_entry_t *re = &inc->received[inc->received_count];
    size_t copy = receipt_len < BW_RECEIPT_TOTAL_LEN ? receipt_len : BW_RECEIPT_TOTAL_LEN;
    memcpy(re->data, receipt, copy);
    re->data_len = copy;
    re->valid    = true;
    inc->received_count++;

    fprintf(stderr, "[incentive] accepted receipt for %" PRIu64 " bytes\n",
            bytes_fwd);

    if (inc->receipt_cb)
        inc->receipt_cb(receipt, receipt_len, bytes_fwd, inc->receipt_ctx);

    return TRELLIS_OK;
}

/*
 * Configure Lightning/BTCPay integration for receipt redemption.
 */
void trellis_incentive_set_lightning(
        trellis_incentive_t *inc,
        const char *btcpay_url,
        const char *btcpay_api_key,
        const char *lncli_path,
        uint64_t sats_per_mb)
{
    if (!inc) return;
    if (btcpay_url) {
        strncpy(inc->btcpay_url, btcpay_url, sizeof(inc->btcpay_url) - 1);
        inc->lightning_enabled = true;
    }
    if (btcpay_api_key) {
        strncpy(inc->btcpay_api_key, btcpay_api_key,
                sizeof(inc->btcpay_api_key) - 1);
    }
    strncpy(inc->lncli_path,
            (lncli_path && lncli_path[0]) ? lncli_path : "lncli",
            sizeof(inc->lncli_path) - 1);
    inc->sats_per_mb = sats_per_mb > 0 ? sats_per_mb : 1;

    fprintf(stderr, "[incentive] Lightning redemption configured: %s "
            "(lncli=%s, rate=%" PRIu64 " sat/MB)\n",
            btcpay_url ? btcpay_url : "(disabled)",
            inc->lncli_path, inc->sats_per_mb);
}

/* Shell-escape a string for use inside a single-quoted shell argument.
 * Replaces every ' with '\'', making the result safe for 'value'.
 * Caller must free the returned pointer. */
static char *shell_sq_escape(const char *src)
{
    if (!src) return NULL;
    size_t src_len = strlen(src);
    if (src_len > (SIZE_MAX - 1) / 4) return NULL;
    char *buf = malloc(src_len * 4 + 1);
    if (!buf) return NULL;
    const char *p = src;
    char *d = buf;
    while (*p) {
        if (*p == '\'') {
            *d++ = '\''; *d++ = '\\'; *d++ = '\''; *d++ = '\'';
        } else {
            *d++ = *p;
        }
        p++;
    }
    *d = '\0';
    return buf;
}

/*
 * Attempt to redeem all buffered receipts via BTCPay (if configured).
 *
 * Relay-generated-invoice flow:
 *   1. Sum bytes_fwd across all unredeemed receipts, convert to sats
 *   2. Create a local BOLT11 invoice via lncli addinvoice (relay is payee)
 *   3. POST receipts + invoice to BTCPay /api/v1/stores/{storeId}/...
 *   4. BTCPay validates receipt signatures, verifies amounts, pays invoice
 *   5. Confirm settlement via lncli lookupinvoice, mark receipts redeemed
 *
 * Uses `curl` and `lncli` subprocesses for HTTP and LN operations
 * (avoids external C library dependencies). For production environments,
 * replace with libcurl direct linkage and LND gRPC bindings.
 */
trellis_err_t trellis_incentive_redeem(trellis_incentive_t *inc)
{
    if (!inc) return TRELLIS_ERR_INVALID_ARG;
    if (!inc->lightning_enabled) {
        fprintf(stderr, "[incentive] Lightning not configured, cannot redeem\n");
        return TRELLIS_ERR_NOT_INITIALIZED;
    }
    if (inc->received_count == 0) {
        fprintf(stderr, "[incentive] no receipts to redeem\n");
        return TRELLIS_OK;
    }

    fprintf(stderr, "[incentive] attempting to redeem %zu receipts via %s\n",
            inc->received_count, inc->btcpay_url);

#ifdef TRELLIS_WITH_LIGHTNING
    /*
     * Step A: Calculate total bytes across all valid receipts and convert
     *         to a sat amount using the configured rate.
     */
    uint64_t total_bytes = 0;
    for (size_t i = 0; i < inc->received_count; i++) {
        if (!inc->received[i].valid || inc->received[i].data_len == 0)
            continue;
        const uint8_t *r = inc->received[i].data;
        uint64_t bfwd = 0;
        size_t boff = 4 + 1 + 32; /* offset of bytes_fwd in receipt */
        for (int b = 0; b < 8; b++)
            bfwd |= ((uint64_t)r[boff + b]) << (b * 8);
        total_bytes += bfwd;
    }

    uint64_t sats = (total_bytes / (1024ULL * 1024)) * inc->sats_per_mb;
    if (sats == 0) sats = 1; /* minimum 1 sat */

    fprintf(stderr, "[incentive] total bytes: %" PRIu64 ", requesting %" PRIu64
            " sats (%" PRIu64 " sat/MB)\n", total_bytes, sats, inc->sats_per_mb);

    /*
     * Step B: Create a local BOLT11 invoice via lncli addinvoice.
     *         The relay is the payee — BTCPay will pay this invoice.
     */
    char *escaped_lncli = shell_sq_escape(inc->lncli_path);
    if (!escaped_lncli) return TRELLIS_ERR_NOMEM;

    char inv_cmd[1024];
    (void)snprintf(inv_cmd, sizeof(inv_cmd),
                  "'%s' addinvoice --amt %" PRIu64
                  " --memo 'bloom-relay-redeem' 2>&1",
                  escaped_lncli, sats);
    free(escaped_lncli);

    FILE *inv_fp = trellis_popen(inv_cmd, "r");
    if (!inv_fp) {
        fprintf(stderr, "[incentive] lncli not found (%s); "
                        "cannot create invoice. Earned %" PRIu64 " sats for %"
                        PRIu64 " bytes — redeem manually.\n",
                inc->lncli_path, sats, total_bytes);
        return TRELLIS_OK;
    }

    char inv_resp[4096];
    size_t inv_resp_len = fread(inv_resp, 1, sizeof(inv_resp) - 1, inv_fp);
    inv_resp[inv_resp_len] = '\0';
    int inv_exit = trellis_pclose(inv_fp);

    if (inv_exit != 0) {
        fprintf(stderr, "[incentive] lncli addinvoice failed (exit=%d): %.256s\n",
                inv_exit, inv_resp);
        return TRELLIS_ERR_TRANSPORT;
    }

    // Extract payment_request (BOLT11 string) from JSON response.
    const char *pr_key = strstr(inv_resp, "\"payment_request\"");
    if (!pr_key) {
        fprintf(stderr, "[incentive] addinvoice response missing payment_request: "
                "%.256s\n", inv_resp);
        return TRELLIS_ERR_TRANSPORT;
    }
    const char *pr_start = strchr(pr_key + 17, '"');
    if (!pr_start) return TRELLIS_ERR_TRANSPORT;
    pr_start++;
    const char *pr_end = strchr(pr_start, '"');
    if (!pr_end || pr_end == pr_start) return TRELLIS_ERR_TRANSPORT;

    size_t bolt11_len = (size_t)(pr_end - pr_start);
    if (bolt11_len > 2048) return TRELLIS_ERR_TRANSPORT;

    char bolt11[2049];
    memcpy(bolt11, pr_start, bolt11_len);
    bolt11[bolt11_len] = '\0';

    // Extract r_hash for settlement confirmation.
    const char *rh_key = strstr(inv_resp, "\"r_hash\"");
    char r_hash[128] = {0};
    if (rh_key) {
        const char *rh_start = strchr(rh_key + 8, '"');
        if (rh_start) {
            rh_start++;
            const char *rh_end = strchr(rh_start, '"');
            if (rh_end && (size_t)(rh_end - rh_start) < sizeof(r_hash))
                memcpy(r_hash, rh_start, (size_t)(rh_end - rh_start));
        }
    }

    fprintf(stderr, "[incentive] created BOLT11 invoice for %" PRIu64 " sats "
            "(len=%zu)\n", sats, bolt11_len);

    /*
     * Step C: Serialize receipts as JSON and POST to BTCPay along with
     *         our invoice. BTCPay verifies receipts and pays the invoice.
     *
     * Payload: {"receipts":["hex",...], "invoice":"<bolt11>"}
     */
    size_t json_buf_sz = 64 + bolt11_len + 16
                         + inc->received_count * (161 * 2 + 6);
    char *json_buf = malloc(json_buf_sz);
    if (!json_buf)
        return TRELLIS_ERR_NOMEM;

    size_t off = 0;
    off += (size_t)snprintf(json_buf + off, json_buf_sz - off,
                            "{\"receipts\":[");

    bool any = false;
    for (size_t i = 0; i < inc->received_count && off + 400 < json_buf_sz; i++) {
        const uint8_t *r = inc->received[i].data;
        size_t rlen     = inc->received[i].data_len;
        if (!inc->received[i].valid || rlen == 0) continue;
        if (any) json_buf[off++] = ',';
        json_buf[off++] = '"';
        for (size_t b = 0; b < rlen && off + 4 < json_buf_sz; b++) {
            off += (size_t)snprintf(json_buf + off, json_buf_sz - off,
                                    "%02x", r[b]);
        }
        json_buf[off++] = '"';
        any = true;
    }
    off += (size_t)snprintf(json_buf + off, json_buf_sz - off,
                            "],\"invoice\":\"%s\"}", bolt11);

    char redeem_url[512];
    snprintf(redeem_url, sizeof(redeem_url),
             "%s/api/v1/stores/bloom/receipts/redeem",
             inc->btcpay_url);

    char *escaped_json = shell_sq_escape(json_buf);
    if (!escaped_json) { free(json_buf); return TRELLIS_ERR_NOMEM; }

    char *escaped_key = shell_sq_escape(inc->btcpay_api_key);
    if (!escaped_key) { free(escaped_json); free(json_buf); return TRELLIS_ERR_NOMEM; }

    char *escaped_redeem = shell_sq_escape(redeem_url);
    if (!escaped_redeem) {
        free(escaped_key);
        free(escaped_json);
        free(json_buf);
        return TRELLIS_ERR_NOMEM;
    }

    size_t cmd_sz = 128 + strlen(escaped_key) + strlen(escaped_json)
                    + strlen(escaped_redeem);
    char *cmd = malloc(cmd_sz);
    if (!cmd) {
        free(escaped_redeem);
        free(escaped_key);
        free(escaped_json);
        free(json_buf);
        return TRELLIS_ERR_NOMEM;
    }
    (void)snprintf(cmd, cmd_sz,
                  "curl -s -X POST "
                  "-H 'Authorization: Bearer %s' "
                  "-H 'Content-Type: application/json' "
                  "-d '%s' '%s' 2>&1",
                  escaped_key, escaped_json, escaped_redeem);
    free(escaped_redeem);
    free(escaped_key);
    free(escaped_json);

    FILE *fp = trellis_popen(cmd, "r");
    free(cmd);
    if (!fp) {
        fprintf(stderr, "[incentive] popen(curl) failed: %s\n", strerror(errno));
        free(json_buf);
        return TRELLIS_ERR_TRANSPORT;
    }

    char response[4096];
    size_t resp_len = fread(response, 1, sizeof(response) - 1, fp);
    response[resp_len] = '\0';
    int exit_code = trellis_pclose(fp);
    free(json_buf);

    if (exit_code != 0) {
        fprintf(stderr, "[incentive] BTCPay redemption failed (exit=%d): %.256s\n",
                exit_code, response);
        return TRELLIS_ERR_TRANSPORT;
    }

    /*
     * Step D: Confirm settlement. Check BTCPay response for immediate
     *         confirmation, or poll lncli lookupinvoice with the r_hash.
     */
    bool settled = (strstr(response, "\"status\"") &&
                    strstr(response, "\"paid\""));

    if (!settled && r_hash[0]) {
        char *escaped_lncli2 = shell_sq_escape(inc->lncli_path);
        char *escaped_rhash  = shell_sq_escape(r_hash);
        if (escaped_lncli2 && escaped_rhash) {
            for (int attempt = 0; attempt < 3 && !settled; attempt++) {
                if (attempt > 0) {
                    trellis_sleep_ms(2000);
                }

                char lookup_cmd[1024];
                (void)snprintf(lookup_cmd, sizeof(lookup_cmd),
                              "'%s' lookupinvoice --rhash '%s' 2>&1",
                              escaped_lncli2, escaped_rhash);

                FILE *lk_fp = trellis_popen(lookup_cmd, "r");
                if (!lk_fp) break;

                char lk_resp[2048];
                size_t lk_len = fread(lk_resp, 1, sizeof(lk_resp) - 1, lk_fp);
                lk_resp[lk_len] = '\0';
                trellis_pclose(lk_fp);

                if (strstr(lk_resp, "\"SETTLED\"") ||
                    strstr(lk_resp, "\"settled\""))
                    settled = true;
            }
        }
        free(escaped_lncli2);
        free(escaped_rhash);
    }

    /*
     * Step E: Mark receipts as consumed on successful settlement.
     */
    if (settled) {
        fprintf(stderr, "[incentive] invoice settled; %" PRIu64 " sats received, "
                        "marking %zu receipts redeemed\n", sats, inc->received_count);
        for (size_t i = 0; i < inc->received_count; i++) {
            inc->received[i].valid    = false;
            inc->received[i].data_len = 0;
        }
        inc->received_count = 0;
    } else {
        fprintf(stderr, "[incentive] BTCPay accepted receipts but invoice not yet "
                "settled — will retry on next cycle. Response: %.256s\n",
                response);
    }
#else
    fprintf(stderr, "[incentive] build without TRELLIS_WITH_LIGHTNING; "
                    "cannot redeem receipts\n");
#endif

    return TRELLIS_OK;
}

/*
 * Get the total bytes relayed this window (for display/metrics).
 */
uint64_t trellis_incentive_total_relayed(const trellis_incentive_t *inc)
{
    if (!inc) return 0;
    uint64_t total = 0;
    for (size_t i = 0; i < inc->peer_count; i++) {
        if (inc->peers[i].active)
            total += inc->peers[i].bytes_recv;
    }
    return total;
}

/*
 * Set callback for incoming receipts.
 */
void trellis_incentive_set_receipt_cb(trellis_incentive_t *inc,
                                       trellis_incentive_receipt_cb cb,
                                       void *ctx)
{
    if (!inc) return;
    inc->receipt_cb  = cb;
    inc->receipt_ctx = ctx;
}

void trellis_incentive_set_send(trellis_incentive_t *inc,
                                 trellis_incentive_send_fn fn,
                                 void *ctx)
{
    if (!inc) return;
    inc->send_fn  = fn;
    inc->send_ctx = ctx;
}
