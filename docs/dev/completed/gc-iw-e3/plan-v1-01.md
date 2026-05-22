# gc-iw-e3：AsyncPinned handle + Gen1 部分保留（C19/C20）实现计划

> **面向执行 Agent：** 使用 dev:executing-plans 技能执行。步骤使用复选框（`- [ ]`）语法跟踪。当前任务带有 `parent_task_id: 20260523-crag-gc-industrialization-sequel`，执行完成时必须先归档当前子任务目录，再同步父任务。

**目标：** 实现 AsyncPinned handle 类型支持和 Gen1 部分保留（3 代分代对齐）

**架构：** 两个独立但均落实在 GC 引擎层的改动；C19 在 handle 表层扩展新 handle 类型，C20 修改 Gen1 收集器从"promote-all"变为"partial-retention"语义

**技术栈：** C++17, Windows (MSVC), CRAG GC

**架构审核模式：** critical（触及 GC 引擎核心路径）

**结构告警重点：** 重复逻辑（C19 的 handle 创建与现有强/pinned handle 高度相似）、文件职责（engine_lifecycle.cpp 已过重）、Codegen 与 Scriban 分层

**权责图审核主题：** gc_handle (engine_lifecycle.cpp) / gc_gen1.cpp authority — ABI handle 类型映射、Gen1 部分保留语义完整性

**AOT/IL2CPP/Test Governance Intake：**
- capabilityFamily: gc
- capabilityItem: AsyncPinnedHandle, Gen1PartialRetention
- ownerSubjectId: gc-subsystem
- proofRequired: true
- benchmarkRequired: false
- hotupdateImpact: None
- formalVerificationObjects: [test_gc_handle, test_gc_gen1, test_gc_sanity]
- requiredGates: collector -> registry -> workspace

**设计文档：** n/a（设计摘要写入 STATUS.md）

**问题清零来源：** parent-roadmap（20260523-crag-gc-industrialization-sequel）

**计划来源：** roadmap-child-auto

**预期知识沉淀：** n/a（GC 架构细节已在现有 wiki 覆盖）

**收尾约束：** 执行完成后必须进入"结构告警与架构审视 -> 测试通过 -> 归档 completed -> 合并&提交"固定链路。

---

## 修改文件清单

| 文件 | 职责 |
|------|------|
| `core/engine_lifecycle.h` | GcHandleEntry 添加 async_pinned 字段 |
| `core/engine_lifecycle.cpp` | 新增 GcCreateAsyncPinnedHandle + 更新 GcFreeHandle/GcProcessWeakHandlesAfterYoungGC + Gen1 处理 |
| `gc/gc_api.h` | 新增 chaos_gc_create_async_pinned_handle 声明 |
| `gc/gc_api.cpp` | 新增 chaos_gc_create_async_pinned_handle 实现 |
| `gc/gc_gen1.cpp` | C20: GcGen1Collection 改为 partial-retention 语义（mark-compact + 分级 promote） |
| `gc/gc_gen1.h` | C20: Gen1CollectionResult 扩展字段 |
| `gc/gc_young_gen.h` | C20: YoungGeneration 添加 gen1_prev_compact_end |
| test files | 编译验证 |

## C19 — AsyncPinned handle

### 设计

AsyncPinned handle 是强引用 + pin 的组合类型，语义等价于 GCHandleType::Normal | GCHandleType::Pinned 同时生效。

当前 `GcHandleEntry` 已有 `pinned` 和 `weak` 两个 bool 标志位。AsyncPinned 新增一个 `async_pinned` 标志位：

```cpp
struct GcHandleEntry {
    void* object_instance;
    bool pinned;          // GCHandleType::Pinned
    bool weak;            // GCHandleType::Weak / WeakTrackResurrection
    bool track_resurrection;  // WeakTrackResurrection
    bool async_pinned;    // GCHandleType::AsyncPinned (strong + pinned)
};
```

AsyncPinned handle 的创建行为：
- 对象添加 pin set（同 GcCreatePinnedHandle）
- 强引用（同 GcCreateStrongHandle）
- 不设置 weak/track_resurrection

### 实现步骤

1. **engine_lifecycle.h**: GcHandleEntry 添加 `bool async_pinned` 字段，调整初始化顺序
2. **engine_lifecycle.cpp**: 新增 `GcCreateAsyncPinnedHandle()` 函数
   - 分配 handle ID
   - 写入 `{ obj, pinned=true, weak=false, track_resurrection=false, async_pinned=true }`
   - 注册到 `s_pin_set`
3. **engine_lifecycle.cpp**: `GcFreeHandle` 中处理 async_pinned（从 pin set 移除）
4. **engine_lifecycle.cpp**: `GcProcessWeakHandlesAfterYoungGC` — async_pinned 不是 weak，无需改动
5. **gc_api.h**: 声明 `chaos_gc_create_async_pinned_handle`
6. **gc_api.cpp**: 实现 — 转发到 GcCreateAsyncPinnedHandle

### 关键边界

- AsyncPinned handle FREE 时必须从 pin set 移除（同 Pinned handle）
- pin set 操作需要持有 `s_pin_set_mutex`
- 不需要在 `GcIterateHandleTable` 中特殊处理（object_instance 非空即参与 mark）

## C20 — Gen1 部分保留

### 设计

当前 Gen1 收集器（`GcGen1Collection()`）在 Phase 4 将全部标记为 live 的对象 promote 到 Gen2，然后在 Phase 5 完全重置 gen1_bump。这等效于 "drain Gen1"。

改为部分保留后的行为：

- **Phase 4a**: 标记阶段不变，仍用 mark bitmap 识别 live 对象
- **Phase 4b**: 对每个 live 对象，判断其"代龄"：
  - `"新"对象` = 地址在 [`gen1_prev_compact_end`, `gen1_bump`) 范围内（上次 Gen1 收集后新 promote 进来的）
  - `"老"对象` = 地址在 [`gen1->begin`, `gen1_prev_compact_end`) 范围内（已存活至少一次 Gen1 收集）
- **Phase 4c**: 
  - "老"对象 → promote 到 Gen2（memcpy）
  - "新"对象 → compact 到 Gen1 起始位置（本地 buffer 收集后 memcpy 回 Gen1 start）
- **Phase 5**: 更新 `gen1_bump` = begin + compacted_size，更新 `gen1_prev_compact_end` = begin + compacted_size

### 边界条件

- `gen1_prev_compact_end` 初始为 `nullptr`（首次 Gen1 收集：所有对象视为"新"）
- promotion_age_threshold_ = 1 语义：所有对象在第一次 Gen1 收集时 kept（compact），第二次 Gen1 收集时 promote
- 当 Gen1 完全为空时（gen1_bump == begin），跳过收集
- 当 compacted_size == 0（所有对象都 promoted 或 dead），gen1_bump 重置为 begin，gen1_prev_compact_end 重置为 nullptr
- 当 compacted_size > 0 但 promotion 部分失败：保持原有失败处理（不 reset gen1_bump）

### 实现步骤

1. **gc_young_gen.h**: YoungGeneration 添加 `char* gen1_prev_compact_end{nullptr}`
2. **gc_gen1.h**: Gen1CollectionResult 添加 `bytes_compacted` 字段
3. **gc_gen1.cpp**: GcGen1Collection Phase 4 重构：
   - 扫描阶段分离"老"和"新"对象的 promote/compact 目标
   - 为 compaction 分配临时 buffer（最大 gen1_bump - begin = 8 MB，stack 不适合，用 malloc）
   - 先 promote "老"对象到 Gen2
   - 再 compact "新"对象到本地 buffer
   - 将 compacted buffer memcpy 回 gen1->begin
4. **gc_gen1.cpp**: GcGen1Collection Phase 5 更新：
   - gen1_bump = gen1->begin + compacted_size
   - gen1_prev_compact_end = gen1->begin + compacted_size

## 测试与验证

- `cmake --build build/vs2022 --target chaos_runtime_core --config Debug` — 编译通过
- `cmake --build build/testing --target test_gc_scheduler --config RelWithDebInfo` — Gen1 调度器测试
- `cmake --build build/testing --target test_gc_gen1 --config RelWithDebInfo` — Gen1 功能测试

---

## 执行步骤

### 步骤 1: C19 — AsyncPinned handle

- [ ] 1.1 engine_lifecycle.h: GcHandleEntry 添加 `bool async_pinned`
- [ ] 1.2 engine_lifecycle.cpp: 新增 GcCreateAsyncPinnedHandle()
- [ ] 1.3 engine_lifecycle.cpp: GcFreeHandle 处理 async_pinned（移除 pin set）
- [ ] 1.4 gc_api.h: 声明 chaos_gc_create_async_pinned_handle
- [ ] 1.5 gc_api.cpp: 实现 chaos_gc_create_async_pinned_handle

### 步骤 2: C20 — Gen1 部分保留

- [ ] 2.1 gc_young_gen.h: YoungGeneration 添加 `char* gen1_prev_compact_end{nullptr}`
- [ ] 2.2 gc_gen1.h: Gen1CollectionResult 添加 `CHAOS_IL2CPP_SIZE bytes_compacted{0}`
- [ ] 2.3 gc_gen1.cpp: GcGen1Collection Phase 4 重构（old→promote, new→compact）
- [ ] 2.4 gc_gen1.cpp: GcGen1Collection Phase 5 更新（gen1_bump + gen1_prev_compact_end）

### 步骤 3: 编译与错误修复

- [ ] 3.1 cmake --build build/vs2022 --target chaos_runtime_core --config Debug
- [ ] 3.2 修复编译错误

### 步骤 4: 结构告警与架构审视

- [ ] 4.1 检查重复代码（C19 handle 创建函数与现有函数高度相似）
- [ ] 4.2 检查文件职责（engine_lifecycle.cpp 是否过重）
- [ ] 4.3 权责图审核：gc_handle / gc_gen1

### 步骤 5: 归档

- [ ] 5.1 更新 STATUS.md 为 completed
- [ ] 5.2 移动目录到 docs/dev/completed/
- [ ] 5.3 更新父 roadmap 的 child mapping + recommended_next_child
- [ ] 5.4 删除 docs/dev/ACTIVE.md（整个 roadmap 完成）
- [ ] 5.5 使用 dev-finishing-a-development-branch 技能
