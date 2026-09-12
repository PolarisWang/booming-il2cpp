# Reflection 生产机品质补齐 — 设计 v1

> **task_id**: reflection-production-readiness
> **创建日期**: 2026-09-12
> **entry_skill**: dev-brainstorm
> **phase**: brainstorming → 待转入 roadmap
> **上游输入**: `docs/discuss/20260515-reflection-comprehensive-evaluation.md`（REF-RISK 15 项基线）
> **评审证据**: 2026-09-12 全系统 review（4 路并行代码普查）

---

## 1. 背景与问题陈述

2026-09-12 对反射系统做全量评审，对照 2026-05-15 的权威评估报告逐条复核，发现三类实质差距：

### 1.1 性能缺陷停滞 4 个月

| 编号 | 缺陷 | 位置 | 状态 |
|---|---|---|---|
| REF-RISK-1 | `GetReflectedType` O(n³) 三重嵌套循环 | `reflection/type_properties.cpp:588-614`(method) / `617-633`(field) | **未修** |
| REF-RISK-2 | 8 处 `static char` 静态缓冲区非可重入 | `internal_helpers.cpp:229/281`、`type_properties.cpp:471/515`、`type_stubs.cpp:266/338/378/418` | **未修** |
| REF-RISK-7 | `GetCallingAssembly`/`GetExecutingAssembly` 硬编码返 CoreLib | `reflection/misc.cpp:95/113` | **未修** |

已修 5 项（RISK-5 BindingFlags、RISK-6 FieldGetValue、RISK-8 Attribute scope、RISK-9 Event）。

### 1.2 验证真实性断裂（最危险）

`tests/e2e/translation/System.Private.CoreLib/chunks/reflection/results/fact.json`：

```json
{ "passed": 460, "total": 462, "unverifiedSmoke": 457, "realTotal": 5 }
```

**462 个 subject 中真实可验证断言仅 5 个（1.1%）**。系统报告"绿"，但绿色的含义是"没崩"，不是"对"。任何性能修复都无法验证收益。

### 1.3 进度记录失真

`docs/dev/completed/reflection-industrialization/` 17 个子任务全部标 `completed`，但：
- R2.4 exit criteria「`remaining_stubs.cpp` 清零」未达成——该文件仍 232 行 / 17 处 `return 0`
- R4.1–R4.8 声称创建的 7 个 proof 测试目录在 `testing/` 下**不存在**
- 完成模式=「把 stub 改成语义合理的非零返回」（`GetBaseDefinition` 返自身、`GetCallingConvention` 硬编码 1）

**处置（用户拍板）**：不重新打开已归档任务，重新定计划。

### 1.4 澄清项：dashboard 0/N 非反射缺陷

`docs/verification/foundation-dll-audit/.../families/*-fact.html` 全部 47 个 family 显示 `0/N` + `No method detail data available`，**包含与反射无关的对照 family（`array-indexing-copy` 0/13）**。这是 dashboard 全局数据源缺口，**不能**作为反射验收依据。

---

## 2. 边界拍板

### 2.1 覆盖

- `System.Reflection` 命名空间下公开 API 的逐项判定与补齐
- 反射 chunk 的验证真实性重建（smoke → 真实断言）
- REF-RISK-1/2/7 三项未修性能/正确性缺陷
- 静默降级 stub 的显式化改造

### 2.2 不覆盖

- `Reflection.Emit` / `TypeBuilder` / `AssemblyBuilder` — 归为 `not-applicable`，不计入分母（AOT 架构下本质不可实现）
- 反射 emit 性能对标 CoreCLR JIT — 非目标
- Mono.Cecil 集成 — 非本项目范围
- 反射相关调试器扩展 — 属调试域

---

## 3. 验收口径（核心拍板）

### 3.1 三档判定制

每个公开 API 必须落到三档之一，**不允许「静默返错值」这一档**：

| 档位 | 含义 | 验收证据 |
|---|---|---|
| **A. `real`** | native 有真逻辑，非 residual stub | managed 真实断言 + native 实现存在 + pipeline 通过 |
| **B. `not-supported`** | 显式抛 `NotSupportedException` | 测试断言确实验异常 |
| **C. `not-applicable`** | AOT 下无意义，已论证 | 论证记录 |

### 3.2 双重 real 档（用户拍板）

| 档位 | 性能要求 | 用途 |
|---|---|---|
| **fact real** | ❌ 不考虑性能（只需语义正确 + 真实断言通过） | Phase 1–3 推进度量，解耦语义修复与性能修复 |
| **最终 real** | ✅ 必须含性能（O(n³)/静态缓冲区解决后才算） | Phase 4 生产机品质验收 |

**设计意图**：让 Phase 1–3 能独立推进而不被性能修复阻塞，在最终验收处强制合并两个验收轴。

### 3.3 三重证据链

① managed 测试（**真实断言，非 smoke**）② native 实现存在（非 residual stub）③ foundation-dll pipeline 通过。

### 3.4 分母界定

- `System.Reflection` 命名空间公开 API **全集**（机器枚举，非现有 82 个 native 声明）
- `Reflection.Emit` 系列初始化为 `not-applicable`
- 矩阵本身作为交付物与计划骨架

---

## 4. 架构

### 4.1 分层不变更

沿用既有 descriptor-based const data 模式（Tier 0 ModuleRegistry + Tier 2 ReflectionQuery Image）。不新增抽象层。矩阵是**文档产物**，不进运行时。

### 4.2 覆盖矩阵结构

产出 `docs/dev/in-progress/reflection-production-readiness/api-coverage-matrix.json`：

```json
{
  "namespace": "System.Reflection",
  "typeName": "System.Type",
  "memberName": "GetMethod",
  "signature": "MethodInfo GetMethod(string, Type[])",
  "tier": "real | not-supported | not-applicable | unclassified",
  "realKind": "fact | final | none",
  "evidence": { "managedTest": "...", "nativeImpl": "...", "pipelineStatus": "..." },
  "riskRefs": ["REF-RISK-1"]
}
```

矩阵是**计划骨架**：每个 `unclassified` 行 = 一个待办项。

### 4.3 工具落点

新建 `src/tools/Chaos.IL2CPP.Tools.ApiSurfaceScanner`，复用 ATG 既有 `DllScanner.cs` / `MetadataLoadContext` 能力（`AutoTestGenerator.csproj:15` 已引用 `System.Reflection.MetadataLoadContext`）。遵守 ATG 层边界：只产出 `.cs`/`.json`，不产 C++。

---

## 5. 阶段切分

| Phase | 目标 | 退出标准 |
|---|---|---|
| **0. 建矩阵** | 枚举 API 全集 + 现有 82 native 声明 + 30 项 codegen 黑名单对齐；**先出总量数字** | 矩阵生成，全部行有初判 tier |
| **1. 证据基础设施** | 反射 chunk `realTotal: 5 → 大幅提升`；给每 API 写真断言；建立 red→green 归因能力 | 反射 chunk 真实断言覆盖主要 API 面 |
| **2. 三档判定落地** | 逐 API 判定；`not-supported` 改为抛异常（**测试先行**） | 矩阵无 `unclassified` |
| **3. 真实实现补齐** | 填 `real` 缺口（fact real 口径，不含性能） | 矩阵 `real` 档三重证据齐全 |
| **4. 收口** | 性能准入合并（最终 real）+ pipeline 全绿 + 归档 | 反射达生产机品质 |

**进入 roadmap 理由**：5 阶段、跨多次会话、多子任务、需稳定交接。

---

## 6. 执行约束（用户拍板）

### 6.1 worktree 策略

**1 个 worktree，串行执行**。不并行分域。缓解全覆盖 × 串行的工期压力：Phase 0 把工作拆成可独立验收的单元，每单元有明确完成信号。

### 6.2 改抛异常的顺序强制（关键）

当前 457 个 smoke 断言不校验返回值，直接改抛异常会让它们全部变红且**无法归因**。强制顺序：

```
枚举矩阵 → 逐个判定应抛/应实现
  → 为每个待改 API 先写断言其正确行为的测试
  → 跑测试确认「现在失败」（red 是"本来就没验证"）
  → 再改实现
  → 跑测试确认转绿（green 是"修复引入"）
```

**测试先行保证 red→green 归因永远清晰**——这是本项目反复踩过的坑（见 `preexisting-failure-not-same-as-unrelated`）。

### 6.3 git 纪律

仓库存在多 agent 并发写 git 的历史。每次 commit 前强制 `git diff --cached --name-only` 核对暂存区，防止误提交并行线文件。

---

## 7. 风险评估

| 风险 | 触发条件 | 后果 | 缓解 |
|---|---|---|---|
| **矩阵规模爆炸** | System.Reflection 公开 API 或达数百项 | 逐项判定工期不可控 | Phase 0 先出**总量数字**，再决定 Phase 2 是否按类别分批 |
| **改抛异常引发大面积红** | smoke 测试依赖旧返回值 | 无法区分新旧失败 | 已拍板测试先行（§6.2） |
| **O(n³) 修复触及 descriptor** | 加 `owning_type` 需改 codegen 模板 | 全量重编译 + 快照基线漂移 | 按 R1.1 先例：新字段末尾追加、默认 0 向后兼容 |
| **串行执行周期长** | 全覆盖 × 单 worktree | 跨多会话 | Phase 0 拆细；每 Phase 可独立交接 |
| **dashboard 0/N 被误当证据** | 全局数据源缺口（47/47 family） | 得出完全错误方向 | 验收走三重证据链，不依赖 dashboard |
| **`remaining_stubs.cpp` 语义陷阱** | 继续「改成语义合理的非零返回」 | 重蹈 R2.4 覆辙 | 三档判定制强制：不许静默返错值 |

---

## 8. 三优先级权衡

| 优先级 | 本方案处理 |
|---|---|
| **P1 性能最优** | O(n³) + 静态缓冲区作为**最终 real 档准入条件**；与全覆盖在 Phase 4 合并达标。fact real 档不设性能门槛，避免阻塞语义修复 |
| **P2 架构完美** | 沿用既有 descriptor 模式，不新增抽象层；矩阵落 `docs/dev/in-progress/`；ApiSurfaceScanner 遵守 ATG 层边界 |
| **P3 热更新** | 反射热更新能力是 Chaos 相对 Unity IL2CPP 的**本质优势**，修复不得破坏 `RegisterModule`/Tombstone 路径；列入回归检查 |

**裁决点**：P1 与 P3 冲突时（如统一 Handle 编码可能影响热更新注册），P1 优先，但不得故意破坏 P3 —— 需在设计变更时显式论证。

---

## 9. 问题清零

```
blocking_questions: []
question_clearance: cleared
clearance_confirmed_by_user: true
```

已拍板项：
1. ✅ 生产机品质 = API 全覆盖优先 + 三重证据链
2. ✅ 静默降级 → 显式抛异常（测试先行）
3. ✅ 覆盖分母 = 公开 API 逐个判定，机器枚举建矩阵
4. ✅ Reflection.Emit = not-applicable
5. ✅ fact real 不含性能；最终 real 必须含性能
6. ✅ worktree = 1 个，串行
7. ✅ 不重新打开已归档任务，重新定计划
8. ✅ 任务目录 = `reflection-production-readiness`

---

## 10. 下一步入口

转入 `roadmap`（dev-roadmap）。最小恢复入口：本文件 + `STATUS.md`。
