# Tests Taxonomy And Subject Project Pruning Roadmap v1.01

Date: 2026-04-13
Status: draft-approved

## 1. 目标

在已完成 retained subject cutover 的基础上，继续完成真正的仓库级收口：

- 顶层 `subjects/` 只保留 `SolutionCorePack`、`HotUpdateHostPack`、`MixedExecutionFeaturePack`
- subject-owned xUnit / perf harness 工程全部删除
- `SolutionCorePack` 不再依赖顶层 legacy subject 目录
- `tests/` 从旧 subject 目录绑定重构为“通用机制测试 + retained subject 烟测”
- solution-mode retained subjects 完成 proof / benchmark 验证闭环

## 2. 范围边界

本 roadmap 负责：

- retained subject 与 legacy subject 的最终物理收口
- solution-mode multi-assembly declared catalog 能力补齐
- public smoke / gate / run manifest / TUI 对 retained subject 的切换
- subject-owned 测试工程删除
- `tests/` taxonomy 清理

本 roadmap 不负责：

- 补齐新的移动设备真机农场
- 扩展新的 executable plan 类型
- 对所有历史测试素材做 1:1 保留

## 3. 非目标

- 不再保留长期 compatibility alias
- 不再继续新增 legacy `*Proof` / `Bench*` / `*Lite` 顶层 subject
- 不再把 subject 内部专有 `validation/` 工程视为主流测试机制

## 4. 阶段列表

### Phase 0: Inventory Freeze

- 冻结 retained / migrate / delete 清单
- 冻结“什么属于 retained smoke，什么属于 generic mechanism test”
- 冻结 `SolutionCorePack` 需要保留的内部 slice

### Phase 1: Solution-Mode Catalog Foundation

- 为 solution-mode retained subject 打通 multi-assembly declared catalog
- 更新 registry / planner / selection 对 retained subject object id 的假设

### Phase 2: SolutionCorePack Source Absorption

- 把顶层 legacy source 吸收到 `SolutionCorePack/source/*` slice
- 去掉 `SolutionCorePack` 对顶层 legacy project 的引用
- 删除已吸收完成的顶层 legacy subject

### Phase 3: Public Entry And Gate Cutover

- public smoke suite 改指 retained slice 或 fixture
- reference desktop gate / trace compare 改指 retained subject
- run manifest / TUI / examples 全部换成 retained subject

### Phase 4: Tests Taxonomy Cleanup

- 把 `tests/` 里的 subject-heavy 测试重写为 generic fixture 或 retained smoke
- 删除只验证旧目录存在的 compatibility 测试
- 把 subject-owned 测试工程对应的覆盖迁回 `tests/`

### Phase 5: Verification And Archive

- 跑 retained solution-mode subjects 的 proof / benchmark
- 跑核心 registry / planner / command / compiled catalog 测试
- 更新文档并归档

## 4.1 执行状态快照

Updated: 2026-04-13 14:28:02 +08:00

| Phase | status | result |
| --- | --- | --- |
| `Phase 0` | `in_progress` | 正在冻结保留集、删除集、tests taxonomy 边界以及第一批真实删除目标 |
| `Phase 1` | `planned` | 待补齐 solution-mode multi-assembly compiled catalog |
| `Phase 2` | `planned` | 待把 legacy source 真正吸收到 `SolutionCorePack` 内部 |
| `Phase 3` | `planned` | 待切换 public smoke / gate / run manifest / TUI |
| `Phase 4` | `planned` | 待完成 `tests/` generic / retained 两层重构 |
| `Phase 5` | `planned` | 待完成 proof / benchmark 与核心测试闭环 |

## 5. 每阶段完成定义

### Phase 0

- `goal`
  - 冻结最终保留结构和第一批删除目标，避免边做边改目标。
- `structure_adjustments`
  - 新增 inventory / matrix 文档。
- `checklist`
  - [ ] 列出顶层 retained subject 保留集
  - [ ] 列出顶层 legacy subject 删除集
  - [ ] 列出 `SolutionCorePack` 必需内部 slice
  - [ ] 列出 subject-owned xUnit / perf harness 删除集
  - [ ] 标注第一批必须保留的 support project
  - [ ] 标注第一批需要迁移的 tests 文件
- `deliverables`
  - inventory 冻结
  - 第一批执行清单
- `exit_criteria`
  - 任意一个 legacy subject 都能回答“迁到哪里 / 删掉为什么成立”
- `open_questions`
  - 无；用户已确认只保留三类 retained subject

### Phase 1

- `goal`
  - 让 solution-mode retained subject 自身就能暴露完整 declared catalog。
- `structure_adjustments`
  - 修改 `compiled_catalog.py`
  - 调整 `registry.py` / `subject_planner.py`
- `checklist`
  - [ ] 识别 `.sln` 源的程序集集合
  - [ ] 从 retained primary project output 收集 solution 程序集
  - [ ] declared catalog 支持多程序集 discovery
  - [ ] registry / planner object id 改用 retained subject id
  - [ ] 增加对应单元/集成测试
- `deliverables`
  - solution-mode compiled catalog 基础能力
- `exit_criteria`
  - `SolutionCorePack` 自身能够暴露 mainline / benchmark 声明条目

### Phase 2

- `goal`
  - 把 legacy source 真正吸回 retained subject 内部。
- `structure_adjustments`
  - 在 `subjects/SolutionCorePack/source/` 内新增内部 slice
  - 删除被吸收完成的顶层 legacy subject 目录
- `checklist`
  - [ ] 吸收 `MainlineFeaturePack`
  - [ ] 吸收 `PerformanceFeaturePack`
  - [ ] 吸收必要 smoke / interop / host / engine / bridge / mobile source
  - [ ] 更新 `SolutionCorePack.sln` 与 launcher 引用
  - [ ] 删除已完成迁移的顶层 legacy subject
- `deliverables`
  - `SolutionCorePack` 内部 slice 结构
  - 删除后的 `subjects/` 顶层清单
- `exit_criteria`
  - `SolutionCorePack` 不再引用顶层 legacy source 路径

### Phase 3

- `goal`
  - 把 public 入口全部切到 retained subject 或 fixture。
- `structure_adjustments`
  - 更新 `public_specs.py`
  - 更新 run manifest 源文件与生成结果
  - 更新 `build.py` / `tui.py`
- `checklist`
  - [ ] smoke suite 改指 retained slice 或 fixture
  - [ ] reference desktop gate 改指 retained subject
  - [ ] trace compare 改指 retained subject
  - [ ] manifest examples 不再出现 deleted legacy subject
  - [ ] TUI 示例 subject id 更新
- `deliverables`
  - public entry cutover
- `exit_criteria`
  - 用户默认入口不再暴露已删除 subject 名称

### Phase 4

- `goal`
  - 完成 `tests/` 的 taxonomy 清理。
- `structure_adjustments`
  - 增加/扩展 `tests/fixtures/subjects`
  - 改写 `tests/unit/*` 与 `tests/integration/*`
- `checklist`
  - [ ] planner / registry / manifest / command 测试迁到 fixture 或 retained smoke
  - [ ] 删除旧 subject 存在性断言
  - [ ] 删除 subject-owned xUnit / perf harness 后的覆盖迁回 `tests/`
  - [ ] 保留最小 retained subject 烟测
- `deliverables`
  - 重分层后的测试集
- `exit_criteria`
  - `tests/` 不再默认依赖 legacy subject 顶层目录

### Phase 5

- `goal`
  - 完成最终验证与归档。
- `structure_adjustments`
  - 更新 docs / wiki / roadmap 状态
- `checklist`
  - [ ] `run test registry refresh`
  - [ ] 验证 `SolutionCorePack` proof
  - [ ] 验证 `SolutionCorePack` benchmark
  - [ ] 验证 `HotUpdateHostPack` proof
  - [ ] 验证 `HotUpdateHostPack` benchmark
  - [ ] 验证 `MixedExecutionFeaturePack` proof
  - [ ] 验证 `MixedExecutionFeaturePack` benchmark
  - [ ] 跑核心 unit / integration suites
  - [ ] 更新状态文档并归档
- `deliverables`
  - 最终验证记录
- `exit_criteria`
  - 三个 retained solution-mode subject 的 proof / benchmark 均有最新证据

## 6. 子任务映射

| task_id | phase | status | owner | purpose | depends_on |
| --- | --- | --- | --- | --- | --- |
| `phase-0-inventory-freeze` | `Phase 0` | `in-progress` | `codex` | 冻结保留集、删除集、support project 与 tests taxonomy 边界 | `-` |
| `phase-1-solution-mode-catalog-foundation` | `Phase 1` | `planned` | `codex` | 打通 retained solution-mode 多程序集 declared catalog | `phase-0-inventory-freeze` |
| `phase-2-solution-core-pack-source-absorption` | `Phase 2` | `planned` | `codex` | 把 legacy source 真正吸回 `SolutionCorePack` | `phase-1-solution-mode-catalog-foundation` |
| `phase-3-public-entry-and-gate-cutover` | `Phase 3` | `planned` | `codex` | public smoke / gate / manifest / TUI 切换 retained 主线 | `phase-2-solution-core-pack-source-absorption` |
| `phase-4-tests-taxonomy-cleanup` | `Phase 4` | `planned` | `codex` | `tests/` 从旧 subject 目录绑定迁回 generic / retained 两层 | `phase-2-solution-core-pack-source-absorption` |
| `phase-5-verification-and-archive` | `Phase 5` | `planned` | `codex` | 跑 proof / benchmark 与核心测试并收口归档 | `phase-3-public-entry-and-gate-cutover, phase-4-tests-taxonomy-cleanup` |

## 7. 依赖

- `Phase 0` 是整个路线的前置冻结。
- `Phase 1` 必须先完成，否则 `SolutionCorePack` 无法真正承接 mainline / benchmark 的 declared objects。
- `Phase 2` 依赖 `Phase 1`，因为删除顶层 source 目录前必须先打通 retained solution-mode discovery。
- `Phase 3` 和 `Phase 4` 在 `Phase 2` 之后可交错推进。
- `Phase 5` 最后执行。

## 8. 风险

### 风险 1：`SolutionCorePack` 仍是“壳 subject”

影响：

- 顶层目录虽然删了，但核心能力仍依赖外部旧路径。

缓解：

- 必须把 source 物理吸收和 compiled catalog 一起做，不接受只改 manifest 的假迁移。

### 风险 2：tests 迁移过程中大量假阳性失败

影响：

- 阻碍判断真实回归问题。

缓解：

- 先分离 generic mechanism 与 retained smoke，再删旧 subject 断言。

### 风险 3：public smoke / gate 入口滞后

影响：

- 用户入口继续暴露旧 subject 名称，心智不一致。

缓解：

- 把 `public_specs.py`、run manifest、TUI、build gate 视为同一阶段收口。

## 9. 备选路径

已被用户否决：

- 继续保留大量顶层 legacy subject 只做别名兼容。
- 只做入口收口，不做物理迁移。

当前唯一执行路径：

- 在 retained subject 已立住的基础上，继续做一次真正的物理收口和 tests taxonomy 重构。

## 10. 当前建议推进顺序

1. 先做 inventory 和第一批删除清单。
2. 立即补齐 solution-mode multi-assembly catalog。
3. 再吸收 `MainlineFeaturePack` / `PerformanceFeaturePack`，因为它们是 `SolutionCorePack` 的最大外部依赖。
4. 然后切 public smoke / gate / manifest。
5. 最后统一重写 `tests/` 并完成 proof / benchmark 验证。
