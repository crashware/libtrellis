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

#ifndef TRELLIS_GREENHOUSE_INTERNAL_H
#define TRELLIS_GREENHOUSE_INTERNAL_H

#include <trellis/greenhouse.h>
#ifndef __EMSCRIPTEN__
#include "../transport/internal.h"
#endif

#include <stdlib.h>
#include <string.h>
#include <sodium.h>
#include <uv.h>
#include <oqs/oqs.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct trellis_onion_hop {
    trellis_fingerprint_t fingerprint;
    uint8_t               shared_secret[32];
    uint8_t               x25519_eph_pk[TRELLIS_X25519_PK_LEN];
    uint8_t              *ml_kem_ct;
    size_t                ml_kem_ct_len;
} trellis_onion_hop_t;

struct trellis_onion_circuit {
    trellis_onion_hop_t *hops;
    size_t               hop_count;
    size_t               max_hops;
    uint32_t             wrap_counter;
};

typedef enum trellis_rendezvous_state {
    TRELLIS_RV_WAITING  = 0,
    TRELLIS_RV_JOINED   = 1,
    TRELLIS_RV_BRIDGED  = 2,
} trellis_rendezvous_state_t;

struct trellis_rendezvous {
    uint8_t                      cookie[TRELLIS_RENDEZVOUS_COOKIE_LEN];
    trellis_rendezvous_state_t   state;
    trellis_onion_circuit_t     *client_circuit;
    trellis_onion_circuit_t     *service_circuit;
    trellis_conn_t              *client_conn;
    trellis_conn_t              *service_conn;
};

#define TRELLIS_MAX_INTRO_POINTS 6

typedef struct trellis_intro_ctx {
    trellis_intro_point_t point;
    trellis_conn_t       *conn;
    bool                  active;
    void                 *recv_ctx; /* intro_recv_ctx_t*, freed on teardown */
} trellis_intro_ctx_t;

#define TRELLIS_GH_MAX_ACL 64   /* max clients for authenticated mode */
#define TRELLIS_GH_INTRO_KEEPALIVE_MS  (5ULL * 60 * 1000)  /* 5 min */
#define TRELLIS_GH_DESCRIPTOR_TTL_MS   (3600ULL * 1000)    /* 1 hour */

struct trellis_greenhouse {
    trellis_identity_t            service_id;
    trellis_greenhouse_type_t     type;
    trellis_dht_t                *dht;
    trellis_router_t             *router;   /* optional: used to send ESTABLISH_INTRO */
    trellis_service_descriptor_t  descriptor;
    trellis_intro_ctx_t           intro_points[TRELLIS_MAX_INTRO_POINTS];
    size_t                        intro_count;
    bool                          running;
    trellis_greenhouse_conn_cb    conn_cb;
    void                         *conn_ctx;

    // Long-lived intro circuit maintenance.
    uv_loop_t                    *loop;
    uv_timer_t                    intro_keepalive;
    bool                          intro_timer_active;

    // Ephemeral mode: self-destruct after first successful connection.
    bool                          ephemeral_used;

    // Authenticated mode: allow-list of client fingerprints.
    trellis_fingerprint_t         acl[TRELLIS_GH_MAX_ACL];
    size_t                        acl_count;

    /* Federated mode: multiple nodes share a service identity via Shamir
     * secret sharing.  Each node holds a share and publishes intro points.
     * The descriptor aggregates intro points from all federation peers. */
    struct {
        trellis_fingerprint_t  peers[TRELLIS_MAX_INTRO_POINTS];
        size_t                 peer_count;
        int                    threshold;  /* k-of-n: minimum shares to sign */
        int                    total;      /* n total shares */
        uint8_t               *local_share;
        size_t                 local_share_len;
        int                    local_share_idx; /* 1-based share index */
    } federation;
};

typedef struct trellis_proxy_entry {
    char                  bloom_addr[128];
    trellis_conn_t       *tunnel;
    uint64_t              last_used;
} trellis_proxy_entry_t;

#define TRELLIS_PROXY_MAX_CONNS 64

typedef struct trellis_proxy {
    uv_loop_t            *loop;
    uv_tcp_t              listener;
    trellis_dht_t        *dht;
    trellis_proxy_entry_t cache[TRELLIS_PROXY_MAX_CONNS];
    size_t                cache_count;
    uint16_t              listen_port;
    bool                  running;
} trellis_proxy_t;

typedef struct trellis_reverse_proxy {
    trellis_greenhouse_t *greenhouse;
    char                  backend_addr[256];
    uv_loop_t            *loop;
    bool                  running;
} trellis_reverse_proxy_t;

#ifdef __cplusplus
}
#endif

#endif /* TRELLIS_GREENHOUSE_INTERNAL_H */
