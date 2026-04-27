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

#ifndef TRELLIS_CLIENT_H
#define TRELLIS_CLIENT_H

#include "types.h"
#include "error.h"
#include "identity.h"
#include "transport.h"
#include "crypto.h"
#include "mesh.h"
#include "morph.h"
#include "protocol.h"
#include "greenhouse.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum trellis_event {
    TRELLIS_EV_STARTED              = 0,
    TRELLIS_EV_STOPPED              = 1,
    TRELLIS_EV_PEER_ADDED           = 2,
    TRELLIS_EV_PEER_REMOVED         = 3,
    TRELLIS_EV_HANDSHAKE_COMPLETE   = 4,
    TRELLIS_EV_MESSAGE              = 5,
    TRELLIS_EV_GARDEN_CREATED       = 6,
    TRELLIS_EV_GARDEN_SUBSCRIBED    = 7,
    TRELLIS_EV_GARDEN_UNSUBSCRIBED  = 8,
    TRELLIS_EV_GOSSIP               = 9,
    TRELLIS_EV_GREENHOUSE_STARTED   = 10,
    TRELLIS_EV_GREENHOUSE_CONNECTION = 11,
    TRELLIS_EV_MORPH_CHANGED        = 12,
    TRELLIS_EV_ROUTING_CHANGED      = 13,
    TRELLIS_EV_ALIAS_CHANGED        = 14,
    TRELLIS_EV_ECLIPSE_DETECTED     = 15,
    TRELLIS_EV_GREENHOUSE_ERROR     = 16,
} trellis_event_t;

typedef struct trellis_event_data {
    trellis_event_t      event;
    trellis_fingerprint_t peer;
    const char           *garden_name;
    const uint8_t        *data;
    size_t                data_len;
    const char           *alias;
} trellis_event_data_t;

typedef void (*trellis_event_cb)(const trellis_event_data_t *ev, void *ctx);

typedef struct trellis_client_config {
    const char              *seed_addr;
    const char              *listen_addr;
    const char              *identity_path;
    trellis_transport_mode_t transport_mode;
    trellis_routing_mode_t   default_routing;
    bool                     morph_metamorphic;
    uint32_t                 cover_traffic_ms;  /* Cover traffic interval; default 5000 ms, 0 to disable */
    bool                     disable_discovery;
    const char              *pt_binary;
    /* PT transport name passed to the binary via TOR_PT_CLIENT_TRANSPORTS /
     * TOR_PT_SERVER_TRANSPORTS (e.g. "obfs4", "snowflake", "webtunnel").
     * Defaults to "obfs4" when NULL. */
    const char              *pt_transport_name;
    const char              *pt_args;
    /* Wire-level camouflage applied on top of the transport layer.
     * Defaults to TRELLIS_CAMO_RAW_OBFS for raw/PT transports.
     * Set to TRELLIS_CAMO_NONE when using the TLS transport to avoid
     * double-wrapping (TLS mimic inside real TLS). */
    trellis_camouflage_type_t wire_camouflage_type;
    bool                      verify_peer;

    /*
     * always_on_daemon: keep the event loop running indefinitely even when
     * the application has no pending messages, ensuring the node maintains
     * stable connectivity and prevents timing correlation from connections
     * appearing only when real traffic is sent.
     *
     * decoy_circuit_interval_ms: if > 0, generate decoy onion circuits to
     * random greenhouse addresses (or tombstone fingerprints) at this interval.
     * This hides which greenhouse services are being actively accessed.
     *
     * msg_batch_delay_ms: if > 0, outgoing messages to async gardens are held
     * for up to this many milliseconds and sent as a batch, blurring the
     * relationship between user actions and sent packets.
     *
     * msg_batch_max: maximum number of messages to accumulate before flushing
     * the batch regardless of the delay timer.  0 means no limit.
     */
    bool     always_on_daemon;
    uint32_t decoy_circuit_interval_ms;
    uint32_t msg_batch_delay_ms;
    uint32_t msg_batch_max;

    /* Per-connection PoW iterations (0 = default TRELLIS_CONN_POW_DEFAULT_ITERS).
     * Set to UINT32_MAX to disable PoW gating entirely. */
    uint32_t conn_pow_iterations;

    /*
     * guard_enabled: create and attach a Tor Proposal 292-style guard set to
     * the router so onion circuits prefer pinned entry nodes. Requires DHT
     * with Sybil subsystem (always available in standard builds).
     *
     * guard_persist_path: file path for persisting guard state across
     * restarts. If NULL and identity_path is set, defaults to
     * "<identity_path>.guards".
     *
     * morph_cell_mode: fragment all outgoing frames into fixed-size cells,
     * defeating packet-length fingerprinting (analogous to Tor's 512-byte
     * cells).
     *
     * morph_cell_size: cell size in bytes when morph_cell_mode is true.
     */
    bool         guard_enabled;
    const char  *guard_persist_path;
    bool         morph_cell_mode;
    uint32_t     morph_cell_size;

    /* Skip preemptive onion circuit pool building (useful for relay bridges
     * that only use relay/adaptive routing and don't need 3-hop circuits). */
    bool         disable_circuit_pool;

    // TLS censorship resistance passthrough.
    const char  *tls_front_host;       /* domain fronting CDN host */
    const char  *tls_relay_target;     /* domain fronting relay target */
    const char  *tls_ech_public_name;  /* ECH outer SNI */
} trellis_client_config_t;

trellis_client_config_t trellis_client_config_default(void);

typedef struct trellis_client trellis_client_t;

trellis_client_t *trellis_client_new(const trellis_client_config_t *cfg);
void              trellis_client_free(trellis_client_t *client);

trellis_err_t trellis_client_start(trellis_client_t *client);
trellis_err_t trellis_client_stop(trellis_client_t *client);

trellis_err_t trellis_client_on(trellis_client_t *client,
                                 trellis_event_t event,
                                 trellis_event_cb cb, void *ctx);

// Identity persistence.
trellis_err_t trellis_identity_save(const trellis_identity_t *id, const char *path);
trellis_err_t trellis_identity_load(const char *path, trellis_identity_t *id);
char         *trellis_identity_default_path(void);

// Identity
const trellis_identity_t *trellis_client_identity(const trellis_client_t *client);
trellis_err_t trellis_client_set_alias(trellis_client_t *client,
                                        const char *alias);
const char   *trellis_client_alias(const trellis_client_t *client);

// Garden operations.
trellis_err_t trellis_client_create_garden(trellis_client_t *client,
                                            const char *name,
                                            trellis_garden_type_t type);
trellis_err_t trellis_client_join_garden(trellis_client_t *client,
                                          const char *name);
trellis_err_t trellis_client_leave_garden(trellis_client_t *client,
                                           const char *name);
trellis_err_t trellis_client_send_message(trellis_client_t *client,
                                           const char *garden_name,
                                           const uint8_t *data, size_t len);
trellis_err_t trellis_client_send_dm(trellis_client_t *client,
                                      const trellis_fingerprint_t *target,
                                      const uint8_t *data, size_t len);
trellis_garden_t *trellis_client_find_garden(const trellis_client_t *client,
                                              const char *name);

// Connect to a remote peer by address.
trellis_err_t trellis_client_connect_addr(trellis_client_t *client,
                                           const char *addr);

// Mesh
trellis_err_t trellis_client_set_routing(trellis_client_t *client,
                                          trellis_routing_mode_t mode);
trellis_err_t trellis_client_set_transport_mode(trellis_client_t *client,
                                                trellis_transport_mode_t mode);
size_t        trellis_client_peer_count(const trellis_client_t *client);
size_t        trellis_client_conn_count(const trellis_client_t *client);
const char   *trellis_client_listen_addr(const trellis_client_t *client);

// Greenhouse
trellis_err_t trellis_client_start_greenhouse(trellis_client_t *client,
                                               trellis_greenhouse_type_t type,
                                               trellis_greenhouse_conn_cb cb,
                                               void *ctx);
trellis_err_t trellis_client_connect_greenhouse(trellis_client_t *client,
                                                 const char *bloom_uri,
                                                 trellis_connect_cb cb,
                                                 void *ctx);

/* Exit relay: build a 3-hop onion circuit to an exit-capable peer and
 * send a CONNECT request for host:port through the exit relay. */
trellis_err_t trellis_client_connect_exit(trellis_client_t *client,
                                           const char *host, uint16_t port);

typedef struct trellis_exit_stream trellis_exit_stream_t;

typedef void (*trellis_exit_connect_cb)(trellis_exit_stream_t *stream,
                                        trellis_err_t status, void *ctx);
typedef void (*trellis_exit_data_cb)(trellis_exit_stream_t *stream,
                                     const uint8_t *data, size_t len,
                                     void *ctx);
typedef void (*trellis_exit_close_cb)(trellis_exit_stream_t *stream,
                                      void *ctx);

trellis_err_t trellis_client_connect_exit_ex(trellis_client_t *client,
                                              const char *host, uint16_t port,
                                              trellis_exit_connect_cb on_connect,
                                              trellis_exit_data_cb on_data,
                                              trellis_exit_close_cb on_close,
                                              void *ctx,
                                              trellis_exit_stream_t **stream_out);

trellis_err_t trellis_exit_stream_write(trellis_exit_stream_t *stream,
                                         const uint8_t *data, size_t len);
trellis_err_t trellis_exit_stream_close(trellis_exit_stream_t *stream);

// Internal subsystem accessors (for relay / operator tooling integration)
uv_loop_t            *trellis_client_get_loop(trellis_client_t *client);
trellis_dht_t        *trellis_client_get_dht(trellis_client_t *client);
trellis_router_t     *trellis_client_get_router(trellis_client_t *client);
trellis_incentive_t  *trellis_client_get_incentive(trellis_client_t *client);

/* Enable clearnet exit relay on the client.  Creates a trellis_exit_relay_t
 * on the client's event loop and marks the client as exit-capable.
 * Must be called after trellis_client_start(). */
trellis_err_t trellis_client_enable_exit_relay(trellis_client_t *client);
trellis_exit_relay_t *trellis_client_get_exit_relay(trellis_client_t *client);

/* Rhizome: attach canopy and bridge subsystems.
 * Must be called after trellis_client_new(), before trellis_client_start(). */
struct trellis_canopy;
struct trellis_bridge;
trellis_err_t trellis_client_set_canopy(trellis_client_t *client,
                                        struct trellis_canopy *canopy);
trellis_err_t trellis_client_set_bridge(trellis_client_t *client,
                                        struct trellis_bridge *bridge);

// Discovery
trellis_err_t trellis_client_discover_gardens(trellis_client_t *client);
trellis_err_t trellis_client_lookup(trellis_client_t *client,
                                     const trellis_fingerprint_t *target);
bool          trellis_client_discovery_active(const trellis_client_t *client);

typedef struct trellis_ctrl trellis_ctrl_t;

/*
 * Start a control interface on a Unix domain socket.
 * Accepts newline-delimited JSON-RPC 2.0 from external tooling.
 *
 * socket_path: filesystem path for the Unix socket (e.g. "/tmp/trellis.sock")
 * cookie_path: if non-NULL, generate a random cookie file for authentication;
 *              clients must send {"method":"auth","params":{"cookie":"<hex>"}}
 *              before any other command.
 */
trellis_ctrl_t *trellis_ctrl_new(trellis_client_t *client,
                                 uv_loop_t *loop,
                                 const char *socket_path,
                                 const char *cookie_path);
void             trellis_ctrl_free(trellis_ctrl_t *ctrl);
const char      *trellis_ctrl_socket_path(const trellis_ctrl_t *ctrl);

typedef struct trellis_metrics trellis_metrics_t;

trellis_metrics_t *trellis_metrics_new(uv_loop_t *loop);
void trellis_metrics_free(trellis_metrics_t *m);

// Start Prometheus HTTP endpoint on localhost:port.
trellis_err_t trellis_metrics_start_http(trellis_metrics_t *m, int port);
void          trellis_metrics_stop_http(trellis_metrics_t *m);

// Generate a scrape payload (Prometheus text format) into buf.
size_t trellis_metrics_scrape(const trellis_metrics_t *m,
                               char *buf, size_t buf_cap);

// Counter/gauge update functions (call from relay, DHT, etc.)
void trellis_metrics_inc_bytes_relayed(trellis_metrics_t *m, size_t n);
void trellis_metrics_inc_bytes_sent(trellis_metrics_t *m, size_t n);
void trellis_metrics_inc_bytes_received(trellis_metrics_t *m, size_t n);
void trellis_metrics_inc_cover_bytes(trellis_metrics_t *m, size_t n);
void trellis_metrics_inc_messages_sent(trellis_metrics_t *m);
void trellis_metrics_inc_messages_received(trellis_metrics_t *m);
void trellis_metrics_inc_messages_dropped(trellis_metrics_t *m);
void trellis_metrics_inc_relay_circuit(trellis_metrics_t *m);
void trellis_metrics_dec_relay_circuit(trellis_metrics_t *m);
void trellis_metrics_set_relay_circuits(trellis_metrics_t *m, uint64_t count);
void trellis_metrics_inc_dht_store(trellis_metrics_t *m);
void trellis_metrics_inc_dht_lookup(trellis_metrics_t *m);
void trellis_metrics_set_dht_peers(trellis_metrics_t *m, uint64_t count);
void trellis_metrics_set_peer_connections(trellis_metrics_t *m, uint64_t count);
void trellis_metrics_inc_handshake_ok(trellis_metrics_t *m);
void trellis_metrics_inc_handshake_fail(trellis_metrics_t *m);
void trellis_metrics_inc_error(trellis_metrics_t *m, trellis_err_t err);
void trellis_metrics_inc_greenhouse_conn(trellis_metrics_t *m);
void trellis_metrics_inc_garden_message(trellis_metrics_t *m);

typedef struct trellis_mobile_profile trellis_mobile_profile_t;

/*
 * Optional platform callback to supply battery state.
 * Set *charging = true if device is on AC power.
 * Set *capacity_pct = 0-100 (or -1 if unknown).
 */
typedef void (*trellis_mobile_battery_cb)(bool *charging, int *capacity_pct,
                                           void *ctx);

trellis_mobile_profile_t *trellis_mobile_profile_new(trellis_client_t *client,
                                                       uv_loop_t *loop);
void trellis_mobile_profile_free(trellis_mobile_profile_t *m);

trellis_err_t trellis_mobile_profile_apply(trellis_mobile_profile_t *m,
                                            bool consume_only,
                                            uint32_t cover_budget_bytes_sec,
                                            trellis_mobile_battery_cb battery_cb,
                                            void *battery_ctx);

void     trellis_mobile_set_battery(trellis_mobile_profile_t *m,
                                     bool charging, int capacity_pct);
uint32_t trellis_mobile_effective_cover_rate(const trellis_mobile_profile_t *m);

// Performance profiling (useful for WASM / mobile debugging)
int    trellis_mobile_perf_begin(trellis_mobile_profile_t *m, const char *label);
void   trellis_mobile_perf_end(trellis_mobile_profile_t *m, int token,
                                 size_t bytes);
void   trellis_mobile_perf_bytes(trellis_mobile_profile_t *m, bool tx,
                                   size_t bytes);
size_t trellis_mobile_perf_report(const trellis_mobile_profile_t *m,
                                    char *buf, size_t buf_cap);

#ifdef __cplusplus
}
#endif

#endif /* TRELLIS_CLIENT_H */
