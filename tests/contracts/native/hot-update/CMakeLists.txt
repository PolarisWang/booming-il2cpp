# chaos_runtime_core.lib carries instantiation_engine.obj duplicating runtime_core.obj
# symbols (pulled transitively via chaos_hot_update/chaos_interpreter); /FORCE:MULTIPLE
# tolerates it (same as add_chaos_test).
if(MSVC)
    add_link_options(/FORCE:MULTIPLE)
endif()

add_executable(chaos_method_replacement_smoke
    method_replacement_smoke.cpp)

target_compile_features(chaos_method_replacement_smoke PRIVATE cxx_std_17)
target_link_libraries(
    chaos_method_replacement_smoke
    PRIVATE
        chaos_hot_update
        chaos_interpreter
        chaos_runtime_core
        chaos_bootstrap
        chaos_common
        chaos_support
        chaos_fmt
        chaos_codegen)

add_executable(chaos_hotupdate_verification_test
    hotupdate_verification_test.cpp)

target_compile_features(chaos_hotupdate_verification_test PRIVATE cxx_std_17)
if(MSVC)
    target_compile_options(chaos_hotupdate_verification_test PRIVATE /utf-8)
endif()
target_link_libraries(
    chaos_hotupdate_verification_test
    PRIVATE
        chaos_hot_update
        chaos_bootstrap)
