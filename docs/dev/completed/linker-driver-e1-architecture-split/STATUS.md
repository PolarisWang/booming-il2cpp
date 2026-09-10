---
task_id: linker-driver-e1-architecture-split
task_type: plan-execution
phase: completed
lifecycle_status: completed
parent_task_id: 20260523-linker-driver-industrialization
source_relation: roadmap-child
arch_review_mode: critical
blocking_questions: []
question_clearance: cleared
clearance_confirmed_by_user: true
clearance_source: parent-roadmap
---

# E1: DriverEntry → PipelineOrchestrator Split — Completed

## 完成证据

- arch_review: ok
- authority_review: ok
- verification:
  - canonicalCommand: dotnet build src/managed/Chaos.IL2CPP.Driver/Chaos.IL2CPP.Driver.csproj
- test_result: passed (26/26)
- wiki: n/a
- next: f1-codegen-g7-g12

## 工作内容

### 新增

- `src/managed/Chaos.IL2CPP.Driver/PipelineOrchestrator.cs` — 管线编排职责
  - `RunPipeline(ManagedClosureRequest) → int`
  - `RunNativeReference(NativeReferenceProofRequest) → int`
  - `RunNativeAot(NativeAotRequest) → int`
  - `WriteJson<T>()` — 私有辅助方法

### 修改

- `DriverEntry.cs` — 三个 `Run()` 方法简化为 PipelineOrchestrator 委托
  - `Run(ManagedClosureRequest)` → `_orchestrator.RunPipeline(request)`
  - `Run(NativeReferenceProofRequest)` → `_orchestrator.RunNativeReference(request)`
  - `Run(NativeAotRequest)` → `_orchestrator.RunNativeAot(request)`

### 架构变更

- DriverEntry 职责范围：CLI 路由、参数解析、帮助文本 → 保持
- PipelineOrchestrator 职责：管线执行 + 产物持久化 → 提取
- DriverEntryTests 不变（仅测试 Main()，不依赖 Run() 实现）
