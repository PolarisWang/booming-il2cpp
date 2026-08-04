# P0 GC 正确性缺陷修复计划

> **状态：全部已实现。** 2026-05-16/17 的 CRAG GC 改进提交已包含全部 4 项 fix。此文档从"执行计划"转为"验证追踪"。

**目标：** ~~修复 GC 子系统 4 项 P0 正确性风险~~ → **确认 4 项 P0 fix 均已实现并验证。**

## 实现验证表

| P0 # | 缺陷 | 实现状态 | 代码位置 |
|------|------|----------|----------|
| P0-1 | BGC finalization 缺失 | **✅ 已实现** | `gc_bgc.cpp:497-516` — BgcThreadMain 在 FINISHED→IDLE 后异步执行 `bgc_dead_finalizables_` |
| P0-2 | BGC weak handle 不更新 | **✅ 已实现** | `gc_bgc.cpp:518-531` — BgcThreadMain 中 finalization 后调用 `GcProcessCollectedWeakHandles()` |
| P0-3 | 卡表 4GB 硬限制 | **✅ 已实现** | `gc_card_table.cpp:42-102` — L1 动态双向扩容，`g_card_l1` 为 unique_ptr 可 grow，无 4GB 上限 |
| P0-4 | GcLayout 4096 槽位上限 | **✅ 已实现** | `gc_layout.h:91-97` — 4096 仅为 `kGcLayoutMinCapacity`，`GcLayoutTable` 在 load factor >75% 时自动 GrowTable |

### 基础设施函数确认

| 函数 | 位置 | 状态 |
|------|------|------|
| `g_old_gen.CollectDeadFinalizables()` | `gc_old_gen.cpp:2017` | **已实现** — 遍历 finalizers_ 列表收集死亡对象 |
| `GcCollectDeadWeakHandles()` | `engine_lifecycle.cpp:299` | **已实现** — 遍历 handle table 收集死亡 weak handle |
| `GcProcessCollectedWeakHandles()` | `engine_lifecycle.cpp:322` | **已实现** — null 处理已收集的 weak handle |
| `GcProcessDependentHandlesAfterBgc()` | `engine_lifecycle.cpp:337` | **已实现** — dependent handle 后处理 |

### 未验证项（待补充测试）

- `gc_bgc_smoke.cpp`: 尚无 BGC finalization 专用测试用例
- `gc_stress_test.cpp`: 压力场景中未包含 finalizer 对象
- `gc_layout_test.cpp`: 不存在独立文件（GcLayout 测试集成在其他测试中）

## 原始计划备份

以下为原始计划内容（保留以供参考，实现已验证）：

## P0-1: BGC finalization 缺失

### 问题
BGC 标记死亡的可终结（finalizable）对象后，不触发 finalizer 执行。Finalizers 只在 STW full GC (`chaos_gc_collect()`) 中同步执行，阻塞所有线程。非托管资源（文件句柄、GDI 对象）会泄漏直至下一次 STW full GC。

### 方案
在 BGC cycle 完成后，由 BGC thread（或独立 finalization thread）异步执行 finalization，不阻塞 mutator。

### 文件修改

| 文件 | 改动 |
|------|------|
| `src/native/runtime-core/gc/gc_old_gen.h` | 新增 `RunFinalizersForUnreachable(Span<void*> unreachable)` 方法 |
| `src/native/runtime-core/gc/gc_old_gen.cpp` | BgcSweep 完成后收集死亡 finalizable 对象列表，`BgcController` 回调触发 finalization |
| `src/native/runtime-core/gc/gc_bgc.h` | 新增 `finalization_pending_` 标记、死亡对象列表传递机制 |
| `src/native/runtime-core/gc/gc_bgc.cpp` | BgcThreadMain 在 FINISHED→IDLE 转换前检查 finalization_pending_，异步执行 finalizer |
| `src/native/runtime-core/gc/gc_events.h` | 新增 `GC_BGC_FINALIZE` event type |

### 关键实现点
- BgcSweep 遍历 finalizers_ 列表，对死亡对象记录到 `bgc_dead_finalizables_` vector
- BGC thread 在 CONCURRENT_SWEEP→COMPACT_NEEDED 之后、或 FINISHED→IDLE 之前，检查 vector 并调用 finalizer
- 注意：BGC thread 执行 finalizer 时需要短暂进入 cooperative mode，或新建 finalization thread
- Finalizer 重新注册逻辑：若 finalizer 调用了 `GC.ReRegisterForFinalize`，对象需重新加入监控

### 验证
- `gc_bgc_smoke.cpp`: 新增测试用例 — 分配 finalizable 对象，触发 BGC，验证 finalizer 在合理时间内被调用
- `gc_stress_test.cpp`: 在压力场景中包含有 finalizer 的对象，验证无泄漏

---

## P0-2: BGC weak handle 不更新

### 问题
BGC cycle 完成后，weak handle（`GCHandle.Weak`、`WeakReference`、`WeakTrackResurrection`）不更新。BGC 标记为死亡的对象，weak handle 仍返回有效指针，语义错误。

### 方案
在 BGC 完成 concurrent sweep 后、进入 COMPACT_NEEDED 前，遍历 handle table 对死亡对象的 weak handle 进行 null 处理。

### 文件修改

| 文件 | 改动 |
|------|------|
| `src/native/runtime-core/core/engine_lifecycle.h` 或 `gc_events.h` | 新增 `GcProcessWeakHandlesAfterBgc(const MarkBitmapAccess&)` 声明 |
| `src/native/runtime-core/core/engine_lifecycle.cpp` | 实现 `GcProcessWeakHandlesAfterBgc` — 遍历 handle table，对 weak handle 的对象检查 mark bitmap |
| `src/native/runtime-core/gc/gc_bgc.cpp` | BgcThreadMain 在 CONCURRENT_SWEEP 完成后、设 COMPACT_NEEDED 前，调用 `GcProcessWeakHandlesAfterBgc` |
| `src/native/runtime-core/gc/gc_old_gen.h` | 暴露 mark bitmap 遍历接口给 BGC 使用 |

### 关键实现点
- 与 full GC 的 `GcProcessWeakHandlesAfterFullGC` 类似，但仅使用 mark bitmap 判断（无 forwarding 逻辑）
- 需要 BGC 线程能安全访问 g_old_gen 的 mark bitmap（此时 BGC sweep 已保留 bitmap）
- WeakTrackResurrection: 对已调用 finalizer 的对象，在 resurrection 处理后 null
- 必须在 COMPACT_NEEDED 前执行，因为 StwCompact 会清除 bitmaps

### 验证
- `gc_bgc_smoke.cpp`: 新增测试 — 创建 weak reference 指向 old-gen 对象，触发 BGC，验证 weak reference 变为 null
- 验证 WeakTrackResurrection 语义：有 finalizer 的对象在 finalization 前不 null

---

## P0-3: 卡表 4GB 硬限制

### 问题
卡表 L1 固定 64K 条目，每条目覆盖 64KB（128 card × 512B），最大覆盖 4GB。堆超过 4GB 时，`DirtyCard()` barrier 在 `addr < g_heap_base` 处静默跳过，导致 UAF。

### 方案
将 L1 从固定数组改为动态扩展结构。当 L1 索引溢出当前容量时，倍增 L1 并重新分配，保持 barrier 的 ~6 指令性能。

### 文件修改

| 文件 | 改动 |
|------|------|
| `src/native/runtime-core/gc/gc_card_table.h` | L1 从 `std::atomic<CardSegment*> g_card_l1[kCardL1Entries]` 改为动态指针 + 容量 |
| `src/native/runtime-core/gc/gc_card_table.cpp` | `GcRegisterHeapRange` 中增加 L1 溢出检测与扩容逻辑；`g_heap_base` 废弃或改为偏移基址 |

### 关键实现点
- L1 改为 `std::atomic<CardSegment*>* g_card_l1` + `std::atomic<size_t> g_card_l1_capacity`
- 扩容时分配新数组（2x 大小），拷贝旧指针，atomic 替换，CAS 保证线程安全
- 扩容后旧数组不能立即释放（可能有线程正在用），使用 RCU 或 epoch 回收
- barrier 中的 `g_card_l1[seg_idx]` 变成指针解引用，但 cache miss 不变（L1 本身就在内存中）
- 为保持 ~6 指令 barrier，考虑将 `g_card_l1` 设为 `thread_local` cache + global fallback（保持热路径性能）

### 替代方案
- 去掉 L1 上限：改为 `std::unordered_map<uintptr_t, CardSegment*>` — 但会破坏 inline barrier 性能，不推荐
- 推荐：L1 初始 64K，倍增扩容（64K→128K→256K...），对应覆盖 4GB→8GB→16GB...

### 验证
- `gc_card_table_test.cpp`: 新增测试 — 注册 >4GB 堆范围，验证 barrier 对所有地址生效
- stress test 配置大堆场景，验证无 UAF

---

## P0-4: GcLayout 4096 槽位上限

### 问题
GcLayout 哈希表初始容量 4096，400+ DLL 热更新场景下类型数可能远超此值。超过后探测链性能退化严重（近似 O(n) 查找）。

### 方案
将哈希表从固定容量升级为动态扩容。当 load factor > 0.75 时，2x 扩容并 rehash。

### 文件修改

| 文件 | 改动 |
|------|------|
| `src/native/runtime-core/gc/gc_layout.h` | `kInitialCapacity` 改为初始值（保留 4096）；`entries_` 从固定数组改为动态分配指针 |
| `src/native/runtime-core/gc/gc_layout.cpp` | `GcLayoutRegistry::Register()` 中增加 load factor 检测与 rehash；`GcLayoutRegistry::Find()` 需兼容 rehash |

### 关键实现点
- 借鉴 unordered_dense 的渐进式 rehash 或一次性 rehash（GC layout 注册是写路径，性能可接受一次性 rehash）
- Rehash 时分配新表，逐 entry 重新插入，atomic 替换指针
- 由于 Find() 在 GC mark 路径被频繁调用（hot path），需要指针 volatile 读确保 rehash 后读到新表
- 指针数组改为 `std::atomic<LayoutEntry*>*` 以支持无锁读

### 验证
- `gc_layout_test.cpp`: 新增测试 — 注册 5000+ 类型，验证查找正确性和性能
- stress test 加载 400+ DLL 模拟，验证 GC mark 阶段的 layout 查找无退化

---

## 执行顺序与依赖

```
P0-1 ──┐
        ├── 独立，无交叉依赖
P0-2 ──┤ (但 P0-2 需要 P0-3 的 bitmap 访问接口暴露)
        │
P0-3 ──┤ 独立
        │
P0-4 ──┤ 独立
```

实际推荐顺序：P0-3 → P0-4 → P0-1 → P0-2（P0-2 依赖 P0-1 的 mark bitmap 接口）

## 关键测试与验证命令

```bash
# 编译
cmake --build build/debug --target chaos_runtime_core

# BGC smoke test
build/debug/tests/bgc_smoke_test.exe

# Card table test
build/debug/tests/gc_card_table_test.exe

# Layout test
build/debug/tests/gc_layout_test.exe

# Stress test
build/debug/tests/gc_stress_test.exe --scenario all
```

## 执行交接

计划已完成并保存到 `docs/dev/in-progress/gc-defect-fixes-p0/plan-v1-01.md`。本计划只基于已确认并清零的问题输入；如果要改动边界或关键拍板，请先返回 brainstorming。准备好执行了吗？
