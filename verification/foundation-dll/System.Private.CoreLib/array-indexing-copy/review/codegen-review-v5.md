# AI Code Review: array-indexing-copy
## Type: codegen
## Date: 2026-05-03T07:02:51.455763+00:00
## Verdict: FAIL

## Summary
- Total issues: 15
- C++ LOC: 332
- C# methods: 1
- C++ functions: 0
- Stub functions: 13
- Uncertain functions: 0
- Bridge calls: 0

## Issues Found
1. NativeReferenceStub_System_Private_CoreLib_System_Array_Copy_System_Void_System_Array_System_Array_System_Int32: returns CHAOS_BRIDGE_STATUS_OK with 0 bridge calls
2. NativeReferenceStub_System_Private_CoreLib_System_Array_Copy_System_Void_System_Array_System_Int32_System_Array_System_Int32_System_Int32: returns CHAOS_BRIDGE_STATUS_OK with 0 bridge calls
3. NativeReferenceStub_System_Private_CoreLib_System_Array_Clear_System_Void_System_Array_System_Int32_System_Int32: returns CHAOS_BRIDGE_STATUS_OK with 0 bridge calls
4. NativeReferenceStub_System_Private_CoreLib_System_Array_Resize_System_Void_T_Ref_System_Int32: returns CHAOS_BRIDGE_STATUS_OK with 0 bridge calls
5. NativeReferenceStub_System_Private_CoreLib_System_Array_Sort_System_Void_System_Array: returns CHAOS_BRIDGE_STATUS_OK with 0 bridge calls
6. NativeReferenceStub_System_Private_CoreLib_System_Array_Sort_System_Void_System_Array_System_Collections_IComparer: returns CHAOS_BRIDGE_STATUS_OK with 0 bridge calls
7. NativeReferenceStub_System_Private_CoreLib_System_Array_BinarySearch_System_Int32_System_Array_System_Object: returns CHAOS_BRIDGE_STATUS_OK with 0 bridge calls
8. NativeReferenceStub_System_Private_CoreLib_System_Array_BinarySearch_System_Int32_System_Array_System_Int32_System_Int32_System_Object: returns CHAOS_BRIDGE_STATUS_OK with 0 bridge calls
9. NativeReferenceStub_System_Private_CoreLib_System_Array_IndexOf_System_Int32_System_Array_System_Object: returns CHAOS_BRIDGE_STATUS_OK with 0 bridge calls
10. NativeReferenceStub_System_Private_CoreLib_System_Array_LastIndexOf_System_Int32_System_Array_System_Object: returns CHAOS_BRIDGE_STATUS_OK with 0 bridge calls
11. NativeReferenceStub_System_Private_CoreLib_System_Array_Reverse_System_Void_System_Array: returns CHAOS_BRIDGE_STATUS_OK with 0 bridge calls
12. NativeReferenceStub_System_Private_CoreLib_System_Array_GetLength_System_Int32_System_Int32: returns CHAOS_BRIDGE_STATUS_OK with 0 bridge calls
13. NativeReferenceStub_System_Private_CoreLib_System_Array_GetValue_System_Object_System_Int32: returns CHAOS_BRIDGE_STATUS_OK with 0 bridge calls
14. No matching C++ function found for C# method 't'
15. No exception handling paths found (try/catch or CHAOS_IL2CPP_RAISE macros)

## Check Results
- stub_detection: FAIL
- signature_alignment: FAIL
- return_type_handling: PASS
- null_handling: PASS
- exception_handling: FAIL