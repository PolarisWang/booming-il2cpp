# 多 Agent 并发下产品发布流程 — 完整方案设计

> 日期：2026-09-02
> 域：发布流程 + 协作 + 构建 + 版本治理
> 目标：git 锁版本 → worktree 隔离 → CI+发布测试 → 全通过才发布 → 否则修 main → 重来

---

## 1. 核心设计原则

### 发布等价于一个"事务"（ACID）

| 属性 | 含义 | 实现 |
|------|------|------|
| **A 原子性** | 发布要么全成，要么全不退 | tag 是原子 git 操作，release.yml 是 CI 原子工作流 |
| **C 一致性** | 发布时版本号、产物、notes 三者一致 | 锁版本快照 + worktree 隔离 |
| **I 隔离性** | 多 agent 发布不互相干扰 | worktree 隔离 + 版本锁文件 |
| **D 持久性** | 发布产物可追溯 | GitHub Release + tag + SBOM |

### 多 agent 并发下发布的第一原则

> **发布 = 锁一个 commit 快照 → 在隔离 worktree 中验证 → 全通过才正式发布 → 否则修 main → 重来**

**不是**在流动的 main 上发，**不是**在脏工作树上发，**不是**在未验证的代码上发。

---

## 2. 完整流程

```
                        ┌─────────────────────────────────────┐
                        │         多 agent 并行开发             │
                        │  codegen / GC / benchmark / release  │
                        │  各自 push 到 main                    │
                        └────────────────┬────────────────────┘
                                         │ 发布者决定发布
                                         ▼
               ╔════════════════════════════════════════╗
               ║  阶段 1: 锁版本快照（独占锁）            ║
               ║  - 获取版本锁（.git/release.lock）       ║
               ║  - 读 VERSION → NEW_VER                ║
               ║  - git fetch origin main               ║
               ║  - git rev-parse origin/main → COMMIT  ║
               ║  - 创建 release 分支:                   ║
               ║    release/v{NEW_VER}                   ║
               ║  - 释放锁                               ║
               ╚════════════════════════════════════════╝
                                         │
                                         ▼
               ╔════════════════════════════════════════╗
               ║  阶段 2: worktree 隔离构建              ║
               ║  - git worktree add .worktrees/rel-    ║
               ║    {NEW_VER} release/v{NEW_VER}        ║
               ║  - cd .worktrees/rel-{NEW_VER}         ║
               ╚════════════════════════════════════════╝
                                         │
                                         ▼
               ╔════════════════════════════════════════╗
               ║  阶段 3: 预热 + 构建 (在 worktree 中)   ║
               ║  - dotnet build Generator + Driver     ║
               ║  - cmake --preset windows-x64-reference ║
               ║  - cmake --build (SDK libs)            ║
               ╚════════════════════════════════════════╝
                                         │
                                         ▼
               ╔════════════════════════════════════════╗
               ║  阶段 4: 验证门禁                       ║
               ║  ⊙ 发布治理检查:                        ║
               ║    check_release_governance.py          ║
               ║  ⊙ publish-smoke 矩阵:                  ║
               ║    publish-smoke.py (app/jit/source)    ║
               ║  ⊙ 单元测试:                            ║
               ║    test_driver --layer unit             ║
               ║  ⊙ 产物完整性:                          ║
               ║    checksums + SBOM 生成 + 验证         ║
               ╚════════════════════════════════════════╝
                      │                        │
                全部通过                   有失败
                      │                        │
                      ▼                        ▼
               ╔════════════════════╗   ╔════════════════════╗
               ║ 阶段 5: 正式发布    ║   ║ 修复并重来          ║
               ║ - version bump     ║   ║ - 删除 release 分支 ║
               ║ - tag v{NEW_VER}   ║   ║ - 修 main 再 push  ║
               ║ - push release 分支 ║   ║ - 释放锁            ║
               ║ - 创建 GitHub      ║   ║ - 重新从阶段 1      ║
               ║   Release          ║   ║                     ║
               ║ - 合并回 main      ║   ║                     ║
               ║ - 释放锁 + 清理     ║   ║                     ║
               ║   worktree         ║   ║                     ║
               ╚════════════════════╝   ╚════════════════════╝
```

---

## 3. 详细执行步骤

### 阶段 1：锁版本快照

```
1. 获取独占锁: .git/release.lock (flock, 非阻塞)
2. 读 VERSION 文件 → NEW_VER
3. git fetch origin main
4. COMMIT = git rev-parse origin/main
5. 创建 release 分支: git branch release/v{NEW_VER} {COMMIT}
   (注意: 不从当前工作树创建，从 origin/main 的固定 commit 创建)
6. 释放锁
```

**为什么从 origin/main 而不是当前 HEAD 创建**：
- 当前工作树有（其他 agent 的）未提交改动
- origin/main 是远程已确认的稳定状态
- 发布 = 基于"已确认的远程代码"构建，不是"本地未提交的代码"

### 阶段 2：worktree 隔离

```
git worktree add .worktrees/rel-{NEW_VER} release/v{NEW_VER}
cd .worktrees/rel-{NEW_VER}
```

worktree 的**好处**：
- 与主工作目录完全隔离（其他 agent 改主工作目录不影响发布）
- 不需要 `git stash`（项目禁止）
- 不需要切换当前分支（不中断其他 agent 的工作）
- 单目录、可独立管理

### 阶段 3：预热构建

在 worktree 内执行（与多 agent 无共享）：

```
dotnet build src/managed/Chaos.IL2CPP.Generator -c Release
dotnet build src/managed/Chaos.IL2CPP.Driver -c Release
cmake --preset windows-x64-reference
cmake --build --preset windows-x64-reference
```

### 阶段 4：验证门禁

**4a. 发布治理检查**：`python scripts/cleanliness/check_release_governance.py --ci`
- 版本三源一致
- README 有效性
- 发布 tag 存在性

**4b. publish-smoke 矩阵**：`python scripts/publish-smoke.py --json publish-smoke-report.json`
- app / jit / source-only 3 case 全 PASS
- 产物完整性断言（entry.exe + manifest + stdout + exit 0）

**4c. 单元测试**：`python tests/runner/test_driver.py --layer unit --quick`
- 保证 codegen 基础能力无回归

**4d. 构建产物完整性**：`scripts/generate-checksums.sh artifacts/release/` + `scripts/generate-sbom.sh`
- 验证 SDK libs 已构建、checksum 可生成、SBOM 可生成

### 阶段 5a：全部通过 → 正式发布

```
1. 版本 bump: release_bump.sh {NEW_VER} --tag
   (tag 在 release 分支上打，不在 main)
2. 生成 release notes: generate-release-notes.sh
3. 构建 SDK + nupkg: build-tool-package.sh
4. 生成 checksums + SBOM: generate-checksums.sh + generate-sbom.sh
5. push release 分支: git push origin release/v{NEW_VER}
6. push tag: git push origin v{NEW_VER}
   (触发 CI release.yml → GitHub Release)
7. 合并回 main: 在 release 分支上提交 version bump + notes
   → push 到 main (PR 或 merge)
8. 清理 worktree: git worktree remove .worktrees/rel-{NEW_VER}
9. 删除锁: rm .git/release.lock
```

### 阶段 5b：失败 → 修复并重来

```
1. 删除失败的 release 分支（临时，不 push）
   git branch -D release/v{NEW_VER}
2. 记录失败原因（写文件）
3. 修改 main 上的代码并 push
4. 释放锁: rm .git/release.lock
5. 重新从阶段 1 开始（新版本号或同版本号 +1）
```

---

## 4. 改造后的 release.sh 设计

```bash
# release.sh — 多 agent 并发下的安全发布编排
#
# 用法:
#   ./scripts/release.sh 0.2.0              # 阶段 1-4: 锁版本 → worktree → 构建 → 验证
#   ./scripts/release.sh 0.2.0 --publish    # 阶段 5a: 全通过后正式发布
#   ./scripts/release.sh 0.2.0 --abort      # 阶段 5b: 失败后清理
#   ./scripts/release.sh 0.2.0 --dry-run     # 演练
#
# 全流程:
#   release.sh 0.2.0          # 锁版本 + worktree + 构建 + 验证
#   # 检查验证结果
#   release.sh 0.2.0 --publish  # 通过 → 正式发布
#   或
#   release.sh 0.2.0 --abort    # 失败 → 清理

STEPS:
  1. ACQUIRE_LOCK     — 获取 .git/release.lock（flock，非阻塞）
  2. CREATE_RELEASE   — 从 origin/main 创建 release/v{NEW_VER} 分支
  3. WORKTREE         — git worktree add .worktrees/rel-{VER}
  4. BUILD            — 构建 Generator + Driver + SDK
  5. GATE_CHECK       — 治理检查 + publish-smoke + 单元测试 + 产物完整性
  6. REPORT           — 显示验证结果（PASS/FAIL 明细）
  7. CLEANUP          — 验证通过流程: tag + push + Release + merge back
  8. ABORT            — 验证失败流程: 删分支 + 释放锁
```

---

## 5. 提交产物与发布资产

### 验证通过后提交到 main 的改动

| 文件 | 改动内容 | 自动化 |
|------|---------|--------|
| `VERSION` | 版本号递增 | `release_bump.sh` |
| `CMakeLists.txt` | `CHAOS_VERSION` 同步 | `release_bump.sh` |
| `Directory.Build.props` | `<Version>` 同步 | `release_bump.sh` |
| `CHANGELOG.md` | 新增版本条目 | `release.sh` 自动 |
| `RELEASE_NOTES_{VER}.md` | 发布说明 | `generate-release-notes.sh` |

### GitHub Release 发布的资产

| 资产 | 来源 | 格式 |
|------|------|------|
| SDK 库 | `artifacts/release/{VER}/*.lib` | `.lib` 文件 |
| dotnet tool nupkg | `artifacts/release/tool/chaos-il2cpp.{VER}.nupkg` | `.nupkg` |
| 校验和 | `SHA256SUMS` | 文本文件 |
| SBOM | `sbom.cyclonedx.json` | CycloneDX JSON |
| 发布说明 | `RELEASE_NOTES_{VER}.md` | Markdown |

---

## 6. 多 agent 冲突表（谁可以做什么）

| 操作 | main | release/v* 分支 | v* tag | .git/release.lock |
|------|------|----------------|--------|-------------------|
| CI 验证 | ✅ 任意 agent | ✅ 发布者 only | ❌ | ❌ 不碰 |
| git push | ✅ 任意 agent | ✅ 发布者 only | ✅ 发布者 only | ❌ 不碰 |
| 版本 bump | ❌（release 分支做） | ✅ 发布者 only | ✅ | ✅ 持有者 |
| 创建 tag | ❌ | ✅ 发布者 only | ✅ 发布者 only | ✅ 持有者 |
| 删除 tag | ❌ | ❌ | ❌ | ❌ |
| 创建 release 分支 | ❌ | ✅ 发布者 only | ❌ | ✅ 持有者 |
| 获取锁 | ❌ 不碰 | ❌ 不碰 | ❌ | ✅ 持有者 |

---

## 7. 风险与缓解

| 风险 | 等级 | 缓解 |
|------|------|------|
| **R1: 锁超时/死锁** | 🟡 中 | 锁超时 30 分钟，超时自动释放 |
| **R2: release 分支与 main 差异大** | 🟡 中 | 发布频率高（小版本），差异小；hotfix 走 cherry-pick |
| **R3: verify 阶段耗时** | 🟡 中 | publish-smoke ~47s + unit ~50s + 构建 ~5min，总计 <10min |
| **R4: worktree 磁盘空间** | 🟢 低 | release worktree 构建后 ~2GB，可清理 |
| **R5: 其他 agent 在 release 期间 push 到 main** | 🟢 低 | 不影响 release worktree（快照隔离） |
| **R6: 锁文件被并行 agent 误删** | 🟡 中 | 锁文件包含 PID，只有持有者/超时才能删 |

---

## 8. 实施计划

| 阶段 | 内容 | 工作量 | 依赖 |
|------|------|--------|------|
| **P0** | release.sh 改造：锁 + worktree + 分步 publish/abort | 1.5 天 | 无 |
| **P1** | verify 门禁集成（publish-smoke + 治理 + unit） | 0.5 天 | P0 |
| **P1** | CI release.yml 对接 worktree 模式 | 0.5 天 | P0 |
| **P2** | 分支门禁文档化 + hotfix 流程 | 0.5 天 | P0+P1 |
| **合计** | | **3 天** | |

---

## 9. 一句话总结

> **发布 = 锁 origin/main 快照 → worktree 隔离 → 构建 + publish-smoke + 单元测试 → 全通过才 tag + Release → 否则修 main → 重来。**
>
> 这个流程让多 agent 同时开发时，发布者与其他 agent 的操作**完全隔离**（worktree + 锁 + 分支），验证**不依赖本地工作树状态**（固定 commit 快照），失败**不污染 main**（只在临时 release 分支上操作）。