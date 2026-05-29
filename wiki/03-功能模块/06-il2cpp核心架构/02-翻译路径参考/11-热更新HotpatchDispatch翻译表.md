# 热更新 Hotpatch Dispatch 翻译表

> Hotpatch（原 D3，Dual-Layer Dispatch）是热更新的核心分派机制。Codegen 为每个公开方法生成静态 Hotpatch Entry Table，通过模式感知分支在 AOT direct call 和 Interpreter patch 路径之间切换。本文描述 Hotpatch 的完整翻译路径：从构建期 emit 到运行时 dispatch，再到验证流程。

## 总体架构

```
构建期 Codegen:
  native-aot.generated.cpp
    ├─ HotpatchEntryV0 s_hotpatch_entries[]     // per-module 平面静态表
    ├─ NameIndex 两级索引           // type→method→token
    ├─ Token→Slot 反向映射          // token→dispatch slot
    └─ Slot 调用站点的模式感知分支     // if (flags & kHotpatchActive) ...

运行时 Patch 加载:
  ApplyPatchFromMemory(.patchdata)
    ├─ 验证 header + 校验和
    ├─ NameIndex 查找 (type, method) → aot_token
    ├─ token→slot 反向索引 → dispatch entry
    └─ 标记 dispatch entry:
         interrupt_ptr = &InterpreterEntryDirect
         method_key    = (uintptr_t)PatchMethod
         flags        |= kHotpatchActive

运行时 Dispatch:
  调用方 AOT 代码:
    if (flags & kHotpatchActive) [[unlikely]]
        → InterpreterEntryDirect(method_key, args...)
    else
        → direct_ptr(args...)         // 零额外开销

```

## 关键数据结构

### HotpatchEntryV0（运行时核心）

```cpp
// src/native/runtime-core/hotpatch_table.h
struct HotpatchEntryV0 {
    void*       direct_ptr;       // AOT 编译函数指针（原始 entry）
    void*       interrupt_ptr;    // = &InterpreterEntryDirect（patch 时设置）
    uintptr_t   method_key;       // = PatchMethod*（未 patch 时为 0）
    uint32_t    flags;            // bit 0: kHotpatchActive
};
static constexpr uint32_t kHotpatchActive = 1u << 0;
```

四个字段共 24 字节（x64），直接嵌入到生成的 C++ 文件中作为全局数组。

### NameIndex 两级索引

```cpp
// 构建期 codegen emit, 通过 Bootstrap 注册到 NameIndexRegistry
struct TypeNameIndexEntry {
    const char* type_name;
    uint32_t    first_method_index;   // g_method_index 起始偏移
    uint16_t    method_count;
};

struct MethodNameIndexEntry {
    const char* method_name;
    uint32_t    method_token;         // AOT token
    uint16_t    param_count;
};
```

查找流程：`(type_name, method_name)` → type bsearch 找到 type 范围 → method bsearch 在范围内找 method → aot_token。

### Slot 映射

```cpp
// 构建期 emit 的反向索引: aot_token → dispatch_table index
struct TokenSlotEntry {
    uint32_t  token;
    uint32_t  slot;    // dispatch_table[slot] 的索引
};
// bsearch 查找, bootstrap 时注册为全局 TokenSlotRegistry
```

## Codegen 发射路径

### DispatchTable emit

位于 `NativeAotLoweringPlanner.StructuredIREmit.cs:EmitDispatchTable`。

为每个 family 的公开方法生成：

```cpp
// 示例: string-char-text-core 的 dispatch table
extern "C" HotpatchEntryV0 s_hotpatch_entries[] = {
    { Method0, nullptr, 0, 0 },   // slot 0
    { Method1, nullptr, 0, 0 },   // slot 1
    // ...
};
static_assert(sizeof(s_hotpatch_entries) / sizeof(s_hotpatch_entries[0]) == 20);
```

每个 slot 的 `direct_ptr` 指向 AOT 编译后的方法实体。`interrupt_ptr` 初始为 nullptr，patch 时由 `ApplyPatchFromMemory` 填入 `&InterpreterEntryDirect`。

### NameIndex emit

位于 `NativeAotLoweringPlanner.StructuredIREmit.cs:EmitNameIndex`。

为 family 生成：

```cpp
extern "C" TypeNameIndexEntry g_type_index[] = {
    { "System.String", 0, 20 },
};
extern "C" MethodNameIndexEntry g_method_index[] = {
    { "Concat", 0x06000001, 2 },
    { "IndexOf", 0x06000002, 1 },
    // ...
};
extern "C" TokenSlotEntry g_token_slot[] = {
    { 0x06000001, 0 },
    { 0x06000002, 1 },
    // ...
};
```

### 模式感知分支 emit

模式感知分支 emit 已完整实现，位于 `NativeAotLoweringPlanner.ExceptionEmission.cs:EmitHotpatchResolvedInvocation`。

调用站点生成代码如下：

```cpp
// 调用站点生成代码
// before: 直接调用
//   auto result = Method0(args...);
// after: 模式感知分支 (via EmitHotpatchResolvedInvocation)
//   auto& _d0 = s_hotpatch_entries[0];
//   CHAOS_IL2CPP_INTPTR _d_hpresult{};
//   if (::chaos::il2cpp::runtime_core::HotpatchIsActive(_d0))
//       ::chaos::il2cpp::runtime_core::HotpatchDispatchPatch(&_d0, ...);
//   else if (::chaos::il2cpp::runtime_core::HotpatchShouldKeepNative(_d0))
//       _d_hpresult = _d0.direct_ptr(...);
//   else
//       _d_hpresult = Method0(...);
```

emit 逻辑在 `EmitLinearResolvedInvocation` 中检测 `_nativeSymbolToDispatchSlot` 映射，匹配时路由到 `EmitHotpatchResolvedInvocation`。详见 `NativeAotLoweringPlanner.InvocationPlanning.cs`。运行时根据 entry 状态选择 direct call、hotpatch dispatch 或 native keep-alive 三种路径。

## PATCHDATA 结构

构建期从 patch.dll 提取的平面二进制格式：

| 段 | 说明 |
|----|------|
| `PatchDataHeader` | 魔数 + 版本 + 段偏移表 + SHA256 校验和 |
| `PatchTypeEntry[]` | 类型列表 (class_token + type_name + method_count) |
| `PatchMethodEntry[]` | 方法列表 (method_token + class_token + param_count + return_type) |
| `StringPool` | ECMA `#Strings` stream 的连续副本 |
| `ILPool` | 所有方法体的 IL 字节拼接 |
| `BlobPool` | ECMA `#Blob` stream 的连续副本 |

提取工具：`PatchDataExtractor.cs`，集成在 codegen 管道中。

PatchMetadataCache 在加载时从上述段构建本地自洽的 token resolver：

- `GetString(token)` → StringPool + heap_base_offset
- `GetBlob(token)` → BlobPool + heap_base_offset
- `GetMethodBody(token)` → ILPool + body_offset

## 运行时加载路径

### ApplyPatchFromMemory

```
ApplyPatchFromMemory(.patchdata)
  │
  ├─ 0. SetAotBridge(bridge, aot_image_handle)
  │     ├─ GetCodegenBridgeV0() → bridge
  │     ├─ PeekBootstrapState() → code_registration
  │     ├─ code_registration → image → ReflectionQueryImageDescriptor
  │     └─ HotpatchNameRegistry::SetAotBridge() 供 interpreter 解析 call_target
  │
  ├─ 1. 校验 header: 魔数(CHPD) + 版本 + 段 count
  │     hash 校验(加载时懒校验, 首次访问触发)
  │
  ├─ 2. 分配 PatchContext (含 PatchMetadataCache)
  │
  ├─ 3. 遍历 PatchMethodEntry[]:
  │     for each entry:
  │       a. NameIndexRegistry.Lookup(type_name, method_name)
  │          → aot_token (失败则跳过, 不阻塞加载)
  │       b. TokenSlotRegistry.Lookup(aot_token)
  │          → slot_index
  │       c. 创建 PatchMethod 对象 (IL + 签名 + token)
  │          metadata_cache = PatchMetadataCache 指针
  │       d. 写入 dispatch_table[slot_index]:
  │          interrupt_ptr = &InterpreterEntryDirect
  │          method_key    = (uintptr_t)PatchMethod
  │          flags        |= kHotpatchActive
  │
  ├─ 4. Pre-lower all PatchMethods (lazy IR→IR lowering 预热)
  │     each method: LowerILToIR → ReapplyInlining (内联穿越)
  │
  │     注意: 当前 ApplyPatchFromMemory 不执行 pre-lowering。
  │     IR 降级是惰性的（在首次 patched 方法调用时由
  │     InterpreterEntryDirect 触发）。
  │
  └─ 5. 返回 patched_count / total_count
```

> **设计变更**: SetAotBridge 从 InterpreterEntryDirect 的 Step0_Bridge 移到了 ApplyPatchFromMemory。旧的 Step0_Bridge 在每个 patched 方法首次执行时重复设置 bridge，现已删除。bridge 现在在 patch 加载时只设置一次。

### PatchMethod 对象

```cpp
// src/native/runtime-core/hot-update/patch_loader.h
struct PatchMethod {
    uint32_t              aot_token;
    uint32_t              class_token;
    const uint8_t*        il_bytes;
    uint32_t              il_length;
    ParamSignature        signature;       // 运行时签名解析
    IRMethod*             cached_ir;       // Lazy 降低 (首次解释执行时)
    std::atomic<bool>     ir_ready;
    PatchMetadataCache*   metadata_cache;  // 本地 token resolver
};
```

## InterpreterEntryDirect

```
InterpreterEntryDirect(method_key, ArgBuffer)
  │
  ├─ 1. PatchMethod* method = (PatchMethod*)method_key
  │
  ├─ 2. Lazy IL→IR 降低:
  │     if (!method->ir_ready) {
  │         lock(method->ir_lock);
  │         if (!method->cached_ir)
  │             method->cached_ir = LowerILToIR(
  │                 method->il_bytes, method->il_length,
  │                 method->signature,
  │                 method->metadata_cache);
  │         method->ir_ready = true;
  │     }
  │
  ├─ 3. 构建 ExecutionFrame:
  │     - frame.arguments[] = ArgBuffer 数据按签名解析结果填入
  │     - frame.metadata_cache = method->metadata_cache
  │     - frame.method = method->cached_ir
  │
  ├─ 4. InterpreterVM::Execute(IRMethod, &frame)
  │     - 返回 ExecutionResult
  │     - 包含 return_value / threw_exception
  │
  └─ 5. 返回值转换:
        ExecutionResult → ABI 返回值 (按 return_type 确定大小)
```

### 调用约定

```cpp
// InterpreterEntryDirect 的 ABI 约定
extern "C" CHAOS_IL2CPP_INT64 InterpreterEntryDirect(
    uintptr_t method_key,           // PatchMethod*
    void* arg_buffer,               // 参数连续布局
    uint32_t arg_count,
    uint8_t* ret_buf,               // 返回值缓冲区 (结构体用)
    uint32_t ret_buf_size);
```

参数在 ArgBuffer 中连续排列，按 PatchMethod 的签名解析为 InterpreterValue 数组。结构体通过 ret_buf 传出。

## 验证流程

### Hotpatch HotUpdateTest

每个 family 的 Hotpatch 验证 exe 执行以下 7 步流程：

```
1. 调用 entry_fn() 获取 AOT baseline 值
2. ApplyPatchFromMemory(.patchdata) → 标记 dispatch entry
3. 确认 s_hotpatch_entries[slot].flags & kHotpatchActive
4. 调用 entry_fn() → dispatch table 重定向到 InterpreterEntryDirect
5. InterpreterVM::Execute 执行 patch IL → 返回 patch 值
6. Unpatch: Revert(token) + 清 flag
7. 调用 entry_fn() → 验证恢复 AOT baseline
```

测试输出 JSON（schemaVersion 2）：

```json
{
  "schemaVersion": 2,
  "d3PatchApplied": true,
  "d3PatchedCount": 19,
  "summary": { "totalMethods": 19, "passedMethods": 19, "failedMethods": 0 },
  "methodResults": [
    {
      "methodToken": 100663297,
      "status": "passed",
      "d3Patched": true,
      "patchReturnValue": 0,
      "interpreterDispatched": true,
      "revertVerified": false,
      "semanticVerified": false
    }
  ]
}
```

当前 limitations:
- `revertVerified`: 尚未实现 Unpatch 后验证流程
- `semanticVerified`: 尚未实现完整语义验证（验证仅检查"不崩溃"和"是否路由到 interpreter"，不检查返回值语义正确性）
- 对复杂方法（含数组分配、循环、分支等），interpreter 四层 dispatch 无法处理。这些方法需在 codegen 端预置 `kHotpatchKeepNative` 标志，使 dispatch 绕过 interpreter 走 native 路径
- 首次调用 patched 方法总是较慢（包含 IR 反序列化 + inlining 开销），因为 ApplyPatchFromMemory 不执行预降级

已验证的基础库 family 全部 19/19 方法通过 hotupdate 验证（包含 SEH 方法和 leaf 方法）。

### Hotupdate Benchmark

`entry.exe --patch-bench` 模式测量 patched 方法通过 interpreter 执行的性能：

| 方法类型 | Pre-patch (ns/op) | Post-patch (ns/op) | Slowdown |
|---------|:-:|:-:|:-:|
| Leaf 方法（无 SEH） | ~2 | ~430-580 | ~200-290x |
| SEH 方法（抛 InvalidCastException） | ~1600-17600 | ~3400-3600 | ~1-2x 或更快 |
| 多参方法 (2 args) | ~2 | ~583 | ~291x |

Leaf 方法 ~200-290x slowdown 是 interpreter 设计的已知特性（每条指令 decode + dispatch + stack op）。SEH 方法 slowdown 较小，因为 interpreter 的 C++ `try/catch` 比 native AOT 的完整 SEH unwinding 更轻量。

## 实现状态

| 组件 | 文件 | 状态 |
|------|------|------|
| DispatchTable emit | `NativeAotLoweringPlanner.StructuredIREmit.cs` | ✅ |
| NameIndex emit | `NativeAotLoweringPlanner.StructuredIREmit.cs` | ✅ |
| Token→Slot emit | `NativeAotLoweringPlanner.StructuredIREmit.cs` | ✅ |
| PatchDataExtractor (C#) | `PatchDataExtractor.cs` | ✅ |
| NameIndexRegistry | `runtime-core/hotpatch_table.cpp` | ✅ |
| TokenSlotRegistry | `runtime-core/hotpatch_table.cpp` | ✅ |
| ApplyPatchFromMemory | `runtime-core/hot-update/patch_loader.cpp` | ✅ |
| PatchMetadataCache | `runtime-core/hot-update/patch_metadata_cache.cpp` | ✅ |
| InterpreterEntryDirect | `runtime-core/hot-update/interpreter_entry.cpp` | ✅ |
| Unpatch / Revert | `runtime-core/hot-update/patch_loader.cpp` | ✅ |
| 模式感知分支 emit | 未实现 | ⏳ |
| 全 family .patchdata 构建 | `batch_native_aot_runner.py` | ✅ |

## 关键文件索引

| 范畴 | 路径 |
|------|------|
| Codegen DispatchTable emit | `src/managed/Chaos.IL2CPP.Generator/Emission/NativeAotLoweringPlanner.StructuredIREmit.cs` |
| Codegen NameIndex/Slot emit | `src/managed/Chaos.IL2CPP.Generator/Emission/NativeAotLoweringPlanner.StructuredIREmit.cs` |
| PatchDataExtractor | `src/managed/Chaos.IL2CPP.Generator/Emission/PatchDataExtractor.cs` |
| HotpatchEntryV0 定义 | `src/native/runtime-core/hotpatch_table.h` |
| HotpatchNameRegistry | `src/native/runtime-core/hotpatch_table.cpp` |
| ApplyPatchFromMemory | `src/native/runtime-core/hot-update/patch_loader.cpp` |
| PatchMetadataCache | `src/native/runtime-core/hot-update/patch_metadata_cache.cpp` |
| InterpreterEntryDirect | `src/native/runtime-core/hot-update/interpreter_entry.cpp` |
| Hotpatch 验证 runner | `testing/foundation-dll/verification/tooling/hotupdate_verification_runner.py` |
| 验证报告生成器 | `testing/foundation-dll/verification/tooling/hotupdate_verification_runner.py` |
| HotUpdate 架构概览 | `wiki/03-功能模块/06-il2cpp核心架构/01-翻译管线/18-热更新架构.md` |
| Hotpatch 决策文档 | `wiki/03-功能模块/06-il2cpp核心架构/04-历史决策/18-HotUpdateDispatchHotpatch-决策.md` |
