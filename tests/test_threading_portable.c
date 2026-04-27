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
#include <stdlib.h>
#include <uv.h>

#define RUN_TEST(fn) do { printf("  %-50s", #fn "..."); fn(); printf("PASS\n"); } while(0)
#define ASSERT(cond) do { if (!(cond)) { fprintf(stderr, "FAIL at %s:%d: %s\n", __FILE__, __LINE__, #cond); abort(); } } while(0)

static void test_mutex_basic(void)
{
    uv_mutex_t mutex;
    ASSERT(uv_mutex_init(&mutex) == 0);

    uv_mutex_lock(&mutex);
    uv_mutex_unlock(&mutex);

    // trylock should succeed when unlocked.
    ASSERT(uv_mutex_trylock(&mutex) == 0);
    uv_mutex_unlock(&mutex);

    uv_mutex_destroy(&mutex);
}

#define CONTENTION_ITERS 100000

typedef struct {
    uv_mutex_t *mutex;
    int        *counter;
} contention_arg_t;

static void contention_worker(void *arg)
{
    contention_arg_t *ca = (contention_arg_t *)arg;
    for (int i = 0; i < CONTENTION_ITERS; i++) {
        uv_mutex_lock(ca->mutex);
        (*ca->counter)++;
        uv_mutex_unlock(ca->mutex);
    }
}

static void test_mutex_contention(void)
{
    uv_mutex_t mutex;
    ASSERT(uv_mutex_init(&mutex) == 0);

    int counter = 0;
    contention_arg_t arg = { .mutex = &mutex, .counter = &counter };

    uv_thread_t t1, t2;
    ASSERT(uv_thread_create(&t1, contention_worker, &arg) == 0);
    ASSERT(uv_thread_create(&t2, contention_worker, &arg) == 0);

    uv_thread_join(&t1);
    uv_thread_join(&t2);

    ASSERT(counter == CONTENTION_ITERS * 2);

    uv_mutex_destroy(&mutex);
}

typedef struct {
    int input;
    int output;
} thread_data_t;

static void data_worker(void *arg)
{
    thread_data_t *td = (thread_data_t *)arg;
    td->output = td->input * 2 + 7;
}

static void test_thread_create_join(void)
{
    thread_data_t td = { .input = 42, .output = 0 };

    uv_thread_t t;
    ASSERT(uv_thread_create(&t, data_worker, &td) == 0);
    uv_thread_join(&t);

    ASSERT(td.output == 42 * 2 + 7);
}

#define NUM_THREADS 8

static void sum_worker(void *arg)
{
    int *val = (int *)arg;
    *val = (*val) * (*val);
}

static void test_multiple_threads(void)
{
    uv_thread_t threads[NUM_THREADS];
    int values[NUM_THREADS];

    for (int i = 0; i < NUM_THREADS; i++) {
        values[i] = i + 1;
        ASSERT(uv_thread_create(&threads[i], sum_worker, &values[i]) == 0);
    }

    for (int i = 0; i < NUM_THREADS; i++)
        uv_thread_join(&threads[i]);

    for (int i = 0; i < NUM_THREADS; i++)
        ASSERT(values[i] == (i + 1) * (i + 1));
}

int main(void)
{
    printf("=== test_threading_portable ===\n");
    RUN_TEST(test_mutex_basic);
    RUN_TEST(test_mutex_contention);
    RUN_TEST(test_thread_create_join);
    RUN_TEST(test_multiple_threads);
    printf("All threading tests passed.\n");
    return 0;
}
