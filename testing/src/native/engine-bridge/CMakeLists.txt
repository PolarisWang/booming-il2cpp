# Engine-bridge subsystem tests (chaos_engine_bridge module)

set(CHAOS_ENGINE_BRIDGE_LIBS
    chaos_engine_bridge
    chaos_runtime_core
    chaos_bootstrap
    chaos_interpreter
    chaos_codegen
    chaos_hot_update
    chaos_common
    chaos_support
    chaos_fmt
)

add_chaos_test(test_engine_bridge_smoke
    engine_bridge_test.cpp
    LIBS ${CHAOS_ENGINE_BRIDGE_LIBS}
)

add_chaos_test(test_engine_bridge_stress
    engine_bridge_stress.cpp
    LIBS ${CHAOS_ENGINE_BRIDGE_LIBS}
)
set_tests_properties(test_engine_bridge_stress PROPERTIES LABELS "stress")
