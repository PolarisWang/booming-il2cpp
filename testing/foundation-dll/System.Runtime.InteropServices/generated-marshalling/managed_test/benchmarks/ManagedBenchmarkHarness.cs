// Auto-generated managed benchmark harness
// Family: generated-marshalling, Assembly: System.Runtime.InteropServices
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
            { // [0] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.ComExposedClassAttribute`1::GetComInterfaceEntries:System.Runtime.InteropServices.ComWrappers+ComInterfaceEntry*(System.Int32&) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 0,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.ComExposedClassAttribute`1::GetComInterfaceEntries:System.Runtime.InteropServices.ComWrappers+ComInterfaceEntry*(System.Int32&)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [1] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.ComExposedClassAttribute`1::.ctor:System.Void() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 1,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.ComExposedClassAttribute`1::.ctor:System.Void()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [2] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.ComInterfaceMarshaller`1::ConvertToManaged:T(System.Void*) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 2,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.ComInterfaceMarshaller`1::ConvertToManaged:T(System.Void*)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [3] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.ComInterfaceMarshaller`1::ConvertToUnmanaged:System.Void*(T) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 3,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.ComInterfaceMarshaller`1::ConvertToUnmanaged:System.Void*(T)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [4] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.ComInterfaceMarshaller`1::Free:System.Void(System.Void*) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 4,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.ComInterfaceMarshaller`1::Free:System.Void(System.Void*)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [5] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.ComObject::FinalRelease:System.Void() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 5,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.ComObject::FinalRelease:System.Void()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [6] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.ComVariantMarshaller::ConvertToManaged:System.Object(System.Runtime.InteropServices.Marshalling.ComVariant) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 6,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.ComVariantMarshaller::ConvertToManaged:System.Object(System.Runtime.InteropServices.Marshalling.ComVariant)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [7] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.ComVariantMarshaller::ConvertToUnmanaged:System.Runtime.InteropServices.Marshalling.ComVariant(System.Object) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 7,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.ComVariantMarshaller::ConvertToUnmanaged:System.Runtime.InteropServices.Marshalling.ComVariant(System.Object)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [8] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.ComVariantMarshaller::Free:System.Void(System.Runtime.InteropServices.Marshalling.ComVariant) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 8,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.ComVariantMarshaller::Free:System.Void(System.Runtime.InteropServices.Marshalling.ComVariant)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [9] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.ComVariantMarshaller+RefPropagate::Free:System.Void() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 9,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.ComVariantMarshaller+RefPropagate::Free:System.Void()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [10] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.ComVariantMarshaller+RefPropagate::FromManaged:System.Void(System.Object) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 10,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.ComVariantMarshaller+RefPropagate::FromManaged:System.Void(System.Object)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [11] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.ComVariantMarshaller+RefPropagate::FromUnmanaged:System.Void(System.Runtime.InteropServices.Marshalling.ComVariant) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 11,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.ComVariantMarshaller+RefPropagate::FromUnmanaged:System.Void(System.Runtime.InteropServices.Marshalling.ComVariant)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [12] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.ComVariantMarshaller+RefPropagate::ToManaged:System.Object() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 12,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.ComVariantMarshaller+RefPropagate::ToManaged:System.Object()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [13] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.ComVariantMarshaller+RefPropagate::ToUnmanaged:System.Runtime.InteropServices.Marshalling.ComVariant() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 13,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.ComVariantMarshaller+RefPropagate::ToUnmanaged:System.Runtime.InteropServices.Marshalling.ComVariant()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [14] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.ExceptionAsDefaultMarshaller`1::ConvertToUnmanaged:T(System.Exception) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 14,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.ExceptionAsDefaultMarshaller`1::ConvertToUnmanaged:T(System.Exception)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [15] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.ExceptionAsHResultMarshaller`1::ConvertToUnmanaged:T(System.Exception) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 15,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.ExceptionAsHResultMarshaller`1::ConvertToUnmanaged:T(System.Exception)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [16] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.ExceptionAsNaNMarshaller`1::ConvertToUnmanaged:T(System.Exception) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 16,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.ExceptionAsNaNMarshaller`1::ConvertToUnmanaged:T(System.Exception)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [17] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.ExceptionAsVoidMarshaller::ConvertToUnmanaged:System.Void(System.Exception) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 17,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.ExceptionAsVoidMarshaller::ConvertToUnmanaged:System.Void(System.Exception)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [18] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.GeneratedComClassAttribute::.ctor:System.Void() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 18,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.GeneratedComClassAttribute::.ctor:System.Void()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [19] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.GeneratedComInterfaceAttribute::get_ExceptionToUnmanagedMarshaller:System.Type() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 19,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.GeneratedComInterfaceAttribute::get_ExceptionToUnmanagedMarshaller:System.Type()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [20] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.GeneratedComInterfaceAttribute::get_Options:System.Runtime.InteropServices.Marshalling.ComInterfaceOptions() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 20,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.GeneratedComInterfaceAttribute::get_Options:System.Runtime.InteropServices.Marshalling.ComInterfaceOptions()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [21] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.GeneratedComInterfaceAttribute::get_StringMarshalling:System.Runtime.InteropServices.StringMarshalling() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 21,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.GeneratedComInterfaceAttribute::get_StringMarshalling:System.Runtime.InteropServices.StringMarshalling()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [22] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.GeneratedComInterfaceAttribute::get_StringMarshallingCustomType:System.Type() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 22,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.GeneratedComInterfaceAttribute::get_StringMarshallingCustomType:System.Type()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [23] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.GeneratedComInterfaceAttribute::set_ExceptionToUnmanagedMarshaller:System.Void(System.Type) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 23,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.GeneratedComInterfaceAttribute::set_ExceptionToUnmanagedMarshaller:System.Void(System.Type)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [24] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.GeneratedComInterfaceAttribute::set_Options:System.Void(System.Runtime.InteropServices.Marshalling.ComInterfaceOptions) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 24,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.GeneratedComInterfaceAttribute::set_Options:System.Void(System.Runtime.InteropServices.Marshalling.ComInterfaceOptions)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [25] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.GeneratedComInterfaceAttribute::set_StringMarshalling:System.Void(System.Runtime.InteropServices.StringMarshalling) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 25,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.GeneratedComInterfaceAttribute::set_StringMarshalling:System.Void(System.Runtime.InteropServices.StringMarshalling)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [26] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.GeneratedComInterfaceAttribute::set_StringMarshallingCustomType:System.Void(System.Type) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 26,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.GeneratedComInterfaceAttribute::set_StringMarshallingCustomType:System.Void(System.Type)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [27] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.GeneratedComInterfaceAttribute::.ctor:System.Void() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 27,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.GeneratedComInterfaceAttribute::.ctor:System.Void()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [28] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.IComExposedClass::GetComInterfaceEntries:System.Runtime.InteropServices.ComWrappers+ComInterfaceEntry*(System.Int32&) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 28,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.IComExposedClass::GetComInterfaceEntries:System.Runtime.InteropServices.ComWrappers+ComInterfaceEntry*(System.Int32&)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [29] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.IComExposedDetails::GetComInterfaceEntries:System.Runtime.InteropServices.ComWrappers+ComInterfaceEntry*(System.Int32&) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 29,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.IComExposedDetails::GetComInterfaceEntries:System.Runtime.InteropServices.ComWrappers+ComInterfaceEntry*(System.Int32&)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [30] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.IIUnknownCacheStrategy::Clear:System.Void(System.Runtime.InteropServices.Marshalling.IIUnknownStrategy) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 30,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.IIUnknownCacheStrategy::Clear:System.Void(System.Runtime.InteropServices.Marshalling.IIUnknownStrategy)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [31] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.IIUnknownCacheStrategy::ConstructTableInfo:System.Runtime.InteropServices.Marshalling.IIUnknownCacheStrategy+TableInfo(System.RuntimeTypeHandle,System.Runtime.InteropServices.Marshalling.IIUnknownDerivedDetails,System.Void*) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 31,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.IIUnknownCacheStrategy::ConstructTableInfo:System.Runtime.InteropServices.Marshalling.IIUnknownCacheStrategy+TableInfo(System.RuntimeTypeHandle,System.Runtime.InteropServices.Marshalling.IIUnknownDerivedDetails,System.Void*)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [32] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.IIUnknownCacheStrategy::TryGetTableInfo:System.Boolean(System.RuntimeTypeHandle,System.Runtime.InteropServices.Marshalling.IIUnknownCacheStrategy+TableInfo&) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 32,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.IIUnknownCacheStrategy::TryGetTableInfo:System.Boolean(System.RuntimeTypeHandle,System.Runtime.InteropServices.Marshalling.IIUnknownCacheStrategy+TableInfo&)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [33] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.IIUnknownCacheStrategy::TrySetTableInfo:System.Boolean(System.RuntimeTypeHandle,System.Runtime.InteropServices.Marshalling.IIUnknownCacheStrategy+TableInfo) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 33,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.IIUnknownCacheStrategy::TrySetTableInfo:System.Boolean(System.RuntimeTypeHandle,System.Runtime.InteropServices.Marshalling.IIUnknownCacheStrategy+TableInfo)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [34] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.IIUnknownCacheStrategy+TableInfo::get_ManagedType:System.RuntimeTypeHandle() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 34,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.IIUnknownCacheStrategy+TableInfo::get_ManagedType:System.RuntimeTypeHandle()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [35] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.IIUnknownCacheStrategy+TableInfo::get_Table:System.Void**() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 35,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.IIUnknownCacheStrategy+TableInfo::get_Table:System.Void**()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [36] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.IIUnknownCacheStrategy+TableInfo::get_ThisPtr:System.Void*() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 36,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.IIUnknownCacheStrategy+TableInfo::get_ThisPtr:System.Void*()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [37] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.IIUnknownCacheStrategy+TableInfo::set_ManagedType:System.Void(System.RuntimeTypeHandle) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 37,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.IIUnknownCacheStrategy+TableInfo::set_ManagedType:System.Void(System.RuntimeTypeHandle)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [38] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.IIUnknownCacheStrategy+TableInfo::set_Table:System.Void(System.Void**) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 38,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.IIUnknownCacheStrategy+TableInfo::set_Table:System.Void(System.Void**)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [39] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.IIUnknownCacheStrategy+TableInfo::set_ThisPtr:System.Void(System.Void*) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 39,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.IIUnknownCacheStrategy+TableInfo::set_ThisPtr:System.Void(System.Void*)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [40] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.IIUnknownDerivedDetails::get_Iid:System.Guid() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 40,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.IIUnknownDerivedDetails::get_Iid:System.Guid()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [41] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.IIUnknownDerivedDetails::get_Implementation:System.Type() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 41,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.IIUnknownDerivedDetails::get_Implementation:System.Type()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [42] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.IIUnknownDerivedDetails::get_ManagedVirtualMethodTable:System.Void**() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 42,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.IIUnknownDerivedDetails::get_ManagedVirtualMethodTable:System.Void**()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [43] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.IIUnknownInterfaceDetailsStrategy::GetComExposedTypeDetails:System.Runtime.InteropServices.Marshalling.IComExposedDetails(System.RuntimeTypeHandle) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 43,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.IIUnknownInterfaceDetailsStrategy::GetComExposedTypeDetails:System.Runtime.InteropServices.Marshalling.IComExposedDetails(System.RuntimeTypeHandle)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [44] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.IIUnknownInterfaceDetailsStrategy::GetIUnknownDerivedDetails:System.Runtime.InteropServices.Marshalling.IIUnknownDerivedDetails(System.RuntimeTypeHandle) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 44,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.IIUnknownInterfaceDetailsStrategy::GetIUnknownDerivedDetails:System.Runtime.InteropServices.Marshalling.IIUnknownDerivedDetails(System.RuntimeTypeHandle)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [45] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.IIUnknownInterfaceType::get_Iid:System.Guid() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 45,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.IIUnknownInterfaceType::get_Iid:System.Guid()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [46] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.IIUnknownInterfaceType::get_ManagedVirtualMethodTable:System.Void**() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 46,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.IIUnknownInterfaceType::get_ManagedVirtualMethodTable:System.Void**()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [47] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.IIUnknownStrategy::CreateInstancePointer:System.Void*(System.Void*) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 47,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.IIUnknownStrategy::CreateInstancePointer:System.Void*(System.Void*)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [48] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.IIUnknownStrategy::QueryInterface:System.Int32(System.Void*,System.Guid&,System.Void*&) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 48,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.IIUnknownStrategy::QueryInterface:System.Int32(System.Void*,System.Guid&,System.Void*&)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [49] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.IIUnknownStrategy::Release:System.Int32(System.Void*) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 49,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.IIUnknownStrategy::Release:System.Int32(System.Void*)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [50] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.IUnknownDerivedAttribute`2::get_Iid:System.Guid() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 50,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.IUnknownDerivedAttribute`2::get_Iid:System.Guid()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [51] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.IUnknownDerivedAttribute`2::get_Implementation:System.Type() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 51,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.IUnknownDerivedAttribute`2::get_Implementation:System.Type()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [52] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.IUnknownDerivedAttribute`2::get_ManagedVirtualMethodTable:System.Void**() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 52,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.IUnknownDerivedAttribute`2::get_ManagedVirtualMethodTable:System.Void**()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [53] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.IUnknownDerivedAttribute`2::.ctor:System.Void() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 53,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.IUnknownDerivedAttribute`2::.ctor:System.Void()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [54] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.IUnmanagedVirtualMethodTableProvider::GetVirtualMethodTableInfoForKey:System.Runtime.InteropServices.Marshalling.VirtualMethodTableInfo(System.Type) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 54,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.IUnmanagedVirtualMethodTableProvider::GetVirtualMethodTableInfoForKey:System.Runtime.InteropServices.Marshalling.VirtualMethodTableInfo(System.Type)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [55] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.StrategyBasedComWrappers::get_DefaultIUnknownInterfaceDetailsStrategy:System.Runtime.InteropServices.Marshalling.IIUnknownInterfaceDetailsStrategy() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 55,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.StrategyBasedComWrappers::get_DefaultIUnknownInterfaceDetailsStrategy:System.Runtime.InteropServices.Marshalling.IIUnknownInterfaceDetailsStrategy()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [56] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.StrategyBasedComWrappers::get_DefaultIUnknownStrategy:System.Runtime.InteropServices.Marshalling.IIUnknownStrategy() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 56,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.StrategyBasedComWrappers::get_DefaultIUnknownStrategy:System.Runtime.InteropServices.Marshalling.IIUnknownStrategy()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [57] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.StrategyBasedComWrappers::.ctor:System.Void() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 57,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.StrategyBasedComWrappers::.ctor:System.Void()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [58] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.UniqueComInterfaceMarshaller`1::ConvertToManaged:T(System.Void*) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 58,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.UniqueComInterfaceMarshaller`1::ConvertToManaged:T(System.Void*)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [59] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.UniqueComInterfaceMarshaller`1::ConvertToUnmanaged:System.Void*(T) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 59,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.UniqueComInterfaceMarshaller`1::ConvertToUnmanaged:System.Void*(T)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [60] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.UniqueComInterfaceMarshaller`1::Free:System.Void(System.Void*) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 60,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.UniqueComInterfaceMarshaller`1::Free:System.Void(System.Void*)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [61] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.VirtualMethodTableInfo::Deconstruct:System.Void(System.Void*&,System.Void**&) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 61,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.VirtualMethodTableInfo::Deconstruct:System.Void(System.Void*&,System.Void**&)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [62] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.VirtualMethodTableInfo::get_ThisPointer:System.Void*() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 62,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.VirtualMethodTableInfo::get_ThisPointer:System.Void*()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [63] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.VirtualMethodTableInfo::get_VirtualMethodTable:System.Void**() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 63,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.VirtualMethodTableInfo::get_VirtualMethodTable:System.Void**()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [64] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.VirtualMethodTableInfo::.ctor:System.Void(System.Void*,System.Void**) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 64,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.VirtualMethodTableInfo::.ctor:System.Void(System.Void*,System.Void**)",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [65] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.CustomMarshallerAttribute::get_ManagedType:System.Type() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 65,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.CustomMarshallerAttribute::get_ManagedType:System.Type()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [66] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.CustomMarshallerAttribute::get_MarshallerType:System.Type() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 66,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.CustomMarshallerAttribute::get_MarshallerType:System.Type()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [67] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.CustomMarshallerAttribute::get_MarshalMode:System.Runtime.InteropServices.Marshalling.MarshalMode() — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 67,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.CustomMarshallerAttribute::get_MarshalMode:System.Runtime.InteropServices.Marshalling.MarshalMode()",
                    ElapsedMilliseconds = 0.0,
                    Iterations = 100000,
                    IsBodyReal = false,
                    IsException = false,
                });
            }
            { // [68] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.CustomMarshallerAttribute::.ctor:System.Void(System.Type,System.Runtime.InteropServices.Marshalling.MarshalMode,System.Type) — unsupported type
                results.Add(new MethodResult {
                    MethodIndex = 68,
                    MethodSubjectId = "System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.CustomMarshallerAttribute::.ctor:System.Void(System.Type,System.Runtime.InteropServices.Marshalling.MarshalMode,System.Type)",
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
