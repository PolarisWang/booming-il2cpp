// dbg_config.h — Debugger feature flag and configuration
//
// Controls whether IL-level debugging support is compiled in.
// When disabled (0), all debugger functions compile to no-ops with
// zero runtime cost.

#ifndef CHAOS_IL2CPP_DBG_CONFIG_H_
#define CHAOS_IL2CPP_DBG_CONFIG_H_

#ifndef CHAOS_IL2CPP_DEBUGGER
#define CHAOS_IL2CPP_DEBUGGER 1  // default: enabled
#endif

// Default port for TCP debugger transport.
static constexpr int kDbgDefaultPort = 56123;

// Buffer size for DAP message serialization (64 KB).
static constexpr int kDbgBufferSize = 65536;

// Maximum number of stack frames to return in a single stackTrace request.
static constexpr int kDbgMaxStackFrames = 256;

#endif  // CHAOS_IL2CPP_DBG_CONFIG_H_
