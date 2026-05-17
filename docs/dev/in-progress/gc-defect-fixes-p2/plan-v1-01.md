# P2 GC 功能/可用性缺陷修复计划

> **面向执行 Agent：** 本计划覆盖 7 个 P2（功能/可用性）缺陷，按优先级从高到低排列。每个子任务独立可验证。

**目标：** 修复 GC 子系统 7 项 P2 缺陷：Managed GC API 缺失、LOH 无 compaction、GcSlotMap 注册效率、Card barrier DC 优化、Cross-page compaction budget 过小、Interpreter 内存泄漏、Thread registry 不可回收

**架构：** 这些修复分布在 GC managed API 层、LOH 管理、根扫描注册、写屏障、compaction 策略、interpreter 等多个独立领域

**技术栈：** C++17, managed interop, VirtualAlloc, worker pool

**架构审核模式：** normal

**结构告警重点：**
- Managed GC API 新增函数注意 extern "C" ABI 统一放在 gc_helpers.h
- LOH compaction 可复用 old-gen 已有 compaction 框架
- Interpreter 内存泄漏修复注意分散的三个 call site 统一处理

**设计文档：** `docs/discuss/20260516-memory-gc-comprehensive-evaluation.md`

**问题清零来源：** direct user confirmation

**计划来源：** direct-plan

**预期知识沉淀：** wiki/03-功能模块/06-il2cpp核心架构/01-翻译管线/24-CRAG-GC架构参考.md

**收尾约束：** 执行完成后必须进入"结构告警与架构审视 → 测试通过 → 归档 completed → 合并&提交"固定链路。

---

## P2-1: Managed GC API 缺失

### 问题
`GC.GetTotalMemory`、`GC.AddMemoryPressure`、`GC.RemoveMemoryPressure` 未实现。不影响 C++ 运行时正确性，但阻止 C# 层 GC 调优。

### 文件修改

| 文件 | 改动 |
|------|------|
| `src/native/runtime-core/gc/gc_helpers.h` | 新增 `chaos_gc_get_total_memory()`、`chaos_gc_add_memory_pressure()`、`chaos_gc_remove_memory_pressure()` 的 extern "C" 声明 |
| `src/native/runtime-core/gc/gc_region.h/cpp` | 实现 `GetTotalAllocated()` 遍历 old gen + LOH + nursery |
| `src/native/runtime-core/gc/gc_old_gen.h/cpp` | 暴露 `TotalAllocated()` |
| `src/native/runtime-core/gc/gc_loh.h/cpp` | 暴露 `TotalAllocated()` |
| `contracts/native/v0/runtime_abi.h` | 新增 ABI entry points |

### 关键实现点
- `GetTotalMemory`: old_gen.TotalAllocated() + loh.TotalAllocated() + nursery usage
- `AddMemoryPressure`: 增加 `external_memory_pressure_` atomic counter，触发 GC 决策时考虑
- `RemoveMemoryPressure`: 递减 counter

### 验证
- 从 managed 侧调用 GC.GetTotalMemory() 验证返回值合理
- AddMemoryPressure 后验证 GC 触发提前

---

## P2-2: LOH 无 compaction

### 问题
Large Object Heap (>=85KB) 无 compaction 能力。CoreCLR 5.0+ 支持 opt-in LOH compaction（`GCSettings.LargeObjectHeapCompactionMode`）。

### 文件修改

| 文件 | 改动 |
|------|------|
| `src/native/runtime-core/gc/gc_loh.h` | 新增 `CompactMode` 参数、`Compact()` 声明、`compact_mode_` 配置 |
| `src/native/runtime-core/gc/gc_loh.cpp` | 实现 `Compact()` — 遍历 LOH segment，标记存活对象，memmove 紧凑排列，更新引用 |
| `src/native/runtime-core/gc/gc_old_gen.cpp` | `Collect()` 中 LOH sweep 后调用 `g_loh.Compact()` |

### 关键实现点
- LOH segment 很大（>=85KB），compact 代价高，默认不启用
- 通过 `CompactMode` 控制（None/OnRequest/Automatic）
- compact 时机：仅当 LOH 空闲段数量 > 阈值或 LOH 碎片率 > 阈值

### 验证
- `gc_loh_test.cpp`: 分配并释放 LOH 对象产生碎片，触发 compact，验证合并
- stress test 验证大对象无正确性问题

---

## P2-3: GcSlotMap O(n) 注册

### 问题
`GcRegisterSlotMap` 使用线性插入 + memmove（`gc_root_scanner.cpp`），注册大量方法（400+ DLLs × 数千方法）时有 O(n²) 风险。

### 方案
改为二分查找（`std::lower_bound`）+ memmove 插入，或使用有序容器（`std::set`）+ 批量排序插入。

### 文件修改

| 文件 | 改动 |
|------|------|
| `src/native/runtime-core/gc/gc_root_scanner.cpp` | `GcRegisterSlotMap` 中线性扫描替换为 `std::lower_bound` 二分查找，memmove 插入不变 |

### 验证
- 注册 10000+ slot map 后测量总注册时间
- 对比优化前优化后的时间，确认 O(n log n) vs O(n²)

---

## P2-4: 写屏障无脏卡检测

### 问题
Card write barrier 在每次托管字段写入时都执行完整的两级定位 + store，不检查 card 是否已 dirty。CoreCLR 有 DC 优化（先读再写）。

### 文件修改

| 文件 | 改动 |
|------|------|
| `src/native/runtime-core/gc/gc_card_table.h` | `DirtyCard()` 中增加 `if (seg->cards[card_idx] != 0xFF)` 检测，脏则跳过 store |

### 关键实现点
- 额外的一次 load + compare，比原来多 ~1 条指令
- 高频重复写场景（状态机、循环内更新同一字段）节省后续的 store
- 注意 card 为 0xFF 的概率：写屏障频率越高，节省越多

### 验证
- 基准测试对比：有序集、集合等高频字段更新场景，总 barrier 时间应下降
- 正确性：功能完全等价（跳过 dirty store 不影响语义）

---

## P2-5: Cross-page compaction budget 128KB

### 问题
`CrossPageCompact` 的 evacuation budget 128KB 太小，大堆碎片化严重时需多次 BGC cycle。

### 方案
增加 budget 到 512KB 或 1MB，或改为按碎片页比例计算（如总碎片量 25%）。

### 文件修改

| 文件 | 改动 |
|------|------|
| `src/native/runtime-core/gc/gc_old_gen.cpp` | 修改 `CrossPageCompact` 中的 evacuation budget 常量，或改为动态计算 |

### 关键实现点
- 增大 budget 会增加 STW pause 时间（更多拷贝），需平衡
- 建议改为基于总堆大小的比例：`max(128KB, total_fragmented * 0.1)`
- 或暴露为可配置参数

### 验证
- fragmentation 重建基准测试，验证单次 BGC cycle 能回收更多碎片
- pause time 监控，确认无不可接受的增加

---

## P2-6: Interpreter 内存泄漏

### 问题
Interpreter `fast_dispatch.cpp` 中 `Handle_Box`、`Handle_NewObj`、`Handle_NewArr` 三个 call site 使用 `operator new`，不使用 CHAOS_IL2CPP_NEW_GC 宏，绕过 GC。

### 文件修改

| 文件 | 改动 |
|------|------|
| `src/native/interpreter/fast_dispatch.cpp` | 三个 call site 的 `new T{}` 替换为 `CHAOS_IL2CPP_NEW_GC(T, {})` 或 `GcAllocate()` |

### 验证
- Interpreter 路径执行 box/newobj/newarr 后，确认对象被 GC 跟踪
- stress test 包含 interpreter 场景验证无泄漏

---

## P2-7: Thread registry 不可回收

### 问题
`thread_state.cpp` 中线程注册到 `s_thread_list` 后从不移除（`is_running = false` 标记停用但不回收 entry）。线程数无限增长时泄漏 ManagedThread 对象。

### 文件修改

| 文件 | 改动 |
|------|------|
| `src/native/runtime-core/thread_state.h/cpp` | 新增 `UnregisterThread()` 函数，从链表移除 entry 并释放 ManagedThread |

### 关键实现点
- 移除时需 CAS 更新前驱的 `next` 指针（lock-free singly linked list）
- 考虑线程终止时是否安全：在 cooperative mode 下不可移除（自身在栈上）
- 建议在 preemptive mode 或 thread detach 路径调用

### 验证
- 循环创建/销毁 1000+ 线程后检查 ManagedThread 数量
- 确认 GC 扫描不受影响（已移除的线程不会被遍历）

---

## 执行顺序（从高到低）

```
P2-6 (Interpreter leak) ── 最容易修，影响最大，优先
P2-1 (Managed API) ─────── 独立
P2-4 (DC优化) ──────────── 独立
P2-3 (SlotMap注册) ─────── 独立
P2-7 (Thread回收) ──────── 需要谨慎处理并发
P2-2 (LOH compact) ─────── 工作量较大
P2-5 (Budget增大) ──────── 需 benchmark 验证
```

## 关键测试与验证命令

```bash
# 编译
cmake --build build/debug --target chaos_runtime_core

# 各组件测试
build/debug/tests/gc_loh_test.exe
build/debug/tests/gc_region_test.exe
build/debug/tests/gc_root_scanner_test.exe
build/debug/tests/gc_card_table_test.exe

# Stress test
build/debug/tests/gc_stress_test.exe --scenario all

# 线程创建/销毁测试
build/debug/tests/thread_test.exe
```

## 执行交接

计划已完成并保存到 `docs/dev/in-progress/gc-defect-fixes-p2/plan-v1-01.md`。
