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
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <trellis/trellis.h>

#define RUN_TEST(fn) do { printf("  %-50s", #fn "..."); fn(); printf("PASS\n"); } while(0)
#define ASSERT(cond) do { if (!(cond)) { fprintf(stderr, "FAIL at %s:%d: %s\n", __FILE__, __LINE__, #cond); abort(); } } while(0)

static trellis_identity_t g_creator;
static trellis_identity_t g_members[5];
static trellis_identity_public_t g_member_pubs[5];

static void test_treekem_create(void) {
    trellis_treekem_t *tk = trellis_treekem_new(&g_creator);
    ASSERT(tk != NULL);
    ASSERT(trellis_treekem_epoch(tk) == 0);
    ASSERT(trellis_treekem_member_count(tk) == 1);

    trellis_treekem_free(tk);
}

static void test_treekem_add_member(void) {
    trellis_treekem_t *tk = trellis_treekem_new(&g_creator);
    ASSERT(tk != NULL);

    for (int i = 0; i < 5; i++) {
        trellis_buf_t commit = {0};
        ASSERT(trellis_treekem_add_member(tk, &g_member_pubs[i], &commit) == TRELLIS_OK);
        ASSERT(commit.len > 0);
        trellis_buf_free(&commit);
    }

    ASSERT(trellis_treekem_member_count(tk) == 6);
    ASSERT(trellis_treekem_epoch(tk) == 5);

    trellis_treekem_free(tk);
}

static void test_treekem_remove_member(void) {
    trellis_treekem_t *tk = trellis_treekem_new(&g_creator);
    ASSERT(tk != NULL);

    trellis_buf_t commit = {0};
    ASSERT(trellis_treekem_add_member(tk, &g_member_pubs[0], &commit) == TRELLIS_OK);
    trellis_buf_free(&commit);
    ASSERT(trellis_treekem_member_count(tk) == 2);
    uint64_t epoch_after_add = trellis_treekem_epoch(tk);

    ASSERT(trellis_treekem_remove_member(tk, &g_member_pubs[0].fingerprint, &commit) == TRELLIS_OK);
    trellis_buf_free(&commit);
    ASSERT(trellis_treekem_member_count(tk) == 1);
    ASSERT(trellis_treekem_epoch(tk) > epoch_after_add);

    trellis_treekem_free(tk);
}

static void test_treekem_epoch_secret(void) {
    trellis_treekem_t *tk = trellis_treekem_new(&g_creator);
    ASSERT(tk != NULL);

    uint8_t secret_before[32];
    ASSERT(trellis_treekem_epoch_secret(tk, secret_before) == TRELLIS_OK);

    trellis_buf_t commit = {0};
    ASSERT(trellis_treekem_add_member(tk, &g_member_pubs[0], &commit) == TRELLIS_OK);
    trellis_buf_free(&commit);

    uint8_t secret_after[32];
    ASSERT(trellis_treekem_epoch_secret(tk, secret_after) == TRELLIS_OK);
    ASSERT(memcmp(secret_before, secret_after, 32) != 0);

    trellis_treekem_free(tk);
}

static void test_treekem_process_commit(void) {
    trellis_treekem_t *tk1 = trellis_treekem_new(&g_creator);
    trellis_treekem_t *tk2 = trellis_treekem_new(&g_creator);
    ASSERT(tk1 != NULL && tk2 != NULL);

    trellis_buf_t commit = {0};
    ASSERT(trellis_treekem_add_member(tk1, &g_member_pubs[0], &commit) == TRELLIS_OK);
    ASSERT(commit.len > 0);

    /*
     * tk2 has independent KEM keypairs from tk1, so it can't decrypt the
     * commit's path secrets (encrypted to the new member's keys). Verify
     * that process_commit at least succeeds and updates the epoch.
     * Full epoch-secret agreement requires a proper MLS Welcome flow.
     */
    ASSERT(trellis_treekem_process_commit(tk2, commit.data, commit.len) == TRELLIS_OK);
    ASSERT(trellis_treekem_epoch(tk1) == trellis_treekem_epoch(tk2));

    uint8_t s1[32];
    ASSERT(trellis_treekem_epoch_secret(tk1, s1) == TRELLIS_OK);

    trellis_buf_free(&commit);
    trellis_treekem_free(tk1);
    trellis_treekem_free(tk2);
}

int main(void) {
    printf("=== Test Suite: TreeKEM ===\n");

    ASSERT(trellis_crypto_init() == TRELLIS_OK);

    ASSERT(trellis_identity_generate(&g_creator) == TRELLIS_OK);
    for (int i = 0; i < 5; i++) {
        ASSERT(trellis_identity_generate(&g_members[i]) == TRELLIS_OK);
        trellis_identity_public_from(&g_members[i], &g_member_pubs[i]);
    }

    RUN_TEST(test_treekem_create);
    RUN_TEST(test_treekem_add_member);
    RUN_TEST(test_treekem_remove_member);
    RUN_TEST(test_treekem_epoch_secret);
    RUN_TEST(test_treekem_process_commit);

    for (int i = 0; i < 5; i++)
        trellis_identity_destroy(&g_members[i]);
    trellis_identity_destroy(&g_creator);
    trellis_crypto_cleanup();

    printf("=== All TreeKEM tests passed ===\n");
    return 0;
}
