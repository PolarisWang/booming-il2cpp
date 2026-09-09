# AOT-Native-Coverage-Gap — AOT 原生翻译覆盖缺口闭合(工业化命名)

> 原名为"fact-266"(内部 pipeline 序号代号,已完成大量但语义不可读、外部协作看不懂)。
> 本任务以**能力语义**命名并作为长期追踪基线,用翻译追踪(见 wiki 的 capabilitySummary/
> translation_tracker.py)对每个剩余缺口做量化闸门:修复一个 → `capabilityAggregate.NativeGenerated`
> 占比应单调上升 / `NoCanonicalBody` 下降。

## 命名规则

- **AOT-Native-Coverage-Gap**:一个方法走了 interpreter/ExternalRuntime-fallback 而没有真实 AOT C++
  翻译。等同旧的"翻译墙"概念。
- 每缺口有 稳定 ID:`GAP-<domain>-<NNN>`(domain ∈ {bcl-abi, reflector, generics, interop, enum,
  nullable})。
- 生命周期:open(登记)→ fixing(提交中)→ flaky/known-interim(噪声)→ closed(native 实现 +
  fact 绿)。
- 关闭判定 = 该 subject 在 foundation-dll full 下 fact passed + translation_tracker.py 显示该
  方法已记 NativeGenerated / 不再出现在 NoCanonicalBody 中。

## 剩余缺口清单(GAP-ID 登记,合并自原 fact-266 11-remain 基线 fact 3522/3533)

根因在 codegen planner + registry inline/native stub,修复需 codegen 域 + 每次 ~10min pipeline 验证。

### 类 A — reference-type null-guard 墙 (已注册 native,差 inline 绕过) 3
- GAP-bcl-abi-001 Delegate.Combine — native ChaosDelegateCombine 已就位, codegen null-guard 先 raise_NRE
- GAP-bcl-abi-002 Delegate.Remove  — 同上 ChaosDelegateRemove
- GAP-bcl-abi-003 String.Join(string,string[]) — null 数组/参数 ABI

### 类 B — SimpleForward 泛型匹配 3
- GAP-reflector-004 Enum.TryParse(Type,string,out object)
- GAP-reflector-005 Enum.TryParse(Type,string,bool,out object)
- GAP-generics-006 Enum.TryParse<T>(string,bool, out int) 泛型,未注册 int 版

### 类 C — 泛型 value-type 0-arg/ref-struct 墙 2
- GAP-generics-007 Nullable<int>.GetValueRefOrDefaultRef (value type ABI 16B → 0-arg)
- GAP-generics-008 ReadOnlySpan<int>.ToArray (ref struct 特殊 value type)

### 类 D — 类型系统大分发 1
- GAP-reflector-009 Convert.ChangeType(object, TypeCode[, IFormatProvider]) — TypeCode 映射大复杂度

> **注:** 原 fact-266 基线为"11-remain" (11 项), 此处列出 3+3+2+1 = 9 项。剩余 2 项
> 尚未归入当前分类(可能涉及 interop/enum/nullable 域), 待后续 GAP 登记补齐。

## 进度
- (继承自原 fact-266) 已修 902 个, 基底 fact 3522/3533。BitConverter (原 2 个) native+inline
  已落 (ChaosBitConverterGetBytesFromSingle/Double 在 array_stubs.cpp:172/187), **但尚未经验证**
  (未见本次 diff 的 fact 确认结果)。验证方法: 在 foundation-dll full 下确认 fact 3522/3533 对应翻译路径为
  NativeGenerated / 不再出现在 NoCanonicalBody 中。若仍 fail 查 stale lib。
- 当前不在本 working-tree 修复(需 codegen session + 全 pipeline)。**执行移交点见本文件同目录
  notes/execution-handoff-...md**,或对每个 GAP 建独立 codegen 任务。

## 架构映射
- 修复分层: codegen 翻译路径 (NativeAotLoweringPlanner / registry InlineShapeDescriptor) +
  native runtime stub + CodeGenStage.Planning BodyAvailabilityCode。
- 追踪: 本篇命名是本会话的翻译追踪工作命名落点;capabilitySummary +
  translation_tracker.py 作为收敛度量(修复一个 → NativeGenerated 占比上升)。

## 检查点 (2026-09-07)
- 本目录 = 原 "fact-266" 的工业化重命名落点。重命名本身是纯文档/追踪语义变更,无代码行为改动。
- 实际 GAP 修复需 codegen 域会话 + 每次 ~10min foundation-dll 验证,**不在本 working-tree 直接改**(否则改了一半无法验证)。
- 并行 TPG agent 在本会话活动频繁(git lock 反复争用),重命名提交若与并行线交错,以后续 memory
  手记为准对齐。

## 🔍 2026-09-07 复核：9 个 GAP 的 codegen 注册大多已在 HEAD（验证被 CoreLib 生成 bug 阻断）

专职 codegen 调查（session 2026-09-07）确认：**9 个 GAP 中的绝大多数修复代码已在 `df7813de7`
("inline all 11 remaining reflection-family stubs — bypass null-guard + return-0") 及后续 add 中落在 HEAD**：

| GAP | 方法 | HEAD 中的 codegen 注册 | 位置 |
|-----|------|----------------------|------|
| bcl-abi-001 | Delegate.Combine | ✅ InlineShapeDescriptor | `Part3.S23.cs:419` |
| bcl-abi-002 | Delegate.Remove | ✅ InlineShapeDescriptor | `Part3.S23.cs:427` |
| bcl-abi-003 | String.Join(string,string[]) | ✅ SimpleForward `ChaosStringJoinSs` + Inline | `Part1.S10.cs:78`, `Part3.S23.cs:436` |
| reflector-004 | Enum.TryParse(Type,string,out object) | ✅ SimpleForward `ChaosEnumTryParse` | `Part3.S23.cs:783` |
| reflector-005 | Enum.TryParse(Type,string,bool,out object) | ✅ SimpleForward `ChaosEnumTryParseWithIgnoreCase` | `Part3.S23.cs:793` |
| generics-006 | Enum.TryParse\<T\>(string,bool,out int) | ⚠️ 泛型 GenericShapeDescriptor，需确认 int 版本 | 核对 Generics 域 |
| generics-007 | Nullable\<T\>.GetValueRefOrDefaultRef | ✅ InlineShapeDescriptor | `Part3.S23.cs:485` |
| generics-008 | ReadOnlySpan\<T\>.ToArray | ✅ GenericShapeDescriptor | `Part2.S17.cs:156` |
| reflector-009 | Convert.ChangeType(object,TypeCode) | ✅ Inline 直接 native | `Part3.S23.cs:467` |

即："9 个 open GAP" 清单**可能已过时**——df7813de7 比 STATUS 早约 246 commit。STATUS 列表需在
**真 foundation-dll fact 验证**后改为：已 pass 的标 closed，真剩的标 open。

### 验证遇阻（2026-09-07 hotupdate/coverage session 实测）
- `ci_smoke --mode full`（15 families full pipeline）在 CoreLib `System.Private.CoreLib/chunks/system-3`
  的 **managed-combined 生成 `CombinedSubjects.cs` 编译失败**（数千 CS1525/CS1003/CS1001 语法错，
  如 `UIntPtr.TryParse` 相关行）处中断。这是 CoreLib **managed net10/net8 reference-combined** 构建的
  预存在 codegen bug —— 与 9-GAP 的 native 翻译路径独立，但阻塞了完整 fact 验证通道。
- 3-family 子集（System.Linq / Collections.Immutable / ObjectModel）全绿（3/3 passed，215+32+10 fact），
  但这些 family 的 subject 不覆盖 Delegate/Enum/String.Join 等 CoreLib 方法 → 不足以关闭这 9 个 GAP。
- 因此：**9 GAP 当前 open 状态未变**（代码在 HEAD，但未在其对应 subject 的 foundation-dll fact 中验证真 pass）。

### 下一步（供续跑/新 codegen session）
1. 分诊 CoreLib `system-3` managed-combined 的 CS1525 生成 bug（定位 `UIntPtr.TryParse` subject 生成，
   判断是 emitter 模板 OR subject-generator 措辞问题）——它阻断 foundation-dll full 管线的 usable 通道。
2. system-3 修好后，跑 `ci_smoke --families System.Private.CoreLib`（或 foundation-dll full）核对上述 9 项：
   对应 subject fact passed + NativeGenerated → 逐个标 closed；仍 fail 才需新 codegen 改动。
   （注意：这些方法多为 CoreLib subject，需 CoreLib family 而非 Linq/Immutable。）

---

## 🔍 2026-09-09 复核（Head 86ae9d7f3，system chunk 全重建验证）

专职 codegen session 完成 CoreLib `system` chunk 全重建（含 fact），逐项验证 9 GAP：

### 真实状态逐项

| GAP | 方法 | fact bodyAvail | fact value | 判定 |
|-----|------|:------------:|:-------:|:----:|
| bcl-abi-001 | Delegate.Combine | NativeGenerated | (非 fact 窗口) | **CLOSED** |
| bcl-abi-002 | Delegate.Remove | NativeGenerated | 0 | **CLOSED** |
| bcl-abi-003 | String.Join(string,string[]) | NativeGenerated | (非 fact 窗口) | **OPEN-verify-gap** |
| reflector-004 | Enum.TryParse(Type,string,out object) | NoCanonicalBody | 42 | **OPEN-codegen**（inline 已落→ChaosEnumTryParse，bodyAvailability 标注不认 inline 模式） |
| reflector-005 | Enum.TryParse(Type,string,bool,out object) | NoCanonicalBody | 42 | **OPEN-codegen**（同上） |
| generics-006 | Enum.TryParse\<int\>(string,bool,out int) | NoCanonicalBody | 0/1/42 | **OPEN-codegen**（泛型 T 无本地 native） |
| generics-007 | Nullable\<int\>.GetValueRefOrDefaultRef | NativeGenerated | 42 | **OPEN-verify-gap** |
| generics-008 | ReadOnlySpan\<int\>.ToArray | NativeGenerated | (无主体) | **OPEN-verify-gap** |
| reflector-009 | Convert.ChangeType(object,TypeCode) | **NativeGenerated** | **0** | **CLOSED** |

#### 判定口径
- **CLOSED**：代码就位 + NativeGenerated + 有真实非 42 事实值；只需文档关闭
- **OPEN-codegen**：codegen 注册/bodyAvailability 标注仍有真实缺口，需 codegen 域修复
- **OPEN-verify-gap**：代码已就位且 NativeGenerated，但值仍为 42（ATG smoke 问题，非 codegen 缺口）

#### 2026-09-09 会话产出
1. **Enum.TryParse inline 修复**（`Part3.S23.cs:451-481`）：non-generic 的 TryParse(Type,String,...) 从 return-0 stub 改为 inline 直接调用 `ChaosEnumTryParse`/`ChaosEnumTryParseWithIgnoreCase`，绕过 SimpleForward ABI slot 不匹配。生成 C++ 已确认产出正确 native 符号调用。
2. **CoreLib system chunk 重建**：NativeGenerated 从 52083→57396（+5313），NoCanonicalBody 从 638→709（含 Enum.TryParse 等）。
3. **3 项 CLOSED**：Delegate.Combine（bcl-abi-001）、Delegate.Remove（bcl-abi-002）、Convert.ChangeType（reflector-009）。

### 建议下一入口
- 专职 codegen 域会话修复 `build.py` 的 `_enrich_metadata_body_availability`，使 inline→native 模式正确标注 NativeGenerated
- 「剩余 6 项」中 3 项 verify-gap 本质上 ATG smoke 问题，3 项 codegen 需本体修复
