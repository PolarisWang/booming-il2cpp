# Build smoke test + GC core tests + interpreter tests + threading tests + metadata tests + runtime_stubs tests
add_subdirectory(gc)
add_subdirectory(interpreter)
add_subdirectory(threading)
add_subdirectory(metadata)
add_subdirectory(runtime_stubs)

# Smoke test for build verification
add_chaos_test(smoke_test smoke_test.cpp
    LABELS "smoke;core"
)

# N1 — Non-GC runtime-core tests
set(CHAOS_COMMON_TEST_LIBS
    chaos_runtime_core
    chaos_bootstrap
    chaos_interpreter
    chaos_codegen
    chaos_hot_update
    chaos_common
    chaos_support
    chaos_fmt
    chaos_debugger
)

add_chaos_test(capacity_stress_test capacity_stress_test.cpp
    LIBS ${CHAOS_COMMON_TEST_LIBS}
    LABELS "stress;core"
)
add_chaos_test(exception_sentinel_test exception_sentinel_test.cpp
    LIBS ${CHAOS_COMMON_TEST_LIBS}
    LABELS "unit;core"
)
add_chaos_test(exception_sentinel_all_modes_test exception_sentinel_all_modes_test.cpp
    LIBS ${CHAOS_COMMON_TEST_LIBS}
    LABELS "unit;core"
)
add_chaos_test(exception_negative_test exception_negative_test.cpp
    LIBS ${CHAOS_COMMON_TEST_LIBS}
    LABELS "unit;core"
)
add_chaos_test(method_aot_map_test method_aot_map_test.cpp gc/gc_test_stubs.cpp
    LIBS ${CHAOS_COMMON_TEST_LIBS}
    LABELS "unit;core"
)

# T0-1: type_registry API unit tests (Type System / VTable Industrialization)
add_chaos_test(type_registry_test type_registry_test.cpp
    LIBS ${CHAOS_COMMON_TEST_LIBS}
    LABELS "unit;core"
)

# T2-8a: EEClass AOT verification test (Type System / VTable Industrialization)
add_chaos_test(eeclass_test eeclass_test.cpp
    LIBS ${CHAOS_COMMON_TEST_LIBS}
    LABELS "unit;core"
)

# T3-1: Generic instantiation unit tests (GenericContextRegistry + ResolveOrInstantiate)
add_chaos_test(generic_instantiation_test generic_instantiation_test.cpp metadata/metadata_test_stubs.cpp
    LIBS ${CHAOS_COMMON_TEST_LIBS}
    LABELS "unit;core"
)

# IE: Instantiation engine comprehensive tests (BuildClosedDescriptor, PrecacheCallTarget,
#     AllocateRuntimeToken, QueryAotMethod, concurrency, reverse-index consistency)
add_chaos_test(instantiation_engine_test instantiation_engine_test.cpp
    gc/gc_test_stubs.cpp metadata/metadata_test_stubs.cpp
    LIBS ${CHAOS_COMMON_TEST_LIBS}
    LABELS "unit;core"
)

# B1 — Blittable benchmark (labelled "benchmark", excluded from default run)
add_chaos_test(blittable_benchmark blittable_benchmark_test.cpp
    LIBS ${CHAOS_COMMON_TEST_LIBS}
    LABELS "benchmark;core"
)

# R4.8 — Reflection benchmark (labelled "benchmark", excluded from default run)
add_chaos_test(reflection_benchmark reflection_benchmark.cpp
    LIBS ${CHAOS_COMMON_TEST_LIBS}
    LABELS "benchmark;core"
)

# ── P/Invoke & Marshal test suite (marshal-industrialization) ──────────

# Shared stubs for marshal tests
set(MARSHAL_TEST_STUBS marshal_test_stubs.cpp)

# M0 — Marshal fixture smoke test (Phase A: test infrastructure verification)
add_chaos_test(marshal_smoke marshal_smoke_test.cpp
    ${MARSHAL_TEST_STUBS}
    LIBS ${CHAOS_COMMON_TEST_LIBS}
    LABELS "unit;marshal"
)

# B1 — Marshal alloc/free/read/write tests (Phase B: core marshal functionality)
add_chaos_test(marshal_alloc marshal_alloc_test.cpp
    ${MARSHAL_TEST_STUBS}
    LIBS ${CHAOS_COMMON_TEST_LIBS}
    LABELS "unit;marshal"
)

# B2 — Marshal string tests (Phase B: core marshal functionality)
add_chaos_test(marshal_string marshal_string_test.cpp
    ${MARSHAL_TEST_STUBS}
    LIBS ${CHAOS_COMMON_TEST_LIBS}
    LABELS "unit;marshal"
)

# B3 — Marshal API basic tests (Phase B: core marshal functionality)
add_chaos_test(marshal_api_basic marshal_api_basic_test.cpp
    ${MARSHAL_TEST_STUBS}
    LIBS ${CHAOS_COMMON_TEST_LIBS}
    LABELS "unit;marshal"
)

# C1 — Struct marshalling descriptor tests (Phase C: struct marshalling)
add_chaos_test(struct_marshal struct_marshal_test.cpp
    ${MARSHAL_TEST_STUBS}
    LIBS ${CHAOS_COMMON_TEST_LIBS}
    LABELS "unit;marshal"
)

# R4.6 — Reflection stress/soak test
add_chaos_test(reflection_stress_test reflection_stress_test.cpp
    LIBS ${CHAOS_COMMON_TEST_LIBS}
    LABELS "stress;core"
)
