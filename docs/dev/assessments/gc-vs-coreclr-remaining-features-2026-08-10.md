# 距离完全对齐 CoreCLR GC 还差多少功能 — 重要程度评估

> 日期：2026-08-10
> 对标基准：`D:\OpenSource\dotnet\runtime\src\coreclr\gc\`（本会话多轮权威源码还原）
> 现状：Phases 1-5（13 子任务）+ 补测后
> 方法：系统枚举 CoreCLR GC 完整能力域，逐项标 CRAG 现状 + 重要程度

---

## 一、重要程度分级标准

- **🔴 高（正确性/生产必需）**：缺失会导致正确性隐患、生产事故、或核心 GC 语义不完整
- **🟠 中（能力/性能）**：缺失影响缩放、延迟、或某类场景支持
- **🟡 低（工程/可观测）**：缺失影响调试、调优、或非核心能力

---

## 二、剩余功能清单（CoreCLR 有、CRAG 未完全对齐）

### A. 内存管理与分配

| # | CoreCLR 功能 | CRAG 现状 | 重要度 | 说明 |
|---|-------------|-----------|--------|------|
| A1 | **Server GC 多堆（per-NUMA heap）** | 🟡 只有 `GcHeapManager` scaffold，多堆协调 + 集成测试未验证 | 🔴 **高** | GC_SERVER 默认 ON，未验证=生产风险 |
| A2 | **Dynamic Heap Count**（`dynamic_heap_count.cpp` 运行期增减堆数） | 无 | 🟠 中 | 缩放能力 |
| A3 | **Region-based GC 完整（demotion/age-based evacuation）** | K1 只引入 size class，无 demotion/evacuation | 🟠 中 | 区域化核心未到 |
| A4 | **LOH 世代细分 / POH 完善** | LOH region-backed（K1b），POH 基础 | 🟡 低 | 有基础 |
| A5 | **半量预算 oom_budget 缩放（gen 预算）** | 只改上报 size，不缩放 gen 预算 | 🟡 低 | 诊断类 |

### B. 写屏障与卡表

| # | CoreCLR 功能 | CRAG 现状 | 重要度 | 说明 |
|---|-------------|-----------|--------|------|
| B1 | **世代写屏障完整管线 regen 验证** | 源码级已对齐（K2），**未 regen 进生成代码** | 🔴 **高** | 最大"实现vs验证"落差 |
| B2 | **世代写屏障压力验证（不漏跨代引用）** | K2b 单测有，无压力场景 | 🔴 **高** | 正确性 |
| B3 | **mark 扫脏卡 gen>condemned 过滤**（mark_phase.cpp:1388） | K2e 判为既有 fast-skip 覆盖，未实证 | 🟠 中 | 需确认不漏 |
| B4 | **card bundle 完整扫描循环**（find_card_dword 全样式） | K2d 做了粗卡跳段 | 🟡 低 | 有基础 |
| B5 | **软件写监视(SoftwareWriteWatch)** | 无（非目标） | 🟡 低 | AOT 专用，非目标 |

### C. 并发 GC

| # | CoreCLR 功能 | CRAG 现状 | 重要度 | 说明 |
|---|-------------|-----------|--------|------|
| C1 | **BGC 分相状态机（1st/2nd 快照 + concurrent plan/sweep）** | J1 已并发 mark+sweep，无分相 | 🟠 中 | 降延迟 |
| C2 | **provisional 完整形态（gen1 强制 compact + NGC2 排队）** | 只实现 force-blocking | 🟠 中 | 降延迟关键 |
| C3 | **GCProvModeStress 压测旋钮** | 无 | 🟡 低 | 压测工具 |
| C4 | **并发根扫描（BGC 期间根 slot 写屏障）** | gc_bgc 有根变更 buffer，未完全 | 🟡 低 | 有基础 |

### D. 分代与晋升

| # | CoreCLR 功能 | CRAG 现状 | 重要度 | 说明 |
|---|-------------|-----------|--------|------|
| D1 | **gen0/1/2 三代 + max_generation** | CRAG 2 代（young/old）+ Gen1 当 young | 🟠 中 | 代间提升语义 |
| D2 | **plan-gen（GC 后重绑 region 到新代）** | K3 只做了 Gen1→young 初始 | 🟠 中 | 区域化核心 |
| D3 | **survivor/打磨算法（CoreCLR data-to-age 调参）** | EMA 调度器，无 age-based | 🟡 低 | 性能 |

### E. 动态调优与配置

| # | CoreCLR 功能 | CRAG 现状 | 重要度 | 说明 |
|---|-------------|-----------|--------|------|
| E1 | **dynamic_tuning（free-list rate + 碎片密度 + memory load 伺服）** | EMA 时域，无空间域伺服 | 🟠 中 | Phase 6 |
| E2 | **72 个配置旋钮全量** | 只有 ~7 个 env 旋钮 | 🟡 低 | E1 加了首批 |
| E3 | **HardLimit 分代（SOH/LOH/POH 独立）** | 单硬限 | 🟡 低 | 边缘 |

### F. 句柄与弱引用

| # | CoreCLR 功能 | CRAG 现状 | 重要度 | 说明 |
|---|-------------|-----------|--------|------|
| F1 | **13 种 GCHandle 类型**（REFCOUNTED/WEAK_INTERIOR_POINTER/CROSSREFERENCE 等） | 只有 5 种 | 🟡 低 | 边缘类型 |
| F2 | **Handle table clump 代剪枝（COMPUTE_CLUMP_MASK）** | F1 做 per-handle bit | 🟡 低 | 有简化版 |

### G. 诊断与可观测性

| # | CoreCLR 功能 | CRAG 现状 | 重要度 | 说明 |
|---|-------------|-----------|--------|------|
| G1 | **>40 个 ETW/EventPipe 事件** | 只有 ~10 事件 | 🟡 低 | 可观测 |
| G2 | **per-GC 原因位图（condemn/compact/expand）** | H1 做了 GcTriggerReason 单值 | 🟡 低 | 有简化版 |
| G3 | **CDAC/SOS 调试器支持** | 无（非目标） | 🟡 低 | 非目标 |
| G4 | **Heap verifier（verify_regions）** | 无 | 🟡 低 | 调试 |

---

## 三、按重要程度汇总

### 🔴 高优先（正确性/生产必须，5 项）
1. **B1** K2c 世代写屏障完整管线 regen 验证（最大实现vs验证落差）
2. **B2** 世代写屏障压力验证（不漏跨代引用 → 潜在 UAF）
3. **A1** Server GC 多堆集成测试（GC_SERVER 默认 ON 未验证 → 生产风险）

### 🟠 中优先（能力/性能，8 项）
4. **C2** provisional 完整形态（gen1 compact + NGC2）
5. **C1** BGC 分相状态机
6. **E1** dynamic_tuning 伺服（Phase 6）
7. **A2** Dynamic Heap Count（Phase 6）
8. **A3** region demotion/age-evacuation
9. **D2** plan-gen 重绑
10. **D1** gen0/1/2 三代
11. **B3** mark 扫脏卡 gen 过滤实证

### 🟡 低优先（工程/可观测/边缘，13 项）
A4/A5, B4/B5, C3/C4, D3, E2/E3, F1/F2, G1/G2/G3/G4

---

## 四、量化：对齐度总览

| 能力域 | CoreCLR 功能数 | CRAG 已对 | 完成率 |
|--------|---------------|-----------|--------|
| 内存管理/分配 | 5 (A) | 2 | 40% |
| 写屏障/卡表 | 5 (B) | 3.5 | 70% |
| 并发 GC | 4 (C) | 2 | 50% |
| 分代/晋升 | 3 (D) | 1 | 33% |
| 动态调优/配置 | 3 (E) | 0.5 | 17% |
| 句柄/弱引用 | 2 (F) | 1.5 | 75% |
| 诊断/可观测 | 4 (G) | 1 | 25% |
| **总计** | **26** | **~11.5** | **~44%** |

> 注意：这是**功能完成率**（粗算）。若按"核心架构语义对齐度"，则高得多（分代/并发/写屏障/精确扫描这类核心已高度对齐）；上述主要是**边缘能力 + 验证深度**的差距。

---

## 五、结论（重要程度判断）

**如果只做 3 件事就能把"生产可依赖度"拉满**（高优先）：
1. **K2c regen 验证**（B1）→ 世代写屏障从源码级变生产级
2. **世代写屏障压力验证**（B2）→ 消除漏引用 UAF 风险
3. **Server GC 多堆集成测试**（A1）→ 消除默认开启未验证

**如果目标是"功能全等 CoreCLR"**（26 项全对齐），还需大量中/低优先项（dynamic tuning、dynamic heap、region demotion、三代、BGC 分相、provisional 完整），这些跨多会话。

**我的判断**：**CRAG GC 的核心架构语义已显著对齐 CoreCLR 的"骨架"（~核心 80%+），但"生产完整度"只有 ~44%**（缺验证深度 + 边缘能力 + 缩放调优）。高优先 3 项是"从学术级变生产级"的必做；中/低优先是"锦上添花到全等"。热更差异化（域卸载 O(region)）是 CoreCLR 不具备的 CRAG 优势，保留。

---

## 六、建议路线（按重要×成本）

| 批次 | 内容 | 重要 | 成本 |
|------|------|------|------|
| P0 | B1 regen + B2 压力 + A1 Server GC 测试 | 高 | 中 |
| P1 | C2 provisional 完整 + C1 BGC 分相 | 中 | 中 |
| P2 | E1 dynamic tuning + A2 dynamic heap（Phase 6）| 中 | 高 |
| P3 | 边缘（三代/D2 plan-gen/A3 demotion/F/G 完善）| 低-中 | 高 |
