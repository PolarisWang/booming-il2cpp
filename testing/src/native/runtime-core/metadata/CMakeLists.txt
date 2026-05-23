# MetadataRegistry unit tests
add_chaos_test(metadata_registry_test metadata_registry_test.cpp metadata_test_stubs.cpp
    LIBS chaos_runtime_core chaos_bootstrap chaos_interpreter chaos_jit
         chaos_hot_update chaos_common chaos_support chaos_fmt
    LABELS "unit;core"
)
