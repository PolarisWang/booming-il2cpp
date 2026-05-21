# 进展记录 v1.04

## 时间

- 2026-04-18 11:21:56 +08:00

## 本轮完成

- 修复 `selection.source.entrySelection` 被错误提升为全局 managed host 规则的回归。
- 新增 `managed-output` 回归测试，锁定 subject entry selection 仍必须构建 source project。
- 重新执行 `managed-runtime-output` 真实命令，确认 host-input 恢复为 source app。
- 完成 `semantic-family-execution-entry-v1-01.json` 首批 27 条 canonical commands 的全量实跑：
  - `14` 条 proof
  - `4` 条 engineering validation
  - `3` 条 engineering workload
  - `6` 条 benchmark
- 完成当前 child task 的架构审视、验证收口与归档准备。

## 当前判断

- 首批 semantic-family execution entry 已从“可生成 contract”推进到“真实 authority 全量实跑通过”。
- 当前 child task 可以归档 `completed`，父 roadmap 可以继续推进 dependency-layer 阶段。
- 后续新增任务应继续使用语义化命名，避免扩散 `Phase{x}` 风格。

## 验证

- `python -m pytest tests/unit/execution/test_subject_workers.py -q -k "test_host_input_build_keeps_source_project_for_subject_entry_selection_managed_output or test_windows_build_target_routes_subject_entry_selection_native_aot_through_benchmark_host_contract or test_runtime_observe_executes_native_aot_benchmark_host_output_for_subject_entry_selection or test_native_proof_emitter_routes_subject_entry_selection_through_native_aot or test_host_input_build_uses_workspace_managed_benchmark_host_for_declared_benchmark or test_host_input_build_uses_workspace_managed_proof_host_for_declared_unit_test"`
  - 结果: `6 passed`
- `python build/toolchains/run/run.py test engineering-validation --id "engineering-validation/SolutionCorePack/managed-runtime-output" --json`
  - 结果: `runId = 20260418-105927-windows-0ea8`，成功运行
- `semantic-family-execution-entry-v1-01.json` 首批 27 条 canonical commands
  - 结果: `20260418-110105-windows-c5c3` 到 `20260418-111909-windows-aea9` 全部通过
- 说明:
  - 本轮未出现 `dotnet build` / `dotnet test` / `msbuild` 崩溃

## Wiki 决策

- 本轮不写 wiki
- 原因: 先把 dependency-layer 阶段推进出来，再判断是否把 execution-entry 实跑经验上升为长期 authority
