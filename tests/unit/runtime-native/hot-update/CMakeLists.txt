# Hot-update subsystem tests (chaos_hot_update module)
# Built via the unified chaos_native_add_test factory. The factory's default
# lib set (chaos_test_libs_v0) covers all transitive deps (hot_update, bootstrap,
# runtime_core, jit, interpreter, debugger, common, support, fmt), so no explicit
# LIBS list is needed here.

chaos_native_add_test(test_method_replacement_smoke
    method_replacement_smoke_test.cpp
    LABELS "unit;hotupdate"
)

chaos_native_add_test(test_hotupdate_verification
    hotupdate_verification_test.cpp
    LABELS "unit;hotupdate"
)

# hotupdate_package_e2e_test — E2E package loading with real directory structure
chaos_native_add_test(test_hotupdate_package_e2e
    hotupdate_package_e2e_test.cpp
    LABELS "unit;hotupdate"
)

# interpreter_mixed_execution_test — InterpreterEntryDirect + method_replacement integration
chaos_native_add_test(test_interpreter_mixed_execution
    interpreter_mixed_execution_test.cpp
    LABELS "unit;hotupdate;interpreter"
)

# hotupdate_vtable_sync_test — Cross-module VTable sync with method_replacement
chaos_native_add_test(test_hotupdate_vtable_sync
    hotupdate_vtable_sync_test.cpp
    LABELS "unit;hotupdate"
)

# hotupdate_generics_test — Generic context registration and dispatch
chaos_native_add_test(test_hotupdate_generics
    hotupdate_generics_test.cpp
    LABELS "unit;hotupdate"
)

# hotupdate_replacement_benchmark_test — Benchmark & stress for method_replacement
chaos_native_add_test(test_hotupdate_replacement_benchmark
    hotupdate_replacement_benchmark_test.cpp
    LABELS "unit;hotupdate;benchmark"
)

# hotpatch_api_contract_test — Public hotpatch C API contract (input validation,
# return-code semantics, host revision).  Host-module-independent behaviors only.
chaos_native_add_test(test_hotpatch_api_contract
    hotpatch_api_contract_test.cpp
    LABELS "unit;hotupdate"
)

