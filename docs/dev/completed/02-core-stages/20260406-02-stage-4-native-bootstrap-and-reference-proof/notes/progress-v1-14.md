# 进度记录 v1-14

## 时间

- 2026-04-06 12:29:22 +08:00

## 已完成

- 修复了 `contracts/native/v0/codegen_bridge.h` 漏掉 `CHAOS_BRIDGE_STATUS_NOT_SUPPORTED` 的契约缺口，并补上 compile-only smoke 与单测守卫，消除了 `bootstrap.cpp` 的真实 MSVC 编译失败。
- 为 `runtime ABI` 静态库链路补齐 `CHAOS_RUNTIME_ABI_STATIC` 分支与 CMake 传播，消除了 `chaos_runtime_get_abi_v0` / `chaos_codegen_get_bridge_v0` 被误当成 `dllimport` 的真实链接失败。
- 在 `windows-x64-reference-real` 上完成了 Stage 4 `chaos_stage4_hello_world_object_proof` 与 `chaos_stage4_hello_world_object_proof_run` 的真实 configure/build/run 闭环。
- 当前真实 run 产物已经满足完成定义：
  - `artifacts/proof/native-reference/HelloWorldObject/run/stdout.log` 包含 `Hello, World!`
  - `artifacts/proof/native-reference/HelloWorldObject/run/exit-code.txt` 记录 `0`
  - `artifacts/proof/native-reference/HelloWorldObject/run/stderr.log` 为空
- `build/scripts/verify-roadmap-0.py` 与 `build/scripts/verify-roadmap-0.ps1` 已接入真实 `chaos_stage4_hello_world_object_proof_run` gate，不再停留在 prepare/codegen helper；Windows 路径会产出 `windows-stage4-native-reference.gate.json`。

## 验证

- `python -m unittest tests.unit.run.test_stage4_bootstrap_support_skeleton`
- `python -m unittest tests.unit.run.test_stage4_runtime_core_skeleton`
- `python -m unittest tests.unit.run.test_stage4_windows_reference_wiring`
- `python -m unittest tests.unit.run.test_native_prefix_naming`
- `python -m unittest tests.tooling.run.test_verify_roadmap0`
- `C:\Users\1\AppData\Local\cmake-4.3.1-portable\PFiles64\CMake\bin\cmake.exe --build E:\agent\booming-il2cpp\artifacts\presets\windows-x64-reference-real --config Release --target chaos_stage4_hello_world_object_proof -- /m`
- `C:\Users\1\AppData\Local\cmake-4.3.1-portable\PFiles64\CMake\bin\cmake.exe --build E:\agent\booming-il2cpp\artifacts\presets\windows-x64-reference-real --config Release --target chaos_stage4_hello_world_object_proof_run -- /m`
- `$env:Path='C:\Users\1\AppData\Local\cmake-4.3.1-portable\PFiles64\CMake\bin;'+$env:Path; powershell -NoLogo -NoProfile -NonInteractive -ExecutionPolicy Bypass -File build/scripts/verify-roadmap-0.ps1 -HostProfile windows`

## 未完成 / 阻塞

- 无。

## 下一步

- 无。若继续 reboot 主线，由父 roadmap 决定是否派生 `Stage 5: Post-Proof Split`。
