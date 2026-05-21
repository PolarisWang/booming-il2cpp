# Phase 4 MainlineFeaturePack Onboarding Brainstorm v1.01

Date: 2026-04-09
Status: plan-ready

## 1. 背景

`MainlineFeaturePack` 在 `Phase 1` 只冻结了 manifest / matrix / pipeline planning surface。
到 `Phase 4`，它必须从 planning-only subject 变成真实完整宿主：

- 有真实 `source/`
- 有 subject-owned `validation/unit/`
- 有 subject-owned `validation/proof/native-reference/`
- 有 `baselines/perf/`
- 能承接 `correctness.dev`、`correctness.platform`、`perf.profile`

## 2. 当前缺口

- 目录下只有 `subject.manifest.json`，没有真实 source tree。
- `proof-dev` 还没有接入 unit validation。
- `perf-profile` 目前只有 pipeline surface，没有真实 baseline 文件。
- 仓库根 `CMakeLists.txt` 在 `windows-x64-reference` 路径下硬编码 `subjects/HelloWorldObject/validation/proof/native-reference`，无法让新 subject 真正 build/run。

## 3. 方案比较

### 方案 A：最小接入真实 source + unit + proof + baseline，并把 proof build 做成 subject-scoped

优点：

- 改动面最小，直接对齐 `Phase 4` 的最小退出条件。
- 不提前混入 Batch A/B capability 实现，边界清晰。
- 能尽快得到真实 `MainlineFeaturePack` 的三条执行入口。

缺点：

- 第一版 source 仍然会比较保守，暂时不会承载 Batch A/B 的真实语义压力。

结论：

- 采用。

### 方案 B：在 onboarding 时就把 Batch A 的 dispatch / generics / arrays 一起落地

优点：

- 表面上更像“完整主宿主”。

缺点：

- 会把 `Phase 4` 和 `Phase 5` 搅在一起。
- 一旦失败，很难判断是 onboarding 问题还是 capability 问题。

结论：

- 当前不采用。

### 方案 C：只补 source 与 manifest，不补 subject-owned proof / trace / perf

优点：

- 实现快。

缺点：

- 不满足完整 subject 的真正目的。
- 仍然无法形成 unified native output / trace / perf 主线。

结论：

- 不采用。

## 4. 结论

`Phase 4` 采用方案 A：

1. `MainlineFeaturePack` 第一版 source 只承载最小可运行主场景，不提前混入 Batch A/B 真功能。
2. 真实补齐 `source`、`validation/unit`、`validation/proof/native-reference`、`baselines/perf`。
3. source 必须支持 `--trace-platform` / `--trace-output`，让 `windows-reference-trace` 真能执行。
4. native proof build 必须从 `HelloWorldObject` 硬编码切到 subject-scoped 入口。
5. perf 仍坚持 generated native executable 为观测对象，不退回 managed-only perf。
