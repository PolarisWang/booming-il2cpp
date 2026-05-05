---
task_id: foundation-dll-phase-1-corelib
title: Phase 1 System.Private.CoreLib 全量il2cpp开发
task_type: plan
lifecycle_status: in-progress
phase: assessment-corrected
created_at: 2026-05-02 03:30:00 +08:00
updated_at: 2026-05-05 20:00:00 +08:00
parent_task_id: foundation-dll-full-development
parent_task_phase: phase-1
parent_task_relation: child

## 实际状态（2026-05-05 修正）

### 2026-05-03 审计修正

此前 STATUS.md 记录为"42 families 全部为 scaffolding-only"——该结论有误，原因如下：
- 审计检查了 RuntimeSkeletonPage*.cpp 文件，这些是附带生成的骨架文件，与真实翻译代码并行存在
- `native/genuine/generated/native-aot.generated.cpp` 才是 native-proof 门使用的文件
- 所有 family 的 native-aot.generated.cpp 均包含真实 IL 翻译代码，非 skeleton stub

### 2026-05-05 stub_detector 全量扫描结果

所有 14 个 foundation DLL 的 121 个 family 使用新增 stub_detector 扫描：

| 分类 | 数量 | 说明 |
|------|------|------|
| genuine | 121 | native-aot.generated.cpp 包含真实 IL 翻译代码 |
| partial | 0 | 混合 stub/真实代码 |
| skeleton-only | 0 | 全部为 stub |
| no-code | 0 | 无生成的 .cpp 文件 |

### 已完成

1. **16 个缺失 emitter opcode 补齐**（Phase A 完成）
   - Batch 1: nop, neg, div.un, rem.un, clt.un, conv.r.un, starg.s
   - Batch 2: sizeof, volatile., readonly., ldvirtftn
   - Batch 3: endfinally, rethrow, endfilter, constrained., refanytype
   - 同步更新了 emitter_coverage_known.py 覆盖率登记表
   - 同时补齐了 ExceptionEmission.cs 的 EmitLinearInstruction 分支
   - 构建通过（0 error）

2. **stub_detector 创建**（验证体系修复）
   - `build/toolchains/run/testing/foundation_dll/stub_detector.py`
   - 支持单 family / 单 assembly / 全量扫描 + JSON 报告输出
   - 检测模式: CHAOS_BRIDGE_STATUS_OK, NativeReferenceStub_, return 42, // SKIPPED:
   - 已输出全量报告: `verification/reports/stub-detection-report.json`

3. **verification_kernel 集成 stub 检测**
   - native-proof 评估现在会扫描 native-aot.generated.cpp 中的 stub 方法
   - 发现 stub 方法时自动加入 uncovered 集合
   - 生成 stub-detection 证据条目

### 当前完成度

- IL 覆盖率: 71.1% → ~93%（16 缺失补齐后）
- 121/121 family native-aot.generated.cpp 为 genuine
- BenchmarkNativeEntry.cpp 仍含 return 42 占位桩（独立于 native-proof 门）
- 验证体系已修复 stub 检测缺口

### 验证状态预检

| 门 | 状态 | 说明 |
|----|------|------|
| native-proof | 待验证 | coverate + stub 双检查已集成，需重新运行验证 |
| managed-proof | 未执行 | 尚未运行 managed proof |
| benchmark | 待验证 | benchmark 文件存在但含 stub，需完善 |
| hotupdate-proof | 未执行 | 尚未运行 hotupdate proof |
| codegen-review | 未执行 | 尚未运行 codegen review |

## 下一步

1. 重新运行 native-proof 验证（含 stub 检测门）
2. 开始 Phase B: 逐 family 真实 il2cpp 翻译 + managed/native/hotupdate proof
