# 2026-04-15 00:53:52 +08:00 First Launcher / Discovery Cutover Green

## 本批完成项

- 删除 `src/reference/Chaos.TestFramework/Chaos.TestFramework.cs` 中的：
  - `ChaosSourceEntrySelection`
  - `ChaosSourceEntryArguments`
- 删除 retained launcher 中的 source-entry 兼容面：
  - `subjects/SolutionCorePack/source/Launcher/Program.cs`
  - `subjects/HotUpdateHostPack/source/Host/Program.cs`
  - `subjects/MixedExecutionFeaturePack/source/ManagedBridge/Program.cs`
- 删除 `subject_workers` 对 `--chaos-source-entry` 的追加：
  - `build/toolchains/run/testing/subject_workers.py`
- 将 declaration discovery 的 attribute schema 解析改为单点 schema + named reader table：
  - `src/managed/Chaos.IL2CPP.DeclarationDiscovery/Program.cs`

## TDD 记录

### RED

- 更新并新增以下测试，使其先失败：
  - `tests/unit/compatibility/test_chaos_test_framework_contracts.py`
  - `tests/unit/compatibility/test_solution_core_pack_subject.py`
  - `tests/unit/compatibility/test_hot_update_skeleton_subject.py`
  - `tests/unit/compatibility/test_interpreter_mixed_execution_subjects.py`
  - `tests/unit/execution/test_subject_workers.py`
  - `tests/unit/compatibility/test_declared_metadata_discovery_contract.py`

### GREEN

- 定向验证命令 1：

```text
python -m pytest tests/unit/compatibility/test_chaos_test_framework_contracts.py tests/unit/compatibility/test_solution_core_pack_subject.py tests/unit/compatibility/test_hot_update_skeleton_subject.py tests/unit/compatibility/test_interpreter_mixed_execution_subjects.py tests/unit/execution/test_subject_workers.py -k "compact_subject_entry_selection_only or compact_contract_surface or does_not_append_declared_unit_source_entry_argument"
```

- 结果：`5 passed, 48 deselected`

- 定向验证命令 2：

```text
python -m pytest tests/unit/compatibility/test_declared_metadata_discovery_contract.py tests/integration/registry/test_declared_metadata_discovery.py
```

- 结果：`9 passed`

## 扩展回归

- 运行了更宽一层的回归集：

```text
python -m pytest tests/unit/compatibility/test_chaos_test_framework_contracts.py tests/unit/compatibility/test_declared_metadata_discovery_contract.py tests/unit/compatibility/test_solution_core_pack_subject.py tests/unit/compatibility/test_hot_update_skeleton_subject.py tests/unit/compatibility/test_interpreter_mixed_execution_subjects.py tests/unit/execution/test_subject_workers.py tests/integration/registry/test_declared_metadata_discovery.py tests/integration/registry/test_registry_scan.py tests/unit/planning/test_solution_core_pack_planner.py tests/tooling/run/test_subject_command.py
```

- 结果：`98 passed, 2 failed`
- 失败项：
  - `tests/unit/compatibility/test_hot_update_skeleton_subject.py::test_hot_update_project_isolation_and_legacy_solution_cutover`
  - `tests/unit/compatibility/test_interpreter_mixed_execution_subjects.py::test_interpreter_project_isolation_and_legacy_solution_cutover`
- 共同原因：
  - 两条旧断言都要求 `solutions/core/windows/chaos-il2cpp-core.sln` 不存在；该状态与本批 source-entry / discovery cutover 无直接关系，先记录为既有仓库状态。

## 下一步

- 继续盘点 retained subject / registry / planner 中仍残留的 `sourceEntry` / `workloadEntry` / subject-specific string surface。
