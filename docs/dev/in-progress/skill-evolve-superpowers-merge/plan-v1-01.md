# superpowers 精华最小移植+三路径路由 实现计划

> **面向执行 Agent：** 本计划为单一进化任务，逐阶段手动更新技能文本。步骤用 `- [ ]` 跟踪。权威增补文本存于用户计划 `composed-zooming-squid.md`，本文件为执行摘要与本地收尾指针。

**目标：** 把 superpowers 的 6 项净增量精华（三路径/A、Interfaces/B、ledger/C、Rulings/D、修复上限+模型分级/E、Batch/F）最小移植进项目技能库，并将三路径分类映射进 expert 域路由。

**架构：** 每个技能文件以"追加增量小节"为主，不破坏既有流程骨架与红线；路由部分升级 dev-il2cpp 二元分类为显式三路径 + 在 expert-registry.json 加 task_classification 块。

**技术栈：** 中文 Markdown SKILL.md + JSON 路由数据。

**架构审核模式：** normal（技能文本改动，非运行时代码）

**设计文档：** 已批准方案 `C:\Users\haochuan.wang\.claude\plans\composed-zooming-squid.md`

**计划来源：** direct-plan（方案1+2 获准）

**预期知识沉淀：** `wiki/` 待定；本次改动完成后按需更新 skill-index 描述。

---

## 阶段清单

- [ ] 阶段0：任务目录 + STATUS.md + 归档 ✅（已完成，本计划文件存在即完成）
- [ ] 阶段1：dev-brainstorm 加「三路径分类与单向棘轮」小节（反模式表之后、轻量级模式之前的增量）
- [ ] 阶段2：dev-writing-plans 加「任务的接口契约（Interfaces 块）」小节（计划要求内）
- [ ] 阶段3：dev-executing-plans 加「恢复 ledger」+「Rulings 裁决默认」两个小节
- [ ] 阶段4：dev-subagent-driven-development 加「Batch 同类小任务合并派发」+「修复循环上限与模型分级」
- [ ] 阶段5：dev-il2cpp 工作流映射升级 + expert-registry task_classification 块 + core-agent 分类判定
- [ ] 验证：verify_skill_pipeline.py + health_engine.py check
- [ ] 收尾：结构告警→测试通过→归档 completed→提交

## 提交策略

单次整理性强提交，subject 前缀 `docs(skill-evolve)`。

commit message 规则要求：因为含方法论移植，标注 `root_cause` / `fix_strategy` / `regression_check`（AI Agent Bug 三规则）。本任务是移植非 bug 修复，仍在 commit body 记录策略与回归核对。
