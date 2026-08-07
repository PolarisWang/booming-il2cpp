# Build smoke test + GC core tests + interpreter tests + threading tests + metadata tests + runtime_stubs tests

# Matrix test sources embed non-ASCII strings and include fmt headers; MSVC needs
# /utf-8 to compile them (same convention as testing/src/native/runtime-core/gc).
if(MSVC)
    add_compile_options(/utf-8)
endif()

add_subdirectory(gc)
add_subdirectory(interpreter)
add_subdirectory(threading)
add_subdirectory(metadata)
add_subdirectory(runtime_stubs)

# Smoke test for build verification
chaos_native_add_test(smoke_test smoke_test.cpp
    LABELS "smoke;core"
)

chaos_native_add_test(capacity_stress_test capacity_stress_test.cpp
    LABELS "stress;core"
)
chaos_native_add_test(exception_sentinel_test exception_sentinel_test.cpp
    LABELS "unit;core"
)
chaos_native_add_test(exception_sentinel_all_modes_test exception_sentinel_all_modes_test.cpp
    LABELS "unit;core"
)
chaos_native_add_test(exception_negative_test exception_negative_test.cpp
    LABELS "unit;core"
)
chaos_native_add_test(method_aot_map_test method_aot_map_test.cpp gc/gc_test_stubs.cpp
    LABELS "unit;core"
)

# T0-1: type_registry API unit tests (Type System / VTable Industrialization)
chaos_native_add_test(type_registry_test type_registry_test.cpp
    LABELS "unit;core"
)

# T2-8a: EEClass AOT verification test (Type System / VTable Industrialization)
# Compile eeclass.cpp from source to test against current implementation.
chaos_native_add_test(eeclass_test eeclass_test.cpp
    ../../../../src/native/runtime-core/eeclass.cpp
    LABELS "unit;core"
)

# T3-1: Generic instantiation unit tests (GenericContextRegistry + ResolveOrInstantiate)
chaos_native_add_test(generic_instantiation_test generic_instantiation_test.cpp metadata/metadata_test_stubs.cpp
    LABELS "unit;core"
)

# IE: Instantiation engine comprehensive tests (BuildClosedDescriptor, PrecacheCallTarget,
#     AllocateRuntimeToken, QueryAotMethod, concurrency, reverse-index consistency)
chaos_native_add_test(instantiation_engine_test instantiation_engine_test.cpp
    metadata/metadata_test_stubs.cpp
    LABELS "unit;core"
)

# M0: Metadata Interface (MetadataRegistry + LookupCallTarget + ModuleLifecycleManager)
#     Null-safety and edge-case tests for the unified metadata resolution layer.
chaos_native_add_test(metadata_interface_test metadata_interface_test.cpp
    LABELS "unit;core"
)

# B1 — Blittable benchmark (labelled "benchmark", excluded from default run)
chaos_native_add_test(blittable_benchmark blittable_benchmark_test.cpp
    LABELS "benchmark;core"
)

# R4.8 — Reflection benchmark (labelled "benchmark", excluded from default run)
chaos_native_add_test(reflection_benchmark reflection_benchmark.cpp
    LABELS "benchmark;core"
)

# T4-1: convert + parse_convert benchmarks (labelled "benchmark", excluded from default run)
chaos_native_add_test(convert_benchmark convert_benchmark_test.cpp
    LABELS "benchmark;core"
)

# ── P/Invoke & Marshal test suite (marshal-industrialization) ──────────

# Shared stubs for marshal tests
set(MARSHAL_TEST_STUBS marshal_test_stubs.cpp)

# M0 — Marshal fixture smoke test (Phase A: test infrastructure verification)
chaos_native_add_test(marshal_smoke marshal_smoke_test.cpp
    ${MARSHAL_TEST_STUBS}
    LABELS "unit;marshal"
)

# B1 — Marshal alloc/free/read/write tests (Phase B: core marshal functionality)
chaos_native_add_test(marshal_alloc marshal_alloc_test.cpp
    ${MARSHAL_TEST_STUBS}
    LABELS "unit;marshal"
)

# B2 — Marshal string tests (Phase B: core marshal functionality)
chaos_native_add_test(marshal_string marshal_string_test.cpp
    ${MARSHAL_TEST_STUBS}
    LABELS "unit;marshal"
)

# B3 — Marshal API basic tests (Phase B: core marshal functionality)
chaos_native_add_test(marshal_api_basic marshal_api_basic_test.cpp
    ${MARSHAL_TEST_STUBS}
    LABELS "unit;marshal"
)

# C1 — Struct marshalling descriptor tests (Phase C: struct marshalling)
chaos_native_add_test(struct_marshal struct_marshal_test.cpp
    ${MARSHAL_TEST_STUBS}
    LABELS "unit;marshal"
)

# E1 — ICustomMarshaler test (Phase E: custom marshaler)
chaos_native_add_test(custom_marshaler custom_marshaler_test.cpp
    ${MARSHAL_TEST_STUBS}
    LABELS "unit;marshal"
)

# F1 — COM RCW test (Phase F: COM RCW)
chaos_native_add_test(com_rcw com_rcw_test.cpp
    ${MARSHAL_TEST_STUBS}
    LABELS "unit;marshal"
)

# F2 — COM CCW test (Phase F: COM CCW)
chaos_native_add_test(com_ccw com_ccw_test.cpp
    ${MARSHAL_TEST_STUBS}
    LABELS "unit;marshal"
)

# F3 — COM connection point test (Phase F: COM connection point)
chaos_native_add_test(com_connection_point com_connection_point_test.cpp
    ${MARSHAL_TEST_STUBS}
    LABELS "unit;marshal"
)

# F4 — COM platform abstraction test
chaos_native_add_test(com_platform com_platform_test.cpp
    ${MARSHAL_TEST_STUBS}
    LABELS "unit;marshal"
)

# F5 — COM factory registry test
chaos_native_add_test(com_factory com_factory_test.cpp
    ${MARSHAL_TEST_STUBS}
    LABELS "unit;marshal"
)

# F6 — COM TypeLib test
chaos_native_add_test(com_typelib com_typelib_test.cpp
    ${MARSHAL_TEST_STUBS}
    LABELS "unit;marshal"
)

# NOTE: com_dispatch is EXCLUDED from the build. It references
# chaos::il2cpp::com_ccw::CcwDispatchMethodInvoke (declared com_ccw.h:160) which is
# NOT implemented in com_ccw.cpp — a missing COM CCW dispatch feature. It is NOT
# in known-failures.unit.yaml (it is excluded, not built/run); re-enable when the
# fn is implemented.
#
# F7 — COM Dispatch test
# chaos_native_add_test(com_dispatch com_dispatch_test.cpp
#     ${MARSHAL_TEST_STUBS}
#     LABELS "unit;marshal"
# )

# S1 — RCW stress test
chaos_native_add_test(com_rcw_stress com_rcw_stress_test.cpp
    ${MARSHAL_TEST_STUBS}
    LABELS "stress;marshal"
)

# S2 — CCW stress test
chaos_native_add_test(com_ccw_stress com_ccw_stress_test.cpp
    ${MARSHAL_TEST_STUBS}
    LABELS "stress;marshal"
)

# B2 — COM benchmark test
chaos_native_add_test(com_benchmark com_benchmark_test.cpp
    ${MARSHAL_TEST_STUBS}
    LABELS "benchmark;marshal"
)

# R4.6 — Reflection stress/soak test
chaos_native_add_test(reflection_stress_test reflection_stress_test.cpp
    LABELS "stress;core"
)

# T3-3: AotCoreIr reader deserialization tests
# Compile aot_core_ir_reader.cpp from source alongside the test to test against
# the current implementation rather than the prebuilt lib's older version.
chaos_native_add_test(aot_core_ir_reader_test aot_core_ir_reader_test.cpp
    ../../../../src/native/runtime-core/aot_core_ir_reader.cpp
    LABELS "unit;core"
)

# T3-4: AssemblyManager + domain_unloader audit tests
# Compile from source alongside test, with stubs for codegen-generated symbols.
# assembly_manager_test.cpp provides external symbol stubs to satisfy linker.
chaos_native_add_test(assembly_manager_test assembly_manager_test.cpp
    ../../../../src/native/runtime-core/assembly_manager.cpp
    ../../../../src/native/runtime-core/domain_unloader.cpp
    LABELS "unit;core"
)

# L0 — DomainUnloader pressure tests (Loader 工业化 Phase 1)
# Compile domain_unloader.cpp from source alongside the test.
chaos_native_add_test(domain_unloader_stress_test domain_unloader_stress_test.cpp
    ../../../../src/native/runtime-core/domain_unloader.cpp
    gc/gc_test_stubs.cpp gc/gc_test_base.cpp
    INCLUDES gc
    LABELS "stress;core;loader"
)

# L1 — AssemblyManager 并发压力测试 (Loader 工业化 Phase 1)
# 验证锁粒度、并发只读查询、NextAlcId 单调性和边界安全
chaos_native_add_test(assembly_manager_stress_test assembly_manager_stress_test.cpp
    LABELS "stress;core;loader"
)

# L2 — ModuleRegistry 单元 + 并发测试 (Loader 工业化 Phase 1)
# 所有 module_registry API 使用 shared_mutex，编译 module_registry.cpp 从源码以确保测试当前版本
chaos_native_add_test(module_registry_test module_registry_test.cpp
    ../../../../src/native/runtime-core/module_registry.cpp
    LABELS "unit;stress;core;loader"
)

# T3-5: convert + parse_convert audit tests
# Prebuilt lib is up-to-date (no uncommitted changes); link via CHAOS_COMMON_TEST_LIBS.
chaos_native_add_test(convert_parse_test convert_parse_test.cpp
    LABELS "unit;core"
)

# T3-6: native_library + static_var_store audit tests
# Compile static_var_store.cpp from source to test against current implementation.
# native_library functions resolve from the prebuilt lib (CHAOS_COMMON_TEST_LIBS).
# No special stubs needed — the test exercises null-safety guards + field-level API.
chaos_native_add_test(native_static_var_test native_static_var_test.cpp
    ../../../../src/native/runtime-core/static_var_store.cpp
    LABELS "unit;core"
)
