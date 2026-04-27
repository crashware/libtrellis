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

static const char *const VALID_PERMISSIONS[] = {
    "read", "write", "admin", "owner"
};
static const size_t VALID_PERMISSION_COUNT = 4;

trellis_err_t trellis_permission_validate(const char *permission)
{
    if (!permission)
        return TRELLIS_ERR_INVALID_ARG;

    for (size_t i = 0; i < VALID_PERMISSION_COUNT; i++) {
        if (strcmp(permission, VALID_PERMISSIONS[i]) == 0)
            return TRELLIS_OK;
    }
    return TRELLIS_ERR_PERMISSION;
}

trellis_err_t trellis_permission_add(trellis_garden_t *garden,
                                     const trellis_fingerprint_t *target,
                                     const char *permission)
{
    if (!garden || !target || !permission)
        return TRELLIS_ERR_INVALID_ARG;

    trellis_err_t err = trellis_permission_validate(permission);
    if (err != TRELLIS_OK)
        return err;

    for (size_t i = 0; i < garden->perm_count; i++) {
        if (trellis_fingerprint_eq(&garden->permissions[i].target, target) &&
            strcmp(garden->permissions[i].permission, permission) == 0)
            return TRELLIS_ERR_ALREADY_EXISTS;
    }

    if (garden->perm_count >= garden->perm_cap) {
        size_t new_cap = garden->perm_cap ? garden->perm_cap * 2 : TRELLIS_GARDEN_PERM_INIT_CAP;
        trellis_garden_permission_t *new_arr = realloc(garden->permissions,
                                                       new_cap * sizeof(*new_arr));
        if (!new_arr)
            return TRELLIS_ERR_NOMEM;
        garden->permissions = new_arr;
        garden->perm_cap = new_cap;
    }

    garden->permissions[garden->perm_count].target = *target;
    garden->permissions[garden->perm_count].permission = strdup(permission);
    if (!garden->permissions[garden->perm_count].permission)
        return TRELLIS_ERR_NOMEM;

    garden->perm_count++;
    return TRELLIS_OK;
}

trellis_err_t trellis_permission_remove_all(trellis_garden_t *garden,
                                            const trellis_fingerprint_t *target)
{
    if (!garden || !target)
        return TRELLIS_ERR_INVALID_ARG;

    size_t write = 0;
    for (size_t read = 0; read < garden->perm_count; read++) {
        if (trellis_fingerprint_eq(&garden->permissions[read].target, target)) {
            free(garden->permissions[read].permission);
        } else {
            if (write != read)
                garden->permissions[write] = garden->permissions[read];
            write++;
        }
    }
    garden->perm_count = write;
    return TRELLIS_OK;
}
