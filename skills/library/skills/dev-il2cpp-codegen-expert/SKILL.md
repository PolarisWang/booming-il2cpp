---
name: dev-il2cpp-codegen-expert
description: dev-il2cpp-codegen-expert — il2cpp CodeGen 专家
---

> ⚠️ **本文件通过 Skill 工具加载，作用是注入领域知识到当前对话上下文。**
> 本文件**不是可执行的 agent**。当前 Agent 需阅读下方"执行流程"作为实现参考。
>
> **当前 Agent 请做**：阅读知识域和约束 → 自行实现代码 → 自行验证
> **当前 Agent 不要做**：加载后等待"Expert 自动执行"——它不会，Skill 只加载文本。
>

# dev-il2cpp-codegen-expert — il2cpp CodeGen 专家

## 领域边界

### 我负责的
- C# codegen 管线（`src/managed/Chaos.IL2CPP.Generator/`）的修改
- `NativeAotLoweringPlanner` 的 25 个 partial class 文件（Planning/Emission/RuntimeSupport）
- `NativeAotEmitter` codegen→C++ 输出管线
- Scriban 模板系统（40+ 模板文件）
- Snapshot 测试（88 个基线夹具）
- T4/JIT native code 生成器（`jit_engine.cpp`）
- PatchDataExtractor / PatchDataDumper（`.patchdata` 构建期生成）

### 我不负责的（超出以下范围 → 标记 remaining，回 Dispatcher 重新分发）

- **运行时实现**（runtime-core/interpreter 的 C++ 代码）→ 超出范围，标记 remaining，原因：需要运行时域知识
- **翻译路径设计**（IL 指令→Planner 选择逻辑）→ 超出范围，标记 remaining，原因：需要翻译域知识
- **测试治理**（subject/manifest/runner）→ 超出范围，标记 remaining，原因：需要测试治理域知识
- **GC/分配策略** → 超出范围，标记 remaining，原因：需要 GC 域知识
- **编译失败 / codegen stub**（LNK 错误、C++ 编译错）→ 超出范围，标记 remaining，原因：需要构建修复域知识

## 输出格式（Dispatcher 回读用）

每个 Expert 处理完任务后，必须在当前上下文中输出：

```
✅ done: [已处理的子任务 ID 列表]
⏳ remaining: [未处理的子任务 ID 列表 + 原因]
```

---

## 触发条件

- 从 `dev-il2cpp-core-agent` 路由到本 expert
- 用户直接输入 `/dev-il2cpp-codegen-expert`
- 用户输入包含：codegen、NativeAotEmitter、NativeAotLoweringPlanner、Scriban、T4 模板、snapshot 测试、PatchDataExtractor、emit-patch-data（C# 侧）

---

## 知识域

### 项目结构

| 路径 | 说明 |
|------|------|
| `src/managed/Chaos.IL2CPP.Generator/` | C# codegen 项目根目录 |
| `NativeAotEmitter.cs` | Codegen 入口（`Generate()`, `GenerateFromArtifacts()`） |
| `NativeAotLoweringPlanner.cs` + 25 partial files | LoweringPlanner 核心逻辑 |
| `CodeGenStage.cs` | Pipeline 集成（CodeGen 为 stage 5） |
| `PatchDataExtractor.cs` | `.patchdata` 构建期序列化 |
| `PatchDataDumper.cs` | `.patchdata` dump/查看 |
| `Templates/` | Scriban 模板目录（40+ 模板） |
| `Validation/NativeCodegenValidator.cs` | 9 条生成代码验证规则 |

### NativeAotLoweringPlanner 文件组织（25 files）

| 区域 | 文件 | 说明 |
|------|------|------|
| **Root** | `NativeAotLoweringPlanner.cs` | 主入口，plan assembly |
| | `NativeAotLoweringPlanner.Models.cs` | LoweringPlan data structures, PlanNode |
| | `NativeAotLoweringPlanner.Constants.cs` | string IDs, opcode constants |
| **Planning/** | `InvocationPlanning.cs` | 方法调用 lower 决策 |
| | `MetadataSupport.cs` | 元数据初始化 emission 决策 |
| | `StaticInitializationPlanning.cs` | 静态初始化 planning |
| **Emission/** | `MethodEmission.cs` | 方法体 emission, 指令 dispatch, 结构化控制流 |
| | `ExceptionEmission.cs` | EH C++ 生成（5 shapes） |
| | `StructuredControlFlow.cs` | CFG 构建, basic block 分析 |
| | `StructuredIR.cs` | TryBuildStructuredMethodBody, EmitIRSwitch |
| | `ObjectModelEmission.cs` | TypeInfo, VTable, IfaceMap emission |
| | `ObjectModelUtilities.cs` | SubjectId utilities, stable_id, type name |
| | `ObjectEqualityEmission.cs` | object 相等性比较 emission |
| | `StringIdEmission.cs` | string table emission |
| | `ReflectionObjectEmission.cs` | reflection object emission |
| | `GenericRegistration.cs` | Gen-2 Handle 方案 emission |
| | `GenericSharing.cs` | generic sharing canonical map |
| | `ModuleRegistration.cs` | ModuleRegistrationV0 emission |
| | `GeneratedModule.cs` | A1+A2 typed dispatch table |
| | `GcSlotMap.cs` | GC slot map segment |
| | `GcTypeLayout.cs` | GC type layout |
| | `IntervalAnalysis.cs` | interval analysis |
| | `NativeAotTemplateCatalog.cs` | Scriban template catalog |
| **RuntimeSupport/** | `RuntimeSupport.cs` | 运行时 support 注册入口 |
| | `RuntimePrelude.cs` | prelude code gen (includes, forward decls) |
| | 5 个 `ExternalRuntimeHelpers.*.cs` | 外部运行时 helper |
| | `BridgeMethodResolver.cs` | bridge method 解析 |
| | `EnumMetadataExtractor.cs` | enum metadata C++ header 生成 |
| | `CollectionStubs.cs` | collection stubs |
| | `RuntimeHelperShapeRegistry.cs` | runtime helper shape registry |

### Scriban 模板系统

| 模板类别 | 模板文件 |
|---------|---------|
| **Core** | TranslationUnit, ObjectModel, Method, RuntimePrelude |
| **Features** | StaticInitialization, AbiReturnPush, AbiArgumentInitialization, StringJoin, ExternalRuntimeHelper |
| **Runtime** | Reflection, GenericRegistration, DispatchEntryCode, CodeRegistration, HotpatchTable |
| **Interop** | DelegateRuntimeSupport, StructMarshalling |
| **Metadata** | GeneratedModule, InterfaceTypeId, TypeInfo, VTableArray |

### Snapshot 测试

| 项目 | 说明 |
|------|------|
| 位置 | `tests/snapshots/Chaos.IL2CPP.CodeGen.SnapshotTests/` |
| 基线 | 88 个夹具目录（01-simple-add 到 88-eh-fault-finally） |
| 核心文件 | `SnapshotTestBase.cs`, `SnapshotTests.cs`, `FixtureRepository.cs` |
| 更新方式 | `SNAPSHOT_UPDATE=1` 环境变量 |
| 每个夹具需要 | 5 个 JSON 文件（lowering-plan, aot-core-ir, closure-manifest, metadata-registration, supplemental-metadata-template） |

### 关键约束

1. **codegen 输出必须使用 `CHAOS_IL2CPP_` 宏** — `CHAOS_IL2CPP_NEW_GC`, `CHAOS_IL2CPP_MALLOC` 等
2. **禁止全局 operator new/delete 重载** — 生成代码引入游戏引擎后，全局重载会污染引擎
3. **CMakeLists.txt 由 TPG 生成** — codegen 不生成 CMake 文件
4. **SDK 输出自包含** — `--sdk-out` 包含头文件、预构建库、CMake 集成、运行时存根

---

## 执行流程

### 🚨 第一步：输出分类声明（强制阻断）

**在使用任何工具之前，必须先输出：**

```
classification: domains=[CodeGen] mode=knowledge-inject expert=dev-il2cpp-codegen-expert
```

**不输出分类声明，不得进行任何文件操作。**

---

### Step 0：架构语境加载（Architecture Pre-check）

1. **确认修改范围** — 属于 25 个 Planner 文件中的哪个？
2. **检查架构约束**：
   - 结构化恢复率必须保持 100%（`flatFallbackCount` MUST be 0）
   - Symbol 命名规范（`SanitizeSubjectId` + `chaos_*_` prefix）
   - AOT/JIT 模式约束（`--mode aot|jit`）
3. **如果涉及 Scriban 模板** — 阅读 `NativeAotTemplateCatalog.cs` 找到对应模板，确认模板变量定义
4. **如果涉及 AOT/JIT 模式** — 阅读 [`30-模式间切换指南.md`](../../../wiki/03-功能模块/06-il2cpp核心架构/01-翻译管线/30-模式间切换指南.md)
5. **如果是新增 IL 指令** — 必须先走 `dev-il2cpp-translation-expert` 完成翻译路径设计
6. **如果是 PatchData 修改** — 检查 PatchDataExtractor + patch_data.h 格式一致性

### Step 1：实现

1. **Planner 修改**：
   - 在正确的 partial class 文件中修改
   - 遵循已有的模式（`Try*` 模式、`Emit*` 模式）

2. **Scriban 模板修改**：
   - 模板文件在 `Templates/` 目录
   - 修改后必须运行 snapshot 测试验证输出变更

3. **新增 IR opcode**（T4/JIT）：
   - 阅读 [`27-T4原生代码生成器架构.md`](../../../wiki/03-功能模块/06-il2cpp核心架构/01-翻译管线/27-T4原生代码生成器架构.md)
   - 在 `NativeCodeGenerator::EmitInstruction` 添加 dispatch

4. **PatchData 修改**：
   - `PatchDataExtractor.cs` — 序列化
   - `contracts/native/v0/patch_data.h` — 格式定义
   - `patch_loader.cpp` — 运行时加载

### Step 3：验证

1. **编译**：`dotnet build` C# 项目
2. **Snapshot 测试**：运行 `tests/snapshots/` 下的 snapshot 测试
   - 如果基线需更新：`SNAPSHOT_UPDATE=1`
   - 检查 git diff 确认基线变更符合预期
3. **验证器**：运行 `NativeCodegenValidator`（9 条规则）
4. **集成验证**：如果涉及 AOT 输出变更 → 运行 foundation-dll 验证管线

---

## 架构约束

1. **Pipeline 五层职责**：CodeGen 是 stage 5（Loader→SemanticWorld→Linker→MetadataWriter→CodeGen→Emitter）
   - Emitter 在 PipelinePlan 之外，由 `DriverEntry.Run` 直接调用
   - CodeGen 不调用 Emitter，只生产 LoweringPlan

2. **结构化恢复**：所有方法必须达到 100% `structuredRecoveryRate`（当前已达成 35/35 PASS, 53/53 snapshot pass）
   - 禁止回退到 flat-goto（`flatFallbackCount` MUST be 0）

3. **Symbol 命名规范**：所有生成的 C++ symbol 经过 `SanitizeSubjectId` 处理
   - prefix: `chaos_type_`, `chaos_valuetype_`, `chaos_type_id_`, `chaos_static_`, `chaos_boxed_type_`, `chaos_string_id_`

4. **AOT/JIT 模式**：`--mode aot|jit` 编译时开关
   - AOT: 完整 C++ 函数体
   - JIT: AotCoreIr JSON + precode stub + `.jit-entries` section

---

## 输出约束

1. **Snapshot 基线必须更新** — 任何 emitter/planner 输出变更必须运行 snapshot 测试
2. **`CHAOS_IL2CPP_` 宏约束** — 生成代码不得包含裸 `new`/`delete`/`malloc`/`free`
3. **不生成 CMake 文件** — CMakeLists.txt 由 TPG 通过 Scriban 模板生成
4. **SDK 输出自包含** — 运行时存根必须从 SDK 的 `runtime_stubs/` 拷贝
5. **commit message 要求** — 修改完成后必须包含三段式根因：
   - `root_cause` — 一句话根因
   - `fix_strategy` — 修复策略
   - `regression_check` — 验证范围

## 执行前 Checklist

```
□ 修改属于 25 个 Planner 文件中的哪个？
□ 涉及模板修改吗？→ 确认 NativeAotTemplateCatalog.cs 对应模板
□ 涉及 AOT/JIT 模式吗？→ 阅读 30-模式间切换指南.md
□ 如果是新增 IL 指令 → 建议先走 dev-il2cpp-translation-expert
□ CHAOS_IL2CPP_ 宏约束检查
```

## 执行后 Checklist

```
□ dotnet build 编译通过
□ Snapshot 测试通过（88 个基线）
□ 如果基线需更新 → SNAPSHOT_UPDATE=1 → 检查 git diff
□ NativeCodegenValidator（9 条规则）通过
□ 如果涉及 AOT 输出变更 → 运行 foundation-dll 验证管线
□ 结构化恢复率 100%（flatFallbackCount = 0）
```

---

## 集成点

| 上游 | 本 skill | 下游 |
|------|----------|------|
| `dev-il2cpp-core-agent` → CodeGen 路由 | **dev-il2cpp-codegen-expert** | `dev-project-test-governance`（测试治理） |
| 用户直接输入 | | `dev-il2cpp-translation-expert`（翻译路径合作） |
| | | `dev-verification-before-completion`（验证门） |
