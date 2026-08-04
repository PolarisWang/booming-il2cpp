# 统一工程生成与 CLI 重构设计

Date: 2026-04-10
Status: pending-review

## 1. 目标

把当前散落在多个命令和模块中的工程生成能力，收口为一个清晰的 CLI 工具和一套完整的 solution 生成流程。

## 2. 架构概览

```
                    ┌──────────────────────────┐
                    │     run.ps1 / run.sh     │  统一入口（开发期交互）
                    │   TUI + 进度 + 错误提示   │
                    └──────────┬───────────────┘
                               │ 调用
                    ┌──────────▼───────────────┐
                    │   chaos-il2cpp CLI        │  核心 CLI（可独立发布）
                    │   convert | build | publish │
                    └──────────┬───────────────┘
                               │
              ┌────────────────┼────────────────┐
              ▼                ▼                 ▼
     ┌────────────┐   ┌──────────────┐   ┌──────────────┐
     │  Pipeline   │   │  CMake Build  │   │   Deploy     │
     │  (Loader→   │   │  (configure   │   │   (copy to   │
     │   CodeGen)  │   │   + build)    │   │    deploy/)  │
     └─────────────┘   └──────────────┘   └──────────────┘
```

## 3. chaos-il2cpp CLI 设计

### 3.1 子命令

| 子命令 | 语法 | 功能 |
| --- | --- | --- |
| `convert` | `chaos-il2cpp convert <subject-dir> --output <dir>` | C# / DLLs → native 源码 |
| `build` | `chaos-il2cpp build <dir> --target <target-id>` | native 源码 → 编译产物 |
| `publish` | `chaos-il2cpp publish <subject-dir> --target <target-id> --output <dir>` | convert + build 一步到位 |

`<subject-dir>` 是包含 `subject.manifest.json` 的目录。CLI 通过读取 manifest 来决定输入类型和路径。

### 3.2 输入模型（Manifest 驱动）

subject.manifest.json 新增 `input` 字段描述输入形态：

```json
{
  "subjectId": "HelloWorldObject",
  "input": {
    "kind": "csproj",
    "project": "source/HelloWorldObject.csproj",
    "dependencies": []
  }
}
```

三种 `input.kind`：

| kind | 含义 | convert 行为 |
| --- | --- | --- |
| `csproj` | 纯 C# 工程 | `dotnet build` 得到 DLL → Pipeline |
| `dlls` | 预编译 DLL 集合 | 跳过编译，直接用 `input.assemblies[]` 指定的 DLL → Pipeline |
| `csproj+dlls` | C# 工程 + 额外依赖 DLL | `dotnet build` 得到主 DLL + 合并 `input.dependencies[]` 中的 DLL → Pipeline |

```json
// 纯 DLL 输入
{
  "subjectId": "ThirdPartyLib",
  "input": {
    "kind": "dlls",
    "assemblies": [
      "source/ThirdParty.dll",
      "source/ThirdParty.Core.dll"
    ],
    "entryAssembly": "source/ThirdParty.dll"
  }
}

// C# 工程 + 依赖 DLL
{
  "subjectId": "MixedApp",
  "input": {
    "kind": "csproj+dlls",
    "project": "source/MixedApp.csproj",
    "dependencies": [
      "source/libs/External.dll",
      "source/libs/External.Core.dll"
    ]
  }
}
```

### 3.3 convert 流程

```
1. 读取 <subject-dir>/subject.manifest.json
2. 根据 input.kind 准备输入程序集：
   - csproj:       dotnet build <project> -o <temp>/host-input/
   - dlls:         直接使用 input.assemblies 路径
   - csproj+dlls:  dotnet build + 复制 input.dependencies 到同目录
3. Pipeline.Execute(inputAssemblies, outputRoot)
   - Loader: 读取 DLL metadata
   - SemanticWorld: 构建语义模型
   - Linker: 链接分析
   - MetadataWriter: 写入 metadata registration
   - CodeGen: 生成 native reference + lowering plan
4. 输出到 <output>/
   ├── generated/
   │   ├── native-reference.generated.cpp
   │   ├── native-reference.generated.h
   │   └── ...
   ├── metadata/
   │   ├── code-registration.json
   │   ├── metadata-registration.json
   │   └── ...
   ├── CMakeLists.txt          ← 新增：可直接 cmake build 的工程文件
   └── convert.manifest.json   ← 转换结果清单
```

### 3.3 build 流程

```
1. 读取 <dir>/convert.manifest.json
2. cmake -S <dir> -B <dir>/build/<target-id> -DCMAKE_BUILD_TYPE=Release
3. cmake --build <dir>/build/<target-id>
4. 复制产物到 deploy/<subject-id>/bin/<target-id>/
```

### 3.4 实现方式

改造 `src/managed/Chaos.IL2CPP.Driver/DriverEntry.cs`：

```csharp
// 当前：位置参数
//   chaos-il2cpp <input.dll> <output-root>
//   chaos-il2cpp emit-native-reference <closure-root> <output-root>

// 改为：子命令 dispatch
static int Main(string[] args)
{
    if (args.Length == 0) return ShowHelp();
    return args[0] switch
    {
        "convert" => RunConvert(args[1..]),
        "build" => RunBuild(args[1..]),
        "publish" => RunPublish(args[1..]),
        // 保持向后兼容
        "emit-native-reference" => RunLegacyEmitNativeReference(args[1..]),
        _ when !args[0].StartsWith("-") => RunLegacyConvert(args),
        _ => ShowHelp(),
    };
}
```

### 3.5 目标平台

复用现有 `CORE_TARGET_SPECS`：

| target-id | 平台 | 工具链 |
| --- | --- | --- |
| `windows-x64-reference` | Windows x64 | MSVC / Visual Studio 17 2022 |
| `macos-reference` | macOS arm64 | Xcode / Apple Clang |
| `android-arm64` | Android arm64 | NDK r26+ |
| `ios-arm64` | iOS arm64 | Xcode |
| `linux-x64` | Linux x64 | GCC / Clang |

## 4. Subject Solution 生成设计

### 4.1 目标结构

每个 subject 生成到 `solutions/subjects/<SubjectId>/`：

```
solutions/subjects/HelloWorldObject/
  HelloWorldObject.sln
  │
  ├─ managed/                               ← C# 工程组（仅 kind=csproj 或 csproj+dlls 时）
  │   ├─ HelloWorldObject.csproj            ← 原始 C# 工程（符号链接/引用）
  │   ├─ HelloWorldObject.Tests.csproj      ← 单元测试工程（符号链接/引用）
  │   └─ deps/                              ← 额外依赖 DLL（kind=csproj+dlls 时）
  │
  ├─ assemblies/                            ← 预编译 DLL（仅 kind=dlls 时）
  │   ├─ ThirdParty.dll
  │   └─ ThirdParty.Core.dll
  │
  ├─ native/                                ← native 工程组
  │   ├─ CMakeLists.txt                     ← convert 生成的 CMake 工程
  │   ├─ generated/                         ← 生成的 .cpp / .h
  │   └─ runtime/                           ← runtime 支撑代码
  │
  ├─ native-tests/                          ← native 测试工程组
  │   ├─ CMakeLists.txt                     ← 测试 CMake 工程
  │   └─ proof/                             ← proof 测试代码（链接到 validation/proof/）
  │
  └─ workspace.manifest.json                ← 工作区清单
```

当 `input.kind` 为 `dlls` 时，.sln 中没有 C# 项目，只有 native + native-tests。

### 4.2 .sln 包含的项目

| 项目 | 类型 | GUID 类型 | 来源 | 条件 |
| --- | --- | --- | --- | --- |
| `<Subject>.csproj` | C# 可执行 | CSHARP | `subjects/<id>/source/` | kind=csproj 或 csproj+dlls |
| `<Subject>.Tests.csproj` | C# 测试 | CSHARP | `subjects/<id>/validation/unit/` | 存在时 |
| native 工程 | CMake vcxproj | VCX | convert 生成 | 始终 |
| native-tests 工程 | CMake vcxproj | VCX | proof 测试 | 存在时 |

### 4.3 改造点

在 `project_workspace.py` 的 `generate_subject_workspace()` 中：

- 确保 `validation/unit/` 下的测试工程被发现并加入 .sln
- 确保 `validation/proof/` 下的 native 测试被包装为 CMake 工程并加入 .sln
- 生成 `workspace.manifest.json` 记录所有 4 类工程的路径

## 5. Core 工程 Solution 设计

基本保持现有 `generate_core_workspace()` 的结构：

```
solutions/core/<host>/
  chaos-il2cpp-core.sln
  ├─ 8 个 managed projects（Contracts, Loader, SemanticWorld, ...）
  ├─ Driver 项目（升级为子命令 CLI）
  └─ native targets（windows-x64-reference, android-arm64, ...）
```

改造点：
- Driver 项目升级后，core solution 自动包含新的 CLI 入口
- 确保 `dotnet publish` Driver 后可以得到独立的 `chaos-il2cpp` 可执行文件

## 6. 统一入口优化设计

### 6.1 简化 TUI 菜单

在 Quick Start 组新增快捷入口：

```
Quick Start
  ├─ Generate all projects    ← 一键生成所有 solution
  ├─ Build subject            ← 选择 subject → convert + build
  └─ ...existing...
```

### 6.2 进度反馈

每个大步骤显示 spinner + 阶段名：

```
[1/4] Building managed assembly...        ✓
[2/4] Running IL2CPP pipeline...           ✓
[3/4] Configuring native project...        ✓
[4/4] Building native target...            ◌ (building...)
```

### 6.3 改善错误提示

| 失败场景 | 当前 | 改进 |
| --- | --- | --- |
| dotnet 未安装 | Python traceback | "dotnet SDK not found. Install from https://dot.net/download" |
| cmake 未安装 | 子进程报错 | "CMake not found. Run: run doctor" |
| C# 编译失败 | 原始 MSBuild 输出 | 摘要错误行 + "See full log at: <path>" |
| cmake 配置失败 | cmake stderr | "CMake configuration failed for target <id>. Check preset: <name>" |

## 7. deploy/ 输出结构

```
deploy/
  <SubjectId>/
    bin/
      <target-id>/
        <executable>              ← 编译产物
        runtime libs...
    manifest.json                 ← 部署清单
```

## 8. 向后兼容

- Driver 的旧调用方式（位置参数）保持兼容
- `run generate project` 系列命令保持可用
- 现有测试不受影响

## 9. 不做的事

- 不在 CLI 中集成运行时库打包
- 不改变 `src/native/` 的目录结构
- 不改变 subject source 的内部结构
- 不做跨平台交叉编译（每个 target 需要对应的宿主工具链）
