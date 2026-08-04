# a1-test-infra 实现计划

> **面向执行 Agent：** 必须使用 dev:executing-plans 来执行本计划。步骤使用复选框（`- [ ]`）语法跟踪。当前任务带有 `parent_task_id: 20260523-linker-driver-industrialization`，执行完成时必须先归档当前子任务目录，再同步父任务。

**目标：** 创建 Chaos.IL2CPP.Driver.Tests xUnit 测试项目，覆盖 DriverEntry、PipelinePlan、ConvertToCppHandler 的 ~17 个单元/集成测试。

**架构：** 遵循 Chaos.IL2CPP.CodeGen.Tests 的模式 — xUnit + .NET 8.0 + 集成测试通过 StubAssembly 运行全管线 + 单元测试通过反射测试私有方法。

**技术栈：** xUnit 2.6.6, Microsoft.NET.Test.Sdk 17.8.0, .NET 8.0

**架构审核模式：** normal

**结构告警重点：** 无 — 新项目创建，遵循既有模式

**权责图审核主题：** n/a

**设计文档：** STATUS.md（当前目录）

**问题清零来源：** parent-roadmap

**计划来源：** roadmap-child-auto

**预期知识沉淀：** n/a

**收尾约束：** 执行完成后必须进入"结构告警与架构审视 -> 测试通过 -> 归档 completed -> 合并&提交"固定链路。

---

## 背景

Driver 和 Pipeline 模块缺少单元测试覆盖。按照 roadmap Phase A 要求，创建测试项目并编写至少 17 个测试。

## 文件清单

**创建：**
- `tests/managed/Chaos.IL2CPP.Driver.Tests/Chaos.IL2CPP.Driver.Tests.csproj`
- `tests/managed/Chaos.IL2CPP.Driver.Tests/DriverEntryTests.cs` — 6 测试
- `tests/managed/Chaos.IL2CPP.Driver.Tests/PipelinePlanTests.cs` — 5 测试
- `tests/managed/Chaos.IL2CPP.Driver.Tests/ConvertToCppHandlerTests.cs` — 3 测试
- `tests/managed/Chaos.IL2CPP.Driver.Tests/ConvertToCppConfigTests.cs` — 3 测试

**引用（不变）：**
- `tests/managed/Chaos.IL2CPP.CodeGen.Tests/StubAssembly/` — 复用 StubAssembly 进行集成测试

## 任务

### Task 1: 创建测试项目

1. 创建 `tests/managed/Chaos.IL2CPP.Driver.Tests/` 目录
2. 创建 `.csproj`：
   - TargetFramework: net8.0
   - 引用 Chaos.IL2CPP.Driver、Chaos.IL2CPP.Pipeline
   - xUnit 包（与 CodeGen.Tests 相同版本）
   - StubAssembly.csproj（ReferenceOutputAssembly=false）
3. 验证 `dotnet build tests/managed/Chaos.IL2CPP.Driver.Tests` 通过

### Task 2: DriverEntryTests.cs — 6 测试

| # | 测试名 | 验证点 |
|---|--------|--------|
| 1 | Run_ManagedClosureRequest_ReturnsZero | full-closure 集成测试，验证返回 0 |
| 2 | Run_ManagedClosureRequest_CreatesOutputDir | 验证 output root 目录被创建 |
| 3 | Main_NoArgs_ReturnsOne | Main() 无参数返回 1 |
| 4 | Main_ConvertToCpp_Invalid_ReturnsOne | convert-to-cpp 无 assembly 返回 1 |
| 5 | Main_LegacyConvert_Invalid_ReturnsOne | 遗留模式 args < 2 返回 1 |
| 6 | Name_ReturnsDriver | Name 属性测试 |

### Task 3: PipelinePlanTests.cs — 5 测试

| # | 测试名 | 验证点 |
|---|--------|--------|
| 1 | Stages_ReturnsFiveStages | Stages 属性返回 5 个阶段 |
| 2 | Execute_FullAssembly_AllArtifactsPresent | 全闭包集成测试，验证 13 artifacts |
| 3 | Execute_EntryPoint_FiltersMethods | entry-point 模式返回有效 AotCoreIr |
| 4 | ExecuteMulti_ReturnsPerAssembly | 多 assembly 返回正确数量 |
| 5 | ExecuteMulti_WithEntryOverride | entry override 模式通过 |

### Task 4: ConvertToCppConfigTests.cs — 3 测试

| # | 测试名 | 验证点 |
|---|--------|--------|
| 1 | Parse_SingleAssembly | --assembly + --output 解析正确 |
| 2 | Parse_MultipleAssemblies | 多 --assembly 解析正确 |
| 3 | Parse_WithAllOptions | 所有选项（entry-point, full-closure, mode, verbose）解析正确 |

### Task 5: ConvertToCppHandlerTests.cs — 3 测试

| # | 测试名 | 验证点 |
|---|--------|--------|
| 1 | Run_NoAssemblies_ReturnsOne | 无 assembly 返回 1 |
| 2 | Run_SingleAssembly_HappyPath | 单 assembly 集成测试（如果 DLL 存在） |
| 3 | Run_MultiAssembly_HappyPath | 多 assembly 集成测试（如果 DLL 存在） |

## 验证方式

```bash
dotnet build tests/managed/Chaos.IL2CPP.Driver.Tests
dotnet test tests/managed/Chaos.IL2CPP.Driver.Tests --no-build
```
