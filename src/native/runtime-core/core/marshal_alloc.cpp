#include <marshal_abi.h>
#include "memory_domain.h"

// GetMarshalAllocationStorage: cast raw CHAOS_IL2CPP_INTPTR to usable void* pointer.
// Defined here (not in marshal_internal.h) because marshal_alloc.cpp is compiled
// as part of the Unity build where including marshal_internal.h would cause
// AllocateBytes/FreeBytes ambiguity with anonymous-namespace definitions in
// runtime_init.cpp and object_creation.cpp.
inline static void* GetMarshalAllocationStorage(CHAOS_IL2CPP_INTPTR memory) {
    return reinterpret_cast<void*>(memory);
}

namespace chaos::il2cpp::runtime_core {
using namespace chaos::il2cpp::marshal_abi;

MarshalAllocationHeader* TryGetMarshalAllocationHeader(CHAOS_IL2CPP_INTPTR memory) {
    if (memory == 0) {
        return nullptr;
    }
    auto* user_storage = static_cast<unsigned char*>(GetMarshalAllocationStorage(memory));
    auto* header = reinterpret_cast<MarshalAllocationHeader*>(user_storage - sizeof(MarshalAllocationHeader));
    return header->magic == kMarshalAllocationMagic ? header : nullptr;
}

CHAOS_IL2CPP_INTPTR AllocateMarshalBlock(RuntimeState* runtime_state, CHAOS_IL2CPP_SIZE requested_size, MarshalAllocationKind kind) {
    const CHAOS_IL2CPP_SIZE safe_size = requested_size == 0u ? 1u : requested_size;

    auto* domain = memory_domain::CurrentDomain();
    void* allocation = nullptr;

    if (domain != nullptr && domain->heap != nullptr) {
        allocation = domain->heap->Allocate(sizeof(MarshalAllocationHeader) + safe_size);
        if (allocation != nullptr) {
            CHAOS_IL2CPP_INT64 new_usage = domain->current_usage.load(std::memory_order_relaxed) + static_cast<CHAOS_IL2CPP_INT64>(safe_size);
            domain->current_usage.store(new_usage, std::memory_order_relaxed);
            CHAOS_IL2CPP_INT64 old_peak = domain->peak_usage.load(std::memory_order_relaxed);
            if (new_usage > old_peak) {
                domain->peak_usage.store(new_usage, std::memory_order_relaxed);
            }
        }
    } else {
        allocation = AllocateBytes(runtime_state->config, sizeof(MarshalAllocationHeader) + safe_size);
    }

    if (allocation == nullptr) {
        return 0;
    }

    auto* header = reinterpret_cast<MarshalAllocationHeader*>(allocation);
    header->magic = kMarshalAllocationMagic;
    header->domain_id = domain != nullptr ? domain->domain_id : memory_domain::kDomainIdInvalid;
    header->kind = kind;
    header->requested_size = safe_size;
    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(allocation) + sizeof(MarshalAllocationHeader);
}

CHAOS_IL2CPP_INTPTR ReallocateMarshalBlock(RuntimeState* runtime_state, CHAOS_IL2CPP_INTPTR memory, CHAOS_IL2CPP_SIZE requested_size, MarshalAllocationKind kind) {
    if (memory == 0) {
        return AllocateMarshalBlock(runtime_state, requested_size, kind);
    }

    const CHAOS_IL2CPP_SIZE safe_size = requested_size == 0u ? 1u : requested_size;

    if (auto* old_header = TryGetMarshalAllocationHeader(memory)) {
        auto* domain = old_header->domain_id != memory_domain::kDomainIdInvalid
            ? memory_domain::FindDomainById(old_header->domain_id)
            : nullptr;

        if (domain != nullptr && domain->heap != nullptr) {
            void* resized = domain->heap->Reallocate(old_header, sizeof(MarshalAllocationHeader) + safe_size);
            if (resized == nullptr) {
                return 0;
            }
            domain->current_usage += static_cast<CHAOS_IL2CPP_INT64>(safe_size) -
                                     static_cast<CHAOS_IL2CPP_INT64>(old_header->requested_size);
            auto* new_header = reinterpret_cast<MarshalAllocationHeader*>(resized);
            new_header->kind = kind;
            new_header->requested_size = safe_size;
            new_header->magic = kMarshalAllocationMagic;
            return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(new_header) + sizeof(MarshalAllocationHeader);
        }

        // Fallback: alloc-copy-free via config allocator.
        void* old_ptr = old_header;
        auto* resized = static_cast<unsigned char*>(AllocateBytes(runtime_state->config, sizeof(MarshalAllocationHeader) + safe_size));
        if (resized == nullptr) {
            return 0;
        }
        CHAOS_IL2CPP_MEMCPY(resized, old_ptr, sizeof(MarshalAllocationHeader) + CHAOS_IL2CPP_MIN(old_header->requested_size, safe_size));
        FreeBytes(runtime_state->config, old_ptr);
        auto* new_header = reinterpret_cast<MarshalAllocationHeader*>(resized);
        new_header->magic = kMarshalAllocationMagic;
        new_header->kind = kind;
        new_header->requested_size = safe_size;
        return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(new_header) + sizeof(MarshalAllocationHeader);
    }

    // Raw realloc without header: we don't know the old allocation size.
    // Copy only up to the new size; the old allocation was at least large
    // enough for the original request, so the copy is truncated on shrink
    // and zero-extended on growth (new buffer is zero-initialized).
    void* old_storage = GetMarshalAllocationStorage(memory);
    auto* resized = static_cast<unsigned char*>(AllocateBytes(runtime_state->config, safe_size));
    if (resized == nullptr) {
        return 0;
    }
    CHAOS_IL2CPP_MEMCPY(resized, old_storage, safe_size);
    FreeBytes(runtime_state->config, old_storage);
    return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(resized);
}

bool FreeMarshalBlock(RuntimeState* runtime_state, CHAOS_IL2CPP_INTPTR memory, MarshalAllocationKind kind, bool zero_before_free) {
    if (memory == 0) {
        return true;
    }

    if (auto* header = TryGetMarshalAllocationHeader(memory)) {
        if (zero_before_free) {
            CHAOS_IL2CPP_MEMSET(
                reinterpret_cast<unsigned char*>(header) + sizeof(MarshalAllocationHeader),
                0,
                header->requested_size);
        }

        if (header->kind != kind) {
            return false;
        }

        auto* domain = header->domain_id != memory_domain::kDomainIdInvalid
            ? memory_domain::FindDomainById(header->domain_id)
            : nullptr;

        if (domain != nullptr && domain->heap != nullptr) {
            domain->current_usage -= static_cast<CHAOS_IL2CPP_INT64>(header->requested_size);
            domain->heap->Free(header);
        } else {
            FreeBytes(runtime_state->config, header);
        }
        return true;
    }

    FreeBytes(runtime_state->config, GetMarshalAllocationStorage(memory));
    return true;
}

bool IsAttached(RuntimeState* runtime_state, ThreadState* thread_state) {
    return runtime_state != nullptr && thread_state != nullptr && thread_state->runtime_state == runtime_state;
}

RuntimeInternalState* GetRuntimeInternalState(RuntimeState* runtime_state) {
    return runtime_state != nullptr ? runtime_state->internal_state : nullptr;
}

ThreadInternalState* GetThreadInternalState(ThreadState* thread_state) {
    return thread_state != nullptr ? thread_state->internal_state : nullptr;
}

}  // namespace chaos::il2cpp::runtime_core