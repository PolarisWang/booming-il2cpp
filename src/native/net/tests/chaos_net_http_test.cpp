// Layer G -- native HTTP client test matrix (NT-13).
//
// Compiles against the same per-platform source set as the TLS test:
//   Windows : Schannel TLS + CryptoAPI self-signed cert
//   POSIX   : OpenSSL TLS + OpenSSL cert
// plus http_impl.cpp (the unit under test).
//
// A loopback HTTP server (raw chaos_net sockets; TLS variant wraps the
// accepted socket in a server-mode TlsProvider) serves canned responses
// that pin every framing path of HttpSend:
//   GET  /hello     -> 200, Content-Length body "Hello, Chaos HTTP!"
//   GET  /chunked   -> 200, Transfer-Encoding: chunked {chunk-1, chunk-22}
//   POST /echo      -> 200, body echoes the request body
//   HEAD /hello     -> 200, Content-Length 18, no body (client skips)
//   GET  /status404 -> 404, "not-found"
//   GET  /close     -> 200, no Content-Length (read-to-close path)
//   GET  /nosuch    -> fallback 404
// Connection reuse is asserted via the server's accept counter: two
// sequential GETs to the same host must ride one pooled connection, and
// HttpPoolClear() must force a new one.  https:// exercises the full
// DNS+connect+TLS handshake+HTTP stack with the certificate-validation
// test hook.
#include <chaos/net/net_api.h>
#include <chaos/net/tls.h>
#include <chaos/net/http.h>

#include <atomic>
#include <chrono>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <string>
#include <thread>
#include <vector>

using chaos::net::NetError;
using chaos::net::NetAddress;
using chaos::net::SocketHandle;
using chaos::net::TlsProvider;
using chaos::net::TlsOptions;
using chaos::net::TlsCertificate;
using chaos::net::TlsMode;
using chaos::net::LoopbackV4;
using chaos::net::NetStartup;
using chaos::net::NetCleanup;
using chaos::net::NetSocketCreate;
using chaos::net::NetSocketBind;
using chaos::net::NetSocketListen;
using chaos::net::NetSocketAccept;
using chaos::net::NetSocketConnect;
using chaos::net::NetSocketGetLocalAddress;
using chaos::net::NetSocketSend;
using chaos::net::NetSocketRecv;
using chaos::net::NetSocketClose;
using chaos::net::HttpSend;
using chaos::net::HttpFreeResponse;
using chaos::net::HttpResponse;
using chaos::net::HttpRequest;
using chaos::net::HttpMethod;
using chaos::net::HttpPoolClear;
#ifdef _WIN32
using chaos::net::CreateSchannelProvider;
#else
using chaos::net::CreateOpenSslProvider;
#endif

static std::atomic<int> g_httpConns{0};
static std::atomic<int> g_httpsConns{0};
static std::atomic<int> g_failures{0};
static std::atomic<bool> g_stop{false};

#define HTTP_FAIL(...)                                       \
    do {                                                     \
        std::printf("[NET-HTTP-TEST] FAIL %s:%d ", __FILE__, \
                    __LINE__);                               \
        std::printf(__VA_ARGS__);                            \
        std::printf("\n");                                   \
        g_failures.fetch_add(1);                             \
    } while (0)

#define HTTP_CHECK(cond, ...)                                \
    do {                                                     \
        if (!(cond)) {                                       \
            HTTP_FAIL(__VA_ARGS__);                          \
        }                                                    \
    } while (0)

// ── self-signed cert (same structure as chaos_net_tls_test.cpp) ────────
#ifdef _WIN32
#ifndef SECURITY_WIN32
#define SECURITY_WIN32 1
#endif
#include <windows.h>
#include <wincrypt.h>
#include <iostream>

#undef TLS_CERT_FAIL
#define TLS_CERT_FAIL(step)                            \
    do {                                                    \
        std::printf("[NET-HTTP-TEST] GenerateSelfSignedCert %s failed (0x%08lX)\n", \
                    step, (unsigned long)::GetLastError()); \
        return false;                                       \
    } while (0)

bool GenerateSelfSignedCert(TlsCertificate* out) {
    wchar_t container[64];
    std::swprintf(container, 64, L"chaos-http-test-%lu", (unsigned long)GetCurrentProcessId());

    HCRYPTPROV hProv = 0;
    if (!CryptAcquireContextW(&hProv, container, MS_ENH_RSA_AES_PROV_W, PROV_RSA_AES,
                              CRYPT_NEWKEYSET)) {
        if (GetLastError() == NTE_EXISTS) {
            if (!CryptAcquireContextW(&hProv, container, MS_ENH_RSA_AES_PROV_W,
                                      PROV_RSA_AES, 0)) {
                TLS_CERT_FAIL("CryptAcquireContext");
            }
        } else {
            TLS_CERT_FAIL("CryptAcquireContext(retry)");
        }
    }

    HCRYPTKEY hKey = 0;
    if (!CryptGenKey(hProv, AT_KEYEXCHANGE, CRYPT_EXPORTABLE | 0x08000000 /*2048 bit*/, &hKey)) {
        CryptReleaseContext(hProv, 0);
        TLS_CERT_FAIL("CryptGenKey");
    }

    DWORD pubKeyInfoLen = 0;
    CryptExportPublicKeyInfoEx(hProv, AT_KEYEXCHANGE, X509_ASN_ENCODING,
                               szOID_RSA_RSA, 0, nullptr, nullptr,
                               &pubKeyInfoLen);
    std::vector<BYTE> pubKeyInfo(pubKeyInfoLen);
    if (!CryptExportPublicKeyInfoEx(hProv, AT_KEYEXCHANGE, X509_ASN_ENCODING,
                                    szOID_RSA_RSA, 0, nullptr,
                                    (CERT_PUBLIC_KEY_INFO*)pubKeyInfo.data(),
                                    &pubKeyInfoLen)) {
        TLS_CERT_FAIL("CryptExportPublicKeyInfoEx");
    }
    CERT_PUBLIC_KEY_INFO* pPubInfo = (CERT_PUBLIC_KEY_INFO*)pubKeyInfo.data();

    const char* cn = "chaos-http-test";
    CERT_RDN_ATTR attr = {};
    attr.pszObjId = szOID_COMMON_NAME;
    attr.dwValueType = CERT_RDN_UTF8_STRING;
    attr.Value.cbData = (DWORD)std::strlen(cn) + 1;
    attr.Value.pbData = (BYTE*)cn;
    CERT_RDN rdn = {};
    rdn.cRDNAttr = 1;
    rdn.rgRDNAttr = &attr;
    CERT_NAME_INFO nameInfo = {};
    nameInfo.cRDN = 1;
    nameInfo.rgRDN = &rdn;
    DWORD nameLen = 0;
    if (!CryptEncodeObject(X509_ASN_ENCODING, X509_NAME, &nameInfo, nullptr, &nameLen)) {
        TLS_CERT_FAIL("CryptEncodeObject X509_NAME");
    }
    std::vector<BYTE> nameEnc(nameLen);
    CryptEncodeObject(X509_ASN_ENCODING, X509_NAME, &nameInfo, nameEnc.data(), &nameLen);

    SYSTEMTIME stNow = {};
    GetSystemTime(&stNow);
    FILETIME ftNow = {};
    SystemTimeToFileTime(&stNow, &ftNow);
    ULARGE_INTEGER ul = {};
    ul.LowPart = ftNow.dwLowDateTime;
    ul.HighPart = ftNow.dwHighDateTime;
    const ULONGLONG ticksSec = 10000000ULL;
    ULONGLONG t = ul.QuadPart;
    ul.QuadPart = t - 24ULL * 3600ULL * ticksSec;
    FILETIME notBefore = {};
    notBefore.dwLowDateTime = ul.LowPart;
    notBefore.dwHighDateTime = ul.HighPart;
    ul.QuadPart = t + 365ULL * 24ULL * 3600ULL * ticksSec;
    FILETIME notAfter = {};
    notAfter.dwLowDateTime = ul.LowPart;
    notAfter.dwHighDateTime = ul.HighPart;

    BYTE serial[8] = {1, 2, 3, 4, 5, 6, 7, 8};
    CERT_INFO ci = {};
    ci.dwVersion = CERT_V1;
    ci.SerialNumber.cbData = sizeof(serial);
    ci.SerialNumber.pbData = serial;
    ci.SignatureAlgorithm.pszObjId = szOID_RSA_SHA256RSA;
    ci.Issuer.cbData = nameLen;
    ci.Issuer.pbData = nameEnc.data();
    ci.NotBefore = notBefore;
    ci.NotAfter = notAfter;
    ci.Subject = ci.Issuer;
    ci.SubjectPublicKeyInfo = *pPubInfo;

    DWORD encLen = 0;
    if (!CryptSignAndEncodeCertificate(hProv, AT_KEYEXCHANGE, X509_ASN_ENCODING,
                                       X509_CERT_TO_BE_SIGNED, &ci, &ci.SignatureAlgorithm,
                                       nullptr, nullptr, &encLen)) {
        TLS_CERT_FAIL("CryptSignAndEncode(1-pass)");
    }
    std::vector<BYTE> enc(encLen);
    CryptSignAndEncodeCertificate(hProv, AT_KEYEXCHANGE, X509_ASN_ENCODING,
                                  X509_CERT_TO_BE_SIGNED, &ci, &ci.SignatureAlgorithm,
                                  nullptr, enc.data(), &encLen);

    PCCERT_CONTEXT ctx = CertCreateCertificateContext(X509_ASN_ENCODING, enc.data(), encLen);
    if (ctx == nullptr) {
        TLS_CERT_FAIL("CryptSignAndEncode(2-pass)");
    }
    CRYPT_KEY_PROV_INFO kpi = {};
    kpi.pwszContainerName = container;
    kpi.pwszProvName = MS_ENH_RSA_AES_PROV_W;
    kpi.dwProvType = PROV_RSA_AES;
    kpi.dwKeySpec = AT_KEYEXCHANGE;
    if (!CertSetCertificateContextProperty(ctx, CERT_KEY_PROV_INFO_PROP_ID, 0, &kpi)) {
        CertFreeCertificateContext(ctx);
        TLS_CERT_FAIL("CertCreateCertificateContext");
    }
    out->native1 = (void*)ctx;
    return true;
}

void FreeSelfSignedCert(TlsCertificate* cert) {
    if (cert->native1 != nullptr) {
        CertFreeCertificateContext((PCCERT_CONTEXT)cert->native1);
        cert->native1 = nullptr;
    }
}

#else  // OpenSSL

#include <openssl/evp.h>
#include <openssl/rsa.h>
#include <openssl/x509.h>
#include <openssl/pem.h>

bool GenerateSelfSignedCert(TlsCertificate* out) {
    EVP_PKEY* pkey = nullptr;
    EVP_PKEY_CTX* pctx = EVP_PKEY_CTX_new_id(EVP_PKEY_RSA, nullptr);
    if (pctx == nullptr) {
        return false;
    }
    bool ok = EVP_PKEY_keygen_init(pctx) > 0 &&
              EVP_PKEY_CTX_set_rsa_keygen_bits(pctx, 2048) > 0 &&
              EVP_PKEY_keygen(pctx, &pkey) > 0;
    EVP_PKEY_CTX_free(pctx);
    if (!ok) {
        return false;
    }

    X509* x = X509_new();
    if (x == nullptr) {
        EVP_PKEY_free(pkey);
        return false;
    }
    X509_set_version(x, 2);
    ASN1_INTEGER_set(X509_get_serialNumber(x), 0x1234L);
    X509_gmtime_adj(X509_get_notBefore(x), -60);
    X509_gmtime_adj(X509_get_notAfter(x), 60L * 60L * 24L * 365L);
    X509_set_pubkey(x, pkey);

    X509_NAME* name = X509_get_subject_name(x);
    X509_NAME_add_entry_by_txt(name, "CN", MBSTRING_ASC,
                               (const unsigned char*)"chaos-http-test", -1, -1, 0);
    X509_set_issuer_name(x, name);
    if (X509_sign(x, pkey, EVP_sha256()) <= 0) {
        X509_free(x);
        EVP_PKEY_free(pkey);
        return false;
    }
    out->native1 = (void*)x;
    out->native2 = (void*)pkey;
    return true;
}

void FreeSelfSignedCert(TlsCertificate* cert) {
    if (cert->native1 != nullptr) {
        X509_free((X509*)cert->native1);
        cert->native1 = nullptr;
    }
    if (cert->native2 != nullptr) {
        EVP_PKEY_free((EVP_PKEY*)cert->native2);
        cert->native2 = nullptr;
    }
}

#endif

// ── loopback test server ───────────────────────────────────────────────
// Serves one accepted connection: reads requests until the client closes.
// Every response is canned; framing is produced per the route table above.
namespace {

struct ServerStream {
    SocketHandle sock;
    TlsProvider* tls = nullptr;      // non-null when TLS-wrapped
    std::vector<unsigned char> buf;  // read buffer
    size_t pos = 0;

    NetError Fill(int timeoutMs) {
        if (pos < buf.size()) return NetError::None;
        buf.resize(16384, 0);
        if (tls) {
            unsigned int got = 0;
            NetError e = tls->Read(&sock, buf.data(),
                                   static_cast<unsigned int>(buf.size()), &got);
            buf.resize(got);
            if (e != NetError::None) return e;
            if (got == 0) return NetError::ConnectionReset;
            return NetError::None;
        }
        int got = 0;
        NetError e = NetSocketRecv(&sock, buf.data(),
                                   static_cast<int>(buf.size()), 0, &got,
                                   timeoutMs);
        buf.resize(static_cast<size_t>(got));
        if (e != NetError::None) return e;
        if (got == 0) return NetError::ConnectionReset;
        return NetError::None;
    }

    NetError ReadLine(std::string* out, int timeoutMs) {
        for (;;) {
            for (size_t i = pos; i + 1 < buf.size(); ++i) {
                if (buf[i] == '\r' && buf[i + 1] == '\n') {
                    out->assign(reinterpret_cast<const char*>(buf.data()) + pos,
                                i - pos);
                    pos = i + 2;
                    return NetError::None;
                }
            }
            if (pos > 0 && pos == buf.size()) {
                // consumed everything; compact buffer for the next Fill
                buf.erase(buf.begin(), buf.begin() + static_cast<ptrdiff_t>(pos));
                pos = 0;
            }
            NetError e = Fill(timeoutMs);
            if (e != NetError::None) return e;
            if (buf.empty()) return NetError::ConnectionReset;
        }
    }

    NetError ReadBytes(unsigned char* dst, size_t n, int timeoutMs) {
        size_t done = 0;
        while (done < n) {
            if (pos >= buf.size()) {
                if (pos > 0) {
                    buf.erase(buf.begin(), buf.begin() + static_cast<ptrdiff_t>(pos));
                    pos = 0;
                }
                NetError e = Fill(timeoutMs);
                if (e != NetError::None) return e;
            }
            size_t avail = buf.size() - pos;
            size_t take = avail < (n - done) ? avail : (n - done);
            std::memcpy(dst + done, buf.data() + pos, take);
            pos += take;
            done += take;
        }
        return NetError::None;
    }

    NetError SendAll(const char* data, size_t n, int timeoutMs) {
        size_t done = 0;
        while (done < n) {
            if (tls) {
                unsigned int wrote = 0;
                NetError e = tls->Write(&sock,
                    reinterpret_cast<const unsigned char*>(data) + done,
                    static_cast<unsigned int>(n - done), &wrote);
                if (e != NetError::None) return e;
                if (wrote == 0) return NetError::ConnectionReset;
                done += wrote;
            } else {
                int sent = 0;
                NetError e = NetSocketSend(&sock,
                    reinterpret_cast<const unsigned char*>(data) + done,
                    static_cast<int>(n - done), 0, &sent, timeoutMs);
                if (e != NetError::None) return e;
                if (sent <= 0) return NetError::ConnectionReset;
                done += static_cast<size_t>(sent);
            }
        }
        return NetError::None;
    }
};

const char kHelloBody[] = "Hello, Chaos HTTP!";
const char kChunkedWire[] =
    "HTTP/1.1 200 OK\r\n"
    "Transfer-Encoding: chunked\r\n"
    "Connection: keep-alive\r\n"
    "\r\n"
    "7\r\nchunk-1\r\n"
    "8\r\nchunk-22\r\n"
    "0\r\n"
    "\r\n";
const char kDecodedChunked[] = "chunk-1chunk-22";
const char kNotFoundBody[] = "not-found";

void ServeConnection(ServerStream* s) {
    for (;;) {
        std::string reqLine;
        NetError e = s->ReadLine(&reqLine, 10000);
        if (e != NetError::None) return;  // client closed or error

        // parse "METHOD /path HTTP/1.1"
        std::string method, path;
        size_t sp1 = reqLine.find(' ');
        if (sp1 == std::string::npos) return;
        method = reqLine.substr(0, sp1);
        size_t sp2 = reqLine.find(' ', sp1 + 1);
        if (sp2 == std::string::npos) return;
        path = reqLine.substr(sp1 + 1, sp2 - sp1 - 1);

        // headers
        long contentLength = -1;
        bool connectionClose = false;
        for (;;) {
            std::string hdr;
            e = s->ReadLine(&hdr, 10000);
            if (e != NetError::None) return;
            if (hdr.empty()) break;
            size_t colon = hdr.find(':');
            if (colon == std::string::npos) continue;
            std::string hname = hdr.substr(0, colon);
            std::string hval = hdr.substr(colon + 1);
            while (!hval.empty() && (hval[0] == ' ' || hval[0] == '\t'))
                hval.erase(hval.begin());
            if (hname == "Content-Length") {
                contentLength = std::strtol(hval.c_str(), nullptr, 10);
            } else if (hname == "Connection") {
                if (hval == "close") connectionClose = true;
            }
        }
        // body
        std::vector<unsigned char> reqBody;
        if (contentLength > 0) {
            reqBody.resize(static_cast<size_t>(contentLength));
            e = s->ReadBytes(reqBody.data(), static_cast<size_t>(contentLength),
                             10000);
            if (e != NetError::None) return;
        }

        // route
        std::string resp;
        bool closeAfter = connectionClose;
        if (path == "/hello") {
            resp = std::string("HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n") +
                   "Content-Length: " + std::to_string(sizeof(kHelloBody) - 1) +
                   "\r\nConnection: keep-alive\r\n\r\n" + kHelloBody;
        } else if (path == "/chunked") {
            resp = kChunkedWire;
        } else if (path == "/echo") {
            resp = std::string("HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n") +
                   "Content-Length: " + std::to_string(reqBody.size()) +
                   "\r\nConnection: keep-alive\r\n\r\n" +
                   std::string(reinterpret_cast<const char*>(reqBody.data()),
                               reqBody.size());
        } else if (path == "/status404") {
            resp = std::string("HTTP/1.1 404 Not Found\r\nContent-Type: text/plain\r\n") +
                   "Content-Length: " + std::to_string(sizeof(kNotFoundBody) - 1) +
                   "\r\nConnection: keep-alive\r\n\r\n" + kNotFoundBody;
        } else if (path == "/close") {
            // no Content-Length -> client must read until connection close
            resp = std::string("HTTP/1.1 200 OK\r\nContent-Type: text/plain\r\n") +
                   "Connection: close\r\n\r\nclosed-body";
            closeAfter = true;
        } else {
            resp = std::string("HTTP/1.1 404 Not Found\r\nContent-Type: text/plain\r\n") +
                   "Content-Length: " + std::to_string(sizeof(kNotFoundBody) - 1) +
                   "\r\nConnection: keep-alive\r\n\r\n" + kNotFoundBody;
        }

        // HEAD: content-length delivered, no entity body
        if (method == "HEAD") {
            size_t bodyAt = resp.find("\r\n\r\n");
            if (bodyAt != std::string::npos) {
                resp = resp.substr(0, bodyAt + 4);
            }
        }

        e = s->SendAll(resp.data(), resp.size(), 10000);
        if (e != NetError::None) return;
        if (closeAfter) return;
    }
}

void RunHttpServer(int* portOut) {
    SocketHandle listener{};
    NetAddress addr = LoopbackV4(0);
    NetError e = NetSocketCreate(chaos::net::kAddressFamilyInet,
                                 chaos::net::kSocketTypeStream,
                                 chaos::net::kProtocolTcp, &listener);
    if (e != NetError::None) { *portOut = -1; return; }
    if (NetSocketBind(&listener, addr) != NetError::None) { *portOut = -1; return; }
    if (NetSocketListen(&listener, 8) != NetError::None) { *portOut = -1; return; }
    NetAddress local{};
    if (NetSocketGetLocalAddress(&listener, &local) != NetError::None) {
        *portOut = -1;
        return;
    }
    *portOut = local.port;

    while (!g_stop.load()) {
        SocketHandle peer{};
        NetError ae = NetSocketAccept(&listener, &peer, nullptr, 200);
        if (ae != NetError::None) {
            if (ae == NetError::ConnectionReset) continue;
            if (ae == NetError::TimedOut) continue;
            return;
        }
        g_httpConns.fetch_add(1);
        ServerStream s;
        s.sock = peer;
        ServeConnection(&s);
        NetSocketClose(&peer);
    }
}

void RunHttpsServer(const TlsCertificate& cert, int* portOut) {
    SocketHandle listener{};
    NetAddress addr = LoopbackV4(0);
    NetError e = NetSocketCreate(chaos::net::kAddressFamilyInet,
                                 chaos::net::kSocketTypeStream,
                                 chaos::net::kProtocolTcp, &listener);
    if (e != NetError::None) { *portOut = -1; return; }
    if (NetSocketBind(&listener, addr) != NetError::None) { *portOut = -1; return; }
    if (NetSocketListen(&listener, 8) != NetError::None) { *portOut = -1; return; }
    NetAddress local{};
    if (NetSocketGetLocalAddress(&listener, &local) != NetError::None) {
        *portOut = -1;
        return;
    }
    *portOut = local.port;

    while (!g_stop.load()) {
        SocketHandle peer{};
        NetError ae = NetSocketAccept(&listener, &peer, nullptr, 200);
        if (ae != NetError::None) {
            if (ae == NetError::ConnectionReset) continue;
            if (ae == NetError::TimedOut) continue;
            return;
        }
        g_httpsConns.fetch_add(1);
#ifdef _WIN32
        TlsProvider* prov = CreateSchannelProvider();
#else
        TlsProvider* prov = CreateOpenSslProvider();
#endif
        bool ok = prov != nullptr;
        if (ok) {
            TlsOptions opts;
            opts.mode = TlsMode::Server;
            opts.serverCertificate = cert;
            opts.timeoutMs = 10000;
            ok = prov->Create(opts) == NetError::None &&
                 prov->Handshake(&peer) == NetError::None;
        }
        if (ok) {
            ServerStream s;
            s.sock = peer;
            s.tls = prov;
            ServeConnection(&s);
        }
        if (prov) prov->Shutdown();
        delete prov;
        NetSocketClose(&peer);
    }
}

// ── client-side helpers ────────────────────────────────────────────────
HttpResponse SendOnce(const char* url, HttpMethod method = HttpMethod::Get,
                      const char* body = nullptr, int bodyLen = 0) {
    HttpRequest req;
    req.method = method;
    req.url = url;
    req.body = reinterpret_cast<const unsigned char*>(body);
    req.bodyLength = bodyLen;
    req.disableCertificateValidation = true;
    HttpResponse resp{};
    NetError e = HttpSend(req, &resp);
    HTTP_CHECK(e == NetError::None, "HttpSend %s failed err=%d", url,
               (int)e);
    return resp;
}

void CheckBody(const HttpResponse& resp, const char* expected, size_t len,
               const char* label) {
    HTTP_CHECK(resp.bodyLength == static_cast<int>(len),
               "%s: bodyLen=%d expected %zu", label, resp.bodyLength, len);
    if (resp.bodyLength == static_cast<int>(len) && len > 0) {
        HTTP_CHECK(std::memcmp(resp.body, expected, len) == 0,
                   "%s: body mismatch", label);
    }
}

}  // namespace

int main() {
    std::setvbuf(stdout, nullptr, _IONBF, 0);
    if (NetStartup() != NetError::None) {
        std::printf("[NET-HTTP-TEST] FAIL NetStartup\n");
        return 1;
    }

    TlsCertificate cert{};
    if (!GenerateSelfSignedCert(&cert)) {
        std::printf("[NET-HTTP-TEST] FAIL GenerateSelfSignedCert\n");
        NetCleanup();
        return 1;
    }

    int httpPort = 0;
    int httpsPort = 0;
    std::thread httpServer(RunHttpServer, &httpPort);
    std::thread httpsServer(RunHttpsServer, std::cref(cert), &httpsPort);
    // wait for both listeners
    for (int i = 0; i < 200 && (httpPort == 0 || httpsPort == 0); ++i) {
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    HTTP_CHECK(httpPort > 0, "http listener port=%d", httpPort);
    HTTP_CHECK(httpsPort > 0, "https listener port=%d", httpsPort);

    std::string httpBase = "http://127.0.0.1:" + std::to_string(httpPort);
    std::string httpsBase = "https://127.0.0.1:" + std::to_string(httpsPort);

    // 1. GET /hello — Content-Length framing
    {
        HttpResponse r = SendOnce((httpBase + "/hello").c_str());
        HTTP_CHECK(r.statusCode == 200, "GET /hello status=%d", r.statusCode);
        CheckBody(r, kHelloBody, sizeof(kHelloBody) - 1, "GET /hello");
        HttpFreeResponse(&r);
    }

    // 2. GET /chunked — chunked decoding
    {
        HttpResponse r = SendOnce((httpBase + "/chunked").c_str());
        HTTP_CHECK(r.statusCode == 200, "GET /chunked status=%d", r.statusCode);
        CheckBody(r, kDecodedChunked, sizeof(kDecodedChunked) - 1,
                  "GET /chunked");
        HttpFreeResponse(&r);
    }

    // 3. POST /echo — request + response body
    {
        const char* payload = "ping-123";
        HttpResponse r = SendOnce((httpBase + "/echo").c_str(),
                                  HttpMethod::Post, payload,
                                  (int)std::strlen(payload));
        HTTP_CHECK(r.statusCode == 200, "POST /echo status=%d", r.statusCode);
        CheckBody(r, payload, std::strlen(payload), "POST /echo");
        HttpFreeResponse(&r);
    }

    // 4. HEAD /hello — no entity body
    {
        HttpResponse r = SendOnce((httpBase + "/hello").c_str(),
                                  HttpMethod::Head);
        HTTP_CHECK(r.statusCode == 200, "HEAD /hello status=%d", r.statusCode);
        HTTP_CHECK(r.bodyLength == 0, "HEAD /hello bodyLen=%d", r.bodyLength);
        HttpFreeResponse(&r);
    }

    // 5. GET /status404 — non-2xx status + body
    {
        HttpResponse r = SendOnce((httpBase + "/status404").c_str());
        HTTP_CHECK(r.statusCode == 404, "GET /status404 status=%d",
                   r.statusCode);
        CheckBody(r, kNotFoundBody, sizeof(kNotFoundBody) - 1,
                  "GET /status404");
        HttpFreeResponse(&r);
    }

    // 6. keep-alive pool: five requests above used ONE connection
    HTTP_CHECK(g_httpConns.load() == 1,
               "pool reuse: conns=%d expected 1", g_httpConns.load());

    // 7. HttpPoolClear forces a new connection
    {
        HttpPoolClear();
        HttpResponse r = SendOnce((httpBase + "/hello").c_str());
        HTTP_CHECK(r.statusCode == 200, "GET after clear status=%d",
                   r.statusCode);
        HTTP_CHECK(g_httpConns.load() == 2,
                   "pool clear: conns=%d expected 2", g_httpConns.load());
        HttpFreeResponse(&r);
    }

    // 8. /close — read-to-close framing, connection not pooled
    {
        // clear the pool so the read-to-close request opens a fresh
        // connection (verifies the framing on a new connection, not over a
        // reused keep-alive one)
        HttpPoolClear();
        HttpResponse r = SendOnce((httpBase + "/close").c_str());
        HTTP_CHECK(r.statusCode == 200, "GET /close status=%d", r.statusCode);
        CheckBody(r, "closed-body", 11, "GET /close");
        HttpFreeResponse(&r);
        HTTP_CHECK(g_httpConns.load() == 3,
                   "read-to-close: conns=%d expected 3", g_httpConns.load());
    }

    // 9. https:// — full TLS stack over HTTP, pool per scheme
    {
        int connsBefore = g_httpsConns.load();
        HttpResponse r = SendOnce((httpsBase + "/hello").c_str());
        HTTP_CHECK(r.statusCode == 200, "GET https /hello status=%d",
                   r.statusCode);
        CheckBody(r, kHelloBody, sizeof(kHelloBody) - 1, "GET https /hello");
        HttpFreeResponse(&r);

        HttpResponse r2 = SendOnce((httpsBase + "/chunked").c_str());
        HTTP_CHECK(r2.statusCode == 200, "GET https /chunked status=%d",
                   r2.statusCode);
        CheckBody(r2, kDecodedChunked, sizeof(kDecodedChunked) - 1,
                  "GET https /chunked");
        HttpFreeResponse(&r2);

        HTTP_CHECK(g_httpsConns.load() == connsBefore + 1,
                   "https pool: httpsConns=%d expected %d",
                   g_httpsConns.load(), connsBefore + 1);
    }

    // 10. 404 from a path with no route — pooling still intact afterwards
    {
        HttpResponse r = SendOnce((httpBase + "/nosuch").c_str());
        HTTP_CHECK(r.statusCode == 404, "GET /nosuch status=%d", r.statusCode);
        HttpFreeResponse(&r);
        HttpResponse r2 = SendOnce((httpBase + "/hello").c_str());
        HTTP_CHECK(r2.statusCode == 200, "GET after 404 status=%d",
                   r2.statusCode);
        HttpFreeResponse(&r2);
    }

    // signal servers to stop and join before tearing down the transport
    HttpPoolClear();
    g_stop.store(true);
    httpServer.join();
    httpsServer.join();

    FreeSelfSignedCert(&cert);
    NetCleanup();

    if (g_failures.load() == 0) {
        std::printf("[NET-HTTP-TEST] ALL PASS (http conns=%d https conns=%d)\n",
                    g_httpConns.load(), g_httpsConns.load());
        return 0;
    }
    std::printf("[NET-HTTP-TEST] FAILED: %d failure(s)\n", g_failures.load());
    return 1;
}
