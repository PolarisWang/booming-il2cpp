# P/Invoke (DllImport) 翻译路径

## 概述

P/Invoke 翻译路径将 `[DllImport]` 标记的 managed 方法降级为对 native 函数的直接调用，包含三个环节：metadata 解析 → IR 下降 → C++ codegen。

## 数据流

```
[DllImport("user32.dll", CharSet = CharSet.Unicode)]
static extern int MessageBox(IntPtr hWnd, string text, string caption, uint type);
                                           ↓
[Metadata 层] LoaderStage.MetadataResolution.cs
  TryDescribeMethodImport()
  ├── ModuleName = "user32.dll"
  ├── EntryPointName = "MessageBox" (默认与方法名相同)
  ├── CallingConvention → WinApi → STDCALL
  ├── CharSet → Unicode → MarshalPtrToStringWide
  └── SetLastError → false
                                           ↓
[Artifact 层] AotCoreIrMethodArtifact
  ├── IsPInvoke = true
  ├── ImportModuleName = "user32.dll"
  ├── ImportEntryPointName = "MessageBox"
  ├── IsInternalLink = false
  ├── IsSuppressGCTransition = false
  ├── StringParameterIndices = [1, 2]   (text, caption)
  ├── HasStringParams = true
  ├── HasStringReturn = false
  ├── HasBlittableStructParams = false
  ├── HasSimpleNonBlittableStructParams = false
  ├── HasComplexStructParams = false
  ├── HasSafeHandleParams = false
  ├── HasSetLastError = false
  └── ReturnAbi → carrier_kind = IntPtr
                                           ↓
[Codegen 层] NativeAotLoweringPlanner.MethodEmission.cs
  EmitPInvokeMethod()
  1. 函数声明 → extern "C" 符号
  2. 静态局部变量 → s_pinvoke_lib_ / s_pinvoke_fn_
  3. 延迟加载 → NativeLibraryLoad + GetProcAddress 或 __Internal 直接符号
  4. 参数 Marshalling → string/struct/SafeHandle 预处理
  5. GC 模式切换 → GC_TRANSITION_TO_PREEMPTIVE/COOPERATIVE
  6. Native 调用 → 4 条 codegen 路径
  7. 后处理 → SetLastError、string 释放、struct 回写
```

## Metadata 层

### 文件

`src/managed/Chaos.IL2CPP.Loader/LoaderStage.MetadataResolution.cs` → `TryDescribeMethodImport()`

### 检测规则

| 条件 | 行为 |
|------|------|
| `moduleName == "__Internal"` | 将 `moduleName` 置空字符串 `""`，标记不经过 LoadLibrary |
| `moduleName == "*"` | 保留 `*` 作为保留标记（当前无特殊处理） |
| 存在 `[SuppressGCTransition]` 属性 | `IsSuppressGCTransition = true` |
| 存在 `[DllImport]` 的 CallingConvention | 映射到对应调用约定字符串 |
| 存在 `[DllImport]` 的 CharSet | 选择字符串 marshal 函数（UTF8/UTF16） |
| 存在 `SetLastError = true` | 插入 `ClearOsLastError`/`SetLastPInvokeError` 包装 |

### __Internal 特殊处理

`[DllImport("__Internal")]` 用于调用静态链接库中的符号。Metadata 层识别后：

```csharp
if (moduleName == "__Internal")
{
    moduleName = "";  // 标记为 internal link
    // EntryPointName 保持不变，codegen 层直接 extern "C" 声明
}
```

### SuppressGCTransition 检测

```csharp
// 复用 HasPreserveAttribute 检测模式
if (HasSuppressGCTransitionAttribute(method))
{
    importModel = importModel with { IsSuppressGCTransition = true };
}
```

## IR 下降层

### 文件

`src/managed/Chaos.IL2CPP.CodeGen/AotCoreIrLowering.cs` → `TryCreateMethod()`

### Artifact 字段传递

```csharp
IsInternalLink = method.Import is not null && string.IsNullOrEmpty(method.Import.ModuleName),
IsSuppressGCTransition = method.Import?.IsSuppressGCTransition ?? false,
```

## Codegen 层

### 文件

`src/managed/Chaos.IL2CPP.CodeGen/Emission/NativeAotLoweringPlanner.MethodEmission.cs` → `EmitPInvokeMethod()`

### 函数声明

所有 P/Invoke wrapper 以 `extern "C"` 导出：

```cpp
extern "C" ReturnType MethodSymbol(ParamTypes... params);
```

对 __Internal 额外生成 `extern "C"` 前向声明：

```cpp
extern "C" void EntryPointName();  // 仅在 __Internal
extern "C" ReturnType MethodSymbol(Params...)
{
    // ...
}
```

### 静态局部变量

**普通 P/Invoke**（非 __Internal）：

```cpp
static void* s_pinvoke_lib_ = nullptr;
static FnPtrType s_pinvoke_fn_ = nullptr;
```

**__Internal**：

```cpp
static FnPtrType s_pinvoke_fn_ = reinterpret_cast<FnPtrType>(&EntryPointName);
```

无 `s_pinvoke_lib_`，无 LoadLibrary/GetProcAddress。

### 延迟加载

```cpp
if (!isInternal)
{
    // 仅在非 __Internal 时执行
    if (s_pinvoke_fn_ == nullptr)
    {
        s_pinvoke_lib_ = NativeLibraryLoad(moduleName);
        if (s_pinvoke_lib_ == nullptr) CHAOS_IL2CPP_FAIL();
        s_pinvoke_fn_ = NativeLibraryGetProcAddress(s_pinvoke_lib_, entryPointName);
        if (s_pinvoke_fn_ == nullptr) CHAOS_IL2CPP_FAIL();
    }
}
```

### GC 模式切换

受 `needsGcTransition = !method.IsSuppressGCTransition` 控制：

- `needsGcTransition == true`: 在 native 调用前/后插入 `GC_TRANSITION_TO_PREEMPTIVE()` / `GC_TRANSITION_TO_COOPERATIVE()`
- `needsGcTransition == false`（即 `[SuppressGCTransition]`）：完全跳过 GC 模式切换

### 4 条 Codegen 路径

| 路径 | 条件 | 行为 |
|------|------|------|
| **Path 1** | Pure blittable — 无 string/struct/SafeHandle/SetLastError | 直接调用 + return，GC 宏包裹 |
| **Path 2** | Non-blittable, non-void return | 先 marshal 参数 → 调用 → coop 切换 → 清理 → return |
| **Path 3** | Non-blittable, void return | 同上，无 return |
| **Path 4** | Blittable struct 场景 | 调用 + 捕获返回值 + cleanup（for non-void） |

### Path 1（Pure blittable）生成代码

```cpp
{
    // static locals + 延迟加载 (同上)
    
    if (needsGcTransition) {
        GC_TRANSITION_TO_PREEMPTIVE();
    }
    // void + 无参数:
    s_pinvoke_fn_();
    // void + 有参数:
    s_pinvoke_fn_(args...);
    // 非 void + 无参数:
    return s_pinvoke_fn_();
    // 非 void + 有参数:
    return s_pinvoke_fn_(args...);
    if (needsGcTransition) {
        GC_TRANSITION_TO_COOPERATIVE();
    }
}
```

### Paths 2–4 生成代码

```cpp
{
    // static locals + 延迟加载 + marshalling 预处理 (同上)
    
    if (needsGcTransition) {
        GC_TRANSITION_TO_PREEMPTIVE();
    }
    ReturnType chaos_ret_ = s_pinvoke_fn_(args);  // 或 void 无返回值
    if (needsGcTransition) {
        GC_TRANSITION_TO_COOPERATIVE();
    }
    
    // SetLastError + 后处理 cleanup (string 释放、struct 回写)
    // 非 void: return chaos_ret_;
}
```

## 参数 Marshalling

| 参数类型 | 预处理 | 后处理 |
|---------|--------|--------|
| `string` (Ansi) | `MarshalStringToCoTaskMemUtf8` → `void*` | `MarshalFreeCoTaskMem` |
| `string` (Unicode) | `MarshalStringToCoTaskMemWide` → `void*` | `MarshalFreeCoTaskMem` |
| `SafeHandle` | `MarshalSafeHandleGetHandle` → `IntPtr` | 无（ownership 不变） |
| Blittable struct | `reinterpret_cast` C 结构体指针 | 无 |
| Non-blittable struct (simple) | 逐 field marshal | 逐 field 释放（string field） |
| Non-blittable struct (complex) | `MarshalStructManagedToNative` | `MarshalStructNativeToManaged` + `DestroyMarshalledStruct` |

## 已支持能力

| 特性 | 状态 |
|------|------|
| DllImport metadata 解析 | 支持 |
| CallingConvention (WinApi/CDecl/StdCall/ThisCall/FastCall) | 支持 |
| CharSet (Ansi/Unicode/Auto) | 支持 |
| SetLastError | 支持 |
| Blittable 直接调用 | 支持 |
| String marshalling (UTF8/UTF16) | 支持 |
| Struct marshalling (descriptor-driven) | 支持 |
| SafeHandle 参数传递 | 支持 |
| reverse P/Invoke (`[UnmanagedCallersOnly]`) | 支持 |
| **__Internal 静态链接** | **2026-05-17 新增** |
| **GC_TRANSITION 模式切换** | **2026-05-17 新增** |
| **[SuppressGCTransition] 支持** | **2026-05-17 新增** |
| NativeLibrary Load/Free API | 支持 |
| DllNotFoundException 处理 | 支持 |
| **P/Invoke override (DllImportResolver)** | **2026-05-17 新增** |
| **DllImportResolver managed callback** | **2026-05-17 新增** |
| COM/WinRT interop | 不支持 |
| varargs | 不支持 |
