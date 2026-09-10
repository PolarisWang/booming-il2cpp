/// common_load_store_test — Unit tests for load_store.h bitcast round-trips.

#include <gtest/gtest.h>
#include <chaos/common.h>
#include <chaos/load_store.h>
#include <limits>

using namespace chaos::il2cpp::common;

// ═══════════════════════════════════════════════════════════════════════════
// Float32 round-trip
// ═══════════════════════════════════════════════════════════════════════════

TEST(CommonLoadStore, Float32RoundTrip) {
    float values[] = { 0.0f, -0.0f, 1.0f, -1.0f, 3.14159f, 1e10f, std::numeric_limits<float>::max(), std::numeric_limits<float>::min() };
    for (auto v : values) {
        auto stored = store_float32(v);
        float loaded = load_float32(stored);
        EXPECT_FLOAT_EQ(v, loaded);
    }
}

TEST(CommonLoadStore, Float32Special) {
    float inf = std::numeric_limits<float>::infinity();
    float nan = std::numeric_limits<float>::quiet_NaN();
    EXPECT_FLOAT_EQ(inf, load_float32(store_float32(inf)));
    EXPECT_TRUE(std::isnan(load_float32(store_float32(nan))));
}

// ═══════════════════════════════════════════════════════════════════════════
// Float64 round-trip
// ═══════════════════════════════════════════════════════════════════════════

TEST(CommonLoadStore, Float64RoundTrip) {
    double values[] = { 0.0, -0.0, 1.0, -1.0, 3.14159265358979, 1e100, std::numeric_limits<double>::max(), std::numeric_limits<double>::min() };
    for (auto v : values) {
        auto stored = store_float64(v);
        double loaded = load_float64(stored);
        EXPECT_DOUBLE_EQ(v, loaded);
    }
}

TEST(CommonLoadStore, Float64Special) {
    double inf = std::numeric_limits<double>::infinity();
    double nan = std::numeric_limits<double>::quiet_NaN();
    EXPECT_DOUBLE_EQ(inf, load_float64(store_float64(inf)));
    EXPECT_TRUE(std::isnan(load_float64(store_float64(nan))));
}

// ═══════════════════════════════════════════════════════════════════════════
// Int64 round-trip
// ═══════════════════════════════════════════════════════════════════════════

TEST(CommonLoadStore, Int64RoundTrip) {
    CHAOS_IL2CPP_INT64 values[] = { 0, 1, -1, 42, CHAOS_IL2CPP_INT64_MAX, CHAOS_IL2CPP_INT64_MIN };
    for (auto v : values) {
        auto stored = store_int64(v);
        auto loaded = load_int64(stored);
        EXPECT_EQ(v, loaded);
    }
}

// ═══════════════════════════════════════════════════════════════════════════
// UInt64 round-trip
// ═══════════════════════════════════════════════════════════════════════════

TEST(CommonLoadStore, UInt64RoundTrip) {
    CHAOS_IL2CPP_UINT64 values[] = { 0, 1, 42, CHAOS_IL2CPP_UINT64_MAX };
    for (auto v : values) {
        auto stored = store_uint64(v);
        auto loaded = load_uint64(stored);
        EXPECT_EQ(v, loaded);
    }
}
