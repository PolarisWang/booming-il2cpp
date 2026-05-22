# HotUpdate 工业化补齐 — Brainstorm

> **task_id**: hotupdate-industrialization
> **创建日期**: 2026-05-23
> **phase**: roadmap
> **基于报告**: 前序对话中的热更新工业化差距深度分析报告

## 背景

基于系统分析报告的 11 项风险点（R1-R11），目标是将热更新系统从当前 ~50% 成熟度提升到工业化水平，在性能、架构、工具链、测试覆盖、多平台支持等方面全面对齐 HybridCLR 商业版性能 + Mono 级别调试/诊断能力。

**总体策略：** B（架构优先），分 4 阶段交付。

## 设计摘要

### 1. 边界拍板

| 维度 | 决策 |
|------|------|
| 目标水平 | HybridCLR 商业版性能 + Mono 级别调试/诊断 |
| 平台范围 | 先 Windows x64 做透 |
| 交付节奏 | 分阶段交付，每阶段 1-3 月 |
| 与 Interpreter 关系 | 合并为一个 roadmap |
| 三优先级 | 性能 > 架构 > 热更 继续适用 |

### 2. 阶段划分

```
Phase 1 (2 月):  架构地基 — C3 MetadataRegistry + C6 跨平台 + C7 ARM64
Phase 2 (3-4 月): 性能飞跃 — C1 寄存器式 IR + C8 OSR
Phase 3 (2-3 月): 体验+质量 — C2 调试器 + C4 工具链 + C5 测试覆盖
Phase 4 (1 月):   收尾对齐 — C9 Managed + C10 静态变量 + C11 COM
```

### 3. 核心架构决策

| 决策项 | 方案 | 理由 |
|--------|------|------|
| C3 MetadataRegistry | 方案 A: 统一注册表, composite key, 统一哈希表 | 为调试器/OSR 提供统一符号查询 |
| C1 寄存器式 IR | 方案 A1: 一步到位全新寄存器 VM + codegen IR emitter | 性能对齐 HybridCLR 的唯一路径 |
| C8 OSR | 利用寄存器 VM 快照, 条件: 1000 calls + 1s interval + 5ms budget | 复用已有 T4 系统 |
| C2 调试器 | DAP Wire Protocol + runtime DebugAgent | 不造轮子, 对齐 VSCode |
| C6 跨平台 | std::atomic 替换 MSVC 内置函数 | 最小改动, 标准路径 |

### 4. authority 决策

- MetadataRegistry 归 `runtime-core` 模块, 与 HotpatchNameRegistry 同级
- 寄存器式 VM 新建 `src/native/interpreter/register_vm.h/.cpp`, 与原栈式 VM 并存
- codegen RegisterIREmitter 新建 `Emission/RegisterIREmitter.cs`
- 调试器的 DebugAdapter (C#) 入 `src/managed/Chaos.IL2CPP.Debugger/`
- Runtime Debug Agent (native) 入 `src/native/diagnostics/`

### 5. 风险评估摘要

| 风险 | 等级 | 缓解措施 |
|------|------|---------|
| Phase 1 修改 AOT bootstrap 影响全系统回归 | 高 | 测试套件必须在 Phase 1 结束后全部通过 |
| C1 寄存器式 VM 开发周期长, 短期无效果 | 中 | Phase 1 交付有独立价值(元数据统一), 不依赖 C1 |
| 调试器 DAP 实现复杂度被低估 | 中 | Phase 3 只做"基础可用", 高级功能放入 Phase 4 |
| 多平台编译问题暴露时间晚 | 低 | C6 在 Phase 1 提前处理 |

### 6. 三优先级权衡结论

三优先级（性能 > 架构 > 热更）贯穿全部决策：
- C1 寄存器式 IR：选择了最高性能的"一步到位"方案（P1 驱动）
- C3 MetadataRegistry：选择了架构最干净的"统一注册表"（P2 驱动）
- 没有出现 P3（热更）与 P1 或 P2 冲突的场景

## 关键文档

前序对话中完成的热更新差距分析报告为本设计的输入来源。

## 子任务映射

| task_id | phase | status | purpose | estimated_effort |
|---------|-------|--------|---------|-----------------|
| C3 | Phase 1 | **completed** | MetadataRegistry 统一元数据注册表 | 3 周 |
| C6 | Phase 1 | planned | 跨平台原子操作抽象 | 1 周 |
| C7 | Phase 1 | planned | ARM64 内存序验证 | 1-2 周 |
| C1 | Phase 2 | planned | 寄存器式 VM + codegen emitter | 3 月 |
| C8 | Phase 2 | planned | OSR 激活策略 | 1 月 |
| C2 | Phase 3 | planned | 调试器 DAP | 2 月 |
| C4a | Phase 3 | planned | EventPipe 事件 | 1 周 |
| C4b | Phase 3 | planned | Profiling | 2 周 |
| C4c | Phase 3 | planned | 增量更新 | 4 周 |
| C5a-d | Phase 3 | planned | 测试覆盖 | 6 周 |
| C9 | Phase 4 | planned | AutoBridge 扩展 | 2 周 |
| C10 | Phase 4 | planned | 静态变量隔离 | 3 周 |
| C11 | Phase 4 | planned | COM Interop | 2 周 |

## blocking_questions

- [x] Q1: "完美"的定义 — HybridCLR 性能 + Mono 调试/诊断
- [x] Q2: 平台范围 — Windows x64 优先
- [x] Q3: 时间预期 — 分阶段交付
- [x] Q4: 与 Interpreter 工业化 — 合并
- [x] Q5: 三优先级 — 继续适用
- [x] Q6: MetadataRegistry 方案 — 方案 A（统一注册表 + composite key + 统一哈希表）
- [x] Q7: 寄存器式 IR 方案 — A1（一步到位全新 VM）
- [x] Q8: Phase 策略 — B 架构优先

**blocking_questions: []**
**question_clearance: cleared**
**clearance_confirmed_by_user: true**

## 调度状态

- `task_type`: roadmap
- `roadmap_or_plan`: roadmap-v1-01.md
- `child_execution_mode`: auto
- `auto_continue`: true
- `auto_stop_policy`: blocking-only
- `dispatch_model`: sequential
- `recommended_next_child`: C6
- `latest_stop_point`: C3 completed (2026-05-23)
- `下一步`: 启动 C6 (跨平台原子操作抽象) 子任务
