/// common_unordered_dense_test — Tests for unordered_dense.h wrappers.

#include <gtest/gtest.h>
#include <chaos/common.h>
#include <string>
#include <vector>

using namespace chaos::il2cpp::common;

TEST(CommonUnorderedDense, MapIdentityInt) {
    CHAOS_IL2CPP_UNORDERED_DENSE_MAP_IDENTITY(int, int) m;
    EXPECT_TRUE(m.empty());

    m[1] = 100;
    m[2] = 200;
    EXPECT_EQ(m.size(), 2u);
    EXPECT_EQ(m[1], 100);
    EXPECT_EQ(m[2], 200);
    EXPECT_EQ(m.find(3), m.end());
}

TEST(CommonUnorderedDense, MapIdentityPointer) {
    int a, b;
    CHAOS_IL2CPP_UNORDERED_DENSE_MAP_IDENTITY(int*, int) m;
    m[&a] = 1;
    m[&b] = 2;
    EXPECT_EQ(m[&a], 1);
    EXPECT_EQ(m[&b], 2);
}

TEST(CommonUnorderedDense, MapString) {
    CHAOS_IL2CPP_UNORDERED_DENSE_MAP(std::string, int) m;
    m["hello"] = 42;
    m["world"] = 99;
    EXPECT_EQ(m["hello"], 42);
    EXPECT_EQ(m["world"], 99);
    EXPECT_EQ(m.find("missing"), m.end());
}

TEST(CommonUnorderedDense, SetInt) {
    CHAOS_IL2CPP_UNORDERED_DENSE_SET(int) s;
    s.insert(1);
    s.insert(2);
    s.insert(1);  // duplicate
    EXPECT_EQ(s.size(), 2u);
    EXPECT_NE(s.find(1), s.end());
    EXPECT_NE(s.find(2), s.end());
    EXPECT_EQ(s.find(3), s.end());
}

TEST(CommonUnorderedDense, EraseAndReinsert) {
    CHAOS_IL2CPP_UNORDERED_DENSE_MAP_IDENTITY(int, int) m;
    m[1] = 10;
    m[2] = 20;
    EXPECT_EQ(m.erase(1), 1u);
    EXPECT_EQ(m.size(), 1u);
    m[1] = 99;  // reinsert
    EXPECT_EQ(m[1], 99);
}

TEST(CommonUnorderedDense, RehashStress) {
    CHAOS_IL2CPP_UNORDERED_DENSE_MAP_IDENTITY(int, int) m;
    for (int i = 0; i < 1000; i++) {
        m[i] = i * 2;
    }
    EXPECT_EQ(m.size(), 1000u);
    for (int i = 0; i < 1000; i++) {
        EXPECT_EQ(m[i], i * 2);
    }
}

TEST(CommonUnorderedDense, ClearAndReuse) {
    CHAOS_IL2CPP_UNORDERED_DENSE_MAP_IDENTITY(int, int) m;
    m[1] = 1;
    m[2] = 2;
    m.clear();
    EXPECT_TRUE(m.empty());
    m[3] = 3;  // reuse after clear
    EXPECT_EQ(m[3], 3);
}
