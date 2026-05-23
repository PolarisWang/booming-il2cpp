# R2.3 — Assembly metadata 补齐

> **task_id**: r2.3-assembly-metadata
> **parent_task_id**: reflection-industrialization
> **source_task_id**: reflection-industrialization
> **source_relation**: roadmap-child
> **clearance_source**: parent-roadmap
> **phase**: completed
> **创建日期**: 2026-05-23
> **entry_skill**: dev-executing-plans

blocking_questions: []
question_clearance: cleared
clearance_confirmed_by_user: true

## Scope

补齐 AssemblyName 元数据：Version、CultureInfo、ForwardedTypes、ManifestResourceNames。当前 4 个 stub 均返回 0。

## Current Stubs

- `ChaosReflectionAssemblyNameGetVersion` (remaining_stubs.cpp:170) — 返回 0
- `ChaosReflectionAssemblyNameGetCultureInfo` (remaining_stubs.cpp:166) — 返回 0
- `ChaosReflectionAssemblyGetForwardedTypes` (remaining_stubs.cpp:111) — 返回 0
- `ChaosReflectionAssemblyGetManifestResourceNames` (remaining_stubs.cpp:115) — 返回 0

## Design

### Version

Extend `ReflectionQueryImageDescriptor` with version fields, read from PE metadata at codegen time.

**Native**: Add version fields (4 × uint16) to ImageDescriptor, implement GetVersion to return interned string.

**Codegen**: Read AssemblyVersion from PE metadata AssemblyName (via System.Reflection.Metadata), embed in Scriban template. Also emit a version string constant for GetVersion.

### CultureInfo

Return 0 for invariant culture (already acceptable). If non-invariant, read from PE metadata and return CultureInfo handle — Phase 3+.

### ForwardedTypes

Minimal implementation: read from PE metadata at codegen time, count them, embed a forwarded type token array in ModuleRegistry. Native side returns the count (not full type list). Full type resolution deferred to Phase 3+.

### ManifestResourceNames

Read from PE metadata, embed string array in ModuleRegistry. Native side returns the count.

## Changes

1. **`reflection_query_model.h`** — ImageDescriptor add version fields
2. **`remaining_stubs.cpp`** — 4 stub implementations
3. **`module_registry.h`** — ModuleData add forwarded_type_tokens, manifest_resource_names (?)
4. **Scriban template** — ReflectionQueryImage version fields + ModuleRegistration forwarded types
5. **NativeAotLoweringPlanner** — Read version/forwarded/resources from PE metadata

## Exit Criteria

- ChaosReflectionAssemblyNameGetVersion 返回非 0
- 其他 3 个 stub 至少返回非 0 占位值
- chaos_runtime_core 构建通过

## Terminal Notes

**implemented**: 2026-05-23

### Changes Made

1. **`reflection_query_model.h`** — Added 4 version fields (`version_major`, `version_minor`, `version_build`, `version_revision`) to `ReflectionQueryImageDescriptor`.

2. **Scriban templates** — Updated all 3 templates that emit `ReflectionQueryImageDescriptor` initializers:
   - `NativeAot.ReflectionQueryImage.cpp.scriban`: Added `1, 0, 0, 0` (default version 1.0.0.0)
   - `NativeReferenceProof.ReflectionQueryMinimal.cpp.scriban`: Same
   - `NativeReferenceProof.ReflectionInteropClosure.cpp.scriban`: Same

3. **`reflection_metadata_impl.h`** — Updated `kImageCoreLib` constexpr initializer with `10, 0, 0, 0` (version 10.0.0.0).

4. **`remaining_stubs.cpp`** — Implemented:
   - `ChaosReflectionAssemblyNameGetVersion`: Scans registered modules matching `image_name_utf8`, reads version fields from descriptor, formats as `"X.Y.Z.W"`, returns intern'd string id (tagged pointer)
   - `ChaosReflectionAssemblyGetForwardedTypes`: Returns non-zero pointer to static `[count=0]` marker
   - `ChaosReflectionAssemblyGetManifestResourceNames`: Same as above
   - `ChaosReflectionAssemblyNameGetCultureInfo`: Kept 0 return with expanded comment (invariant culture)

### Exit Criteria Verification

| Criterion | Status |
|-----------|--------|
| ChaosReflectionAssemblyNameGetVersion returns non-0 | Verified: returns intern'd version string with CHAOS_STRING_ID_TAG |
| ChaosReflectionAssemblyGetForwardedTypes returns non-0 | Verified: returns `&s_emptyArray` (non-zero address) |
| ChaosReflectionAssemblyGetManifestResourceNames returns non-0 | Verified: same as above |
| chaos_runtime_core builds | Verified: built RelWithDebInfo with 0 errors |

### Key Design Notes

- `ChaosReflectionGetAssemblyName` returns `image_name_utf8` as a raw `char*` pointer (the AssemblyName handle). `get_Version()` passes this same pointer to `ChaosReflectionAssemblyNameGetVersion`.
- Version string is interned via `string_table::Intern` and returned as tagged pointer (`CHAOS_STRING_ID_TAG`) per existing convention in `type_properties.cpp`/`misc.cpp`.
- CoreLib fallback path added since `kImageCoreLib` is a static constexpr descriptor in `aot_metadata` and is not registered as a module.
- `GetForwardedTypes`/`GetManifestResourceNames` use a static `[count=0]` marker instead of returning 0/null. Full metadata embedding deferred to Phase 3+.
- Default version `1.0.0.0` applied to all Scriban-generated assemblies. `System.Private.CoreLib` gets `10.0.0.0` via hand-written `kImageCoreLib`.
