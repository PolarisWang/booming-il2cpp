# 风险评估报告 — Skill 触发链（设计→brainstorm→plan→executing）

- task_id: `20260827-fixing-code-review-agent`
- 报告: `docs/dev/in-progress/20260827-fixing-code-review-agent/risk-report-skill-trigger-chain.md`
- 评估对象: 从"用户提需求 → 复杂度判定 → brainstorm → writing-plans/roadmap → executing-plans → 验证"的完整 skill 触发与逻辑
- 触发: 用户 review"设计到落地计划的整个 skill 触发与逻辑"
- 日期: 2026-08-28
- 方法: 实读 CLAUDE.md 第〇条 / check_classification hook / core-agent(含刚加的复杂度闸门) / dev-brainstorm / dev-il2cpp 路由, 交叉验证触发链是否真能发生

---

## 〇、自测诚实结论（先坦白）

A+B 复杂度闸门**只做了静态验证，未做动态验证**：

| 验证 | 状态 |
|------|------|
| `generate_skill_catalog.py --check` exit0 | ✅ 做（A+B 不破坏生成） |
| A+B 两文件 git diff 复核 | ✅ 做 |
| 复杂度闸门逻辑评审（DIRECT 条件/升档规则） | ✅ 做（纸面） |
| 单元测试 | ❌ 无（skill 是 markdown 文档, 无测试框架） |
| **端到端触发演练**（真实跑一轮"重活任务应触发 brainstorm"） | ❌ **未做** |

**核心盲区**：复杂度闸门是**文档指令**，没有自动测试能证明它在实战中真的被 agent 执行。当前只有"写进了 SKILL"的静态证据，**无"agent 真的会跑闸门"的动态证据**。

---

## 一、触发链现状（实读结论）

```
用户提需求
  │
  ├─ CLAUDE.md 第〇条: 分类声明 → 加载 dev-il2cpp → dev-xxx-expert   【已强制, hook 拦截】
  │
  ├─ core-agent 阶段1.5: 复杂度评估闸门 → complexity=direct|brainstorm|plan|roadmap  【新增,A+B, 无强制】
  │     ├─ direct     → 派发 expert 直接实现
  │     ├─ brainstorm → dev-brainstorm(清零执行前问题) → 分流 roadmap/writing-plans
  │     ├─ plan       → dev-writing-plans
  │     └─ roadmap    → dev-roadmap → 子任务 → ... → executing-plans → 验证
  │
  └─ 兜底: agent 读 registry 触发信号主观判断   【旧机制, 仍存在】
```

**关键事实**:
1. 分类门（第〇条 + hook）是**真强制**的——不写 `.classified` hook 拦截工具。
2. 复杂度闸门（A+B）是**新增的文档指令，但无 hook 强制**——不执行也不被拦。
3. brainstorm→plan 交接**有设计**（dev-brainstorm 明写分流到 roadmap/writing-plans + 轻量级模式）。
4. **但旧的主观兜底仍在**——两条路径并存，可能冲突。

---

## 二、风险清单

### 🔴 R1 — 复杂度闸门无强制，实际仍靠 agent 自觉（HIGH, 最重要）
**证据**：`check_classification.py` 中 `复杂度|complexity` = **0 处**；复杂度闸门只存在于 core-agent SKILL.md 文档，无 hook、无拦截、无 preflight 强制。
**后果**：A+B 把"要不要走流程"升级成"复杂度归哪档"，但**"不执行闸门"仍是自由裁量**。与改造前相比，只是判据更明确，机制上**并未保证触发**。若 agent 不主动跑闸门，流程仍不触发——**根因 R1 只解决了"判据", 未解决"强制执行"**。
**建议**：把复杂度判定加到 hook（如 check_classification 在分类时要求输出 `complexity=` 字段），或做成 preflight 检查，实现**真强制**。

### 🟠 R2 — 复杂度判定在需求当下未必可行（MEDIUM-HIGH）
DIRECT 显式条件含"单文件/单域/无跨子任务依赖/改动量小"——但**很多任务在接到需求当下无法判断**（要读代码才知道实际涉及几个域/多大改动）。
**后果**：agent 在不知道真实复杂度时，可能(a) 倾向保守归 direct（回到根因1默认）, 或 (b) 误归 brainstorm 过度流程化（僵化）。判定时机与判定可行性矛盾。
**建议**：闸门允许"先做最小探读（读目录/符号表）再判定", 明确"无法判断复杂度时必须升档"。

### 🟠 R3 — 两条判定路径并存，可能冲突（MEDIUM）
旧机制（读 registry 触发信号主观判断）与新复杂度闸门**并存**。若 agent 按旧路径（"这是单会话修复"直接实现）跳过闸门，或按新路径强制分级，两边结论可能不一致。
**后果**：行为不确定——同一类任务有时走流程有时直接实现，取决于 agent 选哪条路径。
**建议**：明确复杂度闸门为**唯一**复杂度判定入口, 旧主观兜底降级为"闸门失败后的 fallback"。

### 🟡 R4 — brainstorm 轻量级模式增加主观模糊点（LOW-MEDIUM）
dev-brainstorm:24 允许"AI 自主判断是否用轻量级模式"（父上下文充分时跳过全流程）。
**后果**：又一个主观判定点——"上下文是否充分"由 agent 自己判断, 可能过早跳流程导致边界未清就实现。
**建议**：给轻量级模式加显式条件（父设计已提供范围/接口/authority 三要素才可轻量）。

### 🟢 R5 — 复杂度闸门改变了机制但短期无行为改变（LOW）
闸门落地后不会立刻让流程触发——它依赖 agent 每轮主动执行。静默期（未 hook 化）内行为与改造前基本一致。
**后果**：用户观察"流程不触发"的问题短期不会自动缓解。
**建议**：配合 R1 的 hook 化, 或明示"预期几轮后流量型验证"。

---

## 三、风险矩阵

| # | 风险 | 严重度 | 可能性 | 缓解 | 残余 |
|---|------|--------|--------|------|------|
| R1 | 复杂度闸门无强制, 靠自觉 | 高 | 高 | hook 化复杂度字段 / preflight 强制 | 高(机制未闭环) |
| R2 | 复杂度判定在需求当下不可行 | 中-高 | 高 | 允许最小探读+无法判定必升档 | 中 |
| R3 | 新旧判定路径并存冲突 | 中 | 中 | 闸门为唯一入口, 旧路径降级 fallback | 中 |
| R4 | brainstorm 轻量模式主观 | 低-中 | 中 | 加显式三要素条件 | 低 |
| R5 | 机制改变短期无行为改变 | 低 | 高 | hook 化 + 周期验证 | 低 |

---

## 四、结论

**最核心风险 R1**：A+B 复杂度闸门解决了"复杂度判据缺失"（根因1），但**未解决"强制执行"**——闸门是无钩子的文档指令，实际触发仍靠 agent 自觉，与改造前无本质差异。

**要真正让 brainstorm→plan→executing 可靠触发，必须把复杂度判定做成强制机制**（hook/preflight），否则判据再清晰，agent 不主动跑也没用。

**建议优先级**:
1. **R1 → hook 化复杂度字段**（最高杠杆: 把 "complexity=" 加到 check_classification, 分类时强制声明, 与 .classified 同机制）——这是让闸门从"文档"变"机制"的关键一步。
2. R2/R3/R4 → 打磨判据与入口唯一性。
3. 之后做真正的端到端触发演练（自测盲区）验证。

---

## 附：与之前风险报告的关系
本报告聚焦**skill 触发链/复杂度闸门**（本次主题）。此前 `risk-report-skill-token-optim-a-b.md` 聚焦 token 优化 A+B 落地风险，`final-landing-skill-token-optim.md` 记录提交。三者互补, 本报告是新闸门的机制完备性审查。
