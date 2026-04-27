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
#include <stdio.h>

trellis_client_config_t trellis_client_config_default(void)
{
    trellis_client_config_t cfg = {0};
    cfg.seed_addr = NULL;
    cfg.listen_addr = "0.0.0.0:0";
    cfg.transport_mode = TRELLIS_TRANSPORT_RAW;
    cfg.default_routing = TRELLIS_ROUTE_AUTO;
    cfg.morph_metamorphic = true;
    cfg.cover_traffic_ms = 5000;   /* 5 s default; set 0 to disable */
    cfg.pt_binary = NULL;
    cfg.pt_args = NULL;
    /* Default wire camouflage: raw-obfs for raw/PT; callers using TLS
     * transport should set this to TRELLIS_CAMO_NONE. */
    cfg.wire_camouflage_type = TRELLIS_CAMO_RAW_OBFS;
    /* TLS certificate verification is enabled by default.  Callers using
     * domain fronting must explicitly set verify_peer = false (or provide
     * no ca_pem and accept VERIFY_OPTIONAL). */
    cfg.verify_peer = true;

    // Temporal metadata countermeasures.
    cfg.always_on_daemon          = false;
    cfg.decoy_circuit_interval_ms = 30000;  /* 30 s between decoy circuits */
    cfg.msg_batch_delay_ms        = 200;    /* 200 ms batch window */
    cfg.msg_batch_max             = 32;     /* flush after 32 queued frames */

    // Per-connection PoW: 0 = use TRELLIS_CONN_POW_DEFAULT_ITERS.
    cfg.conn_pow_iterations = 0;

    // Anonymity hardening (Tor parity): guard pinning + fixed-size cells.
    cfg.guard_enabled      = true;
    cfg.guard_persist_path = NULL;
    cfg.morph_cell_mode    = true;
    cfg.morph_cell_size    = 512;

    return cfg;
}

/*
 * On each tick, we generate a decoy onion lookup to a random "tombstone"
 * greenhouse address.  The address is constructed from a random fingerprint
 * so it is unlikely to be a real service, but observers watching the network
 * see a stream of lookups indistinguishable from real service requests.
 */
static void decoy_circuit_tick(uv_timer_t *handle)
{
    trellis_client_t *client = handle->data;
    if (!client || !client->running)
        return;

    trellis_fingerprint_t dummy_fp;
    randombytes_buf(dummy_fp.bytes, TRELLIS_FINGERPRINT_LEN);

    /* Phase 1: DHT lookup for a non-existent descriptor — produces FIND_VALUE
     * traffic indistinguishable from a genuine greenhouse connection attempt. */
    uint8_t desc_key[32];
    if (trellis_bloom_addr_descriptor_key(&dummy_fp, desc_key) == TRELLIS_OK) {
        trellis_dht_find_value(client->dht, desc_key, sizeof(desc_key),
                               NULL, NULL);
    }

    /* Phase 2: Build a dummy 3-hop onion circuit carrying random-sized random
     * payload (64–512 bytes).  We temporarily force ONION mode on the router
     * so the circuit construction is identical to real traffic.  This runs on
     * the UV event-loop thread so no concurrent access to router->mode. */
    if (client->router) {
        trellis_routing_mode_t saved = trellis_router_mode(client->router);

        uint32_t payload_len = 64 + randombytes_uniform(512 - 64 + 1);
        uint8_t *payload = malloc(payload_len);
        if (payload) {
            randombytes_buf(payload, payload_len);

            trellis_router_set_mode(client->router, TRELLIS_ROUTE_ONION);
            trellis_router_send(client->router, &dummy_fp,
                                payload, payload_len, NULL, NULL);
            trellis_router_set_mode(client->router, saved);

            free(payload);
        }
    }

    fprintf(stderr, "[DECOY] decoy circuit sent toward %02x%02x...\n",
            dummy_fp.bytes[0], dummy_fp.bytes[1]);
}

trellis_err_t trellis_client_start_decoy_timer(trellis_client_t *client)
{
    if (!client || client->decoy_interval_ms == 0)
        return TRELLIS_OK;

    if (client->decoy_timer_active)
        return TRELLIS_OK;

    uv_timer_init(&client->loop, &client->decoy_timer);
    client->decoy_timer.data = client;
    uv_timer_start(&client->decoy_timer, decoy_circuit_tick,
                   client->decoy_interval_ms, client->decoy_interval_ms);
    client->decoy_timer_active = true;
    return TRELLIS_OK;
}

void trellis_client_stop_decoy_timer(trellis_client_t *client)
{
    if (!client || !client->decoy_timer_active)
        return;
    uv_timer_stop(&client->decoy_timer);
    client->decoy_timer_active = false;
}
