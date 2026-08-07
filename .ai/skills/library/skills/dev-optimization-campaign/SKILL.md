---
name: dev-optimization-campaign
description: >
  全自动 family 优化流水线。线性 12 步驱动：Setup → Verify → Fix Loop → Perf Check → 
  Perf Loop → HotUpdate Check → HotUpdate Loop → Commit → Merge → Cleanup。
  每步 bounded retry（max 3），Worktree 强制，Merge 内置。全流程不中断。
---

# 优化战役 — 全自动流水线

## 核心原则

1. **Worktree 强制** — 所有开发必须在 worktree 中完成，无例外
2. **全自动收口** — merge + push + worktree 清理是流程标准终点，不是可选步骤
3. **Bounded retry** — 每个修复循环最多 3 次，超出写 `blocker.md` 并终止
4. **禁止 hack 测试代码** — 必须直接修复 codegen 或 il2cpp runtime
5. **数据完整性是硬要求** — benchmark timing > 0，hotupdate semantic_changed > 0
6. **文档语言统一为中文** — 分析文档、commit message 均用中文（代码片段、数据表、缩写除外）

## 流水线概览

```
Step  1: Setup Worktree → EnterWorktree + claim family
Step  2: Run Pipeline  → python -m verification.entry_points.cli <slug> --mode strict --native-config profile
Step  3: Diagnose      → 对每个 failed stage 输出根因
Step  4: Fix & Rerun   → 修复 + 重跑（--native-config check），最多 3 次。超限写 blocker.md → abort
Step  5: Perf Check    → 从已有报告验证 benchmark timing > 0（不重跑 pipeline）
Step  6: .NET 8 对比    → 跑 pipeline（--native-config profile），vs .NET 8 ≤ 20%？不满足 → 诊断 + 优化 + 重跑，最多 3 次
Step  7: HotUpdate Check → 跑 pipeline（--native-config profile），semantic_changed > 0？overhead ≤ 100%？不满足 → 修复 + 重跑，最多 3 次
Step  8: 文档           → 写 docs/archive/optimize/YYYY-MM-DD-<slug>/README.md
Step  9: Commit         → git add + git commit（含性能表）
Step 10: Push           → git push origin claim/<slug>/<worker-id>
Step 11: Merge → Cleanup → checkout main → merge → push → del worktree branch
Step 12: CI Verify      → 等待 CI 通过（超时 10 min）
Step 13: Pull           → git pull origin main（确保 main 最新且干净）
```

## 自动化执行说明

- 每个 Step 执行完自动进入下一个 Step，不等待用户确认
- Step 4/6/7 的修复循环使用 `max_attempts=3`，超出后写 `blocker.md` 并 abort
- 长时间步骤（pipeline run、benchmark）每 60 秒输出一次 keepalive
- Abort 时输出 `optimization-campaign/workers/<worker-id>/blocker.md`，然后跳到 Step 11 清理，不留在 worktree

## Native config 选择

| 用途 | Config | LOG_LEVEL | ASSERT | PROFILE_SCOPE |
|------|--------|-----------|--------|---------------|
| 诊断/调试（Step 4 修复循环） | `check` | DEBUG | 启用 | 启用 |
| 性能测量（Step 2/6/7 数据采集） | `profile` | INFO | 关 | 启用 |

优化战役中，**数据采集步骤用 `profile`** 获得更真实的性能数据，**修复迭代用 `check`** 获得完整诊断能力。

---

## Step 1: Setup Worktree

### 创建 worktree + claim 分支

调用 `EnterWorktree(name=<worker-id>)` 进入隔离环境。然后在 worktree 内创建 claim 分支并推送：

```bash
# worktree 内创建 claim 分支
git checkout -b claim/<slug>/<worker-id>
git push origin claim/<slug>/<worker-id>
```

### Claim family

```bash
mkdir -p docs/archive/optimize/.claims
echo "claimed-by: <worker-id>" > docs/archive/optimize/.claims/<slug>.claim
echo "claimed-at: $(date -u +%Y-%m-%dT%H:%M:%SZ)" >> docs/archive/optimize/.claims/<slug>.claim
git add docs/archive/optimize/.claims/<slug>.claim
git commit -m "claim: <slug> by <worker-id>"
git push origin claim/<slug>/<worker-id>
```

如果 push 被拒（别人已 claim）→ abort，选另一个 family。

---

## Step 1.5: Pre-verification Test Audit

在跑完整 pipeline 之前，先审计受试 family 的测试代码完整性，确认测试有意义并在需要时自动补充手写测试。

### 1.5.1 运行审计

```bash
python -m verification.stages.pre_verification_audit <slug> --assembly System.Private.CoreLib --json
```

### 1.5.2 解读判决

读取 stdout 的 JSON 输出，根据 `verdict` 字段决定后续动作：

| 判决 | 含义 | 动作 |
|------|------|------|
| `PASS` | 测试代码完整有意义 | → 进入 Step 2 |
| `MISSING_HANDWRITTEN` | 有方法不可 auto-call 且无 handwritten 实现 | → 执行 1.5.3 补充 |
| `STALE_METADATA` | contract.json 的 customEntryIndices 与实际情况不同步 | → 执行 1.5.3 自动修复 |

### 1.5.3 自动修复

```bash
python -m verification.stages.pre_verification_audit <slug> --assembly System.Private.CoreLib --fix
```

`--fix` 会做两件事：
1. 更新 `capability-family-contract.json` 的 `customEntryIndices`（添加缺失索引、移除冗余索引）
2. 在 `handwritten/` 下生成缺失的 `{ClassName}.Custom.cs` 存根文件（含 TODO 注释）

生成后检查 `handwritten/*.Custom.cs` 中的 `TODO` 注释，判断是否需要补充真实实现。然后重新运行审计确认：

```bash
python -m verification.stages.pre_verification_audit <slug> --assembly System.Private.CoreLib --json
```

### 出口条件

- ✅ 审计 verdict = `PASS` → 进入 Step 2
- ⚠️ 需要修复且已修复完毕，重审计 PASS → 进入 Step 2
- ❌ 非 auto-callable 方法过多（>50%）→ 考虑是否跳过此 family 优化

---

## Step 2: Run Pipeline

```bash
python -m verification.entry_points.cli \
    <slug> \
    --assembly System.Private.CoreLib \
    --mode strict \
    --native-config profile \
    --verbose
```

> 首次运行使用 `profile` config 获取真实性能基线。如果 pipeline 因 assertion 失败等诊断问题挂掉，切到 `--native-config check` 重新诊断根因。

**出口条件**：pipeline 执行完毕（无论 pass/fail），`unified-verification-report.json` 已写入。

---

## Step 3: Diagnose

读取 `unified-verification-report.json`（位于 `testing/results/foundation-dll/System.Private.CoreLib/<slug>/`），对每个 status=`failed` 或 status=`error` 的 stage 输出诊断：

| Stage | 诊断要点 |
|-------|---------|
| codegen / jit_codegen | 编译错误 → 读 C++ 错误信息，定位 codegen emitter |
| fact / fact_jit / managed_fact | assert 失败 → 对比 IL 与生成代码语义 |
| audit | 原则审计不通过 → 对齐架构规范 |
| asm_compare | IR 扩展比异常 → 检查 lowering 路径 |
| benchmark | timing = -1.0？→ dispatch thunks ABI 不匹配 |
| hotupdate | semantic_changed = 0？→ dispatch 返回状态码而非返回值 |

**出口条件**：所有 failed stage 的根因已写入 `diagnosis.json`。

---

## Step 4: Fix & Rerun

对每个 failed stage 执行修复：

1. 根据 Step 3 的诊断根因修改代码（codegen / runtime / test）
2. 重新跑 pipeline（同 Step 2，但 **切换为 `--native-config check`** 获得完整 ASSERT 和 DEBUG 日志以便验证修复）
3. 如果修复后仍失败 → `attempt += 1`，回到 Step 3 重新诊断
4. `attempt > max_attempts(3)` → 写 `blocker.md`，跳到 Step 11 Cleanup

**出口条件**：pipeline 全部 passed，或 max_attempts 超限。

---

## Step 5: Perf Check（轻量，不重跑 pipeline）

直接从 Step 4 输出的 `unified-verification-report.json` 中提取 benchmark 数据验证 timing 完整性，无需再跑一次 pipeline：

```bash
bash testing/scripts/check-perf-timings.sh <slug>
```

**出口条件**：
- passed → 进入 Step 6
- failed（timing 不全） → 回到 Step 3 诊断修复

---

## Step 6: .NET 8 对比

```bash
python -m verification.entry_points.cli \
    <slug> \
    --assembly System.Private.CoreLib \
    --mode strict \
    --native-config profile \
    --verbose
```

报告路径：`testing/results/foundation-dll/System.Private.CoreLib/<slug>/unified-verification-report.json`
从该报告中提取 native-aot 和 native-jit 的 ns/op，对比 .NET 8 基线：

```bash
bash testing/scripts/check-net8-slowdown.sh <slug>
```

检查条件：
- `aot_slowdown_vs_net8 <= 20%`
- `jit_slowdown_vs_net8 <= 20%`

**出口条件**：
- passed → 进入 Step 7
- failed → 诊断根因 → 优化实现 → 重跑 pipeline + benchmark，最多 3 次
- 超限 → 写 `blocker.md`，跳到 Step 11

---

## Step 7: HotUpdate Check

```bash
python -m verification.entry_points.cli \
    <slug> \
    --assembly System.Private.CoreLib \
    --mode strict \
    --native-config profile \
    --verbose
```

报告路径：`testing/results/foundation-dll/System.Private.CoreLib/<slug>/unified-verification-report.json`

验证 HotUpdate 数据：

```bash
bash testing/scripts/check-hotupdate.sh <slug>
```

检查条件：
- `semantic_changed_count > 0`（patch 确实改变了语义）
- AOT hotupdate overhead ≤ 100%
- JIT hotupdate overhead ≤ 100%

**出口条件**：
- passed → 进入 Step 8
- failed → 诊断根因 → 修复 → 重跑，最多 3 次
- 超限 → 写 `blocker.md`，跳到 Step 11

---

## Step 8: 文档

创建 `docs/archive/optimize/YYYY-MM-DD-<slug>/README.md`：

```markdown
# 优化：<slug>

## 优化对象
- family: `<slug>`
- 涉及方法: <list>

## 问题根因分析

## 横向方案对比

| 方案 | 实现方式 | 优点 | 缺点 |
|------|---------|------|------|

## 候选方案与选择

## 性能数据

| 方法 | chaos-aot (ns) | chaos-jit (ns) | .NET 8 (ns) | vs .NET 8 | 加速比 |
|------|---------------|---------------|-------------|-----------|--------|

## HotUpdate 开销

| 方法 | 热更前 | 热更后 | 开销 | 路径 |
|------|--------|--------|------|------|

## 收敛检查

- [ ] Step 4: Pipeline 全部 passed
- [ ] Step 5: benchmark timing > 0
- [ ] Step 6: vs .NET 8 ≤ 20%（AOT + JIT）
- [ ] Step 7: hotupdate semantic_changed > 0
- [ ] Step 7: hotupdate overhead ≤ 100%（AOT + JIT）
```

---

## Step 9: Commit

```bash
git add docs/archive/optimize/YYYY-MM-DD-<slug>/
git add <codegen/runtime changes>
git commit -m "$(cat <<'EOF'
perf: optimize <slug>

## 性能对比

| method | baseline | optimized | .NET 8 | vs .NET 8 | speedup |
|--------|----------|-----------|--------|-----------|---------|
| ...    | ...      | ...       | ...    | +X%       | 1.XXx  |

## 根因

<一句话>

## 关键决策

<方案选择理由>

Co-Authored-By: Claude Opus 4.7 <noreply@anthropic.com>
EOF
)"
```

---

## Step 10: Push

```bash
git push origin claim/<slug>/<worker-id>
```

---

## Step 11: Merge → Cleanup

```bash
# 回到 main
git checkout main
git pull origin main

# Merge claim 分支
git merge claim/<slug>/<worker-id> --no-edit

# 如果 merge 失败 → 按文件类型分层解决冲突：
#   - build/工具链文件 + docs/ + 测试数据 JSON → 保留 main 版本
#   - tests/e2e/translation/ 下的合约、entry 文件 → 取 main 版本（claim 分支的属于临时生成）
#   - src/native/ 下的 runtime/codegen 变更 → 取 claim 分支版本
#   - docs/archive/optimize/ 下的优化文档 → 取 claim 分支版本
#   - 其他文件按具体 diff 逐条确认
# 解决方法：git checkout --ours/--theirs <path> + git add

# Push main
git push origin main

# 删除远程 claim 分支 + 本地分支
git push origin --delete claim/<slug>/<worker-id>
git branch -D claim/<slug>/<worker-id>
```

调用 `ExitWorktree(action="remove", discard_changes=true)` 清理 worktree。

**如果前面步骤 abort（blocker.md 被写入）：**
仍执行 Step 11 清理 worktree，不 push claim 分支到 main，只删除远程 claim 分支。

---

## Step 12: CI 验证

```bash
bash testing/scripts/verify-ci.sh --timeout 10 --branch main
```

**出口条件**：
- CI passed → 进入 Step 13
- CI failed → 诊断根因，提交修复 PR（不破坏 main），不在当前 worktree 修复
- 超时 → 手动检查 CI 状态，确认通过后继续

---

## Step 13: Pull

```bash
git pull origin main
git status  # 确认干净
```

**最终状态**：`main` 分支，干净 working tree，已同步远程。

---

## Abort 处理

如果 Step 4/6/7 的修复循环达到 `max_attempts=3`：

1. 写 `blocker.md` 到 `optimization-campaign/workers/<worker-id>/blocker.md`，包含：
   - 哪个 Step 失败
   - 尝试了哪些修复
   - 每次修复的结果
   - 剩余问题
2. 跳到 Step 11 清理 worktree
3. **不合并到 main**，只删除远程 claim 分支

---

## 目录契约

```
optimization-campaign/
└── workers/
    └── <worker-id>/
        ├── blocker.md          # 仅 abort 时产生（不提交）
        └── diagnosis.json      # Step 3 诊断输出（不提交）

docs/archive/optimize/
├── INDEX.md                    # 项目索引（提交）
└── YYYY-MM-DD-<slug>/
    └── README.md               # 分析文档（提交）
```

## 时间预算

| Step | 预估 | 超时处理 |
|------|------|---------|
| 1 Setup | 2 min | — |
| 2 Pipeline | 10-30 min | 每 60s keepalive |
| 3 Diagnose | 3 min | — |
| 4 Fix loop (×3) | 60 min | max_attempts → abort |
| 5 Perf Check | 1 min | — |
| 6 .NET 8 loop (×3) | 60 min | max_attempts → abort |
| 7 HotUpdate loop (×3) | 30 min | max_attempts → abort |
| 8 文档 | 5 min | — |
| 9 Commit | 1 min | — |
| 10 Push | 1 min | — |
| 11 Merge → Cleanup | 3 min | — |
| 12 CI Verify | 10 min | 超时后手动检查 |
| 13 Pull | 1 min | — |
| **总计** | **~3 hours** | |

超限写 blocker.md，不是停下来等人。

---

## 验收口径

1. ✅ Step 1: Worktree 已创建，family 已 claim
2. ✅ Step 2-4: Pipeline 全部 stage passed（或 blocker.md 已记录）
3. ✅ Step 5: 所有 benchmark timing > 0
4. ✅ Step 6: vs .NET 8 ≤ 20%（AOT + JIT，或 blocker.md 已记录）
5. ✅ Step 7: hotupdate semantic_changed > 0 + overhead ≤ 100%（或 blocker.md 已记录）
6. ✅ Step 8-9: docs/archive/optimize/ 完整 + git commit
7. ✅ Step 10-12: main 已合并 + CI 已通过 + worktree 已删除 + main 已 pull
