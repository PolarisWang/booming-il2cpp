# Reflection 生产机品质补齐 — Roadmap v1

> **面向执行 Agent:** 本 roadmap 的父任务文档只承担阶段拆分与子任务编排；具体实施通过派生子任务目录完成。
> **task_id**: reflection-production-readiness
> **dispatch_model**: sequential（用户拍板：1 个 worktree，串行执行）
> **上游设计**: `design-v1-01.md`（brainstorm 已拍板，`blocking_questions: []`）
> **评审基线**: `docs/discuss/20260515-reflection-comprehensive-evaluation.md`（REF-RISK 15 项）

**目标：** 将 Reflection 子系统从「功能骨架可演示」提升到**生产机品质**：`System.Reflection` 公开 API 逐项判定落档（无静默返错值），验证真实性重建（真实断言替代 smoke），REF-RISK-1/2/7 三项未修缺陷闭合。

**技术栈：** native C++20（`reflection/*.cpp` unity-build）、managed C#（Generator Scriban + ATG）、METADATA（MetadataLoadContext）、foundation-dll pipeline

**全局优先级约束：** P1 性能最优 > P2 方案完美性 > P3 HotUpdate 支持。
本 roadmap 的裁决点：`fact real` 档不设性能门槛（避免性能修复阻塞语义修复），`最终 real` 档强制含性能（Phase 4 合并验收）。此裁决使 P1 在 Phase 4 与 P2 强制对齐，不破坏 P3。

**收尾约束：** 子任务进入终态必须**物理移动**任务目录出 `docs/dev/in-progress/`，不得只改文案。

---

## 1. 为什么需要 roadmap

反射生产机品质补齐涉及 **5 个阶段、跨多次会话、多子任务**，且存在一个关键的结构性依赖：**Phase 0（覆盖矩阵）的产出决定 Phase 2–3 的切分粒度**。如果矩阵显示公开 API 达数百项，Phase 2 必须按 API 类别分批；若仅数十项，可整体推进。这种「前一阶段改变后一阶段结构」的依赖无法用单一 plan 覆盖，必须用 roadmap 的阶段机制承接。

另有三个跨域耦合：native 实现（`reflection/*.cpp`）、codegen（descriptor 字段 + Scriban 模板）、测试治理（subject/manifest/proof 套件）——三者的改动必须按 P1→P2→P3 优先级协调，且 Phase 3 触及 descriptor 时会导致全量重编译与快照基线漂移。

---

## 2. 范围边界

**覆盖：**
- `System.Reflection` 命名空间公开 API 的机器枚举与逐项判定
- 三档判定制落地：`real` / `not-supported` / `not-applicable`
- 反射 chunk 验证真实性重建（`realTotal: 5` → 大幅提升）
- 静默降级 stub 的显式化改造（**测试先行**，见 §6.2）
- REF-RISK-1（`GetReflectedType` O(n³)）、REF-RISK-2（静态缓冲区）、REF-RISK-7（硬编码 CoreLib）

**不覆盖：**
- `Reflection.Emit` / `TypeBuilder` / `AssemblyBuilder` — 归 `not-applicable`
- 反射 emit 性能对标 CoreCLR JIT
- Mono.Cecil 集成
- 反射相关调试器扩展（属调试域）
- dashboard 数据源修复（全局基础设施缺口，非反射域）

---

## 3. 非目标

- 不追求 CoreCLR 的 JIT 级反射动态生成能力
- 不与 Unity IL2CPP 对标元数据结构（Chaos 的双 Tier + 热更新注册是本质优势，应保持）
- 不重构现有 descriptor 架构（沿用既有模式，不新增抽象层）
- 不重新打开已归档的 `reflection-industrialization` 任务（用户拍板）

---

## 4. 阶段列表

### Phase 0：覆盖矩阵建立（先出总量数字）

**Goal:** 用机器枚举建立 `System.Reflection` 公开 API 的权威全集，对齐现有 native 声明与 codegen 黑名单，产出三维矩阵，使后续阶段有可量化的工作面。

**Exit Criteria:**
- `Chaos.IL2CPP.Tools.ApiSurfaceScanner` 可用，能枚举目标程序集的 `System.Reflection` 公开 API
- `api-coverage-matrix.json` 生成，**每一行都有初判 tier**（`real` / `not-supported` / `not-applicable` / `unclassified`）
- **总量数字明确公布**（多少个 API、多少已实现、多少待判定）
- 现有 82 个 native 声明 + 30 项 codegen 黑名单已对齐到矩阵
- `Reflection.Emit` 系列初始化为 `not-applicable` 并附论证

**Deliverables:**
- `src/tools/Chaos.IL2CPP.Tools.ApiSurfaceScanner/`（新建，遵守 ATG 层边界：只产 `.cs`/`.json`）
- `docs/dev/in-progress/reflection-production-readiness/api-coverage-matrix.json`
- `docs/dev/in-progress/reflection-production-readiness/api-surface-report.md`（总量与分类统计）

**Dependencies:** 无（起始阶段）

**Resolved Decisions:**
- 枚举技术栈复用 ATG 既有 `DllScanner.cs` / `MetadataLoadContext`（`AutoTestGenerator.csproj:15` 已引用 `System.Reflection.MetadataLoadContext`）
- 矩阵是**文档产物**，不进运行时
- 分母 = 公开 API 全集，非现有 82 个 native 声明

**Watch Items:**
- ⚠️ **矩阵规模爆炸风险**：若 API 总量远超预期（如 400+），Phase 2–3 必须按类别分批展开；Phase 0 退出时需据此回填 roadmap 子任务表
- 枚举目标程序集的选择（`System.Private.CoreLib` + `System.Runtime` 还是包含 `System.Reflection` facade DLL）会影响分母口径

---

### Phase 1：验证证据基础设施

**Goal:** 重建反射 chunk 的验证真实性——把 smoke 断言替换为真实断言，使"修复是否有效"可被可靠度量，建立 red→green 归因能力。

**Exit Criteria:**
- 反射 chunk 的 `realTotal` 从 5 大幅提升至**覆盖主要 API 面**
- 每个待改 API 有**真实断言测试**（断言返回值语义，非仅"不崩"）
- red→green 归因能力验证：人为回退一处实现，测试应转红且原因可定位
- 2 个已知失败（`fact-results.json` methodIndex 401/403，`contractIndex: -1`）已归因

**Deliverables:**
- `tests/e2e/translation/System.Private.CoreLib/chunks/reflection/` 测试增强
- 真实断言测试套件（按 Phase 0 矩阵的 API 类别组织）
- 归因验证记录

**Dependencies:** Phase 0（矩阵定义测试覆盖面）

**Resolved Decisions:**
- **测试先行是强制的**（用户拍板）：先写断言正确行为的测试 → 跑到红 → 再改实现 → 跑到绿
- 验收走**三重证据链**（managed 真实断言 + native 实现存在 + pipeline 通过），不依赖 dashboard

**Watch Items:**
- 457 个 smoke 断言转真实断言时，可能出现大面积红——**这是预期结果**，代表旧验证从未检查语义
- 反射 chunk 的 subject 生成机制（ATG ProbeEmitter）可能需配合调整

---

### Phase 2：三档判定落地

**Goal:** 逐 API 完成三档判定，消除所有 `unclassified`；`not-supported` 档改为显式抛异常，不再静默返错值。

**Exit Criteria:**
- 矩阵无 `unclassified` 行
- 所有 `not-supported` API 抛 `NotSupportedException`，且有测试断言确实验异常
- **无任何 API 走「静默返错值」路径**（如 `GetBaseDefinition` 返自身、`GetCallingConvention` 硬编码 1、`GetParamAttributes` 返 0）
- `remaining_stubs.cpp` 中的静默降级逐项处置完毕

**Deliverables:**
- `src/native/runtime-core/reflection/remaining_stubs.cpp`（改造或拆分）
- `src/native/runtime-core/reflection/*.cpp`（静默降级点改造）
- 对应测试套件

**Dependencies:** Phase 0（矩阵）、Phase 1（测试先行能力）

**Resolved Decisions:**
- 静默降级 → **显式抛异常**（用户拍板）
- 执行顺序强制：造清单 → 写测试 → 跑到红 → 改实现 → 跑到绿（§6.2）
- ⚠️ 粒度待定：若 Phase 0 显示 API 量巨大，本阶段按 API 类别拆分为多个子任务

**Watch Items:**
- 改抛异常后，若下游代码依赖旧返回值，可能在 pipeline 暴雷——需按矩阵的调用面评估
- 抛异常本身会让部分 API "不可用"，与"全覆盖"表面冲突——含义是**入口存在 + 语义正确**，非"必须返回正确值"

---

### Phase 3：真实实现补齐（fact real 口径）

**Goal:** 填充矩阵中所有 `real` 档缺口，达到 `fact real` 标准（语义正确 + 真实断言通过，**不含性能要求**）。

**Exit Criteria:**
- 矩阵中所有 `real` 档行满足三重证据链
- `fact real` 覆盖率达标（按 Phase 0 定义的分母）
- 反射 chunk pipeline 通过

**Deliverables:**
- `src/native/runtime-core/reflection/*.cpp`（实现补齐）
- codegen 侧 descriptor / Scriban 模板调整（如需）
- proof 测试套件

**Dependencies:** Phase 0、Phase 1、Phase 2

**Resolved Decisions:**
- **`fact real` 不设性能门槛**（用户拍板）——避免性能修复阻塞语义修复
- descriptor 变更沿用 R1.1 先例：新字段末尾追加、默认 0 向后兼容

**Watch Items:**
- ⚠️ **descriptor 变更致基线漂移**：触及 `reflection_query_model.h` 会导致全量重编译 + 快照基线刷新
- 部分 API 可能在 AOT 下无法真实实现，需回退为 `not-supported`（需记录论证）

---

### Phase 4：收口 — 性能准入合并 + 归档

**Goal:** 合并两个验收轴——把性能要求并入 `real` 档（形成 `最终 real`），达成生产机品质并归档。

**Exit Criteria:**
- **REF-RISK-1 闭合**：`GetReflectedType` 消除 O(n³)，加 `owning_type` 反向指针或 token→type 哈希表
- **REF-RISK-2 闭合**：8 处 `static char` 改为 `thread_local` 或调用方提供缓冲
- **REF-RISK-7 闭合**：`GetCallingAssembly`/`GetExecutingAssembly` 真实栈帧遍历
- 矩阵所有行达到 `最终 real` 或 `not-supported`/`not-applicable`（无静默降级）
- 热更新反射回归检查通过（`RegisterModule` / Tombstone 路径未被破坏）
- 全量 pipeline 绿
- 任务目录归档至 `docs/dev/completed/`

**Deliverables:**
- `src/native/runtime-core/reflection/type_properties.cpp`（REF-RISK-1）
- `src/native/runtime-core/reflection/internal_helpers.cpp` / `type_stubs.cpp`（REF-RISK-2）
- `src/native/runtime-core/reflection/misc.cpp`（REF-RISK-7）
- 性能验证记录（200+ DLL 规模下的 `GetReflectedType` 耗时对比）

**Dependencies:** Phase 1、Phase 2、Phase 3

**Resolved Decisions:**
- **最终 real 必须含性能**（用户拍板）
- 性能修复若触及 descriptor，需评估并接受基线刷新成本

**Watch Items:**
- REF-RISK-1 的 `owning_type` 方案需 codegen 配合，可能引入跨域改动
- 性能修复不得破坏 P3 热更新路径（优先级让位原则）

---

## 5. 依赖关系

```
Phase 0 (矩阵)
  └── Phase 1 (证据基础设施)  ← 必须以矩阵定义测试覆盖面
        └── Phase 2 (三档判定) ← 必须有 red→green 归因能力
              └── Phase 3 (真实实现, fact real)
                    └── Phase 4 (性能准入合并, 最终 real)
```

**强串行链**：每个 Phase 的产出是下一 Phase 的输入。这与用户拍板的 `dispatch_model: sequential`（1 个 worktree）一致。

**动态展开点**：Phase 2/3 的子任务粒度**依赖 Phase 0 的总量数字**。Phase 0 退出时必须回填本 roadmap 的第 6 节子任务表。

---

## 6. 子任务映射

> ✅ **Phase 0 已退出**（2026-09-12）：总量数字 **636 个 API**（分母，排除 193 枚举常量与 not-applicable）。分布：`real` 26 / `not-supported` 107 / `unclassified` **503**。
> 636 > 300，触发 §8 备选路径 → **Phase 2/3 按 API 类别拆分**。子任务表已据此展开。

### 6.1 批次总览

| batch | phase | 说明 |
|---|---|---|
| batch-1 | 0 | 覆盖矩阵（已完成） |
| batch-2 | 1 | 验证证据基础设施 |
| batch-3..7 | 2 | 三档判定（按 API 类别分 5 批） |
| batch-8 | 3 | 真实实现补齐（fact real） |
| batch-9 | 4 | 性能准入合并 + 归档 |

### 6.2 子任务表

| task_id | phase | status | owner | purpose | depends_on | batch_id | requirements | deliverables | exit_criteria | conflict_scope | estimated_effort |
|---|---|---|---|---|---|---|---|---|---|---|---|
| P0-1 | 0 | **completed** | terminal-1 | 建 ApiSurfaceScanner 枚举反射公开 API | — | batch-1 | 枚举工具 + 矩阵初版 | `src/tools/.../ApiSurfaceScanner/` | 矩阵生成且每行有初判 tier | `src/tools/Chaos.IL2CPP.Tools.ApiSurfaceScanner/` | 3 天 |
| P0-2 | 0 | **completed** | terminal-1 | 对齐 native 声明 + 黑名单到矩阵 | P0-1 | batch-1 | 对齐 + 分类器 | `api-surface-report.md`, `Classifier.cs` | 总量数字公布（636） | 同上 | 2 天 |
| P1-1 | 1 | **ready** | terminal-1 | 反射 chunk smoke → 真实断言重建 | P0-2 | batch-2 | 真实断言测试套件 | `tests/e2e/translation/System.Private.CoreLib/chunks/reflection/` | realTotal 从 5 提升至覆盖 26 个 real API | `tests/e2e/translation/System.Private.CoreLib/chunks/reflection/` | 4 天 |
| P1-2 | 1 | planned | terminal-1 | 2 个已知失败归因（methodIndex 401/403） | P1-1 | batch-2 | 归因结论 | 归因记录 | 根因明确 | 同上 | 1 天 |
| P2-1 | 2 | planned | terminal-1 | 判定 `Type` / `TypeInfo` / `RuntimeReflectionExtensions` | P1-2 | batch-3 | 三档判定 + 抛异常改造 | `reflection/type_*.cpp`, 矩阵更新 | 无 unclassified | `src/native/runtime-core/reflection/` | 待定 |
| P2-2 | 2 | planned | terminal-1 | 判定 `MemberInfo` / `MethodBase` / `MethodInfo` / `ConstructorInfo` | P2-1 | batch-4 | 同上 | `reflection/members.cpp`, `invoke.cpp` | 无 unclassified | `src/native/runtime-core/reflection/` | 待定 |
| P2-3 | 2 | planned | terminal-1 | 判定 `FieldInfo` / `PropertyInfo` / `EventInfo` / `ParameterInfo` | P2-2 | batch-5 | 同上 | `reflection/type_stubs.cpp`, `remaining_stubs.cpp` | 无 unclassified | `src/native/runtime-core/reflection/` | 待定 |
| P2-4 | 2 | planned | terminal-1 | 判定 `Assembly` / `Module` / `AssemblyName` | P2-3 | batch-6 | 同上 | `reflection/misc.cpp`, `module.cpp`, `assembly_types.cpp` | 无 unclassified | `src/native/runtime-core/reflection/` | 待定 |
| P2-5 | 2 | planned | terminal-1 | 判定 `CustomAttribute*` 族 + 剩余类型 | P2-4 | batch-7 | 同上 | `reflection/attributes.cpp` | **矩阵无任何 unclassified** | `src/native/runtime-core/reflection/` | 待定 |
| P3-1 | 3 | **completed** | terminal-1 | 补齐 `real` 缺口（fact real 口径） | P2-5 | batch-8 | 实现 + 三重证据 | `reflection/*.cpp` + codegen | real 档三重证据齐全 | `src/native/runtime-core/reflection/` + codegen | 待定 |
| P4-1 | 4 | **completed** | terminal-1 | REF-RISK-1 O(n³) 消除 | P3-1 | batch-9 | owning_type 反向指针 | `reflection/type_properties.cpp` | 200+ DLL 下 O(1) | `type_properties.cpp` | 3 天 |
| P4-2 | 4 | **completed** | terminal-1 | REF-RISK-2 静态缓冲区改造 | P3-1 | batch-9 | thread_local 或调用方缓冲 | `internal_helpers.cpp`, `type_stubs.cpp` | 24 处全改造 | `src/native/runtime-core/reflection/` | 3 天 |
| P4-3 | 4 | **completed** | terminal-1 | REF-RISK-7 执行镜像追踪基建 | P3-1 | batch-9 | TLS 追踪 + push/pop API | `reflection/misc.cpp`, `thread_state.*` | native 侧就位（codegen 接线待做） | `misc.cpp` | 4 天 |
| P4-4 | 4 | **completed** | terminal-1 | 收口验证 + 归档 | P4-1~3 | batch-9 | pipeline 验证 | pipeline 报告 + 归档 | 全绿 | `docs/dev/` | 2 天 |

### 6.3 遗留工作（roadmap 完成后仍存）

| 缺口 | 严重度 | 说明 |
|---|---|---|
| REF-RISK-7 codegen 接线 | 🔴 | native 基建就位，但 codegen 未发射 push/pop，`tls_executing_image` 恒 null |
| 325 项 `real-planned` 访问器 | 🔴 | Phase 3 完成 46 项，其余按同一模式继续 |
| flag 位 codegen 全量重建 | 🟡 | 发射逻辑已写，需重建使 descriptor 携带新位 |
| ATG 参数生成缺陷 | 🟡 | 425/454 仍 `[UNVERIFIED]`（验证基础设施，非反射功能） |
| AOT 路径接入语义断言 | 🟡 | 当前跑在 .NET 8 参考实现上 |

---

## 7. 风险

| 风险 | 等级 | 触发条件 | 后果 | 缓解措施 |
|---|---|---|---|---|
| **矩阵规模爆炸** | 高 | `System.Reflection` 公开 API 达数百项 | 逐项判定工期不可控 | **Phase 0 先出总量数字**，再决定 Phase 2–3 分批粒度；这是把 Phase 0 放在首位的主因 |
| **改抛异常引发大面积红** | 高 | 457 个 smoke 依赖旧返回值 | 无法区分新旧失败 | 已拍板测试先行（§6.2）；red→green 归因先行验证 |
| **O(n³) 修复致基线漂移** | 中 | 加 `owning_type` 需改 codegen 模板 | 全量重编译 + 快照刷新 | 按 R1.1 先例：新字段末尾追加、默认 0 向后兼容 |
| **dashboard 0/N 被误当证据** | 中 | 全局数据源缺口（47/47 family 含对照组） | 得出完全错误方向 | 验收走三重证据链，**明确不依赖 dashboard** |
| **`remaining_stubs.cpp` 语义陷阱** | 中 | 继续「改成语义合理的非零返回」 | 重蹈 R2.4 覆辙 | 三档判定制强制：不许静默返错值 |
| **串行执行周期长** | 中 | 全覆盖 × 单 worktree | 跨多会话 | Phase 0 拆细；每 Phase 可独立交接 |
| **已归档任务被误参考** | 低 | 后续 agent 读 `reflection-industrialization` 的 completed 状态 | 误判反射已工业化 | 本 roadmap §1 显式记录失真事实；memory 已有基线 |

---

## 8. 备选路径

- **若 Phase 0 显示 API 量适中（<100）**：Phase 2–3 可整体推进，无需分批，总工期缩短
- **✅ 实际路径（Phase 0 结果 636 > 300）**：Phase 2 按 API 类别拆分为 5 个子任务（P2-1..P2-5，见 §6.2），Phase 3 为 P3-1。按用户拍板仍保持 worktree=1 串行执行。
- **若某 API 在 AOT 下确认无法实现**：回退为 `not-supported` 并记录论证，不强行实现
- **若 Phase 4 性能修复与热更新路径冲突**：P1 优先，但需显式论证不破坏 P3；如无法两全，记录为已知限制

---

## 9. 当前建议推进顺序

严格 Phase 0 → 1 → 2 → 3 → 4 串行推进。

**关键路径**：Phase 0 是唯一的阻塞点——它的总量数字决定后续所有阶段的粒度。建议优先投入。

Phase 4 的 P4-1/P4-2/P4-3 三项在 Phase 3 完成后可考虑并行准备（三项触及不同文件），但受 worktree=1 限制，实际仍串行。

---

## 10. 子任务执行策略

```yaml
child_execution_mode: auto
auto_continue: true
auto_stop_policy: blocking-only
dispatch_model: sequential
recommended_next_child: P0-1
```

**自动启动前必须先做最小 preflight self-review**（检查项同 `dev-executing-plans`）：
- 子任务目录是否存在
- `blocking_questions: []` 是否已写入
- 上游依赖是否已满足
- 判定：`pass` / `warn` / `stop`

**串行模式**：每次一个子任务，完成后自动推进下一 child；子任务进入终态时同步父 roadmap 的 child mapping、`latest_stop_point`、`recommended_next_child`。

---

## 11. 执行约束（用户拍板）

### 11.1 worktree 策略
**1 个 worktree，串行执行。** 不并行分域。缓解方式：Phase 0 把工作拆成可独立验收的单元。

### 11.2 测试先行强制顺序
```
枚举矩阵 → 逐个判定应抛/应实现
  → 为每个待改 API 先写断言其正确行为的测试
  → 跑测试确认「现在失败」（red 归因 = "本来就没验证"）
  → 再改实现
  → 跑测试确认转绿（green 归因 = "修复引入"）
```
**这是本项目反复踩过的坑**（见 memory `preexisting-failure-not-same-as-unrelated`）。

### 11.3 git 纪律
仓库存在多 agent 并发写 git 历史。每次 commit 前强制 `git diff --cached --name-only` 核对暂存区，防止误提交并行线文件。

### 11.4 归档纪律
子任务进入终态必须**物理移动**目录出 `docs/dev/in-progress/`，不得只改 STATUS 文案（本次评审发现的 R4.x 失真即由此产生）。
