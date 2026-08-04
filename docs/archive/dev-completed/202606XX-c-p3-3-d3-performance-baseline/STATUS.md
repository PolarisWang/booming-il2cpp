# C-P3-3: D3 性能基线 — 编译时间 + 码体积追踪

> 父任务: 20260523-codegen-industrialization
> source_task_id: C-P3-3
> source_relation: roadmap-child
> 创建日期: 2026-05-23

---

## Phase

`completed`

## 最近摘要

2026-05-23: C-P3-3 D3 Performance baseline 完成。创建 `perf_baseline.py`，集成到 `aggregate.py`：

1. **码体积基线**: 扫描生成 C++ 文件大小、编译后 entry.exe 大小、.text section 大小
2. **GC slot 计数**: 从生成 C++ 解析 GC slot map entries
3. **方法计数**: 从生成 C++ 估计函数数
4. **编译时间**: 从 pipeline codegen stage 读取 duration
5. **基线存储**: 写入 `testing/results/verification-history/<assembly>/<slug>/baseline-perf.json`
6. **回归检测**: 比较当前 vs 基线，支持可配置阈值（默认: C++ 大小 +15% warning / +30% critical, 码大小 +10%/+25%, 时长 +20%/+50%）

完成文件:
- 新建: `_core/python/perf_baseline.py`
- 修改: `_core/python/aggregate.py`（集成基线采集 + 回归检测）

## Type

`plan`

## 设计摘要

### 边界拍板

- **目标**: 建立 codegen 性能基线（编译时间 + 码体积），支持回归检测
- **范围**: `testing/foundation-dll/_core/python/perf_*.py` + `testing/results/`
- **非目标**: 不修改 codegen 逻辑、不修改 pipeline 阶段、不做 microbenchmark

### 方案

1. **码体积基线**: 扫描生成 C++ 文件大小 + 编译后 native 二进制段大小
2. **编译时间基线**: 测量 codegen 阶段 elapsed time
3. **基线存储**: 按 family 存储到 `testing/results/verification-history/`
4. **回归检测**: 在 aggregate 阶段比较当前 vs 基线，超阈值报警

### Authority 决策

- 属于 Thread C (HT) Phase 3 任务
- 无严格前置依赖（C-P3-1 已完成）
- 冲突域: `testing/`

---

## 问题清零

```yaml
blocking_questions: []
question_clearance: cleared
clearance_confirmed_by_user: true
clearance_source: parent-roadmap
```

## 下一步

1. 创建 `perf_baseline.py` — 码体积 + 编译时间采集
2. 创建 `perf_baseline_config.json` — 阈值配置
3. 测试: 对 convert-char 采集基线
4. 归档
