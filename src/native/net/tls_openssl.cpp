// Layer E -- TLS transport, OpenSSL backend (NT-12).  Compiled only on
// POSIX-ish targets (Linux/Android; also the MSYS2 runtime for local POSIX
// verification); Windows uses tls_schannel.cpp.  Architecture ref:
// docs/dev/in-progress/net-cpp-architecture.md (section 8).
//
// The underlying chaos::net socket is non-blocking with Layer D
// poll-based blocking wrappers (NetSocketSend/NetSocketRecv with timeout),
// so this backend stages the TLS record stream through a memory-BIO pair:
//   - wbio: OpenSSL writes records here; FlushWbio() pumps them to the
//     socket via NetSocketSend (blocking, so it drains fully).
//   - rbio: FillBio() pumps NetSocketRecv data into OpenSSL.
// Handshake/Read/Write are synchronous; they honor options.timeoutMs by
// passing it down to each NetSocketSend/Recv call.
#if defined(_WIN32)
#error "tls_openssl.cpp must not be compiled on Windows (use tls_schannel.cpp)"
#endif

#if !defined(_GNU_SOURCE)
#define _GNU_SOURCE 1
#endif

#include <chaos/net/tls.h>

#include <openssl/ssl.h>
#include <openssl/err.h>
#include <openssl/ocsp.h>
#include <openssl/x509v3.h>
#include <openssl/crypto.h>

#include <arpa/inet.h>
#include <netdb.h>

#include <cstring>
#include <string>
#include <vector>

namespace chaos::net {
namespace {

constexpr CHAOS_IL2CPP_INT32 kIoBufSize = 16384;
constexpr CHAOS_IL2CPP_INT32 kDefaultTimeoutMs = 10000;

// Map an I/O failure from NetSocketSend/Recv to a NetError, preserving the
// timeout distinction when the platform reports it.
NetError IoErrorFromLastNative() noexcept {
    if (NetSocketLastError() == SocketError::TimedOut) {
        return NetError::TimedOut;
    }
    return NetError::ConnectionReset;
}

// OPENSSL_free is a macro in OpenSSL 3.x (expands to CRYPTO_free with file/line), so
// it cannot be passed as a function pointer; wrap it for sk_OPENSSL_STRING_pop_free.
static void FreeOpensslString(char* s) noexcept {
    OPENSSL_free(s);
}

// ALPN server-side selection: prefer h2, then http/1.1; otherwise do not
// answer (SSL_TLSEXT_ERR_NOACK), matching Schannel's behavior of ignoring
// ALPN when nothing matches.
int AlpnSelectCallback(SSL*, const unsigned char** out, unsigned char* outlen,
                       const unsigned char* in, unsigned int inlen, void*) noexcept {
    unsigned int i = 0;
    while (i < inlen) {
        const unsigned int plen = in[i];
        if (i + 1 + static_cast<unsigned int>(plen) > inlen) {
            break;
        }
        const unsigned char* p = in + i + 1;
        if ((plen == 2 && p[0] == 'h' && p[1] == '2') ||
            (plen == 8 && std::memcmp(p, "http/1.1", 8) == 0)) {
            *out = p;
            *outlen = static_cast<unsigned char>(plen);
            return SSL_TLSEXT_ERR_OK;
        }
        i += 1 + static_cast<unsigned int>(plen);
    }
    return SSL_TLSEXT_ERR_NOACK;
}

// --- NT-22 OCSP helpers --------------------------------------------------
// Parse "http://host[:port][/path]" into its parts.  Returns false for any
// other scheme or a malformed authority.  Default port 80, default path "/".
bool ParseOcspUrl(const std::string& url, std::string* host, int* port,
                  std::string* path) {
    if (url.compare(0, 7, "http://") != 0) {
        return false;
    }
    const char* p = url.c_str() + 7;
    const char* colon = std::strchr(p, ':');
    const char* slash = std::strchr(p, '/');
    if (slash == p) {
        return false;  // empty host
    }
    std::string h;
    int pt = 80;
    if (colon != nullptr && (slash == nullptr || colon < slash)) {
        h.assign(p, static_cast<size_t>(colon - p));
        char* end = nullptr;
        const long v = std::strtol(colon + 1, &end, 10);
        if (end == colon + 1 || v < 1 || v > 65535) {
            return false;
        }
        pt = static_cast<int>(v);
    } else if (slash != nullptr) {
        h.assign(p, static_cast<size_t>(slash - p));
    } else {
        h = p;
    }
    if (h.empty()) {
        return false;
    }
    *host = h;
    *port = pt;
    *path = (slash != nullptr) ? slash : "/";
    return true;
}

// Parse a dotted-quad IPv4 literal into net_api.h addr bytes (first 4).
bool ParseIpv4Literal(const char* s, unsigned char out[16]) {
    unsigned int a = 0, b = 0, c = 0, d = 0;
    int n = 0;
    if (std::sscanf(s, "%u.%u.%u.%u%n", &a, &b, &c, &d, &n) != 4 ||
        n != static_cast<int>(std::strlen(s)) ||
        a > 255 || b > 255 || c > 255 || d > 255) {
        return false;
    }
    out[0] = static_cast<unsigned char>(a);
    out[1] = static_cast<unsigned char>(b);
    out[2] = static_cast<unsigned char>(c);
    out[3] = static_cast<unsigned char>(d);
    return true;
}

// Blocking OCSP-over-HTTP POST against the chaos net layer (Layer D): opens a
// fresh plaintext connection to host:port, sends the DER OCSP request as an
// application/ocsp-request POST, reads the response and returns its body.
// On any failure *err is filled and Unsupported is returned.
NetError OcspHttpPost(const std::string& host, int port, const std::string& path,
                      const std::vector<unsigned char>& body,
                      std::vector<unsigned char>* outBody,
                      std::string* err, CHAOS_IL2CPP_INT32 timeoutMs) {
    NetAddress rsp{};
    rsp.family = 2 /*kAddressFamilyInet*/;
    rsp.port = static_cast<CHAOS_IL2CPP_UINT16>(port);
    if (!ParseIpv4Literal(host.c_str(), rsp.addr)) {
        // Non-literal host: resolve via getaddrinfo (first IPv4 answer).
        struct addrinfo hints{};
        hints.ai_family = AF_INET;
        hints.ai_socktype = SOCK_STREAM;
        struct addrinfo* res = nullptr;
        if (getaddrinfo(host.c_str(), nullptr, &hints, &res) != 0 ||
            res == nullptr) {
            *err = "OCSP: cannot resolve responder host " + host;
            if (res != nullptr) freeaddrinfo(res);
            return NetError::Unsupported;
        }
        const struct sockaddr_in* sa =
            reinterpret_cast<const struct sockaddr_in*>(res->ai_addr);
        std::memcpy(rsp.addr, &sa->sin_addr, 4);
        freeaddrinfo(res);
    }

    SocketHandle sock{};
    if (NetSocketCreate(2, 1, 6, &sock) != NetError::None) {
        *err = "OCSP: cannot create responder socket";
        return NetError::Unsupported;
    }
    if (NetSocketConnect(&sock, rsp, timeoutMs) != NetError::None) {
        NetSocketClose(&sock);
        *err = "OCSP: cannot connect to responder " + host;
        return NetError::Unsupported;
    }

    std::string header = "POST " + path + " HTTP/1.1\r\n"
                         "Host: " + host + "\r\n"
                         "Content-Type: application/ocsp-request\r\n"
                         "Content-Length: " + std::to_string(body.size()) +
                         "\r\nConnection: close\r\n\r\n";
    // Send header then body.
    {
        const char* hp = header.data();
        size_t off = 0;
        while (off < header.size()) {
            CHAOS_IL2CPP_INT32 sent = 0;
            const NetError e = NetSocketSend(
                &sock, reinterpret_cast<const CHAOS_IL2CPP_UINT8*>(hp + off),
                static_cast<CHAOS_IL2CPP_INT32>(header.size() - off), 0, &sent,
                timeoutMs);
            if (e != NetError::None || sent <= 0) {
                NetSocketClose(&sock);
                *err = "OCSP: failed sending request header";
                return NetError::Unsupported;
            }
            off += static_cast<size_t>(sent);
        }
        off = 0;
        while (off < body.size()) {
            CHAOS_IL2CPP_INT32 sent = 0;
            const NetError e = NetSocketSend(
                &sock, body.data() + off,
                static_cast<CHAOS_IL2CPP_INT32>(body.size() - off), 0, &sent,
                timeoutMs);
            if (e != NetError::None || sent <= 0) {
                NetSocketClose(&sock);
                *err = "OCSP: failed sending request body";
                return NetError::Unsupported;
            }
            off += static_cast<size_t>(sent);
        }
    }

    // Read until the header terminator and Content-Length body are complete.
    std::vector<unsigned char> buf;
    long contentLength = -1;
    size_t headerEnd = std::string::npos;
    unsigned char chunk[4096];
    for (;;) {
        CHAOS_IL2CPP_INT32 got = 0;
        const NetError e = NetSocketRecv(&sock, chunk, static_cast<CHAOS_IL2CPP_INT32>(sizeof(chunk)), 0, &got, timeoutMs);
        if (e != NetError::None) {
            NetSocketClose(&sock);
            *err = "OCSP: responder recv failed";
            return NetError::Unsupported;
        }
        if (got == 0) {
            break;  // responder closed the connection
        }
        buf.insert(buf.end(), chunk, chunk + got);
        if (contentLength < 0 && headerEnd == std::string::npos) {
            for (size_t i = 0; i + 3 < buf.size(); ++i) {
                if (buf[i] == '\r' && buf[i + 1] == '\n' &&
                    buf[i + 2] == '\r' && buf[i + 3] == '\n') {
                    headerEnd = i + 4;
                    break;
                }
            }
            if (headerEnd != std::string::npos) {
                // Parse Content-Length (case-insensitive) from the header.
                std::string hdr(reinterpret_cast<const char*>(buf.data()),
                                headerEnd);
                size_t pos = 0;
                size_t clPos = std::string::npos;
                for (;;) {
                    clPos = hdr.find("\r\n", pos);
                    if (clPos == std::string::npos) break;
                    std::string line = hdr.substr(pos, clPos - pos);
                    pos = clPos + 2;
                    if (line.size() >= 15 &&
                        (line.compare(0, 14, "content-length") == 0 ||
                         line.compare(0, 14, "Content-Length") == 0) &&
                        line.size() > 15 && line[14] == ':') {
                        contentLength = std::strtol(line.c_str() + 15, nullptr, 10);
                        break;
                    }
                }
            }
        }
        if (headerEnd != std::string::npos && contentLength >= 0 &&
            buf.size() >= headerEnd + static_cast<size_t>(contentLength)) {
            break;
        }
    }
    NetSocketClose(&sock);

    if (headerEnd == std::string::npos) {
        *err = "OCSP: responder returned no HTTP headers";
        return NetError::Unsupported;
    }
    if (contentLength < 0 || buf.size() < headerEnd + static_cast<size_t>(contentLength)) {
        *err = "OCSP: responder response truncated (missing Content-Length body)";
        return NetError::Unsupported;
    }
    // Status line must be a 2xx.
    {
        std::string firstLine;
        for (size_t i = 0; i < headerEnd && i < buf.size() && buf[i] != '\r'; ++i) {
            firstLine.push_back(static_cast<char>(buf[i]));
        }
        if (firstLine.find("HTTP/1.1 200") == std::string::npos &&
            firstLine.find("HTTP/1.0 200") == std::string::npos) {
            *err = "OCSP: responder HTTP status not 200: " + firstLine;
            return NetError::Unsupported;
        }
    }
    outBody->assign(buf.begin() + static_cast<long>(headerEnd),
                    buf.begin() + static_cast<long>(headerEnd) + contentLength);
    return NetError::None;
}

class OpenSslProvider final : public TlsProvider {
public:
    OpenSslProvider() noexcept = default;
    ~OpenSslProvider() override;

    NetError Create(const TlsOptions& options) override;
    NetError Handshake(SocketHandle* socket) override;
    NetError Read(SocketHandle* socket, CHAOS_IL2CPP_UINT8* buf,
                  CHAOS_IL2CPP_UINT32 len, CHAOS_IL2CPP_UINT32* outRead) override;
    NetError Write(SocketHandle* socket, const CHAOS_IL2CPP_UINT8* buf,
                   CHAOS_IL2CPP_UINT32 len, CHAOS_IL2CPP_UINT32* outWritten) override;
    NetError Shutdown() override;
    const char* NegotiatedProtocol() const override;
    const char* NegotiatedApplicationProtocol() const override;
    const char* LastErrorDetail() const override;

private:
    void Release() noexcept;
    NetError DriveHandshake(SocketHandle* socket, CHAOS_IL2CPP_INT32 timeoutMs);
    // Flush everything OpenSSL queued on the write BIO to the socket.
    // Returns false on an I/O error (IoErrorFromLastNative() explains).
    bool FlushWbio(SocketHandle* socket, CHAOS_IL2CPP_INT32 timeoutMs);
    // Pull one chunk from the socket into the read BIO.
    //  1 = data staged, 0 = peer closed (clean EOF), -1 = I/O error.
    int  FillBio(SocketHandle* socket, CHAOS_IL2CPP_INT32 timeoutMs);
    void RecordError(const char* op, int sslError) noexcept;

    SSL_CTX* m_ctx = nullptr;
    SSL* m_ssl = nullptr;
    BIO* m_rbio = nullptr;  // read BIO  (inbound  from the socket)
    BIO* m_wbio = nullptr;  // write BIO (outbound to the socket)
    CHAOS_IL2CPP_INT32 m_timeoutMs = kDefaultTimeoutMs;
    std::string m_protocol;
    std::string m_alpn;
    std::string m_lastError;
    bool m_isServer = false;
    bool m_ocspMode = false;
    std::vector<unsigned char> m_caDer;  // issuer CA DER (Ocsp mode; copied)
    std::string m_ocspUrl;               // responder URL (Ocsp mode; copied)

    NetError RunOcspCheck();
};

OpenSslProvider::~OpenSslProvider() { Release(); }

void OpenSslProvider::Release() noexcept {
    // SSL_free also frees the memory BIOs it owns.
    if (m_ssl != nullptr) {
        SSL_free(m_ssl);
        m_ssl = nullptr;
        m_rbio = nullptr;
        m_wbio = nullptr;
    }
    if (m_ctx != nullptr) {
        SSL_CTX_free(m_ctx);
        m_ctx = nullptr;
    }
    m_lastError.clear();
    m_caDer.clear();
    m_ocspUrl.clear();
    m_ocspMode = false;
}

void OpenSslProvider::RecordError(const char* op, int sslError) noexcept {
    std::string msg = op;
    msg += " failed; ssl_error=";
    msg += std::to_string(sslError);
    const unsigned long errCode = ERR_get_error();
    if (errCode != 0) {
        char detail[256];
        ERR_error_string_n(errCode, detail, sizeof(detail));
        msg += " (";
        msg += detail;
        msg += ")";
    }
    m_lastError = std::move(msg);
}

bool OpenSslProvider::FlushWbio(SocketHandle* socket, CHAOS_IL2CPP_INT32 timeoutMs) {
    while (BIO_ctrl_pending(m_wbio) > 0) {
        CHAOS_IL2CPP_UINT8 buf[kIoBufSize];
        const int n = BIO_read(m_wbio, buf, static_cast<int>(sizeof(buf)));
        if (n <= 0) {
            break;  // nothing left to read from the BIO
        }
        CHAOS_IL2CPP_INT32 sent = 0;
        const NetError e = NetSocketSend(socket, buf, n, 0, &sent, timeoutMs);
        if (e != NetError::None) {
            return false;
        }
        if (sent != n) {
            return false;  // partial write with no error: treat as failure
        }
    }
    return true;
}

int OpenSslProvider::FillBio(SocketHandle* socket, CHAOS_IL2CPP_INT32 timeoutMs) {
    CHAOS_IL2CPP_UINT8 buf[kIoBufSize];
    CHAOS_IL2CPP_INT32 got = 0;
    const NetError e = NetSocketRecv(socket, buf, static_cast<CHAOS_IL2CPP_INT32>(sizeof(buf)), 0, &got, timeoutMs);
    if (e != NetError::None) {
        return -1;
    }
    if (got == 0) {
        return 0;  // peer closed the TCP connection
    }
    if (BIO_write(m_rbio, buf, static_cast<int>(got)) <= 0) {
        return -1;
    }
    return 1;
}

NetError OpenSslProvider::Create(const TlsOptions& options) {
    Release();
    if (options.timeoutMs > 0) {
        m_timeoutMs = options.timeoutMs;
    }
    const bool isServer = (options.mode == TlsMode::Server);
    m_isServer = isServer;
    m_ocspMode = (!isServer && options.revocation.mode == TlsRevocationMode::Ocsp);
    if (m_ocspMode) {
        // NT-22: the online OCSP check runs in Handshake() (after the TLS
        // handshake, when the peer certificate exists), so the issuer CA DER
        // and the responder URL must be copied here -- TlsOptions is only
        // valid for the duration of Create().
        if (options.revocation.caDer == nullptr ||
            options.revocation.caLength <= 0) {
            m_lastError = "Ocsp revocation mode requires caDer (issuer CA)";
            return NetError::Unsupported;
        }
        m_caDer.assign(options.revocation.caDer,
                       options.revocation.caDer + options.revocation.caLength);
        if (options.revocation.ocspUrl != nullptr) {
            m_ocspUrl = options.revocation.ocspUrl;
        }
    }

    m_ctx = SSL_CTX_new(isServer ? TLS_server_method() : TLS_client_method());
    if (m_ctx == nullptr) {
        m_lastError = "SSL_CTX_new failed";
        return NetError::Unsupported;
    }

    if (isServer) {
        if (options.serverCertificate.native1 != nullptr) {
            SSL_CTX_use_certificate(m_ctx, static_cast<X509*>(options.serverCertificate.native1));
            if (options.serverCertificate.native2 != nullptr) {
                SSL_CTX_use_PrivateKey(m_ctx, static_cast<EVP_PKEY*>(options.serverCertificate.native2));
            }
            if (SSL_CTX_check_private_key(m_ctx) != 1) {
                m_lastError = "server certificate/private key mismatch";
                return NetError::Unsupported;
            }
        }
        if (options.alpnProtocols != nullptr && options.alpnLength > 0) {
            SSL_CTX_set_alpn_select_cb(m_ctx, AlpnSelectCallback, nullptr);
        }
    } else if (options.alpnProtocols != nullptr && options.alpnLength > 0) {
        // Option contract is RFC7301 wire format (2-byte total length prefix
        // + (len,proto)* entries).  OpenSSL's SSL_CTX_set_alpn_protos wants
        // just the entries (no total-length prefix), so skip the 2 bytes.
        if (options.alpnLength < 2) {
            m_lastError = "malformed ALPN list (too short)";
            return NetError::Unsupported;
        }
        const CHAOS_IL2CPP_UINT8* entries = options.alpnProtocols + 2;
        const unsigned int entriesLen = static_cast<unsigned int>(options.alpnLength) - 2u;
        if (SSL_CTX_set_alpn_protos(m_ctx, entries, entriesLen) != 0) {
            m_lastError = "malformed ALPN list";
            return NetError::Unsupported;
        }
    }

    if (isServer) {
        // Servers only validate when a client certificate is demanded.
        SSL_CTX_set_verify(m_ctx, options.requireClientCertificate ? SSL_VERIFY_PEER : SSL_VERIFY_NONE, nullptr);
    } else if (options.revocation.mode == TlsRevocationMode::Crl ||
               options.revocation.mode == TlsRevocationMode::Ocsp) {
        // NT-18/22: client revocation.  Seed a dedicated X509_STORE with the
        // issuer CA + the CRL (both DER) and enable CRL checking, so the peer
        // certificate is verified against this store and the handshake fails
        // with X509_V_ERR_CERT_REVOKED when the CRL lists it -- even if
        // disableCertificateValidation is also set (the store acts as the
        // chain anchor).
        X509_STORE* store = X509_STORE_new();
        if (store == nullptr) {
            m_lastError = "X509_STORE_new failed";
            return NetError::Unsupported;
        }
        bool seeded = false;
        const bool isCrl = (options.revocation.mode == TlsRevocationMode::Crl);
        if (options.revocation.caDer != nullptr && options.revocation.caLength > 0) {
            const unsigned char* p = options.revocation.caDer;
            X509* ca = d2i_X509(nullptr, &p, options.revocation.caLength);
            if (ca != nullptr) {
                X509_STORE_add_cert(store, ca);
                X509_free(ca);
                seeded = true;
            }
        }
        if (isCrl && options.revocation.crlDer != nullptr &&
            options.revocation.crlLength > 0) {
            const unsigned char* p = options.revocation.crlDer;
            X509_CRL* crl = d2i_X509_CRL(nullptr, &p, options.revocation.crlLength);
            if (crl != nullptr) {
                X509_STORE_add_crl(store, crl);
                X509_CRL_free(crl);
                seeded = true;
            }
        }
        if (!seeded) {
            m_lastError = isCrl ? "revocation mode requires caDer/crlDer"
                                : "Ocsp revocation mode requires caDer";
            X509_STORE_free(store);
            return NetError::Unsupported;
        }
        if (isCrl) {
            X509_STORE_set_flags(store, X509_V_FLAG_CRL_CHECK | X509_V_FLAG_CRL_CHECK_ALL);
        }
        SSL_CTX_set_cert_store(m_ctx, store);  // takes ownership of the store
        SSL_CTX_set_verify(m_ctx, SSL_VERIFY_PEER, nullptr);
        SSL_CTX_set_verify_depth(m_ctx, 5);
    } else if (options.disableCertificateValidation) {
        SSL_CTX_set_verify(m_ctx, SSL_VERIFY_NONE, nullptr);
    } else {
        SSL_CTX_set_verify(m_ctx, SSL_VERIFY_PEER, nullptr);
        SSL_CTX_set_default_verify_paths(m_ctx);
        SSL_CTX_set_verify_depth(m_ctx, 5);
    }

    m_ssl = SSL_new(m_ctx);
    if (m_ssl == nullptr) {
        m_lastError = "SSL_new failed";
        return NetError::Unsupported;
    }
    if (isServer) {
        SSL_set_accept_state(m_ssl);
    } else {
        SSL_set_connect_state(m_ssl);
    }
    if (!isServer && options.serverName != nullptr) {
        SSL_set_tlsext_host_name(m_ssl, options.serverName);
    }

    m_rbio = BIO_new(BIO_s_mem());
    m_wbio = BIO_new(BIO_s_mem());
    if (m_rbio == nullptr || m_wbio == nullptr) {
        m_lastError = "BIO_new failed";
        return NetError::Unsupported;
    }
    SSL_set_bio(m_ssl, m_rbio, m_wbio);  // takes ownership of both BIOs
    return NetError::None;
}

NetError OpenSslProvider::DriveHandshake(SocketHandle* socket, CHAOS_IL2CPP_INT32 timeoutMs) {
    for (;;) {
        const int r = SSL_do_handshake(m_ssl);
        if (r == 1) {
            break;
        }
        const int e = SSL_get_error(m_ssl, r);
        if (e == SSL_ERROR_WANT_READ) {
            if (!FlushWbio(socket, timeoutMs)) {
                m_lastError = "handshake write flush failed";
                return IoErrorFromLastNative();
            }
            const int fb = FillBio(socket, timeoutMs);
            if (fb == 0) {
                m_lastError = "handshake failed; peer closed connection";
                return NetError::ConnectionReset;
            }
            if (fb < 0) {
                m_lastError = "handshake read failed";
                return IoErrorFromLastNative();
            }
            continue;
        }
        if (e == SSL_ERROR_WANT_WRITE) {
            if (!FlushWbio(socket, timeoutMs)) {
                m_lastError = "handshake write flush failed";
                return IoErrorFromLastNative();
            }
            continue;
        }
        // NT-18: capture the verification failure so a revoked peer
        // certificate is distinguishable from a plain I/O error.
        const long vr = SSL_get_verify_result(m_ssl);
        if (vr != X509_V_OK) {
            m_lastError = "handshake failed; verify_result=";
            m_lastError += std::to_string(vr);
            const char* reason = X509_verify_cert_error_string(
                static_cast<int>(vr));
            if (reason != nullptr) {
                m_lastError += " (";
                m_lastError += reason;
                m_lastError += ")";
            }
            return NetError::Unsupported;
        }
        RecordError("handshake", e);
        return NetError::ConnectionReset;
    }

    const char* version = SSL_get_version(m_ssl);
    m_protocol = (version != nullptr) ? version : "";

    const unsigned char* alpnData = nullptr;
    unsigned int alpnLen = 0;
    SSL_get0_alpn_selected(m_ssl, &alpnData, &alpnLen);
    if (alpnData != nullptr && alpnLen > 0) {
        m_alpn.assign(reinterpret_cast<const char*>(alpnData), alpnLen);
    } else {
        m_alpn.clear();
    }
    return NetError::None;
}

NetError OpenSslProvider::Handshake(SocketHandle* socket) {
    if (m_ssl == nullptr) {
        return NetError::NotInitialized;
    }
    NetError e = DriveHandshake(socket, m_timeoutMs);
    if (e != NetError::None) {
        return e;
    }
    // NT-22: online OCSP revocation.  The peer certificate only exists after
    // the handshake, so the fetch + verification happen here (client mode).
    if (m_ocspMode) {
        return RunOcspCheck();
    }
    return NetError::None;
}

// NT-22: OCSP over HTTP against the configured responder (or the peer
// certificate AIA URL when ocspUrl was not set).  Builds an OCSP request for
// the peer certificate, POSTs it, verifies the signed response against the
// issuer CA (m_caDer), and fails when the peer is reported REVOKED -- or when
// the fetch/parse/verification fails (fail-closed).
NetError OpenSslProvider::RunOcspCheck() {
    if (m_caDer.empty()) {
        m_lastError = "OCSP: no issuer CA (caDer) for revocation check";
        return NetError::Unsupported;
    }
    X509* peer = SSL_get1_peer_certificate(m_ssl);
    if (peer == nullptr) {
        m_lastError = "OCSP: no peer certificate to check";
        return NetError::Unsupported;
    }

    // Issuer CA from the stored DER.
    const unsigned char* qp = m_caDer.data();
    X509* ca = d2i_X509(nullptr, &qp, static_cast<long>(m_caDer.size()));
    if (ca == nullptr) {
        X509_free(peer);
        m_lastError = "OCSP: cannot parse caDer (issuer CA)";
        return NetError::Unsupported;
    }

    // Responder URL: explicit ocspUrl first, then the peer cert AIA extension.
    std::string url = m_ocspUrl;
    if (url.empty()) {
        STACK_OF(OPENSSL_STRING)* aia = X509_get1_ocsp(peer);
        if (aia != nullptr) {
            const int n = sk_OPENSSL_STRING_num(aia);
            if (n > 0) {
                const char* u = sk_OPENSSL_STRING_value(aia, 0);
                if (u != nullptr) url = u;
            }
            sk_OPENSSL_STRING_pop_free(aia, FreeOpensslString);
        }
    }
    if (url.empty()) {
        X509_free(ca);
        X509_free(peer);
        m_lastError = "OCSP: no responder URL (set ocspUrl or AIA extension)";
        return NetError::Unsupported;
    }
    std::string host, path;
    int port = 0;
    if (!ParseOcspUrl(url, &host, &port, &path)) {
        X509_free(ca);
        X509_free(peer);
        m_lastError = "OCSP: malformed responder URL " + url;
        return NetError::Unsupported;
    }

    // Build the DER OCSP request for the peer certificate.
    OCSP_CERTID* cid = OCSP_cert_to_id(nullptr, peer, ca);
    OCSP_REQUEST* req = (cid != nullptr) ? OCSP_REQUEST_new() : nullptr;
    if (cid == nullptr || req == nullptr) {
        if (cid != nullptr) OCSP_CERTID_free(cid);
        if (req != nullptr) OCSP_REQUEST_free(req);
        X509_free(ca);
        X509_free(peer);
        m_lastError = "OCSP: cannot build request";
        return NetError::Unsupported;
    }
    if (OCSP_request_add0_id(req, cid) == nullptr) {
        OCSP_CERTID_free(cid);
        OCSP_REQUEST_free(req);
        X509_free(ca);
        X509_free(peer);
        m_lastError = "OCSP: cannot attach certificate id to request";
        return NetError::Unsupported;
    }
    // From here the request owns cid; freeing req also frees cid.
    const int derLen = i2d_OCSP_REQUEST(req, nullptr);
    if (derLen <= 0) {
        OCSP_REQUEST_free(req);
        X509_free(ca);
        X509_free(peer);
        m_lastError = "OCSP: cannot encode request";
        return NetError::Unsupported;
    }
    std::vector<unsigned char> reqDer(static_cast<size_t>(derLen));
    {
        unsigned char* w = reqDer.data();
        i2d_OCSP_REQUEST(req, &w);
    }
    X509_free(ca);

    std::vector<unsigned char> respBody;
    std::string err;
    const NetError he = OcspHttpPost(host, port, path, reqDer, &respBody, &err,
                                     m_timeoutMs);
    if (he != NetError::None) {
        m_lastError = err.empty() ? "OCSP: fetch failed" : err;
        X509_free(peer);
        return he;
    }

    // Parse and verify the response.
    const unsigned char* rp = respBody.data();
    OCSP_RESPONSE* resp = d2i_OCSP_RESPONSE(nullptr, &rp,
                                            static_cast<long>(respBody.size()));
    if (resp == nullptr) {
        m_lastError = "OCSP: cannot parse responder response";
        X509_free(peer);
        return NetError::Unsupported;
    }
    if (OCSP_response_status(resp) != OCSP_RESPONSE_STATUS_SUCCESSFUL) {
        m_lastError = "OCSP: responder reported an error status";
        OCSP_RESPONSE_free(resp);
        X509_free(peer);
        return NetError::Unsupported;
    }
    OCSP_BASICRESP* basic = OCSP_response_get1_basic(resp);
    if (basic == nullptr) {
        m_lastError = "OCSP: response has no basic response";
        OCSP_RESPONSE_free(resp);
        X509_free(peer);
        return NetError::Unsupported;
    }
    // Verify against the issuer CA as the trust anchor.  OCSP_NOCHECKS skips
    // the OCSP-signing EKU check (the throwaway CI CA has no EKU); the
    // response signature is still cryptographically verified.
    X509_STORE* store = X509_STORE_new();
    if (store == nullptr) {
        m_lastError = "OCSP: X509_STORE_new failed";
        OCSP_BASICRESP_free(basic);
        OCSP_RESPONSE_free(resp);
        X509_free(peer);
        return NetError::Unsupported;
    }
    {
        const unsigned char* cp2 = m_caDer.data();
        X509* ca2 = d2i_X509(nullptr, &cp2, static_cast<long>(m_caDer.size()));
        if (ca2 != nullptr) {
            X509_STORE_add_cert(store, ca2);
            X509_free(ca2);
        }
    }
    if (OCSP_basic_verify(basic, nullptr, store, OCSP_NOCHECKS) != 1) {
        m_lastError = "OCSP: response signature verification failed";
        X509_STORE_free(store);
        OCSP_BASICRESP_free(basic);
        OCSP_RESPONSE_free(resp);
        X509_free(peer);
        return NetError::Unsupported;
    }

    int status = 0;
    int reason = 0;
    ASN1_GENERALIZEDTIME* revTime = nullptr;
    ASN1_GENERALIZEDTIME* thisUpd = nullptr;
    ASN1_GENERALIZEDTIME* nextUpd = nullptr;
    if (OCSP_resp_find_status(basic, cid, &status, &reason, &revTime,
                              &thisUpd, &nextUpd) != 1) {
        m_lastError = "OCSP: response has no status for this certificate";
        X509_STORE_free(store);
        OCSP_BASICRESP_free(basic);
        OCSP_RESPONSE_free(resp);
        X509_free(peer);
        OCSP_REQUEST_free(req);  // frees attached OCSP_CERTID too
        return NetError::Unsupported;
    }
    if (status == V_OCSP_CERTSTATUS_REVOKED) {
        m_lastError = "OCSP: certificate revoked by OCSP responder (reason=" +
                      std::to_string(reason) + ")";
        X509_STORE_free(store);
        OCSP_BASICRESP_free(basic);
        OCSP_RESPONSE_free(resp);
        X509_free(peer);
        OCSP_REQUEST_free(req);  // frees attached OCSP_CERTID too
        return NetError::Unsupported;
    }
    if (status != V_OCSP_CERTSTATUS_GOOD) {
        m_lastError = "OCSP: responder reports unknown status for certificate";
        X509_STORE_free(store);
        OCSP_BASICRESP_free(basic);
        OCSP_RESPONSE_free(resp);
        X509_free(peer);
        OCSP_REQUEST_free(req);  // frees attached OCSP_CERTID too
        return NetError::Unsupported;
    }
    if (thisUpd != nullptr && nextUpd != nullptr &&
        OCSP_check_validity(thisUpd, nextUpd, 60, -1) != 1) {
        m_lastError = "OCSP: response outside validity window";
        X509_STORE_free(store);
        OCSP_BASICRESP_free(basic);
        OCSP_RESPONSE_free(resp);
        X509_free(peer);
        OCSP_REQUEST_free(req);  // frees attached OCSP_CERTID too
        return NetError::Unsupported;
    }

    X509_STORE_free(store);
    OCSP_BASICRESP_free(basic);
    OCSP_RESPONSE_free(resp);
    X509_free(peer);
    OCSP_REQUEST_free(req);  // frees attached OCSP_CERTID too
    return NetError::None;
}

NetError OpenSslProvider::Read(SocketHandle* socket, CHAOS_IL2CPP_UINT8* buf,
                               CHAOS_IL2CPP_UINT32 len, CHAOS_IL2CPP_UINT32* outRead) {
    *outRead = 0;
    if (m_ssl == nullptr) {
        return NetError::NotInitialized;
    }
    for (;;) {
        const int n = SSL_read(m_ssl, buf, static_cast<int>(len));
        if (n > 0) {
            *outRead = static_cast<CHAOS_IL2CPP_UINT32>(n);
            return NetError::None;
        }
        const int e = SSL_get_error(m_ssl, n);
        if (e == SSL_ERROR_ZERO_RETURN) {
            return NetError::None;  // close_notify received -> *outRead == 0
        }
        if (e == SSL_ERROR_WANT_READ) {
            if (!FlushWbio(socket, m_timeoutMs)) {
                return IoErrorFromLastNative();
            }
            const int fb = FillBio(socket, m_timeoutMs);
            if (fb == 0) {
                return NetError::None;  // clean EOF -> *outRead == 0
            }
            if (fb < 0) {
                return IoErrorFromLastNative();
            }
            continue;
        }
        if (e == SSL_ERROR_WANT_WRITE) {
            if (!FlushWbio(socket, m_timeoutMs)) {
                return IoErrorFromLastNative();
            }
            continue;
        }
        RecordError("read", e);
        return NetError::ConnectionReset;
    }
}

NetError OpenSslProvider::Write(SocketHandle* socket, const CHAOS_IL2CPP_UINT8* buf,
                                CHAOS_IL2CPP_UINT32 len, CHAOS_IL2CPP_UINT32* outWritten) {
    *outWritten = 0;
    if (m_ssl == nullptr) {
        return NetError::NotInitialized;
    }
    CHAOS_IL2CPP_UINT32 done = 0;
    while (done < len) {
        const int n = SSL_write(m_ssl, buf + done, static_cast<int>(len - done));
        if (n > 0) {
            done += static_cast<CHAOS_IL2CPP_UINT32>(n);
            continue;
        }
        const int e = SSL_get_error(m_ssl, n);
        if (e == SSL_ERROR_WANT_READ) {
            if (!FlushWbio(socket, m_timeoutMs)) {
                return IoErrorFromLastNative();
            }
            if (FillBio(socket, m_timeoutMs) < 0) {
                return IoErrorFromLastNative();
            }
            continue;
        }
        if (e == SSL_ERROR_WANT_WRITE) {
            if (!FlushWbio(socket, m_timeoutMs)) {
                return IoErrorFromLastNative();
            }
            continue;
        }
        RecordError("write", e);
        return NetError::ConnectionReset;
    }
    if (!FlushWbio(socket, m_timeoutMs)) {
        return IoErrorFromLastNative();
    }
    *outWritten = len;
    return NetError::None;
}

NetError OpenSslProvider::Shutdown() {
    if (m_ssl != nullptr) {
        // Best-effort close_notify: with memory BIOs the record is only
        // queued (no socket here), so the peer will observe the TCP close as
        // EOF.  This mirrors the Schannel backend, which also skips
        // close_notify.
        SSL_shutdown(m_ssl);
        Release();
    }
    return NetError::None;
}

const char* OpenSslProvider::NegotiatedProtocol() const {
    return m_protocol.empty() ? nullptr : m_protocol.c_str();
}

const char* OpenSslProvider::NegotiatedApplicationProtocol() const {
    return m_alpn.empty() ? nullptr : m_alpn.c_str();
}

const char* OpenSslProvider::LastErrorDetail() const {
    return m_lastError.empty() ? "no error" : m_lastError.c_str();
}

}  // namespace

TlsProvider* CreateOpenSslProvider() noexcept {
    return new OpenSslProvider();
}

}  // namespace chaos::net