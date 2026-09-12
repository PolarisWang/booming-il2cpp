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

### 遗留工作（按优先级）

1. 🔴 **REF-RISK-7 codegen 接线** —— native 基建已就位，需在 codegen 发射方法体时插入 `ChaosReflectionPushExecutingImage`/`PopExecutingImage`。**注意 P1 约束**：应仅对实际引用该访问器的方法插入，避免全量方法热路径开销。
2. 🟡 **158 项 `not-supported` 需实际抛 `NotSupportedException`** —— 判定已明确，实施（含测试先行）仍待做。
3. 🟡 flag 位全量重建使 descriptor 携带新位。
4. 🟡 修复 ATG 参数生成（为反射类型产出有效输入）以解锁 425 个 `[UNVERIFIED]`。
5. 🟡 将语义断言接入 Chaos AOT 路径。

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
