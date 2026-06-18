# 内存优化全流程方案

## 一、当前状态诊断

### 1.1 数据流分析

```
fast-path alloc (gc_alloc_stubs.h)
  │
  ├── tls_alloc_fast_bytes++     ← 95%+ 的分配走此路径
  │
  └── FlushTlsFastStats() → g_gc_stats (GC 收集时触发)
      非实时，不更新 tls_total_allocated_bytes

slow-path alloc (gc_region.cpp)
  │
  └── tls_total_allocated_bytes++   ← 仅 OOM/GC 触发时走此路径

benchmark (runtime-entry.cpp)
  │
  └── chaos_gc_get_allocated_bytes_for_current_thread()
       → 读取 tls_total_allocated_bytes → 几乎总是 0
```

### 1.2 Root Cause

**`chaosAotAllocBytes = 0` 不是因为不分配，而是因为读错了计数器。**

快速路径分配（`GcAllocateFast`）使用 `tls_alloc_fast_bytes`，但 benchmark 读取的 `chaos_gc_get_allocated_bytes_for_current_thread()` 返回的是 `tls_total_allocated_bytes`——后者只在慢速路径中更新。

### 1.3 现有基础设施评估

| 组件 | 状态 |
|------|------|
| TLS fast-path counters | ✅ 已存在 (`tls_alloc_fast_bytes`) |
| TLS total counter | ✅ 已存在但未关联 fast-path |
| benchmark alloc 采样 | ✅ (读取 `tls_total_allocated_bytes`) |
| `gcComparison` 报告管道 | ✅ (`profile_compare.py`) |
| .NET 8 baseline 数据 | ✅ (`net8AllocBytes`) |
| 对比逻辑 | ✅ (`aotAllocVsNet8Pct`) |

**缺的只有一行代码**：让 `chaos_gc_get_allocated_bytes_for_current_thread()` 包含 `tls_alloc_fast_bytes`。

---

## 二、Phase 1: 数据修复（预估 1 天）

### Step 1: 修复分配统计

修改 `gc_api.cpp` 的 `chaos_gc_get_allocated_bytes_for_current_thread()`：

```cpp
// gc_alloc_stubs.h — 暴露快速路径计数器
extern thread_local CHAOS_IL2CPP_SIZE tls_alloc_fast_bytes;

// gc_api.cpp — 让总计数器包含快速路径
extern thread_local CHAOS_IL2CPP_SIZE chaos::il2cpp::runtime_core::tls_alloc_fast_bytes;

extern "C" CHAOS_IL2CPP_INT64 CHAOS_RUNTIME_ABI_CALL 
chaos_gc_get_allocated_bytes_for_current_thread() noexcept
{
    // 快速路径 + 慢速路径 = 真实分配总量
    return tls_total_allocated_bytes + 
           static_cast<CHAOS_IL2CPP_INT64>(chaos::il2cpp::runtime_core::tls_alloc_fast_bytes);
}
```

### Step 2: 验证修复

```bash
# 跑一轮 Immutable chunk 验证数据
python testing/foundation-dll/verification/chunk_pipeline.py \
    --assembly System.Collections.Immutable --all-chunks --mode standard
```

预期结果：`chaosAotAllocBytes` 从 0 变为正数，`aotAllocVsNet8Pct` 显示真实对比。

### Step 3: 建立基线

```python
# 修复后跑全量 benchmark，生成基线报告
python testing/foundation-dll/verification/nightly_build.py \
    --native-config ship --skip-nightly-report
```

基线报告格式：
| Family | 方法数 | chaos_alloc | net8_alloc | 对比 | 
|--------|--------|-------------|------------|------|
| System.Linq | 339 | 1.2MB | 0.9MB | +33% |
| System.Data.Common | 953 | 4.5MB | 3.2MB | +40% |
| ... | ... | ... | ... | ... |

---

## 三、Phase 2: 热点定位（1 天）

### 3.1 自动标记问题方法

`comparison-summary.json` 已含 `gcComparison` 字段。添加一个新的分析脚本 `memory_hotspots.py`：

```python
# memory_hotspots.py — 按分配量排序标记问题方法

LEVELS = {
    "P0": lambda r: r["aotAllocVsNet8Pct"] > 200,   # Chaos > 2x .NET8
    "P1": lambda r: r["aotAllocVsNet8Pct"] > 100,   # Chaos > 1x .NET8
    "P2": lambda r: r["chaosAotAllocBytes"] > 1_000_000,  # 绝对量 > 1MB
    "P3": lambda r: r["chaosAotGcPauseNs"] > 100_000,     # GC暂停 > 100μs
}
```

### 3.2 优先级排序矩阵

```
高分配量 + 高比率 = P0  → 立即优化
高分配量 + 低比率 = P1  → 检查是否有优化空间
低分配量 + 高比率 = P2  → 关注但不紧急
低分配量 + 低比率 = P3  → 无需处理
```

### 3.3 输出报告格式

```json
{
  "family": "System.Linq",
  "methods": [
    {
      "methodSubjectId": "...",
      "chaosAotAllocBytes": 524288,
      "net8AllocBytes": 1024,
      "ratio": 51200,
      "priority": "P0",
      "bottleneck": "boxing_in_loop",
      "optimization": "GenericShapeDescriptor"
    }
  ]
}
```

---

## 四、Phase 3: 优化策略库（持续）

### 4.1 常见分配模式与修复方案

| 模式 | 检测 | 修复方案 |
|------|------|---------|
| **装箱 (boxing)** | IL 中大量 `box` 指令 | GenericShapeDescriptor 路由到原生 helper，避开装箱路径 |
| **临时数组** | `newarr` + 短生命周期 | 栈上分配 (`alloca`) 或线程本地数组池 |
| **string 拼接** | 多个 `string.Concat` | StringBuilder 重用或 `Span` 操作 |
| **集合扩容** | `List.Add` 循环 | 预分配容量 (`List<T>(n)`) |
| **委托分配** | `newobj` + 委托 | 缓存委托实例，避免重复分配 |
| **闭包 (closure)** | 编译器生成的 `<Main>b__0` 类 | 静态委托 + 状态参数 |
| **LINQ** | `Enumerable.Where().Select()` | IL  level 优化或直接 for 循环替代 |
| **异常分配** | `newobj Exception` | 缓存异常实例或使用结果码 |

### 4.2 优化验证流程

```
1. 定位热点方法 (memory_hotspots.py)
2. 分析 IL 确定瓶颈模式
3. 实施修复:
   - codegen 层: GenericShapeDescriptor (如 Interlocked 模式)
   - 运行时层: 对象池 / 缓存 / 栈上分配
   - 模板层: Scriban 模板优化
4. benchmark 验证:
   - chaosAotAllocBytes 下降
   - aotAllocVsNet8Pct 趋近 0%
   - 执行时间不退化
5. 更新 comparison-summary
```

### 4.3 预期优化效果

| 模式 | 典型降低 | 方法数占比 |
|------|---------|-----------|
| 去装箱 | 60-90% | ~15% 方法 |
| 数组池 | 40-70% | ~5% 方法 |
| 委托缓存 | 50-80% | ~10% 方法 |
| 集合预分配 | 20-40% | ~8% 方法 |

---

## 五、实施路线图

```
Phase 1: 修复统计 (1天)
  ├── 修改 gc_api.cpp (1行)
  ├── 验证 Immutable chunk
  └── 跑全量 baseline

Phase 2: 热点定位 (1天)
  ├── 开发 memory_hotspots.py
  ├── 生成 P0/P1 列表
  └── 确认优先修复目标

Phase 3: 逐个优化 (2-4周)
  ├── Week 1: P0 方法（装箱 + 临时数组）
  ├── Week 2: P1 方法（string + 集合）
  └── Week 3-4: P2 方法 + 回归验证

持续: 每轮 benchmark 自动检查退化
```

## 六、推荐

**立即开始 Phase 1 Step 1** — 修复 `chaos_gc_get_allocated_bytes_for_current_thread()` 的计数器读取。这是一行代码改动，能立刻让所有内存对比数据变得有意义。有了真实数据后，Phase 2 和 Phase 3 的优化决策才有依据。
