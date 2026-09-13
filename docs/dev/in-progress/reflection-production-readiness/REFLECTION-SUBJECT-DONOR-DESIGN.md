# 设计分析：反射探测主体供体类型 (ReflectionSubjectDonor)

> **状态**: 分析完成，待选方案
> **日期**: 2026-09-13
> **阻断**: reflection chunk pipeline 无法构建（C2371）
> **前序**: `c61ba23d6`（真实实例工厂）、`a9fa0def7`（三处配套修复）、`4941dcace`（扫描终止符）

---

## 1. 问题陈述

`c61ba23d6` 为解锁验证真实性，让 ATG 对反射类型使用**真实实例**（而非
`SubjectInstanceFactory.Create<T>()` = `GetUninitializedObject`，后者背后无元数据，
访问器全返 null/0）。

工厂表达式形如 `typeof(ReflectionSubjectSample).GetMethod("SampleMethod")` 被写入
**生成的测试方法体**，因此把供体类型 `ReflectionSubjectSample` 拉进了 codegen 的
AOT 翻译闭包。

## 2. 确证的机制（非推测）

生成物 `native-aot.generated.header.h` 中同时存在：

```cpp
// 行 9 —— 来自正常闭包发射（引用类型形状，正确）
struct chaos_valuetype_..._ReflectionSubjectSample {
    CHAOS_IL2CPP_INT32 field_..._SampleProperty_k__BackingField = 0;
    CHAOS_IL2CPP_INTPTR field_..._SampleEvent = 0;
};

// 行 90 —— 同样来自正常发射（引用类型形状，正确）
struct chaos_type_..._ReflectionSubjectSample : public chaos_type_System_Private_CoreLib_System_Object {
    /* 相同字段 */
};

// 行 362 —— boxed 定义内嵌 valuetype 成员
struct chaos_boxed_type_..._ReflectionSubjectSample {
    PureTypeHeader header{};
    chaos_valuetype_..._ReflectionSubjectSample value{};   // ← 需要 valuetype 存在
};

// 行 1806 —— safety net 补的 typedef
typedef CHAOS_IL2CPP_INT32 chaos_valuetype_..._ReflectionSubjectSample;  // ← 与行 9 冲突
```

→ `error C2371: redefinition; different basic types`

### 对照证据

| 类型 | `chaos_boxed_type_*` 形态 |
|---|---|
| 普通测试类（`AssemblyTests` 等） | **仅前向声明**（无定义） |
| `ReflectionSubjectSample`（供体） | **完整定义**，含 `chaos_valuetype_* value{}` |

即：只有供体触发了 boxed **定义**，其余类型只有声明。

### 已确证的完整因果链（2026-09-13，逐段实测）

```
1. typeof(ReflectionSubjectSample) 编译为 ldtoken
   → ObjectModelEmission.cs 约 480 行无条件 hashSet3.Add(subjectId)

2. _trackTypeRef（1577 行）对每个 id 分类：
     else if (IsStructuredValueTypeSubjectId(typeSubjectId))   // 1605 行
         valueTypeSubjectIds.Add(typeSubjectId);
   而该谓词的实现是：
     return !PrimitiveValueTypeSubjectIds.Contains(subjectId);
   → 任意非基元 subject id 都被判为「结构化值类型」，包括引用类型

3. _emittedValueTypeSubjectIds = new HashSet(valueTypeSubjectIds)   // 1701 行

4. GeneratedModule.cs:292 无条件为集合中每个 id 发射 typedef
     typedef CHAOS_IL2CPP_INT32 <sym>;

5. 同时引用类型路径发射了 struct chaos_type_<id>（正确），
   valuetype 路径也发射了 struct chaos_valuetype_<id>
   → typedef 与 struct 同名、底层类型不同 → error C2371
```

**真正的修复点在第 2 步**：该分类判据需要真实类型形状
（`ManagedTypeModel.IsValueType`），而不是「非基元即值类型」的名字启发式。
该启发式**已有 `ReadOnlyCollection<T>` 特例补丁**（`IsReadOnlyCollectionTypeSubjectId`），
本次供体类型是同一缺陷的第二个受害者 —— 说明这是**通用缺陷**，特例补丁只会不断累积。

**注意**：`referenceTypeSubjectIds` 在 1848 行才填充，晚于 `_trackTypeRef`（1577），
所以「在该处查询 referenceTypeSubjectIds」的守卫是无效的（已实测确认）。

### 触发点定位（早期假设，部分被上述链条取代）

`ObjectModelEmission.cs` 约 1757 行：遍历指令的 `TargetReference`，把类型加入
`hashSet3`，注释说明「boxing / LdObj / StObj 指令需要 chaos_boxed_type_* 定义」。

**但已核实**：供体的测试方法体里**没有** box/LdObj/StObj 指令，只有
`typeof(ReflectionSubjectSample)` 与 `GetMethod(...)` 调用。因此触发点不是指令，
而更可能是**类型声明本身**被纳入 `hashSet3` —— 这一点**尚未最终确证**。

### 额外约束（实测）

供体需同时提供 field + property + event + method + 接口 + attribute。
**扫描 `System.Private.CoreLib` 全部公开类型：没有任何类型同时具备公开实例字段与事件**
（具备公开字段的仅 `ValueTuple<T>`、若干 Attribute、`StrongBox<T>`）。
→ **不能简单用 BCL 类型替换自定义供体。**

## 3. 候选方案

### 方案 A：让工厂表达式不进入测试方法体（推荐）

把 `typeof(X)...` 求值移到**探针侧**，测试方法只接收已求得的值。

- **优点**：供体不再出现在生成的测试方法 IL 中 → 不进翻译闭包 → 根因消除；
  不触碰 codegen 的类型形状判定（风险最低区域）
- **代价**：ATG 需增加"值传递"机制（探针求值 → 序列化为表达式 → 测试引用）；
  现有 `KnownTypeFactories` 是纯字符串替换，需要新通路
- **风险**：中（改 ATG 架构，但不改 codegen）

### 方案 B：修 codegen 的 boxed 定义条件

定位并把「供体这种类型不需要 boxed 定义」的判定加进去。

- **优点**：直击生成物异常
- **代价**：`ObjectModelEmission` 的 `hashSet3` 逻辑影响面大（跨程序集类型引用、
  泛型参数、boxing 都走这里），误改会波及所有 chunk
- **风险**：高（我已在此处尝试 3 次未果，超出规则允许的迭代次数）

### 方案 C：改供体类型形状，规避 boxed 路径

让供体成为 **static class**（对照：`SubjectInstanceFactory` 是 static class，
在闭包内但无此问题）。

- **障碍**：工厂需要 `GetField("SampleField")`（实例字段）与
  `GetConstructor(EmptyTypes)`（实例构造）——static class 无法提供
- **变体**：把 field/ctor 需求改由 **BCL 类型**（如 `ValueTuple<int>` 有 `Item1` 字段）
  满足，供体只提供 property/event/method/接口/attribute
- **风险**：中（需拆分工厂到多个来源，验证每个来源的元数据确实可用）

### 方案 D：回退到 `c61ba23d6` 之前

- **优点**：立即解除阻断
- **代价**：验证真实性退回 16/29，`[UNVERIFIED]` 回到 425
- **定位**：仅作为兜底；除非 A/B/C 都不可行

## 4. 建议

**先做方案 A**。理由：
1. 它在**产生问题的层次**（ATG 生成什么代码进方法体）解决，而不是在下游（codegen
   类型形状）打补丁
2. 不触碰 `hashSet3` 这类高影响面逻辑——那正是我三次尝试失败的区域
3. 探针侧求值本来就更合理：断言期望值已经在探针里算过，工厂表达式只是为了构造
   **调用主体**；把主体也移到探针侧是概念一致的

**不建议再做方案 B 的盲试**。已用掉 3 次修复机会（扫描终止符/闭包排除集/发射顺序），
符合项目规则的停止条件；再试需要先有确证（例如打印 `hashSet3` 的实际内容）。

## 5. 下一步（方案 A 的最小验证）

1. 确认 `typeof(...)` 是唯一把供体拉进闭包的引用（grep 生成物中供体出现的所有位置）
2. 设计探针→测试的值传递：供体实例能否以**已构造的值**（如 `Type` 对象）传给测试方法
3. 小范围验证：只改 `Type`/`MemberInfo` 两个工厂（无 field/event 需求），跑 pipeline
   看 C2371 是否消失——这会验证「供体进闭包」这一因果链

## 6. 关联

- 记忆：`reflection-donor-type-codegen-blocker`
- 提交：`c61ba23d6` / `a9fa0def7` / `4941dcace`
- 状态：`docs/dev/in-progress/reflection-production-readiness/STATUS.md`
