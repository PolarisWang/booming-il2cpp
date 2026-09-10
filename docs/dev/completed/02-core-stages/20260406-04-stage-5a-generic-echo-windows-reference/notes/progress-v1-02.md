# 进度记录 v1-02

## 时间

- 2026-04-06 14:07:01 +08:00

## 已完成

- 按 TDD 先补回归测试，要求 native reference emission 不只输出 `native-reference.generated.cpp` 与 `native-proof.manifest.json`，还必须额外落盘显式 `native-proof.plan.json`。
- 在 `Chaos.IL2CPP.Contracts` 中新增 native reference lowering plan contract，并让 `DriverEntry` 负责把该 plan artifact 写到输出目录。
- 重构 `NativeReferenceProofEmitter`：先从 managed closure bundle 归约出显式 lowering plan，再由模板渲染消费该 plan，而不是一边猜 shape 一边直接渲染。
- 保持 `src/**` sample-agnostic：清掉 `ClosedGenericEcho` / `first-proof` 等样例绑定标签，把 `MetadataWriterStage` 的入口原因统一为 `entrypoint`。
- 同步 Stage 4 CMake byproduct、AOT canonical artifact 与 schema snapshot，避免 contract / wiring 测试继续漂移。

## 验证

- `dotnet build src/managed/Chaos.IL2CPP.Driver/Chaos.IL2CPP.Driver.csproj -c Release`
- `python -m unittest tests.unit.run.test_stage3_managed_minimal_closure`
- `python -m unittest tests.unit.run.test_stage4_native_reference_proof`
- `python -m unittest tests.unit.run.test_stage4_windows_reference_wiring`
- `python -m unittest tests.unit.run.test_stage5a_generic_echo_windows_reference`
- `python -m unittest tests.unit.run.test_native_prefix_naming`

## 结论

- 当前 Stage 5A 已不再把具体 proof 样例语义硬编码进 `src/**`；native reference emission 现在有了可落盘、可检查、可复用的 lowering plan contract。
- 这一步尚未完成 Stage 5A；剩余主缺口已经收敛为 `GenericEchoClosedMinimal` 的真实 Windows proof host / run gate 接线。

## 下一步

- 为 `GenericEchoClosedMinimal` 新增 proof host 目录、`main.cpp`、`CMakeLists.txt` 与运行脚本，沿 Stage 4 spine 做最小并行接线。
- 先写失败测试，要求真实 proof run 输出 `generic-ok` 且 exit code 为 `0`，再补构建与运行链路。
