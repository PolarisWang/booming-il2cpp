// NT-16 loopback test: native HTTP/2 (RFC 9113) client + server.
//
//   * h2c:// server -- plaintext HTTP/2 on loopback: connection preface,
//     SETTINGS exchange, HEADERS/DATA frames, HPACK static-table + literal
//     fields, multiplexed streams, flow control (client-side and
//     server-side WINDOW_UPDATE, triggered by a >64KiB body so the default
//     window is exhausted).
//   * h2://  server -- same over TLS (self-signed cert, client uses
//     disableCertificateValidation; ALPN negotiates "h2").
//
// plus standalone HPACK codec unit tests (NT-21: dynamic table, Huffman,
// index compression, eviction, never-indexed).
//
// Scenarios: GET /hello (200 + body + literal response header),
// POST /echo (body echo), GET /nope (404), POST /big (70KiB echo, forces
// flow-control waits on both directions), GET /hdr (HPACK literal request
// header round trip), interleaved multiplexed streams, TLS path.
//
// Dual platform: MSVC/Schannel + CryptoAPI on Windows, MSYS2/OpenSSL else.
#include <chaos/net/http2.h>
#include <chaos/net/hpack.h>
#include <chaos/net/tls.h>
#include <chaos/net/net_api.h>
#include <chaos/net/net.h>

#include <atomic>
#include <chrono>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <string>
#include <thread>
#include <vector>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <wincrypt.h>
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "secur32.lib")
#pragma comment(lib, "crypt32.lib")
#pragma comment(lib, "advapi32.lib")
#else
#include <openssl/evp.h>
#include <openssl/rsa.h>
#include <openssl/x509.h>
#include <openssl/pem.h>
#endif

using chaos::net::NetError;
using chaos::net::NetAddress;
using chaos::net::SocketHandle;
using chaos::net::TlsCertificate;
using chaos::net::LoopbackV4;
using chaos::net::NetStartup;
using chaos::net::NetCleanup;
using chaos::net::NetSocketCreate;
using chaos::net::NetSocketBind;
using chaos::net::NetSocketListen;
using chaos::net::NetSocketAccept;
using chaos::net::NetSocketGetLocalAddress;
using chaos::net::NetSocketClose;
using chaos::net::HttpMethod;
using chaos::net::HttpHeader;
using chaos::net::H2Client;
using chaos::net::H2Server;
using chaos::net::H2Request;
using chaos::net::H2Response;
using chaos::net::H2ServerRequest;
using chaos::net::H2Connect;
using chaos::net::H2BeginRequest;
using chaos::net::H2RecvResponse;
using chaos::net::H2FreeResponse;
using chaos::net::H2Free;
using chaos::net::H2ServerAccept;
using chaos::net::H2ServerNextRequest;
using chaos::net::H2ServerRespond;
using chaos::net::H2ServerFree;

namespace {

std::atomic<int> g_failures{0};

#define H2_FAIL(msg)                                                      \
    do {                                                                  \
        std::printf("[NET-HTTP2-TEST] FAIL %s:%d %s\n", __FILE__, __LINE__, msg); \
        ++g_failures;                                                     \
    } while (0)

#define H2_CHECK(cond, msg)                                               \
    do {                                                                  \
        if (!(cond)) H2_FAIL(msg);                                        \
    } while (0)

// ── self-signed cert (same structure as chaos_net_tls_test.cpp) ─────────
#ifdef _WIN32
bool GenerateSelfSignedCert(TlsCertificate* out) {
    wchar_t container[64];
    std::swprintf(container, 64, L"chaos-h2-test-%lu",
                  (unsigned long)GetCurrentProcessId());
    HCRYPTPROV hProv = 0;
    if (!CryptAcquireContextW(&hProv, container, MS_ENH_RSA_AES_PROV_W,
                              PROV_RSA_AES, CRYPT_NEWKEYSET)) {
        if (GetLastError() == NTE_EXISTS) {
            if (!CryptAcquireContextW(&hProv, container, MS_ENH_RSA_AES_PROV_W,
                                      PROV_RSA_AES, 0)) {
                std::printf("[NET-HTTP2-TEST] FAIL CryptAcquireContext\n");
                return false;
            }
        } else {
            std::printf("[NET-HTTP2-TEST] FAIL CryptAcquireContext(retry)\n");
            return false;
        }
    }
    HCRYPTKEY hKey = 0;
    if (!CryptGenKey(hProv, AT_KEYEXCHANGE, CRYPT_EXPORTABLE | 0x08000000, &hKey)) {
        CryptReleaseContext(hProv, 0);
        std::printf("[NET-HTTP2-TEST] FAIL CryptGenKey\n");
        return false;
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
        CryptReleaseContext(hProv, 0);
        std::printf("[NET-HTTP2-TEST] FAIL CryptExportPublicKeyInfoEx\n");
        return false;
    }
    CERT_PUBLIC_KEY_INFO* pPubInfo = (CERT_PUBLIC_KEY_INFO*)pubKeyInfo.data();

    const char* cn = "chaos-h2-test";
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
    CryptEncodeObject(X509_ASN_ENCODING, X509_NAME, &nameInfo, nullptr, &nameLen);
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

    BYTE serial[8] = {1, 2, 3, 4, 5, 6, 7, 9};
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
                                       X509_CERT_TO_BE_SIGNED, &ci,
                                       &ci.SignatureAlgorithm, nullptr, nullptr,
                                       &encLen)) {
        CryptReleaseContext(hProv, 0);
        std::printf("[NET-HTTP2-TEST] FAIL CryptSignAndEncode(1-pass)\n");
        return false;
    }
    std::vector<BYTE> enc(encLen);
    CryptSignAndEncodeCertificate(hProv, AT_KEYEXCHANGE, X509_ASN_ENCODING,
                                  X509_CERT_TO_BE_SIGNED, &ci,
                                  &ci.SignatureAlgorithm, nullptr, enc.data(),
                                  &encLen);
    PCCERT_CONTEXT ctx = CertCreateCertificateContext(X509_ASN_ENCODING,
                                                      enc.data(), encLen);
    if (ctx == nullptr) {
        CryptReleaseContext(hProv, 0);
        std::printf("[NET-HTTP2-TEST] FAIL CertCreateCertificateContext\n");
        return false;
    }
    CRYPT_KEY_PROV_INFO kpi = {};
    kpi.pwszContainerName = container;
    kpi.pwszProvName = MS_ENH_RSA_AES_PROV_W;
    kpi.dwProvType = PROV_RSA_AES;
    kpi.dwKeySpec = AT_KEYEXCHANGE;
    if (!CertSetCertificateContextProperty(ctx, CERT_KEY_PROV_INFO_PROP_ID,
                                           0, &kpi)) {
        CertFreeCertificateContext(ctx);
        CryptReleaseContext(hProv, 0);
        std::printf("[NET-HTTP2-TEST] FAIL CertSetCertProp\n");
        return false;
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
bool GenerateSelfSignedCert(TlsCertificate* out) {
    EVP_PKEY* pkey = nullptr;
    EVP_PKEY_CTX* pctx = EVP_PKEY_CTX_new_id(EVP_PKEY_RSA, nullptr);
    if (pctx == nullptr) return false;
    bool ok = EVP_PKEY_keygen_init(pctx) > 0 &&
              EVP_PKEY_CTX_set_rsa_keygen_bits(pctx, 2048) > 0 &&
              EVP_PKEY_keygen(pctx, &pkey) > 0;
    EVP_PKEY_CTX_free(pctx);
    if (!ok) return false;
    X509* x = X509_new();
    if (x == nullptr) {
        EVP_PKEY_free(pkey);
        return false;
    }
    X509_set_version(x, 2);
    ASN1_INTEGER_set(X509_get_serialNumber(x), 0x1235L);
    X509_gmtime_adj(X509_get_notBefore(x), -60);
    X509_gmtime_adj(X509_get_notAfter(x), 60L * 60L * 24L * 365L);
    X509_set_pubkey(x, pkey);
    X509_NAME* name = X509_get_subject_name(x);
    X509_NAME_add_entry_by_txt(name, "CN", MBSTRING_ASC,
                               (const unsigned char*)"chaos-h2-test", -1, -1, 0);
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

// ── H2 loopback server: accepts ONE connection, serves requests until the
// client hangs up.  tlsCert == nullptr -> h2c (plaintext).
void RunH2Server(std::atomic<int>* portOut, const TlsCertificate* tlsCert) {
    SocketHandle ls{};
    if (NetSocketCreate(chaos::net::kAddressFamilyInet,
                        chaos::net::kSocketTypeStream,
                        chaos::net::kProtocolTcp, &ls) != NetError::None) {
        portOut->store(-1);
        return;
    }
    if (NetSocketBind(&ls, LoopbackV4(0)) != NetError::None ||
        NetSocketListen(&ls, 8) != NetError::None) {
        portOut->store(-1);
        NetSocketClose(&ls);
        return;
    }
    NetAddress la{};
    if (NetSocketGetLocalAddress(&ls, &la) != NetError::None) {
        portOut->store(-1);
        NetSocketClose(&ls);
        return;
    }
    portOut->store(la.port);

    H2Server* hs = nullptr;
    NetError ae = H2ServerAccept(&ls, tlsCert, &hs);
    if (ae != NetError::None) {
        std::fprintf(stderr, "[HTTP2-SRV] H2ServerAccept failed: %d\n", (int)ae);
        NetSocketClose(&ls);
        return;
    }
    NetSocketClose(&ls);

    HttpHeader respHdr;
    respHdr.name = "x-server";
    respHdr.value = "h2test";

    for (;;) {
        H2ServerRequest r{};
        NetError ne = H2ServerNextRequest(hs, &r, 10000);
        if (ne != NetError::None) {
            std::fprintf(stderr, "[HTTP2-SRV] NextRequest failed: %d\n", (int)ne);
            break;  // client gone / timeout
        }
        std::string path = r.path ? r.path : "";
        CHAOS_IL2CPP_INT32 status = 200;
        std::string body;
        if (path == "/hello") {
            body = "Hello, Chaos H2!";
        } else if (path == "/nope") {
            status = 404;
            body = "not-found";
        } else if (path == "/echo" || path == "/big") {
            body.assign(reinterpret_cast<const char*>(r.body),
                        static_cast<std::size_t>(r.bodyLength));
        } else if (path == "/hdr") {
            const char* v = "";
            for (CHAOS_IL2CPP_INT32 i = 0; i < r.headerCount; ++i) {
                if (r.headers[i].name &&
                    std::strcmp(r.headers[i].name, "x-test") == 0) {
                    v = r.headers[i].value ? r.headers[i].value : "";
                    break;
                }
            }
            body = std::string("x-test:") + v;
        } else {
            status = 404;
            body = "not-found";
        }
        NetError e = H2ServerRespond(
            hs, r.streamId, status, &respHdr, 1,
            reinterpret_cast<const CHAOS_IL2CPP_UINT8*>(body.data()),
            static_cast<CHAOS_IL2CPP_INT32>(body.size()), 10000);
        if (e != NetError::None) {
            std::fprintf(stderr, "[HTTP2-SRV] Respond failed (sid=%d path=%s): %d\n",
                         (int)r.streamId, path.c_str(), (int)e);
            break;
        }
        std::fprintf(stderr, "[HTTP2-SRV] served %s sid=%d -> %d (%d bytes)\n",
                     path.c_str(), (int)r.streamId, (int)status, (int)body.size());
    }
    H2ServerFree(hs);
}

// find a header by name in a response; returns its value or "".
const char* FindHeader(const H2Response& r, const char* name) {
    for (CHAOS_IL2CPP_INT32 i = 0; i < r.headerCount; ++i) {
        if (r.headers[i].name && std::strcmp(r.headers[i].name, name) == 0)
            return r.headers[i].value ? r.headers[i].value : "";
    }
    return "";
}

bool WaitPorts(const std::atomic<int>& h2c, const std::atomic<int>& h2cMux,
               const std::atomic<int>& h2) {
    auto deadline = std::chrono::steady_clock::now() + std::chrono::seconds(15);
    while (std::chrono::steady_clock::now() < deadline) {
        if (h2c.load() != 0 && h2cMux.load() != 0 && h2.load() != 0)
            return h2c.load() > 0 && h2cMux.load() > 0 && h2.load() > 0;
        if (h2c.load() < 0 || h2cMux.load() < 0 || h2.load() < 0) return false;
        std::this_thread::sleep_for(std::chrono::milliseconds(2));
    }
    return false;
}


// ---- HPACK unit tests (NT-21): pure in-process codec checks, no sockets --
void RunHpackUnitTests() {
    namespace hpack = chaos::net::hpack;

    // (a) Huffman: RFC 7541 Appendix C decode vectors + round trip + EOS
    {
        struct V { const char* plain; const char* hex; };
        const V vecs[] = {
            {"www.example.com", "f1e3c2e5f23a6ba0ab90f4ff"},
            {"no-cache", "a8eb10649cbf"},
            {"custom-key", "25a849e95ba97d7f"},
            {"custom-value", "25a849e95bb8e8b4bf"},
        };
        for (const V& v : vecs) {
            std::vector<uint8_t> huff;
            hpack::HuffmanEncode(v.plain, &huff);
            char hexbuf[64] = {};
            std::size_t pos = 0;
            for (uint8_t b : huff) {
                std::snprintf(hexbuf + pos, sizeof(hexbuf) - pos, "%02x", b);
                pos += 2;
            }
            H2_CHECK(std::strcmp(hexbuf, v.hex) == 0, "huffman encode vector");
            std::string dec;
            H2_CHECK(hpack::HuffmanDecode(huff.data(), huff.size(), &dec) &&
                         dec == v.plain,
                     "huffman decode vector");
        }
        const std::string mixed =
            "GET /index.html HTTP/2.0 host:example.com x-custom:value123!";
        std::vector<uint8_t> huff;
        hpack::HuffmanEncode(mixed, &huff);
        std::string dec;
        H2_CHECK(hpack::HuffmanDecode(huff.data(), huff.size(), &dec) &&
                     dec == mixed,
                 "huffman roundtrip mixed");
        // a streampf all-ones (= EOS) must be rejected
        const uint8_t eos[4] = {0xFF, 0xFF, 0xFF, 0xFF};
        std::string eOut;
        H2_CHECK(!hpack::HuffmanDecode(eos, 4, &eOut), "huffman rejects EOS");
    }

    // (b) dynamic table: second identical field compresses to one indexed
    //     byte (0xBE = 0x80 | 62, overall index of dynamic entry 1)
    {
        hpack::Encoder enc;
        hpack::Decoder dec;
        std::vector<uint8_t> b1;
        enc.EncodeField(&b1, "x-prop", "alpha");
        std::vector<std::pair<std::string, std::string>> h1;
        H2_CHECK(dec.DecodeHeaderBlock(b1.data(), b1.size(), &h1) &&
                     h1.size() == 1 && h1[0].first == "x-prop" &&
                     h1[0].second == "alpha",
                 "hpack decode literal");
        H2_CHECK(enc.Table().Count() == 1 && dec.Table().Count() == 1,
                 "hpack tables populated after incremental");
        std::vector<uint8_t> b2;
        enc.EncodeField(&b2, "x-prop", "alpha");
        H2_CHECK(b2.size() == 1 && b2[0] == 0xBE,
                 "hpack second field is dynamic indexed 0xBE");
        std::vector<std::pair<std::string, std::string>> h2;
        H2_CHECK(dec.DecodeHeaderBlock(b2.data(), b2.size(), &h2) &&
                     h2.size() == 1 && h2[0].first == "x-prop" &&
                     h2[0].second == "alpha",
                 "hpack decode dynamic index");
    }

    // (c) capacity eviction: 3 x 32B entries in a 64B table -> newest 2
    {
        hpack::DynamicTable t(64);
        H2_CHECK(t.Insert("", "") && t.Insert("", "") && t.Insert("", ""),
                 "insert 3 empty entries");
        std::string n1, v1;
        H2_CHECK(t.Count() == 2 && t.Size() == 64 &&
                     t.Get(1, &n1, &v1) && t.Get(2, &n1, &v1) &&
                     !t.Get(3, &n1, &v1),
                 "eviction keeps newest 2 of 3");
        hpack::DynamicTable t2(64);
        const std::string big(40, 'x');
        H2_CHECK(!t2.Insert(big, big), "oversized entry rejected");
        H2_CHECK(t2.Count() == 0, "no insert when entry over capacity");
    }

    // (d) never-indexed (authorization) does not touch the dynamic table
    {
        hpack::Encoder enc;
        hpack::Decoder dec;
        std::vector<uint8_t> b;
        enc.EncodeField(&b, "authorization", "s3cr3t",
                        hpack::FieldMode::NeverIndexed);
        std::vector<std::pair<std::string, std::string>> h;
        H2_CHECK(dec.DecodeHeaderBlock(b.data(), b.size(), &h) &&
                     h.size() == 1 && h[0].first == "authorization" &&
                     h[0].second == "s3cr3t",
                 "never-indexed decode");
        H2_CHECK(dec.Table().Count() == 0, "never-indexed not inserted");
    }

    // (e) block integration: static indexed + literal + dynamic indexed +
    //     empty value fields round trip identically
    {
        hpack::Encoder enc;
        hpack::Decoder dec;
        const std::vector<std::pair<std::string, std::string>> fields = {
            {":method", "GET"},
            {":scheme", "https"},
            {":authority", "example.com:443"},
            {":path", "/hello"},
            {"x-prop", "alpha"},
            {"x-prop", "alpha"},
            {"x-empty", ""},
        };
        std::vector<uint8_t> block;
        for (const auto& f : fields) enc.EncodeField(&block, f.first, f.second);
        std::vector<std::pair<std::string, std::string>> out;
        H2_CHECK(dec.DecodeHeaderBlock(block.data(), block.size(), &out) &&
                     out.size() == fields.size() && out == fields,
                 "header block round trip");
    }
}
}  // namespace

int main() {
    std::setvbuf(stdout, nullptr, _IONBF, 0);
    RunHpackUnitTests();
    if (NetStartup() != NetError::None) {
        std::printf("[NET-HTTP2-TEST] NetStartup failed\n");
        return 1;
    }
    TlsCertificate cert{};
    if (!GenerateSelfSignedCert(&cert)) {
        std::printf("[NET-HTTP2-TEST] cert generation failed\n");
        NetCleanup();
        return 1;
    }

    std::atomic<int> h2cPort{0}, h2cMuxPort{0}, h2Port{0};
    std::thread h2cT(RunH2Server, &h2cPort, (const TlsCertificate*)nullptr);
    // the multiplexing scenario needs a SECOND connection on a fresh port
    std::thread h2cMuxT(RunH2Server, &h2cMuxPort, (const TlsCertificate*)nullptr);
    std::thread h2T(RunH2Server, &h2Port, &cert);

    if (!WaitPorts(h2cPort, h2cMuxPort, h2Port)) {
        std::printf("[NET-HTTP2-TEST] FAIL server ports never came up\n");
        ++g_failures;
    } else {
        // ----------------------------------------------------------------
        // 1) h2c GET /hello: 200 + body + literal response header
        // ----------------------------------------------------------------
        std::string h2cBase = "h2c://127.0.0.1:" + std::to_string(h2cPort.load());
        std::string urlHello = h2cBase + "/hello";
        H2Client* c = nullptr;
        H2Request req{};
        req.url = urlHello.c_str();
        req.timeoutMs = 10000;
        H2_CHECK(H2Connect(req, &c) == NetError::None, "h2c connect");
        if (c) {
            CHAOS_IL2CPP_INT32 sid = 0;
            H2_CHECK(H2BeginRequest(c, req, &sid) == NetError::None && sid == 1,
                     "h2c begin stream 1");
            H2Response resp{};
            H2_CHECK(H2RecvResponse(c, sid, &resp, 10000) == NetError::None,
                     "h2c recv /hello");
            H2_CHECK(resp.statusCode == 200, "h2c /hello status==200");
            H2_CHECK(resp.bodyLength == 16 &&
                         std::memcmp(resp.body, "Hello, Chaos H2!", 16) == 0,
                     "h2c /hello body");
            H2_CHECK(std::strcmp(FindHeader(resp, "x-server"), "h2test") == 0,
                     "h2c /hello literal header");
            H2FreeResponse(&resp);

            // 2) POST /echo: body echo
            const char* ping = "ping-h2";
            H2Request post{};
            post.method = HttpMethod::Post;
            std::string urlEcho = h2cBase + "/echo";
            post.url = urlEcho.c_str();
            post.body = reinterpret_cast<const CHAOS_IL2CPP_UINT8*>(ping);
            post.bodyLength = 7;
            post.timeoutMs = 10000;
            sid = 0;
            H2_CHECK(H2BeginRequest(c, post, &sid) == NetError::None && sid == 3,
                     "h2c begin stream 3");
            H2Response resp2{};
            H2_CHECK(H2RecvResponse(c, sid, &resp2, 10000) == NetError::None,
                     "h2c recv /echo");
            H2_CHECK(resp2.statusCode == 200 && resp2.bodyLength == 7 &&
                         std::memcmp(resp2.body, ping, 7) == 0,
                     "h2c /echo body");
            H2FreeResponse(&resp2);

            // 3) GET /nope -> 404
            H2Request nope{};
            std::string urlNope = h2cBase + "/nope";
            nope.url = urlNope.c_str();
            nope.timeoutMs = 10000;
            sid = 0;
            H2_CHECK(H2BeginRequest(c, nope, &sid) == NetError::None && sid == 5,
                     "h2c begin stream 5");
            H2Response resp3{};
            H2_CHECK(H2RecvResponse(c, sid, &resp3, 10000) == NetError::None,
                     "h2c recv /nope");
            H2_CHECK(resp3.statusCode == 404, "h2c /nope status==404");
            H2FreeResponse(&resp3);

            // 4) POST /big: 70KiB echo -> exhausts the default 64KiB
            // flow-control window on BOTH directions (multi-frame DATA +
            // WINDOW_UPDATE round trips).
            std::vector<CHAOS_IL2CPP_UINT8> big(70000);
            for (std::size_t i = 0; i < big.size(); ++i)
                big[i] = static_cast<CHAOS_IL2CPP_UINT8>((i * 7 + 13) & 0xFF);
            H2Request bigReq{};
            bigReq.method = HttpMethod::Post;
            std::string urlBig = h2cBase + "/big";
            bigReq.url = urlBig.c_str();
            bigReq.body = big.data();
            bigReq.bodyLength = static_cast<CHAOS_IL2CPP_INT32>(big.size());
            bigReq.timeoutMs = 20000;
            sid = 0;
            H2_CHECK(H2BeginRequest(c, bigReq, &sid) == NetError::None && sid == 7,
                     "h2c begin stream 7");
            H2Response resp4{};
            H2_CHECK(H2RecvResponse(c, sid, &resp4, 20000) == NetError::None,
                     "h2c recv /big");
            H2_CHECK(resp4.statusCode == 200 &&
                         resp4.bodyLength == (CHAOS_IL2CPP_INT32)big.size() &&
                         std::memcmp(resp4.body, big.data(), big.size()) == 0,
                     "h2c /big echo mismatch");
            H2FreeResponse(&resp4);

            // 5) GET /hdr with x-test: frobnicated -> HPACK literal request
            // header round trip (response body encodes the received value).
            HttpHeader hdrs[1];
            hdrs[0].name = "x-test";
            hdrs[0].value = "frobnicated";
            H2Request hdrReq{};
            std::string urlHdr = h2cBase + "/hdr";
            hdrReq.url = urlHdr.c_str();
            hdrReq.headers = hdrs;
            hdrReq.headerCount = 1;
            hdrReq.timeoutMs = 10000;
            sid = 0;
            H2_CHECK(H2BeginRequest(c, hdrReq, &sid) == NetError::None && sid == 9,
                     "h2c begin stream 9");
            H2Response resp5{};
            H2_CHECK(H2RecvResponse(c, sid, &resp5, 10000) == NetError::None,
                     "h2c recv /hdr");
            H2_CHECK(resp5.statusCode == 200 && resp5.bodyLength == 18 &&
                         std::memcmp(resp5.body, "x-test:frobnicated", 18) == 0,
                     "h2c /hdr literal header round trip");
            H2FreeResponse(&resp5);
            H2Free(c);
            c = nullptr;

            // 6) multiplexing: two in-flight streams on a fresh connection,
            // responses reaped out of order (stream 3 before stream 1).
            std::string h2cMuxBase = "h2c://127.0.0.1:" + std::to_string(h2cMuxPort.load());
            H2Client* m = nullptr;
            H2Request m1{};
            std::string urlM1 = h2cMuxBase + "/hello";
            m1.url = urlM1.c_str();
            m1.timeoutMs = 10000;
            H2_CHECK(H2Connect(m1, &m) == NetError::None, "mux connect");
            if (m) {
                CHAOS_IL2CPP_INT32 s1 = 0, s3 = 0;
                H2_CHECK(H2BeginRequest(m, m1, &s1) == NetError::None && s1 == 1,
                         "mux begin stream 1");
                H2Request m2{};
                m2.method = HttpMethod::Post;
                std::string urlM2 = h2cMuxBase + "/echo";
                m2.url = urlM2.c_str();
                const char* muxBody = "mux-2";
                m2.body = reinterpret_cast<const CHAOS_IL2CPP_UINT8*>(muxBody);
                m2.bodyLength = 5;
                m2.timeoutMs = 10000;
                H2_CHECK(H2BeginRequest(m, m2, &s3) == NetError::None && s3 == 3,
                         "mux begin stream 3");
                H2Response r3{};
                H2_CHECK(H2RecvResponse(m, s3, &r3, 10000) == NetError::None,
                         "mux recv stream 3");
                H2_CHECK(r3.statusCode == 200 && r3.bodyLength == 5 &&
                             std::memcmp(r3.body, muxBody, 5) == 0,
                         "mux stream 3 echo");
                H2FreeResponse(&r3);
                H2Response r1{};
                H2_CHECK(H2RecvResponse(m, s1, &r1, 10000) == NetError::None,
                         "mux recv stream 1");
                H2_CHECK(r1.statusCode == 200 && r1.bodyLength == 16 &&
                             std::memcmp(r1.body, "Hello, Chaos H2!", 16) == 0,
                         "mux stream 1 body");
                H2FreeResponse(&r1);
                H2Free(m);
            }

            // 7) h2:// (TLS, ALPN h2)
            H2Client* tc = nullptr;
            H2Request tr{};
            std::string urlTls = "h2://127.0.0.1:" + std::to_string(h2Port.load()) + "/hello";
            tr.url = urlTls.c_str();
            tr.disableCertificateValidation = true;
            tr.timeoutMs = 15000;
            NetError tce = H2Connect(tr, &tc);
            H2_CHECK(tce == NetError::None, "h2 tls connect");
            if (tc) {
                CHAOS_IL2CPP_INT32 sid = 0;
                NetError tbe = H2BeginRequest(tc, tr, &sid);
                H2_CHECK(tbe == NetError::None && sid == 1,
                         "h2 tls begin stream 1");
                H2Response resp{};
                NetError tre = H2RecvResponse(tc, sid, &resp, 15000);
                H2_CHECK(tre == NetError::None,
                         "h2 tls recv /hello");
                H2_CHECK(resp.statusCode == 200 && resp.bodyLength == 16 &&
                             std::memcmp(resp.body, "Hello, Chaos H2!", 16) == 0,
                         "h2 tls /hello body");
                H2FreeResponse(&resp);
                H2Free(tc);
            }
        }
    }

    h2cT.join();
    h2cMuxT.join();
    h2T.join();
    FreeSelfSignedCert(&cert);
    NetCleanup();

    if (g_failures.load() == 0) {
        std::printf("[NET-HTTP2-TEST] ALL PASS\n");
        return 0;
    }
    std::printf("[NET-HTTP2-TEST] FAILED\n");
    return 1;
}