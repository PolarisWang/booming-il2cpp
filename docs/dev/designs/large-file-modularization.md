# Large-File Modularization — Recommendation

> Status: **proposed** (not implemented). A deliberate refactor, deferred from
> the 2026-08 repository-hygiene pass because splitting these hot-path files
> carries real regression risk and needs a build/test baseline first.
>
> Related historical analysis: `dev-jit-engine-modularization` (8-module sketch).

## Why deferred

These are **single-coherent-class** files, not accidental bloat. Splitting is a
behavior-preserving refactor that touches the JIT / GC / interpreter hot paths
and must be verified against a passing baseline (unit → integration → e2e).
Doing it inside a hygiene/cleanup pass without a build-verify loop would risk
silent regressions in code generation and memory management.

## Candidates

| File | Lines | Nature | Split recommendation |
|------|-------|--------|---------------------|
| `src/native/jit/jit_engine.cpp` | 6229 | Monolithic `NativeCodeGenerator` | Extract into ~8 modules (register allocation, SEH/unwind, tier promotion, instruction encoding, OSR, etc.) |
| `src/native/runtime-core/gc/gc_old_gen.cpp` | 3433 | Old-gen GC (mark-compact) | Split by sub-responsibility (marking, compaction, card-table, free-list) |
| `src/native/interpreter/ir_reg_alloc.cpp` | 2639 | IR register allocator | Extract allocator passes |
| `src/native/runtime-core/runtime_stubs/enum_stubs.cpp` | 2520 | Repetitive enum stubs (copy-paste) | Generate via macro/template rather than hand-copy (known 118KB copy-paste) |
| `src/managed/.../NativeAotLoweringPlanner.StructuredIR.Emit.cs` | 2447 | Emit cases | Split by IR node family |
| `src/tools/.../FoundationPack.Analysis/Program.cs` | 2400 | Tool main | Low priority (CLI tool) |

## How to run it (when scheduled)

1. Establish a green baseline: `python tests/runner/test_driver.py --layer all` + foundation-dll pipeline.
2. Split one file at a time, **no behavior change**, compile + unit-test after each step.
3. For `enum_stubs.cpp`: prefer codegen/macro generation over manual split.
4. Use `git worktree` to isolate the refactor (per CLAUDE.md 禁止 stash rule).

## Guardrails

- Preserve the `CHAOS_IL2CPP_` allocation/GC macros — do not change allocation domain.
- Keep includes self-contained per extracted module (visit `wiki/06-测试验证` / header conventions).
- Run the pre-commit gate (arch + trace + layer-boundary) after each commit.
