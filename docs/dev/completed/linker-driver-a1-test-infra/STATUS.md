---
task_id: linker-driver-a1-test-infra
task_type: plan
phase: completed
lifecycle_status: completed
parent_task_id: 20260523-linker-driver-industrialization
source_task_id: a1-test-infra
source_relation: roadmap-child
clearance_source: parent-roadmap
blocking_questions: []
question_clearance: cleared
clearance_confirmed_by_user: true
entry_skill: dev-writing-plans
preflight_review: pass
auto_execution_decision: continue
---

# a1-test-infra: Chaos.IL2CPP.Driver.Tests 测试基础设施

## 设计摘要

为 Driver 和 Pipeline 模块创建 xUnit 测试项目 `Chaos.IL2CPP.Driver.Tests`，遵循 CodeGen.Tests 的模式：
- 项目文件引用 Chaos.IL2CPP.Driver 和 Chaos.IL2CPP.Pipeline
- 使用 `InternalsVisibleTo` 访问 internal 类型（ConvertToCppConfig、ConvertToCppHandler）
- 单元测试聚焦 CLI 参数解析、PipelinePlan 结构验证、错误路径验证
- 生产代码修复：ConvertToCppConfig.Parse() 移除 Environment.Exit(1) 改为安全返回

## 完成证据

- **文件创建**：4 个测试文件 + 1 个 .csproj
  - `tests/managed/Chaos.IL2CPP.Driver.Tests/Chaos.IL2CPP.Driver.Tests.csproj`
  - `tests/managed/Chaos.IL2CPP.Driver.Tests/DriverEntryTests.cs` (8 测试)
  - `tests/managed/Chaos.IL2CPP.Driver.Tests/ConvertToCppConfigTests.cs` (6 测试)
  - `tests/managed/Chaos.IL2CPP.Driver.Tests/ConvertToCppHandlerTests.cs` (4 测试)
  - `tests/managed/Chaos.IL2CPP.Driver.Tests/PipelinePlanTests.cs` (4 测试)
- **测试结果**：22 测试全部通过
- **生产代码修改**：
  - `src/managed/Chaos.IL2CPP.Driver/Chaos.IL2CPP.Driver.csproj` — 添加 `InternalsVisibleTo`
  - `src/managed/Chaos.IL2CPP.Driver/ConvertToCpp/ConvertToCppConfig.cs` — 移除 `Environment.Exit(1)` 改为 return empty config
  - `src/managed/Chaos.IL2CPP.Generator/Emission/NativeAotLoweringPlanner.ExceptionEmission.cs` — 补全 partial class 缺少的 `}`
  - `src/managed/Chaos.IL2CPP.Generator/Planning/NativeAotLoweringPlanner.MetadataSupport.cs` — 添加 `(uint)` 转换
  - `src/managed/Chaos.IL2CPP.Generator/Emission/NativeAotLoweringPlanner.StructuredControlFlow.cs` — `?? []` → `new HashSet<int>()`
  - `src/managed/Chaos.IL2CPP.Generator/Emission/NativeAotLoweringPlanner.MethodEmission.cs` — 修复 orphan `else if`

- **验证命令**：
  ```bash
  dotnet build tests/managed/Chaos.IL2CPP.Driver.Tests  # 0 errors
  dotnet test tests/managed/Chaos.IL2CPP.Driver.Tests --no-build  # 22/22 passed
  ```

## 架构审视

- **结构告警**：`ConvertToCppConfig.Parse()` 原包含 `Environment.Exit(1)`，属于 Parse 方法中不应有的副作用。已修正为返回空 config，由调用方处理。warn 级别，已修复。
- **文件职责**：测试文件按被测试类拆分，职责清晰。
- **测试粒度**：避免集成测试（运行完整管线会崩溃测试宿主），聚焦单元级验证。集成测试推迟到 Phase C。
- **权责图审核 (critical)**：
  - `authority`：Driver.Tests 测试项目，依赖 Driver/Pipeline/Contracts/Generator，无新增 authority
  - `upstream`：无上游依赖变更
  - `downstream`：后续 Phase B-G 将扩展此测试项目
  - `allowed_deps`：仅引用 Driver/Pipeline/Contracts，通过 StubAssembly.csproj 引用测试用 DLL
  - `shim_debt`：无新增 shim
  - `proof`：22 个 passing tests 作为回归护栏
