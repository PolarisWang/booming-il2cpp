# Finding Record - IL2CPP Reboot After Abandoned Roadmap

Date: 2026-04-05 20:51:20 +08:00
Status: active

### 2026-04-05 20:51:20 +08:00 - Reboot task initialized after old roadmap abandonment

**User request:**

- Mark the old task `20260329-01-il2cpp-like-cs-to-cpp` as abandoned.
- Re-brainstorm a new plan.
- The new plan must explicitly梳理 the full process of the abandoned plan.

**Execution summary:**

- The old parent roadmap task is being archived as `abandoned`.
- The two derived but not-yet-started child skeletons are also retired as `abandoned`.
- A new reboot planning task is created:
  - `20260405-01-il2cpp-reboot-after-abandoned-roadmap`

**Working interpretation:**

- The old route is not treated as “wrong”.
- It is treated as a large historical asset pack that needs to be:
  - reviewed end to end
  - separated into retained / reopened / abandoned conclusions
  - reorganized into a new proof-first planning spine

**Current direction:**

- The reboot task should prefer a `HelloWorld proof-first` planning entry rather than inheriting the old parent roadmap as the immediate execution mother document.

### 2026-04-05 21:00:00 +08:00 - User selected skeleton-reset first, then HelloWorld proof

**User decision:**

- The reboot sequence should be:
  1. reorganize the execution entry and engineering skeleton
  2. then enter `HelloWorld proof-first`

**Working interpretation:**

- The reset step is not a restart of the old large `Roadmap 0`.
- It is a narrower re-entry pass that should:
  - reuse existing contracts and skeleton assets
  - redefine the current execution entry
  - separate the current smoke `HelloWorld` from the target object-based proof `HelloWorld`

**Design consequence:**

- The reboot mainline now becomes:
  - reset historical assets
  - reset execution entry and skeleton
  - freeze object-based `HelloWorld`
  - prove on a reference platform
  - only then reopen roadmap splitting

### 2026-04-05 21:05:00 +08:00 - User escalated Stage 1 to a heavy contract reset

**User decision:**

- Stage 1 should use the heavy version rather than the light or medium version.

**Working interpretation:**

- Before entering `HelloWorld proof-first`, the reboot should reopen and refreeze the proof-facing contract surfaces, instead of only reorganizing document entry points.
- The reopened surface now tentatively includes:
  - `typed-il-ir`
  - `runtime ABI`
  - `codegen bridge`
  - `AOT manifest`
  - `code registration / metadata registration`

**Guardrail:**

- This is still not a restart of the full old roadmap.
- The reset must stay constrained by one rule:
  - if a contract topic is not required to make the first object-based `HelloWorld` proof executable, it should not enter Stage 1

### 2026-04-05 21:08:00 +08:00 - User approved manifest and registration into Stage 1

**User decision:**

- `AOT manifest`
- `code registration`
- `metadata registration`

should all enter the heavy Stage 1 contract reset.

**Design consequence:**

- Stage 1 is now formally defined as a proof-facing contract pack reset, not just an ABI/IR reset.
- The pack must cover the full minimum proof envelope from:
  - typed IL artifact shape
  - runtime-facing ABI/helper boundary
  - retained-world / manifest expression
  - registration products consumed by bootstrap and generated code

### 2026-04-05 21:20:00 +08:00 - Stage 1 contract pack draft completed

**Execution summary:**

- `Pack A: Proof Envelope` was expanded to explicitly separate:
  - current smoke `HelloWorld`
  - target object-based proof `HelloWorld`
- `Pack B: Frontend Artifact Contract` now freezes the proof-required `typed-il-ir` subset around:
  - `Greeter..ctor`
  - `Greeter.BuildMessage`
  - `Program.Main`
- `Pack C: Runtime Boundary Contract` now narrows the proof-facing split between:
  - low-level `runtime ABI`
  - generated-code-facing `codegen bridge`
- `Pack D: Retained World And Bootstrap Contract` now ties together:
  - `AOT manifest`
  - `code registration`
  - `metadata registration`
  - shared canonical `subjectId`
- `Pack E: Verification And Sample Roles` now separates:
  - smoke
  - proof
  - follow-on samples

**New design addition:**

- A proposed directory structure and matching technical route was added so the reboot plan has a concrete landing shape instead of remaining purely conceptual.

### 2026-04-05 21:28:00 +08:00 - Structure option comparison added for contracts, managed naming, and tests

**Execution summary:**

- Added 3-way option comparison for contract layering.
- Added `Chaos.IL2CPP.*` managed project naming options.
- Re-evaluated the current `tests/` tree against the new proof-first direction and added multiple restructuring options.

**Current recommendation set:**

- contracts: unify into a single `contracts/` root over time
- managed naming: fine-grained `Chaos.IL2CPP.*` layers
- tests: role-oriented split with explicit `contracts / smoke / proof / integration / platform / unit / fixtures`

### 2026-04-05 21:34:00 +08:00 - User confirmed the structure combination

**User decision:**

- contracts: option 2
- managed naming: option 1
- tests: option 2

**Practical conclusion:**

- The reboot design now has a confirmed structural landing shape.
- The next appropriate step is no longer brainstorm comparison, but plan writing against:
  - unified `contracts/`
  - fine-grained `Chaos.IL2CPP.*`
  - role-oriented `tests/`

### 2026-04-05 21:42:00 +08:00 - Stage 1 execution plan created

**Execution summary:**

- Created `docs/dev/in-progress/20260405-01-il2cpp-reboot-after-abandoned-roadmap/plan-v1-01.md`.
- The new plan targets:
  - unified `contracts/`
  - `Chaos.IL2CPP.*` managed skeleton
  - role-oriented `tests/`
  - Stage 1 proof-facing contract reset

**Practical conclusion:**

- This task has now moved from brainstorm convergence into planning readiness.

### 2026-04-05 21:48:00 +08:00 - Phase corrected back to design stage

**Issue observed:**

- A `plan-v1-01.md` had already been drafted, but the task was moved to `planning` too early.
- The current workflow state is more accurately:
  - design artifact exists
  - execution plan exists as a ready draft
  - design checkpoint is not yet formally closed

**Practical conclusion:**

- The task should remain in the design stage until the design checkpoint is considered passed.
- `plan-v1-01.md` remains valid, but should be treated as the next-step execution draft rather than proof that design is already closed.

### 2026-04-05 20:51:20 +08:00 - Abandoned copies created while stale hanging directories remain due file locks

**Environment note:**

- The old task directories were intended to move from `docs/dev/hanging/` to `docs/dev/abandoned/`.
- Windows file locks on currently opened documents prevented a clean directory move/removal in this session.
- As a result:
  - the abandoned copies under `docs/dev/abandoned/` are the authoritative archived copies
  - some stale directories may still remain under `docs/dev/hanging/` until a later cleanup pass

**Practical conclusion:**

- Task state, indexes, and `current_dir` now point to the abandoned copies.
- Residual hanging-path duplicates should be treated as temporary filesystem leftovers rather than active tasks.

### 2026-04-05 22:10:00 +08:00 - Design review closure decisions recorded

**Issues closed:**

- Direct-call landing was not explicit enough for the first proof path.
- `typed-il-ir` and canonical `subjectId` were not explicitly tied together.
- Stage 1 boundary risked collapsing back into a new large roadmap.
- Final tests structure did not explicitly preserve the canonical trace contract location.

**User-aligned decisions:**

- First proof call landing now explicitly uses generated direct-call landing for:
  - `Program.Main`
  - `Greeter..ctor(string)`
  - `Greeter.BuildMessage()`
- `typed-il-ir` should explicitly carry canonical `subjectId`, so Pack B and Pack D share the same primary identity surface.
- Stage 1 keeps the larger option A design boundary; if implementation becomes too large, it may be split into phased execution without shrinking the design target itself.
- `tests/contracts/trace` is the canonical trace contract path; `tests/contract/trace` is compatibility-only.

**Practical conclusion:**

- The design artifact now reflects the main review closures rather than leaving them implicit.
- The remaining next step is a final consistency pass on the updated design before entering plan execution.

### 2026-04-05 22:20:00 +08:00 - Design consistency pass cleared for planning entry

**Execution summary:**

- Re-reviewed the updated design artifact after writing back the 4 review decisions.
- Confirmed the new additions are aligned with:
  - Stage 1 heavy-boundary intent
  - Pack A/B/C/D/E responsibilities
  - role-oriented `tests/` landing

**Practical conclusion:**

- No new blocking design inconsistency was found in this pass.
- The task can now move from design closure into planning / execution entry based on `plan-v1-01.md`.

### 2026-04-05 22:40:00 +08:00 - Parent task promoted to roadmap and Stage 1 split into child plans

**Issue observed:**

- The current Stage 1 execution plan already spans:
  - contract root migration
  - artifact/schema freeze
  - ABI / bridge freeze
  - managed skeleton creation
  - tests restructuring
  - tooling path migration
- That is too large for a single stable execution plan.

**Decision taken:**

- Promote the reboot parent task from a single `plan` entry into a lightweight `roadmap`.
- Split Stage 1 into two child plans:
  - `20260405-02-stage-1a-contract-surface-reset`
  - `20260405-03-stage-1b-structure-and-test-migration`

**Practical conclusion:**

- Parent task now owns stage sequencing and child mapping.
- Active execution moves to Stage 1A first.
- Stage 1B remains created but non-active until Stage 1A has further frozen canonical paths.
