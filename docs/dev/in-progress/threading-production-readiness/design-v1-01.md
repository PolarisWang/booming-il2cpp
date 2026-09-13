# design-v1-01 — Threading 生产级就绪

> **任务 ID**：`threading-production-readiness`
> **类型**：roadmap（多阶段、跨会话）
> **依据**：本轮三路审计（native 实现面 / 验证覆盖度 / codegen 翻译缺口）+ 证据链核验
> **日期**：2026-09-13

---

## 0. 起点事实（已核验，非推断）

| 事实 | 证据 |
|---|---|
| 三个 threading chunk `build.status` 全为 `error` | 三份 `results/provenance.json`；`results/` 下只有 provenance，**无 fact.json** |
| `threading` / `threading-2` 在 2026-09-12 honest report 中**完全缺席** | 70 行中做差集，二者是**唯一**有产物却无报告行的 chunk |
| `threading-2` 是真实 chunk（151 方法，占 threading+threading-2 的 43.0%） | 权威 partition `tests/e2e/translation/.../namespace-partition.json`（200/chunk）；nightly `config.py:37` → `worklist.py:72,86-89` 遍历其全部 chunks。**注**：分母是 threading+threading-2 合计 351；若按三个 threading chunk 合计 461 计为 32.8% |
| `threading` 的 `crt_stubs.cpp` 仍是旧签名 | 第 5 行 `int __cdecl _Thrd_sleep_for(const void*, void*)`，Sep 10 00:18 |
| **28** 个 native threading 测试文件（**244 用例**）**零 CI 门禁** | 实测 `ls tests/unit/runtime-native/runtime-core/threading/*.cpp` = 28；用例宏扫描 244。CI：`ci-framework.yml:143,156,173` 与 `codegen-regression.yml:121,124,135` 的 `-R` 正则均不含 threading |
| benchmark 覆盖为零 | 4 个 threading benchmark HTML 全为空模板 |

**🟡 已修正（2026-09-13 对抗性审查 Y1）**：测试规模原写「15 文件 / 约 126 用例」，
**实测为 28 个 `.cpp` 文件 / 244 个用例**（`tests/unit/runtime-native/runtime-core/threading/`）。
原因是该目录含 async 家族测试（128 用例），原统计口径遗漏。
**所有验收标准的分母必须用 244，不得用 126**，否则通过率会算错。
**⚠️ 诚实标注**：chunk build 失败的**具体编译错误未实测**，报告中的原因（陈旧 crt_stubs）是推断。本计划 Task 0 即为此设。

---

## 1. 边界拍板

### 做什么（全量 0-4）

| 阶段 | 内容 | 域 |
|---|---|---|
| **Phase 0** | 止血：重建 3 chunk、实测 build 错误、堵假绿向量、CI 接入 | build / testing / verification |
| **Phase 1** | 关闭语义造假：CancellationToken 3 点、Parallel、SynchronizationContext | runtime |
| **Phase 2** | 接线缺 ABI 出口的 12 个原语 —— **🔴 成本大幅上调，需检查点重新评估优先级**（见 §12） | codegen + runtime |
| **Phase 3** | 加固：非原子 static、句柄表、热路径 fprintf、文档不符、benchmark | runtime / testing |
| **Phase 4** | 制度化：门禁阈值、回归告警 | verification |

**🔴 原判断修正**：审计报告称 Phase 2 是「native 实现已存在且真实，纯接线」——
经核验**不成立**。`synchronization.h:12,32` 与 `wait_handle.h:24` 的 API 是**内部 C++ 句柄风格**
（`uint32_t SemaphoreSlimCreate(...)`），**无 `extern "C"`**，codegen 生成的 C++ 无法直接调用。
详见 §12。

### 不做什么（非目标，明确排除）

| 排除项 | 理由 |
|---|---|
| **hotupdate slot mapping** | 用户拍板**拆出独立任务**。根因 `ApplyPatchFromMemoryEx` 返回 null 与 threading 无关，且已有独立 handoff 文档 |
| **POSIX 平台缺陷**（`pal_wakeable_posix` 丢回调、`pal_suspend_posix` 恒返 unsupported） | 仅 Windows 目标；登记为 watch_item 不阻塞 |
| **Thread.Abort 语义**（`state_obj` 被忽略） | .NET Core 本身已弃用 Thread.Abort，无需对齐 |
| **前缀过捕获** | 审计确认**未发生**，属误报类，无需处理 |

### 偏差声明（精确措辞）

**本计划截至定稿时，未修改 threading 方向的任何实现代码（C++ / C# / Python）。**
设计文档（`design-v1-01.md` / `STATUS.md`）的创建与修改本身不计入实现改动。

**⚠️ 基线漂移披露（对抗性审查 R3）**：定稿期间工作区**并非静止**。`git diff --stat HEAD` 可见：

```
src/managed/Chaos.IL2CPP.Generator/RuntimeSupport/RuntimeHelperShapeRegistry.CoreStubs.Part1.S16.cs | 8 +++++++-
tests/e2e/translation/sdk/windows-x64-reference/.source_hash                                        | 3 ++-
src/native/runtime-core/reflection/attributes.cpp    （暂存区修改）
src/native/runtime-core/reflection_api.h             （未暂存修改）
```

外加大量未跟踪文件（`sarif_out/`、`nj*.json`、`wr.json`、`docs/dev/in-progress/dispatch-overhead-fix/` 等）。

⇒ **「当前会话未修改实现代码」这一主张在基线漂移下无法独立验证**，故按上句精确表述：
声明范围限定为「threading 方向」。非 threading 方向的并发活动见 §8 风险表最后一行的修订条目。

---

## 2. authority 决策

| 决策点 | 结论 |
|---|---|
| 权威 partition | `tests/e2e/translation/System.Private.CoreLib/_dll/namespace-partition.json`（200/chunk，有 threading-2）。`manifest.py:110-113` 仅为**默认命名映射表**，非权威 |
| 产物根 | `build_root()` → `<repo>/artifacts/foundation-dll/`（gitignored，可丢弃） |
| 源根 | `foundation_root()` → `tests/e2e/translation/`（nightly 经 `run.py:240` 显式设置 `CHAOS_FOUNDATION_DLL`） |
| 测试权威树 | `tests/e2e/verification`（`testing/foundation-dll/verification` 为已废弃僵尸树，preflight 会拒绝其复现） |
| 修改授权 | **四层全开**：native / codegen(C#) / 验证管线(Python) / CI 配置 |

---

## 3. 执行策略

- **阶段内并行，阶段间串行**
- **Phase 0 后设检查点**：拿到真实 fact 数据后重新评估剩余阶段优先级（真实数据可能推翻报告推断，尤其 real% 与失败分布）
- CI 接入方式：**新建独立 workflow**（不进 ci-framework，便于单独重跑与查看报告）
- ≥2 域任务按 CLAUDE.md 走 Workflow 委托
- **worktree 策略**：见 §3.1 —— 主工作区止于 T0.0，T0.1 起全部在 worktree 内

---

## 3.1 worktree 切出时机（决策）

### 结论

**T0.0（实测 build 错误）留在主工作区；T0.1 起全部动作在 worktree 内。**

切成三个物理工作区：

| 工作区 | 覆盖 | 理由 |
|---|---|---|
| **主工作区**（`D:/agent/chaos-il2cpp`，`main`） | T0.0 只读取证 + **全程的构建/验证读** | 已存在构建缓存、工具已装；主工作区不写代码，不会被并发 agent 抹掉 |
| **worktree-A** | Phase 0 代码改动（T0.1-T0.4） | 独立分支，改动有落点 |
| **worktree-B** | Phase 1-4 | 与 A 串行，阶段间跨 worktree 交接 |

**worktree 在 T0.0 完成、且拿到实测错误类型之后立即切出**，不早于也不晚于。

### 为什么不是「Phase 0 一开始就切」

T0.0 是**纯读取证**（跑 `chunk_pipeline --stages build` 看真实编译错误），不产生代码改动，
**不需要隔离**。而它需要主工作区已有的构建缓存与已安装工具链 —— 换到新 worktree 会
失去这些（`artifacts/` 与 `testing/artifacts/` 均为 gitignored，不入 worktree）。

### 为什么不是「Phase 0 完全结束才切」

T0.0 之后（T0.1 清除残留 / T0.2 重建 / 改 stub / 改 Python / 改 CI）开始产生**代码改动**，此时：

- 主工作区当前**已有并发 agent 活动**（见 §8 风险行「并发 agent 抹除改动」）
- 记忆 `parallel-agent-clean-checkout-clobber` 记录了「已提交改动被并行 agent 整树覆盖」的反复事故
- 无 worktree 时改动直接躺在共享工作区，暴露在竞态下

### 关键工程事实（已验证，决定了 worktree 的物理边界）

1. **产物根不在 worktree 内。** `build_root()` 锚定
   `_HERE.parents[2] / "artifacts" / "foundation-dll"`，即**运行脚本所在那棵树**。
   在 `.claude/worktrees/<name>/` 里跑时，`_HERE` 已解析为 worktree 路径
   （`Path(__file__).resolve()`），所以产物会落到
   `<worktree>/artifacts/foundation-dll/`，而 `artifacts/` 被 `.gitignore:36` 忽略。
   `testing/artifacts` 同样被忽略（`.gitignore:632`）。

2. **本次任务涉及 native 编译，工具链是仓库级的。** `_repo_root()` 同样按
   `Path(__file__).resolve()` 上溯找 `.git`，在 worktree 内即解析到 worktree 根。
   Generator/TPG 的 DLL 在 worktree 里是**冷启动**（首次构建需重编工具），
   这是隔离的代价，需在 T0.1 开工时预留构建时间。

3. **`EnterWorktree` 的 worktree 落在 `.claude/worktrees/`**（目录已存在），
   与主工作区不共享 `artifacts/` —— 因此**每换一个 worktree，Phase 0 的 chunk
   需要重新构建**。这是把 Phase 0 与 Phase 1-4 分成两个 worktree 的**主要成本**；
   若构建耗时不可接受，退化为**单个 worktree 覆盖 Phase 0-4**（见下方备选）。

### 切出动作

```
1. 主工作区：git status 确认无未提交的 threading 相关改动（防并发 agent 混入）
2. EnterWorktree(name=threading-phase0)
3. 在 worktree 内：git checkout -b feat/threading-phase0
4. 归档 design-v1-01.md / STATUS.md 的改动（文档本身属源码，随分支走）
```

**主工作区不 stash、不 reset** —— 符合 CLAUDE.md「禁止 git stash」。
未提交的文档改动随 worktree 分支提交。

### 阶段交接

- Phase 0 收口时：worktree-A 的改动**必须已 commit**，否则切 worktree 会丢失工作区
- Phase 1-4 在 worktree-B 内推进；两阶段之间的交接通过 **main 上的 merge** 完成
  （Phase 0 的修复必须先落 main，Phase 1 才能基于它工作）
- **每次跨 worktree 前重跑一次完整验证** —— 新 worktree 的 `artifacts/` 是空的，
  旧 worktree 的绿色结果不继承

### 备选路径（若构建成本过高）

**单 worktree 覆盖 Phase 0-4**：T0.0 完成后切出**一个** worktree，全程不换。
- 优点：`artifacts/` 只建一次，构建成本摊销
- 缺点：失去「阶段间隔离」这道防线；一旦 Phase 1 改动污染 Phase 0 的产物，
  难以定位
- **裁决**：默认走双 worktree；若 T0.2 实测构建时间 > 30 分钟，
  改走单 worktree 并在 STATUS.md 记录该裁决

---

## 4. 组件与职责边界

```
┌─ Phase 0 ────────────────────────────────────────────┐
│  T0.0 实测 build 错误（决定性：计划形状由它决定）      │
│       ⚠️ 主工作区执行（只读取证，不切 worktree）        │
│       ⚠️ 无保留构建日志，实际是一次完整重建            │
│  ─────────── ✂️ worktree 切出点（T0.0 完成即切）────────│
│  T0.1 清除源树陈旧 fact.json（🔴 假绿向量，见 §7.1）   │
│  T0.2 重建 threading / threading-2 / threading-tasks  │
│  T0.3 取证 BuildMethodSourceSafe 是否吞 threading 异常 │
│  T0.4 新建 threading native test workflow             │
│  → 检查点：重新评估 Phase 1-4 优先级                   │
│  → 收口：改动必须 commit 后 merge 回 main              │
└──────────────────────────────────────────────────────┘
                  ↓ merge 到 main（Phase 1 的前置）
┌─ Phase 1 ── runtime 语义 ────────────────────────────┐
│  T1.1 CancellationToken throw_if_... 真实抛出         │
│  T1.2 source_get_token 构造真实 token                 │
│  T1.3 CreateLinkedTokenSource 实现或显式拒绝          │
│  T1.4 Parallel.failed 接线 + 结果返回 + 异常传播       │
│  T1.5 SynchronizationContext::Post 真实入队            │
└──────────────────────────────────────────────────────┘
┌─ Phase 2 ── codegen 接线 ────────────────────────────┐
│  ⚠️ 原标「杠杆最大」已撤销 —— 成本也最大，见 §12       │
│  T2.0 🔴 新建 extern "C" ABI 出口层（前置，非可选）    │
│  T2.1 托管对象 ↔ native 句柄 映射机制                  │
│  T2.2-2.5 分批注册 12 个原语                           │
│  ⚠️ 其中 CancellationToken/CancellationTokenSource/    │
│     SynchronizationContext/Parallel/Interlocked 全族/  │
│     Volatile 全族 **已在 ShapeRegistry 中**，           │
│     本阶段只处理「已注册但缺 ABI 出口」的原语            │
└──────────────────────────────────────────────────────┘
┌─ Phase 3 ── 加固 ────────────────────────────────────┐
│  T3.1 非原子 static 桥变量（8 处）                    │
│  T3.2 句柄表真正释放                                  │
│  T3.3 移除热路径 fprintf                              │
│  T3.4 文档不符修正                                    │
│  T3.5 benchmark 填充                                  │
└──────────────────────────────────────────────────────┘
┌─ Phase 4 ── 制度化 ──────────────────────────────────┐
│  T4.1 threading chunk 门禁阈值 + 回归告警             │
└──────────────────────────────────────────────────────┘
```

> **worktree 边界**：Phase 0 在 `worktree-A`（T0.1 起），Phase 1-4 在 `worktree-B`；
> 阶段间经 main 的 merge 交接。详见 §3.1。

---

## 5. 关键控制流 / 数据流

**验证信号链**：
```
namespace-partition.json (37 chunks，含 threading-2)
  → nightly/worklist.py 发现 workitem
  → chunk_pipeline --stages build,fact
  → codegen (T4→C++) → cmake build → entry.exe
  → entry.exe --fact-json → factResults[]
  → fact_chunk.py 分类 resultKind
  → fact.json: realVerified / realTotal / gateTotal
  → honest-report 门禁
```

**当前断点**：`build` 阶段即失败 → 后续全断。

---

## 6. error handling 与边缘情况

| 场景 | 处理 |
|---|---|
| T0.0 实测出**新**错误类型（非 crt_stubs） | 以实测为准，报告推断作废；重新评估 Phase 0 形状 |
| rebuild 后 `threading-2` 仍失败 | 单独隔离，不阻塞 `threading`；记录到 watch_items |
| Phase 2 注册后 real% 未显著提升 | 说明存在第二道断点，回到取证（**不得**直接加大注册量） |
| `BuildMethodSourceSafe` 确认吞异常 | 提升为 Phase 0 必修项（用户已拍板提到 Phase 0） |
| 陈旧 `threading` 产物与 `d5d0630da` 交互 | `d5d0630da` 已移除 write-once 守卫，**重建即自动覆盖**，无需手工删。（**Y4 标注**：该机制在 **text chunk** 已验证；`.hephaestus-cache/` 中**无 threading 条目**，故 threading 专属验证在 T0.2） |

---

## 7. testing 与验证方式

- **每个修复必须有反例验证**（承袭 async-task-industrialization 的纪律）：in-place revert 后对应测试必须失败，且不依赖该性质的测试保持绿
- **诚实负数结果必须记录**：若某检查被证明不承重（如 P2-5 前缀碰撞那样），显式标注为冗余并回退，不得保留未证实的改动
- **不得用陈旧 DLL 假绿**：任何测试变绿必须确认读的是当前构建产物
- Phase 0 验收：三个 chunk 在**产物根**产出 `fact.json`，`build.status=passed`，且 CI workflow 跑出 **244** 用例结果

---

## 7.1 🔴 验收读取根必须钉死在产物根（对抗性审查 R1）

### 问题：当前存在一个**活着的假绿向量**

`reporting.py:57` 读的是：

```python
_enrich_with_chunk_facts(asm_data, foundation_dir / asm_name / "chunks")
```

而 `chunk_pipeline.py:321` 设 `foundation_dir = _FOUNDATION_DLL / assembly`，
`_FOUNDATION_DLL = foundation_root()` —— **这是源树**（`tests/e2e/translation/`），
不是产物根（`artifacts/foundation-dll/`）。

于是 honest report 的「chunk fact 富化」实际上是从**源树的 chunks/ 目录**读 `fact.json`。
而 `fact_chunk.py:305` 写的是 `ctx.chunk_dir / "results" / "fact.json"`，
`chunk_dir` 按 `context.py:43-45` 是**产物根**。

**两者不是同一个目录。**

### 现场证据（2026-09-13 实测）

| 路径 | 内容 | mtime |
|---|---|---|
| `tests/e2e/translation/.../threading-tasks/results/fact.json` | `passed:456, realTotal:135` | **2026-09-08 14:54** |
| `artifacts/.../threading-tasks/results/provenance.json` | `build.status: error`，`runId: fdn-20260913-001` | 当前 run |
| `artifacts/.../threading-tasks/results/fact.json` | **不存在** | — |

即：honest report 里 `threading-tasks ... 456 456 78 ... PASS` 那一行，
读的是**五天前的源树残留**，与当前 `build.status=error` 完全无关。

`threading` / `threading-2` 源树下无 `fact.json`，故它们在 70 行报告中缺席 —— 症状同源。

### 为什么这不是「脏工作区」而是计划必须处理的事

- 该路径**已被 `.gitignore:600` 忽略**（`tests/e2e/translation/**/chunks/`），
  所以 `git clean -fdX` 能清掉，但**没有任何门禁会自动清它**
- `git ls-files` 确认它**未被 git 跟踪** ⇒ 不会随 worktree 建立而出现，
  但也**不会随 `git status` 暴露**
- 只要源树残留存在，**每一次 honest report 都会被污染**，且污染方向总是「比实际更绿」

⇒ **这是设计 §7 验收标准的缺陷**：原文只写「产出 `fact.json`，`build.status=passed`」，
未指定读取哪个根。按现状，Phase 0 可以在 chunk 实际 `build.status=error` 的情况下
被误判为「已通过」。

### 修正

**1. 新增 T0.1「清除源树陈旧 fact.json」**（见 §4 组件图，已插入）

```bash
# 在 worktree 内执行，清除三个 threading chunk 的源树残留
rm -f tests/e2e/translation/System.Private.CoreLib/chunks/threading{,-2,-tasks}/results/fact.json
```

> ⚠️ 用 `rm -f` 显式列举，**不要**用 `git clean`（会连带删除其他未跟踪的源树内容；
> 且源树 chunks/ 下还有 `chunk.json` 等版本控制内容需要保留）。

**2. 验收读取根钉死为产物根**

Phase 0 的验收**必须**读：

```
artifacts/foundation-dll/System.Private.CoreLib/chunks/<slug>/results/fact.json
```

**不得**读 `tests/e2e/translation/.../results/fact.json`。

**3. 加 mtime / runId 一致性断言**

`fact.json` 本身不含时间戳，故用**旁证**：

- `fact.json` 的 mtime **必须晚于**同目录 `provenance.json` 的 mtime
- 或：同目录 `provenance.json` 的 `runId` 必须等于本次 Phase 0 的 runId
  （当前为 `fdn-20260913-001`，Phase 0 执行时会变）

**凡不满足者，一律判定为「未完成」，不得计入验收。**

### 遗留（本计划不修，但必须登记）

`reporting.py:57` 读源树而非产物根，是**验证管线自身的缺陷**，
影响的不止 threading。修它需要单独评估（可能改变所有 chunk 的报告行为）。
**本计划只做规避（T0.1 + 验收标准钉死），不动 `reporting.py`。**

> ⚠️ **本缺陷的影响面超出 threading**：`reporting.py:57` 对**所有 chunk** 都读源树。
> 本计划不修它，但一旦修复，所有 chunk 的历史报告数据都可能需要重新解读。
> 这是「先记录、后评估」的典型 —— **不得**在 Phase 0 顺手改掉。

建议在 Phase 4（制度化）时评估是否修复，或登记为独立任务。

---

## 8. 风险评估

| 风险 | 触发条件 | 后果 | 缓解 |
|---|---|---|---|
| **T0.0 暴露的根因与推断不符** | 实测错误非 crt_stubs | Phase 0 形状需重写 | 计划已把 T0.0 设为 Task 0；**不得**在无实测情况下开工 T0.2 |
| **T0.0 实际是一次完整重建（非「读取错误」）** | 构建目录只有 `CMakeOutput.log`，**无保留的构建日志**；且各 chunk 下**无 `entry.exe`** | 耗时不符预期；若重建后转绿则根因既无法证实也无法证伪 | 见下方 §8.1「T0.0 的真实形态」 |
| **重建后 chunk 意外转绿** | 错误为暂态，或 `d5d0630da` 之外的其他修复已生效 | 对根因的推断落空，Phase 0 需重新定义 | 转绿**不等于**推断正确 —— 必须回溯确认到底哪一处改动生效；**不得**因转绿就跳过 T0.0 的取证目的 |
| **`BuildMethodSourceSafe` 是假绿源头** | 取证确认其吞 threading 异常 | Phase 0 拿到的绿色不可信，后续全部验证失去意义 | 已拍板提到 Phase 0；取证优先于 rebuild |
| **Phase 2 接线后 real% 不升** | 存在第二道未知断点 | 杠杆最大的动作失效 | 设专项取证；**三次规则**——同一断点三次未解则转 dev-brainstorm |
| **重建触发 cross-chunk 回归** | crt_stubs 无条件重写影响其他 chunk | 已有绿色 chunk 变红 | 重建后对比 honest report 前后差异；单 chunk 隔离验证 |
| **hotupdate 拆出后失联** | 无追踪载体 | 该问题被遗忘 | 明确登记为独立任务（用户已拍板） |
| **244 用例接入 CI 后暴露大量失败** | 测试长期未跑已腐烂 | CI 长期红 | 首跑用 `continue-on-error` 收集基线，再决定阻断阈值 |
| **主工作区被并发 agent 抹除改动** | 已有并发 agent 在活动，**且其改动直接触及 threading** | Phase 0 改动丢失、误提交他人工作、ShapeRegistry 注册状态在本计划不知情下变化 | T0.0 后**立即切 worktree**（§3.1）；每次 commit 前 `git diff --cached --name-only`；参见 §8.2 |
| **worktree 冷构建成本超预期** | 新 worktree 无 `artifacts/`，工具链需重建；且 T0.0 本身已是一次完整重建 | T0.2 起步耗时不可控 | T0.2 首步即测构建耗时；> 30 分钟则退化为单 worktree（§3.1 备选路径） |
| **跨 worktree 交接丢弃未提交工作** | Phase 0 收口时改动未 commit | 整阶段成果丢失 | 切 worktree 前强制 `git status` 检查；改动必须先 commit 再 merge |

---

## 8.1 T0.0 的真实形态（对抗性审查 Y3）

**T0.0 名义上是「读取已保存的构建错误」，实际上已无错误文本可读。**

实测：构建产出目录只有 `CMakeCache.txt`、`CMakeOutput.log` 与 `.obj` 文件，
**没有任何保存的构建日志**；各 chunk 目录下也**无 `entry.exe`**。

⇒ T0.0 **必然是一次完整重建**，不是一次读取。影响：

- 耗时按「完整重建」预估，不按「读取日志」预估（见 §8 冷构建风险行）
- §3.1 的 worktree 切出点因此更关键：T0.0 在主工作区跑完完整构建，
  T0.2 在 worktree 里**还要再冷构建一次** —— 这是双 worktree 的主要成本
- **若重建后 chunk 转绿**，则对根因（陈旧 crt_stubs）的推断既无法证实也无法证伪。
  **转绿不等于推断正确** —— 必须回溯确认生效的到底是哪一处改动，
  **不得**因转绿就跳过 T0.0 的取证目的。

## 8.2 并发 agent 触及 threading 自身（对抗性审查 Y6）

原风险条只写「reflection 方向的并发 agent」，**这个定性不准确**。

`git diff --stat HEAD` 显示并发改动包含：

```
src/managed/Chaos.IL2CPP.Generator/RuntimeSupport/RuntimeHelperShapeRegistry.CoreStubs.Part1.S16.cs
```

**这正是 threading codegen 注册表本身**（该文件含 `RegisterCancellationToken`（:1680）、
`RegisterCancellationTokenSource`（:1720）、`RegisterSynchronizationContext`（:1818）、
`RegisterParallelLoops`（:2025））。

⇒ 风险不是「别人的无关活动」，而是：**threading 原语的注册状态可能在本计划不知情的情况下发生变化**，
直接改变 Phase 2 的工作面（哪些原语缺 ABI 出口）。

**处置**：
- Phase 2 开工前**必须重新盘点** ShapeRegistry 中 threading 原语的当前注册状态，不得沿用本设计的清单
- 切 worktree 后，若并发 agent 的 `Part1.S16.cs` 改动已进 main，需 rebase 并重评 §12

---

---

## 9. 三优先级权衡

| 方案 | P1 性能最优 | P2 架构完美 | P3 HotUpdate |
|---|---|---|---|
| **A. 严格串行 0→4** | 中性 | 高（每步验证充分） | 中性 |
| **B. 阶段内并行、阶段间串行**（**采纳**） | 中性 | 高（阶段门禁不放松） | 中性 |
| **C. 全并行** | 低（争抢构建资源） | 低（验证信号互相污染） | 中性 |
| **D. 只做 Phase 2 接线** | 中 | 低（留 P0 语义造假） | 中性 |

**采纳 B**。理由：
- threading 是**运行时基础设施**，其正确性影响所有上层 —— 性能（P1）的前提是语义正确，而当前存在会**静默产出错误结果**的 P0 缺陷（`throw_if_cancellation_requested` 空体、`Parallel` 恒返 -1），必须先于任何性能优化处理
- 架构完美性（P2）体现为「阶段门禁不放松」，B 满足
- HotUpdate（P3）无冲突：本计划不触碰 hotupdate 路径，且该问题已拆出独立任务
- **无优先级冲突需裁决**

---

## 10. 问题清零

```yaml
blocking_questions: []
question_clearance: cleared
clearance_confirmed_by_user: true
```

**已拍板（用户确认）**：
1. 范围 = 全量 Phase 0-4
2. 授权 = 四层全开（native / codegen / 验证管线 / CI）
3. 策略 = 阶段内并行、阶段间串行
4. T0.0 实测 build 错误写入计划作为 Task 0
5. Phase 0 后设检查点，重新评估剩余优先级
6. `BuildMethodSourceSafe` 假绿向量提到 Phase 0
7. CI 接入 = 新建独立 workflow
8. hotupdate slot mapping = 拆出独立任务

**watch_items**（执行中观察，非待拍板）：
- POSIX 平台缺陷（仅 Windows 目标，不阻塞）
- `threading-2` 若长期无法转绿，是否降级为已知限制
- 首跑 244 用例的失败基线分布

---

## 11. 下一步入口

`roadmap`（满足：≥2 阶段、≥2 相对独立子任务、跨多次会话）

前 3 轮讨论已提供充足全局设计上下文，Phase 0-4 各子任务可在轻量级模式下推进。

---

## 12. 🔴 Phase 2 前置修正（决策性发现，2026-09-13 核验）

### 原判断

审计报告称 Phase 2「12 个原语的 native 实现已存在且真实，**纯接线**」。

### 核验结果：**不成立**。存在两层鸿沟。

**鸿沟一：无 ABI 出口。**

| 位置 | 签名 | 可被 codegen 调用？ |
|---|---|---|
| `synchronization.h:12` | `uint32_t SemaphoreSlimCreate(int32_t, int32_t) noexcept;` | ❌ 内部 C++ 风格 |
| `synchronization.h:32` | `uint32_t ReaderWriterLockSlimCreate() noexcept;` | ❌ |
| `wait_handle.h:24` | `uint32_t WaitHandleCreate(bool, WaitHandleType) noexcept;` | ❌ |
| `interlocked_stubs.h:4-11` | `ChaosInterlocked*` + `extern "C"` | ✅ 先例 |
| `threading_stubs.h:23,26` | `chaos_monitor_enter` / `ChaosMonitorTryEnter` + `extern "C"` | ✅ 先例 |

codegen 能调用的符号全部集中在 `runtime_stubs/*.h`，用 `extern "C"` + `CHAOS_IL2CPP_INTPTR` ABI 类型。
`synchronization.h` / `wait_handle.h` **完全没有 `extern "C"`** —— 它们是给 runtime 内部与 native 测试用的。

**鸿沟二：无托管对象 ↔ native 句柄映射机制。**

- 托管侧：`SemaphoreSlim.Wait()` 是**实例方法**（`this` + `timeout` + `CancellationToken`）
- native 侧：`SemaphoreSlimWait(sem_id, timeout)` 是**句柄函数**（`g_semaphores[1024]` 槽位 ID）
- 全仓库检索 `chaos_semaphore` / `semaphore_handle` / `ManagedHandle` —— **无任何映射机制先例**

⇒ 需要一个新机制：托管 `SemaphoreSlim` 实例 ↔ native 槽位的绑定、创建时机、生命周期与 GC 交互。

### 对计划的影响

Phase 2 工作量**上调一个量级**：不是 12 个注册条目，而是
**1 套 ABI 出口层 + 1 套句柄映射机制 + 12 组语义映射**。

这也解释了现状：这些原语之所以「未注册」，可能不是遗漏，而是**当初就没有可用的 ABI 面**。

### 🟡 「未注册」的定性过宽（对抗性审查 Y5）

**部分原语其实已经在 ShapeRegistry 中注册了**（HEAD 已确认）：

| 已注册（在 ShapeRegistry） | 位置 |
|---|---|
| `RegisterMonitor` / `RegisterThread` / `RegisterThreadSleep` / `RegisterThreadYield` | `RuntimeHelperShapeRegistry.CoreStubs.Part1.S11.cs:14,37,74,92` |
| `RegisterCancellationToken` / `RegisterCancellationTokenSource` | `...Part1.S16.cs:1680,1720` |
| `RegisterSynchronizationContext` / `RegisterParallelLoops` | `...Part1.S16.cs:1818,2025` |
| `RegisterInterlockedMemoryBarrier` / `Read` / `Increment` / `Decrement` / `Exchange` / `CompareExchange` | `...Part2.S22.cs` / `S23.cs` |
| `RegisterVolatileRead` / `RegisterVolatileWrite` | `...Part3.S4.cs` |

（注：已注册 ≠ 语义正确 —— 其中 `CancellationToken` / `Parallel` 的 **native 体是空的**，
那正是 Phase 1 要修的「语义造假」。）

⇒ **Phase 2 的正确定性是「缺 ABI 出口层的原语」，不是「未注册的原语」。**
确需 Phase 2 处理的是这组**确实没有 `extern "C"` 出口**的：
`SemaphoreSlim` / `ReaderWriterLockSlim` / `WaitHandle` / `Barrier` / `CountdownEvent` /
`Timer` / `SpinWait` / `SpinLock` / `ThreadPool` 可调用面 / `Parallel.Invoke`。

**开工前必须重新盘点**（理由见 §8.2 —— 并发 agent 正在改 `Part1.S16.cs`）。

### 修正后的 Phase 2 结构

```
T2.0  🔴 新建 extern "C" ABI 出口层（前置，不可跳过）
      — 参照 interlocked_stubs.h / threading_stubs.h 既有模式
T2.1  托管对象 ↔ native 句柄映射机制（新机制，无先例）
T2.2  注册 SemaphoreSlim + ReaderWriterLockSlim（最难，含 upgradeable）
T2.3  注册 Barrier + CountdownEvent
T2.4  注册 ManualResetEvent(Slim) + AutoResetEvent + Timer
T2.5  注册 SpinWait + SpinLock + ThreadPool 可调用面 + Parallel.Invoke
```

### 决策点（需在 roadmap 阶段重新评估）

Phase 2 从「杠杆最大、成本最低」变成了**「杠杆最大、成本也最大」**。
Phase 0 检查点时需重新权衡：是否先做 Phase 1/3（成本确定），把 Phase 2 拆成独立后续任务。

**本修正是基于代码核验的事实修正，不是猜测。** 原审计报告的措辞需在 roadmap 中同步更正。
