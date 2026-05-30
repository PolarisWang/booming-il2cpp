---
name: dev-optimization-campaign
description: >
  全自动 family 优化流水线。线性 14 步驱动：Setup → Audit(结构+正确性) → Freeze+Baseline → 
  Pre-flight → Pipeline → Fix Loop(冻结守卫) → Perf Check+Regression → .NET 8 对比 → 
  HotUpdate Check → Docs → Commit → Merge → Cleanup → CI → Pull。
  每步 bounded retry（max 3），Worktree 强制，Merge 内置。全流程不中断。
---

# 优化战役 — 全自动流水线

## 核心原则

1. **Worktree 强制** — 所有开发必须在 worktree 中完成，无例外
2. **全自动收口** — merge + push + worktree 清理是流程标准终点，不是可选步骤
3. **分类修复** — 失败按类型分流：Infrastructure → 1 次重建，Logic → 3 次迭代，Design → 调研后再进入修复。不混用 retry budget
4. **禁止 hack 测试代码** — 必须直接修复 codegen 或 il2cpp runtime。Subject 文件在 Step 1.6 冻结后**禁止修改**
5. **数据完整性是硬要求** — benchmark timing > 0，exception-path 方法自动排除，hotupdate d3PatchApplied 必须先为 true 再检查 semantic_changed > 0
6. **Subject 冻结** — Step 1.6 后 managed/subjects/ 和 handwritten/ 下的 .cs 文件被冻结，优化循环中不得修改。Step 4/6/7 每次重跑前自动验证冻结
7. **文档语言统一为中文** — 分析文档、commit message 均用中文（代码片段、数据表、缩写除外）

## 流水线概览

```
Step  1: Setup Worktree       → EnterWorktree + claim family
Step  1.5: Pre-audit          → 结构审计 + 正确性审计（两阶段）
Step  1.6: Freeze + Baseline  → 冻结 subject + 捕获 pre-opt 基线
Step  1.7: Pre-flight Check   → 验证构建系统、源文件、二进制合约完整性
Step  2: Run Pipeline         → python -m verification.entry_points.cli <slug> --mode strict --native-config profile
Step  3: Classify & Diagnose  → 按失败类型分流（Infra / Logic / Design / Regression），输出根因
Step  4: Fix & Rerun          → 按分类使用不同 retry budget 修复 + 重跑（每轮前 --verify-freeze）
Step  5: Perf + Regression    → timing 完整性 + 语义回归检查 + 性能回归检查（不重跑 pipeline）
Step  6: .NET 8 对比          → 跑 pipeline（--native-config profile），vs .NET 8 ≤ 20%？不满足 → 诊断 + 优化 + 重跑，最多 3 次
Step  7: HotUpdate Check      → 跑 pipeline（--native-config profile），semantic_changed > 0？overhead ≤ 100%？不满足 → 修复 + 重跑，最多 3 次
Step  8: 文档                 → 写 docs/optimize/YYYY-MM-DD-<slug>/README.md
Step  9: Commit               → git add + git commit（含性能表）
Step 10: Push                 → git push origin claim/<slug>/<worker-id>
Step 11: Merge → Cleanup      → checkout main → merge → push → del worktree branch
Step 12: CI Verify            → 等待 CI 通过（超时 10 min）
Step 13: Pull                 → git pull origin main（确保 main 最新且干净）
```

## 自动化执行说明

- 每个 Step 执行完自动进入下一个 Step，不等待用户确认
- Step 1.7 预检查出问题 → 自动修复后继续，修复失败不阻塞（标记 infra 信息进入 Step 2）
- Step 4 的修复循环按分类使用不同 retry budget：Infrastructure 1 次，Logic 3 次，Design 不 retry
- Step 6/7 的修复循环使用 Logic 类 retry（max_attempts=3），超出后写 `blocker.md` 并 abort
- Infrastructure 类失败升级为 Logic 类后，**attemp 计数器重置**（infra 尝试不计入 logic quota）
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
mkdir -p docs/optimize/.claims
echo "claimed-by: <worker-id>" > docs/optimize/.claims/<slug>.claim
echo "claimed-at: $(date -u +%Y-%m-%dT%H:%M:%SZ)" >> docs/optimize/.claims/<slug>.claim
git add docs/optimize/.claims/<slug>.claim
git commit -m "claim: <slug> by <worker-id>"
git push origin claim/<slug>/<worker-id>
```

如果 push 被拒（别人已 claim）→ abort，选另一个 family。

---

## Step 1.5: Pre-verification Audit — 两阶段审计

在跑完整 pipeline 之前，先审计受试 family 的测试代码完整性和正确性。

### Phase A: 结构审计（已有逻辑）

```bash
python -m verification.stages.pre_verification_audit <slug> --assembly System.Private.CoreLib --json
```

检查内容：
- `customEntryIndices` 与实际情况是否同步
- 非 auto-callable 方法是否有 handwritten 实现
- 标注完整性（benchmarkMethodIndices、hotupdateMethodIndices）

### Phase B: 正确性审计（新增）

```bash
python -m verification.stages.subject_correctness_audit <slug> --assembly System.Private.CoreLib
```

检查内容：

| 检查项 | 检测目标 | 示例发现 |
|--------|---------|---------|
| Assertion Quality | CustomEntrySubject_N() 是否有 Assert.* 调用 | reflection-member-complete: 17 方法 0 Assert |
| Stub Detection | `_ = 0;` / `_ = default(T)!.Member` | reflection-member-complete: 2 个 `_ = 0;`, 15 个 `_ = default` |
| Triviality Detection | Subject_N() 是否总走 catch 路径 | — |
| Benchmark Input Check | benchmark 标注的方法是否始终抛异常 | — |

正确性审计不阻塞 pipeline，但 **WARN 级别的问题必须在优化前修复**。

### 审计判决

读取 stdout 的 JSON 输出，根据 `verdict` 字段决定后续动作：

| 判决 | 含义 | 动作 |
|------|------|------|
| `PASS` | 测试代码完整有意义 | → 进入 Step 1.6 |
| `MISSING_HANDWRITTEN` | 有方法不可 auto-call 且无 handwritten 实现 | → 执行自动修复 |
| `STALE_METADATA` | contract.json 的 customEntryIndices 与实际情况不同步 | → 执行自动修复 |
| `ISSUES_FOUND` | 正确性审计发现问题 | → 修复问题后继续 |

### 自动修复

```bash
python -m verification.stages.pre_verification_audit <slug> --assembly System.Private.CoreLib --fix
```

`--fix` 会做两件事：
1. 更新 `capability-family-contract.json` 的 `customEntryIndices`（添加缺失索引、移除冗余索引）
2. 在 `handwritten/` 下生成缺失的 `{ClassName}.Custom.cs` 存根文件（含 TODO 注释）

如果 Phase B 发现 assertion 缺失或 stub 问题，手动修复 handwritten 代码。

生成后重新运行审计确认：

```bash
python -m verification.stages.pre_verification_audit <slug> --assembly System.Private.CoreLib --json
python -m verification.stages.subject_correctness_audit <slug> --assembly System.Private.CoreLib
```

### 出口条件

- ✅ 结构审计 verdict = `PASS` → 进入 Step 1.6
- ✅ 正确性审计无 WARN 级别问题 → 进入 Step 1.6
- ⚠️ 已修复完毕，重审计 PASS → 进入 Step 1.6
- ❌ 非 auto-callable 方法过多（>50%）→ 考虑是否跳过此 family 优化

---

## Step 1.6: Subject Freeze + Baseline Capture

在开始优化前，冻结受试 subject 代码，捕获优化前的语义基线和性能基线。

### 1.6.1 冻结 Subject

```bash
python -m verification.stages.pre_verification_audit <slug> --assembly System.Private.CoreLib --freeze
```

此命令：
1. 计算 `managed/subjects/` 和 `handwritten/` 下所有 .cs 文件的 SHA256 哈希
2. 写入 `subject-freeze-manifest.json`
3. 备份冻结副本到 `subject-freeze/` 目录

**冻结期间禁止修改** `managed/subjects/` 和 `handwritten/` 下的任何文件。
Step 4/6/7 每次重跑 pipeline 前自动验证冻结：

```bash
python -m verification.stages.pre_verification_audit <slug> --assembly System.Private.CoreLib --verify-freeze
```

如果冻结被打破 → 恢复 subject 文件，修改必须来自 codegen/runtime 而非 test。

### 1.6.2 捕获优化前基线

```bash
# 1. 跑 managed_record 捕获 golden record
python -m verification.entry_points.cli \
    <slug> --assembly System.Private.CoreLib --mode strict \
    --stages managed_record --verbose

# 2. 保存基线 golden record
cp native/golden-record.json native/baseline-golden-record.json
```

首次完整 pipeline（Step 2）跑完后，性能基线会自动保存在 `perf/benchmark-history.jsonl` 中。

### 1.6.3 记录基线 commit

```bash
git rev-parse HEAD > native/baseline-commit.txt
echo "Baseline captured at commit $(cat native/baseline-commit.txt)"
```

### 出口条件

- [x] subject-freeze-manifest.json 已生成
- [x] baseline-golden-record.json 已保存
- [x] baseline-commit.txt 已记录

---

## Step 1.7: Pre-flight Check

在跑完整 pipeline 之前，先验证构建系统和源文件基础设施的完整性。**约 60%+ 的 pipeline 假性失败源于构建系统/缓存/文件缺失问题，预检可以在 30-60 秒内过滤掉。**

### 1.7.1 源文件完整性验证

```bash
# 检查 native/ 目录下所有关键源文件存在
_check_file "${native_dir}/runtime-entry.cpp" "runtime-entry.cpp" || fail=1
_check_file "${native_dir}/verification_dispatch.generated.cpp" "verification_dispatch.generated.cpp" || fail=1
_check_file "${native_dir}/microbench.cpp" "microbench.cpp" || fail=1
_check_file "${native_dir}/CMakeLists.txt" "CMakeLists.txt" || fail=1
```

如果缺失：
- `runtime-entry.cpp` / `verification_dispatch.generated.cpp` → 需要重新跑 codegen 或从其他 family 复制模板
- `microbench.cpp` → 自动生成 fallback 版本（技能 Step 4 的 hotupdate rebuild 已有此逻辑）

### 1.7.2 构建系统验证

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

检查 vcxproj 中是否包含 `runtime-patchdata.cpp`、`verification_dispatch.generated.cpp` 等关键源文件。如果 cmake configure 时这些文件不存在，vcxproj 不会引用它们。

### 1.7.3 二进制合约验证

```bash
cmake --build "$build_dir" --config RelWithDebInfo --target entry --parallel
_entry_binary=$(_find_entry_binary "$build_dir")

# 验证 kSubjectEntryCount > 0
_symbol_count=$(strings "$_entry_binary" | grep -c "kSubjectEntryCount" || echo 0)
if [ "$_symbol_count" -eq 0 ]; then
    echo "FAIL: kSubjectEntryCount not found in entry binary"
    echo "      可能原因: native-aot.generated.cpp 缺失或未编译"
    echo "      修复: 确认 codegen 输出存在，cmake 配置正确"
    exit 1
fi
```

> 此步仅验证二进制包含预期符号，不验证符号值。完整的符号值验证在 Step 5 通过 benchmark timing 确认。

### 1.7.4 自动修复

如果预检发现问题：
- **源文件缺失** → 从 codegen 输出拷贝或生成模板
- **CMakeCache 过期** → 删除后重新 configure
- **符号不正确** → 确认 codegen 输出 → 确认 cmake 源文件列表 → 重新构建

修复后重新执行 1.7.3 验证。如果仍失败 → 记录为 **Infrastructure 类失败**，进入 Step 3 诊断。

### 出口条件

- ✅ 全部验证通过 → 进入 Step 2
- ✅ 自动修复后验证通过 → 进入 Step 2
- ❌ 自动修复仍失败 → 记录 infra 诊断信息，进入 Step 2（不阻塞，pipeline 会暴露根因）

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

## Step 3: Classify & Diagnose

读取 `unified-verification-report.json`（位于 `testing/foundation-dll/System.Private.CoreLib/<slug>/`），对每个 status=`failed` 或 status=`error` 的 stage 执行三阶段诊断：

### 3.1 失败类型分类

诊断的第一件事不是找根因，而是**判断失败类别**。不同类型的失败需要不同的修复策略和 retry budget：

| 类别 | 典型症状 | 常见根因 | Retry budget |
|------|---------|---------|-------------|
| **Infrastructure** | 编译错、链接错、file not found、cmake 失败、entry.exe 缺符号 | 构建缓存过期、源文件缺失、SDK lib 未同步 | 1 次（重建/重配后 100% 恢复） |
| **Logic** | assert 失败、benchmark timing=-1、hotupdate semanticChangedCount=0 | codegen IR 错误、dispatch ABI 不匹配、运行时 bug | 3 次（现有逻辑） |
| **Regression** | Post-opt semantic mismatch 或 benchmark degradation >5% | 优化引入的新分支未覆盖所有路径、代码生成遗漏 case | 3 次（Logic 类同） |
| **Design** | 同一 Logic 失败 3 次仍未解决、架构不支持 | 技术方案设计缺陷、IL 语义与 AOT 不可兼得 | 不 retry，调研后再修复 |

分类依据（按优先级）：

**诊断问诊单（按顺序检查）：**

1. **编译/链接错误？** → Infrastructure
   - 读 C++ 错误信息（`error CXXXX`、`LNKXXXX`、`undefined reference`）
   - 检查是否是 cmake 缓存过期导致的源文件列表不一致
   - 如果是 `runtime-patchdata.cpp` 相关错误 → Infrastructure

2. **file not found / 目录不存在？** → Infrastructure
   - codegen 输出目录不存在 → 检查 pipeline codegen stage 是否成功
   - `native-aot.generated.cpp` 不在了 → cmake rebuild 时被清理，需重新 codegen

3. **fact assert 失败？** → Logic
   - 对比 IL 与生成代码语义
   - 检查 dispatch thunks 的 ABI 是否正确

4. **benchmark timing = -1.0？** → Logic
   - dispatch thunks ABI 不匹配
   - 如果 timing 异常高（100x+ vs .NET 8）→ 检查是否为 exception-path（null/invalid 输入）
   - exception-path → 标记排除，不是 Logic 问题

5. **hotupdate 相关？** → 两步判断：
   - `d3PatchApplied=false` → Infrastructure（patchdata 未正确嵌入）
     - 检查 `runtime-patchdata.cpp` 是否被 sentinel 覆盖
     - 检查 entry.exe rebuild 后 `kPatchDataSize` 符号值
   - `d3PatchApplied=true` 但 `semanticChangedCount=0` → Logic
     - dispatch 返回状态码而非返回值
     - IR 映射错误导致 interpreter hang

6. **同一 Logic 失败进入第 3 次？** → Design
   - 停止 retry，启动架构调研

### 3.2 根因诊断

根据分类结果执行针对性诊断：

| Stage | Infrastructure 诊断要点 | Logic 诊断要点 |
|-------|------------------------|----------------|
| codegen / jit_codegen | SDK 路径解析失败、dotnet SDK 版本不匹配 | 编译错误 → 定位 codegen emitter |
| fact / fact_jit / managed_fact | entry.exe 未更新、使用了旧缓存 | assert 失败 → 对比 IL 与生成代码语义 |
| audit | — | 原则审计不通过 → 对齐架构规范 |
| asm_compare | — | IR 扩展比异常 → 检查 lowering 路径 |
| benchmark | cmake 构建输出不是最新 binary | timing = -1.0 → dispatch thunks ABI 不匹配；exception-path（timing 100x+ vs .NET 8）→ 标记为 exception-path 排除 |
| hotupdate | `d3PatchApplied=false`：patchdata 未嵌入（检查 `kPatchDataSize`、`runtime-patchdata.cpp` 内容） | `semanticChangedCount=0`：dispatch 返回状态码而非返回值；IR 映射错误 |

### 3.3 输出

**出口条件**：所有 failed stage 的类别和根因已写入 `diagnosis.json`：

```json
{
  "stages": {
    "hotupdate": {
      "classification": "infrastructure",
      "root_cause": "runtime-patchdata.cpp was sentinel (size=0) instead of real patchdata",
      "fix_strategy": "regenerate patchdata with --aot-core-ir and rebuild entry.exe"
    }
  },
  "patterns": []
}
```

---

## Step 4: Fix & Rerun

根据 Step 3 的分类结果执行针对性修复：

### 4.1 Infrastructure 类修复（max_attempts=1）

基础设施问题通常一次重建即可恢复。不要浪费 retry 次数：

```bash
# 修复 cmake 缓存过期
rm -rf "$build_dir/CMakeCache.txt" "$build_dir/CMakeFiles"
cmake -S "$native_dir" -B "$build_dir" -DCMAKE_BUILD_TYPE=RelWithDebInfo

# 修复源文件缺失
# 从 codegen 输出拷贝或自动生成
if [ ! -f "${native_dir}/microbench.cpp" ]; then
    # 自动生成 fallback microbench.cpp
    cat > "${native_dir}/microbench.cpp" << 'EOF'
// microbench.cpp — Auto-generated fallback
#include "fast_frame_pool.h"
#include <chrono>
#include <cstdio>
using Clock = std::chrono::high_resolution_clock;
using chaos::il2cpp::runtime_core::tls_frame_pool;
using chaos::il2cpp::runtime_core::FastFramePool;
using chaos::il2cpp::runtime_core::FastFrame;
extern "C" const int kAotMethodCount;
struct BenchmarkResult { double elapsed_ms; int64_t allocated_bytes; bool caught_exception; };
extern "C" BenchmarkResult RunBenchmark(int, int) { return {-1.0, 0, false}; }
extern "C" void RunMicrobench() { printf("microbench: no-op (fallback)\\n"); }
EOF
fi

# 修复运行时 patchdata 被 sentinel 覆盖
if [ -f "${native_dir}/subjects.patchdata" ]; then
    _regenerate_runtime_patchdata_cpp "${native_dir}/subjects.patchdata" "${native_dir}/runtime-patchdata.cpp"
fi

# 重建
cmake --build "$build_dir" --config RelWithDebInfo --target entry --parallel
```

修复后重跑 pipeline（`--native-config check`）验证。如果仍失败 → 升级为 Logic 类，进入 4.2。

### 4.2 Logic 类修复（max_attempts=3）

根据 Step 3 的诊断根因修改代码（codegen / runtime），然后重跑 pipeline。**禁止修改已冻结的 subject 文件。**

每轮重跑前先验证 subject freeze：
```bash
python -m verification.stages.pre_verification_audit \
    <slug> --assembly System.Private.CoreLib --verify-freeze
```

如果 freeze 被打破 → 恢复 subject 文件，确认修改来自 codegen/runtime 而非 test。

```bash
python -m verification.entry_points.cli \
    <slug> \
    --assembly System.Private.CoreLib \
    --mode strict \
    --native-config check \
    --verbose
```

- 如果修复后仍失败 → `attempt += 1`，回到 Step 3 重新诊断
- `attempt > max_attempts(3)` → 升级为 Design 类，进入 4.3

### 4.3 Design 类处理（不 retry）

同一 Logic 失败达到 max_attempts 意味着这不是简单代码 bug，而是架构设计缺陷：

1. 启动 `dev-architecture-first-development` 技能做架构审视
2. 必要时启动 `dev-brainstorm` 技能做方案探索
3. 在 `docs/discuss/` 完成设计讨论
4. 形成实施方案后再进入 Step 4.2 修复

### 4.4 首次发现数据完整性问题的特殊处理

如果 pipeline 全部 passed 但 report 中数据不完整（如 benchmark timing 全为 0.0、hotupdate totalMethods=0），这些不会触发 stage failure，需要单独检查：

```bash
# 检查 unified-verification-report.json 的 benchmark 数据
_report="${results_dir}/unified-verification-report.json"
_bench_timing=$(python3 -c "
import json
r = json.load(open('$_report'))
b = r.get('benchmark', {})
timings = [m.get('elapsedMilliseconds', 0) or 0 for m in b.get('details', {}).get('results', [])]
print(','.join(str(t) for t in timings))
")
if echo "$_bench_timing" | grep -q '^0\.0,'; then
    echo "FAIL: benchmark timing is 0.0 — possible infrastructure issue (stale binary)"
    # 标记为 Infrastructure 类，进入 Step 4.1
fi
```

### 出口条件

- ✅ pipeline 全部 passed + 数据完整 → 进入 Step 5
- ✅ Infrastructure 类已修复 → 进入 Step 5
- ❌ Logic 类 max_attempts 超限 → 升级 Design 类
- ❌ Design 类调研未完成 → 写 `blocker.md`，跳到 Step 11 Cleanup

---

## Step 5: Perf Check + Regression Check（轻量，不重跑 pipeline）

直接从 Step 4 输出的 `unified-verification-report.json` 和 `multi-run/multi-run-report.json` 中提取 benchmark 数据验证：

### 5.1 Timing 完整性 + Exception-path 检测（existing）

```bash
bash testing/scripts/check-perf-timings.sh <slug> --verbose
```

检查项：
- 所有 benchmark timing > 0
- Exception-path 检测：识别因 null/invalid 输入导致始终抛异常的 method（chaos-aot timing 比 .NET 8 慢 20x+），输出警告并排除
- 数据一致性：okCount == totalMethods

### 5.2 语义回归检查（new）

对比优化后的 golden record 与基线，确认语义未被破坏：

```bash
python -m verification.analysis.semantic_regression_check <slug> --assembly System.Private.CoreLib
```

检查项：
- 每个 method 的 pass/fail 状态与基线一致
- 发生 exception 的 method 的 exception type 与基线一致

任何回归（regression）都必须修复后才能进入 Step 6。

### 5.3 性能回归检查（new）

对比优化后的 benchmark 数据与基线，确认性能未退化：

```bash
python -m verification.analysis.perf_regression_check <slug> --assembly System.Private.CoreLib --max-degradation 1.05
```

检查项：
- 每个 method 的 ns/op 与基线对比
- 超过 5% degradation 的方法标记为回归

性能回归不阻塞（优化可能有意 trade off 某些路径），但必须在文档中说明。

### 出口条件
- 5.1 passed → 记录 exception-path 列表（供 Step 6 排除）
- 5.2 passed（无 semantic regression）→ 继续
- 5.2 failed → 回到 Step 3 诊断（新增 Regression 分类）
- 5.3 的结果记入文档 → 进入 Step 6
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

报告路径：`testing/foundation-dll/System.Private.CoreLib/<slug>/unified-verification-report.json`
从该报告中提取 chaos-aot 和 chaos-jit 的 ns/op，对比 .NET 8 基线。
**必须排除 Step 5 标记的 exception-path methods**（这些方法传入 null/invalid 输入导致始终抛异常，timing 反映的是异常处理开销而非转换性能）：

```bash
bash testing/scripts/check-net8-slowdown.sh <slug>
```

检查条件：
- 仅对**非 exception-path** methods 检查 `aot_slowdown_vs_net8 <= 20%`
- 仅对**非 exception-path** methods 检查 `jit_slowdown_vs_net8 <= 20%`
- Exception-path methods 自动跳过，输出警告

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

报告路径：`testing/foundation-dll/System.Private.CoreLib/<slug>/hotupdate-verification-report.json`

验证步骤（分两层）：

**第一层：必须确认有真实 patch 被应用**

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
- **此步骤跳过**，进入 Step 8（不阻塞优化，但文档必须说明）

**第二层：验证 hotupdate 语义和开销**（仅 d3PatchApplied=true 时执行）

```bash
bash testing/scripts/check-hotupdate.sh <slug>
```

检查条件（仅 patch 已应用时）：
- `semanticChangedCount > 0`（patch 确实改变了语义）
- AOT hotupdate overhead ≤ 100%
- JIT hotupdate overhead ≤ 100%

**出口条件**：
- passed → 进入 Step 8
- failed → 诊断根因 → 修复 → 重跑，最多 3 次
- 超限 → 写 `blocker.md`，跳到 Step 11

---

## Step 8: 文档

创建 `docs/optimize/YYYY-MM-DD-<slug>/README.md`：

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
- [ ] Step 5: benchmark timing > 0，数据完整性 OK
- [ ] Step 5: exception-path methods 已排除 `(N methods)`
- [ ] Step 6: vs .NET 8 ≤ 20%（AOT + JIT，exception-path 排除后）
- [ ] Step 7: d3PatchApplied=true → semantic_changed > 0 + overhead ≤ 100%
- [ ] Step 7: d3PatchApplied=false → 预期行为，需手动创建 patch 项目
```

---

## Step 9: Commit

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

如果 Step 4/6/7 的修复循环达到 max_attempts（Logic 类超限或 Design 类无可行方案）：

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

docs/optimize/
├── INDEX.md                    # 项目索引（提交）
└── YYYY-MM-DD-<slug>/
    └── README.md               # 分析文档（提交）
```

## 时间预算

| Step | 预估 | 超时处理 |
|------|------|---------|
| 1 Setup | 2 min | — |
| 1.5 Audit (enhanced) | 3 min | — |
| 1.6 Freeze + Baseline | 2 min | — |
| 1.7 Pre-flight | 1 min | 自动修复，不阻塞 |
| 2 Pipeline | 10-30 min | 每 60s keepalive |
| 3 Classify & Diagnose | 3 min | — |
| 4 Fix loop (Infra×1 / Logic×3) | 30 min | max_attempts → Design / abort |
| 5 Perf + Regression | 2 min | — |
| 6 .NET 8 loop (×3) | 60 min | max_attempts → abort |
| 7 HotUpdate loop (×3) | 30 min | max_attempts → abort |
| 8 文档 | 5 min | — |
| 9 Commit | 1 min | — |
| 10 Push | 1 min | — |
| 11 Merge → Cleanup | 3 min | — |
| 12 CI Verify | 10 min | 超时后手动检查 |
| 13 Pull | 1 min | — |
| **总计** | **~2.5 hours** | |

超限写 blocker.md，不是停下来等人。

---

## 收敛检查

- [ ] Step 1.5: Structural + correctness audit passed
- [ ] Step 1.6: Subject freeze captured, baseline recorded
- [ ] Step 4: Pipeline 全部 stage passed（或 blocker.md 已记录）
- [ ] Step 5: 所有 benchmark timing > 0，数据完整性 OK
- [ ] Step 5: Exception-path methods 已标记并排除
- [ ] Step 5: Regression check passed（semantic + benchmark）
- [ ] Step 6: vs .NET 8 ≤ 20%（AOT + JIT，exception-path 已排除，或 blocker.md 已记录）
- [ ] Step 7: d3PatchApplied 已确认（为 false 时预期，文档说明原因；为 true 时 semanticChangedCount > 0 + overhead ≤ 100%）
- [ ] Step 8: docs/optimize/ 完整
- [ ] Step 9-12: git commit + merge + CI + main 已同步
