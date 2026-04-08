# Subject Framework Foundation And Windows Cutover v1

Date: 2026-04-06
Status: active

## 1. 目标

把 `subject` 测试框架 Stage A+B+C 的正式边界写成长期文档，避免回退到旧的隐式 Stage 4 orchestration。

## 2. Canonical Truth

### 2.1 Subject 真源

- `subjects/HelloWorldObject/` 是 `HelloWorldObject` 的唯一正式真源
- `subjects/HelloWorldObject/source/HelloWorldObject.csproj`
- `subjects/HelloWorldObject/source/Program.cs`
- `tests/proof/` 只保留通用迁移说明，不再承载 subject 专属 proof source 或 native host

### 2.2 Contract 真源

`build/toolchains/run/testing/contracts.py` 当前读取的 canonical contract source：

- `contracts/artifacts/v0/schemas`
- `contracts/artifacts/v0/samples`
- `contracts/artifacts/v0/snapshots`

根目录 `analysis/` 已退役；subject 主线里的 analysis 只表示 run-scoped artifact bucket，而不是仓库顶层源码根。

## 3. Artifact Bucket Layout

subject artifact root：

`artifacts/subjects/<subject-id>/runs/<run-id>/`

当前冻结的 bucket 路径：

- `analysis/source/source.manifest.json`
- `analysis/host-input/host-input.manifest.json`
- `analysis/analysis/analysis.manifest.json`
- `analysis/generated/generated.manifest.json`
- `matrices/<matrix-id>/build/build.manifest.json`
- `matrices/<matrix-id>/runtime/runtime.manifest.json`

## 4. Execution Spine

最小 worker 集合：

- `source-resolve`
- `host-input-build`
- `analysis-frontend`
- `generated-native-proof`
- `build-target`
- `runtime-observe`
- `runtime-trace-compare`

## 5. Windows Cutover Boundary

### 5.1 Proof Host CMake

`subjects/HelloWorldObject/validation/proof/native-reference/CMakeLists.txt` 不再隐式执行：

- host-input build
- frontend pipeline
- native emission

它当前只消费：

- `artifacts/subjects/HelloWorldObject/runs/<run-id>/analysis/generated/generated/native-reference.generated.cpp`

以及外部注入的 root：

- `CHAOS_SUBJECT_GENERATED_ROOT`
- `CHAOS_SUBJECT_BUILD_OUT_ROOT`
- `CHAOS_SUBJECT_RUNTIME_ROOT`

### 5.2 首批 Windows Matrix

- `windows-dev-output`
- `windows-reference-trace`
- `windows-android-buildable`
- `windows-linux-buildable`

## 6. verify-roadmap-0 Compatibility Gate

`build/scripts/verify-roadmap-0.py` 现在是 compatibility gate，不再是 monolithic orchestrator。

## 7. 非目标

本轮 Stage A+B+C 不包含：

- `perf.dev / perf.release`
- 完整 reporting / subject summary / session summary
- public CLI / registry / TUI cutover
- 多 subject 批量扩展
