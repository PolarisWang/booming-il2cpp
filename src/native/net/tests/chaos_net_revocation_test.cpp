// Layer E -- TLS certificate revocation: offline CRL (NT-18) + online OCSP
// (NT-22).  Validates that TlsOptions::revocation makes the client reject a
// server certificate listed in the CRL -- or reported REVOKED by an OCSP
// responder -- with a real error, while a valid certificate (and the same
// revoked certificate WITHOUT the revocation option) still completes the
// handshake.
//
//   Windows : Schannel  -- handshake proceeds (SECURITY_FLAG_IGNORE_REVOCATION
//                          skips online fetches), then CheckPeerRevocation()
//                          checks the peer serial against an in-memory CRL
//                          (Crl mode, and Ocsp mode with crlDer); Ocsp mode
//                          WITHOUT crlDer is ignored (documented limitation).
//   POSIX   : OpenSSL   -- Crl: X509_STORE(ca) + CRL with CRL_CHECK; the
//                          handshake fails with X509_V_ERR_CERT_REVOKED.
//                          Ocsp: after the TLS handshake an OCSP request is
//                          POSTed to the responder (explicit ocspUrl or peer
//                          AIA); REVOKED (or fetch/verify failure) rejects.
//
// Fixtures are generated per platform at runtime (CryptoAPI containers vs
// OpenSSL X509/X509_CRL/OCSP): a throwaway CA, one valid leaf, one revoked
// leaf (serial listed in the CRL), the CRL itself, and OCSP responses
// (GOOD for the valid leaf, REVOKED for the revoked leaf) -- the latter
// served by an in-process loopback HTTP responder on POSIX.
// Exit 0 + "[NET-REVOCATION-TEST] ALL PASS" means the gates pass.
#include <chaos/net/tls.h>
#include <chaos/net/socket_handle.h>
#include <chaos/net/net.h>

#include <atomic>
#include <chrono>
#include <cstdio>
#include <cstring>
#include <string>
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
#include <openssl/ocsp.h>
#endif

using chaos::net::NetError;
using chaos::net::NetAddress;
using chaos::net::SocketHandle;
using chaos::net::TlsProvider;
using chaos::net::TlsOptions;
using chaos::net::TlsCertificate;
using chaos::net::TlsMode;
using chaos::net::TlsRevocationOptions;
using chaos::net::TlsRevocationMode;
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

#define REV_CHECK(cond, what)                                                  \
    do {                                                                       \
        if (!(cond)) {                                                         \
            std::printf("[NET-REVOCATION-TEST] FAIL %s:%d %s\n", __FILE__,    \
                        __LINE__, what);                                        \
            ++g_failures;                                                       \
        }                                                                      \
    } while (0)

std::atomic<int> g_validPort{0};  // server presenting the VALID leaf
std::atomic<int> g_revPort{0};    // server presenting the REVOKED leaf

#if defined(_WIN32)
TlsProvider* CreateTestProvider() {
    return CreateSchannelProvider();
}
#else
TlsProvider* CreateTestProvider() {
    return CreateOpenSslProvider();
}
#endif

struct Fixture {
    TlsCertificate validLeaf{};    // server certificate (not revoked)
    TlsCertificate revokedLeaf{};  // server certificate (listed in the CRL)
    std::vector<unsigned char> caDer;   // DER of the issuing CA
    std::vector<unsigned char> crlDer;  // DER of the CRL (revokes revokedLeaf)
#if !defined(_WIN32)
    void* caX509 = nullptr;  // X509* of the CA (kept to build OCSP responses)
    void* caKey = nullptr;   // EVP_PKEY* of the CA
#endif
};

// ---------------------------------------------------------------------------
// Fixture generation (per platform).
// ---------------------------------------------------------------------------
#if defined(_WIN32)

static const BYTE kCaSerial[8] =     { 0, 0, 0, 0, 0, 0, 3, 0xE9 };  // 1001
static const BYTE kValidSerial[8] =  { 0, 0, 0, 0, 0, 0, 3, 0xEA };  // 1002
static const BYTE kRevokedSerial[8] ={ 0, 0, 0, 0, 0, 0, 3, 0xEB };  // 1003

FILETIME FileTimeFromNow(__int64 deltaSec) {
    SYSTEMTIME st = {};
    GetSystemTime(&st);
    FILETIME ft = {};
    SystemTimeToFileTime(&st, &ft);
    ULARGE_INTEGER ul = {};
    ul.LowPart = ft.dwLowDateTime;
    ul.HighPart = ft.dwHighDateTime;
    ul.QuadPart += deltaSec * 10000000LL;
    FILETIME out = {};
    out.dwLowDateTime = ul.LowPart;
    out.dwHighDateTime = ul.HighPart;
    return out;
}

bool EncName(const char* cn, std::vector<BYTE>* out) {
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
    DWORD len = 0;
    if (!CryptEncodeObject(X509_ASN_ENCODING, X509_NAME, &nameInfo, nullptr,
                           &len)) {
        return false;
    }
    out->resize(len);
    return CryptEncodeObject(X509_ASN_ENCODING, X509_NAME, &nameInfo,
                             out->data(), &len) != 0;
}

bool NewKeyContainer(const wchar_t* container, HCRYPTPROV* prov,
                     HCRYPTKEY* key) {
    if (!CryptAcquireContextW(prov, container, MS_ENH_RSA_AES_PROV_W,
                              PROV_RSA_AES, CRYPT_NEWKEYSET)) {
        if (GetLastError() == NTE_EXISTS) {
            if (!CryptAcquireContextW(prov, container, MS_ENH_RSA_AES_PROV_W,
                                      PROV_RSA_AES, 0)) {
                return false;
            }
        } else {
            return false;
        }
    }
    if (!CryptGenKey(*prov, AT_KEYEXCHANGE, CRYPT_EXPORTABLE | 0x08000000,
                     key)) {
        return false;
    }
    return true;
}

// Sign a certificate: the PUBLIC KEY comes from subjProv (the subject's own
// key container), the SIGNATURE from signProv (the issuer / CA).  For a
// self-signed CA pass signProv == subjProv and issuerBlob == subjectBlob.
PCCERT_CONTEXT MakeCert(HCRYPTPROV signProv, HCRYPTPROV subjProv,
                        const BYTE* serial, const std::vector<BYTE>& issuerBlob,
                        const std::vector<BYTE>& subjectBlob, FILETIME nb,
                        FILETIME na) {
    DWORD pubLen = 0;
    if (!CryptExportPublicKeyInfoEx(subjProv, AT_KEYEXCHANGE, X509_ASN_ENCODING,
                                    szOID_RSA_RSA, 0, nullptr, nullptr,
                                    &pubLen)) {
        return nullptr;
    }
    std::vector<BYTE> pubKeyInfo(pubLen);
    if (!CryptExportPublicKeyInfoEx(subjProv, AT_KEYEXCHANGE, X509_ASN_ENCODING,
                                    szOID_RSA_RSA, 0, nullptr,
                                    (CERT_PUBLIC_KEY_INFO*)pubKeyInfo.data(),
                                    &pubLen)) {
        return nullptr;
    }
    CERT_INFO ci = {};
    ci.dwVersion = CERT_V1;
    ci.SerialNumber.cbData = 8;
    ci.SerialNumber.pbData = const_cast<BYTE*>(serial);
    ci.SignatureAlgorithm.pszObjId = szOID_RSA_SHA256RSA;
    ci.Issuer.cbData = (DWORD)issuerBlob.size();
    ci.Issuer.pbData = const_cast<BYTE*>(issuerBlob.data());
    ci.NotBefore = nb;
    ci.NotAfter = na;
    ci.Subject.cbData = (DWORD)subjectBlob.size();
    ci.Subject.pbData = const_cast<BYTE*>(subjectBlob.data());
    ci.SubjectPublicKeyInfo = *(CERT_PUBLIC_KEY_INFO*)pubKeyInfo.data();
    DWORD len = 0;
    if (!CryptSignAndEncodeCertificate(signProv, AT_KEYEXCHANGE,
                                       X509_ASN_ENCODING, X509_CERT_TO_BE_SIGNED,
                                       &ci, &ci.SignatureAlgorithm, nullptr,
                                       nullptr, &len)) {
        return nullptr;
    }
    std::vector<BYTE> enc(len);
    if (!CryptSignAndEncodeCertificate(signProv, AT_KEYEXCHANGE,
                                       X509_ASN_ENCODING, X509_CERT_TO_BE_SIGNED,
                                       &ci, &ci.SignatureAlgorithm, nullptr,
                                       enc.data(), &len)) {
        return nullptr;
    }
    return CertCreateCertificateContext(X509_ASN_ENCODING, enc.data(), len);
}

bool BuildFixture(Fixture* f) {
    const unsigned long pid = (unsigned long)GetCurrentProcessId();
    wchar_t caC[64], vC[64], rC[64];
    std::swprintf(caC, 64, L"chaos-rev-ca-%lu", pid);
    std::swprintf(vC, 64, L"chaos-rev-v-%lu", pid);
    std::swprintf(rC, 64, L"chaos-rev-r-%lu", pid);

    HCRYPTPROV hCa = 0, hV = 0, hR = 0;
    HCRYPTKEY kCa = 0, kV = 0, kR = 0;
    if (!NewKeyContainer(caC, &hCa, &kCa) ||
        !NewKeyContainer(vC, &hV, &kV) ||
        !NewKeyContainer(rC, &hR, &kR)) {
        return false;
    }

    std::vector<BYTE> caName, vName, rName;
    if (!EncName("chaos-net-ca", &caName) ||
        !EncName("chaos-net-valid", &vName) ||
        !EncName("chaos-net-revoked", &rName)) {
        return false;
    }
    const FILETIME nb = FileTimeFromNow(-86400LL);
    const FILETIME na = FileTimeFromNow(3650LL * 86400LL);

    PCCERT_CONTEXT caCtx = MakeCert(hCa, hCa, kCaSerial, caName, caName, nb, na);
    PCCERT_CONTEXT vCtx =
        MakeCert(hCa, hV, kValidSerial, caName, vName, nb, na);
    PCCERT_CONTEXT rCtx =
        MakeCert(hCa, hR, kRevokedSerial, caName, rName, nb, na);
    if (caCtx == nullptr || vCtx == nullptr || rCtx == nullptr) {
        return false;
    }

    // Attach each leaf's own key container so Schannel can sign with it.
    CRYPT_KEY_PROV_INFO kpiV = {};
    kpiV.pwszContainerName = vC;
    kpiV.pwszProvName = MS_ENH_RSA_AES_PROV_W;
    kpiV.dwProvType = PROV_RSA_AES;
    kpiV.dwKeySpec = AT_KEYEXCHANGE;
    if (!CertSetCertificateContextProperty(vCtx, CERT_KEY_PROV_INFO_PROP_ID, 0,
                                           &kpiV)) {
        return false;
    }
    CRYPT_KEY_PROV_INFO kpiR = {};
    kpiR.pwszContainerName = rC;
    kpiR.pwszProvName = MS_ENH_RSA_AES_PROV_W;
    kpiR.dwProvType = PROV_RSA_AES;
    kpiR.dwKeySpec = AT_KEYEXCHANGE;
    if (!CertSetCertificateContextProperty(rCtx, CERT_KEY_PROV_INFO_PROP_ID, 0,
                                           &kpiR)) {
        return false;
    }

    // CRL listing the revoked leaf's serial, signed by the CA container.
    FILETIME thisUpd = FileTimeFromNow(-3600LL);
    FILETIME nextUpd = FileTimeFromNow(3650LL * 86400LL);
    FILETIME revDate = FileTimeFromNow(0);
    CRL_ENTRY entry = {};
    entry.SerialNumber.cbData = 8;
    entry.SerialNumber.pbData = const_cast<BYTE*>(kRevokedSerial);
    entry.RevocationDate = revDate;
    CRL_INFO crlInfo = {};
    crlInfo.SignatureAlgorithm.pszObjId = szOID_RSA_SHA256RSA;
    crlInfo.Issuer.cbData = (DWORD)caName.size();
    crlInfo.Issuer.pbData = const_cast<BYTE*>(caName.data());
    crlInfo.ThisUpdate = thisUpd;
    crlInfo.NextUpdate = nextUpd;
    crlInfo.cCRLEntry = 1;
    crlInfo.rgCRLEntry = &entry;
    DWORD clen = 0;
    if (!CryptSignAndEncodeCertificate(hCa, AT_KEYEXCHANGE, X509_ASN_ENCODING,
                                       X509_CERT_CRL_TO_BE_SIGNED, &crlInfo,
                                       &crlInfo.SignatureAlgorithm, nullptr,
                                       nullptr, &clen)) {
        return false;
    }
    std::vector<BYTE> crlEnc(clen);
    if (!CryptSignAndEncodeCertificate(hCa, AT_KEYEXCHANGE, X509_ASN_ENCODING,
                                       X509_CERT_CRL_TO_BE_SIGNED, &crlInfo,
                                       &crlInfo.SignatureAlgorithm, nullptr,
                                       crlEnc.data(), &clen)) {
        return false;
    }
    PCCRL_CONTEXT crlCtx =
        CertCreateCRLContext(X509_ASN_ENCODING, crlEnc.data(), clen);
    if (crlCtx == nullptr) {
        return false;
    }

    f->caDer.assign(caCtx->pbCertEncoded,
                    caCtx->pbCertEncoded + caCtx->cbCertEncoded);
    f->crlDer.assign(crlCtx->pbCrlEncoded,
                     crlCtx->pbCrlEncoded + crlCtx->cbCrlEncoded);
    f->validLeaf.native1 = (void*)vCtx;
    f->revokedLeaf.native1 = (void*)rCtx;

    CertFreeCRLContext(crlCtx);
    CertFreeCertificateContext(caCtx);
    // The leaf contexts stay owned by the fixture (FreeFixture frees them);
    // their key containers persist under the container names.
    CryptReleaseContext(hCa, 0);
    CryptReleaseContext(hV, 0);
    CryptReleaseContext(hR, 0);
    return true;
}

void FreeFixture(Fixture* f) {
    if (f->validLeaf.native1 != nullptr) {
        CertFreeCertificateContext((PCCERT_CONTEXT)f->validLeaf.native1);
        f->validLeaf.native1 = nullptr;
    }
    if (f->revokedLeaf.native1 != nullptr) {
        CertFreeCertificateContext((PCCERT_CONTEXT)f->revokedLeaf.native1);
        f->revokedLeaf.native1 = nullptr;
    }
}

#else  // OpenSSL

EVP_PKEY* GenRsaKey() {
    EVP_PKEY_CTX* pctx = EVP_PKEY_CTX_new_id(EVP_PKEY_RSA, nullptr);
    if (pctx == nullptr) {
        return nullptr;
    }
    EVP_PKEY* pkey = nullptr;
    const bool ok = EVP_PKEY_keygen_init(pctx) > 0 &&
                    EVP_PKEY_CTX_set_rsa_keygen_bits(pctx, 2048) > 0 &&
                    EVP_PKEY_keygen(pctx, &pkey) > 0;
    EVP_PKEY_CTX_free(pctx);
    return ok ? pkey : nullptr;
}

// If issuerName is null the certificate is self-signed.
X509* MakeCertSigned(EVP_PKEY* subjKey, EVP_PKEY* signKey, long serial,
                     const char* cn, X509_NAME* issuerName) {
    X509* x = X509_new();
    if (x == nullptr) {
        return nullptr;
    }
    X509_set_version(x, 2);
    {
        const bool isCa = (issuerName == nullptr);
        X509_EXTENSION* bc = X509V3_EXT_conf_nid(
            nullptr, nullptr, NID_basic_constraints,
            isCa ? (char*)"critical,CA:TRUE" : (char*)"critical,CA:FALSE");
        if (bc != nullptr) {
            X509_add_ext(x, bc, -1);
            X509_EXTENSION_free(bc);
        }
    }
    ASN1_INTEGER_set(X509_get_serialNumber(x), serial);
    X509_gmtime_adj(X509_get_notBefore(x), -86400L);
    X509_gmtime_adj(X509_get_notAfter(x), 3650L * 24L * 3600L);
    X509_set_pubkey(x, subjKey);
    X509_NAME* name = X509_get_subject_name(x);
    X509_NAME_add_entry_by_txt(name, "CN", MBSTRING_ASC,
                               (const unsigned char*)cn, -1, -1, 0);
    if (issuerName != nullptr) {
        X509_set_issuer_name(x, issuerName);
    } else {
        X509_set_issuer_name(x, name);
    }
    if (X509_sign(x, signKey, EVP_sha256()) <= 0) {
        X509_free(x);
        return nullptr;
    }
    return x;
}

std::vector<unsigned char> DerOfX509(X509* x) {
    const int n = i2d_X509(x, nullptr);
    std::vector<unsigned char> v(n > 0 ? (size_t)n : 0);
    if (n > 0) {
        unsigned char* p = v.data();
        i2d_X509(x, &p);
    }
    return v;
}

std::vector<unsigned char> DerOfCrl(X509_CRL* c) {
    const int n = i2d_X509_CRL(c, nullptr);
    std::vector<unsigned char> v(n > 0 ? (size_t)n : 0);
    if (n > 0) {
        unsigned char* p = v.data();
        i2d_X509_CRL(c, &p);
    }
    return v;
}

bool BuildFixture(Fixture* f) {
    EVP_PKEY* caKey = GenRsaKey();
    if (caKey == nullptr) {
        return false;
    }
    X509* ca = MakeCertSigned(caKey, caKey, 1001, "chaos-net-ca", nullptr);
    if (ca == nullptr) {
        EVP_PKEY_free(caKey);
        return false;
    }
    X509_NAME* caName = X509_get_subject_name(ca);  // owned by ca
    EVP_PKEY* vKey = GenRsaKey();
    EVP_PKEY* rKey = GenRsaKey();
    X509* v = (vKey != nullptr)
                  ? MakeCertSigned(vKey, caKey, 1002, "chaos-net-valid", caName)
                  : nullptr;
    X509* r = (rKey != nullptr)
                  ? MakeCertSigned(rKey, caKey, 1003, "chaos-net-revoked", caName)
                  : nullptr;
    if (v == nullptr || r == nullptr) {
        if (v != nullptr) X509_free(v);
        if (r != nullptr) X509_free(r);
        if (vKey != nullptr) EVP_PKEY_free(vKey);
        if (rKey != nullptr) EVP_PKEY_free(rKey);
        if (ca != nullptr) X509_free(ca);
        EVP_PKEY_free(caKey);
        return false;
    }

    X509_CRL* crl = X509_CRL_new();
    if (crl == nullptr) {
        return false;
    }
    X509_CRL_set_issuer_name(crl, caName);
    X509_gmtime_adj(X509_CRL_get_lastUpdate(crl), -3600L);
    X509_gmtime_adj(X509_CRL_get_nextUpdate(crl), 3650L * 24L * 3600L);
    X509_REVOKED* rev = X509_REVOKED_new();
    if (rev != nullptr) {
        ASN1_INTEGER_set(const_cast<ASN1_INTEGER*>(X509_REVOKED_get0_serialNumber(rev)), 1003);
        X509_gmtime_adj(const_cast<ASN1_TIME*>(X509_REVOKED_get0_revocationDate(rev)), 0);
        X509_CRL_add0_revoked(crl, rev);  // takes ownership
        X509_CRL_sort(crl);
    }
    if (X509_CRL_sign(crl, caKey, EVP_sha256()) <= 0) {
        X509_CRL_free(crl);
        X509_free(v);
        X509_free(r);
        EVP_PKEY_free(vKey);
        EVP_PKEY_free(rKey);
        X509_free(ca);
        EVP_PKEY_free(caKey);
        return false;
    }

    f->caDer = DerOfX509(ca);
    f->crlDer = DerOfCrl(crl);
    f->validLeaf.native1 = (void*)v;
    f->validLeaf.native2 = (void*)vKey;
    f->revokedLeaf.native1 = (void*)r;
    f->revokedLeaf.native2 = (void*)rKey;
    f->caX509 = (void*)ca;    // CA stays owned by the fixture (OCSP replies)
    f->caKey = (void*)caKey;

    X509_CRL_free(crl);
    return true;
}

void FreeFixture(Fixture* f) {
    if (f->validLeaf.native1 != nullptr) {
        X509_free((X509*)f->validLeaf.native1);
        f->validLeaf.native1 = nullptr;
    }
    if (f->validLeaf.native2 != nullptr) {
        EVP_PKEY_free((EVP_PKEY*)f->validLeaf.native2);
        f->validLeaf.native2 = nullptr;
    }
    if (f->revokedLeaf.native1 != nullptr) {
        X509_free((X509*)f->revokedLeaf.native1);
        f->revokedLeaf.native1 = nullptr;
    }
    if (f->revokedLeaf.native2 != nullptr) {
        EVP_PKEY_free((EVP_PKEY*)f->revokedLeaf.native2);
        f->revokedLeaf.native2 = nullptr;
    }
    if (f->caX509 != nullptr) {
        X509_free((X509*)f->caX509);
        f->caX509 = nullptr;
    }
    if (f->caKey != nullptr) {
        EVP_PKEY_free((EVP_PKEY*)f->caKey);
        f->caKey = nullptr;
    }
}

// ---------------------------------------------------------------------------
// NT-22: OCSP response builder + in-process loopback HTTP responder (POSIX).
// ---------------------------------------------------------------------------
// Builds the DER of a signed OCSP response for `leaf` (issued by `ca`):
// status V_OCSP_CERTSTATUS_GOOD or _REVOKED, signed by the CA key.  This is
// the authority the client s TlsRevocationOptions::ocspUrl points at.  The
// signer (the CA cert) is embedded so the verifier can find it; the client
// calls OCSP_basic_verify with OCSP_NOCHECKS, so the throwaway CA s missing
// OCSP-signing EKU is not an obstacle while the signature is still verified.
std::vector<unsigned char> BuildOcspResponse(X509* leaf, X509* ca,
                                             EVP_PKEY* caKey, int status) {
    std::vector<unsigned char> der;
    OCSP_CERTID* cid = OCSP_cert_to_id(EVP_sha1(), leaf, ca);
    if (cid == nullptr) {
        return der;
    }
    OCSP_BASICRESP* br = OCSP_BASICRESP_new();
    if (br == nullptr) {
        OCSP_CERTID_free(cid);
        return der;
    }
    ASN1_TIME* revTime = nullptr;
    ASN1_TIME* thisUpd = ASN1_TIME_new();
    ASN1_TIME* nextUpd = ASN1_TIME_new();
    if (status == V_OCSP_CERTSTATUS_REVOKED) {
        revTime = ASN1_TIME_new();
        if (revTime != nullptr) {
            X509_gmtime_adj(revTime, 0L);
        }
    }
    if (thisUpd != nullptr) {
        X509_gmtime_adj(thisUpd, -60L);
    }
    if (nextUpd != nullptr) {
        X509_gmtime_adj(nextUpd, 3600L * 2L);
    }
    OCSP_SINGLERESP* single = OCSP_basic_add1_status(br, cid, status,
                                                   OCSP_REVOKED_STATUS_CERTIFICATEHOLD,
                                                   revTime, thisUpd, nextUpd);
    if (single != nullptr &&
        OCSP_basic_sign(br, ca, caKey, EVP_sha256(), nullptr, 0) == 1) {
        OCSP_RESPONSE* resp =
            OCSP_response_create(OCSP_RESPONSE_STATUS_SUCCESSFUL, br);
        if (resp != nullptr) {
            const int n = i2d_OCSP_RESPONSE(resp, nullptr);
            if (n > 0) {
                der.resize(static_cast<size_t>(n));
                unsigned char* p = der.data();
                i2d_OCSP_RESPONSE(resp, &p);
            }
            OCSP_RESPONSE_free(resp);
        }
    }
    OCSP_CERTID_free(cid);
    OCSP_BASICRESP_free(br);
    if (revTime != nullptr) ASN1_TIME_free(revTime);
    if (thisUpd != nullptr) ASN1_TIME_free(thisUpd);
    if (nextUpd != nullptr) ASN1_TIME_free(nextUpd);
    return der;
}

// Index just past the end of a complete HTTP header block inside `req`, or
// req.size() if the terminator has not arrived yet.
size_t OspHeaderEnd(const std::vector<unsigned char>& req) {
    for (size_t j = 0; j + 3 < req.size(); ++j) {
        if (req[j] == '\r' && req[j + 1] == '\n' &&
            req[j + 2] == '\r' && req[j + 3] == '\n') {
            return j + 4;
        }
    }
    return req.size();
}

// Minimal loopback HTTP responder: reads the incoming OCSP POST until the
// header block and the Content-Length body have arrived, then answers 200
// with the prebuilt DER response.  Serves `connections` requests then closes.
void RunOcspResponder(std::atomic<int>* portOut,
                      const std::vector<unsigned char>& der, int connections) {
    SocketHandle srv = {};
    if (NetSocketCreate(2 /*Inet*/, 1 /*Stream*/, 6 /*Tcp*/, &srv) !=
        NetError::None) {
        REV_CHECK(false, "ocsp responder NetSocketCreate");
        return;
    }
    if (NetSocketBind(&srv, LoopbackV4(0)) != NetError::None ||
        NetSocketListen(&srv, 8) != NetError::None) {
        REV_CHECK(false, "ocsp responder bind/listen");
        NetSocketClose(&srv);
        return;
    }
    NetAddress local = {};
    if (NetSocketGetLocalAddress(&srv, &local) != NetError::None) {
        REV_CHECK(false, "ocsp responder GetLocalAddress");
        NetSocketClose(&srv);
        return;
    }
    portOut->store(local.port, std::memory_order_release);

    const std::string head = "HTTP/1.1 200 OK\r\n"
                             "Content-Type: application/ocsp-response\r\n"
                             "Content-Length: " + std::to_string(der.size()) +
                             "\r\nConnection: close\r\n\r\n";

    for (int i = 0; i < connections; ++i) {
        SocketHandle cli = {};
        NetAddress peer = {};
        if (NetSocketAccept(&srv, &cli, &peer, 10000) != NetError::None) {
            break;
        }
        std::vector<unsigned char> req;
        unsigned char chunk[1024];
        long cl = -1;
        bool headerDone = false;
        bool ok = true;
        while (ok) {
            CHAOS_IL2CPP_INT32 got = 0;
            const NetError e = NetSocketRecv(&cli, chunk,
                                             (CHAOS_IL2CPP_INT32)sizeof(chunk),
                                             0, &got, 10000);
            if (e != NetError::None) {
                ok = false;
                break;
            }
            req.insert(req.end(), chunk, chunk + got);
            if (!headerDone) {
                const size_t he = OspHeaderEnd(req);
                if (he < req.size()) {
                    headerDone = true;
                    std::string hdr((const char*)req.data(), he);
                    size_t pos = 0;
                    while (pos < hdr.size()) {
                        const size_t eol = hdr.find("\r\n", pos);
                        if (eol == std::string::npos) break;
                        const std::string line = hdr.substr(pos, eol - pos);
                        pos = eol + 2;
                        if (line.size() > 16 &&
                            line.compare(0, 16, "Content-Length:") == 0) {
                            cl = std::strtol(line.c_str() + 16, nullptr, 10);
                        }
                    }
                    if (cl < 0) cl = 0;
                }
            }
            if (headerDone && (long)req.size() >= (long)OspHeaderEnd(req) + cl) {
                break;
            }
        }
        if (ok && !der.empty()) {
            CHAOS_IL2CPP_INT32 sent = 0;
            NetSocketSend(&cli, (const CHAOS_IL2CPP_UINT8*)head.data(),
                          (CHAOS_IL2CPP_INT32)head.size(), 0, &sent, 10000);
            size_t off = 0;
            while (off < der.size()) {
                CHAOS_IL2CPP_INT32 bs = 0;
                const NetError be = NetSocketSend(&cli, der.data() + off,
                                                  (CHAOS_IL2CPP_INT32)(der.size() - off),
                                                  0, &bs, 10000);
                if (be != NetError::None || bs <= 0) break;
                off += (size_t)bs;
            }
        }
        NetSocketClose(&cli);
    }
    NetSocketClose(&srv);
}

#endif

// ---------------------------------------------------------------------------
// Servers (one TLS connection each or a small fixed count).
// ---------------------------------------------------------------------------
// expectAbortFirst: on connection 0 the client is EXPECTED to abort the
// handshake (it detected a revoked certificate).  OpenSSL aborts during the
// server-side handshake; Schannel completes the handshake and then the client
// closes without sending application data.  Both are tolerated.
void RunTlsServer(std::atomic<int>* portOut, const TlsCertificate& cert,
                  int connections, bool expectAbortFirst, int tolerateReadAt = -1) {
    SocketHandle srv = {};
    if (NetSocketCreate(2 /*Inet*/, 1 /*Stream*/, 6 /*Tcp*/, &srv) !=
        NetError::None) {
        REV_CHECK(false, "server NetSocketCreate");
        return;
    }
    NetAddress addr = LoopbackV4(0);
    if (NetSocketBind(&srv, addr) != NetError::None ||
        NetSocketListen(&srv, 8) != NetError::None) {
        REV_CHECK(false, "server bind/listen");
        NetSocketClose(&srv);
        return;
    }
    if (NetSocketGetLocalAddress(&srv, &addr) != NetError::None) {
        REV_CHECK(false, "server GetLocalAddress");
        NetSocketClose(&srv);
        return;
    }
    portOut->store(addr.port, std::memory_order_release);

    for (int i = 0; i < connections; ++i) {
        SocketHandle peer = {};
        if (NetSocketAccept(&srv, &peer, nullptr, 10000) != NetError::None) {
            REV_CHECK(false, "server accept");
            break;
        }
        TlsProvider* prov = CreateTestProvider();
        TlsOptions opts = {};
        opts.mode = TlsMode::Server;
        opts.serverCertificate = cert;
        opts.timeoutMs = 10000;
        bool skip = false;
        if (prov->Create(opts) != NetError::None) {
            REV_CHECK(false, "server Create");
            skip = true;
        } else {
            NetError he = prov->Handshake(&peer);
            const bool tAbort = (expectAbortFirst && i == 0) || (i == tolerateReadAt);
            if (he != NetError::None) {
                if (tAbort) {
                    std::fprintf(stderr,
                                 "[NET-REVOCATION-TEST] server conn %d handshake "
                                 "failed as expected: %s\n",
                                 i, prov->LastErrorDetail());
                    std::fflush(stderr);
                } else {
                    std::fprintf(stderr,
                                 "[NET-REVOCATION-TEST] server handshake failed: "
                                 "%s\n",
                                 prov->LastErrorDetail());
                    std::fflush(stderr);
                    REV_CHECK(false, "server Handshake");
                }
            } else if (tAbort) {
                // Schannel path: client finished its side then aborted on
                // revocation; expect EOF instead of tls-ping.
                unsigned char buf[16] = {};
                unsigned int got = 0;
                (void)prov->Read(&peer, buf, sizeof(buf), &got);
                std::fprintf(stderr,
                             "[NET-REVOCATION-TEST] server conn %d read %u bytes "
                             "after client abort (expected no tls-ping)\n",
                             i, got);
                std::fflush(stderr);
            } else {
                unsigned char buf[16] = {};
                unsigned int got = 0;
                if (prov->Read(&peer, buf, 9, &got) != NetError::None ||
                    got != 9 || std::memcmp(buf, "tls-ping", 9) != 0) {
                    REV_CHECK(false, "server Read tls-ping");
                } else {
                    unsigned int w = 0;
                    if (prov->Write(&peer, (const unsigned char*)"pong-tls", 8,
                                    &w) != NetError::None ||
                        w != 8) {
                        REV_CHECK(false, "server Write pong-tls");
                    }
                }
            }
        }
        if (!skip) {
            prov->Shutdown();
        }
        delete prov;
        NetSocketClose(&peer);
    }
    NetSocketClose(&srv);
}

// ---------------------------------------------------------------------------
// Client: one TLS client handshake + round-trip, with the given revocation
// options.  expectOk=true asserts success; expectOk=false asserts failure.
// Returns 1 on mismatch (also recorded via g_failures).
// ---------------------------------------------------------------------------
int RunClientOnce(int port, const TlsRevocationOptions& rev, bool expectOk,
                  const char* tag) {
    SocketHandle cli = {};
    if (NetSocketCreate(2, 1, 6, &cli) != NetError::None) {
        REV_CHECK(false, "client NetSocketCreate");
        return 1;
    }
    NetAddress addr = LoopbackV4(port);
    if (NetSocketConnect(&cli, addr, 10000) != NetError::None) {
        REV_CHECK(false, "client Connect");
        NetSocketClose(&cli);
        return 1;
    }

    TlsProvider* prov = CreateTestProvider();
    TlsOptions opts = {};
    opts.mode = TlsMode::Client;
    opts.serverName = "chaos-net-ca";
    opts.disableCertificateValidation = true;  // throwaway CA, not trusted
    opts.revocation = rev;
    opts.timeoutMs = 10000;

    NetError e = prov->Create(opts);
    if (e == NetError::None) {
        e = prov->Handshake(&cli);
    }
    int rc = 0;
    if (expectOk) {
        if (e != NetError::None) {
            std::fprintf(stderr,
                         "[NET-REVOCATION-TEST] client(%s) unexpected handshake "
                         "failure: %s\n",
                         tag, prov->LastErrorDetail());
            std::fflush(stderr);
            REV_CHECK(false, tag);
            rc = 1;
        } else {
            unsigned int w = 0;
            unsigned char rbuf[64] = {};
            unsigned int got = 0;
            if (prov->Write(&cli, (const unsigned char*)"tls-ping", 9, &w) !=
                    NetError::None ||
                w != 9) {
                REV_CHECK(false, tag);
                rc = 1;
            } else if (prov->Read(&cli, rbuf, 16, &got) != NetError::None ||
                       got != 8 || std::memcmp(rbuf, "pong-tls", 8) != 0) {
                REV_CHECK(false, tag);
                rc = 1;
            }
            prov->Shutdown();
        }
    } else {
        if (e == NetError::None) {
            std::fprintf(stderr,
                         "[NET-REVOCATION-TEST] client(%s) ACCEPTED a revoked "
                         "certificate\n",
                         tag);
            std::fflush(stderr);
            REV_CHECK(false, tag);
            prov->Shutdown();
            rc = 1;
        } else {
            std::fprintf(stderr,
                         "[NET-REVOCATION-TEST] client(%s) handshake failed as "
                         "expected: %s\n",
                         tag, prov->LastErrorDetail());
            std::fflush(stderr);
            const char* detail = prov->LastErrorDetail();
            if (detail == nullptr || std::strstr(detail, "revoked") == nullptr) {
                // "real error code" gate: the failure must be attributable to
                // revocation, not a generic I/O error.
                std::fprintf(stderr,
                             "[NET-REVOCATION-TEST] client(%s) error detail did "
                             "not mention revocation\n",
                             tag);
                std::fflush(stderr);
                REV_CHECK(false, tag);
                rc = 1;
            }
        }
    }
    delete prov;
    NetSocketClose(&cli);
    return rc;
}

}  // namespace

int main() {
    std::setvbuf(stdout, nullptr, _IONBF, 0);
    if (NetStartup() != NetError::None) {
        std::printf("[NET-REVOCATION-TEST] FAIL NetStartup\n");
        return 1;
    }

    Fixture fx = {};
    if (!BuildFixture(&fx) || fx.caDer.empty() || fx.crlDer.empty()) {
        std::printf("[NET-REVOCATION-TEST] FAIL BuildFixture\n");
        NetCleanup();
        return 1;
    }

    std::thread valid(RunTlsServer, &g_validPort, std::cref(fx.validLeaf), 2,
                      false, -1);
    std::thread revoked(RunTlsServer, &g_revPort, std::cref(fx.revokedLeaf),
#if !defined(_WIN32)
                        3,
#else
                        4,
#endif
                        true, 2);

    // Spin until both servers published their ports.
    int vp = 0, rp = 0;
    while ((vp = g_validPort.load(std::memory_order_acquire)) == 0 ||
           (rp = g_revPort.load(std::memory_order_acquire)) == 0) {
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }

    TlsRevocationOptions crl = {};
    crl.mode = TlsRevocationMode::Crl;
    crl.caDer = fx.caDer.data();
    crl.caLength = (CHAOS_IL2CPP_INT32)fx.caDer.size();
    crl.crlDer = fx.crlDer.data();
    crl.crlLength = (CHAOS_IL2CPP_INT32)fx.crlDer.size();

    int rc = 0;
    rc |= RunClientOnce(vp, crl, true, "valid+crl");            // 1 (valid conn0)
    rc |= RunClientOnce(rp, crl, false, "revoked+crl");         // 2 (revoked conn0)
    rc |= RunClientOnce(rp, TlsRevocationOptions{}, true, "revoked+nocrl");  // 3 (revoked conn1)

#if !defined(_WIN32)
    // POSIX: online OCSP via an in-process loopback HTTP responder.
    std::vector<unsigned char> ocspGoodDer = BuildOcspResponse(
        (X509*)fx.validLeaf.native1, (X509*)fx.caX509, (EVP_PKEY*)fx.caKey,
        V_OCSP_CERTSTATUS_GOOD);
    std::vector<unsigned char> ocspRevokedDer = BuildOcspResponse(
        (X509*)fx.revokedLeaf.native1, (X509*)fx.caX509, (EVP_PKEY*)fx.caKey,
        V_OCSP_CERTSTATUS_REVOKED);
    REV_CHECK(!ocspGoodDer.empty(), "build ocsp good response");
    REV_CHECK(!ocspRevokedDer.empty(), "build ocsp revoked response");

    std::atomic<int> gOcspGoodPort(0);
    std::atomic<int> gOcspRevPort(0);
    std::thread thOcspGood(RunOcspResponder, &gOcspGoodPort, std::cref(ocspGoodDer), 1);
    std::thread thOcspRev(RunOcspResponder, &gOcspRevPort, std::cref(ocspRevokedDer), 1);
    int ogp = 0, orp = 0;
    while ((ogp = gOcspGoodPort.load(std::memory_order_acquire)) == 0 ||
           (orp = gOcspRevPort.load(std::memory_order_acquire)) == 0) {
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
    }

    TlsRevocationOptions ocspGood = {};
    ocspGood.mode = TlsRevocationMode::Ocsp;
    ocspGood.caDer = fx.caDer.data();
    ocspGood.caLength = (CHAOS_IL2CPP_INT32)fx.caDer.size();
    std::string goodUrl = "http://127.0.0.1:" + std::to_string(ogp) + "/ocsp";
    ocspGood.ocspUrl = goodUrl.c_str();
    rc |= RunClientOnce(vp, ocspGood, true, "valid+ocsp");       // 4 (valid conn1)

    TlsRevocationOptions ocspRev = {};
    ocspRev.mode = TlsRevocationMode::Ocsp;
    ocspRev.caDer = fx.caDer.data();
    ocspRev.caLength = (CHAOS_IL2CPP_INT32)fx.caDer.size();
    std::string revUrl = "http://127.0.0.1:" + std::to_string(orp) + "/ocsp";
    ocspRev.ocspUrl = revUrl.c_str();
    rc |= RunClientOnce(rp, ocspRev, false, "revoked+ocsp");     // 5 (revoked conn2)

    thOcspGood.join();
    thOcspRev.join();
#else
    // Schannel cannot do an in-process online OCSP fetch; mode Ocsp with
    // crlDer falls back to the offline CRL check, without crlDer it is
    // ignored (documented limitation).
    TlsRevocationOptions ocspCrl = {};
    ocspCrl.mode = TlsRevocationMode::Ocsp;
    ocspCrl.caDer = fx.caDer.data();
    ocspCrl.caLength = (CHAOS_IL2CPP_INT32)fx.caDer.size();
    ocspCrl.crlDer = fx.crlDer.data();
    ocspCrl.crlLength = (CHAOS_IL2CPP_INT32)fx.crlDer.size();
    rc |= RunClientOnce(vp, ocspCrl, true, "valid+ocsp-crlf");     // 4 (valid conn1)
    rc |= RunClientOnce(rp, ocspCrl, false, "revoked+ocsp-crlf");  // 5 (revoked conn2)

    TlsRevocationOptions ocspIgn = {};
    ocspIgn.mode = TlsRevocationMode::Ocsp;
    rc |= RunClientOnce(rp, ocspIgn, true, "revoked+ocsp-ignore"); // 6 (revoked conn3)
#endif

    valid.join();
    revoked.join();

    FreeFixture(&fx);
    NetCleanup();

    if (g_failures == 0 && rc == 0) {
        std::printf("[NET-REVOCATION-TEST] ALL PASS\n");
        return 0;
    }
    std::printf("[NET-REVOCATION-TEST] FAILED (%d failures)\n", g_failures);
    return 1;
}