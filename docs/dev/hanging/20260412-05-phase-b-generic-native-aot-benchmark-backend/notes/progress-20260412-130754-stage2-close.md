# 2026-04-12 13:07:54 Stage 2 收口

- BenchArithmetic manifest 已冻结 `workloadEntry`，并把 `validation.perf.project` 指向外置 perf harness。
- subject planner 已把 `workloadEntry` 纳入 selection 与 stage fingerprint，避免 workload boundary 变化被错误复用。
- managed / interpreter perf worker 统一通过 harness 传递 `--assembly --workload-entry --mode`，runtime manifest 也补齐 workload / harness 元数据。
- benchmark command 已让 managed / interpreter 走通用 subject pipeline；native 仍保留 `_run_native_benchmark_pipeline()` 包装层。
- 验证:
  - `python -m pytest tests/unit/run/test_benchmark_subject_sources.py tests/unit/run/test_subject_planner.py tests/unit/run/test_subject_workers.py tests/unit/run/test_benchmark_command.py tests/unit/run/test_phase_b_aot_contract_split.py -q`
  - `python -m pytest tests/unit/run/test_phase_b_aot_contract_split.py tests/unit/run/test_benchmark_subject_sources.py tests/unit/run/test_subject_workers.py tests/unit/run/test_subject_executor.py tests/unit/run/test_managed_closure_contract_bundle.py tests/unit/run/test_benchmark_command.py tests/unit/run/test_subject_planner.py -q`
  - `dotnet build subjects/BenchArithmetic/validation/perf/BenchArithmetic.Subject.PerfHarness/BenchArithmetic.Subject.PerfHarness.csproj -c Release`
  - `dotnet build src/managed/Chaos.IL2CPP.Driver/Chaos.IL2CPP.Driver.csproj -c Release -m:1`
- 下一步: 开始 Stage 3，最小化实现 `BenchArithmetic` 的 generic native AOT backend MVP，避免回退到 proof family。
