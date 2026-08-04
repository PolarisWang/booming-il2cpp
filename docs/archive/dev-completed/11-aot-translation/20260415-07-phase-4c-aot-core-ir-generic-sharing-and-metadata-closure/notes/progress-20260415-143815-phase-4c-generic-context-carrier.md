# 2026-04-15 14:38:15 +08:00 Phase 4C generic context carrier slice

## 本轮完成

- 以 RED -> GREEN 方式新增 3 个 Phase 4C 测试：
  - `test_phase4c_aot_core_ir_generics.py`
  - `test_phase4c_generic_layout_aot_core_ir.py`
  - `test_phase4c_metadata_closure_contract.py`
- `ManagedClosureContracts` 新增：
  - `GenericContextKind`
  - `GenericContextArtifact`
  - `ManagedNaming.TryCreateGenericContext(...)`
- `AotCoreIrMethodArtifact` / `AotCoreIrReferenceArtifact` 开始显式携带 `genericContext`
- supplemental metadata template 的 type/method entry 开始显式携带 `genericContext`
- retained generic proofs 已能在 closure artifact 上验证：
  - `GenericLayoutProofEntry`
  - `RequiredInstantiationClosureProofEntry`

## 验证

1. `python -m pytest tests/unit/compatibility/test_phase4c_aot_core_ir_generics.py tests/unit/compatibility/test_phase4c_generic_layout_aot_core_ir.py tests/unit/compatibility/test_phase4c_metadata_closure_contract.py -q`
   - 结果：4 passed
2. `python -m pytest tests/unit/compatibility/test_phase4b_aot_core_ir_object_model.py tests/unit/compatibility/test_phase4c_aot_core_ir_generics.py tests/unit/compatibility/test_phase4c_generic_layout_aot_core_ir.py tests/unit/compatibility/test_phase4c_metadata_closure_contract.py tests/unit/compatibility/test_metadata_supplement_bridge_subjects.py -q`
   - 结果：13 passed
3. `dotnet build src/managed/Chaos.IL2CPP.CodeGen/Chaos.IL2CPP.CodeGen.csproj -c Release -m:1`
   - 结果：Build succeeded
4. `dotnet build src/managed/Chaos.IL2CPP.Driver/Chaos.IL2CPP.Driver.csproj -c Release -m:1`
   - 结果：Build succeeded

## 下一步

- 继续在 `NativeAotLoweringPlanner` / template model 上消费 `genericContext`
- 优先瞄准 exact static generic call 与 closed generic type reference
- 不把 generic `callvirt` / interface dispatch widening 偷渡到本切片；那部分仍属于 Phase 4D
