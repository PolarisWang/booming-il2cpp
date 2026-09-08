# Nightly Runner Refactor — STATUS

## Phase
`brainstorming(2026-09-08) → design(route-3 subprocess-per-chunk) → writing-plans → **implementation completed(2026-09-08)**`

## 🔄 重大转向（2026-09-08）—— 方案 D / Route 3 取代旧 nightly_runner 进程内模型

上一轮实现的 `verification/nightly_runner/`（进程/线程池 + 局部 import verification.stages.run_*）在 Windows multiprocessing spawn 下每个 worker 重 import `__main__` 时无法解析 `verification` 包（ModuleNotFoundError），且单 stdout 无心跳 → 假卡死/无法定位。经 brainstorm 裁决改走 **方案 D + chunk_pipeline --provided-stages（Route 3）**，删旧 nightly_runner、新建 `verification/nightly/`。

**决定文件（authority）**：
- 设计：`design-v1-02-subprocess-per-chunk.md`
- 实施计划：`plan-v1-01.md`
- 风险评估：`risk-assessment.md`
- 本 STATUS.md 记录当前状态

## 当前结论（实现完成 + 已推送 origin/main）
Phase 0/1/2 + Phase 3 核心验证已完成并合入 main。

### 已实现（Route 3）
`tests/e2e/verification/nightly/`（10 文件，取代 nightly_runner）：
| 文件 | 职责 |
|------|------|
| `config.py` | NightlyConfig（retry_on_error 按错误类、oom_auto_downgrade、resume_run_id…） |
| `worklist.py` | 遍历 foundation → namespace-partition → (asm, slug)；纯 manifest 驱动 |
| `state.py` | 心跳(.heartbeat)/结果(.result)分离文件；`read_all_results(for_run_id=…)`；error_class 分类器 |
| `observer.py` | 每 chunk 子进程 stdout → run.log；utf-8+replace(GBK 安全)；daemon reader |
| `watchdog.py` | 心跳超时 + 子进程树存活判定 + OOM(仅137)检测/降级 |
| `run.py` | subprocess farm + phase 分批(a: build+fact → b: benchmark+coverage) + `--provided-stages build,fact` + retry 策略 |
| `resume.py` | 读 PRIOR run state -> skip passed（修复读错 run_id 的 L2） |
| `aggregate.py` | 汇总 chunk 结果 → nightly-result.json + nightly-summary.md；错误分 translation-defect/infra/code |
| `cli.py` | 入口 `python -m verification.nightly.cli` |

`chunk_pipeline.py` 增加 `--provided-stages` + provenance(git-commit) 守护（Provenance check）、`coverage-audit` 名确认。

### 已删除
- `verification/nightly_runner/`（整目录）
- `tests/.../test_nightly_orchestrator_phaseB.py`、`test_soak_log_robustness.py`

## 测试
| 测试 | 结果 |
|------|------|
| `tests/e2e/verification/tests/test_nightly_state.py`（error_class 分类器, real-failure 文本 mock） | ✅ 14 passed |
| `tests/e2e/verification/tests/test_nightly_observer.py`（GBK + heartbeat） | ✅ 2 passed |
| Nightly 包 import + worklist discovery(82 chunks × 27 asm) | ✅ |
| 单 assembly 端到端 (System.Collections.NonGeneric) | ✅ Phase A build+fact PASS; Phase B benchmark/managed_benchmark/coverage-audit 经 --provided-stages PASS; hotupdate → hotupdate-patch-arm(不盲目重试) |

## 三优先级
- P1 性能：进程级隔离真并行; 状态文件低成本。
- P2 架构：单引擎(chunk_pipeline)单源; 编排层薄; structured state/error_class 定位准。
- P3 热更：不涉及。

## blocking_questions
`[]`
## question_clearance
`cleared`
## clearance_confirmed_by_user
`true`

## 下一步（如果续跑全量）
```bash
cd tests/e2e
# 全量 27 个 family（Phase A + B）
CHAOS_FOUNDATION_DLL=$PWD/translation python -m verification.nightly.cli --max-workers 8 --verbose

# resume 续跑
CHAOS_FOUNDATION_DLL=$PWD/translation python -m verification.nightly.cli --resume <run_id>

# 单 assembly 快速验证
python -m verification.nightly.cli --assembly System.Collections.NonGeneric --max-workers 2
```
> 注：全量跑过一次 82 chunk 基线（Phase A: 72/82 pass；~5 chunk 因 codegen/ATG/SYSLIB 已知翻译缺陷 fail，见 risk-assessment P1-4），本 STATUS 反映引擎本身稳定可复跑——真实翻译缺陷独立于 nightly 引擎。