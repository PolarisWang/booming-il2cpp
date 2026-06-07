# STATUS — p01-runner-gcinfo

## 元信息

```yaml
phase: planned
task_id: p01-runner-gcinfo
parent_task_id: foundation-dll-profile-comparison
source_task_id: foundation-dll-profile-comparison
source_relation: roadmap-child
clearance_source: parent-roadmap
blocking_questions: []
question_clearance: cleared
clearance_confirmed_by_user: true
entry_skill: dev-executing-plans
```

## Scope

扩展 Chaos.TestFramework.Runtime runner，在 benchmark 模式下采集 GC 统计信息并输出到 JSON。

## Inputs

- `src/reference/Chaos.TestFramework.Runtime/` — .NET runner 源码
- `testing/foundation-dll/verification/stages/managed_benchmark.py` — 消费方

## Expected Outputs

| 文件 | 说明 |
|------|------|
| `src/reference/Chaos.TestFramework.Runtime/Runner.cs` 或 `BenchmarkRunner.cs` 修改 | 在 benchmark 循环前后采集 GC 统计 |

## Exit Criteria

```bash
dotnet exec runner.dll --assembly X --kind benchmark --iterations 1000
# 输出 JSON 包含 gcInfo 字段:
# {"results": [{"label":"...", "elapsedMs": 1.23, "gcInfo": {
#   "totalAllocatedBytes": 4096,
#   "collectionCount0": 0,
#   "heapBefore": 16777216,
#   "heapAfter": 16781312
# }}]}
```

## 实现步骤

### Step 1: 定位 Runner 的 benchmark 方法

在 `Chaos.TestFramework.Runtime` 中找到执行 benchmark 循环的方法（通常是 `RunBenchmark` 或 `ExecuteBenchmark`）。

### Step 2: 在 benchmark 循环前记录

```csharp
var allocBefore = GC.GetAllocatedBytesForCurrentThread();
var heapBefore = GC.GetTotalMemory(false);
var gcCount0Before = GC.CollectionCount(0);
```

### Step 3: 执行 benchmark 循环（不变）

```csharp
stopwatch.Start();
for (int i = 0; i < iterations; i++) {
    method();
}
stopwatch.Stop();
```

### Step 4: 在 benchmark 循环后记录

```csharp
var allocAfter = GC.GetAllocatedBytesForCurrentThread();
var heapAfter = GC.GetTotalMemory(false);
var gcCount0After = GC.CollectionCount(0);
```

### Step 5: 将 gcInfo 加入输出

```csharp
var gcInfo = new {
    totalAllocatedBytes = allocAfter - allocBefore,
    heapBefore = heapBefore,
    heapAfter = heapAfter,
    collectionCount0 = gcCount0After - gcCount0Before,
};

// 加入已有输出
var record = new {
    label = methodName,
    elapsedMs = sw.Elapsed.TotalMilliseconds,
    opsPerSecond = iterations / sw.Elapsed.TotalSeconds,
    threw = false,
    gcInfo = gcInfo,  // 新增
};
```
