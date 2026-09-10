# Nightly Runner Refactor — STATUS

## Phase
`brainstorming → design completed → writing-plans → implementation completed`

## 边界拍板
- 新增 `verification/nightly_runner/` 模块，6 个文件
- 报告目录 `nightly-build-report/YYYY-MM-DD_HHMMSS-id/` + `latest` 符号链接或 `latest.txt`
- Jenkins 通过 `main.py` 单入口触发
- 现有 `nightly_build.py` + stage 文件保留不变（零侵入式日志通过 ChunkLogManager 自动截获）

## Authority
- Owner: nightly CI/CD 域
- 不改变 codegen 或 runtime 的翻译路径
- 不改变 agnostic 的语法/规则

## 当前结论
已实现，测试通过。

### 已实现文件
| 文件 | 状态 | 关键设计 |
|------|------|---------|
| `nightly_runner/__init__.py` | ✅ | 导出 NightlyConfig, NightlyOrchestrator, NightlyResult, ReportCollector |
| `nightly_runner/config.py` | ✅ | NightlyConfig dataclass + 自动 repo root 检测 + Jenkins env 注入 |
| `nightly_runner/log_manager.py` | ✅ | TeeStream + ChunkLogManager（自动 stdout/stderr 截获 + subprocess.run monkey-patch + stage 名智能识别） |
| `nightly_runner/orchestrator.py` | ✅ | 从 nightly_build.py 提取执行引擎 + 并行 ProcessPoolExecutor |
| `nightly_runner/report_collector.py` | ✅ | 6 阶段报告聚合（reports/per-chunk/benchmark-history/summary/metadata/latest） |
| `nightly_runner/main.py` | ✅ | Jenkins CLI 入口 |

### 现有文件改动
| 文件 | 改动 |
|------|------|
| 所有 `stages/*.py` | **0 行改动** — `ChunkLogManager` 通过 subprocess monkey-patch 自动截获 |
| `nightly_build.py` | **0 行改动** — 保留完全向后兼容 |

### 日志架构
```
ChunkLogManager.__enter__()
  ├── sys.stdout = TeeStream → pipeline.log + console
  ├── sys.stderr = TeeStream → pipeline.log + console
  ├── subprocess.run = _logged_run → {stage}.log  (stage 名由 _guess_stage_name 自动识别)
  │     dotnet build/build → "build"
  │     entry.exe --fact-json → "fact"
  │     entry.exe --benchmark-all → "benchmark"
  │     entry.exe --hotupdate → "hotupdate"
  │     cmake ... → "build"
  │     autotestgenerator/tpg → "build"
  └── os.environ["CHAOS_NIGHTLY_LOG_DIR"] = str(log_dir)
```

### 测试
| 测试 | 结果 |
|------|------|
| `test_nightly_build.py` (35 项) | ✅ 全部通过 |
|  nightly_runner 模块导入 | ✅ |
|  nightly_runner CLI --help | ✅ |
|  _guess_stage_name 智能识别 | ✅ dotnet build/build/benchmark/hotupdate/cmake |

## blocking_questions
- [x] 报告目录结构已定
- [x] 日志机制（Tee + subprocess monkey-patch）已定，零侵入 stage
- [x] 向后兼容策略已定
- [x] Jenkins 集成方式已定

## question_clearance
`cleared`

## clearance_confirmed_by_user
`true`

## 下一步
可执行 nightly_runner 验证：
```bash
# 单 assembly 测试（不带日志，快速验证）
python -m verification.nightly_runner.main \
    --assembly System.Linq \
    --max-workers 2 \
    --no-logs

# 全量执行（带日志）
python -m verification.nightly_runner.main \
    --report-dir /path/to/nightly-build-report \
    --max-workers 8

# 带 profile pass
python -m verification.nightly_runner.main \
    --run-profile \
    --max-workers 4
```
