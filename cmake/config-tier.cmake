# ── Config tier selection ────────────────────────────────────────────────
#
# Include in any CMakeLists.txt that needs CHAOS_IL2CPP_CONFIG_* definitions.
# Set via CMakePresets or -DCHAOS_IL2CPP_CONFIG_TIER=profile|ship.
# Default: debug (CHECK tier — full assertions + trace).
#
# Usage:
#   include("${REPO_ROOT}/cmake/config-tier.cmake")
#
# Recognized tiers:
#   debug   → CHAOS_IL2CPP_CONFIG_CHECK  (default)
#   profile → CHAOS_IL2CPP_CONFIG_PROFILE + CHAOS_IL2CPP_PROFILE_ENABLED=1
#   ship    → CHAOS_IL2CPP_CONFIG_SHIP

set(CHAOS_IL2CPP_CONFIG_TIER "debug" CACHE STRING "Build config tier: debug|profile|ship")

# /GS- globally: disable buffer security check for all targets in CHECK/PROFILE
# tiers. The verification family builds link across many .lib files; a single
# /GS function in any lib triggers 0xC0000409 on stack manipulation patterns
# that are safe in AOT-generated code. SHIP tier omits it for production.
if(MSVC AND NOT CHAOS_IL2CPP_CONFIG_TIER STREQUAL "ship")
    add_compile_options(/GS-)
endif()

if(CHAOS_IL2CPP_CONFIG_TIER STREQUAL "profile")
    add_compile_definitions(CHAOS_IL2CPP_CONFIG_PROFILE)
    add_compile_definitions(CHAOS_IL2CPP_PROFILE_ENABLED=1)
elseif(CHAOS_IL2CPP_CONFIG_TIER STREQUAL "ship")
    add_compile_definitions(CHAOS_IL2CPP_CONFIG_SHIP)
else()
    add_compile_definitions(CHAOS_IL2CPP_CONFIG_CHECK)
endif()