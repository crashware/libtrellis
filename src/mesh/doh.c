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

#include "internal.h"
#include <uv.h>
#include <string.h>
#include <stdio.h>

/*
 * DNS-over-HTTPS resolver for exit relays.
 *
 * Builds a minimal DNS wire-format query, sends it via HTTPS POST to
 * a configurable DoH endpoint (RFC 8484), and parses the A/AAAA answer.
 * Falls back to system resolver (uv_getaddrinfo) on failure.
 *
 * This avoids leaking plaintext DNS queries from exit relays.
 */
#define DNS_FLAG_RD         0x0100
#define DNS_TYPE_A          1
#define DNS_TYPE_TXT        16
#define DNS_TYPE_AAAA       28
#define DNS_CLASS_IN        1
#define DNS_HDR_LEN         12

static size_t dns_encode_name(const char *name, uint8_t *out, size_t cap)
{
    size_t off = 0;
    const char *p = name;
    while (*p) {
        const char *dot = strchr(p, '.');
        size_t label_len = dot ? (size_t)(dot - p) : strlen(p);
        if (label_len > 63 || off + 1 + label_len >= cap)
            return 0;
        out[off++] = (uint8_t)label_len;
        memcpy(out + off, p, label_len);
        off += label_len;
        p += label_len;
        if (dot) p++;
    }
    if (off >= cap) return 0;
    out[off++] = 0; /* root label */
    return off;
}

static size_t dns_build_query(const char *hostname, uint16_t qtype,
                              uint8_t *buf, size_t cap)
{
    if (cap < DNS_HDR_LEN + 256 + 4)
        return 0;

    uint16_t id = (uint16_t)(randombytes_random() & 0xFFFF);

    // Header
    buf[0] = (uint8_t)(id >> 8);
    buf[1] = (uint8_t)(id);
    buf[2] = (uint8_t)(DNS_FLAG_RD >> 8);
    buf[3] = (uint8_t)(DNS_FLAG_RD);
    buf[4] = 0; buf[5] = 1;   /* QDCOUNT = 1 */
    buf[6] = 0; buf[7] = 0;   /* ANCOUNT */
    buf[8] = 0; buf[9] = 0;   /* NSCOUNT */
    buf[10] = 0; buf[11] = 0; /* ARCOUNT */

    // Question
    size_t name_len = dns_encode_name(hostname, buf + DNS_HDR_LEN,
                                       cap - DNS_HDR_LEN - 4);
    if (name_len == 0) return 0;

    size_t off = DNS_HDR_LEN + name_len;
    buf[off++] = (uint8_t)(qtype >> 8);
    buf[off++] = (uint8_t)(qtype);
    buf[off++] = 0;
    buf[off++] = DNS_CLASS_IN;

    return off;
}

static bool dns_parse_a_record(const uint8_t *resp, size_t resp_len,
                               struct sockaddr_storage *out)
{
    if (resp_len < DNS_HDR_LEN + 4)
        return false;

    uint16_t ancount = ((uint16_t)resp[6] << 8) | resp[7];
    if (ancount == 0)
        return false;

    // Skip question section.
    size_t off = DNS_HDR_LEN;
    while (off < resp_len && resp[off] != 0) {
        if ((resp[off] & 0xC0) == 0xC0) { off += 2; goto past_qname; }
        off += 1 + resp[off];
    }
    off++; /* null terminator */
past_qname:
    off += 4; /* QTYPE + QCLASS */

    // Parse first answer.
    for (uint16_t i = 0; i < ancount && off < resp_len; i++) {
        // Skip name (may be compressed)
        if ((resp[off] & 0xC0) == 0xC0)
            off += 2;
        else {
            while (off < resp_len && resp[off] != 0) off += 1 + resp[off];
            off++;
        }
        if (off + 10 > resp_len) return false;

        uint16_t rtype = ((uint16_t)resp[off] << 8) | resp[off + 1];
        uint16_t rdlen = ((uint16_t)resp[off + 8] << 8) | resp[off + 9];
        off += 10;

        if (rtype == DNS_TYPE_A && rdlen == 4 && off + 4 <= resp_len) {
            struct sockaddr_in *sa = (struct sockaddr_in *)out;
            memset(sa, 0, sizeof(*sa));
            sa->sin_family = AF_INET;
            memcpy(&sa->sin_addr, resp + off, 4);
            return true;
        }
        if (rtype == DNS_TYPE_AAAA && rdlen == 16 && off + 16 <= resp_len) {
            struct sockaddr_in6 *sa = (struct sockaddr_in6 *)out;
            memset(sa, 0, sizeof(*sa));
            sa->sin6_family = AF_INET6;
            memcpy(&sa->sin6_addr, resp + off, 16);
            return true;
        }
        off += rdlen;
    }
    return false;
}

/*
 * Parse TXT records from a DNS response into a single concatenated buffer.
 * Each TXT record consists of one or more <length><data> strings within RDATA.
 * We concatenate all strings with ';' separators.
 * Returns the number of bytes written to out (excluding NUL terminator).
 */
static size_t dns_parse_txt_records(const uint8_t *resp, size_t resp_len,
                                    char *out, size_t out_cap)
{
    if (resp_len < DNS_HDR_LEN + 4 || out_cap == 0)
        return 0;

    uint16_t ancount = ((uint16_t)resp[6] << 8) | resp[7];
    if (ancount == 0)
        return 0;

    size_t off = DNS_HDR_LEN;
    // Skip question section.
    while (off < resp_len && resp[off] != 0) {
        if ((resp[off] & 0xC0) == 0xC0) { off += 2; goto past_txt_qname; }
        off += 1 + resp[off];
    }
    off++;
past_txt_qname:
    off += 4; /* QTYPE + QCLASS */

    size_t written = 0;
    for (uint16_t i = 0; i < ancount && off < resp_len; i++) {
        if ((resp[off] & 0xC0) == 0xC0)
            off += 2;
        else {
            while (off < resp_len && resp[off] != 0) off += 1 + resp[off];
            off++;
        }
        if (off + 10 > resp_len) break;

        uint16_t rtype = ((uint16_t)resp[off] << 8) | resp[off + 1];
        uint16_t rdlen = ((uint16_t)resp[off + 8] << 8) | resp[off + 9];
        off += 10;

        if (rtype == DNS_TYPE_TXT && off + rdlen <= resp_len) {
            size_t rd_off = 0;
            while (rd_off < rdlen) {
                uint8_t slen = resp[off + rd_off];
                rd_off++;
                if (rd_off + slen > rdlen) break;
                size_t copy = slen;
                if (written + copy + 2 > out_cap)
                    copy = out_cap - written - 2;
                if (copy > 0) {
                    if (written > 0) out[written++] = ';';
                    memcpy(out + written, resp + off + rd_off, copy);
                    written += copy;
                }
                rd_off += slen;
            }
        }
        off += rdlen;
    }
    if (written < out_cap) out[written] = '\0';
    return written;
}

// --- DoH resolver ---
#define DOH_CACHE_MAX       1024
#define DOH_CACHE_TTL_MS    300000  /* 5 minutes default */

typedef struct doh_cache_entry {
    char                     hostname[256];
    struct sockaddr_storage  addr;
    uint64_t                 expires_at;
    bool                     occupied;
} doh_cache_entry_t;

struct trellis_doh_resolver {
    char                endpoint[512];
    uv_loop_t          *loop;
    doh_cache_entry_t   cache[DOH_CACHE_MAX];
    bool                fallback_enabled;
};

trellis_doh_resolver_t *trellis_doh_new(uv_loop_t *loop,
                                         const char *endpoint)
{
    trellis_doh_resolver_t *r = calloc(1, sizeof(*r));
    if (!r) return NULL;
    r->loop = loop;
    r->fallback_enabled = true;
    if (endpoint)
        snprintf(r->endpoint, sizeof(r->endpoint), "%s", endpoint);
    else
        snprintf(r->endpoint, sizeof(r->endpoint),
                 "https://1.1.1.1/dns-query");
    return r;
}

void trellis_doh_free(trellis_doh_resolver_t *r)
{
    free(r);
}

// Check cache for a matching entry.
static bool doh_cache_lookup(trellis_doh_resolver_t *r,
                             const char *hostname,
                             struct sockaddr_storage *out)
{
    uint64_t now = trellis_now_ms();
    for (size_t i = 0; i < DOH_CACHE_MAX; i++) {
        if (!r->cache[i].occupied) continue;
        if (now > r->cache[i].expires_at) {
            r->cache[i].occupied = false;
            continue;
        }
        if (strcmp(r->cache[i].hostname, hostname) == 0) {
            *out = r->cache[i].addr;
            return true;
        }
    }
    return false;
}

static void doh_cache_insert(trellis_doh_resolver_t *r,
                             const char *hostname,
                             const struct sockaddr_storage *addr)
{
    // Find free or expired slot.
    uint64_t now = trellis_now_ms();
    int slot = -1;
    for (size_t i = 0; i < DOH_CACHE_MAX; i++) {
        if (!r->cache[i].occupied || now > r->cache[i].expires_at) {
            slot = (int)i;
            break;
        }
    }
    if (slot < 0) slot = 0; /* evict first entry */

    snprintf(r->cache[slot].hostname, sizeof(r->cache[slot].hostname),
             "%s", hostname);
    r->cache[slot].addr = *addr;
    r->cache[slot].expires_at = now + DOH_CACHE_TTL_MS;
    r->cache[slot].occupied = true;
}

// --- DoH HTTPS POST via libuv worker thread ---

/*
 * We perform the DoH HTTPS POST on a libuv thread-pool worker to avoid
 * blocking the event loop. The worker uses mbedTLS (already a project
 * dependency) for TLS and POSIX sockets for TCP. On failure, we fall
 * back to the system resolver.
 *
 * The DoH server IP (1.1.1.1 / 1.0.0.1 for Cloudflare, 8.8.8.8 for
 * Google) is hard-coded to avoid a recursive DNS dependency.
 */
#include "../platform.h"

#ifdef TRELLIS_WITH_TLS
#include <mbedtls/ssl.h>
#include <mbedtls/entropy.h>
#include <mbedtls/ctr_drbg.h>
#include <mbedtls/net_sockets.h>
#include <mbedtls/error.h>
#include <mbedtls/x509_crt.h>

/* DigiCert Global Root G2 — pinned CA for Cloudflare 1.1.1.1 DoH.
 * Also covers Google 8.8.8.8 via the GTS Root R1 cross-sign chain.
 * Validity: 2013-08-01 to 2038-01-15  Subject: CN=DigiCert Global Root G2 */
static const char DOH_PINNED_CA_PEM[] =
    "-----BEGIN CERTIFICATE-----\n"
    "MIIDjjCCAnagAwIBAgIQAzrx5qcRqaC7KGSxHQn65TANBgkqhkiG9w0BAQsFADBh\n"
    "MQswCQYDVQQGEwJVUzEVMBMGA1UEChMMRGlnaUNlcnQgSW5jMRkwFwYDVQQLExB3\n"
    "d3cuZGlnaWNlcnQuY29tMSAwHgYDVQQDExdEaWdpQ2VydCBHbG9iYWwgUm9vdCBH\n"
    "MjAeFw0xMzA4MDExMjAwMDBaFw0zODAxMTUxMjAwMDBaMGExCzAJBgNVBAYTAlVT\n"
    "MRUwEwYDVQQKEwxEaWdpQ2VydCBJbmMxGTAXBgNVBAsTEHd3dy5kaWdpY2VydC5j\n"
    "b20xIDAeBgNVBAMTF0RpZ2lDZXJ0IEdsb2JhbCBSb290IEcyMIIBIjANBgkqhkiG\n"
    "9w0BAQEFAAOCAQ8AMIIBCgKCAQEAuzfNNNx7a8myaJCtSnX/RrohCgiN9RlUyfuI\n"
    "2/Ou8jqJkTx65qsGGmvPrC3oXgkkRLpimn7Wo6h+4FR1IAWsULecYxpsMNzaHxmx\n"
    "1x7e/dfgy5SDN67sH0NO3Xss0r0upS/kqbitOtSZpLYl6ZtrAGCSYP9PIUkY92eQ\n"
    "q2EGnI/yuum06ZIya7XzV+hdG82MHauVBJVJ8zUtluNJbd134/tJS7SsVQepj5Wzt\n"
    "COfergPmMfAtGyi+1KQLHL/Jrn3qGPKAY8S2SQvgAw+8mfVB4KEvPSmqrK1+TlgT\n"
    "iw9bO2S3p0eA6WBWylp4GHy6MYZB7ox46RBRL+/M8JGhiJwLQIDAQABo0IwQDAP\n"
    "BgNVHRMBAf8EBTADAQH/MA4GA1UdDwEB/wQEAwIBhjAdBgNVHQ4EFgQUTiJUIBiV\n"
    "5uNu5g/6+rkS7QYXjzkwDQYJKoZIhvcNAQELBQADggEBAGBnKJRvDkhj+zHhVL3m\n"
    "bIe+OI0Jw+i/2cAhmcKW3yomK/gBnhFBiwnf2pTrfLeVUjT6cFsLz3uWGI9G1CA4\n"
    "oHnWYOITYjuc7c3os36BHWbBKi3xN7KkCY/CxWm+N3G2GqBWQkBH/d0cENsqiYQB\n"
    "5VCb0JlD5lQimVcma+zYoPC6zH2ljVGGioyB62QL70I0MBMRnIC+YKXER7OgO3oD\n"
    "FfBOd1nNGBFQ9r0rP5xZN3vVwKQ3Z4B5PmTiYqahrKyXhMtBqOE5ZslBWhIFLEqH\n"
    "sy1WZQBKUbI5s0M/rObW2YJdjS+R1gmQh0fx5f7JVelx4e0SAJfPf/gYMOxCHNE7\n"
    "LFo=\n"
    "-----END CERTIFICATE-----\n";
#endif

typedef struct {
    trellis_doh_resolver_t *resolver;
    char                    hostname[256];
    uint16_t                port;
    int                     ai_family;
    trellis_doh_resolve_cb  cb;
    void                   *ctx;
    struct sockaddr_storage result;
    bool                    resolved;
    uv_work_t               work;
    uv_getaddrinfo_t        fallback_req;
} doh_resolve_req_t;

#ifdef TRELLIS_WITH_TLS
/*
 * Extract the IP + port from a DoH endpoint URL ("https://1.1.1.1/dns-query").
 * Returns false if parsing fails.
 */
static bool doh_parse_endpoint(const char *endpoint,
                                char *ip_out, size_t ip_cap,
                                char *host_out, size_t host_cap,
                                char *path_out, size_t path_cap)
{
    const char *p = endpoint;
    if (strncmp(p, "https://", 8) != 0) return false;
    p += 8;
    const char *slash = strchr(p, '/');
    size_t hlen = slash ? (size_t)(slash - p) : strlen(p);
    if (hlen >= ip_cap || hlen >= host_cap) return false;
    memcpy(ip_out, p, hlen);
    ip_out[hlen] = '\0';
    memcpy(host_out, p, hlen);
    host_out[hlen] = '\0';
    if (slash)
        snprintf(path_out, path_cap, "%s", slash);
    else
        snprintf(path_out, path_cap, "/dns-query");
    return true;
}

static bool doh_https_post(const char *server_ip, const char *sni_host,
                            const char *path,
                            const uint8_t *body, size_t body_len,
                            uint8_t *resp_out, size_t resp_cap,
                            size_t *resp_len_out)
{
    mbedtls_net_context server_fd;
    mbedtls_ssl_context ssl;
    mbedtls_ssl_config conf;
    mbedtls_entropy_context entropy;
    mbedtls_ctr_drbg_context ctr_drbg;
    bool ok = false;

    mbedtls_x509_crt cacert;

    mbedtls_net_init(&server_fd);
    mbedtls_ssl_init(&ssl);
    mbedtls_ssl_config_init(&conf);
    mbedtls_entropy_init(&entropy);
    mbedtls_ctr_drbg_init(&ctr_drbg);
    mbedtls_x509_crt_init(&cacert);

    if (mbedtls_ctr_drbg_seed(&ctr_drbg, mbedtls_entropy_func, &entropy,
                               (const unsigned char *)"doh", 3) != 0)
        goto cleanup;

    if (mbedtls_net_connect(&server_fd, server_ip, "443",
                             MBEDTLS_NET_PROTO_TCP) != 0)
        goto cleanup;

    if (mbedtls_ssl_config_defaults(&conf, MBEDTLS_SSL_IS_CLIENT,
                                     MBEDTLS_SSL_TRANSPORT_STREAM,
                                     MBEDTLS_SSL_PRESET_DEFAULT) != 0)
        goto cleanup;

    // Pin the DoH resolver's CA certificate for MITM protection.
    if (mbedtls_x509_crt_parse(&cacert,
                                (const unsigned char *)DOH_PINNED_CA_PEM,
                                sizeof(DOH_PINNED_CA_PEM)) != 0)
        goto cleanup;

    mbedtls_ssl_conf_ca_chain(&conf, &cacert, NULL);
    mbedtls_ssl_conf_authmode(&conf, MBEDTLS_SSL_VERIFY_REQUIRED);
    mbedtls_ssl_conf_rng(&conf, mbedtls_ctr_drbg_random, &ctr_drbg);

    if (mbedtls_ssl_setup(&ssl, &conf) != 0)
        goto cleanup;
    if (mbedtls_ssl_set_hostname(&ssl, sni_host) != 0)
        goto cleanup;

    mbedtls_ssl_set_bio(&ssl, &server_fd, mbedtls_net_send,
                         mbedtls_net_recv, NULL);

    int ret;
    while ((ret = mbedtls_ssl_handshake(&ssl)) != 0) {
        if (ret != MBEDTLS_ERR_SSL_WANT_READ &&
            ret != MBEDTLS_ERR_SSL_WANT_WRITE)
            goto cleanup;
    }

    // Build HTTP request.
    char http_req[1024];
    int http_len = snprintf(http_req, sizeof(http_req),
        "POST %s HTTP/1.1\r\n"
        "Host: %s\r\n"
        "Content-Type: application/dns-message\r\n"
        "Accept: application/dns-message\r\n"
        "Content-Length: %zu\r\n"
        "Connection: close\r\n"
        "\r\n",
        path, sni_host, body_len);

    while ((ret = mbedtls_ssl_write(&ssl, (unsigned char *)http_req,
                                     (size_t)http_len)) <= 0) {
        if (ret != MBEDTLS_ERR_SSL_WANT_WRITE) goto cleanup;
    }
    while ((ret = mbedtls_ssl_write(&ssl, body, body_len)) <= 0) {
        if (ret != MBEDTLS_ERR_SSL_WANT_WRITE) goto cleanup;
    }

    // Read response (we only need the body after headers)
    size_t total = 0;
    uint8_t raw_resp[4096];
    while (total < sizeof(raw_resp)) {
        ret = mbedtls_ssl_read(&ssl, raw_resp + total,
                                sizeof(raw_resp) - total);
        if (ret == 0 || ret == MBEDTLS_ERR_SSL_PEER_CLOSE_NOTIFY) break;
        if (ret < 0) {
            if (ret == MBEDTLS_ERR_SSL_WANT_READ) continue;
            goto cleanup;
        }
        total += (size_t)ret;
    }

    // Find end of HTTP headers ("\r\n\r\n")
    const char *hdr_end = NULL;
    for (size_t i = 0; i + 3 < total; i++) {
        if (raw_resp[i] == '\r' && raw_resp[i+1] == '\n' &&
            raw_resp[i+2] == '\r' && raw_resp[i+3] == '\n') {
            hdr_end = (const char *)(raw_resp + i + 4);
            break;
        }
    }
    if (!hdr_end) goto cleanup;

    size_t body_off = (size_t)(hdr_end - (const char *)raw_resp);
    size_t dns_len = total - body_off;
    if (dns_len > resp_cap) dns_len = resp_cap;
    memcpy(resp_out, raw_resp + body_off, dns_len);
    *resp_len_out = dns_len;
    ok = true;

cleanup:
    mbedtls_ssl_close_notify(&ssl);
    mbedtls_net_free(&server_fd);
    mbedtls_ssl_free(&ssl);
    mbedtls_ssl_config_free(&conf);
    mbedtls_ctr_drbg_free(&ctr_drbg);
    mbedtls_entropy_free(&entropy);
    mbedtls_x509_crt_free(&cacert);
    return ok;
}

static void doh_work_cb(uv_work_t *req)
{
    doh_resolve_req_t *dr = (doh_resolve_req_t *)req->data;

    char ip[256], host[256], path[256];
    if (!doh_parse_endpoint(dr->resolver->endpoint, ip, sizeof(ip),
                             host, sizeof(host), path, sizeof(path)))
        return;

    uint16_t qtype = (dr->ai_family == AF_INET6) ? DNS_TYPE_AAAA : DNS_TYPE_A;
    uint8_t query[512];
    size_t qlen = dns_build_query(dr->hostname, qtype, query, sizeof(query));
    if (qlen == 0) return;

    uint8_t resp[2048];
    size_t resp_len = 0;
    if (!doh_https_post(ip, host, path, query, qlen,
                         resp, sizeof(resp), &resp_len))
        return;

    if (dns_parse_a_record(resp, resp_len, &dr->result))
        dr->resolved = true;

    // If we asked for A and got nothing, try AAAA (or vice versa)
    if (!dr->resolved && dr->ai_family == AF_UNSPEC) {
        qlen = dns_build_query(dr->hostname, DNS_TYPE_AAAA, query, sizeof(query));
        if (qlen > 0 && doh_https_post(ip, host, path, query, qlen,
                                         resp, sizeof(resp), &resp_len)) {
            if (dns_parse_a_record(resp, resp_len, &dr->result))
                dr->resolved = true;
        }
    }
}
#endif /* TRELLIS_WITH_TLS */

static void doh_fallback_cb(uv_getaddrinfo_t *req, int status,
                            struct addrinfo *res)
{
    doh_resolve_req_t *dr = (doh_resolve_req_t *)req->data;

    if (status == 0 && res) {
        memcpy(&dr->result, res->ai_addr,
               res->ai_addr->sa_family == AF_INET6
                   ? sizeof(struct sockaddr_in6)
                   : sizeof(struct sockaddr_in));
        if (dr->result.ss_family == AF_INET)
            ((struct sockaddr_in *)&dr->result)->sin_port = htons(dr->port);
        else
            ((struct sockaddr_in6 *)&dr->result)->sin6_port = htons(dr->port);

        doh_cache_insert(dr->resolver, dr->hostname, &dr->result);
        if (dr->cb)
            dr->cb(&dr->result, TRELLIS_OK, dr->ctx);
    } else {
        if (dr->cb)
            dr->cb(NULL, TRELLIS_ERR_NOT_FOUND, dr->ctx);
    }
    if (res) uv_freeaddrinfo(res);
    free(dr);
}

#ifdef TRELLIS_WITH_TLS
static void doh_after_work_cb(uv_work_t *req, int status)
{
    doh_resolve_req_t *dr = (doh_resolve_req_t *)req->data;

    if (dr->resolved) {
        if (dr->result.ss_family == AF_INET)
            ((struct sockaddr_in *)&dr->result)->sin_port = htons(dr->port);
        else if (dr->result.ss_family == AF_INET6)
            ((struct sockaddr_in6 *)&dr->result)->sin6_port = htons(dr->port);
        doh_cache_insert(dr->resolver, dr->hostname, &dr->result);
        if (dr->cb)
            dr->cb(&dr->result, TRELLIS_OK, dr->ctx);
        free(dr);
        return;
    }

    // DoH failed — fall back to system resolver.
    if (dr->resolver->fallback_enabled) {
        dr->fallback_req.data = dr;
        char port_str[8];
        snprintf(port_str, sizeof(port_str), "%u", dr->port);
        struct addrinfo hints = {0};
        hints.ai_family = dr->ai_family;
        hints.ai_socktype = SOCK_STREAM;
        if (uv_getaddrinfo(dr->resolver->loop, &dr->fallback_req,
                            doh_fallback_cb, dr->hostname, port_str,
                            &hints) == 0)
            return; /* will free dr in callback */
    }

    if (dr->cb)
        dr->cb(NULL, TRELLIS_ERR_NOT_FOUND, dr->ctx);
    free(dr);
}
#endif /* TRELLIS_WITH_TLS */

trellis_err_t trellis_doh_resolve(trellis_doh_resolver_t *r,
                                  const char *hostname,
                                  uint16_t port,
                                  int ai_family,
                                  trellis_doh_resolve_cb cb,
                                  void *ctx)
{
    if (!r || !hostname)
        return TRELLIS_ERR_INVALID_ARG;

    // Check cache first.
    struct sockaddr_storage cached;
    if (doh_cache_lookup(r, hostname, &cached)) {
        if (cached.ss_family == AF_INET)
            ((struct sockaddr_in *)&cached)->sin_port = htons(port);
        else
            ((struct sockaddr_in6 *)&cached)->sin6_port = htons(port);
        if (cb) cb(&cached, TRELLIS_OK, ctx);
        return TRELLIS_OK;
    }

    doh_resolve_req_t *dr = calloc(1, sizeof(*dr));
    if (!dr)
        return TRELLIS_ERR_NOMEM;

    dr->resolver = r;
    snprintf(dr->hostname, sizeof(dr->hostname), "%s", hostname);
    dr->port = port;
    dr->ai_family = ai_family;
    dr->cb = cb;
    dr->ctx = ctx;

#ifdef TRELLIS_WITH_TLS
    // Use real DoH via HTTPS POST.
    dr->work.data = dr;
    int rc = uv_queue_work(r->loop, &dr->work, doh_work_cb, doh_after_work_cb);
    if (rc != 0) {
        free(dr);
        return TRELLIS_ERR_IO;
    }
#else
    // No TLS support — fall back to system resolver.
    dr->fallback_req.data = dr;
    char port_str[8];
    snprintf(port_str, sizeof(port_str), "%u", port);
    struct addrinfo hints = {0};
    hints.ai_family = ai_family;
    hints.ai_socktype = SOCK_STREAM;
    int rc = uv_getaddrinfo(r->loop, &dr->fallback_req,
                            doh_fallback_cb, hostname, port_str, &hints);
    if (rc != 0) {
        free(dr);
        return TRELLIS_ERR_IO;
    }
#endif

    return TRELLIS_OK;
}

// --- DoH TXT record resolution ---
typedef struct {
    trellis_doh_resolver_t *resolver;
    char                    hostname[256];
    trellis_doh_txt_cb      cb;
    void                   *ctx;
    char                    txt_buf[4096];
    size_t                  txt_len;
    bool                    resolved;
    uv_work_t               work;
} doh_txt_req_t;

#ifdef TRELLIS_WITH_TLS
static void doh_txt_work_cb(uv_work_t *req)
{
    doh_txt_req_t *tr = (doh_txt_req_t *)req->data;

    char ip[256], host[256], path[256];
    if (!doh_parse_endpoint(tr->resolver->endpoint, ip, sizeof(ip),
                             host, sizeof(host), path, sizeof(path)))
        return;

    uint8_t query[512];
    size_t qlen = dns_build_query(tr->hostname, DNS_TYPE_TXT,
                                   query, sizeof(query));
    if (qlen == 0) return;

    uint8_t resp[4096];
    size_t resp_len = 0;
    if (!doh_https_post(ip, host, path, query, qlen,
                         resp, sizeof(resp), &resp_len))
        return;

    tr->txt_len = dns_parse_txt_records(resp, resp_len,
                                         tr->txt_buf, sizeof(tr->txt_buf));
    if (tr->txt_len > 0)
        tr->resolved = true;
}

static void doh_txt_after_work_cb(uv_work_t *req, int status)
{
    doh_txt_req_t *tr = (doh_txt_req_t *)req->data;
    (void)status;

    if (tr->resolved) {
        if (tr->cb)
            tr->cb(tr->txt_buf, tr->txt_len, TRELLIS_OK, tr->ctx);
    } else {
        if (tr->cb)
            tr->cb(NULL, 0, TRELLIS_ERR_NOT_FOUND, tr->ctx);
    }
    free(tr);
}
#endif

trellis_err_t trellis_doh_resolve_txt(trellis_doh_resolver_t *r,
                                       const char *hostname,
                                       trellis_doh_txt_cb cb,
                                       void *ctx)
{
    if (!r || !hostname)
        return TRELLIS_ERR_INVALID_ARG;

    doh_txt_req_t *tr = calloc(1, sizeof(*tr));
    if (!tr)
        return TRELLIS_ERR_NOMEM;

    tr->resolver = r;
    snprintf(tr->hostname, sizeof(tr->hostname), "%s", hostname);
    tr->cb = cb;
    tr->ctx = ctx;

#ifdef TRELLIS_WITH_TLS
    tr->work.data = tr;
    int rc = uv_queue_work(r->loop, &tr->work,
                            doh_txt_work_cb, doh_txt_after_work_cb);
    if (rc != 0) {
        free(tr);
        return TRELLIS_ERR_IO;
    }
#else
    // No TLS: TXT resolution unsupported without DoH.
    if (cb) cb(NULL, 0, TRELLIS_ERR_NOT_FOUND, ctx);
    free(tr);
#endif

    return TRELLIS_OK;
}
