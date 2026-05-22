# Interpreter 工业化补齐 — Brainstorm

> **task_id**: interpreter-industrialization
> **创建日期**: 2026-05-22
> **phase**: brainstorming

## 背景

基于工业化差距分析，Interpreter 子系统在 12 个维度上存在缺口（I-01 ~ I-12），涵盖调试器、诊断、性能、测试、OSR、COM interop 等多个方面。当前执行引擎成熟度评估 ~45%。

## 12 项差距总览

| ID | 差距 | 优先级 | 当前状态 | 涉及子系统 |
|----|------|--------|---------|-----------|
| I-01 | Debugger 基础设施缺失 | P0 | 无 debugger 目录，无单步/断点/IL sequence point | interpreter, diagnostics |
| I-02 | Tier upgrade 策略缺失 | P0 | OSR state 框架存在但无激活策略 | interpreter, codegen |
| I-03 | EventPipe/诊断事件缺失 | P0 | 无 EventPipe 引用 | diagnostics |
| I-04 | 多线程 Stress 测试缺失 | P1 | interpreter 测试无 stress 目标 | testing |
| I-05 | GC 交互验证缺失 | P1 | GC 与 interpreter 测试完全隔离 | gc, interpreter, testing |
| I-06 | 解释器性能远低于工业标准 | P1 | ~200-290x vs AOT；已有 MIC/FastFrame/TLS 优化 | interpreter |
| I-07 | Benchmark 回归检测缺失 | P1 | 无 interpreter benchmark 基线管理 | testing, tools |
| I-08 | IR opcode 覆盖未工具化验证 | P2 | 100 opcodes 定义，但覆盖率未验证 | interpreter, testing |
| I-09 | 长时间稳定性测试缺失 | P2 | 无 interpreter soak test | testing |
| I-10 | COM interop 缺失 | P2 | COM 基础设施存在但 interpreter 未接 | interpreter, runtime-core |
| I-11 | Overflow-check 变体测试不足 | P2 | 实现存在但无专项测试 | interpreter, testing |
| I-12 | OSR 无激活策略 | P2 | OSR API 存在但 tier upgrade 未联动 | interpreter, codegen, tier_manager |

## 全局优先级约束

沿用项目三条优先级（来自 CLAUDE.md）：
1. **性能最优 (P1)** — 生成的 C++ 代码性能第一
2. **方案完美性 (P2)** — 架构完整性第二
3. **HotUpdate 支持 (P3)** — 热更新友好第三

## blocking_questions

待在本阶段清零的问题：

1. **范围边界** — I-01 (Debugger) 做到什么程度？完整 IL-level 调试器 vs 最小化调试信息输出？
2. **平台目标** — 是否绑定 Windows x64 或需要跨平台？
3. **预算约束** — 各 gap 的投入比例和预期里程碑？
4. **验收标准** — 解释器工业化的"完成定义"是什么？

## 关键文档

- `wiki/03-功能模块/06-il2cpp核心架构/21-interpreter-optimizations.md` — 现有优化
- `docs/reports/infrastructure-comparison-vs-coreclr-unity-mono.md` — 成熟度评估 ~45%
- `docs/discuss/20260513-architecture-comparison-report.md` — Interpreter 作为差异化能力
- `docs/discuss/20260513-hotupdate-comparison-report.md` — 性能对比 ~200-290x vs AOT
