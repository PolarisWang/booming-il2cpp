# Changelog

All notable changes are tracked via git **commit history** (`git log`) and the
detailed design/decision records in `wiki/` and `docs/dev/`. This file is a
high-level summary for cross-cutting milestones.

> **Note**: Per the repo's workflow, per-feature detail lives in the task's
> STATUS.md / design docs. This changelog captures only cross-cutting totals.

## [Unreleased]

### 发布流程（Product-Grade Release Pipeline，2026-08-31）

- **`scripts/release.sh`**：产品级主编排脚本。整合 release_bump + release 分支 + release notes + SDK 构建 + SHA256 校验和 + CycloneDX SBOM + `gh release` 创建/上传 + 合并回 main。支持 `--dry-run` / `--publish` / `--no-push`。
- **`scripts/generate-release-notes.sh`**：从 `git log` 按 conventional-commit 前缀自动分类（breaking/feature/fix/perf/CI/etc.），按域（GC/JIT/CodeGen/HotUpdate）分组，输出 Markdown。
- **`scripts/generate-checksums.sh`**：可移植 Python 实现，生成标准 SHA256SUMS。
- **`scripts/generate-sbom.sh`**：生成 CycloneDX 1.5 SBOM（记录每个产物的 SHA256、体积、路径与构建 commit）。
- **`.github/workflows/release.yml`**：推送 `v*` 语义化 tag 时自动构建多平台 SDK + 生成校验和/SBOM + 创建 GitHub Release。
- **`.github/dependabot.yml`**：NuGet + GitHub Actions 依赖每周自动更新。
- **`.github/workflows/codeql.yml`**：每周 C++/C#/Python 安全静态分析。
- **`PublishController.cs`**：`toolVersion` 从编译程序集 `InformationalVersion` 动态解析（消除硬编码 `0.1.0` 漂移）。
- **`.githooks/pre-push`**：`main`/`release/*` 分支上 L1 深审严重问题改为 blocking（发布质量门禁），其他分支保持 advisory。
- **发布 v0.1.1**：完整跑通 `release.sh 0.1.1 --publish` — release/0.1.x 分支、v0.1.1 tag、release notes、SDK 构建、校验和、SBOM 全部生成并推送。

### Repository hygiene (2026-08)

- Removed ~22 GB of stale agent worktrees (branch commits archived to `refs/archive/`).
- Stopped tracking root run artifacts (build/test logs, probe output) + tightened `.gitignore`.
- Enabled the `.githooks/pre-commit` gate (arch compliance + trace + layer boundary).
- Removed obsolete/isolated batch and migration scripts.
- Skill system: unified `.ai/skills/` paths, repaired self-check tooling, catalog/manifests, expert routing, dual-taxonomy mapping.

### GC — 三代链 / BGC 死锁修复 / Server 接线 / NGC2（plan-v5 里程碑，2026-08）

- **Config 体系（M11）**：`CHAOS_GC_CONFIGURATION_KEYS` 扩至 24 旋钮（调度自适应、BGC、旧代阈值），3 个死旋钮（MaxTlabAlloc/LohThreshold/ParallelMarkWorkers）接线到真实热路径；config 早读归零整族 bug 修复。
- **GCHandle（M12）**：内部类型 REFCOUNTED + WEAK_INTERIOR_POINTER（对齐 CoreCLR HNDTYPE），新增 4 个 C-API。
- **GC 事件（M13）**：ETW fire 覆盖测试接入。
- **OOM 预算（M15）**：`GcGetOomReportBudget()` 从硬编码改 gen 级缩放（config 驱动）。
- **三代分代（M9/M10/M8）**：gen1 独立 generation 标（`kRegionGenGen1`）、gen>condemned 过滤实激活、plan-gen 重绑验证。
- **Region demotion（M7）**：demotion region-gen 验证 + age-based evacuation + handle relocation 正确性（域卸载不碎片）。
- **BGC 两快照（M5）**：remark clear-as-scan；**修复 BGC–YoungGC 暂停死锁**（`bgc_race_test` 完成率 ~0%→~75%）。
- **Server GC 接线（M3A-1/3）**：`GcHeapManager::Initialize()` 接线 + `GC_SERVER` CMake 默认修正 OFF + bare `g_old_gen` 改走 `G_OldGen()` accessor 路由。
- **M6**：frag→gen 预算伺服验证。**M4/M3B**：NGC2 queue（mandated gen2 collection）。

## [2026-06] — Foundation-DLL verification & skill system

- foundation-dll pipeline: 45-chunk build sweep; fact/benchmark/verification fixes.
- Skill system: expert-registry v2 (single authority), 6-round governance review.
- GC: Server GC / thread hijacking / coordinated pause; BGC–YoungGC interaction fix.

## [2026-05] — Hot-update & JIT baseline

- Hot-update: PatchLoader, LCAC, non-CoreLib build.
- JIT: VEH throttle, baseline infra, MSVC portability.

## [2026-04] — corelib / codegen

- NativeAOT codegen: external-runtime helper ABI normalization, value-type/typdef guards.
- Corelib certification substrate (archived to `docs/archive/dev-completed/`).

---

Prior history: see `git log` and `docs/archive/` (frozen completed-work records).
