#ifndef CHAOS_IL2CPP_COM_CONNECTION_POINT_H_
#define CHAOS_IL2CPP_COM_CONNECTION_POINT_H_

#include <cstdint>
#include <chaos/native_types.h>

/// CHAOS_IL2CPP_GUID is a pointer to a 16-byte interface GUID (IID).
/// Used in COM connection point bridge functions.
#define CHAOS_IL2CPP_GUID const CHAOS_IL2CPP_UINT8*

namespace chaos::il2cpp::runtime_core {

// ── HRESULT constants ────────────────────────────────────────────────
static constexpr uint32_t kS_OK                    = 0x00000000u;
static constexpr uint32_t kS_FALSE                 = 0x00000001u;
static constexpr uint32_t kE_FAIL                  = 0x80004005u;
static constexpr uint32_t kE_POINTER               = 0x80004003u;
static constexpr uint32_t kE_INVALIDARG            = 0x80070057u;
static constexpr uint32_t kCONNECT_E_NOCONNECTION  = 0x80040200u;
static constexpr uint32_t kCONNECT_E_ADVISELIMIT   = 0x80040201u;

// ── Data structures ───────────────────────────────────────────────────

/// A single connection between a COM event source and a managed delegate sink.
/// The sink_unknown is a CCW (COM Callable Wrapper) that wraps a managed
/// delegate and exposes it as IUnknown.  The cookie is a unique identifier
/// used by the event source to disconnect.
struct ConnectionPoint {
    void*            sink_unknown;   // the event sink (CCW wrapping a managed delegate)
    uint32_t         cookie;         // unique connection cookie
    ConnectionPoint* next;           // linked list next (nullptr = end)
};

/// Per-interface connection point list (one IConnectionPoint for one IID).
struct ConnectionPointList {
    const CHAOS_IL2CPP_UINT8* iid;       // source interface IID (16 bytes)
    ConnectionPoint*           head;      // linked list of connections
    uint32_t                   next_cookie; // monotonically increasing cookie counter
};

/// Container holds all connection point lists for a CCW.
/// Allocated lazily on the first Advise call.
struct ConnectionPointContainer {
    static constexpr uint32_t kMaxSourceIfs = 8;
    ConnectionPointList lists[kMaxSourceIfs];
    uint32_t            list_count;
};

/// Forward declaration from com_ccw.h.
struct ComCcw;

// ── Bridge function declarations ─────────────────────────────────────

/// Check whether a CCW supports the specified source interface IID.
/// Returns S_OK (0) if supported, CONNECT_E_NOCONNECTION otherwise.
uint32_t ComFindConnectionPoints(
    CHAOS_IL2CPP_INTPTR ccw_handle,
    CHAOS_IL2CPP_GUID   iid) noexcept;

/// Register a sink (CCW wrapping a delegate) for a specific event source
/// interface IID on a CCW.  Allocates a ConnectionPoint, assigns a unique
/// cookie, and links it into the ConnectionPointList.
/// Returns S_OK on success, CONNECT_E_ADVISELIMIT if the list is full.
uint32_t ComAdvise(
    CHAOS_IL2CPP_INTPTR  ccw_handle,
    CHAOS_IL2CPP_GUID    iid,
    CHAOS_IL2CPP_INTPTR  sink_unknown,
    uint32_t*            cookie) noexcept;

/// Remove a connection by cookie from the ConnectionPointList.
/// Frees the ConnectionPoint.
/// Returns S_OK on success, CONNECT_E_NOCONNECTION if cookie not found.
uint32_t ComUnadvise(
    CHAOS_IL2CPP_INTPTR ccw_handle,
    CHAOS_IL2CPP_GUID   iid,
    uint32_t            cookie) noexcept;

/// Create a CCW that wraps a managed delegate pointer as an IUnknown.
/// This allows passing a managed delegate as an event sink to COM.
/// Returns the CCW pointer (IUnknown*) as IntPtr, or 0 on failure.
CHAOS_IL2CPP_INTPTR ComCreateEventSinkCcw(
    CHAOS_IL2CPP_INTPTR delegate_ptr,
    CHAOS_IL2CPP_GUID   event_iid) noexcept;

}  // namespace chaos::il2cpp::runtime_core

#endif  // CHAOS_IL2CPP_COM_CONNECTION_POINT_H_
