#include <cstdlib>
#include <cstring>
#include <chaos/log.h>
#include <chaos/native_types.h>
#include <runtime_abi.h>
#include "com_ccw.h"
#include "runtime_core.h"
#include "com_connection_point.h"
#include "memory_domain.h"

namespace chaos::il2cpp::runtime_core {

// ── Helper: find or create a ConnectionPointList for a given IID ─────

/// Find a ConnectionPointList for the given interface IID.
/// Returns the index into cp_container->lists[], or -1 if not found.
static int32_t FindListByIid(ConnectionPointContainer* container, CHAOS_IL2CPP_GUID iid) noexcept {
    for (uint32_t i = 0; i < container->list_count; ++i) {
        if (CHAOS_IL2CPP_MEMCMP(iid, container->lists[i].iid, 16) == 0) {
            return static_cast<int32_t>(i);
        }
    }
    return -1;
}

/// Ensure a ConnectionPointList exists for the given IID.
/// Returns the index into cp_container->lists[], or -1 if the container is full.
static int32_t EnsureList(ConnectionPointContainer* container, CHAOS_IL2CPP_GUID iid) noexcept {
    // Try to find an existing list first.
    int32_t idx = FindListByIid(container, iid);
    if (idx >= 0) return idx;

    // No existing list — create one if there is room.
    if (container->list_count >= ConnectionPointContainer::kMaxSourceIfs) {
        return -1;
    }

    uint32_t slot = container->list_count++;
    container->lists[slot].iid = iid;
    container->lists[slot].head = nullptr;
    container->lists[slot].next_cookie = 1;  // start at 1 (0 is reserved)
    return static_cast<int32_t>(slot);
}

// ── Bridge function implementations ─────────────────────────────────

uint32_t ComFindConnectionPoints(
    CHAOS_IL2CPP_INTPTR ccw_handle,
    CHAOS_IL2CPP_GUID   iid) noexcept {
    if (ccw_handle == 0 || iid == nullptr) {
        return kE_POINTER;
    }

    auto* ccw = reinterpret_cast<chaos::il2cpp::com_ccw::ComCcw*>(
        reinterpret_cast<void*>(ccw_handle));

    // Scan the CCW's registered interfaces for a match against the
    // requested source interface IID.
    for (CHAOS_IL2CPP_SIZE i = 0; i < ccw->interface_count; ++i) {
        if (ccw->interfaces[i].guid != nullptr &&
            CHAOS_IL2CPP_MEMCMP(iid, ccw->interfaces[i].guid, 16) == 0) {
            return kS_OK;
        }
    }

    CHAOS_IL2CPP_LOG_DEBUG_M("COM", "CCW {0}: no connection point for IID",
                              reinterpret_cast<void*>(ccw));
    return kCONNECT_E_NOCONNECTION;
}

uint32_t ComAdvise(
    CHAOS_IL2CPP_INTPTR  ccw_handle,
    CHAOS_IL2CPP_GUID    iid,
    CHAOS_IL2CPP_INTPTR  sink_unknown,
    uint32_t*            cookie) noexcept {
    if (ccw_handle == 0 || iid == nullptr || sink_unknown == 0 || cookie == nullptr) {
        return kE_POINTER;
    }

    auto* ccw = reinterpret_cast<chaos::il2cpp::com_ccw::ComCcw*>(
        reinterpret_cast<void*>(ccw_handle));

    // Lazily allocate the ConnectionPointContainer on first Advise.
    if (ccw->cp_container == nullptr) {
        auto* container = static_cast<ConnectionPointContainer*>(
            memory_domain::DomainCurrentAllocateTagged(sizeof(ConnectionPointContainer)));
        if (container == nullptr) {
            return kE_FAIL;
        }
        container->list_count = 0;
        ccw->cp_container = container;

        CHAOS_IL2CPP_LOG_DEBUG_M("COM", "CCW {0}: allocated ConnectionPointContainer",
                                  reinterpret_cast<void*>(ccw));
    }

    // Ensure a ConnectionPointList exists for this IID.
    int32_t list_idx = EnsureList(ccw->cp_container, iid);
    if (list_idx < 0) {
        CHAOS_IL2CPP_LOG_WARN_M("COM", "CCW {0}: connection point list full for IID",
                                  reinterpret_cast<void*>(ccw));
        return kCONNECT_E_ADVISELIMIT;
    }

    auto& list = ccw->cp_container->lists[list_idx];

    // Allocate a new ConnectionPoint node.
    auto* cp = static_cast<ConnectionPoint*>(
        memory_domain::DomainCurrentAllocateTagged(sizeof(ConnectionPoint)));
    if (cp == nullptr) {
        return kE_FAIL;
    }

    cp->sink_unknown = reinterpret_cast<void*>(sink_unknown);
    cp->cookie = list.next_cookie++;
    cp->next = list.head;
    list.head = cp;

    *cookie = cp->cookie;

    // AddRef the sink to keep it alive while connected.
    // The sink is a CCW, so its vtable starts with IUnknown methods.
    if (cp->sink_unknown != nullptr) {
        auto** vtbl = static_cast<chaos::il2cpp::com_ccw::ComCcwVtbl**>(cp->sink_unknown);
        (*vtbl)->AddRef(cp->sink_unknown);
    }

    CHAOS_IL2CPP_LOG_DEBUG_M("COM", "CCW {0}: Advise cookie={1} for IID",
                              reinterpret_cast<void*>(ccw), cp->cookie);
    return kS_OK;
}

uint32_t ComUnadvise(
    CHAOS_IL2CPP_INTPTR ccw_handle,
    CHAOS_IL2CPP_GUID   iid,
    uint32_t            cookie) noexcept {
    if (ccw_handle == 0 || iid == nullptr) {
        return kE_POINTER;
    }

    auto* ccw = reinterpret_cast<chaos::il2cpp::com_ccw::ComCcw*>(
        reinterpret_cast<void*>(ccw_handle));

    if (ccw->cp_container == nullptr) {
        return kCONNECT_E_NOCONNECTION;
    }

    // Find the list for this IID.
    int32_t list_idx = FindListByIid(ccw->cp_container, iid);
    if (list_idx < 0) {
        return kCONNECT_E_NOCONNECTION;
    }

    auto& list = ccw->cp_container->lists[list_idx];

    // Search for the matching cookie.
    ConnectionPoint* prev = nullptr;
    ConnectionPoint* cur = list.head;
    while (cur != nullptr) {
        if (cur->cookie == cookie) {
            // Unlink from the linked list.
            if (prev == nullptr) {
                list.head = cur->next;
            } else {
                prev->next = cur->next;
            }

            // Release the sink reference.
            if (cur->sink_unknown != nullptr) {
                auto** vtbl = static_cast<chaos::il2cpp::com_ccw::ComCcwVtbl**>(cur->sink_unknown);
                (*vtbl)->Release(cur->sink_unknown);
            }

            // Free the ConnectionPoint node.
            memory_domain::DomainFreeTagged(cur);

            CHAOS_IL2CPP_LOG_DEBUG_M("COM", "CCW {0}: Unadvise cookie={1}",
                                      reinterpret_cast<void*>(ccw), cookie);
            return kS_OK;
        }
        prev = cur;
        cur = cur->next;
    }

    return kCONNECT_E_NOCONNECTION;
}

CHAOS_IL2CPP_INTPTR ComCreateEventSinkCcw(
    CHAOS_IL2CPP_INTPTR delegate_ptr,
    CHAOS_IL2CPP_GUID   event_iid) noexcept {
    if (delegate_ptr == 0 || event_iid == nullptr) {
        return 0;
    }

    // The delegate_ptr is a managed object pointer. Wrap it in a CCW.
    void* managed_object = reinterpret_cast<void*>(delegate_ptr);

    // Obtain the current thread's RuntimeState for GCHandle allocation.
    void* runtime_state = reinterpret_cast<void*>(GetCurrentRuntimeState());

    CHAOS_IL2CPP_INTPTR ccw_handle = chaos::il2cpp::com_ccw::CreateCcw(
        managed_object, runtime_state);
    if (ccw_handle == 0) {
        CHAOS_IL2CPP_LOG_WARN_M("COM", "ComCreateEventSinkCcw: CreateCcw failed for delegate {0}",
                                  managed_object);
        return 0;
    }

    CHAOS_IL2CPP_LOG_DEBUG_M("COM", "ComCreateEventSinkCcw: created CCW {0} for delegate {1} (event IID)",
                              reinterpret_cast<void*>(ccw_handle), managed_object);
    return ccw_handle;
}

}  // namespace chaos::il2cpp::runtime_core
