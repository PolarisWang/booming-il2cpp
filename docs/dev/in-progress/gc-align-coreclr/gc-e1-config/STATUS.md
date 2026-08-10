# STATUS — GC-E1 Config

## 元信息

```yaml
task_id: gc-align-coreclr/gc-e1-config
task_type: plan
phase: completed
roadmap_or_plan: docs/dev/in-progress/gc-align-coreclr/gc-e1-config/design-e1-v1-01.md
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

GC 配置旋钮体系对齐 CoreCLR `gcconfig.h` 的 `GC_CONFIGURATION_KEYS` 宏式单表声明，**确保跨平台 + AOT/JIT 适配**。首批核心旋钮走 env 覆盖 + native 程序化调参双通道。

## 对齐 CoreCLR + 跨平台 + AOT/JIT

| CoreCLR 源 | CRAG 等价 | 跨平台 |
|-----------|----------|--------|
| env 变量 (`gcServer`/`GCHeapHardLimit`) | `CHAOS_GC_<EnvSuffix>` 经 `std::getenv` | ✅ std 标准 |
| managed 开关 (`System.GC.*`) | native API `chaos_gc_config_set_*` + `_get_*`（AOT/JIT codegen 可调） | ✅ 纯 C 接口 |
| `GCConfig::Get{name}()` | `GcConfigImpl` struct 字段 + `GcConfig()` 单例 | ✅ |

**跨平台**：纯 `std::getenv`，无平台代码。
**AOT/JIT**：`chaos_gc_config_*` C 符号，AOT(静态)与 JIT 都能链同一运行时单例。

## 实现

### 新增 `gc/gc_config.h` + `gc/gc_config.cpp`
- `CHAOS_GC_CONFIGURATION_KEYS` 宏表（首批 7 个核心旋钮：DefaultNurserySize / DefaultGen1Size / MaxTlabAlloc / LohThreshold / ParallelMarkWorkers / HeapHardLimitMB / HeapSoftLimitMB）。
- `GcConfigImpl::Initialize()`：`std::getenv("CHAOS_GC_" ENV)` 覆盖编译期默认；`CHAOS_IL2CPP_SIZE` 非负解析。
- native API：`chaos_gc_config_get_nursery_size()` / `set_nursery_size()` / `set_heap_hard_limit_mb()` / `set_heap_soft_limit_mb()`。

### 接线 `gc_region.cpp InitYoungGeneration()`
- 顶部调 `GcConfig().Initialize()`。
- 硬/软限从 `GcConfig().HeapHardLimitMB/SoftLimitMB` 读（保留编译期 `#if` 作 fallback）。
- nursery/gen1 大小从 `GcConfig().DefaultNurserySize/DefaultGen1Size` 读（替代硬编码 `kDefaultYoungRegionSize`）。

### CMake
- 添加 `gc/gc_config.h` + `gc/gc_config.cpp`。

## 验证

| 项 | 结果 |
|----|------|
| `chaos_runtime_core` Debug 编译 | ✅ |
| **env 功能验证** | ✅ `CHAOS_GC_NurserySize=16777216` 后 `young_collector_test` nursery 区间 64MB→16MB（`[0x...0040, 0x...40040)` → 16MB） |
| GC 单测批（region/atomic/young_collector/gen1/loh/old_gen/scheduler） | ✅ 全部 0 failures |

## 已提交

- commit（填）

## Exit Criteria

- [x] `chaos_runtime_core` Debug 编译通过
- [x] `CHAOS_GC_NurserySize=...` 生效（InitYoungGeneration 用新值）
- [x] native `chaos_gc_config_*` 程序化调参可达（AOT/JIT codegen 可链）
- [x] GC 确定性单测无回归

## 后续（本次未做，记录）

- `MaxTlabAlloc` / `LohThreshold` / `ParallelMarkWorkers` 已入 config 表但**尚未接线**到热路径（涉及 `gc_alloc_stubs.cpp`/`gc_loh.cpp`/`gc_parallel_mark.cpp` 多处，属独立小改动，避免污染本次已验证范围）。列为本子任务后续或独立小任务。
