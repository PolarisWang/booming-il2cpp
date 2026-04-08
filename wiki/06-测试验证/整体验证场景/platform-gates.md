objectId: runtime-baseline-platform-gates-guide
objectType: guide

# runtime-baseline platform gates

## 正式对象

- `gate/android-arm64-smoke`
- `system/android-startup-gate`
- `gate/ios-arm64-packaging`
- `system/ios-packaging-gate`
- `gate/linux-x64-packaging`
- `system/linux-packaging-gate`
- `gate/windows-reference-desktop`
- `system/windows-reference-gate`
- `gate/macos-reference-desktop`
- `system/macos-reference-gate`

## 入口

- `run test suite --id gate/android-arm64-smoke`
- `run test system --id system/android-startup-gate`
- `run test suite --id gate/ios-arm64-packaging`
- `run test system --id system/ios-packaging-gate`
- `run test suite --id gate/linux-x64-packaging`
- `run test system --id system/linux-packaging-gate`
- `run test suite --id gate/windows-reference-desktop`
- `run test system --id system/windows-reference-gate`
- `run test suite --id gate/macos-reference-desktop`
- `run test system --id system/macos-reference-gate`

## 失败定位

- 汇总日志位于 `artifacts/logs/tests/<run-id>/`
- gate record 位于 `artifacts/verify-runtime-baseline/<host>/`
- 运行态 trace 证据仍由 `subject` pipeline 写入 `artifacts/subjects/HostEmbeddingLite/runs/<run-id>/...`
