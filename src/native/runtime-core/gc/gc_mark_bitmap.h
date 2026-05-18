#ifndef CHAOS_IL2CPP_GC_MARK_BITMAP_H_
#define CHAOS_IL2CPP_GC_MARK_BITMAP_H_

#include <chaos/native_types.h>

#include <cstddef>
#include <cstdint>
#include <cstring>

#include <chaos/config.h>
#include <chaos/log.h>

namespace chaos::il2cpp::runtime_core {

/// Bounded mark-bitmap wrapper with atomic marking and safety checks.
///
/// Each bit represents one pointer-sized (8-byte) slot in the page payload.
/// The bitmap is stored inline in each OldGenPage, immediately after the
/// page header and before the payload.
///
/// All marking operations use atomic test-and-set so concurrent marking
/// (BGC + STW parallel) is safe.
class GcMarkBitmap {
public:
    GcMarkBitmap(unsigned char* data, CHAOS_IL2CPP_SIZE byte_count) noexcept
        : data_(data), byte_count_(byte_count) {}

    /// Atomically mark @a num_slots slots starting at @a start_slot.
    /// Clamped to bitmap capacity.  Returns true if at least one bit
    /// was newly set (was 0, now 1).
    bool MarkRange(CHAOS_IL2CPP_SIZE start_slot, CHAOS_IL2CPP_SIZE num_slots) noexcept {
        if (num_slots == 0 || data_ == nullptr) return false;
        CHAOS_IL2CPP_SIZE max_slots = byte_count_ * 8;
        CHAOS_IL2CPP_SIZE end_slot = start_slot + num_slots;
        CHAOS_IL2CPP_ASSERT(end_slot <= max_slots && "GcMarkBitmap::MarkRange overflow");
        if (end_slot > max_slots) end_slot = max_slots;
        bool newly_set = false;
        for (CHAOS_IL2CPP_SIZE s = start_slot; s < end_slot; ) {
            CHAOS_IL2CPP_SIZE byte_idx = s / 8;
            int bit_off = static_cast<int>(s % 8);
            int bits_here = std::min(8 - bit_off, static_cast<int>(end_slot - s));
            unsigned char mask = static_cast<unsigned char>(((1u << bits_here) - 1) << bit_off);
        #if defined(_MSC_VER) && !defined(__clang__)
            auto prev = _InterlockedOr8(reinterpret_cast<volatile char*>(&data_[byte_idx]),
                                         static_cast<char>(mask));
            if ((static_cast<unsigned char>(prev) & mask) != mask) newly_set = true;
        #else
            auto prev = __atomic_fetch_or(&data_[byte_idx], mask, __ATOMIC_RELAXED);
            if ((prev & mask) != mask) newly_set = true;
        #endif
            s += bits_here;
        }

    #if defined(CHAOS_IL2CPP_DEBUG)
        // Poison verification: check the last 16 bytes for 0xCD pattern.
        // An overflow beyond bitmap capacity would corrupt the poison zone.
        if (byte_count_ >= 16) {
            const unsigned char* poison = data_ + byte_count_ - 16;
            for (int i = 0; i < 16; i++) {
                if (poison[i] != 0xCD) {
                    CHAOS_IL2CPP_FAIL("GcMarkBitmap overflow: poison corrupted at byte offset %d/%llu (value=0x%02x)",
                        i, static_cast<unsigned long long>(byte_count_), poison[i]);
                    break;
                }
            }
        }
    #endif
        return newly_set;
    }

    /// Test whether a single slot is marked.
    bool TestSlot(CHAOS_IL2CPP_SIZE slot) const noexcept {
        CHAOS_IL2CPP_SIZE byte_idx = slot / 8;
        if (byte_idx >= byte_count_) return false;
        int bit_off = static_cast<int>(slot % 8);
        return (data_[byte_idx] >> bit_off) & 1u;
    }

    /// View the bitmap as an array of 64-bit words (read-only).
    /// Used by sweep/compact for fast bit-scanning.
    const uint64_t* Words() const noexcept {
        return reinterpret_cast<const uint64_t*>(data_);
    }

    /// Number of 64-bit words in this bitmap.
    CHAOS_IL2CPP_SIZE WordCount() const noexcept {
        return byte_count_ / sizeof(uint64_t);
    }

    /// Clear all bits (memset zero).
    void Clear() noexcept {
        if (data_ != nullptr) {
            std::memset(data_, 0, byte_count_);
        }
    }

    /// Return true if any bit is set.
    bool AnySet() const noexcept {
        for (CHAOS_IL2CPP_SIZE i = 0; i < byte_count_; i++) {
            if (data_[i] != 0) return true;
        }
        return false;
    }

    CHAOS_IL2CPP_SIZE ByteCount() const noexcept { return byte_count_; }
    CHAOS_IL2CPP_SIZE SlotCapacity() const noexcept { return byte_count_ * 8; }

private:
    unsigned char* data_;
    CHAOS_IL2CPP_SIZE byte_count_;
};

}  // namespace chaos::il2cpp::runtime_core

#endif  // CHAOS_IL2CPP_GC_MARK_BITMAP_H_
