# Native 输出工程目录结构设计

Date: 2026-04-11
Status: draft-v1
Parent: `phase-architecture-v1-01.md`

---

## 1. 设计目标

| 目标 | 说明 |
| --- | --- |
| 分层清晰 | 生成代码、元数据、运行时胶水、构建配置各自独立 |
| 按类型拆分 | 每个 C# 类型生成一个 .cpp，最大化编译并行度 |
| 增量友好 | 修改一个类型只需重编译对应 .cpp + 重链接 |
| 性能优化预留 | 为 LTO、PGO、code size 优化预留目录和构建开关 |
| 不含 subject 逻辑 | 输出结构是通用的，不绑定任何 subject 名称 |
| 可直接 cmake build | 输出目录自包含，可独立构建 |

---

## 2. 输出目录总览

`chaos-il2cpp convert <subject-dir> --output <output-dir>` 的输出：

```
<output-dir>/
│
│  ══════ 构建入口 ══════
│
├── CMakeLists.txt                       ← 顶层 CMake，可直接 cmake -S . -B build
├── il2cpp-config.h                      ← 全局配置宏（target platform, feature flags, optimization level）
├── convert.manifest.json                ← convert 产物清单
│
│  ══════ 生成代码（核心产物）══════
│
├── generated/
│   │
│   ├── assemblies/                      ← 按程序集分组，每个程序集一个子目录
│   │   │
│   │   ├── <AssemblyName>/              ← 例如 MyApp/
│   │   │   ├── types/                   ← 按类型拆分，每个类型一个 .cpp
│   │   │   │   ├── <Type1>.cpp          ← 包含该类型所有方法的 native 实现
│   │   │   │   ├── <Type2>.cpp
│   │   │   │   └── ...
│   │   │   ├── <AssemblyName>.assembly.cpp   ← assembly 级初始化（class init table、static fields）
│   │   │   ├── <AssemblyName>.assembly.h     ← assembly 导出符号声明
│   │   │   └── <AssemblyName>.sources.cmake  ← 该 assembly 的源文件列表（被顶层 CMakeLists 包含）
│   │   │
│   │   └── <AnotherAssembly>/           ← 多程序集时，每个 assembly 独立目录
│   │       ├── types/
│   │       ├── *.assembly.cpp
│   │       ├── *.assembly.h
│   │       └── *.sources.cmake
│   │
│   ├── forward-declarations.h           ← 所有类型的前向声明（避免循环 include）
│   ├── method-pointers.h                ← 所有生成方法的函数指针 typedef
│   └── generated-index.h               ← 汇总 include，方便外部引用
│
│  ══════ 元数据注册表 ══════
│
├── metadata/
│   ├── type-registration.cpp            ← 类型元数据表（token → type info）
│   ├── method-registration.cpp          ← 方法注册表（token → function pointer）
│   ├── field-registration.cpp           ← 字段元数据表
│   ├── string-literals.cpp              ← 字符串字面量池（UTF-8 预编码）
│   ├── generic-instantiations.cpp       ← 泛型实例化注册
│   ├── metadata-tables.h               ← 元数据表声明
│   └── metadata.sources.cmake           ← 元数据源文件列表
│
│  ══════ 运行时胶水 ══════
│
├── runtime-glue/
│   ├── entry-point.cpp                  ← 程序入口：bootstrap → init → run entry method
│   ├── bridge-init.cpp                  ← CodegenBridge 初始化 + metadata resolution
│   ├── icall-table.cpp                  ← internal call 路由表（Console.WriteLine 等）
│   ├── delegate-wrappers.cpp            ← delegate 桥接 wrapper 函数
│   ├── exception-support.cpp            ← 异常处理支撑（catch dispatch、finally runner）
│   ├── runtime-glue.h                   ← 胶水层公共声明
│   └── runtime-glue.sources.cmake       ← 胶水层源文件列表
│
│  ══════ 分析产物（JSON，不参与编译）══════
│
├── analysis/
│   ├── typed-il-ir.json                 ← TypedIL IR 中间表示
│   ├── aot-manifest.json                ← AOT 编译清单
│   ├── code-registration.json           ← 方法注册 JSON
│   ├── metadata-registration.json       ← 元数据注册 JSON
│   ├── optimization-facts.json          ← 优化决策记录
│   ├── native-reference.lowering-plan.json  ← lowering 计划
│   └── closure.manifest.json            ← 闭包清单
│
│  ══════ 性能优化预留 ══════
│
├── optimization/
│   ├── pgo-profiles/                    ← PGO profile 数据存放目录
│   │   └── .gitkeep
│   ├── lto-config.cmake                 ← LTO 配置（默认关闭，可开启）
│   ├── code-size-report.cmake           ← code size 统计目标
│   └── hot-cold-split.cmake             ← 冷热分离配置（预留）
│
│  ══════ 热更预留（Phase 5+ 才产出）══════
│
└── hot-update/
    ├── supplemental-metadata-template.json  ← 供热更包生成时使用的元数据模板
    └── bridge-stubs/                        ← AOT→HotUpdate 桥接 stub（Phase 6 生成）
        └── .gitkeep
```

---

## 3. 文件命名规则

### 3.1 类型文件命名

C# 类型 → C++ 文件名的映射规则：

```
{Assembly}_{Namespace}_{TypeName}.cpp
```

| C# 类型 | 文件名 |
| --- | --- |
| `MyApp.Program` | `MyApp_Program.cpp` |
| `MyApp.Models.Greeter` | `MyApp_Models_Greeter.cpp` |
| `MyApp.Services.IService` | `MyApp_Services_IService.cpp` |
| `System.Collections.Generic.List<int>` | `System_Collections_Generic_List_T_int.cpp`（泛型实例化） |
| `MyApp.Nested+Inner` | `MyApp_Nested_Inner.cpp`（嵌套类型用 `_` 分隔） |

### 3.2 生成函数命名

```
{Assembly}_{Type}_{Method}  （C linkage，extern "C"）
```

例如：`MyApp_Greeter__ctor`、`MyApp_Greeter_BuildMessage`、`MyApp_Program_Main`

### 3.3 Namespace

所有生成代码统一在 `namespace chaos::il2cpp::generated` 下：

```cpp
namespace chaos::il2cpp::generated {
namespace MyApp {      // per-assembly namespace
    // type implementations
}
}
```

---

## 4. 每个文件的职责

### 4.1 类型文件（`types/<Type>.cpp`）

```cpp
// MyApp_Greeter.cpp
#include "il2cpp-config.h"
#include "generated-index.h"
#include "MyApp.assembly.h"

namespace chaos::il2cpp::generated::MyApp {

// ---- 方法实现 ----

extern "C" void CHAOS_RUNTIME_ABI_CALL
MyApp_Greeter__ctor(RuntimeState* runtime, ThreadState* thread, void* this_ptr, void* name_arg) {
    // field set via ABI
}

extern "C" void* CHAOS_RUNTIME_ABI_CALL
MyApp_Greeter_BuildMessage(RuntimeState* runtime, ThreadState* thread, void* this_ptr) {
    // field get + string concat via bridge
}

} // namespace
```

**关键约束**：
- 每个 .cpp 只包含**一个类型**的方法实现
- 通过 `#include "generated-index.h"` 获得其他类型的前向声明
- 通过 `#include "<Assembly>.assembly.h"` 获得 assembly 级符号
- 不直接 include runtime 内部头文件，只通过 `il2cpp-config.h` + ABI headers

### 4.2 Assembly 文件（`<Assembly>.assembly.cpp`）

```cpp
// MyApp.assembly.cpp
#include "il2cpp-config.h"
#include "MyApp.assembly.h"

namespace chaos::il2cpp::generated::MyApp {

// 静态字段存储
static void* s_static_fields[MYAPP_STATIC_FIELD_COUNT] = {};

// class 初始化状态
static bool s_class_init_flags[MYAPP_TYPE_COUNT] = {};

// Assembly 初始化
void InitializeAssembly(const CodegenBridgeV0* bridge) {
    // 解析 assembly 级 metadata handles
}

} // namespace
```

### 4.3 元数据注册（`metadata/*.cpp`）

```cpp
// type-registration.cpp
#include "metadata-tables.h"

namespace chaos::il2cpp::generated::metadata {

// 类型表：token → TypeInfo 结构
const TypeRegistrationEntry g_type_entries[] = {
    { 0x02000001, "MyApp/Program", sizeof_MyApp_Program, ... },
    { 0x02000002, "MyApp/Greeter", sizeof_MyApp_Greeter, ... },
};

const size_t g_type_entry_count = sizeof(g_type_entries) / sizeof(g_type_entries[0]);

} // namespace
```

### 4.4 运行时胶水（`runtime-glue/*.cpp`）

```cpp
// entry-point.cpp
#include "il2cpp-config.h"
#include "runtime-glue.h"

int32_t chaos_il2cpp_main(int argc, const char** argv) {
    // 1. 获取 ABI + bridge
    const auto* abi = chaos_runtime_get_abi_v0();
    const auto* bridge = chaos_codegen_get_bridge_v0();
    
    // 2. 初始化运行时
    RuntimeState* runtime = nullptr;
    abi->runtime_init(&init_params, nullptr, &runtime);
    
    // 3. 附加主线程
    ThreadState* thread = nullptr;
    abi->thread_attach(runtime, &thread);
    
    // 4. 注册 codegen
    bridge->register_codegen(&g_code_registration, &g_metadata_registration, &g_options);
    bridge->bootstrap_runtime();
    
    // 5. 初始化所有 assembly
    generated::MyApp::InitializeAssembly(bridge);
    
    // 6. 调用入口方法
    auto result = MyApp_Program_Main(runtime, thread, managed_args);
    
    // 7. 清理
    abi->thread_detach(runtime, thread);
    abi->runtime_shutdown(runtime);
    return result;
}
```

---

## 5. CMakeLists.txt 结构

```cmake
cmake_minimum_required(VERSION 3.20)
project(il2cpp_generated LANGUAGES CXX)

# ---- 全局配置 ----
set(CMAKE_CXX_STANDARD 17)
set(IL2CPP_CONTRACTS_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../contracts/native/v0")
set(IL2CPP_RUNTIME_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../src/native")

# ---- 编译选项 ----
option(IL2CPP_ENABLE_LTO "Enable Link-Time Optimization" OFF)
option(IL2CPP_ENABLE_PGO_GENERATE "Generate PGO profile data" OFF)
option(IL2CPP_ENABLE_PGO_USE "Use PGO profile data" OFF)
option(IL2CPP_CODE_SIZE_REPORT "Generate code size report" OFF)

# ---- Include paths ----
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/generated
    ${CMAKE_CURRENT_SOURCE_DIR}/metadata
    ${CMAKE_CURRENT_SOURCE_DIR}/runtime-glue
    ${IL2CPP_CONTRACTS_DIR}
    ${IL2CPP_RUNTIME_DIR}/runtime-core
    ${IL2CPP_RUNTIME_DIR}/bootstrap
    ${IL2CPP_RUNTIME_DIR}/support
)

# ---- 源文件收集（通过 .sources.cmake 文件）----
include(generated/assemblies/MyApp/MyApp.sources.cmake)
# include(generated/assemblies/MyLib/MyLib.sources.cmake)  # 多 assembly 时
include(metadata/metadata.sources.cmake)
include(runtime-glue/runtime-glue.sources.cmake)

# ---- 主目标 ----
add_executable(il2cpp_output
    ${MYAPP_SOURCES}
    # ${MYLIB_SOURCES}
    ${METADATA_SOURCES}
    ${RUNTIME_GLUE_SOURCES}
)

# ---- 链接运行时库 ----
add_subdirectory(${IL2CPP_RUNTIME_DIR}/runtime-core runtime-core)
add_subdirectory(${IL2CPP_RUNTIME_DIR}/bootstrap bootstrap)
add_subdirectory(${IL2CPP_RUNTIME_DIR}/support support)

target_link_libraries(il2cpp_output PRIVATE
    chaos_runtime_core
    chaos_bootstrap
    chaos_support
)

# ---- 性能优化 ----
include(optimization/lto-config.cmake)

if(IL2CPP_ENABLE_PGO_GENERATE)
    # MSVC: /GENPROFILE   GCC/Clang: -fprofile-generate
    target_compile_options(il2cpp_output PRIVATE $<$<CXX_COMPILER_ID:MSVC>:/GL>)
    target_link_options(il2cpp_output PRIVATE $<$<CXX_COMPILER_ID:MSVC>:/LTCG /GENPROFILE>)
endif()

if(IL2CPP_ENABLE_PGO_USE AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/optimization/pgo-profiles")
    # MSVC: /USEPROFILE   GCC/Clang: -fprofile-use
    target_link_options(il2cpp_output PRIVATE $<$<CXX_COMPILER_ID:MSVC>:/LTCG /USEPROFILE>)
endif()

if(IL2CPP_CODE_SIZE_REPORT)
    include(optimization/code-size-report.cmake)
endif()
```

---

## 6. il2cpp-config.h 设计

```cpp
#pragma once

// ---- 平台检测 ----
#if defined(_WIN32)
    #define IL2CPP_PLATFORM_WINDOWS 1
#elif defined(__APPLE__)
    #include <TargetConditionals.h>
    #if TARGET_OS_IOS
        #define IL2CPP_PLATFORM_IOS 1
    #else
        #define IL2CPP_PLATFORM_MACOS 1
    #endif
#elif defined(__ANDROID__)
    #define IL2CPP_PLATFORM_ANDROID 1
#elif defined(__linux__)
    #define IL2CPP_PLATFORM_LINUX 1
#endif

// ---- ABI 约定 ----
#include "runtime_abi.h"
#include "codegen_bridge.h"

// ---- 编译模式 ----
// IL2CPP_VARIANT: CHECK (debug) / PROFILE (optimized+symbols) / SHIP (release)
#ifndef IL2CPP_VARIANT
    #define IL2CPP_VARIANT CHECK
#endif

// ---- 特性开关 ----
#ifndef IL2CPP_ENABLE_EXCEPTION_HANDLING
    #define IL2CPP_ENABLE_EXCEPTION_HANDLING 1
#endif

#ifndef IL2CPP_ENABLE_THREAD_SUPPORT
    #define IL2CPP_ENABLE_THREAD_SUPPORT 1
#endif

#ifndef IL2CPP_ENABLE_GC
    #define IL2CPP_ENABLE_GC 1
#endif

// ---- 性能优化宏 ----
#if defined(IL2CPP_HOT_PATH)
    // 标记热路径函数，编译器可据此优化
    #if defined(_MSC_VER)
        #define IL2CPP_LIKELY(x) (x)
        #define IL2CPP_UNLIKELY(x) (x)
    #else
        #define IL2CPP_LIKELY(x) __builtin_expect(!!(x), 1)
        #define IL2CPP_UNLIKELY(x) __builtin_expect(!!(x), 0)
    #endif
#else
    #define IL2CPP_LIKELY(x) (x)
    #define IL2CPP_UNLIKELY(x) (x)
#endif

// ---- Debug 支持 ----
#if IL2CPP_VARIANT == CHECK
    #define IL2CPP_DEBUG 1
    #define IL2CPP_ASSERT(cond, msg) do { if (!(cond)) { il2cpp_debug_break(msg); } } while(0)
#else
    #define IL2CPP_DEBUG 0
    #define IL2CPP_ASSERT(cond, msg) ((void)0)
#endif
```

---

## 7. 性能优化预留

### 7.1 LTO（Link-Time Optimization）

```cmake
# optimization/lto-config.cmake
if(IL2CPP_ENABLE_LTO)
    set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)
    message(STATUS "IL2CPP: LTO enabled")
endif()
```

**为什么按类型拆分不影响 LTO**：LTO 在链接时重新合并所有 .o，编译器可以跨 translation unit 内联。按类型拆分只影响编译并行度，不影响最终优化质量。

### 7.2 PGO（Profile-Guided Optimization）

两阶段流程：
```bash
# 第 1 阶段：生成 profile
chaos-il2cpp build <dir> --target windows-x64 --pgo-generate
# 运行程序收集 profile 数据
./output.exe --collect-profile
# profile 数据保存到 optimization/pgo-profiles/

# 第 2 阶段：使用 profile 优化
chaos-il2cpp build <dir> --target windows-x64 --pgo-use
```

### 7.3 Code Size 优化

```cmake
# optimization/code-size-report.cmake
# 构建后生成每个 .o 文件的 size 报告
add_custom_target(code_size_report
    COMMAND ${CMAKE_SIZE_TOOL} --format=berkeley $<TARGET_FILE:il2cpp_output>
    DEPENDS il2cpp_output
)
```

### 7.4 Hot/Cold 分离（预留）

```cmake
# optimization/hot-cold-split.cmake
# 未来：根据 PGO 数据将热函数和冷函数分到不同 section
# GCC/Clang: -freorder-functions
# MSVC: /ORDER
```

### 7.5 增量编译优势

按类型拆分的增量编译优势：

| 修改 | 重编译范围 | 估算时间 |
| --- | --- | --- |
| 修改 1 个方法体 | 对应类型的 1 个 .cpp | < 1s |
| 新增 1 个类型 | 新 .cpp + assembly.cpp + registration.cpp | < 3s |
| 修改接口签名 | 涉及的 .cpp + forward-declarations.h | 几个 .cpp |
| 完全重新生成 | 所有文件 | 与当前一样 |

---

## 8. 与 codegen 的接线

### 8.1 CodeGen 需要的改造

当前 `NativeReferenceProofEmitter` 输出单个 .cpp。需要改造为：

```
CodeGen Pipeline:
    LinkedWorldModel
        ↓
    PerTypeCodeEmitter（新增）
        ├── 为每个 reachable type 生成 types/<Type>.cpp
        ├── 生成 <Assembly>.assembly.cpp/h
        └── 生成 <Assembly>.sources.cmake
        ↓
    MetadataTableEmitter（新增）
        ├── 生成 type-registration.cpp
        ├── 生成 method-registration.cpp
        ├── 生成 string-literals.cpp
        └── 生成 metadata.sources.cmake
        ↓
    RuntimeGlueEmitter（新增）
        ├── 生成 entry-point.cpp
        ├── 生成 bridge-init.cpp
        ├── 生成 icall-table.cpp
        └── 生成 runtime-glue.sources.cmake
        ↓
    ProjectFileEmitter（新增）
        ├── 生成 CMakeLists.txt
        ├── 生成 il2cpp-config.h
        └── 生成 convert.manifest.json
```

### 8.2 Scriban 模板改造

当前 12 个模板是 proof-level 全包模板。改造为：

| 模板 | 用途 | 输出目标 |
| --- | --- | --- |
| `TypeMethods.cpp.scriban` | 类型方法实现 | `generated/assemblies/<Asm>/types/<Type>.cpp` |
| `AssemblyInit.cpp.scriban` | Assembly 初始化 | `generated/assemblies/<Asm>/<Asm>.assembly.cpp` |
| `AssemblyHeader.h.scriban` | Assembly 导出声明 | `generated/assemblies/<Asm>/<Asm>.assembly.h` |
| `TypeRegistration.cpp.scriban` | 类型注册表 | `metadata/type-registration.cpp` |
| `MethodRegistration.cpp.scriban` | 方法注册表 | `metadata/method-registration.cpp` |
| `StringLiterals.cpp.scriban` | 字符串池 | `metadata/string-literals.cpp` |
| `EntryPoint.cpp.scriban` | 入口点 | `runtime-glue/entry-point.cpp` |
| `BridgeInit.cpp.scriban` | Bridge 初始化 | `runtime-glue/bridge-init.cpp` |
| `ICallTable.cpp.scriban` | ICall 路由 | `runtime-glue/icall-table.cpp` |
| `CMakeProject.cmake.scriban` | CMake 工程 | `CMakeLists.txt` |
| `Config.h.scriban` | 配置头 | `il2cpp-config.h` |

现有 12 个 proof 模板保留用于 proof subject 验证。

---

## 9. 向后兼容

- 现有 proof subject 继续使用旧的单文件输出模式
- `NativeReferenceProofEmitter` 保留不动
- 新的 per-type 输出由新的 `ProductionCodeEmitter` 处理
- `chaos-il2cpp convert` 新增 `--output-mode proof|production` 开关（默认 `production`）
