# Contributing

Thanks for contributing to **chaos-il2cpp** — a C++ IL2CPP runtime with NativeAOT codegen, precise generational GC, and hot-update support.

## Development Conventions

**Read [CLAUDE.md](CLAUDE.md) first** — it is the authoritative development rulebook:

- **Architecture-first development**: query the architecture before implementing a new translation path.
- **4-layer boundary** (ATG / CodeGen / TPG / Python): each layer has a strict output boundary; cross-layer writes need `# BOUNDARY_OVERRIDE`.
- **Priority**: P1 (performance) > P2 (design) > P3 (hot-update support).
- **Trace-first debugging**: on failure, use the trace system before adding print/log.
- **Test-first**: after changes, run `python tests/runner/test_driver.py --layer unit`; no-known `[FAIL]` = real regression.

## Getting Started

```bash
cmake --preset debug
cmake --build artifacts/presets/debug --target chaos_runtime_core
dotnet build src/managed/Chaos.IL2CPP.Generator
python tests/runner/test_driver.py --layer unit
```

## Workflow

1. Follow the skill-system routing (CLAUDE.md's classification gate) for the domain you touch.
2. Keep the repository hygienic: don't commit run artifacts (build logs, test stdout, probe output).
3. Run the self-check tooling: `python .ai/skills/tooling/verification/verify_skill_pipeline.py`.
4. The `.githooks/pre-commit` gate (arch + trace + layer-boundary) runs automatically; use `--no-verify` only when justified.

## Commits

- Reference the `root_cause / fix_strategy / regression_check` convention for bug fixes (see CLAUDE.md "AI Agent Bug 修复三规则").
- Keep commits scoped and reviewable.

## Docs

- New translation paths → `wiki/03-功能模块/06-il2cpp核心架构/`.
- Design discussions → `docs/dev/designs/`; completed work → `docs/archive/`.
