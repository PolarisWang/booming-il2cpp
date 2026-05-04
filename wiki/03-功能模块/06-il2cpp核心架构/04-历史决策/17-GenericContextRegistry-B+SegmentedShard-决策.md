# 17 — GenericContextRegistry B+ 分层分片惰性解析方案

## 决策

将 `generic_context.cpp` 中的单例全局 `GenericContextRegistry` 重构为 Per-Module Shard + 全局 RoutingTable 的 Two-Tier 架构，采用 Eager (open definitions) + Lazy (closed instantiations) 两级注册，实现 O(1) 热更卸载和冷启动加速。

## 问题

当前 `GenericContextRegistry` 是单例 + 全局 `std::mutex` 保护的 6 个索引结构：

| 问题 | 影响 |
|------|------|
| 全局锁竞争 | RegisterModuleGenerics 持有锁期间所有查询被阻塞 |
| O(N) cold start | 所有 closed type 的 token→handle 解析在模块加载时同步完成 |
| `arg_buffer[8]` 硬编码 | >8 类型参数的泛型被静默截断 |
| `owned_entries` 未释放 | ModuleShard 卸载时 `MethodGenericContextEntry` 对象不被释放 |
| O(entries) 卸载 | `UnregisterModuleGenerics` 需扫描所有 index 执行 `remove_if`/`erase` |

## 方案对比

| 维度 | 原方案 | Unity il2cpp | B+ Two-Tier（选中） |
|------|--------|-------------|-------------------|
| 架构 | 全局单例 + 6 maps | 全局 hash cons + flat array | Per-module shard + 全局 RoutingTable |
| 冷启动 | O(all_entries) | O(all_entries) | O(open_definitions) |
| 卸载 | O(entries) | 不支持 | O(1) |
| 内存泄漏 | `owned_entries` | 无泄漏 | 无泄漏（shard owning store） |
| 锁粒度 | 全局 mutex | 全局 mutex | per-shard + 全局 CAS |
| 参数限制 | `arg_buffer[8]` | 动态数组 | 动态分配 |

## 架构

```
Module Load (Eager):
  RegisterModuleGenerics()
    ├─ 解析 open_token → open_handle（少量）
    ├─ 在全局 by_open_type 中创建空 vector 占位
    ├─ 保存 raw token data 到 shard.lazy_buffer
    └─ 存方法 token raw data → shard token_pool（不解析）

First Query (Lazy):
  TryResolveClosedType(open_handle, args)
    ├─ by_open_type 查找命中 + 未 resolved
    └─ 触发 LazyResolveOpenType()
        ├─ batch 解析 closed_token→handle, arg_tokens→handles
        ├─ 填充 by_open_type[open_handle]
        └─ 标记 resolved → retry

HotUpdate Unload:
  RoutingTableRemove(module_id)
    └─ 析构 ModuleShard（O(1)）
```

## 数据结构

```cpp
struct ModuleShard {
    TypeInfoHandle[] open_definitions;     // eager
    LazyTypeEntry[]  lazy_types;           // raw tokens
    LazyMethodContext[] lazy_contexts;     // raw tokens
    uint32_t[]       token_pool;            // flat token storage
    unique_ptr[]     owned_entries;         // owning store
    mutex            shard_mutex;
    bool[]           resolve_state;         // per-open-type resolution flag
};

ShardEntry s_routing_table[347];  // open-addressing, CAS-based
```

## 约束

- API 完全向后兼容（`extern "C"` 接口不变）
- 分 Phase 实施：Phase A = 内部重构为 shard + RoutingTable，Phase B = 按 module_id 拆分，Phase C = 惰性解析
- 单体注册 API（`RegisterGenericInstantiation`/`RegisterGenericMethodInstantiation`）保留，module_id=0 直写全局

## 三优先级权衡

| 优先级 | 评估 |
|--------|------|
| P1 性能最优 | ✅ 冷启动从 O(N) 降到 O(open)；查询路径多一次 shard pointer chase，但 P95 不变 |
| P2 方案完美性 | ✅ Per-Module 职责内聚，数据与生命周期一致 |
| P3 热更适配 | ✅ 卸载从 O(N) 降为 O(1)，消除内存泄漏 |

## 关联文档

- [`14-B2-ModuleRegistry-TwoTier-决策.md`](./14-B2-ModuleRegistry-TwoTier-决策.md) — TypeInfoHandle (module_id, token) 编码的基础设施
- [`13-泛型注册Gen2Handle方案决策.md`](./13-泛型注册Gen2Handle方案决策.md) — Gen-2 Handle 双路径架构
- [`16-RuntimeCentricCrossDllDispatch决策.md`](./16-RuntimeCentricCrossDllDispatch决策.md) — 跨 DLL 分派背景
- [`15-泛型上下文运行时.md`](../01-翻译管线/15-泛型上下文运行时.md) — 当前运行时文档
