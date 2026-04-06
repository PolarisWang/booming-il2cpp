# Subject Framework Foundation And Windows Cutover v1

Date: 2026-04-06
Status: active

## 1. 目标

把 `subject` 测试框架首轮已经稳定下来的 Stage A+B+C 边界写成长期文档，避免后续实现继续回退到旧的 Stage 4 隐式 orchestration。

## 2. Canonical Truth

### 2.1 Subject 真源

- `subjects/HelloWorldObject/` 是 `HelloWorldObject` 的唯一正式真源。
- `subjects/HelloWorldObject/source/HelloWorldObject.csproj`
- `subjects/HelloWorldObject/source/Program.cs`
- `tests/proof/input/HelloWorldObject/HelloWorldObject.csproj` 只保留 compatibility shell 角色，不再作为 canonical source。

### 2.2 Contract 真源

`build/toolchains/run/testing/contracts.py` 的 canonical contract source 固定为：

- `contracts/artifacts/v0/schemas`
- `contracts/examples/v0/artifacts`
- `tests/contracts/schema`

`analysis/contracts/**` 可以继续保留为兼容镜像或历史资产，但不再是长期正式真源。

## 3. Artifact Bucket Layout

Subject artifact root 固定为：

`artifacts/subjects/<subject-id>/`

其中 Stage A+B+C 已冻结的 bucket 命名与 manifest 路径为：

- `shared/source/source.manifest.json`
- `shared/host-input/host-input.manifest.json`
- `shared/analysis/analysis.manifest.json`
- `shared/generated/generated.manifest.json`
- `matrices/<matrix-id>/build/build.manifest.json`
- `matrices/<matrix-id>/runtime/runtime.manifest.json`

`HelloWorldObject` 当前使用的生成链路为：

1. `source`
2. `host-input`
3. `analysis`
4. `generated`
5. `build`
6. `runtime`

## 4. Execution Spine

### 4.1 Planner

`build/toolchains/run/testing/subject_planner.py` 输出的最小稳定 contract 包含：

- `planVersion`
- `request`
- `selection`
- `artifactsRoot`
- `stagePlan`

每个 stage 需要固定：

- `stageId`
- `kind`
- `scope`
- `bucket`
- `dependsOn`
- `fingerprint`
- `upstreamFingerprints`
- `paths`
- `executionMode`
- `reuse`

`executionMode` 的稳定取值为：

- `reused`
- `executed`
- `invalidated`

### 4.2 Executor

`build/toolchains/run/testing/subject_executor.py` 只负责：

- 按 `stagePlan` 顺序执行
- 按 bucket-local contract 调度 worker
- 汇总 matrix-local `stageResults`

它不直接写：

- `subject summary`
- `session summary`
- session `events.jsonl`

### 4.3 Worker Boundary

Stage A+B+C 首轮冻结的最小 worker 集合为：

- `source-resolve`
- `host-input-build`
- `analysis-frontend`
- `generated-native-proof`
- `build-target`
- `runtime-observe`
- `runtime-trace-compare`

其中最关键的三段内部职责边界为：

- `host-input-build`
  - 只把 canonical source materialize 成 host input bucket。
- `analysis-frontend`
  - 只生成 managed closure / analysis bucket，并做 contract validate。
- `generated-native-proof`
  - 只把 analysis bucket materialize 成 generated bucket。

## 5. Windows Cutover Boundary

### 5.1 Proof Host CMake

`tests/proof/native-reference/HelloWorldObject/CMakeLists.txt` 不再隐式执行：

- host-input build
- frontend pipeline
- native emission

它现在只消费：

- `artifacts/subjects/HelloWorldObject/shared/generated/generated/native-reference.generated.cpp`

以及外部注入的 root：

- `CHAOS_HELLOWORLD_GENERATED_ROOT`
- `CHAOS_HELLOWORLD_BUILD_OUT_ROOT`
- `CHAOS_HELLOWORLD_RUNTIME_ROOT`

### 5.2 首批 Windows Matrix

Stage C 首轮固定的矩阵为：

- `windows-dev-output`
- `windows-reference-trace`
- `windows-android-buildable`
- `windows-linux-buildable`

它们都必须通过 `subject_planner` + `subject_executor` 消费新的 subject artifact 链。

## 6. verify-roadmap-0 Compatibility Gate

`build/scripts/verify-roadmap-0.py` 的职责已经退化为 compatibility gate，而不是 monolithic orchestrator。

Windows 分支固定为：

- 触发 `HelloWorldObject` subject matrix
- 校验 runtime artifacts
- 记录 gate json

`build/scripts/verify-roadmap-0.ps1` 只保留 Python forwarder 角色，不再复制 subject orchestration 或 proof helper 逻辑。

为兼容过渡期资产，`validate_stage4_proof_run_artifacts()` 同时接受：

- 新的 runtime bucket 根目录
- legacy 根目录下的 `run/` 子目录

## 7. 非目标

本轮 Stage A+B+C 明确不包含：

- `perf.dev / perf.release`
- 完整 reporting / subject summary / session summary
- public CLI / registry / TUI cutover
- 多 subject 批量扩展

这些内容分别后置到后续 `Stage D + Stage E` 与 `Stage F`。
