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

#ifndef TRELLIS_MORPH_INTERNAL_H
#define TRELLIS_MORPH_INTERNAL_H

#include <trellis/morph.h>
#include <sodium.h>
#ifndef __EMSCRIPTEN__
#include <uv.h>
#endif

#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

#define MORPH_ROTATION_INTERVAL 10  /* frames between encoding switches; tunable */

struct trellis_morph {
    trellis_morph_config_t    config;
    trellis_morph_ctx_t       ctx;
    uint8_t                   session_key[64];
    size_t                    session_key_len;
    bool                      session_key_set;
    uint32_t                  frame_count;      /* frames encoded (sent) */
    uint32_t                  recv_frame_count; /* frames decoded (received) */
    trellis_timing_profile_t  timing_profile;
};

#ifndef __EMSCRIPTEN__

typedef struct probe_endpoint {
    char     ip[64];
    uint16_t port;
} probe_endpoint_t;

typedef struct probe_attempt probe_attempt_t; /* forward for circular ref */

struct trellis_mimicry_probe {
    uv_loop_t               *loop;
    trellis_morph_t         *morph;

    trellis_mimicry_select_cb select_cb;
    void                    *select_ctx;

    trellis_timing_profile_t  selected;    /* currently active profile */
    bool                      running;

    // Per-profile endpoint lists.
    probe_endpoint_t endpoints[TRELLIS_TIMING_PROFILE_COUNT]
                               [TRELLIS_MIMICRY_MAX_ENDPOINTS];
    size_t           ep_count[TRELLIS_TIMING_PROFILE_COUNT];

    // Tracks per-profile probe results during one probe round.
    bool    probe_ok[TRELLIS_TIMING_PROFILE_COUNT];
    size_t  probe_pending;  /* how many async probes are in flight */
    bool    probe_round_active;

    // Periodic re-probe timer.
    uv_timer_t reprobe_timer;
    bool       timer_active;

    // Failure-backoff: profiles blocked temporarily after failure.
    uint64_t blocked_until_ms[TRELLIS_TIMING_PROFILE_COUNT];
};
#endif /* !__EMSCRIPTEN__ */

trellis_err_t morph_mutate_encode(trellis_morph_t *morph,
                                  const uint8_t *in, size_t in_len,
                                  trellis_buf_t *out);
trellis_err_t morph_mutate_decode(trellis_morph_t *morph,
                                  const uint8_t *in, size_t in_len,
                                  trellis_buf_t *out);

trellis_err_t morph_pad(const trellis_morph_t *morph,
                        const uint8_t *in, size_t in_len,
                        trellis_buf_t *out);
trellis_err_t morph_unpad(const uint8_t *in, size_t in_len,
                          trellis_buf_t *out);

trellis_err_t morph_burst_fragment(const trellis_morph_t *morph,
                                   const uint8_t *in, size_t in_len,
                                   trellis_buf_t *chunks, size_t *chunk_count,
                                   size_t max_chunks);
trellis_err_t morph_burst_reassemble(const trellis_buf_t *chunks, size_t chunk_count,
                                     trellis_buf_t *out);

trellis_err_t morph_cell_fragment(const trellis_morph_t *morph,
                                  const uint8_t *in, size_t in_len,
                                  trellis_buf_t *cells, size_t *cell_count,
                                  size_t max_cells);
trellis_err_t morph_cell_reassemble(const trellis_buf_t *cells, size_t cell_count,
                                    trellis_buf_t *out);

#ifdef __cplusplus
}
#endif

#endif /* TRELLIS_MORPH_INTERNAL_H */
