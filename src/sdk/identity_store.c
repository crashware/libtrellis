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

#include <stdio.h>
#include <errno.h>
#include "../platform.h"

#define VINE_IDENTITY_MAGIC "VINE"
#define VINE_IDENTITY_MAGIC_LEN 4
#define VINE_IDENTITY_VERSION 1

trellis_err_t trellis_identity_save(const trellis_identity_t *id,
                                    const char *path)
{
    if (!id || !path)
        return TRELLIS_ERR_INVALID_ARG;

    // Ensure parent directory exists.
    char *dir = strdup(path);
    if (!dir)
        return TRELLIS_ERR_NOMEM;
    char *slash = strrchr(dir, '/');
    if (slash) {
        *slash = '\0';
        trellis_mkdir(dir, 0700);
    }
    free(dir);

    /* Create the file with mode 0600 atomically using open(2) so that the
     * private key material is never visible to other users, even briefly.
     * fopen("wb") would create the file at the process umask before chmod
     * could restrict it — a TOCTOU window that exposes the key. */
    int fd = trellis_open_secure(path, 0600);
    if (fd < 0)
        return TRELLIS_ERR_IO;
    FILE *f = fdopen(fd, "wb");
    if (!f) {
        close(fd);
        return TRELLIS_ERR_IO;
    }

    uint8_t header[VINE_IDENTITY_MAGIC_LEN + 2];
    memcpy(header, VINE_IDENTITY_MAGIC, VINE_IDENTITY_MAGIC_LEN);
    header[VINE_IDENTITY_MAGIC_LEN] = (VINE_IDENTITY_VERSION >> 8) & 0xff;
    header[VINE_IDENTITY_MAGIC_LEN + 1] = VINE_IDENTITY_VERSION & 0xff;

    if (fwrite(header, 1, sizeof(header), f) != sizeof(header))
        goto fail;

    // Write all key material in fixed order.
    if (fwrite(id->ed25519_pk, 1, TRELLIS_ED25519_PK_LEN, f) != TRELLIS_ED25519_PK_LEN) goto fail;
    if (fwrite(id->ed25519_sk, 1, TRELLIS_ED25519_SK_LEN, f) != TRELLIS_ED25519_SK_LEN) goto fail;
    if (fwrite(id->ml_dsa_pk, 1, TRELLIS_ML_DSA_87_PK_LEN, f) != TRELLIS_ML_DSA_87_PK_LEN) goto fail;
    if (fwrite(id->ml_dsa_sk, 1, TRELLIS_ML_DSA_87_SK_LEN, f) != TRELLIS_ML_DSA_87_SK_LEN) goto fail;
    if (fwrite(id->x25519_pk, 1, TRELLIS_X25519_PK_LEN, f) != TRELLIS_X25519_PK_LEN) goto fail;
    if (fwrite(id->x25519_sk, 1, TRELLIS_X25519_SK_LEN, f) != TRELLIS_X25519_SK_LEN) goto fail;
    if (fwrite(id->ml_kem_pk, 1, TRELLIS_ML_KEM_1024_PK_LEN, f) != TRELLIS_ML_KEM_1024_PK_LEN) goto fail;
    if (fwrite(id->ml_kem_sk, 1, TRELLIS_ML_KEM_1024_SK_LEN, f) != TRELLIS_ML_KEM_1024_SK_LEN) goto fail;
    if (fwrite(id->slh_dsa_pk, 1, TRELLIS_SLH_DSA_PK_LEN, f) != TRELLIS_SLH_DSA_PK_LEN) goto fail;
    if (fwrite(id->slh_dsa_sk, 1, TRELLIS_SLH_DSA_SK_LEN, f) != TRELLIS_SLH_DSA_SK_LEN) goto fail;
    if (fwrite(id->fingerprint.bytes, 1, TRELLIS_FINGERPRINT_LEN, f) != TRELLIS_FINGERPRINT_LEN) goto fail;

    fclose(f);
    return TRELLIS_OK;

fail:
    fclose(f);
    // Remove any partial file to avoid leaving corrupt key material on disk.
    remove(path);
    return TRELLIS_ERR_IO;
}

trellis_err_t trellis_identity_load(const char *path,
                                    trellis_identity_t *id)
{
    if (!path || !id)
        return TRELLIS_ERR_INVALID_ARG;

    FILE *f = fopen(path, "rb");
    if (!f)
        return TRELLIS_ERR_NOT_FOUND;

    uint8_t header[VINE_IDENTITY_MAGIC_LEN + 2];
    if (fread(header, 1, sizeof(header), f) != sizeof(header))
        goto fail;

    if (memcmp(header, VINE_IDENTITY_MAGIC, VINE_IDENTITY_MAGIC_LEN) != 0)
        goto fail;

    uint16_t version = ((uint16_t)header[VINE_IDENTITY_MAGIC_LEN] << 8) |
                       header[VINE_IDENTITY_MAGIC_LEN + 1];
    if (version != VINE_IDENTITY_VERSION)
        goto fail;

    if (fread(id->ed25519_pk, 1, TRELLIS_ED25519_PK_LEN, f) != TRELLIS_ED25519_PK_LEN) goto fail;
    if (fread(id->ed25519_sk, 1, TRELLIS_ED25519_SK_LEN, f) != TRELLIS_ED25519_SK_LEN) goto fail;
    if (fread(id->ml_dsa_pk, 1, TRELLIS_ML_DSA_87_PK_LEN, f) != TRELLIS_ML_DSA_87_PK_LEN) goto fail;
    if (fread(id->ml_dsa_sk, 1, TRELLIS_ML_DSA_87_SK_LEN, f) != TRELLIS_ML_DSA_87_SK_LEN) goto fail;
    if (fread(id->x25519_pk, 1, TRELLIS_X25519_PK_LEN, f) != TRELLIS_X25519_PK_LEN) goto fail;
    if (fread(id->x25519_sk, 1, TRELLIS_X25519_SK_LEN, f) != TRELLIS_X25519_SK_LEN) goto fail;
    if (fread(id->ml_kem_pk, 1, TRELLIS_ML_KEM_1024_PK_LEN, f) != TRELLIS_ML_KEM_1024_PK_LEN) goto fail;
    if (fread(id->ml_kem_sk, 1, TRELLIS_ML_KEM_1024_SK_LEN, f) != TRELLIS_ML_KEM_1024_SK_LEN) goto fail;
    if (fread(id->slh_dsa_pk, 1, TRELLIS_SLH_DSA_PK_LEN, f) != TRELLIS_SLH_DSA_PK_LEN) goto fail;
    if (fread(id->slh_dsa_sk, 1, TRELLIS_SLH_DSA_SK_LEN, f) != TRELLIS_SLH_DSA_SK_LEN) goto fail;
    if (fread(id->fingerprint.bytes, 1, TRELLIS_FINGERPRINT_LEN, f) != TRELLIS_FINGERPRINT_LEN) goto fail;

    fclose(f);
    return TRELLIS_OK;

fail:
    fclose(f);
    return TRELLIS_ERR_IO;
}

char *trellis_identity_default_path(void)
{
    const char *home = getenv("HOME");
    if (!home)
        home = "/tmp";

    size_t len = strlen(home) + strlen("/.vine/identity.key") + 1;
    char *path = malloc(len);
    if (!path)
        return NULL;
    snprintf(path, len, "%s/.vine/identity.key", home);
    return path;
}
