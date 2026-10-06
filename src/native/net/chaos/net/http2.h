// Layer I -- HTTP/2 client (RFC 9113) (NT-16).
//
// Native HTTP/2 client (and loopback-server framing helpers) over the
// Layer D transport + NT-9 DNS + NT-12 TLS.  Same decision rationale as
// NT-13/NT-14 (native, no external deps, dual-platform single-source,
// loopback-testable in CI, reuses the delivered transport/DNS/TLS stack).
//
// Covered (RFC 9113 section refs):
//   * connection preface  (3.5) -- "PRI * HTTP/2.0\r\n\r\nSM\r\n\r\n"
//   * framing             (4.1) -- 9-byte frame header + length/type/flags
//   * DATA / HEADERS      (6.1/6.2), END_STREAM, CONTINUATION (6.10)
//   * SETTINGS + ACK      (6.5), PING + ACK (6.7), GOAWAY (6.8),
//     WINDOW_UPDATE flow control (6.9), RST_STREAM (6.4)
//   * multiplexing        -- odd client stream ids over one connection
//   * header compression  -- HPACK (RFC 7541) STATIC table (61 entries) +
//     literal-without-indexing; no Huffman, no dynamic table in this
//     revision (both loopback ends are this implementation, so the
//     encoder never emits dynamic-table references).
//   * transport           -- h2c (cleartext prior knowledge, RFC 9113 3.4)
//     over TCP, or h2 over TLS with ALPN "h2" (NT-12 TlsProvider).
//
// Not covered (deliberate, documented): server push (PUSH_PROMISE is
// ignored on receipt, never advertised), priority trees, trailers after
// END_STREAM bodies, 100-continue, HPACK Huffman/dynamic-table integration.
// HTTP/2 has NO reason phrase (8.1.2.6) -- H2Response.reason is nullptr.
#pragma once
#include <chaos/net/net.h>
#include <chaos/net/net_api.h>
#include <chaos/net/http.h>
#include <chaos/net/tls.h>
#include <cstdint>

namespace chaos::net {

struct H2Request {
    HttpMethod method = HttpMethod::Get;
    const char* url = nullptr;     // "h2://host[:port]/path[?query]" (TLS,
                                   // ALPN "h2") or "h2c://..." (cleartext
                                   // prior knowledge)
    const HttpHeader* headers = nullptr;  // extra headers (pseudo-headers are
    CHAOS_IL2CPP_INT32 headerCount = 0;   // synthesized from method/url)
    const CHAOS_IL2CPP_UINT8* body = nullptr;  // for Post/Put
    CHAOS_IL2CPP_INT32 bodyLength = 0;
    bool disableCertificateValidation = false;  // test hook (h2:// only)
    CHAOS_IL2CPP_INT32 timeoutMs = 10000;      // connect/handshake/io
};

struct H2Response {
    CHAOS_IL2CPP_INT32 streamId = 0;     // the stream this response came from
    CHAOS_IL2CPP_INT32 statusCode = 0;   // 200, 404, ...
    const char* reason = nullptr;        // ALWAYS nullptr: HTTP/2 has no
                                         // reason phrase (RFC 9113 8.1.2.6)
    const HttpHeader* headers = nullptr; // owned; free with H2FreeResponse
    CHAOS_IL2CPP_INT32 headerCount = 0;
    const CHAOS_IL2CPP_UINT8* body = nullptr;  // owned; len bytes
    CHAOS_IL2CPP_INT32 bodyLength = 0;
};

struct H2Client;  // opaque: ONE HTTP/2 connection, N concurrent streams

// Establishes one HTTP/2 connection: TCP (+TLS with ALPN "h2" for h2://),
// client connection preface, SETTINGS exchange.  On success *out is a
// client that can carry multiple concurrent requests.  On failure *out is
// zeroed and a NetError is returned.
NetError H2Connect(const H2Request& req, H2Client** out) noexcept;

// Begins one request on a new stream (odd, incrementing stream id; stream
// ids are never reused within a connection).  Returns the stream id that
// H2RecvResponse must be called with.  Does NOT block for the response;
// call H2RecvResponse for each begun stream (any order -- the connection
// demultiplexes frames from all live streams).
NetError H2BeginRequest(H2Client* c, const H2Request& req,
                        CHAOS_IL2CPP_INT32* streamId) noexcept;

// Receives the complete response for one stream.  Frames belonging to other
// live streams are consumed and buffered transparently (multiplexing).
// Blocks up to timeoutMs (<=0: default).  On success *out owns storage that
// MUST be released with H2FreeResponse.  The stream is closed after this
// call; it cannot be requested twice.
NetError H2RecvResponse(H2Client* c, CHAOS_IL2CPP_INT32 streamId,
                        H2Response* out, CHAOS_IL2CPP_INT32 timeoutMs = -1) noexcept;

void H2FreeResponse(H2Response* resp) noexcept;

// Sends GOAWAY (best effort) and tears the connection down.
void H2Free(H2Client* c) noexcept;

// ── loopback server (test/diagnostic support) ────────────────────────────
// The same framing/HPACK machinery drives a minimal HTTP/2 server for the
// dual-platform loopback matrix.  One H2Server handles ONE accepted
// connection (with multiplexed streams); it is not a production server
// (no graceful shutdown signaling, minimal concurrency).

struct H2ServerRequest {
    CHAOS_IL2CPP_INT32 streamId = 0;
    HttpMethod method = HttpMethod::Get;
    const char* path = nullptr;       // owned by H2Server; valid until the
    const HttpHeader* headers = nullptr;  // next H2ServerNextRequest call
    CHAOS_IL2CPP_INT32 headerCount = 0;
    const CHAOS_IL2CPP_UINT8* body = nullptr;  // owned; bodyLength bytes
    CHAOS_IL2CPP_INT32 bodyLength = 0;
};

struct H2Server;  // opaque

// Accepts one connection on *listen; when tlsCert != nullptr wraps it in
// TLS server mode (ALPN advertises h2 + http/1.1, h2 preferred).  Reads the
// client connection preface, sends the server SETTINGS and ACKs the
// client's SETTINGS.  Returns ready (call H2ServerNextRequest next).
NetError H2ServerAccept(SocketHandle* listen, const TlsCertificate* tlsCert,
                        H2Server** out) noexcept;

// Returns the next COMPLETE request from any stream (server demultiplexes
// all streams; previously completed but unread requests are returned in
// arrival order, so multiplexed requests can be collected out of order).
// Fields are owned by the H2Server and stay valid until the next call.
// Returns ConnectionReset when the client closed the connection.
NetError H2ServerNextRequest(H2Server* s, H2ServerRequest* out,
                             CHAOS_IL2CPP_INT32 timeoutMs = -1) noexcept;

// Sends a response on an already-received request's stream.  HTTP/2 has no
// reason phrase, so only statusCode is sent (:status).  END_STREAM goes on
// the HEADERS when bodyLength==0, else on the last DATA frame.
NetError H2ServerRespond(H2Server* s, CHAOS_IL2CPP_INT32 streamId,
                         CHAOS_IL2CPP_INT32 statusCode,
                         const HttpHeader* headers,
                         CHAOS_IL2CPP_INT32 headerCount,
                         const CHAOS_IL2CPP_UINT8* body,
                         CHAOS_IL2CPP_INT32 bodyLength,
                         CHAOS_IL2CPP_INT32 timeoutMs = -1) noexcept;

void H2ServerFree(H2Server* s) noexcept;

}  // namespace chaos::net