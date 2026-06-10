---
name: dev-il2cpp-gc-expert
description: dev-il2cpp-gc-expert — il2cpp GC 专家
---

> ⚠️ **本文件通过 Skill 工具加载，作用是注入领域知识到当前对话上下文。**
> 本文件**不是可执行的 agent**。当前 Agent 需阅读下方"执行流程"作为实现参考。
>
> **当前 Agent 请做**：阅读知识域和约束 → 自行实现代码 → 自行验证
> **当前 Agent 不要做**：加载后等待"Expert 自动执行"——它不会，Skill 只加载文本。
>

# dev-il2cpp-gc-expert — il2cpp GC 专家

## 领域边界

### 我负责的
- CRAG GC（Chaos Region-Aware GC）架构理解和修改
- 分代策略（Young/Gen1/Old/LOH）调整
- 分配策略（Bump Arena → TLS GC Cache → Precise Generational）切换
- 写屏障（SATB + two-level card table）修改
- GC Safepoint（per-thread handshake）实现
- Page Decommission / Free List 内存管理
- Stress test 回归分析与修复
- GC Crash 根因定位（分配异常、写屏障损坏、Page 回收）

### 我不负责的（超出以下范围 → 标记 remaining，回 Dispatcher 重新分发）

- **运行时非 GC 分配**（MemoryDomain、Domain 分配）→ 超出范围，标记 remaining，原因：需要运行时域知识
- **翻译路径设计**（IL→C++ codegen）→ 超出范围，标记 remaining，原因：需要翻译域知识
- **纯性能优化**（profile 驱动的优化）→ 超出范围，标记 remaining，原因：需要优化域知识
- **对象模型/ObjectHeader** → 超出范围，标记 remaining，原因：需要运行时域知识
- **编译失败 / codegen stub**（LNK 错误、C++ 编译错）→ 超出范围，标记 remaining，原因：需要构建修复域知识

## 输出格式（Dispatcher 回读用）

每个 Expert 处理完任务后，必须在当前上下文中输出：

```
✅ done: [已处理的子任务 ID 列表]
⏳ remaining: [未处理的子任务 ID 列表 + 原因]
```

---

## 触发条件

- 从 `dev-il2cpp-core-agent` 路由到本 expert
- 用户直接输入 `/dev-il2cpp-gc-expert`
- 用户输入包含：GC、CRAG、分代、allocation、写屏障、safepoint、stress test、Page Decommission、NurseryAllocateSlow、GcAllocate、OOM

---

## 知识域

### 核心源文件

| 文件路径 | 说明 |
|---------|------|
| `src/native/runtime-core/gc/` | CRAG GC 核心实现目录（~15 个文件） |
| `runtime-core/gc_bridge.cpp` | GC 桥接层（runtime core ↔ GC 的协作） |
| `runtime-core/runtime_core.cpp` | GcAllocate/GcAllocateAtomic/ObjectNew/ArrayNew |
| `runtime-core/thread_state.cpp` | SafepointPoll、per-thread handshake |
| `runtime-core/gc_bump_cache.h` | GcAllocateImpl — bump pointer fast path |
| `common/chaos/profile.h` | RDTSC profile scope 预埋点 |

### 关键概念速查

| 概念 | 说明 | 关键参数 |
|------|------|---------|
| **Young GC (Nursery)** | 256KB per-thread region, bump-pointer, Cheney 复制 | 耗时 ~350µs |
| **Gen1 (Survivor)** | 16MB fixed region, bump-pointer promote | kPromotionAgeThreshold=2 |
| **Old (Tenured)** | 28 size-class × 64KB pages, 并行 mark-sweep | >30% fragmentation → compaction |
| **LOH** | >85KB, VirtualAlloc segment, mark-sweep | 25% frag → compact (AUTOMATIC) |
| **Write Barrier** | SATB (BGC) + two-level card table (Young) | ~6 native instructions |
| **BGC Concurrent Mark** | SATB write barrier, event-driven | IDLE→START→MARK→SWEEP→COMPACT |
| **Safepoint** | Per-thread suspend_seq/suspend_ack | fast path ~0.5ns |
| **Full GC** | Young + Old + LOH 并行 mark-sweep | Prallel mark ~90ms (100MB+) |
| **NurseryAllocateSlow** | 慢分配路径（TLS → GC trigger → gen promotion） | oversize 走 Phase 2 直分 |
| **Page Decommission** | 100%-free page → pool 回收（非 VirtualFree） | 见 `[[page-decommission-free-list-constraint]]` |

### 已知故障模式

| 模式 | 症状 | 解决方法 |
|------|------|---------|
| `[[page-decommission-free-list-constraint]]` | stress test AV | 100%-free page 必须 pool 回收，不可 VirtualFree |
| `[[nursery-allocateslow-oversized-recursion]]` | stack overflow | oversized 走 Phase 2 直分，不从 NurseryAllocate 重入 |
| `[[gc-full-gc-unwired]]` | FULL GC 未挂接 | GC.Collect() 不可调用 → CHAOS_IL2CPP_FAIL |
| `[[gc-low-mem-pointer-truncation]]` | x64 AV | HANDLE* stored in int (32-bit) → must use intptr_t |
| `[[gc-performance-optimizations]]` | 分配性能 | Allocate direct-pop after AllocatePage + TypeInfo cache |
| `[[three-generational-gc-analysis]]` | 分代策略 | 2-gen (Nursery + Old) with survivor, kPromotionAgeThreshold=2 |
| `[[gc-verification-table-index-fix]]` | segfault | kChaosExternalRuntimeFnTable index mismatch |

---

## 执行流程

### Step 0：架构语境加载（Architecture Pre-check）

1. **确认 GC 架构基线** — 阅读 [`24-CRAG-GC架构参考.md`](../../../wiki/03-功能模块/06-il2cpp核心架构/01-翻译管线/24-CRAG-GC架构参考.md)
2. **检查已知故障模式**（7 种已知 GC 模式，维护者：dev-il2cpp-debug-expert）
3. **如果涉及 Phase 3 并发** — 加载 [`08-GC子系统/CRAG-GC-Phase3-并发能力.md`](../../../wiki/03-功能模块/08-GC子系统/CRAG-GC-Phase3-并发能力.md)
4. **如果涉及 ETW 诊断** — 加载 [`08-GC子系统/CRAG-GC-ETW诊断事件.md`](../../../wiki/03-功能模块/08-GC子系统/CRAG-GC-ETW诊断事件.md)
5. **确认修改不违反架构约束**（精确扫描、写屏障预算、BGC 事件驱动）

### Step 1：定位与实现

1. **GC crash 定位流程**：
   - 先检查已知故障模式表
   - 运行 stress test 缩小范围：`chaos_gc_stress_test --scenario [A-N]`
   - 启用 `CHAOS_GC_STRESS=1`（每 16 次分配触发 GC）加速重现
   - 使用 trace 系统定位失败阶段

2. **分配策略修改**：
   - 通过 `GcAllocate`/`GcAllocateAtomic` 内部切换，不影响 codegen
   - Bump Arena → TLS GC Cache → Precise Generational 三阶段，入口代码无需修改

3. **写屏障修改**：
   - 必须保持 ~6 native instruction 性能预算
   - L1: 64K segment pointer table (512KB)
   - L2: 128 cards/segment × 512B = 64KB coverage
   - 总覆盖: 4GB heap, no mutex

4. **Page/Region 管理修改**：
   - 100%-free page → pool 回收（不可直接 VirtualFree，会导致 freelist dangling pointer）
   - 参考 `[[page-decommission-free-list-constraint]]`

### Step 3：验证

1. **单元测试**：`tests/` 下 GC 单元测试（19 个）
2. **Stress test**：5 个 stress test（17 个场景 A-Q），通过 CLI 运行
3. **性能基线**：对比 stress 前后的 Young GC 耗时基线（avg ~350µs）
4. **回归检查**：确认已知修复（9 个 critical fix）未被破坏

### Stress Test 场景选择器

| 症状 | 推荐场景 | 说明 |
|------|---------|------|
| 常规分配异常 | A (baseline concurrent) | 基线并发分配测试 |
| 大对象分配 | G (oversized objects) | 测试 LOH 路径 |
| Young GC 频繁 | C (aggressive young GC) | 加速 Young GC 触发 |
| Domain Unload | H (domain unload storm) | 批量 Domain 卸载 |
| BGC 并发标记 | L (BGC concurrent mark) | 后台并发标记测试 |
| 并行 mark 伸缩 | M (parallel mark scale) | 大数据量并行标记 |
| SATB barrier | N (SATB barrier stress) | 写屏障压力测试 |
| Page 回收 | E (extended GC pressure) | 长时间 GC 压力 |
| Pinned root | F (concurrent pinned root) | 固定根并发测试 |
| 混合场景 | B (mixed size) | 大小混合分配 |
| 写屏障 | D (baseline + write barrier) | 写屏障压力 |

---

## 架构约束

1. **精确扫描（Precise Scanning）**：
   - GcLayout hash table (4096 slots, open addressing, keyed by TypeInfo stable_id)
   - TypeInfo 地址验证：fixed array (64 slots) + release fence
   - 不降级为保守扫描（接受 ~10% pause increase）

2. **分代策略**：
   - 当前推荐：2-gen (Nursery + Old) with survivor space
   - 不要轻易加 full Gen1：`[[three-generational-gc-analysis]]`

3. **BGC (Background Concurrent Marking)**：
   - SATB (Snapshot-At-The-Beginning) write barrier
   - Event-driven (condition_variable::wait, no polling)
   - 状态机: IDLE → START → CONCURRENT_MARK → SWEEP → COMPACT

4. **Safepoint**：
   - 每个 ManagedThread 有 suspend_seq/suspend_ack/suspend_event
   - 请求: set per-thread seq → wait (~50µs spin then APC fallback)
   - 释放: clear seq → SetEvent

5. **Domain Integration**：
   - 每个 hot-update module 有独立 MemoryDomain
   - Domain unload: safepoint → scan cross-domain refs → release all domain regions

6. **NurseryAllocateSlow 禁止递归**：
   - oversized (size > kMaxTlabAlloc) 必须走 Phase 2 直接分配
   - 不可从 NurseryAllocate 重入，否则 stack overflow

---

## 核心数据（Debug, 100 threads）

| 指标 | 数值 |
|------|------|
| Young GC avg | ~350µs (250-600µs) |
| Full GC (mark only) | ~20µs (5-85µs) |
| Full GC (parallel mark, 100MB+) | ~90ms (50-150ms) |
| Full GC (parallel compact) | ~80ms |
| LOH Sweep | ~1ms |
| LOH Compact | ~5ms |
| Safepoint fast path | ~0.5ns |
| Young region size | 256KB |
| Gen1 region size | 16MB |
| Tenured page size | 64KB |

---

## 输出约束

1. **分配策略修改必须通过入口内部切换** — codegen 无需修改
2. **Page Decommission 必须用 pool 回收** — 禁止 VirtualFree 100%-free pages
3. **写屏障修改必须保持性能预算** — ~6 native instructions
4. **分代策略修改必须验证 kPromotionAgeThreshold 影响**
5. **BGC 状态机修改必须保持事件驱动模型** — 不要加轮询
6. **commit message 要求** — 修复完成后必须包含三段式根因：
   - `root_cause` — 一句话根因
   - `fix_strategy` — 修复策略
   - `regression_check` — 验证范围

## 执行前 Checklist

```
□ 是否匹配已知故障模式表？（优先参考已知修复）
□ 确认 GC 架构基线：24-CRAG-GC架构参考.md + 17-统一内存分配体系.md
□ 如果涉及 Phase 3 并发 → 加载 CRAG-GC-Phase3-并发能力.md
□ 如果是 crash → 启用 CHAOS_GC_STRESS=1 加速重现
□ 选择正确的 stress test 场景（见场景选择器）
```

## 执行后 Checklist

```
□ 单元测试（19 个）通过
□ stress test 对应场景通过
□ Young GC 基线未退化（avg ~350µs）
□ 已知修复未被破坏（9 个 critical fix）
□ 如果涉及 NurseryAllocateSlow → 检查 oversized 递归防护
□ 如果涉及 Page → 使用 pool 回收，非 VirtualFree
```

---

## 集成点

| 上游 | 本 skill | 下游 |
|------|----------|------|
| `dev-il2cpp-core-agent` → GC 路由 | **dev-il2cpp-gc-expert** | `dev-pressure-test-management`（stress test） |
| 用户直接输入 | | `dev-systematic-debugging`（crash 定位回退） |
| | | `dev-optimization-campaign`（性能优化） |
| | | `dev-verification-before-completion`（验证门） |
