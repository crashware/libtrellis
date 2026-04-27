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

#ifndef TRELLIS_CRYPTO_H
#define TRELLIS_CRYPTO_H

#include "types.h"
#include "error.h"
#include "identity.h"

#ifdef __cplusplus
extern "C" {
#endif

trellis_err_t trellis_crypto_init(void);
void          trellis_crypto_cleanup(void);

typedef struct trellis_kem_keypair {
    uint8_t x25519_pk[TRELLIS_X25519_PK_LEN];
    uint8_t x25519_sk[TRELLIS_X25519_SK_LEN];
    uint8_t ml_kem_pk[TRELLIS_ML_KEM_1024_PK_LEN];
    uint8_t ml_kem_sk[TRELLIS_ML_KEM_1024_SK_LEN];
} trellis_kem_keypair_t;

typedef struct trellis_kem_encaps_result {
    uint8_t x25519_pk[TRELLIS_X25519_PK_LEN];
    uint8_t ml_kem_ct[TRELLIS_ML_KEM_1024_CT_LEN];
    uint8_t shared_secret[TRELLIS_ML_KEM_1024_SS_LEN];
} trellis_kem_encaps_result_t;

trellis_err_t trellis_kem_keygen(trellis_kem_keypair_t *kp);
trellis_err_t trellis_kem_encaps(const uint8_t *peer_x25519_pk,
                                 const uint8_t *peer_ml_kem_pk,
                                 trellis_kem_encaps_result_t *result);
trellis_err_t trellis_kem_decaps(const trellis_kem_keypair_t *kp,
                                 const uint8_t *peer_x25519_pk,
                                 const uint8_t *ml_kem_ct,
                                 uint8_t *shared_secret);

trellis_err_t trellis_hkdf_shake256(const uint8_t *ikm, size_t ikm_len,
                                    const uint8_t *salt, size_t salt_len,
                                    const uint8_t *info, size_t info_len,
                                    uint8_t *okm, size_t okm_len);

trellis_err_t trellis_kmac256(const uint8_t *key, size_t key_len,
                              const uint8_t *data, size_t data_len,
                              const uint8_t *custom, size_t custom_len,
                              uint8_t *out, size_t out_len);

trellis_err_t trellis_shake256(const uint8_t *in, size_t in_len,
                               uint8_t *out, size_t out_len);

#define TRELLIS_AES_KEY_LEN   32
#define TRELLIS_AES_NONCE_LEN 12
#define TRELLIS_AES_TAG_LEN   16

trellis_err_t trellis_aes256gcm_encrypt(const uint8_t *key,
                                        const uint8_t *nonce,
                                        const uint8_t *plaintext, size_t pt_len,
                                        const uint8_t *ad, size_t ad_len,
                                        uint8_t *ciphertext,
                                        uint8_t *tag);
trellis_err_t trellis_aes256gcm_decrypt(const uint8_t *key,
                                        const uint8_t *nonce,
                                        const uint8_t *ciphertext, size_t ct_len,
                                        const uint8_t *ad, size_t ad_len,
                                        const uint8_t *tag,
                                        uint8_t *plaintext);

typedef enum trellis_handshake_role {
    TRELLIS_ROLE_INITIATOR = 0,
    TRELLIS_ROLE_RESPONDER = 1,
} trellis_handshake_role_t;

typedef enum trellis_handshake_state {
    TRELLIS_HS_INIT           = 0,
    TRELLIS_HS_AWAITING_MSG2  = 1,
    TRELLIS_HS_AWAITING_MSG3  = 2,
    TRELLIS_HS_COMPLETE       = 3,
} trellis_handshake_state_t;

typedef struct trellis_handshake trellis_handshake_t;

trellis_handshake_t *trellis_handshake_new(const trellis_identity_t *local_id,
                                           trellis_handshake_role_t role);
void                 trellis_handshake_free(trellis_handshake_t *hs);

trellis_err_t trellis_handshake_create_msg1(trellis_handshake_t *hs,
                                            trellis_buf_t *out);
trellis_err_t trellis_handshake_process_msg1(trellis_handshake_t *hs,
                                             const uint8_t *msg, size_t len,
                                             trellis_buf_t *out);
trellis_err_t trellis_handshake_process_msg2(trellis_handshake_t *hs,
                                             const uint8_t *msg, size_t len,
                                             trellis_buf_t *out);
trellis_err_t trellis_handshake_process_msg3(trellis_handshake_t *hs,
                                             const uint8_t *msg, size_t len);

trellis_handshake_state_t trellis_handshake_state(const trellis_handshake_t *hs);
trellis_handshake_role_t  trellis_handshake_role(const trellis_handshake_t *hs);
trellis_err_t trellis_handshake_session_key(const trellis_handshake_t *hs,
                                            uint8_t *key_out);
trellis_err_t trellis_handshake_remote_identity(const trellis_handshake_t *hs,
                                                trellis_identity_public_t *out);
trellis_err_t trellis_handshake_remote_eph_kem(const trellis_handshake_t *hs,
                                               uint8_t *x25519_pk,
                                               uint8_t *ml_kem_pk);
trellis_err_t trellis_handshake_local_eph_kem(const trellis_handshake_t *hs,
                                              trellis_kem_keypair_t *kp);

trellis_err_t trellis_elligator2_keygen(uint8_t sk[32], uint8_t pk[32],
                                        uint8_t repr[32]);
trellis_err_t trellis_elligator2_decode(const uint8_t repr[32],
                                        uint8_t pk_out[32]);
trellis_err_t trellis_elligator2_encode(const uint8_t pk[32],
                                        uint8_t repr_out[32]);

#define TRELLIS_OBFS_PAD_SIZE    32768
#define TRELLIS_OBFS_NONCE_LEN   24
#define TRELLIS_OBFS_TAG_LEN     16
#define TRELLIS_OBFS_KEY_LEN     32

trellis_err_t trellis_handshake_obfuscate(const uint8_t *msg, size_t msg_len,
                                          const uint8_t *obfs_key,
                                          trellis_buf_t *out);

trellis_err_t trellis_handshake_deobfuscate(const uint8_t *data, size_t data_len,
                                            const uint8_t *obfs_key,
                                            trellis_buf_t *out);

trellis_err_t trellis_derive_obfs_key(const uint8_t *server_pk,
                                      size_t pk_len,
                                      uint8_t *obfs_key_out);

/* Same as trellis_derive_obfs_key but with an explicit UTC hour epoch.
 * Use this when validating messages that may have been signed with the
 * previous hour's key (epoch boundary tolerance). */
trellis_err_t trellis_derive_obfs_key_epoch(const uint8_t *server_pk,
                                             size_t pk_len,
                                             uint64_t epoch,
                                             uint8_t *obfs_key_out);

#define TRELLIS_RATCHET_PQC_INTERVAL 50

typedef struct trellis_ratchet trellis_ratchet_t;

trellis_ratchet_t *trellis_ratchet_init_from_handshake(const trellis_handshake_t *hs);
trellis_ratchet_t *trellis_ratchet_init_from_handshake_ex(
    const trellis_handshake_t *hs,
    const uint8_t *domain, size_t domain_len);
void               trellis_ratchet_free(trellis_ratchet_t *r);

trellis_err_t trellis_ratchet_encrypt(trellis_ratchet_t *r,
                                      const uint8_t *plaintext, size_t pt_len,
                                      trellis_buf_t *out);
trellis_err_t trellis_ratchet_decrypt(trellis_ratchet_t *r,
                                      const uint8_t *ciphertext, size_t ct_len,
                                      trellis_buf_t *out);

typedef struct trellis_ratchet_stats {
    uint64_t messages_sent;
    uint64_t messages_received;
    uint64_t pqc_ratchet_steps;
    uint64_t sym_ratchet_steps;
} trellis_ratchet_stats_t;

trellis_ratchet_stats_t trellis_ratchet_stats(const trellis_ratchet_t *r);

typedef struct trellis_treekem trellis_treekem_t;

trellis_treekem_t *trellis_treekem_new(const trellis_identity_t *creator);
void               trellis_treekem_free(trellis_treekem_t *tk);

trellis_err_t trellis_treekem_add_member(trellis_treekem_t *tk,
                                         const trellis_identity_public_t *member,
                                         trellis_buf_t *commit_out);
trellis_err_t trellis_treekem_remove_member(trellis_treekem_t *tk,
                                            const trellis_fingerprint_t *member,
                                            trellis_buf_t *commit_out);
trellis_err_t trellis_treekem_process_commit(trellis_treekem_t *tk,
                                             const uint8_t *commit, size_t len);
trellis_err_t trellis_treekem_epoch_secret(const trellis_treekem_t *tk,
                                           uint8_t *secret_out);
uint64_t      trellis_treekem_epoch(const trellis_treekem_t *tk);
size_t        trellis_treekem_member_count(const trellis_treekem_t *tk);

// Forward declaration — mesh.h defines trellis_dht_t.
#ifndef TRELLIS_DHT_TYPEDEF_DEFINED
#define TRELLIS_DHT_TYPEDEF_DEFINED
typedef struct trellis_dht trellis_dht_t;
#endif

typedef void (*trellis_kt_verify_cb)(const trellis_fingerprint_t *fp,
                                      bool chain_valid, void *ctx);

// Serialize a new KT entry chained from prev_hash (NULL = genesis).
size_t trellis_kt_serialize_entry(const trellis_identity_t *id,
                                   uint32_t sequence,
                                   const uint8_t prev_hash[32],
                                   uint8_t *buf, size_t buf_cap);

// Verify a KT entry's signature and optional chain prev_hash.
trellis_err_t trellis_kt_verify_entry(const uint8_t *data, size_t data_len,
                                       const uint8_t *expected_prev,
                                       const uint8_t *ed25519_pk,
                                       uint8_t *entry_hash_out);

// Publish current identity keys to the DHT KT log.
trellis_err_t trellis_kt_publish(trellis_dht_t *dht,
                                  const trellis_identity_t *id);

// Record first-contact keys for TOFU tracking.
void trellis_kt_record_first_contact(const trellis_fingerprint_t *fp,
                                      const uint8_t *ed25519_pk,
                                      const uint8_t *x25519_pk,
                                      uint32_t sequence);

// Check if a peer's presented Ed25519 key matches our TOFU record.
trellis_err_t trellis_kt_check_peer(const trellis_fingerprint_t *fp,
                                     const uint8_t *ed25519_pk);

// Asynchronously verify the KT log chain from TOFU sequence to to_seq.
trellis_err_t trellis_kt_verify_chain(trellis_dht_t *dht,
                                       const trellis_fingerprint_t *fp,
                                       uint32_t to_seq,
                                       trellis_kt_verify_cb cb, void *ctx);

#ifdef __cplusplus
}
#endif

#endif /* TRELLIS_CRYPTO_H */
