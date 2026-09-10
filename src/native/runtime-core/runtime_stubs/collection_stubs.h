// ── Collection stub declarations ────────────────────────────
#pragma once

#include <chaos/native_types.h>

extern "C" {

// List<T> helpers
// Only ToArray remains (needs GC allocation). All other List<T>
// methods use inline field operations from shape resolvers.
CHAOS_IL2CPP_INTPTR CollectionListToArray(CHAOS_IL2CPP_INTPTR handle) noexcept;
void CollectionListAdd(CHAOS_IL2CPP_INTPTR handle, CHAOS_IL2CPP_INTPTR value) noexcept;
CHAOS_IL2CPP_INT32 CollectionListGetCount(CHAOS_IL2CPP_INTPTR handle) noexcept;

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

// Queue<T> helpers
CHAOS_IL2CPP_INT32 CollectionQueueGetCount(CHAOS_IL2CPP_INTPTR handle) noexcept;
void CollectionQueueEnqueue(CHAOS_IL2CPP_INTPTR handle, CHAOS_IL2CPP_INTPTR value) noexcept;
CHAOS_IL2CPP_INTPTR CollectionQueueDequeue(CHAOS_IL2CPP_INTPTR handle) noexcept;
CHAOS_IL2CPP_INTPTR CollectionQueuePeek(CHAOS_IL2CPP_INTPTR handle) noexcept;
CHAOS_IL2CPP_INT32 CollectionQueueTryDequeue(CHAOS_IL2CPP_INTPTR handle, CHAOS_IL2CPP_INTPTR* out_value) noexcept;
CHAOS_IL2CPP_INT32 CollectionQueueTryPeek(CHAOS_IL2CPP_INTPTR handle, CHAOS_IL2CPP_INTPTR* out_value) noexcept;
void CollectionQueueClear(CHAOS_IL2CPP_INTPTR handle) noexcept;

// Stack<T> helpers
CHAOS_IL2CPP_INT32 CollectionStackGetCount(CHAOS_IL2CPP_INTPTR handle) noexcept;
void CollectionStackPush(CHAOS_IL2CPP_INTPTR handle, CHAOS_IL2CPP_INTPTR value) noexcept;
CHAOS_IL2CPP_INTPTR CollectionStackPop(CHAOS_IL2CPP_INTPTR handle) noexcept;
CHAOS_IL2CPP_INTPTR CollectionStackPeek(CHAOS_IL2CPP_INTPTR handle) noexcept;
CHAOS_IL2CPP_INT32 CollectionStackTryPop(CHAOS_IL2CPP_INTPTR handle, CHAOS_IL2CPP_INTPTR* out_value) noexcept;
CHAOS_IL2CPP_INT32 CollectionStackTryPeek(CHAOS_IL2CPP_INTPTR handle, CHAOS_IL2CPP_INTPTR* out_value) noexcept;
void CollectionStackClear(CHAOS_IL2CPP_INTPTR handle) noexcept;

// Comparer<T> helpers
CHAOS_IL2CPP_INT32 CollectionComparerCompareInt32(CHAOS_IL2CPP_INT32 x, CHAOS_IL2CPP_INT32 y) noexcept;

}  // extern "C"
