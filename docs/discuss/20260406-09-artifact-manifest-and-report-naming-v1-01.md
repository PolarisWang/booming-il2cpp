# 阶段产物 Manifest / Report 命名 v1.01

Date: 2026-04-06
Status: design-discuss

## 1. 目标

本文件固定首批 `subject + matrix + stage` 管线下最小的文件命名约定。

本轮只覆盖：

- `HelloWorldObject`
- Windows 首批四个 matrix

但命名规则应当可直接推广到后续 subject。

## 2. 结论

首批应固定四类文件名：

- `*.manifest.json`
- `report.json`
- `summary.json`
- `events.jsonl`

并明确它们的语义边界：

- `*.manifest.json`
  - 表示某个 bucket 的正式产物清单与输入输出索引
- `report.json`
  - 表示单个 matrix 或单个局部校验动作的结果报告
- `summary.json`
  - 表示聚合汇总
- `events.jsonl`
  - 表示事件流

## 3. 命名原则

## 3.1 `manifest` 只描述“产物是什么”

`manifest` 不负责表达最终通过/失败结论，它只回答：

- 这个 bucket 产出了什么
- 这些产物来自哪个上游
- 关键文件在哪里

所以：

- `source.manifest.json`
- `host-input.manifest.json`
- `analysis.manifest.json`
- `generated.manifest.json`
- `build.manifest.json`
- `runtime.manifest.json`

都应是“产物索引”，而不是“测试总结”。

## 3.2 `report` 只描述“验证结果如何”

`report.json` 回答的是：

- 这条 matrix 跑得怎么样
- 哪些 stage 成功，哪些失败
- 最终停在哪个 bucket

所以：

- matrix 根下放 `report.json`
- 局部校验动作可放专用 `*.report.json`

例如：

- `trace-compare.report.json`
- `contract-validate.report.json`

## 3.3 `summary` 只用于聚合层

`summary.json` 不应该到处滥用。

首批只保留两类：

- `artifacts/subjects/<subject-id>/subject-report/summary.json`
- `artifacts/logs/tests/<run-id>/summary.json`

两者分别表示：

- subject 级汇总
- session / run 级汇总

## 3.4 `events.jsonl` 保持现有习惯

沿用现有 `events.jsonl`，因为当前 reporting / TUI 已经围绕它工作：

- 现有 session 报告使用 `summary.json` + `events.jsonl`
- 新 subject 报告也继续沿用同一命名

见：
[reporting.py](/E:/agent/booming-il2cpp/build/toolchains/run/testing/reporting.py#L72)
[test_reporting_layout.py](/E:/agent/booming-il2cpp/tests/integration/run/test_reporting_layout.py#L76)

## 4. 推荐目录骨架

```text
artifacts/subjects/<subject-id>/
  shared/
    source/
      source.manifest.json
    host-input/
      host-input.manifest.json
    analysis/
      analysis.manifest.json
      contract-validate.report.json
    generated/
      generated.manifest.json
  matrices/
    <matrix-id>/
      build/
        build.manifest.json
        stdout.log
        stderr.log
      runtime/
        runtime.manifest.json
        stdout.log
        stderr.log
        exit-code.json
        trace.runtime.json
        trace-compare.report.json
      report.json
      events.jsonl
  subject-report/
    summary.json
    events.jsonl
```

说明：

- `shared/` 放可跨 matrix 复用的前半段主线产物
- `matrices/<matrix-id>/` 放 matrix 独有的 `build/runtime/report`
- `subject-report/` 只放聚合视图

## 5. 每个文件的最小职责

## 5.1 `source.manifest.json`

职责：

- 记录当前 subject 的 source 解析结果

最小字段建议：

```json
{
  "subjectId": "HelloWorldObject",
  "bucket": "source",
  "sourceType": "dotnet-project",
  "sourcePath": "subjects/HelloWorldObject/source/HelloWorldObject.csproj",
  "entry": "HelloWorldObject/Program::Main(System.String[])",
  "inputs": [
    "subjects/HelloWorldObject/source/HelloWorldObject.csproj",
    "subjects/HelloWorldObject/source/Program.cs"
  ]
}
```

## 5.2 `host-input.manifest.json`

职责：

- 记录宿主构建后的正式 managed 输入包

最小字段建议：

```json
{
  "subjectId": "HelloWorldObject",
  "bucket": "host-input",
  "sourceManifestPath": "artifacts/subjects/HelloWorldObject/shared/source/source.manifest.json",
  "primaryAssemblyPath": "artifacts/subjects/HelloWorldObject/shared/host-input/HelloWorldObject.dll",
  "files": [
    "artifacts/subjects/HelloWorldObject/shared/host-input/HelloWorldObject.dll",
    "artifacts/subjects/HelloWorldObject/shared/host-input/HelloWorldObject.deps.json",
    "artifacts/subjects/HelloWorldObject/shared/host-input/HelloWorldObject.runtimeconfig.json"
  ]
}
```

## 5.3 `analysis.manifest.json`

职责：

- 记录 `frontend-pipeline` 产出的 `proof-input-bundle`

最小字段建议：

```json
{
  "subjectId": "HelloWorldObject",
  "bucket": "analysis",
  "hostInputManifestPath": "artifacts/subjects/HelloWorldObject/shared/host-input/host-input.manifest.json",
  "bundleKind": "proof-input-bundle",
  "artifacts": {
    "typedIlIrPath": "artifacts/subjects/HelloWorldObject/shared/analysis/typed-il-ir.json",
    "aotManifestPath": "artifacts/subjects/HelloWorldObject/shared/analysis/aot-manifest.json",
    "metadataRegistrationPath": "artifacts/subjects/HelloWorldObject/shared/analysis/metadata-registration.json",
    "codeRegistrationPath": "artifacts/subjects/HelloWorldObject/shared/analysis/code-registration.json",
    "closureManifestPath": "artifacts/subjects/HelloWorldObject/shared/analysis/closure.manifest.json"
  }
}
```

## 5.4 `contract-validate.report.json`

职责：

- 记录 analysis contract 校验结果

最小字段建议：

```json
{
  "subjectId": "HelloWorldObject",
  "stageId": "analysis-frontend",
  "status": "ok",
  "schemaSourceRoot": "contracts/artifacts/v0/schemas",
  "exampleSourceRoot": "contracts/examples/v0/artifacts",
  "snapshotSourceRoot": "tests/contracts/schema",
  "validatedArtifacts": [
    "typed-il-ir",
    "aot-manifest",
    "metadata-registration",
    "code-registration"
  ],
  "errors": []
}
```

## 5.5 `generated.manifest.json`

职责：

- 记录 Stage 4 generated 产物

最小字段建议：

```json
{
  "subjectId": "HelloWorldObject",
  "bucket": "generated",
  "analysisManifestPath": "artifacts/subjects/HelloWorldObject/shared/analysis/analysis.manifest.json",
  "generatedSourcePath": "artifacts/subjects/HelloWorldObject/shared/generated/HelloWorldObject.generated.cpp",
  "nativeProofManifestPath": "artifacts/subjects/HelloWorldObject/shared/generated/native-proof.manifest.json"
}
```

## 5.6 `build.manifest.json`

职责：

- 记录当前 matrix 的 build 输出

最小字段建议：

```json
{
  "subjectId": "HelloWorldObject",
  "matrixId": "windows-dev-output",
  "bucket": "build",
  "targetPlatform": "windows-x64",
  "toolchainProfile": "msvc-reference",
  "generatedManifestPath": "artifacts/subjects/HelloWorldObject/shared/generated/generated.manifest.json",
  "binaryRoot": "artifacts/subjects/HelloWorldObject/matrices/windows-dev-output/build/out",
  "outputs": [
    "artifacts/subjects/HelloWorldObject/matrices/windows-dev-output/build/out/chaos_stage4_hello_world_object_proof.exe"
  ]
}
```

## 5.7 `runtime.manifest.json`

职责：

- 记录 runtime 观察阶段的主要证据

最小字段建议：

```json
{
  "subjectId": "HelloWorldObject",
  "matrixId": "windows-reference-trace",
  "bucket": "runtime",
  "buildManifestPath": "artifacts/subjects/HelloWorldObject/matrices/windows-reference-trace/build/build.manifest.json",
  "stdoutPath": "artifacts/subjects/HelloWorldObject/matrices/windows-reference-trace/runtime/stdout.log",
  "stderrPath": "artifacts/subjects/HelloWorldObject/matrices/windows-reference-trace/runtime/stderr.log",
  "exitCodePath": "artifacts/subjects/HelloWorldObject/matrices/windows-reference-trace/runtime/exit-code.json",
  "tracePaths": [
    "artifacts/subjects/HelloWorldObject/matrices/windows-reference-trace/runtime/trace.runtime.json"
  ]
}
```

## 5.8 `exit-code.json`

职责：

- 只存进程退出证据

最小字段建议：

```json
{
  "exitCode": 0
}
```

不要把大段运行报告塞进这个文件。

## 5.9 `trace-compare.report.json`

职责：

- 只存 trace compare 结果

最小字段建议：

```json
{
  "subjectId": "HelloWorldObject",
  "matrixId": "windows-reference-trace",
  "status": "ok",
  "schemaPath": "tests/contracts/trace/schema/warmup-trace.schema.json",
  "expectedSnapshotPath": "tests/contracts/trace/snapshots/windows-warmup-trace.snapshot.json",
  "actualTracePath": "artifacts/subjects/HelloWorldObject/matrices/windows-reference-trace/runtime/trace.runtime.json",
  "errors": []
}
```

## 5.10 `report.json`

职责：

- 当前 matrix 的最终结果报告

最小字段建议：

```json
{
  "subjectId": "HelloWorldObject",
  "matrixId": "windows-dev-output",
  "goalId": "correctness.dev",
  "status": "ok",
  "terminalBucket": "runtime",
  "stageResults": [
    {"stageId": "source-resolve", "status": "ok"},
    {"stageId": "host-input-build", "status": "ok"},
    {"stageId": "analysis-frontend", "status": "ok"},
    {"stageId": "generated-native-proof", "status": "ok"},
    {"stageId": "build-target", "status": "ok"},
    {"stageId": "runtime-observe", "status": "ok"}
  ],
  "artifactResults": [
    {"bucket": "source", "manifestPath": "artifacts/subjects/HelloWorldObject/shared/source/source.manifest.json"},
    {"bucket": "host-input", "manifestPath": "artifacts/subjects/HelloWorldObject/shared/host-input/host-input.manifest.json"},
    {"bucket": "analysis", "manifestPath": "artifacts/subjects/HelloWorldObject/shared/analysis/analysis.manifest.json"},
    {"bucket": "generated", "manifestPath": "artifacts/subjects/HelloWorldObject/shared/generated/generated.manifest.json"},
    {"bucket": "build", "manifestPath": "artifacts/subjects/HelloWorldObject/matrices/windows-dev-output/build/build.manifest.json"},
    {"bucket": "runtime", "manifestPath": "artifacts/subjects/HelloWorldObject/matrices/windows-dev-output/runtime/runtime.manifest.json"}
  ],
  "errors": []
}
```

## 5.11 `subject-report/summary.json`

职责：

- 聚合当前 subject 的 matrix 结果

最小字段建议：

```json
{
  "subjectId": "HelloWorldObject",
  "requestedGoalId": "correctness.dev",
  "defaultMatrixId": "windows-dev-output",
  "matrixResults": [
    {
      "matrixId": "windows-dev-output",
      "status": "ok",
      "reportPath": "artifacts/subjects/HelloWorldObject/matrices/windows-dev-output/report.json"
    }
  ],
  "statusCounts": {
    "total": 1,
    "ok": 1,
    "fail": 0,
    "skip": 0,
    "aborted": 0
  }
}
```

## 6. `report.json` 与 `summary.json` 的边界

这是首批最容易混乱的点，必须定死。

### `report.json`

用于：

- 单个 matrix
- 单个局部校验动作

例如：

- `matrices/windows-dev-output/report.json`
- `runtime/trace-compare.report.json`
- `analysis/contract-validate.report.json`

### `summary.json`

用于：

- subject 聚合
- session 聚合

例如：

- `subject-report/summary.json`
- `artifacts/logs/tests/<run-id>/summary.json`

## 7. 与现有 session reporting 的兼容策略

现有 run/test 框架已经固定了：

- `artifacts/logs/tests/<run-id>/summary.json`
- `artifacts/logs/tests/<run-id>/events.jsonl`

见：
[reporting.py](/E:/agent/booming-il2cpp/build/toolchains/run/testing/reporting.py#L176)

首批不建议推翻它，而是采用双层结构：

- session 层继续写 `artifacts/logs/tests/<run-id>/summary.json`
- subject 层新增 `artifacts/subjects/<subject-id>/subject-report/summary.json`

推荐关系：

- session summary 记录 `subjectSummaryPath`
- subject summary 记录各 matrix `reportPath`

这样：

- 现有 TUI / watch / summary 入口不需要先重写
- 新 subject 管线也能有自己的稳定内部布局

## 8. 推荐的事件文件策略

首批建议只保留两级事件文件：

- matrix 级：`matrices/<matrix-id>/events.jsonl`
- subject 级：`subject-report/events.jsonl`

不建议再为每个 bucket 额外引入 `events.jsonl`，否则粒度太细，第一版维护成本过高。

事件最少应包含：

- `subjectId`
- `matrixId`
- `stageId`
- `eventType`
- `status`
- `timestamp`

## 9. 第一批必须固定的文件名

如果只保留第一批最小不可缺清单，我建议正式固定为：

- `source.manifest.json`
- `host-input.manifest.json`
- `analysis.manifest.json`
- `generated.manifest.json`
- `build.manifest.json`
- `runtime.manifest.json`
- `contract-validate.report.json`
- `trace-compare.report.json`
- `exit-code.json`
- `report.json`
- `summary.json`
- `events.jsonl`
- `stdout.log`
- `stderr.log`

## 10. 下一步实现含义

这份命名一旦固定，后续代码改造就可以直接围绕它进行：

1. `contracts.py` 切到 canonical `contracts/` + `tests/contracts/`
2. `frontend-pipeline-worker` 输出 `analysis.manifest.json`
3. `native-proof-emitter` 输出 `generated.manifest.json`
4. `build-target` 输出 `build.manifest.json`
5. `runtime-observe` 输出 `runtime.manifest.json`
6. `matrix-report-assembler` 输出 `report.json`
7. `subject-report-assembler` 输出 `subject-report/summary.json`
