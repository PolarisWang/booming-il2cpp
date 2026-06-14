# runtime-ctl — 运行时子 Controller

## 职责
管理运行时域（1）+ 调试域（3）的任务分发。

## 管理的 Expert

- `dev-il2cpp-runtime-expert` — runtime-core/interpreter/VTable/bootstrap
- `dev-il2cpp-jit-expert` — JIT 编译/寄存器分配/SEH/tier promotion
- `dev-il2cpp-debug-expert` — crash/segfault/异常行为追踪

## 分组路由策略

路由策略、执行顺序、混合域处理统一在 `expert-registry.json` 的 `expert_sub_controller.runtime-ctl.routing_strategy` 中定义。
本文不重复维护。

## 输出约束
- 处理不完 → 标记 `⏳ remaining: [子任务 ID + 原因]` 回顶层 core-agent
- 修复前有调试 → 交接时必须附诊断上下文
