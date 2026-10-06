#pragma once
#include <chaos/native_types.h>

// ABI contract between generated NativeBody wrappers and chaos_net.
// Slot kinds mirror AotCoreIrAbiCarrierKind values used at registration
// time (Chaos.IL2CPP.Contracts): Void=0, Int32=1, NativeInt=2, ByRef=12.
namespace chaos::net {

enum class AbiSlotKind : CHAOS_IL2CPP_INT32 {
    Int32 = 1,     // 32-bit scalar slot
    NativeInt = 2, // pointer-sized slot (object refs, raw ptr, handles)
    ByRef = 12,    // pointer to caller slot (out/ref)
};

// System.Net.Sockets.SocketError numeric values. On Windows the values ARE
// the WSA error codes (WSAENOTCONN=10057 etc.), so native mapping is a
// near-identity; the posix remap lives in error.cpp alongside net_impl_posix.
enum class SocketError : CHAOS_IL2CPP_INT32 {
    Success = 0,
    Interrupted = 10004,
    AccessDenied = 10013,
    WouldBlock = 10035,
    InProgress = 10036,
    AlreadyInProgress = 10037,
    NotSocket = 10038,
    DestinationAddressRequired = 10039,
    MessageSize = 10040,
    ProtocolType = 10041,
    ProtocolOption = 10042,
    ProtocolNotSupported = 10043,
    SocketNotSupported = 10044,
    OperationNotSupported = 10045,
    ProtocolFamilyNotSupported = 10046,
    AddressFamilyNotSupported = 10047,
    AddressAlreadyInUse = 10048,
    AddressNotAvailable = 10049,
    NetworkDown = 10050,
    NetworkUnreachable = 10051,
    NetworkReset = 10052,
    ConnectionAborted = 10053,
    ConnectionReset = 10054,
    NoBufferSpaceAvailable = 10055,
    IsConnected = 10056,
    NotConnected = 10057,
    Shutdown = 10058,
    TooManyOpenSockets = 10059,
    TimedOut = 10060,
    ConnectionRefused = 10061,
    HostDown = 10064,
    HostUnreachable = 10065,
    ProcessLimit = 10067,
    SystemNotReady = 10091,
    VersionNotSupported = 10092,
    NotInitialized = 10093,
    Disconnecting = 10101,
    TypeNotFound = 10109,
    HostNotFound = 11001,
    TryAgain = 11002,
    NoRecovery = 11003,
    NoData = 11004,
};

inline CHAOS_IL2CPP_INT32 ToInt(SocketError e) noexcept {
    return static_cast<CHAOS_IL2CPP_INT32>(e);
}

}  // namespace chaos::net
