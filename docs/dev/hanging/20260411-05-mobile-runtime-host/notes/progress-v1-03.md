# 进展记录 v1-03

## 时间

- 2026-04-11 14:42:36 +08:00

## 本轮完成

- 完成 Batch 4.1 `MobileHelloWorldProof`
  - 新增 `subjects/MobileHelloWorldProof/subject.manifest.json`
  - 新增 `subjects/MobileHelloWorldProof/source/MobileHelloWorldProof.csproj`
  - 新增 `subjects/MobileHelloWorldProof/source/Program.cs`
- 新增 subject-scoped mobile host root
  - 新增 `subjects/MobileHelloWorldProof/validation/mobile/android-host/*`
  - 新增 `subjects/MobileHelloWorldProof/validation/mobile/ios-host/*`
- root `CMakeLists.txt` 支持 subject-scoped mobile host route
  - 新增 `CHAOS_SUBJECT_ANDROID_HOST_ROOT`
  - 新增 `CHAOS_SUBJECT_IOS_HOST_ROOT`
  - Android/iOS preset route 改为读取上述 cache 变量
- `build/toolchains/run/testing/subject_workers.py` 完成 mobile route 接线
  - Android build-target 优先查找 subject-scoped `android-host`
  - 新增 iOS `targetPlatform == "ios-arm64"` validate-only build-target
  - iOS build-target 优先查找 subject-scoped `ios-host`
- 新增/更新测试
  - 新增 `tests/unit/run/test_phase4_mobile_hello_world_proof.py`
  - 更新 `tests/unit/run/test_subject_planner.py`
  - 更新 `tests/unit/run/test_subject_workers.py`

## 验证结果

- `python -m pytest tests/unit/run/test_phase4_mobile_hello_world_proof.py -v` -> 4 passed
- `python -m pytest tests/unit/run/test_subject_planner.py -k mobile_android_buildable -v` -> 1 passed
- `python -m pytest tests/unit/run/test_subject_workers.py -k "validate_only_build_uses_visual_studio_generator_and_records_non_repo_cmake_binary_dir or validate_only_build_supports_ios_target_and_subject_scoped_host_root" -v` -> 2 passed
- `python -m pytest tests/unit/run/test_phase4_mobile_runtime_host_batch1.py tests/unit/run/test_phase4_mobile_runtime_host_batch2_android.py tests/unit/run/test_phase4_mobile_runtime_host_batch3_ios.py tests/unit/run/test_phase4_mobile_hello_world_proof.py tests/unit/run/test_subject_manifest_schema.py tests/unit/run/test_subject_planner.py tests/unit/run/test_subject_workers.py -v` -> 50 passed
- `dotnet build subjects/MobileHelloWorldProof/source/MobileHelloWorldProof.csproj -c Release -o artifacts/.tmp-mobile-hello-world-proof` -> succeeded
- `dotnet artifacts/.tmp-mobile-hello-world-proof/MobileHelloWorldProof.dll` -> 输出 `MobileHelloWorldProof entry reached.`
- `cmake -S subjects/MobileHelloWorldProof/validation/mobile/android-host -B artifacts/.tmp-mobile-hello-world-proof-android -G "Visual Studio 17 2022"` -> configured
- `cmake --build artifacts/.tmp-mobile-hello-world-proof-android --config Release` -> built `mobile_hello_world_android_host.dll`
- `cmake -S . -B artifacts/.tmp-mobile-proof-android-route -G "Visual Studio 17 2022" -DROADMAP0_PRESET_TARGET=android-arm64-smoke -DROADMAP0_TOOLCHAIN_VALIDATE_ONLY=ON -DCHAOS_SUBJECT_ANDROID_HOST_ROOT=subjects/MobileHelloWorldProof/validation/mobile/android-host -DCMAKE_TOOLCHAIN_FILE=build/toolchains/android-arm64.cmake` -> configured
- `cmake --build artifacts/.tmp-mobile-proof-android-route --config Release` -> built validate-only route
- `cmake -S . -B artifacts/.tmp-mobile-proof-ios-route -G "Visual Studio 17 2022" -DROADMAP0_PRESET_TARGET=ios-arm64-packaging -DROADMAP0_TOOLCHAIN_VALIDATE_ONLY=ON -DCHAOS_SUBJECT_IOS_HOST_ROOT=subjects/MobileHelloWorldProof/validation/mobile/ios-host -DCMAKE_TOOLCHAIN_FILE=build/toolchains/ios-arm64.cmake` -> configured
- `cmake --build artifacts/.tmp-mobile-proof-ios-route --config Release` -> built validate-only route

## 备注

- 本轮把 mobile 证据从“generic gate shell”推进到了“subject-scoped buildable route”，但仍不是 Android/iOS 真 runtime 证据。
- Android 2.4 仍需要真实 NDK + startup runtime 环境。
- iOS 3.4+ 仍需要 macOS/Xcode 才能进入 simulator/device 级验证。

## 下一步

- 记录 Batch 2.4 / 3.4 的平台阻塞边界。
- 评估是否先切换到 `20260411-06-hot-update-skeleton` 继续推进父 roadmap。
