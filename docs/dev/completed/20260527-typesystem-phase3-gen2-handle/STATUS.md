# Phase 3 — Gen-2 Handle + Cross-DLL VTable

> **task_id**: 20260527-typesystem-phase3-gen2-handle
> **创建日期**: 2026-05-27
> **parent_task_id**: 20260523-typesystem-vtable-industrialization
> **source_relation**: roadmap-child
> **clearance_source**: parent-roadmap
> **lifecycle_status**: completed
> **phase**: T3 — Complete
> **blocking_questions**: []
> **question_clearance**: cleared
> **clearance_confirmed_by_user**: true

## Scope

Gen-2 Handle 运行时泛型 + 跨 DLL vtable 工业化补齐。承接父 roadmap Phase 3 全部 3 项子任务。

| 子任务 | Scope | Status |
|--------|-------|--------|
| T3-1 | 运行时 MakeGenericType（Gen-2 Handle）完整实现验证 + 测试覆盖 | ✅ |
| T3-3 | 跨 DLL ABI 兼容性强化 | ✅ |
| T3-2 | 跨程序集调用走运行时桩 | ✅ |

## Inputs

- 父 roadmap `design-v1-01.md`（已读）
- `generic_context.h/cpp` — GenericContextRegistry（已实现）
- `instantiation/resolve_instantiate.cpp` — ResolveOrInstantiateType/Method（已实现）
- `instantiation/interpret_method_call.cpp` — 泛型方法解释执行（已实现）
- `instantiation/type_descriptor.cpp` — BuildClosedDescriptor（已实现）
- `generic_method_instantiation.h/cpp` — RuntimeInstantiatedMethod（已实现）
- `runtime_instantiation.h` — RuntimeInstantiationBridgeV0 ABI（已定义）
- `abi_manifest.h/cpp` — ABI manifest（已实现）
- `method_table.h` — MethodOrigin + ResolveMethodTableWithAbiCheck（已实现）
- `hotpatch_table.cpp` — HotpatchNameRegistry cross-module lookup（已实现）

## Exit Criteria

- T3-1: 泛型类型/方法实例化测试覆盖关键路径，build + test 通过 ✅
- T3-3: ABI manifest 在跨模块边界被校验 ✅
- T3-2: 跨 DLL 调用走运行时桩（非 vtable_array） ✅

## Execution Summary

### T3-1: Gen-2 Handle 验证+测试 ✅

**Key Actions:**
- Created `testing/src/native/runtime-core/generic_instantiation_test.cpp` with 18 tests covering:
  - RegisterGenericInstantiation / TryResolveClosedType (basic, idempotent, multiple types, arg match/mismatch)
  - RegisterGenericMethodInstantiation / TryResolveClosedMethod
  - GetClosedTypeGenericArgs reverse lookup (found, count-only, truncated, not found)
  - RegisterMethodGenericContext / GetGenericContextForMethod
  - GenericContext accessors (ClassTypeArgCount, ClassTypeArg, MethodTypeArgCount, MethodTypeArg) with null safety
  - GetRegisteredInstantiationCount monotonic
  - Zero/null input safety across all APIs
  - UnregisterModuleGenerics no-op for unregistered modules
  - BridgeV0 non-null contract
- Added test target to `testing/src/native/runtime-core/CMakeLists.txt`
- Build ✅, 18/18 tests pass
- **Result:** GenericContextRegistry implementation verified, test gap closed.

### T3-3: ABI 兼容性强化 ✅

**Key Actions:**
- Verified ABI manifest validation in `RegisterModule()` → calls `ChaosAbiManifestValidate()`
- Verified `CHAOS_ABI_MANIFEST_FAIL_CLOSE` compile switch in both signature check and validate
- Verified `SetMethodOrigin` emitted from Scriban template
- Verified `ResolveMethodTableWithAbiCheck` implemented and tested
- Verified `LookupModuleAbiManifest` for cross-module manifest retrieval
- **Result:** All ABI compatibility APIs implemented, tested, and integrated.

### T3-2: 跨 DLL vtable 桩 ✅

**Key Actions:**
- Verified codegen: `TryGetMethodTableIndex()` detects cross-module calls → allocates method_table index
- Verified `BuildMethodTableInitialization()` generates `WriteMethodTable` + `SetMethodOrigin` code
- Verified interpreter dispatch: `PrecacheCallTarget` resolves `direct_ptr` through `HotpatchNameRegistry.LookupMethod` (covers all modules)
- Verified `InterpreterDispatchRaw` uses `direct_ptr` to bypass `method_invoke` for AOT calls
- **Result:** Cross-DLL dispatch goes through runtime stubs (method_table in codegen, HotpatchNameRegistry-resolved direct_ptr in interpreter).

## Deliverables

- [x] `testing/src/native/runtime-core/generic_instantiation_test.cpp` — 18 tests
- [x] `testing/src/native/runtime-core/CMakeLists.txt` — test target added
- [x] Verification: method_table codegen integration (Scriban + LoweringPlanner)
- [x] Verification: ABI manifest validation in module registration
- [x] Verification: cross-module dispatch through HotpatchNameRegistry
