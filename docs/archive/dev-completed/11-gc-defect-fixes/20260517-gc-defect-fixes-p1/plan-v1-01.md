# P1 GC 性能缺陷修复计划

> **面向执行 Agent：** 必须使用 dev:subagent-driven-development（如果可用子 Agent）或 dev:executing-plans 来执行本计划。步骤使用复选框（`- [ ]`）语法跟踪。

**目标：** 修复 GC 子系统 4 项 P1（严重性能）缺陷：BGC parallel workers 无 work-stealing、保守栈扫描误根、Parallel compact 单线程、BGC 10ms 忙等

**实现状态：**
| P1 # | 缺陷 | 状态 | 说明 |
|------|------|------|------|
| P1-1 | BGC 无 work-stealing | ✅ 已实现 | `gc_bgc.cpp` BgcWorkerMain 有 per-worker deque + random steal |
| P1-2 | 保守栈扫描误根 | ✅ 已实现 | `GcScanConservativeFrame` + `GcScanAllThreadRoots` 增加 g_heap_base 范围过滤，消除非堆指针误根 |
| P1-3 | Parallel compact 单线程 | ✅ 已实现 | `ParallelCompactPages()` 使用 GcWorkerPool 并行化；CrossPageCompact Phase 4/5 也并行化 |
| P1-4 | BGC 10ms 忙等 | ✅ 已实现 | 三个 `wait_for(100ms)` 改为纯 `wait()`，BGC 线程事件驱动阻塞；所有信号路径（FlushSatbBuffer/Start/Stop/StwCompact/ForceComplete）调用 NotifyBgc() |

**架构：** 这些修复集中在 BGC 并行标记机制升级和 GC 线程同步机制优化两个领域

**技术栈：** C++17, atomic operations, thread synchronization (condition_variable)

**架构审核模式：** critical（命中 runtime-core GC 主线）

**结构告警重点：**
- gc_bgc.cpp 的并行标记逻辑需要与 gc_parallel_mark.h 的 work-stealing 框架统一
- BGC 线程同步从 sleep_for 改为 condition_variable 要注意虚假唤醒处理
- thread_state.cpp 的栈扫描逻辑需要增加精确根优先的逻辑

**设计文档：** `docs/archive/discuss/20260516-memory-gc-comprehensive-evaluation.md`

**问题清零来源：** direct user confirmation

**计划来源：** direct-plan

**预期知识沉淀：** wiki/03-功能模块/06-il2cpp核心架构/01-翻译管线/24-CRAG-GC架构参考.md

**收尾约束：** 执行完成后必须进入"结构告警与架构审视 → 测试通过 → 归档 completed → 合并&提交"固定链路。

---

## P1-1: BGC parallel workers 无 work-stealing

### 问题
BGC 并行标记阶段，所有 worker 共享一个 mutex-protected mark stack。当某 worker 遇到大对象，其他 worker 须等待 mutex。实测 100MB 堆 parallel mark ~130ms（8 workers），无 work-stealing 意味着随堆增长接近线性而非亚线性。

### 方案
为每个 BGC worker 添加 work-stealing deque。Worker 优先处理本地 deque（lock-free push/pop），本地空时从其他 worker 的 deque 底部 steal（带 mutex）。

### 文件修改

| 文件 | 改动 |
|------|------|
| `src/native/runtime-core/gc/gc_bgc.h` | `bgc_parallel_workers_` 改为 per-worker 结构（含本地 deque、steal 计数器） |
| `src/native/runtime-core/gc/gc_bgc.cpp` | `BgcWorkerMain` 改为优先 pop 本地 deque，空则随机 steal；`SpawnParallelMarkWorkers` 创建 per-worker deque |
| `src/native/runtime-core/gc/gc_parallel_mark.h` | 可复用的 `WorkStealingDeque<T>` 模板类（push_bottom/pop_bottom/pop_top） |

### WorkStealingDeque 设计
```cpp
template<typename T>
class WorkStealingDeque {
    static constexpr int kInitialCapacity = 64;
    std::atomic<T*>* entries_;       // ring buffer
    std::atomic<int> bottom_{0};     // private: push/pop at bottom-1
    std::atomic<int> top_{0};        // shared: steal from top
    std::mutex grow_mutex_;          // only on resize

    void PushBottom(T item);           // lock-free, CAS on bottom
    T PopBottom();                     // lock-free, CAS on bottom
    T PopTop();                        // mutex-protected (steal)
};
```

### 关键实现点
- Batch pop 策略保留：`PopBottom` 最多返回 `kBgcPopBatchSize`（32）个条目
- Steal 目标选择：XorShift32 随机选 worker，避免所有 worker 抢同一个
- deque 扩容：ring buffer 满时需 mutex 保护 + realloc（低频操作，不影响 hot path）
- 终止检测：所有 worker 本地 deque 空 + 全局 mark stack 空 + SATB 空

### 验证
- 对比 baseline：100MB 堆 BGC parallel mark 时间应减少 30%+
- `gc_stress_test.cpp`: 验证无数据竞争

---

## P1-2: 保守栈扫描误根

### 问题
Full GC 时 `GcScanAllThreadRoots` 对所有线程栈做保守扫描（每 pointer-aligned word 视为潜在根），可能误认非指针为根，导致对象无法回收，增加内存驻留。

### 方案
当前已经是 hybrid 模式（精确 GcSlotMap + 保守后备），但 full GC 的保守扫描覆盖率太高。优化方案：将精确根扫描覆盖到更多 managed 栈帧，减少保守扫描范围。

### 文件修改

| 文件 | 改动 |
|------|------|
| `src/native/runtime-core/gc/gc_root_scanner.cpp` | `GcScanConservativeFrame` 增加启发式过滤：排除明显非指针（超出堆范围的值） |
| `src/native/runtime-core/thread_state.cpp` | `GcScanAllThreadRoots` 运行时增加 managed 帧链识别，优先精确路径 |
| `src/native/runtime-core/gc/gc_old_gen.cpp` | `Collect()` Phase 1 根标记中，对保守扫描结果增加堆范围验证 |

### 关键实现点
- 启发式过滤：`value < g_heap_base || value > g_heap_end` 的值一定不是指针，跳过
- 非对齐值过滤：指针需 8-byte aligned，非对齐值跳过
- 精确帧优先：识别 `managed_stack` 帧链中的 managed 方法帧，用 GcSlotMap 精确扫描
- 保守帧降级：对无法精确的帧启用上述启发式过滤，而非全量标记

### 验证
- 对比相同堆的保守 vs 过滤后扫描：确认误根减少，存活对象回收增加
- stress test 验证无正确性回退

---

## P1-3: Parallel compact 单线程

### 问题
`Collect()` Phase 4b 的 compaction（`RelocatePage`、`CompactPage`、`CrossPageCompact` 的 GlobalRelocate）全部在单线程下执行。实测 100MB 堆 compaction ~180ms。

### 方案
将 relocation 和 compact 两阶段并行化。Relocation（指针更新）按 page 分派到 worker pool；compact（memmove + free-list rebuild）也按 page 并行。

### 文件修改

| 文件 | 改动 |
|------|------|
| `src/native/runtime-core/gc/gc_old_gen.cpp` | `RelocatePage` 改为通过 `GcWorkerPool` 并行调度；`CompactPage` 同 |

### 关键实现点
- RelocatePage 并行：每个 worker 处理一部分 `CompactPlan.entries`，互不重叠
- CompactPage 并行：每个 worker compact 不同的 page，无需同步
- CrossPageCompact 的 GlobalRelocate：需要等所有 relocation 完成后再开始 copy，加 barrier
- `RelocatePage` 内对全局 old-gen page 的遍历需要保护（或通过 page index 分段）

### 风险
- BgcCompact（BGC 路径下的 compaction）当前在 safepoint 内执行，单线程已够用
- 此优化主要针对 full GC 的 STW compaction

### 验证
- 对比 baseline：100MB 堆 compaction 时间应减少 50%+

---

## P1-4: BGC 10ms 忙等

### 问题
BGC thread 在等待工作时（CONCURRENT_MARK 等待栈非空、COMPACT_NEEDED 等待 mutator 处理），使用 `std::this_thread::sleep_for(10ms)` 轮询。10ms 休眠在低负载场景产生不必要延迟，在高负载场景则可能延迟标记完成。

### 方案
改为 condition_variable 事件驱动模式。BGC thread 休眠直至显式信号唤醒。

### 文件修改

| 文件 | 改动 |
|------|------|
| `src/native/runtime-core/gc/gc_bgc.h` | 新增 `std::condition_variable bgc_cv_`、`std::mutex bgc_cv_mutex_` |
| `src/native/runtime-core/gc/gc_bgc.cpp` | `BgcThreadMain` 中所有 `sleep_for` 改为 `wait_for`(有限超时，兜底防漏信号)；mutator 在 push mark stack / 设 COMPACT_NEEDED / 设 FINISHED 时 notify |

### 关键实现点
- 分三处信号：mutator flush SATB 时 notify（新数据）、mutator 在 StwCompact 完成后 notify（FINISHED→IDLE）、worker push 回 mark stack 时 notify
- 使用 `wait_for(100ms)` + predicate 防止漏信号导致永久休眠
- 保持 cycle_complete_ 等原有机制作为后备

### 验证
- BGC smoke test 验证无 hang
- stress test 观察 BGC 延迟改善

---

## 执行顺序

```
P1-4 (condition_variable) ── 独立，无依赖
P1-1 (work-stealing) ──────── 独立，但建议先做 P1-4
P1-2 (保守扫描) ──────────── 独立
P1-3 (parallel compact) ──── 独立
```

## 关键测试与验证命令

```bash
# 编译
cmake --build build/debug --target chaos_runtime_core

# BGC smoke test
build/debug/tests/bgc_smoke_test.exe

# Stress test
build/debug/tests/gc_stress_test.exe --scenario all

# Profile timing (对比 baseline)
build/debug/tests/gc_stress_test.exe --profile --output baseline.json
```

## 执行交接

计划已完成并保存到 `docs/dev/in-progress/gc-defect-fixes-p1/plan-v1-01.md`。
