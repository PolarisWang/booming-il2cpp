---
name: dev-optimization-campaign
description: >
  三阶段 family 优化流水线：Phase 0 数据驱动选择（扫描→排序→ROI 预估）→
  Phase 1 预备阶段（审计+修复→再冻结→Baseline）→
  Phase 2 优化阶段（Claim→Pipeline→三明治优化循环→回归验证→收尾）。
  Worktree 强制，Merge 内置，bounded retry（max 3），三明治模型自动回滚。
---

# 优化战役 — 三阶段流水线

## 核心原则

1. **Worktree 强制** — 所有 Phase 2 开发必须在 worktree 中完成，无例外
2. **全自动收口** — merge + push + worktree 清理是流程标准终点，不是可选步骤
3. **分类修复** — 失败按类型分流：Infrastructure → 1 次重建，Logic → 3 次迭代，Design → 调研后再进入修复。不混用 retry budget
4. **禁止 hack 测试代码** — 必须直接修复 codegen 或 il2cpp runtime。Phase 1 修复测试代码，Phase 1.3 冻结后**禁止修改** subject 文件
5. **数据完整性是硬要求** — benchmark timing > 0，exception-path 方法自动排除，hotupdate d3PatchApplied 必须先为 true 再检查 semantic_changed > 0
6. **冻结时序：先修测试，再冻结** — 与旧版关键区别：Phase 1.1~1.2 先完成审计和修复，Phase 1.3 才执行 freeze+baseline。冻结后优化循环中不得修改 subject 文件
7. **优化循环自动回滚** — 每次修改必须验证性能提升，不达标自动回滚
8. **文档语言统一为中文** — 分析文档、commit message 均用中文（代码片段、数据表、缩写除外）

---

# 三阶段架构总览

```
Phase 0: 数据驱动选择（不 claim，不修改代码）
  Step 0.1 → 全量 Benchmark 扫描
  Step 0.2 → 优先级队列生成
  Step 0.3 → 候选瓶颈分析 + ROI 预估
  出口: 选定优化目标 + ROI 通过

Phase 1: 预备阶段（不 claim，不 commit）
  Step 1.1 → Subject 质量审计 + 自动修复 broken tests
  Step 1.2 → 结构性审计（contract sync）
  Step 1.3 → Freeze + Baseline 捕获（修复后冻结）
  出口: 审计 PASS + freeze 完成 + baseline 已记录

Phase 2: 优化阶段（claim + commit）
  Step 2.1 → Setup Worktree + Claim
  Step 2.2 → Pre-flight Check
  Step 2.3 → Pipeline（首次 Baseline 测量）
  Step 2.4 → 瓶颈验证（profile scope 数据确认瓶颈）
  Step 2.5 → 优化循环（三明治模型：假设→实现→验证→回滚，max_attempts=3）
  Step 2.6 → 回归验证（semantic + benchmark）
  Step 2.7 → .NET 8 对比
  Step 2.8 → HotUpdate 检查
  Step 2.9 → 文档 + Commit + Merge + CI + Pull
  出口: pipeline passed + regression passed + perf improved + commit merged
```

## 自动化执行说明

- 每个 Step 执行完自动进入下一个 Step，不等待用户确认
- 长时间步骤（pipeline run、benchmark）每 60 秒输出一次 keepalive
- Abort 时输出 `optimization-campaign/workers/<worker-id>/blocker.md`，然后跳到 Step 2.9 清理，不留在 worktree

## Native config 选择

| 用途 | Config | LOG_LEVEL | ASSERT | PROFILE_SCOPE |
|------|--------|-----------|--------|---------------|
| 诊断/调试（Phase 2 修复循环） | `check` | DEBUG | 启用 | 启用 |
| 性能测量（Phase 0 扫描、Phase 2 数据采集） | `profile` | INFO | 关 | 启用 |

优化战役中，**数据采集步骤用 `profile`** 获得更真实的性能数据，**修复迭代用 `check`** 获得完整诊断能力。

---

# Phase 0: 数据驱动选择

**性质**: 只读分析，不 claim，不修改任何代码
**执行位置**: main 分支，不需要 worktree
**目标**: 从全量 benchmark 数据中自动识别 ROI 最高的优化候选

## Step 0.1: 全量 Benchmark 扫描

扫描所有已有 family 的 benchmark 数据，收集 slowdown 信息：

```bash
python -m verification.analysis.benchmark_scanner \
    --assembly System.Private.CoreLib \
    --native-config profile \
    --output benchmark-scan-report.json
```

输出内容：
- 每个 family 的 chaos-aot ns/op（已有数据）
- 每个 family 的 slowdown vs .NET 8（已有数据）
- 每个 family 的 method 数量
- 数据置信度（是否有完整 pipeline 数据）

**出口条件**: `benchmark-scan-report.json` 已生成，包含所有候选 family 的基础数据。

## Step 0.2: 优先级队列生成

按综合优先级公式排序，生成优化候选队列：

```bash
python -m verification.analysis.benchmark_scanner \
    --assembly System.Private.CoreLib \
    --priority-sort \
    --output optimization-priority.json
```

### 优先级排序公式

```
priority_score = (chaos_aot_ns / net8_ns) × method_count × data_confidence
```

即：slowdown 越大 × 方法越多 × 数据越可信 → 优先级越高。

### 过滤条件

自动排除以下 family：
- 已有 claim 标记（`docs/optimize/.claims/` 中存在 `.claim` 文件）
- 数据置信度过低（benchmark timing 全为 0.0 或无数据）
- exception-path 占比 > 50%（表明该 family 主要测试异常路径，优化收益有限）

**出口条件**: `optimization-priority.json` 已生成，队列按 priority_score 降序排列。

## Step 0.3: 候选瓶颈分析 + ROI 预估

对 Top-N 候选（默认 Top 3）执行 profile scope 分析，确认瓶颈位置并预估优化收益：

```bash
# 对每个候选 family 执行 profile 分析
python -m verification.analysis.profile_analysis <slug> \
    --assembly System.Private.CoreLib \
    --native-config profile \
    --output bottleneck-report.md
```

### ROI 预估维度

| 维度 | 评估方法 | 权重 |
|------|---------|------|
| 预期加速比 | profile scope 数据显示的热点函数耗时占比 | 50% |
| 实现复杂度 | codegen 修改范围估计（单 emitter / 多 IR / 跨模块） | 30% |
| 影响范围 | 该优化是否惠及其他 family | 20% |

### 决策门

- **通过**: 预期收益 > 20%（chaos-aot ns 降低 20% 以上）→ 进入 Phase 1
- **待定**: 预期收益 10-20% → 标记为"后备"，进入下一个候选
- **拒绝**: 预期收益 < 10% → 跳过该 family，进入下一个候选

### 输出

- `bottleneck-report.md`: 每个候选的瓶颈分析和 ROI 预估
- `optimization-priority.json`: 更新后的优先级队列（含 ROI 标记）

### 出口条件

- [x] 选定优化目标 family（slug 确定）
- [x] ROI 预估通过（预期收益 > 20%）
- [x] `bottleneck-report.md` 记录了瓶颈位置
- [x] 输出文件已保存到 `docs/optimize/YYYY-MM-DD-<slug>/`（分析阶段目录）

---

# Phase 1: 预备阶段

**性质**: 本地审计和修复，不 claim family，不 commit，不需要 worktree
**执行位置**: main 分支
**核心原则**: **先修测试，再冻结** — Phase 1.1~1.2 完成审计和修复后，Phase 1.3 才执行 freeze

## Step 1.1: Subject 质量审计 + 自动修复

在跑完整 pipeline 之前，先审计受试 family 的测试代码正确性和完整性。

### 正确性审计（两阶段检查）

```bash
python -m verification.stages.subject_correctness_audit <slug> --assembly System.Private.CoreLib
```

检查内容：

| 检查项 | 检测目标 | 示例发现 | 严重级别 |
|--------|---------|---------|---------|
| Assertion Quality | CustomEntrySubject_N() 是否有 Assert.* 调用 | reflection-member-complete: 17 方法 0 Assert | WARN |
| Stub Detection | `_ = 0;` / `_ = default(T)!.Member` | reflection-member-complete: 2 个 `_ = 0;`, 15 个 `_ = default` | WARN |
| Triviality Detection | Subject_N() 是否总走 catch 路径 | — | WARN |
| Benchmark Input Check | benchmark 标注的方法是否始终抛异常 | — | WARN |

### 自动修复 broken tests

对审计发现的问题执行自动修复：

```bash
# 自动修复 stub 和 assertion 缺失
python -m verification.stages.subject_correctness_audit <slug> --assembly System.Private.CoreLib --fix
```

`--fix` 会：
1. 在 `handwritten/` 下生成或修补 `{ClassName}.Custom.cs` 文件
2. 自动填充 missing assertion 调用

### 人工介入条件

自动修复无法处理以下情况（需要人工判断）：
- Triviality Detection 发现 Subject_N() 始终走 catch 路径 → 需人工确认测试意图
- Benchmark Input Check 发现 benchmark 标注的方法始终抛异常 → 需人工确认是否为 exception-path

### 出口条件

- [x] 正确性审计 verdict = `PASS` 或所有 WARN 已修复
- [x] 自动修复已执行完毕
- [x] 需人工介入的问题已标记（不阻塞 Phase 1 进入下一阶段，但需在 Phase 2 前处理）

## Step 1.2: 结构性审计（contract sync）

检查 capability-family-contract 与实际 subject 代码的一致性：

```bash
python -m verification.stages.pre_verification_audit <slug> --assembly System.Private.CoreLib --json
```

检查内容：
- `customEntryIndices` 与实际情况是否同步
- 非 auto-callable 方法是否有 handwritten 实现
- 标注完整性（benchmarkMethodIndices、hotupdateMethodIndices）

### 自动修复合同不同步

```bash
python -m verification.stages.pre_verification_audit <slug> --assembly System.Private.CoreLib --fix
```

`--fix` 会：
1. 更新 `capability-family-contract.json` 的 `customEntryIndices`（添加缺失索引、移除冗余索引）
2. 在 `handwritten/` 下生成缺失的 `{ClassName}.Custom.cs` 存根文件（含 TODO 注释）

### 审计判决

读取 stdout 的 JSON 输出，根据 `verdict` 字段决定后续动作：

| 判决 | 含义 | 动作 |
|------|------|------|
| `PASS` | 测试代码完整有意义 | → 进入 Step 1.3 |
| `MISSING_HANDWRITTEN` | 有方法不可 auto-call 且无 handwritten 实现 | → 执行自动修复 |
| `STALE_METADATA` | contract.json 的 customEntryIndices 与实际情况不同步 | → 执行自动修复 |

生成后重新运行审计确认：

```bash
python -m verification.stages.pre_verification_audit <slug> --assembly System.Private.CoreLib --json
```

### 出口条件

- [x] 结构审计 verdict = `PASS` → 进入 Step 1.3
- [x] 已修复完毕，重审计 PASS → 进入 Step 1.3
- [x] 非 auto-callable 方法过多（>50%）→ 考虑是否跳过此 family 优化

## Step 1.3: Subject Freeze + Baseline Capture

**与旧版关键区别**: 先修测试（Step 1.1~1.2），再冻结。冻结后优化循环中禁止修改 subject 文件。

### 1.3.1 冻结 Subject

```bash
python -m verification.stages.pre_verification_audit <slug> --assembly System.Private.CoreLib --freeze
```

此命令：
1. 计算 `managed/subjects/` 和 `handwritten/` 下所有 .cs 文件的 SHA256 哈希
2. 写入 `subject-freeze-manifest.json`
3. 备份冻结副本到 `subject-freeze/` 目录

**冻结期间禁止修改** `managed/subjects/` 和 `handwritten/` 下的任何文件。
Phase 2 每次重跑 pipeline 前自动验证冻结：

```bash
python -m verification.stages.pre_verification_audit <slug> --assembly System.Private.CoreLib --verify-freeze
```

如果冻结被打破 → 恢复 subject 文件，修改必须来自 codegen/runtime 而非 test。

### 1.3.2 捕获优化前基线

```bash
# 1. 跑 managed_record 捕获 golden record
python -m verification.entry_points.cli \
    <slug> --assembly System.Private.CoreLib --mode strict \
    --stages managed_record --verbose

# 2. 保存基线 golden record
cp native/golden-record.json native/baseline-golden-record.json
```

首次完整 pipeline（Phase 2 Step 2.3）跑完后，性能基线会自动保存在 `perf/benchmark-history.jsonl` 中。

### 1.3.3 记录基线 commit

```bash
git rev-parse HEAD > native/baseline-commit.txt
echo "Baseline captured at commit $(cat native/baseline-commit.txt)"
```

### 出口条件

- [x] subject-freeze-manifest.json 已生成
- [x] baseline-golden-record.json 已保存
- [x] baseline-commit.txt 已记录
- [x] Phase 1 审计全部 PASS（测试质量 + 结构同步）

---

# Phase 2: 优化阶段

**性质**: 修改代码、测量、优化、收尾
**执行位置**: worktree（强制）
**入口条件**: Phase 1 全部出口条件满足

## Step 2.1: Setup Worktree + Claim

### 创建 worktree + claim 分支

调用 `EnterWorktree(name=<worker-id>)` 进入隔离环境。然后在 worktree 内创建 claim 分支并推送：

```bash
# worktree 内创建 claim 分支
git checkout -b claim/<slug>/<worker-id>
git push origin claim/<slug>/<worker-id>
```

### Claim family

```bash
mkdir -p docs/optimize/.claims
echo "claimed-by: <worker-id>" > docs/optimize/.claims/<slug>.claim
echo "claimed-at: $(date -u +%Y-%m-%dT%H:%M:%SZ)" >> docs/optimize/.claims/<slug>.claim
git add docs/optimize/.claims/<slug>.claim
git commit -m "claim: <slug> by <worker-id>"
git push origin claim/<slug>/<worker-id>
```

如果 push 被拒（别人已 claim）→ abort，选另一个 family（回到 Phase 0 Step 0.2 选择下一个候选）。

### 出口条件

- [x] worktree 创建成功
- [x] claim 分支已推送远程
- [x] `.claim` 文件已提交

## Step 2.2: Pre-flight Check

在跑完整 pipeline 之前，先验证构建系统和源文件基础设施的完整性。**约 60%+ 的 pipeline 假性失败源于构建系统/缓存/文件缺失问题，预检可以在 30-60 秒内过滤掉。**

### 2.2.1 源文件完整性验证

```bash
# 检查 native/ 目录下所有关键源文件存在
_check_file "${native_dir}/runtime-entry.cpp" "runtime-entry.cpp" || fail=1
_check_file "${native_dir}/verification_dispatch.generated.cpp" "verification_dispatch.generated.cpp" || fail=1
_check_file "${native_dir}/microbench.cpp" "microbench.cpp" || fail=1
_check_file "${native_dir}/CMakeLists.txt" "CMakeLists.txt" || fail=1
```

如果缺失：
- `runtime-entry.cpp` / `verification_dispatch.generated.cpp` → 需要重新跑 codegen 或从其他 family 复制模板
- `microbench.cpp` → 自动生成 fallback 版本

### 2.2.2 构建系统验证

```bash
build_dir="${native_dir}/build/vs2022"
if [ -d "$build_dir" ] && [ -f "$build_dir/CMakeCache.txt" ]; then
    # CMakeCache 存在 → 验证关键源文件在 vcxproj 中
    grep -q "runtime-patchdata.cpp" "$build_dir/entry.vcxproj" 2>/dev/null || {
        echo "WARN: runtime-patchdata.cpp not in vcxproj — cmake reconfigure needed"
        cmake -S "$native_dir" -B "$build_dir" -DCMAKE_BUILD_TYPE=RelWithDebInfo
    }
else
    # 首次配置
    cmake -S "$native_dir" -B "$build_dir" -DCMAKE_BUILD_TYPE=RelWithDebInfo
fi
```

### 2.2.3 二进制合约验证

```bash
cmake --build "$build_dir" --config RelWithDebInfo --target entry --parallel
_entry_binary=$(_find_entry_binary "$build_dir")

# 验证 kSubjectEntryCount > 0
_symbol_count=$(strings "$_entry_binary" | grep -c "kSubjectEntryCount" || echo 0)
if [ "$_symbol_count" -eq 0 ]; then
    echo "FAIL: kSubjectEntryCount not found in entry binary"
    exit 1
fi
```

### 2.2.4 自动修复

如果预检发现问题：
- **源文件缺失** → 从 codegen 输出拷贝或生成模板
- **CMakeCache 过期** → 删除后重新 configure
- **符号不正确** → 确认 codegen 输出 → 确认 cmake 源文件列表 → 重新构建

修复后重新执行 2.2.3 验证。如果仍失败 → 记录为 **Infrastructure 类失败**。

### 出口条件

- [x] 全部验证通过 → 进入 Step 2.3
- [x] 自动修复后验证通过 → 进入 Step 2.3
- [x] 自动修复仍失败 → 记录 infra 诊断信息，进入 Step 2.3（不阻塞，pipeline 会暴露根因）

## Step 2.3: Pipeline（首次 Baseline 测量）

跑完整 pipeline，使用 `profile` config 获取真实性能基线：

```bash
python -m verification.entry_points.cli \
    <slug> \
    --assembly System.Private.CoreLib \
    --mode strict \
    --native-config profile \
    --verbose
```

> 如果 pipeline 因 assertion 失败等诊断问题挂掉，切到 `--native-config check` 重新诊断根因。

**出口条件**：pipeline 执行完毕（无论 pass/fail），`unified-verification-report.json` 已写入。

### 根因诊断（如果 pipeline failed）

读取 `unified-verification-report.json`，对每个 status=`failed` 或 status=`error` 的 stage 执行分类诊断：

#### 失败类型分类

| 类别 | 典型症状 | Retry budget |
|------|---------|-------------|
| **Infrastructure** | 编译错、链接错、file not found | 1 次（重建/重配后 100% 恢复） |
| **Logic** | assert 失败、benchmark timing=-1 | 3 次（优化循环中迭代） |
| **Design** | 同一 Logic 失败 3 次仍未解决 | 不 retry，调研后再修复 |

分类方法：
1. **编译/链接错误？** → Infrastructure
2. **file not found / 目录不存在？** → Infrastructure
3. **fact assert 失败？** → Logic
4. **benchmark timing = -1.0？** → Logic

如果 Step 2.3 失败且为 Infrastructure 类 → 执行 1 次修复后重跑，然后进入 Step 2.4。
如果 Step 2.3 失败且为 Logic 类 → 在 Step 2.5 优化循环中一并修复。

## Step 2.4: 瓶颈验证

使用 profile scope 数据确认性能瓶颈位置，与 Phase 0.3 的 ROI 预估对比：

```bash
# 从 Step 2.3 输出的 profile 数据中提取瓶颈分析
python -m verification.analysis.profile_analysis <slug> \
    --assembly System.Private.CoreLib \
    --from-report testing/foundation-dll/System.Private.CoreLib/<slug>/unified-verification-report.json \
    --output bottleneck-verified.md
```

### 验证内容

| 项目 | Phase 0.3 预估 | 实际数据 | 匹配？ |
|------|---------------|---------|--------|
| 最热函数/IR | ... | ... | ✅/❌ |
| 耗时占比 | ... | ... | ✅/❌ |
| 预期收益 | ... | ... | ✅/❌ |

### 偏差处理

如果实际瓶颈与预估偏差 > 30%：
1. 更新 `bottleneck-report.md` 记录偏差
2. 重新计算 ROI 预估
3. 如果重新计算后预期收益 < 10% → 回到 Phase 0 选择下一个候选

### 出口条件

- [x] 瓶颈位置已确认与预估一致（或偏差在 30% 以内）
- [x] `bottleneck-verified.md` 已写入
- [x] ROI 重新确认通过（预期收益 > 20%）

## Step 2.5: 优化循环（三明治模型）

核心修复迭代步骤。每次循环是一个完整的三明治回合。

### 三明治模型结构

```
┌─────────────────────────────────────────────────┐
│  提出假设: "改变 X 会使 Y 提升 Z%"              │
│         ↓                                       │
│  实现修改: 修改 codegen/runtime 代码             │
│         ↓                                       │
│  验证: 跑 pipeline + benchmark                   │
│         ↓                                       │
│  Z' < Z/2 ?                                     │
│   ├─ Yes → 自动回滚 + 记录失败                   │
│   └─ No  → 确认提升 + 进入下一轮                 │
│         ↓                                       │
│  attempt += 1                                    │
│  attempt > max_attempts(3) ?                     │
│   ├─ Yes → Design 升级 → abort                   │
│   └─ No  → 下一轮（新假设）                      │
└─────────────────────────────────────────────────┘
```

### 2.5.1 提出假设

每次循环开始前，必须明确写出假设声明：

```markdown
## 假设
- 修改对象: <具体的文件/函数/IR 节点>
- 预期效果: <具体的性能指标变化>
- 预期提升: Z%
- 理论依据: <为什么这个修改会提升性能>
```

假设必须有量化预期（Z%），禁止模糊表述如"提升性能"。

### 2.5.2 实现修改

修改 codegen 或 runtime 代码。**禁止修改已冻结的 subject 文件。**

修改前验证 subject freeze：
```bash
python -m verification.stages.pre_verification_audit \
    <slug> --assembly System.Private.CoreLib --verify-freeze
```

如果 freeze 被打破 → 恢复 subject 文件，确认修改来自 codegen/runtime 而非 test。

### 2.5.3 验证

使用 `check` config 跑 pipeline 验证正确性 + 基准性能：

```bash
python -m verification.entry_points.cli \
    <slug> \
    --assembly System.Private.CoreLib \
    --mode strict \
    --native-config check \
    --verbose
```

从 report 中提取优化后的 benchmark 数据：

```bash
# 提取实际提升 Z'%
python -m verification.analysis.extract_timing <slug> \
    --assembly System.Private.CoreLib \
    --output attempt-<N>-timing.json
```

### 2.5.4 回滚决策

对比实际提升 Z'% 与预期提升 Z%：

| 条件 | 动作 |
|------|------|
| Z' >= Z/2 | 确认提升 → 保留修改 → 进入下一假设（如需） |
| Z' < Z/2 | **自动回滚** → 恢复修改前状态 → 记录失败原因 + enter next attempt |
| Z' <= 0（退化） | **自动回滚** → 恢复修改前状态 → 记录失败原因 |
| Pipeline failed（Logic 类） | attempt 不消耗 → 修复 Logic 问题后继续 |

#### 自动回滚操作

```bash
# 通过 git 回滚到修改前状态
git checkout -- <modified-files>
# 或使用 git restore
git restore <modified-files>
```

回滚后记录到 `attempt-<N>-rollback.md`：
- 修改了什么
- 预期提升 Z%
- 实际提升 Z'%
- 失败根因分析

### 2.5.5 循环控制

- `max_attempts = 3`（每次循环算一次 attempt，无论回滚与否）
- 如果全部 3 次 attempt 均回滚 → 升级为 **Design 类** → 启动架构调研
- 如果有 attempt 成功（Z' >= Z/2）→ 可继续下一假设（不消耗额外 attempt 配额，但总修改次数仍然有限）
- **注意**：Infrastructure 类失败不消耗 attempt 配额（infra 失败本质不是优化假设的验证）

### 出口条件

- [x] 至少一次优化假设验证通过（Z' >= Z/2）
- [x] 确认正确性无退化（pipeline all passed 或已知可接受差异）
- [x] 修改已保留在 worktree（未回滚）
- [x] 回滚记录已保存（如有回滚发生）
- [x] Design 升级时 blocker.md 已写入 → 跳到 Step 2.9 清理

## Step 2.6: 回归验证（semantic + benchmark）

直接从 Step 2.5 输出的 `unified-verification-report.json` 和 `multi-run/multi-run-report.json` 中提取数据验证。

### 2.6.1 Timing 完整性 + Exception-path 检测

```bash
bash testing/scripts/check-perf-timings.sh <slug> --verbose
```

检查项：
- 所有 benchmark timing > 0
- Exception-path 检测：识别因 null/invalid 输入导致始终抛异常的 method（chaos-aot timing 比 .NET 8 慢 20x+），输出警告并排除
- 数据一致性：okCount == totalMethods

### 2.6.2 语义回归检查

对比优化后的 golden record 与基线，确认语义未被破坏：

```bash
python -m verification.analysis.semantic_regression_check <slug> --assembly System.Private.CoreLib
```

检查项：
- 每个 method 的 pass/fail 状态与基线一致
- 发生 exception 的 method 的 exception type 与基线一致

### 2.6.3 性能回归检查

对比优化后的 benchmark 数据与基线，确认性能未退化：

```bash
python -m verification.analysis.perf_regression_check <slug> --assembly System.Private.CoreLib --max-degradation 1.05
```

检查项：
- 每个 method 的 ns/op 与基线对比
- 超过 5% degradation 的方法标记为回归

### 出口条件

- [x] 2.6.1 passed → 记录 exception-path 列表
- [x] 2.6.2 passed（无 semantic regression）→ 继续
- [x] 2.6.3 的结果记入文档 → 进入 Step 2.7
- [x] 2.6.2 failed（有 semantic regression）→ 回到 Step 2.5 修复
- [x] 2.6.1 failed（timing 不全）→ 回到 Step 2.5 修复

## Step 2.7: .NET 8 对比

```bash
python -m verification.entry_points.cli \
    <slug> \
    --assembly System.Private.CoreLib \
    --mode strict \
    --native-config profile \
    --verbose
```

从报告中提取 chaos-aot 和 chaos-jit 的 ns/op，对比 .NET 8 基线。
**必须排除 Step 2.6.1 标记的 exception-path methods：**

```bash
bash testing/scripts/check-net8-slowdown.sh <slug>
```

检查条件：
- 仅对**非 exception-path** methods 检查 `aot_slowdown_vs_net8 <= 20%`
- 仅对**非 exception-path** methods 检查 `jit_slowdown_vs_net8 <= 20%`
- Exception-path methods 自动跳过，输出警告

### 出口条件

- [x] passed → 进入 Step 2.8
- [x] failed → 诊断根因 → 回到 Step 2.5 优化 → 重跑，最多 3 次（计入 attempt 配额）
- [x] 超限 → 写 `blocker.md`，跳到 Step 2.9 清理

## Step 2.8: HotUpdate 检查

```bash
python -m verification.entry_points.cli \
    <slug> \
    --assembly System.Private.CoreLib \
    --mode strict \
    --native-config profile \
    --verbose
```

### 第一层：必须确认有真实 patch 被应用

```bash
python -c "
import json
hu = json.load(open('testing/foundation-dll/System.Private.CoreLib/<slug>/hotupdate-verification-report.json'))
assert hu.get('d3PatchApplied', False), 'd3PatchApplied=false: no real patch DLL deployed'
"
```

如果 `d3PatchApplied=false`：
- postPatchNsPerOp=0.0 是**预期行为**（没有真实 patch DLL）
- 输出警告：此 family 的 hotupdate 数据不可用，需创建 patch 项目补充
- **此步骤跳过**，进入 Step 2.9（不阻塞优化，但文档必须说明）

### 第二层：验证 hotupdate 语义和开销（仅 d3PatchApplied=true 时执行）

```bash
bash testing/scripts/check-hotupdate.sh <slug>
```

检查条件（仅 patch 已应用时）：
- `semanticChangedCount > 0`（patch 确实改变了语义）
- AOT hotupdate overhead ≤ 100%
- JIT hotupdate overhead ≤ 100%

### 出口条件

- [x] passed → 进入 Step 2.9
- [x] d3PatchApplied=false（预期）→ 进入 Step 2.9
- [x] failed → 诊断根因 → 回到 Step 2.5 修复 → 重跑，最多 3 次
- [x] 超限 → 写 `blocker.md`，跳到 Step 2.9 清理

## Step 2.9: 文档 + Commit + Merge + CI + Pull

### 2.9.1 文档

创建 `docs/optimize/YYYY-MM-DD-<slug>/README.md`：

```markdown
# 优化：<slug>

## 优化对象
- family: `<slug>`
- 涉及方法: <list>

## 问题根因分析

## 优化循环记录

| Attempt | 假设 | 预期提升 | 实际提升 | 结果 |
|---------|------|---------|---------|------|
| 1       | ...  | Z%      | Z'%     | ✅/❌ |
| 2       | ...  | Z%      | Z'%     | ✅/❌ |

## 性能数据

| 方法 | Baseline (ns) | Optimized (ns) | .NET 8 (ns) | vs .NET 8 | 加速比 |
|------|--------------|----------------|-------------|-----------|--------|

## HotUpdate 开销

| 方法 | 热更前 | 热更后 | 开销 | 路径 |
|------|--------|--------|------|------|

## 收敛检查

- [ ] Phase 1: Subject 审计 + 结构性审计 passed
- [ ] Phase 1: Subject freeze + baseline 已记录
- [ ] Phase 2: Pipeline 全部 passed
- [ ] Phase 2: 至少一次优化假设验证通过
- [ ] Phase 2: 回归验证 passed（semantic + benchmark）
- [ ] Phase 2: vs .NET 8 ≤ 20%（exception-path 排除后）
- [ ] Phase 2: HotUpdate 已确认（为 false 时预期，文档说明原因）
- [ ] Phase 2: Commit + Merge + CI 完成
```

### 2.9.2 Commit

```bash
git add docs/optimize/YYYY-MM-DD-<slug>/
git add <codegen/runtime changes>
git commit -m "$(cat <<'EOF'
perf: optimize <slug>

## 性能对比

| method | baseline | optimized | .NET 8 | vs .NET 8 | speedup |
|--------|----------|-----------|--------|-----------|---------|
| ...    | ...      | ...       | ...    | +X%       | 1.XXx  |

## 根因

<一句话>

## 优化循环摘要

| Attempt | 假设 | 结果 |
|---------|------|------|
| 1       | ...  | ✅/❌ |

## 关键决策

<方案选择理由>

Co-Authored-By: Claude Opus 4.7 <noreply@anthropic.com>
EOF
)"
```

### 2.9.3 Push

```bash
git push origin claim/<slug>/<worker-id>
```

### 2.9.4 Merge → Cleanup

```bash
# 回到 main
git checkout main
git pull origin main

# Merge claim 分支
git merge claim/<slug>/<worker-id> --no-edit

# 如果 merge 失败 → 按文件类型分层解决冲突：
#   - build/工具链文件 + docs/ + 测试数据 JSON → 保留 main 版本
#   - testing/foundation-dll/ 下的合约、entry 文件 → 取 main 版本（claim 分支的属于临时生成）
#   - src/native/ 下的 runtime/codegen 变更 → 取 claim 分支版本
#   - docs/optimize/ 下的优化文档 → 取 claim 分支版本
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
仍执行 2.9.4 清理 worktree，不 push claim 分支到 main，只删除远程 claim 分支。

### 2.9.5 CI 验证

```bash
bash testing/scripts/verify-ci.sh --timeout 10 --branch main
```

**出口条件**：
- CI passed → 进入 2.9.6
- CI failed → 诊断根因，提交修复 PR（不破坏 main），不在当前 worktree 修复
- 超时 → 手动检查 CI 状态，确认通过后继续

### 2.9.6 Pull

```bash
git pull origin main
git status  # 确认干净
```

**最终状态**：`main` 分支，干净 working tree，已同步远程。

### Step 2.9 出口条件（Phase 2 整体出口）

- [x] Pipeline 全部 passed（或 blocker.md 已记录）
- [x] 至少一次优化假设验证通过
- [x] 回归验证 passed（semantic + benchmark）
- [x] vs .NET 8 ≤ 20%（或 blocker.md 已记录）
- [x] HotUpdate 已确认（为 false 时预期）
- [x] `docs/optimize/YYYY-MM-DD-<slug>/README.md` 完整
- [x] git commit + merge + CI + main 已同步

---

# Abort 处理

如果 Phase 2 的优化循环（Step 2.5）、.NET 8 对比（Step 2.7）、HotUpdate 检查（Step 2.8）达到 max_attempts（Logic 类超限或 Design 类无可行方案）：

1. 写 `blocker.md` 到 `optimization-campaign/workers/<worker-id>/blocker.md`，包含：
   - 哪个 Step 失败
   - 尝试了哪些修复（含每次 attempt 的假设和结果）
   - 每次修复的结果
   - 剩余问题
2. 跳到 Step 2.9 清理 worktree
3. **不合并到 main**，只删除远程 claim 分支

# 目录契约

```
optimization-campaign/
└── workers/
    └── <worker-id>/
        ├── blocker.md                  # 仅 abort 时产生（不提交）
        ├── diagnosis.json              # 诊断输出（不提交）
        ├── attempt-<N>-timing.json     # 每次 attempt 的 timing 数据（不提交）
        └── attempt-<N>-rollback.md     # 回滚记录（不提交）

docs/optimize/
├── INDEX.md                            # 项目索引（提交）
└── YYYY-MM-DD-<slug>/
    ├── bottleneck-report.md            # Phase 0.3 瓶颈分析（提交）
    ├── bottleneck-verified.md          # Phase 2.4 验证后瓶颈（提交）
    └── README.md                       # 优化文档（提交）
```

# 时间预算

| Phase | Step | 预估 | 说明 |
|-------|------|------|------|
| Phase 0 | 0.1 扫描 | 3 min | 扫描现有数据，不跑 pipeline |
| Phase 0 | 0.2 排序 | 2 min | 自动生成优先级队列 |
| Phase 0 | 0.3 ROI | 5 min | profile 分析 + 瓶颈定位 |
| Phase 1 | 1.1 Subject 审计+修复 | 4 min | 含 test-fix 自动化 |
| Phase 1 | 1.2 结构审计 | 3 min | contract sync |
| Phase 1 | 1.3 Freeze+Baseline | 3 min | 修复后冻结 |
| Phase 2 | 2.1 Setup+Claim | 2 min | — |
| Phase 2 | 2.2 Pre-flight | 1 min | 自动修复，不阻塞 |
| Phase 2 | 2.3 Pipeline | 10 min | 首次 baseline 测量 |
| Phase 2 | 2.4 瓶颈验证 | 2 min | profile 数据确认 |
| Phase 2 | 2.5 优化循环 ×3 | 45 min | 每次 15 min，含回滚验证 |
| Phase 2 | 2.6 回归验证 | 3 min | semantic + benchmark |
| Phase 2 | 2.7 .NET 8 | 10 min | 对比测量 |
| Phase 2 | 2.8 HotUpdate | 10 min | 语义+开销验证 |
| Phase 2 | 2.9 收尾 | 8 min | doc+commit+merge+CI+pull |
| **总计** | | **~1.9 hours** | 比旧版 ~2.5 hours 更高效 |

超限写 blocker.md，不是停下来等人。

# 收敛检查

## Phase 0 检查
- [ ] `benchmark-scan-report.json` 已生成
- [ ] `optimization-priority.json` 已生成（按 priority_score 排序）
- [ ] 选定优化目标 + ROI 预估通过（预期收益 > 20%）
- [ ] `bottleneck-report.md` 已写入

## Phase 1 检查
- [ ] Step 1.1: Subject 质量审计 PASS + 自动修复完成
- [ ] Step 1.2: 结构审计 verdict = `PASS`
- [ ] Step 1.3: Subject freeze captured, baseline recorded

## Phase 2 检查
- [ ] Step 2.3: Pipeline 全部 stage passed
- [ ] Step 2.4: 瓶颈验证通过（偏差 ≤ 30%）
- [ ] Step 2.5: 至少一次优化假设验证通过
- [ ] Step 2.6: 回归验证 passed（semantic + benchmark）
- [ ] Step 2.6: Exception-path methods 已标记并排除
- [ ] Step 2.7: vs .NET 8 ≤ 20%（或 blocker.md 已记录）
- [ ] Step 2.8: HotUpdate 已确认（为 false 时预期，文档说明原因）
- [ ] Step 2.9: docs/optimize/ 完整 + commit + merge + CI + main 已同步

## 整体
- [ ] 每次 attempt 都有假设、验证、结果记录
- [ ] 回滚记录已保存（如有回滚）
- [ ] blocker.md 已写入（如 abort）
