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

#ifndef TRELLIS_PROTOCOL_H
#define TRELLIS_PROTOCOL_H

#include "types.h"
#include "error.h"
#include "identity.h"
#include "crypto.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum trellis_msg_type {
    // Identity & presence.
    TRELLIS_MSG_ALIAS         = 0x0001,
    TRELLIS_MSG_LOOKUP        = 0x0002,
    TRELLIS_MSG_ANNOUNCE      = 0x0003,

    // Garden operations.
    TRELLIS_MSG_SUBSCRIBE     = 0x0010,
    TRELLIS_MSG_UNSUBSCRIBE   = 0x0011,
    TRELLIS_MSG_MSG           = 0x0012,
    TRELLIS_MSG_PERMIT        = 0x0013,
    TRELLIS_MSG_REVOKE        = 0x0014,
    TRELLIS_MSG_META          = 0x0015,
    TRELLIS_MSG_DISCOVER      = 0x0016,

    // Mesh control.
    TRELLIS_MSG_PING          = 0x0020,
    TRELLIS_MSG_PONG          = 0x0021,
    TRELLIS_MSG_FIND_NODE     = 0x0022,
    TRELLIS_MSG_FIND_VALUE    = 0x0023,
    TRELLIS_MSG_STORE         = 0x0024,

    // Handshake
    TRELLIS_MSG_HANDSHAKE_1   = 0x0030,
    TRELLIS_MSG_HANDSHAKE_2   = 0x0031,
    TRELLIS_MSG_HANDSHAKE_3   = 0x0032,

    // Greenhouse
    TRELLIS_MSG_GH_DESCRIPTOR     = 0x0040,
    TRELLIS_MSG_GH_INTRODUCE      = 0x0041,
    TRELLIS_MSG_GH_RENDEZVOUS_EST = 0x0042,
    TRELLIS_MSG_GH_RENDEZVOUS_JOIN = 0x0043,
    TRELLIS_MSG_GH_RENDEZVOUS_CONN = 0x0044,
    TRELLIS_MSG_GH_ERROR           = 0x0045,

    // Gossip
    TRELLIS_MSG_GOSSIP        = 0x0050,

    // Relay
    TRELLIS_MSG_RELAY         = 0x0060,
    TRELLIS_MSG_SENDME        = 0x0061,  /* window-based flow control */

    // Peer exchange.
    TRELLIS_MSG_PEER_EXCHANGE = 0x0070,

    // Onion routing.
    TRELLIS_MSG_ONION         = 0x0080,

    // Exit relay stream request (inner onion payload)
    TRELLIS_MSG_EXIT_STREAM   = 0x0081,
    TRELLIS_MSG_CIRCUIT_DATA  = 0x0082,  /* bidirectional circuit return path */
    TRELLIS_MSG_CIRCUIT_DESTROY = 0x0083, /* tear down circuit at all hops */

    // Bandwidth probe: measure actual throughput to a peer.
    TRELLIS_MSG_BW_PROBE      = 0x0084, /* probe request: [probe_id(4)][payload_size(4)] */
    TRELLIS_MSG_BW_PROBE_ACK  = 0x0085, /* probe response: [probe_id(4)][recv_bytes(4)] */

    // Padding negotiation.
    TRELLIS_MSG_PADDING_NEGOTIATE = 0x0086, /* per-circuit padding params */

    // Session-keyed relay (low-overhead media relay)
    TRELLIS_MSG_CIRCUIT_CREATE_SESSION  = 0x0087, /* KEM-encapsulated session key proposal */
    TRELLIS_MSG_CIRCUIT_CREATED_SESSION = 0x0088, /* relay confirms session established */
    TRELLIS_MSG_SESSION_DATA            = 0x0089, /* fast-path: [circuit_id(4)][dest_fp(32)][nonce(12)][ct][tag(16)] */

    // Capability advertisement exchanged immediately after handshake.
    TRELLIS_MSG_CAPABILITIES  = 0x0090,

    // Bandwidth receipt (relay incentive system)
    TRELLIS_MSG_BW_RECEIPT    = 0x00A0,

    // Cover traffic (DPI resistance) — receiver decrypts, sees COVER, discards.
    TRELLIS_MSG_COVER         = 0x00FF,

    /* WebRTC media signaling (Section 8.7) — placed in the 0x8000+ extension
     * range as per the extension registry spec (TRELLIS_EXT_MIN_TYPE). */
    TRELLIS_MSG_WEBRTC_OFFER        = 0x8001,   /* SDP offer */
    TRELLIS_MSG_WEBRTC_ANSWER       = 0x8002,   /* SDP answer */
    TRELLIS_MSG_WEBRTC_ICE          = 0x8003,   /* ICE candidate */
    TRELLIS_MSG_WEBRTC_HANGUP       = 0x8004,   /* end session */
    TRELLIS_MSG_WEBRTC_RENEGOTIATE  = 0x8005,   /* re-offer (e.g. screen-share added) */

    // Rhizome / Canopy inter-grove routing (0x9000+ range)
    TRELLIS_MSG_CANOPY_ANNOUNCE       = 0x9001,  /* grove reachability advert */
    TRELLIS_MSG_CANOPY_WITHDRAW       = 0x9002,  /* grove unreachability */
    TRELLIS_MSG_CANOPY_CIRCUIT_CREATE = 0x9003,  /* cross-grove circuit ext */
    TRELLIS_MSG_CANOPY_CIRCUIT_DATA   = 0x9004,  /* cross-grove circuit data */
    TRELLIS_MSG_CANOPY_CIRCUIT_DESTROY = 0x9005, /* cross-grove circuit tear */
    TRELLIS_MSG_GROVE_JOIN_REQUEST     = 0x9010, /* membership request */
    TRELLIS_MSG_GROVE_JOIN_RESPONSE    = 0x9011, /* membership approve/deny */
} trellis_msg_type_t;

#define TRELLIS_FLAG_ENCRYPTED      0x01
#define TRELLIS_FLAG_COMPRESSED     0x02
#define TRELLIS_FLAG_URGENT         0x04
#define TRELLIS_FLAG_RELAYED        0x08

/* Morph bypass: skip the morph/timing engine for this message (media streams).
 * Only valid when negotiated_caps includes "morph-bypass" feature. */
#define TRELLIS_FLAG_MORPH_BYPASS   0x20

/* Sealed Sender: the source fingerprint in the header is zeroed.
 * The real sender fingerprint is encrypted under the recipient's public KEM
 * key and prepended to the payload (inside any per-hop onion encryption).
 * Only the final recipient can recover the sender identity. */
#define TRELLIS_FLAG_SEALED_SENDER  0x10

/* version(1) + type(2) + flags(1) + sequence(8) + timestamp(8) +
   source(32) + target(32) + payload_len(4) = 88                           */
#define TRELLIS_MSG_HEADER_LEN 88

typedef struct trellis_message {
    uint8_t             version;
    uint16_t            type;
    uint8_t             flags;
    uint64_t            sequence;
    uint64_t            timestamp;
    trellis_fingerprint_t source;
    trellis_fingerprint_t target;
    uint32_t            payload_len;
    uint8_t            *payload;
    trellis_signature_t signature;
} trellis_message_t;

trellis_err_t trellis_message_serialize(const trellis_message_t *msg,
                                        trellis_buf_t *out);
trellis_err_t trellis_message_deserialize(const uint8_t *data, size_t len,
                                          trellis_message_t *msg);
trellis_err_t trellis_message_sign(trellis_message_t *msg,
                                   const trellis_identity_t *id);
trellis_err_t trellis_message_verify(const trellis_message_t *msg,
                                     const trellis_identity_public_t *pub);
void          trellis_message_free(trellis_message_t *msg);

/*
 * Sealed sender: encrypt the sender's fingerprint under the recipient's
 * public key pair and set TRELLIS_FLAG_SEALED_SENDER.  The source field in
 * the wire header is zeroed so routing hops cannot correlate sender identity.
 *
 * sealed_prefix_out must be freed by the caller with trellis_buf_free().
 * The sealed prefix must be prepended to msg->payload before serialization.
 */
trellis_err_t trellis_message_seal_sender(trellis_message_t *msg,
                                           const trellis_fingerprint_t *sender_fp,
                                           const trellis_identity_public_t *recipient_pub,
                                           trellis_buf_t *sealed_prefix_out);

/*
 * Unseal: decrypt the sealed sender prefix from the beginning of the payload.
 * On success, *sender_fp_out is set to the recovered sender fingerprint.
 * The msg->payload pointer is advanced past the prefix (the remaining
 * bytes are the original plaintext payload).
 */
trellis_err_t trellis_message_unseal_sender(trellis_message_t *msg,
                                             const trellis_kem_keypair_t *recipient_kp,
                                             trellis_fingerprint_t *sender_fp_out);

typedef enum trellis_garden_type {
    TRELLIS_GARDEN_PUBLIC     = 0,
    TRELLIS_GARDEN_PRIVATE    = 1,
    TRELLIS_GARDEN_EPHEMERAL  = 2,   /* "Wildflowers" */
    TRELLIS_GARDEN_PERSISTENT = 3,   /* "Perennials" */
} trellis_garden_type_t;

typedef struct trellis_garden trellis_garden_t;

trellis_garden_t *trellis_garden_create(const char *name,
                                         trellis_garden_type_t type,
                                         const trellis_identity_t *owner);
void              trellis_garden_free(trellis_garden_t *garden);

trellis_err_t trellis_garden_subscribe(trellis_garden_t *garden,
                                        const trellis_fingerprint_t *member);
trellis_err_t trellis_garden_unsubscribe(trellis_garden_t *garden,
                                          const trellis_fingerprint_t *member);
bool          trellis_garden_has_member(const trellis_garden_t *garden,
                                        const trellis_fingerprint_t *member);
trellis_err_t trellis_garden_post_message(trellis_garden_t *garden,
                                           const trellis_fingerprint_t *author,
                                           const uint8_t *data, size_t len);
trellis_err_t trellis_garden_set_permission(trellis_garden_t *garden,
                                             const trellis_fingerprint_t *target,
                                             const char *permission);
trellis_err_t trellis_garden_revoke(trellis_garden_t *garden,
                                     const trellis_fingerprint_t *target);
trellis_err_t trellis_garden_set_metadata(trellis_garden_t *garden,
                                           const char *key, const char *value);
const char   *trellis_garden_name(const trellis_garden_t *garden);
trellis_garden_type_t trellis_garden_type(const trellis_garden_t *garden);
size_t        trellis_garden_member_count(const trellis_garden_t *garden);
trellis_err_t trellis_garden_epoch_key(const trellis_garden_t *garden,
                                        uint8_t *key_out);

/*
 * Issue an anonymous membership credential (Pedersen commitment + owner sig).
 * cred_out: 96 bytes (32 commitment C + 64 Ed25519 sig)
 * r_scalar_out: 32 bytes (secret blinding scalar — must be stored securely)
 */
trellis_err_t trellis_garden_issue_credential(
        const trellis_garden_t *garden,
        const trellis_identity_t *owner_id,
        const trellis_fingerprint_t *member_fp,
        uint8_t *cred_out,
        uint8_t *r_scalar_out);

/*
 * Generate an anonymous membership proof for posting without revealing identity.
 * proof_out: 128 bytes (C' || c || z_r || z_fp)
 */
trellis_err_t trellis_garden_prove_membership(
        const uint8_t *cred,
        const uint8_t *r_scalar,
        const trellis_fingerprint_t *member_fp,
        const trellis_garden_t *garden,
        const uint8_t nonce[32],
        uint8_t *proof_out);

/*
 * Verify an anonymous membership proof.
 * Returns TRELLIS_OK if the proof is valid and was signed by owner.
 */
trellis_err_t trellis_garden_verify_membership_proof(
        const trellis_garden_t *garden,
        const uint8_t *owner_ed25519_pk,
        const uint8_t *cred,
        const uint8_t *proof,
        const uint8_t nonce[32]);

typedef struct trellis_capabilities {
    const char **transports;
    size_t       transports_count;
    const char **morph_modes;
    size_t       morph_modes_count;
    const char **media;
    size_t       media_count;
    const char **features;
    size_t       features_count;
    const char **extensions;
    size_t       extensions_count;
} trellis_capabilities_t;

trellis_capabilities_t trellis_capabilities_default(void);
trellis_err_t trellis_capabilities_negotiate(const trellis_capabilities_t *local,
                                              const trellis_capabilities_t *remote,
                                              trellis_capabilities_t *result);
void          trellis_capabilities_free(trellis_capabilities_t *caps);

/* Serialise caps into a flat byte buffer suitable for TRELLIS_MSG_CAPABILITIES.
 * Format: [4 n_sections] for each section: [1 section_id][4 count]
 *         for each string: [2 len][bytes].
 * Sections: 0=transports 1=morph_modes 2=media 3=features 4=extensions */
trellis_err_t trellis_capabilities_serialize(const trellis_capabilities_t *caps,
                                              trellis_buf_t *out);
trellis_err_t trellis_capabilities_deserialize(const uint8_t *data, size_t len,
                                               trellis_capabilities_t *out);

// Append a feature string to caps->features if not already present.
trellis_err_t trellis_capabilities_add_feature(trellis_capabilities_t *caps,
                                                const char *feature);

// Convenience: return true if 'feature' appears in caps->features.
bool trellis_capabilities_has_feature(const trellis_capabilities_t *caps,
                                       const char *feature);
// Return the first morph mode string from the negotiated caps, or NULL.
const char *trellis_capabilities_preferred_morph(const trellis_capabilities_t *caps);
// Return true if 'transport' appears in caps->transports (e.g. "tcp","ws")
bool trellis_capabilities_has_transport(const trellis_capabilities_t *caps,
                                        const char *transport);

/*
 * A WebRTC media session managed over Bloom signaling channels.
 *
 * SDP and ICE candidates are exchanged via encrypted Bloom messages.
 * Media tracks (audio/video) use the actual WebRTC DTLS-SRTP path, which
 * may bypass the Bloom morph engine (TRELLIS_FLAG_MORPH_BYPASS) for latency.
 *
 * Standard media capability strings for trellis_capabilities_t.media:
 *   "video/h264"     - H.264 video
 *   "video/vp8"      - VP8 video
 *   "video/vp9"      - VP9 video
 *   "audio/opus"     - Opus audio
 *   "screen/h264"    - Screen sharing via H.264
 *   "morph-bypass"   - Peer supports per-stream morph bypass
 */
typedef enum trellis_webrtc_media_type {
    TRELLIS_MEDIA_AUDIO       = 0,
    TRELLIS_MEDIA_VIDEO       = 1,
    TRELLIS_MEDIA_SCREEN      = 2,
    TRELLIS_MEDIA_DATA        = 3,
} trellis_webrtc_media_type_t;

typedef struct trellis_webrtc_session trellis_webrtc_session_t;

typedef void (*trellis_webrtc_offer_cb)(const char *sdp, void *ctx);
typedef void (*trellis_webrtc_ice_cb)(const char *candidate,
                                       const char *sdp_mid,
                                       int sdp_mline_index, void *ctx);
typedef void (*trellis_webrtc_hangup_cb)(void *ctx);

trellis_webrtc_session_t *trellis_webrtc_session_new(
        const trellis_fingerprint_t *remote_fp,
        bool is_offerer);
void trellis_webrtc_session_free(trellis_webrtc_session_t *s);

// Set callbacks for received signaling events.
void trellis_webrtc_session_on_offer(trellis_webrtc_session_t *s,
                                      trellis_webrtc_offer_cb cb, void *ctx);
void trellis_webrtc_session_on_answer(trellis_webrtc_session_t *s,
                                       trellis_webrtc_offer_cb cb, void *ctx);
void trellis_webrtc_session_on_ice(trellis_webrtc_session_t *s,
                                    trellis_webrtc_ice_cb cb, void *ctx);
void trellis_webrtc_session_on_hangup(trellis_webrtc_session_t *s,
                                       trellis_webrtc_hangup_cb cb, void *ctx);

// Dispatch an incoming Bloom signaling message to the appropriate callback.
trellis_err_t trellis_webrtc_session_dispatch(trellis_webrtc_session_t *s,
                                               const trellis_message_t *msg);

// Build outbound signaling Bloom messages.
trellis_err_t trellis_webrtc_build_offer(const trellis_fingerprint_t *src,
                                          const trellis_fingerprint_t *dst,
                                          const char *sdp,
                                          trellis_webrtc_media_type_t types,
                                          bool morph_bypass,
                                          trellis_message_t *msg_out);

trellis_err_t trellis_webrtc_build_answer(const trellis_fingerprint_t *src,
                                           const trellis_fingerprint_t *dst,
                                           const char *sdp,
                                           trellis_message_t *msg_out);

trellis_err_t trellis_webrtc_build_ice(const trellis_fingerprint_t *src,
                                        const trellis_fingerprint_t *dst,
                                        const char *candidate,
                                        const char *sdp_mid,
                                        int sdp_mline_index,
                                        trellis_message_t *msg_out);

trellis_err_t trellis_webrtc_build_hangup(const trellis_fingerprint_t *src,
                                           const trellis_fingerprint_t *dst,
                                           trellis_message_t *msg_out);

#define TRELLIS_EXT_MIN_TYPE 0x8000

typedef void (*trellis_ext_handler_fn)(const trellis_message_t *msg, void *ctx);

typedef struct trellis_extensions trellis_extensions_t;

trellis_extensions_t *trellis_extensions_new(void);
void                  trellis_extensions_free(trellis_extensions_t *ext);

trellis_err_t trellis_extensions_register(trellis_extensions_t *ext,
                                           uint16_t type,
                                           const char *name,
                                           trellis_ext_handler_fn handler,
                                           void *ctx);
trellis_err_t trellis_extensions_dispatch(trellis_extensions_t *ext,
                                           const trellis_message_t *msg);

#ifdef __cplusplus
}
#endif

#endif /* TRELLIS_PROTOCOL_H */
