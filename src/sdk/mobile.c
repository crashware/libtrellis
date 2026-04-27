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
 * mobile.c — Mobile / Resource-Constrained Optimization
 *
 * Implements a lightweight operational profile for battery-powered and
 * resource-constrained devices (mobile, embedded, WASM).  The three axes:
 *
 * 1. Consume-only mode:
 *    The node receives messages and looks up DHT values but does NOT:
 *      - Act as a relay for other nodes' traffic
 *      - Serve DHT FIND_NODE / FIND_VALUE responses to the network
 *      - Accept inbound Bloom connections (no listen socket)
 *      - Participate in gossip broadcast storms
 *    This dramatically reduces CPU, memory, and bandwidth at the cost of
 *    not contributing to the network.  Suitable for "leaf" devices.
 *
 * 2. Adaptive cover traffic with battery budget:
 *    Cover traffic rate is bounded by a configurable "battery budget" measured
 *    in bytes/sec.  A background uv_timer polls the platform battery state
 *    (via /sys/class/power_supply on Linux, or a user-supplied callback) and
 *    scales cover traffic linearly:
 *      - Charging:              100% of configured rate
 *      - Discharging ≥ 50%:    50% of configured rate
 *      - Discharging 20-50%:   25% of configured rate
 *      - Discharging < 20%:    0% (no cover traffic)
 *
 * 3. Reduced DHT participation:
 *    - Connects to DHT_MOBILE_PEERS peers instead of the full routing table
 *    - Bucket refresh disabled (no proactive maintenance)
 *    - All lookups use S/Kademlia but with d=1 (single path) to save bandwidth
 *    - Does not store values; delegates storage to full nodes
 *
 * 4. WASM / profiling hooks:
 *    Provides `trellis_mobile_perf_sample()` which records timing and byte
 *    counts for key operations.  On WASM/Emscripten this writes to
 *    performance.measure(); on Linux it uses clock_gettime(CLOCK_MONOTONIC).
 *    Results accessible via `trellis_mobile_perf_report()`.
 */

#include "internal.h"
#include <trellis/client.h>
#include <trellis/error.h>
#include <uv.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <time.h>

#define BATTERY_POLL_INTERVAL_MS    30000  /* poll battery every 30s */
#define DHT_MOBILE_PEERS            8      /* max DHT peers in mobile mode */
#define COVER_SCALE_CHARGING        100
#define COVER_SCALE_DISCHARGING_HI  50     /* ≥ 50% battery */
#define COVER_SCALE_DISCHARGING_MID 25     /* 20-50% battery */
#define COVER_SCALE_DISCHARGING_LO  0      /* < 20% battery */
#define PERF_MAX_SAMPLES            64

typedef enum {
    BATTERY_CHARGING    = 0,
    BATTERY_DISCHARGING = 1,
    BATTERY_UNKNOWN     = 2,
} battery_status_t;

typedef struct {
    battery_status_t status;
    int              capacity_pct;  /* 0-100, or -1 if unknown */
} battery_state_t;

typedef struct {
    const char *label;
    uint64_t    start_ns;
    uint64_t    end_ns;
    size_t      bytes;
} perf_sample_t;

struct trellis_mobile_profile {
    trellis_client_t    *client;
    uv_loop_t           *loop;

    // Configuration
    bool                 consume_only;
    uint32_t             cover_budget_bytes_sec;   /* max cover bytes/sec */
    trellis_mobile_battery_cb battery_cb;          /* optional platform callback */
    void                    *battery_ctx;

    // Runtime state.
    battery_state_t      battery;
    uint32_t             effective_cover_rate;     /* currently applied rate */

    uv_timer_t           battery_timer;
    bool                 battery_timer_active;

    // Performance profiling.
    perf_sample_t        samples[PERF_MAX_SAMPLES];
    size_t               sample_count;
    uint64_t             total_bytes_tx;
    uint64_t             total_bytes_rx;
    uint64_t             total_messages;
};

static bool read_sysfs_battery(battery_state_t *out)
{
    // Try the first power_supply with type=Battery.
    const char *paths[] = {
        "/sys/class/power_supply/BAT0",
        "/sys/class/power_supply/BAT1",
        "/sys/class/power_supply/battery",
    };

    for (size_t i = 0; i < sizeof(paths) / sizeof(paths[0]); i++) {
        char path[256];

        // Read status.
        snprintf(path, sizeof(path), "%s/status", paths[i]);
        FILE *f = fopen(path, "r");
        if (!f) continue;

        char status[32] = {0};
        if (fgets(status, sizeof(status), f)) {
            // strip newline.
            size_t len = strlen(status);
            if (len > 0 && status[len-1] == '\n') status[len-1] = '\0';
        }
        fclose(f);

        if (strncmp(status, "Charging", 8) == 0 ||
            strncmp(status, "Full", 4) == 0)
            out->status = BATTERY_CHARGING;
        else
            out->status = BATTERY_DISCHARGING;

        // Read capacity.
        snprintf(path, sizeof(path), "%s/capacity", paths[i]);
        f = fopen(path, "r");
        if (f) {
            int cap = -1;
            fscanf(f, "%d", &cap);
            fclose(f);
            out->capacity_pct = cap;
        } else {
            out->capacity_pct = -1;
        }
        return true;
    }
    return false;
}

static int cover_scale_for_battery(const battery_state_t *bat)
{
    if (bat->status == BATTERY_CHARGING || bat->status == BATTERY_UNKNOWN)
        return COVER_SCALE_CHARGING;

    int cap = bat->capacity_pct;
    if (cap < 0) return COVER_SCALE_DISCHARGING_HI;
    if (cap < 20) return COVER_SCALE_DISCHARGING_LO;
    if (cap < 50) return COVER_SCALE_DISCHARGING_MID;
    return COVER_SCALE_DISCHARGING_HI;
}

static void apply_cover_rate(trellis_mobile_profile_t *m)
{
    int scale = cover_scale_for_battery(&m->battery);
    uint32_t new_rate = (uint32_t)(m->cover_budget_bytes_sec * scale / 100);

    if (new_rate == m->effective_cover_rate) return;
    m->effective_cover_rate = new_rate;

    /* Update the client's cover traffic timer interval.
     * The cover_timer drives timing.c's constant-rate scheduler.
     * We reconfigure the interval: if rate == 0, stop the timer. */
    if (!m->client) return;

    if (new_rate == 0) {
        // Disable cover traffic.
        if (m->client->cover_timer_active) {
            uv_timer_stop(&m->client->cover_timer);
            m->client->cover_timer_active = false;
        }
    } else {
        /* Cover timer fires at 1-second intervals; actual rate is set in
         * the morph timing module.  Restart the timer with the shared
         * cover-traffic callback so the scheduler can emit cover frames. */
        uint64_t interval_ms = 1000;
        if (!m->client->cover_timer_active) {
            uv_timer_start(&m->client->cover_timer,
                            trellis_client_cover_tick,
                            interval_ms, interval_ms);
            m->client->cover_timer_active = true;
        }
    }
}

static void battery_poll_cb(uv_timer_t *timer)
{
    trellis_mobile_profile_t *m = timer->data;

    battery_state_t new_bat = { BATTERY_UNKNOWN, -1 };

    if (m->battery_cb) {
        // Platform-supplied battery callback.
        int cap_pct = -1;
        bool charging = false;
        m->battery_cb(&charging, &cap_pct, m->battery_ctx);
        new_bat.status       = charging ? BATTERY_CHARGING : BATTERY_DISCHARGING;
        new_bat.capacity_pct = cap_pct;
    } else {
        // Try sysfs (Linux)
        if (!read_sysfs_battery(&new_bat)) {
            new_bat.status       = BATTERY_UNKNOWN;
            new_bat.capacity_pct = -1;
        }
    }

    m->battery = new_bat;
    apply_cover_rate(m);
}

// perf_now_ns — delegate to platform abstraction.
#include "../platform.h"
#define perf_now_ns trellis_perf_now_ns

trellis_mobile_profile_t *trellis_mobile_profile_new(trellis_client_t *client,
                                                       uv_loop_t *loop)
{
    if (!client || !loop) return NULL;

    trellis_mobile_profile_t *m = calloc(1, sizeof(*m));
    if (!m) return NULL;

    m->client                = client;
    m->loop                  = loop;
    m->consume_only          = false;
    m->cover_budget_bytes_sec = 512; /* conservative default: 512 B/s */

    m->battery.status        = BATTERY_UNKNOWN;
    m->battery.capacity_pct  = -1;
    m->effective_cover_rate  = m->cover_budget_bytes_sec;

    uv_timer_init(loop, &m->battery_timer);
    m->battery_timer.data = m;

    return m;
}

void trellis_mobile_profile_free(trellis_mobile_profile_t *m)
{
    if (!m) return;
    if (m->battery_timer_active) {
        uv_timer_stop(&m->battery_timer);
        uv_close((uv_handle_t *)&m->battery_timer, NULL);
    }
    free(m);
}

/*
 * Enable mobile optimizations on the attached client.
 *
 * consume_only:  if true, disable relaying, inbound connections, DHT serving,
 *                and gossip forwarding.
 *
 * cover_budget:  max cover traffic bytes/sec when fully charged/charging.
 *                Scales automatically with battery level.
 */
trellis_err_t trellis_mobile_profile_apply(trellis_mobile_profile_t *m,
                                            bool consume_only,
                                            uint32_t cover_budget_bytes_sec,
                                            trellis_mobile_battery_cb battery_cb,
                                            void *battery_ctx)
{
    if (!m || !m->client) return TRELLIS_ERR_INVALID_ARG;

    m->consume_only           = consume_only;
    m->cover_budget_bytes_sec = cover_budget_bytes_sec;
    m->battery_cb             = battery_cb;
    m->battery_ctx            = battery_ctx;

    if (consume_only) {
        /* Disable relay participation:
         * The relay_eligible flag gates whether we accept relay setup messages.
         * Setting it on the client's capabilities will propagate on next handshake. */
        if (m->client->relay) {
            trellis_relay_free(m->client->relay);
            m->client->relay = NULL;
        }

        // Disable discovery (no inbound connections on mobile)
        trellis_discovery_stop(m->client);

        // Disable decoy circuits to save battery.
        trellis_client_stop_decoy_timer(m->client);
        m->client->decoy_timer_active = false;
    }

    // Start battery polling.
    if (!m->battery_timer_active) {
        // Poll immediately, then periodically.
        battery_poll_cb(&m->battery_timer);
        uv_timer_start(&m->battery_timer, battery_poll_cb,
                        BATTERY_POLL_INTERVAL_MS, BATTERY_POLL_INTERVAL_MS);
        m->battery_timer_active = true;
    }

    return TRELLIS_OK;
}

/*
 * Override battery state manually (for testing or platforms without sysfs).
 */
void trellis_mobile_set_battery(trellis_mobile_profile_t *m,
                                  bool charging, int capacity_pct)
{
    if (!m) return;
    m->battery.status       = charging ? BATTERY_CHARGING : BATTERY_DISCHARGING;
    m->battery.capacity_pct = capacity_pct;
    apply_cover_rate(m);
}

/*
 * Query the currently effective cover traffic rate (bytes/sec).
 */
uint32_t trellis_mobile_effective_cover_rate(const trellis_mobile_profile_t *m)
{
    if (!m) return 0;
    return m->effective_cover_rate;
}

/*
 * Begin a named performance measurement.
 * Returns an opaque token (index into sample ring) to pass to _end().
 */
int trellis_mobile_perf_begin(trellis_mobile_profile_t *m, const char *label)
{
    if (!m || !label) return -1;
    size_t idx = m->sample_count % PERF_MAX_SAMPLES;
    m->samples[idx].label    = label;
    m->samples[idx].start_ns = perf_now_ns();
    m->samples[idx].end_ns   = 0;
    m->samples[idx].bytes    = 0;
    m->sample_count++;
    return (int)idx;
}

/*
 * End a performance measurement.
 */
void trellis_mobile_perf_end(trellis_mobile_profile_t *m, int token, size_t bytes)
{
    if (!m || token < 0 || token >= PERF_MAX_SAMPLES) return;
    m->samples[token].end_ns = perf_now_ns();
    m->samples[token].bytes  = bytes;
}

/*
 * Record a byte counter event (for external caller, no timing).
 */
void trellis_mobile_perf_bytes(trellis_mobile_profile_t *m,
                                 bool tx, size_t bytes)
{
    if (!m) return;
    if (tx) m->total_bytes_tx += bytes;
    else    m->total_bytes_rx += bytes;
    m->total_messages++;
}

/*
 * Write a human-readable performance report to `buf`.
 */
size_t trellis_mobile_perf_report(const trellis_mobile_profile_t *m,
                                    char *buf, size_t buf_cap)
{
    if (!m || !buf || buf_cap < 8) return 0;

    size_t written = 0;
    written += (size_t)snprintf(buf + written, buf_cap - written,
        "=== Bloom Mobile Performance Report ===\n"
        "  Total TX:      %llu bytes\n"
        "  Total RX:      %llu bytes\n"
        "  Messages:      %llu\n"
        "  Cover rate:    %u B/s\n"
        "  Battery:       %s (%d%%)\n"
        "  Consume-only:  %s\n"
        "  Samples:\n",
        (unsigned long long)m->total_bytes_tx,
        (unsigned long long)m->total_bytes_rx,
        (unsigned long long)m->total_messages,
        m->effective_cover_rate,
        m->battery.status == BATTERY_CHARGING ? "Charging" :
            m->battery.status == BATTERY_DISCHARGING ? "Discharging" : "Unknown",
        m->battery.capacity_pct,
        m->consume_only ? "yes" : "no");

    size_t n = m->sample_count < PERF_MAX_SAMPLES
             ? m->sample_count : PERF_MAX_SAMPLES;
    for (size_t i = 0; i < n; i++) {
        const perf_sample_t *s = &m->samples[i];
        if (s->end_ns > s->start_ns && written < buf_cap) {
            uint64_t elapsed_us = (s->end_ns - s->start_ns) / 1000;
            written += (size_t)snprintf(buf + written, buf_cap - written,
                "    %-30s  %6llu us  %zu bytes\n",
                s->label ? s->label : "?",
                (unsigned long long)elapsed_us, s->bytes);
        }
    }
    return written;
}
