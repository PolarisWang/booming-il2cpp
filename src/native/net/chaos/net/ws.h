// Layer H -- WebSocket (RFC 6455) client (NT-14).
//
// Native WebSocket client over the Layer D transport + NT-9 DNS + NT-12 TLS.
// Scope decision (recorded in the architecture doc §10 TODO note): native
// implementation -- same rationale as NT-13 HttpClient (no external deps,
// dual-platform single-source, loopback-testable in CI, reuses the 4-layer
// net stack already delivered).
//
// Covered (RFC 6455):
//   * opening handshake  (§4.1/4.2): GET Upgrade with Sec-WebSocket-Key,
//     response validated against Sec-WebSocket-Accept ==
//     base64(SHA-1(key + 258EAFA5-E914-47DA-95CA-C5AB0DC85B11)).
//   * framing (§5.2/5.3): FIN/opcode byte, MASK/len byte, 126/127 extended
//     lengths, client frames always masked, server frames accepted unmasked
//     (masked ones are unmasked defensively).
//   * control frames (§5.5): Ping auto-answered with Pong carrying the same
//     payload; Close handled by WsClose (send + await echo). Pong frames are
//     returned to the caller as ordinary frames.
//
// Not covered (deliberate, documented): per-message fragmentation state
// machine on receive (continuation frames are returned verbatim so callers
// can reassemble), subprotocol negotiation, extensions (permessage-deflate).
#pragma once
#include <chaos/net/net.h>
#include <chaos/net/net_api.h>
#include <chaos/net/http.h>
#include <cstddef>

namespace chaos::net {

// RFC 6455 §5.2 opcodes.
enum class WsOpcode : CHAOS_IL2CPP_INT32 {
    Continuation = 0x0,   // §5.4 fragmentation
    Text         = 0x1,   // UTF-8 text
    Binary       = 0x2,
    Close        = 0x8,
    Ping         = 0x9,
    Pong         = 0xA,
};

struct WsOptions {
    const char* url = nullptr;      // ws:// or wss://host[:port]/path[?query]
    const HttpHeader* headers = nullptr;  // extra handshake headers (owned by caller)
    CHAOS_IL2CPP_INT32 headerCount = 0;
    const char* subprotocol = nullptr;    // Sec-WebSocket-Protocol value (optional)
    bool disableCertificateValidation = false;  // wss test hook (same as TLS)
    CHAOS_IL2CPP_INT32 timeoutMs = 10000;
};

// One received frame. payload points into client-owned storage valid until
// the next WsRecv/WsClose call; closeCode/closeReason valid for Close frames.
struct WsFrame {
    WsOpcode opcode = WsOpcode::Continuation;
    bool fin = true;
    const CHAOS_IL2CPP_UINT8* payload = nullptr;
    CHAOS_IL2CPP_INT32 payloadLength = 0;
    CHAOS_IL2CPP_UINT16 closeCode = 0;
    const char* closeReason = nullptr;   // may be "" for empty reason
};

struct WsClient;  // opaque

// Opens a ws:// or wss:// connection: TCP (+TLS for wss) then the RFC 6455
// opening handshake with Sec-WebSocket-Accept validation.
NetError WsOpen(const WsOptions& opts, WsClient** out) noexcept;

// Sends one frame. fin=true unless fragmentation is wanted (begin a message
// with fin=false + opcode Text/Binary, continue with WsOpcode::Continuation
// frames and end with fin=true). Payloads > 125 bytes use the 126/127
// extended-length forms; control opcodes (Ping/Pong/Close) must be <= 125.
NetError WsSendFrame(WsClient* c, WsOpcode opcode, bool fin,
                     const CHAOS_IL2CPP_UINT8* data, CHAOS_IL2CPP_INT32 len,
                     CHAOS_IL2CPP_INT32 timeoutMs = -1) noexcept;

// Convenience: fin=true single-frame message.
inline NetError WsSend(WsClient* c, WsOpcode opcode,
                       const CHAOS_IL2CPP_UINT8* data, CHAOS_IL2CPP_INT32 len,
                       CHAOS_IL2CPP_INT32 timeoutMs = -1) noexcept {
    return WsSendFrame(c, opcode, true, data, len, timeoutMs);
}

// Receives one frame (data or control). Ping frames are auto-answered with a
// Pong (same payload) and not surfaced; Pong and Close frames are returned
// as-is. Blocks up to timeoutMs (<=0: default).
NetError WsRecv(WsClient* c, WsFrame* out, CHAOS_IL2CPP_INT32 timeoutMs = -1) noexcept;

// Close handshake (§5.5.1): sends a Close frame (code + reason), then awaits
// the peer's Close echo frame. Returns None once the echo arrived (the
// transport is torn down by WsFree).
NetError WsClose(WsClient* c, CHAOS_IL2CPP_UINT16 code, const char* reason,
                 CHAOS_IL2CPP_INT32 timeoutMs = -1) noexcept;

void WsFree(WsClient* c) noexcept;

// Server-side handshake helper (used by test servers/loopback): computes the
// Sec-WebSocket-Accept value for a client's Sec-WebSocket-Key.
// out must hold at least 29 bytes (28 chars + NUL). Returns false on error.
bool WsComputeAccept(const char* clientKey, char* out, std::size_t outSize) noexcept;

constexpr CHAOS_IL2CPP_INT32 kWsMaxFrameBytes = 16 * 1024 * 1024;  // sanity cap

}  // namespace chaos::net
