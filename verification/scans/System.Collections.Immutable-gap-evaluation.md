# Phase A Gap Evaluation — System.Collections.Immutable

## Overview

- Assembly: System.Collections.Immutable
- Total methods: 2,236 (2,147 with body)
- Unique opcodes scanned: 130
- Emitter coverage: 126/130 = **96.9%**
- Methods affected by missing opcodes: **105 methods** across 10 families

## Gap 1: `constrained.` (prefix)

**Status: MISSING in emitter** — No `case "constrained"` in the switch. The emitter handles `callvirt` directly but does not consume the `constrained.` prefix that precedes `call`/`callvirt` for calls on generic type parameters.

**Usage:** ~85 methods in Immutable families (FrozenDictionary, FrozenSet, ImmutableHashSet, ImmutableList, ImmutableSortedSet, ImmutableSortedDictionary, ImmutableDictionary, etc.)

**Affected families:** frozen-collections (bulk), immutable-hash-set (bulk), immutable-list (bulk), immutable-sorted-set, immutable-sorted-dictionary, immutable-dictionary, immutable-interlocked, immutable-queue-stack

### Solution A: Emit direct call with type handle (Recommended)
Resolve `constrained.` by emitting `call` with the runtime type handle as an extra argument, calling the interface method directly via the vtable.
| Dimension | Score | Notes |
|-----------|-------|-------|
| Performance | 9/10 | Direct vtable dispatch, no boxing |
| Architecture | 9/10 | Standard pattern for constrained calls, matches runtime behavior |
| HotUpdate | 8/10 | Call target can be redirected |
| Risk | 7/10 | Needs correct runtime type handle retrieval |
| Code Elegance | 9/10 | Clean prefix-slot pattern |

### Solution B: Fall back to `callvirt` with boxing
For `constrained.` + `callvirt` on unknown type params, box the value and use regular virtual call.
| Dimension | Score | Notes |
|-----------|-------|-------|
| Performance | 3/10 | Heap allocation per call destroys performance |
| Architecture | 7/10 | Simple to implement |
| HotUpdate | 6/10 | Boxing complicates patching |
| Risk | 8/10 | Well-understood fallback pattern |
| Code Elegance | 8/10 | Single fallthrough handler |

### Solution C: Constrained prefix state machine
Add a prefix state to the emitter — when `constrained.` loads a type token, the next `callvirt` resolves the vtable lookup via that type instead of the object's runtime type.
| Dimension | Score | Notes |
|-----------|-------|-------|
| Performance | 8/10 | One extra register load vs Solution A |
| Architecture | 8/10 | Prefix state machine is clean IL pattern |
| HotUpdate | 8/10 | Same as Solution A |
| Risk | 6/10 | State machine complicates emitter |
| Code Elegance | 6/10 | Adds mutable state to IL walker |

## Gap 2: `readonly.` (prefix)

**Status: MISSING in emitter** — The `readonly.` prefix is used before `ldflda` to indicate the resulting ref/byref is readonly (immutable struct field). The emitter processes `ldflda` but does not check for the prefix.

**Usage:** ~20 methods (ImmutableArray::ItemRef, FrozenDictionary::GetValueRefOrNullRefCore, etc.)

**Affected families:** immutable-array, frozen-collections

### Solution A: Ignore prefix (Recommended)
The `readonly.` prefix is a verification hint — the emitter can ignore it since the generated C++ already handles correct const semantics through type information.
| Dimension | Score | Notes |
|-----------|-------|-------|
| Performance | 10/10 | No codegen change |
| Architecture | 9/10 | Prefix is informational at codegen level |
| HotUpdate | 10/10 | No change to patch flow |
| Risk | 9/10 | Withfield/verification don't depend on it |
| Code Elegance | 10/10 | Zero additional code |

### Solution B: Emit `const` qualifier on the ref
Track the prefix and emit a `const`-qualified ref type in the C++.
| Dimension | Score | Notes |
|-----------|-------|-------|
| Performance | 10/10 | No runtime cost |
| Architecture | 8/10 | More sematically correct |
| HotUpdate | 10/10 | No change |
| Risk | 7/10 | Type system integration risk |
| Code Elegance | 7/10 | Prefix state tracking needed |

## Gap 3: `ldelem.i4`

**Status: MISSING in emitter** — The emitter has `case "ldelem"` and `case "ldelem.ref"` but NOT `case "ldelem.i4"` or other typed element loads.

**Usage:** ~20 methods (FrozenDictionary::GetValueRefOrNullRefCore, FrozenSet::FindItemIndex, FrozenHashTable, etc.)

**Affected families:** frozen-collections (bulk)

### Solution A: Map typed `ldelem.*` to `ldelem` (Recommended)
Map `ldelem.i4`, `ldelem.i1`, `ldelem.i2`, `ldelem.i8`, `ldelem.r4`, `ldelem.r8`, `ldelem.u1`, `ldelem.u2`, `ldelem.u4` to the existing `case "ldelem"` handler. The typed variants push the same element pointer after bounds check — the ABI is identical.
| Dimension | Score | Notes |
|-----------|-------|-------|
| Performance | 10/10 | No change in codegen |
| Architecture | 10/10 | Typed variants are redundant at C++ level |
| HotUpdate | 9/10 | No change to patch flow |
| Risk | 9/10 | One-line case alias |
| Code Elegance | 10/10 | `case "ldelem.i4": case "ldelem.i1": ... goto case "ldelem"` |

### Solution B: Emit typed element load with cast
Generate typed C++ code that asserts element type at runtime and does the typed load.
| Dimension | Score | Notes |
|-----------|-------|-------|
| Performance | 5/10 | Runtime type checks |
| Architecture | 6/10 | Unnecessary in statically-typed C++ |
| HotUpdate | 7/10 | Extra checks complicate patching |
| Risk | 6/10 | Runtime type check dependency |
| Code Elegance | 5/10 | Duplicate per-type handlers |

## Gap 4: `stelem.i4`

**Status: MISSING in emitter** — The emitter has `case "stelem"` and `case "stelem.ref"` but NOT `case "stelem.i4"` or other typed element stores. This is the write counterpart to Gap 3.

**Usage:** 1 method (FrozenHashTable::Create)

**Affected families:** frozen-collections

### Solution A: Map typed `stelem.*` to `stelem` (Recommended)
Same approach as Gap 3: alias all typed `stelem.*` variants to `case "stelem"`.
| Dimension | Score | Notes |
|-----------|-------|-------|
| Performance | 10/10 | No codegen change |
| Architecture | 10/10 | Typed variants redundant in C++ |
| HotUpdate | 10/10 | No change to patch |
| Risk | 9/10 | One-line change |
| Code Elegance | 10/10 | Same as Gap 3 |

### Solution B: Emit type-checked store
Additional runtime type verification for the stored value.
| Dimension | Score | Notes |
|-----------|-------|-------|
| Performance | 5/10 | Extra checks |
| Architecture | 5/10 | Redundant with C++ type safety |
| HotUpdate | 6/10 | More complex patch |
| Risk | 6/10 | Extra runtime failure mode |
| Code Elegance | 5/10 | Duplicated per-type handlers |

## Summary

| Gap | Affected Families | Methods | Recommended Solution | Effort |
|-----|-------------------|---------|---------------------|--------|
| `constrained.` | 8 families | ~85 | A — Direct call with type handle | ~2-3 hours |
| `readonly.` | 2 families | ~20 | A — Ignore prefix | ~0 hours |
| `ldelem.i4` | 1 family | ~20 | A — Map to `ldelem` | ~0.5 hour |
| `stelem.i4` | 1 family | 1 | A — Map to `stelem` | ~0.5 hour |
| **Total** | **10 families** | **~105** | All Solution A | **~3-4 hours** |

After these 4 fixes, coverage reaches **100%** — all 130 scanned opcodes handled. Only `constrained.` requires non-trivial emitter work; `readonly.`, `ldelem.i4`, and `stelem.i4` are trivial.
