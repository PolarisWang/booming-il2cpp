# JSON / XML 翻译层 — 收口交接（XML 侧完成）

> **交接日期**: 2026-09-17
> **上一轮交接**: `HANDOFF-2026-09-15.md`（本轮起点）
> **本轮范围**: M5 XmlTextReader 收尾 → M6 XmlTextWriter → M3 Utf8JsonWriter → XML DOM 全族 → 非 async stubGap 清零
> **结论**: **XML 翻译层的非 async stubGap 已全部清零**；剩余 84 个 stubGap 100% 是 async 方法，需 async 主线能力
>
> ## 🔴 2026-09-17 后续更正（必读，覆盖下文多个结论）
>
> 本文写就后，同一日的工作推翻了其中两项判断，**下文表格中的 stubGap / real
> 数字均已失效**。执行下一轮前请以下表为准：
>
> | 项 | 本文原述 | 实测更正 |
> |:---|:---------|:---------|
> | XML chunk stubGap | 84 → 0 | **185** |
> | XML chunk realTotal | 338 | **234** |
> | XML chunk total | 659 | **746** |
> | text-json stubGap | 76 | **177** |
> | text-json realTotal | 51 | **5** |
>
> **两个原因**：
>
> 1. **84 个 `XmlWriter.*Async` 已实现**（commit `8391009af`）。但实测发现：ATG 的
>    fixture 是 `XmlWriter.Create(new StringBuilder())`，其 `Settings.Async == false`，
>    故正确语义是「参数校验通过后抛 `InvalidOperationException`」——**不是真异步写**。
>    这批随后落进 `unassertable` 桶（返回类型为 `Task`，value 天然无法承载断言）。
> 2. **发现并修复了一个更根本的 ATG 缺陷**（commit `def792ba3` + `dbddc7ec1`）：
>    `ProbeEmitter` 从不发射 `CSharpExpressionBuilder` 引用的 `ReflectionSubjectSample`
>    种子类型 → 任何含 **Type 参数**方法的类型，其 probe **必然编译失败**且**静默**
>    （`probe-results.json` 产出 2 字节的 `[]`，无任何报错）→ 该类型**全部**方法退化为
>    无保护的裸调用 → 断言 `(object)ret != null` 恒真 → **假绿**。
>
>    影响 **29 个类型、3 个 assembly**，全是地基类型（`System.Type`/`Attribute`/
>    `Enum`/`Delegate`/`Array`/`JsonSerializer` 等）。修复后重跑，四个 chunk 的
>    `stubGap` **全线上升**——那不是回归，是**假绿被揭为诚实的已知缺口**。
>
>    连带推翻了 reflection chunk 的「283/283 全绿」（其 216/283 落在失效类型下；
>    真相为 stubGap 390 / real 47 / total 440）。
>
> **因此：本文 §1 的「stubGap 清零」应读作「该轮口径下的清零」**，不代表真实覆盖率。

---

## 1. 本轮成果（可复算）

### 1.1 硬指标

| 指标 | 09-15 交接 | 现在 | 变化 |
|:-----|:----------:|:----:|:----:|
| **XML chunk stubGap** | **637** | **84** | **-553 (-87%)** |
| XML chunk realTotal | 229 | **338** | +109 (+48%) |
| XML chunk unverifiedSmoke | 700 | **280** | -420 (-60%) |
| **kCodegenFailureCount** | **17** ❌ | **0** ✅ | 硬门禁解除 |
| text-json stubGap | 188 | **76** | -112 (-60%) |

**复算方式**：
```bash
cat artifacts/foundation-dll/System.Private.Xml/chunks/xml/results/fact.json
# 关键字段: stubGap / realTotal / realVerified / unverifiedSmoke
```

### 1.2 已完全清零的 XML 类型

| 类型 | 09-15 stubGap | 现在 |
|:-----|:-------------:|:----:|
| XmlDocument | 72 | **0** |
| XmlNode / XmlElement / XmlAttribute | 76 | **0** |
| XmlCharacterData / XmlText / XmlCDataSection | 21 | **0** |
| XmlTextReader / XmlValidatingReader / XmlNodeReader | 39 | **0** |
| XmlWriter（同步面） / XmlTextWriter | 248 | **0** |
| XmlConvert | 61 | **0** |
| XmlNameTable / XmlNamespaceManager / 集合类 | 40 | **0** |
| **XmlWriter.\*Async** | 84 | **84** ← 全部剩余 |

### 1.3 提交清单（本轮，全部在 main）

| 提交 | 内容 | 效果 |
|:-----|:-----|:-----|
| `fcdddce06` | M5 XmlTextReader native + S19 | kCodegenFailure 17 的来源消除 |
| `98fc5042e` | `.ctor` ReturnAbi 改 Void | **17 → 0** |
| `2d9c3220f` | 恒0桩实现（namespace/lineInfo/LookupNamespace） | — |
| `60b024c25` | M6 XmlTextWriter 13 方法 | — |
| `f9e904d9c` | XmlTextWriter 异常语义一致性 | 40 stubGap → 0 |
| `9c7d7046f` | XmlWriter 抽象类 shape 注册 | — |
| `2ce5d78e7` | ATG XmlWriter 抽象类白名单 | 208 → 118 |
| `ea4810ff0` | WriteNode/WriteAttributes | — |
| `9638d3431` | ATG Xml DOM fixture 构造 | real 264 → 318 |
| `1b942da25` | XmlConvert native + S20 | 61 → 0 |
| `a9d93cee4` | DOM stubs + reader 残差 | 160 → 部分 |
| `585248f6a` | NameTable + NamespaceManager + 集合类 | — |
| `33de2fca9` | M3 Utf8JsonWriter native + S21 | text-json 114 → 24 |
| `3572fa456` | Utf8JsonWriter value-only 变体 | 99 → 76 |
| `ac9ed349e` | ValidatingReader/NodeReader shape | — |
| `a1c8a034e` | T2 XmlDocument 残差 shape | 72 → 28 |
| `64b475be7` | ParameterInfo MethodTable extern | 解 C2065 第一层 |
| `7dd6180b5` | ParameterInfo struct 无条件发射 | **解 C2065 链尾** |
| `dcab99b88` | XmlDocument 多参重载 | 28 → 12 |
| `ab37589db` | XmlNode XPath/Clone + XmlText.SplitText | 106 → 87 |
| `d81f8a646` | XmlWriterSettings.Clone + NamedNodeMap.GetEnumerator | 87 → 84 |

---

## 2. 🔴 给 async 主线的输入（本文件最重要部分）

### 2.1 剩余的 84 个 stubGap 全是 `System.Xml.XmlWriter` 的 `*Async` 方法

**27 种方法，84 个 subject 变体**：

```
DisposeAsync(1)              WriteAttributesAsync(3)      WriteBase64Async(5)
FlushAsync(1)                WriteBinHexAsync(5)          WriteCDataAsync(2)
WriteCharEntityAsync(2)      WriteCharsAsync(5)           WriteCommentAsync(2)
WriteDocTypeAsync(5)         WriteElementStringAsync(5)   WriteEndDocumentAsync(1)
WriteEndElementAsync(1)      WriteEntityRefAsync(2)       WriteFullEndElementAsync(1)
WriteNameAsync(2)            WriteNmTokenAsync(2)         WriteNodeAsync(6)
WriteProcessingInstructionAsync(4)  WriteQualifiedNameAsync(4)  WriteRawAsync(6)
WriteStartDocumentAsync(3)   WriteStartElementAsync(5)    WriteStringAsync(1)
WriteSurrogateCharEntityAsync(3)  WriteWhitespaceAsync(2)
WriteAttributeStringAsync(5)
```

### 2.2 为什么它们不能靠 stub 解决（已证实的判断）

这 84 个 subject 的 ATG 生成形态是：

```csharp
public long WriteStringAsync_66_string_1()
{
    // AOT-STUB-GAP
    // [UNVERIFIED] AOT stub: System.InvalidOperationException thrown by
    //   new XmlTextWriter(new StringWriter()).WriteStringAsync("...")
    return 42L;
}
```

`value=42` = **ATG 在编译期写死的 smoke stub**，**根本不调用 native**。
无论 native 侧怎么实现、白名单怎么加，都不会改变它。

**证伪依据**：本轮的 XmlWriter *同步* 方法（同名、同类型、同样裸对象）通过
「native 实现 + S20 注册 + ATG 白名单」三件套**全部转 real**（40→0、208→118→84）。
同样的三件套对 `*Async` 无效 —— 差异只在 `Async` 后缀。

### 2.3 它们真正需要什么

`WriteXxxAsync` 返回 `Task`/`ValueTask`，ATG 无法为它们生成断言（返回类型是 async 状态机），
所以永远走 smoke 分支。要让它们变成可断言/可验证，需要：

| 需求 | 归属 |
|:-----|:-----|
| async 方法在 ATG 里能被识别为「可等待并断言」 | ATG 层 |
| `Task`/`ValueTask` 的 native 实现（当前 `ChaosExternalRuntimeFallback`） | async 主线 Phase 1-3 |
| async 状态机 lowering（`WriteStringAsync` 本身是 `async` 方法） | async 主线 Phase 2 |

**具体请求**：async 主线在完成 Phase 2（状态机翻译引擎）后，回来复跑
`System.Private.Xml/xml` chunk，看这 84 个是否自然转 real。如果 ATG 侧仍需调整，
这是需要联合定位的点。

### 2.5 🔴 为什么「继续加 stub」不再有收益（本轮末尾实测得出）

本文件 §1 的成果来自「native 实现 + shape 注册 + ATG 白名单」三件套。**该模式已到边界**。
xml chunk 的 697 个 subject 里，real 只有 224，其余 473 个分布在 6 个桶，**每一个的卡点都不在 native 实现**：

| 桶 | 数量 | 卡点 | 加 stub 有用吗 |
|:---|:----:|:-----|:--------------:|
| `unassertable` | 103 | void 方法，`value=42` 是结构性的（非缺口） | ❌ |
| `failed` | 114 | ATG 生成形态与 fact 判定的交互 | ❌ |
| `smoke` | 93 | **59 个是对象返回类型**（native 无法构造 managed 对象图）；34 个是标量 | ❌（59 个） |
| `stubGap` | 84 | async（§2.1-2.3） | ❌ |
| `realDefect` | 44 | 真实执行了，但结果与托管期望有差异 | ❌ |
| `factoryGap` | 35 | 裸对象路径（`SubjectInstanceFactory.Create<T>` 返 null） | ❌ |

**`failed` 的实证**（以 `XmlDocument.Load` 为例）：

```csharp
// ATG 生成（期望"调用必抛异常"）
try { new System.Xml.XmlDocument().Load(default(string)!); throw new System.Exception("AOT stub did not throw"); } catch { }
return 42L;
```
```json
// fact 记录
{"value": 0, "caught": true, "assertFailed": false, "resultKind": "failed"}
```

`caught=true` 说明 **native 确实抛了异常**（行为正确），但 runner 因异常逃逸到顶层而记
`passed=false`。`classify_fact_record` 因缺 shape 元数据落到末尾的 `return "failed"`。
**这不是 native 缺陷** —— 实现真实 DOM 树不会改变这些记录的分类。

**`smoke` 的实证**：59 个的 `returnType` 是 `XmlNode`/`XmlElement`/`XmlAttribute`/
`XmlNodeList`/`XPathNavigator` 等**对象类型**。native 层无法构造真实的 managed 对象图
（那需要解释器或完整 managed DOM 实现），所以这些 subject 永远拿不到可断言的值。

**结论**：XML 线的下一步需要**跨层能力**（async 基础设施 / managed 对象构造 / ATG-fact
分类对齐），不在 stub 层。

### 2.4 验证命令（供 async 主线复跑）

```bash
cd D:/agent/chaos-il2cpp
# 清 3 层缓存
rm -rf artifacts/foundation-dll/System.Private.Xml/chunks/.hephaestus-cache \
       tests/e2e/translation/System.Private.Xml/chunks/.hephaestus-cache \
       testing/foundation-dll/System.Private.Xml/.hephaestus-cache \
       artifacts/foundation-dll/System.Private.Xml/chunks/xml/{managed,results,native,build_jit_output} \
       tests/e2e/translation/System.Private.Xml/chunks/xml/managed/.autogen
# 跑
CHAOS_FOUNDATION_DLL=$(pwd)/testing/foundation-dll PYTHONPATH=$(pwd)/tests/e2e \
  python tests/e2e/verification/chunk_pipeline.py \
  --assembly System.Private.Xml --chunk xml --stages build,fact --native-config check
# 期望: stubGap 从 84 下降
```

---

## 3. 本轮踩过的 6 个真坑（避免重踩）

### 坑 1：`.ctor` shape 的 ReturnAbi 必须是 Void 🔴 最贵的一个

**症状**：`kCodegenFailureCount=17`，pipeline 硬门禁 FAIL，17 个 `XmlTextReaderTests` subject 全变 stub。

**根因**：`NativeAotLoweringPlanner.ExceptionEmission.Linear.cs:525` 硬性要求构造方法
`ReturnAbi.CarrierKindCode == Void`。S19 把 `.ctor` 注册成 `CreateNativeIntAbiSlot()`（INTPTR）
→ 抛 `NotSupportedException` → 被 `BuildMethodSourceSafe` 的泛化 catch 截获 → 计数 +1。

**修法**：`CreateNativeIntAbiSlot()` → `CreateVoidAbiSlot()`（1 行）。

**为什么难查**：pipeline 的 `build.py:975` 在 JIT 成功路径上**只打印 stdout**，
codegen 的 stderr（含 `[codegen] WARNING: codegen failed for <SubjectId>, Root cause: ...`）
被丢弃。要看到必须**直接跑 TPG 并捕获 stderr**：
```bash
dotnet exec <TPG.dll> generate-dll --jit --dll <subjects.dll> --metadata <meta.json> \
  --output /tmp/x --config-tier check 2>&1 | grep -E "CODGEN-FAIL|codegen failed"
```

### 坑 2：`chaos_mt_` 与 `chaos_type_` 是**两个**符号面

**症状**：`chaos_reflection_get_parameters_b3` 引用 `ParameterInfo`，报 C2065 → C2061 → C3536 → C2440 连锁。

**根因**：`EmitMethodParameterNameCase` 同时用
- `GetNativeTypeInfoSymbol()` → `chaos_mt_X.AsTypeInfoHot()`（MethodTable）
- `GetNativeTypeSymbol()` → `chaos_type_X`（**完整 struct**，`NEW_GC` 要 `sizeof`）

**只补 `chaos_mt_` 的 extern 声明不够** —— `chaos_type_X` 需要 object-model 阶段发射的
完整 struct（含 `runtime_name_value` 字段，见 `GcTypeLayout.HasHardcodedGcRefs`）。

**修法**（两处，分别在 `64b475be7` 与 `7dd6180b5`）：
1. `RegisterExtraMethodTableSymbol` → `_extraMethodTableSymbols`（MethodTable extern）
2. `EmitObjectModelDeclarations` 里**无条件** `TrackReferenceType(ParameterInfo)`
   （因为该函数在 `CollectReflectionMemberMetadataFromClosure` **之前**跑，
   无法预知闭包是否有带参方法）

### 坑 3：shape 键 = (type, method, paramTypes)，多参重载必须独立注册

`CreateElement(string)` 和 `CreateElement(string,string)` 是**不同的 shape 键**。
只注册单参版本 → 多参 subject 全部落到 fallback。

且 native 侧需要**独立符号**（`ChaosXmlDocumentCreateElement2` / `...3`）——
用同一个 1 参符号会让 codegen 按多槽 ABI 生成调用 → `C2660: function does not take N arguments`。

### 坑 4：ATG 白名单与 shape 注册是**两件事**

- **shape 注册**（S20/S21）→ codegen 知道怎么调 native
- **ATG 白名单**（`TestEmitter.ExternalStubRaisesManagedException`）→ ATG 生成
  `try { call(); throw; } catch { }` 而非 `return 42L`

**只做前者，stubGap 不动**。两者必须配对。

### 坑 5：并行 agent 污染 —— 提交前必须逐文件核对

本会话发生过两次：
- 提交时被并行 agent 的 `math_stubs.cpp/h` 混入（需 `git reset --soft` 重做）
- `async_stubs.cpp`、`exception_helpers.cpp`、`S24.cs` 出现并行 agent 的半成品编译错误

**每次提交前**：
```bash
git add <显式文件列表>          # 禁 git add -A
git diff --cached --name-only    # 逐行核对
```

### 坑 6：TPG 捆绑自己的 `Chaos.IL2CPP.Generator.dll`

改 Generator 后**必须重建 TPG**，否则 TPG 用旧的 Generator.dll 跑。
同理 native `chaos_runtime_core.lib` 在 SDK 里也有一份副本。

---

## 4. 未验证 / 不诚实项（必须明说）

| 项 | 状态 |
|:---|:-----|
| `d81f8a646` 的 pipeline 端到端 | ✅ 已跑（stubGap 84 确认），但当时 ATG DLL 曾被并行 agent 锁定 |
| text-json 的最新指标 | ✅ 已复跑（2026-09-17）：stubGap **76**、failed **0**、realVerified **51/51 (100%)** |
| 84 个 async 是否「只需 Phase M」 | ⚠️ **推断**，未实测。依据是同步面全部转 real 而 async 面不动，但未直接验证 ATG 的 async 分类逻辑 |
| 其余 3 个 chunk（System.Xml.ReaderWriter 等） | ❌ 本轮未覆盖 |

**不要假设 §2.2 的推断已被证实** —— 它是强推论而非实测结论。

---

## 5. 交接给下一轮的建议

### 如果继续 JSON/XML 线

先读 **§2.5** —— 那 6 个桶的卡点全在跨层，stub 模式已到边界。

**真正有空间的方向（按预估收益排序）**：

| 方向 | 收益 | 卡点 | 是否建议 |
|:-----|:-----|:-----|:--------:|
| **M1 Utf8JsonReader** | 全新读面，不受既有桶限制 | 需 span 支持（重） | ✅ 最高价值 |
| **ATG-fact 分类对齐** | 解放 failed 114 + smoke 34 标量 | 需理解 `classify_fact_record` 与 `has_codegen_shape` 的缺元数据分支 | ✅ 中 |
| **M4 Load/Save 真 DOM 树** | 对象返回 smoke 59 的一部分 | native 无法构造 managed 对象图 | ⚠️ 需解释器配合 |
| 剩余 3 个 XML chunk | 未知 | 未探索 | ❓ |

### 如果转 async 主线

见 §2 —— 84 个方法的清单与验证命令已备好。
⚠️ **转入前先确认 4 个 async worktree 的归属**（`async-t3` 处于 locked 状态，
说明有 agent 正在工作）。

---

## 6. 签名

> **交接人**: Claude Code（会话 `chaos-il2cpp`，2026-09-15 ~ 09-17）
> **本轮性质**: XML 翻译层收口。**非 async 部分已清零**，剩余 84 个全部需要 async 能力。
> **诚实标注**: §2.2 的「84 个只能等 Phase M」是**强推论**，不是实测结论；
> §2.5 的「6 个桶都到边界」是本轮**实测**得出（含 Load/Save 与 smoke 对象返回的
> 具体证据）；§4 列出了全部未验证项。
>
> ⚠️ **本轮末尾自我更正**：我在会话中曾建议做 M4（XmlDocument Load/Save 真实 DOM），
> 实测后发现 **Load/Save 已在真实执行**（`caught=true`），卡点是 ATG/fact 分类而非
> native 实现 —— 该建议已被 §2.5 推翻。**不要按旧建议做 M4。**
>
> ```
> ——— 2026-09-17 / commit 83cfc091c ———
> ```
