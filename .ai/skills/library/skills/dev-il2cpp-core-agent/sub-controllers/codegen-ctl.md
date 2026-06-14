# codegen-ctl — CodeGen 子 Controller

## 职责
管理 CodeGen 域 + 翻译域 + 构建域 + 测试域 + 热更新域的任务分发。

## Expert 映射

Expert 的完整关键词映射和分组定义在 `skills/discovery/expert-registry.json` 中：

- `expert_keywords` — 任务描述关键词 → Expert 名
- `expert_sub_controller.codegen-ctl` — 本控制器负责的 Expert 列表

当前本控制器管理的 Expert 见 `expert-registry.json` 的 `expert_sub_controller.codegen-ctl` 字段。

## 执行顺序约束
**必须先派发 translation-expert，再派发 codegen-expert**（两者都涉及 Planner/Emission 文件）。

## 路由策略
- 单域 → Skill 注入后自行实现
- 双域 → [主域 → 从域] 串行
- 处理不完 → 标记 remaining 回顶层 core-agent
