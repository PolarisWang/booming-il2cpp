# 2026-04-22 02:41:00 +08:00 进展记录

## 本轮完成

- `MetadataWriterStage` 切到 `GenericInstantiationDemandGraph` authority。
- `AotCoreIrLowering` 在 method/reference 层面显式输出：
  - `openDefinitionSubjectId`
  - `sharedGenericBodyId`
  - `instantiationStubId`
- `test_phase4c_aot_core_ir_generics.py`、`test_phase4c_generic_layout_aot_core_ir.py`、`test_phase4c_metadata_closure_contract.py` 以及 loader authority contract 回归已变绿。

## 结论

- `20260421-04` 的 demand graph hard cut 已经完成 authority 收口。
- 后续 shared generic body / thin stub 的真实执行语义，需要在 `20260421-05` 继续推进 planner/emitter/runtime。
