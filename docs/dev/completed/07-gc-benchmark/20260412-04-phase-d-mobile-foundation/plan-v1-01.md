# Phase D 移动端基础设施收口实现计划

> 面向执行 Agent：本计划已完成，复盘时以本文件和 `notes/progress-v1-02.md` 为准。

**目标**: 在当前 Windows 环境内完成 Phase D 的移动端基础设施收口，留下可验证代码与明确的外部环境边界。  
**设计文档**: `docs/dev/completed/20260412-04-phase-d-mobile-foundation/design-v1-01.md`

## 执行结果

### Batch 1: iOS 合规策略与 managed 校验

- [x] 新增 `contracts/shared/v0/ios-hot-update-strategy.md`
- [x] 新增 `src/managed/Chaos.IL2CPP.HotUpdate/IosComplianceValidator.cs`
- [x] 在 `PackageValidator.cs` 暴露 `ValidateIosDistributionCompliance(...)`
- [x] 通过 `tests/contracts/managed/ios-hot-update-compliance-smoke/`
- [x] 通过 `python -m pytest tests/unit/run/test_phase8_mobile_foundation.py -v`

### Batch 2: mobile perf collector 与 worker

- [x] 新增 `build/toolchains/run/testing/mobile_perf_collector.py`
- [x] 在 `subject_workers.py` 新增 `run_mobile_native_perf()`
- [x] 注册 stage kind `mobile-native-perf`
- [x] 使用 `targetPlatform` 作为移动 perf baseline 平台
- [x] 通过 `python -m pytest tests/unit/run/test_subject_workers_perf.py -v`

### Batch 3: 移动端热更新入口与 benchmark manifest

- [x] 补齐 Android JNI `load/unload hot update package` 包装
- [x] 补齐 iOS bridge `load/unload hot update package` 包装
- [x] 在 `src/mobile/android/CMakeLists.txt` / `src/mobile/ios/CMakeLists.txt` 链接 `chaos_hot_update`
- [x] 为 4 个 benchmark subject 新增 `mobile-native-benchmark` pipeline
- [x] 为 4 个 benchmark subject 新增 `android-arm64-native-perf` / `ios-arm64-native-perf` matrix

### Batch 4: 回归验证与归档

- [x] 通过 `python -m pytest tests/unit/run/test_phase8_mobile_foundation.py -v`
- [x] 通过 `python -m pytest tests/unit/run/test_subject_workers_perf.py -v`
- [x] 通过 `python -m pytest tests/unit/run/test_subject_manifest_schema.py -v`
- [x] 通过 `python -m pytest tests/unit/run/test_subject_workers.py -v`
- [x] 通过 `python -m pytest tests/unit/run -v`（267 passed）
- [x] 更新 `STATUS.md` / `ACTIVE.md` / `INDEX.md`
- [x] 子任务归档到 `docs/dev/completed/20260412-04-phase-d-mobile-foundation/`

## 验证命令

- `python -m pytest tests/unit/run/test_phase8_mobile_foundation.py -v`
- `python -m pytest tests/unit/run/test_subject_workers_perf.py -v`
- `python -m pytest tests/unit/run/test_subject_manifest_schema.py -v`
- `python -m pytest tests/unit/run/test_subject_workers.py -v`
- `python -m pytest tests/unit/run -v`

## 结果边界

- 本计划已完成代码与测试闭环。
- 真正的 Android/iOS 设备采样、iOS Xcode 执行与分发审核证据仍留在父 roadmap 的外部阻塞范围内。
