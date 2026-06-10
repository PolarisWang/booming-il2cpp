---
name: dev-il2cpp-core-agent
description: il2cpp 核心开发统一入口 — 分类任务并路由到对应的 Expert Agent，完成后自动执行质量门
---

# dev-il2cpp-core-agent — il2cpp 核心开发 Controller

## 概述

本 skill 是 **所有 il2cpp 开发任务的统一入口**。它不直接实现任何代码，而是：
1. 加载 il2cpp 开发语境（优先级约束 + 架构知识索引）
2. 将任务分类到对应域（翻译/调试/运行时/GC/CodeGen/测试/复合）
3. 路由到对应的 **Expert Agent** 执行专业实现
4. 执行完成后自动注入质量门

核心原则：**专业的事情交给专业的 agent 处理**。

---

## 触发条件

- 用户会话从 `dev-using-booming` 路由到 il2cpp 域（替换模式）
- 用户直接输入 `/dev-il2cpp-core-agent`
- 用户输入包含以下关键词：il2cpp、翻译、IL 指令、AOT、codegen、GC、运行时、runtime-core、interpreter、VTable、Emission、Planner、NativeAot、热更新

---

## 三阶段流程

### 阶段 1: il2cpp 语境加载（Context Loading）

进入本 skill 后，首先执行语境加载：

```
1. 重申全局优先级约束：
   P1（最高）= 性能最优 > P2 = 方案完美性 > P3 = HotUpdate 支持
   低优先级与高优先级冲突时，低优先级让位于高优先级

2. 加载 00-快速导航.md：
   读取 wiki/03-功能模块/06-il2cpp核心架构/00-快速导航.md
   → 建立完整的架构知识索引

3. 检查工作区状态：
   - 当前分支和 git 状态
   - 检查 STATUS.md 是否存在、是否有进行中的任务
   - 检查 docs/dev/ACTIVE.md 活跃任务清单

4. 建立会话级"il2cpp 态"：
   确认当前会话专注于 il2cpp 核心开发，后续所有操作在此语境下展开
```

### 阶段 2: 任务分类与路由（Task Classification & Routing）

根据用户输入的任务描述，按以下分类矩阵匹配：

#### 分类矩阵

| 任务信号 | 域 | 路由目标 | 说明 |
|----------|-----|---------|------|
| 新 IL 指令、翻译路径、Emission、Planner、Lowering | **翻译** | `dev-il2cpp-translation-expert` | 含 `dev-architecture-first-development` 三阶段流程 |
| runtime-core、interpreter、VTable、bootstrap、method_table、线程状态 | **运行时** | `dev-il2cpp-runtime-expert` | runtime-core/interpreter 领域 |
| crash、segfault、test failure、编译错误、异常行为 | **调试** | `dev-il2cpp-debug-expert` | il2cpp 域定制化 debugging |
| GC 相关、分配模式、内存回收、写屏障、stress test | **GC** | `dev-il2cpp-gc-expert` | CRAG GC、分代、Page Decommission |
| C# codegen、T4 模板、NativeAot lowering、snapshot | **CodeGen** | `dev-il2cpp-codegen-expert` | C# codegen 管线、Scriban 模板 |
| foundation-dll、subject、测试管线、manifest | **测试** | `dev-project-test-governance`（复用现有） | 测试治理是全局职责 |
| 性能优化、profile 分析、benchmark | **优化** | `dev-optimization-campaign`（复用现有） | 复用现有 skill |
| 热更新、PatchLoader、patchdata、HotpatchDispatch | **热更新** | `dev-il2cpp-hotupdate-expert` | Hotpatch、PatchMetadataCache、Interpreter 路由 |
| **跨域**（命中 2+ 个域） | **复合** | Controller 保留 → 自行调度 | 见下方跨域处理 |

#### 单域路由（≥80% 清晰）

直接调用对应的 Expert Agent（通过 Skill 工具），传入原始任务描述作为 args。

```
用户任务 → 分类 → 路由到 Expert Agent → Expert 执行 → 返回结果 → 阶段 3 质量门
```

#### 跨域任务处理规则

**2 个域**：Controller 按 `[主域 → 从域]` 串行执行。
- 示例 "GC crash 发生在 runtime 分配处" → 先调 `dev-il2cpp-debug-expert` 定位 crash 根因 → 再调 GC 处理修复
- 示例 "翻译新 IL + 改 runtime helper" → 先调 `dev-il2cpp-translation-expert` 完成翻译 → 再调 `dev-il2cpp-runtime-expert` 实现 helper

**3+ 个域或涉及翻译+运行时+测试**：
- 默认走 `dev-roadmap` → `dev-writing-plans` → `dev-executing-plans` 生成正式多步计划
- 或提示用户是否使用 Workflow Orchestrator（当需要并行执行多个 Expert Agent 时）
- 与用户确认后再执行

**不明确的输入**：
- 先反问用户澄清："你的意思是做 X（翻译类）还是 Y（调试类）？"
- 根据用户澄清结果重路由

#### 执行模型决策树

```
用户输入任务描述
  │
  ▼
阶段 1: 加载 il2cpp 语境
  │
  ▼
阶段 2: 分类任务域
  │
  ├─ 单域（≥80% 清晰）→ 调用 Expert Agent
  │                            │
  │                            └→ 执行完成 → 阶段 3
  │
  ├─ 双域 → Controller 串行调度
  │       └→ 主域 Expert → 从域 Expert → 阶段 3
  │
  ├─ 三域+ → 走 roadmap / 或询问用户是否用 Orchestrator
  │          └→ 逐步执行 → 阶段 3
  │
  └─ 不明确的 → 反问澄清 → 重路由
```

### 阶段 3: 自动质量门（Post-Execution Quality Gates）

在 Expert Agent 执行完成后，自动执行以下质量门：

```
必做：
1. dev-trace-enforcement — 检查修改中是否缺少必要的 trace 点
2. dev-verification-before-completion — 执行验证门检查
3. 更新 STATUS.md — 必须包含：
   - 本次修改的内容摘要
   - 涉及的文件列表
   - 如果涉及翻译路径变更：## 架构映射 小节（违反 dev-architecture-first-development 要求时需补充）
   - 验证结果

按需：
4. 如果涉及翻译路径变更或 wiki 知识变更：
   → 建议执行 dev-project-wiki-maintenance 更新对应文档
5. 如果修改了 AOT/IL2CPP 新特性：
   → 检查是否需要更新 wiki 中的翻译路径参考表
```

---

## 与现有体系的集成

### dev-using-booming 集成

`dev-using-booming` 的路由表中，在 `Default (bounded, single-session, single-goal)` 分支之前已插入规则：

```
- IL2CPP/AOT/GC/Runtime/翻译/CodeGen/interpreter/VTable 相关工作
  → dev-il2cpp-core-agent
```

因此当用户进入 il2cpp 域的任务时，会自动跳转到本 Controller。

### Expert Agent 目录结构

所有 Expert Agent 位于 `skills/library/skills/dev-il2cpp-*-expert/SKILL.md`，当前已注册：

| Expert Agent | 文件 | 状态 |
|-------------|------|------|
| `dev-il2cpp-translation-expert` | 翻译专家 | ✅ Phase 1 |
| `dev-il2cpp-runtime-expert` | 运行时专家 | ✅ Phase 1 |
| `dev-il2cpp-debug-expert` | 调试专家 | ✅ Phase 1 |
| `dev-il2cpp-gc-expert` | GC 专家 | ✅ Phase 2 |
| `dev-il2cpp-codegen-expert` | CodeGen 专家 | ✅ Phase 2 |
| `dev-il2cpp-hotupdate-expert` | 热更新专家 | ✅ Phase 2 |

### Workflow Orchestrator

复杂跨域任务（3+ 域）可调用 Workflow Orchestrator：
- 模板文件: `skills/library/skills/dev-il2cpp-core-agent/orchestrator-template.workflow.md`
- 触发条件: 涉及 3+ 域 或 10+ 文件 或 ABI 接口变更
- 流程: 分解 → 并行执行 → 冲突检测 → 集成验证

---

## 输出约束

1. **Controller 不处理实现** — 本 skill 只做分类和路由，不写任何实现代码
2. **不做重复路由** — 如果已经处于某个 Expert Agent 内部，不再触发本 Controller
3. **质量门是强制的** — 阶段 3 不可跳过
4. **先分类再行动** — 禁止跳过分类直接猜测实现路径
5. **跨域任务必须与用户确认** — 不要替用户决定是串行还是走 roadmap

---

## 集成点

| 上游 | 本 skill | 下游 |
|------|----------|------|
| `dev-using-booming` → il2cpp 路由 | **dev-il2cpp-core-agent** | `dev-il2cpp-translation-expert` |
| 用户直接输入 /dev-il2cpp-core-agent | | `dev-il2cpp-runtime-expert` |
| | | `dev-il2cpp-debug-expert` |
| | | `dev-project-test-governance`（测试） |
| | | `dev-optimization-campaign`（优化） |
| | | `dev-architecture-first-development`（回退） |
| | | `dev-systematic-debugging`（回退） |
