# Phase 0 Native Proof And Benchmark Matrix v1.01

## 1. 本阶段定位

`Phase 0` 不是单 DLL nativeization 阶段，而是 foundation asset intake 阶段。

因此本阶段的正式证据不是“某个 DLL 已 native 化”，而是：

- source policy 已冻结
- catalog 可重现
- diff contract 可审计
- 后续 phase 的 proof / benchmark 监控口径已稳定

## 2. Proof Matrix

| object | type | purpose | current gate |
| --- | --- | --- | --- |
| `asset-source-policy-v1-01.md` | policy-proof | 冻结 `.NET 8` 与 `.NET 10` source model | 必需 |
| `framework-catalog-v1-01.json` | catalog-proof | 固定选定版本、根目录、DLL 集与 present/missing 状态 | 必需 |
| `pack-diff-summary-v1-01.json` | diff-proof | 固定 `.NET 8` ref/runtime 差异与 `.NET 10` 缺口状态 | 必需 |
| `dotnet --list-sdks` / `dotnet --list-runtimes` | environment-proof | 证明本机当前可见 SDK/runtime 事实 | 必需 |

## 3. Benchmark Matrix

| benchmark | required | purpose | current state |
| --- | --- | --- | --- |
| catalog build time | optional | 观测未来 `framework-catalog` 生成开销 | 本阶段不强制 |
| pack diff build time | optional | 观测 future CI 的 metadata scan 成本 | 本阶段不强制 |
| per-assembly native throughput | no | 尚未进入 nativeization 阶段 | Phase 3+ 才进入 |

## 4. Monitor Signals

`Phase 0` 至少监控：

- `selectedNet8RefVersion`
- `selectedNet8RuntimeVersion`
- `net8RefDllCount`
- `net8RuntimeDllCount`
- `net8SharedAssemblyCount`
- `net8RuntimeOnlyDllCount`
- `net10SourceStatus`
- `outOfScopeLocalVersions`

## 5. 进入下一阶段前必须满足

- `framework-catalog-v1-01.json` 可解析
- `pack-diff-summary-v1-01.json` 可解析
- `.NET 10` source policy 已固定为单一路径模型
- 父 roadmap 与 child task 状态一致地指向 `Phase 1`

## 6. 当前说明

- 本阶段 `benchmarkRequired = false`
- 本阶段 `hotupdateImpact = None`
- 本阶段 formal verification object 以文档与 JSON 产物为主，而不是 declared proof / benchmark entry
