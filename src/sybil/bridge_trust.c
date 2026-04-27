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
 * bridge_trust.c — Bridge-Specific Trust and Reputation
 *
 * Manages trust scoring for bridge nodes that relay traffic across grove
 * boundaries.  Each bridge accumulates a reputation based on:
 *   - Successful cross-grove relays (increase score)
 *   - Failed or timed-out relays (decrease score)
 *   - Verified bridge attestation (baseline trust)
 *   - Subnet diversity across bridges to prevent isolation attacks
 *
 * Bridge trust is separate from the per-grove Sybil scoring; it represents
 * inter-grove routing reliability rather than intra-grove identity trust.
 */

#include <trellis/rhizome.h>
#include <trellis/sybil.h>
#include <sodium.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#else
#include <arpa/inet.h>
#include <netinet/in.h>
#endif

#define BRIDGE_TRUST_MAX_ENTRIES   256
#define BRIDGE_TRUST_DECAY         0.95  /* EWMA decay per update */
#define BRIDGE_TRUST_INITIAL       0.5
#define BRIDGE_TRUST_SUCCESS_DELTA 0.05
#define BRIDGE_TRUST_FAILURE_DELTA 0.15

typedef struct bridge_trust_entry {
    trellis_fingerprint_t bridge_fp;
    double                score;
    uint64_t              success_count;
    uint64_t              failure_count;
    uint64_t              last_updated;
    bool                  attested;     /* has a verified attestation */
    bool                  occupied;

    // Subnet tracking for diversity checks.
    uint16_t              subnet_prefix; /* /16 from last known address */

    // Grove associations from attestations.
    trellis_grove_id_t    groves[TRELLIS_BRIDGE_MAX_GROVES];
    size_t                grove_count;
} bridge_trust_entry_t;

struct trellis_bridge_trust {
    trellis_sybil_t       *sybil;  /* parent sybil subsystem (may be NULL) */
    bridge_trust_entry_t   entries[BRIDGE_TRUST_MAX_ENTRIES];
    size_t                 entry_count;
};

static bridge_trust_entry_t *find_entry(trellis_bridge_trust_t *bt,
                                         const trellis_fingerprint_t *fp)
{
    for (size_t i = 0; i < BRIDGE_TRUST_MAX_ENTRIES; i++) {
        if (bt->entries[i].occupied &&
            trellis_fingerprint_eq(&bt->entries[i].bridge_fp, fp))
            return &bt->entries[i];
    }
    return NULL;
}

static bridge_trust_entry_t *alloc_entry(trellis_bridge_trust_t *bt,
                                          const trellis_fingerprint_t *fp)
{
    bridge_trust_entry_t *existing = find_entry(bt, fp);
    if (existing)
        return existing;

    // Find free slot.
    for (size_t i = 0; i < BRIDGE_TRUST_MAX_ENTRIES; i++) {
        if (!bt->entries[i].occupied) {
            memset(&bt->entries[i], 0, sizeof(bt->entries[i]));
            bt->entries[i].occupied = true;
            bt->entries[i].bridge_fp = *fp;
            bt->entries[i].score = BRIDGE_TRUST_INITIAL;
            bt->entries[i].last_updated = trellis_now_ms();
            bt->entry_count++;
            return &bt->entries[i];
        }
    }

    // Evict lowest-score entry.
    size_t worst = 0;
    double worst_score = 1.0;
    for (size_t i = 0; i < BRIDGE_TRUST_MAX_ENTRIES; i++) {
        if (bt->entries[i].score < worst_score) {
            worst_score = bt->entries[i].score;
            worst = i;
        }
    }

    memset(&bt->entries[worst], 0, sizeof(bt->entries[worst]));
    bt->entries[worst].occupied = true;
    bt->entries[worst].bridge_fp = *fp;
    bt->entries[worst].score = BRIDGE_TRUST_INITIAL;
    bt->entries[worst].last_updated = trellis_now_ms();
    return &bt->entries[worst];
}

trellis_bridge_trust_t *trellis_bridge_trust_new(trellis_sybil_t *sybil)
{
    trellis_bridge_trust_t *bt = calloc(1, sizeof(*bt));
    if (!bt)
        return NULL;
    bt->sybil = sybil;
    return bt;
}

void trellis_bridge_trust_free(trellis_bridge_trust_t *bt)
{
    free(bt);
}

double trellis_bridge_trust_score(const trellis_bridge_trust_t *bt,
                                  const trellis_fingerprint_t *bridge_fp)
{
    if (!bt || !bridge_fp)
        return 0.0;

    for (size_t i = 0; i < BRIDGE_TRUST_MAX_ENTRIES; i++) {
        if (bt->entries[i].occupied &&
            trellis_fingerprint_eq(&bt->entries[i].bridge_fp, bridge_fp))
            return bt->entries[i].score;
    }

    return BRIDGE_TRUST_INITIAL;
}

void trellis_bridge_trust_record_success(trellis_bridge_trust_t *bt,
                                         const trellis_fingerprint_t *bridge_fp)
{
    if (!bt || !bridge_fp)
        return;

    bridge_trust_entry_t *e = alloc_entry(bt, bridge_fp);
    if (!e)
        return;

    e->success_count++;
    e->score = BRIDGE_TRUST_DECAY * e->score + (1.0 - BRIDGE_TRUST_DECAY) * 1.0;
    if (e->score > 1.0)
        e->score = 1.0;
    e->last_updated = trellis_now_ms();
}

void trellis_bridge_trust_record_failure(trellis_bridge_trust_t *bt,
                                         const trellis_fingerprint_t *bridge_fp)
{
    if (!bt || !bridge_fp)
        return;

    bridge_trust_entry_t *e = alloc_entry(bt, bridge_fp);
    if (!e)
        return;

    e->failure_count++;
    e->score = BRIDGE_TRUST_DECAY * e->score + (1.0 - BRIDGE_TRUST_DECAY) * 0.0;
    if (e->score < 0.0)
        e->score = 0.0;
    e->last_updated = trellis_now_ms();
}

trellis_err_t trellis_bridge_trust_register(trellis_bridge_trust_t *bt,
                                            const trellis_bridge_attestation_t *att,
                                            const trellis_identity_public_t *pub_a,
                                            const trellis_identity_public_t *pub_b)
{
    if (!bt || !att || !pub_a || !pub_b)
        return TRELLIS_ERR_INVALID_ARG;

    // Verify the attestation.
    trellis_err_t err = trellis_bridge_attestation_verify(att, pub_a, pub_b);
    if (err != TRELLIS_OK)
        return err;

    // Register bridge in grove A context.
    bridge_trust_entry_t *ea = alloc_entry(bt, &att->fp_in_a);
    if (ea) {
        ea->attested = true;
        ea->score = fmax(ea->score, 0.6);
        for (int g = 0; g < 2; g++) {
            const trellis_grove_id_t *gid = g == 0 ? &att->grove_a : &att->grove_b;
            bool dup = false;
            for (size_t k = 0; k < ea->grove_count; k++)
                if (trellis_grove_id_eq(&ea->groves[k], gid)) { dup = true; break; }
            if (!dup && ea->grove_count < TRELLIS_BRIDGE_MAX_GROVES)
                ea->groves[ea->grove_count++] = *gid;
        }
    }

    // Register bridge in grove B context.
    bridge_trust_entry_t *eb = alloc_entry(bt, &att->fp_in_b);
    if (eb) {
        eb->attested = true;
        eb->score = fmax(eb->score, 0.6);
        for (int g = 0; g < 2; g++) {
            const trellis_grove_id_t *gid = g == 0 ? &att->grove_a : &att->grove_b;
            bool dup = false;
            for (size_t k = 0; k < eb->grove_count; k++)
                if (trellis_grove_id_eq(&eb->groves[k], gid)) { dup = true; break; }
            if (!dup && eb->grove_count < TRELLIS_BRIDGE_MAX_GROVES)
                eb->groves[eb->grove_count++] = *gid;
        }
    }

    return TRELLIS_OK;
}

void trellis_bridge_trust_record_addr(trellis_bridge_trust_t *bt,
                                       const trellis_fingerprint_t *bridge_fp,
                                       const struct sockaddr *addr)
{
    if (!bt || !bridge_fp || !addr)
        return;

    bridge_trust_entry_t *e = find_entry(bt, bridge_fp);
    if (!e)
        e = alloc_entry(bt, bridge_fp);
    if (!e)
        return;

    if (addr->sa_family == AF_INET) {
        // Extract the /16 prefix from the IPv4 address (first 2 octets).
        const struct sockaddr_in *sin = (const struct sockaddr_in *)addr;
        uint32_t ip = ntohl(sin->sin_addr.s_addr);
        e->subnet_prefix = (uint16_t)(ip >> 16);
    } else if (addr->sa_family == AF_INET6) {
        /* Extract a /48 prefix for IPv6 diversity — hash first 6 bytes
         * (network + site) down to a uint16_t to fit the existing field. */
        const struct sockaddr_in6 *sin6 = (const struct sockaddr_in6 *)addr;
        const uint8_t *b = sin6->sin6_addr.s6_addr;
        e->subnet_prefix = (uint16_t)((b[0] ^ b[2] ^ b[4]) << 8 |
                                       (b[1] ^ b[3] ^ b[5]));
        if (e->subnet_prefix == 0)
            e->subnet_prefix = 1;
    }
}

bool trellis_bridge_trust_diversity_ok(const trellis_bridge_trust_t *bt,
                                       const trellis_grove_id_t *grove_id)
{
    if (!bt || !grove_id)
        return false;

    /*
     * Check that bridges to this grove come from at least 2 different /16
     * subnets.  This prevents a single network operator from controlling
     * all ingress/egress for a grove.
     */
    uint16_t subnets[BRIDGE_TRUST_MAX_ENTRIES];
    size_t subnet_count = 0;

    for (size_t i = 0; i < BRIDGE_TRUST_MAX_ENTRIES; i++) {
        if (!bt->entries[i].occupied)
            continue;

        // Check if this bridge serves the target grove.
        bool serves_grove = false;
        for (size_t g = 0; g < bt->entries[i].grove_count; g++) {
            if (trellis_grove_id_eq(&bt->entries[i].groves[g], grove_id)) {
                serves_grove = true;
                break;
            }
        }
        if (!serves_grove)
            continue;

        uint16_t sn = bt->entries[i].subnet_prefix;
        if (sn == 0)
            continue;

        // Check if we've seen this subnet before.
        bool seen = false;
        for (size_t s = 0; s < subnet_count; s++) {
            if (subnets[s] == sn) {
                seen = true;
                break;
            }
        }
        if (!seen && subnet_count < BRIDGE_TRUST_MAX_ENTRIES)
            subnets[subnet_count++] = sn;
    }

    return subnet_count >= 2;
}
