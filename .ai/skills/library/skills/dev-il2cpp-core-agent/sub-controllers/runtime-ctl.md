# runtime-ctl — 运行时子 Controller

## 职责
管理运行时域 + 调试域的任务分发。

## Expert 映射

Expert 的完整关键词映射和分组定义在 `skills/discovery/expert-registry.json` 中：

- `expert_keywords` — 任务描述关键词 → Expert 名
- `expert_sub_controller.runtime-ctl` — 本控制器负责的 Expert 列表

当前本控制器管理的 Expert 见 `expert-registry.json` 的 `expert_sub_controller.runtime-ctl` 字段。

## 路由策略
- 单域 → Skill 注入后自行实现
- 双域 → [主域 → 从域] 串行
- 处理不完 → 标记 remaining 回顶层 core-agent
