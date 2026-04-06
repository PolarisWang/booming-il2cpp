# 进度记录 v1-10

## 时间

- 2026-04-06 11:44:37 +08:00

## 已完成

- 扩大 `tests/unit/run/test_stage4_native_reference_proof.py` 的源码边界检查范围，从原先的少量单文件断言升级为目录级扫描。
- 新的回归守卫覆盖 `src/managed/Chaos.IL2CPP.CodeGen`、`src/managed/Chaos.IL2CPP.Contracts`、`src/managed/Chaos.IL2CPP.Driver` 与 `src/native/{bootstrap,runtime-core,support}`。
- 守卫显式禁止 `HelloWorldObject`、`Greeter`、`tests/proof/input/HelloWorldObject`、`artifacts/proof/managed-closure/HelloWorldObject`、`artifacts/proof/native-reference/HelloWorldObject` 回流到 `src/**`。
- 复跑 Stage 4 相关单测，确认新增守卫没有误伤当前 sample-agnostic 的实现边界。

## 验证

- `python -m unittest tests.unit.run.test_stage4_native_reference_proof`
- `python -m unittest tests.unit.run.test_stage4_bootstrap_support_skeleton`
- `python -m unittest tests.unit.run.test_stage4_runtime_core_skeleton`
- `python -m unittest tests.unit.run.test_stage4_windows_reference_wiring`
- `python -m unittest tests.unit.run.test_native_prefix_naming`

## 未完成 / 阻塞

- 当前仍缺少 `cmake` 与可用的独立 C++ 编译器，无法把新增守卫进一步推进到真实 configure/build/link 级别验证。
- `proof target` 仍保持 `EXCLUDE_FROM_ALL`，直到 generated C++ 与 `runtime-core/bootstrap/support` 经过真实工具链验证。

## 下一步

- 等工具链恢复后，优先对 `runtime-core + bootstrap + support + proof-host` 执行真实 configure/build/link，并把 `verify-roadmap-0` 从 Stage 4 prepare-only 推进到实际 proof build/run gate。
- 在工具链恢复前，继续维持 `src/**` sample-agnostic 约束，只做必要的编译边界压缩与状态同步，不重新引入任何测试工程特化逻辑。
