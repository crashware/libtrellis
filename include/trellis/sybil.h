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

#ifndef TRELLIS_SYBIL_H
#define TRELLIS_SYBIL_H

#include "types.h"
#include "error.h"
#include "identity.h"

#ifdef __cplusplus
extern "C" {
#endif

struct trellis_peer_info;
struct trellis_gossip;

#define TRELLIS_VDF_CHECKPOINTS     16
#define TRELLIS_VDF_HASH_LEN        32
#define TRELLIS_VDF_DEFAULT_ITERS   (1U << 22) /* ~4M iterations, ~10 min */
#define TRELLIS_VDF_MIN_AGE_MS      (24ULL * 3600ULL * 1000ULL) /* 24 hours */

typedef struct trellis_vdf_proof {
    uint64_t iterations;
    uint64_t created_at;
    uint8_t  final_hash[TRELLIS_VDF_HASH_LEN];
    uint8_t  checkpoints[TRELLIS_VDF_CHECKPOINTS][TRELLIS_VDF_HASH_LEN];
    trellis_signature_t signature;
} trellis_vdf_proof_t;

trellis_err_t trellis_vdf_compute(const trellis_fingerprint_t *fp,
                                  uint64_t iterations,
                                  const trellis_identity_t *signer,
                                  trellis_vdf_proof_t *proof_out);

trellis_err_t trellis_vdf_verify(const trellis_fingerprint_t *fp,
                                 const trellis_vdf_proof_t *proof,
                                 const trellis_identity_public_t *pub);

double trellis_vdf_age_score(const trellis_vdf_proof_t *proof);

#define TRELLIS_TRUST_MAX_ENTRIES   1024
#define TRELLIS_TRUST_MAX_DEPTH     3
#define TRELLIS_TRUST_DECAY         0.6

typedef struct trellis_trust_entry {
    trellis_fingerprint_t peer;
    double   direct_trust;
    double   transitive_trust;
    double   curvature_penalty;
    uint64_t first_seen;
    uint64_t last_vouched;
    size_t   vouch_count;
    bool     occupied;
} trellis_trust_entry_t;

typedef struct trellis_vouch {
    trellis_fingerprint_t voucher;
    trellis_fingerprint_t vouchee;
    uint64_t              timestamp;
    trellis_signature_t   signature;
} trellis_vouch_t;

typedef struct trellis_trust_graph trellis_trust_graph_t;

trellis_trust_graph_t *trellis_trust_graph_new(void);
void                   trellis_trust_graph_free(trellis_trust_graph_t *tg);

trellis_err_t trellis_trust_observe(trellis_trust_graph_t *tg,
                                    const trellis_fingerprint_t *peer,
                                    double delta);

trellis_err_t trellis_trust_add_vouch(trellis_trust_graph_t *tg,
                                      const trellis_vouch_t *vouch,
                                      const trellis_identity_public_t *voucher_pub);

void trellis_trust_recompute(trellis_trust_graph_t *tg);

double trellis_trust_score(const trellis_trust_graph_t *tg,
                           const trellis_fingerprint_t *peer);

const trellis_trust_entry_t *trellis_trust_lookup(
    const trellis_trust_graph_t *tg,
    const trellis_fingerprint_t *peer);

trellis_err_t trellis_vouch_create(const trellis_identity_t *voucher_id,
                                   const trellis_fingerprint_t *vouchee,
                                   trellis_vouch_t *vouch_out);

#define TRELLIS_BEHAVIOR_MAX_RECORDS 1024
#define TRELLIS_UPTIME_BUCKETS       168  /* 7 days * 24 hours */

typedef struct trellis_behavior_record {
    trellis_fingerprint_t peer;
    uint32_t circuits_attempted;
    uint32_t circuits_completed;
    double   claimed_bandwidth;
    double   measured_bandwidth;
    uint8_t  uptime_buckets[TRELLIS_UPTIME_BUCKETS];
    uint8_t  current_bucket;
    uint8_t  addr_prefix[4];
    bool     occupied;
} trellis_behavior_record_t;

typedef struct trellis_behavior_tracker trellis_behavior_tracker_t;

trellis_behavior_tracker_t *trellis_behavior_new(void);
void                        trellis_behavior_free(trellis_behavior_tracker_t *bt);

trellis_err_t trellis_behavior_record_circuit(trellis_behavior_tracker_t *bt,
                                              const trellis_fingerprint_t *peer,
                                              bool completed);

trellis_err_t trellis_behavior_record_bandwidth(trellis_behavior_tracker_t *bt,
                                                const trellis_fingerprint_t *peer,
                                                double claimed, double measured);

trellis_err_t trellis_behavior_record_seen(trellis_behavior_tracker_t *bt,
                                           const trellis_fingerprint_t *peer,
                                           const char *addr);

void trellis_behavior_advance_bucket(trellis_behavior_tracker_t *bt);

double trellis_behavior_score(const trellis_behavior_tracker_t *bt,
                              const trellis_fingerprint_t *peer);

double trellis_behavior_uptime_correlation(const trellis_behavior_tracker_t *bt,
                                           const trellis_fingerprint_t *a,
                                           const trellis_fingerprint_t *b);

bool trellis_behavior_same_subnet(const trellis_behavior_tracker_t *bt,
                                  const trellis_fingerprint_t *a,
                                  const trellis_fingerprint_t *b);

trellis_err_t trellis_pop_verify(const trellis_fingerprint_t *peer,
                                  const uint8_t *credential, size_t cred_len);

/*
 * Generate a presentation bound to verifier_fp.  Pass the actual recipient's
 * fingerprint — using the local identity's fingerprint enables replay attacks.
 */
trellis_err_t trellis_pop_present(const trellis_identity_t *id,
                                   const trellis_fingerprint_t *verifier_fp,
                                   uint8_t **credential_out,
                                   size_t *cred_len_out);

bool          trellis_pop_has_credential(const trellis_fingerprint_t *peer);
void          trellis_pop_store_credential(const trellis_fingerprint_t *peer,
                                            const uint8_t *cred, size_t cred_len);

/*
 * Register a peer's Ed25519 public key so that holder signatures in PoP
 * credentials can be verified.  Should be called when first exchanging
 * keys with a peer (e.g., during handshake).
 */
void trellis_pop_register_peer_pubkey(const trellis_fingerprint_t *fp,
                                       const uint8_t *ed25519_pk);

// Issuer-side: register an established node as a PoP issuer.
trellis_err_t trellis_pop_register_issuer(
        const trellis_fingerprint_t *issuer_fp,
        const uint8_t *issuer_pubkey,
        size_t pubkey_len);

// Issuer-side: create a partial blind signature for a requesting node.
trellis_err_t trellis_pop_issue_partial(
        const trellis_identity_t *issuer,
        const uint8_t *blinded_commitment, size_t commitment_len,
        uint8_t **sig_out, size_t *sig_len_out);

// Client-side: aggregate T partial signatures into a credential.
trellis_err_t trellis_pop_aggregate(
        const uint8_t *blinded_commitment,
        const uint8_t *r_scalar,
        const uint8_t *sk_scalar,
        const uint8_t **partial_sigs, const size_t *sig_lens,
        const uint8_t **issuer_pubkeys,
        size_t count,
        const trellis_identity_t *holder,
        uint8_t *cred_out);

// Client-side: re-randomize credential and generate a Schnorr proof.
trellis_err_t trellis_pop_create_presentation(
        const uint8_t *cred,
        const uint8_t *r_scalar,
        const uint8_t *sk_scalar,
        const trellis_identity_t *holder_id,
        const trellis_fingerprint_t *verifier_fp,
        uint8_t *proof_out);

typedef struct trellis_hco_challenge {
    uint8_t  challenge_id[16];
    uint64_t issued_at;
    uint64_t expires_at;
    uint8_t  payload[256];
    size_t   payload_len;
} trellis_hco_challenge_t;

trellis_err_t trellis_hco_issue(trellis_hco_challenge_t *challenge_out);
trellis_err_t trellis_hco_verify(const trellis_hco_challenge_t *challenge,
                                 const uint8_t *response, size_t resp_len);
bool          trellis_hco_is_current(const trellis_fingerprint_t *peer);
double        trellis_hco_freshness(const trellis_fingerprint_t *peer);

#ifndef TRELLIS_SYBIL_TYPEDEF_DEFINED
#define TRELLIS_SYBIL_TYPEDEF_DEFINED
typedef struct trellis_sybil trellis_sybil_t;
#endif

trellis_sybil_t *trellis_sybil_new(void);
void             trellis_sybil_free(trellis_sybil_t *sybil);

trellis_trust_graph_t      *trellis_sybil_trust(trellis_sybil_t *sybil);
trellis_behavior_tracker_t *trellis_sybil_behavior(trellis_sybil_t *sybil);

trellis_err_t trellis_sybil_check_admission(
    trellis_sybil_t *sybil,
    const trellis_fingerprint_t *peer,
    const trellis_vdf_proof_t *vdf_proof,
    const trellis_identity_public_t *pub);

double trellis_sybil_composite_score(
    const trellis_sybil_t *sybil,
    const trellis_fingerprint_t *peer,
    double latency_ms, double bandwidth, double reliability,
    const trellis_vdf_proof_t *vdf_proof);

/*
 * Trust-weighted, AS-diverse hop selection for onion circuits.
 * Fills hops_out with up to hops_wanted peers. Returns actual count.
 */
size_t trellis_sybil_select_hops(
    const trellis_sybil_t *sybil,
    struct trellis_peer_info **candidates, size_t candidate_count,
    const trellis_fingerprint_t *exclude,
    struct trellis_peer_info **hops_out, size_t hops_wanted);

#define TRELLIS_GOSSIP_VOUCH_TAG 0x56 /* 'V' */

/*
 * Publish a vouch through the gossip subsystem so other peers can
 * incorporate it into their local trust graphs.
 */
trellis_err_t trellis_sybil_publish_vouch(
    trellis_sybil_t *sybil,
    struct trellis_gossip *gossip,
    const trellis_vouch_t *vouch,
    const trellis_identity_public_t *voucher_pub);

/*
 * Register a gossip subscriber that automatically feeds received
 * vouch messages into the sybil manager's trust graph.
 */
trellis_err_t trellis_sybil_subscribe_vouches(
    trellis_sybil_t *sybil,
    struct trellis_gossip *gossip);

#ifdef __cplusplus
}
#endif

#endif /* TRELLIS_SYBIL_H */
