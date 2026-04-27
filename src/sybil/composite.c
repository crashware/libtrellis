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
 * composite.c — Weighted composite Sybil scorer and trust-weighted,
 *               AS-diverse hop selection for onion circuits.
 */

#include "internal.h"
#include <trellis/mesh.h>

trellis_sybil_t *trellis_sybil_new(void)
{
    trellis_sybil_t *s = calloc(1, sizeof(*s));
    if (!s)
        return NULL;

    s->trust = trellis_trust_graph_new();
    s->behavior = trellis_behavior_new();
    if (!s->trust || !s->behavior) {
        trellis_sybil_free(s);
        return NULL;
    }
    return s;
}

void trellis_sybil_free(trellis_sybil_t *sybil)
{
    if (!sybil)
        return;
    trellis_trust_graph_free(sybil->trust);
    trellis_behavior_free(sybil->behavior);
    free(sybil);
}

trellis_trust_graph_t *trellis_sybil_trust(trellis_sybil_t *sybil)
{
    return sybil ? sybil->trust : NULL;
}

trellis_behavior_tracker_t *trellis_sybil_behavior(trellis_sybil_t *sybil)
{
    return sybil ? sybil->behavior : NULL;
}

trellis_err_t trellis_sybil_check_admission(
    trellis_sybil_t *sybil,
    const trellis_fingerprint_t *peer,
    const trellis_vdf_proof_t *vdf_proof,
    const trellis_identity_public_t *pub)
{
    if (!sybil || !peer)
        return TRELLIS_ERR_INVALID_ARG;

    /*
     * VDF verification: a valid proof is required for full admission.
     * Peers without a proof (or with an invalid one) are quarantined --
     * they can participate in the DHT but are excluded from relay/onion
     * hop selection.
     */
    if (!vdf_proof || !pub)
        return TRELLIS_ERR_VDF_INVALID;

    trellis_err_t err = trellis_vdf_verify(peer, vdf_proof, pub);
    if (err != TRELLIS_OK)
        return err;

    // PoP check: enforced after bootstrap grace period (see personhood.c)
    if (!trellis_pop_has_credential(peer))
        return TRELLIS_ERR_SYBIL;

    return TRELLIS_OK;
}

static double compute_perf_score(double latency_ms, double bandwidth,
                                 double reliability)
{
    double lat = (1000.0 - latency_ms) / 1000.0;
    if (lat < 0.0) lat = 0.0;
    return lat * 0.4 + bandwidth * 0.3 + reliability * 0.3;
}

double trellis_sybil_composite_score(
    const trellis_sybil_t *sybil,
    const trellis_fingerprint_t *peer,
    double latency_ms, double bandwidth, double reliability,
    const trellis_vdf_proof_t *vdf_proof)
{
    if (!sybil || !peer)
        return 0.0;

    double perf = compute_perf_score(latency_ms, bandwidth, reliability);

    double trust = 0.0;
    if (sybil->trust)
        trust = trellis_trust_score(sybil->trust, peer);

    double behavior = 0.5;
    if (sybil->behavior)
        behavior = trellis_behavior_score(sybil->behavior, peer);

    double vdf_age = 0.0;
    if (vdf_proof)
        vdf_age = trellis_vdf_age_score(vdf_proof);

    double hco = trellis_hco_freshness(peer);

    double composite = perf      * SYBIL_W_PERF
                     + trust     * SYBIL_W_TRUST
                     + behavior  * SYBIL_W_BEHAVIOR
                     + vdf_age   * SYBIL_W_VDF_AGE
                     + hco       * SYBIL_W_HCO;

    if (composite < 0.0) composite = 0.0;
    if (composite > 1.0) composite = 1.0;
    return composite;
}

size_t trellis_sybil_select_hops(
    const trellis_sybil_t *sybil,
    trellis_peer_info_t **candidates, size_t candidate_count,
    const trellis_fingerprint_t *exclude,
    trellis_peer_info_t **hops_out, size_t hops_wanted)
{
    if (!sybil || !candidates || !hops_out || candidate_count == 0 ||
        hops_wanted == 0)
        return 0;

    /*
     * Compute a score for each candidate and build a weighted pool.
     * Then select hops greedily, enforcing AS diversity.
     */
    double *scores = calloc(candidate_count, sizeof(double));
    if (!scores)
        return 0;

    double total_score = 0.0;
    for (size_t i = 0; i < candidate_count; i++) {
        if (exclude &&
            trellis_fingerprint_eq(&candidates[i]->fingerprint, exclude)) {
            scores[i] = 0.0;
            continue;
        }

        /* VDF minimum age: peers whose proof is younger than
         * TRELLIS_VDF_MIN_AGE_MS are ineligible for relay duty. */
        if (candidates[i]->vdf_proof) {
            uint64_t now = trellis_now_ms();
            if (candidates[i]->vdf_proof->created_at > 0 &&
                now - candidates[i]->vdf_proof->created_at <
                    TRELLIS_VDF_MIN_AGE_MS) {
                scores[i] = 0.0;
                continue;
            }
        }

        scores[i] = trellis_sybil_composite_score(
            sybil, &candidates[i]->fingerprint,
            candidates[i]->latency_ms,
            candidates[i]->bandwidth,
            candidates[i]->reliability,
            candidates[i]->vdf_proof);
        total_score += scores[i];
    }

    if (total_score < 1e-12) {
        free(scores);
        return 0;
    }

    // Track which candidates we've already selected.
    bool *used = calloc(candidate_count, sizeof(bool));
    if (!used) {
        free(scores);
        return 0;
    }

    size_t selected = 0;
    size_t max_attempts = candidate_count * 3;
    size_t attempts = 0;

    while (selected < hops_wanted && attempts < max_attempts) {
        attempts++;

        /*
         * Weighted random selection: generate a random threshold
         * and walk the cumulative distribution.
         */
        uint32_t rnd = randombytes_uniform(1000000);
        double threshold = ((double)rnd / 1000000.0) * total_score;
        double cumulative = 0.0;
        size_t pick = candidate_count; /* sentinel */

        for (size_t i = 0; i < candidate_count; i++) {
            if (used[i] || scores[i] <= 0.0)
                continue;
            cumulative += scores[i];
            if (cumulative >= threshold) {
                pick = i;
                break;
            }
        }

        if (pick >= candidate_count)
            continue;

        /*
         * Diversity checks against already-selected hops:
         * 1. AS diversity: reject if same /24 as any selected hop
         * 2. Uptime correlation: reject if Pearson r > 0.8 with any
         *    selected hop (suspiciously correlated availability)
         */
        bool diversity_conflict = false;
        if (sybil->behavior) {
            for (size_t h = 0; h < selected; h++) {
                if (trellis_behavior_same_subnet(
                        sybil->behavior,
                        &candidates[pick]->fingerprint,
                        &hops_out[h]->fingerprint)) {
                    diversity_conflict = true;
                    break;
                }
                double corr = trellis_behavior_uptime_correlation(
                    sybil->behavior,
                    &candidates[pick]->fingerprint,
                    &hops_out[h]->fingerprint);
                if (corr > 0.8) {
                    diversity_conflict = true;
                    break;
                }
            }
        }

        if (diversity_conflict)
            continue;

        hops_out[selected++] = candidates[pick];
        used[pick] = true;
        total_score -= scores[pick];
    }

    /*
     * If we couldn't fill all hops with AS-diverse peers, do a
     * relaxed pass without the subnet constraint.
     */
    if (selected < hops_wanted) {
        for (size_t i = 0; i < candidate_count && selected < hops_wanted; i++) {
            if (used[i] || scores[i] <= 0.0)
                continue;
            hops_out[selected++] = candidates[i];
            used[i] = true;
        }
    }

    free(scores);
    free(used);
    return selected;
}

/*
 * Wire format for vouch gossip messages:
 *   [1 byte tag (0x56)] [trellis_vouch_t] [trellis_identity_public_t]
 */
trellis_err_t trellis_sybil_publish_vouch(
    trellis_sybil_t *sybil,
    trellis_gossip_t *gossip,
    const trellis_vouch_t *vouch,
    const trellis_identity_public_t *voucher_pub)
{
    if (!sybil || !gossip || !vouch || !voucher_pub)
        return TRELLIS_ERR_INVALID_ARG;

    // Also add to our own trust graph.
    trellis_err_t err = trellis_trust_add_vouch(sybil->trust, vouch, voucher_pub);
    if (err != TRELLIS_OK)
        return err;

    size_t msg_len = 1 + sizeof(trellis_vouch_t) +
                     sizeof(trellis_identity_public_t);
    uint8_t *msg = malloc(msg_len);
    if (!msg)
        return TRELLIS_ERR_NOMEM;

    msg[0] = TRELLIS_GOSSIP_VOUCH_TAG;
    memcpy(msg + 1, vouch, sizeof(trellis_vouch_t));
    memcpy(msg + 1 + sizeof(trellis_vouch_t), voucher_pub,
           sizeof(trellis_identity_public_t));

    err = trellis_gossip_publish(gossip, msg, msg_len);
    free(msg);
    return err;
}

static void vouch_gossip_cb(const uint8_t *data, size_t len,
                            const trellis_fingerprint_t *origin, void *ctx)
{
    (void)origin;
    trellis_sybil_t *sybil = ctx;
    if (!sybil || !data)
        return;

    size_t expected = 1 + sizeof(trellis_vouch_t) +
                      sizeof(trellis_identity_public_t);
    if (len != expected || data[0] != TRELLIS_GOSSIP_VOUCH_TAG)
        return;

    trellis_vouch_t vouch;
    trellis_identity_public_t voucher_pub;
    memcpy(&vouch, data + 1, sizeof(trellis_vouch_t));
    memcpy(&voucher_pub, data + 1 + sizeof(trellis_vouch_t),
           sizeof(trellis_identity_public_t));

    trellis_err_t err = trellis_trust_add_vouch(sybil->trust, &vouch, &voucher_pub);
    if (err != TRELLIS_OK)
        fprintf(stderr, "[SYBIL] vouch rejected: %d\n", err);
}

trellis_err_t trellis_sybil_subscribe_vouches(
    trellis_sybil_t *sybil,
    trellis_gossip_t *gossip)
{
    if (!sybil || !gossip)
        return TRELLIS_ERR_INVALID_ARG;
    return trellis_gossip_subscribe(gossip, vouch_gossip_cb, sybil);
}
