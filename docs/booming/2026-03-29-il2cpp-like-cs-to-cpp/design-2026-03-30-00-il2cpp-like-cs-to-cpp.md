# IL2CPP-Like C# to C++ System Design

**Date:** 2026-03-30 00:16:31 +08:00

**Status:** Draft approved in conversation, pending user review

## Goal

Design a system that translates C#/.NET projects into C++ in a way that is broadly comparable to Unity IL2CPP, while following a phased roadmap:

- Phase 1 prioritizes engineering deliverability
- Performance is the next major priority
- The long-term target approaches IL2CPP-like breadth and robustness

The design assumes:

- IL/assembly-first canonical pipeline
- Fully self-built runtime
- Cross-platform targets from the beginning
- Mainline support for libraries, console apps, CLI tools, background jobs, general server programs, and native-host embedding
- Layered support for dynamic features rather than requiring everything in phase 1

## Non-Goals

The following are not part of the phase-1 mainline commitment:

- `dynamic`
- `System.Reflection.Emit`
- `Assembly.Load`-style dynamic late loading
- Full expression-tree compilation
- Framework-heavy UI stacks such as WPF, WinForms, MAUI
- Full-stack ASP.NET-style framework completeness

These may be added later, but they are not used to define the phase-1 architecture boundary.

## Architecture Overview

The system is split into two primary planes:

1. Translation pipeline
2. Runtime platform

Primary flow:

`source -> dotnet build/csc -> IL assemblies + metadata -> semantic/AOT analysis -> runtime-contract IR -> C++ emission -> native build -> executable/library/bundle`

Core subsystems:

1. `Frontend Gateway`
2. `Assembly and Metadata Loader`
3. `Semantic and AOT Analyzer`
4. `IR Layer`
5. `C++ Backend`
6. `Runtime Core`
7. `Interop and Platform Abstraction`
8. `Build and Packaging`

Core architectural rule:

- High-level managed semantics must converge into a unified IR and runtime contract before platform-specific lowering.

## Frontend Boundary

The canonical system boundary is IL/assembly-first.

This means:

- Standard C# compilation remains the front door
- The system consumes compiled assemblies (`.dll`, `.exe`) plus metadata and symbols
- The translator behaves as an IL AOT backend rather than as a replacement C# language compiler

Source-aware support is still useful, but only as an auxiliary layer for:

- diagnostics
- source mapping
- debug metadata enhancement
- developer experience improvements

Source-aware logic is not treated as a co-equal translation frontend.

## Runtime Model

The runtime is divided into three internal layers:

1. Object and memory model
2. Execution semantics layer
3. Metadata and reflection layer

### Object and Memory Model

All managed heap objects share a stable `ObjectHeader`.

The header should minimally support:

- `TypeInfo*`
- GC state/marking bits
- synchronization/monitor-related state
- optional cached identity/hash state

Arrays, strings, and normal objects share the same root object semantics, with payload-specific layout behind a runtime-defined contract.

Generated C++ may not assume raw memory layouts directly. It must always operate through runtime-defined object access rules.

### GC Direction

The architecture should be designed for a future moving collector even if phase 1 uses a simpler precise collector.

This means generated code must already depend on:

- explicit root tracking
- handle registration where needed
- write barriers
- safepoints
- runtime-owned object/slot access helpers

That keeps future GC upgrades possible without invalidating the generated-code ABI.

### Execution Semantics Layer

This layer owns:

- exception creation, propagation, and native unwinding bridges
- thread registration and detach
- TLS integration
- safepoints and stop-the-world coordination
- virtual dispatch
- interface dispatch
- generic runtime support
- managed/native transition protocols

### Metadata and Reflection Layer

Reflection metadata must be a first-class runtime model rather than an incidental codegen byproduct.

The runtime should maintain formal structures for:

- `TypeInfo`
- `MethodInfo`
- `FieldInfo`
- `PropertyInfo`
- `EventInfo`
- `GenericContext`
- assembly/module identity

This is necessary not only for reflection, but also for:

- exception formatting
- serialization
- diagnostics
- runtime generic resolution

Core rule:

- Generated code expresses business behavior; runtime contracts own managed semantics.

## Translation Pipeline and IR

Use three representation layers between IL input and C++ output.

### 1. Metadata Graph

Purpose:

- define the semantic world model of the loaded assemblies

Responsibilities:

- assemblies and modules
- type definitions and references
- inheritance and interface relationships
- field and method signatures
- attributes
- generic parameters and constraints
- resources and symbol attachment

This layer avoids repeated raw assembly parsing across later stages.

### 2. Typed IL IR

Purpose:

- transform method bodies into explicit control/data-flow suitable for analysis

Properties:

- stack semantics are normalized away
- locals and temporaries are explicit
- control flow graph is explicit
- exception regions are explicit
- call sites and dispatch kinds are explicit
- boxing/unboxing/array operations become explicit nodes

This is the main analysis layer for:

- call graph
- virtual closure
- interface closure
- generic instantiation discovery
- reflection preservation logic
- serialization preservation logic

### 3. Runtime-Contract IR

Purpose:

- represent managed semantic actions in a backend-stable form independent of original IL shape

Examples of operations at this layer:

- object allocation
- array allocation
- dispatch operations
- metadata lookup
- type checks and casts
- write barriers
- exception boundary operations
- GC/handle transitions
- P/Invoke transitions

The C++ backend should target this IR rather than target IL directly.

### AOT Analysis Loop

The analyzer should materialize an explicit closed-world result:

`roots -> call graph -> virtual/interface closure -> generic instantiations -> reflection preservation -> serialization preservation -> native export set`

The output must be recorded in an explicit `AOT manifest`.

The manifest is used for:

- reproducibility
- incremental rebuild support
- diagnostics
- codegen boundary stability
- regression comparison

Core rule:

- analysis outputs must be durable artifacts, not hidden side effects inside code generation.

## C++ Backend and ABI Strategy

The backend is split into:

1. Portable emission layer
2. Platform ABI adaptation layer

### Portable Emission Layer

Responsibilities:

- emit stable, uniform C++ for runtime-contract IR
- avoid compiler-specific semantics where possible
- preserve useful debug structure
- keep code generation predictable for optimization and diagnosis

### Platform ABI Adaptation Layer

Responsibilities:

- calling conventions
- symbol visibility/export rules
- exception interop edges
- thread-local storage differences
- atomics and memory model details
- compiler-family quirks

This layer isolates platform/compiler differences from the rest of the backend.

### Type and Layout Strategy

Managed semantics take precedence over native idioms.

The runtime owns the canonical representation for:

- reference types
- value types
- boxed values
- strings
- arrays
- delegates
- generic instances

Generated C++ targets these contracts rather than creating ad hoc per-platform layouts.

### Interop Boundaries

Three boundaries must be designed explicitly:

1. `Managed -> Native`
2. `Native -> Managed`
3. `Generated C++ -> Runtime Core`

The third one is the most important internal ABI and must be versioned and stabilized early.

### Output Forms

The system should support:

- standalone native executable
- static or dynamic library
- embeddable runtime bundle

This is required to support both server-style deployment and native-host embedding.

Core rule:

- stabilize the internal ABI first, then adapt to external ABIs.

## Interop and Platform Abstraction

Because the target is cross-platform from phase 1, platform abstraction must exist from the start.

The abstraction layer should cover:

- threads
- mutexes/condition variables
- filesystem
- time and timers
- process/environment access
- TLS
- atomics
- sockets/network primitives where phase-1 runtime needs them
- loader and symbol resolution utilities

The goal is not to hide every platform difference, but to prevent platform assumptions from leaking into the translation pipeline and runtime semantics model.

## Build and Packaging

The build driver should orchestrate:

- project compilation to assemblies
- translator invocation
- runtime source selection
- generated C++ compilation
- platform-specific native linking
- symbol/debug file generation
- packaging into executable, library, or bundle form

Suggested outputs:

- translated sources manifest
- AOT manifest
- generated CMake/native build description
- final packaged binary or library
- symbol/mapping artifacts

The build system should treat reproducibility as a first-class concern.

## Phase Roadmap

### Phase 1 - MVP

Goal:

- translate, compile, and run real projects in the selected mainline scope

In scope:

- class libraries
- console apps
- CLI tools
- background jobs
- general server programs
- native-host embedding

Required capabilities:

- stable assembly loading and metadata model
- typed IL IR and runtime-contract IR
- root/call-graph/generic/reflection preservation loop
- self-built runtime core
- precise GC baseline
- exception support
- thread registration and safepoints
- reflection foundation
- generics baseline
- P/Invoke baseline
- cross-platform native builds

Not required in phase 1:

- `dynamic`
- `Emit`
- `Assembly.Load`
- full expression-tree compilation
- heavyweight framework completeness

### Phase 2 - Enhanced Platform

Goal:

- increase semantic depth, project success rate, diagnosability, and performance

Typical work:

- stronger reflection preservation
- better generic sharing/specialization strategy
- incremental manifests and caching
- compatibility diagnostics and reports
- more robust host embedding surface
- better metadata and binary size reduction
- stronger optimization passes and devirtualization opportunities

### Phase 3 - Near-IL2CPP Capability

Goal:

- approach platform-product quality and breadth

Typical work:

- deeper dynamic feature support
- stronger debug/symbol experience
- more advanced GC/perf tuning
- broader framework adapters
- improved startup time, throughput, and binary-size optimization

Phase rule:

- every phase is judged by real project success rate, not by a feature checklist alone.

## Verification Strategy

Validation must be layered from the start.

### 1. Pipeline Correctness

Verify consistency between:

- input assemblies
- metadata graph
- typed IL IR
- AOT manifest
- generated C++
- produced native artifact

Purpose:

- catch drift between analysis and codegen

### 2. Runtime Semantic Tests

Dedicated runtime conformance tests for:

- object model
- GC behavior and safepoints
- exceptions
- reflection queries
- generic runtime behavior
- P/Invoke transitions
- thread attach/detach behavior

### 3. Golden Project Suite

Maintain a set of real representative .NET projects covering:

- libraries
- CLI tools
- background jobs
- server programs
- embedding scenarios

Success criteria must include:

- translates successfully
- compiles successfully
- runs successfully
- behaves acceptably versus expected results

### 4. Cross-Platform Conformance

Run representative projects across the supported platform matrix to verify:

- ABI correctness
- exception behavior
- threading behavior
- filesystem and encoding behavior
- time/timer behavior
- compiler-family compatibility

Core rule:

- validate the full chain before optimizing individual pieces.

## Major Risks

Top-level risks:

1. Generic and reflection closure explosion
2. GC/thread/exception coupling destabilizing runtime behavior
3. Platform ABI differences leaking into generated code
4. Premature pressure to support heavyweight frameworks outside phase-1 scope

Risk-control approach:

- keep explicit AOT manifests
- stabilize runtime contracts before aggressive optimization
- isolate ABI adaptation
- enforce strict phase boundaries
- measure real-project success continuously

## Phase Exit Criteria

Phase completion should require representative project success, not module completion.

Example phase-1 exit criteria:

- at least one representative library suite translates and executes correctly
- at least one CLI/background-job sample works end to end
- at least one native-host embedding scenario succeeds
- at least one multi-platform subset of the matrix passes core semantic validation on more than one platform

Core rule:

- enforce phase exit criteria before expanding scope.

## Open Follow-On Planning Areas

This design is sufficient to begin implementation planning, but the next planning pass should break the work into concrete execution tracks such as:

- metadata loader and world model
- IL normalization and typed IR
- AOT closure engine and manifest format
- runtime object model and precise GC baseline
- exception/thread/safepoint protocol
- reflection metadata runtime
- portable C++ emitter
- ABI adaptation layer
- packaging/build driver
- golden-project and semantic validation suites

## Review Notes

This document was written from the approved brainstorming record.

Not completed in this session:

- Sub-agent specification review loop, because no explicit user authorization for delegation was given in this session
- Git commit of the design artifacts, because the current workspace root is not a git repository
