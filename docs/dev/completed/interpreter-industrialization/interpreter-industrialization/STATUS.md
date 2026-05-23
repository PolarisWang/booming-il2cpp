# Interpreter 工业化补齐 — Roadmap

> **task_id**: interpreter-industrialization
> **创建日期**: 2026-05-22
> **更新日期**: 2026-05-23
> **task_type**: roadmap
> **phase**: completed — 全部完成
> **lifecycle_status**: completed
> **child_execution_mode**: auto
> **auto_continue**: true
> **auto_stop_policy**: blocking-only
> **dispatch_model**: sequential

## 背景

基于工业化差距分析，Interpreter 子系统在 15 个维度上存在缺口，涵盖调试器、诊断、性能、测试、OSR、COM interop 等多个方面。当前执行引擎成熟度评估 ~35%。

## 拍板确认

| 问题 | 决策 | 说明 |
|------|------|------|
| Q1: 平台 | Windows x64 专注 | CI 和测试覆盖集中，当前开发平台 |
| Q2: OSR | 方案 C: 混合 OSR | Backedge + Call-site 双向 OSR，当前帧 + 下次调用双触发 |
| Q3: Debugger | 方案 C-2: 自定义 IL-level 调试协议 | 非 ICorDebug（COM 兼容成本过高），走 DAP 协议接 VSCode |
| Q4: EventPipe | 方案 B-1: 核心层先行 | IPC + GC/TP/Exception 3 类事件，自定义 receiver CLI |

## 最终方案：4 层架构

```
Layer 1: 核心执行引擎加固 (Core Engine)           ~2 周 ✅ 完成
  ├── R-5: FastFrame 容量扩展                   1 天 ✅
  ├── R-1: SEH in FastExecute                   1 周 ✅
  ├── R-3: Cpblk/InitBlk 实现                   0.5 天 ✅
  ├── R-4: CallVirtConstrained 实现              0.5 天 ✅
  ├── R-10: Calli 实现                           0.5 天 ✅
  └── R-12: Tracked 对象泄漏修复                 0.5 天 ✅

Layer 2: Tier 升级与自适应 (Tiering)              ~3.5 周 ✅ 完成
  ├── R-2 Phase 1: Call-site tier upgrade        3 天 ✅
  ├── R-2 Phase 2: Backedge OSR                 2 周 ✅
  └── R-11: T4 Native Codegen 加固               1 周 ✅

Layer 3: 交叉验证 (Cross-cutting)                 ~2 周 ✅ 完成
  ├── R-6: GC × Interpreter 交互验证             5 天 ✅
  ├── R-7: Stress / Soak 测试                    5 天 ✅
  ├── R-8: Benchmark 回归管线                    3 天 ✅
  └── R-9: Overflow-check 测试补齐               1 天 ✅

Layer 4: 诊断与可观测 (Diagnostics)               ~4-6 周 ✅ 完成
  ├── Debugger (MVP: FastExecute hooks)           3 周 ✅
  ├── Debugger (完整: 调用栈+变量+RegisterExecute) +2 周 ✅
  ├── EventPipe (IPC + 3 类事件)                  1.5 周 ✅
  └── EventPipe (计数器 + receiver CLI)            1 周 ✅

预估总工期: ~8-10 周（Layer 3+4 与 Layer 1+2 可并行）
```

## 子任务状态

| task_id | phase | status | purpose | estimated_effort |
|---------|-------|--------|---------|-----------------|
| I-R5 | 1 | **completed** | FastFrame 容量扩展 | 1 天 |
| I-R1 | 1 | **completed** | SEH in FastExecute | 1 周 |
| I-R3 | 1 | **completed** | Cpblk/InitBlk 实现 | 0.5 天 |
| I-R4 | 1 | **completed** | CallVirtConstrained 实现 | 0.5 天 |
| I-R10 | 1 | **completed** | Calli 实现 | 0.5 天 |
| I-R12 | 1 | **completed** | Tracked 泄漏修复 | 0.5 天 |
| I-R2a | 2 | **completed** | Call-site tier upgrade | 3 天 |
| I-R2b | 2 | **completed** | Backedge OSR | 2 周 |
| I-R11 | 2 | **completed** | T4 Codegen 加固 | 1 周 |
| I-R6 | 3 | **completed** | GC×Interpreter 验证 | 5 天 |
| I-R7 | 3 | **completed** | Stress/Soak 测试 | 5 天 |
| I-R8 | 3 | **completed** | Benchmark 回归管线 | 3 天 |
| I-R9 | 3 | **completed** | Overflow 测试 | 1 天 |
| I-DBG | 4 | **completed** | Debugger MVP | 5 周 |
| I-EVP | 4 | **completed** | EventPipe 核心层 | 2.5 周 |

## 执行摘要

### Phase 1 (Layer 1: 核心执行引擎加固) — 全部完成 ✅

所有 6 个子任务完成，编译通过 0 errors：

| 子任务 | 改动 | 说明 |
|--------|------|------|
| I-R5 | fast_dispatch.h, osr_state.h | kMaxStack 16→64, kMaxLocals 8→32, kMaxTracked 8→32 |
| I-R1 | fast_dispatch.h/cpp, fast_frame_pool.cpp | 5 个 SEH handler（Throw/Leave/EndFinally/EndFilter/Rethrow） |
| I-R3 | fast_dispatch.cpp | Cpblk (memcpy) + InitBlk (memset) 实现 |
| I-R4 | fast_dispatch.cpp | CallVirtConstrained：struct direct call + ObjectRef MIC |
| I-R10 | fast_dispatch.cpp | Calli：函数指针 + PopCallArgs + InterpreterDispatchRaw |
| I-R12 | fast_dispatch.h, fast_frame_pool.cpp | TrackedBlock 溢出块修复泄漏 |

### Phase 2 (Layer 2: Tier 升级与自适应) — 全部完成 ✅

所有 3 个子任务完成，chaos_codegen.lib 编译通过 0 errors：

| 子任务 | 改动 | 说明 |
|--------|------|------|
| I-R2a | entry_direct.cpp | Call-site tier upgrade：T1→T2→T3→T4 状态机 + 自适应阈值 |
| I-R2b | fast_dispatch.h/cpp, fast_frame_pool.cpp | Backedge OSR：loop_counter + TryFastOsrPromotion + T4 OSR 入口 |
| I-R11 | code_buffer.h/cpp, code_generator.cpp | T4 Codegen 加固：数组溢出修复、OOM 安全、SEH 验证、Failed 标记 |

### Phase 3 (Layer 3: 交叉验证) — 全部完成 ✅

全部 4 个子任务完成：

| 子任务 | 说明 |
|--------|------|
| I-R6 | GC×Interpreter 交互验证 — 审计结论：当前交互安全 |
| I-R7 | Stress/Soak 测试 — register interpreter stress + FastExecute stress |
| I-R8 | Benchmark 回归管线 — bench-compare, baselines, multi-thread |
| I-R9 | Overflow-check 测试补齐 — checked arithmetic 操作码覆盖 |

### Phase 4 (Layer 4: 诊断与可观测) — 全部完成 ✅

#### EventPipe 核心层 ✅

EventPipe 核心层已完成：
- IPC transport via Windows Named Pipes
- 3 类事件：GC (9 种)、ThreadPool (6 种)、Exception (3 种)
- `chaos_diag.exe` receiver CLI，输出 JSON Lines
- 已接入 fast_dispatch.cpp Handle_Throw、interpreter_vm.cpp Throw/Rethrow、threadpool_events.cpp

#### Debugger MVP ✅

基于 DAP 协议的 IL-level 调试器全部完成（209 tests, 181 unit + 28 integration）：
- DAP 协议核心 + stdio transport + 12 个命令处理器
- 断点管理器 O(1) hash map lookup
- 单步控制器 Step Into/Over/Out
- FastExecute 断点检查 + 帧快照捕获（walk prev_frame chain + locals）
- RegisterExecute 断点检查 + 帧快照捕获（顶层帧）
- 调用栈展开（从快照读取 prev_frame 链）
- 变量检视（从快照读取 locals）
- 构建集成：所有子模块 0 errors

#### 预存编译错误修复

修复了 3 个非本任务引入的预存 build error：
- `t4_seh_handler.cpp`: ResetUnwindState 前向声明缺失 + ExceptionExecuteHandler 未定义
- `entry_direct.cpp`: PerInstrPicData 错误的 `codegen::` 命名空间限定

## 关键文档

- `roadmap-v1-01.md` — 阶段路线图
- `wiki/03-功能模块/06-il2cpp核心架构/21-interpreter-optimizations.md` — 现有优化
- `docs/reports/infrastructure-comparison-vs-coreclr-unity-mono.md` — 成熟度评估
