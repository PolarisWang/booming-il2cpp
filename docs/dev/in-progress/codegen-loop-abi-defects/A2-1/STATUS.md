# A2-1 — descriptor 扩展设计（用户选 2b：纳入元数据管线）

```yaml
task_id: A2-1
task_type: plan
phase: design
parent_task_id: codegen-loop-abi-defects
source_task_id: codegen-loop-abi-defects
source_relation: roadmap-child
clearance_source: parent-roadmap
entry_skill: dev-brainstorm
blocking_questions: []
question_clearance: cleared
clearance_confirmed_by_user: true
lifecycle_status: in-progress
```

> ⚠️ **边界变更**：用户拍板 **2b** —— 把 property/field/event 元数据发射管线
> **纳入本 roadmap**。这超出原 §2 已批准边界（原定仅
> `remaining_stubs.cpp` / `reflection_query_model.h` / registry）。
> 按 dev-roadmap 规则，边界扩展须显式记录并重估阶段。

## 🔴 决定性发现：`Type::GetProperty/GetField/GetEvent` 是**故意 abort** 的桩

`RuntimeHelperShapeRegistry.CoreStubs.Part1.S15.cs`：

```csharp
// RegisterTypeGetProperty（:99）—— 三个姊妹桩同形
var src = RenderSimpleExternalRuntimeHelper("CHAOS_IL2CPP_INTPTR", symbol, paramSig,
[
    $"    {voidExprs};",
    "    CHAOS_IL2CPP_FAIL();",     // ← 故意 abort
    "    return 0;",
]);
```

同样的 `CHAOS_IL2CPP_FAIL()` 桩出现在：
- `Type::GetField`（`:41`）
- `Type::GetEvent`（`:72`）
- `Type::GetProperty`（`:103`）

而 `CHAOS_IL2CPP_FAIL()`（`native_types.h:106`）无 hook 时走 `CHAOS_IL2CPP_ABORT()`。

**⇒ 这解释了 si=54/56 的 `[ABORT-FAULT]`**：
其 C# 是 `typeof(string).GetProperty("Length")!.GetSetMethod()`，
`GetProperty` 直接 abort。**不是循环问题，也不是 A1 的槽位问题。**

（也解释了为何 stderr 无 `[ASSERT FAIL]` —— 在执行到断言之前就 abort 了。）

## 为什么这些桩要 abort

因为**解析它们所需的元数据不存在**：

| 证据 | 结果 |
|---|---|
| chunk 生成物含 `ReflectionQueryPropertyDescriptor` 表？ | ❌ 0 命中 |
| chunk 调用 `ChaosRegisterExternalType`（唯一注册入口）？ | ❌ 全部 0 |
| `reflection_metadata_impl.h` 含 property 表？ | ❌ 0 命中 |
| `kTypeObject` 的 properties 字段 | `nullptr, 0` |

且 `reflection_metadata_impl.h:3-6` 自述：

> "These tables are compiled into runtime-core and shared across all AOT families.
>  **Per-family type metadata may be generated in a future
>  per-family native-aot.metadata.h.**"

⇒ 这是**已记录的未来工作**，本 roadmap 现在把它纳入（2b）。

## 2b 的真实工作分解

| # | 工作 | 层 | 现状 |
|---|---|---|---|
| **B1** | `ReflectionQueryPropertyDescriptor` / `...Field...` / `...Event...` **发射管线** | codegen | **完全不存在** |
| **B2** | 从 IL/metadata 提取 property/field/event 的 subject_id、name、type、flags | codegen | 不存在 |
| **B3** | 生成 `native-aot.metadata.h`（per-family 元数据）+ 接入注册 | codegen + native | 不存在 |
| **B4** | `ChaosRegisterExternalType` 在 chunk 侧接线 | native | 定义了但无人调用 |
| **B5** | 三个 FAIL 桩改为**真实解析**（查 registered descriptor） | native | 当前 abort |
| **B6** | `constant_value` 字段扩展 + `GetConstantValue` 实现 | 跨层 | 依赖 B1 |

**B1–B4 是基础设施**（服务的不止本 roadmap 的 15 项，而是所有反射成员查询）；
**B5–B6 是本 roadmap 的原始目标**。

## 三优先级重估（2b 下）

- **P1 性能**：descriptor 表是 constexpr 静态数据，零运行时开销；
  查询是 O(property_count) 线性扫（与既有 field/method 一致）。**无降级**
- **P2 架构**：新建一条元数据发射管线。**风险**：
  - 四层边界（ATG/Codegen/TPG/Python）—— 元数据发射归 **Codegen** 层，
    产出 `.generated.h`/`.json`，不越界
  - descriptor 是 ABI 结构，`flags` 注释要求 "must be last"
    ⇒ **新字段须加在 `flags` 之前**，或调整注释约定并核查所有初始化点
- **P3 热更**：descriptor 是编译期常量，不影响 hotupdate 路径。**无影响**

## 风险

| 风险 | 等级 | 缓解 |
|---|---|---|
| **工作量大（≥ 本 roadmap 其余全部之和）** | 🟠 高 | 拆为 B1–B6 独立子任务，逐项验证 |
| descriptor ABI（`flags` 必须最后） | 🟠 中 | 新字段插在 `flags` 之前；核查全部初始化点 |
| 元数据体积膨胀（每类型每成员一条） | 🟡 中 | 先量 CoreLib 的 property/field/event 总量 |
| 与并发线冲突（同一批 codegen 文件） | 🟠 中 | 开工前核对暂存区 |

## 下一步

**B1 前置测量**：CoreLib 有多少 property/field/event 需要发射？
决定表结构的规模与分页策略。

---

## ✅ B1 前置测量结果（决定设计规模）

数据源：reflection chunk 的 `aot-manifest.json`（1298 entries）

| subjectKind | 数量 |
|---|---|
| `method` | **1273** |
| `type` | 18 |
| `field` | **7** |
| **`property`** | **0** ← 不存在该 category |

**关键**：manifest **没有 `property` 这个 subjectKind**。
属性只以访问器**方法**的形式出现（`get_Length` / `set_*`），
共 15 个含 `get_` 的 subject —— 但它们都是**方法**，不是属性对象。

⇒ `Type.GetProperty("Length")` 之所以必须 abort：
**闭包里根本没有"属性"这个实体**，只有它的 getter/setter 方法。

### 对设计的含义

元数据管线不能只"发射已有数据" —— 需要**从访问器方法反推属性实体**
（IL metadata 里 `PropertyDef` ↔ `MethodDef` 存在 `PropertyAccessor` 关联表，
需从 PE metadata 读取）。

这进一步抬高了 B1/B2 的成本：不只是"序列化"，而是**引入 PropertyDef 读取**
（当前 DllScanner/ATG 只读 MethodDef/FieldDef/TypeDef）。

### 规模估计

- **field 仅 7 条**（几乎可忽略）
- **event 0 条**
- **property 0 条**（需新建；数量取决于 closure 的 PropertyDef 数，**未测**）

**待测**：closure 的 PropertyDef 总量 → 决定表规模与是否需要分页。

---

## ✅ 重大更正：元数据读取能力**已存在**，缺口只在发射

前文推测「需新建 PropertyDef 读取」—— **实测推翻**。能力链已完整：

| 环节 | 状态 | 位置 |
|---|---|---|
| 读 `metadataReader.PropertyDefinitions` | ✅ 已实现 | `LoaderStage.AssemblyLoading.cs:176` |
| 读 `GetPropertyDefinition` + **`GetAccessors()`**（Property↔Method 关联） | ✅ 已实现 | 同上 `:177-180` |
| `LoadProperties(...)` 产出模型 | ✅ 已实现 | 同上 `:32` |
| `LoadedAssemblyModel.Properties` | ✅ 已填充 | `:87`，字段定义 `LoaderModels.cs:104` |
| 进 `linkedWorld.Properties`（codegen 可见） | ✅ 已可见 | `CodeGenStage.cs:487`（用于收集 assembly 名） |
| **发射 `ReflectionQueryPropertyDescriptor[]`** | ❌ **不存在** | —— |
| **注册（`ChaosRegisterExternalType`）** | ❌ chunk 侧无人调用 | —— |

⇒ **2b 的缺口是「发射 + 注册」，不是「读取」。** 这大幅降低了 B1/B2 的成本
（原估计需引入 PE metadata 的 PropertyAccessor 读取 —— 已有）。

### 修正后的工作分解

| # | 工作 | 状态 |
|---|---|---|
| ~~B1 读 PropertyDef~~ | ✅ **已存在** | —— |
| ~~B2 提取 subject_id/name/type/flags~~ | 🟡 模型已有；需**映射到 descriptor 形状** | 中小 |
| **B3 发射 `native-aot.metadata.h`** | ❌ 不存在 | **中** |
| **B4 `ChaosRegisterExternalType` chunk 侧接线** | ❌ 不存在 | 小 |
| **B5 三个 FAIL 桩改为真实解析** | ❌ 当前 abort | 小（B3/B4 完成后） |
| **B6 `constant_value` 扩展** | ❌ 依赖 B3 | 小 |

**关键洞察**：B3/B4 完成后，`Type::GetProperty/GetField/GetEvent` 的 abort 即可
改为查表返回真实 handle —— 这**一举解决本 roadmap 的 15 项 + 所有成员查询类 subject**。

## Terminal Notes

- 本轮（A2-1）产出**设计 + 决定性取证**，非实现
- 2b 使本 roadmap 阶段扩为 P4a（B3–B4 发射+注册）+ P4b（B5–B6 语义）
- **成本已下修**：读取能力既有，缺口是发射/注册
- **收益已上修**：不止 15 项，而是所有反射成员查询


