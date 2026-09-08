# Nightly Runner 重构 — 方案 D 设计（subprocess-per-chunk 编排层）

> 状态：brainstorming → **design 待审**
> Supersede：`design-v1-01.md` 的 nightly_runner 进程内编排（已被方案 D 取代）
> 决策来源：2026-09-08 brainstorm —— 用户选 方案 D + phase 分批编排(a) + 替换旧 nightly_runner + 结构化 error_class

---

## 一、目标（验收口径）

把 nightly build 从"单进程 stddout 编排"改成 **"每 chunk 一个 chunk_pipeline 子进程 + orchestrator 只做派发/状态/聚合"**，使 nightly 能**稳定**运行、出问题时**立刻定位**、且可 **resume 续跑**。

三诉求对应验收：
1. **稳定**：单个 chunk 的崩溃/卡死不拖垮整轮；Windows 无 spawn/GBK 坑；有看门狗
2. **立刻定位**：每 chunk 独立日志 + 结构化 state + error_class 分类，`grep`/`tail` 即得（非翻 stdout）
3. **续跑**：state 文件跳过已完成（passed）chunk，中断可 resume

---

## 二、核心架构

```
tests/e2e/verification/nightly/        ← 新模块（取代 nightly_runner/，后者删除）
├── __init__.py
├── config.py        NightlyConfig（max_workers, stages, native_config, foundation_root...）
├── worklist.py      遍历 foundation_root→assembly dirs→读 _dll/namespace-partition.json→列出所有 (asm, slug)
├── run.py           subprocess farm：flink 一批 chunks → 每 chunk Popen(chunk_pipeline...) → 收集 exit + 状态
├── watchdog.py      每 chunk 心跳；超时/无子进程存活→判 stalled→kill→记 error_class=timeout
├── observer.py      实时 tee 每 chunk stdout→logs/<asm>/<slug>/ {stage}.log；写 state
├── resume.py        读 state/，跳 passed
├── aggregate.py     读每 chunk 已写 _dll/reports/latest/fact-summary.json → 收敛 family/delta/summary
└── cli.py           入口（取代 nightly_runner.main）
```
旧 `nightly_runner/{main,orchestrator,report_collector,log_manager,config}.py` **删除**（被取代）；`chunk_pipeline` 保持纯引擎不动。唯一 engine = `chunk_pipeline`。唯一编排 = `nightly`。

## 架构图
```
nightly/cli.py
   └─ plan 阶段  → worklist.py: (asm,slug) list from namespace-partition
   └─ exec 阶段(分批 a) 
        phase A  = chunk_pipeline --stages build,fact            (全 part 并行)
        phase B  = chunk_pipeline --stages benchmark,managed_benchmark,hotupdate,coverage_audit  (全 part 并行)
        max_workers → 并发 Popen 上限（进程级）
        └── observer.py: 每 chunk Popen 的 stdout → tee logs/<asm>/<slug>/run.log + 控制台一行"进度"
        └── watchdog.py: 心跳监控每 chunk Popen 子进程树
        └── 每 chunk 完成 → 写 state/<run>/<asm>__<slug>.json
   └─ 收尾 → aggregate.py: 读各 chunk _dll reports → 汇总 + delta + nightly report
```

## phase 批(jitter a)
```
[已定] phase A → phase B 分两个批次（先全 build+fact，再 benchmark/coverage）。
      理由：Phase A 全 fact 绿 = Phase B benchmark 前置准入（与 pipeline-config 现有排序一致）。
```

---

## 三、状态/可观测（这是方案 D 与 v1.01 最大区别）

### state 文件（每 chunk 实时，取代"先跑完才在 stdout 打一行"）
```
nightly-build-report/run-state/<run_id>/<asm>__<slug>.json
{
  "phase":"A","stage_filter":["build","fact"],
  "status":"running|passed|failed|stalled|skipped",   ← 运行中即写 running+心跳
  "pid":1234, "exit_code":null,
  "error_class":"none|atg-combined-cs|csharp-error|native-crash|timeout|env-missing",
  "chunk_dir":"...", "started":"...","updated":"...",
  "last_heartbeat":"...",   ← watchdog 每秒更新
  "raw_tail":"..."           ← 末 20 行 error 文本（供 error_class 提取）
}
```
### error_class 分类器（结构化，取代"10 failed 只是计数"）
提取条件按 precedence：
| error_class | 识别依据 |
|---|---|
| `atg-combined-cs` | chunk_pipeline 的 combined 构建失败、stderr 含 `CombinedSubjects.cs(...)error CS` **且** 在 `managed/combined` 下 |
| `native-crash` | exit code 0xC0000005 / 0xC0000409 / SEH / "Internal CLR error" / linkage 崩 |
| `csharp-error` | `error CS####` 但不属 combined-ATG（native subjects 编译错） |
| `timeout` | watchdog 判 stalled（>阈值 + 无子进程存活） |
| `env-missing` | 找不到 foundation/chunk / namespace-partition 缺失 / DllNotFound |
| `pass` | exit 0 + phase 通过 |

分类器是一个独立函数 `classify_exit(exit_code, tail_text, chunk_dir) -> error_class`，单测覆盖。=> 解决"非 unified list、无法归类、无法重试分 layer"。

### 每 chunk 独立进程日志（取代全局 --no-logs stdout + GBK 坑）
- observer：每 chunk 一个 `logs/<run>/<asm>/<slug>/{phase}.log`，subprocess 读时 `encoding='utf-8', errors='replace'`（根治 GBK UnicodeDecodeError）
- 主控台只打一行汇总/进度，不打 10000 行 stage print（根治"stdout 淹没""假卡死误判"）
- 定位问题 = `tail -f logs/<asm>/<slug>/..log` + `grep error_class=* state/*.json`

### watchDog / 心跳
- observer 里每 chunk 维护 `last_heartbeat`（有 stdout 输出即更新）
- watchdog 周期扫描：若某 running chunk 心跳停滞 > `max_idle`(默认 2×stage_timeout 或可配) **且**其 Popen 子进程树无存活 → 判定 stalled：kill、记 error_class=timeout、继续下一 chunk
- 根治：无总看门狗、单个 chunk 挂死拖垮整批

---

## 四、config.py 新增项
```
report_dir,foundation_dir(foundation_root),native_config,stages,max_workers     (同旧)
max_idle_seconds=1200          # watchdog stalled 阈值
error_class_retry_once=True     # 对 atg-combined-cs / env 类尝试一次 rebuild(仍 fail 则记双标记)
resume_run_id=None              # 续跑
```

---

## 五、chunk_pipeline 是否需要改动？

倾向 **几乎不碰 chunk_pipeline**（它已能单 chunk 跑绿）。方案 D 把 nightly_runner 里 import `stages.run_*` 的重复 stage 编排逻辑**整个删除**，改由 orchestrator 调 `chunk_pipeline` 子进程。所以 nightly_runner 那 ~29KB orchestrator + 16KB report_collector 不迁移——只留下其 config 思想。

唯一需要在 orchestrator 侧校验的：chunk_pipeline 对 benchmark/managed_benchmark/hotupdate 是否都是 `--stages ...` 可达（已确认 usage 支持 `--stages build,fact,hotupdate,coverage-audit` 逗号多值；需确认 benchmark/managed_benchmark 也在其 stage 白名单）。若某 stage（如 managed_benchmark / profile / benchmark_report）不在 chunk_pipeline 白名单 → 需扩展 chunk_pipeline 白名单或方案 D 显式串一串子命令。**此为本设计最大开放风险，落地前需 spike。**

---

## 六、phase 内并发上限
- 用**进程并发闸**（`max_workers`），不是 Python Pool——直接维护 Popen 集合，每完成一个再放一个进（简单 queue + worker counter）
- 避免 ProcessPoolExecutor spawn + 避免 ThreadPool 共享副作用
- 手动 subprocess farm 反而是本设计 root 去除 Process/Thread pool 的目标

---

## 七、report/result 聚合（aggregate.py）契约
- 每 chunk 跑完，`chunk_pipeline` 已写 `<foundation>/<asm>/_dll/reports/latest/fact-summary.json`（已验证存在,含 totalPassed/capabilityAggregate）
- aggregate.py：遍历全部 (asm,slug) 的该文件 → family 汇总 → nightly-delta vs 上一 run 的 state/delta
- 复用现有 nightly_delta/nightly_summary 或将它们归并入 aggregate（保留原脚本名，调用它们）
- nightly-build-report/<run_id>/ 结构沿用旧 design 但由新模块写

---

## 八、删除与兼容（替换而非并存）
| 项 | 动作 |
|---|---|
| `verification/nightly_runner/*` | 删除（被 nightly/ 取代） |
| `tests/e2e/verification/tests/test_nightly_orchestrator_phaseB.py` | 迁移为自测新 nightly/run+state(而非测旧 orchestrator) |
| `verification/nightly_build.py`, nightly_delta.py, nightly_summary.py | 保留(它们是被调用脚本,方案D aggregate 调它们或合并) |
| CI 入口 | 旧 `python -m verification.nightly_runner.main` → `python -m verification.nightly.cli` |
| `stages/*.py` | 不动（chunk_pipeline 是引擎） |

---

## 九、风险
| 风险 | 概率 | 影响 | 缓解 |
|---|---|---|---|
| chunk_pipeline stage 白名单不含 benchmark/managed/…（需 spike 确认） | 中 | 方案D phase B 跑不了 | 落地前 spike：对一个小 chunk `chunk_pipeline --chunk global-ns --stages benchmark` 试跑 |
| 每 chunk 冷启动成本(2-5s×82) | 低 | 慢几秒 | 可接受 |
| 删 nighty_runner/ 破坏引用 | 中 | 回归 | 先 grep 引用迁移；测试随新模块重写 |
| watchdog 误杀长 benchmark | 中 | 误 kill | max_idle 可配且强制要求子进程存活才算 stalled（非仅无输出） |
| 多 run 同写 foundation dirs | 低 | 冲突 | nightly 串行；resume 也串行 |

---

## 十、三优先级
- P1 性能：每 chunk 真并行(进程)，天然多核；无 log 全局开销
- P2 架构：单引擎单源 + 编排层薄 + 结构化 state/error_class，职责清晰
- P3 热更新：不涉及

---

## 十一、落地顺序（writing-plans 阶段展开）
1. spike：确认 chunk_pipeline `--stages` 覆盖 build/fact/benchmark/managed_benchmark/hotupdate/coverage-audit（若缺口→扩白名单）
2. 逐文件建 nightly/：config → worklist → run(subprocess farm) → observer/log → watchdog/state → resume → aggregate → cli
3. 迁移测试到新模块；删除旧 nightly_runner；CI 入口切换
4. 用真实小 assembly(foundation→NonGeneric)端到端验证 + 与 Phase A(B 5 failed) 对照
