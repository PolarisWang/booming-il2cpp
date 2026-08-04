# 20260421-07 Dispatch Reflection HotUpdate Cutover

## 当前状态

- 状态: `completed`
- 完成阶段: `Step 7 CLOSEOUT`
- 当前目标: `20260421-07` 已完成，正式向 `20260421-08-legacy-generic-path-purge` 交接

## 当前结论

- `20260421-07` 的 generated review / generated cpp size-memory evidence 已形成当前批次闭环：
  - `ReflectionInteropClosureEntry` 形式对象 `20260422-221836-windows-6e9e` = `ok`
  - `generatedCppTotalBytes = 14378`
  - `generatedSymbolCount = 1`
  - `peakWorkingSetBytes = 38309888`
  - `matrixProofLinkage.boundaryKinds = [DispatchBoundary, HotUpdateBoundary, ReflectionBoundary]`
  - `proofArtifactCount = 7`
  - `CoreLibHotUpdateProof` 形式对象 `20260422-222036-windows-f713` = `ok`
  - `generatedCppTotalBytes = 27291`
  - `generatedSymbolCount = 0`
  - `peakWorkingSetBytes = 66863104`
  - `nativeReferencePlanKind = assembly-full-closure-runtime-skeleton`
  - `runtimeExecutionKind = assembly-bound-native-reference-skeleton`
  - `runtimeSkeletonUncoveredMethodCount = 0`
  - hotupdate runtime stdout = `corelib-reference-hotupdate:System.Private.CoreLib|System.Runtime|System.Console:16:3`
- `engineering-workload/SolutionCorePack/codegen` 已于 `20260422-222442-windows-9f82` 重跑为 `ok`，补齐当前批次的 `generatedCppTotalBytes / generatedSymbolCount / peakWorkingSetBytes` benchmark evidence
- native-aot audit 入口错配已清零：
  - build/router 现在会从 generated manifest / lowering plan 读取 `nativeEntryFunctionName`
  - proof host / benchmark host 通过 `CHAOS_NATIVE_AOT_ENTRY` 消费真实入口，而不是写死 `RunNativeAot`
  - audit-only generated C++ 显式导出 `RunNativeAotAudit(std::int32_t)`，不再把 audit translation unit 绑到固定入口名
- native-hotupdate runtime-skeleton compile blocker 已清零：
  - runtime-skeleton page 模板补齐 `#include "runtime_core.h"`
  - generated native reference 现在能在当前 compile route 下解析 `chaos::il2cpp::runtime_core::*`
- `AutoBridgeProof` / `BridgeRoundtripProof` 相关 HotUpdate e2e regression 当前仍绿，说明 `advanced carrier` 规则不需要为 child 07 额外追加新的 completion 级 formal object；后续剩余工作转入 `20260421-08`

## 本轮修改文件

- `build/toolchains/run/testing/subject_workers.py`
- `src/native/proof-host/native_aot_main.cpp`
- `src/native/benchmark-host/native_aot_main.cpp`
- `src/managed/Chaos.IL2CPP.CodeGen/Templates/AssemblyFullClosureAuditSummary.cpp.scriban`
- `src/managed/Chaos.IL2CPP.CodeGen/Templates/NativeReferenceProof.RuntimeSkeletonPage.cpp.scriban`
- `tests/unit/execution/test_subject_workers_build_native_aot_entry_routing.py`
- `tests/unit/compatibility/test_full_assembly_closure_codegen_audit_plan.py`
- `tests/unit/compatibility/test_full_assembly_closure_codegen_contracts_runtime_templates.py`

## 文件审计

- `build/toolchains/run/testing/subject_workers.py`
- `src/native/proof-host/native_aot_main.cpp`
- `src/native/benchmark-host/native_aot_main.cpp`
- `src/managed/Chaos.IL2CPP.CodeGen/Templates/AssemblyFullClosureAuditSummary.cpp.scriban`
- `src/managed/Chaos.IL2CPP.CodeGen/Templates/NativeReferenceProof.RuntimeSkeletonPage.cpp.scriban`
- `src/managed/Chaos.IL2CPP.HotUpdate/AutoBridgeGenerator.cs`
- `src/managed/Chaos.IL2CPP.HotUpdate/BridgeCarrierSchema.cs`
- `tests/unit/compatibility/test_hot_update_e2e_flows.py`

## 架构审视

- arch_review_mode: `critical`
- 结论: `blocker-fixed`
- authority_review: `ok`
- duplicate_code:
  - native-aot 入口选择仍停留在 build/router + host main 两层，没有把入口判定复制到 proof host / benchmark host / generated artifact 三处
  - audit-only `RunNativeAotAudit` stub 只承担链接契约，不回流业务语义到 host main
- structure:
  - runtime-skeleton compile blocker 通过补头文件解决，没有把 `runtime_core` 解析逻辑硬编码回生成器或 runner
  - `subject_workers.py` 继续负责 manifest 路由、CMake 参数和 build manifest 回写；入口 authority 未扩散到 reporting 层
- warn:
  - `NativeReferenceProof.RuntimeSkeletonPage.cpp.scriban` 仍然是大模板文件，但本轮修复只补了缺失 include，没有额外引入新的分支债务
- 权责图审核:
  - authority: `nativeEntryFunctionName` 以 lowering plan / generated manifest 为准；runtime-skeleton compile authority 以 generated page 模板的真实 include 集合为准
  - upstream: `NativeAotLoweringPlanner` / generated manifest / runtime skeleton templates
  - downstream: subject build manifest、proof host / benchmark host CMake 定义、formal object build / runtime
  - allowed_deps: build/router -> manifest/plan -> host main compile define；runtime template -> `runtime_core.h`
  - shim_debt: `RunNativeAotAudit` 是 audit-only shim，用于保持 audit translation unit 可链接运行；不应扩展为常规 runtime entry 模式
  - proof: latest unit regression、HotUpdate e2e、reflection proof、hotupdate proof、codegen workload 均为 `ok`

## 完成证据

- arch_review: blocker-fixed
- authority_review: ok
- verification.reflection.canonicalCommand: `python -m build.toolchains.run.run test declared-unit-test --id "declared-unit-test/SolutionCorePack::CoreRuntimeFeatures::CoreRuntimeFeatures.ReflectionInteropClosureEntry::Run()" --matrix windows-reflection-interop-closure-native-proof`
- verification.reflection.formalObject: `[native generic proofs, generated code review]`
- verification.reflection.summaryPath: `artifacts/subjects/SolutionCorePack/runs/20260422-221836-windows-6e9e/declared/unit/reflection-interop-closure-proof/summary.json`
- verification.reflection.eventsPath: `artifacts/subjects/SolutionCorePack/runs/20260422-221836-windows-6e9e/run-report/events.jsonl`
- verification.reflection.consolePath: `artifacts/subjects/SolutionCorePack/runs/20260422-221836-windows-6e9e/run-report/console.log`
- verification.hotupdate.canonicalCommand: `python -m build.toolchains.run.run test declared-unit-test --id "declared-unit-test/SolutionCorePack::GoldenCoreLibReference.HotUpdatePatch::GoldenCoreLibReference.HotUpdatePatch.CoreLibHotUpdateProof::Run()" --matrix windows-corelib-reference-native-hotupdate-proof`
- verification.hotupdate.formalObject: `[hotupdate generic boundary proofs, generated code review]`
- verification.hotupdate.summaryPath: `artifacts/subjects/SolutionCorePack/runs/20260422-222036-windows-f713/declared/unit/corelib-reference-hotupdate-proof/summary.json`
- verification.hotupdate.eventsPath: `artifacts/subjects/SolutionCorePack/runs/20260422-222036-windows-f713/run-report/events.jsonl`
- verification.hotupdate.consolePath: `artifacts/subjects/SolutionCorePack/runs/20260422-222036-windows-f713/run-report/console.log`
- verification.codegen.canonicalCommand: `python -m build.toolchains.run.run test engineering-workload --id engineering-workload/SolutionCorePack/codegen`
- verification.codegen.formalObject: `[generated cpp size/memory benchmark]`
- verification.codegen.summaryPath: `artifacts/subjects/SolutionCorePack/runs/20260422-222442-windows-9f82/engineering/workloads/codegen/summary.json`
- verification.codegen.eventsPath: `artifacts/subjects/SolutionCorePack/runs/20260422-222442-windows-9f82/run-report/events.jsonl`
- verification.codegen.consolePath: `artifacts/subjects/SolutionCorePack/runs/20260422-222442-windows-9f82/run-report/console.log`
- test_result: passed
- wiki: n/a
- next: handoff to `20260421-08-legacy-generic-path-purge`

## 验证

- `python -m pytest tests/unit/compatibility/test_full_assembly_closure_codegen_contracts_runtime_templates.py tests/unit/compatibility/test_full_assembly_closure_codegen_audit_plan.py tests/unit/execution/test_subject_workers_build_native_aot_entry_routing.py tests/unit/execution/test_subject_workers_build_native_aot_contracts_core.py -q`
  - 结果: `22 passed`
- `python -m pytest tests/unit/compatibility/test_hot_update_e2e_flows.py -q`
  - 结果: `8 passed`
- `python -m build.toolchains.run.run test declared-unit-test --id "declared-unit-test/SolutionCorePack::CoreRuntimeFeatures::CoreRuntimeFeatures.ReflectionInteropClosureEntry::Run()" --matrix windows-reflection-interop-closure-native-proof`
  - 结果: `ok`
  - run id: `20260422-221836-windows-6e9e`
- `python -m build.toolchains.run.run test declared-unit-test --id "declared-unit-test/SolutionCorePack::GoldenCoreLibReference.HotUpdatePatch::GoldenCoreLibReference.HotUpdatePatch.CoreLibHotUpdateProof::Run()" --matrix windows-corelib-reference-native-hotupdate-proof`
  - 结果: `ok`
  - run id: `20260422-222036-windows-f713`
- `python -m build.toolchains.run.run test engineering-workload --id engineering-workload/SolutionCorePack/codegen`
  - 结果: `ok`
  - run id: `20260422-222442-windows-9f82`

## 下一步

- 在父级 roadmap 中把 `20260421-07` 标记为 `completed`
- 启动 `20260421-08-legacy-generic-path-purge`
- 优先清理 clone/materialization 主路径、散落的 generic fallback logic 与残余 legacy authority route

## 父级路线对齐

- 父级 roadmap: `docs/dev/in-progress/20260421-02-canonical-generic-runtime-kernel-roadmap/STATUS.md`
- 当前 child 已从 `docs/dev/in-progress/` 迁移到 `docs/dev/completed/`
- `recommended_next_child` 应切到 `20260421-08-legacy-generic-path-purge`
