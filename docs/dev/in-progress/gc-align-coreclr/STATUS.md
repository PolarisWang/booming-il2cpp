# STATUS — GC 代码级对齐 CoreCLR + 工业化落地三批次

## 元信息

```yaml
task_id: gc-align-coreclr
task_type: roadmap
phase: roadmap-v3
roadmap_or_plan: docs/dev/in-progress/gc-align-coreclr/roadmap-v3-01.md
created: 2026-08-10
created_by: main-agent
source: 用户要求 - 按优先级逐项对齐 CoreCLR GC 全部剩余功能；2026-08-14 用户确认按 1+2+3 三批次推进工业化落地（gc-industrialization-gap-analysis-2026-08-14.md）
blocking_questions: []
question_clearance: cleared
clearance_confirmed_by_user: true
```

## 用户确认门（2026-08-10）

| 决策 | 内容 |
|------|------|
| 对齐深度 | 全部完成（roadmap-v2 覆盖 P0/P1/P2 全部剩余功能域） |
| 约束 | 多平台 + JIT/AOT + 热更友好 |
| 形态 | 正式 roadmap（roadmap-v2-01.md，可执行） |
| 保留 | CRAG 热更新差异化（域卸载 O(region)） |

## 进度（截至 2026-08-14）

| 阶段 | 子任务 | 状态 |
|------|--------|------|
| Phase 1-5 | GC-A1..K4（13 子任务） | ✅ 已完成 |
| Phase 6 | GC-L1/L2 | ⬜ 纳入 v2 Phase 8 (M6/M3B) |
| Phase 7-9 | GC-M1..M15（v2 功能对齐） | ⬜ M1 部分完成（regen link-ready），M2 已建测试暴露疑点后闭环 |
| **Phase 10（批次 1）** | GC-N1..N4（P0 护网闭合） | ✅ GC-N1/N3/N4 已提交；GC-N2 由并行线承接 |
| **Phase 11（批次 2）** | GC-N5..N8（P1 工程闭环） | 🔄 GC-N5 ✅ 已提交；GC-N6 发现 2 个真实缺陷（屏障已修 `ef0012d49`，gen1↔old-gen 重叠升级 P0 专项）；**GC-N7 Release 基准双缺陷已修 `904114c3d`（crash 率 80%→~8%），残余 young-GC scan_ptr 越界专项中** |
| **Phase 12（批次 3）** | GC-N9..N12（P2 能力拉平） | ⬜ |

## 下一步

- 批次 1：GC-N1/N3/N4 已提交（待 CI 实跑确认）；GC-N2 由并行 GC 调试线承接。
- 批次 2：GC-N5 已提交（L1 卡表 UAF）；GC-N6 发现已固化（`notes/gc-n6-liveness-findings-2026-08-25.md`）。**GC-N7 余波**：`904114c3d` 修双缺陷（析构 FreePage UAF + ResizeGen1Region 清空新 gen1）；残余偶发 young-GC Phase 2 scan_ptr 越界（:537）+ old-gen `retired_arrays_` teardown AV（`notes/gc-n7-release-benchmark-crash-2026-08-25.md`）——**dev-systematic-debugging 专项进行中**。GC-N7 Release 基准需残余修复后产出。
- 约束满足方式：每子任务按 roadmap 三约束原则（多平台纯 C++ / JIT-AOT 同符号 / 热更兼容入口）+ 架构优先前置。

## 关键文档

- `docs/dev/in-progress/gc-align-coreclr/roadmap-v2-01.md`
- `docs/dev/assessments/gc-vs-coreclr-remaining-features-2026-08-10.md`（26 项功能域输入）
- 子任务：gc-a1..gc-k4（Phase 1-5 已提交）

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
- 2026-08-10：**Phase 5 区域化重构全部完成**：
  - GC-K1（`69613b545` SelectRegionSize 4/2/1MB）+ K1b（`862da35a1` LOH-via-region）。
  - GC-K2（`cbc1b6925` K2a 表 + `53246252f` K2b 双参屏障 + `4f59836d1` K2c codegen + `547f8864f` K2d card bundle；K2e 覆盖）。
  - GC-K3（`ec71dce0b` Gen1→young 重绑）。
  - GC-K4（区域化回归：11 项 GC 单测 0 失败）。
- 验证：完整 region 世代写屏障链路（skewed 表 + 双参屏障 + codegen + bundle + Gen1-rebind）跨平台纯 C++，11 项单测全 0。

## latest_stop_point

- Phase 1-5 完成；v2 M1 部分完成。**批次 1（GC-N1/N3/N4）已落地**：barrier 测试进快 gate（本地 `-LE`/`-L` 验证）、Server GC 冒烟 job、ASAN nightly job、夜间去 `|| true`。GC-N2 由并行 GC 调试线承接。下一步批次 2 GC-N5（L1 卡表并发读稳定化）。worktree 干净（除并行线未提交文件）。

## 进度（截至 2026-08-10）

| 阶段 | 子任务 | 状态 |
|------|--------|------|
| Phase 1 | GC-B1 / GC-A1 / GC-C1 | ✅ |
| Phase 2 | GC-E1 / GC-D1 | ✅ |
| Phase 3 | GC-G1 / GC-F1 / GC-H1 | ✅ |
| Phase 4 | GC-J1 | ✅ |
| Phase 5 | GC-K1/K1b, K2a-d, K3, K4 | ✅ **全部完成** |
| Phase 6 | GC-L1 / GC-L2 | ⬜ 下一步 |

> Phase 5 剩余（K2 实现 + K3 分代重绑 + K4 回归 + K2c codegen BOUNDARY）与 Phase 6 是跨多会话主线，每个强依赖前者。

---

## Archive 节点（只读历史，用于派生时的目录校验）

### 已完成
- 无

### 进行中
- 无

### 挂起 / 废弃
- 相关：`docs/dev/hanging/gc-phase3-industrialization/`（不属本任务）
