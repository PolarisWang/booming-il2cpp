# 2026-04-22 02:03:37 +08:00 demand-graph artifact cutover

## 结论

- `GenericInstantiationDemandGraph` 已从 loader 内部 contract 提升为 managed-closure 正式 artifact：
  - 新 artifact 名: `generic-instantiation-demand-graph.json`
  - `ManagedClosureResult` / `closure.manifest.json` / `DriverEntry` 已贯通
  - analysis frontend manifest 已显式暴露 `genericInstantiationDemandGraphPath`
- 这一步的价值不是“多一个 JSON”，而是把后续 `20260421-05` 所需的 canonical generic input 从内存透传变成可 review、可 diff、可持久化的 authority candidate

## 验证

- `python -m pytest tests/unit/compatibility/test_loader_instantiation_demand_graph_managed_closure_artifact.py tests/unit/compatibility/test_managed_closure_contract_bundle.py tests/unit/execution/test_subject_workers_frontend_codegen_analysis.py -q`
- `python -m pytest tests/unit/compatibility/test_loader_instantiation_demand_graph_contract.py tests/unit/compatibility/test_loader_instantiation_demand_graph_pipeline_contract.py tests/unit/compatibility/test_full_assembly_closure_without_entry_point.py tests/unit/compatibility/test_core_contract_cutover.py -q`
- `dotnet build src/managed/Chaos.IL2CPP.Driver/Chaos.IL2CPP.Driver.csproj -c Release -m:1`

## 剩余问题

- `GenericCompatibilityProjection` 仍然存在，说明 loader 还没有彻底 hard-cut 到 graph-first authority
- downstream AOT IR/runtime 仍未消费 graph 作为唯一 generic execution authority
- 下一刀应该优先找出 compatibility projection 被哪些 stage 真正依赖，然后逐步削薄
