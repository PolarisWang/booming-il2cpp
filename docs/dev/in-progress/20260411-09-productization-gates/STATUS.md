---
task_id: 20260411-09-productization-gates
title: Productization Gates
task_type: plan
lifecycle_status: in_progress
phase: executing
created_at: 2026-04-11 21:10:00 +08:00
updated_at: 2026-04-11 22:52:01 +08:00
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

- current_focus: Phase 8 本地实现批次继续收口；本轮在修复 `Batch 5.3` perf 治理缺口后，又用 `soak_harness.py` 对 `MobileHelloWorldProof` 的 `windows-android-runtime` 做了一次真实 Android soak pilot，确认 harness 可以包住真实链路并产出报告，但同时暴露出当前 Android runtime binary 仅执行一次 `il2cpp_host_run()` 即退出，尚缺可信的长跑 mobile soak entry/driver 来支撑 `Batch 4.3 / 5.2` 的“连续 4h/24h”语义。
- why_now: 之前 productization 的真实平台验证主要卡在 Windows 主机缺少 Android host tooling 与 Java；补齐 repo-cache bootstrap 之后，Android 侧后续可以直接在仓库内复用环境，iOS 则明确只在 macOS 主机验证，不再让 Windows `doctor` 误报。
- done_definition: 本轮切片的完成标准是修复 perf goal 默认选择与 perf baseline 比对缺口，补齐对应回归测试，并重新实跑 perf dashboard 覆盖到的 4 个真实 perf 条目，确认 `InterfaceDispatchProof` 真实输出 dispatch 指标且本地 `Batch 5.3 perf baseline 无回归` 证据不再出现静默误判。

## 最近摘要

- 2026-04-11 22:52:01 +08:00: 执行 Android soak pilot：用 `build/toolchains/run/testing/soak_harness.py` 包裹 `python build/toolchains/run/run.py test subject --id subject/MobileHelloWorldProof --goal correctness.platform --matrix windows-android-runtime`，成功产出 `artifacts/productization-gates/20260411-225121/android-soak-pilot.json`，单次真实 Android run 耗时约 `40.594s`、`18` 个采样点、`0 crash`。同时核对 `subjects/MobileHelloWorldProof/validation/mobile/android-host/mobile_runtime_main.cpp` 与 `src/mobile/shared/host_main.cpp` 后确认：当前 Android runtime binary 只执行一次 `il2cpp_host_run()` 就退出，现有 soak harness 也仅是“循环执行短命命令”的模型，因此 `Batch 4.3 / 5.2` 仍需要先明确并实现可信的长跑 mobile soak entry/driver，不能直接把当前几十秒命令等同于 4h/24h 连续 soak。
- 2026-04-11 22:43:27 +08:00: 修复 `build/toolchains/run/testing/subject_planner.py` 与 `subjects.py` 中 perf goal 默认选择逻辑，改为先按 goal/matrix 选中 perf intent，再推导 `validationProfileId` / `validationKind` / `variant`；同时修复 `build/toolchains/run/testing/perf.py`，当 baseline 指标在实际 evidence 中缺失时改为标记 `regressed` 而不是静默 `ok`。新增 planner / perf policy / command 回归测试，并执行 `python -m pytest tests/unit/run tests/tooling/run -q`（406 passed, 34 skipped）。随后真实复跑 `GenericEcho` (`perf.dev` / `perf.release`)、`InterfaceDispatchProof` (`perf.profile`) 与 `MainlineFeaturePack` (`perf.profile`) 四个 perf 条目，全部 `regressionStatus=ok`；其中 `InterfaceDispatchProof` 已正确落盘 `meanDispatchNanoseconds=2.068`，汇总产物见 `artifacts/productization-gates/20260411-224327/perf-dashboard.json` 与 `artifacts/productization-gates/20260411-224327/perf-runs-summary.json`。
- 2026-04-11 22:20:28 +08:00: 修复 `MobileHelloWorldProof` lowering 入口形状，并把 Windows Android repo-cache bootstrap 改为按主机架构选择 emulator system image；在当前 Windows x86_64 主机上重新 `clean --scope android-host` / `prepare android-host` / `doctor` 后，repo cache 成功切到 `x86_64` system image + `chaos-android-36-x86_64` AVD，随后 `python build/toolchains/run/run.py test subject --id subject/MobileHelloWorldProof --goal correctness.platform --matrix windows-android-runtime` 真实通过。
- 2026-04-11 21:07:28 +08:00: 进一步补齐 Android bootstrap 下载头与 PATH 兼容性修复，并把 `doctor` 的 Windows blocker 测试改为临时 repo root，避免真实 repo cache 污染单测；`tests/tooling/run` 全套 160 项通过，确认本轮 Android host 改动未破坏同子系统其他入口。
- 2026-04-11 21:04:03 +08:00: 补齐 `prepare android-host` / `clean --scope android-host`、repo-cache Android 环境注入、doctor repo-cache 识别与 Windows 跳过 iOS host 检查；新增 Android bootstrap 回归测试并通过 55 项 tooling/run 相关验证，随后在当前 Windows 主机真实执行 `prepare android-host` 与 `doctor`，确认 repo cache 下的 JDK 17、adb 与 emulator 可用。
- 2026-04-11 22:19:33 +08:00: 补齐 Batch 2.5 验证闭环；执行 `test_subject_*`、`test_phase5_hot_update_skeleton.py` 与 `test_phase8_productization_gates.py` 共 68 项回归全部通过，确认 rollback 不会破坏现有 subject 测试面。
- 2026-04-11 21:45:00 +08:00: 完成 Batch 2 最小闭环：`RuntimeManager` 新增 `Rollback()`、`GetActivePatches()`、`ValidateIntegrity()` / `HotUpdateIntegrityReport`，并补 `release-checklist.md`。
- 2026-04-11 21:22:00 +08:00: 完成 Batch 3 最小切片：新增 iOS distribution policy 与 version compatibility matrix 文档，`PackageValidator` 升级为 `major.minor` compatibility band 校验。
- 2026-04-11 20:12:32 +08:00: 完成 Batch 4 本地 skeleton：新增 `perf_dashboard.py`、`unsupported_feature_report.py` 与 `soak_harness.py` 三个入口。
- 2026-04-11 19:59:47 +08:00: 完成 Batch 1 最小闭环：新增 `subjects/CompatibilityMatrixProof/`、compatibility matrix JSON config、`compatibility_matrix_runner.py` 与 JSON report skeleton。

## 下一步

- next_action: 先为 Android mobile runtime 明确并落地可信的长跑 soak entry/driver（避免把当前几十秒短命 command 当成连续 soak），随后再推进 4h/24h Android soak；iOS 仍只在 macOS 主机上验证，并在外部环境具备后推进 full compatibility matrix 与 release checklist 的最终复跑。
- owner: codex
- trigger: 本地 perf governance 缺口已关闭，后续推进主要依赖 Android 长时运行资源与 macOS/Xcode 资源，而不再受本机 SDK/NDK/adb/emulator 或 perf 默认选择错误阻塞。

## 验证

- passed: `python - <<PY ... soak_harness.run_soak_harness([... 'subject/MobileHelloWorldProof' ... 'windows-android-runtime'], duration_seconds=900, poll_interval_seconds=2.0, max_iterations=1) ... PY`
- passed: `python -m pytest tests/unit/run/test_subject_planner.py tests/unit/run/test_phase2_perf_governance.py tests/unit/run/test_subject_perf_policy.py tests/unit/run/test_subject_workers_perf.py tests/tooling/run/test_subject_command.py tests/unit/run/test_phase8_productization_gates.py -v`
- passed: `python -m pytest tests/unit/run tests/tooling/run -q`
- passed: `python build/toolchains/run/run.py test subject --id subject/InterfaceDispatchProof --goal perf.profile`
- passed: `python build/toolchains/run/run.py test subject --id subject/MainlineFeaturePack --goal perf.profile`
- passed: `python build/toolchains/run/run.py test subject --id subject/GenericEcho --goal perf.dev`
- passed: `python build/toolchains/run/run.py test subject --id subject/GenericEcho --goal perf.release`
- passed: `python -m pytest tests/tooling/run/test_doctor.py tests/tooling/run/test_prepare_scopes.py tests/tooling/run/test_command_manifest.py tests/tooling/run/test_tui.py tests/tooling/run/test_android_bootstrap.py -v`
- passed: `python -m pytest tests/tooling/run -v`
- passed: `python build/toolchains/run/run.py clean --scope android-host`
- passed: `python build/toolchains/run/run.py prepare android-host`
- passed: `python build/toolchains/run/run.py doctor`
- passed: `python build/toolchains/run/run.py test subject --id subject/MobileHelloWorldProof --goal correctness.platform --matrix windows-android-runtime`
- passed: `python -m pytest tests/tooling/run/test_android_bootstrap.py tests/tooling/run/test_doctor.py tests/unit/run/test_phase4_mobile_hello_world_proof.py tests/unit/run/test_subject_workers.py -v`
- passed: `python -m pytest tests/unit/run/test_subject_manifest_schema.py tests/unit/run/test_subject_executor.py tests/unit/run/test_subject_planner.py tests/unit/run/test_subject_workers.py tests/unit/run/test_phase4_mobile_hello_world_proof.py tests/unit/run/test_phase8_productization_gates.py -v`
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

- Android host tooling 已恢复，且 `windows-android-runtime` 已闭环通过，但 Batch 4/5 的真实结论仍依赖 emulator / device 上的长时间运行证据，而不是单纯的单次 smoke 通过。
- Windows x86_64 主机必须使用 `x86_64` Android emulator system image；若 repo cache 回退到 `arm64-v8a` image，emulator 会在 adb 暴露 serial 之前直接退出。
- Windows 平台现已明确跳过 iOS host 校验；iOS runtime evidence 仍必须在 macOS + Xcode + Simulator 或签名设备上采集。
- `perf_dashboard.py`、`unsupported_feature_report.py` 与 `soak_harness.py` 仍是 repo-local skeleton，后续还需要真实 CI / mobile evidence 持续回灌；不过 `Batch 5.3` 本地 perf 选择与 baseline 比对逻辑已补到可可信状态。

### blockers

- Batch 4 的 24h soak acceptance 与 perf dashboard 最终验收，除了 Android 长时运行窗口外，还需要一个可信的 Android 长跑 soak entry/driver；当前 `MobileHelloWorldProof` runtime binary 是几十秒内退出的短命命令。
- Batch 5 的 full compatibility matrix、24h soak、all-subject x all-platform perf baseline 仍依赖上述 Android / macOS 外部环境恢复。
- iOS runtime host 证据仍未收集；这部分当前只在 macOS 平台推进。

## Wiki 决策

- 本轮长期知识继续沉淀在 `build/toolchains/run/*`、`docs/architecture/*.md` 与任务目录 `notes/` 中，暂不额外写入 `wiki/`。
