# Progress v1.01

Date: 2026-04-09 13:13:08 +08:00

## 本轮更新

- 已正式启动 `20260409-06-phase-4-mainline-feature-pack-onboarding`。
- 已确认当前缺口不是 pipeline/planning contract，而是 `MainlineFeaturePack` 缺少真实 `source / validation / baselines`。
- 已确认 `build-target` 仍通过根 `CMakeLists.txt` 硬编码 `HelloWorldObject` proof 目录；这必须成为 `Phase 4` 的一部分。
- 已冻结本任务边界：只做完整 subject onboarding，不提前混入 Batch A/B capability 实现。

## 当前建议

- 先补 RED 锁定真实 source tree、trace export、unit validation 和 subject-scoped proof build。
- 再最小实现 `MainlineFeaturePack` source / unit / proof / baseline，并用真实 `test subject` 三条入口做 focused verification。
