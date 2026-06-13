# Roadmap: 传统 IL2CPP 全量翻译 — chaos-il2cpp 模式

## 目标

```bash
chaos-il2cpp convert-to-cpp \
  --assembly System.Private.CoreLib.dll \
  --output il2cpp_dist/ \
  --additional-assembly-dir references/
```

直接输出 `System.Private.CoreLib.generated.cpp`，完整 IL→C++ 翻译。

用法与 Unity IL2CPP 一致。`chaos-il2cpp.exe` 是已有的 Driver 项目，新增 `convert-to-cpp` 命令。

---

## 具体用法

```bash
# 编译单个 assembly
chaos-il2cpp convert-to-cpp \
  --assembly System.Private.CoreLib.dll \
  --output build/native/
  --additional-assembly-dir packages/
  --additional-assembly-dir lib/

# 编译多个 assembly（创建 multi-assembly 解决方案）
chaos-il2cpp convert-to-cpp \
  --assembly App.dll \
  --assembly System.Linq.dll \
  --output build/native/

# 输出内容
build/native/
  System.Private.CoreLib.generated.cpp    ← 所有 methods 的 lowering
  System.Private.CoreLib.types.h          ← TypeInfo 等类型元数据
  System.Linq.generated.cpp
  System.Linq.types.h
  ...
```

### 入口代码生成

```bash
# 生成入口点（Foundation DLL 验证用的小桩）
chaos-il2cpp convert-to-cpp \
  --assembly ConvertCharNativeEntry.dll \
  --entry-point Class::Run \
  --output il2cpp_dist/genuine/
```

但这与 Foundation DLL 场景无关——上面已产出全量翻译的 C++ 文件。Native exe 用 `generated.cpp` 直接链接。

---

## 数据流（实际实现）

```
input.dll (EnumParsingSubjects.dll)
  │
  ▼
PipelinePlan.Execute() — 复用现有 5 阶段流水线
  ├── LoaderStage.LoadMultiple()     — 读取 PE + 依赖 assembly
  ├── SemanticWorldStage.Build()     — 语义模型
  ├── LinkerStage.Link()             — reachability（FullAssemblyClosure 时不过滤）
  ├── MetadataWriterStage.Write()    — metadata graph
  └── CodeGenStage.Generate()        — AotCoreIr + NativeAotLoweringPlan
  │
  ▼
ManagedClosureResult (内存中工件集合)
  │
  ▼
FullAssemblyEmitter.Emit()           — 新实现，通过 NativeAotEmitter
  │                                    .GenerateFromArtifacts() 直接发射
  │                                    跳过 JSON 序列化/反序列化往返
  ▼
NativeAotLoweringPlanner.Create()
  │ fullAssemblyMode=true
  │ CollectAllMethods() — 收集所有方法
  ▼
C++ 文件输出:
  ├── generated/native-aot.generated.cpp    ← 主翻译单元
  ├── generated/native-aot.generated.header.h  ← 类型声明（多分页时）
  ├── generated/shape_dispatch.h             ← dispatch 表
  ├── generated/enum_metadata.generated.h    ← enum metadata
  ├── generated/chaos_generated_module.h     ← 类型化 dispatch table header
  └── generated/chaos_generated_module.cpp   ← dispatch wiring source
```

### 架构说明

与初始 roadmap 的关键差异：
1. **FullAssemblyEmitter** 不再自行遍历方法，改为委托给 NativeAotEmitter
2. **FullAssemblyLinker** 未单独实现 — 通过 CodeGenStage 的 FullAssemblyClosure 模式处理
3. **InternalCallHandler** 未实现 — InternalCall 通过 P/Invoke 路径处理
4. **JSON 往返消除** — FullAssemblyEmitter 通过 GenerateFromArtifacts() 直接使用内存工件

### 跨程序集调用

```
// 当前: 全 extern stub → return 0

// 目标: 直接 extern "C" 声明
// B.dll 调用 A.dll::Foo()
// B.generated.cpp 中包含:
extern "C" CHAOS_IL2CPP_INT32 A_dll_Foo(CHAOS_IL2CPP_INT32 x);
// 调用时直接使用
int result = A_dll_Foo(42);

// A.generated.cpp 中定义了:
extern "C" CHAOS_IL2CPP_INT32 A_dll_Foo(CHAOS_IL2CPP_INT32 x) {
    // IL lowering 代码
    return x * 2;
}
```

跨程序集调用不需要 dispatch table——直接通过 `extern "C"` 符号链接。Linker 在链接阶段解析所有跨模块符号。

---

## 分层策略

```
Layer 1: il2cpp.exe 直接编译（本次 roadmap 全部）
  ├── full assembly → 单个 .generated.cpp
  ├── self-contained: 所有 IL 被翻译，无 extern stub
  └── InternalCall / PInvoke → 保留 runtime bridge

Layer 2: 多程序集联合编译（后续）
  ├── il2cpp.exe --link A.dll B.dll
  ├── 跨模块 extern "C" 解析在编译时完成
  └── 支持 200+ DLL 联合产出

Layer 3: Hot Path Inlining（后续优化）
  ├── PGO 数据驱动
  └── 高频路径跨模块内联
```

---

## Phase 1：`chaos-il2cpp.exe convert-to-cpp` 实现

不需新建项目。`chaos-il2cpp.exe` 就是已有的 `Chaos.IL2CPP.Driver`。新增一个命令模式。

### DriverEntry.cs — 新增命令

```csharp
// DriverEntry.cs 的 Main() 中增加:
"convert-to-cpp" => RunConvertToCpp(args[1..]),

// 新增方法:
private static int RunConvertToCpp(string[] args)
{
    var config = ParseConvertToCppArgs(args);
    // config.Assembly → 目标程序集
    // config.OutputDir → 输出目录
    // config.AdditionalAssemblyDirs → 依赖搜索路径
    
    // 1. 加载 assembly + 所有依赖
    var loader = new LoaderStage();
    var world = loader.LoadAssembly(
        config.Assembly, config.AdditionalAssemblyDirs);
    
    // 2. 构建全量语义模型（无 entrypoint 约束）
    var semantic = new SemanticWorldStage();
    var fullWorld = semantic.BuildFull(world);
    
    // 3. 标记 InternalCall / PInvoke
    var linker = new FullAssemblyLinker();
    var linked = linker.Link(fullWorld);
    
    // 4. 生成 C++
    var emitter = new FullAssemblyEmitter();
    emitter.Emit(linked, config.OutputDir);
    
    return 0;
}
```

### 新增文件

```
src/managed/Chaos.IL2CPP.Driver/
  ConvertToCpp/                              ← 新建目录
    ConvertToCppConfig.cs                    ← 参数模型
    ConvertToCppHandler.cs                   ← 入口处理
    FullAssemblyEmitter.cs                   ← 遍历 method → lowering
    FullAssemblyLinker.cs                    ← 标记 InternalCall/PInvoke
    AssemblyCppWriter.cs                     ← 组织 C++ 输出
    CrossModuleResolver.cs                   ← extern "C" 符号
    
  DriverEntry.cs                             ← 修改: 增加 convert-to-cpp 命令
```

### 复用的现有组件

| 组件 | 来源 | 说明 |
|------|------|------|
| `LoaderStage` | `Chaos.IL2CPP.Loader` | 读取 PE → IL body，直接复用 |
| `SemanticWorldStage` | `Chaos.IL2CPP.Pipeline` | 需增加 `BuildFull()` 无 entrypoint 模式 |
| `ManagedMethodModel.Body` | `Chaos.IL2CPP.Contracts` | 已有 decoded IL instructions |
| `NativeAotLoweringPlanner.*` | `Chaos.IL2CPP.CodeGen` | 复用 lowering 逻辑，修改 planner |
| `AotCoreIrMethodArtifact` | `Chaos.IL2CPP.Contracts` | 现有 IR 模型 |

---

## Phase 2：Foundation DLL 验证接入

### batch_native_aot_runner.py 改造

```python
# 当前: entrypoint → convert → trim → emit 四步
# 目标: chaos-il2cpp convert-to-cpp 直接翻译
def run_family(family_slug):
    dll_path = build_entrypoint(family_slug)
    
    subprocess.run([
        "dotnet", "run", "--project", "src/managed/Chaos.IL2CPP.Driver",
        "--", "convert-to-cpp",
        "--assembly", dll_path,
        "--output", f"il2cpp_dist/genuine/",
    ])
    
    # 产出: il2cpp_dist/genuine/<entry>.generated.cpp
    #       + il2cpp_dist/genuine/System.Private.CoreLib.generated.cpp
    #       + native-reference.runtime-skeleton.coverage.json
```

---

## Phase 3：多程序集联合编译（200+ DLL）

```bash
dotnet run --project src/managed/Chaos.IL2CPP.Driver -- convert-to-cpp \
  --assembly System.Private.CoreLib \
  --assembly System.Linq \
  --output il2cpp_dist/full/
```

---

## 文件改动清单

### Phase 1（核心 — 实际实现）

| 文件 | 类型 | 改动 | 状态 |
|------|------|------|------|
| `DriverEntry.cs` | 修改 | Main() 增加 `convert-to-cpp` 命令 | ✅ 已有（DriverEntry.cs:109） |
| `ConvertToCpp/ConvertToCppConfig.cs` | **新建** | --assembly / --output 参数模型 | ✅ 已有 |
| `ConvertToCpp/ConvertToCppHandler.cs` | 修改 | 重写为 FullAssemblyEmitter 直接发射路径 + 修复 `--full-closure` 标志 | ✅ 本轮完成 |
| `ConvertToCpp/FullAssemblyEmitter.cs` | **新建** → 重写 | 原占位符 → 通过 NativeAotEmitter.GenerateFromArtifacts() 直接发射 | ✅ 本轮重写 |
| `NativeAotEmitter.cs` | 修改 | 新增 `GenerateFromArtifacts()` 公共方法（跳过 JSON 往返） | ✅ 本轮完成 |
| `AssemblyCppWriter.cs` | — | 未实现（由 NativeAotEmitter 内置的 BuildGeneratedSources 替代） | ❌ 不需要 |
| `CrossModuleResolver.cs` | — | 未实现（Phase 3 多程序集联合时按需） | ❌ 推迟 |
| `InternalCallHandler.cs` | — | 未实现（InternalCall 通过现有 P/Invoke 路径处理） | ❌ 推迟 |
| `SemanticWorldStage.cs` | — | 未修改（无需单独 BuildFull()，FullAssemblyClosure 模式已处理） | ❌ 不需要 |
| `NativeAotLoweringPlanner.*.cs` | — | 未修改（已有 fullAssemblyMode 支持） | ❌ 不需要

### Phase 2（验证集成）

| 文件 | 类型 | 改动 |
|------|------|------|
| `batch_native_aot_runner.py` | 修改 | 调用 `convert-to-cpp` 替代三步流程 |
| `native_compile_runner.py` | 修改 | 编译多文件 C++ |
| `fact_verifier.py` | 替换 | 新管线直接运行 entry.exe，旧 fact_l2_verifier.py 已删除 |
| `native_verify_main.cpp` | 修改 | include 新路径 |

---

## 依赖关系

```
Phase 1: il2cpp.exe 核心（2 个文件修改 + 1 个文件重写）
    │
    ▼
Phase 2: 验证接入（4 个文件修改）
    │
    ▼
Phase 3: 多程序集联合（按需，200+ DLL）
```

---

## 风险评估

| 风险 | 概率 | 影响 | 缓解 |
|------|------|------|------|
| InternalCall 列表不全 | 中 | 运行时 crash | Phase 1 只标记已知的，运行时 interpreter 兜底 |
| lowering 不完备 | 中 | 生成代码编译失败 | Phase 1 验证 convert-char 等简单 family |
| C++ 文件过大 | 中 | 编译 OOM | Phase 1 验证单 assembly，后续按 namespace 分文件 |
| 跨模块符号冲突 | 低 | 链接失败 | `extern "C"` 用名称修饰避免冲突 |

---

## 优先级评估

| 优先级 | 方案满足情况 |
|--------|------------|
| **P1 性能最优** | Full assembly lowering 无分发开销；后续 L1 内联补性能 |
| **P2 架构完美** | `il2cpp.exe` 1:1 对应传统 IL2CPP 工具链，架构清晰 |
| **P3 热更新适配** | 生成的 C++ 通过 dispatch table 注册（复用），patch 取代原入口 |
