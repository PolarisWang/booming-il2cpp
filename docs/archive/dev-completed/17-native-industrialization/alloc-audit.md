# native 模块裸 `new` 与 `reinterpret_cast` 全量审计清单

> 状态：审计中（阶段一 · 白名单扫描）
> 审计范围：`src/native/`（约 123,000 行 C++）
> 审计时间：2026-09-02
> 说明：本文档只做审计与修复建议，未修改任何源码。

---

## 0. 审计方法说明

### 0.1 扫描对象
对 `src/native/` 下所有 C++ 源文件用 Grep（rusty-grep）分级扫描 5 类模式：裸 `new`、`reinterpret_cast`、`malloc/free/calloc/realloc`、`delete`、`std::unique_ptr/shared_ptr`。

### 0.2 去噪原则
- 裸 `new` 仅统计 `new T` 对象构造；`new[]`、placement new、`CHAOS_IL2CPP_NEW(T)` 宏体不计入。
- `new[]` 单独统计，必须配 `delete[]`。
- `reinterpret_cast` 区分「指针/整型句柄往返」（合法）与「类型双关」（UB），详见第 5 节。

### 0.3 风险分级
- HIGH：热点路径上的裸 new 或类型双关 reinterpret_cast；无配对释放的分配。
- MEDIUM：非热点路径但缺乏安全护栏（RAII / static_assert / 边界检查）。
- LOW：存在护栏（placement + 自定义分配器、static_assert、ABI 必需的句柄转换）。

---

## 1. 按模块统计汇总

### 1.1 裸 new（标量对象构造，排除 new[]/placement）

| 模块 | 数量 | 主要分配对象 | 风险倾向 |
|------|:---:|------|------|
| runtime-core/gc | 91 | GcLayoutTable、std::atomic[]、worklist | HIGH |
| runtime-core | 56 | DecimalCarrier、MemoryDomain、SyncBlock、ManagedThread | HIGH |
| jit | 22 | Map、JitPrecode | MEDIUM |
| runtime-core/runtime_stubs | 13 | ListRuntimeStorage、PatchMethod、pthread_mutex_t | HIGH（泄漏） |
| runtime-core/core | 11 | SyncBlock | MEDIUM |
| pal | 11 | CRITICAL_SECTION / CONDITION_VARIABLE / SRWLOCK | MEDIUM |
| jit/tree | 9 | arena | LOW |
| common/chaos | 9 | DictionaryRuntimeStorage | MEDIUM |
| interpreter/interp_entry | 8 | RegisterMethod、IRMethod | MEDIUM |
| interpreter | 5 | -- | MEDIUM |
| pal/chaos/pal | 4 | -- | LOW |
| diagnostics/eventpipe | 4 | -- | LOW |
| runtime-core/instantiation | 3 | argv、raw_data（new[]） | MEDIUM |
| common | 3 | SpanRuntimeEntry、MemoryRuntimeEntry | LOW |
| hot-update | 2 | vector（new[]） | LOW |
| runtime-core/reflection | 1 | -- | LOW |
| 合计 | 约 176 | | |

### 1.2 new[]（须配 delete[]）-- 基本全部正确配对
- interpreter_dispatch.cpp:78-79 -> delete[] argv/raw_data（241-242）✓
- tier_manager.cpp:68 -> delete[] data->methods（47, 108）✓
- execution_context.cpp -> delete[] heap_values（41）✓
- gc_layout.h:137-138 -> delete[] keys/values（143-144）✓
- pal_sync_posix.cpp:254 -> delete[] nodes（296, 336）✓
- gc_card_table.cpp:20 -> 待确认内存来源（见 H2）

### 1.3 reinterpret_cast（按模块）
| 模块 | 数量 | 占比 | 类别倾向 |
|------|:---:|:---:|------|
| runtime-core/runtime_stubs | 594 | 30% | 桩句柄/互操作（指针转换） |
| runtime-core | 375 | 19% | 句柄、Padding、多态下转 |
| runtime-core/gc | 331 | 17% | 卡表/位图/基数堆运算 |
| runtime-core/core | 210 | 11% | 同步、句柄 |
| jit | 102 | 5% | 代码缓冲/卡页 |
| runtime-core/reflection | 86 | 4% | 元数据指针 |
| pal | 79 | 4% | 句柄封装 |
| interpreter | 78 | 4% | 栈帧/槽位 |
| bootstrap | 54 | 3% | 启动 ABI |
| interpreter/interp_entry | 45 | 2% | 入口桩 |
| common/chaos | 42 | 2% | span/句柄 |
| 其余模块 | 约 80 | 4% | 分散 |
| 合计 | 约 1976 | 100% | |

### 1.4 malloc/free/calloc/realloc
分布：runtime-core/gc(7)、interpreter/interp_entry(7)、jit(4)、runtime-core/runtime_stubs(3)、runtime-core(3)、pal(3)、jit/tree(2)、core(1)、chaos(1)、interpreter(1)。大部分使用 std:: 前缀且配对良好；interop_stubs.cpp 存在堆族分叉风险（见 H4）。

### 1.5 裸 delete（排除 = delete; / 注释）
pal(22)、runtime-core(15)、runtime-core/gc(3)、jit(3+)、engine-bridge(3)、runtime-core/runtime_stubs(2)、common/chaos(2)、interpreter/interp_entry(1)、hot-update(1)。

---

## 2. 高风险项清单（HIGH · 立即修）

### H1. collection_stubs.cpp 集合存储零释放（确定性泄漏）
- runtime_stubs/collection_stubs.cpp:34 / 53 / 226 / 307 -> new ListRuntimeStorage / HashSetRuntimeStorage / QueueRuntimeStorage / StackRuntimeStorage。
- 全目录 grep delete 未发现该文件释放点 -> 四个 storage 终身未 free。
- 修复：在对应类型的 Dispose/Finalize 补 delete storage；或改用 RAII 包装。

### H2. gc_card_table.cpp:20 裸 new[] 与 calloc 混用
- :46 g_card_bundle = std::calloc（安全）；:20 new std::atomic<CardSegment*>[] 需确认内存来源与析构一致，避免 new[] 配 free() 造成堆不匹配。卡表处于写障碍热路径。
- 修复：统一走 GC 专用分配器或 arena。

### H3. parse_convert.cpp Decimal 类型双关 UB
- 20+ 处 reinterpret_cast<const DecimalCarrier*>（315, 343-356, 367-380, 399-400, 418, 440-441, 455-462, 474-481, 602, 608）。若来源为非对齐 buffer，则属强别名/对齐 UB。
- 修复：改用 std::bit_cast 或经内存拷贝传递。

### H4. interop_stubs.cpp:263-321 互操作热路径堆族分叉
- MarshalAllocByteCount(263)、MarshalFree(296-298)、MarshalAllocHGlobal。std::free(298) 与 _aligned_free(296) 并存 -> Win 堆族混用风险。
- 修复：统一收口到 CHAOS_IL2CPP_MALLOC / FREE 宏。

### H5. jit_precode.cpp:654 + deferred-delete 队列
- new JitPrecode()，735-752 静态 defer-delete 队列（每 1024 才 drain），无上限水位监控，JIT 抖动会积压。
- 修复：加水位告警，并确认 JitPrecode 析构为空。

### H6. runtime-core/core 与 runtime-core 类型双关候选（待定）
- 210 / 375 处中存在把 void* 或 uint8_t* 读成 uint64_t/字段的用法（卡组、手指表、位图）。clang-tidy 未强制 strict-aliasing。
- 修复：连续内存位图读取优先用 std::bit_cast 替换。

---

## 3. 中风险项清单（MEDIUM · 计划修）

- M1. sync_mutex.cpp:75 return new SyncBlock() -- 无 RAII/引用计数，需追踪生命周期（建议 shared_ptr 或专用 Arena）。
- M2. jit_inline.cpp:26, 310 new Map() -- 310 构造 / 313 delete 配对 VALID；建议并入 JIT arena 降停顿。
- M3. collection.h:47 new DictionaryRuntimeStorage -- 与 H1 同类，若泄漏则升级 HIGH 一并处理。
- M4. pal_sync_win32.cpp new CRITICAL_SECTION / CONDITION_VARIABLE / SRWLOCK -- delete reinterpret_cast 配对 VALID；建议封装 RAII + static_assert(sizeof)。
- M5. patch_method_lower.cpp 5 处 new RegisterMethod()/IRMethod() -- 需确认 cached_ir/cached_reg_method 释放链，建议改 unique_ptr。
- M6. thread_pool.cpp:83, 690 -- WorkerLocalQueue / ContextRunItem 高并发热路径，建议对象池化。
- M7. tier_manager.cpp / interpreter_dispatch.cpp new ModuleTierData -- delete 配对 VALID，可并入 tier 专用 arena。

---

## 4. 低风险项清单（LOW · 监控即可）

- L1. common/span.cpp:53/67/96 SpanRuntimeEntry/MemoryRuntimeEntry -- 句柄工厂，语义清晰。
- L2. jit/tree/jit_tree_builder.cpp arena（realloc + free）-- 配对良好。
- L3. ABI 指针/整型句柄转换（pal/common/runtime_stubs）-- 合法句柄通道，建议集中到工具函数便于审计。
- L4. pal_wakeable_* new PalWakeable -- 低频创建、delete w 配对。
- L5. runtime-core/gc 的 calloc/realloc 内部数组 -- 有明确所有权与析构。
- 正向样板：已用 std::unique_ptr 的 5 处（struct_marshal_descriptors、gc_card_table、gc_heap_manager、gc_old_gen、wait_handle）。

---

## 5. runtime-core 高密度区（1610 处 reinterpret_cast）专项区分

### 5.1 指针转换（ABI 必需，LOW）-- 占绝大多数
- 句柄/整型往返 reinterpret_cast<CHAOS_IL2CPP_INTPTR>：span、async、collection_stubs、mutex、interop_stubs 近 40%。
- Opaque 句柄具体化：pal 实现。
- GC 连续内存指针运算：卡表/位图按字节行走读。
- 元数据指针归位：reflection/metadata。

### 5.2 类型双关（UB 风险，HIGH/MEDIUM）
- DecimalCarrier 扁平化（parse_convert.cpp，见 H3）。
- 位图/密集 flag 组、GC 位标记（gc_layout/gc_region :263/:341/:565 lock-free 位集）。
- 判定模板：reinterpret_cast<T*>(&other) 或 reinterpret_cast<uint64_t*>(uint8_t arr) = 类型双关；reinterpret_cast<T*>(uintptr_t) 且 T 为原类型 = 合法指针还原。

---

## 6. 推荐修复策略（替代方案模板）

### 6.1 内存分配统一化（裸 new -> 宏/RAII）
凡裸 `new X` -> 至少改 `CHAOS_IL2CPP_NEW(X)`（native_types.h:395）；生命周期明确的单对象 -> std::unique_ptr；跨边界共享 -> std::shared_ptr；集合/JIT/tier 高频小对象 -> 专用 arena/内存池。

### 6.2 配对核查（new -> delete）
- new[] -> delete[]：全部匹配。
- new T -> delete + 同一具体类型：全部匹配（pal 需 reinterpret_cast 回原类型）。
- calloc -> free：匹配。
- _aligned_malloc -> _aligned_free：匹配。
- new ListRuntimeStorage（H1）：无释放（泄漏）。

### 6.3 std::bit_cast 替换 reinterpret_cast（C++20）
`auto* c = reinterpret_cast<const DecimalCarrier*>(carrier_ptr);` [现状：类型双关 UB]
`DecimalCarrier c = std::bit_cast<DecimalCarrier>(raw);` [推荐：需 trivially-copyable]
gcc/clang/msvc 均支持 C++20 std::bit_cast；优先替换 parse_convert 的 Decimal 双关。

### 6.4 INTEROP 分配去分叉
将 interop_stubs.cpp 的 malloc / free / _aligned_realloc 三态归并单入口宏，Win 固定 _aligned_* 族、其余 std:: 族。

---

## 7. 执行顺序建议

- 批次 1（立即 HIGH）：H1 collection_stubs 补 delete storage；H3/H6 Decimal 类型双关改 bit_cast；H4 interop 堆族收口；H2 卡表来源统一。
- 批次 2（一轮 HIGH 热路径）：H5 JIT precode defer 水位；M6 线程池对象池。
- 批次 3（规划 MEDIUM）：M1/M3/M5/M7 所有权与 arena 化；全量裸 new -> CHAOS_IL2CPP_NEW 收口（约 176 处）。
- 批次 4（持续监控 LOW）：span/eventpipe/bootstrap 保持；pal 句柄 cast 加 static_assert；新代码 gate：类型双关必须 static_assert 或 std::bit_cast。

---

## 附：核心证据路径
- src/native/runtime-core/runtime_stubs/collection_stubs.cpp（H1）
- src/native/runtime-core/parse_convert.cpp（H3）
- src/native/runtime-core/runtime_stubs/interop_stubs.cpp（H4）
- src/native/runtime-core/gc/gc_card_table.cpp（H2）
- src/native/jit/jit_precode.cpp（H5）
- src/native/common/chaos/native_types.h（分配宏 145-147 / 395-397）
- src/native/runtime-core/runtime_core.h:412（DecimalCarrier 定义）
- src/native/pal/pal_sync_win32.cpp（LOW 样板）
- src/native/jit/tree/jit_tree_builder.cpp（arena 样板）
