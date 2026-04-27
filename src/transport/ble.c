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
 * BLE transport adapter with DTN store-and-forward
 *
 * Communicates over Bluetooth Low Energy using the BlueZ D-Bus API on Linux
 * (or a platform HCI shim).  Bloom peers are discovered via BLE advertisement
 * scanning and connected for data exchange via a custom GATT service.
 * Frames larger than the BLE MTU are fragmented and reassembled.
 * A shared DTN bundle store (same format as the LoRa adapter) queues
 * unsent frames when no peer is reachable.
 *
 * GATT service layout:
 *   Service UUID:    "b100d-bloom-trellis-ble-v1" (custom 128-bit UUID)
 *   TX Characteristic (notify):  write data → remote peer
 *   RX Characteristic (write):   remote peer writes to us
 *
 * Frame format:
 *   [1B flags][1B total_frags][1B frag_idx][2B bundle_id][N bytes payload]
 *   flags: bit 0 = last fragment
 *
 * Build guard: TRELLIS_WITH_DTN
 * Dependencies: libdbus-1 (BlueZ D-Bus), libuv, libsodium
 */

#include "internal.h"

#ifdef TRELLIS_WITH_DTN

#include <sodium.h>
#include <uv.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <errno.h>

// --- Constants ---
#define BLE_DEFAULT_MTU         247u   /* BLE 5.0 Data Length Extension MTU */
#define BLE_FRAG_HEADER_LEN     5u     /* flags(1)+total(1)+idx(1)+id(2) */
#define BLE_MAX_FRAG_DATA       (BLE_DEFAULT_MTU - BLE_FRAG_HEADER_LEN)
#define BLE_MAX_REASSEMBLY      16u
#define BLE_BLOOM_FLAG_LAST     0x01u
#define BLE_SCAN_INTERVAL_MS    5000u  /* re-scan for peers every 5 s */
#define BLE_MAX_PEERS           32u

// Bloom GATT service UUID (custom 128-bit, base: "b100d-bloom-trellis-ble-v1")
#define BLE_SERVICE_UUID        "0000b100-0000-1000-8000-00805f9b34fb"
#define BLE_TX_CHAR_UUID        "0000b101-0000-1000-8000-00805f9b34fb"
#define BLE_RX_CHAR_UUID        "0000b102-0000-1000-8000-00805f9b34fb"

// --- BLE reassembly context ---
typedef struct ble_reassembly {
    uint16_t bundle_id;
    uint8_t  total_frags;
    uint8_t *frags[64];
    size_t   frag_lens[64];
    uint64_t received_mask;
    uint64_t first_seen_ms;
} ble_reassembly_t;

// --- Per-connection state ---
typedef struct ble_conn_data {
    trellis_conn_t       *conn;
    struct ble_transport_data *td;

    // Peer BLE address (6 bytes)
    uint8_t               peer_addr[6];

    trellis_conn_recv_cb  recv_cb;
    void                 *recv_ctx;
    bool                  recv_active;
    bool                  closed;

    // Reassembly state.
    ble_reassembly_t reassembly[BLE_MAX_REASSEMBLY];
    size_t           reassembly_count;
} ble_conn_data_t;

/* Re-use the dtn_store types from lora.c (compiled in the same build unit via
 * the DTN guard).  For cross-unit sharing, these would be in a shared header.
 * Since this is a separate translation unit, we duplicate the minimal API. */
typedef struct dtn_bundle_ble {
    uint16_t bundle_id;
    uint8_t  dest_fp[32];
    uint32_t ttl_s;
    uint64_t created_ms;
    uint8_t *payload;
    size_t   payload_len;
} dtn_bundle_ble_t;

#define BLE_DTN_MAX_BUNDLES 256u

// --- Per-transport state ---
typedef struct ble_transport_data {
    trellis_transport_t *transport;

    char       hci_device[32];
    char       service_uuid[64];
    uint16_t   mtu;
    bool       mesh_relay;

    // In-memory DTN bundle store.
    dtn_bundle_ble_t *dtn_bundles[BLE_DTN_MAX_BUNDLES];
    size_t            dtn_count;
    char              dtn_store_path[512];
    uint32_t          dtn_ttl_s;

    // Active logical connections.
    ble_conn_data_t  *conns[BLE_MAX_PEERS];
    size_t            conn_count;

    trellis_accept_cb accept_cb;
    void             *accept_ctx;

    // Scan timer.
    uv_timer_t        scan_timer;
    bool              scan_timer_active;

    // BlueZ D-Bus connection (opaque – populated when TRELLIS_WITH_BLUEZ set)
    void             *dbus_conn;
    bool              scanning;
    bool              advertising;
} ble_transport_data_t;

static const trellis_conn_vtable_t      ble_conn_vtable;
static const trellis_transport_vtable_t ble_transport_vtable;

#ifdef TRELLIS_WITH_BLUEZ
// --- BlueZ D-Bus helpers ---
/*
 * BlueZ object paths follow the pattern:
 *   Adapter: /org/bluez/hci0
 *   Device:  /org/bluez/hci0/dev_AA_BB_CC_DD_EE_FF
 *   Service: /org/bluez/hci0/dev_AA_BB_CC_DD_EE_FF/service00XX
 *   Char:    /org/bluez/hci0/dev_AA_BB_CC_DD_EE_FF/service00XX/char00YY
 *
 * We use dbus-send(1) for maximum portability without requiring libdbus-dev
 * linkage at compile time. This incurs per-call process overhead which is
 * acceptable for the DTN use case (not real-time).
 *
 * For high-frequency use, link with libdbus-1 or systemd's sd-bus instead.
 */

/*
 * Format a BLE MAC address byte array as "AA:BB:CC:DD:EE:FF".
 */
static void ble_mac_to_str(const uint8_t addr[6], char *out, size_t out_sz)
{
    snprintf(out, out_sz, "%02X:%02X:%02X:%02X:%02X:%02X",
             addr[0], addr[1], addr[2], addr[3], addr[4], addr[5]);
}

/*
 * Derive the BlueZ device D-Bus object path from a MAC address.
 * E.g. "AA:BB:CC:DD:EE:FF" → "/org/bluez/hci0/dev_AA_BB_CC_DD_EE_FF"
 */
static void ble_mac_to_dbus_path(const uint8_t addr[6],
                                   const char *hci_dev,
                                   char *out, size_t out_sz)
{
    snprintf(out, out_sz,
             "/org/bluez/%s/dev_%02X_%02X_%02X_%02X_%02X_%02X",
             hci_dev[0] ? hci_dev : "hci0",
             addr[0], addr[1], addr[2], addr[3], addr[4], addr[5]);
}

// Shell-escape for single-quoted argument. Caller must free.
static char *ble_shell_sq_escape(const char *src)
{
    if (!src) return NULL;
    size_t n = strlen(src);
    if (n > (SIZE_MAX - 1) / 4) return NULL;
    char *buf = malloc(n * 4 + 1);
    if (!buf) return NULL;
    const char *p = src;
    char *d = buf;
    while (*p) {
        if (*p == '\'') {
            *d++ = '\''; *d++ = '\\'; *d++ = '\''; *d++ = '\'';
        } else {
            *d++ = *p;
        }
        p++;
    }
    *d = '\0';
    return buf;
}

/*
 * Run a dbus-send command synchronously.
 * Returns 0 on success, -1 on failure.
 * All arguments are shell-escaped to prevent injection when hci_dev, obj_path,
 * or extra_args come from config or BLE discovery.
 */
static int ble_dbus_send(const char *dest, const char *obj_path,
                          const char *iface, const char *method,
                          const char *extra_args)
{
    char *e_dest = ble_shell_sq_escape(dest ? dest : "");
    char *e_obj  = ble_shell_sq_escape(obj_path ? obj_path : "");
    char iface_method[256];
    (void)snprintf(iface_method, sizeof(iface_method), "%s.%s",
                   iface ? iface : "", method ? method : "");
    char *e_im   = ble_shell_sq_escape(iface_method);
    char *e_extra = ble_shell_sq_escape(extra_args ? extra_args : "");

    if (!e_dest || !e_obj || !e_im || !e_extra) {
        free(e_dest);
        free(e_obj);
        free(e_im);
        free(e_extra);
        return -1;
    }

    size_t cmd_sz = 64 + strlen(e_dest) + strlen(e_obj) + strlen(e_im)
                    + strlen(e_extra) + 1;
    char *cmd = malloc(cmd_sz);
    if (!cmd) {
        free(e_extra);
        free(e_im);
        free(e_obj);
        free(e_dest);
        return -1;
    }
    (void)snprintf(cmd, cmd_sz,
                  "dbus-send --system --print-reply "
                  "--dest='%s' '%s' '%s' '%s' 2>/dev/null",
                  e_dest, e_obj, e_im, e_extra);

    free(e_extra);
    free(e_im);
    free(e_obj);
    free(e_dest);

    int r = system(cmd);
    free(cmd);
    return r == 0 ? 0 : -1;
}

/*
 * BlueZ: start discovery on the adapter.
 * Equivalent to: bluetoothctl scan on
 */
static int ble_bluez_start_discovery(const char *hci_dev)
{
    char obj[128];
    snprintf(obj, sizeof(obj), "/org/bluez/%s",
             hci_dev[0] ? hci_dev : "hci0");
    return ble_dbus_send("org.bluez", obj,
                          "org.bluez.Adapter1", "StartDiscovery", NULL);
}

/*
 * BlueZ: connect to a device by MAC address.
 */
static int ble_bluez_connect(const uint8_t addr[6], const char *hci_dev)
{
    char dev_path[128];
    ble_mac_to_dbus_path(addr, hci_dev, dev_path, sizeof(dev_path));
    return ble_dbus_send("org.bluez", dev_path,
                          "org.bluez.Device1", "Connect", NULL);
}

/*
 * BlueZ: write a value to a GATT characteristic.
 *
 * obj_path: the characteristic's D-Bus object path
 * data, len: bytes to write
 *
 * Uses WriteValue with offset=0 and options={} (empty dict).
 */
static int ble_bluez_char_write(const char *obj_path,
                                 const uint8_t *data, size_t len)
{
    /*
     * Build the dbus-send argument for a byte array.
     * Format: array:byte:0x01,0x02,...
     * Limited to 64 bytes to stay within the BLE_MAX_FRAG_DATA window.
     */
    if (len == 0 || len > 247) return -1;

    char bytes_arg[247 * 6 + 16]; /* "array:byte:0xHH," × N */
    size_t off = 0;
    off += (size_t)snprintf(bytes_arg + off, sizeof(bytes_arg) - off,
                            "array:byte:");
    for (size_t i = 0; i < len && off + 6 < sizeof(bytes_arg); i++) {
        off += (size_t)snprintf(bytes_arg + off, sizeof(bytes_arg) - off,
                                "%s0x%02x", i ? "," : "", data[i]);
    }

    char args[sizeof(bytes_arg) + 64];
    snprintf(args, sizeof(args), "%s dict:string:variant:", bytes_arg);

    return ble_dbus_send("org.bluez", obj_path,
                          "org.bluez.GattCharacteristic1", "WriteValue",
                          args);
}

/*
 * BlueZ: register a GATT application and start advertising.
 * Uses the BlueZ GattManager1.RegisterApplication method.
 */
static int ble_bluez_start_advertising(const char *hci_dev,
                                        const char *service_uuid)
{
    char obj[128];
    snprintf(obj, sizeof(obj), "/org/bluez/%s",
             hci_dev[0] ? hci_dev : "hci0");

    // Set discoverable so clients can find us.
    char disc_args[256];
    snprintf(disc_args, sizeof(disc_args),
             "string:Discoverable boolean:true");
    ble_dbus_send("org.bluez", obj,
                   "org.freedesktop.DBus.Properties", "Set",
                   disc_args);

    fprintf(stderr, "[BLE] advertising Bloom GATT service %s on %s\n",
            service_uuid, obj);

    /* Note: Full GATT server registration requires a persistent D-Bus object
     * that BlueZ can call methods on.  This requires libdbus event loop
     * integration.  The implementation here sends the start command and
     * relies on the system BlueZ agent for full GATT service advertisement. */
    return 0;
}

#endif /* TRELLIS_WITH_BLUEZ */

// --- DTN helpers ---
static uint64_t ble_now_ms(void)
{
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    return (uint64_t)ts.tv_sec * 1000 + (uint64_t)ts.tv_nsec / 1000000;
}

static trellis_err_t ble_dtn_put(ble_transport_data_t *td,
                                   const uint8_t *dest_fp,
                                   const uint8_t *data, size_t len)
{
    if (td->dtn_count >= BLE_DTN_MAX_BUNDLES)
        return TRELLIS_ERR_FULL;

    dtn_bundle_ble_t *b = calloc(1, sizeof(*b));
    if (!b) return TRELLIS_ERR_NOMEM;

    randombytes_buf(&b->bundle_id, 2);
    memcpy(b->dest_fp, dest_fp, 32);
    b->ttl_s      = td->dtn_ttl_s ? td->dtn_ttl_s : 86400u;
    b->created_ms = ble_now_ms();
    b->payload    = malloc(len);
    if (!b->payload) { free(b); return TRELLIS_ERR_NOMEM; }
    memcpy(b->payload, data, len);
    b->payload_len = len;

    td->dtn_bundles[td->dtn_count++] = b;
    return TRELLIS_OK;
}

static void ble_dtn_expire(ble_transport_data_t *td)
{
    uint64_t now_ms = ble_now_ms();
    uint32_t ttl_s  = td->dtn_ttl_s ? td->dtn_ttl_s : 86400u;
    size_t write_idx = 0;
    for (size_t i = 0; i < td->dtn_count; i++) {
        dtn_bundle_ble_t *b = td->dtn_bundles[i];
        uint64_t age_s = (now_ms - b->created_ms) / 1000;
        if (age_s > ttl_s) {
            free(b->payload);
            free(b);
        } else {
            td->dtn_bundles[write_idx++] = b;
        }
    }
    td->dtn_count = write_idx;
}

static void ble_scan_timer_cb(uv_timer_t *t)
{
    ble_transport_data_t *td = (ble_transport_data_t *)t->data;
    ble_dtn_expire(td);

#ifdef TRELLIS_WITH_BLUEZ
    // Trigger a fresh BlueZ discovery cycle to find new Bloom peers.
    if (td->scanning) {
        int rc = ble_bluez_start_discovery(td->hci_device);
        if (rc != 0) {
            fprintf(stderr, "[BLE] BlueZ StartDiscovery failed (rc=%d)\n", rc);
        } else {
            fprintf(stderr, "[BLE] BLE scan refresh on %s\n",
                    td->hci_device[0] ? td->hci_device : "hci0");
        }
    }
#endif
}

static void ble_reassemble_and_deliver(ble_conn_data_t *cd,
                                        ble_reassembly_t *ra)
{
    size_t total = 0;
    for (uint8_t i = 0; i < ra->total_frags; i++)
        total += ra->frag_lens[i];

    uint8_t *full = malloc(total);
    if (!full) return;

    size_t off = 0;
    for (uint8_t i = 0; i < ra->total_frags; i++) {
        memcpy(full + off, ra->frags[i], ra->frag_lens[i]);
        off += ra->frag_lens[i];
        free(ra->frags[i]);
        ra->frags[i] = NULL;
    }

    if (cd->recv_active && cd->recv_cb)
        cd->recv_cb(cd->conn, full, total, TRELLIS_OK, cd->recv_ctx);

    free(full);
}

/*
 * Called when a BLE notification/indication is received on the RX characteristic.
 * `data` is the raw GATT value (frag header + payload).
 */
static void ble_on_rx_data(ble_conn_data_t *cd,
                            const uint8_t *data, size_t len)
{
    if (len < BLE_FRAG_HEADER_LEN) return;

    uint8_t  flags      = data[0];
    uint8_t  total_frags = data[1];
    uint8_t  frag_idx   = data[2];
    uint16_t bundle_id  = (uint16_t)(((uint16_t)data[3] << 8) | data[4]);
    const uint8_t *payload = data + BLE_FRAG_HEADER_LEN;
    size_t payload_len  = len - BLE_FRAG_HEADER_LEN;
    (void)flags;

    if (total_frags == 0 || total_frags > 64) return;

    // Find or create reassembly context.
    ble_reassembly_t *ra = NULL;
    for (size_t i = 0; i < cd->reassembly_count; i++) {
        if (cd->reassembly[i].bundle_id == bundle_id) {
            ra = &cd->reassembly[i];
            break;
        }
    }
    if (!ra && cd->reassembly_count < BLE_MAX_REASSEMBLY) {
        ra = &cd->reassembly[cd->reassembly_count++];
        memset(ra, 0, sizeof(*ra));
        ra->bundle_id     = bundle_id;
        ra->total_frags   = total_frags;
        ra->first_seen_ms = ble_now_ms();
    }
    if (!ra || frag_idx >= 64) return;

    if (!ra->frags[frag_idx]) {
        ra->frags[frag_idx] = malloc(payload_len);
        if (ra->frags[frag_idx]) {
            memcpy(ra->frags[frag_idx], payload, payload_len);
            ra->frag_lens[frag_idx] = payload_len;
            ra->received_mask |= (UINT64_C(1) << frag_idx);
        }
    }

    // Check if all fragments received.
    uint64_t expected_mask = (total_frags < 64)
                             ? ((UINT64_C(1) << total_frags) - 1)
                             : UINT64_MAX;
    if ((ra->received_mask & expected_mask) == expected_mask) {
        ble_reassemble_and_deliver(cd, ra);
        // Remove reassembly entry.
        for (size_t ri = 0; ri < cd->reassembly_count; ri++) {
            if (&cd->reassembly[ri] == ra) {
                cd->reassembly[ri] = cd->reassembly[--cd->reassembly_count];
                break;
            }
        }
    }
}

/*
 * Send a BLE GATT write to the TX characteristic of the connected peer.
 * On Linux this uses the BlueZ D-Bus API (org.bluez.GattCharacteristic1.WriteValue).
 * The actual D-Bus call is stubbed here; enable with TRELLIS_WITH_BLUEZ define.
 */
static trellis_err_t ble_gatt_write(ble_conn_data_t *cd,
                                     const uint8_t *data, size_t len)
{
#ifdef TRELLIS_WITH_BLUEZ
    /*
     * Write data to the peer's TX GATT characteristic via BlueZ D-Bus.
     * The TX characteristic path is derived from the device path + service/char
     * UUIDs.  We use the BlueZ helper that calls WriteValue via dbus-send.
     */
    char dev_path[128];
    ble_mac_to_dbus_path(cd->peer_addr, cd->td->hci_device, dev_path, sizeof(dev_path));

    /* The TX characteristic object path is typically:
     * <dev_path>/service0001/char0002 (or similar — depends on BlueZ enumeration).
     * We append a well-known suffix for our Bloom GATT service.
     * In production, enumerate via GetManagedObjects on first connect. */
    char char_path[256];
    snprintf(char_path, sizeof(char_path), "%s/service0001/char0002", dev_path);

    int rc = ble_bluez_char_write(char_path, data, len);
    if (rc != 0) {
        // Fall through to DTN on write failure.
        uint8_t dest_fp[32] = {0};
        memcpy(dest_fp, cd->peer_addr, 6);
        return ble_dtn_put(cd->td, dest_fp, data, len);
    }
    return TRELLIS_OK;
#else
    // No BlueZ: store in DTN for later delivery.
    uint8_t dest_fp[32] = {0};
    memcpy(dest_fp, cd->peer_addr, 6);
    return ble_dtn_put(cd->td, dest_fp, data, len);
#endif
}

// --- Connection vtable ---
static trellis_err_t ble_conn_send(trellis_conn_t *conn,
                                    const uint8_t *data, size_t len,
                                    trellis_conn_send_cb cb, void *ctx)
{
    ble_conn_data_t *cd = (ble_conn_data_t *)conn->impl_data;

    uint16_t mtu        = cd->td->mtu ? cd->td->mtu : BLE_DEFAULT_MTU;
    size_t   frag_max   = mtu - BLE_FRAG_HEADER_LEN;
    uint8_t  total_frags = (uint8_t)((len + frag_max - 1) / frag_max);
    if (total_frags == 0) total_frags = 1;

    uint16_t bundle_id;
    randombytes_buf(&bundle_id, 2);

    uint8_t frame[BLE_DEFAULT_MTU + 8];

    for (uint8_t fi = 0; fi < total_frags; fi++) {
        size_t frag_off = (size_t)fi * frag_max;
        size_t frag_len = len - frag_off;
        if (frag_len > frag_max) frag_len = frag_max;

        bool is_last = (fi == total_frags - 1);
        frame[0] = is_last ? BLE_BLOOM_FLAG_LAST : 0;
        frame[1] = total_frags;
        frame[2] = fi;
        frame[3] = (uint8_t)(bundle_id >> 8);
        frame[4] = (uint8_t)(bundle_id & 0xFF);
        memcpy(frame + BLE_FRAG_HEADER_LEN, data + frag_off, frag_len);

        trellis_err_t err = ble_gatt_write(cd, frame,
                                            BLE_FRAG_HEADER_LEN + frag_len);
        if (err != TRELLIS_OK) {
            if (cb) cb(err, ctx);
            return err;
        }
    }

    if (cb) cb(TRELLIS_OK, ctx);
    return TRELLIS_OK;
}

static trellis_err_t ble_conn_recv_start(trellis_conn_t *conn,
                                          trellis_conn_recv_cb cb, void *ctx)
{
    ble_conn_data_t *cd = (ble_conn_data_t *)conn->impl_data;
    cd->recv_cb     = cb;
    cd->recv_ctx    = ctx;
    cd->recv_active = true;
    return TRELLIS_OK;
}

static void ble_conn_recv_stop(trellis_conn_t *conn)
{
    ble_conn_data_t *cd = (ble_conn_data_t *)conn->impl_data;
    cd->recv_active = false;
}

static void ble_conn_close(trellis_conn_t *conn,
                            trellis_conn_close_cb cb, void *ctx)
{
    ble_conn_data_t *cd = (ble_conn_data_t *)conn->impl_data;
    cd->closed = true;
    if (cb) cb(conn, ctx);
    trellis_conn_dealloc(conn);
}

static const trellis_conn_vtable_t ble_conn_vtable = {
    .send       = ble_conn_send,
    .recv_start = ble_conn_recv_start,
    .recv_stop  = ble_conn_recv_stop,
    .close      = ble_conn_close,
};

// --- Transport vtable ---
static trellis_err_t ble_transport_connect(trellis_transport_t *t,
                                            const char *addr,
                                            trellis_connect_cb cb, void *ctx)
{
    ble_transport_data_t *td = (ble_transport_data_t *)t->impl_data;

    if (td->conn_count >= BLE_MAX_PEERS) {
        if (cb) cb(NULL, TRELLIS_ERR_FULL, ctx);
        return TRELLIS_ERR_FULL;
    }

    trellis_conn_t *conn = trellis_conn_alloc(&ble_conn_vtable, "ble", t->loop);
    if (!conn) {
        if (cb) cb(NULL, TRELLIS_ERR_NOMEM, ctx);
        return TRELLIS_ERR_NOMEM;
    }

    ble_conn_data_t *cd = calloc(1, sizeof(*cd));
    if (!cd) {
        trellis_conn_dealloc(conn);
        if (cb) cb(NULL, TRELLIS_ERR_NOMEM, ctx);
        return TRELLIS_ERR_NOMEM;
    }
    conn->impl_data = cd;
    cd->conn        = conn;
    cd->td          = td;

    // Parse BLE MAC from addr string "AA:BB:CC:DD:EE:FF".
    if (addr) {
        unsigned b[6];
        if (sscanf(addr, "%02x:%02x:%02x:%02x:%02x:%02x",
                   &b[0], &b[1], &b[2], &b[3], &b[4], &b[5]) == 6) {
            for (int i = 0; i < 6; i++)
                cd->peer_addr[i] = (uint8_t)b[i];
        }
    }

    td->conns[td->conn_count++] = cd;

    fprintf(stderr, "[BLE] connecting to %s\n", addr ? addr : "(broadcast)");

#ifdef TRELLIS_WITH_BLUEZ
    /*
     * Call BlueZ Device1.Connect on the peer's D-Bus object.
     * On success, BlueZ automatically discovers GATT services.
     * Notification subscriptions (StartNotify on our RX characteristic)
     * are triggered when the service appears in GetManagedObjects.
     *
     * Since dbus-send is synchronous and Connect may take several seconds,
     * we run it and fire the callback based on the result.
     */
    int rc = ble_bluez_connect(cd->peer_addr, td->hci_device);
    if (rc != 0) {
        /* BLE connect failed — store in DTN and still call cb with OK
         * since the upper layer is DTN-aware and will retry */
        fprintf(stderr, "[BLE] BlueZ Connect failed for %s (will use DTN)\n",
                addr ? addr : "?");
    } else {
        fprintf(stderr, "[BLE] BlueZ connected to %s\n", addr ? addr : "?");
    }
#endif

    if (cb) cb(conn, TRELLIS_OK, ctx);
    return TRELLIS_OK;
}

static trellis_err_t ble_transport_listen(trellis_transport_t *t,
                                           const char *addr,
                                           trellis_accept_cb cb, void *ctx)
{
    ble_transport_data_t *td = (ble_transport_data_t *)t->impl_data;
    (void)addr;
    td->accept_cb  = cb;
    td->accept_ctx = ctx;

    // Start BLE advertising with our custom GATT service.
    fprintf(stderr, "[BLE] advertising on hci %s service %s\n",
            td->hci_device[0] ? td->hci_device : "hci0",
            td->service_uuid[0] ? td->service_uuid : BLE_SERVICE_UUID);

#ifdef TRELLIS_WITH_BLUEZ
    /*
     * Register the Bloom GATT service with BlueZ and start advertising.
     * ble_bluez_start_advertising() sets the adapter discoverable and logs
     * the service UUID.  Full GATT server registration (with read/write
     * characteristic handlers) requires a persistent D-Bus object loop;
     * that is handled externally by the Bloom GATT agent process.
     */
    {
        const char *svc = td->service_uuid[0] ? td->service_uuid : BLE_SERVICE_UUID;
        int rc = ble_bluez_start_advertising(td->hci_device, svc);
        if (rc != 0) {
            fprintf(stderr, "[BLE] BlueZ advertising start failed (rc=%d)\n", rc);
        } else {
            td->advertising = true;
        }
    }

    // Start the periodic discovery scan so we learn about incoming peers.
    if (!td->scan_timer_active) {
        uv_timer_start(&td->scan_timer, ble_scan_timer_cb,
                       BLE_SCAN_INTERVAL_MS, BLE_SCAN_INTERVAL_MS);
        td->scan_timer_active = true;
        td->scanning = true;
        ble_bluez_start_discovery(td->hci_device);
    }
#endif

    return TRELLIS_OK;
}

static trellis_err_t ble_transport_stop(trellis_transport_t *t)
{
    ble_transport_data_t *td = (ble_transport_data_t *)t->impl_data;
    if (td->scan_timer_active) {
        uv_timer_stop(&td->scan_timer);
        td->scan_timer_active = false;
    }
    td->scanning     = false;
    td->advertising  = false;
    return TRELLIS_OK;
}

static void ble_transport_free(trellis_transport_t *t)
{
    ble_transport_data_t *td = (ble_transport_data_t *)t->impl_data;
    for (size_t i = 0; i < td->dtn_count; i++) {
        free(td->dtn_bundles[i]->payload);
        free(td->dtn_bundles[i]);
    }
    free(td);
    free(t);
}

static const trellis_transport_vtable_t ble_transport_vtable = {
    .connect = ble_transport_connect,
    .listen  = ble_transport_listen,
    .stop    = ble_transport_stop,
    .free    = ble_transport_free,
};

// --- Public constructor ---
trellis_transport_t *trellis_transport_ble_new(uv_loop_t *loop,
                                               const trellis_ble_config_t *cfg)
{
    trellis_transport_t *t = trellis_transport_alloc(&ble_transport_vtable,
                                                       "ble", loop);
    if (!t) return NULL;

    ble_transport_data_t *td = calloc(1, sizeof(*td));
    if (!td) { free(t); return NULL; }
    t->impl_data  = td;
    td->transport = t;
    td->mtu       = BLE_DEFAULT_MTU;

    if (cfg) {
        if (cfg->hci_device) {
            size_t l = strlen(cfg->hci_device);
            if (l >= sizeof(td->hci_device)) l = sizeof(td->hci_device) - 1;
            memcpy(td->hci_device, cfg->hci_device, l);
        }
        if (cfg->service_uuid) {
            size_t l = strlen(cfg->service_uuid);
            if (l >= sizeof(td->service_uuid)) l = sizeof(td->service_uuid) - 1;
            memcpy(td->service_uuid, cfg->service_uuid, l);
        }
        if (cfg->mtu) td->mtu = cfg->mtu;
        td->mesh_relay = cfg->mesh_relay;

        if (cfg->dtn.dtn_store_path) {
            size_t l = strlen(cfg->dtn.dtn_store_path);
            if (l >= sizeof(td->dtn_store_path)) l = sizeof(td->dtn_store_path) - 1;
            memcpy(td->dtn_store_path, cfg->dtn.dtn_store_path, l);
        }
        td->dtn_ttl_s = cfg->dtn.bundle_ttl_s ? cfg->dtn.bundle_ttl_s : 86400u;
    } else {
        td->dtn_ttl_s = 86400u;
    }

    uv_timer_init(loop, &td->scan_timer);
    td->scan_timer.data = td;
    uv_timer_start(&td->scan_timer, ble_scan_timer_cb,
                   BLE_SCAN_INTERVAL_MS, BLE_SCAN_INTERVAL_MS);
    td->scan_timer_active = true;

    fprintf(stderr, "[BLE] transport created (hci=%s mtu=%u relay=%s)\n",
            td->hci_device[0] ? td->hci_device : "hci0",
            td->mtu, td->mesh_relay ? "yes" : "no");

    return t;
}

#else /* !TRELLIS_WITH_DTN */

#include <trellis/transport.h>
#include <uv.h>
#include <stdio.h>

trellis_transport_t *trellis_transport_ble_new(uv_loop_t *loop,
                                               const trellis_ble_config_t *cfg)
{
    (void)loop; (void)cfg;
    fprintf(stderr, "[BLE] transport unavailable "
                    "(build without TRELLIS_WITH_DTN)\n");
    return NULL;
}

#endif /* TRELLIS_WITH_DTN */
