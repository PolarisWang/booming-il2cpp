// Layer H -- SSE (Server-Sent Events) client implementation (NT-14).
// See sse.h. One GET connection per stream; WHATWG parsing (§9.2) with
// blank-line dispatch, comment lines, and EOF-flush of a final event.
#include <chaos/net/sse.h>
#include <chaos/net/dns.h>
#include <chaos/net/tls.h>

#include <algorithm>
#include <cctype>
#include <cstring>
#include <string>
#include <vector>

namespace chaos::net {
namespace {

bool ParseSseUrl(const char* url, bool* https_, std::string* host,
                 CHAOS_IL2CPP_UINT16* port, std::string* path) {
    std::string s = url;
    *https_ = false;
    if (s.rfind("http://", 0) == 0) {
        s = s.substr(7);
    } else if (s.rfind("https://", 0) == 0) {
        *https_ = true;
        s = s.substr(8);
    } else {
        return false;
    }
    std::size_t slash = s.find('/');
    std::string authority = slash == std::string::npos ? s : s.substr(0, slash);
    *path = slash == std::string::npos ? "/" : s.substr(slash);
    std::size_t colon = authority.rfind(':');
    if (colon == std::string::npos) {
        *host = authority;
        *port = *https_ ? 443 : 80;
    } else {
        *host = authority.substr(0, colon);
        long p = std::strtol(authority.c_str() + colon + 1, nullptr, 10);
        if (p <= 0 || p > 65535) return false;
        *port = static_cast<CHAOS_IL2CPP_UINT16>(p);
    }
    if (host->empty()) return false;
    for (auto& ch : *host)
        ch = static_cast<char>(std::tolower(static_cast<unsigned char>(ch)));
    return true;
}

struct SseConn {
    SocketHandle sock{};
    TlsProvider* tls = nullptr;
    std::vector<CHAOS_IL2CPP_UINT8> buf;
    std::size_t pos = 0;
    bool dead = false;
    // pending event accumulator (WHATWG "current event" buffer)
    std::string eventName;   // "message" default
    std::string data;
    std::string id;
    CHAOS_IL2CPP_INT64 retry = 0;
    bool sawAnyField = false;    // data/event/id/retry seen since last dispatch
    bool eofFinalDispatched = false;
};

constexpr std::size_t kReadChunk = 16384;

NetError Fill(SseConn* c, CHAOS_IL2CPP_INT32 timeoutMs) {
    if (c->pos < c->buf.size()) return NetError::None;
    c->buf.clear();
    c->pos = 0;
    c->buf.resize(kReadChunk);
    if (c->tls) {
        CHAOS_IL2CPP_UINT32 got = 0;
        NetError err = c->tls->Read(&c->sock, c->buf.data(),
                                    static_cast<CHAOS_IL2CPP_UINT32>(c->buf.size()),
                                    &got);
        if (err != NetError::None || got == 0) {
            c->dead = true;
            return err != NetError::None ? err : NetError::ConnectionReset;
        }
        c->buf.resize(static_cast<std::size_t>(got));
        return NetError::None;
    }
    CHAOS_IL2CPP_INT32 got = 0;
    NetError err = NetSocketRecv(&c->sock, c->buf.data(),
                                 static_cast<CHAOS_IL2CPP_INT32>(c->buf.size()), 0,
                                 &got, timeoutMs);
    if (err != NetError::None || got <= 0) {
        c->dead = true;
        return err != NetError::None ? err : NetError::ConnectionReset;
    }
    c->buf.resize(static_cast<std::size_t>(got));
    return NetError::None;
}

// Pulls the next line; returns false on EOF/error. out excludes CRLF.
// A trailing bare "\r" on the final line is stripped too.
bool ReadLine(SseConn* c, std::string* out, CHAOS_IL2CPP_INT32 timeoutMs) {
    out->clear();
    for (;;) {
        NetError err = Fill(c, timeoutMs);
        if (err != NetError::None) return false;
        std::size_t avail = c->buf.size() - c->pos;
        for (std::size_t i = 0; i < avail; ++i) {
            if (c->buf[c->pos + i] == '\n') {
                std::size_t len = i;
                if (len > 0 && c->buf[c->pos + len - 1] == '\r') --len;
                out->append(reinterpret_cast<const char*>(c->buf.data() + c->pos), len);
                c->pos += i + 1;
                return true;
            }
        }
        out->append(reinterpret_cast<const char*>(c->buf.data() + c->pos), avail);
        c->pos = c->buf.size();
    }
}

NetError SendAll(SseConn* c, const CHAOS_IL2CPP_UINT8* data, std::size_t n,
                 CHAOS_IL2CPP_INT32 timeoutMs) {
    std::size_t done = 0;
    while (done < n) {
        if (c->tls) {
            CHAOS_IL2CPP_UINT32 wrote = 0;
            NetError err = c->tls->Write(&c->sock, data + done,
                                         static_cast<CHAOS_IL2CPP_UINT32>(n - done),
                                         &wrote);
            if (err != NetError::None || wrote == 0) {
                c->dead = true;
                return err != NetError::None ? err : NetError::ConnectionReset;
            }
            done += static_cast<std::size_t>(wrote);
            continue;
        }
        CHAOS_IL2CPP_INT32 sent = 0;
        NetError err = NetSocketSend(&c->sock, data + done,
                                     static_cast<CHAOS_IL2CPP_INT32>(n - done), 0,
                                     &sent, timeoutMs);
        if (err != NetError::None || sent <= 0) {
            c->dead = true;
            return err != NetError::None ? err : NetError::ConnectionReset;
        }
        done += static_cast<std::size_t>(sent);
    }
    return NetError::None;
}

void Dispatch(SseConn* c, SseEvent* ev) {
    ev->event = c->eventName.empty() ? "message" : c->eventName;
    ev->data = c->data;
    ev->id = c->id;
    ev->retry = c->retry;
    c->eventName.clear();
    c->data.clear();
    c->sawAnyField = false;
    // id/retry persist across events per WHATWG (last-event-id).
}

// One parsed line -> apply field. Returns true if a dispatch happened.
bool ApplyLine(SseConn* c, const std::string& line) {
    if (line.empty()) {
        if (c->sawAnyField) return true;  // blank line dispatches
        // Empty line with nothing pending: "dispatch an event that just
        // resets the buffers" -- nothing to surface.
        return false;
    }
    if (line[0] == ':') return false;  // comment
    std::size_t colon = line.find(':');
    std::string field = colon == std::string::npos ? line : line.substr(0, colon);
    std::string value = colon == std::string::npos ? "" : line.substr(colon + 1);
    if (!value.empty() && value[0] == ' ') value.erase(0, 1);

    if (field == "data") {
        if (c->data.empty()) c->data = value;
        else { c->data.push_back('\n'); c->data += value; }
        c->sawAnyField = true;
    } else if (field == "event") {
        c->eventName = value;
        c->sawAnyField = true;
    } else if (field == "id") {
        // strip trailing NULs per spec
        while (!value.empty() && value.back() == '\0') value.pop_back();
        c->id = value;
        c->sawAnyField = true;
    } else if (field == "retry") {
        long v = std::strtol(value.c_str(), nullptr, 10);
        if (v > 0) { c->retry = v; c->sawAnyField = true; }
    }
    return false;
}

}  // namespace

NetError SseOpen(const SseOptions& opts, SseClient** out) noexcept {
    *out = nullptr;
    if (opts.url == nullptr || opts.url[0] == 0) return NetError::Unsupported;
    bool https_ = false;
    std::string host, path;
    CHAOS_IL2CPP_UINT16 port = 0;
    if (!ParseSseUrl(opts.url, &https_, &host, &port, &path))
        return NetError::Unsupported;

    SseConn* c = new SseConn();
    NetError err = NetSocketCreate(kAddressFamilyInet, kSocketTypeStream,
                                   kProtocolTcp, &c->sock);
    if (err == NetError::None)
        err = NetSocketSetOption(&c->sock, NetOption::NoDelay, 1);
    NetAddress addr{};
    if (err == NetError::None) err = DnsResolveEndPoint(host.c_str(), port, &addr);
    if (err == NetError::None) err = NetSocketConnect(&c->sock, addr, opts.timeoutMs);
    if (err == NetError::None && https_) {
#ifdef _WIN32
        c->tls = CreateSchannelProvider();
#else
        c->tls = CreateOpenSslProvider();
#endif
        TlsOptions to;
        to.mode = TlsMode::Client;
        to.serverName = host.c_str();
        to.disableCertificateValidation = opts.disableCertificateValidation;
        to.timeoutMs = opts.timeoutMs;
        err = c->tls->Create(to);
        if (err == NetError::None) err = c->tls->Handshake(&c->sock);
    }

    if (err == NetError::None) {
        std::string req = "GET " + path + " HTTP/1.1\r\n";
        req += "Host: " + host + ":" + std::to_string(port) + "\r\n";
        req += "Accept: text/event-stream\r\n";
        req += "Connection: close\r\n";
        for (CHAOS_IL2CPP_INT32 i = 0; i < opts.headerCount; ++i) {
            req += opts.headers[i].name;
            req += ": ";
            req += opts.headers[i].value;
            req += "\r\n";
        }
        req += "\r\n";
        err = SendAll(c, reinterpret_cast<const CHAOS_IL2CPP_UINT8*>(req.data()),
                      req.size(), opts.timeoutMs);
    }
    if (err == NetError::None) {
        // status line
        std::string status;
        if (!ReadLine(c, &status, opts.timeoutMs)) err = NetError::ConnectionReset;
        else if (status.find(" 200 ") == std::string::npos) err = NetError::Unsupported;
    }
    if (err == NetError::None) {
        // headers through the blank line
        for (;;) {
            std::string line;
            if (!ReadLine(c, &line, opts.timeoutMs)) { err = NetError::ConnectionReset; break; }
            if (line.empty()) break;
        }
    }
    if (err != NetError::None) {
        if (c->tls) { c->tls->Shutdown(); delete c->tls; }
        if (c->sock.fd >= 0) NetSocketClose(&c->sock);
        delete c;
        return err;
    }
    *out = reinterpret_cast<SseClient*>(c);
    return NetError::None;
}

NetError SseNextEvent(SseClient* c, SseEvent* ev,
                      CHAOS_IL2CPP_INT32 timeoutMs) noexcept {
    SseConn* w = reinterpret_cast<SseConn*>(c);
    if (w == nullptr || ev == nullptr || w->dead) return NetError::ConnectionReset;
    for (;;) {
        std::string line;
        if (!ReadLine(w, &line, timeoutMs)) {
            // EOF: dispatch a final event if anything is pending.
            if (w->sawAnyField && !w->eofFinalDispatched) {
                w->eofFinalDispatched = true;
                Dispatch(w, ev);
                return NetError::None;
            }
            w->dead = true;
            return NetError::ConnectionReset;
        }
        if (ApplyLine(w, line)) {
            Dispatch(w, ev);
            return NetError::None;
        }
    }
}

void SseFree(SseClient* c) noexcept {
    if (c == nullptr) return;
    SseConn* w = reinterpret_cast<SseConn*>(c);
    if (w->tls) { w->tls->Shutdown(); delete w->tls; }
    if (w->sock.fd >= 0) NetSocketClose(&w->sock);
    delete w;
}

}  // namespace chaos::net
