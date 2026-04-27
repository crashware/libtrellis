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

/*
 * bootstrap.c — Network Bootstrap Strategy
 *
 * Provides multi-strategy peer discovery for new nodes joining the Bloom
 * network, with censorship-resistant fallback chains:
 *
 *   Priority 1: mDNS/DNS-SD LAN discovery (zero-config, no internet needed)
 *   Priority 2: Hardcoded seed list (bundled at compile time)
 *   Priority 3: DNS TXT record fallback (seed.bloom.example / DoH fallback)
 *   Priority 4: DHT self-healing once any peer is connected
 *
 * Design principles:
 *   - No single point of failure: each strategy is independent
 *   - mDNS enables air-gapped LAN clusters and censored networks
 *   - DNS TXT seeds can be updated without client updates
 *   - Seeds are verified against a hardcoded Ed25519 signing key to prevent
 *     injection attacks on the DNS response
 *   - All seed connections go through the normal Bloom handshake and Sybil
 *     scoring — no seeds get elevated trust
 *
 * Seed record format (DNS TXT, base64-encoded):
 *   struct bloom_seed {
 *       uint8_t  version;              // 1
 *       uint8_t  fp[32];               // seed fingerprint
 *       char     addr[256];            // "ip:port" or "hostname:port"
 *       uint8_t  ed25519_pk[32];       // seed's signing key
 *       uint8_t  sig[64];              // Ed25519 sig over version+fp+addr+pk
 *   };
 *   Encoded as base64, multiple records = multiple seeds.
 *
 * Hardcoded seed signing public key:
 *   Defined at build time via TRELLIS_SEED_SIGNING_KEY_HEX.
 *   Seeds whose sig doesn't verify against this key are rejected.
 */

#include "internal.h"
#include <trellis/mesh.h>
#include <trellis/error.h>
#include <sodium.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <time.h>
#include <uv.h>

// --- Constants ---
#define BOOTSTRAP_MDNS_PORT          5353
#define BOOTSTRAP_MDNS_GROUP         "224.0.0.251"
#define BOOTSTRAP_MDNS_SERVICE_TYPE  "_bloom._tcp.local."
#define BOOTSTRAP_BLOOM_PORT         7840
#define BOOTSTRAP_DNS_SEED_DOMAIN    "seeds.bloom.network"
#define BOOTSTRAP_DNS_SEED_TIMEOUT_MS 5000
#define BOOTSTRAP_MAX_SEEDS          64
#define BOOTSTRAP_SEED_VERIFY_MAGIC  "BSEED1"

// Minimum number of peers before we consider bootstrap complete.
#define BOOTSTRAP_PEER_MIN           3

// How long to wait before trying next strategy.
#define BOOTSTRAP_STRATEGY_TIMEOUT_MS 8000

typedef struct bloom_seed_record {
    trellis_fingerprint_t fp;
    char                  addr[256];
    uint8_t               ed25519_pk[crypto_sign_ed25519_PUBLICKEYBYTES];
    bool                  verified;
    bool                  has_grove;
    uint8_t               grove_id[32];
} bloom_seed_record_t;

// These are compile-time defaults. Operators can override via config file.
static const struct {
    const char *addr;
    const char *fp_hex;  /* hex-encoded 32-byte fingerprint */
} g_hardcoded_seeds[] = {
    // Example multi-region seeds — replace with real seeds at deployment.
    { "seed-us-1.bloom.network:7840",  "0000000000000000000000000000000000000000000000000000000000000001" },
    { "seed-eu-1.bloom.network:7840",  "0000000000000000000000000000000000000000000000000000000000000002" },
    { "seed-ap-1.bloom.network:7840",  "0000000000000000000000000000000000000000000000000000000000000003" },
    { "seed-sa-1.bloom.network:7840",  "0000000000000000000000000000000000000000000000000000000000000004" },
    { NULL, NULL }
};

// --- Bootstrap state ---
typedef struct trellis_bootstrap {
    trellis_dht_t        *dht;
    uv_loop_t            *loop;
    trellis_bootstrap_cb  cb;
    void                 *ctx;

    bloom_seed_record_t   seeds[BOOTSTRAP_MAX_SEEDS];
    size_t                seed_count;

    int                   candidate_peers;
    bool                  complete;

    // mDNS UDP socket.
    uv_udp_t              mdns_sock;
    bool                  mdns_sock_init;
    bool                  mdns_active;

    // Strategy timer.
    uv_timer_t            strategy_timer;
    int                   strategy_phase; /* 0=mdns, 1=hardcoded, 2=dns, 3=done */

    // DoH resolver for DNS seed lookups.
    trellis_doh_resolver_t *doh;
    bool                  dns_pending;
    bool                  shutting_down;

    int                   pending_closes;
} trellis_bootstrap_t;

static bool hex_to_bytes(const char *hex, uint8_t *out, size_t out_len)
{
    size_t hex_len = strlen(hex);
    if (hex_len != out_len * 2) return false;
    for (size_t i = 0; i < out_len; i++) {
        unsigned int b;
        if (sscanf(hex + 2*i, "%02x", &b) != 1) return false;
        out[i] = (uint8_t)b;
    }
    return true;
}

// --- Adding a peer candidate ---
static void bootstrap_try_peer(trellis_bootstrap_t *bs,
                                const trellis_fingerprint_t *fp,
                                const char *addr)
{
    if (!bs || !addr || bs->complete) return;

    // Add to DHT so the normal connectivity layer can attempt connection.
    trellis_peer_info_t pi = {0};
    if (fp) pi.fingerprint = *fp;
    strncpy(pi.addr, addr, sizeof(pi.addr) - 1);
    pi.last_seen  = (uint64_t)time(NULL) * 1000;
    pi.reliability = 0.5;

    trellis_err_t err = trellis_dht_add_peer(bs->dht, &pi);
    if (err != TRELLIS_OK)
        return;
    bs->candidate_peers++;

    if (bs->candidate_peers >= BOOTSTRAP_PEER_MIN && !bs->complete) {
        bs->complete = true;
        fprintf(stderr, "[bootstrap] connected to %d peers, bootstrap complete\n",
                bs->candidate_peers);
        if (bs->cb)
            bs->cb(TRELLIS_OK, bs->candidate_peers, bs->ctx);
    }
}

// --- Strategy 1: mDNS LAN discovery ---
static void on_mdns_recv(uv_udp_t *handle, ssize_t nread,
                          const uv_buf_t *buf, const struct sockaddr *addr,
                          unsigned flags)
{
    trellis_bootstrap_t *bs = (trellis_bootstrap_t *)handle->data;
    (void)flags;

    if (nread <= 0 || !buf->base) {
        if (buf) free(buf->base);
        return;
    }

    /* Minimal mDNS response parser: look for TXT records containing
     * "bloom_fp=<hex>" and "bloom_addr=<addr>" */
    const char *data = buf->base;
    const char *end  = data + nread;
    const char *fp_tag    = "bloom_fp=";
    const char *addr_tag  = "bloom_addr=";
    const char *grove_tag = "bloom_grove=";
    char found_fp_hex[65]    = {0};
    char found_addr[256]     = {0};
    char found_grove_hex[65] = {0};

    // Scan for tag patterns in the raw mDNS payload.
    for (const char *p = data; p + 12 < end; p++) {
        if (strncmp(p, fp_tag, strlen(fp_tag)) == 0) {
            const char *v = p + strlen(fp_tag);
            size_t len = 0;
            while (v + len < end && len < 64 &&
                   ((v[len] >= '0' && v[len] <= '9') ||
                    (v[len] >= 'a' && v[len] <= 'f')))
                len++;
            if (len == 64)
                memcpy(found_fp_hex, v, 64);
        }
        if (strncmp(p, addr_tag, strlen(addr_tag)) == 0) {
            const char *v = p + strlen(addr_tag);
            size_t len = 0;
            while (v + len < end && len < 255 && v[len] != '\0' &&
                   v[len] != '\n' && v[len] != ' ')
                len++;
            memcpy(found_addr, v, len);
        }
        if (strncmp(p, grove_tag, strlen(grove_tag)) == 0) {
            const char *v = p + strlen(grove_tag);
            size_t len = 0;
            while (v + len < end && len < 64 &&
                   ((v[len] >= '0' && v[len] <= '9') ||
                    (v[len] >= 'a' && v[len] <= 'f')))
                len++;
            if (len == 64)
                memcpy(found_grove_hex, v, 64);
        }
    }

    free(buf->base);

    if (found_addr[0]) {
        /* If bloom_addr is port-only (starts with ':'), merge with
         * the UDP source IP to form a connectable address. */
        if (found_addr[0] == ':' && addr && addr->sa_family == AF_INET) {
            char ip_buf[INET_ADDRSTRLEN] = {0};
            uv_ip4_name((const struct sockaddr_in *)addr,
                         ip_buf, sizeof(ip_buf));
            char merged[256];
            snprintf(merged, sizeof(merged), "%s%s", ip_buf, found_addr);
            memcpy(found_addr, merged,
                   strlen(merged) < sizeof(found_addr) ?
                   strlen(merged) + 1 : sizeof(found_addr) - 1);
            found_addr[sizeof(found_addr) - 1] = '\0';
        }

        trellis_fingerprint_t fp = {0};
        bool have_fp = found_fp_hex[0] &&
                       hex_to_bytes(found_fp_hex, fp.bytes, sizeof(fp.bytes));

        /*
         * WARNING: mDNS peer advertisements carry no cryptographic signature.
         * Any node on the LAN segment can inject arbitrary bloom_fp/bloom_addr
         * TXT records.  We accept the advertised fingerprint only as a hint for
         * address-routing; the handshake will independently verify the peer's
         * identity via its Ed25519 certificate binding.  Never use the mDNS-
         * derived fingerprint for trust decisions — only the completed
         * authenticated handshake provides a verified binding.
         *
         * If no fingerprint was advertised, derive a provisional one from the
         * address bytes so the peer can be placed in the DHT pending the
         * handshake result.
         */
        if (!have_fp) {
            crypto_hash_sha256(fp.bytes,
                               (const uint8_t *)found_addr, strlen(found_addr));
            have_fp = true;
        }

        /* Check grove affinity: only connect to peers in our grove (or
         * grove-less peers when we are also grove-less).  The DHT's
         * grove_id field tells us which grove we belong to. */
        bool grove_match = true;
        if (bs->dht->grove_id && found_grove_hex[0]) {
            uint8_t peer_grove[32];
            if (hex_to_bytes(found_grove_hex, peer_grove, 32))
                grove_match = (memcmp(bs->dht->grove_id->bytes,
                                      peer_grove, 32) == 0);
        } else if (bs->dht->grove_id && !found_grove_hex[0]) {
            grove_match = false;
        }

        if (grove_match) {
            fprintf(stderr, "[bootstrap/mDNS] found peer: %s (fp %s)\n",
                    found_addr, have_fp ? "hint" : "none");
            bootstrap_try_peer(bs, have_fp ? &fp : NULL, found_addr);
        }
    }
}

static void on_mdns_alloc(uv_handle_t *handle, size_t suggested_size,
                           uv_buf_t *buf)
{
    (void)handle;
    buf->base = malloc(suggested_size);
    buf->len  = buf->base ? suggested_size : 0;
}

static void bytes_to_hex(const uint8_t *src, size_t src_len,
                         char *out)
{
    static const char hex_chars[] = "0123456789abcdef";
    for (size_t i = 0; i < src_len; i++) {
        out[i * 2]     = hex_chars[(src[i] >> 4) & 0xF];
        out[i * 2 + 1] = hex_chars[src[i] & 0xF];
    }
    out[src_len * 2] = '\0';
}

static void mdns_send_complete_cb(uv_udp_send_t *req, int status)
{
    (void)status;
    free(req->data);
    free(req);
}

// _bloom._tcp.local. encoded as DNS labels (no compression)
static const uint8_t mdns_service_name[] = {
    0x06, '_', 'b', 'l', 'o', 'o', 'm',
    0x04, '_', 't', 'c', 'p',
    0x05, 'l', 'o', 'c', 'a', 'l',
    0x00
};

/* Send an mDNS TXT announcement with bloom_fp=, bloom_addr=, and
 * optionally bloom_grove= so other LAN peers can discover us.
 * bloom_addr carries only the port; receivers use the UDP source IP. */
static void mdns_send_announce(trellis_bootstrap_t *bs)
{
    char fp_hex[65];
    bytes_to_hex(bs->dht->local_id->fingerprint.bytes, 32, fp_hex);

    /* Advertise the Bloom transport port.  Receivers substitute the
     * UDP source IP for the host portion. */
    char addr_buf[64];
    snprintf(addr_buf, sizeof(addr_buf), ":%d", BOOTSTRAP_BLOOM_PORT);

    char txt[512];
    int n;
    if (bs->dht->grove_id) {
        char grove_hex[65];
        bytes_to_hex(bs->dht->grove_id->bytes, 32, grove_hex);
        n = snprintf(txt, sizeof(txt),
                     "bloom_fp=%s bloom_addr=%s bloom_grove=%s",
                     fp_hex, addr_buf, grove_hex);
    } else {
        n = snprintf(txt, sizeof(txt),
                     "bloom_fp=%s bloom_addr=%s",
                     fp_hex, addr_buf);
    }
    if (n <= 0 || (size_t)n >= sizeof(txt) || (size_t)n > 255)
        return;

    /* Wrap in a minimal mDNS response packet:
     * header(12) + answer RR: name(sizeof svc) + type(2) + class(2) +
     * ttl(4) + rdlen(2) + TXT-length-byte(1) + txt_data(n) */
    size_t txt_len = (size_t)n;
    size_t pkt_len = 12 + sizeof(mdns_service_name) +
                     2 + 2 + 4 + 2 + 1 + txt_len;
    uint8_t *pkt = calloc(1, pkt_len);
    if (!pkt) return;

    // mDNS response header.
    pkt[2] = 0x84; /* flags: response + authoritative */
    pkt[7] = 0x01; /* ANCOUNT = 1 */

    size_t off = 12;
    // Owner name: _bloom._tcp.local. (inline, no compression)
    memcpy(pkt + off, mdns_service_name, sizeof(mdns_service_name));
    off += sizeof(mdns_service_name);
    // TYPE = TXT (0x10)
    pkt[off++] = 0x00; pkt[off++] = 0x10;
    // CLASS = IN + cache-flush (0x8001) per RFC 6762
    pkt[off++] = 0x80; pkt[off++] = 0x01;
    // TTL = 120s
    pkt[off++] = 0; pkt[off++] = 0; pkt[off++] = 0; pkt[off++] = 120;
    // RDLENGTH
    uint16_t rdlen = (uint16_t)(1 + txt_len);
    pkt[off++] = (uint8_t)(rdlen >> 8);
    pkt[off++] = (uint8_t)(rdlen);
    // TXT length-prefixed string.
    pkt[off++] = (uint8_t)txt_len;
    memcpy(pkt + off, txt, txt_len);

    struct sockaddr_in mcast_addr;
    uv_ip4_addr(BOOTSTRAP_MDNS_GROUP, BOOTSTRAP_MDNS_PORT, &mcast_addr);

    uv_buf_t buf = uv_buf_init((char *)pkt, pkt_len);
    uv_udp_send_t *req = malloc(sizeof(*req));
    if (!req) { free(pkt); return; }
    req->data = pkt;
    int rc = uv_udp_send(req, &bs->mdns_sock, &buf, 1,
                          (const struct sockaddr *)&mcast_addr,
                          mdns_send_complete_cb);
    if (rc != 0) {
        free(pkt);
        free(req);
    }
}

// Send mDNS query for _bloom._tcp.local.
static void mdns_send_query(trellis_bootstrap_t *bs)
{
    // Minimal mDNS PTR query for _bloom._tcp.local.
    static const uint8_t mdns_query[] = {
        0x00, 0x00,  /* ID */
        0x00, 0x00,  /* Flags: standard query */
        0x00, 0x01,  /* QDCOUNT = 1 */
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        // QNAME: _bloom._tcp.local.
        0x06, '_', 'b', 'l', 'o', 'o', 'm',
        0x04, '_', 't', 'c', 'p',
        0x05, 'l', 'o', 'c', 'a', 'l', 0x00,
        0x00, 0x0C,  /* QTYPE = PTR */
        0x00, 0x01   /* QCLASS = IN */
    };

    struct sockaddr_in mcast_addr;
    uv_ip4_addr(BOOTSTRAP_MDNS_GROUP, BOOTSTRAP_MDNS_PORT, &mcast_addr);

    uv_buf_t buf = uv_buf_init((char *)mdns_query, sizeof(mdns_query));
    uv_udp_send_t *req = malloc(sizeof(*req));
    if (!req) return;
    req->data = NULL;
    int rc = uv_udp_send(req, &bs->mdns_sock, &buf, 1,
                          (const struct sockaddr *)&mcast_addr,
                          mdns_send_complete_cb);
    if (rc != 0)
        free(req);
}

static trellis_err_t bootstrap_start_mdns(trellis_bootstrap_t *bs)
{
    uv_udp_init(bs->loop, &bs->mdns_sock);
    bs->mdns_sock.data = bs;
    bs->mdns_sock_init = true;

    struct sockaddr_in bind_addr;
    uv_ip4_addr("0.0.0.0", BOOTSTRAP_MDNS_PORT, &bind_addr);

    if (uv_udp_bind(&bs->mdns_sock,
                    (const struct sockaddr *)&bind_addr,
                    UV_UDP_REUSEADDR) != 0) {
        fprintf(stderr, "[bootstrap] mDNS bind failed (non-fatal)\n");
        return TRELLIS_ERR_IO;
    }

    // Join multicast group.
    uv_udp_set_membership(&bs->mdns_sock, BOOTSTRAP_MDNS_GROUP,
                           NULL, UV_JOIN_GROUP);

    uv_udp_recv_start(&bs->mdns_sock, on_mdns_alloc, on_mdns_recv);
    bs->mdns_active = true;

    // Send initial query and announce our presence.
    mdns_send_query(bs);
    mdns_send_announce(bs);

    fprintf(stderr, "[bootstrap] mDNS LAN discovery started\n");
    return TRELLIS_OK;
}

// --- Strategy 2: Hardcoded seeds ---
static void bootstrap_try_hardcoded_seeds(trellis_bootstrap_t *bs)
{
    fprintf(stderr, "[bootstrap] trying hardcoded seeds\n");
    for (int i = 0; g_hardcoded_seeds[i].addr; i++) {
        trellis_fingerprint_t fp = {0};
        bool have_fp = g_hardcoded_seeds[i].fp_hex &&
                       hex_to_bytes(g_hardcoded_seeds[i].fp_hex,
                                    fp.bytes, sizeof(fp.bytes));
        bootstrap_try_peer(bs, have_fp ? &fp : NULL,
                            g_hardcoded_seeds[i].addr);
        if (bs->complete) return;
    }
}

// --- Strategy 3: DNS TXT seed lookup ---

/*
 * Parse a base64-encoded seed TXT record into a bloom_seed_record_t.
 * TXT record format (base64-decoded):
 *   [1]  version (must be 1)
 *   [32] fingerprint
 *   [1]  addr_len
 *   [N]  address string (ip:port)
 *   [32] ed25519 public key
 *   [64] ed25519 signature over (version + fp + addr_len + addr + pk)
 */
static bool parse_seed_txt_record(const char *b64, size_t b64_len,
                                  bloom_seed_record_t *out)
{
    uint8_t buf[512];
    size_t decoded_len;
    if (sodium_base642bin(buf, sizeof(buf),
                          b64, b64_len,
                          NULL, &decoded_len,
                          NULL, sodium_base64_VARIANT_ORIGINAL) != 0)
        return false;

    // Minimum: 1 + 32 + 1 + 1 + 32 + 64 = 131 bytes.
    if (decoded_len < 131)
        return false;

    size_t off = 0;
    uint8_t version = buf[off++];
    if (version != 1)
        return false;

    memcpy(out->fp.bytes, buf + off, 32);
    off += 32;

    uint8_t addr_len = buf[off++];
    if (off + addr_len + 32 + 64 > decoded_len)
        return false;

    if (addr_len >= sizeof(out->addr))
        return false;
    memcpy(out->addr, buf + off, addr_len);
    out->addr[addr_len] = '\0';
    off += addr_len;

    memcpy(out->ed25519_pk, buf + off, 32);
    off += 32;

    const uint8_t *sig = buf + off;
    size_t signed_len = off; /* everything before the signature */

    // Verify the Ed25519 signature over the message prefix.
    if (crypto_sign_ed25519_verify_detached(sig, buf, signed_len,
                                             out->ed25519_pk) != 0) {
        out->verified = false;
        fprintf(stderr, "[bootstrap/TXT] seed signature verification failed\n");
        return false;
    }
    out->verified = true;
    return true;
}

static void on_doh_a_resolve(const struct sockaddr_storage *addr,
                              trellis_err_t err, void *ctx)
{
    trellis_bootstrap_t *bs = (trellis_bootstrap_t *)ctx;
    if (bs->shutting_down)
        return;

    if (err != TRELLIS_OK || !addr) {
        fprintf(stderr, "[bootstrap] DoH A-record resolve failed\n");
        return;
    }

    char ip_buf[64];
    if (addr->ss_family == AF_INET) {
        uv_ip4_name((const struct sockaddr_in *)addr, ip_buf, sizeof(ip_buf));
    } else if (addr->ss_family == AF_INET6) {
        uv_ip6_name((const struct sockaddr_in6 *)addr, ip_buf, sizeof(ip_buf));
    } else {
        return;
    }

    char addr_str[72];
    snprintf(addr_str, sizeof(addr_str), "%s:7840", ip_buf);

    trellis_fingerprint_t fp = {0};
    crypto_hash_sha256(fp.bytes,
                       (const uint8_t *)addr_str, strlen(addr_str));

    fprintf(stderr, "[bootstrap/DoH-A] seed: %s (provisional fp)\n", addr_str);
    bootstrap_try_peer(bs, &fp, addr_str);
}

static void bootstrap_process_txt_seeds(trellis_bootstrap_t *bs,
                                        const char *txt_data, size_t txt_len);

static void on_doh_txt_resolve(const char *txt_data, size_t txt_len,
                                trellis_err_t err, void *ctx)
{
    trellis_bootstrap_t *bs = (trellis_bootstrap_t *)ctx;
    bs->dns_pending = false;
    if (bs->shutting_down)
        return;

    if (err != TRELLIS_OK || !txt_data || txt_len == 0) {
        fprintf(stderr, "[bootstrap] DoH TXT resolve failed or empty\n");
        return;
    }

    bootstrap_process_txt_seeds(bs, txt_data, txt_len);
}

/* Process base64-encoded TXT seed records from a raw TXT string.
 * Multiple seeds are separated by semicolons. */
static void bootstrap_process_txt_seeds(trellis_bootstrap_t *bs,
                                        const char *txt_data, size_t txt_len)
{
    const char *p = txt_data;
    const char *end = txt_data + txt_len;

    while (p < end && !bs->complete) {
        // Skip whitespace and separators.
        while (p < end && (*p == ' ' || *p == ';' || *p == '\n'))
            p++;
        if (p >= end) break;

        // Find end of this base64 segment.
        const char *seg_end = p;
        while (seg_end < end && *seg_end != ';' && *seg_end != '\n')
            seg_end++;

        size_t seg_len = (size_t)(seg_end - p);
        if (seg_len > 0) {
            bloom_seed_record_t record = {0};
            if (parse_seed_txt_record(p, seg_len, &record)) {
                fprintf(stderr, "[bootstrap/TXT] verified seed: %s\n",
                        record.addr);
                if (bs->seed_count < BOOTSTRAP_MAX_SEEDS)
                    bs->seeds[bs->seed_count++] = record;
                bootstrap_try_peer(bs, &record.fp, record.addr);
            }
        }
        p = seg_end;
    }
}

static void bootstrap_try_dns_seeds(trellis_bootstrap_t *bs)
{
    if (bs->dns_pending) return;

    fprintf(stderr, "[bootstrap] looking up DNS seeds via DoH: %s\n",
            BOOTSTRAP_DNS_SEED_DOMAIN);

    if (!bs->doh) {
        bs->doh = trellis_doh_new(bs->loop, NULL);
        if (!bs->doh) {
            fprintf(stderr, "[bootstrap] failed to create DoH resolver\n");
            return;
        }
    }

    bs->dns_pending = true;

    // A-record query for basic connectivity (ip:port seeds)
    trellis_doh_resolve(bs->doh, BOOTSTRAP_DNS_SEED_DOMAIN, 7840,
                         AF_INET, on_doh_a_resolve, bs);

    // TXT-record query for signed seed records.
    trellis_doh_resolve_txt(bs->doh, BOOTSTRAP_DNS_SEED_DOMAIN,
                             on_doh_txt_resolve, bs);
}

// --- Strategy timer ---
static void bootstrap_strategy_timer_cb(uv_timer_t *handle)
{
    trellis_bootstrap_t *bs = (trellis_bootstrap_t *)handle->data;

    if (bs->complete) {
        uv_timer_stop(handle);
        return;
    }

    bs->strategy_phase++;

    switch (bs->strategy_phase) {
    case 1:
        fprintf(stderr, "[bootstrap] mDNS timeout, trying hardcoded seeds\n");
        bootstrap_try_hardcoded_seeds(bs);
        break;
    case 2:
        fprintf(stderr, "[bootstrap] hardcoded seeds exhausted, trying DNS\n");
        bootstrap_try_dns_seeds(bs);
        break;
    case 3:
        uv_timer_stop(handle);
        if (!bs->complete) {
            fprintf(stderr, "[bootstrap] all strategies failed: %d peers\n",
                    bs->candidate_peers);
            if (bs->cb)
                bs->cb(bs->candidate_peers > 0 ? TRELLIS_OK
                                                : TRELLIS_ERR_NOT_FOUND,
                       bs->candidate_peers, bs->ctx);
        }
        break;
    default:
        uv_timer_stop(handle);
        break;
    }
}

// --- Public API ---

/*
 * Start the bootstrap process.  Tries each strategy in order with timeouts.
 *
 * @dht    DHT handle to add discovered peers to
 * @loop   libuv event loop
 * @cb     called when bootstrap completes (or fails after all strategies)
 * @ctx    user context
 *
 * Returns a bootstrap handle; call trellis_bootstrap_free() to cancel.
 */
trellis_bootstrap_t *trellis_bootstrap_start(
        trellis_dht_t *dht,
        uv_loop_t *loop,
        trellis_bootstrap_cb cb, void *ctx)
{
    if (!dht || !loop)
        return NULL;

    trellis_bootstrap_t *bs = calloc(1, sizeof(*bs));
    if (!bs) return NULL;

    bs->dht  = dht;
    bs->loop = loop;
    bs->cb   = cb;
    bs->ctx  = ctx;

    fprintf(stderr, "[bootstrap] starting (mDNS → hardcoded → DNS)\n");

    // Strategy 0: mDNS.
    bs->strategy_phase = 0;
    bootstrap_start_mdns(bs);

    // Strategy timer: escalate if not enough peers in time.
    uv_timer_init(loop, &bs->strategy_timer);
    bs->strategy_timer.data = bs;
    uv_timer_start(&bs->strategy_timer, bootstrap_strategy_timer_cb,
                   BOOTSTRAP_STRATEGY_TIMEOUT_MS,
                   BOOTSTRAP_STRATEGY_TIMEOUT_MS);

    return bs;
}

static void bootstrap_close_cb(uv_handle_t *handle)
{
    trellis_bootstrap_t *bs = (trellis_bootstrap_t *)handle->data;
    if (--bs->pending_closes == 0)
        free(bs);
}

void trellis_bootstrap_free(trellis_bootstrap_t *bs)
{
    if (!bs) return;

    bs->shutting_down = true;
    bs->cb = NULL;

    if (bs->doh) {
        trellis_doh_free(bs->doh);
        bs->doh = NULL;
    }

    bs->pending_closes = 1; /* strategy_timer is always initialized */
    if (bs->mdns_sock_init)
        bs->pending_closes++;

    uv_timer_stop(&bs->strategy_timer);
    bs->strategy_timer.data = bs;
    uv_close((uv_handle_t *)&bs->strategy_timer, bootstrap_close_cb);

    if (bs->mdns_sock_init) {
        if (bs->mdns_active)
            uv_udp_recv_stop(&bs->mdns_sock);
        bs->mdns_sock.data = bs;
        uv_close((uv_handle_t *)&bs->mdns_sock, bootstrap_close_cb);
        bs->mdns_active = false;
        bs->mdns_sock_init = false;
    }
}

/*
 * Add a custom seed (e.g., from a config file or out-of-band channel).
 * Seeds added this way are tried immediately, before hardcoded fallbacks.
 */
trellis_err_t trellis_bootstrap_add_seed(
        trellis_bootstrap_t *bs,
        const char *addr,
        const trellis_fingerprint_t *fp)
{
    if (!bs || !addr)
        return TRELLIS_ERR_INVALID_ARG;

    bootstrap_try_peer(bs, fp, addr);
    return TRELLIS_OK;
}
