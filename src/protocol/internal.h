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

#ifndef TRELLIS_PROTOCOL_INTERNAL_H
#define TRELLIS_PROTOCOL_INTERNAL_H

#include <trellis/protocol.h>

#include <stdlib.h>
#include <string.h>
#include <sodium.h>

static inline void trellis_write_u16_be(uint8_t *buf, uint16_t v)
{
    buf[0] = (uint8_t)(v >> 8);
    buf[1] = (uint8_t)(v);
}

static inline uint16_t trellis_read_u16_be(const uint8_t *buf)
{
    return (uint16_t)((uint16_t)buf[0] << 8 | (uint16_t)buf[1]);
}

static inline void trellis_write_u32_be(uint8_t *buf, uint32_t v)
{
    buf[0] = (uint8_t)(v >> 24);
    buf[1] = (uint8_t)(v >> 16);
    buf[2] = (uint8_t)(v >> 8);
    buf[3] = (uint8_t)(v);
}

static inline uint32_t trellis_read_u32_be(const uint8_t *buf)
{
    return (uint32_t)buf[0] << 24
         | (uint32_t)buf[1] << 16
         | (uint32_t)buf[2] << 8
         | (uint32_t)buf[3];
}

static inline void trellis_write_u64_be(uint8_t *buf, uint64_t v)
{
    buf[0] = (uint8_t)(v >> 56);
    buf[1] = (uint8_t)(v >> 48);
    buf[2] = (uint8_t)(v >> 40);
    buf[3] = (uint8_t)(v >> 32);
    buf[4] = (uint8_t)(v >> 24);
    buf[5] = (uint8_t)(v >> 16);
    buf[6] = (uint8_t)(v >> 8);
    buf[7] = (uint8_t)(v);
}

static inline uint64_t trellis_read_u64_be(const uint8_t *buf)
{
    return (uint64_t)buf[0] << 56
         | (uint64_t)buf[1] << 48
         | (uint64_t)buf[2] << 40
         | (uint64_t)buf[3] << 32
         | (uint64_t)buf[4] << 24
         | (uint64_t)buf[5] << 16
         | (uint64_t)buf[6] << 8
         | (uint64_t)buf[7];
}

#ifdef __cplusplus
extern "C" {
#endif

typedef struct trellis_garden_meta {
    char *key;
    char *value;
} trellis_garden_meta_t;

#define TRELLIS_GARDEN_HISTORY_CAP 256

typedef struct trellis_garden_history_entry {
    trellis_fingerprint_t author;
    uint8_t              *data;
    size_t                data_len;
    uint64_t              timestamp;
} trellis_garden_history_entry_t;

typedef struct trellis_garden_permission {
    trellis_fingerprint_t target;
    char                 *permission;
} trellis_garden_permission_t;

#define TRELLIS_GARDEN_MEMBERS_INIT_CAP 16
#define TRELLIS_GARDEN_META_INIT_CAP    8
#define TRELLIS_GARDEN_PERM_INIT_CAP    16

struct trellis_garden {
    char                     *name;
    trellis_garden_type_t     type;
    trellis_fingerprint_t     owner;
    trellis_fingerprint_t    *members;
    size_t                    member_count;
    size_t                    member_cap;
    trellis_treekem_t        *treekem;
    trellis_garden_meta_t    *metadata;
    size_t                    meta_count;
    size_t                    meta_cap;
    trellis_garden_permission_t *permissions;
    size_t                       perm_count;
    size_t                       perm_cap;
    trellis_garden_history_entry_t history[TRELLIS_GARDEN_HISTORY_CAP];
    size_t                    history_head;
    size_t                    history_count;
    uint64_t                  epoch;
};

typedef struct trellis_ext_entry {
    uint16_t                type;
    char                   *name;
    trellis_ext_handler_fn  handler;
    void                   *ctx;
} trellis_ext_entry_t;

#define TRELLIS_EXT_INIT_CAP 8

struct trellis_extensions {
    trellis_ext_entry_t *entries;
    size_t               count;
    size_t               cap;
};

trellis_err_t trellis_permission_validate(const char *permission);
trellis_err_t trellis_permission_add(trellis_garden_t *garden,
                                     const trellis_fingerprint_t *target,
                                     const char *permission);
trellis_err_t trellis_permission_remove_all(trellis_garden_t *garden,
                                            const trellis_fingerprint_t *target);

#ifdef __cplusplus
}
#endif

#endif /* TRELLIS_PROTOCOL_INTERNAL_H */
