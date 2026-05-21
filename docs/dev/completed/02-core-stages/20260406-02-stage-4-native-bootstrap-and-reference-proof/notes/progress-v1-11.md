# 进度记录 v1-11

## 时间

- 2026-04-06 11:52:45 +08:00

## 已完成

- 将 `tests/proof/native-reference/HelloWorldObject/CMakeLists.txt` 从“单条 `add_custom_command` 串联 proof input build / driver build / managed closure / native reference emission”重构为四个显式阶段。
- 新增 `chaos_stage4_hello_world_object_host_input_build`、`chaos_stage4_hello_world_object_driver_build`、`chaos_stage4_hello_world_object_managed_closure`、`chaos_stage4_hello_world_object_codegen` 四个目标，并为各阶段补上 stamp 与 byproducts。
- 在 proof CMake 中显式声明 Stage 3 bundle 文件：`typed-il-ir.json`、`aot-manifest.json`、`metadata-registration.json`、`code-registration.json`、`closure.manifest.json`，把 Stage 4 对 Stage 3 输入的消费边界写进 build graph。
- 将 `build/scripts/verify-roadmap-0.py` 与 `build/scripts/verify-roadmap-0.ps1` 的 Stage 4 prepare helper 从 `dotnet run --project` 改为执行已构建 `Chaos.IL2CPP.Driver.dll`，把“build”与“run”边界收口一致。

## 验证

- `python -m unittest tests.unit.run.test_stage4_windows_reference_wiring`
- `python -m unittest tests.tooling.run.test_verify_roadmap0`
- `python -m unittest tests.unit.run.test_stage4_native_reference_proof`
- `python -m unittest tests.unit.run.test_stage4_bootstrap_support_skeleton`
- `python -m unittest tests.unit.run.test_stage4_runtime_core_skeleton`
- `python -m unittest tests.unit.run.test_native_prefix_naming`

## 未完成 / 阻塞

- 当前仍缺少 `cmake` 与可用的独立 C++ 编译器，显式阶段化只能降低 build graph 不确定性，不能替代真实 configure/build/link 验证。
- `proof target` 仍保持 `EXCLUDE_FROM_ALL`，直到 generated C++ 与 `runtime-core/bootstrap/support` 在真实工具链下闭环通过。

## 下一步

- 等工具链恢复后，基于已经显式化的 Stage 4 build graph 执行真实 `cmake --preset windows-x64-reference`、build、link 与 proof run，记录第一批真实编译/链接错误。
- 若工具链短期仍不可用，则继续只做文件级回归收口，不重新引入隐式 build 链路或测试工程特化逻辑。
