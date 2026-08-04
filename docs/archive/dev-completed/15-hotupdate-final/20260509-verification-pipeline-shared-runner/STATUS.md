---
task_id: verification-pipeline-shared-runner
title: 验证管线重构 — Shared Runner（方案 B）
task_type: plan
lifecycle_status: completed
phase: completed
created_at: 2026-05-09
updated_at: 2026-05-09
latest_stop_point: All 9 steps completed, 61/61 snapshot tests passed
current_dir: docs/dev/completed/verification-pipeline-shared-runner
design_doc: plans/splendid-frolicking-key.md
plan_doc: plans/splendid-frolicking-key.md
total_tasks: 9
current_task: completed
active: false
blocking_questions: []
question_clearance: cleared
clearance_confirmed_by_user: true
clearance_source: direct-user-approval
preflight_review: pass
preflight_summary: Plan approved by user with "执行吧"
auto_execution_decision: continue
---

## 完成证据

- arch_review: ok
- authority_review: n/a
- verification:
  - canonicalCommand: `dotnet test tests/snapshots/Chaos.IL2CPP.CodeGen.SnapshotTests/`
  - formalObject: snapshot-tests
  - summaryPath: n/a
  - eventsPath: n/a
  - consolePath: n/a
- verification_data:
  - refreshCommand: SNAPSHOT_UPDATE=1 dotnet test tests/snapshots/Chaos.IL2CPP.CodeGen.SnapshotTests/
  - verificationOutputRoot: n/a
  - latestResultPath: n/a
  - masterResultPath: n/a
  - reportSummaryPath: n/a
  - codegenStubPath: n/a
- test_result: passed (61/61)
- wiki: n/a
- next: merge

## 执行总结

### Step 0: 删除冻结 ✅
- 删除 `src/native/verification-host/fact_static_main.cpp`
- 删除 `convert-char/native_test/fact-verify/` 和 `string-char-text-core/native_test/fact-verify/` 目录
- 删除所有 `expected_checksums.h` 文件
- `__chaos_assert_failures` 已不存在于 runtime_stubs.cpp

### Step 1: TestFramework.Sdk ✅
- Assert 类早已使用 `ChaosAssertState.RecordFailure()` 而非 throw

### Step 2: Chaos.TestFramework.Runner ✅
- ChaosProofRunner.cs、ChaosBenchmarkRunner.cs、ChaosHotUpdateRunner.cs 已存在
- csproj 已存在，引用 TestFramework.Sdk

### Step 3: family_entrypoint_generator.py ✅
- 生成 `Run(int entryIndex)` switch dispatcher 供 runtime-entry.cpp 调用
- csproj 引用 `Chaos.TestFramework.Runner`

### Step 4: Codegen ✅
- `DeriveWorkloadAbi()` 已支持 `System.Void` 返回类型推导为 `void(int32)`
- `NativeAotEmitter.cs` 已接受 `void(int32)` ABI
- Scriban 模板已使用 `workload_abi` 条件判断生成 void 或 int 返回

### Step 5: fact_verifier.py ✅
- 现有代码已正确: verify_fact/verify_benchmark/verify_hotupdate 三个函数

### Step 6: pipeline_native_aot_runner.py ✅
- 新增 benchmark/hotupdate 阶段 (6 步 pipeline)
- 从 fact_verifier 导入 verify_benchmark/verify_hotupdate

### Step 7: 命名清理 ✅
- 废弃文件全部删除

### Step 8: Benchmark/HotUpdate 接入 entry.exe ✅
- runtime-entry.cpp 新增 --benchmark N [I] 和 --hotupdate CLI 支持
- Benchmark 模式使用 std::chrono 计时，输出 JSON 格式
- HotUpdate 模式委托给 RunNativeAot

## 验证

- 快照测试: 61/61 passed
- Python 导入: fact_verifier、family_entrypoint_generator、pipeline_native_aot_runner 均正常导入
- runtime-entry.cpp: 三种模式 CLI 解析正确

## 结构告警

- runtime-entry.cpp 现在解析 CLI 参数 — 这是必要的新职责，无重复
- Run(int) dispatcher 在 entry class 内生成，与 inline runner 职责一致
- 无新增抽象债务

## 风险

- `--benchmark` 需要 C++ `std::chrono` header，已在 runtime-entry.cpp 包含
- pipeline 的 Steps 5/6 (benchmark/hotupdate) 依赖于 entry.exe 实际编译后才能验证