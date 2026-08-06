# Changelog

All notable changes are tracked via git **commit history** (`git log`) and the
detailed design/decision records in `wiki/` and `docs/dev/`. This file is a
high-level summary for cross-cutting milestones.

> **Note**: Per the repo's workflow, per-feature detail lives in the task's
> STATUS.md / design docs. This changelog captures only cross-cutting totals.

## [Unreleased]

### Repository hygiene (2026-08)

- Removed ~22 GB of stale agent worktrees (branch commits archived to `refs/archive/`).
- Stopped tracking root run artifacts (build/test logs, probe output) + tightened `.gitignore`.
- Enabled the `.githooks/pre-commit` gate (arch compliance + trace + layer boundary).
- Removed obsolete/isolated batch and migration scripts.
- Skill system: unified `.ai/skills/` paths, repaired self-check tooling, catalog/manifests, expert routing, dual-taxonomy mapping.

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
