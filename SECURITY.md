# Security Policy

## Reporting a Vulnerability

This is a research/development IL2CPP runtime. Security issues are handled on a best-effort basis.

To report a vulnerability, **do not** open a public issue. Instead:

- Open a **private** issue, or
- Contact the maintainers directly via the git remote owner.

## Supported / Unsupported

- The runtime is under active development; no formal security support guarantees are made.
- Third-party vendored components (`third_party/`) are pinned by the pipeline — see Dependabot/CI for supply-chain notices (e.g. the recent Scriban CVE fix).

## Scope

- The C++ runtime core, GC, interpreter, JIT.
- The managed codegen pipeline (`src/managed/Chaos.IL2CPP.Generator`).
- Test / verification tooling (`testing/`, `tests/`).

## Response

Reproducer + proposed fix are welcome. Maintainers will triage on a best-effort timeline appropriate to a research project.
