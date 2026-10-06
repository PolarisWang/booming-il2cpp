#pragma once
#include <chaos/net/abi.h>

namespace chaos::net {

// Maps a native error code (WSAE*/errno) to the managed SocketError value.
// Windows base set is identity (WSA codes == SocketError values); posix
// errno remap is added alongside net_impl_posix in NT-4+.
SocketError NativeCodeToSocketError(CHAOS_IL2CPP_INT32 native_code) noexcept;

}  // namespace chaos::net
