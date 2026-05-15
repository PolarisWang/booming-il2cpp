#ifndef CHAOS_IL2CPP_GC_LAYOUT_H_
#define CHAOS_IL2CPP_GC_LAYOUT_H_

#include <chaos/native_types.h>
#include <chaos/type_info.h>

#include <atomic>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <mutex>
#include <vector>

namespace chaos::il2cpp::runtime_core {

// Forward declarations.
struct Region;
struct YoungCollectionResult;

// ======================================================================
// GcTypeLayout — precise GC pointer layout for a managed type
//
// Each managed type that can contain GC pointers registers a layout
// describing the offset of each pointer field within the object.
// The young collector (Phase 2) and mark-sweep collector (DrainMarkStack)
// use this to precisely scan objects instead of conservative word-by-word
// scanning.
//
// Layouts are stored in a process-wide hash table keyed by TypeInfo
// stable_id (FNV-1a 64-bit).
//
// For pointer-free types (value types, blittable, raw arrays), pointer_count
// is 0 and instance_size is the full object size.  The scanner skips these
// entirely, eliminating false positives from fill bytes.
// ======================================================================

/// Maximum pointer offsets per type.
/// Types with more pointers use a chained overflow layout.
static constexpr int kGcLayoutMaxInlinePointers = 32;

/// One GC pointer offset entry — offset from object start to a
/// pointer-typed field.
struct GcPointerOffset {
    uint16_t offset;  // byte offset from object base (0 = first word = TypeInfo*)
};

/// Per-type GC layout (cache-line aligned for scanner hot path).
///
/// The layout is designed for the scanner to read instance_size and
/// pointer_count in one cache line, then iterate pointer_offsets[]
/// in the second cache line.
struct alignas(64) GcTypeLayout {
    uint32_t instance_size;       // total object size in bytes (including header)
    uint16_t pointer_count;       // number of pointer fields (0 = pointer-free)
    uint16_t _reserved;           // padding
    uint32_t _reserved2;          // padding
    GcPointerOffset pointer_offsets[kGcLayoutMaxInlinePointers];  // sorted offsets (may be partial when pointer_count < kGcLayoutMaxInlinePointers)
};

/// Sentinel stable_id for raw/pointer-free allocations in the stress test.
/// The scanner treats objects with this stable_id as pointer-free and
/// does not scavenge them.
static constexpr CHAOS_IL2CPP_UINT64 kGcLayoutRawAllocStableId =
    CHAOS_IL2CPP_UINT64(0x00000000DEADBEEFull);

// ======================================================================
// TypeInfo address range — used to validate TypeInfo* pointers before
// dereferencing during conservative candidate validation.
//
// Lock-free: ranges are registered once at startup and never modified.
// We use a fixed-size array with atomic read index, so IsValidTypeInfoPointer
// is O(log N) with no mutex contention.
// ======================================================================

static constexpr int kMaxTypeInfoRanges = 64;

struct TypeInfoRange {
    uintptr_t begin;
    uintptr_t end;  // exclusive
};

// ======================================================================
// GcLayoutTable — dynamically growable hash table for GC layouts
//
// RCU-managed: published atomically via GcLayoutRegistry::current_table_.
// Readers (Lookup) load the pointer and probe lock-free; writers publish
// a new table and retire the old one for deferred reclamation.
// ======================================================================

/// Load factor threshold that triggers GrowTable (75%).
static constexpr float kGcLayoutLoadFactorThreshold = 0.75f;

/// Minimum table capacity (must be >= initial entry count).
static constexpr int kGcLayoutMinCapacity = 4096;

/// Sentinel for empty slots.
static constexpr uint64_t kGcLayoutEmptySlot = 0;

struct GcLayoutTable {
    std::atomic<uint64_t>* keys;      // stable_id keys, allocated array
    GcTypeLayout**         values;    // GcTypeLayout* values, allocated array
    int                    capacity;  // total slots
    int                    count;     // occupied slots

    GcLayoutTable(int cap)
        : keys(new std::atomic<uint64_t>[static_cast<size_t>(cap)]())
        , values(new GcTypeLayout*[static_cast<size_t>(cap)]())
        , capacity(cap)
        , count(0) {}

    ~GcLayoutTable() {
        delete[] keys;
        delete[] values;
        // Note: GcTypeLayout objects are NOT owned by the table — they are
        // allocated and owned by the registrar (GcLayoutRegistry delegates
        // to the currently-active table on lookup; layout objects survive
        // table retirement and are re-linked into the new table).
    }
};

// ======================================================================
// GcLayoutRegistry — process-wide registry of GC layouts
//
// Thread-safe: reads (Lookup) are lock-free via RCU-protected table pointer;
// writes (Register) use a mutex and publish new table atomically.
// ======================================================================

class GcLayoutRegistry {
public:
    /// Access the process-wide singleton.
    static GcLayoutRegistry& Instance();

    /// Register a GC layout for a type identified by stable_id.
    /// @param stable_id  The type's FNV-1a stable ID.
    /// @param instance_size  Total object size.
    /// @param pointer_offsets  Array of byte offsets for pointer fields.
    /// @param pointer_count  Number of entries in pointer_offsets.
    void Register(uint64_t stable_id, uint32_t instance_size,
                  const uint16_t* pointer_offsets, uint16_t pointer_count);

    /// Look up a layout by stable_id.
    /// Returns nullptr if no layout is registered.
    /// Lock-free (atomic load of current table + linear probe).
    const GcTypeLayout* Lookup(uint64_t stable_id) const;

    /// Register a range of memory that contains valid TypeInfo structs.
    /// The young collector uses this to validate candidate TypeInfo*
    /// pointers before dereferencing them.
    void RegisterTypeInfoRange(uintptr_t range_begin, uintptr_t range_end);

    /// Check if a pointer points to a known TypeInfo range.
    bool IsValidTypeInfoPointer(const void* ptr) const;

    /// Register a raw allocation type (pointer-free) for a given size.
    /// Used by the stress test and for untyped allocations.
    void RegisterRawAllocType(uint32_t instance_size);

    /// Convenience: register a pointer-free layout for any size.
    /// Returns the stable_id used so the caller can write it at offset 0.
    uint64_t RegisterOrGetRawAllocType(uint32_t instance_size);

    /// Reclaim all retired (old) tables that are no longer in use.
    /// Safe to call inside a GC safepoint — no concurrent Lookup.
    /// Called from Collect() after mark-sweep completes.
    void ReclaimRetiredTables();

private:
    GcLayoutRegistry();
    ~GcLayoutRegistry() = default;
    GcLayoutRegistry(const GcLayoutRegistry&) = delete;
    GcLayoutRegistry& operator=(const GcLayoutRegistry&) = delete;

    /// Grow the hash table to 2x capacity.  Must hold register_mutex_.
    void GrowTable();

    /// Insert a (key, value) pair into the given table.
    /// Returns true on success, false if table is full.
    static bool InsertIntoTable(GcLayoutTable* table, uint64_t key, GcTypeLayout* value);

    /// Retire an old table for deferred reclamation.
    void RetireTable(GcLayoutTable* table);

    // RCU-managed hash table: readers load this atomically; writers
    // publish a new table under register_mutex_.
    std::atomic<GcLayoutTable*> current_table_{nullptr};

    // TypeInfo address ranges (lock-free: fixed array with atomic index).
    TypeInfoRange typeinfo_ranges_[kMaxTypeInfoRanges]{};
    std::atomic<int> typeinfo_range_count_{0};

    // Mutex for registration (only Register() needs it).
    mutable std::mutex register_mutex_;

    // Last raw-alloc serial number (for generating unique stable_ids).
    std::atomic<uint32_t> raw_alloc_serial_{0};

    // Retired tables pending safe reclamation.
    // Access is serialized by register_mutex_.
    std::vector<GcLayoutTable*> retired_tables_;
};

// ======================================================================
// VerifyPattern-compatible TypeInfo validation
//
// Before the young collector Phase 2 scavenges a candidate pointer,
// it checks that the candidate's first word (expected TypeInfo*) is
// a valid pointer into a registered TypeInfo range.  This eliminates
// false-positive scavenging of random fill bytes.
// ======================================================================

/// Check if @a obj_addr is a valid managed object start.
///
/// Returns true if:
///   1. The first word at obj_addr is a non-null pointer to a registered
///      TypeInfo range (GcLayoutRegistry::IsValidTypeInfoPointer).
///
/// Returns false for:
///   - nullptr / uninitialized memory (first word = 0)
///   - Fill bytes / pattern bytes that don't point to TypeInfo
///
/// This check is safe: we only dereference the first word as a pointer
/// (which is always readable since it's within the nursery), then check
/// if that pointer falls within a known TypeInfo range without
/// dereferencing it further.
inline bool IsValidManagedObject(const void* obj_addr) {
    if (obj_addr == nullptr) return false;

    // Read the first word — in a real managed object this is the TypeInfo*.
    const void* first_word = *static_cast<const void* const*>(obj_addr);
    if (first_word == nullptr) return false;

    // Check if the first word points to a registered TypeInfo range.
    return GcLayoutRegistry::Instance().IsValidTypeInfoPointer(first_word);
}

// ======================================================================
// Scanner helpers — precise object scanning using GcLayout
// ======================================================================

/// Scan a single managed object for nursery pointers, scavenging any found.
///
/// @param obj       Start of the managed object.
/// @param layout    GC layout for this object's type.
/// @param nursery   The nursery region to check against.
/// @param result    Young collection result accumulator (may be null).
///
/// Only the offsets listed in layout->pointer_offsets are scanned.
/// Returns the number of scavenged pointers found.
int ScanObjectPointers(void* obj, const GcTypeLayout* layout,
                       Region* nursery, YoungCollectionResult* result);

// ======================================================================
// Enhanced Cheney BFS — walks promoted objects using precise layouts
// ======================================================================

/// Perform Cheney BFS over the promoted tenured range, scanning each
/// object's pointer fields using registered GC layouts, and scavenging
/// any pointers back into the nursery.
///
/// @param tenured_begin  Start of the tenured region containing promoted objs.
/// @param tenured_end    End of the tenured region.
/// @param nursery        The nursery region.
/// @param result         Young collection result accumulator.
void CheneyBfsPrecise(void* tenured_begin, void* tenured_end,
                      Region* nursery, YoungCollectionResult* result);

}  // namespace chaos::il2cpp::runtime_core

#endif  // CHAOS_IL2CPP_GC_LAYOUT_H_