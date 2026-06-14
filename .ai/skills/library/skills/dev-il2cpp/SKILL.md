---
name: dev-il2cpp
description: 项目唯一入口 skill — 对话启动时加载，通过 skill-index.md 发现领域并路由到对应技能
---

<SUBAGENT-STOP>
如果你是作为子 Agent 被派发来执行一个已经明确边界的子任务，跳过本技能。
</SUBAGENT-STOP>

<EXTREMELY-IMPORTANT>
只要有任何一个技能与当前任务存在明显关联，就必须先使用该技能。
不要先开始分析、读代码、写计划或写实现，再回头补技能。
</EXTREMELY-IMPORTANT>

## 会话缓存（跳过已加载）

**同一会话连续多轮时，如果缓存存在则跳过重读本 SKILL.md：**

```bash
# 如果以下文件存在且含 dev-il2cpp，说明本轮已加载，跳过本 SKILL.md
grep -q '^dev-il2cpp$' .claude/.loaded_skills_cache 2>/dev/null \
  && echo "[cache] dev-il2cpp 已加载，跳过" \
  && exit 0
```

> ⚠️ 仅在分类轮次 N ≥ 2 且域与前一轮相同时检查缓存。第 1 轮或新任务必须完整加载。
>
> 新任务开始时：`rm -f .claude/.loaded_skills_cache`

## 架构说明

`.claude/skills/` 只有本技能一个注册入口。所有子技能通过 discovery index 发现：

```
用户输入
  │
  ▼
dev-il2cpp（.claude/skills/ 唯一入口）
  │
  ├── 读取 skills/discovery/skill-index.md（已预加载）
  ├── 匹配领域 → 读取 registries/<domain>.md
  ├── 找到 skill → 读取 library/skills/<name>/SKILL.md
  └── 按 SKILL.md 指令执行
```

无需在 `.claude/skills/` 注册任何子技能，所有技能从 library 按需读取。

## 指令优先级

1. 用户明确指令
2. 项目技能规则
3. 默认系统行为

## 自动加载资源

每次对话启动时，系统通过 `promptTemplate` 配置自动加载以下资源到上下文：

- **`skills/discovery/skill-index.md`** — 技能目录索引，用于快速发现和激活相关技能
- **`wiki/INDEX.md`** — 项目 wiki 导航页，用于查找长期知识和架构文档

这些资源已自动可用，无需额外读取操作。在技能选择阶段应直接利用这些信息做判断。

## 路由协议（强制 — 从 CLAUDE.md 分类门进入）

当 CLAUDE.md 第〇条规则的分类声明命中域编号表 1-8 后，**强制**执行以下路由：

```
分类命中域 1-8
  │
  ├── 读取 skills/discovery/registries/<domain>.md
  ├── 找到该域的核心入口 skill（通常是 dev-il2cpp-core-agent）
  ├── 读取 library/skills/<name>/SKILL.md
  └── 按 SKILL.md 指令执行，不走通用回复
```

此路由在分类声明后、任何工具使用前执行。所有 il2cpp 域的任务必须先经此门。

## 核心规则

### 1. 在任何响应或行动前先选技能

- 默认的边界清晰、单会话、单目标任务，且执行前提已确认完毕：可直接实现；如果已存在正式任务目录，默认只维护 `STATUS.md`
- 新功能、行为调整、流程重构，或仍存在任何影响执行的问题（边界、authority、结构、依赖、验收口径、阶段切分、启动条件）：先用 `brainstorming`
- 已有明确跨会话或多步骤任务，且上游问题已清零并得到用户确认：用 `writing-plans` 或 `roadmap`
- 已有计划并开始推进：用 `executing-plans`
- bug、回归、异常结果：先用 `systematic-debugging`
- 完成前的结果确认：用 `verification-before-completion`

### 2. 命中测试治理或 AOT obligation 任务时，先走 `project-test-governance`

只要任务涉及以下任一内容，在进入计划或实现前必须使用 `project-test-governance`：

- `subjects/`、`tests/`、`Chaos.TestFramework`、collection file、manifest、runner、dashboard、generated codegen
- managed / native / hotupdate 测试主线或 `.NET 8` collector
- AOT / IL2CPP 新 feature 接入
- `ownerSubjectId`、`proofRequired`、`benchmarkRequired`、`hotupdateImpact` 之类的 obligation 变更
- `formalVerificationObjects`、`requiredGates`、`completed` gate 或正式验证对象选择逻辑

### 3. 工作流映射

**⛔ 强制规则 — 由上方"路由协议"触发，优先于以下所有条目匹配：**
分类命中域编号表 1-8（运行时/GC/调试/CodeGen/测试/翻译/构建/热更新）
→ 读取 `skills/discovery/registries/il2cpp.md` 找到 `dev-il2cpp-core-agent`
→ 由 Controller 判断任务域，路由到对应的 Expert Agent 或降级处理
→ **不走通用回复，不跳过此路由**

以下为通用任务的工作流参考（仅当上方强制规则未命中时使用）：

- “设计一个新功能 / 规范 / 架构”：
  先 `brainstorming` 把执行相关问题清零并拿到用户确认；之后进入 `writing-plans`，如果是多阶段主线则转 `roadmap`
- “边界清晰的小范围改动 / 单文件修复 / 单会话任务”：
  直接实现；如已存在正式任务目录，则走 `STATUS.md` 轻量维护，不强制补 `plan` / `design`
- “任务已经跨会话 / 多步骤，但还有执行前未确认的问题”：
  先回 `brainstorming`，不要直接写 `plan` 或 `roadmap`
- “继续执行现有 roadmap / plan”：
  `executing-plans`
- “修这个 bug / 回归 / benchmark 异常 / 测试阶段 dotnet 崩溃”：
  `systematic-debugging -> project-test-governance -> test-driven-development`
- “AOT 新 feature / owner subject / proof-benchmark obligation 调整”：
  `project-test-governance -> writing-plans`
- “调整 subject/test/runner/codegen 主线”：
  `project-test-governance -> writing-plans` 或 `executing-plans`

### 4. 工作流：通过 skill-index.md 发现并加载技能

当需要激活某个技能时，不依赖 Skill 工具直接调用（因为 `.claude/skills/` 只注册了本入口），改为：

```
1. 读取 skills/discovery/skill-index.md（已预加载）
2. 根据任务领域选择对应的 registry 页面
3. 从 registry 中找到目标技能，获取其 SKILL.md 路径
4. 读取 skills/library/skills/<name>/SKILL.md
5. 按 SKILL.md 中的指令执行
```

所有技能源码在 `skills/library/skills/` 中维护，`skills/discovery/registries/` 由 `generate_skill_catalog.py` 自动生成。

### 5. 不要把 `docs/dev/ACTIVE.md` 当成每一步都要拦截用户的总开关

- 小范围阅读、局部核对、低风险验证可以直接处理
- 一旦进入新的正式主线任务，再由计划/执行类技能负责更新 `docs/dev/ACTIVE.md`、`STATUS.md` 和索引
- 默认不要为了简单任务创建额外文档；只有跨会话/多步骤才引入 `plan`，只有边界或 authority 真实变化才引入 `design`

### 6. `STATUS-only` 不是永久豁免

- 边界清晰、单会话、单目标任务可以先走 `STATUS.md` 轻量维护
- 但只要出现以下任一信号，就必须在继续实现前升级：
  - 任务将跨会话继续
  - 已拆成多个可验证步骤
  - 需要稳定交接给后续执行者
  - 边界、authority、结构方案不再稳定
  - 仍存在任何影响执行的问题尚未确认
- 命中上述条件时：
  - 如果仍有执行前未确认的问题：先升级到 `brainstorming`
  - 如果问题已清零，只是执行步骤增多：升级到 `writing-plans`
  - 如果问题已清零，且已经形成多阶段或多个独立子任务：升级到 `roadmap`
  - 升级必须在原任务目录完成，并在 `STATUS.md` 记录升级原因与下一步入口

### 7. 技能激活前检查健康仪表盘

在选定要激活的技能后、实际调用前，先做一次轻量健康检查：

```
步骤:
1. 检查 skills/lifecycle/telemetry/health/ 下最新的 health-snapshot-*.json
2. 如果目标技能在 "Skills Needing Attention" 中带有 FIX / LOW-TOOL / HIGH-FALLBACK 标记：
   - 记录一条警告：[health] <skill> has active issue: <flag>
   - 仍然激活该技能（健康问题只辅助决策，不阻止使用）
3. 如果没有健康快照文件或无异常，正常激活
```

**触发器**：以下情况必须做健康检查：
- 技能被明确选中且将承担本次会话的核心角色
- 连续第二次及以上使用同一技能

**不做检查的情况**：
- 子 Agent 派发（SUBAGENT-STOP 命中时直接跳过）
- 紧急 bug 修复路径

**健康快照不存在时不报错**——系统首次部署或刚清理后没有快照是正常状态，静默跳过即可。

## 红旗

以下想法说明你在绕过流程：

- “我先看几眼代码再决定要不要用技能”
- “这只是测试目录的小改动，不算测试治理”
- “先把 benchmark 跑通，之后再补自动化测试”
- “先把 emitter 拼出来，模板后面再说”
- “formal verification object 后面再补，现在先归档 completed”
- “先建 roadmap，剩下的 open questions 边做边确认”

## 关联技能

- 上游：无
- 协作：`project-test-governance`、`test-driven-development`、`project-wiki-maintenance`
- 下游：`brainstorming`、`roadmap`、`writing-plans`、`executing-plans`、`systematic-debugging`

## 加载确认（强制）

读取本 SKILL.md 后，**立即执行以下操作**以完成加载：

1. 确认 `.claude/.classified` 已存在（由分类声明 echo 创建）
2. 确认其内容含单行 `loaded_expert` 标记，格式为：
   ```
   格式示例: loaded_expert:dev-il2cpp→dev-il2cpp-codegen-expert
   ```
   其中 `dev-il2cpp` 必须在 `→` 分隔的首位
3. 写入会话缓存（跳过后续轮次重复加载）：
   ```bash
   echo "dev-il2cpp" >> .claude/.loaded_skills_cache
   ```
4. 按上方 **路由协议** 找到域名对应 Expert，继续读取其 SKILL.md
5. 读取完所有 Expert 的 SKILL.md 后，确认 `.claude/.classified` 中已写入完整标记：
   ```bash
   echo "本轮任务涉及 CodeGen(4) ，fix 操作，第 1 轮 → 加载 dev-il2cpp → dev-il2cpp-codegen-expert" > .claude/.classified
   echo 'loaded_expert:dev-il2cpp→dev-il2cpp-codegen-expert' >> .claude/.classified
   ```
6. 将对应 Expert 也加入会话缓存：
   ```bash
   echo "dev-il2cpp-codegen-expert" >> .claude/.loaded_skills_cache
   ```

> ⚠️ 不执行步骤 1-6 会导致 hook 阻断后续工具调用。`dev-il2cpp` 必须出现在 `loaded_expert:` 行的首位，且 `loaded_skills_cache` 必须写入对应条目。
