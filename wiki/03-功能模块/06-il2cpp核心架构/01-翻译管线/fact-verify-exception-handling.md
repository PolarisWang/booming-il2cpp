# Fact Static 验证模式下的异常处理经验

> convert-char 验证调试中积累的关键教训。

---

## 1. Entry.exe 不初始化完整托管运行时

- `bootstrap_runtime()` 只初始化最小结构（字符串表、类型系统骨架等）。
- `GetCurrentRuntimeState()` 返回 `nullptr`。
- 任何依赖完整运行时 ABI 的操作都会崩溃。

## 2. 禁止调用托管异常抛出函数

- `RaiseManagedException()` / `RaiseInvalidCastException()` 最终调用 `abi->raise_managed_exception()`。
- 该指针在 entry.exe 模式下为 `nullptr` → `std::abort()` → 进程退出码 `STATUS_FAIL_FAST` (`0xC0000409`)。
- **原则**：entry.exe 模式不要调用任何 `abi->` 函数。

## 3. C++ `throw` 与 MSVC `/EHc` 的冲突

- 直接 `throw chaos_managed_exception{...}` 原理上可行。
- 但 CMake 默认设置 `/EHsc`（`/EHc` 的含义：`extern "C"` 函数视为不抛异常，不为它们生成栈展开表）。
- 所有 codegen 入口方法均为 `extern "C"`，且包含 `try/catch` 块。`/EHc` 导致这些块的异常无法传播，引发未定义行为。

## 4. 解决方案：强制 `/EHs`

```cmake
set(CMAKE_CXX_FLAGS_INIT "/DWIN32 /D_WINDOWS /utf-8 /EHs")
```

- 必须在 `project()` 调用**之前**设置。
- `_INIT` 后缀使 CMake 不覆盖此值，不走默认的 `/EHsc`。
- `/EHs` 为所有函数（包括 `extern "C"`）生成同步异常展开表。

## 5. Pipeline 重试路径也要同步修复

- `pipeline_native_aot_runner.py` 的 corrected CMakeLists.txt 是实际构建模板。
- 该模板必须同样包含 `/EHs` 标志，否则重试构建仍会用 `/EHsc`。

## 6. 验证模式更简单的替代方案

如果异常无法传播，验证代码可以完全不用 try/catch：

- 外部桩对非法转换直接返回 0（等效于无异常基础设施时的行为）。
- 托管探针阶段（managed probe phase）已在完整 .NET 运行时下验证了正确的异常行为。
- Entry.exe 只需要验证**不崩溃**和**正常路径返回值正确**。

## 7. Object 参数场景的装箱内存布局

`Convert.ToChar(object)` 在托管代码中对 int 参数进行装箱。

Entry.exe 模式下装箱值的布局：

| 区域 | 大小 | 说明 |
|------|------|------|
| FatHeader | 24 字节 | 对象头 |
| Value | 8 字节 | 实际值（int） |

- 值位于偏移 24 处。
- 以 `intptr_t[]` 视角查看时，即 `slots[3]`。

## 8. Bitmask 退出码编码

```text
failures += _exitCode << N
```

- 每个方法的验证失败编码为退出码中的一个 bit。
- Pipeline 解析方式：
  ```
  returncode & ((1 << N) - 1)
  ```
  统计置位 bit 的个数 = 失败方法数。
- 适用于无需异常、用返回值表示成功/失败的验证场景。