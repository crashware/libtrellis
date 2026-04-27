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

typedef struct {
    trellis_greenhouse_t *gh;
    size_t                index;
} intro_recv_ctx_t;

static void on_intro_message(trellis_conn_t *conn,
                             const uint8_t *data, size_t len,
                             trellis_err_t err, void *ctx)
{
    intro_recv_ctx_t *rc = ctx;
    if (err != TRELLIS_OK || !data || len == 0)
        return;

    if (!rc->gh->running)
        return;

    /*
     * The intro point received an INTRODUCE cell.  Parse the payload to
     * extract the client fingerprint (if present), then invoke the service's
     * connection callback.
     *
     * INTRODUCE1 payload format (set by greenhouse_connect in service.c):
     *   [4]  magic  "INT1"
     *   [1]  version = 1
     *   [32] rendezvous relay fingerprint
     *   [TRELLIS_RENDEZVOUS_COOKIE_LEN] rendezvous cookie
     *
     * The client's fingerprint is not in the current INTRODUCE payload
     * (it would require an additional KEM round-trip to extract, since
     * the payload is onion-encrypted). We recover what we can:
     *  - If len >= 5+32+RENDEZVOUS_COOKIE_LEN, we have a valid INT1 cell.
     *  - The client fingerprint is conveyed via the transport-layer source
     *    address on authenticated greenhouse types (conn->peer_fingerprint).
     *
     * For TRELLIS_GH_AUTHENTICATED mode: pass the transport-layer peer
     * fingerprint if the connection struct exposes it; otherwise NULL.
     * The service-side ACL check uses trellis_greenhouse_acl_add().
     */
    const trellis_fingerprint_t *caller_fp = NULL;

    /* Attempt to extract the caller fingerprint from the connection's peer
     * identity if the transport layer populated it. */
    if (conn && conn->peer_fingerprint_valid)
        caller_fp = &conn->peer_fingerprint;

    // For authenticated greenhouses, check the ACL before calling conn_cb.
    if (rc->gh->type == TRELLIS_GH_AUTHENTICATED && caller_fp) {
        bool acl_ok = false;
        for (size_t ai = 0; ai < rc->gh->acl_count; ai++) {
            if (trellis_fingerprint_eq(&rc->gh->acl[ai], caller_fp)) {
                acl_ok = true;
                break;
            }
        }
        if (!acl_ok && rc->gh->acl_count > 0) {
            fprintf(stderr, "[GH] INTRODUCE rejected: caller not in ACL\n");
            return;
        }
    }

    if (rc->gh->conn_cb)
        rc->gh->conn_cb(conn, caller_fp, rc->gh->conn_ctx);

    // Ephemeral mode: self-destruct descriptor after first connection.
    if (rc->gh->type == TRELLIS_GH_EPHEMERAL && !rc->gh->ephemeral_used) {
        rc->gh->ephemeral_used = true;
        uint8_t desc_key[32];
        if (trellis_bloom_addr_descriptor_key(&rc->gh->service_id.fingerprint,
                                              desc_key) == TRELLIS_OK) {
            trellis_dht_store(rc->gh->dht, desc_key, sizeof(desc_key),
                              NULL, 0, NULL, NULL);
            fprintf(stderr, "[GH] ephemeral descriptor deleted after first use\n");
        }
    }
}

trellis_err_t trellis_intro_establish(trellis_greenhouse_t *gh,
                                      trellis_conn_t *relay_conn,
                                      size_t index)
{
    if (!gh || !relay_conn || index >= TRELLIS_MAX_INTRO_POINTS)
        return TRELLIS_ERR_INVALID_ARG;

    gh->intro_points[index].conn = relay_conn;
    gh->intro_points[index].active = true;

    intro_recv_ctx_t *rc = calloc(1, sizeof(*rc));
    if (!rc)
        return TRELLIS_ERR_NOMEM;

    rc->gh = gh;
    rc->index = index;

    // Store pointer so teardown can free it.
    gh->intro_points[index].recv_ctx = rc;

    return trellis_conn_recv_start(relay_conn, on_intro_message, rc);
}

void trellis_intro_teardown(trellis_greenhouse_t *gh, size_t index)
{
    if (!gh || index >= TRELLIS_MAX_INTRO_POINTS)
        return;

    trellis_intro_ctx_t *ic = &gh->intro_points[index];
    if (ic->conn) {
        trellis_conn_close(ic->conn, NULL, NULL);
        ic->conn = NULL;
    }
    if (ic->recv_ctx) {
        free(ic->recv_ctx);
        ic->recv_ctx = NULL;
    }
    ic->active = false;
}

bool trellis_intro_is_active(const trellis_greenhouse_t *gh, size_t index)
{
    if (!gh || index >= TRELLIS_MAX_INTRO_POINTS)
        return false;
    return gh->intro_points[index].active;
}
