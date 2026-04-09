# Phase A Stable Metadata Lookup Boundary v1.01

Date: 2026-04-08
Status: design-discuss

## 1. 问题点
前面几轮我们已经连续拍了几件事：

- `SemanticWorld` 的 canonical output 只收六块：
  - `CanonicalSubjects`
  - `SemanticShapes`
  - `MethodBodies`
  - `CapabilityBundles`
  - `LiteralPool`
  - `Diagnostics`
- `CanonicalSubjects` 只承接 identity + declaring graph
- `assembly / namespace / token / display-name` 这类 definition-level 信息不抬进 `CanonicalSubjects`

但如果这一步继续停在这里，后面很容易马上出现三个回流问题：

1. 下游为了拿 `token / signature / arity / assembly identity`，又直接回头依赖 raw loader facts
2. 某些“只是为了 definition-level 反查”的信息，被硬塞回 `CanonicalSubjects` 或别的 canonical block
3. 为了方便处理泛型，metadata lookup 不知不觉开始承接 closed instantiation context，重新污染 `CanonicalSubjects` 已经拍死的 definition-only 边界

所以这一轮真正要回答的是：

- **`stable metadata lookup` 到底是不是 `SemanticWorld` 的第七块正式输出**
- **它最小允许提供哪些 definition-level 反查能力**
- **它是否允许带 instantiated generic context 查询**
- **下游什么时候可以用它，什么时候必须只看 canonical artifacts**

## 2. 这里讨论的不是什么

这里讨论的不是：

- metadata lookup 的缓存实现
- token 编码格式
- 全量 Roslyn/ECMA-335 风格的类型系统建模
- signature/type ref descriptor 的最终内部数据结构
- source-level debug/provenance 展示

这里讨论的是：

- **在 canonical artifacts 已经冻结之后，definition-level metadata 还允许通过一个什么样的受控边界被反查**

一句话说：

- `stable metadata lookup` 回答的是“**给定 canonical subject id，还能稳定、受控地反查哪些 definition-level 描述信息**”

## 3. 三种可选方案

## 3.1 方案 A：把需要的 metadata 直接塞回 canonical output

### 方案描述

做法是：

- 既然下游会需要 `assembly / namespace / token / signature / arity`
- 那就把它们直接抬进：
  - `CanonicalSubjects`
  - 或别的 canonical block

### 优点

- 表面上“一站式”
- 下游不需要再过一个 lookup boundary

### 缺点

- 会把 canonical output 重新撑胖
- identity / semantic facts / definition descriptors 会重新混在一起
- 和我们前面刚刚拍死的“六块 canonical output + `CanonicalSubjects` 保持克制”直接冲突
- 一旦后续想调整 metadata 表达，主 contract 会被迫一起抖动

### 我的判断

- **不推荐**

## 3.2 方案 B：定义独立的、受控的 definition-level stable metadata lookup

### 方案描述

做法是：

- canonical output 仍然保持六块，不增加第七块 artifact
- 同时在 `SemanticWorld` 的消费边界旁边，提供一个单独的、只读的、definition-level 的 lookup 面
- lookup 只接受 canonical ids 作为入口：
  - `TypeId`
  - `MethodId`
  - `FieldId`
  - 必要时 `MemberId`

### 优点

- canonical artifacts 继续保持瘦
- raw loader facts 不需要重新暴露给下游
- definition-level metadata 有正式出口，但不会升级成 semantic facts
- 非常符合我们前面一路都在做的“主 contract 收紧、辅助边界受控”思路

### 缺点

- 比“全塞进 canonical output”多一层边界
- 需要明确下游不能拿这个 lookup 去做 semantic 回推

### 我的判断

- **推荐**

## 3.3 方案 C：metadata lookup 同时支持 definition-level 和 instantiated-context 查询

### 方案描述

做法是：

- 不只允许 `GetMethodMetadata(MethodId)`
- 还允许类似：
  - `GetClosedMethodMetadata(MethodId, QueryKeyId)`
  - `GetInstantiatedTypeMetadata(TypeId, ClosedContext)`

### 优点

- 表面上“更强”
- 某些泛型相关问题看起来一步到位

### 缺点

- 会把 closed instantiation context 重新抬回 metadata 主边界
- `CanonicalSubjects` 已经拍死的 definition-only 原则会被绕开
- query/capability/lowering 本来该承接的责任会回流到 metadata lookup
- downstream 很容易开始在 metadata lookup 上做 semantic 决策，而不是消费 canonical artifacts

### 我的判断

- **不推荐**

## 4. 我的建议

如果这一轮要直接拍板，我建议采用：

- **方案 B：独立的、受控的 definition-level stable metadata lookup**

一句话定义：

- `stable metadata lookup` 不是第七块 canonical output
- 它是 `SemanticWorld` 旁边的一个 **companion lookup boundary**

它的职责是：

- 给 canonical subject 提供 definition-level descriptor 反查

它不负责：

- 发布新的 semantic facts
- 补充新的 canonical subjects
- 表达 closed instantiation identity
- 替代 `SemanticShapes / MethodBodies / CapabilityBundles`

## 5. 我建议冻结的边界结论

## 5.1 它不是第七块 canonical artifact

这一点我建议明确钉死：

- **canonical output 仍然只有六块**
- `stable metadata lookup` 是 companion boundary，不是第七块 artifact block

原因很直接：

- 它承接的是 definition descriptors
- 不是 semantic layer 承诺给下游的 canonical facts

如果把它也算成正式 artifact block，
canonical output 的边界又会重新变胖。

## 5.2 它只接受 canonical ids 作为入口

推荐最小入口是：

```text
GetTypeMetadata(TypeId)
GetMethodMetadata(MethodId)
GetFieldMetadata(FieldId)
```

如确实需要，也可以允许：

```text
GetMemberMetadata(MemberId)
```

但这里的 `MemberId` 仍然只是 umbrella projection，
它不改变 typed lookup 的主地位。

我当前 **不建议** 允许下面这类入口进入主 contract：

- raw metadata token 直接反查
- assembly/name 字符串直接反查
- instantiated generic context 直接参与 lookup key

换句话说：

- **lookup 的入口必须先经过 canonical identity 层，而不是绕过它**

## 5.3 它只提供 definition-level metadata，不提供 instantiated-context metadata

这一点我建议直接拍死：

- **`stable metadata lookup` 只做 definition-level 反查**

也就是说：

- 可以查 `Box<T>::Get`
- 不可以查 “`Box<int>::Get` 在这个 closed context 下的特化 metadata”

后者如果未来需要，应该继续留在：

- query 层
- capability 层
- lowering / specialization context 层

而不是回流进 metadata 主边界。

## 6. 我建议允许返回的最小 metadata 类别

当前 `Phase A` 我建议只冻结“允许返回的 metadata 类别”，
而不在这一轮就把每个 descriptor 的完整 schema 做深。

## 6.1 identity-display 类

用于回答：

- 这个 subject 在 definition-level 上叫什么
- 它属于哪个 assembly identity
- 它的 generic arity 是多少

推荐最小覆盖：

- `assembly identity`
- `namespace`
- `simple name`
- `generic arity`
- `metadata token`

注意这里我说的是：

- **definition descriptors**

不是：

- semantic role
- body availability
- capability

## 6.2 declaration-signature 类

用于回答：

- method 的 definition signature 是什么
- field 的 declared type 是什么

推荐允许有：

- method return signature descriptor
- method parameter signature descriptors
- field type descriptor

但我当前 **不建议** 这一轮就继续展开：

- type reference descriptor 的最终 contract
- signature descriptor 的内部统一模型

这些应当成为下一层更聚焦的话题。

## 6.3 loader-owned definition annotations 类

这类是我建议保留的一个小出口，
用于承接某些明确属于 definition-level、但不是 semantic shape 的附着信息。

最典型的例子就是：

- imported method 的 binding metadata

例如：

- library name
- entry point name
- calling convention 之类的原始定义信息

它们可以通过 metadata lookup 反查，
但不应该升级成：

- `CanonicalSubjects`
- `SemanticShapes`
- `CapabilityBundles`

里的新字段。

## 7. 我建议明确禁止的几类能力

为了防止 metadata lookup 重新变成“raw loader fallback 总入口”，
我建议明确列几个禁区。

### A. 不返回 method body / IL / opcode stream

这些不是 metadata lookup 的工作，
而是：

- `MethodBodies`
- 或 raw loader internals

### B. 不允许用 metadata lookup 回推 semantic shape

例如下面这种我建议明确禁止：

- “因为 method 有 extern/import 注记，所以我从 metadata lookup 推断它是 `imported-method`”
- “因为 method 没有 body token，所以我从 metadata lookup 推断它是 `no-canonical-body`”

这些应该以：

- `SemanticShapes`

为准。

### C. 不允许用 metadata lookup 做 retained/root/lowering 决策

例如：

- `entry`
- `retained`
- `dependency-root`
- `pair-concat-lowering`

这些都不应从 metadata lookup 推出来。

### D. 不允许 instantiated generic context 混入 lookup

例如：

- `GetMethodMetadata(MethodId, QueryKeyId)`
- `GetTypeMetadata(TypeId, ClosedTypeArgs)`

当前 `Phase A` 我建议明确不允许。

## 8. 下游消费规则

我建议把使用规则压成一句话：

- **semantic 决策只看 canonical artifacts；metadata lookup 只做 definition-level descriptor 反查**

更具体一点：

## 8.1 `Linker`

`Linker` 的 retained/dependency 语义决策，
应继续建立在：

- `CanonicalSubjects`
- `SemanticShapes`
- `MethodBodies`
- `CapabilityBundles`
- `Diagnostics`

之上。

`Linker` 不应把 metadata lookup 当成新的 semantic facts 来源。

但 `Linker` 可以在受控情况下读取 metadata lookup 来拿：

- 外部 definition identity 展示信息
- 受控的 signature/assembly 反查

## 8.2 `CodeGen`

`CodeGen` 选择 lowering family 的主依据，
仍然应该是：

- semantic structure
- capability bundles
- linked closure facts

不是 metadata lookup。

但 `CodeGen` 可以在确定了 semantic 路径之后，再通过 metadata lookup 取：

- declaration signature descriptors
- imported binding metadata
- 名称与宿主桥接所需的 definition descriptors

## 8.3 `Emitter`

`Emitter` 仍然只 render plan，
不应该直接从 metadata lookup 重新做 semantic 决策。

## 9. 用两个例子看推荐边界

## 9.1 普通方法例子

```csharp
class Greeter
{
    private readonly string _name;

    public string Build()
    {
        return "Hello, " + _name;
    }
}
```

这里推荐理解成：

- `CanonicalSubjects`
  - 告诉我们 `MethodId(Greeter::Build)` 存在，并声明在 `Greeter` 下
- `SemanticShapes`
  - 告诉我们它是 `instance-method + has-canonical-body`
- `MethodBodies`
  - 告诉我们它读实例字段、做字符串拼接
- `CapabilityBundles`
  - 告诉我们它 `uses-instance-field-state`、`requires-string-concat`
- `StableMetadataLookup.GetMethodMetadata(MethodId(Greeter::Build))`
  - 只返回 definition-level 描述，例如：
    - simple name = `Build`
    - generic arity = `0`
    - signature descriptor = `string ()`
    - metadata token = `...`

这里 metadata lookup 没有资格决定：

- 它是不是 instance method
- 它有没有 canonical body
- 它需不需要 string concat

## 9.2 imported method 例子

```csharp
[DllImport("kernel32.dll", EntryPoint = "Sleep")]
static extern void Sleep(uint milliseconds);
```

这里推荐理解成：

- `SemanticShapes`
  - 告诉我们这是 `imported-method + no-canonical-body`
- `CapabilityBundles`
  - 告诉我们它 `requires-imported-call`
- `StableMetadataLookup.GetMethodMetadata(MethodId(Sleep))`
  - 可以返回 definition-level import annotation，例如：
    - library = `kernel32.dll`
    - entry point = `Sleep`

也就是说：

- **“它是 imported-method” 是 semantic 结论**
- **“它绑定到哪个库和入口点” 是 definition-level metadata**

这两层不要混。

## 10. 我建议冻结的最小结论

这一轮我建议正式拍板下面几点：

1. **`stable metadata lookup` 不是第七块 canonical output，而是 `SemanticWorld` 旁边的 companion lookup boundary**
2. **lookup 的入口必须是 canonical ids，不允许 raw token/name/context 绕过 canonical identity 直接进入**
3. **当前 `Phase A` 的 metadata lookup 只做 definition-level 反查，不做 instantiated generic context 查询**
4. **允许返回的最小 metadata 类别先收为三类：**
   - `identity-display`
   - `declaration-signature`
   - `loader-owned definition annotations`
5. **`Linker / CodeGen / Emitter` 都不能把 metadata lookup 当成新的 semantic facts 来源**
6. **semantic 决策继续只看 canonical artifacts；metadata lookup 只承担 definition-level descriptor 反查职责**

## 11. 这一步之后最值得继续讨论的点

如果继续顺着主线往下收，下一步最值得冻结的问题就是：

- **definition-level signature / type-reference descriptor contract 到底怎么定**

因为 metadata lookup 这一步虽然已经把边界收住了，
但它返回的：

- method signature descriptor
- field type descriptor

内部到底是一个怎样的最小 typed contract，
现在还没有真正拍死。
