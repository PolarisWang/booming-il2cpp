# ChaosRegisterIlDataTable 未接线 — 调查与立项

> task_id: `il-data-table-not-registered`
> 类型：plan（域：运行时 runtime-core）
> 创建：2026-09-18
> 发现于：3 项 AOT/JIT 跨技术分歧排查（commit `33c9078af`）

## 一、问题陈述

`ChaosExternalRuntimeFallback` 的 **Phase 1 是死代码**：它读取的 IL 数据表指针
永远是 `nullptr`，因为**设置该指针的函数从未被调用**。

即：codegen 发射了 `kChaosExternalRuntimeIlData[]` 表（约 97 条），
但该表**从未发布给运行时**。

## 二、证据（全仓 grep，三处齐全 = 缺调用方）

| 角色 | 位置 | 状态 |
|---|---|---|
| 表定义（codegen 发射） | `ModuleRegistration.Dispatch.cs:476` `extern "C" ChaosIlDataEntry kChaosExternalRuntimeIlData[] = {` | ✅ 存在 |
| 指针 + setter | `interop_stubs.cpp:708` `static ChaosIlDataEntry* s_chaos_external_runtime_il_data = nullptr;`<br>`interop_stubs.cpp:710` `ChaosRegisterIlDataTable(table)` | ✅ 存在 |
| 头文件声明 | `interop_stubs.h:155` | ✅ 存在 |
| **消费方** | `interop_stubs.cpp:795` `auto* table = s_chaos_external_runtime_il_data;` | ✅ 存在 |
| **调用方** | — | ❌ **不存在** |

验证命令：
```bash
grep -rn "ChaosRegisterIlDataTable" --include="*.cpp" --include="*.h" --include="*.cs" src/
# 仅 2 处命中：定义 + 声明。零调用。
```

## 三、设计意图（从注释与结构可读）

该机制本是**「有 IL 数据就能执行」的最后兜底**：

```
ChaosExternalRuntimeFallback(subject_id)
  ├── Phase 0.5  SIMD 特判
  ├── Phase 1    _TryExecuteViaIlData  ← 本条，需 s_..._il_data != nullptr
  ├── Phase 2    kChaosExternalRuntimeSubjects 分派表扫描
  └── Phase 3    RaiseManagedException(NotImplementedException)
```

Phase 1 命中时：用 `json_data`（AotCoreIr JSON，由 codegen 的
`TryBuildCryptoAotIrJson` 生成）构造 `PatchMethod` 并缓存到
`entry.patch_method`，经 `InterpreterEntryDirect` 执行，**返回真实结果**
（D-class 修复：不再硬编码 0）。

注：表**以 crypto 命名**（`_TryExecuteViaIlData` 的注释写「Try to execute a
crypto method」），但发射条件已泛化（`hasIlData`），需确认预期覆盖范围。

## 四、接线方式（待拍板）

参照 `chaos_runtime_host.h` 的既有模式：

```cpp
// Step 4: Fill unresolved external runtime table entries with safe stubs.
FillExternalRuntimeStubs();
// Step 5: Register JIT methods (no-op in AOT mode).
ChaosJitRegisterAll();
```

`ChaosJitRegisterAll()` 是 codegen 发射的空实现 / 真实实现（按 `#ifdef`），
**AOT 模式下为空体**。IL 数据表可循同一模式，但有两个差异需决定：

1. **表是数据不是函数** —— 需要一个 `extern "C"` 的 getter 或让 codegen
   发射一个 `ChaosRegisterIlDataTables()` 空/真实实现（与 JIT 同构），
   在 `ChaosRuntimeHost::Initialize` 的 Step 5.x 调用。
2. **链接问题** —— `interop_stubs.cpp:705` 注释说明用间接指针正是为了避免
   「runtime core 不带 codegen 产物链接」时的未解析符号。若改成直接 extern
   会破坏该性质；**应保持间接指针 + 由 codegen 侧发射注册函数**。

## 五、⚠️ 收益估算（⚠️ 结论已被第七节推翻）

> **本节的两个结论都不成立，保留作排查过程记录。**
> - 「应当接线」→ ❌ 实测为负收益（359→340）
> - 「51 个能走 Phase 1 执行真实 IL」→ ❌ 它们执行时全抛异常
>
> 仍有效的部分：**交集测量方法**与**交集数字 51**（说明表与 catch-all 确有重叠）。

在 `threading-tasks` chunk 上做的交集测量：

| 指标 | 数值 |
|---|---|
| `kChaosExternalRuntimeIlData[]` 表条目 | **97** |
| 生成代码中传给 `ChaosExternalRuntimeFallback(...)` 的**不同** subject_id | **286** |
| **两者交集（接线后可经 Phase 1 执行）** | **51** |

即当前落在 Phase 2/3 的 286 个 catch-all subject 中，**51 个在表里有 IL 数据**，
接线后它们能走 Phase 1 执行真实 IL，而不是抛 `NotImplementedException`。

**命中的正是本 chunk 的主要失败族**（节选）：

```
Task::WaitAll / WaitAny / WaitAsync / WhenAll / WhenAny / Delay / Wait
Task::ContinueWith（多个重载）
TaskCompletionSource::SetCanceled / TrySetCanceled
TaskFactory::StartNew（多个重载）
ValueTask::Preserve
ConcurrentExclusiveSchedulerPair::Complete
Decimal::op_Inequality / String::op_Inequality / Exception::GetType
```

未命中的 235 个主要是 `Chaos.TestFramework.Sdk/*`（断言辅助类，测试框架自身）。

> ⚠️ 但这 51 个**不等于 51 个 fact 转绿**：Phase 1 走解释器执行 IL，
> 结果正确性取决于该 IL 的执行路径（与 AOT native body 可能有差异）。
> **接线后必须实测 AOT/JIT 两侧，不能假设等量转绿。**

### 验证脚本（可复用）

`tmp_ildata_check.py` 的思路（已用完删除，需要时重写）：
1. 正则截取 `ChaosIlDataEntry kChaosExternalRuntimeIlData[] = { ... };` 块内所有首字段字符串
2. 扫描全文所有 `ChaosExternalRuntimeFallback("...")` 的字面量参数
3. 两者求交集

> 注意：heredoc 里写含反斜杠的正则会被 shell 吞掉，**应写 .py 文件再跑**。

## 六、与本次分歧的关系

**无关。** 3 项 AOT/JIT 分歧的根因是 hotpatch flags（见
`system-threading-chunk-triage/crosstech-divergence-2026-09-18.md`），
Phase 1 在两边都未执行，不参与该分歧。

本项是**排查中偶然发现的独立缺口**，单独立项。

---

## 七、❌ 接线尝试已实测并回退（2026-09-18）

**结论：收益为负，已回退全部代码改动。**

### 实施内容（三个文件）

1. `ModuleRegistration.Dispatch.cs` — 在有 IL 数据的 chunk 里发射
   `ChaosRegisterIlDataTables()` 定义所需信息（新增 `HasEmbeddedIlData`）
2. `NativeAotLoweringPlanner.Methods.cs` — 在 **文件作用域**（globalDeclarations）
   发射 `ChaosRegisterIlDataTables()` 定义；namespacePreamble 里加声明
3. `chaos_runtime_host.h` — `Initialize()` 新增 Step 6 调用

### 实测结果（threading-tasks，build+fact）

| 指标 | 接线前 | **接线后** |
|---|---|---|
| AOT passed | **359/456** | **340/456** ⬇️ −19 |
| `factoryGap` | 65 | **87** ⬆️ +22 |
| JIT passed | 356/456 | 339/456 ⬇️ |

**Phase 1 变得可达后，结果变差了。**

### 为什么（机制）

Phase 1 用解释器执行嵌入的 IL。这些 IL **执行时会抛异常**
（`caught=True`），而不是正常返回：

```
Dispose_2__0          passed=False  kind=factoryGap  caught=True
RunSynchronously_0__0 passed=False  kind=factoryGap  caught=True
Wait_7__0             passed=False  kind=factoryGap  caught=True
Preserve_6__0         passed=False  kind=factoryGap  caught=True
```

即：这些 subject 之前**经由 native body 静默通过**（或落 Phase 3 被记为
可解释的失败），现在改走「解释器执行 IL → 抛异常」，反而**从 pass 变 fail**。

### 这印证了本文档第五节的警告

> ⚠️ 但这 51 个**不等于 51 个 fact 转绿**：Phase 1 走解释器执行 IL，
> 结果正确性取决于该 IL 的执行路径（与 AOT native body 可能有差异）。

该警告是对的，且实际情况比警告更差 —— **不是「不一定转绿」，而是「大面积转红」**。

### 判定

**Phase 1 不是「缺失的功能」，而是一条语义不等价的执行路径。**
把它接上等于用「解释器跑一遍同一段 IL」替换「native body / 明确 fallback」，
而解释器路径在当前成熟度下**成功率低于**原有路径。

**因此「未接线」很可能是有意为之**（或至少：接线前必须先让解释器对该类 IL
的执行成功率高于 native 路径）。

### 回退状态

三个源文件已 `git checkout` 回 HEAD，**工作区无本轮残留**。
（`ObjectModelEmission.cs` / `ReflectionObjectEmission.cs` 的修改属**并发会话**，未触碰。）

### 后续若要重做，前置条件

1. 先量化：这 51 个 subject 的 IL 在解释器下的**成功率**（当前为 0% → 全抛）
2. 修解释器侧的执行失败原因，而不是先接线
3. 接线后必须 A/B 对比 **passed 总数**，不能只看「表里有几条」

> 教训已沉淀 memory：`extern-c-definition-in-namespace-vs-global-decl-c2733`
> （含 C2598 / C2733 / `#define` 互斥三个 codegen 坑）
