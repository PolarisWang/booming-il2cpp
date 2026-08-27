# 并行 session 的 git index 干扰与误吸收教训

> 记录 2026-08 多次出现的**多并行 session 共用同一 git 仓库**时，index 竞态导致的 commit 误吸收、staged 被卷入并行 commit 链、以及正确的处置方式。属跨功能复用的项目级 git 工作流约束。

## 背景

本仓库实际由**多个并行 session（多个 agent 实例）同时**在一个 `main` 分支上工作。git 的 `index`（staging area）是**进程级共享资源**，不是分支私有的。当两个 session 交错执行 `git add` / `git commit` / `git reset` 时，会产生一系列特有的竞态，反复作用于实际工作，且与 `recurring-branch-switch`（分支被外部切换）是同一家族的不同变体。

## 已观测的干扰模式（2026-08，全部实踩）

| # | 模式 | 现象 | 实例 |
|---|------|------|------|
| 1 | **staged 被并行 commit 扫走** | 我 `git add` 了文件，还没来得及 `commit`，并行 session 的 `git commit` 把整个 index（含我的文件）一次提交，标成它的 message | 方案的 WCT/根因产物被 crypto session 扫进 `81970017b` |
| 2 | **commit 误吸收并行已 staged 的 index** | 我 `git add <只1个文件>` + `git commit`，但 index 里已被并行 session staged 了它自己的 20 个文件，我的 commit 把它们全带进 | 方案1 note commit `56b1a60c5` 误吸收 hygiene 清理 20 文件 |
| 3 | **重置竞态** | 我 `reset --soft` 想拆分，但并行 session 同时在 `git reset`/`git add`，最终把我的 staged note 卷进**它的新 commit 链**（`9254007e0`+`19d3a9676`） | 方案1 §六 note 最终落在并行 `feat(hygiene)` 链里 |
| 4 | **fresh index.lock** | 并行 session 活跃提交时，`index.lock` 反复出现（0 字节、新时间戳），我的 `git add`/`commit` 间歇性失败 | 多次出现，需轮询等待 |
| 5 | **stale index.lock**（区别 4） | 22+ 分钟 0 字节旧锁，持锁 git 进程疑似卡死（如卡 `booming` remote），仓库死锁 | 方案3 提交前遇到的 22min stale lock |

## 关键判别：fresh vs stale index.lock

**不能盲目删 `index.lock`**，先判别：

- **fresh**（新时间戳，持锁 git.exe 有 CPU/内存增长）＝并行 session **真在写 index** → **必须等待**，不可删。
- **stale**（0 字节且 20+ 分钟无进展，持锁进程疑似卡死）＝可删。git 自身标准恢复允许。

判别工具：
```
ls -la --time-style=+%H:%M:%S .git/index.lock      # 看时间戳
tasklist //FI "IMAGENAME eq git.exe"                # 看是否持锁进程在活动
```

## 处置纪律（固化）

1. **commit 前必核** `git diff --cached --name-only`，确认只含自己要的文件（防模式 2 —— 我自己的 commit 别误带并行 staged）。
2. **commit 前必核** `git log --oneline -1`，确认 HEAD 与 message 归属（防模式 1/3）。
3. **staged 可能随时被并行扫走**：`git add` 后尽快 `commit`，并把提交做成原子（add+commit 紧邻）。
4. **绝不改写并行已 commit 的链**：即使我的内容被卷入它的 commit，也不去 `reset/rebase/cherry-pick` 拆它——那会破坏并行 line 正在构建的工作。宁可让它"漏标"入库，也不冒险改写活跃平行工作。
5. **误吸收后处理层级**（按侵入递增）：
   - 若误吸收 commit `未 push` 且并行未在上面继续 → 可 `reset --soft HEAD~1` 拆分（用户授权下）。
   - 若并行已在上面继续提交（chain 变长）→ **放弃拆分**，接受"内容已入库、只是 message 归属错"，确保内容 reachable 即可，报给用户。
6. **内容安全性优先**：并行干扰只会造成"commit 归属/message 错"，几乎不会丢内容（磁盘 worktree 状态保留）。处置时先验证内容 reachable（`git merge-base --is-ancestor <commit> HEAD` / `git show HEAD:path`），再谈归属。

## 对 GC task#16 的影响（交付状态）

并行 git 干扰导致多次 commit 重排，但**所有内容最终安全入库并 reachable**：

| 产出 | commit | 状态 |
|------|--------|------|
| WCT 工具 build + 盲区实证 + 根因刻划 | `058fdf2ee` | ✅ 独立 docs(gc) commit |
| 方案3 `ParallelMarkWorkers` 接线（止血） | `3dd8ab4cb` | ✅ 独立 fix(gc) commit |
| 方案1 §六 patch 草稿（根治） | 在 `9254007e0`/`19d3a9676`（并行 hygiene 链内） | ⚠️ 内容已入库 reachable，但 message 是 `feat(hygiene)`，非独立 docs(gc) |

方案1 §六草稿文件：`docs/dev/in-progress/gc-align-coreclr/notes/mark-hang-static-lockanalysis-2026-08-27.md`

## 结论

并行 session 是常态而非例外。Git index 的共享性使"staged 被卷入 / commit 误吸收 / 拆分成撞车"成为高概率事件。**正确姿态**：原子化提交、commit 前核 staged+HEAD、绝不改写并行链、内容 reachable 优先于 commit 归属完美。若确实需要拆分且未 push，在用户授权 + 无并行活跃时用 `reset --soft` 一次性完成，不要与并行反复拉锯。
