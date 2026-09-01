# 发布流程改动 — 风险管理分析报告

> 审查日期：2026-08-31
> 审查范围：`de36c085d` 到 `ce3fbc9a7`（10 commits，25 files changed，+1864/-59）
> 审查级别：全面（含 GC 快照、版本 bump、工作流、安全工件）

---

## 风险等级说明

| 等级 | 定义 | 行动要求 |
|------|------|---------|
| 🔴 **P0 — 阻断** | 可能导致 CI 失败、数据丢失、或发布产物不可用 | 发版前必须修复 |
| 🟡 **P1 — 严重** | 可能影响日常开发流程或发布质量 | 建议在下次发布前修复 |
| 🔵 **P2 — 建议** | 代码质量/可维护性/可读性问题 | 下次迭代时修复 |

---

## 风险清单

### 🔴 P0-1: GC 半成品快照未测试（691bb1558）

| 属性 | 值 |
|------|-----|
| 文件 | `src/native/runtime-core/gc/{gc_bgc.cpp,gc_demotion.cpp,gc_gen1.cpp,gc_lock.h,gc_old_gen.cpp,gc_old_gen.h,thread_state.cpp,thread_state.h}` |
| 改动 | 8 文件，+136/-53 行 |

**描述**：工作区存在另一并行工作流的 GC 半成品改动（gc_bgc/demotion/gen1/lock/old_gen/thread_state）。为满足 `release.sh` 的干净工作树要求，以 "chore(gc): 保留并行工作流的 GC 未提交改动（发布前快照）" 原样提交。**未经过任何 GC 测试验证**。

**风险**：若后续 CI 中 GC 测试失败（如 `gc-ci.yml` 跑 `gc-ci` 或 `gc-stress-nightly`），根因可能指向此快照而非当前工作流改动。这将导致诊断成本增加和归因困难。

**缓解**：
- 此快照本身是"记录"性质的，不是"功能"性质的。所有 GC 测试的回归需要由快照作者负责排查
- 建议在 CLAUDE.md 或 STATUS.md 中标记此快照的范围和责任人
- 下次恢复 GC 工作流时，应优先运行 `gc-ci.yml` 确认此快照无回归

---

### 🟡 P1-1: pre-push 分支检测只读 REMOTE_DEFAULT 非实际目标

| 属性 | 值 |
|------|-----|
| 文件 | `.githooks/pre-push` |
| 行号 | 53 |
| 风险 | 中等 |

**描述**：`TARGET_BRANCH="${REMOTE_DEFAULT:-main}"` 始终解析为 `origin/main`（由第 24-29 行从 `refs/remotes/origin/HEAD` 推导），而非 **实际推送的目标分支名**。这意味着：

```
# 用户推送到 feature/my-fix
git push origin feature/my-fix
# → TARGET_BRANCH 仍然是 "main"，不是 "feature/my-fix"
# → HARD_GATE=1，触发 blocking
# → 用户困惑：为什么改 feature 分支被 blocking？
```

**影响**：`--no-verify` 会变成日常习惯，降低门禁有效性。feature 分支开发者也面临 L1 review 阻断，与"feature 分支保持 advisory"的设计意图矛盾。

**修复方案**：pre-push hook 接收标准输入流（`git push` 通过 stdin 传递目标 ref 信息），应从 stdin 解析目标分支名：

```bash
# pre-push hook 的标准输入格式：
# <local-ref> <local-oid> <remote-ref> <remote-oid>
while read local_ref local_oid remote_ref remote_oid; do
    TARGET_BRANCH="${remote_ref#refs/heads/}"
done
```

---

### 🟡 P1-2: gh CLI 未认证导致本地 Release 创建失败

| 属性 | 值 |
|------|-----|
| 文件 | `scripts/release.sh` |
| 行号 | 248-251 |
| 风险 | 高（本地不可用，CI 不受影响） |

**描述**：`release.sh --publish` 第 8 步执行 `gh release create`，但本地 `gh auth status` 显示未登录。`release.yml` 工作流通过 `GITHUB_TOKEN` 自动认证，本地执行则直接失败。

**影响**：本地执行 `--publish` 全流程会停在 Release 创建步骤。用户需要先运行 `gh auth login` 或设置 `GH_TOKEN` 环境变量。

**缓解**：脚本前端已检查 `gh` CLI 存在性（第 91-95 行），但未检查认证状态。建议增加：

```bash
if [ "$DO_PUBLISH" -eq 1 ]; then
    if ! gh auth status 2>/dev/null; then
        echo "Error: gh CLI not authenticated. Run 'gh auth login' or set GH_TOKEN." >&2
        exit 1
    fi
fi
```

---

### 🟡 P1-4: release.sh 合并回 main 后未确认 main 的 CI 通过

| 属性 | 值 |
|------|-----|
| 文件 | `scripts/release.sh` |
| 行号 | 265-276 |
| 风险 | 中等 |

**描述**：第 9 步 `git merge --no-ff` 将 release 分支合并回 main 后，立即 `git push origin main`，**未等待 CI 运行结果**。如果 CI 失败（如 foundation-dll 管线因版本变化而中断），破坏已推送到 main。

**影响**：main 分支可能被破坏，需要额外的 revert 提交。

**缓解**：
- 建议在 `--publish` 模式下，合并后先 push 到临时分支或使用 PR 合并
- 或增加 `--no-merge` 选项让用户手动处理合并（已存在 `--no-merge` 选项）

---

### 🔵 P2-1: release.sh 使用 `--verify-tag` 但未验证 tag 与 version 一致

| 属性 | 值 |
|------|-----|
| 文件 | `scripts/release.sh` |
| 行号 | 250 |
| 风险 | 低 |

**描述**：`gh release create` 使用 `--verify-tag` 确保 tag 存在，但未验证 tag 中的版本号与 `NEW_VER` 变量一致。如果用户调用 `release.sh 0.1.2 --publish` 而 tag 是 `v0.1.1`（已经存在），`--verify-tag` 通过但实际版本不匹配。

---


### 🔵 P2-3: gen_release_notes.py 的 "patch" 关键词匹配过宽

| 属性 | 值 |
|------|-----|
| 文件 | `scripts/ci/gen_release_notes.py` |
| 行号 | 84 |
| 风险 | 低 |

**描述**：HotUpdate 域匹配包含 `" patch" in lc`（带空格前缀），意图匹配 hotupdate patch 相关 commit，但可能误匹配包含 "patch" 的通用术语（如 "patch release"）。

---

### 🔵 P2-4: release.yml 的 release-notes 步骤输出变量未使用

| 属性 | 值 |
|------|-----|
| 文件 | `.github/workflows/release.yml` |
| 行号 | 99-100 |
| 风险 | 低 |

**描述**：`release-notes` job 声明了 `outputs: { notes-file: release-notes.md }`，但没有任何下游 job 消费这个输出（`publish` job 直接通过 `actions/download-artifact` 下载，而非通过 `needs.release-notes.outputs`）。该输出定义是死代码，但文档价值仍然存在（表明意图）。

---

### 🔵 P2-5: release.sh 的 SDK 构建不完整

| 属性 | 值 |
|------|-----|
| 文件 | `scripts/release.sh` |
| 行号 | 202-216 |
| 风险 | 中（发布产物体积/完整性） |

**描述**：SDK 构建步骤使用 `find artifacts/presets -type f -name '*.lib' -o -name '*.a' -exec cp {} ...` 扁平化拷贝。这会导致：
- 不同配置（Debug/Release）的 `.lib` 文件互相覆盖（同名文件只能保留一个）
- 缺少 `.dll`（运行时 DLL）和头文件（include/）
- 不适用于跨平台发布（只构建了当前平台）

**缓解**：`release.yml` 工作流已经在 CI 中分别构建 Windows + Linux 平台。`release.sh` 的本地构建主要用于验证，建议在文档中说明。

---

### 🔵 P2-6: codeql.yml 的 C++ 构建未配置 MSVC 工具链

| 属性 | 值 |
|------|-----|
| 文件 | `.github/workflows/codeql.yml` |
| 行号 | 46-48 |
| 风险 | 中（CodeQL C++ 分析可能失败） |

**描述**：C++ 分析使用 `autobuild` 自动检测构建系统。在 Ubuntu runner 上，CMake 预设 `linux-x64-native` 可用，但 `autobuild` 可能无法正确检测预设。`continue-on-error: true` 意味着如果 autobuild 失败，分析步骤会静默继续。

---

## 风险热力图

```
风险等级    P0     P1     P2
严重性
 高        1      2      0
 中        0      2      2
 低        0      0      2

总计: P0=1, P1=4, P2=4
```

## 分类汇总

### 发布流程风险（release.sh + release.yml）
- P1-2: gh CLI 未认证 → 本地 Release 创建失败
- P1-3: gh CLI 参数不一致
- P1-4: 合并回 main 后未确认 CI 通过
- P2-1: --verify-tag 未验证版本一致性
- P2-4: release.yml 死代码输出
- P2-5: 本地 SDK 构建不完整

### 门禁风险（pre-push）
- P1-1: 分支检测只读 REMOTE_DEFAULT 非实际目标

### 代码质量风险（PublishController）
- 无问题（`ResolveToolVersion` 改动正确，已编译通过）

### 安全风险（Dependabot + CodeQL）
- P2-6: C++ autobuild 可能失败（`continue-on-error` 掩盖）

### 遗传风险（GC 快照）
- **P0-1: GC 半成品快照未测试** ← 唯一 P0

### 产物风险（checksums + SBOM）
- P2-2: SBOM serialNumber 使用 SHA1 而非 UUID

---

## 结论

### 总体评价：🟡 中等风险

**P0（1 项）**：GC 快照未测试是本次发布中最大的单一风险，但它是"记录性"而非"功能性的"——它只是把已有的工作区改动提交了，没有引入新逻辑。风险在于后续归因。

**P1（4 项）**：主要是 pre-push 分支检测逻辑错误和 gh CLI 认证问题。pre-push 的 bug 会导致 feature 分支开发者体验下降（需要 `--no-verify`）。gh CLI 认证问题影响本地发布演练但不影响 CI。

**P2（4 项）**：主要是代码质量和可维护性问题，不影响功能正确性。

### 需要立即修复的项（P0-P1）

| # | 优先级 | 问题 | 预估修复时间 |
|---|--------|------|------------|
| P1-1 | 🔴 | pre-push 分支检测只读 REMOTE_DEFAULT | 0.5 天 |
| P1-2 | 🟡 | gh CLI 认证检查缺失 | 0.25 天 |

### 下次迭代修复（P2）

| # | 问题 | 预估修复时间 |
|---|------|------------|
| P2-2 | SBOM serialNumber 格式 | 0.25 天 |
| P2-6 | CodeQL C++ autobuild 配置 | 0.25 天 |

### 不建议立即修复的项

| # | 问题 | 原因 |
|---|------|------|
| P0-1 | GC 快照未测试 | 快照是并行工作流的存量，发版后由原工作流负责人验证 |
| P1-3 | gh CLI 参数不一致 | 两个路径不会同时执行，不会冲突 |
| P1-4 | 合并回 main 未确认 CI | 可通过 `--no-merge` 手动处理 |
| P2-1 | --verify-tag 版本一致性 | 场景罕见，用户错误使用脚本时才有风险 |
| P2-3 | "patch" 关键词过宽 | 误匹配概率低，不影响正确性 |
| P2-4 | release.yml 死代码输出 | 不影响功能 |
| P2-5 | 本地 SDK 构建不完整 | release.yml 的 CI 构建已覆盖多平台 |