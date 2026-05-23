// dbg_breakpoint.cpp — Breakpoint manager implementation

#include "dbg_breakpoint.h"

#if CHAOS_IL2CPP_DEBUGGER == 1

#include <algorithm>
#include <atomic>
#include <shared_mutex>

namespace chaos::il2cpp::diagnostics {

namespace {

// Global breakpoint table: (method_token, il_offset) → breakpoint_id.
std::unordered_map<BreakpointKey, int, BreakpointKeyHash, BreakpointKeyEqual> g_breakpoints;

// Reverse map: breakpoint_id → BreakpointKey + metadata.
struct BreakpointEntry {
    int id;
    uint32_t method_token;
    uint32_t il_offset;
    std::string source_path;
    int line;
};
std::unordered_map<int, BreakpointEntry> g_breakpoint_entries;

std::atomic<int> g_next_breakpoint_id{1};
std::shared_mutex g_breakpoint_mutex;

}  // anonymous namespace

void DbgBreakpointInitialize() noexcept {
    // No initialization needed.
}

void DbgBreakpointShutdown() noexcept {
    std::lock_guard lock(g_breakpoint_mutex);
    g_breakpoints.clear();
    g_breakpoint_entries.clear();
}

int DbgSetBreakpoint(uint32_t method_token, uint32_t il_offset,
                     const char* source_path, int line) noexcept
{
    std::lock_guard lock(g_breakpoint_mutex);

    // Check if breakpoint already exists.
    BreakpointKey key{ method_token, il_offset };
    auto it = g_breakpoints.find(key);
    if (it != g_breakpoints.end()) {
        return it->second;  // return existing ID
    }

    int id = g_next_breakpoint_id.fetch_add(1, std::memory_order_relaxed);
    g_breakpoints[key] = id;
    g_breakpoint_entries[id] = { id, method_token, il_offset, source_path, line };
    return id;
}

void DbgRemoveBreakpoint(int breakpoint_id) noexcept {
    std::lock_guard lock(g_breakpoint_mutex);

    auto it = g_breakpoint_entries.find(breakpoint_id);
    if (it != g_breakpoint_entries.end()) {
        BreakpointKey key{ it->second.method_token, it->second.il_offset };
        g_breakpoints.erase(key);
        g_breakpoint_entries.erase(it);
    }
}

void DbgClearAllBreakpoints() noexcept {
    std::lock_guard lock(g_breakpoint_mutex);
    g_breakpoints.clear();
    g_breakpoint_entries.clear();
}

int DbgCheckBreakpoint(uint32_t method_token, uint32_t il_offset) noexcept {
    // Shared lock allows concurrent reads; only blocks when a writer is active.
    // Breakpoints are modified on the debugger thread during "continue" state,
    // so write contention is extremely rare.
    std::shared_lock lock(g_breakpoint_mutex);
    BreakpointKey key{ method_token, il_offset };
    auto it = g_breakpoints.find(key);
    if (it != g_breakpoints.end()) {
        return it->second;
    }
    return -1;
}

std::vector<DapBreakpoint> DbgGetAllBreakpoints() noexcept {
    std::shared_lock lock(g_breakpoint_mutex);
    std::vector<DapBreakpoint> result;
    for (const auto& [id, entry] : g_breakpoint_entries) {
        DapBreakpoint bp;
        bp.id = entry.id;
        bp.verified = true;
        bp.source_path = entry.source_path;
        bp.line = entry.line;
        result.push_back(bp);
    }
    return result;
}

std::vector<DapBreakpoint> DbgGetBreakpointsForSource(const char* source_path) noexcept {
    std::shared_lock lock(g_breakpoint_mutex);
    std::vector<DapBreakpoint> result;
    for (const auto& [id, entry] : g_breakpoint_entries) {
        if (entry.source_path == source_path) {
            DapBreakpoint bp;
            bp.id = entry.id;
            bp.verified = true;
            bp.source_path = entry.source_path;
            bp.line = entry.line;
            result.push_back(bp);
        }
    }
    return result;
}

}  // namespace chaos::il2cpp::diagnostics

#endif  // CHAOS_IL2CPP_DEBUGGER
