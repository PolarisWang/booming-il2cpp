---
name: dev-il2cpp-core-agent
description: il2cpp 核心开发 Controller — Hub-and-Spoke 分发循环。派发→Expert 做一部分→收回→再派发，直到全部完成
---

# dev-il2cpp-core-agent — il2cpp 核心开发 Controller

## 概述

本 skill 是 **所有 il2cpp 开发任务的统一入口**。采用 **Hub-and-Spoke + Loop-back** 分发模式：

```
Dispatcher 接收任务
  │
  ├── 拆分子任务清单
  ├── 循环:
  │     ├── 选一个 Expert
  │     ├── Skill("dev-xxx-expert") → 注入知识
  │     ├── Expert 做自己能做的部分
  │     ├── Expert 标记:
  │     │   ✅ done:     [已处理的部分]
  │     │   ⏳ remaining: [未处理的部分]
  │     ├── Dispatcher 更新待办清单
  │     └── 待办为空? → 退出循环; 否则 → 选下一个 Expert
  │
  └── ✅ 全部完成 → 质量门
```

核心原则：
- **Expert 做 partial**：每个 Expert 只做自己域内的部分，做不完的标记 remaining 回 Dispatcher
- **Dispatcher 管进度**：维护待办清单，循环分派直到清空
- **终止守卫**：连续 N 轮无进展 → 报错终止，防止无限循环

---

## 触发条件

- 用户会话从 `dev-using-booming` 路由到 il2cpp 域（替换模式）
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

这是核心循环，反复执行直到待办清单为空。

```
todo = [子任务清单]       ← 初始 = 阶段 2 的输出
tried = []              ← 已尝试过的 Expert
round = 0

while todo 非空:
  round++
  if round > MAX_ROUNDS (默认 10):
    报错: "超过最大轮次，未完成: {todo}" → 终止

  target_expert = 从分类矩阵选最匹配 todo 的 Expert
  if target_expert in tried:
    报错: "Expert {target_expert} 已尝试过但仍有剩余，其他 Expert 也无法处理 → 人工介入"

  tried.append(target_expert)
  输出 classification: "domains=[{todo的域}] mode=knowledge-inject expert={target_expert} round={round}"

  Skill("dev-{target_expert}")  →  加载 Expert 知识到当前上下文

  /* 当前 Agent 利用 Expert 知识处理 todo 中自己能做的部分 */
  ✅ done:       [已处理的子任务]
  ⏳ remaining:  [未处理的子任务]

  todo = remaining
  /* 进入下一轮 while 循环 */
```

**终止守卫**：
```
MAX_ROUNDS = 10（默认）
- 超过 10 轮 → 终止，报残留任务
- 连续 3 轮 todo 无缩减 → 终止，报"无进展"
- 所有 Expert 已尝试但 todo 仍有残留 → 终止，报"无法处理的子任务"
```

### 阶段 4: 质量门

全部子任务完成后，执行质量门：

```
□ 如果有代码修改 → dev-trace-enforcement
□ dev-verification-before-completion
□ 更新 STATUS.md（内容摘要、文件列表、验证结果）
□ 如果涉及翻译路径变更 → wiki 维护
□ 如果涉及多域串行修改（≥2 Expert）→ 运行 foundation-dll pipeline 集成验证
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
2. **每轮输出 classification 声明** — 格式 `classification: domains=[...] expert=xxx round=N`
3. **维护待办清单** — 在上下文中显式维护 todo list，每轮同步
4. **终止守卫不可跳过** — 超过 MAX_ROUNDS 或无进展时必须终止，不能死循环
5. **Expert 标记 completed/remaining** — 每个 Expert 执行完后必须输出 done/remaining
6. **质量门不可跳过** — 阶段 4

## 集成点

| 上游 | 本 skill | 下游 |
|------|----------|------|
| `dev-using-booming` → il2cpp 路由 | **dev-il2cpp-core-agent** (Dispatcher) | `Skill("dev-il2cpp-*-expert")` — 知识注入 |
| 用户直接输入 | | `Skill("dev-project-test-governance")` |
| | | `Workflow({scriptPath: "orchestrator.workflow.js"})` |
