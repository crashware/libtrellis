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
#include <trellis/trellis.h>

#define RUN_TEST(fn) do { printf("  %-50s", #fn "..."); fn(); printf("PASS\n"); } while(0)
#define ASSERT(cond) do { if (!(cond)) { fprintf(stderr, "FAIL at %s:%d: %s\n", __FILE__, __LINE__, #cond); abort(); } } while(0)

static void test_client_create_defaults(void)
{
    trellis_client_config_t cfg = trellis_client_config_default();
    trellis_client_t *c = trellis_client_new(&cfg);
    ASSERT(c != NULL);
    trellis_client_free(c);
}

static void test_client_create_custom_config(void)
{
    trellis_client_config_t cfg = trellis_client_config_default();
    cfg.listen_addr = "127.0.0.1:0";
    cfg.transport_mode = TRELLIS_TRANSPORT_RAW;
    cfg.default_routing = TRELLIS_ROUTE_RELAY;
    cfg.morph_metamorphic = true;
    cfg.cover_traffic_ms = 5000;

    trellis_client_t *c = trellis_client_new(&cfg);
    ASSERT(c != NULL);
    trellis_client_free(c);
}

static void test_client_start_stop(void)
{
    trellis_client_config_t cfg = trellis_client_config_default();
    cfg.listen_addr = "127.0.0.1:0";
    trellis_client_t *c = trellis_client_new(&cfg);
    ASSERT(c != NULL);

    ASSERT(trellis_client_start(c) == TRELLIS_OK);

    const trellis_identity_t *id = trellis_client_identity(c);
    ASSERT(id != NULL);

    ASSERT(trellis_client_stop(c) == TRELLIS_OK);
    trellis_client_free(c);
}

static void test_client_alias(void)
{
    trellis_client_config_t cfg = trellis_client_config_default();
    cfg.listen_addr = "127.0.0.1:0";
    trellis_client_t *c = trellis_client_new(&cfg);
    ASSERT(trellis_client_start(c) == TRELLIS_OK);

    ASSERT(trellis_client_set_alias(c, "alice") == TRELLIS_OK);
    const char *alias = trellis_client_alias(c);
    ASSERT(alias != NULL);
    ASSERT(strcmp(alias, "alice") == 0);

    ASSERT(trellis_client_set_alias(c, "bob") == TRELLIS_OK);
    ASSERT(strcmp(trellis_client_alias(c), "bob") == 0);

    trellis_client_stop(c);
    trellis_client_free(c);
}

static void test_client_garden_lifecycle(void)
{
    trellis_client_config_t cfg = trellis_client_config_default();
    cfg.listen_addr = "127.0.0.1:0";
    trellis_client_t *c = trellis_client_new(&cfg);
    ASSERT(trellis_client_start(c) == TRELLIS_OK);

    ASSERT(trellis_client_create_garden(c, "general", TRELLIS_GARDEN_PUBLIC) == TRELLIS_OK);
    ASSERT(trellis_client_join_garden(c, "general") == TRELLIS_OK);

    const uint8_t msg[] = "hello garden";
    ASSERT(trellis_client_send_message(c, "general", msg, sizeof(msg) - 1) == TRELLIS_OK);

    ASSERT(trellis_client_leave_garden(c, "general") == TRELLIS_OK);

    trellis_client_stop(c);
    trellis_client_free(c);
}

static void test_client_peer_count(void)
{
    trellis_client_config_t cfg = trellis_client_config_default();
    cfg.listen_addr = "127.0.0.1:0";
    trellis_client_t *c = trellis_client_new(&cfg);
    ASSERT(trellis_client_start(c) == TRELLIS_OK);

    ASSERT(trellis_client_peer_count(c) == 0);

    trellis_client_stop(c);
    trellis_client_free(c);
}

static void test_client_set_routing(void)
{
    trellis_client_config_t cfg = trellis_client_config_default();
    cfg.listen_addr = "127.0.0.1:0";
    trellis_client_t *c = trellis_client_new(&cfg);
    ASSERT(trellis_client_start(c) == TRELLIS_OK);

    ASSERT(trellis_client_set_routing(c, TRELLIS_ROUTE_DIRECT)    == TRELLIS_OK);
    ASSERT(trellis_client_set_routing(c, TRELLIS_ROUTE_RELAY)     == TRELLIS_OK);
    ASSERT(trellis_client_set_routing(c, TRELLIS_ROUTE_MULTIPATH) == TRELLIS_OK);
    ASSERT(trellis_client_set_routing(c, TRELLIS_ROUTE_ONION)     == TRELLIS_OK);

    trellis_client_stop(c);
    trellis_client_free(c);
}

static void test_client_set_transport_mode(void)
{
    trellis_client_config_t cfg = trellis_client_config_default();
    cfg.listen_addr = "127.0.0.1:0";
    trellis_client_t *c = trellis_client_new(&cfg);
    ASSERT(trellis_client_start(c) == TRELLIS_OK);

    ASSERT(trellis_client_set_transport_mode(c, TRELLIS_TRANSPORT_RAW) == TRELLIS_OK);
    ASSERT(trellis_client_set_transport_mode(c, TRELLIS_TRANSPORT_TLS) == TRELLIS_OK);
    ASSERT(trellis_client_set_transport_mode(c, TRELLIS_TRANSPORT_PT)  == TRELLIS_OK);

    trellis_client_stop(c);
    trellis_client_free(c);
}

static void test_client_null_safety(void)
{
    // Passing NULL config must not crash; result may be NULL or valid.
    (void)trellis_client_new(NULL);

    trellis_client_config_t cfg = trellis_client_config_default();
    cfg.listen_addr = "127.0.0.1:0";
    trellis_client_t *c = trellis_client_new(&cfg);
    ASSERT(c != NULL);

    ASSERT(trellis_client_set_alias(c, NULL) == TRELLIS_ERR_INVALID_ARG);
    ASSERT(trellis_client_create_garden(c, NULL, TRELLIS_GARDEN_PUBLIC) == TRELLIS_ERR_INVALID_ARG);
    ASSERT(trellis_client_join_garden(c, NULL) == TRELLIS_ERR_INVALID_ARG);
    ASSERT(trellis_client_leave_garden(c, NULL) == TRELLIS_ERR_INVALID_ARG);
    ASSERT(trellis_client_send_message(c, NULL, NULL, 0) == TRELLIS_ERR_INVALID_ARG);

    trellis_client_free(c);
}

static void test_client_double_stop(void)
{
    trellis_client_config_t cfg = trellis_client_config_default();
    cfg.listen_addr = "127.0.0.1:0";
    trellis_client_t *c = trellis_client_new(&cfg);
    ASSERT(trellis_client_start(c) == TRELLIS_OK);
    ASSERT(trellis_client_stop(c) == TRELLIS_OK);

    trellis_err_t err = trellis_client_stop(c);
    ASSERT(err == TRELLIS_OK || err == TRELLIS_ERR_NOT_INITIALIZED);

    trellis_client_free(c);
}

static int g_started_event;

static void on_started(const trellis_event_data_t *ev, void *ctx)
{
    (void)ev; (void)ctx;
    g_started_event = 1;
}

static void test_client_events(void)
{
    trellis_client_config_t cfg = trellis_client_config_default();
    cfg.listen_addr = "127.0.0.1:0";
    trellis_client_t *c = trellis_client_new(&cfg);

    g_started_event = 0;
    ASSERT(trellis_client_on(c, TRELLIS_EV_STARTED, on_started, NULL) == TRELLIS_OK);

    ASSERT(trellis_client_start(c) == TRELLIS_OK);
    trellis_client_stop(c);
    ASSERT(g_started_event == 1);
    trellis_client_free(c);
}

static void test_client_discover_gardens(void)
{
    trellis_client_config_t cfg = trellis_client_config_default();
    cfg.listen_addr = "127.0.0.1:0";
    trellis_client_t *c = trellis_client_new(&cfg);
    ASSERT(trellis_client_start(c) == TRELLIS_OK);

    trellis_err_t err = trellis_client_discover_gardens(c);
    ASSERT(err == TRELLIS_OK);

    trellis_client_stop(c);
    trellis_client_free(c);
}

static void test_client_garden_types(void)
{
    trellis_client_config_t cfg = trellis_client_config_default();
    cfg.listen_addr = "127.0.0.1:0";
    trellis_client_t *c = trellis_client_new(&cfg);
    ASSERT(trellis_client_start(c) == TRELLIS_OK);

    ASSERT(trellis_client_create_garden(c, "pub",  TRELLIS_GARDEN_PUBLIC)     == TRELLIS_OK);
    ASSERT(trellis_client_create_garden(c, "priv", TRELLIS_GARDEN_PRIVATE)    == TRELLIS_OK);
    ASSERT(trellis_client_create_garden(c, "eph",  TRELLIS_GARDEN_EPHEMERAL)  == TRELLIS_OK);
    ASSERT(trellis_client_create_garden(c, "per",  TRELLIS_GARDEN_PERSISTENT) == TRELLIS_OK);

    trellis_client_stop(c);
    trellis_client_free(c);
}

int main(void)
{
    printf("=== Test Suite: Client SDK ===\n");

    ASSERT(trellis_crypto_init() == TRELLIS_OK);

    RUN_TEST(test_client_create_defaults);
    RUN_TEST(test_client_create_custom_config);
    RUN_TEST(test_client_start_stop);
    RUN_TEST(test_client_alias);
    RUN_TEST(test_client_garden_lifecycle);
    RUN_TEST(test_client_peer_count);
    RUN_TEST(test_client_set_routing);
    RUN_TEST(test_client_set_transport_mode);
    RUN_TEST(test_client_null_safety);
    RUN_TEST(test_client_double_stop);
    RUN_TEST(test_client_events);
    RUN_TEST(test_client_discover_gardens);
    RUN_TEST(test_client_garden_types);

    trellis_crypto_cleanup();

    printf("=== All client tests passed ===\n");
    return 0;
}
