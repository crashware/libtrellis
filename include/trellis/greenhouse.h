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

#ifndef TRELLIS_GREENHOUSE_H
#define TRELLIS_GREENHOUSE_H

#include "types.h"
#include "error.h"
#include "identity.h"
#include "crypto.h"
#include "protocol.h"
#include "mesh.h"

// Forward declare uv_loop_t.
#ifndef UV_LOOP_FORWARD_DECLARED
#define UV_LOOP_FORWARD_DECLARED
typedef struct uv_loop_s uv_loop_t;
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef enum trellis_greenhouse_type {
    TRELLIS_GH_PERSISTENT    = 0,
    TRELLIS_GH_EPHEMERAL     = 1,
    TRELLIS_GH_AUTHENTICATED = 2,
    TRELLIS_GH_FEDERATED     = 3,
} trellis_greenhouse_type_t;

typedef enum trellis_gh_error_code {
    TRELLIS_GH_ERR_NO_SERVICE   = 1,   /* no registered service for target */
    TRELLIS_GH_ERR_NO_SESSION   = 2,   /* no pending rendezvous for cookie */
    TRELLIS_GH_ERR_BAD_PAYLOAD  = 3,   /* malformed or undersized payload */
    TRELLIS_GH_ERR_INTERNAL     = 4,   /* generic internal error */
} trellis_gh_error_code_t;

#define TRELLIS_BLOOM_ADDR_PREFIX "bloom:"

// Grove identity type — full definition needed for member access.
#ifndef TRELLIS_GROVE_ID_DEFINED
#define TRELLIS_GROVE_ID_DEFINED
#ifndef TRELLIS_GROVE_ID_LEN
#define TRELLIS_GROVE_ID_LEN 32
#endif
typedef struct trellis_grove_id {
    uint8_t bytes[TRELLIS_GROVE_ID_LEN];
} trellis_grove_id_t;
#endif

typedef struct trellis_bloom_addr {
    trellis_fingerprint_t fingerprint;
    char                  uri[128]; /* bloom:<base32-fingerprint>[@<grove-short>] */
    bool                  has_grove;
    uint8_t               grove_id_short[10]; /* first 10 bytes of grove_id */
} trellis_bloom_addr_t;

trellis_err_t trellis_bloom_addr_from_fingerprint(const trellis_fingerprint_t *fp,
                                                   trellis_bloom_addr_t *addr);

// Create a cross-grove bloom address with an explicit grove.
trellis_err_t trellis_bloom_addr_from_fingerprint_grove(
        const trellis_fingerprint_t *fp,
        const trellis_grove_id_t *grove_id,
        trellis_bloom_addr_t *addr);

trellis_err_t trellis_bloom_addr_parse(const char *uri,
                                        trellis_bloom_addr_t *addr);
trellis_err_t trellis_bloom_addr_descriptor_key(const trellis_fingerprint_t *fp,
                                                 uint8_t *key_out);

typedef struct trellis_intro_point {
    trellis_fingerprint_t node_fingerprint;
    uint8_t               onion_key[TRELLIS_ML_KEM_1024_PK_LEN];
    char                 *address_hints[8];
    size_t                hint_count;
} trellis_intro_point_t;

typedef struct trellis_service_descriptor {
    trellis_identity_public_t service_identity;
    trellis_intro_point_t    *intro_points;
    size_t                    intro_point_count;
    uint8_t                   protocol_version;
    uint64_t                  created_at;
    uint64_t                  expires_at;
    trellis_signature_t       signature;

    // Rhizome: grove this service belongs to (zero = global/unscoped)
    bool                      has_grove;
    trellis_grove_id_t        grove_id;
} trellis_service_descriptor_t;

trellis_err_t trellis_descriptor_sign(trellis_service_descriptor_t *desc,
                                       const trellis_identity_t *id);
trellis_err_t trellis_descriptor_verify(const trellis_service_descriptor_t *desc);
trellis_err_t trellis_descriptor_serialize(const trellis_service_descriptor_t *desc,
                                            trellis_buf_t *out);
trellis_err_t trellis_descriptor_deserialize(const uint8_t *data, size_t len,
                                              trellis_service_descriptor_t *desc);
bool          trellis_descriptor_is_expired(const trellis_service_descriptor_t *desc);
void          trellis_descriptor_free(trellis_service_descriptor_t *desc);

size_t        trellis_descriptor_intro_point_count(
                  const trellis_service_descriptor_t *desc);
trellis_err_t trellis_descriptor_get_intro_point(
                  const trellis_service_descriptor_t *desc,
                  size_t index,
                  trellis_fingerprint_t *fp_out,
                  uint8_t *onion_key_out);
trellis_err_t trellis_descriptor_get_service_identity(
                  const trellis_service_descriptor_t *desc,
                  uint8_t *buf, size_t buf_len, size_t *written);

#define TRELLIS_ONION_DEFAULT_HOPS 3

typedef struct trellis_onion_circuit trellis_onion_circuit_t;

trellis_onion_circuit_t *trellis_onion_new(size_t hop_count);
void                     trellis_onion_free(trellis_onion_circuit_t *circuit);

trellis_err_t trellis_onion_add_hop(trellis_onion_circuit_t *circuit,
                                     const trellis_fingerprint_t *relay,
                                     const uint8_t *x25519_pk,
                                     const uint8_t *ml_kem_pk);
trellis_err_t trellis_onion_wrap(trellis_onion_circuit_t *circuit,
                                  const uint8_t *data, size_t len,
                                  trellis_buf_t *out);
trellis_err_t trellis_onion_peel(const trellis_kem_keypair_t *kp,
                                  const uint8_t *data, size_t len,
                                  trellis_buf_t *out,
                                  trellis_fingerprint_t *next_hop);
trellis_err_t trellis_onion_peel_key(const trellis_kem_keypair_t *kp,
                                      const uint8_t *data, size_t len,
                                      trellis_buf_t *out,
                                      trellis_fingerprint_t *next_hop,
                                      uint8_t layer_key_out[32]);

#define TRELLIS_RENDEZVOUS_COOKIE_LEN 32

typedef struct trellis_rendezvous trellis_rendezvous_t;

trellis_rendezvous_t *trellis_rendezvous_new(void);
void                  trellis_rendezvous_free(trellis_rendezvous_t *rv);

trellis_err_t trellis_rendezvous_establish(trellis_rendezvous_t *rv,
                                            uint8_t *cookie_out);
trellis_err_t trellis_rendezvous_join(trellis_rendezvous_t *rv,
                                       const uint8_t *cookie);
trellis_err_t trellis_rendezvous_bridge(trellis_rendezvous_t *rv);

/* Create a connected in-memory conn pair for direct rendezvous.
 * Writing to *a_out delivers to *b_out's recv callback, and vice versa. */
trellis_err_t trellis_rendezvous_conn_pair(trellis_conn_t **a_out,
                                            trellis_conn_t **b_out);

typedef struct trellis_greenhouse trellis_greenhouse_t;

typedef void (*trellis_greenhouse_conn_cb)(trellis_conn_t *tunnel,
                                           const trellis_fingerprint_t *client,
                                           void *ctx);

trellis_greenhouse_t *trellis_greenhouse_new(const trellis_identity_t *service_id,
                                              trellis_greenhouse_type_t type,
                                              trellis_dht_t *dht);
void                  trellis_greenhouse_free(trellis_greenhouse_t *gh);

/* Start the service. loop is used for intro-circuit keepalive timers and
 * may be NULL (timer maintenance is disabled when loop is NULL). */
trellis_err_t trellis_greenhouse_start(trellis_greenhouse_t *gh,
                                        trellis_greenhouse_conn_cb cb, void *ctx);
trellis_err_t trellis_greenhouse_start_with_loop(trellis_greenhouse_t *gh,
                                                  trellis_greenhouse_conn_cb cb,
                                                  void *ctx, uv_loop_t *loop);
trellis_err_t trellis_greenhouse_stop(trellis_greenhouse_t *gh);
trellis_err_t trellis_greenhouse_connect(trellis_dht_t *dht,
                                          const trellis_bloom_addr_t *addr,
                                          trellis_connect_cb cb, void *ctx);

/* Cross-node rendezvous: sends INTRODUCE1 via `router` to the service's intro
 * relay so the service can join the rendezvous relay and establish a tunnel.
 * Pass router=NULL for the in-process / test fallback. */
trellis_err_t trellis_greenhouse_connect_with_router(
                                          trellis_dht_t *dht,
                                          trellis_router_t *router,
                                          const trellis_bloom_addr_t *addr,
                                          trellis_connect_cb cb, void *ctx);

// Forward declaration for canopy (rhizome.h defines the full type).
#ifndef TRELLIS_CANOPY_TYPEDEF_DEFINED
#define TRELLIS_CANOPY_TYPEDEF_DEFINED
typedef struct trellis_canopy trellis_canopy_t;
#endif

/* Cross-grove Greenhouse connect: resolves address's grove-id-short via canopy,
 * fetches the descriptor through a canopy circuit, then proceeds to rendezvous.
 * The canopy must be started and have a route to the target grove. */
trellis_err_t trellis_greenhouse_connect_cross_grove(
                                          trellis_dht_t *dht,
                                          trellis_router_t *router,
                                          trellis_canopy_t *canopy,
                                          const trellis_bloom_addr_t *addr,
                                          trellis_connect_cb cb, void *ctx);

/*
 * Set the router used by this greenhouse to send ESTABLISH_INTRO messages to
 * relay nodes.  When a router is set, trellis_greenhouse_start() will send a
 * real ESTABLISH_INTRO cell to each selected relay via trellis_router_send().
 * Without a router, intro points are recorded in the descriptor but the relay
 * is not notified (useful for loopback / unit tests).
 */
void trellis_greenhouse_set_router(trellis_greenhouse_t *gh,
                                    trellis_router_t *router);

/* Global service registry: register/unregister a greenhouse so that
 * incoming INTRODUCE2 messages can be dispatched to the correct service. */
void trellis_greenhouse_register(trellis_greenhouse_t *gh);
void trellis_greenhouse_unregister(trellis_greenhouse_t *gh);

#ifndef TRELLIS_SURB_TYPEDEF_DEFINED
#define TRELLIS_SURB_TYPEDEF_DEFINED
typedef struct trellis_surb trellis_surb_t;
#endif

/* Send a reply via a SURB for unlinkable service responses.
 * The SURB is consumed (marked used) after a successful call. */
trellis_err_t trellis_greenhouse_reply_via_surb(trellis_greenhouse_t *gh,
                                                trellis_surb_t *surb,
                                                const uint8_t *data, size_t len);

/* Authenticated mode: add/remove a client fingerprint from the allow-list.
 * Has no effect for non-TRELLIS_GH_AUTHENTICATED services. */
trellis_err_t trellis_greenhouse_acl_add(trellis_greenhouse_t *gh,
                                          const trellis_fingerprint_t *fp);
trellis_err_t trellis_greenhouse_acl_remove(trellis_greenhouse_t *gh,
                                             const trellis_fingerprint_t *fp);

/* Federated mode (Section 9.7): configure this node's share of the service
 * identity.  `share` is a Shamir share blob produced by
 * trellis_identity_split_secret().  `threshold` / `total` are the k and n
 * parameters.  `peer_fps` lists the fingerprints of all federation peers
 * (including self); `peer_count` <= TRELLIS_MAX_INTRO_POINTS.
 * Must be called before trellis_greenhouse_start(). */
trellis_err_t trellis_greenhouse_set_federation(
                    trellis_greenhouse_t *gh,
                    const uint8_t *share, size_t share_len,
                    int share_idx,
                    int threshold, int total,
                    const trellis_fingerprint_t *peer_fps,
                    size_t peer_count);

/*
 * Construct a SURB.  The sender provides:
 *   rv_fp       — fingerprint of the rendezvous relay the sender listens on
 *   rv_identity — public KEM keys of the rv relay (distinct from relay hops)
 *   hops        — ordered list of DHT peers forming the return path
 *   hop_count   — number of hops (1..SURB_MAX_HOPS)
 *
 * The rv_identity must be distinct from the last relay hop's identity;
 * sharing keys would allow the last relay to decrypt the RV layer and
 * deanonymize the rendezvous point.
 *
 * dummy_payload and dummy_len are ignored (kept for ABI compatibility);
 * the inner onion payload is always the derived reply_key.
 */
trellis_surb_t *trellis_surb_build(
    const trellis_fingerprint_t    *rv_fp,
    const trellis_identity_public_t *rv_identity,
    const trellis_peer_info_t      **hops,
    size_t                           hop_count,
    const uint8_t                   *dummy_payload,
    size_t                           dummy_len);

void trellis_surb_free(trellis_surb_t *s);

trellis_err_t trellis_surb_serialize(const trellis_surb_t *s, trellis_buf_t *out);
trellis_err_t trellis_surb_deserialize(const uint8_t *data, size_t len,
                                        trellis_surb_t **out);

/*
 * Use a SURB to send a reply.  The reply is XSalsa20-Poly1305 encrypted
 * under the SURB's reply_key.  On success, `onion_out` contains the packet
 * [nonce(24) || ct_len(4) || ct || onion_header]; route toward entry hop.
 * After this call the SURB is marked used and cannot be reused.
 */
trellis_err_t trellis_surb_reply(trellis_surb_t *s,
                                  const uint8_t *reply_data, size_t reply_len,
                                  trellis_buf_t *onion_out);

/*
 * Decrypt a SURB reply blob [nonce(24) || ct_len(4) || ct] as delivered by
 * the rv node.  On success, *reply_out is malloc'd; caller must free it.
 */
trellis_err_t trellis_surb_decrypt_reply(const trellis_surb_t *s,
                                          const uint8_t *sealed, size_t sealed_len,
                                          uint8_t **reply_out, size_t *reply_len_out);

bool                         trellis_surb_is_used(const trellis_surb_t *s);
const trellis_fingerprint_t *trellis_surb_entry_hop(const trellis_surb_t *s);
const trellis_fingerprint_t *trellis_surb_rv_fingerprint(const trellis_surb_t *s);

typedef struct trellis_reverse_proxy trellis_reverse_proxy_t;

trellis_reverse_proxy_t *trellis_reverse_proxy_new(trellis_greenhouse_t *gh,
                                                    uv_loop_t *loop,
                                                    const char *backend_addr);
void                      trellis_reverse_proxy_free(trellis_reverse_proxy_t *rp);
trellis_err_t             trellis_reverse_proxy_start(trellis_reverse_proxy_t *rp);
trellis_err_t             trellis_reverse_proxy_stop(trellis_reverse_proxy_t *rp);

typedef struct trellis_dms trellis_dms_t;
typedef struct dms_watch_ctx trellis_dms_watch_t;

/*
 * Called when a watched DMS fires (heartbeat expired).
 * value / value_len: decrypted alarm message plaintext.
 */
typedef void (*trellis_dms_alarm_cb)(const trellis_fingerprint_t *svc_fp,
                                      const uint8_t *value, size_t value_len,
                                      void *ctx);

// Operator side.
trellis_dms_t  *trellis_dms_new(trellis_dht_t *dht,
                                  trellis_identity_t *service_id);
void            trellis_dms_free(trellis_dms_t *dms);
trellis_err_t   trellis_dms_add_recipient(trellis_dms_t *dms,
                                           const trellis_fingerprint_t *rcpt_fp,
                                           const uint8_t *rcpt_pk,
                                           const uint8_t *message,
                                           size_t message_len);
trellis_err_t   trellis_dms_arm(trellis_dms_t *dms, uv_loop_t *loop);
trellis_err_t   trellis_dms_disarm(trellis_dms_t *dms);
trellis_err_t   trellis_dms_heartbeat(trellis_dms_t *dms);

// Recipient side.
trellis_dms_watch_t *trellis_dms_watch(trellis_dht_t *dht,
                                        uv_loop_t *loop,
                                        const trellis_fingerprint_t *svc_fp,
                                        const trellis_fingerprint_t *my_fp,
                                        const uint8_t *my_ml_kem_sk,
                                        trellis_dms_alarm_cb cb, void *ctx);
void                 trellis_dms_unwatch(trellis_dms_watch_t *watch);

/* Dispatch a pre-parsed Trellis message to the appropriate greenhouse handler.
 * Used by the fuzz harness and integration tests.  Safe to call with any
 * message type; unknown types are silently ignored. */
void trellis_greenhouse_dispatch_msg(const trellis_message_t *msg);

#ifdef __cplusplus
}
#endif

#endif /* TRELLIS_GREENHOUSE_H */
