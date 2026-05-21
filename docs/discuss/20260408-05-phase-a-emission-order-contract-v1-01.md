# Phase A EmissionOrder Contract v1.01

Date: 2026-04-08
Status: design-discuss

## 1. 问题点

前两步我们已经基本收住了两件事：

- `Linker` 输出什么
  - retained closure
  - external managed dependencies
  - semantic-layer reasons
- `Linker` 不输出什么
  - lowering family
  - runtime roles
  - host bindings
  - template choices

但只要 `EmissionOrder` 还没冻结，`LinkedWorld` 仍然不算真正稳定。  
因为这时 `CodeGen`、`Emitter` 甚至后续 diagnostics/reporting 很容易再次各自发明一套遍历顺序：

1. `CodeGen` 自己按某种规则枚举 types/members
2. `Emitter` 或模板层再按另一套规则重排
3. 不同 backend/host 变体逐渐长出不同的默认顺序

最后就会出现三个问题：

1. 同一个 linked world，输出 diff 不稳定
2. 排障和对比时，很难判断差异到底来自语义变化还是遍历顺序变化
3. `Linker` 明明已经冻结了 closure，却没有真正冻结“下游如何稳定消费这份 closure”

所以这一轮真正要回答的是：

- **`EmissionOrder` 到底只是一个 deterministic list，还是需要更强的 grouping/phase contract。**

## 2. 问题的具体描述

这里最容易混淆的是三种完全不同层级的“顺序”：

### A. semantic enumeration order

回答的是：

- retained types 以什么稳定顺序枚举
- retained members 以什么稳定顺序枚举
- dependencies 以什么稳定顺序枚举

这是 `LinkedWorld` 应该冻结的东西。

### B. codegen assembly order

回答的是：

- `CodeGen` 先处理哪类 subject
- 先组 type-level lowering，还是先组 member-level lowering
- 某些 lowering family 的局部拼装顺序是什么

这是 `CodeGen` 自己的内部工作顺序。

### C. emitter/template render order

回答的是：

- translation unit 里先写 include，还是先写 forward declarations
- 先写 type metadata，还是先写 method bodies
- import stubs、runtime helpers、entry body 谁在前谁在后

这是 backend/template 层的渲染顺序。

这一轮的关键就是：

- **`Linker` 最多只能冻结 A，不能越界冻结 B 和 C。**

如果把 B/C 也塞进 `EmissionOrder`，那 `Linker` 就不再只是冻结 semantic closure，而是在偷偷扮演 codegen scheduler。

## 3. 有哪些可解决的路径

### 路径 A：不提供正式 `EmissionOrder`，让每个消费者自己排序

做法：

- `LinkedWorld` 只给 retained/dependency 集合
- `CodeGen` 自己决定 types/members/dependencies 的顺序
- `Emitter` 如有需要再局部重排

优点：

- `Linker` 最轻
- 设计成本最低

缺点：

- 不同消费者会逐渐漂出不同排序规则
- deterministic output 变成“靠实现自觉”
- 一旦输出 diff 抖动，很难追责是哪个层在改顺序
- `Linker` 没有真正完成“冻结 linked closure”的最后一环

判断：

- 不推荐

### 路径 B：`Linker` 输出“结构化的稳定语义顺序”，但不表达 codegen/emitter phase

做法：

- `EmissionOrder` 只覆盖 semantic surfaces：
  - `EntryOrder`
  - `TypeOrder`
  - `MemberOrder`
  - `DependencyOrder`
- 保证它们 deterministic、stable、与 lowering/backend 无关
- 可以包含少量 semantic grouping 约束
  - 例如 members 按 owner type 连续分组
- 但不表达：
  - `forward-decls-before-bodies`
  - `runtime-imports-before-entry`
  - `type-layout-before-method-layout`

优点：

- 真正把 linked semantic closure 的消费顺序冻结下来
- 防止 `CodeGen` 和 `Emitter` 各自发明排序
- 仍然不侵入 lowering/backend/template 层
- 最符合当前 `Phase A` 的分层目标

缺点：

- 需要 upfront 收一点 order invariants
- 名字叫 `EmissionOrder`，但必须反复强调它不是 render schedule

判断：

- 推荐

### 路径 C：把 `EmissionOrder` 做成强 phase/groupping contract，直接表达 codegen/render 阶段

做法：

- `Linker` 不只输出 semantic order，还输出类似：
  - `bootstrap-phase`
  - `type-definition-phase`
  - `method-body-phase`
  - `runtime-import-phase`
  - `entry-phase`

优点：

- 下游实现看起来更省事
- 某些当前 backend/template 可能更容易直接消费

缺点：

- 直接把 codegen/emitter 的内部调度绑回 `Linker`
- 一旦 backend/template 变化，`EmissionOrder` contract 也会被迫变化
- 容易把单 backend 的偶然顺序冻结成主线架构约束

判断：

- 不推荐

## 4. 我的方案和理由

我推荐：

- **路径 B：`Linker` 输出“结构化的稳定语义顺序”，但不表达 codegen/emitter phase。**

一句话定义：

- `EmissionOrder` 是 **stable semantic traversal contract**
- 不是 **code generation schedule**
- 更不是 **template render script**

我更推荐它的原因有五个：

1. 它刚好补上 `LinkedWorld` 冻结的最后一块
2. 它足以支撑 deterministic output 和稳定 diff
3. 它不会把 backend/template 细节提前压回 `Linker`
4. 它让 `CodeGen` 拿到稳定输入，但仍保留自己的 lowering 组织自由
5. 它对未来多 backend、多 host 变体更稳，因为顺序 contract 仍然停留在 semantic layer

## 5. 推荐的 contract 形状

我建议当前先把 `EmissionOrder` 收成四个 surface：

### A. `EntryOrder`

回答：

- 当前 linked world 的 entry methods 以什么稳定顺序枚举

意义：

- 让多 entry 场景下的 diagnostics/reporting/codegen 消费顺序固定下来

### B. `TypeOrder`

回答：

- retained types 以什么稳定顺序枚举

意义：

- 让 type-level codegen、type-level diagnostics、type-level reporting 不再各自排序

### C. `MemberOrder`

回答：

- retained members 以什么稳定顺序枚举

意义：

- 让 member-level 消费稳定
- 同时避免 `CodeGen` 再自己发明一套全局 member sort

### D. `DependencyOrder`

回答：

- external managed dependencies 以什么稳定顺序枚举

意义：

- 让 dependency-related diagnostics/reporting/codegen 输入稳定

## 6. 我建议冻结的 order invariants

这一轮我建议冻结的是“可观察不变量”，而不是某个具体排序实现算法。

### invariant 1：同一 `LinkedWorld` 必须导出同一 `EmissionOrder`

也就是说，只要以下条件不变：

- entry set 不变
- retained closure 不变
- external managed dependencies 不变
- canonical ids 不变

那么 `EmissionOrder` 就应保持完全一致。

### invariant 2：`EmissionOrder` 只依赖 semantic-layer 信息

它不应依赖：

- `LoweringFamily`
- `TemplateKind`
- `RuntimeRoles`
- `HostBindings`
- 当前 backend
- 当前宿主平台

也就是说：

- 同一个 linked semantic closure 在 Windows host 和未来的其他 host 上，`EmissionOrder` 应保持一致
- 差异只能出现在后面的 lowering/render 层

### invariant 3：`MemberOrder` 应体现最小语义分组，而不是全扁平随机顺序

我建议当前至少冻结一条：

- **同一 `OwnerTypeId` 的 retained members 应在 `MemberOrder` 中连续出现**

并且：

- owner type block 的相对顺序应跟随 `TypeOrder`

这样做的价值是：

- 这仍然是 semantic grouping，不是 backend phase
- 但它足以避免下游每次都重新按 owner type regroup

### invariant 4：不承诺 codegen/render 级 phase 语义

当前不应冻结下面这些更强保证：

- base type 一定先于 derived type
- field 一定先于 method
- constructor 一定先于 instance method
- callee 一定先于 caller
- runtime-related dependency 一定先于其他 dependency

不是因为这些顺序永远没价值，而是因为：

- **这些已经开始逼近 codegen/template 内部调度，而不是 semantic closure 的稳定枚举。**

如果未来某个 family 真需要这些顺序，也应优先由 `CodeGen` 在消费 `EmissionOrder` 之后局部组织，而不是回推给 `Linker`。

## 7. 一个更具体的推荐表达

如果要把这层 contract 写得稍微像数据形状，我建议先理解成：

```json
{
  "EmissionOrder": {
    "EntryOrder": ["Program::Main()"],
    "TypeOrder": ["Program", "Greeter"],
    "MemberOrder": [
      "Program::Main()",
      "Greeter::_name",
      "Greeter::.ctor(System.String)",
      "Greeter::Build()"
    ],
    "DependencyOrder": [
      "System.Object::.ctor()",
      "System.String::Concat(System.String,System.String)",
      "System.Console::WriteLine(System.String)"
    ]
  }
}
```

这里最关键的不是这个例子里的局部排序细节本身，而是它表达了四件事：

1. entry/type/member/dependency 各自有稳定顺序
2. member 仍然按 owner type 聚拢
3. dependency 仍然只是 external managed dependency 的稳定枚举
4. 整个 order view 没有出现任何 lowering/template/runtime phase 术语

## 8. 明确不属于当前 `EmissionOrder` 的东西

我建议把下面这些内容明确排除出当前 contract：

- `forward-declaration-order`
- `type-definition-order`
- `method-body-order`
- `runtime-helper-order`
- `template-section-order`
- `translation-unit-fragment-order`

这些东西回答的都不是：

- “semantic closure 该如何稳定消费”

而是：

- “某个 backend/template 该如何排版和生成”

这已经超出 `Linker` 的职责。

## 9. 我的最终结论

这一轮我建议正式拍板下面几件事：

1. `EmissionOrder` 不是可有可无的附属物，而是 `LinkedWorld` 正式 contract 的一部分
2. 当前 `Phase A` 使用“结构化的稳定语义顺序”，而不是“消费者自行排序”
3. `EmissionOrder` 当前只覆盖：
   - `EntryOrder`
   - `TypeOrder`
   - `MemberOrder`
   - `DependencyOrder`
4. `MemberOrder` 当前至少冻结一个最小 grouping 约束：
   - 同 owner type 的 members 连续出现
   - owner type block 顺序跟随 `TypeOrder`
5. 当前不冻结任何 codegen/render 级 phase 语义
6. 一切与 lowering/template/runtime/helper 排程相关的顺序，都明确不属于当前 `Linker` contract

## 10. 这一步之后最值得继续讨论的点

如果 `Linker` output contract 现在已经有了：

- retained closure
- reasons
- emission order

那接下来最自然的下一个话题就是：

- **`Diagnostics` contract**

也就是继续回答：

- `Linker` 这一层到底应该产出哪些 diagnostics
- 哪些属于 closure/dependency 问题
- 哪些则必须留给 `CodeGen` 或 `Emitter`

这样才能把 `LinkedWorld` 的输出面真正补齐。
