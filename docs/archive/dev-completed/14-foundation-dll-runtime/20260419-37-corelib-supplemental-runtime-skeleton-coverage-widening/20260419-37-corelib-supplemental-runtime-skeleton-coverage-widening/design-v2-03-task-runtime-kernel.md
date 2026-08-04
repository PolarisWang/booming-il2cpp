# CoreLib Task Runtime Kernel Design v2.03

Date: 2026-04-26 23:08:00 +08:00
Status: draft-approved-for-execution

## Goal

Freeze the `TaskContinuationFamily` architecture as a hybrid runtime model that:

- keeps hotupdate binding on a stable task runtime kernel surface
- allows native-first optimization for task continuation/state hot paths
- avoids a destructive rewrite of the entire TPL runtime in one step

## Final Shape

The chosen model is:

- `Hybrid Task Runtime`

with:

- `TaskRuntimeKernelV1`
- `TaskContinuationFamily`
- task-specific native fast paths for kernel-owned hotspots
- managed fallback inside the same family for the long tail

This is intentionally not:

- pure managed-invoke-only
- pure full-native TPL rewrite

## Kernel Ownership

`TaskRuntimeKernelV1` is the stable hotupdate-facing root.

It owns capability declaration for:

- `state-core`
- `continuation`
- `wait`
- `factory`
- `introspection`

Patch code must not depend on task object internal field layout directly.

## Family Strategy

`TaskContinuationFamily` is split logically into:

- `TaskStateCoreLane`
- `TaskContinuationLane`
- `TaskWaitLane`
- `TaskFactoryLane`
- `TaskIntrospectionLane`

But codegen is allowed to land these lanes incrementally under one family as long as:

- fast paths remain kernel-owned
- long-tail APIs stay inside family fallback
- no legacy special-case growth returns

## First Fast Path

The first kernelized task method is:

- `System.Threading.Tasks.Task::NewId:System.Int32()`

This is the first safe `TaskRuntimeKernelV1` proof cut because it:

- is hot enough to justify kernel ownership
- does not require task object graph semantics
- does not leak task internal layout to patch code

## Family Completion Rule

`Task` reaches family-complete when:

- all `System.Threading.Tasks.Task::*` residuals are emitted
- kernel-owned hotspots are routed through `TaskRuntimeKernelV1` or its future sub-helpers
- the remaining long-tail shapes are handled through task-family fallback, not repository-wide ad hoc helpers

## Immediate Next Authority

With `Task` complete, the broader roadmap returns to:

- `Phase 4A: Platform Capability`

The next platform capability target should be selected from the remaining top residuals under phase ordering rather than re-opening task architecture.
