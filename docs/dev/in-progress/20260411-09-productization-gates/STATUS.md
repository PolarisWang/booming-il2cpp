---
task_id: 20260411-09-productization-gates
title: Productization Gates
task_type: plan
lifecycle_status: in_progress
phase: executing
created_at: 2026-04-11 21:10:00 +08:00
updated_at: 2026-04-11 21:07:28 +08:00
current_dir: docs/dev/in-progress/20260411-09-productization-gates
parent_task_id: 20260409-10-total-solution-and-ios-hot-update-analysis
source_task_id: 20260409-10-total-solution-and-ios-hot-update-analysis
source_relation: child_of_roadmap_phase_8
active: true
---

## 关键文档

- parent_roadmap: `docs/dev/in-progress/20260409-10-total-solution-and-ios-hot-update-analysis/roadmap-v1-01.md`
- plan: `docs/dev/in-progress/20260411-09-productization-gates/plan-v1-01.md`
- brainstorm: `docs/dev/in-progress/20260411-09-productization-gates/brainstorm-v1-01.md`
- design: `docs/dev/in-progress/20260411-09-productization-gates/design-v1-01.md`

## 当前判断

- current_focus: Phase 8 本地实现批次已经闭环，本轮继续消减 Phase 4 外部阻塞；Windows 侧 Android SDK / NDK / adb / emulator 与 JDK 已落到 repo cache，并接入 `run prepare android-host`、`run clean --scope android-host`、`run doctor` 与入口菜单。
- why_now: 之前 productization 的真实平台验证主要卡在 Windows 主机缺少 Android host tooling 与 Java；补齐 repo-cache bootstrap 之后，Android 侧后续可以直接在仓库内复用环境，iOS 则明确只在 macOS 主机验证，不再让 Windows `doctor` 误报。
- done_definition: 本轮切片的完成标准是 Android host tooling 自动化可执行、`doctor` 能识别 repo cache、入口菜单可见、Windows 平台不再要求 iOS host，并且有真实主机 bootstrap / doctor / version probe 证据。

## 最近摘要

- 2026-04-11 21:07:28 +08:00: 进一步补齐 Android bootstrap 下载头与 PATH 兼容性修复，并把 `doctor` 的 Windows blocker 测试改为临时 repo root，避免真实 repo cache 污染单测；`tests/tooling/run` 全套 160 项通过，确认本轮 Android host 改动未破坏同子系统其他入口。
- 2026-04-11 21:04:03 +08:00: 补齐 `prepare android-host` / `clean --scope android-host`、repo-cache Android 环境注入、doctor repo-cache 识别与 Windows 跳过 iOS host 检查；新增 Android bootstrap 回归测试并通过 55 项 tooling/run 相关验证，随后在当前 Windows 主机真实执行 `prepare android-host` 与 `doctor`，确认 repo cache 下的 JDK 17、adb 与 emulator 可用。
- 2026-04-11 22:19:33 +08:00: 补齐 Batch 2.5 验证闭环；执行 `test_subject_*`、`test_phase5_hot_update_skeleton.py` 与 `test_phase8_productization_gates.py` 共 68 项回归全部通过，确认 rollback 不会破坏现有 subject 测试面。
- 2026-04-11 21:45:00 +08:00: 完成 Batch 2 最小闭环：`RuntimeManager` 新增 `Rollback()`、`GetActivePatches()`、`ValidateIntegrity()` / `HotUpdateIntegrityReport`，并补 `release-checklist.md`。
- 2026-04-11 21:22:00 +08:00: 完成 Batch 3 最小切片：新增 iOS distribution policy 与 version compatibility matrix 文档，`PackageValidator` 升级为 `major.minor` compatibility band 校验。
- 2026-04-11 20:12:32 +08:00: 完成 Batch 4 本地 skeleton：新增 `perf_dashboard.py`、`unsupported_feature_report.py` 与 `soak_harness.py` 三个入口。
- 2026-04-11 19:59:47 +08:00: 完成 Batch 1 最小闭环：新增 `subjects/CompatibilityMatrixProof/`、compatibility matrix JSON config、`compatibility_matrix_runner.py` 与 JSON report skeleton。

## 下一步

- next_action: 继续在有可用 Android emulator / device 的前提下收集真实 Android perf / soak evidence；iOS 只在 macOS 主机上验证，并推进 full compatibility matrix、24h soak 与 release checklist 的最终复跑。
- owner: codex
- trigger: Windows Android host repo-cache 已恢复，后续推进主要依赖 Android 运行资源与 macOS/Xcode 资源，而不再受本机 SDK/NDK/adb/emulator 缺失阻塞。

## 验证

- passed: `python -m pytest tests/tooling/run/test_doctor.py tests/tooling/run/test_prepare_scopes.py tests/tooling/run/test_command_manifest.py tests/tooling/run/test_tui.py tests/tooling/run/test_android_bootstrap.py -v`
- passed: `python -m pytest tests/tooling/run -v`
- passed: `python build/toolchains/run/run.py clean --scope android-host`
- passed: `python build/toolchains/run/run.py prepare android-host`
- passed: `python build/toolchains/run/run.py doctor`
- passed: `artifacts/toolchains/android/jdk/bin/java.exe -version`
- passed: `artifacts/toolchains/android/sdk/platform-tools/adb.exe version`
- passed: `artifacts/toolchains/android/sdk/emulator/emulator.exe -version`
- passed: `python -m pytest tests/unit/run/test_phase8_productization_gates.py -v`
- passed: `python -m pytest tests/unit/run/test_subject_manifest_schema.py -v`
- passed: `python -m pytest tests/unit/run/test_subject_manifest_schema.py tests/unit/run/test_subject_contracts_source_cutover.py tests/unit/run/test_subject_executor.py tests/unit/run/test_subject_perf_policy.py tests/unit/run/test_subject_planner.py tests/unit/run/test_subject_reporting.py tests/unit/run/test_subject_workers.py tests/unit/run/test_subject_workers_perf.py tests/unit/run/test_phase5_hot_update_skeleton.py tests/unit/run/test_phase8_productization_gates.py -v`
- passed: `dotnet run --project subjects/CompatibilityMatrixProof/source/CompatibilityMatrixProof.csproj`
- passed: `python -m pytest tests/unit/run/test_phase5_hot_update_skeleton.py -v`
- passed: `dotnet build src/managed/Chaos.IL2CPP.HotUpdate/Chaos.IL2CPP.HotUpdate.csproj -c Release`

## 风险 / 阻塞

### risks

- Android host tooling 已恢复，但 Batch 4/5 的真实结论仍依赖 emulator / device 上的长时间运行证据，而不是单纯的 SDK 安装完成。
- Windows 平台现已明确跳过 iOS host 校验；iOS runtime evidence 仍必须在 macOS + Xcode + Simulator 或签名设备上采集。
- `perf_dashboard.py`、`unsupported_feature_report.py` 与 `soak_harness.py` 仍是 repo-local skeleton，后续还需要真实 CI / mobile evidence 持续回灌。

### blockers

- Batch 4 的 24h soak acceptance 与 perf dashboard 最终验收，仍需要更完整的 Android 运行资源与长时运行窗口。
- Batch 5 的 full compatibility matrix、24h soak、all-subject x all-platform perf baseline 仍依赖上述 Android / macOS 外部环境恢复。
- iOS runtime host 证据仍未收集；这部分当前只在 macOS 平台推进。

## Wiki 决策

- 本轮长期知识继续沉淀在 `build/toolchains/run/*`、`docs/architecture/*.md` 与任务目录 `notes/` 中，暂不额外写入 `wiki/`。
