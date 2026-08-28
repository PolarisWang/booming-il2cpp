---
name: dev-il2cpp
description: 项目唯一入口 skill — 对话启动时加载，通过 skill-index.md 发现领域并路由到对应技能
---

<SUBAGENT-STOP>
如果你是作为子 Agent 被派发来执行一个已经明确边界的子任务：
1. 仍然执行下方 **加载确认**（写入 .classified 标记文件）
2. 读取 **路由协议** 找到 Expert → 读 Expert 的 SKILL.md
3. 跳过本页其余内容（核心规则、红旗、关联技能）
</SUBAGENT-STOP>

<EXTREMELY-IMPORTANT>
只要有任何一个技能与当前任务存在明显关联，就必须先使用该技能。
不要先开始分析、读代码、写计划或写实现，再回头补技能。
</EXTREMELY-IMPORTANT>

## 会话缓存（跳过已加载）

**同一会话连续多轮时，如果缓存存在则跳过重读本 SKILL.md：**

在执行任何工具前，先检查缓存文件：
```bash
grep -q '^dev-il2cpp$' .claude/.loaded_skills_cache
```
- 如果返回 0（命中）：本轮已加载过 dev-il2cpp，**跳到路由协议直接执行**
- 如果返回 1（未命中）：继续读本 SKILL.md

> ⚠️ 仅在分类轮次 N ≥ 2 且域与前一轮相同时检查缓存。第 1 轮或新任务必须完整加载。
>
> 新任务开始时：`rm -f .claude/.loaded_skills_cache`

## 架构说明

`.claude/skills/` 只有本技能一个注册入口。所有子技能通过 discovery index + core-agent dispatcher 发现：

```
用户输入
  │
  ▼
dev-il2cpp（.claude/skills/ 唯一入口）
  │
  ├── 读取 .ai/skills/discovery/expert-registry.json（已预加载）
  ├── 路由协议: 读取 .ai/skills/discovery/registries/il2cpp.md
  ├── 找到 dev-il2cpp-core-agent → 读取其 SKILL.md
  ├── core-agent 读取 expert-registry.json 获取 Expert 名
  └── core-agent 读 Expert SKILL.md → 自行实现或 Workflow 委托
```

无需在 `.claude/skills/` 注册任何子技能。Expert 路由通过 `expert-registry.json` 集中管理，所有路径使用 `.ai/skills/` 前缀。

## 指令优先级

1. 用户明确指令
2. 项目技能规则
3. 默认系统行为

## 自动加载资源

每次对话启动时，系统通过 `promptTemplate` 配置自动加载以下资源到上下文：

- **`.ai/skills/discovery/skill-index.md`** — 技能目录索引，用于快速发现和激活相关技能
- **`.ai/skills/discovery/expert-registry.json`** — 领域→Expert 映射（路由决策核心数据源）
- **`wiki/INDEX.md`** — 项目 wiki 导航页，用于查找长期知识和架构文档

这些资源已自动可用，无需额外读取操作。在技能选择阶段应直接利用这些信息做判断。

## 加载确认（强制 — 第 0 步，读路由协议前先执行）

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
4. 按下方 **路由协议** 找到域名对应 Expert，继续读取其 SKILL.md
5. 读取完所有 Expert 的 SKILL.md 后，更新 loaded_expert 行追加 Expert 名：
   ```bash
   # 将 loaded_expert:dev-il2cpp 更新为 loaded_expert:dev-il2cpp→<expert_name>
   sed -i "s/^loaded_expert:dev-il2cpp$/loaded_expert:dev-il2cpp→dev-il2cpp-codegen-expert/" .claude/.classified
   ```
   > 注意：分类声明已在 CLAUDE.md 步骤 0 写入，本步骤**只追加 Expert 名**，不覆盖分类文本。
6. 将对应 Expert 也加入会话缓存：
   ```bash
   echo "dev-il2cpp-codegen-expert" >> .claude/.loaded_skills_cache
   ```

> ⚠️ 不执行步骤 1-6 会导致 hook 阻断后续工具调用。`dev-il2cpp` 必须出现在 `loaded_expert:` 行的首位，且 `loaded_skills_cache` 必须写入对应条目。

## 路由协议（强制 — 从 CLAUDE.md 分类门进入）

当 CLAUDE.md 第〇条规则的分类声明命中域编号表 1-8 后，**强制**执行以下路由：

```
分类命中域 1-8
  │
  ├── (Hot-first) 检查 .claude/.hot_skills：
  │      目标 Expert ∈ hot → 直接读 .ai/skills/library/skills/<expert>/SKILL.md（1 步，跳过发现链）
  │
  ├── 未命中 hot → 读取 .ai/skills/discovery/registries/il2cpp.md 找到 dev-il2cpp-core-agent
  ├── 读取 .ai/skills/discovery/expert-registry.json 获取域名对应 Expert 名
  ├── 读取 .ai/skills/library/skills/<expert_name>/SKILL.md
  └── 按 SKILL.md 指令执行，不走通用回复
```

> 🔴 Hot-first 为强制：热列表内 Expert 不得走完整发现链。`.claude/.hot_skills` 是唯一权威源（`.claude/dot-claude/.hot_skills` 已废弃）。

> 也可使用确定性查询工具替代手动读 JSON：
> ```bash
> python tests/e2e/verification/tools/expert_lookup.py --domain <N>
> python tests/e2e/verification/tools/expert_lookup.py --keyword "<描述关键词>"
> ```

此路由在分类声明后、任何工具使用前执行。所有 il2cpp 域的任务必须先经此门。

**下一步**: 读取 `dev-il2cpp-core-agent` 的 SKILL.md，由其 Dispatcher 完成 Expert 分派和执行。

## 核心规则

### 1. 在任何响应或行动前先选技能（含复杂度分级）

**先做复杂度分级（与 core-agent 阶段 1.5 闸门一致），禁止默认直接实现：**

- **DIRECT（可直接实现）** —— 必须**同时满足**：① 单文件/单域改动、无跨域 stub/符号依赖 ② 用户已给出明确规格(非"改进/看看"这类开放需求) ③ 无跨子任务依赖 ④ 预计改动量小(局部修复/配置/文档)。缺任一 → 不得归 DIRECT。
- **BRAINSTORM（先用 dev-brainstorm）** —— 新功能、行为调整、流程重构，或仍存在影响执行的问题（边界、authority、结构、依赖、验收口径、阶段切分、启动条件）。清零前不直接实现。
- **PLAN（先 dev-writing-plans / dev-roadmap）** —— 上游问题已清零 + 跨会话或多步骤 + 需稳定交接。多阶段主线用 dev-roadmap。
- **EXECUTING（dev-executing-plans）** —— 已有计划并开始推进。
- bug/回归/异常 → `systematic-debugging`；完成前确认 → `verification-before-completion`。

> **注意**：`direct` 是受限档位而非默认。拿不准归属时**升档**到 brainstorm, 不擅自按 direct 实现。

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
→ 读取 `.ai/skills/discovery/registries/il2cpp.md` 找到 `dev-il2cpp-core-agent`
→ 由 Controller 判断任务域，路由到对应的 Expert Agent 或降级处理
→ **不走通用回复，不跳过此路由**

以下为通用任务的工作流参考（仅当上方强制规则未命中时使用）：

- “设计一个新功能 / 规范 / 架构”：
  先 `dev-brainstorm` 把执行相关问题清零并拿到用户确认；之后进入 `writing-plans`，如果是多阶段主线则转 `roadmap`
- “边界清晰的小范围改动 / 单文件修复 / 单会话任务”：
  直接实现；如已存在正式任务目录，则走 `STATUS.md` 轻量维护，不强制补 `plan` / `design`
- “任务已经跨会话 / 多步骤，但还有执行前未确认的问题”：
  先回 `dev-brainstorm`，不要直接写 `plan` 或 `roadmap`
- “继续执行现有 roadmap / plan”：
  `executing-plans`
- “修这个 bug / 回归 / benchmark 异常 / 测试阶段 dotnet 崩溃”：
  `systematic-debugging -> project-test-governance -> test-driven-development`
- “AOT 新 feature / owner subject / proof-benchmark obligation 调整”：
  `project-test-governance -> writing-plans`
- “调整 subject/test/runner/codegen 主线”：
  `project-test-governance -> writing-plans` 或 `executing-plans`

### 4. 不要把 `docs/dev/ACTIVE.md` 当成每一步都要拦截用户的总开关

- 小范围阅读、局部核对、低风险验证可以直接处理
- 一旦进入新的正式主线任务，再由计划/执行类技能负责更新 `docs/dev/ACTIVE.md`、`STATUS.md` 和索引
- 默认不要为了简单任务创建额外文档；只有跨会话/多步骤才引入 `plan`，只有边界或 authority 真实变化才引入 `design`

### 5. `STATUS-only` 不是永久豁免

- 边界清晰、单会话、单目标任务可以先走 `STATUS.md` 轻量维护
- 但只要出现以下任一信号，就必须在继续实现前升级：
  - 任务将跨会话继续
  - 已拆成多个可验证步骤
  - 需要稳定交接给后续执行者
  - 边界、authority、结构方案不再稳定
  - 仍存在任何影响执行的问题尚未确认
- 命中上述条件时：
  - 如果仍有执行前未确认的问题：先升级到 `dev-brainstorm`
  - 如果问题已清零，只是执行步骤增多：升级到 `writing-plans`
  - 如果问题已清零，且已经形成多阶段或多个独立子任务：升级到 `roadmap`
  - 升级必须在原任务目录完成，并在 `STATUS.md` 记录升级原因与下一步入口

### 6. 结论偏好（decisiveness-over-options）

除用户明确要求"给几个方案 / 对比 / 权衡"外，**默认给一个带理由的推荐实现**，直接可推进，而不是罗列多个方案等用户拍板。

- 只在存在**多重等权重权衡、且选错会显著浪费返工**时，才给出备选并用 AskUserQuestion 让用户定。
- 禁止把"给 N 个方案 + 分析"当默认姿态——那是把决策推回给用户。
- 给方案后若用户没明确选，主动给**主推荐**（哪个、为什么、下一步），而不是结束在"你选一个"。

### 7. 技能激活前检查健康仪表盘

在选定要激活的技能后、实际调用前，先做一次轻量健康检查：

```
步骤:
1. 检查 .ai/skills/lifecycle/telemetry/health/ 下最新的 health-snapshot-*.json
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
**快照文件损坏（JSON 解析失败）时也静默跳过**——视为无快照，不影响技能激活。

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
- 协作：`project-test-governance`、`test-driven-development`、`project-wiki-maintenance`、`verification-before-completion`
- 下游：`dev-brainstorm`、`roadmap`、`writing-plans`、`executing-plans`、`systematic-debugging`、`dev-il2cpp-core-agent`

## 任务结束清理

任务完成或切换时，清理会话标记文件：

```bash
rm -f .claude/.classified .claude/.loaded_skills_cache
```

> 新任务开始时 CLAUDE.md 执行协议步骤 3 也有此要求。两个文件残留会导致下一任务的 hook 或缓存逻辑误判。
