# ASan 内存错误检测指南

## 概述

AddressSanitizer (ASan) 是 MSVC 内置的运行时内存错误检测工具，用于捕获：
- 堆缓冲区溢出/下溢（heap buffer overflow）
- 栈缓冲区溢出/下溢（stack buffer overflow）
- 释放后使用（use-after-free）
- 作用域后使用（use-after-scope）
- 重复释放（double-free）
- 内存泄漏（LeakSanitizer）

项目分三阶段引入 ASan：
1. **Phase 1** — CMake 构建基础设施、ASan DLL 自动部署、suppressions 管理
2. **Phase 2** — GC 堆内存的 `__asan_poison`/`__asan_unpoison` 钩子（`asan_interface.h`）
3. **Phase 3** — 扩展至全部测试套件、修复保守栈扫描假阳性

## 构建配置

### 使用 preset（推荐）

```bash
cmake --preset asan
cmake --build artifacts/presets/asan
```

### 手动配置

```bash
cmake -S testing -B build/asan \
    -DUSE_ASAN=ON \
    -DCMAKE_BUILD_TYPE=Debug
cmake --build build/asan
```

### ASan 构建的特殊行为

- `gtest_force_shared_crt` 设为 `OFF`（ASan 要求静态链接 CRT）
- `_DISABLE_VECTOR_ANNOTATION` / `_DISABLE_STRING_ANNOTATION` 宏已定义（避免 STL 容器 ASan 误报）
- `add_link_options(/FORCE:MULTIPLE)` 允许重复符号（MSVC ASan 对 `extern "C"` 函数生成的 COMDAT thunk 会引入 LNK2005）
- `clang_rt.asan_dynamic-x86_64.dll` 通过 CMake `POST_BUILD` 自动复制到每个测试输出目录
- `asan-suppressions.txt` 通过 CMake `POST_BUILD` 自动复制到每个测试输出目录

## 运行 ASan 测试

### 运行全部非 stress 测试

```bash
cd testing/build
ASAN_OPTIONS=suppressions=asan-suppressions.txt ctest -C Debug -E "stress|bgc|gc_oversized"
```

### 运行指定测试

```bash
ASAN_OPTIONS=suppressions=asan-suppressions.txt ctest -C Debug -R "test_gc_region"
```

### 直接运行测试可执行文件

```bash
ASAN_OPTIONS=suppressions=asan-suppressions.txt ./runtime-core/gc/Debug/test_gc_region.exe
```

**注意**：必须使用 `-C Debug`（CMake 多配置生成器），且必须在 `build` 目录下运行（suppressions 文件路径解析基于 CWD）。

## 理解 ASan 输出

ASan 报告的典型格式：

```
=================================================================
==12345==ERROR: AddressSanitizer: heap-buffer-overflow on address 0x...
WRITE of size 4 at 0x... thread T0
    #0 0x... in MyFunction my_file.cpp:42
    #1 0x... in TestBody my_test.cpp:10
    ...
0x... is located 0 bytes after 1024-byte region [...]
freed by thread T0 here:
    #0 0x... in free
    #1 0x... in MyFreeFunction ...
previously allocated by thread T0 here:
    #0 0x... in malloc
    #1 0x... in MyAllocFunction ...
```

关键信息：
- **错误类型**：`heap-buffer-overflow`、`stack-use-after-scope`、`use-after-free`
- **访问地址**：出错的内存地址
- **分配/释放栈**：对象被分配和释放的位置
- **Shadow bytes**：`[f8]` 标记 poison 区域

### Shadow Byte 解读

| 值 | 含义 |
|----|------|
| `00` | 可寻址（Addressable） |
| `fa` | 堆左 redzone |
| `fd` | 已释放堆内存 |
| `f1` | 栈左 redzone |
| `f2` | 栈中间 redzone |
| `f3` | 栈右 redzone |
| `f5` | 返回后栈内存 |
| `f8` | 作用域后栈内存 |

## 假阳性处理

### 保守 GC 栈扫描

GC 在标记根时会扫描线程栈上的所有指针大小值。ASan 的栈 redzone 也在扫描范围内。这种保守扫描导致的假阳性通过 `AsanReadPtrNoCheck` 绕过：

```cpp
// src/native/common/chaos/asan_interface.h
// 在保守栈扫描位置替代直接指针读取
void* val = AsanReadPtrNoCheck(
    reinterpret_cast<void*>(slot));
```

使用位置：
- `thread_state.cpp` — 多线程栈扫描（T4 返回地址、saved RBP、保守值槽）
- `gc_old_gen.cpp` — Collect/TryMarkRoot 中的全局句柄扫描

### Suppressions 管理

当 ASan 假阳性无法通过代码规避时，注册 suppression。当前活跃 suppression（`cmake/sanitizers/asan-suppressions.txt`）：

```
interceptor_via_fun:DomainArenaAlloc
interceptor_via_fun:ThreadPoolWorkerThread
interceptor_via_fun:MarkSweepOldGen::Collect
```

#### 添加新 suppression

1. 从 ASan 报告中获取函数名（`interceptor_via_fun:<function_name>`）
2. 添加到 `cmake/sanitizers/asan-suppressions.txt`
3. 在 suppression 旁加注释说明原因

#### Suppression 查找策略

ASan 按以下顺序查找 suppressions 文件：
1. `ASAN_OPTIONS=suppressions=<path>` 指定的路径（相对路径基于 CWD）
2. 文件必须存在于测试可执行文件的工作目录中

项目中通过 CMake `configure_file` 将 `cmake/sanitizers/asan-suppressions.txt` 复制到 `testing/build/`，再通过 `POST_BUILD` 复制到每个测试输出目录。

## 已知限制

### 跨线程假阳性（不可消除）

ASan 无法跨线程跟踪栈帧生命周期。以下场景会产生跨线程假阳性：
- **BGC（后台 GC）** — 工作线程并发扫描其他线程的栈
- **ThreadPool 工作线程** — 线程复用导致栈内存残留
- **Stress 测试** — 大量并发分配/释放

这些场景用 suppression 绕过，不在非 stress 测试中出现。

### ASan 运行时开销

- **CPU 开销**：约 2x 性能降级（每次内存访问额外检查）
- **内存开销**：约 3x（shadow memory + redzones）
- **不适用于**：性能 benchmark、生产构建、长时间压力测试

### 不支持的场景

- `cmake --preset ship` / `release` 构建不支持 ASan
- 跨 DLL 边界的内存错误检测有限
- Windows 上 SEH/VEH 异常处理可能与 ASan 交互异常

## 常见问题

### `clang_rt.asan_dynamic-x86_64.dll` 缺失

```
程序无法启动，因为计算机中丢失 clang_rt.asan_dynamic-x86_64.dll
```

**解决方案**：CMake 已通过 `POST_BUILD` 自动复制 DLL 到每个测试输出目录。如果仍缺失：
1. 确认 Visual Studio 安装了 ASan 组件（"C++ AddressSanitizer"）
2. 手动从 `VS/VC/Tools/MSVC/14.x/bin/Hostx64/x64/` 复制到测试输出目录

### LNK2005 重复符号

```
error LNK2005: chaos_gc_dirty_card already defined in chaos_interpreter.lib
```

MSVC ASan 对 `extern "C"` 函数生成 COMDAT thunk，跨静态库引用时产生重复符号。

**解决方案**：CMake 已添加 `/FORCE:MULTIPLE` 链接标志：
```cmake
add_link_options(/fsanitize=address /FORCE:MULTIPLE)
```

### `_CrtDbgReport` 链接错误

```
unresolved external symbol __imp__CrtDbgReport
```

ASan 构建需要 `gtest_force_shared_crt OFF`。确认 CMake 配置中已设此项。

### STL 容器 ASan 误报

```
stack-use-after-scope on std::string internal buffer
```

项目中已定义 `_DISABLE_VECTOR_ANNOTATION` 和 `_DISABLE_STRING_ANNOTATION` 消除 STL 容器的 ASan 注解。如果遇到新的 STL 误报，确认这两个宏已定义。

### 测试在 ASan 下超时

ASan 约 2x 性能开销。Stress 测试（`_stress` 后缀）在 ASan 下可能超时或过慢：
- 使用 `-E "stress"` 排除
- 或使用 `--timeout 600` 增加超时

## 构建架构参考

### CMakeLists.txt 中的 ASan 配置

```cmake
# tests/unit/runtime-native/CMakeLists.txt
if(USE_ASAN)
    add_compile_options(/fsanitize=address)
    add_link_options(/fsanitize=address /FORCE:MULTIPLE)
    add_compile_definitions(_DISABLE_VECTOR_ANNOTATION _DISABLE_STRING_ANNOTATION)
    find_file(CHAOS_ASAN_DLL clang_rt.asan_dynamic-x86_64.dll ...)
    configure_file(asan-suppressions.txt ${CMAKE_BINARY_DIR}/ COPYONLY)
endif()
```

### 🆕 新增测试目标

创建新测试时，使用 `add_chaos_test` 函数（自动处理 ASan DLL 和 suppressions 复制）：

```cmake
add_chaos_test(test_my_feature
    my_feature_test.cpp
    LIBS chaos_runtime_core
)
```

### 关键文件

| 文件 | 作用 |
|------|------|
| `cmake/sanitizers/asan-suppressions.txt` | 全局 ASan suppression 配置 |
| `src/native/common/chaos/asan_interface.h` | ASan 抽象层（Unpoison/Poison/ReadPtrNoCheck） |
| `tests/unit/runtime-native/CMakeLists.txt` | ASan 构建基础设施 |
| `CMakePresets.json` | ASan preset 定义 |
