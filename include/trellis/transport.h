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

#ifndef TRELLIS_TRANSPORT_H
#define TRELLIS_TRANSPORT_H

#include "types.h"
#include "error.h"

#ifdef __cplusplus
extern "C" {
#endif

#define TRELLIS_MAX_FRAME_LEN   (16u * 1024u * 1024u)  /* 16 MiB */
#define TRELLIS_FRAME_TAG_LEN   4

typedef struct trellis_conn trellis_conn_t;

typedef void (*trellis_conn_recv_cb)(trellis_conn_t *conn,
                                     const uint8_t *data, size_t len,
                                     trellis_err_t err, void *ctx);
typedef void (*trellis_conn_send_cb)(trellis_err_t err, void *ctx);
typedef void (*trellis_conn_close_cb)(trellis_conn_t *conn, void *ctx);

trellis_err_t   trellis_conn_send(trellis_conn_t *conn,
                                  const uint8_t *data, size_t len,
                                  trellis_conn_send_cb cb, void *ctx);
trellis_err_t   trellis_conn_recv_start(trellis_conn_t *conn,
                                        trellis_conn_recv_cb cb, void *ctx);
void            trellis_conn_recv_stop(trellis_conn_t *conn);
void            trellis_conn_close(trellis_conn_t *conn,
                                   trellis_conn_close_cb cb, void *ctx);
void            trellis_conn_close_graceful(trellis_conn_t *conn,
                                             trellis_conn_close_cb cb,
                                             void *ctx);
const char     *trellis_conn_remote_addr(const trellis_conn_t *conn);
const char     *trellis_conn_local_addr(const trellis_conn_t *conn);
const char     *trellis_conn_transport(const trellis_conn_t *conn);

void            trellis_conn_set_frame_tag(trellis_conn_t *conn,
                                           const uint8_t *tag);
void            trellis_conn_derive_frame_tag(trellis_conn_t *conn,
                                              const uint8_t *session_key,
                                              size_t key_len);

typedef struct trellis_morph trellis_morph_t;
typedef struct trellis_camouflage trellis_camouflage_t;

void            trellis_conn_set_wire_morph(trellis_conn_t *conn,
                                            trellis_morph_t *morph);
void            trellis_conn_set_wire_camo(trellis_conn_t *conn,
                                           trellis_camouflage_t *camo);

typedef struct trellis_transport trellis_transport_t;

typedef void (*trellis_connect_cb)(trellis_conn_t *conn,
                                   trellis_err_t err, void *ctx);
typedef void (*trellis_accept_cb)(trellis_conn_t *conn, void *ctx);

trellis_err_t trellis_transport_connect(trellis_transport_t *t,
                                        const char *addr,
                                        trellis_connect_cb cb, void *ctx);
trellis_err_t trellis_transport_listen(trellis_transport_t *t,
                                       const char *addr,
                                       trellis_accept_cb cb, void *ctx);
trellis_err_t trellis_transport_stop(trellis_transport_t *t);
const char   *trellis_transport_protocol(const trellis_transport_t *t);
int            trellis_transport_bound_port(trellis_transport_t *t);

typedef enum trellis_transport_mode {
    TRELLIS_TRANSPORT_RAW   = 0,
    TRELLIS_TRANSPORT_TLS   = 1,
    TRELLIS_TRANSPORT_PT    = 2,
} trellis_transport_mode_t;

typedef struct uv_loop_s uv_loop_t;

trellis_transport_t *trellis_transport_tcp_new(uv_loop_t *loop);
trellis_transport_t *trellis_transport_udp_new(uv_loop_t *loop);
trellis_transport_t *trellis_transport_ws_new(uv_loop_t *loop);
void                 trellis_transport_free(trellis_transport_t *t);

typedef struct trellis_tls_config {
    const char *sni;
    const char *cert_pem;
    const char *key_pem;
    /* PEM-encoded CA certificate(s) to trust when verify_peer is true.
     * If NULL, the system-default CA bundle (loaded via mbedTLS defaults) is
     * used instead.  Set this to pin a specific CA for your deployment. */
    const char *ca_pem;

    const char **sni_pool;
    size_t       sni_pool_count;

    /* Domain fronting: when front_host is set, use it as the TLS SNI.
     * After the TLS handshake completes, send an HTTP CONNECT to
     * relay_target (e.g. "relay.example.com:443") so the CDN tunnels
     * the connection to the actual Trellis relay. */
    const char *front_host;
    const char *relay_target;  /* host:port for the HTTP CONNECT target */

    /* Certificate verification mode for outgoing connections.
     *   true  (default) — verify peer certificate against system CA store.
     *   false           — disable verification (required for domain fronting
     *                     where the CDN cert does not match relay_target). */
    bool verify_peer;

    /* Encrypted Client Hello (ECH / RFC 9001):
     *
     * When ech_public_name is set, it becomes the outer TLS SNI.  The CDN
     * sees only the public_name; a passive observer cannot distinguish this
     * from any other connection to that CDN.  An ECH ClientHello extension is
     * also injected (GREASE when no ECHConfigList is available, full HPKE when
     * ech_config_list is provided) so that the traffic pattern matches real
     * ECH deployments.
     *
     * ech_inner_sni  – the real relay hostname, encrypted in the ECH payload.
     * ech_public_name – outer SNI / public name (CDN front domain).
     * ech_config_list / ech_config_list_len – ECHConfigList bytes from the
     *   server's DNS HTTPS RR.  When provided, real HPKE encryption is used
     *   (DHKEM-X25519-HKDF-SHA256-ChaCha20Poly1305).  When absent, a properly
     *   formatted GREASE extension is sent, giving plausible deniability.
     *
     * verify_peer is automatically set to false when ECH is active (the TLS
     * connection terminates at the CDN, whose cert does not match the relay).
     */
    const char    *ech_public_name;      /* outer SNI (CDN / public_name) */
    const char    *ech_inner_sni;        /* real relay SNI (hidden from DPI) */
    const uint8_t *ech_config_list;      /* ECHConfigList from DNS HTTPS RR */
    size_t         ech_config_list_len;
} trellis_tls_config_t;

trellis_transport_t *trellis_transport_tls_new(uv_loop_t *loop,
                                               const trellis_tls_config_t *cfg);

/*
 * QUIC v1 (RFC 9000) transport adapter.
 *
 * Provides real QUIC multiplexing over UDP — multiple Trellis logical
 * connections share a single QUIC connection (one UDP socket), each carried
 * on its own QUIC bidirectional stream.  The TLS 1.3 handshake is performed
 * within QUIC using ngtcp2 + mbedTLS.
 *
 * 0-RTT session resumption (RFC 9001 §4.6):
 *   When zero_rtt=true and session_cache_path is set, session tickets are
 *   persisted to disk and replayed on reconnect to skip the full handshake
 *   round-trip.  Early-data is encrypted with the 0-RTT keys.
 *
 * MASQUE HTTP/3 CONNECT-UDP (RFC 9298):
 *   When masque_proxy_addr is set, the client sends an HTTP/3
 *   CONNECT-UDP request through the QUIC connection to ask the proxy to
 *   forward UDP datagrams to the real Trellis relay address.  This
 *   provides an additional indirection hop over an HTTP/3 proxy.
 */
typedef struct trellis_quic_config {
    const char *sni;                 /* TLS SNI for QUIC handshake */
    const char *ca_pem;              /* PEM CA certificate(s) for peer verify */
    const char *cert_pem;            /* Server certificate PEM (server mode) */
    const char *key_pem;             /* Server private key PEM (server mode) */

    bool        zero_rtt;            /* Enable 0-RTT session resumption */
    const char *session_cache_path;  /* Path to persist 0-RTT session tickets */

    /* MASQUE HTTP/3 UDP proxy (RFC 9298).
     * When non-NULL, the QUIC connection is made to masque_proxy_addr and an
     * HTTP/3 CONNECT-UDP capsule is sent to proxy datagrams to the relay. */
    const char *masque_proxy_addr;

    bool        verify_peer;         /* Verify peer certificate (default true) */
} trellis_quic_config_t;

trellis_transport_t *trellis_transport_quic_new(uv_loop_t *loop,
                                                const trellis_quic_config_t *cfg);

/*
 * Steganographic transport: embeds Bloom frames into the low-order bits of
 * RTP G.711/G.722/Opus audio PCM samples inside a functional NeverCast
 * WebRTC video call.  To passive DPI the stream is indistinguishable from
 * normal VoIP/video conferencing traffic.
 *
 * Build guard: TRELLIS_WITH_WEBRTC_STEGO
 * Dependencies: libdatachannel (WebRTC), libopus (audio codec), libuv
 *
 * Steganographic capacity (audio only):
 *   G.711 µ-law: 8 kHz × 8 bit/sample × 1 LSB = 1 kbit/s carrier
 *   Opus (48 kHz stereo, 20 ms frames): ~3–6 kbit/s carrier per channel
 *   With 2 LSBs: 2–12 kbit/s net depending on codec and frame rate
 *
 * A lightweight FEC and interleaving scheme spreads Bloom frames across
 * multiple consecutive RTP packets so single-packet loss does not corrupt
 * the embedded data.
 */
typedef struct trellis_webrtc_stego_config {
    // WebRTC signaling server address for NeverCast call setup (WebSocket)
    const char *signaling_url;
    // Identity string presented in the SDP offer.
    const char *display_name;

    // LSBs of each PCM sample used for steganography (1 or 2; default 1)
    int         lsb_depth;

    // Opus bitrate for the "cover" call (kbit/s; 0 = auto ~64 kbit/s)
    int         cover_bitrate_kbps;

    // FEC: redundancy ratio 0–100 (percent of extra bytes; default 30)
    int         fec_redundancy_pct;

    // Optional DTLS certificate override; NULL = self-signed.
    const char *dtls_cert_pem;
    const char *dtls_key_pem;
} trellis_webrtc_stego_config_t;

trellis_transport_t *trellis_transport_webrtc_stego_new(
    uv_loop_t *loop,
    const trellis_webrtc_stego_config_t *cfg);

/*
 * Store-and-forward Delay-Tolerant Networking (DTN) transports for use in
 * internet-shutdown and air-gapped scenarios.
 *
 * LoRa transport (src/transport/lora.c):
 *   Communicates over a LoRa radio modem attached via serial port (e.g.
 *   SX1276/SX1278 connected via a USB-to-serial adapter, or a Meshtastic
 *   node connected over USB UART or BLE).  Frames are chunked to fit within
 *   LoRa maximum payload (255 bytes for LoRaWAN / up to 240 bytes for raw
 *   LoRa), numbered for reassembly, and stored in a persistent DTN bundle
 *   store for later forwarding when a relay comes in range.
 *
 * BLE transport (src/transport/ble.c):
 *   Uses the BlueZ D-Bus API (Linux) or platform BLE HCI to advertise and
 *   discover nearby Bloom peers, exchange frames via BLE GATT notifications
 *   / indications.  Frame size is capped at BLE MTU (up to 512 bytes with
 *   DLE extensions).  DTN bundle store is shared with the LoRa adapter.
 *
 * DTN bundle store (RFC 9171 BPv7 -inspired):
 *   Bundles are stored on-disk at `dtn_store_path` as length-prefixed blobs
 *   with a destination fingerprint, creation timestamp, TTL, and retry count.
 *   The store is periodically scanned and bundles forwarded when a relay
 *   node with the matching fingerprint (or any relay for multi-hop) connects.
 */
typedef struct trellis_dtn_config {
    // Shared DTN bundle store path (can be shared across LoRa + BLE)
    const char *dtn_store_path;
    // Bundle TTL in seconds (default 86400 = 24 h)
    uint32_t    bundle_ttl_s;
    // Max bundle store size in bytes (default 64 MiB; 0 = unlimited)
    uint64_t    max_store_bytes;
    // Max number of stored bundles (default 1024)
    size_t      max_bundles;
} trellis_dtn_config_t;

typedef struct trellis_lora_config {
    trellis_dtn_config_t dtn;        /* DTN store shared config */
    const char *serial_port;         /* e.g. "/dev/ttyUSB0" */
    uint32_t    baud_rate;           /* e.g. 115200 */
    uint32_t    frequency_hz;        /* e.g. 915000000 (915 MHz) */
    uint8_t     spreading_factor;    /* 7–12 (default 9) */
    uint8_t     bandwidth_khz;       /* 125, 250, or 500 */
    uint8_t     coding_rate;         /* 5–8 (4/5 .. 4/8; default 5) */
    int8_t      tx_power_dbm;        /* transmit power (default 14 dBm) */
    bool        meshtastic_compat;   /* wrap in Meshtastic packet format */
} trellis_lora_config_t;

typedef struct trellis_ble_config {
    trellis_dtn_config_t dtn;
    const char *hci_device;          /* e.g. "hci0" (NULL = auto-detect) */
    const char *service_uuid;        /* GATT service UUID (NULL = default) */
    uint16_t    mtu;                 /* BLE MTU (default 247 for DLE) */
    bool        mesh_relay;          /* relay frames to other BLE nodes */
} trellis_ble_config_t;

trellis_transport_t *trellis_transport_lora_new(uv_loop_t *loop,
                                                const trellis_lora_config_t *cfg);
trellis_transport_t *trellis_transport_ble_new(uv_loop_t *loop,
                                               const trellis_ble_config_t *cfg);

/* transport_name: PT protocol name, e.g. "obfs4", "snowflake", "webtunnel".
 * Passed to the binary via TOR_PT_CLIENT_TRANSPORTS / SERVER_TRANSPORTS.
 * NULL defaults to "obfs4". */
trellis_transport_t *trellis_transport_pt_new(uv_loop_t *loop,
                                              const char *pt_binary,
                                              const char *transport_name,
                                              const char *pt_args);

typedef struct trellis_mux trellis_mux_t;
typedef struct trellis_mux_stream trellis_mux_stream_t;

/* key/key_len are optional: pass NULL/0 for unencrypted (plain stream IDs).
 * When key is provided, mux encryption is activated immediately so the
 * stream ID header never appears in cleartext on the wire. */
trellis_mux_t        *trellis_mux_new(trellis_conn_t *conn,
                                       const uint8_t *key, size_t key_len);
void                  trellis_mux_free(trellis_mux_t *mux);
trellis_mux_stream_t *trellis_mux_open(trellis_mux_t *mux, uint16_t stream_id);
trellis_err_t         trellis_mux_stream_send(trellis_mux_stream_t *stream,
                                              const uint8_t *data, size_t len,
                                              trellis_conn_send_cb cb, void *ctx);
void                  trellis_mux_stream_close(trellis_mux_stream_t *stream);

/* Per-stream receive registration and flow control.
 *
 * trellis_mux_stream_recv_start() — register a callback invoked each time
 *   data arrives on this specific stream. May be called at any time after
 *   trellis_mux_open(); replaces any previously registered callback.
 *
 * trellis_mux_stream_recv_stop() — deregister the receive callback. Data
 *   arriving while no callback is registered is silently discarded (use
 *   trellis_mux_stream_pause() to buffer instead).
 *
 * trellis_mux_stream_pause() — buffer incoming data without invoking the
 *   callback. Enables back-pressure when the consumer is slow.
 *
 * trellis_mux_stream_resume() — drain buffered data through the callback,
 *   then return to immediate delivery mode.
 */
trellis_err_t         trellis_mux_stream_recv_start(trellis_mux_stream_t *stream,
                                                    trellis_conn_recv_cb cb,
                                                    void *ctx);
void                  trellis_mux_stream_recv_stop(trellis_mux_stream_t *stream);
trellis_err_t         trellis_mux_stream_pause(trellis_mux_stream_t *stream);
trellis_err_t         trellis_mux_stream_resume(trellis_mux_stream_t *stream);

void                  trellis_mux_set_encrypt_key(trellis_mux_t *mux,
                                                  const uint8_t *key,
                                                  size_t key_len);

#ifdef __cplusplus
}
#endif

#endif /* TRELLIS_TRANSPORT_H */
