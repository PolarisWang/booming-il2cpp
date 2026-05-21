# Phase 4 MainlineFeaturePack Onboarding 设计 v1.01

Date: 2026-04-09
Status: plan-ready

## 1. 设计目标

把 planning-only `subjects/MainlineFeaturePack` 接成真实完整 subject，同时不提前侵入 `Phase 5/6` 的 capability implementation：

- `source/` 可被 `dotnet build` 与 managed trace export 直接执行
- `validation/unit/` 提供 subject-owned xUnit 验证
- `validation/proof/native-reference/` 提供 native reference host
- `baselines/perf/` 提供 native perf baseline 入口
- `build-target` / 根 `CMakeLists.txt` 支持 subject-scoped proof build

## 2. 第一版 source 策略

第一版 source 采用最小可运行主场景：

- 保留 `Program::Main(System.String[])`
- 保留对象状态 + 实例方法消息构造，继续兼容当前 `Phase 3` 已稳定的 lowering family
- 增加受控的 trace export CLI：
  - `--trace-platform <platform>`
  - `--trace-output <path>`

默认执行路径负责输出稳定文本。
trace 执行路径负责生成与 `tests/contracts/trace/snapshots/windows-warmup-trace.snapshot.json` 对齐的 warmup trace 文档，并补齐 compare 脚本要求的 provenance。

## 3. subject-owned validation 设计

### 3.1 unit

新增：

- `subjects/MainlineFeaturePack/validation/unit/MainlineFeaturePack.Subject.UnitTests/`

职责：

- 锁定 manifest profile/matrix 绑定
- 锁定 trace export CLI surface
- 锁定 proof 目录与 baseline 文件存在

### 3.2 proof

新增：

- `subjects/MainlineFeaturePack/validation/proof/native-reference/`

实现策略：

- 复用当前 reference proof host 结构
- 仅把 subject-specific 常量、默认路径和 `image_name_utf8` 切到 `MainlineFeaturePack`

### 3.3 perf

不新增 managed perf harness。

理由：

- `Phase 4` 的 perf 目标是 generated native executable，而不是 managed runtime。
- 当前 `native-runtime-perf` worker 已能直接运行 native proof executable 并生成 perf evidence/report。

因此第一版只补：

- `subjects/MainlineFeaturePack/baselines/perf/windows-native-profile/windows.json`

## 4. build routing 设计

当前问题：

- 仓库根 `CMakeLists.txt` 在 `windows-x64-reference` 下硬编码 `HelloWorldObject` proof 目录。

调整方案：

- 在根 `CMakeLists.txt` 增加 `CHAOS_SUBJECT_PROOF_ROOT` cache 变量
- 默认值仍可回退到 `subjects/HelloWorldObject/validation/proof/native-reference`
- `build/toolchains/run/testing/subject_workers.py` 的 `_windows_subject_build(...)` 必须显式传入当前 subject 的 proof root

这样可以：

- 保持既有 `HelloWorldObject` 行为不变
- 让 `MainlineFeaturePack` 真的走自己的 proof host

## 5. manifest 调整

`subjects/MainlineFeaturePack/subject.manifest.json` 需要从 planning-only 调整为真实 onboarding surface：

- `proof-dev` -> `["proof", "unit"]`
- `trace-platform` -> `["proof"]`
- `perf-profile` -> `["proof", "perf"]`
- 新增 `validation.unit.project`
- 维持 `validation.perf.driver = native-runtime-perf`

## 6. 非目标

本阶段不做：

- Batch A 的 dispatch / generics / arrays 真功能实现
- Batch B 的 delegates / exceptions / reflection/interop 补位
- engine binding
- `contract` / `subject` 边界清理 follow-up

## 7. 结论

`Phase 4` 的正确最小落点是：

1. `MainlineFeaturePack` 变成真实完整 subject，而不是继续停留在 manifest 规划层。
2. proof build 改成 subject-scoped，消除 `HelloWorldObject` 硬编码。
3. perf 继续以 generated native executable 为观测对象，保持 performance-first 决策一致性。
