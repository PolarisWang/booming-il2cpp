---
name: dev-il2cpp-core-agent
description: il2cpp 核心开发 Controller — Hub-and-Spoke 分发循环。派发→Expert 做一部分→收回→再派发，直到全部完成
---

# dev-il2cpp-core-agent — il2cpp 核心开发 Controller

## 概述

本 skill 是 **所有 il2cpp 开发任务的统一入口**。采用 **Workflow 委托 + Hub-and-Spoke 混合**模式：

```
Dispatcher 接收任务
  │
  ├── 拆分子任务清单
  ├── 循环:
  │     ├── 单域 → Skill 注入 → 当前 Agent 自行实现
  │     ├── 多域 → Workflow 委托 → 并行分派给各 Expert
  │     ├── Expert 标记:
  │     │   ✅ done:     [已处理的子任务]
  │     │   ⏳ remaining: [未处理的子任务]
  │     ├── Dispatcher 更新待办清单
  │     └── 待办为空? → 退出循环; 否则 → 继续下一轮
  │
  └── ✅ 全部完成 → 质量门
```

核心原则：
- **Workflow 优先**：多域任务默认走 `Workflow({scriptPath: ...})` 并行委托，不询问用户
- **Expert 做 partial**：每个 Expert 只做自己域内的部分，做不完的标记 remaining 返回 Dispatcher
- **Dispatcher 管编排**：生成 Workflow 脚本，定义 Expert 之间的依赖和并行关系
- **终止守卫**：Workflow 中定义 bounded retry（max 3），超时或连续无进展 → 报错终止

---

## 触发条件

- 用户会话从 `dev-il2cpp` 路由到 il2cpp 域（替换模式）
- 用户直接输入 `/dev-il2cpp-core-agent`
- 用户输入包含：il2cpp、翻译、IL 指令、AOT、codegen、GC、运行时、runtime-core、interpreter、VTable、Emission、Planner、NativeAot、热更新

---

## 四阶段流程

### 阶段 1: il2cpp 语境加载（Context Loading）

```
1. 重申全局优先级约束:
   P1（最高）= 性能最优 > P2 = 方案完美性 > P3 = HotUpdate 支持

2. 加载 00-快速导航.md:
   读取 wiki/03-功能模块/06-il2cpp核心架构/00-快速导航.md

3. 检查工作区状态:
   - 当前分支和 git 状态
   - STATUS.md / ACTIVE.md

4. 建立会话级"il2cpp 态"
```

### 阶段 2: 任务分解（Task Decomposition）

将用户输入的原始任务拆分为 **子任务清单**：

```
用户: "翻译 newobj + 改 runtime helper + 加 GC 写屏障"
  → 子任务清单: [
       {id: T1, desc: "翻译 newobj 指令", domain: "translation", status: pending},
       {id: T2, desc: "实现 runtime helper", domain: "runtime", status: pending},
       {id: T3, desc: "添加 GC 写屏障", domain: "gc", status: pending},
     ]
```

子任务颗粒度：一个 Expert 一轮能完成的量。太粗→Expert 做不完；太细→循环太多轮。

### 阶段 3: 分发循环（Dispatch Loop）

循环执行直到待办清单为空。单域走 Skill 注入，多域走 Workflow 委托。

```
todo = [子任务清单]       ← 初始 = 阶段 2 的输出
round = 0

while todo 非空:
  round++
  if round > MAX_ROUNDS (默认 5):
    报错: "超过最大轮次，未完成: {todo}" → 终止

  domains = todo 涉及的所有域

  if domains == 1:
    ── 单域: 用 Skill 注入
    target_expert = 从分类矩阵选匹配的 Expert
    Skill("dev-{target_expert}") → 加载知识
    当前 Agent 处理 todo 中自己能做的部分
    ✅ done / ⏳ remaining → 更新待办

  else:
    ── 多域: 走 Workflow 委托（默认，不询问用户）
    1. 子任务按 Expert 域分组
    2. 生成 Workflow 脚本:
       export const meta = { name, phases: [...] }
       parallel: 各 Expert 并行处理（无依赖时）
       pipeline: 有依赖时定义先后顺序
       每个 agent() 带 schema 约束输出格式
    3. Workflow({script}) → 异步并行执行
    4. 收集各 Expert 结果:
       ✅ done / ⏳ remaining → 更新待办

  输出 classification: "本轮任务涉及 {domain_names(IDs)} ，{mode} 操作，第{round}轮"
  /* 进入下一轮 while 循环 */
```

**终止守卫**：
```
MAX_ROUNDS = 5（默认）
- 超过 5 轮 → 终止，报残留任务
- 连续 2 轮 todo 无缩减 → 终止，报"无进展"
- 所有 Expert 已尝试但 todo 仍有残留 → 终止，报"无法处理的子任务"
```

### 阶段 3a: 未知域解析（Adversarial Jury）

当子任务域在分类矩阵中无匹配时，触发对抗陪审流程。

#### 检测与触发

```
分类矩阵无匹配 → 记录到 skills/.unknown-domains.json:
  {
    "domain": "ci-cd",
    "first_seen": "2026-06-11",
    "hit_count": 1,
    "sample_task": "配置 GitHub Actions pipeline",
    "status": "pending"   // pending | jury_passed | jury_rejected | registered
  }

hit_count >= 2 且 status == "pending" → 触发陪审
```

#### 陪审流程

```
Step 1: Generator — 根据 sample_task 生成 skill draft
         模板强制包含:
           □ name / description / trigger keywords
           □ domain（做什么）
           □ boundaries（不做什么）
           □ 3 个 handling scenarios
           □ quality checklist（自评）

Step 2: 3 个独立 Jury Agent 并行审查（Workflow 委托）

  ┌─────────────────────────────────────────────────────────┐
  │ Jury 1 — 完整性审查                                      │
  │ 问题: 这个 skill 完整覆盖了该领域吗？                       │
  │ 输出: {verdict: PASS|FAIL, gaps: [...], suggestions: ...} │
  ├─────────────────────────────────────────────────────────┤
  │ Jury 2 — 边界性审查                                      │
  │ 问题: 与现有 skill 冲突/重复吗？ scope 清晰吗？             │
  │ 输出: {verdict: PASS|FAIL, overlaps: [...], fixes: ...}  │
  ├─────────────────────────────────────────────────────────┤
  │ Jury 3 — 可执行性审查                                     │
  │ 问题: 指导内容正确且可操作吗？                              │
  │ 输出: {verdict: PASS|FAIL, issues: [...], fixes: ...}   │
  └─────────────────────────────────────────────────────────┘

Step 3: 汇总裁决

  ≥2/3 PASS → 注册:
    1. 写 skills/library/skills/dev-il2cpp-{domain}-expert/SKILL.md
    2. 更新分类矩阵（追加新行）
    3. 更新 skills/.unknown-domains.json → status=registered
    4. 当前轮继续用新 skill 处理子任务

  <2/3 PASS → 拒绝:
    1. 收集 3 个 jury 的改进建议
    2. 更新 .unknown-domains.json → status=jury_rejected
    3. 附加建议供下次改进
    4. 当前轮走 Generic Fallback（当前 Agent 自行实现）
    5. 如果同一域再次触发 → 用上次的建议做针对性重生成
```

#### Generic Fallback

陪审拒绝后，或 hit_count < 2 时，走通用兜底：

```
分类矩阵无匹配 + 未触发陪审
  → 不用 Skill 注入
  → 当前 Agent 自行实现
  → 标记 ⏳ remaining: ["域 {domain} 无 Expert，用 generic fallback 实现"]
```

### 阶段 4: 质量门

全部子任务完成后，执行质量门：

```
□ 如果有代码修改 → dev-trace-enforcement
□ dev-verification-before-completion
□ 更新 STATUS.md（内容摘要、文件列表、验证结果）
□ 如果涉及翻译路径变更 → wiki 维护
□ 如果涉及多域修改（≥2 Expert）→ 运行 foundation-dll pipeline 集成验证
□ ✅ 全部通过 → 提交并推送：
   1. git add -A
   2. git commit -m 按规范格式:
      - 功能/优化: "<type>: <subject>\n\n<body>"
        type: feat / refactor / perf / chore / test / docs
      - bug 修复: 必须包含三段式根因摘要:
        root_cause — 一句话根因
        fix_strategy — 修复策略
        regression_check — 验证范围
   3. git push
```

---

## 分类矩阵（用于阶段 3 选 Expert）

> ⚠️ **执行顺序约束**：Translation Expert 和 CodeGen Expert 都涉及 Planner/Emission 文件。
> 当子任务同时包含翻译路径修改和 codegen 修改时，**必须先派发 Translation Expert，再派发 CodeGen Expert**。
> Translation Expert 完成翻译路径设计后，CodeGen Expert 在此基础上做代码生成修改。

| 子任务信号 | 目标 Expert |
|-----------|-----------|
| 新 IL 指令、翻译路径、Emission、Planner、Lowering | `dev-il2cpp-translation-expert` |
| runtime-core、interpreter、VTable、bootstrap、method_table、线程状态 | `dev-il2cpp-runtime-expert` |
| crash、segfault、test failure、异常行为 | `dev-il2cpp-debug-expert` |
| GC 相关、分配模式、内存回收、写屏障、stress test | `dev-il2cpp-gc-expert` |
| C# codegen、T4 模板、NativeAot lowering、snapshot | `dev-il2cpp-codegen-expert` |
| foundation-dll、subject、测试管线、manifest | `dev-project-test-governance` |
| 性能优化、profile 分析、benchmark | `dev-il2cpp-foundation-dll-optimizer` |
| 热更新、PatchLoader、patchdata、HotpatchDispatch | `dev-il2cpp-hotupdate-expert` |
| 编译失败、链接错误、codegen stub、dotnet build 失败、CMake 错误 | `dev-il2cpp-build-fixer` |
| fact 验证、fact_chunk、skip-list 维护、value_suspicious、dll 验证结果审计、ATG/TPG 质量审查、codegen 输出审查 | `dev-il2cpp-fact-verification-expert` |

---

## 输入/输出规范

### Expert 收到任务时的输出格式

每个 Expert 在处理完自己能做的部分后，必须在当前上下文中输出：

```
✅ done: [已处理的子任务 ID 列表]
⏳ remaining: [未处理的子任务 ID 列表 + 原因]
```

示例：
```
✅ done: [T1 — 翻译 newobj 指令已完成]
⏳ remaining: [T2 — runtime helper 的实现涉及 codegen 模板修改，超出翻译 Expert 范围]
```

### Dispatcher 收到 remaining 后的行为

```
1. 把 ✅ done 从 todo 中移除
2. 把 ⏳ remaining 保留在 todo 中，标记原因为"需要 XX Expert"
3. 重新进入 while 循环，选下一个 Expert
4. remaining 的原因帮助 Dispatcher 选更精准的 target_expert
```

---

## 输出约束

1. **先分类再行动** — 禁止跳过分类直接实现
2. **每轮输出 classification 声明** — 可读格式，如 `本轮任务涉及 运行时(1) + 构建(7) ，fix 操作，第 1 轮`
3. **多域默认 Workflow** — ≥2 域直接走 Workflow 委托，不询问用户
4. **维护待办清单** — 在上下文中显式维护 todo list，每轮同步
5. **Expert 标记 completed/remaining** — 每个 Expert 执行完后必须输出 done/remaining
6. **质量门不可跳过** — 阶段 4
7. **Workflow bounded retry** — max 3 次重试，超时则终止报残留

## 集成点

| 上游 | 本 skill | 下游 |
|------|----------|------|
| `dev-il2cpp` → il2cpp 路由 | **dev-il2cpp-core-agent** (Dispatcher) | 单域: `Skill("dev-*-expert")` — 知识注入 |
| 用户直接输入 | | 多域: `Workflow({script})` — 并行委托 |
| | | 质量门: `dev-trace-enforcement` / `dev-verification-before-completion` |
