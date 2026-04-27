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

static char **dup_string_array(const char *const *src, size_t count)
{
    if (count == 0)
        return NULL;

    char **arr = calloc(count, sizeof(*arr));
    if (!arr)
        return NULL;

    for (size_t i = 0; i < count; i++) {
        arr[i] = strdup(src[i]);
        if (!arr[i]) {
            for (size_t j = 0; j < i; j++)
                free(arr[j]);
            free(arr);
            return NULL;
        }
    }
    return arr;
}

static void free_string_array(const char **arr, size_t count)
{
    if (!arr)
        return;
    for (size_t i = 0; i < count; i++)
        free((void *)arr[i]);
    free(arr);
}

static trellis_err_t intersect_arrays(const char **a, size_t a_count,
                                      const char **b, size_t b_count,
                                      const char ***out, size_t *out_count)
{
    const char **result = calloc(a_count < b_count ? a_count : b_count,
                                 sizeof(*result));
    if (!result && (a_count > 0 && b_count > 0))
        return TRELLIS_ERR_NOMEM;

    size_t count = 0;
    for (size_t i = 0; i < a_count; i++) {
        for (size_t j = 0; j < b_count; j++) {
            if (strcmp(a[i], b[j]) == 0) {
                result[count] = strdup(a[i]);
                if (!result[count]) {
                    free_string_array(result, count);
                    return TRELLIS_ERR_NOMEM;
                }
                count++;
                break;
            }
        }
    }
    *out = result;
    *out_count = count;
    return TRELLIS_OK;
}

trellis_capabilities_t trellis_capabilities_default(void)
{
    static const char *transports[] = {"tcp", "ws", "udp"};
    static const char *morph_modes[] = {"https-mimic", "http2-mimic",
                                        "dns-tunnel", "ws-mimic", "raw"};
    static const char *media[] = {"audio/opus", "video/vp9", "video/av1"};
    static const char *features[] = {"relay", "greenhouse", "morph-bypass",
                                     "constant-rate"};

    trellis_capabilities_t caps = {0};

    caps.transports = (const char **)dup_string_array(transports, 3);
    caps.transports_count = caps.transports ? 3 : 0;

    caps.morph_modes = (const char **)dup_string_array(morph_modes, 5);
    caps.morph_modes_count = caps.morph_modes ? 5 : 0;

    caps.media = (const char **)dup_string_array(media, 3);
    caps.media_count = caps.media ? 3 : 0;

    caps.features = (const char **)dup_string_array(features, 4);
    caps.features_count = caps.features ? 4 : 0;

    caps.extensions = NULL;
    caps.extensions_count = 0;

    return caps;
}

trellis_err_t trellis_capabilities_negotiate(const trellis_capabilities_t *local,
                                              const trellis_capabilities_t *remote,
                                              trellis_capabilities_t *result)
{
    if (!local || !remote || !result)
        return TRELLIS_ERR_INVALID_ARG;

    memset(result, 0, sizeof(*result));
    trellis_err_t err;

    err = intersect_arrays(local->transports, local->transports_count,
                           remote->transports, remote->transports_count,
                           &result->transports, &result->transports_count);
    if (err != TRELLIS_OK)
        goto fail;

    err = intersect_arrays(local->morph_modes, local->morph_modes_count,
                           remote->morph_modes, remote->morph_modes_count,
                           &result->morph_modes, &result->morph_modes_count);
    if (err != TRELLIS_OK)
        goto fail;

    err = intersect_arrays(local->media, local->media_count,
                           remote->media, remote->media_count,
                           &result->media, &result->media_count);
    if (err != TRELLIS_OK)
        goto fail;

    err = intersect_arrays(local->features, local->features_count,
                           remote->features, remote->features_count,
                           &result->features, &result->features_count);
    if (err != TRELLIS_OK)
        goto fail;

    err = intersect_arrays(local->extensions, local->extensions_count,
                           remote->extensions, remote->extensions_count,
                           &result->extensions, &result->extensions_count);
    if (err != TRELLIS_OK)
        goto fail;

    return TRELLIS_OK;

fail:
    trellis_capabilities_free(result);
    return err;
}

void trellis_capabilities_free(trellis_capabilities_t *caps)
{
    if (!caps)
        return;

    free_string_array(caps->transports, caps->transports_count);
    free_string_array(caps->morph_modes, caps->morph_modes_count);
    free_string_array(caps->media, caps->media_count);
    free_string_array(caps->features, caps->features_count);
    free_string_array(caps->extensions, caps->extensions_count);
    memset(caps, 0, sizeof(*caps));
}

/*
 * Wire format for TRELLIS_MSG_CAPABILITIES payload:
 *   [4] section_count (LE uint32)
 *   For each section:
 *     [1] section_id  (0-4)
 *     [4] string_count (LE uint32)
 *     For each string:
 *       [2] string_len (LE uint16)
 *       [string_len] UTF-8 bytes
 */
typedef struct { const char **arr; size_t count; } caps_section_t;

static trellis_err_t write_section(uint8_t **p, uint8_t *end,
                                    uint8_t section_id,
                                    const char **arr, size_t count)
{
    if (*p + 5 > end)
        return TRELLIS_ERR_NOMEM;

    **p = section_id; (*p)++;
    trellis_write_u32_be(*p, (uint32_t)count); *p += 4;

    for (size_t i = 0; i < count; i++) {
        size_t slen = strlen(arr[i]);
        if (slen > 0xFFFF)
            slen = 0xFFFF;
        if (*p + 2 + slen > end)
            return TRELLIS_ERR_NOMEM;
        trellis_write_u16_be(*p, (uint16_t)slen); *p += 2;
        memcpy(*p, arr[i], slen); *p += slen;
    }
    return TRELLIS_OK;
}

trellis_err_t trellis_capabilities_serialize(const trellis_capabilities_t *caps,
                                              trellis_buf_t *out)
{
    if (!caps || !out)
        return TRELLIS_ERR_INVALID_ARG;

    caps_section_t sections[5] = {
        { caps->transports,  caps->transports_count  },
        { caps->morph_modes, caps->morph_modes_count },
        { caps->media,       caps->media_count       },
        { caps->features,    caps->features_count    },
        { caps->extensions,  caps->extensions_count  },
    };

    // Estimate buffer size.
    size_t total = 4; /* section_count (4-byte LE u32) */
    for (int s = 0; s < 5; s++) {
        total += 5; /* section_id + string_count */
        for (size_t i = 0; i < sections[s].count; i++)
            total += 2 + strlen(sections[s].arr[i]);
    }

    *out = trellis_buf_alloc(total);
    if (!out->data)
        return TRELLIS_ERR_NOMEM;

    uint8_t *p = out->data;
    uint8_t *end = p + total;

    trellis_write_u32_be(p, 5); p += 4; /* 5 sections */

    for (int s = 0; s < 5; s++) {
        trellis_err_t err = write_section(&p, end, (uint8_t)s,
                                          sections[s].arr, sections[s].count);
        if (err != TRELLIS_OK) {
            trellis_buf_free(out);
            return err;
        }
    }

    out->len = (size_t)(p - out->data);
    return TRELLIS_OK;
}

static trellis_err_t read_string_array(const uint8_t **p, const uint8_t *end,
                                        size_t count,
                                        const char ***arr_out, size_t *count_out)
{
    char **arr = calloc(count, sizeof(char *));
    if (!arr && count > 0)
        return TRELLIS_ERR_NOMEM;

    for (size_t i = 0; i < count; i++) {
        if (*p + 2 > end) {
            for (size_t j = 0; j < i; j++) free(arr[j]);
            free(arr);
            return TRELLIS_ERR_PROTOCOL;
        }
        uint16_t slen = trellis_read_u16_be(*p); *p += 2;
        if (*p + slen > end) {
            for (size_t j = 0; j < i; j++) free(arr[j]);
            free(arr);
            return TRELLIS_ERR_PROTOCOL;
        }
        arr[i] = malloc(slen + 1);
        if (!arr[i]) {
            for (size_t j = 0; j < i; j++) free(arr[j]);
            free(arr);
            return TRELLIS_ERR_NOMEM;
        }
        memcpy(arr[i], *p, slen);
        arr[i][slen] = '\0';
        *p += slen;
    }

    *arr_out = (const char **)arr;
    *count_out = count;
    return TRELLIS_OK;
}

trellis_err_t trellis_capabilities_deserialize(const uint8_t *data, size_t len,
                                               trellis_capabilities_t *out)
{
    if (!data || !out || len < 1)
        return TRELLIS_ERR_INVALID_ARG;

    memset(out, 0, sizeof(*out));

    const uint8_t *p = data;
    const uint8_t *end = data + len;

    if (p + 4 > end)
        return TRELLIS_ERR_PROTOCOL;
    uint32_t n_sections = trellis_read_u32_be(p); p += 4;
    if (n_sections > 64) /* sanity bound */
        return TRELLIS_ERR_PROTOCOL;

    for (uint32_t s = 0; s < n_sections; s++) {
        if (p + 5 > end)
            goto fail;

        uint8_t section_id = *p++;
        uint32_t count = trellis_read_u32_be(p); p += 4;

        const char **arr = NULL;
        size_t arr_count = 0;
        trellis_err_t err = read_string_array(&p, end, count, &arr, &arr_count);
        if (err != TRELLIS_OK)
            goto fail;

        switch (section_id) {
        case 0: out->transports  = arr; out->transports_count  = arr_count; break;
        case 1: out->morph_modes = arr; out->morph_modes_count = arr_count; break;
        case 2: out->media       = arr; out->media_count       = arr_count; break;
        case 3: out->features    = arr; out->features_count    = arr_count; break;
        case 4: out->extensions  = arr; out->extensions_count  = arr_count; break;
        default:
            free_string_array(arr, arr_count);
            break;
        }
    }

    return TRELLIS_OK;

fail:
    trellis_capabilities_free(out);
    return TRELLIS_ERR_PROTOCOL;
}

trellis_err_t trellis_capabilities_add_feature(trellis_capabilities_t *caps,
                                                const char *feature)
{
    if (!caps || !feature)
        return TRELLIS_ERR_INVALID_ARG;

    for (size_t i = 0; i < caps->features_count; i++) {
        if (caps->features[i] && strcmp(caps->features[i], feature) == 0)
            return TRELLIS_OK;
    }

    size_t new_count = caps->features_count + 1;
    const char **new_arr = realloc(caps->features, new_count * sizeof(char *));
    if (!new_arr)
        return TRELLIS_ERR_NOMEM;

    new_arr[caps->features_count] = strdup(feature);
    if (!new_arr[caps->features_count])
        return TRELLIS_ERR_NOMEM;

    caps->features = new_arr;
    caps->features_count = new_count;
    return TRELLIS_OK;
}

bool trellis_capabilities_has_feature(const trellis_capabilities_t *caps,
                                       const char *feature)
{
    if (!caps || !feature)
        return false;
    for (size_t i = 0; i < caps->features_count; i++) {
        if (caps->features[i] && strcmp(caps->features[i], feature) == 0)
            return true;
    }
    return false;
}

const char *trellis_capabilities_preferred_morph(const trellis_capabilities_t *caps)
{
    if (!caps || caps->morph_modes_count == 0)
        return NULL;
    return caps->morph_modes[0];
}

bool trellis_capabilities_has_transport(const trellis_capabilities_t *caps,
                                        const char *transport)
{
    if (!caps || !transport)
        return false;
    for (size_t i = 0; i < caps->transports_count; i++) {
        if (caps->transports[i] && strcmp(caps->transports[i], transport) == 0)
            return true;
    }
    return false;
}
