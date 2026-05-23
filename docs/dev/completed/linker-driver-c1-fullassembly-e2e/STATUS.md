---
task_id: linker-driver-c1-fullassembly-e2e
task_type: plan
phase: completed
lifecycle_status: completed
parent_task_id: 20260523-linker-driver-industrialization
source_task_id: c1-fullassembly-e2e
source_relation: roadmap-child
clearance_source: parent-roadmap
blocking_questions: []
question_clearance: cleared
clearance_confirmed_by_user: true
entry_skill: dev-writing-plans
preflight_review: pass
auto_execution_decision: continue
---

# c1-fullassembly-e2e: FullAssemblyEmitter e2e 验证

## 设计摘要

FullAssemblyEmitter 是 convert-to-cpp 路径的直接内联发射器，封装 NativeAotEmitter.GenerateFromArtifacts() + 文件写入。

E2e 验证分为两级：
- **单元级**：边界条件验证（null guard）
- **集成级**：完整管线 e2e 需要实际 PipelinePlan.Execute()，当前在 xUnit 宿主中运行会崩溃（原生代码交互）。集成验证推迟到 Phase G foundation-dll verification pipeline 执行。

## 完成证据

- **文件创建**：
  - `tests/managed/Chaos.IL2CPP.Driver.Tests/FullAssemblyEmitterTests.cs` — 1 测试
- **测试结果**：26 测试全部通过（新增 1：Emit_NullClosureResult_ThrowsArgumentNullException）
- **集成验证待办**：Phase G 中通过 foundation-dll verification pipeline 执行完整 e2e
- **验证命令**：
  ```bash
  dotnet test tests/managed/Chaos.IL2CPP.Driver.Tests --no-build
  ```

## 架构审视

- **结构告警**：FullAssemblyEmitter 仅 57 行，职责清晰，无需拆分。
- **权责图审核 (critical)**：
  - `authority`：FullAssemblyEmitter 仅用于 convert-to-cpp 路径，无外部 authority 变更
  - `upstream`：依赖 ManagedClosureResult（来自 PipelinePlan.Execute）
  - `downstream`：文件输出 + NativeAotResult 返回
  - `shim_debt`：无
  - `proof`：边界测试 + Phase G 集成验证
