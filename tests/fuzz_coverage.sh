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

# fuzz_coverage.sh — Build fuzz harnesses with coverage instrumentation
#                     and run each for a fixed duration to produce
#                     per-target line/branch coverage reports.
#
# Prerequisites:
#   - clang with -fprofile-instr-generate -fcoverage-mapping support
#   - llvm-profdata and llvm-cov on PATH
#   - libtrellis built with coverage: cmake -DCMAKE_C_FLAGS="--coverage" ..
#
# Usage:
#   ./fuzz_coverage.sh [duration_seconds] [target]
#     duration: per-target fuzz run time (default: 60)
#     target:   one of the TARGETS below, or "all" (default)
#
# Output:
#   coverage/<target>/report.txt    — line/branch coverage summary
#   coverage/<target>/report.html   — HTML detail (if llvm-cov show -format=html works)
#   coverage/summary.txt            — combined summary

set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
BUILD_DIR="${SCRIPT_DIR}/../build"
INC_DIR="${SCRIPT_DIR}/../include"
COVERAGE_DIR="${SCRIPT_DIR}/coverage"

CC="${CC:-clang}"

DURATION="${1:-60}"
TARGET_ARG="${2:-all}"

# Coverage + fuzzer flags
CFLAGS="-fsanitize=fuzzer,address,undefined -O1 -g"
CFLAGS+=" -fprofile-instr-generate -fcoverage-mapping"
CFLAGS+=" -I${INC_DIR} -I${SCRIPT_DIR}/../src"
LDFLAGS="-L${BUILD_DIR} -ltrellis -lsodium -loqs -luv -lm"
LDFLAGS+=" -fprofile-instr-generate"

TARGETS=(
    handshake
    morph_decode
    dht_rpc
    onion_peel
    greenhouse_msg
    surb_unpack
    message_parse
    naming_record
    capabilities
    stun_parse
    turn_server
    exit_stream_payload
    relay_descriptor
    ratchet_decrypt
    gossip_receive
    circuit_create
)

mkdir -p "${COVERAGE_DIR}"

build_target() {
    local target="$1"
    local out="${COVERAGE_DIR}/fuzz_${target}"
    echo "[cov] Building fuzz_${target} with coverage..."
    ${CC} ${CFLAGS} \
        -DFUZZ_TARGET="${target}" \
        "${SCRIPT_DIR}/fuzz_harnesses.c" \
        -o "${out}" \
        ${LDFLAGS} 2>&1 || {
        echo "[cov] WARN: Failed to build fuzz_${target}"
        return 1
    }
}

run_target() {
    local target="$1"
    local binary="${COVERAGE_DIR}/fuzz_${target}"
    local corpus="${SCRIPT_DIR}/corpus/${target}"
    local profraw="${COVERAGE_DIR}/${target}.profraw"
    local profdata="${COVERAGE_DIR}/${target}.profdata"
    local report_dir="${COVERAGE_DIR}/${target}"

    if [[ ! -x "${binary}" ]]; then
        echo "[cov] Skip ${target}: binary not found"
        return 0
    fi

    mkdir -p "${corpus}" "${report_dir}"

    echo "[cov] Running fuzz_${target} for ${DURATION}s..."
    LLVM_PROFILE_FILE="${profraw}" \
        "${binary}" "${corpus}" \
        -max_total_time="${DURATION}" \
        -print_final_stats=1 \
        2>&1 | tail -20 || true

    if [[ ! -f "${profraw}" ]]; then
        echo "[cov] No profile data for ${target}"
        return 0
    fi

    # Merge raw profile
    llvm-profdata merge -sparse "${profraw}" -o "${profdata}"

    # Generate text summary
    llvm-cov report "${binary}" \
        -instr-profile="${profdata}" \
        -ignore-filename-regex='(tests/|_deps/)' \
        > "${report_dir}/report.txt" 2>&1 || true

    # Generate HTML report (best-effort)
    llvm-cov show "${binary}" \
        -instr-profile="${profdata}" \
        -format=html \
        -output-dir="${report_dir}/html" \
        -ignore-filename-regex='(tests/|_deps/)' \
        2>/dev/null || true

    # Print summary
    echo "[cov] ${target}:"
    head -20 "${report_dir}/report.txt" 2>/dev/null || echo "  (no report)"
    echo ""
}

# Build phase
echo "=== Building fuzz harnesses with coverage ==="
if [[ "${TARGET_ARG}" == "all" ]]; then
    for t in "${TARGETS[@]}"; do
        build_target "${t}" || true
    done
else
    build_target "${TARGET_ARG}" || exit 1
fi

# Run phase
echo ""
echo "=== Running fuzz harnesses (${DURATION}s each) ==="
if [[ "${TARGET_ARG}" == "all" ]]; then
    for t in "${TARGETS[@]}"; do
        run_target "${t}"
    done
else
    run_target "${TARGET_ARG}"
fi

# Combined summary
echo "=== Coverage Summary ===" | tee "${COVERAGE_DIR}/summary.txt"
for t in "${TARGETS[@]}"; do
    report="${COVERAGE_DIR}/${t}/report.txt"
    if [[ -f "${report}" ]]; then
        echo "--- ${t} ---" | tee -a "${COVERAGE_DIR}/summary.txt"
        # Extract the TOTAL line from llvm-cov report
        grep -E "^TOTAL|^---" "${report}" 2>/dev/null | tee -a "${COVERAGE_DIR}/summary.txt" || true
    fi
done

echo ""
echo "[cov] Reports in ${COVERAGE_DIR}/"
echo "[cov] Per-target HTML reports in ${COVERAGE_DIR}/<target>/html/ (if generated)"
