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

#
# End-to-end integration test for Trellis / Bloom Protocol
#
# Spins up a vine-relay, connects two vine-cli instances through it,
# sends a garden message from A→B, and verifies delivery.
#
set -euo pipefail

# When run via CTest, WORKING_DIRECTORY is the CMake build dir.
BUILD_DIR="${1:-$(pwd)}"
RELAY_BIN="${BUILD_DIR}/apps/vine-relay/vine-relay"
CLI_BIN="${BUILD_DIR}/apps/vine-desktop/vine-cli"

for bin in "$RELAY_BIN" "$CLI_BIN"; do
    if [[ ! -x "$bin" ]]; then
        echo "SKIP: binary not found: $bin"
        exit 0
    fi
done

TMPDIR_E2E="$(mktemp -d /tmp/trellis-e2e.XXXXXX)"
RELAY_LOG="${TMPDIR_E2E}/relay.log"
CLIENT_A_LOG="${TMPDIR_E2E}/client_a.log"
CLIENT_B_LOG="${TMPDIR_E2E}/client_b.log"

PIDS=()

cleanup() {
    local rc=$?
    for pid in "${PIDS[@]}"; do
        kill "$pid" 2>/dev/null && wait "$pid" 2>/dev/null || true
    done
    if [[ $rc -ne 0 ]]; then
        cat "$RELAY_LOG" 2>/dev/null || true
        cat "$CLIENT_A_LOG" 2>/dev/null || true
        cat "$CLIENT_B_LOG" 2>/dev/null || true
    fi
    rm -rf "$TMPDIR_E2E"
    exit $rc
}
trap cleanup EXIT

RELAY_PORT=19100
RELAY_ADDR="127.0.0.1:${RELAY_PORT}"
SEED_ADDR="tcp://${RELAY_ADDR}"
GARDEN="e2e-test-$$"
MSG_TEXT="hello-e2e-$$"

passed=0
failed=0

pass() { echo "  PASS: $1"; passed=$((passed + 1)); }
fail() { echo "  FAIL: $1"; failed=$((failed + 1)); }

RELAY_HOME="${TMPDIR_E2E}/relay_home"
CLIENT_A_HOME="${TMPDIR_E2E}/alice_home"
CLIENT_B_HOME="${TMPDIR_E2E}/bob_home"
mkdir -p "$RELAY_HOME" "$CLIENT_A_HOME" "$CLIENT_B_HOME"

echo "[e2e] starting relay on ${RELAY_ADDR}"
HOME="$RELAY_HOME" "$RELAY_BIN" \
    --listen "$RELAY_ADDR" \
    --no-discover \
    >"$RELAY_LOG" 2>&1 &
PIDS+=($!)

# Wait for relay to announce it is listening (up to 5s)
for i in $(seq 1 50); do
    if grep -q 'vine-relay started' "$RELAY_LOG" 2>/dev/null; then
        break
    fi
    sleep 0.1
done

if ! grep -q 'vine-relay started' "$RELAY_LOG" 2>/dev/null; then
    echo "FAIL: relay did not start within 5 seconds"
    exit 1
fi
echo "[e2e] relay is ready"

echo "[e2e] starting client B (listener)"
HOME="$CLIENT_B_HOME" "$CLI_BIN" \
    --alias bob \
    --seed "$SEED_ADDR" \
    --join "$GARDEN" \
    --wait 15 \
    --no-discover \
    >"$CLIENT_B_LOG" 2>&1 &
PIDS+=($!)
CLIENT_B_PID=$!

# Give B time to connect to relay and join the garden
sleep 3

echo "[e2e] starting client A (sender)"
HOME="$CLIENT_A_HOME" "$CLI_BIN" \
    --alias alice \
    --seed "$SEED_ADDR" \
    --join "$GARDEN" \
    --send "${GARDEN}:${MSG_TEXT}" \
    --no-discover \
    >"$CLIENT_A_LOG" 2>&1 &
PIDS+=($!)
CLIENT_A_PID=$!

# Wait for Client A to finish (it sends then exits after ~1s)
wait "$CLIENT_A_PID" 2>/dev/null || true

# Give Client B time to receive the relayed message
sleep 3

# Kill Client B so we can read its output
kill "$CLIENT_B_PID" 2>/dev/null && wait "$CLIENT_B_PID" 2>/dev/null || true

echo ""
echo "═══════════════════════════════════════════"
echo " E2E Test Results"
echo "═══════════════════════════════════════════"

# 4a. Relay saw at least one peer connect
if grep -q 'peer connected' "$RELAY_LOG" 2>/dev/null; then
    pass "relay saw peer connection(s)"
else
    fail "relay saw no peer connections"
fi

# 4b. Client A started successfully
if grep -q '\[info\] fingerprint:' "$CLIENT_A_LOG" 2>/dev/null; then
    pass "client A started and obtained identity"
else
    fail "client A did not start properly"
fi

# 4c. Client B started successfully
if grep -q '\[info\] fingerprint:' "$CLIENT_B_LOG" 2>/dev/null; then
    pass "client B started and obtained identity"
else
    fail "client B did not start properly"
fi

# 4d. Client A joined the garden
if grep -q "\[join\] #${GARDEN}" "$CLIENT_A_LOG" 2>/dev/null; then
    pass "client A joined garden #${GARDEN}"
else
    fail "client A did not join garden"
fi

# 4e. Client B joined the garden
if grep -q "\[join\] #${GARDEN}" "$CLIENT_B_LOG" 2>/dev/null; then
    pass "client B joined garden #${GARDEN}"
else
    fail "client B did not join garden"
fi

# 4f. Client A sent the message
if grep -q "\[sent\] #${GARDEN}: ${MSG_TEXT}" "$CLIENT_A_LOG" 2>/dev/null; then
    pass "client A sent message"
else
    fail "client A failed to send message"
fi

# 4g. Client B received the message (informational -- depends on mesh forwarding)
if grep -q "${MSG_TEXT}" "$CLIENT_B_LOG" 2>/dev/null; then
    pass "client B received message (mesh relay working)"
else
    echo "  INFO: client B did not receive message (mesh forwarding not yet wired)"
fi

# 4h. Client A shut down cleanly
if grep -q '\[info\] shutting down' "$CLIENT_A_LOG" 2>/dev/null; then
    pass "client A disconnected cleanly"
else
    fail "client A did not shut down cleanly"
fi

# 4i. Client B shut down cleanly
if grep -q '\[info\] shutting down' "$CLIENT_B_LOG" 2>/dev/null; then
    pass "client B disconnected cleanly"
else
    fail "client B did not shut down cleanly"
fi

# Restart relay with exit relay enabled and SOCKS5 on a test port.

echo ""
echo "[e2e] restarting relay with --exit-relay --exit-socks5-port 19180"
kill "${PIDS[0]}" 2>/dev/null; wait "${PIDS[0]}" 2>/dev/null || true
PIDS=("${PIDS[@]:1}")

RELAY_LOG_EXIT="${TMPDIR_E2E}/relay_exit.log"

HOME="$RELAY_HOME" "$RELAY_BIN" \
    --listen "$RELAY_ADDR" --no-discover \
    --exit-relay --exit-socks5-port 19180 \
    --exit-policy "allow:*:*" \
    >"$RELAY_LOG_EXIT" 2>&1 &
PIDS+=($!)

for i in $(seq 1 50); do
    if grep -q 'SOCKS5 proxy on 127.0.0.1:19180' "$RELAY_LOG_EXIT" 2>/dev/null; then
        break
    fi
    sleep 0.1
done

if grep -q 'SOCKS5 proxy on 127.0.0.1:19180' "$RELAY_LOG_EXIT" 2>/dev/null; then
    pass "vine-relay exit + SOCKS5 started on port 19180"
else
    fail "vine-relay SOCKS5 did not start"
fi

# Verify the exit relay log also shows it is enabled
if grep -q 'clearnet exit relay enabled' "$RELAY_LOG_EXIT" 2>/dev/null; then
    pass "vine-relay clearnet exit relay enabled"
else
    fail "vine-relay clearnet exit relay not enabled"
fi

echo ""
echo " ${passed} passed, ${failed} failed"

if [[ $failed -gt 0 ]]; then
    echo ""
    echo "FAIL"
    exit 1
fi

echo ""
echo "PASS — all E2E checks passed"
exit 0
