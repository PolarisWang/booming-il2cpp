# Build smoke test + GC core tests + interpreter tests
add_subdirectory(gc)
add_subdirectory(interpreter)

# Smoke test for build verification
add_chaos_test(smoke_test smoke_test.cpp)
