add_executable(chaos_method_replacement_smoke
    method_replacement_smoke.cpp)

target_compile_features(chaos_method_replacement_smoke PRIVATE cxx_std_17)
target_link_libraries(
    chaos_method_replacement_smoke
    PRIVATE
        chaos_hot_update)

add_executable(chaos_hotupdate_verification_test
    hotupdate_verification_test.cpp)

target_compile_features(chaos_hotupdate_verification_test PRIVATE cxx_std_17)
target_link_libraries(
    chaos_hotupdate_verification_test
    PRIVATE
        chaos_hot_update
        chaos_bootstrap)
