# STATUS — void writer 方法副作用断言

```yaml
task_id: void-writer-sideeffect-assertion
phase: brainstorming
lifecycle_status: in-progress
current_dir: docs/dev/in-progress/void-writer-sideeffect-assertion
创建日期: 2026-09-20
上游: json-xml-industrialization Phase 2 阻塞
关键文档:
  - problem-and-options.md
  - plan-v1-01.md

blocking_questions: []
question_clearance: cleared
clearance_confirmed_by_user: true
dispatch_model: sequential
recommended_next_child: Step-A
```

## 问题

JSON/XML 的写面 API 绝大多数返回 void，而 fact 的 `real` 判据是 `value != 42`
—— void 方法 `value` 恒为 sentinel 42，**永远无法成为 real**。

量化：`unassertable` 213 条（166 Void + 47 Task/ValueTask），
`Utf8JsonWriter` 104/115、`XmlTextWriter` 50/56。

**这是 JSON/XML 线特有的**（对比 reflection 0.7%、text-2 0%）。

## 核心发现：两条线性质完全不同（🔴 推翻原拍板前提）

| | XML writer | JSON writer |
|:--|:----------:|:-----------:|
| 写入操作（AppendStr/memcpy） | 大量 | **0** |
| 抛异常调用 | 少数（错误路径） | **38**（全部） |
| 内部输出缓冲 | ✅ `WriterState::buf/len` | ❌ 无 |
| **性质** | **已实现，缺断言** | **未实现** |

**证据**（生成代码确实调 native，但 native 无写入）：
```cpp
ChaosXmlWriterWriteCData(chaos_arg_0, chaos_arg_1);   // XML：真实写入
_s1 = 42; return _i1;                                  // 之后无条件返回 sentinel

void ChaosUtf8JsonWriterWriteStringStr(...) {          // JSON：直接抛
    (void)value;
    RaiseDisposedOrInvalid();
}
```

## 方案（XML 侧）

1. native 加 `ChaosXmlWriterGetOutput(handle)`（读已存在的 `WriterState::buf`）
2. shape 注册新接口
3. ATG 为 `XmlTextWriter` void 方法生成「调用后读回输出并断言」

## 待用户重新拍板

「XML + JSON 一起」的前提**已被推翻** —— JSON 侧是「从零实现 writer」，
与 XML 侧的「加断言」不是同一类工作，放一起会让范围严重失衡。

**建议**：本任务只做 XML 侧；JSON writer 实现独立立项。
