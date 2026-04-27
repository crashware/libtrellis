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

// Forward declarations for S/Kademlia disjoint path enforcement.
static skad_group_t *skad_find_group(trellis_dht_t *dht, uint32_t group_id);
static bool skad_global_already_queried(skad_group_t *g,
                                         const trellis_fingerprint_t *fp);
static void skad_global_mark_queried(skad_group_t *g,
                                      const trellis_fingerprint_t *fp);

// borrowed from vine-relay, should deduplicate at some point.
static uint64_t fnv1a(const uint8_t *data, size_t len)
{
    uint64_t h = 0xcbf29ce484222325ULL;
    for (size_t i = 0; i < len; i++) {
        h ^= data[i];
        h *= 0x100000001b3ULL;
    }
    return h;
}

int dht_map_init(dht_map_t *m, size_t cap)
{
    if (!m)
        return -1;
    if (cap == 0)
        cap = DHT_MAP_INIT_CAP;
    m->entries = calloc(cap, sizeof(dht_map_entry_t));
    if (!m->entries)
        return -1;
    m->cap = cap;
    m->count = 0;
    return 0;
}

void dht_map_destroy(dht_map_t *m)
{
    if (!m || !m->entries)
        return;
    for (size_t i = 0; i < m->cap; i++) {
        if (m->entries[i].occupied) {
            free(m->entries[i].key);
            free(m->entries[i].value);
        }
    }
    free(m->entries);
    m->entries = NULL;
    m->cap = 0;
    m->count = 0;
}

static int dht_map_resize(dht_map_t *m)
{
    size_t new_cap = m->cap * 2;
    dht_map_entry_t *new_entries = calloc(new_cap, sizeof(dht_map_entry_t));
    if (!new_entries)
        return -1;

    for (size_t i = 0; i < m->cap; i++) {
        if (!m->entries[i].occupied)
            continue;
        uint64_t h = fnv1a(m->entries[i].key, m->entries[i].key_len);
        size_t idx = h % new_cap;
        while (new_entries[idx].occupied)
            idx = (idx + 1) % new_cap;
        new_entries[idx] = m->entries[i];
    }

    free(m->entries);
    m->entries = new_entries;
    m->cap = new_cap;
    return 0;
}

int dht_map_put(dht_map_t *m, const uint8_t *key, size_t key_len,
                const uint8_t *value, size_t value_len)
{
    if (!m || !key || !value)
        return -1;

    // Check for existing key first (updates are always allowed)
    uint64_t h = fnv1a(key, key_len);
    size_t idx = h % m->cap;

    while (m->entries[idx].occupied) {
        if (m->entries[idx].key_len == key_len &&
            memcmp(m->entries[idx].key, key, key_len) == 0) {
            uint8_t *new_val = malloc(value_len);
            if (!new_val)
                return -1;
            memcpy(new_val, value, value_len);
            free(m->entries[idx].value);
            m->entries[idx].value = new_val;
            m->entries[idx].value_len = value_len;
            return 0;
        }
        idx = (idx + 1) % m->cap;
    }

    // New entry: enforce global cap to prevent unbounded growth.
    if (m->count >= DHT_MAP_MAX_ENTRIES)
        return -2;

    if ((double)(m->count + 1) / (double)m->cap > DHT_MAP_LOAD_MAX) {
        if (dht_map_resize(m) != 0)
            return -1;
        // Recompute idx after resize.
        idx = h % m->cap;
        while (m->entries[idx].occupied)
            idx = (idx + 1) % m->cap;
    }

    m->entries[idx].key = malloc(key_len);
    m->entries[idx].value = malloc(value_len);
    if (!m->entries[idx].key || !m->entries[idx].value) {
        free(m->entries[idx].key);
        free(m->entries[idx].value);
        memset(&m->entries[idx], 0, sizeof(dht_map_entry_t));
        return -1;
    }

    memcpy(m->entries[idx].key, key, key_len);
    m->entries[idx].key_len = key_len;
    memcpy(m->entries[idx].value, value, value_len);
    m->entries[idx].value_len = value_len;
    m->entries[idx].occupied = true;
    m->count++;
    return 0;
}

int dht_map_get(const dht_map_t *m, const uint8_t *key, size_t key_len,
                const uint8_t **value_out, size_t *value_len_out)
{
    if (!m || !key || !value_out)
        return -1;

    uint64_t h = fnv1a(key, key_len);
    size_t idx = h % m->cap;

    for (size_t probes = 0; probes < m->cap; probes++) {
        if (!m->entries[idx].occupied)
            return -1;
        if (m->entries[idx].key_len == key_len &&
            memcmp(m->entries[idx].key, key, key_len) == 0) {
            *value_out = m->entries[idx].value;
            if (value_len_out)
                *value_len_out = m->entries[idx].value_len;
            return 0;
        }
        idx = (idx + 1) % m->cap;
    }
    return -1;
}

// --- Dedup hash set ---
int dedup_set_init(dedup_set_t *s, size_t cap)
{
    if (!s)
        return -1;
    if (cap == 0)
        cap = DEDUP_SET_INIT_CAP;
    s->entries = calloc(cap, sizeof(dedup_entry_t));
    if (!s->entries)
        return -1;
    s->cap = cap;
    s->count = 0;
    return 0;
}

void dedup_set_destroy(dedup_set_t *s)
{
    if (!s)
        return;
    free(s->entries);
    s->entries = NULL;
    s->cap = 0;
    s->count = 0;
}

bool dedup_set_contains(const dedup_set_t *s, const uint8_t digest[32])
{
    if (!s || !s->entries)
        return false;

    uint64_t h = fnv1a(digest, 32);
    size_t idx = h % s->cap;

    for (size_t probes = 0; probes < s->cap; probes++) {
        if (!s->entries[idx].occupied)
            return false;
        if (sodium_memcmp(s->entries[idx].digest, digest, 32) == 0)
            return true;
        idx = (idx + 1) % s->cap;
    }
    return false;
}

static int dedup_set_resize(dedup_set_t *s)
{
    size_t new_cap = s->cap * 2;
    if (new_cap > DEDUP_SET_MAX_CAP)
        new_cap = DEDUP_SET_MAX_CAP;
    if (new_cap <= s->cap)
        return -1; /* already at max */
    dedup_entry_t *new_entries = calloc(new_cap, sizeof(dedup_entry_t));
    if (!new_entries)
        return -1;

    for (size_t i = 0; i < s->cap; i++) {
        if (!s->entries[i].occupied)
            continue;
        uint64_t h = fnv1a(s->entries[i].digest, 32);
        size_t idx = h % new_cap;
        while (new_entries[idx].occupied)
            idx = (idx + 1) % new_cap;
        new_entries[idx] = s->entries[i];
    }

    free(s->entries);
    s->entries = new_entries;
    s->cap = new_cap;
    return 0;
}

int dedup_set_insert(dedup_set_t *s, const uint8_t digest[32])
{
    if (!s)
        return -1;

    if (dedup_set_contains(s, digest))
        return 0;

    if ((double)(s->count + 1) / (double)s->cap > DHT_MAP_LOAD_MAX) {
        if (dedup_set_resize(s) != 0)
            return -1;
    }

    uint64_t h = fnv1a(digest, 32);
    size_t idx = h % s->cap;
    while (s->entries[idx].occupied)
        idx = (idx + 1) % s->cap;

    memcpy(s->entries[idx].digest, digest, 32);
    s->entries[idx].occupied = true;
    s->entries[idx].inserted_at = trellis_now_ms();
    s->count++;
    return 0;
}

void xor_distance(const trellis_fingerprint_t *a,
                  const trellis_fingerprint_t *b,
                  uint8_t out[TRELLIS_FINGERPRINT_LEN])
{
    for (size_t i = 0; i < TRELLIS_FINGERPRINT_LEN; i++)
        out[i] = a->bytes[i] ^ b->bytes[i];
}

int bucket_index(const trellis_fingerprint_t *a,
                 const trellis_fingerprint_t *b)
{
    for (int i = 0; i < TRELLIS_FINGERPRINT_LEN; i++) {
        uint8_t diff = a->bytes[i] ^ b->bytes[i];
        if (diff == 0)
            continue;
        int bit = 7;
        while (bit >= 0 && !(diff & (1 << bit)))
            bit--;
        return (TRELLIS_DHT_ID_BITS - 1) - (i * 8 + (7 - bit));
    }
    return 0;
}

// --- Random peer selection ---
void select_random_peers(const routing_table_t *rt,
                         const trellis_sybil_t *sybil,
                         trellis_peer_info_t **out,
                         size_t count, size_t *actual)
{
    size_t total = 0;
    for (int b = 0; b < TRELLIS_DHT_BUCKETS; b++)
        total += rt->buckets[b].count;

    if (total == 0) {
        *actual = 0;
        return;
    }

    trellis_peer_info_t **all = malloc(total * sizeof(trellis_peer_info_t *));
    if (!all) {
        *actual = 0;
        return;
    }

    // Collect only non-quarantined peers.
    size_t eligible = 0;
    for (int b = 0; b < TRELLIS_DHT_BUCKETS; b++) {
        for (size_t p = 0; p < rt->buckets[b].count; p++) {
            if (!rt->buckets[b].peers[p]->quarantined)
                all[eligible++] = rt->buckets[b].peers[p];
        }
    }

    if (eligible == 0) {
        free(all);
        *actual = 0;
        return;
    }

    /*
     * When a sybil manager is provided, use trust-weighted selection:
     * compute composite scores and pick peers via weighted random draw.
     * Otherwise fall back to uniform Fisher-Yates shuffle.
     */
    if (sybil) {
        double *scores = calloc(eligible, sizeof(double));
        if (!scores) {
            free(all);
            *actual = 0;
            return;
        }

        double total_score = 0.0;
        for (size_t i = 0; i < eligible; i++) {
            scores[i] = trellis_sybil_composite_score(
                sybil, &all[i]->fingerprint,
                all[i]->latency_ms,
                all[i]->bandwidth,
                all[i]->reliability,
                all[i]->vdf_proof);
            total_score += scores[i];
        }

        bool *used = calloc(eligible, sizeof(bool));
        if (!used) {
            free(scores);
            free(all);
            *actual = 0;
            return;
        }

        size_t selected = 0;
        size_t max_attempts = eligible * 3;
        size_t attempts = 0;

        while (selected < count && attempts < max_attempts && total_score > 1e-12) {
            attempts++;
            uint32_t rnd = randombytes_uniform(1000000);
            double threshold = ((double)rnd / 1000000.0) * total_score;
            double cumulative = 0.0;
            size_t pick = eligible;

            for (size_t i = 0; i < eligible; i++) {
                if (used[i] || scores[i] <= 0.0)
                    continue;
                cumulative += scores[i];
                if (cumulative >= threshold) {
                    pick = i;
                    break;
                }
            }

            if (pick >= eligible)
                continue;

            out[selected++] = all[pick];
            used[pick] = true;
            total_score -= scores[pick];
        }

        *actual = selected;
        free(used);
        free(scores);
    } else {
        // Uniform Fisher-Yates shuffle, then take first `count`.
        for (size_t i = eligible - 1; i > 0; i--) {
            uint32_t j = randombytes_uniform((uint32_t)(i + 1));
            trellis_peer_info_t *tmp = all[i];
            all[i] = all[j];
            all[j] = tmp;
        }

        size_t n = count < eligible ? count : eligible;
        for (size_t i = 0; i < n; i++)
            out[i] = all[i];
        *actual = n;
    }

    free(all);
}

// --- K-bucket operations ---

/* Extract /16 prefix (first 2 octets) from an address string.
 * For IPv4 "a.b.c.d:port" stores {a, b}.
 * For IPv4-mapped IPv6 "::ffff:a.b.c.d" extracts the embedded IPv4 octets.
 * For native IPv6, hashes the full address for a stable prefix. */
static void kbucket_extract_prefix16(const char *addr, uint8_t prefix[2])
{
    prefix[0] = prefix[1] = 0;
    if (!addr || !addr[0])
        return;

    // IPv4-mapped IPv6: "::ffff:a.b.c.d" or "[::ffff:a.b.c.d]".
    const char *mapped = strstr(addr, "::ffff:");
    if (!mapped)
        mapped = strstr(addr, "::FFFF:");
    if (mapped) {
        const char *ipv4_start = mapped + 7; /* skip "::ffff:" */
        unsigned a, b;
        if (sscanf(ipv4_start, "%u.%u.", &a, &b) == 2 &&
            a <= 255 && b <= 255) {
            prefix[0] = (uint8_t)a;
            prefix[1] = (uint8_t)b;
            return;
        }
    }

    // Plain IPv4: "a.b.c.d:port".
    unsigned a, b;
    if (sscanf(addr, "%u.%u.", &a, &b) == 2 &&
        a <= 255 && b <= 255) {
        prefix[0] = (uint8_t)a;
        prefix[1] = (uint8_t)b;
        return;
    }

    // Strip brackets from "[addr]:port".
    const char *start = addr;
    if (start[0] == '[')
        start++;
    size_t len = strlen(start);
    const char *bracket = strchr(start, ']');
    if (bracket)
        len = (size_t)(bracket - start);
    const char *pct = strchr(start, '%');
    if (pct && (size_t)(pct - start) < len)
        len = (size_t)(pct - start);

    // Hash the normalized address for a stable prefix.
    uint8_t hash[32];
    crypto_generichash(hash, sizeof(hash),
                       (const uint8_t *)start, len, NULL, 0);
    prefix[0] = hash[0];
    prefix[1] = hash[1];
}

// Count peers in bucket sharing the same /16 subnet as the candidate.
static size_t kbucket_subnet_count(const k_bucket_t *kb, const uint8_t prefix[2])
{
    size_t count = 0;
    for (size_t i = 0; i < kb->count; i++) {
        uint8_t p[2];
        kbucket_extract_prefix16(kb->peers[i]->addr, p);
        if (p[0] == prefix[0] && p[1] == prefix[1])
            count++;
    }
    return count;
}

static int kbucket_add(k_bucket_t *kb, const trellis_peer_info_t *peer)
{
    for (size_t i = 0; i < kb->count; i++) {
        if (trellis_fingerprint_eq(&kb->peers[i]->fingerprint,
                                   &peer->fingerprint)) {
            kb->peers[i]->last_seen = trellis_now_ms();
            kb->peers[i]->latency_ms = peer->latency_ms;
            if (peer->addr[0])
                memcpy(kb->peers[i]->addr, peer->addr, sizeof(peer->addr));
            static const uint8_t zero_pk[TRELLIS_X25519_PK_LEN] = {0};
            if (memcmp(peer->identity.x25519_pk, zero_pk, TRELLIS_X25519_PK_LEN) != 0)
                kb->peers[i]->identity = peer->identity;

            // Update sybil fields: deep-copy new VDF proof if provided.
            if (peer->vdf_proof &&
                peer->vdf_proof != kb->peers[i]->vdf_proof) {
                void *new_proof = malloc(sizeof(*peer->vdf_proof));
                if (new_proof) {
                    memcpy(new_proof, peer->vdf_proof, sizeof(*peer->vdf_proof));
                    free(kb->peers[i]->vdf_proof);
                    kb->peers[i]->vdf_proof = new_proof;
                }
            }
            kb->peers[i]->quarantined = peer->quarantined;

            return 0;
        }
    }

    // Reject peers with no address — they cannot be dialed or relayed through.
    if (!peer->addr[0])
        return 1;

    // Subnet diversity check: reject if this /16 already has too many peers.
    uint8_t new_prefix[2];
    kbucket_extract_prefix16(peer->addr, new_prefix);
    if (kbucket_subnet_count(kb, new_prefix) >= KBUCKET_MAX_PER_SUBNET)
        return 1; /* rejected: subnet over-represented */

    if (kb->count < TRELLIS_DHT_K) {
        trellis_peer_info_t *copy = malloc(sizeof(trellis_peer_info_t));
        if (!copy)
            return -1;
        memcpy(copy, peer, sizeof(trellis_peer_info_t));
        copy->vdf_proof = NULL;
        if (peer->vdf_proof) {
            copy->vdf_proof = malloc(sizeof(*peer->vdf_proof));
            if (!copy->vdf_proof) { free(copy); return -1; }
            memcpy(copy->vdf_proof, peer->vdf_proof, sizeof(*peer->vdf_proof));
        }
        copy->last_seen = trellis_now_ms();
        kb->peers[kb->count++] = copy;
        return 0;
    }

    // Bucket full — evict the stalest entry if it's older than 5 minutes.
    uint64_t now = trellis_now_ms();
    size_t stalest = 0;
    uint64_t oldest = kb->peers[0]->last_seen;
    for (size_t i = 1; i < kb->count; i++) {
        if (kb->peers[i]->last_seen < oldest) {
            oldest = kb->peers[i]->last_seen;
            stalest = i;
        }
    }

    if (now - oldest > 300000) {
        trellis_peer_info_t *copy = malloc(sizeof(trellis_peer_info_t));
        if (!copy)
            return -1;
        memcpy(copy, peer, sizeof(trellis_peer_info_t));
        copy->vdf_proof = NULL;
        if (peer->vdf_proof) {
            copy->vdf_proof = malloc(sizeof(*peer->vdf_proof));
            if (!copy->vdf_proof) { free(copy); return -1; }
            memcpy(copy->vdf_proof, peer->vdf_proof, sizeof(*peer->vdf_proof));
        }
        copy->last_seen = now;
        free(kb->peers[stalest]->vdf_proof);
        free(kb->peers[stalest]);
        kb->peers[stalest] = copy;
        return 0;
    }

    return 1; /* bucket full, no stale entry to evict */
}

static void kbucket_remove(k_bucket_t *kb, const trellis_fingerprint_t *fp)
{
    for (size_t i = 0; i < kb->count; i++) {
        if (trellis_fingerprint_eq(&kb->peers[i]->fingerprint, fp)) {
            free(kb->peers[i]->vdf_proof);
            free(kb->peers[i]);
            kb->peers[i] = kb->peers[kb->count - 1];
            kb->peers[kb->count - 1] = NULL;
            kb->count--;
            return;
        }
    }
}

typedef struct {
    trellis_peer_info_t *peer;
    uint8_t              distance[TRELLIS_FINGERPRINT_LEN];
} peer_distance_t;

static int cmp_peer_distance(const void *a, const void *b)
{
    const peer_distance_t *pa = a;
    const peer_distance_t *pb = b;
    return memcmp(pa->distance, pb->distance, TRELLIS_FINGERPRINT_LEN);
}

static size_t find_closest_peers(const routing_table_t *rt,
                                 const trellis_fingerprint_t *target,
                                 trellis_peer_info_t *out[], size_t max)
{
    size_t total = 0;
    for (int b = 0; b < TRELLIS_DHT_BUCKETS; b++)
        total += rt->buckets[b].count;

    if (total == 0)
        return 0;

    peer_distance_t *pds = malloc(total * sizeof(peer_distance_t));
    if (!pds)
        return 0;

    size_t idx = 0;
    for (int b = 0; b < TRELLIS_DHT_BUCKETS; b++) {
        for (size_t p = 0; p < rt->buckets[b].count; p++) {
            pds[idx].peer = rt->buckets[b].peers[p];
            xor_distance(&rt->buckets[b].peers[p]->fingerprint,
                         target, pds[idx].distance);
            idx++;
        }
    }

    qsort(pds, total, sizeof(peer_distance_t), cmp_peer_distance);

    size_t n = max < total ? max : total;
    for (size_t i = 0; i < n; i++)
        out[i] = pds[i].peer;

    free(pds);
    return n;
}

/*
 * RPC payload format (inside trellis_message_t.payload):
 *   [4] request_id (LE)
 *   [1] rpc_type (trellis_dht_rpc_t)
 *   ... type-specific data
 */
static bool dht_has_send(const trellis_dht_t *dht)
{
    return dht->send_fn != NULL;
}

static void dht_send_rpc(trellis_dht_t *dht,
                          const trellis_fingerprint_t *target,
                          uint16_t msg_type,
                          const uint8_t *payload, size_t payload_len)
{
    if (!dht_has_send(dht))
        return;

    // Build message envelope: [1 version][2 type][4 req_id + payload]
    size_t total = 3 + payload_len;
    uint8_t *buf = malloc(total);
    if (!buf)
        return;

    buf[0] = 1; /* protocol version */
    buf[1] = (uint8_t)(msg_type >> 8);
    buf[2] = (uint8_t)(msg_type);
    memcpy(buf + 3, payload, payload_len);

    dht->send_fn(dht->send_client, target, buf, total);
    free(buf);
}

static bool dht_rpc_id_in_use(trellis_dht_t *dht, uint32_t id)
{
    for (size_t j = 0; j < DHT_MAX_PENDING_RPCS; j++) {
        if (dht->rpcs[j].active && dht->rpcs[j].request_id == id)
            return true;
    }
    return false;
}

static dht_pending_rpc_t *dht_alloc_rpc(trellis_dht_t *dht)
{
    for (size_t i = 0; i < DHT_MAX_PENDING_RPCS; i++) {
        if (!dht->rpcs[i].active) {
            /* Preserve the uv_timer_t if it was previously initialized —
             * zeroing it would corrupt libuv's internal handle queue. */
            uv_timer_t saved_timer = dht->rpcs[i].timer;
            bool had_timer = (saved_timer.loop != NULL);
            memset(&dht->rpcs[i], 0, sizeof(dht_pending_rpc_t));
            if (had_timer)
                dht->rpcs[i].timer = saved_timer;
            dht->rpcs[i].active = true;

            uint32_t id;
            do {
                id = randombytes_random();
            } while (id == 0 || dht_rpc_id_in_use(dht, id));
            dht->rpcs[i].request_id = id;

            dht->rpcs[i].dht = dht;
            dht->rpcs[i].sent_at_ms = trellis_now_ms();
            return &dht->rpcs[i];
        }
    }
    return NULL;
}

static void dht_free_rpc(dht_pending_rpc_t *rpc)
{
    if (!rpc || !rpc->active)
        return;

    if (uv_is_active((uv_handle_t *)&rpc->timer))
        uv_timer_stop(&rpc->timer);

    free(rpc->key);
    free(rpc->value);
    rpc->key = NULL;
    rpc->value = NULL;
    rpc->active = false;
}

static dht_pending_rpc_t *dht_find_rpc(trellis_dht_t *dht, uint32_t request_id)
{
    for (size_t i = 0; i < DHT_MAX_PENDING_RPCS; i++) {
        if (dht->rpcs[i].active && dht->rpcs[i].request_id == request_id)
            return &dht->rpcs[i];
    }
    return NULL;
}

static void dht_remove_rpc(trellis_dht_t *dht, uint32_t request_id)
{
    dht_pending_rpc_t *rpc = dht_find_rpc(dht, request_id);
    if (rpc)
        dht_free_rpc(rpc);
}

static bool dht_already_queried(const dht_pending_rpc_t *rpc,
                                const trellis_fingerprint_t *fp)
{
    for (size_t i = 0; i < rpc->queried_count; i++) {
        if (trellis_fingerprint_eq(&rpc->queried[i], fp))
            return true;
    }
    return false;
}

static void dht_rpc_timeout(uv_timer_t *handle)
{
    dht_pending_rpc_t *rpc = handle->data;
    if (!rpc || !rpc->active)
        return;

    switch (rpc->kind) {
    case DHT_RPC_STORE_OP:
        if (rpc->store_cb)
            rpc->store_cb(rpc->acks_received > 0 ? TRELLIS_OK
                                                  : TRELLIS_ERR_TIMEOUT,
                          rpc->store_ctx);
        break;
    case DHT_RPC_FIND_VALUE_OP:
        if (rpc->find_cb)
            rpc->find_cb(NULL, 0, TRELLIS_ERR_NOT_FOUND, rpc->find_ctx);
        break;
    case DHT_RPC_FIND_NODE_OP:
        if (rpc->find_node_cb)
            rpc->find_node_cb(NULL, 0, TRELLIS_ERR_NOT_FOUND,
                              rpc->find_node_ctx);
        break;
    }

    dht_free_rpc(rpc);
}

/*
 * Serialize a STORE request payload:
 *   [4] request_id  [4] key_len  [key]  [4] value_len  [value]
 */
static void dht_send_store_rpc(trellis_dht_t *dht,
                                const trellis_fingerprint_t *target,
                                uint32_t request_id,
                                const uint8_t *key, size_t key_len,
                                const uint8_t *value, size_t value_len)
{
    if (key_len > SIZE_MAX - 12 - value_len)
        return;
    size_t plen = 4 + 4 + key_len + 4 + value_len;
    uint8_t *payload = malloc(plen);
    if (!payload)
        return;

    size_t off = 0;
    trellis_write_u32_be(payload + off, request_id);        off += 4;
    trellis_write_u32_be(payload + off, (uint32_t)key_len); off += 4;
    memcpy(payload + off, key, key_len);                    off += key_len;
    trellis_write_u32_be(payload + off, (uint32_t)value_len); off += 4;
    memcpy(payload + off, value, value_len);

    dht_send_rpc(dht, target, TRELLIS_MSG_STORE, payload, plen);
    free(payload);
}

/*
 * Serialize a STORE ACK reply:
 *   [4] request_id
 */
static void dht_send_store_ack(trellis_dht_t *dht,
                                const trellis_fingerprint_t *target,
                                uint32_t request_id)
{
    uint8_t payload[4];
    trellis_write_u32_be(payload, request_id);
    dht_send_rpc(dht, target, TRELLIS_MSG_STORE, payload, 4);
}

/*
 * Serialize a FIND_VALUE request:
 *   [4] request_id  [4] key_len  [key]
 */
static void dht_send_find_value_rpc(trellis_dht_t *dht,
                                     const trellis_fingerprint_t *target,
                                     uint32_t request_id,
                                     const uint8_t *key, size_t key_len)
{
    size_t plen = 4 + 4 + key_len;
    uint8_t *payload = malloc(plen);
    if (!payload)
        return;

    trellis_write_u32_be(payload, request_id);
    trellis_write_u32_be(payload + 4, (uint32_t)key_len);
    memcpy(payload + 8, key, key_len);

    dht_send_rpc(dht, target, TRELLIS_MSG_FIND_VALUE, payload, plen);
    free(payload);
}

/*
 * Serialize a FIND_VALUE response:
 *   [4] request_id  [1] found
 *   if found: [4] value_len  [value]
 *   else:     [4] peer_count  [FINGERPRINT_LEN + 256] * peer_count
 */
static void dht_send_find_value_result(trellis_dht_t *dht,
                                        const trellis_fingerprint_t *target,
                                        uint32_t request_id,
                                        const uint8_t *value, size_t value_len,
                                        trellis_peer_info_t **closest, size_t nclose)
{
    bool found = (value != NULL);
    size_t plen;

    if (found) {
        plen = 4 + 1 + 4 + value_len;
    } else {
        plen = 4 + 1 + 4 + nclose * (TRELLIS_FINGERPRINT_LEN + 256);
    }

    uint8_t *payload = malloc(plen);
    if (!payload)
        return;

    size_t off = 0;
    trellis_write_u32_be(payload + off, request_id); off += 4;
    payload[off++] = found ? 1 : 0;

    if (found) {
        trellis_write_u32_be(payload + off, (uint32_t)value_len); off += 4;
        memcpy(payload + off, value, value_len);
    } else {
        trellis_write_u32_be(payload + off, (uint32_t)nclose); off += 4;
        for (size_t i = 0; i < nclose; i++) {
            memcpy(payload + off, closest[i]->fingerprint.bytes,
                   TRELLIS_FINGERPRINT_LEN);
            off += TRELLIS_FINGERPRINT_LEN;
            memcpy(payload + off, closest[i]->addr, 256);
            off += 256;
        }
    }

    dht_send_rpc(dht, target, TRELLIS_MSG_FIND_VALUE, payload, plen);
    free(payload);
}

/*
 * Serialize a FIND_NODE request:
 *   [4] request_id  [FINGERPRINT_LEN] target
 */
static void dht_send_find_node_rpc(trellis_dht_t *dht,
                                    const trellis_fingerprint_t *target,
                                    uint32_t request_id,
                                    const trellis_fingerprint_t *find_target)
{
    size_t plen = 4 + TRELLIS_FINGERPRINT_LEN;
    uint8_t payload[4 + TRELLIS_FINGERPRINT_LEN];
    trellis_write_u32_be(payload, request_id);
    memcpy(payload + 4, find_target->bytes, TRELLIS_FINGERPRINT_LEN);

    dht_send_rpc(dht, target, TRELLIS_MSG_FIND_NODE, payload, plen);
}

/*
 * Serialize a FIND_NODE response:
 *   [4] request_id  [4] peer_count  [peer_info] * peer_count
 *   peer_info: [FINGERPRINT_LEN] fp  [256] addr
 */
static void dht_send_find_node_result(trellis_dht_t *dht,
                                       const trellis_fingerprint_t *target,
                                       uint32_t request_id,
                                       trellis_peer_info_t **peers, size_t count)
{
    size_t per_peer = TRELLIS_FINGERPRINT_LEN + 256;
    size_t plen = 4 + 4 + count * per_peer;
    uint8_t *payload = malloc(plen);
    if (!payload)
        return;

    trellis_write_u32_be(payload, request_id);
    trellis_write_u32_be(payload + 4, (uint32_t)count);

    size_t off = 8;
    for (size_t i = 0; i < count; i++) {
        memcpy(payload + off, peers[i]->fingerprint.bytes,
               TRELLIS_FINGERPRINT_LEN);
        off += TRELLIS_FINGERPRINT_LEN;
        memcpy(payload + off, peers[i]->addr, 256);
        off += 256;
    }

    dht_send_rpc(dht, target, TRELLIS_MSG_FIND_NODE, payload, plen);
    free(payload);
}

// Forward declaration: defined later with trellis_dht_store.
static bool dht_store_rate_check(trellis_dht_t *dht,
                                  const trellis_fingerprint_t *fp);

// --- Incoming RPC dispatch ---
#define DHT_STORE_MAX_KEY_LEN    256
#define DHT_STORE_MAX_VALUE_LEN  65536

static void dht_handle_store_request(trellis_dht_t *dht,
                                      const trellis_fingerprint_t *sender,
                                      const uint8_t *payload, size_t len)
{
    if (len < 12)
        return;

    uint32_t request_id = trellis_read_u32_be(payload);
    uint32_t key_len = trellis_read_u32_be(payload + 4);

    if (key_len > DHT_STORE_MAX_KEY_LEN || len < 12 + (size_t)key_len)
        return;

    const uint8_t *key = payload + 8;
    uint32_t value_len = trellis_read_u32_be(payload + 8 + key_len);

    if (value_len > DHT_STORE_MAX_VALUE_LEN ||
        len < 12 + (size_t)key_len + (size_t)value_len)
        return;

    const uint8_t *value = payload + 12 + key_len;

    // Per-sender rate limiting on inbound STOREs.
    if (!dht_store_rate_check(dht, sender))
        return;

    if (dht_map_put(&dht->store, key, key_len, value, value_len) != 0)
        return;

    dht_send_store_ack(dht, sender, request_id);
}

static void dht_handle_store_ack(trellis_dht_t *dht,
                                  const uint8_t *payload, size_t len)
{
    if (len < 4)
        return;

    uint32_t request_id = trellis_read_u32_be(payload);
    dht_pending_rpc_t *rpc = dht_find_rpc(dht, request_id);
    if (!rpc || rpc->kind != DHT_RPC_STORE_OP)
        return;

    rpc->acks_received++;

    if (rpc->acks_received >= rpc->acks_needed) {
        if (rpc->store_cb)
            rpc->store_cb(TRELLIS_OK, rpc->store_ctx);
        dht_free_rpc(rpc);
    }
}

static void dht_handle_find_value_request(trellis_dht_t *dht,
                                           const trellis_fingerprint_t *sender,
                                           const uint8_t *payload, size_t len)
{
    if (len < 8)
        return;

    uint32_t request_id = trellis_read_u32_be(payload);
    uint32_t key_len = trellis_read_u32_be(payload + 4);

    if (key_len > DHT_STORE_MAX_KEY_LEN || len < 8 + (size_t)key_len)
        return;

    const uint8_t *key = payload + 8;

    const uint8_t *val = NULL;
    size_t val_len = 0;

    if (dht_map_get(&dht->store, key, key_len, &val, &val_len) == 0) {
        dht_send_find_value_result(dht, sender, request_id,
                                   val, val_len, NULL, 0);
    } else {
        trellis_fingerprint_t key_fp = {0};
        size_t copy = key_len < TRELLIS_FINGERPRINT_LEN
                    ? key_len : TRELLIS_FINGERPRINT_LEN;
        memcpy(key_fp.bytes, key, copy);

        trellis_peer_info_t *closest[TRELLIS_DHT_K];
        size_t n = find_closest_peers(&dht->rt, &key_fp,
                                      closest, TRELLIS_DHT_K);
        dht_send_find_value_result(dht, sender, request_id,
                                   NULL, 0, closest, n);
    }
}

static void dht_handle_find_value_response(trellis_dht_t *dht,
                                            const trellis_fingerprint_t *sender,
                                            const uint8_t *payload, size_t len)
{
    if (len < 5)
        return;

    uint32_t request_id = trellis_read_u32_be(payload);
    bool found = payload[4] != 0;

    dht_pending_rpc_t *rpc = dht_find_rpc(dht, request_id);
    if (!rpc || rpc->kind != DHT_RPC_FIND_VALUE_OP)
        return;

    (void)sender;

    if (found) {
        if (len < 9)
            return;
        uint32_t value_len = trellis_read_u32_be(payload + 5);
        if (value_len > DHT_STORE_MAX_VALUE_LEN || len < 9 + (size_t)value_len)
            return;

        const uint8_t *value = payload + 9;

        /* Callers are responsible for caching validated values via
         * trellis_dht_store(); never cache unvalidated remote data. */
        if (rpc->find_cb)
            rpc->find_cb(value, value_len, TRELLIS_OK, rpc->find_ctx);
        dht_free_rpc(rpc);
        return;
    }

    // Not found -- response contains closer peers for iterative lookup.
    if (len < 9)
        return;

    uint32_t peer_count = trellis_read_u32_be(payload + 5);
    size_t per_peer = TRELLIS_FINGERPRINT_LEN + 256;
    if (peer_count > SIZE_MAX / per_peer || len < 9 + peer_count * per_peer)
        return;

    // Add returned peers to routing table and query closer ones.
    size_t new_queries = 0;
    size_t off = 9;

    for (uint32_t i = 0; i < peer_count && new_queries < DHT_ALPHA; i++) {
        trellis_fingerprint_t fp;
        memcpy(fp.bytes, payload + off, TRELLIS_FINGERPRINT_LEN);
        off += TRELLIS_FINGERPRINT_LEN;

        char addr[256];
        memcpy(addr, payload + off, 255);
        addr[255] = '\0';
        off += 256;

        if (dht_already_queried(rpc, &fp))
            continue;

        // S/Kademlia disjoint enforcement: skip peers queried by other paths.
        if (rpc->skad_group_id) {
            skad_group_t *sg = skad_find_group(dht, rpc->skad_group_id);
            if (sg && skad_global_already_queried(sg, &fp))
                continue;
            if (sg) skad_global_mark_queried(sg, &fp);
        }

        trellis_peer_info_t new_peer = {0};
        new_peer.fingerprint = fp;
        memcpy(new_peer.addr, addr, 256);
        new_peer.last_seen = trellis_now_ms();
        new_peer.reliability = 0.5;
        trellis_dht_add_peer(dht, &new_peer);

        if (rpc->queried_count < sizeof(rpc->queried)/sizeof(rpc->queried[0]))
            rpc->queried[rpc->queried_count++] = fp;

        dht_send_find_value_rpc(dht, &fp, rpc->request_id,
                                rpc->key, rpc->key_len);
        rpc->pending_responses++;
        new_queries++;
    }

    if (rpc->pending_responses > 0)
        rpc->pending_responses--;
    if (rpc->pending_responses == 0 && new_queries == 0) {
        if (rpc->find_cb)
            rpc->find_cb(NULL, 0, TRELLIS_ERR_NOT_FOUND, rpc->find_ctx);
        dht_free_rpc(rpc);
    }
}

static void dht_handle_find_node_request(trellis_dht_t *dht,
                                          const trellis_fingerprint_t *sender,
                                          const uint8_t *payload, size_t len)
{
    if (len < 4 + TRELLIS_FINGERPRINT_LEN)
        return;

    uint32_t request_id = trellis_read_u32_be(payload);
    trellis_fingerprint_t target;
    memcpy(target.bytes, payload + 4, TRELLIS_FINGERPRINT_LEN);

    trellis_peer_info_t *closest[TRELLIS_DHT_K];
    size_t n = find_closest_peers(&dht->rt, &target, closest, TRELLIS_DHT_K);

    dht_send_find_node_result(dht, sender, request_id, closest, n);
}

static void dht_handle_find_node_response(trellis_dht_t *dht,
                                           const uint8_t *payload, size_t len)
{
    if (len < 8)
        return;

    uint32_t request_id = trellis_read_u32_be(payload);
    uint32_t peer_count = trellis_read_u32_be(payload + 4);

    dht_pending_rpc_t *rpc = dht_find_rpc(dht, request_id);

    // Reject unsolicited/spoofed responses before adding peers.
    if (!rpc || rpc->kind != DHT_RPC_FIND_NODE_OP)
        return;

    size_t per_peer = TRELLIS_FINGERPRINT_LEN + 256;
    if (peer_count > SIZE_MAX / per_peer || len < 8 + peer_count * per_peer)
        return;

    // Add all returned peers to routing table.
    size_t off = 8;
    for (uint32_t i = 0; i < peer_count; i++) {
        trellis_peer_info_t new_peer = {0};
        memcpy(new_peer.fingerprint.bytes, payload + off,
               TRELLIS_FINGERPRINT_LEN);
        off += TRELLIS_FINGERPRINT_LEN;
        memcpy(new_peer.addr, payload + off, 255);
        new_peer.addr[255] = '\0';
        off += 256;
        new_peer.last_seen = trellis_now_ms();
        new_peer.reliability = 0.5;

        trellis_dht_add_peer(dht, &new_peer);
    }

    // Build result array from routing table.
    trellis_fingerprint_t key_fp = {0};
    if (rpc->key) {
        size_t copy = rpc->key_len < TRELLIS_FINGERPRINT_LEN
                    ? rpc->key_len : TRELLIS_FINGERPRINT_LEN;
        memcpy(key_fp.bytes, rpc->key, copy);
    }

    trellis_peer_info_t *closest[TRELLIS_DHT_K];
    size_t n = find_closest_peers(&dht->rt, &key_fp, closest, TRELLIS_DHT_K);

    // Check if new queries would yield closer results.
    size_t new_queries = 0;
    for (size_t i = 0; i < n && new_queries < DHT_ALPHA; i++) {
        if (dht_already_queried(rpc, &closest[i]->fingerprint))
            continue;
        if (rpc->queried_count < sizeof(rpc->queried)/sizeof(rpc->queried[0]))
            rpc->queried[rpc->queried_count++] = closest[i]->fingerprint;

        dht_send_find_node_rpc(dht, &closest[i]->fingerprint,
                               rpc->request_id, &key_fp);
        rpc->pending_responses++;
        new_queries++;
    }

    if (rpc->pending_responses > 0)
        rpc->pending_responses--;
    if (rpc->pending_responses == 0 && new_queries == 0) {
        trellis_peer_info_t *result = malloc(n * sizeof(trellis_peer_info_t));
        if (result) {
            for (size_t i = 0; i < n; i++)
                result[i] = *closest[i];
            if (rpc->find_node_cb)
                rpc->find_node_cb(result, n, TRELLIS_OK, rpc->find_node_ctx);
            free(result);
        } else {
            if (rpc->find_node_cb)
                rpc->find_node_cb(NULL, 0, TRELLIS_ERR_NOMEM,
                                  rpc->find_node_ctx);
        }
        dht_free_rpc(rpc);
    }
}

// --- DHT public API ---
#define ECLIPSE_CHECK_INTERVAL_MS  30000  /* 30 seconds */

static void eclipse_timer_cb(uv_timer_t *handle)
{
    trellis_dht_t *dht = (trellis_dht_t *)handle->data;
    if (dht)
        trellis_dht_check_eclipse(dht);
}

trellis_dht_t *trellis_dht_new(const trellis_identity_t *local_id,
                                uv_loop_t *loop)
{
    if (!local_id || !loop)
        return NULL;

    trellis_dht_t *dht = calloc(1, sizeof(trellis_dht_t));
    if (!dht)
        return NULL;

    dht->local_id = local_id;
    dht->loop = loop;

    if (dht_map_init(&dht->store, DHT_MAP_INIT_CAP) != 0) {
        free(dht);
        return NULL;
    }

    dht->sybil = trellis_sybil_new();
    trellis_clock_sync_init(&dht->clock_sync);

    // Start periodic eclipse check timer.
    uv_timer_init(loop, &dht->eclipse_timer);
    dht->eclipse_timer.data = dht;
    dht->eclipse_timer_init = true;
    uv_timer_start(&dht->eclipse_timer, eclipse_timer_cb,
                   ECLIPSE_CHECK_INTERVAL_MS, ECLIPSE_CHECK_INTERVAL_MS);

    return dht;
}

static void dht_final_free(trellis_dht_t *dht)
{
    for (int b = 0; b < TRELLIS_DHT_BUCKETS; b++) {
        for (size_t p = 0; p < dht->rt.buckets[b].count; p++) {
            if (dht->rt.buckets[b].peers[p]) {
                free(dht->rt.buckets[b].peers[p]->vdf_proof);
                free(dht->rt.buckets[b].peers[p]);
            }
        }
    }

    trellis_sybil_free(dht->sybil);
    dht_map_destroy(&dht->store);
    free(dht->grove_id);
    free(dht);
}

static void dht_rpc_teardown_close_cb(uv_handle_t *handle)
{
    trellis_dht_t *dht = (trellis_dht_t *)handle->data;
    if (!dht)
        return;
    dht->rpc_close_pending--;
    if (dht->rpc_close_pending <= 0)
        dht_final_free(dht);
}

static void dht_cleanup_and_free(trellis_dht_t *dht)
{
    /* Scan all RPC slots: free heap data and track timer handles that need
     * async close.  Slots freed by dht_free_rpc (active==false) may still
     * have an initialized timer handle that must be closed. */
    bool needs_close[DHT_MAX_PENDING_RPCS] = {false};
    int rpc_closes = 0;

    for (size_t i = 0; i < DHT_MAX_PENDING_RPCS; i++) {
        if (dht->rpcs[i].active) {
            free(dht->rpcs[i].key);
            free(dht->rpcs[i].value);
            dht->rpcs[i].key = NULL;
            dht->rpcs[i].value = NULL;
            dht->rpcs[i].active = false;
        }

        if (dht->rpcs[i].timer.loop) {
            if (uv_is_active((uv_handle_t *)&dht->rpcs[i].timer))
                uv_timer_stop(&dht->rpcs[i].timer);
            needs_close[i] = true;
            rpc_closes++;
        }
    }

    if (rpc_closes == 0) {
        dht_final_free(dht);
        return;
    }

    // Defer free(dht) until all RPC timer handles are closed.
    dht->rpc_close_pending = rpc_closes;
    for (size_t i = 0; i < DHT_MAX_PENDING_RPCS; i++) {
        if (needs_close[i]) {
            dht->rpcs[i].timer.data = dht;
            uv_close((uv_handle_t *)&dht->rpcs[i].timer,
                     dht_rpc_teardown_close_cb);
        }
    }
}

static void eclipse_timer_close_cb(uv_handle_t *handle)
{
    trellis_dht_t *dht = (trellis_dht_t *)handle->data;
    if (dht)
        dht_cleanup_and_free(dht);
}

void trellis_dht_free(trellis_dht_t *dht)
{
    if (!dht)
        return;

    /* Stop the periodic eclipse timer and defer free until close completes.
     * uv_close is async; the handle memory (inside dht) must stay alive
     * until the close callback fires. */
    if (dht->eclipse_timer_init) {
        uv_timer_stop(&dht->eclipse_timer);
        dht->eclipse_timer_init = false;
        if (!uv_is_closing((uv_handle_t *)&dht->eclipse_timer)) {
            uv_close((uv_handle_t *)&dht->eclipse_timer, eclipse_timer_close_cb);
            return;
        }
    }

    dht_cleanup_and_free(dht);
}

void trellis_dht_set_send(trellis_dht_t *dht,
                           trellis_dht_send_fn fn, void *client)
{
    if (!dht)
        return;
    dht->send_fn = fn;
    dht->send_client = client;
}

trellis_err_t trellis_dht_on_message(trellis_dht_t *dht,
                                      const trellis_fingerprint_t *sender,
                                      const uint8_t *data, size_t len)
{
    if (!dht || !sender || !data || len < 3)
        return TRELLIS_ERR_INVALID_ARG;

    uint16_t msg_type = ((uint16_t)data[1] << 8) | data[2];
    const uint8_t *payload = data + 3;
    size_t payload_len = len - 3;

    /*
     * Distinguish requests from responses by payload length heuristics:
     * - STORE requests have key+value (>= 12 bytes)
     * - STORE acks have just request_id (4 bytes)
     * - FIND_VALUE requests have key (>= 8 bytes, no 'found' flag at [4])
     * - FIND_VALUE results have 'found' flag at [4]
     * - FIND_NODE requests have just target fingerprint (4 + FP_LEN)
     * - FIND_NODE results have peer_count at [4]
     *
     * We use the message type and check if this is a response to a pending
     * RPC (by request_id) to disambiguate.
     */
    if (payload_len < 4)
        return TRELLIS_ERR_INVALID_ARG;

    uint32_t request_id = trellis_read_u32_be(payload);
    dht_pending_rpc_t *pending = dht_find_rpc(dht, request_id);

    switch (msg_type) {
    case TRELLIS_MSG_PING: {
        /* Echo PONG: [request_id(4)][sender_time_echo(8)][responder_time(8)]
         * Backwards-compatible: old PING has 4 bytes, new has 12. */
        uint8_t pong_buf[20];
        trellis_write_u32_be(pong_buf, request_id);
        uint64_t sender_time = 0;
        if (payload_len >= 12) {
            memcpy(&sender_time, payload + 4, 8);
            memcpy(pong_buf + 4, payload + 4, 8); /* echo sender_time */
        } else {
            memset(pong_buf + 4, 0, 8);
        }
        uint64_t responder_time = trellis_now_ms();
        memcpy(pong_buf + 12, &responder_time, 8);
        dht_send_rpc(dht, sender, TRELLIS_MSG_PONG, pong_buf, 20);
        break;
    }

    case TRELLIS_MSG_PONG:
        if (pending) {
            uint64_t now = trellis_now_ms();
            double rtt = 0.0;
            if (pending->sent_at_ms > 0)
                rtt = (double)(now - pending->sent_at_ms);

            // Update peer latency via EWMA.
            int bi = bucket_index(&dht->local_id->fingerprint, sender);
            if (bi >= 0) {
                k_bucket_t *kb = &dht->rt.buckets[bi];
                for (size_t pi = 0; pi < kb->count; pi++) {
                    if (trellis_fingerprint_eq(&kb->peers[pi]->fingerprint,
                                               sender)) {
                        double old = kb->peers[pi]->latency_ms;
                        kb->peers[pi]->latency_ms = old > 0.0
                            ? 0.8 * old + 0.2 * rtt
                            : rtt;
                        break;
                    }
                }
            }

            // Clock skew estimation from PONG timestamps.
            if (payload_len >= 20 && pending->sent_at_ms > 0) {
                uint64_t responder_time;
                memcpy(&responder_time, payload + 12, 8);
                if (responder_time > 0) {
                    int64_t offset = (int64_t)responder_time -
                                     (int64_t)(pending->sent_at_ms +
                                               (uint64_t)(rtt / 2.0));
                    trellis_clock_sync_add_sample(&dht->clock_sync, offset);
                }
            }

            dht_remove_rpc(dht, request_id);
        }
        break;

    case TRELLIS_MSG_STORE:
        if (pending && pending->kind == DHT_RPC_STORE_OP) {
            dht_handle_store_ack(dht, payload, payload_len);
        } else {
            dht_handle_store_request(dht, sender, payload, payload_len);
        }
        break;

    case TRELLIS_MSG_FIND_VALUE:
        if (pending && pending->kind == DHT_RPC_FIND_VALUE_OP) {
            dht_handle_find_value_response(dht, sender,
                                           payload, payload_len);
        } else {
            dht_handle_find_value_request(dht, sender,
                                          payload, payload_len);
        }
        break;

    case TRELLIS_MSG_FIND_NODE:
        if (pending && pending->kind == DHT_RPC_FIND_NODE_OP) {
            dht_handle_find_node_response(dht, payload, payload_len);
        } else {
            dht_handle_find_node_request(dht, sender,
                                         payload, payload_len);
        }
        break;

    default:
        return TRELLIS_ERR_PROTOCOL;
    }

    // Update sender's last_seen.
    trellis_dht_update_peer_seen(dht, sender);

    return TRELLIS_OK;
}

trellis_err_t trellis_dht_bootstrap(trellis_dht_t *dht, const char *seed_addr)
{
    if (!dht || !seed_addr)
        return TRELLIS_ERR_INVALID_ARG;

    trellis_peer_info_t seed = {0};
    size_t addr_len = strlen(seed_addr);
    if (addr_len >= sizeof(seed.addr))
        return TRELLIS_ERR_INVALID_ARG;
    memcpy(seed.addr, seed_addr, addr_len + 1);
    /* Use a deterministic fingerprint derived from the seed address so that:
     *  (a) the same seed appears as the same peer across bootstrap calls, and
     *  (b) the peer is placed in a consistent k-bucket in the routing table.
     * The real fingerprint will be learned from the FIND_NODE response and
     * the routing table entry updated at that time. */
    crypto_hash_sha256(seed.fingerprint.bytes,
                       (const uint8_t *)seed_addr, addr_len);
    seed.last_seen = trellis_now_ms();
    seed.reliability = 1.0;

    trellis_err_t err = trellis_dht_add_peer(dht, &seed);
    if (err != TRELLIS_OK)
        return err;

    if (dht_has_send(dht)) {
        // Async: issue FIND_NODE(self) to populate routing table.
        return trellis_dht_find_node(dht, &dht->local_id->fingerprint,
                                     NULL, NULL);
    }

    // Fallback: synchronous best-effort pass over known peers.
    trellis_peer_info_t *closest[TRELLIS_DHT_K];
    size_t n = find_closest_peers(&dht->rt, &dht->local_id->fingerprint,
                                  closest, TRELLIS_DHT_K);
    (void)n;

    return TRELLIS_OK;
}

trellis_err_t trellis_dht_add_peer(trellis_dht_t *dht,
                                    const trellis_peer_info_t *peer)
{
    if (!dht || !peer)
        return TRELLIS_ERR_INVALID_ARG;

    // Reject new peers while routing table is frozen due to eclipse.
    if (dht->eclipse_frozen) {
        if (trellis_now_ms() >= dht->eclipse_freeze_until_ms) {
            dht->eclipse_frozen = false;
            dht->eclipse_detected = false;
        } else {
            return TRELLIS_ERR_RATE_LIMITED;
        }
    }

    /*
     * Sybil admission gate: if the subsystem is active, check VDF
     * proof validity. Peers failing the check are quarantined rather
     * than rejected outright -- they can still receive data but will
     * never be selected as relay or onion hops.
     */
    bool quarantine = false;
    if (dht->sybil) {
        trellis_err_t admit = trellis_sybil_check_admission(
            dht->sybil, &peer->fingerprint,
            peer->vdf_proof, &peer->identity);
        if (admit != TRELLIS_OK)
            quarantine = true;

        // Record the peer's address for behavioral subnet tracking.
        if (peer->addr[0]) {
            trellis_behavior_record_seen(
                trellis_sybil_behavior(dht->sybil),
                &peer->fingerprint, peer->addr);
        }
    }

    if (trellis_fingerprint_eq(&dht->local_id->fingerprint, &peer->fingerprint))
        return TRELLIS_OK;

    int idx = bucket_index(&dht->local_id->fingerprint, &peer->fingerprint);
    if (idx < 0 || idx >= TRELLIS_DHT_BUCKETS)
        idx = 0;

    // Make a mutable copy so we can set quarantine status and first_seen.
    trellis_peer_info_t augmented = *peer;
    augmented.quarantined = quarantine;
    /* Stamp first_seen if not already set (preserve existing timestamp on
     * re-adds so the age is calculated from the original insertion time). */
    if (augmented.first_seen == 0)
        augmented.first_seen = trellis_now_ms();

    int rc = kbucket_add(&dht->rt.buckets[idx], &augmented);
    if (rc < 0)
        return TRELLIS_ERR_NOMEM;
    if (rc > 0)
        return TRELLIS_ERR_LIMIT_EXCEEDED;

    // Track bucket access time for stale-bucket refresh.
    dht->bucket_last_access[idx] = trellis_now_ms();

    dht->peer_count = 0;
    for (int b = 0; b < TRELLIS_DHT_BUCKETS; b++)
        dht->peer_count += dht->rt.buckets[b].count;

    /* Update sibling table: keep the TRELLIS_DHT_K closest peers to ourselves.
     * This enables neighbourhood-isolation detection: if our sibling table is
     * ever dominated by peers from a single subnet or Sybil cluster, we alert. */
    trellis_peer_info_t *all[TRELLIS_DHT_K * 2];
    size_t all_n = find_closest_peers(&dht->rt, &dht->local_id->fingerprint,
                                      all, TRELLIS_DHT_K);
    dht->sibling_count = all_n;
    for (size_t i = 0; i < all_n; i++)
        dht->sibling_table[i] = all[i];

    // Check for eclipse attack after every sibling table update.
    trellis_dht_check_eclipse(dht);

    return TRELLIS_OK;
}

trellis_err_t trellis_dht_store(trellis_dht_t *dht,
                                 const uint8_t *key, size_t key_len,
                                 const uint8_t *value, size_t value_len,
                                 trellis_dht_store_cb cb, void *ctx)
{
    if (!dht || !key || !value)
        return TRELLIS_ERR_INVALID_ARG;

    // Apply grove namespace scoping if bound to a grove.
    uint8_t scoped_buf[32];
    const uint8_t *effective_key = key;
    size_t effective_key_len = key_len;
    if (dht->grove_id) {
        trellis_grove_scope_key(dht->grove_id, key, key_len, scoped_buf);
        effective_key = scoped_buf;
        effective_key_len = 32;
    }

    // Always store locally.
    if (dht_map_put(&dht->store, effective_key, effective_key_len, value, value_len) != 0) {
        if (cb) cb(TRELLIS_ERR_NOMEM, ctx);
        return TRELLIS_ERR_NOMEM;
    }

    trellis_fingerprint_t key_fp = {0};
    size_t copy_len = effective_key_len < TRELLIS_FINGERPRINT_LEN
                    ? effective_key_len : TRELLIS_FINGERPRINT_LEN;
    memcpy(key_fp.bytes, effective_key, copy_len);

    trellis_peer_info_t *closest[TRELLIS_DHT_K];
    size_t n = find_closest_peers(&dht->rt, &key_fp, closest, TRELLIS_DHT_K);

    if (!dht_has_send(dht) || n == 0) {
        if (cb) cb(TRELLIS_OK, ctx);
        return TRELLIS_OK;
    }

    // Async: replicate to k closest peers via RPC_STORE.
    dht_pending_rpc_t *rpc = dht_alloc_rpc(dht);
    if (!rpc) {
        if (cb) cb(TRELLIS_ERR_NOMEM, ctx);
        return TRELLIS_ERR_NOMEM;
    }

    rpc->kind = DHT_RPC_STORE_OP;
    rpc->key = malloc(effective_key_len);
    rpc->value = malloc(value_len);
    if (!rpc->key || !rpc->value) {
        dht_free_rpc(rpc);
        if (cb) cb(TRELLIS_ERR_NOMEM, ctx);
        return TRELLIS_ERR_NOMEM;
    }
    memcpy(rpc->key, effective_key, effective_key_len);
    rpc->key_len = effective_key_len;
    memcpy(rpc->value, value, value_len);
    rpc->value_len = value_len;
    rpc->store_cb = cb;
    rpc->store_ctx = ctx;
    rpc->acks_needed = n < DHT_STORE_QUORUM ? n : DHT_STORE_QUORUM;

    if (!rpc->timer.loop)
        uv_timer_init(dht->loop, &rpc->timer);
    rpc->timer.data = rpc;
    uv_timer_start(&rpc->timer, dht_rpc_timeout, DHT_RPC_TIMEOUT_MS, 0);

    for (size_t i = 0; i < n; i++) {
        dht_send_store_rpc(dht, &closest[i]->fingerprint,
                           rpc->request_id, effective_key, effective_key_len,
                           value, value_len);
    }

    return TRELLIS_OK;
}

trellis_err_t trellis_dht_find_value(trellis_dht_t *dht,
                                      const uint8_t *key, size_t key_len,
                                      trellis_dht_find_cb cb, void *ctx)
{
    if (!dht || !key)
        return TRELLIS_ERR_INVALID_ARG;

    // Apply grove namespace scoping if bound to a grove.
    uint8_t scoped_buf[32];
    const uint8_t *effective_key = key;
    size_t effective_key_len = key_len;
    if (dht->grove_id) {
        trellis_grove_scope_key(dht->grove_id, key, key_len, scoped_buf);
        effective_key = scoped_buf;
        effective_key_len = 32;
    }

    // Check local store first.
    const uint8_t *val = NULL;
    size_t val_len = 0;
    if (dht_map_get(&dht->store, effective_key, effective_key_len, &val, &val_len) == 0) {
        if (cb) cb(val, val_len, TRELLIS_OK, ctx);
        return TRELLIS_OK;
    }

    trellis_fingerprint_t key_fp = {0};
    size_t copy_len = effective_key_len < TRELLIS_FINGERPRINT_LEN
                    ? effective_key_len : TRELLIS_FINGERPRINT_LEN;
    memcpy(key_fp.bytes, effective_key, copy_len);

    trellis_peer_info_t *closest[TRELLIS_DHT_K];
    size_t n = find_closest_peers(&dht->rt, &key_fp, closest, TRELLIS_DHT_K);

    if (!dht_has_send(dht) || n == 0) {
        if (cb) cb(NULL, 0, TRELLIS_ERR_NOT_FOUND, ctx);
        return TRELLIS_ERR_NOT_FOUND;
    }

    // Async: iterative FIND_VALUE to closest peers.
    dht_pending_rpc_t *rpc = dht_alloc_rpc(dht);
    if (!rpc) {
        if (cb) cb(NULL, 0, TRELLIS_ERR_NOT_FOUND, ctx);
        return TRELLIS_ERR_NOT_FOUND;
    }

    rpc->kind = DHT_RPC_FIND_VALUE_OP;
    rpc->key = malloc(effective_key_len);
    if (!rpc->key) {
        dht_free_rpc(rpc);
        if (cb) cb(NULL, 0, TRELLIS_ERR_NOMEM, ctx);
        return TRELLIS_ERR_NOMEM;
    }
    memcpy(rpc->key, effective_key, effective_key_len);
    rpc->key_len = effective_key_len;
    rpc->find_cb = cb;
    rpc->find_ctx = ctx;

    if (!rpc->timer.loop)
        uv_timer_init(dht->loop, &rpc->timer);
    rpc->timer.data = rpc;
    uv_timer_start(&rpc->timer, dht_rpc_timeout, DHT_RPC_TIMEOUT_MS, 0);

    // Query alpha closest peers.
    size_t to_query = n < DHT_ALPHA ? n : DHT_ALPHA;
    for (size_t i = 0; i < to_query; i++) {
        if (rpc->queried_count < sizeof(rpc->queried)/sizeof(rpc->queried[0]))
            rpc->queried[rpc->queried_count++] = closest[i]->fingerprint;
        dht_send_find_value_rpc(dht, &closest[i]->fingerprint,
                                rpc->request_id, effective_key, effective_key_len);
        rpc->pending_responses++;
    }

    return TRELLIS_OK;
}

trellis_err_t trellis_dht_find_node(trellis_dht_t *dht,
                                     const trellis_fingerprint_t *target,
                                     trellis_dht_find_node_cb cb, void *ctx)
{
    if (!dht || !target)
        return TRELLIS_ERR_INVALID_ARG;

    trellis_peer_info_t *closest[TRELLIS_DHT_K];
    size_t n = find_closest_peers(&dht->rt, target, closest, TRELLIS_DHT_K);

    if (!dht_has_send(dht) || n == 0) {
        // Synchronous fallback.
        if (n == 0) {
            if (cb) cb(NULL, 0, TRELLIS_ERR_NOT_FOUND, ctx);
            return TRELLIS_ERR_NOT_FOUND;
        }

        trellis_peer_info_t *result = malloc(n * sizeof(trellis_peer_info_t));
        if (!result) {
            if (cb) cb(NULL, 0, TRELLIS_ERR_NOMEM, ctx);
            return TRELLIS_ERR_NOMEM;
        }
        for (size_t i = 0; i < n; i++)
            result[i] = *closest[i];
        if (cb) cb(result, n, TRELLIS_OK, ctx);
        free(result);
        return TRELLIS_OK;
    }

    // Async: iterative FIND_NODE.
    dht_pending_rpc_t *rpc = dht_alloc_rpc(dht);
    if (!rpc) {
        // Fall back to local results.
        trellis_peer_info_t *result = malloc(n * sizeof(trellis_peer_info_t));
        if (result) {
            for (size_t i = 0; i < n; i++)
                result[i] = *closest[i];
            if (cb) cb(result, n, TRELLIS_OK, ctx);
            free(result);
        } else {
            if (cb) cb(NULL, 0, TRELLIS_ERR_NOMEM, ctx);
        }
        return TRELLIS_OK;
    }

    rpc->kind = DHT_RPC_FIND_NODE_OP;
    rpc->key = malloc(TRELLIS_FINGERPRINT_LEN);
    if (!rpc->key) {
        dht_free_rpc(rpc);
        if (cb) cb(NULL, 0, TRELLIS_ERR_NOMEM, ctx);
        return TRELLIS_ERR_NOMEM;
    }
    memcpy(rpc->key, target->bytes, TRELLIS_FINGERPRINT_LEN);
    rpc->key_len = TRELLIS_FINGERPRINT_LEN;
    rpc->find_node_cb = cb;
    rpc->find_node_ctx = ctx;

    if (!rpc->timer.loop)
        uv_timer_init(dht->loop, &rpc->timer);
    rpc->timer.data = rpc;
    uv_timer_start(&rpc->timer, dht_rpc_timeout, DHT_RPC_TIMEOUT_MS, 0);

    size_t to_query = n < DHT_ALPHA ? n : DHT_ALPHA;
    for (size_t i = 0; i < to_query; i++) {
        if (rpc->queried_count < sizeof(rpc->queried)/sizeof(rpc->queried[0]))
            rpc->queried[rpc->queried_count++] = closest[i]->fingerprint;
        dht_send_find_node_rpc(dht, &closest[i]->fingerprint,
                               rpc->request_id, target);
        rpc->pending_responses++;
    }

    return TRELLIS_OK;
}

// --- Redundant descriptor storage ---
#define STORE_RATE_LIMIT_PER_WINDOW  10
#define STORE_RATE_WINDOW_MS         60000u  /* 1 minute */

/*
 * Derive replica key i from primary key:
 *   replica_key = HKDF-SHAKE-256(ikm=primary_key, salt=byte(i), info="dht-replica")[:32]
 */
static void dht_derive_replica_key(const uint8_t *key, size_t key_len,
                                    int replica_idx,
                                    uint8_t *out, size_t *out_len)
{
    uint8_t salt = (uint8_t)replica_idx;
    static const uint8_t info[] = "dht-replica";
    trellis_hkdf_shake256(key, key_len,
                          &salt, 1,
                          info, sizeof(info) - 1,
                          out, 32);
    *out_len = 32;
}

// Returns true if the identity is allowed to store (not rate-limited).
static bool dht_store_rate_check(trellis_dht_t *dht,
                                  const trellis_fingerprint_t *fp)
{
    if (!fp)
        return true;  /* NULL identity = skip rate limiting (see mesh.h) */

    uint64_t now = trellis_now_ms();

    // Find existing entry.
    for (size_t i = 0; i < dht->store_rate_count; i++) {
        if (trellis_fingerprint_eq(&dht->store_rate[i].fp, fp)) {
            // Reset window if expired.
            if (now - dht->store_rate[i].window_start_ms > STORE_RATE_WINDOW_MS) {
                dht->store_rate[i].window_start_ms = now;
                dht->store_rate[i].count = 0;
            }
            if (dht->store_rate[i].count >= STORE_RATE_LIMIT_PER_WINDOW)
                return false;
            dht->store_rate[i].count++;
            return true;
        }
    }

    // New entry.
    size_t max = sizeof(dht->store_rate) / sizeof(dht->store_rate[0]);
    if (dht->store_rate_count < max) {
        size_t idx = dht->store_rate_count++;
        dht->store_rate[idx].fp               = *fp;
        dht->store_rate[idx].window_start_ms  = now;
        dht->store_rate[idx].count            = 1;
    }
    // If table full: allow (conservative -- don't drop unknowns)
    return true;
}

// Context for tracking redundant store completions.
typedef struct redundant_store_ctx {
    int                  total;
    int                  done;
    int                  succeeded;
    bool                 in_loop;
    trellis_dht_store_cb cb;
    void                *ctx;
} redundant_store_ctx_t;

static void redundant_store_cb(trellis_err_t err, void *raw_ctx)
{
    redundant_store_ctx_t *rsc = (redundant_store_ctx_t *)raw_ctx;
    rsc->done++;
    if (err == TRELLIS_OK)
        rsc->succeeded++;

    /* While the issuing loop is still running, just tally — the loop
     * will check completion and free after it finishes iterating. */
    if (rsc->in_loop || rsc->done < rsc->total)
        return;

    trellis_err_t final_err = (rsc->succeeded > 0) ? TRELLIS_OK
                                                    : TRELLIS_ERR_NOT_FOUND;
    if (rsc->cb)
        rsc->cb(final_err, rsc->ctx);
    free(rsc);
}

trellis_err_t trellis_dht_store_redundant(
        trellis_dht_t *dht,
        const uint8_t *key, size_t key_len,
        const uint8_t *value, size_t value_len,
        const trellis_fingerprint_t *identity,
        trellis_dht_store_cb cb, void *ctx)
{
    if (!dht || !key)
        return TRELLIS_ERR_INVALID_ARG;

    // Rate limit check.
    if (!dht_store_rate_check(dht, identity)) {
        fprintf(stderr, "[DHT] store rate-limited for identity\n");
        if (cb) cb(TRELLIS_ERR_RATE_LIMITED, ctx);
        return TRELLIS_ERR_RATE_LIMITED;
    }

    redundant_store_ctx_t *rsc = calloc(1, sizeof(*rsc));
    if (!rsc)
        return TRELLIS_ERR_NOMEM;
    rsc->total   = TRELLIS_DHT_REDUNDANT_REPLICAS;
    rsc->cb      = cb;
    rsc->ctx     = ctx;
    rsc->in_loop = true;

    for (int i = 0; i < TRELLIS_DHT_REDUNDANT_REPLICAS; i++) {
        uint8_t replica_key[32];
        size_t  replica_key_len;
        dht_derive_replica_key(key, key_len, i, replica_key, &replica_key_len);

        trellis_err_t err = trellis_dht_store(dht,
                                               replica_key, replica_key_len,
                                               value, value_len,
                                               redundant_store_cb, rsc);
        /* trellis_dht_store fires the callback synchronously on NOMEM
         * (dht_map_put fail) AND returns NOMEM.  For other error returns
         * the callback is NOT invoked — count manually. */
        if (err != TRELLIS_OK && err != TRELLIS_ERR_NOMEM)
            rsc->done++;
    }

    rsc->in_loop = false;

    if (rsc->done >= rsc->total) {
        trellis_err_t final = (rsc->succeeded > 0) ? TRELLIS_OK
                                                    : TRELLIS_ERR_NOT_FOUND;
        if (cb) cb(final, ctx);
        free(rsc);
    }

    return TRELLIS_OK;
}

// Context for redundant find: race across replicas, return first found.
typedef struct redundant_find_ctx {
    int                 total;
    int                 done;
    bool                delivered;
    bool                in_loop;
    trellis_dht_find_cb cb;
    void               *ctx;
} redundant_find_ctx_t;

static void redundant_find_cb(const uint8_t *value, size_t value_len,
                               trellis_err_t err, void *raw_ctx)
{
    redundant_find_ctx_t *rfc = (redundant_find_ctx_t *)raw_ctx;
    rfc->done++;

    if (!rfc->delivered && err == TRELLIS_OK && value && value_len > 0) {
        rfc->delivered = true;
        if (rfc->cb)
            rfc->cb(value, value_len, TRELLIS_OK, rfc->ctx);
    }

    if (rfc->in_loop || rfc->done < rfc->total)
        return;

    if (!rfc->delivered) {
        if (rfc->cb)
            rfc->cb(NULL, 0, TRELLIS_ERR_NOT_FOUND, rfc->ctx);
    }
    free(rfc);
}

trellis_err_t trellis_dht_find_value_redundant(
        trellis_dht_t *dht,
        const uint8_t *key, size_t key_len,
        trellis_dht_find_cb cb, void *ctx)
{
    if (!dht || !key)
        return TRELLIS_ERR_INVALID_ARG;

    redundant_find_ctx_t *rfc = calloc(1, sizeof(*rfc));
    if (!rfc)
        return TRELLIS_ERR_NOMEM;
    rfc->total   = TRELLIS_DHT_REDUNDANT_REPLICAS;
    rfc->cb      = cb;
    rfc->ctx     = ctx;
    rfc->in_loop = true;

    for (int i = 0; i < TRELLIS_DHT_REDUNDANT_REPLICAS; i++) {
        uint8_t replica_key[32];
        size_t  replica_key_len;
        dht_derive_replica_key(key, key_len, i, replica_key, &replica_key_len);

        trellis_err_t err = trellis_dht_find_value_skad(dht,
                                                         replica_key,
                                                         replica_key_len,
                                                         redundant_find_cb, rfc);
        /* find_value_skad fires the callback synchronously on local hit
         * (returns OK), on no-peers (returns NOT_FOUND), and on NOMEM
         * (via fallback to trellis_dht_find_value).  In all these cases
         * the callback already incremented done — skip here. */
        if (err != TRELLIS_OK && err != TRELLIS_ERR_NOT_FOUND &&
            err != TRELLIS_ERR_NOMEM)
            rfc->done++;
    }

    rfc->in_loop = false;

    if (rfc->done >= rfc->total) {
        if (!rfc->delivered) {
            if (cb) cb(NULL, 0, TRELLIS_ERR_NOT_FOUND, ctx);
        }
        free(rfc);
    }

    return TRELLIS_OK;
}

// --- S/Kademlia disjoint parallel lookups ---

// Find a free skad_group slot.
static skad_group_t *skad_alloc_group(trellis_dht_t *dht)
{
    for (int i = 0; i < SKAD_MAX_GROUPS; i++) {
        if (!dht->skad_groups[i].active) {
            memset(&dht->skad_groups[i], 0, sizeof(skad_group_t));
            dht->skad_groups[i].active   = true;
            dht->skad_groups[i].group_id = ++dht->next_skad_group_id;
            dht->skad_groups[i].dht      = dht;
            return &dht->skad_groups[i];
        }
    }
    return NULL;
}

static skad_group_t *skad_find_group(trellis_dht_t *dht, uint32_t group_id)
{
    for (int i = 0; i < SKAD_MAX_GROUPS; i++) {
        if (dht->skad_groups[i].active &&
            dht->skad_groups[i].group_id == group_id)
            return &dht->skad_groups[i];
    }
    return NULL;
}

// Called when a path within an S/Kademlia group completes.
static void skad_path_done(skad_group_t *g, int path_idx,
                            const uint8_t *result, size_t result_len)
{
    g->path_done[path_idx] = true;
    g->completed_paths++;

    if (result && result_len > 0) {
        g->path_results[path_idx] = malloc(result_len);
        if (g->path_results[path_idx]) {
            memcpy(g->path_results[path_idx], result, result_len);
            g->path_result_lens[path_idx] = result_len;
            g->successful_paths++;
        }
    }

    // Check if all paths are done.
    if (g->completed_paths < SKAD_D)
        return;

    // Agreement check: find value that appears >= SKAD_AGREE_THRESHOLD times.
    const uint8_t *agreed     = NULL;
    size_t         agreed_len = 0;
    int            agree_count = 0;

    for (int i = 0; i < SKAD_D; i++) {
        if (!g->path_results[i]) continue;
        int count = 0;
        for (int j = 0; j < SKAD_D; j++) {
            if (!g->path_results[j]) continue;
            if (g->path_result_lens[i] == g->path_result_lens[j] &&
                memcmp(g->path_results[i], g->path_results[j],
                       g->path_result_lens[i]) == 0)
                count++;
        }
        if (count >= SKAD_AGREE_THRESHOLD) {
            agreed     = g->path_results[i];
            agreed_len = g->path_result_lens[i];
            agree_count = count;
            break;
        }
    }

    fprintf(stderr, "[S/Kademlia] lookup done: %d/%d paths succeeded, "
                    "%d agree (threshold=%d)\n",
            g->successful_paths, SKAD_D,
            agree_count, SKAD_AGREE_THRESHOLD);

    if (!g->is_find_node) {
        if (g->find_cb) {
            if (agreed)
                g->find_cb(agreed, agreed_len, TRELLIS_OK, g->find_ctx);
            else
                g->find_cb(NULL, 0, TRELLIS_ERR_NOT_FOUND, g->find_ctx);
        }
    } else {
        // For FIND_NODE: return the best set of closest peers found.
        if (g->find_node_cb) {
            // Use non-NULL result from path 0, 1, or 2.
            const uint8_t *node_data = NULL;
            size_t         node_len  = 0;
            for (int i = 0; i < SKAD_D; i++) {
                if (g->path_results[i]) {
                    node_data = g->path_results[i];
                    node_len  = g->path_result_lens[i];
                    break;
                }
            }
            if (node_data && agree_count >= SKAD_AGREE_THRESHOLD) {
                // Deserialize peers from node_data and call find_node_cb.
                // Peer data format: N * TRELLIS_FINGERPRINT_LEN bytes.
                size_t peer_count = node_len / TRELLIS_FINGERPRINT_LEN;
                trellis_peer_info_t *peers = calloc(peer_count,
                                                     sizeof(trellis_peer_info_t));
                if (peers) {
                    for (size_t pi = 0; pi < peer_count; pi++) {
                        memcpy(peers[pi].fingerprint.bytes,
                               node_data + pi * TRELLIS_FINGERPRINT_LEN,
                               TRELLIS_FINGERPRINT_LEN);
                    }
                    g->find_node_cb(peers, peer_count, TRELLIS_OK,
                                    g->find_node_ctx);
                    free(peers);
                } else {
                    g->find_node_cb(NULL, 0, TRELLIS_ERR_NOMEM,
                                    g->find_node_ctx);
                }
            } else {
                g->find_node_cb(NULL, 0, TRELLIS_ERR_NOT_FOUND,
                                g->find_node_ctx);
            }
        }
    }

    // Clean up.
    for (int i = 0; i < SKAD_D; i++) {
        free(g->path_results[i]);
        g->path_results[i] = NULL;
    }
    g->active = false;
}

/* Wrapper callback: intercepts a single-path find_value result and routes
 * it to the parent S/Kademlia group. */
typedef struct skad_path_ctx {
    uint32_t group_id;
    int      path_idx;
    trellis_dht_t *dht;
} skad_path_ctx_t;

static void skad_path_find_value_cb(const uint8_t *value, size_t value_len,
                                     trellis_err_t err, void *ctx)
{
    skad_path_ctx_t *pc = (skad_path_ctx_t *)ctx;
    skad_group_t *g = skad_find_group(pc->dht, pc->group_id);
    if (g) {
        if (err == TRELLIS_OK && value && value_len > 0)
            skad_path_done(g, pc->path_idx, value, value_len);
        else
            skad_path_done(g, pc->path_idx, NULL, 0);
    }
    free(pc);
}

static void skad_path_find_node_cb(const trellis_peer_info_t *peers,
                                    size_t count, trellis_err_t err, void *ctx)
{
    skad_path_ctx_t *pc = (skad_path_ctx_t *)ctx;
    skad_group_t *g = skad_find_group(pc->dht, pc->group_id);
    if (g) {
        if (err == TRELLIS_OK && peers && count > 0) {
            // Encode peers as fingerprint bytes for agreement check.
            size_t  data_len = count * TRELLIS_FINGERPRINT_LEN;
            uint8_t *data    = malloc(data_len);
            if (data) {
                for (size_t i = 0; i < count; i++)
                    memcpy(data + i * TRELLIS_FINGERPRINT_LEN,
                           peers[i].fingerprint.bytes, TRELLIS_FINGERPRINT_LEN);
                skad_path_done(g, pc->path_idx, data, data_len);
                free(data);
            } else {
                skad_path_done(g, pc->path_idx, NULL, 0);
            }
        } else {
            skad_path_done(g, pc->path_idx, NULL, 0);
        }
    }
    free(pc);
}

/*
 * Check if `fp` is in the group's global queried set.
 * Returns true if already queried (and thus this path should skip it).
 */
static bool skad_global_already_queried(skad_group_t *g,
                                         const trellis_fingerprint_t *fp)
{
    for (size_t i = 0; i < g->global_queried_count; i++) {
        if (trellis_fingerprint_eq(&g->global_queried[i], fp))
            return true;
    }
    return false;
}

static void skad_global_mark_queried(skad_group_t *g,
                                      const trellis_fingerprint_t *fp)
{
    size_t max = sizeof(g->global_queried) / sizeof(g->global_queried[0]);
    if (g->global_queried_count < max) {
        g->global_queried[g->global_queried_count++] = *fp;
    }
}

/*
 * S/Kademlia FIND_VALUE: d=3 disjoint parallel iterative lookups.
 */
trellis_err_t trellis_dht_find_value_skad(trellis_dht_t *dht,
                                           const uint8_t *key, size_t key_len,
                                           trellis_dht_find_cb cb, void *ctx)
{
    if (!dht || !key)
        return TRELLIS_ERR_INVALID_ARG;

    // Apply grove namespace scoping if bound to a grove.
    uint8_t scoped_buf[32];
    const uint8_t *effective_key = key;
    size_t effective_key_len = key_len;
    if (dht->grove_id) {
        trellis_grove_scope_key(dht->grove_id, key, key_len, scoped_buf);
        effective_key = scoped_buf;
        effective_key_len = 32;
    }

    // Check local store first.
    const uint8_t *val = NULL;
    size_t val_len = 0;
    if (dht_map_get(&dht->store, effective_key, effective_key_len, &val, &val_len) == 0) {
        if (cb) cb(val, val_len, TRELLIS_OK, ctx);
        return TRELLIS_OK;
    }

    trellis_fingerprint_t key_fp = {0};
    size_t copy_len = effective_key_len < TRELLIS_FINGERPRINT_LEN
                    ? effective_key_len : TRELLIS_FINGERPRINT_LEN;
    memcpy(key_fp.bytes, effective_key, copy_len);

    // Gather up to SKAD_D * DHT_ALPHA closest peers for seed assignment.
    size_t seed_max = (size_t)(SKAD_D * TRELLIS_DHT_K);
    trellis_peer_info_t *seeds[SKAD_D * TRELLIS_DHT_K];
    size_t n = find_closest_peers(&dht->rt, &key_fp, seeds, seed_max);

    if (!dht_has_send(dht) || n == 0) {
        if (cb) cb(NULL, 0, TRELLIS_ERR_NOT_FOUND, ctx);
        return TRELLIS_ERR_NOT_FOUND;
    }

    skad_group_t *g = skad_alloc_group(dht);
    if (!g) {
        /* Fall back to single-path lookup.  Pass the raw key because
         * trellis_dht_find_value applies its own scoping pass. */
        return trellis_dht_find_value(dht, key, key_len, cb, ctx);
    }
    g->find_cb      = cb;
    g->find_ctx     = ctx;
    g->is_find_node = false;

    // Assign seeds to paths round-robin to ensure disjointness.
    for (size_t i = 0; i < n; i++) {
        int path = (int)(i % SKAD_D);
        if (!skad_global_already_queried(g, &seeds[i]->fingerprint))
            skad_global_mark_queried(g, &seeds[i]->fingerprint);
        (void)path;
    }

    /* Start d independent find_value operations, each with a different
     * seed offset so they diverge into different network regions */
    for (int d = 0; d < SKAD_D; d++) {
        // Select this path's starting peer (round-robin offset d)
        size_t start = (size_t)d;
        if (start >= n) {
            // Not enough peers for this path; mark it done immediately.
            g->path_done[d]    = true;
            g->completed_paths++;
            continue;
        }

        skad_path_ctx_t *pc = malloc(sizeof(*pc));
        if (!pc) {
            g->path_done[d]    = true;
            g->completed_paths++;
            continue;
        }
        pc->group_id = g->group_id;
        pc->path_idx = d;
        pc->dht      = dht;

        // Allocate an RPC for this path, seeded from `start` peer.
        dht_pending_rpc_t *rpc = dht_alloc_rpc(dht);
        if (!rpc) {
            free(pc);
            g->path_done[d]    = true;
            g->completed_paths++;
            continue;
        }

        rpc->kind          = DHT_RPC_FIND_VALUE_OP;
        rpc->skad_group_id = g->group_id;
        rpc->skad_path_idx = d;
        rpc->find_cb       = skad_path_find_value_cb;
        rpc->find_ctx      = pc;
        rpc->key = malloc(effective_key_len);
        if (!rpc->key) {
            dht_free_rpc(rpc); free(pc);
            g->path_done[d]    = true;
            g->completed_paths++;
            continue;
        }
        memcpy(rpc->key, effective_key, effective_key_len);
        rpc->key_len = effective_key_len;

        if (!rpc->timer.loop)
            uv_timer_init(dht->loop, &rpc->timer);
        rpc->timer.data = rpc;
        uv_timer_start(&rpc->timer, dht_rpc_timeout, DHT_RPC_TIMEOUT_MS, 0);

        g->path_rpc_ids[d] = rpc->request_id;

        // Send initial FIND_VALUE to the d-th closest peer.
        size_t peer_idx = start;
        if (rpc->queried_count <
                sizeof(rpc->queried)/sizeof(rpc->queried[0]))
            rpc->queried[rpc->queried_count++] =
                seeds[peer_idx]->fingerprint;
        dht_send_find_value_rpc(dht, &seeds[peer_idx]->fingerprint,
                                rpc->request_id,
                                effective_key, effective_key_len);
        rpc->pending_responses++;
    }

    // If all paths were instantly marked done (no peers), fire callback.
    if (g->completed_paths == SKAD_D) {
        if (cb) cb(NULL, 0, TRELLIS_ERR_NOT_FOUND, ctx);
        g->active = false;
    }

    return TRELLIS_OK;
}

/*
 * S/Kademlia FIND_NODE: d=3 disjoint parallel iterative lookups.
 */
trellis_err_t trellis_dht_find_node_skad(trellis_dht_t *dht,
                                          const trellis_fingerprint_t *target,
                                          trellis_dht_find_node_cb cb, void *ctx)
{
    if (!dht || !target)
        return TRELLIS_ERR_INVALID_ARG;

    size_t seed_max = (size_t)(SKAD_D * TRELLIS_DHT_K);
    trellis_peer_info_t *seeds[SKAD_D * TRELLIS_DHT_K];
    size_t n = find_closest_peers(&dht->rt, target, seeds, seed_max);

    if (!dht_has_send(dht) || n == 0) {
        if (cb) cb(NULL, 0, TRELLIS_ERR_NOT_FOUND, ctx);
        return TRELLIS_ERR_NOT_FOUND;
    }

    skad_group_t *g = skad_alloc_group(dht);
    if (!g)
        return trellis_dht_find_node(dht, target, cb, ctx);

    g->find_node_cb  = cb;
    g->find_node_ctx = ctx;
    g->is_find_node  = true;

    // Encode target key as bytes for use in seed marking.
    for (size_t i = 0; i < n; i++) {
        if (!skad_global_already_queried(g, &seeds[i]->fingerprint))
            skad_global_mark_queried(g, &seeds[i]->fingerprint);
    }

    for (int d = 0; d < SKAD_D; d++) {
        size_t start = (size_t)d;
        if (start >= n) {
            g->path_done[d]    = true;
            g->completed_paths++;
            continue;
        }

        skad_path_ctx_t *pc = malloc(sizeof(*pc));
        if (!pc) {
            g->path_done[d]    = true;
            g->completed_paths++;
            continue;
        }
        pc->group_id = g->group_id;
        pc->path_idx = d;
        pc->dht      = dht;

        dht_pending_rpc_t *rpc = dht_alloc_rpc(dht);
        if (!rpc) {
            free(pc);
            g->path_done[d]    = true;
            g->completed_paths++;
            continue;
        }

        rpc->kind             = DHT_RPC_FIND_NODE_OP;
        rpc->skad_group_id    = g->group_id;
        rpc->skad_path_idx    = d;
        rpc->find_node_cb     = skad_path_find_node_cb;
        rpc->find_node_ctx    = pc;
        rpc->key = malloc(TRELLIS_FINGERPRINT_LEN);
        if (!rpc->key) {
            dht_free_rpc(rpc); free(pc);
            g->path_done[d]    = true;
            g->completed_paths++;
            continue;
        }
        memcpy(rpc->key, target->bytes, TRELLIS_FINGERPRINT_LEN);
        rpc->key_len = TRELLIS_FINGERPRINT_LEN;

        if (!rpc->timer.loop)
            uv_timer_init(dht->loop, &rpc->timer);
        rpc->timer.data = rpc;
        uv_timer_start(&rpc->timer, dht_rpc_timeout, DHT_RPC_TIMEOUT_MS, 0);

        g->path_rpc_ids[d] = rpc->request_id;

        size_t peer_idx = start;
        if (rpc->queried_count <
                sizeof(rpc->queried)/sizeof(rpc->queried[0]))
            rpc->queried[rpc->queried_count++] =
                seeds[peer_idx]->fingerprint;

        dht_send_find_node_rpc(dht, &seeds[peer_idx]->fingerprint,
                               rpc->request_id, target);
        rpc->pending_responses++;
    }

    if (g->completed_paths == SKAD_D) {
        if (cb) cb(NULL, 0, TRELLIS_ERR_NOT_FOUND, ctx);
        g->active = false;
    }

    return TRELLIS_OK;
}

trellis_sybil_t *trellis_dht_sybil(trellis_dht_t *dht)
{
    return dht ? dht->sybil : NULL;
}

size_t trellis_dht_peer_count(const trellis_dht_t *dht)
{
    if (!dht)
        return 0;

    size_t count = 0;
    for (int b = 0; b < TRELLIS_DHT_BUCKETS; b++)
        count += dht->rt.buckets[b].count;
    return count;
}

size_t trellis_dht_get_peers(const trellis_dht_t *dht,
                              trellis_peer_info_t *out, size_t max_out)
{
    if (!dht || !out || max_out == 0)
        return 0;

    size_t n = 0;
    for (int b = 0; b < TRELLIS_DHT_BUCKETS && n < max_out; b++) {
        for (size_t p = 0; p < dht->rt.buckets[b].count && n < max_out; p++)
            out[n++] = *dht->rt.buckets[b].peers[p];
    }
    return n;
}

const trellis_peer_info_t *trellis_dht_lookup_peer(const trellis_dht_t *dht,
                                                    const trellis_fingerprint_t *fp)
{
    if (!dht || !fp)
        return NULL;

    for (int b = 0; b < TRELLIS_DHT_BUCKETS; b++) {
        for (size_t p = 0; p < dht->rt.buckets[b].count; p++) {
            if (trellis_fingerprint_eq(&dht->rt.buckets[b].peers[p]->fingerprint, fp))
                return dht->rt.buckets[b].peers[p];
        }
    }
    return NULL;
}

const char *trellis_dht_lookup_addr(const trellis_dht_t *dht,
                                     const trellis_fingerprint_t *fp)
{
    if (!dht || !fp)
        return NULL;

    for (int b = 0; b < TRELLIS_DHT_BUCKETS; b++) {
        for (size_t p = 0; p < dht->rt.buckets[b].count; p++) {
            if (trellis_fingerprint_eq(&dht->rt.buckets[b].peers[p]->fingerprint, fp) &&
                dht->rt.buckets[b].peers[p]->addr[0])
                return dht->rt.buckets[b].peers[p]->addr;
        }
    }
    return NULL;
}

void trellis_dht_remove_peer(trellis_dht_t *dht,
                              const trellis_fingerprint_t *fp)
{
    if (!dht || !fp)
        return;

    for (int b = 0; b < TRELLIS_DHT_BUCKETS; b++) {
        for (size_t p = 0; p < dht->rt.buckets[b].count; p++) {
            if (trellis_fingerprint_eq(&dht->rt.buckets[b].peers[p]->fingerprint, fp)) {
                kbucket_remove(&dht->rt.buckets[b], fp);
                dht->peer_count = 0;
                for (int b2 = 0; b2 < TRELLIS_DHT_BUCKETS; b2++)
                    dht->peer_count += dht->rt.buckets[b2].count;

                // Rebuild sibling table — old entries may point to freed memory.
                trellis_peer_info_t *all[TRELLIS_DHT_K * 2];
                size_t all_n = find_closest_peers(&dht->rt,
                    &dht->local_id->fingerprint, all, TRELLIS_DHT_K);
                dht->sibling_count = all_n;
                for (size_t i = 0; i < all_n; i++)
                    dht->sibling_table[i] = all[i];
                return;
            }
        }
    }
}

void trellis_dht_update_peer_seen(trellis_dht_t *dht,
                                   const trellis_fingerprint_t *fp)
{
    if (!dht || !fp)
        return;

    for (int b = 0; b < TRELLIS_DHT_BUCKETS; b++) {
        for (size_t p = 0; p < dht->rt.buckets[b].count; p++) {
            if (trellis_fingerprint_eq(&dht->rt.buckets[b].peers[p]->fingerprint, fp)) {
                dht->rt.buckets[b].peers[p]->last_seen = trellis_now_ms();
                dht->bucket_last_access[b] = trellis_now_ms();
                return;
            }
        }
    }
}

size_t trellis_dht_find_closest(const trellis_dht_t *dht,
                                 const trellis_fingerprint_t *target,
                                 trellis_peer_info_t *out, size_t max_out)
{
    if (!dht || !target || !out || max_out == 0)
        return 0;

    trellis_peer_info_t *closest[TRELLIS_DHT_K];
    size_t n = find_closest_peers(&dht->rt, target, closest,
                                  max_out < TRELLIS_DHT_K ? max_out : TRELLIS_DHT_K);
    for (size_t i = 0; i < n; i++)
        out[i] = *closest[i];
    return n;
}

// --- Eclipse detection ---
#define ECLIPSE_SUBNET_THRESHOLD_PCT  50
#define ECLIPSE_FREEZE_DURATION_MS    60000

void trellis_dht_set_eclipse_cb(trellis_dht_t *dht,
                                 trellis_dht_eclipse_cb cb, void *ctx)
{
    if (!dht) return;
    dht->eclipse_cb     = cb;
    dht->eclipse_cb_ctx = ctx;
}

bool trellis_dht_eclipse_detected(const trellis_dht_t *dht)
{
    return dht ? dht->eclipse_detected : false;
}

void trellis_dht_set_eclipse_enabled(trellis_dht_t *dht, bool enabled)
{
    if (!dht) return;
    dht->eclipse_disabled = !enabled;
    if (!enabled) {
        dht->eclipse_detected = false;
        dht->eclipse_frozen = false;
    }
}

void trellis_dht_disable_sybil(trellis_dht_t *dht)
{
    if (!dht) return;
    trellis_sybil_free(dht->sybil);
    dht->sybil = NULL;

    for (int b = 0; b < TRELLIS_DHT_BUCKETS; b++) {
        k_bucket_t *kb = &dht->rt.buckets[b];
        for (size_t p = 0; p < kb->count; p++)
            kb->peers[p]->quarantined = false;
    }
}

void trellis_dht_check_eclipse(trellis_dht_t *dht)
{
    if (!dht || dht->eclipse_disabled || dht->sibling_count < 4)
        return;

    // Count peers per /16 subnet in the sibling table.
    uint8_t prefixes[TRELLIS_DHT_K][2];
    for (size_t i = 0; i < dht->sibling_count; i++)
        kbucket_extract_prefix16(dht->sibling_table[i]->addr, prefixes[i]);

    size_t max_same = 0;
    for (size_t i = 0; i < dht->sibling_count; i++) {
        size_t same = 0;
        for (size_t j = 0; j < dht->sibling_count; j++) {
            if (prefixes[i][0] == prefixes[j][0] &&
                prefixes[i][1] == prefixes[j][1])
                same++;
        }
        if (same > max_same)
            max_same = same;
    }

    size_t threshold = (dht->sibling_count * ECLIPSE_SUBNET_THRESHOLD_PCT) / 100;
    if (threshold < 2) threshold = 2;

    bool was_detected = dht->eclipse_detected;
    dht->eclipse_detected = (max_same >= threshold);

    if (dht->eclipse_detected && !was_detected) {
        // Freeze routing table updates to prevent further poisoning.
        dht->eclipse_frozen = true;
        dht->eclipse_freeze_until_ms = trellis_now_ms() + ECLIPSE_FREEZE_DURATION_MS;

        fprintf(stderr, "[DHT] eclipse detected: %zu/%zu siblings share /16\n",
                max_same, dht->sibling_count);

        if (dht->eclipse_cb)
            dht->eclipse_cb(dht, dht->eclipse_cb_ctx);
    }

    // Unfreeze once the freeze window expires.
    if (dht->eclipse_frozen && trellis_now_ms() >= dht->eclipse_freeze_until_ms) {
        dht->eclipse_frozen = false;
        dht->eclipse_detected = false;
    }
}

// --- Sybil-aware bucket refresh ---
#define BUCKET_STALE_THRESHOLD_MS  3600000  /* 60 minutes */

typedef struct bucket_refresh_ctx {
    trellis_dht_t *dht;
    int            bucket;
} bucket_refresh_ctx_t;

static void bucket_refresh_cb(const trellis_peer_info_t *peers,
                               size_t count,
                               trellis_err_t err,
                               void *raw_ctx)
{
    bucket_refresh_ctx_t *ctx = (bucket_refresh_ctx_t *)raw_ctx;
    if (!ctx)
        return;
    if (err == TRELLIS_OK && count > 0)
        ctx->dht->bucket_last_access[ctx->bucket] = trellis_now_ms();
    free(ctx);
}

trellis_err_t trellis_dht_refresh_buckets(trellis_dht_t *dht)
{
    if (!dht)
        return TRELLIS_ERR_INVALID_ARG;

    uint64_t now = trellis_now_ms();

    for (int b = 0; b < TRELLIS_DHT_BUCKETS; b++) {
        if (dht->rt.buckets[b].count == 0)
            continue;

        uint64_t last = dht->bucket_last_access[b];
        if (last != 0 && (now - last) < BUCKET_STALE_THRESHOLD_MS)
            continue;

        // Generate a random target ID in this bucket's range.
        trellis_fingerprint_t target;
        randombytes_buf(target.bytes, TRELLIS_FINGERPRINT_LEN);

        /* bucket_index returns (ID_BITS-1) - msb_position, so bucket b
         * corresponds to a first-differing-bit at MSB position ID_BITS-1-b.
         * Copy local_id bits for MSB positions 0..flip_pos-1, then flip
         * bit at flip_pos so the XOR distance lands in bucket b. */
        int flip_pos = TRELLIS_DHT_ID_BITS - 1 - b;
        for (int i = 0; i < flip_pos && i < TRELLIS_FINGERPRINT_LEN * 8; i++) {
            int byte_idx = i / 8;
            int bit_idx  = 7 - (i % 8);
            uint8_t local_bit = (dht->local_id->fingerprint.bytes[byte_idx] >> bit_idx) & 1;
            if (local_bit)
                target.bytes[byte_idx] |=  (1u << bit_idx);
            else
                target.bytes[byte_idx] &= ~(1u << bit_idx);
        }
        if (flip_pos >= 0 && flip_pos < TRELLIS_FINGERPRINT_LEN * 8) {
            int byte_idx = flip_pos / 8;
            int bit_idx  = 7 - (flip_pos % 8);
            target.bytes[byte_idx] ^= (1u << bit_idx);
        }

        bucket_refresh_ctx_t *ctx = malloc(sizeof(bucket_refresh_ctx_t));
        if (!ctx)
            continue;
        ctx->dht = dht;
        ctx->bucket = b;

        trellis_dht_find_node_skad(dht, &target, bucket_refresh_cb, ctx);
    }

    return TRELLIS_OK;
}
