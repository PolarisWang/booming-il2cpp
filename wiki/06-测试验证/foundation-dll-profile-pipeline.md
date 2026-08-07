# Foundation-DLL Profile Pipeline 使用文档

## 概述

Profile Pipeline 是 foundation-dll 验证管线的扩展 stage，用于采集每方法的 GC pause、allocation volume、heap delta、fast path rate 和代码体积数据，为 GC 优化和性能分析提供数据支撑。

## 架构

```
entry.exe --profile (profile build)
  → profile_stats.h/.cpp (thread-local 采集)
  → stages/profile.py (Python 编排)
    → results/profile.json (原始数据)
    → results/profile-history.jsonl (时序数据)
    → analysis/gc_check.py (GC 健康检查)
    → analysis/perf_baseline.py (基线)
    → analysis/perf_compare.py (对比)
    → analysis/regression_report.py (聚合报告)
    
.NET Runner --kind benchmark
  → BenchmarkRunner.cs (gcInfo 采集: GC.GetAllocatedBytesForCurrentThread)
  → managed_benchmark.py → benchmark-history.jsonl
  → benchmark_report.py → comparison.json (gcComparison 节)
  → analysis/profile_compare.py (AOT vs JIT 逐方法对比)
```

## 前置条件

### Native 构建

Profile Stage 需要 `cmake --preset profile` 构建的 `entry.exe`：

```bash
# 在 chunk native 目录中
cd tests/e2e/translation/System.Private.CoreLib/chunks/{slug}/native
cmake --preset profile -B build
cmake --build build --config RelWithDebInfo
```

Profile 构建自动启用：
- `CHAOS_IL2CPP_PROFILE_ENABLED=1` (PROFILE_SCOPE + profile_stats 采集)
- `CHAOS_IL2CPP_CONFIG_PROFILE` (INFO 日志级别)
- 所有预埋的 PROFILE_SCOPE 热点统计

### Python 依赖

无额外依赖。Profile stage 使用 Python 标准库。

## 使用方法

### 管线运行

```bash
# 单 chunk profile
cd tests/e2e/translation
python -m verification.chunk_pipeline \
  --assembly System.Private.CoreLib \
  --chunk numerics \
  --stages build,profile \
  --native-config profile

# profile + benchmark 联合
python -m verification.chunk_pipeline \
  --assembly System.Private.CoreLib \
  --chunk numerics \
  --stages build,fact,profile,benchmark \
  --native-config profile
```

### 独立运行

```bash
# 直接运行 entry.exe
./entry.exe --profile

# 输出格式:
# {"profile":[
#   {"methodIndex":0,"gcPauseNs":1250000,"maxGcPauseNs":480000,
#    "gcPauseCount":3,"nurseryAllocBytes":4096,
#    "oldGenAllocBytes":0,"largeObjBytes":0,
#    "allocCount":42,"heapBefore":16777216,"heapAfter":16781312,
#    "fastPathCount":40,"slowPathCount":2},
#   ...
# ]}
```

### GC 健康检查

```bash
# 读取 profile.json 执行检查
python -m verification.analysis.gc_check chunks/numerics/results/profile.json

# 从 stdin 读取
cat chunks/numerics/results/profile.json | python -m verification.analysis.gc_check
```

### 基线管理

```bash
# 建立基线 (在 profile stage 运行后)
python -c "
from verification.analysis.perf_baseline import PerfBaseline
import json
bl = PerfBaseline()
profile = json.load(open('results/foundation-dll/System.Private.CoreLib/numerics/perf/profile-history.jsonl'))
bl.establish('System.Private.CoreLib', 'numerics', profile_data)
"
```

### 回归检测

```bash
python -c "
from verification.analysis.perf_compare import compare_profiles, print_comparison
bl = json.load(open('results/.../baseline.json'))
target = json.load(open('results/.../profile.json'))['summary']
result = compare_profiles(bl, target)
print_comparison(result)
"
```

### 聚合报告

```bash
# 单 chunk 报告
python -m verification.analysis.regression_report System.Private.CoreLib --slug numerics
```

## 数据格式

### profile.json

```json
{
  "exitCode": 0,
  "nativeConfig": "profile",
  "entryCount": 121,
  "profileData": [
    {
      "methodIndex": 0,
      "gcPauseNs": 1250000,
      "maxGcPauseNs": 480000,
      "gcPauseCount": 3,
      "nurseryAllocBytes": 4096,
      "oldGenAllocBytes": 0,
      "largeObjBytes": 0,
      "allocCount": 42,
      "heapBefore": 16777216,
      "heapAfter": 16781312,
      "fastPathCount": 40,
      "slowPathCount": 2
    }
  ],
  "summary": {
    "methodCount": 121,
    "totalGcPauseNs": 3750000,
    "maxGcPauseNs": 480000,
    "totalNurseryAllocBytes": 4096,
    "totalAllocCount": 42,
    "fastPathRate": 0.9524,
    "methodsWithGc": 3,
    "methodsWithAlloc": 10,
    "codeTextBytes": 28194048,
    "codeDataBytes": 1048576
  },
  "sectionSizes": {
    "text": 28194048,
    "data": 1048576,
    "pdata": 524288,
    "total": 30720000
  }
}
```

### GC 健康检查阈值

| 检查 | 阈值 | 说明 |
|------|------|------|
| GC-01 | maxGcPauseNs > 5ms | 单次 GC 暂停过长 |
| GC-02 | gcPauseCount > 100 | 方法内频繁 GC |
| GC-03 | fastPathRate < 80% | Bump cache miss 过多 |
| GC-04 | heapDelta/alloc > 2.0 | 堆碎片化迹象 |
| GC-05 | allocCount > 10000 | 分配次数异常 |

## 实现细节

### 采集点

| 位置 | 采集内容 |
|------|---------|
| `profile_stats.h/.cpp` | thread_local ProfileData 累加器 + 环形缓冲区 |
| `gc_region.cpp` (NurseryAllocateSlow) | Slow path + Large Object + GC pause timing |
| `gc_alloc_stubs.cpp` (GcAllocateProfiled) | Nursery alloc + fast path + alloc count |
| `code_size_tracker.py` | dumpbin /HEADERS + .map file 解析 |
| Template: `--profile` flag | heap_before/after + FlushThreadProfileData |

### 编译开关

- 默认 (check/ship): profile_stats 编译为 no-op stubs
- Profile 构建 (`--preset profile`): 启用完整采集
- 运行时通过 `--profile` flag 触发

## 已知限制

1. 依赖 cmake --preset profile 构建；check 构建下运行 --profile 输出空数据
2. .map 文件解析只有 MSVC link.exe 格式；llvm-lld 需要单独适配
3. 多线程 dispatch 下 thread_local 数据在 Flush 时自动合并

## 跨技术栈 GC 对比

### managed_benchmark + benchmark_report

```bash
# 运行 managed benchmark（自动采集 gcInfo）
cd tests/e2e/translation
python -m verification.chunk_pipeline \
  --assembly System.Private.CoreLib \
  --chunk buffers \
  --stages managed_benchmark,benchmark_report

# 验证 gcComparison 输出
python -c "
import json
d = json.load(open('System.Private.CoreLib/chunks/buffers/results/comparison.json'))
with_gc = [m for m in d['methods'] if m.get('gcComparison')]
print(f'gcComparison: {len(with_gc)}/{len(d[\"methods\"])} methods')
for m in with_gc[:5]:
    gc = m['gcComparison']
    print(f'  {gc[\"net8AllocBytes\"]}B col0={gc[\"net8CollectionCount0\"]}')
"
```

### profile_compare.py

```bash
python -m verification.analysis.profile_compare \
  --aot-profile chunks/buffers/results/profile.json \
  --managed-jsonl results/foundation-dll/System.Private.CoreLib/buffers/perf/benchmark-history.jsonl \
  --metadata chunks/buffers/managed/subjects/subjects.metadata.json \
  --output chunks/buffers/results/profile-comparison.json \
  --threshold 2.0
```

标记 AOT 分配 > JIT 2x 的方法。

## 已知限制

1. `entry.exe --profile` 在 check build 下不产生真实数据（stubs 输出空 `{"profile":[]}`）。需使用 `--native-config profile`。
2. AOT profile 数据目前只采集 allocation count 和 bytes，GC pause 信息需要 TLAB/GC 压力测试场景。
3. 部分 chunk（numerics、system）因 codegen 问题无法编译，限制统计覆盖范围。
