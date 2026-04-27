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

#ifndef TRELLIS_PROBE_RESIST_H
#define TRELLIS_PROBE_RESIST_H

#include "types.h"
#include "error.h"

#ifdef __cplusplus
extern "C" {
#endif

#define TRELLIS_PROBE_HMAC_LEN      32
#define TRELLIS_PROBE_TIMESTAMP_LEN 8
#define TRELLIS_PROBE_NONCE_LEN     16
#define TRELLIS_PROBE_HEADER_LEN    (TRELLIS_PROBE_HMAC_LEN + \
                                     TRELLIS_PROBE_TIMESTAMP_LEN + \
                                     TRELLIS_PROBE_NONCE_LEN)

#define TRELLIS_PROBE_MAX_AGE_MS    60000   /* 60 seconds */
#define TRELLIS_PROBE_REPLAY_SLOTS  4096    /* time-keyed ring; entries older than MAX_AGE are evicted */

typedef struct trellis_probe_guard trellis_probe_guard_t;

trellis_probe_guard_t *trellis_probe_guard_new(const uint8_t *obfs_key,
                                               size_t key_len);

/* Create with a custom IP bucket count (default is 1024).
 * Higher values improve resilience against distributed probe attacks. */
trellis_probe_guard_t *trellis_probe_guard_new_sized(const uint8_t *obfs_key,
                                                     size_t key_len,
                                                     size_t ip_buckets);

void                   trellis_probe_guard_free(trellis_probe_guard_t *pg);

trellis_err_t trellis_probe_guard_wrap(trellis_probe_guard_t *pg,
                                       const uint8_t *msg, size_t msg_len,
                                       trellis_buf_t *out);

trellis_err_t trellis_probe_guard_validate(trellis_probe_guard_t *pg,
                                           const uint8_t *data, size_t data_len,
                                           const uint8_t **msg_out,
                                           size_t *msg_len_out);

/* Set the global (aggregate) rate limit — requests per second from all IPs.
 * Default: 500/s.  Set to 0 to disable. */
void trellis_probe_guard_set_rate_limit(trellis_probe_guard_t *pg,
                                        uint32_t max_per_second);

/* Set the per-IP rate limit — requests per second from a single source IP.
 * Default: 10/s.  Set to 0 to disable per-IP limiting. */
void trellis_probe_guard_set_per_ip_rate_limit(trellis_probe_guard_t *pg,
                                               uint32_t max_per_second);

/* Set the clock skew offset (from DHT PING/PONG measurement).
 * The probe guard adjusts timestamp validation by this offset so peers
 * with different clocks don't falsely reject each other. */
void trellis_probe_guard_set_clock_offset(trellis_probe_guard_t *pg,
                                          int64_t offset_ms);

/* Re-derive the guard's HMAC key from a new obfs_key.
 * Must be called each time the obfuscation epoch rolls over so that wrap()
 * and validate() stay in sync with the epoch the remote peer uses. */
void trellis_probe_guard_rekey(trellis_probe_guard_t *pg,
                               const uint8_t *obfs_key, size_t key_len);

/* Per-IP variant: applies a per-source-address rate limit (LRU 32-slot table)
 * before the global HMAC/replay/timestamp checks.  remote_ip may be NULL to
 * skip per-IP accounting and use only the global limiter. */
trellis_err_t trellis_probe_guard_validate_addr(trellis_probe_guard_t *pg,
                                                const uint8_t *data,
                                                size_t data_len,
                                                const char *remote_ip,
                                                const uint8_t **msg_out,
                                                size_t *msg_len_out);

#ifdef __cplusplus
}
#endif

#endif /* TRELLIS_PROBE_RESIST_H */
