# 路由规则（Routing Rules）

> 本文定义路由**流程与策略**。所有域/Expert/关键词的**数据定义**统一在 `skills/discovery/expert-registry.json` 中，
> 本文不重复定义。

---

## 1. 分类声明与入口

**分类声明必须包含 Expert 加载声明**：

```
格式: 本轮任务涉及 CodeGen(4) ，fix 操作，第 1 轮 → 加载 dev-il2cpp → dev-il2cpp-codegen-expert
```

**执行顺序**:
0. 调用 `Skill("dev-il2cpp")` 加载入口技能，读取其 SKILL.md 获取路由指令
1. 输出分类声明（`dev-il2cpp` 为固定首加载）→ `echo "..." > .claude/.classified`
2. 按 dev-il2cpp 的路由协议读取对应 Expert 的 SKILL.md 加载知识
3. 编辑域文件 → hook 验证分类声明格式 + `loaded_expert` 首位为 `dev-il2cpp`

---

## 2. 分层路由架构

顶层 core-agent 按域分组路由到子 Controller，子 Controller 再分派到具体 Expert。

Expert 与子控制器的完整映射见 `expert-registry.json` 中的 `expert_sub_controller` 字段。

```
core-agent  →  runtime-ctl  →  运行时/调试/热更新 Expert
            →  gc-ctl       →  GC/优化 Expert
            →  codegen-ctl  →  CodeGen/翻译/构建/测试/热更新/平台 Expert
```

### 执行顺序约束

Translation Expert 和 CodeGen Expert 都涉及 Planner/Emission 文件：
- **必须先派发 Translation Expert，再派发 CodeGen Expert**
- Translation Expert 完成翻译路径设计后，CodeGen Expert 在此基础上做代码生成修改

---

## 3. 多域处理规则

| 域数 | 策略 |
|------|------|
| 1 | Skill 加载 Expert 知识后当前 Agent 自行实现 |
| 2 | Controller 串行调度 [主域 → 从域] |
| ≥3 | 默认走 Workflow 委托 |
| 不明确的输入 | 反问用户澄清后重路由 |

### 多域执行顺序约束

Translation Expert 和 CodeGen Expert 都涉及 Planner/Emission 文件：
- **必须先派发 Translation Expert，再派发 CodeGen Expert**
- Translation Expert 完成翻译路径设计后，CodeGen Expert 在此基础上做代码生成修改

---

## 4. 终止守卫

| 条件 | 行为 |
|------|------|
| 超过 5 轮 | 终止，报残留任务 |
| 连续 2 轮无进展 | 终止，报"无进展" |
| 所有 Expert 已尝试但仍有残留 | 终止，报"无法处理的子任务" |

---

## 5. 深度调试 Workflow 触发

当以下任一条件满足时，**必须切换为 Workflow 深度调试模式**，不得继续手动单域迭代：

| 触发条件 | 说明 |
|---------|------|
| 同一 chunk 的 fact 失败超过 5 轮未解决 | 当前 numerics chunk 已达 9 轮 |
| 失败根因横跨 2 个以上域 | CodeGen + 运行时 + 构建同时涉及 |
| 需要 trace C++ 编译/链接/运行全链路 | extern 声明 → 编译 → dispatch → SEH |
| 当前 Agent 连续 3 次尝试未取得进展 | 109/121 已停滞多轮 |

深度调试 Workflow 执行模式：

```
Step 1 — 并行域内调查（3 个独立 agent）
├── Expert A (CodeGen):   分析 extern 声明 emit 逻辑、slot map 生成、DirectNativeSymbol 路由
├── Expert B (运行时):    编译 entry.exe → trace dispatch 链 → 确认 12 个方法卡在哪一步
└── Expert C (构建):     分析 stub 插入点、cmake 增量编译导致 interop_stubs.cpp 不重新编译的问题

Step 2 — 汇聚根因
└── synthesize agent: 合并三域调查结果，输出 root_cause + fix_strategy + regression_check

Step 3 — 分域修复
├── CodeGen fix: 修改 extern 声明 emit
├── 运行时 fix:  修改 ChaoternalRuntimeFallback 拦截逻辑
└── 构建 fix:    修复 Python stub 插入点 + 缓存 key 含 runtime stub hash
```

---

## 6. 质量门

```
□ 如果有代码修改 → dev-trace-enforcement
□ dev-verification-before-completion
□ 更新 STATUS.md
□ 如果涉及翻译路径变更 → wiki 维护
□ 如果涉及多域修改（≥2 Expert）→ 运行 foundation-dll pipeline 集成验证
□ ✅ 全部通过 → 提交并推送
```

---

## 7. Expert 加载（A+B 模式）

Expert 的知识通过 Agent spawn 加载和执行：

```
单域:
  Core Agent 从 expert-registry.json 匹配 Expert 名
  → 读取 skills/library/skills/{expert}/SKILL.md
  → 提取 ===BEGIN_AGENT_PROMPT=== 块
  → Agent({spawn}, prompt=expert_knowledge + task)
  → 子 Agent 自动执行并返回结果

多域:
  Core Agent 按域分组
  → 选择 Workflow 模板（dual/triple/debug）
  → Workflow({scriptPath: template_path, args: {agents, tasks}})
  → 各 Expert 并行/串行执行
```

`Skill("dev-xxx-expert")` **不可用**（Claude Code 不支持子技能），替换为 Agent spawn。

Expert 注册表（唯一权威数据源）: `skills/discovery/expert-registry.json`

---

## 8. 跨域技能

跨域技能不绑定到单一域，定义在 `expert-registry.json` 的 `cross_cutting` 字段中。

激活方式：通过 discovery 流程读取 SKILL.md。

---

## 9. Hot Expert 缓存

常用 Expert 可直接引用 `.claude/.hot_skills`，跳过发现链（3 步 → 1 步）。

```
.hot_skills 中列出的 Expert 可直接读取其 SKILL.md 加载知识，
无需走 skill-index.md → registry → SKILL.md 的发现流程。
```

> ⚠️ `dev-il2cpp` 为固定首加载，使用 Hot Expert 前必须先走 dev-il2cpp 路由协议。Hot Expert 仅加速「找到入口后→读具体 Expert」这一步。

当前 Hot Expert：
- `dev-il2cpp`（入口技能，始终首加载）
- `dev-il2cpp-runtime-expert`
- `dev-il2cpp-codegen-expert`
- `dev-il2cpp-gc-expert`
- `dev-il2cpp-translation-expert`
- `dev-il2cpp-build-fixer`

---

## 10. 变更记录

| 日期 | 变更 | 原因 |
|------|------|------|
| 2026-06-14 | 删除所有 inline 数据表，统一引用 expert-registry.json | 消除多源冲突 |
| 2026-06-11 | 初始创建 | 从 CLAUDE.md / core-agent SKILL.md 提取为单一源 |
