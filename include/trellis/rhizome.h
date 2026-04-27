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

#ifndef TRELLIS_RHIZOME_H
#define TRELLIS_RHIZOME_H

#include "types.h"
#include "error.h"
#include "identity.h"
#include "mesh.h"

#ifdef _WIN32
#  include <winsock2.h>
#else
#  include <sys/socket.h>
#endif

#ifndef UV_LOOP_FORWARD_DECLARED
#define UV_LOOP_FORWARD_DECLARED
typedef struct uv_loop_s uv_loop_t;
#endif

#ifdef __cplusplus
extern "C" {
#endif

// Forward declarations for types used across sections.
#ifndef TRELLIS_CANOPY_TYPEDEF_DEFINED
#define TRELLIS_CANOPY_TYPEDEF_DEFINED
typedef struct trellis_canopy trellis_canopy_t;
#endif
typedef struct trellis_bridge_trust trellis_bridge_trust_t;

/* ═══════════════════════════════════════════════════════════════════════════
 * Rhizome: Federated Mesh Interconnection for Bloom
 *
 * Groves are sovereign mini-meshes with independent DHT namespaces, trust
 * policies, and governance.  Bridge nodes interconnect groves through the
 * Canopy routing protocol.  The aggregate system of interconnected groves
 * is the Rhizome.
 * ═══════════════════════════════════════════════════════════════════════════ */

// --- Grove Identity ---
#define TRELLIS_GROVE_ID_LEN        32
#define TRELLIS_GROVE_NAME_MAX      128
#define TRELLIS_GROVE_MAX_ADMINS    16
#define TRELLIS_GROVE_MAX_PEERS     32
#define TRELLIS_GROVE_SHORT_ID_LEN  10   /* first 10 bytes, base32 = 16 chars */

#ifndef TRELLIS_GROVE_ID_DEFINED
#define TRELLIS_GROVE_ID_DEFINED
typedef struct trellis_grove_id {
    uint8_t bytes[TRELLIS_GROVE_ID_LEN];
} trellis_grove_id_t;
#endif

bool trellis_grove_id_eq(const trellis_grove_id_t *a,
                         const trellis_grove_id_t *b);

char *trellis_grove_id_hex(const trellis_grove_id_t *id);

// Encode the first TRELLIS_GROVE_SHORT_ID_LEN bytes as base32 (16 chars).
trellis_err_t trellis_grove_id_short(const trellis_grove_id_t *id,
                                     char *out, size_t out_cap);

/* Resolve a short id back to a full grove_id from the canopy table.
 * Returns TRELLIS_ERR_NOT_FOUND if the grove is unknown. */
trellis_err_t trellis_grove_id_from_short(const char *short_id,
                                          const trellis_canopy_t *canopy,
                                          trellis_grove_id_t *out);

// --- Grove Peering Policy ---
typedef enum trellis_grove_peer_mode {
    TRELLIS_GROVE_PEER_OPEN       = 0,  /* any grove may peer */
    TRELLIS_GROVE_PEER_WHITELIST  = 1,  /* only listed groves */
    TRELLIS_GROVE_PEER_BLACKLIST  = 2,  /* all except listed groves */
} trellis_grove_peer_mode_t;

typedef struct trellis_grove_policy {
    trellis_grove_peer_mode_t peer_mode;

    // Whitelist/blacklist entries.
    trellis_grove_id_t peer_list[TRELLIS_GROVE_MAX_PEERS];
    size_t             peer_list_count;

    // Whether greenhouse descriptors are visible to cross-grove queries.
    bool               descriptors_visible;

    // Whether gossip may cross grove boundaries.
    bool               gossip_relay_allowed;

    // Max cross-grove circuits this grove will serve simultaneously.
    uint32_t           max_cross_grove_circuits;
} trellis_grove_policy_t;

trellis_grove_policy_t trellis_grove_policy_default(void);

typedef struct trellis_grove_genesis {
    trellis_grove_id_t        grove_id;     /* SHA-256(serialized genesis) */
    trellis_identity_public_t creator;
    trellis_fingerprint_t     creator_fp;
    char                      name[TRELLIS_GROVE_NAME_MAX];
    uint64_t                  created_at;
    trellis_grove_policy_t    initial_policy;
    trellis_signature_t       signature;
} trellis_grove_genesis_t;

trellis_err_t trellis_grove_genesis_serialize(const trellis_grove_genesis_t *gen,
                                              trellis_buf_t *out);
trellis_err_t trellis_grove_genesis_deserialize(const uint8_t *data, size_t len,
                                                trellis_grove_genesis_t *gen);
trellis_err_t trellis_grove_genesis_verify(const trellis_grove_genesis_t *gen);

typedef struct trellis_grove_membership {
    trellis_grove_id_t    grove_id;
    trellis_fingerprint_t member_fp;
    trellis_fingerprint_t admin_fp;    /* admin who signed this attestation */
    uint64_t              issued_at;
    uint64_t              expires_at;  /* 0 = no expiry */
    bool                  is_bridge;   /* member is authorized as a bridge */
    trellis_signature_t   signature;
} trellis_grove_membership_t;

trellis_err_t trellis_grove_membership_serialize(
        const trellis_grove_membership_t *mem,
        trellis_buf_t *out);
trellis_err_t trellis_grove_membership_deserialize(
        const uint8_t *data, size_t len,
        trellis_grove_membership_t *mem);
trellis_err_t trellis_grove_membership_verify(
        const trellis_grove_membership_t *mem,
        const trellis_identity_public_t *admin_pub);

// --- Grove Manager ---
typedef struct trellis_grove trellis_grove_t;

/* Create a new grove.  Generates the genesis record, signs it, and derives
 * the grove_id.  The caller becomes the first admin. */
trellis_grove_t *trellis_grove_create(const char *name,
                                      const trellis_identity_t *creator,
                                      const trellis_grove_policy_t *policy);

// Open an existing grove from a serialized genesis record.
trellis_grove_t *trellis_grove_open(const trellis_grove_genesis_t *genesis);

void trellis_grove_free(trellis_grove_t *grove);

const trellis_grove_id_t *trellis_grove_id(const trellis_grove_t *grove);
const trellis_grove_genesis_t *trellis_grove_genesis(const trellis_grove_t *grove);
const trellis_grove_policy_t *trellis_grove_policy(const trellis_grove_t *grove);

trellis_err_t trellis_grove_set_policy(trellis_grove_t *grove,
                                       const trellis_grove_policy_t *policy);

// Issue a membership attestation for a node. Caller must be an admin.
trellis_err_t trellis_grove_issue_membership(
        trellis_grove_t *grove,
        const trellis_identity_t *admin_id,
        const trellis_fingerprint_t *member_fp,
        bool is_bridge,
        uint64_t expires_at,
        trellis_grove_membership_t *out);

// Verify and add a membership to the grove's roster.
trellis_err_t trellis_grove_add_member(trellis_grove_t *grove,
                                       const trellis_grove_membership_t *mem,
                                       const trellis_identity_public_t *admin_pub);

// Check if a fingerprint is a member.
bool trellis_grove_is_member(const trellis_grove_t *grove,
                             const trellis_fingerprint_t *fp);

// Check if a fingerprint is an admin.
bool trellis_grove_is_admin(const trellis_grove_t *grove,
                            const trellis_fingerprint_t *fp);

// Add an admin to the grove (must be called by existing admin).
trellis_err_t trellis_grove_add_admin(trellis_grove_t *grove,
                                      const trellis_fingerprint_t *fp);

// Check if a remote grove is allowed to peer based on policy.
bool trellis_grove_peering_allowed(const trellis_grove_t *grove,
                                   const trellis_grove_id_t *remote_grove);

// --- Grove Join Protocol ---

/* Join request wire format:
 *   "GJRQ" magic(4) || version(1) || grove_id(32) || requester_fp(32) ||
 *   ed25519_pk(32) || timestamp(8) || ed25519_sig(64)
 * Total: 173 bytes */
#define TRELLIS_GROVE_JOIN_REQUEST_LEN  (4 + 1 + 32 + 32 + 32 + 8 + 64)

/* Join response wire format:
 *   "GJRS" magic(4) || version(1) || grove_id(32) || requester_fp(32) ||
 *   approved(1) || timestamp(8) || membership_len(2) || membership(var) ||
 *   ed25519_sig(64)
 * membership is only present when approved=1 */
#define TRELLIS_GROVE_JOIN_RESPONSE_MIN_LEN  (4 + 1 + 32 + 32 + 1 + 8 + 2 + 64)

typedef void (*trellis_grove_join_cb)(trellis_err_t err,
                                      const trellis_grove_membership_t *mem,
                                      void *ctx);

/* Request to join a grove.  Publishes a signed join request to the
 * target grove's DHT.  Admins monitoring the grove process requests
 * and publish responses.  Use trellis_grove_poll_join_response() to
 * check for a response. */
trellis_err_t trellis_grove_join_request(trellis_dht_t *dht,
                                         const trellis_grove_id_t *grove_id,
                                         const trellis_identity_t *requester);

/* Poll for a join response from the grove's admins.  The callback fires
 * with the membership attestation if approved, or TRELLIS_ERR_NOT_FOUND
 * if no response is available yet.
 *
 * The returned membership attestation is extracted from the DHT but the
 * outer response envelope is NOT cryptographically verified (the admin's
 * public key is not available in this context).  Callers MUST verify the
 * membership via trellis_grove_membership_verify() before trusting it. */
trellis_err_t trellis_grove_poll_join_response(
        trellis_dht_t *dht,
        const trellis_grove_id_t *grove_id,
        const trellis_fingerprint_t *requester_fp,
        trellis_grove_join_cb cb, void *ctx);

/* Process an incoming join request (admin-side handler).
 * Extracts and returns the requester's self-asserted fingerprint and
 * Ed25519 public key.  The signature is verified against the embedded
 * key, proving the sender controls it.  The fingerprint-to-key binding
 * is NOT verified here (requires the full identity bundle).  Admins
 * MUST independently verify the requester's identity before approving.
 * ed25519_pk_out may be NULL if the admin doesn't need the raw key. */
trellis_err_t trellis_grove_on_join_request(trellis_grove_t *grove,
                                             const uint8_t *data, size_t len,
                                             trellis_fingerprint_t *requester_fp_out,
                                             uint8_t *ed25519_pk_out);

/* Approve or deny a join request.  If approved, issues a membership
 * attestation and sends the response. */
trellis_err_t trellis_grove_join_respond(trellis_grove_t *grove,
                                          trellis_dht_t *dht,
                                          const trellis_identity_t *admin_id,
                                          const trellis_fingerprint_t *requester_fp,
                                          bool approve,
                                          uint64_t expires_at);

/* Derive a grove-scoped DHT key from a raw key.
 * grove_scoped_key = HKDF-SHAKE-256(ikm=raw_key, salt=grove_id, info="grove-ns")[:32]
 * If grove_id is NULL, copies raw_key unchanged (single-mesh mode). */
void trellis_grove_scope_key(const trellis_grove_id_t *grove_id,
                             const uint8_t *raw_key, size_t raw_key_len,
                             uint8_t *scoped_key_out);

/* Bind a grove to a DHT instance.  All subsequent store/find operations
 * will automatically scope keys via the grove namespace.
 * Pass NULL to unbind (revert to global single-mesh mode). */
trellis_err_t trellis_dht_set_grove(trellis_dht_t *dht,
                                    const trellis_grove_id_t *grove_id);
const trellis_grove_id_t *trellis_dht_grove(const trellis_dht_t *dht);

// --- Bridge Node ---
#define TRELLIS_BRIDGE_MAX_GROVES  8

typedef struct trellis_bridge trellis_bridge_t;

/* Bridge attestation: proves the same entity controls identities in two
 * groves without revealing cross-grove linkability to non-bridge peers. */
typedef struct trellis_bridge_attestation {
    trellis_grove_id_t    grove_a;
    trellis_grove_id_t    grove_b;
    trellis_fingerprint_t fp_in_a;
    trellis_fingerprint_t fp_in_b;
    uint64_t              issued_at;
    uint64_t              expires_at;
    trellis_signature_t   sig_a;   /* signed by identity in grove A */
    trellis_signature_t   sig_b;   /* signed by identity in grove B */
} trellis_bridge_attestation_t;

trellis_err_t trellis_bridge_attestation_serialize(
        const trellis_bridge_attestation_t *att,
        trellis_buf_t *out);
trellis_err_t trellis_bridge_attestation_deserialize(
        const uint8_t *data, size_t len,
        trellis_bridge_attestation_t *att);
trellis_err_t trellis_bridge_attestation_verify(
        const trellis_bridge_attestation_t *att,
        const trellis_identity_public_t *pub_a,
        const trellis_identity_public_t *pub_b);

// Create a bridge node. home_dht is the primary grove's DHT instance.
trellis_bridge_t *trellis_bridge_new(trellis_dht_t *home_dht,
                                     trellis_router_t *home_router,
                                     const trellis_identity_t *identity,
                                     uv_loop_t *loop);
void trellis_bridge_free(trellis_bridge_t *bridge);

/* Join an additional grove.  Bootstraps a new DHT instance with the given
 * seed address and grove identity.  Returns the index of the new grove
 * slot on success. */
trellis_err_t trellis_bridge_add_grove(trellis_bridge_t *bridge,
                                       const trellis_grove_id_t *grove_id,
                                       const char *seed_addr,
                                       int *slot_out);

// Leave a grove. Tears down the DHT instance and removes the slot.
trellis_err_t trellis_bridge_remove_grove(trellis_bridge_t *bridge,
                                          const trellis_grove_id_t *grove_id);

// Policy-checked forwarding of a DHT operation across grove boundary.
trellis_err_t trellis_bridge_forward_find(trellis_bridge_t *bridge,
                                          const trellis_grove_id_t *target_grove,
                                          const uint8_t *key, size_t key_len,
                                          trellis_dht_find_cb cb, void *ctx);

trellis_err_t trellis_bridge_forward_store(trellis_bridge_t *bridge,
                                           const trellis_grove_id_t *target_grove,
                                           const uint8_t *key, size_t key_len,
                                           const uint8_t *value, size_t value_len,
                                           trellis_dht_store_cb cb, void *ctx);

// Publish bridge reachability advertisement to connected groves.
trellis_err_t trellis_bridge_advertise(trellis_bridge_t *bridge);

// Get the number of groves this bridge participates in.
size_t trellis_bridge_grove_count(const trellis_bridge_t *bridge);

// Get the DHT instance for a specific grove (by index).
trellis_dht_t *trellis_bridge_grove_dht(const trellis_bridge_t *bridge,
                                        size_t index);

// Get the grove_id for a specific slot.
const trellis_grove_id_t *trellis_bridge_grove_at(const trellis_bridge_t *bridge,
                                                   size_t index);

// Get the grove policy for a specific slot (NULL if no grove manager).
const trellis_grove_policy_t *trellis_bridge_grove_policy(
        const trellis_bridge_t *bridge, size_t index);

// Check if a remote grove is allowed by the home grove's peering policy.
bool trellis_bridge_peering_allowed(const trellis_bridge_t *bridge,
                                    const trellis_grove_id_t *remote_grove);

// Get the bridge's identity (for signing announcements).
const trellis_identity_t *trellis_bridge_identity(const trellis_bridge_t *bridge);

// Get the bridge's fingerprint.
const trellis_fingerprint_t *trellis_bridge_fingerprint(const trellis_bridge_t *bridge);

// Send a message via a grove's DHT send function (for canopy layer).
trellis_err_t trellis_bridge_dht_send(const trellis_bridge_t *bridge,
                                      size_t grove_index,
                                      const trellis_fingerprint_t *target,
                                      const uint8_t *data, size_t len);

// Select random peers from a grove's routing table (for canopy gossip).
trellis_err_t trellis_bridge_select_peers(const trellis_bridge_t *bridge,
                                          size_t grove_index,
                                          trellis_peer_info_t **out,
                                          size_t count, size_t *actual);

// --- Canopy Protocol (Inter-Grove Routing) ---
#define TRELLIS_CANOPY_MAX_PATH_LEN   16
#define TRELLIS_CANOPY_MAX_ROUTES     256
#define TRELLIS_CANOPY_ANNOUNCE_INTERVAL_MS  30000
#define TRELLIS_CANOPY_ROUTE_EXPIRE_MS       120000

typedef struct trellis_canopy_route {
    trellis_grove_id_t    target_grove;
    trellis_grove_id_t    path[TRELLIS_CANOPY_MAX_PATH_LEN];
    size_t                path_len;
    trellis_fingerprint_t next_bridge_fp;
    uint8_t               bridge_ed25519_pk[32]; /* TOFU: pinned on first announcement */
    uint32_t              capabilities;
    uint64_t              last_seen;   /* local receive time (for expiry) */
    uint64_t              remote_ts;   /* remote announcement timestamp (for freshness comparison) */
    double                score;
} trellis_canopy_route_t;

/* Canopy announcement wire format:
 * magic(4) || version(1) || origin_grove_id(32) || path_len(1) ||
 * grove_id_path(32 * path_len) || bridge_fp(32) || ed25519_pk(32) ||
 * capabilities(4) || ttl(1) || timestamp(8) || ed25519_sig(64) */
#define TRELLIS_CANOPY_ANNOUNCE_MAGIC  "CANO"
#define TRELLIS_CANOPY_ANNOUNCE_VERSION 1

trellis_canopy_t *trellis_canopy_new(trellis_bridge_t *bridge, uv_loop_t *loop);

// Retrieve the bridge backing this canopy instance.
trellis_bridge_t *trellis_canopy_bridge(const trellis_canopy_t *canopy);

/* Attach a bridge trust system for trust-weighted route scoring.
 * Must be called before trellis_canopy_start(). bt may be NULL. */
void trellis_canopy_set_trust(trellis_canopy_t *canopy,
                               trellis_bridge_trust_t *bt);

void              trellis_canopy_free(trellis_canopy_t *canopy);

// Start periodic announcement broadcasts.
trellis_err_t trellis_canopy_start(trellis_canopy_t *canopy);
void          trellis_canopy_stop(trellis_canopy_t *canopy);

// Broadcast reachability to all peered groves.
trellis_err_t trellis_canopy_announce(trellis_canopy_t *canopy);

// Process an incoming canopy announcement.
trellis_err_t trellis_canopy_on_announce(trellis_canopy_t *canopy,
                                         const uint8_t *data, size_t len,
                                         const trellis_fingerprint_t *sender);

/* Process an incoming canopy withdrawal.
 * Wire format: "CANW" magic(4) || grove_id(32) || bridge_fp(32) ||
 *              bridge_ed25519_pk(32) || timestamp(8) || ed25519_sig(64) */
trellis_err_t trellis_canopy_on_withdraw(trellis_canopy_t *canopy,
                                          const uint8_t *data, size_t len,
                                          const trellis_fingerprint_t *sender);

// Determine the next bridge hop for a target grove.
trellis_err_t trellis_canopy_route(const trellis_canopy_t *canopy,
                                   const trellis_grove_id_t *target_grove,
                                   trellis_canopy_route_t *route_out);

// Get the number of known grove routes.
size_t trellis_canopy_route_count(const trellis_canopy_t *canopy);

// Check if a specific grove is reachable.
bool trellis_canopy_grove_reachable(const trellis_canopy_t *canopy,
                                    const trellis_grove_id_t *grove_id);

// Build a cross-grove onion circuit via bridge chain.
typedef void (*trellis_canopy_circuit_cb)(trellis_err_t err,
                                          uint32_t circuit_id,
                                          void *ctx);

trellis_err_t trellis_canopy_circuit_create(trellis_canopy_t *canopy,
                                            const trellis_grove_id_t *target_grove,
                                            const trellis_fingerprint_t *dest,
                                            trellis_canopy_circuit_cb cb,
                                            void *ctx);

trellis_err_t trellis_canopy_circuit_send(trellis_canopy_t *canopy,
                                          uint32_t circuit_id,
                                          const uint8_t *data, size_t len);

trellis_err_t trellis_canopy_circuit_destroy(trellis_canopy_t *canopy,
                                             uint32_t circuit_id);

/* Optional receiver for application payloads on circuits where this node is
 * an endpoint (cross-grove greenhouse CDHT responses). */
typedef void (*trellis_canopy_app_recv_fn)(uint32_t circuit_id,
                                           const uint8_t *payload,
                                           size_t payload_len,
                                           void *ctx);

trellis_err_t trellis_canopy_circuit_set_app_recv(trellis_canopy_t *canopy,
                                                  uint32_t circuit_id,
                                                  trellis_canopy_app_recv_fn fn,
                                                  void *ctx);

/* Process an incoming circuit create request (bridge node handler).
 * Wire: circuit_id(4) || target_grove(32) || dest_fp(32) */
trellis_err_t trellis_canopy_on_circuit_create(trellis_canopy_t *canopy,
                                               const uint8_t *data, size_t len,
                                               const trellis_fingerprint_t *sender);

/* Process incoming circuit data (bridge node handler).
 * Wire: circuit_id(4) || payload_len(4) || payload(N) */
trellis_err_t trellis_canopy_on_circuit_data(trellis_canopy_t *canopy,
                                             const uint8_t *data, size_t len,
                                             const trellis_fingerprint_t *sender);

/* Process an incoming circuit destroy (bridge node handler).
 * Wire: circuit_id(4) */
trellis_err_t trellis_canopy_on_circuit_destroy(trellis_canopy_t *canopy,
                                                const uint8_t *data, size_t len,
                                                const trellis_fingerprint_t *sender);

// --- Bridge Trust ---
trellis_bridge_trust_t *trellis_bridge_trust_new(trellis_sybil_t *sybil);
void                    trellis_bridge_trust_free(trellis_bridge_trust_t *bt);

// Score a bridge node's reliability for cross-grove routing.
double trellis_bridge_trust_score(const trellis_bridge_trust_t *bt,
                                  const trellis_fingerprint_t *bridge_fp);

// Record a successful cross-grove relay through a bridge.
void trellis_bridge_trust_record_success(trellis_bridge_trust_t *bt,
                                         const trellis_fingerprint_t *bridge_fp);

// Record a failed cross-grove relay through a bridge.
void trellis_bridge_trust_record_failure(trellis_bridge_trust_t *bt,
                                         const trellis_fingerprint_t *bridge_fp);

// Verify a bridge attestation and register it in the trust system.
trellis_err_t trellis_bridge_trust_register(trellis_bridge_trust_t *bt,
                                            const trellis_bridge_attestation_t *att,
                                            const trellis_identity_public_t *pub_a,
                                            const trellis_identity_public_t *pub_b);

/* Record a bridge's network address for /16 subnet diversity tracking.
 * Call when a bridge's transport address is learned (e.g. after handshake). */
void trellis_bridge_trust_record_addr(trellis_bridge_trust_t *bt,
                                       const trellis_fingerprint_t *bridge_fp,
                                       const struct sockaddr *addr);

// Check minimum bridge diversity for a grove (different /16 subnets).
bool trellis_bridge_trust_diversity_ok(const trellis_bridge_trust_t *bt,
                                       const trellis_grove_id_t *grove_id);

#ifdef __cplusplus
}
#endif

#endif /* TRELLIS_RHIZOME_H */
