# 进展记录 v1-06

## 时间

- 2026-04-11 11:28:13 +08:00

## 本轮完成

- 完成 Batch 4 nested EH 最小 proof slice：
  - 新增 `brainstorm-v1-03.md`
  - 新增 `design-v1-03.md`
  - 新增 `tests/unit/run/test_phase2_nested_exception_proof.py`
  - 新增 `subjects/NestedExceptionProof/`
  - `SemanticWorldStage` 新增 `requires-nested-exception-handler`
  - `LinkerStage` 新增 nested EH shape 冻结入口
  - `NativeReferenceLoweringPlanner` / `NativeReferenceProofEmitter` 新增 `managed-exceptions.nested-throw-catch-finally.minimal`
  - 新增模板 `NativeReferenceProof.NestedExceptionThrowCatchFinally.cpp.scriban`
- 完成 Batch 4 marshaling/export metadata 最小 proof slice：
  - 新增 `brainstorm-v1-04.md`
  - 新增 `design-v1-04.md`
  - 新增 `tests/unit/run/test_phase2_marshaling_proof.py`
  - 新增 `subjects/MarshalingProof/`
  - `ManagedMethodModel` 补充 `IsUnmanagedCallersOnly`
  - loader 新增 `HasUnmanagedCallersOnlyAttribute(...)`
  - `SemanticWorldStage` 新增 `requires-utf8-string-marshaling` / `requires-unmanaged-callers-only-export`
  - `LinkerStage` 新增 marshal/export shape 冻结入口
  - `NativeReferenceLoweringPlanner` / `NativeReferenceProofEmitter` 新增 `interop.marshaling-utf8-export.minimal`
  - 新增模板 `NativeReferenceProof.MarshalingUtf8Export.cpp.scriban`

## 验证结果

- `python -m pytest tests/unit/run/test_phase2_nested_exception_proof.py -v` -> 3 passed
- `python -m pytest tests/unit/run/test_phase2_marshaling_proof.py -v` -> 3 passed
- `python -m pytest tests/unit/run -v` -> 166 passed
- `dotnet build solutions/core/windows/chaos-il2cpp-core.sln -c Debug` -> succeeded
- `dotnet build subjects/NestedExceptionProof/source/NestedExceptionProof.csproj -c Debug` -> succeeded
- `dotnet build subjects/MarshalingProof/source/MarshalingProof.csproj -c Debug` -> succeeded

## 备注

- Batch 4 的 interop 仅冻结了 UTF-8 string marshal 与 unmanaged export metadata；真正的 reverse P/Invoke callback 执行仍未落地。
- Batch 4 的 nested EH 仅闭环了 nested `try/catch/finally`；`filter` / `fault` 仍停留在 loader 可见、proof 未闭环的状态。

## Wiki

- 本轮仍未写入 wiki。
- 原因：Phase 2 的 runtime completeness 能力矩阵仍在继续扩展，等 Batch 5/6 的最小 slice 稳定后再统一沉淀。

## 下一步

- 收敛 Batch 5 linker stripping / debug baseline 的最小 proof 边界，并建立新的 RED 入口。
