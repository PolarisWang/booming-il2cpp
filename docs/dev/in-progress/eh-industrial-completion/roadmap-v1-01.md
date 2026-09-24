# EH 工业化补齐 — Roadmap v1

> **task_id**: eh-industrial-completion
> **创建日期**: 2026-09-24
> **entry_skill**: dev-brainstorm → dev-roadmap
> **主 Expert**: `dev-il2cpp-eh-expert`（跨域协调）
> **协作 Expert**: translation(6) / runtime(1) / codegen(4) / jit(1) / platform(9) / fact-verification(5) / verification-pipeline(11)

---

## 一、目标与边界

### 目标

让 EH 子系统达到**工业级生产质量**，即：
1. 三平台（Windows / Linux / iOS+Android）的托管异常语义**等价**
2. C# 的 `throw`/`catch`/`finally`/`filter` 翻译**正确**（含异常类型匹配）
3. 有**独立 EH 用例集 + CI 门**防止再被破坏
4. **EH 铁律**成为 agent 开发时的强制约束

### 覆盖

- L2 codegen 包装层（硬编码 C++ try/catch → `CHAOS_EH_*` 宏）
- L3 异常翻译正确性（throw 对象 → catch 类型匹配链路）
- L4 异常类型表完备性（`FileOnlyExceptionTypeNames` + scanner CI 门）
- 三模式等价性验证框架
- EH 铁律落地（skill + 审查门）

### 不覆盖

- GC / 线程调度本体（非 EH）
- 调试器 EH 集成（CoreCLR 级）
- 各平台构建链的通用可移植性（→ platform-expert 独立推进）

---

## 二、现状基线（实测）

| 指标 | 值 | 来源 |
|---|---|---|
| 运行时 EH 机制（L1） | 成熟 | `eh-industrialization` 已完成 |
| codegen 硬编码 C++ catch | `MethodEmission.cs:474/499` | 本会话实测 |
| EH 逃逸（`preAssertionRaise`）| **~566 项，9 个 chunk** | fact.json 汇总 |
| 三模式等价性测试 | 无 | 无此门 |
| EH CI 门 | 无 | 无 |

**EH 逃逸分布**：system 266 / xml 311 / reflection 31 / text-json 25 / text 4 / threading 1

---

## 三、阶段列表

### Phase 1：L2 — codegen 包装层对齐三模式

| 字段 | 值 |
|---|---|
| **goal** | subject 外层包装改用 `CHAOS_EH_*` 宏；解决 C2712 使 SEH 模式可用 |
| **exit_criteria** | ① 产物中不再有硬编码 `try {/catch(chaos_managed_exception&)` 包装；② Windows 编译零 C2712；③ 三模式编译均通过 |
| **主 Expert** | `dev-il2cpp-translation-expert` |
| **协作** | `dev-il2cpp-eh-expert`（铁律审查）、`runtime-expert`（桥接 TU 的 native 侧） |
| **关键交付** | ① `MethodEmission.cs` 包装层改宏；② `seh_bridge.{h,cpp}`（SEH 独立 TU，零 C++ 对象）；③ `chaos_runtime_host.h` 接线 |
| **worktree** | `wt-eh-p1-wrap` |
| **铁律约束** | 铁律 1（禁硬编码）、2（C2712）、7（raise 路径禁 C++ 对象） |
| **watch_items** | 多一层函数调用的性能影响（P1 敏感）；桥接 TU 的链接正确性 |

### Phase 2：L3+L4 — 异常翻译正确性与类型表完备

| 字段 | 值 |
|---|---|
| **goal** | 确保 C# `throw` 的每个异常类型都能被 `catch` 正确匹配 |
| **exit_criteria** | ① 抛出对象 header.type_info 指向真实 `chaos_mt_<Type>`；② `ResolveTypeByName` 覆盖全部 throw 类型；③ 负控验证通过（不匹配的类型必须不捕获）|
| **主 Expert** | `dev-il2cpp-runtime-expert` + `dev-il2cpp-codegen-expert` |
| **协作** | `dev-il2cpp-eh-expert`（跨层协调）、`abi-expert`（对象编码） |
| **关键交付** | ① 类型表发射扩展（扫 C# throw 站点，非仅 native stubs）；② `ResolveTypeByName` 类型面补全；③ 异常对象编码规范化 |
| **worktree** | `wt-eh-p2-translate` |
| **依赖** | 依赖 P1（包装层不对齐时，类型匹配无法观测）|
| **铁律约束** | 铁律 3（新类型登记）、4（真实 MethodTable）、8（不匹配可观测）、11（禁兜底）|
| **watch_items** | 类型面扩展可能引入 MT 发射链上新的 LNK 问题（参照 `f824cf49f` 的 gate 修法）|

### Phase 3：测试框架 — 独立 EH 用例集 + CI 门

| 字段 | 值 |
|---|---|
| **goal** | 建立三模式等价性的独立验证网（不编译 ×3，用最小 EH 用例集）|
| **exit_criteria** | ① 每个 EH 模式一组最小用例（throw/catch/finally/filter/嵌套/跨线程）；② 三模式行为等价断言通过；③ CI 门接入，EH 回归即红 |
| **主 Expert** | `dev-il2cpp-fact-verification-expert` |
| **协作** | `dev-il2cpp-verification-pipeline`（CI 接入）、`dev-il2cpp-eh-expert`（用例设计）|
| **关键交付** | ① `tests/unit/eh/` 独立用例集；② 三模式构建脚本（只编 EH 小 TU，非整 chunk）；③ CI workflow gate |
| **worktree** | `wt-eh-p3-tests` |
| **并行** | **可与 P2 并行**（用例设计不依赖 P2 实现）|
| **铁律约束** | 铁律 9（双向验证）、10（三模式必跑）|
| **watch_items** | 三模式构建在 CI 的耗时；iOS/Android 无 CI runner（见 P4）|

### Phase 4：全平台验证 + 铁律落地

| 字段 | 值 |
|---|---|
| **goal** | ① 全平台（含 iOS/Android）EH 语义验证；② EH 铁律成为强制约束；③ 全量回归 |
| **exit_criteria** | ① Windows/Linux 实跑通过；② iOS/Android **设计验证**通过（构建链未就绪时以静态验证 + 文档化替代，明确标注）；③ EH 铁律进入 skill 并被 pre-commit/review 门引用；④ 全 chunk EH 逃逸指标下降到已知 stubGap 面 |
| **主 Expert** | `dev-il2cpp-platform-expert` + `dev-il2cpp-eh-expert` |
| **协作** | 全体 EH 相关 Expert |
| **关键交付** | ① 平台验证报告；② `dev-il2cpp-eh-expert` SKILL 定稿；③ EH 铁律接入审查门；④ 全量 fact 对比 |
| **worktree** | `wt-eh-p4-platform` |
| **依赖** | 依赖 P1/P2/P3 全部完成 |
| **watch_items** | iOS/Android 无 CI runner —— **验收口径必须诚实**：不能声称"已验证" |

---

## 四、并行与 worktree 策略

```
P1 (包装层)  ──┬──> P2 (翻译正确性) ──┐
               │                      ├──> P4 (全平台 + 铁律落地)
P3 (测试框架) ─┴──────────────────────┘
        ↑ 可与 P2 并行
```

| worktree | 分支 | 合回后 |
|---|---|---|
| `wt-eh-p1-wrap` | `eh/p1-wrap` | ✅ 合并 main 后**删除** |
| `wt-eh-p2-translate` | `eh/p2-translate` | ✅ 同上 |
| `wt-eh-p3-tests` | `eh/p3-tests` | ✅ 同上 |
| `wt-eh-p4-platform` | `eh/p4-platform` | ✅ 同上 |

**纪律**（来自实战教训）：
- 开 worktree 前先 `git worktree list` 核对无同名；核对 `.claude/.classified` 无并发会话
- **worktree 里跑 pipeline 时 `CHAOS_FOUNDATION_DLL` 必须指向 main 的 foundation**
  （指错会得到假信号 —— 曾因此误判"C# 改动不生效"）
- 合回 main 用 **cherry-pick 到最新 main**（避免并发会话的工作区冲突）
- **合并后立即 `git worktree remove` + `git branch -d`**，不留残留

---

## 五、三优先级权衡

| 工作项 | P1 性能 | P2 架构 | P3 热更 | 裁决 |
|---|---|---|---|---|
| P1 包装层改宏 | 中（多一层桥接调用）| 优（三平台统一）| 无影响 | **采用**，需 benchmark 验证无回归 |
| P1 备选：统一 CPP_THROW | ❌ 差（Windows 慢 2×）| 优 | 无影响 | **否决**（违 P1）|
| P2 类型表扩展 | 无影响 | 优（消除静默失败）| 无影响 | **采用** |
| P3 独立用例集 | 无影响（非全量 ×3）| 优 | 无影响 | **采用** |
| P4 iOS/Android 设计验证 | 无影响 | 中 | 无影响 | **采用**（诚实标注未实跑）|

---

## 六、风险评估

| 风险 | 概率 | 影响 | 缓解 |
|---|---|---|---|
| C2712 在桥接 TU 仍出现 | 中 | 高 | 桥接 TU 严格零 C++ 对象；参照 `tier_manager_seh.cpp` |
| 桥接层引入性能回归 | 中 | 高（P1）| benchmark 对照；只桥接 raise 路径不桥接热路径 |
| 类型表扩展引发 LNK | 中 | 中 | 复用 `f824cf49f` 的「MethodTable 定义已发射」gate |
| iOS/Android 无法实跑 | 高 | 中 | **验收口径诚实标注**为"设计验证"，不冒充实跑 |
| 并发会话冲突 | 中 | 中 | worktree 隔离 + cherry-pick 到最新 main |
| 修复被后续改动破坏 | 中 | 高 | **P3 的 CI 门是核心防线** —— 无门则一切白做 |

---

## 七、验收口径

| 层级 | 判据 |
|---|---|
| **P1** | 产物无硬编码 C++ 包装 + 三模式编译通过 |
| **P2** | 异常类型匹配正控 + **负控**通过 |
| **P3** | 三模式用例全绿 + CI 门可红（用负控证明） |
| **P4** | Windows/Linux 实跑 + iOS/Android 设计验证（诚实标注）|

**最终验收**：EH 逃逸指标（`preAssertionRaise`）从 ~566 降到「已归因为 stubGap 的面」，且 CI 门能拦住回退。

---

## 八、下一步入口

进入 `executing-plans`，从 **P1 + P3 并行**开始（P3 不依赖 P1）。
