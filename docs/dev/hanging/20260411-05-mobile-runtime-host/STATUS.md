---
task_id: 20260411-05-mobile-runtime-host
title: Mobile Runtime Host
task_type: plan
lifecycle_status: hanging
phase: hanging
created_at: 2026-04-11 13:59:04 +08:00
updated_at: 2026-04-12 00:04:39 +08:00
current_dir: docs/dev/hanging/20260411-05-mobile-runtime-host
parent_task_id: 20260409-10-total-solution-and-ios-hot-update-analysis
source_task_id: 20260409-10-total-solution-and-ios-hot-update-analysis
source_relation: child_of_roadmap_phase_4
active: false
---

## 关键文档

- parent_roadmap: `docs/archive/dev-completed/20260409-10-total-solution-and-ios-hot-update-analysis/roadmap-v1-01.md`
- phase_architecture: `docs/archive/dev-completed/20260409-10-total-solution-and-ios-hot-update-analysis/phase-architecture-v1-01.md`
- brainstorm: `docs/dev/hanging/20260411-05-mobile-runtime-host/brainstorm-v1-04.md`
- design: `docs/dev/hanging/20260411-05-mobile-runtime-host/design-v1-04.md`
- plan: `docs/dev/hanging/20260411-05-mobile-runtime-host/plan-v1-01.md`
- latest_progress: `docs/dev/hanging/20260411-05-mobile-runtime-host/notes/progress-v1-06.md`

## 当前判断

- current_focus: Windows/Android 侧现在已经补齐 `MobileHelloWorldProof` 的 runtime host baseline 证据；本 hanging 任务剩余的是 `macOS + Xcode` iOS runtime host，以及 crash/perf/package 这类外部环境 follow-up。
- why_now: 用户已明确要求跳过 `macOS/iOS runtime host evidence`，因此父 roadmap 已可归档；本任务保留为外部环境 follow-up，而不再作为当前主线 blocker。
- done_definition: 继续保持 hanging；如未来需要无 waiver 的 mobile 发布证据，再恢复 iOS runtime host、crash 收集与 mobile perf/package baseline。

## 最近摘要

- 2026-04-12 00:04:39 +08:00: `windows-android-soak` 已提供真实 Android runtime host baseline；用户同时明确跳过 `macOS/iOS runtime host evidence`，因此本任务继续保留为 hanging external follow-up，不再阻塞父 roadmap 归档。
- 2026-04-11 22:45:00 +08:00: 扩展 public command `run doctor`，新增 Android SDK/NDK/adb/emulator 与 iOS host 缺口探测；当前 Windows 宿主可直接得到 mobile runtime host 恢复前的环境阻塞清单。
- 2026-04-11 13:59:04 +08:00: 激活任务；上游 `20260411-04-engine-binding-contract` 已归档，`EngineHostProof` 与 `HostEmbeddingLite` ownership proof 已闭环，开始推进 Phase 4 mobile host。
- 2026-04-11 14:22:48 +08:00: 完成 Batch 1 shared host framework、Batch 2 Android skeleton、Batch 3 iOS skeleton，`src/mobile/shared/*`、`src/mobile/android/*`、`src/mobile/ios/*` 已落地，并通过 9 项 Phase 4 相关 pytest 与 Android shared host 桌面编译验证。
- 2026-04-11 14:42:36 +08:00: 完成 Batch 4.1 `MobileHelloWorldProof`；subject-scoped Android/iOS mobile host root、root CMake route 与 iOS validate-only worker 已接通，并通过 50 项相关 pytest、managed runtime、Android host 桌面编译以及 Android/iOS validate-only route 验证。
- 2026-04-11 15:00:53 +08:00: 明确记录真实移动端验证阻塞边界并挂起任务；主线切换到 `20260411-06-hot-update-skeleton`，以 desktop 可执行路径继续推进父 roadmap。

## 下一步

- next_action: 如后续需要无 waiver 的 mobile 发布证据，准备 `macOS + Xcode` 宿主后恢复本任务，继续 Batch 3.4-3.7、4.2-4.4，并按需补齐 crash/perf/package baseline。
- owner: codex
- trigger: 提供 `macOS + Xcode` 验证环境，或要求把 mobile proof 提升到无 waiver 的正式发布证据。

## 验证

- passed: `python -m pytest tests/unit/run/test_phase4_mobile_runtime_host_batch1.py -v`
- passed: `python -m pytest tests/unit/run/test_phase4_mobile_runtime_host_batch2_android.py -v`
- passed: `python -m pytest tests/unit/run/test_phase4_mobile_runtime_host_batch3_ios.py -v`
- passed: `python -m pytest tests/unit/run/test_phase4_mobile_hello_world_proof.py -v`
- passed: `python -m pytest tests/unit/run/test_subject_planner.py -k mobile_android_buildable -v`
- passed: `python -m pytest tests/unit/run/test_subject_workers.py -k "validate_only_build_uses_visual_studio_generator_and_records_non_repo_cmake_binary_dir or validate_only_build_supports_ios_target_and_subject_scoped_host_root" -v`
- passed: `python -m pytest tests/unit/run/test_phase4_mobile_runtime_host_batch1.py tests/unit/run/test_phase4_mobile_runtime_host_batch2_android.py tests/unit/run/test_phase4_mobile_runtime_host_batch3_ios.py tests/unit/run/test_phase4_mobile_hello_world_proof.py tests/unit/run/test_subject_manifest_schema.py tests/unit/run/test_subject_planner.py tests/unit/run/test_subject_workers.py -v`
- passed: `dotnet build subjects/MobileHelloWorldProof/source/MobileHelloWorldProof.csproj -c Release -o artifacts/.tmp-mobile-hello-world-proof`
- passed: `dotnet artifacts/.tmp-mobile-hello-world-proof/MobileHelloWorldProof.dll`
- passed: `cmake -S subjects/MobileHelloWorldProof/validation/mobile/android-host -B artifacts/.tmp-mobile-hello-world-proof-android -G "Visual Studio 17 2022"`
- passed: `cmake --build artifacts/.tmp-mobile-hello-world-proof-android --config Release`
- passed: `cmake -S . -B artifacts/.tmp-mobile-proof-android-route -G "Visual Studio 17 2022" -DROADMAP0_PRESET_TARGET=android-arm64-smoke -DROADMAP0_TOOLCHAIN_VALIDATE_ONLY=ON -DCHAOS_SUBJECT_ANDROID_HOST_ROOT=subjects/MobileHelloWorldProof/validation/mobile/android-host -DCMAKE_TOOLCHAIN_FILE=build/toolchains/android-arm64.cmake`
- passed: `cmake --build artifacts/.tmp-mobile-proof-android-route --config Release`
- passed: `cmake -S . -B artifacts/.tmp-mobile-proof-ios-route -G "Visual Studio 17 2022" -DROADMAP0_PRESET_TARGET=ios-arm64-packaging -DROADMAP0_TOOLCHAIN_VALIDATE_ONLY=ON -DCHAOS_SUBJECT_IOS_HOST_ROOT=subjects/MobileHelloWorldProof/validation/mobile/ios-host -DCMAKE_TOOLCHAIN_FILE=build/toolchains/ios-arm64.cmake`
- passed: `cmake --build artifacts/.tmp-mobile-proof-ios-route --config Release`
- passed: `python -m pytest tests/tooling/run/test_doctor.py -v`
- passed: `python -m pytest tests/tooling/run/test_prepare_scopes.py -v`
- passed: `python build/toolchains/run/run.py doctor`
- passed: `python build/toolchains/run/run.py test subject --id subject/MobileHelloWorldProof --goal correctness.platform --matrix windows-android-soak`
- accepted: `python build/toolchains/run/run.py test subject --id subject/MobileHelloWorldProof --goal correctness.platform --matrix windows-android-soak-4h` (`20260411-233957-windows-ce2b`) was user-aborted after entering `runtime-observe`; no fail event was observed before termination

## 风险 / 阻塞

### risks

- 当前 iOS runtime host 仍没有真实 `macOS + Xcode` 证据；如果未来目标升级为无 waiver 的移动端发布证明，仍需恢复本任务继续补齐。
- root preset 现在支持 subject-scoped mobile host root，但后续若切换到 `src/mobile/android` / `src/mobile/ios` 作为正式入口，仍需继续分批收敛，避免 routing 与 runtime 证据混在同一轮。
- `src/mobile/` 必须保持为 mobile host glue 层，不能把 JNI / ObjC 平台细节反向塞回 `runtime-core` 或 `engine-bridge`。

### blockers

- 当前宿主为 Windows，无法直接推进 Batch 3.4+ 所需的 `macOS + Xcode` simulator/device 验证，`run doctor` 已明确输出这一阻塞。
