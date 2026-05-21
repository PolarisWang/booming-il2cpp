# Managed Minimal Closure v0

## 目标

本文定义 Roadmap 0 Stage 3 的 managed minimal closure 边界。
Stage 3 负责把真实编译产出的 managed assembly 收束成 generated-code input bundle，给后续 native proof 提供稳定上游；
它不负责 native bootstrap、reference runtime，或最终 `Hello, World!` 的执行证明。

## 输入

- legacy standalone proof input
  - `verification/catalog/owners/<subject-id>/support/host/<subject-id>.csproj`
  - `verification/catalog/owners/<subject-id>/support/host/bin/Release/net8.0/<subject-id>.dll`
- retained solution slice input
  - `verification/catalog/scenarios/<subject-pack>/<solution-name>/<slice-name>/<slice-name>.csproj`
  - `verification/catalog/scenarios/<subject-pack>/<solution-name>/<slice-name>/bin/Release/net8.0/<slice-name>.dll`
- concrete retained example
  - `verification/catalog/scenarios/SolutionCorePack/SimpleLibrarySolution/App/GoldenSimpleLib.App.csproj`
  - `verification/catalog/scenarios/SolutionCorePack/SimpleLibrarySolution/App/bin/Release/net8.0/GoldenSimpleLib.App.dll`

Stage 3 的实现必须读取真实 assembly metadata 和 method body，
不允许直接复制 Stage 2 的 canonical artifact JSON 作为伪实现输出。

## 输出

Stage 3 推荐输出根目录：

- `artifacts/subjects/<subject-id>/runs/<run-id>/analysis/analysis/`

根目录下最小文件集：

- `artifacts/subjects/<subject-id>/runs/<run-id>/analysis/analysis/typed-il-ir.json`
- `artifacts/subjects/<subject-id>/runs/<run-id>/analysis/analysis/aot-manifest.json`
- `artifacts/subjects/<subject-id>/runs/<run-id>/analysis/analysis/metadata-registration.json`
- `artifacts/subjects/<subject-id>/runs/<run-id>/analysis/analysis/hot-update/supplemental-metadata-template.json`
- `artifacts/subjects/<subject-id>/runs/<run-id>/analysis/analysis/code-registration.json`
- `artifacts/subjects/<subject-id>/runs/<run-id>/analysis/analysis/closure.manifest.json`

## 边界

- Stage 3 只结束在 managed closure bundle。
- Stage 3 不生成 native bootstrap 或 reference runtime proof。
- Stage 3 可以在 proof-bound 范围内只支持当前最小 metadata / IL 子集，但这些限制必须体现在通用 managed pipeline 的支持矩阵里，而不是把 test project / sample 名称硬编码进 `src/managed` 的核心职责里。

## 与 Stage 2 的关系

Stage 3 输出必须能对齐 Stage 2 已冻结的 canonical truth：

- entry subject: `HelloWorldObject/Program::Main(System.String[])`
- stdout truth: `Hello, World!`
- exit code truth: `0`
- generated direct-call landing path

四个核心 artifact 仍以 Stage 2 冻结的 schema / example / proof spec 作为上游契约来源，
`closure.manifest.json` 仅承担 Stage 3 聚合入口职责。
