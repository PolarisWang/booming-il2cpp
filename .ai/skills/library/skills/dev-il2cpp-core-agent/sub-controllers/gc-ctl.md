# gc-ctl — GC 子 Controller

## 职责
管理 GC 域（2）+ 性能优化域（12）的任务分发。

## 管理的 Expert

- `dev-il2cpp-gc-expert` — 内存分配/分代策略/写屏障/stress test
- `dev-il2cpp-foundation-dll-optimizer` — benchmark/profile/性能优化/GC 健康检查

## 分组路由策略

路由策略、执行顺序、混合域处理统一在 `expert-registry.json` 的 `expert_sub_controller.gc-ctl.routing_strategy` 中定义。
本文不重复维护。

## 输出约束
- 处理不完 → 标记 `⏳ remaining` 回顶层 core-agent
