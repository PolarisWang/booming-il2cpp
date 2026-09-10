# CoreLib LowLevelMemoryFamily Execution Plan v2.01

Date: 2026-04-26 10:35:00 +08:00
Status: ready-for-execution

## Goal

Execute Phase 1 `LowLevelMemoryFamily` as the next canonical CoreLib runtime-skeleton closure step.

Scope:

- `System.Runtime.CompilerServices.Unsafe`
- `System.MemoryExtensions`
- `System.SpanHelpers`
- `System.Array`
- `System.Runtime.InteropServices.MemoryMarshal`

## Batches

### Batch 1: `UnsafeManagedInvoke`

Goal:

- introduce a managed-invoke family for the wrapper-heavy `Unsafe` residual surface

Exit criteria:

- `Unsafe` wrapper-heavy residuals are emitted through a dedicated helper ABI
- split-file governance tests and template tests are green
- no legacy fallback is reintroduced

### Batch 2: `MemoryExtensionsManagedInvoke`

Goal:

- cover the public wrapper-heavy `MemoryExtensions` surface with the same managed-invoke authority model

### Batch 3: `SpanHelpersKernel`

Goal:

- handle low-level span kernels via explicit kernel-family support

### Batch 4: `ArrayAndMemoryMarshal`

Goal:

- finish Phase 1 by expanding array and low-level marshal helpers

## Validation

- `python -m pytest tests/unit/compatibility/test_native_runtime_core.py -q`
- `python -m pytest tests/unit/compatibility/test_il2cpp_codegen_structure_governance_reference.py -q`
- `python -m pytest tests/unit/compatibility/test_full_assembly_closure_codegen_contracts_runtime_templates.py -q`
- `python -m pytest tests/unit/compatibility/test_full_assembly_closure_codegen_contracts_string_templates_basic.py -q`
- `dotnet build src/managed/Chaos.IL2CPP.CodeGen/Chaos.IL2CPP.CodeGen.csproj -v:minimal`
- `python build/toolchains/run/run.py test subject --id subject/SolutionCorePack --matrix windows-corelib-reference-native-hotupdate-proof --json`

## Recommended next action

Start `Batch 1: UnsafeManagedInvoke`.
