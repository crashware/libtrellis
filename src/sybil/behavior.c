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
 * behavior.c — Behavioral anomaly scoring: routing fidelity, bandwidth
 *              honesty, uptime correlation, and subnet/AS diversity.
 */

#include "internal.h"

trellis_behavior_record_t *behavior_find_or_create(
    trellis_behavior_tracker_t *bt,
    const trellis_fingerprint_t *peer)
{
    for (size_t i = 0; i < bt->count; i++) {
        if (bt->records[i].occupied &&
            trellis_fingerprint_eq(&bt->records[i].peer, peer))
            return &bt->records[i];
    }

    if (bt->count >= TRELLIS_BEHAVIOR_MAX_RECORDS)
        return NULL;

    trellis_behavior_record_t *r = &bt->records[bt->count];
    memset(r, 0, sizeof(*r));
    r->peer = *peer;
    r->occupied = true;
    bt->count++;
    return r;
}

const trellis_behavior_record_t *behavior_find(
    const trellis_behavior_tracker_t *bt,
    const trellis_fingerprint_t *peer)
{
    for (size_t i = 0; i < bt->count; i++) {
        if (bt->records[i].occupied &&
            trellis_fingerprint_eq(&bt->records[i].peer, peer))
            return &bt->records[i];
    }
    return NULL;
}

/*
 * Extract /24 prefix from an address string. For IPv4 "1.2.3.4:port",
 * stores the first 3 octets. For anything else, hashes the first 12
 * characters as a rough /48 proxy.
 */
void extract_addr_prefix(const char *addr, uint8_t prefix[4])
{
    memset(prefix, 0, 4);
    if (!addr || !addr[0])
        return;

    unsigned a, b, c;
    if (sscanf(addr, "%u.%u.%u.", &a, &b, &c) == 3) {
        prefix[0] = (uint8_t)a;
        prefix[1] = (uint8_t)b;
        prefix[2] = (uint8_t)c;
        prefix[3] = 0;
        return;
    }

    // Fallback: hash the address prefix.
    size_t len = strlen(addr);
    if (len > 12) len = 12;
    uint8_t hash[32];
    trellis_shake256((const uint8_t *)addr, len, hash, sizeof(hash));
    memcpy(prefix, hash, 4);
}

trellis_behavior_tracker_t *trellis_behavior_new(void)
{
    trellis_behavior_tracker_t *bt = calloc(1, sizeof(*bt));
    return bt;
}

void trellis_behavior_free(trellis_behavior_tracker_t *bt)
{
    free(bt);
}

trellis_err_t trellis_behavior_record_circuit(
    trellis_behavior_tracker_t *bt,
    const trellis_fingerprint_t *peer,
    bool completed)
{
    if (!bt || !peer)
        return TRELLIS_ERR_INVALID_ARG;

    trellis_behavior_record_t *r = behavior_find_or_create(bt, peer);
    if (!r)
        return TRELLIS_ERR_NOMEM;

    r->circuits_attempted++;
    if (completed)
        r->circuits_completed++;
    return TRELLIS_OK;
}

trellis_err_t trellis_behavior_record_bandwidth(
    trellis_behavior_tracker_t *bt,
    const trellis_fingerprint_t *peer,
    double claimed, double measured)
{
    if (!bt || !peer)
        return TRELLIS_ERR_INVALID_ARG;

    trellis_behavior_record_t *r = behavior_find_or_create(bt, peer);
    if (!r)
        return TRELLIS_ERR_NOMEM;

    r->claimed_bandwidth = claimed;
    r->measured_bandwidth = measured;
    return TRELLIS_OK;
}

trellis_err_t trellis_behavior_record_seen(
    trellis_behavior_tracker_t *bt,
    const trellis_fingerprint_t *peer,
    const char *addr)
{
    if (!bt || !peer)
        return TRELLIS_ERR_INVALID_ARG;

    trellis_behavior_record_t *r = behavior_find_or_create(bt, peer);
    if (!r)
        return TRELLIS_ERR_NOMEM;

    // Mark current bucket as online.
    r->uptime_buckets[r->current_bucket] = 1;

    if (addr)
        extract_addr_prefix(addr, r->addr_prefix);

    return TRELLIS_OK;
}

void trellis_behavior_advance_bucket(trellis_behavior_tracker_t *bt)
{
    if (!bt)
        return;

    for (size_t i = 0; i < bt->count; i++) {
        if (!bt->records[i].occupied)
            continue;
        trellis_behavior_record_t *r = &bt->records[i];
        r->current_bucket = (r->current_bucket + 1) % TRELLIS_UPTIME_BUCKETS;
        r->uptime_buckets[r->current_bucket] = 0;
    }
}

double trellis_behavior_score(const trellis_behavior_tracker_t *bt,
                              const trellis_fingerprint_t *peer)
{
    if (!bt || !peer)
        return 0.0;

    const trellis_behavior_record_t *r = behavior_find(bt, peer);
    if (!r)
        return 0.5; /* no data: neutral */

    // Routing fidelity: ratio of completed to attempted.
    double fidelity = 0.5;
    if (r->circuits_attempted > 0)
        fidelity = (double)r->circuits_completed /
                   (double)r->circuits_attempted;

    // Bandwidth honesty: min(measured/claimed, 1.0)
    double bw_honesty = 1.0;
    if (r->claimed_bandwidth > 0.0) {
        bw_honesty = r->measured_bandwidth / r->claimed_bandwidth;
        if (bw_honesty > 1.0) bw_honesty = 1.0;
        if (bw_honesty < 0.0) bw_honesty = 0.0;
    }

    return fidelity * 0.5 + bw_honesty * 0.5;
}

double trellis_behavior_uptime_correlation(
    const trellis_behavior_tracker_t *bt,
    const trellis_fingerprint_t *a,
    const trellis_fingerprint_t *b)
{
    if (!bt || !a || !b)
        return 0.0;

    const trellis_behavior_record_t *ra = behavior_find(bt, a);
    const trellis_behavior_record_t *rb = behavior_find(bt, b);
    if (!ra || !rb)
        return 0.0;

    // Pearson correlation on uptime bucket arrays.
    double sum_a = 0, sum_b = 0;
    for (int i = 0; i < TRELLIS_UPTIME_BUCKETS; i++) {
        sum_a += ra->uptime_buckets[i];
        sum_b += rb->uptime_buckets[i];
    }
    double mean_a = sum_a / TRELLIS_UPTIME_BUCKETS;
    double mean_b = sum_b / TRELLIS_UPTIME_BUCKETS;

    double cov = 0, var_a = 0, var_b = 0;
    for (int i = 0; i < TRELLIS_UPTIME_BUCKETS; i++) {
        double da = ra->uptime_buckets[i] - mean_a;
        double db = rb->uptime_buckets[i] - mean_b;
        cov += da * db;
        var_a += da * da;
        var_b += db * db;
    }

    if (var_a < 1e-12 || var_b < 1e-12)
        return 0.0;

    return cov / sqrt(var_a * var_b);
}

bool trellis_behavior_same_subnet(const trellis_behavior_tracker_t *bt,
                                  const trellis_fingerprint_t *a,
                                  const trellis_fingerprint_t *b)
{
    if (!bt || !a || !b)
        return false;

    const trellis_behavior_record_t *ra = behavior_find(bt, a);
    const trellis_behavior_record_t *rb = behavior_find(bt, b);
    if (!ra || !rb)
        return false;

    // Compare first 3 bytes (/24 for IPv4)
    return ra->addr_prefix[0] == rb->addr_prefix[0] &&
           ra->addr_prefix[1] == rb->addr_prefix[1] &&
           ra->addr_prefix[2] == rb->addr_prefix[2];
}
