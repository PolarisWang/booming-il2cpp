---
task_id: linker-driver-d1-result-type
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

# D1: PipelineResult\<T\> Error Handling — Completed

## 完成证据

- arch_review: ok
- authority_review: ok
- verification:
  - canonicalCommand: dotnet build src/managed/Chaos.IL2CPP.Driver/Chaos.IL2CPP.Driver.csproj
  - formalObject: n/a
  - summaryPath: n/a
  - eventsPath: n/a
  - consolePath: n/a
- test_result: passed (26/26 Driver tests, 0 errors in Driver project)
- wiki: n/a
- next: e1-architecture-split

## 工作内容

### PipelineResult\<T\> 类型

- `PipelineResult<T>` 和 `PipelineError` 定义在 `Chaos.IL2CPP.Contracts`（避免循环依赖）
- 所有 5 个 Stage 公开方法改为返回 `PipelineResult<T>`:
  - `LoaderStage.LoadMultiple()` → `PipelineResult<LoadedWorldModel>`
  - `SemanticWorldStage.Build()` → `PipelineResult<SemanticWorldModel>`
  - `LinkerStage.Link()` → `PipelineResult<LinkedWorldModel>`
  - `MetadataWriterStage.Write()` → `PipelineResult<MetadataWriterOutput>`
  - `CodeGenStage.Generate()` → `PipelineResult<ManagedClosureResult>`
- `PipelinePlan.Execute()` 和 `ExecuteMulti()` 返回 `PipelineResult<...>`
- 所有消费者（DriverEntry、ConvertToCppHandler、AsmCompareHandler）已适配

### 修复的问题

- **循环依赖**: `PipelineResult.cs` 从 Pipeline 项目移到 Contracts 项目（Contracts 被所有项目依赖）
- **CS0105 重复 using**: LinkerStage/MetadataWriterStage 修复
- **CS1524 缺少 catch**: CodeGenStage.cs try 缺少 catch 块
- **CS1061 StaticInitializationPlanning**: LoaderStage.LoadMultiple() 返回 PipelineResult 需要解包
- **Consumer 适配**: 单 assembly 路径的 ConvertToCppHandler 缺少 IsFailure 检查
- **CodeGen 测试适配**: PipelinePlan.Execute/ExecuteMulti 调用点添加 .Value! 解包
- **5 个 csproj 回滚**: Revert 了误加的 Pipeline 项目引用（循环依赖）

### 架构变更

- 新增 `PipelineResult.cs` → `Chaos.IL2CPP.Contracts`
- 删除 `PipelineResult.cs` ← `Chaos.IL2CPP.Pipeline`
- `using Chaos.IL2CPP.Pipeline;` → `using Chaos.IL2CPP.Contracts;` 在 5 个 Stage 中
- Driver 消费者保持 `using Chaos.IL2CPP.Contracts;` + `using Chaos.IL2CPP.Pipeline;`
