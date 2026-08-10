# STATUS — GC 代码级对齐 CoreCLR

## 元信息

```yaml
task_id: gc-align-coreclr
task_type: roadmap
phase: roadmap
roadmap_or_plan: docs/dev/in-progress/gc-align-coreclr/roadmap-v1-01.md
created: 2026-08-10
created_by: main-agent
source: 用户要求（比对 dotnet 源码后产出代码级对齐 CoreCLR 的开发计划）
blocking_questions: []
question_clearance: cleared
clearance_confirmed_by_user: true
```

## 用户确认门（2026-08-10）

用户三项拍板已确认：

| 决策 | 用户选择 |
|------|---------|
| 对齐深度 | P0 + P1 + 区域化重构（对齐 CoreCLR region-based 内存模型） |
| 纪律 | 遵循架构优先开发（每子任务开工前查架构表） |
| 形态 | 正式 roadmap 文档（跨会话，可由 executing-plans 接手） |

附加约束：**保留 CRAG 热更新差异化**（MemoryDomain + 域卸载 = O(region) 批量释放），不做会破坏它的对齐。

## 阶段结构

| 阶段 | 主题 | 子任务 |
|------|------|--------|
| Phase 1 | 正确性堵洞(P0) | GC-A1, GC-B1, GC-C1 |
| Phase 2 | 工业化韧性(P1) | GC-D1, GC-E1 |
| Phase 3 | 可观测性 + handle 硬化(P1) | GC-H1, GC-F1, GC-G1 |
| Phase 4 | 并发能力强化(P1) | GC-J1 |
| Phase 5 | 区域化内存重构(最高风险) | GC-K1, K2, K3, K4 |
| Phase 6 | 自适应增强(P2) | GC-L1, GC-L2 |

详见 `roadmap-v1-01.md` 第 4 节。

## 执行策略

```yaml
execution_mode: sequential
child_execution_mode: auto
auto_continue: true
auto_stop_policy: blocking-only
dispatch_model: sequential
```

## 关键决策摘要

1. **分配入口是稳定收敛点**：`CHAOS_IL2CPP_NEW_GC → GcAllocate → NurseryAllocate → tls_tlab`。区域化重构**内部切换 GcAllocate 实现，codegen 零改动**（wiki 17 号明确支持），是 Phase 5 可行的关键前提。
2. **对齐分四维**：正确性 → 工业化韧性 → 可调性 → 可观测性，区域化重构最末（依赖前四维硬化 + 旋钮体系）。
3. **非目标**（保留差异化）：SoftwareWriteWatch、standalone 插件化、CDAC/SOS、对象头内联位。
4. **相关既有任务**：`docs/dev/hanging/gc-phase3-industrialization/`（profile 驱动调参）作 E1/L2 数据来源参考，不并入本 roadmap。

## 调度状态

```yaml
dispatch_doc: 无（sequential 模式）
dispatch_model: sequential
active_batches: []
completed_batches: []
```

## 最近摘要

- 2026-08-10：创建本 roadmap（v1-01）。分析了 `D:\OpenSource\dotnet\runtime\src\coreclr\gc\`（~79,400 行）与 CRAG GC（~18,750 行）的核心差异，识别 12 项欠缺功能，按 P0/P1/区域化重构排列为 6 阶段 16 子任务。
- 2026-08-10：**GC-B1 已完成**。按用户要求移除 GC 核心 `CHAOS_GC_STRESS` 测试宏（对齐 CoreCLR「GC 核心零 stress」）：删 8 处热路径 `GcStressShouldTrigger()` + `gc_stress.h/cpp`(2 文件删除) + CMake option + features 文档行；因原 `gc_stress.h` 传递提供 `chaos_gc_collect` 声明，补直连 `#include "gc_helpers.h"` 到 `gc_region.cpp`。验证：`chaos_runtime_core` Debug 编译通过，`chaos_gc_region_test`(4/4) + `chaos_gc_atomic_alloc_test`(5/5) 运行 0 失败。`tests/` 压力测试用例全部保留（`gc_stress_test`/`gc_bgc_stress_test`/`gc_satb_stress_test` 等），不依赖已删符号。

## latest_stop_point

- GC-B1 已完成并验证。roadmap 其余子任务未派生。

## 下一步

- 启动 `recommended_next_child = GC-A1`（young GC 全根集扫描，修 UAF）。
- 派生子任务前，按用户要求先做 GC-A1 的架构优先检查（读 `24-CRAG-GC架构参考.md`，确认全根集扫描方案与既有 CRAG 架构一致）。

## recommended_next_child

- `GC-A1` — young GC 全根集扫描修复（正确性 P0，风险最高且独立）

## 关键文档

- `docs/dev/in-progress/gc-align-coreclr/roadmap-v1-01.md`

---

## Archive 节点（只读历史，用于派生时的目录校验）

### 已完成
- 无

### 进行中
- 无

### 挂起 / 废弃
- 相关：`docs/dev/hanging/gc-phase3-industrialization/`（不属本任务）
