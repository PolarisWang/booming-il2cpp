# Path Mapping v1 — 当前路径 → 终态路径映射表

Date: 2026-04-10
Status: frozen-v1

---

## 1. Contract Fixture 迁移（Phase 1）

### 1.1 Artifact Samples

| 当前路径 | 终态路径 | 动作 | 受影响 Consumer |
| --- | --- | --- | --- |
| `contracts/artifacts/v0/samples/*.min.json` | `tests/contracts/analysis/v0/samples/*.min.json` | 移动 | `path_resolver.contract_roots()`, `testing/contracts.py`, `test_contract_check.py`, `test_contract_paths.py`, `docs/architecture/runtime-baseline/schema-pack-v0.md` |
| `contracts/artifacts/v0/snapshots/*.snapshot.json` | `tests/contracts/analysis/v0/snapshots/*.snapshot.json` | 移动 | `path_resolver.contract_roots()`, `testing/contracts.py`, `test_contract_snapshot_baselines_live_under_contracts_tree()` |

### 1.2 Native Samples

| 当前路径 | 终态路径 | 动作 | 受影响 Consumer |
| --- | --- | --- | --- |
| `contracts/native/v0/samples/abi/*.cpp` | `tests/contracts/native/v0/samples/abi/*.cpp` | 移动 | `path_resolver.contract_roots()` |
| `contracts/native/v0/samples/bridge/*.cpp` | `tests/contracts/native/v0/samples/bridge/*.cpp` | 移动 | `path_resolver.contract_roots()` |

### 1.3 Legacy / Compatibility 路径删除

| 当前路径 | 终态路径 | 动作 | 受影响 Consumer |
| --- | --- | --- | --- |
| `contracts/examples/v0/artifacts/*.min.json` | 删除 | 删除 | `path_resolver.contract_roots()` (`artifactSampleCompatibilityRoot`), docs |
| `contracts/native/examples/v0/**` | 删除 | 删除 | `path_resolver.contract_roots()` (`nativeSampleCompatibilityRoot`), docs |
| `contracts/native/docs/v0/*.md` | `contracts/docs/v0/` | 合并移动 | docs 引用 |
| `tests/contract/**` | 删除 | 删除 | `CMakeLists.txt` 负面断言, `test_repo_layout.py` 负面断言, `verify-runtime-baseline.*` 负面断言 |

### 1.4 path_resolver.py 变更

```python
# 当前
def contract_roots(repo_root, *, version="v0"):
    return {
        "artifactSchemaRoot":              repo_root / "contracts" / "artifacts" / version / "schemas",
        "artifactSampleRoot":              repo_root / "contracts" / "artifacts" / version / "samples",
        "artifactSnapshotRoot":            repo_root / "contracts" / "artifacts" / version / "snapshots",
        "nativeSampleRoot":                repo_root / "contracts" / "native" / version / "samples",
        "artifactSampleCompatibilityRoot": repo_root / "contracts" / "examples" / version / "artifacts",
        "nativeSampleCompatibilityRoot":   repo_root / "contracts" / "native" / "examples" / version,
    }

# 终态
def contract_roots(repo_root, *, version="v0"):
    return {
        "artifactSchemaRoot":    repo_root / "contracts" / "artifacts" / version / "schemas",
        "artifactSampleRoot":    repo_root / "tests" / "contracts" / "analysis" / version / "samples",
        "artifactSnapshotRoot":  repo_root / "tests" / "contracts" / "analysis" / version / "snapshots",
        "nativeSampleRoot":      repo_root / "tests" / "contracts" / "native" / version / "samples",
    }
    # artifactSampleCompatibilityRoot 和 nativeSampleCompatibilityRoot 删除
```

---

## 2. Build Tooling 重组（Phase 2）

### 2.1 根目录文件迁移

| 当前路径 | 终态路径 | 动作 |
| --- | --- | --- |
| `build/toolchains/run/common.py` | `build/toolchains/run/core/common.py` | 移动 |
| `build/toolchains/run/manifest.py` | `build/toolchains/run/core/manifest.py` | 移动 |
| `build/toolchains/run/result.py` | `build/toolchains/run/core/result.py` | 移动 |
| `build/toolchains/run/operation_reporting.py` | `build/toolchains/run/core/operation_reporting.py` | 移动 |
| `build/toolchains/run/tooling.py` | `build/toolchains/run/core/tooling.py` | 移动 |
| `build/toolchains/run/tui.py` | 拆分到 `build/toolchains/run/ui/` 下 6 个文件 | 拆分移动 |
| `build/toolchains/run/menu.py` | `build/toolchains/run/ui/menu.py` | 移动 |
| `build/toolchains/run/project_workspace.py` | `build/toolchains/run/subject/project_workspace.py` | 移动 |
| `build/toolchains/run/deploy_layout.py` | `build/toolchains/run/subject/deploy_layout.py` | 移动 |
| `build/toolchains/run/runtime.py` | 保持原位 | 不动 |
| `build/toolchains/run/run.py` | 保持原位 | 不动 |
| `build/toolchains/run/run_manifest.json` | `build/toolchains/run/manifests/run_manifest.json` | 移动（Phase 3 分片） |
| `build/toolchains/run/runtime_manifest.json` | 保持原位 | 不动 |

### 2.2 commands/ 重组

| 当前路径 | 终态路径 | 动作 |
| --- | --- | --- |
| `build/toolchains/run/commands/test.py` | 拆分到 `build/toolchains/run/commands/test/` 下 6 个文件 | 拆分移动 |
| 其他 `commands/*.py` | 保持原位 | 不动 |

### 2.3 testing/ 重组

| 当前路径 | 终态路径 | 动作 |
| --- | --- | --- |
| `testing/contracts.py` | `testing/contracts/contracts.py` | 移动 |
| `testing/path_resolver.py` | `testing/contracts/path_resolver.py` | 移动 |
| `testing/selectors.py` | `testing/contracts/selectors.py` | 移动 |
| `testing/catalog.py` | `testing/registry/catalog.py` | 移动 |
| `testing/registry.py` | `testing/registry/registry.py` | 移动 |
| `testing/public_specs.py` | `testing/registry/public_specs.py` | 移动 |
| `testing/suite_manifest.py` | `testing/registry/suite_manifest.py` | 移动 |
| `testing/subjects.py` | `testing/subject/subjects.py` | 移动 |
| `testing/subject_planner.py` | `testing/subject/subject_planner.py` | 移动 |
| `testing/subject_executor.py` | `testing/subject/subject_executor.py` | 移动 |
| `testing/subject_reporting.py` | `testing/subject/subject_reporting.py` | 移动 |
| `testing/subject_validations.py` | `testing/subject/subject_validations.py` | 移动 |
| `testing/subject_workers.py` | 拆分到 `testing/subject/workers/` 下 5 个文件 | 拆分移动 |
| `testing/events.py` | `testing/session/events.py` | 移动 |
| `testing/session.py` | `testing/session/session.py` | 移动 |
| `testing/reporting.py` | `testing/session/reporting.py` | 移动 |
| `testing/traffic_light.py` | `testing/session/traffic_light.py` | 移动 |
| `testing/fingerprints.py` | `testing/fingerprints.py` | 保持 |
| `testing/perf.py` | `testing/perf.py` | 保持 |
| `testing/adapters/**` | `testing/adapters/**` | 保持 |

---

## 3. Manifest 分片（Phase 3）

| 当前路径 | 终态路径 | 动作 |
| --- | --- | --- |
| `build/toolchains/run/run_manifest.json` (925 行单文件) | `build/toolchains/run/manifests/run/groups.json` + `commands.*.json` × N | 分片 |

分片规则见 `file-split-policy-v1.md`。

---

## 4. Managed Source 拆分（Phase 4）

### 4.1 CodeGen

| 当前路径 | 终态路径 | 动作 |
| --- | --- | --- |
| `NativeReferenceLoweringPlanner.cs` (~2036 行) | facade 保留 + `Lowering/` 下 7+ 个文件 | 拆分 |
| `NativeReferenceProofEmitter.cs` (~1165 行) | `Emission/NativeReferenceProofEmitter.cs` (可选同轮拆分) | 移动 |
| `CodeGenStage.cs` | 保持原位 | 不动 |

### 4.2 Loader

| 当前路径 | 终态路径 | 动作 |
| --- | --- | --- |
| `LoaderStage.cs` (~1899 行) | facade 保留 + `Metadata/` + `Decode/` + `Materialization/` + `Models/` 下 10+ 个文件 | 拆分 |

---

## 5. Tests 镜像重排（Phase 5）

### 5.1 tests/tooling/run/

| 当前路径 | 终态子目录 | 动作 |
| --- | --- | --- |
| `test_build.py`, `test_build_batch.py` | `commands/` | 移动 |
| `test_prepare_scopes.py`, `test_project_command.py` | `commands/` | 移动 |
| `test_registry_command.py`, `test_unified_test_command.py` | `commands/` | 移动 |
| `test_verify_roadmap0.py` | `commands/` | 移动 |
| `test_tui.py`, `test_tui_routing.py` | `tui/` | 移动 |
| `test_interactive_session.py` | `tui/` | 移动 |
| `test_subject_command.py`, `test_trace_compare.py` | `subject/` | 移动 |
| `test_test_watch_summary.py` | `subject/` | 移动 |
| `test_contract_check.py`, `test_contract_paths.py` | `contracts/` | 移动 |
| `test_python_unittest_command.py` | `contracts/` | 移动 |
| `test_cmake_bootstrap.py`, `test_dotnet_bootstrap.py` | `bootstrap/` | 移动 |
| `test_wrapper_bootstrap_unix.py` | `bootstrap/` | 移动 |
| `test_platform_gate_validate_only.py` | `platform/` | 移动 |
| `test_windows_terminal_handoff.ps1` | `platform/` | 移动 |
| `test_run_host_platform_compat.ps1` | `platform/` | 移动 |
| `test_command_manifest.py`, `test_common.py` | `infra/` | 移动 |
| `test_doctor.py`, `test_json_output.py` | `infra/` | 移动 |
| `test_operation_reporting.py`, `test_runtime_manifest.py` | `infra/` | 移动 |
| `test_run_cli.ps1`, `test_run_cli.sh` | `infra/` | 移动 |

### 5.2 tests/unit/run/

| 当前路径 | 终态子目录 | 动作 |
| --- | --- | --- |
| `test_path_resolver.py`, `test_subject_contracts_source_cutover.py` | `contracts/` | 移动 |
| `test_repo_layout.py`, `test_managed_closure_contract_bundle.py` | `contracts/` | 移动 |
| `test_subject_planner.py` ~ `test_subject_workers_perf.py` | `subject/` | 移动 |
| `test_subject_manifest_schema.py` | `subject/` | 移动 |
| `test_adapter_registry.py`, `test_suite_manifest.py` | `registry/` | 移动 |
| `test_session.py`, `test_events_schema.py` | `registry/` | 移动 |
| `test_fingerprints.py`, `test_traffic_light.py` | `registry/` | 移动 |
| `test_perf_policy.py` | `registry/` | 移动 |
| `test_phase*.py` (6 files) | `phases/` | 移动 |
| `test_project_workspace.py`, `test_deploy_core.py` | `workspace/` | 移动 |
| `test_native_*.py` (3 files), `test_scriban_vendor_build.py` | `native/` | 移动 |
| `test_stage*.py` (6 legacy shim files) | `stages/` | 移动 |

---

## 6. Legacy 清理（Phase 6）

| 当前路径 | 动作 | 说明 |
| --- | --- | --- |
| `contracts/examples/**` | 确认删除 | Phase 1 已删除 |
| `contracts/native/examples/**` | 确认删除 | Phase 1 已删除 |
| `contracts/native/docs/**` | 确认已合并 | Phase 1 已合并 |
| `tests/contract/**` | 确认删除 | Phase 1 已删除 |
| `path_resolver.contract_roots()` compatibility keys | 确认已移除 | Phase 1 已清理 |
| 旧 import 路径兼容层 | 确认已移除 | Phase 2 已清理 |
| 新增 repo layout enforcement tests | 新建 | 防止结构回退 |
| 新增 file budget enforcement tests | 新建 | 防止超长文件回退 |
