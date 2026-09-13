# 反射探测供体 (ReflectionSubjectDonor) — 方案对比与深度分析

> 日期：2026-09-13
> 状态：待用户决策
> 前序：`DONOR-DESIGN-ANALYSIS`（因果链确证）、提交 `4941dcace` / `4ec3d4ce4` / `942ea3349`

---

## 0. 事实基础（全部实测，非推断）

### 0.1 阻断的真实形态

```
error C2371: 'chaos_valuetype_..._ReflectionSubjectSample': redefinition; different basic types
```
同一 header 中同时存在：
- 行 9：`struct chaos_valuetype_..._ReflectionSubjectSample { 字段... };`
- 行 1805：`typedef CHAOS_IL2CPP_INT32 chaos_valuetype_..._ReflectionSubjectSample;`

### 0.2 影响面（关键：这不是普遍缺陷）

扫描 `artifacts/foundation-dll/**/native-aot.generated.header.h` 共 **106 个 header**：

| chunk | `struct chaos_valuetype_*` 数 | `typedef` 数 | 冲突 |
|---|---|---|---|
| 反射 chunk | **有**（≥1） | 19 | **≥1** |
| 其余所有 chunk | **0** | 10–19 | **0** |

→ **只有反射 chunk 触发**，且触发前提是「同一个符号既有 struct 定义、又被补 typedef」。
其他 chunk 只有 typedef，无 struct，故无冲突。

### 0.3 供体确实进入了翻译闭包

`native-aot.generated.page*.cpp:2414` 存在
```
extern "C" void ..._ReflectionSubjectSample_SampleMethod_System_Int32(...)
```
即供体的 `SampleMethod` 被**完整 AOT 编译**（非仅声明）。

### 0.4 三个 typedef 发射点（均已修复各自缺陷，但均未解除阻断）

| # | 位置 | 注释标记 |
|---|---|---|
| ① | `Methods.Remaining.cs` ~1056 | `(safety net: TPG stub declarations)` |
| ② | `GeneratedModule.cs` ~289 | `(opaque 32-bit managed value types)` |
| ③ | `NativeAotEmitter.Shared.cs` ~547 | `(post-scan: TPG extern declarations)` ← 实际产出者 |

### 0.5 未解疑点

对最终落盘 header **离线重放**修复后的收集逻辑 → **能正确识别 struct**；
但**在线运行仍补 typedef**。矛盾只可能来自：
- (a) post-scan 拿到的 `sources[headerIdx].Contents` 与最终落盘内容不同步，或
- (b) 存在第四个发射点。

---

## 1. 方案 A：探针侧求值（不改 codegen）

### 做法
把 `typeof(X).GetMethod("M")` 这类表达式从**测试方法体**移到**探针**里求值，
测试方法只接收已求得的值（如序列化的 Type 名 / 句柄），不再直接引用供体类型。

### 优点
- **根因消除**：供体不再出现在测试方法 IL 中 → 不进 AOT 翻译闭包 → 三个
  typedef 发射点全都不会看到它
- **不触碰 codegen 的形状判定**：那正是五轮尝试失败的区域（`IsStructuredValueType
  SubjectId` 名字启发式，已有 `ReadOnlyCollection` 特例补丁）
- **概念一致**：断言的**期望值**本来就在探针里算过；工厂表达式是为构造**调用主体**，
  把主体也移到探针侧是同构的做法

### 缺点
- **改动 ATG 架构**：现有 `KnownTypeFactories` 是纯字符串替换
  （`GetInstanceExpression` → 返回一段 C# 表达式文本），要改成"探针求值 + 值传递"
  需要新的序列化通路（参考现有 `ProbeResult.ReturnValueJson` 的机制）
- **类型保真度**：`typeof(X)` 的结果要跨进程传回测试方法，需决定传
  `Type` 对象（不可序列化）还是 `AssemblyQualifiedName` 字符串（需在测试侧反解）
- **验证面变化**：测试方法里构造实例的代码没了，断言的对象来自探针——需确认
  **AOT 侧的调用主体**与探针侧一致（否则断言对象错位）

### 风险
中。改 ATG 架构，但不改 codegen 类型系统。

---

## 2. 方案 B：完成 (a)/(b) 诊断并修根因

### 做法
在 `CollectExistingValueTypeTypedefs` 加临时打印（`existingVT.Count` + 是否含目标符号），
一次运行判定 (a) 还是 (b)，再针对性修复。

### 优点
- **修通用缺陷**：若判定为 (a)，修的是"post-scan 输入不同步"这一**真实架构问题**，
  惠及所有 chunk
- 不引入 ATG 架构变更

### 缺点
- **已用掉五轮**：远超项目规则的三修上限
- **不确定性**：若判定为 (b)（存在第四发射点），还需继续追
- **触碰高影响面**：post-scan 影响所有 chunk 的 header 生成

### 风险
中高。诊断本身成本低（一次运行），但修复的爆炸半径大。

---

## 3. 方案 C：让供体不是"有实例字段的类型"

### 做法
把供体的 field/event 需求改由其它类型满足，使供体成为 **static class**
（对照：`SubjectInstanceFactory` 是 static class，同在闭包内但**无此问题**）。

### 障碍（实测）
- 工厂需要 `GetField("SampleField")` + `GetEvent("SampleEvent")` + 属性 + 方法 + 接口 + 特性
- **扫描 CoreLib 全部公开类型：没有任何类型同时具备公开实例字段与事件**
  （有公开字段的仅 `ValueTuple<T>`、若干 Attribute、`StrongBox<T>`）
- 故供体必须自定义；只能拆成"两个供体"（一个提供 field、一个提供 event），
  或放弃 field/event 的工厂

### 风险
中。可行但需重新验证每个工厂的表达式的元数据确实可用。

---

## 4. 方案 D：回退 `c61ba23d6`

### 做法
撤销真实实例工厂，回到 `Create<T>()`。

### 代价
- 真实断言 **58 → 29**，`[UNVERIFIED]` **396 → 425**
- 即验证真实性修复全部作废

### 定位
仅作兜底。

---

## 5. 架构层统一性分析

### 5.1 方案 A 与既有架构的关系

ATG 现有数据流：
```
探针工程（Probe.cs） ──求值──> ProbeResult.ReturnValueJson ──序列化──> 测试方法断言
```
**期望值**已经走"探针求值 → 序列化 → 测试引用"这条路。

方案 A 是把**调用主体**也纳入同一条路 —— 是**架构收敛**，不是新增抽象。
`ValueGenerator.cs:797` 的注释也提到「Strategy mirrors CSharpExpressionBuilder.
GetInstanceExpression but is argument-agnostic」——说明"实例表达式"本就是
一个跨模块概念，已有两处实现，收敛它有正收益。

### 5.2 方案 B 与既有架构的关系

方案 B 修的是 codegen 的**生成物一致性**问题。但注意：
`IsStructuredValueTypeSubjectId` 的名字启发式已有 `ReadOnlyCollection` 特例补丁，
**本次供体是同一缺陷的第二个受害者**。若不修根因（改用真实类型形状），
第五个、第六个特例还会来。从这个角度看，B 的"修根因"价值更高。

### 5.3 两者不冲突

方案 A 解除**当前**阻断且不碰 codegen；方案 B 修**通用**缺陷。
可先 A 后 B。

---

## 6. 性能分析

| 方案 | 对生成代码性能 | 对 codegen 编译时间 | 对运行时 |
|---|---|---|---|
| A | **改善**：供体不再被 AOT 编译（少一个类型的翻译 + 方法体） | **降低**：生成的翻译闭包变小 | 无影响（测试代码不进生产镜像） |
| B | 无变化 | 无变化（仅改 typedef 收集） | 无影响 |
| C | 改善（供体变小） | 降低 | 无影响 |
| D | 回退：供体仍被翻译 | 回退 | 无影响 |

> 注意：供体是**测试夹具**，其翻译产物只存在于测试镜像中，不影响生产代码路径。
> 因此"性能"维度在此决策中的权重**低**；主要权重是**解除阻断的成本与风险**。

---

## 7. 推荐

**分两步：先 A（解除阻断），再 B（修根因）。**

理由：
1. **A 的因果链最干净**：供体不进闭包 → 三个发射点全都不触发 → 不依赖
   对 codegen 现有行为的完整理解
2. **B 已五次未果**，且其修复点（post-scan 输入同步性）影响所有 chunk，
   在缺乏确证前不宜动
3. **A 与既有架构收敛**（主体求值走与期望值相同的通路），B 只是打补丁式修正
4. 解除阻断后，B 可以在**有稳定验证基线**的前提下从容做（这恰恰是当前缺失的）

**若选 A**，建议的最小验证顺序：
1. 先只改 `Type` / `MemberInfo` 两个工厂（无 field/event 需求，最容易验证）
2. 跑 pipeline 确认 C2371 消失（验证"供体进闭包"这唯一的因果链）
3. 再推广到其余工厂，逐个确认元数据可用

---

## 8. 附：本轮的增量结论

无论选哪个方案，以下**独立缺陷修复**应当保留（均已在 main）：
- `4941dcace` — struct 名扫描缺换行终止符（`Methods.Remaining.cs`）
- `4ec3d4ce4` — safety-net 的 `existing` 存裸名、`symbol` 是全名
- `942ea3349` — post-scan 的 struct 收集同样缺换行终止符

三处均为**同一族扫描缺陷**，且在 CRLF 环境下必然触发。修它们不解除本阻断，
但都是真实缺陷。
