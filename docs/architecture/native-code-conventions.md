# Chaos IL2CPP — Native C++ Code Conventions

> **Single source of truth**: `build/architecture.yaml`
> **Enforcement**: `build/check_arch.py` (CLI), CMake targets (`check_arch_<module>`), pre-commit hook

---

## 1. Module Architecture

All native C++ code is organized into modules. Each module has a source directory, a C++ namespace, a visibility level, and declared dependencies.

| Module | Path | Namespace | Visibility |
|--------|------|-----------|------------|
| `chaos_common` | `src/native/common` | `ChaosIl2cpp::Common` | public |
| `chaos_runtime_core` | `src/native/runtime-core` | `chaos::il2cpp::runtime_core` | public |
| `chaos_hot_update` | `src/native/hot-update` | `chaos::il2cpp::hot_update` | internal |
| `chaos_interpreter` | `src/native/interpreter` | `chaos::il2cpp::interpreter` | internal |
| `chaos_support` | `src/native/support` | `chaos::il2cpp::support` | internal |
| `chaos_bootstrap` | `src/native/bootstrap` | `chaos::il2cpp::bootstrap` | internal |
| `chaos_engine_bridge` | `src/native/engine-bridge` | *(anonymous)* | internal |
| `chaos_bdwgc` | `third_party/bdwgc` | *(C library)* | public |

### Dependency Graph

```
chaos_bdwgc       (no dependencies)
chaos_common      (no dependencies)
  └→ chaos_runtime_core     (depends on: chaos_bdwgc)
       ├→ chaos_hot_update  (depends on: chaos_runtime_core)
       ├→ chaos_interpreter (depends on: chaos_runtime_core)
       ├→ chaos_support     (depends on: chaos_runtime_core)
       └→ chaos_bootstrap   (depends on: chaos_runtime_core, chaos_hot_update, chaos_support)
            └→ chaos_engine_bridge (depends on: chaos_runtime_core, chaos_bootstrap)
```

Dependencies are directional and enforced by `check_arch.py`. A module may only `#include` headers from modules it explicitly depends on.

---

## 2. Naming Conventions

| Category | Convention | Example |
|----------|-----------|---------|
| Types (classes, structs, enums) | PascalCase | `FinallyGuard`, `SpanRuntimeEntry` |
| Functions | snake_case | `checked_conv_ovf_i1`, `wrap_add` |
| Constants (constexpr, enum values) | `k_` + snake_case | `k_managed_pointer_local_slot_tag` |
| Include guards | `CHAOS_IL2CPP_{MODULE}_H_` | `CHAOS_IL2CPP_COMMON_H_` |
| Namespaces | Per-module (see Module Architecture) | `ChaosIl2cpp::Common` |

### Exception: `chaos_common`

The `chaos_common` module uses:
- **Namespace**: `ChaosIl2cpp::Common` (not `chaos::il2cpp::common`)
- **Types**: PascalCase — `FinallyGuard`, `ListRuntimeStorage<T>`
- **Functions**: snake_case — `checked_conv_ovf_i1`, `resolve_native_int_slot`
- **Constants**: `k_` + snake_case — `k_managed_pointer_local_slot_tag`

### Exception: `chaos_bdwgc`

The vendored BDWGC library is C code; C++ naming conventions do not apply.

---

## 3. Include Rules

### Allowed Includes

Each module declares `allowed_includes` (public and private) in `build/architecture.yaml`:

```yaml
chaos_runtime_core:
  allowed_includes:
    public:
      - contracts/native/v0/      # any file in this path
    private:
      - src/native/runtime-core/  # own module headers
      - third_party/bdwgc/        # dependency headers
```

### Forbidden Includes

Each module declares `forbidden_includes` — paths that must NOT be included:

```yaml
chaos_runtime_core:
  forbidden_includes:
    - src/native/hot-update/
    - src/native/support/
    - src/native/interpreter/
    - src/native/engine-bridge/
    - src/native/common/
```

### Global Bans

The following are forbidden in ALL native C++ code:

- `using namespace std;`
- `using namespace chaos;`
- `NULL` (use `nullptr` instead)
- Bare `int`/`long` types (use `<cstdint>` types: `std::int32_t`, etc.)
- `#pragma once` (use `#ifndef`/`#define` include guards instead)

---

## 4. Include Guard Format

Every header file must use:

```cpp
#ifndef CHAOS_IL2CPP_{MODULE}_H_
#define CHAOS_IL2CPP_{MODULE}_H_

// ... contents ...

#endif  // CHAOS_IL2CPP_{MODULE}_H_
```

Module prefix mapping:
| Module | Guard Prefix |
|--------|-------------|
| chaos_common | `CHAOS_IL2CPP_COMMON` |
| chaos_runtime_core | `CHAOS_IL2CPP_RUNTIME_CORE` |
| chaos_hot_update | `CHAOS_IL2CPP_HOT_UPDATE` |
| chaos_interpreter | `CHAOS_IL2CPP_INTERPRETER` |
| chaos_support | `CHAOS_IL2CPP_SUPPORT` |
| chaos_bootstrap | `CHAOS_IL2CPP_BOOTSTRAP` |
| chaos_engine_bridge | `CHAOS_IL2CPP_ENGINE_BRIDGE` |

---

## 5. Codegen Validation

Generated C++ code (from `NativeAotEmitter` and `NativeReferenceProofEmitter`) is validated at emission time by `NativeCodegenValidator` (`src/managed/Chaos.IL2CPP.CodeGen/Validation/NativeCodegenValidator.cs`).

Validation rules:
1. No `using namespace std;` or `using namespace chaos;`
2. No `using std::...` or `using chaos::...` directives
3. No `NULL` — use `nullptr`
4. Prefer `<cstdint>` types over bare `int`/`long`
5. Include guard format: `CHAOS_IL2CPP_*_H_`
6. No `chaos_` prefix inside `ChaosIl2cpp::Common` namespace
7. No inline prelude definitions (must use `#include <chaos/common.h>`)

---

## 6. Enforcement Layers

| Layer | Tool | When |
|-------|------|------|
| CLI | `python build/check_arch.py [--module X] [--file Y]` | On demand |
| CLI (CI mode) | `python build/check_arch.py --ci` | Exit code 1 on any issue |
| CMake | `cmake --build <dir> --target check_arch_<module>` | Per-module check |
| CMake (all) | `cmake --build <dir> --target check_arch` | Full project check |
| Pre-commit | `.githooks/pre-commit` | On `git commit` |
| Codegen | `NativeCodegenValidator` | At C# codegen emission time |

---

## 7. When Architecture Changes

1. Edit **`build/architecture.yaml`** — the single source of truth
2. Run `python build/check_arch.py --fix` to auto-fix trivial issues
3. Run `python build/check_arch.py --ci` to validate all modules
4. Update affected CMakeLists.txt files if modules/paths changed
5. Update this document if conventions changed
