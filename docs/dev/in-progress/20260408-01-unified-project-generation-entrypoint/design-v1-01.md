# 统一入口工程生成与可调试工作区设计 v1

## 设计目标

在不破坏现有 canonical artifact 规则的前提下，为统一入口新增一套正式的 project/workspace 生成能力，使用户可以：

- 一键生成 subject 的 managed + test + native 调试工程；
- 一键生成 IL2CPP 核心代码工程；
- 通过统一入口对生成出的目标平台工程执行 build。

## 总体方案

对外保留 `generate project` 语义，对内落地为“工作区生成器”。

工作区只生成以下内容：

- `solution` / `workspace manifest`
- native CMake configure root
- 指向现有源码与 stable generated root 的配置文件

工作区不复制以下内容：

- `subjects/**` 下源码
- `src/managed/**` 下源码
- canonical run-scoped 证据

## 推荐命令面

### Subject 工作区

```text
run generate project subject --id subject/<subject-id> [--matrix <matrix-id>] [--all-targets] [--variant CHECK|PROFILE|SHIP] [--refresh-generated]
```

语义：

- 解析指定 subject。
- 如 subject 包含 `generated-native-proof` 阶段，则先把 stable run `subject-exec` 刷新到最新 generated/native 输入。
- 生成 subject 调试工作区。
- `--matrix` 只生成指定 matrix 的 native/project。
- `--all-targets` 为该 subject 的所有 matrix 生成工程。

### Core 工作区

```text
run generate project core [--host windows|macos|linux] [--all-targets]
```

语义：

- 生成 IL2CPP 核心 managed 工程工作区。
- 生成 core native/contracts/platform routing 的工作区。
- `--all-targets` 生成当前 host 可支持的所有目标平台 configure root。

### 配套 build 命令

```text
run build project subject --id subject/<subject-id> [--matrix <matrix-id>] [--all-targets]
run build project core [--host windows|macos|linux] [--all-targets]
```

语义：

- build 命令不再重新推断路径，而是读取已生成的 workspace manifest。
- 如果工作区不存在，则返回明确错误并提示先运行 `generate project ...`。

## 输出布局

### Subject

```text
artifacts/projects/subjects/<subject-id>/
  workspace.manifest.json
  managed/
    <subject-id>.sln
  native/
    <matrix-id>/
      <cmake configure root and IDE files>
```

`workspace.manifest.json` 至少记录：

- `subjectId`
- `variant`
- `managedSolutionPath`
- `generatedRoot`
- `matrices[]`
- 每个 matrix 对应的：
  - `matrixId`
  - `targetPlatform`
  - `toolchainProfile`
  - `configureRoot`
  - `buildCommand`
  - `primaryOpenTarget`

### Core

```text
artifacts/projects/core/
  workspace.manifest.json
  managed/
    chaos-il2cpp-core.sln
  native/
    <target-id>/
      <cmake configure root and IDE files>
```

`workspace.manifest.json` 至少记录：

- `managedSolutionPath`
- `nativeTargets[]`
- 每个 native target 的：
  - `targetId`
  - `presetTarget`
  - `targetPlatform`
  - `configureRoot`
  - `buildCommand`

## Subject 生成流程

### 1. 刷新 stable generated 输入

如果 subject 需要 `generated-native-proof`：

- 执行 `source-resolve`
- 执行 `host-input-build`
- 执行 `analysis-frontend`
- 执行 `generated-native-proof`

固定使用 `run_id=subject-exec`，这样本地调试工程引用的是稳定路径：

`artifacts/subjects/<subject-id>/runs/subject-exec/analysis/generated/`

这样做的原因：

- 复用当前 native proof host 默认 wiring。
- 避免把最新 run-id 硬编码进生成的工程文件。
- 不新增第二套 generated canonical 规则。

### 2. 生成 managed solution

managed solution 包含：

- `subjects/<subject-id>/source/*.csproj`
- `subjects/<subject-id>/validation/unit/**/*.csproj`（如存在）
- 后续可按 manifest 增补 perf harness 等 validation project

首版在 Windows 上优先产出 Visual Studio 可直接打开的 `.sln`。

### 3. 生成 native 调试工程

对于每个选定 matrix：

- 读取 `subject.manifest.json` 的 `executionContext.targetPlatform`
- 生成 CMake configure root 到 `artifacts/projects/subjects/<subject-id>/native/<matrix-id>/`
- Windows 本地调试优先使用 Visual Studio generator，而不是当前执行态的 `Ninja Multi-Config`
- configure 时显式注入：
  - `CHAOS_SUBJECT_VARIANT`
  - `CHAOS_SUBJECT_GENERATED_ROOT`
  - `CHAOS_SUBJECT_BUILD_OUT_ROOT`
  - `CHAOS_SUBJECT_RUNTIME_ROOT`

其中：

- `GENERATED_ROOT` 指向 stable `subject-exec`
- `BUILD_OUT_ROOT` / `RUNTIME_ROOT` 指向 `artifacts/projects/subjects/<subject-id>/native/<matrix-id>/out|runtime`

### 4. 产出 workspace manifest

工作区 manifest 是 build 命令与 IDE 打开的统一真源。

## Core 生成流程

### 1. 生成 managed core solution

包含 `src/managed/` 下所有核心项目：

- `Chaos.IL2CPP.Contracts`
- `Chaos.IL2CPP.Loader`
- `Chaos.IL2CPP.SemanticWorld`
- `Chaos.IL2CPP.MetadataWriter`
- `Chaos.IL2CPP.CodeGen`
- `Chaos.IL2CPP.Linker`
- `Chaos.IL2CPP.Pipeline`
- `Chaos.IL2CPP.Driver`

### 2. 生成 native/core/contracts/platform 工程

工作区覆盖当前真实 native/core 面：

- `src/native/runtime-core`
- `src/native/support`
- `src/native/bootstrap`
- `tests/contracts/native/abi`
- `tests/contracts/native/bridge`
- `tests/gate/android-smoke`
- `tests/gate/ios-smoke`（仅 macOS host）
- `tests/gate/linux-packaging`

Windows host 首版优先覆盖：

- windows reference
- android-arm64 buildable
- linux-x64 buildable

macOS host 后续覆盖：

- macos reference
- ios-arm64 buildable
- linux-x64 buildable

## Build 语义

`build project ...` 的职责：

- 读取工作区 manifest
- 对 managed solution 执行 `dotnet build`
- 对 native configure root 执行 `cmake --build`
- `--all-targets` 时串行构建 manifest 中所有 target
- 输出统一 artifacts/report，且把 build 结果写回 workspace manifest 或配套 build report

## 与现有统一入口的关系

- 不新增新的菜单分组，首版命令放入 `Build` 组即可。
- `run help` / `run list` / `run capability` 自动暴露新命令。
- handler 建议新增 `project.dispatch`，避免把生成逻辑塞入现有 `build.dispatch`。
- `build project ...` 仍归 `build.dispatch`，但新增 `kind=project-workspace` 分支。

## 关键边界

### 不是 canonical 证据的内容

以下内容属于“开发工作区”，不是正式证据：

- `artifacts/projects/**`
- `.sln`
- CMake configure root
- IDE 辅助文件

### 仍然是 canonical 的内容

- `artifacts/subjects/<subject-id>/runs/<run-id>/**`
- `artifacts/logs/tests/**`
- `artifacts/verify-roadmap-0/**`

## 验证策略

至少需要新增 / 调整以下验证：

- `tests/tooling/run/test_command_manifest.py`
  - 新命令可见性
  - CLI parse
- `tests/tooling/run/*`
  - `generate project` / `build project` 的 handler 行为
  - workspace manifest 结构
- `tests/unit/run/test_subject_workers.py`
  - subject stable generated root 与 native configure 参数
- 如有必要新增：
  - `tests/tooling/run/test_project_workspace_generation.py`

## 分阶段实施建议

本设计建议进入 roadmap，并按以下阶段推进：

1. 统一入口命令面、workspace manifest、静态文档与测试脚手架。
2. subject 工作区生成：managed solution + stable generated refresh + native configure roots。
3. core 工作区生成与 `build project ...` 集成。

## 推荐拍板项

如果没有额外要求，建议按以下假设推进：

1. Windows/Visual Studio 作为首版一等公民体验。
2. `generate project` 生成的是“引用仓库真源的工作区”，不是源码副本。
3. subject native generated 输入统一绑定 stable `subject-exec`。
