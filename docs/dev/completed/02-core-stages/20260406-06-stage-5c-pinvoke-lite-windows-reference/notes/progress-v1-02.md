# 进度记录 v1-02

## 时间

- 2026-04-06 16:19:51 +08:00

## 已完成

- 新增 `tests/proof/input/PInvokeLiteDllImportMinimal/`，把 Stage 5C 首轮 proof 输入冻结为最小 `DllImport("kernel32.dll")` 整数 import 样例。
- 新增 `tests/unit/run/test_stage5c_pinvoke_lite_windows_reference.py`，先以 TDD 方式冻结 closure manifest、metadata registration、native lowering plan、proof host wiring 与真实 proof run 期望。
- 扩展 `ManagedClosureContracts.cs`、`LoaderStage.cs` 与 `MetadataWriterStage.cs`，让 imported method 的 module / entry-point metadata 能从 loader 一直保留到 metadata registration。
- 扩展 `NativeReferenceProofEmitter.cs` 与 `NativeReferenceProof.PInvokeDllImportMinimal.cpp.scriban`，把 Stage 5C 的 generated C++ emission 落到显式 lowering plan + Scriban 模板渲染。
- 新增 `tests/proof/native-reference/PInvokeLiteDllImportMinimal/` proof host，拿到真实 `pinvoke-ok|14` / `exit code 0` 证据。

## 验证

- `python -m unittest -v tests.unit.run.test_stage5c_pinvoke_lite_windows_reference`
- `dotnet build src/managed/Chaos.IL2CPP.Driver/Chaos.IL2CPP.Driver.csproj -c Release`
- `python -m unittest -v tests.unit.run.test_stage5b_reflection_lite_windows_reference`
- `python -m unittest -v tests.unit.run.test_stage5a_generic_echo_windows_reference`
- `python -m unittest -v tests.unit.run.test_stage4_native_reference_proof`
- `python -m unittest -v tests.unit.run.test_stage4_windows_reference_wiring`
- `python -m unittest -v tests.unit.run.test_native_prefix_naming`

## 关键判断

- Stage 5C 已经形成一条稳定的 Windows interop foundation：从 managed `DllImport` 声明到 generated native reference proof，再到真实 proof run 输出 `pinvoke-ok|14`，中间 contract 形状保持一致。
- `src/**` 继续保持 sample-agnostic；`PInvokeLiteDllImportMinimal` 只停留在 `tests/**`、artifact 输出与任务文档中。
- 当前 Stage 5C proof 绑定 `kernel32.dll` / `MulDiv`，因此它证明的是“Windows 上的最小 import path 已闭环”，而不是“这一份 interop sample 已具备跨宿主 parity”。

## 下一步

- 本任务已满足完成条件；由 Stage 5 父 roadmap 收窄并归档 `20260406-07-stage-5d-macos-reference-parity` 的 parity handoff 计划。
