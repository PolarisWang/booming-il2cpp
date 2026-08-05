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

### 阶段 1: il2cpp 语境加载 + 健康自检（Context Loading & Self-Check）

```
1. 重申全局优先级约束:
   P1（最高）= 性能最优 > P2 = 方案完美性 > P3 = HotUpdate 支持

2. 健康自检（断路器）:
   a. 列出 .ai/skills/library/skills/ 下所有 dev-il2cpp-*-expert 目录
   b. 与 <code>.ai/skills/discovery/expert-registry.json</code> 对比，检查是否有 Expert 存在但未注册
   c. 检查 registry 中 domains 字段是否为空或只有退化条目
   d. 结果:
      - ✅ 正常 → 继续
      - ⚠️ 有未注册 Expert → 发警告，继续
      - ❌ registry 损坏 → 启用降级模式:
        降级模式: 跳过 registry，直接输出:
        "请选择任务域: [1]运行时 [2]GC [3]调试 [4]CodeGen [5]测试 [6]翻译 [7]构建 [8]热更新"
        用户选择后 → 路由到对应域的 Expert

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

循环执行直到待办清单为空。单域当前 Agent 自行实现，多域走 Workflow 委托。

```
todo = [子任务清单]       ← 初始 = 阶段 2 的输出
round = 0

while todo 非空:
  round++
  if round > MAX_ROUNDS (默认 5):
    报错: "超过最大轮次，未完成: {todo}" → 终止

  domains = todo 涉及的所有域

  if domains == 1:
    ── 单域: 当前 Agent 自行实现
    expert = 从 expert-registry.json 匹配 Expert 名
    skill_md = 读取 .ai/skills/library/skills/{expert}/SKILL.md

    // 注入领域知识到当前上下文
    读取 SKILL.md → 提取领域边界、已知约束、执行流程
    // 当前 Agent 自行实现代码修改、编译、验证
    // 输出 done/remaining 标记

    ✅ done / ⏳ remaining → 更新待办

  else:
    ── 多域: 走 Workflow 委托（默认，不询问用户）
    1. 子任务按 Expert 域分组
    2. 从 expert-registry.json 读 workflow_templates
    3. 选择模板: 任意数量→multi, 深度调试→debug
    4. Workflow({scriptPath: template_path, args: {agents, tasks}})
    5. 收集各 Expert 结果:
       ✅ done / ⏳ remaining → 更新待办

  输出 classification: "本轮任务涉及 {domain_names(IDs)} ，{mode} 操作，第{round}轮 → 加载 {expert}"
  echo classification > .claude/.classified
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

当子任务域在 registry 中无匹配时，触发对抗陪审流程。

#### 检测与触发

```
registry 无匹配 → 记录到 .ai/skills/.unknown-domains.json:
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
    1. 写 .ai/skills/library/skills/dev-il2cpp-{domain}-expert/SKILL.md
    2. 更新 .ai/skills/discovery/expert-registry.json（追加新行）
    3. 更新 .ai/skills/.unknown-domains.json → status=registered
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
registry 无匹配 + 未触发陪审
  → 不用 Skill 注入
  → 当前 Agent 自行实现
  → 标记 ⏳ remaining: ["域 {domain} 无 Expert，用 generic fallback 实现"]
```

### 阶段 4: 质量门（三级）

根据修改范围和影响面自动选择级别：

```
L1（快速 — 注释/常量/纯新增文件）:
  □ dotnet build（C# 项目）
  □ git diff 自查（无遗留调试代码）
  □ 更新 STATUS.md
  耗时: ~10s

L2（标准 — codegen 输出变更/翻译路径修改/已有逻辑修改）:
  □ L1 全部
  □ dev-trace-enforcement（trace 点检查）
  □ 如果涉及 C# codegen → snapshot 测试
  □ 如果涉及翻译路径 → wiki 维护
  耗时: ~2min

L3（完整 — 翻译路径变更/AOT 输出变更/ABI 修改/多域修改）:
  □ L1 + L2 全部
  □ dev-verification-before-completion
  □ 如果涉及多域（≥2 Expert）→ foundation-dll pipeline 集成验证
  □ ✅ 全部通过 → 提交并推送
  耗时: ~10min
```

---

## Expert 路由

域编号 → Expert 名、关键词 → Expert 名、子控制器分组的**完整映射**统一在 `.ai/skills/discovery/expert-registry.json` 中定义，本文不重复。

拓扑结构（完整映射见 expert-registry.json）：

```

core-agent  →  runtime-ctl  →  runtime-expert / jit-expert / debug-expert
            →  gc-ctl       →  gc-expert / foundation-dll-optimizer
            →  codegen-ctl  →  codegen-expert / translation-expert / build-fixer /
                               fact-verification-expert / hotupdate-expert / platform-expert /
                               abi-expert / external-runtime-expert / pipeline-expert /
                               simd-expert / codegen-capabilities / verification-pipeline
```

跨域 Expert（跨越多个 domain，组织上归属于 codegen-ctl）：
- abi-expert — 涉及运行时(1) + CodeGen(4) + 构建(7)
- external-runtime-expert — 涉及运行时(1) + CodeGen(4)
- pipeline-expert — 涉及测试(5) + 构建(7)
- simd-expert — 涉及 CodeGen(4) + 翻译(6)

### Expert 查找顺序

1. 按域编号查 `expert-registry.json.domains[N].defaultExpert`
2. 如果域编号不明确，按任务描述关键词查 `expert_registry.json.expert_keywords`
3. 如果仍不匹配，通过子控制器查 `expert_registry.json.expert_sub_controller`
4. 以上都不匹配 → 降级为当前 Agent 自行实现

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
| `dev-il2cpp` → il2cpp 路由 | **dev-il2cpp-core-agent** (Dispatcher) | 单域: 当前 Agent 自行实现（注入 Expert SKILL.md 知识） |
| 用户直接输入 | | 多域: `Workflow({scriptPath})` — 并行委托 |
| | | 质量门: `dev-trace-enforcement` / `dev-verification-before-completion` |
