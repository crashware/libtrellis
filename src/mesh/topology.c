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

#include "internal.h"
#include <math.h>

static double score_peer(const trellis_peer_info_t *peer,
                         const trellis_sybil_t *sybil)
{
    if (sybil) {
        return trellis_sybil_composite_score(
            sybil, &peer->fingerprint,
            peer->latency_ms, peer->bandwidth, peer->reliability,
            peer->vdf_proof);
    }

    // Fallback if sybil subsystem is unavailable.
    double latency_score = (1000.0 - peer->latency_ms) / 1000.0;
    if (latency_score < 0.0)
        latency_score = 0.0;
    return latency_score * 0.4 + peer->bandwidth * 0.3 + peer->reliability * 0.3;
}

static size_t compute_target_neighbors(size_t network_size)
{
    size_t target = (size_t)floor(sqrt((double)network_size));
    return target < 4 ? 4 : target;
}

static void heartbeat_cb(uv_timer_t *handle)
{
    trellis_topology_t *topo = handle->data;

    size_t network_size = trellis_dht_peer_count(topo->dht);
    topo->target_neighbors = compute_target_neighbors(network_size);

    // Recompute trust graph if sybil subsystem is active.
    if (topo->dht->sybil)
        trellis_trust_recompute(trellis_sybil_trust(topo->dht->sybil));

    for (size_t i = 0; i < topo->neighbor_count; i++) {
        if (!topo->neighbors[i].active)
            continue;
        topo->neighbors[i].score = score_peer(&topo->neighbors[i].info,
                                              topo->dht->sybil);

        // Refresh cached behavioral tracking fields.
        if (topo->dht->sybil) {
            trellis_behavior_tracker_t *bt =
                trellis_sybil_behavior(topo->dht->sybil);
            trellis_trust_graph_t *tg =
                trellis_sybil_trust(topo->dht->sybil);
            topo->neighbors[i].behavior_score =
                trellis_behavior_score(bt, &topo->neighbors[i].info.fingerprint);
            topo->neighbors[i].trust_score =
                trellis_trust_score(tg, &topo->neighbors[i].info.fingerprint);

            double max_corr = 0.0;
            for (size_t j = 0; j < topo->neighbor_count; j++) {
                if (j == i || !topo->neighbors[j].active)
                    continue;
                double c = trellis_behavior_uptime_correlation(
                    bt, &topo->neighbors[i].info.fingerprint,
                    &topo->neighbors[j].info.fingerprint);
                if (c > max_corr) max_corr = c;
            }
            topo->neighbors[i].max_uptime_corr = max_corr;
        }
    }

    // Sync latency/bandwidth from DHT routing table into neighbor entries.
    for (size_t i = 0; i < topo->neighbor_count; i++) {
        if (!topo->neighbors[i].active)
            continue;
        const trellis_peer_info_t *dht_peer = trellis_dht_lookup_peer(
            topo->dht, &topo->neighbors[i].info.fingerprint);
        if (dht_peer) {
            if (dht_peer->latency_ms > 0.0)
                topo->neighbors[i].info.latency_ms = dht_peer->latency_ms;
            if (dht_peer->bandwidth > 0.0)
                topo->neighbors[i].info.bandwidth = dht_peer->bandwidth;
        }
    }

    // Prune neighbors below 0.2 quality threshold.
    for (size_t i = 0; i < topo->neighbor_count; ) {
        if (topo->neighbors[i].active && topo->neighbors[i].score < 0.2) {
            topo->neighbors[i].active = false;
            topo->neighbor_count--;
            if (i < topo->neighbor_count) {
                topo->neighbors[i] = topo->neighbors[topo->neighbor_count];
                memset(&topo->neighbors[topo->neighbor_count], 0,
                       sizeof(neighbor_entry_t));
            }
        } else {
            i++;
        }
    }

    // Fill up to target by picking high-quality peers from the DHT.
    if (topo->neighbor_count < topo->target_neighbors) {
        size_t needed = topo->target_neighbors - topo->neighbor_count;
        trellis_peer_info_t *candidates[TRELLIS_DHT_K];
        size_t actual = 0;
        select_random_peers(&topo->dht->rt, topo->dht->sybil,
                            candidates, TRELLIS_DHT_K, &actual);

        for (size_t c = 0; c < actual && needed > 0; c++) {
            bool already = false;
            for (size_t n = 0; n < topo->neighbor_count; n++) {
                if (trellis_fingerprint_eq(&topo->neighbors[n].info.fingerprint,
                                           &candidates[c]->fingerprint)) {
                    already = true;
                    break;
                }
            }
            if (already)
                continue;

            double s = score_peer(candidates[c], topo->dht->sybil);
            if (s < 0.3)
                continue;

            if (topo->neighbor_count >= TRELLIS_TOPOLOGY_MAX_NEIGHBORS)
                break;

            neighbor_entry_t *ne = &topo->neighbors[topo->neighbor_count];
            ne->info = *candidates[c];
            ne->score = s;
            ne->active = true;
            topo->neighbor_count++;
            needed--;
        }
    }
}

trellis_topology_t *trellis_topology_new(trellis_dht_t *dht, uv_loop_t *loop)
{
    if (!dht || !loop)
        return NULL;

    trellis_topology_t *topo = calloc(1, sizeof(trellis_topology_t));
    if (!topo)
        return NULL;

    topo->dht = dht;
    topo->loop = loop;
    topo->heartbeat_ms = TRELLIS_TOPOLOGY_HEARTBEAT_MS;
    topo->target_neighbors = 4;
    topo->running = false;

    uv_timer_init(loop, &topo->heartbeat_timer);
    topo->heartbeat_timer.data = topo;

    return topo;
}

static void topology_close_cb(uv_handle_t *handle)
{
    trellis_topology_t *topo = (trellis_topology_t *)handle->data;
    free(topo);
}

void trellis_topology_free(trellis_topology_t *topo)
{
    if (!topo)
        return;
    if (topo->running)
        trellis_topology_stop(topo);
    if (!uv_is_closing((uv_handle_t *)&topo->heartbeat_timer))
        uv_close((uv_handle_t *)&topo->heartbeat_timer, topology_close_cb);
    else
        free(topo);
}

trellis_err_t trellis_topology_start(trellis_topology_t *topo)
{
    if (!topo)
        return TRELLIS_ERR_INVALID_ARG;
    if (topo->running)
        return TRELLIS_OK;

    int rc = uv_timer_start(&topo->heartbeat_timer, heartbeat_cb,
                            topo->heartbeat_ms, topo->heartbeat_ms);
    if (rc != 0)
        return TRELLIS_ERR_DHT;

    topo->running = true;
    return TRELLIS_OK;
}

void trellis_topology_stop(trellis_topology_t *topo)
{
    if (!topo || !topo->running)
        return;
    uv_timer_stop(&topo->heartbeat_timer);
    topo->running = false;
}

size_t trellis_topology_neighbor_count(const trellis_topology_t *topo)
{
    if (!topo)
        return 0;

    size_t count = 0;
    for (size_t i = 0; i < topo->neighbor_count; i++) {
        if (topo->neighbors[i].active)
            count++;
    }
    return count;
}

size_t trellis_topology_target_neighbors(const trellis_topology_t *topo)
{
    return topo ? topo->target_neighbors : 0;
}
