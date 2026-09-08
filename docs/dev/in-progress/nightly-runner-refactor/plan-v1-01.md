# Nightly Runner 重构 — 实施计划 v1（Route 3：chunk_pipeline --provided-stages）

> 上游设计：`design-v1-02-subprocess-per-chunk.md`
> 风险版本：`risk-assessment.md`（所有风险缓解措施已整合入本 plan）
> 落地策略：夜间模块替换 + 分步验证

---

## 一、实施阶段总览

```
Phase 0: 实现 chunk_pipeline --provided-stages + provenance 护网
Phase 1: 新建 nightly/ 模块（文件逐件落地，含风险缓解措施）
Phase 2: 迁移测试 + 已分类真实失败文本 mock + 删除旧 nightly_runner
Phase 3: CI 入口切换 + 全量验证 + 对比旧 run 10 failed 清单
```

每阶段独立可验证。阶段间依赖：Phase 1 依赖 Phase 0 结果；Phase 2 依赖 Phase 1；Phase 3 依赖 Phase 2。

---

## 二、Phase 0：实现 chunk_pipeline `--provided-stages`

**目标**：给 chunk_pipeline.py 加一个 flag，让 Phase B 能跳过已由 Phase A 产出的 stages（build+fact），同时用 provenance.json 做信任护栏。

### 改动（chunk_pipeline.py，~30 行）

| 位置 | 改动 | 风险缓解 |
|------|------|---------|
| argparse | 加 `--provided-stages`（逗号分隔 stage 名列表） | — |
| DAG 校验 | `stage_set = set(stage_names) \| provided_set`，provided 的 stages 满足 dep 但不执行 | — |
| 执行循环 | 遍历 `stage_names` 时跳过 provided 的 stage | — |
| 护网 P1-1 | 若 `--provided-stages` 包含 `build` 或 `fact`，检查 `chunk_dir / results / provenance.json` 是否存在且 `git_commit` 匹配当前 HEAD。**不匹配 → 报错 + 自动降级为重传 full stages 重跑**（不盲信 provenance） | ⚡ P1-1 |

> chunk_pipeline 其余逻辑（toposort / hephaestus / result 写入）**完全不动**。

### 验证（集成）
```bash
# 1. 先正常跑 Phase A
cd tests/e2e && PYTHONPATH=$PWD python -m verification.chunk_pipeline \
  --assembly System.Collections.NonGeneric --chunk global-ns \
  --stages build,fact

# 2. 用 --provided-stages 跑 Phase B
cd tests/e2e && PYTHONPATH=$PWD python -m verification.chunk_pipeline \
  --assembly System.Collections.NonGeneric --chunk global-ns \
  --stages benchmark,managed_benchmark,hotupdate,coverage_audit \
  --provided-stages build,fact

# 3. 验证 DAG 校验通过、benchmark 正常读 fact 产物、provenance 护网不触发
```

---

## 三、Phase 1：新建 nightly/ 模块（10 个文件，风险缓解措施已嵌入）

### 文件清单

```
tests/e2e/verification/nightly/
├── __init__.py          约 10 行
├── config.py            约 100 行 — 含所有风险缓解配置项
├── worklist.py          约 60 行 — 遍历 foundation_root → (asm,slug) list
├── state.py             约 120 行 — 状态读写（心跳/结果分离文件，防 P1-3）
├── observer.py          约 150 行 — 日志 + 心跳（PYTHONUTF8=1 防 P1-2，daemon 线程防 P3-2）
├── watchdog.py          约 80 行 — 看门狗 + OOM 检测（P2-1）+ exit 137 降级
├── run.py               约 200 行 — subprocess farm + error_class 分类器（P1-4）+ 重试策略
├── resume.py            约 40 行 — 续跑
├── aggregate.py         约 180 行 — 报告聚合 + translation-defect 区分（P1-5）
└── cli.py               约 80 行 — 入口
```

**总计新增**：约 1020 行 Python（含注释、空行）。

### 逐文件详细规格（含风险缓解嵌入）

#### 1. `__init__.py`
- 导出：`NightlyConfig`, `NightlyResult`, `discover_worklist`, `run_phases`, `aggregate_reports`
- 行内 import 不触发外部依赖

#### 2. `config.py`
```python
@dataclass
class NightlyConfig:
    foundation_dir: Path
    report_dir: Path
    run_id: str
    max_workers: int = 4
    native_config: str = "check"
    stages: list[str] = field(default_factory=lambda: ["build","fact","benchmark","managed_benchmark","hotupdate","coverage_audit","aggregate"])
    max_idle_seconds: int = 1200

    # ⚡ P1-4：重试策略按 error_class 区分（默认只对 transient 重试）
    retry_on_error: dict[str, bool] = field(default_factory=lambda: {
        "timeout": True,          # 可能是机器负载波动，重试一次
        "native-crash": True,     # 可能是启动竞态，重试一次
        "env-missing": True,      # 可能是环境变量临时问题
        "atg-combined-cs": False, # 真实 ATG 缺陷，重试浪费
        "native-codegen-missing-symbol": False,  # 真实翻译缺陷
        "native-codegen-syntax": False,          # 真实 codegen bug
        "platform-diagnostic": False,            # 需 Suppress 的编译器诊断
        "csharp-error": False,    # 真实编译错误
        "unknown": True,          # 未分类→ 重试一次来确认
    })
    # 重试次数上限，防无限重试
    max_retries_per_chunk: int = 1

    # ⚡ P2-1：OOM 自动降级
    oom_auto_downgrade: bool = True  # 连续 2 个 OOM 则 max_workers //= 2

    resume_run_id: str | None = None
    assembly_filter: list[str] | None = None
    profile_pass: bool = False
    verbose: bool = False
```

#### 3. `worklist.py`
- `discover_worklist(config) -> list[WorkItem]`
- 复用 `verification/orchestration/discovery.py` 的 `discover_chunks` 函数

#### 4. `state.py`（含 P1-3 缓解）
```python
# ⚡ P1-3：心跳和状态结果分离文件，消除并发写入竞争
# 文件结构：
#   state/<run_id>/<asm>__<slug>.heartbeat   ← observer 每 5 秒更新（只写 updated 时间戳 + pid）
#   state/<run_id>/<asm>__<slug>.result      ← run.py 主线程写一次（passed/failed + error_class）

# 写入/读取都 .get() 默认值（P3-1 缓解）

# error_class 分类规则（P1-4 + P2-5 严格 precedence from narrow to broad）：
# 优先级(高→低):
# 1. "native-codegen-missing-symbol":  tail 含 "chaos_type_" + "undeclared identifier" + "error C2"
# 2. "native-codegen-syntax":          tail 含 "native-aot.generated.cpp(...)error C" + (C2061|C2143|C3536)
# 3. "atg-combined-cs":                tail 含 "CombinedSubjects.cs" + "error CS"
# 4. "platform-diagnostic":            tail 含 "error SYSLIB"
# 5. "native-crash":                   exit_code in (0xC0000005, 0xC0000409) 或 "Internal CLR error"
# 6. "csharp-error":                   "error CS" 但不属上述
# 7. "timeout":                        watchdog 判定
# 8. "oom":                            exit_code == 137 (SIGKILL)
# 9. "env-missing":                    找不到 foundation/chunk / namespace-partition 缺失
# 10. "pass":                          exit_code == 0 + 所有 stage passed
# 11. "killed":                        exit_code == -9 or -15
# 12. "unknown":                       以上都不匹配
```

#### 5. `observer.py`（含 P1-2 + P3-2 缓解）
```python
class ChunkObserver:
    def __init__(self, item, config):
        self.log_dir = config.report_dir / "logs" / config.run_id / item.assembly / item.slug
        self.log_dir.mkdir(parents=True, exist_ok=True)
        self.heartbeat_path = config.report_dir / "run-state" / config.run_id / f"{item.assembly}__{item.slug}.heartbeat"
        self._reader_thread = None
        self._tail = []  # 最近 20 行
        self._done = threading.Event()

    def attach(self, proc: subprocess.Popen):
        # ⚡ P1-2：读写子进程 stdout 时强制 utf-8 + errors=replace，根治 GBK 崩
        # ⚡ P3-2：reader 线程设 daemon=True，pipe 断裂时自动退出
        def _reader():
            try:
                with open(self.log_dir / "run.log", "w", encoding="utf-8") as f:
                    for line in iter(proc.stdout.readline, b""):
                        text = line.decode("utf-8", errors="replace")
                        f.write(text)
                        f.flush()
                        self._tail.append(text)
                        if len(self._tail) > 20:
                            self._tail.pop(0)
                        # 每 5 秒更新心跳（写独立 .heartbeat 文件，避免与 mark_passed 竞争）
                        # 实际按行数计数节流，不每次写
            except ValueError:
                pass  # pipe closed
            self._done.set()

        self._reader_thread = threading.Thread(target=_reader, daemon=True)
        self._reader_thread.start()
```

#### 6. `watchdog.py`（含 P2-1 缓解）
```python
def start_watchdog(config, running, state_dir, abort_event):
    oom_count = 0

    while not abort_event.is_set():
        for item, (proc, obs) in list(running.items()):
            heartbeat_path = state_dir / f"{item.assembly}__{item.slug}.heartbeat"
            result_path = state_dir / f"{item.assembly}__{item.slug}.result"
            if result_path.exists():
                continue  # 已有结果，跳过

            # 检查心跳超时
            if heartbeat_path.exists():
                hb = json.loads(heartbeat_path.read_text(encoding="utf-8"))
                age = time.time() - hb.get("updated", 0)
                if age > config.max_idle_seconds:
                    # 子进程是否存活？
                    poll = proc.poll()
                    if poll is not None:
                        # 进程已退出但结果未写 → 可能是 write 落后
                        continue
                    # 进程仍活着但心跳超时 → 可能是长 benchmark，不杀
                    # 仅当进程已死且结果未写才判 stalled
                    pass
            # ⚡ P2-1：OOM 检测 + 自动降级
            if proc.poll() == 137:
                oom_count += 1
                if oom_count >= 2 and config.oom_auto_downgrade:
                    new_workers = max(1, config.max_workers // 2)
                    print(f"  [watchdog] OOM detected ({oom_count}x), reducing max_workers to {new_workers}")
                    config.max_workers = new_workers
                    oom_count = 0
        time.sleep(30)
```

#### 7. `run.py`（核心编排，含 P1-4 重试策略 + P1-1 provenance 护网）

```python
def run_phases(config: NightlyConfig, worklist: list[WorkItem]) -> NightlyResult:
    # Phase A: build + fact
    phase_a_items = run_phase("A", ["build","fact"], worklist, config)
    # Phase B: benchmark + coverage（仅 passed 的 items）
    passed_items = [i for i in phase_a_items if i.status == "passed"]
    phase_b_items = run_phase("B",
        ["benchmark","managed_benchmark","hotupdate","coverage_audit"],
        passed_items, config,
        provided_stages="build,fact")  # Route 3
    return aggregate_results(config, phase_a_items + phase_b_items)

def run_phase(phase, stages, items, config, provided_stages=None):
    ...
    while queue or running:
        while len(running) < config.max_workers and queue:
            item = queue.pop(0)
            observer = ChunkObserver(item, config)
            cmd = [
                sys.executable, "-m", "verification.chunk_pipeline",
                "--assembly", item.assembly,
                "--chunk", item.slug,
                "--stages", ",".join(stages),
                "--native-config", config.native_config,
            ]
            if provided_stages:
                cmd.extend(["--provided-stages", provided_stages])
            if config.verbose:
                cmd.append("--verbose")
            # ⚡ P1-2：子进程环境变量注入 PYTHONUTF8=1，治 GBK
            env = os.environ.copy()
            env["PYTHONUTF8"] = "1"
            proc = subprocess.Popen(
                cmd, stdout=subprocess.PIPE, stderr=subprocess.STDOUT,
                cwd=config.foundation_dir.parent, env=env)
            observer.attach(proc)
            ...

        # 轮询完成
        for item, (proc, obs) in list(running.items()):
            ret = proc.poll()
            if ret is not None:
                obs.close()
                tail = obs.tail_text()
                error_class = classify_exit(ret, tail, item.chunk_dir)

                if error_class == "pass":
                    write_result(state.path, status="passed", error_class="none")
                else:
                    # ⚡ P1-4：重试策略按 error_class 区分
                    retry_count = item.retry_count
                    if retry_count < config.max_retries_per_chunk \
                       and config.retry_on_error.get(error_class, False):
                        item.retry_count += 1
                        queue.insert(0, item)  # 重放队列
                        write_result(state.path, status="retrying", error_class=error_class)
                    else:
                        write_result(state.path, status="failed", error_class=error_class, exit_code=ret)
                ...
```

**error_class 分类器**（拒绝简单规则，按可观察根因精确细分）：
```python
def classify_exit(exit_code, tail_text, chunk_dir) -> str:
    # strict precedence: narrow → broad

    # native-codegen-missing-symbol: codegen 生成的符号未声明
    if "chaos_type_" in tail_text and "undeclared identifier" in tail_text and "error C2" in tail_text:
        return "native-codegen-missing-symbol"
    # native-codegen-syntax: codegen 生成的 C++ 语法错（C2061/C2143/C3536 等）
    if "native-aot.generated.cpp" in tail_text and "error C" in tail_text:
        return "native-codegen-syntax"
    # atg-combined-cs: ATG 生成的 CombinedSubjects.cs 编译错
    if "CombinedSubjects.cs" in tail_text and "error CS" in tail_text:
        return "atg-combined-cs"
    # platform-diagnostic: dotnet 编译器诊断（SYSLIB 等）
    if "error SYSLIB" in tail_text:
        return "platform-diagnostic"
    # native-crash: 0xC0000005 / SEH / Internal CLR error
    if exit_code in (0xC0000005, 0xC0000409) or "Internal CLR error" in tail_text:
        return "native-crash"
    # oom: 被 OOM killer 杀
    if exit_code == 137:
        return "oom"
    # 一般 csharp 编译错
    if "error CS" in tail_text and "error CS" not in tail_text:  # 确保不是"no error CS"
        return "csharp-error"
    # pass
    if exit_code == 0:
        return "pass"
    # killed
    if exit_code in (-9, -15):
        return "killed"
    return "unknown"
```

#### 8. `resume.py`
- 读 `state/<run_id>/` 下的 `.result` 文件
- 跳过 `status == "passed"` 的 items
- 打印统计：`"Resume: {n_skip} skipped, {n_remain} remaining"`

#### 9. `aggregate.py`（含 P1-5 + P2-3 缓解）
```python
def aggregate_reports(config, results) -> ReportSummary:
    # 遍历每个 chunk 的 _dll/reports/latest/fact-summary.json
    # 按 error_class 分组汇总：
    summary = {
        "total_passed": 0,
        "total_failed": 0,
        # ⚡ P1-5：区分翻译缺陷 vs 基础设施失败 vs 超时
        "translation_defect_fails": [],   # error_class in (native-codegen-*, atg-combined-cs, platform-diagnostic)
        "infra_fails": [],                # error_class in (env-missing, oom, timeout)
        "code_defect_fails": [],          # error_class in (csharp-error, native-crash, unknown)
        "retried_to_pass": [],            # 重试后变 passed 的 chunk（P2-3）
    }
    # 写入 nightly-build-report/<run_id>/summary/
    # 复用 nightly_delta.py 和 nightly_summary.py（import 调用）
```

#### 10. `cli.py`
```python
def main():
    parser = argparse.ArgumentParser(description="Nightly Build — subprocess-per-chunk engine (Route 3)")
    # 参数: --max-workers, --native-config, --assembly, --resume, --no-profile, --verbose, --foundation-dll, --retry
    config = NightlyConfig.from_args(args)
    worklist = discover_worklist(config) if not config.resume_run_id \
               else build_resume_worklist(config, discover_worklist(config))
    result = run_phases(config, worklist)
    summary = aggregate_reports(config, result)
    print(summary.summary_text())
    return 1 if summary.failed_count > 0 else 0
```

---

## 四、Phase 2：测试 + 删除旧模块

### 测试文件清单（含风险缓解验证）

| 测试文件 | 验证点 | 对应风险 |
|----------|--------|---------|
| `test_nightly_config.py` | NightlyConfig 所有字段、retry_on_error 默认值、from_env | — |
| `test_nightly_state.py` | 心跳/结果分离文件读写、error_class 分类器（**用 raw-output.txt 真实失败文本 mock**：diagnostics 的 C2061、runtime-compiler 的 SYSLIB5007、system-xml-schema 的 CS1503） | ⚡ P1-4, P2-5 |
| `test_nightly_observer.py` | GBK 兼容（写入含 `0x94` 的行）、reader 线程 daemon + pipe 断裂、心跳文件写入 | ⚡ P1-2, P3-2 |
| `test_nightly_watchdog.py` | 心跳超时判定、子进程存活检查、OOM exit 137 检测 + 降级 | ⚡ P2-1 |
| `test_nightly_run.py` | subprocess farm 并发限制、队列消费、**重试策略**（timeout 重试、atg-combined-cs 不重试） | ⚡ P1-4 |
| `test_nightly_resume.py` | 跳过已 passed、跨 run 新增 chunk 包含 | — |
| `test_nightly_aggregate.py` | error_class 分组汇总、translation-defect 区分 | ⚡ P1-5 |

### 删除旧文件
```
verification/nightly_runner/            ← 整目录删除（git 可恢复，历史 RB）
verification/tests/test_nightly_orchestrator_phaseB.py  ← 删除
verification/tests/test_soak_log_robustness.py          ← 删除
```

> ⚡ P2-4 缓解：删除前确认无外部引用（已 grep 确认仅 nightly_runner 内部引用 + 2 测试）。CI 入口切换分两步：先让新 nightly/ 可用但旧入口保留，确认无回归后再删除旧文件。

---

## 五、Phase 3：CI 入口切换 + 全量验证

### 入口切换
```
旧: python -m verification.nightly_runner.main --max-workers 8 --native-config check
新: python -m verification.nightly.cli --max-workers 8 --native-config check
```

### 全量验证步骤
1. **单 assembly 端到端**（Phase A + B）：
   ```bash
   python -m verification.nightly.cli --assembly System.Collections.NonGeneric --max-workers 2 --verbose
   ```
2. **CoreLib 单 chunk**：
   ```bash
   python -m verification.nightly.cli --assembly System.Private.CoreLib --chunk system-3 --stages build,fact
   ```
3. **resume 验证**（中断后续跑）
4. **失败分类验证**（用 xml-schema 等 ATG 错 chunk 验证 error_class 分类）
5. **回归验证**：对比旧 run 的 10 failed 清单与新 run 的 error_class 分类清单，**一致**

---

## 六、风险缓解措施汇总表

| 风险 | 缓解方案 | 落地模块 | 实现难度 |
|------|---------|---------|---------|
| P1-1 provenance 不牢 | `--provided-stages` 护网：校验 provenance.json 存在 + git_commit 匹配，不匹配则自动降级重跑 | chunk_pipeline.py + run.py | 低 |
| P1-2 环境/GBK | Popen env 注入 `PYTHONUTF8=1`；observer 读 stdout 用 `encoding='utf-8', errors='replace'` | run.py + observer.py | 低 |
| P1-3 state 并发 | 心跳(.heartbeat)和结果(.result)分离文件，消除写竞争 | state.py | 低 |
| P1-4 多样失败根因 | error_class 按 12 类细分（含 native-codegen-* / platform-diagnostic）；重试策略按 error_class 区分 | run.py + state.py | 中 |
| P1-5 区分翻译缺陷 | aggregate 按 error_class 分组：translation-defect / infra / code-defect | aggregate.py | 低 |
| P2-1 OOM | exit 137 检测 + 自动降级 max_workers | watchdog.py | 低 |
| P2-2 轮询空转 | 保持 1s sleep，不优化 | run.py | 无 |
| P2-3 失败 chunk | 分阶段汇总 + 重试后 passed 的单独记录 | aggregate.py | 低 |
| P2-4 回退困难 | 两步删除 + git 可恢复 | 流程 | 低 |
| P2-5 分类器误判 | 严格 precedence + 真实文本 mock 单测 | state.py + test | 中 |
| P3-1 schema 变化 | .get() 默认值 | state.py | 低 |
| P3-2 线程泄漏 | daemon=True + pipe 断裂自动退出 | observer.py | 低 |
| P3-3 磁盘膨胀 | Jenkins 侧管理（非本模块职责） | — | 无 |

---

## 七、变更清单汇总

| 操作 | 文件 | 行数 | 说明 |
|------|------|------|------|
| 修改 | verification/chunk_pipeline.py | ~30 | 加 `--provided-stages` + provenance 护网 |
| 新增 | verification/nightly/__init__.py | ~10 | 导出 |
| 新增 | verification/nightly/config.py | ~100 | NightlyConfig + retry_on_error + oom_auto_downgrade |
| 新增 | verification/nightly/worklist.py | ~60 | 发现 worklist |
| 新增 | verification/nightly/state.py | ~120 | 心跳/结果分离文件 + error_class 分类器 |
| 新增 | verification/nightly/observer.py | ~150 | 日志 + 心跳 + GBK 兼容 + daemon 线程 |
| 新增 | verification/nightly/watchdog.py | ~80 | 看门狗 + OOM 检测 + 降级 |
| 新增 | verification/nightly/run.py | ~200 | subprocess farm + 重试策略 + provenance 校验 |
| 新增 | verification/nightly/resume.py | ~40 | 续跑 |
| 新增 | verification/nightly/aggregate.py | ~180 | 报告聚合 + translation-defect 区分 |
| 新增 | verification/nightly/cli.py | ~80 | 入口 |
| 删除 | verification/nightly_runner/ | ~6 files | 整目录 |
| 删除 | verification/tests/test_nightly_orchestrator_phaseB.py | - | 旧测试 |
| 删除 | verification/tests/test_soak_log_robustness.py | - | 旧测试 |
| 新增 | verification/tests/test_nightly_state.py | ~60 | error_class 分类器 + 真实文本 mock |
| 新增 | verification/tests/test_nightly_run.py | ~60 | subprocess farm + 重试策略 |
| 新增 | verification/tests/test_nightly_observer.py | ~50 | GBK 兼容 + daemon 线程 |
| 新增 | verification/tests/test_nightly_watchdog.py | ~50 | 心跳 + OOM 检测 |
| 新增 | verification/tests/test_nightly_resume.py | ~30 | 续跑 |
| 新增 | verification/tests/test_nightly_aggregate.py | ~50 | error_class 分组 |
| 总计 | 新增约 1020 + 测试约 300 = **~1320 行** | | |