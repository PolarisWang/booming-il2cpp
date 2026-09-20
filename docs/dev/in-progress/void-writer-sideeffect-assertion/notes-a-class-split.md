# A 类切分的**前提被推翻**：JSON writer 零写入逻辑

> 日期：2026-09-20

## 🔴 关键发现

JSON writer 的 native 侧**没有任何写入逻辑**：

```
grep -c "AppendRaw|AppendStr" json_writer_stubs.cpp  →  0
```

**全部 37 个函数遵循同一模式**：

```cpp
void ChaosUtf8JsonWriterXxx(this_ptr, ...) noexcept {
    CheckThis(this_ptr);                          // this=0 → DisposedOrInvalid
    if (arg == 0) RaiseArgumentNullException(...); // 参数校验
    RaiseDisposedOrInvalid();                      // ← 统一：直接抛，不写入
}
```

**它们只复刻「bare object 的异常语义」** —— 因为 ATG 的 fixture 用
`SubjectInstanceFactory.Create<Utf8JsonWriter>()`（`GetUninitializedObject`），
而 .NET 对 bare writer 的调用**本来就抛异常**。

## 对 A 类切分的影响

我原本的 A1/A2 切分（「只需补注册」vs「需实现 native」）**前提不成立**：

- 原假设：native 有**真实实现**，只是某些重载没注册
- 实际：native **只有 bare-object 语义**，没有"真实写入"这回事

**所以「补注册」的价值需要重新评估**：

| 若补注册该重载 | 结果 |
|:---------------|:-----|
| native 函数接受该参数形态 | 复刻 bare-object 异常 → 期望若是异常则 PASS |
| native 函数不接受该参数形态 | 需**新写函数**，但新写的也只是复刻异常 |

**即：A 类的「实现」成本很低**（每个新变体 ~5 行，都是同一模式），
**但收益也有限**（只是让更多 subject 能"正确复刻异常"）。

## 修正后的切分

| 子类 | 含义 | 数量 |
|:-----|:-----|:-----|
| **A1. 有语义等价函数，补注册即可** | 如 `WriteStringValue(JsonEncodedText)` ← `WriteStringValueEncoded` | 4 |
| **A2. 需新写函数**（同一 bare-object 模式） | 如 `WriteNumber(JsonEncodedText, X)` | 27 |

**但 A2 的「新写」是低成本** —— 不是实现 JSON 序列化逻辑，而是
**再写一遍「参数校验 + RaiseDisposedOrInvalid」**。

## 💡 由此引出的真问题

**JSON writer 从始至终就没有真正的写入实现** —— 所有 subject 验证的都是
「bare object 拒绝写入」这一件事。这意味着：

1. **JSON 的写面 API 覆盖度被高估了** —— `real` 与否只反映"异常复刻是否对"
2. 真正要用 JSON writer（非 bare object）**完全不可用**
3. 但这**不是本任务（void 断言）的范围** —— 本任务只让 void 方法可验证，
   而它们验证的是「异常语义」，这已经是当前 fixture 能验的全部

## 建议

**A2 的 27 项暂不实现**，理由：
- 成本虽低（~135 行同模式代码），但**收益只是让更多 bare-object subject 通过**
- 它们**不增加任何真实写能力**
- 更有价值的是**记录这个事实**：JSON writer 无写入实现

**若要真正实现 JSON writer**，那是**独立的"实现"任务**（设计输出缓冲、
对接 IBufferWriter、处理转义/格式化），工作量远大于本任务。

## 已确认：XML 与 JSON 的 native 成熟度**完全不同**

| | XML writer | JSON writer |
|:--|:----------:|:-----------:|
| `AppendRaw`/`AppendStr` 次数 | **85** | **0** |
| 真实写入逻辑 | ✅ 有（维护 `WriterState::buf`） | ❌ 无 |
| native 函数在做什么 | 真实写入 + 参数校验 | 仅复刻 bare-object 异常 |

**即：**
- **XML writer**：有真实实现 → 本任务的副作用断言**有实际意义**（能验证输出）
- **JSON writer**：只有异常复刻 → 断言只能验证「异常语义对不对」

**我此前把两条线当同类处理（都做 typed catch）在"断言机制"层面是对的，
但在"验证价值"层面不对** —— XML 侧验的是真实行为，JSON 侧验的是异常复刻。

**这解释了为什么 A2 在 JSON 侧暴露的全是「异常类型不符」而非「输出错误」。**
