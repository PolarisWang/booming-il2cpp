# 多 Agent 并发开发下的产品发布流程 — 深度分析与方案

> 日期：2026-09-02
> 域：发布流程 + 协作 + 构建 + 版本治理
> 背景：本仓库现在是多 agent 并行迭代（codegen/GC/foundation-dll/benchmark/release 各 agent 独立开发，git 为共享状态）

---

## 1. 现状问题：多 agent 并发如何摧毁"传统发布流程"

### 1.1 我经历的直接冲突

在本次会话中（多 agent 并行），我反复遇到：

| 冲突类型 | 具体表现 |
|---------|---------|
| **文件被并行 agent 覆盖** | `native_types.h` `chaos_pch.h` `PublishController.cs` 我改完又被其他 agent 改回/改走，git index.lock 不断被占用 |
| **同一问题多 agent 独立解决** | A3 terminate 问题，我和并行 agent 独立推到了同一个 `<corecrt_terminate.h>` 方案（`d283c3905`），白费了我几十轮调试 |
| **git 领先远程** | 当前 main 领先 origin/main 4 个 commit，都是并行 agent 的独立提交，我 push 会冲突 |
| **工作树永不清净** | 任何时刻都有其他 agent 的未提交改动（gc/benchmark/codegen），release.sh 的"干净工作树"前置检查永远失败 |
| **索引锁争用** | `git add`/`commit`/`push` 因 index.lock 反复失败 |

### 1.2 传统发布流程在多 agent 下的致命缺陷

我当前设计的 release 流程（`release.sh`）假设：

```
前提 A: 发布时工作在 main，工作树干净
前提 B: 只有发布者一个 agent 在改代码
前提 C: push 到 main 不会和其他 agent 冲突
前提 D: 版本号三源唯一、稳定
```

**多 agent 并行下，这四个前提全部失效：**
- A 假：工作树永远有其他 agent 的改动
- B 假：多个 agent 同时 push main
- C 假：push 遇到 lock / 非 fast-forward / 冲突
- D 假：版本号可能被多个 agent 并发改

**结果**：任何一次 `release.sh 0.2.0 --publish` 都会：
1. pre-flight 因工作树脏而失败
2. 或成功 bump 版本后，被其他 agent 的 push 覆盖
3. 或创建一个错误的版本/分支

---

## 2. 核心设计决策：发布必须是"独占操作"

### 多 agent 并发下发布流程的第一原则

> **发布（bump 版本 + tag + 构建产物 + GitHub Release）必须是全局唯一的、串行的、与其他 agent 的开发隔离的独占操作。**

这类似数据库的"长事务"：
- 发布期间，其他 agent 不能 bup 版本、不能动 release/ 分支、不能 push 到被发布的 tag
- 发布必须能在不依赖"干净工作树"的前提下工作（因为工作树永远不干净）
- 发布的结果必须是"在快照上构建"，而非"在移动的 main 上构建"

---

## 3. 方案设计（4 个方案深度对比）

### 方案 A：GitHub Actions 独占发布（推荐）

**核心**：发布完全由 CI（GitHub Actions）执行，从 main/release 分支触发，本地 agent 不直接发。

```
[触发] push tag v0.2.0 或 手动 dispatch release 工作流
   ↓
[CI] GitHub Actions release.yml
    ├── 在干净的 CI 容器 checkout 指定 tag/commit（非移动 main）
    ├── bump 版本、生成 notes/checksum/SBOM
    ├── 构建 SDK + nupkg
    ├── 跑 publish-smoke 验证
    └── 创建 GitHub Release
```

**优点**：
- CI 容器是**干净的**（无并行 agent 干扰）
- checkout 固定 commit，**快照隔离**
- 发布串行（GitHub 保证 tag 唯一的 workflow 只跑一次）
- agent 完全不需要碰 git lock

**缺点**：
- 发布逻辑从本地 bash 移到 YAML，双份
- 需要管理 GitHub 权限/secret
- 本地调试发布逻辑变难

### 方案 B：分支门禁发布（推荐补充）

**核心**：发布走独立的 `release/X.Y.x` 分支，main 只收 PR/merge，tag 只在 release 分支打。

```
feature agent → PR → main (通过 CI)
   ↓ main
   ↓ squash/merge
   ↓ (定期/手动) 从 main 切 release/0.2.x
   ↓ 在 release 分支上跑 CI 全量验证 + publish-smoke
   ↓ 验证通过 → 打 tag v0.2.0 → GitHub Release
```

**优点**：
- main 永远可发布（release 分支是稳定快照）
- agent 开发 merge 到 main，不影响 release 分支
- 版本号在 release 分支确定，main 保持流动

**缺点**：
- 需要维护 release 分支同步
- 历史需要 cherry-pick 或 merge-back

### 方案 C：本地"发布代理"独享写锁

**核心**：引入一个专门的"发布代理"（独立 agent/进程），持有 git 写锁，只有它能 commit/push/tag。

```
[发布代理] 唯一的发布执行者
    ├── 持有 git 写锁（如 .git/release.lock）
    ├── 其他 agent 的 commit/push 必须等锁释放
    └── 发布 = 锁 + 干净快照 + 版本 bump + 产物 + Release
```

**优点**：
- 逻辑仍可在本地 bash（可调试）
- 锁保证独占

**缺点**：
- 需要实现分布式锁（多 agent 共享文件系统才可用）
- agent 之间协调仍靠锁，复杂

### 方案 D：CI 专属 `release.yml` + 本地 `release.sh` 双轨

**核心**：保留本地 `release.sh` 供开发/演练，但**真实发布必须走 CI 的 `release.yml`**。

```
本地:
  release.sh 0.2.0 --dry-run   # 演练，不 push
  release.sh 0.2.0 --publish   # 只 prep（分支+notes），不 push 真实 tag

CI:
  push v* tag → release.yml → 干净构建 → 验证 → GitHub Release
```

**优点**：
- 本地可调试 + CI 可发布，两全
- 真实发布走 CI，避免本地下游（Kits path / git lock）

**缺点**：
- 需要权衡：本地 --publish 是否 push tag
- 双轨需文档化

---

## 4. 推荐组合

**方案 A + B + D 组合**（多 agent 下的完整发布护栏）：

```
1. main 永不清净（多 agent 常态）
     ↓
2. 发布者不直接在移动 main 上发
     ↓
3. 从 release/0.2.x 分支发（方案 B）：main → 定期切 release 分支 → CI 验证
     ↓
4. CI 是唯一真实发布入口（方案 A + D）：
   - release.yml 在干净容器 checkout 快照 + 构建 + publish-smoke + Release
   - 本地 release.sh 只做 prep/演练，不 push 真实 tag
     ↓
5. 并发 agent 的 push 只发生在 main，不碰 release 分支 / v* tag
```

**为什么这三者组合能满足多 agent 发布**：

| 需求 | 方案 A | 方案 B | 方案 D |
|------|--------|--------|--------|
| 干净快照（避开移动 main） | ✅ CI 容器 checkout 固定 commit | ✅ release 分支稳定 | ✅ |
| 串行独占（避免 lock/冲突） | ✅ GitHub 保证唯一 workflow | ✅ 分支隔离 | ✅ |
| 本地可调试 | ❌ | ✅ | ✅ release.sh prep |
| 真实发布可追溯 | ✅ GitHub log | ✅ | ✅ |
| 不依赖干净工作树 | ✅ CI 容器天然干净 | ✅ | ✅ |

---

## 5. 多 agent 发布流程（目标态全貌）

```
                    ┌─────────────────────────────────┐
                    │  main 分支（多 agent 流动)        │
                    │  - codegen / GC / benchmark      │
                    │  - 每个 agent PR/merge 到 main    │
                    │  - 工作树/commit 永远在变          │
                    └──────────────┬──────────────────┘
                                   │ (定期/手动)
                                   ▼
                    ┌─────────────────────────────────┐
                    │  release/0.2.x 分支（发布快照）   │
                    │  - 从 main 切出的稳定快照          │
                    │  - CI 全量验证 + publish-smoke    │
                    └──────────────┬──────────────────┘
                                   │ (验证通过)
                                   ▼
                    ┌─────────────────────────────────┐
                    │  tag v0.2.0 → GitHub Actions     │
                    │  release.yml（CI 独占发布）        │
                    │  - 干净容器 + 快照 checkout        │
                    │  - 构建 SDK + nupkg               │
                    │  - checksum + SBOM                │
                    │  - GitHub Release                 │
                    └─────────────────────────────────┘
```

---

## 6. 实施步骤（立即可做）

### 阶段 1：先解决"版本号冲突"（P0，0.5 天）

版本三源（VERSION/CMake/Directory.Build.props）被多 agent 并发改，是最高危冲突。

**方案**：`release_bump.sh` 加**独占写锁**（基于文件锁），保证同一时刻只有一个 agent 能改版本号。并发 pull 到版本号变化时，release_bump 检测并拒绝（非幂等）。

### 阶段 2：release.sh 改造为"prep-only"（P1，1 天）

- 本地 `release.sh --publish` **不再** push v* tag 到 origin（避免和 CI 竞争）
- 改为 prep（分支 + notes + 版本检查）并输出"待 CI 发布"提示
- 真实发布交给 CI `release.yml`

### 阶段 3：release.yml 增强为完整发布（P1，1 天）

- 已存在 release.yml（tag 触发）
- 增强：构建 SDK + publish-smoke 验证 + checksum/SBOM + GitHub Release
- 用 `actions/checkout` 的 `ref: <tag>` 保证快照

### 阶段 4：分支门禁（P2，1 天）

- 定义 release/X.Y.x 分支流程
- CI 只保护 main 的 PR，release 分支独立验证

---

## 7. 风险

| 风险 | 等级 | 缓解 |
|------|------|------|
| 本地 release.sh 与 CI 双轨不一致 | 🟡 | 文档化 + 同脚本校验 |
| CI 发布增加一次构建耗时 | 🟢 | 复用 artifacts cache |
| release 分支同步漂移 | 🟡 | 定期从 main 重建 |
| bug 修复需 hotfix 到 release 分支 | 🟡 | cherry-pick 标准流程 |

---

## 8. 结论

**当前发布流程（本地 bash release.sh）在多 agent 并发下不可靠**，因为它假设"干净工作树 + 唯一发布者"。

**推荐方案**：**方案 A+B+D 组合** —— 从 `release/` 分支发，CI（release.yml）是唯一真实发布入口（干净快照 + 独占 workflow），本地 release.sh 只做 prep/演练。

**最小改动启动**：阶段 1（版本锁）+ 阶段 2（release.sh prep-only）即可让现有多 agent 环境下的发布"不再互相打架"，约 1.5 天。