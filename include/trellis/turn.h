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

#ifndef TRELLIS_TURN_H
#define TRELLIS_TURN_H

#include "types.h"
#include "error.h"

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ═══════════════════════════════════════════════════════════════════════════
 *  STUN / TURN protocol implementation (RFC 5389 / RFC 5766 subset)
 *
 *  Implements the minimal message set that Chrome and Firefox actually use
 *  for TURN relay:
 *    STUN:  Binding Request/Response
 *    TURN:  Allocate, Refresh, CreatePermission, ChannelBind,
 *           Send/Data, ChannelData
 *
 *  Authentication uses the coturn-compatible "use-auth-secret" scheme:
 *    username = "timestamp:arbitrary"
 *    password = Base64(HMAC-SHA1(secret, username))
 * ═══════════════════════════════════════════════════════════════════════════ */
#define STUN_HEADER_LEN          20
#define STUN_MAGIC_COOKIE        0x2112A442u
#define STUN_TXN_ID_LEN         12
#define STUN_FINGERPRINT_XOR     0x5354554Eu  /* "STUN" */

// STUN message class bits (in the 14-bit type field)
#define STUN_CLASS_REQUEST       0x0000
#define STUN_CLASS_INDICATION    0x0010
#define STUN_CLASS_SUCCESS       0x0100
#define STUN_CLASS_ERROR         0x0110

// STUN methods.
#define STUN_METHOD_BINDING      0x0001

// TURN methods (RFC 5766)
#define TURN_METHOD_ALLOCATE     0x0003
#define TURN_METHOD_REFRESH      0x0004
#define TURN_METHOD_SEND         0x0006
#define TURN_METHOD_DATA         0x0007
#define TURN_METHOD_CREATE_PERM  0x0008
#define TURN_METHOD_CHANNEL_BIND 0x0009

// STUN attributes.
#define STUN_ATTR_MAPPED_ADDRESS         0x0001
#define STUN_ATTR_USERNAME               0x0006
#define STUN_ATTR_MESSAGE_INTEGRITY      0x0008
#define STUN_ATTR_ERROR_CODE             0x0009
#define STUN_ATTR_UNKNOWN_ATTRIBUTES     0x000A
#define STUN_ATTR_REALM                  0x0014
#define STUN_ATTR_NONCE                  0x0015
#define STUN_ATTR_XOR_MAPPED_ADDRESS     0x0020
#define STUN_ATTR_SOFTWARE               0x8022
#define STUN_ATTR_FINGERPRINT            0x8028

// TURN attributes.
#define TURN_ATTR_CHANNEL_NUMBER         0x000C
#define TURN_ATTR_LIFETIME               0x000D
#define TURN_ATTR_XOR_PEER_ADDRESS       0x0012
#define TURN_ATTR_DATA                   0x0013
#define TURN_ATTR_XOR_RELAYED_ADDRESS    0x0016
#define TURN_ATTR_REQUESTED_TRANSPORT    0x0019
#define TURN_ATTR_DONT_FRAGMENT          0x001A

// STUN error codes.
#define STUN_ERR_TRY_ALTERNATE       300
#define STUN_ERR_BAD_REQUEST         400
#define STUN_ERR_UNAUTHORIZED        401
#define STUN_ERR_FORBIDDEN           403
#define STUN_ERR_UNKNOWN_ATTRIBUTE   420
#define STUN_ERR_ALLOCATION_MISMATCH 437
#define STUN_ERR_STALE_NONCE         438
#define STUN_ERR_WRONG_CREDENTIALS   441
#define STUN_ERR_UNSUPPORTED_PROTO   442
#define STUN_ERR_ALLOCATION_QUOTA    486
#define STUN_ERR_INSUFFICIENT_CAP    508
#define STUN_ERR_SERVER_ERROR        500

// ChannelData range (RFC 5766 §11.4)
#define TURN_CHANNEL_MIN   0x4000
#define TURN_CHANNEL_MAX   0x7FFF
#define TURN_CHANNEL_HEADER_LEN 4

typedef struct stun_addr {
    uint8_t  family;       /* 0x01 = IPv4, 0x02 = IPv6 */
    uint16_t port;
    union {
        uint8_t v4[4];
        uint8_t v6[16];
    } ip;
} stun_addr_t;

typedef struct stun_msg {
    uint16_t type;                  /* full 14-bit type (method + class) */
    uint16_t length;                /* payload length after header */
    uint8_t  txn_id[STUN_TXN_ID_LEN];

    // Parsed attributes (populated by stun_parse)
    const uint8_t *username;
    uint16_t       username_len;
    const uint8_t *realm;
    uint16_t       realm_len;
    const uint8_t *nonce;
    uint16_t       nonce_len;

    // Integrity / fingerprint.
    bool     has_integrity;
    uint16_t integrity_offset;      /* byte offset of MESSAGE-INTEGRITY attr */
    bool     has_fingerprint;

    // Error code (present in error responses)
    bool     has_error;
    uint16_t error_code;

    // TURN-specific
    uint32_t lifetime;              /* seconds; 0 if not present */
    bool     has_lifetime;
    uint16_t channel_number;        /* 0 if not present */
    bool     has_channel_number;
    uint8_t  requested_transport;   /* 17=UDP, 6=TCP */
    bool     has_requested_transport;

    stun_addr_t xor_peer_addr;
    bool        has_xor_peer_addr;
    stun_addr_t xor_relayed_addr;
    bool        has_xor_relayed_addr;
    stun_addr_t xor_mapped_addr;
    bool        has_xor_mapped_addr;

    // DATA attribute (Send/Data indications)
    const uint8_t *data_attr;
    uint16_t       data_attr_len;

    // Raw buffer reference for integrity checks.
    const uint8_t *raw;
    size_t         raw_len;
} stun_msg_t;

// Returns true if buf starts with a valid STUN header (not ChannelData).
bool stun_is_stun(const uint8_t *buf, size_t len);

// Returns true if buf starts with a ChannelData header (0x40-0x7F first byte).
bool stun_is_channel_data(const uint8_t *buf, size_t len);

// Parse a STUN message. Returns TRELLIS_OK on success.
trellis_err_t stun_parse(const uint8_t *buf, size_t len, stun_msg_t *out);

/* Verify MESSAGE-INTEGRITY using HMAC-SHA1 with the given key.
 * The key for long-term credentials is: MD5(username:realm:password). */
bool stun_verify_integrity(const stun_msg_t *msg, const uint8_t *key, size_t key_len);

/* Verify MESSAGE-INTEGRITY using the coturn "use-auth-secret" scheme directly.
 * Derives the password from HMAC-SHA1(secret, username), then computes
 * the long-term credential key as MD5(username:realm:password). */
bool stun_verify_auth_secret(const stun_msg_t *msg,
                             const char *secret, const char *realm);

/* Build a STUN message into buf. Returns bytes written or 0 on error.
 * key/key_len: if non-NULL, MESSAGE-INTEGRITY and FINGERPRINT are appended. */
size_t stun_build_response(uint16_t type, const uint8_t txn_id[STUN_TXN_ID_LEN],
                           const uint8_t *attrs, size_t attrs_len,
                           const uint8_t *key, size_t key_len,
                           uint8_t *buf, size_t buf_cap);

size_t stun_attr_xor_mapped_address(const stun_addr_t *addr,
                                    const uint8_t txn_id[STUN_TXN_ID_LEN],
                                    uint8_t *buf, size_t cap);
size_t stun_attr_xor_relayed_address(const stun_addr_t *addr,
                                     const uint8_t txn_id[STUN_TXN_ID_LEN],
                                     uint8_t *buf, size_t cap);
size_t stun_attr_xor_peer_address(const stun_addr_t *addr,
                                  const uint8_t txn_id[STUN_TXN_ID_LEN],
                                  uint8_t *buf, size_t cap);
size_t stun_attr_error_code(uint16_t code, const char *reason,
                            uint8_t *buf, size_t cap);
size_t stun_attr_lifetime(uint32_t seconds, uint8_t *buf, size_t cap);
size_t stun_attr_software(const char *name, uint8_t *buf, size_t cap);
size_t stun_attr_nonce(const uint8_t *nonce, size_t nonce_len,
                       uint8_t *buf, size_t cap);
size_t stun_attr_realm(const char *realm, uint8_t *buf, size_t cap);

/* ═══════════════════════════════════════════════════════════════════════════
 *  TURN Allocation Manager
 * ═══════════════════════════════════════════════════════════════════════════ */
#define TURN_MAX_PERMISSIONS     32
#define TURN_MAX_CHANNELS        16
#define TURN_MAX_ALLOCATIONS    256
#define TURN_DEFAULT_LIFETIME   600    /* seconds */
#define TURN_MAX_LIFETIME       3600   /* seconds */
#define TURN_PERMISSION_LIFETIME 300   /* seconds */
#define TURN_CHANNEL_LIFETIME   600    /* seconds */
#define TURN_NONCE_LEN          24

typedef struct turn_permission {
    stun_addr_t peer_addr;
    uint64_t    expires_at_ms;
    bool        active;
} turn_permission_t;

typedef struct turn_channel {
    uint16_t    number;
    stun_addr_t peer_addr;
    uint64_t    expires_at_ms;
    bool        active;
} turn_channel_t;

typedef struct turn_allocation {
    bool        active;
    uint64_t    id;                 /* unique allocation ID */

    // Client 5-tuple.
    stun_addr_t client_addr;
    uint8_t     client_transport;   /* 6=TCP, 17=UDP */

    // Allocated relay address (virtual, mapped to trellis circuits)
    stun_addr_t relay_addr;

    // Authentication
    char        username[256];
    uint8_t     auth_key[16];       /* MD5(username:realm:password) */

    // Nonce for replay protection.
    uint8_t     nonce[TURN_NONCE_LEN];

    // Lifetime
    uint64_t    created_at_ms;
    uint64_t    expires_at_ms;

    // Permissions and channels.
    turn_permission_t permissions[TURN_MAX_PERMISSIONS];
    turn_channel_t    channels[TURN_MAX_CHANNELS];

    // Rate limiting / flow control.
    uint64_t    bytes_relayed;
    uint64_t    bytes_window_start;
    uint64_t    bytes_this_window;
    uint32_t    max_bps;            /* 0 = unlimited */
} turn_allocation_t;

typedef void (*turn_send_cb)(const uint8_t *data, size_t len,
                             const stun_addr_t *dst, void *ctx);

typedef void (*turn_relay_cb)(uint64_t alloc_id,
                              const stun_addr_t *peer_addr,
                              const uint8_t *data, size_t len,
                              void *ctx);

typedef void (*turn_log_cb)(int level, const char *msg, void *ctx);

typedef struct turn_server {
    turn_allocation_t allocations[TURN_MAX_ALLOCATIONS];
    size_t            alloc_count;
    uint64_t          next_alloc_id;

    // Configuration
    char              realm[256];
    char             *secret;            /* HMAC shared secret (heap-allocated) */
    size_t            secret_len;
    uint32_t          default_lifetime;
    uint32_t          max_lifetime;
    uint32_t          max_allocations;
    uint32_t          max_bps;           /* per-allocation bandwidth */
    stun_addr_t       external_addr;     /* advertised relay IP */
    uint16_t          relay_port_min;
    uint16_t          relay_port_max;
    uint16_t          next_relay_port;

    // Nonce generation state.
    uint8_t           nonce_key[32];
    uint64_t          nonce_epoch;

    // Callbacks
    turn_send_cb      send_cb;           /* send STUN response to client */
    void             *send_ctx;
    turn_relay_cb     relay_cb;          /* forward media to trellis mesh */
    void             *relay_ctx;
    turn_log_cb       log_cb;            /* optional structured logging */
    void             *log_ctx;
} turn_server_t;

trellis_err_t turn_server_init(turn_server_t *srv,
                               const char *realm,
                               const char *secret,
                               const stun_addr_t *external_addr);
void          turn_server_destroy(turn_server_t *srv);

/* Process an incoming STUN/TURN message from a client.
 * The server will call send_cb to deliver responses and relay_cb for data. */
trellis_err_t turn_server_process(turn_server_t *srv,
                                  const uint8_t *data, size_t len,
                                  const stun_addr_t *client_addr,
                                  uint8_t client_transport);

/* Deliver data arriving from the trellis mesh back to a TURN client.
 * Wraps in ChannelData (if bound) or Data indication. */
trellis_err_t turn_server_deliver(turn_server_t *srv,
                                  uint64_t alloc_id,
                                  const stun_addr_t *peer_addr,
                                  const uint8_t *data, size_t len);

// Expire stale allocations, permissions, and channels. Call periodically.
void turn_server_tick(turn_server_t *srv);

// Look up allocation by client address. Returns NULL if not found.
turn_allocation_t *turn_server_find_alloc_by_client(turn_server_t *srv,
                                                     const stun_addr_t *client);

// Look up allocation by relay address. Returns NULL if not found.
turn_allocation_t *turn_server_find_alloc_by_relay(turn_server_t *srv,
                                                    const stun_addr_t *relay);

// Look up allocation by ID. Returns NULL if not found.
turn_allocation_t *turn_server_find_alloc_by_id(turn_server_t *srv,
                                                 uint64_t alloc_id);

// Check if a peer has an active permission in the given allocation.
bool turn_alloc_has_permission(const turn_allocation_t *alloc,
                               const stun_addr_t *peer);

// Find channel number for a peer address, or 0 if unbound.
uint16_t turn_alloc_find_channel(const turn_allocation_t *alloc,
                                 const stun_addr_t *peer);

// Find peer address for a channel number. Returns false if unbound.
bool turn_alloc_find_peer_by_channel(const turn_allocation_t *alloc,
                                     uint16_t channel,
                                     stun_addr_t *peer_out);

#ifdef __cplusplus
}
#endif

#endif /* TRELLIS_TURN_H */
