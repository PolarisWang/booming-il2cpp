---
name: dev-fixing-code-review
description: 通用 code-review 结果修复 orchestrator — 逐条 meta-review 一份 review 报告(验证有效性/定严重度/找遗漏)，按 file:line 自动路由到底层 domain expert 落地修复并回归。
---

# 修复 Code Review 结果（通用 orchestrator）

## 概述

收到一份 **code review 报告**（任意域：GC/并发/JIT/CodeGen/翻译/…）时，本技能负责：
1. **Meta-review**：逐条验证 review 中的每条发现是否真/准/漏——不是照单全收。
2. **定严重度**：区分"运行期真实代价 / 防御性 / 文档与实现不匹配 / 假警报 / 遗漏"。
3. **发现遗漏**：补上 reviewer 没覆盖、但 meta-review 抓到的真实缺陷（最大增值）。
4. **按域自动路由**：从每条发现的 `file:line` 所属代码路径推断领域，分发到底层 domain expert。
5. **落地修复 + 回归**：每个实现者一个隔离 Agent，统一跑该域测试入口。

**关键约束：本技能是 domain-free 的。** 不内嵌任何具体领域分支（`gc_`/`jit_`/`codegen_` 等不作为路由逻辑）。
唯一路由源是 `file:line → expert-registry.json`（`.ai/skills/discovery/expert-registry.json`）。

---

## 职责边界

### 我负责
- 对任意 review 报告做 meta-review，逐条判定有效性与严重度
- 从 `file:line` 路由到正确的 domain expert
- 协调多域并行核查与落地修复
- 收各域回归门 + 反查原 review 是否每条都已落项或明确定延

### 我不负责（标记 remaining 回派发者，遵循 dev-il2cpp-core-agent 输出格式 `✅ done / ⏳ remaining`）
- 超出 review 范围的开放式重构
- 涉及测试治理 obligation 变更（走 `project-test-governance`）
- 纯性能优化 campaign（走 `dev-optimization-campaign`）

---

## 三层架构

```
dev-fixing-code-review（domain-free 控制器，skill 逻辑驱动）
        │  输入：一份 code review 报告（任意域）
        ▼
 Phase A  Meta-Review（单 agent，domain-free，纯审查逻辑）
        │  逐条：真/假/准/漏 → 严重度 → 可并/可延
        │  产物：修复清单 [ {file,line,claim,verdict,severity,domainHint,fixScope} ]
        ▼  domain 从 file:line 的代码路径自动推断（expert-registry.json，不写死）
 Phase B   按 domain 分组 → parallel 派发底层 domain expert
        │  GC → dev-il2cpp-gc-expert；JIT → dev-il2cpp-jit-expert；
        │  Runtime → dev-il2cpp-runtime-expert；CodeGen → dev-il2cpp-codegen-expert；…其余走 registry
        ▼  Phase A 的 verdict 作为"实现者盲分隔核查依据"（防单点污染）
 Phase C  Synth 汇聚裁决（交汇根因/合并冲突项/裁定延后/拒假警报）
        ▼ 每项由对应 expert 在自己域内实现 + 各自测试
 Phase D  回归门：跑该域统一测试入口 + 反查原 review 是否仍有未落项
```

---

## 混合编排（本 skill 的执行机制）

### 判定类（Phase A + Phase B）→ Workflow
- 每条发现独立、可并行、结果结构化 → 用 `Workflow` fan-out，schema 强校验 verdict
- 每个 domain expert **盲分隔**：只拿到 `(file,line,claim,fixScope)` 与它自己的领域知识，不看其它专家结论，防单点污染
- `parallel()` barrier：需所有核查返回才进入 Phase C 裁决

### 仲裁+执行类（Phase C + Phase D）→ 主循环内 Agent 派发
- Phase C 裁决（合并冲突/延后/拒假警报）在主循环，由 orchestrator 把关，不预写死
- Phase D 每个实现者一个 Agent（`isolation: worktree` 隔离文件冲突），主 agent 收回归 + 两阶段审查（遵循 `dev-subagent-driven-development`：规格合规 → 代码质量）

---

## Phase A：Meta-review 判定准则

对 review 每条发现，落在以下判定（**这是本 skill 的核心**）：

| 判定 | 含义 | 处置 |
|------|------|------|
| **真/运行期代价** | 落到实际运行期确定性影响（如确定性额外停顿、丢标记） | 必修，高优先 |
| **真/防御性** | 类型不匹配/半析构等，当前无实际资源逃逸，但留隐患 | 归一到干净写法，低优先，不阻塞 |
| **真/防错位** | 实现当前不挂（有隐式契约兜底），但注释/文档与实现矛盾，是脆弱的谎言 | 改两行收口对齐，severity 降级；根在契约/documentation |
| **假警报** | reviewer 误判，实现正确 | 拒，给技术理由 |
| **遗漏** | review 完全没覆盖、但 meta-review 抓到的真实缺陷 | **升级为主要项**（最高增值，优先于已列出项） |

**可并/可延判断：**
- **可并**：互补命、不同文件 → Phase B 并行
- **可延**：纯防御风格（如半析构但无资源逃逸）→ 单独列，不阻塞主线

> **教例（用于校准 severity，非路由分支）**：某次 GC review 中——
> - "ready 屏障缺 notify，全靠 1ms 超时兜底" → **真/运行期代价**（确定性额外 GC 停顿）→ 补 notify 即消
> - "手动析构漏了某个 non-trivial 成员（std::mutex）" → **真/防御性**（实际无资源逃逸）→ 用编译器析构归一，不阻塞
> - "无超时 wait 但注释称 bounded" → **真/防错位**（store 先于 notify 实际不挂，但注释是谎言）→ 改 wait_for 两行收口
> - "收敛检查持锁读他 worker 的 pending，写者无锁写" → **遗漏（review 没列）** → 互斥域相反的 data race + 漏标记窗口 → **升主**

---

## 输出契约（Phase A → Phase B 喂给实现者）

```json
[
  { "id":"A", "file":"<file:line>", "line":215,
    "claim":"<review 原始或 meta-review 补充的一句话>",
    "verdict":"CONFIRMED | PLAUSIBLE | REJECTED",
    "severity":"HIGH | MEDIUM | LOW",
    "domainHint":"<从 file:line 推断的 expert 名>",
    "fixScope":"<该项修复边界——给范围，不给答案（盲分隔）>" }
]
```

- `verdict` 由 Phase B 的 domain expert 独立复核后定论（Phase A 给倾向，Phase B 确认）
- `fixScope` 描述"修什么、不动什么"，避免实现者越界

---

## Phase D：回归门（通用）

1. 每项修复跑该域统一测试入口（CLAUDE.md：`python tests/runner/test_driver.py --layer unit`，不得新增 `[FAIL]`）
2. 若为 GC 域，走 gc-expert 场景选择器（M 并行 mark / F pinned root / D barrier 等）
3. **反查原 review 每条是否都已落项或明确定延** —— 防"只修一半"
4. 收尾固定链路：`结构告警与架构审视 → 测试通过 → 归档 completed → 合并&提交`

---

## 已知故障/陷阱

- **勿把教例当路由分支**：教例只校准 severity 尺度，不得作为"只处理这些域"的 whitelist。任何 `file:line` 都走 registry。
- **勿对 pending/lock 类做想当然**：data race 判定必须实读写者/读者是否持同一互斥域，不能用"加了把锁"当结论。
- **勿跳过契约反查**：只修"看起来重要"的几条、漏掉 reviewer 列的冷门项，是常见失职。

---

## 关联技能

- 上游：`dev-receiving-code-review`（被动的处理哲学）
- 协作：`dev-dispatching-parallel-agents`（构造 agent 上下文）、`dev-subagent-driven-development`（Phase D 两阶段审查）、`dev-il2cpp-core-agent`（底层 expert 路由）
- 遵循：`dev-executing-plans`（执行纪律）、`dev-verification-before-completion`（完成前验证）
