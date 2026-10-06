# chaos_net - static library for the net-stack industrialization.
# Layers: A entry.cpp (shape dispatch), B socket_ops.cpp (state machine),
# D net_impl_win32.cpp / net_impl_posix.cpp (transport, NT-6+), address.cpp (NT-10),
# F dns.cpp (host resolution, NT-9).
get_filename_component(REPO_ROOT "${CMAKE_CURRENT_LIST_DIR}/../../.." ABSOLUTE)

set(CHAOS_NET_SOURCES
    chaos/net/net.h
    chaos/net/abi.h
    chaos/net/socket_handle.h
    chaos/net/error.h
    chaos/net/net_api.h
    chaos/net/async_op.h
    chaos/net/address.h
    chaos/net/dns.h
    entry.cpp
    socket_ops.cpp
    error.cpp
    async_ops.cpp
    completion.cpp
    dns.cpp
    address.cpp
    http_impl.cpp
    http2_impl.cpp
    hpack.cpp
    ws_impl.cpp
    sse_impl.cpp
    chaos/net/tls.h
    chaos/net/http.h
    chaos/net/http2.h
    chaos/net/hpack.h
    chaos/net/ws.h
    chaos/net/sse.h)
if(WIN32)
    list(APPEND CHAOS_NET_SOURCES net_impl_win32.cpp tls_schannel.cpp)
else()
    # POSIX transport (NT-11): Linux uses epoll + eventfd; other POSIX
    # platforms (MSYS2/Cygwin/Android/...) fall back to poll + pipe.
    list(APPEND CHAOS_NET_SOURCES net_impl_posix.cpp)
    if(NOT APPLE)
        # NT-12: OpenSSL TLS transport (Linux/Android; also MSYS2 runtime).
        list(APPEND CHAOS_NET_SOURCES tls_openssl.cpp)
    endif()
endif()

add_library(chaos_net STATIC ${CHAOS_NET_SOURCES})

target_compile_features(chaos_net PUBLIC cxx_std_17)

# MSVC: fmt/format.h requires /utf-8 (matches every other native target).
target_compile_options(chaos_net PRIVATE $<$<CXX_COMPILER_ID:MSVC>:/utf-8>)
target_link_libraries(chaos_net PUBLIC chaos_common)
if(WIN32)
    target_link_libraries(chaos_net PUBLIC ws2_32 secur32 crypt32)
elseif(NOT APPLE)
    find_package(OpenSSL REQUIRED)
    target_link_libraries(chaos_net PUBLIC OpenSSL::SSL OpenSSL::Crypto)
endif()
target_include_directories(chaos_net PUBLIC "${CMAKE_CURRENT_LIST_DIR}")