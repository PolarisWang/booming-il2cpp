// Layer H -- WebSocket (RFC 6455) client implementation (NT-14).
// See ws.h for the surface. Uses Layer D transport + NT-9 DNS + NT-12 TLS.
//
// * Handshake: RFC 6455 §§4.1/4.2 GET upgrade with Sec-WebSocket-Key,
//   validates Sec-WebSocket-Accept = base64(SHA-1(key + GUID)).
// * Frames:    §5.2 layout; client frames always masked (§5.3); extended
//   lengths 126/127; Ping auto-Pong; Close echo in WsClose.
#include <chaos/net/ws.h>
#include <chaos/net/dns.h>
#include <chaos/net/tls.h>

#include <algorithm>
#include <chrono>
#include <cstring>
#include <string>
#include <vector>

namespace chaos::net {
namespace {

constexpr char kGuid[] = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";

// ---- tiny SHA-1 (RFC 3174) -------------------------------------------
void Sha1(const CHAOS_IL2CPP_UINT8* data, std::size_t len, CHAOS_IL2CPP_UINT8 out[20]) {
    CHAOS_IL2CPP_UINT32 h[5] = {0x67452301u, 0xEFCDAB89u, 0x98BADCFEu,
                                0x10325476u, 0xC3D2E1F0u};
    std::size_t padded = ((len + 9 + 63) / 64) * 64;
    std::vector<CHAOS_IL2CPP_UINT8> m(padded, 0);
    std::memcpy(m.data(), data, len);
    m[len] = 0x80;
    CHAOS_IL2CPP_UINT64 bits = static_cast<CHAOS_IL2CPP_UINT64>(len) * 8;
    for (int i = 0; i < 8; ++i)
        m[padded - 1 - i] = static_cast<CHAOS_IL2CPP_UINT8>(bits >> (8 * i));
    auto rol = [](CHAOS_IL2CPP_UINT32 v, int n) {
        return (v << n) | (v >> (32 - n));
    };
    for (std::size_t off = 0; off < padded; off += 64) {
        CHAOS_IL2CPP_UINT32 w[80];
        for (int i = 0; i < 16; ++i)
            w[i] = (static_cast<CHAOS_IL2CPP_UINT32>(m[off + 4 * i]) << 24) |
                   (static_cast<CHAOS_IL2CPP_UINT32>(m[off + 4 * i + 1]) << 16) |
                   (static_cast<CHAOS_IL2CPP_UINT32>(m[off + 4 * i + 2]) << 8) |
                   static_cast<CHAOS_IL2CPP_UINT32>(m[off + 4 * i + 3]);
        for (int i = 16; i < 80; ++i)
            w[i] = rol(w[i - 3] ^ w[i - 8] ^ w[i - 14] ^ w[i - 16], 1);
        CHAOS_IL2CPP_UINT32 a = h[0], b = h[1], c = h[2], d = h[3], e = h[4];
        for (int i = 0; i < 80; ++i) {
            CHAOS_IL2CPP_UINT32 f = 0, k = 0;
            if (i < 20) {
                f = (b & c) | ((~b) & d);
                k = 0x5A827999u;
            } else if (i < 40) {
                f = b ^ c ^ d;
                k = 0x6ED9EBA1u;
            } else if (i < 60) {
                f = (b & c) | (b & d) | (c & d);
                k = 0x8F1BBCDCu;
            } else {
                f = b ^ c ^ d;
                k = 0xCA62C1D6u;
            }
            CHAOS_IL2CPP_UINT32 t = rol(a, 5) + f + e + k + w[i];
            e = d;
            d = c;
            c = rol(b, 30);
            b = a;
            a = t;
        }
        h[0] += a; h[1] += b; h[2] += c; h[3] += d; h[4] += e;
    }
    for (int i = 0; i < 5; ++i) {
        out[4 * i] = static_cast<CHAOS_IL2CPP_UINT8>((h[i] >> 24) & 0xFF);
        out[4 * i + 1] = static_cast<CHAOS_IL2CPP_UINT8>((h[i] >> 16) & 0xFF);
        out[4 * i + 2] = static_cast<CHAOS_IL2CPP_UINT8>((h[i] >> 8) & 0xFF);
        out[4 * i + 3] = static_cast<CHAOS_IL2CPP_UINT8>(h[i] & 0xFF);
    }
}

std::string Base64Encode(const CHAOS_IL2CPP_UINT8* data, std::size_t len) {
    static const char tbl[] =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    std::string out;
    out.reserve(((len + 2) / 3) * 4);
    std::size_t i = 0;
    while (i + 3 <= len) {
        CHAOS_IL2CPP_UINT32 v = (static_cast<CHAOS_IL2CPP_UINT32>(data[i]) << 16) |
                                (static_cast<CHAOS_IL2CPP_UINT32>(data[i + 1]) << 8) |
                                static_cast<CHAOS_IL2CPP_UINT32>(data[i + 2]);
        out.push_back(tbl[(v >> 18) & 63]);
        out.push_back(tbl[(v >> 12) & 63]);
        out.push_back(tbl[(v >> 6) & 63]);
        out.push_back(tbl[v & 63]);
        i += 3;
    }
    if (i < len) {
        CHAOS_IL2CPP_UINT32 v =
            static_cast<CHAOS_IL2CPP_UINT32>(data[i]) << 16;
        std::size_t rem = len - i;
        if (rem == 2) v |= static_cast<CHAOS_IL2CPP_UINT32>(data[i + 1]) << 8;
        out.push_back(tbl[(v >> 18) & 63]);
        out.push_back(tbl[(v >> 12) & 63]);
        out.push_back(rem == 2 ? tbl[(v >> 6) & 63] : '=');
        out.push_back('=');
    }
    return out;
}

// ---- tiny xorshift RNG for mask keys / handshake nonce ------------------
CHAOS_IL2CPP_UINT32 g_rngState = [] {
    CHAOS_IL2CPP_UINT64 t = static_cast<CHAOS_IL2CPP_UINT64>(
        std::chrono::high_resolution_clock::now().time_since_epoch().count());
    CHAOS_IL2CPP_UINT32 s = static_cast<CHAOS_IL2CPP_UINT32>(t ^ (t >> 32));
    return s ? s : 0x9E3779B9u;
}();
CHAOS_IL2CPP_UINT32 NextRand() {
    g_rngState ^= g_rngState << 13;
    g_rngState ^= g_rngState >> 17;
    g_rngState ^= g_rngState << 5;
    return g_rngState;
}
void MaskBytes(CHAOS_IL2CPP_UINT8 m[4]) {
    CHAOS_IL2CPP_UINT32 v = NextRand();
    m[0] = static_cast<CHAOS_IL2CPP_UINT8>(v & 0xFF);
    m[1] = static_cast<CHAOS_IL2CPP_UINT8>((v >> 8) & 0xFF);
    m[2] = static_cast<CHAOS_IL2CPP_UINT8>((v >> 16) & 0xFF);
    m[3] = static_cast<CHAOS_IL2CPP_UINT8>((v >> 24) & 0xFF);
}

// ---- URL parsing --------------------------------------------------------
bool ParseWsUrl(const char* url, bool* wss, std::string* host,
                CHAOS_IL2CPP_UINT16* port, std::string* path) {
    std::string s = url;
    *wss = false;
    if (s.rfind("ws://", 0) == 0) {
        s = s.substr(5);
    } else if (s.rfind("wss://", 0) == 0) {
        *wss = true;
        s = s.substr(6);
    } else {
        return false;
    }
    std::size_t slash = s.find('/');
    std::string authority =
        slash == std::string::npos ? s : s.substr(0, slash);
    *path = slash == std::string::npos ? "/" : s.substr(slash);
    std::size_t colon = authority.rfind(':');
    if (colon == std::string::npos) {
        *host = authority;
        *port = *wss ? 443 : 80;
    } else {
        *host = authority.substr(0, colon);
        long p = std::strtol(authority.c_str() + colon + 1, nullptr, 10);
        if (p <= 0 || p > 65535) return false;
        *port = static_cast<CHAOS_IL2CPP_UINT16>(p);
    }
    if (host->empty()) return false;
    for (auto& ch : *host) ch = static_cast<char>(std::tolower(static_cast<unsigned char>(ch)));
    return true;
}

// ---- Buffered stream over transport (+ optional TLS) --------------------
struct WsConn {
    SocketHandle sock{};
    TlsProvider* tls = nullptr;
    std::vector<CHAOS_IL2CPP_UINT8> buf;
    std::size_t pos = 0;
    std::vector<CHAOS_IL2CPP_UINT8> frameBuf;   // last received frame
    std::string closeReason;                    // Close frame reason (owned)
    bool dead = false;
};

constexpr std::size_t kReadChunk = 16384;

NetError Fill(WsConn* c, CHAOS_IL2CPP_INT32 timeoutMs) {
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

// Reads exactly n bytes into out (buffered).
NetError ReadBytes(WsConn* c, CHAOS_IL2CPP_UINT8* out, std::size_t n,
                   CHAOS_IL2CPP_INT32 timeoutMs) {
    std::size_t done = 0;
    while (done < n) {
        NetError err = Fill(c, timeoutMs);
        if (err != NetError::None) return err;
        std::size_t avail = c->buf.size() - c->pos;
        std::size_t take = std::min(avail, n - done);
        std::memcpy(out + done, c->buf.data() + c->pos, take);
        c->pos += take;
        done += take;
    }
    return NetError::None;
}

NetError SendAll(WsConn* c, const CHAOS_IL2CPP_UINT8* data, std::size_t n,
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

// ---- frame encode/decode -------------------------------------------------
NetError SendFrame(WsConn* c, WsOpcode opcode, bool fin,
                   const CHAOS_IL2CPP_UINT8* payload, std::size_t len,
                   CHAOS_IL2CPP_INT32 timeoutMs) {
    CHAOS_IL2CPP_UINT8 hdr[14];
    std::size_t n = 2;
    hdr[0] = static_cast<CHAOS_IL2CPP_UINT8>((fin ? 0x80 : 0x00) |
                                             static_cast<CHAOS_IL2CPP_UINT8>(opcode));
    if (len < 126) {
        hdr[1] = static_cast<CHAOS_IL2CPP_UINT8>(0x80 | len);  // MASK set
    } else if (len <= 0xFFFFu) {
        hdr[1] = static_cast<CHAOS_IL2CPP_UINT8>(0x80 | 126);
        hdr[2] = static_cast<CHAOS_IL2CPP_UINT8>((len >> 8) & 0xFF);
        hdr[3] = static_cast<CHAOS_IL2CPP_UINT8>(len & 0xFF);
        n = 4;
    } else {
        hdr[1] = static_cast<CHAOS_IL2CPP_UINT8>(0x80 | 127);
        CHAOS_IL2CPP_UINT64 L = static_cast<CHAOS_IL2CPP_UINT64>(len);
        n = 2;
        for (int i = 7; i >= 0; --i)
            hdr[n++] = static_cast<CHAOS_IL2CPP_UINT8>((L >> (8 * i)) & 0xFF);
    }
    CHAOS_IL2CPP_UINT8 mask[4];
    MaskBytes(mask);
    std::memcpy(hdr + n, mask, 4);
    n += 4;
    std::vector<CHAOS_IL2CPP_UINT8> wire(n + len);
    std::memcpy(wire.data(), hdr, n);
    for (std::size_t i = 0; i < len; ++i)
        wire[n + i] = payload[i] ^ mask[i & 3];
    return SendAll(c, wire.data(), wire.size(), timeoutMs);
}

NetError RecvFrame(WsConn* c, WsFrame* out, CHAOS_IL2CPP_INT32 timeoutMs) {
    for (;;) {
        CHAOS_IL2CPP_UINT8 b0 = 0, b1 = 0;
        NetError err = ReadBytes(c, &b0, 1, timeoutMs);
        if (err != NetError::None) return err;
        err = ReadBytes(c, &b1, 1, timeoutMs);
        if (err != NetError::None) return err;
        bool fin = (b0 & 0x80) != 0;
        CHAOS_IL2CPP_UINT32 opcode = b0 & 0x0F;
        bool masked = (b1 & 0x80) != 0;
        CHAOS_IL2CPP_UINT64 len = b1 & 0x7F;
        if (len == 126) {
            CHAOS_IL2CPP_UINT8 ext[2];
            err = ReadBytes(c, ext, 2, timeoutMs);
            if (err != NetError::None) return err;
            len = (static_cast<CHAOS_IL2CPP_UINT64>(ext[0]) << 8) | ext[1];
        } else if (len == 127) {
            CHAOS_IL2CPP_UINT8 ext[8];
            err = ReadBytes(c, ext, 8, timeoutMs);
            if (err != NetError::None) return err;
            len = 0;
            for (int i = 0; i < 8; ++i) len = (len << 8) | ext[i];
        }
        if (len > static_cast<CHAOS_IL2CPP_UINT64>(kWsMaxFrameBytes))
            return NetError::Unsupported;
        CHAOS_IL2CPP_UINT8 mask[4] = {0, 0, 0, 0};
        if (masked) {
            err = ReadBytes(c, mask, 4, timeoutMs);
            if (err != NetError::None) return err;
        }
        c->frameBuf.clear();
        c->frameBuf.reserve(static_cast<std::size_t>(len));
        if (len > 0) {
            c->frameBuf.resize(static_cast<std::size_t>(len));
            err = ReadBytes(c, c->frameBuf.data(), static_cast<std::size_t>(len),
                            timeoutMs);
            if (err != NetError::None) return err;
            if (masked) {
                for (std::size_t i = 0; i < static_cast<std::size_t>(len); ++i)
                    c->frameBuf[i] ^= mask[i & 3];
            }
        }
        if (opcode == static_cast<CHAOS_IL2CPP_UINT32>(WsOpcode::Ping)) {
            // §5.5.3: answer with a Pong carrying the same payload, never surface.
            err = SendFrame(c, WsOpcode::Pong, true, c->frameBuf.data(),
                            c->frameBuf.size(), timeoutMs);
            if (err != NetError::None) return err;
            continue;
        }
        out->opcode = static_cast<WsOpcode>(opcode);
        out->fin = fin;
        out->payload = c->frameBuf.empty() ? nullptr : c->frameBuf.data();
        out->payloadLength = static_cast<CHAOS_IL2CPP_INT32>(c->frameBuf.size());
        out->closeCode = 0;
        out->closeReason = nullptr;
        if (opcode == static_cast<CHAOS_IL2CPP_UINT32>(WsOpcode::Close)) {
            c->closeReason.clear();
            if (c->frameBuf.size() >= 2) {
                out->closeCode = static_cast<CHAOS_IL2CPP_UINT16>(
                    (c->frameBuf[0] << 8) | c->frameBuf[1]);
                if (c->frameBuf.size() > 2) {
                    c->closeReason.assign(
                        reinterpret_cast<const char*>(c->frameBuf.data() + 2),
                        c->frameBuf.size() - 2);
                    out->closeReason = c->closeReason.c_str();
                } else {
                    out->closeReason = "";
                }
            } else {
                out->closeReason = "";
            }
        }
        return NetError::None;
    }
}

// forward decl so Handshake (defined before ReadLine's definition) can use it
bool ReadLine(WsConn* c, std::string* out, CHAOS_IL2CPP_INT32 timeoutMs);

// ---- opening handshake ---------------------------------------------------
NetError Handshake(WsConn* c, const std::string& host, CHAOS_IL2CPP_UINT16 port,
                   const std::string& path, const WsOptions& opts,
                   CHAOS_IL2CPP_INT32 timeoutMs) {
    // Sec-WebSocket-Key: base64 of 16 random bytes (§4.1: 16-byte nonce).
    CHAOS_IL2CPP_UINT8 nonce[16];
    for (int i = 0; i < 16; i += 4) {
        CHAOS_IL2CPP_UINT32 v = NextRand();
        nonce[i] = static_cast<CHAOS_IL2CPP_UINT8>(v & 0xFF);
        nonce[i + 1] = static_cast<CHAOS_IL2CPP_UINT8>((v >> 8) & 0xFF);
        nonce[i + 2] = static_cast<CHAOS_IL2CPP_UINT8>((v >> 16) & 0xFF);
        nonce[i + 3] = static_cast<CHAOS_IL2CPP_UINT8>((v >> 24) & 0xFF);
    }
    std::string key = Base64Encode(nonce, sizeof(nonce));

    std::string req = "GET " + path + " HTTP/1.1\r\n";
    req += "Host: " + host + ":" + std::to_string(port) + "\r\n";
    req += "Upgrade: websocket\r\n";
    req += "Connection: Upgrade\r\n";
    req += "Sec-WebSocket-Key: " + key + "\r\n";
    req += "Sec-WebSocket-Version: 13\r\n";
    if (opts.subprotocol && opts.subprotocol[0])
        req += std::string("Sec-WebSocket-Protocol: ") + opts.subprotocol + "\r\n";
    for (CHAOS_IL2CPP_INT32 i = 0; i < opts.headerCount; ++i) {
        req += opts.headers[i].name;
        req += ": ";
        req += opts.headers[i].value;
        req += "\r\n";
    }
    req += "\r\n";
    NetError err = SendAll(c, reinterpret_cast<const CHAOS_IL2CPP_UINT8*>(req.data()),
                           req.size(), timeoutMs);
    if (err != NetError::None) return err;

    std::string status;
    if (!ReadLine(c, &status, timeoutMs)) return NetError::ConnectionReset;
    if (status.compare(0, 9, "HTTP/1.1 ") != 0 && status.compare(0, 9, "HTTP/1.0 ") != 0)
        return NetError::Unsupported;
    if (status.find(" 101 ") == std::string::npos) {
            return NetError::Unsupported;  // non-101 upgrade response
    }

    std::string accept;
    for (;;) {
        std::string line;
        if (!ReadLine(c, &line, timeoutMs)) return NetError::ConnectionReset;
        if (line.empty()) break;  // end of headers
        std::size_t colon = line.find(':');
        if (colon == std::string::npos) continue;
        std::string name = line.substr(0, colon);
        std::string value = line.substr(colon + 1);
        if (!value.empty() && value[0] == ' ') value.erase(0, 1);
        if (name.size() == 20 &&
            name.compare(0, 20, "Sec-WebSocket-Accept") == 0)
            accept = value;
    }
    if (accept.empty()) return NetError::Unsupported;

    char expect[29];
    if (!WsComputeAccept(key.c_str(), expect, sizeof(expect)))
        return NetError::Unsupported;
    if (accept != expect) {
            return NetError::Unsupported;
    }
    return NetError::None;
}

// Reads one CRLF-terminated line (no CRLF in out); false on EOF/error.
bool ReadLine(WsConn* c, std::string* out, CHAOS_IL2CPP_INT32 timeoutMs) {
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

}  // namespace

NetError WsOpen(const WsOptions& opts, WsClient** out) noexcept {
    *out = nullptr;
    if (opts.url == nullptr || opts.url[0] == 0) return NetError::Unsupported;
    bool wss = false;
    std::string host, path;
    CHAOS_IL2CPP_UINT16 port = 0;
    if (!ParseWsUrl(opts.url, &wss, &host, &port, &path)) {
            return NetError::Unsupported;
    }

    WsConn* c = new WsConn();
    NetError err = NetSocketCreate(kAddressFamilyInet, kSocketTypeStream,
                                   kProtocolTcp, &c->sock);
    if (err == NetError::None)
        err = NetSocketSetOption(&c->sock, NetOption::NoDelay, 1);
    NetAddress addr{};
    if (err == NetError::None)
        err = DnsResolveEndPoint(host.c_str(), port, &addr);
    if (err == NetError::None)
        err = NetSocketConnect(&c->sock, addr, opts.timeoutMs);
    if (err == NetError::None && wss) {
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
    if (err == NetError::None)
        err = Handshake(c, host, port, path, opts, opts.timeoutMs);
    if (err != NetError::None) {
            if (c->tls) {
            c->tls->Shutdown();
            delete c->tls;
        }
        if (c->sock.fd >= 0) NetSocketClose(&c->sock);
        delete c;
        return err;
    }
    *out = reinterpret_cast<WsClient*>(c);
    return NetError::None;
}

NetError WsSendFrame(WsClient* c, WsOpcode opcode, bool fin,
                     const CHAOS_IL2CPP_UINT8* data, CHAOS_IL2CPP_INT32 len,
                     CHAOS_IL2CPP_INT32 timeoutMs) noexcept {
    WsConn* w = reinterpret_cast<WsConn*>(c);
    if (w == nullptr || w->dead || len < 0) return NetError::Unsupported;
    CHAOS_IL2CPP_UINT32 oc = static_cast<CHAOS_IL2CPP_UINT32>(opcode);
    bool control = (oc & 0x8) != 0;
    if (control && (len > 125 || !fin)) return NetError::Unsupported;
    return SendFrame(w, opcode, fin, data, static_cast<std::size_t>(len), timeoutMs);
}

NetError WsRecv(WsClient* c, WsFrame* out, CHAOS_IL2CPP_INT32 timeoutMs) noexcept {
    WsConn* w = reinterpret_cast<WsConn*>(c);
    if (w == nullptr || out == nullptr || w->dead) return NetError::Unsupported;
    return RecvFrame(w, out, timeoutMs);
}

NetError WsClose(WsClient* c, CHAOS_IL2CPP_UINT16 code, const char* reason,
                 CHAOS_IL2CPP_INT32 timeoutMs) noexcept {
    WsConn* w = reinterpret_cast<WsConn*>(c);
    if (w == nullptr || w->dead) return NetError::Unsupported;
    std::string payload;
    payload.push_back(static_cast<char>((code >> 8) & 0xFF));
    payload.push_back(static_cast<char>(code & 0xFF));
    if (reason) payload += reason;
    NetError err = SendFrame(w, WsOpcode::Close, true,
                             reinterpret_cast<const CHAOS_IL2CPP_UINT8*>(payload.data()),
                             payload.size(), timeoutMs);
    if (err != NetError::None) return err;
    // Await the peer's Close echo (§5.5.1).
    for (;;) {
        WsFrame f;
        err = RecvFrame(w, &f, timeoutMs);
        if (err != NetError::None) return err;
        if (f.opcode == WsOpcode::Close) return NetError::None;
    }
}

void WsFree(WsClient* c) noexcept {
    if (c == nullptr) return;
    WsConn* w = reinterpret_cast<WsConn*>(c);
    if (w->tls) {
        w->tls->Shutdown();
        delete w->tls;
    }
    if (w->sock.fd >= 0) NetSocketClose(&w->sock);
    delete w;
}

bool WsComputeAccept(const char* clientKey, char* out, std::size_t outSize) noexcept {
    if (clientKey == nullptr || out == nullptr || outSize < 29) return false;
    std::string blob = std::string(clientKey) + kGuid;
    CHAOS_IL2CPP_UINT8 digest[20];
    Sha1(reinterpret_cast<const CHAOS_IL2CPP_UINT8*>(blob.data()), blob.size(), digest);
    std::string b64 = Base64Encode(digest, sizeof(digest));
    if (b64.size() + 1 > outSize) return false;
    std::memcpy(out, b64.data(), b64.size());
    out[b64.size()] = 0;
    return true;
}

}  // namespace chaos::net
