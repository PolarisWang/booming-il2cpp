# AOT Skill Flow And Token Optimization 实现计划 v1.01

> **面向执行 Agent：** 必须使用 dev:subagent-driven-development（如果可用子 Agent）或 dev:executing-plans 来执行本计划。步骤使用复选框（`- [ ]`）语法跟踪。

**目标：** 把当前 skill 主线优化成一条 obligation-driven 的 AOT 工作流，并降低高频热路径 token 成本。

**架构：** 先冻结 authority，再改 control skill。高频 skill 只保留 routing/gate，把 owner subject、proof/native/hotupdate/benchmark 的深规则集中到 architecture 与 `wiki/06-测试验证/`。`completed` 前统一强制 formal verification gate。

**技术栈：** Markdown skill docs、project wiki、architecture docs、文本校验命令、scenario-based skill validation

**设计文档：** `docs/dev/in-progress/20260417-04-aot-skill-flow-and-token-optimization-plan/design-v1-01.md`

**预期知识沉淀：** `wiki/02-Skill体系/`、`wiki/06-测试验证/`、`docs/architecture/managed-native-hotupdate-test-pipeline.md`

**收尾约束：** 执行完成后必须进入“审视架构合理性 -> 测试通过 -> 归档 completed -> 合并&提交”固定链路。

---

## 涉及文件

- `.codex/skills/dev-using-booming/SKILL.md`
  - 收紧 routing，避免重复承载 AOT 深规则
- `.codex/skills/dev-project-test-governance/SKILL.md`
  - 作为治理入口，显式绑定 authority 与 AOT onboarding 入口
- `.codex/skills/dev-writing-plans/SKILL.md`
  - 增加 capability / owner / obligation 计划字段
- `.codex/skills/dev-executing-plans/SKILL.md`
  - 把 AOT formal verification chain 绑定到 `completed` gate
- `.codex/skills/dev-subagent-driven-development/SKILL.md`
  - 与顺序执行主线对齐相同的 formal verification gate
- `.codex/skills/dev-verification-before-completion/SKILL.md`
  - 明确 AOT onboarding 时的对象选择与 escalation 规则
- `.codex/skills/dev-test-driven-development/SKILL.md`
  - 只保留与 failure-first discipline 相关的仓库绑定，去掉可由 authority 承担的重复说明
- `docs/architecture/managed-native-hotupdate-test-pipeline.md`
  - 作为 authority，固定 AOT feature onboarding 的正式链路入口
- `wiki/06-测试验证/INDEX.md`
  - 保持正式验证入口与对象导航一致
- `wiki/06-测试验证/AOT新Feature接入自测规范.md`
  - 作为 owner/proof/native/hotupdate/benchmark obligation 深规则真源
- `wiki/02-Skill体系/01-主线工作流/*.md`
  - 同步主线技能镜像
- `wiki/02-Skill体系/04-质量保障/*.md`
  - 同步质量保障 skill 镜像
- `wiki/02-Skill体系/skill-registry.md`
  - 同步 skill 职责边界

## 计划清单

- [ ] Task 1: 冻结 authority matrix，并补齐 `project-test-governance` 与 AOT onboarding 规则之间的真源绑定
  - 明确 `docs/architecture/managed-native-hotupdate-test-pipeline.md` 与 `wiki/06-测试验证/AOT新Feature接入自测规范.md` 的职责分层
  - 让 `project-test-governance` skill 正文补齐：
    - `subject.features.json` 是 owner/proof/benchmark obligation authority
    - canonical proof / benchmark / host correctness 禁止 stdout 判定
    - AOT onboarding 必须回到 `wiki/06-测试验证/AOT新Feature接入自测规范.md`
  - 删除与 authority 冲突或已经过时的旧描述

- [ ] Task 2: 重建 planning intake contract，让 plan 显式冻结 obligation
  - 修改 `dev-writing-plans` 模板，新增必填字段：
    - `capabilityFamily`
    - `capabilityItem`
    - `ownerSubjectId`
    - `proofRequired`
    - `benchmarkRequired`
    - `hotupdateImpact`
    - `formalVerificationObjects`
  - 如有必要，同步 `brainstorming` 的 design quality 提示，避免 plan 上游漏掉 capability/owner 判断
  - 明确 AOT onboarding 类计划不得只写“跑测试”，必须写清 proof/native/hotupdate/benchmark obligation

- [ ] Task 3: 把 formal verification gate 接入 `completed` 判定
  - 修改 `dev-executing-plans` 与 `dev-subagent-driven-development`
  - 对命中 AOT / IL2CPP 主线的任务，`completed` 前必须显式确认：
    - collector / registry / workspace gate
    - owner managed proof
    - owner native proof
    - hotupdate proof（按需）
    - benchmark（按需）
    - affected regressions
  - 让执行主线显式调用或消费 `dev-verification-before-completion` 的正式对象选择规则，而不是各自重复定义

- [ ] Task 4: 收紧 `verification-before-completion`，让它成为 formal object selector
  - 保留 registry / `canonicalCommand` / escalation 规则
  - 补齐与 AOT onboarding 规范的直接映射：
    - 当计划写明 `formalVerificationObjects` 时，优先执行这些正式对象
    - 当对象缺失时，先补对象或 case，再允许完成
  - 避免它继续承载可以放回 authority 文档的长篇 AOT 解释

- [ ] Task 5: 做高频热路径 token slimming
  - 复核 `using-booming`、`project-test-governance`、`executing-plans`、`verification-before-completion`、`systematic-debugging`、`test-driven-development` 的正文
  - 目标是把重复规则压缩成“单一 authority + 多点引用”
  - 重点去重：
    - `dotnet` 编译崩溃闸门
    - benchmark 不替代 correctness
    - `completed` 前必须有正式验证证据
    - AOT onboarding 的 owner/proof/native/hotupdate/benchmark 顺序
  - 若去重后热路径仍然偏重，再评估是否新增一个轻量 `aot-feature-intake` skill 作为高频入口

- [ ] Task 6: 做文本回归与 scenario-based skill 验证
  - 文本一致性检查：
    - skill 正文与 wiki 镜像一致
    - skill 正文与 `docs/architecture` / `wiki/06-测试验证/` 不冲突
  - token 预算复算：
    - 对比改造前后的高频热路径组合体积
  - scenario-based 验证至少覆盖：
    - “新增一个 AOT feature，需要 owner subject、managed/native/hotupdate/proof/benchmark obligation”
    - “benchmark 异常排查，需要 routing 到 debugging + governance + TDD”
    - “任务准备完成，试图在 formal verification object 缺失时归档 completed”
  - 将验证结论写回任务目录与 wiki 镜像

## 预期验证

- `git diff --check -- .codex/skills docs/architecture wiki`
- 对目标文件执行 `Select-String`，确认以下关键词和链路一致：
  - `ownerSubjectId`
  - `proofRequired`
  - `benchmarkRequired`
  - `hotupdateImpact`
  - `collector -> registry -> workspace`
  - `managed proof`
  - `native proof`
  - `hotupdate proof`
  - `canonicalCommand`
  - `required-before-completion`
- 复算高频热路径体积，确认常见 AOT 计划执行链路的总字符数明显下降
- 记录至少三组 skill scenario baseline/green 结果，证明 routing、gate 与 completed 判定没有退化
