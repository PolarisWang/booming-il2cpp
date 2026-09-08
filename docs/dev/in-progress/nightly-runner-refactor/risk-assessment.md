# Nightly Runner 重构 — 风险评估报告

> 基于 design-v1-02-subprocess-per-chunk.md + plan-v1-01.md（Route 3）
> 生成日期：2026-09-08

---

## 一、风险总览

| 风险等级 | 数量 | 说明 |
|---------|------|------|
| 🔴 **P0（阻塞）** | 0 | 无已知无法解决的问题 |
| 🟠 **P1（高）** | 3 | 需要设计阶段确认或落地前 spike |
| 🟡 **P2（中）** | 5 | 可预见的实现风险，有缓解措施 |
| 🔵 **P3（低）** | 3 | 边缘情况，影响小 |

---

## 二、P1 风险（高优先级，必须解决）

### 🔴 P1-1：chunk_pipeline `--provided-stages` 的 provenance 护栏可能不够牢固

**根因**：`provenance.json` 当前只写入一次（chunk_pipeline 末尾），其中包含 `result: "passed" / "failed"` 等字段。但 **Phase B 启动时，provenance.json 存在 ≠ Phase A 的 build+fact 产物有效**。可能有以下情况：
- Phase A 因运行中断（kill/OOM）导致 **provenance.json 写了一半**（或在 phase A 成功写完后 Phase B 启动前被人为删了 entry.exe）
- Phase A 的 build 产物的 git commit 与 Phase B 当前工作树不一致（有人中间提交了）

**影响**：Phase B 信任了无效的 build 产物 → `entry.exe --benchmark-all` 跑旧数据 → 假阳性/假阴性

**缓解措施**（选 1 或组合）：
1. **provenance.json 增加 `git_commit` 字段**（已有，chunk_pipeline.py:485-486 已写），Phase B 校验：`provenance.git_commit == current HEAD`，不一致则 fallback 重跑 build
2. **provenance.json 增加 `build_hash` 字段**：Phase A 跑完后，对 entry.exe 计算 sha256 写入 provenance；Phase B 校验 entry.exe 当前 sha256 是否匹配——不匹配则重跑
3. **最简 fallback**：Phase B 调 `--provided-stages` 时，若 `provenance.json` 缺失或 git_commit 不匹配，**自动降级到 Route 2**（重传 `--stages build,fact,benchmark,...` 靠 hephaestus cache）

**建议**：落地 Phase 1 时先实现(3)作为兜底，时间允许再补(2)。(1) 的 git_commit 校验几乎零成本，**必须做**。

---

### 🟠 P1-2：Phase B 子进程的 cwd/PYTHONPATH/windows 环境问题

**根因**：`chunk_pipeline` 当前设计中，foundation 根和 sys.path 的解析依赖 `_path.py` 从 `__file__` 向上找到 `pipeline-config.yaml` 标记。当 nightly/run.py 通过 `subprocess.Popen(sys.executable, "-m", "verification.chunk_pipeline", ...)` 启动子进程时：
- 子进程的 `cwd` 可能是 `foundation_dir.parent` 或 `tests/e2e/`
- 但 `_path.py` 的 `__file__` 解析不受 cwd 影响（它锚定在 file 自身）
- 上一轮 nightly run 的 GBK UnicodeDecodeError 就是 subprocess stdout 读取的编码问题

**影响**：chunk_pipeline 子进程无法找到 foundation 根 → 0 chunk 发现 → 静默空跑或报错。GBK 崩导致 collector 退出。

**缓解措施**：
1. observer.py 读 subprocess stdout 时强制 `encoding='utf-8', errors='replace'`（已有方案，但需确认子进程内的 `PYTHONUTF8=1` 环境变量 —— 在 Popen 的 `env` 参数注入 `PYTHONUTF8=1`）
2. 子进程 `cwd` 设为 `foundation_dir.parent`（即 `tests/e2e/`），确保 `_path.py` 的 .parents 寻路正确
3. **验证方法**：在 Windows 上用中文用户名目录跑一次，确认无 GBK 崩

---

### 🟠 P1-3：state 文件并发写入冲突

**根因**：observer.py 每 5 秒从子进程 reader 线程写一次 heartbeat 到 state JSON。如果 run.py 的主线程（轮询完成循环）**在同一时刻**也写 state（`mark_passed` / `mark_failed`），两个线程写同一个文件 → **JSON 损坏或数据丢失**。

**影响**：state 文件损坏 → resume 无法判断 chunk 状态 → 重跑或跳过已完成的 chunk。

**缓解措施**：
1. **state 文件写入用独占锁**：`open(path, 'w')` 前先获取 `portalocker.Lock(path, 'r+')` 或同一进程内 threading.Lock
2. **或：heartbeat 和 mark_* 使用同一把锁**（`state_lock = threading.Lock()`，每 chunk 一把）
3. **或：observer 不写完整 JSON，只写一个独立的心跳文件**（`state/<run>/<asm>__<slug>.heartbeat`），`mark_passed` 写另一个文件（`.result`），两文件之间无竞争

**建议**：用方案(3)最干净——心跳文件和状态文件分离，无竞争。但实现时需确认 observer 的 reader 线程在 chunk 完成后的清理时序。

### 🔴 P1-4：实际 Phase A 失败的真实根因是多维的，单一 retry/error_class 无法覆盖

**Spike 核实（2026-09-08）**：把被 kill 前 nightly run 的 stdout 归档 (`nightly-build-report/latest-run-raw-output.txt`) 逐条对最后状态的失败 chunk，发现根因**远超单一 "atg-combined-cs 可重试" 假设**：

| 失败 chunk | 真实根因 | error_class 归类 |
|---|---|---|
| `diagnostics` | **native codegen 编译错**：`chaos_type_...ObjectModel_ReadOnlyCollection_System_Int32_` undeclared symbol + C2061/C2143/C3536 → 生成的 `native-aot.generated.cpp` 语法缺失 | `native-codegen-missing-symbol` |
| `runtime-compiler` | **SYSLIB5007**（`AsyncHelpers is for evaluation purposes only`）—— 不是 CS 语法错，是 dotnet TFM 平台的 Suppress-then-proceed 诊断 | `platform-diagnostic` |
| `system-xml-schema` | ATG 参数错位：`CultureInfo` → 期望 `IXmlNamespaceResolver`（CS1503） | `atg-combined-cs` |
| `system-3`(earlier) | ATG `UInt128`→`bool`（CS1503） | `atg-combined-cs` |
| 某 benchmark runner | net10 entry 0xc0000005 + `Internal CLR error` | `native-crash` |

**影响**：方案 D 的 error_class 分类器 + 重试策略**必须覆盖这些多样根因**，其中多数（native codegen / SYSLIB）是**真实 codegen/翻译缺陷，不是"重试 1 次能解"的 transient**。若 error_class 只分 atg/crash/timeout 三档，会误导"要不要自动重试"。

**缓解措施**：
1. error_class 按**可观察根因细分**（不是按 exit code 粗分）：
   - `native-codegen-missing-symbol`：tail 含 `chaos_type_` + `undeclared identifier` + `error C2`
   - `native-codegen-syntax`：tail 含 `native-aot.generated.cpp(...)error C`（C2061/C2143/C3536 等语法族）
   - `atg-combined-cs`：tail 含 `CombinedSubjects.cs(...)error CS`
   - `platform-diagnostic`：tail 含 `error SYSLIB5007/...`（需 Suppress 的编译器诊断）
   - `native-crash`：0xC0000005 / `Internal CLR error`
   - `csharp-error`：`error CS####` 不属上述
   - `timeout` 走 watchdog
2. **重试策略按 error_class 区分**：只对 `timeout` / `native-crash`(可能是启动竞态) / `env` 自动重试 1 次；`atg-combined-cs`/`native-codegen-*`/`platform-diagnostic` 是真实缺陷，**记录 fail 不自动重试**（存真实数据供修复，避免每次 nightly 都白重跑）
3. **分类器单测必须用真实失败文本 mock**（从 raw-output.txt 取以上每个类别的真实行），杜绝"凭 exit code 猜"
4. `error_class_retry_once` 默认**关**，仅对明确 transient 开启——避免掩埋真实翻译缺陷

**这是我在最初 risk 里低估的点**：只按 atg/crash 二分类会误导可重试决策。已核实后补上。

---

### 🔴 P1-5：readiness——batch 到 native codegen 失败说明 ATG 缺陷会随 nightly 镜像暴露，运维需能区分"引擎可跑"与"翻译有 bug"

**背景**：`diagnostics` 的 native codegen 缺 `chaos_type_...ReadOnlyCollection_...` symbol，与已记录 [[readonlycollection-objectmodel-join-traversal]] 的翻译缺口同源；是真实的 codegen/semantic 缺陷。这类在方案 D 下会稳定失败（不是 flaky）。CF 若把这类当 `failed` 常态化，操作者会把 nightly 误读为"整天红"。

**缓解**：
1. aggregate 输出区分 `translation-defect-fail` vs `infra-fail` vs `flaky-timeout`（用 error_class 归类）→ 让操作者看到"引擎本身好，N 个翻译需修"
2. 决策在**跑完一次方案 D nightly 之后再抽一次 status**，基于真实红黄绿判断 CI 门禁阈值（不要把真实翻译缺陷设成必须 0 fail 才算绿，否则门禁永远红、无法反哺修复节奏）

---

### 🟡 P2-1：chunk_pipeline 子进程 consume 过多内存

**根因**：最大并发 `max_workers=4`，每 chunk 子进程可能触发全量 codegen + dotnet build + cmake + cl.exe 编译。4 个并发 chunk 可能同时启动 4 个 cl.exe 进程，每个 cl.exe 可吃 1-2GB。32GB 机器上 4 并发 = 8-16GB，加上 system 其他进程，可能 OOM。

**影响**：OOM 被 OS kill → 子进程 137 退出 → 报 `native-crash` false positive，实际是资源不足。

**缓解措施**：
1. 默认 `max_workers=4` 在当前机器（64GB+）safe；若 32GB 设 `max_workers=2`
2. watchdog 可以检测 exit code 137（SIGKILL）→ `error_class=oom`
3. 若反复 OOM，自动降级到 `max_workers=1`（串行）

---

### 🟡 P2-2：run.py 的轮询循环 CPU 浪费

**根因**：`while queue or running: ... time.sleep(1)` 每 1 秒轮询所有 running 子进程。82 chunks × 2 阶段 = 164 轮次 × 长时间 idle → 空转 CPU。

**影响**：可忽略（1 秒 sleep 在 Python 中不占 CPU，但 4 并发时 ~0.1% CPU，12h run 也不到 1 分钟 CPU 时间）。不是真问题，但代码美观上可用 `threading.Event` 或 `select` 替代。

**建议**：保持 1 秒 sleep，不优化。若需要更精确的退出响应，用 `threading.Event` 让 observer 在子进程退出时 notify。

---

### 🟡 P2-3：Phase A 有 10 个 failed chunks，Phase B 只跑 72 个 passed

**根因**：这是语义正确的（Phase B 只跑 Phase A 已 passed 的 chunks）。但若 10 个 failed 中有 `atg-combined-cs` 类（可重试），默认 error_class_retry_once 会重试 1 次，可能一部分变成 passed。

**影响**：无。正常行为。
**建议**：跑完 Phase A 后打印分阶段汇总，让操作者知道哪些被重试过的 chunks 最终仍 fail。不再做额外处理。

---

### 🟡 P2-4：删除旧 nightly_runner 后回退困难

**根因**：plan 规定删除 `verification/nightly_runner/`。一旦删除，旧入口不可用，只能走新 nightly/。

**影响**：若新模块有严重 bug 且短时间内无法修复，CI 门禁中断。

**缓解措施**：
1. **删除前确认旧 nightly_runner 无外部引用**（已 grep 确认仅 nightly_runner 内部引用自己 + 测试引用 = 2 个测试文件，可安全删除）
2. **保留旧 nightly_runner 的 git 历史**（git 中永远可恢复，`git checkout <hash> -- verification/nightly_runner/` 即可）
3. **CI 入口切换分两步**：先让新 nightly/ 可用但旧入口保留，再公告删除旧文件的提交

---

### 🟡 P2-5：error_class 分类器可能误判

**根因**：`classify_exit(exit_code, tail_text, chunk_dir)` 基于规则（含 `CombinedSubjects.cs`+`error CS` → `atg-combined-cs`），可能有误判：
- `error CS` 在 tail_text 中但不在 CombinedSubjects.cs（native subjects 编译错）→ 误判为 `atg-combined-cs` 而非 `csharp-error`
- 0xC0000005 也可能是其他原因（如测试 fixture 崩溃）而非 native 代码缺陷

**影响**：错误分类 → 可重试策略错误（对 native-crash 重试无意义）→ 统计偏差。

**缓解措施**：
1. 规则按 precedence 从严格到宽松：`atg-combined-cs` 要求 `CombinedSubjects.cs` **在路径中**（非仅是提到），否则 `csharp-error`
2. `native-crash` 要求 exit code 0xC0000005 **且** tail 含 `Fatal error.` 或 `Internal CLR error`
3. 分类器单测覆盖各模式（`test_nightly_state.py` 内）
4. 保留 `raw_tail` 原始文本，人工复核时可查

---

## 四、P3 风险（低优先级）

### 🔵 P3-1：resume 跨 run 时 state 文件格式变化

**根因**：Phase 1 实现的 state 格式是 `{status, phase, ...}`。若后续版本改了 state schema（如删了 `pid` 字段、加了 `build_id`），旧 run 的 state 文件在新版读时会缺字段。

**影响**：resume 报错或跳过过多 chunks。

**缓解**：`read_state()` 用 `.get()` 访问，对所有缺失字段给默认值。不做 schema 版本化（代价过高）。

---

### 🔵 P3-2：observer 的 reader 线程泄漏

**根因**：`ChunkObserver.attach(proc)` 启动 reader 线程持续读 stdout。若子进程僵死或 `proc.poll()` 永不返回 None，reader 线程会一直阻塞 read。

**影响**：线程泄漏 → 资源耗尽。

**缓解**：reader 线程设 `daemon=True`；用 `subprocess.PIPE` 的超时读（`os.read(fd, 4096)` 或 `select`）；watchdog 判定 stalled 后 kill 子进程，reader 的 pipe 断裂 → read 返 0 → 线程自然退出。

---

### 🔵 P3-3：nightly-build-report 目录膨胀

**根因**：每 run 约 1-2GB（logs + state + per-chunk results）。若每天跑 1 次，30 天 = 30-60GB。

**影响**：磁盘满。

**缓解**：aggregate.py 可加 `--prune-days 7` 参数（默认保留 7 天），但建议由 Jenkins 侧管理保留策略（nightly 脚本不负责删除历史数据）。

---

## 五、风险缓解措施汇总

| 风险 | 缓解方案 | 责任模块 | 难度 |
|------|---------|---------|------|
| P1-1 provenance 不牢 | git_commit 校验 + fallback 降级 | run.py + chunk_pipeline.py | 低 |
| P1-2 环境问题 | PYTHONUTF8=1 + cwd 设定 | run.py Popen env | 低 |
| P1-3 state 并发 | 心跳/结果分离文件 | state.py | 低 |
| P2-1 OOM | exit code 137 检测 + 降级 | watchdog.py | 低 |
| P2-2 轮询空转 | 保持 1s sleep，不优化 | run.py | 无 |
| P2-3 失败 chunk | 分阶段汇总打印 | aggregate.py | 低 |
| P2-4 回退困难 | git 可恢复 | 流程 | 低 |
| P2-5 分类器误判 | 严格 precedence + 单测 | state.py | 中 |
| P3-1 schema 变化 | .get() 默认值 | state.py | 低 |
| P3-2 线程泄漏 | daemon + pipe 断裂 | observer.py | 低 |
| P3-3 磁盘膨胀 | Jenkins 侧管理 | 流程 | 无 |

**无不可缓解的 P0 风险。** 所有可识别风险都有可落地的缓解措施。

---

## 六、决策建议

1. **P1-1 的 git_commit 校验必须做**（provenance.json 已有 commit 字段，校验只需 ~10 行）
2. **P1-3 的 heartbeat/result 分离文件**：heartbeat 写入 `.heartbeat` 文件，`mark_passed` 写入 `.result` 文件，两者不竞争
3. **P1-2 的 PYTHONUTF8=1**：Popen 的 `env` 参数注入，避免 Windows 终端 GBK 崩
4. **P2-5 分类器严格 precedence**：在 `test_nightly_state.py` 中用 mock 文本覆盖所有分类模式