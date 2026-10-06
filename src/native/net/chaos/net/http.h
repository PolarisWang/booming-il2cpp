// Layer G -- HTTP client (NT-13).  Native HttpClient implementation.
//
// Decision point (roadmap-v1-01.md §2 NT-13, net-cpp-architecture.md §10):
// native implementation chosen over managed BCL handler and libcurl --
//   * q_bcl already committed us to a native rewrite of the high-level
//     semantics (Socket/TcpClient/UdpClient/NetworkStream); an HttpClient
//     on the same native stack stays isomorphic with that decision.
//   * CI is loopback-only (architecture §14 risk 6); a managed BCL handler
//     would need SslStream delivered and external-network semantics to
//     exercise, neither available here.
//   * libcurl would add supply-chain review + ABI adaptation and needs a
//     separate build per platform (MSVC/MSYS2/Linux); native http_*.cpp
//     reuses the Layer D transport (NT-6/7), DNS (NT-9) and TLS (NT-12)
//     already in-tree, with one loopback test matrix per platform.
//
// Surface: synchronous request/response over pooled keep-alive connections
// (one pool per scheme://host:port), HTTP/1.1 request framing, response
// framing via Content-Length / chunked / read-to-close, optional TLS
// (https) through the TlsProvider from NT-12.  Everything is noexcept;
// errors return NetError.  RFC 7230-ish subset -- no redirects, no
// cookies, no proxies, one-body-per-response (the BCL layer above owns
// those semantics).
#pragma once
#include <chaos/net/net.h>
#include <chaos/net/net_api.h>
#include <cstdint>

namespace chaos::net {

enum class HttpMethod : CHAOS_IL2CPP_INT32 {
    Get = 0,
    Post,
    Put,
    Delete,
    Head,
};

// One request header: name and value are borrowed by HttpSend (must stay
// alive for the call).  headers may be nullptr with headerCount == 0.
struct HttpHeader {
    const char* name;
    const char* value;
};

struct HttpRequest {
    HttpMethod method = HttpMethod::Get;
    const char* url = nullptr;   // "http://host[:port]/path[?query]" or
                                 // "https://..." -- bare path not allowed
    const HttpHeader* headers = nullptr;  // extra headers (Host is added)
    CHAOS_IL2CPP_INT32 headerCount = 0;
    const CHAOS_IL2CPP_UINT8* body = nullptr;  // for Post/Put
    CHAOS_IL2CPP_INT32 bodyLength = 0;
    bool disableCertificateValidation = false;  // https test hook
    CHAOS_IL2CPP_INT32 timeoutMs = 10000;      // connect/handshake/io
};

struct HttpResponse {
    CHAOS_IL2CPP_INT32 statusCode = 0;      // 200, 404, ...
    const char* reason = nullptr;           // "OK", "Not Found" (static)
    const HttpHeader* headers = nullptr;    // owned; free with HttpFreeResponse
    CHAOS_IL2CPP_INT32 headerCount = 0;
    const CHAOS_IL2CPP_UINT8* body = nullptr;  // owned; len bytes
    CHAOS_IL2CPP_INT32 bodyLength = 0;
};

// Sends one request over a pooled connection (establishes it if needed).
// On success *out is filled with owned header/body storage that MUST be
// released with HttpFreeResponse.  On failure *out is zeroed and a NetError
// is returned (TimedOut / ConnectionReset / DnsFailure / ...).
NetError HttpSend(const HttpRequest& req, HttpResponse* out) noexcept;

void HttpFreeResponse(HttpResponse* resp) noexcept;

// Test hook: drains the connection pool (closes every kept-alive socket).
void HttpPoolClear() noexcept;

}  // namespace chaos::net
