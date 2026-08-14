# 解释器 Profiler 功能 — 整体设计深度分析

> 日期：2026-08-14 ｜ 状态：设计分析（非实现）
> 覆盖：D1 ns/op 锚 + register_vm_profiler + FastExecute opcode histogram 的完整 picture 与完善方案
> 前置：已落地的 D1 代码（`b4d1ec85b` VmProfileScope 接入 + VM_PROFILER 挂 PROFILE tier）

---

## 一、这个功能要解决什么问题（触发）

问题不是"缺一个 profiler"，而是 **无法量化解释器各 tier 的成本**，导致性能优化是"盲优化"。

### 1.1 表象：解释器性能锚缺失
- `register_vm_profiler` 的 `CHAOS_IL2CPP_VM_PROFILER_ENABLED` 默认 **0**
- `DumpFastExecuteOpcodeHistogram` 有实现但只在 PROFILE build + entry Scriban shutdown 调用
- 结果：**没有任何机制产出"每 opcode 多少次、每方法多少周期/GC 字节"的量化基线** —— `tiering_benchmark` 是唯一的 ns/op 目标，且它不 dump per-method 锚

### 1.2 深层：三个具体痛点（从已落地的 4 维分析 + 111-gap 实证）
1. **正确性 vs 性能不可分割**：`a1_external_fallback_gap_report` 实测 67 预编译 entry 扫出 **111 个外部 runtime fallback 缺口**（无 C++ body + 无 stub → `ChaosExternalRuntimeFallback` return-0）。修复/判定这些缺口都必须先知道"哪些方法在真实路径上是热点"—— 没有 opcode/方法级 profile 就无法给热点排序。
2. **优化无法归因**：C2（扁平存器文件）、C3（QuickJIT 阈值）、省写穿等优化，若无"优化前 vs 优化后 ns/op + per-opcode 分布"基线，无法判定收益真假。这正是 handoff 反复强调的"D1 是所有 P2 性能优化的量化前置"。
3. **分层归因缺失**：同 IL 可跑 T1(FastExecute)、T2/T3(RegisterVM)、T4(JIT/native)。哪个 tier 在真实 workload 占 dominant？没有 per-tier 的 cycle 归因就无法判断"该优化 FastExecute 还是 RegisterVM"。

### 1.3 用户视角的"转向"含义
本轮要"转向 profile"，即把焦点从**正确性修复**（review 修的 P1/P2）转向**性能量化基建** —— 让后续优化有尺子。

---

## 二、完整 picture（现状全貌）

### 2.1 已存在的三块，各功能完备度不同

| 组件 | 位置 | 已做 | 缺口 |
|------|------|------|------|
| **FastExecute opcode histogram** | `fast_dispatch_execute.inc:136-164` | `g_fast_op_freq[128]` thread_local 计数 + `DumpFastExecuteOpcodeHistogram()` | ✅ `tiering_benchmark.cpp:1944` 已调 dump（in-tree 可用）；❌ 只统计 FastExecute(T1)，RegisterVM(T2/T3) 无直方图；❌ thread_local 跨线程需聚合 |
| **RegisterVM per-method 锚** | `register_vm_profiler.h/.cpp` | `VmProfileScope`(RDTSC) 已接入 Step-B + OSR promotion + method-replacement 三处（`b4d1ec85b`） | ❌ **无 in-tree 调用 `DumpProfilerToFile`** —— 只在 Scriban(TestProject.RuntimeEntry.cpp.scriban:1316)，codegen 不在原生树 → profile build 也 dump 不出；❌ `FindOrCreateSlot` 线性探测 hash(4096 slot) 每次方法入口 RDTSC+原子 CAS → **注入测量开销污染被测值** |
| **ns/op 锚** | `tiering_benchmark.cpp` | 循环 iter 调 `InterpreterEntryDirect` 计时 | ❌ 只有整体 ns/op，无 per-opcode/per-tier/per-method 归因 |

### 2.2 关键设计约束（当前代码硬约束）
1. **RDTSC 仅 x86**：`register_vm_profiler.h:159` `#error` if 非 x86/ARM64 with VM_PROFILER_ENABLED → ARM64/Android 无 per-method 锚。ARM64 只能用 `CHAOS_IL2CPP_PROFILE_SCOPE`（更粗）。
2. **`VmProfileScope` 体全编译出（默认 0）**：PROFILE tier 以下热路径零开销（设计正确），但意味着"要测就得整仓 PROFILE 构建"。
3. **`FindOrCreateSlot` 是 O(探测) 原子操作**：`__rdtsc` 到 `find-or-create` 到 `compare_exchange_weak` 每次方法进入都做 → 高频短生命周期方法下测量本身主导。
4. **`g_vm_profiler` 是全局原子**：多线程并发累积，跨线程归因模糊。
5. **两套 profiler 不互通**：opcode histogram（FastExecute，thread_local）+ per-method（RegisterVM，全局 RDTSC）无法关联"哪个方法贡献了哪个 opcode"。

### 2.3 当前"可验证产出"的断点链
```
VmProfileScope 埋点 ✓ → g_vm_profiler 累积 ✓ → DumpProfilerToFile ✓
                                                        ↑
                                    缺 in-tree 触发点（只在 Scriban）
→ 原生树任何测试/benchmark 都触发不了 per-method dump
→ D1 锚"code 已落但输出不可生产"
```

---

## 三、完善这个功能的几个方案

按风险/完整度/成本递进。**主推荐 = 方案 A + 方案 B（组合，先最小可产出再加深）**。

### 方案 A：补 in-tree dump 触发点 —— 最小可产出（0.5d，低风险）
**目标**：让 `tiering_benchmark` 在 PROFILE build 下同时 dump opcode 直方图 + per-method 锚，解锁首个量化基线。
**改动**：`tiering_benchmark.cpp` `main` 里（现有 line 1944 直方图后）加
```cpp
#if CHAOS_IL2CPP_VM_PROFILER_ENABLED
    extern void chaos::il2cpp::interpreter::DumpProfilerToFile(const char*) noexcept;
    extern void chaos::il2cpp::interpreter::ResetProfiler() noexcept;
    DumpProfilerToFile(nullptr);  // per-method cycles → stderr
    ResetProfiler();
#endif
```
镜像 Scriban 的 hook。配 `--preset profile`（`artifacts/presets/profile`，`CHAOS_IL2CPP_CONFIG_TIER=profile`）构建跑出首个 ns/op + per-method 基线。
**价值**：立刻得到可量化基线，C2/C3 有尺子。**风险**：低（只加 dump 调用）。
**局限**：仍 RDTSC-only、仍无 per-opcode×per-method 关联。

### 方案 B：降低测量注入开销 —— 保住测量可信度（1-1.5d，中风险）
**目标**：让 `VmProfileScope` 测出的值不被测量本身污染（当前 FindOrCreateSlot 线性探测 + RDTSC 偏重）。
**子项**：
- B1：slot 查找改 **thread_local cache**（方法 token → slot 的一级缓存，命中免线性探测/原子 CAS）—— 高频方法 hit 后 0 开销。
- B2：RDTSC 替换 —— 采样（每 N 次方法调用记一次周期）而非每次都记，降注入。
- B3：区分 `gc_alloc_bytes` 通道当前恒传 0（`RecordCall(..., 0)`）—— 接上真 GC 分配差分，让 per-method 锚同时给周期+分配。
- B4：直方图跨线程聚合（thread_local → 汇总线程），修 `tiering_benchmark` 单线程假设。
**价值**：测量可信（不被自身主导），且拿到 GC 分配维度。**风险**：中（改热路径，需回归证明非 PROFILE build 零开销不变）。
**递进**：A 之后做 B，A 的基线成为 B 的对照组。

### 方案 C：跨层归因 + per-opcode×per-method 关联（2-3d，高风险）
**目标**：回答"哪个 tier 在真实 workload 主导"—— 不只是整体 ns/op，而是 FastExecute vs RegisterVM 各贡献多少。
**子项**：
- C1：给 RegisterVM 也加 **opcode 频率直方图**（对齐 `g_fast_op_freq`），让 T1/T2/T3 的 opcode 分布可比。
- C2：per-method 锚与 opcode 直方图**share 方法 token**，导出"方法→opcode 占比"二维。
- C3：`tiering_benchmark` 扩展 scenarios 覆盖每 tier（现在混测）。
**价值**：真正的热点归因，指导"C2 该优化哪种 opcode"。**风险**：高（触 register dispatch 表/数据模型，动 hot path）。
**递进**：A+B 站稳后才到 C。

### 方案 D：跨平台/可移植 profiler（远期，2-3d+）
**目标**：解除 RDTSC-only 约束（ARM64/Android/iOS）。
**子项**：换 `std::chrono::steady_clock`/`QueryPerformanceCounter`/`clock_gettime(CLOCK_MONOTONIC)` 抽象（`clock.hpp`），或采样 profiler；`#error` 改优雅降级。
**价值**：移动端也能量化。**风险**：低（抽象层），但低优先（当前主战场 x64 Windows）。

---

## 四、建议路线（主推荐）

**阶段 1（本案最小可产出 = 方案 A）**：补 in-tree dump + profile build 跑出首个基线 → 解锁 C2/C3 量化。这是把"D1 代码已落地但输出不可生产"补成"可生产"的关键一步。

**阶段 2（可信度 = 方案 B1+B3）**：thread_local slot 缓存消除测量注入 + 接真 GC 分配维度 —— 保证后续优化判断不受测量伪影干扰。

**阶段 3（归因 = 方案 C1+B4）**：RegisterVM opcode 直方图 + 跨线程聚合 —— 回答"该优化哪层哪个 opcode"。

**阶段 4（跨平台 = 方案 D）**：解 RDTSC-only 约束（等待移到 ARM64 平台时）。

> 不建议现在做 C（前置注册表/数据结构改动，需 A+B 基线压舱）；D 低优先（平台未移动端）。

---

## 五、风险与验收口径

- **验收锚**：`--preset profile` 构建后 `tiering_benchmark` 输出：① FastExecute opcode 直方图（非空）② per-method RDTSC cycles（非空）③ 若做 B3，含 GC 分配字节 → 三者落到一文件，可比对前后。
- **非 PROFILE 零开销不变**：所有改动都必须保持 `CHAOS_IL2CPP_VM_PROFILER_ENABLED=0` 下 body 全编译出（现有设计正确，勿破坏）。
- **测量可信度**：B 完成后，某热方法测得的 mean cycles 不应因"开 profiler"显著劣化（对照 A 基线）。

---

## 六、关联
- memory：`interpreter-session-2026-08-13-round2`（D1 code）、`interpreter-c4-remainder-2026-08-14`、`perf-line-benchmark-credibility-and-dispatch`
- handoff：`session-handoff-2026-08-13.md` §二.D1
- 代码：`register_vm_profiler.h/.cpp`、`fast_dispatch_execute.inc:136-164`、`tiering_benchmark.cpp:1944`
