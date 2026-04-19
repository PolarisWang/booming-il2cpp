# Complex BCL And Common External DLL Lane 实施计划 v1.01

Status: completed

## AOT/IL2CPP/Test Governance Intake

- capabilityFamily: `FrameworkPackNativeization`
- capabilityItem: `ComplexBclAndCommonExternalDllLane`
- ownerSubjectId: `SolutionCorePack`
- proofRequired: `true`
- benchmarkRequired: `true`
- hotupdateImpact: `None`
- requiredGates: `collector -> registry -> workspace -> dependency-layer-proof -> benchmark`

## 计划清单

- [x] 步骤 1: 让 `dependency-layer` generator 消费已认证程序集输入，并用测试锁定 layer advancement 行为
- [x] 步骤 2: 使用 `core-bcl-layer-01 29/29 ok` summary 重新生成 artifacts，使 `core-bcl-layer-02` 成为 first ready layer
- [x] 步骤 3: 生成 `core-bcl-layer-02-scope-v1-01.json`
- [x] 步骤 4: 执行当前 layer 的 proof / benchmark / engineering validation 去重集合
- [x] 步骤 5: 生成 proof / benchmark summary
- [x] 步骤 6: 运行 `dependency-layer-certify`，生成 `13` 个 DLL 的 `assembly-certification-report/*.json`
- [x] 步骤 7: 生成 `layer-certification-summary-v1-01.json` 并更新 child / parent / ACTIVE / index

## 预期验证命令

- `dotnet build src/tools/Chaos.IL2CPP.Tools.FoundationPack.Analysis/Chaos.IL2CPP.Tools.FoundationPack.Analysis.csproj -c Release`
- `python -m pytest tests/tooling/foundation/test_dependency_layer_plan.py -q`
- `python build/toolchains/run/run.py test registry refresh --json`
- `Get-Content docs/dev/in-progress/20260418-07-complex-bcl-and-common-external-dll-lane/layer-plan-v1-01.json -Raw | ConvertFrom-Json | Out-Null`
- `Get-Content docs/dev/in-progress/20260418-07-complex-bcl-and-common-external-dll-lane/proof-benchmark-lane-v1-01.json -Raw | ConvertFrom-Json | Out-Null`
- `Get-ChildItem docs/dev/in-progress/20260418-07-complex-bcl-and-common-external-dll-lane/assembly-nativeization-plan/*.json | ForEach-Object { Get-Content $_.FullName -Raw | ConvertFrom-Json | Out-Null }`

## 收口条件

- `core-bcl-layer-02` 的跨版本并集 `13` 个程序集都有逐 DLL certification report。
- 全部 report `finalStatus = ok`。
- 当前 child 不声称 `managed -> native -> hotupdate` 全链路完成。
