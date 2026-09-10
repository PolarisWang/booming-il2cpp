# 20260421-06 Generic Matrix And IR Gates

## 当前状态

- 状态: `completed`
- 完成阶段: `Step 4 CLOSEOUT`
- 当前目标: `20260421-06` 已收口完成，正式交接给 `20260421-01` async generic family cutover

## 当前结论

- managed closure 已稳定产出 `generic-capability-matrix.json`，并且不再用字符串猜测 boundary，改为显式 `boundaryCases`
- `DispatchBoundary` / `ReflectionBoundary` / `HotUpdateBoundary` 现在基于真实 case 计数进入 matrix，而不是靠 emitter 命名约定推断
- native emit 阶段现在固定写出 codegen metrics sidecar：
  - `native-reference.codegen-metrics.json`
  - `native-aot.codegen-metrics.json`
- generated 阶段会按 subject 读取 baseline，并对以下指标做 gate：
  - `generatedCppTotalBytes`
  - `generatedSymbolCount`
  - `peakWorkingSetBytes`
- generated manifest 已暴露：
  - `codegenMetricsPath`
  - `codegenBaselineComparePath`
  - `codegenRegressionStatus`
- reporting 现在会稳定落盘：
  - `pipeline-report/report/codegen-summary.json`
  - `pipeline-report/report/codegen-baseline-compare.json`
  - `pipeline-report/report/codegen-metrics.json`
  - `pipeline-report/report/generic-matrix-proof-linkage.json`
- `subject summary` 现在会上卷：
  - `codegenMetrics`
  - `codegenRegressionStatus`
  - `codegenReportPaths`
  - `matrixProofLinkage`

## 本轮修改文件

- `src/managed/Chaos.IL2CPP.Contracts/ManagedClosureArtifactModels.cs`
- `src/managed/Chaos.IL2CPP.CodeGen/GenericCapabilityMatrixBuilder.cs`
- `src/managed/Chaos.IL2CPP.CodeGen/NativeCodegenMetricsBuilder.cs`
- `src/managed/Chaos.IL2CPP.CodeGen/NativeAotEmitter.cs`
- `src/managed/Chaos.IL2CPP.CodeGen/NativeReferenceProofEmitter.cs`
- `src/managed/Chaos.IL2CPP.CodeGen/ReferenceProof/NativeReferenceLoweringPlanner.EngineAndFamilySelection.cs`
- `src/managed/Chaos.IL2CPP.Driver/DriverEntry.cs`
- `build/toolchains/run/testing/path_resolver.py`
- `build/toolchains/run/testing/perf.py`
- `build/toolchains/run/testing/subject_workers.py`
- `build/toolchains/run/testing/subject_reporting.py`
- `tests/unit/compatibility/test_phase4c_generic_matrix_gates.py`
- `tests/unit/compatibility/test_phase5_virtual_dispatch_native_aot.py`
- `tests/unit/execution/test_subject_workers_frontend_codegen_emitter.py`
- `tests/unit/performance/test_subject_perf_policy.py`
- `tests/unit/reporting/test_subject_reporting_codegen_metrics.py`
- `tests/unit/reporting/test_subject_reporting_matrix_proof_linkage.py`
- `tests/unit/reporting/test_subject_reporting_summary.py`

## 架构审视

- `generic-capability-matrix.json` 继续承担“语义覆盖与 authority gate”的职责；native codegen metrics 则拆成 emitter sidecar，避免把运行期构建成本重新塞回 managed-closure-only contract
- codegen metrics 的 authority 放在 native emit 阶段，baseline compare 放在 Python generated/reporting 层，职责边界更清晰：
  - emitter 只负责产出事实
  - generated 负责对比 baseline
  - reporting 负责物化 summary/artifact
- `boundaryCases` 真值链已经建立，matrix row 的边界覆盖不再依赖命名启发式
- matrix case 与 proof artifact 的正式关联放在 reporting 层，而不是回写 bundle 期 sidecar；这样 managed closure 继续只产出语义事实，proof/run 证据由 reporting 负责挂接

## 生成产物审视

- `generic-capability-matrix.json` 现在可直接看到真实 boundary case 计数
- generated manifest 已带出 codegen gate 所需路径和回归状态
- pipeline report 已能稳定审阅 codegen baseline compare 与 metrics 明细
- pipeline report 已能把 `boundaryCases` 与本次 matrix run 的 generated/build/runtime 证据正式挂接
- `subject summary` 已具备消费 codegen gate 与 matrix-proof linkage 的稳定字段

## 验证

- `python -m pytest tests/unit/execution/test_subject_workers_frontend_codegen_emitter.py tests/unit/performance/test_subject_perf_policy.py tests/unit/reporting/test_subject_reporting_codegen_metrics.py -q`
  - 结果: `7 passed`
- `python -m pytest tests/unit/execution/test_subject_workers_frontend_codegen_emitter.py tests/unit/performance/test_subject_perf_policy.py tests/unit/reporting/test_subject_reporting_codegen_metrics.py tests/unit/reporting/test_subject_reporting_matrix_perf.py -q`
  - 结果: `8 passed`
- `python -m pytest tests/unit/compatibility/test_phase4c_generic_matrix_gates.py tests/unit/compatibility/test_phase5_virtual_dispatch_native_aot.py -q`
  - 结果: `11 passed`
- `python -m pytest tests/unit/reporting/test_subject_reporting_matrix_proof_linkage.py tests/unit/reporting/test_subject_reporting_codegen_metrics.py tests/unit/reporting/test_subject_reporting_artifacts.py tests/unit/reporting/test_subject_reporting_summary.py -q`
  - 结果: `9 passed`
- `dotnet build-server shutdown`
  - 结果: 成功
- `dotnet build src/managed/Chaos.IL2CPP.Driver/Chaos.IL2CPP.Driver.csproj -c Release -m:1`
  - 结果: `0 Error(s)`，存在既有 nullable warnings，不是本轮新引入问题

## 下一步

- 切回 `20260421-01-aot-generic-task-lowering-normalization`
- 利用 `20260421-06` 已冻结的 matrix/gate/proof-linkage 基线继续 async generic family cutover
- 待 `20260421-01` 收口后，顺序创建 `20260421-07` / `20260421-08`

## 残余风险

- 当前 codegen gate 仍只覆盖 `generatedCppTotalBytes`、`generatedSymbolCount`、`peakWorkingSetBytes` 三项，还不是完整的 native generated review
- `subject summary` 已上卷 matrix/codegen 关键信息，但更高层 dashboard 还没有专门消费这些字段
