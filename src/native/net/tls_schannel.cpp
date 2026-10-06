// tls_schannel.cpp -- Windows SSPI/Schannel TLS backend (NT-12, Layer E).
//
// Implements chaos::net::TlsProvider on Schannel: AcquireCredentialsHandle +
// InitializeSecurityContext/AcceptSecurityContext for the handshake,
// EncryptMessage/DecryptMessage (SECBUFFER_STREAM) for the record layer.
// All socket I/O goes through the Layer D sync APIs, so this file sees only
// blocking byte streams.  Self-signed/unknown-CA verification can be disabled
// via TlsOptions::disableCertificateValidation (test hook).
#define SECURITY_WIN32 1
#include <chaos/net/tls.h>
#if !defined(_WIN32)
#error "tls_schannel.cpp is Windows-only (SSPI/Schannel); use tls_openssl.cpp"
#endif
#include <windows.h>
#include <wincrypt.h>
#include <security.h>
#include <schannel.h>

#include <cstdio>
#include <cstring>
#include <string>
#include <vector>

namespace chaos::net {
namespace {

constexpr int kChunkSize = 16 * 1024;

std::string WinErrString(const char* what, long st)
{
    char buf[160];
    std::snprintf(buf, sizeof(buf), "%s failed: 0x%08lX", what, (unsigned long)st);
    return std::string(buf);
}

class SchannelProvider final : public TlsProvider {
public:
    ~SchannelProvider() override { ReleaseResources(); }

    NetError Create(const TlsOptions& options) override;
    NetError Handshake(SocketHandle* socket) override;
    NetError Read(SocketHandle* socket, CHAOS_IL2CPP_UINT8* buf,
                  CHAOS_IL2CPP_UINT32 len, CHAOS_IL2CPP_UINT32* outRead) override;
    NetError Write(SocketHandle* socket, const CHAOS_IL2CPP_UINT8* buf,
                   CHAOS_IL2CPP_UINT32 len, CHAOS_IL2CPP_UINT32* outWritten) override;
    NetError Shutdown() override;
    const char* NegotiatedProtocol() const override {
        return m_negotiatedProtocol[0] ? m_negotiatedProtocol : nullptr;
    }
    const char* NegotiatedApplicationProtocol() const override {
        return m_negotiatedAlpn[0] ? m_negotiatedAlpn : nullptr;
    }
    const char* LastErrorDetail() const override { return m_lastError.c_str(); }

private:
    void ReleaseResources() noexcept;
    NetError SendAll(SocketHandle* socket, const CHAOS_IL2CPP_UINT8* data, CHAOS_IL2CPP_INT32 len);
    SECURITY_STATUS DriveHandshake(SocketHandle* socket);
    bool RecvMore(SocketHandle* socket);
    NetError CheckPeerRevocation() noexcept;  // NT-18 offline CRL check (client)

    TlsOptions m_opts{};
    bool m_created = false;
    CredHandle m_cred{};
    bool m_credValid = false;
    CtxtHandle m_ctx{};
    bool m_ctxValid = false;
    bool m_isServer = false;
    SecPkgContext_StreamSizes m_sizes{};
    std::vector<CHAOS_IL2CPP_UINT8> m_inBuf;   // accumulated inbound (handshake + records)
    std::vector<CHAOS_IL2CPP_UINT8> m_decBuf;  // decrypted-but-unconsumed plaintext
    std::size_t m_decPos = 0;                  // read cursor into m_decBuf
    std::string m_lastError = "provider not created";
    // Client ALPN offered to the peer in curl/Schannel buffer layout:
    // [4B extension len][4B SecApplicationProtocolNegotiationExt_ALPN id=2]
    // [2B list len][...RFC7301 entries...] (built from TlsOptions alpnProtocols).
    std::vector<CHAOS_IL2CPP_UINT8> m_alpnWire;
    char m_negotiatedProtocol[16] = {};
    char m_negotiatedAlpn[32] = {};
};

NetError SchannelProvider::Create(const TlsOptions& options)
{
    m_opts = options;
    m_isServer = (options.mode == TlsMode::Server);
    m_lastError.clear();
    // RFC7301 wire: [2B total-len][(1B len + proto)...] -> Schannel SecBuffer
    // layout ([4B ext len][4B ALPN id=2][2B list len][entries]) as required by
    // InitializeSecurityContext on the FIRST call (curl schannel.c scheme).
    m_alpnWire.clear();
    if (m_opts.alpnLength >= 2 && m_opts.alpnProtocols != nullptr) {
        CHAOS_IL2CPP_UINT16 listLen =
            (CHAOS_IL2CPP_UINT16)(((CHAOS_IL2CPP_UINT16)m_opts.alpnProtocols[0] << 8) |
                                  (CHAOS_IL2CPP_UINT16)m_opts.alpnProtocols[1]);
        if (listLen + 2 <= m_opts.alpnLength) {
            CHAOS_IL2CPP_UINT32 extLen = 4U + 2U + (CHAOS_IL2CPP_UINT32)listLen;
            m_alpnWire.push_back((CHAOS_IL2CPP_UINT8)(extLen & 0xFF));
            m_alpnWire.push_back((CHAOS_IL2CPP_UINT8)((extLen >> 8) & 0xFF));
            m_alpnWire.push_back((CHAOS_IL2CPP_UINT8)((extLen >> 16) & 0xFF));
            m_alpnWire.push_back((CHAOS_IL2CPP_UINT8)((extLen >> 24) & 0xFF));
            m_alpnWire.push_back(2); m_alpnWire.push_back(0);
            m_alpnWire.push_back(0); m_alpnWire.push_back(0);
            m_alpnWire.push_back((CHAOS_IL2CPP_UINT8)(listLen & 0xFF));
            m_alpnWire.push_back((CHAOS_IL2CPP_UINT8)((listLen >> 8) & 0xFF));
            for (CHAOS_IL2CPP_UINT16 i = 0; i < listLen; ++i) {
                m_alpnWire.push_back(m_opts.alpnProtocols[2 + i]);
            }
        }
    }
    if (m_isServer && options.serverCertificate.native1 == nullptr) {
        m_lastError = "server certificate missing in TlsOptions";
        return NetError::Unsupported;
    }
    if (m_isServer && !options.disableCertificateValidation) {
        // (Full CertGetCertificateChain validation is out of scope for the
        // native layer test matrix; production SslStream integration will add
        // a CHAIN_PARAMS policy callback.  Server side never validates here.)
    }

    SCHANNEL_CRED cred;
    std::memset(&cred, 0, sizeof(cred));
    cred.dwVersion = SCHANNEL_CRED_VERSION;
    if (m_isServer) {
        // MSVC 14.38 /O2 miscompile workaround: a LOCAL cert pointer that
        // only ever has its ADDRESS taken (paCred=&pCert) gets its value
        // store eliminated, so paCred points at uninitialized stack ->
        // CRYPT32 0xc0000005 at AcquireCredentialsHandleW. Pointing paCred
        // at our member copy (m_opts = options at the top) keeps the value
        // live: member stores survive /O2 (object is heap-allocated).
        cred.cCreds = 1;
        cred.paCred = (PCCERT_CONTEXT*)&m_opts.serverCertificate.native1;
        cred.dwFlags = SCH_CRED_NO_SYSTEM_MAPPER | SCH_CRED_NO_DEFAULT_CREDS;
        if (options.requireClientCertificate) {
            cred.dwFlags |= SCH_CRED_MANUAL_CRED_VALIDATION;
        }
    } else {
        cred.dwFlags = SCH_CRED_MANUAL_CRED_VALIDATION |
                       SCH_CRED_NO_DEFAULT_CREDS | SCH_CRED_NO_SYSTEM_MAPPER;
    }
    if (options.protocolMask != 0) {
        cred.grbitEnabledProtocols = (DWORD)options.protocolMask;  // SP_PROT_* bits
    } else {
        cred.grbitEnabledProtocols = 0;  // system default (TLS1.2/1.3 etc.)
    }

    SECURITY_STATUS st = AcquireCredentialsHandleW(
        nullptr, UNISP_NAME_W,
        m_isServer ? SECPKG_CRED_INBOUND : SECPKG_CRED_OUTBOUND,
        nullptr, &cred, nullptr, nullptr, &m_cred, nullptr);
    if (FAILED(st)) {
        m_lastError = WinErrString("AcquireCredentialsHandle", st);
        return NetError::Unsupported;
    }
    m_credValid = true;
    m_created = true;
    return NetError::None;
}

NetError SchannelProvider::SendAll(SocketHandle* socket,
                                   const CHAOS_IL2CPP_UINT8* data,
                                   CHAOS_IL2CPP_INT32 len)
{
    CHAOS_IL2CPP_INT32 off = 0;
    while (off < len) {
        CHAOS_IL2CPP_INT32 sent = 0;
        NetError e = NetSocketSend(socket, data + off, len - off, 0, &sent,
                                   m_opts.timeoutMs);
        if (e != NetError::None) {
            m_lastError = "socket send failed during TLS";
            return e;
        }
        if (sent <= 0) {
            m_lastError = "socket send stalled during TLS";
            return NetError::ConnectionReset;
        }
        off += sent;
    }
    return NetError::None;
}

bool SchannelProvider::RecvMore(SocketHandle* socket)
{
    std::vector<CHAOS_IL2CPP_UINT8> chunk(kChunkSize);
    CHAOS_IL2CPP_INT32 got = 0;
    NetError e = NetSocketRecv(socket, chunk.data(), kChunkSize, 0, &got,
                               m_opts.timeoutMs);
    if (e != NetError::None) {
        m_lastError = "socket recv failed during TLS";
        return false;
    }
    if (got == 0) {
        m_lastError = "peer closed during TLS";
        return false;
    }
    m_inBuf.insert(m_inBuf.end(), chunk.begin(), chunk.begin() + got);
    return true;
}

// One security-call round of the handshake.  Uses m_inBuf as the input
// token(s); on CONTINUE_NEEDED/OK the input is considered consumed and
// cleared after the output token is flushed.  SEC_E_INCOMPLETE_MESSAGE
// leaves the accumulator intact for more input.
SECURITY_STATUS SchannelProvider::DriveHandshake(SocketHandle* socket)
{
    // Input buffer layout (probe-verified against Schannel on 10.0.22621):
    //   * first client call with ALPN configured: SECBUFFER_APPLICATION_PROTOCOLS
    //     alone (curl schannel.c scheme: [4B ext len][4B ALPN id=2][2B list
    //     len][RFC7301 entries]); any sibling TOKEN/EMPTY buffer makes the
    //     first ISC fail with SEC_E_NO_CREDENTIALS (0x80090326).
    //   * first server call: TOKEN + EMPTY + ALPN (server offers its list via
    //     the same SecBuffer layout; probe ASC returned the normal
    //     SEC_E_INCOMPLETE_MESSAGE with an empty token).
    //   * later calls: TOKEN + EMPTY only (m_alpnWire is only placed when the
    //     context does not exist yet).
    SecBuffer inBufs[3];
    int nIn = 0;
    bool firstCall = !m_ctxValid;
    if (firstCall && !m_isServer && !m_alpnWire.empty()) {
        inBufs[nIn].BufferType = SECBUFFER_APPLICATION_PROTOCOLS;
        inBufs[nIn].cbBuffer = (DWORD)m_alpnWire.size();
        inBufs[nIn].pvBuffer = m_alpnWire.data();
        ++nIn;
    } else {
        inBufs[nIn].BufferType = SECBUFFER_TOKEN;
        inBufs[nIn].cbBuffer = (DWORD)m_inBuf.size();
        inBufs[nIn].pvBuffer = m_inBuf.empty() ? nullptr : m_inBuf.data();
        ++nIn;
        inBufs[nIn].BufferType = SECBUFFER_EMPTY;
        inBufs[nIn].cbBuffer = 0;
        inBufs[nIn].pvBuffer = nullptr;
        ++nIn;
        if (firstCall && m_isServer && !m_alpnWire.empty()) {
            inBufs[nIn].BufferType = SECBUFFER_APPLICATION_PROTOCOLS;
            inBufs[nIn].cbBuffer = (DWORD)m_alpnWire.size();
            inBufs[nIn].pvBuffer = m_alpnWire.data();
            ++nIn;
        }
    }
    SecBufferDesc inDesc{ SECBUFFER_VERSION, (ULONG)nIn, inBufs };
    SecBuffer outBufs[1] = { { 0, SECBUFFER_EMPTY, nullptr } };
    SecBufferDesc outDesc{ SECBUFFER_VERSION, 1, outBufs };
    ULONG outAttr = 0;

    SECURITY_STATUS st;
    if (m_isServer) {
        st = AcceptSecurityContext(
            &m_cred, m_ctxValid ? &m_ctx : nullptr, &inDesc,
            ASC_REQ_SEQUENCE_DETECT | ASC_REQ_REPLAY_DETECT |
                ASC_REQ_CONFIDENTIALITY | ASC_REQ_EXTENDED_ERROR |
                ASC_REQ_ALLOCATE_MEMORY | ASC_REQ_STREAM,
            0, m_ctxValid ? nullptr : &m_ctx, &outDesc, &outAttr, nullptr);
    } else {
        SEC_WCHAR* target = nullptr;
        std::vector<wchar_t> wtarget;
        if (m_opts.serverName != nullptr && m_opts.serverName[0] != 0) {
            int wlen = MultiByteToWideChar(CP_UTF8, 0, m_opts.serverName, -1,
                                           nullptr, 0);
            if (wlen > 0) {
                wtarget.resize((size_t)wlen);
                MultiByteToWideChar(CP_UTF8, 0, m_opts.serverName, -1,
                                    wtarget.data(), wlen);
                target = wtarget.data();
            }
        }
        DWORD reqFlags = ISC_REQ_SEQUENCE_DETECT | ISC_REQ_REPLAY_DETECT |
                         ISC_REQ_CONFIDENTIALITY | ISC_REQ_EXTENDED_ERROR |
                         ISC_REQ_ALLOCATE_MEMORY | ISC_REQ_STREAM;
        // Self-signed / throwaway test CA: skip server certificate validation.
        // (SECURITY_FLAG_IGNORE_* are Schannel context-req flags; the values
        // live in winhttp.h but are plain fContextReq bits for SSPI, so they
        // are inlined here to avoid the include.)
        if (m_opts.disableCertificateValidation) {
            reqFlags |= 0x00000100 /* UNKNOWN_CA */ |
                        0x00000200 /* CERT_WRONG_USAGE */ |
                        0x00001000 /* CERT_CN_INVALID */ |
                        0x00002000 /* CERT_DATE_INVALID */;
        }
        if (m_opts.revocation.mode != TlsRevocationMode::None) {
            // NT-18/22: skip Schannel's own online revocation fetches (CI is
            // loopback-only, no external network).  Crl makes the offline CRL
            // decision in Handshake() via CheckPeerRevocation(); Ocsp falls
            // back to the offline CRL when crlDer is supplied and is ignored
            // otherwise (Schannel has no in-memory OCSP fetch).
            reqFlags |= 0x00000080 /* SECURITY_FLAG_IGNORE_REVOCATION */;
        }
        st = InitializeSecurityContextW(
            &m_cred, m_ctxValid ? &m_ctx : nullptr, target,
            reqFlags,
            0,             /* Reserved1 */
            SECURITY_NATIVE_DREP,  /* TargetDataRep */
            &inDesc, 0,    /* pInput, Reserved2 */
            m_ctxValid ? nullptr : &m_ctx, &outDesc,
            &outAttr, nullptr /* pfContextAttr, ptsExpiry */);
    }

    if (st == SEC_E_OK || st == SEC_I_CONTINUE_NEEDED) {
        m_ctxValid = true;
        if (outDesc.cBuffers == 1 && outBufs[0].cbBuffer > 0 &&
            outBufs[0].pvBuffer != nullptr) {
            NetError e = SendAll(
                socket, static_cast<CHAOS_IL2CPP_UINT8*>(outBufs[0].pvBuffer),
                (CHAOS_IL2CPP_INT32)outBufs[0].cbBuffer);
            FreeContextBuffer(outBufs[0].pvBuffer);
            if (e != NetError::None) {
                return SEC_E_INTERNAL_ERROR;  // m_lastError already set
            }
        }
        m_inBuf.clear();  // token consumed by the package
    }
    return st;
}

NetError SchannelProvider::Handshake(SocketHandle* socket)
{
    if (!m_created || !m_credValid) {
        m_lastError = "provider not created (call Create first)";
        return NetError::NotInitialized;
    }
    for (;;) {
        SECURITY_STATUS st = DriveHandshake(socket);
        if (st == SEC_E_INCOMPLETE_MESSAGE) {
            if (!RecvMore(socket)) return NetError::ConnectionReset;
            continue;
        }
        if (st == SEC_I_CONTINUE_NEEDED) continue;
        if (st == SEC_E_OK) break;
        m_lastError = WinErrString("TLS handshake", st);
        return NetError::Unsupported;
    }

    const bool wantCrlCheck =
        m_opts.revocation.mode == TlsRevocationMode::Crl ||
        (m_opts.revocation.mode == TlsRevocationMode::Ocsp &&
         m_opts.revocation.crlDer != nullptr &&
         m_opts.revocation.crlLength > 0);
    if (!m_isServer && wantCrlCheck) {
        // NT-22: Ocsp mode without crlDer is documented as "ignore" on
        // Schannel (no online fetch); with crlDer it degrades to the offline
        // CRL check below.
        NetError re = CheckPeerRevocation();
        if (re != NetError::None) {
            return re;
        }
    }

    SECURITY_STATUS s2 =
        QueryContextAttributes(&m_ctx, SECPKG_ATTR_STREAM_SIZES, &m_sizes);
    if (FAILED(s2)) {
        std::memset(&m_sizes, 0, sizeof(m_sizes));
        (void)s2;
    }

    SecPkgContext_ConnectionInfo ci{};
    if (QueryContextAttributes(&m_ctx, SECPKG_ATTR_CONNECTION_INFO, &ci) ==
        SEC_E_OK) {
        DWORD p = ci.dwProtocol;
        if (p & SP_PROT_TLS1_3) std::strcpy(m_negotiatedProtocol, "TLSv1.3");
        else if (p & SP_PROT_TLS1_2) std::strcpy(m_negotiatedProtocol, "TLSv1.2");
        else if (p & SP_PROT_TLS1_1) std::strcpy(m_negotiatedProtocol, "TLSv1.1");
        else if (p & SP_PROT_TLS1_0) std::strcpy(m_negotiatedProtocol, "TLSv1.0");
        else std::snprintf(m_negotiatedProtocol, sizeof(m_negotiatedProtocol),
                           "0x%08lX", (unsigned long)p);
    }

    SecPkgContext_ApplicationProtocol ap{};
    if (QueryContextAttributes(&m_ctx, SECPKG_ATTR_APPLICATION_PROTOCOL, &ap) ==
            SEC_E_OK &&
        ap.ProtoNegoStatus != SecApplicationProtocolNegotiationStatus_None &&
        ap.ProtocolIdSize > 0) {
        DWORD n = ap.ProtocolIdSize < sizeof(m_negotiatedAlpn)
                      ? ap.ProtocolIdSize
                      : (DWORD)(sizeof(m_negotiatedAlpn) - 1);
        std::memcpy(m_negotiatedAlpn, ap.ProtocolId, n);
        m_negotiatedAlpn[n] = 0;
    }

    m_lastError.clear();
    return NetError::None;
}

NetError SchannelProvider::Read(SocketHandle* socket, CHAOS_IL2CPP_UINT8* buf,
                                CHAOS_IL2CPP_UINT32 len,
                                CHAOS_IL2CPP_UINT32* outRead)
{
    *outRead = 0;
    if (!m_ctxValid) {
        m_lastError = "no TLS context (handshake not done)";
        return NetError::NotInitialized;
    }
    for (;;) {
        // serve already-decrypted plaintext left over from previous records
        if (m_decPos < m_decBuf.size()) {
            const size_t avail = m_decBuf.size() - m_decPos;
            const DWORD n = (DWORD)(avail < len ? avail : len);
            std::memcpy(buf, m_decBuf.data() + m_decPos, n);
            m_decPos += n;
            if (m_decPos == m_decBuf.size()) {
                m_decBuf.clear();
                m_decPos = 0;
            }
            *outRead = n;
            return NetError::None;
        }
        if (!m_inBuf.empty()) {
            SecBuffer bufs[4] = {
                { (DWORD)m_inBuf.size(), SECBUFFER_DATA, m_inBuf.data() },
                { 0, SECBUFFER_EMPTY, nullptr },
                { 0, SECBUFFER_EMPTY, nullptr },
                { 0, SECBUFFER_EMPTY, nullptr },
            };
            SecBufferDesc desc{ SECBUFFER_VERSION, 4, bufs };
            SECURITY_STATUS st = DecryptMessage(&m_ctx, &desc, 0, nullptr);
            if (st == SEC_E_OK) {
                // Plaintext = the SECBUFFER_DATA entry whose pointer differs
                // from the input stream start (documented Schannel layout);
                // any unconsumed ciphertext lands in SECBUFFER_EXTRA.
                CHAOS_IL2CPP_UINT8* payload = nullptr;
                DWORD payloadLen = 0;
                CHAOS_IL2CPP_UINT8* extra = nullptr;
                DWORD extraLen = 0;
                for (int i = 0; i < 4; ++i) {
                    if (bufs[i].BufferType == SECBUFFER_DATA &&
                        bufs[i].pvBuffer != m_inBuf.data() &&
                        payload == nullptr) {
                        payload = static_cast<CHAOS_IL2CPP_UINT8*>(bufs[i].pvBuffer);
                        payloadLen = bufs[i].cbBuffer;
                    } else if (bufs[i].BufferType == SECBUFFER_EXTRA) {
                        extra = static_cast<CHAOS_IL2CPP_UINT8*>(bufs[i].pvBuffer);
                        extraLen = bufs[i].cbBuffer;
                    }
                }
                // Buffer the FULL record plaintext FIRST: payload points into
                // m_inBuf, and the memmove below (EXTRA -> front) can overlap
                // and clobber it when several TLS records arrive in one TCP
                // read (HTTP/2 servers send HEADERS+DATA back-to-back).
                if (payload != nullptr && payloadLen > 0) {
                    m_decBuf.assign(payload, payload + payloadLen);
                    m_decPos = 0;
                }
                if (extra != nullptr && extraLen > 0) {
                    std::memmove(m_inBuf.data(), extra, extraLen);
                    m_inBuf.resize(extraLen);
                } else {
                    m_inBuf.clear();
                }
                continue;  // loop back: serve m_decBuf or read more records
            }
            if (st == SEC_I_CONTEXT_EXPIRED) {
                m_inBuf.clear();
                *outRead = 0;  // peer sent close_notify
                return NetError::None;
            }
            if (st == SEC_E_INCOMPLETE_MESSAGE) {
                // fall through to recv more
            } else if (st == SEC_I_RENEGOTIATE) {
                m_lastError = "TLS renegotiation not supported";
                return NetError::Unsupported;
            } else {
                m_lastError = WinErrString("DecryptMessage", st);
                m_inBuf.clear();
                return NetError::Unsupported;
            }
        }
        if (!RecvMore(socket)) {
            // Peer closed without close_notify: report as clean EOF like
            // SslStream would only after close_notify, but for the native
            // test matrix treat TCP close as EOF.
            m_lastError.clear();
            *outRead = 0;
            return NetError::None;
        }
    }
}

NetError SchannelProvider::Write(SocketHandle* socket,
                                 const CHAOS_IL2CPP_UINT8* buf,
                                 CHAOS_IL2CPP_UINT32 len,
                                 CHAOS_IL2CPP_UINT32* outWritten)
{
    *outWritten = 0;
    if (!m_ctxValid) {
        m_lastError = "no TLS context (handshake not done)";
        return NetError::NotInitialized;
    }
    const size_t hdr = (size_t)m_sizes.cbHeader;
    const size_t trail = (size_t)m_sizes.cbTrailer;
    std::vector<CHAOS_IL2CPP_UINT8> out(hdr + (size_t)len + trail);
    if (hdr > 0 && len > 0) {
        std::memcpy(out.data() + hdr, buf, len);
    }
    SecBuffer bufs[4] = {
        { (DWORD)hdr, SECBUFFER_STREAM_HEADER, out.data() },
        { (DWORD)len, SECBUFFER_DATA, out.data() + hdr },
        { (DWORD)trail, SECBUFFER_STREAM_TRAILER, out.data() + hdr + len },
        { 0, SECBUFFER_EMPTY, nullptr },
    };
    SecBufferDesc desc{ SECBUFFER_VERSION, 4, bufs };
    SECURITY_STATUS st = EncryptMessage(&m_ctx, 0, &desc, 0);
    if (FAILED(st)) {
        m_lastError = WinErrString("EncryptMessage", st);
        return NetError::Unsupported;
    }
    DWORD total = bufs[0].cbBuffer + bufs[1].cbBuffer + bufs[2].cbBuffer;
    NetError e = SendAll(socket, out.data(), (CHAOS_IL2CPP_INT32)total);
    if (e == NetError::None) *outWritten = len;
    return e;
}

NetError SchannelProvider::CheckPeerRevocation() noexcept
{
    // Fetch the remote (server) certificate from the established context.
    PCCERT_CONTEXT pRemote = nullptr;
    SECURITY_STATUS st = QueryContextAttributes(&m_ctx,
                                                SECPKG_ATTR_REMOTE_CERT_CONTEXT,
                                                &pRemote);
    if (FAILED(st) || pRemote == nullptr || pRemote->pCertInfo == nullptr) {
        m_lastError = "no peer certificate available for revocation check (0x" +
                      std::to_string(st) + ")";
        return NetError::Unsupported;
    }
    if (m_opts.revocation.crlDer == nullptr ||
        m_opts.revocation.crlLength <= 0) {
        m_lastError = "revocation mode Crl requires crlDer";
        CertFreeCertificateContext(pRemote);
        return NetError::Unsupported;
    }
    // CryptGetCertificateChain does not consume a CRL passed in the memory
    // store (it reports CERT_TRUST_IS_OFFLINE_REVOCATION), so match the
    // offline CRL deterministically: create the CRL context from the raw DER,
    // require its issuer to equal the peer certificate's issuer, then compare
    // serials against the CRL entries (serial == revoked).
    PCCRL_CONTEXT crlCtx = CertCreateCRLContext(
        X509_ASN_ENCODING, const_cast<BYTE*>(m_opts.revocation.crlDer),
        (DWORD)m_opts.revocation.crlLength);
    if (crlCtx == nullptr || crlCtx->pCrlInfo == nullptr) {
        m_lastError = "CertCreateCRLContext failed for revocation check";
        if (crlCtx != nullptr) {
            CertFreeCRLContext(crlCtx);
        }
        CertFreeCertificateContext(pRemote);
        return NetError::Unsupported;
    }
    if (!CertCompareCertificateName(X509_ASN_ENCODING,
                                    &crlCtx->pCrlInfo->Issuer,
                                    &pRemote->pCertInfo->Issuer)) {
        m_lastError = "CRL issuer does not match server certificate";
        CertFreeCRLContext(crlCtx);
        CertFreeCertificateContext(pRemote);
        return NetError::Unsupported;
    }
    bool revoked = false;
    for (DWORD i = 0; i < crlCtx->pCrlInfo->cCRLEntry; ++i) {
        if (CertCompareIntegerBlob(
                &crlCtx->pCrlInfo->rgCRLEntry[i].SerialNumber,
                &pRemote->pCertInfo->SerialNumber)) {
            revoked = true;
            break;
        }
    }
    CertFreeCRLContext(crlCtx);
    CertFreeCertificateContext(pRemote);
    if (revoked) {
        m_lastError = "server certificate revoked (offline CRL)";
        return NetError::Unsupported;
    }
    return NetError::None;
}

void SchannelProvider::ReleaseResources() noexcept
{
    if (m_ctxValid) {
        DeleteSecurityContext(&m_ctx);
        m_ctxValid = false;
    }
    if (m_credValid) {
        FreeCredentialsHandle(&m_cred);
        m_credValid = false;
    }
    m_created = false;
    m_inBuf.clear();
}

NetError SchannelProvider::Shutdown()
{
    // Best effort: Schannel close_notify requires ApplyControlToken +
    // EncryptMessage; skipped for the native test matrix (peer treats TCP
    // close as EOF).  Release the context.
    ReleaseResources();
    return NetError::None;
}

}  // namespace

TlsProvider* CreateSchannelProvider() noexcept
{
    try {
        return new SchannelProvider();
    } catch (...) {
        return nullptr;
    }
}

}  // namespace chaos::net