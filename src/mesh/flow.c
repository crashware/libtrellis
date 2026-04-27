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

// Flow control and rate limiting.
#include "internal.h"

void trellis_flow_init_circuit(relay_circuit_t *c)
{
    if (!c)
        return;
    c->send_window = FLOW_WINDOW_INIT;
    c->recv_window = FLOW_WINDOW_INIT;
    c->deliver_count = 0;
    trellis_flow_vegas_init(c);
}

bool trellis_flow_can_send(const relay_circuit_t *c)
{
    return c && c->send_window > 0;
}

void trellis_flow_on_send(relay_circuit_t *c, size_t bytes)
{
    if (!c)
        return;
    uint32_t cells = (uint32_t)((bytes + FLOW_CELL_SIZE - 1) / FLOW_CELL_SIZE);
    if (cells > c->send_window)
        c->send_window = 0;
    else
        c->send_window -= cells;
    c->inflight += cells;
}

bool trellis_flow_on_recv(relay_circuit_t *c, size_t bytes)
{
    if (!c)
        return false;
    uint32_t cells = (uint32_t)((bytes + FLOW_CELL_SIZE - 1) / FLOW_CELL_SIZE);
    c->deliver_count += cells;

    if (c->deliver_count >= FLOW_WINDOW_INCREMENT) {
        c->deliver_count -= FLOW_WINDOW_INCREMENT;
        return true; /* caller should send SENDME */
    }
    return false;
}

void trellis_flow_on_sendme(relay_circuit_t *c)
{
    if (!c)
        return;
    c->send_window += FLOW_WINDOW_INCREMENT;
    if (c->send_window > c->cwnd)
        c->send_window = c->cwnd;

    // Decrement inflight for the acknowledged cells.
    if (c->inflight > FLOW_WINDOW_INCREMENT)
        c->inflight -= FLOW_WINDOW_INCREMENT;
    else
        c->inflight = 0;
}

void trellis_flow_vegas_init(relay_circuit_t *c)
{
    if (!c) return;
    c->cwnd = FLOW_WINDOW_INIT;
    c->base_rtt_us = 0;
    c->smoothed_rtt_us = 0;
    c->inflight = 0;
}

void trellis_flow_vegas_update(relay_circuit_t *c, uint64_t rtt_us)
{
    if (!c || rtt_us == 0) return;

    // Track minimum RTT as our base.
    if (c->base_rtt_us == 0 || rtt_us < c->base_rtt_us)
        c->base_rtt_us = rtt_us;

    // EWMA smoothing.
    if (c->smoothed_rtt_us == 0)
        c->smoothed_rtt_us = rtt_us;
    else
        c->smoothed_rtt_us = (uint64_t)(
            FLOW_BDP_SMOOTHING * (double)c->smoothed_rtt_us +
            (1.0 - FLOW_BDP_SMOOTHING) * (double)rtt_us);

    /* Vegas algorithm: estimate queue depth from RTT difference.
     * diff = cwnd/base_rtt - cwnd/current_rtt
     * This represents the number of extra packets sitting in queues. */
    if (c->base_rtt_us == 0) return;

    double expected = (double)c->cwnd / (double)c->base_rtt_us;
    double actual   = (double)c->cwnd / (double)c->smoothed_rtt_us;
    double diff     = (expected - actual) * (double)c->base_rtt_us;

    if (diff < FLOW_VEGAS_ALPHA) {
        // Under-utilizing: increase window.
        if (c->cwnd < FLOW_CWND_MAX)
            c->cwnd++;
    } else if (diff > FLOW_VEGAS_BETA) {
        // Over-utilizing / queuing: decrease window.
        if (c->cwnd > FLOW_CWND_MIN)
            c->cwnd--;
    }
    // diff in [alpha, beta]: steady state, no change.
}

void trellis_circuit_table_init(circuit_table_t *ct)
{
    if (!ct)
        return;
    memset(ct, 0, sizeof(*ct));
}

trellis_err_t trellis_circuit_table_insert(circuit_table_t *ct,
                                           uint32_t stream_id,
                                           const trellis_fingerprint_t *prev_hop,
                                           const uint8_t layer_key[32])
{
    if (!ct || !prev_hop)
        return TRELLIS_ERR_INVALID_ARG;

    // Reuse an expired or inactive slot, or find an empty one.
    uint64_t now = trellis_now_ms();
    int oldest_idx = -1;
    uint64_t oldest_ts = UINT64_MAX;

    for (size_t i = 0; i < CIRCUIT_TABLE_MAX; i++) {
        if (!ct->entries[i].active) {
            ct->entries[i].stream_id = stream_id;
            ct->entries[i].prev_hop = *prev_hop;
            memcpy(ct->entries[i].layer_key, layer_key, 32);
            ct->entries[i].created_at = now;
            ct->entries[i].active = true;
            ct->count++;
            return TRELLIS_OK;
        }
        if (ct->entries[i].created_at < oldest_ts) {
            oldest_ts = ct->entries[i].created_at;
            oldest_idx = (int)i;
        }
    }

    // Table full — evict oldest entry.
    if (oldest_idx >= 0) {
        ct->entries[oldest_idx].stream_id = stream_id;
        ct->entries[oldest_idx].prev_hop = *prev_hop;
        memcpy(ct->entries[oldest_idx].layer_key, layer_key, 32);
        ct->entries[oldest_idx].created_at = now;
        ct->entries[oldest_idx].active = true;
        return TRELLIS_OK;
    }

    return TRELLIS_ERR_NOMEM;
}

circuit_entry_t *trellis_circuit_table_lookup(circuit_table_t *ct,
                                              uint32_t stream_id)
{
    if (!ct)
        return NULL;
    for (size_t i = 0; i < CIRCUIT_TABLE_MAX; i++) {
        if (ct->entries[i].active && ct->entries[i].stream_id == stream_id)
            return &ct->entries[i];
    }
    return NULL;
}

void trellis_circuit_table_remove(circuit_table_t *ct, uint32_t stream_id)
{
    if (!ct)
        return;
    for (size_t i = 0; i < CIRCUIT_TABLE_MAX; i++) {
        if (ct->entries[i].active && ct->entries[i].stream_id == stream_id) {
            sodium_memzero(ct->entries[i].layer_key, 32);
            ct->entries[i].active = false;
            if (ct->count > 0)
                ct->count--;
            return;
        }
    }
}

void trellis_circuit_table_expire(circuit_table_t *ct, uint64_t max_age_ms)
{
    if (!ct)
        return;
    uint64_t now = trellis_now_ms();
    for (size_t i = 0; i < CIRCUIT_TABLE_MAX; i++) {
        if (ct->entries[i].active &&
            now - ct->entries[i].created_at > max_age_ms) {
            sodium_memzero(ct->entries[i].layer_key, 32);
            ct->entries[i].active = false;
            if (ct->count > 0)
                ct->count--;
        }
    }
}
