// Layer H -- Server-Sent Events (SSE, WHATWG HTML §9.2) client (NT-14).
//
// Native SSE client over the Layer D transport + NT-9 DNS + NT-12 TLS, in
// the same native-route as NT-13 HttpClient / NT-14 WebSocket. One loopback
// GET connection per SseOpen; events are dispatched per the WHATWG parsing
// rules (data:/event:/id:/retry: fields, comment lines starting with ':',
// blank-line dispatch, final event at EOF).
#pragma once
#include <chaos/net/net.h>
#include <chaos/net/net_api.h>
#include <chaos/net/http.h>
#include <string>

namespace chaos::net {

struct SseOptions {
    const char* url = nullptr;    // http:// or https://host[:port]/path
    const HttpHeader* headers = nullptr;   // extra request headers (owned by caller)
    CHAOS_IL2CPP_INT32 headerCount = 0;
    bool disableCertificateValidation = false;  // https test hook
    CHAOS_IL2CPP_INT32 timeoutMs = 10000;
};

// One dispatched SSE event.
struct SseEvent {
    std::string event;   // last event: field, "message" if never set
    std::string data;    // data: lines joined with '\n'
    std::string id;      // last id: field ("" if never set)
    CHAOS_IL2CPP_INT64 retry = 0;  // retry: field in ms, 0 = never set
};

struct SseClient;  // opaque

// Opens the SSE stream: connects, sends "GET path HTTP/1.1" with
// "Accept: text/event-stream", and validates a 200 response.
NetError SseOpen(const SseOptions& opts, SseClient** out) noexcept;

// Reads the next event. Events become available on blank-line dispatch; a
// final event with pending data is dispatched when the server closes the
// connection. After the stream is exhausted (or the connection errors)
// returns ConnectionReset.
NetError SseNextEvent(SseClient* c, SseEvent* ev,
                      CHAOS_IL2CPP_INT32 timeoutMs = -1) noexcept;

void SseFree(SseClient* c) noexcept;

}  // namespace chaos::net
