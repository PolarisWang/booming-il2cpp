# Phase 4 Unified Entry And Registry Cutover 实施计划

> 面向执行 Agent：继续使用 `dev:executing-plans`、`dev:test-driven-development` 和 `dev:verification-before-completion`。
> 目标：在不提前删除 compatibility alias 的前提下，把 retained solution-mode subject 的入口模型、manifest 表达、registry / selector / CLI 默认路径统一起来。

## 计划任务

- [ ] Task 1: 审计 retained subject 当前入口差异
  - 范围：
    - 盘点 `SolutionCorePack`、`HotUpdateHostPack`、`MixedExecutionFeaturePack` 的：
      - `Launcher/Program.cs`
      - `source.entry`
      - matrix `source.entry`
      - `workloadEntry`
      - registry / selector / CLI 展示与默认路径
    - 标记哪些差异只是 subject 内部 slice 差异，哪些差异已经泄漏到公共路由层。
  - 验收：
    - 输出最小 cutover 方案，不把 Phase 4 扩大成 backend 重构。

- [ ] Task 2: 引入 typed entry / slice dispatch
  - 范围：
    - 设计并落地轻量入口类型：
      - `ChaosSubjectEntryKind`
      - `ChaosSubjectSlice`
    - 所有公开枚举成员补注释。
    - 尽量使用枚举与紧凑类型，避免新增字符串协议。
    - 让 retained subject 的 launcher 收敛到统一的 dispatch 形态。
  - 验收：
    - 三个 retained subject 内部入口风格一致。
    - 不需要继续记忆散落的 `Program::Run*` 入口。

- [ ] Task 3: 完成 registry / selector / CLI cutover
  - 范围：
    - 更新 compiled catalog / planner / executor / selector / CLI 帮助信息，使默认 public 路径以 retained subject 为中心。
    - 兼容 alias 仅保留 redirect 语义，不再作为独立主路径。
  - 验收：
    - 默认展示和默认解析优先走 retained subject。
    - 不破坏已有兼容入口。

- [ ] Task 4: 运行回归并收口 Phase 4
  - 范围：
    - 跑与 manifest / planner / registry / executor / selector 相关的定向回归。
    - 必要时刷新 registry。
    - 更新 child / parent 状态、进度记录与索引。
  - 验收：
    - Phase 4 收口后，Phase 5 可以直接执行 retained subject proof / benchmark 全量验收。

## 初始验证候选

- `python -m pytest tests/unit/planning/test_subject_planner.py tests/unit/execution/test_subject_executor.py tests/unit/execution/test_subject_workers.py tests/unit/compatibility/test_compiled_subject_catalog.py tests/unit/compatibility/test_subject_manifest_schema.py tests/integration/registry/test_declared_metadata_discovery.py -q`

## 执行备注

- 本阶段不删除 compatibility alias。
- 本阶段不展开 `tests/` 与 `subjects/` 的最终解耦清理。
- 本阶段不把 `MixedExecutionFeaturePack` 的 native benchmark 收口重新扩大成通用 generic native AOT benchmark backend 改造。
