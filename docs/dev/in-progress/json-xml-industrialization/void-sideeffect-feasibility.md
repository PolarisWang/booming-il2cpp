# void 方法副作用断言 — 可行性调查（方案 A）

> 日期：2026-09-20
> 目的：验证「为 void 方法生成副作用断言」在技术上是否可行

## 1. 核心思路

void 方法无返回值 → 无法用 `value != 42` 判定。
但**有副作用** → 可以「调用前记录状态 → 调用 → 调用后比对」。

## 2. ✅ XML 侧：可行（基础设施已在）

`xml_writer_stubs.cpp` 的 `WriterState` **已维护完整输出缓冲**：

```cpp
struct WriterState {
    char*  buf;          // growable output buffer (NUL-terminated)
    size_t len;          // bytes used (excluding NUL)
    ...
};
```

**只是没有暴露读取接口** —— `xml_writer_stubs.h` 里全是 `Write*` 方法，
无 `GetOutput`。

**方案 A 在 XML 侧的最小实现**：
1. 新增 `ChaosXmlWriterGetOutput(handle) -> managed string`（读 `buf`）
2. ATG 为 void writer 方法生成：
   ```
   var w = new XmlTextWriter(sw);
   w.WriteCData("abc");           // 被测调用
   Assert.AreEqual("<![CDATA[abc]]>", GetWriterOutput(w));   // 副作用断言
   ```
3. 断言非空且符合预期 → 该 subject 可成为 `real`

## 3. ⚠️ JSON 侧：不可行（原因不同）

`json_writer_stubs.cpp` 的情况**根本不同**：

```cpp
void ChaosUtf8JsonWriterWriteStartObject(CHAOS_IL2CPP_INTPTR this_ptr) noexcept
{
    CheckThis(this_ptr);
    RaiseManagedException("System.InvalidOperationException", ...);   // ← 直接抛异常
}
```

**JSON writer 的核心写入方法本身未实现**（无内部缓冲，输出走
`IBufferWriter` 外部 sink，且当前整族直接抛 `InvalidOperationException`）。

**所以在 JSON 侧，方案 A 不是「加断言」的问题，而是「先实现 writer」的问题。**

## 4. 对 Phase 2 的影响（重要）

两条线需要**分开处理**：

| 线 | 现状 | 方案 A 的适用性 |
|:---|:-----|:----------------|
| **XML writer** | 有真实实现（内部缓冲） | ✅ 加读取接口 + ATG 生成断言即可 |
| **JSON writer** | 核心方法未实现 | ❌ 需先实现 writer，断言问题随之解决 |

**因此 Phase 2 的优先级应调整为**：
1. **先做 XML writer 的 void 断言基础设施**（成本低、收益直接）
2. **JSON writer 归入「实现」类工作**（不是断言问题）

## 5. 待确认（等并行调查）

- ATG 当前对 void 方法生成什么代码（是否有"不生成断言"的分支）
- void subject 的生成 C++ 是否真的调用了 native（决定它们是"实现了但没断言"
  还是"根本没实现"）

---

## 6. 并行调查结论（2026-09-20）

### 6.1 ATG 侧：确认「void ⇒ 不生成断言」

| 事实 | 位置 |
|:-----|:-----|
| 显式分支：确定性断言块被 `!result.IsVoid` 门控 | `TestEmitter.cs:546` |
| Probe 主动放弃确定性 | `ProbeEmitter.cs:673-678`：`!(method.IsVoid && !r1.HasException)`，注释明说 *"no observable side effect to verify across runs"* |
| void 检测 | `DllScanner.cs:725-726` → `Models.cs:31` |
| 无副作用捕获 | `AppendAssert` 只读 out/ref 参数（`TestEmitter.cs:645-677`） |
| 断言机制 | `Chaos.TestFramework.Assert.*`（`AreEqual`/`IsTrue`/`Throws`） |

### 6.2 生成代码：确实调用 native，但调用后无条件返回 42

```cpp
ChaosXmlWriterWriteCData(chaos_arg_0, chaos_arg_1);   // ✅ 真实调用（native 侧有完整实现）
_s1 = static_cast<CHAOS_IL2CPP_INTPTR>(42);           // ← 之后直接返回 sentinel
return _i1;
```

`bodyAvailability: "NativeGenerated"` —— 173/173 的 unassertable 都是这个值。

**结论**：这 166 个 void 方法**不是"未实现"，是"实现了但无法验证"**。

### 6.3 🔴 计划外发现：AOT 与 JIT 的判据不一致

同一 subject（`XmlTextWriterTests::WriteCData_9_string_0`）：

```
aot: value=42  resultKind=unassertable   ← sentinel 被识别
jit: value=0   resultKind=real           ← 未被识别
```

**JIT 段共 356 条** `resultKind=real` 且 `returnType=System.Void`。

**初步判断**：JIT 模式下方法**真实执行了托管代码**（`value=0` 而非 sentinel），
故判 `real` **可能是合理的** —— 但这与 AOT 的口径**不一致**，
需在报告中显式说明「AOT 与 JIT 的 real 数不可直接比较」。

⚠️ **未验证**：JIT 的 `real` 是否真的做了语义比较（而非"没抛异常就算 real"）。
若后者，则 JIT 的 `real` 也是虚高的。**这需要独立调查。**
