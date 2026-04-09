# Phase A Semantic Operation Payload Cost And Shape v1.01

Date: 2026-04-08
Status: design-discuss

## 1. 问题点

上一轮我们已经基本定下两件事：

- `SemanticWorld` 输出的是 semantic-action 级的 `operation stream`
- operation 名字只表达“发生了什么语义动作”，差异尽量下沉到 payload

但这里马上会出现一个自然追问：

- **把差异放进 payload，会不会带来明显性能问题？**
- **如果会，有没有比 payload 更优的方案？**

这个问题如果不收住，后面很容易出现两种极端：

1. 因为担心 payload 成本，重新把差异塞回 operation 名字，最后 vocabulary 爆炸
2. 虽然保留了 payload，但实现成松散 `bag/map/object`，把本来可控的成本做成了真瓶颈

所以这一轮真正要回答的不是“要不要 payload”这个抽象问题，而是：

- **payload 以什么形态存在，成本差异到底有多大？**

## 2. 先明确：这里讨论的是哪类性能

这里讨论的主要不是最终生成 C++ 程序的运行时性能，而是编译管线内部的：

- `SemanticWorld` 构造成本
- `LinkedWorld -> CodeGen` 消费成本
- 内存占用和分配压力
- 遍历局部性和缓存友好性
- 后续序列化 / 调试输出 / 诊断查看的成本

也就是说，这里问的本质是：

- **semantic operation model 会不会让主线编译器自己变慢、变重、变难维护？**

## 3. 先给结论

我的结论很明确：

- **payload 这个概念本身不是性能问题**
- **真正有问题的是“无类型、字符串键、稀疏字段袋”的 payload 形态**
- **如果采用 typed payload 或 compact payload-ref 方案，Phase A 完全可以接受**

一句话概括：

- **坏的不是 payload，坏的是 bag-shaped payload**

## 4. 三种常见形态

## 4.1 方案 A：通用字段袋 payload

### 形态

例如每个 operation 都长成：

```text
Operation {
  Kind: "query-closed-type-member",
  Payload: Dictionary<string, object>
}
```

或者：

```text
Payload = {
  "memberKind": "property",
  "subjectTypeId": "...",
  "argumentRefs": [...],
  "literal": "Hello"
}
```

### 看起来的优点

- 很灵活
- 加字段很方便
- 调试输出看起来“通用”

### 真正的问题

这类方案通常是最差的性能形态：

- 每个 operation 都容易产生额外对象分配
- `string` key 查找有常数成本
- `object` value 很容易触发 boxing / unboxing
- schema 不固定，消费方要做大量运行时判断
- 很难获得好的内存局部性
- 很难静态校验“某个 operation 应该带哪些字段”

如果 operation 数量一上来，这种成本会很快变成真实负担。

### 判断

- **不推荐**

这不是“payload 必然慢”的证据，而是“bag payload 很慢”的证据。

## 4.2 方案 B：`OperationKind + typed payload variant`

### 形态

例如：

```text
Operation {
  Kind: QueryClosedTypeMember,
  Payload: QueryClosedTypeMemberPayload
}
```

```text
QueryClosedTypeMemberPayload {
  SubjectTypeId
  MemberKind
  QueryKeyId
}
```

或者更紧凑一点：

```text
Operation {
  Kind
  ResultRef
  PayloadRef
}
```

`PayloadRef` 再指向一个按 kind 区分的 typed record。

### 优点

- 没有字符串键查找
- 没有通用 `object` 包装
- schema 固定，验证简单
- `CodeGen` 可以直接按 `Kind` 分发
- 能保持 operation 名字稳定，不把差异挤回 vocabulary

### 成本

- 仍然会有一些 payload 对象或记录
- 对可变长数据要额外设计承载方式
- 需要维护一套明确的 payload contract

### 判断

- **推荐作为 Phase A 主线**

它在性能、清晰度、可演进性之间最平衡。

## 4.3 方案 C：紧凑 header + side-table / arena payload

### 形态

例如 operation 本体只保留固定小头：

```text
Operation {
  Kind
  Flags
  ResultRef
  PayloadIndex
}
```

具体 payload 不直接挂在 operation 上，而是进入：

- `ConstructObjectPayloadTable`
- `InvokeImportedMethodPayloadTable`
- `QueryClosedTypeMemberPayloadTable`
- `OperandSpanTable`

这类 side table 或 arena。

### 优点

- operation 主数组可以非常紧凑
- 遍历时缓存局部性更好
- 可变长字段可以统一进 span/arena
- 对大规模 method / 大型 solution 更友好

### 成本

- 设计复杂度明显更高
- 调试可读性略差
- 实现期更容易把简单问题做重

### 判断

- **如果目标是更强的规模化性能，这是比普通 typed payload 更优的方向**
- **但对 Phase A 来说，更像是 B 的增强版，而不是第一天就必须上满的方案**

## 5. “不用 payload”真的更优吗

很多人第一直觉会是：

- 那我别要 payload 了，直接把差异塞回 operation 名字不就行了？

例如从：

- `query-closed-type-member + payload.memberKind`

变成：

- `query-closed-type-field`
- `query-closed-type-property`
- `query-closed-type-method`
- `query-closed-type-parameter`

再比如从：

- `invoke-imported-method + payload.argumentSpan`

变成：

- `invoke-imported-method-0arg`
- `invoke-imported-method-1arg`
- `invoke-imported-method-2arg`

这条路表面上减少了 payload 判断，实际上会带来更坏的问题：

- vocabulary 爆炸
- operation 语义边界漂移
- 很多差异其实不是“不同语义动作”，只是“同类动作的参数不同”
- `CodeGen` 规则数量会上升
- 新增切片时更容易重复发明名字

所以：

- **“不用 payload”通常不是更优，而是把数据成本换成了 vocabulary 和规则复杂度成本**

## 6. 真正决定性能的，不是有没有 payload，而是 payload 里装什么

我建议把 payload 的内容收得很硬：

### 应该优先放进去的

- canonical ids
  - `TypeId`
  - `MethodId`
  - `FieldId`
  - `MemberId`
- 小型枚举
  - `MemberKind`
  - `InvocationKind`
- 稳定引用
  - `ValueRef`
  - `OperationRef`
  - `LiteralRef`
- `SpanId` / `PayloadRef` / `TableIndex`

### 不应该直接塞进去的

- 原始字符串大对象
- 完整 metadata object
- 运行时才关心的大结构
- `Dictionary<string, object>`
- 临时推导出来、但没有 canonical id 的松散匿名对象

一句话说：

- **payload 最好装“引用”和“小标签”，不要装“肥对象”**

## 7. 用当前 vocabulary 看几个例子

## 7.1 `construct-object`

推荐长成：

```text
ConstructObjectPayload {
  TargetTypeId
  CtorMethodId
  ArgumentSpanId
}
```

这里 `ArgumentSpanId` 指向统一的 operand span 表，而不是每个 operation 自己挂一个可变长数组。

## 7.2 `query-closed-type-member`

推荐长成：

```text
QueryClosedTypeMemberPayload {
  SubjectTypeId
  MemberKind
  QueryKeyId
}
```

这样 `field/property/method/parameter` 的差异被收在一个小枚举里，不需要拆四个 operation 名字。

## 7.3 `invoke-imported-method`

推荐长成：

```text
InvokeImportedMethodPayload {
  MethodId
  ArgumentSpanId
}
```

它表达的是“这是 imported method 调用语义”，
而不是“最后怎么做 host binding / runtime resolution”。

## 7.4 `string-concat`

推荐长成：

```text
StringConcatPayload {
  InputSpanId
}
```

不要因为未来可能有 pair-chain / builder / helper 几种 lowering，
就把这些差异提早写进 operation 名字。

## 8. 如果担心性能，真正该优先控制的点

如果我们已经接受 “operation 名字稳定、差异下沉 payload”，
那最该控制的是下面几个点：

### 1. 不要让 payload 变成 bag

这是最大的红线。

### 2. 可变长数据不要内联肥数组

像 argument list、concat inputs、query operands 这类数据，更适合放到：

- span table
- operand arena
- side table

而不是每个 operation 单独挂一份对象数组。

### 3. payload 尽量用 id，不要直挂 rich object

如果 payload 挂的是 metadata object / semantic node object，
那遍历和序列化都会变重。

### 4. 把“固定小头”和“可变细节”分开

最稳的方向通常是：

- operation header 固定
- payload record typed
- 可变长数据单独池化

## 9. 我给你的三个可选建议

## 9.1 方案 1：最省事，但不推荐

- 使用通用 `payload bag`

### 适合什么

- 只想快速试验 vocabulary

### 问题

- 很容易把临时试验形态做成长期包袱

### 我的建议

- **不选**

## 9.2 方案 2：Phase A 最平衡方案

- 使用 `OperationKind + typed payload`
- 对可变长参数再加一个 `SpanId` / `ArgumentSpanId`

### 适合什么

- 现在就要落地主线 contract
- 同时不想把实现复杂度抬太高

### 我的建议

- **这是我推荐的当前主线**

## 9.3 方案 3：更偏长期性能优化

- 使用 `compact operation header + payload side tables`

### 适合什么

- 后面确认 operation 数量会非常大
- 编译器内部吞吐和内存会成为重点指标

### 我的建议

- **可以作为方案 2 的演进方向**
- **不建议在 Phase A 一开始就全量上这个复杂度**

## 10. 我的最终建议

如果这一轮要直接给结论，我建议定成：

1. **继续保留 payload 思路，不回退到 vocabulary 爆炸路线**
2. **明确禁止 bag-shaped payload**
3. **Phase A 主线采用 `OperationKind + typed payload`**
4. **所有可变长字段统一用 `SpanId` / side-table 引用承载**
5. **payload 主要携带 canonical ids、小枚举、稳定引用，不携带 rich object**
6. **如果后续实测表明 operation stream 成本开始上升，再向 compact header + side-table 继续演进**

一句话收尾：

- **payload 不是问题；松散 payload 才是问题**
- **比“泛 payload”更优的方案，不是“不要 payload”，而是“typed payload + compact reference shape”**

## 11. 这一步之后最值得继续讨论的点

如果认可这个方向，下一步最自然的问题就是：

- **首轮 canonical operation payload contract 应该具体冻结到什么字段级别**

也就是继续往前走到：

- 哪些 operation 需要固定 header 字段
- 哪些 operation 需要 typed payload
- 哪些字段必须下沉到 span/arena/side-table

