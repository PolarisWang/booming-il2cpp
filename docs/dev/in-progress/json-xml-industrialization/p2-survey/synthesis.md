I have the decisive timeline. Let me write the final report.

**Timeline established**: `exc_diag.txt` was committed at `aed6f0287` (2026-09-21 12:18), which is a descendant of the exception-table fix `cb129e362` (2026-09-21 10:49) — so the diag data was produced from an **already-fixed** runtime. Yet all 245 raises still show `handle=0`.

---

# XML realDefect 跨族根因分析报告

## 0. 结论摘要

| # | 结论 | 置信度 |
|:-:|:-----|:------:|
| 1 | **4 个族不是 4 个根因**。存在 **1 个跨族共享根因**（异常类型解析失败 → 抛 null 对象），叠加 **2 个「测试载体」根因**（void 无法断言 / Matches() 无元数门控） | 高 |
| 2 | 当前 W1-W4 分族**部分不成立**：W2/W3/W4 的绝大部分是同一个缺陷，按测试类分族会产生**重复修复与语义冲突** | 高 |
| 3 | 投入产出比最高的是**先修载体与解析闭环（P0）**，再按 native 文件分族做语义对齐 | 中高 |

---

## 1. 已确认的跨族共同根因

### RC-1 [已确认] 异常类型解析失败 → 抛 null 对象 → typed catch 结构性不匹配

**证据（一手实测，非推断）**：`exc_diag.txt` 共 490 行 = 245 对 `[MOD]`/`[EXC]`，统计结果：

```
in_types=0 : 245 / 245   (100%)
handle=0   : 245 / 245   (100%)
module=1   : 245 / 245
名称分布   : NullReferenceException 120 / ObjectDisposedException 52
             ArgumentNullException 52 / InvalidOperationException 16
             ArgumentException 4 / TaskCanceledException 1
```

**245/245 全零命中，覆盖了四个族各自期望的全部异常类**（NRE 是 reader/document 族的主期望；ANE 是 convert 族的主期望；InvalidOperation 是 document/reader 族的主期望）。

**机制（源码已确认）**：`src/native/runtime-core/exception_helpers.cpp:201-209`

```cpp
const auto type_handle = ResolveTypeByName(type_full_name);
if (type_handle == 0) {
    chaos_raise_exception(0);      // ← 抛 null 对象
}
```

`chaos_eh_match_type` 对 null 对象必然失败 → 控制流落入 ATG 生成的 `catch { throw new Exception("wrong exception type"); }` → `caught=true, value=0`。这与 `convert-defects.json` / `document-defects.json` / `reader-defects.json` / `writer-defects.json` 中**全部 159 条的 `caught=true` 且 value≠42** 的实测形态完全一致。

**这是四个族报告的「异常类不匹配」的最强解释**：各族独立调查都在比较「native 抛的类名」vs「probe 记录的类名」，但若**两个类名都解析不到**，则无论类名是否改对，typed catch 都不会命中。

**⚠️ 该结论与「native 类名写错」并不互斥** —— 二者是**串联**的两道闸：
1. 闸 1（RC-1）：解析不到 → 抛 null → 任何 catch 都失败；
2. 闸 2：解析得到但类名与 probe 不符 → typed catch 失败。

**`exc_diag.txt` 的时序说明闸 1 至今仍存在**：该文件入库于 `aed6f0287`（2026-09-21 12:18），其祖先是异常表修复 `cb129e362`（2026-09-21 10:49，即 `ResolveTypeByName` 的 compile-time fast path）。**即：数据是在 fast path 已存在的前提下采集的，仍然是 245/245 全零**。

### RC-2 [已确认缺陷，影响范围待测] `BuildExceptionTypeTable` 的 MethodTable 闸门可能剔除关键类型

`NativeAotLoweringPlanner.ModuleRegistration.Dispatch.cs:853-866`：

```csharp
var definedTypeIds = emittedMtTypeIds.Count > 0
    ? emittedMtTypeIds
    : (_allEmittedTypeSubjectIds ?? ...);
var emitTypes = typeIds
    .Select(NormalizeExceptionTypeSubjectId)
    .Where(id => !string.IsNullOrEmpty(id) && definedTypeIds.Contains(id!))
```

并且 `:875-880` 把未发射项**只打成 stderr 警告**，不失败：

```
"[codegen] exception table: N type(s) have no MethodTable in this TU
 (reflection-image fallback must resolve them): ..."
```

`FileOnlyExceptionTypeNames` 当前为 **15 条**（`tools/scan_raised_exception_types.py --check` → `OK: 15 raised exception types, C# list in sync.`）。而 `exc_diag.txt` 实测 `type_count=16`（且 `module=1`）。**16 ≠ 15**，说明该诊断截图的构建与当前表**不同批**（RC-1 的实证存在于一个与当前源不同步的构建上）。

> 这是**本轮最重要的诚实标注**：`exc_diag.txt` 与 `p2-survey/*-defects.json` 是否同批未经验证。若不同批，则 RC-1 能否解释这 159 条不能直接断言。

**reader 族已独立发现同一疑点**（其 `unverified` 第 2 条：「`subjects.json` contains no exception type names at all … the null-object hypothesis is inferred … not directly observed」）—— **reader 的条件性推断现已由 `exc_diag.txt` 的 245/245 实测证实，但前提是两者同批。**

### RC-3 [已确认，载体缺陷] void/async void 无法被断言 → `real` 口径系统性失效

`TestEmitter.cs:635-638`：

```csharp
if (method.IsVoid || isPlainTask || hasException)
    sb.AppendLine("            return 42L;");
```

**writer 族 P1（48 条 sync）的正解**：这些 subject 的生成体是 `XmlWriter.Create(...).M(...); return 42L;`，**全文无断言**，而 AOT native 实现是真实的、正常返回的。fact 侧 `fact_chunk.py` 的 `real` 判据依赖 `value != 42` —— void 主体**结构上永远得不到非 42 的 value**。

**writer 族的「value 恒为 0」与 RC-3 存在张力**：writer 说 96 条 value 恒为 0（「AOT 主体 return 0」）。但 `TestEmitter.cs:638` 对 void **无条件**写 `return 42L`。两者要同时成立，只能解释为 `AppendAssert` 在 `stmts` 中注入了一条**早于** `return 42L` 的 `return`（或在 `isExternalAssembly` 分支）。**此点未确证**，writer 族自己也将其列入 `unverified`。若 value 确实为 0 而非 42，则 writer 的 P1 分类（「载体无断言」）**不成立**，应与 P2 合并为 RC-1/RC-2 的下游。

### RC-4 [已确认缺陷] `Matches()` 无元数/参数门控 → 名字级白名单 × 元数级实现

`TestEmitter.cs:81-95`：

```csharp
if (!Methods.Contains(methodName, StringComparer.Ordinal)) return false;
foreach (var exact in ExactTypeNames) { if (declaringType == exact) return true; }
foreach (var fragment in TypeFragments) { if (declaringType.Contains(fragment)) return true; }
```

**只按「方法名 + 类型片段」匹配，完全忽略 arity 与参数类型**。而 `xml_document_stubs.cpp` 中 `RaiseNotSupported()` 出现 **19 次**（`:220,250,268,289,296,315,334,348,360,376,388,400,409,418,436,457,466,477`），大量重载**没有任何校验**。于是这些重载被强制断言 `Throws`，但 native 走的是 NotSupported 兜底 → 类名不符。

**RC-4 是「把正确的 native 实现判成缺陷」的放大器**，且**与 RC-3 同属 `TestEmitter.cs` 一个文件**。

---

## 2. 分族假设判定：W1-W4 需要重新划分

roadmap-v4 §7 自己已标注：「Phase 2 分族基于 **realDefect 的测试类分布**，各族内部的失败模式尚未调查 —— P2-00 会验证『按测试类分族 == 按根因分族』」。

**判定：该假设不成立，且 v4 正文的「按 native 实现文件分族」与其「按测试类分族」在 W4 上自相矛盾。**

### 2.1 v4 内部矛盾（已确认）

roadmap-v4 §2.2 称 W4 = `XmlTextReaderTests`(9) + `XmlValidatingReaderTests`(5) + `XmlNodeReaderTests`(1) = **15**；但 §4 Phase 2 表格称 W4 = `xml_reader_stubs.cpp`**(32KB)**。

**这两者不是同一个集合**：`xml_reader_stubs.cpp` 被 `XmlNodeReader` / `XmlValidatingReader` **共享**。reader 族的 `unverified` 第 3 条已独立指出：

> `RuntimeHelperShapeRegistry.CoreStubs.Part1.S20.cs` RegisterReaderVariant routes XmlValidatingReader and XmlNodeReader onto the **SAME ChaosXmlTextReader\* symbols**

**按 native 文件分族在 W4 内部就失效**：改 `xml_reader_stubs.cpp` 会同时影响三个测试类。

### 2.2 归属重划建议

| 族 | v4 归属 | 实际根因分布 | 建议 |
|:--|:--|:--|:--|
| **W1 writer (96)** | `xml_writer_stubs.cpp` | RC-3（载体）+ 疑似 RC-1 | **降级为 P0 载体问题的验证场，不是独立族** |
| **W2 convert (23)** | `xml_convert_stubs.cpp` | 单一 helper `ToCString`(:72-79) + Verify* 的 3 处 `return`(:199/216/227) | **保留为独立族**（唯一「单一文件、单一 helper、类名写错」的真·native 缺陷） |
| **W3 document (25)** | `xml_document_stubs.cpp` | RC-4（Matches 门控）+ `CreateElement/SetAttribute/CreateEntityReference` 期望 NRE 是 **null-this 契约** —— ATG 用 `SubjectInstanceFactory.Create<T>()` 造裸对象（`TestEmitter.cs:207` Rationale 明写「bare objects (not real document trees), so every instance method throws NRE」）。**null-this 的 NRE 在 AOT 上结构不可复现** | **拆两半**：`CreateNode/CreateDocumentType/Save/Validate`（真 native 缺陷）归 native 域；NRE 那 8 条归**载体域** |
| **W4 reader (15)** | `xml_reader_stubs.cpp` | RC-1（ArgumentOutOfRangeException 缺失）+ P4a 的**探针接收者不一致** | **必须与 W3 的载体域一并处理**，且不能单独占一个 worktree 改该文件 |

### 2.3 会产生冲突的具体文件（已确认）

| 文件 | 被哪些族同时需要改 | 冲突类型 |
|:--|:--|:--|
| `TestEmitter.cs` | W1(元数/断言) + W3(Matches 门控) + W4(异常表 Rationale) | **语义冲突**：三族都要改同一函数，且 W1 的「加 void 副作用断言」与 W3 的「收紧 Matches 避免误报」方向相反 |
| `xml_document_stubs.cpp` | W3 + W4（`CreateElement` 的 NRE 契约） | 同函数改写 |
| `RuntimeHelperShapeRegistry.CoreStubs.Part1.S19-S22.cs` | W3 + W4 | v4 §4 已标注「共享文件（Q2c：接受冲突，合并时解决）」 |
| `RuntimeHelperShapeRegistry.CoreStubs.Part1.S20.cs` | W4 内部（TextReader/ValidatingReader/NodeReader 同符号） | **worktree 内自冲突**，无法靠分工解决 |

---

## 3. 修复优先级（投入产出比排序）

### P0 — 载体与解析闭环（一次修复，跨族生效）

| 序 | 动作 | 消除量 | 依据 |
|:-:|:--|:--:|:--|
| **P0-a** | **先做一次控制实验分辨闸 1 vs 闸 2**：在干净的 main worktree 跑一次 xml chunk，**同时** dump `kChaosExceptionTypeCount` 的实际值 + `ResolveTypeByName` 的命中计数 | — | 这是**唯一**能把 RC-1 与「native 类名写错」分开的手段；reader 族自己要求 2-way control |
| **P0-b** | 把 `BuildExceptionTypeTable` 的「MethodTable 未发射」警告**升级为硬失败**（`Dispatch.cs:875-880`），并补 `scan_raised_exception_types.py --check` 进 CI | 防回退 | 负控脚本已存在且当前 `OK`；把它接进 pre-commit 即可 |
| **P0-c** | 若 P0-a 证实 RC-1：修 `ResolveTypeByName` 的回落路径 / 补 `FileOnlyExceptionTypeNames` | **159 条中的大部分** | `exc_diag.txt` 245/245 |
| **P0-d** | 修 `Matches()` 加 arity 门控（`TestEmitter.cs:81-95`），并处理 null-this 契约（`HasNullReceiver` → 不生成 typed catch） | 消除 W3/W4 的**假缺陷** | 文档族 8 条 + reader P4a 10 条 |

### P1 — 按 native 文件分族做语义对齐（保留 v4 的 W2）

| 序 | 族 | 动作 | 消除量 |
|:-:|:--|:--|:--:|
| **P1-a** | **W2 convert** | `ToCString`(:76) 的 `System.FormatException` → `System.ArgumentNullException`；Verify* 三处 `return`(:199/216/227) → raise ANE | **23**（该族 `confidence: high`，是唯一「已定根因」的族） |
| **P1-b** | **W3 document** | `CreateNode`(:292/:327)、`CreateDocumentType`(:351)、`Save`(:412)、`Validate`(:460)、`ImportNode`(:469) 的异常类对齐 | ~10 |
| **P1-c** | **W4 reader** | 先解决 S20 的 `RegisterReaderVariant` 共享符号问题（探测已证 XmlNodeReader Set0 `hasException=FALSE` 而 XmlTextReader 为 NRE，**同一符号无法满足两者**） | ~10 |

### P2 — writer 载体（成本最高，收益最多但阻塞）

**W1 的 96 条不应先做**：若 P0-c 成立，这 96 条中相当部分会自动好转；若 P0-d 的载体问题成立，则需要先落地 `phase2-blocker-void-assertability.md` 的方案 A（ATG 生成 void 副作用断言），那是**跨层改动**（ATG + native 读接口），单独占一个 worktree 更合理。

---

## 4. 明确的「已确认 / 推测」划线

### 已确认（一手文件/实测证据）
- `exception_helpers.cpp:206-209`：`ResolveTypeByName` 返 0 → `chaos_raise_exception(0)`
- `exc_diag.txt`：245/245 全为 `in_types=0 handle=0`
- `exc_diag.txt` 入库于 `aed6f0287`，祖先是异常表修复 `cb129e362`（**时序：数据来自已修版本**）
- `tools/scan_raised_exception_types.py --check` → `OK: 15 raised exception types`
- `xml_convert_stubs.cpp:76` 抛 `System.FormatException`，消息文本却是 ANE 的
- `xml_convert_stubs.cpp:199/216/227` 在空/不可解析串上**静默 return**
- `xml_document_stubs.cpp` 有 **19 处** `RaiseNotSupported()`
- `TestEmitter.cs:635-638` 对 void 无条件 `return 42L`
- `TestEmitter.cs:81-95` `Matches()` 无 arity 门控
- 4 个 xml worktree 均在同一 commit `b3e1d4240`，**尚无修复**

### 推测（需 P0-a 控制实验确认）
- `exc_diag.txt` 与 `p2-survey/*-defects.json` **同批**（`type_count=16` vs 当前表 15 条，存在不一致信号）
- writer 96 条的 `value=0` 机理（与 `TestEmitter.cs:638` 的 `return 42L` 冲突，未确证）
- reader P4a（4 条）的失败是 RC-1 而非 native 契约问题

### 建议的立即行动
在**任一** xml worktree 上跑一次 chunk，落盘 `entry.exe` 的 fact 原始 JSON + `kChaosExceptionTypeCount` 实际值。这一个实验同时判定：RC-1 是否为主因、`exc_diag.txt` 是否同批、writer 的 value 到底是 0 还是 42。**在此之前不应开始 W1-W4 的任何 native 改动**——否则三个族会各自去改异常类名，而闸 1 未开，全部无效。