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

trellis_extensions_t *trellis_extensions_new(void)
{
    trellis_extensions_t *ext = calloc(1, sizeof(*ext));
    if (!ext)
        return NULL;

    ext->entries = calloc(TRELLIS_EXT_INIT_CAP, sizeof(*ext->entries));
    if (!ext->entries) {
        free(ext);
        return NULL;
    }
    ext->cap = TRELLIS_EXT_INIT_CAP;
    return ext;
}

void trellis_extensions_free(trellis_extensions_t *ext)
{
    if (!ext)
        return;

    for (size_t i = 0; i < ext->count; i++)
        free(ext->entries[i].name);

    free(ext->entries);
    free(ext);
}

trellis_err_t trellis_extensions_register(trellis_extensions_t *ext,
                                           uint16_t type,
                                           const char *name,
                                           trellis_ext_handler_fn handler,
                                           void *ctx)
{
    if (!ext || !name || !handler)
        return TRELLIS_ERR_INVALID_ARG;

    if (type < TRELLIS_EXT_MIN_TYPE)
        return TRELLIS_ERR_EXTENSION;

    for (size_t i = 0; i < ext->count; i++) {
        if (ext->entries[i].type == type)
            return TRELLIS_ERR_ALREADY_EXISTS;
    }

    if (ext->count >= ext->cap) {
        size_t new_cap = ext->cap * 2;
        trellis_ext_entry_t *new_entries = realloc(ext->entries,
                                                   new_cap * sizeof(*new_entries));
        if (!new_entries)
            return TRELLIS_ERR_NOMEM;
        ext->entries = new_entries;
        ext->cap = new_cap;
    }

    ext->entries[ext->count].type = type;
    ext->entries[ext->count].name = strdup(name);
    if (!ext->entries[ext->count].name)
        return TRELLIS_ERR_NOMEM;
    ext->entries[ext->count].handler = handler;
    ext->entries[ext->count].ctx = ctx;
    ext->count++;

    return TRELLIS_OK;
}

trellis_err_t trellis_extensions_dispatch(trellis_extensions_t *ext,
                                           const trellis_message_t *msg)
{
    if (!ext || !msg)
        return TRELLIS_ERR_INVALID_ARG;

    for (size_t i = 0; i < ext->count; i++) {
        if (ext->entries[i].type == msg->type) {
            ext->entries[i].handler(msg, ext->entries[i].ctx);
            return TRELLIS_OK;
        }
    }

    return TRELLIS_ERR_NOT_FOUND;
}
