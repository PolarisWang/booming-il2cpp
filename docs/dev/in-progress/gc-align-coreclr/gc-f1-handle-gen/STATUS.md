# STATUS — GC-F1 HandleGenPrune

## 元信息

```yaml
task_id: gc-align-coreclr/gc-f1-handle-gen
task_type: plan
phase: completed
parent_task_id: gc-align-coreclr
source_relation: roadmap-child
clearance_source: parent-roadmap
blocking_questions: []
question_clearance: cleared
clearance_confirmed_by_user: true
entry_skill: dev-executing-plans
created: 2026-08-10
created_by: main-agent
```

## Scope

handle 表扫描对齐 CoreCLR 分代剪枝（handletablepriv.h clump 代字节 + 按代扫描）。CRAG 现全表遍历 + 每 handle `IsNurseryPointer` region 查询；加 **per-handle 缓存 nursery bit** 扫描短路，免重复 region 查询。

## 实现（engine_lifecycle.cpp/.h）

- **`GcHandleEntry` 加第 6 字段 `bool points_to_nursery`**：缓存 object_instance 是否指向 nursery。
- 5 个构造点（strong/weak/longweak/pinned/asyncpinned）初始化 `RegionManager::IsNurseryPointer(obj)`。
- `GcSetHandleTarget`：目标更新后刷新 bit。
- weak 转发（promote 后）：更新 object_instance 同步 `points_to_nursery=false`。
- `GcIterateTenuredHandles` / `GcProcessWeakHandlesAfterYoungGC`：用缓存 bit 剪枝（免 region 查询）。

## 对比 CoreCLR

| CoreCLR | CRAG |
|---------|------|
| per-clump 代字节 + COMPUTE_CLUMP_MASK 剪枝（handletablescan.cpp:702） | ✅ per-handle `points_to_nursery` bit 剪枝（简化落地：unordered_dense_map 无 clump，改为 per-entry 缓存） |
| 按 condemned 代跳过 | ✅ young/tenured 扫描用缓存 bit 短路 |

## 验证

| 项 | 结果 |
|----|------|
| `chaos_runtime_core` Debug 编译 | ✅ |
| GC 单测：handle 11/11、finalizer 16/16、events 5/5、atomic 5/5、region 4/4、young_collector 5/5、gen1 11/11、loh 8/8、old_gen 6/6 | ✅ 全部 0 |

## 已提交

- commit（填）

## Exit Criteria

- [x] per-handle 缓存 nursery bit，扫描短路免 region 查询
- [x] 无回归（handle/finalizer 等全 0）
