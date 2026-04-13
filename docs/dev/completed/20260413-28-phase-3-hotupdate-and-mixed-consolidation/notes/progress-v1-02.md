# 进度记录 v1.02

Date: 2026-04-13 12:14:06 +08:00

## 本次完成

- 为 `HotUpdateHostPack` 保持 retained solution 模式并完成真实 proof / benchmark 闭环。
- 为 `MixedExecutionFeaturePack` 补齐 native benchmark 收口路径：
  - 新增 `subjects/MixedExecutionFeaturePack/source/MixedExecutionNativeBenchmark.cs`
  - 将 `windows-native-perf` 的 `source.entry` 与 `workloadEntry` 指向 native-only entry
  - 保留顶层 `workloadEntry` 继续指向 managed benchmark entry
- 扩充 `src/managed/Chaos.IL2CPP.Loader/LoaderStage.cs` 对以下 opcode/语义的支持：
  - `switch`
  - `constrained`
  - token-based `ldelem` / `stelem`
  - `unbox` / `unbox.any`
- 新增回归测试 `tests/unit/compatibility/test_loader_switch_opcode.py`
- 更新与 retained benchmark entry/declared metadata 相关的回归：
  - `tests/unit/performance/test_benchmark_subject_sources.py`
  - `tests/unit/planning/test_subject_planner.py`
  - `tests/integration/registry/test_declared_metadata_discovery.py`

## 验证结果

- 定向回归：
  - `python -m pytest tests/unit/compatibility/test_loader_switch_opcode.py tests/unit/compatibility/test_hot_update_skeleton_subject.py tests/unit/compatibility/test_hot_update_e2e_flows.py tests/unit/compatibility/test_interpreter_mixed_execution_subjects.py tests/unit/performance/test_benchmark_subject_sources.py tests/unit/planning/test_subject_planner.py tests/unit/compatibility/test_subject_manifest_schema.py tests/integration/registry/test_declared_metadata_discovery.py -q`
  - 结果：`103 passed`
- 真实运行：
  - `HotUpdateHostPack` proof: `20260413-hotupdatehostpack-managed-proof`
  - `HotUpdateHostPack` benchmark: `20260413-hotupdatehostpack-managed-perf`
  - `MixedExecutionFeaturePack` proof: `20260413-mixedexecutionfeaturepack-managed-proof`
  - `MixedExecutionFeaturePack` managed benchmark: `20260413-mixedexecutionfeaturepack-managed-perf-rerun`
  - `MixedExecutionFeaturePack` interpreter benchmark: `20260413-mixedexecutionfeaturepack-interpreter-perf-rerun`
  - `MixedExecutionFeaturePack` native benchmark: `20260413-mixedexecutionfeaturepack-native-perf-rerun-native-entry`

## 当前结论

- Phase 3 已经不再缺“能声明”的 retained subject；当前三个 retained subject 都具备真实执行证据。
- `MixedExecutionFeaturePack` 的 native benchmark 收口采用了“矩阵级 native-only entry”而不是扩展通用 AOT benchmark backend，这与父 roadmap 当前边界保持一致。
- 后续主线应转入统一入口与 registry cutover，而不是继续在 Phase 3 范围内扩大 backend 范围。

## 下一步建议

- 激活 Phase 4，先审计三个 retained subject 的 launcher / manifest / registry 暴露差异。
- 以 typed dispatch 为主线统一 `source.entry`、matrix `source.entry`、`workloadEntry` 与 selector/CLI 的默认行为。
