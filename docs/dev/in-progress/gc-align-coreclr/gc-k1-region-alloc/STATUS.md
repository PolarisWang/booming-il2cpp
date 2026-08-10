# STATUS — GC-K1 RegionAlloc (Phase 5 地基)

## 元信息

```yaml
task_id: gc-align-coreclr/gc-k1-region-alloc
task_type: plan
phase: in-progress   # 地基 (SelectRegionSize) done; 完整区域化后续
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

## Scope（跨会话 Phase 5 第一步：可验证地基）

引入 CoreCLR 式 region size class（4/2/1MB），作为区域化重构的地基。**非一次性完成整个区域化**（LOH-via-region / 世代写屏障 region→gen 映射 / 分代重绑为后续 K2/K3）。

## 实现（gc_region.h/.cpp）

- **`RegionManager::SelectRegionSize(kind, min_size)`** 自适应 helper：
  - NURSERY/GEN1/DOMAIN/POH 保持固定大小（不破坏行为）。
  - TENURED/default 按 CoreCLR class：min>2MB→4MB，>1MB→2MB，否则 1MB。
- **接入 `AllocateRegion`**：TENURED/default 路径用 `SelectRegionSize`，NURSERY/GEN1/DOMAIN/POH 不变。

## 对比 CoreCLR

| CoreCLR | CRAG |
|---------|------|
| basic region 4/2/1MB 自适应（interface.cpp:455） | ✅ SelectRegionSize 4/2/1MB class |
| large region for UOH | ⬜ LOH-via-region 后续 |

## 验证

| 项 | 结果 |
|----|------|
| `chaos_runtime_core` Debug 编译 | ✅ |
| GC 单测：region 4/4、atomic 5/5、young_collector 5/5、gen1 11/11、loh 8/8、old_gen 6/6、poh 10/10、scheduler 8/8 | ✅ 全部 0 |

## 已提交

- commit（填）

## Exit Criteria（地基已满足）

- [x] SelectRegionSize 4/2/1MB 自适应 + 接入 TENURED
- [x] 既有 kind 大小不变 + 无回归

## 后续（跨会话，记录）

- K1b：LOH-via-region（大对象走 region 而非独立 VirtualAlloc segment）。
- K2：世代写屏障 region→gen 映射 + card bundle。
- K3：分代决策区化重绑。
- K4：区域化回归。
