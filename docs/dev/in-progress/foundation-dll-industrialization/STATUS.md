---
task_id: foundation-dll-industrialization
task_name: foundation-dll 工业化级验证
task_type: roadmap
phase: roadmap
lifecycle_status: in-progress
roadmap_or_plan: docs/dev/in-progress/foundation-dll-industrialization/roadmap-v1-01.md
child_execution_mode: auto
auto_continue: true
auto_stop_policy: blocking-only
dispatch_model: sequential
blocking_questions: []
question_clearance: cleared
clearance_confirmed_by_user: true
---

# Foundation DLL 工业化级验证

## 最近摘要

Roadmap 已创建。21 个子任务，从 fd-p0-infra 开始串行推进。

## 下一步

启动 fd-p0-infra（基础设施搭建）。

## 推荐下一子任务

fd-p0-infra

# Foundation DLL 工业化级验证

## 最近摘要

已完成 13-stage 全覆盖验证的 brainstorm。范围：14 个 assembly，~135 个 family。以 family 为推进粒度，CoreLib 优先，首次运行时以 JIT 结果为 baseline。

## 设计摘要

### 边界拍板

- 覆盖 14 个 assembly（System.Private.CoreLib, System.Linq, System.Collections.Immutable, System.Runtime.InteropServices, System.Text.Json, System.Formats.Asn1, System.IO.Compression.Brotli, System.IO.Compression.ZipFile, System.IO.Pipelines, System.Net.ServerSentEvents, System.ObjectModel, System.Runtime.Serialization.Formatters, System.Security.Principal.Windows, System.Threading.Tasks.Parallel）
- 所有具备 contract.json 的 family 均纳入
- 推进粒度：family 级
- CoreLib 策略：优先攻坚
- Benchmark 基线：首次运行以 JIT 结果为 baseline
- HotUpdate 范围：全覆盖

### 非目标

- 不新增 family 或 method subject
- 不修改 family 的 contract.json 内容（只修复编译错误）
- 不重构测试基础设施，只做填补和扩展

### 三优先级权衡结论

P1 性能最优、P2 方案完美性、P3 HotUpdate 在本方案中方向一致，无冲突。13-stage 全覆盖同时满足三个优先级。

### 风险评估摘要

| 风险 | 概率 | 影响 | 缓解 |
|------|------|------|------|
| JIT codegen 对某些 family 不可用 | 中 | 高 | Phase 0 先验证 |
| HotUpdate 未接入 entry.exe | 中 | 高 | Phase 0 确认 flag |
| CoreLib 47 family 批量修复耗时长 | 高 | 高 | 从简单 family 开始 |
| 运行总时长 ~12-60 小时 | 高 | 中 | 增量验证 |

### 阶段划分

- Phase 0: 基础设施搭建（batch_run_all 扩展、JIT/hotupdate 通路验证）
- Phase 1: CoreLib 优先攻坚（47 families）
- Phase 2: 小 assembly 热身（Brotli, ZipFile, Pipelines, ServerSentEvents, Tasks.Parallel — 19 families）
- Phase 3: 中等 assembly（ObjectModel, Security.Principal.Windows, Serialization.Formatters — 18 families）
- Phase 4: 大 assembly 收尾（Collections.Immutable, Linq, S.R.IS, Text.Json, Formats.Asn1 — 53 families）

### 13-stage 标准流程

preflight → codegen AOT → jit_codegen → fact AOT → fact JIT → audit → asm_compare → microbench → benchmark → hotupdate AOT fact → hotupdate AOT bench → hotupdate JIT fact → hotupdate JIT bench

### 关键文档

- 设计摘要嵌入本 STATUS.md
- [roadmap-v1-01.md](roadmap-v1-01.md)

### 下一步入口

从 fd-p0-infra 开始，先搭建基础设施。

### Authority 决策

- 推进粒度：family 级
- 调度方式：串行（逐 family 推进）
- 终端容量：1
- 验收标准：每个 family 的 13-stage 全部通过（passed/skipped 合法），部分 stage 可因基础设施不足而 skip（但需记录原因）
