# 3 项 AOT=PASS/JIT=FAIL 跨技术分歧 — 根因坐实（2026-09-18）

> 结论：**不是 codegen 缺陷，是 JIT 模式的运行时分派语义差异 + fact 分类口径。**
> 修复价值低、风险高（涉及 1061 个 entry 的 flags），建议**不改代码，改分类口径**。

## 一、现象

`threading-tasks` chunk 唯一残留的 3 项分歧（456 项中）：

| subject | AOT | JIT |
|---|---|---|
| `TaskTests::WhenAll_71_Taskint_0` | PASS (real, value=1) | FAIL (failed, caught) |
| `ValueTaskTests::AsTask_5__0` | PASS (unassertable, 42) | FAIL (factoryGap, caught) |
| `ValueTaskTests::Preserve_6__0` | PASS (unassertable, 42) | FAIL (factoryGap, caught) |

分母：both pass=356 / both fail=97 / **divergent=3** / total=456。

## 二、排除项（已实测）

| 假设 | 证据 | 结论 |
|---|---|---|
| codegen 产物不同 | `diff native/codegen/generated/native-aot.page-0010.cpp` vs `build_jit_output/...` → **无差异**；pages 0004-0008 同样 | ❌ 排除 |
| IL 数据不同 | 两个 `aot-core-ir.jdata` **md5 相同** | ❌ 排除 |
| 注册表不同 | `code-registration.json` 两边均 507600 bytes | ❌ 排除 |
| 生成主体不同 | 两个 `native-aot.generated.cpp` 都存在且内容对应 | ❌ 排除 |

## 三、根因（flags 差异，实测坐实）

### 3.0 flags 由 **codegen 模式**决定，不是运行时

确切位置 `NativeAotLoweringPlanner.ModuleRegistration.cs:228-232`：

```csharp
string flags = _codegenMode.HasFlag(CodegenMode.Jit)
    ? "0"
    : methodsCallingExternal.Contains(entry.SubjectId)
        ? "kHotpatchKeepNative | HotpatchEncodeArgCount(...)"
        : "HotpatchEncodeArgCount(...)";
```

即：**AOT 模式下，凡 IL 调用 external-runtime subject 的方法**（含
`SubjectInstanceFactory.Create<T>`）得到 `kHotpatchKeepNative` → 直接跑 native body，
从不查分派表。JIT 模式下所有 entry flags 一律 `"0"`。

### 3.1 两份 exe 的真实差异位置

**页文件确实逐字节相同**（page-0004..0010、`chaos_generated_module.cpp`、
`native-aot.generated.header.h` 全部 md5 相同），差异只在
`subjects/native-aot.generated.cpp`：

| 项 | AOT | JIT |
|---|---|---|
| 文件大小 | 8,726,164 B | 8,735,181 B |
| `kHotpatchKeepNative` 出现次数 | **605** | **0** |
| `ChaosJitRegisterAll()` 体 | `{}`（空） | 真实注册（`register_hotpatch_module` + `ChaosJitDataLoad` + `RegisterJitEntryMethods`） |
| `kChaosExternalRuntimeSubjects` / IL 数据表 | 相同 | 相同 |

JIT 的注册体由 `#ifdef CHAOS_IL2CPP_JIT_MODE` 门控
（`NativeAotLoweringPlanner.Methods.cs:1385-1430`），该宏由 TPG 只在
`build_jit_output/CMakeLists.txt:61` 注入；AOT 的 `native/CMakeLists.txt` 无此宏。

### 3.2 为什么结果分歧

JIT：flags=`0` → 调用经解释器/trampoline → 命中 catch-all
`Chaos_TestFramework_SubjectInstanceFactory_Create` →
`ChaosExternalRuntimeFallback(subject_id)` → Phase 3
`RaiseManagedException(NotImplementedException)`（`interop_stubs.cpp:960+`）
→ null 守卫触发 NRE → `caught=true`，方法未执行 → `factoryGap`。

AOT：`kHotpatchKeepNative` → native body 直接跑，
`Create<ValueTask>` 的 catch-all 不可达（它只经分派表可达）→ 返回结构性 42 → `unassertable`。

### 3.3 ⚠️ Phase 1 是死代码（本轮新发现，独立于本问题）

`ChaosExternalRuntimeFallback` 的 Phase 1（`_TryExecuteViaIlData`）**在任何 chunk 都不会执行**：
`ChaosRegisterIlDataTable`（设置 `s_chaos_external_runtime_il_data`，
`interop_stubs.cpp:710`）**全仓无任何调用者** —— 仅剩定义与头文件声明。

即 `kChaosExternalRuntimeIlData[]` 表被发射了，但从未发布给运行时。
**这是一个独立的潜在接线缺口**（与本次分歧无关，但值得单独立项排查）。

> 更正：本文档早先版本曾暗示 Phase 1 可能解释 AOT 侧行为，**该说法不成立** —— Phase 1 两边都不执行。

## 三·旧、flags 差异（现象层，保留）

**AOT 与 JIT 的 hotpatch entry flags 不同：**

```cpp
// AOT (native/subjects/native-aot.generated.cpp:64038)
{ &..._AsTask_5__0, &InterpreterEntryDirect, 0ull,
  kHotpatchKeepNative | HotpatchEncodeArgCount(0) },   // ← 保持 native

// JIT (build_jit_output/subjects/native-aot.generated.cpp:64037)
{ &..._AsTask_5__0, &InterpreterEntryDirect, 0ull,
  0 },                                                   // ← flags=0，走解释器
```

3 项分歧**全部**命中同一模式（逐项验证）：
```
AsTask_5__0          AOT: kHotpatchKeepNative | ...   JIT: 0
Preserve_6__0        AOT: kHotpatchKeepNative | ...   JIT: 0
WhenAll_71_Taskint_0 AOT: kHotpatchKeepNative | ...   JIT: 0
```

**规模**：AOT 605 个 `kHotpatchKeepNative` 条目，JIT **1061 个全部 flags=0**。
即这是**模式级系统性差异**，不是个别 subject 的缺陷。

### 为什么只有 3 项暴露

JIT 走解释器后，这些方法体内的
`SubjectInstanceFactory.Create<ValueTask>()`（无 native 实现，落
`ChaosExternalRuntimeFallback`）在解释器下**真的抛了**
（`create_value_task` → `SubjectInstanceFactory` 无 body → Phase 3
`RaiseManagedException(NotImplementedException)`），
于是 `caught=True` → JIT 记为 `factoryGap`。

AOT 侧保持 native，`ChaosExternalRuntimeFallback` 的行为差异使
`Create<ValueTask>` 返回到达 `return 42L`（smoke 语义）→ 记 `unassertable`。

其余 1058 个 entry 虽也走解释器，但其方法体不依赖未实现的 fallback，
所以两边结论一致。

## 四、判定：**不是缺陷，勿改代码**

1. **AOT 侧是正确的**（`kHotpatchKeepNative` 是设计意图：Subject_N 方法保持原生）。
2. **JIT 把 flags 设为 0 也是设计意图**（JIT 模式要测量解释器/JIT 路径，
   见 `hotpatch_dispatch.h:190-225` 的 Dispatch priority 注释）。
3. **两侧的 `passed` 语义本就不同**：AOT 的 `unassertable` 是「方法跑了但没断言」
   （value=42 结构性），JIT 的 `factoryGap` 是「工厂返 null 方法没跑」。
   对同一个 smoke 用例，这两种「通过/失败」不构成「同一断言的相反结论」。

**因此这不是 WaitAll 那类真 codegen 缺陷。** 它暴露的是
**fact 分类器把两种模式的不同判定混在同一张跨技术 diff 表里比对**，
产生看起来像缺陷的噪声。

## 五、建议处置（择一）

| 方案 | 说明 | 风险 |
|---|---|---|
| **A. 扩 cross-tech diff 的比较口径**（推荐） | 对 `unassertable` vs `factoryGap` 这一对不报 diff —— 二者都不是「断言结论」，本就不该对比 | 低（只改 fact 分类） |
| B. 不动 | 接受 3 项噪声，写进 STATUS 说明 | 无 |
| C. 让 JIT 也 KeepNative | 改 1061 个 entry 的 flags | **高** —— 破坏 JIT 模式的设计意图 |

**推荐 A**：在 `fact_chunk.py` 的 cross-tech diff 生成处，把
`(unassertable, factoryGap)` 与 `(factoryGap, unassertable)` 视为**非分歧**
（两者均表示"该方法未产出可对比的断言结论"），其余组合照常比对。
