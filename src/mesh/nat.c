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

/*
 * STUN message format (RFC 5389):
 *   Byte 0-1:  Message type (0x0001 = Binding Request)
 *   Byte 2-3:  Message length (body, excluding 20-byte header)
 *   Byte 4-7:  Magic cookie (0x2112A442)
 *   Byte 8-19: Transaction ID (96 bits)
 */
#define STUN_MAGIC_COOKIE       0x2112A442
#define STUN_BINDING_REQUEST    0x0001
#define STUN_BINDING_RESPONSE   0x0101
#define STUN_HEADER_LEN         20
#define STUN_ATTR_MAPPED_ADDR   0x0001
#define STUN_ATTR_XOR_MAPPED    0x0020

trellis_nat_t *trellis_nat_new(uv_loop_t *loop)
{
    if (!loop)
        return NULL;

    trellis_nat_t *nat = calloc(1, sizeof(trellis_nat_t));
    if (!nat)
        return NULL;

    nat->loop = loop;
    nat->detected_type = NAT_TYPE_UNKNOWN;
    nat->resolved = false;

    uv_udp_init(loop, &nat->stun_handle);
    nat->stun_handle.data = nat;

    return nat;
}

void trellis_nat_free(trellis_nat_t *nat)
{
    if (!nat)
        return;
    uv_close((uv_handle_t *)&nat->stun_handle, NULL);
    free(nat);
}

static void build_stun_request(uint8_t buf[STUN_HEADER_LEN],
                               uint8_t txn_id[12])
{
    memset(buf, 0, STUN_HEADER_LEN);

    buf[0] = (STUN_BINDING_REQUEST >> 8) & 0xff;
    buf[1] = STUN_BINDING_REQUEST & 0xff;

    // Length = 0 (no attributes in request)
    buf[2] = 0;
    buf[3] = 0;

    buf[4] = (STUN_MAGIC_COOKIE >> 24) & 0xff;
    buf[5] = (STUN_MAGIC_COOKIE >> 16) & 0xff;
    buf[6] = (STUN_MAGIC_COOKIE >> 8) & 0xff;
    buf[7] = STUN_MAGIC_COOKIE & 0xff;

    randombytes_buf(txn_id, 12);
    memcpy(buf + 8, txn_id, 12);
}

static trellis_err_t parse_stun_response(const uint8_t *data, size_t len,
                                         const uint8_t expected_txn[12],
                                         char *addr_out, size_t addr_cap,
                                         uint16_t *port_out)
{
    if (len < STUN_HEADER_LEN)
        return TRELLIS_ERR_DHT;

    uint16_t msg_type = ((uint16_t)data[0] << 8) | data[1];
    if (msg_type != STUN_BINDING_RESPONSE)
        return TRELLIS_ERR_DHT;

    if (sodium_memcmp(data + 8, expected_txn, 12) != 0)
        return TRELLIS_ERR_DHT;

    uint16_t body_len = ((uint16_t)data[2] << 8) | data[3];
    if (STUN_HEADER_LEN + body_len > len)
        return TRELLIS_ERR_DHT;

    size_t offset = STUN_HEADER_LEN;
    while (offset + 4 <= STUN_HEADER_LEN + body_len) {
        uint16_t attr_type = ((uint16_t)data[offset] << 8) | data[offset + 1];
        uint16_t attr_len = ((uint16_t)data[offset + 2] << 8) | data[offset + 3];
        offset += 4;

        if (offset + attr_len > len)
            break;

        if (attr_type == STUN_ATTR_XOR_MAPPED && attr_len >= 8) {
            uint16_t xport = (((uint16_t)data[offset + 2] << 8) |
                              data[offset + 3]) ^ 0x2112;
            uint32_t xaddr = ((uint32_t)data[offset + 4] << 24) |
                             ((uint32_t)data[offset + 5] << 16) |
                             ((uint32_t)data[offset + 6] << 8) |
                             data[offset + 7];
            xaddr ^= STUN_MAGIC_COOKIE;

            snprintf(addr_out, addr_cap, "%u.%u.%u.%u",
                     (xaddr >> 24) & 0xff, (xaddr >> 16) & 0xff,
                     (xaddr >> 8) & 0xff, xaddr & 0xff);
            *port_out = xport;
            return TRELLIS_OK;
        }

        if (attr_type == STUN_ATTR_MAPPED_ADDR && attr_len >= 8) {
            uint16_t port = ((uint16_t)data[offset + 2] << 8) |
                            data[offset + 3];
            snprintf(addr_out, addr_cap, "%u.%u.%u.%u",
                     data[offset + 4], data[offset + 5],
                     data[offset + 6], data[offset + 7]);
            *port_out = port;
            return TRELLIS_OK;
        }

        // Align attribute length to 4-byte boundary.
        offset += attr_len;
        if (attr_len % 4 != 0)
            offset += 4 - (attr_len % 4);
    }

    return TRELLIS_ERR_DHT;
}

typedef struct {
    trellis_nat_t *nat;
    uint8_t        txn_id[12];
} stun_ctx_t;

static void alloc_cb(uv_handle_t *handle, size_t suggested, uv_buf_t *buf)
{
    (void)handle;
    buf->base = malloc(suggested);
    buf->len = buf->base ? suggested : 0;
}

static void recv_cb(uv_udp_t *handle, ssize_t nread,
                    const uv_buf_t *buf,
                    const struct sockaddr *addr, unsigned flags)
{
    (void)addr;
    (void)flags;
    stun_ctx_t *ctx = handle->data;

    if (nread > 0 && ctx) {
        char mapped[256] = {0};
        uint16_t port = 0;
        trellis_err_t err = parse_stun_response(
            (const uint8_t *)buf->base, (size_t)nread,
            ctx->txn_id, mapped, sizeof(mapped), &port);

        if (err == TRELLIS_OK) {
            size_t len = strlen(mapped);
            if (len < sizeof(ctx->nat->mapped_addr))
                memcpy(ctx->nat->mapped_addr, mapped, len + 1);
            ctx->nat->mapped_port = port;
            ctx->nat->resolved = true;
            ctx->nat->detected_type = NAT_TYPE_FULL_CONE;
        }
    }

    free(buf->base);
}

trellis_err_t trellis_nat_probe(trellis_nat_t *nat,
                                const char *stun_server, uint16_t port)
{
    if (!nat || !stun_server)
        return TRELLIS_ERR_INVALID_ARG;

    struct sockaddr_in server_addr;
    if (uv_ip4_addr(stun_server, port, &server_addr) != 0)
        return TRELLIS_ERR_INVALID_ARG;

    stun_ctx_t *ctx = calloc(1, sizeof(stun_ctx_t));
    if (!ctx)
        return TRELLIS_ERR_NOMEM;
    ctx->nat = nat;

    uint8_t request[STUN_HEADER_LEN];
    build_stun_request(request, ctx->txn_id);

    nat->stun_handle.data = ctx;

    int rc = uv_udp_recv_start(&nat->stun_handle, alloc_cb, recv_cb);
    if (rc != 0) {
        free(ctx);
        return TRELLIS_ERR_DHT;
    }

    uv_buf_t send_buf = uv_buf_init((char *)request, STUN_HEADER_LEN);
    uv_udp_send_t *send_req = calloc(1, sizeof(uv_udp_send_t));
    if (!send_req) {
        uv_udp_recv_stop(&nat->stun_handle);
        free(ctx);
        return TRELLIS_ERR_NOMEM;
    }

    rc = uv_udp_send(send_req, &nat->stun_handle, &send_buf, 1,
                      (const struct sockaddr *)&server_addr, NULL);
    if (rc != 0) {
        uv_udp_recv_stop(&nat->stun_handle);
        free(send_req);
        free(ctx);
        return TRELLIS_ERR_DHT;
    }

    return TRELLIS_OK;
}

nat_type_t trellis_nat_detected_type(const trellis_nat_t *nat)
{
    return nat ? nat->detected_type : NAT_TYPE_UNKNOWN;
}

const char *trellis_nat_mapped_address(const trellis_nat_t *nat)
{
    if (!nat || !nat->resolved)
        return NULL;
    return nat->mapped_addr;
}

uint16_t trellis_nat_mapped_port(const trellis_nat_t *nat)
{
    if (!nat || !nat->resolved)
        return 0;
    return nat->mapped_port;
}
