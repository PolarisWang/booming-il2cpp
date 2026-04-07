# docs/discuss INDEX

## 说明

- `docs/discuss/` 现在按 `docs/discuss/{topic}/xxxx.md` 组织，topic 目录是后续讨论的 canonical 入口。
- 根目录仍保留一批旧的平铺文件作为兼容副本；当前由于 Windows 文件锁，暂未就地清理，后续可在锁释放后收尾。
- 当某个方向已经完成决策并进入正式实施，应把稳定结论迁移到 `docs/architecture/` 或 `docs/dev/`。

## Topics

- `test-framework/`
  - 测试框架重构最早一轮 brainstorm / design 基线。
  - 入口：`docs/discuss/test-framework/INDEX.md`
- `roadmap-0/`
  - 历史 `roadmap-0` 在新框架中的定位与降级判断。
  - 入口：`docs/discuss/roadmap-0/INDEX.md`
- `helloworldobject-windows/`
  - `HelloWorldObject` 首批 Windows matrix 与 stage/worker 设计。
  - 入口：`docs/discuss/helloworldobject-windows/INDEX.md`
- `subject-framework/`
  - `subject + matrix + artifact-chain` 的主模型、cutover、planner / executor / worker / manifest 讨论。
  - 入口：`docs/discuss/subject-framework/INDEX.md`
- `reporting/`
  - reporting / events / session compatibility 的设计讨论。
  - 入口：`docs/discuss/reporting/INDEX.md`
- `il2cpp-mainline/`
  - IL2CPP 主线 roadmap 骨架与后续按阶段持续细化的专题讨论。
  - 入口：`docs/discuss/il2cpp-mainline/INDEX.md`

## 当前建议阅读顺序

如果是理解新测试框架：

1. `docs/discuss/test-framework/INDEX.md`
2. `docs/discuss/subject-framework/INDEX.md`
3. `docs/discuss/reporting/INDEX.md`
4. `docs/discuss/roadmap-0/INDEX.md`

如果是继续 IL2CPP 主线讨论：

1. `docs/discuss/il2cpp-mainline/20260407-01-il2cpp-mainline-roadmap-skeleton-v1-01.md`
2. `docs/discuss/il2cpp-mainline/20260407-02-phase-a-phase-b-boundary-v1-01.md`
