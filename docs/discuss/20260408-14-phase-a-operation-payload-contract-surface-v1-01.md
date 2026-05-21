# Phase A Operation Payload Contract Surface v1.01

Date: 2026-04-08
Status: design-discuss

## 1. 问题点

上一轮我们已经把两件事基本定住了：

- operation 名字保持在 semantic-action 级，不向 opcode 回退，也不向 lowering family 泄漏
- payload 可以保留，但禁止做成 bag-shaped payload，推荐 typed payload

那么下一步最自然的问题就不再是：

- “要不要 payload”

而是：

- **首轮 canonical operation payload contract，到底应该冻结到什么字段级别**

如果这一步不继续收口，后面很容易出现新的漂移：

1. `SemanticWorld` 输出的 operation 只是“有个 kind”，但 payload 细节各写各的
2. `CodeGen` 为了消费方便，自己重新发明“隐含字段”
3. 有的 operation 直接内联大数组，有的 operation 用引用表，结构越来越不统一
4. 最后虽然表面上大家都叫 `typed payload`，实际上 contract 仍然不稳定

所以这一轮真正要回答的是：

- **operation 本体应该统一携带哪些 fixed fields**
- **哪些信息必须放入 kind-specific typed payload**
- **哪些可变长信息必须统一下沉到 `SpanId` / side-table**

## 2. 这里讨论的不是哪件事

这里讨论的不是：

- 最终 C# 类型定义长什么样
- 序列化 JSON 长什么样
- C++ emitter 最终模板怎么消费
- 内存布局是否必须一开始就极限压缩

这里讨论的是：

- **`SemanticWorld -> CodeGen` 之间的 canonical payload surface**

也就是：

- 哪些字段是所有 operation 都有资格带的
- 哪些字段必须由具体 operation 自己声明
- 哪些字段绝不应该混进统一 header

## 3. 三种可选方案

## 3.1 方案 A：统一大 header，差异尽量塞 nullable 槽位

### 方案描述

做法是让所有 operation 共用一个较大的统一结构，例如都预留：

- `PrimarySubjectId`
- `SecondarySubjectId`
- `LiteralRef`
- `MemberKind`
- `OperandSpanId`
- `Flags`
- `ResultRef`

然后不同 operation 只使用其中一部分字段。

### 优点

- 看起来最统一
- 消费端可以用一个 record 直接遍历
- 实现早期比较省事

### 缺点

- “字段有但经常不用”会很快把语义边界做模糊
- 同一个槽位在不同 operation 中含义会漂移
- 很多空字段会让结构臃肿
- 后面新增 operation 时，常常会继续想往大 header 里塞新槽位

### 我的判断

- **不推荐作为主线**

它会让 contract 表面统一，实则语义越来越松。

## 3.2 方案 B：小型固定 header + kind-specific typed payload + shared spans

### 方案描述

operation 本体只保留非常小、非常稳定的公共头，例如：

- `OperationId`
- `Kind`
- `ResultRef?`
- `PayloadRef`

具体语义细节进入各自的 typed payload：

- `ConstructObjectPayload`
- `ReadInstanceFieldPayload`
- `InvokeInstanceMethodPayload`
- `QueryClosedTypeMemberPayload`

所有可变长输入统一下沉到：

- `OperandSpanId`
- `InputSpanId`
- 或其他 side-table 引用

### 优点

- header 语义稳定，不容易继续膨胀
- kind-specific contract 清晰
- `CodeGen` 消费时不需要猜字段含义
- 可变长数据不会破坏 operation 本体的紧凑性

### 缺点

- 需要维护一套更明确的 payload schema
- 部分 operation 之间不能再假装“字段都差不多”

### 我的判断

- **推荐**

这是当前最稳的收敛方式。

## 3.3 方案 C：极端归一化，只保留 operation 到多个 bundle 的引用

### 方案描述

做法是再进一步，把 payload 也拆散成大量 bundle：

- `SubjectBundleRef`
- `OperandBundleRef`
- `QueryBundleRef`
- `LiteralBundleRef`

operation 本体只保留对这些 bundle 的引用。

### 优点

- 结构复用潜力高
- 对未来极端规模可能更友好

### 缺点

- 抽象层次过高
- 调试和阅读成本明显变差
- 对 Phase A 来说很容易过度设计

### 我的判断

- **不建议现在就走到这一步**

这更像方案 B 的远期演进，不是当前最需要的稳定面。

## 4. 我的建议

如果这一轮要拍板，我建议：

- **采用方案 B：小型固定 header + kind-specific typed payload + shared spans**

一句话定义：

- 公共头只放“所有 operation 都稳定成立的信息”
- 语义差异放入 typed payload
- 可变长输入统一放引用表，不内联肥数组

## 5. 我建议冻结的最小公共 header

当前 `Phase A` 我建议 operation 公共头只保留下面几项：

- `OperationId`
  - 稳定标识当前 operation，便于诊断、引用、排序
- `Kind`
  - canonical semantic action 名字
- `ResultRef?`
  - 如果这个 operation 产出值，则给出结果引用；无结果则为空
- `PayloadRef`
  - 指向该 kind 对应的 typed payload

我当前**不建议**一开始就把下面这些字段放进所有 operation 的公共头：

- `MemberKind`
- `LiteralRef`
- `OperandSpanId`
- `SourceRange`
- `Flags` 大杂烩
- `PrimarySubjectId / SecondarySubjectId`

原因很简单：

- 这些字段并不是所有 operation 的稳定共性
- 一旦进了 header，后面就很容易被滥用成“万能槽位”

## 6. 我建议的 payload 分工原则

## 6.1 公共头回答“这是什么动作、产出什么值”

例如：

- 这是 `construct-object`
- 它产出了一个 `ResultRef`

## 6.2 typed payload 回答“这个动作具体作用到谁”

例如：

- 构造的目标类型是谁
- 调用的方法是谁
- 读取的字段是谁
- 查询的 member kind 是什么

## 6.3 side-table / span 回答“这个动作涉及哪些可变长输入”

例如：

- 调用参数列表
- `string-concat` 的输入列表
- 某些 query 的输入序列

这样三层分工会比较干净：

- header 讲动作身份
- payload 讲动作语义参数
- span/side-table 讲可变长依赖集合

## 7. 用几个 operation 看具体长什么样

## 7.1 `load-const-string`

### 公共头

- `OperationId`
- `Kind = load-const-string`
- `ResultRef`
- `PayloadRef`

### typed payload

```text
LoadConstStringPayload {
  LiteralRef
}
```

这里不需要单独再引入 span。

## 7.2 `construct-object`

### 公共头

- `OperationId`
- `Kind = construct-object`
- `ResultRef`
- `PayloadRef`

### typed payload

```text
ConstructObjectPayload {
  TargetTypeId
  CtorMethodId
  ArgumentSpanId
}
```

这里 `ArgumentSpanId` 统一承载实参数量变化。

## 7.3 `read-instance-field`

### typed payload

```text
ReadInstanceFieldPayload {
  ReceiverRef
  FieldId
}
```

这里我更倾向于把 `ReceiverRef` 保持为显式字段，
而不是塞进通用 `OperandSpanId`，因为 receiver 在语义上不是普通“第一个参数”，而是实例语义的一部分。

## 7.4 `invoke-instance-method`

### typed payload

```text
InvokeInstanceMethodPayload {
  ReceiverRef
  MethodId
  ArgumentSpanId
}
```

这里同样建议：

- receiver 显式
- 其他 arguments 进 span

这样比“所有输入都压成一个未分角色的 operand list”更清楚。

## 7.5 `query-closed-type-member`

### typed payload

```text
QueryClosedTypeMemberPayload {
  SubjectTypeId
  MemberKind
  QueryKeyId
}
```

这里 `MemberKind` 是小枚举，适合留在 typed payload，
不需要升级为新的 operation 名字，也不应该放进全局 header。

## 7.6 `string-concat`

### typed payload

```text
StringConcatPayload {
  InputSpanId
}
```

不同输入个数、未来不同 lowering 方式，都不应影响 operation 名字本身。

## 7.7 `return`

### typed payload

```text
ReturnPayload {
  ValueRef?
}
```

无返回值时为空，有返回值时显式给出。

## 8. 我建议明确冻结的几个 contract 原则

## 原则 1：公共头不承载“语义角色差异”

例如：

- receiver
- field target
- queried member kind

这些都不应进入统一 header。

## 原则 2：可变长输入一律引用化

例如：

- 调用参数
- concat 输入

统一进入 `SpanId` / side-table，而不是内联数组。

## 原则 3：payload 主要承载 canonical id、小枚举、稳定 ref

应该优先放：

- `TypeId`
- `MethodId`
- `FieldId`
- `MemberKind`
- `ValueRef`
- `LiteralRef`
- `SpanId`

不应该直接放：

- 富对象
- metadata object
- runtime-specific descriptor
- backend-specific helper descriptor

## 原则 4：receiver 这类“语义上特殊的输入”要显式化

不要为了统一而统一，把所有输入都挤进一个无角色列表。

因为：

- `invoke-instance-method` 的 receiver
- `read-instance-field` 的 receiver
- 某些 query 的 subject

在语义上都比普通参数更特殊。

## 9. 我给你的三个判断建议

## 9.1 如果你更在意“短期实现最省事”

你会更容易被方案 A 吸引。

但我的建议仍然是：

- **别选**

因为这会把后面的 schema 漂移提前埋雷。

## 9.2 如果你更在意“主线 contract 现在就稳定”

那就选方案 B：

- 小 header
- typed payload
- shared spans

这是我当前明确推荐的主线。

## 9.3 如果你更在意“未来超大规模时的吞吐和压缩”

那可以把方案 C 作为长期演进方向。

但我不建议它抢跑到 `Phase A`。

## 10. 我的最终建议

这一轮我建议正式收成下面几点：

1. **首轮 canonical operation contract 采用“小型公共头 + kind-specific typed payload + shared spans”**
2. **公共头只保留 `OperationId / Kind / ResultRef? / PayloadRef`**
3. **所有语义角色差异都留在 typed payload，不进入大一统 header**
4. **所有可变长输入统一下沉到 `SpanId` / side-table**
5. **receiver / subject 这类语义特殊输入保持显式字段，不压成无角色输入列表**

## 11. 这一步之后最值得继续讨论的点

如果你认可这个方向，下一步最自然的问题就是：

- **`ValueRef / LiteralRef / SpanId` 这三类引用在 canonical semantic operation model 里分别怎么定义，边界怎么切**

也就是进一步往下收：

- operation 之间传递值的引用模型
- literal 是否进入统一 literal pool
- operand span 的顺序、稳定性和可诊断性 contract

