# Nightly Runner 重构 — 设计 v1

## 背景

原 `nightly_build.py` + `ci_smoke.py` 存在以下问题：
- 报告分散在 4 个不同目录（`results/nightly/`、各 `_dll/reports/`、`results/foundation-dll/`、chunk 级 `results/`）
- 无日志留存（stage 的 print 输出跑完即消失）
- Jenkins 集成需要统一提取点
- 无运行时元数据（git commit、触发方式、运行参数）

## 设计目标

1. **Jenkins 友好**：单入口点 + 单一 `nightly-build-report/<date>/` 目录可供提取
2. **全量执行**：所有 22+ assemblies × 所有 8 阶段一次跑通
3. **日志完整**：per-stage/per-chunk 的详细日志文件，含 subprocess 原始输出
4. **向后兼容**：现有 `nightly_build.py` + stage 文件保持不变

## 架构

```
verification/nightly_runner/
├── __init__.py           ← 导出 NightlyConfig, NightlyResult
├── config.py             ← NightlyConfig dataclass
├── orchestrator.py       ← 从 nightly_build.py 提取执行引擎
├── log_manager.py        ← TeeStream + ChunkLogManager
├── report_collector.py   ← 报告聚合（delta/summary/日志/元数据）
└── main.py               ← Jenkins 入口点
```

### 模块职责

| 模块 | 职责 | 关键设计点 |
|------|------|-----------|
| `config.py` | 集中管理所有运行参数 | dataclass + 从环境变量注入 Jenkins 元数据 |
| `orchestrator.py` | 发现 chunks + 并行执行 + profile pass | 从 nightly_build.py 提取 `_run_chunk_stages` + `_discover_assemblies` |
| `log_manager.py` | 每个 worker 进程内 Tee 截获 | context manager 接管 sys.stdout/sys.stderr；注入 `CHAOS_NIGHTLY_LOG_DIR` 环境变量 |
| `report_collector.py` | 聚合所有产出到统一目录 | copy + 符号链接 latest；写入 git/jenkins 元数据 |
| `main.py` | CLI 入口 | argparse + 从 `os.environ` 自动检测 Jenkins 变量 |

### 日志机制

```
主进程 stdout → CI 控制台（keep_console_output=True）
                          
ProcessPoolExecutor worker
  ├── ChunkLogManager.__enter__()
  │   ├── sys.stdout = TeeStream(file=pipeline.log, console=True)
  │   └── sys.stderr = TeeStream(file=pipeline.log, console=True)
  │
  ├── os.environ["CHAOS_NIGHTLY_LOG_DIR"] = "{report_dir}/logs/{asm}/{slug}/"
  │
  ├── stage_fn(ctx, stages)   ← 每个 stage 内部检测 CHAOS_NIGHTLY_LOG_DIR
  │   ├── print("...")        → Tee → pipeline.log + 控制台
  │   ├── subprocess.run()    → stdout/stderr 追加到 {stage}.log
  │   └── ...
  │
  └── ChunkLogManager.__exit__()
      └── sys.stdout/stderr 恢复
```

### 报告目录结构

```
nightly-build-report/
└── YYYY-MM-DD_HHMMSS-{run_id}/
    ├── summary/
    │   ├── nightly-delta.json
    │   ├── nightly-summary.md
    │   └── pipeline-status.json
    ├── reports/<assembly>/          ← _dll/reports/latest/ 的副本
    ├── per-chunk/<asm>/<slug>/      ← chunk 级结果文件
    ├── benchmark-history/<asm>/<slug>.jsonl
    ├── logs/
    │   ├── main.log
    │   └── <asm>/<slug>/            ← {stage}.log + pipeline.log
    └── metadata/
        ├── git-info.json
        ├── jenkins-info.json
        ├── config.json
        └── duration.json
latest -> ./YYYY-MM-DD_HHMMSS-{run_id}/
```

### 向后兼容策略

| 现有文件 | 策略 |
|----------|------|
| `nightly_build.py` | 保留不变 |
| `nightly_delta.py` | 保留不变 |
| `nightly_summary.py` | 保留不变 |
| `stages/*.py` | 每个文件加 ≤3 行检测 `CHAOS_NIGHTLY_LOG_DIR` env var |
| `test_nightly_build.py` | 新增 `test_nightly_runner.py` |

## 风险评估

| 风险 | 概率 | 影响 | 缓解 |
|------|------|------|------|
| 多进程 Tee 文件竞争 | 低 | 日志乱序 | 每个 worker 写独立文件 + 文件名含 pid |
| 符号链接在 Windows 上不可用 | 中 | latest 目录失效 | 回退为 copy，+ 检测平台 |
| stage internal refactoring 破坏原有测试 | 低 | 回归 | 从 nightly_build.py 提取的纯函数保持签名不变 |

## 三优先级权衡

| 维度 | 评估 |
|------|------|
| P1 性能 | 日志 IO 在 worker 进程中写本地文件，不阻塞主 pipeline 的 ProcessPoolExecutor |
| P2 架构 | 模块职责分离清晰；现有文件不做大改；新模块完全覆盖旧入口 |
| P3 热更新 | 不涉及 |

## 边界拍板

- `nightly-build-report/` 默认在 repo root 下生成（可被 `.gitignore` 忽略）
- Jenkins 提取方式：`nightly-build-report/latest/` 符号链接
- 日志保留策略：保留最近 7 天（由 Jenkins 侧管理，非本模块职责）
- 每个 stage 的日志写入由子进程自己的 print 触发，不对 stage 做侵入式修改
