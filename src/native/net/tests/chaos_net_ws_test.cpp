// NT-14 loopback test: WebSocket (RFC 6455) client + SSE client.
//
//   * ws://  server -- raw loopback socket, RFC 6455 opening handshake
//     (Sec-WebSocket-Accept validated via WsComputeAccept) then a frame
//     echo loop: text/binary (incl. extended-length 126/127), fragmented
//     messages, Ping->Pong, Close echo. Every client frame is checked for
//     the required MASK bit (RFC 6455 §5.3).
//   * wss:// server -- same but wrapped in TLS (self-signed cert, client
//     uses disableCertificateValidation, mirroring the TLS test).
//   * SSE   server -- text/event-stream: comment, id/event/data, multi-line
//     data, retry, blank-line dispatch, EOF-flush, then close.
//
// Dual platform: MSVC/Schannel + CryptoAPI on Windows, MSYS2/OpenSSL else.
#include <chaos/net/ws.h>
#include <chaos/net/sse.h>
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
using chaos::net::NetSocketSend;
using chaos::net::NetSocketRecv;
using chaos::net::NetSocketSetOption;
using chaos::net::WsClient;
using chaos::net::WsOptions;
using chaos::net::WsFrame;
using chaos::net::WsOpcode;
using chaos::net::WsOpen;
using chaos::net::WsSend;
using chaos::net::WsSendFrame;
using chaos::net::WsRecv;
using chaos::net::WsClose;
using chaos::net::WsFree;
using chaos::net::WsComputeAccept;
using chaos::net::SseClient;
using chaos::net::SseOptions;
using chaos::net::SseEvent;
using chaos::net::SseOpen;
using chaos::net::SseNextEvent;
using chaos::net::SseFree;
#ifdef _WIN32
using chaos::net::CreateSchannelProvider;
#else
using chaos::net::CreateOpenSslProvider;
#endif

namespace {

std::atomic<int> g_failures{0};

#define WS_FAIL(msg)                                                     \
    do {                                                                 \
        std::printf("[NET-WS-TEST] FAIL %s:%d %s\n", __FILE__, __LINE__, msg); \
        ++g_failures;                                                    \
    } while (0)

#define WS_CHECK(cond, msg)                                              \
    do {                                                                 \
        if (!(cond)) WS_FAIL(msg);                                       \
    } while (0)

#define TLS_CERT_FAIL(step)                                              \
    do {                                                                 \
        std::printf("GenerateSelfSignedCert %s failed 0x%08lX\n", step,  \
                    (unsigned long)GetLastError());                      \
        return false;                                                    \
    } while (0)
// ── self-signed cert (same structure as chaos_net_tls_test.cpp) ────────
#ifdef _WIN32
#undef TLS_CERT_FAIL
#define TLS_CERT_FAIL(step)                            \
    do {                                                    \
        std::printf("[NET-WS-TEST] GenerateSelfSignedCert %s failed (0x%08lX)\n", \
                    step, (unsigned long)::GetLastError()); \
        return false;                                       \
    } while (0)

bool GenerateSelfSignedCert(TlsCertificate* out) {
    wchar_t container[64];
    std::swprintf(container, 64, L"chaos-ws-test-%lu", (unsigned long)GetCurrentProcessId());

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

    const char* cn = "chaos-ws-test";
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
                               (const unsigned char*)"chaos-ws-test", -1, -1, 0);
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
// ---- server-side stream (transport + optional TLS) ----
struct ServerStream {
    SocketHandle sock{};
    TlsProvider* tls = nullptr;
    std::vector<CHAOS_IL2CPP_UINT8> buf{};
    std::size_t pos = 0;

    bool Fill(int tmo) {
        if (pos < buf.size()) return true;
        buf.clear();
        pos = 0;
        buf.resize(16384);
        if (tls) {
            CHAOS_IL2CPP_UINT32 g = 0;
            NetError err = tls->Read(&sock, buf.data(),
                                     (CHAOS_IL2CPP_UINT32)buf.size(), &g);
            if (err != NetError::None || g == 0) return false;
            buf.resize((std::size_t)g);
            return true;
        }
        CHAOS_IL2CPP_INT32 got = 0;
        NetError err = NetSocketRecv(&sock, buf.data(),
                                     (CHAOS_IL2CPP_INT32)buf.size(), 0, &got, tmo);
        if (err != NetError::None || got <= 0) return false;
        buf.resize((std::size_t)got);
        return true;
    }
    bool ReadN(CHAOS_IL2CPP_UINT8* dst, std::size_t n, int tmo) {
        std::size_t done = 0;
        while (done < n) {
            if (!Fill(tmo)) return false;
            std::size_t avail = buf.size() - pos;
            std::size_t take = avail < (n - done) ? avail : (n - done);
            std::memcpy(dst + done, buf.data() + pos, take);
            pos += take;
            done += take;
        }
        return true;
    }
    bool ReadLine(std::string* out, int tmo) {
        out->clear();
        for (;;) {
            if (!Fill(tmo)) return false;
            std::size_t avail = buf.size() - pos;
            for (std::size_t i = 0; i < avail; ++i) {
                if (buf[pos + i] == '\n') {
                    std::size_t len = i;
                    if (len > 0 && buf[pos + len - 1] == '\r') --len;
                    out->append((const char*)buf.data() + pos, len);
                    pos += i + 1;
                    return true;
                }
            }
            out->append((const char*)buf.data() + pos, avail);
            pos = buf.size();
        }
    }
    bool SendAll(const CHAOS_IL2CPP_UINT8* data, std::size_t n, int tmo) {
        std::size_t done = 0;
        while (done < n) {
            if (tls) {
                CHAOS_IL2CPP_UINT32 wrote = 0;
                NetError err = tls->Write(&sock, data + done,
                                          (CHAOS_IL2CPP_UINT32)(n - done), &wrote);
                if (err != NetError::None || wrote == 0) return false;
                done += (std::size_t)wrote;
                continue;
            }
            CHAOS_IL2CPP_INT32 sent = 0;
            NetError err = NetSocketSend(&sock, data + done,
                                         (CHAOS_IL2CPP_INT32)(n - done), 0, &sent, tmo);
            if (err != NetError::None || sent <= 0) return false;
            done += (std::size_t)sent;
        }
        return true;
    }
    bool SendText(const std::string& s, int tmo = 5000) {
        return SendAll((const CHAOS_IL2CPP_UINT8*)s.data(), s.size(), tmo);
    }
};

// ---- WS frame wire helpers (server side; %server frames are unmasked) ----
struct ServerFrame {
    int opcode = 0;
    bool fin = true;
    bool masked = false;
    std::vector<CHAOS_IL2CPP_UINT8> payload;
};

bool ServerReadFrame(ServerStream* s, ServerFrame* f, int tmo = 5000) {
    CHAOS_IL2CPP_UINT8 b0 = 0, b1 = 0;
    if (!s->ReadN(&b0, 1, tmo) || !s->ReadN(&b1, 1, tmo)) return false;
    f->fin = (b0 & 0x80) != 0;
    f->opcode = b0 & 0x0F;
    f->masked = (b1 & 0x80) != 0;
    CHAOS_IL2CPP_UINT64 len = b1 & 0x7F;
    if (len == 126) {
        CHAOS_IL2CPP_UINT8 ext[2];
        if (!s->ReadN(ext, 2, tmo)) return false;
        len = ((CHAOS_IL2CPP_UINT64)ext[0] << 8) | ext[1];
    } else if (len == 127) {
        CHAOS_IL2CPP_UINT8 ext[8];
        if (!s->ReadN(ext, 8, tmo)) return false;
        len = 0;
        for (int i = 0; i < 8; ++i) len = (len << 8) | ext[i];
    }
    CHAOS_IL2CPP_UINT8 mask[4] = {0, 0, 0, 0};
    if (f->masked && !s->ReadN(mask, 4, tmo)) return false;
    f->payload.assign((std::size_t)len, 0);
    if (len > 0 && !s->ReadN(f->payload.data(), (std::size_t)len, tmo)) return false;
    if (f->masked) {
        for (std::size_t i = 0; i < f->payload.size(); ++i)
            f->payload[i] ^= mask[i & 3];
    }
    return true;
}

// WS server: one connection, handshake + echo loop.
void RunWsServer(std::atomic<int>* portOut, bool useTls, const TlsCertificate& cert) {
    SocketHandle ls{};
    if (NetSocketCreate(chaos::net::kAddressFamilyInet, chaos::net::kSocketTypeStream,
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
    SocketHandle peer{};
    if (NetSocketAccept(&ls, &peer, nullptr, 10000) != NetError::None) {
        NetSocketClose(&ls);
        return;
    }
    NetSocketClose(&ls);

    ServerStream s;
    s.sock = peer;
    TlsProvider* prov = nullptr;
    if (useTls) {
#ifdef _WIN32
        prov = CreateSchannelProvider();
#else
        prov = CreateOpenSslProvider();
#endif
        TlsOptions to;
        to.mode = TlsMode::Server;
        to.serverCertificate = cert;
        to.timeoutMs = 10000;
        if (prov->Create(to) != NetError::None ||
            prov->Handshake(&peer) != NetError::None) {
            delete prov;
            NetSocketClose(&peer);
            return;
        }
        s.tls = prov;
    }

    // ---- RFC 6455 opening handshake ----
    std::string reqLine;
    if (!s.ReadLine(&reqLine, 5000)) {
        if (prov) { prov->Shutdown(); delete prov; }
        NetSocketClose(&peer);
        return;
    }
    std::string key;
    for (;;) {
        std::string line;
        if (!s.ReadLine(&line, 5000)) break;
        if (line.empty()) break;
        std::size_t colon = line.find(':');
        if (colon == std::string::npos) continue;
        std::string name = line.substr(0, colon);
        std::string value = line.substr(colon + 1);
        if (!value.empty() && value[0] == ' ') value.erase(0, 1);
        // case-insensitive compare
        for (auto& ch : name)
            if (ch >= 'A' && ch <= 'Z') ch = (char)(ch - 'A' + 'a');
        if (name == "sec-websocket-key") key = value;
    }
    char accept[29];
    if (key.empty() || !WsComputeAccept(key.c_str(), accept, sizeof(accept))) {
        if (prov) { prov->Shutdown(); delete prov; }
        NetSocketClose(&peer);
        return;
    }
    std::string resp =
        "HTTP/1.1 101 Switching Protocols\r\n"
        "Upgrade: websocket\r\n"
        "Connection: Upgrade\r\n"
        "Sec-WebSocket-Accept: " +
        std::string(accept) + "\r\n\r\n";
    if (!s.SendText(resp, 5000)) {
        if (prov) { prov->Shutdown(); delete prov; }
        NetSocketClose(&peer);
        return;
    }

    // ---- echo loop ----
    for (;;) {
        ServerFrame f;
        if (!ServerReadFrame(&s, &f, 5000)) break;  // peer closed
        if (f.opcode == 0x9) {
            // Ping -> Pong (unmasked)
            CHAOS_IL2CPP_UINT8 hdr[8];
            std::size_t n = 2;
            hdr[0] = 0x80 | 0xA;
            if (f.payload.size() < 126) {
                hdr[1] = (CHAOS_IL2CPP_UINT8)f.payload.size();
            } else {
                hdr[1] = 126;
                hdr[2] = (CHAOS_IL2CPP_UINT8)((f.payload.size() >> 8) & 0xFF);
                hdr[3] = (CHAOS_IL2CPP_UINT8)(f.payload.size() & 0xFF);
                n = 4;
            }
            if (!s.SendAll(hdr, n, 5000)) break;
            if (!f.payload.empty() && !s.SendAll(f.payload.data(), f.payload.size(), 5000)) break;
            continue;
        }
        if (f.opcode == 0x8) {
            // Close -> echo Close, then close the stream
            CHAOS_IL2CPP_UINT8 hdr[8];
            std::size_t n = 2;
            hdr[0] = 0x80 | 0x8;
            if (f.payload.size() < 126) {
                hdr[1] = (CHAOS_IL2CPP_UINT8)f.payload.size();
            } else {
                hdr[1] = 126;
                hdr[2] = (CHAOS_IL2CPP_UINT8)((f.payload.size() >> 8) & 0xFF);
                hdr[3] = (CHAOS_IL2CPP_UINT8)(f.payload.size() & 0xFF);
                n = 4;
            }
            s.SendAll(hdr, n, 5000);
            if (!f.payload.empty()) s.SendAll(f.payload.data(), f.payload.size(), 5000);
            break;
        }
        // data frame -> echo (preserve fin/opcode/payload; unmasked)
        CHAOS_IL2CPP_UINT8 hdr[8];
        std::size_t n = 2;
        hdr[0] = (CHAOS_IL2CPP_UINT8)((f.fin ? 0x80 : 0x00) | (f.opcode & 0x0F));
        if (f.payload.size() < 126) {
            hdr[1] = (CHAOS_IL2CPP_UINT8)f.payload.size();
        } else if (f.payload.size() <= 0xFFFF) {
            hdr[1] = 126;
            hdr[2] = (CHAOS_IL2CPP_UINT8)((f.payload.size() >> 8) & 0xFF);
            hdr[3] = (CHAOS_IL2CPP_UINT8)(f.payload.size() & 0xFF);
            n = 4;
        } else {
            hdr[1] = 127;
            CHAOS_IL2CPP_UINT64 L = (CHAOS_IL2CPP_UINT64)f.payload.size();
            n = 2;
            for (int i = 7; i >= 0; --i)
                hdr[n++] = (CHAOS_IL2CPP_UINT8)((L >> (8 * i)) & 0xFF);
        }
        if (!s.SendAll(hdr, n, 5000)) break;
        if (!f.payload.empty() && !s.SendAll(f.payload.data(), f.payload.size(), 5000)) break;
    }
    if (prov) {
        prov->Shutdown();
        delete prov;
    }
    NetSocketClose(&peer);
}

// SSE server: one connection, sends a fixed event stream then closes.
void RunSseServer(std::atomic<int>* portOut) {
    SocketHandle ls{};
    if (NetSocketCreate(chaos::net::kAddressFamilyInet, chaos::net::kSocketTypeStream,
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
    SocketHandle peer{};
    if (NetSocketAccept(&ls, &peer, nullptr, 10000) != NetError::None) {
        NetSocketClose(&ls);
        return;
    }
    NetSocketClose(&ls);
    ServerStream s;
    s.sock = peer;
    std::string line;
    s.ReadLine(&line, 5000);  // GET line
    for (;;) {
        if (!s.ReadLine(&line, 5000)) break;
        if (line.empty()) break;  // end of headers
    }
    std::string body =
        ": stream comment line\n"
        "id: 1\n"
        "event: greeting\n"
        "data: hello-sse\n"
        "\n"
        "data: multi\n"
        "data: line\n"
        "\n"
        "retry: 5000\n"
        "data: last\n"
        "\n";
    std::string resp = "HTTP/1.1 200 OK\r\n"
                       "Content-Type: text/event-stream\r\n"
                       "Connection: close\r\n"
                       "\r\n";
    s.SendText(resp, 5000);
    s.SendText(body, 5000);
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    NetSocketClose(&peer);
}

// wait for all ports to be set (or any failure), returns false on timeout
bool WaitPorts(const std::atomic<int>& ws, const std::atomic<int>& wss,
               const std::atomic<int>& sse) {
    auto deadline = std::chrono::steady_clock::now() + std::chrono::seconds(15);
    while (std::chrono::steady_clock::now() < deadline) {
        if (ws.load() != 0 && wss.load() != 0 && sse.load() != 0)
            return ws.load() > 0 && wss.load() > 0 && sse.load() > 0;
        if (ws.load() < 0 || wss.load() < 0 || sse.load() < 0) return false;
        std::this_thread::sleep_for(std::chrono::milliseconds(2));
    }
    return false;
}

}  // namespace

int main() {
    std::setvbuf(stdout, nullptr, _IONBF, 0);
    if (NetStartup() != NetError::None) {
        std::printf("[NET-WS-TEST] NetStartup failed\n");
        return 1;
    }
    TlsCertificate cert{};
    if (!GenerateSelfSignedCert(&cert)) {
        std::printf("[NET-WS-TEST] cert generation failed\n");
        NetCleanup();
        return 1;
    }

    std::atomic<int> wsPort{0}, wssPort{0}, ssePort{0};
    std::thread wsT(RunWsServer, &wsPort, false, std::cref(cert));
    std::thread wssT(RunWsServer, &wssPort, true, std::cref(cert));
    std::thread sseT(RunSseServer, &ssePort);

    int rc = 0;
    if (!WaitPorts(wsPort, wssPort, ssePort)) {
        std::printf("[NET-WS-TEST] FAIL server ports never came up\n");
        ++g_failures;
        rc = 1;
    } else {
        const int wp = wsPort.load();
        const int wsp = wssPort.load();
        const int sp = ssePort.load();

        // ---- ws:// scenarios ----
        std::string wsUrl = "ws://127.0.0.1:" + std::to_string(wp) + "/chat";
        WsOptions wo;
        wo.url = wsUrl.c_str();
        wo.timeoutMs = 10000;
        WsClient* ws = nullptr;
        WS_CHECK(WsOpen(wo, &ws) == NetError::None, "ws open");
        if (ws) {
            // text round trip
            const char* hello = "hello-ws";
            WS_CHECK(WsSend(ws, WsOpcode::Text, (const CHAOS_IL2CPP_UINT8*)hello,
                            (CHAOS_IL2CPP_INT32)strlen(hello)) == NetError::None,
                     "ws send text");
            WsFrame f{};
            WS_CHECK(WsRecv(ws, &f, 5000) == NetError::None, "ws recv text");
            WS_CHECK(f.opcode == WsOpcode::Text && f.fin && f.payloadLength == 8 &&
                         std::memcmp(f.payload, hello, 8) == 0,
                     "ws text echo mismatch");

            // binary with NUL byte
            CHAOS_IL2CPP_UINT8 bin[5] = {0x00, 0x01, 0xFE, 0xFF, 0x42};
            WS_CHECK(WsSend(ws, WsOpcode::Binary, bin, 5) == NetError::None,
                     "ws send binary");
            f = WsFrame{};
            WS_CHECK(WsRecv(ws, &f, 5000) == NetError::None, "ws recv binary");
            WS_CHECK(f.opcode == WsOpcode::Binary && f.fin && f.payloadLength == 5 &&
                         std::memcmp(f.payload, bin, 5) == 0,
                     "ws binary echo mismatch");

            // extended length (300 bytes)
            std::vector<CHAOS_IL2CPP_UINT8> big(300);
            for (int i = 0; i < 300; ++i) big[(std::size_t)i] = (CHAOS_IL2CPP_UINT8)(i & 0xFF);
            WS_CHECK(WsSend(ws, WsOpcode::Binary, big.data(), (CHAOS_IL2CPP_INT32)big.size()) ==
                         NetError::None,
                     "ws send big");
            f = WsFrame{};
            WS_CHECK(WsRecv(ws, &f, 5000) == NetError::None, "ws recv big");
            WS_CHECK(f.opcode == WsOpcode::Binary && f.fin && f.payloadLength == 300 &&
                         std::memcmp(f.payload, big.data(), 300) == 0,
                     "ws big echo mismatch");

            // fragmentation
            WS_CHECK(WsSendFrame(ws, WsOpcode::Text, false,
                                 (const CHAOS_IL2CPP_UINT8*)"hello-", 6) == NetError::None,
                     "ws send frag1");
            WS_CHECK(WsSendFrame(ws, WsOpcode::Continuation, true,
                                 (const CHAOS_IL2CPP_UINT8*)"world", 5) == NetError::None,
                     "ws send frag2");
            f = WsFrame{};
            WS_CHECK(WsRecv(ws, &f, 5000) == NetError::None, "ws recv frag1");
            WS_CHECK(f.opcode == WsOpcode::Text && !f.fin && f.payloadLength == 6 &&
                         std::memcmp(f.payload, "hello-", 6) == 0,
                     "ws frag1 echo mismatch");
            f = WsFrame{};
            WS_CHECK(WsRecv(ws, &f, 5000) == NetError::None, "ws recv frag2");
            WS_CHECK(f.opcode == WsOpcode::Continuation && f.fin && f.payloadLength == 5 &&
                         std::memcmp(f.payload, "world", 5) == 0,
                     "ws frag2 echo mismatch");

            // ping -> auto pong
            WS_CHECK(WsSendFrame(ws, WsOpcode::Ping, true,
                                 (const CHAOS_IL2CPP_UINT8*)"hb", 2) == NetError::None,
                     "ws send ping");
            f = WsFrame{};
            WS_CHECK(WsRecv(ws, &f, 5000) == NetError::None, "ws recv pong");
            WS_CHECK(f.opcode == WsOpcode::Pong && f.payloadLength == 2 &&
                         std::memcmp(f.payload, "hb", 2) == 0,
                     "ws pong mismatch");

            // close handshake
            WS_CHECK(WsClose(ws, 1000, "bye") == NetError::None, "ws close handshake");
            WsFree(ws);
        }

        // ---- wss:// scenarios ----
        std::string wssUrl = "wss://127.0.0.1:" + std::to_string(wsp) + "/secure";
        WsOptions wso;
        wso.url = wssUrl.c_str();
        wso.disableCertificateValidation = true;
        wso.timeoutMs = 10000;
        WsClient* wss = nullptr;
        WS_CHECK(WsOpen(wso, &wss) == NetError::None, "wss open");
        if (wss) {
            const char* secure = "secure-ws";
            WS_CHECK(WsSend(wss, WsOpcode::Text, (const CHAOS_IL2CPP_UINT8*)secure,
                            (CHAOS_IL2CPP_INT32)strlen(secure)) == NetError::None,
                     "wss send");
            WsFrame f{};
            WS_CHECK(WsRecv(wss, &f, 5000) == NetError::None, "wss recv");
            WS_CHECK(f.opcode == WsOpcode::Text && f.fin && f.payloadLength == 9 &&
                         std::memcmp(f.payload, secure, 9) == 0,
                     "wss echo mismatch");
            WS_CHECK(WsClose(wss, 1000, "done") == NetError::None, "wss close handshake");
            WsFree(wss);
        }

        // ---- SSE scenarios ----
        std::string sseUrl = "http://127.0.0.1:" + std::to_string(sp) + "/events";
        SseOptions so;
        so.url = sseUrl.c_str();
        so.timeoutMs = 10000;
        SseClient* sse = nullptr;
        WS_CHECK(SseOpen(so, &sse) == NetError::None, "sse open");
        if (sse) {
            SseEvent ev;
            WS_CHECK(SseNextEvent(sse, &ev, 5000) == NetError::None, "sse event1");
            WS_CHECK(ev.event == "greeting" && ev.data == "hello-sse" && ev.id == "1",
                     "sse event1 mismatch");
            WS_CHECK(SseNextEvent(sse, &ev, 5000) == NetError::None, "sse event2");
            WS_CHECK(ev.event == "message" && ev.data == "multi\nline" && ev.id == "1",
                     "sse event2 mismatch");
            WS_CHECK(SseNextEvent(sse, &ev, 5000) == NetError::None, "sse event3");
            WS_CHECK(ev.event == "message" && ev.data == "last" && ev.id == "1" &&
                         ev.retry == 5000,
                     "sse event3 mismatch");
            WS_CHECK(SseNextEvent(sse, &ev, 5000) == NetError::ConnectionReset,
                     "sse eof");
            SseFree(sse);
        }
    }

    wsT.join();
    wssT.join();
    sseT.join();
    FreeSelfSignedCert(&cert);
    NetCleanup();

    if (g_failures.load() == 0 && rc == 0) {
        std::printf("[NET-WS-TEST] ALL PASS\n");
        return 0;
    }
    std::printf("[NET-WS-TEST] FAILED (%d)\n", g_failures.load());
    return 1;
}
