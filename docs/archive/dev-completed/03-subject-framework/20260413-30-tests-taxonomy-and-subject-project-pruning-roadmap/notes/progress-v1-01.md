# Progress v1.01

Date: 2026-04-13 14:28:02 +08:00

## 本轮进展

- 新建 `20260413-30-tests-taxonomy-and-subject-project-pruning-roadmap` 父任务。
- 冻结本轮主目标：
  - 顶层 `subjects/` 最终只保留 3 个 retained subject
  - subject-owned xUnit / perf harness 工程全部删除
  - `tests/` 从 legacy subject 目录绑定迁回 generic mechanism + retained smoke
- 完成现状盘点：
  - `SolutionCorePack` launcher 仍直接引用顶层 `MainlineFeaturePack` / `PerformanceFeaturePack`
  - `compiled_catalog.py` 当前仅扫描 primary project 程序集
  - `public_specs.py` / run manifest / `build.py` 仍引用多个 legacy subject
  - `tests/` 中仍有大量“旧 subject 目录必须存在”的 compatibility 测试

## 下一步

- 做第一批真实代码改动：
  - 删除 subject-owned xUnit / perf harness 工程及对应 manifest 引用
  - 盘点 `SolutionCorePack` 对顶层 legacy source 的剩余物理依赖
  - 为 solution-mode retained subject 补齐 multi-assembly declared catalog 的落地方向

## Wiki

- 本轮先不写 wiki，等结构冻结和第一批代码改动落地后再决定是否沉淀长期知识。
