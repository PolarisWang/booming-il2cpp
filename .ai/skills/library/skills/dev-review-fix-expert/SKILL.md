---
name: dev-review-fix-expert
description: 代码审查结果修复专属入口。按复杂度分级（triager）处理 review finding：simple 快速派发，complex 走 dev-brainstorm 深挖根因+多方案权衡后修复。
---

# 代码审查修复专家

收到代码审查反馈（平台 review 工具、ReportFindings、外部 review 报告、评审列表）时，用本流程修复。

## ① 铁律：拉起 expert agent 是不可绕过的前置步骤

**收到 review 结果的立即行为：**

1. **停止后续工具调用**
2. **执行域分类声明**（CLAUDE.md 第〇条：`echo "本轮任务涉及...{域}..." > .claude/.classified`）
3. **按整个 review 集映射到"≥2 域"规则** — 即使这些 finding 不属于开发新翻译路径，也按多域 Workflow 处理
4. **执行 triager 分级判定**（见下方复杂度签名清单）—— 对每条 finding 分流为 simple 或 complex。**不再一刀切"信任 review 直接派发"**。

## ② Triager：复杂度判定（派发前的核心分流）

对每条 review finding，**逐条扫描签名清单**。命中任一 → **complex**（强制走 dev-brainstorm 深挖，不得直接修）。未命中 → **simple**（走快速派发路径）。

### 复杂度签名清单（硬编码，禁止主观跳过）

| 序号 | 签名 | 说明与反例 |
|------|------|-----------|
| **S1** | **review 自相矛盾 / 与源码事实冲突** | 两条 review 对同一 finding 说法冲突，或 review 断言的事实与代码实际不符（如结构体布局偏移、断言值、调用链、API 签名）。例：A 说"offset 0 是 hasValue"，B 说"offset 0 是 value" → complex。 |
| **S2** | **涉及内存安全** | 关键词：`ref`、`by-ref`、`ref 返回`、`裸指针`、`NULL 解引用`、`UB`、`use-after-free`、`越界`、`悬挂指针`。例：ref-returning 方法返回 NULL、返回结构体基址而非字段指针 → complex。 |
| **S3** | **涉及契约一致性** | AOT / JIT / interpreter 三路之一行为不一致；跨平台（Windows/Linux/macOS/arm64）；AOT 与 JIT 结果的 by-ref 语义需一致 → complex。 |
| **S4** | **影响 GC / 运行时正确性** | 改涉及 safepoint、GC 调度、内存分配、线程状态、全局开关（如 `g_bgc_enabled`）。禁用特性规避根因 vs 修根因 → complex。 |
| **S5** | **多 finding 重叠 / 矛盾** | 同一行/同段代码被多条 review 反复命中，或修复 A 会破坏 review B 的主张。例：5 条 review 说同一件事但角度不同 → complex。 |
| **S6** | **僵尸代码 / 根因已修** | 修复对象是"为规避某 bug 写的硬编码/开关"，而 review 或 git 历史证明该 bug 已修。例：`g_bgc_enabled = false` 是规避 BgcSweep 挂起，但 RAII 守卫已修根因 → complex。 |

### 分流后的路由

```
收到 review 结果（N 条 finding）
  │
  ├── [triager] 逐条扫描复杂度签名清单
  │     ├── 全部 simple → 进入下方派发流（快速路径）
  │     └── 存在 ≥1 complex → 先收集 complex 集
  │
  ├── complex 集非空 → 走 complex 子协议（见③）→ 用户裁决 → 回续修
  │
  ├── simple finding 按域分组：
  │     ├── ≥2 域 → 多域 Workflow 并行派发
  │     └── 单域 → 当前 Agent 自行实现
  │
  └── 全部 finding 处理完成 → 验证 → commit
```

> ⚠️ **complex 不进派发池**：complex 集先走 dev-brainstorm 收敛方案并让用户拍板，之后才作为"方案已定"的 finding 进入派发（降级为 simple 执行）。

## ③ Complex 子协议（dev-brainstorm 接入）

complex finding 收集完毕后，严格按此执行：

### 步骤 1：停止派发

不读取更多文件做"确认性复核"，不开始派发 agent。直接进入根因链分析。

### 步骤 2：画完整根因链

对每个 complex finding，画 **现象→直接原因→上游根因→副作用面**：

```markdown
现象： 硬编码 g_bgc_enabled = false 禁用 GC 特性
直接原因：app_main 在 generated code 中写死 false
上游根因：BgcSweep 在协作模式下阻塞 std::mutex → safepoint hang
副作用面：长任务 app 失去 OOM 保护；benchmark 入口有合法测量需求
根因是否已修：是（ScopedPreemptiveMode RAII 守卫，commit X）
已修后硬编码变僵尸代码 → 需删除，但不能一刀切（benchmark 需保留）
```

### 步骤 3：调 dev-brainstorm 技能

```
Skill("dev-brainstorm")
```

走 dev-brainstorm 的三路径前置分类。review-fix 的 complex 一般落在：
- **Bounded**（有现成 flow 可改：补日志、加 guard、改配置传播）
- **Architectural**（改接口/影响 GC 正确性/新子系统）

**遵守 dev-brainstorm 的 HARD-GATE**：展示方案+用户批准前，不落地实现。
**遵守单向棘轮**：执行中发现隐藏复杂度，升级路径而非降级。

### 步骤 4：给 2-4 个修复方案 + 权衡表

每方案覆盖：
- **改造量**（小/中/大，行数估算）
- **是否解决根因**（是/否/部分）
- **风险**（低/中/高，含回归面）
- **OOM 保护/安全影响**（正/负/中性）
- **推荐理由**
- **三优先级权衡**（性能/架构/热更）

**示例输出格式**（对应刚才 B1-B4 四方案）：

| 方案 | 改造量 | 解决根因 | 风险 | OOM 保护 | 推荐 |
|------|--------|----------|------|----------|------|
| B₁ 删两行 | 极小(2行) | 依赖已修 | 中 | ✅ | |
| B₂ 宏开关 | 小(15行) | 依赖已修 | 低 | ✅ | |
| **B₃ 门控器** | 中(80行) | ✅ | 低 | ✅ | ✅ |
| B₄ 自检 | 大(150行) | ✅验证 | 中 | ✅ | 增强 |

**让用户裁决**，绝不默认只修一个。用户选定后，明确问"该方案是否继续"（轻量级确认门）。

### 步骤 5：回本流程继续修复

用户批准方案后：
- 将 complex finding 的"方案已定"状态标记为 simple（执行层面已无分歧）
- 把该 finding 投入派发池，按原流程（跨域 Workflow 或单域自行实现）修复
- 修复+验证+commit 与原流程一致

> ⚠️ complex finding 修复**必须回填根因链到 commit message** 的 root_cause 段
> （三段式 root_cause / fix_strategy / regression_check），避免"只修表面"。必须在
> `root_cause` 段引用完成根因已修的 commit hash。

## ④ Simple 快速派发路径

simple finding 的 routing 与原有流程一致：

```
simple finding 按域分组
  │
  ├── ≥2 域 → 启动多域 Workflow：
  │     每个域 = 一个 expert agent（含"该域全部 finding"）
  │     一次性并行派出全部 agent（不要分批 3+3）
  │
  ├── 单域（所有 finding 落于同一域）→ 当前 Agent 自行实现
  │
  └── 全部 finding 处理完成后：
       ├── 验证 → 按 CLAUDE.md「统一测试入口」强制自测：
       │    python tests/runner/test_driver.py --layer unit
       │    出现非 known 的 [FAIL]（OVERALL: FAILED）即为真实回归，须修复后再推
       │    报告见 tests/runner/test-report.json
       └── commit（三段式 root_cause/fix_strategy/regression_check）× push
```

> ⚠️ **域计数说明**：CLAUDE.md「代码审查触发」要求"按审查结果场景映射到 ≥2 域规则"，这是指**路由协议**（走 review-fix-expert 而非直接实现），不改变实际域数。按文件→域分组后，若实际落于单域，按上方单域路径执行。

## ⑤ 分工原则

- **按文件→域 切分**，一次性全量派发，不逐批补
- **每域一个 agent**，给全该域所有 finding（含文件:行号 + 完整诊断）
- 只有需要"该不该修"裁决的 finding 才单独核实（读反例源码）
- 批处理完后，主 agent 统一做一次跨域质量 review（不要全信子 agent 自述"已修"）
- **complex finding 不进派发池**：先走 dev-brainstorm 收敛方案并让用户拍板，之后才可作为"方案已定"的 finding 进派发池（简单化执行）

## ⑥ 禁止

- ❌ **对所有 finding 一刀切"信任 review 直接派发"** — 仅 simple 可 trust；complex 必须深挖
- ❌ 先 Read 多个文件复核再分工（simple 禁止；complex 的源码核实是根因链的一部分，不在此列）
- ❌ 分批派 agent（3+3）而非一次全派
- ❌ 让子 agent 自证"已修"就收工 —— 主 agent 需抽验
- ❌ git stash（CLAUDE.md 强制）—— 切换用 git worktree 或显式提交

## ⑦ 下游

- 有效实施路径已梳理 → 交接给 `dev-requesting-code-review`（请求审查）或按 CLAUDE.md 三规则落 commit
- 复杂多域 → 参考 `dev-dispatching-parallel-agents` 的并行编排
- complex 子协议 → 调 `dev-brainstorm` 技能实现根因链分析和方案权衡