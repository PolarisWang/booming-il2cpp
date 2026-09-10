# R1.4 — GetElementType 完整实现

> **task_id**: r1.4-getelementtype
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

完整性实现 `ChaosTypeGetElementType`，支持 nested array（int[][]）、managed pointer（int&）和 by-ref 类型。当前实现在 `type_stubs.cpp` 中有 descriptor-based 扫描路径但有限制。

## Inputs

- 文件：`type_stubs.cpp` — ChaosTypeGetElementType 当前实现（195-241 行）
- 现有：ModuleRegistry Tier 0 type_flags 中的 kFlagIsArray/kFlagIsPointer/kFlagIsByRef

## Expected Outputs

- `type_stubs.cpp` — ChaosTypeGetElementType 完整实现
- 构建验证通过

## Exit Criteria

- nested array 可解析（int[] → int, int[][] → int[]）
- managed pointer/ByRef 可解析
- 非数组/指针类型返回 0
- chaos_runtime_core 构建通过

## Terminal Notes

- Implementation completed at `type_stubs.cpp:212-329`.
- Two parallel paths:
  - **Path 1 (ModuleRegistry TypeRef)**: Uses type_names + type_flags for fast detection. Checks kFlagIsByRef/kFlagIsPointer flags first (higher priority), then "[]" suffix for arrays. Strips suffix and searches across all modules' type_names for the element type. Returns MakeTypeHandle(mod_id, token) on match.
  - **Path 2 (Descriptor/ReflectionQuery)**: Uses subject_id_utf8 to detect suffixes ("&" > "*" > "[]"). Strips suffix and searches module image->types arrays, falls back to aot_metadata::kAllTypes static descriptors. Returns EncodeReflectionQueryTypeHandle on match.
- Two internal helpers (lambdas):
  - `findDescBySubjectId`: scans all module images + static AOT metadata for matching subject_id
  - `findTypeRefByName`: scans all ModuleRegistry type_names for matching type name
- Nested array (int[][] → int[]) handled correctly: strip one "[]" → "System.Int32[]" → found as registered array type.
- ByRef (int& → int) and Pointer (int* → int) handled by stripping "&"/"*" suffix.
- Non-array/pointer/byref types return 0.
- Build: chaos_runtime_core.lib passes (RelWithDebInfo, no new warnings).
