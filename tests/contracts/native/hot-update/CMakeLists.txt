# Hot-Update contracts targets — factory (cmake/chaos_native_test.cmake) provides
# the runtime lib set, /FORCE:MULTIPLE + /utf-8 flags, include dirs, and the
# codegen-symbol stub.

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/method_replacement_smoke.cpp")
    chaos_native_add_test(chaos_method_replacement_smoke method_replacement_smoke.cpp)
endif()

if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/hotupdate_verification_test.cpp")
    chaos_native_add_test(chaos_hotupdate_verification_test hotupdate_verification_test.cpp)
endif()
