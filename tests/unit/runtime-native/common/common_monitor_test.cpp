#include <gtest/gtest.h>
#include <chaos/common.h>
#include <chaos/monitor.h>

using namespace chaos::il2cpp::common;

TEST(CommonMonitor, RequireEntryCreatesNew) {
    // Use a real heap pointer as a unique handle
    auto* dummy = new char[64];
    CHAOS_IL2CPP_INTPTR handle = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(dummy);

    auto& entry = require_monitor_runtime_entry(handle);
    // Just verify we can access it without crash
    (void)entry;

    delete[] dummy;
}

TEST(CommonMonitor, SameHandleReturnsSameEntry) {
    auto* dummy = new char[64];
    CHAOS_IL2CPP_INTPTR handle = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(dummy);

    auto& entry1 = require_monitor_runtime_entry(handle);
    auto& entry2 = require_monitor_runtime_entry(handle);

    EXPECT_EQ(&entry1, &entry2);

    delete[] dummy;
}

TEST(CommonMonitor, DifferentHandlesDifferentEntries) {
    auto* dummy1 = new char[64];
    auto* dummy2 = new char[64];
    CHAOS_IL2CPP_INTPTR handle1 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(dummy1);
    CHAOS_IL2CPP_INTPTR handle2 = reinterpret_cast<CHAOS_IL2CPP_INTPTR>(dummy2);

    auto& entry1 = require_monitor_runtime_entry(handle1);
    auto& entry2 = require_monitor_runtime_entry(handle2);

    EXPECT_NE(&entry1, &entry2);

    delete[] dummy1;
    delete[] dummy2;
}
