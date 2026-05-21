# Phase 0 Framework Pack Catalog And Asset Policy Design v1.01

Date: 2026-04-18 01:50:00 +08:00
Status: design-frozen-for-execution

## 1. 目标

把 `20260418-01` 父 roadmap 的 `Phase 0` 收成可执行底座，先冻结：

- `.NET 8` 与 `.NET 10` foundation 资产来源
- `framework-pack` 目录模型
- Phase 0 catalog 的最小 machine-readable 产物
- 进入后续 facade/shim lane 之前必须成立的 proof / benchmark 监控口径

这一步不尝试直接解决 `.NET 10` 缺失问题，也不提前进入单 DLL nativeization。本阶段只负责把“输入资产与认证入口”钉死。

## 2. 边界

### 2.1 本阶段负责

- 固定 `Microsoft.NETCore.App.Ref` 与 `Microsoft.NETCore.App` 的 source policy
- 选定 `.NET 8` 当前开发基线版本
- 明确 `.NET 10` 的资产策略与目标目录
- 产出第一版 `framework-catalog` / `pack-diff-summary`
- 产出后续 phase 要消费的 proof / benchmark / monitor matrix

### 2.2 本阶段不负责

- 导入完整 `.NET 10` snapshot
- 建立 `DotNetFoundationPack` subject
- 进入 facade/shim、corelib 或 `System.*` 的单 DLL 认证
- 修改 current managed/native/hotupdate runner 或 subject 主线

## 3. 核心决策

### 3.1 资产来源

- `.NET 8`
  - 当前开发基线直接解析本机官方安装的 `Microsoft.NETCore.App.Ref/8.0.11` 与 `Microsoft.NETCore.App/8.0.11`
  - catalog 中必须显式记录 machine-global 来源与绝对路径
- `.NET 10`
  - 当前仓库不依赖 machine-global `.NET 10`
  - 统一冻结为“repo-frozen snapshot”策略
  - 目标路径先固定为 `assets/framework-packs/dotnet-foundation/net10/`
  - 在 snapshot 未导入前，Phase 0 保持 `in_progress`

### 3.2 Catalog 结构

第一版 `framework-catalog-v1-01.json` 只要求回答四件事：

- 当前选中的 foundation 版本是什么
- ref/runtime 根目录是什么
- 当前 DLL 集规模是什么
- `.NET 10` 当前处于 `missing-snapshot` 还是 `present`

它不在本阶段承担 facade/shim/classification 语义，这些留到 `Phase 1`。

### 3.3 Diff 结构

第一版 `pack-diff-summary-v1-01.json` 只冻结：

- `.NET 8` 当前 ref 与 runtime 的集合差异
- `.NET 10` 资产当前不可比对的阻塞原因
- 后续需要比较的目标维度

## 4. 产物

- `asset-source-policy-v1-01.md`
- `framework-catalog-v1-01.json`
- `pack-diff-summary-v1-01.json`
- `phase-0-native-proof-and-benchmark-matrix-v1-01.md`

## 5. 完成定义

本 child task 只有在以下条件同时满足后才允许 completed：

- `.NET 8` 基线版本与来源已固定
- `.NET 10` 资产策略已固定，且 snapshot 已存在或用户明确改成外部预装
- catalog / diff / monitor matrix 已落盘并通过基本解析校验
- 父 roadmap 已把 `20260418-02` 标记为 `completed`

如果 `.NET 10` snapshot 仍缺失，则本任务可以推进文档和 catalog 基线，但不能声称 completed。

## 6. 验证口径

- 文档校验：
  - `asset-source-policy-v1-01.md`
  - `phase-0-native-proof-and-benchmark-matrix-v1-01.md`
- 机器可读校验：
  - `framework-catalog-v1-01.json`
  - `pack-diff-summary-v1-01.json`
- 本机事实校验：
  - `dotnet --list-sdks`
  - `dotnet --list-runtimes`
  - `Get-ChildItem "$env:ProgramFiles\\dotnet\\packs\\Microsoft.NETCore.App.Ref"`
  - `Get-ChildItem "$env:ProgramFiles\\dotnet\\shared\\Microsoft.NETCore.App"`

## 7. 风险

- 如果把 `.NET 8` 的 machine-global 解析与 `.NET 10` 的 repo snapshot 策略混成同一类，会让 resolver contract 失真。
- 如果 Phase 0 就把 facade/shim/classification 写死进 catalog，Phase 1 会被迫返工。
- 如果不显式承认 `.NET 10` 仍缺 snapshot，就会出现“文档说可执行、实际无法重现”的伪推进。
