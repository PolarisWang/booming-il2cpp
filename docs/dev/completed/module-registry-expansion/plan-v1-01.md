# Module Registry 扩容 + Tombstone 实现计划

**目标：** Module Registry 从 64→256 模块扩容，新增空闲列表分配和 tombstone 标记，支持 hotunload 安全降级。

**架构：** 以空闲列表替代线性自增分配；已卸载模块保留条目但标记 tombstone，确保旧 TypeInfoHandle 始终可解析。

**技术栈：** C++17, module_registry.h/cpp

**架构审核模式：** critical（涉及 runtime-core + contracts 主线）

**结构告警重点：**
- 新增空闲列表逻辑不得与原有线性分配路径冲突
- Tombstone 标记必须在所有 LookupModule 消费路径中一致处理

**设计文档：** 父任务 roadmap-v1-01.md

**问题清零来源：** parent-roadmap

**收尾约束：** 测试通过 → 归档 completed → 合并&提交

---

## 修改清单

### `module_registry.h`

| 修改 | 说明 |
|------|------|
| `kMaxModules`: 64 → 256 | 常量调整 |
| `ModuleDescriptor` 新增 `bool tombstone = false` | tombstone 标记字段 |
| 声明 `MarkModuleTombstone(uint32_t module_id)` | 将模块标记为 tombstone |
| 声明 `IsModuleTombstone(uint32_t module_id)` | 查询 tombstone 状态 |
| 声明 `FreeModuleId(uint32_t module_id)` | 回收 module_id 到空闲列表 |

### `module_registry.cpp`

| 修改 | 说明 |
|------|------|
| 新增 `g_free_list` (CHAOS_IL2CPP_VECTOR<uint32_t>) | 空闲列表 |
| `RegisterModule`: 优先从空闲列表分配 | 先查空闲列表，再查线性槽位 |
| `LookupModule`: 对 tombstone 模块返回合法指针 | type_count = 0，其余字段保留 |
| 实现 `MarkModuleTombstone` | 设置 tombstone=true, type_count=0, 入空闲列表 |
| 实现 `IsModuleTombstone` | 检查 tombstone 字段 |
| 实现 `FreeModuleId` | 将 module_id 加入空闲列表 |

### `runtime_instantiation.cpp` (可能)

`UnregisterModuleGenerics` 中调用 `MarkModuleTombstone(module_id)` 标记模块为 tombstone。

---

## 风险

| 风险 | 缓解 |
|------|------|
| g_module_count 语义变化（不再单调递增） | RegisterModule 返回后可确认 handle 有效 |
| 已缓存的 TypeInfoHandle 指向 tombstone 模块 | LookupModule 返回合法指针，type_count=0 表示无类型 |
| 空闲列表竞争 | 初始实现：AOT 启动串行，hotunload 单线程 |
