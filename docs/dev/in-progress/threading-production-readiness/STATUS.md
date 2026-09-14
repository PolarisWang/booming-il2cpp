# STATUS — Threading 生产级就绪

## 元信息

```yaml
task_id: threading-production-readiness
task_type: roadmap
phase: roadmap
roadmap_or_plan: docs/dev/in-progress/threading-production-readiness/roadmap-v1-01.md
design: docs/dev/in-progress/threading-production-readiness/design-v1-01.md
created: 2026-09-13
updated: 2026-09-13
created_by: user
blocking_questions: []
question_clearance: cleared
clearance_confirmed_by_user: true
child_execution_mode: auto
auto_continue: true
auto_stop_policy: blocking-only
dispatch_model: sequential
recommended_next_child: T0.2-run
latest_stop_point: threading chunk build 转绿 1/1 passed（三层缺陷已修）；T0.2 尚差「运行 + fact 比对」
```

## 最近摘要

对抗性审查（3 阻断级 + 6 需修正 + 12 确认无误）**已全部闭环**，
`roadmap-v1-01.md` 已产出：**24 个子任务 / 5 个阶段 / `dispatch_model: sequential`**。

用户拍板：**Phase 2 保留在 roadmap 内**（不拆出独立任务）；
Phase 0 拆 4 个 T0.x + 检查点。

**T0.0 实测（2026-09-13）**：`0/1 passed`，550s。错误**非 crt_stubs**，而是
**C3861 声明缺口**（`chaos_cancellation_token_*` 定义与注册都在，仅缺 header 声明）
+ **C3313/C3536**（`chaos_result` 为 `const void`，独立第二缺陷）。
⇒ Phase 0 新增 **T0.0b / T0.0c**，且 worktree 需**立即**切出（T0.0 已证明必须先改代码）。

**T0.3 取证**：`BuildMethodSourceSafe` 确证吞异常降级为空 stub，
`kCodegenFailureCount` 信号**全仓零消费者** ⇒ 第三条假绿通路。

**假绿向量（源树残留）**：`reporting.py:57` 读源树而非产物根，已转为 **T0.1**。

## 下一步

**✅ 阻塞已解除**：threading chunk **`1/1 passed`**（724 subjects → entry.exe，509s）。
三层缺陷全部定位并修复，详见下节。

---

## ✅ T0.0b/T0.0c/T0.2 闭环（2026-09-13 实测）

**起点**：`0/1 passed`（550s），9× C3861 + 2× C3313/C3536。
**终局**：`1/1 passed`（534s），零 error。中间经历了**三个独立缺陷**，
且前两个被第三个掩盖 —— 每修好一层才暴露出下一层。

### 缺陷 1 — C3861：缺 codegen 可见声明（T0.0b）

`chaos_cancellation_token_*` 定义与 ShapeRegistry 注册都在，唯独缺 header 声明。
修：新建 `runtime_stubs/cancellation_token_stubs.h`（8 个 `extern "C"` 声明）+
在 `stubs.h` 中引入。**注意**该头拉入 `<chaos/native_types.h>` → `<chaos/config.h>`
→ 全部 C++ std 头，**必须放在 `extern "C"` 块之外**（同 `threading_stubs.h` 的
MSVC C2039 `::terminate` 约束）。

### 缺陷 2 — C3313/C3536：void helper 破坏 INTPTR 分派（T0.0c）

真因**不是** ABI slot，而是 **C++ 声明**。生成的 shape 分派
（`runtime_helper_shapes.h`）对**每个** helper 都包一层
`reinterpret_cast<CHAOS_IL2CPP_INTPTR>(sym(args...))`，调用点再加
`const auto chaos_result = ...`。两步都要求 helper 是**值**，void 都不行。
改 slot 只能修正 wrapper 的**类型**，改不了 callee 的**声明** —— 这正是
`7f17415cd` 只改 slot 却无效的原因。

修（`a2b06d690`）：三个 void helper 全部改为返回 `CHAOS_IL2CPP_INTPTR`(0)：
`ChaosAsyncTaskAwaiterGetResultVoid`、`chaos_async_yield_get_result`
（含 TPG fallback `CppProjectEmitter.cs:308`）、
`ChaosRuntimeEnvironmentGetRuntimeInterfaceAsObject`。
**取证手段**：把分派头里 290 个被 `reinterpret_cast<INTPTR>` 包裹的符号
与 `src/native/**` 的声明逐一交叉比对 ⇒ 恰好 3 个 void，全修。

### 缺陷 3 — LNK2019：header 的 `extern "C"` 块提前收口（b96856e01）

编译全绿后进入链接，只剩 2 个未解析：
`chaos_thread_yield`（Thread.Yield）/ `chaos_thread_sleep`（Thread.Sleep）。

**真因**：`threading_stubs.cpp` 用**文件级** `extern "C" {` ⇒ 全部定义 C linkage
（lib 里导出**未修饰名**，dumpbin 实证）；而 `threading_stubs.h` 的块**只包了
`chaos_monitor_enter`/`chaos_monitor_exit` 两个函数**就收口，后面的声明落到
**C++ linkage** ⇒ 声明与定义命名成两个不同符号（`?chaos_thread_yield@@YAHXZ`
vs `chaos_thread_yield`）。

**判别信号**：同一 obj 里的 `chaos_monitor_enter` 链接正常、`chaos_thread_yield`
却报未解析 —— 这个**不对称**直接指向「块的覆盖范围」。
修：延展 header 的块覆盖全部导出声明，文件尾收口；
`chaos_thread_get_current` 保留在块外（故意的 inline C++）。

### T0.3 假绿门禁：**通过**（独立取证）

- `CODGEN-FAIL` 行：**0 次**
- `kCodegenFailureCount` 符号：chunk 产物中**不存在**（设计上仅 `>0` 时 emit）
⇒ **零 codegen 降级**，本次 `1/1` 是真实翻译成功，非 `BuildMethodSourceSafe` 兜底。

### T0.2 运行 + fact 比对（2026-09-13 完成）—— **跑出真缺陷，结论不是「绿」**

补上了此前欠的「运行 entry.exe + fact 比对」一步。两个 chunk 均 `--stages build,fact`：

| chunk | build | fact 判定 | realVerified |
|---|---|---|---|
| `threading` | `1/1 passed`（489s） | `1/2 passed`（**failed**） | 136/136 |
| `threading-2` | `1/1 passed`（557s） | 未跑（时间预算内先定位缺陷 1） | — |

**`519/523 passed` 不是真实通过率**，必须按 `fact.json` 分字段读：

| 字段 | 值 | 含义 |
|---|---|---|
| `total`/`passed` | 523 / 520 | 原始记录（口径混用，见缺陷 2） |
| `gateTotal`/`gatePassed` | 519 / 520 | 门禁分子分母 —— **`gatePassed > gateTotal` 是计数 bug** |
| `realTotal`/`realPassed` | 136 / 136 | **真实语义断言，全通过** |
| `unverifiedSmoke` | 383 | 仅 42 哨兵、无真断言 |
| `unverifiedMarkers` | 321 | 编译期 `[UNVERIFIED]` 标记 |
| `factoryGap` | 4 | factory 返 null，方法**根本没跑** |

#### 🔴 缺陷 1（真缺陷）：`SubjectInstanceFactory.Create<T>()` 降级 → 主体未被构造

> **⚠️ 本节结论已于 2026-09-13 被自身取证推翻并重写。**
> 曾一度记为「`ChaosAsyncTaskAwaiterGetResultVoid` 指针类型混淆」，**该结论是错的**：
> 4 个 `factoryGap` 中有 3 个（`Cancel_0__0` / `CancelAsync_2__0` / `Dispose_6__0`）
> 在 **AOT 与 JIT 下同为 `factoryGap`**，语义上根本不是 AOT 缺陷，且它们的生成体
> **不含任何 awaiter helper 调用** —— helper 不可能是其原因。
> 只有 `DisposeAsync_1__0` 是真 cross-tech diff。**推翻过程**见下方「取证更正」。

**症状**：cross-tech diff 恰 1 条 ——
`CancellationTokenRegistrationTests::DisposeAsync_1__0` `AOT=factoryGap / JIT=real+PASS`。

**取证链（逐层，全部实测）**：

| 步 | 手段 | 事实 |
|---|---|---|
| 1 | fact 记录 | AOT 侧 `resultKind: factoryGap`、`assertFailed: false` —— **不是断言失败** |
| 2 | 插桩 `runtime-entry.cpp` 的 `__except` 过滤器 | `code=0xE0000001`（`kChaosManagedExceptionCode`）、**`exobj=nullptr`** |
| 3 | 反查 raise 源 | `exception_helpers.cpp:163` — `ResolveTypeByName` 返 0 ⇒ `chaos_raise_exception(0)`（**null payload**） |
| 4 | 看生成体 `native-aot.generated.page2.cpp:3517` | `Create<CancellationTokenRegistration>()` 走 `chaos_external_runtime_..._Create_...` |
| 5 | 看该符号定义 `native-aot.generated.cpp:8283` | 体是 `ChaosExternalRuntimeFallback(...)` ⇒ **恒返 0（null）** |
| 6 | 看 null 的消费方式 | 返回值 store 进 `chaos_locals[0]`，**重新加载后带 null-guard** ⇒ guard 抛 NRE ⇒ 主体从未构造、方法从未运行 |

**根因**：`Chaos.TestFramework.SubjectInstanceFactory.Create<T>()` 的泛型实例化在
chunk 内**没有 native 实现**，被降级为 `ChaosExternalRuntimeFallback`（返 0）。
`Create<CancellationTokenRegistration>()` 因而恒返 null。

**关键对照（为何只有它是 diff）**：同样走 fallback 的 `Cancel_0__0`，
其 `Create<CancellationTokenSource>()` **也**返 null，但生成体把它**直接**传给
`chaos_cancellation_token_source_cancel(_s2)` —— **没有 null-guard**，null 从未被
消费，故 AOT 照样 `passed:true`。`DisposeAsync_1__0` 的差别**不在 awaiter helper**，
而在返回值经过「store → reload → null-guard」这条路径。**判别信号是 null 是否被消费。**
⇒ 这是 **roadmap Phase 2 预判的「缺 ABI 出口层」**的一个实例，**不是** Phase 1 语义缺陷。

**处置**：归入 Phase 2（T2.x 句柄映射 + ABI 出口层）覆盖，**不在 Phase 1 修**。
`ChaosAsyncTaskAwaiterGetResultVoid` 本身**无缺陷**，不改。

**取证更正（复盘）**：初版把「插桩看到 `canceled=107`」当作根因证据，
但那**只证明读到了栈垃圾，不证明读到垃圾就是失败原因** —— 缺陷 1 的真正判别是
「被测方法是否运行过」，而非「某个 helper 里读到了什么」。教训：
**插桩观测到异常值 ≠ 该值是因果链上的原因**；先做分母审计（4 个 factoryGap
对照 AOT/JIT），再谈单点根因。

#### 🔴 缺陷 2（假绿向量）：`fact_chunk.py` 分子分母跨技术混用

`fact_chunk.py:310-318` 在 JIT 通过率 > AOT 时把 **`passed`/`total` 整体替换为 JIT 的数**；
而 `:441` 的 `gate_denominator = total - factory_gap_ct` 里，
`factory_gap_ct` **永远来自 AOT 记录**（`:431` 遍历 `per_method["aot"]`）。

⇒ **分子可能取自 JIT、分母取自 AOT**，两个不同总体相减。
现场症状：`gatePassed=520 > gateTotal=519`。

**影响**：门禁在「AOT 差、JIT 好」时**系统性偏乐观** —— 恰好掩盖了缺陷 1 那类
「AOT 独有失败」。这条与 T0.1（源树残留）、T0.3（codegen 降级）构成**第三条独立假绿通路**。
**处置**：Phase 0 内修（属「堵假绿向量」），见 T0.5。

---

## ✅ T0.4 完成（2026-09-13）—— 244 用例首次真正跑起来，并跑出 7 个失败

### 决定性发现：**Windows 下 ctest 一个测试都没跑过**

`chaos_native_add_test()`（`cmake/chaos_native_test.cmake:172`）每个测试都以
`add_test(NAME ...)` 收尾。**`add_test()` 在未调用 `enable_testing()` 时是空操作** ——
而全仓 `enable_testing()` **只出现在 `cmake/arm64-jit-test.cmake` 的 `if(QEMU_AARCH64)` 内**
（`CMakeLists.txt` 仅在 ARM64 分支 578/799 行调用）。

⇒ **x64 路径下所有 native 测试的 `add_test` 全部落入虚空**：

```
$ ctest --test-dir artifacts/presets/windows-x64-reference -C Debug -N
Total Tests: 0
$ ctest ... -LE "benchmark|stress|soak" --output-on-failure
No tests were found!!!
EXIT CODE: 0        ← 零测试却退出码 0
```

**影响面远超 threading**：`tests/suite_contract.yaml` 的 `contracts-native` 组
（注释自称「runs the CTest targets under contracts/native + tests/unit/runtime-native
（unit/deterministic only … ~200 deterministic native tests）」）**从未真正执行过任何用例**。
这是与 T0.1（源树残留）、T0.3（codegen 降级信号零消费者）、T0.5（分子分母跨技术混用）
**同族的第四条假绿通路** —— 且是最大的一条：不是「读错了源」，而是**根本没读**。

**诚实标注**：`test_driver.py` 的 `ran_ok = res.error is None and res.total > 0`
配合 `native.py:116` 的 `if res.total == 0: res.error = ...` **确实会**把零测试判为
失败并让 driver 退出 1。所以**不是**「CI 一直绿着骗人」；准确的说法是：
**该用例集从未被这个门禁执行过**（gate 在"发现 0 个测试"这一态上是诚实的），
而 `contracts-native` 组所声明的覆盖范围与实际执行范围存在**巨大缺口**。

**修复**：`CMakeLists.txt` 的 `windows-x64-reference` 分支中、
`include(cmake/chaos_native_test.cmake)` 之后加 `enable_testing()`。
修复后 `ctest -N` 由 **0 → 300**。

### 顺带修复：三个「僵尸测试文件」

28 个 threading test 文件中，**3 个从未被任何 CMakeLists 引用**（全仓 grep 零命中），
即从未编译、从未运行：

| 文件 | 用例 | 处置 |
|---|---|---|
| `monitor_pulseall_stress_test.cpp` | 1 | ✅ **已注册并验证通过**（0.81s PASS） |
| `hill_climbing_smoke_test.cpp` | 15 | ❌ **未注册**：访问 `HillClimbingController::cpu_count_` / `::SigmoidGain`，二者现已 **private**（C2248） |
| `threadpool_events_smoke_test.cpp` | 10 | ❌ **未注册**：调用 `ThreadPoolEventEmitThreadCreate/Attach/Detach/SafepointBegin/SafepointEnd/MonitorContention`，**全部已不存在**（C2039/C3861） |

后两个是**腐烂测试**（rotted），不是被隐藏的覆盖 —— 它们针对的 API 已经改名或移除。
**注册只会让构建变红而不增加任何覆盖**，故显式排除并在 CMakeLists 内注明原因。
复活它们需**按现行 API 重写**，属独立任务。

另外 `async_when_each_test.cpp` 有一个可推导类型 bug（`auto* t` 遍历
`std::vector<CHAOS_IL2CPP_INTPTR>`，`__int64` 无法推导为指针，C3535/C2440）——
一并修复。

### 首次真实基线：**20/27 通过，7 失败**

| 用例 | 结果 | 耗时 | 判定 |
|---|---|---|---|
| `test_threading_benchmark` | **SEGFAULT** | — | 真缺陷 |
| `test_async_when_async` | **SEGFAULT** | — | 真缺陷 |
| `test_async_continue_with` | Failed | — | 真缺陷（`ContinuationTaskCarriesTheContinuationsReturnValue`，SEH 0xC0000005） |
| `test_async_when_each` | Failed | 26s | 真缺陷（`NullElementStillTerminatesTheStream` 挂起 23s） |
| `test_queue_backpressure` | **SEGFAULT** | — | 真缺陷 |
| `test_threading_stress` | **Timeout** | 1800s | 需判定：真死锁 or 超时阈值过紧 |
| `test_phase3_industrialization` | **SEGFAULT** | — | 真缺陷 |

**关键结论**：这 7 个失败**此前从未被任何人看到过** —— 因为这套用例从未运行。
threading 的「生产级就绪」比 roadmap 撰写时的估计**更差**：
不只是「无 CI 门禁」，而是**有 5 个真实的崩溃/挂起缺陷一直躺在树里**。

**处置**：T0.4 交付 workflow 并按 roadmap 原定 `continue-on-error` 收基线
（`enforce_gate` 默认 false）。这 7 个失败登记为 **T1.7**，在 Phase 1 处理 ——
它们正是 Phase 1「关闭语义造假」要面对的东西，且**优先级高于**原 T1.1-T1.5
（崩溃 > 静默错误结果 > 语义缺失）。

### 交付物

- `.github/workflows/threading-native-tests.yml`（独立 workflow，不进 ci-framework）
  - 含**显式的发现数断言**：ctest 发现 0 个 threading 测试即 `exit 1`
    —— 直接堵住本节发现的那条假绿通路
  - `-LE "benchmark|soak"` 排除长跑测量层，**保留 `stress`**（并发缺陷就在那里）
  - `enforce_gate` 默认 `false` 收集基线，可切换为阻断

---

## 🔴 worktree 构建隔离缺失（2026-09-13 实测，阻断级）—— **已修（a8e807696）**

**T0.0b + T0.0c 的修复已提交（`7f17415cd`），但重跑 `--stages build` 仍 `0/1`，
错误逐条未变。根因不是修复无效，而是 worktree 的改动对构建完全不可见。**

### 证据链（逐层实测）

| 环节 | 事实 | 证据 |
|---|---|---|
| native include 路径 | 指向 **主检出**，非 worktree | `chaos_entry.vcxproj:89` 的 `AdditionalIncludeDirectories` 首项 = `D:\agent\chaos-il2cpp\src\native\runtime-core` |
| 主检出有无我的头 | **无** | `ls /d/agent/chaos-il2cpp/src/native/runtime-core/runtime_stubs/cancellation_token_stubs.h` → 不存在 |
| 全仓声明位置 | 只在 `.cpp` 里 | `grep -rln chaos_cancellation_token_source_cancel src/native/` → 仅 `cancellation_token.cpp` |
| codegen 工具根 | 解析到**主检出** | `tool_helpers._repo_root()` 从 `tool_helpers.py` 自身位置上溯；该文件在 `tests/e2e/verification/_pipeline/`，即主树 ⇒ 返回 `D:\agent\chaos-il2cpp` |
| 工具新鲜度判定 | 比的是**主树的 Generator 源码**，故判「已是最新」而跳过重建 | `ensure_tool_built` → `_referenced_projects` → `_project_sources` 全部经 `_tool_dir()`（主树）解析 |
| 后果 | TPG 跑的是 **15:26 的旧 Generator.dll**，不含 14:59 的 C# 修复 | TPG bin 内 DLL mtime `15:26:57` vs 我的 .cs mtime `14:59:53` |

**关键句**：`_repo_root()` 的注释写着「Walks up so it is robust to where the
_pipeline package is relocated」——它锚定的是**代码所在的那棵树**，而 worktree 里的
`tests/e2e/verification/` 是主树的一份副本，但**工具本体、include 路径、构建产物全在主树**。

### 结论

**worktree 隔离在此仓库对「验证管线驱动的构建」不成立。**
`EnterWorktree` 隔离了 git 工作区，但**没有隔离**：

1. native 编译期 include 路径（硬编码主检出绝对路径）
2. codegen 工具链（TPG/Generator DLL 及其新鲜度判定）
3. `artifacts/` 产物根（本来就按主树解析）

⇒ 在 worktree 里改 native 头/Generator 源码，**构建不会看到**，
且**不会报错**——只会继续用主树的旧代码，症状与「修复无效」完全一致。
这是一个**新的假绿/假红向量**：改动看似落地、构建照常输出、结果与改动无关。

### 处置（✅ 已采纳方案 A，`a8e807696`）

| 方案 | 内容 | 结论 |
|---|---|---|
| **A. 修复 worktree 的根解析**（✅ 已实施） | `tool_helpers._worktree_root()` + TPG `TryDetectWorktreeRoot()`，以「`.git` 是文件且首行 `gitdir:`」识别 linked worktree | 已修；**code 根锚 worktree，data 根锚主树** |
| B. 放弃 worktree，回主工作区开发 | — | 未采纳 |
| C. worktree 只写代码，构建在主树跑 | — | 未采纳 |

**⚠️ 实施中发现的约束（务必遵守）**：修根解析后必须**区分 code 根与 data 根**：

| 根 | 应锚 | 理由 |
|---|---|---|
| `_repo_root()` / `_tool_dir()` | **worktree** | 代码：Generator / TPG / native 头 |
| `build_root()`（`_path.py`） | **worktree** | 产物，天然隔离 |
| `foundation_root()` | **主检出** | 数据：`tests/e2e/translation/` 是 gitignored 输入树，只在主检出被填充 |

把 `CHAOS_FOUNDATION_DLL` 指向 worktree 的同名路径会立刻
`namespace-partition.json not found` —— 那是**数据根指错**，不是修复失败。
**正确调用**：`CHAOS_FOUNDATION_DLL=<主检出>/tests/e2e/translation`，其余自动锚 worktree。

---

## 问题来源

用户要求「整体看下 thread 的功能完善情况，如果要达到生产机的开发品质，还需要做些什么？」。
产出一份综合分析报告后，用户要求用 dev-brainstorm 规划并整体 review。

**分析报告的关键结论**：Threading 是全仓库**最不可信**的一族——不是实现最差，而是**已知最少**。
内核（ThreadPool / TimerQueue / Monitor / Interlocked / WaitHandle）已是真实工业实现，
但「验证面」与「翻译面」两头断裂。

## 边界拍板

### 做什么

全量 Phase 0-4：

| 阶段 | 内容 |
|---|---|
| Phase 0 | 止血：实测 build 错误、**清除源树陈旧假绿产物**、重建 3 chunk、堵假绿向量、CI 接入 |
| Phase 1 | 关闭语义造假（CancellationToken 3 点 / Parallel / SynchronizationContext） |
| Phase 2 | 接线 **缺 ABI 出口层**的原语（**非「未注册」——部分已在 ShapeRegistry**；成本大幅上调，见 design §12） |
| Phase 3 | 加固（非原子 static / 句柄表 / 热路径 fprintf / 文档 / benchmark） |
| Phase 4 | 制度化（门禁阈值 / 回归告警） |

### 不做什么
- **hotupdate slot mapping** → 用户拍板**拆出独立任务**
- **POSIX 平台缺陷** → 仅 Windows 目标，登记 watch_item
- **Thread.Abort 语义** → .NET Core 已弃用该 API
- **前缀过捕获** → 审计确认未发生，属误报

## worktree 策略（2026-09-13 增补）

**切出时机：T0.0 完成、拿到实测编译错误之后立即切，不早于也不晚于。**

| 工作区 | 覆盖 |
|---|---|
| 主工作区（`main`） | T0.0 只读取证 —— 需要已有的构建缓存与工具链 |
| `worktree-A` | Phase 0 代码改动（T0.1-T0.4） |
| `worktree-B` | Phase 1-4；与 A 经 main 的 merge 交接 |

**不早切**：T0.0 是纯读取证，不产生改动，无需隔离。
**不晚切**：T0.1 起产生代码改动，而主工作区**已有并发 agent 活动**（且直接触及 threading —— 见 design §8.2），
暴露在 `parallel-agent-clean-checkout-clobber` 记录的整树覆盖竞态下。

**已知成本**：worktree 的 `artifacts/` 与 `testing/artifacts/` 均为 gitignored 且不共享，
工具链与 chunk 产物需冷重建。**若 T0.2 实测构建 > 30 分钟，退化为单 worktree 覆盖 Phase 0-4。**

详见 `design-v1-01.md` §3.1。

## authority 决策

| 决策点 | 结论 |
|---|---|
| 权威 partition | `tests/e2e/translation/.../namespace-partition.json`（200/chunk，**有 threading-2**） |
| `manifest.py:110-113` | 仅为**默认命名映射表**，非权威 chunk 来源 |
| 源根 / 产物根 | `foundation_root()` = `tests/e2e/translation/`；`build_root()` = `artifacts/foundation-dll/` |
| 修改授权 | **四层全开**（native / codegen / 验证管线 / CI） |

## 当前结论

**三个 threading chunk 全部 `build.status = error`**，`results/` 下无 `fact.json`；
`threading` 与 `threading-2` 在 2026-09-12 honest report 中**完全缺席**（70 行差集的唯一两个）；
**28 个 native threading 测试文件 / 244 用例**（原写 15 文件 / 126 用例，已按实测修正）**零 CI 门禁**。

**⚠️ 诚实标注**：build 失败的**具体编译错误未实测**，报告原因为推断。计划 Task 0 即为此设。

### 🔴 活着的假绿向量（对抗性审查 R1，2026-09-13 实测确认）

`reporting.py:57` 读的是 `foundation_dir/asm/chunks`，而 `foundation_dir`
（`chunk_pipeline.py:321` → `foundation_root()`）是**源树**
`tests/e2e/translation/`，**不是产物根** `artifacts/foundation-dll/`。

现场证据：`tests/e2e/translation/.../threading-tasks/results/fact.json`
存在（**mtime 2026-09-08 14:54**，`passed:456`），而产物根下
`provenance.json` 显示 `build.status: error` 且**无 fact.json**。

⇒ honest report 中 `threading-tasks ... PASS` 那一行读的是**五天前的源树残留**。
该文件已被 `.gitignore:600` 忽略、未被 git 跟踪 ⇒ **不会随 `git status` 暴露**。

**处置**：Phase 0 新增 **T0.1 清除源树陈旧 fact.json**；验收读取根**钉死为产物根**，
并加 mtime/runId 一致性断言。详见 `design-v1-01.md` §7.1。

## 🟡 T0.3 取证结论（2026-09-13）—— 假说**成立**，且比预想更系统

**`BuildMethodSourceSafe` 确实吞掉一切异常并静默降级为空 stub。**

| 项 | 事实 | 证据 |
|---|---|---|
| 捕获类型 | `catch (Exception)`，**无过滤器** | `NativeAotLoweringPlanner.Methods.ModuleData.Helpers.cs:551-575` |
| 捕获后行为 | 吞掉 + 记 stderr + 计数 + **降级为空 stub + 不重抛** | 同上；降级走 `BuildAotUnreachableMethodStub`（:492-524） |
| 调用点 | **所有 AOT-reachable 方法**，无命名空间豁免 | `Helpers.cs:599`（唯一调用点），经 `Methods.cs:1108,1115` 单/并行两条路径 |
| 降级产物 | 语法完整、**可编译可链接**的 `extern "C"` 空函数（非 void 返 `{}`） | `Helpers.cs:492-524` |
| 内层还有一层吞 | `NotSupportedException`/`InvalidOperationException` 也被吞 | `ExceptionEmission.EmitInstruction.cs:58-67` |

### 🔴 关键：信号被写出，但**没有任何消费者**

存在三条痕迹，但**均无门禁读取**：

1. stderr：`[codegen] WARNING: codegen failed for {SubjectId}, emitting stub.`
2. 汇总日志：`[CODGEN-FAIL] total={CodegenFailureCount} ... by exception type / by chunk`
3. 生成 C++ 符号：`extern "C" const int kCodegenFailureCount = N;`（仅 `>0` 时 emit）

**全仓 grep `kCodegenFailureCount` 的消费者**：`src/native/`（0）、`src/tools/`（TPG，0）、
`tests/e2e/verification/**/*.py`（0）。现存反假绿门禁 `fake_green_gate.py:32-51`
只检查 `kUnsupportedAsyncIteratorCount` / `ChaosExternalRuntimeFallback(` / 「全部是 stub」——
**不覆盖 `kCodegenFailureCount`**。

⇒ **`kCodegenFailureCount > 0` 不会让 `build.status` 变红。**

### 项目自己已经知道这件事

代码注释里写了两次：

- `NativeAotLoweringPlanner.ModuleRegistration.Dispatch.cs:228-231`：
  > Emission runs behind `BuildMethodSourceSafe`, which catches EVERY exception and
  > substitutes an unreachable stub. A throw from the emission site is swallowed and
  > the build stays green. The non-throwing channel is the only one that actually reaches
  > the outside world.
- `Methods.cs:412-418` 几乎同文
- `AsyncBoxPointerDetectorRegressionTests.cs:1-35`：该类**曾实吞 4 次** `CodegenFailureCount` 且构建保持绿（实测先例）

### 对本计划的影响（🔴 阻断级）

**T0.0 拿到 `build.status=passed` 不构成「threading 翻译正确」的证据。**
若 codegen 降级了部分方法，Phase 0 的验收会**再次**是假绿 —— 与 T0.1 修的
`reporting.py` 假绿向量是**两条独立的假绿通路**。

**处置（写入 Phase 0）**：
- T0.2 重建后**必须同时读** `[CODGEN-FAIL] total=` 汇总行与 `kCodegenFailureCount` 符号
- 若 `total > 0` ⇒ Phase 0 **未完成**，先定位被降级的方法再谈验收
- Phase 4（制度化）的 T4.1 门禁**必须**把 `kCodegenFailureCount` 纳入阈值

### 未查清（诚实标注，未实测）

- **未实测** threading 域是否真的触发了降级（本任务只读，未构造 threading 输入）
- **未验证** `build.status=passed` 与 `kCodegenFailureCount` 之间是否存在**间接**关联
  （确认了无直接消费者，未逐行读完整个 build stage）
- `CodegenFailureCount++`（`Helpers.cs:561`）在 `Parallel.For` 路径下**非原子**
  （无 `Interlocked`），若并发丢计数则数值可能低估 —— 未实测严重程度

## 🔴 T0.0 实测结论（2026-09-13）—— 报告推断作废，根因是声明缺口

**跑法**：主工作区 `chunk_pipeline --chunk threading --stages build --native-config check`。
**结果**：`0/1 passed`，Duration **550s**。

### 实测错误（非 crt_stubs）

```
native-aot.generated.cpp(9127,5)  error C3861: 'chaos_cancellation_token_source_cancel': identifier not found
native-aot.generated.cpp(9132,5)  error C3861: 'chaos_cancellation_token_source_dispose': identifier not found
native-aot.generated.cpp(9142,5)  error C3861: 'chaos_cancellation_token_throw_if_cancellation_requested': identifier not found
page2.cpp(3561,27)                error C3313: 'chaos_result': variable cannot have the type 'const void'
page2.cpp(3562,42)                error C3536: 'chaos_result': cannot be used before it is initialized
page2.cpp(3677,3) / 3908,3 / 3968,3 / 4420,3 / 4535,3 / 4798,3  error C3861: (同上三符号，重复命中)
```

**⇒ 设计 §0 的「陈旧 crt_stubs」推断作废。** 实证错误是 **C3861 声明缺口 + 2 个 C3313/C3536 类型错误**。

### 根因（已定位到行）

| 环节 | 状态 | 证据 |
|---|---|---|
| native **定义** | ✅ 存在 | `src/native/runtime-core/cancellation_token.cpp:255,289,297`（均为 `extern "C"`） |
| ShapeRegistry **注册** | ✅ 存在 | `RuntimeHelperShapeRegistry.CoreStubs.Part1.S16.cs:1697,1731,1739`（`ShapeKind.SimpleForward`） |
| **header 声明** | ❌ **不存在** | `grep -rln "chaos_cancellation_token" src/native/ --include=*.h` → **零命中** |

`cancellation_token.h` 存在但只声明**内部 C++ 句柄 API**（`uint32_t CancellationTokenSourceCancel(uint32_t)`），
**不是** codegen 用的 `chaos_*` ABI 名。

**对照先例**：`chaos_monitor_enter` 能编译，是因为它声明在
`src/native/runtime-core/runtime_stubs/threading_stubs.h`。
`runtime_stubs/` 下**没有**任何 cancellation 相关的头 ⇒ 三个符号全部悬空。

⇒ **同族于记忆 `c3861-codegen-handoff`**：定义与注册都在，唯独缺 codegen 可见的声明。

### 对 Phase 0 形状的影响（🔴 阻断级）

1. **报告推断作废**，Phase 0 不再是「重建陈旧产物」那么简单 —— 需要**补声明**（代码改动）
2. **T0.2「清残留 → 重建」的顺序要改**：光重建不会让 chunk 转绿，必须**先补 header 声明**
3. **C3313/C3536（`chaos_result` 是 `const void`）是独立第二缺陷** —— 与 cancellation 无关，
   需单独定位（很可能是某个返回 `void` 的 SimpleForward 被赋给了变量）
4. **worktree 切出点前移**：原设计「T0.0 只读取证、不产生改动」**不再成立** ——
   T0.0 已证明必须先改代码。切 worktree 应**立即**进行，不等 T0.2

### 与 T0.3 的关系（重要）

T0.3 确证的假绿机制**没有**在这里生效 —— 若 `BuildMethodSourceSafe` 吞掉了这些方法，
就不会有 C3861。说明这些调用点**在 stub 降级之外**，或者异常发生在更早的阶段。
**两条假绿通路 + 这条真实失败并存**，Phase 0 需同时处理。

## 下一步入口

见上方「下一步」—— `roadmap-v1-01.md` 已产出，启动 **T0.0**。

## 风险评估摘要

| 风险 | 缓解 |
|---|---|
| T0.0 实测根因与推断不符 | Task 0 优先；**不得**无实测即开工 T0.2 |
| `BuildMethodSourceSafe` 是假绿源头 | 已拍板提到 Phase 0，取证优先于 rebuild |
| Phase 2 接线后 real% 不升 | 专项取证；三次规则后转 dev-brainstorm |
| 重建触发 cross-chunk 回归 | 单 chunk 隔离验证 + honest report 前后对比 |
| 244 用例首跑大量失败 | 首跑 `continue-on-error` 收基线，再定阻断阈值 |
| 主工作区被并发 agent 抹除改动 | **T0.0 后立即切 worktree**；commit 前核对 `git diff --cached` |
| worktree 冷构建成本超预期 | T0.2 首步实测耗时；> 30 分钟则退化单 worktree |

## 三优先级权衡结论

采纳**方案 B（阶段内并行、阶段间串行）**。

理由：threading 是运行时基础设施，其正确性影响所有上层 —— 性能（P1）的前提是语义正确，
而当前存在**会静默产出错误结果**的 P0 缺陷，必须先于性能优化处理。
P2（架构完美）体现为阶段门禁不放松；P3（HotUpdate）无冲突，该问题已拆出。
**无优先级冲突需裁决。**

## 已完成

- ✅ 三路并行审计（native 实现面 / 验证覆盖度 / codegen 翻译缺口）
- ✅ 证据链核验（partition 权威性、nightly 消费路径、chunk 缺席差集、crt_stubs 旧签名）
- ✅ 综合分析报告交付
- ✅ `design-v1-01.md` 写入
- ✅ `blocking_questions = []`，用户确认清零
- ✅ **对抗性审查（2026-09-13）**：3 阻断级 + 6 需修正 + 12 确认无误，**已全部处理**
  - R1 → §7.1 假绿向量 + 新增 T0.1
  - R2 → Phase 2 表述修正（「杠杆最大」撤销）
  - R3 → 偏差声明精确措辞 + 基线漂移披露
  - Y1 → 测试规模 126 → **244**
  - Y2 → 43% 分母声明
  - Y3 → §8.1 T0.0 真实形态（无日志，是完整重建）
  - Y4 → d5d0630da 外推标注
  - Y5 → §12「未注册」→「缺 ABI 出口」
  - Y6 → §8.2 并发 agent 触及 `Part1.S16.cs`

## 遗留（非本计划）

- `chaos_continuation` 回归测试仍未落地（`bb9a02fd0` 无测试守护）
- `pcdistpatch-cond-exit-target-dropped` 未修
- 其余 32 个陈旧 chunk 重建（属 `chaos-continuation-scope` 任务）

---

## Phase 2 / Phase 3 执行记录（2026-09-14 增补）

### Phase 2 — codegen ABI 接线（T2.0-T2.5）

| task | 结果 | commit | 说明 |
|---|---|---|---|
| T2.0 | ✅ | `f55fddfa1` | 39 个 `extern "C"` ABI 出口 + 链接期验证 |
| T2.1 | ✅ | `a0b46d5d3` | 句柄存托管对象**字段**（非映射表）；GC 压缩式回收下唯一正确解 |
| T2.2 | ✅ | `aabcf8887` | ReaderWriterLockSlim 托管面（15 入口 / 8 测试 / mutation 验证） |
| T2.3 | ⛔ **blocked** | — | Barrier + CountdownEvent 在该 chunk **零 subject**，无对象可接线 |
| T2.4 | ✅ | `b922c97e5` | ManualResetEventSlim（8 注册 / 8 测试） |
| T2.5 | ✅ | `33c355ef0` | SpinLock（byref 写回）+ SpinWait + ThreadPool 可调用面 |

**T2.3 的处置不是失败而是范围修正**：`grep -c` 证实 chunk 的
`CombinedSubjects.cs` / `native-aot.generated.cpp` 中 Barrier 与 CountdownEvent
均无 subject，连 fallback 符号都不存在。注册代码写了也不会有 subject 匹配它。
**待 ATG 侧产出这些 subject 后再补**，当前不占用 Phase 2 工时，也不记为已完成。

**T2.2/T2.4/T2.5 的验收口径已修正**（详见 roadmap 的
`### ⚠️ T2.2/T2.3 的验收口径需修正` 与 `### 🔴 比上面更深一层` 两节）：

- ❌ 不用 `real%` 判注册是否生效 —— 该 chunk `SemaphoreSlim` 的 real 用例数为 **0**；
- ❌ 更不用 `passed: true` —— 实测 656 条 `real` 中 **645 条 `value == 0`**，
  与「抛异常被 `catch (const chaos_managed_exception&) { return {}; }` 吞掉」
  在 fact 记录里**不可区分**；
- ✅ 唯一可信证据是 native 层**可证伪的行为测试** + 生成产物里 fallback 体 → `Chaos*` 直调。

**独立于本 roadmap 的缺陷**：harness 应在异常路径上把 subject 标为
`assertFailed` 而非静默 `return {}`。这是跨域指标缺陷，已记入 memory
`fact-real-kind-does-not-detect-thrown-exception`，**建议单开 issue**。

### Phase 3 — 加固（T3.1-T3.3）

| task | 结果 | commit | 说明 |
|---|---|---|---|
| T3.1 | ✅ | `cec2e0973` | 非原子 static 桥变量 → `std::atomic` + `AllocId` |
| T3.2 | ✅ | `dfc380d5f` | 句柄表槽位回收 + 并发 claim（**修出第二个缺陷**） |
| T3.3 | ✅ | `cec2e0973` | 热路径 `fprintf` → 分级 log 宏（`delegate_helpers.cpp` 7 处） |
| T3.4 | 本文档 | — | 文档与实现对齐 |
| T3.5 | ⏳ | — | benchmark 填充（未验证是否仍为空模板） |

**T3.1 范围修正**：design 称「8 处」。实测受影响的只有
`synchronization.cpp` 的 4 个 id 计数器加 `wait_handle.cpp` 的 1 个；
`cancellation_token.cpp`(×2) 与 `timer_queue.cpp`(×1) 的同名计数器**已有锁保护**，
不是缺陷。原「8 处」把受保护与不受保护的混为一谈。

**T3.2 在修 (a) 时暴露出 (b)**，两者独立：

- (a) `Destroy` 只设 `active=false` 不清 `id` ⇒ 槽位对分配器永久不可见，
  1023 次**生命周期内**创建即表满（即使从不重叠存活）；
- (b) 修完 (a) 后并发测试立刻报 **90 次**失败：多个线程同时看到 `id == 0`，
  都 claim 同一槽位 ⇒ 两个对象共用一槽。改用 `compare_exchange_strong` 原子 claim。

隔离验证：并发用例修前**每次**失败、修后**连跑 5 次**全绿。

### 仍未解决

### 已关闭

`test_parallel_for` 失败（`mutex destroyed while busy`）：**已于 2026-09-14 取证并修复**（commit `c0670a6c1`）。根因是测试中 `std::lock_guard<std::mutex>` 的作用域覆盖了整个剩余函数体，`delete mu` 时该 mutex 仍被本线程持有且曾被 worker 线程通过 `static` 指针访问过，MSVC STL Debug 断言检测到 owner thread id 仍被设置。修复后将断言放入独立作用域，使 `lock_guard` 在 `ThreadPoolShutdown` 前释放。**推翻此前「预存在失败、可能与线程池收尾同族」的推测**——真因是纯测试缺陷，与实现无关。

