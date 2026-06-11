# CRAG-GC-Phase3 — 高级并发能力

> 对应路线图 Phase 3，包含 Server GC、Thread Hijacking、并发根扫描、BGC-YoungGC 协调暂停、NUMA 感知空闲页回收。
> 编写日期: 2026-05-22
> 最后更新: 2026-06-11

## 1. Server GC 模式 (G-4, gc-p4-01)

### 设计决策

Young generation (nursery + Gen1 survivor) 在 Server GC 模式下**保持共享**，不按 heap 分割。只有 OldGen、LOH、Scheduler 是 per-NUMA-node 的。这一决策避免了 per-heap root scanning 和 card table 分区的复杂性，同时仍能实现并行 old-gen GC。

### 架构

```
WKS 模式 (CHAOS_IL2CPP_GC_SERVER=0):
  G_YoungGen() → g_young_gen (全局)
  G_OldGen()   → g_old_gen   (全局)
  G_Loh()      → g_loh       (全局)
  G_Scheduler()→ g_gc_scheduler (全局)

Server 模式 (CHAOS_IL2CPP_GC_SERVER=1):
  G_YoungGen() → g_young_gen           (共享)
  G_OldGen()   → tls_current_heap->old_gen (per-heap)
  G_Loh()      → tls_current_heap->loh     (per-heap)
  G_Scheduler()→ tls_current_heap->scheduler (per-heap)
  G_Gen1State()→ g_gen1_state (共享)
```

### 核心组件

| 组件 | 职责 |
|------|------|
| `GcHeapManager` | 管理 per-NUMA-node heap 数组，提供线程→heap 映射 |
| `GcCoordinator` | 多 heap GC 编排：获取全局 safepoint → Phase 1 共享 young GC → Phase 2 并行 old-gen GC |
| `GcHeapContext` | 每个 heap 的上下文包（old gen, LOH, scheduler） |
| `GcWorkerPool` | 用于并行执行 per-heap GC 的 worker 线程池 |

### 执行流程 (ExecuteMultiHeapGc)

```
1. 获取全局 safepoint
2. Phase 1: 共享 young GC + Gen1 collection（在共享 garden 上执行一次）
3. Phase 2: per-heap old-gen full GC（GcWorkerPool 并行执行）
4. 恢复线程 heap 绑定
5. 释放 safepoint
```

### 线程绑定

- `RegisterThread()` → `SetThreadHeap()` 根据 NUMA 节点绑定线程
- `UnregisterThread()` → `ClearThreadHeap()` 清除绑定
- `HeapForCurrentThread()` → 查询当前 NUMA 节点

## 2. 并发根扫描 (G-25, gc-p4-02)

### 问题

BGC 并发标记期间，mutator 线程可能覆盖根 slot（静态字段、GCHandle 目标等）。如果某个根 slot 持有某对象的唯一引用，覆盖后该对象可能被过早回收。

### 解决方案

Per-thread ring buffer 跟踪根 slot 修改，在 STW re-mark 阶段重新标记。

### 组件

| 组件 | 职责 |
|------|------|
| `ManagedThread::root_change_buffer_[64]` | 64-entry ring buffer |
| `ManagedThread::root_change_head_/tail_` | 生产者/消费者位置 |
| `BgcRecordRootChange(slot, old_value)` | 根 slot 写屏障中调用 |
| `BgcDrainRootChangeBuffer(mt, mark_fn)` | STW re-mark 阶段 drain |

### 数据流

```
Mutator: 覆盖 root slot
  → BgcRecordRootChange(slot, old_value)
  → 写入 ring buffer (CAS head)

STW Re-Mark:
  → EnumerateThreads → BgcDrainRootChangeBuffer
  → 对每个 entry: old_value 在 old-gen 且未标记 → BgcTryMark + PushToBgcMarkDeque
```

### 写入屏障插入点

根 slot 写屏障（静态字段赋值、GCHandle 目标修改）需要额外调用 `BgcRecordRootChange`。当前实现提供基础设施，具体屏障插入由后续集成完成。

## 3. NUMA 感知空闲页回收 (G-23, gc-p4-03)

### 问题

旧 gen 空闲页池（`page_pool_`）不感知 NUMA 节点。AllocatePage 从池中弹出页时，可能从远程 NUMA 节点分配，增加跨节点访问延迟。

### 解决方案

池条目（`PoolEntry`）增加 `numa_node` 字段，AllocatePage 优先匹配当前 NUMA 节点。

### 改动

| 文件 | 改动 |
|------|------|
| `gc_old_gen.h` | `PoolEntry` 增加 `int8_t numa_node` |
| `gc_old_gen.cpp` | `AllocatePage` 池搜索优先匹配当前 NUMA 节点 |
| `gc_old_gen.cpp` | 回收页时保留 `numa_node` |
| `gc_old_gen.cpp` | BgcSweep 裁剪池时按节点公平保留 |

### 限流

`kMaxPoolSize = 16` 限制池大小。当超出时，优先释放远程 NUMA 节点的空闲页。

## 4. Thread Hijacking — SIGUSR2 Trampoline

### 问题

当 cooperative-mode 线程在 safepoint poll 中未及时响应（软超时 ~100ms），GC 协调器通过 SIGUSR2 信号中断该线程。SIGUSR2 handler 使用 SA_SIGINFO 获取 ucontext，但 handler 运行在有限的信号栈（sigaltstack）上，无法在信号栈上自旋等待。

### 解决方案

CoreCLR 风格的 lightweight return-address hijacking：信号 handler 修改 ucontext 中的 RIP/RSP，让线程恢复执行时跳转到 assembly trampoline，在**线程的正常栈**上调用 `SafepointPoll()`。

### 组件

| 组件 | 职责 |
|------|------|
| `gc_suspend_trampoline_x64.S` | x64 assembly trampoline：保存 9 个 scratch 寄存器 → 调用 `chaos_safepoint_poll` → 恢复寄存器 → RET 到原始 RIP |
| `gc_suspend_trampoline.h` | extern "C" 声明 |
| `PreemptiveSuspendHandler()` | 读取 ucontext，设置 suspend_ack，修改 REG_RIP/REG_RSP 指向 trampoline |

### 数据流

```
GC 协调器请求 safepoint
  → 等待 ~100ms，未响应的 cooperative 线程收到 SIGUSR2
  → SA_SIGINFO handler 获取 ucontext
  → 设置 suspend_ack（避免竞争条件）
  → 将原始 RIP 写入 [original_rsp - 8]
  → 修改 uc_mcontext.gregs[REG_RIP] = &gc_suspend_trampoline_x64
  → 修改 uc_mcontext.gregs[REG_RSP] = original_rsp - 8
  → 信号 handler 返回，内核恢复修改后的 ucontext
  → 线程在 trampoline 入口恢复执行（正常栈）
  → trampoline 调用 SafepointPoll()
  → 线程在 safepoint 自旋等待
  → GC 完成 → ReleaseGlobalSafepoint
  → SafepointPoll 返回 → trampoline RET 到原始 RIP
```

### 关键设计

- 信号栈（sigaltstack）仅用于 handler 执行，不用于自旋等待
- ucontext 修改在信号 handler 中完成，handler 返回后自动生效
- 9 个 scratch 寄存器（RAX, RCX, RDX, RSI, RDI, R8-R11）在 trampoline 中保存/恢复

## 5. 根 Slot 写屏障 — AOT Codegen 接线

### 问题

`BgcRecordRootChange()` 基础设施（per-thread ring buffer + STW re-mark drain）已在 Phase 3 前期完成，但 AOT codegen 的静态字段存储路径（`stsfld`）未接线。当 BGC 并发标记期间 mutator 覆盖根 slot 时，旧值不会被记录。

### 解决方案

在 `NativeAotLoweringPlanner.ExceptionEmission.cs` 的 `stsfld` handler 中，在静态字段赋值前插入 SATB 预写屏障和根变更记录。

### 类型过滤

仅对可持有 GC 引用的非原始类型字段插入屏障。对 `int` / `float` / `bool` 等跳过。

### 生成的 C++ 代码

```cpp
{
    auto chaos_value = <pop>;
    auto& chaos_slot = chaos_static_<field_id>;
    BgcSatbPreWriteBarrier(reinterpret_cast<void**>(&chaos_slot));
    BgcRecordRootChange(reinterpret_cast<void**>(&chaos_slot), chaos_slot);
    chaos_slot = chaos_value;
}
```

### 验证

解释器、JIT helper、AOT codegen 三条路径现在都一致地调用 `BgcRecordRootChange`。

## 6. BGC-YoungGC 协调暂停协议

### 问题 (G-3)

当 BGC 并发标记正在运行，同时 Young GC 触发 nursery evacuation（Cheney BFS）时，BGC 的并发对象扫描可能读到被 forwarding pointer 覆盖的旧对象头，导致 segfault 或存活对象丢失。

### 解决方案

Atomic flag 握手协议：`bgc_pause_requested_` / `bgc_paused_`。

| 字段 | 用途 |
|------|------|
| `bgc_pause_requested_` | Young GC 设置此标志请求 BGC 暂停 |
| `bgc_paused_` | BGC 确认暂停后设置此标志 |

### 协议流程

```
Young GC Phase 1（evacuation 前）：
  → 检查 g_bgc_is_marking
  → 若 BGC 在标记 → PauseForYoungGc()
    1. bgc_pause_requested_ = true
    2. spin-wait: bgc_paused_ == true
    3. drain BGC 工作 deques 中的 nursery 指针

BGC 主循环（每次迭代）：
  → 检查 bgc_pause_requested_
  → 若暂停请求：
    1. StopParallelMarkWorkers()
    2. bgc_paused_ = true
    3. spin-wait: bgc_pause_requested_ == false

Young GC Phase 3d（evacuation 后）：
  → 扫描 bfs_worklist，将 promoted 到 old-gen 的
    对象重新入队到 BGC mark deque 0

Young GC Phase 5（恢复）：
  → ResumeAfterYoungGc()
    1. bgc_pause_requested_ = false
    2. spin-wait: bgc_paused_ == false
```

### 安全性

- SATB 不变式：暂停期间 mutator 仍可运行，SATB 预写屏障捕获所有引用覆盖
- Phase 3d re-rooting：promoted 对象的 old-gen 引用被推入 BGC work deque，BGC 恢复后正确处理
- Nursery drain：暂停时 BGC 工作 deques 中的 nursery 条目被移除（安全网，正常情况 BGC 只处理 old-gen）

## 7. 文件改动清单

### gc-p4-01 (Server GC 模式)

| 文件 | 改动 |
|------|------|
| `CMakeLists.txt` | 添加 Server GC 源文件 + `CHAOS_IL2CPP_GC_SERVER` option |
| `gc_features.h` | 添加 CHAOS_IL2CPP_GC_SERVER 文档 |
| `gc_heap.h` | 访问器函数 + 共享 young gen + Server GC 模式 per-heap 路由 |
| `gc_heap_manager.h/cpp` | GcHeapManager 实现 + per-heap old_gen.Init() |
| `gc_coordinator.h/cpp` | multi-heap GC 编排 |
| `gc_region.cpp` | chaos_gc_collect 路由到 GcCoordinator |
| `thread_state.cpp` | RegisterThread/UnregisterThread 加入 heap 绑定 |

### gc-p4-02 (并发根扫描)

| 文件 | 改动 |
|------|------|
| `thread_state.h` | ManagedThread 添加 root change ring buffer |
| `gc_root_change.h` | BgcRecordRootChange + BgcDrainRootChangeBuffer |
| `gc_bgc.h` | PushToBgcMarkDeque 公开方法 |
| `gc_bgc.cpp` | StwRemark 集成 root change buffer drain |

### gc-p4-03 (NUMA 感知空闲页)

| 文件 | 改动 |
|------|------|
| `gc_old_gen.h` | PoolEntry 增加 numa_node |
| `gc_old_gen.cpp` | AllocatePage NUMA 优先 + 回收保留 numa_node |

### Accessor 迁移

~17 个文件中的 200+ 处 `g_young_gen`/`g_old_gen`/`g_gc_scheduler`/`g_loh` 替换为 `G_YoungGen()`/`G_OldGen()`/`G_Scheduler()`/`G_Loh()`，实现 WKS/Server 双模式切换。

### gc-p4-04 (Thread Hijacking — SIGUSR2 Trampoline)

| 文件 | 改动 |
|------|------|
| `gc/gc_suspend_trampoline_x64.S` | x64 assembly trampoline |
| `gc/gc_suspend_trampoline.h` | extern "C" 声明 |
| `thread_state.cpp` | PreemptiveSuspendHandler ucontext RIP/RSP 修改 + chaos_safepoint_poll C-linkage 包装 |
| `CMakeLists.txt` | 添加 ASM 语言支持 + .S 文件到 Linux 平台源 |

### gc-p4-05 (AOT Codegen 根 Slot 写屏障)

| 文件 | 改动 |
|------|------|
| `NativeAotLoweringPlanner.ExceptionEmission.cs` | stsfld handler 添加 BgcSatbPreWriteBarrier + BgcRecordRootChange |

### gc-p4-06 (BGC-YoungGC 协调暂停)

| 文件 | 改动 |
|------|------|
| `gc_bgc.h` | bgc_pause_requested_ / bgc_paused_ atomic 字段；PauseForYoungGc / ResumeAfterYoungGc 公开方法 |
| `gc_bgc.cpp` | BGC 主循环 pause check；StopParallelMarkWorkers + 原子握手 |
| `gc_young_collector.cpp` | Young GC Phase 1 前暂停 BGC；Phase 3d re-rooting；Phase 5 恢复 |

### gc-p4-07 (CI 矩阵)

| 文件 | 改动 |
|------|------|
| `.github/workflows/ci-framework.yml` | 添加 gc_server input，cmake 阶段传递 `-DCHAOS_IL2CPP_GC_SERVER=ON/OFF` |

## 8. 构建与测试

| 构建 | 结果 |
|------|------|
| Linux GCC cmake chaos_runtime_core (Debug) | ✅ PASS |
| Managed codegen dotnet build | ✅ 0 errors |
| ASM trampoline (gc_suspend_trampoline_x64.S) | ✅ 编译通过 |
| GC 单元测试 (gc_safepoint/gc_bgc_smoke/gc_api) | ✅ 通过 |
| BGC-YoungGC race test | ✅ 已实现 |
| BGC root scan test | ✅ 已实现 |

## 9. 已知限制

- 多 NUMA 节点环境需要额外的 OOM 测试
- Thread hijacking trampoline 仅为 x64 架构实现；ARM64 需后续适配
- Server GC CI matrix 通过 `ci-framework.yml` 的 `gc_server` input 启用
