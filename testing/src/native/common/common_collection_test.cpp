#include <gtest/gtest.h>
#include <chaos/common.h>
#include <chaos/collection.h>

using chaos::il2cpp::common::kNativeStorageSlotOffset;
using chaos::il2cpp::common::DictionaryRuntimeStorage;

TEST(CommonCollection, NativeStorageSlotOffset) {
    EXPECT_EQ(16, kNativeStorageSlotOffset);
}

TEST(CommonCollection, DictionaryRuntimeStorage_Empty) {
    DictionaryRuntimeStorage<int, int> storage;
    EXPECT_TRUE(storage.entries.empty());
}

TEST(CommonCollection, DictionaryRuntimeStorage_Insert) {
    DictionaryRuntimeStorage<int, int> storage;
    storage.entries.push_back({1, 100});
    storage.entries.push_back({2, 200});
    EXPECT_EQ(2u, storage.entries.size());
    EXPECT_EQ(100, storage.entries[0].second);
    EXPECT_EQ(200, storage.entries[1].second);
}

TEST(CommonCollection, DictionaryRuntimeStorage_StringKey) {
    DictionaryRuntimeStorage<std::string, double> storage;
    storage.entries.push_back({"pi", 3.14});
    storage.entries.push_back({"e", 2.718});
    EXPECT_EQ(2u, storage.entries.size());
    EXPECT_DOUBLE_EQ(3.14, storage.entries[0].second);
}

TEST(CommonCollection, PairType) {
    CHAOS_IL2CPP_PAIR(int, int) p{42, 99};
    EXPECT_EQ(42, p.first);
    EXPECT_EQ(99, p.second);
}
