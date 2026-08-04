# STATUS — t01-cpp-profile-stats

## 元信息

```yaml
phase: planned
task_id: t01-cpp-profile-stats
parent_task_id: foundation-dll-profile-pipeline
source_task_id: foundation-dll-profile-pipeline
source_relation: roadmap-child
clearance_source: parent-roadmap
blocking_questions: []
question_clearance: cleared
clearance_confirmed_by_user: true
entry_skill: dev-executing-plans
```

## Scope

在 `src/native/runtime-core/` 中实现 profile 数据采集的基础设施，并在 entry.exe 中新增 `--profile` flag 路由。

## Inputs

- `docs/dev/in-progress/foundation-dll-profile-pipeline/STATUS.md` — 父设计摘要
- `src/native/runtime-core/` — 现有 runtime-core 代码结构
- `src/native/common/chaos/profile.h` — 现有 PROFILE_SCOPE 基础设施

## Expected Outputs

| 文件 | 说明 |
|------|------|
| `src/native/runtime-core/profile_stats.h` | ProfileData / GcPauseStats / AllocVolumeStats 结构体定义 |
| `src/native/runtime-core/profile_stats.cpp` | thread_local 采集器实现 + Flush/Reset/GetThreadProfileData |
| `src/native/runtime-core/entry.cpp` 修改 | 新增 `--profile` flag 路由 + JSON 序列化输出 |

## Exit Criteria

```bash
# 构建 profile 配置
cmake --preset profile && cmake --build build-profile

# 验证 --profile flag 输出
./entry.exe --profile --chunk numerics
# 输出包含以下 JSON 字段:
# - gcPauseNs / maxGcPauseNs / gcPauseCount
# - nurseryAllocBytes / oldGenAllocBytes / allocCount
# - heapBefore / heapAfter
# - fastPathCount / slowPathCount
```

## 实现步骤

### Step 1: `profile_stats.h` — 数据结构

```cpp
// src/native/runtime-core/profile_stats.h
#ifndef CHAOS_IL2CPP_PROFILE_STATS_H_
#define CHAOS_IL2CPP_PROFILE_STATS_H_

#include <cstdint>

namespace chaos::il2cpp::runtime_core {

struct GcPauseStats {
    int64_t total_pause_ns = 0;
    int64_t max_pause_ns = 0;
    int32_t pause_count = 0;
};

struct AllocVolumeStats {
    int64_t nursery_bytes = 0;
    int64_t old_gen_bytes = 0;
    int64_t large_object_bytes = 0;
    int32_t alloc_count = 0;
};

struct ProfileData {
    GcPauseStats gc_pause;
    AllocVolumeStats alloc_volume;
    int64_t heap_before = 0;
    int64_t heap_after = 0;
    int32_t fast_path_count = 0;
    int32_t slow_path_count = 0;
};

// Thread-local accumulator
ProfileData& GetThreadProfileData();
void ResetThreadProfileData();
void FlushThreadProfileData(int method_index);

} // namespace chaos::il2cpp::runtime_core

#endif // CHAOS_IL2CPP_PROFILE_STATS_H_
```

### Step 2: `profile_stats.cpp` — 采集实现

```cpp
// thread_local ProfileData + Get/Reset/Flush 实现
// Flush: 将当前 thread_local 数据写入全局环形缓冲区
// (类似 perf store 的结构，按 method_index 索引)
```

### Step 3: `entry.cpp` — --profile flag

在 `--benchmark-all` 的 `RunMode` 枚举中新增 `kProfileMode`，复用相同的 dispatch 循环但：
- dispatch 前后采集 `heap_before` / `heap_after`（通过 GC API）
- 每方法 dispatch 后 `FlushThreadProfileData(method_index)`
- 最终输出 JSON 格式的 profile 数据

### Step 4: PROFILE_SCOPE 预埋点

在以下热点函数中插入 `CHAOS_IL2CPP_PROFILE_SCOPE` 以便 profile build 下自动采集：
- `NurseryAllocateSlow` (GC pause start/end)
- `GcAllocateImpl` (fast/slow path count)

## 注意事项

- 只在 `CHAOS_IL2CPP_PROFILE_ENABLED=1` (profile build) 时编译，不影响 check/ship build
- thread_local ProfileData 初始化开销在 profile build 中可接受
- JSON 输出格式必须与 Phase 2 的 `perf_baseline.py` 兼容
