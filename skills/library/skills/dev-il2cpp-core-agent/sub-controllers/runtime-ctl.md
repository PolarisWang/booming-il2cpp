# runtime-ctl — 运行时子 Controller

## 职责
管理运行时域 + 调试域的任务分发。

## Expert 映射

| 子任务信号 | 路由目标 |
|-----------|---------|
| runtime-core、interpreter、VTable、bootstrap、method_table、线程状态 | `dev-il2cpp-runtime-expert` |
| crash、segfault、test failure、异常行为 | `dev-il2cpp-debug-expert` |

## 路由策略
- 单域 → Skill 注入后自行实现
- 双域 → [主域 → 从域] 串行
- 处理不完 → 标记 remaining 回顶层 core-agent
