# Codegen 内存分配体系审核报告

> 日期：2026-05-11
> 审核范围：codegen 代码生成侧、native 运行时侧、分配性能监控、项目级约束遵守情况
> 所有位置 claims 已通过 grep/read 确认

---

## 1. 架构总览：三域分配模型

```
┌──────────────────────────────────────────────────────────────────┐
│                      Three Allocation Domains                    │
├──────────────┬───────────────────┬──────────────────────────────┤
│  GC Domain   │  Domain Domain    │  Raw Domain                  │
├──────────────┼───────────────────┼──────────────────────────────┤
│  managed     │ per-module        │ temp buffers, vtable arrays, │
│  objects/    │ metadata,         │ non-GC structures            │
│  arrays/     │ runtime internals │                              │
│  strings     │                   │                              │
├──────────────┼───────────────────┼──────────────────────────────┤
│  GcAllocate  │ IDomainHeap       │ std::malloc / std::free      │
│  → GcBump    │  (SegregatedHeap  │                              │
│    Cache     │   → HeapAlloc/    │                              │
│    → GC_     │    HeapDestroy)   │                              │
│    MALLOC    │                   │                              │
├──────────────┼───────────────────┼──────────────────────────────┤
│  GC reclaim  │ heap->Destroy()   │ std::free                    │
│  (无 explicit│ (bulk, O(1))     │                              │
│   free)      │                   │                              │
└──────────────┴───────────────────┴──────────────────────────────┘

  GcAllocate → tls_gc_arena.Allocate → GcBumpCache
    ├── size-class hit  → pop free list            (fast path, O(1))
    ├── size-class miss → RefillSizeClass → BumpAllocate → AllocatePage → GC_MALLOC
    └── oversized (>32KB) → direct GC_MALLOC
```

**A→B→C 三阶段路线图**：
- A 阶段 (GcBumpArena)：遗留实现，无 size class 缓存
- B 阶段 (GcBumpCache, **当前活跃**)
- C 阶段（精确分代 GC）：**无设计文档**

---

## 2. P0 — 严重违规（codegen 生成裸 `new T{}`）

项目级硬性约束要求 all codegen 生成的 C++ 必须使用 `CHAOS_IL2CPP_NEW_GC` / `CHAOS_IL2CPP_NEW_GC_ARRAY`，而非裸 `new`。已确认 **11 处真实违规**：

| # | 文件 | 代码行 | 生成的 C++ | 修复方式 |
|---|------|-------|-----------|---------|
| 1 | `ExceptionEmission.cs` | 2280 | `auto* chaos_string = new System.String{}` | `CHAOS_IL2CPP_NEW_GC` |
| 2 | `ReflectionObjectEmission.cs` | 295 | `auto* chaos_array = new chaos_managed_array{}` | `CHAOS_IL2CPP_NEW_GC` |
| 3 | `ReflectionObjectEmission.cs` | 300 | `new CHAOS_IL2CPP_INTPTR[chaos_length]` | `CHAOS_IL2CPP_NEW_GC_ARRAY` |
| 4 | `ReflectionObjectEmission.cs` | 361 | `auto* chaos_type = new System.Type{}` | `CHAOS_IL2CPP_NEW_GC` |
| 5 | `ReflectionObjectEmission.cs` | 816 | `auto* chaos_field = new FieldInfo{}` | `CHAOS_IL2CPP_NEW_GC` |
| 6 | `ReflectionObjectEmission.cs` | 1312 | `auto* chaos_assembly = new Assembly{}` | `CHAOS_IL2CPP_NEW_GC` |
| 7 | `ReflectionObjectEmission.cs` | 1455 | `auto* chaos_assembly_name = new AssemblyName{}` | `CHAOS_IL2CPP_NEW_GC` |
| 8 | `ReflectionObjectEmission.cs` | 1648 | `auto* chaos_method = new MethodInfo{}` | `CHAOS_IL2CPP_NEW_GC` |
| 9 | `ModuleRegistration.cs` | 492 | `auto* attr = new CustomType{}` | `CHAOS_IL2CPP_NEW_GC` |
| 10 | `NativeAot.ReflectionCreateInstance.cpp.scriban` | 14 | `auto* chaos_object = new {{ type }}` | `CHAOS_IL2CPP_NEW_GC` |
| 11 | `NativeAot.StaticInitializationAction.cpp.scriban` | 3 | `auto* chaos_object = new {{ type }}` | `CHAOS_IL2CPP_NEW_GC` |

**影响**：这些裸 `new` 走 `malloc` 而非 GC 堆。BDWGC 无法追踪这些对象，一旦被 GC 堆引用 → use-after-free。

---

## 3. P1 — 架构/设计问题

### 3.1 GcBumpCache::SizeClassIndex 线性扫描
- **位置**：`gc_bump_cache.h:89-96`
- **问题**：每次分配都遍历 20 个大小类。最高 20 次整数比较。
- **影响**：低。但可优化为 constexpr 二分查找（5 次比较）。

### 3.2 GC 分配零计数器
- **位置**：`gc_bump_cache.h` 全域
- **问题**：无任何分配计数器——无法获知分配字节量、命中率、大对象频率。
- **影响**：**高**。OOM 排查、性能分析、热点识别均为盲区。

### 3.3 AllocateBytes 绕过 GcBumpCache
- **位置**：`runtime_core.cpp:383`
- **问题**：`AllocateBytes(size)` → 直接 `config.allocator(=GC_MALLOC)`，不经过 GcBumpCache。运行时关键结构（RuntimeState、ThreadState、ObjectHeaderFat）走慢路径。
- **影响**：中。启动阶段影响小，但 ObjectHeaderFat 在运行期频繁分配。

### 3.4 无跨域 free 检测
- **位置**：`memory_domain.h:140` (DomainFreeTagged)
- **问题**：无法检测 GC 域指针误传入 DomainFreeTagged、Raw 域指针误传入等跨域违规。
- **修复方向**：AllocationHeader 加入 magic number + 校验。

### 3.5 DOMAIN_CURRENT_REALLOC 标记丢失
- **位置**：`native_types.h:242-245`
- **问题**：文档自认 REALLOC 不保留 tag，后续 FREE 路由错误。
- **影响**：中。当前靠使用场景约束（alloc+free 必须在同一域上下文）。

### 3.6 fast_dispatch.cpp LOG_LEVEL=0
- **位置**：`fast_dispatch.cpp:10`
- **问题**：`#define CHAOS_IL2CPP_LOG_LEVEL 0` 使该文件所有分配路径日志被编译消除。
- **影响**：中。调试时不可见。

### 3.7 GcBumpArena 遗留代码
- **位置**：`gc_bump_arena.h` vs `gc_bump_cache.h`
- **问题**：Phase A 遗留代码仍存在，文档描述与实际实现有出入。

---

## 4. P2 — 测试覆盖不足

| 缺口 | 详情 |
|------|------|
| GcBumpCache 无单元测试 | 命中/未命中/大对象/原子 vs 扫描 均无覆盖 |
| MemoryDomain 无单元测试 | 注册/查找/卸载/跨域路由 无覆盖 |
| 无 GC 压力测试或跨域 free 验证 | 测试清单自认"内存正确性：不验证" |
| foundation-dll `performanceObligation=false` | buffer-memory family 未纳入性能验证 |

---

## 5. P3 — 文档/流程

- **C 阶段（精确分代 GC）无设计文档** — wiki 仅有 MemoryDomain 文档，无 GC 路线图
- **codegen 无自动化 new 检查** — 此次发现的 11 处违规说明人工审查未捕获

---

## 6. 性能监控缺口

| 缺口 | 影响 | 说明 |
|------|------|------|
| GcBumpCache 零计数器 | **高** | 分配总量/命中率/大对象频率不可见 |
| PROFILE_SCOPE 默认关闭 | **高** | `profile.h:47`，CHAOS_IL2CPP_PROFILE_ENABLED=0 |
| AllocateBytes 无 PROFILE | **中** | runtime_core.cpp:383 无 PROFILE_SCOPE |
| fast_dispatch LOG_LEVEL=0 | **中** | 分配日志被编译消除 |
| Domain 分配无计数器 | **中** | 仅有 current_usage/peak_usage，无分配次数 |
| SizeClass miss 无日志 | **低** | 非标准大小绕过缓存 0 可见性 |

---

## 7. 优化建议（按优先级）

### S1 — 替换 11 处裸 `new` 为 `CHAOS_IL2CPP_NEW_GC`（P0 修复）
- **收益**：消除 11 处 use-after-free 风险
- **成本**：极低（每处 1 行替换）
- **风险**：低

### S2 — GcBumpCache 加分配计数器
- **收益**：可观测性大幅提升
- **成本**：在 SizeClassState 加 3 个 atomic<uint64_t>

### S3 — codegen 新增自动化 new 检查
- **收益**：防止未来引入相同违规
- **成本**：在 emission 代码或 unit test 中 grep "`new `" 模式

### S4 — AllocateBytes 接入 GcBumpCache
- **收益**：运行时基础设施从缓存受益
- **成本**：中（需确认语义兼容性）

### S5 — SizeClassIndex 二分查找
- **收益**：每次 Allocate 减少 ~15 次比较
- **成本**：极低（替换 5 行函数）

### S6 — Debug 构建默认开启 PROFILE_SCOPE
- **收益**：分配路径性能数据立即可用
- **成本**：config.h 条件宏调整（PROFILE_SCOPE 约 30 周期开销）

### S7 — Domain REALLOC tag 保留
- **收益**：消除已知的跨域路由风险
- **成本**：中（header 读写逻辑）

### S8 — 移除 GcBumpArena 遗留代码
- **收益**：降低维护者混淆
- **成本**：极低

---

## 8. 项目级约束遵守情况总结

| 约束 | 状态 | 详情 |
|------|------|------|
| 禁止全局 operator new/delete 重载 | ✅ 正常 | 无全局重载 |
| codegen 使用 CHAOS_IL2CPP_ 宏 | ❌ **严重违反** | 11 处裸 `new` |
| 禁止跨域分配/free 不匹配 | ⚠️ 人工依赖 | 无运行时防御 |
| 分配策略通过 GcAllocate 切换 | ✅ 架构正确 | codegen 透明 |

---

报告完毕。如需对某项建议做详细实现设计或开始修复，请指示。