# Engine-bridge subsystem tests (chaos_engine_bridge module)
# Built via the unified chaos_native_add_test factory (default full runtime libs).

chaos_native_add_test(test_engine_bridge_smoke
    engine_bridge_test.cpp
    LABELS "unit;engine-bridge"
)

chaos_native_add_test(test_engine_bridge_stress
    engine_bridge_stress.cpp
    LABELS "stress;engine-bridge"
)
