# Foundation-DLL Verification 包

## 概述

`verification/` 是 foundation-dll **chunk-based 验证管线**的 Python 包。它将目标 DLL 的 public 方法按 namespace 拆分为多个 chunk，对每个 chunk 独立执行 codegen + TPG + native build + fact 验证 + benchmark。

## 快速开始

```bash
cd testing/foundation-dll

# 运行单个 chunk（完整流程）
python -m verification.chunk_pipeline --chunk numerics

# 运行所有 chunk（当前 assembly 的全部 chunks）
python -m verification.chunk_pipeline --all-chunks

# 指定阶段
python -m verification.chunk_pipeline --chunk system --stages build,fact,coverage-audit

# 严格模式（coverage 缺口 >5% 会使 stage 失败）
python -m verification.chunk_pipeline --chunk numerics --mode strict

# CI smoke（CoreLib 快速验证）
python verification/ci_smoke.py
python verification/ci_smoke.py --mode=full                    # core + standard families
python verification/ci_smoke.py --mode=extended                 # all families
python verification/ci_smoke.py --families System.Linq,System.Net.Http  # 指定 family

# 离线：扫描 DLL 生成 chunk 划分
python -m verification.stages.manifest --dll <path_to_dll>

# 缓存管理
python -m verification.chunk_pipeline hephaestus stats
python -m verification.chunk_pipeline hephaestus clear

# 运行单元测试
python -m pytest verification/tests/ -v
```

## 目录结构

```
verification/
  __init__.py              # 包声明
  _path.py                 # sys.path 设置
  chunk_pipeline.py        # 统一 CLI 入口 + 管线编排
  ci_smoke.py              # CI 入口（分级: smoke/full/extended）

  orchestration/           # 编排层
    context.py             # ChunkContext / StageResult / UnifiedReport 数据模型
    discovery.py           # chunk 发现（读 namespace-partition.json）

  stages/                  # 10 阶段执行器
    build.py               # ATG → subjects DLL → TPG → entry.exe
    fact_chunk.py          # entry.exe --fact-json 验证 + AOT/JIT 交叉对比
    hotupdate_chunk.py     # ATG patch → patch DLL → PatchDataExtractor → hotupdate 验证
    benchmark_chunk.py     # entry.exe --benchmark-all（自适应迭代/采样）
    managed_benchmark.py   # CombinedSubjects.dll 在 net8.0/net10.0 JIT 下跑 benchmark
    benchmark_report.py    # 跨技术栈对比（chaos-aot vs net8-jit vs net10-jit vs chaos-jit）
    profile.py             # entry.exe --profile GC/分配/code size 数据
    coverage_audit.py      # 验证 chunk subject 覆盖了声明的方法列表
    aggregate.py           # 汇总所有 chunk 结果到 _dll/reports/
    reporting.py           # 生成 AI 可用报告 + 跨 assembly 看板
    hephaestus_cache.py    # chunk 级构建缓存（SHA-256 key, LRU 淘汰）
    manifest.py            # 独立工具：扫描 DLL → dll-method-manifest.json + namespace-partition.json

  preflight/               # 预检
    check_layer_boundaries.py  # 四层架构红线检查

  analysis/                # 分析工具
    benchmark_scanner.py   # benchmark 扫描
    code_size_tracker.py   # code size 追踪
    gc_check.py            # GC 健康检查
    perf_baseline.py       # 基线管理
    perf_compare.py        # 性能对比
    profile_compare.py     # profile 对比
    regression_report.py   # 回归报告

  reporting/               # 报告生成
    dashboard.py           # Dashboard 数据
    dashboard_renderer.py  # HTML 渲染
    models.py              # 数据模型

  soak/                    # 长期压力测试
    orchestrator.py        # 24h soak 编排
    report.py              # soak 报告

  tests/                   # 单元测试
    conftest.py
    test_context.py
    test_discovery.py
    test_benchmark_diff.py

  benchmark_diff.py        # 跨 run benchmark 差异对比
  p3_cleanup.py            # Phase 3 数据清理
```

## 10 阶段管线

| # | 阶段 | 依赖 | 说明 |
|---|------|------|------|
| 1 | build | — | ATG codegen → subjects DLL → TPG → entry.exe（Hephaestus cache 加速） |
| 2 | fact | build | entry.exe --fact-json + entry-jit.exe --fact-json, AOT/JIT 交叉差异检查 |
| 3 | hotupdate | build, fact | patch 生成 → PatchDataExtractor → baseline/patched/revert 三态验证 |
| 4 | benchmark | build, fact | entry.exe --benchmark-all N, 自适应迭代+采样 |
| 5 | managed_benchmark | build | net8.0/net10.0 JIT baseline, mark known-unsafe benchmarks |
| 6 | benchmark_report | benchmark, managed_benchmark | chaos-aot vs net8-jit vs net10-jit vs chaos-jit 对比 |
| 7 | coverage_audit | build, fact | 验证 chunk 声明方法被 ATG 覆盖（strict 模式 >5% 缺口 = fail） |
| 8 | profile | build | entry.exe --profile GC pause/alloc/code size |
| 9 | aggregate | build, fact, coverage-audit | 汇总所有 chunk → _dll/reports/ |
| 10 | reporting | aggregate | AI-ready 报告 → results/pipeline-runs.jsonl |

## 缓存

Hephaestus Cache（`.hephaestus-cache/`）保存 chunk build 产物：
- **Cache key**: SHA-256（subjects DLL + metadata + 依赖 DLL + **ATG/TPG 工具二进制** + runtime stubs + pipeline 脚本）
- **LRU 淘汰**: 最多 50 个 entry
- **命令**: `python -m verification.chunk_pipeline hephaestus {stats|clear|invalidate|prune}`

## 详细文档

- [验证 Gate 流程](../../../wiki/06-测试验证/FactStatic-Native验证Gate流程.md)
- [故障排除指南](../../../wiki/06-测试验证/foundation-dll-pipeline-troubleshooting.md)
