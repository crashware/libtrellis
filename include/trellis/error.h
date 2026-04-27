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

#ifndef TRELLIS_ERROR_H
#define TRELLIS_ERROR_H

#ifdef __cplusplus
extern "C" {
#endif

typedef enum trellis_err {
    TRELLIS_OK                  = 0,

    // Generic
    TRELLIS_ERR_NOMEM           = -1,
    TRELLIS_ERR_INVALID_ARG     = -2,
    TRELLIS_ERR_NOT_FOUND       = -3,
    TRELLIS_ERR_ALREADY_EXISTS  = -4,
    TRELLIS_ERR_NOT_INITIALIZED = -5,
    TRELLIS_ERR_TIMEOUT         = -6,
    TRELLIS_ERR_CANCELLED       = -7,
    TRELLIS_ERR_IO              = -8,
    TRELLIS_ERR_RATE_LIMITED    = -9,
    TRELLIS_ERR_CONFLICT        = -10,
    TRELLIS_ERR_LIMIT_EXCEEDED  = -11,
    TRELLIS_ERR_INVALID_STATE   = -12,

    // Crypto (Layer 1)
    TRELLIS_ERR_CRYPTO          = -99,
    TRELLIS_ERR_CRYPTO_INIT     = -100,
    TRELLIS_ERR_KEM_KEYGEN      = -101,
    TRELLIS_ERR_KEM_ENCAPS      = -102,
    TRELLIS_ERR_KEM_DECAPS      = -103,
    TRELLIS_ERR_SIGN_KEYGEN     = -104,
    TRELLIS_ERR_SIGN_FAILED     = -105,
    TRELLIS_ERR_VERIFY_FAILED   = -106,
    TRELLIS_ERR_HANDSHAKE       = -107,
    TRELLIS_ERR_DECRYPT         = -108,
    TRELLIS_ERR_ENCRYPT         = -109,
    TRELLIS_ERR_RATCHET         = -110,

    // Transport (Layer 0)
    TRELLIS_ERR_TRANSPORT       = -200,
    TRELLIS_ERR_CONNECT         = -201,
    TRELLIS_ERR_LISTEN          = -202,
    TRELLIS_ERR_SEND            = -203,
    TRELLIS_ERR_CLOSED          = -204,

    // Mesh (Layer 2)
    TRELLIS_ERR_DHT             = -300,
    TRELLIS_ERR_ROUTING         = -301,
    TRELLIS_ERR_GOSSIP          = -302,

    // Morph (Layer 3)
    TRELLIS_ERR_MORPH           = -400,
    TRELLIS_ERR_DISGUISE        = -401,
    TRELLIS_ERR_DECODE          = -402,
    TRELLIS_ERR_FULL            = -403,   /* queue/buffer full */
    TRELLIS_ERR_OVERFLOW        = -404,   /* payload exceeds frame size */

    // Protocol (Layer 4)
    TRELLIS_ERR_PROTOCOL        = -500,
    TRELLIS_ERR_GARDEN          = -501,
    TRELLIS_ERR_PERMISSION      = -502,
    TRELLIS_ERR_EXTENSION       = -503,
    TRELLIS_ERR_MSG_FORMAT      = -504,

    // Greenhouse
    TRELLIS_ERR_GREENHOUSE      = -600,
    TRELLIS_ERR_DESCRIPTOR      = -601,
    TRELLIS_ERR_RENDEZVOUS      = -602,
    TRELLIS_ERR_ONION           = -603,
    TRELLIS_ERR_INTRO           = -604,
    TRELLIS_ERR_EXPIRED         = -605,

    // Sybil resistance.
    TRELLIS_ERR_SYBIL           = -700,
    TRELLIS_ERR_VDF_INVALID     = -701,
    TRELLIS_ERR_VDF_TOO_YOUNG   = -702,
    TRELLIS_ERR_TRUST           = -703,
    TRELLIS_ERR_VOUCH_INVALID   = -704,
    TRELLIS_ERR_QUARANTINED     = -705,
    TRELLIS_ERR_BEHAVIOR        = -706,

    // Rhizome (grove federation)
    TRELLIS_ERR_GROVE           = -800,
    TRELLIS_ERR_GROVE_UNKNOWN   = -801,
    TRELLIS_ERR_GROVE_POLICY    = -802,
    TRELLIS_ERR_BRIDGE          = -803,
    TRELLIS_ERR_CANOPY          = -804,
    TRELLIS_ERR_CANOPY_LOOP     = -805,
    TRELLIS_ERR_CANOPY_EXPIRED  = -806,
} trellis_err_t;

const char *trellis_err_str(trellis_err_t err);

#ifdef __cplusplus
}
#endif

#endif /* TRELLIS_ERROR_H */
