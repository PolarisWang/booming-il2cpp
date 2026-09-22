# P1-02 诊断：`SubjectInstanceFactory.Create<T>` 泛型方法的 canonical-body 优先级

> 日期：2026-09-22
> 状态：**已识别，未解决**（升格为独立任务，不阻塞 P1 交付）
> 触发：P1-01（ATG 消费 shape registry）落地后，AOT 侧 `real` 未提升
> 关联：`roadmap-v3-01.md` §4 Phase 1

---

## 0. 一句话结论

P1-01 让 ATG 停止发桩后，AOT 侧 `real` 仍是 2（JIT 侧 199）。根因**不是** P1-01 的缺陷，
而是下游的 `SubjectInstanceFactory.Create<T>()` 在 AOT 侧**没有接通**：它被标记为
`has-canonical-body` + `generated-direct-call`，绑到 `InterpreterEntryDirect`，
**绕过了 `TryCreateExternalRuntimeHelperDefinition` 整条 shape 决策链**。

给 `Create` 注册泛型 shape **不生效**（已实测，见 §3）。

---

## 1. 现象与量化（text-json chunk）

| 指标 | main 基线 | P1-01 后 | 说明 |
|:-----|:---------:|:--------:|:-----|
| `AOT-STUB-GAP` 标记数 | 211 | **146** | P1-01 生效 ✅ |
| stubGap | 176 | **130** | -46 ✅ |
| unassertable | 65 | 49 | |
| **factoryGap** | **0** | **22** | ⚠️ **新出现** |
| **real (AOT)** | 2 | **2** | ❌ 未提升 |
| real (JIT) | — | **199** | |

**`factoryGap` 22 条全在 `Utf8JsonWriterTests`**，与 `Create<Utf8JsonWriter>` 一一对应。

### 1.1 factoryGap 的语义（分类器定义）

`fact_chunk.py` 的 `_EXCLUDED_FROM_GATE` 说明：

> `factoryGap` — the test's `SubjectInstanceFactory.Create<T>()` returned null in AOT,
> so the emitted null-guard raised before the call.

即：**方法根本没跑到**。它被排除在 gate 分子/分母之外，所以修它不直接影响 gate 占比，
但它是"方法真的执行"的必要条件。

---

## 2. 根因链（逐层实测）

### 层 1：`Create<T>` 生成了 generic 条目，但绑到解释器

生成物 `native-aot.generated.cpp`：

```cpp
extern "C" CHAOS_IL2CPP_INTPTR
Chaos_TestFramework_Sdk_Chaos_TestFramework_SubjectInstanceFactory_Create_Utf8JsonWriter__generic(void);

// 派发表：
{ reinterpret_cast<void*>(&..._Create_Utf8JsonWriter__generic),
  reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, 0 }
```

**它不是 catch-all helper**，而是走 `InterpreterEntryDirect`。

### 层 2：`Create` 被标记为 canonical body

`aot-manifest.json`：

```json
{ "assemblyName": "Chaos.TestFramework.Sdk", "subjectKind": "method",
  "subjectId": "Chaos.TestFramework.Sdk/Chaos.TestFramework.SubjectInstanceFactory::Create`1:!!0()",
  "reason": "generated-direct-call",
  "bodyAvailability": "has-canonical-body" }
```

### 层 3：该标记使其绕过 shape 决策

`TryCreateExternalRuntimeHelperDefinition`（`NativeAotLoweringPlanner.ExternalRuntimeHelpers.cs:566`）
的入口在 **589 行**才调 `TryMatchGenericShape`。而 `Create` 因为
`has-canonical-body`，在更早的路径就被判定为"有 AOT body"，不会被送到这里。

> 注：589 行上方的注释写着 "check BEFORE `_methodsBySubjectId`"，但那指的是 **622 行**的
> 另一个 `_methodsBySubjectId` 检查。**158 行的守卫**（在 `TryBuildExternalRuntimeAotIrJson` 内）
> 是另一条路径。两个同名检查点容易混淆——这是本次排查绕弯的主要原因。

### 层 4：结果

`Create<Utf8JsonWriter>` → 解释器 → 返回 null → `SubjectInstanceFactory.Create<T>` 抛
`InvalidOperationException`（其自身有 null 检查）→ 但 ATG 生成的代码期望的是
`WriteStartArray` 等的 `InvalidOperationException`，异常来源不符 → `factoryGap`。

---

## 3. 已尝试且**无效**的修法（勿重复）

**给 `Create` 注册泛型 shape，指到 `ChaosRuntimeHelpersGetUninitializedObject`。**

- 位置：`RuntimeHelperShapeRegistry.CoreStubs.Part1.S10.cs`，`RegisterRuntimeHelpers` 之后
- 做法：`RegisterGeneric` + `TypeDisplayNamePrefix: "Chaos.TestFramework.SubjectInstanceFactory"` + `MethodName: "Create"`，resolver 解析 `Create<T>` 取出 `T`，拼 `chaos_mt_*` 符号
- 结果：**factoryGap 仍为 22，生成物未变**
- 已回滚（该改动未提交）

**为什么无效**：如 §2 层 3，`Create` 根本不进入 `TryMatchGenericShape` 的调用点。

> 踩坑记录：第一次 prefix 写成了 `"Chaos.TestFramework.Sdk/Chaos.TestFramework.SubjectInstanceFactory"`。
> `GetTypeDisplayNameFromSubjectId` 会**剥掉 assembly 前缀**（只保留 `/` 后的部分），
> 所以正确的 prefix 是 `"Chaos.TestFramework.SubjectInstanceFactory"`。修正后仍无效——
> 但这说明**不能用"注册后无效"直接推断前缀写对了**，两个问题叠加时容易误判。

---

## 4. 后续入口（三条候选，均未评估）

| 路径 | 做法 | 风险 |
|:-----|:-----|:-----|
| **(a) 改泛型规划策略** | 让带 canonical-body 的泛型方法在特定条件下优先走 shape | 触及 AOT generic-method 规划核心 |
| **(b) 改 ATG 侧构造方式** | 让 ATG 不用 `Create<T>()`，改用别的实例构造路径 | 影响 ATG 全部 subject 生成 |
| **(c) 给该类型补真正的 AOT body** | 让 `Create<T>` 有 native 实现而非走解释器 | 泛型实例化面大 |

**优先级判断**：`factoryGap` 被排除在 gate 之外，且只影响 22 条（单一类型）。
**不建议**在 P1 阶段投入——应等 Phase 3（P0 核心 API 补齐）时，连同 `Utf8JsonWriter` 的
真实写入状态机一并处理，那时该类型的 AOT body 本来就要补。

---

## 5. 与 P1-01 的关系

**P1-01 是成功的**：它消除了 65 个错误的 `AOT-STUB-GAP` 标记，且守卫测试（470 checks）
+ 两次负控已验证。AOT `real` 未提升的原因是**下游独立缺口**，不是 P1-01 的回退。

roadmap-v3 §4 Phase 1 的 Exit Criteria 需要修正为：

> ~~AOT `real` 提升~~ → **`AOT-STUB-GAP` 标记数下降**（这是 P1-01 的直接产出，且已验证 211→146）
