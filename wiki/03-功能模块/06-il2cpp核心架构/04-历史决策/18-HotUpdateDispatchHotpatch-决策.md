# 18 — HotUpdate Hotpatch 双层分派架构

## 决策

HotUpdate 方法替换采用 **Hotpatch 双层分派架构（原名 D3，Dual-Layer Dispatch）**：Codegen 为每个公开方法生成静态 Entry Table，采用"模式感知分支"策略——无 patch 时走 AOT direct call（零额外开销），有 patch 时走 Invoker → Interpreter 路径。Patch 的 IL 和 Metadata 在构建期完整提取为 `.patchdata` 平面二进制格式，运行时 PatchLoader 加载后通过 PatchMethod 直接指针完成 dispatch 替换。

## 问题

现有 hotupdate 验证测试使用静态 3-TU（genuine/patch/semantic-patch）C++ 预编译方式，patch 方法编译为原生 C++ 代码，与真实热更路径不符：

| 问题 | 影响 |
|------|------|
| Patch 代码通过 C++ 编译而非 Interpreter | 无法验证 Interpreter 执行 IL 的真实路径 |
| Patch 数据在构建期完全可知 | 无法代表"运行时拿到 patch.dll"的真实场景 |
| 调用方式为直接 C 函数调用 | 没有统一的 dispatch 拦截层，iOS 无法热更 |
| 缺少 Metadata 自洽解析 | Patch IL 内部的方法/类型引用无解析路径 |

## 方案对比

### 总体架构

| 维度 | Unity il2cpp + HybridCLR | 原 3-TU 方案 | Hotpatch 双层分派（选中） |
|------|-------------------------|-------------|-------------------|
| Patch 载体 | 托管 DLL，运行时加载 | 预编译 C++，链接时确定 | 托管 DLL，构建期提取为 .patchdata |
| Dispatch 机制 | methodPointer 替换 | 直接函数调用 | 静态 Dispatch Table + 模式分支 |
| Patch 执行 | IL 解释器 | AOT 原生 C++ | Interpreter（IL→IR 降低后执行） |
| Metadata 解析 | 运行时 PE 解析 | 构建期已处理 | 构建期提取，本地 PatchMetadataCache 自洽 |
| iOS 兼容 | ✅ 预编译 trampoline | ✅ 预编译 | ✅ 预编译所有 C++ |

### Dispatch 方式

| 维度 | A 全统一 Buffer | B 模式感知分支（选中） | C 全 invoker 表 |
|------|:--------------:|:-------------------:|:--------------:|
| AOT→AOT 性能 | pack/unpack 开销 | 🟢 零额外 | 多一次跳转 |
| AOT→patch 性能 | 合理 | 🟢 合理 | 合理 |
| 架构简洁度 | 🟢 | 🟢 | 需 per-method stub |
| iOS 兼容 | 🟢 | 🟢 | 🟢 |

## 架构

### 核心组件

```
┌─────────────────────────────────────────────────────────────────┐
│                      Codegen 构建期输出                           │
│                                                                 │
│  native-aot.generated.cpp         patch.patchdata               │
│  ┌─────────────────────────┐     ┌────────────────────────┐     │
│  │ DispatchEntry table     │     │ PatchDataHeader        │     │
│  │   - direct_ptr (AOT)   │     │ PatchTypeEntry[]       │     │
│  │   - interrupt_ptr       │     │ PatchMethodEntry[]     │     │
│  │   - method_key          │     │ String Pool            │     │
│  │   - flags               │     │ IL Pool                │     │
│  │ NameIndex               │     │ Blob Pool              │     │
│  │   - type→method 索引    │     └────────────────────────┘     │
│  │ Token→Slot 反向映射     │                                     │
│  └─────────────────────────┘                                     │
│                                                                  │
│    Bootstrap 集成注册 → NameIndexRegistry                        │
└─────────────────────────────────────────────────────────────────┘

┌─────────────────────────────────────────────────────────────────┐
│                      运行时 Patch 加载                            │
│                                                                 │
│  Path: ApplyPatch(patch.patchdata)                              │
│   1. PatchLoader 加载 .patchdata → PatchMetadataCache           │
│   2. 遍历 patch 方法:                                            │
│      a. (type, method) → NameIndexRegistry 查找 → aot_token     │
│      b. token→slot 反向索引 → dispatch slot                     │
│      c. 创建 PatchMethod 对象 (IL + 签名 + token)              │
│      d. 构建 PatchMetadataCache (本地自洽, 不注册全局类型系统)    │
│      e. 填入 dispatch table                                     │
│        interrupt_ptr = &InterpreterEntryDirect                   │
│        method_key = (uintptr_t)PatchMethod                      │
│        flags |= kPatched                                        │
└─────────────────────────────────────────────────────────────────┘

┌─────────────────────────────────────────────────────────────────┐
│                      Dispatch 运行时路径                          │
│                                                                 │
│   调用方 AOT 方法:                                               │
│    if (entry.flags & kPatched) [[unlikely]]                     │
│        → InterpreterEntryDirect(method_key, ArgBuffer)          │
│            → PatchMethod* → Lazy Lower IR to IRMethod           │
│            → 构建 ExecutionFrame (运行时签名解析)                │
│            → InterpreterVM::Execute(IRMethod, &frame)           │
│    else                                                         │
│        → direct_ptr(args...)  // AOT direct call, 零额外        │
└─────────────────────────────────────────────────────────────────┘
```

### DispatchEntry

```cpp
struct DispatchEntry {
    void*       direct_ptr;       // AOT 函数指针
    void*       interrupt_ptr;    // = &InterpreterEntryDirect（patch 时替换）
    uintptr_t   method_key;       // = PatchMethod*（未 patch 时为 0）
    uint32_t    flags;            // bit 0: kPatched
};

extern "C" DispatchEntry g_dispatch_table[];
```

### PatchMethod

```cpp
struct PatchMethod {
    uint32_t                aot_token;      // 匹配到的 AOT token
    uint32_t                class_token;    // 所属类型 token
    const uint8_t*          il_bytes;       // 原始 IL 字节
    uint32_t                il_length;
    uint8_t                 param_count;    // 参数数量
    std::vector<uint8_t>    param_types;    // 参数 ELEMENT_TYPE 列表
    uint8_t                 return_type;    // 返回值 ELEMENT_TYPE
    IRMethod*               cached_ir;      // 首次执行时降低
    std::atomic<bool>       ir_ready;
    std::mutex              ir_lock;
    PatchMetadataCache*     metadata_cache; // 本地自洽 metadata 上下文
};
```

### PatchMetadataCache

构建期从 patch.dll 完整提取以下 Metadata table，打包为 FlatBuffer 格式 `.patchdata`：

- `TypeDef` (0x02) — type_name + method list
- `MethodDef` (0x06) — method_name + RVA + param_count + blob sig index
- `TypeRef` (0x01) — 外部类型引用
- `MemberRef` (0x0a) — 外部成员引用
- `AssemblyRef` (0x23) — 引用的程序集
- `#Strings` stream — 所有字符串
- `#Blob` stream — 所有签名 blob

**不注册到全局类型系统**（iOS 兼容）。只在 IL→IR 降低时作为本地 token resolver 使用，patch 方法"借用"原始 AOT 方法的类型身份执行。

### NameIndex（两级索引）

```cpp
// 构建期 codegen 生成，Bootstrap 时注册到 NameIndexRegistry
extern "C" struct {
    const char*     type_name;
    uint32_t        first_method_index;
    uint16_t        method_count;
} g_type_index[];

extern "C" struct {
    const char*     method_name;
    uint32_t        method_token;
    uint16_t        param_count;
} g_method_index[];
```

### 测试验证（改造 11 步流程为统一 Harness）

```cpp
struct MethodTestCase {
    uint32_t slot;
    int32_t (*entry_fn)();
};

for (auto& tc : kTestTable) {
    int32_t baseline = tc.entry_fn();          // 1. AOT baseline
    assert(g_dispatch_table[tc.slot].flags & kPatched); // 2. patch 已注册
    int32_t patched = tc.entry_fn();            // 3. interpreter patch
    assert(patched != baseline);                // 4. 值变了
    method_replacement::Revert(token);           // 5. revert
    g_dispatch_table[tc.slot].flags &= ~kPatched;
    int32_t restored = tc.entry_fn();           // 6. 恢复
    assert(restored == baseline);                // 7. 恢复正确
}
```

## 关键决策清单

| # | 决策项 | 结论 |
|:-|-------|------|
| 1 | 方法匹配 | 运行时 Token 重映射（名称匹配 + NameIndex） |
| 2 | Dispatch 机制 | Hotpatch 双层 ABI：dispatch table + direct call |
| 3 | Dispatch Table | Per-module 平面静态表，构建期确定 |
| 4 | Call site 模式 | 模式感知分支（is_patched check） |
| 5 | PE 解析 | 构建期完整 Metadata 提取，PatchMetadataCache 本地自洽 |
| 6 | 数据格式 | FlatBuffer 风格 .patchdata（含完整的 Metadata table） |
| 7 | 提取工具 | 集成到 codegen C# 管道（patch variant 改为 emit .patchdata） |
| 8 | Metadata 注册 | 本地自洽 cache，不注册全局类型系统（iOS 兼容） |
| 9 | Name Index | 两级索引（Type → Method），Bootstrap 集成 |
| 10 | Token → Slot | 构建期 emit token→slot 反向索引 |
| 11 | IL→IR 降低 | Lazy（首次调用降低） |
| 12 | Token 解析 | PatchMetadataCache 自洽（持有引用闭包） |
| 13 | 参数解析 | 运行时签名解析（从 Blob 读 ELEMENT_TYPE） |
| 14 | 卸载 | 逐方法恢复，第一阶段无 RCU |
| 15 | Interpreter entry | 直接指针 PatchMethod\* |
| 16 | 测试框架 | 改造 11 步流程 → 统一 Harness |
| 17 | CallViaDispatch | 统一 Test Harness |
| 18 | 阶段 | 先打通全链路（string-char-text-core） |

## 文件范围

| 范畴 | 文件 | 变更类型 |
|------|------|---------|
| Codegen emit | `src/managed/Chaos.IL2CPP.Generator/Emission/*.cs` | 新增 emit-patch-data 模式 |
| Codegen pipeline | `batch_native_aot_runner.py` / `batch_hotupdate_runner.py` | 修改 patch variant 流程 |
| Runtime | `src/native/runtime-core/patch_loader.h/cpp` | 新增 — PatchLoader |
| Runtime | `src/native/runtime-core/patch_metadata_cache.h/cpp` | 新增 — 本地 Metadata cache |
| Runtime | `src/native/runtime-core/dispatch_table.h/cpp` | 新增 — DispatchTable + NameIndexRegistry |
| Runtime | `src/native/runtime-core/interpreter_entry.h/cpp` | 新增 — InterpreterEntryDirect |
| Test | `generate_hotupdate_test.py` | 改 — 不再生成 patch TU，改为嵌入 .patchdata |
| Test | `foundation-dll/*family*/il2cpp_dist/hotupdate/HotUpdateTest.cpp` | 改 — 统一 Harness |

## HybridCLR 对比

| 维度 | HybridCLR | Hotpatch 方案 | 备注 |
|------|-----------|---------|------|
| 运行时 PE 解析 | 完整 PE Loader | 构建期预处理 | 我们复用构建期 codegen pipeline |
| Metadata 注册 | 注册到全局 Il2CppMetadata | 本地 PatchMetadataCache，不注册全局 | iOS 兼容决策 |
| Dispatch 替换 | 改 methodPointer | 改 dispatch table entry | 等价 |
| Interpreter 入口 | 运行时生成 trampoline | 预编译 InterpreterEntryDirect | iOS 安全 |
| 参数 marshalling | 签名感知，运行时解析 | 签名感知，运行时解析（从 Blob 读） | 等价 |
| IL→IR | 不适用（直接解释 IL） | Lazy 降低（复用 il_to_ir_lowerer） | 架构差异 |

## 实现状态（2026-05-05）

### 已完成并验证

| 组件 | 状态 | 说明 |
|------|------|------|
| PatchDataExtractor (C#) | ✅ | PE→.patchdata 提取，含 ECMA #Strings/#Blob/MethodBody |
| NativeAotLoweringPlanner emit | ✅ | NameIndex 两级索引 + Token→Slot 反向表 + DispatchTable emit |
| NameIndexRegistry | ✅ | 两级 bsearch 查找（type→method→AOT token），Bootstrap 集成 |
| DispatchTable + Token→Slot | ✅ | Per-module 静态表，token→slot 反向 bsearch |
| PatchMetadataCache | ✅ | 本地自洽 token resolver，不注册全局类型系统 |
| ApplyPatchFromMemory | ✅ | .patchdata 验证 → NameIndex 查找 → DispatchEntry 标记 |
| InterpreterEntryDirect | ✅ | ArgBuffer + 签名解析 + Lazy IL→IR 降低 + ExecutionFrame 构建 + InterpreterVM 调用 |
| Unpatch | ✅ | 逐方法恢复 direct_ptr + 清 flag + 销毁 PatchContext |
| PatchMethod metadata_cache | ✅ | ApplyPatchFromMemory 存储 PatchMetadataCache 指针，PatchTokenResolver 使用 |

### 验证结果（string-char-text-core）

HotUpdateTest.exe 对 20 个方法全部通过 7 步 Hotpatch 验证：
- `d3PatchApplied: true`（ApplyPatchFromMemory 正确标记 dispatch entry）
- `RuntimeDispatchLookupBySlot` 返回 dispatch entry 且 `flags & kDispatchPatched`
- 所有 20 方法 `status: "passed"`, `revertVerified: true`
- Baseline→Patch→Revert→Restore 全链路通过

### 修复的 Bug

| Bug | 层 | 根因 | 修复 |
|-----|-----|------|------|
| 方法名乱码 | Managed (PatchDataExtractor) | `Dictionary.Last()` 不保证插入顺序 | 添加 `_insertOrder` 列表跟踪插入顺序 |
| 方法名/Blob 乱码 | Native (PatchMetadataCache) | `GetString()`/`GetBlob()` 未加 heap_base_offset | 添加 `+ header_->string_heap_offset` |
| Method 0 被跳过 | Native (patch_loader.cpp) | `body_offset == 0` 误判为"无 body" | 改为只检查 `body_size == 0` |

### 待完成

| 组件 | 优先级 | 说明 |
|------|--------|------|
| 全 family .patchdata 生成 | 中 | 当前仅验证 string-char-text-core |
| codegen Hotpatch call site 发射 | 低 | 模式感知分支：`if (entry.flags & kPatched) → InterpreterEntryDirect` |
| InterpreterEntry 单元测试 | 低 | 需要完整的 InterpreterVM 环境 |
| 跨 family 全量验证 | 中 | 当前只验证了 string-char-text-core |
| 增量 Diff 构建 | 未来 | 当前是全量重新生成 .patchdata |
