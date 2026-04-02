# Windows Reference Desktop Gate

## 定位

Windows 是 `Roadmap 0` 的 reference desktop 主基线。

## Gate 组成

- native ABI compile-only smoke
- native bridge compile-only smoke
- 5 个 smoke 输入项目 build
- Windows warmup trace compare
- Android startup smoke shell 调度
- Linux cross-compile / packaging shell 调度

## Trace Compare

```powershell
dotnet build tests/smoke/input/HostEmbeddingLite/HostEmbeddingLite.csproj -c Release
dotnet artifacts/smoke/bin/HostEmbeddingLite/Release/net8.0/HostEmbeddingLite.dll --trace-platform windows --trace-output artifacts/verify-roadmap-0/windows/windows-warmup-trace.runtime.json
pwsh ./tests/contracts/trace/compare-warmup-trace.ps1 -ExpectedPath ./tests/contracts/trace/snapshots/windows-warmup-trace.snapshot.json -ActualPath ./artifacts/verify-roadmap-0/windows/windows-warmup-trace.runtime.json
```

## `icall` 留白边界

- 只允许平台能力相关 `icall` 留在 query / basic-path
- 不允许把 ABI / bridge / lazy warmup contract 留空
- Android 与 Linux gate 在本阶段仍以 shell 调度为主，不宣称完整平台 runtime 已实现
