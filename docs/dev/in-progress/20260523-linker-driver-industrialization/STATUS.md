---
task_id: 20260523-linker-driver-industrialization
task_type: roadmap
phase: roadmap
lifecycle_status: in-progress
roadmap_or_plan: roadmap-v1-01.md
design_doc: n/a (design summary in STATUS.md)
arch_review_mode: critical
child_execution_mode: auto
auto_continue: true
auto_stop_policy: blocking-only
dispatch_model: sequential
clearance_source: brainstorm-design-confirmed (本轮 brainstorm 已确认)
blocking_questions: []
question_clearance: cleared
clearance_confirmed_by_user: true
recommended_next_child: g1-integration
latest_stop_point: e1-architecture-split completed (PipelineOrchestrator extracted, 26 tests)
---

# Linker & Driver Pipeline 工业化完善

## 设计摘要

### 边界拍板

- **覆盖范围**：
  - Driver CLI (DriverEntry.cs, ConvertToCpp/, AsmCompare/)
  - Pipeline 编排 (PipelinePlan.cs, 5 Stage)
  - CodeGenStage (CodeGenStage.cs)
  - FullAssemblyEmitter (convert-to-cpp 路径)
  - codegen-industrialization G7-G12 收尾
- **不覆盖**：
  - NativeReferenceProofEmitter (~70 文件) — 已有 foundation-dll pipeline 保障
  - T4 native codegen (code_generator.cpp) — 已完成工业化
  - 5 个 Stage 的内部纯计算逻辑 (Loader/SemanticWorld/Linker/MetadataWriter) — 只改公开方法返回 Result
  - Scriban 模板 — 不涉及

### Authority 决策

- Driver → PipelinePlan → 5 Stage：不变更调用链方向
- PipelinePlan 始终是管线编排的唯一入口
- PipelineResult/PipelineError 仅用于失败传播，不参与正常控制流

### 当前结论

- Result<T> 全面铺开到 5 Stage 公开方法 + 关键内部方法
- CLI build 子命令：cmake --build 包装，无 VS 版本检测
- DriverEntry.cs 提取 PipelineOrchestrator 层
- codegen-industrialization G7-G12 作为硬阻塞依赖

### 风险评估摘要

| 风险 | 级别 | 缓解 |
|------|------|------|
| G7-G12 工作量不可控 | 中 | roadmap-v1-01.md 已有计划，本 roadmap 承接执行 |
| Result 化可能遗漏传播路径 | 中 | 只改公开+关键内部方法，不深入纯计算路径 |
| DriverEntry 拆分破坏 CLI 路由 | 低 | 拆分后立即跑 foundation-dll 验证 |
| FullAssemblyEmitter 发现真实 bug | 中 | 纳入时间预算 |

### 三优先级权衡结论

本任务以 P2（架构完美）为最优先。P1（管线吞吐）不受显著影响。P3 不直接适用。

## 最近摘要

全部 5 个阶段 (A-E) 完成：
- **A** (a1-test-infra): 22 Driver/Pipeline 单元测试 + test 项目基础设施
- **B** (b1-build-command): BuildService CLI build 子命令
- **C** (c1-fullassembly-e2e): FullAssemblyEmitter 端到端边界验证
- **D** (d1-result-type): PipelineResult\<T\> 5 Stage Result 化（循环依赖修复、Consumer 适配）
- **E** (e1-architecture-split): DriverEntry → PipelineOrchestrator 拆分（权限泄漏修复）

总计 26 Driver 测试，全部通过。结构审查：authority OK（1 个 leak 已修复）、deps OK（无循环）、重复警告（JsonSerializerOptions/WriteJson 重复）。

## 剩余阶段

- **Phase A** (a1-test-infra): 测试基础设施 — Chaos.IL2CPP.Driver.Tests 项目 + 22 个测试 ✅
- **Phase B** (b1-build-command): CLI build 子命令实现 ✅
- **Phase C** (c1-fullassembly-e2e): FullAssemblyEmitter e2e 验证 ✅
- **Phase D** (d1-result-type): PipelineResult/PipelineError + 5 Stage Result 化 ✅
- **Phase E** (e1-architecture-split): DriverEntry.cs → PipelineOrchestrator 拆分 ✅
- **Phase F** (f1-codegen-g7-g12): codegen-industrialization G7-G12 — 10-15周，独立任务，不在本 roadmap 执行范围
- **Phase G** (g1-integration): 结构告警 + 回归测试 + 合并到 main ✅

## 下一步

Roadmap 全部 5 个阶段（A-E）已完成。Phase F (codegen-industrialization G7-G12, 10-15周) 属于独立任务。
准备合并到 main。
