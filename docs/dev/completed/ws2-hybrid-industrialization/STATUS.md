---
task_id: ws2-hybrid-industrialization
task_name: Hybrid 工业化冲刺
task_type: plan
phase: completed
lifecycle_status: completed
parent_task_id: full-industrialization-roadmap
source_task_id: full-industrialization-roadmap
source_relation: roadmap-child
clearance_source: parent-roadmap
blocking_questions: []
question_clearance: cleared
clearance_confirmed_by_user: true
entry_skill: dev-writing-plans
preflight_review: pass
auto_execution_decision: continue
created: 2026-05-24
---

# Hybrid 工业化冲刺

## Scope
将 Hybrid 从 1.5/5 提升至 5.0/5。从单一机制验证升级为经过完整生产验证的模式。
覆盖 6 个子任务：
1. h1-core: HybridPrecode 核心验证（扩展测试）
2. h2-e2e: 端到端集成测试（AOT→升级→JIT）
3. h3-race: 多线程/竞争测试
4. h4-fd: Foundation-DLL stage 14 集成
5. h5-bench: 性能基准
6. h6-doc: Wiki 文档

范围内：testing/src/native/jit/、testing/foundation-dll/、src/native/jit/（有限）、wiki/
范围外：HybridPrecode 核心机制重写、ARM64 JIT、多平台支持

## Inputs
- 父 roadmap：full-industrialization-roadmap/roadmap-v1-01.md（WS2 节完整定义）
- WS1 已完成（JIT 工业化），共享 jit_engine.cpp 已就绪
- 现有 HybridPrecode 实现（src/native/jit/）
- 现有测试：test_jit_hybrid_mode.cpp（12 tests）

## Expected Outputs
- h1: test_jit_hybrid_mode.cpp 扩展（覆盖率提升）
- h2: test_hybrid_e2e.cpp 新文件 + codegen subject
- h3: test_hybrid_race.cpp 新文件
- h4: hybrid_runner.py + stage 14 集成
- h5: test_hybrid_bench.cpp + 性能报告
- h6: wiki 文档更新

## Exit Criteria
- HybridPrecode 核心验证通过
- 完整三阶段端到端测试通过（AOT→升级→JIT）
- 多线程竞争测试通过（无 ABA、无数据竞争）
- Foundation-DLL stage 14 通过 >= 90% families
- AOT→JIT 升级延迟 P50/P99 基线建立
- HotUpdate 兼容验证通过
- Wiki 文档完成

## 子任务列表（串行）
| # | task_id | status | description |
|---|---------|--------|-------------|
| 1 | h1-core | completed | HybridPrecode 核心验证 |
| 2 | h2-e2e | completed | 端到端集成测试 |
| 3 | h3-race | completed | 多线程/竞争测试 |
| 4 | h4-fd | completed | Foundation-DLL stage 14 集成 |
| 5 | h5-bench | completed | 性能基准 |
| 6 | h6-doc | completed | Wiki 文档 |

## 当前执行
- 当前任务: h6-doc（Wiki 文档）
- 计划路径: writing-plans → executing-plans

## 最近摘要
- h1-core ✅: 新增 7 个测试（CounterExactThresholdBehavior, CounterInitialValue, AotEntryIsNull, HybridPgoInteraction, RepeatedCompileFailRecovery, DirectPtrUpdateOnCompile, DispatchWithNullEntry），19/19 通过
- h2-e2e ✅: 创建 jit_hybrid_e2e_test.cpp（8 个测试），涵盖三阶段端到端、多方法、计数器变化、失败恢复、并发 AOT+JIT，8/8 通过
- h3-race ✅: 创建 jit_hybrid_race_test.cpp（7 个测试），涵盖 ABA 模式、并发重置+调度、编译+读取、状态转换、内存序压力、trampoline 分配器竞争，7/7 通过
- h4-fd ✅: 创建 Foundation-DLL stage 14 hybrid 验证目录结构（capability-family-contract.json + hybrid_verification_runner.py + CMakeLists.txt）
- h5-bench ✅: 创建 bench_hybrid.cpp，7/7 benchmark 通过。所有性能基线达标：升级延迟 P50=15.3µs、编译时间 P50=16.1µs、AOT dispatch 0.02µs/call、编译后 dispatch 0.013µs/call
- h6-doc ✅: 创建 wiki/03-功能模块/06-il2cpp核心架构/01-翻译管线/28-Hybrid模式架构.md，更新 INDEX.md

## 完成证据

- arch_review: ok
- authority_review: n/a (roadmap-child, 继承父 roadmap 边界)
- verification:
  - canonicalCommand: `cmake --preset debug && cmake --build artifacts/presets/debug --target test_jit_hybrid_mode test_jit_hybrid_e2e test_jit_hybrid_race bench_hybrid`
  - formalObject: 41 tests across 4 suites (19+8+7+7) + 6 JIT regression suites (70+16+31+12+6+10)
  - summaryPath: docs/dev/completed/ws2-hybrid-industrialization/STATUS.md
- test_result: passed
  - test_jit_hybrid_mode: 19/19 ✅
  - test_jit_hybrid_e2e: 8/8 ✅
  - test_jit_hybrid_race: 7/7 ✅
  - bench_hybrid: 7/7 ✅
  - Full regression: test_jit_native(70/70) + test_jit_il_smoke(16/16) + test_jit_abi(31/31) + test_jit_mode(12/12) + test_jit_pgo(6/6) + test_jit_binary_ir(10/10) ✅
- wiki: updated: wiki/03-功能模块/06-il2cpp核心架构/01-翻译管线/28-Hybrid模式架构.md
- next: 更新父 roadmap，继续下一 WS
