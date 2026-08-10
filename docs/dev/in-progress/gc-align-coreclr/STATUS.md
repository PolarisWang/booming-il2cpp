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
- 2026-08-10：**Phase 1 全部完成**（GC-B1 `6659812d4` / GC-A1 `e4dae1f97` / GC-C1 `f7850324e` + 两个 pre-existing 测试修复 `f5ceb0072`/`b9311d2e9`）。
- 2026-08-10：**Phase 2 GC-E1 完成**（`589baadfd` 配置旋钮，跨平台 env + AOT/JIT API）。
- 2026-08-10：**Phase 2 GC-D1 增量1+增量2 完成**：
  - 增量1（`5dc595cf7`）：OOM 终态对齐 handle_oom —— 半量预算 clamp + OOM 归因升级（真OOM/误报）。
  - 增量2（`226990998`）：provisional force-blocking —— 高记忆压力下 GC 强制 blocking（不 defer BGC），OOM 进入/恢复退出。
  - 验证：GC 单测 0 回归 + stress OOM 阶梯正常。

## latest_stop_point

- Phase 1 + GC-E1 + GC-D1（增量1+2）完成。GC-D1 记忆压力 decommit 待续。

## 进度（截至 2026-08-10）

| 阶段 | 子任务 | 状态 |
|------|--------|------|
| Phase 1 | GC-B1 / GC-A1 / GC-C1 | ✅ 完成 |
| Phase 2 | GC-E1 | ✅ 完成 |
| Phase 2 | GC-D1 | 🟠 增量1(半量+归因)+增量2(provisional force-blocking) done；decommit 待续 |
| Phase 3 | GC-H1 / GC-F1 / GC-G1 | ⬜ |
| Phase 4 | GC-J1 | ⬜ |
| Phase 5 | GC-K1..K4（区域化重构） | ⬜（最高风险，跨多会话） |
| Phase 6 | GC-L1 / GC-L2 | ⬜ |

---

## Archive 节点（只读历史，用于派生时的目录校验）

### 已完成
- 无

### 进行中
- 无

### 挂起 / 废弃
- 相关：`docs/dev/hanging/gc-phase3-industrialization/`（不属本任务）
