---
name: dev-optimization-campaign
description: 多 family 性能优化战役的编排技能。跨设备 git 异步协调：coordinator 先跑 baseline，worker 在 worktree 中独立优化，每次优化必须产出中文 docs/optimize/ 完整分析文档，commit 必须含性能对比数据。禁止 hack 测试代码，必须直接修复 codegen 或 il2cpp 项目。
---

# 优化战役编排 — 跨设备文档驱动模式

## 核心原则

1. **每次优化必须交付分析文档** — `docs/optimize/YYYY-MM-DD-<标题>/` 是正式交付物，不写文档不算完成
2. **文档语言统一为中文** — 所有分析文档、方案对比、README.md、commit message 均使用中文书写（代码片段、数据表、专业术语缩写除外）
3. **数据驱动决策** — 方案选择必须基于 CoreCLR/Mono/Unity IL2CPP 横向对比，不得凭直觉
4. **收敛验证** — 优化后 vs .NET 8 差距在 20% 以内（`slowdown_vs_net8 <= 20%`），否则必须继续分析优化
5. **Commit 可审计** — 每个优化 round 的 commit message 必须包含性能对比表

## 修复原则

1. **禁止 hack 测试代码** — 发现问题时必须直接修复 codegen 目录或 il2cpp runtime 项目，不得为了通过测试而修改测试用例、跳过断言或降低测试标准
2. **保持测试框架通用性** — 测试代码是通用验证工具，不是某个优化的专属补丁。优化完成后所有已有测试仍应通过
3. **修改测试框架需谨慎** — 如确需修改测试框架/工具链，必须在 README.md 中说明修改动机和影响范围，并经 Coordinator 或架构负责人评审

## 工作流

```
Phase 0 ─ Coordinator: 全量 baseline → 推入 repo
              ↓
Phase 1 ─ Workers（跨设备，git 异步协调）:
  1. git pull → 扫描 docs/optimize/INDEX.md 看哪些 family 已完成
  2. git push claim 一个 family
  3. 在 worktree 中隔离开发
  4. 写 docs/optimize/YYYY-MM-DD-<标题>/README.md
  5. 优化 → build → benchmark
  6. 更新 README.md 中的数据 + 收敛检查
  7. Git add + commit（含性能对比表）+ push
              ↓
Phase 2 ─ Coordinator: 聚合对比报告
```

## 目录契约

```
optimization-campaign/
├── state.json              # 仅 Phase 0 本地使用，不提交 git
├── baseline/
│   ├── array-indexing-copy.json    # 仅 Phase 0 本地使用，不提交 git
│   └── ...
├── workers/
│   ├── agent-01/
│   │   ├── status.md       # Worker 进度日志（不提交）
│   │   ├── blocker.md      # 阻塞项（不提交）
│   │   └── results/        # 过程结果（不提交）
│   └── agent-02/
│       └── ...
└── aggregate/
    └── comparison-report.json  # Coordinator 聚合产物（提交）

docs/optimize/
├── INDEX.md                          # 索引文件（提交，自动维护）
├── 2026-05-27-array-copy-range-opt/  # 每个优化一个独立目录
│   ├── README.md                     # 完整分析文档（正式交付物）
│   └── data/
│       ├── baseline.json             # 优化前数据
│       └── result.json               # 优化后数据
└── 2026-05-28-enum-to-string-opt/
    └── ...
```

### 各目录 git 提交规则

| 路径 | 提交规则 |
|------|---------|
| `docs/optimize/INDEX.md` | 提交 — 项目正式索引 |
| `docs/optimize/YYYY-MM-DD-xxx/` 全部 | 提交 — 正式交付物 |
| `optimization-campaign/aggregate/comparison-report.json` | 提交 — 正式聚合报告 |
| `optimization-campaign/baseline/*.json` | **不提交**（太大，仅本地） |
| `optimization-campaign/workers/*/` | **不提交**（过程产物） |
| `optimization-campaign/state.json` | **不提交**（仅本地协调） |

## Phase 0：Baseline（Coordinator 独占，单机执行）

1. 创建 `optimization-campaign/` 目录
2. 选定 families，写 `state.json`（仅本地）
3. 逐 family 跑 baseline benchmark
4. 评估 family 健康度：
   ```
   healthy:  >=80% methods completed
   partial:  1%-79% methods completed
   broken:   0% methods completed（标为 excluded）
   ```
5. 推进 phase → `baseline_done`，开放 claim

### Baseline benchmark 命令

```bash
for method_index in $(seq 0 $((method_count - 1))); do
    entry.exe --benchmark $method_index 100000
done
```

baseline 结果写入 `optimization-campaign/baseline/<family>.json`。

## Phase 1：并行优化（Worker 独立执行）

### 1. Claim 协议（Git push 原子性）

Worker 通过 git push 原子性 claim family，不依赖共享文件系统：

```bash
# Step 1: 从 main 拉取最新状态
git checkout main
git pull origin main

# Step 2: 创建 claim 工作分支
git checkout -b claim/<family>/<worker-id>

# Step 3: 创建 claim 标记文件
mkdir -p docs/optimize/.claims
echo "claimed-by: <worker-id>" > docs/optimize/.claims/<family>.claim
echo "claimed-at: $(date -u +%Y-%m-%dT%H:%M:%SZ)" >> docs/optimize/.claims/<family>.claim

# Step 4: 提交 claim 并推送
git add docs/optimize/.claims/<family>.claim
git commit -m "claim: <family> by <worker-id>"
git push origin claim/<family>/<worker-id>

# 如果 push 被拒 → 别人已 claim → 删除本地分支，选另一个 family
# 如果 push 成功 → claim 成功，开始优化
```

**释放 claim（放弃或完成后）：**

```bash
# 放弃：删除远程分支
git push origin --delete claim/<family>/<worker-id>

# 完成：通过 PR 合并回 main（含 docs/optimize/ 产物），合并后 claim 自动释放
```

### 2. 隔离开发（推荐用 worktree）

```bash
git worktree add optimization-campaign/workers/<worker-id>/worktree claim/<family>/<worker-id>
cd optimization-campaign/workers/<worker-id>/worktree
```

### 3. 标准优化循环

```
claim success
  → 诊断分析并写 README.md 草案
  → 优化实现
  → build + benchmark
  → 更新 README.md 数据 + 收敛检查
  → commit + push（含性能对比表）
  → 如果未收敛 → 回到"优化实现"继续迭代
```

### 4. 标准诊断流程（6 阶段）

#### Phase A — 测试用例正确性（Fact Validation）

```bash
entry.exe --fact-json
# 检查 JSON 输出：全部 passed==true
# 任何 failed → blocking issue，先修验证再继续
```

#### Phase B — 生成代码正确性（Codegen Audit）

读取 `native-aot.generated.cpp`，对每个 Subject_N 函数分析：
- 桥接调用模式：`kChaosExternalRuntimeFnTable` bridge call vs 内联实现
- 异常处理：`CHAOS_EH_TRY/CATCH` 结构是否正确
- 类型转换：`static_cast` 是否合理
- GC 分配：`CHAOS_IL2CPP_NEW_GC` / `CHAOS_IL2CPP_NEW_GC_ARRAY` 是否必要
- **不必要的 GC 分配**：new 出临时对象后立即使用并丢弃（应改用栈分配或已有实例）

#### Phase C — 多运行时 Benchmark + 内存分配采集

收集全部运行时的 ns/op 和 alloc/op 数据：
- chaos-aot（native AOT）
- chaos-jit（native JIT）
- net8-jit（.NET 8 JIT 基线）
- chaos-hu-aot（hotupdate AOT）
- chaos-hu-jit（hotupdate JIT）

#### Phase D — 与托管基线对比（vs .NET 8/10 JIT）

```
aot_slowdown_vs_net8 = (chaos_aot_ns - net8_ns) / net8_ns * 100
```

分类标记：
- "faster-than-net8": AOT 比 .NET 8 快
- "within-noise": ±50% 以内
- "slower-than-net8": AOT 比 .NET 8 慢 50%+
- "critically-slow": AOT 比 .NET 8 慢 200%+（优先优化）

**分配对比**：如果 chaos-aot 和 net8-jit 都有 allocPerOp 数据：
- `extra_allocs = chaos_aot_alloc_per_op - net8_alloc_per_op`
- extra_allocs > 0 → 标记为"分配瓶颈"，high priority

#### Phase E — HotUpdate 开销分析

```
hu_aot_overhead = (hu_aot_ns - aot_ns) / aot_ns * 100
```
分类：low(<10%) / moderate(10-50%) / high(50-100%) / critical(>100%)

#### Phase F — 综合瓶颈排名

- P0: codegen 正确性问题（fact failed、异常路径未接通）
- P1: vs .NET 8 严重退化（critically-slow）
- P2: hotupdate 开销过高（critical-overhead）
- P3: vs .NET 8 明显退化（slower）
- P4: **过度分配** — native allocPerOp 显著高于 managed
- P5: 绝对值最慢（内部热点排名）

## 核心交付物：docs/optimize/README.md 模板

```markdown
# 优化：<标题>

## 优化对象
- family: `<family-name>`
- 涉及方法: `<method-list>`
- 优化前性能摘要: `XX ns/op`（或 `XX ops/sec`）

## 问题根因分析

<深入分析性能瓶颈的根本原因。
从 codegen 层面、runtime 层面、内存分配层面逐层分析。
附代码片段和调用链分析。>

## 横向方案对比

| 方案 | 实现方式 | 优点 | 缺点 |
|------|---------|------|------|
| **CoreCLR** | <JIT 如何处理此操作> | <...> | <...> |
| **Mono** | <Mono 如何处理此操作> | <...> | <...> |
| **Unity IL2CPP** | <Unity IL2CPP 如何处理此操作> | <...> | <...> |
| **本方案** | <本项目的实现方式> | <...> | <...> |

## 候选方案

### 方案 A：<方案名称>
<描述>

### 方案 B：<方案名称>
<描述>

### 最终选择
<选择的方案名称 + 选择理由>

## 优化后数据

| 方法 | baseline (ns/op) | 优化后 (ns/op) | .NET 8 (ns/op) | vs .NET 8 | 提升幅度 | 备注 |
|------|------------------|----------------|----------------|-----------|---------|------|
| ...  | ...              | ...            | ...            | +X%       | +XX%    |      |

## 收敛检查

- [ ] 全部 fact passed
- [ ] 无退化方法（退化 > 10% 需说明并回退）
- [ ] 优化后 vs .NET 8 差距在 20% 以内（`slowdown_vs_net8 <= 20%`）
- [ ] 或在 README.md 中已注明理论极限并经 Coordinator 确认

## 遗留问题

<如有未解决的优化空间，注明>
```

### 收敛条件定义

收敛标准是与 .NET 8 JIT 对比，**不是与优化前的 baseline 对比**：

```
slowdown_vs_net8 = (optimized_ns - net8_ns) / net8_ns * 100

要求: slowdown_vs_net8 <= 20%
即 优化后比 .NET 8 最多慢 20%
```

若未收敛（`slowdown_vs_net8 > 20%`），Worker **不得** 提交完成，必须：
1. 回到诊断阶段，深入分析根因
2. 寻找替代优化方案
3. 重新优化 → build → benchmark
4. 再次对比 .NET 8 数据
5. 直到收敛或达到理论极限并在 README.md 中注明

**理论极限豁免**：如果经分析确认当前方案已逼近理论极限（如算法本身限制、硬件指令集限制），可在 README.md 中详细论证并注明 `slowdown_vs_net8` 值及不可达标的根因。此豁免需经 Coordinator 评审确认。

## Commit 规范

每个优化 round 的 commit 必须包含性能对比表：

```
<type>: optimize <family> — <简短描述>

## 性能对比

| method | baseline | optimized | .NET 8 | vs .NET 8 | speedup |
|--------|----------|-----------|--------|-----------|---------|
| Foo    | 100ns    | 72ns      | 60ns   | +20%      | 1.39x   |
| Bar    | 250ns    | 210ns     | 180ns  | +16.7%    | 1.19x   |

## 根因

<一句话根因，如 "String.Concat 走 bridge call 产生了不必要的 GC 分配">

## 关键决策

<方案选择的核心理由，如 "方案 B 因为 xxxx 性能更好">
```

### Commit 内容范围

每次 commit 包含且仅包含：
1. `docs/optimize/YYYY-MM-DD-<标题>/` 下该优化的全部文件
2. 优化涉及的实际代码变更（codegen / runtime / test）

**禁止**：将多个不相关优化的变更混入同一个 commit。

## Phase 2：聚合（Coordinator）

优化全部完成后，coordinator 执行：

```bash
# 拉取所有 worker 的结果
git checkout main
git pull origin main

# 运行聚合脚本
python build/toolchains/run/testing/foundation_dll/aggregate_campaign.py \
    --baseline-dir optimization-campaign/baseline/ \
    --docs-dir docs/optimize/ \
    --output optimization-campaign/aggregate/comparison-report.json

# 提交聚合报告
git add optimization-campaign/aggregate/comparison-report.json
git add docs/optimize/INDEX.md  # 自动更新索引
git commit -m "aggregate: optimization campaign comparison report"
git push origin main
```

### INDEX.md 自动维护规则

`docs/optimize/INDEX.md` 由聚合脚本自动生成，格式如下：

```markdown
# 优化记录索引

| 日期 | 标题 | Family | 优化前 | 优化后 | 加速比 | Worker |
|------|------|--------|--------|--------|--------|--------|
| 2026-05-27 | Array.Copy range check | array-indexing-copy | 100ns | 72ns | 1.39x | agent-01 |
| 2026-05-28 | Enum.ToString fast path | enum-parsing | 250ns | 180ns | 1.39x | agent-02 |
```

## Worker 时间预算

| 阶段 | 时间上限 | 说明 |
|------|---------|------|
| 诊断 + README 文档 | 2 小时 | 先分析再动手 |
| 单轮优化（含 build） | 30 分钟 | 心流窗口 |
| Benchmark | 15 分钟 | 自动化 |
| 文档更新 + commit | 15 分钟 | 收口 |
| 整轮（诊断到 commit） | 3 小时 | 超过应暂停并写出 blocker |

**超时不自动释放 claim**（跨设备无 coordinator 实时监控）。Worker 应自己评估时间，主动在 blocker.md 中记录阻塞原因。

## 跨设备协作规则

1. **禁止同时 claim 同一 family** — claim 前必须 git pull 确认 `.claims/` 目录
2. **长耗时优化应分多次 commit** — 不要在本地憋一个大 commit 几天不推送
3. **以 main 为 truth** — 任何数据以 main 分支上的 `docs/optimize/INDEX.md` 为准
4. **Claim 释放** — 优化完通过 PR 合并到 main，合并后 `.claims/<family>.claim` 在 PR 中一并删除

## 验收口径

1. ✅ `docs/optimize/YYYY-MM-DD-<标题>/README.md` 完整（问题分析 + 横向对比 + 方案选择 + 数据）
2. ✅ fact 验证通过（全部 passed）
3. ✅ benchmark 数据完整
4. ✅ 收敛检查通过（vs .NET 8 差距在 20% 以内或注明理论极限并经 Coordinator 确认）
5. ✅ commit message 包含性能对比表
6. ✅ 与 baseline 对比：无退化超过 10% 的方法（如有，已在 README.md 中注明根因）
