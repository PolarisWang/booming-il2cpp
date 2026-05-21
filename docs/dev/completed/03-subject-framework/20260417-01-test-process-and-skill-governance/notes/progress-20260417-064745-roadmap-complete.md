# 2026-04-17 06:47:45 +08:00 收口记录

## 本轮完成

- 完成 `workspace_declared_catalog.py -> workspace_declared_collection.py` 重命名，并同步 benchmark command、dashboard generator、subject workers 与对应单测。
- 清理当前代码与测试里残留的 `workspace catalog` / `declared catalog` 用户可见文案，统一到 `collection` 口径。
- 重新验证 HotUpdate proof / benchmark host 的真实 CLI 链路，确认 `collection + binding manifest` 主线稳定。

## 验证结果

- `python -m pytest tests/unit/planning/test_project_workspace.py tests/unit/planning/test_generated_managed_hosts.py tests/unit/planning/test_generated_hotupdate_hosts.py tests/unit/execution/test_subject_workers.py tests/unit/performance/test_workspace_declared_collection.py tests/unit/performance/test_benchmark_command.py tests/unit/performance/test_benchmark_dashboard_generator.py tests/unit/performance/test_subject_workers_perf.py tests/tooling/run/test_subject_command.py tests/integration/registry/test_declared_metadata_discovery.py tests/integration/run/test_perf_harness_hotupdate_benchmark_host.py tests/unit/compatibility/test_phase5_hotupdate_host_pack_declared_proof.py -q`
  - `134 passed`
- `python -u build/toolchains/run/run.py test declared-benchmark --id "declared-benchmark/HotUpdateHostPack::HotUpdateHostPack::HotUpdateHostPack.HotUpdateLoadBenchmarkEntry::RunWorkload()" --json`
  - `final-summary.status = ok`
- `python -u build/toolchains/run/run.py test declared-unit-test --id 'declared-unit-test/HotUpdateHostPack::HotUpdateHostPack::HotUpdateHostPack.HotUpdateSkeletonProofEntry::Run()' --json`
  - `final-summary.status = ok`

## 备注

- PowerShell 下手工重跑包含 `()` 的 declared proof id 时，使用单引号最稳妥；这次出现的 object-not-found 不是 registry 回归，而是 shell 传参差异。
- `SolutionCorePack` 的 `generic-analysis-only` native proof emission 阻塞继续留在 AOT roadmap，不回流到本测试治理任务。
