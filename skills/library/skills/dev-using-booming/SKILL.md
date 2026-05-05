---
name: dev-using-booming
description: 在开始任何对话时使用，负责先判断应该激活哪些技能，再进入设计、计划、实现或验证
---

<SUBAGENT-STOP>
如果你是作为子 Agent 被派发来执行一个已经明确边界的子任务，跳过本技能。
</SUBAGENT-STOP>

<EXTREMELY-IMPORTANT>
只要有任何一个技能与当前任务存在明显关联，就必须先使用该技能。
不要先开始分析、读代码、写计划或写实现，再回头补技能。
</EXTREMELY-IMPORTANT>

## 指令优先级

1. 用户明确指令
2. 项目技能规则
3. 默认系统行为

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
  - 如果仍有执行前未确认的问题：先升级到 `brainstorming`
  - 如果问题已清零，只是执行步骤增多：升级到 `writing-plans`
  - 如果问题已清零，且已经形成多阶段或多个独立子任务：升级到 `roadmap`
  - 升级必须在原任务目录完成，并在 `STATUS.md` 记录升级原因与下一步入口

### 6. Foundation DLL 验证工作路由

当任务涉及 foundation DLL 能力族验证或测试时，根据具体场景选择对应技能：

| 场景 | 入口技能 | 说明 |
|------|---------|------|
| 验证一个或多个 family 的全部三闸门 | `dev:foundation-dll-verification-pipeline` | 主管线：codegen → integrity → three-gate → aggregate |
| 验证前检查 dashboard 数据一致性 | `dev:foundation-dll-verify-data-integrity` | claims/ledger/coverage 一致性校验 |
| 已有 batch pipeline 产出，跑单 family 三闸门 | `dev:foundation-dll-family-verification` | Fact → Benchmark → HotUpdate |
| 验证后聚合结果 + 刷新 dashboard | `dev:foundation-dll-verify-aggregate` | 汇总 + 回归检测 + dashboard 刷新 |
| 接入一个全新的 capability family | `dev:foundation-dll-onboard-family` | ledger → Phase 1/2 → codegen → verification |

**触发条件**：只要任务涉及以下任一内容，必须先使用上述技能之一：

- `verification/foundation-dll/` 下的任何能力族
- 运行 `batch_native_aot_runner.py`、`batch_hotupdate_runner.py` 等 batch 管线
- 对 capability family 执行 Fact / Benchmark / HotUpdate 验证
- 检查或刷新 foundation-dll dashboard
- 在 `capability-family-ledger.json` 中新增 family
- 修复 foundation-dll 验证数据不一致（如 claims 为空、coverage 缺失）

**工作流映射**：
- "验证这个 family 能否通过"：`verification-pipeline`
- "dashboard 显示 0/13，查一下为什么"：`verify-data-integrity`
- "跑一下这个 family 的 benchmark"：`family-verification`（仅 Step 2）
- "刷新 dashboard"：`verify-aggregate`（含 dashboard 刷新）
- "接一个新 family 到验证体系"：`onboard-family`

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
- Foundation DLL 验证系：`verification-pipeline`、`verify-data-integrity`、`family-verification`、`verify-aggregate`、`onboard-family`
- 下游：`brainstorming`、`roadmap`、`writing-plans`、`executing-plans`、`systematic-debugging`
