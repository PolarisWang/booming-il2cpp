# macOS Reference Desktop Gate

## 定位

macOS 是 `Roadmap 0` reference desktop 的同语义子集，由 macOS host 承接。

## Gate 组成

- native ABI / bridge contract 相关子集
- 5 个 smoke 输入项目 build
- macOS warmup trace compare
- iOS compile/link/packaging shell 调度
- Linux cross-compile / packaging shell 调度

## Trace Compare

```bash
dotnet build tests/smoke/input/HostEmbeddingLite/HostEmbeddingLite.csproj -c Release
dotnet artifacts/smoke/bin/HostEmbeddingLite/Release/net8.0/HostEmbeddingLite.dll --trace-platform macos --trace-output artifacts/verify-roadmap-0/macos/macos-warmup-trace.runtime.json
pwsh ./tests/contracts/trace/compare-warmup-trace.ps1 -ExpectedPath ./tests/contracts/trace/snapshots/macos-warmup-trace.snapshot.json -ActualPath ./artifacts/verify-roadmap-0/macos/macos-warmup-trace.runtime.json
```

## `icall` 留白边界

- 只允许平台能力相关 `icall` 留在 query / basic-path
- 不允许把 trace contract、reference desktop contract 主干留空
- iOS gate 在本阶段是 compile/link/packaging shell，不等于完整移动端 runtime 语义
