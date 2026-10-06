// Layer E -- TLS native test matrix (NT-12).  This single source compiles
// against both backends:
//   Windows : Schannel (CreateSchannelProvider) + CryptoAPI self-signed cert
//   POSIX   : OpenSSL   (CreateOpenSslProvider)   + OpenSSL-generated cert
// It drives a real loopback TLS handshake: a server thread accepts the
// connection and performs the server-side handshake, the main thread runs
// the client side; then "tls-ping"/"pong-tls" round-trip over the TLS
// record layer.  Exit 0 + "[NET-TLS-TEST] ALL PASS" means the gates pass.
#include <chaos/net/tls.h>
#include <chaos/net/socket_handle.h>
#include <chaos/net/net.h>

#include <atomic>
#include <chrono>
#include <cstdio>
#include <cstring>
#include <thread>
#include <vector>

#if defined(_WIN32)
#include <windows.h>
#include <wincrypt.h>
#else
#include <openssl/err.h>
#include <openssl/evp.h>
#include <openssl/x509.h>
#include <openssl/x509v3.h>
#endif

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
using chaos::net::NetSocketClose;
using chaos::net::CreateSchannelProvider;
using chaos::net::CreateOpenSslProvider;

namespace {

int g_failures = 0;

#define TLS_CHECK(cond, what)                                                    \
    do {                                                                         \
        if (!(cond)) {                                                           \
            std::printf("[NET-TLS-TEST] FAIL %s:%d %s\n", __FILE__, __LINE__,   \
                        what);                                                    \
            ++g_failures;                                                         \
        }                                                                        \
    } while (0)

std::atomic<int> g_port{0};

// ---------------------------------------------------------------------------
// Self-signed certificate generation (per platform).
// ---------------------------------------------------------------------------
#if defined(_WIN32)

TlsProvider* CreateTestProvider() {
    return CreateSchannelProvider();
}

#define TLS_CERT_FAIL(step)                            \
    do {                                                    \
        std::printf("[NET-TLS-TEST] GenerateSelfSignedCert %s failed (0x%08lX)\n", \
                    step, (unsigned long)::GetLastError()); \
        return false;                                       \
    } while (0)

bool GenerateSelfSignedCert(TlsCertificate* out) {
    wchar_t container[64];
    std::swprintf(container, 64, L"chaos-tls-test-%lu", (unsigned long)GetCurrentProcessId());

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

    // Export a proper DER CERT_PUBLIC_KEY_INFO (raw CSP PUBLICKEYBLOB bytes
    // would produce a malformed SPKI -> CRYPT_E_ASN1_BADTAG (0x8009310B) when
    // Schannel parses the server certificate at AcquireCredentialsHandle).
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
    // Use the exported buffer directly: it holds the CERT_PUBLIC_KEY_INFO
    // struct with pbData pointers adjusted into the same buffer (the whole
    // buffer must stay alive through CryptSignAndEncodeCertificate).  A
    // struct copy here would overflow (the buffer is struct+DER bytes).
    CERT_PUBLIC_KEY_INFO* pPubInfo = (CERT_PUBLIC_KEY_INFO*)pubKeyInfo.data();

    // CN = chaos-tls-test (UTF-8; ASCII content).
    const char* cn = "chaos-tls-test";
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

TlsProvider* CreateTestProvider() {
    return CreateOpenSslProvider();
}

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
                               (const unsigned char*)"chaos-tls-test", -1, -1, 0);
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

// RFC7301 wire format for { h2, http/1.1 }: 1 total-length byte + entries.
const unsigned char kAlpnBlob[] = {  // RFC7301: 2-byte total len (0x000C) + entries
    0x00, 0x0C,
    0x02, 'h', '2',
    0x08, 'h', 't', 't', 'p', '/', '1', '.', '1',
};
constexpr unsigned int kAlpnBlobLen = 14;

void RunServer(const TlsCertificate& cert) {
    SocketHandle srv = {};
    SocketHandle peer = {};
    TlsProvider* prov = nullptr;

    if (NetSocketCreate(2 /*Inet*/, 1 /*Stream*/, 6 /*Tcp*/, &srv) != NetError::None) {
        TLS_CHECK(false, "server NetSocketCreate");
        return;
    }
    NetAddress addr = LoopbackV4(0);
    if (NetSocketBind(&srv, addr) != NetError::None ||
        NetSocketListen(&srv, 8) != NetError::None) {
        TLS_CHECK(false, "server bind/listen");
        NetSocketClose(&srv);
        return;
    }
    if (NetSocketGetLocalAddress(&srv, &addr) != NetError::None) {
        TLS_CHECK(false, "server GetLocalAddress");
        NetSocketClose(&srv);
        return;
    }
    g_port.store(addr.port, std::memory_order_release);

    if (NetSocketAccept(&srv, &peer, nullptr, 10000) != NetError::None) {
        TLS_CHECK(false, "server accept");
        NetSocketClose(&srv);
        return;
    }
    std::printf("[TLS-PROBE] server accepted, calling provider Create\n");
    std::fflush(stdout);

    prov = CreateTestProvider();
    TlsOptions opts = {};
    opts.mode = TlsMode::Server;
    opts.serverCertificate = cert;
    opts.alpnProtocols = kAlpnBlob;
    opts.alpnLength = kAlpnBlobLen;
    opts.timeoutMs = 10000;
    if (prov->Create(opts) != NetError::None) {
        TLS_CHECK(false, "server Create");
    } else if (prov->Handshake(&peer) != NetError::None) {
        std::printf("[TLS-PROBE] server handshake FAILED\n"); std::fflush(stdout);
        std::fprintf(stderr, "[NET-TLS-TEST] server handshake failed: %s\n", prov->LastErrorDetail());
        std::fflush(stderr);
        TLS_CHECK(false, "server Handshake");
    } else {
        unsigned char buf[64] = {};
        unsigned int got = 0;
        if (prov->Read(&peer, buf, 9, &got) != NetError::None || got != 9 ||
            std::memcmp(buf, "tls-ping", 9) != 0) {
            TLS_CHECK(false, "server Read tls-ping");
        } else {
            unsigned int w = 0;
            if (prov->Write(&peer, (const unsigned char*)"pong-tls", 8, &w) != NetError::None ||
                w != 8) {
                TLS_CHECK(false, "server Write pong-tls");
            }
        }
        prov->Shutdown();
    }
    delete prov;
    NetSocketClose(&peer);
    NetSocketClose(&srv);
}

int RunClient() {
    SocketHandle cli = {};
    if (NetSocketCreate(2, 1, 6, &cli) != NetError::None) {
        TLS_CHECK(false, "client NetSocketCreate");
        return 1;
    }
    int port = 0;
    while ((port = g_port.load(std::memory_order_acquire)) == 0) {
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }
    NetAddress addr = LoopbackV4(port);
    if (NetSocketConnect(&cli, addr, 10000) != NetError::None) {
        TLS_CHECK(false, "client Connect");
        NetSocketClose(&cli);
        return 1;
    }
    std::printf("[TLS-PROBE] client connected, calling provider Create\n");
    std::fflush(stdout);

    TlsProvider* prov = CreateTestProvider();
    TlsOptions opts = {};
    opts.mode = TlsMode::Client;
    opts.serverName = "chaos-tls-test";
    opts.disableCertificateValidation = true;
    opts.alpnProtocols = kAlpnBlob;
    opts.alpnLength = kAlpnBlobLen;
    opts.timeoutMs = 10000;

    std::printf("[TLS-PROBE] client provider Created, starting handshake\n");
    std::fflush(stdout);
    if (prov->Create(opts) != NetError::None) {
        std::printf("[TLS-PROBE] client Create FAILED\n"); std::fflush(stdout);
        TLS_CHECK(false, "client Create");
    } else if (prov->Handshake(&cli) != NetError::None) {
        std::printf("[TLS-PROBE] client handshake FAILED\n"); std::fflush(stdout);
        std::fprintf(stderr, "[NET-TLS-TEST] client handshake failed: %s\n", prov->LastErrorDetail());
        std::fflush(stderr);
        TLS_CHECK(false, "client Handshake");
    } else {
        const char* proto = prov->NegotiatedProtocol();
        TLS_CHECK(proto != nullptr, "client NegotiatedProtocol non-null");
        if (proto != nullptr) {
            std::printf("[NET-TLS-TEST] negotiated %s, alpn=%s\n", proto,
                        prov->NegotiatedApplicationProtocol() ?
                            prov->NegotiatedApplicationProtocol() : "(none)");
        }
        const char* alpn = prov->NegotiatedApplicationProtocol();
        if (alpn != nullptr) {
            TLS_CHECK(std::strcmp(alpn, "h2") == 0, "client ALPN h2");
        }
        unsigned int w = 0;
        unsigned char rbuf[64] = {};
        unsigned int got = 0;
        if (prov->Write(&cli, (const unsigned char*)"tls-ping", 9, &w) != NetError::None ||
            w != 9) {
            TLS_CHECK(false, "client Write tls-ping");
        } else if (prov->Read(&cli, rbuf, 16, &got) != NetError::None || got != 8 ||
                   std::memcmp(rbuf, "pong-tls", 8) != 0) {
            TLS_CHECK(false, "client Read pong-tls");
        }
        prov->Shutdown();
    }
    delete prov;
    NetSocketClose(&cli);
    return 0;
}

}  // namespace

int main() {
    std::setvbuf(stdout, nullptr, _IONBF, 0);  // live output under redirected streams
    if (NetStartup() != NetError::None) {
        std::printf("[NET-TLS-TEST] FAIL NetStartup\n");
        return 1;
    }

    TlsCertificate cert = {};
    if (!GenerateSelfSignedCert(&cert)) {
        std::printf("[NET-TLS-TEST] FAIL GenerateSelfSignedCert\n");
        NetCleanup();
        return 1;
    }

    std::thread server(RunServer, std::cref(cert));
    int clientRc = RunClient();
    server.join();

    FreeSelfSignedCert(&cert);
    NetCleanup();

    if (g_failures == 0 && clientRc == 0) {
        std::printf("[NET-TLS-TEST] ALL PASS\n");
        return 0;
    }
    std::printf("[NET-TLS-TEST] FAILED (%d failures)\n", g_failures);
    return 1;
}
