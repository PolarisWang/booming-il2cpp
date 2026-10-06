// Layer E -- TLS transport (NT-12).  System.Net.Security.SslStream is absent
// from the AOT'd BCL, so the native layer must cover TLS.  Architecture
// (docs/dev/in-progress/net-cpp-architecture.md, section 8): TlsProvider
// isolates the two platform backends -- Windows Schannel/SSPI
// (tls_schannel.cpp) and OpenSSL (tls_openssl.cpp, linux/android; also
// compiles under MSYS2 for local POSIX verification).
//
// The provider wraps an already-connected (but not yet TLS'd) chaos::net
// socket; all I/O goes through the Layer D sync APIs (NetSocketRecv/Send),
// which block on the non-blocking fd via poll with a timeout, so the TLS
// backends see nothing but blocking byte streams (memory BIOs on OpenSSL,
// SECBUFFER tokens on Schannel).  Handshake/Read/Write are synchronous with
// options.timeoutMs.  (Divergence from section 8
// Handshake(SocketHandle*, AsyncOp**) -- async TLS streaming remains for the
// future SslStream integration; this sync surface is what the native test
// matrix pins.)
#pragma once
#include <chaos/net/net_api.h>
#include <cstdint>

namespace chaos::net {

enum class TlsMode : CHAOS_IL2CPP_INT32 {
    Client = 0,
    Server = 1,
};

// Backend-specific certificate/key handles (owned by the caller, valid for
// the lifetime of TlsOptions/Create):
//   Schannel : native1 = PCCERT_CONTEXT, native2 = unused
//   OpenSSL  : native1 = X509*,         native2 = EVP_PKEY*
struct TlsCertificate {
    void* native1 = nullptr;
    void* native2 = nullptr;
};

// NT-18/22: certificate revocation.
//   Crl  -- offline CRL mode.  CI is loopback-only (no external network), so
//           the caller supplies a DER-encoded CRL plus the issuer CA and the
//           provider checks the peer certificate against them during the
//           handshake.  OpenSSL: X509_STORE with X509_V_FLAG_CRL_CHECK
//           (| CRL_CHECK_ALL); Schannel: SECURITY_FLAG_IGNORE_REVOCATION
//           (skip Schannel's online checks) then CheckPeerRevocation() against
//           a memory CRL seeded from crlDer.
//   Ocsp -- online OCSP mode (NT-22).  OpenSSL: after the handshake the peer
//           certificate is checked against the responder named by ocspUrl (or,
//           when null, the OCSP URL from the peer certificate's AIA extension)
//           over a plain HTTP POST; a non-revoked (GOOD) response passes, any
//           REVOKED response or fetch/verification failure fails the
//           handshake.  Schannel: no online OCSP fetch is issued -- the mode
//           falls back to the offline CRL check when crlDer is supplied and is
//           otherwise silently ignored (documented Schannel limitation).
enum class TlsRevocationMode : CHAOS_IL2CPP_INT32 {
    None = 0,  // default: no revocation checking
    Crl = 1,   // check peer against the supplied offline CRL
    Ocsp = 2,  // online OCSP (OpenSSL) / CRL fallback-else-ignore (Schannel)
};

struct TlsRevocationOptions {
    TlsRevocationMode mode = TlsRevocationMode::None;
    const CHAOS_IL2CPP_UINT8* crlDer = nullptr;  // DER-encoded X.509 CRL
    CHAOS_IL2CPP_INT32 crlLength = 0;
    const CHAOS_IL2CPP_UINT8* caDer = nullptr;   // DER-encoded issuer CA cert
    CHAOS_IL2CPP_INT32 caLength = 0;
    const char* ocspUrl = nullptr;  // OCSP responder "http://host[:port][/path]"
                                    // (Ocsp mode; nullptr = peer AIA fallback)
};

struct TlsOptions {
    TlsMode mode = TlsMode::Client;
    const char* serverName = nullptr;           // SNI + client validation target
    CHAOS_IL2CPP_INT32 protocolMask = 0;        // 0 = system/OpenSSL default
    const CHAOS_IL2CPP_UINT8* alpnProtocols = nullptr;  // RFC7301 wire format
                                                // (2-byte total len + entries);
                                                // nullptr = no ALPN
    CHAOS_IL2CPP_UINT32 alpnLength = 0;
    bool requireClientCertificate = false;      // server mode (best effort)
    bool disableCertificateValidation = false;  // test hook (self-signed)
    TlsCertificate serverCertificate{};         // server mode only
    TlsRevocationOptions revocation{};          // client mode revocation check
    CHAOS_IL2CPP_INT32 timeoutMs = 10000;
};

struct TlsProvider {
    virtual ~TlsProvider() = default;
    virtual NetError Create(const TlsOptions& options) = 0;
    // TLS handshake over an already-connected socket (blocking).
    virtual NetError Handshake(SocketHandle* socket) = 0;
    // Reads up to len decrypted bytes.  *outRead == 0 means the peer sent
    // close_notify (or closed); all errors (incl. timeout) return a NetError.
    virtual NetError Read(SocketHandle* socket, CHAOS_IL2CPP_UINT8* buf,
                          CHAOS_IL2CPP_UINT32 len, CHAOS_IL2CPP_UINT32* outRead) = 0;
    virtual NetError Write(SocketHandle* socket, const CHAOS_IL2CPP_UINT8* buf,
                           CHAOS_IL2CPP_UINT32 len, CHAOS_IL2CPP_UINT32* outWritten) = 0;
    // Best-effort close_notify + releases the TLS context (the socket itself
    // is closed by the caller).
    virtual NetError Shutdown() = 0;
    virtual const char* NegotiatedProtocol() const = 0;           // "TLSv1.2"/"TLSv1.3"/nullptr
    virtual const char* NegotiatedApplicationProtocol() const = 0; // nullptr if none
    virtual const char* LastErrorDetail() const = 0;              // human-readable last failure
protected:
    TlsProvider() = default;
};

// Backend factory; each returns a provider for one connection.  The other
// backend's file is not compiled (CMake picks per platform).
TlsProvider* CreateSchannelProvider() noexcept;
TlsProvider* CreateOpenSslProvider() noexcept;

}  // namespace chaos::net
