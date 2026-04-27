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
 * trust.c — Local directed trust graph with transitive scoring
 *            and Ollivier-Ricci curvature-based Sybil detection
 */

#include "internal.h"

trellis_trust_entry_t *trust_find_or_create(trellis_trust_graph_t *tg,
                                            const trellis_fingerprint_t *peer)
{
    for (size_t i = 0; i < tg->count; i++) {
        if (tg->entries[i].occupied &&
            trellis_fingerprint_eq(&tg->entries[i].peer, peer))
            return &tg->entries[i];
    }

    if (tg->count >= TRELLIS_TRUST_MAX_ENTRIES)
        return NULL;

    trellis_trust_entry_t *e = &tg->entries[tg->count];
    memset(e, 0, sizeof(*e));
    e->peer = *peer;
    e->first_seen = trellis_now_ms();
    e->occupied = true;
    tg->count++;
    return e;
}

const trellis_trust_entry_t *trust_find(const trellis_trust_graph_t *tg,
                                        const trellis_fingerprint_t *peer)
{
    for (size_t i = 0; i < tg->count; i++) {
        if (tg->entries[i].occupied &&
            trellis_fingerprint_eq(&tg->entries[i].peer, peer))
            return &tg->entries[i];
    }
    return NULL;
}

static ssize_t entry_index(const trellis_trust_graph_t *tg,
                           const trellis_fingerprint_t *peer)
{
    for (size_t i = 0; i < tg->count; i++) {
        if (tg->entries[i].occupied &&
            trellis_fingerprint_eq(&tg->entries[i].peer, peer))
            return (ssize_t)i;
    }
    return -1;
}

static trust_adj_node_t *adj_find_or_create(trellis_trust_graph_t *tg,
                                            const trellis_fingerprint_t *peer)
{
    for (size_t i = 0; i < tg->adj_count; i++) {
        if (trellis_fingerprint_eq(&tg->adj[i].peer, peer))
            return &tg->adj[i];
    }

    if (tg->adj_count >= TRELLIS_TRUST_MAX_ENTRIES)
        return NULL;

    trust_adj_node_t *n = &tg->adj[tg->adj_count];
    memset(n, 0, sizeof(*n));
    n->peer = *peer;
    tg->adj_count++;
    return n;
}

trellis_trust_graph_t *trellis_trust_graph_new(void)
{
    trellis_trust_graph_t *tg = calloc(1, sizeof(trellis_trust_graph_t));
    return tg;
}

void trellis_trust_graph_free(trellis_trust_graph_t *tg)
{
    free(tg);
}

trellis_err_t trellis_trust_observe(trellis_trust_graph_t *tg,
                                    const trellis_fingerprint_t *peer,
                                    double delta)
{
    if (!tg || !peer)
        return TRELLIS_ERR_INVALID_ARG;

    trellis_trust_entry_t *e = trust_find_or_create(tg, peer);
    if (!e)
        return TRELLIS_ERR_NOMEM;

    e->direct_trust += delta;
    if (e->direct_trust < 0.0) e->direct_trust = 0.0;
    if (e->direct_trust > 1.0) e->direct_trust = 1.0;
    return TRELLIS_OK;
}

trellis_err_t trellis_trust_add_vouch(trellis_trust_graph_t *tg,
                                      const trellis_vouch_t *vouch,
                                      const trellis_identity_public_t *voucher_pub)
{
    if (!tg || !vouch || !voucher_pub)
        return TRELLIS_ERR_INVALID_ARG;

    // Verify the vouch signature.
    uint8_t msg[TRELLIS_FINGERPRINT_LEN * 2 + 8];
    memcpy(msg, vouch->voucher.bytes, TRELLIS_FINGERPRINT_LEN);
    memcpy(msg + TRELLIS_FINGERPRINT_LEN, vouch->vouchee.bytes,
           TRELLIS_FINGERPRINT_LEN);
    for (int b = 0; b < 8; b++)
        msg[2 * TRELLIS_FINGERPRINT_LEN + b] =
            (uint8_t)(vouch->timestamp >> (56 - 8 * b));

    trellis_err_t err = trellis_identity_verify(voucher_pub, msg, sizeof(msg),
                                                &vouch->signature);
    if (err != TRELLIS_OK)
        return TRELLIS_ERR_VOUCH_INVALID;

    // Ensure both peers exist in the graph.
    trust_find_or_create(tg, &vouch->voucher);
    trellis_trust_entry_t *vouchee = trust_find_or_create(tg, &vouch->vouchee);
    if (!vouchee)
        return TRELLIS_ERR_NOMEM;

    vouchee->vouch_count++;
    vouchee->last_vouched = vouch->timestamp;

    // Record adjacency: voucher -> vouchee.
    trust_adj_node_t *adj = adj_find_or_create(tg, &vouch->vouchee);
    if (!adj)
        return TRELLIS_ERR_NOMEM;

    ssize_t voucher_idx = entry_index(tg, &vouch->voucher);
    if (voucher_idx >= 0 && adj->voucher_count < TRUST_ADJ_MAX) {
        bool dup = false;
        for (size_t i = 0; i < adj->voucher_count; i++) {
            if (adj->voucher_indices[i] == (size_t)voucher_idx) {
                dup = true;
                break;
            }
        }
        if (!dup)
            adj->voucher_indices[adj->voucher_count++] = (size_t)voucher_idx;
    }

    return TRELLIS_OK;
}

/*
 * Recompute transitive trust and curvature penalties.
 *
 * Transitive trust for a peer P:
 *   T(P) = max over all paths of length <= TRELLIS_TRUST_MAX_DEPTH from
 *          a directly trusted peer to P, where each hop decays by TRELLIS_TRUST_DECAY.
 *
 * Curvature penalty: approximate Ollivier-Ricci curvature on the vouch graph.
 * For each peer, measure the ratio of actual vs possible edges among its
 * immediate vouchers. High clustering (many mutual vouches among vouchers)
 * indicates a Sybil farm.
 */
void trellis_trust_recompute(trellis_trust_graph_t *tg)
{
    if (!tg)
        return;

    // Reset transitive trust.
    for (size_t i = 0; i < tg->count; i++) {
        if (tg->entries[i].occupied)
            tg->entries[i].transitive_trust = tg->entries[i].direct_trust;
    }

    // BFS-style propagation up to max depth.
    for (int depth = 1; depth <= TRELLIS_TRUST_MAX_DEPTH; depth++) {
        double decay = pow(TRELLIS_TRUST_DECAY, (double)depth);
        for (size_t i = 0; i < tg->adj_count; i++) {
            trust_adj_node_t *adj = &tg->adj[i];
            ssize_t target_idx = entry_index(tg, &adj->peer);
            if (target_idx < 0)
                continue;

            for (size_t v = 0; v < adj->voucher_count; v++) {
                size_t src_idx = adj->voucher_indices[v];
                if (src_idx >= tg->count)
                    continue;
                double contrib = tg->entries[src_idx].direct_trust * decay;
                if (contrib > tg->entries[target_idx].transitive_trust)
                    tg->entries[target_idx].transitive_trust = contrib;
            }
        }
    }

    // Clamp transitive trust.
    for (size_t i = 0; i < tg->count; i++) {
        if (tg->entries[i].transitive_trust > 1.0)
            tg->entries[i].transitive_trust = 1.0;
    }

    /*
     * Curvature penalty: for each node, examine the voucher set.
     * If vouchers are densely interconnected (many mutual vouches), it
     * suggests a Sybil cluster. Penalty = -(clustering coefficient - 0.5)
     * when clustering > 0.5 (high clustering is suspicious).
     */
    for (size_t i = 0; i < tg->adj_count; i++) {
        trust_adj_node_t *adj = &tg->adj[i];
        ssize_t target_idx = entry_index(tg, &adj->peer);
        if (target_idx < 0)
            continue;

        size_t n = adj->voucher_count;
        if (n < 2) {
            tg->entries[target_idx].curvature_penalty = 0.0;
            continue;
        }

        /*
         * Count edges between vouchers: do voucher A also vouch for voucher B?
         * This approximates the local clustering coefficient.
         */
        size_t mutual_edges = 0;
        size_t possible_edges = n * (n - 1) / 2;

        for (size_t a = 0; a < n; a++) {
            for (size_t b = a + 1; b < n; b++) {
                size_t idx_a = adj->voucher_indices[a];
                size_t idx_b = adj->voucher_indices[b];
                if (idx_a >= tg->count || idx_b >= tg->count)
                    continue;

                // Check if idx_a is a voucher of idx_b or vice versa.
                const trellis_fingerprint_t *fp_b = &tg->entries[idx_b].peer;
                for (size_t k = 0; k < tg->adj_count; k++) {
                    if (trellis_fingerprint_eq(&tg->adj[k].peer, fp_b)) {
                        for (size_t v = 0; v < tg->adj[k].voucher_count; v++) {
                            if (tg->adj[k].voucher_indices[v] == idx_a) {
                                mutual_edges++;
                                break;
                            }
                        }
                        break;
                    }
                }
            }
        }

        double clustering = (possible_edges > 0)
            ? (double)mutual_edges / (double)possible_edges
            : 0.0;

        // Penalty kicks in above 0.5 clustering coefficient.
        if (clustering > 0.5)
            tg->entries[target_idx].curvature_penalty = -(clustering - 0.5);
        else
            tg->entries[target_idx].curvature_penalty = 0.0;
    }
}

double trellis_trust_score(const trellis_trust_graph_t *tg,
                           const trellis_fingerprint_t *peer)
{
    if (!tg || !peer)
        return 0.0;

    const trellis_trust_entry_t *e = trust_find(tg, peer);
    if (!e)
        return 0.0;

    double score = e->transitive_trust + e->curvature_penalty;
    if (score < 0.0) score = 0.0;
    if (score > 1.0) score = 1.0;
    return score;
}

const trellis_trust_entry_t *trellis_trust_lookup(
    const trellis_trust_graph_t *tg,
    const trellis_fingerprint_t *peer)
{
    return trust_find(tg, peer);
}

trellis_err_t trellis_vouch_create(const trellis_identity_t *voucher_id,
                                   const trellis_fingerprint_t *vouchee,
                                   trellis_vouch_t *vouch_out)
{
    if (!voucher_id || !vouchee || !vouch_out)
        return TRELLIS_ERR_INVALID_ARG;

    memset(vouch_out, 0, sizeof(*vouch_out));
    vouch_out->voucher = voucher_id->fingerprint;
    vouch_out->vouchee = *vouchee;
    vouch_out->timestamp = trellis_now_ms();

    uint8_t msg[TRELLIS_FINGERPRINT_LEN * 2 + 8];
    memcpy(msg, vouch_out->voucher.bytes, TRELLIS_FINGERPRINT_LEN);
    memcpy(msg + TRELLIS_FINGERPRINT_LEN, vouchee->bytes,
           TRELLIS_FINGERPRINT_LEN);
    for (int b = 0; b < 8; b++)
        msg[2 * TRELLIS_FINGERPRINT_LEN + b] =
            (uint8_t)(vouch_out->timestamp >> (56 - 8 * b));

    return trellis_identity_sign(voucher_id, msg, sizeof(msg),
                                 &vouch_out->signature);
}
