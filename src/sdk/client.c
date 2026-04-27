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
#include "../mesh/internal.h"
#include <trellis/probe_resist.h>
#include <trellis/camouflage.h>
#include <trellis/greenhouse.h>
#include <trellis/rhizome.h>

static trellis_exit_stream_t *trellis_exit_stream_find(trellis_client_t *client,
                                                       uint32_t stream_id);

// Map a negotiated morph mode string to the corresponding timing profile.
static trellis_timing_profile_t morph_mode_to_profile(const char *mode)
{
    if (!mode)                              return TRELLIS_TIMING_DEFAULT;
    if (strcmp(mode, "https-mimic") == 0)    return TRELLIS_TIMING_HTTPS;
    if (strcmp(mode, "http2-mimic") == 0)    return TRELLIS_TIMING_HTTP2;
    if (strcmp(mode, "dns-tunnel")  == 0)    return TRELLIS_TIMING_DNS;
    if (strcmp(mode, "ws-mimic")    == 0)    return TRELLIS_TIMING_HTTPS;
    if (strcmp(mode, "raw")         == 0)    return TRELLIS_TIMING_DEFAULT;
    return TRELLIS_TIMING_DEFAULT;
}

trellis_garden_t *trellis_client_find_garden(const trellis_client_t *client,
                                             const char *name)
{
    if (!client || !name)
        return NULL;

    for (size_t i = 0; i < client->garden_count; i++) {
        if (strcmp(client->gardens[i].name, name) == 0)
            return client->gardens[i].garden;
    }
    return NULL;
}

static trellis_err_t garden_map_insert(trellis_client_t *client,
                                       const char *name,
                                       trellis_garden_t *garden)
{
    if (client->garden_count >= client->garden_cap) {
        size_t new_cap = client->garden_cap
                         ? client->garden_cap * 2
                         : TRELLIS_GARDENS_INIT_CAP;
        trellis_garden_entry_t *new_arr = realloc(
            client->gardens, new_cap * sizeof(*new_arr));
        if (!new_arr)
            return TRELLIS_ERR_NOMEM;
        client->gardens = new_arr;
        client->garden_cap = new_cap;
    }

    trellis_garden_entry_t *e = &client->gardens[client->garden_count];
    e->name = strdup(name);
    if (!e->name)
        return TRELLIS_ERR_NOMEM;
    e->garden = garden;
    client->garden_count++;
    return TRELLIS_OK;
}

static void garden_map_remove(trellis_client_t *client, const char *name)
{
    // TODO: O(n) scan -- use a hash map if garden count grows
    for (size_t i = 0; i < client->garden_count; i++) {
        if (strcmp(client->gardens[i].name, name) == 0) {
            free(client->gardens[i].name);
            trellis_garden_free(client->gardens[i].garden);
            client->gardens[i] = client->gardens[--client->garden_count];
            return;
        }
    }
}

static void stop_handshake_timer(trellis_peer_conn_t *pc);

trellis_peer_conn_t *trellis_client_find_peer(trellis_client_t *client,
                                               const trellis_fingerprint_t *fp)
{
    if (!client || !fp)
        return NULL;
    for (size_t i = 0; i < client->peer_conn_count; i++) {
        if (trellis_fingerprint_eq(&client->peers[i].remote_fp, fp))
            return &client->peers[i];
    }
    return NULL;
}

trellis_err_t trellis_client_add_peer_conn(trellis_client_t *client,
                                            trellis_peer_conn_t *pc)
{
    if (client->peer_conn_count >= client->peer_conn_cap) {
        size_t new_cap = client->peer_conn_cap
                         ? client->peer_conn_cap * 2
                         : TRELLIS_PEER_CONN_INIT_CAP;
        trellis_peer_conn_t *arr = realloc(
            client->peers, new_cap * sizeof(*arr));
        if (!arr)
            return TRELLIS_ERR_NOMEM;
        client->peers = arr;
        client->peer_conn_cap = new_cap;
    }
    client->peers[client->peer_conn_count++] = *pc;
    return TRELLIS_OK;
}

static void cleanup_peer_conn_at(trellis_client_t *client, size_t i,
                                  const trellis_fingerprint_t *fp)
{
    trellis_peer_conn_t *pc = &client->peers[i];
    if (pc->handshake_timer)
        stop_handshake_timer(pc);
    if (pc->ratchet)
        trellis_ratchet_free(pc->ratchet);
    if (pc->handshake)
        trellis_handshake_free(pc->handshake);
    if (pc->morph)
        trellis_morph_free(pc->morph);
    client->peers[i] = client->peers[--client->peer_conn_count];

    bool still_connected = false;
    for (size_t j = 0; j < client->peer_conn_count; j++) {
        if (trellis_fingerprint_eq(&client->peers[j].remote_fp, fp)) {
            still_connected = true;
            break;
        }
    }
    if (!still_connected && client->dht)
        trellis_dht_remove_peer(client->dht, fp);
}

void trellis_client_remove_peer_conn(trellis_client_t *client,
                                      const trellis_fingerprint_t *fp)
{
    // Close all relay circuits involving this peer.
    if (client->relay)
        trellis_relay_close_circuits_for_peer(client->relay, fp);

    for (size_t i = 0; i < client->peer_conn_count; i++) {
        if (trellis_fingerprint_eq(&client->peers[i].remote_fp, fp)) {
            cleanup_peer_conn_at(client, i, fp);
            return;
        }
    }
}

static void remove_peer_conn_by_ptr(trellis_client_t *client,
                                     trellis_conn_t *conn)
{
    for (size_t i = 0; i < client->peer_conn_count; i++) {
        if (client->peers[i].conn == conn) {
            trellis_fingerprint_t fp = client->peers[i].remote_fp;
            cleanup_peer_conn_at(client, i, &fp);
            return;
        }
    }
}

static trellis_err_t send_handshake_msg(trellis_client_t *client,
                                        trellis_conn_t *conn,
                                        uint16_t hs_type,
                                        const uint8_t *payload,
                                        size_t payload_len,
                                        const uint8_t *target_obfs_pk);
static void send_peer_exchange(trellis_client_t *client,
                               trellis_peer_conn_t *target_pc);
static void handle_peer_exchange(trellis_client_t *client,
                                 const uint8_t *payload, size_t len);
static void handle_ping(trellis_client_t *client, trellis_conn_t *conn,
                        const trellis_message_t *msg);
static void handle_pong(trellis_client_t *client, trellis_conn_t *conn,
                        const trellis_message_t *msg);
static void handle_onion(trellis_client_t *client, trellis_conn_t *conn,
                         const trellis_message_t *msg);
static void handle_find_node(trellis_client_t *client, trellis_conn_t *conn,
                             const trellis_message_t *msg);
static void handle_store(trellis_client_t *client,
                         const trellis_message_t *msg);
static void on_seed_connected(trellis_conn_t *conn, trellis_err_t err, void *ctx);
static const char *strip_proto_prefix(const char *addr);

// --- Handshake timeout ---
typedef struct hs_timeout_ctx {
    trellis_client_t      *client;
    trellis_fingerprint_t  fp;
} hs_timeout_ctx_t;

static void hs_timer_close_cb(uv_handle_t *handle)
{
    free(handle->data);
    free(handle);
}

static void on_handshake_timeout(uv_timer_t *handle)
{
    hs_timeout_ctx_t *tctx = handle->data;
    trellis_client_t *client = tctx->client;

    trellis_peer_conn_t *pc = trellis_client_find_peer(client, &tctx->fp);
    if (pc && !pc->handshake_complete) {
        trellis_event_data_t ev = {0};
        ev.event = TRELLIS_EV_PEER_REMOVED;
        ev.peer = pc->remote_fp;
        trellis_emit(client, &ev);

        pc->handshake_timer = NULL;
        trellis_client_remove_peer_conn(client, &tctx->fp);
    }

    uv_close((uv_handle_t *)handle, hs_timer_close_cb);
}

static void start_handshake_timer(trellis_client_t *client,
                                  trellis_peer_conn_t *pc)
{
    uv_timer_t *timer = malloc(sizeof(*timer));
    if (!timer)
        return;

    hs_timeout_ctx_t *tctx = malloc(sizeof(*tctx));
    if (!tctx) {
        free(timer);
        return;
    }
    tctx->client = client;
    tctx->fp = pc->remote_fp;

    uv_timer_init(&client->loop, timer);
    timer->data = tctx;
    pc->handshake_timer = timer;
    uv_timer_start(timer, on_handshake_timeout,
                   TRELLIS_HANDSHAKE_TIMEOUT_MS, 0);
}

static void stop_handshake_timer(trellis_peer_conn_t *pc)
{
    if (!pc->handshake_timer)
        return;
    uv_timer_stop(pc->handshake_timer);
    uv_close((uv_handle_t *)pc->handshake_timer, hs_timer_close_cb);
    pc->handshake_timer = NULL;
}

// --- Incoming connection and message handling ---
static trellis_peer_conn_t *find_peer_by_conn(trellis_client_t *client,
                                               trellis_conn_t *conn)
{
    for (size_t i = 0; i < client->peer_conn_count; i++) {
        if (client->peers[i].conn == conn)
            return &client->peers[i];
    }
    return NULL;
}

static trellis_err_t send_handshake_msg(trellis_client_t *client,
                                        trellis_conn_t *conn,
                                        uint16_t hs_type,
                                        const uint8_t *payload,
                                        size_t payload_len,
                                        const uint8_t *target_obfs_pk)
{
    trellis_message_t msg = {0};
    msg.version = TRELLIS_PROTOCOL_VERSION;
    msg.type = hs_type;
    msg.sequence = client->next_sequence++;
    msg.timestamp = trellis_now_ms();
    msg.source = client->identity.fingerprint;
    msg.payload_len = (uint32_t)payload_len;
    msg.payload = (uint8_t *)payload;

    trellis_err_t err = trellis_message_sign(&msg, &client->identity);
    if (err != TRELLIS_OK)
        return err;

    trellis_buf_t wire = {0};
    err = trellis_message_serialize(&msg, &wire);
    if (err != TRELLIS_OK)
        return err;

    /*
     * Obfuscate with the RECEIVER's public key so both sides can agree
     * on the same obfuscation key: the receiver always decrypts with
     * its own key.  If target_obfs_pk is NULL (unknown target), send
     * the raw serialized message and rely on the receiver's fallback.
     */
    if (target_obfs_pk) {
        uint8_t obfs_key[TRELLIS_OBFS_KEY_LEN];
        trellis_derive_obfs_key(target_obfs_pk,
                                TRELLIS_X25519_PK_LEN, obfs_key);

        trellis_buf_t obfuscated = {0};
        err = trellis_handshake_obfuscate(wire.data, wire.len, obfs_key,
                                          &obfuscated);
        trellis_buf_free(&wire);

        if (err != TRELLIS_OK) {
            sodium_memzero(obfs_key, sizeof(obfs_key));
            return err;
        }

        /* Wrap with a probe guard keyed to the RECEIVER's obfs_key so
           the receiver's own probe_guard can validate it. */
        trellis_probe_guard_t *target_pg =
            trellis_probe_guard_new(obfs_key, sizeof(obfs_key));
        sodium_memzero(obfs_key, sizeof(obfs_key));

        if (target_pg) {
            trellis_buf_t guarded = {0};
            if (trellis_probe_guard_wrap(target_pg, obfuscated.data,
                    obfuscated.len, &guarded) == TRELLIS_OK && guarded.data) {
                trellis_buf_free(&obfuscated);
                obfuscated = guarded;
            }
            trellis_probe_guard_free(target_pg);
        }

        err = trellis_conn_send(conn, obfuscated.data, obfuscated.len,
                                NULL, NULL);
        trellis_buf_free(&obfuscated);
        return err;
    }

    err = trellis_conn_send(conn, wire.data, wire.len, NULL, NULL);
    trellis_buf_free(&wire);
    return err;
}

static void send_subscribe_to_peer(trellis_client_t *client,
                                    trellis_peer_conn_t *pc,
                                    const char *garden_name)
{
    trellis_message_t msg = {0};
    msg.version = TRELLIS_PROTOCOL_VERSION;
    msg.type = TRELLIS_MSG_SUBSCRIBE;
    msg.sequence = client->next_sequence++;
    msg.timestamp = trellis_now_ms();
    msg.source = client->identity.fingerprint;
    msg.payload_len = (uint32_t)strlen(garden_name);
    msg.payload = (uint8_t *)garden_name;

    if (trellis_message_sign(&msg, &client->identity) != TRELLIS_OK)
        return;

    trellis_buf_t wire = {0};
    if (trellis_message_serialize(&msg, &wire) != TRELLIS_OK)
        return;

    const uint8_t *send_data = wire.data;
    size_t send_len = wire.len;
    trellis_buf_t encrypted = {0};

    if (pc->handshake_complete && pc->ratchet) {
        if (trellis_ratchet_encrypt(pc->ratchet, wire.data, wire.len,
                                     &encrypted) == TRELLIS_OK) {
            send_data = encrypted.data;
            send_len  = encrypted.len;
        }
    }

    trellis_morph_t *morph = pc->morph ? pc->morph : client->morph;
    trellis_buf_t morphed = {0};
    if (trellis_morph_encode(morph, send_data, send_len, &morphed) == TRELLIS_OK) {
        trellis_conn_send(pc->conn, morphed.data, morphed.len, NULL, NULL);
        trellis_buf_free(&morphed);
    }
    if (encrypted.data)
        trellis_buf_free(&encrypted);
    trellis_buf_free(&wire);
}

static void finalize_handshake(trellis_client_t *client,
                               trellis_peer_conn_t *pc)
{
    pc->ratchet = trellis_ratchet_init_from_handshake(pc->handshake);
    if (!pc->ratchet) {
        return;
    }

    pc->handshake_complete = true;
    stop_handshake_timer(pc);

    trellis_identity_public_t remote_pub;
    if (trellis_handshake_remote_identity(pc->handshake, &remote_pub) == TRELLIS_OK) {
        trellis_fingerprint_t old_fp = pc->remote_fp;
        pc->remote_id = remote_pub;
        pc->remote_fp = remote_pub.fingerprint;

        /* Remove the stale DHT entry (keyed on hash-of-address)
           and replace it with the real fingerprint + identity so
           onion routing can derive per-hop keys. */
        if (!trellis_fingerprint_eq(&old_fp, &remote_pub.fingerprint))
            trellis_dht_remove_peer(client->dht, &old_fp);

        trellis_peer_info_t dht_entry = {0};
        dht_entry.fingerprint = remote_pub.fingerprint;
        dht_entry.identity = remote_pub;
        if (pc->addr[0])
            memcpy(dht_entry.addr, pc->addr, sizeof(dht_entry.addr));
        dht_entry.last_seen = trellis_now_ms();
        dht_entry.reliability = 1.0;
        trellis_err_t add_err = trellis_dht_add_peer(client->dht, &dht_entry);
        if (add_err == TRELLIS_ERR_RATE_LIMITED)
            fprintf(stderr, "[client] DHT frozen (eclipse); peer not added to routing table\n");

        /* Deduplicate: only the side with the higher fingerprint
           closes duplicate connections.  This ensures both peers
           agree on which connection survives. */
        if (memcmp(client->identity.fingerprint.bytes,
                   remote_pub.fingerprint.bytes,
                   TRELLIS_FINGERPRINT_LEN) > 0) {
            for (size_t di = 0; di < client->peer_conn_count; ) {
                trellis_peer_conn_t *other = &client->peers[di];
                if (other != pc &&
                    trellis_fingerprint_eq(&other->remote_fp,
                                           &remote_pub.fingerprint)) {
                    if (other->conn)
                        trellis_conn_close(other->conn, NULL, NULL);
                    other->conn = NULL;
                    trellis_fingerprint_t keep_fp = remote_pub.fingerprint;
                    trellis_conn_t *keep_conn = pc->conn;
                    if (other->handshake_timer)
                        stop_handshake_timer(other);
                    if (other->ratchet)
                        trellis_ratchet_free(other->ratchet);
                    if (other->handshake)
                        trellis_handshake_free(other->handshake);
                    if (other->morph)
                        trellis_morph_free(other->morph);
                    client->peers[di] = client->peers[--client->peer_conn_count];
                    pc = NULL;
                    for (size_t ri = 0; ri < client->peer_conn_count; ri++) {
                        if (client->peers[ri].conn == keep_conn &&
                            trellis_fingerprint_eq(&client->peers[ri].remote_fp,
                                                   &keep_fp)) {
                            pc = &client->peers[ri];
                            break;
                        }
                    }
                    if (!pc) return;
                } else {
                    di++;
                }
            }
        }
    }

    // Per-connection morph context with session key for metamorphic mode.
    trellis_morph_config_t mcfg = trellis_morph_config_default();
    mcfg.metamorphic = client->config.morph_metamorphic;
    mcfg.cover_traffic_interval_ms = client->config.cover_traffic_ms;
    pc->morph = trellis_morph_new(&mcfg);

    uint8_t session_key[32];
    if (trellis_handshake_session_key(pc->handshake, session_key) == TRELLIS_OK) {
        trellis_morph_set_session_key(client->morph, session_key, 32);
        if (pc->morph)
            trellis_morph_set_session_key(pc->morph, session_key, 32);

        /* DPI Defense: derive a unique frame tag from the session key so
           each connection's "magic" is unpredictable. */
        trellis_conn_derive_frame_tag(pc->conn, session_key, 32);

        /* DPI Defense: set up wire-level morph and camouflage on the
           connection so transport framing is obfuscated. */
        trellis_morph_config_t wire_mcfg = trellis_morph_config_default();
        wire_mcfg.metamorphic = client->config.morph_metamorphic;
        trellis_morph_t *wire_morph = trellis_morph_new(&wire_mcfg);
        if (wire_morph) {
            trellis_morph_set_session_key(wire_morph, session_key, 32);
            trellis_conn_set_wire_morph(pc->conn, wire_morph);
        }

        bool is_initiator = (trellis_handshake_role(pc->handshake) == TRELLIS_ROLE_INITIATOR);
        /* When using the TLS transport, the connection is already inside a
         * real TLS 1.3 tunnel.  Applying a camouflage mimic on top creates
         * detectable double-wrapping.  Use NONE unless the caller explicitly
         * configured a non-default type for TLS. */
        trellis_camouflage_type_t camo_type = client->config.wire_camouflage_type;
        if (client->config.transport_mode == TRELLIS_TRANSPORT_TLS &&
            camo_type == TRELLIS_CAMO_RAW_OBFS) {
            camo_type = TRELLIS_CAMO_NONE;
        }
        if (camo_type != TRELLIS_CAMO_NONE) {
            trellis_camouflage_t *wire_camo = trellis_camouflage_new(
                camo_type, session_key, 32, is_initiator);
            if (wire_camo)
                trellis_conn_set_wire_camo(pc->conn, wire_camo);
        }

        sodium_memzero(session_key, sizeof(session_key));
    }

    trellis_event_data_t ev = {0};
    ev.event = TRELLIS_EV_HANDSHAKE_COMPLETE;
    ev.peer = pc->remote_fp;
    trellis_emit(client, &ev);

    // Send peer exchange after handshake completes.
    send_peer_exchange(client, pc);

    // Advertise our capabilities so the peer can enforce them at their end.
    {
        trellis_capabilities_t local_caps = trellis_capabilities_default();
        if (client->exit_capable)
            trellis_capabilities_add_feature(&local_caps, "exit");
        trellis_buf_t caps_payload = {0};
        if (trellis_capabilities_serialize(&local_caps, &caps_payload) == TRELLIS_OK) {
            trellis_message_t cmsg = {0};
            cmsg.version     = TRELLIS_PROTOCOL_VERSION;
            cmsg.type        = TRELLIS_MSG_CAPABILITIES;
            cmsg.sequence    = client->next_sequence++;
            cmsg.timestamp   = trellis_now_ms();
            cmsg.source      = client->identity.fingerprint;
            cmsg.target      = pc->remote_fp;
            cmsg.payload     = caps_payload.data;
            cmsg.payload_len = (uint32_t)caps_payload.len;

            if (trellis_message_sign(&cmsg, &client->identity) == TRELLIS_OK) {
                trellis_buf_t cwire = {0};
                if (trellis_message_serialize(&cmsg, &cwire) == TRELLIS_OK) {
                    const uint8_t *sd = cwire.data;
                    size_t sl = cwire.len;
                    trellis_buf_t enc = {0};
                    if (pc->ratchet &&
                        trellis_ratchet_encrypt(pc->ratchet, cwire.data,
                                                cwire.len, &enc) == TRELLIS_OK) {
                        sd = enc.data;
                        sl = enc.len;
                    }
                    trellis_morph_t *m = pc->morph ? pc->morph : client->morph;
                    trellis_buf_t morphed = {0};
                    if (trellis_morph_encode(m, sd, sl, &morphed) == TRELLIS_OK) {
                        trellis_conn_send(pc->conn, morphed.data,
                                          morphed.len, NULL, NULL);
                        trellis_buf_free(&morphed);
                    }
                    if (enc.data) trellis_buf_free(&enc);
                    trellis_buf_free(&cwire);
                }
            }
            trellis_buf_free(&caps_payload);
        }
        trellis_capabilities_free(&local_caps);
    }

    // Re-send SUBSCRIBE for all local gardens so the new peer registers us.
    for (size_t gi = 0; gi < client->garden_count; gi++) {
        if (client->gardens[gi].name)
            send_subscribe_to_peer(client, pc, client->gardens[gi].name);
    }

    // Send our alias so the new peer can display our name.
    if (client->alias) {
        send_subscribe_to_peer(client, pc, "__alias__");
        trellis_message_t amsg = {0};
        amsg.version = TRELLIS_PROTOCOL_VERSION;
        amsg.type = TRELLIS_MSG_ALIAS;
        amsg.sequence = client->next_sequence++;
        amsg.timestamp = trellis_now_ms();
        amsg.source = client->identity.fingerprint;
        amsg.payload_len = (uint32_t)strlen(client->alias);
        amsg.payload = (uint8_t *)client->alias;

        if (trellis_message_sign(&amsg, &client->identity) == TRELLIS_OK) {
            trellis_buf_t awire = {0};
            if (trellis_message_serialize(&amsg, &awire) == TRELLIS_OK) {
                const uint8_t *sd = awire.data;
                size_t sl = awire.len;
                trellis_buf_t encrypted = {0};

                if (pc->handshake_complete && pc->ratchet) {
                    if (trellis_ratchet_encrypt(pc->ratchet, awire.data,
                                                 awire.len, &encrypted)
                            == TRELLIS_OK) {
                        sd = encrypted.data;
                        sl = encrypted.len;
                    }
                }

                trellis_morph_t *morph = pc->morph ? pc->morph : client->morph;
                trellis_buf_t morphed = {0};
                if (trellis_morph_encode(morph, sd, sl, &morphed) == TRELLIS_OK) {
                    trellis_conn_send(pc->conn, morphed.data, morphed.len,
                                      NULL, NULL);
                    trellis_buf_free(&morphed);
                }
                if (encrypted.data) trellis_buf_free(&encrypted);
                trellis_buf_free(&awire);
            }
        }
    }
}

// --- Peer exchange ---
static void send_peer_exchange(trellis_client_t *client,
                               trellis_peer_conn_t *target_pc)
{
    if (!client || !target_pc || !target_pc->conn)
        return;

    /*
     * Wire format: [2-byte count][per peer: 32-byte fp + 2-byte addr_len + addr]
     * Max payload: 2 + peer_count * (32 + 2 + 256) = reasonable
     */
    size_t max_payload = 2 + client->peer_conn_count * (TRELLIS_FINGERPRINT_LEN + 2 + 256);
    uint8_t *payload = malloc(max_payload);
    if (!payload)
        return;

    size_t off = 2;
    uint16_t count = 0;

    for (size_t i = 0; i < client->peer_conn_count; i++) {
        trellis_peer_conn_t *pc = &client->peers[i];
        if (!pc->handshake_complete)
            continue;
        if (trellis_fingerprint_eq(&pc->remote_fp, &target_pc->remote_fp))
            continue;

        memcpy(payload + off, pc->remote_fp.bytes, TRELLIS_FINGERPRINT_LEN);
        off += TRELLIS_FINGERPRINT_LEN;

        const char *addr = pc->addr[0] ? pc->addr :
            trellis_dht_lookup_addr(client->dht, &pc->remote_fp);

        uint16_t addr_len = addr ? (uint16_t)strlen(addr) : 0;
        payload[off] = (addr_len >> 8) & 0xff;
        payload[off + 1] = addr_len & 0xff;
        off += 2;
        if (addr_len > 0) {
            memcpy(payload + off, addr, addr_len);
            off += addr_len;
        }
        count++;
    }

    payload[0] = (count >> 8) & 0xff;
    payload[1] = count & 0xff;

    if (count == 0) {
        free(payload);
        return;
    }

    const uint8_t *pk = target_pc->handshake_complete
                        ? target_pc->remote_id.x25519_pk : NULL;
    send_handshake_msg(client, target_pc->conn, TRELLIS_MSG_PEER_EXCHANGE,
                       payload, off, pk);
    free(payload);
}

static void handle_peer_exchange(trellis_client_t *client,
                                 const uint8_t *payload, size_t len)
{
    if (len < 2)
        return;

    uint16_t count = ((uint16_t)payload[0] << 8) | payload[1];
    size_t off = 2;

    for (uint16_t i = 0; i < count && off < len; i++) {
        if (off + TRELLIS_FINGERPRINT_LEN + 2 > len)
            break;

        trellis_fingerprint_t fp;
        memcpy(fp.bytes, payload + off, TRELLIS_FINGERPRINT_LEN);
        off += TRELLIS_FINGERPRINT_LEN;

        uint16_t addr_len = ((uint16_t)payload[off] << 8) | payload[off + 1];
        off += 2;

        char addr[256] = {0};
        if (addr_len > 0 && addr_len < 256 && off + addr_len <= len) {
            memcpy(addr, payload + off, addr_len);
            off += addr_len;
        } else {
            off += addr_len;
        }

        if (trellis_fingerprint_eq(&fp, &client->identity.fingerprint))
            continue;
        if (trellis_client_find_peer(client, &fp))
            continue;

        // Add to DHT.
        trellis_peer_info_t pi = {0};
        pi.fingerprint = fp;
        if (addr[0]) {
            size_t alen = strlen(addr);
            if (alen >= sizeof(pi.addr)) alen = sizeof(pi.addr) - 1;
            memcpy(pi.addr, addr, alen);
        }
        pi.last_seen = trellis_now_ms();
        pi.reliability = 0.5;
        (void)trellis_dht_add_peer(client->dht, &pi);

        // Attempt direct connection if we have an address.
        if (addr[0] && client->transport) {
            trellis_transport_connect(client->transport, addr,
                                      on_seed_connected, client);
        }
    }
}

static void send_dht_rpc(trellis_client_t *client, trellis_conn_t *conn,
                         uint16_t type, const uint8_t *payload, size_t len)
{
    trellis_peer_conn_t *pc = find_peer_by_conn(client, conn);
    const uint8_t *pk = (pc && pc->handshake_complete)
                        ? pc->remote_id.x25519_pk : NULL;
    send_handshake_msg(client, conn, type, payload, len, pk);
}

static void handle_ping(trellis_client_t *client, trellis_conn_t *conn,
                        const trellis_message_t *msg)
{
    /*
     * Respond with PONG carrying: [32-byte fingerprint][2-byte port][addr]
     */
    const char *addr = trellis_client_listen_addr(client);
    size_t addr_len = addr ? strlen(addr) : 0;
    size_t plen = TRELLIS_FINGERPRINT_LEN + 2 + addr_len;
    uint8_t *payload = malloc(plen);
    if (!payload) return;

    memcpy(payload, client->identity.fingerprint.bytes, TRELLIS_FINGERPRINT_LEN);
    payload[TRELLIS_FINGERPRINT_LEN] = (client->listen_port >> 8) & 0xff;
    payload[TRELLIS_FINGERPRINT_LEN + 1] = client->listen_port & 0xff;
    if (addr_len > 0)
        memcpy(payload + TRELLIS_FINGERPRINT_LEN + 2, addr, addr_len);

    // Include STUN-resolved public address if available.
    if (client->nat && trellis_nat_mapped_address(client->nat) &&
        trellis_nat_mapped_address(client->nat)[0]) {
        const char *ma = trellis_nat_mapped_address(client->nat);
        uint16_t mp = trellis_nat_mapped_port(client->nat);
        snprintf(client->public_addr, sizeof(client->public_addr),
                 "%s:%u", ma, mp);
    }

    send_dht_rpc(client, conn, TRELLIS_MSG_PONG, payload, plen);
    free(payload);

    trellis_peer_conn_t *pc = find_peer_by_conn(client, conn);
    if (pc)
        trellis_dht_update_peer_seen(client->dht, &pc->remote_fp);
}

static void handle_pong(trellis_client_t *client, trellis_conn_t *conn,
                        const trellis_message_t *msg)
{
    if (!msg->payload || msg->payload_len < TRELLIS_FINGERPRINT_LEN + 2)
        return;

    trellis_fingerprint_t fp;
    memcpy(fp.bytes, msg->payload, TRELLIS_FINGERPRINT_LEN);

    trellis_peer_conn_t *pc = find_peer_by_conn(client, conn);
    if (pc) {
        pc->last_pong = trellis_now_ms();

        // Update the peer's real fingerprint if we now know it.
        if (!trellis_fingerprint_eq(&pc->remote_fp, &fp) && pc->handshake_complete) {
            // Fingerprint came from handshake, don't override.
        }
    }

    trellis_dht_update_peer_seen(client->dht, &fp);
}

static void handle_find_node(trellis_client_t *client, trellis_conn_t *conn,
                             const trellis_message_t *msg)
{
    if (!msg->payload || msg->payload_len < TRELLIS_FINGERPRINT_LEN)
        return;

    trellis_fingerprint_t target;
    memcpy(target.bytes, msg->payload, TRELLIS_FINGERPRINT_LEN);

    trellis_peer_info_t closest[TRELLIS_DHT_K];
    size_t n = trellis_dht_find_closest(client->dht, &target, closest, TRELLIS_DHT_K);

    size_t max_len = 2 + n * (TRELLIS_FINGERPRINT_LEN + 2 + 256);
    uint8_t *payload = malloc(max_len);
    if (!payload) return;

    payload[0] = (n >> 8) & 0xff;
    payload[1] = n & 0xff;
    size_t off = 2;
    for (size_t i = 0; i < n; i++) {
        memcpy(payload + off, closest[i].fingerprint.bytes, TRELLIS_FINGERPRINT_LEN);
        off += TRELLIS_FINGERPRINT_LEN;
        uint16_t alen = (uint16_t)strlen(closest[i].addr);
        payload[off] = (alen >> 8) & 0xff;
        payload[off + 1] = alen & 0xff;
        off += 2;
        if (alen > 0) {
            memcpy(payload + off, closest[i].addr, alen);
            off += alen;
        }
    }

    send_dht_rpc(client, conn, TRELLIS_MSG_FIND_NODE, payload, off);
    free(payload);
}

static void handle_store(trellis_client_t *client,
                         const trellis_message_t *msg)
{
    if (!msg->payload || msg->payload_len < 36)
        return;

    // [32-byte key][4-byte value_len][value]
    const uint8_t *key = msg->payload;
    uint32_t val_len = ((uint32_t)msg->payload[32] << 24) |
                       ((uint32_t)msg->payload[33] << 16) |
                       ((uint32_t)msg->payload[34] << 8) |
                       msg->payload[35];
    if (val_len > msg->payload_len - 36)
        return;

    trellis_dht_store(client->dht, key, 32,
                      msg->payload + 36, val_len, NULL, NULL);
}

static bool handle_handshake(trellis_client_t *client,
                             trellis_conn_t *conn,
                             const trellis_message_t *msg)
{
    trellis_peer_conn_t *pc = find_peer_by_conn(client, conn);
    if (!pc || !pc->handshake)
        return false;

    switch (msg->type) {
    case TRELLIS_MSG_HANDSHAKE_1: {
        trellis_buf_t msg2 = {0};
        trellis_err_t err = trellis_handshake_process_msg1(
            pc->handshake, msg->payload, msg->payload_len, &msg2);
        if (err != TRELLIS_OK)
            return true;
        trellis_identity_public_t remote_pub;
        const uint8_t *target_pk = NULL;
        if (trellis_handshake_remote_identity(pc->handshake, &remote_pub)
                == TRELLIS_OK)
            target_pk = remote_pub.x25519_pk;
        trellis_err_t send_err = send_handshake_msg(client, conn, TRELLIS_MSG_HANDSHAKE_2,
                           msg2.data, msg2.len, target_pk);
        trellis_buf_free(&msg2);
        return true;
    }

    case TRELLIS_MSG_HANDSHAKE_2: {
        trellis_buf_t msg3 = {0};
        trellis_err_t err = trellis_handshake_process_msg2(
            pc->handshake, msg->payload, msg->payload_len, &msg3);
        if (err != TRELLIS_OK)
            return true;
        trellis_identity_public_t remote_pub;
        const uint8_t *target_pk = NULL;
        if (trellis_handshake_remote_identity(pc->handshake, &remote_pub)
                == TRELLIS_OK)
            target_pk = remote_pub.x25519_pk;
        send_handshake_msg(client, conn, TRELLIS_MSG_HANDSHAKE_3,
                           msg3.data, msg3.len, target_pk);
        trellis_buf_free(&msg3);
        finalize_handshake(client, pc);
        return true;
    }

    case TRELLIS_MSG_HANDSHAKE_3: {
        trellis_err_t err = trellis_handshake_process_msg3(
            pc->handshake, msg->payload, msg->payload_len);
        if (err != TRELLIS_OK)
            return true;
        finalize_handshake(client, pc);
        return true;
    }

    default:
        return false;
    }
}

// --- Circuit data write callback (exit relay → onion return path) ---
static void circuit_data_write_cb(const uint8_t *data, size_t len,
                                   uint32_t stream_id, void *ctx)
{
    trellis_client_t *client = (trellis_client_t *)ctx;
    if (!client) return;
    if (!client->relay) {
        fprintf(stderr, "[exit-relay] dropping return data for stream %08x "
                "(relay subsystem unavailable)\n", stream_id);
        return;
    }

    circuit_entry_t *ce = trellis_circuit_table_lookup(
        &client->relay->circuit_table, stream_id);
    if (!ce) {
        fprintf(stderr, "[exit-relay] no circuit entry for stream %08x\n",
                stream_id);
        return;
    }

    size_t msg_len = 4 + len;
    uint8_t *buf = malloc(msg_len);
    if (!buf) return;

    buf[0] = (uint8_t)(stream_id >> 24);
    buf[1] = (uint8_t)(stream_id >> 16);
    buf[2] = (uint8_t)(stream_id >> 8);
    buf[3] = (uint8_t)(stream_id);
    memcpy(buf + 4, data, len);

    trellis_peer_conn_t *prev_pc =
        trellis_client_find_peer(client, &ce->prev_hop);
    if (prev_pc && prev_pc->conn) {
        trellis_message_t cd_msg = {0};
        cd_msg.version = TRELLIS_PROTOCOL_VERSION;
        cd_msg.type = TRELLIS_MSG_CIRCUIT_DATA;
        cd_msg.timestamp = trellis_now_ms();
        cd_msg.target = ce->prev_hop;
        cd_msg.payload_len = (uint32_t)msg_len;
        cd_msg.payload = buf;

        trellis_buf_t wire = {0};
        if (trellis_message_serialize(&cd_msg, &wire) == TRELLIS_OK) {
            trellis_conn_send(prev_pc->conn, wire.data, wire.len,
                              NULL, NULL);
            trellis_buf_free(&wire);
        }
    }

    free(buf);
}

// --- Onion peel ---
static void handle_onion(trellis_client_t *client, trellis_conn_t *conn,
                         const trellis_message_t *msg)
{
    if (!msg->payload || msg->payload_len == 0)
        return;

    // Onion replay protection: drop packets we've already processed.
    if (client->onion_replay) {
        uint8_t digest[32];
        crypto_hash_sha256(digest, msg->payload, msg->payload_len);
        dedup_set_t *replay = (dedup_set_t *)client->onion_replay;
        if (dedup_set_contains(replay, digest))
            return;
        dedup_set_insert(replay, digest);
    }

    trellis_kem_keypair_t kp;
    memcpy(kp.x25519_pk, client->identity.x25519_pk, TRELLIS_X25519_PK_LEN);
    memcpy(kp.x25519_sk, client->identity.x25519_sk, TRELLIS_X25519_SK_LEN);
    memcpy(kp.ml_kem_pk, client->identity.ml_kem_pk, TRELLIS_ML_KEM_1024_PK_LEN);
    memcpy(kp.ml_kem_sk, client->identity.ml_kem_sk, TRELLIS_ML_KEM_1024_SK_LEN);

    trellis_buf_t out = {0};
    trellis_fingerprint_t next_hop = {0};
    uint8_t peel_layer_key[32] = {0};

    if (trellis_onion_peel_key(&kp, msg->payload, msg->payload_len,
                               &out, &next_hop, peel_layer_key) != TRELLIS_OK) {
        sodium_memzero(&kp, sizeof(kp));
        return;
    }
    sodium_memzero(&kp, sizeof(kp));

    static const trellis_fingerprint_t zero_fp = {0};
    bool is_final = trellis_fingerprint_eq(&next_hop, &client->identity.fingerprint) ||
                    trellis_fingerprint_eq(&next_hop, &zero_fp);

    /*
     * Circuit-mode onions (from route_onion) embed a 4-byte stream_id
     * after next_hop_fp.  Extract it from the peeled inner payload for
     * intermediate hops to record reverse-path state.
     */
    uint32_t stream_id = 0;
    const uint8_t *inner_data = out.data;
    size_t inner_len = out.len;

    if (!is_final && inner_len >= 4) {
        stream_id = ((uint32_t)out.data[0] << 24) |
                    ((uint32_t)out.data[1] << 16) |
                    ((uint32_t)out.data[2] << 8) |
                    (uint32_t)out.data[3];
        if (stream_id != 0) {
            inner_data = out.data + 4;
            inner_len = out.len - 4;

            // Record reverse-path: stream_id -> prev_hop (sender of this onion)
            if (client->relay) {
                trellis_fingerprint_t prev_hop = {0};
                if (conn && conn->peer_fingerprint_valid)
                    prev_hop = conn->peer_fingerprint;
                else if (msg->source.bytes[0] != 0)
                    prev_hop = msg->source;

                /* RELAY_EARLY: check if a circuit entry already exists for
                 * this stream and enforce maximum hop depth */
                circuit_entry_t *existing = trellis_circuit_table_lookup(
                    &client->relay->circuit_table, stream_id);
                if (existing && existing->hop_count >= CIRCUIT_MAX_HOPS) {
                    fprintf(stderr, "[onion] circuit %08x exceeded max hop "
                            "depth %d — dropping\n",
                            stream_id, CIRCUIT_MAX_HOPS);
                    trellis_buf_free(&out);
                    return;
                }

                trellis_circuit_table_insert(
                    &client->relay->circuit_table,
                    stream_id, &prev_hop, peel_layer_key);

                // Increment hop count on the newly inserted entry.
                circuit_entry_t *ce = trellis_circuit_table_lookup(
                    &client->relay->circuit_table, stream_id);
                if (ce) {
                    ce->hop_count = existing
                        ? (uint8_t)(existing->hop_count + 1)
                        : 1;
                }
            }
        }
    }

    if (is_final) {
        // Final destination: extract stream_id and route appropriately.
        uint32_t final_sid = 0;
        const uint8_t *final_data = out.data;
        size_t final_len = out.len;

        if (out.len >= 4) {
            final_sid = ((uint32_t)out.data[0] << 24) |
                        ((uint32_t)out.data[1] << 16) |
                        ((uint32_t)out.data[2] << 8) |
                        (uint32_t)out.data[3];
            if (final_sid != 0) {
                final_data = out.data + 4;
                final_len = out.len - 4;
            }
        }

        if (final_sid != 0 && client->exit_capable && client->exit_relay) {
            /* Exit stream via onion circuit: record reverse-path and
             * pass upstream context so the exit relay can send data back */
            trellis_fingerprint_t prev_hop = {0};
            if (conn && conn->peer_fingerprint_valid)
                prev_hop = conn->peer_fingerprint;
            else if (msg->source.bytes[0] != 0)
                prev_hop = msg->source;

            if (client->relay) {
                trellis_circuit_table_insert(
                    &client->relay->circuit_table,
                    final_sid, &prev_hop, peel_layer_key);
            }

            exit_circuit_upstream_t cu;
            cu.write_fn = circuit_data_write_cb;
            cu.ctx = client;
            cu.stream_id = final_sid;

            trellis_exit_relay_handle_stream(
                client->exit_relay,
                final_data, final_len, final_sid, &cu);
        } else {
            trellis_client_on_data(NULL, final_data, final_len,
                                   TRELLIS_OK, client);
        }
    } else {
        // Derive prev_hop for flow tracking.
        trellis_fingerprint_t onion_prev = {0};
        if (conn && conn->peer_fingerprint_valid)
            onion_prev = conn->peer_fingerprint;
        else if (msg->source.bytes[0] != 0)
            onion_prev = msg->source;

        trellis_peer_conn_t *next_pc =
            trellis_client_find_peer(client, &next_hop);
        if (next_pc && next_pc->conn) {
            // Route through relay flow control (auto-creates circuit if needed)
            if (client->relay) {
                trellis_relay_create_circuit(client->relay,
                                             &onion_prev, &next_hop);
                trellis_err_t fwd_err = trellis_relay_forward(
                    client->relay, &onion_prev, &next_hop,
                    inner_data, inner_len);
                if (fwd_err == TRELLIS_ERR_RATE_LIMITED) {
                    trellis_buf_free(&out);
                    return;
                }
            }
            if (client->incentive)
                trellis_incentive_record_relay(client->incentive,
                                               &onion_prev, inner_len);
            trellis_message_t fwd = {0};
            fwd.version = TRELLIS_PROTOCOL_VERSION;
            fwd.type = TRELLIS_MSG_ONION;
            fwd.timestamp = trellis_now_ms();
            fwd.target = next_hop;
            fwd.payload_len = (uint32_t)inner_len;
            fwd.payload = (uint8_t *)inner_data;

            trellis_buf_t wire = {0};
            if (trellis_message_serialize(&fwd, &wire) == TRELLIS_OK) {
                trellis_conn_send(next_pc->conn, wire.data, wire.len,
                                  NULL, NULL);
                trellis_buf_free(&wire);
            }

            // Generate SENDME back to sender when recv threshold reached.
            if (client->relay &&
                trellis_relay_needs_sendme(client->relay,
                                           &onion_prev, &next_hop)) {
                trellis_peer_conn_t *src_pc =
                    trellis_client_find_peer(client, &onion_prev);
                if (src_pc && src_pc->conn) {
                    uint8_t sm_buf[2 * TRELLIS_FINGERPRINT_LEN + 8];
                    memcpy(sm_buf, onion_prev.bytes,
                           TRELLIS_FINGERPRINT_LEN);
                    memcpy(sm_buf + TRELLIS_FINGERPRINT_LEN,
                           next_hop.bytes, TRELLIS_FINGERPRINT_LEN);
                    uint64_t echo_ts = trellis_now_ms();
                    memcpy(sm_buf + 2 * TRELLIS_FINGERPRINT_LEN,
                           &echo_ts, 8);
                    trellis_message_t sm_msg = {0};
                    sm_msg.version = TRELLIS_PROTOCOL_VERSION;
                    sm_msg.type = TRELLIS_MSG_SENDME;
                    sm_msg.timestamp = trellis_now_ms();
                    sm_msg.target = onion_prev;
                    sm_msg.payload = sm_buf;
                    sm_msg.payload_len = sizeof(sm_buf);
                    trellis_buf_t sm_wire = {0};
                    if (trellis_message_serialize(&sm_msg, &sm_wire) == TRELLIS_OK) {
                        trellis_conn_send(src_pc->conn, sm_wire.data,
                                          sm_wire.len, NULL, NULL);
                        trellis_buf_free(&sm_wire);
                    }
                }
            }
        }
    }

    trellis_buf_free(&out);
}

static void dispatch_message(trellis_client_t *client, trellis_conn_t *conn,
                             trellis_message_t *msg)
{
    const uint8_t *data = NULL;
    size_t len = 0;

    /* Handle relay-forwarded messages.  Only authenticated peers (whose
     * handshake is complete and signature verified) may use relay services,
     * otherwise an anonymous connection could abuse the relay to forward
     * arbitrary traffic to other peers. */
    if (msg->type == TRELLIS_MSG_RELAY && client->relay) {
        trellis_peer_conn_t *relay_sender = find_peer_by_conn(client, conn);
        if (!relay_sender || !relay_sender->handshake_complete)
            return;
        if (trellis_message_verify(msg, &relay_sender->remote_id) != TRELLIS_OK)
            return;
        if (msg->payload && msg->payload_len > 2 * TRELLIS_FINGERPRINT_LEN) {
            trellis_fingerprint_t relay_from, relay_to;
            memcpy(relay_from.bytes, msg->payload, TRELLIS_FINGERPRINT_LEN);
            memcpy(relay_to.bytes, msg->payload + TRELLIS_FINGERPRINT_LEN,
                   TRELLIS_FINGERPRINT_LEN);

            const uint8_t *inner = msg->payload + 2 * TRELLIS_FINGERPRINT_LEN;
            size_t inner_len = msg->payload_len - 2 * TRELLIS_FINGERPRINT_LEN;

            if (trellis_fingerprint_eq(&relay_to,
                                       &client->identity.fingerprint)) {
                trellis_client_on_data(conn, inner, inner_len,
                                       TRELLIS_OK, client);
            } else {
                trellis_relay_create_circuit(client->relay,
                                             &relay_from, &relay_to);
                trellis_err_t fwd_err = trellis_relay_forward(
                    client->relay, &relay_from, &relay_to,
                    inner, inner_len);
                if (fwd_err == TRELLIS_ERR_RATE_LIMITED)
                    return;
                if (client->incentive)
                    trellis_incentive_record_relay(client->incentive,
                                                   &relay_from, inner_len);
                trellis_peer_conn_t *dest_pc =
                    trellis_client_find_peer(client, &relay_to);
                if (dest_pc && dest_pc->conn) {
                    trellis_buf_t wire = {0};
                    if (trellis_message_serialize(msg, &wire) == TRELLIS_OK) {
                        trellis_conn_send(dest_pc->conn, wire.data, wire.len,
                                          NULL, NULL);
                        trellis_buf_free(&wire);
                    }
                }

                /* Generate SENDME back to sender when recv threshold reached.
                 * Payload: [from_fp][to_fp][echo_timestamp(8)] for RTT. */
                if (trellis_relay_needs_sendme(client->relay,
                                               &relay_from, &relay_to)) {
                    trellis_peer_conn_t *src_pc =
                        trellis_client_find_peer(client, &relay_from);
                    if (src_pc && src_pc->conn) {
                        uint8_t sm_buf[2 * TRELLIS_FINGERPRINT_LEN + 8];
                        memcpy(sm_buf, relay_from.bytes, TRELLIS_FINGERPRINT_LEN);
                        memcpy(sm_buf + TRELLIS_FINGERPRINT_LEN,
                               relay_to.bytes, TRELLIS_FINGERPRINT_LEN);
                        uint64_t echo_ts = trellis_now_ms();
                        memcpy(sm_buf + 2 * TRELLIS_FINGERPRINT_LEN,
                               &echo_ts, 8);
                        trellis_message_t sm_msg = {0};
                        sm_msg.version = TRELLIS_PROTOCOL_VERSION;
                        sm_msg.type = TRELLIS_MSG_SENDME;
                        sm_msg.timestamp = trellis_now_ms();
                        sm_msg.target = relay_from;
                        sm_msg.payload = sm_buf;
                        sm_msg.payload_len = sizeof(sm_buf);
                        trellis_buf_t sm_wire = {0};
                        if (trellis_message_serialize(&sm_msg, &sm_wire) == TRELLIS_OK) {
                            trellis_conn_send(src_pc->conn, sm_wire.data,
                                              sm_wire.len, NULL, NULL);
                            trellis_buf_free(&sm_wire);
                        }
                    }
                }
            }
        }
        return;
    }

    trellis_peer_conn_t *sender_pc = find_peer_by_conn(client, conn);
    trellis_fingerprint_t event_fp = sender_pc
        ? sender_pc->remote_fp
        : msg->source;

    /* Verify the hybrid signature (Ed25519 + ML-DSA-87) before processing.
     * We need the sender's verified public identity, which is only available
     * after the handshake is complete.  Unsigned messages (zero signature)
     * from an unauthenticated peer are silently dropped. */
    if (sender_pc && sender_pc->handshake_complete) {
        if (trellis_message_verify(msg, &sender_pc->remote_id) != TRELLIS_OK)
            return;
    }

    switch (msg->type) {
    case TRELLIS_MSG_MSG: {
        static const trellis_fingerprint_t zero_fp = {0};
        bool has_target = !trellis_fingerprint_eq(&msg->target, &zero_fp);
        bool for_us = trellis_fingerprint_eq(&msg->target,
                                              &client->identity.fingerprint);

        if (has_target && !for_us)
            break;

        trellis_event_data_t ev = {0};
        ev.event = TRELLIS_EV_MESSAGE;
        ev.peer = event_fp;
        ev.data = msg->payload;
        ev.data_len = msg->payload_len;

        if (!has_target) {
            for (size_t gi = 0; gi < client->garden_count; gi++) {
                if (trellis_garden_has_member(client->gardens[gi].garden,
                                              &event_fp)) {
                    ev.garden_name = client->gardens[gi].name;
                    break;
                }
            }
        }

        trellis_emit(client, &ev);
        break;
    }

    case TRELLIS_MSG_ALIAS: {
        trellis_event_data_t ev = {0};
        ev.event = TRELLIS_EV_ALIAS_CHANGED;
        ev.peer = event_fp;
        if (msg->payload && msg->payload_len > 0) {
            char *alias_str = malloc(msg->payload_len + 1);
            if (alias_str) {
                memcpy(alias_str, msg->payload, msg->payload_len);
                alias_str[msg->payload_len] = '\0';
                ev.alias = alias_str;
                trellis_emit(client, &ev);
                free(alias_str);
            }
        }
        break;
    }

    case TRELLIS_MSG_SUBSCRIBE:
        if (msg->payload && msg->payload_len > 0) {
            char *garden_name = malloc(msg->payload_len + 1);
            if (garden_name) {
                memcpy(garden_name, msg->payload, msg->payload_len);
                garden_name[msg->payload_len] = '\0';

                trellis_garden_t *sg = trellis_client_find_garden(client,
                                                                   garden_name);
                if (!sg) {
                    trellis_client_create_garden(client, garden_name,
                                                 TRELLIS_GARDEN_PUBLIC);
                    sg = trellis_client_find_garden(client, garden_name);
                }
                if (sg) {
                    trellis_err_t sub_err = trellis_garden_subscribe(sg, &event_fp);
                    if (sub_err == TRELLIS_OK) {
                        trellis_event_data_t ev = {0};
                        ev.event = TRELLIS_EV_GARDEN_SUBSCRIBED;
                        ev.peer = event_fp;
                        ev.garden_name = garden_name;
                        trellis_emit(client, &ev);
                    }
                }
                free(garden_name);
            }
        }
        break;

    case TRELLIS_MSG_UNSUBSCRIBE:
        if (msg->payload && msg->payload_len > 0) {
            char *garden_name = malloc(msg->payload_len + 1);
            if (garden_name) {
                memcpy(garden_name, msg->payload, msg->payload_len);
                garden_name[msg->payload_len] = '\0';

                trellis_garden_t *ug = trellis_client_find_garden(client,
                                                                   garden_name);
                if (ug) {
                    trellis_err_t unsub_err = trellis_garden_unsubscribe(ug, &event_fp);
                    if (unsub_err == TRELLIS_OK) {
                        trellis_event_data_t ev = {0};
                        ev.event = TRELLIS_EV_GARDEN_UNSUBSCRIBED;
                        ev.peer = event_fp;
                        ev.garden_name = garden_name;
                        trellis_emit(client, &ev);
                    }
                }
                free(garden_name);
            }
        }
        break;

    case TRELLIS_MSG_GOSSIP:
        if (client->gossip && msg->payload && msg->payload_len > 0) {
            trellis_gossip_receive(client->gossip, msg->payload,
                                   msg->payload_len, &event_fp);
            trellis_event_data_t ev = {0};
            ev.event = TRELLIS_EV_GOSSIP;
            ev.peer = event_fp;
            ev.data = msg->payload;
            ev.data_len = msg->payload_len;
            trellis_emit(client, &ev);
        }
        break;

    case TRELLIS_MSG_PEER_EXCHANGE:
        handle_peer_exchange(client, msg->payload, msg->payload_len);
        break;

    case TRELLIS_MSG_PING:
        handle_ping(client, conn, msg);
        break;

    case TRELLIS_MSG_PONG:
        handle_pong(client, conn, msg);
        break;

    case TRELLIS_MSG_FIND_NODE:
        handle_find_node(client, conn, msg);
        break;

    case TRELLIS_MSG_STORE:
        handle_store(client, msg);
        break;

    case TRELLIS_MSG_ONION:
        handle_onion(client, conn, msg);
        break;

    case TRELLIS_MSG_EXIT_STREAM:
        if (client->exit_capable && client->exit_relay &&
            msg->payload && msg->payload_len > 0) {
            /* Direct (non-onion) exit stream: no circuit return path.
             * stream_id 0 = untracked (fire-and-forget). */
            trellis_exit_relay_handle_stream(
                client->exit_relay,
                msg->payload, msg->payload_len,
                0, NULL);
        }
        break;

    case TRELLIS_MSG_SENDME:
        /* Flow control: the remote consumed cells and is ready for more.
         * Payload: [from_fp][to_fp][echo_timestamp(8)]. */
        if (client->relay && msg->payload &&
            msg->payload_len >= 2 * TRELLIS_FINGERPRINT_LEN) {
            trellis_fingerprint_t sm_from, sm_to;
            memcpy(sm_from.bytes, msg->payload, TRELLIS_FINGERPRINT_LEN);
            memcpy(sm_to.bytes, msg->payload + TRELLIS_FINGERPRINT_LEN,
                   TRELLIS_FINGERPRINT_LEN);
            trellis_relay_on_sendme(client->relay, &sm_from, &sm_to);

            // Extract echo timestamp and compute RTT for Vegas CC.
            if (msg->payload_len >= 2 * TRELLIS_FINGERPRINT_LEN + 8) {
                uint64_t echo_ts;
                memcpy(&echo_ts,
                       msg->payload + 2 * TRELLIS_FINGERPRINT_LEN, 8);
                uint64_t now = trellis_now_ms();
                if (now > echo_ts) {
                    uint64_t rtt_us = (now - echo_ts) * 1000;
                    relay_circuit_t *rc =
                        trellis_relay_find_circuit(client->relay,
                                                   &sm_from, &sm_to);
                    if (rc)
                        trellis_flow_vegas_update(rc, rtt_us);
                }
            }
        }
        break;

    case TRELLIS_MSG_CIRCUIT_DATA:
        // Reverse path for onion circuits: [stream_id(4)][payload]
        if (msg->payload && msg->payload_len > 4) {
            uint32_t sid = ((uint32_t)msg->payload[0] << 24) |
                           ((uint32_t)msg->payload[1] << 16) |
                           ((uint32_t)msg->payload[2] << 8) |
                           (uint32_t)msg->payload[3];
            const uint8_t *cd_payload = msg->payload + 4;
            size_t cd_len = msg->payload_len - 4;

            /* Relay forwarding: if we have a circuit table entry for this
             * stream_id, re-encrypt and forward towards the originator. */
            circuit_entry_t *ce = NULL;
            if (client->relay)
                ce = trellis_circuit_table_lookup(
                    &client->relay->circuit_table, sid);

            if (ce) {
                uint8_t *enc_payload = malloc(msg->payload_len);
                if (enc_payload) {
                    memcpy(enc_payload, msg->payload, 4);
                    uint8_t nonce[24] = {0};
                    memcpy(nonce, &sid, 4);
                    crypto_stream_xchacha20_xor_ic(
                        enc_payload + 4, cd_payload, cd_len,
                        nonce, 0, ce->layer_key);

                    trellis_peer_conn_t *prev_pc =
                        trellis_client_find_peer(client, &ce->prev_hop);
                    if (prev_pc && prev_pc->conn) {
                        trellis_message_t cd_msg = {0};
                        cd_msg.version = TRELLIS_PROTOCOL_VERSION;
                        cd_msg.type = TRELLIS_MSG_CIRCUIT_DATA;
                        cd_msg.timestamp = trellis_now_ms();
                        cd_msg.target = ce->prev_hop;
                        cd_msg.payload_len = (uint32_t)msg->payload_len;
                        cd_msg.payload = enc_payload;

                        trellis_buf_t wire = {0};
                        if (trellis_message_serialize(&cd_msg, &wire) == TRELLIS_OK) {
                            trellis_conn_send(prev_pc->conn, wire.data, wire.len,
                                              NULL, NULL);
                            trellis_buf_free(&wire);
                        }
                    }
                    free(enc_payload);
                }
            } else {
                /* No circuit entry: this is the originating client.
                 * Check if an exit stream is waiting for this data. */
                trellis_exit_stream_t *es =
                    trellis_exit_stream_find(client, sid);
                if (es && es->on_data) {
                    es->on_data(es, cd_payload, cd_len, es->ctx);
                } else {
                    trellis_client_on_data(conn, cd_payload, cd_len,
                                           TRELLIS_OK, client);
                }
                if (client->router && client->router->guard && sender_pc)
                    trellis_guard_path_bias_success(
                        client->router->guard, &sender_pc->remote_fp);
            }
        }
        break;

    case TRELLIS_MSG_CIRCUIT_DESTROY:
        /* Tear down a circuit: [stream_id(4)] -- propagate to next hop,
         * then remove from circuit table and relay circuits */
        if (msg->payload && msg->payload_len >= 4) {
            uint32_t destroy_sid = ((uint32_t)msg->payload[0] << 24) |
                                   ((uint32_t)msg->payload[1] << 16) |
                                   ((uint32_t)msg->payload[2] << 8) |
                                   (uint32_t)msg->payload[3];

            if (client->relay) {
                circuit_entry_t *ce = trellis_circuit_table_lookup(
                    &client->relay->circuit_table, destroy_sid);
                if (ce) {
                    // Propagate DESTROY to prev_hop.
                    trellis_peer_conn_t *prev_pc =
                        trellis_client_find_peer(client, &ce->prev_hop);
                    if (prev_pc && prev_pc->conn) {
                        trellis_message_t d_msg = {0};
                        d_msg.version = TRELLIS_PROTOCOL_VERSION;
                        d_msg.type = TRELLIS_MSG_CIRCUIT_DESTROY;
                        d_msg.timestamp = trellis_now_ms();
                        d_msg.target = ce->prev_hop;
                        d_msg.payload = msg->payload;
                        d_msg.payload_len = 4;
                        trellis_buf_t dw = {0};
                        if (trellis_message_serialize(&d_msg, &dw) == TRELLIS_OK) {
                            trellis_conn_send(prev_pc->conn, dw.data, dw.len,
                                              NULL, NULL);
                            trellis_buf_free(&dw);
                        }
                    }
                    trellis_circuit_table_remove(
                        &client->relay->circuit_table, destroy_sid);
                } else {
                    // No circuit entry: originating client -- circuit collapsed.
                    if (client->router && client->router->guard && sender_pc)
                        trellis_guard_path_bias_collapse(
                            client->router->guard, &sender_pc->remote_fp);
                }
            }
        }
        break;

    case TRELLIS_MSG_BW_PROBE:
        // Bandwidth probe: [probe_id(4)][payload]. Respond with ACK.
        if (sender_pc && msg->payload && msg->payload_len >= 4) {
            uint8_t ack_buf[8];
            memcpy(ack_buf, msg->payload, 4);
            uint32_t recv_sz = (uint32_t)msg->payload_len;
            ack_buf[4] = (uint8_t)(recv_sz >> 24);
            ack_buf[5] = (uint8_t)(recv_sz >> 16);
            ack_buf[6] = (uint8_t)(recv_sz >> 8);
            ack_buf[7] = (uint8_t)(recv_sz);
            trellis_message_t ack = {0};
            ack.version = TRELLIS_PROTOCOL_VERSION;
            ack.type = TRELLIS_MSG_BW_PROBE_ACK;
            ack.timestamp = trellis_now_ms();
            ack.target = sender_pc->remote_fp;
            ack.payload = ack_buf;
            ack.payload_len = sizeof(ack_buf);
            trellis_buf_t aw = {0};
            if (trellis_message_serialize(&ack, &aw) == TRELLIS_OK) {
                trellis_conn_send(sender_pc->conn, aw.data, aw.len,
                                  NULL, NULL);
                trellis_buf_free(&aw);
            }
        }
        break;

    case TRELLIS_MSG_BW_PROBE_ACK:
        // Bandwidth probe response: measure throughput from RTT.
        if (sender_pc && msg->payload && msg->payload_len >= 8) {
            uint32_t probe_id = ((uint32_t)msg->payload[0] << 24) |
                                ((uint32_t)msg->payload[1] << 16) |
                                ((uint32_t)msg->payload[2] << 8) |
                                (uint32_t)msg->payload[3];
            uint32_t recv_bytes = ((uint32_t)msg->payload[4] << 24) |
                                  ((uint32_t)msg->payload[5] << 16) |
                                  ((uint32_t)msg->payload[6] << 8) |
                                  (uint32_t)msg->payload[7];
            (void)probe_id;
            // Compute approximate bandwidth: bytes / rtt.
            if (sender_pc->latency_ms > 0 && recv_bytes > 0) {
                double bw_kbps = ((double)recv_bytes * 8.0) /
                                 (sender_pc->latency_ms * 1.0);
                const trellis_peer_info_t *pi =
                    trellis_dht_lookup_peer(client->dht,
                                             &sender_pc->remote_fp);
                if (pi) {
                    // Feed into behavior scorer as measured bandwidth.
                    trellis_peer_info_t *mpi = (trellis_peer_info_t *)pi;
                    double old = mpi->bandwidth;
                    mpi->bandwidth = old * 0.7 + (bw_kbps / 1000.0) * 0.3;
                }
            }
        }
        break;

    case TRELLIS_MSG_PADDING_NEGOTIATE:
        /* Per-circuit padding negotiation: [stream_id(4)][flags(1)]
         * flags: 0x01 = enable padding, 0x02 = constant-rate mode */
        if (msg->payload && msg->payload_len >= 5) {
            uint32_t pad_sid = ((uint32_t)msg->payload[0] << 24) |
                               ((uint32_t)msg->payload[1] << 16) |
                               ((uint32_t)msg->payload[2] << 8) |
                               (uint32_t)msg->payload[3];
            uint8_t pad_flags = msg->payload[4];

            // Store per-circuit padding flags if circuit exists.
            if (client->relay && pad_sid != 0) {
                circuit_entry_t *pad_ce = trellis_circuit_table_lookup(
                    &client->relay->circuit_table, pad_sid);
                if (pad_ce)
                    pad_ce->padding_flags = pad_flags;
            }

            if (client->morph) {
                bool constant_rate = (pad_flags & 0x02) != 0;
                trellis_morph_set_constant_rate(client->morph,
                                                 constant_rate);
            }
        }
        break;

    case TRELLIS_MSG_CAPABILITIES:
        if (sender_pc && msg->payload && msg->payload_len > 0) {
            trellis_capabilities_t remote_caps = {0};
            if (trellis_capabilities_deserialize(msg->payload, msg->payload_len,
                                                  &remote_caps) == TRELLIS_OK) {
                trellis_capabilities_t local_caps = trellis_capabilities_default();
                if (client->exit_capable)
                    trellis_capabilities_add_feature(&local_caps, "exit");
                trellis_capabilities_t negotiated = {0};

                if (trellis_capabilities_negotiate(&local_caps, &remote_caps,
                                                    &negotiated) == TRELLIS_OK) {
                    trellis_capabilities_free(&sender_pc->negotiated_caps);
                    sender_pc->negotiated_caps = negotiated;
                    sender_pc->caps_received = true;

                    /* Enforce relay eligibility based on negotiated features.
                     * A peer with no common transports cannot relay for us. */
                    bool has_common_transport =
                        (negotiated.transports_count > 0);
                    sender_pc->relay_eligible =
                        has_common_transport &&
                        trellis_capabilities_has_feature(&negotiated, "relay");
                    sender_pc->greenhouse_eligible =
                        has_common_transport &&
                        trellis_capabilities_has_feature(&negotiated, "greenhouse");
                    sender_pc->exit_eligible =
                        has_common_transport &&
                        trellis_capabilities_has_feature(&negotiated, "exit");

                    // Apply the negotiated morph timing profile.
                    const char *pref_morph =
                        trellis_capabilities_preferred_morph(&negotiated);
                    if (pref_morph && sender_pc->conn && sender_pc->conn->wire_morph)
                        trellis_morph_set_timing_profile(
                            sender_pc->conn->wire_morph,
                            morph_mode_to_profile(pref_morph));

                    /* If both sides support constant-rate and local morph
                     * has it enabled, enforce it on the per-connection morph */
                    if (trellis_capabilities_has_feature(&negotiated, "constant-rate") &&
                        sender_pc->conn && sender_pc->conn->wire_morph &&
                        trellis_morph_is_constant_rate(sender_pc->conn->wire_morph))
                        trellis_morph_set_require_constant_rate(
                            sender_pc->conn->wire_morph, true);

                    /* Propagate relay eligibility to the DHT peer info entry
                     * so routing can filter relays without iterating peers */
                    trellis_peer_info_t *dht_peer = (trellis_peer_info_t *)
                        trellis_dht_lookup_peer(client->dht, &sender_pc->remote_fp);
                    if (dht_peer) {
                        dht_peer->quarantined = dht_peer->quarantined ||
                                                !sender_pc->relay_eligible;
                        dht_peer->greenhouse_capable =
                            sender_pc->greenhouse_eligible;
                        dht_peer->exit_capable =
                            sender_pc->exit_eligible;
                    }
                } else {
                    /* Negotiation failed: no common capabilities — treat as
                     * non-relay to avoid using this peer for sensitive routing */
                    sender_pc->caps_received = true;
                    sender_pc->relay_eligible = false;
                    sender_pc->greenhouse_eligible = false;
                    sender_pc->exit_eligible = false;
                }

                trellis_capabilities_free(&local_caps);
                trellis_capabilities_free(&remote_caps);
            }
        }
        break;

    case TRELLIS_MSG_BW_RECEIPT:
        if (client->incentive && sender_pc && sender_pc->handshake_complete &&
            msg->payload && msg->payload_len > 0) {
            trellis_incentive_accept_receipt(
                client->incentive,
                msg->payload, msg->payload_len,
                sender_pc->remote_id.ed25519_pk);
        }
        break;

    case TRELLIS_MSG_GH_ERROR:
        if (msg->payload && msg->payload_len >= 4) {
            trellis_event_data_t ev = {0};
            ev.event    = TRELLIS_EV_GREENHOUSE_ERROR;
            ev.peer     = msg->source;
            ev.data     = msg->payload;
            ev.data_len = msg->payload_len;
            trellis_emit(client, &ev);
            fprintf(stderr, "[GH] received GH_ERROR code=%u from %02x%02x...\n",
                    (unsigned)((msg->payload[0] << 24) | (msg->payload[1] << 16) |
                               (msg->payload[2] << 8) | msg->payload[3]),
                    msg->source.bytes[0], msg->source.bytes[1]);
        }
        break;

    case TRELLIS_MSG_GH_DESCRIPTOR:
    case TRELLIS_MSG_GH_INTRODUCE:
    case TRELLIS_MSG_GH_RENDEZVOUS_EST:
    case TRELLIS_MSG_GH_RENDEZVOUS_JOIN:
    case TRELLIS_MSG_GH_RENDEZVOUS_CONN:
        trellis_greenhouse_dispatch_msg(msg);
        break;

    case TRELLIS_MSG_CIRCUIT_CREATE_SESSION:
        if (client->relay && msg->payload && msg->payload_len > 0) {
            trellis_peer_conn_t *sess_sender = find_peer_by_conn(client, conn);
            if (!sess_sender || !sess_sender->handshake_complete)
                break;
            uint8_t resp_buf[64];
            size_t resp_len = sizeof(resp_buf);
            trellis_err_t sess_err = trellis_relay_handle_create_session(
                client->relay, &sess_sender->remote_fp,
                msg->payload, msg->payload_len,
                resp_buf, &resp_len);
            if (sess_err == TRELLIS_OK && resp_len > 0) {
                trellis_message_t resp_msg = {0};
                resp_msg.version = TRELLIS_PROTOCOL_VERSION;
                resp_msg.type = TRELLIS_MSG_CIRCUIT_CREATED_SESSION;
                resp_msg.source = client->identity.fingerprint;
                resp_msg.target = sess_sender->remote_fp;
                resp_msg.payload = resp_buf;
                resp_msg.payload_len = (uint32_t)resp_len;
                if (trellis_message_sign(&resp_msg, &client->identity) == TRELLIS_OK) {
                    trellis_buf_t wire = {0};
                    if (trellis_message_serialize(&resp_msg, &wire) == TRELLIS_OK) {
                        trellis_conn_send(conn, wire.data, wire.len, NULL, NULL);
                        trellis_buf_free(&wire);
                    }
                }
            }
        }
        break;

    case TRELLIS_MSG_CIRCUIT_CREATED_SESSION:
        /* Client-side: the relay confirmed our session circuit.
         * The session is already usable (key derived locally), so this is
         * just an acknowledgment. Log it for diagnostics. */
        fprintf(stderr, "[SESSION] session circuit confirmed by relay %02x%02x...\n",
                msg->source.bytes[0], msg->source.bytes[1]);
        break;

    case TRELLIS_MSG_SESSION_DATA:
        if (client->relay && msg->payload && msg->payload_len > 0) {
            trellis_peer_conn_t *sd_sender = find_peer_by_conn(client, conn);
            if (!sd_sender || !sd_sender->handshake_complete)
                break;
            uint8_t pt_buf[65536];
            size_t pt_len = sizeof(pt_buf);
            trellis_fingerprint_t dest_fp;
            trellis_err_t sd_err = trellis_relay_handle_session_data(
                client->relay, &sd_sender->remote_fp,
                msg->payload, msg->payload_len,
                pt_buf, &pt_len, &dest_fp);
            if (sd_err == TRELLIS_OK) {
                // Forward decrypted payload to the destination peer.
                trellis_peer_conn_t *dest_pc = trellis_client_find_peer(client, &dest_fp);
                if (dest_pc && dest_pc->conn)
                    trellis_conn_send(dest_pc->conn, pt_buf, pt_len, NULL, NULL);
            }
        }
        break;

    case TRELLIS_MSG_CANOPY_ANNOUNCE:
        if (client->canopy && msg->payload && msg->payload_len > 0) {
            trellis_canopy_on_announce(client->canopy,
                                       msg->payload, msg->payload_len,
                                       &msg->source);
        }
        break;

    case TRELLIS_MSG_CANOPY_WITHDRAW:
        if (client->canopy && msg->payload && msg->payload_len > 0) {
            trellis_canopy_on_withdraw(client->canopy,
                                        msg->payload, msg->payload_len,
                                        &msg->source);
        }
        break;

    case TRELLIS_MSG_CANOPY_CIRCUIT_CREATE:
        if (client->canopy && msg->payload && msg->payload_len > 0)
            trellis_canopy_on_circuit_create(client->canopy,
                                             msg->payload, msg->payload_len,
                                             &msg->source);
        break;

    case TRELLIS_MSG_CANOPY_CIRCUIT_DATA:
        if (client->canopy && msg->payload && msg->payload_len > 0)
            trellis_canopy_on_circuit_data(client->canopy,
                                           msg->payload, msg->payload_len,
                                           &msg->source);
        break;

    case TRELLIS_MSG_CANOPY_CIRCUIT_DESTROY:
        if (client->canopy && msg->payload && msg->payload_len > 0)
            trellis_canopy_on_circuit_destroy(client->canopy,
                                              msg->payload, msg->payload_len,
                                              &msg->source);
        break;

    case TRELLIS_MSG_GROVE_JOIN_REQUEST:
    case TRELLIS_MSG_GROVE_JOIN_RESPONSE:
        /* Grove join messages are delivered via DHT store/find;
         * the message type is used when relayed through gossip. */
        break;

    default:
        if (client->extensions)
            trellis_extensions_dispatch(client->extensions, msg);
        break;
    }

    (void)data;
    (void)len;
}

void trellis_client_on_data(trellis_conn_t *conn, const uint8_t *data,
                             size_t len, trellis_err_t err, void *ctx)
{
    trellis_client_t *client = (trellis_client_t *)ctx;
    if (err != TRELLIS_OK || !data || len == 0 || !client->running) {
        if (err != TRELLIS_OK && conn && client->running) {
            trellis_peer_conn_t *pc = find_peer_by_conn(client, conn);
            if (pc) {
                trellis_event_data_t ev = {0};
                ev.event = TRELLIS_EV_PEER_REMOVED;
                ev.peer = pc->remote_fp;
                trellis_emit(client, &ev);
                remove_peer_conn_by_ptr(client, conn);
            }
        }
        return;
    }

    /*
     * Three-tier deobfuscation (matches pending_on_data):
     *   1. probe_guard_validate → deobfuscate → deserialize
     *   2. deobfuscate → deserialize
     *   3. (fall through to raw path below)
     *
     * Retry with epoch-1 and epoch+1 to tolerate ±1 hour clock skew between
     * sender and receiver (both boundary directions).
     */
    {
        uint64_t cur_epoch = trellis_now_ms() / 3600000ULL;
        uint8_t obfs_key[TRELLIS_OBFS_KEY_LEN];
        trellis_derive_obfs_key_epoch(client->identity.x25519_pk,
                                      TRELLIS_X25519_PK_LEN, cur_epoch, obfs_key);
        uint8_t obfs_key_prev[TRELLIS_OBFS_KEY_LEN];
        trellis_derive_obfs_key_epoch(client->identity.x25519_pk,
                                      TRELLIS_X25519_PK_LEN,
                                      cur_epoch > 0 ? cur_epoch - 1 : 0,
                                      obfs_key_prev);
        uint8_t obfs_key_next[TRELLIS_OBFS_KEY_LEN];
        trellis_derive_obfs_key_epoch(client->identity.x25519_pk,
                                      TRELLIS_X25519_PK_LEN,
                                      cur_epoch + 1, obfs_key_next);

        const uint8_t *epoch_keys[3] = { obfs_key, obfs_key_prev, obfs_key_next };
        trellis_buf_t deobfs = {0};
        bool got_inner = false;

        /* Tier 1: probe_guard + deobfuscate (cur/prev/next epoch keys).
         * Also try a temporary guard keyed to epoch+1 in case the sender
         * rolled over before we did. */
        if (!got_inner && client->probe_guard) {
            const uint8_t *pg_inner = NULL;
            size_t pg_inner_len = 0;
            const char *remote_ip = trellis_conn_remote_addr(conn);
            if (trellis_probe_guard_validate_addr(client->probe_guard, data, len,
                                                  remote_ip, &pg_inner, &pg_inner_len)
                    == TRELLIS_OK) {
                for (int ki = 0; ki < 3 && !got_inner; ki++) {
                    trellis_buf_free(&deobfs);
                    if (trellis_handshake_deobfuscate(pg_inner, pg_inner_len,
                                                      epoch_keys[ki], &deobfs)
                            == TRELLIS_OK && deobfs.data)
                        got_inner = true;
                }
            }
            // Try epoch+1 guard (sender's clock is one hour ahead)
            if (!got_inner) {
                trellis_probe_guard_t *pg_next =
                    trellis_probe_guard_new(obfs_key_next, sizeof(obfs_key_next));
                if (pg_next) {
                    pg_inner = NULL; pg_inner_len = 0;
                    if (trellis_probe_guard_validate_addr(pg_next, data, len,
                                                          remote_ip, &pg_inner,
                                                          &pg_inner_len) == TRELLIS_OK) {
                        for (int ki = 0; ki < 3 && !got_inner; ki++) {
                            trellis_buf_free(&deobfs);
                            if (trellis_handshake_deobfuscate(pg_inner, pg_inner_len,
                                                              epoch_keys[ki], &deobfs)
                                    == TRELLIS_OK && deobfs.data)
                                got_inner = true;
                        }
                    }
                    trellis_probe_guard_free(pg_next);
                }
            }
        }

        // Tier 2: deobfuscate without probe_guard (cur/prev/next epoch)
        if (!got_inner) {
            for (int ki = 0; ki < 3 && !got_inner; ki++) {
                trellis_buf_free(&deobfs);
                if (trellis_handshake_deobfuscate(data, len, epoch_keys[ki], &deobfs)
                        == TRELLIS_OK && deobfs.data)
                    got_inner = true;
            }
        }

        sodium_memzero(obfs_key, sizeof(obfs_key));
        sodium_memzero(obfs_key_prev, sizeof(obfs_key_prev));
        sodium_memzero(obfs_key_next, sizeof(obfs_key_next));

        if (got_inner) {
            trellis_message_t raw_msg = {0};
            trellis_err_t deser_err = trellis_message_deserialize(
                deobfs.data, deobfs.len, &raw_msg);
            if (deser_err == TRELLIS_OK) {
                if (raw_msg.type == TRELLIS_MSG_HANDSHAKE_1 ||
                    raw_msg.type == TRELLIS_MSG_HANDSHAKE_2 ||
                    raw_msg.type == TRELLIS_MSG_HANDSHAKE_3) {
                    handle_handshake(client, conn, &raw_msg);
                    trellis_message_free(&raw_msg);
                    trellis_buf_free(&deobfs);
                    return;
                }
                dispatch_message(client, conn, &raw_msg);
                trellis_message_free(&raw_msg);
                trellis_buf_free(&deobfs);
                return;
            }
            trellis_buf_free(&deobfs);
        }
    }

    /*
     * Fall through: try raw deserialization for non-obfuscated messages
     * (e.g. onion-delivered payloads, legacy peers).
     */
    trellis_message_t raw_msg = {0};
    trellis_err_t raw_err = trellis_message_deserialize(data, len, &raw_msg);
    if (raw_err == TRELLIS_OK) {
        if (raw_msg.type == TRELLIS_MSG_HANDSHAKE_1 ||
            raw_msg.type == TRELLIS_MSG_HANDSHAKE_2 ||
            raw_msg.type == TRELLIS_MSG_HANDSHAKE_3) {
            handle_handshake(client, conn, &raw_msg);
            trellis_message_free(&raw_msg);
            return;
        }
        if (raw_msg.type == TRELLIS_MSG_ONION) {
            handle_onion(client, conn, &raw_msg);
            trellis_message_free(&raw_msg);
            return;
        }
        dispatch_message(client, conn, &raw_msg);
        trellis_message_free(&raw_msg);
        return;
    }

    // Post-handshake path: demorph first, using per-peer morph if available.
    trellis_peer_conn_t *sender_pc = find_peer_by_conn(client, conn);
    trellis_morph_t *morph = (sender_pc && sender_pc->morph)
                             ? sender_pc->morph : client->morph;
    trellis_buf_t decoded = {0};
    err = trellis_morph_decode(morph, data, len, &decoded);
    if (err != TRELLIS_OK)
        return;

    // Ratchet decrypt if the peer has a completed handshake.
    trellis_buf_t plaintext = {0};
    const uint8_t *msg_data = decoded.data;
    size_t msg_len = decoded.len;

    if (sender_pc && sender_pc->handshake_complete && sender_pc->ratchet) {
        err = trellis_ratchet_decrypt(sender_pc->ratchet,
                                      decoded.data, decoded.len, &plaintext);
        trellis_buf_free(&decoded);
        if (err != TRELLIS_OK)
            return;
        msg_data = plaintext.data;
        msg_len = plaintext.len;
    }

    trellis_message_t msg = {0};
    err = trellis_message_deserialize(msg_data, msg_len, &msg);

    if (plaintext.data)
        trellis_buf_free(&plaintext);
    else
        trellis_buf_free(&decoded);

    if (err != TRELLIS_OK)
        return;

    dispatch_message(client, conn, &msg);
    trellis_message_free(&msg);
}

void trellis_client_setup_peer(trellis_client_t *client, trellis_conn_t *conn,
                               trellis_handshake_role_t role)
{
    trellis_peer_info_t peer_info = {0};
    const char *addr = trellis_conn_remote_addr(conn);
    if (addr) {
        size_t alen = strlen(addr);
        if (alen >= sizeof(peer_info.addr))
            alen = sizeof(peer_info.addr) - 1;
        memcpy(peer_info.addr, addr, alen);
        crypto_hash_sha256(peer_info.fingerprint.bytes,
                           (const unsigned char *)addr, alen);
    } else {
        randombytes_buf(peer_info.fingerprint.bytes, TRELLIS_FINGERPRINT_LEN);
    }
    peer_info.last_seen = trellis_now_ms();
    peer_info.reliability = 1.0;

    trellis_peer_conn_t pc = {0};
    pc.conn = conn;
    pc.remote_fp = peer_info.fingerprint;
    pc.handshake = trellis_handshake_new(&client->identity, role);
    pc.handshake_complete = false;
    pc.last_pong = trellis_now_ms();
    if (addr) {
        size_t alen2 = strlen(addr);
        if (alen2 >= sizeof(pc.addr)) alen2 = sizeof(pc.addr) - 1;
        memcpy(pc.addr, addr, alen2);
    }

    if (!pc.handshake)
        return;

    trellis_err_t err = trellis_client_add_peer_conn(client, &pc);
    if (err != TRELLIS_OK) {
        trellis_handshake_free(pc.handshake);
        return;
    }

    trellis_conn_recv_start(conn, trellis_client_on_data, client);
    (void)trellis_dht_add_peer(client->dht, &peer_info);

    trellis_peer_conn_t *timer_pc =
        trellis_client_find_peer(client, &peer_info.fingerprint);
    if (timer_pc)
        start_handshake_timer(client, timer_pc);

    trellis_event_data_t ev = {0};
    ev.event = TRELLIS_EV_PEER_ADDED;
    ev.peer = peer_info.fingerprint;
    trellis_emit(client, &ev);

    /* Initiator kicks off the 3-message handshake.
       We don't yet know the responder's identity so target_obfs_pk is
       NULL -- the responder's receive path handles unobfuscated HS_1
       via its fallback deserialize. */
    if (role == TRELLIS_ROLE_INITIATOR) {
        trellis_peer_conn_t *added_pc =
            trellis_client_find_peer(client, &peer_info.fingerprint);
        if (added_pc && added_pc->handshake) {
            trellis_buf_t msg1 = {0};
            err = trellis_handshake_create_msg1(added_pc->handshake, &msg1);
            if (err == TRELLIS_OK) {
                send_handshake_msg(client, conn, TRELLIS_MSG_HANDSHAKE_1,
                                   msg1.data, msg1.len, NULL);
                trellis_buf_free(&msg1);
            }
        }
    }
}

// --- Pending connection: validate first frame before full peer setup ---
typedef struct pending_conn {
    trellis_client_t *client;
    trellis_conn_t   *conn;
    uv_timer_t       *timeout;
    bool              pow_challenge_sent;  /* server: waiting for PoW solution */
    trellis_conn_pow_challenge_t pow_challenge; /* the challenge we sent */
} pending_conn_t;

static void pending_timeout_close_cb(uv_handle_t *handle)
{
    free(handle);
}

static void pending_cleanup(pending_conn_t *pc)
{
    if (pc->timeout) {
        uv_timer_stop(pc->timeout);
        uv_close((uv_handle_t *)pc->timeout, pending_timeout_close_cb);
        pc->timeout = NULL;
    }
    free(pc);
}

static void pending_on_data(trellis_conn_t *conn, const uint8_t *data,
                            size_t len, trellis_err_t err, void *ctx)
{
    pending_conn_t *pc = (pending_conn_t *)ctx;
    trellis_client_t *client = pc->client;

    if (err != TRELLIS_OK || !data || len == 0 || !client->running) {
        trellis_conn_close(conn, NULL, NULL);
        pending_cleanup(pc);
        return;
    }

    /*
     * Per-connection PoW gate: if we sent a challenge, the first response
     * must be a valid solution.  Verification is O(1) (hash comparison),
     * resisting Tor-style top-half flooding.
     */
    if (pc->pow_challenge_sent && client->conn_pow) {
        if (len < TRELLIS_CONN_POW_SOLUTION_LEN ||
            data[0] != TRELLIS_CONN_POW_SOLUTION_TAG) {
            trellis_conn_close(conn, NULL, NULL);
            pending_cleanup(pc);
            return;
        }
        trellis_conn_pow_solution_t sol;
        if (trellis_conn_pow_solution_deserialize(data, len, &sol) != TRELLIS_OK ||
            trellis_conn_pow_verify(client->conn_pow, &pc->pow_challenge, &sol)
                != TRELLIS_OK) {
            trellis_conn_close(conn, NULL, NULL);
            pending_cleanup(pc);
            return;
        }
        pc->pow_challenge_sent = false;
        /* TCP may coalesce the PoW solution and HS_1 into one segment.
         * If there is trailing data, process it as the handshake message. */
        size_t remaining = len - TRELLIS_CONN_POW_SOLUTION_LEN;
        if (remaining > 0) {
            pending_on_data(conn, data + TRELLIS_CONN_POW_SOLUTION_LEN,
                            remaining, TRELLIS_OK, ctx);
        }
        return;
    }

    /*
     * Active probe resistance -- three-tier validation:
     *   1. probe_guard_validate → deobfuscate → deserialize  (full protection)
     *   2. deobfuscate → deserialize                          (obfuscation only)
     *   3. raw deserialize                                    (fallback)
     * Any path that yields a valid HANDSHAKE_1 is accepted.
     * Anything else is silently dropped (no error response to probes).
     *
     * Each deobfuscation attempt also retries with epoch-1 to tolerate
     * messages signed just before an hourly key rotation boundary.
     */
    uint64_t cur_epoch = trellis_now_ms() / 3600000ULL;
    uint8_t obfs_key[TRELLIS_OBFS_KEY_LEN];
    trellis_derive_obfs_key_epoch(client->identity.x25519_pk,
                                  TRELLIS_X25519_PK_LEN, cur_epoch, obfs_key);
    uint8_t obfs_key_prev[TRELLIS_OBFS_KEY_LEN];
    trellis_derive_obfs_key_epoch(client->identity.x25519_pk,
                                  TRELLIS_X25519_PK_LEN,
                                  cur_epoch > 0 ? cur_epoch - 1 : 0,
                                  obfs_key_prev);
    uint8_t obfs_key_next[TRELLIS_OBFS_KEY_LEN];
    trellis_derive_obfs_key_epoch(client->identity.x25519_pk,
                                  TRELLIS_X25519_PK_LEN,
                                  cur_epoch + 1, obfs_key_next);

    const uint8_t *msg_data = NULL;
    size_t msg_len = 0;
    trellis_buf_t deobfs = {0};

    /* Helper: try deobfuscating with three candidate keys (cur, prev, next).
     * This tolerates ±1 epoch skew between sender and receiver. */
#define TRY_DEOBFS(src, src_len, result_data, result_len) do {              \
    const uint8_t *_keys[] = { obfs_key, obfs_key_prev, obfs_key_next };   \
    for (int _ki = 0; _ki < 3 && !(result_data); _ki++) {                   \
        trellis_buf_free(&deobfs);                                           \
        if (trellis_handshake_deobfuscate((src), (src_len), _keys[_ki],     \
                                          &deobfs) == TRELLIS_OK             \
                && deobfs.data) {                                            \
            (result_data) = deobfs.data;                                     \
            (result_len)  = deobfs.len;                                      \
        }                                                                    \
    }                                                                        \
} while (0)

    /* Tier 1: probe_guard + deobfuscate (current/prev/next epoch keys).
     * Also try a temporary guard keyed to epoch+1 in case the sender is
     * one hour ahead (their clock rolled over, ours hasn't yet). */
    if (client->probe_guard) {
        const uint8_t *inner = NULL;
        size_t inner_len = 0;
        const char *remote_ip = trellis_conn_remote_addr(conn);
        if (trellis_probe_guard_validate_addr(client->probe_guard, data, len,
                                              remote_ip, &inner, &inner_len) == TRELLIS_OK) {
            TRY_DEOBFS(inner, inner_len, msg_data, msg_len);
        }

        /* If the current guard rejected it, try a guard keyed to epoch+1
         * (sender may have rotated before we did). */
        if (!msg_data) {
            trellis_probe_guard_t *pg_next =
                trellis_probe_guard_new(obfs_key_next, sizeof(obfs_key_next));
            if (pg_next) {
                inner = NULL; inner_len = 0;
                if (trellis_probe_guard_validate_addr(pg_next, data, len,
                                                      remote_ip, &inner, &inner_len)
                        == TRELLIS_OK) {
                    TRY_DEOBFS(inner, inner_len, msg_data, msg_len);
                }
                trellis_probe_guard_free(pg_next);
            }
        }
    }

    // Tier 2: deobfuscate without probe_guard (current/prev/next epoch)
    if (!msg_data) {
        TRY_DEOBFS(data, len, msg_data, msg_len);
    }
#undef TRY_DEOBFS

    /* Tier 3: raw message (no obfuscation, e.g. HS_1 from peer without
       our public key) */
    if (!msg_data) {
        msg_data = data;
        msg_len = len;
    }

    sodium_memzero(obfs_key, sizeof(obfs_key));
    sodium_memzero(obfs_key_prev, sizeof(obfs_key_prev));
    sodium_memzero(obfs_key_next, sizeof(obfs_key_next));

    trellis_message_t raw = {0};
    trellis_err_t deser_err = trellis_message_deserialize(msg_data, msg_len, &raw);
    if (deser_err != TRELLIS_OK || raw.type != TRELLIS_MSG_HANDSHAKE_1) {
        trellis_message_free(&raw);
        if (deobfs.data) trellis_buf_free(&deobfs);
        trellis_conn_close(conn, NULL, NULL);
        pending_cleanup(pc);
        return;
    }

    trellis_conn_recv_stop(conn);
    pending_cleanup(pc);

    trellis_client_setup_peer(client, conn, TRELLIS_ROLE_RESPONDER);
    handle_handshake(client, conn, &raw);
    trellis_message_free(&raw);
    if (deobfs.data) trellis_buf_free(&deobfs);
}

static void pending_on_timeout(uv_timer_t *handle)
{
    pending_conn_t *pc = handle->data;
    trellis_conn_recv_stop(pc->conn);
    trellis_conn_close(pc->conn, NULL, NULL);
    pending_cleanup(pc);
}

void trellis_client_on_accept(trellis_conn_t *conn, void *ctx)
{
    trellis_client_t *client = (trellis_client_t *)ctx;
    if (!client || !client->running || !conn)
        return;

    pending_conn_t *pc = calloc(1, sizeof(*pc));
    if (!pc) {
        trellis_conn_close(conn, NULL, NULL);
        return;
    }
    pc->client = client;
    pc->conn = conn;

    pc->timeout = malloc(sizeof(uv_timer_t));
    if (pc->timeout) {
        uv_timer_init(&client->loop, pc->timeout);
        pc->timeout->data = pc;
        uv_timer_start(pc->timeout, pending_on_timeout,
                       TRELLIS_HANDSHAKE_TIMEOUT_MS, 0);
    }

    /* Send PoW challenge before accepting any data.
     * This gates connection setup with fixed-difficulty computational work,
     * avoiding Tor's dynamic control loop vulnerabilities.
     * Fail closed: if generation or serialization fails, reject the
     * connection rather than silently degrading to no-PoW. */
    if (client->conn_pow) {
        bool pow_ok = false;
        if (trellis_conn_pow_generate(client->conn_pow, &pc->pow_challenge)
                == TRELLIS_OK) {
            uint8_t wire[TRELLIS_CONN_POW_CHALLENGE_LEN];
            if (trellis_conn_pow_challenge_serialize(&pc->pow_challenge, wire)
                    == TRELLIS_OK) {
                trellis_conn_send(conn, wire, sizeof(wire), NULL, NULL);
                pc->pow_challenge_sent = true;
                pow_ok = true;
            }
        }
        if (!pow_ok) {
            trellis_conn_close(conn, NULL, NULL);
            pending_cleanup(pc);
            return;
        }
    }

    trellis_conn_recv_start(conn, pending_on_data, pc);
}

/*
 * Client-side PoW: when connecting to a server that requires PoW, the first
 * message received will be a CHALLENGE.  Solve it and send the SOLUTION,
 * then proceed with normal handshake.
 *
 * We use a short timeout: if no data arrives within TRELLIS_POW_WAIT_MS,
 * the server doesn't require PoW and we start the handshake directly.
 */
#define TRELLIS_POW_WAIT_MS 2000

typedef struct pow_connect_ctx {
    trellis_client_t *client;
    trellis_conn_t   *conn;
    uv_timer_t       *timeout;
} pow_connect_ctx_t;

static void pow_connect_timeout_close(uv_handle_t *h) { free(h); }

static void pow_connect_cleanup(pow_connect_ctx_t *pctx)
{
    if (pctx->timeout) {
        uv_timer_stop(pctx->timeout);
        uv_close((uv_handle_t *)pctx->timeout, pow_connect_timeout_close);
        pctx->timeout = NULL;
    }
    free(pctx);
}

static void pow_connect_proceed(pow_connect_ctx_t *pctx)
{
    trellis_client_t *client = pctx->client;
    trellis_conn_t *conn = pctx->conn;
    trellis_conn_recv_stop(conn);
    pow_connect_cleanup(pctx);
    trellis_client_setup_peer(client, conn, TRELLIS_ROLE_INITIATOR);
}

static void pow_connect_on_timeout(uv_timer_t *handle)
{
    pow_connect_ctx_t *pctx = handle->data;
    pow_connect_proceed(pctx);
}

static void pow_connect_on_data(trellis_conn_t *conn, const uint8_t *data,
                                size_t len, trellis_err_t err, void *ctx)
{
    pow_connect_ctx_t *pctx = (pow_connect_ctx_t *)ctx;
    trellis_client_t *client = pctx->client;

    if (err != TRELLIS_OK || !data || len == 0 || !client->running) {
        trellis_conn_close(conn, NULL, NULL);
        pow_connect_cleanup(pctx);
        return;
    }

    if (len >= TRELLIS_CONN_POW_CHALLENGE_LEN &&
        data[0] == TRELLIS_CONN_POW_CHALLENGE_TAG) {
        trellis_conn_pow_challenge_t ch;
        if (trellis_conn_pow_challenge_deserialize(data, len, &ch)
                != TRELLIS_OK) {
            trellis_conn_close(conn, NULL, NULL);
            pow_connect_cleanup(pctx);
            return;
        }

        trellis_conn_pow_solution_t sol;
        if (trellis_conn_pow_solve(&ch, NULL, &sol) != TRELLIS_OK) {
            trellis_conn_close(conn, NULL, NULL);
            pow_connect_cleanup(pctx);
            return;
        }

        uint8_t wire[TRELLIS_CONN_POW_SOLUTION_LEN];
        if (trellis_conn_pow_solution_serialize(&sol, wire) != TRELLIS_OK) {
            trellis_conn_close(conn, NULL, NULL);
            pow_connect_cleanup(pctx);
            return;
        }
        trellis_conn_send(conn, wire, sizeof(wire), NULL, NULL);

        /* PoW solved — proceed to handshake. The server verifies the
         * solution in pending_on_data, then waits for HS_1. */
        pow_connect_proceed(pctx);
        return;
    }

    // Not a PoW challenge — server doesn't require PoW. Proceed.
    pow_connect_proceed(pctx);
}

static void on_seed_connected(trellis_conn_t *conn, trellis_err_t err, void *ctx)
{
    trellis_client_t *client = (trellis_client_t *)ctx;
    if (err != TRELLIS_OK || !conn || !client->running)
        return;

    /*
     * If the client is configured for PoW (non-zero, non-UINT32_MAX),
     * wait briefly for a challenge from the server before handshaking.
     * The server sends the challenge immediately on accept, so under
     * normal conditions it arrives within milliseconds.  The timeout
     * prevents blocking forever if the server doesn't require PoW.
     *
     * When PoW is disabled (iterations == 0 or UINT32_MAX), skip the
     * wait and start the handshake immediately — zero latency penalty.
     */
    if (client->config.conn_pow_iterations != UINT32_MAX) {
        pow_connect_ctx_t *pctx = calloc(1, sizeof(*pctx));
        if (!pctx) {
            trellis_client_setup_peer(client, conn, TRELLIS_ROLE_INITIATOR);
            return;
        }
        pctx->client = client;
        pctx->conn = conn;
        pctx->timeout = malloc(sizeof(uv_timer_t));
        if (pctx->timeout) {
            uv_timer_init(&client->loop, pctx->timeout);
            pctx->timeout->data = pctx;
            uv_timer_start(pctx->timeout, pow_connect_on_timeout,
                           TRELLIS_POW_WAIT_MS, 0);
        }
        trellis_conn_recv_start(conn, pow_connect_on_data, pctx);
        return;
    }

    trellis_client_setup_peer(client, conn, TRELLIS_ROLE_INITIATOR);
}

static const char *strip_proto_prefix(const char *addr)
{
    const char *p = strstr(addr, "://");
    return p ? p + 3 : addr;
}

// --- Jitter-delayed send ---
typedef struct jitter_send_ctx {
    trellis_conn_t *conn;
    uint8_t        *data;
    size_t          len;
    uv_timer_t      timer;
} jitter_send_ctx_t;

static void on_jitter_close(uv_handle_t *h)
{
    jitter_send_ctx_t *jctx = h->data;
    free(jctx->data);
    free(jctx);
}

static void on_jitter_timer(uv_timer_t *handle)
{
    jitter_send_ctx_t *jctx = handle->data;
    if (jctx->conn)
        trellis_conn_send(jctx->conn, jctx->data, jctx->len, NULL, NULL);
    uv_close((uv_handle_t *)&jctx->timer, on_jitter_close);
}

static void send_with_jitter(trellis_client_t *client, trellis_conn_t *conn,
                             uint8_t *data, size_t len)
{
    uint32_t delay = trellis_morph_jitter_delay(client->morph);
    if (delay == 0 || !client->loop_initialized) {
        trellis_conn_send(conn, data, len, NULL, NULL);
        free(data);
        return;
    }

    jitter_send_ctx_t *jctx = malloc(sizeof(*jctx));
    if (!jctx) {
        trellis_conn_send(conn, data, len, NULL, NULL);
        free(data);
        return;
    }
    jctx->conn = conn;
    jctx->data = data;
    jctx->len = len;
    uv_timer_init(&client->loop, &jctx->timer);
    jctx->timer.data = jctx;
    uv_timer_start(&jctx->timer, on_jitter_timer, delay, 0);
}

static void on_eclipse_detected(trellis_dht_t *dht, void *ctx)
{
    (void)dht;
    trellis_client_t *client = (trellis_client_t *)ctx;
    if (!client)
        return;

    fprintf(stderr, "[client] WARNING: DHT eclipse attack detected — "
            "routing table frozen\n");

    trellis_event_data_t ev = {0};
    ev.event = TRELLIS_EV_ECLIPSE_DETECTED;
    trellis_emit(client, &ev);
}

static void gossip_send_cb(void *client_ptr,
                           const trellis_fingerprint_t *target,
                           const uint8_t *data, size_t len)
{
    trellis_client_t *client = (trellis_client_t *)client_ptr;
    if (!client || !client->running)
        return;

    trellis_peer_conn_t *pc = trellis_client_find_peer(client, target);
    if (!pc || !pc->conn || !pc->handshake_complete)
        return;

    send_handshake_msg(client, pc->conn, TRELLIS_MSG_GOSSIP, data, len,
                       pc->remote_id.x25519_pk);
}

// --- Incentive send callback ---
static void incentive_send_cb(void *client_ptr,
                               const trellis_fingerprint_t *peer_fp,
                               const uint8_t *data, size_t len)
{
    trellis_client_t *client = (trellis_client_t *)client_ptr;
    if (!client || !client->running)
        return;

    trellis_peer_conn_t *pc = trellis_client_find_peer(client, peer_fp);
    if (!pc || !pc->conn || !pc->handshake_complete)
        return;

    send_handshake_msg(client, pc->conn, TRELLIS_MSG_BW_RECEIPT, data, len,
                       pc->remote_id.x25519_pk);
}

/*
 * Thread-safe: queue serialized wire bytes to be broadcast via the
 * event-loop thread's on_send_async handler.
 */
static void queue_broadcast(trellis_client_t *client,
                             const trellis_buf_t *wire)
{
    trellis_send_item_t *item = malloc(sizeof(*item));
    if (!item)
        return;
    item->data = malloc(wire->len);
    if (!item->data) {
        free(item);
        return;
    }
    memcpy(item->data, wire->data, wire->len);
    item->len = wire->len;

    uv_mutex_lock(&client->send_lock);
    item->next = client->send_head;
    client->send_head = item;
    uv_mutex_unlock(&client->send_lock);
    uv_async_send(&client->send_async);
}

/*
 * Send a serialized (but unencrypted/unmorphed) message to every connected
 * peer, applying per-peer ratchet encryption and morph encoding.
 * MUST be called from the event-loop thread.
 */
static void broadcast_to_peers(trellis_client_t *client,
                                const trellis_buf_t *wire)
{
    for (size_t i = 0; i < client->peer_conn_count; i++) {
        trellis_peer_conn_t *pc = &client->peers[i];
        if (!pc->conn || !pc->handshake_complete || !pc->ratchet)
            continue;

        const uint8_t *send_data = wire->data;
        size_t send_len = wire->len;
        trellis_buf_t encrypted = {0};

        trellis_err_t e = trellis_ratchet_encrypt(pc->ratchet,
                                                   wire->data, wire->len,
                                                   &encrypted);
        if (e != TRELLIS_OK)
            continue;
        send_data = encrypted.data;
        send_len  = encrypted.len;

        trellis_morph_t *morph = (pc->morph) ? pc->morph : client->morph;
        trellis_buf_t morphed = {0};
        trellis_err_t me = trellis_morph_encode(morph, send_data, send_len,
                                                 &morphed);
        trellis_buf_free(&encrypted);
        if (me != TRELLIS_OK)
            continue;

        send_with_jitter(client, pc->conn, morphed.data, morphed.len);
    }
}

// --- Keepalive timer (sends PING to all peers every 30s) ---
static void on_guard_tick_timer(uv_timer_t *handle)
{
    trellis_client_t *client = handle->data;
    if (!client || !client->running || !client->guard)
        return;
    trellis_guard_tick(client->guard, trellis_now_ms());
}

static void on_keepalive_timer(uv_timer_t *handle)
{
    trellis_client_t *client = handle->data;
    if (!client || !client->running)
        return;

    uint64_t now = trellis_now_ms();
    uint64_t dead_threshold = 90000;

    for (size_t i = 0; i < client->peer_conn_count; i++) {
        trellis_peer_conn_t *pc = &client->peers[i];
        if (!pc->handshake_complete || !pc->conn)
            continue;

        // Check if peer is dead (no pong in 90s)
        if (pc->last_pong > 0 && (now - pc->last_pong) > dead_threshold) {
            trellis_event_data_t ev = {0};
            ev.event = TRELLIS_EV_PEER_REMOVED;
            ev.peer = pc->remote_fp;
            trellis_emit(client, &ev);

            // Save address for reconnection.
            if (pc->addr[0] && client->reconnect_count < client->reconnect_cap) {
                client->reconnect_addrs[client->reconnect_count] = strdup(pc->addr);
                if (client->reconnect_addrs[client->reconnect_count])
                    client->reconnect_count++;
            }

            remove_peer_conn_by_ptr(client, pc->conn);
            i--;
            continue;
        }

        // PING with sender timestamp for clock skew detection.
        {
            uint8_t ping_buf[12];
            uint32_t ping_id = randombytes_random();
            trellis_write_u32_be(ping_buf, ping_id);
            uint64_t sender_time = trellis_now_ms();
            memcpy(ping_buf + 4, &sender_time, 8);
            send_dht_rpc(client, pc->conn, TRELLIS_MSG_PING,
                         ping_buf, sizeof(ping_buf));
        }
    }

    // Sync clock skew offset into probe guard for timestamp tolerance.
    if (client->probe_guard && client->dht) {
        int64_t offset = trellis_clock_sync_offset_ms(&client->dht->clock_sync);
        trellis_probe_guard_set_clock_offset(client->probe_guard, offset);
    }

    // Periodic path-bias check: warn/disable guards with poor ratios.
    if (client->router && client->router->guard)
        trellis_guard_path_bias_check(client->router->guard);

    // Attempt reconnection to lost peers.
    for (size_t i = 0; i < client->reconnect_count; ) {
        const char *addr = client->reconnect_addrs[i];
        bool already_connected = false;

        for (size_t j = 0; j < client->peer_conn_count; j++) {
            if (client->peers[j].addr[0] &&
                strcmp(client->peers[j].addr, addr) == 0) {
                already_connected = true;
                break;
            }
        }

        if (already_connected) {
            free(client->reconnect_addrs[i]);
            client->reconnect_addrs[i] = client->reconnect_addrs[--client->reconnect_count];
        } else {
            trellis_transport_connect(client->transport, addr,
                                      on_seed_connected, client);
            free(client->reconnect_addrs[i]);
            client->reconnect_addrs[i] = client->reconnect_addrs[--client->reconnect_count];
        }
    }
}

static void on_pex_timer(uv_timer_t *handle)
{
    trellis_client_t *client = handle->data;
    if (!client || !client->running)
        return;

    for (size_t i = 0; i < client->peer_conn_count; i++) {
        trellis_peer_conn_t *pc = &client->peers[i];
        if (pc->handshake_complete)
            send_peer_exchange(client, pc);
    }
}

static void on_probe_guard_epoch_timer(uv_timer_t *handle)
{
    trellis_client_t *client = handle->data;
    if (!client || !client->running || !client->probe_guard)
        return;

    uint64_t cur_epoch = trellis_now_ms() / 3600000ULL;
    if (cur_epoch == client->probe_guard_epoch)
        return;

    uint8_t new_obfs_key[TRELLIS_OBFS_KEY_LEN];
    trellis_derive_obfs_key_epoch(client->identity.x25519_pk,
                                  TRELLIS_X25519_PK_LEN,
                                  cur_epoch, new_obfs_key);
    trellis_probe_guard_rekey(client->probe_guard,
                              new_obfs_key, sizeof(new_obfs_key));
    sodium_memzero(new_obfs_key, sizeof(new_obfs_key));
    client->probe_guard_epoch = cur_epoch;
}

// --- Cover traffic timer ---
void trellis_client_cover_tick(uv_timer_t *handle)
{
    trellis_client_t *client = handle->data;
    if (!client || !client->running || client->peer_conn_count == 0)
        return;

    // Pick a random connected peer with a completed ratchet.
    size_t start = randombytes_uniform((uint32_t)client->peer_conn_count);
    trellis_peer_conn_t *target = NULL;
    for (size_t i = 0; i < client->peer_conn_count; i++) {
        size_t idx = (start + i) % client->peer_conn_count;
        trellis_peer_conn_t *pc = &client->peers[idx];
        if (pc->conn && pc->handshake_complete && pc->ratchet) {
            target = pc;
            break;
        }
    }

    if (!target)
        return;

    trellis_morph_t *morph = target->morph ? target->morph : client->morph;
    trellis_buf_t send_buf = {0};

    {
        /*
         * Generate cover traffic via the morph engine unconditionally so
         * the magic prefix, mutation, and padding are always applied.
         *
         * In constant-rate mode trellis_morph_encode pads to
         * constant_rate_frame_size, giving the fixed-size guarantee.
         *
         * trellis_morph_generate_cover_ratchet produces a real
         * TRELLIS_MSG_COVER ciphertext; the receiver decrypts and discards.
         */
        trellis_buf_t cover = {0};
        trellis_err_t err = trellis_morph_generate_cover_ratchet(
            morph, target->ratchet, &cover);

        if (err != TRELLIS_OK) {
            if (trellis_morph_generate_cover(morph, &cover) != TRELLIS_OK)
                return;
        }

        err = trellis_morph_encode(morph, cover.data, cover.len, &send_buf);
        trellis_buf_free(&cover);
        if (err != TRELLIS_OK)
            return;
    }

    if (target->conn) {
        /* Constant-rate frames must be sent immediately — jitter defeats the
         * timing guarantee.  All other cover traffic uses the normal jitter path. */
        if (trellis_morph_is_constant_rate(morph)) {
            trellis_conn_send(target->conn, send_buf.data, send_buf.len, NULL, NULL);
            free(send_buf.data);
        } else {
            send_with_jitter(client, target->conn, send_buf.data, send_buf.len);
        }
    } else {
        trellis_buf_free(&send_buf);
    }
}

// --- Event loop thread ---
static void on_stop_async(uv_async_t *handle)
{
    uv_stop(handle->loop);
}

static void on_send_async(uv_async_t *handle)
{
    trellis_client_t *client = handle->data;
    if (!client || !client->running)
        return;

    uv_mutex_lock(&client->send_lock);
    trellis_send_item_t *queue = client->send_head;
    client->send_head = NULL;
    uv_mutex_unlock(&client->send_lock);

    int item_count = 0;
    while (queue) {
        trellis_send_item_t *item = queue;
        queue = item->next;
        item_count++;

        trellis_buf_t wire = { .data = item->data, .len = item->len };
        broadcast_to_peers(client, &wire);

        free(item->data);
        free(item);
    }
}

static void loop_thread_fn(void *arg)
{
    trellis_client_t *client = (trellis_client_t *)arg;
    uv_run(&client->loop, UV_RUN_DEFAULT);
}

// --- Lifecycle ---
trellis_client_t *trellis_client_new(const trellis_client_config_t *cfg)
{
    if (!cfg)
        return NULL;

    trellis_client_t *c = calloc(1, sizeof(*c));
    if (!c)
        return NULL;

    c->config = *cfg;

    if (cfg->listen_addr) {
        c->config.listen_addr = strdup(cfg->listen_addr);
        if (!c->config.listen_addr) {
            free(c);
            return NULL;
        }
    }

    if (cfg->seed_addr) {
        c->config.seed_addr = strdup(cfg->seed_addr);
        if (!c->config.seed_addr) {
            free((void *)c->config.listen_addr);
            free(c);
            return NULL;
        }
    }

    if (cfg->identity_path) {
        c->config.identity_path = strdup(cfg->identity_path);
        if (!c->config.identity_path) {
            free((void *)c->config.seed_addr);
            free((void *)c->config.listen_addr);
            free(c);
            return NULL;
        }
    }

    return c;
}

trellis_err_t trellis_client_start(trellis_client_t *client)
{
    if (!client)
        return TRELLIS_ERR_INVALID_ARG;

    if (client->running)
        return TRELLIS_ERR_ALREADY_EXISTS;

    trellis_err_t err = trellis_crypto_init();
    if (err != TRELLIS_OK)
        return err;

    // Load persisted identity or generate a new one.
    char *id_path = NULL;
    if (client->config.identity_path) {
        id_path = strdup(client->config.identity_path);
    } else {
        id_path = trellis_identity_default_path();
    }

    bool id_loaded = false;
    if (id_path && trellis_identity_load(id_path, &client->identity) == TRELLIS_OK)
        id_loaded = true;

    if (!id_loaded) {
        err = trellis_identity_generate(&client->identity);
        if (err != TRELLIS_OK) {
            free(id_path);
            return err;
        }
        if (id_path) {
            err = trellis_identity_save(&client->identity, id_path);
            if (err != TRELLIS_OK) {
                free(id_path);
                return err;
            }
        }
    }
    free(id_path);

    /*
     * From here on, all subsystems are stored in the client struct.
     * On failure we simply return the error -- trellis_client_free()
     * handles partial teardown (NULL-checks every subsystem, drains
     * the libuv loop, etc.).  The previous goto-chain called
     * uv_loop_close() in the fail path, which left the loop in a
     * closed state; a subsequent trellis_client_free() then crashed
     * trying to uv_walk() the dead loop.
     */
    uv_loop_init(&client->loop);
    client->loop_initialized = true;

    switch (client->config.transport_mode) {
#ifdef TRELLIS_WITH_TLS
    case TRELLIS_TRANSPORT_TLS: {
        trellis_tls_config_t tls_cfg = {0};
        tls_cfg.verify_peer = client->config.verify_peer;
        if (client->config.tls_front_host)
            tls_cfg.front_host = client->config.tls_front_host;
        if (client->config.tls_relay_target)
            tls_cfg.relay_target = client->config.tls_relay_target;
        if (client->config.tls_ech_public_name)
            tls_cfg.ech_public_name = client->config.tls_ech_public_name;
        client->transport = trellis_transport_tls_new(&client->loop, &tls_cfg);
        break;
    }
#endif
    case TRELLIS_TRANSPORT_PT:
        client->transport = trellis_transport_pt_new(
            &client->loop,
            client->config.pt_binary,
            client->config.pt_transport_name,
            client->config.pt_args);
        break;
    default:
        client->transport = trellis_transport_tcp_new(&client->loop);
        break;
    }
    if (!client->transport)
        return TRELLIS_ERR_TRANSPORT;

    client->dht = trellis_dht_new(&client->identity, &client->loop);
    if (!client->dht)
        return TRELLIS_ERR_DHT;
    trellis_dht_set_eclipse_cb(client->dht, on_eclipse_detected, client);

    client->gossip = trellis_gossip_new(client->dht, &client->identity);
    if (!client->gossip)
        return TRELLIS_ERR_GOSSIP;
    trellis_gossip_set_send(client->gossip, gossip_send_cb, client);

    client->router = trellis_router_new(client->dht, client);
    if (!client->router)
        return TRELLIS_ERR_ROUTING;
    trellis_router_set_mode(client->router, client->config.default_routing);

    /* Guard node pinning (Tor parity): create and attach guard set so
     * onion circuits prefer stable, high-reputation entry nodes. */
    if (client->config.guard_enabled && trellis_dht_sybil(client->dht)) {
        const char *guard_path = client->config.guard_persist_path;
        char *auto_path = NULL;
        if (!guard_path && client->config.identity_path) {
            size_t id_len = strlen(client->config.identity_path);
            auto_path = malloc(id_len + sizeof(".guards"));
            if (auto_path) {
                memcpy(auto_path, client->config.identity_path, id_len);
                memcpy(auto_path + id_len, ".guards", sizeof(".guards"));
                guard_path = auto_path;
            }
        }
        client->guard = trellis_guard_new(client->dht,
                                           trellis_dht_sybil(client->dht),
                                           guard_path);
        free(auto_path);
        if (client->guard) {
            trellis_router_set_guard(client->router, client->guard);
            trellis_guard_select(client->guard, trellis_now_ms());
        }
    }

    client->relay = trellis_relay_new(client->dht, 0);
    if (client->relay)
        trellis_relay_start(client->relay, &client->loop);

    // Start preemptive circuit pool for the router.
    if (client->router && !client->config.disable_circuit_pool)
        trellis_circuit_pool_start(client->router, &client->loop);

    // Onion replay protection.
    {
        dedup_set_t *replay = malloc(sizeof(dedup_set_t));
        if (replay && dedup_set_init(replay, DEDUP_SET_INIT_CAP) == 0)
            client->onion_replay = replay;
        else
            free(replay);
    }

    client->incentive = trellis_incentive_new(&client->identity, &client->loop);
    if (client->incentive)
        trellis_incentive_set_send(client->incentive, incentive_send_cb, client);

    client->topology = trellis_topology_new(client->dht, &client->loop);
    if (!client->topology)
        return TRELLIS_ERR_DHT;

    trellis_morph_config_t mcfg = trellis_morph_config_default();
    mcfg.metamorphic = client->config.morph_metamorphic;
    mcfg.cover_traffic_interval_ms = client->config.cover_traffic_ms;
    mcfg.cell_mode = client->config.morph_cell_mode;
    if (client->config.morph_cell_size > 0)
        mcfg.cell_size = client->config.morph_cell_size;
    client->morph = trellis_morph_new(&mcfg);
    if (!client->morph)
        return TRELLIS_ERR_MORPH;

    client->extensions = trellis_extensions_new();
    if (!client->extensions)
        return TRELLIS_ERR_NOMEM;

    if (uv_async_init(&client->loop, &client->stop_async, on_stop_async) != 0)
        return TRELLIS_ERR_TRANSPORT;
    client->stop_async.data = client;

    uv_mutex_init(&client->send_lock);
    client->send_lock_initialized = true;
    client->send_async.data = client;
    if (uv_async_init(&client->loop, &client->send_async, on_send_async) != 0)
        return TRELLIS_ERR_TRANSPORT;

    if (client->config.listen_addr) {
        err = trellis_transport_listen(client->transport,
                                      client->config.listen_addr,
                                      trellis_client_on_accept, client);
        if (err != TRELLIS_OK) {
            if (strstr(client->config.listen_addr, ":0") != NULL
                || strcmp(client->config.listen_addr, "0") == 0) {
                err = trellis_transport_listen(client->transport,
                                              "0.0.0.0:9100",
                                              trellis_client_on_accept, client);
            }
            if (err != TRELLIS_OK)
                return err;
        }

        int port = trellis_transport_bound_port(client->transport);
        if (port > 0) {
            client->listen_port = (uint16_t)port;

            // Preserve the configured host; only replace the port.
            const char *orig = client->config.listen_addr;
            const char *colon = strrchr(orig, ':');
            char resolved[320];
            if (colon) {
                size_t host_len = (size_t)(colon - orig);
                if (host_len >= sizeof(resolved) - 8)
                    host_len = sizeof(resolved) - 8;
                memcpy(resolved, orig, host_len);
                snprintf(resolved + host_len,
                         sizeof(resolved) - host_len, ":%d", port);
            } else {
                snprintf(resolved, sizeof(resolved), "%s:%d", orig, port);
            }

            char *new_addr = strdup(resolved);
            if (!new_addr)
                return TRELLIS_ERR_NOMEM;
            free((void *)client->config.listen_addr);
            client->config.listen_addr = new_addr;
        }
    }

    if (client->config.seed_addr) {
        err = trellis_dht_bootstrap(client->dht, client->config.seed_addr);
        if (err != TRELLIS_OK)
            return err;

        const char *connect_addr = strip_proto_prefix(client->config.seed_addr);
        trellis_transport_connect(client->transport, connect_addr,
                                  on_seed_connected, client);
    }

    trellis_topology_start(client->topology);

    if (!client->config.disable_discovery && client->listen_port > 0)
        trellis_discovery_start(client);

    /* Phase 5: periodic cover traffic.
     * In constant-rate mode, the interval comes from the morph config;
     * otherwise it comes from the client config. */
    {
        uint64_t cover_interval = client->config.cover_traffic_ms;
        if (trellis_morph_is_constant_rate(client->morph)) {
            uint32_t cr_interval = trellis_morph_constant_rate_interval(client->morph);
            if (cr_interval > 0)
                cover_interval = cr_interval;
        }
        if (cover_interval > 0) {
            uv_timer_init(&client->loop, &client->cover_timer);
            client->cover_timer.data = client;
            client->cover_timer_active = true;
            uint64_t initial_delay =
                randombytes_uniform((uint32_t)cover_interval);
            uv_timer_start(&client->cover_timer, trellis_client_cover_tick,
                            initial_delay, cover_interval);
        }
    }

    // Keepalive timer: PING all peers every 30s.
    uv_timer_init(&client->loop, &client->keepalive_timer);
    client->keepalive_timer.data = client;
    client->keepalive_timer_active = true;
    uv_timer_start(&client->keepalive_timer, on_keepalive_timer, 30000, 30000);

    // Peer exchange timer: re-share peer tables every 60s.
    uv_timer_init(&client->loop, &client->pex_timer);
    client->pex_timer.data = client;
    client->pex_timer_active = true;
    uv_timer_start(&client->pex_timer, on_pex_timer, 60000, 60000);

    // Guard tick timer: expire/rotate guards and vanguards every 60s.
    if (client->guard) {
        uv_timer_init(&client->loop, &client->guard_timer);
        client->guard_timer.data = client;
        client->guard_timer_active = true;
        uv_timer_start(&client->guard_timer, on_guard_tick_timer, 60000, 60000);
    }

    // Temporal metadata countermeasures.
    client->always_on_daemon       = client->config.always_on_daemon;
    client->decoy_interval_ms      = client->config.decoy_circuit_interval_ms;
    client->msg_batch_delay_ms     = client->config.msg_batch_delay_ms;
    client->msg_batch_max          = client->config.msg_batch_max;
    trellis_client_start_decoy_timer(client);

    // Reconnection address list.
    client->reconnect_cap = 32;
    client->reconnect_addrs = calloc(client->reconnect_cap, sizeof(char *));
    if (!client->reconnect_addrs)
        return TRELLIS_ERR_NOMEM;
    client->reconnect_count = 0;

    // Store seed address for reconnection.
    if (client->config.seed_addr) {
        const char *addr = strip_proto_prefix(client->config.seed_addr);
        if (client->reconnect_addrs) {
            client->reconnect_addrs[client->reconnect_count] = strdup(addr);
            if (client->reconnect_addrs[client->reconnect_count])
                client->reconnect_count++;
        }
    }

    // STUN probe to discover public address for peer exchange.
    client->nat = trellis_nat_new(&client->loop);
    if (client->nat)
        trellis_nat_probe(client->nat, "stun.l.google.com", 19302);

    /* DDoS Defense Layer 0: per-connection PoW challenge server.
     * Fixed difficulty avoids Tor's control loop gaming attacks.
     * UINT32_MAX disables PoW gating entirely. */
    if (client->config.conn_pow_iterations != UINT32_MAX)
        client->conn_pow = trellis_conn_pow_server_new(
            client->config.conn_pow_iterations);

    /* DPI Defense 1B: active probe resistance guard, keyed to our own
       identity so we can validate incoming handshake messages. */
    {
        uint8_t obfs_key[TRELLIS_OBFS_KEY_LEN];
        trellis_derive_obfs_key(client->identity.x25519_pk,
                                TRELLIS_X25519_PK_LEN, obfs_key);
        client->probe_guard = trellis_probe_guard_new(obfs_key,
                                                       sizeof(obfs_key));
        client->probe_guard_epoch = trellis_now_ms() / 3600000ULL;
        sodium_memzero(obfs_key, sizeof(obfs_key));
    }

    /* Probe-guard epoch timer: rekey every 60s so we catch hourly
     * obfs-key rotations regardless of whether cover traffic is active. */
    if (client->probe_guard) {
        uv_timer_init(&client->loop, &client->probe_guard_timer);
        client->probe_guard_timer.data = client;
        client->probe_guard_timer_active = true;
        uv_timer_start(&client->probe_guard_timer,
                       on_probe_guard_epoch_timer, 60000, 60000);
    }

    client->running = true;

    int rc = uv_thread_create(&client->loop_thread, loop_thread_fn, client);
    if (rc != 0) {
        client->running = false;
        return TRELLIS_ERR_TRANSPORT;
    }
    client->loop_thread_started = true;

    trellis_event_data_t ev = {0};
    ev.event = TRELLIS_EV_STARTED;
    ev.peer = client->identity.fingerprint;
    trellis_emit(client, &ev);

    return TRELLIS_OK;
}

trellis_err_t trellis_client_stop(trellis_client_t *client)
{
    if (!client)
        return TRELLIS_ERR_INVALID_ARG;

    if (!client->running)
        return TRELLIS_OK;

    client->running = false;

    /* Stop the event-loop thread FIRST — subsystem teardown closes
       file descriptors, and doing that while the loop thread is still
       polling them causes epoll_ctl to abort(). */
    if (client->loop_thread_started) {
        uv_async_send(&client->stop_async);
        uv_thread_join(&client->loop_thread);
        client->loop_thread_started = false;
    }

    if (client->cover_timer_active) {
        uv_timer_stop(&client->cover_timer);
        client->cover_timer_active = false;
    }

    if (client->keepalive_timer_active) {
        uv_timer_stop(&client->keepalive_timer);
        client->keepalive_timer_active = false;
    }

    if (client->pex_timer_active) {
        uv_timer_stop(&client->pex_timer);
        client->pex_timer_active = false;
    }

    if (client->guard_timer_active) {
        uv_timer_stop(&client->guard_timer);
        client->guard_timer_active = false;
    }

    if (client->probe_guard_timer_active) {
        uv_timer_stop(&client->probe_guard_timer);
        client->probe_guard_timer_active = false;
    }

    trellis_client_stop_decoy_timer(client);

    if (client->router)
        trellis_circuit_pool_stop(client->router);

    if (client->relay)
        trellis_circuit_table_init(&client->relay->circuit_table);

    // Flush and tear down message batch queue.
    trellis_client_batch_flush(client);
    if (client->batch_timer_active) {
        uv_timer_stop(&client->batch_timer);
        client->batch_timer_active = false;
    }
    free(client->batch_queue);
    client->batch_queue = NULL;
    client->batch_count = 0;
    client->batch_cap   = 0;

    for (size_t i = 0; i < client->reconnect_count; i++)
        free(client->reconnect_addrs[i]);
    free(client->reconnect_addrs);
    client->reconnect_addrs = NULL;
    client->reconnect_count = 0;

    if (client->nat) {
        trellis_nat_free(client->nat);
        client->nat = NULL;
    }

    trellis_discovery_stop(client);

    if (client->greenhouse) {
        trellis_greenhouse_stop(client->greenhouse);
        trellis_greenhouse_free(client->greenhouse);
        client->greenhouse = NULL;
    }

    trellis_topology_stop(client->topology);
    trellis_transport_stop(client->transport);

    if (client->send_lock_initialized) {
        uv_mutex_lock(&client->send_lock);
        trellis_send_item_t *sq = client->send_head;
        client->send_head = NULL;
        uv_mutex_unlock(&client->send_lock);
        while (sq) {
            trellis_send_item_t *next = sq->next;
            free(sq->data);
            free(sq);
            sq = next;
        }
    }

    for (size_t i = 0; i < client->peer_conn_count; i++) {
        trellis_peer_conn_t *pc = &client->peers[i];
        if (pc->handshake_timer) {
            uv_timer_stop(pc->handshake_timer);
            pc->handshake_timer = NULL;
        }
        if (pc->ratchet)
            trellis_ratchet_free(pc->ratchet);
        if (pc->handshake)
            trellis_handshake_free(pc->handshake);
        if (pc->morph)
            trellis_morph_free(pc->morph);
    }
    client->peer_conn_count = 0;

    trellis_event_data_t ev = {0};
    ev.event = TRELLIS_EV_STOPPED;
    ev.peer = client->identity.fingerprint;
    trellis_emit(client, &ev);

    return TRELLIS_OK;
}

static void close_walk_cb(uv_handle_t *handle, void *arg)
{
    (void)arg;
    if (!uv_is_closing(handle))
        uv_close(handle, NULL);
}

void trellis_client_free(trellis_client_t *client)
{
    if (!client)
        return;

    if (client->running)
        trellis_client_stop(client);

    if (client->loop_initialized) {
        uv_walk(&client->loop, close_walk_cb, NULL);
        while (uv_loop_alive(&client->loop))
            uv_run(&client->loop, UV_RUN_ONCE);
    }

    trellis_canopy_free(client->canopy);
    trellis_bridge_free(client->bridge);
    trellis_extensions_free(client->extensions);
    trellis_morph_free(client->morph);
    trellis_topology_free(client->topology);
    trellis_guard_free(client->guard);
    trellis_router_free(client->router);
    trellis_relay_free(client->relay);
    trellis_incentive_free(client->incentive);
    trellis_gossip_free(client->gossip);
    trellis_dht_free(client->dht);
    trellis_transport_free(client->transport);

    for (size_t i = 0; i < client->garden_count; i++) {
        free(client->gardens[i].name);
        trellis_garden_free(client->gardens[i].garden);
    }
    free(client->gardens);
    free(client->peers);

    free(client->handlers);
    free(client->alias);

    if (client->send_lock_initialized) {
        uv_mutex_destroy(&client->send_lock);
        client->send_lock_initialized = false;
    }

    trellis_probe_guard_free(client->probe_guard);
    trellis_conn_pow_server_free(client->conn_pow);

    if (client->onion_replay) {
        dedup_set_destroy((dedup_set_t *)client->onion_replay);
        free(client->onion_replay);
    }

    sodium_memzero(&client->identity, sizeof(client->identity));
    free((void *)client->config.listen_addr);
    free((void *)client->config.seed_addr);

    if (client->loop_initialized)
        uv_loop_close(&client->loop);
    free(client);
}

trellis_err_t trellis_client_on(trellis_client_t *client,
                                 trellis_event_t event,
                                 trellis_event_cb cb, void *ctx)
{
    if (!client || !cb)
        return TRELLIS_ERR_INVALID_ARG;

    if (client->handler_count >= client->handler_cap) {
        size_t new_cap = client->handler_cap
                         ? client->handler_cap * 2
                         : TRELLIS_EVENT_HANDLERS_INIT_CAP;
        trellis_event_handler_t *arr = realloc(
            client->handlers, new_cap * sizeof(*arr));
        if (!arr)
            return TRELLIS_ERR_NOMEM;
        client->handlers = arr;
        client->handler_cap = new_cap;
    }

    trellis_event_handler_t *h = &client->handlers[client->handler_count++];
    h->event = event;
    h->cb = cb;
    h->ctx = ctx;
    return TRELLIS_OK;
}

// --- Identity ---
const trellis_identity_t *trellis_client_identity(const trellis_client_t *client)
{
    return client ? &client->identity : NULL;
}

trellis_err_t trellis_client_set_alias(trellis_client_t *client,
                                        const char *alias)
{
    if (!client || !alias)
        return TRELLIS_ERR_INVALID_ARG;

    char *new_alias = strdup(alias);
    if (!new_alias)
        return TRELLIS_ERR_NOMEM;

    free(client->alias);
    client->alias = new_alias;

    if (client->running) {
        trellis_message_t msg = {0};
        msg.version = TRELLIS_PROTOCOL_VERSION;
        msg.type = TRELLIS_MSG_ALIAS;
        msg.flags = 0;
        msg.sequence = client->next_sequence++;
        msg.timestamp = trellis_now_ms();
        msg.source = client->identity.fingerprint;
        msg.payload_len = (uint32_t)strlen(alias);
        msg.payload = (uint8_t *)alias;

        trellis_err_t err = trellis_message_sign(&msg, &client->identity);
        if (err == TRELLIS_OK) {
            trellis_buf_t wire = {0};
            err = trellis_message_serialize(&msg, &wire);
            if (err == TRELLIS_OK) {
                queue_broadcast(client, &wire);
                trellis_buf_free(&wire);
            }
        }
    }

    trellis_event_data_t ev = {0};
    ev.event = TRELLIS_EV_ALIAS_CHANGED;
    ev.peer = client->identity.fingerprint;
    ev.alias = client->alias;
    trellis_emit(client, &ev);

    return TRELLIS_OK;
}

const char *trellis_client_alias(const trellis_client_t *client)
{
    return client ? client->alias : NULL;
}

// --- Garden operations ---
trellis_err_t trellis_client_create_garden(trellis_client_t *client,
                                            const char *name,
                                            trellis_garden_type_t type)
{
    if (!client || !name)
        return TRELLIS_ERR_INVALID_ARG;

    if (trellis_client_find_garden(client, name))
        return TRELLIS_ERR_ALREADY_EXISTS;

    trellis_garden_t *g = trellis_garden_create(name, type, &client->identity);
    if (!g)
        return TRELLIS_ERR_GARDEN;

    trellis_err_t err = garden_map_insert(client, name, g);
    if (err != TRELLIS_OK) {
        trellis_garden_free(g);
        return err;
    }

    if (client->running && client->gossip) {
        trellis_gossip_publish(client->gossip,
                               (const uint8_t *)name, strlen(name));
    }

    trellis_event_data_t ev = {0};
    ev.event = TRELLIS_EV_GARDEN_CREATED;
    ev.peer = client->identity.fingerprint;
    ev.garden_name = name;
    trellis_emit(client, &ev);

    return TRELLIS_OK;
}

trellis_err_t trellis_client_join_garden(trellis_client_t *client,
                                          const char *name)
{
    if (!client || !name)
        return TRELLIS_ERR_INVALID_ARG;

    trellis_garden_t *g = trellis_client_find_garden(client, name);
    if (!g)
        return TRELLIS_ERR_NOT_FOUND;

    trellis_err_t err = trellis_garden_subscribe(g,
                                                 &client->identity.fingerprint);
    if (err != TRELLIS_OK && err != TRELLIS_ERR_ALREADY_EXISTS)
        return err;

    if (client->running && client->gossip) {
        trellis_gossip_publish(client->gossip,
                               (const uint8_t *)name, strlen(name));
    }

    if (client->running) {
        trellis_message_t msg = {0};
        msg.version = TRELLIS_PROTOCOL_VERSION;
        msg.type = TRELLIS_MSG_SUBSCRIBE;
        msg.sequence = client->next_sequence++;
        msg.timestamp = trellis_now_ms();
        msg.source = client->identity.fingerprint;
        msg.payload_len = (uint32_t)strlen(name);
        msg.payload = (uint8_t *)name;

        trellis_err_t sign_err = trellis_message_sign(&msg, &client->identity);
        if (sign_err == TRELLIS_OK) {
            trellis_buf_t wire = {0};
            sign_err = trellis_message_serialize(&msg, &wire);
            if (sign_err == TRELLIS_OK) {
                queue_broadcast(client, &wire);
                trellis_buf_free(&wire);
            }
        }
    }

    trellis_event_data_t ev = {0};
    ev.event = TRELLIS_EV_GARDEN_SUBSCRIBED;
    ev.peer = client->identity.fingerprint;
    ev.garden_name = name;
    trellis_emit(client, &ev);

    return TRELLIS_OK;
}

trellis_err_t trellis_client_leave_garden(trellis_client_t *client,
                                           const char *name)
{
    if (!client || !name)
        return TRELLIS_ERR_INVALID_ARG;

    trellis_garden_t *g = trellis_client_find_garden(client, name);
    if (!g)
        return TRELLIS_ERR_NOT_FOUND;

    if (client->running) {
        trellis_message_t msg = {0};
        msg.version = TRELLIS_PROTOCOL_VERSION;
        msg.type = TRELLIS_MSG_UNSUBSCRIBE;
        msg.sequence = client->next_sequence++;
        msg.timestamp = trellis_now_ms();
        msg.source = client->identity.fingerprint;
        msg.payload_len = (uint32_t)strlen(name);
        msg.payload = (uint8_t *)name;

        trellis_err_t sign_err = trellis_message_sign(&msg, &client->identity);
        if (sign_err == TRELLIS_OK) {
            trellis_buf_t wire = {0};
            sign_err = trellis_message_serialize(&msg, &wire);
            if (sign_err == TRELLIS_OK) {
                queue_broadcast(client, &wire);
                trellis_buf_free(&wire);
            }
        }
    }

    trellis_event_data_t ev = {0};
    ev.event = TRELLIS_EV_GARDEN_UNSUBSCRIBED;
    ev.peer = client->identity.fingerprint;
    ev.garden_name = name;
    trellis_emit(client, &ev);

    garden_map_remove(client, name);

    return TRELLIS_OK;
}

trellis_err_t trellis_client_send_message(trellis_client_t *client,
                                           const char *garden_name,
                                           const uint8_t *data, size_t len)
{
    if (!client || !garden_name || !data || len == 0)
        return TRELLIS_ERR_INVALID_ARG;

    if (!client->running)
        return TRELLIS_ERR_NOT_INITIALIZED;

    trellis_garden_t *g = trellis_client_find_garden(client, garden_name);
    if (!g)
        return TRELLIS_ERR_NOT_FOUND;

    trellis_message_t msg = {0};
    msg.version = TRELLIS_PROTOCOL_VERSION;
    msg.type = TRELLIS_MSG_MSG;
    msg.flags = TRELLIS_FLAG_ENCRYPTED;
    msg.sequence = client->next_sequence++;
    msg.timestamp = trellis_now_ms();
    msg.source = client->identity.fingerprint;
    msg.payload_len = (uint32_t)len;
    msg.payload = (uint8_t *)data;

    trellis_err_t err = trellis_message_sign(&msg, &client->identity);
    if (err != TRELLIS_OK)
        return err;

    trellis_buf_t wire = {0};
    err = trellis_message_serialize(&msg, &wire);
    if (err != TRELLIS_OK)
        return err;

    trellis_garden_post_message(g, &client->identity.fingerprint, data, len);

    // When message batching is enabled, defer sending for temporal resistance.
    if (trellis_client_batch_enqueue(client, &wire) == TRELLIS_OK) {
        trellis_buf_free(&wire);
        return TRELLIS_OK;
    }

    queue_broadcast(client, &wire);
    trellis_buf_free(&wire);
    return TRELLIS_OK;
}

trellis_err_t trellis_client_send_dm(trellis_client_t *client,
                                      const trellis_fingerprint_t *target,
                                      const uint8_t *data, size_t len)
{
    if (!client || !target || !data || len == 0)
        return TRELLIS_ERR_INVALID_ARG;

    if (!client->running)
        return TRELLIS_ERR_NOT_INITIALIZED;

    trellis_message_t msg = {0};
    msg.version = TRELLIS_PROTOCOL_VERSION;
    msg.type = TRELLIS_MSG_MSG;
    msg.flags = TRELLIS_FLAG_ENCRYPTED;
    msg.sequence = client->next_sequence++;
    msg.timestamp = trellis_now_ms();
    msg.source = client->identity.fingerprint;
    msg.target = *target;
    msg.payload_len = (uint32_t)len;
    msg.payload = (uint8_t *)data;

    trellis_err_t err = trellis_message_sign(&msg, &client->identity);
    if (err != TRELLIS_OK)
        return err;

    trellis_buf_t wire = {0};
    err = trellis_message_serialize(&msg, &wire);
    if (err != TRELLIS_OK)
        return err;

    /*
     * Route through the router when a non-direct routing mode is active
     * (relay, multipath, onion). Fall back to broadcast on failure or
     * when no router is configured.
     */
    if (client->router &&
        trellis_router_mode(client->router) != TRELLIS_ROUTE_DIRECT) {
        err = trellis_router_send(client->router, target,
                                  wire.data, wire.len, NULL, NULL);
        trellis_buf_free(&wire);
        return err;
    }

    queue_broadcast(client, &wire);
    trellis_buf_free(&wire);
    return TRELLIS_OK;
}

// --- Mesh configuration ---
trellis_err_t trellis_client_connect_addr(trellis_client_t *client,
                                           const char *addr)
{
    if (!client || !addr || !client->transport)
        return TRELLIS_ERR_INVALID_ARG;

    const char *clean = strstr(addr, "://");
    if (clean)
        clean += 3;
    else
        clean = addr;

    return trellis_transport_connect(client->transport, clean,
                                     on_seed_connected, client);
}

trellis_err_t trellis_client_set_routing(trellis_client_t *client,
                                          trellis_routing_mode_t mode)
{
    if (!client)
        return TRELLIS_ERR_INVALID_ARG;

    trellis_err_t err = TRELLIS_OK;
    if (client->router)
        err = trellis_router_set_mode(client->router, mode);

    if (err == TRELLIS_OK) {
        trellis_event_data_t ev = {0};
        ev.event = TRELLIS_EV_ROUTING_CHANGED;
        ev.peer = client->identity.fingerprint;
        trellis_emit(client, &ev);
    }

    return err;
}

trellis_err_t trellis_client_set_transport_mode(trellis_client_t *client,
                                                trellis_transport_mode_t mode)
{
    if (!client)
        return TRELLIS_ERR_INVALID_ARG;

    client->config.transport_mode = mode;

    trellis_event_data_t ev = {0};
    ev.event = TRELLIS_EV_MORPH_CHANGED;
    ev.peer = client->identity.fingerprint;
    trellis_emit(client, &ev);

    return TRELLIS_OK;
}

size_t trellis_client_peer_count(const trellis_client_t *client)
{
    if (!client)
        return 0;

    size_t count = 0;
    for (size_t i = 0; i < client->peer_conn_count; i++) {
        if (!client->peers[i].handshake_complete || !client->peers[i].conn)
            continue;
        bool dup = false;
        for (size_t j = 0; j < i; j++) {
            if (client->peers[j].handshake_complete && client->peers[j].conn &&
                trellis_fingerprint_eq(&client->peers[i].remote_fp,
                                       &client->peers[j].remote_fp)) {
                dup = true;
                break;
            }
        }
        if (!dup)
            count++;
    }
    return count;
}

size_t trellis_client_conn_count(const trellis_client_t *client)
{
    return client ? client->peer_conn_count : 0;
}

const char *trellis_client_listen_addr(const trellis_client_t *client)
{
    return client ? client->config.listen_addr : NULL;
}

bool trellis_client_discovery_active(const trellis_client_t *client)
{
    return client ? client->discovery_active : false;
}

// --- Greenhouse ---
trellis_err_t trellis_client_start_greenhouse(trellis_client_t *client,
                                               trellis_greenhouse_type_t type,
                                               trellis_greenhouse_conn_cb cb,
                                               void *ctx)
{
    if (!client || !cb)
        return TRELLIS_ERR_INVALID_ARG;

    if (!client->running)
        return TRELLIS_ERR_NOT_INITIALIZED;

    if (client->greenhouse)
        return TRELLIS_ERR_ALREADY_EXISTS;

    client->greenhouse = trellis_greenhouse_new(&client->identity,
                                                type, client->dht);
    if (!client->greenhouse)
        return TRELLIS_ERR_GREENHOUSE;

    trellis_err_t err = trellis_greenhouse_start(client->greenhouse, cb, ctx);
    if (err != TRELLIS_OK) {
        trellis_greenhouse_free(client->greenhouse);
        client->greenhouse = NULL;
        return err;
    }

    trellis_event_data_t ev = {0};
    ev.event = TRELLIS_EV_GREENHOUSE_STARTED;
    ev.peer = client->identity.fingerprint;
    trellis_emit(client, &ev);

    return TRELLIS_OK;
}

trellis_err_t trellis_client_connect_greenhouse(trellis_client_t *client,
                                                 const char *bloom_uri,
                                                 trellis_connect_cb cb,
                                                 void *ctx)
{
    if (!client || !bloom_uri)
        return TRELLIS_ERR_INVALID_ARG;

    if (!client->running)
        return TRELLIS_ERR_NOT_INITIALIZED;

    trellis_bloom_addr_t addr;
    trellis_err_t err = trellis_bloom_addr_parse(bloom_uri, &addr);
    if (err != TRELLIS_OK)
        return err;

    return trellis_greenhouse_connect(client->dht, &addr, cb, ctx);
}

// --- Exit relay client ---
#define EXIT_POW_NONCE_LEN  32
#define EXIT_POW_TOKEN_LEN  (EXIT_POW_NONCE_LEN + 4)  /* nonce + counter */

static bool exit_pow_check_bits(const uint8_t hash[32], uint32_t bits)
{
    uint32_t checked = 0;
    for (size_t i = 0; i < 32 && checked < bits; i++) {
        uint8_t byte = hash[i];
        for (int b = 7; b >= 0 && checked < bits; b--) {
            if ((byte >> b) & 1) return false;
            checked++;
        }
    }
    return true;
}

static trellis_err_t exit_pow_generate(uint32_t difficulty,
                                       uint8_t token_out[EXIT_POW_TOKEN_LEN])
{
    if (difficulty == 0) {
        randombytes_buf(token_out, EXIT_POW_TOKEN_LEN);
        return TRELLIS_OK;
    }

    randombytes_buf(token_out, EXIT_POW_NONCE_LEN);

    for (uint32_t ctr = 0; ; ctr++) {
        token_out[EXIT_POW_NONCE_LEN + 0] = (uint8_t)(ctr);
        token_out[EXIT_POW_NONCE_LEN + 1] = (uint8_t)(ctr >> 8);
        token_out[EXIT_POW_NONCE_LEN + 2] = (uint8_t)(ctr >> 16);
        token_out[EXIT_POW_NONCE_LEN + 3] = (uint8_t)(ctr >> 24);

        uint8_t hash[32];
        crypto_hash_sha256(hash, token_out, EXIT_POW_TOKEN_LEN);

        if (exit_pow_check_bits(hash, difficulty))
            return TRELLIS_OK;

        if (ctr == UINT32_MAX)
            return TRELLIS_ERR_NOMEM;
    }
}

trellis_err_t trellis_client_connect_exit(trellis_client_t *client,
                                           const char *host, uint16_t port)
{
    if (!client || !host || port == 0)
        return TRELLIS_ERR_INVALID_ARG;

    if (!client->running)
        return TRELLIS_ERR_NOT_INITIALIZED;

    if (!client->router)
        return TRELLIS_ERR_NOT_INITIALIZED;

    // Select an exit-capable peer from the DHT.
    trellis_peer_info_t peers[TRELLIS_DHT_K];
    size_t n = trellis_dht_get_peers(client->dht, peers, TRELLIS_DHT_K);

    trellis_fingerprint_t exit_fp = {0};
    bool found_exit = false;
    for (size_t i = 0; i < n; i++) {
        if (peers[i].exit_capable && !peers[i].quarantined) {
            exit_fp = peers[i].fingerprint;
            found_exit = true;
            break;
        }
    }

    if (!found_exit) {
        fprintf(stderr, "[exit-client] no exit-capable peers in DHT\n");
        return TRELLIS_ERR_NOT_FOUND;
    }

    // Generate PoW token for exit relay anti-abuse.
    uint8_t pow_token[EXIT_POW_TOKEN_LEN];
    trellis_err_t pow_err = exit_pow_generate(TRELLIS_EXIT_POW_DIFFICULTY,
                                              pow_token);
    if (pow_err != TRELLIS_OK)
        return pow_err;

    // Build the EXIT_STREAM payload: [cmd(1)][host_len(1)][host][port_be(2)][pow_token]
    size_t host_len = strlen(host);
    if (host_len > 255)
        return TRELLIS_ERR_INVALID_ARG;

    size_t stream_payload_len = 1 + 1 + host_len + 2 + EXIT_POW_TOKEN_LEN;
    uint8_t *stream_payload = malloc(stream_payload_len);
    if (!stream_payload)
        return TRELLIS_ERR_NOMEM;

    size_t off = 0;
    stream_payload[off++] = TRELLIS_EXIT_CMD_OPEN;
    stream_payload[off++] = (uint8_t)host_len;
    memcpy(stream_payload + off, host, host_len); off += host_len;
    stream_payload[off++] = (uint8_t)(port >> 8);
    stream_payload[off++] = (uint8_t)(port & 0xFF);
    memcpy(stream_payload + off, pow_token, EXIT_POW_TOKEN_LEN);

    /* Force onion routing mode for exit traffic so the circuit is built
     * with 3 relay hops before reaching the exit node.  Send the raw
     * stream payload directly — the router tags the inner onion layer
     * with TRELLIS_MSG_EXIT_STREAM based on the exit-capable destination. */
    trellis_routing_mode_t prev_mode = trellis_router_mode(client->router);
    if (prev_mode != TRELLIS_ROUTE_ONION)
        trellis_router_set_mode(client->router, TRELLIS_ROUTE_ONION);

    trellis_err_t err = trellis_router_send(client->router, &exit_fp,
                                            stream_payload, stream_payload_len,
                                            NULL, NULL);

    if (prev_mode != TRELLIS_ROUTE_ONION)
        trellis_router_set_mode(client->router, prev_mode);

    free(stream_payload);

    fprintf(stderr, "[exit-client] sent EXIT_STREAM for %s:%u via %02x%02x...\n",
            host, port, exit_fp.bytes[0], exit_fp.bytes[1]);

    return err;
}

// --- Bidirectional exit stream API ---
static trellis_exit_stream_t *exit_stream_alloc(trellis_client_t *client)
{
    for (int i = 0; i < TRELLIS_EXIT_STREAM_MAX; i++) {
        if (!client->exit_streams[i].active) {
            trellis_exit_stream_t *es = &client->exit_streams[i];
            memset(es, 0, sizeof(*es));
            es->client = client;
            es->active = true;
            /* stream_id is left 0; the router assigns it during the
             * first onion send and we store it afterwards. */
            return es;
        }
    }
    return NULL;
}

static trellis_exit_stream_t *trellis_exit_stream_find(trellis_client_t *client,
                                                       uint32_t stream_id)
{
    for (int i = 0; i < TRELLIS_EXIT_STREAM_MAX; i++) {
        if (client->exit_streams[i].active &&
            client->exit_streams[i].stream_id == stream_id)
            return &client->exit_streams[i];
    }
    return NULL;
}

trellis_err_t trellis_client_connect_exit_ex(trellis_client_t *client,
                                              const char *host, uint16_t port,
                                              trellis_exit_connect_cb on_connect,
                                              trellis_exit_data_cb on_data,
                                              trellis_exit_close_cb on_close,
                                              void *ctx,
                                              trellis_exit_stream_t **stream_out)
{
    if (!client || !host || port == 0)
        return TRELLIS_ERR_INVALID_ARG;
    if (!client->running || !client->router)
        return TRELLIS_ERR_NOT_INITIALIZED;

    trellis_exit_stream_t *es = exit_stream_alloc(client);
    if (!es)
        return TRELLIS_ERR_NOMEM;

    es->on_connect = on_connect;
    es->on_data    = on_data;
    es->on_close   = on_close;
    es->ctx        = ctx;

    // Select an exit-capable peer.
    trellis_peer_info_t peers[TRELLIS_DHT_K];
    size_t n = trellis_dht_get_peers(client->dht, peers, TRELLIS_DHT_K);

    bool found_exit = false;
    for (size_t i = 0; i < n; i++) {
        if (peers[i].exit_capable && !peers[i].quarantined) {
            es->exit_fp = peers[i].fingerprint;
            found_exit = true;
            break;
        }
    }

    if (!found_exit) {
        es->active = false;
        return TRELLIS_ERR_NOT_FOUND;
    }

    // Generate PoW token.
    uint8_t pow_token[EXIT_POW_TOKEN_LEN];
    trellis_err_t pow_err = exit_pow_generate(TRELLIS_EXIT_POW_DIFFICULTY,
                                              pow_token);
    if (pow_err != TRELLIS_OK) {
        es->active = false;
        return pow_err;
    }

    // Build payload: [cmd(1)][host_len(1)][host][port_be(2)][pow_token]
    size_t host_len = strlen(host);
    if (host_len > 255) { es->active = false; return TRELLIS_ERR_INVALID_ARG; }

    size_t payload_len = 1 + 1 + host_len + 2 + EXIT_POW_TOKEN_LEN;
    uint8_t *payload = malloc(payload_len);
    if (!payload) { es->active = false; return TRELLIS_ERR_NOMEM; }

    size_t off = 0;
    payload[off++] = TRELLIS_EXIT_CMD_OPEN;
    payload[off++] = (uint8_t)host_len;
    memcpy(payload + off, host, host_len); off += host_len;
    payload[off++] = (uint8_t)(port >> 8);
    payload[off++] = (uint8_t)(port & 0xFF);
    memcpy(payload + off, pow_token, EXIT_POW_TOKEN_LEN);

    uint32_t assigned_sid = 0;
    trellis_err_t err = trellis_router_send_ex(client->router, &es->exit_fp,
                                               payload, payload_len,
                                               0, &assigned_sid,
                                               NULL, NULL);
    free(payload);

    if (err != TRELLIS_OK) {
        es->active = false;
        return err;
    }

    es->stream_id = assigned_sid;

    if (stream_out)
        *stream_out = es;

    if (on_connect)
        on_connect(es, TRELLIS_OK, ctx);

    return TRELLIS_OK;
}

trellis_err_t trellis_exit_stream_write(trellis_exit_stream_t *stream,
                                         const uint8_t *data, size_t len)
{
    if (!stream || !stream->active || !stream->client)
        return TRELLIS_ERR_INVALID_ARG;
    if (stream->stream_id == 0)
        return TRELLIS_ERR_NOT_INITIALIZED;

    trellis_client_t *client = stream->client;
    if (!client->router)
        return TRELLIS_ERR_NOT_INITIALIZED;

    // Frame: [TRELLIS_EXIT_CMD_DATA][raw data...]
    size_t frame_len = 1 + len;
    uint8_t *frame = malloc(frame_len);
    if (!frame)
        return TRELLIS_ERR_NOMEM;

    frame[0] = TRELLIS_EXIT_CMD_DATA;
    memcpy(frame + 1, data, len);

    trellis_err_t err = trellis_router_send_ex(client->router, &stream->exit_fp,
                                               frame, frame_len,
                                               stream->stream_id, NULL,
                                               NULL, NULL);
    free(frame);
    return err;
}

trellis_err_t trellis_exit_stream_close(trellis_exit_stream_t *stream)
{
    if (!stream || !stream->active)
        return TRELLIS_ERR_INVALID_ARG;

    /* Send CLOSE command to the exit relay so it tears down the clearnet
     * socket.  Best-effort: don't fail the close if the send fails. */
    if (stream->stream_id != 0 && stream->client && stream->client->router) {
        uint8_t close_frame = TRELLIS_EXIT_CMD_CLOSE;
        trellis_router_send_ex(stream->client->router, &stream->exit_fp,
                               &close_frame, 1,
                               stream->stream_id, NULL,
                               NULL, NULL);
    }

    if (stream->on_close)
        stream->on_close(stream, stream->ctx);

    stream->active = false;
    return TRELLIS_OK;
}

// --- Discovery ---
trellis_err_t trellis_client_discover_gardens(trellis_client_t *client)
{
    if (!client)
        return TRELLIS_ERR_INVALID_ARG;

    if (!client->running)
        return TRELLIS_ERR_NOT_INITIALIZED;

    trellis_message_t msg = {0};
    msg.version = TRELLIS_PROTOCOL_VERSION;
    msg.type = TRELLIS_MSG_DISCOVER;
    msg.sequence = client->next_sequence++;
    msg.timestamp = trellis_now_ms();
    msg.source = client->identity.fingerprint;

    trellis_err_t err = trellis_message_sign(&msg, &client->identity);
    if (err != TRELLIS_OK)
        return err;

    trellis_buf_t wire = {0};
    err = trellis_message_serialize(&msg, &wire);
    if (err != TRELLIS_OK)
        return err;

    err = trellis_gossip_publish(client->gossip, wire.data, wire.len);
    trellis_buf_free(&wire);
    return err;
}

trellis_err_t trellis_client_lookup(trellis_client_t *client,
                                     const trellis_fingerprint_t *target)
{
    if (!client || !target)
        return TRELLIS_ERR_INVALID_ARG;

    if (!client->running)
        return TRELLIS_ERR_NOT_INITIALIZED;

    return trellis_dht_find_node(client->dht, target, NULL, NULL);
}

// --- Message batch queue (temporal metadata defence) ---
static void on_batch_timer(uv_timer_t *handle)
{
    trellis_client_t *client = handle->data;
    trellis_client_batch_flush(client);
}

trellis_err_t trellis_client_batch_enqueue(trellis_client_t *client,
                                            const trellis_buf_t *wire)
{
    if (!client || !wire || !wire->data)
        return TRELLIS_ERR_INVALID_ARG;

    if (client->msg_batch_delay_ms == 0)
        return TRELLIS_ERR_NOT_INITIALIZED; /* batching disabled */

    // Grow queue if needed.
    if (client->batch_count >= client->batch_cap) {
        size_t new_cap = client->batch_cap ? client->batch_cap * 2 : 16;
        trellis_buf_t *new_queue = realloc(client->batch_queue,
                                           new_cap * sizeof(trellis_buf_t));
        if (!new_queue)
            return TRELLIS_ERR_NOMEM;
        client->batch_queue = new_queue;
        client->batch_cap   = new_cap;
    }

    trellis_buf_t copy = trellis_buf_from(wire->data, wire->len);
    if (!copy.data)
        return TRELLIS_ERR_NOMEM;

    client->batch_queue[client->batch_count++] = copy;

    // Start batch timer on first enqueue.
    if (!client->batch_timer_active) {
        uv_timer_init(&client->loop, &client->batch_timer);
        client->batch_timer.data = client;
        client->batch_timer_active = true;
        uv_timer_start(&client->batch_timer, on_batch_timer,
                       client->msg_batch_delay_ms, 0);
    }

    // Flush immediately if max batch size reached.
    if (client->msg_batch_max > 0 &&
        client->batch_count >= (size_t)client->msg_batch_max)
        trellis_client_batch_flush(client);

    return TRELLIS_OK;
}

void trellis_client_batch_flush(trellis_client_t *client)
{
    if (!client || client->batch_count == 0)
        return;

    if (client->batch_timer_active) {
        uv_timer_stop(&client->batch_timer);
        client->batch_timer_active = false;
    }

    /* Fisher-Yates shuffle: destroy temporal ordering so observers cannot
     * correlate message submission time with wire-send order. */
    for (size_t i = client->batch_count - 1; i > 0; i--) {
        uint32_t j = randombytes_uniform((uint32_t)(i + 1));
        trellis_buf_t tmp = client->batch_queue[i];
        client->batch_queue[i] = client->batch_queue[j];
        client->batch_queue[j] = tmp;
    }

    for (size_t i = 0; i < client->batch_count; i++) {
        queue_broadcast(client, &client->batch_queue[i]);
        trellis_buf_free(&client->batch_queue[i]);
    }
    client->batch_count = 0;
}

// --- Subsystem accessors for relay/operator tooling ---
uv_loop_t *trellis_client_get_loop(trellis_client_t *client)
{
    return client ? &client->loop : NULL;
}

trellis_dht_t *trellis_client_get_dht(trellis_client_t *client)
{
    return client ? client->dht : NULL;
}

trellis_router_t *trellis_client_get_router(trellis_client_t *client)
{
    return client ? client->router : NULL;
}

trellis_incentive_t *trellis_client_get_incentive(trellis_client_t *client)
{
    return client ? client->incentive : NULL;
}

trellis_err_t trellis_client_enable_exit_relay(trellis_client_t *client)
{
    if (!client)
        return TRELLIS_ERR_INVALID_ARG;
    if (client->exit_relay)
        return TRELLIS_OK;

    client->exit_relay = trellis_exit_relay_new(&client->loop);
    if (!client->exit_relay)
        return TRELLIS_ERR_NOMEM;

    client->exit_capable = true;
    return TRELLIS_OK;
}

trellis_exit_relay_t *trellis_client_get_exit_relay(trellis_client_t *client)
{
    return client ? client->exit_relay : NULL;
}

trellis_err_t trellis_client_set_canopy(trellis_client_t *client,
                                        struct trellis_canopy *canopy)
{
    if (!client)
        return TRELLIS_ERR_INVALID_ARG;
    client->canopy = canopy;
    return TRELLIS_OK;
}

trellis_err_t trellis_client_set_bridge(trellis_client_t *client,
                                        struct trellis_bridge *bridge)
{
    if (!client)
        return TRELLIS_ERR_INVALID_ARG;
    client->bridge = bridge;
    return TRELLIS_OK;
}
