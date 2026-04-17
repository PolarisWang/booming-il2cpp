# Phase 0 Asset Source Policy v1.01

## 1. 目标

固定 `20260418-01` 主线在 `Phase 0` 的 foundation pack source model，避免后续 catalog、CI、proof 与 release gate 的资产来源漂移。

## 2. 当前结论

### 2.1 `.NET 8`

- sourceKind: `machine-global-official-installation`
- selectedVersion:
  - ref: `8.0.11`
  - runtime: `8.0.11`
- selectedRoots:
  - ref: `C:\Program Files\dotnet\packs\Microsoft.NETCore.App.Ref\8.0.11\ref\net8.0`
  - runtime: `C:\Program Files\dotnet\shared\Microsoft.NETCore.App\8.0.11`
- decision:
  - `Phase 0` 允许直接解析本机官方安装的 `.NET 8` packs
  - catalog 中必须显式记录绝对路径、版本与 sourceKind
  - 后续如果需要 CI 重现或离线执行，再评估是否把 `.NET 8` 也镜像到仓库资产目录

### 2.2 `.NET 10`

- sourceKind: `repo-frozen-snapshot`
- selectedVersion: `pending-snapshot-import`
- targetRoots:
  - ref: `assets/framework-packs/dotnet-foundation/net10/ref`
  - runtime: `assets/framework-packs/dotnet-foundation/net10/runtime`
- decision:
  - 当前执行环境不依赖 machine-global `.NET 10`
  - `.NET 10` 统一冻结为仓库内快照
  - 在 snapshot 未导入前，`Phase 0` 只允许推进 policy / catalog contract，不允许宣称可重现 `.NET 10` foundation path

## 3. 选择理由

- `.NET 8` 已在当前机器具备官方安装，可先作为实测 catalog baseline。
- `.NET 10` 当前机器缺失，继续把它视为外部预装会让 CI 与跨机会话重现性失控。
- 将 `.NET 10` 固定为 repo-frozen snapshot 后，后续 `FrameworkPackResolver` 可以对 `net8` 与 `net10` 采用统一的“显式 source record + version lock”模型，只是 root 不同。

## 4. 非目标

- 本文档不决定 `.NET 10` 最终具体 patch 版本号。
- 本文档不负责导入 snapshot 本体。
- 本文档不负责 facade/shim/classification 语义。

## 5. 对后续阶段的约束

- `Phase 1+` 不得再把 `.NET 10` 回退成“谁机器上装了就用谁”的隐式策略。
- 任何 `framework-catalog` 产物都必须同时记录：
  - `sourceKind`
  - `selectedVersion`
  - `rootPath`
  - `present/missing`
- 如果后续决定把 `.NET 8` 也切为 repo snapshot，必须新开 follow-up，并同步重写本 policy。

## 6. 当前 blocker

- blocker: `.NET 10` snapshot 尚未导入仓库。
- impact:
  - 可以继续推进 `.NET 8` catalog baseline
  - 不可以宣称 `.NET 10` 可重现或可比较
