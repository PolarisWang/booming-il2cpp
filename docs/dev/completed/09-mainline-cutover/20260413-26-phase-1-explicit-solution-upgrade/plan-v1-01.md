# Phase 1 Explicit Solution Upgrade 实现计划

> 面向执行 Agent：必须使用 `dev:executing-plans` 与 `dev:test-driven-development` 执行本计划。
> 目标：让 retained subject 真正具备显式 solution-aware source 通路，先打通 `.sln` source 的最小执行链，再为后续 `SolutionCorePack` / `HotUpdateHostPack` / `MixedExecutionFeaturePack` 物理合并提供稳定底座。

## 计划任务

- [x] Task 1: 激活 Phase 1 执行任务，并修正文档状态。
  - 范围：
    - 归档 Phase 0
    - 新建 `docs/dev/in-progress/20260413-26-phase-1-explicit-solution-upgrade/`
    - 更新 `docs/dev/ACTIVE.md`、父 roadmap 状态与索引
  - 验收：
    - `ACTIVE.md` 指向 Phase 1
    - 父任务明确记录 Phase 0 已完成、Phase 1 已启动

- [ ] Task 2: 通过失败测试冻结 `.sln` source 的 primary project / assembly 解析规则。
  - 范围：
    - 在 `tests/unit/execution/test_subject_workers.py` 为 solution source 增加 host-input build 测试
    - 在 `tests/unit/compatibility/test_compiled_subject_catalog.py` 为 solution source 增加 declared catalog / assembly resolve 测试
    - 明确 `source.path` 指向 `.sln` 时，必须如何提供 `primaryProjectPath`
  - 验收：
    - 新测试在实现前失败
    - 失败原因来自当前代码仍把 `.sln` source 当成 `.csproj` 或仍假设 `subjectId.dll`

- [ ] Task 3: 落地 solution-aware source 解析与 host-input / compiled catalog 实现。
  - 范围：
    - 修改 `build/toolchains/run/testing/subject_workers.py`
    - 修改 `build/toolchains/run/testing/compiled_catalog.py`
    - 必要时补充 `build/toolchains/run/testing/subjects.py` 的 source 辅助解析
  - 验收：
    - `.sln` source 能解析到 primary project
    - host-input manifest 能记录正确 primary assembly path
    - declared catalog 能从 solution source 找到 discovery assembly

- [ ] Task 4: 为 retained / archetype subject 补齐第一批显式 `.sln` 骨架并完成定向验证。
  - 范围：
    - 优先处理 `SolutionSimpleLib`、`SolutionMultiProject`、`SolutionPackageReference`
    - 评估并处理 `HotUpdateHostPack`、`MixedExecutionFeaturePack` 的 `.sln` 骨架
    - 只做 Phase 1 需要的最小结构调整，不在本批做大规模物理迁移
  - 验收：
    - 上述 subject 有真实 `.sln`
    - manifest/source 已能按 solution-aware 模式工作

- [ ] Task 5: 运行定向验证并收口 Phase 1。
  - 范围：
    - 运行新增单测
    - 运行相关定向兼容测试
    - 更新状态、progress 与索引
  - 验收：
    - Phase 1 的 solution-aware 通路验证通过
    - 父 roadmap 下一步明确切到 Phase 2 `SolutionCorePack` 合并

## 验证

- `python -m pytest tests/unit/execution/test_subject_workers.py -q`
- `python -m pytest tests/unit/compatibility/test_compiled_subject_catalog.py -q`
- `python -m pytest tests/unit/compatibility/test_subject_manifest_schema.py -q`

## 执行备注

- 本阶段不做“一次性重构全部底层 pipeline 实现”。
- `compatibility alias` 仍然保留到 Phase 6 一次性清零，不在本阶段半删半留。
- 如果 `.sln` source 需要新增最小 manifest 字段，优先使用 typed / 明确字段，例如 `primaryProjectPath`，避免继续扩散自由字符串约定。
