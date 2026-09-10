// hotupdate_probe.cpp — Hot-update semantic observability probe implementation.
//
// Uses a word-sized atomic bitset for lock-free mark/read operations.
// All marks use relaxed ordering: the probe is observability-only and the
// harness serialises phases (baseline -> patch -> revert) externally.
//
// The bitset is allocated on first use and never freed; reset only zeroes the
// words.  This avoids pulling a .data section into AOT-only builds.

#include "hotupdate_probe.h"
#include <bit>
#include <cstdint>
#include <atomic>
#include <cstdlib>

namespace chaos::il2cpp::runtime_core {
namespace {

constexpr uint32_t kProbeWordCount =
    (kHotupdateProbeCapacity + 63u) / 64u;

std::atomic<uint64_t>* g_probe_bitset = nullptr;
std::atomic<bool>      g_probe_initialized{false};

void EnsureBitset() noexcept {
    if (g_probe_initialized.load(std::memory_order_acquire)) return;
    auto* mem = static_cast<std::atomic<uint64_t>*>(
        std::calloc(kProbeWordCount, sizeof(uint64_t)));
    if (mem == nullptr) return;
    g_probe_bitset = mem;
    g_probe_initialized.store(true, std::memory_order_release);
}

}  // anonymous namespace

void HotupdateProbeMark(uint32_t index) noexcept {
    if (index >= kHotupdateProbeCapacity) return;
    EnsureBitset();
    uint32_t word = index / 64u;
    uint64_t bit  = uint64_t{1} << (index % 64u);
    g_probe_bitset[word].fetch_or(bit, std::memory_order_relaxed);
}

bool HotupdateProbeIsMarked(uint32_t index) noexcept {
    if (!g_probe_initialized.load(std::memory_order_acquire)) return false;
    if (index >= kHotupdateProbeCapacity) return false;
    uint32_t word = index / 64u;
    uint64_t bit  = uint64_t{1} << (index % 64u);
    return (g_probe_bitset[word].load(std::memory_order_relaxed) & bit) != 0;
}

uint32_t HotupdateProbeMarkedCount() noexcept {
    if (!g_probe_initialized.load(std::memory_order_acquire)) return 0;
    uint32_t count = 0;
    for (uint32_t i = 0; i < kProbeWordCount; ++i) {
        uint64_t w = g_probe_bitset[i].load(std::memory_order_relaxed);
        count += static_cast<uint32_t>(std::popcount(w));
    }
    return count;
}

uint32_t HotupdateProbeSnapshot(uint64_t* out_words, uint32_t word_capacity) noexcept {
    if (out_words == nullptr) return 0;
    uint32_t n = (word_capacity < kProbeWordCount) ? word_capacity : kProbeWordCount;
    for (uint32_t i = 0; i < n; ++i) {
        out_words[i] = g_probe_bitset[i].load(std::memory_order_relaxed);
    }
    return n;
}

void HotupdateProbeReset() noexcept {
    if (!g_probe_initialized.load(std::memory_order_acquire)) return;
    for (uint32_t i = 0; i < kProbeWordCount; ++i) {
        g_probe_bitset[i].store(0, std::memory_order_relaxed);
    }
}

}  // namespace chaos::il2cpp::runtime_core

extern "C" void chaos_hotupdate_probe_mark(CHAOS_IL2CPP_UINT32 subject_index) noexcept {
    chaos::il2cpp::runtime_core::HotupdateProbeMark(subject_index);
}

extern "C" void chaos_hotupdate_probe_reset(void) noexcept {
    chaos::il2cpp::runtime_core::HotupdateProbeReset();
}

extern "C" CHAOS_IL2CPP_UINT32 chaos_hotupdate_probe_marked_count(void) noexcept {
    return static_cast<CHAOS_IL2CPP_UINT32>(
        chaos::il2cpp::runtime_core::HotupdateProbeMarkedCount());
}