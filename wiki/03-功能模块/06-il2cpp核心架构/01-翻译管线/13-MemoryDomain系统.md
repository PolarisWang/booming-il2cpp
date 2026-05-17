# MemoryDomain 系统

## 核心职责

每个加载的模块（AOT 或热更新）获得一个独立的 `MemoryDomain`，绑定额外的堆。域卸载时批量释放所有内存，无需逐个回收。

## 关键类型

| 类型 | 说明 |
|------|------|
| `DomainId` | 不透明 uint32 域标识 |
| `MemoryDomain` | 域结构：domain_id、module_name、module_kind（0=AOT/1=热更新）、堆指针、使用量统计、GC 分配追踪 |
| `IDomainHeap` | 可插拔堆策略抽象基类（Allocate/Reallocate/Free/Destroy） |
| `DomainScope` | RAII 守卫，自动 push/pop 当前域 |
| `HeapFactoryFn` | 进程级工厂，允许调用者覆盖每域堆策略 |

## MemoryDomain 结构

```cpp
struct MemoryDomain {
    DomainId      domain_id;
    const char*   module_name;
    uint32_t      module_kind;       // 0 = AOT, 1 = HotUpdate
    IDomainHeap*  heap;              // 域专属堆
    int64_t       current_usage;     // 当前域内存使用量
    int64_t       peak_usage;        // 峰值使用量
    int64_t       usage_limit;       // 配额上限 (0 = 不限)
    int64_t       gc_allocated_bytes; // 累计 GC 分配字节数（托管堆）
    bool          is_unloaded;       // 是否已卸载
};
```

`gc_allocated_bytes` 由 `GcTrackDomainAlloc(size)` 在每个分配路径的慢路径中累加，用于按域追踪托管堆分配量。

## 三堆策略

| 策略 | 说明 |
|------|------|
| SegregatedHeap | 默认策略，Win32 HeapCreate/HeapDestroy，批量释放 O(1) |
| ArenaHeap | 竞技场分配器 |
| PassthroughHeap | 直接 malloc/free 回退 |

## TLS 域栈

通过线程本地存储维护当前域栈：

- `PushDomain(domain_id)` — 将域压入当前线程的域栈
- `PopDomain()` — 弹出域栈顶
- `CurrentDomain()` — 获取当前域
- `DomainScope` — RAII 包装器

## 每域 GC 分配追踪

每个 `MemoryDomain` 维护 `gc_allocated_bytes` 字段，由 `GcTrackDomainAlloc(size)` 在 GC 分配慢路径中累加：

- **NurseryAllocateSlow**：每个 bump 返回点（young GC / full GC / BGC remark / BGC compact 之后）调用 `memory_domain::GcTrackDomainAlloc(size)`
- **OldGen::Allocate**：每个返回点（size-class freelist / oversized page / LOH）调用 `memory_domain::GcTrackDomainAlloc(size)`

追踪使用 `CurrentDomain()` 获取当前线程的活跃域，无额外参数传递开销。该数据可用于按模块统计 GC 分配量、诊断内存泄漏、以及未来实现每域 GC 触发策略。


## 与 codegen 管线的关系

- AOT 编译在 bootstrap 阶段为每个模块发射一个 `RegisterMemoryDomain` 调用
- 后续的分配通过 TLS 域栈隐式标记当前模块的 `DomainId`
- codegen 无需手动传递 domain id
- 热更新卸载时通过 `UnregisterDomain` 批量释放该域的所有内存

## 位置

- 模块: `chaos_runtime_core`
- 文件: `src/native/runtime-core/memory_domain.*`
