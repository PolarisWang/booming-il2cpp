# Hotupdate 性能优化 — Roadmap

## 阶段划分

按优先级从高到低、依赖关系排列：

### Phase 1: 低成本高收益（Tier 加速 + 预热 + setup 精简）

| # | 子任务 | 工作量 | 涉及文件 | 预期收益 |
|---|--------|--------|---------|---------|
| 1.1 | **Tier 升级加速** | 低 | `entry_direct.cpp` 阈值常量 | 2-10x 冷启动 |
| 1.2 | **Patch 应用时主动预热** | 低 | `patch_loader.cpp`, `patch_method_lower.cpp` | 消除首次卡顿峰值 |
| 1.3 | **InterpreterEntryDirect setup 精简** | 中 | `entry_direct.cpp` | ~50% setup 缩减 |

### Phase 2: 核心架构改造（桥接函数 + 寄存器 IR）

| # | 子任务 | 工作量 | 涉及文件 | 预期收益 |
|---|--------|--------|---------|---------|
| 2.1 | **寄存器 IR 替代栈 IR** | 中-高 | `fast_dispatch.cpp`, `entry_direct.cpp`, `patch_method_lower.cpp` | 2-4x 指令执行 |
| 2.2 | **预生成桥接函数消除 method_invoke** | 高 | codegen C# + `aot_direct_dispatch.cpp`, `precache_dispatch_raw.cpp` | 2-10x 解释器性能 |

### Phase 3: 功能增强（DHE）

| # | 子任务 | 工作量 | 涉及文件 | 预期收益 |
|---|--------|--------|---------|---------|
| 3.1 | **DHE 差分混合执行** | 中 | emit-patch-data + `codegen_bridge.h` | 未变方法 1x |

## 验收方法

最终一次性验证：
1. `boxing-unboxing-casts` family 跑完整 pipeline（`--mode strict --native-config profile`）
2. 确认 all benchmark timing > 0
3. 确认 hotupdate `semantic_changed_count > 0` + overhead ≤ 100%
4. 对比优化前后的 hotupdate benchmark timing

## 阶段入口

- Phase 1 → `writing-plans` 落地
- Phase 2 → `writing-plans` 落地
- Phase 3 → `writing-plans` 落地
