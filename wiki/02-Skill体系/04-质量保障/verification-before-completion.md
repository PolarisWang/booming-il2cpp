# verification-before-completion

> 项目中的完成前验证 skill，负责在宣称完成前确认“已执行什么验证、为什么这些验证足够、失败时去哪里定位”。

## 基本信息

- 分层：质量保障
- 项目职责：在标记 `completed` 前确认验证和 wiki 更新已完成
- 实现路径：`.codex/skills/dev-verification-before-completion/SKILL.md`
- 状态：`project`

## skill 应读取什么

- 项目级测试知识只认 [`../../06-测试验证/INDEX.md`](../../06-测试验证/INDEX.md) 这一正式入口。
- 运行时对象与 selector 信息优先读取 registry 快照或 `run test registry list --json`。
- 当 registry 里给出 `docRefs` 时，再回退到 `wiki/06-测试验证/` 对应页面读取说明。
- `docs/dev/**` 只记录任务过程，不是长期有效的测试说明来源。

## skill 应怎么判断“可以完成”

- `completed` 必须同时满足：任务完成、验证通过、应写 wiki 已更新。
- 没有验证证据时，不能归档为 `completed`。
- 如果已有正式对象，应优先执行正式对象对应的 `run test suite/module/system/pipeline ...`。
- 如果当前改动引入了新的重要验证要求，不能只在本次开发里手工跑一遍后结束。

## 重要验证升格规则

- 修复真实 bug 的回归验证，必须沉淀为正式 case。
- 被 skill 认定为 `required-before-completion` 的验证，必须沉淀为正式 case、module、system 或 pipeline 对象。
- 被 module/system/pipeline 引用的验证，必须沉淀为正式对象。

## skill 的默认动作

1. 先查 registry，确认当前改动可复用的 suite/module/system/pipeline 对象。
2. 若 registry 中已有合适对象，直接使用对应 `canonicalCommand`。
3. 若只有 wiki 说明没有正式对象，按 [`../../06-测试验证/新增测试接入规范.md`](../../06-测试验证/新增测试接入规范.md) 补齐正式对象或 case。
4. 执行完成后，结果中至少要能回指 `summaryPath`、`eventsPath`、`consolePath` 等定位信息。

## 最近变更

- `2026-04-04`：补充与 `wiki/06-测试验证/`、registry 快照、正式对象升格规则之间的绑定关系。
- `2026-04-02`：与 execution 自动 completed 规则绑定。
