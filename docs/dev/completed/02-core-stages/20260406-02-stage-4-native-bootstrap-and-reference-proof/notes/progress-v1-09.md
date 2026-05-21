# 进度记录 v1-09

## 时间

- 2026-04-06 11:34:54 +08:00

## 已完成

- 按边界约束回收 `src/**` 中的样例特化逻辑。`NativeReferenceProofEmitter` 不再写死 `HelloWorldObject` / `Greeter`，而是从 Stage 3 bundle 推导 entry、ctor、instance method、field 与 metadata token。
- 把 Stage 4 生成产物名从 `generated/HelloWorldObject.generated.cpp` 统一收口为 `generated/native-reference.generated.cpp`，避免 contracts surface 与测试工程名绑死。
- 把 `src/native/bootstrap/bootstrap.cpp` 从样例 token switch 改成通用 `token -> opaque handle` 解析，继续保留 `resolve_icall` 的 first-proof 最小语义。
- 在 `src/native/runtime-core/runtime_core.cpp` 中补上最小 inline field storage，使 generated direct-call path 能真实落到 `field_set_value` / `field_get_value`。
- 补记 Chunk 5 当前已完成的部分接线：`windows-x64-reference` 继续保留 contract smoke target，同时新增 Stage 4 proof/codegen target；`verify-roadmap-0.py` 与 `.ps1` 已先接入 Stage 4 codegen prepare helper。

## 验证

- `python -m unittest tests.unit.run.test_stage4_native_reference_proof`
- `python -m unittest tests.unit.run.test_stage4_bootstrap_support_skeleton`
- `python -m unittest tests.unit.run.test_stage4_windows_reference_wiring`
- `python -m unittest tests.unit.run.test_stage4_runtime_core_skeleton`
- `python -m unittest tests.unit.run.test_native_prefix_naming`

## 未完成 / 阻塞

- 当前 `proof target` 仍保持 `EXCLUDE_FROM_ALL`，原因不是 CMake 接线缺失，而是生成 C++ 还没有经过真实 configure/build/link 验证。
- 环境仍缺少 `cmake` 与可用的独立 C++ 编译器，因此 Stage 4 只能做文件级 TDD 与接口收口，无法完成真实 proof build/run。

## 下一步

- 工具链恢复后，优先对 `runtime-core + bootstrap + support + proof-host` 执行真实 configure/build/link，并把 `verify-roadmap-0` 从 Stage 4 prepare-only 推进到实际 proof build/run gate。
- 在工具链恢复前，继续保持 `src/**` sample-agnostic，只做必要的编译边界压缩和文档同步，不扩大功能面。
