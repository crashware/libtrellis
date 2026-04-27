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
 * deaddrop.c — Epoch-rotating DHT dead drops for asynchronous,
 *              timing-decorrelated message delivery.
 *
 * Supports multiple messages per epoch via sequence-numbered keys.
 */

#include "internal.h"
#include <trellis/crypto.h>
#include <sodium.h>

static const uint8_t DEADDROP_LABEL[] = "trellis-deaddrop";
#define DEADDROP_LABEL_LEN 16

struct trellis_deaddrop {
    trellis_ratchet_t  *ratchet;
    uint8_t             shared_secret[64];
    size_t              secret_len;
    uint64_t            epoch_ms;
    uint64_t            last_send_epoch;
    uint32_t            send_seq;
};

static uint64_t deaddrop_current_epoch_index(const trellis_deaddrop_t *dd)
{
    uint64_t now = trellis_now_ms();
    uint64_t epoch = dd->epoch_ms;
    if (epoch == 0)
        epoch = TRELLIS_DEADDROP_DEFAULT_EPOCH_MS;
    return now / epoch;
}

static trellis_err_t deaddrop_derive_key(const trellis_deaddrop_t *dd,
                                          uint64_t epoch_index,
                                          uint32_t seq,
                                          uint8_t key[TRELLIS_DEADDROP_KEY_LEN])
{
    // info = label || epoch_index_le64 || seq_le32
    uint8_t info[DEADDROP_LABEL_LEN + 8 + 4];
    memcpy(info, DEADDROP_LABEL, DEADDROP_LABEL_LEN);

    // little-endian epoch index.
    size_t off = DEADDROP_LABEL_LEN;
    info[off + 0] = (uint8_t)(epoch_index);
    info[off + 1] = (uint8_t)(epoch_index >> 8);
    info[off + 2] = (uint8_t)(epoch_index >> 16);
    info[off + 3] = (uint8_t)(epoch_index >> 24);
    info[off + 4] = (uint8_t)(epoch_index >> 32);
    info[off + 5] = (uint8_t)(epoch_index >> 40);
    info[off + 6] = (uint8_t)(epoch_index >> 48);
    info[off + 7] = (uint8_t)(epoch_index >> 56);

    // little-endian sequence number.
    off += 8;
    info[off + 0] = (uint8_t)(seq);
    info[off + 1] = (uint8_t)(seq >> 8);
    info[off + 2] = (uint8_t)(seq >> 16);
    info[off + 3] = (uint8_t)(seq >> 24);

    return trellis_hkdf_shake256(
        dd->shared_secret, dd->secret_len,
        NULL, 0,
        info, sizeof(info),
        key, TRELLIS_DEADDROP_KEY_LEN);
}

trellis_deaddrop_t *trellis_deaddrop_new(trellis_ratchet_t *ratchet,
                                          const uint8_t *shared_secret,
                                          size_t secret_len,
                                          uint64_t epoch_ms)
{
    if (!ratchet || !shared_secret || secret_len == 0)
        return NULL;

    trellis_deaddrop_t *dd = calloc(1, sizeof(*dd));
    if (!dd)
        return NULL;

    dd->ratchet = ratchet;
    dd->epoch_ms = epoch_ms ? epoch_ms : TRELLIS_DEADDROP_DEFAULT_EPOCH_MS;

    size_t copy = secret_len < sizeof(dd->shared_secret)
                ? secret_len : sizeof(dd->shared_secret);
    memcpy(dd->shared_secret, shared_secret, copy);
    dd->secret_len = copy;

    return dd;
}

void trellis_deaddrop_free(trellis_deaddrop_t *dd)
{
    if (!dd)
        return;
    sodium_memzero(dd->shared_secret, sizeof(dd->shared_secret));
    free(dd);
}

trellis_err_t trellis_deaddrop_send(trellis_deaddrop_t *dd,
                                     trellis_dht_t *dht,
                                     const uint8_t *plaintext, size_t len)
{
    if (!dd || !dht || !plaintext || len == 0)
        return TRELLIS_ERR_INVALID_ARG;

    uint64_t epoch_idx = deaddrop_current_epoch_index(dd);

    // Reset sequence counter on epoch change.
    if (epoch_idx != dd->last_send_epoch) {
        dd->send_seq = 0;
        dd->last_send_epoch = epoch_idx;
    }

    uint32_t seq = dd->send_seq;

    uint8_t key[TRELLIS_DEADDROP_KEY_LEN];
    trellis_err_t err = deaddrop_derive_key(dd, epoch_idx, seq, key);
    if (err != TRELLIS_OK) {
        sodium_memzero(key, sizeof(key));
        return err;
    }

    // Encrypt under the ratchet.
    trellis_buf_t ct = {0};
    err = trellis_ratchet_encrypt(dd->ratchet, plaintext, len, &ct);
    if (err != TRELLIS_OK) {
        sodium_memzero(key, sizeof(key));
        return err;
    }

    // Store at the dead-drop key with redundant replicas.
    err = trellis_dht_store_redundant(dht, key, TRELLIS_DEADDROP_KEY_LEN,
                                       ct.data, ct.len, NULL, NULL, NULL);
    trellis_buf_free(&ct);
    sodium_memzero(key, sizeof(key));

    if (err == TRELLIS_OK)
        dd->send_seq++;

    return err;
}

#define DEADDROP_MAX_MESSAGES  64

typedef struct deaddrop_poll_ctx deaddrop_poll_ctx_t;

typedef struct {
    deaddrop_poll_ctx_t *pc;
    uint64_t             epoch_idx;
    uint32_t             seq;
} deaddrop_lookup_ctx_t;

struct deaddrop_poll_ctx {
    trellis_deaddrop_t      *dd;
    trellis_dht_t           *dht;
    trellis_deaddrop_poll_cb cb;
    void                    *ctx;
    int                      pending;
    bool                     delivered;

    trellis_buf_t            results[DEADDROP_MAX_MESSAGES];
    size_t                   result_count;

    bool                     cur_epoch_done;
    bool                     prev_epoch_done;
    uint64_t                 epoch_idx;
    uint64_t                 prev_epoch_idx;
};

static void deaddrop_deliver_results(deaddrop_poll_ctx_t *pc);
static trellis_err_t deaddrop_issue_seq_lookup(deaddrop_poll_ctx_t *pc,
                                                uint64_t epoch_idx,
                                                uint32_t seq);

static void deaddrop_find_cb(const uint8_t *value, size_t len,
                              trellis_err_t err, void *ctx)
{
    deaddrop_lookup_ctx_t *lctx = (deaddrop_lookup_ctx_t *)ctx;
    if (!lctx)
        return;

    deaddrop_poll_ctx_t *pc = lctx->pc;
    uint64_t epoch_idx = lctx->epoch_idx;
    uint32_t seq       = lctx->seq;
    free(lctx);

    pc->pending--;

    if (err == TRELLIS_OK && value && len > 0 && !pc->delivered) {
        trellis_buf_t pt = {0};
        trellis_err_t decrypt_err = trellis_ratchet_decrypt(
            pc->dd->ratchet, value, len, &pt);

        if (decrypt_err == TRELLIS_OK && pt.data && pt.len > 0) {
            if (pc->result_count < DEADDROP_MAX_MESSAGES) {
                pc->results[pc->result_count++] = pt;
            } else {
                trellis_buf_free(&pt);
            }
        } else {
            trellis_buf_free(&pt);
        }

        /* Found seq N — issue a lookup for seq N+1 in the same epoch.
         * Guard against UINT32_MAX wrap-around. */
        if (pc->result_count < DEADDROP_MAX_MESSAGES && seq < UINT32_MAX)
            deaddrop_issue_seq_lookup(pc, epoch_idx, seq + 1);
    } else {
        // NOT_FOUND or error: this epoch has no more messages.
        if (epoch_idx == pc->epoch_idx)
            pc->cur_epoch_done = true;
        else
            pc->prev_epoch_done = true;
    }

    if (pc->pending <= 0)
        deaddrop_deliver_results(pc);
}

static void deaddrop_deliver_results(deaddrop_poll_ctx_t *pc)
{
    if (pc->delivered)
        goto cleanup;

    pc->delivered = true;

    for (size_t i = 0; i < pc->result_count; i++) {
        if (pc->cb)
            pc->cb(pc->results[i].data, pc->results[i].len, TRELLIS_OK, pc->ctx);
    }

    // Terminal NOT_FOUND signals the caller that polling is complete.
    if (pc->cb)
        pc->cb(NULL, 0, TRELLIS_ERR_NOT_FOUND, pc->ctx);

cleanup:
    for (size_t i = 0; i < pc->result_count; i++)
        trellis_buf_free(&pc->results[i]);
    free(pc);
}

static trellis_err_t deaddrop_issue_seq_lookup(deaddrop_poll_ctx_t *pc,
                                                uint64_t epoch_idx,
                                                uint32_t seq)
{
    uint8_t key[TRELLIS_DEADDROP_KEY_LEN];
    trellis_err_t err = deaddrop_derive_key(pc->dd, epoch_idx, seq, key);
    if (err != TRELLIS_OK) {
        sodium_memzero(key, sizeof(key));
        return err;
    }

    deaddrop_lookup_ctx_t *lctx = malloc(sizeof(*lctx));
    if (!lctx) {
        sodium_memzero(key, sizeof(key));
        return TRELLIS_ERR_NOMEM;
    }
    lctx->pc        = pc;
    lctx->epoch_idx = epoch_idx;
    lctx->seq       = seq;

    pc->pending++;
    err = trellis_pir_find_value_skad(pc->dht, key, TRELLIS_DEADDROP_KEY_LEN,
                                       deaddrop_find_cb, lctx);
    sodium_memzero(key, sizeof(key));

    if (err != TRELLIS_OK) {
        pc->pending--;
        free(lctx);
    }

    return err;
}

trellis_err_t trellis_deaddrop_poll(trellis_deaddrop_t *dd,
                                     trellis_dht_t *dht,
                                     trellis_deaddrop_poll_cb cb, void *ctx)
{
    if (!dd || !dht)
        return TRELLIS_ERR_INVALID_ARG;

    uint64_t epoch_idx = deaddrop_current_epoch_index(dd);

    deaddrop_poll_ctx_t *pc = calloc(1, sizeof(*pc));
    if (!pc)
        return TRELLIS_ERR_NOMEM;

    pc->dd             = dd;
    pc->dht            = dht;
    pc->cb             = cb;
    pc->ctx            = ctx;
    pc->epoch_idx      = epoch_idx;
    pc->prev_epoch_idx = epoch_idx > 0 ? epoch_idx - 1 : 0;

    if (epoch_idx == 0)
        pc->prev_epoch_done = true;

    // Issue seq=0 for current epoch.
    trellis_err_t err = deaddrop_issue_seq_lookup(pc, epoch_idx, 0);
    if (err != TRELLIS_OK) {
        if (cb)
            cb(NULL, 0, err, ctx);
        free(pc);
        return err;
    }

    // Issue seq=0 for previous epoch (boundary race mitigation)
    if (!pc->prev_epoch_done) {
        trellis_err_t prev_err = deaddrop_issue_seq_lookup(pc, epoch_idx - 1, 0);
        if (prev_err != TRELLIS_OK)
            pc->prev_epoch_done = true;
    }

    return TRELLIS_OK;
}
