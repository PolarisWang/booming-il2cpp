// NT-15 performance baseline: loopback benchmark harness over chaos_net.
//
// Scenarios (all loopback, dual platform MSVC/Schannel + MSYS2/OpenSSL):
//   1. TCP throughput  -- one client->server bulk stream, GB/s (1e9 bytes/s)
//   2. connect rate    -- sequential connect/close cycles, conn/s
//   3. concurrency     -- N sockets open simultaneously, peak + open rate
//   4. TLS handshake   -- Schannel/OpenSSL handshake + 1-byte round trip, hs/s
//   5. HTTP req/s      -- HttpSend against a loopback HTTP/1.1 server (close per req)
//   6. concurrency-high -- large-concurrency target (100k aspiration on real
//      Linux; MSYS2 caps in-process sockets ~2048, peak actually reached is
//      reported and the soft stop is expected, not a failure)
//
// Output: [NET-BENCH] metric lines + a [NET-BENCH] platform= line (posix|win32)
// consumed by scripts/ci/net-bench-gate.py, + a final all-PASS summary.
// Baseline numbers are recorded in docs/ (net-performance-baseline.md and
// net-performance-gates.md, main repo). Counts are tunable via argv:
// chaos_net_bench.exe [throughputMB] [conns] [concurrent] [tlsHs] [httpReqs]
// [concHigh]
#include <chaos/net/net.h>
#include <chaos/net/net_api.h>
#include <chaos/net/dns.h>
#include <chaos/net/tls.h>
#include <chaos/net/http.h>

#include <atomic>
#include <chrono>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
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
using chaos::net::CreateSchannelProvider;
#else
#include <openssl/evp.h>
#include <openssl/rsa.h>
#include <openssl/x509.h>
#include <openssl/pem.h>
#include <sys/resource.h>
#include <unistd.h>
using chaos::net::CreateOpenSslProvider;
#endif

using namespace chaos::net;
using std::chrono::steady_clock;

namespace {

std::atomic<int> g_failures{0};

void benchFail(const char* msg) {
    std::printf("[NET-BENCH] FAIL %s\n", msg);
    ++g_failures;
}

// ---- timing helpers -----------------------------------------------------
double SecondsSince(steady_clock::time_point t0) {
    return std::chrono::duration<double>(steady_clock::now() - t0).count();
}

// ---- self-signed cert (dual platform, same shape as https/tls tests) ----
bool GenerateSelfSignedCert(TlsCertificate* out) {
#ifdef _WIN32
    wchar_t container[64];
    std::swprintf(container, 64, L"chaos-bench-test-%lu",
                  (unsigned long)GetCurrentProcessId());
    HCRYPTPROV hProv = 0;
    if (!CryptAcquireContextW(&hProv, container, MS_ENH_RSA_AES_PROV_W,
                              PROV_RSA_AES, CRYPT_NEWKEYSET)) {
        if (GetLastError() == NTE_EXISTS) {
            if (!CryptAcquireContextW(&hProv, container, MS_ENH_RSA_AES_PROV_W,
                                      PROV_RSA_AES, 0)) {
                return false;
            }
        } else {
            return false;
        }
    }
    HCRYPTKEY hKey = 0;
    if (!CryptGenKey(hProv, AT_KEYEXCHANGE, CRYPT_EXPORTABLE | 0x08000000,
                     &hKey)) {
        CryptReleaseContext(hProv, 0);
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
        return false;
    }
    CERT_PUBLIC_KEY_INFO* pPubInfo = (CERT_PUBLIC_KEY_INFO*)pubKeyInfo.data();
    const char* cn = "chaos-bench-test";
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
    CryptEncodeObject(X509_ASN_ENCODING, X509_NAME, &nameInfo, nullptr,
                      &nameLen);
    std::vector<BYTE> nameEnc(nameLen);
    CryptEncodeObject(X509_ASN_ENCODING, X509_NAME, &nameInfo,
                      nameEnc.data(), &nameLen);
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
    CryptSignAndEncodeCertificate(hProv, AT_KEYEXCHANGE, X509_ASN_ENCODING,
                                  X509_CERT_TO_BE_SIGNED, &ci,
                                  &ci.SignatureAlgorithm, nullptr, nullptr,
                                  &encLen);
    std::vector<BYTE> enc(encLen);
    CryptSignAndEncodeCertificate(hProv, AT_KEYEXCHANGE, X509_ASN_ENCODING,
                                  X509_CERT_TO_BE_SIGNED, &ci,
                                  &ci.SignatureAlgorithm, nullptr, enc.data(),
                                  &encLen);
    PCCERT_CONTEXT ctx = CertCreateCertificateContext(X509_ASN_ENCODING,
                                                      enc.data(), encLen);
    if (ctx == nullptr) {
        CryptReleaseContext(hProv, 0);
        return false;
    }
    CRYPT_KEY_PROV_INFO kpi = {};
    kpi.pwszContainerName = container;
    kpi.pwszProvName = MS_ENH_RSA_AES_PROV_W;
    kpi.dwProvType = PROV_RSA_AES;
    kpi.dwKeySpec = AT_KEYEXCHANGE;
    if (!CertSetCertificateContextProperty(ctx, CERT_KEY_PROV_INFO_PROP_ID, 0,
                                           &kpi)) {
        CertFreeCertificateContext(ctx);
        CryptReleaseContext(hProv, 0);
        return false;
    }
    out->native1 = (void*)ctx;
    if (hKey) CryptDestroyKey(hKey);
    CryptReleaseContext(hProv, 0);
    return true;
#else
    EVP_PKEY* pkey = nullptr;
    EVP_PKEY_CTX* pctx = EVP_PKEY_CTX_new_id(EVP_PKEY_RSA, nullptr);
    if (pctx == nullptr) return false;
    bool ok = EVP_PKEY_keygen_init(pctx) > 0 &&
              EVP_PKEY_CTX_set_rsa_keygen_bits(pctx, 2048) > 0 &&
              EVP_PKEY_keygen(pctx, &pkey) > 0;
    EVP_PKEY_CTX_free(pctx);
    if (!ok) return false;
    X509* x = X509_new();
    if (x == nullptr) { EVP_PKEY_free(pkey); return false; }
    X509_set_version(x, 2);
    ASN1_INTEGER_set(X509_get_serialNumber(x), 0x1234L);
    X509_gmtime_adj(X509_get_notBefore(x), -60);
    X509_gmtime_adj(X509_get_notAfter(x), 60L * 60L * 24L * 365L);
    X509_set_pubkey(x, pkey);
    X509_NAME* name = X509_get_subject_name(x);
    X509_NAME_add_entry_by_txt(name, "CN", MBSTRING_ASC,
                               (const unsigned char*)"chaos-bench-test", -1,
                               -1, 0);
    X509_set_issuer_name(x, name);
    if (X509_sign(x, pkey, EVP_sha256()) <= 0) {
        X509_free(x);
        EVP_PKEY_free(pkey);
        return false;
    }
    out->native1 = (void*)x;
    out->native2 = (void*)pkey;
    return true;
#endif
}

void FreeSelfSignedCert(TlsCertificate* cert) {
#ifdef _WIN32
    if (cert->native1) CertFreeCertificateContext((PCCERT_CONTEXT)cert->native1);
    cert->native1 = nullptr;
#else
    if (cert->native1) X509_free((X509*)cert->native1);
    if (cert->native2) EVP_PKEY_free((EVP_PKEY*)cert->native2);
    cert->native1 = nullptr;
    cert->native2 = nullptr;
#endif
}

// ---- raw transport helpers ----------------------------------------------
SocketHandle MakeLoopbackListener(CHAOS_IL2CPP_UINT16* portOut) {
    SocketHandle ls{};
    if (NetSocketCreate( kAddressFamilyInet, kSocketTypeStream, kProtocolTcp,
                         &ls) != NetError::None) {
        benchFail("listener create");
        return SocketHandle{};
    }
    NetAddress any{};
    any.family = kAddressFamilyInet;
    any.port = 0;
    if (NetSocketBind(&ls, any) != NetError::None) {
        benchFail("listener bind");
        NetSocketClose(&ls);
        return SocketHandle{};
    }
    if (NetSocketListen(&ls, 256) != NetError::None) {
        benchFail("listener listen");
        NetSocketClose(&ls);
        return SocketHandle{};
    }
    if (portOut) {
        NetAddress got{};
        NetSocketGetLocalAddress(&ls, &got);
        *portOut = got.port;
    }
    return ls;
}

// Platform TLS provider factory (Schannel on Windows, OpenSSL elsewhere).
TlsProvider* MakeProvider() {
#ifdef _WIN32
    return CreateSchannelProvider();
#else
    return CreateOpenSslProvider();
#endif
}

// Cygwin/MSYS2 default RLIMIT_NOFILE is 1024 -- raise it so concurrency
// scenarios are not capped by the fd limit (Windows handles are plentiful).
void RaiseFdLimit() {
#ifndef _WIN32
    struct rlimit rl = {};
    if (getrlimit(RLIMIT_NOFILE, &rl) == 0) {
        if (rl.rlim_cur < 8192) {
            rl.rlim_cur = 8192;
            if (rl.rlim_max < 8192) rl.rlim_max = 8192;
            setrlimit(RLIMIT_NOFILE, &rl);
        }
    }
#else
    (void)0;
#endif
}

}  // namespace

int main(int argc, char** argv) {
    CHAOS_IL2CPP_INT32 tputMB = argc > 1 ? std::atoi(argv[1]) : 64;
    CHAOS_IL2CPP_INT32 nConns = argc > 2 ? std::atoi(argv[2]) : 2000;
    CHAOS_IL2CPP_INT32 nConc  = argc > 3 ? std::atoi(argv[3]) : 1000;
    CHAOS_IL2CPP_INT32 nTls   = argc > 4 ? std::atoi(argv[4]) : 200;
    CHAOS_IL2CPP_INT32 nHttp  = argc > 5 ? std::atoi(argv[5]) : 500;
    CHAOS_IL2CPP_INT32 nConcHigh = argc > 6 ? std::atoi(argv[6]) : 20000;
    CHAOS_IL2CPP_INT32 nUdpPkts = argc > 7 ? std::atoi(argv[7]) : 100000;
    if (tputMB <= 0) tputMB = 64;
    if (nConns <= 0) nConns = 2000;
    if (nConc <= 0) nConc = 1000;
    if (nTls <= 0) nTls = 200;
    if (nHttp <= 0) nHttp = 500;
    if (nConcHigh <= 0) nConcHigh = 20000;
    if (nUdpPkts <= 0) nUdpPkts = 100000;

    RaiseFdLimit();
    if (NetStartup() != NetError::None) {
        benchFail("NetStartup");
        return 1;
    }
#ifdef _WIN32
    std::printf("[NET-BENCH] platform=win32\n");
#else
    std::printf("[NET-BENCH] platform=posix\n");
#endif

    // ---- 1. TCP throughput (client -> server bulk stream) ---------------
    {
        CHAOS_IL2CPP_UINT16 port = 0;
        SocketHandle ls = MakeLoopbackListener(&port);
        if (ls.fd <= 0) return 1;
        std::atomic<long long> serverGot{0};
        std::thread server([&] {
            SocketHandle peer{};
            if (NetSocketAccept(&ls, &peer, nullptr, 15000) != NetError::None) {
                benchFail("tput accept");
                NetSocketClose(&ls);
                return;
            }
            std::vector<CHAOS_IL2CPP_UINT8> buf(65536);
            for (;;) {
                CHAOS_IL2CPP_INT32 got = 0;
                NetError err = NetSocketRecv(&peer, buf.data(),
                                             (CHAOS_IL2CPP_INT32)buf.size(),
                                             0, &got, 15000);
                if (err != NetError::None) break;
                if (got == 0) break;
                serverGot += got;
            }
            NetSocketClose(&peer);
            NetSocketClose(&ls);
        });
        SocketHandle cli{};
        if (NetSocketCreate(kAddressFamilyInet, kSocketTypeStream, kProtocolTcp,
                            &cli) != NetError::None) {
            benchFail("tput client create");
            server.join();
            return 1;
        }
        NetSocketSetOption(&cli, NetOption::NoDelay, 1);
        if (NetSocketConnect(&cli, LoopbackV4(port), 15000) != NetError::None) {
            benchFail("tput connect");
            NetSocketClose(&cli);
            server.join();
            return 1;
        }
        std::vector<CHAOS_IL2CPP_UINT8> chunk(65536, 0xAB);
        long long total = (long long)tputMB * 1024 * 1024;
        long long sent = 0;
        auto t0 = steady_clock::now();
        while (sent < total) {
            CHAOS_IL2CPP_INT32 want = (CHAOS_IL2CPP_INT32)chunk.size();
            CHAOS_IL2CPP_INT32 s = 0;
            NetError err = NetSocketSend(&cli, chunk.data(),
                                         (CHAOS_IL2CPP_INT32)chunk.size(), 0,
                                         &s, 15000);
            if (err != NetError::None) break;
            if (s <= 0) break;
            sent += s;
        }
        double dt = SecondsSince(t0);
        NetSocketClose(&cli);
        server.join();
        double mbps = (double)sent / (1024.0 * 1024.0) / dt;
        double gbps = (double)sent * 8.0 / 1e9 / dt;
        std::printf("[NET-BENCH] tcp-throughput sent=%lldMB dt=%.3fs %.1f MiB/s %.3f Gb/s\n",
                    sent / (1024 * 1024), dt, mbps, gbps);
        if (serverGot != sent) {
            std::printf("[NET-BENCH] tput mismatch serverGot=%lld sent=%lld\n",
                        serverGot.load(), sent);
            ++g_failures;
        }
    }

    // ---- 2. sequential connect rate -------------------------------------
    {
        CHAOS_IL2CPP_UINT16 port = 0;
        SocketHandle ls = MakeLoopbackListener(&port);
        if (ls.fd <= 0) return 1;
        std::atomic<int> accepted{0};
        std::thread server([&] {
            for (int i = 0; i < nConns; ++i) {
                SocketHandle peer{};
                if (NetSocketAccept(&ls, &peer, nullptr, 15000) != NetError::None)
                    break;
                NetSocketClose(&peer);
                ++accepted;
            }
            NetSocketClose(&ls);
        });
        auto t0 = steady_clock::now();
        int fail = 0;
        for (int i = 0; i < nConns; ++i) {
            SocketHandle c{};
            if (NetSocketCreate(kAddressFamilyInet, kSocketTypeStream,
                                kProtocolTcp, &c) != NetError::None) { ++fail; break; }
            if (NetSocketConnect(&c, LoopbackV4(port), 15000) != NetError::None) {
                ++fail;
                NetSocketClose(&c);
                break;
            }
            NetSocketClose(&c);
        }
        double dt = SecondsSince(t0);
        server.join();
        std::printf("[NET-BENCH] connect-rate conns=%d dt=%.3fs %.0f conn/s (accept=%d fail=%d)\n",
                    nConns, dt, nConns / dt, accepted.load(), fail);
        if (fail != 0 || accepted.load() != nConns) ++g_failures;
    }

    // ---- 3. concurrency: N sockets open simultaneously -------------------
    {
        CHAOS_IL2CPP_UINT16 port = 0;
        SocketHandle ls = MakeLoopbackListener(&port);
        if (ls.fd <= 0) return 1;
        std::atomic<int> accepted{0};
        std::thread server([&] {
            for (int i = 0; i < nConc; ++i) {
                SocketHandle peer{};
                if (NetSocketAccept(&ls, &peer, nullptr, 20000) != NetError::None)
                    break;
                ++accepted;
            }
            NetSocketClose(&ls);
        });
        std::vector<SocketHandle> open_;
        open_.reserve((std::size_t)nConc);
        auto t0 = steady_clock::now();
        int fail = 0;
        for (int i = 0; i < nConc; ++i) {
            SocketHandle c{};
            if (NetSocketCreate(kAddressFamilyInet, kSocketTypeStream,
                                kProtocolTcp, &c) != NetError::None) { ++fail; break; }
            if (NetSocketConnect(&c, LoopbackV4(port), 20000) != NetError::None) {
                ++fail;
                NetSocketClose(&c);
                break;
            }
            open_.push_back(c);
        }
        double openDt = SecondsSince(t0);
        int peak = (int)open_.size();
        for (auto& c : open_) NetSocketClose(&c);
        server.join();
        // Environmental fd/handle caps make the exact target unachievable on
        // some platforms (MSYS2/cygwin ~2048 open sockets process-wide, both
        // ends in-process); a soft stop there is expected, not a failure.
        std::printf("[NET-BENCH] concurrency peak=%d target=%d dt=%.3fs %.0f conn/s\n",
                    peak, nConc, openDt, peak / openDt);
        if (fail != 0 && peak < 100) ++g_failures;
        if (accepted.load() == 0) ++g_failures;
    }

    // ---- 4. TLS handshake rate -------------------------------------------
    {
        TlsCertificate cert{};
        if (!GenerateSelfSignedCert(&cert)) {
            benchFail("tls cert");
            NetCleanup();
            return 1;
        }
        CHAOS_IL2CPP_UINT16 port = 0;
        SocketHandle ls = MakeLoopbackListener(&port);
        if (ls.fd <= 0) return 1;
        std::atomic<int> done{0};
        std::thread server([&] {
            for (int i = 0; i < nTls; ++i) {
                SocketHandle peer{};
                if (NetSocketAccept(&ls, &peer, nullptr, 20000) != NetError::None)
                    break;
                TlsOptions o{};
                o.mode = TlsMode::Server;
                o.serverCertificate = cert;
                o.timeoutMs = 15000;
                TlsProvider* sv = MakeProvider();
                sv->Create(o);
                NetError he = sv->Handshake(&peer);
                if (he == NetError::None) {
                    CHAOS_IL2CPP_UINT8 in = 0;
                    CHAOS_IL2CPP_UINT32 got = 0;
                    sv->Read(&peer, &in, 1, &got);
                    CHAOS_IL2CPP_UINT32 wr = 0;
                    sv->Write(&peer, &in, 1, &wr);
                }
                sv->Shutdown();
                delete sv;
                NetSocketClose(&peer);
                ++done;
            }
            NetSocketClose(&ls);
        });
        auto t0 = steady_clock::now();
        int fail = 0;
        for (int i = 0; i < nTls; ++i) {
            SocketHandle c{};
            if (NetSocketCreate(kAddressFamilyInet, kSocketTypeStream,
                                kProtocolTcp, &c) != NetError::None) { ++fail; break; }
            if (NetSocketConnect(&c, LoopbackV4(port), 20000) != NetError::None) {
                ++fail;
                NetSocketClose(&c);
                break;
            }
            TlsOptions o{};
            o.mode = TlsMode::Client;
            o.serverName = "127.0.0.1";
            o.disableCertificateValidation = true;
            o.timeoutMs = 15000;
            TlsProvider* cl = MakeProvider();
            if (cl->Create(o) != NetError::None ||
                cl->Handshake(&c) != NetError::None) {
                ++fail;
            } else {
                CHAOS_IL2CPP_UINT8 b = 0x42;
                CHAOS_IL2CPP_UINT32 wr = 0;
                CHAOS_IL2CPP_UINT32 got = 0;
                cl->Write(&c, &b, 1, &wr);
                cl->Read(&c, &b, 1, &got);
            }
            cl->Shutdown();
            delete cl;
            NetSocketClose(&c);
        }
        double dt = SecondsSince(t0);
        server.join();
        FreeSelfSignedCert(&cert);
        std::printf("[NET-BENCH] tls-handshake hs=%d done=%d dt=%.3fs %.0f hs/s\n",
                    nTls, done.load(), dt, nTls / dt);
        if (fail != 0 || done.load() != nTls) ++g_failures;
    }

    // ---- 5. HTTP req/s (HttpSend, connection: close server) ---------------
    {
        CHAOS_IL2CPP_UINT16 port = 0;
        SocketHandle ls = MakeLoopbackListener(&port);
        if (ls.fd <= 0) return 1;
        std::atomic<int> served{0};
        std::thread server([&] {
            for (int i = 0; i < nHttp; ++i) {
                SocketHandle peer{};
                if (NetSocketAccept(&ls, &peer, nullptr, 20000) != NetError::None)
                    break;
                // read request head (request line + headers until blank line)
                std::vector<CHAOS_IL2CPP_UINT8> req(4096);
                std::size_t used = 0;
                bool blankFound = false;
                while (used < req.size()) {
                    CHAOS_IL2CPP_INT32 got = 0;
                    NetError e = NetSocketRecv(&peer, req.data() + used,
                                               (CHAOS_IL2CPP_INT32)(req.size() - used),
                                               0, &got, 15000);
                    if (e != NetError::None || got <= 0) break;
                    for (CHAOS_IL2CPP_INT32 k = 0; k < got; ++k) {
                        if (req[used + (std::size_t)k] == '\n' &&
                            used + (std::size_t)k > 0 &&
                            req[used + (std::size_t)k - 1] == '\r') {
                            blankFound = true;
                        }
                    }
                    used += (std::size_t)got;
                    if (blankFound) break;
                }
                const char* resp =
                    "HTTP/1.1 200 OK\r\nContent-Length: 2\r\n"
                    "Connection: close\r\n\r\nok";
                CHAOS_IL2CPP_INT32 sent = 0;
                NetSocketSend(&peer, (const CHAOS_IL2CPP_UINT8*)resp,
                              (CHAOS_IL2CPP_INT32)std::strlen(resp), 0, &sent, 15000);
                NetSocketClose(&peer);
                ++served;
            }
            NetSocketClose(&ls);
        });
        auto t0 = steady_clock::now();
        int fail = 0;
        for (int i = 0; i < nHttp; ++i) {
            char url[128];
            std::snprintf(url, sizeof(url), "http://127.0.0.1:%u/hello", port);
            HttpRequest rq{};
            rq.method = HttpMethod::Get;
            rq.url = url;
            rq.timeoutMs = 15000;
            HttpResponse rs{};
            if (HttpSend(rq, &rs) != NetError::None || rs.statusCode != 200) {
                ++fail;
            }
            HttpFreeResponse(&rs);
        }
        double dt = SecondsSince(t0);
        server.join();
        HttpPoolClear();
        std::printf("[NET-BENCH] http-rps reqs=%d served=%d dt=%.3fs %.0f req/s\n",
                    nHttp, served.load(), dt, nHttp / dt);
        if (fail != 0 || served.load() != nHttp) ++g_failures;
    }

    // ---- 6. concurrency-high: large-concurrency target --------------------
    // Aspirational goal is 100k+ sockets open on a real Linux runner (NT-19
    // gate line; enforced once the NT-20 ubuntu leg is green). MSYS2/cygwin
    // caps in-process sockets at ~2048 (both ends share the fd budget), so its
    // soft stop at ~1024 peers is expected; Win32 reaches the target. The peak
    // actually reached is reported for the baseline record.
    {
        CHAOS_IL2CPP_UINT16 port = 0;
        SocketHandle ls = MakeLoopbackListener(&port);
        if (ls.fd <= 0) return 1;
        std::atomic<int> accepted{0};
        std::thread server([&] {
            for (int i = 0; i < nConcHigh; ++i) {
                SocketHandle peer{};
                if (NetSocketAccept(&ls, &peer, nullptr, 30000) != NetError::None)
                    break;
                ++accepted;
            }
            NetSocketClose(&ls);
        });
        std::vector<SocketHandle> open_;
        open_.reserve((std::size_t)nConcHigh);
        auto t0 = steady_clock::now();
        int fail = 0;
        for (int i = 0; i < nConcHigh; ++i) {
            SocketHandle c{};
            if (NetSocketCreate(kAddressFamilyInet, kSocketTypeStream,
                                kProtocolTcp, &c) != NetError::None) { ++fail; break; }
            if (NetSocketConnect(&c, LoopbackV4(port), 30000) != NetError::None) {
                ++fail;
                NetSocketClose(&c);
                break;
            }
            open_.push_back(c);
        }
        double dt = SecondsSince(t0);
        int peak = (int)open_.size();
        for (auto& c : open_) NetSocketClose(&c);
        server.join();
        std::printf("[NET-BENCH] concurrency-high peak=%d target=%d dt=%.3fs %.0f conn/s\n",
                    peak, nConcHigh, dt, peak / dt);
        if (fail != 0 && peak < 100) ++g_failures;
        if (accepted.load() == 0) ++g_failures;
    }

    // ---- 7. UDP pkt rate (connected dgram, loopback) --------------------
    {
        // server: bound UDP socket on an ephemeral loopback port
        SocketHandle us{};
        if (NetSocketCreate(kAddressFamilyInet, kSocketTypeDgram, kProtocolUdp, &us) != NetError::None) {
            benchFail("udp server create");
            return 1;
        }
        if (NetSocketBind(&us, LoopbackV4(0)) != NetError::None) {
            benchFail("udp server bind");
            NetSocketClose(&us);
            return 1;
        }
        NetAddress ua{};
        if (NetSocketGetLocalAddress(&us, &ua) != NetError::None) {
            benchFail("udp server local addr");
            NetSocketClose(&us);
            return 1;
        }
        std::atomic<int> got{0};
        std::thread server([&] {
            CHAOS_IL2CPP_UINT8 buf[64];
            for (int i = 0; i < nUdpPkts; ++i) {
                CHAOS_IL2CPP_INT32 n = 0;
                if (NetSocketRecv(&us, buf, (CHAOS_IL2CPP_INT32)sizeof(buf), 0, &n, 30000) != NetError::None || n <= 0)
                    break;
                got.fetch_add(1);
            }
            NetSocketClose(&us);
        });
        SocketHandle uc{};
        if (NetSocketCreate(kAddressFamilyInet, kSocketTypeDgram, kProtocolUdp, &uc) != NetError::None) {
            benchFail("udp client create");
            return 1;
        }
        if (NetSocketConnect(&uc, LoopbackV4(ua.port), 30000) != NetError::None) {
            benchFail("udp client connect");
            NetSocketClose(&uc);
            return 1;
        }
        const CHAOS_IL2CPP_UINT8 payload[16] = {0xAA,0xBB,0xCC,0xDD,0xEE,0xFF,1,2,3,4,5,6,7,8,9,10};
        auto t0 = steady_clock::now();
        for (int i = 0; i < nUdpPkts; ++i) {
            CHAOS_IL2CPP_INT32 sent = 0;
            if (NetSocketSend(&uc, payload, (CHAOS_IL2CPP_INT32)sizeof(payload), 0, &sent, 30000) != NetError::None || sent != (CHAOS_IL2CPP_INT32)sizeof(payload))
                break;
        }
        double dt = SecondsSince(t0);
        server.join();
        NetSocketClose(&uc);
        std::printf("[NET-BENCH] udp-pkt pkts=%d got=%d dt=%.3fs %.0f pkt/s\n",
                    nUdpPkts, got.load(), dt, nUdpPkts / dt);
        if (got.load() != nUdpPkts) ++g_failures;
    }
    NetCleanup();
    if (g_failures.load() == 0) {
        std::printf("[NET-BENCH] ALL PASS\n");
        return 0;
    }
    std::printf("[NET-BENCH] FAILED (%d)\n", g_failures.load());
    return 1;
}
