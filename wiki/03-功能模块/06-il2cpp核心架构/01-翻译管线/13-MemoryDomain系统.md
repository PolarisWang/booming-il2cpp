# MemoryDomain 系统

## 核心职责

每个加载的模块（AOT 或热更新）获得一个独立的 `MemoryDomain`，绑定额外的堆。域卸载时批量释放所有内存，无需逐个回收。

## 关键类型

| 类型 | 说明 |
|------|------|
| `DomainId` | 不透明 uint32 域标识 |
| `MemoryDomain` | 域结构：domain_id、module_name、module_kind（0=AOT/1=热更新）、堆指针、使用量统计 |
| `IDomainHeap` | 可插拔堆策略抽象基类（Allocate/Reallocate/Free/Destroy） |
| `DomainScope` | RAII 守卫，自动 push/pop 当前域 |
| `HeapFactoryFn` | 进程级工厂，允许调用者覆盖每域堆策略 |

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

## 与 codegen 管线的关系

- AOT 编译在 bootstrap 阶段为每个模块发射一个 `RegisterMemoryDomain` 调用
- 后续的分配通过 TLS 域栈隐式标记当前模块的 `DomainId`
- codegen 无需手动传递 domain id
- 热更新卸载时通过 `UnregisterDomain` 批量释放该域的所有内存

## 位置

- 模块: `chaos_runtime_core`
- 文件: `src/native/runtime-core/memory_domain.*`
