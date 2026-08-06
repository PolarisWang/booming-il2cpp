# Bootstrap subsystem tests (chaos_bootstrap module)
#
# Each test links bootstrap_test_stubs.cpp to provide extern symbols
# (kChaosExternalRuntime*, chaos_il2cpp_aot_hotpatch_module) normally
# supplied by the codegen entrypoint. WITHOUT_CODEGEN_STUB avoids the factory's
# default gc_test_stubs.cpp (those symbols come from bootstrap_test_stubs.cpp here).

set(CHAOS_BOOTSTRAP_STUBS bootstrap_test_stubs.cpp)

# B-0 — Pre-existing smoke test
chaos_native_add_test(test_bootstrap_smoke
    bootstrap_test.cpp
    ${CHAOS_BOOTSTRAP_STUBS}
    WITHOUT_CODEGEN_STUB
    LABELS "unit;bootstrap"
)

# B-0 — Pre-existing stress test (defines its own stubs inline)
chaos_native_add_test(test_bootstrap_stress
    bootstrap_stress.cpp
    WITHOUT_CODEGEN_STUB
    LABELS "stress;bootstrap"
)

# C-1 — RegisterCodegen unit tests
chaos_native_add_test(test_bootstrap_register
    bootstrap_register_test.cpp
    ${CHAOS_BOOTSTRAP_STUBS}
    WITHOUT_CODEGEN_STUB
    LABELS "unit;bootstrap"
)
set_tests_properties(test_bootstrap_register PROPERTIES TIMEOUT 30)

# C-2 — BootstrapRuntime unit tests
chaos_native_add_test(test_bootstrap_bootstrap
    bootstrap_bootstrap_test.cpp
    ${CHAOS_BOOTSTRAP_STUBS}
    WITHOUT_CODEGEN_STUB
    LABELS "unit;bootstrap"
)
set_tests_properties(test_bootstrap_bootstrap PROPERTIES TIMEOUT 30)

# C-3 — Delegate system unit tests
chaos_native_add_test(test_bootstrap_delegate
    bootstrap_delegate_test.cpp
    ${CHAOS_BOOTSTRAP_STUBS}
    WITHOUT_CODEGEN_STUB
    LABELS "unit;bootstrap"
)
set_tests_properties(test_bootstrap_delegate PROPERTIES TIMEOUT 30)

# C-4 — Virtual method resolution unit tests
chaos_native_add_test(test_bootstrap_resolve
    bootstrap_resolve_test.cpp
    ${CHAOS_BOOTSTRAP_STUBS}
    WITHOUT_CODEGEN_STUB
    LABELS "unit;bootstrap"
)
set_tests_properties(test_bootstrap_resolve PROPERTIES TIMEOUT 30)

# C-5 — Icall resolution unit tests
chaos_native_add_test(test_bootstrap_icall
    bootstrap_icall_test.cpp
    ${CHAOS_BOOTSTRAP_STUBS}
    WITHOUT_CODEGEN_STUB
    LABELS "unit;bootstrap"
)
set_tests_properties(test_bootstrap_icall PROPERTIES TIMEOUT 30)
