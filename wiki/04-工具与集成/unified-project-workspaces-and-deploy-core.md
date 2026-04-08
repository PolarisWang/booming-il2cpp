# 统一入口工程工作区与 `deploy core`

## 一句话摘要

统一入口现在同时承载三类正式能力：

- `generate project ...` 生成本地调试工作区。
- `build project ...` 从工作区执行本地构建。
- `deploy core ...` 生成仓库正式 `core` 产品输出。

## 命令面

### Subject

```powershell
./run.ps1 generate project subject --id subject/<subject-id>
./run.ps1 generate project subject --id subject/<subject-id> --all-targets --refresh-generated
./run.ps1 build project subject --id subject/<subject-id> --all-targets
```

规则：

- `generate project subject` 生成 subject 源工程、subject 测试工程和 native 调试工程。
- `--refresh-generated` 只在显式传入时刷新 stable `subject-exec`。
- 不传 `--refresh-generated` 时，工作区复用已有 `artifacts/subjects/<subject-id>/runs/subject-exec/analysis/generated/`。

### Core

```powershell
./run.ps1 generate project core --host windows --all-targets
./run.ps1 build project core --host windows --all-targets
./run.ps1 deploy core --host windows --all-targets
```

规则：

- `generate project core` 生成 IL2CPP managed 核心工程和目标平台 native 工作区。
- `build project core` 读取工作区 manifest 构建 managed 与 native 目标。
- `deploy core` 不把工作区当正式产物，而是导出到 `deploy/core/**`。

## 产物边界

### 调试工作区

- Subject 工作区：`artifacts/projects/subjects/<subject-id>/`
- Core 工作区：`artifacts/projects/core/<host>/`

这些目录只服务于：

- IDE 打开
- 本地调试
- 本地构建
- CMake configure root / solution / build report

### 正式产品输出

- `deploy/core/<host>/<target>/`

这些目录才是仓库主线正式产物。

## `subject-exec` 规则

- `subject-exec` 不是 latest-run alias。
- 普通 `run test subject ...` 继续写入时间戳 run。
- 只有 `generate project subject --refresh-generated` 才会刷新 `subject-exec`。
- Subject native 工作区始终引用 `subject-exec`，从而保证 IDE 路径稳定。

## 当前落地语义

### Subject

- managed solution 自动收集 subject source 与 subject-owned validation project。
- native 工作区按 matrix 生成到 `artifacts/projects/subjects/<subject-id>/native/<matrix-id>/`。

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
- `python -m unittest tests.tooling.run.test_project_command`
- `python -m unittest tests.unit.run.test_project_workspace`
- `python -m unittest tests.unit.run.test_deploy_core`

## 最近更新

- `2026-04-08`：新增统一入口工程工作区与 `deploy core` 正式规则，冻结 `artifacts/projects/**` 与 `deploy/core/**` 边界，并明确 `subject-exec` 只允许手动刷新。
