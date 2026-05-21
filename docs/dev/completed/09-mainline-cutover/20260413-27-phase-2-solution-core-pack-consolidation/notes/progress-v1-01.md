# Progress v1.01

日期：2026-04-13 09:33:43 +08:00

## 本轮启动

- Phase 1 已完成并归档。
- 主 roadmap 已切入 Phase 2。
- 本批范围冻结为：
  - 建立 `SolutionCorePack` 顶层 retained subject
  - 先承接 archetype solution
  - 再承接 mainline / performance 的最小骨架
  - compatibility alias 暂留，等 Phase 6 一次性清理

## 当前约束

- `.sln + primaryProjectPath` 已经是既定执行契约，不再回退。
- `tests/` 中 subject-heavy 机制测试清理后置，不与本轮物理合并交叉推进。
