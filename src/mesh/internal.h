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

#ifndef TRELLIS_MESH_INTERNAL_H
#define TRELLIS_MESH_INTERNAL_H

#include <trellis/mesh.h>
#include <trellis/crypto.h>
#include <trellis/sybil.h>
#include <trellis/rhizome.h>
#include <uv.h>
#include <sodium.h>

#include <trellis/protocol.h>

#include <stdlib.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

// --- Wire-format helpers (network byte order) ---
static inline void trellis_write_u32_le(uint8_t *buf, uint32_t v)
{
    buf[0] = (uint8_t)(v);
    buf[1] = (uint8_t)(v >> 8);
    buf[2] = (uint8_t)(v >> 16);
    buf[3] = (uint8_t)(v >> 24);
}

static inline uint32_t trellis_read_u32_le(const uint8_t *buf)
{
    return (uint32_t)buf[0]
         | ((uint32_t)buf[1] << 8)
         | ((uint32_t)buf[2] << 16)
         | ((uint32_t)buf[3] << 24);
}

static inline void trellis_write_u16_be(uint8_t *buf, uint16_t v)
{
    buf[0] = (uint8_t)(v >> 8);
    buf[1] = (uint8_t)(v);
}

static inline uint16_t trellis_read_u16_be(const uint8_t *buf)
{
    return (uint16_t)((uint16_t)buf[0] << 8 | (uint16_t)buf[1]);
}

static inline void trellis_write_u32_be(uint8_t *buf, uint32_t v)
{
    buf[0] = (uint8_t)(v >> 24);
    buf[1] = (uint8_t)(v >> 16);
    buf[2] = (uint8_t)(v >> 8);
    buf[3] = (uint8_t)(v);
}

static inline uint32_t trellis_read_u32_be(const uint8_t *buf)
{
    return (uint32_t)buf[0] << 24
         | (uint32_t)buf[1] << 16
         | (uint32_t)buf[2] << 8
         | (uint32_t)buf[3];
}

static inline void trellis_write_u64_be(uint8_t *buf, uint64_t v)
{
    buf[0] = (uint8_t)(v >> 56);
    buf[1] = (uint8_t)(v >> 48);
    buf[2] = (uint8_t)(v >> 40);
    buf[3] = (uint8_t)(v >> 32);
    buf[4] = (uint8_t)(v >> 24);
    buf[5] = (uint8_t)(v >> 16);
    buf[6] = (uint8_t)(v >> 8);
    buf[7] = (uint8_t)(v);
}

static inline uint64_t trellis_read_u64_be(const uint8_t *buf)
{
    return (uint64_t)buf[0] << 56
         | (uint64_t)buf[1] << 48
         | (uint64_t)buf[2] << 40
         | (uint64_t)buf[3] << 32
         | (uint64_t)buf[4] << 24
         | (uint64_t)buf[5] << 16
         | (uint64_t)buf[6] << 8
         | (uint64_t)buf[7];
}

// --- Simple open-addressing hash map ---
#define DHT_MAP_INIT_CAP     64
#define DHT_MAP_LOAD_MAX     0.75
#define DHT_MAP_MAX_ENTRIES  10000

typedef struct dht_map_entry {
    uint8_t *key;
    size_t   key_len;
    uint8_t *value;
    size_t   value_len;
    bool     occupied;
} dht_map_entry_t;

typedef struct dht_map {
    dht_map_entry_t *entries;
    size_t           cap;
    size_t           count;
} dht_map_t;

int    dht_map_init(dht_map_t *m, size_t cap);
void   dht_map_destroy(dht_map_t *m);
int    dht_map_put(dht_map_t *m, const uint8_t *key, size_t key_len,
                   const uint8_t *value, size_t value_len);
int    dht_map_get(const dht_map_t *m, const uint8_t *key, size_t key_len,
                   const uint8_t **value_out, size_t *value_len_out);

// --- Simple hash set for dedup (stores 32-byte digests) ---
#define DEDUP_SET_INIT_CAP 256
#define DEDUP_SET_MAX_CAP  16384  /* hard cap to prevent unbounded memory growth */

typedef struct dedup_entry {
    uint8_t  digest[32];
    bool     occupied;
    uint64_t inserted_at;  /* for TTL-based expiry */
} dedup_entry_t;

typedef struct dedup_set {
    dedup_entry_t *entries;
    size_t         cap;
    size_t         count;
} dedup_set_t;

int  dedup_set_init(dedup_set_t *s, size_t cap);
void dedup_set_destroy(dedup_set_t *s);
bool dedup_set_contains(const dedup_set_t *s, const uint8_t digest[32]);
int  dedup_set_insert(dedup_set_t *s, const uint8_t digest[32]);

/* Max peers from the same /16 subnet allowed in a single k-bucket.
 * Prevents an attacker with many IPs in one /16 from dominating buckets. */
#define KBUCKET_MAX_PER_SUBNET  3

typedef struct k_bucket {
    trellis_peer_info_t *peers[TRELLIS_DHT_K];
    size_t               count;
} k_bucket_t;

typedef struct routing_table {
    k_bucket_t buckets[TRELLIS_DHT_BUCKETS];
} routing_table_t;

#define DHT_MAX_PENDING_RPCS     64
#define DHT_RPC_TIMEOUT_MS       5000
#define DHT_ALPHA                3      /* Kademlia concurrency parameter */
#define DHT_STORE_QUORUM         3

// --- S/Kademlia disjoint parallel lookup (Baumgart & Mies 2007) ---
#define SKAD_D               3        /* number of disjoint lookup paths */
#define SKAD_MAX_GROUPS      16       /* max concurrent S/Kademlia lookups */
// Minimum agreement threshold: ceil(SKAD_D/2)+1 = 2 out of 3.
#define SKAD_AGREE_THRESHOLD 2

/*
 * Aggregates d=3 independent lookup paths.  A result is accepted only when
 * at least SKAD_AGREE_THRESHOLD paths agree on the same value / fingerprint.
 */
typedef struct skad_group {
    bool                  active;
    uint32_t              group_id;

    // Per-path request IDs (0 = path not started)
    uint32_t              path_rpc_ids[SKAD_D];

    // Per-path results (NULL = no result yet)
    uint8_t              *path_results[SKAD_D];
    size_t                path_result_lens[SKAD_D];
    bool                  path_done[SKAD_D];

    // Aggregate state.
    uint8_t              *agreed_value;
    size_t                agreed_len;
    int                   completed_paths;
    int                   successful_paths;

    // Global queried set (shared across all d paths for disjointness)
    trellis_fingerprint_t global_queried[TRELLIS_DHT_K * SKAD_D * 3];
    size_t                global_queried_count;

    // Callbacks
    trellis_dht_find_cb       find_cb;
    void                     *find_ctx;
    trellis_dht_find_node_cb  find_node_cb;
    void                     *find_node_ctx;
    bool                      is_find_node; /* true = FIND_NODE, false = FIND_VALUE */

    struct trellis_dht       *dht;
} skad_group_t;

typedef enum dht_rpc_kind {
    DHT_RPC_STORE_OP      = 0,
    DHT_RPC_FIND_VALUE_OP = 1,
    DHT_RPC_FIND_NODE_OP  = 2,
} dht_rpc_kind_t;

typedef struct dht_pending_rpc {
    uint32_t              request_id;
    dht_rpc_kind_t        kind;
    bool                  active;
    uv_timer_t            timer;
    uint64_t              sent_at_ms;  /* RTT measurement for PING/PONG */

    // Key being queried (for find operations and store)
    uint8_t              *key;
    size_t                key_len;

    // Store-specific
    uint8_t              *value;
    size_t                value_len;
    size_t                acks_received;
    size_t                acks_needed;
    trellis_dht_store_cb  store_cb;
    void                 *store_ctx;

    // Find-value-specific
    trellis_dht_find_cb   find_cb;
    void                 *find_ctx;
    bool                  found;

    // Find-node-specific
    trellis_dht_find_node_cb  find_node_cb;
    void                     *find_node_ctx;

    // Iterative lookup state.
    trellis_fingerprint_t     queried[TRELLIS_DHT_K * 3];
    size_t                    queried_count;
    size_t                    pending_responses;
    size_t                    round;

    // S/Kademlia: non-zero when this RPC is part of a disjoint group.
    uint32_t                  skad_group_id;
    int                       skad_path_idx; /* 0..SKAD_D-1 */

    struct trellis_dht       *dht;
} dht_pending_rpc_t;

#define CLOCK_SYNC_SAMPLES 16

typedef struct trellis_clock_sync {
    int64_t  offsets[CLOCK_SYNC_SAMPLES];
    size_t   count;
    size_t   next_idx;
} trellis_clock_sync_t;

// --- DHT ---
struct trellis_dht {
    const trellis_identity_t *local_id;
    routing_table_t           rt;
    dht_map_t                 store;
    uv_loop_t                *loop;
    size_t                    peer_count;
    trellis_sybil_t          *sybil;

    // Send function for RPC messaging.
    trellis_dht_send_fn       send_fn;
    void                     *send_client;

    // Pending RPC tracking (request IDs are random, not sequential)
    dht_pending_rpc_t         rpcs[DHT_MAX_PENDING_RPCS];

    // S/Kademlia disjoint lookup groups.
    skad_group_t              skad_groups[SKAD_MAX_GROUPS];
    uint32_t                  next_skad_group_id;

    // Per-identity STORE rate limiting.
    struct {
        trellis_fingerprint_t fp;
        uint64_t              window_start_ms;
        uint32_t              count;
    } store_rate[64];
    size_t                    store_rate_count;

    /* Sibling table: the TRELLIS_DHT_K peers numerically closest to our own
     * node ID.  Maintained in trellis_dht_add_peer() to detect and resist
     * neighbourhood-isolation (eclipse) attacks. */
    trellis_peer_info_t      *sibling_table[TRELLIS_DHT_K];
    size_t                    sibling_count;

    // Eclipse detection state.
    bool                      eclipse_detected;
    bool                      eclipse_frozen;    /* routing table updates frozen */
    bool                      eclipse_disabled;  /* skip eclipse checks entirely */
    uint64_t                  eclipse_freeze_until_ms;
    trellis_dht_eclipse_cb    eclipse_cb;
    void                     *eclipse_cb_ctx;
    uv_timer_t                eclipse_timer;     /* periodic eclipse check */
    bool                      eclipse_timer_init; /* whether eclipse_timer was uv_timer_init'd */

    // Per-bucket last-access timestamps for stale-bucket refresh.
    uint64_t                  bucket_last_access[TRELLIS_DHT_BUCKETS];

    // Teardown bookkeeping: counts pending RPC timer uv_close callbacks.
    int                       rpc_close_pending;

    // Clock skew estimation from PING/PONG timestamps.
    trellis_clock_sync_t      clock_sync;

    // Rhizome: grove namespace scoping (NULL = single-mesh mode)
    trellis_grove_id_t       *grove_id;
};

// --- Gossip subscriber ---
#define GOSSIP_MAX_SUBSCRIBERS 64
#define GOSSIP_PUB_RATE_SLOTS  64
#define GOSSIP_PUB_RATE_LIMIT  10   /* max messages per publisher per window */
#define GOSSIP_PUB_RATE_WINDOW 60000 /* 60 second window */
#define GOSSIP_MAX_PAYLOAD     4096  /* reject payloads larger than 4 KB */

typedef struct gossip_subscriber {
    trellis_gossip_cb cb;
    void             *ctx;
} gossip_subscriber_t;

typedef struct gossip_pub_rate {
    uint8_t  fp_hash[8]; /* truncated keyed hash of publisher fingerprint */
    uint32_t count;
    uint64_t window_start;
} gossip_pub_rate_t;

struct trellis_gossip {
    trellis_dht_t           *dht;
    const trellis_identity_t *identity;
    uint32_t                 fan_out;
    uint32_t                 ttl_ms;
    dedup_set_t              seen;
    gossip_subscriber_t      subscribers[GOSSIP_MAX_SUBSCRIBERS];
    size_t                   subscriber_count;
    trellis_gossip_send_fn   send_fn;
    void                    *client;

    gossip_pub_rate_t        pub_rates[GOSSIP_PUB_RATE_SLOTS];

    // Rhizome: optional grove scope for cross-grove gossip relay.
    trellis_grove_id_t      *grove_scope;
};

#define ROUTER_MAX_EXCLUDED_JURISDICTIONS 16

// --- Circuit pool (preemptive build & reuse) ---
#define CIRCUIT_POOL_SIZE      4
#define CIRCUIT_POOL_MAX_AGE_MS (5 * 60 * 1000) /* 5 min max age for pre-built */
#define CIRCUIT_MAX_STREAMS    16

typedef struct circuit_stream {
    uint32_t stream_id;    /* unique random stream ID for onion layer */
    bool     active;
    bool     half_closed;  /* received END from remote */
} circuit_stream_t;

typedef struct prebuilt_circuit {
    bool                   valid;
    uint32_t               circuit_id; /* internal pool identifier */
    trellis_fingerprint_t  hops[3];     /* hop1, hop2, hop3 fingerprints */
    trellis_fingerprint_t  dest;        /* destination (may be zero for general) */
    uint64_t               created_at;
    circuit_stream_t       streams[CIRCUIT_MAX_STREAMS];
    size_t                 stream_count;
    uint8_t                hop_depth;   /* 0 = not built, 3 = fully built */

    // Pre-computed KEM encapsulation for each relay hop.
    trellis_kem_encaps_result_t hop_encaps[3];
    uint8_t                hop_x25519_snapshot[3][TRELLIS_X25519_PK_LEN];
    bool                   encaps_valid;
} prebuilt_circuit_t;

// --- Session-keyed circuits (low-overhead media relay) ---
#define SESSION_POOL_SIZE      4
#define SESSION_KEY_LEN        32
#define SESSION_OVERHEAD       (4 + TRELLIS_FINGERPRINT_LEN + TRELLIS_AES_NONCE_LEN + TRELLIS_AES_TAG_LEN) /* 64 bytes: circuit_id(4)+dest_fp(32)+nonce(12)+tag(16) */

typedef struct session_circuit {
    bool                   valid;
    uint32_t               circuit_id;
    trellis_fingerprint_t  relay;             /* single-hop relay peer */
    uint64_t               created_at;
    uint8_t                session_key[SESSION_KEY_LEN]; /* AES-256-GCM key */
    uint64_t               nonce_counter;     /* monotonic nonce for GCM */

    trellis_kem_encaps_result_t encaps;       /* KEM result for circuit setup */
    bool                   encaps_valid;
} session_circuit_t;

struct trellis_router {
    trellis_dht_t          *dht;
    trellis_routing_mode_t  mode;
    void                   *client;
    trellis_guard_t        *guard;  /* optional guard set; NULL = unguarded */

    // Jurisdiction exclusion list (ISO 3166-1 alpha-2 codes)
    char                    excluded_jurs[ROUTER_MAX_EXCLUDED_JURISDICTIONS][4];
    size_t                  excluded_jur_count;

    // Pre-built circuit pool for reuse (3-hop onion)
    prebuilt_circuit_t      circuit_pool[CIRCUIT_POOL_SIZE];
    uv_timer_t              pool_timer;
    bool                    pool_timer_active;

    // Pre-built session circuit pool (1-hop session-keyed)
    session_circuit_t       session_pool[SESSION_POOL_SIZE];

    // Privacy preference for adaptive routing.
    trellis_privacy_level_t privacy_level;
};

void              trellis_session_pool_init(trellis_router_t *router);
session_circuit_t *trellis_session_pool_acquire(trellis_router_t *router,
                                                const trellis_fingerprint_t *dest);
void              trellis_session_pool_expire(trellis_router_t *router);

// --- Topology manager ---
#define TRELLIS_TOPOLOGY_HEARTBEAT_MS 5000
#define TRELLIS_TOPOLOGY_MAX_NEIGHBORS 256

typedef struct neighbor_entry {
    trellis_peer_info_t info;
    double              score;
    bool                active;

    // Behavioral tracking (cached from sybil subsystem each heartbeat)
    double              behavior_score;
    double              trust_score;
    double              max_uptime_corr;  /* peak uptime correlation with any other neighbor */
} neighbor_entry_t;

struct trellis_topology {
    trellis_dht_t    *dht;
    uv_loop_t        *loop;
    uv_timer_t        heartbeat_timer;
    uint32_t          heartbeat_ms;
    size_t            target_neighbors;
    neighbor_entry_t  neighbors[TRELLIS_TOPOLOGY_MAX_NEIGHBORS];
    size_t            neighbor_count;
    bool              running;
};

// --- Flow control ---
#define FLOW_WINDOW_INIT        1000  /* initial send/recv window (cells) */
#define FLOW_WINDOW_INCREMENT   100   /* SENDME refills this many cells */
#define FLOW_CELL_SIZE          512   /* bytes per flow-control cell */

// Vegas-style congestion parameters.
#define FLOW_CWND_MIN           50
#define FLOW_CWND_MAX           2000
#define FLOW_VEGAS_ALPHA        3     /* RTT queue depth threshold (increase) */
#define FLOW_VEGAS_BETA         6     /* RTT queue depth threshold (decrease) */
#define FLOW_BDP_SMOOTHING      0.875 /* EWMA for base RTT */

// --- Circuit table (onion reverse-path state) ---
#define CIRCUIT_TABLE_MAX       512

#define CIRCUIT_MAX_HOPS 8  /* absolute maximum circuit extension depth */

typedef struct circuit_entry {
    uint32_t              stream_id;
    trellis_fingerprint_t prev_hop;
    uint8_t               layer_key[32]; /* symmetric key for re-encryption */
    uint64_t              created_at;
    bool                  active;
    uint8_t               hop_count;  /* tracks how many hops this circuit has traversed */
    uint8_t               padding_flags; /* per-circuit padding negotiation state */
} circuit_entry_t;

typedef struct circuit_table {
    circuit_entry_t       entries[CIRCUIT_TABLE_MAX];
    size_t                count;
} circuit_table_t;

void             trellis_circuit_table_init(circuit_table_t *ct);
trellis_err_t    trellis_circuit_table_insert(circuit_table_t *ct,
                                              uint32_t stream_id,
                                              const trellis_fingerprint_t *prev_hop,
                                              const uint8_t layer_key[32]);
circuit_entry_t *trellis_circuit_table_lookup(circuit_table_t *ct,
                                              uint32_t stream_id);
void             trellis_circuit_table_remove(circuit_table_t *ct,
                                              uint32_t stream_id);
void             trellis_circuit_table_expire(circuit_table_t *ct,
                                              uint64_t max_age_ms);

typedef void (*exit_circuit_write_fn)(const uint8_t *data, size_t len,
                                      uint32_t stream_id, void *ctx);

typedef struct exit_circuit_upstream {
    exit_circuit_write_fn write_fn;
    void                 *ctx;
    uint32_t              stream_id;
} exit_circuit_upstream_t;

// --- Relay ---
#define RELAY_MAX_CIRCUITS 128
#define RELAY_CIRCUIT_MAX_AGE_MS    (10 * 60 * 1000)  /* 10 minutes */
#define RELAY_CIRCUIT_IDLE_MS       (2 * 60 * 1000)   /* 2 minutes */
#define RELAY_LIFECYCLE_INTERVAL_MS 30000              /* 30 seconds */

typedef struct relay_circuit {
    trellis_fingerprint_t from;
    trellis_fingerprint_t to;
    bool                  active;
    uint64_t              bytes_relayed;
    uint64_t              created_at;
    uint64_t              last_activity_at;
    uint32_t              send_window;
    uint32_t              recv_window;
    uint32_t              deliver_count;

    // Vegas-style adaptive congestion control.
    uint32_t              cwnd;         /* congestion window (cells) */
    uint64_t              base_rtt_us;  /* minimum observed RTT */
    uint64_t              smoothed_rtt_us;
    uint32_t              inflight;     /* cells in flight (sent but not ACK'd) */

    // Session-keyed relay: set when CIRCUIT_CREATE_SESSION is processed.
    bool                  session_keyed;
    uint32_t              session_circuit_id;
    uint8_t               session_key[32];
    uint64_t              session_nonce_counter;
} relay_circuit_t;

#define RELAY_PEER_RATE_SLOTS     256
#define RELAY_PEER_CIRCUIT_LIMIT  5    /* max circuit creations per peer per window */
#define RELAY_PEER_RATE_WINDOW_MS 60000 /* 60 seconds */
#define RELAY_GLOBAL_CIRCUIT_RATE 50   /* max new circuits per second globally */

typedef struct relay_peer_rate {
    trellis_fingerprint_t peer;
    uint32_t              circuits_this_window;
    uint64_t              window_start_ms;
} relay_peer_rate_t;

typedef struct trellis_relay {
    trellis_dht_t    *dht;
    relay_circuit_t   circuits[RELAY_MAX_CIRCUITS];
    size_t            circuit_count;
    size_t            max_capacity;

    relay_peer_rate_t peer_rates[RELAY_PEER_RATE_SLOTS];
    size_t            peer_rate_count;

    uint32_t          global_circuit_count;
    uint64_t          global_circuit_window;

    // Lifecycle timer for circuit expiry.
    uv_loop_t        *loop;
    uv_timer_t        lifecycle_timer;
    bool              lifecycle_timer_active;

    // Fair queuing: round-robin index for circuit scheduling.
    size_t            rr_index;

    // Onion circuit reverse-path table.
    circuit_table_t   circuit_table;
} trellis_relay_t;

// --- NAT traversal ---
typedef enum nat_type {
    NAT_TYPE_UNKNOWN       = 0,
    NAT_TYPE_OPEN          = 1,
    NAT_TYPE_FULL_CONE     = 2,
    NAT_TYPE_RESTRICTED    = 3,
    NAT_TYPE_PORT_RESTRICT = 4,
    NAT_TYPE_SYMMETRIC     = 5,
} nat_type_t;

typedef struct trellis_nat {
    uv_loop_t  *loop;
    uv_udp_t    stun_handle;
    nat_type_t  detected_type;
    char        mapped_addr[256];
    uint16_t    mapped_port;
    bool        resolved;
} trellis_nat_t;

// --- Signed relay descriptors ---
typedef struct trellis_relay_descriptor {
    trellis_fingerprint_t fingerprint;
    uint32_t              version;
    uint64_t              published_at;
    uint32_t              capabilities;
    char                 *exit_policy;
    uint16_t              exit_policy_len;
    uint32_t              bandwidth_kbps;
    char                 *contact;
    uint8_t               contact_len;
    uint8_t              *signed_bytes;
    size_t                signed_len;
} trellis_relay_descriptor_t;

trellis_relay_descriptor_t *trellis_relay_descriptor_create(
    const trellis_fingerprint_t *fp, uint32_t capabilities,
    uint32_t bandwidth_kbps);
void    trellis_relay_descriptor_free(trellis_relay_descriptor_t *d);
void    trellis_relay_descriptor_set_exit_policy(
    trellis_relay_descriptor_t *d, const char *policy);
void    trellis_relay_descriptor_set_contact(
    trellis_relay_descriptor_t *d, const char *contact);
trellis_err_t trellis_relay_descriptor_sign(
    trellis_relay_descriptor_t *d, const trellis_identity_t *id);
trellis_err_t trellis_relay_descriptor_verify(
    const uint8_t *data, size_t len,
    const trellis_identity_public_t *pub,
    trellis_relay_descriptor_t **out);
trellis_err_t trellis_relay_descriptor_publish(
    trellis_dht_t *dht, trellis_relay_descriptor_t *d);

/* Fetch a relay descriptor from the DHT, verifying signature.
 * Results are cached for 30 minutes. */
typedef void (*trellis_relay_descriptor_cb)(
    trellis_relay_descriptor_t *desc, trellis_err_t err, void *ctx);
trellis_err_t trellis_relay_descriptor_fetch(
    trellis_dht_t *dht,
    const trellis_fingerprint_t *fp,
    const trellis_identity_public_t *pub,
    trellis_relay_descriptor_cb cb, void *ctx);

// Synchronous cache lookup (no DHT query)
trellis_relay_descriptor_t *trellis_relay_descriptor_cached(
    const trellis_fingerprint_t *fp);

// --- DoH resolver (DNS-over-HTTPS for exit relays) ---
typedef struct trellis_doh_resolver trellis_doh_resolver_t;

typedef void (*trellis_doh_resolve_cb)(const struct sockaddr_storage *addr,
                                       trellis_err_t err, void *ctx);

trellis_doh_resolver_t *trellis_doh_new(uv_loop_t *loop,
                                         const char *endpoint);
void                    trellis_doh_free(trellis_doh_resolver_t *r);
trellis_err_t           trellis_doh_resolve(trellis_doh_resolver_t *r,
                                             const char *hostname,
                                             uint16_t port,
                                             int ai_family,
                                             trellis_doh_resolve_cb cb,
                                             void *ctx);

typedef void (*trellis_doh_txt_cb)(const char *txt_data, size_t txt_len,
                                    trellis_err_t err, void *ctx);
trellis_err_t           trellis_doh_resolve_txt(trellis_doh_resolver_t *r,
                                                 const char *hostname,
                                                 trellis_doh_txt_cb cb,
                                                 void *ctx);

// --- Clock skew detection helpers ---
void    trellis_clock_sync_init(trellis_clock_sync_t *cs);
void    trellis_clock_sync_add_sample(trellis_clock_sync_t *cs, int64_t offset);
int64_t trellis_clock_sync_offset_ms(const trellis_clock_sync_t *cs);

/* Return the estimated network-adjusted time (local time + skew offset).
 * Subsystems (canary, probe_resist) should use this instead of raw
 * trellis_now_ms() when comparing timestamps from remote peers. */
static inline uint64_t trellis_adjusted_now_ms(const trellis_dht_t *dht)
{
    uint64_t now = trellis_now_ms();
    if (dht) {
        int64_t offset = trellis_clock_sync_offset_ms(&dht->clock_sync);
        if (offset > 0)
            now += (uint64_t)offset;
        else if ((uint64_t)(-offset) < now)
            now -= (uint64_t)(-offset);
    }
    return now;
}

// --- Internal helpers ---
void xor_distance(const trellis_fingerprint_t *a,
                  const trellis_fingerprint_t *b,
                  uint8_t out[TRELLIS_FINGERPRINT_LEN]);
int  bucket_index(const trellis_fingerprint_t *a,
                  const trellis_fingerprint_t *b);
void select_random_peers(const routing_table_t *rt,
                         const trellis_sybil_t *sybil,
                         trellis_peer_info_t **out,
                         size_t count, size_t *actual);

#ifdef __cplusplus
}
#endif

#endif /* TRELLIS_MESH_INTERNAL_H */
