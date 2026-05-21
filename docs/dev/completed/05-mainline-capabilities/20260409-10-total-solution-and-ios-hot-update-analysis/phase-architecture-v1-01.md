# 完整 IL2CPP 总方案 — 9 Phase 精细架构设计与执行计划

Date: 2026-04-11
Status: draft-v1
Parent: `docs/dev/in-progress/20260409-10-total-solution-and-ios-hot-update-analysis/roadmap-v1-01.md`

---

## 总体架构基线

当前代码库状态（2026-04-11）：

- **Managed Pipeline**: Loader → SemanticWorld → Linker → MetadataWriter → CodeGen（单程序集闭包）
- **Native Runtime**: bootstrap + runtime-core + support（proof 级，~1600 行 C/C++）
- **CLI 入口**: `chaos-il2cpp convert/build/publish`（manifest 驱动，三种 input.kind）
- **Identity 模型**: `{Assembly}/{Namespace}.{Type}::{Member}` 层级式 SubjectId
- **ABI v0**: `RuntimeAbiV0`（20+ 函数）+ `CodegenBridgeV0`（12+ 函数）
- **Lowering**: 13 个 family，每个对应一种 CLR 特性组合
- **测试**: 10 个 subject，validation profiles，environment matrices，perf baseline（GenericEcho）

---

## 目录结构与分层架构

### 分层原则

```
Layer 0 (Contracts)     ← 纯数据模型，零业务逻辑，不依赖任何其他层
Layer 1 (Processing)    ← 独立处理单元，只依赖 Layer 0
Layer 2 (Orchestration) ← 编排层，依赖 Layer 0 + Layer 1
Layer 3 (Tooling)       ← 开发工具链（Python），不属于产品代码
Layer 4 (Subjects)      ← 测试用例，只被 Layer 3 消费，绝不被 Layer 0-2 引用
```

### Managed 项目结构（src/managed/）

```
src/managed/
│
│  ══════ Layer 0: Contracts ══════
│
├── Chaos.IL2CPP.Contracts/              ← 所有共享数据模型、artifact schema、request/response
│       不依赖任何其他项目
│
│  ══════ Layer 1: Processing（AOT 主线）══════
│
├── Chaos.IL2CPP.Loader/                 ← PE metadata 加载（单/多程序集）
│       依赖: Contracts
│
├── Chaos.IL2CPP.ProjectGraph/           ← 多工程图解析、MSBuild 接线、依赖闭包
│       依赖: Contracts                   [Phase 1 新增]
│
├── Chaos.IL2CPP.SemanticWorld/          ← IL 语义分析、capability 识别、shape 分类
│       依赖: Contracts, Loader
│
├── Chaos.IL2CPP.Linker/                 ← 可达性分析、metadata stripping、optimization facts
│       依赖: Contracts, SemanticWorld
│
├── Chaos.IL2CPP.MetadataWriter/         ← 元数据注册表生成（slot assignment、registration tables）
│       依赖: Contracts, Linker
│
├── Chaos.IL2CPP.CodeGen/               ← C++ 代码生成（lowering families、模板渲染）
│       依赖: Contracts, MetadataWriter, Scriban
│
├── Chaos.IL2CPP.Debug/                  ← 调试元数据生成（IL→C++ 行号映射、stack trace recovery）
│       依赖: Contracts, CodeGen          [Phase 2 新增]
│
│  ══════ Layer 1: Processing（引擎绑定）══════
│
├── Chaos.IL2CPP.EngineBinding/          ← 引擎 ABI 定义、binding 代码生成、ownership 协议
│       依赖: Contracts                   [Phase 3 新增]
│       禁止依赖: Loader, SemanticWorld, Linker, CodeGen（引擎层不依赖 pipeline 内部）
│
│  ══════ Layer 1: Processing（热更主线）══════
│
├── Chaos.IL2CPP.HotUpdate/              ← package loader、supplemental metadata、bridge generation
│       依赖: Contracts                   [Phase 5-6 新增]
│       禁止依赖: CodeGen（热更不依赖 AOT codegen）
│
├── Chaos.IL2CPP.Interpreter/            ← IL→IR lowering、InterpreterVM、mixed execution dispatch
│       依赖: Contracts, HotUpdate        [Phase 7 新增]
│       禁止依赖: CodeGen, Loader, SemanticWorld（解释器不走 AOT pipeline）
│
│  ══════ Layer 2: Orchestration ══════
│
├── Chaos.IL2CPP.Pipeline/               ← AOT 管线编排（Loader→Semantic→Linker→Writer→CodeGen）
│       依赖: Contracts + 所有 Layer 1 AOT 模块
│
└── Chaos.IL2CPP.Driver/                 ← CLI 入口（convert/build/publish 子命令）
        依赖: Pipeline, CodeGen, Contracts
        Phase 5+: 追加依赖 HotUpdate, Interpreter
```

### Native 项目结构（src/native/）

```
src/native/
│
│  ══════ 核心运行时（不含 subject 逻辑）══════
│
├── runtime-core/                        ← 运行时核心：对象模型、GC、线程、异常
│       CMakeLists.txt
│       runtime_core.h / runtime_core.cpp
│
├── bootstrap/                           ← 启动注册：codegen registration、metadata tables、method dispatch
│       CMakeLists.txt
│       bootstrap.h / bootstrap.cpp
│
├── support/                             ← 通用工具函数
│       CMakeLists.txt
│       support.h / support.cpp
│
│  ══════ 引擎桥接（Phase 3 新增）══════
│
├── engine-bridge/                       ← 引擎 lifecycle、ownership、callback C 实现
│       CMakeLists.txt
│       engine_bridge.h / engine_bridge.cpp
│       禁止依赖: subjects/ 下的任何代码
│
│  ══════ 热更运行时（Phase 5+ 新增）══════
│
├── hot-update/                          ← package loader、runtime mode switch、metadata supplement loader
│       CMakeLists.txt
│       hot_update.h / hot_update.cpp
│
└── interpreter/                         ← 解释器 VM（Phase 7 新增）
        CMakeLists.txt
        interpreter.h / interpreter.cpp
        ir_opcodes.h                     ← InterpreterIR 指令定义
```

### Contracts 结构（contracts/）

```
contracts/
├── artifacts/v0/schemas/                ← artifact JSON schema（typed-il-ir, aot-manifest, ...）
├── native/v0/                           ← native ABI headers（runtime_abi.h, codegen_bridge.h）
├── docs/v0/                             ← contract 级文档
│
│  ══════ Phase 0 新增 ══════
│
├── shared/v0/                           ← AOT 与热更共享 contract
│   ├── identity-model.md
│   ├── object-model.md
│   ├── handle-model.md
│   ├── abi-calling-convention.md
│   ├── exception-boundary.md
│   ├── delegate-abi.md
│   ├── metadata-token-mapping.md
│   ├── version-policy.md
│   ├── interpreter-ir-decision.md
│   └── package-manifest.schema.json
│
│  ══════ Phase 3 新增 ══════
│
└── engine/v0/                           ← 引擎 ABI contract
    ├── lifecycle.h
    ├── ownership.h
    ├── callback.h
    └── thread-model.md
```

### Subject 结构（subjects/）— 严格隔离

```
subjects/                                ← 全部是测试用例，绝不被 src/ 引用
├── Directory.Build.props                ← 共享 MSBuild 属性
│
│  ══════ 现有 subjects ══════
│
├── HelloWorld/
├── HelloWorldObject/
├── GenericEcho/
├── MainlineFeaturePack/
├── PInvokeLite/
├── ReflectionLite/
├── EngineLogWriteLite/
├── EngineObjectHandleLite/
├── EngineLifecycleCallbackLite/
├── HostEmbeddingLite/
│
│  ══════ Phase 1 新增（golden projects）══════
│
├── golden/
│   ├── GoldenSimpleLib/
│   ├── GoldenMultiProject/
│   └── GoldenWithPackage/
│
│  ══════ Phase 2 新增 ══════
│
├── InterfaceDispatchProof/
├── AsyncAwaitProof/
├── ThreadingProof/
├── NestedExceptionProof/
├── MarshalingProof/
│
│  ══════ Phase 3 新增 ══════
│
├── EngineHostProof/
│
│  ══════ Phase 4 新增 ══════
│
├── MobileHelloWorldProof/
│
│  ══════ Phase 5-7 新增 ══════
│
├── SharedContractProof/
├── HotUpdateSkeletonProof/
├── BridgeRoundtripProof/
├── GenericSupplementProof/
├── MixedExecutionProof/
├── InterpreterArithmeticProof/
│
│  ══════ Phase 8 新增 ══════
│
└── CompatibilityMatrixProof/
```

### 禁止规则（硬约束）

| 规则编号 | 描述 | 检测方式 |
| --- | --- | --- |
| **L-01** | `src/managed/` 不得 import/reference `subjects/` 下的任何代码 | `test_repo_layout.py` 断言 |
| **L-02** | `src/native/` 不得 `#include` `subjects/` 下的任何头文件 | CMake + test 断言 |
| **L-03** | Layer 0 (Contracts) 不得依赖任何其他 managed 项目 | .csproj ProjectReference 检查 |
| **L-04** | Layer 1 模块之间不得循环依赖 | `test_repo_layout.py` 断言 |
| **L-05** | `Chaos.IL2CPP.Interpreter` 不得依赖 `Chaos.IL2CPP.CodeGen` | .csproj ProjectReference 检查 |
| **L-06** | `Chaos.IL2CPP.HotUpdate` 不得依赖 `Chaos.IL2CPP.CodeGen` | .csproj ProjectReference 检查 |
| **L-07** | `Chaos.IL2CPP.EngineBinding` 不得依赖 pipeline 内部模块（Loader/Semantic/Linker/CodeGen） | .csproj ProjectReference 检查 |
| **L-08** | `build/toolchains/run/` 中的 Python 工具代码不得硬编码 subject 名称 | `test_repo_layout.py` 断言 |

### Mobile Host 结构（Phase 4 新增）

```
src/mobile/                              ← 移动端宿主，不属于 IL2CPP 核心
├── android/
│   ├── app/                             ← 最小 Android Activity
│   ├── jni/                             ← JNI bridge
│   └── CMakeLists.txt
├── ios/
│   ├── app/                             ← 最小 iOS App Delegate
│   ├── bridge/                          ← ObjC bridge
│   └── CMakeLists.txt
└── shared/
    ├── host_main.cpp                    ← 统一入口
    └── result_reporter.cpp              ← 结果上报
```

### Deploy 输出结构

```
deploy/                                  ← CLI build 输出的最终产物
└── <SubjectId>/
    ├── bin/<target-id>/                 ← 编译产物
    ├── lib/                             ← 运行时依赖
    └── manifest.json                    ← 部署清单
```

## Phase 0: Shared Contract Freeze

### 0.1 架构设计

#### 目标

冻结 AOT 主线与热更运行模式共享的底层模型。一旦冻结，后续 Phase 不能单方面修改这些 contract。

#### 需要冻结的 contract 清单

| Contract | 当前状态 | 冻结目标 |
| --- | --- | --- |
| **Type Identity** | `{Assembly}/{Namespace}.{Type}` 已有 | 冻结 SubjectId 格式 + generic instantiation 编码规则 + 跨版本 stable token |
| **Method Identity** | `{Type}::{Method}({Params})` 已有 | 冻结签名编码（含 generic arity）+ overload disambiguation |
| **Field Identity** | `{Type}::{Field}` 已有 | 冻结 |
| **Object Model** | runtime_core 中有 ObjectHeader/StringObject/ArrayObject | 冻结 header layout + GC bits + type info slot |
| **Handle Model** | `TypeInfoHandle`, `MethodInfoHandle`, etc. 已有 | 冻结 handle 为 opaque pointer，不可序列化，不可跨进程 |
| **ABI Calling Convention** | `CHAOS_RUNTIME_ABI_EXPORT` / `CHAOS_RUNTIME_ABI_CALL` 已有 | 冻结调用约定 + 参数传递规则 + 返回值约定 |
| **Metadata Token** | `uint32_t` token（PE 格式） | 冻结 token → stable identity 的映射策略（用于热更跨版本引用） |
| **Exception Boundary** | `out_exception` 模式已有 | 冻结异常传播边界 + managed/native 切换点 |
| **Delegate ABI** | `create_delegate` / `delegate_invoke` 已有 | 冻结 delegate 结构 + 函数指针 + target 布局 |
| **Package Manifest** | 无 | 新建 hot update package manifest v0 schema |
| **Version Policy** | `formatVersion: "v0"` 已有 | 冻结 schema 版本策略 + struct_size 向前兼容规则 |

#### Interpreter IR 架构决策

在 Phase 0 必须做出的架构决策：

**选项 A: 直接解释 IL 字节码**
- 优点：不需要额外 IR 转换步骤
- 缺点：IL 指令集庞大（200+ opcode），解释器实现成本高

**选项 B: IL → 内部 IR → 解释执行**
- 优点：可设计更紧凑的指令集，优化解释性能
- 缺点：需要额外的 lowering pass

**选项 C: IL → TypedIL IR（复用现有 TypedIlIrArtifact）→ 解释执行**
- 优点：复用现有 SemanticWorld 的 capability/shape 分析
- 缺点：TypedIL 目前为 JSON 序列化格式，不适合直接解释

**推荐：选项 B**，但 IR 设计必须与 Phase 0 的 identity/metadata 模型兼容。

### 0.2 执行条目

```
- [ ] 0.2.1  编写 `contracts/shared/v0/identity-model.md`：冻结 SubjectId 格式规范
- [ ] 0.2.2  编写 `contracts/shared/v0/object-model.md`：冻结 ObjectHeader 布局（header 字段、GC bits、type slot、sync block）
- [ ] 0.2.3  编写 `contracts/shared/v0/handle-model.md`：冻结 handle 类型列表与不透明约束
- [ ] 0.2.4  编写 `contracts/shared/v0/abi-calling-convention.md`：冻结 export/call 宏、参数顺序、返回值、错误码
- [ ] 0.2.5  编写 `contracts/shared/v0/exception-boundary.md`：冻结 out_exception 模式 + managed/native EH 切换点
- [ ] 0.2.6  编写 `contracts/shared/v0/delegate-abi.md`：冻结 delegate 内存布局 + 函数指针 + method_token + target
- [ ] 0.2.7  编写 `contracts/shared/v0/metadata-token-mapping.md`：冻结 PE token → stable identity 映射策略
- [ ] 0.2.8  编写 `contracts/shared/v0/package-manifest.schema.json`：hot update package manifest v0
- [ ] 0.2.9  编写 `contracts/shared/v0/version-policy.md`：冻结 formatVersion 策略 + struct_size 兼容规则
- [ ] 0.2.10 编写 `contracts/shared/v0/interpreter-ir-decision.md`：记录 IR 架构决策（选项 B）+ IR 指令集初步设计
- [ ] 0.2.11 为每个 contract 编写 snapshot test（schema round-trip）
- [ ] 0.2.12 编写 AOT + hot-update mock 双边验证样例（验证同一组 identity/ABI 可在两边使用）
```

### 0.3 验证产物

| 产物 | 路径 | 验证方式 |
| --- | --- | --- |
| Identity model spec | `contracts/shared/v0/identity-model.md` | 现有 10 个 subject 的 SubjectId 全部符合规范 |
| Object model spec | `contracts/shared/v0/object-model.md` | runtime_core 的 ObjectHeader 布局与 spec 一致 |
| Package manifest schema | `contracts/shared/v0/package-manifest.schema.json` | JSON schema validation 通过 |
| Snapshot tests | `tests/contracts/shared/` | `python -m pytest tests/contracts/shared/ -v` |
| 双边 mock 样例 | `tests/contracts/shared/test_dual_mode_identity.py` | AOT mock 和 hot-update mock 可互操作 |

### 0.4 Subject 验收

- 现有 10 个 subject 的 `subject.manifest.json` 中所有 `source.entry` 字段符合冻结的 identity 格式
- `contracts/native/v0/runtime_abi.h` 和 `codegen_bridge.h` 中所有 handle 类型符合冻结的 handle model
- 新增 subject `SharedContractProof`：一个最小 C# 程序 + 对应的 hot-update mock package，验证同一组 identity 在两边可用

---

## Phase 1: Full Project Ingestion And Build Graph

### 1.1 架构设计

#### 目标

把 Pipeline 的输入层从单 PE 程序集升级到完整的 project/solution graph。

#### 当前输入模型

```
ManagedClosureRequest(InputAssemblyPath, OutputRootPath, EntryPointSubjectIdOverride?)
```

LoaderStage 通过 `PEReader` 读取单个 DLL。

#### 目标输入模型

```
ProjectGraphRequest {
    ProjectPath: string             // .csproj 或 .sln 路径
    OutputRootPath: string
    Configuration: "Debug" | "Release"
    TargetFramework: "net8.0" | ...
    RuntimeIdentifier?: string      // e.g., "win-x64", "linux-x64"
    EntryPointOverride?: string
}
```

#### 架构分层

```
ProjectGraphRequest
    ↓
ProjectGraphLoader（新模块）
    ├── MSBuild design-time evaluation（读取 .csproj 依赖图）
    ├── NuGet package resolution（解析 PackageReference）
    ├── Assembly output discovery（找到所有编译产物 DLL）
    └── 输出 ProjectGraph { Projects[], Dependencies[], EntryProject }
    ↓
MultiAssemblyLoader（扩展现有 LoaderStage）
    ├── 依次加载每个 assembly
    ├── 建立 cross-assembly reference index
    └── 输出 LoadedWorldModel { Assemblies[], CrossReferences[] }
    ↓
现有 Pipeline（SemanticWorld → Linker → MetadataWriter → CodeGen）
    但扩展为处理 multi-assembly 输入
```

#### MSBuild 接线策略

**轻量方案（推荐）**：使用 `dotnet msbuild -getProperty` / `-getItem` 或 `Microsoft.Build.Locator` + `Microsoft.Build.Evaluation`：
- 读取 ProjectReference 图
- 读取 PackageReference 依赖
- 获取编译输出路径
- 不做完整 design-time build

#### LoaderStage 扩展

当前 `LoaderStage.Load()` 加载单个 assembly。扩展为：

```csharp
// 新增
public LoadedWorldModel LoadMultiple(IReadOnlyList<string> assemblyPaths, string entryAssemblyPath)
{
    var assemblies = assemblyPaths.Select(path => Load(path)).ToList();
    var crossReferences = BuildCrossReferenceIndex(assemblies);
    return new LoadedWorldModel(assemblies, crossReferences, entryAssemblyPath);
}
```

### 1.2 执行条目

```
- [ ] 1.2.1  新建 `src/managed/Chaos.IL2CPP.ProjectGraph/` 项目
- [ ] 1.2.2  实现 ProjectGraphLoader：读取 .csproj → 解析 ProjectReference 闭包
- [ ] 1.2.3  实现 PackageReference 基础解析（读取 obj/project.assets.json）
- [ ] 1.2.4  实现 assembly output discovery（从 build output 找到所有 DLL）
- [ ] 1.2.5  定义 ProjectGraph 数据模型（Projects, Dependencies, EntryProject）
- [ ] 1.2.6  定义 LoadedWorldModel 数据模型（Assemblies[], CrossReferences[]）
- [ ] 1.2.7  扩展 LoaderStage 支持 LoadMultiple
- [ ] 1.2.8  建立 cross-assembly reference index（assembly A 的方法调用 assembly B 的类型）
- [ ] 1.2.9  扩展 ManagedClosureRequest 或新增 ProjectGraphRequest
- [ ] 1.2.10 扩展 SemanticWorld 处理 multi-assembly input
- [ ] 1.2.11 扩展 Linker 的 reachability analysis 跨 assembly 边界
- [ ] 1.2.12 更新 chaos-il2cpp convert 支持 multi-project 输入
- [ ] 1.2.13 创建 3 个 golden project fixtures（见下方）
- [ ] 1.2.14 编写 project graph snapshot tests
- [ ] 1.2.15 编写 incremental build detection baseline
```

### 1.3 Golden Projects

| Golden Project | 结构 | 验证目标 |
| --- | --- | --- |
| `GoldenSimpleLib` | 1 个 .csproj 引用 1 个 class library | 最基础的 ProjectReference |
| `GoldenMultiProject` | 3 个 .csproj 互相引用 + 1 个 entry | 多级 ProjectReference 图 |
| `GoldenWithPackage` | 1 个 .csproj + 1 个 NuGet PackageReference | 外部依赖解析 |

放置位置：`subjects/golden/` 下，复用现有 subject 测试框架。

### 1.4 验证产物

| 产物 | 验证方式 |
| --- | --- |
| ProjectGraphLoader | 3 个 golden project 的 graph 输出与 snapshot 一致 |
| LoadedWorldModel | multi-assembly 加载后 cross-reference index 正确 |
| Pipeline 端到端 | `chaos-il2cpp convert subjects/golden/GoldenMultiProject` 成功输出 |
| Snapshot tests | `python -m pytest tests/unit/run/test_project_graph.py -v` |

### 1.5 性能验证

- `GoldenMultiProject`（3 个项目，~50 个类型）的 convert 时间应 < 10 秒
- 增量构建：第二次 convert（无变更）应 < 2 秒

---

## Phase 2: AOT Mainline Runtime Completeness

### 2.1 架构设计

#### 目标

把 AOT 主线从 proof-level 能力补齐到可承担主包代码的程度。

#### 能力扩展矩阵

| 能力类别 | 当前状态 | Phase 2 目标 |
| --- | --- | --- |
| **Generic instantiation** | 仅 closed generic 识别 | rgctx / shared generic / VTable 调度 |
| **Interface dispatch** | 仅 callvirt devirtualization | 接口表 + 约束调用 |
| **async/await** | 无 | 状态机识别 + async entry lowering |
| **Threading** | thread_attach/detach 骨架 | ThreadStatic, lock, Monitor, volatile |
| **GC** | gc_handle_new/free 骨架 | safepoint + root reporting + finalizer |
| **EH** | throw/catch/finally proof | nested EH + filter + fault |
| **Reflection** | ldtoken + GetField/GetMethod | GetProperties, GetEvents, Activator.CreateInstance |
| **Interop** | DllImport proof | blittable/non-blittable marshal, reverse P/Invoke |
| **Linker** | entry reachability + devirtualization | metadata stripping + conditional preserve |
| **Debug** | 无 | managed stack trace + crash mapping |

#### Linker 扩展架构

```
LinkerStage（扩展）
    ├── ReachabilityAnalysis（现有）
    │     └── 扩展：跨 assembly + interface + generic instantiation
    ├── MetadataStripping（新增）
    │     ├── 标记 unreachable types/methods
    │     ├── 支持 [Preserve] 属性
    │     └── 输出 stripping report
    └── OptimizationFacts（扩展）
          ├── 扩展 DispatchFacts：interface dispatch
          ├── 新增 GenericFacts：instantiation sharing decisions
          └── 新增 AsyncFacts：状态机 lowering 决策
```

#### Debug Baseline 架构

```
DebugMetadataWriter（新模块）
    ├── 为每个 generated C++ 函数记录：
    │     ├── 原始 C# 方法 SubjectId
    │     ├── IL offset → C++ line number 映射
    │     └── 局部变量名映射
    ├── 输出 debug-metadata.json
    └── 在 native crash handler 中读取 → 恢复 managed stack trace
```

### 2.2 执行条目

```
- [ ] 2.2.1  扩展 SemanticWorld：识别 interface implementation + constraint call
- [ ] 2.2.2  扩展 Linker：interface dispatch resolution + VTable 构建
- [ ] 2.2.3  扩展 CodeGen：新增 lowering family `interface-dispatch`
- [ ] 2.2.4  扩展 SemanticWorld：识别 async 状态机（IAsyncStateMachine）
- [ ] 2.2.5  扩展 CodeGen：async 状态机 lowering（展开为同步调用链）
- [ ] 2.2.6  扩展 runtime_core：ThreadStatic storage + Monitor enter/exit
- [ ] 2.2.7  扩展 runtime_core：GC safepoint + root reporting interface
- [ ] 2.2.8  扩展 CodeGen：nested EH + filter + fault handlers
- [ ] 2.2.9  扩展 SemanticWorld：更完整的 reflection capability 识别
- [ ] 2.2.10 扩展 Linker：metadata stripping + [Preserve] support
- [ ] 2.2.11 扩展 CodeGen：blittable/non-blittable marshaling
- [ ] 2.2.12 实现 DebugMetadataWriter：IL offset → C++ line mapping
- [ ] 2.2.13 实现 native crash handler 中的 managed stack trace recovery
- [ ] 2.2.14 建立 native perf baseline framework
- [ ] 2.2.15 为每个新增能力创建 subject（见下方）
```

### 2.3 新增 Subjects

| Subject | 验证能力 | 性能指标 |
| --- | --- | --- |
| `InterfaceDispatchProof` | 接口调用 + 约束调用 | dispatch overhead < 5ns |
| `AsyncAwaitProof` | async 方法 + Task.WhenAll | 状态机展开正确性 |
| `ThreadingProof` | lock + ThreadStatic + volatile | 多线程正确性 |
| `NestedExceptionProof` | nested try-catch-finally + filter | EH 正确性 |
| `MarshalingProof` | blittable struct + string marshal | P/Invoke 往返正确性 |

### 2.4 性能验证规划

| 指标 | 基线目标 | 回归阈值 |
| --- | --- | --- |
| GenericEcho 执行时间 | meanDurationMs ≤ 12.0 | +20% 回归报警 |
| 接口 dispatch 单次调用 | ≤ 5ns | +50% 回归报警 |
| 方法调用 overhead vs 直接 C++ | ≤ 3x | +30% 回归报警 |
| metadata stripping 率 | ≥ 60% unreachable types 被剥离 | 低于 40% 报警 |
| convert 时间（GoldenMultiProject） | ≤ 10s | +100% 回归报警 |

---

## Phase 3: Engine Binding Contract

### 3.1 架构设计

#### 目标

把引擎接入从 proof-level helper 升级为正式的 engine-facing contract。

#### Engine ABI 分层

```
Layer 0: Runtime ABI（Phase 0 已冻结）
    ├── runtime_init / shutdown
    ├── object allocation
    ├── GC handle
    └── method invoke

Layer 1: Codegen Bridge（Phase 0 已冻结）
    ├── register_codegen / bootstrap_runtime
    ├── metadata resolution
    └── virtual dispatch

Layer 2: Engine Contract（Phase 3 新增）
    ├── Lifecycle: engine_init / engine_shutdown / engine_reload / engine_scene_attach
    ├── Ownership: engine_retain_object / engine_release_object / engine_transfer_ownership
    ├── Callback: engine_register_callback / engine_dispatch_callback
    ├── Thread: engine_post_to_main_thread / engine_is_main_thread
    ├── Serialization: engine_serialize_object / engine_deserialize_object
    └── Binding Generation: auto-generated C++ <-> C# wrapper
```

#### Ownership Model

```
                    ┌──────────────────┐
                    │  Engine Object   │
                    │  (native owner)  │
                    └────────┬─────────┘
                             │ GCHandle (weak/strong)
                    ┌────────▼─────────┐
                    │  Managed Proxy   │
                    │  (GC tracked)    │
                    └──────────────────┘

规则：
  - Engine 持有 native 对象的 ownership
  - Managed 通过 GCHandle 引用 engine 对象
  - Engine 释放时通知 managed 侧 invalidate handle
  - Managed GC 回收 proxy 时通知 engine 侧 release ref
```

### 3.2 执行条目

```
- [ ] 3.2.1  编写 `contracts/engine/v0/lifecycle.h`：engine lifecycle ABI
- [ ] 3.2.2  编写 `contracts/engine/v0/ownership.h`：cross-boundary ownership protocol
- [ ] 3.2.3  编写 `contracts/engine/v0/callback.h`：callback registration + dispatch ABI
- [ ] 3.2.4  编写 `contracts/engine/v0/thread-model.md`：engine thread 约束文档
- [ ] 3.2.5  实现 runtime_core 扩展：engine lifecycle hook 注册/派发
- [ ] 3.2.6  实现 runtime_core 扩展：ownership bridge（GCHandle ↔ engine ref）
- [ ] 3.2.7  实现 callback dispatch：engine → managed + managed → engine
- [ ] 3.2.8  实现 engine mock host：最小宿主完成 init → callback → shutdown 流程
- [ ] 3.2.9  编写 binding generation baseline（从 C# interface → C++ wrapper stub）
- [ ] 3.2.10 新增 subject `EngineHostProof`：完整 lifecycle + ownership + callback 验证
```

### 3.3 验证产物

| 产物 | 验证方式 |
| --- | --- |
| Engine lifecycle ABI | engine mock host 完成 init → register → callback → shutdown |
| Ownership protocol | managed proxy GC 回收后 engine 收到 release 通知 |
| Callback bridge | engine 侧 C++ 调 managed C# → 拿到正确返回值 |
| Binding generation | 从 `[EngineExport]` 标注的 C# interface 自动生成 C++ wrapper |

### 3.4 Subject 验收

- `EngineHostProof`：lifecycle stress test（1000 次 init/shutdown 无泄漏）
- `EngineLogWriteLite` / `EngineObjectHandleLite` / `EngineLifecycleCallbackLite`：现有 subject 继续通过
- `HostEmbeddingLite`：扩展为使用新的 ownership protocol

---

## Phase 4: Mobile Runtime And Host Matrix

### 4.1 架构设计

#### 目标

把 Android/iOS 从工具链 gate 推进到有运行时证据的 runtime/host 级验证。

#### 平台验证矩阵

| 平台 | 当前 Gate | Phase 4 目标 |
| --- | --- | --- |
| Windows x64 | reference desktop ✓ | 保持 |
| macOS arm64 | reference desktop ✓ | 保持 |
| Android arm64 | startup smoke | runtime subset + host harness |
| iOS arm64 | compile/link/packaging | simulator + device host + runtime subset |
| Linux x64 | packaging | runtime subset（低优先级） |

#### Mobile Host Harness 架构

```
mobile-host/
  android/
    app/                    ← 最小 Android Activity
    jni/                    ← JNI bridge: Java → native → IL2CPP runtime
    CMakeLists.txt
  ios/
    app/                    ← 最小 iOS App Delegate
    bridge/                 ← Objective-C → C → IL2CPP runtime
    CMakeLists.txt
  shared/
    host_main.cpp           ← 统一入口：init runtime → run entry → report result
    result_reporter.cpp     ← 结果上报（stdout/file/logcat/NSLog）
```

### 4.2 执行条目

```
- [ ] 4.2.1  创建 Android host harness（最小 Activity + JNI bridge）
- [ ] 4.2.2  创建 iOS host harness（最小 App Delegate + ObjC bridge）
- [ ] 4.2.3  实现 shared host_main.cpp（统一入口）
- [ ] 4.2.4  实现 result_reporter（平台适配日志输出）
- [ ] 4.2.5  Android：从 compile/packaging 升级到 startup + HelloWorld runtime
- [ ] 4.2.6  iOS simulator：startup + HelloWorld runtime
- [ ] 4.2.7  iOS device：code signing + provisioning + startup
- [ ] 4.2.8  建立 crash/symbol 收集流程（dsym/ndk-stack）
- [ ] 4.2.9  建立 package size baseline
- [ ] 4.2.10 新增 subject `MobileHelloWorldProof`：在 Android + iOS 上运行 HelloWorld
```

### 4.3 性能验证规划

| 指标 | Android arm64 | iOS arm64 |
| --- | --- | --- |
| 冷启动时间（runtime init → entry） | ≤ 500ms | ≤ 300ms |
| Package size（stripped） | ≤ 5MB | ≤ 3MB |
| 内存占用（idle） | ≤ 20MB | ≤ 15MB |

---

## Phase 5: Hot Update Runtime Skeleton

### 5.1 架构设计

#### 目标

建立热更运行模式的最小骨架。**先在 desktop 验证**，不等 mobile。

#### Runtime Mode 架构

```
                        ┌─────────────────────────┐
                        │     RuntimeManager       │
                        │   mode: AOT | Mixed      │
                        └─────────┬───────────────┘
                                  │
              ┌───────────────────┼───────────────────┐
              ▼                                        ▼
     ┌─────────────────┐                    ┌──────────────────┐
     │  AOT Execution   │                    │  HotUpdate       │
     │  (generated C++) │                    │  Execution       │
     │  ← 现有主线      │                    │  (interpreter)   │
     └─────────────────┘                    └──────────────────┘
              │                                        │
              └────────────── Shared ABI ──────────────┘
                      (Phase 0 冻结的 contract)
```

#### Hot Update Package Format

```json
{
  "formatVersion": "v0",
  "packageId": "com.example.hotupdate.2026-04-15",
  "targetAotVersion": "1.0.0",
  "assemblies": [
    {
      "name": "HotPatch.dll",
      "hash": "sha256:...",
      "size": 12345,
      "entryPoint": "HotPatch/Patch::Apply()"
    }
  ],
  "supplementalMetadata": "metadata-supplement.bin",
  "signature": "..."
}
```

#### Package Loader 流程

```
1. 读取 package manifest
2. 验证 signature（如果启用）
3. 验证 targetAotVersion 兼容性
4. 加载 assembly IL（不执行）
5. 注册 supplemental metadata（Phase 6）
6. 切换 runtime mode 到 Mixed
7. 可通过 interpreter 执行 hot update 方法
```

### 5.2 执行条目

```
- [ ] 5.2.1  定义 HotUpdatePackage 数据结构（C#/C++ 双侧）
- [ ] 5.2.2  实现 PackageLoader：读取 + 验证 + 注册
- [ ] 5.2.3  实现 RuntimeMode switch（AOT ↔ Mixed）
- [ ] 5.2.4  实现 assembly IL 内存加载（不走 PEReader 文件路径）
- [ ] 5.2.5  实现最小 interpreter stub（只执行 `ldc.i4 42; ret`）
- [ ] 5.2.6  实现 hot update method registration（method token → interpreter entry）
- [ ] 5.2.7  验证 AOT-only mode 无回归
- [ ] 5.2.8  新增 subject `HotUpdateSkeletonProof`
```

### 5.3 Subject 验收

`HotUpdateSkeletonProof`：
1. AOT 主包包含 `MainApp.Main()` 调用 `Helper.GetValue()`
2. `Helper.GetValue()` 在 AOT 中返回 1
3. 加载 hot update package，替换 `Helper.GetValue()` 返回 42
4. 重新调用 → 返回 42
5. 卸载 hot update package → 回到 AOT 默认值 1

---

## Phase 6: Metadata Supplement And Bridge Generation

### 6.1 架构设计

#### 目标

建立 AOT 与热更之间的元数据补充和桥接代码生成。

#### Supplemental Metadata 架构

```
AOT 主包编译时：
    MetadataWriter → code-registration.json + metadata-registration.json
    新增 → supplemental-metadata-template.json（预留槽位给热更类型/方法）

热更包生成时：
    读取 AOT 主包的 template
    分析热更 assembly 引用了哪些 AOT 类型/方法
    生成 supplemental-metadata.bin：
        - 热更类型的 metadata registration
        - AOT generic instantiation 的补充（热更代码可能用到 AOT 没有的泛型实例化）
        - bridge function stubs（AOT 调热更、热更调 AOT）
```

#### Bridge Function 类型

| 桥接方向 | 场景 | 生成策略 |
| --- | --- | --- |
| AOT → HotUpdate | AOT 代码通过 virtual/interface 调用了热更新增的方法 | 预生成 bridge stub，运行时路由到 interpreter |
| HotUpdate → AOT | 热更代码调用 AOT 已有的方法 | 直接通过 method token 查找 AOT 函数指针 |
| HotUpdate → Engine | 热更代码调用引擎 API | 通过 Phase 3 的 engine contract bridge |
| AOT ↔ HotUpdate (delegate) | delegate target 在热更侧 | bridge wrapper 包装 interpreter call |

### 6.2 执行条目

```
- [ ] 6.2.1  扩展 MetadataWriter：输出 supplemental-metadata-template
- [ ] 6.2.2  实现 SupplementalMetadataWriter：从热更 assembly + AOT template → supplement.bin
- [ ] 6.2.3  实现 SupplementalMetadataLoader：runtime 加载 supplement.bin
- [ ] 6.2.4  实现 AOT→HotUpdate bridge stub 生成器
- [ ] 6.2.5  实现 delegate cross-mode wrapper 生成
- [ ] 6.2.6  实现 generic instantiation supplement（热更用到的 AOT 泛型实例化）
- [ ] 6.2.7  新增 subject `BridgeRoundtripProof`
- [ ] 6.2.8  新增 subject `GenericSupplementProof`
```

### 6.3 Subject 验收

`BridgeRoundtripProof`：
1. AOT 有 `IService.Process(int) → int` 接口
2. 热更实现 `HotService : IService { Process(x) => x * 2 }`
3. AOT 通过接口调 `IService.Process(21)` → 返回 42（bridge 路由到 interpreter）
4. 热更调 AOT 的 `Math.Max(1, 2)` → 返回 2

`GenericSupplementProof`：
1. AOT 有 `List<int>` 的 generic instantiation
2. 热更代码使用 `List<string>`（AOT 没有这个实例化）
3. supplement metadata 补充 `List<string>` 的元数据
4. 热更代码成功操作 `List<string>`

---

## Phase 7: Interpreter Core And Mixed Execution

### 7.1 架构设计

#### 目标

建立 IL 解释器核心和 AOT ↔ HotUpdate 混合执行。

#### Interpreter 架构

```
IL Bytecode (from hot update assembly)
    ↓
IL → InterpreterIR Lowering（Phase 0 决策的 IR 格式）
    ↓
InterpreterIR
    ↓
InterpreterVM
    ├── 操作数栈（模拟 CLR evaluation stack）
    ├── 局部变量表
    ├── 参数表
    ├── 调用栈（frames）
    └── 异常处理栈
```

#### IR 指令集（最小子集）

| 类别 | 指令 | 对应 IL |
| --- | --- | --- |
| 常量 | `ir_ldc_i4`, `ir_ldstr`, `ir_ldnull` | ldc.i4.*, ldstr, ldnull |
| 局部变量 | `ir_ldloc`, `ir_stloc` | ldloc.*, stloc.* |
| 参数 | `ir_ldarg`, `ir_starg` | ldarg.*, starg.* |
| 字段 | `ir_ldfld`, `ir_stfld`, `ir_ldsfld`, `ir_stsfld` | ldfld, stfld, ldsfld, stsfld |
| 调用 | `ir_call`, `ir_callvirt`, `ir_call_bridge` | call, callvirt + bridge dispatch |
| 分支 | `ir_br`, `ir_brtrue`, `ir_brfalse`, `ir_beq`, `ir_blt`, ... | br.*, brtrue.*, ... |
| 对象 | `ir_newobj`, `ir_box`, `ir_unbox`, `ir_castclass`, `ir_isinst` | newobj, box, unbox, ... |
| 数组 | `ir_newarr`, `ir_ldelem`, `ir_stelem`, `ir_ldlen` | newarr, ldelem.*, stelem.*, ldlen |
| 异常 | `ir_throw`, `ir_rethrow`, `ir_leave`, `ir_endfinally` | throw, rethrow, leave, endfinally |
| 返回 | `ir_ret` | ret |

#### Mixed Execution 调用链

```
AOT method A
  → callvirt IService.Process(21)
  → bridge stub (generated Phase 6)
    → InterpreterVM.Execute(HotService.Process, args=[21])
      → ir_ldarg 0
      → ir_ldc_i4 2
      → ir_mul
      → ir_ret → result = 42
    ← return 42
  ← return 42
```

### 7.2 执行条目

```
- [ ] 7.2.1  定义 InterpreterIR 数据结构（指令、操作数、基本块）
- [ ] 7.2.2  实现 IL → InterpreterIR lowering（最小子集：ldc, ldloc, stloc, ldarg, call, ret, br）
- [ ] 7.2.3  实现 InterpreterVM：操作数栈 + 局部变量表 + 调用栈
- [ ] 7.2.4  实现基础指令解释：算术、比较、分支、调用
- [ ] 7.2.5  实现对象/数组指令解释（通过 runtime ABI 调用 object_new/array_new）
- [ ] 7.2.6  实现异常处理解释（try-catch-finally + EH dispatcher）
- [ ] 7.2.7  实现 AOT → Interpreter 调用（bridge stub → VM.Execute）
- [ ] 7.2.8  实现 Interpreter → AOT 调用（VM 内遇到 AOT method → 直接 call 函数指针）
- [ ] 7.2.9  实现 Interpreter → Engine 调用（通过 engine contract bridge）
- [ ] 7.2.10 新增 subject `MixedExecutionProof`
- [ ] 7.2.11 新增 subject `InterpreterArithmeticProof`
```

### 7.3 性能验证规划

| 指标 | 目标 |
| --- | --- |
| 解释器纯算术循环 vs native | ≤ 50x 慢（合理的解释器范围） |
| AOT → interpreter 跨边界调用 | ≤ 1μs / call |
| interpreter → AOT 跨边界调用 | ≤ 500ns / call |
| 热更方法首次执行 (IL → IR + execute) | ≤ 10ms |

### 7.4 Subject 验收

`MixedExecutionProof`：
1. AOT 主包有 `Calculator.Add(a, b)` 和 `IFormatter.Format(result)`
2. 热更 `HotFormatter : IFormatter { Format(x) => $"Result: {x}" }`
3. AOT 调 `Calculator.Add(1, 2)` → 3（AOT 执行）
4. AOT 调 `IFormatter.Format(3)` → "Result: 3"（interpreter 执行）
5. 热更调 AOT 的 `Calculator.Add(10, 20)` → 30（interpreter → AOT）
6. 异常传播：interpreter 抛异常 → AOT catch → 正确捕获

---

## Phase 8: Productization Gates

### 8.1 架构设计

#### 目标

补齐产品化所需的兼容性、回滚、版本管理和发行 gate。

#### 兼容性测试矩阵

```
CompatibilityMatrix {
    aotVersions: ["1.0.0", "1.1.0", "1.2.0"]
    hotUpdateVersions: ["patch-001", "patch-002", "patch-003"]
    platforms: ["windows-x64", "android-arm64", "ios-arm64"]
    
    testCases:
        - aot=1.0.0 + hot=patch-001 → expected: OK
        - aot=1.0.0 + hot=patch-003 → expected: OK (forward compat)
        - aot=1.2.0 + hot=patch-001 → expected: OK (backward compat)
        - aot=1.0.0 + hot=incompatible → expected: REJECT with error
}
```

#### 回滚策略

```
HotUpdateManager
    ├── ApplyPatch(package) → 加载 + 验证 + 激活
    ├── Rollback() → 卸载当前 patch → 回到 AOT-only
    ├── GetActivePatches() → 当前已加载的 patch 列表
    └── ValidateIntegrity() → 校验所有 metadata/bridge 一致性
```

#### iOS 发行策略文档

需要明确回答：
1. 热更内容是否下发 IL/IR？（技术上可以，发行上需评估）
2. 热更是否改变"应用功能"？（App Store 2.5.2 的界定）
3. 推荐策略：配置/数据热更（安全）vs 逻辑热更（需谨慎评估）

### 8.2 执行条目

```
- [ ] 8.2.1  建立兼容性测试矩阵框架
- [ ] 8.2.2  实现 upgrade/downgrade 自动化测试
- [ ] 8.2.3  实现 HotUpdateManager.Rollback() + 验证回滚正确性
- [ ] 8.2.4  实现 package integrity validation
- [ ] 8.2.5  建立长时间运行 soak test（连续运行 24h 无泄漏/崩溃）
- [ ] 8.2.6  编写 iOS distribution policy 文档
- [ ] 8.2.7  建立 release checklist（每个发布版本必须通过的 gate）
- [ ] 8.2.8  建立 perf regression dashboard（所有平台 + 所有 subject）
- [ ] 8.2.9  建立 unsupported feature report（自动检测不支持的 CLR 特性并报告）
- [ ] 8.2.10 新增 subject `CompatibilityMatrixProof`
```

### 8.3 验证产物

| 产物 | 验证方式 |
| --- | --- |
| 兼容性矩阵 | 3×3×3 矩阵全部通过 |
| 回滚 | apply → verify → rollback → verify AOT-only → apply again → verify |
| Soak test | 24h 连续运行，内存增长 < 1MB，无 crash |
| Perf dashboard | 所有 subject × 所有平台 baseline 无回归 |
| Release checklist | 文档 + 自动化 gate script |

---

## 跨 Phase 依赖总览

```
Phase 0 ──┬──→ Phase 1 ──→ Phase 2 ──┬──→ Phase 4
           │                           │
           ├──→ Phase 3 ──────────────┤
           │                           │
           └──→ Phase 5 ──→ Phase 6 ──→ Phase 7 ──→ Phase 8
                                                        ↑
                                              Phase 4 ──┘
```

Phase 4 (mobile) 和 Phase 5 (hot update skeleton) 可并行。
Phase 8 (productization) 等所有其他 Phase 完成后收尾。
