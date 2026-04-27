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

/*
 * Adaptive padding (Section 7.2).
 *
 * Pad format: [4-byte big-endian original length][original data][random padding]
 * Total size is rounded up to the morph context's padding_target or the next
 * multiple of padding_target that can hold the data.
 *
 * Burst masking fragments large messages into irregular-sized chunks,
 * each prefixed with [4-byte big-endian chunk length].
 */
#define PAD_HEADER_LEN 4

trellis_err_t morph_pad(const trellis_morph_t *morph,
                        const uint8_t *in, size_t in_len,
                        trellis_buf_t *out)
{
    if (!morph || !in || !out)
        return TRELLIS_ERR_INVALID_ARG;

    uint32_t target = morph->ctx.padding_target;
    if (target == 0)
        target = 256;

    size_t needed = PAD_HEADER_LEN + in_len;
    size_t padded_len = target;
    while (padded_len < needed)
        padded_len += target;

    *out = trellis_buf_alloc(padded_len);
    if (!out->data)
        return TRELLIS_ERR_NOMEM;

    uint32_t len32 = (uint32_t)in_len;
    out->data[0] = (uint8_t)((len32 >> 24) & 0xFF);
    out->data[1] = (uint8_t)((len32 >> 16) & 0xFF);
    out->data[2] = (uint8_t)((len32 >> 8) & 0xFF);
    out->data[3] = (uint8_t)(len32 & 0xFF);

    memcpy(out->data + PAD_HEADER_LEN, in, in_len);

    size_t pad_start = PAD_HEADER_LEN + in_len;
    if (padded_len > pad_start)
        randombytes_buf(out->data + pad_start, padded_len - pad_start);

    out->len = padded_len;
    return TRELLIS_OK;
}

trellis_err_t morph_unpad(const uint8_t *in, size_t in_len,
                          trellis_buf_t *out)
{
    if (!in || !out)
        return TRELLIS_ERR_INVALID_ARG;

    if (in_len < PAD_HEADER_LEN)
        return TRELLIS_ERR_DECODE;

    uint32_t original_len = ((uint32_t)in[0] << 24) |
                            ((uint32_t)in[1] << 16) |
                            ((uint32_t)in[2] << 8) |
                            (uint32_t)in[3];

    if (PAD_HEADER_LEN + (size_t)original_len > in_len)
        return TRELLIS_ERR_DECODE;

    *out = trellis_buf_from(in + PAD_HEADER_LEN, original_len);
    if (!out->data && original_len > 0)
        return TRELLIS_ERR_NOMEM;

    return TRELLIS_OK;
}

trellis_err_t morph_burst_fragment(const trellis_morph_t *morph,
                                   const uint8_t *in, size_t in_len,
                                   trellis_buf_t *chunks, size_t *chunk_count,
                                   size_t max_chunks)
{
    if (!morph || !in || !chunks || !chunk_count)
        return TRELLIS_ERR_INVALID_ARG;

    uint32_t min_chunk = morph->config.burst_min_chunk;
    uint32_t max_chunk = morph->config.burst_max_chunk;
    if (min_chunk == 0) min_chunk = 64;
    if (max_chunk == 0) max_chunk = 512;
    if (min_chunk > max_chunk) min_chunk = max_chunk;

    *chunk_count = 0;
    size_t offset = 0;

    while (offset < in_len && *chunk_count < max_chunks) {
        size_t remaining = in_len - offset;
        uint32_t range = max_chunk - min_chunk + 1;
        uint32_t chunk_size = min_chunk + randombytes_uniform(range);

        if ((size_t)chunk_size > remaining)
            chunk_size = (uint32_t)remaining;

        /* If the leftover after this chunk would be smaller than min_chunk,
         * absorb it into this chunk to avoid a runt fragment. */
        size_t leftover = remaining - chunk_size;
        if (leftover > 0 && leftover < min_chunk)
            chunk_size = (uint32_t)remaining;

        trellis_buf_t *c = &chunks[*chunk_count];
        *c = trellis_buf_alloc(4 + chunk_size);
        if (!c->data)
            return TRELLIS_ERR_NOMEM;

        c->data[0] = (uint8_t)((chunk_size >> 24) & 0xFF);
        c->data[1] = (uint8_t)((chunk_size >> 16) & 0xFF);
        c->data[2] = (uint8_t)((chunk_size >> 8) & 0xFF);
        c->data[3] = (uint8_t)(chunk_size & 0xFF);
        memcpy(c->data + 4, in + offset, chunk_size);
        c->len = 4 + chunk_size;

        offset += chunk_size;
        (*chunk_count)++;
    }

    return TRELLIS_OK;
}

/*
 * Cell wire layout (each cell is exactly cell_size bytes):
 *
 *   [0]      flags       (TRELLIS_MORPH_CELL_FLAG_LAST = 0x01 on final cell)
 *   [1..3]   sequence    big-endian 24-bit cell index (0-based)
 *   [4..5]   payload_len big-endian 16-bit, bytes of real payload in this cell
 *   [6..]    payload     up to (cell_size - HEADER) bytes of plaintext
 *            padding     random bytes filling remainder
 */
trellis_err_t morph_cell_fragment(const trellis_morph_t *morph,
                                  const uint8_t *in, size_t in_len,
                                  trellis_buf_t *cells, size_t *cell_count,
                                  size_t max_cells)
{
    if (!morph || (!in && in_len > 0) || !cells || !cell_count)
        return TRELLIS_ERR_INVALID_ARG;

    uint32_t cell_size = morph->config.cell_size;
    if (cell_size < TRELLIS_MORPH_CELL_HEADER_LEN + 1)
        cell_size = TRELLIS_MORPH_CELL_SIZE_DEFAULT;

    uint32_t payload_cap = cell_size - TRELLIS_MORPH_CELL_HEADER_LEN;

    // Number of cells needed.
    size_t num_cells = (in_len == 0) ? 1 : (in_len + payload_cap - 1) / payload_cap;
    if (num_cells > max_cells)
        return TRELLIS_ERR_INVALID_ARG;

    *cell_count = 0;
    size_t offset = 0;

    for (size_t seq = 0; seq < num_cells; seq++) {
        size_t remaining = in_len - offset;
        uint16_t plen = (uint16_t)((remaining < payload_cap) ? remaining : payload_cap);
        bool last = (seq == num_cells - 1);

        trellis_buf_t *c = &cells[seq];
        *c = trellis_buf_alloc(cell_size);
        if (!c->data)
            return TRELLIS_ERR_NOMEM;

        // Header
        c->data[0] = last ? TRELLIS_MORPH_CELL_FLAG_LAST : 0;
        c->data[1] = (uint8_t)((seq >> 16) & 0xFF);
        c->data[2] = (uint8_t)((seq >> 8) & 0xFF);
        c->data[3] = (uint8_t)(seq & 0xFF);
        c->data[4] = (uint8_t)((plen >> 8) & 0xFF);
        c->data[5] = (uint8_t)(plen & 0xFF);

        // Payload
        if (plen > 0)
            memcpy(c->data + TRELLIS_MORPH_CELL_HEADER_LEN, in + offset, plen);

        // Random padding.
        size_t pad_start = TRELLIS_MORPH_CELL_HEADER_LEN + plen;
        if (cell_size > pad_start)
            randombytes_buf(c->data + pad_start, cell_size - pad_start);

        c->len = cell_size;
        offset += plen;
        (*cell_count)++;
    }

    return TRELLIS_OK;
}

trellis_err_t morph_cell_reassemble(const trellis_buf_t *cells, size_t cell_count,
                                    trellis_buf_t *out)
{
    if (!cells || !out)
        return TRELLIS_ERR_INVALID_ARG;

    if (cell_count == 0) {
        *out = trellis_buf_alloc(0);
        out->len = 0;
        return TRELLIS_OK;
    }

    // Find the maximum sequence number to size the reassembly array.
    uint32_t max_seq = 0;
    for (size_t i = 0; i < cell_count; i++) {
        if (cells[i].len < TRELLIS_MORPH_CELL_HEADER_LEN)
            return TRELLIS_ERR_DECODE;
        uint32_t seq = ((uint32_t)cells[i].data[1] << 16)
                     | ((uint32_t)cells[i].data[2] << 8)
                     |  (uint32_t)cells[i].data[3];
        if (seq > max_seq)
            max_seq = seq;
    }

    // Accumulate total payload length, ordered by sequence.
    size_t total = 0;
    bool last_seen = false;
    for (size_t i = 0; i < cell_count; i++) {
        uint16_t plen = ((uint16_t)cells[i].data[4] << 8)
                      |  (uint16_t)cells[i].data[5];
        if (TRELLIS_MORPH_CELL_HEADER_LEN + (size_t)plen > cells[i].len)
            return TRELLIS_ERR_DECODE;
        total += plen;
        if (cells[i].data[0] & TRELLIS_MORPH_CELL_FLAG_LAST)
            last_seen = true;
    }

    if (!last_seen)
        return TRELLIS_ERR_DECODE; /* incomplete message */

    *out = trellis_buf_alloc(total);
    if (!out->data && total > 0)
        return TRELLIS_ERR_NOMEM;

    // Copy payloads in sequence order (simple sort for up to ~1024 cells)
    size_t offset = 0;
    for (uint32_t seq = 0; seq <= max_seq; seq++) {
        for (size_t i = 0; i < cell_count; i++) {
            uint32_t s = ((uint32_t)cells[i].data[1] << 16)
                       | ((uint32_t)cells[i].data[2] << 8)
                       |  (uint32_t)cells[i].data[3];
            if (s != seq)
                continue;
            uint16_t plen = ((uint16_t)cells[i].data[4] << 8)
                          |  (uint16_t)cells[i].data[5];
            if (plen > 0) {
                memcpy(out->data + offset,
                       cells[i].data + TRELLIS_MORPH_CELL_HEADER_LEN,
                       plen);
                offset += plen;
            }
            break;
        }
    }

    out->len = total;
    return TRELLIS_OK;
}

trellis_err_t morph_burst_reassemble(const trellis_buf_t *chunks, size_t chunk_count,
                                     trellis_buf_t *out)
{
    if (!chunks || !out)
        return TRELLIS_ERR_INVALID_ARG;

    size_t total = 0;
    for (size_t i = 0; i < chunk_count; i++) {
        if (chunks[i].len < 4)
            return TRELLIS_ERR_DECODE;
        uint32_t clen = ((uint32_t)chunks[i].data[0] << 24) |
                        ((uint32_t)chunks[i].data[1] << 16) |
                        ((uint32_t)chunks[i].data[2] << 8) |
                        (uint32_t)chunks[i].data[3];
        if (4 + (size_t)clen > chunks[i].len)
            return TRELLIS_ERR_DECODE;
        total += clen;
    }

    *out = trellis_buf_alloc(total);
    if (!out->data && total > 0)
        return TRELLIS_ERR_NOMEM;

    size_t offset = 0;
    for (size_t i = 0; i < chunk_count; i++) {
        uint32_t clen = ((uint32_t)chunks[i].data[0] << 24) |
                        ((uint32_t)chunks[i].data[1] << 16) |
                        ((uint32_t)chunks[i].data[2] << 8) |
                        (uint32_t)chunks[i].data[3];
        memcpy(out->data + offset, chunks[i].data + 4, clen);
        offset += clen;
    }

    out->len = total;
    return TRELLIS_OK;
}
