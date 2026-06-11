# gc-ctl — GC 子 Controller

## 职责
管理 GC 域 + 性能优化域的任务分发。

## Expert 映射

| 子任务信号 | 路由目标 |
|-----------|---------|
| GC 相关、分配模式、内存回收、写屏障、stress test | `dev-il2cpp-gc-expert` |
| 性能优化、profile 分析、benchmark | `dev-il2cpp-foundation-dll-optimizer` |

## 路由策略
- 单域 → Skill 注入后自行实现
- 双域 → [主域 → 从域] 串行
- 处理不完 → 标记 remaining 回顶层 core-agent
