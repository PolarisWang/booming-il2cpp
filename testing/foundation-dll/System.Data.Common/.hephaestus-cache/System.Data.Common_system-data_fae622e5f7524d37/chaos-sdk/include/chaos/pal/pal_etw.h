// pal_etw.h — Event Tracing for Windows abstraction
//
// On Win32: delegates to the real ETW API (EventRegister/EventWrite/EventUnregister).
// On POSIX: all operations are no-ops (provider handle always returns nullptr from
//           PalEtwRegister, and write calls are silently dropped).
#pragma once

#include <cstdint>

namespace chaos::il2cpp::pal {

/// Opaque handle for an ETW provider registration.
/// On Win32: wraps a REGHANDLE.
/// On POSIX: always nullptr (no-ops).
struct PalEtwProvider;

/// Register an ETW provider with a 16-byte GUID.
/// On Win32: calls EventRegister.
/// On POSIX: returns nullptr (no-op).
/// @param provider_id  16-byte GUID (binary, not string).
/// @returns Pointer to provider handle, or nullptr on failure/unsupported.
PalEtwProvider* PalEtwRegister(const uint8_t provider_id[16]) noexcept;

/// Unregister a previously registered ETW provider.
/// Safe to call with nullptr (no-op).
void PalEtwUnregister(PalEtwProvider* provider) noexcept;

/// Check whether the provider is enabled (has active consumers).
/// On Win32: checks if the provider has any registered event consumers.
/// On POSIX: always returns false.
bool PalEtwIsEnabled(PalEtwProvider* provider) noexcept;

/// Write an event to the ETW provider.
/// On Win32: calls EventWrite with the given descriptor and data.
/// On POSIX: no-op (returns true silently).
/// @param provider     Provider handle from PalEtwRegister.
/// @param event_id     Event ID (descriptor Id field).
/// @param version      Event version (descriptor Version field).
/// @param channel      Event channel (descriptor Channel field).
/// @param level        Event level (descriptor Level field) — 4 = Informational.
/// @param keyword      Event keyword bitmask.
/// @param data         Pointer to event payload data.
/// @param data_len     Size of event payload in bytes.
/// @returns true on success (always true on POSIX).
bool PalEtwWrite(PalEtwProvider* provider,
                 uint8_t event_id, uint8_t version, uint8_t channel,
                 uint8_t level, uint64_t keyword,
                 const void* data, uint32_t data_len) noexcept;

}  // namespace chaos::il2cpp::pal
