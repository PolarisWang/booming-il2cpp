# Hotupdate 性能优化 — 设计方案

## 风险评估

| 风险 | 触发条件 | 后果 | 缓解方案 |
|------|---------|------|---------|
| **P0 桥接函数与既有 dispatch 不兼容** | 修改 InterpreterDispatchRaw 导致未预期行为 | 热更新后方法执行错误 | 保留 method_invoke 作为 fallback，分阶段上线 |
| **寄存器 IR 改造破坏 FastExecute** | FastExecute 同时支持栈 IR 和寄存器 IR | 解释器崩溃 | 寄存器 IR 走独立路径，不影响现有栈 IR 路径 |
| **Tier 阈值降低导致过早 JIT** | JIT codegen 未准备好 | 冷启动变慢 | JIT 阈值保持原值，只降低 T0→T1→T2 阈值 |
| **预热逻辑增加 patch 应用耗时** | ApplyPatchFromMemory 触发大量 JSON 解析 | patch 应用从 μs 级变为 ms 级 | 预热放在后台线程（patch 应用后 defer） |
| **DHE diff 准确性** | IL diff 误判导致 keep-native 标记错误 | 应走解释器的方法走了 AOT | DHE 默认关闭，通过 flag 启用 |
| **最终一次性验证失败** | 多个子任务叠加导致根因定位困难 | 需要回退到单步验证 | 每完成一个 P-level 做中间检查 |

## 三优先级权衡

| 方案 | 性能 (P1) | 架构 (P2) | HotUpdate (P3) | 推荐 |
|------|----------|----------|----------------|------|
| **P0 桥接函数** | ★★★ 消除 1500-2200ns method_invoke | ★★ 新增桥接函数生成 + 运行时查找 | ★★★ 热更方法调用 AOT 方法变快 | **实施** |
| **P1 寄存器 IR** | ★★★ 指令数减 2-4x，执行更快 | ★★★ 复用现有 reg_ir_data 路径 | ★★★ 解释器更快 | **实施** |
| **P1 Tier 加速** | ★★ 冷启动 2-10x 提升 | ★ 仅改阈值常量 | ★ 不影响热更逻辑 | **实施** |
| **P2 预热** | ★★★ 消除首次调用卡顿 | ★★ patch 应用时 defer 触发 | ★★ 热更后首次调用不再慢 | **实施** |
| **P2 DHE** | ★ 仅 5%+ 场景有收益 | ★★ 需 emit-patch-data diff | ★ 技术原型的补充 | **实施**（默认关闭） |
| **P3 setup 精简** | ★ 仅 setup 减 50%（绝对 ~50ns） | ★ 代码合并 | ★ 无影响 | **实施** |

## 执行边界拍板

- **范围**：P0-P3 全部实施
- **顺序**：按优先级从高到低（P0→P1→P2→P3）
- **分支**：直接在 main 上开发
- **验证**：最终一次性验证

## 架构影响评估

| 变更 | 影响模块 | 是否需要新文件 |
|------|---------|--------------|
| P0 桥接函数 | codegen (C#) + runtime dispatch (C++) | 新增 `bridge_function_generator.cpp`（或嵌入现有 codegen） |
| P1 寄存器 IR | fast_dispatch.cpp + entry_direct.cpp | 修改现有文件，无新文件 |
| P1 Tier 加速 | entry_direct.cpp | 修改常量，无新文件 |
| P2 预热 | patch_loader.cpp + patch_method_lower.cpp | 修改现有文件，无新文件 |
| P2 DHE | emit-patch-data 流水线 + codegen_bridge.h | 修改现有文件，无新文件 |
| P3 setup 精简 | entry_direct.cpp | 修改现有文件，无新文件 |
