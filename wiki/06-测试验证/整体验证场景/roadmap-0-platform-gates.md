objectId: roadmap-0-platform-gates-guide
objectType: guide

# roadmap-0 platform gates

## 目的

- 把 `Roadmap 0` 里已经稳定存在的 platform gate 收敛成正式测试对象。
- 为 harness、skill 和发布前验证提供统一入口与准确的失败定位页面。

## 当前正式对象

- `gate/android-arm64-smoke`
- `system/android-startup-gate`
- `gate/ios-arm64-packaging`
- `system/ios-packaging-gate`
- `gate/linux-arm64-smoke`
- `system/linux-arm64-smoke-gate`
- `gate/linux-x64-packaging`
- `system/linux-packaging-gate`
- `gate/windows-reference-desktop`
- `system/windows-reference-gate`
- `gate/macos-reference-desktop`
- `system/macos-reference-gate`

## 边界

- `gate/android-arm64-smoke` 验证 Android startup smoke 路由是否仍可在 Windows 宿主上完成 configure/build 检查。
- `system/android-startup-gate` 把这条 gate 作为正式整体验证场景暴露给 registry 与 `run test system ...`。
- `gate/ios-arm64-packaging` 验证 iOS compile/link/packaging 路由是否仍可在 macOS 宿主上完成 configure/build 检查。
- `system/ios-packaging-gate` 把这条 gate 作为正式整体验证场景暴露给 registry 与 `run test system ...`。
- `gate/linux-arm64-smoke` 验证 Linux ARM64 cross-compile 路由是否可在当前宿主上通过 cmake + ninja 完成完整构建，并通过 QEMU user-mode 运行核心 GC 测试。
- `system/linux-arm64-smoke-gate` 把这条 gate 作为正式整体验证场景暴露给 registry 与 `run test system ...`。
- `gate/linux-x64-packaging` 验证 Linux cross-compile / packaging 路由是否仍可在当前宿主上完成 configure/build 检查。
- `system/linux-packaging-gate` 把这条 gate 作为正式整体验证场景暴露给 registry 与 `run test system ...`。
- `gate/windows-reference-desktop` 验证 Windows reference desktop 是否仍能完成 reference preset 构建，并通过 `HostEmbeddingLite` 的 `windows-managed-trace` subject matrix 产出并校验 Windows warmup trace。
- `system/windows-reference-gate` 把这条 gate 作为正式整体验证场景暴露给 registry 与 `run test system ...`。
- `gate/macos-reference-desktop` 验证 macOS reference desktop 是否仍能完成 reference preset 构建，并通过 `HostEmbeddingLite` 的 `macos-managed-trace` subject matrix 产出并校验 macOS warmup trace。
- `system/macos-reference-gate` 把这条 gate 作为正式整体验证场景暴露给 registry 与 `run test system ...`。
- 这条 gate 验证的是“矩阵可调度、路由可构建、shell 可落盘”，不是 Linux 目标平台的完整运行时闭环。
- reference desktop gate 验证的是“参考桌面基线仍可构建并且 trace contract 未漂移”，不是把整条 `workflow/roadmap-0-*` 全量重跑一遍。

## 正式入口

- `run test suite --id gate/android-arm64-smoke`
- `run test system --id system/android-startup-gate`
- `run test suite --id gate/ios-arm64-packaging`
- `run test system --id system/ios-packaging-gate`
- `run test suite --id gate/linux-arm64-smoke`
- `run test system --id system/linux-arm64-smoke-gate`
- `run test suite --id gate/linux-x64-packaging`
- `run test system --id system/linux-packaging-gate`
- `run test suite --id gate/windows-reference-desktop`
- `run test system --id system/windows-reference-gate`
- `run test suite --id gate/macos-reference-desktop`
- `run test system --id system/macos-reference-gate`

## 失败定位

- 统一测试日志：
  - `artifacts/logs/tests/<run-id>/summary.json`
  - `artifacts/logs/tests/<run-id>/console.log`
- reference desktop gate 附加产物：
  - gate record 仍落在 `artifacts/verify-roadmap-0/windows/windows-reference-desktop.gate.json`
  - gate record 仍落在 `artifacts/verify-roadmap-0/macos/macos-reference-desktop.gate.json`
  - 实际 trace runtime 证据改由 subject pipeline 落在 `artifacts/subjects/HostEmbeddingLite/runs/<run-id>/matrices/windows-managed-trace/runtime/trace.runtime.json`
  - 实际 trace runtime 证据改由 subject pipeline 落在 `artifacts/subjects/HostEmbeddingLite/runs/<run-id>/matrices/macos-managed-trace/runtime/trace.runtime.json`
  - 对应 matrix report 落在 `artifacts/subjects/HostEmbeddingLite/runs/<run-id>/matrices/<matrix-id>/pipeline-report/report.json`
- 更完整的本地执行背景，继续参考 [`docs/architecture/roadmap-0/local-verification.md`](../../../docs/architecture/roadmap-0/local-verification.md)。
