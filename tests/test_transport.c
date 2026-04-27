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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <uv.h>
#include <trellis/trellis.h>

#define RUN_TEST(fn) do { printf("  %-50s", #fn "..."); fn(); printf("PASS\n"); } while(0)
#define ASSERT(cond) do { if (!(cond)) { fprintf(stderr, "FAIL at %s:%d: %s\n", __FILE__, __LINE__, #cond); abort(); } } while(0)

static void timer_stop_cb(uv_timer_t *handle)
{
    uv_stop(handle->loop);
}

static void run_loop_ms(uv_loop_t *loop, int ms)
{
    uv_timer_t timer;
    uv_timer_init(loop, &timer);
    uv_timer_start(&timer, timer_stop_cb, ms, 0);
    uv_run(loop, UV_RUN_DEFAULT);
    uv_close((uv_handle_t *)&timer, NULL);
    uv_run(loop, UV_RUN_NOWAIT);
}

static void close_walk_cb(uv_handle_t *h, void *arg)
{
    (void)arg;
    if (!uv_is_closing(h))
        uv_close(h, NULL);
}

static void drain_loop(uv_loop_t *loop)
{
    uv_walk(loop, close_walk_cb, NULL);
    while (uv_loop_alive(loop))
        uv_run(loop, UV_RUN_ONCE);
}

// --- TCP: listen + connect ---
static int g_accepted;

static void tcp_accept_cb(trellis_conn_t *conn, void *ctx)
{
    (void)ctx;
    g_accepted = 1;
    ASSERT(conn != NULL);
    ASSERT(trellis_conn_remote_addr(conn) != NULL);
    trellis_conn_close(conn, NULL, NULL);
}

static int g_connected;

static void tcp_connect_cb(trellis_conn_t *conn, trellis_err_t err, void *ctx)
{
    (void)ctx;
    g_connected = 1;
    ASSERT(err == TRELLIS_OK);
    ASSERT(conn != NULL);
    trellis_conn_close(conn, NULL, NULL);
}

static void test_tcp_listen_connect(void)
{
    uv_loop_t loop;
    uv_loop_init(&loop);
    trellis_transport_t *server = trellis_transport_tcp_new(&loop);
    trellis_transport_t *client = trellis_transport_tcp_new(&loop);
    ASSERT(server && client);

    g_accepted = 0;
    g_connected = 0;

    ASSERT(trellis_transport_listen(server, "127.0.0.1:0", tcp_accept_cb, NULL) == TRELLIS_OK);
    int port = trellis_transport_bound_port(server);
    ASSERT(port > 0);
    char addr[64];
    snprintf(addr, sizeof(addr), "127.0.0.1:%d", port);
    ASSERT(trellis_transport_connect(client, addr, tcp_connect_cb, NULL) == TRELLIS_OK);

    run_loop_ms(&loop, 200);

    drain_loop(&loop);
    trellis_transport_free(server);
    trellis_transport_free(client);
    uv_loop_close(&loop);
}

// --- TCP: send + receive ---
static uint8_t g_recv_buf[4096];
static size_t  g_recv_len;
static int     g_recv_done;

static void tcp_recv_cb(trellis_conn_t *conn, const uint8_t *data, size_t len,
                         trellis_err_t err, void *ctx)
{
    (void)conn; (void)ctx;
    if (err == TRELLIS_OK && len > 0) {
        memcpy(g_recv_buf + g_recv_len, data, len);
        g_recv_len += len;
        g_recv_done = 1;
    }
}

static trellis_conn_t *g_server_conn;

static void tcp_accept_recv_cb(trellis_conn_t *conn, void *ctx)
{
    (void)ctx;
    g_server_conn = conn;
    trellis_conn_recv_start(conn, tcp_recv_cb, NULL);
}

static trellis_conn_t *g_client_conn;

static void tcp_connect_send_cb(trellis_conn_t *conn, trellis_err_t err, void *ctx)
{
    (void)ctx;
    ASSERT(err == TRELLIS_OK);
    g_client_conn = conn;
}

static void test_tcp_send_recv(void)
{
    uv_loop_t loop;
    uv_loop_init(&loop);
    trellis_transport_t *server = trellis_transport_tcp_new(&loop);
    trellis_transport_t *client = trellis_transport_tcp_new(&loop);

    g_recv_len = 0;
    g_recv_done = 0;
    g_server_conn = NULL;
    g_client_conn = NULL;

    ASSERT(trellis_transport_listen(server, "127.0.0.1:0", tcp_accept_recv_cb, NULL) == TRELLIS_OK);
    {
        int port = trellis_transport_bound_port(server);
        char addr[64]; snprintf(addr, sizeof(addr), "127.0.0.1:%d", port);
        ASSERT(trellis_transport_connect(client, addr, tcp_connect_send_cb, NULL) == TRELLIS_OK);
    }

    run_loop_ms(&loop, 100);

    if (g_client_conn) {
        const uint8_t payload[] = "hello trellis transport";
        trellis_conn_send(g_client_conn, payload, sizeof(payload) - 1, NULL, NULL);
        run_loop_ms(&loop, 200);

        ASSERT(g_recv_done);
        ASSERT(g_recv_len == sizeof(payload) - 1);
        ASSERT(memcmp(g_recv_buf, payload, g_recv_len) == 0);
    }

    drain_loop(&loop);
    trellis_transport_free(server);
    trellis_transport_free(client);
    uv_loop_close(&loop);
}

// --- TCP: multiple frames ---
static int g_frame_count;

static void tcp_multi_recv_cb(trellis_conn_t *conn, const uint8_t *data, size_t len,
                               trellis_err_t err, void *ctx)
{
    (void)conn; (void)ctx;
    if (err == TRELLIS_OK && len > 0)
        g_frame_count++;
}

static trellis_conn_t *g_multi_server;
static trellis_conn_t *g_multi_client;

static void tcp_multi_accept(trellis_conn_t *conn, void *ctx)
{
    (void)ctx;
    g_multi_server = conn;
    trellis_conn_recv_start(conn, tcp_multi_recv_cb, NULL);
}

static void tcp_multi_connect(trellis_conn_t *conn, trellis_err_t err, void *ctx)
{
    (void)ctx;
    ASSERT(err == TRELLIS_OK);
    g_multi_client = conn;
}

static void test_tcp_multiple_frames(void)
{
    uv_loop_t loop;
    uv_loop_init(&loop);
    trellis_transport_t *server = trellis_transport_tcp_new(&loop);
    trellis_transport_t *client = trellis_transport_tcp_new(&loop);

    g_frame_count = 0;
    g_multi_server = NULL;
    g_multi_client = NULL;

    ASSERT(trellis_transport_listen(server, "127.0.0.1:0", tcp_multi_accept, NULL) == TRELLIS_OK);
    {
        int port = trellis_transport_bound_port(server);
        char addr[64]; snprintf(addr, sizeof(addr), "127.0.0.1:%d", port);
        ASSERT(trellis_transport_connect(client, addr, tcp_multi_connect, NULL) == TRELLIS_OK);
    }
    run_loop_ms(&loop, 100);

    if (g_multi_client) {
        for (int i = 0; i < 5; i++) {
            uint8_t buf[32];
            snprintf((char *)buf, sizeof(buf), "frame-%d", i);
            trellis_conn_send(g_multi_client, buf, strlen((char *)buf), NULL, NULL);
        }
        run_loop_ms(&loop, 300);
        ASSERT(g_frame_count >= 1);
    }

    drain_loop(&loop);
    trellis_transport_free(server);
    trellis_transport_free(client);
    uv_loop_close(&loop);
}

// --- TCP: close ---
static int g_close_called;

static void tcp_close_done(trellis_conn_t *conn, void *ctx)
{
    (void)conn; (void)ctx;
    g_close_called = 1;
}

static trellis_conn_t *g_close_server;
static trellis_conn_t *g_close_client;

static void tcp_close_accept(trellis_conn_t *conn, void *ctx)
{
    (void)ctx;
    g_close_server = conn;
}

static void tcp_close_connect(trellis_conn_t *conn, trellis_err_t err, void *ctx)
{
    (void)ctx;
    ASSERT(err == TRELLIS_OK);
    g_close_client = conn;
}

static void test_tcp_close(void)
{
    uv_loop_t loop;
    uv_loop_init(&loop);
    trellis_transport_t *server = trellis_transport_tcp_new(&loop);
    trellis_transport_t *client = trellis_transport_tcp_new(&loop);

    g_close_called = 0;
    g_close_server = NULL;
    g_close_client = NULL;

    ASSERT(trellis_transport_listen(server, "127.0.0.1:0", tcp_close_accept, NULL) == TRELLIS_OK);
    {
        int port = trellis_transport_bound_port(server);
        char addr[64]; snprintf(addr, sizeof(addr), "127.0.0.1:%d", port);
        ASSERT(trellis_transport_connect(client, addr, tcp_close_connect, NULL) == TRELLIS_OK);
    }
    run_loop_ms(&loop, 100);

    if (g_close_client) {
        trellis_conn_close(g_close_client, tcp_close_done, NULL);
        run_loop_ms(&loop, 100);
        ASSERT(g_close_called);
    }

    drain_loop(&loop);
    trellis_transport_free(server);
    trellis_transport_free(client);
    uv_loop_close(&loop);
}

// --- UDP: send + receive ---
static uint8_t g_udp_recv[256];
static size_t  g_udp_recv_len;

static void udp_recv_cb(trellis_conn_t *conn, const uint8_t *data, size_t len,
                         trellis_err_t err, void *ctx)
{
    (void)conn; (void)ctx;
    if (err == TRELLIS_OK && len > 0) {
        memcpy(g_udp_recv, data, len);
        g_udp_recv_len = len;
    }
}

static trellis_conn_t *g_udp_server_conn;

static void udp_accept_cb(trellis_conn_t *conn, void *ctx)
{
    (void)ctx;
    g_udp_server_conn = conn;
    trellis_conn_recv_start(conn, udp_recv_cb, NULL);
}

static trellis_conn_t *g_udp_client_conn;

static void udp_connect_cb(trellis_conn_t *conn, trellis_err_t err, void *ctx)
{
    (void)ctx;
    ASSERT(err == TRELLIS_OK);
    g_udp_client_conn = conn;
}

static void test_udp_send_recv(void)
{
    uv_loop_t loop;
    uv_loop_init(&loop);
    trellis_transport_t *server = trellis_transport_udp_new(&loop);
    trellis_transport_t *client = trellis_transport_udp_new(&loop);

    g_udp_recv_len = 0;
    g_udp_server_conn = NULL;
    g_udp_client_conn = NULL;

    ASSERT(trellis_transport_listen(server, "127.0.0.1:0", udp_accept_cb, NULL) == TRELLIS_OK);
    int port = trellis_transport_bound_port(server);
    char addr[64]; snprintf(addr, sizeof(addr), "127.0.0.1:%d", port);
    ASSERT(trellis_transport_connect(client, addr, udp_connect_cb, NULL) == TRELLIS_OK);
    run_loop_ms(&loop, 100);

    if (g_udp_client_conn) {
        const uint8_t msg[] = "udp datagram";
        trellis_conn_send(g_udp_client_conn, msg, sizeof(msg) - 1, NULL, NULL);
        run_loop_ms(&loop, 200);

        ASSERT(g_udp_recv_len == sizeof(msg) - 1);
        ASSERT(memcmp(g_udp_recv, msg, g_udp_recv_len) == 0);
    }

    drain_loop(&loop);
    trellis_transport_free(server);
    trellis_transport_free(client);
    uv_loop_close(&loop);
}

static void test_transport_protocol_name(void)
{
    uv_loop_t loop;
    uv_loop_init(&loop);

    trellis_transport_t *tcp = trellis_transport_tcp_new(&loop);
    ASSERT(tcp);
    ASSERT(strcmp(trellis_transport_protocol(tcp), "tcp") == 0);
    trellis_transport_free(tcp);

    trellis_transport_t *udp = trellis_transport_udp_new(&loop);
    ASSERT(udp);
    ASSERT(strcmp(trellis_transport_protocol(udp), "udp") == 0);
    trellis_transport_free(udp);

    uv_loop_close(&loop);
}

// --- MASQUE capsule framing round-trip test ---
#ifdef TRELLIS_WITH_QUIC
extern size_t quic_masque_capsule_wrap(const uint8_t *data, size_t len,
                                       uint8_t *out, size_t out_cap);
extern int    quic_masque_capsule_unwrap(const uint8_t *data, size_t len,
                                         const uint8_t **payload,
                                         size_t *payload_len);
#endif

static void test_masque_capsule_roundtrip(void)
{
#ifdef TRELLIS_WITH_QUIC
    uint8_t original[] = "Hello from Bloom mesh";
    uint8_t capsule[256];
    size_t capsule_len = quic_masque_capsule_wrap(
        original, sizeof(original), capsule, sizeof(capsule));
    ASSERT(capsule_len == sizeof(original) + 1);
    ASSERT(capsule[0] == 0x00); /* context_id */

    const uint8_t *payload;
    size_t payload_len;
    int rc = quic_masque_capsule_unwrap(capsule, capsule_len,
                                         &payload, &payload_len);
    ASSERT(rc == 0);
    ASSERT(payload_len == sizeof(original));
    ASSERT(memcmp(payload, original, sizeof(original)) == 0);
#else
    printf("  (QUIC not enabled, skipping)\n");
#endif
}

// --- LoRa: constructor returns non-NULL when DTN enabled ---
static void test_lora_new_returns_transport(void)
{
#ifdef TRELLIS_WITH_DTN
    uv_loop_t loop;
    uv_loop_init(&loop);

    trellis_lora_config_t cfg = {0};
    cfg.dtn.dtn_store_path = "/tmp/trellis_test_lora_dtn";
    cfg.dtn.bundle_ttl_s = 3600;
    cfg.dtn.max_store_bytes = 1024;
    cfg.serial_port = "/dev/null";
    cfg.baud_rate = 9600;
    cfg.spreading_factor = 9;
    cfg.bandwidth_khz = 125;
    cfg.coding_rate = 5;
    cfg.tx_power_dbm = 14;
    cfg.meshtastic_compat = false;

    trellis_transport_t *t = trellis_transport_lora_new(&loop, &cfg);
    ASSERT(t != NULL);
    ASSERT(strcmp(trellis_transport_protocol(t), "lora") == 0);
    trellis_transport_free(t);

    drain_loop(&loop);
    uv_loop_close(&loop);
#else
    printf("  (DTN not enabled, skipping)\n");
#endif
}

// --- BLE: constructor returns non-NULL when DTN enabled ---
static void test_ble_new_returns_transport(void)
{
#ifdef TRELLIS_WITH_DTN
    uv_loop_t loop;
    uv_loop_init(&loop);

    trellis_ble_config_t cfg = {0};
    cfg.dtn.dtn_store_path = "/tmp/trellis_test_ble_dtn";
    cfg.dtn.bundle_ttl_s = 3600;
    cfg.dtn.max_store_bytes = 512;
    cfg.hci_device = NULL;
    cfg.service_uuid = NULL;
    cfg.mtu = 247;
    cfg.mesh_relay = false;

    trellis_transport_t *t = trellis_transport_ble_new(&loop, &cfg);
    ASSERT(t != NULL);
    ASSERT(strcmp(trellis_transport_protocol(t), "ble") == 0);
    trellis_transport_free(t);

    drain_loop(&loop);
    uv_loop_close(&loop);
#else
    printf("  (DTN not enabled, skipping)\n");
#endif
}

static void test_webrtc_stego_new_returns_transport(void)
{
#ifdef TRELLIS_WITH_WEBRTC_STEGO
    uv_loop_t loop;
    uv_loop_init(&loop);

    trellis_webrtc_stego_config_t cfg = {0};
    cfg.signaling_url = "ws://127.0.0.1:1/signal";
    cfg.display_name = "test-node";
    cfg.lsb_depth = 1;
    cfg.cover_bitrate_kbps = 64;
    cfg.fec_redundancy_pct = 30;

    trellis_transport_t *t = trellis_transport_webrtc_stego_new(&loop, &cfg);
    ASSERT(t != NULL);
    ASSERT(strcmp(trellis_transport_protocol(t), "webrtc-stego") == 0);
    trellis_transport_free(t);

    drain_loop(&loop);
    uv_loop_close(&loop);
#else
    printf("  (WebRTC stego not enabled, skipping)\n");
#endif
}

int main(void)
{
    printf("=== Test Suite: Transport ===\n");

    RUN_TEST(test_transport_protocol_name);
    RUN_TEST(test_tcp_listen_connect);
    RUN_TEST(test_tcp_send_recv);
    RUN_TEST(test_tcp_multiple_frames);
    RUN_TEST(test_tcp_close);
    RUN_TEST(test_udp_send_recv);
    RUN_TEST(test_masque_capsule_roundtrip);
    RUN_TEST(test_lora_new_returns_transport);
    RUN_TEST(test_ble_new_returns_transport);
    RUN_TEST(test_webrtc_stego_new_returns_transport);

    printf("=== All transport tests passed ===\n");
    return 0;
}
