# A3 综合分析与报告 — standalone tool 编译失败

> 日期：2026-09-01
> 分析范围：`chaos-il2cpp` dotnet tool 在**非 repo 环境**（standalone）下 `publish` 时 C++ 编译失败
> 影响：最终用户实际路径（`dotnet tool install chaos-il2cpp` → `chaos-il2cpp publish HelloWorld.csproj` → 编译 entry.exe）

---

## 1. 现状

### 1.1 已修复并推送的

| 子项 | 状态 | 提交 |
|------|------|------|
| 嵌入式 SDK 头文件完整打包（224 个 .h，含 interpreter_entry.h / exception_jmp.h / runtime_abi.h / codegen_bridge.h / stubs.h / ankerl） | ✅ 已修复 | `2436e3cf8` |
| `SdkEmitter.cs` interpreter_entry.h 从 `src/native/interpreter/` 拷贝（原路径指向 runtime-core，bug） | ✅ 已修复 | `2436e3cf8` |
| `build-tool-package.sh` 全量拷贝 runtime-core + contracts + interpreter + runtime_stubs + ankerl | ✅ 已修复 | `2436e3cf8` |
| `chaos-targets.cmake` INTERFACE 加 `_HAS_EXCEPTIONS=1` | ✅ 已修复 | `2436e3cf8` |
| `stubs.h` threading_stubs.h 移出 `extern "C"` 块 | ✅ 已修复 | `a1584204b` |
| `native_types.h` + `chaos_pch.h` forward-declare CRT terminate 符号 | ✅ 已修复 | `266322f95` |
| 清理重复 nupkg | ✅ 已修复 | 途中 |

### 1.2 剩余问题

当前执行 `chaos-il2cpp publish HelloWorld.csproj --mode app` 后手动编译的错误：

```
C2039: 'set_terminate' is not a member of 'global namespace'
C2873: 'set_terminate' cannot be used in a using-declaration
C2381: 'terminate': redefinition; 'noreturn' differs
C1083: Cannot open include file: 'gc_helpers.h' (stub_common.h)
C2338: static_assert failed: 'Unicode support requires compiling with /utf-8'
```

---

## 2. 根因分析（3 个独立问题）

### 问题 A：`std::terminate` C2039 — MSVC 14.44 + CRT 链接规范冲突

**严重程度：🔴 高**

**现象**：`<exception>(38)` 的 `_EXPORT_STD using ::terminate;` 报 C2039，`<exception>(41)` 的 `using ::set_terminate;` 报 C2039。

**根因链**：
1. MSVC 14.44 BuildTools 的 `<exception>` 头（约 14.44.35207）在第 38 行做 `_EXPORT_STD using ::terminate;`
2. `_EXPORT_STD` 是 `__declspec(dllexport)` 或空，取决于 TU 的 DLL 导入/导出状态
3. `::terminate` 的唯一声明在 Windows Kits UCRT 的 `corecrt_terminate.h` 中，用 `_CRT_BEGIN_C_HEADER`（`extern "C"`）包裹
4. 当 MSVC 的 UCRT 路径正确铺入时，`corecrt_terminate.h` 被包含（C2381 证明），但 `extern "C"` 链接的 `::terminate` 不满足 `<exception>` 的 C++ using 要求
5. 当 MSVC 的 UCRT 路径**未正确铺入**时，`corecrt_terminate.h` 被跳过，`::terminate` 完全不声明，C2039 直接报
6. 当前环境（BuildTools）UCRT 路径通过 `%(AdditionalIncludeDirectories)` 被 MSBuild 自动添加，但**顺序是 SDK include 路径在前**，导致 `corecrt_terminate.h` 被包含但链接规范不匹配

**关键证据**：`C2381 'terminate': redefinition; 'noreturn' differs` 证明了 `corecrt_terminate.h` 已被包含，但我们的 forward-declare 与 CRT 的 `extern "C"` 声明冲突。

### 问题 B：`gc_helpers.h` 路径错误 — 嵌入式 SDK 目录结构不匹配

**严重程度：🟡 中**

**现象**：`stub_common.h(8)` 报 `C1083: Cannot open include file: 'gc_helpers.h'`。

**根因**：
- Repo 内 `stub_common.h` 做 `#include "gc_helpers.h"`（引号，相对路径，期望在 cwd 或当前 include 根）
- 嵌入式 SDK 把 `gc_helpers.h` 放在 `include/gc/gc_helpers.h`（子目录），但 `include/` 根下没有 `gc_helpers.h`
- 在 repo 中 `runtime_stubs/` 与 `gc/` 是同级的（都在 `runtime-core/` 下），所以 `"gc_helpers.h"` 能解析
- 在嵌入式 SDK 中，`include/runtime_stubs/stub_common.h` 引用 `"gc_helpers.h"`，但 `include/gc/gc_helpers.h` 在独立子目录，不在 include 搜索路径的根下

### 问题 C：`fmt/base.h` 需要 `/utf-8` 编译标志

**严重程度：🟢 低**

**现象**：`static_assert failed: 'Unicode support requires compiling with /utf-8'`

**根因**：CMake 的 `add_compile_options` 中的 `/utf-8` 被 generator expression 包裹，在 `find_package(chaos)` 的 INTERFACE 传播中可能未正确铺设到所有 TU。

---

## 3. 目标

**最终目标**：用户在任意 Windows 机器上运行以下命令后，`entry.exe` 编译成功且 exit 0：

```bash
dotnet tool install --global chaos-il2cpp --add-source <feed>
chaos-il2cpp publish HelloWorld.csproj --mode app --output /tmp/out
/tmp/out/build/RelWithDebInfo/chaos_entry.exe
# 期望输出: HelloWorld smoke entry reached.
# 期望 exit: 0
```

### 量化目标

| 指标 | 当前 | 目标 |
|------|------|------|
| standalone 编译通过率 | ❌ 0%（3 个错误拦截） | ✅ 100% |
| 产物完整性 | ❌ entry.exe 不存在 | ✅ entry.exe + manifest 存在 |
| 运行 exit code | ❌ N/A（无 exe） | ✅ 0 |
| 运行时 stdout | ❌ N/A | ✅ "HelloWorld smoke entry reached." |
| repo 内 dev 模式 | ✅ 全部 PASS | ✅ 保持 |

---

## 4. 修复计划（3 个子项）

### 修复 A：`terminate` C2039 — 统一 extern "C" 前向声明

**方案**：在 `native_types.h` 和 `chaos_pch.h` 中，用 `extern "C"` 包裹 terminate 前向声明，匹配 `corecrt_terminate.h` 的链接规范。

```cpp
#ifdef _MSC_VER
// 匹配 corecrt_terminate.h 的 _CRT_BEGIN_C_HEADER 链接规范
extern "C" {
    __declspec(noreturn) void __cdecl terminate() throw();
    typedef void (__cdecl* terminate_handler)();
    terminate_handler __cdecl set_terminate(terminate_handler) throw();
    terminate_handler __cdecl _get_terminate() throw();
}
#endif
```

**关键**：必须同时移除当前非 `extern "C"` 的声明，否则 C2381。`build-tool-package.sh` 的嵌入式 SDK 包含需要同步。

**工作量**：0.5 天（改 2 个头 + 重建 nupkg + 验证）

### 修复 B：`gc_helpers.h` — 额外 include 路径

**方案**：在 `PublishController.cs` 生成的 CMakeLists.txt 中，将 `sdk/include/gc/` 也加入 `target_include_directories`（当前只加了 `sdk/include/` 和 `sdk/include/chaos` 等，缺 `gc/` 子目录作为 include 根）。

**或者**：在 `build-tool-package.sh` 中把 `gc_helpers.h` 复制一份到 `include/` 顶层。

**工作量**：0.25 天（改 CMakeLists 生成 或 打包脚本）

### 修复 C：`/utf-8` — 确保 CMake 标志传播

**方案**：确认 `add_compile_options` 的 `/utf-8` 在 `chaos-targets.cmake` 的 INTERFACE 中正确存在。当前已存在，可能是 `find_package(chaos)` 的 `CHAOS_SDK_DIR` 路径问题导致 `chaos-config.cmake` 找不到 `chaos-targets.cmake`。

**工作量**：0.25 天（验证 config 路径）

---

## 5. 风险分析

| 风险 | 等级 | 说明 | 缓解 |
|------|------|------|------|
| **R1: extern "C" 前向声明与 CRT 的 `_ACRTIMP` 宏冲突** | 🟡 中 | `extern "C"` 包围的声明与 `corecrt_terminate.h` 的 `_CRT_BEGIN_C_HEADER` 完全相同，但如果某头文件在 `_CRT_BEGIN_C_HEADER` 前被包含，`_ACRTIMP` 宏未定义 -> C4430 | 不用 `_ACRTIMP`，直接用 `__declspec(noreturn)` |
| **R2: 修复后回归 dev 模式** | 🟢 低 | 头文件改动在 repo 编译路径中也存在，但 dev 模式有完整的 UCRT 路径，声明不会冲突 | 验证：`python scripts/publish-smoke.py` 3 case 全 PASS |
| **R3: CI 的 `windows-latest` 的 MSVC 版本不同** | 🟡 中 | CI 用 VS 2022 Pro（14.38），本地用 BuildTools（14.44）。`_EXPORT_STD` 行为在 14.38 vs 14.44 可能不同 | 修复后 CI 自动验证 |
| **R4: 修复后 A2（Main(args) null）未解决** | 🔴 高 | standalone 编译通过后，运行 `entry.exe` 传入 null string[]，如果 Main 读 `args.Length` 会崩 | 运行时创建空数组 helper，需 runtime 专项，1-2 天 |
| **R5: 修复不完全，用户仍遇到其他路径问题** | 🟡 中 | 当前发现 3 个问题，可能还有更多（如 `stub_common.h` 的 `gc_helpers.h` 路径是第 3 个隐藏问题） | 修复后全量跑 `publish-smoke` 3 case |

---

## 6. 工作量估算

| 项 | 预估时间 | 依赖 |
|------|---------|------|
| 修复 A：extern "C" terminate 声明 | 0.5 天 | 无 |
| 修复 B：gc_helpers.h 路径 | 0.25 天 | 无 |
| 修复 C：/utf-8 传播验证 | 0.25 天 | 无 |
| 重建 nupkg + 验证 dev 模式回归 | 0.5 天 | A+B+C |
| **合计** | **1.5 天** | — |

**注意**：A2（Main(args) null 崩溃）不是 A3 的一部分，但修复 A3 后会暴露 A2。A2 需要 runtime 空数组创建 API，约 1-2 天，**独立于 A3**。

---

## 7. 结论

**A3 的难度等级：中等**。3 个问题中，2 个（B、C）是配置问题，E 1 个（A）是 MSVC 版本兼容性。

**当前状态**：90% 修复已完成（头文件完整性、interpreter 路径、_HAS_EXCEPTIONS、extern C 块拆分）。剩余 3 个问题预计 1.5 天。

**建议**：如果 1.5 天的投入对当前优先级是可接受的，建议立即修复。如果优先级更高的是发布 v0.2.0 验证端到端流程，建议先走 dev 模式（repo 内），A3 收尾排期到 1.5 天后。