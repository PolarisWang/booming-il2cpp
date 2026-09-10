# Phase A Generated Native Performance-First Governing Decisions v1.01

Date: 2026-04-08
Status: design-discuss

## 1. 问题点
到目前为止，这条主线已经把很多 semantic contract 边界收得比较干净了。

这件事本身没有错，
但如果现在明确把目标改成：

- **generated native 工程性能最优**

那么我们就必须承认一件事：

- 当前很多 provisional decision，更偏向“semantic cleanliness / contract controllability 优先”
- 还不是严格意义上的“native code quality ceiling 优先”

这轮不再停留在 review 建议层，
而是把 review 里的发现直接提升为后续主线必须遵守的正式决策。

## 2. 总原则

从这一轮开始，我建议把下面这条作为后续讨论的 governing rule：

- **在 correctness 成立、边界仍可维护的前提下，一切核心层架构决策优先服务于 generated native 工程性能最优。**

这句话的实际含义是：

1. semantic contract 的干净程度仍然重要
2. 但它不再是最高优先级
3. 如果“边界更优雅”和“保留更强 native 优化空间”发生冲突，默认优先后者
4. 后续未达重要级别的小分叉，统一按这个原则由我自行拍板

## 3. 正式决策

## 3.1 决策 1：`SemanticWorld` 的六块 canonical output 保持不变

这一点我建议保留，不推翻：

- `CanonicalSubjects`
- `SemanticShapes`
- `MethodBodies`
- `CapabilityBundles`
- `LiteralPool`
- `Diagnostics`

原因不是“语义洁癖”，
而是：

- `SemanticWorld` 仍然应该保持 canonical semantic layer 的职责边界

所以这轮 **不把** performance-specific 信息粗暴塞回：

- `CanonicalSubjects`
- `SemanticShapes`
- `CapabilityBundles`
- `stable metadata lookup`

## 3.2 决策 2：`CanonicalSubjects` 继续只承接 definition identity

这一点也保留：

- `CanonicalSubjects` 继续只承接 definition identity
- 不把 `closed instantiation identity` 直接抬成 canonical subject

也就是说：

- `Box<T>` 仍然是 canonical subject
- `Box<int>` 不会直接变成新的 canonical `TypeId`

这条不改，
但它不再意味着：

- closed-world specialization 可以继续停留在隐式状态

相反，它引出下一条必须补上的正式决策。

## 3.3 决策 3：必须引入 first-class `OptimizationFacts` 合同

这是这轮最核心的新决策。

我建议正式拍板：

- **在 `LinkedWorld -> CodeGen` 边界，引入 first-class `OptimizationFacts` contract**

它的地位是：

- 不是 `SemanticWorld` 的第七块 canonical output
- 而是 closed-world、performance-first codegen 所必需的正式下游合同

一句话定义：

- `SemanticWorld` 负责 canonical semantic facts
- `OptimizationFacts` 负责 performance-oriented codegen facts

## 3.3.1 为什么必须单独引入 `OptimizationFacts`

因为下面这些信息，
如果不 first-class 化，
后面只能散落在：

- query
- payload
- ad-hoc lowering logic
- metadata fallback

最终一定会损伤 generated native 代码质量上限：

- closed generic instantiation facts
- specialization context facts
- devirtualization-relevant facts
- layout/storage-relevant facts
- const-foldability / concat-shape / allocation-elision-relevant facts

## 3.3.2 `OptimizationFacts` 最少要承接哪两大类事实

当前我建议最少承接两大类：

### A. `ClosedWorldSpecializationFacts`

回答：

- 当前 retained world 里，哪些 definition 在哪些 concrete closed context 下真正被使用

典型例子：

- `Box<T>` 仍在 `CanonicalSubjects`
- `Box<int>` 的 retained instantiation 进入 `OptimizationFacts`

### B. `OptimizationShapeFacts`

回答：

- 哪些稳定结构事实会直接影响 native code quality

典型类型包括：

- dispatch facts
- layout/storage facts
- mutability/constness facts
- operation-shape facts

## 3.4 决策 4：`stable metadata lookup` 继续 definition-only，不承接优化上下文

这一点也保留：

- `stable metadata lookup` 继续只是 companion lookup boundary
- 继续只做 definition-level descriptor 反查
- 继续不接受 instantiated generic context 查询

也就是说：

- `GetMethodMetadata(MethodId)` 可以存在
- `GetMethodMetadata(MethodId, QueryKeyId)` 仍然不进入主线 contract

原因不是说 instantiated context 不重要，
而是：

- **它现在有了新的正式归属：`OptimizationFacts`**

这样边界就比较清楚：

- definition descriptors 归 `stable metadata lookup`
- optimization-oriented closed-world facts 归 `OptimizationFacts`

## 3.5 决策 5：`CapabilityBundles` 保持轻量，但不再承担性能主表达

这一点我建议明确修正原先的潜台词：

- `CapabilityBundles` 仍然保留
- 仍然是 canonical semantic requirement summary
- 但它 **不能再被视为 codegen 性能决策的主要高分辨率输入**

所以这里正式拍板两点：

1. `CapabilityBundle` 继续保持轻量：
   - 无序 capability name set
   - 不塞 count/confidence/reason/lowering hint
2. performance-sensitive 的细粒度事实不再强行塞进 capability：
   - 统一进入 `OptimizationFacts`

这意味着：

- `requires-string-concat`
  - 仍属于 capability
- “2 元还是 N 元拼接”
- “是否常量折叠”
- “是否可零分配拼接”
  - 不再试图塞进 capability bundle，而进入 `OptimizationFacts`

## 3.6 决策 6：`SemanticShapes` 保持 semantic role 层，但不能再以“极简”为理由屏蔽优化事实

当前的 `SemanticShapes` 仍然保留它的职责定位：

- semantic role layer

但从这一轮开始，增加一条新的硬约束：

- **凡是会显著影响 generated native 代码质量的结构事实，不能因为“shape 想保持极简”而被排除出正式合同。**

这些事实未必都要塞进 `SemanticShapes` 本体，
但必须进入正式 contract。

当前我的决定是：

- semantic role 继续放在 `SemanticShapes`
- optimization-relevant structural facts 进入 `OptimizationFacts`

也就是说，后续下面这些信息不能再长期缺席：

- value/reference storage implications
- sealed/final/devirtualization-relevant facts
- layout/blittability-relevant facts
- field mutability / constness-relevant facts

## 3.7 决策 7：`LoweringFamily` 选择从“优先级裁决”升级为“合法性过滤 + 性能裁决”

这是第二个关键修正。

我建议正式拍板：

- **`LoweringFamily` selection 不再以静态优先级式作为最终主规则**

新的主规则改成两段式：

### 第一段：合法性过滤

仍然基于：

- semantic subject structure
- capability bundle
- 必要的 optimization facts

先过滤出：

- 所有合法 family candidates

### 第二段：性能裁决

在多个 candidate 同时合法时，
不再由静态 rule order 直接决定，
而是由显式的 performance-first policy 决定：

- target-aware cost policy
- optimization preference
- native code quality objective

### 静态优先级的地位

静态优先级不再是主裁决器，
只保留为：

- fallback tie-breaker

也就是：

- 当性能裁决结果等价
- 或当前切片没有足够 optimization facts

时，才允许回退到 deterministic order。

## 3.7.1 为什么要这样改

因为如果继续把“谁排前面”当主裁决逻辑，
本质上还是：

- rule governance 优先

而不是：

- native performance outcome 优先

这在“同语义可落多个 lowering family”时会直接卡住上限。

## 3.8 决策 8：未来非关键分叉默认按“保优化空间”拍板

从现在开始，后续我对非重要问题的默认拍板规则是：

### 默认选择 1

优先选择能保留更强 specialization / devirtualization / layout-aware codegen 空间的方案。

### 默认选择 2

如果一个方案只是 semantic 更整洁，
但会把 optimization facts 压回隐式逻辑，
默认不选。

### 默认选择 3

如果一个方案能让 codegen 更早拿到 concrete closed-world facts，
默认优先。

## 4. 对既有讨论结论的修正

为了避免后面读文档时误解，我建议明确下面几条“保留/修正”关系。

## 4.1 保留的结论

下面这些结论继续有效：

- `SemanticWorld` 六块 canonical output 继续成立
- `CanonicalSubjects` 继续 definition-only
- `stable metadata lookup` 继续 definition-only companion boundary
- `CapabilityBundles` 仍是 semantic requirement summary
- `SemanticShapes` 仍是 semantic role layer

## 4.2 被修正的结论

下面这些 provisional understanding 从这一轮起要按新规则理解：

### A. 对 `CapabilityBundles` 的理解被修正

原先更接近：

- capability 是 codegen 的主要高层信号之一

现在修正为：

- capability 仍重要
- 但它只承担 semantic requirement summary
- 不能承载 performance-first codegen 所需的高分辨率输入

### B. 对 `SemanticShapes` 极简化的容忍度被修正

原先更接近：

- shape 能极简就极简

现在修正为：

- 只要缺失会影响 native 代码质量，
  那就必须有正式落点
- 只是这个落点优先放进 `OptimizationFacts`，而不是盲目撑胖 `SemanticShapes`

### C. 对 family selection rule 的主线选择被修正

原先主线更接近：

- `Phase A` 先采用优先级式

现在修正为：

- 主线目标改为：
  - `legality filter + performance arbitration`
- 优先级式只保留为 fallback

## 5. 用例子看新决策后的分层

## 5.1 泛型特化例子

```csharp
class Box<T>
{
    public T Value;
    public T Get() => Value;
}

static int Main()
{
    var box = new Box<int>();
    return box.Get();
}
```

现在推荐理解成：

- `CanonicalSubjects`
  - `TypeId(Box<T>)`
  - `FieldId(Box<T>::Value)`
  - `MethodId(Box<T>::Get)`
- `StableMetadataLookup`
  - 反查 `Box<T>` 的 definition signature / token / arity
- `OptimizationFacts.ClosedWorldSpecializationFacts`
  - 明确记录 retained concrete instantiation：
    - `Box<int>`
    - `Box<int>::Get`

也就是说：

- definition identity 不变
- concrete specialization 也不再隐式

## 5.2 多种 lowering 可选例子

假设同一个 semantic slice 同时可以合法落到：

- family A：更泛化，但多一层间接
- family B：更特化，可内联、可少一次分配

从这一轮开始，主线裁决逻辑不再是：

- 因为 B 排在 A 前面，所以选 B

而是：

- 先确认 A/B 都合法
- 再根据 performance-first policy 选 native outcome 更优的一条

## 6. 从这一轮开始的默认推进方向

为了后面我能自行继续收口，这里直接把推进顺序也定掉：

1. 下一优先话题默认转向：
   - **`OptimizationFacts` contract**
2. 其后再继续冻结：
   - performance arbitration contract
   - definition-level signature / type-reference descriptor contract
3. 后续讨论中，若不是会显著改变核心分层，我将不再停下来征询小分叉意见

## 7. 我的最终决策汇总

这一轮正式拍板如下：

1. **generated native 工程性能最优，成为后续核心层架构的最高目标之一，并在与“语义边界更整洁”冲突时默认优先。**
2. **`SemanticWorld` 六块 canonical output 保持不变。**
3. **`CanonicalSubjects` 继续只承接 definition identity，不承接 closed instantiation identity。**
4. **必须新增 first-class `OptimizationFacts` contract，作为 `LinkedWorld -> CodeGen` 的正式性能导向输入。**
5. **`stable metadata lookup` 继续 definition-only，不承接 instantiated context；相关需求转移到 `OptimizationFacts`。**
6. **`CapabilityBundles` 保持轻量 semantic summary，不再承担高分辨率性能输入职责。**
7. **optimization-relevant structural facts 不得再因“shape 极简”而缺席正式 contract，默认收进 `OptimizationFacts`。**
8. **`LoweringFamily` selection 主规则改为“合法性过滤 + 性能裁决”，静态优先级只保留为 fallback tie-breaker。**
9. **后续非关键决策默认由我按“最大化 generated native 优化空间”原则自行拍板。**
