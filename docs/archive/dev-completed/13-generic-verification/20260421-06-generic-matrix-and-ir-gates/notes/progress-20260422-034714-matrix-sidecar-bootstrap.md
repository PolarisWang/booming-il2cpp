# 20260422-034714 Matrix Sidecar Bootstrap

- `20260421-06` 已启动并完成第一刀：`generic-capability-matrix.json` 正式进入 managed closure bundle。
- sidecar 现已覆盖：
  - canonical HotUpdate naming
  - family budget
  - per-demand authority summary
  - `unsupportedLeakCount / missingAuthorityCount / nonCanonicalHotUpdateNameCount`
- `subject_workers.py` 已暴露 `genericCapabilityMatrixPath`，tooling 不再需要硬编码猜这个 sidecar。
- full-assembly-closure 与 zero-demand bundle 已验证可稳定输出 zero-state matrix。
- 已完成验证：
  - `tests/unit/compatibility/test_phase4c_generic_matrix_gates.py`
  - `tests/unit/compatibility/test_managed_closure_contract_bundle.py`
  - `tests/unit/compatibility/test_full_assembly_closure_codegen_audit_plan.py`
  - 以及 phase4c / loader generic authority 回归集
