# 统一入口工程工作区、`verification/workspaces/**` 与 `deploy core`

## 一句话摘要

统一入口现在区分两类输出：
- `generate/build project ...` 面向 IDE 与本地调试，统一落在 `verification/workspaces/**`
- `deploy core ...` 面向正式产品导出，继续落在 `deploy/core/**`

`run menu` 中的 `Project And IDE` / `工程 / IDE` 负责聚合工程生成与工程构建入口。

## 命令面

### 聚合入口

```powershell
./run.ps1 generate project all --host windows
./run.ps1 generate project all --host windows --refresh-generated
```

规则：
- `generate project all` 为当前宿主生成全部可用 subject/core workspace。
- 聚合索引固定写到：
  - `verification/workspaces/verification-all.manifest.json`
  - `verification/workspaces/verification-all.generation.report.json`
- `--refresh-generated` 只在显式传入时刷新 subject stable `subject-exec`。

### Subject

```powershell
./run.ps1 generate project subject --id subject/<subject-id>
./run.ps1 generate project subject --id subject/<subject-id> --matrix <matrix-id> --variant PROFILE
./run.ps1 generate project subject --id subject/<subject-id> --all-targets --refresh-generated
./run.ps1 build project subject --id subject/<subject-id> --all-targets
```

规则：
- `generate project subject` 生成 subject source 工程、subject-owned validation 工程和 native 调试工程。
- Subject 工作区固定落在 `verification/workspaces/subjects/<subject-id>/`。
- Subject managed solution 固定命名为 `verification/workspaces/subjects/<subject-id>/<subject-id>.sln`。
- Windows 宿主下生成 subject solution 时，会把同次生成的 native `.vcxproj` 一并写入 `.sln`。
- 不传 `--refresh-generated` 时，工作区继续复用 `artifacts/subjects/<subject-id>/runs/subject-exec/analysis/generated/`。

### Core

```powershell
./run.ps1 generate project core --host windows --all-targets
./run.ps1 build project core --host windows --target linux-x64
./run.ps1 deploy core --host windows --all-targets
```

规则：
- `generate project core` 生成 IL2CPP managed 核心工程和目标平台 native 工作区。
- Core 工作区固定落在 `verification/workspaces/core/<host>/`。
- Core managed solution 固定命名为 `verification/workspaces/core/<host>/chaos-il2cpp-core.sln`。
- `deploy core` 不把工作区当正式产物，而是导出到 `deploy/core/**`。

## 菜单语义

### 文本菜单

- `run menu` 提供 `Project And IDE` 分组。
- 该分组固定暴露 5 个入口：
  - `generate-project-all`
  - `generate-project-subject`
  - `generate-project-core`
  - `build-project-subject`
  - `build-project-core`
- `deploy-core` 继续留在 `Build`，不与 IDE 工程入口混用。

### 全屏 / 内联 TUI

- 一级菜单顺序固定为：
  - `prepare`
  - `project`
  - `build`
  - `test`
  - `clean`
  - `inspect`
- `project-menu` 继续收集参数，而不是把带必填参数的命令直接暴露给用户。
- 当前支持的参数流：
  - `generate project all`：选择是否 `--refresh-generated`
  - `generate/build project subject`：选择 `subject`、目标范围、可选 `matrix`，以及 generate 场景下的 `variant` / `refresh-generated`
  - `generate/build project core`：选择目标范围和可选 `target`
- 菜单底部详情区固定显示：
  - `简介`
  - `输出`
  - `目标范围`
  - `当前宿主支持`
  - `执行命令`
  - `备注`

## 产物边界

### IDE / 调试工作区

```text
verification/workspaces/
  verification-all.manifest.json
  verification-all.generation.report.json
  subjects/
    <subject-id>/
      workspace.manifest.json
      <subject-id>.sln
      native/
        <matrix-id>/
  core/
    <host>/
      workspace.manifest.json
      chaos-il2cpp-core.sln
      native/
        <target-id>/
```

这些目录只服务于：
- IDE 打开
- 本地调试
- 本地构建
- solution / native project / workspace manifest / build report

### 正式产品输出

- `deploy/core/<host>/<target>/`

这些目录才是仓库主线正式产物。

## `subject-exec` 规则

- `subject-exec` 不是 latest-run alias。
- 普通 `run test subject ...` 继续写入时间戳 run。
- 只有 `generate project subject --refresh-generated` 和 `generate project all --refresh-generated` 才会刷新 `subject-exec`。
- Subject native 工作区始终引用 stable `subject-exec`，从而保证 IDE 路径稳定。

## 当前落地语义

### Subject

- managed solution 自动收集 subject source 与 subject-owned validation project。
- native 工作区按 matrix 生成到 `verification/workspaces/subjects/<subject-id>/native/<matrix-id>/`。
- Windows 下 subject solution 会引用同次生成的 native `.vcxproj`。

### Core

- managed solution 固定包含 `Chaos.IL2CPP.Contracts`、`Loader`、`SemanticWorld`、`MetadataWriter`、`CodeGen`、`Linker`、`Pipeline`、`Driver`。
- native 目标按 host 支持矩阵生成：
  - Windows：`windows-x64-reference`、`android-arm64`、`linux-x64`
  - macOS：`macos-reference`、`ios-arm64`、`linux-x64`
  - Linux：`linux-x64`

### Deploy

- 桌面 reference 目标导出 `bin/chaos-il2cpp(.exe)` 与 `package-root/`。
- 非桌面目标至少导出 `package-root/` 与 `deploy.manifest.json`。
- `deploy.manifest.json` 记录 `deliveryKind`、`workspaceManifestPath`、`buildReportPath` 和入口可执行文件。

## 验证入口

本轮功能以以下自动化回归为基线：
- `python -m unittest tests.tooling.run.test_command_manifest`
- `python -m unittest tests.tooling.run.test_tui`
- `python -m unittest tests.tooling.run.test_tui_routing`
- `python -m unittest tests.tooling.run.test_project_command`
- `python -m unittest tests.unit.run.test_project_workspace`

## 最近更新

- `2026-04-08`：新增统一入口工程工作区与 `deploy core` 正式规则，冻结 `artifacts/projects/**` 与 `deploy/core/**` 边界，并明确 `subject-exec` 只允许手动刷新。
- `2026-04-10`：工程工作区根从 `artifacts/projects/**` 迁移到 `verification/workspaces/**`，新增 `Project And IDE` 菜单与 `generate project all` 聚合入口，并让 Windows subject solution 引用同次生成的 native project。
