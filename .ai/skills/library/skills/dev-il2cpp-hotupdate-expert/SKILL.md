---
name: dev-il2cpp-hotupdate-expert
description: dev-il2cpp-hotupdate-expert — il2cpp 热更新专家
---

> ⚠️ **本文件通过 Skill 工具加载，作用是注入领域知识到当前对话上下文。**
> 本文件**不是可执行的 agent**。当前 Agent 需阅读下方"执行流程"作为实现参考。
>
> **当前 Agent 请做**：阅读知识域和约束 → 自行实现代码 → 自行验证
> **当前 Agent 不要做**：加载后等待"Expert 自动执行"——它不会，Skill 只加载文本。
>

# dev-il2cpp-hotupdate-expert — il2cpp 热更新专家

## 领域边界

### 我负责的
- **Hotpatch 双层分派架构** — DispatchEntryV0、mode-aware branching、InterpreterEntryDirect
- **PatchLoader 运行时** — `ApplyPatchFromMemory`、`Unpatch`、`DestroyPatchContext`
- **`.patchdata` 格式** — v2 binary format、PatchDataExtractor（C#）、PatchDataDumper
- **PatchMetadataCache** — 本地 ECMA 表提取、NameIndex 查找、TokenSlot 映射
- **InterpreterEntryDirect** — 懒 IL→IR lowering、ExecutionContext 构建、InterpreterVM::Execute
- **旧版 chaos_hot_update** — method_replacement 模块（兼容路径）
- **emit-patch-data CLI** — 完整闭包管线 + patch 提取
- **Hotpatch 验证** — 7 步验证流程

### 我不负责的（超出以下范围 → 标记 remaining，回 Dispatcher 重新分发）

- **翻译路径设计**（Planner/Emission 的翻译逻辑）→ 超出范围，标记 remaining，原因：需要翻译域知识
- **运行时核心**（runtime-core 非 hotpatch 部分）→ 超出范围，标记 remaining，原因：需要运行时域知识
- **CodeGen / Scriban 模板**（非 hotpatch 部分）→ 超出范围，标记 remaining，原因：需要 CodeGen 域知识
- **GC / 内存域分配** → 超出范围，标记 remaining，原因：需要 GC 域知识
- **编译失败 / codegen stub**（LNK 错误、C++ 编译错）→ 超出范围，标记 remaining，原因：需要构建修复域知识

## 输出格式（Dispatcher 回读用）

每个 Expert 处理完任务后，必须在当前上下文中输出：

```
✅ done: [已处理的子任务 ID 列表]
⏳ remaining: [未处理的子任务 ID 列表 + 原因]
```

---

## 触发条件

- 从 `dev-il2cpp-core-agent` 路由到本 expert
- 用户直接输入 `/dev-il2cpp-hotupdate-expert`
- 用户输入包含：热更新、hotpatch、PatchLoader、patchdata、PatchMetadataCache、InterpreterEntryDirect、emit-patch-data、HotpatchDispatch、method_replacement、HotpatchNameRegistry

---

---REF---

## 知识域

### 三层架构

```
Build-time (Codegen):
  NativeAotLoweringPlanner → emit s_hotpatch_entries[], g_type_index[], g_method_index[], g_token_slot[]
  PatchDataExtractor → extract .patchdata from patch.dll

Patch Load (Runtime):
  ApplyPatchFromMemory(data, size) → validate magic + version → PatchContext → match methods via NameIndex + TokenSlot → update dispatch_entry

Dispatch (Runtime):
  Call site checks flags & kHotpatchActive → InterpreterEntryDirect(method_key, args) → InterpreterVM::Execute
```

### 核心源文件

| 文件 | 说明 |
|------|------|
| `src/native/runtime-core/patch_loader.h/.cpp` | PatchLoader 运行时实现（~900 行） |
| `src/native/runtime-core/hotpatch_table.h/.cpp` | HotpatchNameRegistry, NameIndex 两层级索引 |
| `src/native/runtime-core/chaos/hotpatch_dispatch.h` | DispatchEntryV0 定义, mode-aware 分支 |
| `src/native/runtime-core/runtime_stubs/interpreter_entry_direct.cpp` | InterpreterEntryDirect 入口 |
| `src/native/runtime-core/instantiation/interpreter_dispatch.cpp` | Interpreter dispatch 实现 |
| `src/native/hot-update/hot_update.h/.cpp` | 旧版热更新架构（兼容） |
| `src/native/hot-update/method_replacement.h/.cpp` | 旧版方法替换模块 |
| `src/managed/Chaos.IL2CPP.Generator/PatchDataExtractor.cs` | `.patchdata` C# 序列化 |
| `src/managed/Chaos.IL2CPP.Generator/PatchDataDumper.cs` | `.patchdata` dump/查看 |
| `contracts/native/v0/patch_data.h` | `.patchdata` 格式定义 |

### 关键数据结构

**DispatchEntryV0 (24 bytes on x64)**:
```cpp
struct DispatchEntryV0 {
    void* direct_ptr;       // AOT 函数指针
    void* interrupt_ptr;    // InterpreterEntryDirect
    uint64_t method_key;    // PatchMethod* (patch active) / unused (AOT)
    uint32_t flags;         // bit 0: kHotpatchActive
};
```

**PatchMethod**:
```cpp
struct PatchMethod {
    HeapAllocated* cached_ir;
    DomainAllocated* call_cache;
    void* cached_reg_method;
    void* cached_arg_types;
    uint32_t module_id;
    uint32_t method_token;
    char subject_id_utf8[];
};
```

**PatchMetadataCache**:
```cpp
struct PatchMetadataCache {
    // 提取自 patch.dll 的 ECMA 表
    // GetString(), GetBlob(), GetBody(), GetAotCoreIr(), GetRegisterIr()
    // GetMethodDef(), ResolveTypeDef/TypeRef, FindInliningTarget()
    // 不注册到全局类型系统（iOS 兼容）
};
```

**.patchdata v2 Header**:
```
magic: 0x50415854 ("PADT"), version: 2, header_size: 124
heap: Strings, Blob, UserString
tables: AssemblyRef(8B), TypeRef(16B), TypeDef(22B), FieldDef(18B), MethodDef(28B), MemberRef(16B), StandaloneSig(8B)
sections: BodyData, AotCoreIr (JSON), RegIr (v2+)
```

### NameIndex 两层级索引

```
g_type_index[]: TypeNameIndexEntry { type_name, first_method_index, method_count }
                   → bsearch by type_name
g_method_index[]: MethodNameIndexEntry { method_name, method_token, param_count }
                   → bsearch by method_name within type's range
g_token_slot[]: TokenSlotEntry { token, slot }
                   → bsearch by token → slot index in dispatch_table
```

### 已知故障模式

| 模式 | 症状 | 解决方法 |
|------|------|---------|
| `[[hotupdate-subject-id-resolver-fix]]` | AV on unmapped descriptor | ResolveSubjectId 返回 metadata token 而非 encoded handle |
| `[[emit-patch-data-aot-core-ir-blocker]]` | Subject_N 合成方法 hang | 原始 aot-core-ir.json 复用问题，`.skip-aot-core-ir` bypass |
| `[[tiering-call-count-guard]]` | interpreter dispatch 异常 | InterpreterEntryDirect 必须 fetch_add call_count |
| `[[lcac-complete]]` | LCAC 替代 BridgeAOT | Phase 1-6 已完成 |

---

## 执行流程

### 🚨 第一步：输出分类声明（强制阻断）

**在使用任何工具之前，必须先输出：**

```
classification: domains=[热更新] mode=knowledge-inject expert=dev-il2cpp-hotupdate-expert
```

**不输出分类声明，不得进行任何文件操作。**

---

### Step 0：架构语境加载（Architecture Pre-check）

1. **确定子系统** — build-time（C#） / patch load（C++） / dispatch（C++）？
2. **加载对应架构文档**：
   - 全貌：[`18-热更新架构.md`](../../../wiki/03-功能模块/06-il2cpp核心架构/01-翻译管线/18-热更新架构.md)
   - 决策：[18-HotUpdateDispatchHotpatch-决策.md](../../../wiki/03-功能模块/06-il2cpp核心架构/04-历史决策/18-HotUpdateDispatchHotpatch-决策.md)
   - 翻译表：[`11-热更新HotpatchDispatch翻译表.md`](../../../wiki/03-功能模块/06-il2cpp核心架构/02-翻译路径参考/11-热更新HotpatchDispatch翻译表.md)
3. **检查已知故障模式**（4 种已知模式）
4. **如果涉及 .patchdata 格式** — 检查 v1→v2 兼容性
5. **如果涉及 InterpreterEntryDirect** — 注意 call_count fetch_add 约束

### Step 1：加载热更新语境

1. **确定需要修改的子系统**：
   - Build-time（C# codegen 侧）→ PatchDataExtractor / Planner
   - Patch Load（C++ runtime 侧）→ patch_loader / hotpatch_table
   - Dispatch（C++ runtime 侧）→ interpreter_entry_direct / hotpatch_dispatch

2. **加载对应架构文档**：
   - 全貌：[`18-热更新架构.md`](../../../wiki/03-功能模块/06-il2cpp核心架构/01-翻译管线/18-热更新架构.md)
   - 决策：[18-HotUpdateDispatchHotpatch-决策.md](../../../wiki/03-功能模块/06-il2cpp核心架构/04-历史决策/18-HotUpdateDispatchHotpatch-决策.md)
   - 翻译表：[`11-热更新HotpatchDispatch翻译表.md`](../../../wiki/03-功能模块/06-il2cpp核心架构/02-翻译路径参考/11-热更新HotpatchDispatch翻译表.md)
   - PatchLoader：[`30-PatchLoader运行时参考.md`](../../../wiki/03-功能模块/06-il2cpp核心架构/01-翻译管线/30-PatchLoader运行时参考.md)
   - Patchdata Toolchain：[`05-patchdata工具链.md`](../../../wiki/03-功能模块/06-il2cpp核心架构/05-patchdata工具链.md)
   - 模式切换：[`30-模式间切换指南.md`](../../../wiki/03-功能模块/06-il2cpp核心架构/01-翻译管线/30-模式间切换指南.md)

3. **检查已知故障模式**（对照上面的故障模式表）

### Step 2：实现

1. **PatchLoader 修改**：
   - `ApplyPatchFromMemory`: 更新 magic/version 检查、PatchContext 分配、name lookup
   - `PatchMethodLowerIR`: IL→IR lowering 逻辑
   - `ReapplyInlining`: inlining 重算（注意 call_cache 释放泄漏）
   - `Unpatch`: 清除 kHotpatchActive flags（atomic<uint32_t>）

2. **Dispatch 修改**：
   - `InterpreterEntryDirect`: method_key → PatchMethod* → lazy IL lowering → ExecutionFrame → Execute
   - `EmitHotpatchResolvedInvocation` (Planner侧): mode-aware branch 代码生成
   - ABI: `InterpreterEntryDirect(method_key, arg_buffer, arg_count, ret_buf, ret_buf_size)` → `CHAOS_IL2CPP_INT64`

3. **PatchData 格式修改**：
   - `patch_data.h`: Header 变更需要更新 magic 版本号
   - `PatchDataExtractor.cs`: 保持与 format 定义一致
   - 兼容性：v1→v2 升级需处理 header_size 变化（112→124）

4. **旧版兼容**：
   - `method_replacement::Register/Revert/Resolve` 在 Hotpatch 激活时不应干扰 dispatch_table

### Step 3：验证

1. **Hotpatch 7 步验证**：
   - AOT baseline → ApplyPatch → verify kHotpatchActive → call entry_fn (redirects to Interpreter) → Unpatch → verify restored AOT baseline
2. **性能验证**：
   - Leaf methods: pre-patch ~2ns/op, post-patch ~430-580ns/op
   - SEH methods: 可能更快（C++ try/catch vs native SEH unwinding）
3. **Memory 验证**：
   - PatchContext domain-allocated, Unpatch/DestroyPatchContext 必须完全回收
   - call_cache 释放顺序遵守 constraint（先 free old 再 replace）

## 执行前 Checklist

```
□ 确定子系统：build-time (C#) / patch load (C++) / dispatch (C++)？
□ 加载对应文档：18-热更新架构.md / 30-PatchLoader运行时参考.md / 05-patchdata工具链.md
□ 检查已知故障模式：hotupdate-subject-id-resolver-fix, emit-patch-data-aot-core-ir-blocker
□ 如果涉及 InterpreterEntryDirect → 注意 call_count fetch_add
□ 如果涉及 .patchdata 格式 → 检查 v1→v2 兼容性
```

## 执行后 Checklist

```
□ Hotpatch 7 步验证通过（AOT→Patch→Verify→Call→Unpatch→Verify→Semantic）
□ Leaf methods 性能可接受（pre ~2ns, post ~430-580ns）
□ SEH methods 不会比 AOT 更慢
□ DestroyPatchContext 完全释放（no leaks）
□ 旧版 method_replacement 不受影响
□ memory_order 约束遵守（release write to flags）
```

---

## 关键约束

1. **PatchMetadataCache 不注册全局类型系统** — iOS 兼容性约束
2. **懒 IL→IR lowering** — ApplyPatchFromMemory 不预 lower，首次调用时按需 lower（double-checked locking with ir_ready + ir_lock）
3. **Domain 分配** — PatchContext、PatchMethod、PatchMetadataCache 全部 domain-allocated，unload 时 batch 回收
4. **`kHotpatchKeepNative`** — 复杂方法（array alloc, loops, branches）可以标记为 keep native 避免 interpreter 性能损失
5. **memory_order 正确性** — dispatch_entry 的 flags 操作必须是 atomic，先写 method_key 再写 flags（release ordering）

---

## 输出约束

1. **`.patchdata` 格式变更必须更新 version + 兼容 header_size check**
2. **Unpatch 必须在 PatchMethodLowerIR 失败时回滚所有已设置 dispatch_entry**
3. **InterpreterEntryDirect call_count 必须使用 fetch_add**（不要 memset PatchMethod）
4. **commit message 要求** — 修改完成后必须包含三段式根因：
   - `root_cause` — 一句话根因
   - `fix_strategy` — 修复策略
   - `regression_check` — 验证范围
4. **Hotpatch dispatch table 位于 codegen 生成的静态数组中**（不在 runtime 侧动态创建）

---

## 集成点

| 上游 | 本 skill | 下游 |
|------|----------|------|
| `dev-il2cpp-core-agent` → 热更新路由 | **dev-il2cpp-hotupdate-expert** | `dev-il2cpp-runtime-expert`（运行时协作） |
| 用户直接输入 | | `dev-il2cpp-codegen-expert`（PatchDataExtractor 协作） |
| | | `dev-il2cpp-translation-expert`（interpreter routing 协作） |
| | | `dev-verification-before-completion`（验证门） |
