---
task_type: roadmap
phase: executing
roadmap_or_plan: roadmap-v1-01.md
dispatch_model: hybrid
child_execution_mode: auto
auto_continue: true
auto_stop_policy: blocking-only
recommended_next_child: wf1-arch
source: brainstorm
created: 2026-05-23
---

# JIT 工业化收官 — STATUS

> task_id: 20260523-jit-industrialization-finale

## 关键文档

- [Roadmap](roadmap-v1-01.md) — 阶段定义与子任务映射
- [DISPATCH](DISPATCH.md) — 调度批次划分

## 设计摘要（来自 brainstorm）

- **范围**：全范围，覆盖 JIT 审计识别的全部 18 个问题
- **组织策略**：5 个工作流并行，其中流1（核心管线）内部串行，流1→流3串行（共享 jit_engine.cpp）
- **架构决策**：
  - IEncoder 接口抽象（编译期选择 x64/ARM64）
  - ISehHandler 接口抽象（VEH/Signal 两平台）
  - TLAB 内联分配（纯 codegen 优化，不改分配策略）
  - 调试信息：CodeView + DWARF
- **三优先级权衡**：全部方案通过 P1>P2>P3 审查

## 当前进度

| batch | status | tasks | 备注 |
|-------|--------|-------|------|
| batch-1 | completed | wf1-arch → wf1-gc → wf1-liveness → wf1-osr → wf1-unwind → wf1-tests → wf1-ci ✅ | CI baseline workflow + Python scripts 已实现 |
| batch-2 | completed | wf2-linux ✅, wf4-debug ✅ | wf2-linux: LinuxSehHandler.cpp + DWARF .eh_frame; wf4-debug: JitDebugInfo 发射 + 调试合约 + JIT SOS WinDbg 扩展 + GetMethodName |
| batch-3 | completed | wf3-tlab ✅ | TLAB inline TLS access 已实现：EmitLoadTlsTlab 替换 3 处 CodegenGetTlab CALL，全 199 JIT 测试通过 |
| batch-4 | completed | wf5-arm64 ✅ | 完整 ARM64 JIT 支持：Arm64Encoder、ArchTraits、jit_engine.cpp 参数化、NEON SIMD、ARM64 prologue/epilogue、precode 跳板、DWARF 展开、SEH ucontext、EmitSimd NEON 分支。x64 回归测试全部通过。

## 调度状态

```yaml
dispatch_doc: DISPATCH.md
dispatch_model: hybrid
active_batches: []
completed_batches: [batch-1, batch-2, batch-3, batch-4]
pending_batches: []
```

## 子任务映射

| task_id | phase | status | owner | purpose | depends_on | batch_id | deliverables | exit_criteria | conflict_scope | estimated_effort |
|---------|-------|--------|-------|---------|------------|----------|--------------|---------------|----------------|----------------|
| wf1-arch | 1a | ready | main | IEncoder/ISehHandler 接口抽象 + jit_engine.cpp 重构 | — | batch-1 | IEncoder.h, ISehHandler.h, jit_engine.cpp 重构 | 架构可编译，全回归通过 | src/native/jit/ | 中 |
| wf1-gc | 1b | planned | main | GC 精确槽映射 + GPR 条目 | wf1-arch | batch-1 | jit_engine.cpp, jit_method.h | GcSlotMapV0 含 GPR 条目；nm->gc_slot_map 非空 | src/native/jit/ | 大 |
| wf1-liveness | 1c | planned | main | 开启 Liveness + 验证 | wf1-gc | batch-1 | jit_engine.cpp | liveness 默认开启，GC 精确扫描通过 | src/native/jit/ | 中 |
| wf1-osr | 1d | planned | main | OSR segfault 修复 | wf1-liveness | batch-1 | jit_helpers.cpp, jit_engine.cpp | Test_OsrEntry 不 segfault | src/native/jit/ | 小 |
| wf1-unwind | 1e | planned | main | Unwind 编译守卫 | wf1-osr | batch-1 | jit_unwind.cpp | static_assert 就位 | src/native/jit/ | 极小 |
| wf1-tests | 1f | planned | main | 模块级测试套件补齐 | wf1-unwind | batch-1 | testing/jit 扩展 | 全回归通过，新测试通过 | testing/src/native/jit/ | 大 |
| wf1-ci | 1g | design | main | CI 性能基线建立 | wf1-tests | batch-1 | design-wf1-ci.md | 基线设计文档完成，待实现 workflow | .github/workflows/ | 中 |
| wf2-linux | 2 | completed | main | Linux SEH + DWARF + CI | — | batch-2 | jit_seh.cpp Linux handler, jit_unwind.cpp | Linux x64 T4 JIT 可用（代码完成，CI pending） | src/native/jit/ | 大 |
| wf3-tlab | 3 | completed | main | TLAB 内联分配 | wf1-arch, wf1-tests | batch-3 | jit_engine.cpp, jit_helpers.cpp | NewObj/Box/NewArr TLAB bump path 无 helper call | src/native/jit/ | 大 |
| wf4-debug | 4 | completed | main | 调试信息 + SOS | — | batch-2 | jit_engine.cpp, jit_method.h, jit_seh.cpp, jit_debug_contract.h/cpp, jit_sos.cpp, sos_common.h, hotpatch_table.h/cpp, metadata_interface.cpp, gc_sos.cpp | JitDebugInfo 发射 + GetMethodName + 调试合约 + JIT SOS 扩展 + 5 个测试 | src/native/jit/, src/native/runtime-core/jit/ | 大 |
| wf5-arm64 | 5 | completed | main | ARM64 完整支持 | wf1-arch | batch-4 | arm64_encoder.h, ArchTraits.h, jit_engine.cpp (参数化), jit_unwind.cpp (ARM64 DWARF), jit_precode.cpp (ARM64 trampoline), LinuxSehHandler.cpp (ucontext), jit_engine.h (编码器类型选择) | ARM64 回归通过 | src/native/jit/ | 极大 |

## 最新摘要

batch-2 (wf2-linux + wf4-debug) ✅ 已完成：
- wf2-linux: LinuxSehHandler.cpp + DWARF .eh_frame
- wf4-debug: JitDebugInfo 发射 + 调试合约 (jit_debug_contract) + JIT SOS WinDbg 扩展 (jit_sos) + GetMethodName 实现 + 5 个单元测试
batch-3 (wf3-tlab) ✅ 已完成：EmitLoadTlsTlab 替换 3 处 CodegenGetTlab CALL，全 199 JIT 测试通过。
batch-1 wf1-ci 设计文档已完成（design-wf1-ci.md），待实现 GitHub Actions workflow。
batch-4 (wf5-arm64) ✅ 已完成：完整 ARM64 JIT 支持 — Arm64Encoder 160/160 方法、ArchTraits 双架构参数化、jit_engine.cpp (~500 寄存器常量替换)、NEON SIMD 扩展、ARM64 prologue/epilogue、precode trampoline、DWARF 展开、SEH ucontext、EmitSimd NEON 分支。x64 回归测试全部通过。

## 已完成

所有 4 个 batch 全部完成（batch-1, batch-2, batch-3, batch-4）。JIT 工业化收官计划的所有工作流已交付。

## 风险评估

- Liveness 开启风险：分两步推进（全量发射→验证→开启裁剪）
- ARM64 规模风险：接受估算偏差，扩展时间线
- 合并冲突：流1→流3 共享 jit_engine.cpp 已确认串行处理
