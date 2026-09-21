# void writer 方法副作用断言 — 设计

> 日期：2026-09-20
> 上游：`json-xml-industrialization` Phase 2 阻塞（`phase2-blocker-void-assertability.md`）
> 用户拍板：范围 = XML + JSON；ATG 改动 = 限定 writer 类

---

## 1. 问题

JSON/XML 的**写面 API 绝大多数返回 void**（`WriteCData`/`WriteString`/
`WriteNumber*`…）。而 fact 的 `real` 判据是 **`value != 42`** ——
void 方法 `value` 恒为 sentinel 42 → **永远无法成为 `real`**。

### 量化

| 事实 | 数值 |
|:-----|:-----|
| `unassertable` 总数（JSON/XML 线） | **213** |
| 其中 `System.Void` | **166** |
| 其中 `Task`/`ValueTask` | 47 |
| `Utf8JsonWriter` 的 `unassertable` | 104 / 115 |
| `XmlTextWriter` 的 `unassertable` | 50 / 56 |

**这不是"未实现"** —— 证据（§2）。

### 对比：这是 JSON/XML 线特有

| chunk | unassertable 占比 |
|:------|:----------------:|
| **System.Net.Sockets** | 35.0% |
| **Xml/xml** | **23.2%** |
| CoreLib/text | 13.3% |
| threading | 12.6% |
| **reflection** | **0.7%** |
| **text-2** | **0.0%** |

---

## 2. 证据：它们是"实现了但无法验证"

### 2.1 生成代码确实调用了 native

```cpp
ChaosXmlWriterWriteCData(chaos_arg_0, chaos_arg_1);   // ✅ 真实调用
_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(42);           // ← 之后无条件返回 sentinel
return _i1;
```

`bodyAvailability: "NativeGenerated"`（173/173 的 unassertable 都是）。

### 2.2 native 侧有真实实现

`xml_writer_stubs.cpp` 的 `WriterState` **已维护完整输出缓冲**：
```cpp
struct WriterState {
    char*  buf;          // growable output buffer (NUL-terminated)
    size_t len;          // bytes used (excluding NUL)
```
**只是没有暴露读取接口。**

### 2.3 ATG 有显式的「void ⇒ 不生成断言」分支

| 事实 | 位置 |
|:-----|:-----|
| 确定性断言块被 `!result.IsVoid` 门控 | `TestEmitter.cs:546` |
| Probe 主动放弃确定性 | `ProbeEmitter.cs:673-678`（注释：*"no observable side effect to verify across runs"*） |
| 无副作用捕获 | `AppendAssert` 只读 out/ref 参数（`TestEmitter.cs:645-677`） |
| 断言机制 | `Chaos.TestFramework.Assert.*` |

---

## 3. 方案：调用后读回副作用并断言

### 3.1 核心思路

```
调用前：记录 writer 输出（基线）
调用：  被测 void 方法
调用后：再次读取输出 → 断言"新增内容符合预期"
```

### 3.2 XML 侧（基础设施已在，改动小）

1. **native**：新增 `ChaosXmlWriterGetOutput(handle) -> managed string`
   —— 读 `WriterState::buf`，返回托管字符串
2. **shape 注册**：把新接口注册为可调用的 native shape
3. **ATG**：为 `XmlTextWriter` 的 void 写入方法生成：
   ```csharp
   var w = new XmlTextWriter(new StringWriter());
   w.WriteCData("abc");
   Assert.AreEqual("<![CDATA[abc]]>", GetXmlWriterOutput(w));
   ```
4. 断言通过 → 该 subject 成为 `real`

### 3.3 ⚠️ JSON 侧（此节结论**已被 §7 推翻**，保留作纠错记录）

**初版判断错误**（曾写「JSON writer 大部分已实现，只缺 4 个容器方法」）——
那是基于 `grep '函数总数 37 vs RaiseManagedException 7'` 的**误判**：
很多函数内部调用的是 `RaiseDisposedOrInvalid()` 而非直接 `RaiseManagedException`。

**spike 实测结论**：

```cpp
void ChaosUtf8JsonWriterWriteStringStr(...) noexcept
{
    CheckThis(this_ptr);
    ...
    (void)value;
    RaiseDisposedOrInvalid();     // ← 直接抛异常，**没有写入任何东西**
}
```

| 指标 | JSON writer | XML writer |
|:-----|:-----------:|:----------:|
| 写入操作（`AppendStr`/`memcpy`） | **0** | 大量 |
| 抛异常调用 | **38** | 少数（仅错误路径） |
| 内部输出缓冲 | ❌ 无 | ✅ `WriterState::buf/len` |
| **性质** | **未实现** | **已实现，缺断言** |

**因此 JSON 侧与 XML 侧的工作性质根本不同**：

- **XML**：加读取接口 + ATG 生成断言 → 让已实现的方法可验证
- **JSON**：**先实现整个 writer**（含输出缓冲设计）→ 断言问题随之解决

⚠️ **这推翻了「XML + JSON 一起」的前提** —— 两者不是同一类工作，
放同一立项会让范围严重失衡。**需用户重新拍板。**

---

## 4. 边界与非目标

**In scope（XML）**：
- `XmlTextWriter` 的 void 写入方法（约 50 个）
- ATG：为 writer 类生成副作用断言

**待定（JSON）**：
- `Utf8JsonWriter` 需**先实现**（约 104 个），断言是后续问题
- 建议**独立立项**，不并入本任务

**非目标**：
- `Task`/`ValueTask` 返回的 async 方法（47 个）—— 属 async 主线
- 非 writer 类的 void 方法（如 `XmlDocument.AppendChild`）—— 副作用不明确
- 不改 fact 的 `real` 判据本身

---

## 5. 风险

| 风险 | 影响 | 缓解 |
|:-----|:-----|:-----|
| **输出格式细节差异** | 断言过严导致假失败 | 用 .NET 8 实测作基准（如本日的 WriteEndDocument 修复） |
| **JSON 输出去向不明** | 读不到输出 | 立项后先做 spike 确认 |
| **ATG 改动波及面** | 影响其他 chunk | 限定 writer 类；未匹配者走原路径 |
| **AOT/JIT 口径不一致**（356 条） | 数字难比较 | 本任务不处理，单独记录 |

---

## 6. 待确认（blocking）

| # | 问题 | 状态 |
|:-:|:-----|:-----|
| 1 | ~~JSON writer 是否未实现~~ | ✅ **已澄清**：是 bare-object 语义复刻，native 行为正确（见 §7） |
| 2 | JSON 侧断言什么？ | ✅ **已明确**：断言「抛 `InvalidOperationException`」（bare writer 的托管契约） |
| 3 | ATG 如何识别"这是 writer 类"？ | ✅ `TestEmitter.cs:985-1030` **已有**显式列表 |
| 4 | 断言的期望值从哪来？ | ⚠️ **未定**：probe 记录 vs .NET 8 实测 |
| 5 | XML 侧输出读取接口的实现细节 | ⚠️ 未定 |

**仍有 2 项未清 → 不得进入 writing-plans。**

---

## 7. 🔴 重大修正：JSON 侧不是「未实现」，是「bare object 语义复刻」

**本文档 §3.3 的「JSON 未实现」结论错误** —— 第二次判断失误。

### 根因：ATG fixture 用 `GetUninitializedObject` 造对象

`TestEmitter.cs:985-992` 的注释是决定性的：

> *"ATG subjects construct this type through `SubjectInstanceFactory.Create<Utf8JsonWriter>()`
> — a **bare** `GetUninitializedObject` instance whose instance methods throw
> ObjectDisposedException / InvalidOperationException / ArgumentNullException
> from the managed implementation. The native stubs (json_writer_stubs.cpp)
> **replicate those contracts**."*

### .NET 实测验证（本日）

```csharp
var w = (Utf8JsonWriter)RuntimeHelpers.GetUninitializedObject(typeof(Utf8JsonWriter));
w.WriteString("k", "v");
// → InvalidOperationException: Cannot write a JSON property within an array
//   or as the first JSON token. Current token type is 'None'.
```

**对 bare writer 调用 WriteString 本来就抛 `InvalidOperationException`。**

**native stub 的行为是正确的** —— 它在复刻 .NET 的 bare-object 语义。

### 结论：JSON 与 XML 的性质**其实相同**

| | XML writer | JSON writer |
|:--|:----------:|:-----------:|
| ATG fixture | `new XmlTextWriter(new StringWriter())`（**已初始化**） | `GetUninitializedObject`（**bare**） |
| .NET 语义 | 正常写入 | 抛 `InvalidOperationException` |
| native 行为 | 真实写入 | 复刻抛异常 |
| **性质** | 已实现，缺断言 | **同样"实现"了（复刻语义），缺断言** |

**两条线的工作性质相同**：都是「fixture 构造方式决定了语义，native 已复刻，
但 void 返回无法断言」。

### 我的两次判断失误记录

| 次序 | 结论 | 错因 |
|:----:|:-----|:-----|
| 1 | "JSON 大部分已实现" | `grep '函数总数 vs RaiseManagedException'` 计数误判（内部调 `RaiseDisposedOrInvalid`） |
| 2 | "JSON 未实现" | 没看 ATG 的 fixture 注释，误把"复刻异常语义"当成"没实现" |

**教训**：判断实现程度**不能只看 native 代码有无"写入操作"** ——
必须结合 **ATG 的 fixture 构造方式**判断期望语义。

---

## 8. 方案拍板（2026-09-20，用户确认）

### 8.1 Blocker 1 清零：期望值来源 = 扩 probe 自动捕获

**已确认可行**：`ProbeEmitter.cs` 已有 `serializableOutRefs` / `_outValues`
捕获机制（`:250-256`），可**复用同一模式**扩展为捕获 writer 输出。

```csharp
// ProbeResult 现有字段（Models.cs:44-55）
bool HasException,
string? ExceptionType,        // ← JSON 侧断言直接用这个
IReadOnlyList<string>? OutRefValues  // ← 扩展基础
```

**JSON 侧**：`HasException` + `ExceptionType` **已足够**（断言"抛 InvalidOperationException"）
**XML 侧**：需扩展捕获「调用后的 writer 输出文本」

### 8.2 Blocker 2 清零：新增 native 读取接口

**用户拍板**：新增 `ChaosXmlWriterGetOutput(handle) -> managed string`

**约束**（`xml_writer_stubs.cpp:15` 注释）：
> *"the buffer is owned here; **recovery of the text is out of scope** for the write-path subset"*

→ 该接口是**新增能力**，需实现：
1. 读 `WriterState::buf`（已存在，含完整输出）
2. 转成托管字符串返回（参考 `alloc_string` / `ChaosStringCreateFromUtf8`）

### 8.3 最终方案

| 侧 | 断言什么 | 期望值来源 | native 改动 |
|:---|:---------|:-----------|:-----------|
| **XML** | 调用后的输出文本 | 扩 probe 捕获 | 新增 `GetOutput` |
| **JSON** | 抛出的异常类型 | `ProbeResult.HasException/ExceptionType`（已有） | 无 |

### 8.4 边界

**In scope**：
- `XmlTextWriter` 的 void 写入方法（~50 个）
- `Utf8JsonWriter` 的 void 方法（~104 个）
- ATG：为这两类生成副作用断言（复用 `TestEmitter.cs:985-1030` 已有的识别列表）

**非目标**：
- `Task`/`ValueTask` 返回的 async 方法（47 个）—— 属 async 主线
- 非 writer 类的 void 方法
- 不改 fact 的 `real` 判据本身

---

## 9. 问题清零

### blocking_questions: ✅ 已清零

| # | 问题 | 结论 |
|:-:|:-----|:-----|
| 1 | JSON 是否未实现 | ✅ bare-object 语义复刻，native 正确（§7） |
| 2 | JSON 断言什么 | ✅ 抛出的异常类型 |
| 3 | ATG 如何识别 writer 类 | ✅ `TestEmitter.cs:985-1030` 已有显式列表 |
| 4 | 期望值从哪来 | ✅ 扩 probe 自动捕获（§8.1） |
| 5 | XML 输出读取 | ✅ 新增 `ChaosXmlWriterGetOutput`（§8.2） |

### watch_items

- W1：JSON 侧的 `WriteTo(Utf8JsonWriter)` 是 `JsonDocument/JsonElement` 的方法，
  期望 `ArgumentNullException` —— 与 writer 自身方法的期望**不同**，需分别处理
- W2：AOT/JIT 口径不一致（356 条）—— 本任务不处理，已记录
- W3：probe 扩展会改变 `ProbeResult` schema —— 需确认下游兼容

## 10. 下一步入口

**blocking_questions = [] → 可进入 `writing-plans`**
