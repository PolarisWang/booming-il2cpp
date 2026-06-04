# STATUS

## phase: completed (all phases done, bulk migration done)

## Completion Summary

### 4-phase implementation completed and verified
- ✅ Phase 1: Contract schema + Pipeline stage filtering
- ✅ Phase 2: Patch scope 裁剪 (PatchDataExtractor + DriverEntry)
- ✅ Phase 3: Audit 补充 + 合同迁移 (--fix-annotations with managed_fact integration)
- ✅ Phase 4: 回归验证 (convert-char pipeline 16/20 passed, annotation works)

### Additional improvement: managed_fact integration for exception detection
`--fix-annotations` now runs managed_fact to detect exception-throwing methods:
- `exceptionMessage: null` → normal path → included in benchmarkMethodIndices
- `exceptionMessage` non-null → exception path → excluded
- Among normal paths, non-void return → included in hotupdateMethodIndices
- Falls back to `is_auto_callable()` if managed_fact fails (e.g. complex interop families)

### Bulk migration results (157/159 families annotated)
| Metric | Value |
|--------|-------|
| Total contracts | 159 |
| Annotated | 157 (98.7%) |
| Missing (0-method contracts) | 2 |
| Total methods across all families | 2738 |
| Benchmark methods (non-exception) | 1963 (71.7%) |
| Hotupdate methods (non-void + non-exception) | 1441 (52.6%) |

### Performance (convert-char validated)
- Native AOT: 233M ops/s (9 benchmark methods, 8 real results after value gate)
- Native JIT: 227M ops/s
- Hotupdate: semanticChangedCount=9, all stages pass
- 4 pre-existing failures (unrelated to annotation changes)

### Additional: D3 runner pipeline integration (bridge report)
- Added `_write_hotupdate_report_from_pipeline()` in `cli.py` — bridges pipeline's hotupdate stages (Stage 9-12) to the `hotupdate-verification-report.json` format that `check-hotupdate.sh` expects
- Pipeline now writes both `unified-verification-report.json` and `hotupdate-verification-report.json` to the results directory
- `check-hotupdate.sh` finds the bridge report via fallback path (`testing/results/foundation-dll/...`)
- `d3PatchApplied=True` with real `semanticChangedCount` from pipeline stages, so the script validates semantic change and overhead correctly
- Golden patched values validation already implemented via `run_managed_patch_fact()` + `patch_cross_verify()` stages (from Phase 2)
- Commit `67a6a988d` pushed to main

## Files modified (complete list)
| File | Change |
|------|--------|
| `testing/foundation-dll/_contracts/family-schema.json` | Added benchmarkMethodIndices + hotupdateMethodIndices |
| `testing/foundation-dll/verification/stages/benchmark.py` | Added _load_benchmark_indices() + filtering |
| `testing/foundation-dll/verification/stages/hotupdate.py` | Added _load_hotupdate_indices() + pass to _ensure_patch_data() for all stages |
| `src/managed/Chaos.IL2CPP.Generator/PatchDataExtractor.cs` | Added --subject-only/--subject-indices filtering, ExtractSubjectIndex() |
| `src/managed/Chaos.IL2CPP.Driver/DriverEntry.cs` | Added --subject-only/--subject-indices arg parsing |
| `testing/foundation-dll/verification/stages/pre_verification_audit.py` | Added p4 annotation check + --fix-annotations with managed_fact integration |
| `testing/foundation-dll/verification/entry_points/cli.py` | Added _write_hotupdate_report_from_pipeline() bridge, writes hotupdate-verification-report.json |
| 157x `capability-family-contract.json` | Added benchmarkMethodIndices + hotupdateMethodIndices |
