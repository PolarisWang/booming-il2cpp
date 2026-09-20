# threading-tasks fact 挂死 — 验证结论与交接状态校正（2026-09-20）

> 上游：`handoff-box-unwrap-breaks-task-handle.md`
> 本文目的：**校正交接文档的过时自述** + 记录本轮 fact 挂死复现实证

## 一、结论摘要

| 项 | 状态 | 证据 |
|---|---|---|
| C1 闭包 lambda 修复 | ✅ **已闭环** | `da9fe2c57` 落地 / `8768cf2da` 合入 main |
| C1 的 threading chunk 验证 | ✅ **已完成**（合并前已做） | merge commit `regression_check` 段 |
| C1 的 threading-tasks 回归 | ❌ **被阻塞** | fact 阶段挂死，见 §三 |
| box 拆解挂死（交接 §一~§八） | ❌ **仍未修复，main 上可复现** | 本轮实测，见 §三 |

## 二、交接文档自述已过时（重要）

`handoff-box-unwrap-breaks-task-handle.md` 的 §九 写着：

> - ❌ 未动手修
> - ⏳ 遗留验证（未做）

**这在 2026-09-20 13:30 写作时正确，但已过时。** 实际情况：

- `da9fe2c57`（2026-09-20 10:02）**早于**交接文档（13:30）就已完成 C1 落地
- `8768cf2da`（10:07）合入 main，其 `regression_check` 段记录了完整验证：

```
regression_check:
  worktree `closure-lambda`（base bdf0630ba，pre-GUO-WIP），threading chunk
  build+fact 实测（nativeConfig=check）：
  - lambda b__ fallback stub：24 → 0（12 各含 Benchmark 变体）
  - <>c::.cctor/.ctor fallback：6 → 6（未变，无回归）
  - EnsureInitialized_3 的闭包 b__14_0 已生成真实体（`_s0=0; return`），
    同族 LazyInitializerTests::EnsureInitialized_2 由 failed → passed(real)
  - snapshot 108 项：86 fail / 22 pass —— 在**回退本改动后同一数字**，
    失败根因为 fixture 临时目录缺 SnapshotTestFixtures.dll（FileNotFound），
    与本次改动无关（A/B 实测，非推断）
  - 残余 2 个 failed（EnsureInitialized_3 / GetCurrentProcessorId_60）已逐个
    归因，均与本改动无关
```

### 教训（可复用）

> **「交接文档说未做」≠「现在仍未做」。**

接手交接任务时，**必须先查** `git log --grep="<关键词>"` 与相关 merge commit 的
`regression_check` 段，**不能只读交接文档自述**。

本轮据此误判，启动了一次**完全冗余的 threading chunk pipeline run**（发现后已终止）。

## 三、fact 挂死复现实证（本轮新增）

### 3.1 复现环境

- commit：`0b93e92f2`（main）
- 命令：`CHAOS_FOUNDATION_DLL=<repo>/testing/foundation-dll python -m verification.chunk_pipeline
  --assembly System.Private.CoreLib --chunk threading-tasks --stages build,fact`

### 3.2 build 阶段：✅ 通过

```
[build] Chunk methods: 110
<<< + [passed] 599 subjects -> entry.exe (297980ms)
[build] JIT entry-jit.exe: ... (12894720 bytes)
```

### 3.3 fact 阶段：❌ 挂死

| 观测 | 值 |
|---|---|
| `entry.exe` PID | 14820 |
| CPU 时间 | **0.125s**（启动即停） |
| 20s 后 CPU | **仍 0.125s** —— 纹丝不动 |
| 判定 | **真阻塞**（非忙等），与交接 §二 症状一致 |

### 3.4 旁证：fact 从未成功产出过

```json
// results/fact.json 与 results/fact.stale.json 均为：
{ "passed": 0, "total": 0, "unverifiedMarkers": 428 /* 或 321 */,
  "metaTotal": 599, "factMethodCount": 258 }
```

**两轮均为全 0。** 独立印证交接 §二 的判断：

> 影响：**fact 阶段完全不可用**，其后 448 个 subject 全跑不到

### 3.5 与 C1 的关系

**无关系。** C1 修的是发射期（`MethodEmission.EmitManagedMethod` 的 `<>c::` 判定），
挂死发生在运行期（`park_until_completed` 的无限等待）。两者独立。

## 四、下一步（交给 box 拆解修复者）

见 `handoff-box-unwrap-breaks-task-handle.md` §五~§七 —— 那部分**依然有效**：

- 发射点：`EmitLinearUnbox`（`LinearEmission.cs:319-335`）
- 推荐修法：Y5 —— 仿 `CtorReturnsNativeHandle`（`5184a0e59`）给
  `GetUninitializedObject` 调用打「返回句柄」标记，`unbox` 时旁路
- 影响面：11 个 `Create<T>` instantiation

**修好 box 拆解后，threading-tasks 的 fact 阶段才能跑通，
C1 的 122-lambda 回归验证才能完成。**

## 五、验证环境备忘（本轮踩坑）

`chunk_pipeline.py` 的 `foundation_root()` 走 marker-walk 找
`config/pipeline-config.yaml`。从 `tests/e2e/verification` 往上**找不到**
`testing/foundation-dll`（两者是**兄弟树**，非祖孙），会静默回退到
`tests/e2e` 并报 `ERROR: Assembly directory not found`。

**必须显式设置**：

```bash
CHAOS_FOUNDATION_DLL=<repo>/testing/foundation-dll \
CHAOS_TESTING_DIR=<repo>/testing \
python -m verification.chunk_pipeline ...
```
