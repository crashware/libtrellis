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

#include <trellis/error.h>

const char *trellis_err_str(trellis_err_t err)
{
    switch (err) {
    case TRELLIS_OK:                  return "success";

    case TRELLIS_ERR_NOMEM:           return "out of memory";
    case TRELLIS_ERR_INVALID_ARG:     return "invalid argument";
    case TRELLIS_ERR_NOT_FOUND:       return "not found";
    case TRELLIS_ERR_ALREADY_EXISTS:  return "already exists";
    case TRELLIS_ERR_NOT_INITIALIZED: return "not initialized";
    case TRELLIS_ERR_TIMEOUT:         return "operation timed out";
    case TRELLIS_ERR_CANCELLED:       return "operation cancelled";
    case TRELLIS_ERR_IO:              return "I/O error";
    case TRELLIS_ERR_RATE_LIMITED:    return "rate limited";
    case TRELLIS_ERR_CONFLICT:        return "conflict";
    case TRELLIS_ERR_LIMIT_EXCEEDED:  return "limit exceeded";
    case TRELLIS_ERR_INVALID_STATE:   return "invalid state";

    case TRELLIS_ERR_CRYPTO:          return "cryptographic operation failed";
    case TRELLIS_ERR_CRYPTO_INIT:     return "cryptographic subsystem initialization failed";
    case TRELLIS_ERR_KEM_KEYGEN:      return "KEM key generation failed";
    case TRELLIS_ERR_KEM_ENCAPS:      return "KEM encapsulation failed";
    case TRELLIS_ERR_KEM_DECAPS:      return "KEM decapsulation failed";
    case TRELLIS_ERR_SIGN_KEYGEN:     return "signature key generation failed";
    case TRELLIS_ERR_SIGN_FAILED:     return "signature creation failed";
    case TRELLIS_ERR_VERIFY_FAILED:   return "signature verification failed";
    case TRELLIS_ERR_HANDSHAKE:       return "handshake failed";
    case TRELLIS_ERR_DECRYPT:         return "decryption failed";
    case TRELLIS_ERR_ENCRYPT:         return "encryption failed";
    case TRELLIS_ERR_RATCHET:         return "ratchet advance failed";

    case TRELLIS_ERR_TRANSPORT:       return "transport error";
    case TRELLIS_ERR_CONNECT:         return "connection failed";
    case TRELLIS_ERR_LISTEN:          return "listen failed";
    case TRELLIS_ERR_SEND:            return "send failed";
    case TRELLIS_ERR_CLOSED:          return "connection closed";

    case TRELLIS_ERR_DHT:             return "DHT operation failed";
    case TRELLIS_ERR_ROUTING:         return "routing failed";
    case TRELLIS_ERR_GOSSIP:          return "gossip protocol error";

    case TRELLIS_ERR_MORPH:           return "morph layer error";
    case TRELLIS_ERR_DISGUISE:        return "traffic disguise failed";
    case TRELLIS_ERR_DECODE:          return "morph decode failed";
    case TRELLIS_ERR_FULL:            return "queue or buffer full";
    case TRELLIS_ERR_OVERFLOW:        return "payload exceeds constant-rate frame size";

    case TRELLIS_ERR_PROTOCOL:        return "protocol error";
    case TRELLIS_ERR_GARDEN:          return "garden operation failed";
    case TRELLIS_ERR_PERMISSION:      return "permission denied";
    case TRELLIS_ERR_EXTENSION:       return "extension error";
    case TRELLIS_ERR_MSG_FORMAT:      return "message format invalid";

    case TRELLIS_ERR_GREENHOUSE:      return "greenhouse error";
    case TRELLIS_ERR_DESCRIPTOR:      return "descriptor error";
    case TRELLIS_ERR_RENDEZVOUS:      return "rendezvous failed";
    case TRELLIS_ERR_ONION:           return "onion routing error";
    case TRELLIS_ERR_INTRO:           return "introduction point error";
    case TRELLIS_ERR_EXPIRED:         return "descriptor or credential has expired";

    case TRELLIS_ERR_SYBIL:           return "sybil resistance error";
    case TRELLIS_ERR_VDF_INVALID:     return "VDF proof invalid";
    case TRELLIS_ERR_VDF_TOO_YOUNG:   return "VDF proof too young for relay duty";
    case TRELLIS_ERR_TRUST:           return "trust graph error";
    case TRELLIS_ERR_VOUCH_INVALID:   return "vouch attestation invalid";
    case TRELLIS_ERR_QUARANTINED:     return "peer is quarantined";
    case TRELLIS_ERR_BEHAVIOR:        return "behavioral scoring error";

    case TRELLIS_ERR_GROVE:           return "grove error";
    case TRELLIS_ERR_GROVE_UNKNOWN:   return "grove not found in route table";
    case TRELLIS_ERR_GROVE_POLICY:    return "operation blocked by grove policy";
    case TRELLIS_ERR_BRIDGE:          return "bridge error";
    case TRELLIS_ERR_CANOPY:          return "canopy routing error";
    case TRELLIS_ERR_CANOPY_LOOP:     return "loop detected in canopy announcement";
    case TRELLIS_ERR_CANOPY_EXPIRED:  return "canopy announcement or route expired";
    }

    return "unknown error";
}
