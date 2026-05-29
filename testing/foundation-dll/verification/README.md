# Foundation-DLL Verification 包

## 概述

`verification/` 是 foundation-dll 验证管线的 Python 包，负责将 C# 入口点 DLL 通过 il2cpp 翻译为原生可执行文件，直接运行来验证 Assert 全部通过。

## 目录结构

```
verification/
  __init__.py              # 包声明
  _path.py                 # 统一 sys.path 设置

  entry_points/            # 入口点
    cli.py                 # 单 family 验证
    batch.py               # 全量运行
    ci_smoke.py            # CI 快速验证

  orchestration/           # 编排层
    engine.py              # VerificationPipeline 管线引擎
    context.py             # FamilyContext/StageResult 数据模型
    family_entrypoint.py   # codegen 自动生成入口
    # (dispatch 代码生成已迁移到 TestProjectGenerator emit)

  stages/                  # 13 阶段执行器
    preflight.py           # Stage 0: Contract 完整性检测
    codegen.py             # Stage 1-2: AOT/JIT 构建
    fact.py                # Stage 3-4: Fact 验证
    audit.py               # Stage 5: 机制 + 原则审计
    asm_compare.py         # Stage 6: 指令对比
    microbench.py          # Stage 7: 微基准
    benchmark.py           # Stage 8: 性能对比
    hotupdate.py           # Stage 9-12: HotUpdate
    principle/             # 原则审计规则包

  reporting/               # 报告生成
    dashboard.py           # Dashboard 数据逻辑
    dashboard_renderer.py  # HTML 渲染
    perf.py                # 性能历史 dashboard
    batch_summary.py       # 批量摘要

  analysis/                # 分析工具
    perf_store.py          # 性能数据持久化
    perf_baseline.py       # 基线管理
    perf_compare.py        # 性能对比
    gc_check.py            # GC 正确性检查
    semantic_check.py      # 语义模式检查

  soak/                    # 长期压力测试
    orchestrator.py        # 24h soak 编排
    report.py              # soak 报告

  tests/                   # 单元测试
    conftest.py
    test_context.py
    test_engine.py
    test_preflight.py
    test_soak.py
```

## 使用方法

```bash
# 单 family 验证（标准模式）
python -m verification.entry_points.cli convert-char --assembly System.Private.CoreLib

# 严格模式（hotupdate 必须通过）
python -m verification.entry_points.cli convert-char --assembly System.Private.CoreLib --mode strict

# 跳过某些阶段
python -m verification.entry_points.cli convert-char --skip jit_codegen benchmark hotupdate

# 全量运行
python -m verification.entry_points.batch

# CI smoke（4 families, 6 阶段快速模式）
python -m verification.entry_points.ci_smoke

# 运行测试
python -m pytest tests/ -v
```

## 13 阶段管线

| # | 阶段 | 说明 |
|---|------|------|
| 0 | preflight | Contract 完整性检测 |
| 1 | codegen | AOT 构建 (chaos-il2cpp → cmake → entry-aot.exe) |
| 2 | jit_codegen | JIT 构建 (entry-jit.exe) |
| 3 | fact | Fact AOT 验证 |
| 4 | fact_jit | Fact JIT 验证 |
| 5 | audit | 机制 + 原则审计 |
| 6 | asm_compare | JIT vs AOT 指令对比 |
| 7 | microbench | 微基准 (Interpreter) |
| 8 | benchmark | 三端性能对比 |
| 9 | hotupdate | HotUpdate AOT Fact |
| 10 | hotupdate_aot_benchmark | HotUpdate AOT 性能 |
| 11 | hotupdate_jit_fact | HotUpdate JIT Fact |
| 12 | hotupdate_jit_benchmark | HotUpdate JIT 性能 |

## 详细文档

- [验证 Gate 流程](../../../wiki/06-测试验证/FactStatic-Native验证Gate流程.md)
- [故障排除指南](../../../wiki/06-测试验证/foundation-dll-pipeline-troubleshooting.md)
