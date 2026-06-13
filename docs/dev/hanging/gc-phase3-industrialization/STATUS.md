# STATUS — GC Phase 3 Industrialization

## 元信息

```yaml
phase: planned
task_id: gc-phase3-industrialization
created: 2026-06-08
source: Profile Pipeline 交付 → GC 调优
blocking_questions: []
```

## 目标

基于 Foundation-DLL Profile Pipeline 采集的每方法 alloc / fast path / GC pause 数据，驱动 GC 参数调优（bump cache size, nursery size, promotion threshold），实现可量化的 GC 性能提升。

## 当前已知数据

| 指标 | buffers | text | 说明 |
|------|---------|------|------|
| 方法数 | 36 | 120 | |
| 总分配 | 512B | 5,216B | |
| 每方法平均分配 | 14B | 43B | 极小对象 |
| Fast path rate | 100% | 100% | bump cache 全命中 |
| GC pause | 0 | 0 | 无 GC 触发 |
| GC pause count | 0 | 0 | |

## 问题

当前 test subject 分配量太小，无法触发 GC。需要构造高分配场景来验证 GC 参数调整的效果。

## 阶段

### Phase 3a — 高分配 Benchmark 方法

在 foundation-dll 中添加高分配 subject 方法（如 `string.Concat` 循环、`byte[]` 大数组、`StringBuilder` 等），使单方法分配达 MB 级，触发 GC。

```csharp
[Benchmark]
public static void Benchmark_StringAlloc_1()
{
    var sb = new StringBuilder();
    for (int i = 0; i < 10000; i++)
        sb.Append(i.ToString());
    _ = sb.ToString();
}
```

目标: 单方法分配 > 256KB，触发至少 1 次 GC。

### Phase 3b — GC 参数 baseline

在 profile build 下运行高分配方法，采集 baseline:

```bash
python -m verification.chunk_pipeline --chunk buffers --stages profile --native-config profile
```

记录 baseline: gcPauseNs, nurseryAllocBytes, slowPathCount

### Phase 3c — 单参数调优

每次调整一个 GC 参数，重新采集，对比 baseline:

| 参数 | 当前位置 | 调优方向 | 预期效果 |
|------|---------|---------|---------|
| Bump cache size | 128KB | 增大至 256KB | 减少 slow path |
| Nursery size | 16MB | 增大至 32MB | 降低 GC 频率 |
| Promotion threshold | 1 | 提高至 2 | 减少 Gen2 晋升 |

### Phase 3d — 参数组合优化

选择最佳组合，验证:
- GC pause 总和降低 > 20%
- Allocation throughput 提升 > 10%
- Gen2 晋升率降低 > 30%

## 产出物

- 每参数组合的 profile.json (gcPauseNs, allocBytes, slowPathCount)
- benchmark report 对比
- GC 参数推荐值
