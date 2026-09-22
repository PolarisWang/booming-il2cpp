# 设计 v1-01 — 段间依赖的显式化（pps-3 架构修订）

> 日期：2026-09-22
> 任务：`page-payload-split`（system chunk C1002 根治）
> 上游：`roadmap-v1-01.md`、`design-a-payload-split.md`、`design-b-hotpatch-chunk-wiring.md`
> 状态：**brainstorm 中，待用户确认问题清零**
> 触发：pps-3b 段拆分后连续 4 轮构建失败，暴露架构前提缺陷

---

## 一、背景：本设计要解决什么

pps-3b 把 page 0 的载荷段拆成 8 个独立 TU，**C1002 成功消除**（71MB → 24MB，不再堆耗尽）。
但构建无法通过：段与段之间的依赖在拆分后全部断裂，连续 4 轮修复（include →
MethodDeclarations → reflection helper → 跨段表符号）**每修一批暴露下一批**。

**本设计不推翻 A 方向**（拆大载荷段），而是补齐它缺失的一环：**跨段依赖的显式化**。

## 二、根因：一个被误认为纯性能缺陷的正确性机制

> 原来的「一个巨型 TU」不只是性能缺陷，**它还是一个正确性机制**。

段间依赖**从未被声明过**，它们靠「与引用方同处一个 TU」隐式满足：

| 跨段符号 | 数量 | 链接性 | 定义在 | 被引用 |
|---|---|---|---|---|
| `kSlots_*` | **96** | `static const` ⚠️ | page0（object model） | page-0007 |
| `kChaosGcSlotMapsSection/Size` | 2 | external ✅ | page-0006 | page-0007 |
| `chaos_codegen_code_registration` 等 | ~3 | external ✅ | page-0007 / page-0001 | 互引 |
| `chaos_reflection_create_*` | 4 | external ✅ | page0 | page-0001 |

**拆 TU 后**：external 者 → `C2065`（缺声明）；`static` 者 → `C2065` + 链接性错误。

### 2.1 设计原则（本方案的立论基础）

> **一组 TU 可安全拆分 ⟺ 所有跨 TU 引用都指向「共享契约头」中已声明的符号。**

- 80 个方法页 **满足**此条件（只引用共享头里的 `chaos_mt_*` / `chaos_type_id_*`）→ 早已跨 TU 正常工作
- payload 段 **不满足**（跨段符号从未进入任何头）→ 拆分即断裂

道路图的设计文档 §4.2 曾列出「**声明同步器**」组件，但**从未实现**。本设计将其落地。

## 三、🔴 关键发现：pps-1 的守卫测试是假绿

`PagePayloadSplitTests.PayloadTables_AreNotStaticWhenReferencedAcrossSections`
—— 这条**专为防 `static` 跨 TU 引用而写的守卫**，**当前通过**（实测 1 passed），
**而产物中存在 96 个 `static` 跨 TU 引用**。

### 3.1 机制（已实测）

定义行形态：
```cpp
static const ::chaos::il2cpp::vtable_registry::VTableSlot kSlots_Foo[] =
```

守卫正则：
```
^[ \t]*static\s+[^\n;=]*?\b(k[A-Z]\w*|chaos_\w+)\s*(\[[^\]]*\])?\s*=
```

实测三种定义形态**全部匹配数为 0**（含 `static const U32 kFoo[] =`）：

| 输入 | 匹配 |
|---|---|
| `static const ::chaos::il2cpp::vtable_registry::VTableSlot kSlots_Foo[] =` | 0 |
| `static const VTableSlot kSlots_Foo[] =` | 0 |
| `static const U32 kFoo[] =` | 0 |

而 `\bk[A-Z]\w*` 单独匹配 `kSlots_` **成功** → 失败点在 `[^\n;=]*?`（懒惰量词拿不下类型名段）。
**该正则对 `static` 表的检出率恒为 0。**

### 3.2 与既有前科一致

本项目已有两次同类事故（记忆：`guard-test-must-not-satisfy-itself`、
`guard-test-global-count-is-self-satisfying`）。判据是「**我见过这个测试红吗**」——
pps-1 的 commit message 记「负控 2/3 红」，**这条 L3 恰是没红的那个**，被当作已实现。

### 3.3 后果

roadmap Phase 1（链接性改造）**声称完成** + 守卫**声称保护**，但两者都失效：
- Phase 1 实际只改了 6 个符号
- 96 个 `kSlots_*` 从未覆盖
- 守卫本该抓住却恒绿

**→ 前 4 轮人工修复，实质是在替一条失效的守卫干活。**

## 四、方案：Planner 登记符号表 + 契约头（A″）

### 4.1 结构

```
Planner（生成各段时）
  ├── 登记：每个跨段符号 (符号名, 定义段, 链接性, 完整类型)
  └── 产出：PayloadContractHeader（契约头文本）
        ↓
Emitter
  ├── 发射 native-aot.payload.contract.h ← 所有 payload TU 都 include
  └── 按段发射 TU（每 TU 只含自己的段内容 + contract 头）
```

### 4.2 组件与职责

| 组件 | 职责 | 位置 |
|---|---|---|
| **符号登记器** | 各段生成时登记其**对外提供**的符号（名/类型/链接性） | `NativeAotLoweringPlanner.*.cs` 各发射点 |
| **契约头生成器** | 汇集登记表 → 发射 `extern` 声明头 | `NativeAotEmitter.Shared.cs::BuildSharedHeader` 扩展 |
| **链接性改写** | 跨段符号的 `static` → `extern` | 各发射点（96 个 `kSlots_*` 等） |
| **守卫修复** | 正则改为锚定真实形态 + **负控用真实产物** | `PagePayloadSplitTests.cs` |

### 4.3 关键设计决策（用户已拍板）

| # | 决策 | 理由 |
|---|---|---|
| D1 | 声明来源 = **Planner 登记**（非发射后文本扫描） | 类型化、完备、不会漏；文本扫描已被 4 轮失败证伪 |
| D2 | 96 个 `kSlots_*` **改 `extern` + 契约头声明** | 与 roadmap Phase 1 已定链接性策略一致 |
| D3 | 不变更段的划分依据 | 段边界 = 生成结构边界，保持稳定 |

## 五、三优先级权衡

| 优先级 | 评估 | 结论 |
|---|---|---|
| **P1 性能最优** | 声明与链接性改动**零运行期影响**（roadmap 已实测三情形指令序列字节级相同） | ✅ 中性偏正 |
| **P2 架构完美** | 把**隐式依赖显式化**，是唯一根治选项；落地 roadmap 搁置的「声明同步器」 | ✅ **强正向** |
| **P3 热更适配** | 不改符号名、不改 ABI、不改 hotpatch 语义 | ⚪ 中性 |

**无冲突。**

## 六、风险评估

| # | 风险 | 触发 | 后果 | 缓解 |
|---|---|---|---|---|
| R1 | 登记遗漏 | 某发射点未登记 | 该符号 C2065 | 守卫重写为**从产物反向校验**（见 §七） |
| R2 | `static`→`extern` 改漏 | 96 个中部分漏改 | LNK2019 | 守卫 + 产物扫描双向核对 |
| R3 | **守卫再次假绿** | 正则/断言自我满足 | 缺陷逃逸 | **负控必须用真实产物**（非合成 fixture） |
| R4 | 类型不完整 | 契约头只有前向声明 | C2737 | 契约头须含**完整类型定义**或 include 来源头 |
| R5 | TPG 陈旧 DLL | 改 Generator 未重建 | 静默跑旧 codegen | 改后必重建 TPG |
| R6 | 隐藏的第 N 类依赖 | 仍有未发现的跨段引用 | 再次失败 | **先做一次全量扫描列出完整清单**（见 §七.2） |

## 七、验证策略

### 7.1 守卫重写（必须先做，否则改完无法验证）

- **断言锚定真实形态**：从产物直接提取 `static` 定义行，逐条判定是否跨 TU 引用
- **负控用真实产物**：回退改动 → 用**真实 system chunk 产物**跑守卫 → 必须红
- **禁止**用合成 fixture 做 L3 负控（本项目已两次栽在合成/全局计数上）

### 7.2 先出完整清单（执行第一步 — 已验证必要性）

在改任何代码前，先全量扫描产物，列出**所有**跨 TU 引用符号及其链接性。

**R6（隐藏依赖）已实测坐实**：初版清单只识别了 4 类（`kSlots_*` / `kChaosGcSlotMaps*`
/ `chaos_codegen_code_registration` / reflection helper）。进一步扫描又发现**未被首轮
识别的类别**：

| 新发现类别 | 引用方 | 定义方 |
|---|---|---|
| `kRefl_desc_*` | page-0001 | page-0008 |
| `kGenericMethodArgTokens/Entries`（+`*Count`） | page-0007 | pps-2 已改 extern 的表 |

→ **手工枚举必然遗漏**，这正是 D1（Planner 登记）优于文本扫描的实证。
清单必须由**登记器产出**，而非人工整理。


## 八、架构映射

- il_feature: n/a（发射结构，非 IL 翻译路径）
- translation_path: n/a
- planner: `NativeAotLoweringPlanner.*.cs`（符号登记）
- emitter: `NativeAotEmitter.Shared.cs::BuildSharedHeader`（契约头）
- runtime_support: **n/a（runtime 零改动）**
- constraints_followed: 复用既有 shared-header 机制（非新发明）
- new_constraints: **跨段符号不得为 `static`**（需 external linkage）
- consistency_check: ok（与既有 `chaos_mt_*` / B3 resolver 声明模式同构）
- snapshot_tests: pending
- wiki_entry: needs-update

## 九、问题清零

```
blocking_questions: []
question_clearance: cleared
clearance_confirmed_by_user: true
```
