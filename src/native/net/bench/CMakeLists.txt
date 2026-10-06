cmake_minimum_required(VERSION 3.25)
project(chaos_net_bench)
# NT-15: performance baseline benchmark -- independent bench project.
# Build: cmake -S src/native/net/bench -B build/net-bench -A x64
#        cmake --build build/net-bench --config RelWithDebInfo --target chaos_net_bench
# Scenarios: TCP throughput (GB/s), connect rate, concurrency, TLS handshake
# rate, HTTP req/s -- all loopback, dual platform (MSVC/Schannel, MSYS2/OpenSSL).
get_filename_component(NET_DIR "${CMAKE_CURRENT_LIST_DIR}/.." ABSOLUTE)
get_filename_component(COMMON_DIR "${NET_DIR}/../common" ABSOLUTE)

if(WIN32)
    set(NET_IMPL_SRC ${NET_DIR}/net_impl_win32.cpp)
    set(TLS_IMPL_SRC ${NET_DIR}/tls_schannel.cpp)
else()
    set(NET_IMPL_SRC ${NET_DIR}/net_impl_posix.cpp)
    set(TLS_IMPL_SRC ${NET_DIR}/tls_openssl.cpp)
    find_package(OpenSSL REQUIRED)
endif()

add_executable(chaos_net_bench
    chaos_net_bench.cpp
    ${NET_DIR}/http_impl.cpp
    ${TLS_IMPL_SRC}
    ${NET_IMPL_SRC}
    ${NET_DIR}/dns.cpp
    ${NET_DIR}/async_ops.cpp
    ${NET_DIR}/completion.cpp
    ${NET_DIR}/socket_ops.cpp
    ${NET_DIR}/error.cpp)
target_compile_features(chaos_net_bench PRIVATE cxx_std_17)
target_include_directories(chaos_net_bench PRIVATE "${NET_DIR}" "${COMMON_DIR}")
if(MSVC)
    target_compile_options(chaos_net_bench PRIVATE /utf-8)
endif()
if(WIN32)
    target_link_libraries(chaos_net_bench PRIVATE ws2_32 secur32 crypt32)
else()
    target_link_libraries(chaos_net_bench PRIVATE OpenSSL::SSL OpenSSL::Crypto)
endif()
