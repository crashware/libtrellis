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

#ifndef TRELLIS_SDK_INTERNAL_H
#define TRELLIS_SDK_INTERNAL_H

#include <trellis/trellis.h>
#include <trellis/probe_resist.h>
#ifndef __EMSCRIPTEN__
#include "../transport/internal.h"
#endif

#include <stdlib.h>
#include <string.h>
#include <sodium.h>
#include <uv.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct trellis_relay trellis_relay_t;
typedef struct trellis_canopy trellis_canopy_t;
typedef struct trellis_bridge trellis_bridge_t;
typedef struct relay_circuit relay_circuit_t;

trellis_relay_t *trellis_relay_new(trellis_dht_t *dht, size_t max_capacity);
void             trellis_relay_free(trellis_relay_t *relay);
trellis_err_t    trellis_relay_start(trellis_relay_t *relay, uv_loop_t *loop);
trellis_err_t    trellis_relay_create_circuit(trellis_relay_t *relay,
                                              const trellis_fingerprint_t *from,
                                              const trellis_fingerprint_t *to);
trellis_err_t    trellis_relay_forward(trellis_relay_t *relay,
                                       const trellis_fingerprint_t *from,
                                       const trellis_fingerprint_t *to,
                                       const uint8_t *data, size_t len);
trellis_err_t    trellis_relay_close_circuit(trellis_relay_t *relay,
                                              const trellis_fingerprint_t *from,
                                              const trellis_fingerprint_t *to);
size_t           trellis_relay_remaining_capacity(const trellis_relay_t *relay);
void             trellis_relay_on_sendme(trellis_relay_t *relay,
                                         const trellis_fingerprint_t *from,
                                         const trellis_fingerprint_t *to);
void             trellis_relay_handle_circuit_data(trellis_relay_t *relay,
                                                    void *client_ptr,
                                                    uint32_t stream_id,
                                                    const uint8_t *data,
                                                    size_t len);
void             trellis_relay_close_circuits_for_peer(trellis_relay_t *relay,
                                                       const trellis_fingerprint_t *peer);
bool             trellis_relay_needs_sendme(trellis_relay_t *relay,
                                            const trellis_fingerprint_t *from,
                                            const trellis_fingerprint_t *to);
relay_circuit_t *trellis_relay_find_circuit(trellis_relay_t *relay,
                                            const trellis_fingerprint_t *from,
                                            const trellis_fingerprint_t *to);
trellis_err_t    trellis_relay_handle_create_session(trellis_relay_t *relay,
                                                     const trellis_fingerprint_t *from,
                                                     const uint8_t *payload,
                                                     size_t payload_len,
                                                     uint8_t *response,
                                                     size_t *resp_len);
trellis_err_t    trellis_relay_handle_session_data(trellis_relay_t *relay,
                                                    const trellis_fingerprint_t *from,
                                                    const uint8_t *payload,
                                                    size_t payload_len,
                                                    uint8_t *plaintext,
                                                    size_t *pt_len,
                                                    trellis_fingerprint_t *dest_out);

void trellis_flow_init_circuit(relay_circuit_t *c);
bool trellis_flow_can_send(const relay_circuit_t *c);
void trellis_flow_on_send(relay_circuit_t *c, size_t bytes);
bool trellis_flow_on_recv(relay_circuit_t *c, size_t bytes);
void trellis_flow_on_sendme(relay_circuit_t *c);
void trellis_flow_vegas_init(relay_circuit_t *c);
void trellis_flow_vegas_update(relay_circuit_t *c, uint64_t rtt_us);

trellis_err_t trellis_circuit_pool_start(trellis_router_t *router, uv_loop_t *loop);
void          trellis_circuit_pool_stop(trellis_router_t *router);

typedef struct trellis_event_handler {
    trellis_event_t  event;
    trellis_event_cb cb;
    void            *ctx;
} trellis_event_handler_t;

#define TRELLIS_EVENT_HANDLERS_INIT_CAP 16

typedef struct trellis_garden_entry {
    char               *name;
    trellis_garden_t   *garden;
} trellis_garden_entry_t;

#define TRELLIS_GARDENS_INIT_CAP 16

typedef struct trellis_peer_conn {
    trellis_conn_t           *conn;
    trellis_handshake_t      *handshake;
    trellis_ratchet_t        *ratchet;
    trellis_identity_public_t remote_id;
    trellis_fingerprint_t     remote_fp;
    bool                      handshake_complete;
    trellis_morph_t          *morph;
    uint64_t                  last_pong;
    char                      addr[256];
    uint64_t                  latency_ms;

    uv_timer_t               *handshake_timer;

    // Capability negotiation — populated after TRELLIS_MSG_CAPABILITIES exchange.
    trellis_capabilities_t    negotiated_caps;
    bool                      caps_received;   /* remote sent CAPABILITIES */
    bool                      relay_eligible;  /* remote advertised "relay" feature */
    bool                      greenhouse_eligible; /* remote has "greenhouse" feature */
    bool                      exit_eligible;       /* remote advertised "exit" feature */
} trellis_peer_conn_t;

#define TRELLIS_HANDSHAKE_TIMEOUT_MS 10000

#define TRELLIS_PEER_CONN_INIT_CAP 32

#define TRELLIS_EXIT_STREAM_MAX 64

struct trellis_exit_stream {
    uint32_t                   stream_id;
    trellis_fingerprint_t      exit_fp;
    trellis_exit_connect_cb    on_connect;
    trellis_exit_data_cb       on_data;
    trellis_exit_close_cb      on_close;
    void                      *ctx;
    struct trellis_client      *client;
    bool                       active;
};

typedef struct trellis_send_item {
    uint8_t                  *data;
    size_t                    len;
    struct trellis_send_item *next;
} trellis_send_item_t;

struct trellis_client {
    trellis_client_config_t  config;
    trellis_identity_t       identity;
    char                    *alias;

    uv_loop_t                loop;
    uv_thread_t              loop_thread;
    uv_async_t               stop_async;
    uv_async_t               send_async;

    uv_mutex_t               send_lock;
    trellis_send_item_t     *send_head;
    bool                     send_lock_initialized;

    trellis_transport_t     *transport;
    trellis_dht_t           *dht;
    trellis_gossip_t        *gossip;
    trellis_router_t        *router;
    trellis_relay_t         *relay;
    trellis_incentive_t     *incentive;
    trellis_topology_t      *topology;
    trellis_morph_t         *morph;

    // Guard node pinning (Tor parity)
    trellis_guard_t         *guard;
    uv_timer_t               guard_timer;
    bool                     guard_timer_active;

    trellis_garden_entry_t  *gardens;
    size_t                   garden_count;
    size_t                   garden_cap;

    trellis_peer_conn_t     *peers;
    size_t                   peer_conn_count;
    size_t                   peer_conn_cap;

    trellis_extensions_t    *extensions;
    trellis_greenhouse_t    *greenhouse;
    trellis_exit_relay_t    *exit_relay;
    bool                     exit_capable;
    trellis_exit_stream_t    exit_streams[TRELLIS_EXIT_STREAM_MAX];
    trellis_nat_t           *nat;

    // Rhizome: canopy and bridge (NULL when not in federated mode)
    trellis_canopy_t        *canopy;
    trellis_bridge_t        *bridge;

    trellis_event_handler_t *handlers;
    size_t                   handler_count;
    size_t                   handler_cap;

    uint64_t                 next_sequence;

    uv_udp_t                 discovery_udp;
    uv_timer_t               discovery_timer;
    bool                     discovery_active;
    uint16_t                 listen_port;

    uv_timer_t               cover_timer;
    bool                     cover_timer_active;

    uv_timer_t               keepalive_timer;
    bool                     keepalive_timer_active;

    uv_timer_t               pex_timer;
    bool                     pex_timer_active;

    char                   **reconnect_addrs;
    size_t                   reconnect_count;
    size_t                   reconnect_cap;

    char                     public_addr[256];

    trellis_probe_guard_t   *probe_guard;
    uint64_t                 probe_guard_epoch; /* last UTC-hour epoch used to key probe_guard */
    uv_timer_t               probe_guard_timer;
    bool                     probe_guard_timer_active;

    trellis_conn_pow_server_t *conn_pow;

    // Temporal metadata countermeasures.
    uv_timer_t               decoy_timer;
    bool                     decoy_timer_active;
    uint32_t                 decoy_interval_ms;

    // Message batching for async gardens.
    uint32_t                 msg_batch_delay_ms;
    uint32_t                 msg_batch_max;
    uv_timer_t               batch_timer;
    bool                     batch_timer_active;

    // Pending outbound batch queue (serialized wire frames)
    trellis_buf_t           *batch_queue;
    size_t                   batch_count;
    size_t                   batch_cap;

    bool                     always_on_daemon;
    bool                     running;
    bool                     loop_initialized;
    bool                     loop_thread_started;

    // Onion replay protection (P1-2)
    void                    *onion_replay; /* dedup_set_t*, forward-declared */
};

void trellis_emit(trellis_client_t *client, const trellis_event_data_t *ev);

trellis_garden_t *trellis_client_find_garden(const trellis_client_t *client,
                                             const char *name);

trellis_peer_conn_t *trellis_client_find_peer(trellis_client_t *client,
                                               const trellis_fingerprint_t *fp);
trellis_err_t trellis_client_add_peer_conn(trellis_client_t *client,
                                            trellis_peer_conn_t *pc);
void trellis_client_remove_peer_conn(trellis_client_t *client,
                                      const trellis_fingerprint_t *fp);

void trellis_client_on_accept(trellis_conn_t *conn, void *ctx);
void trellis_client_on_data(trellis_conn_t *conn, const uint8_t *data,
                             size_t len, trellis_err_t err, void *ctx);
void trellis_client_setup_peer(trellis_client_t *client, trellis_conn_t *conn,
                                trellis_handshake_role_t role);

trellis_err_t trellis_discovery_start(trellis_client_t *client);
void          trellis_discovery_stop(trellis_client_t *client);

trellis_err_t trellis_client_start_decoy_timer(trellis_client_t *client);
void          trellis_client_stop_decoy_timer(trellis_client_t *client);

// Cover traffic timer callback — shared by client.c and mobile.c.
void trellis_client_cover_tick(uv_timer_t *handle);

/* Enqueue a wire frame for batched delivery. If batching is disabled, returns
 * TRELLIS_ERR_NOT_INITIALIZED (caller falls through to immediate send). */
trellis_err_t trellis_client_batch_enqueue(trellis_client_t *client,
                                            const trellis_buf_t *wire);

// Flush all pending batch frames to queue_broadcast immediately.
void trellis_client_batch_flush(trellis_client_t *client);

#ifdef __cplusplus
}
#endif

#endif /* TRELLIS_SDK_INTERNAL_H */
