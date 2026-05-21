// Auto-generated managed benchmark harness
// Family: native-memory-pointers, Assembly: System.Runtime.InteropServices
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Text.Json;

class ManagedBenchmarkHarness
{
    static volatile int _g;  // volatile side-effect prevents JIT DCE

    struct MethodResult
    {
        public int MethodIndex { get; set; }
        public string MethodSubjectId { get; set; }
        public double ElapsedMilliseconds { get; set; }
        public int Iterations { get; set; }
        public bool IsBodyReal { get; set; }
        public bool IsException { get; set; }
    }



    static void Main()
    {
        var results = new List<MethodResult>();
            { // [0] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::AddRef:System.Int32(System.IntPtr) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 0,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::AddRef:System.Int32(System.IntPtr)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [1] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::AllocCoTaskMem:System.IntPtr(System.Int32) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 1,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::AllocCoTaskMem:System.IntPtr(System.Int32)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [2] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::AllocHGlobal:System.IntPtr(System.Int32) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 2,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::AllocHGlobal:System.IntPtr(System.Int32)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [3] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::AllocHGlobal:System.IntPtr(System.IntPtr) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 3,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::AllocHGlobal:System.IntPtr(System.IntPtr)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [4] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::AreComObjectsAvailableForCleanup:System.Boolean() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 4,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::AreComObjectsAvailableForCleanup:System.Boolean()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [5] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::BindToMoniker:System.Object(System.String) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 5,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::BindToMoniker:System.Object(System.String)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [6] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::ChangeWrapperHandleStrength:System.Void(System.Object,System.Boolean) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 6,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::ChangeWrapperHandleStrength:System.Void(System.Object,System.Boolean)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [7] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::CleanupUnusedObjectsInCurrentContext:System.Void() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 7,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::CleanupUnusedObjectsInCurrentContext:System.Void()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [8] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::Copy:System.Void(System.Int32{},System.Int32,System.IntPtr,System.Int32) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 8,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::Copy:System.Void(System.Int32{},System.Int32,System.IntPtr,System.Int32)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [9] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::Copy:System.Void(System.Char{},System.Int32,System.IntPtr,System.Int32) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 9,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::Copy:System.Void(System.Char{},System.Int32,System.IntPtr,System.Int32)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [10] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::Copy:System.Void(System.Int16{},System.Int32,System.IntPtr,System.Int32) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 10,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::Copy:System.Void(System.Int16{},System.Int32,System.IntPtr,System.Int32)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [11] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::Copy:System.Void(System.Int64{},System.Int32,System.IntPtr,System.Int32) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 11,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::Copy:System.Void(System.Int64{},System.Int32,System.IntPtr,System.Int32)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [12] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::Copy:System.Void(System.Single{},System.Int32,System.IntPtr,System.Int32) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 12,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::Copy:System.Void(System.Single{},System.Int32,System.IntPtr,System.Int32)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [13] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::Copy:System.Void(System.Double{},System.Int32,System.IntPtr,System.Int32) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 13,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::Copy:System.Void(System.Double{},System.Int32,System.IntPtr,System.Int32)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [14] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::Copy:System.Void(System.Byte{},System.Int32,System.IntPtr,System.Int32) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 14,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::Copy:System.Void(System.Byte{},System.Int32,System.IntPtr,System.Int32)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [15] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::Copy:System.Void(System.IntPtr{},System.Int32,System.IntPtr,System.Int32) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 15,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::Copy:System.Void(System.IntPtr{},System.Int32,System.IntPtr,System.Int32)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [16] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::Copy:System.Void(System.IntPtr,System.Int32{},System.Int32,System.Int32) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 16,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::Copy:System.Void(System.IntPtr,System.Int32{},System.Int32,System.Int32)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [17] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::Copy:System.Void(System.IntPtr,System.Char{},System.Int32,System.Int32) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 17,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::Copy:System.Void(System.IntPtr,System.Char{},System.Int32,System.Int32)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [18] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::Copy:System.Void(System.IntPtr,System.Int16{},System.Int32,System.Int32) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 18,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::Copy:System.Void(System.IntPtr,System.Int16{},System.Int32,System.Int32)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [19] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::Copy:System.Void(System.IntPtr,System.Int64{},System.Int32,System.Int32) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 19,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::Copy:System.Void(System.IntPtr,System.Int64{},System.Int32,System.Int32)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [20] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::Copy:System.Void(System.IntPtr,System.Single{},System.Int32,System.Int32) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 20,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::Copy:System.Void(System.IntPtr,System.Single{},System.Int32,System.Int32)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [21] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::Copy:System.Void(System.IntPtr,System.Double{},System.Int32,System.Int32) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 21,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::Copy:System.Void(System.IntPtr,System.Double{},System.Int32,System.Int32)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [22] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::Copy:System.Void(System.IntPtr,System.Byte{},System.Int32,System.Int32) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 22,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::Copy:System.Void(System.IntPtr,System.Byte{},System.Int32,System.Int32)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [23] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::Copy:System.Void(System.IntPtr,System.IntPtr{},System.Int32,System.Int32) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 23,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::Copy:System.Void(System.IntPtr,System.IntPtr{},System.Int32,System.Int32)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [24] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::CreateAggregatedObject:System.IntPtr(System.IntPtr,System.Object) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 24,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::CreateAggregatedObject:System.IntPtr(System.IntPtr,System.Object)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [25] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::CreateAggregatedObject{T}:System.IntPtr(System.IntPtr,T) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 25,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::CreateAggregatedObject{T}:System.IntPtr(System.IntPtr,T)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [26] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::CreateWrapperOfType:System.Object(System.Object,System.Type) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 26,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::CreateWrapperOfType:System.Object(System.Object,System.Type)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [27] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::CreateWrapperOfType{T,TWrapper}:TWrapper(T) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 27,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::CreateWrapperOfType{T,TWrapper}:TWrapper(T)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [28] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::DestroyStructure:System.Void(System.IntPtr,System.Type) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 28,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::DestroyStructure:System.Void(System.IntPtr,System.Type)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [29] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::DestroyStructure{T}:System.Void(System.IntPtr) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 29,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::DestroyStructure{T}:System.Void(System.IntPtr)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [30] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::FinalReleaseComObject:System.Int32(System.Object) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 30,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::FinalReleaseComObject:System.Int32(System.Object)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [31] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::FreeBSTR:System.Void(System.IntPtr) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 31,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::FreeBSTR:System.Void(System.IntPtr)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [32] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::FreeCoTaskMem:System.Void(System.IntPtr) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 32,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::FreeCoTaskMem:System.Void(System.IntPtr)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [33] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::FreeHGlobal:System.Void(System.IntPtr) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 33,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::FreeHGlobal:System.Void(System.IntPtr)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [34] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::GenerateGuidForType:System.Guid(System.Type) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 34,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::GenerateGuidForType:System.Guid(System.Type)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [35] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::GenerateProgIdForType:System.String(System.Type) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 35,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::GenerateProgIdForType:System.String(System.Type)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [36] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::GetComInterfaceForObject:System.IntPtr(System.Object,System.Type) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 36,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::GetComInterfaceForObject:System.IntPtr(System.Object,System.Type)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [37] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::GetComInterfaceForObject{T,TInterface}:System.IntPtr(T) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 37,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::GetComInterfaceForObject{T,TInterface}:System.IntPtr(T)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [38] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::GetComInterfaceForObject:System.IntPtr(System.Object,System.Type,System.Runtime.InteropServices.CustomQueryInterfaceMode) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 38,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::GetComInterfaceForObject:System.IntPtr(System.Object,System.Type,System.Runtime.InteropServices.CustomQueryInterfaceMode)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [39] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::GetComObjectData:System.Object(System.Object,System.Object) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 39,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::GetComObjectData:System.Object(System.Object,System.Object)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [40] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::GetDelegateForFunctionPointer:System.Delegate(System.IntPtr,System.Type) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 40,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::GetDelegateForFunctionPointer:System.Delegate(System.IntPtr,System.Type)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [41] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::GetDelegateForFunctionPointer{TDelegate}:TDelegate(System.IntPtr) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 41,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::GetDelegateForFunctionPointer{TDelegate}:TDelegate(System.IntPtr)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [42] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::GetEndComSlot:System.Int32(System.Type) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 42,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::GetEndComSlot:System.Int32(System.Type)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [43] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::GetExceptionCode:System.Int32() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 43,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::GetExceptionCode:System.Int32()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [44] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::GetExceptionForHR:System.Exception(System.Int32) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 44,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::GetExceptionForHR:System.Exception(System.Int32)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [45] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::GetExceptionForHR:System.Exception(System.Int32,System.IntPtr) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 45,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::GetExceptionForHR:System.Exception(System.Int32,System.IntPtr)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [46] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::GetExceptionForHR:System.Exception(System.Int32,System.Guid&,System.IntPtr) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 46,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::GetExceptionForHR:System.Exception(System.Int32,System.Guid&,System.IntPtr)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [47] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::GetExceptionPointers:System.IntPtr() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 47,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::GetExceptionPointers:System.IntPtr()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [48] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::GetFunctionPointerForDelegate:System.IntPtr(System.Delegate) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 48,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::GetFunctionPointerForDelegate:System.IntPtr(System.Delegate)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [49] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::GetFunctionPointerForDelegate{TDelegate}:System.IntPtr(TDelegate) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 49,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::GetFunctionPointerForDelegate{TDelegate}:System.IntPtr(TDelegate)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [50] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::GetHINSTANCE:System.IntPtr(System.Reflection.Module) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 50,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::GetHINSTANCE:System.IntPtr(System.Reflection.Module)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [51] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::GetHRForException:System.Int32(System.Exception) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 51,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::GetHRForException:System.Int32(System.Exception)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [52] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::GetHRForLastWin32Error:System.Int32() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 52,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::GetHRForLastWin32Error:System.Int32()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [53] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::GetIDispatchForObject:System.IntPtr(System.Object) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 53,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::GetIDispatchForObject:System.IntPtr(System.Object)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [54] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::GetIUnknownForObject:System.IntPtr(System.Object) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 54,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::GetIUnknownForObject:System.IntPtr(System.Object)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [55] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::GetLastPInvokeError:System.Int32() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 55,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::GetLastPInvokeError:System.Int32()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [56] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::GetLastPInvokeErrorMessage:System.String() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 56,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::GetLastPInvokeErrorMessage:System.String()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [57] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::GetLastSystemError:System.Int32() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 57,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::GetLastSystemError:System.Int32()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [58] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::GetLastWin32Error:System.Int32() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 58,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::GetLastWin32Error:System.Int32()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [59] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::GetNativeVariantForObject:System.Void(System.Object,System.IntPtr) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 59,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::GetNativeVariantForObject:System.Void(System.Object,System.IntPtr)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [60] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::GetNativeVariantForObject{T}:System.Void(T,System.IntPtr) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 60,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::GetNativeVariantForObject{T}:System.Void(T,System.IntPtr)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [61] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::GetObjectForIUnknown:System.Object(System.IntPtr) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 61,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::GetObjectForIUnknown:System.Object(System.IntPtr)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [62] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::GetObjectForNativeVariant:System.Object(System.IntPtr) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 62,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::GetObjectForNativeVariant:System.Object(System.IntPtr)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [63] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::GetObjectForNativeVariant{T}:T(System.IntPtr) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 63,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::GetObjectForNativeVariant{T}:T(System.IntPtr)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [64] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::GetObjectsForNativeVariants:System.Object{}(System.IntPtr,System.Int32) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 64,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::GetObjectsForNativeVariants:System.Object{}(System.IntPtr,System.Int32)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [65] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::GetObjectsForNativeVariants{T}:T{}(System.IntPtr,System.Int32) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 65,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::GetObjectsForNativeVariants{T}:T{}(System.IntPtr,System.Int32)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [66] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::GetPInvokeErrorMessage:System.String(System.Int32) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 66,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::GetPInvokeErrorMessage:System.String(System.Int32)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [67] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::GetStartComSlot:System.Int32(System.Type) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 67,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::GetStartComSlot:System.Int32(System.Type)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [68] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::GetTypedObjectForIUnknown:System.Object(System.IntPtr,System.Type) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 68,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::GetTypedObjectForIUnknown:System.Object(System.IntPtr,System.Type)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [69] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::GetTypeFromCLSID:System.Type(System.Guid) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 69,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::GetTypeFromCLSID:System.Type(System.Guid)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [70] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::GetTypeInfoName:System.String(System.Runtime.InteropServices.ComTypes.ITypeInfo) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 70,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::GetTypeInfoName:System.String(System.Runtime.InteropServices.ComTypes.ITypeInfo)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [71] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::GetUniqueObjectForIUnknown:System.Object(System.IntPtr) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 71,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::GetUniqueObjectForIUnknown:System.Object(System.IntPtr)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [72] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::InitHandle:System.Void(System.Runtime.InteropServices.SafeHandle,System.IntPtr) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 72,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::InitHandle:System.Void(System.Runtime.InteropServices.SafeHandle,System.IntPtr)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [73] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::IsComObject:System.Boolean(System.Object) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 73,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::IsComObject:System.Boolean(System.Object)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [74] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::IsTypeVisibleFromCom:System.Boolean(System.Type) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 74,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::IsTypeVisibleFromCom:System.Boolean(System.Type)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [75] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::OffsetOf:System.IntPtr(System.Type,System.String) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 75,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::OffsetOf:System.IntPtr(System.Type,System.String)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [76] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::OffsetOf{T}:System.IntPtr(System.String) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 76,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::OffsetOf{T}:System.IntPtr(System.String)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [77] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::Prelink:System.Void(System.Reflection.MethodInfo) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 77,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::Prelink:System.Void(System.Reflection.MethodInfo)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [78] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::PrelinkAll:System.Void(System.Type) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 78,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::PrelinkAll:System.Void(System.Type)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [79] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::PtrToStringAnsi:System.String(System.IntPtr) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 79,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::PtrToStringAnsi:System.String(System.IntPtr)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [80] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::PtrToStringAnsi:System.String(System.IntPtr,System.Int32) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 80,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::PtrToStringAnsi:System.String(System.IntPtr,System.Int32)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [81] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::PtrToStringAuto:System.String(System.IntPtr,System.Int32) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 81,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::PtrToStringAuto:System.String(System.IntPtr,System.Int32)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [82] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::PtrToStringAuto:System.String(System.IntPtr) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 82,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::PtrToStringAuto:System.String(System.IntPtr)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [83] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::PtrToStringBSTR:System.String(System.IntPtr) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 83,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::PtrToStringBSTR:System.String(System.IntPtr)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [84] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::PtrToStringUni:System.String(System.IntPtr) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 84,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::PtrToStringUni:System.String(System.IntPtr)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [85] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::PtrToStringUni:System.String(System.IntPtr,System.Int32) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 85,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::PtrToStringUni:System.String(System.IntPtr,System.Int32)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [86] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::PtrToStringUTF8:System.String(System.IntPtr) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 86,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::PtrToStringUTF8:System.String(System.IntPtr)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [87] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::PtrToStringUTF8:System.String(System.IntPtr,System.Int32) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 87,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::PtrToStringUTF8:System.String(System.IntPtr,System.Int32)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [88] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::PtrToStructure:System.Object(System.IntPtr,System.Type) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 88,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::PtrToStructure:System.Object(System.IntPtr,System.Type)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [89] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::PtrToStructure:System.Void(System.IntPtr,System.Object) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 89,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::PtrToStructure:System.Void(System.IntPtr,System.Object)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [90] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::PtrToStructure{T}:System.Void(System.IntPtr,T) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 90,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::PtrToStructure{T}:System.Void(System.IntPtr,T)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [91] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::PtrToStructure{T}:T(System.IntPtr) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 91,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::PtrToStructure{T}:T(System.IntPtr)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [92] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::QueryInterface:System.Int32(System.IntPtr,System.Guid&,System.IntPtr&) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 92,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::QueryInterface:System.Int32(System.IntPtr,System.Guid&,System.IntPtr&)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [93] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::ReadByte:System.Byte(System.Object,System.Int32) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 93,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::ReadByte:System.Byte(System.Object,System.Int32)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [94] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::ReadByte:System.Byte(System.IntPtr,System.Int32) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 94,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::ReadByte:System.Byte(System.IntPtr,System.Int32)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [95] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::ReadByte:System.Byte(System.IntPtr) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 95,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::ReadByte:System.Byte(System.IntPtr)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [96] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::ReadInt16:System.Int16(System.Object,System.Int32) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 96,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::ReadInt16:System.Int16(System.Object,System.Int32)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [97] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::ReadInt16:System.Int16(System.IntPtr,System.Int32) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 97,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::ReadInt16:System.Int16(System.IntPtr,System.Int32)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [98] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::ReadInt16:System.Int16(System.IntPtr) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 98,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::ReadInt16:System.Int16(System.IntPtr)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [99] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::ReadInt32:System.Int32(System.Object,System.Int32) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 99,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::ReadInt32:System.Int32(System.Object,System.Int32)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [100] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::ReadInt32:System.Int32(System.IntPtr,System.Int32) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 100,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::ReadInt32:System.Int32(System.IntPtr,System.Int32)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [101] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::ReadInt32:System.Int32(System.IntPtr) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 101,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::ReadInt32:System.Int32(System.IntPtr)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [102] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::ReadInt64:System.Int64(System.Object,System.Int32) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 102,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::ReadInt64:System.Int64(System.Object,System.Int32)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [103] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::ReadInt64:System.Int64(System.IntPtr,System.Int32) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 103,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::ReadInt64:System.Int64(System.IntPtr,System.Int32)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [104] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::ReadInt64:System.Int64(System.IntPtr) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 104,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::ReadInt64:System.Int64(System.IntPtr)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [105] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::ReadIntPtr:System.IntPtr(System.Object,System.Int32) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 105,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::ReadIntPtr:System.IntPtr(System.Object,System.Int32)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [106] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::ReadIntPtr:System.IntPtr(System.IntPtr,System.Int32) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 106,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::ReadIntPtr:System.IntPtr(System.IntPtr,System.Int32)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [107] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::ReadIntPtr:System.IntPtr(System.IntPtr) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 107,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::ReadIntPtr:System.IntPtr(System.IntPtr)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [108] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::ReAllocCoTaskMem:System.IntPtr(System.IntPtr,System.Int32) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 108,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::ReAllocCoTaskMem:System.IntPtr(System.IntPtr,System.Int32)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [109] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::ReAllocHGlobal:System.IntPtr(System.IntPtr,System.IntPtr) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 109,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::ReAllocHGlobal:System.IntPtr(System.IntPtr,System.IntPtr)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [110] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::Release:System.Int32(System.IntPtr) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 110,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::Release:System.Int32(System.IntPtr)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [111] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::ReleaseComObject:System.Int32(System.Object) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 111,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::ReleaseComObject:System.Int32(System.Object)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [112] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::SecureStringToBSTR:System.IntPtr(System.Security.SecureString) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 112,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::SecureStringToBSTR:System.IntPtr(System.Security.SecureString)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [113] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::SecureStringToCoTaskMemAnsi:System.IntPtr(System.Security.SecureString) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 113,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::SecureStringToCoTaskMemAnsi:System.IntPtr(System.Security.SecureString)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [114] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::SecureStringToCoTaskMemUnicode:System.IntPtr(System.Security.SecureString) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 114,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::SecureStringToCoTaskMemUnicode:System.IntPtr(System.Security.SecureString)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [115] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::SecureStringToGlobalAllocAnsi:System.IntPtr(System.Security.SecureString) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 115,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::SecureStringToGlobalAllocAnsi:System.IntPtr(System.Security.SecureString)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [116] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::SecureStringToGlobalAllocUnicode:System.IntPtr(System.Security.SecureString) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 116,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::SecureStringToGlobalAllocUnicode:System.IntPtr(System.Security.SecureString)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [117] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::SetComObjectData:System.Boolean(System.Object,System.Object,System.Object) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 117,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::SetComObjectData:System.Boolean(System.Object,System.Object,System.Object)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [118] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::SetLastPInvokeError:System.Void(System.Int32) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 118,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::SetLastPInvokeError:System.Void(System.Int32)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [119] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::SetLastSystemError:System.Void(System.Int32) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 119,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::SetLastSystemError:System.Void(System.Int32)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [120] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::SizeOf:System.Int32(System.Object) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 120,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::SizeOf:System.Int32(System.Object)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [121] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::SizeOf{T}:System.Int32(T) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 121,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::SizeOf{T}:System.Int32(T)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [122] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::SizeOf:System.Int32(System.Type) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 122,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::SizeOf:System.Int32(System.Type)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [123] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::SizeOf{T}:System.Int32() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 123,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::SizeOf{T}:System.Int32()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [124] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::StringToBSTR:System.IntPtr(System.String) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 124,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::StringToBSTR:System.IntPtr(System.String)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [125] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::StringToCoTaskMemAnsi:System.IntPtr(System.String) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 125,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::StringToCoTaskMemAnsi:System.IntPtr(System.String)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [126] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::StringToCoTaskMemAuto:System.IntPtr(System.String) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 126,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::StringToCoTaskMemAuto:System.IntPtr(System.String)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [127] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::StringToCoTaskMemUni:System.IntPtr(System.String) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 127,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::StringToCoTaskMemUni:System.IntPtr(System.String)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [128] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::StringToCoTaskMemUTF8:System.IntPtr(System.String) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 128,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::StringToCoTaskMemUTF8:System.IntPtr(System.String)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [129] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::StringToHGlobalAnsi:System.IntPtr(System.String) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 129,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::StringToHGlobalAnsi:System.IntPtr(System.String)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [130] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::StringToHGlobalAuto:System.IntPtr(System.String) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 130,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::StringToHGlobalAuto:System.IntPtr(System.String)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [131] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::StringToHGlobalUni:System.IntPtr(System.String) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 131,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::StringToHGlobalUni:System.IntPtr(System.String)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [132] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::StructureToPtr:System.Void(System.Object,System.IntPtr,System.Boolean) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 132,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::StructureToPtr:System.Void(System.Object,System.IntPtr,System.Boolean)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [133] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::StructureToPtr{T}:System.Void(T,System.IntPtr,System.Boolean) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 133,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::StructureToPtr{T}:System.Void(T,System.IntPtr,System.Boolean)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [134] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::ThrowExceptionForHR:System.Void(System.Int32) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 134,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::ThrowExceptionForHR:System.Void(System.Int32)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [135] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::ThrowExceptionForHR:System.Void(System.Int32,System.IntPtr) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 135,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::ThrowExceptionForHR:System.Void(System.Int32,System.IntPtr)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [136] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::ThrowExceptionForHR:System.Void(System.Int32,System.Guid&,System.IntPtr) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 136,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::ThrowExceptionForHR:System.Void(System.Int32,System.Guid&,System.IntPtr)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [137] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::UnsafeAddrOfPinnedArrayElement:System.IntPtr(System.Array,System.Int32) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 137,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::UnsafeAddrOfPinnedArrayElement:System.IntPtr(System.Array,System.Int32)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [138] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::UnsafeAddrOfPinnedArrayElement{T}:System.IntPtr(T{},System.Int32) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 138,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::UnsafeAddrOfPinnedArrayElement{T}:System.IntPtr(T{},System.Int32)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [139] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::WriteByte:System.Void(System.Object,System.Int32,System.Byte) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 139,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::WriteByte:System.Void(System.Object,System.Int32,System.Byte)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [140] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::WriteByte:System.Void(System.IntPtr,System.Int32,System.Byte) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 140,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::WriteByte:System.Void(System.IntPtr,System.Int32,System.Byte)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [141] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::WriteByte:System.Void(System.IntPtr,System.Byte) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 141,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::WriteByte:System.Void(System.IntPtr,System.Byte)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [142] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::WriteInt16:System.Void(System.Object,System.Int32,System.Int16) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 142,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::WriteInt16:System.Void(System.Object,System.Int32,System.Int16)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [143] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::WriteInt16:System.Void(System.IntPtr,System.Int32,System.Int16) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 143,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::WriteInt16:System.Void(System.IntPtr,System.Int32,System.Int16)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [144] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::WriteInt16:System.Void(System.IntPtr,System.Int16) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 144,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::WriteInt16:System.Void(System.IntPtr,System.Int16)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [145] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::WriteInt16:System.Void(System.IntPtr,System.Int32,System.Char) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 145,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::WriteInt16:System.Void(System.IntPtr,System.Int32,System.Char)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [146] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::WriteInt16:System.Void(System.Object,System.Int32,System.Char) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 146,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::WriteInt16:System.Void(System.Object,System.Int32,System.Char)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [147] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::WriteInt16:System.Void(System.IntPtr,System.Char) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 147,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::WriteInt16:System.Void(System.IntPtr,System.Char)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [148] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::WriteInt32:System.Void(System.Object,System.Int32,System.Int32) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 148,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::WriteInt32:System.Void(System.Object,System.Int32,System.Int32)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [149] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::WriteInt32:System.Void(System.IntPtr,System.Int32,System.Int32) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 149,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::WriteInt32:System.Void(System.IntPtr,System.Int32,System.Int32)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [150] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::WriteInt32:System.Void(System.IntPtr,System.Int32) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 150,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::WriteInt32:System.Void(System.IntPtr,System.Int32)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [151] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::WriteInt64:System.Void(System.Object,System.Int32,System.Int64) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 151,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::WriteInt64:System.Void(System.Object,System.Int32,System.Int64)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [152] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::WriteInt64:System.Void(System.IntPtr,System.Int32,System.Int64) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 152,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::WriteInt64:System.Void(System.IntPtr,System.Int32,System.Int64)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [153] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::WriteInt64:System.Void(System.IntPtr,System.Int64) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 153,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::WriteInt64:System.Void(System.IntPtr,System.Int64)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [154] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::WriteIntPtr:System.Void(System.IntPtr,System.Int32,System.IntPtr) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 154,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::WriteIntPtr:System.Void(System.IntPtr,System.Int32,System.IntPtr)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [155] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::WriteIntPtr:System.Void(System.Object,System.Int32,System.IntPtr) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 155,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::WriteIntPtr:System.Void(System.Object,System.Int32,System.IntPtr)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [156] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::WriteIntPtr:System.Void(System.IntPtr,System.IntPtr) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 156,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::WriteIntPtr:System.Void(System.IntPtr,System.IntPtr)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [157] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::ZeroFreeBSTR:System.Void(System.IntPtr) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 157,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::ZeroFreeBSTR:System.Void(System.IntPtr)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [158] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::ZeroFreeCoTaskMemAnsi:System.Void(System.IntPtr) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 158,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::ZeroFreeCoTaskMemAnsi:System.Void(System.IntPtr)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [159] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::ZeroFreeCoTaskMemUnicode:System.Void(System.IntPtr) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 159,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::ZeroFreeCoTaskMemUnicode:System.Void(System.IntPtr)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [160] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::ZeroFreeCoTaskMemUTF8:System.Void(System.IntPtr) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 160,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::ZeroFreeCoTaskMemUTF8:System.Void(System.IntPtr)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [161] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::ZeroFreeGlobalAllocAnsi:System.Void(System.IntPtr) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 161,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::ZeroFreeGlobalAllocAnsi:System.Void(System.IntPtr)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [162] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::ZeroFreeGlobalAllocUnicode:System.Void(System.IntPtr) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 162,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::ZeroFreeGlobalAllocUnicode:System.Void(System.IntPtr)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [163] System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::Dispose:System.Void() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 163,
                    MethodSubjectId = "System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::Dispose:System.Void()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [164] System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::get_CanRead:System.Boolean() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 164,
                    MethodSubjectId = "System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::get_CanRead:System.Boolean()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [165] System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::get_CanWrite:System.Boolean() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 165,
                    MethodSubjectId = "System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::get_CanWrite:System.Boolean()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [166] System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::get_Capacity:System.Int64() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 166,
                    MethodSubjectId = "System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::get_Capacity:System.Int64()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [167] System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::Read{T}:System.Void(System.Int64,T&) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 167,
                    MethodSubjectId = "System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::Read{T}:System.Void(System.Int64,T&)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [168] System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::ReadArray{T}:System.Int32(System.Int64,T{},System.Int32,System.Int32) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 168,
                    MethodSubjectId = "System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::ReadArray{T}:System.Int32(System.Int64,T{},System.Int32,System.Int32)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [169] System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::ReadBoolean:System.Boolean(System.Int64) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 169,
                    MethodSubjectId = "System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::ReadBoolean:System.Boolean(System.Int64)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [170] System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::ReadByte:System.Byte(System.Int64) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 170,
                    MethodSubjectId = "System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::ReadByte:System.Byte(System.Int64)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [171] System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::ReadChar:System.Char(System.Int64) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 171,
                    MethodSubjectId = "System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::ReadChar:System.Char(System.Int64)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [172] System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::ReadDecimal:System.Decimal(System.Int64) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 172,
                    MethodSubjectId = "System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::ReadDecimal:System.Decimal(System.Int64)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [173] System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::ReadDouble:System.Double(System.Int64) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 173,
                    MethodSubjectId = "System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::ReadDouble:System.Double(System.Int64)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [174] System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::ReadInt16:System.Int16(System.Int64) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 174,
                    MethodSubjectId = "System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::ReadInt16:System.Int16(System.Int64)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [175] System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::ReadInt32:System.Int32(System.Int64) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 175,
                    MethodSubjectId = "System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::ReadInt32:System.Int32(System.Int64)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [176] System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::ReadInt64:System.Int64(System.Int64) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 176,
                    MethodSubjectId = "System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::ReadInt64:System.Int64(System.Int64)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [177] System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::ReadSByte:System.SByte(System.Int64) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 177,
                    MethodSubjectId = "System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::ReadSByte:System.SByte(System.Int64)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [178] System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::ReadSingle:System.Single(System.Int64) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 178,
                    MethodSubjectId = "System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::ReadSingle:System.Single(System.Int64)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [179] System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::ReadUInt16:System.UInt16(System.Int64) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 179,
                    MethodSubjectId = "System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::ReadUInt16:System.UInt16(System.Int64)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [180] System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::ReadUInt32:System.UInt32(System.Int64) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 180,
                    MethodSubjectId = "System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::ReadUInt32:System.UInt32(System.Int64)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [181] System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::ReadUInt64:System.UInt64(System.Int64) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 181,
                    MethodSubjectId = "System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::ReadUInt64:System.UInt64(System.Int64)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [182] System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::Write:System.Void(System.Int64,System.Boolean) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 182,
                    MethodSubjectId = "System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::Write:System.Void(System.Int64,System.Boolean)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [183] System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::Write:System.Void(System.Int64,System.Byte) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 183,
                    MethodSubjectId = "System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::Write:System.Void(System.Int64,System.Byte)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [184] System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::Write:System.Void(System.Int64,System.Char) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 184,
                    MethodSubjectId = "System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::Write:System.Void(System.Int64,System.Char)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [185] System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::Write:System.Void(System.Int64,System.Int16) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 185,
                    MethodSubjectId = "System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::Write:System.Void(System.Int64,System.Int16)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [186] System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::Write:System.Void(System.Int64,System.Int32) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 186,
                    MethodSubjectId = "System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::Write:System.Void(System.Int64,System.Int32)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [187] System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::Write:System.Void(System.Int64,System.Int64) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 187,
                    MethodSubjectId = "System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::Write:System.Void(System.Int64,System.Int64)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [188] System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::Write:System.Void(System.Int64,System.Decimal) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 188,
                    MethodSubjectId = "System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::Write:System.Void(System.Int64,System.Decimal)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [189] System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::Write:System.Void(System.Int64,System.Single) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 189,
                    MethodSubjectId = "System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::Write:System.Void(System.Int64,System.Single)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [190] System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::Write:System.Void(System.Int64,System.Double) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 190,
                    MethodSubjectId = "System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::Write:System.Void(System.Int64,System.Double)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [191] System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::Write:System.Void(System.Int64,System.SByte) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 191,
                    MethodSubjectId = "System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::Write:System.Void(System.Int64,System.SByte)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [192] System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::Write:System.Void(System.Int64,System.UInt16) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 192,
                    MethodSubjectId = "System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::Write:System.Void(System.Int64,System.UInt16)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [193] System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::Write:System.Void(System.Int64,System.UInt32) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 193,
                    MethodSubjectId = "System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::Write:System.Void(System.Int64,System.UInt32)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [194] System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::Write:System.Void(System.Int64,System.UInt64) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 194,
                    MethodSubjectId = "System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::Write:System.Void(System.Int64,System.UInt64)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [195] System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::Write{T}:System.Void(System.Int64,T&) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 195,
                    MethodSubjectId = "System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::Write{T}:System.Void(System.Int64,T&)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [196] System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::WriteArray{T}:System.Void(System.Int64,T{},System.Int32,System.Int32) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 196,
                    MethodSubjectId = "System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::WriteArray{T}:System.Void(System.Int64,T{},System.Int32,System.Int32)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [197] System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::.ctor:System.Void(System.Runtime.InteropServices.SafeBuffer,System.Int64,System.Int64) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 197,
                    MethodSubjectId = "System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::.ctor:System.Void(System.Runtime.InteropServices.SafeBuffer,System.Int64,System.Int64)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [198] System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::.ctor:System.Void(System.Runtime.InteropServices.SafeBuffer,System.Int64,System.Int64,System.IO.FileAccess) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 198,
                    MethodSubjectId = "System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::.ctor:System.Void(System.Runtime.InteropServices.SafeBuffer,System.Int64,System.Int64,System.IO.FileAccess)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
        // Consume accum so JIT cannot elide the computation
        string json = JsonSerializer.Serialize(new { results }, new JsonSerializerOptions { PropertyNamingPolicy = JsonNamingPolicy.CamelCase });
        Console.WriteLine(json);
    }
}
