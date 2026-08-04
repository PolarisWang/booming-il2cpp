# Exception Handling Coverage Audit

> Generated: 2026-05-17
> Context: P1-EXCEPTION-EQUIV — structured EH shape detection implementation

## Current Status

EH shape detection methods in `ExceptionShapePlanning.cs` previously all returned `false`, meaning **every method with exception regions fell through to flat goto fallback**. As of this commit, the `TryCreate*` methods are implemented.

## EH Pattern Coverage Matrix

| EH Pattern | StructuredIR | FlatGoto | Foundation-DLL Subject | Status |
|---|---|---|---|---|
| try-catch (single) | ✅ Now supported | ✅ | `exception-throw-diagnostics` (no try/catch) | ⚠️ No existing subject with try-catch |
| try-catch (multiple catch blocks) | ❌ Not yet (single catch only) | ✅ | — | Gap: only single-catch shape detected |
| try-finally (single) | ✅ Now supported | ✅ | — | ⚠️ No existing subject |
| try-finally (nested, multi-finally) | ✅ Now supported | ✅ | — | ⚠️ No existing subject |
| try-filter | ✅ Now supported | ✅ | — | Gap: no subject, untested |
| try-catch-finally | ✅ Now supported (catch-and-finally shape) | ✅ | — | ⚠️ No existing subject |
| try-filter-finally | ✅ Now supported | ✅ | — | Gap: no subject, untested |
| nesting try (2 levels) | ✅ Partially (via finally-only or catch+finally) | ✅ | — | ⚠️ No existing subject |
| nesting try (3+ levels) | ⚠️ Untested — may hit flat fallback for non-standard nesting | ✅ | — | Gap: untested |
| nesting finally in catch | ✅ Via catch-and-finally shape | ✅ | — | ⚠️ No existing subject |
| leave in try | ✅ (stripped from try body) | ✅ | — | ⚠️ No existing subject |
| leave in catch | ✅ (stripped from handler body) | ✅ | — | ⚠️ No existing subject |
| leave in finally | ⚠️ Semantic mismatch in C++ finally | ✅ | — | Gap: known design limitation |
| Fault (ECMA fault handler) | ❌ Not supported | ✅ | — | Gap: not handled by any shape |

## Implementation Details

### Shape Detection (ExceptionShapePlanning.cs)

| Method | Matches | Detection Logic |
|---|---|---|
| `TryCreateCatchOnlyExceptionMethodShape` | Single `Catch` region, no nesting | `ExceptionRegions.Count == 1 && kind == Catch` |
| `TryCreateFilterOnlyExceptionMethodShape` | Single `Filter` region, no nesting | `ExceptionRegions.Count == 1 && kind == Filter && FilterOffset != null` |
| `TryCreateFinallyOnlyExceptionMethodShape` | One or more `Finally` regions, no catch/filter mixed in | All regions are `Finally`; sorted by nesting (innermost first) |
| `TryCreateCatchAndFinallyExceptionMethodShape` | Catch region nested inside Finally region(s) | Finds first Catch, verifies it's inside a Finally's try range |
| `TryCreateFilterAndFinallyExceptionMethodShape` | Filter region nested inside Finally region(s) | Finds first Filter with Finally regions present |

### Instruction Partitioning

`PartitionInstructionsByOffset` splits the method's `Instructions` by IL offset ranges:
- `Prefix`: instructions before try block
- `TryBody`: instructions within `[TryOffset, TryOffset + TryLength)`
- `Handler`: instructions within `[HandlerOffset, HandlerOffset + HandlerLength)`
- `Tail`: instructions after handler

Trailing `leave` instructions are stripped from try/handler bodies since structured IR handles control flow via block terminators.

### Known Limitations

1. **Multiple catch blocks**: Only single-catch shapes are detected. Multiple catch blocks on one try require multiple `AotCoreIrExceptionRegionArtifact` entries with the same `TryOffset` — this pattern is not yet handled.
2. **Fault handlers**: The `Fault` EH kind is not consumed by any shape. Methods with only fault handlers fall through to flat goto.
3. **Non-reducible CFG inside EH bodies**: If a partition's control flow graph is not reducible, the structured IR emits empty blocks for that partition. Code inside unrecoverable try/catch/finally bodies is silently dropped.

## Next Steps

1. Add foundation-dll verification subjects for each EH pattern
2. Verify structured EH output works end-to-end for all 5 shapes
3. Add multiple-catch support (next priority)
4. Add Fault handler support (lower priority)
