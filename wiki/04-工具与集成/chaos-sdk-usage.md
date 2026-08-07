# Chaos IL2CPP SDK 使用指南

## 概述

`chaos-il2cpp convert-to-cpp --sdk-out` 输出自包含的 SDK 目录，消费方通过 `find_package(chaos)` 一行接入。

SDK 目录结构：

```
codegen/
├── chaos-config.cmake          # find_package(chaos) 入口
├── include/
│   ├── chaos.h                 # 单头文件聚合入口
│   ├── chaos_generated_module.h # 生成的分发表
│   ├── chaos_generated_module.cpp
│   ├── ChaosGeneratedRuntimePrelude.h
│   ├── chaos_runtime_host.h    # RAII Host 类
│   ├── chaos/*.h               # 运行时头文件（chaos/common.h, config.h, log.h 等）
│   ├── gc/*.h                  # GC 头文件
│   ├── runtime_stubs/*.h      # 运行时桩头文件
│   └── contracts/*.h           # 合约头文件（codegen_bridge.h, runtime_abi.h 等）
├── lib/
│   ├── chaos_codegen.lib       # 预编译的 native-aot.generated.cpp
│   ├── chaos_runtime_core.lib  # 预构建运行时库
│   ├── chaos_interpreter.lib
│   ├── chaos_bootstrap.lib
│   ├── chaos_common.lib
│   ├── chaos_support.lib
│   ├── chaos_fmt.lib
│   ├── chaos_jit.lib
│   ├── chaos_debugger.lib
│   ├── chaos_hot_update.lib    # 热更新支持
│   └── chaos_eventpipe.lib
└── cmake/
    └── chaos-targets.cmake     # target 定义
```

## CMake 接入

### 最小接入（推荐）

```cmake
cmake_minimum_required(VERSION 3.20)
project(my_app LANGUAGES CXX)

# 一行获取全部
find_package(chaos REQUIRED PATHS path/to/codegen)

add_executable(my_app main.cpp)
target_link_libraries(my_app PRIVATE
    chaos::codegen   # 预编译生成代码
    chaos::runtime   # 运行时库 + 编译选项
)
```

### 调试模式（step-into generated code）

```cmake
find_package(chaos REQUIRED PATHS path/to/codegen)

# 自己编译生成的 .cpp（方便单步调试）
add_library(my_codegen STATIC path/to/chaos_generated_module.cpp)
target_link_libraries(my_codegen PRIVATE chaos::runtime)

add_executable(my_app main.cpp)
target_link_libraries(my_app PRIVATE
    my_codegen
    chaos::codegen    # 仍然链接预编译代码确保符号完整
    chaos::runtime
)
```

## Chaos::runtime 提供的编译选项

| 选项 | 说明 |
|------|------|
| `/utf-8` | UTF-8 源码编码 |
| `/EHa` | 同步+异步异常处理（C++ 异常拦截） |
| `/GS-` | 禁用缓冲区安全检查 |
| `cxx_std_20` | C++20 标准 |
| `/DWIN32 /D_WINDOWS` | Windows 平台定义 |

## Chaos::codegen

预编译的 `native-aot.generated.cpp`（~1MB .lib），包含 assembly 中所有方法的 AOT 翻译结果。

### 链接顺序

`chaos::codegen` 必须在 `chaos::runtime` **之前**链接，因为 codegen 符号依赖 runtime 符号。CMake 的 `target_link_libraries` 会保持指定顺序。

## 头文件索引

### 聚合入口

`#include <chaos.h>` 一行包含全部常用头文件：

```c
#include <chaos.h>

int main() {
    ChaosRuntimeHost host;
    if (!host.Initialize()) return -1;
    host.RegisterModule(&s_codeRegistration, &s_metadataRegistration, &s_codegenOptions);
    // Call generated methods...
}
```

### 包含路径

SDK `include/` 在 `chaos::runtime` 的 INTERFACE include directories 中，消费方代码：

- `#include <chaos.h>` — 聚合入口
- `#include <chaos/common.h>` — 公共类型和宏
- `#include <chaos/log.h>` — 日志
- `#include <runtime_abi.h>` — Runtime ABI v0
- `#include <codegen_bridge.h>` — Codegen 注册桥接
- `#include <chaos_generated_module.h>` — 生成的分发表

## Foundation-DLL Pipeline 集成

验证管线（`tests/e2e/verification/`）已使用 SDK 模式：

1. `convert-to-cpp` 调用时传入 `--sdk-out <codegen_dir>` 输出 SDK
2. `find_package(chaos)` 在 native/CMakeLists.txt 中自动生成
3. `chaos::runtime` + `chaos::codegen` 提供完整链接环境
4. 预编译的 `chaos_codegen.lib` 避免每次重新编译生成代码

## 与旧模式对比

| 维度 | 旧模式（硬编码路径） | SDK 模式 |
|------|---------------------|----------|
| CMakeLists.txt | 60+ 行硬编码路径 | 5 行 `find_package` |
| 运行时库引用 | 每 family 各自指定 artifact 路径 | SDK lib/ 统一管理 |
| 生成代码编译 | 每次 cmake --build 重新编译 | 预编译 chaos_codegen.lib |
| 头文件访问 | 依赖源码树结构 | SDK include/ 自包含 |
| 跨机器可移植 | 需要完整 repo | 只需 SDK 目录 |

## 已知限制

- `chaos/runtime.h` 内部引用 `<fmt/format.h>`，消费方仍需提供 `third_party/fmt/include` 路径（如编译 stub 文件时）
- 部分 stub 文件（microbench.cpp, enum_stubs.cpp）需要源码树内部头文件（fast_frame_pool.h, string_table.h, reflection_api.h 等）
- 如需完全自包含（零源码树依赖），需在 SDK include/ 中补充这些内部头文件
