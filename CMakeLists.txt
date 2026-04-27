find_package(Threads REQUIRED)

set(TRELLIS_SOURCES
    src/types.c
    src/error.c
    src/platform.c

    # Layer 1: Crypto Core
    src/crypto/init.c
    src/crypto/kdf.c
    src/crypto/symmetric.c
    src/crypto/sign.c
    src/crypto/kem.c
    src/crypto/identity.c
    src/crypto/handshake.c
    src/crypto/ratchet.c
    src/crypto/treekem.c
    src/crypto/probe_resist.c
    src/crypto/elligator2.c
    src/crypto/key_transparency.c
    src/crypto/conn_pow.c

    # Layer 0: Transport
    src/transport/interface.c
    src/transport/tcp.c
    src/transport/udp.c
    src/transport/websocket.c
    src/transport/multiplexer.c
    src/transport/tls.c
    src/transport/pt_subprocess.c
    src/transport/camouflage.c
    src/transport/quic.c
    src/transport/ble.c
    src/transport/lora.c
    src/transport/webrtc_stego.c

    # Layer 2: Mesh Fabric
    src/mesh/dht.c
    src/mesh/routing.c
    src/mesh/gossip.c
    src/mesh/relay.c
    src/mesh/topology.c
    src/mesh/nat.c
    src/mesh/pir.c
    src/mesh/incentive.c
    src/mesh/guard.c
    src/mesh/bootstrap.c
    src/mesh/canary.c
    src/mesh/naming.c
    src/mesh/exit_relay.c
    src/mesh/deaddrop.c
    src/mesh/flow.c
    src/mesh/clock_sync.c
    src/mesh/relay_descriptor.c
    src/mesh/doh.c
    src/mesh/circuit_pool.c
    src/mesh/grove.c
    src/mesh/bridge.c
    src/mesh/canopy.c

    # Layer 3: Morph Engine (traffic shaping only — disguise codecs removed)
    src/morph/engine.c
    src/morph/mutation.c
    src/morph/padding.c
    src/morph/timing.c

    # Layer 4: Protocol
    src/protocol/messages.c
    src/protocol/garden.c
    src/protocol/permissions.c
    src/protocol/capabilities.c
    src/protocol/extensions.c
    src/protocol/webrtc_media.c

    # Sybil Resistance
    src/sybil/vdf.c
    src/sybil/trust.c
    src/sybil/behavior.c
    src/sybil/personhood.c
    src/sybil/hco.c
    src/sybil/composite.c
    src/sybil/bridge_trust.c

    # Greenhouse
    src/greenhouse/service.c
    src/greenhouse/descriptor.c
    src/greenhouse/addressing.c
    src/greenhouse/introduction.c
    src/greenhouse/rendezvous.c
    src/greenhouse/onion.c
    src/greenhouse/proxy.c
    src/greenhouse/reverse_proxy.c
    src/greenhouse/deadman.c
    src/greenhouse/surb.c

    # TURN/STUN
    src/turn/stun.c
    src/turn/turn.c

    # SDK
    src/sdk/client.c
    src/sdk/events.c
    src/sdk/config.c
    src/sdk/discovery.c
    src/sdk/identity_store.c
    src/sdk/metrics.c
    src/sdk/mobile.c
    src/sdk/identity_recovery.c
    src/sdk/control.c
)

set(TRELLIS_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)

# Sybil resistance optional features (stubs active when OFF)
option(TRELLIS_WITH_POP "Enable real Proof-of-Personhood (default: stub)" OFF)
option(TRELLIS_WITH_HCO "Enable real Human Challenge Oracle (default: stub)" OFF)

# Relay incentive: Lightning receipt redemption via lncli + BTCPay
option(TRELLIS_WITH_LIGHTNING "Enable Lightning receipt redemption (requires lncli at runtime)" OFF)

# Transport options (stubs active when OFF)
option(TRELLIS_WITH_QUIC         "Enable QUIC transport (requires ngtcp2)"        OFF)
option(TRELLIS_WITH_DTN          "Enable LoRa/BLE DTN transports"                 OFF)
option(TRELLIS_WITH_BLUEZ        "Enable BLE BlueZ D-Bus support (Linux)"         OFF)
option(TRELLIS_WITH_WEBRTC_STEGO "Enable WebRTC steganographic transport"         OFF)

# liboqs FetchContent exposes src/ as its include dir, but the generated
# oqs/oqs.h header lives in the build tree's include/ directory.
set(LIBOQS_GENERATED_INCLUDE "${CMAKE_BINARY_DIR}/_deps/liboqs-build/include")

# ── Helper: configure a trellis library target ──────────────────────────────
function(trellis_configure_target target_name)
    target_include_directories(${target_name}
        PUBLIC  ${TRELLIS_INCLUDE_DIR}
        PUBLIC  ${LIBOQS_GENERATED_INCLUDE}
        PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src
    )
    target_link_libraries(${target_name}
        PUBLIC  oqs PkgConfig::SODIUM uv_a m Threads::Threads
    )
    if(TARGET msgpack-c)
        target_link_libraries(${target_name} PRIVATE msgpack-c)
    endif()
    if(TARGET tinycbor)
        target_link_libraries(${target_name} PRIVATE tinycbor)
    endif()
    if(TRELLIS_WITH_TLS AND TARGET mbedtls)
        target_compile_definitions(${target_name} PRIVATE TRELLIS_WITH_TLS)
        target_link_libraries(${target_name} PRIVATE mbedtls mbedx509 mbedcrypto)
    endif()
    if(TRELLIS_WITH_POP)
        target_compile_definitions(${target_name} PRIVATE TRELLIS_WITH_POP)
    endif()
    if(TRELLIS_WITH_HCO)
        target_compile_definitions(${target_name} PRIVATE TRELLIS_WITH_HCO)
    endif()
    if(TRELLIS_WITH_LIGHTNING)
        target_compile_definitions(${target_name} PRIVATE TRELLIS_WITH_LIGHTNING)
    endif()
    if(TRELLIS_WITH_QUIC)
        target_compile_definitions(${target_name} PUBLIC TRELLIS_WITH_QUIC NGTCP2_STATICLIB NGTCP2_CRYPTO_OPENSSL)
        target_link_libraries(${target_name} PRIVATE ngtcp2_static ngtcp2_crypto_quictls_static)
    endif()
    if(TRELLIS_WITH_DTN)
        target_compile_definitions(${target_name} PUBLIC TRELLIS_WITH_DTN)
    endif()
    if(TRELLIS_WITH_BLUEZ)
        target_compile_definitions(${target_name} PUBLIC TRELLIS_WITH_BLUEZ)
    endif()
    if(TRELLIS_WITH_WEBRTC_STEGO)
        target_compile_definitions(${target_name} PUBLIC TRELLIS_WITH_WEBRTC_STEGO)
        target_link_libraries(${target_name} PRIVATE LibDataChannel::LibDataChannelStatic opus)
    endif()
endfunction()

# ── Static library ───────────────────────────────────────────────────────────
if(TRELLIS_BUILD_STATIC)
    add_library(trellis_static STATIC ${TRELLIS_SOURCES})
    set_target_properties(trellis_static PROPERTIES OUTPUT_NAME trellis)
    trellis_configure_target(trellis_static)
endif()

# ── Shared library ───────────────────────────────────────────────────────────
if(TRELLIS_BUILD_SHARED)
    add_library(trellis_shared SHARED ${TRELLIS_SOURCES})
    set_target_properties(trellis_shared PROPERTIES OUTPUT_NAME trellis)
    trellis_configure_target(trellis_shared)
endif()

# ── Alias for consumers ─────────────────────────────────────────────────────
if(TRELLIS_BUILD_STATIC)
    add_library(trellis ALIAS trellis_static)
elseif(TRELLIS_BUILD_SHARED)
    add_library(trellis ALIAS trellis_shared)
endif()

# ── Tests ────────────────────────────────────────────────────────────────────
if(TRELLIS_BUILD_TESTS)
    enable_testing()

    set(TRELLIS_TESTS
        test_crypto
        test_elligator2
        test_mesh
        test_morph
        test_greenhouse
        test_client
        test_protocol
        test_treekem
        test_integration
        test_onion
        test_sybil
        test_adversarial
        test_kat
        test_turn
        test_rhizome
        test_exit_relay
    )

    foreach(test_name ${TRELLIS_TESTS})
        add_executable(${test_name} tests/${test_name}.c)
        target_link_libraries(${test_name} PRIVATE trellis oqs PkgConfig::SODIUM)
        target_include_directories(${test_name} PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}/src
            ${CMAKE_CURRENT_SOURCE_DIR}/tests
        )
        add_test(NAME ${test_name} COMMAND ${test_name})
        set_tests_properties(${test_name} PROPERTIES TIMEOUT 90)
    endforeach()

    # Transport tests need direct libuv linkage for uv_loop / uv_timer
    add_executable(test_transport tests/test_transport.c)
    target_link_libraries(test_transport PRIVATE trellis uv_a oqs PkgConfig::SODIUM)
    target_include_directories(test_transport PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${CMAKE_CURRENT_SOURCE_DIR}/tests
    )
    add_test(NAME test_transport COMMAND test_transport)
    set_tests_properties(test_transport PROPERTIES TIMEOUT 90)

    # Tests that use libuv directly also need explicit uv_a linkage
    # to avoid link-order issues on some toolchains
    foreach(uv_test test_adversarial test_mesh test_morph test_rhizome test_exit_relay)
        target_link_libraries(${uv_test} PRIVATE uv_a)
    endforeach()

    # E2E integration test (shell harness — needs vine-relay + vine-cli)
    add_test(NAME test_e2e COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/tests/test_e2e.sh
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
    set_tests_properties(test_e2e PROPERTIES TIMEOUT 60)

    # TURN integration test (shell harness — needs vine-relay binary)
    add_test(NAME test_turn_integration
        COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/tests/test_turn_integration.sh
            ${CMAKE_BINARY_DIR}/apps/vine-relay/vine-relay
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
    set_tests_properties(test_turn_integration PROPERTIES TIMEOUT 120)

    # Platform abstraction tests
    add_executable(test_platform tests/test_platform.c)
    target_link_libraries(test_platform PRIVATE trellis oqs PkgConfig::SODIUM)
    target_include_directories(test_platform PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${CMAKE_CURRENT_SOURCE_DIR}/tests)
    add_test(NAME test_platform COMMAND test_platform)
    set_tests_properties(test_platform PROPERTIES TIMEOUT 30)

    add_executable(test_threading_portable tests/test_threading_portable.c)
    target_link_libraries(test_threading_portable PRIVATE trellis uv_a oqs PkgConfig::SODIUM)
    target_include_directories(test_threading_portable PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${CMAKE_CURRENT_SOURCE_DIR}/tests)
    add_test(NAME test_threading_portable COMMAND test_threading_portable)
    set_tests_properties(test_threading_portable PROPERTIES TIMEOUT 30)

    # Vine-relay config parsing test (standalone, links vine-relay sources directly)
    add_executable(test_vine_config
        ${CMAKE_SOURCE_DIR}/apps/vine-relay/tests/test_vine_config.c
        ${CMAKE_SOURCE_DIR}/apps/vine-relay/src/config.c
        ${CMAKE_SOURCE_DIR}/apps/vine-relay/src/log.c)
    target_include_directories(test_vine_config PRIVATE
        ${CMAKE_SOURCE_DIR}/apps/vine-relay/src
        ${CMAKE_CURRENT_SOURCE_DIR}/src)
    target_link_libraries(test_vine_config PRIVATE trellis)
    add_test(NAME test_vine_config COMMAND test_vine_config)
    set_tests_properties(test_vine_config PROPERTIES TIMEOUT 90)
endif()

# ── Fuzz harnesses ───────────────────────────────────────────────────────
option(TRELLIS_BUILD_FUZZ "Build fuzz harnesses with libFuzzer + ASan" OFF)

if(TRELLIS_BUILD_FUZZ)
    set(TRELLIS_FUZZ_TARGETS
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
        config_parse
    )

    foreach(fuzz_name ${TRELLIS_FUZZ_TARGETS})
        add_executable(fuzz_${fuzz_name} tests/fuzz_harnesses.c)
        target_compile_definitions(fuzz_${fuzz_name} PRIVATE FUZZ_TARGET=${fuzz_name})
        target_compile_options(fuzz_${fuzz_name} PRIVATE
            -fsanitize=fuzzer,address,undefined -fno-omit-frame-pointer)
        target_link_options(fuzz_${fuzz_name} PRIVATE
            -fsanitize=fuzzer,address,undefined)
        target_link_libraries(fuzz_${fuzz_name} PRIVATE trellis oqs PkgConfig::SODIUM)
        target_include_directories(fuzz_${fuzz_name} PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}/src
            ${CMAKE_CURRENT_SOURCE_DIR}/tests
        )
    endforeach()

    # Targets that use uv.h directly need explicit libuv linkage
    target_link_libraries(fuzz_exit_stream_payload PRIVATE uv_a)
    target_link_libraries(fuzz_gossip_receive PRIVATE uv_a)

    # config_parse needs vine-relay config sources
    target_sources(fuzz_config_parse PRIVATE
        ${CMAKE_SOURCE_DIR}/apps/vine-relay/src/config.c
        ${CMAKE_SOURCE_DIR}/apps/vine-relay/src/log.c)
    target_include_directories(fuzz_config_parse PRIVATE
        ${CMAKE_SOURCE_DIR}/apps/vine-relay/src)

    add_custom_target(fuzz_all DEPENDS
        ${TRELLIS_FUZZ_TARGETS})
endif()

# ── Benchmarks ───────────────────────────────────────────────────────────
option(TRELLIS_BUILD_BENCHMARKS "Build performance benchmarks" OFF)

if(TRELLIS_BUILD_BENCHMARKS)
    set(TRELLIS_BENCHMARKS
        bench_crypto
        bench_mesh
        bench_morph
        bench_turn
    )

    foreach(bench_name ${TRELLIS_BENCHMARKS})
        add_executable(${bench_name} benchmarks/${bench_name}.c)
        target_link_libraries(${bench_name} PRIVATE trellis oqs PkgConfig::SODIUM)
        target_include_directories(${bench_name} PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}/src
        )
    endforeach()

    # bench_mesh needs libuv for uv_loop_t
    target_link_libraries(bench_mesh PRIVATE uv_a)

    add_custom_target(run_benchmarks
        COMMENT "Running Trellis benchmarks..."
        DEPENDS ${TRELLIS_BENCHMARKS}
    )

    foreach(bench_name ${TRELLIS_BENCHMARKS})
        add_custom_command(TARGET run_benchmarks POST_BUILD
            COMMAND ${bench_name}
            COMMENT "  Running ${bench_name}..."
        )
    endforeach()
endif()
