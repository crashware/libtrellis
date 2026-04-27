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

/*
 * grove.c — Grove Identity, Namespace, and Membership
 *
 * A grove is a sovereign mini-mesh with its own DHT namespace, trust
 * boundary, and governance.  Each grove has a unique grove_id derived
 * from the SHA-256 hash of its genesis record.
 *
 * Key operations:
 *   - Create a grove: sign genesis, derive grove_id
 *   - Issue membership attestations (admin -> member)
 *   - Scope DHT keys via HKDF-SHAKE-256(ikm=raw_key, salt=grove_id, info="grove-ns")
 *   - Policy enforcement for peering decisions
 */

#include "internal.h"
#include <trellis/rhizome.h>
#include <sodium.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

bool trellis_grove_id_eq(const trellis_grove_id_t *a,
                         const trellis_grove_id_t *b)
{
    if (!a || !b)
        return false;
    return sodium_memcmp(a->bytes, b->bytes, TRELLIS_GROVE_ID_LEN) == 0;
}

char *trellis_grove_id_hex(const trellis_grove_id_t *id)
{
    if (!id)
        return NULL;
    char *hex = malloc(TRELLIS_GROVE_ID_LEN * 2 + 1);
    if (!hex)
        return NULL;
    sodium_bin2hex(hex, TRELLIS_GROVE_ID_LEN * 2 + 1,
                   id->bytes, TRELLIS_GROVE_ID_LEN);
    return hex;
}

static const char base32_alphabet[] = "abcdefghijklmnopqrstuvwxyz234567";

trellis_err_t trellis_grove_id_short(const trellis_grove_id_t *id,
                                     char *out, size_t out_cap)
{
    if (!id || !out || out_cap < 17)
        return TRELLIS_ERR_INVALID_ARG;

    // Encode first 10 bytes as base32 -> 16 characters.
    const uint8_t *src = id->bytes;
    size_t pos = 0;
    for (size_t i = 0; i < TRELLIS_GROVE_SHORT_ID_LEN && pos < 16; i += 5) {
        size_t remaining = TRELLIS_GROVE_SHORT_ID_LEN - i;
        uint64_t acc = 0;
        size_t bits = 0;
        for (size_t j = 0; j < 5 && j < remaining; j++) {
            acc = (acc << 8) | src[i + j];
            bits += 8;
        }
        while (bits >= 5 && pos < 16) {
            bits -= 5;
            out[pos++] = base32_alphabet[(acc >> bits) & 0x1F];
        }
        if (bits > 0 && pos < 16) {
            out[pos++] = base32_alphabet[(acc << (5 - bits)) & 0x1F];
        }
    }
    out[pos] = '\0';
    return TRELLIS_OK;
}

trellis_grove_policy_t trellis_grove_policy_default(void)
{
    trellis_grove_policy_t p;
    memset(&p, 0, sizeof(p));
    p.peer_mode = TRELLIS_GROVE_PEER_OPEN;
    p.descriptors_visible = true;
    p.gossip_relay_allowed = true;
    p.max_cross_grove_circuits = 64;
    return p;
}

// Genesis Record.

/*
 * Serialized genesis (pre-signature):
 *   "GROV" magic(4) || version(1) || name_len(1) || name(name_len) ||
 *   creator_fp(32) || created_at(8) || policy_mode(1) ||
 *   descriptors_visible(1) || gossip_relay_allowed(1) ||
 *   max_cross_grove_circuits(4) || peer_list_count(1) ||
 *   peer_list(32 * peer_list_count)
 */
#define GENESIS_MAGIC "GROV"
#define GENESIS_VERSION 1

static size_t genesis_presig_size(const trellis_grove_genesis_t *gen)
{
    size_t name_len = strlen(gen->name);
    if (name_len > TRELLIS_GROVE_NAME_MAX - 1)
        name_len = TRELLIS_GROVE_NAME_MAX - 1;
    size_t peer_count = gen->initial_policy.peer_list_count;
    if (peer_count > TRELLIS_GROVE_MAX_PEERS)
        peer_count = TRELLIS_GROVE_MAX_PEERS;
    return 4 + 1 + 1 + name_len + 32 + 8 + 1 + 1 + 1 + 4 + 1 +
           (32 * peer_count);
}

static size_t serialize_genesis_presig(const trellis_grove_genesis_t *gen,
                                       uint8_t *buf, size_t cap)
{
    size_t name_len = strlen(gen->name);
    if (name_len > TRELLIS_GROVE_NAME_MAX - 1)
        name_len = TRELLIS_GROVE_NAME_MAX - 1;

    size_t need = genesis_presig_size(gen);
    if (cap < need)
        return 0;

    size_t off = 0;
    memcpy(buf + off, GENESIS_MAGIC, 4); off += 4;
    buf[off++] = GENESIS_VERSION;
    buf[off++] = (uint8_t)name_len;
    memcpy(buf + off, gen->name, name_len); off += name_len;
    memcpy(buf + off, gen->creator_fp.bytes, 32); off += 32;

    for (int i = 0; i < 8; i++)
        buf[off++] = (uint8_t)(gen->created_at >> ((7 - i) * 8));

    buf[off++] = (uint8_t)gen->initial_policy.peer_mode;
    buf[off++] = gen->initial_policy.descriptors_visible ? 1 : 0;
    buf[off++] = gen->initial_policy.gossip_relay_allowed ? 1 : 0;

    uint32_t mcc = gen->initial_policy.max_cross_grove_circuits;
    buf[off++] = (uint8_t)(mcc >> 24);
    buf[off++] = (uint8_t)(mcc >> 16);
    buf[off++] = (uint8_t)(mcc >> 8);
    buf[off++] = (uint8_t)(mcc);

    size_t peer_count = gen->initial_policy.peer_list_count;
    if (peer_count > TRELLIS_GROVE_MAX_PEERS)
        peer_count = TRELLIS_GROVE_MAX_PEERS;
    buf[off++] = (uint8_t)peer_count;
    for (size_t i = 0; i < peer_count; i++) {
        memcpy(buf + off, gen->initial_policy.peer_list[i].bytes, 32);
        off += 32;
    }

    return off;
}

trellis_err_t trellis_grove_genesis_serialize(const trellis_grove_genesis_t *gen,
                                              trellis_buf_t *out)
{
    if (!gen || !out)
        return TRELLIS_ERR_INVALID_ARG;

    size_t presig_sz = genesis_presig_size(gen);
    size_t total = presig_sz + 32 + TRELLIS_SIGNATURE_LEN +
                   sizeof(trellis_identity_public_t);

    *out = trellis_buf_alloc(total);
    if (!out->data)
        return TRELLIS_ERR_NOMEM;

    size_t off = serialize_genesis_presig(gen, out->data, out->cap);
    if (off == 0) {
        trellis_buf_free(out);
        return TRELLIS_ERR_INVALID_ARG;
    }

    memcpy(out->data + off, gen->grove_id.bytes, 32); off += 32;

    size_t pub_len = trellis_identity_public_serialize(&gen->creator,
                                                       out->data + off,
                                                       out->cap - off);
    off += pub_len;

    memcpy(out->data + off, gen->signature.ed25519_sig,
           TRELLIS_ED25519_SIG_LEN);
    off += TRELLIS_ED25519_SIG_LEN;
    memcpy(out->data + off, gen->signature.ml_dsa_sig,
           TRELLIS_ML_DSA_87_SIG_LEN);
    off += TRELLIS_ML_DSA_87_SIG_LEN;

    out->len = off;
    return TRELLIS_OK;
}

trellis_err_t trellis_grove_genesis_deserialize(const uint8_t *data, size_t len,
                                                trellis_grove_genesis_t *gen)
{
    if (!data || !gen || len < 4 + 1 + 1 + 32 + 8 + 1 + 1 + 1 + 4 + 1)
        return TRELLIS_ERR_INVALID_ARG;

    memset(gen, 0, sizeof(*gen));

    size_t off = 0;
    if (memcmp(data + off, GENESIS_MAGIC, 4) != 0)
        return TRELLIS_ERR_MSG_FORMAT;
    off += 4;

    if (data[off++] != GENESIS_VERSION)
        return TRELLIS_ERR_MSG_FORMAT;

    uint8_t name_len = data[off++];
    if (name_len >= TRELLIS_GROVE_NAME_MAX || off + name_len > len)
        return TRELLIS_ERR_MSG_FORMAT;

    memcpy(gen->name, data + off, name_len);
    gen->name[name_len] = '\0';
    off += name_len;

    if (off + 32 > len)
        return TRELLIS_ERR_MSG_FORMAT;
    memcpy(gen->creator_fp.bytes, data + off, 32); off += 32;

    if (off + 8 > len)
        return TRELLIS_ERR_MSG_FORMAT;
    gen->created_at = 0;
    for (int i = 0; i < 8; i++)
        gen->created_at |= (uint64_t)data[off++] << ((7 - i) * 8);

    if (off + 3 > len)
        return TRELLIS_ERR_MSG_FORMAT;
    gen->initial_policy.peer_mode = (trellis_grove_peer_mode_t)data[off++];
    gen->initial_policy.descriptors_visible = data[off++] != 0;
    gen->initial_policy.gossip_relay_allowed = data[off++] != 0;

    if (off + 4 > len)
        return TRELLIS_ERR_MSG_FORMAT;
    gen->initial_policy.max_cross_grove_circuits =
        ((uint32_t)data[off] << 24) | ((uint32_t)data[off+1] << 16) |
        ((uint32_t)data[off+2] << 8) | (uint32_t)data[off+3];
    off += 4;

    if (off + 1 > len)
        return TRELLIS_ERR_MSG_FORMAT;
    gen->initial_policy.peer_list_count = data[off++];
    if (gen->initial_policy.peer_list_count > TRELLIS_GROVE_MAX_PEERS)
        return TRELLIS_ERR_MSG_FORMAT;

    if (off + 32 * gen->initial_policy.peer_list_count > len)
        return TRELLIS_ERR_MSG_FORMAT;
    for (size_t i = 0; i < gen->initial_policy.peer_list_count; i++) {
        memcpy(gen->initial_policy.peer_list[i].bytes, data + off, 32);
        off += 32;
    }

    if (off + 32 > len)
        return TRELLIS_ERR_MSG_FORMAT;
    memcpy(gen->grove_id.bytes, data + off, 32); off += 32;

    trellis_err_t err = trellis_identity_public_deserialize(
        data + off, len - off, &gen->creator);
    if (err != TRELLIS_OK)
        return err;

    size_t pub_sz = trellis_identity_public_serialize(&gen->creator, NULL, 0);
    off += pub_sz;

    if (off + TRELLIS_SIGNATURE_LEN > len)
        return TRELLIS_ERR_MSG_FORMAT;
    memcpy(gen->signature.ed25519_sig, data + off,
           TRELLIS_ED25519_SIG_LEN);
    off += TRELLIS_ED25519_SIG_LEN;
    memcpy(gen->signature.ml_dsa_sig, data + off,
           TRELLIS_ML_DSA_87_SIG_LEN);
    off += TRELLIS_ML_DSA_87_SIG_LEN;

    return TRELLIS_OK;
}

trellis_err_t trellis_grove_genesis_verify(const trellis_grove_genesis_t *gen)
{
    if (!gen)
        return TRELLIS_ERR_INVALID_ARG;

    // Verify the grove_id matches the presig content.
    uint8_t presig_buf[4096];
    size_t presig_len = serialize_genesis_presig(gen, presig_buf, sizeof(presig_buf));
    if (presig_len == 0)
        return TRELLIS_ERR_MSG_FORMAT;

    uint8_t expected_id[32];
    crypto_hash_sha256(expected_id, presig_buf, presig_len);
    if (sodium_memcmp(expected_id, gen->grove_id.bytes, 32) != 0)
        return TRELLIS_ERR_VERIFY_FAILED;

    // Verify the creator's signature over the presig content.
    return trellis_identity_verify(&gen->creator,
                                   presig_buf, presig_len,
                                   &gen->signature);
}

// Membership Attestation.

/*
 * Serialized membership (pre-signature):
 *   "GMEM" magic(4) || version(1) || grove_id(32) || member_fp(32) ||
 *   admin_fp(32) || issued_at(8) || expires_at(8) || is_bridge(1)
 */
#define MEMBERSHIP_MAGIC   "GMEM"
#define MEMBERSHIP_VERSION 1
#define MEMBERSHIP_PRESIG_LEN (4 + 1 + 32 + 32 + 32 + 8 + 8 + 1)

static void serialize_membership_presig(const trellis_grove_membership_t *mem,
                                        uint8_t buf[MEMBERSHIP_PRESIG_LEN])
{
    size_t off = 0;
    memcpy(buf + off, MEMBERSHIP_MAGIC, 4); off += 4;
    buf[off++] = MEMBERSHIP_VERSION;
    memcpy(buf + off, mem->grove_id.bytes, 32); off += 32;
    memcpy(buf + off, mem->member_fp.bytes, 32); off += 32;
    memcpy(buf + off, mem->admin_fp.bytes, 32); off += 32;

    for (int i = 0; i < 8; i++)
        buf[off++] = (uint8_t)(mem->issued_at >> ((7 - i) * 8));
    for (int i = 0; i < 8; i++)
        buf[off++] = (uint8_t)(mem->expires_at >> ((7 - i) * 8));

    buf[off++] = mem->is_bridge ? 1 : 0;
}

trellis_err_t trellis_grove_membership_serialize(
        const trellis_grove_membership_t *mem, trellis_buf_t *out)
{
    if (!mem || !out)
        return TRELLIS_ERR_INVALID_ARG;

    size_t total = MEMBERSHIP_PRESIG_LEN + TRELLIS_SIGNATURE_LEN;
    *out = trellis_buf_alloc(total);
    if (!out->data)
        return TRELLIS_ERR_NOMEM;

    serialize_membership_presig(mem, out->data);

    size_t off = MEMBERSHIP_PRESIG_LEN;
    memcpy(out->data + off, mem->signature.ed25519_sig,
           TRELLIS_ED25519_SIG_LEN);
    off += TRELLIS_ED25519_SIG_LEN;
    memcpy(out->data + off, mem->signature.ml_dsa_sig,
           TRELLIS_ML_DSA_87_SIG_LEN);
    off += TRELLIS_ML_DSA_87_SIG_LEN;

    out->len = off;
    return TRELLIS_OK;
}

trellis_err_t trellis_grove_membership_deserialize(
        const uint8_t *data, size_t len,
        trellis_grove_membership_t *mem)
{
    if (!data || !mem)
        return TRELLIS_ERR_INVALID_ARG;
    if (len < MEMBERSHIP_PRESIG_LEN + TRELLIS_SIGNATURE_LEN)
        return TRELLIS_ERR_MSG_FORMAT;

    memset(mem, 0, sizeof(*mem));

    size_t off = 0;
    if (memcmp(data + off, MEMBERSHIP_MAGIC, 4) != 0)
        return TRELLIS_ERR_MSG_FORMAT;
    off += 4;

    if (data[off++] != MEMBERSHIP_VERSION)
        return TRELLIS_ERR_MSG_FORMAT;

    memcpy(mem->grove_id.bytes, data + off, 32); off += 32;
    memcpy(mem->member_fp.bytes, data + off, 32); off += 32;
    memcpy(mem->admin_fp.bytes, data + off, 32); off += 32;

    mem->issued_at = 0;
    for (int i = 0; i < 8; i++)
        mem->issued_at |= (uint64_t)data[off++] << ((7 - i) * 8);

    mem->expires_at = 0;
    for (int i = 0; i < 8; i++)
        mem->expires_at |= (uint64_t)data[off++] << ((7 - i) * 8);

    mem->is_bridge = data[off++] != 0;

    memcpy(mem->signature.ed25519_sig, data + off,
           TRELLIS_ED25519_SIG_LEN);
    off += TRELLIS_ED25519_SIG_LEN;
    memcpy(mem->signature.ml_dsa_sig, data + off,
           TRELLIS_ML_DSA_87_SIG_LEN);

    return TRELLIS_OK;
}

trellis_err_t trellis_grove_membership_verify(
        const trellis_grove_membership_t *mem,
        const trellis_identity_public_t *admin_pub)
{
    if (!mem || !admin_pub)
        return TRELLIS_ERR_INVALID_ARG;

    // Verify admin_fp matches the public key.
    trellis_fingerprint_t computed_fp;
    trellis_err_t err = trellis_identity_fingerprint(admin_pub, &computed_fp);
    if (err != TRELLIS_OK)
        return err;

    if (!trellis_fingerprint_eq(&computed_fp, &mem->admin_fp))
        return TRELLIS_ERR_VERIFY_FAILED;

    uint8_t presig[MEMBERSHIP_PRESIG_LEN];
    serialize_membership_presig(mem, presig);

    return trellis_identity_verify(admin_pub, presig, MEMBERSHIP_PRESIG_LEN,
                                   &mem->signature);
}

// Grove Manager.
#define GROVE_MAX_MEMBERS 1024

typedef struct grove_member {
    trellis_fingerprint_t fp;
    bool                  is_bridge;
    uint64_t              expires_at;
} grove_member_t;

struct trellis_grove {
    trellis_grove_genesis_t genesis;
    trellis_grove_policy_t  policy;

    trellis_fingerprint_t   admins[TRELLIS_GROVE_MAX_ADMINS];
    size_t                  admin_count;

    grove_member_t          members[GROVE_MAX_MEMBERS];
    size_t                  member_count;
};

trellis_grove_t *trellis_grove_create(const char *name,
                                      const trellis_identity_t *creator,
                                      const trellis_grove_policy_t *policy)
{
    if (!name || !creator)
        return NULL;

    trellis_grove_t *grove = calloc(1, sizeof(*grove));
    if (!grove)
        return NULL;

    trellis_grove_genesis_t *gen = &grove->genesis;

    size_t name_len = strlen(name);
    if (name_len >= TRELLIS_GROVE_NAME_MAX)
        name_len = TRELLIS_GROVE_NAME_MAX - 1;
    memcpy(gen->name, name, name_len);
    gen->name[name_len] = '\0';

    trellis_identity_public_from(creator, &gen->creator);
    gen->creator_fp = creator->fingerprint;
    gen->created_at = trellis_now_ms();

    if (policy)
        gen->initial_policy = *policy;
    else
        gen->initial_policy = trellis_grove_policy_default();

    grove->policy = gen->initial_policy;

    // Serialize pre-signature content and derive grove_id.
    uint8_t presig_buf[4096];
    size_t presig_len = serialize_genesis_presig(gen, presig_buf,
                                                  sizeof(presig_buf));
    if (presig_len == 0) {
        free(grove);
        return NULL;
    }

    crypto_hash_sha256(gen->grove_id.bytes, presig_buf, presig_len);

    // Sign the genesis.
    trellis_err_t err = trellis_identity_sign(creator,
                                              presig_buf, presig_len,
                                              &gen->signature);
    if (err != TRELLIS_OK) {
        free(grove);
        return NULL;
    }

    // Creator is the first admin.
    grove->admins[0] = creator->fingerprint;
    grove->admin_count = 1;

    // Creator is also the first member.
    grove->members[0].fp = creator->fingerprint;
    grove->members[0].is_bridge = false;
    grove->members[0].expires_at = 0;
    grove->member_count = 1;

    return grove;
}

trellis_grove_t *trellis_grove_open(const trellis_grove_genesis_t *genesis)
{
    if (!genesis)
        return NULL;

    trellis_err_t err = trellis_grove_genesis_verify(genesis);
    if (err != TRELLIS_OK)
        return NULL;

    trellis_grove_t *grove = calloc(1, sizeof(*grove));
    if (!grove)
        return NULL;

    grove->genesis = *genesis;
    grove->policy = genesis->initial_policy;

    grove->admins[0] = genesis->creator_fp;
    grove->admin_count = 1;

    grove->members[0].fp = genesis->creator_fp;
    grove->members[0].is_bridge = false;
    grove->members[0].expires_at = 0;
    grove->member_count = 1;

    return grove;
}

void trellis_grove_free(trellis_grove_t *grove)
{
    free(grove);
}

const trellis_grove_id_t *trellis_grove_id(const trellis_grove_t *grove)
{
    return grove ? &grove->genesis.grove_id : NULL;
}

const trellis_grove_genesis_t *trellis_grove_genesis(const trellis_grove_t *grove)
{
    return grove ? &grove->genesis : NULL;
}

const trellis_grove_policy_t *trellis_grove_policy(const trellis_grove_t *grove)
{
    return grove ? &grove->policy : NULL;
}

trellis_err_t trellis_grove_set_policy(trellis_grove_t *grove,
                                       const trellis_grove_policy_t *policy)
{
    if (!grove || !policy)
        return TRELLIS_ERR_INVALID_ARG;
    grove->policy = *policy;
    return TRELLIS_OK;
}

trellis_err_t trellis_grove_issue_membership(
        trellis_grove_t *grove,
        const trellis_identity_t *admin_id,
        const trellis_fingerprint_t *member_fp,
        bool is_bridge,
        uint64_t expires_at,
        trellis_grove_membership_t *out)
{
    if (!grove || !admin_id || !member_fp || !out)
        return TRELLIS_ERR_INVALID_ARG;

    if (!trellis_grove_is_admin(grove, &admin_id->fingerprint))
        return TRELLIS_ERR_PERMISSION;

    memset(out, 0, sizeof(*out));
    out->grove_id = grove->genesis.grove_id;
    out->member_fp = *member_fp;
    out->admin_fp = admin_id->fingerprint;
    out->issued_at = trellis_now_ms();
    out->expires_at = expires_at;
    out->is_bridge = is_bridge;

    uint8_t presig[MEMBERSHIP_PRESIG_LEN];
    serialize_membership_presig(out, presig);

    return trellis_identity_sign(admin_id, presig, MEMBERSHIP_PRESIG_LEN,
                                 &out->signature);
}

trellis_err_t trellis_grove_add_member(trellis_grove_t *grove,
                                       const trellis_grove_membership_t *mem,
                                       const trellis_identity_public_t *admin_pub)
{
    if (!grove || !mem || !admin_pub)
        return TRELLIS_ERR_INVALID_ARG;

    if (!trellis_grove_id_eq(&grove->genesis.grove_id, &mem->grove_id))
        return TRELLIS_ERR_INVALID_ARG;

    trellis_err_t err = trellis_grove_membership_verify(mem, admin_pub);
    if (err != TRELLIS_OK)
        return err;

    // Check expiry.
    if (mem->expires_at > 0 && mem->expires_at < trellis_now_ms())
        return TRELLIS_ERR_EXPIRED;

    // Check duplicate.
    for (size_t i = 0; i < grove->member_count; i++) {
        if (trellis_fingerprint_eq(&grove->members[i].fp, &mem->member_fp))
            return TRELLIS_ERR_ALREADY_EXISTS;
    }

    if (grove->member_count >= GROVE_MAX_MEMBERS)
        return TRELLIS_ERR_LIMIT_EXCEEDED;

    grove_member_t *m = &grove->members[grove->member_count++];
    m->fp = mem->member_fp;
    m->is_bridge = mem->is_bridge;
    m->expires_at = mem->expires_at;

    return TRELLIS_OK;
}

bool trellis_grove_is_member(const trellis_grove_t *grove,
                             const trellis_fingerprint_t *fp)
{
    if (!grove || !fp)
        return false;

    uint64_t now = trellis_now_ms();
    for (size_t i = 0; i < grove->member_count; i++) {
        if (!trellis_fingerprint_eq(&grove->members[i].fp, fp))
            continue;
        if (grove->members[i].expires_at > 0 &&
            grove->members[i].expires_at < now)
            continue;
        return true;
    }
    return false;
}

bool trellis_grove_is_admin(const trellis_grove_t *grove,
                            const trellis_fingerprint_t *fp)
{
    if (!grove || !fp)
        return false;
    for (size_t i = 0; i < grove->admin_count; i++) {
        if (trellis_fingerprint_eq(&grove->admins[i], fp))
            return true;
    }
    return false;
}

trellis_err_t trellis_grove_add_admin(trellis_grove_t *grove,
                                      const trellis_fingerprint_t *fp)
{
    if (!grove || !fp)
        return TRELLIS_ERR_INVALID_ARG;
    if (grove->admin_count >= TRELLIS_GROVE_MAX_ADMINS)
        return TRELLIS_ERR_LIMIT_EXCEEDED;
    if (trellis_grove_is_admin(grove, fp))
        return TRELLIS_ERR_ALREADY_EXISTS;

    grove->admins[grove->admin_count++] = *fp;
    return TRELLIS_OK;
}

bool trellis_grove_peering_allowed(const trellis_grove_t *grove,
                                   const trellis_grove_id_t *remote_grove)
{
    if (!grove || !remote_grove)
        return false;

    const trellis_grove_policy_t *p = &grove->policy;

    switch (p->peer_mode) {
    case TRELLIS_GROVE_PEER_OPEN:
        return true;

    case TRELLIS_GROVE_PEER_WHITELIST:
        for (size_t i = 0; i < p->peer_list_count; i++) {
            if (trellis_grove_id_eq(&p->peer_list[i], remote_grove))
                return true;
        }
        return false;

    case TRELLIS_GROVE_PEER_BLACKLIST:
        for (size_t i = 0; i < p->peer_list_count; i++) {
            if (trellis_grove_id_eq(&p->peer_list[i], remote_grove))
                return false;
        }
        return true;

    default:
        return false;
    }
}

// Grove Join Protocol.
#define GROVE_JOIN_REQ_MAGIC  "GJRQ"
#define GROVE_JOIN_RESP_MAGIC "GJRS"
#define GROVE_JOIN_VERSION    1

static void serialize_join_request(const trellis_grove_id_t *grove_id,
                                   const trellis_fingerprint_t *requester_fp,
                                   const uint8_t *ed25519_pk,
                                   uint64_t timestamp,
                                   uint8_t *buf)
{
    size_t off = 0;
    memcpy(buf + off, GROVE_JOIN_REQ_MAGIC, 4); off += 4;
    buf[off++] = GROVE_JOIN_VERSION;
    memcpy(buf + off, grove_id->bytes, 32); off += 32;
    memcpy(buf + off, requester_fp->bytes, 32); off += 32;
    memcpy(buf + off, ed25519_pk, 32); off += 32;
    for (int i = 0; i < 8; i++)
        buf[off++] = (uint8_t)(timestamp >> ((7 - i) * 8));
}

trellis_err_t trellis_grove_join_request(trellis_dht_t *dht,
                                         const trellis_grove_id_t *grove_id,
                                         const trellis_identity_t *requester)
{
    if (!dht || !grove_id || !requester)
        return TRELLIS_ERR_INVALID_ARG;

    uint8_t buf[TRELLIS_GROVE_JOIN_REQUEST_LEN];
    uint64_t now = trellis_now_ms();

    serialize_join_request(grove_id, &requester->fingerprint,
                           requester->ed25519_pk, now, buf);

    size_t presig_len = TRELLIS_GROVE_JOIN_REQUEST_LEN - 64;
    crypto_sign_ed25519_detached(buf + presig_len, NULL,
                                 buf, presig_len,
                                 requester->ed25519_sk);

    uint8_t join_key[32];
    static const uint8_t info[] = "grove-join";
    trellis_hkdf_shake256(grove_id->bytes, TRELLIS_GROVE_ID_LEN,
                          requester->fingerprint.bytes, TRELLIS_FINGERPRINT_LEN,
                          info, sizeof(info) - 1,
                          join_key, 32);

    return trellis_dht_store(dht, join_key, 32,
                             buf, sizeof(buf), NULL, NULL);
}

// Context for poll_join_response DHT callback.
typedef struct grove_poll_ctx {
    trellis_grove_join_cb     cb;
    void                     *ctx;
    trellis_grove_id_t        grove_id;
    trellis_fingerprint_t     requester_fp;
} grove_poll_ctx_t;

static void grove_poll_find_cb(const uint8_t *value, size_t value_len,
                                trellis_err_t err, void *raw_ctx)
{
    grove_poll_ctx_t *pc = (grove_poll_ctx_t *)raw_ctx;
    if (err != TRELLIS_OK || !value || value_len < TRELLIS_GROVE_JOIN_RESPONSE_MIN_LEN) {
        if (pc->cb)
            pc->cb(err == TRELLIS_OK ? TRELLIS_ERR_NOT_FOUND : err, NULL, pc->ctx);
        free(pc);
        return;
    }

    // Validate magic and version.
    if (memcmp(value, GROVE_JOIN_RESP_MAGIC, 4) != 0 || value[4] != GROVE_JOIN_VERSION) {
        if (pc->cb)
            pc->cb(TRELLIS_ERR_MSG_FORMAT, NULL, pc->ctx);
        free(pc);
        return;
    }

    // Verify grove_id and requester_fp match our request.
    if (memcmp(value + 5, pc->grove_id.bytes, 32) != 0 ||
        memcmp(value + 37, pc->requester_fp.bytes, 32) != 0) {
        if (pc->cb)
            pc->cb(TRELLIS_ERR_MSG_FORMAT, NULL, pc->ctx);
        free(pc);
        return;
    }

    size_t off = 4 + 1 + 32 + 32;
    bool approved = value[off++] != 0;

    off += 8; /* timestamp */

    uint16_t mem_len = ((uint16_t)value[off] << 8) | (uint16_t)value[off + 1];
    off += 2;

    if (!approved) {
        if (pc->cb)
            pc->cb(TRELLIS_ERR_GROVE_POLICY, NULL, pc->ctx);
        free(pc);
        return;
    }

    if (mem_len == 0 || off + mem_len + 64 > value_len) {
        if (pc->cb)
            pc->cb(TRELLIS_ERR_MSG_FORMAT, NULL, pc->ctx);
        free(pc);
        return;
    }

    trellis_grove_membership_t mem = {0};
    trellis_err_t derr = trellis_grove_membership_deserialize(
        value + off, mem_len, &mem);
    if (derr != TRELLIS_OK) {
        if (pc->cb)
            pc->cb(derr, NULL, pc->ctx);
        free(pc);
        return;
    }

    if (!trellis_fingerprint_eq(&mem.member_fp, &pc->requester_fp)) {
        if (pc->cb)
            pc->cb(TRELLIS_ERR_VERIFY_FAILED, NULL, pc->ctx);
        free(pc);
        return;
    }

    if (pc->cb)
        pc->cb(TRELLIS_OK, &mem, pc->ctx);
    free(pc);
}

trellis_err_t trellis_grove_poll_join_response(
        trellis_dht_t *dht,
        const trellis_grove_id_t *grove_id,
        const trellis_fingerprint_t *requester_fp,
        trellis_grove_join_cb cb, void *ctx)
{
    if (!dht || !grove_id || !requester_fp)
        return TRELLIS_ERR_INVALID_ARG;

    uint8_t resp_key[32];
    static const uint8_t info[] = "grove-join-resp";
    trellis_hkdf_shake256(grove_id->bytes, TRELLIS_GROVE_ID_LEN,
                          requester_fp->bytes, TRELLIS_FINGERPRINT_LEN,
                          info, sizeof(info) - 1,
                          resp_key, 32);

    grove_poll_ctx_t *pc = malloc(sizeof(*pc));
    if (!pc)
        return TRELLIS_ERR_NOMEM;
    pc->cb = cb;
    pc->ctx = ctx;
    pc->grove_id = *grove_id;
    pc->requester_fp = *requester_fp;

    trellis_err_t err = trellis_dht_find_value(dht, resp_key, 32,
                                                grove_poll_find_cb, pc);
    if (err != TRELLIS_OK)
        free(pc);
    return err;
}

trellis_err_t trellis_grove_on_join_request(trellis_grove_t *grove,
                                             const uint8_t *data, size_t len,
                                             trellis_fingerprint_t *requester_fp_out,
                                             uint8_t *ed25519_pk_out)
{
    if (!grove || !data || !requester_fp_out)
        return TRELLIS_ERR_INVALID_ARG;
    if (len < TRELLIS_GROVE_JOIN_REQUEST_LEN)
        return TRELLIS_ERR_MSG_FORMAT;
    if (memcmp(data, GROVE_JOIN_REQ_MAGIC, 4) != 0)
        return TRELLIS_ERR_MSG_FORMAT;
    if (data[4] != GROVE_JOIN_VERSION)
        return TRELLIS_ERR_MSG_FORMAT;

    if (memcmp(data + 5, grove->genesis.grove_id.bytes, 32) != 0)
        return TRELLIS_ERR_GROVE_UNKNOWN;

    memcpy(requester_fp_out->bytes, data + 37, 32);
    const uint8_t *ed25519_pk = data + 69;

    size_t presig_len = TRELLIS_GROVE_JOIN_REQUEST_LEN - 64;
    if (crypto_sign_ed25519_verify_detached(
            data + presig_len, data, presig_len,
            ed25519_pk) != 0)
        return TRELLIS_ERR_VERIFY_FAILED;

    if (ed25519_pk_out)
        memcpy(ed25519_pk_out, ed25519_pk, 32);

    // Validate timestamp freshness (reject requests older than 5 minutes)
    uint64_t req_ts = 0;
    for (int i = 0; i < 8; i++)
        req_ts |= (uint64_t)data[101 + i] << ((7 - i) * 8);
    uint64_t now = trellis_now_ms();
    if (now > req_ts && now - req_ts > 300000)
        return TRELLIS_ERR_CANOPY_EXPIRED;
    if (req_ts > now && req_ts - now > 60000)
        return TRELLIS_ERR_CANOPY_EXPIRED;

    return TRELLIS_OK;
}

trellis_err_t trellis_grove_join_respond(trellis_grove_t *grove,
                                          trellis_dht_t *dht,
                                          const trellis_identity_t *admin_id,
                                          const trellis_fingerprint_t *requester_fp,
                                          bool approve,
                                          uint64_t expires_at)
{
    if (!grove || !dht || !admin_id || !requester_fp)
        return TRELLIS_ERR_INVALID_ARG;

    // Only admins can respond.
    if (!trellis_grove_is_admin(grove, &admin_id->fingerprint))
        return TRELLIS_ERR_GROVE_POLICY;

    trellis_grove_membership_t mem = {0};
    trellis_buf_t mem_buf = {0};

    if (approve) {
        // Issue membership attestation.
        trellis_err_t err = trellis_grove_issue_membership(
            grove, admin_id, requester_fp, false, expires_at, &mem);
        if (err != TRELLIS_OK)
            return err;

        err = trellis_grove_membership_serialize(&mem, &mem_buf);
        if (err != TRELLIS_OK)
            return err;
    }

    /* Build response:
     * "GJRS"(4) || version(1) || grove_id(32) || requester_fp(32) ||
     * approved(1) || timestamp(8) || membership_len(2) || membership(var) ||
     * ed25519_sig(64) */
    if (approve && mem_buf.len > UINT16_MAX) {
        trellis_buf_free(&mem_buf);
        return TRELLIS_ERR_LIMIT_EXCEEDED;
    }
    uint16_t mem_len = approve ? (uint16_t)mem_buf.len : 0;
    size_t resp_len = 4 + 1 + 32 + 32 + 1 + 8 + 2 + mem_len + 64;
    uint8_t *resp = calloc(1, resp_len);
    if (!resp) {
        trellis_buf_free(&mem_buf);
        return TRELLIS_ERR_NOMEM;
    }

    size_t off = 0;
    memcpy(resp + off, GROVE_JOIN_RESP_MAGIC, 4); off += 4;
    resp[off++] = GROVE_JOIN_VERSION;
    memcpy(resp + off, grove->genesis.grove_id.bytes, 32); off += 32;
    memcpy(resp + off, requester_fp->bytes, 32); off += 32;
    resp[off++] = approve ? 1 : 0;

    uint64_t now = trellis_now_ms();
    for (int i = 0; i < 8; i++)
        resp[off++] = (uint8_t)(now >> ((7 - i) * 8));

    resp[off++] = (uint8_t)(mem_len >> 8);
    resp[off++] = (uint8_t)(mem_len);

    if (approve && mem_buf.data) {
        memcpy(resp + off, mem_buf.data, mem_len);
        off += mem_len;
    }
    trellis_buf_free(&mem_buf);

    // Sign everything before the signature.
    size_t presig_len = resp_len - 64;
    crypto_sign_ed25519_detached(resp + presig_len, NULL,
                                 resp, presig_len,
                                 admin_id->ed25519_sk);

    // Store the response at a key the requester can poll.
    uint8_t resp_key[32];
    static const uint8_t info[] = "grove-join-resp";
    trellis_hkdf_shake256(grove->genesis.grove_id.bytes, TRELLIS_GROVE_ID_LEN,
                          requester_fp->bytes, TRELLIS_FINGERPRINT_LEN,
                          info, sizeof(info) - 1,
                          resp_key, 32);

    trellis_err_t err = trellis_dht_store(dht, resp_key, 32,
                                          resp, resp_len, NULL, NULL);
    free(resp);
    return err;
}

// DHT Namespace Scoping.
void trellis_grove_scope_key(const trellis_grove_id_t *grove_id,
                             const uint8_t *raw_key, size_t raw_key_len,
                             uint8_t *scoped_key_out)
{
    if (!raw_key || !scoped_key_out) {
        if (scoped_key_out)
            memset(scoped_key_out, 0, 32);
        return;
    }
    if (!grove_id) {
        // Single-mesh mode: pass through unchanged.
        if (raw_key_len >= 32)
            memcpy(scoped_key_out, raw_key, 32);
        else {
            memset(scoped_key_out, 0, 32);
            memcpy(scoped_key_out, raw_key, raw_key_len);
        }
        return;
    }

    /* HKDF-SHAKE-256(ikm=raw_key, salt=grove_id, info="grove-ns")[:32]
     * Using HKDF's extract-then-expand structure prevents concatenation
     * ambiguities across variable-length inputs and provides proper
     * domain separation through the info parameter. */
    static const uint8_t info[] = "grove-ns";
    trellis_hkdf_shake256(raw_key, raw_key_len,
                          grove_id->bytes, TRELLIS_GROVE_ID_LEN,
                          info, sizeof(info) - 1,
                          scoped_key_out, 32);
}

trellis_err_t trellis_dht_set_grove(trellis_dht_t *dht,
                                    const trellis_grove_id_t *grove_id)
{
    if (!dht)
        return TRELLIS_ERR_INVALID_ARG;

    if (grove_id) {
        if (!dht->grove_id) {
            dht->grove_id = malloc(sizeof(trellis_grove_id_t));
            if (!dht->grove_id)
                return TRELLIS_ERR_NOMEM;
        }
        memcpy(dht->grove_id, grove_id, sizeof(trellis_grove_id_t));
    } else {
        free(dht->grove_id);
        dht->grove_id = NULL;
    }

    return TRELLIS_OK;
}

const trellis_grove_id_t *trellis_dht_grove(const trellis_dht_t *dht)
{
    return dht ? dht->grove_id : NULL;
}
