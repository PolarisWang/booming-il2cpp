# 产品发布流程 — 规范化设计方案

> 日期：2026-09-02
> 域：发布流程 + 协作 + 构建 + 版本治理
> 状态：brainstorm 收敛

---

## 1. 设计基础（Q1-Q4 对齐）

| 决策 | 选项 | 含义 |
|------|------|------|
| **Q1: 谁触发** | B 本地 `release.sh` | 本地脚本驱动全闭环，CI 留作兜底/验证 |
| **Q2: 分支策略** | B 临时快照 | 从 main 最新切临时快照分支，验证通过后存档/tag，不保留长期 release 分支 |
| **Q3: 修复路径** | A 在 worktree 内修复 | 修复提交进 release 快照分支，然后同步到 main（cherry-pick/merge） |
| **Q4: 脚本粒度** | B 子命令 + 状态文件 | 分解成 `init/verify/fix/publish/abort` 子命令，用 `.release_state.json` 做可重入状态机 |

---

## 2. 全流程设计

### 核心状态机

```
                        ┌──────────────┐
                        │    init      │
                        │ 锁版本+分支   │
                        │ + worktree   │
                        └──────┬───────┘
                               │
                               ▼
                        ┌──────────────┐
                  ┌─────│   verify     │─────┐
                  │     │ 门禁全跑     │     │
                  │     └──────┬───────┘     │
                  │ PASS       │             │ FAIL
                  ▼            ▼             ▼
          ┌────────────┐ ┌──────────┐ ┌──────────────┐
          │  publish   │ │  abort   │ │    fix       │
          │ tag+Release│ │ 清理     │ │ 修+提交分支+main│
          │ +merge main│ │ 删除     │ └──────┬───────┘
          └────────────┘ └──────────┘        │
                  │                          │ 重来
                  ▼                          ▼
              DONE                     ┌──────────────┐
                                      │  re-verify   │
                                      │ 重建 worktree│
                                      │ 重新跑测试    │
                                      └──────┬───────┘
                                             │ PASS
                                             ▼
                                       ┌────────────┐
                                       │  publish   │
                                       └────────────┘
```

### 状态文件 `.release_state.json`

```json
{
  "version": "0.2.0",
  "tag": "v0.2.0",
  "snapshotBranch": "release/v0.2.0",
  "snapshotCommit": "abc1234...",
  "worktreeDir": ".worktrees/rel",
  "phases": {
    "init": "done",
    "verify": "failed",
    "fix": "done",
    "reverify": "pending",
    "publish": "pending"
  },
  "verifyResults": {
    "governance": "pass",
    "publishSmoke": "fail",
    "unit": "pass"
  },
  "failures": ["publish-smoke jit: terminate C2039"],
  "createdAt": "2026-09-02T12:00:00Z",
  "updatedAt": "2026-09-02T13:00:00Z"
}
```

---

## 3. 子命令设计

### `release.sh init 0.2.0`

```
1. 版本校验 (SemVer)
2. 获取独占锁 (.git/release.lock)
3. git fetch origin main
4. SNAPSHOT = git rev-parse origin/main
5. git branch release/v0.2.0 ${SNAPSHOT}
6. git worktree add .worktrees/rel release/v0.2.0
7. release_bump.sh 0.2.0 (在 worktree 内)
8. 写入 .release_state.json
```

### `release.sh verify`

```
1. 读取 .release_state.json
2. 检查 worktree 存在
3. 在 worktree 内依次跑:
   a. check_release_governance.py --ci
   b. publish-smoke.py (app/jit/source-only)
   c. test_driver --layer unit --quick
   d. generate-checksums.sh + generate-sbom.sh (确认产物完整性)
4. 所有结果写入 .release_state.json
5. 输出 PASS/FAIL 汇总
6. 退出码: 0=PASS, 1=FAIL
```

### `release.sh fix`

```
1. 读取 .release_state.json (检查 verify=failed)
2. 选手动修复（在 worktree 内修改代码）
   或自动修复（已知模式，如修改头文件）
3. 提交到 release/v0.2.0 分支:
   git add/commit -m "fix(release): ..."
   git push origin release/v0.2.0
4. 创建 PR 同步到 main:
   gh pr create --base main --head release/v0.2.0
   (或 git push origin release/v0.2.0:main 直接推)
5. 更新状态: phases.fix = "done", phases.verify = "pending"
6. 提示: 需要重新跑 verify
```

### `release.sh publish`

```
1. 读取 .release_state.json (检查 verify=pass)
2. 在 worktree 内:
   - release_bump.sh 0.2.0 --tag (打 tag)
   - build-tool-package.sh 0.2.0
   - generate-checksums.sh + generate-sbom.sh
   - generate-release-notes.sh
   - gh release create v0.2.0
   - git push origin release/v0.2.0
   - git push origin v0.2.0
3. 合并回 main:
   git checkout main
   git merge release/v0.2.0 --no-ff
   git push origin main
4. 清理 worktree + 删除快照分支 + 释放锁
5. 更新状态: phases.publish = "done"
```

### `release.sh abort`

```
1. 读取 .release_state.json
2. 清理 worktree (git worktree remove --force)
3. 删除本地快照分支 (git branch -D release/v0.2.0)
4. 释放锁
5. 删除 .release_state.json
6. 输出: 清理完成
```

---

## 4. 完整闭环流程

### 理想路径（一次通过）

```bash
# 1. 初始化
release.sh init 0.2.0
  → 锁版本 → 创建分支 → worktree → 版本 bump

# 2. 验证
release.sh verify
  → governance: PASS
  → publish-smoke: PASS
  → unit: PASS
  → 产物完整性: PASS
  → 全部通过

# 3. 发布
release.sh publish
  → tag → push → GitHub Release → 合并回 main → 清理
  → DONE
```

### 失败修复路径（需要修复）

```bash
# 1. 初始化
release.sh init 0.2.0

# 2. 验证 → 失败
release.sh verify
  → publish-smoke: FAIL (terminate C2039)
  → 状态: verify=failed

# 3. 修复（在 worktree 内修改 + 提交到 release 分支 + 同步到 main）
release.sh fix
  → 在 worktree 内修改代码
  → git add/commit
  → git push origin release/v0.2.0
  → 创建 PR 合并到 main (或直接 push)
  → 状态: fix=done, verify=pending

# 4. 重新验证
release.sh verify
  → publish-smoke: PASS
  → 状态: verify=pass

# 5. 发布
release.sh publish
  → DONE
```

### 放弃路径

```bash
release.sh abort
  → 清理 worktree + 删除分支 + 释放锁
```

---

## 5. 与现有流程的对比

| 维度 | 当前 release.sh | 新设计 |
|------|----------------|--------|
| **状态持久化** | ❌ 无（每次重跑从头开始） | ✅ `.release_state.json` 状态机 |
| **失败循环** | ❌ 手动修 + 手动 `--abort` + 重跑 | ✅ `fix` → `verify` 循环，不丢状态 |
| **修复提交** | ❌ 修 main 但不保证同步到发布分支 | ✅ 在 worktree 内提交到 release 分支 + 同步到 main |
| **worktree 隔离** | ✅ 已有 | ✅ 增强（状态文件追踪） |
| **子命令粒度** | ❌ `--dry-run`/`--publish`/`--abort` 混在一起 | ✅ `init`/`verify`/`fix`/`publish`/`abort` 五大独立子命令 |
| **可重入性** | ❌ 失败后重跑从头开始 | ✅ 读取状态文件，只重跑失败的 phase |
| **产物验证** | ❌ 通过后不记录 | ✅ 每个 verify 的详细结果存 state |
| **锁机制** | ✅ 已有 | ✅ 增强（状态文件与锁联动） |

---

## 6. 实施建议

### 改动范围

| 文件 | 改动 | 工作量 |
|------|------|--------|
| `scripts/release.sh` | 从线性脚本改为子命令状态机 | 完全重写（~400 行） |
| `.gitignore` | 加 `.release_state.json` | 1 行 |
| **总计** | | **1.5 天** |

### 风险

| 风险 | 等级 | 缓解 |
|------|------|------|
| 状态文件与锁一致性 | 🟡 中 | 状态文件在锁内读写 |
| fix 阶段受损代码 | 🟢 低 | worktree 隔离，不影响主工作树 |
| 多 agent 同时 init | 🟡 中 | 锁阻止 |

---

## 7. 一句话总结

> **发布变成状态机：`init`(锁版本+worktree) → `verify`(门禁) → `fix`(修复+同步main) → `verify`(重验) → `publish`(tag+Release+merge)。所有状态持久化到 `.release_state.json`，每个子命令可重入、可恢复、可排查。**