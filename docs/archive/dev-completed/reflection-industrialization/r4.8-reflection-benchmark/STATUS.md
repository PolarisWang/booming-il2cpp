# R4.8 — 反射 Benchmark 基线

> **task_id**: r4.8-reflection-benchmark
> **parent_task_id**: reflection-industrialization
> **source_task_id**: reflection-industrialization
> **source_relation**: roadmap-child
> **clearance_source**: parent-roadmap
> **phase**: completed
> **创建日期**: 2026-05-23
> **完成日期**: 2026-05-23
> **entry_skill**: dev-executing-plans

blocking_questions: []
question_clearance: cleared
clearance_confirmed_by_user: true

## Scope

创建反射性能基准测试，覆盖主要查询路径的性能基线。

## Implementation

创建 `testing/src/native/runtime-core/reflection_benchmark.cpp`

使用 GoogleTest benchmark 模式（简单循环计时），注册 mock 模块并通过 ModuleRegistry 句柄调用真实反射 API：

1. **Type flag queries**: IsArray, IsEnum, IsPublic, IsAbstract, IsSealed, IsValueType, IsInterface — 各 100000 次
2. **GetFields** — 10000 次
3. **GetMethods** — 10000 次
4. **GetProperties** — 10000 次
5. **GetElementType** — 10000 次
6. **Type.GetTypeFromHandle** — 100000 次（覆盖 query handle / module handle / raw token 三种路径）

## Exit Criteria

- [x] 基准测试文件创建 `testing/src/native/runtime-core/reflection_benchmark.cpp`
- [x] CMakeLists.txt 注册 `reflection_benchmark` target (labeled "benchmark;core")
- [x] 构建通过
- [x] 6 test cases all pass

## 基线数据 (RelWithDebInfo, x64)

```
--- Type Flag Queries (100k iterations each) ---
  ChaosReflectionGetIsArray (class, false)        100000 iters      1075 us total      10.8 ns/iter
  ChaosReflectionGetIsArray (array, true)         100000 iters      1077 us total      10.8 ns/iter
  ChaosReflectionGetIsEnum (enum, true)           100000 iters      1071 us total      10.7 ns/iter
  ChaosReflectionGetIsEnum (class, false)         100000 iters      1075 us total      10.8 ns/iter
  ChaosReflectionGetIsPublic (true)               100000 iters      1067 us total      10.7 ns/iter
  ChaosReflectionGetIsAbstract (false)            100000 iters      1082 us total      10.8 ns/iter
  ChaosReflectionGetIsSealed (enum, true)         100000 iters      1071 us total      10.7 ns/iter
  ChaosReflectionGetIsValueType (false)           100000 iters      1034 us total      10.3 ns/iter
  ChaosReflectionGetIsInterface (false)           100000 iters      1070 us total      10.7 ns/iter

--- GetFields (10k iterations) ---
  ChaosReflectionGetFields (module handle)         10000 iters       143 us total      14.3 ns/iter

--- GetProperties (10k iterations) ---
  ChaosReflectionGetProperties (module handle)     10000 iters       141 us total      14.1 ns/iter

--- GetMethods (10k iterations) ---
  ChaosReflectionGetMethods (module handle)        10000 iters       250 us total      25.0 ns/iter

--- GetElementType (10k iterations) ---
  ChaosTypeGetElementType (array -> ElementType)    10000 iters       432 us total      43.2 ns/iter
  ChaosTypeGetElementType (class, returns 0)       10000 iters       226 us total      22.6 ns/iter

--- Type.GetTypeFromHandle (100k iterations) ---
  ChaosReflectionGetTypeFromHandle (query handle)   100000 iters       114 us total       1.1 ns/iter
  ChaosReflectionGetTypeFromHandle (module handle)   100000 iters       106 us total       1.1 ns/iter
  ChaosReflectionGetTypeFromHandle (raw token)    100000 iters       103 us total       1.0 ns/iter
```

### 分析

- **Type flag queries (~10.8 ns/iter)**: 极快，因为走 ModuleRegistry Tier-0 type_flags[] 位运算，无间接调用
- **GetFields/GetProperties (~14 ns/iter)**: 快，走 descriptor 路径直接读取静态 constexpr 数组
- **GetMethods (~25 ns/iter)**: 次之，比 GetFields 慢 ~1.8x，因额外遍历 parent chain（即使 parent=nullptr 也有模块扫描开销）
- **GetElementType (~43 ns/iter)**: 最慢路径，涉及字符串拼接和跨模块名称扫描
- **GetTypeFromHandle (~1.1 ns/iter)**: 极快，query handle 走直接指针解码，module handle 走 module lookup

## Files

- `testing/src/native/runtime-core/reflection_benchmark.cpp` — benchmark 实现
- `testing/src/native/runtime-core/CMakeLists.txt` — target 注册 (reflection_benchmark)
