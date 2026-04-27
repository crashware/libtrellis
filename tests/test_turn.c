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
 * test_turn.c — STUN/TURN message parsing & TURN server unit tests
 *
 * Build:
 *   cc -I../include test_turn.c -o test_turn -ltrellis -lsodium
 *   ./test_turn
 */

#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <trellis/turn.h>

#define RUN_TEST(fn) do { printf("  %-55s", #fn "..."); fn(); printf("PASS\n"); } while(0)
#define ASSERT(cond) do { if (!(cond)) { fprintf(stderr, "FAIL at %s:%d: %s\n", __FILE__, __LINE__, #cond); abort(); } } while(0)

// --- STUN parser tests ---
static void test_stun_is_stun_valid(void)
{
    // Minimal valid STUN Binding Request: type 0x0001, length 0, magic cookie.
    uint8_t pkt[20] = {0};
    pkt[0] = 0x00; pkt[1] = 0x01;           /* type: Binding Request */
    pkt[2] = 0x00; pkt[3] = 0x00;           /* length: 0 */
    pkt[4] = 0x21; pkt[5] = 0x12;           /* magic cookie (big-endian) */
    pkt[6] = 0xA4; pkt[7] = 0x42;
    memset(pkt + 8, 0xAA, 12);              /* transaction ID */

    ASSERT(stun_is_stun(pkt, 20));
}

static void test_stun_is_stun_too_short(void)
{
    uint8_t pkt[10] = {0};
    ASSERT(!stun_is_stun(pkt, sizeof(pkt)));
    ASSERT(!stun_is_stun(NULL, 0));
    ASSERT(!stun_is_stun(pkt, 0));
}

static void test_stun_is_stun_bad_magic(void)
{
    uint8_t pkt[20] = {0};
    pkt[0] = 0x00; pkt[1] = 0x01;
    pkt[2] = 0x00; pkt[3] = 0x00;
    pkt[4] = 0xFF; pkt[5] = 0xFF; pkt[6] = 0xFF; pkt[7] = 0xFF;
    ASSERT(!stun_is_stun(pkt, 20));
}

static void test_stun_is_stun_channel_data(void)
{
    // ChannelData starts with 0x40-0x7F — should NOT be detected as STUN.
    uint8_t pkt[20] = {0};
    pkt[0] = 0x40; pkt[1] = 0x00;
    pkt[2] = 0x00; pkt[3] = 0x04;
    ASSERT(!stun_is_stun(pkt, 20));
    ASSERT(stun_is_channel_data(pkt, 4));
}

static void test_stun_is_channel_data_boundaries(void)
{
    uint8_t pkt[8] = {0};

    pkt[0] = 0x40; pkt[1] = 0x01; pkt[2] = 0x00; pkt[3] = 0x00;
    ASSERT(stun_is_channel_data(pkt, 4));

    pkt[0] = 0x7F;
    ASSERT(stun_is_channel_data(pkt, 4));

    pkt[0] = 0x3F;
    ASSERT(!stun_is_channel_data(pkt, 4));

    pkt[0] = 0x80;
    ASSERT(!stun_is_channel_data(pkt, 4));

    ASSERT(!stun_is_channel_data(pkt, 3));
}

static void test_stun_parse_binding_request(void)
{
    uint8_t pkt[20];
    pkt[0] = 0x00; pkt[1] = 0x01;           /* Binding Request */
    pkt[2] = 0x00; pkt[3] = 0x00;           /* length 0 */
    pkt[4] = 0x21; pkt[5] = 0x12;           /* magic cookie */
    pkt[6] = 0xA4; pkt[7] = 0x42;
    memset(pkt + 8, 0xBB, 12);

    stun_msg_t msg;
    memset(&msg, 0, sizeof(msg));
    ASSERT(stun_parse(pkt, sizeof(pkt), &msg) == TRELLIS_OK);
    ASSERT(msg.type == (STUN_METHOD_BINDING | STUN_CLASS_REQUEST));
    ASSERT(msg.length == 0);
    ASSERT(msg.txn_id[0] == 0xBB);

    ASSERT(!msg.has_integrity);
    ASSERT(!msg.has_fingerprint);
    ASSERT(!msg.has_error);
    ASSERT(!msg.username);
    ASSERT(!msg.realm);
    ASSERT(!msg.nonce);
}

static void test_stun_parse_with_username_attr(void)
{
    // Build a binding request with a USERNAME attribute.
    const char *user = "1234567890:test";
    size_t ulen = strlen(user);
    size_t padded = (ulen + 3) & ~(size_t)3;
    size_t total = 20 + 4 + padded;  /* header + attr-header + padded value */

    uint8_t *pkt = calloc(1, total);
    ASSERT(pkt);

    // STUN header.
    pkt[0] = 0x00; pkt[1] = 0x01;           /* Binding Request */
    pkt[2] = (uint8_t)((4 + padded) >> 8);
    pkt[3] = (uint8_t)(4 + padded);          /* attrs length */
    pkt[4] = 0x21; pkt[5] = 0x12;
    pkt[6] = 0xA4; pkt[7] = 0x42;
    memset(pkt + 8, 0xCC, 12);

    // USERNAME attribute: type 0x0006.
    pkt[20] = 0x00; pkt[21] = 0x06;
    pkt[22] = (uint8_t)(ulen >> 8);
    pkt[23] = (uint8_t)(ulen);
    memcpy(pkt + 24, user, ulen);

    stun_msg_t msg;
    memset(&msg, 0, sizeof(msg));
    ASSERT(stun_parse(pkt, total, &msg) == TRELLIS_OK);
    ASSERT(msg.username != NULL);
    ASSERT(msg.username_len == ulen);
    ASSERT(memcmp(msg.username, user, ulen) == 0);

    free(pkt);
}

static void test_stun_parse_truncated(void)
{
    // Header claims 100 bytes of attributes but packet is only 20.
    uint8_t pkt[20];
    pkt[0] = 0x00; pkt[1] = 0x01;
    pkt[2] = 0x00; pkt[3] = 100;
    pkt[4] = 0x21; pkt[5] = 0x12;
    pkt[6] = 0xA4; pkt[7] = 0x42;
    memset(pkt + 8, 0, 12);

    stun_msg_t msg;
    memset(&msg, 0, sizeof(msg));
    ASSERT(stun_parse(pkt, sizeof(pkt), &msg) != TRELLIS_OK);
}

static void test_stun_parse_null_inputs(void)
{
    stun_msg_t msg;
    ASSERT(stun_parse(NULL, 20, &msg) != TRELLIS_OK);
    uint8_t pkt[20] = {0};
    ASSERT(stun_parse(pkt, 0, &msg) != TRELLIS_OK);
}

// --- STUN builder tests ---
static void test_stun_build_response_minimal(void)
{
    uint8_t txn_id[STUN_TXN_ID_LEN];
    memset(txn_id, 0xDD, STUN_TXN_ID_LEN);

    uint8_t buf[256];
    size_t n = stun_build_response(
        STUN_METHOD_BINDING | STUN_CLASS_SUCCESS,
        txn_id, NULL, 0, NULL, 0, buf, sizeof(buf));
    ASSERT(n >= STUN_HEADER_LEN);

    // Parse back.
    stun_msg_t msg;
    memset(&msg, 0, sizeof(msg));
    ASSERT(stun_parse(buf, n, &msg) == TRELLIS_OK);
    ASSERT(msg.type == (STUN_METHOD_BINDING | STUN_CLASS_SUCCESS));
    ASSERT(memcmp(msg.txn_id, txn_id, STUN_TXN_ID_LEN) == 0);
}

static void test_stun_build_response_with_attrs(void)
{
    uint8_t txn_id[STUN_TXN_ID_LEN];
    memset(txn_id, 0xEE, STUN_TXN_ID_LEN);

    // Build a LIFETIME attribute.
    uint8_t attr_buf[64];
    size_t attr_len = stun_attr_lifetime(600, attr_buf, sizeof(attr_buf));
    ASSERT(attr_len == 8); /* 4 type+len + 4 value */

    uint8_t buf[256];
    size_t n = stun_build_response(
        TURN_METHOD_ALLOCATE | STUN_CLASS_SUCCESS,
        txn_id, attr_buf, attr_len, NULL, 0, buf, sizeof(buf));
    ASSERT(n > STUN_HEADER_LEN);

    stun_msg_t msg;
    memset(&msg, 0, sizeof(msg));
    ASSERT(stun_parse(buf, n, &msg) == TRELLIS_OK);
    ASSERT(msg.has_lifetime);
    ASSERT(msg.lifetime == 600);
}

static void test_stun_build_buffer_too_small(void)
{
    uint8_t txn[STUN_TXN_ID_LEN] = {0};
    uint8_t buf[4]; /* too small for a header */
    size_t n = stun_build_response(
        STUN_METHOD_BINDING | STUN_CLASS_SUCCESS,
        txn, NULL, 0, NULL, 0, buf, sizeof(buf));
    ASSERT(n == 0);
}

static void test_stun_attr_error_code(void)
{
    uint8_t buf[128];
    size_t n = stun_attr_error_code(401, "Unauthorized", buf, sizeof(buf));
    ASSERT(n > 0);
    // Type should be ERROR-CODE (0x0009)
    ASSERT(buf[0] == 0x00 && buf[1] == 0x09);
}

static void test_stun_attr_software(void)
{
    uint8_t buf[128];
    size_t n = stun_attr_software("vine-relay/1.0", buf, sizeof(buf));
    ASSERT(n > 0);
    ASSERT(buf[0] == 0x80 && buf[1] == 0x22);
}

static void test_stun_attr_nonce_roundtrip(void)
{
    uint8_t nonce[TURN_NONCE_LEN];
    memset(nonce, 0xAB, TURN_NONCE_LEN);

    uint8_t buf[128];
    size_t n = stun_attr_nonce(nonce, TURN_NONCE_LEN, buf, sizeof(buf));
    ASSERT(n > 0);
    ASSERT(buf[0] == 0x00 && buf[1] == 0x15);
}

static void test_stun_attr_xor_mapped_address(void)
{
    stun_addr_t addr = {0};
    addr.family = 0x01; /* IPv4 */
    addr.port = 12345;
    addr.ip.v4[0] = 192; addr.ip.v4[1] = 168;
    addr.ip.v4[2] = 1;   addr.ip.v4[3] = 100;

    uint8_t txn[STUN_TXN_ID_LEN];
    memset(txn, 0x11, STUN_TXN_ID_LEN);

    uint8_t buf[64];
    size_t n = stun_attr_xor_mapped_address(&addr, txn, buf, sizeof(buf));
    ASSERT(n > 0);
    ASSERT(buf[0] == 0x00 && buf[1] == 0x20); /* XOR-MAPPED-ADDRESS */
}

// --- TURN server tests ---
static uint8_t g_send_buf[4096];
static size_t  g_send_len = 0;
static stun_addr_t g_send_dst;

static void test_send_cb(const uint8_t *data, size_t len,
                         const stun_addr_t *dst, void *ctx)
{
    (void)ctx;
    if (len > sizeof(g_send_buf)) len = sizeof(g_send_buf);
    memcpy(g_send_buf, data, len);
    g_send_len = len;
    g_send_dst = *dst;
}

static uint8_t  g_relay_data[4096];
static size_t   g_relay_len = 0;
static uint64_t g_relay_alloc_id = 0;

static void test_relay_cb(uint64_t alloc_id,
                          const stun_addr_t *peer_addr,
                          const uint8_t *data, size_t len,
                          void *ctx)
{
    (void)peer_addr; (void)ctx;
    g_relay_alloc_id = alloc_id;
    if (len > sizeof(g_relay_data)) len = sizeof(g_relay_data);
    memcpy(g_relay_data, data, len);
    g_relay_len = len;
}

static void test_turn_server_init_destroy(void)
{
    turn_server_t srv;
    stun_addr_t ext = {0};
    ext.family = 0x01;
    ext.port = 443;
    ext.ip.v4[0] = 1; ext.ip.v4[1] = 2; ext.ip.v4[2] = 3; ext.ip.v4[3] = 4;

    ASSERT(turn_server_init(&srv, "test.realm", "test-secret", &ext) == TRELLIS_OK);
    ASSERT(strcmp(srv.realm, "test.realm") == 0);
    ASSERT(srv.secret != NULL);
    ASSERT(strcmp(srv.secret, "test-secret") == 0);
    ASSERT(srv.secret_len == 11);
    ASSERT(srv.external_addr.port == 443);
    ASSERT(srv.alloc_count == 0);

    turn_server_destroy(&srv);
}

static void test_turn_server_binding_request(void)
{
    turn_server_t srv;
    stun_addr_t ext = {0};
    ext.family = 0x01; ext.port = 443;

    ASSERT(turn_server_init(&srv, "test.realm", "test-secret", &ext) == TRELLIS_OK);
    srv.send_cb = test_send_cb;
    srv.send_ctx = NULL;

    // Build a STUN Binding Request.
    uint8_t pkt[20];
    pkt[0] = 0x00; pkt[1] = 0x01;
    pkt[2] = 0x00; pkt[3] = 0x00;
    pkt[4] = 0x21; pkt[5] = 0x12;
    pkt[6] = 0xA4; pkt[7] = 0x42;
    memset(pkt + 8, 0xFF, 12);

    stun_addr_t client = {0};
    client.family = 0x01; client.port = 50000;
    client.ip.v4[0] = 10; client.ip.v4[1] = 0;
    client.ip.v4[2] = 0; client.ip.v4[3] = 1;

    g_send_len = 0;
    ASSERT(turn_server_process(&srv, pkt, sizeof(pkt), &client, 17) == TRELLIS_OK);

    // Should have sent a Binding Success response.
    ASSERT(g_send_len > 0);

    stun_msg_t resp;
    memset(&resp, 0, sizeof(resp));
    ASSERT(stun_parse(g_send_buf, g_send_len, &resp) == TRELLIS_OK);
    ASSERT(resp.type == (STUN_METHOD_BINDING | STUN_CLASS_SUCCESS));
    ASSERT(resp.has_xor_mapped_addr);

    turn_server_destroy(&srv);
}

static void test_turn_server_allocate_unauth(void)
{
    turn_server_t srv;
    stun_addr_t ext = {0};
    ext.family = 0x01; ext.port = 443;

    ASSERT(turn_server_init(&srv, "test.realm", "test-secret", &ext) == TRELLIS_OK);
    srv.send_cb = test_send_cb;
    srv.send_ctx = NULL;

    // Build an Allocate Request WITHOUT credentials.
    uint8_t req_transport[8] = {0};
    req_transport[0] = 0x00; req_transport[1] = 0x19; /* REQUESTED-TRANSPORT */
    req_transport[2] = 0x00; req_transport[3] = 0x04;
    req_transport[4] = 17; /* UDP */

    uint8_t pkt[28];
    pkt[0] = 0x00; pkt[1] = 0x03; /* Allocate Request */
    pkt[2] = 0x00; pkt[3] = 0x08;
    pkt[4] = 0x21; pkt[5] = 0x12;
    pkt[6] = 0xA4; pkt[7] = 0x42;
    memset(pkt + 8, 0xAA, 12);
    memcpy(pkt + 20, req_transport, 8);

    stun_addr_t client = {0};
    client.family = 0x01; client.port = 50001;

    g_send_len = 0;
    turn_server_process(&srv, pkt, sizeof(pkt), &client, 17);

    // Should receive 401 Unauthorized with REALM and NONCE.
    ASSERT(g_send_len > 0);
    stun_msg_t resp;
    memset(&resp, 0, sizeof(resp));
    ASSERT(stun_parse(g_send_buf, g_send_len, &resp) == TRELLIS_OK);
    ASSERT(resp.has_error);
    ASSERT(resp.error_code == 401);
    ASSERT(resp.realm != NULL);
    ASSERT(resp.nonce != NULL);

    turn_server_destroy(&srv);
}

static void test_turn_server_tick_no_crash(void)
{
    turn_server_t srv;
    stun_addr_t ext = {0};
    ext.family = 0x01; ext.port = 443;

    ASSERT(turn_server_init(&srv, "test.realm", "test-secret", &ext) == TRELLIS_OK);

    // tick with no allocations should be a no-op.
    turn_server_tick(&srv);

    turn_server_destroy(&srv);
}

static void test_turn_alloc_has_permission_empty(void)
{
    turn_allocation_t alloc;
    memset(&alloc, 0, sizeof(alloc));
    alloc.active = true;

    stun_addr_t peer = {0};
    peer.family = 0x01; peer.port = 9999;
    ASSERT(!turn_alloc_has_permission(&alloc, &peer));
}

static void test_turn_alloc_find_channel_empty(void)
{
    turn_allocation_t alloc;
    memset(&alloc, 0, sizeof(alloc));
    alloc.active = true;

    stun_addr_t peer = {0};
    peer.family = 0x01; peer.port = 8888;
    ASSERT(turn_alloc_find_channel(&alloc, &peer) == 0);
}

static void test_turn_alloc_find_peer_by_channel_empty(void)
{
    turn_allocation_t alloc;
    memset(&alloc, 0, sizeof(alloc));
    alloc.active = true;

    stun_addr_t out = {0};
    ASSERT(!turn_alloc_find_peer_by_channel(&alloc, 0x4000, &out));
}

// --- Additional coverage tests ---
static void test_stun_build_response_with_integrity(void)
{
    uint8_t txn_id[STUN_TXN_ID_LEN];
    memset(txn_id, 0x99, STUN_TXN_ID_LEN);

    uint8_t key[16] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
                       0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10};

    uint8_t attr_buf[64];
    size_t attr_len = stun_attr_lifetime(300, attr_buf, sizeof(attr_buf));

    uint8_t buf[512];
    size_t n = stun_build_response(
        TURN_METHOD_ALLOCATE | STUN_CLASS_SUCCESS,
        txn_id, attr_buf, attr_len, key, sizeof(key), buf, sizeof(buf));
    ASSERT(n > STUN_HEADER_LEN + attr_len);

    // Parse it back - should have integrity and fingerprint.
    stun_msg_t msg;
    memset(&msg, 0, sizeof(msg));
    ASSERT(stun_parse(buf, n, &msg) == TRELLIS_OK);
    ASSERT(msg.has_integrity);
    ASSERT(msg.has_fingerprint);

    // Verify integrity with the same key.
    ASSERT(stun_verify_integrity(&msg, key, sizeof(key)));

    // Verify with wrong key fails.
    uint8_t bad_key[16] = {0};
    ASSERT(!stun_verify_integrity(&msg, bad_key, sizeof(bad_key)));
}

static void test_stun_verify_integrity_no_integrity(void)
{
    // A message without MESSAGE-INTEGRITY should fail verify.
    uint8_t pkt[20];
    pkt[0] = 0x00; pkt[1] = 0x01;
    pkt[2] = 0x00; pkt[3] = 0x00;
    pkt[4] = 0x21; pkt[5] = 0x12;
    pkt[6] = 0xA4; pkt[7] = 0x42;
    memset(pkt + 8, 0xAA, 12);

    stun_msg_t msg;
    memset(&msg, 0, sizeof(msg));
    ASSERT(stun_parse(pkt, sizeof(pkt), &msg) == TRELLIS_OK);
    ASSERT(!msg.has_integrity);

    uint8_t key[16] = {0};
    ASSERT(!stun_verify_integrity(&msg, key, sizeof(key)));
}

static void test_turn_server_process_null_send_cb(void)
{
    turn_server_t srv;
    stun_addr_t ext = {0};
    ext.family = 0x01; ext.port = 443;

    ASSERT(turn_server_init(&srv, "test.realm", "test-secret", &ext) == TRELLIS_OK);
    srv.send_cb = NULL; /* deliberately NULL */

    uint8_t pkt[20];
    pkt[0] = 0x00; pkt[1] = 0x01;
    pkt[2] = 0x00; pkt[3] = 0x00;
    pkt[4] = 0x21; pkt[5] = 0x12;
    pkt[6] = 0xA4; pkt[7] = 0x42;
    memset(pkt + 8, 0xCC, 12);

    stun_addr_t client = {0};
    client.family = 0x01; client.port = 50002;

    // Should not crash even with NULL send_cb.
    trellis_err_t err = turn_server_process(&srv, pkt, sizeof(pkt), &client, 17);
    ASSERT(err == TRELLIS_OK);

    turn_server_destroy(&srv);
}

int main(void)
{
    printf("test_turn: STUN/TURN unit tests\n");
    printf("────────────────────────────────────────────────────────────────\n");

    printf("\n  STUN detection:\n");
    RUN_TEST(test_stun_is_stun_valid);
    RUN_TEST(test_stun_is_stun_too_short);
    RUN_TEST(test_stun_is_stun_bad_magic);
    RUN_TEST(test_stun_is_stun_channel_data);
    RUN_TEST(test_stun_is_channel_data_boundaries);

    printf("\n  STUN parsing:\n");
    RUN_TEST(test_stun_parse_binding_request);
    RUN_TEST(test_stun_parse_with_username_attr);
    RUN_TEST(test_stun_parse_truncated);
    RUN_TEST(test_stun_parse_null_inputs);

    printf("\n  STUN building:\n");
    RUN_TEST(test_stun_build_response_minimal);
    RUN_TEST(test_stun_build_response_with_attrs);
    RUN_TEST(test_stun_build_buffer_too_small);

    printf("\n  STUN attributes:\n");
    RUN_TEST(test_stun_attr_error_code);
    RUN_TEST(test_stun_attr_software);
    RUN_TEST(test_stun_attr_nonce_roundtrip);
    RUN_TEST(test_stun_attr_xor_mapped_address);

    printf("\n  STUN integrity:\n");
    RUN_TEST(test_stun_build_response_with_integrity);
    RUN_TEST(test_stun_verify_integrity_no_integrity);

    printf("\n  TURN server:\n");
    RUN_TEST(test_turn_server_init_destroy);
    RUN_TEST(test_turn_server_binding_request);
    RUN_TEST(test_turn_server_allocate_unauth);
    RUN_TEST(test_turn_server_tick_no_crash);
    RUN_TEST(test_turn_server_process_null_send_cb);
    RUN_TEST(test_turn_alloc_has_permission_empty);
    RUN_TEST(test_turn_alloc_find_channel_empty);
    RUN_TEST(test_turn_alloc_find_peer_by_channel_empty);

    printf("\n────────────────────────────────────────────────────────────────\n");
    printf("All STUN/TURN tests passed.\n");
    return 0;
}
