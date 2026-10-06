cmake_minimum_required(VERSION 3.25)
project(chaos_net_test)
# chaos_net_test -- standalone native unit test for chaos_net (NT-6).
# Build: cmake -S src/native/net/tests -B build/net-tests -A x64
#        cmake --build build/net-tests --config RelWithDebInfo
get_filename_component(NET_DIR "${CMAKE_CURRENT_LIST_DIR}/.." ABSOLUTE)
get_filename_component(COMMON_DIR "${NET_DIR}/../common" ABSOLUTE)

# Platform transport driver: win32 (IOCP) on Windows, posix (epoll/poll) elsewhere.
if(WIN32)
    set(NET_IMPL_SRC ${NET_DIR}/net_impl_win32.cpp)
    set(TLS_IMPL_SRC ${NET_DIR}/tls_schannel.cpp)
else()
    set(NET_IMPL_SRC ${NET_DIR}/net_impl_posix.cpp)
    set(TLS_IMPL_SRC ${NET_DIR}/tls_openssl.cpp)
    find_package(OpenSSL REQUIRED)
endif()

add_executable(chaos_net_test
    chaos_net_test.cpp
    ${NET_IMPL_SRC}
    ${NET_DIR}/async_ops.cpp
    ${NET_DIR}/completion.cpp
    ${NET_DIR}/socket_ops.cpp
    ${NET_DIR}/error.cpp)

target_compile_features(chaos_net_test PRIVATE cxx_std_17)
target_include_directories(chaos_net_test PRIVATE "${NET_DIR}" "${COMMON_DIR}")
if(WIN32)
    target_link_libraries(chaos_net_test PRIVATE ws2_32)
endif()

# NT-7: IOCP async completion pipeline test.
add_executable(chaos_net_async_test
    chaos_net_async_test.cpp
    ${NET_IMPL_SRC}
    ${NET_DIR}/async_ops.cpp
    ${NET_DIR}/completion.cpp
    ${NET_DIR}/socket_ops.cpp
    ${NET_DIR}/error.cpp)
target_compile_features(chaos_net_async_test PRIVATE cxx_std_17)
target_include_directories(chaos_net_async_test PRIVATE "${NET_DIR}" "${COMMON_DIR}")
if(WIN32)
    target_link_libraries(chaos_net_async_test PRIVATE ws2_32)
endif()

# NT-10: Primitives address interop test (IPAddress/IPEndPoint/SocketAddress
# carriers, NetAddress <-> sockaddr_in / sockaddr_in6, text forms).
add_executable(chaos_net_address_test
    chaos_net_address_test.cpp
    ${NET_DIR}/address.cpp
    ${NET_DIR}/error.cpp)
target_compile_features(chaos_net_address_test PRIVATE cxx_std_17)
target_include_directories(chaos_net_address_test PRIVATE "${NET_DIR}" "${COMMON_DIR}")
if(WIN32)
    target_link_libraries(chaos_net_address_test PRIVATE ws2_32)
endif()

# NT-8: S34 high-level entry points (Stream::FlushAsync / UdpClient::SendAsync /
# IPEndPoint::.ctor).  entry.cpp (Layer A extern "C" shape dispatch) is included
# here because it is where the S34 symbols live; the fact-run covers the full
# generated awaiter chain, this executable pins the native contract.
add_executable(chaos_net_entry_test
    chaos_net_entry_test.cpp
    ${NET_DIR}/entry.cpp
    ${NET_IMPL_SRC}
    ${NET_DIR}/async_ops.cpp
    ${NET_DIR}/completion.cpp
    ${NET_DIR}/socket_ops.cpp
    ${NET_DIR}/error.cpp
    # fmt::vformat / report_error used by the inline log path in entry.cpp
    "${CMAKE_CURRENT_LIST_DIR}/../../../../third_party/fmt/src/format.cc")
target_compile_features(chaos_net_entry_test PRIVATE cxx_std_17)
target_include_directories(chaos_net_entry_test PRIVATE "${NET_DIR}" "${COMMON_DIR}"
    "${CMAKE_CURRENT_LIST_DIR}/../../../../third_party/fmt/include"
    "${CMAKE_CURRENT_LIST_DIR}/../../pal")
if(MSVC)
    # fmt/format.h static_assert requires /utf-8 on MSVC.
    target_compile_options(chaos_net_entry_test PRIVATE /utf-8)
endif()
if(WIN32)
    target_link_libraries(chaos_net_entry_test PRIVATE ws2_32)
endif()

# NT-9: DNS host resolution test (hosts file / literal fast path / TTL cache /
# async via completion pipeline).  Links the same set as chaos_net_test plus
# dns.cpp; NetStartup (net_impl_win32) required because getaddrinfo needs
# WSAStartup on Windows.
add_executable(chaos_net_dns_test
    chaos_net_dns_test.cpp
    ${NET_DIR}/dns.cpp
    ${NET_IMPL_SRC}
    ${NET_DIR}/async_ops.cpp
    ${NET_DIR}/completion.cpp
    ${NET_DIR}/socket_ops.cpp
    ${NET_DIR}/error.cpp)
target_compile_features(chaos_net_dns_test PRIVATE cxx_std_17)
target_include_directories(chaos_net_dns_test PRIVATE "${NET_DIR}" "${COMMON_DIR}")
if(WIN32)
    target_link_libraries(chaos_net_dns_test PRIVATE ws2_32)
endif()

# NT-12: TLS provider matrix (Schannel on Windows, OpenSSL elsewhere).
add_executable(chaos_net_tls_test
    chaos_net_tls_test.cpp
    ${TLS_IMPL_SRC}
    ${NET_IMPL_SRC}
    ${NET_DIR}/async_ops.cpp
    ${NET_DIR}/completion.cpp
    ${NET_DIR}/socket_ops.cpp
    ${NET_DIR}/error.cpp)
target_compile_features(chaos_net_tls_test PRIVATE cxx_std_17)
target_include_directories(chaos_net_tls_test PRIVATE "${NET_DIR}" "${COMMON_DIR}")
if(MSVC)
    target_compile_options(chaos_net_tls_test PRIVATE /utf-8)
endif()
if(WIN32)
    target_link_libraries(chaos_net_tls_test PRIVATE ws2_32 secur32 crypt32)
else()
    target_link_libraries(chaos_net_tls_test PRIVATE OpenSSL::SSL OpenSSL::Crypto)
endif()

# NT-18: offline CRL certificate revocation (tls.h TlsRevocationOptions).
# Valid leaf accepted, revoked leaf rejected with a real error, and the same
# revoked leaf accepted when the option is off (proves the gate is opt-in).
add_executable(chaos_net_revocation_test
    chaos_net_revocation_test.cpp
    ${TLS_IMPL_SRC}
    ${NET_IMPL_SRC}
    ${NET_DIR}/async_ops.cpp
    ${NET_DIR}/completion.cpp
    ${NET_DIR}/socket_ops.cpp
    ${NET_DIR}/error.cpp)
target_compile_features(chaos_net_revocation_test PRIVATE cxx_std_17)
target_include_directories(chaos_net_revocation_test PRIVATE "${NET_DIR}" "${COMMON_DIR}")
if(MSVC)
    target_compile_options(chaos_net_revocation_test PRIVATE /utf-8)
endif()
if(WIN32)
    target_link_libraries(chaos_net_revocation_test PRIVATE ws2_32 secur32 crypt32)
else()
    target_link_libraries(chaos_net_revocation_test PRIVATE OpenSSL::SSL OpenSSL::Crypto)
endif()

# NT-13: native HttpClient (http_impl.cpp) over transport/DNS/TLS; loopback
# server matrix (http + https, chunked / content-length / read-to-close,
# keep-alive pooling). Per NT-13 decision: native implementation.
add_executable(chaos_net_http_test
    chaos_net_http_test.cpp
    ${NET_DIR}/http_impl.cpp
    ${TLS_IMPL_SRC}
    ${NET_IMPL_SRC}
    ${NET_DIR}/dns.cpp
    ${NET_DIR}/async_ops.cpp
    ${NET_DIR}/completion.cpp
    ${NET_DIR}/socket_ops.cpp
    ${NET_DIR}/error.cpp)
target_compile_features(chaos_net_http_test PRIVATE cxx_std_17)
target_include_directories(chaos_net_http_test PRIVATE "${NET_DIR}" "${COMMON_DIR}")
if(MSVC)
    target_compile_options(chaos_net_http_test PRIVATE /utf-8)
endif()
if(WIN32)
    target_link_libraries(chaos_net_http_test PRIVATE ws2_32 secur32 crypt32)
else()
    target_link_libraries(chaos_net_http_test PRIVATE OpenSSL::SSL OpenSSL::Crypto)
endif()

# NT-14: WebSocket (RFC 6455) + SSE clients (ws_impl.cpp / sse_impl.cpp)
# over transport/DNS/TLS; loopback ws:// + wss:// + SSE servers.
add_executable(chaos_net_ws_test
    chaos_net_ws_test.cpp
    ${NET_DIR}/ws_impl.cpp
    ${NET_DIR}/sse_impl.cpp
    ${TLS_IMPL_SRC}
    ${NET_IMPL_SRC}
    ${NET_DIR}/dns.cpp
    ${NET_DIR}/async_ops.cpp
    ${NET_DIR}/completion.cpp
    ${NET_DIR}/socket_ops.cpp
    ${NET_DIR}/error.cpp)
target_compile_features(chaos_net_ws_test PRIVATE cxx_std_17)
target_include_directories(chaos_net_ws_test PRIVATE "${NET_DIR}" "${COMMON_DIR}")
if(MSVC)
    target_compile_options(chaos_net_ws_test PRIVATE /utf-8)
endif()
if(WIN32)
    target_link_libraries(chaos_net_ws_test PRIVATE ws2_32 secur32 crypt32)
else()
    target_link_libraries(chaos_net_ws_test PRIVATE OpenSSL::SSL OpenSSL::Crypto)
endif()
# NT-16/21: native HTTP/2 client + loopback server (RFC 9113) -- http2_impl.cpp
# + full HPACK (RFC 7541: dynamic table, Huffman; chaos/net/hpack.h),
# multiplexed streams, flow control, h2c and h2 (ALPN) paths.
add_executable(chaos_net_http2_test
    chaos_net_http2_test.cpp
    ${NET_DIR}/http2_impl.cpp
    ${NET_DIR}/hpack.cpp
    ${TLS_IMPL_SRC}
    ${NET_IMPL_SRC}
    ${NET_DIR}/dns.cpp
    ${NET_DIR}/async_ops.cpp
    ${NET_DIR}/completion.cpp
    ${NET_DIR}/socket_ops.cpp
    ${NET_DIR}/error.cpp)
target_compile_features(chaos_net_http2_test PRIVATE cxx_std_17)
target_include_directories(chaos_net_http2_test PRIVATE "${NET_DIR}" "${COMMON_DIR}")
if(MSVC)
    target_compile_options(chaos_net_http2_test PRIVATE /utf-8)
endif()
if(WIN32)
    target_link_libraries(chaos_net_http2_test PRIVATE ws2_32 secur32 crypt32)
else()
    target_link_libraries(chaos_net_http2_test PRIVATE OpenSSL::SSL OpenSSL::Crypto)
endif()
# NT-17:
# NT-23: Happy-Eyeballs connect (RFC 8305 simplified, ordered address list
# with short per-attempt timeout): refused/timeout family falls through to
# the next candidate, first success wins, legacy single-address flag, and
# ping/pong over the connected socket.
add_executable(chaos_net_eyeballs_test
    chaos_net_eyeballs_test.cpp
    ${NET_IMPL_SRC}
    ${NET_DIR}/async_ops.cpp
    ${NET_DIR}/completion.cpp
    ${NET_DIR}/socket_ops.cpp
    ${NET_DIR}/error.cpp)
target_compile_features(chaos_net_eyeballs_test PRIVATE cxx_std_17)
target_include_directories(chaos_net_eyeballs_test PRIVATE "${NET_DIR}" "${COMMON_DIR}")
if(MSVC)
    target_compile_options(chaos_net_eyeballs_test PRIVATE /utf-8)
endif()
if(WIN32)
    target_link_libraries(chaos_net_eyeballs_test PRIVATE ws2_32)
endif()
# IPv6 multi-address-family test (AF_INET6 loopback echo, IPV6_V6ONLY
# roundtrip, dual-stack in6addr_any + IPv4-mapped unmap, v6only v4 rejection).
add_executable(chaos_net_ipv6_test
    chaos_net_ipv6_test.cpp
    ${NET_IMPL_SRC}
    ${NET_DIR}/async_ops.cpp
    ${NET_DIR}/completion.cpp
    ${NET_DIR}/socket_ops.cpp
    ${NET_DIR}/error.cpp)
target_compile_features(chaos_net_ipv6_test PRIVATE cxx_std_17)
target_include_directories(chaos_net_ipv6_test PRIVATE "${NET_DIR}" "${COMMON_DIR}")
if(MSVC)
    target_compile_options(chaos_net_ipv6_test PRIVATE /utf-8)
endif()
if(WIN32)
    target_link_libraries(chaos_net_ipv6_test PRIVATE ws2_32)
endif()

# NT-24: UDP datagram layer (loopback echo, multi-client delivery,
# SO_BROADCAST to 255.255.255.255, datagram option plumbing).
add_executable(chaos_net_udp_test
    chaos_net_udp_test.cpp
    ${NET_IMPL_SRC}
    ${NET_DIR}/async_ops.cpp
    ${NET_DIR}/completion.cpp
    ${NET_DIR}/socket_ops.cpp
    ${NET_DIR}/error.cpp)
target_compile_features(chaos_net_udp_test PRIVATE cxx_std_17)
target_include_directories(chaos_net_udp_test PRIVATE "${NET_DIR}" "${COMMON_DIR}")
if(MSVC)
    target_compile_options(chaos_net_udp_test PRIVATE /utf-8)
endif()
if(WIN32)
    target_link_libraries(chaos_net_udp_test PRIVATE ws2_32)
endif()
