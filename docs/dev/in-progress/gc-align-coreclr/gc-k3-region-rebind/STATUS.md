# STATUS — GC-K3 RegionRebind

## 元信息

```yaml
task_id: gc-align-coreclr/gc-k3-region-rebind
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

分代决策区化重绑：region→gen 表随 region 新代重写，使 K2a 表跨 GC 保持正确。对齐 CoreCLR `set_region_gen_num` + gen1 为独立代。

## 实现（gc_region.cpp AllocateRegion）

- `REGION_GEN1`（survivor）从 old(2) 改为 **young(0)** —— Gen1 属 young 侧（`G_YoungGen().gen1_region`，young GC Phase 2 精确扫其跨代引用），标 young 使写屏障对 Gen1 对象写入正确短路（contents scanned wholesale）。
- `region_gen = (NURSERY || GEN1) ? young : old`，其余成熟区 old(2)。

## 对比 CoreCLR

| CoreCLR | CRAG |
|---------|------|
| set_region_gen_num（gen1 = gen1 独立代） | ✅ Gen1→young（CRAG 2-gen 模型下 young 侧） |

## 验证

| 项 | 结果 |
|----|------|
| `chaos_runtime_core` Debug 编译 | ✅ |
| GC 单测：gen1 11/11、young_collector 5/5、atomic 5/5、region 4/4、loh 8/8、old_gen 6/6、poh 10/10、scheduler 8/8、root_scanner 5/5、handle 11/11、card_table_ext 5/5 | ✅ 全部 0 |

## 已提交

- `ec71dce0b`

## Exit Criteria

- [x] Gen1→young 重绑（K2a 表与 CRAG 分代模型一致）
- [x] 无回归
