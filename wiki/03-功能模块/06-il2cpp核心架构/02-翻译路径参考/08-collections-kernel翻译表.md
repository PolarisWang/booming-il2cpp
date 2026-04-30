# Collections Kernel 翻译表

## 概述

System.Collections.Generic 集合类型热路径方法的 Kernel 优化翻译路径。用直接 C++ helper 调用替代 managed invoke，消除 6 个运行时参数的传递开销。

## 约束

- **通用约束**：所有 Kernel 翻译路径共享：void 6 runtime params、`_KernelArgs` struct 承载参数、`chaos::il2cpp::runtime_core::*` helper 调用、返回 `CHAOS_BRIDGE_STATUS_OK`（成功）/ `CHAOS_BRIDGE_STATUS_INVALID_ARGUMENT`（参数校验失败）
- **版本断言**：struct 字段偏移运行时校验（`collection_field_offset`），偏移不匹配时返回 `CHAOS_BRIDGE_STATUS_NOT_SUPPORTED`（hot-update 回退 managed invoke）
- **Plugin 优先级**：165（Kernel 类），位于 array@160 和 exception@170 之间
- **Contract ID**：`collections-kernel-v1`

## 翻译路径

### 1. List\<T\>

| 方法 | 签名 | Helper | 参数 |
|------|------|--------|------|
| `get_Count` | `int32()` | `CollectionListGetCount(this_arg)` | this |
| `get_Item` | `T(int32)` | `CollectionListGetItem(this_arg, arg0)` | this, int32 index |
| `set_Item` | `void(int32, T)` | `CollectionListSetItem(this_arg, arg0, arg1)` | this, int32 index, T value |
| `Add` | `void(T)` | `CollectionListAdd(this_arg, arg0)` | this, T value |
| `Insert` | `void(int32, T)` | `CollectionListInsert(this_arg, arg0, arg1)` | this, int32 index, T value |
| `RemoveAt` | `void(int32)` | `CollectionListRemoveAt(this_arg, arg0)` | this, int32 index |
| `IndexOf` | `int32(T)` | `CollectionListIndexOf(this_arg, arg0)` | this, T value |
| `Contains` | `bool(T)` | `CollectionListContains(this_arg, arg0)` | this, T value |

### 2. Dictionary\<K,V\>

| 方法 | 签名 | Helper | 参数 |
|------|------|--------|------|
| `get_Count` | `int32()` | `CollectionDictionaryGetCount(this_arg)` | this |
| `get_Item` | `V(K)` | `CollectionDictionaryGetItem(this_arg, arg0)` | this, K key |
| `set_Item` | `void(K, V)` | `CollectionDictionarySetItem(this_arg, arg0, arg1)` | this, K key, V value |
| `TryGetValue` | `bool(K, V&)` | `CollectionDictionaryTryGetValue(this_arg, arg0, arg1)` | this, K key, V* out_value |
| `ContainsKey` | `bool(K)` | `CollectionDictionaryContainsKey(this_arg, arg0)` | this, K key |
| `Remove` | `bool(K)` | `CollectionDictionaryRemove(this_arg, arg0)` | this, K key |
| `Add` | `void(K, V)` | `CollectionDictionaryAdd(this_arg, arg0, arg1)` | this, K key, V value |

### 3. HashSet\<T\>

| 方法 | 签名 | Helper | 参数 |
|------|------|--------|------|
| `get_Count` | `int32()` | `CollectionHashSetGetCount(this_arg)` | this |
| `Add` | `bool(T)` | `CollectionHashSetAdd(this_arg, arg0)` | this, T value |
| `Contains` | `bool(T)` | `CollectionHashSetContains(this_arg, arg0)` | this, T value |
| `Remove` | `bool(T)` | `CollectionHashSetRemove(this_arg, arg0)` | this, T value |

### 4. Queue\<T\>

| 方法 | 签名 | Helper | 参数 |
|------|------|--------|------|
| `get_Count` | `int32()` | `CollectionQueueGetCount(this_arg)` | this |
| `Enqueue` | `void(T)` | `CollectionQueueEnqueue(this_arg, arg0)` | this, T value |
| `Dequeue` | `T()` | `CollectionQueueDequeue(this_arg)` | this |
| `Peek` | `T()` | `CollectionQueuePeek(this_arg)` | this |

### 5. Stack\<T\>

| 方法 | 签名 | Helper | 参数 |
|------|------|--------|------|
| `get_Count` | `int32()` | `CollectionStackGetCount(this_arg)` | this |
| `Push` | `void(T)` | `CollectionStackPush(this_arg, arg0)` | this, T value |
| `Pop` | `T()` | `CollectionStackPop(this_arg)` | this |
| `Peek` | `T()` | `CollectionStackPeek(this_arg)` | this |
| `TryPop` | `bool(T&)` | `CollectionStackTryPop(this_arg, arg0)` | this, T* out_value |
| `TryPeek` | `bool(T&)` | `CollectionStackTryPeek(this_arg, arg0)` | this, T* out_value |

## 版本断言

每个 kernel stub 编译时常量 `kContractVersion`（`collections-kernel-v1` → `0x8A3B7C01u`），在函数入口处通过 `metadata_registration` 读取运行时 contract version：
- `field_offset_count > 0` 时：读取 `*static_cast<const uint32_t*>(metadata_registration->field_offsets)` 与 `kContractVersion` 比较
- `field_offset_count == 0` 时（开发模式）：跳过校验，不阻塞
- 不匹配时返回 `CHAOS_BRIDGE_STATUS_NOT_SUPPORTED`，触发插件链回退到 managed-invoke

版本断言表达式由 `RuntimeSkeletonCollectionsKernelAbiFactory.GetVersionAssertionExpression()` 生成。

## 类型映射

| Managed 类型 | C++ 类型 |
|-------------|---------|
| `System.Boolean` | `bool` |
| `System.Byte` | `CHAOS_IL2CPP_UINT8` |
| `System.SByte` | `CHAOS_IL2CPP_INT8` |
| `System.Int16` | `CHAOS_IL2CPP_INT16` |
| `System.UInt16` / `System.Char` | `CHAOS_IL2CPP_UINT16` |
| `System.Int32` | `CHAOS_IL2CPP_INT32` |
| `System.UInt32` | `CHAOS_IL2CPP_UINT32` |
| `System.Int64` | `CHAOS_IL2CPP_INT64` |
| `System.UInt64` | `CHAOS_IL2CPP_UINT64` |
| `System.Single` | `float` |
| `System.Double` | `double` |
| `System.IntPtr` | `CHAOS_IL2CPP_INTPTR` |
| `System.UIntPtr` | `CHAOS_IL2CPP_UINTPTR` |
| 泛型参数 (`!0`, `!1`, `!!0`) | `void*` (reference carrier) |

## 存储后端

| 集合类型 | C++ 存储 | factory |
|---------|---------|---------|
| `List<T>` | `ListRuntimeStorage<T>` (vector) | `require_list_runtime_storage<T>(handle)` |
| `Dictionary<K,V>` | `DictionaryRuntimeStorage<K,V>` (vector of pairs) | `require_dictionary_runtime_storage<K,V>(handle)` |
| `HashSet<T>` | `HashSetRuntimeStorage<T>` (vector) | `require_hashset_runtime_storage<T>(handle)` |
| `Queue<T>` | `QueueRuntimeStorage<T>` (deque) | `require_queue_runtime_storage<T>(handle)` |
| `Stack<T>` | `StackRuntimeStorage<T>` (deque) | `require_stack_runtime_storage<T>(handle)` |

## 架构映射

| 层级 | 文件 | 职责 |
|------|------|------|
| Family Gate | `NativeReferenceProofEmitter.CollectionsKernelFamily.cs` | 5 个集合类型前缀守卫（List/Dictionary/HashSet/Queue/Stack）+ `TryBuildCollectionsKernelStub` |
| ABI | `RuntimeSkeletonCollectionsKernelAbi.cs` | SubjectId 后缀解析、类型映射、Helper 选择 |
| Template | `NativeReferenceProof.RuntimeSkeleton.CollectionsKernelStub.cpp.scriban` | `_KernelArgs` struct、6 runtime params void、helper call |
| Runtime | `runtime_core.h/.cpp` | 20 个 `chaos::il2cpp::runtime_core::Collection*` 实现（List×8 + Dictionary×7 + HashSet×4 + Queue×4 + Stack×6，含 widened 方法） |
| Storage | `chaos/collection.h` | `HashSetRuntimeStorage<T>` / `QueueRuntimeStorage<T>` / `StackRuntimeStorage<T>` |
| Registration | `NativeReferenceProofEmitter.GeneratedPluginExports.cs` | `[RuntimeSkeletonFamilyPlugin("collections-kernel", ..., 165, Kernel)]` |
