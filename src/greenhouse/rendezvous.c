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
#include "../transport/internal.h"

/*
 * A connected pair of trellis_conn_t objects backed by a single shared state.
 * Writing to end A synchronously delivers to end B's recv callback, and
 * vice versa.  This lets us represent a completed rendezvous as two in-process
 * conn ends without needing an actual network relay.
 */
typedef struct rv_pipe_state {
    // End A callbacks.
    trellis_conn_recv_cb a_recv_cb;
    void                *a_recv_ctx;
    // End B callbacks.
    trellis_conn_recv_cb b_recv_cb;
    void                *b_recv_ctx;

    trellis_conn_t *conn_a;
    trellis_conn_t *conn_b;

    bool closed_a;
    bool closed_b;
    int  refcount; /* freed when both ends are closed */
} rv_pipe_state_t;

static trellis_err_t rv_pipe_send_a(trellis_conn_t *conn,
                                    const uint8_t *data, size_t len,
                                    trellis_conn_send_cb cb, void *ctx)
{
    rv_pipe_state_t *ps = conn->impl_data;
    (void)conn;
    if (ps->b_recv_cb && !ps->closed_b)
        ps->b_recv_cb(ps->conn_b, data, len, TRELLIS_OK, ps->b_recv_ctx);
    if (cb) cb(TRELLIS_OK, ctx);
    return TRELLIS_OK;
}

static trellis_err_t rv_pipe_send_b(trellis_conn_t *conn,
                                    const uint8_t *data, size_t len,
                                    trellis_conn_send_cb cb, void *ctx)
{
    rv_pipe_state_t *ps = conn->impl_data;
    (void)conn;
    if (ps->a_recv_cb && !ps->closed_a)
        ps->a_recv_cb(ps->conn_a, data, len, TRELLIS_OK, ps->a_recv_ctx);
    if (cb) cb(TRELLIS_OK, ctx);
    return TRELLIS_OK;
}

static trellis_err_t rv_pipe_recv_start_a(trellis_conn_t *conn,
                                          trellis_conn_recv_cb cb, void *ctx)
{
    rv_pipe_state_t *ps = conn->impl_data;
    ps->a_recv_cb  = cb;
    ps->a_recv_ctx = ctx;
    return TRELLIS_OK;
}

static trellis_err_t rv_pipe_recv_start_b(trellis_conn_t *conn,
                                          trellis_conn_recv_cb cb, void *ctx)
{
    rv_pipe_state_t *ps = conn->impl_data;
    ps->b_recv_cb  = cb;
    ps->b_recv_ctx = ctx;
    return TRELLIS_OK;
}

static void rv_pipe_recv_stop_a(trellis_conn_t *conn)
{
    rv_pipe_state_t *ps = conn->impl_data;
    ps->a_recv_cb  = NULL;
    ps->a_recv_ctx = NULL;
}

static void rv_pipe_recv_stop_b(trellis_conn_t *conn)
{
    rv_pipe_state_t *ps = conn->impl_data;
    ps->b_recv_cb  = NULL;
    ps->b_recv_ctx = NULL;
}

static void rv_pipe_close_a(trellis_conn_t *conn,
                             trellis_conn_close_cb cb, void *ctx)
{
    rv_pipe_state_t *ps = conn->impl_data;
    ps->closed_a = true;
    // Notify b that its peer closed.
    if (ps->b_recv_cb && !ps->closed_b)
        ps->b_recv_cb(ps->conn_b, NULL, 0, TRELLIS_ERR_CLOSED, ps->b_recv_ctx);
    if (cb) cb(conn, ctx);
    conn->impl_data = NULL;
    if (--ps->refcount == 0) free(ps);
    trellis_conn_dealloc(conn);
}

static void rv_pipe_close_b(trellis_conn_t *conn,
                             trellis_conn_close_cb cb, void *ctx)
{
    rv_pipe_state_t *ps = conn->impl_data;
    ps->closed_b = true;
    if (ps->a_recv_cb && !ps->closed_a)
        ps->a_recv_cb(ps->conn_a, NULL, 0, TRELLIS_ERR_CLOSED, ps->a_recv_ctx);
    if (cb) cb(conn, ctx);
    conn->impl_data = NULL;
    if (--ps->refcount == 0) free(ps);
    trellis_conn_dealloc(conn);
}

static const trellis_conn_vtable_t rv_pipe_vtable_a = {
    .send       = rv_pipe_send_a,
    .recv_start = rv_pipe_recv_start_a,
    .recv_stop  = rv_pipe_recv_stop_a,
    .close      = rv_pipe_close_a,
};

static const trellis_conn_vtable_t rv_pipe_vtable_b = {
    .send       = rv_pipe_send_b,
    .recv_start = rv_pipe_recv_start_b,
    .recv_stop  = rv_pipe_recv_stop_b,
    .close      = rv_pipe_close_b,
};

trellis_err_t trellis_rendezvous_conn_pair(trellis_conn_t **a_out,
                                            trellis_conn_t **b_out)
{
    if (!a_out || !b_out)
        return TRELLIS_ERR_INVALID_ARG;

    rv_pipe_state_t *ps = calloc(1, sizeof(*ps));
    if (!ps)
        return TRELLIS_ERR_NOMEM;
    ps->refcount = 2;

    trellis_conn_t *a = trellis_conn_alloc(&rv_pipe_vtable_a, "rv-pipe", NULL);
    trellis_conn_t *b = trellis_conn_alloc(&rv_pipe_vtable_b, "rv-pipe", NULL);
    if (!a || !b) {
        free(a); free(b); free(ps);
        return TRELLIS_ERR_NOMEM;
    }

    a->impl_data = ps;
    b->impl_data = ps;
    ps->conn_a = a;
    ps->conn_b = b;

    *a_out = a;
    *b_out = b;
    return TRELLIS_OK;
}

trellis_rendezvous_t *trellis_rendezvous_new(void)
{
    trellis_rendezvous_t *rv = calloc(1, sizeof(*rv));
    if (!rv)
        return NULL;
    rv->state = TRELLIS_RV_WAITING;
    return rv;
}

void trellis_rendezvous_free(trellis_rendezvous_t *rv)
{
    if (!rv)
        return;

    if (rv->client_conn)
        trellis_conn_recv_stop(rv->client_conn);
    if (rv->service_conn)
        trellis_conn_recv_stop(rv->service_conn);

    trellis_onion_free(rv->client_circuit);
    trellis_onion_free(rv->service_circuit);
    sodium_memzero(rv->cookie, TRELLIS_RENDEZVOUS_COOKIE_LEN);  /* paranoia */
    free(rv);
}

trellis_err_t trellis_rendezvous_establish(trellis_rendezvous_t *rv,
                                            uint8_t *cookie_out)
{
    if (!rv || !cookie_out)
        return TRELLIS_ERR_INVALID_ARG;

    if (rv->state != TRELLIS_RV_WAITING)
        return TRELLIS_ERR_RENDEZVOUS;

    randombytes_buf(rv->cookie, TRELLIS_RENDEZVOUS_COOKIE_LEN);
    memcpy(cookie_out, rv->cookie, TRELLIS_RENDEZVOUS_COOKIE_LEN);

    return TRELLIS_OK;
}

trellis_err_t trellis_rendezvous_join(trellis_rendezvous_t *rv,
                                       const uint8_t *cookie)
{
    if (!rv || !cookie)
        return TRELLIS_ERR_INVALID_ARG;

    if (rv->state != TRELLIS_RV_WAITING)
        return TRELLIS_ERR_RENDEZVOUS;

    memcpy(rv->cookie, cookie, TRELLIS_RENDEZVOUS_COOKIE_LEN);
    rv->state = TRELLIS_RV_JOINED;
    return TRELLIS_OK;
}

typedef struct rv_bridge_ctx {
    trellis_rendezvous_t *rv;
    trellis_conn_t       *forward_to;
} rv_bridge_ctx_t;

static void rv_forward_cb(trellis_conn_t *conn, const uint8_t *data,
                           size_t len, trellis_err_t err, void *ctx)
{
    rv_bridge_ctx_t *bc = ctx;
    (void)conn;

    if (err != TRELLIS_OK || !data || len == 0) {
        /* One side closed — mark the rendezvous as no longer bridged so the
         * other direction's callback stops forwarding once it fires. */
        bc->rv->state = TRELLIS_RV_WAITING;
        free(bc);
        return;
    }

    /* Only forward if the rendezvous is still alive. After one side closes,
     * rv->state is reset to TRELLIS_RV_WAITING, preventing us from sending
     * into a half-closed pipe. */
    if (bc->rv->state == TRELLIS_RV_BRIDGED)
        trellis_conn_send(bc->forward_to, data, len, NULL, NULL);
}

trellis_err_t trellis_rendezvous_bridge(trellis_rendezvous_t *rv)
{
    if (!rv)
        return TRELLIS_ERR_INVALID_ARG;

    if (rv->state == TRELLIS_RV_BRIDGED)
        return TRELLIS_ERR_RENDEZVOUS;

    if (!rv->client_conn || !rv->service_conn) {
        rv->state = TRELLIS_RV_BRIDGED;
        return TRELLIS_OK;
    }

    // Start bidirectional forwarding between client and service.
    rv_bridge_ctx_t *c2s = calloc(1, sizeof(*c2s));
    rv_bridge_ctx_t *s2c = calloc(1, sizeof(*s2c));
    if (!c2s || !s2c) {
        free(c2s);
        free(s2c);
        return TRELLIS_ERR_NOMEM;
    }

    c2s->rv = rv;
    c2s->forward_to = rv->service_conn;
    s2c->rv = rv;
    s2c->forward_to = rv->client_conn;

    trellis_conn_recv_start(rv->client_conn, rv_forward_cb, c2s);
    trellis_conn_recv_start(rv->service_conn, rv_forward_cb, s2c);

    rv->state = TRELLIS_RV_BRIDGED;
    return TRELLIS_OK;
}
