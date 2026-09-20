# ctor 句柄 ABI 修复 — 设计 v1.01

> **日期**: 2026-09-20
> **上游**: `xml-writeend-stack-and-eh-escape-handoff`（其 EH 逃逸假设已被推翻）
> **本轮性质**: 架构级修复方案拍板（方案 B）

---

## 1. 问题陈述

### 1.1 现象

XML chunk 中 4 个 subject 记 `resultKind=realDefect`（`passed=False`, `caught=True`, `value=0`）：

```
System_Xml_XmlTextWriterTests::WriteEndDocument_2__0
System_Xml_XmlTextWriterTests::WriteEndElement_5__0
System_Xml_XmlTextWriterTests::WriteFullEndElement_6__0
System_Xml_XmlTextWriterTests::WriteEndAttribute_8__0
```

### 1.2 根因（一手实证，非推断）

在 `ChaosXmlWriterWriteEndDocument` 入口插桩后重建 SDK 重跑，只被调用 **1 次**：

```
[WED-TRACE] entered handle=1468802672832 st=0000000000000000
```

- `handle` 是**托管对象堆指针**
- `Resolve()` 期望 `AllocSlot()` 发出的 **1-based 小整数槽位**（`handle-1 < kWritersCap`）
- `Resolve()` 返回 `nullptr` → 函数**静默 return**，从不抛异常
- 随后 ATG 生成的 `CHAOS_EH_THROW("AOT stub did not throw")` 触发 → 这才是 `caught=true` 的来源

### 1.3 机制

```
ChaosXmlTextWriterCreate (native)  ──返回──▶  native slot handle
                                              ✗ 被丢弃
        shape 注册用 CreateVoidAbiSlot() (S19.cs:193)
        ↑ EmitLinearNewObject 对非 Void ctor 返回抛 NotSupportedException
                          │
                          ▼
codegen 推 chaos_object（托管对象指针）上 eval 栈
                          │
                          ▼
后续 callvirt 把托管指针当 handle 传 → Resolve 落空 → 静默 return
```

**native 语义完全正确，勿改。** `InvalidOperationException` / `ArgumentException` 的选择有 probe + .NET 8 双锚点依据。

### 1.4 影响面（mechanism 级，非现象级）

| 范围 | 数量 | 状态 |
|:-----|:----:|:-----|
| 已实证的 subject | 4 | WriteEnd* 全族 |
| shape registry 中 `.ctor` 注册 | ~28 | 未逐个验证 |
| 已知同 pattern 的 native 工厂 | ≥2 | `ChaosXmlTextWriterCreate`、`ChaosXmlTextReaderCreate`(xml_reader_stubs.cpp:476) |

⚠️ **本轮只对 4 个 subject 取得直接证据。** 其余为 mechanism 推断，需在执行期以全量 chunk 对比验证。

---

## 2. 边界拍板

### 2.1 In scope

- 修复「引用类型 `newobj` 的 native 工厂返回值被丢弃」这一机制缺陷
- 覆盖整个句柄式 stub 家族（XmlTextWriter / XmlTextReader / XmlWriter 等所有注册了 handle 语义的 ctor）
- 全量 chunk 对比验证

### 2.2 Out of scope（明确非目标）

- **不改** native stub 的异常语义（已正确）
- **不改** `CHAOS_EH_*` 宏体系
- **不引入**基于裸托管对象指针的全局状态表（见 §4 方案 A 否决理由）
- **不对账** XML chunk 剩余 ~96 项 `realDefect`/`failed`（根因未定，不得外推）
- 不涉及 async 面

---

## 3. 关键约束（已查证）

| 约束 | 证据 | 含义 |
|:-----|:-----|:-----|
| codegen 对引用类型 `newobj` **恒定**分配对象并推 `chaos_object` 上栈，ctor 返回值被设计性忽略 | `ExceptionEmission.Linear.cs:556-583` | 这是要被修改的机制本身 |
| `CreateVoidAbiSlot()` **不能**改回 `CreateNativeIntAbiSlot()` | `Linear.cs:481/527` 抛 `NotSupportedException` → `BuildMethodSourceSafe` 泛型 catch 吞掉 → **整个 subject body 被替换为 stub** | 回退 ABI 是「更糟」，不是修复 |
| `AotCoreIrAbiSlotArtifact` 仅 3 字段，**无法表达**「ctor 返回 handle」 | `TypedIlAndAotCoreIrContracts.cs:255` | 需新增元数据通道 |
| `InvocationTarget` 已有 11 个可选字段的先例 | `NativeAotLoweringPlanner.Models.cs:7-18` | 新增标记与既有模式同构 |
| GC **有** STW compaction | `gc_bgc.h:198`、`gc_bgc.cpp:1253` | 裸对象指针会移动 → 否决方案 A |

---

## 4. 方案对比与裁决

### 方案 A — 「托管对象指针 → 状态」旁路表（否决）

让工厂把状态登记到以托管对象指针为键的表，`Resolve()` 改查该表。

- 与 `stream_stubs.cpp:78-94` 同构，改动最小
- **否决理由**：`g_stream_state` 用**裸指针**做键且无 GC write barrier，而 GC 有 `StwCompact()`。
  compaction 后键全部失效。
- `stream_stubs` 至今未暴露，是因为 fact 测试的时序恰好未触发压缩 —— **这是假绿，不是安全**。
  复制该模式等于把已知定时炸弹扩散到 XML 全家。

### 方案 B — codegen 侧让工厂返回值接入 eval 栈 ✅ **采纳**

在 `EmitLinearNewObject` 的引用类型分支，当该 ctor 被标记为「返回 native handle」时，
把**工厂返回值**（而非 `chaos_object`）推上 eval 栈。

- **优点**：修的是「返回值无处可去」这一机制本身；native 侧 slot 句柄设计
  （含 `AllocSlot`/`Resolve` 边界检查）**完整保留**；无指针生命周期问题；一次覆盖全家族
- **缺点**：动 EH lowering 核心，回归面 = 所有走 `newobj` 的 subject
- **缓解**：标记**默认关闭**，仅对显式注册者生效；全量 chunk 对比兜底

### 方案 C — 显式 receiver 字段（否决）

给类型加字段存 handle，`newobj` 后由 native 写回。

- **否决理由**：本质是方案 B 的变体，但多一次内存往返；且 `XmlTextWriter` 是 BCL 类型，
  改其布局侵入对象模型，影响面远超 B。

---

## 5. 方案 B 设计

### 5.1 components

**C1 — 元数据标记**
`InvocationTarget` 新增可选字段（与 `DirectNativeSymbol` 同构）：

```csharp
bool CtorReturnsNativeHandle = false
```

由 shape 注册侧在注册句柄式 ctor 时置 `true`。

**C2 — shape 注册侧**
`RuntimeHelperShapeRegistry.CoreStubs.Part1.S19.cs` 等处，把句柄式 ctor 的注册
从「仅 `CreateVoidAbiSlot()`」扩展为「`CreateVoidAbiSlot()` + `CtorReturnsNativeHandle=true`」。

⚠️ **P2 架构权衡点**：`ReturnAbi` 仍须是 Void（否则触发 §3 的 `NotSupportedException`）。
标记语义是「**忽略 ctor 返回值，改用工厂返回值**」，与 ABI 的 Void 声明并不矛盾 ——
Void 描述的是**托管语义**（ctor 概念上不返回值），标记描述的是**native 工厂的接线方式**。

**C3 — codegen 发射点**
`EmitLinearNewObject` 引用类型分支（`Linear.cs:556-583`）：
当 `CtorReturnsNativeHandle == true` 时，`EmitEvalStackPush` 的表达式从
`reinterpret_cast<INTPTR>(chaos_object)` 改为**工厂调用的返回值**。

需同时保证 native 符号的返回类型声明为 `INTPTR`（当前 wrapper 声明为 `void`）。

**C4 — native wrapper**
`chaos_external_runtime_..._ctor_...` wrapper 的返回类型从 `void` → `CHAOS_IL2CPP_INTPTR`，
`return ChaosXmlTextWriterCreate(chaos_fn_arg_0);`。

### 5.2 数据流（修复后）

```
newobj XmlTextWriter(TextWriter)
  ├─ 分配 chaos_object（仍需要，托管侧要有对象身份）
  ├─ 调工厂：handle = ChaosXmlTextWriterCreate(text_writer)
  └─ eval 栈推 handle（而非 chaos_object）
        ↓
callvirt WriteEndDocument
  └─ Resolve(handle) → 命中 slot → 抛出正确异常 ✅
```

**为何仍要分配 `chaos_object`**：托管代码可能持有该引用（如作为参数传递）。
分配保留对象身份，只是**不再把它的地址当作 handle**。

### 5.3 错误处理与边缘情况

| 情况 | 处理 |
|:-----|:-----|
| 工厂返回 0（分配失败 / 参数非法） | 推 0 上栈；后续 `Resolve(0)` 返回 nullptr，与原行为一致 |
| ctor 未标记 | 走原路径，行为**由产物的逐字节 diff 验证**不变（见 §5.4-3） |
| 值类型 ctor | 不受影响（走 `ValueType` 分支） |
| delegate ctor | 不受影响（走 `IsDelegateTypeSubjectId` 分支） |
| 无参 reference-type ctor | 不受影响（`Linear.cs:509-521` 独立分支，本任务不触及） |

#### 5.3.1 `newobj` 的四个分支（影响面精确化）

`EmitLinearNewObject` 对引用类型有**多条**独立分支，本任务**只触及其中一条**：

| 分支 | 位置 | 本任务 |
|:-----|:-----|:------:|
| delegate ctor | `Linear.cs:462-478` | ❌ 不触及 |
| 值类型 ctor | `Linear.cs:479-507` | ❌ 不触及 |
| 无参 reference ctor | `Linear.cs:509-521` | ❌ 不触及 |
| **有参 reference ctor** | `Linear.cs:543-580` | ✅ **C3 落点** |

⚠️ **「未标记路径逐字节不变」是待验证命题，不是可直接断言的事实。**
`newobj` 是所有 subject 的公共路径，且本仓库有「guard 注释理由已过时」的先例
（见 `HANDOFF-c1-evidence.md` §3：某条 guard 的注释理由在对应代码支持后已失效）。
**任何「看起来不影响」的判断都必须用产物 diff 证明。**

### 5.4 验证方式

**口径：全量 chunk 对比**（用户已拍板）

0. **前置**：确认工作树无并行 agent 未提交改动（`git status` 逐文件核对）
1. **目标 subject**：4 个 WriteEnd* 从 `realDefect` → `real`
2. **全量扫描**：整个 XML subject 集的 `resultKind` 迁移矩阵，
   确认无「修好 N 个、弄坏 M 个」
3. **反向验证（门槛，非观察项）**：未标记 ctor 必须产出**逐字节相同**的 `.cpp` ——
   实施前后 `diff` 生成的 page 文件，差异只允许出现在已标记的 ctor 调用点
4. **跨 chunk**：至少复跑一个非 XML chunk，确认无全局回归

⚠️ **必须避开陈旧产物陷阱**（记忆 `generated-page-files-stale-orphans` /
`chaos-il2cpp-tool-reinstall-required`）：
- 改 Generator 后**必须重建 TPG**（TPG 内嵌自己的 Generator.dll）
- 清 3 层 hephaestus cache
- 校验 exe 时间戳晚于 codegen 产物

---

## 6. 风险评估

| 风险 | 触发条件 | 后果 | 缓解 |
|:-----|:---------|:-----|:-----|
| **R1 回归面广** | `newobj` 是所有 subject 公共路径 | 大面积 subject 行为漂移 | 标记默认 false；未标记路径**以产物逐字节 diff 证明**不变（§5.4-3）；全量矩阵对比 |
| **R2 wrapper 返回类型不同步** | 只改 codegen 未改 native wrapper 声明 | C 编译错误 / 返回值被截断 | 同一提交内改 C3+C4；用 pipeline 编译门禁拦截 |
| **R3 标记漏注册** | 部分句柄式 ctor 未加标记 | 该类 subject 静默保持原缺陷 | 执行期用 §5.4 全量扫描发现；非静默失败 |
| **R4 陈旧产物误判** | 未重建 TPG / 未清 cache | 结论建立在旧代码上 | §5.4 三条校验 |
| **R5 并发 WIP 污染** | 并行 agent 未提交改动混入 | 归因错乱 | 提交前 `git diff --cached --name-only` 逐文件核对 |

### 6.1 回归面的精确界定（修正）

原设计称未标记路径「逐字节不变」。经核对 `EmitLinearNewObject` 实际结构，
该说法**过强**，修正为：C3 只改**有参 reference ctor 分支**（`Linear.cs:543-580`），
其余三条分支物理上不进入；未标记的**有参 ctor** 则需靠产物 diff 证明等价。

依据（外部交接 `HANDOFF-c1-evidence.md` §3 的教训）：
本仓库存在「guard 注释所述理由在对应代码支持后已失效」的先例 ——
**判断影响面必须基于当前代码结构 + 产物 diff，不能基于注释或直觉。**

---

## 7. 三优先级权衡

| 优先级 | 方案 B 表现 | 裁决说明 |
|:-------|:-----------|:---------|
| **P1 性能最优** | ✅ 优 | `newobj` 路径**无额外运行时开销** —— 只是把已有的工厂返回值接到 eval 栈，不新增查表、不加屏障。对比方案 A 每实例方法都要查哈希表 |
| **P2 架构完美** | ✅ 优 | 修复机制本身而非绕过；保留 native slot 的边界检查；标记设计沿用 `InvocationTarget` 既有模式 |
| **P3 热更适配** | ✅ 中性偏好 | 有标记的 ctor 走直连 native 工厂。Hotpatch 路径不受影响（`_sharedContextSymbols` / `chaos_generic_context` 逻辑不动） |

**无优先级冲突** —— 方案 B 在三个维度上均优于或持平于 A/C，故无需低优先级让位裁决。

---

## 8. 问题清零

### blocking_questions: []

| # | 问题 | 结论 |
|:-:|:-----|:-----|
| 1 | 方案选择 | ✅ **方案 B**（用户拍板） |
| 2 | 修复范围 | ✅ **整个句柄家族**（用户拍板） |
| 3 | 验证口径 | ✅ **全量 chunk 对比**（用户拍板） |
| 4 | codegen 如何识别「ctor 返回 handle」 | ✅ 新增 `InvocationTarget.CtorReturnsNativeHandle`，与 `DirectNativeSymbol` 同构 |
| 5 | 是否仍需分配 `chaos_object` | ✅ 需要 —— 保留托管对象身份，只是不再把地址当 handle |
| 6 | 能否回退 `CreateVoidAbiSlot()` | ✅ **不能** —— 会让 subject body 被替换为 stub（更糟） |
| 7 | 方案 A 的裸指针键是否可行 | ✅ **不可行** —— GC 有 `StwCompact()`，键会失效 |

### watch_items（执行期观察项，非待拍板问题）

- W1：~~未标记 ctor 是否真的零改动~~ → **已提升为验证门槛 §5.4-3**（产物的逐字节 diff）
- W2：XML chunk 剩余 ~96 项 `realDefect`/`failed` 是否与本机制同源（**不得外推**）
- W3：`stream_stubs` 的裸指针键隐患是否需独立立项（本轮不做，但应记录）

### 7.1 外部输入核对记录

`HANDOFF-c1-evidence.md`（`closure-lambda-emitting` 线）经核对**不改变本设计方向**，
但促成两处修正：

| 该交接的论点 | 对本文档的影响 |
|:-------------|:---------------|
| §3：某 guard 注释的「无法降低」理由在代码支持后已失效 | → 促成 §5.3.1 / §6.1：影响面必须用**当代码结构 + 产物 diff** 判定，不能靠注释 |
| §3：`EmitLinearNewObject` 无参 reference 分支已存在（`Linear.cs:509-521`） | → 促成 §5.3.1 四分支清单；确认 C3 只落有参分支 |

**未采纳**：该交接的主体（删 `<>c::` guard、解封 224 lambda）属
`closure-lambda-emitting` 任务线，与本轮 ctor 句柄无关，不纳入本文档范围。

**约束复核**：该交接未推翻 §3 约束表中的 `CreateVoidAbiSlot()` 约束 ——
本文档引自 `Linear.cs:481/527` 的**实际代码**（非注释），复核后仍然成立。

---

## 9. 下一步入口

`writing-plans` —— 按 C1→C2→C3→C4 顺序拆解实施步骤，每步带验证。
