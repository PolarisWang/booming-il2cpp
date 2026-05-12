// ── Collection stub declarations ────────────────────────────
#pragma once

#include <chaos/native_types.h>

// List<T> helpers
CHAOS_IL2CPP_INT32 CollectionListGetCount(CHAOS_IL2CPP_INTPTR handle) noexcept;
CHAOS_IL2CPP_INTPTR CollectionListGetItem(CHAOS_IL2CPP_INTPTR handle, CHAOS_IL2CPP_INT32 index) noexcept;
void CollectionListSetItem(CHAOS_IL2CPP_INTPTR handle, CHAOS_IL2CPP_INT32 index, CHAOS_IL2CPP_INTPTR value) noexcept;
void CollectionListAdd(CHAOS_IL2CPP_INTPTR handle, CHAOS_IL2CPP_INTPTR value) noexcept;
void CollectionListInsert(CHAOS_IL2CPP_INTPTR handle, CHAOS_IL2CPP_INT32 index, CHAOS_IL2CPP_INTPTR value) noexcept;
void CollectionListRemoveAt(CHAOS_IL2CPP_INTPTR handle, CHAOS_IL2CPP_INT32 index) noexcept;
CHAOS_IL2CPP_INT32 CollectionListIndexOf(CHAOS_IL2CPP_INTPTR handle, CHAOS_IL2CPP_INTPTR value) noexcept;
CHAOS_IL2CPP_INT32 CollectionListContains(CHAOS_IL2CPP_INTPTR handle, CHAOS_IL2CPP_INTPTR value) noexcept;
CHAOS_IL2CPP_INT32 CollectionListRemove(CHAOS_IL2CPP_INTPTR handle, CHAOS_IL2CPP_INTPTR value) noexcept;
CHAOS_IL2CPP_INTPTR CollectionListToArray(CHAOS_IL2CPP_INTPTR handle) noexcept;
void CollectionListClear(CHAOS_IL2CPP_INTPTR handle) noexcept;
void CollectionListSort(CHAOS_IL2CPP_INTPTR handle) noexcept;

// Dictionary<K,V> helpers
CHAOS_IL2CPP_INT32 CollectionDictionaryGetCount(CHAOS_IL2CPP_INTPTR handle) noexcept;
CHAOS_IL2CPP_INTPTR CollectionDictionaryGetItem(CHAOS_IL2CPP_INTPTR handle, CHAOS_IL2CPP_INTPTR key) noexcept;
void CollectionDictionarySetItem(CHAOS_IL2CPP_INTPTR handle, CHAOS_IL2CPP_INTPTR key, CHAOS_IL2CPP_INTPTR value) noexcept;
CHAOS_IL2CPP_INT32 CollectionDictionaryTryGetValue(CHAOS_IL2CPP_INTPTR handle, CHAOS_IL2CPP_INTPTR key, CHAOS_IL2CPP_INTPTR* out_value) noexcept;
CHAOS_IL2CPP_INT32 CollectionDictionaryContainsKey(CHAOS_IL2CPP_INTPTR handle, CHAOS_IL2CPP_INTPTR key) noexcept;
CHAOS_IL2CPP_INT32 CollectionDictionaryRemove(CHAOS_IL2CPP_INTPTR handle, CHAOS_IL2CPP_INTPTR key) noexcept;
void CollectionDictionaryAdd(CHAOS_IL2CPP_INTPTR handle, CHAOS_IL2CPP_INTPTR key, CHAOS_IL2CPP_INTPTR value) noexcept;

// HashSet<T> helpers
CHAOS_IL2CPP_INT32 CollectionHashSetGetCount(CHAOS_IL2CPP_INTPTR handle) noexcept;
CHAOS_IL2CPP_INT32 CollectionHashSetAdd(CHAOS_IL2CPP_INTPTR handle, CHAOS_IL2CPP_INTPTR value) noexcept;
CHAOS_IL2CPP_INT32 CollectionHashSetContains(CHAOS_IL2CPP_INTPTR handle, CHAOS_IL2CPP_INTPTR value) noexcept;
CHAOS_IL2CPP_INT32 CollectionHashSetRemove(CHAOS_IL2CPP_INTPTR handle, CHAOS_IL2CPP_INTPTR value) noexcept;