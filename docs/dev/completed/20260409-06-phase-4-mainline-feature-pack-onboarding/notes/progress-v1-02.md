# Progress v1.02

Date: 2026-04-09 14:05:00 +08:00

## 本轮更新

- 在进入 `Chunk 1` RED 前确认了一个真实结构冲突：当前 native proof 链路默认绑定程序集真实 entrypoint，但 `MainlineFeaturePack` 又要求真实 `Main` 支持 trace CLI。
- 现有 `LoaderStage` / `NativeReferenceLoweringPlanner` 仍要求 proof entry 保持极简单块形状，因此不能直接把 trace 参数解析塞进真实 `Main` 后还继续沿用当前 proof 主线。
- 已补写 `brainstorm-v1-02.md` 与 `design-v1-02.md`，推荐把 subject manifest 的 `source.entry` 升级为真实 proof entry override。

## 当前建议

- 先确认是否采用 `proof entry override` 设计。
- 一旦确认，就先补这一层 RED，再继续 `MainlineFeaturePack` onboarding 与 subject-scoped proof build。
