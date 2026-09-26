# 剩余 35 条 realDefect — 一手根因报告（2026-09-24）

> 基线：`p2-baseline/fact-results-59454792b.json`（provenance gitCommit=59454792b，已比对 main HEAD）
> 方法：读**新生成体**（`artifacts/.../managed/combined/CombinedSubjects.cs`）+ 读生成 C++
> + **real .NET 8 实跑探针**（`tmp_probe/`）+ native stub 源码
> 状态：**一手证据已足；workflow 的对抗性证伪仍在跑**

---

## 0. 责任层裁定（推翻两个主假设）

| 假设 | 裁定 |
|:-----|:-----|
| **假设 N**「native 异常类名写错」 | **部分成立，但不是主因**（仅覆盖 ~12 条） |
| **假设 A**「ATG probe 在裸对象语境记录期望」 | **已被证伪**（见 §1） |
| **真实主因（第三方）** | **native 的 receiver 构造函数被 stub 成 `RaiseNotSupported()`**，导致 subject 在**到达被测方法之前**就抛 `NotSupportedException`（见 §2） |

---

## 1. 假设 A 被证伪（一手证据）

ATG 的 probe 与 subject **共用同一个实例表达式来源**：

- `ProbeEmitter.cs:143` 与 `TestEmitter.cs:555` 都调 `CSharpExpressionBuilder.GetInstanceExpression(...)`
- 该方法**先查 `KnownInstances`**（`CSharpExpressionBuilder.cs:43-54` 读者/写者、`:65-89` DOM 类型），
  再回落到 `SubjectInstanceFactory.Create<T>()`（`:599`）
- `KnownInstances` 里是**真实构造表达式**：
  `XmlTextReader = new XmlTextReader(new StringReader("<root/>"))`、
  `XmlDocument = new XmlDocument()`、
  `XmlElement = new XmlDocument().CreateElement("root")` 等

生成体交叉验证一致（`CombinedSubjects.cs:8654` 等）。

⇒ **probe 与 subject 的语境相同，都是真实实例**。假设 A 不成立。
（`TestEmitter.cs:252` 那段 "bare objects" 注释是**过时注释**，描述的是引入 fixture 之前的状态。）

---

## 2. 真实主因：receiver 构造函数是 `RaiseNotSupported()` 桩

`src/native/runtime-core/runtime_stubs/xml_document_stubs.cpp:241-251`：

```cpp
CHAOS_IL2CPP_INTPTR ChaosXmlDocumentCreateElement(
    CHAOS_IL2CPP_INTPTR this_ptr, CHAOS_IL2CPP_INTPTR name) CHAOS_STUB_NOEXCEPT
{
    (void)this_ptr;
    ... // name 校验
    RaiseNotSupported();      // ← 构造 receiver 就抛
}
```

`xml_document_stubs.cpp` 共 **18 处 `RaiseNotSupported()`**。

**生成体的实际执行链**（`native-aot.page-0010.cpp`，`SetAttribute_2_string_string_0`）：

```cpp
_S0 = new XmlDocument();
_S1 = "root";
chaos_result = ChaosXmlDocumentCreateElement(_S0, _S1);   // ← 抛 NotSupportedException!
_S0 = chaos_result;
...
_S1 = 0; _S2 = 0;                                          // name/value = null
if (chaos_arg_0 == 0) { raise_null_reference_exception(); } // guard 正确，receiver 非 null，不触发
ChaosXmlElementSetAttribute(chaos_arg_0, chaos_arg_1, chaos_arg_2);  // ← 永远到不了
```

⇒ **异常在 CreateElement 处就抛了**，`SetAttribute` 从未执行。
抛出的 `NotSupportedException` 与 typed catch 的 `NullReferenceException` **不匹配**
→ 落到 `catch { throw new Exception("wrong exception type"); }`
→ runner 记 `caught=true, value=0` → `realDefect`。

**这解释了为什么 native 的 `ChaosXmlElementSetAttribute` 已经抛正确的 NRE 却仍然失败** ——
它根本没被调用到。这是 workflow F6 提出的「已抛正确类却仍失败」判别的答案。

### 2.1 受影响范围（按 receiver 构造被 stub 化分组）

凡 subject 的 receiver 表达式形如
`new XmlDocument().CreateElement(...)` / `.CreateAttribute(...)`
（即经 `CreateElement`/`CreateAttribute` 构造）的，**全部**在构造阶段即失败。

已确认 `RaiseNotSupported()` 出现在以下 receiver 构造路径：
`ChaosXmlDocumentCreateElement`、`ChaosXmlDocumentCreateAttribute`、
`ChaosXmlDocumentCreateNodeStr`、`ChaosXmlDocumentCreateDocumentType`、
`ChaosXmlDocumentCreateProcessingInstruction`、`ChaosXmlDocumentCreateXmlDeclaration`、
`ChaosXmlDocumentCreateEntityReference`、`ChaosXmlDocumentLoadXml`、
`ChaosXmlDocumentLoadStream`、`ChaosXmlDocumentSaveStream`、
`ChaosXmlDocumentCreateNavigator`、`ChaosXmlDocumentReadNode`、
`ChaosXmlDocumentValidate`、`ChaosXmlDocumentImportNode`、
`ChaosXmlNodeCreateNavigator` 等。

**注**：`CreateTextNode` / `CreateNode` **不是** NotSupported 桩（grep 计数 0），
故 `SplitText_1_int_0` 等不经此路径 —— 它们属 §3 的异常类名不匹配类。

---

## 3. 次要根因：native 异常类名与 real .NET 不符（~12 条）

**real .NET 8 实测**（`tmp_probe/Program.cs`）已确立 ground truth：

```
XmlTextReader.GetAttribute(null)          -> NullReferenceException
XmlTextReader.MoveToAttribute(null)       -> NullReferenceException
XmlDocument.CreateAttribute(null,null)    -> NullReferenceException
XmlDocument.CreateElement(null,null)      -> NullReferenceException
XmlDocument.CreateElement("root").GetElementsByTagName(null) -> ArgumentNullException
```

`xml_reader_stubs.cpp:735-739`：

```cpp
if (!ManagedStringView(name, aname, an_len))
    RaiseManagedException("System.ArgumentOutOfRangeException",
        "Value cannot be null. (Parameter 'name')");   // ← 应为 NRE
```

⇒ `GetAttribute(null)` / `MoveToAttribute(null)` 抛 AOORE，real .NET 抛 NRE，**类名不符**。

另：`ChaosXmlDocumentCreateNodeStr`（`:331-339`）对 `CreateNode("","","")` 抛
`NotSupportedException`，而 real .NET 抛 `ArgumentException`。

---

## 4. 逐条分组（35 条）

| 组 | 根因 | 条数 | 关键 file:line |
|:---|:-----|:----:|:---------------|
| **G1** | receiver 构造被 `RaiseNotSupported()` 阻断 | 构建期即失败，需逐条确认 | `xml_document_stubs.cpp:250` 等 18 处 |
| **G2** | native 异常类名不符（AOORE→NRE） | 12 | `xml_reader_stubs.cpp:735-739` |
| **G3** | `CreateNodeStr` 抛 NotSupported 而非 ArgException | 1(+2) | `xml_document_stubs.cpp:338` |
| **G4** | 待定（workflow 裁定中） | 余下 | — |

**形态分布**（读新生成体）：TYPED-CATCH **28** / AREQUAL **5** / BARE **2**
（全部 `caught=true, assertFailed=false, bodyAvailability=NativeGenerated`）。

---

## 5. 历史对照：31/35 是既有缺陷，非本次修复引入

按 `methodSubjectId` 对齐旧基线（`fact-results.stale.json`，`5fa746b86`）：

| 旧基线中的状态 | 条数 |
|:---------------|:----:|
| 已是 `realDefect` | **31** |
| 原为 `real` | 4 |

⇒ **这些是长期存在的缺陷，异常表修复只是让它们从"被 RC-1 掩盖"变为"可见"**。
不是回归。

---

## 6. 诚实标注

- §1、§2、§3 均为**一手证据**（读生成 C++ + 读 stub 源码 + real .NET 实跑）
- §4 的 G4 分组**尚未裁定**，workflow 的 3 条 lane + 对抗性证伪仍在跑
- `RaiseNotSupported()` 的存在是**架构性**的：DOM 构造未实现。修复 G1 等于
  **实现 XmlDocument DOM 构造**（属 roadmap Phase 3 范围），不是"改个异常类名"能解决的
- 因此 **P2 的真实工作量比 v5 初稿估计的更大**：G1 与 Phase 3 的
  「XmlDocument DOM（19 处 NotSupported）」是**同一件事**
