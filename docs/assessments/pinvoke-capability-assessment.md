# P/Invoke DllImport 能力综合评估报告

> 评估日期：2026-05-17（更新）
> 评估范围：metadata 解析 → AOT codegen → 原生运行时完整管线

---

## 1. 当前架构总览

### 1.1 管线三阶段

```
[Metadata 层]                    [Codegen 层]                      [Runtime 层]
LoaderStage                   AotCoreIrLowering                native_library.cpp
  TryDescribeMethodImport()     TryCreateMethod()                 NativeLibraryLoad
  └─ ModuleReference              └─ IsPInvoke=true              NativeLibraryGetProcAddress
  └─ EntryPoint                   └─ Import (ModuleRef + Name)   NativeLibraryFree
  └─ CallingConvention            └─ Param classification        └─ 全局 unordered_map
  └─ CharSet                          ├─ string                       └─ canonicalized names
  └─ SetLastError                     ├─ SafeHandle
                                      ├─ blittable struct        native_library.h
                                      └─ non-blittable struct     └─ shared_mutex 保护
                                                              └─ refcount 管理
                                EmitPInvokeMethod()
                                  ├─ static local lib handle   core/marshal_api.cpp
                                  ├─ static local fn ptr       └─ MarshalAllocHGlobal
                                  ├─ GC_TRANSITION wrapper     └─ MarshalStringToCoTaskMemUtf8
                                  ├─ __Internal direct link    └─ MarshalStringToCoTaskMemUni
                                  ├─ SuppressGCTransition skip └─ SetLastPInvokeError
                                  ├─ DllImportResolver check   core/struct_marshal.cpp
                                  ├─ SetLastError wrapper      └─ MarshalStructManagedToNative
                                  ├─ calling convention attr   └─ MarshalStructNativeToManaged
                                  ├─ blittable: direct cast    └─ DestroyMarshalledStruct
                                  └─ non-blittable: marshal    core/marshal_string.cpp
                                      ├─ string conversion     core/marshal_alloc.cpp
                                      ├─ struct marshalling    delegate_thunks.cpp (reverse P/Invoke)
                                      └─ SafeHandle extraction
```

### 1.2 已支持能力

| 能力 | 状态 | 位置 |
|------|------|------|
| DllImport metadata 解析 | 支持 | `LoaderStage.MetadataResolution.cs:137` |
| CallingConvention 映射 | 支持 | WinApi/CDecl/StdCall/ThisCall/FastCall |
| CharSet 映射 | 支持 | Ansi/Unicode/Auto（Auto=Unicode on Windows, Ansi on other） |
| SetLastError | 支持 | codegen ClearOsLastError + GetOsLastError; runtime SetLastPInvokeError |
| blittable 直接调用 | 支持 | codegen static local + reinterpret_cast |
| string marshalling | 支持 | CoTaskMem 分配 UTF8/UTF16 转换 |
| struct marshalling | 支持 | descriptor 驱动 field-by-field |
| SafeHandle 传递 | 支持 | `marshal_api.cpp:MarshalSafeHandleToIntPtr` |
| reverse P/Invoke (calli) | 支持 | `delegate_thunks.cpp` 含 Arity0-4 |
| 返回值 + [out] param | 支持 | codegen 四种路径覆盖 |
| NativeLibrary Load/Free API | 支持 | `native_library.cpp` refcounting |
| DllNotFoundException 处理 | 支持 | codegen emit `raise` + `throw` |
| **GC 模式切换 (GC_TRANSITION)** | **支持** | codegen 四条路径预置 `GC_TRANSITION_TO_PREEMPTIVE/COOPERATIVE` |
| **__Internal 静态链接** | **支持** | codegen 检测 `__Internal` → 跳过 LoadLibrary，直接 extern 声明 + 函数指针 |
| **SuppressGCTransition** | **支持** | codegen 检测 `[SuppressGCTransition]` → 跳过 GC_TRANSITION 宏 |
| **CharSet.Auto 平台感知** | **支持** | `IsWindowsTarget` 常量 + `IsUnicodeCharSet()` 按平台映射 |
| **P/Invoke override (DllImportResolver)** | **部分支持** | codegen + native 层完成；managed 回调注册待补 |

---

## 2. 横向对比

### 2.1 能力矩阵

| 特性 | Chaos IL2CPP | Unity IL2CPP | Mono |
|------|-------------|--------------|------|
| **GC 模式切换** | **支持** | 自动生成 trampoline | JIT trampoline |
| **__Internal 链接** | **支持** | `[DllImport("__Internal")]` 原生静态链接 | `[DllImport("__Internal")]` 原生静态链接 |
| **SuppressGCTransition** | **支持** | `[SuppressGCTransition]` 跳过切换 | `[SuppressGCTransition]` 跳过切换 |
| **SetLastError** | 支持 | 支持 | 支持 |
| **calli (reverse P/Invoke)** | 支持 | 支持 | 支持 |
| **string marshalling** | 支持 | 支持 | 支持 |
| **struct marshalling** | 支持 | 支持 | 支持 |
| **SafeHandle** | 支持 | 支持 | 支持 |
| **blittable 优化路径** | 支持 | 支持 | JIT AOT 支持 |
| **DllImportResolver** | **部分支持** | 支持 | 支持 |
| **LCID/Charset 自动转换** | 部分 | 支持 | 支持 |
| **varargs** | 不支持 | 有限支持 | 支持 |
| **best-fit mapping** | 不支持 | 支持 | 支持 |
| **callback/delegate 参数** | 支持 | 支持 | 支持 |
| **COM interop** | 不支持 | 有限支持 | 支持 |
| **WinRT interop** | 不支持 | 支持 | 不支持 |
| **Marshal.GetFunctionPointerForDelegate** | 支持 | 支持 | 支持 |

### 2.2 GC 模式切换（已修复）

当前 codegen 在四条 P/Invoke 生成路径中都正确插入了 `GC_TRANSITION_TO_PREEMPTIVE()` 和 `GC_TRANSITION_TO_COOPERATIVE()`，并受 `SuppressGCTransition` 属性控制跳过的能力。

```cpp
// Chaos IL2CPP 当前生成的 P/Invoke wrapper（已修复）
static void* s_lib = nullptr;
static FnPtr s_fn = nullptr;
if (!s_lib) {
    s_lib = NativeLibraryLoad(module_name);
    s_fn = NativeLibraryGetProcAddress(s_lib, entrypoint);
}
GC_TRANSITION_TO_PREEMPTIVE()       // ← 已加入
result = s_fn(args);
GC_TRANSITION_TO_COOPERATIVE()      // ← 已加入
SafepointPoll()                      // ← 在 GC_TRANSITION_TO_COOPERATIVE 内部
```

### 2.3 __Internal 静态链接（已修复）

`[DllImport("__Internal")]` 被 codegen 识别后跳过 LoadLibrary，直接生成：

```cpp
// __Internal 路径: 编译时链接，仅声明外链符号
extern "C" void entryPointName();
static FnPtr s_fn = nullptr;
if (!s_fn) {
    s_fn = &::entryPointName;
}
GC_TRANSITION_TO_PREEMPTIVE()
result = s_fn(args);
GC_TRANSITION_TO_COOPERATIVE()
```

### 2.4 SuppressGCTransition（已修复）

检测 `[SuppressGCTransition]` 属性后，跳过 GC_TRANSITION 宏：

```cpp
// SuppressGCTransition 路径: 零额外开销直接调用
result = s_fn(args);
// 没有 GC_TRANSITION，没有 SafepointPoll
```

---

## 3. 问题状态

### P0 — 正确性风险（已全部修复）

| # | 问题 | 状态 | 修复位置 |
|---|------|------|----------|
| 1 | **P/Invoke codegen 未插入 GC 模式切换** | **已修复** | `NativeAotLoweringPlanner.MethodEmission.cs` 四条路径增加 `GC_TRANSITION_TO_PREEMPTIVE/COOPERATIVE` |
| 2 | **缺少 __Internal 支持** | **已修复** | `NativeAotLoweringPlanner.MethodEmission.cs` 增加 __Internal 检测 + extern 声明路径 |

### P1 — 功能完整性（部分修复）

| # | 问题 | 状态 | 说明 |
|---|------|------|------|
| 3 | **缺少 `[SuppressGCTransition]` 支持** | **已修复** | `NativeAotLoweringPlanner.MethodEmission.cs` 检测属性后跳过 GC_TRANSITION |
| 4 | **部分 CharSet 行为未实现** | **已修复** | `IsWindowsTarget` 常量 + `IsUnicodeCharSet()` 按平台映射 CharSet.Auto |
| 5 | **无 P/Invoke override 机制** | **部分完成** | codegen + native `TryResolveDllImport` 完成；managed 回调注册待实现 |

### P2 — 工程化提升（部分完成）

| # | 问题 | 状态 | 说明 |
|---|------|------|------|
| 6 | **codegen 输出无 GC_TRANSITION benchmark 数据** | **已修复** | 新增 `PInvokeGcTransitionBenchmark.cs`，含 baseline/suppressed 对照 |
| 7 | **marshalling 错误路径未充分测试** | **已修复** | 新增 `PInvokeMarshalling` smoke test + `test_pinvoke_marshalling_coverage.py` 单元测试 |

---

## 4. 差距根因分析（历史）

### 4.1 GC 过渡缺失的技术背景

分析发现，GC_TRANSITION 宏（`gc_transition.h`）**已经存在**并在以下内部模块使用：
- `wait_handle.cpp` — WaitForSingleObject 等阻塞调用
- `synchronization.cpp` — Monitor::Wait 等同步操作
- `timer_queue.cpp` — 定时器回调
- `thread_pool.cpp` — 线程池等待

**但 codegen 层** `EmitPInvokeMethod()` 生成的 P/Invoke wrapper **完全没有引用** GC_TRANSITION。这意味着：

1. 由 codegen 生成的每个 P/Invoke 调用 → **无 GC 模式切换**
2. 由 runtime 内部手写的阻塞调用 → **有 GC 模式切换**
3. 这个割裂说明：GC_TRANSITION 是在 runtime 层"补"的，codegen 管线从未接上

> 此问题已于 2026-05 月修复，codegen 四条路径均插入 GC_TRANSITION 宏。

### 4.2 历史原因推测

从代码结构推断，GC_TRANSITION 是在 GC 开发后期（BGC + CRAG 阶段）才加入的，而 P/Invoke codegen 管线开发更早。两者没有同步对接。

---

## 5. 剩余工作

### 5.1 P/Invoke Override 最后一公里

当前状态：codegen 层在 `NativeLibraryLoad` 前插入 `TryResolveDllImport` 检查；native 层 `native_library.cpp` 实现了函数指针回调机制。**缺少** managed 端：

1. `PInvokeResolverRegistry` — 管理 `Assembly → DllImportResolver` 映射的静态类
2. `[UnmanagedCallersOnly]` 回调 — native 代码可调用的 resolver 入口
3. 初始化注册 — 将 managed 回调注册到 `RegisterPInvokeResolverCallback`

### 5.2 验证方案（当前状态）

| 场景 | 验证方式 | 状态 |
|------|----------|------|
| GC_TRANSITION 存在性 | 编译后反编译 P/Invoke wrapper 确认 | **已确认** |
| GC safepoint 参与 | 多线程 P/Invoke调用 + BGC触发，确认 STW 时间正常 | **待验证** |
| __Internal 链接 | 静态链接 .obj 后通过 `[DllImport("__Internal")]` 调用 | **已实现，待集成测试** |
| SuppressGCTransition | benchmark 对照实验验证性能差异 | **benchmark 已添加** |
| SetLastError 正确性 | P/Invoke 调用 `SetLastError(ERROR_ACCESS_DENIED)` 后 `Marshal.GetLastWin32Error()` 返回正确值 | **smoke test 已覆盖** |
| blittable 性能 | 空 P/Invoke 调用微基准，确认零额外开销 | **已覆盖** |
| string/struct marshalling | 各编码 + 复杂结构体 marshalling 往返一致性 | **smoke test 已覆盖** |

---

## 6. 总结

**核心结论**：Chaos IL2CPP P/Invoke 管线在架构上完整（metadata → codegen → runtime 三阶段齐备），**全部 P0 问题已修复**：

1. **GC 模式切换**（原 P0）— 已修复，四条 codegen 路径均插入 `GC_TRANSITION_TO_PREEMPTIVE/COOPERATIVE`
2. **__Internal**（原 P0）— 已修复，codegen 检测 `__Internal` 后跳过 LoadLibrary
3. **SuppressGCTransition**（原 P1）— 已修复，受属性控制可跳过 GC 切换
4. **CharSet.Auto**（原 P1）— 已修复，`IsWindowsTarget` 常量驱动映射
5. **DllImportResolver**（原 P1）— 部分完成，codegen + native 层就绪，managed 回调待补
6. **Benchmark + 测试覆盖**（原 P2）— 已补全

当前唯一剩余缺口是 **P/Invoke Override managed 回调注册**（DllImportResolver 最后一公里）。
