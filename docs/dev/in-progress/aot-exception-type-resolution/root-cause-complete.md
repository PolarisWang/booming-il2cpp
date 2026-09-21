# 完整根因链（三步实测）

> 日期：2026-09-21

## 根因链（4 层，逐层实测确认）

### 层 1：异常类型的 MethodTable parent = nullptr ✅ **已修**

`ObjectModelEmission.cs:879-884` 的 parent 来自 `_referenceTypeBaseSubjectIds`，
而该字典的收集（`CollectReferenceTypeBaseSubjectIds`）**只认 instruction 的
TargetReference** —— 异常类型从未作为 TargetReference 出现。

**已修**（本次）：两处
1. `GetSyntheticReferenceTypeBaseSubjectId` 补异常继承链（.NET 实测）
2. `Pass 1b` 把 catch 类型的**完整基类链**加入字典

**验证**：生成体里 `chaos_mt_..._ObjectDisposedException` 的 parent
**从 `nullptr` 变为真实符号** ✅

### 层 2：反射镜像只有 16 个类型，且全是 `Chaos.TestFramework.Sdk` ❌ **未修**

`kReflImage = { "CombinedSubjects", kReflTypePtrs, 16u, ... }`

16 个类型实测：
```
Chaos.TestFramework.Assert / AssertionException / FactAttribute / ...
```
**全是 Sdk 的，没有任何 System 类型。**

**根因**（`Methods.ModuleData.cs:47-49`）：
```csharp
if (!string.Equals(metadataAssemblyName, _assemblyName, StringComparison.Ordinal))
    return;    // ← 只扫当前 assembly
```

### 层 3：`ResolveTypeByName` 因此返回 0 ❌

`System.ObjectDisposedException` 在 `System.Private.CoreLib`，
**不在镜像里** → 按名找不到 → `handle=0`。

### 层 4：抛 null 异常对象 ❌

`RaiseManagedException`：`type_handle == 0` → `chaos_raise_exception(0)`
→ **抛 null** → `chaos_eh_match_type` 必然失败。

## 为什么层 1 修好但 fact 仍失败（62 项不变）

**层 2-4 未修** —— 异常对象是 null，parent 链再正确也无对象可匹配。

**层 1 的价值**：它为「一旦对象建对，`catch (Exception)` 就能工作」铺平了路。

## 层 2 的修复成本评估

**需要**：让镜像**跨 assembly 收录**异常类型。

**难点**：
- 现设计**刻意**只扫当前 assembly（注释：*"keeps the scan proportional to the
  closure rather than to the BCL"*）
- 跨 assembly 扫描会让产物膨胀（BCL 类型众多）

**可行做法（候选）**：
| 方案 | 做法 | 成本 |
|:-----|:-----|:-----|
| **2a** | 镜像**额外收录**「catch 类型及其基类链」——只增量，不扫全 BCL | 低 |
| 2b | 去掉 assembly 过滤，扫全部 resolved 程序集 | 高（产物膨胀） |

**2a 最合理** —— 复用层 1 已收集的 catch 类型集合，把它们从**各自所属 assembly**
的元数据里取出（而非只扫当前 assembly）。

## 当前状态

| 层 | 状态 |
|:--:|:-----|
| 1 | ✅ 已修（parent 链） |
| 2 | ❌ 待修（镜像跨 assembly） |
| 3-4 | 随层 2 自动解决 |

**已投入较多轮次，继续做层 2 前需确认方向。**

---

# 🔴 2026-09-21 追加：层 3 实施后的「表发射了但仍不命中」

> 本轮（第 N 轮）实测，推翻「表没发射」的第一假设，定位到**真正的命中缺口**。

## 1. 先修了一个前提：C2226 尾逗号（已修）

`BuildExceptionTypeTable()` 发射 `kExcDesc<i>` 时用了 `.AppendLine(" },")`
（右花括号 + **逗号**）。`kExcDesc<i>` 是**独立变量声明**（`static const <type> kExcDesc0 = {...};`），
**不是数组元素** —— 正确的结尾必须是 `};`。

**为什么难定位**：单独的尾逗号在 C++11 聚合初始化里**是被允许的**，
所以编译器第一轮不报错；但右花括号带逗号会令编译器认为**声明未结束**，
于是把下一行 `static const <type> kExcDesc1` 当成同一个声明的**第二个 declarator**
（类型不匹配 → C2226 等连锁），**报错位置落在 kExcDesc1 而非 kExcDesc0** ——
第一个条目看起来是无辜的。

**证据（修复前后产物对比）**：

| | 旧产物 `..._a20598db479ece61`（21:10:04） | 新产物 `..._ac35248364c19b48`（21:14:09） |
|:--|:--|:--|
| `kChaosExceptionTypeCount` | **0 次** | **1 次** |
| `// --- Exception Type Table` | **0 次** | **1 次** |

> ⚠️ **注意**：旧产物 `a20598db479ece61` 的时间戳是 **10:04**，
> 而 `cb129e362`（发射该表的提交）是 **10:32** ——
> **旧产物是陈旧缓存，早于发射代码存在**。
> 「表没发射」的第一个假设由此**被推翻**：表确实发射了。

## 2. 真根因：`kChaosExceptionTypes` 的来源是 **catch 集合**，不是 **raise 集合**

`BuildExceptionTypeTable()` 的 `typeIds` 收集自 IR 的
`region.CatchTypeSubjectId`（**catch 子句**的类型）及其基类链。

但**消费方**是 `RaiseManagedException` —— 它按名字查表的是
**native stub 实际抛出的**类型。**两者是两个不同的集合。**

### text-json chunk 实测对比

**表里有什么（4 项，全部来自 catch 集合）**：

```
{ "!!0",                                &kExcDesc0 },   ← 泛型参数，非异常类型
{ "Chaos.TestFramework.AssertionException", &kExcDesc1 },
{ "System.Exception",                   &kExcDesc2 },
{ "System.InvalidOperationException",   &kExcDesc3 },
```

**native stub 实际请求什么**（`src/native/runtime-core/runtime_stubs/json_writer_stubs.cpp`）：

```cpp
RaiseManagedException("System.ObjectDisposedException", ...)   // ← 表里没有 ❌
RaiseManagedException("System.InvalidOperationException", ...)  // ← 表里有 ✅
RaiseManagedException("System.ArgumentException", ...)          // ← 表里没有 ❌
```

**`System.ObjectDisposedException` 恰好不在表里** —— 而它正是
`Utf8JsonWriter` Reset/Flush 那批 `factoryGap` 失败的原因
（native 抛出了**正确类型**的异常，但 `ResolveTypeByName` 返回 0
→ `chaos_raise_exception(0)` → 抛 null 对象 → `chaos_eh_match_type` 必然失败）。

### 更广的口径缺口

生成体中出现的异常名（来自 `chaos_external_runtime_*` 调用族）远多于表里的 4 项：

```
"System.AggregateException"        ← 表里没有
"System.ArgumentException"          ← 表里没有
"System.Exception"                  ← 有
"System.InvalidCastException"       ← 表里没有
"System.InvalidOperationException"  ← 有
"System.OperationCanceledException" ← 表里没有
```

**结论**：`catch` 集合只是 `raise` 集合的一个**子集**（且乱序），
用它当表的来源**系统性漏掉**了 native stub 抛出的类型。

## 3. 第二个缺口：name 匹配格式（未验证，待确认）

表项名来自 `typeIds`，形如 `System.Private.CoreLib/System.ObjectDisposedException`
（**assembly-qualified，带 `/`**），发射时按 `/` 切成 display name：

```csharp
var slash = bare.IndexOf('/');
var displayName = slash >= 0 ? bare.Substring(slash + 1) : bare;
```

→ 得 `System.ObjectDisposedException`，与 stub 请求的字符串**形式一致** ✅

但 `ResolveTypeByName` 的 fallback（模块扫描）走的是
`MakeTypeHandle/FindReflectionQueryTypeByName`（按 namespace+name 分列）——
两条路径的**键格式不同**。**本项尚未实测**，仅在加 `ObjectDisposedException`
到表之后才能区分「表没这条」vs「表有这条但匹配不上」。

## 4. 修复方向（待用户拍板）

| 方案 | 做法 | 覆盖 | 成本 |
|:-----|:-----|:-----|:-----|
| **A（推荐）** | 表的来源从 catch 集合改为 **native stub 实际 raise 的类型集合**（或两者**并集**） | 直接命中缺口 | 需让 codegen 知道 stub 抛什么 —— 可从 `runtime_stubs/*.cpp` 侧提供一份清单，或在 codegen 侧静态收集 `chaos_external_runtime_*` 的异常参数 |
| B | 在 A 基础上再加「反射镜像兜底」 | 更全 | 中 |
| C | 只在 ATG/fact 侧把 not-matched 记为 not-supported | 不修根因 | 低，但掩盖 |

**推荐 A**：`raise` 集合 ⊇ `catch` 集合，且**只有 raise 集合会被 `ResolveTypeByName` 查询**。
保持并集可以让「catch 一个从没 throw 过的类型」这种情况也安全。

## 5. 诚实标注

- 「C2226 尾逗号」已修（`.AppendLine(" },")` → `" };"`），**产物已验证**（表出现）
- 「catch 集合 ≠ raise 集合」**已由产物 + stub 源码双向实测**，非推测
- 「name 匹配格式」**未验证** —— 需在补上 `ObjectDisposedException` 后复跑才能判定

---

# 🔴 2026-09-21 收尾：修复已落地，但**指标零变化** —— 诚实结论

> 方法 A（raise ∪ catch）已实施并提交（`d55628cf3`，被并行 agent 一并提交）。
> 但**对照实测显示指标完全不变**。以下是实测数据与归因。

## 1. 修复内容（已落地）

| # | 改动 | 位置 |
|:-:|:-----|:-----|
| 1 | 异常表来源扩展为 **raise ∪ catch** | `FileOnlyExceptionTypeNames`（13 项）+ catch 集合 |
| 2 | 反射名 → subject-id 归一化 | `NormalizeExceptionTypeSubjectId()` |
| 3 | 定义门槛从 `_referenceTypeBaseSubjectIds` 改为 **`_allEmittedTypeSubjectIds`** | 见下 |
| 4 | 未发射类型改为**显式打印**而非静默丢弃 | `[codegen] exception table: N type(s)...` |
| 5 | C2226 尾逗号 `.AppendLine(" },")` → `" };"` | 已由 `d55628cf3` 提交 |

### 门槛修正是第 3 项的**关键发现**

原门槛 `_referenceTypeBaseSubjectIds` 是**从 lowering 闭包构建的**，
**只含极少数异常类型**。实测（text-json，`refBase=31` 项）：

```
System.ArgumentException               -> inRefBase=False   ← 但其 MethodTable 在本 TU 有定义
System.ObjectDisposedException         -> inRefBase=False   ← 同上
...（13 项中 10 项被判 "undefined" 而丢弃）
System.Exception                       -> inRefBase=True
System.InvalidOperationException       -> inRefBase=True
```

**一手反证**：`grep` 生成体可见
`MethodTable chaos_mt_System_Private_CoreLib_System_ArgumentNullException = {...}`
**就定义在同一 TU** —— 门槛判它"未定义"是**错的**。

改用 `_allEmittedTypeSubjectIds`（真正"MethodTable 定义已发射"的集合）后：
表项 **4 → 10**，`ObjectDisposedException` / `ArgumentException` / `SystemException` /
`NullReferenceException` / `Object` 均入表。

## 2. 对照实测：**指标零变化**

控制变量：唯一差异 = raise 集合是否参与（其余代码完全相同）。

| metric | BASE（无 raise 集） | AFTER（有 raise 集） | delta |
|:-------|:------------------:|:-------------------:|:-----:|
| total | 246 | 246 | 0 |
| passed | 181 | 181 | **0** |
| realVerified | 2 | 2 | **0** |
| stubGap | 176 | 176 | **0** |
| factoryGap | 62 | 62 | **0** |
| failed | 3 | 3 | **0** |
| gatePassed/Total | 5/8 | 5/8 | 0 |
| preAssertionRaise | 65 | 65 | **0** |

## 3. 归因：**异常表不是当时的阻断点**

生成体侧已验证修复生效（表含 `ObjectDisposedException`），但指标不动 → 说明
**这 62 项 factoryGap 卡在异常匹配之前**。

`Reset_0_Stream_0` / `Reset_1_IBufferWriterbyte_0` 的 fact 记录：

```json
{"value": 0, "caught": true, "assertFailed": false,
 "resultKind": "factoryGap", "returnType": "System.Void"}
```

**`factoryGap` 的语义是 `SubjectInstanceFactory.Create<T>()` 返回 null** ——
**被测对象根本没被构造出来**，方法体从未执行，异常匹配自然无从谈起。

即：`catch { }` 时代它已经是 factoryGap；typed catch 之后**仍是 factoryGap**。
异常类型解析是**更下游**的一环。

> ⚠️ **这修正了交接文档里「62 项 factoryGap 是 A2 typed catch 暴露的异常类型缺陷」
> 的表述** —— typed catch 只是让它们从"静默通过"变成"显式失败"，
> **失败原因自始至终是 factory 返回 null，不是异常不匹配**。

## 4. 本次工作的诚实结论

| 项 | 结论 |
|:---|:-----|
| C2226 尾逗号 | ✅ **真缺陷，已修**，产物验证（表从 0 → 出现） |
| 定义门槛 `_referenceTypeBaseSubjectIds` 过窄 | ✅ **真缺陷，已修**，产物验证（表 4 → 10）；**10/13 项曾被静默丢弃** |
| raise 集合入表 | ⚠️ **正确但当前 chunk 无收益**（指标零变化） |
| 「异常表是 62 项 factoryGap 的根因」 | ❌ **被本次对照实测推翻** —— 真阻断在 factory 层 |
| 异常匹配本身（`chaos_eh_match_type` 路径） | ❓ **仍未验证** —— 表修好后仍未观察到它被走到 |

## 5. 下一步（真正的阻断点）

1. **`SubjectInstanceFactory.Create<T>()` 返回 null** —— 62 项 factoryGap 的**直接原因**，
   优先级最高。需定位为何返回 null（裸对象路径 / Create 不跑 ctor）。
2. 上述修好后**再复测**异常匹配 —— 届时 `ObjectDisposedException` 已在表内，
   可直接判定异常表修复是否终于生效。
3. `FileOnlyExceptionTypeNames` 的漂移守卫已就位：
   `python tools/scan_raised_exception_types.py --check`（含负控验证，删一项即红）。
