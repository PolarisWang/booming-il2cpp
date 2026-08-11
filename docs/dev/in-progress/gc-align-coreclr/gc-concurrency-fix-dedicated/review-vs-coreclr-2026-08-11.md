# Review — 本仓库 GC vs CoreCLR GC（2026-08-11）

> 域：GC(6)。本 review 基于权威 CoreCLR 源码（`D:\OpenSource\dotnet\runtime\src\coreclr\gc\`）4 域调研，
> 对照本仓库 `src/native/runtime-core/gc/` 当前状态（含本会话层1 `GcMarkRangeOld` + 未解层2/stress）。
> 目的：列出 CoreCLR 有、本仓库缺/未对齐，且值得做的内容，按正确性>性能>热更分级。

## 一、正确性（🔴 高优先）

### C1. 年轻 GC 跨代引用正确性（层2 未解 stress）— 本会话核心缺口 ⬅️ 最急
- **本仓库**：`gc_region_barrier_stress_test` known-failing（~212/284 dangling）。已精确定位：
  dangling ref 是"晋升目标页未进 `page_array_` / 未 `GcMarkRangeOld` 标 OLD"（`region_gen=0, IsOoG=0,
  FindPage=null`，且高于 nursery used-end）。晋升走 `G_OldGen().Allocate` 但产物页未被簿记。
- **CoreCLR 对照**（aa5e4796238e3d567）：正确性靠 4 个机制，本仓库都缺/弱：
  1. **精确 field 地址设卡**（`shr dst,0x0B` 卡的是写的确切字段地址）——本仓库 codegen 传对象 base，
     多卡对象丢非 base cell。
  2. **扫描先清卡再 mark**（`clear_cards` 在扫卡前）→ 竞态重设卡被下次 GC 兜住。
  3. **只清 condemned 代卡、保留旧代卡**（`mark_phase.cpp:3733`）。
  4. **共享卡字原子 `lock or`**（Bit region barrier）→ 并发写不丢 bit。
  5. **真 STW 挂起**保证 store+barrier 期间无 mutator；本仓库 SPB ack-and-continue 缺真挂起。
- **要做的**：定位并修 `G_OldGen().Allocate` 晋升页簿记缺口（page_array_/GcMarkRangeOld 覆盖）；
  评估 CoreCLR 的 clear-before-scan / 精确 field 卡 / 只清 condemned with 本仓库 young-GC 对齐。

### C2. 卡表/写屏障完整性
- **本仓库**：双层卡表 + card bundle + 世代屏障(layer1)。缺 CoreCLR 的：
  - `gen > condemned` 过滤（`mark_phase.cpp:1393`）——卡扫描只作用于 condemned 及更老代，缩小扫描范围。
  - 卡 bundle 的重建靠硬件 write-watch（`update_card_table_bundle`），本仓库无。
  - 共享卡字原子 `lock or`（多线程写同 512B cell 时不丢位）。
- **要做的**：`gen>condemned` 过滤（正确性+性能）；评估并发写卡原子性。

### C3. 自检/验证（verify_heap）— 缺失
- **CoreCLR**：`verify_heap`（GC 入口/出口校验，填 0xaa poison、校验 plugin/mark bits/card）、
  `verify_region_to_generation_map`、`HeapVerify` 配置分级。**本仓库全缺。**
- **要做的**：加轻量 `verify_heap` + `verify_region_to_generation_map`（本仓库已实现精确扫描/卡表，
  出独立自查能防层2这类 簿记 漂移回归）。**高层推荐：这是本次 review 中最值得做的工程健壮性项。**

## 二、性能/能力（🟠 中优先，P1 性能最优）

### P1. BGC 分相状态机 — 简化版
- **CoreCLR**：`bgc_state` 分相（`mark_handles/mark_stack/revisit_soh/uoh/overflow/final_marking/sweep_soh/uoh/plan`）
  两快照（并发第一遍 + STW 第二遍 `final_marking`）+ `current_c_gc_state` 允许 foreground mini-GC 与 BGC 交错。
- **本仓库**：`gc_bgc.h` 简化 IDLE/concurrent mark/sweep/compact。缺两快照/分相/交错。
- **要做的**：若降延迟是目标，补两快照最终标记 + foreground 交错协调。

### P2. dynamic_tuning 伺服（free-list/碎片/内存负载）
- **CoreCLR**：`desired_new_allocation`（surv_ratio→growth、碎片过高降 gen2、gen0 空列表率）、
  `bgc_tuning` FL 伺服、decommit 策略、hard-limit 90% 触发。
- **本仓库**：`gc_scheduler` 有 surv_ratio + old_gen_fragmentation 记录，但**无闭环伺服**（只记录不喂预算）。
- **要做的**：把 frag/surv 闭环到 gen 预算缩放（对齐 `desired_new_allocation`）。

### P3. 硬件 write-watch / SoftwareWriteWatch
- **CoreCLR**：卡 bundle 重建 + 写监视标记。**本仓库无。**
- 低优先（AOT 场景才关键），可列为 backlog。

### P4. 配置旋钮面（~75 vs ~7）
- **CoreCLR**：`GCConfig` ~75 旋钮（ServerGC/ConcurrentGC/HeapLimit/Gen0Size/Latency/NumaAware/FL tuning 等）。
- **本仓库**：~7 env 旋钮。对齐核心几枚即可（heap 限制/并发/latency），不必全 75。

## 三、工程/诊断（🟡 低-中，架构完美 P2）

### E1. HandleTable 子系统 — 缺失（集中文件）
- **CoreCLR**：`handletablecore.cpp`（IGCHandle* 抽象，强/pinning/weak/dependent + aging + per-thread 表）。
- **本仓库**：句柄分散在 gc_bgc/gc_old_gen/reflection（`GcAllocHandle` 等），无独立 HandleTable 子系统文件。
- **要做的**：若热更/域卸载依赖，聚合为独立 subsystem + 明确 strong/weak 分类。

### E2. 终止队列分代 + 复活语义
- **CoreCLR**：`CFinalize` 分代段 + resurrect（BIT_SBLK_FINALIZER_RUN）+ 交给终结线程。
- **本仓库**：gc_old_gen 有 finalize 队列基础。核对分代/复活/交给线程是否齐。

### E3. 挂起抽象层（GCToEEInterface/GCToOSInterface）
- **CoreCLR**：GC 算法与 挂起/OS 抽象解耦（`SuspendEE`/`RestartEE` + `GCToOSInterface`）→ 硬 STW 由 VM 实现。
- **本仓库**：SAM 在 thread_state + pal，封装度低。热更/多平台可考虑抽象边界。

### E4. GC 完备性评审（已有文档）
- 本会话早期 `gc-vs-coreclr-remaining-features-2026-08-10.md` 已列 26 项 vs ~44% 对齐；本 review 聚焦
  本次 4 域新调研 + 层1/层2 增量。

## 四、优先级建议（要做什么，排优先级）

| 序 | 项 | 域 | 理由 | 工作量 |
|----|----|----|------|--------|
| 1 | **修层2 晋升页簿记缺口**（page_array/GcMarkRangeOld 覆盖） | 正确性 | 唯一 open correct bug，stress 仍 known-failing | 中 |
| 2 | **加 verify_heap + verify_region_to_generation_map** | 工程健壮 | 防簿记漂移回归，承接层1验证缺口 | 中 |
| 3 | **gen>condemned 卡过滤** | 正确性+性能 | 对齐 CoreCLR mark 语义，缩小扫描 | 低中 |
| 4 | **BGC 两快照/分相** | 性能(P1) | 降延迟目标才做 | 高 |
| 5 | **dynamic_tuning 伺服闭环** | 性能(P1) | frag/surv→gen 预算 | 中 |
| 6 | **HandleTable 子系统聚合** | 架构(P2) | 热更/域卸载 | 中 |
| 7 | software write-watch / 配置面 | 能力 | 低优，backlog | 低 |

> 注：layers 1 (GcMarkRangeOld) 已落地无回归；其余为本 review 建议的继续方向。
