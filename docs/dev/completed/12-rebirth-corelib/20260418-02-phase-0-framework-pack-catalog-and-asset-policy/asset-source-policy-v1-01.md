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
- selectedVersion:
  - ref: `10.0.6`
  - runtime: `10.0.6`
- selectedRoots:
  - ref: `D:\agent\booming-il2cpp\assets\framework-packs\dotnet-foundation\net10\ref`
  - runtime: `D:\agent\booming-il2cpp\assets\framework-packs\dotnet-foundation\net10\runtime`
- provenance:
  - importedFrom:
    - ref: `C:\Program Files\dotnet\packs\Microsoft.NETCore.App.Ref\10.0.6\ref\net10.0`
    - runtime: `C:\Program Files\dotnet\shared\Microsoft.NETCore.App\10.0.6`
  - importPolicy:
    - 本次由本机官方 `.NET 10` 安装导入到仓库
    - 导入后以 repo-frozen snapshot 作为唯一 canonical source
    - 后续消费读取仓库快照，不直接依赖 machine-global `.NET 10`
- decision:
  - `.NET 10` 统一冻结为仓库内快照
  - 本机官方安装只承担 snapshot provenance，不承担后续执行期 source model
  - `Phase 0` 可以把 `.NET 10` foundation path 视为可重现输入层

## 3. 选择理由

- `.NET 8` 已在当前机器具备官方安装，可先作为实测 catalog baseline。
- `.NET 10` 虽然当前机器已具备官方安装，但继续把它视为执行期外部预装仍会让 CI 与跨机会话重现性失控。
- 将 `.NET 10` 固定为 repo-frozen snapshot 后，后续 `FrameworkPackResolver` 可以对 `net8` 与 `net10` 采用统一的“显式 source record + version lock”模型，只是 root 不同。

## 4. 非目标

- 本文档不负责后续 `.NET 10` patch 升级策略；当前 `Phase 0` 已锁定 `10.0.6`。
- 本文档不负责 facade/shim/classification 语义。

## 5. 对后续阶段的约束

- `Phase 1+` 不得再把 `.NET 10` 回退成“谁机器上装了就用谁”的隐式策略。
- 任何 `framework-catalog` 产物都必须同时记录：
  - `sourceKind`
  - `selectedVersion`
  - `rootPath`
  - `present/missing`
- 如果后续决定把 `.NET 8` 也切为 repo snapshot，必须新开 follow-up，并同步重写本 policy。

## 6. 当前 blocker 状态

- blocker: `none`
- impact:
  - `Phase 0` 可以 closed 并进入 `Phase 1`
  - 后续 phase 可以把 `.NET 10` 视为可重现、可比较的 foundation 输入层
