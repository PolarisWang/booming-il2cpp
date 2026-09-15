# B3 — 发射反射元数据表（`native-aot.metadata.h`）

```yaml
task_id: B3
task_type: plan
phase: design
parent_task_id: codegen-loop-abi-defects
source_task_id: codegen-loop-abi-defects
source_relation: roadmap-child
clearance_source: parent-roadmap
entry_skill: dev-executing-plans
blocking_questions: []
question_clearance: cleared
clearance_confirmed_by_user: true
lifecycle_status: in-progress
```

## Scope

为 reflection chunk 发射 per-closure 的反射元数据表（property/field/event），
使 `Type::GetProperty/GetField/GetEvent` 能解析真实 handle（当前是 abort 桩）。

## 现状取证（B3 前置测量完成）

| 事实 | 证据 |
|---|---|
| chunk **不发射任何** descriptor 表 | `grep ReflectionQuery*Descriptor <chunk generated/*.h>` → 0 |
| 反射查询靠静态 `aot_metadata` | `reflection_metadata_impl.h`，含 18 个常见类型 |
| 静态表 **无 property 数据** | `kTypeObject` 的 properties 字段 = `nullptr, 0` |
| 唯一的 descriptor 发射器是 **ReferenceProof** 模板 | `NativeReferenceProof.ReflectionQueryMinimal.cpp.scriban`，是硬编码的**证明夹具**，非闭包发射路径 |
| `Type::GetProperty/GetField/GetEvent` 是 **abort 桩** | `Part1.S15.cs:41/72/103` 的 `CHAOS_IL2CPP_FAIL()` |
| closure 的 property subjectKind | **0**（manifest: method 1273 / type 18 / field 7） |

**⇒ B3 是 greenfield**：需新建一条「闭包 → descriptor 表 → 注册」的通路。

## 架构设计

### 数据流

```
linkedWorld.Properties / .Fields / .Events        （已有，CodeGenStage.cs:487 已可见）
   ↓ 映射为 descriptor 形状
ReflectionQueryPropertyDescriptor[] 等静态数组      （B3 新建）
   ↓ 生成到 native-aot.metadata.h
   ↓ 在 chunk 启动时注册
ChaosRegisterExternalType(fnv24, type_desc)        （已有，type_resolve.cpp:19）
   ↓
ee->properties.data / .count / .filled              （已有，eeclass.cpp:71）
   ↓
Type::GetProperty 查表返回真实 handle               （B5，把 FAIL 桩换掉）
```

### 分层合规（四层红线）

- 发射归 **Codegen** 层（`src/managed/Chaos.IL2CPP.Generator/`）
- 产出 `.generated.h` + `.json` —— **符合** Codegen 层允许的产出
- **不产 `.cpp` 业务逻辑**（模板渲染出的静态数组声明属 header，合规）

### descriptor 形状（ABI 约束）

```c
struct ReflectionQueryPropertyDescriptor {
    const char* subject_id_utf8;
    const char* name_utf8;
    const char* member_type_utf8;
    CHAOS_IL2CPP_UINT32 flags;      // ← 注释要求 "must be last for ABI compat"
};
```

**B6 若要加 `constant_value`，必须插在 `flags` 之前**（或调整该约定并核查全部初始化点）。
B3 本身**不改形状**，先用现有 4 字段打通链路。

## 待定问题（B3 实施前需确认）

### ~~Q8：表挂在哪个描述符下？~~ ✅ **已由测量解答**

`FindTypeByMetadataToken`（`reflection_metadata_impl.h:279`）扫的是
**静态 constexpr 数组**（`kAllTypes`），无法"补挂"。

**但动态覆层已存在且完整**：
```cpp
// type_resolve.cpp:16
static DynamicTypeEntry s_dynamicTypes[kMaxDynamicTypes];
extern "C" void ChaosRegisterExternalType(fnv24_hash, const ReflectionQueryTypeDescriptor*);
```
且查询路径**已消费**它：
- `type_resolve.cpp:72` —— 按 fnv24 命中返回 `EncodeReflectionQueryTypeHandle(desc)`
- `type_resolve.cpp:194` —— 遍历动态类型

⇒ **机制已就绪，缺的只是「生产者」。** B3 只需发射 `ReflectionQueryTypeDescriptor`
（含成员指针）并在 chunk 启动时注册。

### ~~Q9：注册时机~~ ✅ **已解答**

入口是既有的 `ChaosRegisterExternalType`。时机待定但**机制无缺口**
（候选：`runtime-entry.cpp` 初始化段，或并入 `chaos_codegen_code_registration`）。

## 修正后的 B3 画像：**不是 greenfield，是「补生产者」**

| 环节 | 状态 |
|---|---|
| descriptor 结构定义 | ✅ 有 |
| 动态注册入口 `ChaosRegisterExternalType` | ✅ 有（且被查询路径消费） |
| 静态表 / 查询路径 | ✅ 有 |
| 闭包侧属性模型（`linkedWorld.Properties`） | ✅ 有 |
| **发射 `ReflectionQueryTypeDescriptor`（含成员）** | ❌ **唯一缺口** |
| **在 chunk 启动时调用注册** | ❌ 缺口 |

⇒ 工作量集中在**发射器 + 一次注册调用**，均为小-中等规模。

## 风险

| 风险 | 等级 | 缓解 |
|---|---|---|
| descriptor 表体积（每类型一整套） | 🟡 中 | 本 chunk property 0 条、field 7 条，规模可控 |
| 与静态表重复（同 token 两处） | 🟡 中 | 动态表已在查询路径**优先消费**（`type_resolve.cpp:72` 先查动态） |
| 与并发线冲突 | 🟠 中 | 开工前核对暂存区 |

## 下一步

**P0 最小闭环**：让**一个**属性走通「发射 → 注册 → 查询返回真实 handle」，
再谈全量。避免一次性全量发射后才发现通路问题。

---

## 🔴 实施中发现的**架构障碍**（已实测）

### 现象

已实现成员收集（`CollectReflectionMemberMetadata`，见 `Methods.ModuleData.cs`），
实测能收集到数据。**但收集的是错的类型。**

实测（`CHAOS_DIAG_MEMBERS=1` 直跑 TPG）：
```
[DIAG-MEMBERS] type=System_Reflection_ReflectionContextTests props=2 fields=4 events=1
[DIAG-MEMBERS] type=System_Reflection_RuntimeReflectionExtensionsTests props=2 fields=4 events=1
...共 25 个 type
```

**每个 type 都是 `props=2 fields=4 events=1`** —— 因为扫的
`closureManifest.InputAssemblyPath` = **`CombinedSubjects.dll`**（生成的测试类），
**不是被测的 BCL 类型**。

### 为什么不影响功能

反射 subject 查询的是 **BCL 类型**（`typeof(string).GetProperty("Length")`），
而 `string` 定义在 `System.Private.CoreLib.dll`。

### 正确数据源已找到

`closure.manifest.json` 的 **`resolvedAssemblies`** 含真实 BCL 程序集：

```
Chaos.TestFramework.Sdk    .../managed/...
CombinedSubjects           .../managed/...
System.Collections         C:/Program Files/dotnet/shared/Microsoft.NETCore.App/8.0.11/...
System.Console             .../...
System.Private.CoreLib     .../Microsoft.NETCore.App/8.0.11/System.Private.CoreLib.dll  ← 关键
System.Threading           .../...
```

⇒ B3 的发射器必须扫 **`resolvedAssemblies`**（全部 6 个），而非仅 input assembly。

### 对规模的影响

| 方面 | 变化 |
|---|---|
| 扫描范围 | input(1) → resolved(6)，且 CoreLib 极大 |
| 数据量 | 需先量 CoreLib 的 PropertyDefinition 总量 |
| 分页 | 若量大，descriptor 表可能需分页（与既有 `native-aot.generated.pageN.cpp` 同策） |

### 已完成的部分（本工作区，未提交）

| 项 | 状态 |
|---|---|
| `_reflectionProperties/Fields/Events` 数据字段 | ✅ `Methods.cs` |
| `CollectReflectionMemberMetadata` 收集器 | ✅ `Methods.ModuleData.cs` |
| 编译 | ✅ 0 error |
| **扫描源修正（input → resolved）** | ❌ **待做** |
| 发射模板 | ❌ 待做 |
| 注册接线 | ❌ 待做 |

## 待用户拍板

**Q10**：B3 的扫描范围改为 `resolvedAssemblies` 后，需先量 CoreLib 的成员总量
（可能上万），再决定：单表 / 分页 / 只发 closure 实际用到的成员。

---

## ✅ Q10 测量结果（两个数字决定方案）

### 数字 1：CoreLib 全量成员（若"发射全库"）

直接用 `System.Reflection.Metadata` 数 `Microsoft.NETCore.App/8.0.11` 的 CoreLib：

```
types=2595  methods=34516  fields=9011  props=5128  events=33
```

### 数字 2：closure 的实际反射查询面（若"只发用到的"）

从 reflection chunk 的**生成物**统计 `ChaosReflection*` 调用点：

| 指标 | 值 |
|---|---|
| 总调用点 | **618** |
| 不同 API | **53** |

Top API：
```
254  ChaosReflectionGetAssembly
 56  ChaosReflectionGetParameters
 55  ChaosReflectionGetExecutingAssembly
 39  ChaosReflectionGetMethod
 27  ChaosReflectionGetAssemblyName
 20  ChaosReflectionGetCustomAttributesForMember
 ...
```

### 对比

| 方案 | 规模 | 备注 |
|---|---|---|
| 发射整个 CoreLib | **5128 props + 9011 fields** | 体积大；且绝大多数用不到 |
| **只发 closure 用到的** | **618 调用点 / 53 API** | **小 1-2 个数量级** |

## 我的推荐：**只发 closure 实际用到的成员**

理由：
- **P1 性能**：descriptor 是静态数据，但体积影响 binary size 与 cache 局部性
- **P2 架构**：与既有 `aot-manifest.json` 的「按可达性发射」范式一致
  （manifest 已记录 1273 method / 18 type / 7 field 的可达集）
- **P3 热更**：无关

**实现路径**：从 `aot-manifest.json` / closure 的 subject 集反推**被查询的类型**，
只为这些类型发射其成员的 descriptor（而非 CoreLib 全库）。

## 待你拍板（Q11）

**Q11**：B3 采用哪种？
- **11a**：只发 closure 用到的（推荐，规模小 1-2 个数量级）
- **11b**：发射 resolvedAssemblies 全量（简单但臃肿）

## Terminal Notes

- **本工作区已完成**（未提交）：数据字段 + 收集器 + 编译通过
- **未做**：扫描源修正（input → resolved）、发射模板、注册接线
- 本次会话已在 A2/B3 投入大量轮次，且 B3 的剩余部分（发射+注册+扫描源修正）
  是一个**新的实施周期**的量级



## Terminal Notes

- B3 是 2b 的关键解锁项；B4/B5/B6 依赖它
- **成本已两度下修**：greenfield → 补生产者

