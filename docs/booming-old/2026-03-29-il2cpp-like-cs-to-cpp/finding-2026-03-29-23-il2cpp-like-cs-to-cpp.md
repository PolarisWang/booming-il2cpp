# IL2CPP-Like C# to C++ System Findings

Date: 2026-03-29 23:37:57 +08:00
Status: active

## Repository Findings

- Current workspace contains process/skill documentation only.
- No existing translator, runtime, or compiler codebase was found to constrain the architecture.

## Design Findings

- The request implies a platform-scale system rather than a single compiler stage.
- A faithful IL2CPP-like solution will require both a translation pipeline and a managed runtime support layer.
- User prefers a phased program: full target architecture plus staged execution roadmap, not a single-step production-equivalent delivery plan.
- User prefers general .NET project coverage over a Unity-only subset. This pushes the architecture toward stronger BCL/runtime fidelity and away from a narrowly optimized game-script pipeline.
- User chose layered dynamic-feature support: first-class architecture for Reflection, exceptions, generics, P/Invoke, and common serialization; defer dynamic runtime code generation and late-bound assembly loading to later phases.
- User confirmed priority ordering: engineering deliverability first, performance second. Semantic completeness remains important, but early architecture should optimize for “can translate, compile, and run real projects” before deeper optimization work.
- User chose an initial cross-platform matrix. This means early architecture cannot assume a single ABI, allocator model, threading primitive set, or native compiler family. Platform abstraction must exist from day one.
- User chose a fully self-built runtime. This increases implementation scope substantially, but it also keeps the long-term architecture independent from Mono/CoreCLR internals and licensing/runtime coupling.

## Q&A Findings

### 2026-03-29 23:51:53 +08:00 - Difference between source-first and IL-first frontends

**Question:** What is the difference between supporting source input and IL input?

**Answer summary:**

- **IL-first** means the system consumes compiled assemblies (`.dll` / `.exe`) and CIL metadata after the normal C# compiler has already run.
- **Source-first** means the system consumes C# source code directly from Roslyn syntax/semantic models before CIL exists.

**Practical difference:**

1. **Compiler boundary**
   - IL-first reuses the standard C# compiler as the front door.
   - Source-first must either embed or tightly couple to Roslyn compilation behavior.

2. **Semantic stability**
   - IL-first sees normalized, compiler-resolved semantics.
   - Source-first must track language-version features, compiler lowering, and source-level corner cases itself or through Roslyn.

3. **Fidelity to IL2CPP**
   - IL-first is much closer to Unity IL2CPP, which fundamentally works from IL/metadata.
   - Source-first is a different product shape, more like a language compiler than an IL AOT backend.

4. **Optimization visibility**
   - Source-first can preserve richer source constructs and developer intent earlier.
   - IL-first loses some source-form information, but gains a simpler and more uniform intermediate contract.

5. **Tooling compatibility**
   - IL-first naturally supports any frontend that can produce valid .NET assemblies, including multiple C# project shapes.
   - Source-first is more tightly coupled to C# source compilation and project-system behavior.

6. **Engineering complexity**
   - IL-first is usually simpler and more robust for a first production architecture.
   - Source-first increases scope because the system must reason about source compilation, analyzer-like semantics, language evolution, and project loading.

**Architecture implication:**

- If the goal is “IL2CPP-like,” IL-first is the cleaner primary path.
- Supporting both is possible, but the most practical design is usually:
  - canonical pipeline = source -> Roslyn/CSC -> IL assemblies -> translator -> C++
  - optional source-aware layer = extra diagnostics / source mapping / richer tooling

### 2026-03-29 23:54:46 +08:00 - Frontend choice confirmed

- User selected IL/assembly-first as the canonical system boundary.
- Source-level support should therefore be treated as an adjunct layer for diagnostics, symbol mapping, and developer experience rather than as an equal translation frontend.

### 2026-03-29 23:56:53 +08:00 - Phase-1 project coverage

- Phase 1 mainline should support:
  - class libraries
  - console applications
  - background jobs / CLI tools
  - general server applications
  - native-host embedding scenarios
- Higher-level framework-heavy app models such as ASP.NET full-stack hosting, desktop UI stacks, and MAUI-like frameworks are not part of the initial mainline commitment.

### 2026-03-30 00:06:35 +08:00 - Visual companion status

- Browser-based visual companion was not used successfully in this session.
- Continue the brainstorming flow in console/text mode.
