// Layer I -- native HTTP/2 implementation (NT-16).  See http2.h for the
// surface and scope; layering mirrors http_impl.cpp:
//   * URL parse        -- inline ("h2://host[:port]/path", "h2c://...")
//   * DNS              -- DnsResolveEndPoint (NT-9)
//   * transport        -- NetSocket* (Layer D, NT-6/7)
//   * TLS (h2://)      -- TlsProvider with ALPN "h2" (NT-12)
//   * framing          -- RFC 9113 frames + connection preface
//   * headers          -- HPACK (RFC 7541): static/dynamic tables,
//     Huffman coding, all literal forms (NT-21, chaos/net/hpack.h).
//   * multiplexing     -- odd client stream ids, per-stream receive state,
//     WINDOW_UPDATE flow control on both directions.
//
// All public functions noexcept.  H2Response owns its storage exactly like
// HttpResponse (headers in one malloc block, body in another; reason is
// always nullptr -- HTTP/2 has no reason phrase).
#include <chaos/net/http2.h>
#include <chaos/net/tls.h>
#include <chaos/net/dns.h>
#include <chaos/net/hpack.h>

#include <cctype>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <map>
#include <string>
#include <vector>

namespace chaos::net {
namespace {

constexpr int kChunkRead       = 16384;
constexpr uint32_t kMaxFrame   = 16U * 1024U * 1024U;   // frame payload cap
constexpr int32_t kMaxWindow   = 0x7FFFFFFF;
constexpr int32_t kDefaultWindow = 65535;
constexpr CHAOS_IL2CPP_INT32 kMaxHeaderCount = 64;

// ── frame types / flags (RFC 9113 §6) ────────────────────────────────────
constexpr uint8_t kFrameData         = 0x0;
constexpr uint8_t kFrameHeaders      = 0x1;
constexpr uint8_t kFramePriority     = 0x2;
constexpr uint8_t kFrameRstStream    = 0x3;
constexpr uint8_t kFrameSettings     = 0x4;
constexpr uint8_t kFramePushPromise  = 0x5;
constexpr uint8_t kFramePing         = 0x6;
constexpr uint8_t kFrameGoAway       = 0x7;
constexpr uint8_t kFrameWindowUpdate = 0x8;
constexpr uint8_t kFrameContinuation = 0x9;
constexpr uint8_t kFlagEndStream     = 0x1;
constexpr uint8_t kFlagAck           = 0x1;
constexpr uint8_t kFlagEndHeaders    = 0x4;
constexpr uint8_t kFlagPadded        = 0x8;
constexpr uint8_t kFlagPriority      = 0x20;

const char kClientPreface[] = "PRI * HTTP/2.0\r\n\r\nSM\r\n\r\n";  // 24 bytes

// ── I/O (socket + optional TLS + read buffer), same shape as http_impl ───
struct H2Io {
    SocketHandle sock{};
    TlsProvider* tls = nullptr;
    std::vector<uint8_t> rbuf;
    size_t rpos = 0;
    int timeoutMs = 10000;

    H2Io() { sock.fd = -1; }
    ~H2Io() { Close(); }
    H2Io(const H2Io&) = delete;
    H2Io& operator=(const H2Io&) = delete;

    void Close() noexcept {
        if (tls) tls->Shutdown();
        if (sock.fd >= 0) NetSocketClose(&sock);
        delete tls;
        tls = nullptr;
        sock.fd = -1;
        rbuf.clear();
        rpos = 0;
    }

    NetError FillOnce(int tmo) noexcept {
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
                                       &got, tmo);
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

    NetError ReadBytes(uint8_t* dst, size_t n, int tmo) noexcept {
        size_t done = 0;
        while (done < n) {
            if (rpos >= rbuf.size()) {
                NetError e = FillOnce(tmo);
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

    NetError SendAll(const uint8_t* data, size_t n, int tmo) noexcept {
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
                                           0, &sent, tmo);
                if (e != NetError::None) return e;
                if (sent <= 0) return NetError::ConnectionReset;
                done += static_cast<size_t>(sent);
            }
        }
        return NetError::None;
    }
};

// ── frame helpers ────────────────────────────────────────────────────────
struct H2Frame {
    uint32_t length = 0;
    uint8_t type = 0;
    uint8_t flags = 0;
    int32_t streamId = 0;
    std::vector<uint8_t> payload;
};

NetError ReadFrame(H2Io* io, H2Frame* f, int tmo) noexcept {
    uint8_t h[9];
    NetError e = io->ReadBytes(h, 9, tmo);
    if (e != NetError::None) return e;
    f->length = (static_cast<uint32_t>(h[0]) << 16) |
                (static_cast<uint32_t>(h[1]) << 8) |
                static_cast<uint32_t>(h[2]);
    f->type = h[3];
    f->flags = h[4];
    f->streamId = (static_cast<int32_t>(h[5]) << 24) |
                  (static_cast<int32_t>(h[6]) << 16) |
                  (static_cast<int32_t>(h[7]) << 8) |
                  static_cast<int32_t>(h[8]);
    f->streamId &= 0x7FFFFFFF;  // reserved bit is ignored
    if (f->length > kMaxFrame) return NetError::Unsupported;
    f->payload.assign(f->length, 0);
    if (f->length > 0) {
        e = io->ReadBytes(f->payload.data(), f->length, tmo);
        if (e != NetError::None) return e;
    }
    return NetError::None;
}

NetError SendFrame(H2Io* io, uint8_t type, uint8_t flags, int32_t streamId,
                   const uint8_t* payload, size_t len, int tmo) noexcept {
    if (len > kMaxFrame) return NetError::Unsupported;
    uint8_t h[9];
    h[0] = static_cast<uint8_t>((len >> 16) & 0xFF);
    h[1] = static_cast<uint8_t>((len >> 8) & 0xFF);
    h[2] = static_cast<uint8_t>(len & 0xFF);
    h[3] = type;
    h[4] = flags;
    h[5] = static_cast<uint8_t>((streamId >> 24) & 0x7F);
    h[6] = static_cast<uint8_t>((streamId >> 16) & 0xFF);
    h[7] = static_cast<uint8_t>((streamId >> 8) & 0xFF);
    h[8] = static_cast<uint8_t>(streamId & 0xFF);
    NetError e = io->SendAll(h, 9, tmo);
    if (e != NetError::None) return e;
    if (len > 0) return io->SendAll(payload, len, tmo);
    return NetError::None;
}

// ── per-stream receive state ─────────────────────────────────────────────
struct H2Stream {
    bool active = false;       // stream opened by us (client) / peer (server)
    bool headersDone = false;
    bool endStream = false;    // END_STREAM received
    bool failed = false;
    NetError error = NetError::None;
    int32_t sendWindow = kDefaultWindow;   // peer-advertised send window
    std::vector<uint8_t> headerAccum;      // HEADERS + CONTINUATION bytes
    std::vector<std::pair<std::string, std::string>> headers;
    std::vector<uint8_t> body;
    CHAOS_IL2CPP_INT32 statusCode = 0;
    std::string method;   // server side: parsed (request) pseudo-headers
    std::string path;
};

NetError SendWindowUpdate(H2Io* io, int32_t streamId, uint32_t inc,
                          int tmo) noexcept {
    if (inc == 0 || inc > static_cast<uint32_t>(kMaxWindow))
        return NetError::Unsupported;
    uint8_t p[4] = {
        static_cast<uint8_t>((inc >> 24) & 0x7F),
        static_cast<uint8_t>((inc >> 16) & 0xFF),
        static_cast<uint8_t>((inc >> 8) & 0xFF),
        static_cast<uint8_t>(inc & 0xFF),
    };
    return SendFrame(io, kFrameWindowUpdate, 0, streamId, p, 4, tmo);
}

// ── connection-level demux / dispatch ────────────────────────────────────
// Applies one received frame.  streams/connSendWindow are the caller's
// connection state.  Called in both directions (client and server).
NetError DispatchFrame(H2Io* io,
                       std::map<int32_t, H2Stream>* streams,
                       int32_t* connSendWindow,
                       hpack::Encoder* enc, hpack::Decoder* dec,
                       const H2Frame& f, int tmo) noexcept {
    switch (f.type) {
        case kFrameSettings: {
            if (f.flags & kFlagAck) break;  // their ACK of our settings
            // SETTINGS_HEADER_TABLE_SIZE (0x1) caps OUR encoder dynamic
            // table (the peer decodes our blocks with that capacity).
            if (f.payload.size() % 6 == 0) {
                for (size_t q = 0; q < f.payload.size(); q += 6) {
                    uint16_t id = static_cast<uint16_t>(
                        (static_cast<uint16_t>(f.payload[q]) << 8) |
                        f.payload[q + 1]);
                    uint32_t val =
                        (static_cast<uint32_t>(f.payload[q + 2]) << 24) |
                        (static_cast<uint32_t>(f.payload[q + 3]) << 16) |
                        (static_cast<uint32_t>(f.payload[q + 4]) << 8) |
                        static_cast<uint32_t>(f.payload[q + 5]);
                    if (id == 0x1 && enc) enc->Table().SetCapacity(val);
                }
            }
            // ACK their SETTINGS (empty payload)
            return SendFrame(io, kFrameSettings, kFlagAck, 0, nullptr, 0, tmo);
        }
        case kFramePing: {
            if (f.flags & kFlagAck) break;
            if (f.payload.size() != 8) return NetError::Unsupported;
            return SendFrame(io, kFramePing, kFlagAck, 0, f.payload.data(), 8, tmo);
        }
        case kFrameGoAway: {
            // we stop initiating new streams; existing streams keep flowing
            break;
        }
        case kFrameRstStream: {
            auto it = streams->find(f.streamId);
            if (it != streams->end()) {
                it->second.failed = true;
                it->second.error = NetError::ConnectionReset;
                it->second.endStream = true;  // unwinds any waiter
            }
            break;
        }
        case kFrameWindowUpdate: {
            if (f.payload.size() != 4) return NetError::Unsupported;
            uint32_t inc = (static_cast<uint32_t>(f.payload[0]) << 24) |
                           (static_cast<uint32_t>(f.payload[1]) << 16) |
                           (static_cast<uint32_t>(f.payload[2]) << 8) |
                           static_cast<uint32_t>(f.payload[3]);
            inc &= 0x7FFFFFFF;
            if (inc == 0) return NetError::Unsupported;
            if (f.streamId == 0) {
                int64_t w = static_cast<int64_t>(*connSendWindow) + inc;
                *connSendWindow = w > kMaxWindow ? kMaxWindow
                                                 : static_cast<int32_t>(w);
            } else {
                auto it = streams->find(f.streamId);
                if (it != streams->end()) {
                    int64_t w = static_cast<int64_t>(it->second.sendWindow) + inc;
                    it->second.sendWindow = w > kMaxWindow ? kMaxWindow
                                                           : static_cast<int32_t>(w);
                }
            }
            break;
        }
        case kFrameHeaders:
        case kFrameContinuation: {
            if (f.streamId == 0) return NetError::Unsupported;
            auto it = streams->find(f.streamId);
            if (it == streams->end() && f.type == kFrameHeaders) {
                // first HEADERS for a peer-initiated stream (server side)
                it = streams->emplace(f.streamId, H2Stream{}).first;
                it->second.active = true;
            }
            if (it == streams->end()) break;  // unknown stream: ignore
            H2Stream& s = it->second;
            if (s.headersDone) return NetError::Unsupported;  // double headers
            size_t off = 0;
            if (f.type == kFrameHeaders && (f.flags & kFlagPadded)) {
                if (f.payload.empty()) return NetError::Unsupported;
                off = 1 + f.payload[0];
            }
            if (f.type == kFrameHeaders && (f.flags & kFlagPriority)) {
                off += 5;  // [4B sid][1B weight]
            }
            if (off > f.payload.size()) return NetError::Unsupported;
            s.headerAccum.insert(s.headerAccum.end(),
                                 f.payload.begin() + static_cast<ptrdiff_t>(off),
                                 f.payload.end());
            if (f.flags & kFlagEndHeaders) {
                bool ok = dec->DecodeHeaderBlock(s.headerAccum.data(),
                                                 s.headerAccum.size(),
                                                 &s.headers);
                s.headerAccum.clear();
                s.headersDone = true;
                if (!ok) {
                    s.failed = true;
                    s.error = NetError::Unsupported;
                    return NetError::Unsupported;
                }
                if (f.flags & kFlagEndStream) s.endStream = true;
            }
            break;
        }
        case kFrameData: {
            if (f.streamId == 0) return NetError::Unsupported;
            auto it = streams->find(f.streamId);
            if (it == streams->end()) break;  // ignore unknown stream
            H2Stream& s = it->second;
            size_t off = 0;
            if (f.flags & kFlagPadded) {
                if (f.payload.empty()) return NetError::Unsupported;
                off = 1 + f.payload[0];
            }
            if (off > f.payload.size()) return NetError::Unsupported;
            size_t dataLen = f.payload.size() - off;
            if (dataLen > 0) {
                s.body.insert(s.body.end(),
                              f.payload.begin() + static_cast<ptrdiff_t>(off),
                              f.payload.end());
                // restore this stream's + the connection's flow-control
                // windows immediately (loopback-friendly; the peer is always
                // consuming).
                NetError e = SendWindowUpdate(io, f.streamId,
                                              static_cast<uint32_t>(dataLen), tmo);
                if (e != NetError::None) return e;
                e = SendWindowUpdate(io, 0, static_cast<uint32_t>(dataLen), tmo);
                if (e != NetError::None) return e;
            }
            if (f.flags & kFlagEndStream) s.endStream = true;
            break;
        }
        default:
            break;  // PRIORITY, PUSH_PROMISE, unknown: benign ignore
    }
    return NetError::None;
}

// Flow-control-aware DATA sender: honors conn + stream windows, splits at
// 16384, waits (dispatching) for WINDOW_UPDATE when the window is exhausted.
NetError SendData(H2Io* io, std::map<int32_t, H2Stream>* streams,
                  int32_t* connSendWindow,
                  hpack::Encoder* enc, hpack::Decoder* dec,
                  int32_t streamId,
                  const uint8_t* data, size_t len, bool endStream,
                  int tmo) noexcept {
    size_t off = 0;
    bool lastSent = false;
    while (off < len || (endStream && !lastSent)) {
        auto it = streams->find(streamId);
        int32_t streamWin = (it != streams->end()) ? it->second.sendWindow
                                                   : kMaxWindow;
        if (*connSendWindow <= 0 || streamWin <= 0) {
            // wait for WINDOW_UPDATE from peer
            H2Frame f;
            NetError e = ReadFrame(io, &f, tmo);
            if (e != NetError::None) return e;
            e = DispatchFrame(io, streams, connSendWindow, enc, dec, f, tmo);
            if (e != NetError::None) return e;
            continue;
        }
        size_t chunk = len - off;
        if (chunk > static_cast<size_t>(kChunkRead))
            chunk = kChunkRead;
        if (chunk > static_cast<size_t>(*connSendWindow))
            chunk = static_cast<size_t>(*connSendWindow);
        if (chunk > static_cast<size_t>(streamWin))
            chunk = static_cast<size_t>(streamWin);
        if (chunk == 0) continue;
        lastSent = (off + chunk >= len);
        uint8_t flags = 0;
        if (endStream && lastSent) flags |= kFlagEndStream;
        NetError e = SendFrame(io, kFrameData, flags, streamId, data + off,
                               chunk, tmo);
        if (e != NetError::None) return e;
        *connSendWindow -= static_cast<int32_t>(chunk);
        if (it != streams->end())
            it->second.sendWindow -= static_cast<int32_t>(chunk);
        off += chunk;
    }
    return NetError::None;
}

// Sends one HEADERS frame (splitting into HEADERS + CONTINUATION when the
// block exceeds 16384).
NetError SendHeaders(H2Io* io, int32_t streamId,
                     const std::vector<uint8_t>& block, bool endStream,
                     int tmo) noexcept {
    size_t off = 0;
    bool first = true;
    const size_t limit = static_cast<size_t>(kChunkRead);
    while (off < block.size()) {
        size_t chunk = block.size() - off;
        if (chunk > limit) chunk = limit;
        bool last = (off + chunk >= block.size());
        if (first) {
            uint8_t flags = kFlagEndHeaders;
            if (endStream && last) flags |= kFlagEndStream;
            NetError e = SendFrame(io, kFrameHeaders, flags, streamId,
                                   block.data() + off, chunk, tmo);
            if (e != NetError::None) return e;
            off += chunk;
            first = false;
        } else {
            NetError e = SendFrame(io, kFrameContinuation,
                                   last ? kFlagEndHeaders : (uint8_t)0,
                                   streamId, block.data() + off, chunk, tmo);
            if (e != NetError::None) return e;
            off += chunk;
        }
    }
    if (first && endStream) {
        // empty HEADERS block (should not happen, but be safe)
        return SendFrame(io, kFrameHeaders, kFlagEndHeaders | kFlagEndStream,
                         streamId, nullptr, 0, tmo);
    }
    return NetError::None;
}

// ── URL parse (h2/h2c) ───────────────────────────────────────────────────
bool ParseH2Url(const char* url, bool* useTls, std::string* host,
                CHAOS_IL2CPP_UINT16* port, std::string* path) {
    if (!url || !*url) return false;
    const char* p = url;
    if (std::strncmp(p, "h2://", 5) == 0) {
        p += 5;
        *useTls = true;
    } else if (std::strncmp(p, "h2c://", 6) == 0) {
        p += 6;
        *useTls = false;
    } else {
        return false;
    }
    const char* start = p;
    while (*p && *p != ':' && *p != '/' && *p != '?') ++p;
    if (p == start) return false;
    *host = std::string(start, p);
    if (*p == ':') {
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
        *port = *useTls ? 443 : 80;
    }
    *path = (*p == '/' || *p == '?') ? std::string(p) : std::string("/");
    for (char& ch : *host)
        ch = static_cast<char>(std::tolower(static_cast<unsigned char>(ch)));
    return true;
}

const char* H2MethodName(HttpMethod m) {
    switch (m) {
        case HttpMethod::Get:    return "GET";
        case HttpMethod::Post:   return "POST";
        case HttpMethod::Put:    return "PUT";
        case HttpMethod::Delete: return "DELETE";
        case HttpMethod::Head:   return "HEAD";
    }
    return "GET";
}

HttpMethod ParseH2Method(const std::string& s) {
    if (s == "GET")    return HttpMethod::Get;
    if (s == "POST")   return HttpMethod::Post;
    if (s == "PUT")    return HttpMethod::Put;
    if (s == "DELETE") return HttpMethod::Delete;
    if (s == "HEAD")   return HttpMethod::Head;
    return HttpMethod::Get;
}

// Pseudo-headers ride in the leading position of the block; then the user
// headers.  Names are lowercased (RFC 9113 8.2: any uppercase -> malformed).
std::vector<uint8_t> EncodeRequestBlock(
    const std::string& method, bool useTls, const std::string& host,
    CHAOS_IL2CPP_UINT16 port, const std::string& path,
    const HttpHeader* headers, CHAOS_IL2CPP_INT32 headerCount,
    hpack::Encoder* enc) {
    std::vector<uint8_t> block;
    enc->EncodeField(&block, ":method", method);
    enc->EncodeField(&block, ":scheme", useTls ? "https" : "http");
    std::string authority = host + ":" + std::to_string(port);
    enc->EncodeField(&block, ":authority", authority);
    enc->EncodeField(&block, ":path", path);
    CHAOS_IL2CPP_INT32 n = headerCount < kMaxHeaderCount
                               ? headerCount : kMaxHeaderCount;
    for (CHAOS_IL2CPP_INT32 i = 0; i < n; ++i) {
        std::string name = headers[i].name ? headers[i].name : "";
        for (char& ch : name)
            ch = static_cast<char>(std::tolower(static_cast<unsigned char>(ch)));
        enc->EncodeField(&block, name, headers[i].value ? headers[i].value : "");
    }
    return block;
}

std::vector<uint8_t> EncodeResponseBlock(
    CHAOS_IL2CPP_INT32 statusCode, const HttpHeader* headers,
    CHAOS_IL2CPP_INT32 headerCount, hpack::Encoder* enc) {
    std::vector<uint8_t> block;
    enc->EncodeField(&block, ":status", std::to_string(statusCode));
    CHAOS_IL2CPP_INT32 n = headerCount < kMaxHeaderCount
                               ? headerCount : kMaxHeaderCount;
    for (CHAOS_IL2CPP_INT32 i = 0; i < n; ++i) {
        std::string name = headers[i].name ? headers[i].name : "";
        for (char& ch : name)
            ch = static_cast<char>(std::tolower(static_cast<unsigned char>(ch)));
        enc->EncodeField(&block, name, headers[i].value ? headers[i].value : "");
    }
    return block;
}

// ── opaque struct bodies ─────────────────────────────────────────────────
struct H2ClientState {
    H2Io io;
    int32_t nextStreamId = 1;         // odd, client-initiated
    int32_t connSendWindow = kDefaultWindow;
    std::map<int32_t, H2Stream> streams;
    hpack::Encoder enc;                // outbound HPACK (request side)
    hpack::Decoder dec;                // inbound HPACK
    bool gotGoAway = false;
};

struct H2ServerState {
    H2Io io;
    int32_t connSendWindow = kDefaultWindow;
    std::map<int32_t, H2Stream> streams;
    hpack::Encoder enc;                // outbound HPACK (response side)
    hpack::Decoder dec;                // inbound HPACK
    // arena for the *current* H2ServerRequest headers (valid until the next
    // H2ServerNextRequest call)
    std::vector<HttpHeader> outHdrs;
    std::vector<char> outBlob;
};

int EffTmo(int tmo) { return tmo > 0 ? tmo : 10000; }

NetError OpenSocket(const std::string& host, CHAOS_IL2CPP_UINT16 port,
                    SocketHandle* sock, int tmo) noexcept {
    NetError e = NetSocketCreate(kAddressFamilyInet, kSocketTypeStream,
                                 kProtocolTcp, sock);
    if (e != NetError::None) return e;
    NetSocketSetOption(sock, NetOption::NoDelay, 1);
    NetAddress addr{};
    e = DnsResolveEndPoint(host.c_str(), port, &addr);
    if (e != NetError::None) {
        NetSocketClose(sock);
        return e;
    }
    e = NetSocketConnect(sock, addr, tmo);
    if (e != NetError::None) NetSocketClose(sock);
    return e;
}

// ALPN wire for the client: advertise exactly "h2".
const CHAOS_IL2CPP_UINT8 kClientAlpnH2[] = {0x00, 0x03, 0x02, 'h', '2'};

}  // namespace

// ── client API ───────────────────────────────────────────────────────────
NetError H2Connect(const H2Request& req, H2Client** out) noexcept {
    if (!out) return NetError::Unsupported;
    *out = nullptr;
    if (!req.url) return NetError::Unsupported;

    bool useTls = false;
    std::string host, path;
    CHAOS_IL2CPP_UINT16 port = 0;
    if (!ParseH2Url(req.url, &useTls, &host, &port, &path))
        return NetError::Unsupported;

    H2ClientState* c = new (std::nothrow) H2ClientState();
    if (!c) return NetError::Unsupported;
    const int tmo = EffTmo(req.timeoutMs);

    NetError e = OpenSocket(host, port, &c->io.sock, tmo);
    if (e != NetError::None) { delete c; return e; }

    if (useTls) {
#ifdef _WIN32
        c->io.tls = CreateSchannelProvider();
#else
        c->io.tls = CreateOpenSslProvider();
#endif
        if (!c->io.tls) { delete c; return NetError::Unsupported; }
        TlsOptions to;
        to.mode = TlsMode::Client;
        to.serverName = host.c_str();
        to.alpnProtocols = kClientAlpnH2;
        to.alpnLength = static_cast<CHAOS_IL2CPP_UINT32>(sizeof(kClientAlpnH2));
        to.disableCertificateValidation = req.disableCertificateValidation;
        to.timeoutMs = tmo;
        e = c->io.tls->Create(to);
        if (e == NetError::None) e = c->io.tls->Handshake(&c->io.sock);
        if (e != NetError::None) { delete c; return e; }
    }

    c->io.timeoutMs = tmo;

    // connection preface + our SETTINGS (empty = RFC defaults)
    e = c->io.SendAll(reinterpret_cast<const uint8_t*>(kClientPreface), 24, tmo);
    if (e == NetError::None)
        e = SendFrame(&c->io, kFrameSettings, 0, 0, nullptr, 0, tmo);
    if (e != NetError::None) { delete c; return e; }

    // settings exchange: wait for the server's SETTINGS (non-ACK), ACKing
    // anything else that needs it on the way.
    for (;;) {
        H2Frame f;
        e = ReadFrame(&c->io, &f, tmo);
        if (e != NetError::None) { delete c; return e; }
        bool serverSettings = (f.type == kFrameSettings && !(f.flags & kFlagAck));
        e = DispatchFrame(&c->io, &c->streams, &c->connSendWindow,
                           &c->enc, &c->dec, f, tmo);
        if (e != NetError::None) { delete c; return e; }
        if (serverSettings) break;
    }

    *out = reinterpret_cast<H2Client*>(c);
    return NetError::None;
}

NetError H2BeginRequest(H2Client* hc, const H2Request& req,
                        CHAOS_IL2CPP_INT32* streamId) noexcept {
    if (!hc || !streamId) return NetError::Unsupported;
    H2ClientState* c = reinterpret_cast<H2ClientState*>(hc);
    if (c->gotGoAway) return NetError::ConnectionReset;
    if (c->nextStreamId <= 0 || c->nextStreamId >= kMaxWindow)
        return NetError::Unsupported;

    bool useTls = false;
    std::string host, path;
    CHAOS_IL2CPP_UINT16 port = 0;
    if (!ParseH2Url(req.url, &useTls, &host, &port, &path))
        return NetError::Unsupported;

    const int sid = c->nextStreamId;
    c->nextStreamId += 2;

    std::vector<uint8_t> block = EncodeRequestBlock(
        H2MethodName(req.method), useTls, host, port, path,
        req.headers, req.headerCount, &c->enc);

    H2Stream& s = c->streams[sid];
    s = H2Stream{};
    s.active = true;

    const int tmo = EffTmo(req.timeoutMs);
    NetError e = SendHeaders(&c->io, sid, block, req.bodyLength == 0, tmo);
    if (e == NetError::None && req.bodyLength > 0) {
        e = SendData(&c->io, &c->streams, &c->connSendWindow,
                     &c->enc, &c->dec, sid,
                     req.body ? req.body : reinterpret_cast<const uint8_t*>(""),
                     static_cast<size_t>(req.bodyLength), true, tmo);
    }
    if (e != NetError::None) {
        c->streams.erase(sid);
        return e;
    }
    *streamId = sid;
    return NetError::None;
}

NetError H2RecvResponse(H2Client* hc, CHAOS_IL2CPP_INT32 streamId,
                        H2Response* out, CHAOS_IL2CPP_INT32 timeoutMs) noexcept {
    if (!hc || !out) return NetError::Unsupported;
    H2ClientState* c = reinterpret_cast<H2ClientState*>(hc);
    *out = H2Response{};
    auto it = c->streams.find(streamId);
    if (it == c->streams.end() || !it->second.active)
        return NetError::Unsupported;
    const int tmo = EffTmo(timeoutMs);

    for (;;) {
        H2Stream& s = it->second;
        if (s.failed) { NetError err = s.error; c->streams.erase(it); return err; }
        if (s.headersDone && s.endStream) break;
        H2Frame f;
        NetError e = ReadFrame(&c->io, &f, tmo);
        if (e != NetError::None) { c->streams.erase(it); return e; }
        if (f.type == kFrameGoAway) c->gotGoAway = true;
        e = DispatchFrame(&c->io, &c->streams, &c->connSendWindow,
                          &c->enc, &c->dec, f, tmo);
        if (e != NetError::None) { c->streams.erase(it); return e; }
    }

    H2Stream& s = it->second;
    CHAOS_IL2CPP_INT32 status = 0;
    for (const auto& h : s.headers) {
        if (h.first == ":status") {
            status = static_cast<CHAOS_IL2CPP_INT32>(
                std::strtol(h.second.c_str(), nullptr, 10));
            break;
        }
    }
    out->streamId = streamId;
    out->statusCode = status;
    out->reason = nullptr;

    size_t hdrCount = 0;
    for (const auto& h : s.headers)
        if (h.first.rfind(':', 0) != 0) ++hdrCount;  // skip pseudo-headers
    if (hdrCount > 0) {
        size_t blobBytes = hdrCount * sizeof(HttpHeader);
        for (const auto& h : s.headers) {
            if (h.first.rfind(':', 0) == 0) continue;
            blobBytes += h.first.size() + 1 + h.second.size() + 1;
        }
        char* mem = static_cast<char*>(std::malloc(blobBytes));
        if (!mem) { c->streams.erase(it); return NetError::Unsupported; }
        char* blobp = mem + hdrCount * sizeof(HttpHeader);
        HttpHeader* arr = reinterpret_cast<HttpHeader*>(mem);
        size_t idx = 0;
        for (const auto& h : s.headers) {
            if (h.first.rfind(':', 0) == 0) continue;
            std::memcpy(blobp, h.first.c_str(), h.first.size() + 1);
            arr[idx].name = blobp;
            blobp += h.first.size() + 1;
            std::memcpy(blobp, h.second.c_str(), h.second.size() + 1);
            arr[idx].value = blobp;
            blobp += h.second.size() + 1;
            ++idx;
        }
        out->headers = arr;
        out->headerCount = static_cast<CHAOS_IL2CPP_INT32>(hdrCount);
    }
    if (!s.body.empty()) {
        uint8_t* bp = static_cast<uint8_t*>(std::malloc(s.body.size()));
        if (!bp) {
            std::free(const_cast<HttpHeader*>(out->headers));
            out->headers = nullptr;
            c->streams.erase(it);
            return NetError::Unsupported;
        }
        std::memcpy(bp, s.body.data(), s.body.size());
        out->body = bp;
        out->bodyLength = static_cast<CHAOS_IL2CPP_INT32>(s.body.size());
    }

    c->streams.erase(it);
    return NetError::None;
}

void H2FreeResponse(H2Response* resp) noexcept {
    if (!resp) return;
    std::free(const_cast<HttpHeader*>(resp->headers));
    std::free(const_cast<CHAOS_IL2CPP_UINT8*>(resp->body));
    *resp = H2Response{};
}

void H2Free(H2Client* hc) noexcept {
    if (!hc) return;
    H2ClientState* c = reinterpret_cast<H2ClientState*>(hc);
    // best-effort GOAWAY
    uint8_t p[8] = {0, 0, 0, 0, 0, 0, 0, 0};  // last-stream-id 0, no error
    SendFrame(&c->io, kFrameGoAway, 0, 0, p, 8, 1000);
    delete c;
}

// ── server API ───────────────────────────────────────────────────────────
NetError H2ServerAccept(SocketHandle* listen, const TlsCertificate* tlsCert,
                        H2Server** out) noexcept {
    if (!out) return NetError::Unsupported;
    *out = nullptr;
    H2ServerState* s = new (std::nothrow) H2ServerState();
    if (!s) return NetError::Unsupported;
    const int tmo = EffTmo(0);

    NetError e = NetSocketAccept(listen, &s->io.sock, nullptr, 10000);
    if (e != NetError::None) { delete s; return e; }

    if (tlsCert) {
#ifdef _WIN32
        s->io.tls = CreateSchannelProvider();
#else
        s->io.tls = CreateOpenSslProvider();
#endif
        if (!s->io.tls) { NetSocketClose(&s->io.sock); delete s; return NetError::Unsupported; }
        TlsOptions to;
        to.mode = TlsMode::Server;
        to.serverCertificate = *tlsCert;
        to.timeoutMs = tmo;
        static const CHAOS_IL2CPP_UINT8 kServerAlpn[] = {
            0x00, 0x0C,
            0x02, 'h', '2',
            0x08, 'h', 't', 't', 'p', '/', '1', '.', '1',
        };
        to.alpnProtocols = kServerAlpn;
        to.alpnLength = static_cast<CHAOS_IL2CPP_UINT32>(sizeof(kServerAlpn));
        e = s->io.tls->Create(to);
        if (e == NetError::None) e = s->io.tls->Handshake(&s->io.sock);
        if (e != NetError::None) { delete s; return e; }
    }
    s->io.timeoutMs = tmo;

    // read the client connection preface
    uint8_t pref[24];
    e = s->io.ReadBytes(pref, 24, tmo);
    if (e != NetError::None) { delete s; return e; }
    if (std::memcmp(pref, kClientPreface, 24) != 0) { delete s; return NetError::Unsupported; }

    // server SETTINGS, then ACK the client's SETTINGS when it arrives
    e = SendFrame(&s->io, kFrameSettings, 0, 0, nullptr, 0, tmo);
    if (e != NetError::None) { delete s; return e; }
    for (;;) {
        H2Frame f;
        e = ReadFrame(&s->io, &f, tmo);
        if (e != NetError::None) { delete s; return e; }
        bool clientSettings = (f.type == kFrameSettings && !(f.flags & kFlagAck));
        e = DispatchFrame(&s->io, &s->streams, &s->connSendWindow,
                          &s->enc, &s->dec, f, tmo);
        if (e != NetError::None) { delete s; return e; }
        if (clientSettings) break;
    }

    *out = reinterpret_cast<H2Server*>(s);
    return NetError::None;
}

NetError H2ServerNextRequest(H2Server* hs, H2ServerRequest* out,
                             CHAOS_IL2CPP_INT32 timeoutMs) noexcept {
    if (!hs || !out) return NetError::Unsupported;
    H2ServerState* s = reinterpret_cast<H2ServerState*>(hs);
    *out = H2ServerRequest{};
    s->outHdrs.clear();
    s->outBlob.clear();
    const int tmo = EffTmo(timeoutMs);

    // a completed request may already be buffered (arrived while handling a
    // previous request or via a multiplexed earlier stream)
    for (;;) {
        auto it = s->streams.begin();
        while (it != s->streams.end() &&
               (!it->second.active || !it->second.headersDone ||
                !it->second.endStream))
            ++it;
        if (it != s->streams.end()) {
            H2Stream& st = it->second;
            out->streamId = it->first;
            for (const auto& h : st.headers) {
                if (h.first == ":method") out->method = ParseH2Method(h.second);
                else if (h.first == ":path") out->path = h.second.c_str();
            }
            size_t hdrCount = 0;
            for (const auto& h : st.headers)
                if (h.first.rfind(':', 0) != 0) ++hdrCount;
            if (hdrCount > 0) {
                size_t blobBytes = 0;
                for (const auto& h : st.headers)
                    if (h.first.rfind(':', 0) != 0)
                        blobBytes += h.first.size() + 1 + h.second.size() + 1;
                s->outHdrs.resize(hdrCount);
                s->outBlob.resize(blobBytes);
                char* blobp = s->outBlob.data();
                size_t idx = 0;
                for (const auto& h : st.headers) {
                    if (h.first.rfind(':', 0) == 0) continue;
                    std::memcpy(blobp, h.first.c_str(), h.first.size() + 1);
                    s->outHdrs[idx].name = blobp;
                    blobp += h.first.size() + 1;
                    std::memcpy(blobp, h.second.c_str(), h.second.size() + 1);
                    s->outHdrs[idx].value = blobp;
                    blobp += h.second.size() + 1;
                    ++idx;
                }
                out->headers = s->outHdrs.data();
                out->headerCount = static_cast<CHAOS_IL2CPP_INT32>(hdrCount);
            }
            out->body = st.body.empty() ? nullptr : st.body.data();
            out->bodyLength = static_cast<CHAOS_IL2CPP_INT32>(st.body.size());
            return NetError::None;
        }
        H2Frame f;
        NetError e = ReadFrame(&s->io, &f, tmo);
        if (e != NetError::None) return e;
        e = DispatchFrame(&s->io, &s->streams, &s->connSendWindow,
                          &s->enc, &s->dec, f, tmo);
        if (e != NetError::None) return e;
    }
}

NetError H2ServerRespond(H2Server* hs, CHAOS_IL2CPP_INT32 streamId,
                         CHAOS_IL2CPP_INT32 statusCode,
                         const HttpHeader* headers,
                         CHAOS_IL2CPP_INT32 headerCount,
                         const CHAOS_IL2CPP_UINT8* body,
                         CHAOS_IL2CPP_INT32 bodyLength,
                         CHAOS_IL2CPP_INT32 timeoutMs) noexcept {
    if (!hs) return NetError::Unsupported;
    H2ServerState* s = reinterpret_cast<H2ServerState*>(hs);
    auto it = s->streams.find(streamId);
    if (it == s->streams.end()) return NetError::Unsupported;
    const int tmo = EffTmo(timeoutMs);

    std::vector<uint8_t> block = EncodeResponseBlock(statusCode, headers,
                                                     headerCount, &s->enc);
    NetError e = SendHeaders(&s->io, streamId, block, bodyLength == 0, tmo);
    if (e == NetError::None && bodyLength > 0) {
        e = SendData(&s->io, &s->streams, &s->connSendWindow,
                     &s->enc, &s->dec, streamId,
                     body ? body : reinterpret_cast<const uint8_t*>(""),
                     static_cast<size_t>(bodyLength), true, tmo);
    }
    if (e == NetError::None) s->streams.erase(it);
    return e;
}

void H2ServerFree(H2Server* hs) noexcept {
    if (!hs) return;
    H2ServerState* s = reinterpret_cast<H2ServerState*>(hs);
    delete s;
}

}  // namespace chaos::net