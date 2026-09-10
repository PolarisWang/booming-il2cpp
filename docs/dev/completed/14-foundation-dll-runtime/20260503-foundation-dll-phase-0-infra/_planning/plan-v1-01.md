# Phase 0 基础设施对齐与 CoreLib 摸底 实现计划

> **面向执行 Agent：** 使用 dev:executing-plans 执行本计划。步骤使用 `- [x]`（已完成）或 `- [ ]`（待办）跟踪。

**目标：** 建立从 DLL → IL body → emitter 覆盖的自动化分析工具链，并完成 CoreLib 摸底。

**架构：** 在现有 `Chaos.IL2CPP.Tools.FoundationPack.Analysis` 中新增 `il-scan` 命令，使用 `System.Reflection.Metadata` 读取 IL body。新增 Python 分析器消费扫描结果并对比 emitter 覆盖。

**技术栈：** C# (.NET 8) + System.Reflection.Metadata + Python

**架构审核模式：** critical

**结构告警重点：** 工具代码保持轻量，不引入复杂抽象。Python 分析器解析 emitter switch-case 时注意正则精度。

**设计文档：** n/a（设计摘要见 STATUS.md）

**问题清零来源：** parent-roadmap

---
## 任务列表

### Step 1: IL Scanner C# 命令实现

文件：`src/tools/Chaos.IL2CPP.Tools.FoundationPack.Analysis/ILScanner/Program.cs`

职责：扫描 DLL 所有 method，提取 IL opcode 序列，输出 JSON。

核心逻辑：
- 使用 `PEReader` + `MetadataReader`（已有引用）
- 遍历所有 TypeDefinition → MethodDefinition
- 对每个有 body 的 method：通过 `GetMethodBody()` + `GetILReader()` 解码每条 IL 指令
- 输出 `{assemblyName, methods: [{name, ilOpcodes[], uniqueOpcodes[], hasBody, parameters, returnType}]}`

- [ ] 创建 `src/tools/Chaos.IL2CPP.Tools.FoundationPack.Analysis/ILScanner/`
- [ ] 编写 `ILScanner/ILScanner.csproj`（引用 `System.Reflection.Metadata`）
- [ ] 编写 `ILScanner/Program.cs`（扫描 + JSON 输出）
- [ ] 构建验证：`dotnet build`

### Step 2: Emitter Coverage Python 分析器

文件：`build/toolchains/run/testing/foundation_dll/il_coverage_analyzer.py`

职责：
- 读取 `NativeAotLoweringPlanner.MethodEmission.cs` 中 `case "xxx":` 列表
- 定义 `EMITTER_KNOWN_OPCODES` 常量列表（97 个 opcodes，硬编码为已知清单）
- 对比 IL Scanner 输出的 uniqueOpcodes vs emitter known opcodes
- 输出 Gap Report JSON

- [ ] 编写 `emitter_coverage_known.py` — 定义已知 emitter opcodes
- [ ] 编写 `il_coverage_analyzer.py` — 对比逻辑 + gap report 生成

### Step 3: CoreLib 扫描 + Gap Report

- [ ] 运行 IL Scanner 扫描 CoreLib：`dotnet run -- il-scan --dll-path <path> --output corelib-il-scan.json`
- [ ] 运行 Python 分析器生成 gap report
- [ ] 分析 CoreLib 24 families 的验证状态摸底
- [ ] 产出摸底报告

### Step 4: Phase 0 归档

- [ ] 验证工具可对任意 DLL 重复 Step 3
- [ ] 更新 STATUS.md 为 completed
- [ ] 标记 Phase 0 完成
