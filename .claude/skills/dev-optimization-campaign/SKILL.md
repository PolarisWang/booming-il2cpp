---
name: dev-optimization-campaign
description: 多 family 性能优化战役的编排技能。跨设备 git 异步协调：coordinator 先跑 baseline，worker 在 worktree 中独立优化。四阶段流程：① 全量 family verification 修复 → ② 测试代码与生成代码逻辑对齐 → ③ AOT/JIT 性能收敛 → ④ 热更性能保障。每次优化必须产出中文 docs/optimize/ 完整分析文档，commit 必须含性能对比数据。禁止 hack 测试代码，必须直接修复 codegen 或 il2cpp 项目。
---

# 优化战役编排 — 四阶段收敛流程

## 核心原则

1. **每次优化必须交付分析文档** — `docs/optimize/YYYY-MM-DD-<标题>/` 是正式交付物，不写文档不算完成
2. **文档语言统一为中文** — 所有分析文档、方案对比、README.md、commit message 均使用中文书写（代码片段、数据表、专业术语缩写除外）
3. **数据驱动决策** — 方案选择必须基于 CoreCLR/Mono/Unity IL2CPP 横向对比，不得凭直觉
4. **四阶段收敛** — 验证修复 → 逻辑对齐 → 性能收敛 → 热更保障，必须四阶段全部通过才算验收成功
5. **Commit 可审计** — 每个优化 round 的 commit message 必须包含性能对比表

## 修复原则

1. **禁止 hack 测试代码** — 发现问题时必须直接修复 codegen 目录或 il2cpp runtime 项目，不得为了通过测试而修改测试用例、跳过断言或降低测试标准
2. **保持测试框架通用性** — 测试代码是通用验证工具，不是某个优化的专属补丁。优化完成后所有已有测试仍应通过
3. **修改测试框架需谨慎** — 如确需修改测试框架/工具链，必须在 README.md 中说明修改动机和影响范围，并经 Coordinator 或架构负责人评审

## 工作流

```
Phase 1 ─ 验证修复
  git pull → 跑完整 16 stage family verification 管线
  → 修复所有报错 stage → 全部绿
              ↓
Phase 2 ─ 逻辑对齐
  逐方法对比测试代码逻辑 vs 生成代码逻辑
  → 不对齐就修复 codegen → 再跑 verification 确认
              ↓
Phase 3 ─ 性能收敛
  native-aot vs .NET 8 ≤ 20%
  native-jit vs .NET 8 ≤ 20%
  → 不满足就诊断分析 → 优化实现 → build → 再 benchmark → 直到收敛
              ↓
Phase 4 ─ 热更性能保障
  hotupdate 后性能 vs hotupdate 前性能 ≤ 100%（最多慢一倍）
  AOT + JIT 双路径验证
  → 不满足就继续优化 → 直到达标
              ↓
验收成功
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

## Phase 1：验证修复

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

### 3. 跑完整 16-stage family verification 管线

```bash
# 全量 16 个 stage：
#   0  preflight
#   1  codegen (AOT)
#   2  jit_codegen
#   3  managed_fact (.NET8)
#   4  fact (AOT)
#   5  fact_jit (JIT)
#   6  audit
#   7  asm_compare
#   8  microbench
#   9  benchmark (5-way)
#  10  hotupdate (AOT Fact)
#  11  hotupdate_aot_benchmark
#  12  hotupdate_jit_fact
#  13  hotupdate_jit_benchmark
#  14  dashboard
#  15  aggregate

python -m verification.entry_points.cli --slug <family> --assembly System.Private.CoreLib --mode strict
```

### 4. 修复策略

检查每个 stage 的输出，按以下优先级修复：

| 优先级 | 失败 stage | 修复方向 |
|--------|-----------|---------|
| P0 | codegen / jit_codegen | C++ 编译错误 → 修复 codegen 发射逻辑 |
| P1 | fact / fact_jit / managed_fact | Assert 失败 → 检查翻译语义正确性 |
| P2 | audit | 原则审计不通过 → 对齐架构规范 |
| P3 | asm_compare | 指令扩展比异常 → 检查 lowering 路径 |
| P4 | benchmark | 性能数据缺失 → 检查 benchmark harness |
| P5 | hotupdate 系列 | 补丁应用失败 → 修复 hotpatch 机制 |

修复后重新跑完整管线，直到 16 个 stage 全部通过。

## Phase 2：逻辑对齐

### 目标

确认测试代码（C# Subject + Test）与生成的 native C++ 代码逻辑完全对齐。不对齐时必须修复 codegen 或 runtime，不得修改测试代码。

### 检查方法

逐方法对比：

```bash
# 1. 找到测试代码和生成代码
testing/foundation-dll/System.Private.CoreLib/<family>/managed/subjects/<Family>Subjects.cs
testing/foundation-dll/System.Private.CoreLib/<family>/codegen/native-aot.generated.cpp

# 2. 对每个 Subject_N 函数，检查：
```

检查要点：

1. **桥接调用模式** — C# 中调用的方法，在 C++ 中是通过 `kChaosExternalRuntimeFnTable` bridge call 还是内联实现？两者语义是否一致？
2. **异常处理路径** — C# 的 try/catch 是否正确映射到 `CHAOS_EH_TRY/CATCH`？
3. **类型转换** — `static_cast` / `reinterpret_cast` 是否与 IL 语义一致？
4. **GC 分配** — `new T{}` 是否对应 `CHAOS_IL2CPP_NEW_GC`？是否有不必要的堆分配？
5. **返回值** — Subject_N 的 `Run(int)` 返回值是否与 C# 的预期一致？

### 不对齐的处理

发现不对齐 → 根因分析 → 修复 codegen（emitter / planner / Scriban 模板）或 runtime → 重新跑 Phase 1 全量 verification 确认修复正确。

**禁止**：为让验证通过而修改测试代码的预期值或跳过断言。

## Phase 3：性能收敛

### 性能目标

```
native-aot vs .NET 8:  slowdown_vs_net8 <= 20%
native-jit vs .NET 8:  slowdown_vs_net8 <= 20%
```

两者均为**硬要求**，不得放宽。

### 多运行时 Benchmark + 内存分配采集

收集全部运行时的 ns/op 和 alloc/op 数据：
- chaos-aot（native AOT）
- chaos-jit（native JIT）
- net8-jit（.NET 8 JIT 基线）
- chaos-hu-aot（hotupdate AOT）
- chaos-hu-jit（hotupdate JIT）

```bash
# 本机平台：优化主验证
entry.exe --benchmark <method_index> <iterations>    # 自动嵌入平台标识

# 跨平台验证（如涉及平台相关代码）:
# 在其他机器上运行相同命令，结果自动带 platform 标签
```

平台自动识别：所有 benchmark JSON 输出自动包含 `"platform"` 字段（由编译期宏确定），无需手动指定 `--platform` 参数。

### 与托管基线对比

```
aot_slowdown_vs_net8 = (chaos_aot_ns - net8_ns) / net8_ns * 100
jit_slowdown_vs_net8 = (chaos_jit_ns - net8_ns) / net8_ns * 100
```

分类标记：
- "faster-than-net8": 比 .NET 8 快
- "within-noise": ±50% 以内
- "slower-than-net8": 比 .NET 8 慢 50%+
- "critically-slow": 比 .NET 8 慢 200%+（优先优化）

**分配对比**：如果 chaos-aot/jit 和 net8-jit 都有 allocPerOp 数据：
- `extra_allocs = chaos_alloc_per_op - net8_alloc_per_op`
- extra_allocs > 0 → 标记为"分配瓶颈"，high priority

### 诊断流程（6 阶段）

#### Phase A — Fact Validation
```bash
entry.exe --fact-json
# 全部 passed==true
```

#### Phase B — Codegen Audit
读取 `native-aot.generated.cpp`，逐 Subject_N 函数分析：
- 桥接调用模式
- 异常处理结构
- 不必要的 GC 分配
- 寄存器/栈使用效率

#### Phase C — 多运行时 Benchmark
收集 5 个运行时的 ns/op + alloc/op

#### Phase D — vs .NET 8 对比
计算 aot/jit 的 slowdown_vs_net8，分类标记

#### Phase E — HotUpdate 开销分析
```
hu_aot_overhead = (hu_aot_ns - aot_ns) / aot_ns * 100
```
分类：low(<10%) / moderate(10-50%) / high(50-100%) / critical(>100%)

#### Phase F — 综合瓶颈排名
- P0: codegen 正确性问题
- P1: vs .NET 8 严重退化（critically-slow）
- P2: hotupdate 开销过高
- P3: vs .NET 8 明显退化
- P4: 过度分配
- P5: 绝对值最慢

### 跨平台收敛

优化必须在 worker 的本机平台上完成收敛验证。如果优化涉及平台相关代码，应额外在异平台验证。

## Phase 4：热更性能保障

### 目标

热更新（hotpatch）应用后的方法性能，不得比热更前慢超过 100%。

### 测量方法

```bash
# Phase 4a: HotUpdate AOT 性能
entry.exe --hotupdate-and-benchmark <method_index> <iterations>

# Phase 4b: HotUpdate JIT 性能
entry-jit.exe --hotupdate-and-benchmark <method_index> <iterations>
```

### 计算方式

```
AOT:  hu_aot_overhead = (hu_aot_ns - aot_ns) / aot_ns * 100
JIT:  hu_jit_overhead = (hu_jit_ns - jit_ns) / jit_ns * 100

要求: hu_aot_overhead <= 100% 且 hu_jit_overhead <= 100%
即    hotupdate 后最多比 hotupdate 前慢一倍
```

AOT + JIT 双路径都必须满足。

### 不满足的处理

如果 `hu_overhead > 100%`，必须：
1. 分析 hotpatch 机制的开销来源（dispatch 间接跳转、thunk 开销、patch 上下文切换）
2. 优化 hotpatch dispatch 路径
3. 重新 build → benchmark
4. 直到达标

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

## 各阶段交付

### Phase 1: 验证修复
<列出 16 stage 运行结果，全部 passed>

### Phase 2: 逻辑对齐
<列出的测试代码与生成代码对齐检查结果>

### Phase 3: 性能收敛

| 方法 | 平台 | baseline (ns/op) | 优化后 (ns/op) | .NET 8 (ns/op) | vs .NET 8 | 提升幅度 | 备注 |
|------|------|------------------|----------------|----------------|-----------|---------|------|
| ...  | windows-x64 | ... | ... | ... | +X% | +XX% | |

**native-jit vs .NET 8:**

| 方法 | 平台 | baseline (ns/op) | 优化后 (ns/op) | .NET 8 (ns/op) | vs .NET 8 | 提升幅度 |
|------|------|------------------|----------------|----------------|-----------|---------|
| ...  | windows-x64 | ... | ... | ... | +X% | +XX% |

### Phase 4: 热更性能保障

| 方法 | 平台 | 热更前 (ns/op) | 热更后 (ns/op) | 开销 | 路径 |
|------|------|----------------|----------------|------|------|
| ...  | windows-x64 | ... | ... | +X% | AOT |
| ...  | windows-x64 | ... | ... | +X% | JIT |

## 收敛检查

- [ ] Phase 1: 16 stage 全部 passed
- [ ] Phase 2: 测试代码与生成代码逻辑对齐
- [ ] Phase 3: native-aot vs .NET 8 ≤ 20%
- [ ] Phase 3: native-jit vs .NET 8 ≤ 20%
- [ ] Phase 3: 跨平台验证通过（涉及平台相关代码时）
- [ ] Phase 4: hotupdate AOT 开销 ≤ 100%
- [ ] Phase 4: hotupdate JIT 开销 ≤ 100%
- [ ] 或在 README.md 中已注明理论极限并经 Coordinator 确认

## 遗留问题

<如有未解决的优化空间，注明>
```

### 收敛条件定义

#### Phase 3: 性能收敛

收敛标准是与 .NET 8 JIT 对比，**不是与优化前的 baseline 对比**：

```
aot_slowdown_vs_net8 = (chaos_aot_ns - net8_ns) / net8_ns * 100
jit_slowdown_vs_net8 = (chaos_jit_ns - net8_ns) / net8_ns * 100

要求: aot_slowdown_vs_net8 <= 20%  且  jit_slowdown_vs_net8 <= 20%
即 优化后比 .NET 8 最多慢 20%
```

**硬要求**，任何一条不满足都算未收敛。

**跨平台收敛**：优化必须在 worker 的本机平台上完成收敛验证。如果优化涉及平台相关代码（汇编指令、ABI 约定、内存布局等），worker 应在本机之外至少一个异平台（如 Linux/macOS）上验证收敛结果。

#### Phase 4: 热更性能保障

```
AOT:  hu_aot_overhead = (hu_aot_ns - aot_ns) / aot_ns * 100
JIT:  hu_jit_overhead = (hu_jit_ns - jit_ns) / jit_ns * 100

要求: hu_aot_overhead <= 100%  且  hu_jit_overhead <= 100%
即  hotupdate 后最多比 hotupdate 前慢一倍
```

AOT + JIT 双路径都必须满足。

#### 未收敛处理

Phase 3 或 Phase 4 未收敛时，Worker **不得**提交完成：
1. 回到诊断阶段，深入分析根因
2. 寻找替代优化方案
3. 重新优化 → build → benchmark
4. 再次对比目标数据
5. 直到收敛或达到理论极限并在 README.md 中注明

**理论极限豁免**：如果经分析确认当前方案已逼近理论极限，可在 README.md 中详细论证并注明未达标值及不可达标的根因。此豁免需经 Coordinator 评审确认。

## Commit 规范

每个优化 round 的 commit 必须包含各阶段性能对比表：

```
<type>: optimize <family> — <简短描述>

## Phase 3: 性能对比

### native-aot vs .NET 8

| method | platform | baseline | optimized | .NET 8 | vs .NET 8 | speedup |
|--------|----------|----------|-----------|--------|-----------|---------|
| Foo    | windows-x64 | 100ns | 72ns | 60ns | +20% | 1.39x |
| Bar    | windows-x64 | 250ns | 210ns | 180ns | +16.7% | 1.19x |

### native-jit vs .NET 8

| method | platform | baseline | optimized | .NET 8 | vs .NET 8 | speedup |
|--------|----------|----------|-----------|--------|-----------|---------|
| Foo    | windows-x64 | 120ns | 90ns | 60ns | +50% | 1.33x |

### Phase 4: 热更开销

| method | platform | 热更前 | 热更后 | 开销 | 路径 |
|--------|----------|--------|--------|------|------|
| Foo    | windows-x64 | 72ns | 95ns | +32% | AOT |
| Foo    | windows-x64 | 90ns | 120ns | +33% | JIT |

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

1. ✅ `docs/optimize/YYYY-MM-DD-<标题>/README.md` 完整（问题分析 + 横向对比 + 方案选择 + 四阶段数据）
2. ✅ Phase 1: 16 stage 全部 passed
3. ✅ Phase 2: 测试代码与生成代码逻辑对齐
4. ✅ Phase 3: native-aot vs .NET 8 差距 ≤ 20%
5. ✅ Phase 3: native-jit vs .NET 8 差距 ≤ 20%
6. ✅ Phase 4: hotupdate AOT 开销 ≤ 100%
7. ✅ Phase 4: hotupdate JIT 开销 ≤ 100%
8. ✅ commit message 包含四阶段性能对比表
9. ✅ 跨平台验证通过（涉及平台相关代码时）
10. ✅ 无退化超过 10% 的方法（如有，已在 README.md 中注明根因）
