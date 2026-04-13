# Phase 2 SolutionCorePack Consolidation 实现计划

> 面向执行 Agent：继续使用 `dev:executing-plans`、`dev:test-driven-development` 和 `dev:verification-before-completion`。
> 目标：建立 `SolutionCorePack` 作为新的顶层 retained subject，并吸收 archetype/mainline/performance 的最小可运行骨架；compatibility alias 延后到 Phase 6 再统一清理。

## 计划任务

- [x] Task 1: 激活 Phase 2 并冻结 `SolutionCorePack` 的最小结构
  - 范围：
    - 创建 `subjects/SolutionCorePack/subject.manifest.json`
    - 创建 `subjects/SolutionCorePack/source/SolutionCorePack.sln`
    - 冻结第一批 slice 目录：`Launcher/`、`Archetypes/`、`Mainline/`、`Benchmarks/`
  - 验收：
    - `SolutionCorePack` 作为新顶层 retained subject 存在
    - manifest/sourceModel/source/path/primaryProjectPath 具备最小可识别结构

- [ ] Task 2: 先吸收 archetype solution 样例
  - 范围：
    - 承接 `SolutionSimpleLib`
    - 承接 `SolutionMultiProject`
    - 承接 `SolutionPackageReference`
    - 保持 golden assembly identity 与 project-graph 行为不变
  - 验收：
    - `SolutionCorePack` 内出现可运行的 archetype slice
    - 相关 convert / project-graph / planning 验证保持通过

- [ ] Task 3: 建立 `Mainline` 与 `Benchmarks` 的第一批承接面
  - 范围：
    - 梳理 `MainlineFeaturePack` 的 proof 入口迁移到 `Mainline/`
    - 梳理 `PerformanceFeaturePack` 的 benchmark 入口迁移到 `Benchmarks/`
    - 补齐 solution 模式下 `source.entry` 指向非 primary assembly 时的真实执行策略
    - 保持 `ChaosUnitTest` / `ChaosBenchmark` 方向，不引入新的 legacy root
  - 验收：
    - `SolutionCorePack` 中出现最小 mainline proof 与 benchmark 承接点
    - 旧 subject 暂时仍可通过 compatibility 路径工作

- [ ] Task 4: 让 registry/planner/selector 能识别 `SolutionCorePack`
  - 范围：
    - 补齐 manifest/registry 最小识别
    - 保证 path resolver / planner / command 层能面向新 retained subject 工作
    - 不提前切换默认用户入口
  - 验收：
    - `SolutionCorePack` 可被发现、选择、建 plan
    - 旧 retained/archetype subject 不被本阶段破坏

- [ ] Task 5: 跑定向回归并收口 Phase 2 第一批交付
  - 范围：
    - 跑与 `SolutionCorePack` 相关的规划、registry、subject command 回归
    - 更新状态、progress、索引
  - 验收：
    - 第一批 `SolutionCorePack` 骨架和承接通路稳定
    - 父 roadmap 可以继续进入 Phase 3/4，而不需要回头补 `.sln` 基础能力

## 初始验证候选

- `python -m pytest tests/unit/planning/test_subject_planner.py -q`
- `python -m pytest tests/unit/planning/test_project_graph.py -q`
- `python -m pytest tests/integration/registry/test_registry_scan.py -q`
- `python -m pytest tests/integration/registry/test_declared_metadata_discovery.py -q`

## 执行备注

- 本阶段不一次性删除 compatibility alias。
- 本阶段不做 `tests/` 与 `subjects/` 的最终解耦清理，那部分已经后置到 roadmap 尾声。
- 如需新增 `SolutionCorePack` 最小 launcher / slice project，优先保持多 project、显式 solution、显式 entry，不要回退到目录约定驱动。
