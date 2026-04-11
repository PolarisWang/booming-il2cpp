---
task_id: 20260411-05-mobile-runtime-host
title: Mobile Runtime Host
task_type: plan
lifecycle_status: hanging
phase: hanging
created_at: 2026-04-11 13:59:04 +08:00
updated_at: 2026-04-11 22:45:00 +08:00
current_dir: docs/dev/hanging/20260411-05-mobile-runtime-host
parent_task_id: 20260409-10-total-solution-and-ios-hot-update-analysis
source_task_id: 20260409-10-total-solution-and-ios-hot-update-analysis
source_relation: child_of_roadmap_phase_4
active: false
---

## 关键文档

- parent_roadmap: `docs/dev/in-progress/20260409-10-total-solution-and-ios-hot-update-analysis/roadmap-v1-01.md`
- phase_architecture: `docs/dev/in-progress/20260409-10-total-solution-and-ios-hot-update-analysis/phase-architecture-v1-01.md`
- brainstorm: `docs/dev/hanging/20260411-05-mobile-runtime-host/brainstorm-v1-04.md`
- design: `docs/dev/hanging/20260411-05-mobile-runtime-host/design-v1-04.md`
- plan: `docs/dev/hanging/20260411-05-mobile-runtime-host/plan-v1-01.md`
- latest_progress: `docs/dev/hanging/20260411-05-mobile-runtime-host/notes/progress-v1-05.md`

## 当前判断

- current_focus: Batch 1-4.1 已闭环；`src/mobile/shared/`、`src/mobile/android/`、`src/mobile/ios/` 与 `subjects/MobileHelloWorldProof/` 已形成 subject-scoped mobile buildable route，并新增 `run doctor` mobile runtime preflight 入口；但真实 Android/iOS runtime、crash 收集与 perf/package baseline 尚未完成。
- why_now: 当前宿主缺少可直接验证 Android startup/runtime 的 NDK/emulator/device，且没有 macOS/Xcode 可推进 iOS simulator/device；继续停留在本任务会把 validate-only/buildable 误报为 runtime 完成。
- done_definition: 保留 shared host、Android/iOS skeleton 与 `MobileHelloWorldProof` 作为 Phase 4 落点，待真实移动端环境具备后继续 Batch 2.4/2.5、3.4-3.7、4.2-4.4。

## 最近摘要

- 2026-04-11 22:45:00 +08:00: 扩展 public command `run doctor`，新增 Android SDK/NDK/adb/emulator 与 iOS host 缺口探测；当前 Windows 宿主可直接得到 mobile runtime host 恢复前的环境阻塞清单。
- 2026-04-11 13:59:04 +08:00: 激活任务；上游 `20260411-04-engine-binding-contract` 已归档，`EngineHostProof` 与 `HostEmbeddingLite` ownership proof 已闭环，开始推进 Phase 4 mobile host。
- 2026-04-11 14:22:48 +08:00: 完成 Batch 1 shared host framework、Batch 2 Android skeleton、Batch 3 iOS skeleton；`src/mobile/shared/*`、`src/mobile/android/*`、`src/mobile/ios/*` 已落地，并通过 9 项 Phase 4 相关 pytest 与 Android shared host 桌面编译验证。
- 2026-04-11 14:42:36 +08:00: 完成 Batch 4.1 `MobileHelloWorldProof`；subject-scoped Android/iOS mobile host root、root CMake route 与 iOS validate-only worker 已接通，并通过 50 项相关 pytest、managed runtime、Android host 桌面编译以及 Android/iOS validate-only route 验证。
- 2026-04-11 15:00:53 +08:00: 明确记录真实移动端验证阻塞边界并挂起任务；主线切换到 `20260411-06-hot-update-skeleton`，以 desktop 可执行路径继续推进父 roadmap。

## 下一步

- next_action: 先在目标宿主运行 `run doctor` 补齐 Android SDK/NDK/adb/emulator 缺口，并准备 macOS/Xcode 宿主；环境到位后恢复本任务，继续 Batch 2.4/2.5、3.4-3.7、4.2-4.4。
- owner: codex
- trigger: 提供真实 Android runtime 验证环境或 macOS/Xcode 验证环境。

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

## 风险 / 阻塞

### risks

- 当前 Windows 宿主只能证明 buildable route 与最小桌面编译，不能把 validate-only 或桌面替代编译误报为 Android/iOS runtime 完成。
- root preset 现在支持 subject-scoped mobile host root，但后续若切换到 `src/mobile/android` / `src/mobile/ios` 作为正式入口，仍需继续分批收敛，避免 routing 与 runtime 证据混在同一轮。
- `src/mobile/` 必须保持为 mobile host glue 层，不能把 JNI / ObjC 平台细节反向塞回 `runtime-core` 或 `engine-bridge`。

### blockers

- 缺少已配置的 Android NDK / emulator / device，无法在当前宿主直接推进 Batch 2.4 真 startup/runtime 证据。
- 当前宿主为 Windows，无法直接推进 Batch 3.4+ 所需的 macOS/Xcode simulator/device 验证；`run doctor` 现已明确输出这一阻塞。
