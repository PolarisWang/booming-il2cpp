# A-P3-1: G6 Generics/Sharing — Generic Instantiation Codegen Closure

## Status: Partially Complete

## Accomplished

### Build Infrastructure Fixes
- Fixed `native/CMakeLists.txt` for all 3 generics families: corrected codegen output path glob (`*Subjects/generated/` → `generated/`), added `runtime_stubs` include dir, added `enum_stubs.cpp` direct source (pre-built lib missing this symbol)
- Fixed `EnumDispatchEntry` struct redefinition in `EnumMetadataExtractor.cs` (added `#ifndef CHAOS_IL2CPP_GENERATED_CODE_COMPAT_H_` guard)

### Test Family Verification (G6-6, Step 3) ✅
| Family | Fact Result | Status |
|--------|-------------|--------|
| generic-nullable-value | 10/10 | ✅ Pass |
| collections-generic-core | 16/16 | ✅ Pass |
| reflection-generics | 12/12 | ✅ Pass |

Pipeline verification (generic-nullable-value): 12 stages passed, 0 failed.

### Root Cause Fixes
- `EnumMetadataExtractor.cs`: Added `#ifndef` guard around `EnumDispatchEntry` struct definition to prevent redefinition when `generated_code_compat.h` is already included
- `native/CMakeLists.txt`: Fixed codegen output path glob pattern to match actual output location

## Remaining

### G6-1: Cross-module generic AOT entries
- Modify `BuildMethodAotRegistration` in `GenericRegistration.cs` to handle cross-module definitions
- Requires a test family that defines generic types (current families only USE BCL generics)

### G6-2: RuntimeInstantiationBridge
- New file for compile-time unresolvable type parameters
- Depends on G6-1 for testability

### G6-4: Reflection boundary handling
- Helper methods for reflection generic calls
- Depends on G6-1

### G6-5: Structure review
- Code organization and abstraction audit

## Next Steps
1. Create a test family that defines generic types (enables validation of G6 codegen changes)
2. Implement G6-1 cross-module AOT entries
3. Implement G6-2 RuntimeInstantiationBridge
4. Implement G6-4 reflection boundary handling
5. Structure review
