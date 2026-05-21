# 进度记录 v1-03

## 时间

- 2026-04-06 14:12:36 +08:00

## 已完成

- 先补 Stage 5A proof host wiring 测试，要求仓库存在：
  - `tests/proof/native-reference/GenericEchoClosedMinimal/main.cpp`
  - `tests/proof/native-reference/GenericEchoClosedMinimal/CMakeLists.txt`
  - `tests/proof/native-reference/GenericEchoClosedMinimal/RunNativeReferenceProof.cmake`
  - root `CMakeLists.txt` 中的 `windows-x64-reference` 路由接线
- 新增 `GenericEchoClosedMinimal` proof host：
  - `main.cpp` 负责初始化 runtime / bridge 并调用 generated `RunNativeReferenceProof`
  - `CMakeLists.txt` 沿 Stage 4 spine 补齐 host input build、driver build、managed closure、codegen、proof run targets
  - `RunNativeReferenceProof.cmake` 统一收集 `stdout.log`、`stderr.log`、`exit-code.txt`
- root `CMakeLists.txt` 已接入 `tests/proof/native-reference/GenericEchoClosedMinimal`

## 验证

- `dotnet build src/managed/Chaos.IL2CPP.Driver/Chaos.IL2CPP.Driver.csproj -c Release`
- `python -m unittest tests.unit.run.test_stage3_managed_minimal_closure tests.unit.run.test_stage4_native_reference_proof tests.unit.run.test_stage4_windows_reference_wiring tests.unit.run.test_stage5a_generic_echo_windows_reference tests.unit.run.test_native_prefix_naming`

## 当前状态

- Stage 5A 的 managed closure、native lowering plan、proof host 静态接线都已齐备。
- `test_windows_reference_proof_run_outputs_generic_ok` 已写好，但当前环境里没有可直接调用的 `cmake`，因此该测试被 `skip`，尚未得到真实本机构建/运行证据。

## 下一步

- 在具备 `cmake` 的 Windows 环境里重跑 `test_windows_reference_proof_run_outputs_generic_ok`。
- 若输出 `generic-ok` 且 `exit code` 为 `0`，再评估是否把这条 run gate 接到更高层脚本或统一验证入口。
