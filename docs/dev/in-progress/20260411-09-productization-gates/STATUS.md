---
task_id: 20260411-09-productization-gates
title: Productization Gates
task_type: plan
lifecycle_status: in_progress
phase: executing
created_at: 2026-04-11 21:10:00 +08:00
updated_at: 2026-04-11 23:29:40 +08:00
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

- current_focus: `Batch 5.3` perf 治理缺口已关闭，本轮继续把 Android mobile soak 从“短命令被 harness 包裹”升级为“单次连续长会话可验证入口”，并确认 Windows 平台只验证 Android，iOS 仍只在 macOS 验证。
- why_now: 之前的 `android-soak-pilot.json` 只证明 `soak_harness.py` 能包住真实 `windows-android-runtime` 命令，但 runtime binary 本身会立即退出，不能代表 `Batch 4.3 / 5.2` 所需的连续 4h/24h soak 语义。
- done_definition: 本切片的完成标准是为 `MobileHelloWorldProof` 补齐可配置的 Android 长时运行入口、把运行参数可靠传进 adb/Android 进程、拿到一次连续 300 秒真实 soak artifact，并完成 `tests/unit/run` + `tests/tooling/run` 全量回归。

## 最近摘要

- 2026-04-11 23:43:48 +08:00: 已把 Android soak 入口进一步正式化为 `windows-android-soak-4h` 与 `windows-android-soak-24h` 两个矩阵，分别固定 `14400s/60s heartbeat` 与 `86400s/300s heartbeat`；对应 manifest surface 回归已通过。与此同时，已启动真实 4h Android soak：命令 `python build/toolchains/run/run.py test subject --id subject/MobileHelloWorldProof --goal correctness.platform --matrix windows-android-soak-4h`，run id `20260411-233957-windows-ce2b`，后台 PID `37860`，launch 元数据位于 `artifacts/productization-gates/20260411-233956/android-soak-4h.launch.json`。当前 event stream 已推进到 `runtime-observe` 阶段并保持运行。
- 2026-04-11 23:29:40 +08:00: 已为 `MobileHelloWorldProof` 新增显式 `windows-android-soak` 矩阵，并把 shared mobile host 升级为支持 `--soak-duration-seconds` / `--heartbeat-interval-seconds` / `--subject-id` 及对应环境变量；`subject_workers.py` 新增 Android runtime 参数到 adb shell 的 env bridge，解决 adb 下 argv 透传不稳定问题。真实执行 `python build/toolchains/run/run.py test subject --id subject/MobileHelloWorldProof --goal correctness.platform --matrix windows-android-soak` 后，run id `20260411-232036-windows-275c` 连续运行约 300 秒，产出 `10` 个 heartbeat 与 `shared-host-soak-complete` 证据，见 `artifacts/subjects/MobileHelloWorldProof/runs/20260411-232036-windows-275c/matrices/windows-android-soak/runtime/runtime.manifest.json` 与同目录 `stdout.log`。
- 2026-04-11 23:28:00 +08:00: 执行 `python -m pytest tests/unit/run tests/tooling/run -q`，结果 `408 passed, 34 skipped`；其中新增 Android soak 入口相关回归覆盖 manifest、shared host 参数解析、adb runtime 参数桥接与 MobileHelloWorldProof subject surface。
- 2026-04-11 22:52:01 +08:00: 旧版 `android-soak-pilot.json` 证明现有 `soak_harness.py` 只能包住一次真实 `windows-android-runtime` 命令；该 pilot 的价值保留为“harness 能工作”，但不再被视为“连续 mobile soak 入口已成立”的证据。
- 2026-04-11 22:43:27 +08:00: 已修复 `perf.profile` 默认 validation/profile/variant 选择与 perf baseline 缺失指标误判问题；`GenericEcho`、`InterfaceDispatchProof`、`MainlineFeaturePack` 的真实 perf 证据与 dashboard 汇总已重新跑通。
- 2026-04-11 22:20:28 +08:00: 已修复 Windows Android emulator ABI 选择，repo cache 成功落到 `x86_64` system image + `chaos-android-36-x86_64` AVD，`windows-android-runtime` 真实 subject 路径已可执行。

## 下一步

- next_action: 观察并等待正在执行的 `windows-android-soak-4h`（run id `20260411-233957-windows-ce2b`）完成，收集内存增长 / crash / heartbeat 证据；如 4h 结果成立，再推进 24h soak 与 macOS/iOS 外部环境项。
- owner: codex
- trigger: 本地 Android 长时 soak 入口已落地，当前主阻塞已从“代码/工具链缺口”切换为“长时间运行窗口”和“macOS/Xcode 外部环境”。

## 验证

- passed: `python -m pytest tests/unit/run/test_phase4_mobile_hello_world_proof.py tests/unit/run/test_phase4_mobile_runtime_host_batch1.py tests/unit/run/test_subject_workers.py tests/unit/run/test_subject_planner.py tests/tooling/run/test_subject_command.py -q`
- running: `python build/toolchains/run/run.py test subject --id subject/MobileHelloWorldProof --goal correctness.platform --matrix windows-android-soak-4h` (run id `20260411-233957-windows-ce2b`, PID `37860`)
- passed: `python -m pytest tests/unit/run/test_phase4_mobile_runtime_host_batch1.py tests/unit/run/test_phase4_mobile_hello_world_proof.py tests/unit/run/test_subject_workers.py -q`
- passed: `python -m pytest tests/unit/run tests/tooling/run -q` (`408 passed, 34 skipped`)
- passed: `python build/toolchains/run/run.py test subject --id subject/MobileHelloWorldProof --goal correctness.platform --matrix windows-android-soak`
- passed: 手工 5 秒 adb probe（使用 `subject_workers._run_android_binary_via_adb(... runtime_arguments=['--soak-duration-seconds=5', '--heartbeat-interval-seconds=1', '--subject-id=Probe'])`）确认 Android 进程连续输出 heartbeat 与 `shared-host-soak-complete`
- passed: `python build/toolchains/run/run.py test subject --id subject/InterfaceDispatchProof --goal perf.profile`
- passed: `python build/toolchains/run/run.py test subject --id subject/MainlineFeaturePack --goal perf.profile`
- passed: `python build/toolchains/run/run.py test subject --id subject/GenericEcho --goal perf.dev`
- passed: `python build/toolchains/run/run.py test subject --id subject/GenericEcho --goal perf.release`

## 风险 / 阻塞

### risks

- `windows-android-soak` 现在已经证明“单次连续长会话”成立，但 `Batch 4.3` 的正式验收仍需要真实 `4h/24h` 窗口下的内存增长 / crash 证据，而不是当前 300 秒 pilot。
- 当前 soak 证据仍来自 Windows + Android emulator；后续如要提升可信度，仍应考虑真实设备或持续回灌到 CI。
- iOS 仍明确只在 macOS 平台验证；Windows 侧不再尝试伪造 iOS 运行期证据。

### blockers

- `Batch 4.3` / `Batch 5.2` 仍受限于 wall-clock 时间窗口；当前 4h soak 已启动，但需要自然完成后才能关闭该阻塞。
- `Batch 5` 里的 full compatibility matrix 与 release checklist 最终复跑，仍依赖 macOS/Xcode 资源来补齐 iOS runtime host 证据。

## Wiki 决策

- 本轮长期知识仍沉淀在 `build/toolchains/run/*`、`subjects/MobileHelloWorldProof/*` 与当前任务 `notes/` 中，暂不单独写入 `wiki/`。
