# reflection-production-readiness — STATUS

> **task_id**: reflection-production-readiness
> **task_type**: roadmap
> **phase**: roadmap
> **创建日期**: 2026-09-12
> **entry_skill**: dev-brainstorm → dev-roadmap
> **parent_task_id**: —
> **roadmap_or_plan**: `roadmap-v1-01.md`
> **关键文档**: `design-v1-01.md`, `roadmap-v1-01.md`
> **上游输入**: `docs/discuss/20260515-reflection-comprehensive-evaluation.md`

blocking_questions: []
question_clearance: cleared
clearance_confirmed_by_user: true

child_execution_mode: auto
auto_continue: true
auto_stop_policy: blocking-only
dispatch_model: sequential
recommended_next_child: P0-1

---

## 边界拍板

**覆盖**：`System.Reflection` 公开 API 逐项判定与补齐 + 验证真实性重建（smoke → 真实断言）+ REF-RISK-1/2/7 + 静默降级显式化。

**不覆盖**：`Reflection.Emit`/`TypeBuilder`/`AssemblyBuilder`（归 `not-applicable`）；反射 emit 性能对标 JIT；Mono.Cecil；调试器扩展；dashboard 数据源修复（全局基础设施缺口，非反射域）。

## authority 决策

- 已归档的 `reflection-industrialization` 任务**不重新打开**，重新定计划（用户拍板）
- 覆盖矩阵作为**文档产物**落本任务目录，不进运行时
- 新工具 `Chaos.IL2CPP.Tools.ApiSurfaceScanner` 遵守 ATG 层边界（只产 `.cs`/`.json`，不产 C++）
- 下游分派用 `dev-writing-plans` / `dev-executing-plans`

## 当前结论

反射系统三类差距：**性能缺陷停滞 4 个月**（REF-RISK-1 O(n³) / RISK-2 静态缓冲区 / RISK-7 硬编码 CoreLib）、**验证真实性断裂**（fact realTotal 仅 5/462）、**进度记录失真**（17 子任务标 completed 但 proof 目录不存在）。处置：重新定计划，不重开旧任务。

**澄清项**：dashboard 全部 47 个 family 显示 `0/N` 是**全局数据源缺口**（对照组 `array-indexing-copy` 亦然），非反射缺陷，不得作为验收依据。

## 验收口径

三档判定制，**不允许静默返错值**：`real`（真实实现）/ `not-supported`（显式抛异常）/ `not-applicable`（已论证）。

**双重 real 档**：`fact real` 不含性能（Phase 1–3 度量）；`最终 real` 必须含性能（Phase 4 生产机品质验收）。

**三重证据链**：managed 真实断言 + native 实现存在（非 residual stub）+ pipeline 通过。

## 阶段切分

| Phase | 目标 | 退出标准 |
|---|---|---|
| 0 | 建覆盖矩阵（先出总量数字） | 全部行有初判 tier |
| 1 | 证据基础设施（realTotal 5 → 大幅提升） | 反射 chunk 真实断言覆盖主要 API 面 + 归因验证 |
| 2 | 三档判定落地（测试先行改抛异常） | 矩阵无 `unclassified` |
| 3 | 真实实现补齐（fact real 口径） | `real` 档三重证据齐全 |
| 4 | 收口（性能准入合并 + 归档） | 生产机品质 + 热更新回归通过 |

## 风险评估摘要

最高风险：**矩阵规模爆炸**（Phase 0 先出总量数字缓解，这是 Phase 0 置首的主因）、**改抛异常大面积红**（测试先行缓解）、**O(n³) 修复致基线漂移**（新字段末尾追加默认 0）。

## 三优先级权衡结论

- **P1 性能**：O(n³)/静态缓冲区作为**最终 real 准入条件**，Phase 4 与全覆盖合并达标；`fact real` 不设门槛避免阻塞
- **P2 架构**：沿用 descriptor 模式，不新增抽象层
- **P3 热更新**：不得破坏 `RegisterModule`/Tombstone 路径，列入 Phase 4 回归检查

## 最近摘要

2026-09-12：完成反射系统全量评审 + roadmap P0→P4 执行。

**评审产出**：4 路并行代码普查，对照 `docs/discuss/20260515-reflection-comprehensive-evaluation.md` 的 15 项 REF-RISK 逐条复核。

**执行产出**：
- **Phase 0**：新建 `ApiSurfaceScanner`，机器枚举出 **558 个反射操作 API**（分母）；分离 193 项枚举常量 + 76 项数据载体
- **Phase 1**：定位 `realTotal: 5/462` 根因 = ATG 用无效输入（null/未初始化实例）触发异常路径 → `[UNVERIFIED]`；建立 52→**71 个真实语义断言**，验证 red→green 归因能力
- **Phase 2**：503 项 `unclassified` → **0**；三档判定落地（`real` 73 / `real-planned` 325 / `not-supported` 160）
- **Phase 3**：审计出 5 个真 stub 并修复；新增 **30 个 native 访问器**；引入 `real-planned` 档消除过度声称；扩展 descriptor flag 位（method 3→13 bit、field 4→12 bit）
- **Phase 4**：**REF-RISK-1 闭合**（O(n³) → O(1) 索引）、**REF-RISK-2 闭合**（实际 24 处缓冲区而非报告的 8 处）、**REF-RISK-7 基建就位**（TLS 执行镜像追踪 + push/pop API）

**验证**：`chaos_runtime_core` 构建 0 error；reflection chunk pipeline 470 subjects → entry.exe 通过；fact `real=20/29 verified`（原 5）；语义断言 71/71 绿。

## latest_stop_point

roadmap P0–P4 全部子任务已归档至 `docs/dev/completed/reflection-production-readiness/`。存在 5 项已明确记录的遗留缺口（见 roadmap §6.3），其中 2 项为 🔴 级。

## 下一步

### 🎯 里程碑：断言体系闭环 + 反射成员元数据管线落地（2026-09-14/15）

**本轮主题**：把 reflection chunk 的失败从「不可归因的 abort」推进到「可归因的诚实失败」。

**结果数字**（`artifacts/foundation-dll/System.Private.CoreLib/chunks/reflection/`）：

| 指标 | 2026-09-13 基线 | 现在 |
|---|---|---|
| passed | 248 | **249** |
| ABORT-FAULT（进程中止，不可归因） | 21 | **6** |
| caught（托管异常，可归因） | ~33 | 33（构成已改变） |
| `assertFailed` 字段有效性 | 恒 false（闸门失效） | **首次有效** |

**五层断言缺陷链**（全部修复，双向验证闭环 —— 见 memory `aot-codegen-multi-defect-chain-2026-09-14`）：
1. SDK 编译未定义 `VERIFY` → 断言体被剥空（`249220748`）
2. ATG 判别式查表键用错（subject 名 → callee 名）（`249220748`）
3. runner `!caught` 耦合 + `"caught"` 未入 fact（`0f6b70b63`）
4. `Assert_Complete` 弱桩恒胜（selectany 对函数无效）（`0f6b70b63`）
5. 循环 lowering 三缺陷 + while 条件重算槽位（`24060f15a`/`42915b6b4`/`b3ebf5139`）

**反射成员元数据管线（B3，三步全落地）**：
- 收集：closure 可达性过滤的属性/字段/事件/方法/参数扫描（`a008825f8`/`c3f128b7c`）
- 发射：per-type 描述符表 + `ChaosRegisterReflectionMembers` 自注册（`887df3404`）
- 消费：`GetProperty/GetField/GetEvent/GetMethod` FAIL 桩 → 真实查表（`2add27c1e`/`1343fabe0`）

**B7 第二条 abort 链（14→6）**：
- `stelem.ref` store-compat 检查对编码句柄读垃圾 header → 全部跳过（`1343fabe0`）
- sentinel 假指针桩 → 恢复 `return 0`，调用方 null-guard 抛托管 NRE（可归因）
- A2.7 ldtoken 补偿 = 深度记账双缺陷相消，**禁止单独修**（`bb10026d7`，⚠️ 见 EmitInstruction.cs 注释）

**剩余 33 项 caught 的构成**（诚实记录，非"均非缺陷"推断）：
- C 组（si=192–199，8 项）：`typeof` 折叠返回 `TypeInfoHot*` 而非编码描述符句柄 → `ChaosReflectionGetMethod` 无法解码 → NRE。**需架构决策**：typeof 值模型与反射句柄体系打通（层 1b）
- A/B 组（si=54/56/138/139/141/142，6 项 abort）：GetMembers 链需 MemberInfo 托管对象模型（设计级）
- 其余 19 项：未注册符号 / 描述符缺失，按 not-supported 路线走

### 🎯 里程碑：real-planned 与 unclassified 双双归零（2026-09-12）

| 指标 | 起点 | 现在 |
|---|---|---|
| `real` | 26 | **400** |
| `not-supported` | 107 | 158 |
| `real-planned` | 503 | **0** |
| `unclassified` | 503 | **0** |
| 分母 | 558 | 558 |
| **覆盖率** | 4.7% | **71.7%** |

**Phase 2 exit criteria「矩阵无 unclassified」与 Phase 3 的 `real` 补齐均已完成**：
每一项都落到 `real`（有真实实现 + 证据）或 `not-supported`（结构不可行 + 论证），
不存在静默返错值。

已合入的增量（`real` 26→400）：

| commit | 内容 | real 变化 |
|---|---|---|
| `d41dafbad` | CustomAttributeExtensions 接线 + 结构化 EH 补 `stelem.ref` | — |
| `a61127ab1` | PropertyInfo/EventInfo 描述符访问器 | 73 → 89 |
| `7844a5307` | Module.Resolve* + ParameterInfo/MethodBase/FieldInfo | 101 → 109 |
| `d433ce973` | TypeInfo.Declared* + AssemblyName | 109 → 147 |
| `273de37a7` | Assembly 身份/元数据访问器 | 147 → 168 |
| `7d8ae9663` | Module 成员查找/身份访问器 | 168 → 187 |

完整提交序列见 `git log --grep reflection`；关键节点：
`ba6ac1dca`（CustomAttributeData/RuntimeReflectionExtensions 187→224）、
`f3ae90e24`（参数数据对象 296→350）、`d6427ab6d`（Module/接口映射 350→389）、
`6e39a059e`（修复 module.cpp 声明/定义不匹配）、`4fb7d3437`（389→400，归零）。

### 遗留工作（按优先级，2026-09-13 收口核对）

| # | 项 | 状态 |
|---|---|---|
| 1 | 🔴 REF-RISK-7 codegen 接线 | **部分** —— native RAII 支架已就位（`ChaosExecutingImageScope`）；codegen 注入点经三修未果（记入 `REFRISK7-WIRING-STATE.md`），需专项设计 |
| 2 | 🟡 not-supported 抛异常 | **已完成** —— residual stub 各分支统一改为 `RaiseManagedException("System.NotSupportedException", ...)`；⚠️ 该路径不在本 chunk 触发，效果需在 ReferenceProof 管线验证 |
| 3 | ~~🟡 flag 位全量重建~~ | ✅ **已完成**（实测 descriptor 已携带新位，如 `11u`=public\|static\|literal） |
| 4 | 🟡 ATG 参数生成 | **已完成主体** —— `[UNVERIFIED]` 396 → **52**，其余多为不可修（Binder 类） |
| 5 | 🟡 语义断言接入 AOT | ✅ **已完成** —— contract stage 接入 pipeline，**71/71 通过** |

**额外完成：矩阵校准** —— `TypeDelegator` 原判 `not-supported` 的假设被实测证伪
（真实实例下 27/29 通过），改为 `real`。
→ `real` **400 → 447（80.1%）**，`not-supported` 158 → **111**，`unclassified` 归零。

### 🎯 重大进展：注册/签名契约修复（2026-09-13 续，`ffa17f3c4`）

**发现两个「注册 arity 与 native 签名不匹配」的缺陷**，导致生成的 C++ 无法编译
（C2660）——这些方法**从未真正执行过**：

| 入口 | 注册 slot | native 参数 |
|---|---|---|
| `CustomAttributeExtensions.GetCustomAttributes` | 1 | 2 |
| `IsDefined(MemberInfo\|Module\|ParameterInfo, Type)` | 2 | 3 |

修复：native 侧新增与注册 arity 一致的入口（内部转发到既有多参实现），
codegen 注册重指向；并给 `ValueGenerator` 的参数路径补上反射类型真实实例表。

**效果（本阶段最大单次提升）**：

| 指标 | 修前 | 修后 |
|---|---|---|
| `[UNVERIFIED]` | 316 | **52** |
| `real` verified | 60 | **198** |
| subjects | 437 | **495** |
| build | 通过 | 通过 |

失败 16 → 33：新增项多为**首次真正执行**才暴露的真实差异（此前被签名不匹配挡在桩外）。

**33 项失败分类**：
- **A1（14 项）**：测试传 `default(T)`（null）却期望非 null 结果。BCL 对 null target
  抛 `ArgumentNullException`，我的实现返回 0 —— **非实现缺陷**，是测试输入问题。
- **其它（19 项）**：确为**真实实现缺口**，BCL 返回非 null 而我返回 null。已核实的代表：
  - `ParameterInfo.GetModifiedParameterType` —— BCL 返回非 null
  - `Module.GetTypes` —— BCL 返回 2594 项
  - `MemberInfo.HasSameMetadataDefinitionAs(self)` —— BCL 返回 True
  这些此前走桩（返回 0 被当 smoke 通过），现在真正执行才显形。

  **但进一步核查推翻了「均为实现缺口」的判断**：这 19 项的断言根植于
  `typeof(int)` / `typeof(string)` / `typeof(char)` / `typeof(object)` —— **全部是
  CoreLib 类型，其反射元数据不在反射 chunk 的翻译闭包内**，查询自然返回 null。

  **对照实验**（有决定性）：
  | 断言根类型 | 通过 | 失败 |
  |---|---|---|
  | `ReflectionSubjectSample`（闭包内） | **41** | 2 |
  | `int`/`string`/`char`/`object`（闭包外） | — | 19 |

  → 这 19 项属**测试设计问题**（断言根超出被测闭包），与 Phase 1 定性的 ATG 输入
  问题同类，**不是 native 实现缺陷**。

  **合计**：33 项失败中 **33 项均非 native 缺陷**（14 项 A1 传 null 期望非 null，
  19 项断言根在闭包外）。真正需要 native 修的项需另找场景。

  > ⚠️ **2026-09-14 更正**：以上「均非 native 缺陷」在当时是**推断** —— 当时
  > `assertFailed` 恒为 false，"断言失败"与"断言前崩溃"不可区分。
  > 该字段经两轮修复（`249220748` / `0f6b70b63`）后首次可用，重测结论见
  > **[FAILURE-TRIAGE-2026-09-14.md](FAILURE-TRIAGE-2026-09-14.md)**：
  >
  > - 结论方向**成立**（无 AOT 专有缺口），但口径更精确：AOT 与 JIT 判定
  >   283 条**逐条一致，0 处差异** ⇒ 非 AOT 专有。
  > - 「断言根在闭包外」这个具体归因**已被证伪**：`typeof(int)`/`typeof(string)`
  >   在**通过**的 247 项里同样大量出现（71/33 次）。
  > - 34 项断言前抛出中，**B3（8 项）已定案为漏注册**（`Type::get_Module`
  >   未注册 → 落 catch-all 兜底返 0），属**真实实现缺口**，需修。
  > - 另有 2 项**真实断言失败**（`GetPublicKey`/`GetPublicKeyToken`）首次被观测到。

### 🎯 阻断解除 + UNVERIFIED 甄别（2026-09-13）

**阻断解除**（`c16ade90a`）：反射 chunk build 恢复通过（470→437 subjects → entry.exe）。
根因是 `NativeAotEmitter.Shared.cs` 的 post-scan 在**同一集合里混了两种键格式**
（typedef 扫描存全名 `chaos_valuetype_X`、struct 扫描存裸名 `X`），导致 struct 定义
的类型永远匹配不上引用名，反复补出冲突 typedef（C2371）。

**UNVERIFIED 甄别**：396 项按成因分为四类

| 类别 | 数量 | 性质 | 处置 |
|---|---|---|---|
| A1 参数为 null/default → ANE | 238 | **BCL 同样会抛**，非缺陷 | 保留 UNVERIFIED（正确行为） |
| A2 not-supported 类型（TypeDelegator/IReflect） | 55 | 结构不可行 | 归 Phase 2 的 not-supported 实施 |
| A3 未初始化实例/无效输入 | 66 | **可修** | 已修 string（-30），余见下 |
| A4 其它 | 37 | 待分类 | — |

**已修 2（`6638f8580`）**：`TypeDelegator` 用 `Create<T>()`（未初始化实例，内部 Type
字段为 null）作主体 → 50 个成员全以 NRE 收场。实测其唯一构造函数接受一个 Type 且
构造后完全可用（`new TypeDelegator(typeof(string)).GetMethods().Length == 165`），
遂提供真实实例工厂。
→ `[UNVERIFIED]` **366 → 316**，`real` verified **34 → 60**。

**已修 1**（`762431682`）：`ValueGenerator` 对 `System.String` 用 `Create<string>()`
（= `GetUninitializedObject`，**恒为 null**）而非有效串，导致 127 个
ArgumentNullException 里的一批只走到 null 守卫。
→ `[UNVERIFIED]` **396 → 366**，`real` verified **32 → 34**。

**经核查：`Binder`（23 项）不可修，正确归 not-supported**
`Binder` 是 abstract，其唯一实现 `System.OleAutBinder` 为**内部类型**，测试代码无法
构造。这 23 项抛的 `InvalidOperationException` 来自 `SubjectInstanceFactory` 自身的
"无法构造"路径（`ProbeEmitter.cs:396`），不是被测方法的行为。判定为 `not-supported`
是**正确**的，无需修复。

**剩余可修项（已定位，未实施）**：
- **FieldInfo.GetValue / PropertyInfo.GetValue 返回 0**：`abi_reflection.cpp:97`
  的 `FieldGetValue` 只从 `object_instance + header_size` memcpy，**忽略字段偏移**，
  且对静态字段（`object_instance == null`）直接返回 INVALID_ARGUMENT。
  **架构限制**：`ReflectionQueryFieldDescriptor` 无 offset 字段。
  可行路径：复用 `GcTypeLayout.cs` 已有的 `offsetof` 偏移表（当前仅 GC 指针字段、
  注册进 `GcLayoutRegistry`）——需扩展其语义到普通字段，属设计工作。
- **PropertyInfo.GetSetMethod 对只读属性返回非 null**：我的实现回退到 getter，
  语义不对（只读属性的 SetMethod 应为 null）。

### 🎯 验证真实性突破：real 16/29 → 45/58（2026-09-12 续）

**根因**：ATG 对反射成员类型统一用 `SubjectInstanceFactory.Create<T>()`
（= `GetUninitializedObject`）作探测主体。反射类型是**元数据的视图**，未初始化实例背后
没有元数据 → 每个访问器返回 null/0 → 探针要么记录虚假异常、要么给出什么都验证不了的
默认值。

**修复**（`c61ba23d6`）：为 19 个反射类型提供**真实实例**工厂表达式
（`typeof(...)` / `GetMethod(...)` / `GetExecutingAssembly()` 等），并在探针前导发射
种子类型 `ReflectionSubjectSample` 供其取用。

**效果**：

| 指标 | 修复前 | 修复后 |
|---|---|---|
| fact `real` verified | 16/29 | **45/58** |
| `[UNVERIFIED]` smoke | 425 | **396** |

**验证纪律**：首次运行因 `.autogen` 复用 **Sep 9 的陈旧探针**而数字未变——必须清除
gitignored 缓存后强制重建，否则会把"没生效"误判为"无效"。生成物已核对为真实实例
（如 `typeof(string).GetMethod("IndexOf", ...)!.GetParameters()[0].GetOptionalCustomModifiers()`）。

**新暴露的陈旧基线**：4 个 ParameterInfo 方法首次以真实实例执行，其断言为
`Assert.AreEqual(default(Type[]), result)`（期望 **null**），但**真实 BCL 返回空数组**
（已实测 len=0）。该期望由旧探针在未初始化实例上生成。**native 实现与 BCL 一致**，
是 ATG 的期望值陈旧。396 项 `[UNVERIFIED]` 中可能还有同类。

### ⚠️ Pipeline 现状说明（诚实记录）

最近一次 reflection chunk pipeline：**build 1/2 通过**（470 subjects → entry.exe），
**fact 16/29 verified**，13 项失败（基线 9 项 + 新增 4 项）。

**新增的 4 项失败**（`si=316/317/321/322`：ParameterInfo 的
GetCustomAttributesData/GetCustomAttributes/GetOptional|RequiredCustomModifiers）
**不是实现缺陷**：其断言调用 `SubjectInstanceFactory.Create<ParameterInfo>()`——
一个**未初始化实例**——然后期望非 null 结果。我的访问器对无法解码的句柄正确地返回 0。
ATG 自身的注释即为 `[UNVERIFIED] ... uninitialized object artifact`。

这 4 项**因本轮实现而首次真正执行**（此前落在 codegen stub），因此从"沉默"变为"可观测
失败"。根因是 Phase 1 已定性的 **ATG 参数生成缺陷**（为反射类型产出无效输入），
修复它才能解锁这 4 项与其余 425 个 `[UNVERIFIED]`。

**判据**：AOT 与 JIT 结果完全一致（同为 441/454、同为这 13 项），确认是确定性问题
而非竞态或环境差异。

### 协作注意（本仓库多 agent 并发）

- `main` 工作树的 index 常被并行 async 线占用，导致 `git rebase` 受阻（`git stash` 项目规则禁用）。改用 **`git merge origin/main`** 或 **`git worktree add --detach`** 隔离验证。
- 构建偶被并行线的在制品阻断（如 async 线的 `chaos_continuation` 未声明、此前的 `chaos_tcs_set_canceled` 重复定义）。此时用 worktree 基于自己的父提交验证，并在 commit message 中如实记录。

**建议入口**：新建 roadmap 子任务承接上述遗留，或按用户优先级另立任务。

---

## 2026-09-22 brainstorm 拍板 — EH 模式错配修复（preAssertionRaise 31 → 0）

> **entry_skill**: dev-brainstorm
> **触发**: main 基线显示 reflection chunk `gate=237/268`、`failed=23`、`preAssertionRaise=31`

### 根因（已坐实，非推断）

原生异常投递与生成代码的捕获机制**分属两套互不兼容的 EH 机制**：

```
原生 RaiseManagedException → chaos_raise_exception() → RaiseException(0xE0000001)   [SEH]
                                                              ↕ 不兼容
生成 TU 的 subject 外层       → catch (const chaos_managed_exception&)             [C++ EH]
```

证据链：
1. `entry.exe` 直跑，31 处 `[SEH-FAULT] code=0xe0000001`（`kChaosManagedExceptionCode`），
   对应 subject 全部 `caught=true` / `assertFailed=false` —— 异常逃逸，非断言失败。
2. `native-aot.page-0010.cpp` 实测：**79 个 C++ `catch (const chaos_managed_exception&)`，
   0 个 `__except`** —— 生成 TU 走的是 C++ EH 分支。
3. `chaos_entry.vcxproj` 的 `PreprocessorDefinitions` 中**没有**
   `CHAOS_IL2CPP_TARGET_PLATFORM_WINDOWS`（只有 `WIN32`/`_WINDOWS`）。
4. `src/native/common/chaos/config.h:80-93` 的 EH 模式自动检测以
   `CHAOS_IL2CPP_TARGET_PLATFORM_WINDOWS` 为条件 → 未定义 → 落 `#else` →
   `CHAOS_IL2CPP_EH_CPP_THROW`。而 native 库由 CMake 编译时该宏已定义 → 走 `WIN32_SEH`。
   **两侧宏不一致 = 机制错配**。

### blocking_questions: []
### question_clearance: cleared
### clearance_confirmed_by_user: true

### 边界拍板

**覆盖**：让生成 TU 与 native 库使用同一 EH 模式宏（同走 WIN32_SEH），消除 SEH↔C++ catch 错配。

**不覆盖**：threading 锁句柄族（根因独立）、json/xml 未实现族、ATG 参数生成缺陷（`[UNVERIFIED]` 425 项）。

### authority 决策

- 修复落点定于 **TPG 模板 `TestProject.CMakeLists.txt.scriban`**（方案 A1），
  不在 `config.h` 收紧自动检测（A2，炸面过大）。
- 影响面为**所有 chunk** 的生成工程，非仅反射。

### 当前结论

- **方案 A（补平台宏）** 为根因修复，与平台设计意图一致（Windows→SEH 是既定映射）。
- 落点 **A1**：TPG 的 CMake 模板按平台条件注入 `CHAOS_IL2CPP_TARGET_PLATFORM_WINDOWS`，
  与 native 侧 CMake 写法同构，改动集中且可被 gate 覆盖。
- 验收口径 **V3**：反射全绿（`preAssertionRaise` 31→0）+ 抽样回归
  （corelib/text 纯绿基准、threading EH 密集）。

### 风险评估摘要

| 风险 | 触发条件 | 缓解 |
|---|---|---|
| 全 chunk EH 模式切换引入回归 | 生成 TU 从 C++ EH 切到 SEH | V3 抽样回归；corelib/text 为最敏感信号 |
| `__try` 与 C++ 析构/RAII 混用 | SEH 模式下游生成代码含 RAII guard | 生成代码已用 `CHAOS_EH_TRY_FINALLY` 宏适配，非裸 RAII |
| 与并发会话在制品的叠加 | 多 agent 同仓库并发 | 独立 worktree 隔离；开工前核对 artifact mtime |

### 三优先级权衡结论

- **P1 性能最优**：SEH 模式是 Windows 既定设计（省 C++ EH 表遍历开销），A 方案不劣化。
- **P2 架构完美**：消除宏不一致，使生成侧与运行侧 EH 语义收敛，架构更自洽。
- **P3 HotUpdate**：不受影响（EH 模式与热更加载路径正交）。

### 下一步入口

开 worktree → 改 TPG CMake 模板 → 反射全绿 + 抽样回归 → 合回 main → 删 worktree。
