# Task6 方向A 实施细节 — InternalCall 编译期符号解析

> **日期**：2026-08-12
> **类型**：实施计划（含机制、具体条目、ABI 验证要求、回归风险）
> **前置**：根因已确认（callee 落 `ChaosExternalRuntimeFallback` → 解释器），见 `dispatch-intermediate-layer-research.md`。
> **关联**：commit `6d64ef994`（调研）、`NativeAotLoweringPlanner.ExternalRuntimeHelpers.cs`。

---

## 1. 机制（方向 A = 把 InternalCall 从"catch-all fallback"挪到"SimpleForward native 调用"）

`TryCreateExternalRuntimeHelperDefinition(callee)`（`ExternalRuntimeHelpers.cs:233`）的决策树：

```
① _methodsBySubjectId + same-module → 直接调用                    :278
② ShapeRegistry SimpleForward shape → return {NativeFnSymbol}(args)  :291-297 ← 目标
③ Custom attribute runtime helper :299
④ catch-all → return ChaosExternalRuntimeFallback(sid)  → 解释器  :304-346  ← 现状(慢)
```

**方向 A 的做法**：把热 InternalCall 在 `RuntimeHelperShapeRegistry.CoreStubs.*.cs` 里注册成 `ShapeKind.SimpleForward`，指向其真实 native 符号 → 自动走 ②（直接 native 调用），绕开 ④（解释器 2.5-30×）。

**已有先例**（证明机制成熟、ABI 模型可用）：
- `RuntimeHelpers:InitializeArray` → `chaos_initialize_array_from_field_data_int32`（S10.cs:156）
- `RuntimeHelpers:GetUninitializedObject` → `ChaosRuntimeHelpersGetUninitializedObject`（S10.cs:166）
- `GC:Collect(int,GCCollectionMode)` → 已注册（S1.cs:538）

---

## 2. 明确缺失、高价值、ABI 可核对的候选（第一优先级）

以下 InternalCall **native stub 已存在**（`misc_stubs.h`），但 **ShapeRegistry 尚未注册** → 当前落 catch-all 解释器（慢）。补齐是低风险、立竿见影的方向 A 首批：

| Managed InternalCall | native 符号（misc_stubs.h 已存在） | ABI（native） |
|---|---|---|
| `System.Buffer:Memmove`（`Buffer::Memmove`）| `ChaosBufferMemmove`（:53）| `(INTPTR dest, INTPTR src, SIZE count)` |
| `System.Buffer:BlockCopy` | `ChaosBufferBlockCopy`（:65）| `(INTPTR src, INT32 src_ofs, INTPTR dst, INT32 dst_ofs, INT32 count)` |
| `System.Buffer:MemoryCopy` | `ChaosBufferMemoryCopy`（:59）| `(INTPTR source, INTPTR dest, INT64 dest_sz, INT64 src_bytes)` |

> 注：`GC:Collect` 已覆盖；`Buffer::Memmove` 由 `IsInternalCallSubjectId` 明确标记（InvocationPlanning.cs:381），证明它确实走 bridge-thunk/表路径，补注册有效。

---

## 3. 实施步骤（每步含 ABI 验证闸）

### Step 1 — 建内部调用注册表（对齐 ABI）
在 `RuntimeHelperShapeRegistry.CoreStubs.Part2`（或新建 Part3）加注册函数，逐一 `registry.Register`：

```csharp
// Buffer.Memmove — BufferInternal.cs, static void Memmove(ref byte dest, ref byte src, nuint len)
registry.Register("System.Buffer", "Memmove",
    ["System.Byte&", "System.Byte&", "System.UIntPtr"],       // 参数 shape
    ShapeKind.SimpleForward, "ChaosBufferMemmove",
    new AotCoreIrAbiSlotArtifact[] {
        CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ByRef),  // dest ref
        CreateNativeIntAbiSlot(null, AotCoreIrTypeShapeKind.ByRef),  // src ref
        CreateNativeIntAbiSlot("System.UIntPtr", AotCoreIrTypeShapeKind.ValueType), // len
    },
    CreateVoidAbiSlot(),                                       // void 返回
    new HashSet<int> { 0, 1 });                                // 引用参数索引(raw indices)
```

**关键 ABI 核对（实施时必须逐条验证，失配=内存损坏，比慢更糟）**：
- `dest`/`src` 是 `ref byte` → 传**指针**（native `INTPTR`接收），ABI slot 需 `ByRef`/`ReferenceType`，且 0/1 是指针传递。
- `len` 是 `nuint`如再 `UIntPtr` → native `SIZE`，需 ValueType slot。
- 返回 void。

### Step 2 — 核对 native stub 的调用约定与 GC 语义
- `ChaosBufferMemmove` 声明 `noexcept`、纯内存拷贝，**无托管引用读取** → 无需 GC cooperative 过渡（不像 `RuntimeHelpers::InitializeArray` 需 `RequiresGcTransition`）。
- 对照 `CreateDefinitionFromShapeEntry`（:350-368）已自动生成 `return {NativeFnSymbol}(args)`，且 `DirectNativeSymbol` 已设 → call site 直调。**不需要额外改 emission**。

### Step 3 — 校验 ShapeRegistry 匹配
- `TryMatchShape(callee)` 需按 SubjectId 命中新注册。确认 NormalizeSubjectIdAssembly 后 `System.Private.CoreLib/System.Buffer::Memmove` → `("System.Buffer","Memmove")` 能匹配 `Register` 的 type/method。
- 若 corelib 归一化把 `System.Buffer` 改成别的 namespace，需在 `Register` 用匹配实际 SubjectId 的 type 名。

### Step 4 — 验证
- **codegen 单测**：构造含 `Buffer.Memmove` 的方法，断言生成的 call site 是 `ChaosBufferMemmove(...)` 直调，**非** `kChaosExternalRuntimeFnTable[idx]`，且 **非** `ChaosExternalRuntimeFallback`。
- **运行验证**：`entry.exe` 跑含 Buffer 拷贝的 subject，checksum 正确（验证 ABI 不 corrupt），且 `elapsedMilliseconds` 显著优于 fallback。
- 全 AOT/JIT 测试绿。

---

## 4. 更大范围的候选（需要 run-data 排序，第二优先级）

方向 A 的"完整覆盖"需要知道**实际哪些 InternalCall 落 catch-all**，这要用 benchmark profile（`bottleneck=dispatch_overhead` 的具体 methodSubjectId）排序。待 pipeline 环境可用后：
- 从 `_externalRuntimeSubjects` + 未命中 shape 的集合导出缺失清单。
- 按 hot-threshold 批量补 SimpleForward 注册（每个都要 ABI 核对 native stub）。
- InternalCall 但 **无 native stub** 的（如泛型 Buffer 变体）→ 方向 B/C 兜底。

---

## 5. 回归风险与防护
1. **ABI 失配**（最大风险）：ref/nuint 的 slot kind 设错 → 传参错位 → 堆损坏。防护：Step 1 逐条对着 `misc_stubs.h` 签名核 ABI slot；Step 4 跑 checksum。
2. **GC 语义**：若 native stub 内部触发分配/碰托管堆，需 `RequiresGcTransition`。防护：只注册纯 native stubs（Memmove/BlockCopy/MemoryCopy）；含 GC 的走既有处理。
3. **匹配遗漏**：shape 未命中 → 静默落回 catch-all，无害（仍是慢但正确）。防护：Step 4 用测试断言已命中。
4. **不回归**：新增注册只影响之前落 catch-all 的方法（从慢变快），不改变已直调/已注册方法的行为。

---

## 6. 结论
方向 A 落地方案清晰、机制已存在、首批 3 个（Buffer.Memmove/BlockCopy/MemoryCopy）ABI 可核对、风险可控。**实施前置**：codegen 单测能验证 call-site 生成；若要覆盖完整 InternalCall 集，需 benchmark profile 跑出实际缺失清单。本环境可先落地首批 3 个（单域 codegen，不走完整 pipeline 也能验证 emission），更大范围待 pipeline。
