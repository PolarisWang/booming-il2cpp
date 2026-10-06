// Layer G -- native HTTP client implementation (NT-13).  See http.h for the
// surface and the decision rationale (native over managed BCL/libcurl).
// Layering:
//   * URL parse        -- inline (http/https://host[:port]/path[?query])
//   * DNS              -- DnsResolveEndPoint (NT-9)
//   * transport        -- NetSocket* (Layer D, NT-6/7)
//   * TLS (https)      -- TlsProvider via CreateSchannelProvider /
//                         CreateOpenSslProvider (NT-12)
//   * framing          -- HTTP/1.1 request; response body by Content-Length,
//                         Transfer-Encoding: chunked, or read-to-close
//   * connection pool  -- one keep-alive connection per scheme://host:port,
//                         reused across HttpSend calls (mutex-guarded)
//
// All functions noexcept.  HttpResponse owns its storage: headers live in
// ONE malloc block (array followed by the name\0value\0 blob), body and
// reason each in their own block; HttpFreeResponse frees all three.
#include <chaos/net/http.h>
#include <chaos/net/tls.h>
#include <chaos/net/dns.h>

#include <cstring>
#include <cstdlib>
#include <cctype>
#include <mutex>
#include <string>
#include <vector>

namespace chaos::net {
namespace {

constexpr CHAOS_IL2CPP_INT32 kMaxHeaderBytes = 1 << 20;   // 1 MiB headers
constexpr CHAOS_IL2CPP_INT32 kMaxBodyBytes   = 64 << 20;  // 64 MiB body
constexpr CHAOS_IL2CPP_INT32 kMaxHeaderCount = 64;
constexpr int kPoolSize    = 8;
constexpr int kChunkRead   = 16384;

// One live HTTP connection (socket + optional TLS + read buffer).
struct HttpConnection {
    std::string key;                 // "scheme://host:port" (lowercased host)
    SocketHandle sock{};
    TlsProvider* tls = nullptr;      // owned; null for plain http
    std::vector<uint8_t> rbuf;       // buffered bytes not yet consumed
    size_t rpos = 0;                 // first unconsumed byte in rbuf
    bool inPool = false;             // parked in the pool (owned by pool)

    HttpConnection() { sock.fd = -1; }
    ~HttpConnection() { Close(); }

    HttpConnection(const HttpConnection&) = delete;
    HttpConnection& operator=(const HttpConnection&) = delete;

    void Close() noexcept {
        if (tls) tls->Shutdown();
        if (sock.fd >= 0) NetSocketClose(&sock);
        delete tls;
        tls = nullptr;
        sock.fd = -1;
        rbuf.clear();
        rpos = 0;
    }

    // Fill the buffer from the wire.  Returns ConnectionReset on clean EOF
    // (TLS close_notify / recv->0), TimedOut/other errors otherwise.
    NetError FillOnce(int timeoutMs) noexcept {
        if (rpos < rbuf.size()) return NetError::None;
        rbuf.clear();
        rpos = 0;
        if (tls) {
            CHAOS_IL2CPP_UINT32 got = 0;
            NetError e = tls->Read(&sock, rbufEmplace(), kChunkRead, &got);
            if (e != NetError::None) return e;
            if (got == 0) return NetError::ConnectionReset;
            rbuf.resize(got);
        } else {
            CHAOS_IL2CPP_INT32 got = 0;
            NetError e = NetSocketRecv(&sock, rbufEmplace(), kChunkRead, 0,
                                       &got, timeoutMs);
            if (e != NetError::None) return e;
            if (got == 0) return NetError::ConnectionReset;
            rbuf.resize(static_cast<size_t>(got));
        }
        return NetError::None;
    }
    uint8_t* rbufEmplace() {
        rbuf.resize(kChunkRead);
        return rbuf.data();
    }

    // True when the buffer holds a CRLF-terminated line; *lineLen excludes
    // the CRLF.  Fills until found, error, or EOF.
    bool ScanLine(size_t* lineLen, int timeoutMs, NetError* err) noexcept {
        for (;;) {
            for (size_t i = rpos; i + 1 < rbuf.size(); ++i) {
                if (rbuf[i] == '\r' && rbuf[i + 1] == '\n') {
                    *lineLen = i - rpos;
                    return true;
                }
            }
            if (rbuf.size() - rpos > static_cast<size_t>(kMaxHeaderBytes)) {
                *err = NetError::Unsupported;
                return false;
            }
            NetError e = FillOnce(timeoutMs);
            if (e != NetError::None) { *err = e; return false; }
        }
    }

    // Read one CRLF-terminated line into out (without the CRLF).
    NetError ReadLine(std::vector<uint8_t>* out, int timeoutMs) noexcept {
        size_t lineLen = 0;
        NetError e = NetError::None;
        if (!ScanLine(&lineLen, timeoutMs, &e)) return e;
        out->assign(rbuf.begin() + static_cast<ptrdiff_t>(rpos),
                    rbuf.begin() + static_cast<ptrdiff_t>(rpos + lineLen));
        rpos += lineLen + 2;
        return NetError::None;
    }

    // Consume exactly n bytes into dst (fill as needed).  EOF before n is
    // ConnectionReset.
    NetError ReadBytes(uint8_t* dst, size_t n, int timeoutMs) noexcept {
        size_t done = 0;
        while (done < n) {
            if (rpos >= rbuf.size()) {
                NetError e = FillOnce(timeoutMs);
                if (e != NetError::None) return e;
            }
            size_t avail = rbuf.size() - rpos;
            size_t take = avail < (n - done) ? avail : (n - done);
            std::memcpy(dst + done, rbuf.data() + rpos, take);
            rpos += take;
            done += take;
        }
        return NetError::None;
    }

    // Consume up to cap bytes into dst; *got receives the count (0 = EOF).
    NetError ReadSome(uint8_t* dst, size_t cap, int timeoutMs,
                      size_t* got) noexcept {
        if (rpos < rbuf.size()) {
            size_t avail = rbuf.size() - rpos;
            size_t take = avail < cap ? avail : cap;
            std::memcpy(dst, rbuf.data() + rpos, take);
            rpos += take;
            *got = take;
            return NetError::None;
        }
        if (tls) {
            CHAOS_IL2CPP_UINT32 g = 0;
            NetError e = tls->Read(&sock, dst,
                                   static_cast<CHAOS_IL2CPP_UINT32>(cap), &g);
            if (e != NetError::None) return e;
            *got = g;
        } else {
            CHAOS_IL2CPP_INT32 g = 0;
            NetError e = NetSocketRecv(&sock, dst,
                                       static_cast<CHAOS_IL2CPP_INT32>(cap),
                                       0, &g, timeoutMs);
            if (e != NetError::None) return e;
            *got = static_cast<size_t>(g);
        }
        if (*got == 0) return NetError::ConnectionReset;
        return NetError::None;
    }

    // Send everything (loop over partial writes).
    NetError SendAll(const uint8_t* data, size_t n, int timeoutMs) noexcept {
        size_t done = 0;
        while (done < n) {
            if (tls) {
                CHAOS_IL2CPP_UINT32 wrote = 0;
                NetError e = tls->Write(&sock, data + done,
                                        static_cast<CHAOS_IL2CPP_UINT32>(n - done),
                                        &wrote);
                if (e != NetError::None) return e;
                if (wrote == 0) return NetError::ConnectionReset;
                done += wrote;
            } else {
                CHAOS_IL2CPP_INT32 sent = 0;
                NetError e = NetSocketSend(&sock, data + done,
                                           static_cast<CHAOS_IL2CPP_INT32>(n - done),
                                           0, &sent, timeoutMs);
                if (e != NetError::None) return e;
                if (sent <= 0) return NetError::ConnectionReset;
                done += static_cast<size_t>(sent);
            }
        }
        return NetError::None;
    }
};

// ── pool ───────────────────────────────────────────────────────────────
std::mutex g_poolMutex;
HttpConnection* g_pool[kPoolSize] = {};

HttpConnection* PoolTake(const std::string& key) {
    std::lock_guard<std::mutex> lock(g_poolMutex);
    for (int i = 0; i < kPoolSize; ++i) {
        if (g_pool[i] && g_pool[i]->inPool && g_pool[i]->key == key) {
            g_pool[i]->inPool = false;
            HttpConnection* c = g_pool[i];
            g_pool[i] = nullptr;  // vacate the slot so PoolPut reuses it
            return c;
        }
    }
    return nullptr;
}

void PoolPut(HttpConnection* c) {
    std::lock_guard<std::mutex> lock(g_poolMutex);
    for (int i = 0; i < kPoolSize; ++i) {
        if (!g_pool[i]) {
            c->inPool = true;
            g_pool[i] = c;
            return;
        }
    }
    // pool full -- close the connection instead
    c->inPool = false;
    c->Close();
    delete c;
}

// ── URL parse ──────────────────────────────────────────────────────────
bool ParseUrl(const char* url, bool* https, std::string* host,
              CHAOS_IL2CPP_UINT16* port, std::string* path) {
    if (!url || !*url) return false;
    const char* p = url;
    if (std::strncmp(p, "http://", 7) == 0) {
        p += 7;
        *https = false;
    } else if (std::strncmp(p, "https://", 8) == 0) {
        p += 8;
        *https = true;
    } else {
        return false;
    }
    const char* start = p;
    while (*p && *p != ':' && *p != '/' && *p != '?') ++p;
    if (p == start) return false;
    *host = std::string(start, p);
    if (*p == ':') {
        // parse port
        long portVal = 0;
        const char* q = p + 1;
        bool any = false;
        while (*q && std::isdigit(static_cast<unsigned char>(*q))) {
            portVal = portVal * 10 + (*q - '0');
            if (portVal > 65535) return false;
            any = true;
            ++q;
        }
        if (!any) return false;
        *port = static_cast<CHAOS_IL2CPP_UINT16>(portVal);
        p = q;
    } else {
        *port = *https ? 443 : 80;
    }
    if (*p == '/' || *p == '?') {
        *path = std::string(p);
    } else {
        *path = "/";
    }
    // lowercase host for the pool key / Host header
    for (char& ch : *host) {
        ch = static_cast<char>(std::tolower(static_cast<unsigned char>(ch)));
    }
    return true;
}

const char* MethodName(HttpMethod m) {
    switch (m) {
        case HttpMethod::Get:    return "GET";
        case HttpMethod::Post:   return "POST";
        case HttpMethod::Put:    return "PUT";
        case HttpMethod::Delete: return "DELETE";
        case HttpMethod::Head:   return "HEAD";
    }
    return "GET";
}

// Header list during parsing (name/value as std::strings).
struct RawHeader {
    std::string name;
    std::string value;
};

void Trim(std::string* s) {
    size_t b = 0;
    while (b < s->size() && (s->at(b) == ' ' || s->at(b) == '\t')) ++b;
    size_t e = s->size();
    while (e > b && (s->at(e - 1) == ' ' || s->at(e - 1) == '\t')) --e;
    s->assign(s->substr(b, e - b));
}

const char* FindHeader(const std::vector<RawHeader>& hdrs, const char* name) {
    size_t nameLen = std::strlen(name);
    size_t totalHeaderBytes = 0;
    (void)totalHeaderBytes;
    for (size_t i = 0; i < hdrs.size(); ++i) {
        if (hdrs[i].name.size() != nameLen) continue;
        bool eq = true;
        for (size_t j = 0; j < nameLen; ++j) {
            char a = static_cast<char>(std::tolower(
                static_cast<unsigned char>(hdrs[i].name[j])));
            char b = static_cast<char>(std::tolower(
                static_cast<unsigned char>(name[j])));
            if (a != b) { eq = false; break; }
        }
        if (eq) return hdrs[i].value.c_str();
    }
    return nullptr;
}

bool HeaderValueIs(const char* v, const char* expected) {
    while (*v == ' ' || *v == '\t') ++v;
    size_t n = std::strlen(expected);
    return std::strncmp(v, expected, n) == 0 &&
           (v[n] == '\0' || v[n] == ',' ||
            v[n] == ' ' || v[n] == '\t');
}

// Parse a chunk-size line ("1A" or "1A;ext=...").  Returns hex size, -1 on
// error.
long ParseChunkSize(const std::vector<uint8_t>& line) {
    long v = 0;
    size_t i = 0;
    while (i < line.size()) {
        char c = static_cast<char>(line[i]);
        if (c == ';' || c == '\r' || c == '\n') break;
        v *= 16;
        if (c >= '0' && c <= '9')      v += c - '0';
        else if (c >= 'a' && c <= 'f') v += c - 'a' + 10;
        else if (c >= 'A' && c <= 'F') v += c - 'A' + 10;
        else return -1;
        if (v > kMaxBodyBytes) return -1;
        ++i;
    }
    if (i == 0) return -1;
    return v;
}

// Read a chunked-encoded body into *body.
NetError ReadChunked(HttpConnection* conn, std::vector<uint8_t>* body,
                     int timeoutMs) {
    std::vector<uint8_t> line;
    std::vector<uint8_t> chunkData;
    for (;;) {
        line.clear();
        NetError e = conn->ReadLine(&line, timeoutMs);
        if (e != NetError::None) return e;
        long size = ParseChunkSize(line);
        if (size < 0) return NetError::Unsupported;
        if (size == 0) {
            // trailers until blank line
            for (;;) {
                line.clear();
                e = conn->ReadLine(&line, timeoutMs);
                if (e != NetError::None) return e;
                if (line.empty()) break;
            }
            return NetError::None;
        }
        if (static_cast<long>(body->size()) + size > kMaxBodyBytes)
            return NetError::Unsupported;
        chunkData.resize(static_cast<size_t>(size));
        e = conn->ReadBytes(chunkData.data(), static_cast<size_t>(size),
                            timeoutMs);
        if (e != NetError::None) return e;
        body->insert(body->end(), chunkData.begin(), chunkData.end());
        // trailing CRLF
        uint8_t crlf[2];
        e = conn->ReadBytes(crlf, 2, timeoutMs);
        if (e != NetError::None) return e;
    }
}

}  // namespace

// ── public API ─────────────────────────────────────────────────────────
NetError HttpSend(const HttpRequest& req, HttpResponse* out) noexcept {
    if (out) *out = HttpResponse{};
    if (!req.url) return NetError::Unsupported;

    bool https = false;
    std::string host;
    CHAOS_IL2CPP_UINT16 port = 0;
    std::string path;
    if (!ParseUrl(req.url, &https, &host, &port, &path))
        return NetError::Unsupported;

    std::string key = (https ? "https://" : "http://") + host + ":" +
                      std::to_string(port);

    HttpConnection* conn = nullptr;
    NetError err = NetError::None;

    // Reuse a pooled connection when available; retry once with a fresh
    // connection if the stale one fails to send.
    int attempts = 2;
    bool reusedPooled = false;
    while (attempts-- > 0) {
        conn = PoolTake(key);
        reusedPooled = (conn != nullptr);
        if (!conn) {
            conn = new HttpConnection();
            conn->key = key;
            NetAddress addr{};
            err = NetSocketCreate(kAddressFamilyInet, kSocketTypeStream,
                                  kProtocolTcp, &conn->sock);
            if (err != NetError::None) break;
            NetSocketSetOption(&conn->sock, NetOption::NoDelay, 1);
            err = DnsResolveEndPoint(host.c_str(), port, &addr);
            if (err != NetError::None) break;
            err = NetSocketConnect(&conn->sock, addr, req.timeoutMs);
            if (err != NetError::None) break;
            if (https) {
#ifdef _WIN32
                conn->tls = CreateSchannelProvider();
#else
                conn->tls = CreateOpenSslProvider();
#endif
                if (!conn->tls) { err = NetError::Unsupported; break; }
                TlsOptions opts;
                opts.mode = TlsMode::Client;
                opts.serverName = host.c_str();
                opts.disableCertificateValidation =
                    req.disableCertificateValidation;
                opts.timeoutMs = req.timeoutMs;
                err = conn->tls->Create(opts);
                if (err != NetError::None) break;
                err = conn->tls->Handshake(&conn->sock);
                if (err != NetError::None) break;
            }
        }

        // Build request (HTTP/1.1 keep-alive).
        std::string wire = std::string(MethodName(req.method)) + " " + path +
                           " HTTP/1.1\r\nHost: " + host + ":" +
                           std::to_string(port) + "\r\n";
        for (CHAOS_IL2CPP_INT32 i = 0; i < req.headerCount; ++i) {
            wire += req.headers[i].name;
            wire += ": ";
            wire += req.headers[i].value;
            wire += "\r\n";
        }
        if (req.body && req.bodyLength > 0) {
            wire += "Content-Length: " + std::to_string(req.bodyLength) +
                    "\r\n";
        }
        wire += "\r\n";
        if (req.body && req.bodyLength > 0) {
            wire.append(reinterpret_cast<const char*>(req.body),
                        static_cast<size_t>(req.bodyLength));
        }

        err = conn->SendAll(reinterpret_cast<const uint8_t*>(wire.data()),
                            wire.size(), req.timeoutMs);
        if (err == NetError::None) break;
        // stale pooled connection -- close it and retry once on a fresh one
        if (reusedPooled) {
            conn->Close();
            delete conn;
            conn = nullptr;
            continue;
        }
        break;
    }

    if (err != NetError::None) {
        if (conn) { conn->Close(); delete conn; }
        return err;
    }

    // ── response ────────────────────────────────────────────────────────
    std::vector<uint8_t> line;
    err = conn->ReadLine(&line, req.timeoutMs);
    if (err != NetError::None) { conn->Close(); delete conn; return err; }

    // status line: "HTTP/1.1 200 OK"
    CHAOS_IL2CPP_INT32 status = 0;
    std::string reason;
    {
        std::string sl(line.begin(), line.end());
        // find second space
        size_t sp = sl.find(' ');
        if (sp != std::string::npos) {
            size_t sp2 = sl.find(' ', sp + 1);
            if (sp2 != std::string::npos) {
                std::string code = sl.substr(sp + 1, sp2 - sp - 1);
                status = static_cast<CHAOS_IL2CPP_INT32>(std::strtol(
                    code.c_str(), nullptr, 10));
                reason = sl.substr(sp2 + 1);
            }
        }
    }

    std::vector<RawHeader> hdrs;
    size_t headerBytes = 0;
    for (;;) {
        line.clear();
        err = conn->ReadLine(&line, req.timeoutMs);
        if (err != NetError::None) { conn->Close(); delete conn; return err; }
        if (line.empty()) break;
        headerBytes += line.size() + 2;
        if (headerBytes > static_cast<size_t>(kMaxHeaderBytes) ||
            hdrs.size() >= static_cast<size_t>(kMaxHeaderCount)) {
            conn->Close(); delete conn;
            return NetError::Unsupported;
        }
        size_t colon = std::string::npos;
        for (size_t i = 0; i < line.size(); ++i) {
            if (line[i] == ':') { colon = i; break; }
        }
        if (colon == std::string::npos) {
            conn->Close(); delete conn;
            return NetError::Unsupported;
        }
        RawHeader h;
        h.name.assign(line.begin(), line.begin() + static_cast<ptrdiff_t>(colon));
        h.value.assign(line.begin() + static_cast<ptrdiff_t>(colon + 1),
                       line.end());
        Trim(&h.name);
        Trim(&h.value);
        hdrs.push_back(std::move(h));
    }

    bool keepAlive = true;
    const char* connHdr = FindHeader(hdrs, "Connection");
    if (connHdr && HeaderValueIs(connHdr, "close")) keepAlive = false;

    std::vector<uint8_t> body;
    bool readBody = !(req.method == HttpMethod::Head) && status != 204 &&
                    status != 304;
    if (readBody) {
        const char* te = FindHeader(hdrs, "Transfer-Encoding");
        if (te && HeaderValueIs(te, "chunked")) {
            err = ReadChunked(conn, &body, req.timeoutMs);
        } else {
            const char* cl = FindHeader(hdrs, "Content-Length");
            if (cl) {
                long n = std::strtol(cl, nullptr, 10);
                if (n < 0) n = 0;
                if (n > kMaxBodyBytes) n = kMaxBodyBytes;
                body.resize(static_cast<size_t>(n));
                err = conn->ReadBytes(body.data(), static_cast<size_t>(n),
                                      req.timeoutMs);
            } else {
                // read-to-close; connection is done
                keepAlive = false;
                uint8_t tmp[kChunkRead];
                for (;;) {
                    size_t got = 0;
                    NetError e = conn->ReadSome(tmp, kChunkRead,
                                                req.timeoutMs, &got);
                    if (e == NetError::ConnectionReset) break;
                    if (e != NetError::None) { err = e; break; }
                    body.insert(body.end(), tmp, tmp + got);
                    if (body.size() > static_cast<size_t>(kMaxBodyBytes)) {
                        err = NetError::Unsupported;
                        break;
                    }
                }
            }
        }
        if (err != NetError::None) {
            conn->Close();
            delete conn;
            return err;
        }
    }

    // Hand the response to the caller: headers as ONE malloc block (array +
    // blob), body + reason each own blocks.  HttpFreeResponse frees all.
    size_t blobSize = 0;
    for (const RawHeader& h : hdrs) blobSize += h.name.size() + 1 +
                                                h.value.size() + 1;
    char* block = static_cast<char*>(std::malloc(
        hdrs.size() * sizeof(HttpHeader) + blobSize));
    if (!block) { conn->Close(); delete conn; return NetError::Unsupported; }
    HttpHeader* hdrsOut = reinterpret_cast<HttpHeader*>(block);
    char* blob = block + hdrs.size() * sizeof(HttpHeader);
    for (size_t i = 0; i < hdrs.size(); ++i) {
        std::memcpy(blob, hdrs[i].name.data(), hdrs[i].name.size());
        blob[hdrs[i].name.size()] = '\0';
        std::memcpy(blob + hdrs[i].name.size() + 1, hdrs[i].value.data(),
                    hdrs[i].value.size());
        blob[hdrs[i].name.size() + 1 + hdrs[i].value.size()] = '\0';
        hdrsOut[i].name = blob;
        hdrsOut[i].value = blob + hdrs[i].name.size() + 1;
        blob += hdrs[i].name.size() + 1 + hdrs[i].value.size() + 1;
    }
    out->statusCode = status;
    out->headerCount = static_cast<CHAOS_IL2CPP_INT32>(hdrs.size());
    out->headers = hdrsOut;
    if (!body.empty()) {
        out->body = static_cast<CHAOS_IL2CPP_UINT8*>(std::malloc(body.size()));
        if (out->body) {
            std::memcpy(const_cast<CHAOS_IL2CPP_UINT8*>(out->body),
                        body.data(), body.size());
            out->bodyLength = static_cast<CHAOS_IL2CPP_INT32>(body.size());
        } else {
            std::free(hdrsOut);
            out->headers = nullptr;
            out->headerCount = 0;
            conn->Close();
            delete conn;
            return NetError::Unsupported;
        }
    }
    if (!reason.empty()) {
        out->reason = static_cast<const char*>(std::malloc(reason.size() + 1));
        if (out->reason) {
            std::memcpy(const_cast<char*>(out->reason), reason.data(),
                        reason.size());
            const_cast<char*>(out->reason)[reason.size()] = '\0';
        }
    }
    out->bodyLength = static_cast<CHAOS_IL2CPP_INT32>(body.size());
    (void)body;

    // Pool or close the connection.
    if (keepAlive) {
        PoolPut(conn);
    } else {
        conn->Close();
        delete conn;
    }
    return NetError::None;
}

void HttpFreeResponse(HttpResponse* resp) noexcept {
    if (!resp) return;
    std::free(const_cast<HttpHeader*>(resp->headers));
    std::free(const_cast<void*>(static_cast<const void*>(resp->body)));
    std::free(const_cast<char*>(resp->reason));
    *resp = HttpResponse{};
}

void HttpPoolClear() noexcept {
    std::lock_guard<std::mutex> lock(g_poolMutex);
    for (int i = 0; i < kPoolSize; ++i) {
        if (g_pool[i]) {
            g_pool[i]->inPool = false;
            g_pool[i]->Close();
            delete g_pool[i];
            g_pool[i] = nullptr;
        }
    }
}

}  // namespace chaos::net
