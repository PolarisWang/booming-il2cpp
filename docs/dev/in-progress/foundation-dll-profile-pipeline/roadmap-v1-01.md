# Roadmap — Foundation-DLL Profile Pipeline

## 目标

为 foundation-dll 验证管线新增 Profile Stage，补全内存 Profile、代码体积追踪、分析模块和多家族覆盖四大能力短板，建立从**数据采集 → 基线对比 → GC 健康检查 → 优化报告**的完整优化闭环。

## 范围

- 新增 C++ `profile_stats.h/.cpp` 实时采集 GC pause、allocation、code size
- 新增 `entry.exe --profile` 入口
- 新增 `stages/profile.py` 编排
- 新增 `analysis/` 4 个模块（perf_baseline / perf_compare / gc_check / regression_report）
- 新增 `batch_manifest.py` 多家族覆盖
- 修改 `chunk_pipeline.py` / `context.py` / `aggregate.py` 集成

## 非目标

- 不修改现有 benchmark stage 的接口和数据格式
- 不修改 codegen 输出
- 不涉及 GC 本身的行为改进（那是 Phase 3+ 工业化的事）
- 不修改 CI smoke 的默认stage列表

## 调度策略

```yaml
dispatch_model: sequential
child_execution_mode: auto
auto_continue: true
auto_stop_policy: blocking-only
```

**串行原因**: Phase 1-3 存在强数据依赖（采集 → 基线 → 分析），Phase 4 可独立但放在最后以减少上下文切换。

## 阶段列表

### Phase 1 — C++ 采集 + Profile Stage 编排 (核心)

| 字段 | 内容 |
|------|------|
| goal | entry.exe 能输出完整的 profile 数据，且 profile stage 可以在管线中编排 |
| exit_criteria | `python -m verification.chunk_pipeline --chunk numerics --stages profile` 输出完整 profile.json |
| deliverables | `profile_stats.h`, `profile_stats.cpp`, `stages/profile.py`, `analysis/gc_check.py` |
| dependencies | cmake --preset profile 可用 (已有) |
| resolved_decisions | --profile 是 --benchmark-all 框架的 flag，不是独立入口 |
| watch_items | Profile stage 耗时是否超出预期 (目标 < 60s per chunk) |

### Phase 2 — 基线管理 + 对比检测

| 字段 | 内容 |
|------|------|
| goal | 可以建立 profile 基线并检测跨版本回归 |
| exit_criteria | `perf_compare.py --baseline DATE1 --target DATE2` 输出回归报告 |
| deliverables | `analysis/perf_baseline.py`, `analysis/perf_compare.py` |
| dependencies | Phase 1 产生 profile data |
| resolved_decisions | 基线数据格式沿用 JSONL |
| watch_items | 基线建立首次需要在 "干净" 状态下运行 |

### Phase 3 — Per-function Code Size + 聚合报告

| 字段 | 内容 |
|------|------|
| goal | 可以追踪每 chunk 的 .text/.data 变化趋势，并输出可读优化报告 |
| exit_criteria | profile 输出包含 per-function code size；`regression_report.py` 输出人类可读报告 |
| deliverables | map file parser, `analysis/regression_report.py`, aggregate 扩展 |
| dependencies | Phase 1 + Phase 2 |
| resolved_decisions | Code size 用 linker .map 实现 per-function，fallback section 级 |
| watch_items | map file 解析在 MSVC lld/link 差异 |

### Phase 4 — 多家族覆盖

| 字段 | 内容 |
|------|------|
| goal | 至少 5 个除 S.P.C 外的 family 有 namespace-partition.json 且可跑管线 |
| exit_criteria | `batch_manifest.py --all` 成功生成 5+ 家族的 partition，且至少 3 个通过 build+fact |
| deliverables | `batch_manifest.py`, 各家族的 namespace-partition.json |
| dependencies | 无（独立于 Phase 1-3） |
| resolved_decisions | 默认 max-methods-per-chunk=500, 与 S.P.C 一致 |
| watch_items | 部分 family 有无 .NET SDK 兼容性问题 |

## 子任务映射

| task_id | phase | status | owner | purpose | depends_on | batch_id | requirements | deliverables | exit_criteria | conflict_scope | estimated_effort |
|---------|-------|--------|-------|---------|------------|----------|--------------|--------------|---------------|----------------|-----------------|
| t01-cpp-profile-stats | 1 | planned | main | C++ 侧 profile_stats.h/.cpp + --profile flag | 无 | batch-1 | thread_local 采集器 + --profile 路由 | `profile_stats.h`, `profile_stats.cpp`, entry.cpp 修改 | entry.exe --profile 输出 GC pause + alloc + heap delta | `src/native/runtime-core/` | 2d |
| t02-stage-profile | 1 | planned | main | Python 侧 profile stage 编排 | t01 | batch-1 | stages/profile.py + context.py 扩展 | `stages/profile.py` | `python -m verification --chunk numerics --stages profile` 通过 | `verification/stages/` | 1d |
| t03-gc-check | 1 | planned | main | GC 健康检查模块 | t02 | batch-1 | 消费 profile.json 做 4 项健康检查 | `analysis/gc_check.py` | gc_check.py 对已知问题 chunk 输出告警 | `verification/analysis/` | 0.5d |
| t04-perf-baseline | 2 | planned | main | 基线管理模块 | t02 | batch-2 | 建立/查询/更新基线 | `analysis/perf_baseline.py` | 可以为 numerics chunk 建立并查询基线 | `verification/analysis/` | 0.5d |
| t05-perf-compare | 2 | planned | main | 对比检测模块 | t04 | batch-2 | 跨基线回归检测 | `analysis/perf_compare.py` | 能检测 GC pause 回归 | `verification/analysis/` | 0.5d |
| t06-code-size | 3 | planned | main | Per-function code size | t01 | batch-3 | .map parser + profile.json 扩展 | map parser, profile code size 字段 | profile.json 包含 per-function code size | `verification/stages/` | 1d |
| t07-regression-report | 3 | planned | main | 聚合优化报告 | t05+t06 | batch-3 | 整合 GC/性能/体积 → 人类可读 | `analysis/regression_report.py`, aggregate 扩展 | 输出包含"优化建议"的 report json | `verification/analysis/` | 1d |
| t08-batch-manifest | 4 | planned | main | 多家族批量扫描 | 无 | batch-4 | 遍历 + 生成 + 验证 5 家族 | `batch_manifest.py`, 5+ namespace-partition.json | 3+ 家族通过 build+fact | `testing/foundation-dll/` | 1.5d |

## 依赖图

```
t01 ─→ t02 ─→ t03      (Phase 1)
                │
                ├→ t04 ─→ t05           (Phase 2)
                │            │
                │            └─→ t06 ─→ t07   (Phase 3)
                │
t08  (独立)                               (Phase 4, 可与其他并行)
```

## 风险

| 风险 | 影响阶段 | 概率 | 缓解 |
|------|---------|------|------|
| thread_local 采集增加 dispatch 开销 | Phase 1 | 低 | PROFILE_SCOPE 启用时才采集，默认零开销 |
| map file 格式变化 | Phase 3 | 低 | fallback 到 section 级 |
| 多家族 DLL 找不到 | Phase 4 | 中 | fallback 链 DOTNET_ROOT → local → prompt |
| --profile flag 与 --benchmark-all 输出冲突 | Phase 1 | 低 | 用不同的 JSON root key 区隔 |

## 备选路径

- **如果 Phase 1 发现 thread_local 采集开销不可接受**：回退到进程级采样（每 N 次 dispatch 采样一次），放弃 per-method 精度
- **如果 .map 解析 2 天未完成**：先实现 dumpbin /HEADERS section 级追踪，Phase 3 完成后再迭代 per-function
- **如果多家族 3 家以上无法通过 build+fact**：只生成 partition，在 pipeline-config.yaml 中标记为 `experimental`

## 当前建议推进顺序

```
Phase 1: t01 → t02 → t03 (串行)
Phase 2: t04 → t05 (串行)
Phase 3: t06 → t07 (串行)
Phase 4: t08 (独立)
```

## 子任务执行策略

```yaml
child_execution_mode: auto
auto_continue: true
auto_stop_policy: blocking-only
recommended_next_child: t01-cpp-profile-stats
```
