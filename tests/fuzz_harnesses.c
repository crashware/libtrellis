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
 * fuzz_harnesses.c — LibFuzzer / AFL Fuzz Harnesses
 *
 * Provides entry points for fuzzing key Trellis subsystems.
 *
 * Building with libFuzzer (clang):
 *   clang -fsanitize=fuzzer,address -DFUZZ_TARGET=<name> \
 *         -I../include fuzz_harnesses.c -o fuzz_<name> \
 *         -ltrellis -lsodium -loqs -luv
 *   ./fuzz_<name> corpus_<name>/
 *
 * Building with AFL++ (afl-cc):
 *   afl-cc -DFUZZ_TARGET=<name> -DAFL_HARNESS=1 \
 *          -I../include fuzz_harnesses.c -o fuzz_<name> \
 *          -ltrellis -lsodium -loqs -luv
 *
 * Available FUZZ_TARGET values:
 *   handshake       — Parse and process a Bloom 3-message handshake
 *   morph_decode    — Decode a morph-encoded frame
 *   dht_rpc         — Parse a DHT RPC message
 *   onion_peel      — Peel one layer of an onion-routed message
 *   greenhouse_msg  — Parse a greenhouse protocol message
 *   surb_unpack     — Unpack a Single-Use Reply Block
 *   message_parse   — Deserialize a Trellis protocol message
 *   naming_record   — Parse a Bloom naming record
 *   capabilities    — Deserialize a capabilities advertisement
 *   stun_parse      — Parse a STUN/TURN message
 *   turn_server     — Process input through the TURN server state machine
 *   exit_stream_payload — Parse an exit stream onion payload
 *   relay_descriptor    — Verify a relay descriptor blob
 *
 * Corpus generation:
 *   See tests/corpus/ directory for seed corpus files.
 */

#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include <trellis/trellis.h>
#include <trellis/crypto.h>
#include <trellis/identity.h>
#include <trellis/protocol.h>
#include <trellis/mesh.h>
#include <trellis/morph.h>
#include <trellis/greenhouse.h>
#include <trellis/turn.h>
#include <sodium.h>

// --- AFL compatibility shim ---
#ifdef AFL_HARNESS
#  include <unistd.h>
static int afl_fuzz_one(const uint8_t *data, size_t size);
int main(int argc, char **argv)
{
    (void)argc; (void)argv;
    sodium_init();
    uint8_t buf[65536];
    ssize_t n = read(STDIN_FILENO, buf, sizeof(buf));
    if (n > 0) afl_fuzz_one(buf, (size_t)n);
    return 0;
}
#  define LLVMFuzzerTestOneInput afl_fuzz_one
#else
// LibFuzzer prototype.
int LLVMFuzzerInitialize(int *argc, char ***argv);
int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size);

int LLVMFuzzerInitialize(int *argc, char ***argv)
{
    (void)argc; (void)argv;
    sodium_init();
    return 0;
}
#endif

static trellis_identity_t g_id;
static int                g_id_initialized = 0;

static const trellis_identity_t *get_test_id(void)
{
    if (!g_id_initialized) {
        trellis_identity_generate(&g_id);
        g_id_initialized = 1;
    }
    return &g_id;
}

// --- Fuzz targets ---
#if defined(FUZZ_TARGET) && FUZZ_TARGET == handshake

/*
 * Fuzz target: Bloom 3-message handshake parser.
 * Feed arbitrary bytes as if they were a Bloom handshake message.
 * We attempt to deserialize them as a trellis_message_t and process
 * as a handshake step.
 */
int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    if (size < 8) return 0;

    // Try to deserialize as a Bloom message.
    trellis_message_t msg = {0};
    trellis_message_deserialize(data, size, &msg);
    trellis_message_free(&msg);

    // Try to interpret as a handshake msg1 from an unknown initiator.
    trellis_handshake_t *hs = trellis_handshake_new(get_test_id(),
                                                      TRELLIS_ROLE_RESPONDER);
    if (hs) {
        trellis_buf_t resp = {0};
        trellis_handshake_process_msg1(hs, data, size, &resp);
        trellis_buf_free(&resp);
        trellis_handshake_free(hs);
    }
    return 0;
}

#elif defined(FUZZ_TARGET) && FUZZ_TARGET == morph_decode

/*
 * Fuzz target: Morph engine frame decoder.
 * Feed arbitrary bytes as a morphed/camouflaged frame.
 */
int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    if (size < 4) return 0;

    trellis_morph_config_t mcfg = trellis_morph_config_default();
    trellis_morph_t *morph = trellis_morph_new(&mcfg);
    if (!morph) return 0;

    trellis_buf_t decoded = {0};
    trellis_morph_decode(morph, data, size, &decoded);
    trellis_buf_free(&decoded);

    trellis_morph_free(morph);
    return 0;
}

#elif defined(FUZZ_TARGET) && FUZZ_TARGET == dht_rpc

/*
 * Fuzz target: DHT RPC message parser.
 * Feed arbitrary bytes as a Bloom protocol message that might be a DHT RPC.
 */
int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    if (size < TRELLIS_MSG_HEADER_LEN) return 0;

    trellis_message_t msg = {0};
    trellis_err_t err = trellis_message_deserialize(data, size, &msg);
    if (err == TRELLIS_OK) {
        // Try to process as DHT message types.
        switch ((int)msg.type) {
        case 0x0022: /* FIND_NODE */
        case 0x0023: /* FIND_VALUE */
        case 0x0024: /* STORE */
            break; /* parsing succeeded, no crash = pass */
        }
        trellis_message_free(&msg);
    }
    return 0;
}

#elif defined(FUZZ_TARGET) && FUZZ_TARGET == onion_peel

/*
 * Fuzz target: Onion layer peeling.
 * Feed arbitrary bytes as an onion-routed payload to peel.
 * The onion peeling must not crash regardless of input.
 */
int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    if (size < 32) return 0;

    const trellis_identity_t *id = get_test_id();

    // Build a KEM keypair from the identity's static KEM keys.
    trellis_kem_keypair_t kp;
    memcpy(kp.x25519_pk, id->x25519_pk, sizeof(kp.x25519_pk));
    memcpy(kp.x25519_sk, id->x25519_sk, sizeof(kp.x25519_sk));
    memcpy(kp.ml_kem_pk, id->ml_kem_pk, sizeof(kp.ml_kem_pk));
    memcpy(kp.ml_kem_sk, id->ml_kem_sk, sizeof(kp.ml_kem_sk));

    // Attempt to peel one onion layer using the identity's KEM secret key.
    trellis_buf_t out = {0};
    trellis_fingerprint_t next_hop = {0};
    trellis_onion_peel(&kp, data, size, &out, &next_hop);
    trellis_buf_free(&out);
    return 0;
}

#elif defined(FUZZ_TARGET) && FUZZ_TARGET == greenhouse_msg

/*
 * Fuzz target: Greenhouse protocol message parser.
 * Feed arbitrary bytes as greenhouse introduction/rendezvous messages.
 */
int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    if (size < TRELLIS_MSG_HEADER_LEN) return 0;

    trellis_message_t msg = {0};
    if (trellis_message_deserialize(data, size, &msg) == TRELLIS_OK) {
        // Route to greenhouse message handler if type matches.
        trellis_greenhouse_dispatch_msg(&msg);
        trellis_message_free(&msg);
    }
    return 0;
}

#elif defined(FUZZ_TARGET) && FUZZ_TARGET == surb_unpack

/*
 * Fuzz target: Single-Use Reply Block unpacking.
 * Feed arbitrary bytes as a SURB payload.
 */
int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    if (size < 64) return 0;

    // Attempt to deserialise the input as a SURB wire blob.
    trellis_surb_t *s = NULL;
    if (trellis_surb_deserialize(data, size, &s) == TRELLIS_OK && s) {
        /* If deserialisation succeeded, try to decrypt a minimal reply blob.
         * The decrypt will fail (wrong key / bad tag) but must not crash. */
        uint8_t *reply_buf = NULL;
        size_t   reply_len = 0;
        trellis_surb_decrypt_reply(s, data, size < 48 ? size : 48,
                                    &reply_buf, &reply_len);
        free(reply_buf);
        trellis_surb_free(s);
    }
    return 0;
}

#elif defined(FUZZ_TARGET) && FUZZ_TARGET == message_parse

/*
 * Fuzz target: Generic Trellis protocol message deserialization.
 * The most broad fuzzer — any input that could crash the parser.
 */
int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    trellis_message_t msg = {0};
    trellis_err_t err = trellis_message_deserialize(data, size, &msg);
    if (err == TRELLIS_OK) trellis_message_free(&msg);
    return 0;
}

#elif defined(FUZZ_TARGET) && FUZZ_TARGET == naming_record

/*
 * Fuzz target: Bloom naming record parser.
 * Feed arbitrary bytes as if they are a DHT naming record.
 */
#include "internal.h"
#include <uv.h>

static trellis_naming_t *fuzz_naming_ctx(void)
{
    static trellis_naming_t *cached;
    if (cached) return cached;

    static uv_loop_t loop;
    uv_loop_init(&loop);

    /*
     * verify_record never dereferences dht, so a sentinel is safe here.
     * We need a real loop for uv_timer_init inside trellis_naming_new.
     */
    cached = trellis_naming_new((trellis_dht_t *)(uintptr_t)1,
                                 NULL, &loop);
    return cached;
}

int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    if (size < 8) return 0;

    const trellis_identity_t *id = get_test_id();

    trellis_naming_t *n = fuzz_naming_ctx();
    if (!n) return 0;

    trellis_naming_verify_record(n, data, size, id->ed25519_pk);
    return 0;
}

#elif defined(FUZZ_TARGET) && FUZZ_TARGET == capabilities

/*
 * Fuzz target: Capabilities advertisement deserializer.
 */
int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    trellis_capabilities_t caps = {0};
    trellis_capabilities_deserialize(data, size, &caps);
    trellis_capabilities_free(&caps);
    return 0;
}

#elif defined(FUZZ_TARGET) && FUZZ_TARGET == stun_parse

/*
 * Fuzz target: STUN/TURN message parser.
 * Feed arbitrary bytes as a potential STUN or ChannelData message.
 * Tests stun_is_stun(), stun_is_channel_data(), and stun_parse().
 */
int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    if (size < 4) return 0;

    (void)stun_is_stun(data, size);
    (void)stun_is_channel_data(data, size);

    stun_msg_t msg;
    memset(&msg, 0, sizeof(msg));
    trellis_err_t err = stun_parse(data, size, &msg);
    if (err == TRELLIS_OK) {
        // Try to verify integrity with a dummy key (will fail, must not crash)
        uint8_t dummy_key[16] = {0};
        (void)stun_verify_integrity(&msg, dummy_key, sizeof(dummy_key));
        (void)stun_verify_auth_secret(&msg, "fuzz-secret", "fuzz-realm");
    }
    return 0;
}

#elif defined(FUZZ_TARGET) && FUZZ_TARGET == turn_server

/*
 * Fuzz target: TURN server state machine.
 * Initializes a minimal TURN server and feeds arbitrary bytes through
 * turn_server_process(). The send callback is a no-op.
 */
static turn_server_t  fuzz_srv;
static int            fuzz_srv_init = 0;

static void fuzz_send_cb(const uint8_t *d, size_t l,
                         const stun_addr_t *dst, void *ctx)
{
    (void)d; (void)l; (void)dst; (void)ctx;
}
static void fuzz_relay_cb(uint64_t id, const stun_addr_t *peer,
                          const uint8_t *d, size_t l, void *ctx)
{
    (void)id; (void)peer; (void)d; (void)l; (void)ctx;
}

int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    if (size < 4) return 0;

    if (!fuzz_srv_init) {
        stun_addr_t ext = {0};
        ext.family = 0x01;
        ext.port = 443;
        ext.ip.v4[0] = 127; ext.ip.v4[3] = 1;
        turn_server_init(&fuzz_srv, "fuzz.realm", "fuzz-secret", &ext);
        fuzz_srv.send_cb = fuzz_send_cb;
        fuzz_srv.relay_cb = fuzz_relay_cb;
        fuzz_srv_init = 1;
    }

    stun_addr_t client = {0};
    client.family = 0x01;
    client.port = 50000;
    client.ip.v4[0] = 10;

    turn_server_process(&fuzz_srv, data, size, &client, 17);
    turn_server_tick(&fuzz_srv);
    return 0;
}

#elif defined(FUZZ_TARGET) && FUZZ_TARGET == exit_stream_payload

/*
 * Fuzz target: Exit stream payload parser.
 * Feed arbitrary bytes as an exit stream onion payload to
 * trellis_exit_relay_handle_stream, exercising the
 * [cmd][host_len][host][port_be][data] format.
 */
#include <uv.h>

int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    if (size < 4) return 0;

    static uv_loop_t *loop = NULL;
    static trellis_exit_relay_t *er = NULL;

    if (!loop) {
        loop = uv_default_loop();
        er = trellis_exit_relay_new(loop);
        if (!er) return 0;
        trellis_exit_relay_add_policy(er, "allow:*:*");
    }

    uint32_t stream_id = 0;
    if (size >= 4) memcpy(&stream_id, data, 4);

    trellis_exit_relay_handle_stream(er, data, size, stream_id, NULL);
    return 0;
}

#elif defined(FUZZ_TARGET) && FUZZ_TARGET == ratchet_decrypt

/*
 * Fuzz target: Ratchet decryption with crafted counters.
 * Exercises counter validation, chain key snapshot/restore,
 * and PQC flag parsing in trellis_ratchet_decrypt.
 */
int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    if (size < 32) return 0;

    static trellis_ratchet_t *recv_r = NULL;
    if (!recv_r) {
        const trellis_identity_t *id = get_test_id();
        trellis_identity_t peer;
        if (trellis_identity_generate(&peer) != TRELLIS_OK) return 0;

        trellis_handshake_t *hs_a = trellis_handshake_new(&peer, TRELLIS_ROLE_INITIATOR);
        trellis_handshake_t *hs_b = trellis_handshake_new(id, TRELLIS_ROLE_RESPONDER);
        if (!hs_a || !hs_b) {
            trellis_handshake_free(hs_a);
            trellis_handshake_free(hs_b);
            trellis_identity_destroy(&peer);
            return 0;
        }

        trellis_buf_t m1 = {0}, m2 = {0}, m3 = {0};
        if (trellis_handshake_create_msg1(hs_a, &m1) == TRELLIS_OK &&
            trellis_handshake_process_msg1(hs_b, m1.data, m1.len, &m2) == TRELLIS_OK &&
            trellis_handshake_process_msg2(hs_a, m2.data, m2.len, &m3) == TRELLIS_OK &&
            trellis_handshake_process_msg3(hs_b, m3.data, m3.len) == TRELLIS_OK) {
            recv_r = trellis_ratchet_init_from_handshake(hs_b);
        }

        trellis_buf_free(&m1);
        trellis_buf_free(&m2);
        trellis_buf_free(&m3);
        trellis_handshake_free(hs_a);
        trellis_handshake_free(hs_b);
        trellis_identity_destroy(&peer);
    }

    if (!recv_r) return 0;

    trellis_buf_t out = {0};
    trellis_ratchet_decrypt(recv_r, data, size, &out);
    trellis_buf_free(&out);
    return 0;
}

#elif defined(FUZZ_TARGET) && FUZZ_TARGET == gossip_receive

/*
 * Fuzz target: Gossip message receive path.
 * Exercises gossip dedup, payload parsing, and rate limiting.
 */
#include <uv.h>

int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    if (size < 4) return 0;

    static trellis_gossip_t *gossip = NULL;
    if (!gossip) {
        static uv_loop_t loop;
        uv_loop_init(&loop);
        const trellis_identity_t *id = get_test_id();
        trellis_dht_t *dht = trellis_dht_new(id, &loop);
        if (!dht) return 0;
        trellis_dht_set_eclipse_enabled(dht, 0);
        trellis_dht_disable_sybil(dht);
        gossip = trellis_gossip_new(dht, id);
    }
    if (!gossip) return 0;

    trellis_fingerprint_t origin = {0};
    if (size >= TRELLIS_FINGERPRINT_LEN)
        memcpy(origin.bytes, data, TRELLIS_FINGERPRINT_LEN);

    trellis_gossip_receive(gossip, data, size, &origin);
    return 0;
}

#elif defined(FUZZ_TARGET) && FUZZ_TARGET == circuit_create

/*
 * Fuzz target: Circuit table insert/lookup.
 * Exercises circuit table capacity limits and lookup correctness.
 */
#include "internal.h"

int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    if (size < 68) return 0;

    static circuit_table_t ct;
    static int ct_init = 0;
    if (!ct_init) {
        trellis_circuit_table_init(&ct);
        ct_init = 1;
    }

    // Extract fields from fuzz input.
    uint32_t stream_id;
    memcpy(&stream_id, data, 4);

    trellis_fingerprint_t prev_hop;
    memcpy(prev_hop.bytes, data + 4, TRELLIS_FINGERPRINT_LEN);

    uint8_t layer_key[32];
    memcpy(layer_key, data + 4 + TRELLIS_FINGERPRINT_LEN, 32);

    trellis_circuit_table_insert(&ct, stream_id, &prev_hop, layer_key);

    circuit_entry_t *e = trellis_circuit_table_lookup(&ct, stream_id);
    (void)e;

    // Periodically expire.
    if (data[0] & 0x80)
        trellis_circuit_table_expire(&ct, 60000);

    // Periodically remove.
    if (data[0] & 0x40)
        trellis_circuit_table_remove(&ct, stream_id);

    return 0;
}

#elif defined(FUZZ_TARGET) && FUZZ_TARGET == config_parse

/*
 * Fuzz target: vine-relay config file parser.
 * Feed arbitrary bytes as a config file to vine_config_load_file.
 * (Uses a temporary file since the parser reads from disk.)
 */
#include <stdio.h>
#include <unistd.h>

// Forward declare vine-relay config parser.
typedef struct vine_relay_config vine_relay_config_t;
vine_relay_config_t vine_config_defaults(void);
int vine_config_load_file(vine_relay_config_t *cfg, const char *path);

int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    if (size < 2 || size > 65536) return 0;

    char tmppath[] = "/tmp/fuzz_config_XXXXXX";
    int fd = mkstemp(tmppath);
    if (fd < 0) return 0;
    write(fd, data, size);
    close(fd);

    vine_relay_config_t cfg = vine_config_defaults();
    vine_config_load_file(&cfg, tmppath);
    unlink(tmppath);
    return 0;
}

#elif defined(FUZZ_TARGET) && FUZZ_TARGET == relay_descriptor

/*
 * Fuzz target: Relay descriptor verifier.
 * Feed arbitrary bytes to trellis_relay_descriptor_verify with a valid
 * public key, exercising the descriptor parser and signature check.
 */
#include "internal.h"

int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    if (size < 8) return 0;

    const trellis_identity_t *id = get_test_id();
    trellis_identity_public_t pub;
    trellis_identity_public_from(id, &pub);

    trellis_relay_descriptor_t *out = NULL;
    trellis_err_t err = trellis_relay_descriptor_verify(data, size, &pub, &out);
    if (err == TRELLIS_OK && out)
        trellis_relay_descriptor_free(out);
    return 0;
}

#else

/*
 * Default: run all targets on the same input (useful for AFL++ dictionary mode).
 */
int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    if (size < 4) return 0;

    // Message parse.
    trellis_message_t msg = {0};
    if (trellis_message_deserialize(data, size, &msg) == TRELLIS_OK)
        trellis_message_free(&msg);

    // Capabilities
    trellis_capabilities_t caps = {0};
    trellis_capabilities_deserialize(data, size, &caps);
    trellis_capabilities_free(&caps);

    // Morph decode.
    trellis_morph_config_t mcfg = trellis_morph_config_default();
    trellis_morph_t *morph = trellis_morph_new(&mcfg);
    if (morph) {
        trellis_buf_t decoded = {0};
        trellis_morph_decode(morph, data, size, &decoded);
        trellis_buf_free(&decoded);
        trellis_morph_free(morph);
    }

    // STUN parse.
    {
        stun_msg_t sm;
        memset(&sm, 0, sizeof(sm));
        if (stun_parse(data, size, &sm) == TRELLIS_OK) {
            uint8_t dk[16] = {0};
            (void)stun_verify_integrity(&sm, dk, sizeof(dk));
        }
    }

    // TURN server.
    {
        static turn_server_t default_srv;
        static int default_srv_init = 0;
        if (!default_srv_init) {
            stun_addr_t ext = {0};
            ext.family = 0x01; ext.port = 443;
            if (turn_server_init(&default_srv, "fuzz.realm", "fuzz-secret", &ext) == TRELLIS_OK)
                default_srv_init = 1;
            else
                default_srv_init = -1; /* init failed, skip */
        }
        if (default_srv_init == 1) {
            stun_addr_t client = {0};
            client.family = 0x01; client.port = 50000;
            turn_server_process(&default_srv, data, size, &client, 17);
        }
    }

    return 0;
}

#endif /* FUZZ_TARGET */
