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
 * deadman.c — Dead Man's Switch for Greenhouse Services
 *
 * A greenhouse operator can configure a dead man's switch (DMS) that
 * automatically delivers pre-encrypted alarm messages to designated
 * recipients if the operator fails to provide a periodic "I'm alive"
 * heartbeat.
 *
 * Use cases:
 *   - Journalists: If arrested, pre-staged documents are automatically
 *     released to trusted contacts.
 *   - Activists: Proof-of-life messages trigger emergency contacts.
 *   - Service operators: Alert admins if the service goes dark.
 *
 * Architecture:
 *   1. Setup: Operator pre-encrypts N alarm messages to N recipients
 *      using their KEM public keys (ML-KEM-1024 sealed messages).
 *   2. The DMS stores these encrypted blobs in the DHT under keys:
 *        SHA256("trellis_dms_alarm" || service_fp || recipient_fp)
 *   3. A heartbeat record is published every HEARTBEAT_INTERVAL_MS:
 *        SHA256("trellis_dms_hb" || service_fp) → signed timestamp
 *   4. Clients watching for a specific DMS poll the heartbeat key.
 *      If the heartbeat has not been refreshed in HEARTBEAT_MAX_AGE_MS,
 *      they fetch and decrypt their designated alarm message.
 *
 * Heartbeat record:
 *   [4]  magic "DMSH"
 *   [32] service fingerprint
 *   [8]  timestamp_ms (little-endian)
 *   [8]  next_heartbeat_ms (little-endian)
 *   [64] Ed25519 signature over above
 *
 * Alarm envelope (per recipient):
 *   [4]  magic "DMSA"
 *   [32] service fingerprint
 *   [32] recipient fingerprint
 *   [1568] ML-KEM-1024 ciphertext (encaps of the content key)
 *   [24] XSalsa20-Poly1305 nonce (crypto_secretbox_NONCEBYTES = 24)
 *   [N+16] encrypted payload (arbitrary size, N bytes + 16 MAC)
 */

#include "internal.h"
#include <trellis/greenhouse.h>
#include <trellis/mesh.h>
#include <trellis/error.h>
#include <sodium.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <time.h>
#include <inttypes.h>
#include <uv.h>
#include <oqs/oqs.h>

// Constants
#define DMS_HB_MAGIC             "DMSH"
#define DMS_ALARM_MAGIC          "DMSA"
#define DMS_HB_FIXED_LEN         (4 + 32 + 8 + 8)        /* magic+fp+ts+next */
#define DMS_HB_SIG_LEN           crypto_sign_ed25519_BYTES /* 64 */
#define DMS_HB_TOTAL_LEN         (DMS_HB_FIXED_LEN + DMS_HB_SIG_LEN)

#define DMS_ALARM_FIXED_LEN      (4 + 32 + 32)            /* magic+svc_fp+rcpt_fp */
#define DMS_KEM_CT_LEN           TRELLIS_ML_KEM_1024_CT_LEN /* 1568 */
#define DMS_NONCE_LEN            crypto_secretbox_NONCEBYTES /* 24 */
#define DMS_MAC_LEN              crypto_secretbox_MACBYTES   /* 16 */

// How long a heartbeat is valid.
#define DMS_HB_MAX_AGE_MS        (72ULL * 60 * 60 * 1000) /* 3 days */

// How often to auto-publish heartbeat.
#define DMS_HB_INTERVAL_MS       (48ULL * 60 * 60 * 1000) /* 2 days */

// Maximum recipients per DMS instance.
#define DMS_MAX_RECIPIENTS       16

// Maximum pre-encrypted alarm payload size.
#define DMS_MAX_ALARM_PAYLOAD    65536

typedef struct dms_recipient {
    trellis_fingerprint_t fp;
    uint8_t               ml_kem_pk[TRELLIS_ML_KEM_1024_PK_LEN];
    uint8_t              *alarm_ct;      /* encrypted alarm ciphertext */
    size_t                alarm_ct_len;
} dms_recipient_t;

typedef struct trellis_dms {
    trellis_dht_t        *dht;
    trellis_identity_t   *service_id;

    dms_recipient_t       recipients[DMS_MAX_RECIPIENTS];
    size_t                recipient_count;

    uv_loop_t            *loop;
    uv_timer_t            hb_timer;
    bool                  timer_active;

    bool                  armed;
} trellis_dms_t;

static uint64_t dms_now_ms(void)
{
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    return (uint64_t)ts.tv_sec * 1000 + (uint64_t)ts.tv_nsec / 1000000;
}

static void dms_hb_dht_key(const trellis_fingerprint_t *svc_fp,
                             uint8_t *key_out)
{
    static const char prefix[] = "trellis_dms_hb";
    crypto_hash_sha256_state hs;
    crypto_hash_sha256_init(&hs);
    crypto_hash_sha256_update(&hs, (const uint8_t *)prefix, strlen(prefix));
    crypto_hash_sha256_update(&hs, svc_fp->bytes, TRELLIS_FINGERPRINT_LEN);
    crypto_hash_sha256_final(&hs, key_out);
}

static void dms_alarm_dht_key(const trellis_fingerprint_t *svc_fp,
                                const trellis_fingerprint_t *rcpt_fp,
                                uint8_t *key_out)
{
    static const char prefix[] = "trellis_dms_alarm";
    crypto_hash_sha256_state hs;
    crypto_hash_sha256_init(&hs);
    crypto_hash_sha256_update(&hs, (const uint8_t *)prefix, strlen(prefix));
    crypto_hash_sha256_update(&hs, svc_fp->bytes, TRELLIS_FINGERPRINT_LEN);
    crypto_hash_sha256_update(&hs, rcpt_fp->bytes, TRELLIS_FINGERPRINT_LEN);
    crypto_hash_sha256_final(&hs, key_out);
}

// Heartbeat publication.
static trellis_err_t dms_publish_heartbeat(trellis_dms_t *dms)
{
    uint8_t buf[DMS_HB_TOTAL_LEN];
    memset(buf, 0, sizeof(buf));

    size_t off = 0;
    memcpy(buf + off, DMS_HB_MAGIC, 4); off += 4;
    memcpy(buf + off, dms->service_id->fingerprint.bytes, 32); off += 32;

    uint64_t now_ms  = dms_now_ms();
    uint64_t next_ms = now_ms + DMS_HB_INTERVAL_MS;
    for (int b = 0; b < 8; b++) buf[off++] = (uint8_t)(now_ms  >> (b * 8));
    for (int b = 0; b < 8; b++) buf[off++] = (uint8_t)(next_ms >> (b * 8));

    // Sign the heartbeat.
    if (crypto_sign_ed25519_detached(buf + off, NULL, buf, off,
                                      dms->service_id->ed25519_sk) != 0)
        return TRELLIS_ERR_SIGN_FAILED;
    off += DMS_HB_SIG_LEN;

    uint8_t dht_key[32];
    dms_hb_dht_key(&dms->service_id->fingerprint, dht_key);

    fprintf(stderr, "[DMS] publishing heartbeat\n");
    return trellis_dht_store(dms->dht, dht_key, sizeof(dht_key),
                              buf, off, NULL, NULL);
}

static void dms_hb_timer_cb(uv_timer_t *handle)
{
    trellis_dms_t *dms = (trellis_dms_t *)handle->data;
    dms_publish_heartbeat(dms);
}

// Alarm envelope construction.

/*
 * Encrypt `message` for `recipient_ml_kem_pk` and store in the DHT.
 *
 * Encryption:
 *   1. Encapsulate a 32-byte content key using ML-KEM-1024:
 *        (ct, ss) = ML-KEM.Encaps(ml_kem_pk)
 *   2. Derive a 32-byte symmetric key: sym_key = HKDF(ss, "dms_alarm_key")
 *   3. Encrypt message with XSalsa20Poly1305:
 *        ciphertext = crypto_secretbox(message, nonce, sym_key)
 *   4. Envelope = magic || svc_fp || rcpt_fp || ct || nonce || ciphertext
 */
static trellis_err_t dms_encrypt_alarm(
        const trellis_fingerprint_t *svc_fp,
        const trellis_fingerprint_t *rcpt_fp,
        const uint8_t *ml_kem_pk,
        const uint8_t *message, size_t message_len,
        uint8_t **out, size_t *out_len)
{
    if (!svc_fp || !rcpt_fp || !ml_kem_pk || !message || !out || !out_len)
        return TRELLIS_ERR_INVALID_ARG;
    if (message_len > DMS_MAX_ALARM_PAYLOAD)
        return TRELLIS_ERR_INVALID_ARG;

    // ML-KEM-1024 encapsulation.
    uint8_t ct[TRELLIS_ML_KEM_1024_CT_LEN];
    uint8_t ss[TRELLIS_ML_KEM_1024_SS_LEN];

    OQS_KEM *kem = OQS_KEM_new(OQS_KEM_alg_ml_kem_1024);
    if (!kem) return TRELLIS_ERR_CRYPTO_INIT;

    if (OQS_KEM_encaps(kem, ct, ss, ml_kem_pk) != OQS_SUCCESS) {
        OQS_KEM_free(kem);
        return TRELLIS_ERR_KEM_ENCAPS;
    }
    OQS_KEM_free(kem);

    // Derive symmetric key from shared secret.
    uint8_t sym_key[crypto_secretbox_KEYBYTES];
    static const char hkdf_label[] = "trellis_dms_alarm_sym_key";
    crypto_auth_hmacsha256_state hs;
    crypto_auth_hmacsha256_init(&hs, ss, TRELLIS_ML_KEM_1024_SS_LEN);
    crypto_auth_hmacsha256_update(&hs, (const uint8_t *)hkdf_label,
                                   strlen(hkdf_label));
    // Pad output to 32 bytes.
    uint8_t hmac_out[32];
    crypto_auth_hmacsha256_final(&hs, hmac_out);
    memcpy(sym_key, hmac_out, crypto_secretbox_KEYBYTES);

    // Nonce
    uint8_t nonce[DMS_NONCE_LEN];
    randombytes_buf(nonce, DMS_NONCE_LEN);

    // Encrypt
    size_t ct_len = message_len + DMS_MAC_LEN;
    uint8_t *ciphertext = malloc(ct_len);
    if (!ciphertext) return TRELLIS_ERR_NOMEM;

    if (crypto_secretbox_easy(ciphertext, message, message_len,
                               nonce, sym_key) != 0) {
        sodium_memzero(ss, sizeof(ss));
        sodium_memzero(sym_key, sizeof(sym_key));
        free(ciphertext);
        return TRELLIS_ERR_ENCRYPT;
    }
    sodium_memzero(ss, sizeof(ss));
    sodium_memzero(sym_key, sizeof(sym_key));

    // Build envelope.
    size_t env_len = DMS_ALARM_FIXED_LEN + DMS_KEM_CT_LEN +
                     DMS_NONCE_LEN + ct_len;
    uint8_t *env = malloc(env_len);
    if (!env) { free(ciphertext); return TRELLIS_ERR_NOMEM; }

    size_t off = 0;
    memcpy(env + off, DMS_ALARM_MAGIC, 4);          off += 4;
    memcpy(env + off, svc_fp->bytes, 32);           off += 32;
    memcpy(env + off, rcpt_fp->bytes, 32);          off += 32;
    memcpy(env + off, ct, DMS_KEM_CT_LEN);          off += DMS_KEM_CT_LEN;
    memcpy(env + off, nonce, DMS_NONCE_LEN);        off += DMS_NONCE_LEN;
    memcpy(env + off, ciphertext, ct_len);          off += ct_len;
    free(ciphertext);

    *out     = env;
    *out_len = off;
    return TRELLIS_OK;
}

// Public API.

/*
 * Create a new Dead Man's Switch instance.
 */
trellis_dms_t *trellis_dms_new(trellis_dht_t *dht,
                                 trellis_identity_t *service_id)
{
    if (!dht || !service_id)
        return NULL;

    trellis_dms_t *dms = calloc(1, sizeof(*dms));
    if (!dms) return NULL;

    dms->dht        = dht;
    dms->service_id = service_id;
    dms->armed      = false;
    return dms;
}

void trellis_dms_free(trellis_dms_t *dms)
{
    if (!dms) return;
    if (dms->timer_active) {
        uv_timer_stop(&dms->hb_timer);
        dms->timer_active = false;
    }
    for (size_t i = 0; i < dms->recipient_count; i++)
        free(dms->recipients[i].alarm_ct);
    free(dms);
}

/*
 * Add a recipient and pre-encrypt an alarm message for them.
 *
 * @dms          dead man's switch instance
 * @rcpt_fp      recipient fingerprint
 * @rcpt_pk      recipient ML-KEM-1024 public key (TRELLIS_ML_KEM_1024_PK_LEN bytes)
 * @message      plaintext alarm message
 * @message_len  length of message
 */
trellis_err_t trellis_dms_add_recipient(
        trellis_dms_t *dms,
        const trellis_fingerprint_t *rcpt_fp,
        const uint8_t *rcpt_pk,
        const uint8_t *message, size_t message_len)
{
    if (!dms || !rcpt_fp || !rcpt_pk || !message)
        return TRELLIS_ERR_INVALID_ARG;
    if (dms->recipient_count >= DMS_MAX_RECIPIENTS)
        return TRELLIS_ERR_FULL;

    dms_recipient_t *r = &dms->recipients[dms->recipient_count];
    r->fp = *rcpt_fp;
    memcpy(r->ml_kem_pk, rcpt_pk, TRELLIS_ML_KEM_1024_PK_LEN);

    trellis_err_t err = dms_encrypt_alarm(
            &dms->service_id->fingerprint, rcpt_fp, rcpt_pk,
            message, message_len,
            &r->alarm_ct, &r->alarm_ct_len);
    if (err != TRELLIS_OK)
        return err;

    dms->recipient_count++;
    return TRELLIS_OK;
}

/*
 * Arm the DMS: publish all encrypted alarm envelopes to the DHT and
 * start the heartbeat timer.
 *
 * @dms   the DMS instance
 * @loop  libuv event loop for heartbeat timer
 */
trellis_err_t trellis_dms_arm(trellis_dms_t *dms, uv_loop_t *loop)
{
    if (!dms || !loop)
        return TRELLIS_ERR_INVALID_ARG;
    if (dms->recipient_count == 0)
        return TRELLIS_ERR_INVALID_ARG;

    // Publish alarm envelopes to DHT.
    for (size_t i = 0; i < dms->recipient_count; i++) {
        dms_recipient_t *r = &dms->recipients[i];
        if (!r->alarm_ct) continue;

        uint8_t dht_key[32];
        dms_alarm_dht_key(&dms->service_id->fingerprint, &r->fp, dht_key);

        trellis_err_t err = trellis_dht_store(
                dms->dht, dht_key, sizeof(dht_key),
                r->alarm_ct, r->alarm_ct_len, NULL, NULL);
        if (err != TRELLIS_OK) {
            fprintf(stderr, "[DMS] failed to publish alarm for recipient %zu\n", i);
        }
    }

    // Publish initial heartbeat.
    trellis_err_t err = dms_publish_heartbeat(dms);
    if (err != TRELLIS_OK)
        return err;

    // Start heartbeat timer.
    dms->loop = loop;
    uv_timer_init(loop, &dms->hb_timer);
    dms->hb_timer.data = dms;
    uv_timer_start(&dms->hb_timer, dms_hb_timer_cb,
                   DMS_HB_INTERVAL_MS, DMS_HB_INTERVAL_MS);
    dms->timer_active = true;
    dms->armed        = true;

    fprintf(stderr, "[DMS] armed with %zu recipients\n", dms->recipient_count);
    return TRELLIS_OK;
}

/*
 * Disarm the DMS: stop the heartbeat timer and remove alarm envelopes
 * from the DHT (best-effort).
 */
trellis_err_t trellis_dms_disarm(trellis_dms_t *dms)
{
    if (!dms || !dms->armed)
        return TRELLIS_OK;

    if (dms->timer_active) {
        uv_timer_stop(&dms->hb_timer);
        dms->timer_active = false;
    }

    // Remove alarm envelopes.
    for (size_t i = 0; i < dms->recipient_count; i++) {
        uint8_t dht_key[32];
        dms_alarm_dht_key(&dms->service_id->fingerprint,
                           &dms->recipients[i].fp, dht_key);
        trellis_dht_store(dms->dht, dht_key, sizeof(dht_key),
                           NULL, 0, NULL, NULL);
    }

    // Remove heartbeat.
    uint8_t hb_key[32];
    dms_hb_dht_key(&dms->service_id->fingerprint, hb_key);
    trellis_dht_store(dms->dht, hb_key, sizeof(hb_key), NULL, 0, NULL, NULL);

    dms->armed = false;
    fprintf(stderr, "[DMS] disarmed\n");
    return TRELLIS_OK;
}

/*
 * Manual heartbeat renewal (called by the operator to prove they're alive).
 * Usually the timer handles this automatically, but can be called explicitly.
 */
trellis_err_t trellis_dms_heartbeat(trellis_dms_t *dms)
{
    if (!dms || !dms->armed)
        return TRELLIS_ERR_NOT_INITIALIZED;
    return dms_publish_heartbeat(dms);
}

// Watcher: recipient side.
typedef struct dms_watch_ctx {
    trellis_fingerprint_t  svc_fp;
    trellis_fingerprint_t  my_fp;
    uint8_t                my_ml_kem_sk[TRELLIS_ML_KEM_1024_SK_LEN];
    trellis_dms_alarm_cb   cb;
    void                  *ctx;
    trellis_dht_t         *dht;
    uv_timer_t             poll_timer;
    bool                   timer_active;
} dms_watch_ctx_t;

static void on_hb_fetched(const uint8_t *value, size_t value_len,
                            trellis_err_t err, void *raw_ctx);
static void on_alarm_fetched(const uint8_t *value, size_t value_len,
                               trellis_err_t err, void *raw_ctx);

static void dms_watch_poll_cb(uv_timer_t *handle)
{
    dms_watch_ctx_t *wc = (dms_watch_ctx_t *)handle->data;

    uint8_t hb_key[32];
    dms_hb_dht_key(&wc->svc_fp, hb_key);

    trellis_dht_find_value(wc->dht, hb_key, sizeof(hb_key),
                            on_hb_fetched, wc);
}

static void on_hb_fetched(const uint8_t *value, size_t value_len,
                            trellis_err_t err, void *raw_ctx)
{
    dms_watch_ctx_t *wc = (dms_watch_ctx_t *)raw_ctx;

    if (err == TRELLIS_OK && value && value_len >= DMS_HB_TOTAL_LEN) {
        // Verify magic.
        if (memcmp(value, DMS_HB_MAGIC, 4) != 0) goto trigger;

        // Extract timestamp.
        uint64_t ts_ms = 0;
        size_t off = 4 + 32;
        for (int b = 0; b < 8; b++)
            ts_ms |= ((uint64_t)value[off + b]) << (b * 8);

        uint64_t now_ms = dms_now_ms();
        if (now_ms - ts_ms < DMS_HB_MAX_AGE_MS)
            return;  /* heartbeat is fresh, all good */

        fprintf(stderr, "[DMS watcher] heartbeat expired — fetching alarm\n");
    } else {
        fprintf(stderr, "[DMS watcher] heartbeat absent — fetching alarm\n");
    }

trigger:;
    // Trigger: fetch the alarm envelope.
    uint8_t alarm_key[32];
    dms_alarm_dht_key(&wc->svc_fp, &wc->my_fp, alarm_key);
    trellis_dht_find_value(wc->dht, alarm_key, sizeof(alarm_key),
                            on_alarm_fetched, wc);
}

static void on_alarm_fetched(const uint8_t *value, size_t value_len,
                               trellis_err_t err, void *raw_ctx)
{
    dms_watch_ctx_t *wc = (dms_watch_ctx_t *)raw_ctx;

    if (err != TRELLIS_OK || !value) {
        fprintf(stderr, "[DMS watcher] alarm envelope not found\n");
        return;
    }

    // Validate envelope.
    size_t min_len = DMS_ALARM_FIXED_LEN + DMS_KEM_CT_LEN +
                     DMS_NONCE_LEN + DMS_MAC_LEN;
    if (value_len < min_len) {
        fprintf(stderr, "[DMS watcher] alarm envelope too short\n");
        return;
    }
    if (memcmp(value, DMS_ALARM_MAGIC, 4) != 0) {
        fprintf(stderr, "[DMS watcher] alarm magic mismatch\n");
        return;
    }

    // Extract fields.
    size_t off = DMS_ALARM_FIXED_LEN;
    const uint8_t *kem_ct  = value + off; off += DMS_KEM_CT_LEN;
    const uint8_t *nonce   = value + off; off += DMS_NONCE_LEN;
    const uint8_t *ctext   = value + off;
    size_t  ctext_len      = value_len - off;

    if (ctext_len < DMS_MAC_LEN) return;
    size_t plain_len = ctext_len - DMS_MAC_LEN;

    // ML-KEM-1024 decapsulation.
    uint8_t ss[TRELLIS_ML_KEM_1024_SS_LEN];
    OQS_KEM *kem = OQS_KEM_new(OQS_KEM_alg_ml_kem_1024);
    if (!kem) return;

    if (OQS_KEM_decaps(kem, ss, kem_ct, wc->my_ml_kem_sk) != OQS_SUCCESS) {
        OQS_KEM_free(kem);
        fprintf(stderr, "[DMS watcher] KEM decapsulation failed\n");
        return;
    }
    OQS_KEM_free(kem);

    // Derive symmetric key.
    uint8_t sym_key[crypto_secretbox_KEYBYTES];
    static const char hkdf_label[] = "trellis_dms_alarm_sym_key";
    crypto_auth_hmacsha256_state hs;
    crypto_auth_hmacsha256_init(&hs, ss, TRELLIS_ML_KEM_1024_SS_LEN);
    crypto_auth_hmacsha256_update(&hs, (const uint8_t *)hkdf_label,
                                   strlen(hkdf_label));
    uint8_t hmac_out[32];
    crypto_auth_hmacsha256_final(&hs, hmac_out);
    memcpy(sym_key, hmac_out, crypto_secretbox_KEYBYTES);

    // Decrypt
    uint8_t *plaintext = malloc(plain_len);
    if (!plaintext) return;

    if (crypto_secretbox_open_easy(plaintext, ctext, ctext_len,
                                    nonce, sym_key) != 0) {
        sodium_memzero(ss, sizeof(ss));
        sodium_memzero(sym_key, sizeof(sym_key));
        free(plaintext);
        fprintf(stderr, "[DMS watcher] alarm decryption failed\n");
        return;
    }
    sodium_memzero(ss, sizeof(ss));
    sodium_memzero(sym_key, sizeof(sym_key));

    fprintf(stderr, "[DMS watcher] alarm delivered! (%zu bytes)\n", plain_len);

    // Deliver to caller.
    if (wc->cb) wc->cb(&wc->svc_fp, plaintext, plain_len, wc->ctx);
    free(plaintext);
}

/*
 * Start watching a service's dead man's switch.
 * Polls every POLL_INTERVAL_MS; fires cb if the heartbeat expires.
 *
 * @dht            DHT handle
 * @loop           libuv event loop
 * @svc_fp         fingerprint of the service to watch
 * @my_fp          this recipient's fingerprint
 * @my_ml_kem_sk   this recipient's ML-KEM-1024 secret key
 * @cb             callback when alarm is triggered and decrypted
 * @ctx            user context
 *
 * Returns a heap-allocated watch context; call trellis_dms_unwatch() to stop.
 */
trellis_dms_watch_t *trellis_dms_watch(
        trellis_dht_t *dht,
        uv_loop_t *loop,
        const trellis_fingerprint_t *svc_fp,
        const trellis_fingerprint_t *my_fp,
        const uint8_t *my_ml_kem_sk,
        trellis_dms_alarm_cb cb, void *ctx)
{
    if (!dht || !loop || !svc_fp || !my_fp || !my_ml_kem_sk)
        return NULL;

    dms_watch_ctx_t *wc = calloc(1, sizeof(*wc));
    if (!wc) return NULL;

    wc->svc_fp = *svc_fp;
    wc->my_fp  = *my_fp;
    memcpy(wc->my_ml_kem_sk, my_ml_kem_sk, TRELLIS_ML_KEM_1024_SK_LEN);
    wc->cb  = cb;
    wc->ctx = ctx;
    wc->dht = dht;

    uv_timer_init(loop, &wc->poll_timer);
    wc->poll_timer.data  = wc;
    wc->timer_active     = true;

    // Poll every DMS_HB_MAX_AGE_MS / 4 to detect expiry promptly.
    uint64_t poll_interval = DMS_HB_MAX_AGE_MS / 4;
    uv_timer_start(&wc->poll_timer, dms_watch_poll_cb,
                   poll_interval, poll_interval);

    // Immediate first check.
    dms_watch_poll_cb(&wc->poll_timer);

    return (trellis_dms_watch_t *)wc;
}

void trellis_dms_unwatch(trellis_dms_watch_t *watch)
{
    if (!watch) return;
    dms_watch_ctx_t *wc = (dms_watch_ctx_t *)watch;
    if (wc->timer_active) {
        uv_timer_stop(&wc->poll_timer);
        wc->timer_active = false;
    }
    sodium_memzero(wc->my_ml_kem_sk, sizeof(wc->my_ml_kem_sk));
    free(wc);
}
