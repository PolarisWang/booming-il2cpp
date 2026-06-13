#ifndef CHAOS_IL2CPP_JIT_DATA_LOADER_H_
#define CHAOS_IL2CPP_JIT_DATA_LOADER_H_

#include <cstddef>
#include <cstdint>

// ── JIT Data Loader ─────────────────────────────────────────────────────
// Loads AotCoreIr JSON data from a .jdata binary file at runtime.
// The .jdata file contains concatenated UTF-8 JSON blobs, one per JitEntry.
// JitEntry::json_offset + JitEntry::json_len together provide the framing.
//
// On Windows: uses CreateFileMapping + MapViewOfFile
// On Linux/macOS: uses mmap
//
// The returned pointer must be kept alive for the program lifetime.
// Caller should call ChaosJitDataUnload() during shutdown (or leave it
// to the OS to clean up on process exit).

/// Load a .jdata file and return a pointer to its content in memory.
/// Returns nullptr on failure (file not found, empty, or mapping error).
/// The returned memory is read-only. The data is valid until
/// ChaosJitDataUnload() is called (or the program exits).
extern "C" void* ChaosJitDataLoad(const char* filepath,
                                  uint64_t* out_size = nullptr) noexcept;

/// Unload a .jdata file previously loaded by ChaosJitDataLoad().
/// Safe to call with nullptr (no-op). After calling, the returned pointer
/// from ChaosJitDataLoad is invalid and must not be accessed.
extern "C" void ChaosJitDataUnload(void* data) noexcept;

#endif // CHAOS_IL2CPP_JIT_DATA_LOADER_H_
