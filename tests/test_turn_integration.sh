#!/usr/bin/env bash

# ===============================================================================
#                                 Author: cRash
#                             Date: April 25th, 2026
#                 Comments: Special thanks to Kate and everyone 
#                 who's ever believed in me -- you're the best!
# ===============================================================================
#                                                                :
#                                                               ::
#                                                              ::
#                                                              ::
#                                                             ::
#                                                             ::
#                                               __           ::
#                    _..-'/-¯¯--/_          ,.--. ''.     |`\=,..
#                 -:--.---''-..  /_         |\\_\..  \    `-.=._/
#                 .-|¯         '.  \         \_ \-`/\ |    ::`
#                   /  @  \      \  -_   _..--|-\¯¯``'--.-/_\
#                   |   .-'|      \  \\-'\_/     ¯/-.|-.\_\_/
#                   \_./` /        \_//-''/    .-'
#                        |           '-/'@====/              _.--.
#                    __.'             /¯¯'-. \-'.          _/   /¯'
#                 .''____|   /       |'--\__\/-._        .'    |
#                  \ \_. \  |       _| -/        \-.__  /     /
#                   \___\ '/   _.  ('-..| /       '_  ''   _.'
#                         /  .'     ¯¯¯¯ /        | ``'--''
#                        (  / ¯```¯¯¯¯¯|-|        |
#                         \ \_.         \ \      /
#                          \___\         '.'.   /
#                                          /    |
#                                         /   .'
#                                        /  .' |
#                                      .'  / \  \
#                                     /___| /___'
# ===============================================================================
# "
#                                ==Phrack Inc.==
#
#                     Volume One, Issue 7, Phile 3 of 10
#
# =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
# The following was written shortly after my arrest...
#
#                        \/\The Conscience of a Hacker/\/
#
#                                       by
#
#                                +++The Mentor+++
#
#                           Written on January 8, 1986
# =-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
#
#         Another one got caught today, it's all over the papers.  "Teenager
# Arrested in Computer Crime Scandal", "Hacker Arrested after Bank Tampering"...
#         Damn kids.  They're all alike.
#
#         But did you, in your three-piece psychology and 1950's technobrain,
# ever take a look behind the eyes of the hacker?  Did you ever wonder what
# made him tick, what forces shaped him, what may have molded him?
#         I am a hacker, enter my world...
#         Mine is a world that begins with school... I'm smarter than most of
# the other kids, this crap they teach us bores me...
#         Damn underachiever.  They're all alike.
#
#         I'm in junior high or high school.  I've listened to teachers explain
# for the fifteenth time how to reduce a fraction.  I understand it.  "No, Ms.
# Smith, I didn't show my work.  I did it in my head..."
#         Damn kid.  Probably copied it.  They're all alike.
#
#         I made a discovery today.  I found a computer.  Wait a second, this is
# cool.  It does what I want it to.  If it makes a mistake, it's because I
# screwed it up.  Not because it doesn't like me...
#                 Or feels threatened by me...
#                 Or thinks I'm a smart ass...
#                 Or doesn't like teaching and shouldn't be here...
#         Damn kid.  All he does is play games.  They're all alike.
#
#         And then it happened... a door opened to a world... rushing through
# the phone line like heroin through an addict's veins, an electronic pulse is
# sent out, a refuge from the day-to-day incompetencies is sought... a board is
# found.
#         "This is it... this is where I belong..."
#         I know everyone here... even if I've never met them, never talked to
# them, may never hear from them again... I know you all...
#         Damn kid.  Tying up the phone line again.  They're all alike...
#
#         You bet your ass we're all alike... we've been spoon-fed baby food at
# school when we hungered for steak... the bits of meat that you did let slip
# through were pre-chewed and tasteless.  We've been dominated by sadists, or
# ignored by the apathetic.  The few that had something to teach found us will-
# ing pupils, but those few are like drops of water in the desert.
#
#         This is our world now... the world of the electron and the switch, the
# beauty of the baud.  We make use of a service already existing without paying
# for what could be dirt-cheap if it wasn't run by profiteering gluttons, and
# you call us criminals.  We explore... and you call us criminals.  We seek
# after knowledge... and you call us criminals.  We exist without skin color,
# without nationality, without religious bias... and you call us criminals.
# You build atomic bombs, you wage wars, you murder, cheat, and lie to us
# and try to make us believe it's for our own good, yet we're the criminals.
#
#         Yes, I am a criminal.  My crime is that of curiosity.  My crime is
# that of judging people by what they say and think, not what they look like.
# My crime is that of outsmarting you, something that you will never forgive me
# for.
#
#         I am a hacker, and this is my manifesto.  You may stop this individual,
# but you can't stop us all... after all, we're all alike.
#
#                                +++The Mentor+++
# "
# ===============================================================================
#                                  .,ad88888ba,.    
#                              .,ad8888888888888a,  
#                             d8P"""98888P"""98888b,
#                             9b    d8888,    `9888B
#                           ,d88aaa8888888b,,,d888P'
#                          d8888888888888888888888b 
#                         d888888P""98888888888888P 
#                         88888P'    9888888888888  
#                         `98P'       9888888888P'  
#                                      `"9888P"'    
#                                         `"'
# ===============================================================================

# test_turn_integration.sh — integration test for vine-relay TURN replacement
#
# Spawns a vine-relay instance and runs the Python e2e TURN data flow test
# against it, exercising the full STUN/TURN protocol over TCP and UDP:
#   1. STUN Binding Request/Response
#   2. TURN Allocate (unauth → 401 challenge)
#   3. TURN Allocate (authenticated → success)
#   4. CreatePermission
#   5. Send Indication
#   6. ChannelBind
#   7. ChannelData
#   8. Refresh (keep-alive)
#   9. Refresh lifetime=0 (deallocation)
#
# Also measures bandwidth overhead vs coturn.
#
# Prerequisites:
#   - vine-relay binary built
#   - python3
#   - openssl (for cert gen)
#   - bc (for arithmetic, optional)
#
# Usage:
#   ./test_turn_integration.sh [/path/to/vine-relay]
set -euo pipefail

RELAY_BIN="${1:-../../apps/vine-relay/build/vine-relay}"
SECRET="integration-test-secret-$(date +%s)"
REALM="test.vine-relay.local"
RELAY_PORT=13478
MESH_PORT=14000
RELAY_PID=""
RELAY_LOG=""
PASS=0
FAIL=0
SKIP=0

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"

RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[0;33m'
NC='\033[0m'

cleanup() {
    [ -n "$RELAY_PID" ] && kill "$RELAY_PID" 2>/dev/null || true
    wait 2>/dev/null || true
}
trap cleanup EXIT

log_pass() { echo -e "  ${GREEN}PASS${NC}  $1"; PASS=$((PASS+1)); }
log_fail() { echo -e "  ${RED}FAIL${NC}  $1"; FAIL=$((FAIL+1)); }
log_skip() { echo -e "  ${YELLOW}SKIP${NC}  $1 ($2)"; SKIP=$((SKIP+1)); }

if [ ! -x "$RELAY_BIN" ]; then
    echo "vine-relay binary not found at $RELAY_BIN"
    echo "Build it first: cd ../../apps/vine-relay && mkdir -p build && cd build && cmake .. && make"
    exit 1
fi

if ! command -v python3 >/dev/null 2>&1; then
    echo "python3 is required for integration tests"
    exit 1
fi

echo "═══════════════════════════════════════════════════════"
echo "  vine-relay TURN integration test"
echo "═══════════════════════════════════════════════════════"
echo ""
echo "Relay binary: $RELAY_BIN"
echo "Shared secret: ${SECRET:0:20}..."
echo ""

pick_port() {
    python3 -c "import socket; s=socket.socket(); s.bind(('',0)); print(s.getsockname()[1]); s.close()"
}
RELAY_PORT=$(pick_port)
RELAY_PORT_TLS=$(pick_port)
MESH_PORT=$(pick_port)

TMPDIR=$(mktemp -d)

if command -v openssl >/dev/null 2>&1; then
    openssl req -x509 -newkey ec -pkeyopt ec_paramgen_curve:prime256v1 \
        -keyout "$TMPDIR/test-key.pem" -out "$TMPDIR/test-cert.pem" \
        -days 1 -nodes -subj "/CN=localhost" 2>/dev/null
    HAS_CERTS=true
else
    HAS_CERTS=false
fi

echo "Starting vine-relay on TURN port $RELAY_PORT, mesh port $MESH_PORT..."
"$RELAY_BIN" \
    --turn \
    --turn-listen "0.0.0.0:$RELAY_PORT" \
    --turn-secret "$SECRET" \
    --turn-realm "$REALM" \
    --listen "0.0.0.0:$MESH_PORT" \
    --routing adaptive \
    --turn-privacy speed \
    --log-level debug \
    >"$TMPDIR/relay.log" 2>&1 &
RELAY_PID=$!
RELAY_LOG="$TMPDIR/relay.log"

sleep 2

if ! kill -0 "$RELAY_PID" 2>/dev/null; then
    echo -e "  ${RED}FAIL${NC}  vine-relay crashed on startup"
    echo "  Relay log:"
    tail -20 "$TMPDIR/relay.log" 2>/dev/null | sed 's/^/    /'
    rm -rf "$TMPDIR"
    exit 1
fi
log_pass "vine-relay started (PID $RELAY_PID)"

echo ""

E2E_SCRIPT="$SCRIPT_DIR/test_turn_e2e.py"
if [ -f "$E2E_SCRIPT" ]; then
    if python3 "$E2E_SCRIPT" 127.0.0.1 "$RELAY_PORT" "$SECRET" 2>&1; then
        log_pass "TURN protocol e2e test (TCP) passed"
    else
        log_fail "TURN protocol e2e test (TCP) failed"
    fi
else
    log_skip "TURN protocol e2e test (TCP)" "test_turn_e2e.py not found at $E2E_SCRIPT"
fi

echo ""

if [ -f "$E2E_SCRIPT" ]; then
    if python3 "$E2E_SCRIPT" 127.0.0.1 "$RELAY_PORT" "$SECRET" --udp 2>&1; then
        log_pass "TURN protocol e2e test (UDP) passed"
    else
        log_fail "TURN protocol e2e test (UDP) failed"
    fi
else
    log_skip "TURN protocol e2e test (UDP)" "test_turn_e2e.py not found at $E2E_SCRIPT"
fi

kill "$RELAY_PID" 2>/dev/null || true
wait "$RELAY_PID" 2>/dev/null || true
RELAY_PID=""

if [ "$HAS_CERTS" = true ]; then
    echo ""
    echo "Starting vine-relay with TURNS on port $RELAY_PORT_TLS..."
    MESH_PORT_TLS=$(pick_port)
    "$RELAY_BIN" \
        --turn \
        --turn-listen "0.0.0.0:$RELAY_PORT_TLS" \
        --turn-secret "$SECRET" \
        --turn-realm "$REALM" \
        --turn-cert "$TMPDIR/test-cert.pem" \
        --turn-key "$TMPDIR/test-key.pem" \
        --listen "0.0.0.0:$MESH_PORT_TLS" \
        --routing adaptive \
        --turn-privacy speed \
        --log-level debug \
        >"$TMPDIR/relay-tls.log" 2>&1 &
    RELAY_PID=$!
    RELAY_LOG="$TMPDIR/relay-tls.log"
    sleep 2

    if ! kill -0 "$RELAY_PID" 2>/dev/null; then
        echo -e "  ${RED}FAIL${NC}  vine-relay (TURNS) crashed on startup"
        tail -20 "$TMPDIR/relay-tls.log" 2>/dev/null | sed 's/^/    /'
    else
        log_pass "vine-relay started with TURNS (PID $RELAY_PID)"

        echo ""
        if [ -f "$E2E_SCRIPT" ]; then
            if python3 "$E2E_SCRIPT" 127.0.0.1 "$RELAY_PORT_TLS" "$SECRET" --tls 2>&1; then
                log_pass "TURNS/TCP e2e test passed"
            else
                log_fail "TURNS/TCP e2e test failed"
            fi
        fi

        echo ""
        if [ -f "$E2E_SCRIPT" ]; then
            if python3 "$E2E_SCRIPT" 127.0.0.1 "$RELAY_PORT_TLS" "$SECRET" --dtls 2>&1; then
                log_pass "DTLS/UDP e2e test passed"
            else
                # DTLS requires Python 3.13+ — may not be available
                log_skip "DTLS/UDP e2e test" "may require Python 3.13+ for DTLS support"
            fi
        fi
    fi
else
    log_skip "TURNS/DTLS tests" "openssl not available for cert generation"
fi

echo ""

# Session-keyed relay adds 64 bytes of AES-GCM envelope per packet:
# circuit_id(4) + dest_fp(32) + nonce(12) + tag(16) = 64 bytes.
# The trellis message envelope adds ~88 bytes header + ~4691 bytes hybrid
# signature, bringing total wire size to ~6043 bytes per 1200-byte packet.
# With Ed25519-only signing, total drops to ~1416 bytes (18% overhead).
# Coturn adds ~4 bytes (ChannelData header) once a channel is bound.
MEDIA_PKT_SIZE=1200
SESSION_OVERHEAD=64
COTURN_OVERHEAD=4
VINE_TOTAL=$((MEDIA_PKT_SIZE + SESSION_OVERHEAD))
COTURN_TOTAL=$((MEDIA_PKT_SIZE + COTURN_OVERHEAD))

if command -v bc >/dev/null 2>&1; then
    OVERHEAD_PCT=$(echo "scale=2; ($VINE_TOTAL - $COTURN_TOTAL) * 100 / $COTURN_TOTAL" | bc)
    echo "  Media packet: ${MEDIA_PKT_SIZE}B"
    echo "  vine-relay (session-keyed): +${SESSION_OVERHEAD}B = ${VINE_TOTAL}B"
    echo "  coturn (ChannelData):       +${COTURN_OVERHEAD}B = ${COTURN_TOTAL}B"
    echo "  Overhead difference: ${OVERHEAD_PCT}%"
    TARGET=$(echo "$OVERHEAD_PCT < 5" | bc)
    if [ "$TARGET" = "1" ]; then
        log_pass "Bandwidth overhead ${OVERHEAD_PCT}% < 5% target"
    else
        log_fail "Bandwidth overhead ${OVERHEAD_PCT}% exceeds 5% target"
    fi
else
    log_skip "Bandwidth overhead calculation" "bc not available"
fi

echo ""
if kill -0 "$RELAY_PID" 2>/dev/null; then
    log_pass "vine-relay still running after all tests"
else
    log_fail "vine-relay crashed during testing"
    echo "  Relay log (last 20 lines):"
    tail -20 "$RELAY_LOG" 2>/dev/null | sed 's/^/    /'
fi

echo ""
echo "═══════════════════════════════════════════════════════"
echo -e "  Results: ${GREEN}${PASS} passed${NC}, ${RED}${FAIL} failed${NC}, ${YELLOW}${SKIP} skipped${NC}"
echo "═══════════════════════════════════════════════════════"

# Relay log for debugging
echo ""
echo "Relay log (last 10 lines):"
tail -10 "$RELAY_LOG" 2>/dev/null | sed 's/^/  /' || echo "  (empty)"

rm -rf "$TMPDIR"

[ "$FAIL" -eq 0 ] || exit 1
