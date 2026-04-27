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

#ifndef TRELLIS_MESH_H
#define TRELLIS_MESH_H

#include "types.h"
#include "error.h"
#include "identity.h"
#include "transport.h"

// Forward declaration — sybil.h is optional for consumers that only need DHT.
#ifndef TRELLIS_SYBIL_TYPEDEF_DEFINED
#define TRELLIS_SYBIL_TYPEDEF_DEFINED
typedef struct trellis_sybil trellis_sybil_t;
#endif

#ifndef TRELLIS_RATCHET_TYPEDEF_DEFINED
#define TRELLIS_RATCHET_TYPEDEF_DEFINED
typedef struct trellis_ratchet trellis_ratchet_t;
#endif

#ifdef __cplusplus
extern "C" {
#endif

// --- Kademlia DHT (Section 6.1) ---
#define TRELLIS_DHT_K          20
#define TRELLIS_DHT_BUCKETS    256
#define TRELLIS_DHT_ID_BITS    256

typedef struct trellis_dht trellis_dht_t;

/* NOTE: trellis_dht_rpc_t is a legacy logical enum for documentation purposes.
 * The wire format uses trellis_msg_type_t values from protocol.h:
 *   PING=0x0020, PONG=0x0021, FIND_NODE=0x0022, FIND_VALUE=0x0023, STORE=0x0024
 * Do NOT use these enum values to construct or dispatch wire messages. */
typedef enum trellis_dht_rpc {
    TRELLIS_RPC_PING            = 0,   /* wire: TRELLIS_MSG_PING  (0x0020) */
    TRELLIS_RPC_PONG            = 1,   /* wire: TRELLIS_MSG_PONG  (0x0021) */
    TRELLIS_RPC_STORE           = 2,   /* wire: TRELLIS_MSG_STORE (0x0024) */
    TRELLIS_RPC_STORE_ACK       = 3,   /* wire: TRELLIS_MSG_STORE (ACK variant) */
    TRELLIS_RPC_FIND_NODE       = 4,   /* wire: TRELLIS_MSG_FIND_NODE  (0x0022) */
    TRELLIS_RPC_FIND_NODE_RESULT = 5,  /* wire: TRELLIS_MSG_FIND_NODE  (result variant) */
    TRELLIS_RPC_FIND_VALUE      = 6,   /* wire: TRELLIS_MSG_FIND_VALUE (0x0023) */
    TRELLIS_RPC_FIND_VALUE_RESULT = 7, /* wire: TRELLIS_MSG_FIND_VALUE (result variant) */
} trellis_dht_rpc_t;

typedef struct trellis_peer_info {
    trellis_fingerprint_t   fingerprint;
    trellis_identity_public_t identity;
    char                    addr[256];
    double                  latency_ms;
    double                  bandwidth;
    double                  reliability;
    uint64_t                last_seen;
    /* first_seen: unix-epoch milliseconds when this peer was first added to
     * the routing table.  Used by guard selection to require a minimum age
     * (GUARD_MIN_AGE_MS) before a peer is eligible as a guard node.
     * Zero means unknown; treated as "very old" for backward compatibility. */
    uint64_t                first_seen;

    // Sybil resistance fields.
    struct trellis_vdf_proof *vdf_proof;  /* NULL if no proof submitted */
    bool                    quarantined;  /* true = excluded from relay selection */
    bool                    greenhouse_capable; /* peer advertised "greenhouse" feature */
    bool                    exit_capable;       /* peer advertised "exit" feature */

    /* Jurisdiction code: ISO 3166-1 alpha-2 country code, zero-terminated.
     * Empty string "" means unknown/unspecified. */
    char                    jurisdiction[4];
} trellis_peer_info_t;

typedef void (*trellis_dht_store_cb)(trellis_err_t err, void *ctx);
typedef void (*trellis_dht_find_cb)(const uint8_t *value, size_t len,
                                    trellis_err_t err, void *ctx);
typedef void (*trellis_dht_find_node_cb)(const trellis_peer_info_t *peers,
                                         size_t count,
                                         trellis_err_t err, void *ctx);

typedef void (*trellis_dht_send_fn)(void *client,
                                     const trellis_fingerprint_t *target,
                                     const uint8_t *data, size_t len);

trellis_dht_t *trellis_dht_new(const trellis_identity_t *local_id,
                                uv_loop_t *loop);
void           trellis_dht_free(trellis_dht_t *dht);

// Access the DHT's internal Sybil manager (may be NULL).
trellis_sybil_t *trellis_dht_sybil(trellis_dht_t *dht);

void trellis_dht_set_send(trellis_dht_t *dht,
                           trellis_dht_send_fn fn, void *client);
trellis_err_t trellis_dht_on_message(trellis_dht_t *dht,
                                      const trellis_fingerprint_t *sender,
                                      const uint8_t *data, size_t len);

trellis_err_t trellis_dht_bootstrap(trellis_dht_t *dht,
                                     const char *seed_addr);
trellis_err_t trellis_dht_add_peer(trellis_dht_t *dht,
                                    const trellis_peer_info_t *peer);
trellis_err_t trellis_dht_store(trellis_dht_t *dht,
                                 const uint8_t *key, size_t key_len,
                                 const uint8_t *value, size_t value_len,
                                 trellis_dht_store_cb cb, void *ctx);
trellis_err_t trellis_dht_find_value(trellis_dht_t *dht,
                                      const uint8_t *key, size_t key_len,
                                      trellis_dht_find_cb cb, void *ctx);
trellis_err_t trellis_dht_find_node(trellis_dht_t *dht,
                                     const trellis_fingerprint_t *target,
                                     trellis_dht_find_node_cb cb, void *ctx);

/*
 * S/Kademlia versions of find_value and find_node.  d=3 independent lookup
 * paths are started in parallel, each seeded with a disjoint subset of the
 * closest routing-table peers.  No peer is queried by more than one path
 * (global disjointness).  A result is reported to cb only when at least
 * ceil(d/2)+1 = 2 of the 3 paths agree on the same value.  This makes it
 * infeasible for an attacker controlling a fraction of the network to
 * return forged results unless they can poison ALL d disjoint paths.
 */
trellis_err_t trellis_dht_find_value_skad(trellis_dht_t *dht,
                                           const uint8_t *key, size_t key_len,
                                           trellis_dht_find_cb cb, void *ctx);
trellis_err_t trellis_dht_find_node_skad(trellis_dht_t *dht,
                                          const trellis_fingerprint_t *target,
                                          trellis_dht_find_node_cb cb, void *ctx);

// --- Redundant descriptor storage ---

/*
 * Store a descriptor at k=REDUNDANT_REPLICAS deterministic replica keys
 * derived from the primary key.  Provides availability even if the
 * primary DHT neighbourhood is eclipsed by an attacker.
 *
 * The replica keys are: key_i = SHA256(primary_key || "replica" || i)
 * for i = 0..REDUNDANT_REPLICAS-1.  Both publisher and clients must use
 * the same derivation to find each other.
 *
 * Per-identity STORE rate limiting: each Bloom fingerprint may publish
 * at most STORE_RATE_LIMIT_PER_WINDOW stores per STORE_RATE_WINDOW_MS.
 * Excess stores are silently dropped.  The identity parameter is optional
 * (pass NULL to skip rate limiting).
 */
#define TRELLIS_DHT_REDUNDANT_REPLICAS  3

trellis_err_t trellis_dht_store_redundant(
        trellis_dht_t *dht,
        const uint8_t *key, size_t key_len,
        const uint8_t *value, size_t value_len,
        const trellis_fingerprint_t *identity,  /* NULL = no rate-limit */
        trellis_dht_store_cb cb, void *ctx);

trellis_err_t trellis_dht_find_value_redundant(
        trellis_dht_t *dht,
        const uint8_t *key, size_t key_len,
        trellis_dht_find_cb cb, void *ctx);

// --- Private Information Retrieval for DHT lookups ---

/*
 * Cover-query PIR: fire PIR_COVER_COUNT additional FIND_VALUE requests for
 * pseudorandom cover keys alongside the real key.  Any single DHT node that
 * answers one query cannot determine which key was "real" with probability
 * better than 1/(PIR_COVER_COUNT+1).  Use for greenhouse descriptor lookups.
 *
 * trellis_pir_find_value_skad additionally uses S/Kademlia disjoint paths
 * for each sub-query, combining Sybil resistance with PIR privacy.
 */
trellis_err_t trellis_pir_find_value(trellis_dht_t *dht,
                                      const uint8_t *key, size_t key_len,
                                      trellis_dht_find_cb cb, void *ctx);
trellis_err_t trellis_pir_find_value_skad(trellis_dht_t *dht,
                                           const uint8_t *key, size_t key_len,
                                           trellis_dht_find_cb cb, void *ctx);

size_t        trellis_dht_peer_count(const trellis_dht_t *dht);

size_t trellis_dht_get_peers(const trellis_dht_t *dht,
                              trellis_peer_info_t *out, size_t max_out);
void   trellis_dht_remove_peer(trellis_dht_t *dht,
                                const trellis_fingerprint_t *fp);
const trellis_peer_info_t *trellis_dht_lookup_peer(const trellis_dht_t *dht,
                                                    const trellis_fingerprint_t *fp);
const char *trellis_dht_lookup_addr(const trellis_dht_t *dht,
                                     const trellis_fingerprint_t *fp);
void trellis_dht_update_peer_seen(trellis_dht_t *dht,
                                   const trellis_fingerprint_t *fp);
size_t trellis_dht_find_closest(const trellis_dht_t *dht,
                                 const trellis_fingerprint_t *target,
                                 trellis_peer_info_t *out, size_t max_out);

typedef struct trellis_nat trellis_nat_t;

trellis_nat_t *trellis_nat_new(uv_loop_t *loop);
void           trellis_nat_free(trellis_nat_t *nat);
trellis_err_t  trellis_nat_probe(trellis_nat_t *nat,
                                  const char *stun_server, uint16_t port);
const char    *trellis_nat_mapped_address(const trellis_nat_t *nat);
uint16_t       trellis_nat_mapped_port(const trellis_nat_t *nat);

// --- Routing (Section 6.2) ---
typedef enum trellis_routing_mode {
    TRELLIS_ROUTE_DIRECT    = 0,
    TRELLIS_ROUTE_RELAY     = 1,
    TRELLIS_ROUTE_MULTIPATH = 2,
    TRELLIS_ROUTE_ONION     = 3,
    TRELLIS_ROUTE_AUTO      = 4,
    TRELLIS_ROUTE_ADAPTIVE  = 5,
} trellis_routing_mode_t;

typedef enum trellis_privacy_level {
    TRELLIS_PRIVACY_SPEED    = 0,  /* 1-hop media, 3-hop signaling */
    TRELLIS_PRIVACY_BALANCED = 1,  /* 1-hop media when latency > threshold, else 3 */
    TRELLIS_PRIVACY_MAX      = 2,  /* always 3-hop */
} trellis_privacy_level_t;

typedef struct trellis_router trellis_router_t;
typedef struct trellis_guard  trellis_guard_t;

trellis_router_t *trellis_router_new(trellis_dht_t *dht, void *client);
void              trellis_router_free(trellis_router_t *router);

trellis_routing_mode_t trellis_router_mode(const trellis_router_t *router);
trellis_err_t trellis_router_set_mode(trellis_router_t *router,
                                       trellis_routing_mode_t mode);

// Attach a guard set so that onion circuits prefer pinned entry guards.
void trellis_router_set_guard(trellis_router_t *router, trellis_guard_t *guard);

/*
 * Set a list of jurisdiction codes to exclude from hop selection.
 * Hops whose trellis_peer_info_t.jurisdiction matches any of the provided
 * codes will be skipped during onion and relay routing.
 *
 * codes: NULL-terminated array of ISO 3166-1 alpha-2 strings,
 *        e.g. { "US", "GB", "DE", NULL }.
 * Pass NULL to clear all exclusions.
 *
 * Returns TRELLIS_ERR_NOMEM if the internal copy fails.
 */
trellis_err_t trellis_route_exclude_jurisdictions(trellis_router_t *router,
                                                   const char **codes);

trellis_err_t trellis_router_send(trellis_router_t *router,
                                   const trellis_fingerprint_t *dest,
                                   const uint8_t *data, size_t len,
                                   trellis_conn_send_cb cb, void *ctx);

/*
 * Extended onion send: forces onion routing and allows callers to pin a
 * stream_id across multiple sends so that the exit relay can correlate
 * them as a single bidirectional stream.
 *
 *   force_stream_id — if >0, reuse this stream_id instead of generating
 *                     a new one.  Pass 0 on the first call.
 *   stream_id_out   — receives the stream_id that was actually embedded
 *                     in the onion layers.  May be NULL if not needed.
 */
trellis_err_t trellis_router_send_ex(trellis_router_t *router,
                                      const trellis_fingerprint_t *dest,
                                      const uint8_t *data, size_t len,
                                      uint32_t force_stream_id,
                                      uint32_t *stream_id_out,
                                      trellis_conn_send_cb cb, void *ctx);

/* Send with explicit message flags (needed for ADAPTIVE mode to distinguish
 * media traffic from signaling based on TRELLIS_FLAG_MORPH_BYPASS). */
trellis_err_t trellis_router_send_with_flags(trellis_router_t *router,
                                              const trellis_fingerprint_t *dest,
                                              const uint8_t *data, size_t len,
                                              uint8_t flags,
                                              trellis_conn_send_cb cb, void *ctx);

// Set privacy preference for adaptive routing.
void trellis_router_set_privacy_level(trellis_router_t *router,
                                       trellis_privacy_level_t level);

// --- Gossip / "Pollination" (Section 6.4) ---
#define TRELLIS_GOSSIP_FAN_OUT    6
#define TRELLIS_GOSSIP_DEFAULT_TTL 30000

typedef struct trellis_gossip trellis_gossip_t;

typedef void (*trellis_gossip_cb)(const uint8_t *data, size_t len,
                                  const trellis_fingerprint_t *origin,
                                  void *ctx);

typedef void (*trellis_gossip_send_fn)(void *client,
                                       const trellis_fingerprint_t *target,
                                       const uint8_t *data, size_t len);

trellis_gossip_t *trellis_gossip_new(trellis_dht_t *dht,
                                      const trellis_identity_t *id);
void              trellis_gossip_free(trellis_gossip_t *gossip);
void              trellis_gossip_set_send(trellis_gossip_t *gossip,
                                          trellis_gossip_send_fn fn, void *client);

trellis_err_t trellis_gossip_publish(trellis_gossip_t *gossip,
                                      const uint8_t *data, size_t len);
trellis_err_t trellis_gossip_subscribe(trellis_gossip_t *gossip,
                                        trellis_gossip_cb cb, void *ctx);
trellis_err_t trellis_gossip_receive(trellis_gossip_t *gossip,
                                      const uint8_t *data, size_t len,
                                      const trellis_fingerprint_t *origin);

// --- Cross-grove gossip relay (Rhizome) ---

// Grove identity type — full definition needed for struct embedding.
#ifndef TRELLIS_GROVE_ID_DEFINED
#define TRELLIS_GROVE_ID_DEFINED
#ifndef TRELLIS_GROVE_ID_LEN
#define TRELLIS_GROVE_ID_LEN 32
#endif
typedef struct trellis_grove_id {
    uint8_t bytes[TRELLIS_GROVE_ID_LEN];
} trellis_grove_id_t;
#endif

/* Set/get the grove scope for this gossip instance.  When set, gossip
 * messages include grove context for cross-grove relay decisions.
 * Pass NULL to clear (single-mesh mode). */
void trellis_gossip_set_grove_scope(trellis_gossip_t *gossip,
                                    const trellis_grove_id_t *grove_id);
const trellis_grove_id_t *trellis_gossip_grove_scope(
                                    const trellis_gossip_t *gossip);

/* Relay a gossip payload from one grove to another.  The bridge re-signs
 * the message under its identity in the target grove.  Rate limiting is
 * per-grove-per-publisher to prevent cross-grove spam. */
trellis_err_t trellis_gossip_cross_grove_relay(
        trellis_gossip_t *target_gossip,
        const uint8_t *original_payload, size_t payload_len,
        const trellis_fingerprint_t *original_publisher,
        const trellis_grove_id_t *source_grove,
        const trellis_identity_t *bridge_identity);

// --- Guard Node Pinning (Tor Proposal 292 variant) ---
trellis_guard_t *trellis_guard_new(trellis_dht_t *dht,
                                    trellis_sybil_t *sybil,
                                    const char *persist_path);
void             trellis_guard_free(trellis_guard_t *g);

// Refresh guard/vanguard sets — call after significant DHT growth or on timer.
void trellis_guard_select(trellis_guard_t *g, uint64_t now_ms);

// Periodic maintenance: expire old guards and rotate vanguard slots as needed.
void trellis_guard_tick(trellis_guard_t *g, uint64_t now_ms);

// Get the fingerprint of the idx-th (0–2) guard. Returns NULL if slot empty.
const trellis_fingerprint_t *trellis_guard_get(const trellis_guard_t *g, int idx);

// Vanguard L2 (0–1) and L3 (0–1) slot accessors.
const trellis_fingerprint_t *trellis_guard_get_l2(const trellis_guard_t *g, int idx);
const trellis_fingerprint_t *trellis_guard_get_l3(const trellis_guard_t *g, int idx);

/* Report a connection failure for fp — increments strike counter; replaces on
 * threshold. Also forces vanguard rotation for L2/L3 positions. */
void trellis_guard_on_failure(trellis_guard_t *g,
                               const trellis_fingerprint_t *fp);

// Record a successful use of fp as a guard (resets failure count).
void trellis_guard_mark_used(trellis_guard_t *g,
                              const trellis_fingerprint_t *fp);

/* Track circuit attempts/successes/collapses per guard.
 * trellis_guard_path_bias_check() evaluates the ratios and disables
 * guards whose success rate drops below a safety threshold. */
void trellis_guard_path_bias_attempt(trellis_guard_t *g,
                                      const trellis_fingerprint_t *guard_fp);
void trellis_guard_path_bias_success(trellis_guard_t *g,
                                      const trellis_fingerprint_t *guard_fp);
void trellis_guard_path_bias_collapse(trellis_guard_t *g,
                                       const trellis_fingerprint_t *guard_fp);
trellis_err_t trellis_guard_path_bias_check(trellis_guard_t *g);

typedef struct trellis_topology trellis_topology_t;

trellis_topology_t *trellis_topology_new(trellis_dht_t *dht, uv_loop_t *loop);
void                trellis_topology_free(trellis_topology_t *topo);

trellis_err_t trellis_topology_start(trellis_topology_t *topo);
void          trellis_topology_stop(trellis_topology_t *topo);
size_t        trellis_topology_neighbor_count(const trellis_topology_t *topo);
size_t        trellis_topology_target_neighbors(const trellis_topology_t *topo);

// --- Clearnet Exit Relay ---
typedef struct trellis_exit_relay trellis_exit_relay_t;

trellis_exit_relay_t *trellis_exit_relay_new(uv_loop_t *loop);
void           trellis_exit_relay_free(trellis_exit_relay_t *er);

// Add an exit policy rule. Format: "allow:*.example.com:443".
trellis_err_t  trellis_exit_relay_add_policy(trellis_exit_relay_t *er,
                                               const char *rule_str);

// Start a SOCKS5 proxy on localhost:port for application-level integration.
trellis_err_t  trellis_exit_relay_start_socks5(trellis_exit_relay_t *er,
                                                 int port);

// Check if a destination is allowed by this relay's exit policy.
bool           trellis_exit_relay_policy_allows(const trellis_exit_relay_t *er,
                                                 const char *host, uint16_t port);

/* Default hashcash difficulty for exit relay PoW (leading zero bits in SHA-256).
 * Shared between client token generation and relay verification. */
#define TRELLIS_EXIT_POW_DIFFICULTY  20

// Verify a proof-of-work anti-abuse token.
bool           trellis_exit_relay_verify_pow(const trellis_exit_relay_t *er,
                                              const uint8_t *token,
                                              size_t token_len);

/*
 * Exit stream command bytes — first byte of every exit-stream onion payload.
 *
 *   OPEN  [cmd(1)][host_len(1)][host][port_be(2)][pow_token...]
 *   DATA  [cmd(1)][raw data...]
 *   CLOSE [cmd(1)]
 */
#define TRELLIS_EXIT_CMD_OPEN   0x01
#define TRELLIS_EXIT_CMD_DATA   0x02
#define TRELLIS_EXIT_CMD_CLOSE  0x03

/* Handle an inbound exit stream payload arriving via onion routing.
 * The first byte is a command (TRELLIS_EXIT_CMD_*).
 * stream_id comes from the onion layer; used for DATA/CLOSE lookup. */
trellis_err_t  trellis_exit_relay_handle_stream(trellis_exit_relay_t *er,
                                                 const uint8_t *payload,
                                                 size_t payload_len,
                                                 uint32_t stream_id,
                                                 void *upstream);

// --- Relay Incentive System ---
typedef struct trellis_incentive trellis_incentive_t;

/*
 * Called when a new bandwidth receipt is accepted.
 * @receipt / @receipt_len: raw receipt bytes
 * @bytes_fwd: bytes this receipt covers
 */
typedef void (*trellis_incentive_receipt_cb)(
        const uint8_t *receipt, size_t receipt_len,
        uint64_t bytes_fwd, void *ctx);

// Send callback: delivers serialized receipt data to a peer over the wire.
typedef void (*trellis_incentive_send_fn)(void *ctx,
                                          const trellis_fingerprint_t *peer_fp,
                                          const uint8_t *data, size_t len);

trellis_incentive_t *trellis_incentive_new(const trellis_identity_t *local_id,
                                            uv_loop_t *loop);
void            trellis_incentive_free(trellis_incentive_t *inc);
void            trellis_incentive_record_relay(trellis_incentive_t *inc,
                                                const trellis_fingerprint_t *peer_fp,
                                                uint64_t bytes);
void            trellis_incentive_record_consume(trellis_incentive_t *inc,
                                                  const trellis_fingerprint_t *relay_fp,
                                                  uint64_t bytes);
trellis_err_t   trellis_incentive_accept_receipt(trellis_incentive_t *inc,
                                                   const uint8_t *receipt,
                                                   size_t receipt_len,
                                                   const uint8_t *payer_ed25519_pk);
trellis_err_t   trellis_incentive_verify_receipt(const uint8_t *receipt,
                                                   size_t receipt_len,
                                                   const uint8_t *payer_ed25519_pk,
                                                   uint64_t *bytes_fwd_out,
                                                   trellis_fingerprint_t *relay_fp_out);
void            trellis_incentive_set_lightning(trellis_incentive_t *inc,
                                                 const char *btcpay_url,
                                                 const char *btcpay_api_key,
                                                 const char *lncli_path,
                                                 uint64_t sats_per_mb);
trellis_err_t   trellis_incentive_redeem(trellis_incentive_t *inc);
uint64_t        trellis_incentive_total_relayed(const trellis_incentive_t *inc);
void            trellis_incentive_set_receipt_cb(trellis_incentive_t *inc,
                                                  trellis_incentive_receipt_cb cb,
                                                  void *ctx);
void            trellis_incentive_set_send(trellis_incentive_t *inc,
                                            trellis_incentive_send_fn fn,
                                            void *ctx);

typedef struct trellis_bootstrap trellis_bootstrap_t;

/*
 * Called when bootstrap completes or fails.
 * @err          TRELLIS_OK if ≥ BOOTSTRAP_PEER_MIN peers were found
 * @peer_count   number of peers successfully contacted
 */
typedef void (*trellis_bootstrap_cb)(trellis_err_t err, int peer_count,
                                      void *ctx);

trellis_bootstrap_t *trellis_bootstrap_start(trellis_dht_t *dht,
                                               uv_loop_t *loop,
                                               trellis_bootstrap_cb cb,
                                               void *ctx);
void          trellis_bootstrap_free(trellis_bootstrap_t *bs);
trellis_err_t trellis_bootstrap_add_seed(trellis_bootstrap_t *bs,
                                          const char *addr,
                                          const trellis_fingerprint_t *fp);

// --- Warrant Canary Infrastructure ---
typedef struct trellis_canary_timer trellis_canary_timer_t;
typedef void (*trellis_canary_cb)(const trellis_fingerprint_t *fp,
                                   bool is_fresh, void *ctx);

// Derive the DHT key for a node's canary: SHA256("trellis_canary" || fp)
void trellis_canary_dht_key(const trellis_fingerprint_t *fp,
                              uint8_t *key_out /* 32 bytes */);

// Serialize + sign a canary record. Returns bytes written, 0 on error.
size_t trellis_canary_serialize(const trellis_identity_t *id,
                                 const char *statement,
                                 uint8_t *buf, size_t buf_cap);

/* Deserialize + verify a canary from the DHT.
 * expected_fp is compared against the embedded node_fp field to prevent
 * a node from signing canaries on behalf of another identity.
 * Returns OK if valid and fresh; EXPIRED or VERIFY_FAILED otherwise. */
trellis_err_t trellis_canary_verify(const uint8_t *data, size_t data_len,
                                     const uint8_t *issuer_pubkey,
                                     const trellis_fingerprint_t *expected_fp,
                                     uint64_t *issued_at_ms_out);

// Publish this node's canary to the DHT immediately.
trellis_err_t trellis_canary_publish(trellis_dht_t *dht,
                                      const trellis_identity_t *id,
                                      const char *statement);

// Asynchronously fetch + verify a remote peer's canary.
trellis_err_t trellis_canary_fetch(trellis_dht_t *dht,
                                    const trellis_fingerprint_t *fp,
                                    const uint8_t *node_pubkey,
                                    trellis_canary_cb cb, void *ctx);

// Synchronous routing check: TRELLIS_OK = canary fresh, else stale/absent.
trellis_err_t trellis_canary_check_peer(const trellis_fingerprint_t *fp);

// Invalidate a peer's cached canary (revocation via gossip).
void trellis_canary_revoke(const trellis_fingerprint_t *fp);

// Start/stop the periodic auto-publish timer.
trellis_canary_timer_t *trellis_canary_timer_start(uv_loop_t *loop,
                                                    trellis_dht_t *dht,
                                                    const trellis_identity_t *id,
                                                    const char *statement);
void trellis_canary_timer_stop(trellis_canary_timer_t *timer);

// --- Bloom Naming Ecosystem ---
typedef struct trellis_naming trellis_naming_t;

typedef void (*trellis_naming_resolve_cb)(const char *alias,
                                           const trellis_fingerprint_t *fp,
                                           trellis_err_t err, void *ctx);

trellis_naming_t *trellis_naming_new(trellis_dht_t *dht,
                                      trellis_identity_t *self_id,
                                      uv_loop_t *loop);
void trellis_naming_free(trellis_naming_t *n);

// Register an alias for our own identity and publish to DHT.
trellis_err_t trellis_naming_register(trellis_naming_t *n,
                                       const char *raw_alias);

// Resolve alias to fingerprint (async, checks cache then DHT)
trellis_err_t trellis_naming_resolve(trellis_naming_t *n,
                                      const char *raw_alias,
                                      trellis_naming_resolve_cb cb,
                                      void *ctx);

// Verify and cache a raw naming record from the DHT.
trellis_err_t trellis_naming_verify_record(trellis_naming_t *n,
                                            const uint8_t *record,
                                            size_t record_len,
                                            const uint8_t *ed25519_pk);

// Local petname system (never broadcast)
trellis_err_t trellis_naming_set_petname(trellis_naming_t *n,
                                          const trellis_fingerprint_t *fp,
                                          const char *label);
const char   *trellis_naming_get_petname(const trellis_naming_t *n,
                                          const trellis_fingerprint_t *fp);
trellis_err_t trellis_naming_remove_petname(trellis_naming_t *n,
                                             const trellis_fingerprint_t *fp);

// DNS TXT bridge: import/export bloom-fp= records.
trellis_err_t trellis_naming_import_dns_txt(trellis_naming_t *n,
                                             const char *alias,
                                             const char *txt_record);
trellis_err_t trellis_naming_export_dns_txt(const trellis_naming_t *n,
                                             const char *alias,
                                             char *out, size_t out_cap);

// --- Dead-Drop Mailboxes ---

/*
 * Epoch-rotating DHT dead drops for asynchronous, timing-decorrelated
 * message delivery.  The dead-drop key is derived as:
 *
 *   key = HKDF-SHAKE-256(shared_secret, "trellis-deaddrop" || epoch_index || seq)
 *
 * The sender stores ratchet-encrypted messages at the key via
 * trellis_dht_store_redundant.  The receiver polls via
 * trellis_pir_find_value_skad (PIR + S/Kademlia) so neither the store
 * nodes nor queried nodes learn the real key or contents.
 *
 * epoch_ms controls key rotation period (e.g. 3600000 = 1 hour).
 */
#define TRELLIS_DEADDROP_KEY_LEN  32
#define TRELLIS_DEADDROP_DEFAULT_EPOCH_MS  3600000u  /* 1 hour */

typedef struct trellis_deaddrop trellis_deaddrop_t;

typedef void (*trellis_deaddrop_poll_cb)(const uint8_t *plaintext, size_t len,
                                         trellis_err_t err, void *ctx);

trellis_deaddrop_t *trellis_deaddrop_new(trellis_ratchet_t *ratchet,
                                          const uint8_t *shared_secret,
                                          size_t secret_len,
                                          uint64_t epoch_ms);
void                trellis_deaddrop_free(trellis_deaddrop_t *dd);

/* Store a message at the current epoch's dead-drop key.
 *
 * Multiple messages per epoch are supported — each send within the same
 * epoch uses a monotonically increasing sequence number embedded in the
 * key derivation.  The sequence resets to 0 on each epoch boundary.
 *
 * The ratchet used by the dead-drop is caller-owned; trellis_deaddrop_free
 * does NOT free it. */
trellis_err_t trellis_deaddrop_send(trellis_deaddrop_t *dd,
                                     trellis_dht_t *dht,
                                     const uint8_t *plaintext, size_t len);

/* Poll the current AND previous epoch's dead-drop keys for all available
 * messages (seq=0, 1, 2, ...).  The callback fires once per successfully
 * decrypted message, then once with TRELLIS_ERR_NOT_FOUND when no more
 * messages remain.  Checking the previous epoch closes the boundary race. */
trellis_err_t trellis_deaddrop_poll(trellis_deaddrop_t *dd,
                                     trellis_dht_t *dht,
                                     trellis_deaddrop_poll_cb cb, void *ctx);

// --- DHT Eclipse Detection ---
typedef void (*trellis_dht_eclipse_cb)(trellis_dht_t *dht, void *ctx);

void trellis_dht_set_eclipse_cb(trellis_dht_t *dht,
                                 trellis_dht_eclipse_cb cb, void *ctx);
bool trellis_dht_eclipse_detected(const trellis_dht_t *dht);

/* Enable or disable the eclipse detection subsystem.  Disabling clears
 * any current freeze.  Useful for test environments where all peers
 * share a subnet (e.g. 127.0.0.1). */
void trellis_dht_set_eclipse_enabled(trellis_dht_t *dht, bool enabled);

/* Check the sibling table for eclipse indicators and fire callback.
 * Called automatically from trellis_dht_add_peer; can also be called
 * from a periodic timer for continuous monitoring. */
void trellis_dht_check_eclipse(trellis_dht_t *dht);

/* Disable the Sybil admission gate so peers are not quarantined.
 * Useful for test environments where nodes lack VDF proofs. */
void trellis_dht_disable_sybil(trellis_dht_t *dht);

/* Refresh stale k-buckets (not accessed in 60 min) using S/Kademlia
 * lookups seeded from high-trust peers.  Call periodically (e.g. every
 * 10 minutes) to maintain routing table health. */
trellis_err_t trellis_dht_refresh_buckets(trellis_dht_t *dht);

#ifdef __cplusplus
}
#endif

#endif /* TRELLIS_MESH_H */
