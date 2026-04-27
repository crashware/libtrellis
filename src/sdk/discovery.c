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
#include "../platform.h"

#define DISCOVERY_MULTICAST_GROUP "239.255.77.69"
#define DISCOVERY_PORT            9199
#define DISCOVERY_INTERVAL_MS     2000
#define DISCOVERY_MAGIC           "BLOOM\x01"
#define DISCOVERY_MAGIC_LEN       6
#define DISCOVERY_PKT_LEN         (DISCOVERY_MAGIC_LEN + TRELLIS_FINGERPRINT_LEN + 2)
#define DISCOVERY_PENDING_MAX     16

// Track fingerprints we've already started connecting to.
static trellis_fingerprint_t pending_fps[DISCOVERY_PENDING_MAX];
static int pending_count = 0;

static bool is_pending(const trellis_fingerprint_t *fp)
{
    for (int i = 0; i < pending_count; i++) {
        if (trellis_fingerprint_eq(&pending_fps[i], fp))
            return true;
    }
    return false;
}

static void add_pending(const trellis_fingerprint_t *fp)
{
    if (pending_count < DISCOVERY_PENDING_MAX)
        pending_fps[pending_count++] = *fp;
    else
        pending_fps[0] = *fp;
}

typedef struct {
    uv_udp_send_t req;
    uint8_t       pkt[DISCOVERY_PKT_LEN];
} discovery_send_req_t;

static void on_send_done(uv_udp_send_t *req, int status)
{
    (void)status;
    free(req);
}

static void send_announcement(trellis_client_t *client,
                               const struct sockaddr *dest)
{
    discovery_send_req_t *sr = malloc(sizeof(*sr));
    if (!sr)
        return;

    uint8_t *p = sr->pkt;
    memcpy(p, DISCOVERY_MAGIC, DISCOVERY_MAGIC_LEN);
    p += DISCOVERY_MAGIC_LEN;
    memcpy(p, client->identity.fingerprint.bytes, TRELLIS_FINGERPRINT_LEN);
    p += TRELLIS_FINGERPRINT_LEN;
    p[0] = (uint8_t)(client->listen_port >> 8);
    p[1] = (uint8_t)(client->listen_port & 0xFF);

    uv_buf_t buf = uv_buf_init((char *)sr->pkt, DISCOVERY_PKT_LEN);
    int rc = uv_udp_send(&sr->req, &client->discovery_udp, &buf, 1,
                          dest, on_send_done);
    if (rc != 0)
        free(sr);
}

static void discovery_timer_cb(uv_timer_t *handle)
{
    trellis_client_t *client = handle->data;
    if (!client || !client->running || client->listen_port == 0)
        return;

    // Loopback broadcast -- reliable for same-machine discovery.
    struct sockaddr_in lo_bcast;
    uv_ip4_addr("127.255.255.255", DISCOVERY_PORT, &lo_bcast);
    send_announcement(client, (const struct sockaddr *)&lo_bcast);

    // Per-interface broadcast for LAN discovery across machines.
    uv_interface_address_t *addrs;
    int count;
    if (uv_interface_addresses(&addrs, &count) == 0) {
        for (int i = 0; i < count; i++) {
            if (addrs[i].address.address4.sin_family != AF_INET)
                continue;
            if (addrs[i].is_internal)
                continue;

            struct sockaddr_in addr = addrs[i].address.address4;
            struct sockaddr_in mask = addrs[i].netmask.netmask4;
            uint32_t ip = ntohl(addr.sin_addr.s_addr);
            uint32_t nm = ntohl(mask.sin_addr.s_addr);
            uint32_t bcast_ip = ip | ~nm;

            struct sockaddr_in bcast;
            memset(&bcast, 0, sizeof(bcast));
            bcast.sin_family = AF_INET;
            bcast.sin_port = htons(DISCOVERY_PORT);
            bcast.sin_addr.s_addr = htonl(bcast_ip);
            send_announcement(client, (const struct sockaddr *)&bcast);
        }
        uv_free_interface_addresses(addrs, count);
    }

    // Multicast group (may work on some networks)
    struct sockaddr_in mcast_dest;
    uv_ip4_addr(DISCOVERY_MULTICAST_GROUP, DISCOVERY_PORT, &mcast_dest);
    send_announcement(client, (const struct sockaddr *)&mcast_dest);
}

static void discovery_alloc_cb(uv_handle_t *handle, size_t suggested,
                                uv_buf_t *buf)
{
    (void)handle;
    (void)suggested;
    static char recv_buf[256];
    buf->base = recv_buf;
    buf->len = sizeof(recv_buf);
}

static void on_discovered_connected(trellis_conn_t *conn, trellis_err_t err,
                                     void *ctx)
{
    trellis_client_t *client = (trellis_client_t *)ctx;
    if (err != TRELLIS_OK || !conn || !client->running)
        return;
    trellis_client_setup_peer(client, conn, TRELLIS_ROLE_INITIATOR);
}

static void discovery_recv_cb(uv_udp_t *handle, ssize_t nread,
                               const uv_buf_t *buf, const struct sockaddr *addr,
                               unsigned flags)
{
    (void)flags;
    trellis_client_t *client = handle->data;
    if (!client || !client->running)
        return;
    if (nread <= 0 || !addr)
        return;
    if (nread < (ssize_t)DISCOVERY_PKT_LEN)
        return;

    const uint8_t *data = (const uint8_t *)buf->base;

    if (memcmp(data, DISCOVERY_MAGIC, DISCOVERY_MAGIC_LEN) != 0)
        return;

    const uint8_t *fp_bytes = data + DISCOVERY_MAGIC_LEN;
    trellis_fingerprint_t remote_fp;
    memcpy(remote_fp.bytes, fp_bytes, TRELLIS_FINGERPRINT_LEN);

    if (trellis_fingerprint_eq(&remote_fp, &client->identity.fingerprint))
        return;

    if (trellis_client_find_peer(client, &remote_fp))
        return;

    if (is_pending(&remote_fp))
        return;

    // Tie-breaking: only the node with the lower fingerprint initiates.
    if (memcmp(client->identity.fingerprint.bytes, remote_fp.bytes,
               TRELLIS_FINGERPRINT_LEN) > 0)
        return;

    uint16_t tcp_port = ((uint16_t)data[DISCOVERY_MAGIC_LEN + TRELLIS_FINGERPRINT_LEN] << 8)
                      |  (uint16_t)data[DISCOVERY_MAGIC_LEN + TRELLIS_FINGERPRINT_LEN + 1];
    if (tcp_port == 0)
        return;

    char ip[64];
    if (addr->sa_family == AF_INET) {
        const struct sockaddr_in *s4 = (const struct sockaddr_in *)addr;
        uv_ip4_name(s4, ip, sizeof(ip));
    } else if (addr->sa_family == AF_INET6) {
        const struct sockaddr_in6 *s6 = (const struct sockaddr_in6 *)addr;
        uv_ip6_name(s6, ip, sizeof(ip));
    } else {
        return;
    }

    char connect_addr[80];
    snprintf(connect_addr, sizeof(connect_addr), "%s:%u", ip, tcp_port);

    // Also check address-derived fingerprint to avoid dup with accept path.
    trellis_fingerprint_t addr_fp;
    crypto_hash_sha256(addr_fp.bytes,
                       (const unsigned char *)connect_addr,
                       strlen(connect_addr));
    if (trellis_client_find_peer(client, &addr_fp))
        return;

    add_pending(&remote_fp);
    trellis_transport_connect(client->transport, connect_addr,
                              on_discovered_connected, client);
}

trellis_err_t trellis_discovery_start(trellis_client_t *client)
{
    if (!client || client->discovery_active)
        return TRELLIS_ERR_ALREADY_EXISTS;

    int rc = uv_udp_init(&client->loop, &client->discovery_udp);
    if (rc != 0)
        return TRELLIS_ERR_TRANSPORT;
    client->discovery_udp.data = client;

    struct sockaddr_in bind_addr;
    uv_ip4_addr("0.0.0.0", DISCOVERY_PORT, &bind_addr);

    rc = uv_udp_bind(&client->discovery_udp,
                      (const struct sockaddr *)&bind_addr,
                      UV_UDP_REUSEADDR);
    if (rc != 0)
        return TRELLIS_ERR_TRANSPORT;

    uv_udp_set_broadcast(&client->discovery_udp, 1);

    // Multicast setup (best-effort)
    uv_udp_set_membership(&client->discovery_udp,
                           DISCOVERY_MULTICAST_GROUP, NULL,
                           UV_JOIN_GROUP);
    uv_udp_set_multicast_loop(&client->discovery_udp, 1);
    uv_udp_set_multicast_ttl(&client->discovery_udp, 1);
    uv_udp_set_multicast_interface(&client->discovery_udp, "0.0.0.0");

    rc = uv_udp_recv_start(&client->discovery_udp,
                            discovery_alloc_cb, discovery_recv_cb);
    if (rc != 0)
        return TRELLIS_ERR_TRANSPORT;

    rc = uv_timer_init(&client->loop, &client->discovery_timer);
    if (rc != 0)
        return TRELLIS_ERR_TRANSPORT;
    client->discovery_timer.data = client;

    uv_timer_start(&client->discovery_timer, discovery_timer_cb,
                    0, DISCOVERY_INTERVAL_MS);

    client->discovery_active = true;
    return TRELLIS_OK;
}

void trellis_discovery_stop(trellis_client_t *client)
{
    if (!client || !client->discovery_active)
        return;

    uv_timer_stop(&client->discovery_timer);
    uv_udp_recv_stop(&client->discovery_udp);

    uv_udp_set_membership(&client->discovery_udp,
                           DISCOVERY_MULTICAST_GROUP, NULL,
                           UV_LEAVE_GROUP);

    client->discovery_active = false;
}
