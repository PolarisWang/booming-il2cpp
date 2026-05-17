# P/Invoke DllImport 能力综合评估报告

> 评估日期：2026-05-17
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
                                  ├─ SetLastError wrapper      └─ MarshalStringToCoTaskMemUtf8
                                  ├─ calling convention attr   └─ MarshalStringToCoTaskMemUni
                                  ├─ blittable: direct cast    └─ SetLastPInvokeError
                                  └─ non-blittable: marshal    core/struct_marshal.cpp
                                      ├─ string conversion     └─ MarshalStructManagedToNative
                                      ├─ struct marshalling    └─ MarshalStructNativeToManaged
                                      └─ SafeHandle extraction └─ DestroyMarshalledStruct
                                                              core/marshal_string.cpp
                                                              core/marshal_alloc.cpp
                                                              delegate_thunks.cpp (reverse P/Invoke)
```

### 1.2 已支持能力

| 能力 | 状态 | 位置 |
|------|------|------|
| DllImport metadata 解析 | 支持 | `LoaderStage.MetadataResolution.cs:137` |
| CallingConvention 映射 | 支持 | WinApi/CDecl/StdCall/ThisCall/FastCall |
| CharSet 映射 | 支持 | Ansi/Unicode/Auto |
| SetLastError | 支持 | codegen ClearOsLastError + GetOsLastError; runtime SetLastPInvokeError |
| blittable 直接调用 | 支持 | codegen static local + reinterpret_cast |
| string marshalling | 支持 | CoTaskMem 分配 UTF8/UTF16 转换 |
| struct marshalling | 支持 | descriptor 驱动 field-by-field |
| SafeHandle 传递 | 支持 | `marshal_api.cpp:MarshalSafeHandleToIntPtr` |
| reverse P/Invoke (calli) | 支持 | `delegate_thunks.cpp` 含 Arity0-4 |
| 返回值 + [out] param | 支持 | codegen 四种路径覆盖 |
| NativeLibrary Load/Free API | 支持 | `native_library.cpp` refcounting |
| DllNotFoundException 处理 | 支持 | codegen emit `raise` + `throw` |

---

## 2. 横向对比

### 2.1 能力矩阵

| 特性 | Chaos IL2CPP | Unity IL2CPP | Mono |
|------|-------------|--------------|------|
| **GC 模式切换** | **缺失** | 自动生成 trampoline | JIT trampoline |
| **__Internal 链接** | **缺失** | `[DllImport("__Internal")]` 原生静态链接 | `[DllImport("__Internal")]` 原生静态链接 |
| **SuppressGCTransition** | **缺失** | `[SuppressGCTransition]` 跳过切换 | `[SuppressGCTransition]` 跳过切换 |
| **SetLastError** | 支持 | 支持 | 支持 |
| **calli (reverse P/Invoke)** | 支持 | 支持 | 支持 |
| **string marshalling** | 支持 | 支持 | 支持 |
| **struct marshalling** | 支持 | 支持 | 支持 |
| **SafeHandle** | 支持 | 支持 | 支持 |
| **blittable 优化路径** | 支持 | 支持 | JIT AOT 支持 |
| **LCID/Charset 自动转换** | 部分 | 支持 | 支持 |
| **varargs** | 不支持 | 有限支持 | 支持 |
| **best-fit mapping** | 不支持 | 支持 | 支持 |
| **callback/delegate 参数** | 支持 | 支持 | 支持 |
| **COM interop** | 不支持 | 有限支持 | 支持 |
| **WinRT interop** | 不支持 | 支持 | 不支持 |
| **Marshal.GetFunctionPointerForDelegate** | 支持 | 支持 | 支持 |

### 2.2 GC 模式切换对比

这是 **最关键的差距**。

**Unity IL2CPP trampoline 伪代码**：
```cpp
// Unity IL2CPP 生成的 P/Invoke wrapper (简化)
void PInvokeWrapper(const void* method) {
    // 1. 获取或初始化函数指针
    // 2. GC 模式切换: cooperative → preemptive
    Il2CppThread* thread = GetCurrentThread();
    int prev_mode = thread->gc_mode;
    thread->gc_mode = PREEMPTIVE;  // 线程不再参与 GC safepoint
    
    // 3. 实际调用
    result = func(args);
    
    // 4. GC 模式恢复: preemptive → cooperative
    thread->gc_mode = prev_mode;
    
    // 5. GC safepoint 检查 (如果 GC 正在等待)
    SafepointCheck();
}
```

**Mono trampoline**：
```c
// Mono JIT 生成的 native wrapper (概念)
MonoMethod * mono_marshal_get_native_wrapper (MonoMethod *method) {
    // 生成 trampoline:
    // 1. Save thread state (LMF 压栈)
    // 2. GC cooperative → preemptive
    // 3. 检查 pinvoke_override
    // 4. 实际调用 (dlsym 或直接地址)
    // 5. GC preemptive → cooperative
    // 6. Restore thread state (LMF 弹栈)
    // 7. 处理异常
}
```

**Chaos IL2CPP 当前生成代码**：
```cpp
// Chaos IL2CPP 当前生成的 P/Invoke wrapper
// ⚠️ 缺少 GC 模式切换
static void* s_lib = nullptr;
static FnPtr s_fn = nullptr;
if (!s_lib) {
    s_lib = NativeLibraryLoad(module_name);
    s_fn = NativeLibraryGetProcAddress(s_lib, entrypoint);
}
// ⚠️ 没有 GC_TRANSITION_TO_PREEMPTIVE()
result = s_fn(args);
// ⚠️ 没有 GC_TRANSITION_TO_COOPERATIVE()
// ⚠️ 没有 SafepointPoll()
```

### 2.3 __Internal 缺失影响

Unity IL2CPP `__Internal` 机制：
```
[DllImport("__Internal")]
static extern int NativeFunction(int x);
```
→ 编译时直接链接符号，不需要 LoadLibrary/GetProcAddress
→ 生成 `extern "C" int NativeFunction(int x);` 直接调用
→ 适用于 embedded native SDK、静态链接库、引擎原生扩展

Mono `__Internal`：相同语义，通过 dlsym(RTLD_DEFAULT, name) 解析

**Chaos IL2CPP 现状**：`__Internal` 被当作普通 DLL 名 → `NativeLibraryLoad("__Internal")` → `LoadLibraryA("__Internal")` 必然失败。

### 2.4 SuppressGCTransition 缺失影响

`[SuppressGCTransition]` 是 .NET 5+ 的优化特性，标记超短 native 调用（如 `GetCurrentProcessorNumber`），跳过 GC 模式切换：

```csharp
[SuppressGCTransition]
[DllImport("kernel32")]
static extern int GetCurrentProcessorNumber();
```

Unity IL2CPP 支持此特性：不加 GC_TRANSITION，零开销直接调用。
Mono 同样支持。

**Chaos IL2CPP 现状**：即使未来加上 GC_TRANSITION，也无法对超短调用跳过。

---

## 3. 问题分级

### P0 — 正确性风险（必须修复）

| # | 问题 | 影响 |
|---|------|------|
| 1 | **P/Invoke codegen 未插入 GC 模式切换** | GC 触发时，线程处于 COOPERATIVE 模式但实际在执行 native 代码 ≥ 100ms，GC 等待所有线程到达 safepoint → 全局 STW 延迟增加。极端情况 GC 认为线程已响应 safepoint 但 native 代码正在访问托管堆 → 内存损坏 |
| 2 | **缺少 __Internal 支持** | 无法通过 `[DllImport("__Internal")]` 调用静态链接库中的函数。游戏引擎嵌入式场景（IL2CPP 的核心用例）完全不可用 |

### P1 — 功能完整性（建议补充）

| # | 问题 | 影响 |
|---|------|------|
| 3 | **缺少 `[SuppressGCTransition]` 支持** | 高频短 native 调用（如原子操作、CPU 信息获取）被迫承受不必要的 GC 切换开销 |
| 4 | **部分 CharSet 行为未实现** | CharSet.Auto 在 Windows 上应为 Unicode，Linux 上应为 Ansi；当前映射逻辑可能不完整 |
| 5 | **无 P/Invoke override 机制** | Mono 的 `PINVOKE_OVERRIDE` 允许宿主拦截 P/Invoke 解析；Unity IL2CPP 有 Intercept 机制。当前无法实现 mock/testing |

### P2 — 工程化提升

| # | 问题 | 影响 |
|---|------|------|
| 6 | **codegen 输出无 GC_TRANSITION benchmark 数据** | 无法量化当前缺失的影响 |
| 7 | **marshalling 错误路径未充分测试** | struct 嵌套、复杂 layout 缺少测试覆盖 |

---

## 4. 差距根因分析

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

### 4.2 历史原因推测

从代码结构推断，GC_TRANSITION 是在 GC 开发后期（BGC + CRAG 阶段）才加入的，而 P/Invoke codegen 管线开发更早。两者没有同步对接。

---

## 5. 整改建议

### 5.1 P0: 补齐 GC_TRANSITION

**EmitPInvokeMethod()** 需在四个 codegen 路径中插入：

```
路径 1 (pure blittable void):
  GC_TRANSITION_TO_PREEMPTIVE()
  s_fn(args)
  GC_TRANSITION_TO_COOPERATIVE()

路径 2 (pure blittable non-void):
  GC_TRANSITION_TO_PREEMPTIVE()
  result = s_fn(args)
  GC_TRANSITION_TO_COOPERATIVE()

路径 3 (non-blittable void):
  GC_TRANSITION_TO_PREEMPTIVE()
  s_fn(args)    // 参数已 marshal
  GC_TRANSITION_TO_COOPERATIVE()
  // post-call marshalling

路径 4 (non-blittable non-void):
  GC_TRANSITION_TO_PREEMPTIVE()
  result = s_fn(args_with_marshal)
  GC_TRANSITION_TO_COOPERATIVE()
  // post-call marshalling + return conversion
```

**工作量估计**：~20 行 codegen C# 代码修改，需修改 `NativeAotLoweringPlanner.MethodEmission.cs`

### 5.2 P0: 补齐 __Internal

**Metadata 层**：`TryDescribeMethodImport()` 识别 `ModuleReference.Name == "__Internal"` → 标记 `IsInternalLink = true`

**Codegen 层**：__Internal 路径跳过 `NativeLibraryLoad`/`NativeLibraryGetProcAddress`，直接：

```cpp
// __Internal 路径: 编译时链接，仅声明
extern "C" int NativeFunction(int x);
// 或使用 decltype 声明函数指针
```

**工作量估计**：metadata 层 ~10 行，codegen 层 ~30 行

### 5.3 P2: 补齐 SuppressGCTransition

新增 `[SuppressGCTransition]` 特性检测（由 managed 层属性 → codegen IR 传递）。标记后跳过 GC_TRANSITION。

### 5.4 验证方案

| 场景 | 验证方式 |
|------|----------|
| GC_TRANSITION 存在性 | 编译后反编译 P/Invoke wrapper 确认 |
| GC safepoint 参与 | 多线程 P/Invoke调用 + BGC触发，确认 STW 时间正常 |
| __Internal 链接 | 静态链接 .obj 后通过 `[DllImport("__Internal")]` 调用 |
| SetLastError 正确性 | P/Invoke 调用 `SetLastError(ERROR_ACCESS_DENIED)` 后 `Marshal.GetLastWin32Error()` 返回正确值 |
| blittable 性能 | 空 P/Invoke 调用微基准，确认零额外开销 |
| string/struct marshalling | 各编码 + 复杂结构体 marshalling 往返一致性 |

---

## 6. 总结

**核心结论**：Chaos IL2CPP P/Invoke 管线在架构上基本完整（metadata → codegen → runtime 三阶段齐备），但存在 **两个 P0 正确性缺口**：

1. **GC 模式切换缺失** — 影响 GC 正确性和 STW 延迟，是当前最严重的功能缺陷
2. **__Internal 不支持** — 堵死了 IL2CPP 核心用例（嵌入式 native SDK 调用）

这两个缺口都有清晰的修复路径：GC_TRANSITION 宏已存在、`__Internal` 的 metadata 识别和 codegen 分支调整均为局部变更。

次要问题（SuppressGCTransition、CharSet 完善、override 机制）属于增量改进，可在 P0 修复后逐步补齐。
