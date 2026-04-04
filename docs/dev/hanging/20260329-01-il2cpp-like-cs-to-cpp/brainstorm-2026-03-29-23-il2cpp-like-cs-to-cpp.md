# IL2CPP-Like C# to C++ System Brainstorm

Date: 2026-03-29 23:37:57 +08:00
Status: in_progress

## Initial Request

User wants a full design for a system that translates C# projects to C++, roughly matching Unity IL2CPP in capability.

## Known Context

- Current workspace does not contain an existing compiler/runtime implementation for this system.
- Treat this as a greenfield architecture/design task.
- The requested target is very large in scope and likely spans multiple subsystems.

## Initial Scope Hypothesis

This is not a single module. It likely includes:

1. C# assembly and metadata loading
2. IL/CIL analysis and reachability trimming
3. AOT strategy for generic code generation and metadata preservation
4. C++ backend and ABI/object model mapping
5. Runtime library for managed semantics
6. GC integration and object lifetime model
7. Reflection, exceptions, interop, and debugging support
8. Native build orchestration and packaging

## Unknowns

- Whether the target is a production-equivalent IL2CPP replacement or a staged MVP
- Whether the primary use case is Unity-like game projects, general .NET apps, or a constrained subset
- Whether compatibility priority is semantic fidelity, performance, portability, or delivery speed

## Discussion Log

- 2026-03-29 23:37:57 +08:00: Initialized brainstorm record.
- 2026-03-29 23:41:02 +08:00: User selected phased roadmap approach. Design should describe the full end-state architecture, but delivery must be staged into MVP, enhanced version, and near-IL2CPP capability levels.
- 2026-03-29 23:42:39 +08:00: User selected general .NET projects as the primary target scope. This increases the importance of runtime completeness, BCL compatibility, reflection behavior, interop, and broad build/deployment support.
- 2026-03-29 23:44:15 +08:00: User selected layered support for dynamic features. Phase 1 should include Reflection, exceptions, generics, P/Invoke, and common serialization in the main architecture, while dynamic, Emit, Assembly.Load, and full expression-tree compilation can be deferred to later phases.
- 2026-03-29 23:46:46 +08:00: User responded with both option 2 and option 3 for priority. Current working interpretation: engineering deliverability is the primary priority, with performance as a strong secondary priority. Needs explicit confirmation because the ranking affects backend, runtime, and optimization strategy.
- 2026-03-29 23:47:33 +08:00: User confirmed the intended ranking: primary priority is engineering deliverability; secondary priority is performance. This favors a design that first guarantees translatability, buildability, and runnable outputs, then layers in aggressive codegen and runtime optimizations.
- 2026-03-29 23:48:29 +08:00: User selected an initial cross-platform matrix rather than a single-platform MVP. Phase 1 architecture must treat platform abstraction, ABI boundaries, native toolchain integration, and runtime portability as first-class concerns across Windows, Linux, macOS, and ARM targets.
- 2026-03-29 23:49:45 +08:00: User selected a fully self-built runtime strategy. The architecture should assume custom ownership of GC, object model, threading semantics, exception machinery, reflection metadata runtime, and platform abstraction rather than leaning on Mono/CoreCLR components.
- 2026-03-29 23:51:53 +08:00: User asked for clarification on the difference between source-first and IL-first frontends before choosing the input model.
- 2026-03-29 23:54:46 +08:00: User selected IL/assembly-first as the canonical frontend. The primary pipeline should be source -> standard C# compilation -> IL assemblies + metadata -> translator -> C++ + runtime integration, with source awareness treated as auxiliary rather than canonical.
- 2026-03-29 23:56:53 +08:00: User selected phase-1 mainline project coverage through general server applications and native-host embedding scenarios, while leaving higher-level UI/application frameworks outside the initial mainline.
- 2026-03-29 23:58:27 +08:00: User selected architecture route 2: layered pipeline plus fully self-built runtime, with optional source-aware capabilities reserved as a later enhancement layer rather than a co-equal translation frontend.
- 2026-03-30 00:06:35 +08:00: Visual companion attempt did not complete successfully. User requested continuing in console mode instead. Architecture presentation will proceed in text form.
- 2026-03-30 00:08:31 +08:00: User approved design segment 1 (overall architecture and subsystem boundaries).
- 2026-03-30 00:11:06 +08:00: User approved design segment 2 (runtime model and semantic core).
- 2026-03-30 00:12:14 +08:00: User approved design segment 3 (translation pipeline, IR layering, and AOT analysis loop).
- 2026-03-30 00:13:34 +08:00: User approved design segment 4 (C++ backend, ABI strategy, and interop boundary model).
- 2026-03-30 00:14:48 +08:00: User approved design segment 5 (phased roadmap and delivery boundaries).
- 2026-03-30 00:16:31 +08:00: User approved design segment 6 (verification, risk control, and phase exit criteria).

## Approved Design Segments

### Segment 1 - Overall Architecture

Approved at: 2026-03-30 00:08:31 +08:00

The system is split into two planes:

- Translation pipeline
- Runtime platform

Primary flow:

`dotnet build / csc` -> `assembly loader` -> `metadata and IL decode` -> `reachability and trimming` -> `AOT specialization` -> `runtime-contract IR` -> `C++ emitter` -> `native build driver` -> `binary / library / embeddable package`

Approved core subsystems:

1. Frontend Gateway
2. Assembly and Metadata Loader
3. Semantic and AOT Analyzer
4. IR Layer
5. C++ Backend
6. Runtime Core
7. Interop and Platform Abstraction
8. Build and Packaging

Key principle:

- High-level semantics should converge into a unified IR and runtime contract before platform-specific lowering.

### Segment 2 - Runtime Model

Approved at: 2026-03-30 00:11:06 +08:00

Runtime is split into three layers:

- object and memory model
- execution semantics layer
- metadata and reflection layer

Approved runtime direction:

- Stable `ObjectHeader` on all managed heap objects
- Unified object semantics for normal objects, arrays, and strings
- Generated C++ must interact with managed objects only through runtime contracts rather than assuming raw layouts
- Architecture should support a future moving GC, even if phase 1 starts with a simpler precise collector and explicit root/handle registration
- Exception flow, thread registration, safepoints, generic runtime support, virtual/interface dispatch, and managed/native boundary transitions all belong in the runtime layer
- Reflection metadata must be a first-class runtime data model, not an incidental codegen byproduct

Key principle:

- Generated code should express business logic; managed semantics should be owned by runtime contracts.

### Segment 3 - Translation Pipeline and IR

Approved at: 2026-03-30 00:12:14 +08:00

Approved translation representation layers:

1. `Metadata Graph`
2. `Typed IL IR`
3. `Runtime-Contract IR`

Responsibilities:

- `Metadata Graph` standardizes assemblies, types, signatures, inheritance, interfaces, generics, attributes, and resources into a single semantic world model.
- `Typed IL IR` transforms method bodies into explicit control/data-flow form suitable for call-graph, generic, reflection, and closure analysis.
- `Runtime-Contract IR` expresses runtime-relevant actions such as allocation, dispatch, metadata queries, write barriers, exception boundaries, and interop transitions independent of source IL shape.

Approved AOT analysis loop:

`roots -> call graph -> virtual/interface closure -> generic instantiations -> reflection preservation -> serialization preservation -> native export set`

Approved artifact requirement:

- Analysis results must be materialized into an explicit `AOT manifest` for reproducibility, incremental work, diagnostics, and stable code generation boundaries.

Key principle:

- Analysis outputs should be explicit artifacts and contracts, not hidden side effects of code generation.

### Segment 4 - C++ Backend and ABI

Approved at: 2026-03-30 00:13:34 +08:00

Approved backend split:

- portable emission layer
- platform ABI adaptation layer

Approved direction:

- Managed semantics take precedence over native compiler idioms.
- Runtime owns canonical type/object representations; generated C++ only targets those contracts.
- Platform-specific differences should be isolated in ABI adaptation, not leak through the whole backend.

Approved interop boundaries:

1. Managed -> Native
2. Native -> Managed
3. Generated C++ -> Runtime Core

Approved artifact forms:

- standalone native executable
- static or dynamic library
- embeddable runtime bundle

Key principle:

- Stabilize the internal ABI first, then adapt to external ABIs.

### Segment 5 - Phased Roadmap

Approved at: 2026-03-30 00:14:48 +08:00

Approved staged delivery:

- Phase 1: MVP that can translate, compile, and run real projects in the selected phase-1 scope
- Phase 2: stronger semantic depth, diagnostics, incremental workflows, and performance engineering
- Phase 3: near-IL2CPP capability, deeper dynamic-feature support, and system-level optimization

Approved scope boundaries:

- Phase 1 includes libraries, console apps, CLI tools, background jobs, general server programs, and native-host embedding
- Phase 1 excludes high-dynamic features such as `dynamic`, `Emit`, `Assembly.Load`, and full expression-tree compilation, along with heavyweight UI/application frameworks

Key principle:

- Each phase must be judged by real-project success rate rather than by a superficial feature checklist.

### Segment 6 - Verification and Risk Control

Approved at: 2026-03-30 00:16:31 +08:00

Approved verification stack:

1. pipeline correctness
2. runtime semantic tests
3. golden project suite
4. cross-platform conformance

Approved top-level risks:

- generic and reflection closure explosion
- GC/thread/exception coupling destabilizing runtime behavior
- platform ABI differences leaking into generated code
- premature support pressure from heavyweight frameworks outside phase-1 scope

Approved phase-exit principle:

- End each phase only when representative real projects in scope can be translated, compiled, executed, and verified successfully across the targeted platforms.

Key principle:

- Validate the full chain before optimizing individual subsystems; enforce phase exit criteria before expanding scope.
