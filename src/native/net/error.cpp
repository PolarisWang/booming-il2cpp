// Error mapping: native code -> managed SocketError.
#include <chaos/net/error.h>

#include <cerrno>

namespace chaos::net {

// ── Windows ───────────────────────────────────────────────────────────
// The winsock base set is identity: WSAE* values ARE the SocketError values
// (WSAENOTCONN == 10057 == SocketError.NotConnected).
#if defined(_WIN32)

SocketError NativeCodeToSocketError(CHAOS_IL2CPP_INT32 native_code) noexcept
{
    switch (native_code) {
        case 0:                                            return SocketError::Success;
        case static_cast<CHAOS_IL2CPP_INT32>(SocketError::Interrupted):              return SocketError::Interrupted;
        case static_cast<CHAOS_IL2CPP_INT32>(SocketError::AccessDenied):             return SocketError::AccessDenied;
        case static_cast<CHAOS_IL2CPP_INT32>(SocketError::WouldBlock):               return SocketError::WouldBlock;
        case static_cast<CHAOS_IL2CPP_INT32>(SocketError::InProgress):               return SocketError::InProgress;
        case static_cast<CHAOS_IL2CPP_INT32>(SocketError::AlreadyInProgress):        return SocketError::AlreadyInProgress;
        case static_cast<CHAOS_IL2CPP_INT32>(SocketError::NotSocket):                return SocketError::NotSocket;
        case static_cast<CHAOS_IL2CPP_INT32>(SocketError::DestinationAddressRequired): return SocketError::DestinationAddressRequired;
        case static_cast<CHAOS_IL2CPP_INT32>(SocketError::MessageSize):              return SocketError::MessageSize;
        case static_cast<CHAOS_IL2CPP_INT32>(SocketError::ProtocolType):             return SocketError::ProtocolType;
        case static_cast<CHAOS_IL2CPP_INT32>(SocketError::ProtocolOption):           return SocketError::ProtocolOption;
        case static_cast<CHAOS_IL2CPP_INT32>(SocketError::ProtocolNotSupported):     return SocketError::ProtocolNotSupported;
        case static_cast<CHAOS_IL2CPP_INT32>(SocketError::SocketNotSupported):       return SocketError::SocketNotSupported;
        case static_cast<CHAOS_IL2CPP_INT32>(SocketError::OperationNotSupported):    return SocketError::OperationNotSupported;
        case static_cast<CHAOS_IL2CPP_INT32>(SocketError::ProtocolFamilyNotSupported): return SocketError::ProtocolFamilyNotSupported;
        case static_cast<CHAOS_IL2CPP_INT32>(SocketError::AddressFamilyNotSupported): return SocketError::AddressFamilyNotSupported;
        case static_cast<CHAOS_IL2CPP_INT32>(SocketError::AddressAlreadyInUse):      return SocketError::AddressAlreadyInUse;
        case static_cast<CHAOS_IL2CPP_INT32>(SocketError::AddressNotAvailable):      return SocketError::AddressNotAvailable;
        case static_cast<CHAOS_IL2CPP_INT32>(SocketError::NetworkDown):              return SocketError::NetworkDown;
        case static_cast<CHAOS_IL2CPP_INT32>(SocketError::NetworkUnreachable):       return SocketError::NetworkUnreachable;
        case static_cast<CHAOS_IL2CPP_INT32>(SocketError::NetworkReset):             return SocketError::NetworkReset;
        case static_cast<CHAOS_IL2CPP_INT32>(SocketError::ConnectionAborted):        return SocketError::ConnectionAborted;
        case static_cast<CHAOS_IL2CPP_INT32>(SocketError::ConnectionReset):          return SocketError::ConnectionReset;
        case static_cast<CHAOS_IL2CPP_INT32>(SocketError::NoBufferSpaceAvailable):   return SocketError::NoBufferSpaceAvailable;
        case static_cast<CHAOS_IL2CPP_INT32>(SocketError::IsConnected):              return SocketError::IsConnected;
        case static_cast<CHAOS_IL2CPP_INT32>(SocketError::NotConnected):             return SocketError::NotConnected;
        case static_cast<CHAOS_IL2CPP_INT32>(SocketError::Shutdown):                 return SocketError::Shutdown;
        case static_cast<CHAOS_IL2CPP_INT32>(SocketError::TooManyOpenSockets):       return SocketError::TooManyOpenSockets;
        case static_cast<CHAOS_IL2CPP_INT32>(SocketError::TimedOut):                 return SocketError::TimedOut;
        case static_cast<CHAOS_IL2CPP_INT32>(SocketError::ConnectionRefused):        return SocketError::ConnectionRefused;
        case static_cast<CHAOS_IL2CPP_INT32>(SocketError::HostDown):                 return SocketError::HostDown;
        case static_cast<CHAOS_IL2CPP_INT32>(SocketError::HostUnreachable):          return SocketError::HostUnreachable;
        case static_cast<CHAOS_IL2CPP_INT32>(SocketError::ProcessLimit):             return SocketError::ProcessLimit;
        case static_cast<CHAOS_IL2CPP_INT32>(SocketError::SystemNotReady):           return SocketError::SystemNotReady;
        case static_cast<CHAOS_IL2CPP_INT32>(SocketError::VersionNotSupported):      return SocketError::VersionNotSupported;
        case static_cast<CHAOS_IL2CPP_INT32>(SocketError::NotInitialized):           return SocketError::NotInitialized;
        case static_cast<CHAOS_IL2CPP_INT32>(SocketError::Disconnecting):            return SocketError::Disconnecting;
        case static_cast<CHAOS_IL2CPP_INT32>(SocketError::TypeNotFound):             return SocketError::TypeNotFound;
        case static_cast<CHAOS_IL2CPP_INT32>(SocketError::HostNotFound):             return SocketError::HostNotFound;
        case static_cast<CHAOS_IL2CPP_INT32>(SocketError::TryAgain):                 return SocketError::TryAgain;
        case static_cast<CHAOS_IL2CPP_INT32>(SocketError::NoRecovery):               return SocketError::NoRecovery;
        case static_cast<CHAOS_IL2CPP_INT32>(SocketError::NoData):                   return SocketError::NoData;
        default:                                           return SocketError::OperationNotSupported;
    }
}

#else  // POSIX

// ── POSIX ─────────────────────────────────────────────────────────────
// errno -> WSA-coded SocketError (so Layer B fills the managed SocketError
// slot with the same values managed code knows).  Known WSA-coded inputs pass
// through unchanged, keeping the shared TestErrorTable green on both
// platforms; unknown codes fall back to OperationNotSupported.  if/else chain
// (not switch) because several errno macros share values on some platforms
// (EAGAIN==EWOULDBLOCK, EOPNOTSUPP==ENOTSUP, ESOCKTNOSUPPORT==EPROTONOSUPPORT).
SocketError NativeCodeToSocketError(CHAOS_IL2CPP_INT32 native_code) noexcept
{
    if (native_code == 0) return SocketError::Success;
    // WSA-coded identity pass-through — but only for codes that ARE managed
    // SocketError values (TestErrorTable expects 99999 -> OperationNotSupported).
    // (Errata: 10109 TypeNotFound is skipped — no errno form exists and the
    // win32 arm treats it the same way.)
    switch (native_code) {
        case 10004: case 10013: case 10035: case 10036: case 10037:
        case 10038: case 10039: case 10040: case 10041: case 10042:
        case 10043: case 10044: case 10045: case 10046: case 10047:
        case 10048: case 10049: case 10050: case 10051: case 10052:
        case 10053: case 10054: case 10055: case 10056: case 10057:
        case 10058: case 10059: case 10060: case 10061: case 10064:
        case 10065: case 10067: case 10091: case 10092: case 10093:
        case 10101: case 11001: case 11002: case 11003: case 11004:
            return static_cast<SocketError>(native_code);
        default:
            break;
    }

    if (native_code == EINTR)                       return SocketError::Interrupted;
    if (native_code == EACCES || native_code == EPERM) return SocketError::AccessDenied;
    if (native_code == EAGAIN || native_code == EWOULDBLOCK) return SocketError::WouldBlock;
    if (native_code == EINPROGRESS)                 return SocketError::InProgress;
    if (native_code == EALREADY)                    return SocketError::AlreadyInProgress;
    if (native_code == ENOTSOCK || native_code == EBADF) return SocketError::NotSocket;
    if (native_code == EDESTADDRREQ)                return SocketError::DestinationAddressRequired;
    if (native_code == EMSGSIZE)                    return SocketError::MessageSize;
    if (native_code == EPROTOTYPE)                  return SocketError::ProtocolType;
    if (native_code == ENOPROTOOPT)                 return SocketError::ProtocolOption;
    if (native_code == EPROTONOSUPPORT)             return SocketError::ProtocolNotSupported;
    if (native_code == ESOCKTNOSUPPORT)             return SocketError::SocketNotSupported;
    if (native_code == EOPNOTSUPP || native_code == ENOTSUP) return SocketError::OperationNotSupported;
    if (native_code == EPFNOSUPPORT)                return SocketError::ProtocolFamilyNotSupported;
    if (native_code == EAFNOSUPPORT)                return SocketError::AddressFamilyNotSupported;
    if (native_code == EADDRINUSE)                  return SocketError::AddressAlreadyInUse;
    if (native_code == EADDRNOTAVAIL)               return SocketError::AddressNotAvailable;
    if (native_code == ENETDOWN)                    return SocketError::NetworkDown;
    if (native_code == ENETUNREACH)                 return SocketError::NetworkUnreachable;
    if (native_code == ENETRESET)                   return SocketError::NetworkReset;
    if (native_code == ECONNABORTED)                return SocketError::ConnectionAborted;
    if (native_code == ECONNRESET)                  return SocketError::ConnectionReset;
    if (native_code == ENOBUFS)                     return SocketError::NoBufferSpaceAvailable;
    if (native_code == EISCONN)                     return SocketError::IsConnected;
    if (native_code == ENOTCONN)                    return SocketError::NotConnected;
    if (native_code == ESHUTDOWN)                   return SocketError::Shutdown;
    if (native_code == EMFILE || native_code == ENFILE) return SocketError::TooManyOpenSockets;
    if (native_code == ETIMEDOUT)                   return SocketError::TimedOut;
    if (native_code == ECONNREFUSED)                return SocketError::ConnectionRefused;
    if (native_code == EHOSTDOWN)                   return SocketError::HostDown;
    if (native_code == EHOSTUNREACH)                return SocketError::HostUnreachable;
    return SocketError::OperationNotSupported;
}

#endif  // defined(_WIN32)

}  // namespace chaos::net
