# GC 完成情况横向对比 CoreCLR — 核心能力缺口综合分析报告

> 日期：2026-08-12
> 对标基准：`D:\OpenSource\dotnet\runtime` 的 `src\coreclr\gc\`（**main @ 14b601ec**，即 CoreCLR _V8_ 当代 GC，2026-08-09 已重构为模块化：`allocation/background/card_table/collect/finalization/mark_phase/plan_phase/relocate_compact/sweep/memory/dynamic_heap_count/dynamic_tuning` 等）
> 本项目：`D:\agent\chaos-il2cpp` 的 `src/native/runtime-core/gc/`（CRAG，64 个源文件）+ plan-v5 里程碑现状
> 方法：双 Explore 源码逐 bucket 盘点 + 结合 2026-08-10 既有评估 + plan-v5 STATUS + git log（近 50 条 GC 提交）
> 声明：本报告为**只读综合分析**，不含任何代码改动。

---

## TL;DR（一页结论）

CRAG GC 的**核心架构语义已高度对齐 CoreCLR 骨架**（分代 / 并发 mark-sweep / 世代写屏障 / 精确扫描 / 压缩 / SATB-BGC），并且自 2026-08-10 评估以来**显著进步**（三代链 M9、condemned 过滤 M10、Server 堆接线 M3A、BGC 两快照+死锁修复 M5、配置旋钮 7→24、handle 类型 M12 均已登 main）。

**当前剩余的核心能力缺口**（按影响排序）：

| 优先级 | 缺口 | 性质 |
|--------|------|------|
| 🔴 P1 | **Server GC 多堆默认 OFF + 零 CI 验证** | 生产风险 |
| 🔴 P1 | **Page 回收缺口**（100%-free 页永不释放）+ `FindPage O(N)` | 正确性/长期内存 |
| 🔴 P1 | **young-GC Phase-1 晋升回写深因（A2b 旧代引用跟进）** | 已知 open 正确性 bug |
| 🟠 P2 | **Verify 深度**（已有轻量自检，缺 CoreCLR 分级 HeapVerify+poison） | 韧性 |
| 🟠 P2 | **动态调优伺服闭环** + 配置旋钮 24 vs ~75 | 性能/工程 |
| 🟡 P3 | 工程/可观测：BCL GCMemoryInfo、BGC ETW payload、handle iterator 收敛 | 边缘 |

**量化对齐度**：按 CoreCLR 能力域逐项粗算，**核心骨架 ~80%+，功能完整度 ~55-60%**（较 8-10 的 ~44% 提升）。CRAG 保有其差异化优势（MemoryDomain 域卸载 O(region)）是 CoreCLR 不具备的。

---

## 一、CRAG 已实现能力（代码级证据，逐 bucket）

依据 Explore 实际读到的源码 `src/native/runtime-core/gc/*.h/cpp`。以下均 **[已实现]** 或 **[部分]**，非臆测：

### 1. 分代策略 — 4 代 [已实现]
| 代 | 机制 | 证据 |
|----|------|------|
| Nursery (gen0) | 256KB/线程 region, bump TLAB, Cheney 复制 | `gc_region.cpp` `NurseryAllocateSlow`(L135), `gc_young_collector.cpp` |
| Gen1 (survivor) | 独立代 tag, GcGen1Collection, 动态 resize | `gc_region.h` `kRegionGenShift=22`(L100), `gc_gen1.h/cpp` |
| Old (Tenured) | 28 size-class × 64KB 页, mark-sweep+compact | `gc_old_gen.h/cpp` |
| LOH | 85KB 阈值, VirtualAlloc segment, mark-sweep+compact | `gc_loh.h` `kLohThreshold=85*1024`(L36) |

附带：**demotion（old→gen1，frag>40% 触发，max 1MB/cycle）**、**condemned-gen 过滤**均已实现。

### 2. GC 模式 [已实现 / 部分]
- Young / Full STW / **BGC**（`IDLE→CONCURRENT_MARK→STW_REMARK→CONCURRENT_SWEEP` SATB 状态机）
- **BGC−Young 协调暂停**（`PauseForYoungGc/ResumeAfterYoungGc`，M5-1 修死锁）
- Blocking（FORCED/AGGRESSIVE）、LatencyMode（含 NoGCRegion）
- **Server GC 多堆：[部分]** 实现存在（`gc_heap_manager.cpp`/`gc_coordinator.cpp`/`G_OldGen()` 访问器），但**默认 OFF**（`CHAOS_IL2CPP_GC_SERVER=0`）

### 3. 写屏障体系 [已实现]
- 两层卡表（L1 segment 指针 idx/128 → L2 card idx%128）+ **card bundle**（2MB/bit）
- **世代感知 `_dst_ref`**（`chaos_gc_dirty_card_dst_ref`，GetRegionGen 判 young→young 不设卡）
- **codegen/解释器/JIT 三层已接线**（`EmitInstruction.cs`、`LinearEmission.cs`、`fast_dispatch.cpp`、`jit_helpers.cpp`）
- **SATB 预写屏障**（TLS buffer + global queue + freeze 协议）+ **根 slot 写屏障**（`gc_root_change.*`）

### 4. 分配路径 [已实现]
`GcAllocate/GcAllocateAtomic` → TLS bump（`gc_bump_cache.h`）→ `NurseryAllocateSlow`（LOH 截断/调度/Gen1 独立收集/FULL→collect）+ oversized + LOH（>85KB）+ **POH（pinned region）** + per-domain GC 分配追踪。

### 5. 精确根扫描 [已实现]
`GcLayoutRegistry`（4096 槽开放寻址，TypeInfo 地址验证）+ 保守兜底（`GcScanConservativeFrame`）+ **寄存器根 spill**（每 RecordGcPoint 前）+ pinning（async-pinned handle + compaction skip list）。

### 6. 压缩 [已实现]
Page 内 compact（frag>0.30 触发）+ **cross-page compact**（≤1MB）+ 动态 evacuation budget（512KB~4MB）+ 并行 relocate/compact + **LOH compact**（AUTOMATIC 默认）+ demotion。

### 7. 并发能力 [已实现]
Concurrent mark（BGC）+ **Parallel mark（≤8 worker, work-stealing, 事件驱动）** + Concurrent sweep（per-page lock）+ SATB freeze 收敛 + 根 change ring buffer + thread hijacking（x64；ARM64 待适配）。

### 8. 其他核心 [已实现 / 部分]
Strong/Weak/Pinned/Dependent GCHandle（+M12 REFCOUNTED/WEAK_INTERIOR）；WeakReference nulling；DependentHandle/Ephemeron（定点迭代+re-mark）；finalizer 线程；POH；MemoryDomain 域卸载；静态根注册 `GcRegisterStaticRootRange`；NUMA（`gc_numa_win/linux`）；GC Stress；ETW provider（Windows-only，8 事件）。

---

## 二、CoreCLR _V8_ 能力全集（作为 baseline）

CoreCLR `main`（14b601ec）当代 GC 已重构为模块化 monolith，能力全集（逐项代码证据见另一半报告）：

- **GC 模式**：WKS/SVR（宏 `FEATURE_SVR_GC`）+ Concurrent/Background GC（默认即开）+ Foreground blocking + **Dynamic Heap Count**（`DYNAMIC_HEAP_COUNT`，运行期增减堆数，.NET 9/10 主打新能力，含 TCP/gen0-growth 自适应调参）
- **分代**：SOH gen0/1/2 + **LOH(代3)** + **POH(代4)**；`total_generation_count=5`；`ALLOW_REFERENCES_IN_POH`；Frozen/RO segment
- **压缩**：sweep 为主 + partial/full compact（`gc_heap_compact_reason` 全量原因：low_ephemeral/high_frag/loh_forced/induced/aggressive 等）+ segment 复用（`expand_reuse_normal/bestfit/new_seg`）+ GCRetainVM
- **区域化**：`USE_REGIONS`（64 位主机）+ `regions_segments.cpp` 动态 region→segment 切换 + SIP special regions（当代已进化为 segment+region 混合，非旧"15 代 fixed"）
- **写屏障/卡表**：card table + card bundle + `check_need_card(child,gen)` 世代判断 + **Software Write Watch**（`WRITE_WATCH`）+ 屏障 flavor 可配置（REGION_BIT/REGION_BYTE/SERVER）+ WRITE_BARRIER_CHECK shadow
- **分配**：per-thread alloc_context（gen0/gen1 bump）+ LOH/POH 各自 segment + `GC_ALLOC_*` flags（FINALIZE/CONTAINS_REF/ALIGN8/ZEROING_OPTIONAL/LARGE/PINNED）
- **精确扫描/终结**：precise（gcdesc slot map）+ conservative（非托管帧）+ **13 种 HNDTYPE**（含 REFCOUNTED/DEPENDENT/ASYNCPINNED/SIZEDREF/WEAK_NATIVE_COM/WEAK_INTERIOR/CROSSREFERENCE/R2R）+ ephemeron/resurrection
- **BGC servo 调优**：`BGC_SERVO_TUNING` + FL(反馈线性)/ML(机器学习) 双伺服配置族 + entry/exit memory_load 处理
- **诊断**：`gcevents.h` 全事件表（GCStart_V2/GCEnd/GCHeapStats/PerHeapHistory/私有 BGC 阶段事件 BGCBegin/1st/2nd/DrainMark/Revisit/Overflow/AllocWait + FullGCNotify + 动态 SizeAdaptation）+ cgroup 感知 + GCDH/DAC 调试器支持 + 分段 trace
- **其他**：no_gc region 完整（含 finalizer 回调阈值）、GC latency modes、demotion/elevation、`GC_HISTORY`、**vxsort SIMD 排序**（AVX2/AVX512/NEON）、GCStress、HeapVerify 分级、GCNumaAware/LargePages/RetainVM/ConserveMem/HardLimit(SOH/LOH/POH独立)、JIT-Marshal GCBridge、standalone GC 构建

---

## 三、横向对比 — 核心能力缺口分析

以下按"**要完全对齐 CoreCLR 还差什么**"组织。已实现且对齐的用 ✅ 标注，缺口标 🔴/🟠/🟡。

### A. 内存管理与分配

| # | CoreCLR | CRAG 现状 | 缺口？ | 重要度 |
|---|---------|-----------|--------|--------|
| A1 | Server GC 多堆（per-NUMA heap） | 实现存在默认 OFF，**零 CI 验证** | 🔴 需 CI | **高** |
| A2 | **Dynamic Heap Count**（运行期增减堆数） | 无 | 🟠 缺 | 中 |
| A3 | Region-based 完整（15 代/region-demotion/age-evacuation） | region + demotion **已做**；age-evacuation 部分 | 🟠 部分 | 中 |
| A4 | LOH/POH 完善 | 已做 | ✅ | — |
| A5 | 半量 oom_budget gen 缩放 | M15 已做 | ✅ | — |
| A6 | **Page decommission（100%-free 页回收）+ FindPage 索引** | 100%-free 页**从不释放**，page_count 线性增长，`FindPage O(N)` | 🔴 缺 | **高（长期内存/性能）** |

### B. 写屏障与卡表

| # | CoreCLR | CRAG 现状 | 缺口？ | 重要度 |
|---|---------|-----------|--------|--------|
| B1 | 世代 `_dst_ref` 卡表完整管线 | codegen/JIT/解释器三层已接线 | 🔴 缺 | 需**压力验证** |
| B2 | 世代写屏障压力验证（不漏跨代引用） | K2b 单测有，**无压力场景** | 🟠 缺 | **中-高（UAF 风险）** |
| B3 | mark 扫脏卡 gen>condemned 过滤 | **M10 已落地** | ✅ | — |
| B4 | card bundle 完整扫描循环 | 已做（粗卡跳段） | ✅ | — |
| B5 | **Software Write Watch**（软件写监视） | 无 | 🟡 缺 | 低（AOT 专用，非目标） |

### C. 并发 GC

| # | CoreCLR | CRAG 现状 | 缺口？ | 重要度 |
|---|---------|-----------|--------|--------|
| C1 | BGC 分相状态机（1st/2nd 快照 + concurrent plan/sweep） | **M5-A 已落地**（clear-as-scan 两快照） | ✅ | — |
| C2 | provisional 完整形态（gen1 强制 compact + NGC2 排队） | force-blocking 有，NGC2 排队待 M3B | 🟠 部分 | 中 |
| C3 | GCProvModeStress 压测旋钮 | 无 | 🟡 缺 | 低 |
| C4 | 并发根扫描（BGC 期间根 slot 写屏障） | `gc_root_change` 已做 | ✅ | — |

### D. 分代与晋升

| # | CoreCLR | CRAG 现状 | 缺口？ | 重要度 |
|---|---------|-----------|--------|--------|
| D1 | gen0/1/2 三代 + max_generation | **M9 三代已做** | ✅ | — |
| D2 | plan-gen（GC 后重绑 region 到新代） | **M8 已做** | ✅ | — |
| D3 | survivor/打磨算法（data-to-age） | EMA 调度器，无 age-based | 🟡 缺 | 低 |

### E. 动态调优与配置

| # | CoreCLR | CRAG 现状 | 缺口？ | 重要度 |
|---|---------|-----------|--------|--------|
| E1 | **dynamic_tuning 伺服**（free-list rate/碎片密度/内存负载闭环） | M11 只 24 旋钮，**空间域伺服未闭环** | 🟠 缺 | 中 |
| E2 | **~75 配置旋钮** | ~24 | 🟠 缺 | 中 |
| E3 | HardLimit 分代（SOH/LOH/POH 独立） | 单硬限 | 🟡 缺 | 低 |

### F. 句柄与弱引用

| # | CoreCLR | CRAG 现状 | 缺口？ | 重要度 |
|---|---------|-----------|--------|--------|
| F1 | **13 种 GCHandle**（REFCOUNTED/WEAK_NAATIVE_COM/SIZEDREF/CROSSREFERENCE...） | ~5 种（+M12 REFCOUNTED/WEAK_INTERIOR） | 🟡 缺 | 低 |
| F2 | HandleTable clump 代剪枝 | 分类型 iterators 被收敛移除 | 🟡 回退 | 低 |

### G. 诊断与可观测性

| # | CoreCLR | CRAG 现状 | 缺口？ | 重要度 |
|---|---------|-----------|--------|--------|
| G1 | **ETW/EventPipe >40 事件**（含 BGC 阶段族/FullGCNotify/动态） | ~8 事件，**缺 BGC 阶段事件族** | 🟡 缺 | 低 |
| G2 | per-GC 原因位图（condemn/compact/expand） | H1 做了 GcTriggerReason 单值 | 🟡 部分 | 低 |
| G3 | CDAC/SOS 调试器支持 | 无（非目标） | 🟡 非目标 | — |
| G4 | **HeapVerify 分级 + poison** | T2 有轻量 GcVerifyHeap；无 CoreCLR 分级 | 🟠 缺 | 中 |

### H. 其他（性能/工程）

| # | CoreCLR | CRAG 现状 | 缺口？ | 重要度 |
|---|---------|-----------|--------|--------|
| H1 | **vxsort SIMD 排序**（压缩时对象排序硬件加速） | 无 | 🟡 缺 | 低（性能） |
| H2 | no_gc region 完整（finalizer 回调阈值） | NoGCRegion 有 | 🟡 部分 | 低 |
| H3 | BGC FL/ML 双伺服调优 | 无 | 🟠 缺 | 中 |
| H4 | segment 复用/RetainVM/ConserveMem | page pool 有简化 | 🟡 部分 | 低 |
| H5 | **增量/并发三色 GC（白灰黑 backlog）** | **无** — 只用 SATB snapshot + STW re-mark | 🟢 非缺口 | 见下 |
| H6 | frozen/RO segment（read-only heap） | 无 | 🟡 缺 | 低（NativeAOT 域） |

> 🔵 **H5 澄清**：CoreCLR _V8_ **本身也是 SATB-based**（`BGC_MARKED_BY_FGC`/卡表 mark-bit scheme），并没有独立的白灰黑三色"增量 GC"主路径——它同样是后台并发 mark + STW re-mark。因此 CRAG 的 `GcEndStubbornChange` 占位（`gc_helpers.h:71` "incremental in later milestones"）**不是相对 CoreCLR 的缺口**，只是本地为"未来增量 GC"预留的钩子。两者并发模型架构一致。

---

## 四、能力域对齐度量化（2026-08-12，较 8-10 更新）

| 能力域 | CoreCLR 项 | 已对齐 | 完成率 | 变化 |
|--------|-----------|--------|--------|------|
| 内存/分配 | 6 (A) | 3 | 50% | +M15 |
| 写屏障/卡表 | 5 (B) | 3 | 60% | +M10 condemned |
| 并发 GC | 4 (C) | 3 | 75% | +M5-A 两快照 |
| 分代/晋升 | 3 (D) | 3 | **100%** | +M9/M8 |
| 动态调优/配置 | 3 (E) | 0.5 | 17% | +M11 旋钮 |
| 句柄/弱引用 | 2 (F) | 1 | 50% | +M12 |
| 诊断/可观测 | 4 (G) | 1 | 25% | +M13 ETW 测试 |
| **总计（核心 7 域）** | **~27** | **~14.5** | **~55%** | ↑（原 ~44%） |

> 说明：这是"核心 7 域功能完整度"粗算。**若按核心架构语义对齐度**（分代/并发/写屏障/精确扫描/压缩骨架），CRAG 已大幅领先此百分比——**核心骨架 ~80%+**；缺口主要在**验证深度 + Server 工程 + 缩放调优 + 边缘能力**。

---

## 五、核心能力缺口总结（按影响优先级）

### 🔴 P1 — 生产必须（正确性 / 生产风险）
1. **B2 世代写屏障压力验证**：世代 `_dst_ref` 卡表已接线但缺压力场景 → 潜在"漏跨代引用 UAF"。**这是最大"实现 vs 验证"落差。**
2. **A1 Server GC 集成测试**：`GC_SERVER` 默认 OFF 且 CI 未跑 → 默认开启即生产风险。M3A-2/3 需独立构建矩阵。
3. **A6 Page 回收 + FindPage O(N)**：100%-free 页永不释放 → 长期内存线性增长 + 慢 mark。已知架构瓶颈。
4. **A2b young-GC 晋升回写深因（旧代引用跟进）**：跨 GC+codegen 域深手术，是残余 stress known-failing 根因（详见 `gc-crossgen-fix-implementation-status`）。

### 🟠 P2 — 能力 / 性能
5. **G4 Verify 深度**：已有轻量自检，缺 CoreCLR 分级 HeapVerify + poison → 防簿记漂移。
6. **E1 dynamic_tuning 伺服闭环** / **H3 BGC 双伺服**：只有 24 旋钮，空间域调优未闭环。
7. **E2 配置旋钮 24 vs ~75**。
8. **A2 Dynamic Heap Count**（运行期堆数伸缩，依赖 Server 工程）。
9. **C2 provisional 完整形态**（依赖 M3B）。
10. **A3 age-evacuation 完整**（M7-B 未落地）。

### 🟡 P3 — 工程 / 可观测 / 边缘
11. **G1 ETW 事件扩容**（缺 BGC 阶段族/FullGCNotify/动态）。
12. **F1 GCHandle 13 型全量**（缺 WEAK_NATIVE_COM/SIZEDREF/CROSSREFERENCE 等边缘型）。
13. **BCL 侧 GCMemoryInfo 托管类型**、**F2 handle iterator 收敛回退**、**H1 vxsort**、**BGC Gen1 ETW payload 占位**。

---

## 六、风险提示（本报告可信度边界）

1. **部分 BGC 数据为占位**：`gc_bgc.cpp:321` Gen1 ETW 事件 payload 硬编码 0。
2. **文档与实际源码不同步**：架构文档引用的 `gc_handle.h/cpp` 独立文件不存在，GCHandle 已并 `engine_lifecycle.cpp`；世代感知 handle iterators 实际已被收敛移除。
3. **AGGRESSIVE→compact 强绑定未实证**：compaction 实际由 `DecideCompactMode` 的 frag 阈值驱动，非 collection-mode 直接驱动。
4. **CoreCLR 路径差异**：本地树为模块化 `src/coreclr/gc/`（vs 旧 `src/coreclr/src/gc/`）；宏名如 `gc_impl.h`→`gcimpl.h`、`gcconfig.h` 取代 `clrconfigvalues.h`。
5. **量化 % 是粗算**：同一 feature 的"对齐深度"（单测级 vs 压力级 vs CI 级）无法仅靠清单精确表达，故骨架 ~80% 与完整度 ~55% 并存是刻意的双视角。

---

## 七、建议路线（按重要 × 成本）

| 批次 | 内容 | 重要 | 成本 | 关联里程碑 |
|------|------|------|------|-----------|
| **P0** | A6 Page 回收 + FindPage 索引（长期内存）；B2 写屏障压力验证 | 高 | 中 | — |
| **P1** | M3A-2/3 Server CI 矩阵 + A2b 晋升深因（正确性收敛） | 高 | 高 | M2/M3A |
| **P2** | E1 dynamic_tuning + H3 BGC 伺服 + E2 旋钮扩容 | 中 | 中 | M3B/M6 |
| **P3** | G4 Verify 分级 + G1 ETW 扩容 + 边缘 handle/vxsort | 中 | 中 | M4/M5 |
| **P4** | 边缘（F/G/H 剩余） | 低 | 高 | backlog |

---

本文档存放于 `docs/dev/assessments/gc-vs-coreclr-review-2026-08-12.md`，为 **2026-08-10 评估（`gc-vs-coreclr-remaining-features-2026-08-10.md`）的续篇/刷新版**，融合 plan-v5 已登 main 的成果。既有差异化结论依然成立：**CRAG 核心架构语义对齐度已显著高于其"生产完整度"；热更/域卸载 O(region) 是 CoreCLR 不具备的 CRAG 差异化优势，保留。**
