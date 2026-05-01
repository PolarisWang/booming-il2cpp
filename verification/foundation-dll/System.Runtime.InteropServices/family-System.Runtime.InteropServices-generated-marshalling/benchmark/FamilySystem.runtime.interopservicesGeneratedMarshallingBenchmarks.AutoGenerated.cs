using System;
using System.IO;
using System.Threading;
using System.Threading.Tasks;
using System.Globalization;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;
using System.Collections.Generic;
using System.Reflection;
using Chaos.TestFramework;

// Auto-generated benchmark skeletons for FamilySystem.runtime.interopservicesGeneratedMarshallingBenchmarks.
// Framework handles timing — body only needs to invoke the method under measurement.
public static class FamilySystem.runtime.interopservicesGeneratedMarshallingBenchmarks
{
    // Purpose: Benchmark native-runtime performance of AnsiStringMarshaller.ConvertToManaged with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.AnsiStringMarshaller::ConvertToManaged:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_AnsiStringMarshaller_ConvertToManaged_System_V() { AnsiStringMarshaller.ConvertToManaged(null!); }
    // Purpose: Benchmark native-runtime performance of AnsiStringMarshaller.ConvertToUnmanaged with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.AnsiStringMarshaller::ConvertToUnmanaged:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_AnsiStringMarshaller_ConvertToUnmanaged_System() { AnsiStringMarshaller.ConvertToUnmanaged(null!); }
    // Purpose: Benchmark native-runtime performance of AnsiStringMarshaller.Free with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.AnsiStringMarshaller::Free:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_AnsiStringMarshaller_Free_System_Void_P0() { AnsiStringMarshaller.Free(null!); }
    // Purpose: Benchmark native-runtime performance of AnsiStringMarshaller+ManagedToUnmanagedIn.Free with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.AnsiStringMarshaller+ManagedToUnmanagedIn::Free:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_AnsiStringMarshaller_ManagedToUnmanagedIn_Free() { AnsiStringMarshaller+ManagedToUnmanagedIn.Free(); }
    // Purpose: Benchmark native-runtime performance of AnsiStringMarshaller+ManagedToUnmanagedIn.FromManaged with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.AnsiStringMarshaller+ManagedToUnmanagedIn::FromManaged:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_AnsiStringMarshaller_ManagedToUnmanagedIn_From() { AnsiStringMarshaller+ManagedToUnmanagedIn.FromManaged(null!, null!); }
    // Purpose: Benchmark native-runtime performance of AnsiStringMarshaller+ManagedToUnmanagedIn.ToUnmanaged with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.AnsiStringMarshaller+ManagedToUnmanagedIn::ToUnmanaged:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_AnsiStringMarshaller_ManagedToUnmanagedIn_ToUn() { AnsiStringMarshaller+ManagedToUnmanagedIn.ToUnmanaged(); }
    // Purpose: Benchmark native-runtime performance of AnsiStringMarshaller+ManagedToUnmanagedIn.get_BufferSize with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.AnsiStringMarshaller+ManagedToUnmanagedIn::get_BufferSize:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_AnsiStringMarshaller_ManagedToUnmanagedIn_get_() { AnsiStringMarshaller+ManagedToUnmanagedIn.BufferSize; }
    // Purpose: Benchmark native-runtime performance of AnsiStringMarshaller+ManagedToUnmanagedIn.set_BufferSize with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.AnsiStringMarshaller+ManagedToUnmanagedIn::set_BufferSize:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_AnsiStringMarshaller_ManagedToUnmanagedIn_set_() { AnsiStringMarshaller+ManagedToUnmanagedIn.BufferSize = null!; }
    // Purpose: Benchmark native-runtime performance of ArrayMarshaller.AllocateContainerForManagedElements with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.ArrayMarshaller`2::AllocateContainerForManagedElements:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_ArrayMarshaller_2_AllocateContainerForManagedE() { ArrayMarshaller.AllocateContainerForManagedElements(null!, null!); }
    // Purpose: Benchmark native-runtime performance of ArrayMarshaller.AllocateContainerForUnmanagedElements with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.ArrayMarshaller`2::AllocateContainerForUnmanagedElements:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_ArrayMarshaller_2_AllocateContainerForUnmanage() { ArrayMarshaller.AllocateContainerForUnmanagedElements(null!, null!); }
    // Purpose: Benchmark native-runtime performance of ArrayMarshaller.Free with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.ArrayMarshaller`2::Free:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_ArrayMarshaller_2_Free_System_Void_P0() { ArrayMarshaller.Free(null!); }
    // Purpose: Benchmark native-runtime performance of ArrayMarshaller.GetManagedValuesDestination with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.ArrayMarshaller`2::GetManagedValuesDestination:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_ArrayMarshaller_2_GetManagedValuesDestination_() { ArrayMarshaller.GetManagedValuesDestination(null!); }
    // Purpose: Benchmark native-runtime performance of ArrayMarshaller.GetManagedValuesSource with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.ArrayMarshaller`2::GetManagedValuesSource:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_ArrayMarshaller_2_GetManagedValuesSource_Syste() { ArrayMarshaller.GetManagedValuesSource(null!); }
    // Purpose: Benchmark native-runtime performance of ArrayMarshaller.GetUnmanagedValuesDestination with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.ArrayMarshaller`2::GetUnmanagedValuesDestination:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_ArrayMarshaller_2_GetUnmanagedValuesDestinatio() { ArrayMarshaller.GetUnmanagedValuesDestination(null!, null!); }
    // Purpose: Benchmark native-runtime performance of ArrayMarshaller.GetUnmanagedValuesSource with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.ArrayMarshaller`2::GetUnmanagedValuesSource:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_ArrayMarshaller_2_GetUnmanagedValuesSource_Sys() { ArrayMarshaller.GetUnmanagedValuesSource(null!, null!); }
    // Purpose: Benchmark native-runtime performance of ArrayMarshaller`2+ManagedToUnmanagedIn.Free with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.ArrayMarshaller`2+ManagedToUnmanagedIn::Free:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_ArrayMarshaller_2_ManagedToUnmanagedIn_Free_Sy() { ArrayMarshaller`2+ManagedToUnmanagedIn.Free(); }
    // Purpose: Benchmark native-runtime performance of ArrayMarshaller`2+ManagedToUnmanagedIn.FromManaged with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.ArrayMarshaller`2+ManagedToUnmanagedIn::FromManaged:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_ArrayMarshaller_2_ManagedToUnmanagedIn_FromMan() { ArrayMarshaller`2+ManagedToUnmanagedIn.FromManaged(null!, null!); }
    // Purpose: Benchmark native-runtime performance of ArrayMarshaller`2+ManagedToUnmanagedIn.GetManagedValuesSource with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.ArrayMarshaller`2+ManagedToUnmanagedIn::GetManagedValuesSource:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_ArrayMarshaller_2_ManagedToUnmanagedIn_GetMana() { ArrayMarshaller`2+ManagedToUnmanagedIn.GetManagedValuesSource(); }
    // Purpose: Benchmark native-runtime performance of ArrayMarshaller`2+ManagedToUnmanagedIn.GetPinnableReference with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.ArrayMarshaller`2+ManagedToUnmanagedIn::GetPinnableReference:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_ArrayMarshaller_2_ManagedToUnmanagedIn_GetPinn() { ArrayMarshaller`2+ManagedToUnmanagedIn.GetPinnableReference(); }
    // Purpose: Benchmark native-runtime performance of ArrayMarshaller`2+ManagedToUnmanagedIn.GetPinnableReference with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.ArrayMarshaller`2+ManagedToUnmanagedIn::GetPinnableReference:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_ArrayMarshaller_2_ManagedToUnmanagedIn_GetPinn() { ArrayMarshaller`2+ManagedToUnmanagedIn.GetPinnableReference(null!); }
    // Purpose: Benchmark native-runtime performance of ArrayMarshaller`2+ManagedToUnmanagedIn.GetUnmanagedValuesDestination with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.ArrayMarshaller`2+ManagedToUnmanagedIn::GetUnmanagedValuesDestination:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_ArrayMarshaller_2_ManagedToUnmanagedIn_GetUnma() { ArrayMarshaller`2+ManagedToUnmanagedIn.GetUnmanagedValuesDestination(); }
    // Purpose: Benchmark native-runtime performance of ArrayMarshaller`2+ManagedToUnmanagedIn.ToUnmanaged with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.ArrayMarshaller`2+ManagedToUnmanagedIn::ToUnmanaged:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_ArrayMarshaller_2_ManagedToUnmanagedIn_ToUnman() { ArrayMarshaller`2+ManagedToUnmanagedIn.ToUnmanaged(); }
    // Purpose: Benchmark native-runtime performance of ArrayMarshaller`2+ManagedToUnmanagedIn.get_BufferSize with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.ArrayMarshaller`2+ManagedToUnmanagedIn::get_BufferSize:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_ArrayMarshaller_2_ManagedToUnmanagedIn_get_Buf() { ArrayMarshaller`2+ManagedToUnmanagedIn.BufferSize; }
    // Purpose: Benchmark native-runtime performance of ArrayMarshaller`2+ManagedToUnmanagedIn.set_BufferSize with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.ArrayMarshaller`2+ManagedToUnmanagedIn::set_BufferSize:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_ArrayMarshaller_2_ManagedToUnmanagedIn_set_Buf() { ArrayMarshaller`2+ManagedToUnmanagedIn.BufferSize = null!; }
    // Purpose: Benchmark native-runtime performance of BStrStringMarshaller.ConvertToManaged with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.BStrStringMarshaller::ConvertToManaged:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_BStrStringMarshaller_ConvertToManaged_System_V() { BStrStringMarshaller.ConvertToManaged(null!); }
    // Purpose: Benchmark native-runtime performance of BStrStringMarshaller.ConvertToUnmanaged with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.BStrStringMarshaller::ConvertToUnmanaged:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_BStrStringMarshaller_ConvertToUnmanaged_System() { BStrStringMarshaller.ConvertToUnmanaged(null!); }
    // Purpose: Benchmark native-runtime performance of BStrStringMarshaller.Free with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.BStrStringMarshaller::Free:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_BStrStringMarshaller_Free_System_Void_P0() { BStrStringMarshaller.Free(null!); }
    // Purpose: Benchmark native-runtime performance of BStrStringMarshaller+ManagedToUnmanagedIn.Free with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.BStrStringMarshaller+ManagedToUnmanagedIn::Free:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_BStrStringMarshaller_ManagedToUnmanagedIn_Free() { BStrStringMarshaller+ManagedToUnmanagedIn.Free(); }
    // Purpose: Benchmark native-runtime performance of BStrStringMarshaller+ManagedToUnmanagedIn.FromManaged with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.BStrStringMarshaller+ManagedToUnmanagedIn::FromManaged:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_BStrStringMarshaller_ManagedToUnmanagedIn_From() { BStrStringMarshaller+ManagedToUnmanagedIn.FromManaged(null!, null!); }
    // Purpose: Benchmark native-runtime performance of BStrStringMarshaller+ManagedToUnmanagedIn.ToUnmanaged with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.BStrStringMarshaller+ManagedToUnmanagedIn::ToUnmanaged:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_BStrStringMarshaller_ManagedToUnmanagedIn_ToUn() { BStrStringMarshaller+ManagedToUnmanagedIn.ToUnmanaged(); }
    // Purpose: Benchmark native-runtime performance of BStrStringMarshaller+ManagedToUnmanagedIn.get_BufferSize with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.BStrStringMarshaller+ManagedToUnmanagedIn::get_BufferSize:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_BStrStringMarshaller_ManagedToUnmanagedIn_get_() { BStrStringMarshaller+ManagedToUnmanagedIn.BufferSize; }
    // Purpose: Benchmark native-runtime performance of BStrStringMarshaller+ManagedToUnmanagedIn.set_BufferSize with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.BStrStringMarshaller+ManagedToUnmanagedIn::set_BufferSize:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_BStrStringMarshaller_ManagedToUnmanagedIn_set_() { BStrStringMarshaller+ManagedToUnmanagedIn.BufferSize = null!; }
    // Purpose: Benchmark native-runtime performance of ComExposedClassAttribute.ComExposedClassAttribute`1: with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.ComExposedClassAttribute`1::ComExposedClassAttribute`1::Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_ComExposedClassAttribute_1_ComExposedClassAttr() { _ = ComExposedClassAttribute.ComExposedClassAttribute`1:(); }
    // Purpose: Benchmark native-runtime performance of ComExposedClassAttribute.GetComInterfaceEntries with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.ComExposedClassAttribute`1::GetComInterfaceEntries:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_ComExposedClassAttribute_1_GetComInterfaceEntr() { ComExposedClassAttribute.GetComInterfaceEntries(null!); }
    // Purpose: Benchmark native-runtime performance of ComInterfaceMarshaller.ConvertToManaged with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.ComInterfaceMarshaller`1::ConvertToManaged:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_ComInterfaceMarshaller_1_ConvertToManaged_Syst() { ComInterfaceMarshaller.ConvertToManaged(null!); }
    // Purpose: Benchmark native-runtime performance of ComInterfaceMarshaller.ConvertToUnmanaged with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.ComInterfaceMarshaller`1::ConvertToUnmanaged:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_ComInterfaceMarshaller_1_ConvertToUnmanaged_Sy() { ComInterfaceMarshaller.ConvertToUnmanaged(null!); }
    // Purpose: Benchmark native-runtime performance of ComInterfaceMarshaller.Free with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.ComInterfaceMarshaller`1::Free:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_ComInterfaceMarshaller_1_Free_System_Void_P0() { ComInterfaceMarshaller.Free(null!); }
    // Purpose: Benchmark native-runtime performance of ComObject.FinalRelease with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.ComObject::FinalRelease:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_ComObject_FinalRelease_System_Void() { ComObject.FinalRelease(); }
    // Purpose: Benchmark native-runtime performance of ComObject.Finalize with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.ComObject::Finalize:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_ComObject_Finalize_System_Void() { ComObject.Finalize(); }
    // Purpose: Benchmark native-runtime performance of ComVariant.As`1 with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.ComVariant::As`1:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_ComVariant_As_1_System_Void() { ComVariant.As`1(); }
    // Purpose: Benchmark native-runtime performance of ComVariant.CreateRaw`1 with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.ComVariant::CreateRaw`1:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_ComVariant_CreateRaw_1_System_Void_P0_P1() { ComVariant.CreateRaw`1(null!, null!); }
    // Purpose: Benchmark native-runtime performance of ComVariant.Create`1 with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.ComVariant::Create`1:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_ComVariant_Create_1_System_Void_P0() { ComVariant.Create`1(null!); }
    // Purpose: Benchmark native-runtime performance of ComVariant.Dispose with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.ComVariant::Dispose:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_ComVariant_Dispose_System_Void() { ComVariant.Dispose(); }
    // Purpose: Benchmark native-runtime performance of ComVariant.GetRawDataRef`1 with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.ComVariant::GetRawDataRef`1:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_ComVariant_GetRawDataRef_1_System_Void() { ComVariant.GetRawDataRef`1(); }
    // Purpose: Benchmark native-runtime performance of ComVariant.get_Null with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.ComVariant::get_Null:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_ComVariant_get_Null_System_Void() { ComVariant.Null; }
    // Purpose: Benchmark native-runtime performance of ComVariant.get_VarType with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.ComVariant::get_VarType:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_ComVariant_get_VarType_System_Void() { ComVariant.VarType; }
    // Purpose: Benchmark native-runtime performance of ComVariant.set_Null with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.ComVariant::set_Null:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_ComVariant_set_Null_System_Void_P0() { ComVariant.Null = null!; }
    // Purpose: Benchmark native-runtime performance of ComVariant.set_VarType with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.ComVariant::set_VarType:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_ComVariant_set_VarType_System_Void_P0() { ComVariant.VarType = null!; }
    // Purpose: Benchmark native-runtime performance of ComVariantMarshaller.ConvertToManaged with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.ComVariantMarshaller::ConvertToManaged:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_ComVariantMarshaller_ConvertToManaged_System_V() { ComVariantMarshaller.ConvertToManaged(null!); }
    // Purpose: Benchmark native-runtime performance of ComVariantMarshaller.ConvertToUnmanaged with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.ComVariantMarshaller::ConvertToUnmanaged:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_ComVariantMarshaller_ConvertToUnmanaged_System() { ComVariantMarshaller.ConvertToUnmanaged(null!); }
    // Purpose: Benchmark native-runtime performance of ComVariantMarshaller.Free with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.ComVariantMarshaller::Free:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_ComVariantMarshaller_Free_System_Void_P0() { ComVariantMarshaller.Free(null!); }
    // Purpose: Benchmark native-runtime performance of ComVariantMarshaller+RefPropagate.Free with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.ComVariantMarshaller+RefPropagate::Free:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_ComVariantMarshaller_RefPropagate_Free_System_() { ComVariantMarshaller+RefPropagate.Free(); }
    // Purpose: Benchmark native-runtime performance of ComVariantMarshaller+RefPropagate.FromManaged with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.ComVariantMarshaller+RefPropagate::FromManaged:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_ComVariantMarshaller_RefPropagate_FromManaged_() { ComVariantMarshaller+RefPropagate.FromManaged(null!); }
    // Purpose: Benchmark native-runtime performance of ComVariantMarshaller+RefPropagate.FromUnmanaged with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.ComVariantMarshaller+RefPropagate::FromUnmanaged:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_ComVariantMarshaller_RefPropagate_FromUnmanage() { ComVariantMarshaller+RefPropagate.FromUnmanaged(null!); }
    // Purpose: Benchmark native-runtime performance of ComVariantMarshaller+RefPropagate.ToManaged with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.ComVariantMarshaller+RefPropagate::ToManaged:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_ComVariantMarshaller_RefPropagate_ToManaged_Sy() { ComVariantMarshaller+RefPropagate.ToManaged(); }
    // Purpose: Benchmark native-runtime performance of ComVariantMarshaller+RefPropagate.ToUnmanaged with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.ComVariantMarshaller+RefPropagate::ToUnmanaged:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_ComVariantMarshaller_RefPropagate_ToUnmanaged_() { ComVariantMarshaller+RefPropagate.ToUnmanaged(); }
    // Purpose: Benchmark native-runtime performance of ExceptionAsDefaultMarshaller.ConvertToUnmanaged with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.ExceptionAsDefaultMarshaller`1::ConvertToUnmanaged:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_ExceptionAsDefaultMarshaller_1_ConvertToUnmana() { ExceptionAsDefaultMarshaller.ConvertToUnmanaged(null!); }
    // Purpose: Benchmark native-runtime performance of ExceptionAsHResultMarshaller.ConvertToUnmanaged with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.ExceptionAsHResultMarshaller`1::ConvertToUnmanaged:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_ExceptionAsHResultMarshaller_1_ConvertToUnmana() { ExceptionAsHResultMarshaller.ConvertToUnmanaged(null!); }
    // Purpose: Benchmark native-runtime performance of ExceptionAsNaNMarshaller.ConvertToUnmanaged with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.ExceptionAsNaNMarshaller`1::ConvertToUnmanaged:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_ExceptionAsNaNMarshaller_1_ConvertToUnmanaged_() { ExceptionAsNaNMarshaller.ConvertToUnmanaged(null!); }
    // Purpose: Benchmark native-runtime performance of ExceptionAsVoidMarshaller.ConvertToUnmanaged with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.ExceptionAsVoidMarshaller::ConvertToUnmanaged:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_ExceptionAsVoidMarshaller_ConvertToUnmanaged_S() { ExceptionAsVoidMarshaller.ConvertToUnmanaged(null!); }
    // Purpose: Benchmark native-runtime performance of GeneratedComClassAttribute.GeneratedComClassAttribute: with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.GeneratedComClassAttribute::GeneratedComClassAttribute::Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_GeneratedComClassAttribute_GeneratedComClassAt() { _ = GeneratedComClassAttribute.GeneratedComClassAttribute:(); }
    // Purpose: Benchmark native-runtime performance of GeneratedComInterfaceAttribute.GeneratedComInterfaceAttribute: with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.GeneratedComInterfaceAttribute::GeneratedComInterfaceAttribute::Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_GeneratedComInterfaceAttribute_GeneratedComInt() { _ = GeneratedComInterfaceAttribute.GeneratedComInterfaceAttribute:(); }
    // Purpose: Benchmark native-runtime performance of GeneratedComInterfaceAttribute.get_ExceptionToUnmanagedMarshaller with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.GeneratedComInterfaceAttribute::get_ExceptionToUnmanagedMarshaller:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_GeneratedComInterfaceAttribute_get_ExceptionTo() { GeneratedComInterfaceAttribute.ExceptionToUnmanagedMarshaller; }
    // Purpose: Benchmark native-runtime performance of GeneratedComInterfaceAttribute.get_Options with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.GeneratedComInterfaceAttribute::get_Options:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_GeneratedComInterfaceAttribute_get_Options_Sys() { GeneratedComInterfaceAttribute.Options; }
    // Purpose: Benchmark native-runtime performance of GeneratedComInterfaceAttribute.get_StringMarshalling with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.GeneratedComInterfaceAttribute::get_StringMarshalling:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_GeneratedComInterfaceAttribute_get_StringMarsh() { GeneratedComInterfaceAttribute.StringMarshalling; }
    // Purpose: Benchmark native-runtime performance of GeneratedComInterfaceAttribute.get_StringMarshallingCustomType with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.GeneratedComInterfaceAttribute::get_StringMarshallingCustomType:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_GeneratedComInterfaceAttribute_get_StringMarsh() { GeneratedComInterfaceAttribute.StringMarshallingCustomType; }
    // Purpose: Benchmark native-runtime performance of GeneratedComInterfaceAttribute.set_ExceptionToUnmanagedMarshaller with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.GeneratedComInterfaceAttribute::set_ExceptionToUnmanagedMarshaller:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_GeneratedComInterfaceAttribute_set_ExceptionTo() { GeneratedComInterfaceAttribute.ExceptionToUnmanagedMarshaller = null!; }
    // Purpose: Benchmark native-runtime performance of GeneratedComInterfaceAttribute.set_Options with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.GeneratedComInterfaceAttribute::set_Options:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_GeneratedComInterfaceAttribute_set_Options_Sys() { GeneratedComInterfaceAttribute.Options = null!; }
    // Purpose: Benchmark native-runtime performance of GeneratedComInterfaceAttribute.set_StringMarshalling with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.GeneratedComInterfaceAttribute::set_StringMarshalling:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_GeneratedComInterfaceAttribute_set_StringMarsh() { GeneratedComInterfaceAttribute.StringMarshalling = null!; }
    // Purpose: Benchmark native-runtime performance of GeneratedComInterfaceAttribute.set_StringMarshallingCustomType with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.GeneratedComInterfaceAttribute::set_StringMarshallingCustomType:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_GeneratedComInterfaceAttribute_set_StringMarsh() { GeneratedComInterfaceAttribute.StringMarshallingCustomType = null!; }
    // Purpose: Benchmark native-runtime performance of IComExposedClass.GetComInterfaceEntries with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.IComExposedClass::GetComInterfaceEntries:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_IComExposedClass_GetComInterfaceEntries_System() { IComExposedClass.GetComInterfaceEntries(null!); }
    // Purpose: Benchmark native-runtime performance of IComExposedDetails.GetComInterfaceEntries with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.IComExposedDetails::GetComInterfaceEntries:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_IComExposedDetails_GetComInterfaceEntries_Syst() { IComExposedDetails.GetComInterfaceEntries(null!); }
    // Purpose: Benchmark native-runtime performance of IIUnknownCacheStrategy.Clear with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.IIUnknownCacheStrategy::Clear:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_IIUnknownCacheStrategy_Clear_System_Void_P0() { IIUnknownCacheStrategy.Clear(null!); }
    // Purpose: Benchmark native-runtime performance of IIUnknownCacheStrategy.ConstructTableInfo with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.IIUnknownCacheStrategy::ConstructTableInfo:System.Void(P0, P1, P2)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_IIUnknownCacheStrategy_ConstructTableInfo_Syst() { IIUnknownCacheStrategy.ConstructTableInfo(null!, null!, null!); }
    // Purpose: Benchmark native-runtime performance of IIUnknownCacheStrategy.TryGetTableInfo with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.IIUnknownCacheStrategy::TryGetTableInfo:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_IIUnknownCacheStrategy_TryGetTableInfo_System_() { IIUnknownCacheStrategy.TryGetTableInfo(null!, null!); }
    // Purpose: Benchmark native-runtime performance of IIUnknownCacheStrategy.TrySetTableInfo with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.IIUnknownCacheStrategy::TrySetTableInfo:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_IIUnknownCacheStrategy_TrySetTableInfo_System_() { IIUnknownCacheStrategy.TrySetTableInfo(null!, null!); }
    // Purpose: Benchmark native-runtime performance of IIUnknownCacheStrategy+TableInfo.get_ManagedType with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.IIUnknownCacheStrategy+TableInfo::get_ManagedType:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_IIUnknownCacheStrategy_TableInfo_get_ManagedTy() { IIUnknownCacheStrategy+TableInfo.ManagedType; }
    // Purpose: Benchmark native-runtime performance of IIUnknownCacheStrategy+TableInfo.get_Table with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.IIUnknownCacheStrategy+TableInfo::get_Table:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_IIUnknownCacheStrategy_TableInfo_get_Table_Sys() { IIUnknownCacheStrategy+TableInfo.Table; }
    // Purpose: Benchmark native-runtime performance of IIUnknownCacheStrategy+TableInfo.get_ThisPtr with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.IIUnknownCacheStrategy+TableInfo::get_ThisPtr:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_IIUnknownCacheStrategy_TableInfo_get_ThisPtr_S() { IIUnknownCacheStrategy+TableInfo.ThisPtr; }
    // Purpose: Benchmark native-runtime performance of IIUnknownCacheStrategy+TableInfo.set_ManagedType with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.IIUnknownCacheStrategy+TableInfo::set_ManagedType:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_IIUnknownCacheStrategy_TableInfo_set_ManagedTy() { IIUnknownCacheStrategy+TableInfo.ManagedType = null!; }
    // Purpose: Benchmark native-runtime performance of IIUnknownCacheStrategy+TableInfo.set_Table with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.IIUnknownCacheStrategy+TableInfo::set_Table:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_IIUnknownCacheStrategy_TableInfo_set_Table_Sys() { IIUnknownCacheStrategy+TableInfo.Table = null!; }
    // Purpose: Benchmark native-runtime performance of IIUnknownCacheStrategy+TableInfo.set_ThisPtr with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.IIUnknownCacheStrategy+TableInfo::set_ThisPtr:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_IIUnknownCacheStrategy_TableInfo_set_ThisPtr_S() { IIUnknownCacheStrategy+TableInfo.ThisPtr = null!; }
    // Purpose: Benchmark native-runtime performance of IIUnknownDerivedDetails.get_Iid with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.IIUnknownDerivedDetails::get_Iid:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_IIUnknownDerivedDetails_get_Iid_System_Void() { IIUnknownDerivedDetails.Iid; }
    // Purpose: Benchmark native-runtime performance of IIUnknownDerivedDetails.get_Implementation with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.IIUnknownDerivedDetails::get_Implementation:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_IIUnknownDerivedDetails_get_Implementation_Sys() { IIUnknownDerivedDetails.Implementation; }
    // Purpose: Benchmark native-runtime performance of IIUnknownDerivedDetails.get_ManagedVirtualMethodTable with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.IIUnknownDerivedDetails::get_ManagedVirtualMethodTable:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_IIUnknownDerivedDetails_get_ManagedVirtualMeth() { IIUnknownDerivedDetails.ManagedVirtualMethodTable; }
    // Purpose: Benchmark native-runtime performance of IIUnknownDerivedDetails.set_Iid with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.IIUnknownDerivedDetails::set_Iid:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_IIUnknownDerivedDetails_set_Iid_System_Void_P0() { IIUnknownDerivedDetails.Iid = null!; }
    // Purpose: Benchmark native-runtime performance of IIUnknownDerivedDetails.set_Implementation with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.IIUnknownDerivedDetails::set_Implementation:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_IIUnknownDerivedDetails_set_Implementation_Sys() { IIUnknownDerivedDetails.Implementation = null!; }
    // Purpose: Benchmark native-runtime performance of IIUnknownDerivedDetails.set_ManagedVirtualMethodTable with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.IIUnknownDerivedDetails::set_ManagedVirtualMethodTable:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_IIUnknownDerivedDetails_set_ManagedVirtualMeth() { IIUnknownDerivedDetails.ManagedVirtualMethodTable = null!; }
    // Purpose: Benchmark native-runtime performance of IIUnknownInterfaceDetailsStrategy.GetComExposedTypeDetails with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.IIUnknownInterfaceDetailsStrategy::GetComExposedTypeDetails:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_IIUnknownInterfaceDetailsStrategy_GetComExpose() { IIUnknownInterfaceDetailsStrategy.GetComExposedTypeDetails(null!); }
    // Purpose: Benchmark native-runtime performance of IIUnknownInterfaceDetailsStrategy.GetIUnknownDerivedDetails with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.IIUnknownInterfaceDetailsStrategy::GetIUnknownDerivedDetails:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_IIUnknownInterfaceDetailsStrategy_GetIUnknownD() { IIUnknownInterfaceDetailsStrategy.GetIUnknownDerivedDetails(null!); }
    // Purpose: Benchmark native-runtime performance of IIUnknownInterfaceType.get_Iid with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.IIUnknownInterfaceType::get_Iid:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_IIUnknownInterfaceType_get_Iid_System_Void() { IIUnknownInterfaceType.Iid; }
    // Purpose: Benchmark native-runtime performance of IIUnknownInterfaceType.get_ManagedVirtualMethodTable with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.IIUnknownInterfaceType::get_ManagedVirtualMethodTable:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_IIUnknownInterfaceType_get_ManagedVirtualMetho() { IIUnknownInterfaceType.ManagedVirtualMethodTable; }
    // Purpose: Benchmark native-runtime performance of IIUnknownInterfaceType.set_Iid with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.IIUnknownInterfaceType::set_Iid:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_IIUnknownInterfaceType_set_Iid_System_Void_P0() { IIUnknownInterfaceType.Iid = null!; }
    // Purpose: Benchmark native-runtime performance of IIUnknownInterfaceType.set_ManagedVirtualMethodTable with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.IIUnknownInterfaceType::set_ManagedVirtualMethodTable:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_IIUnknownInterfaceType_set_ManagedVirtualMetho() { IIUnknownInterfaceType.ManagedVirtualMethodTable = null!; }
    // Purpose: Benchmark native-runtime performance of IIUnknownStrategy.CreateInstancePointer with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.IIUnknownStrategy::CreateInstancePointer:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_IIUnknownStrategy_CreateInstancePointer_System() { IIUnknownStrategy.CreateInstancePointer(null!); }
    // Purpose: Benchmark native-runtime performance of IIUnknownStrategy.QueryInterface with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.IIUnknownStrategy::QueryInterface:System.Void(P0, P1, P2)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_IIUnknownStrategy_QueryInterface_System_Void_P() { IIUnknownStrategy.QueryInterface(null!, null!, null!); }
    // Purpose: Benchmark native-runtime performance of IIUnknownStrategy.Release with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.IIUnknownStrategy::Release:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_IIUnknownStrategy_Release_System_Void_P0() { IIUnknownStrategy.Release(null!); }
    // Purpose: Benchmark native-runtime performance of IUnknownDerivedAttribute.IUnknownDerivedAttribute`2: with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.IUnknownDerivedAttribute`2::IUnknownDerivedAttribute`2::Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_IUnknownDerivedAttribute_2_IUnknownDerivedAttr() { _ = IUnknownDerivedAttribute.IUnknownDerivedAttribute`2:(); }
    // Purpose: Benchmark native-runtime performance of IUnknownDerivedAttribute.get_Iid with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.IUnknownDerivedAttribute`2::get_Iid:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_IUnknownDerivedAttribute_2_get_Iid_System_Void() { IUnknownDerivedAttribute.Iid; }
    // Purpose: Benchmark native-runtime performance of IUnknownDerivedAttribute.get_Implementation with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.IUnknownDerivedAttribute`2::get_Implementation:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_IUnknownDerivedAttribute_2_get_Implementation_() { IUnknownDerivedAttribute.Implementation; }
    // Purpose: Benchmark native-runtime performance of IUnknownDerivedAttribute.get_ManagedVirtualMethodTable with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.IUnknownDerivedAttribute`2::get_ManagedVirtualMethodTable:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_IUnknownDerivedAttribute_2_get_ManagedVirtualM() { IUnknownDerivedAttribute.ManagedVirtualMethodTable; }
    // Purpose: Benchmark native-runtime performance of IUnknownDerivedAttribute.set_Iid with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.IUnknownDerivedAttribute`2::set_Iid:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_IUnknownDerivedAttribute_2_set_Iid_System_Void() { IUnknownDerivedAttribute.Iid = null!; }
    // Purpose: Benchmark native-runtime performance of IUnknownDerivedAttribute.set_Implementation with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.IUnknownDerivedAttribute`2::set_Implementation:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_IUnknownDerivedAttribute_2_set_Implementation_() { IUnknownDerivedAttribute.Implementation = null!; }
    // Purpose: Benchmark native-runtime performance of IUnknownDerivedAttribute.set_ManagedVirtualMethodTable with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.IUnknownDerivedAttribute`2::set_ManagedVirtualMethodTable:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_IUnknownDerivedAttribute_2_set_ManagedVirtualM() { IUnknownDerivedAttribute.ManagedVirtualMethodTable = null!; }
    // Purpose: Benchmark native-runtime performance of IUnmanagedVirtualMethodTableProvider.GetVirtualMethodTableInfoForKey with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.IUnmanagedVirtualMethodTableProvider::GetVirtualMethodTableInfoForKey:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_IUnmanagedVirtualMethodTableProvider_GetVirtua() { IUnmanagedVirtualMethodTableProvider.GetVirtualMethodTableInfoForKey(null!); }
    // Purpose: Benchmark native-runtime performance of MarshalUsingAttribute.MarshalUsingAttribute: with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.MarshalUsingAttribute::MarshalUsingAttribute::Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_MarshalUsingAttribute_MarshalUsingAttribute_Vo() { _ = MarshalUsingAttribute.MarshalUsingAttribute:(); }
    // Purpose: Benchmark native-runtime performance of MarshalUsingAttribute.MarshalUsingAttribute: with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.MarshalUsingAttribute::MarshalUsingAttribute::Void(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_MarshalUsingAttribute_MarshalUsingAttribute_Vo() { _ = MarshalUsingAttribute.MarshalUsingAttribute:(42); }
    // Purpose: Benchmark native-runtime performance of MarshalUsingAttribute.get_ConstantElementCount with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.MarshalUsingAttribute::get_ConstantElementCount:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_MarshalUsingAttribute_get_ConstantElementCount() { MarshalUsingAttribute.ConstantElementCount; }
    // Purpose: Benchmark native-runtime performance of MarshalUsingAttribute.get_CountElementName with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.MarshalUsingAttribute::get_CountElementName:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_MarshalUsingAttribute_get_CountElementName_Sys() { MarshalUsingAttribute.CountElementName; }
    // Purpose: Benchmark native-runtime performance of MarshalUsingAttribute.get_ElementIndirectionDepth with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.MarshalUsingAttribute::get_ElementIndirectionDepth:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_MarshalUsingAttribute_get_ElementIndirectionDe() { MarshalUsingAttribute.ElementIndirectionDepth; }
    // Purpose: Benchmark native-runtime performance of MarshalUsingAttribute.get_NativeType with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.MarshalUsingAttribute::get_NativeType:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_MarshalUsingAttribute_get_NativeType_System_Vo() { MarshalUsingAttribute.NativeType; }
    // Purpose: Benchmark native-runtime performance of MarshalUsingAttribute.set_ConstantElementCount with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.MarshalUsingAttribute::set_ConstantElementCount:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_MarshalUsingAttribute_set_ConstantElementCount() { MarshalUsingAttribute.ConstantElementCount = null!; }
    // Purpose: Benchmark native-runtime performance of MarshalUsingAttribute.set_CountElementName with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.MarshalUsingAttribute::set_CountElementName:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_MarshalUsingAttribute_set_CountElementName_Sys() { MarshalUsingAttribute.CountElementName = null!; }
    // Purpose: Benchmark native-runtime performance of MarshalUsingAttribute.set_ElementIndirectionDepth with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.MarshalUsingAttribute::set_ElementIndirectionDepth:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_MarshalUsingAttribute_set_ElementIndirectionDe() { MarshalUsingAttribute.ElementIndirectionDepth = null!; }
    // Purpose: Benchmark native-runtime performance of MarshalUsingAttribute.set_NativeType with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.MarshalUsingAttribute::set_NativeType:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_MarshalUsingAttribute_set_NativeType_System_Vo() { MarshalUsingAttribute.NativeType = null!; }
    // Purpose: Benchmark native-runtime performance of PointerArrayMarshaller.AllocateContainerForManagedElements with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.PointerArrayMarshaller`2::AllocateContainerForManagedElements:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_PointerArrayMarshaller_2_AllocateContainerForM() { PointerArrayMarshaller.AllocateContainerForManagedElements(null!, null!); }
    // Purpose: Benchmark native-runtime performance of PointerArrayMarshaller.AllocateContainerForUnmanagedElements with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.PointerArrayMarshaller`2::AllocateContainerForUnmanagedElements:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_PointerArrayMarshaller_2_AllocateContainerForU() { PointerArrayMarshaller.AllocateContainerForUnmanagedElements(null!, null!); }
    // Purpose: Benchmark native-runtime performance of PointerArrayMarshaller.Free with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.PointerArrayMarshaller`2::Free:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_PointerArrayMarshaller_2_Free_System_Void_P0() { PointerArrayMarshaller.Free(null!); }
    // Purpose: Benchmark native-runtime performance of PointerArrayMarshaller.GetManagedValuesDestination with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.PointerArrayMarshaller`2::GetManagedValuesDestination:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_PointerArrayMarshaller_2_GetManagedValuesDesti() { PointerArrayMarshaller.GetManagedValuesDestination(null!); }
    // Purpose: Benchmark native-runtime performance of PointerArrayMarshaller.GetManagedValuesSource with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.PointerArrayMarshaller`2::GetManagedValuesSource:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_PointerArrayMarshaller_2_GetManagedValuesSourc() { PointerArrayMarshaller.GetManagedValuesSource(null!); }
    // Purpose: Benchmark native-runtime performance of PointerArrayMarshaller.GetUnmanagedValuesDestination with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.PointerArrayMarshaller`2::GetUnmanagedValuesDestination:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_PointerArrayMarshaller_2_GetUnmanagedValuesDes() { PointerArrayMarshaller.GetUnmanagedValuesDestination(null!, null!); }
    // Purpose: Benchmark native-runtime performance of PointerArrayMarshaller.GetUnmanagedValuesSource with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.PointerArrayMarshaller`2::GetUnmanagedValuesSource:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_PointerArrayMarshaller_2_GetUnmanagedValuesSou() { PointerArrayMarshaller.GetUnmanagedValuesSource(null!, null!); }
    // Purpose: Benchmark native-runtime performance of PointerArrayMarshaller`2+ManagedToUnmanagedIn.Free with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.PointerArrayMarshaller`2+ManagedToUnmanagedIn::Free:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_PointerArrayMarshaller_2_ManagedToUnmanagedIn_() { PointerArrayMarshaller`2+ManagedToUnmanagedIn.Free(); }
    // Purpose: Benchmark native-runtime performance of PointerArrayMarshaller`2+ManagedToUnmanagedIn.FromManaged with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.PointerArrayMarshaller`2+ManagedToUnmanagedIn::FromManaged:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_PointerArrayMarshaller_2_ManagedToUnmanagedIn_() { PointerArrayMarshaller`2+ManagedToUnmanagedIn.FromManaged(null!, null!); }
    // Purpose: Benchmark native-runtime performance of PointerArrayMarshaller`2+ManagedToUnmanagedIn.GetManagedValuesSource with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.PointerArrayMarshaller`2+ManagedToUnmanagedIn::GetManagedValuesSource:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_PointerArrayMarshaller_2_ManagedToUnmanagedIn_() { PointerArrayMarshaller`2+ManagedToUnmanagedIn.GetManagedValuesSource(); }
    // Purpose: Benchmark native-runtime performance of PointerArrayMarshaller`2+ManagedToUnmanagedIn.GetPinnableReference with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.PointerArrayMarshaller`2+ManagedToUnmanagedIn::GetPinnableReference:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_PointerArrayMarshaller_2_ManagedToUnmanagedIn_() { PointerArrayMarshaller`2+ManagedToUnmanagedIn.GetPinnableReference(); }
    // Purpose: Benchmark native-runtime performance of PointerArrayMarshaller`2+ManagedToUnmanagedIn.GetPinnableReference with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.PointerArrayMarshaller`2+ManagedToUnmanagedIn::GetPinnableReference:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_PointerArrayMarshaller_2_ManagedToUnmanagedIn_() { PointerArrayMarshaller`2+ManagedToUnmanagedIn.GetPinnableReference(null!); }
    // Purpose: Benchmark native-runtime performance of PointerArrayMarshaller`2+ManagedToUnmanagedIn.GetUnmanagedValuesDestination with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.PointerArrayMarshaller`2+ManagedToUnmanagedIn::GetUnmanagedValuesDestination:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_PointerArrayMarshaller_2_ManagedToUnmanagedIn_() { PointerArrayMarshaller`2+ManagedToUnmanagedIn.GetUnmanagedValuesDestination(); }
    // Purpose: Benchmark native-runtime performance of PointerArrayMarshaller`2+ManagedToUnmanagedIn.ToUnmanaged with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.PointerArrayMarshaller`2+ManagedToUnmanagedIn::ToUnmanaged:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_PointerArrayMarshaller_2_ManagedToUnmanagedIn_() { PointerArrayMarshaller`2+ManagedToUnmanagedIn.ToUnmanaged(); }
    // Purpose: Benchmark native-runtime performance of PointerArrayMarshaller`2+ManagedToUnmanagedIn.get_BufferSize with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.PointerArrayMarshaller`2+ManagedToUnmanagedIn::get_BufferSize:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_PointerArrayMarshaller_2_ManagedToUnmanagedIn_() { PointerArrayMarshaller`2+ManagedToUnmanagedIn.BufferSize; }
    // Purpose: Benchmark native-runtime performance of PointerArrayMarshaller`2+ManagedToUnmanagedIn.set_BufferSize with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.PointerArrayMarshaller`2+ManagedToUnmanagedIn::set_BufferSize:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_PointerArrayMarshaller_2_ManagedToUnmanagedIn_() { PointerArrayMarshaller`2+ManagedToUnmanagedIn.BufferSize = null!; }
    // Purpose: Benchmark native-runtime performance of StrategyBasedComWrappers.StrategyBasedComWrappers: with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.StrategyBasedComWrappers::StrategyBasedComWrappers::Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_StrategyBasedComWrappers_StrategyBasedComWrapp() { _ = StrategyBasedComWrappers.StrategyBasedComWrappers:(); }
    // Purpose: Benchmark native-runtime performance of StrategyBasedComWrappers.ComputeVtables with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.StrategyBasedComWrappers::ComputeVtables:System.Void(P0, P1, P2)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_StrategyBasedComWrappers_ComputeVtables_System() { StrategyBasedComWrappers.ComputeVtables(null!, null!, null!); }
    // Purpose: Benchmark native-runtime performance of StrategyBasedComWrappers.CreateCacheStrategy with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.StrategyBasedComWrappers::CreateCacheStrategy:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_StrategyBasedComWrappers_CreateCacheStrategy_S() { StrategyBasedComWrappers.CreateCacheStrategy(); }
    // Purpose: Benchmark native-runtime performance of StrategyBasedComWrappers.CreateDefaultCacheStrategy with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.StrategyBasedComWrappers::CreateDefaultCacheStrategy:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_StrategyBasedComWrappers_CreateDefaultCacheStr() { StrategyBasedComWrappers.CreateDefaultCacheStrategy(); }
    // Purpose: Benchmark native-runtime performance of StrategyBasedComWrappers.CreateObject with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.StrategyBasedComWrappers::CreateObject:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_StrategyBasedComWrappers_CreateObject_System_V() { StrategyBasedComWrappers.CreateObject(null!, null!); }
    // Purpose: Benchmark native-runtime performance of StrategyBasedComWrappers.CreateObject with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.StrategyBasedComWrappers::CreateObject:System.Void(P0, P1, P2, P3)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_StrategyBasedComWrappers_CreateObject_System_V() { StrategyBasedComWrappers.CreateObject(null!, null!, null!, null!); }
    // Purpose: Benchmark native-runtime performance of StrategyBasedComWrappers.GetOrCreateIUnknownStrategy with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.StrategyBasedComWrappers::GetOrCreateIUnknownStrategy:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_StrategyBasedComWrappers_GetOrCreateIUnknownSt() { StrategyBasedComWrappers.GetOrCreateIUnknownStrategy(); }
    // Purpose: Benchmark native-runtime performance of StrategyBasedComWrappers.GetOrCreateInterfaceDetailsStrategy with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.StrategyBasedComWrappers::GetOrCreateInterfaceDetailsStrategy:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_StrategyBasedComWrappers_GetOrCreateInterfaceD() { StrategyBasedComWrappers.GetOrCreateInterfaceDetailsStrategy(); }
    // Purpose: Benchmark native-runtime performance of StrategyBasedComWrappers.ReleaseObjects with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.StrategyBasedComWrappers::ReleaseObjects:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_StrategyBasedComWrappers_ReleaseObjects_System() { StrategyBasedComWrappers.ReleaseObjects(null!); }
    // Purpose: Benchmark native-runtime performance of StrategyBasedComWrappers.get_DefaultIUnknownInterfaceDetailsStrategy with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.StrategyBasedComWrappers::get_DefaultIUnknownInterfaceDetailsStrategy:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_StrategyBasedComWrappers_get_DefaultIUnknownIn() { StrategyBasedComWrappers.DefaultIUnknownInterfaceDetailsStrategy; }
    // Purpose: Benchmark native-runtime performance of StrategyBasedComWrappers.get_DefaultIUnknownStrategy with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.StrategyBasedComWrappers::get_DefaultIUnknownStrategy:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_StrategyBasedComWrappers_get_DefaultIUnknownSt() { StrategyBasedComWrappers.DefaultIUnknownStrategy; }
    // Purpose: Benchmark native-runtime performance of StrategyBasedComWrappers.set_DefaultIUnknownInterfaceDetailsStrategy with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.StrategyBasedComWrappers::set_DefaultIUnknownInterfaceDetailsStrategy:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_StrategyBasedComWrappers_set_DefaultIUnknownIn() { StrategyBasedComWrappers.DefaultIUnknownInterfaceDetailsStrategy = null!; }
    // Purpose: Benchmark native-runtime performance of StrategyBasedComWrappers.set_DefaultIUnknownStrategy with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.StrategyBasedComWrappers::set_DefaultIUnknownStrategy:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_StrategyBasedComWrappers_set_DefaultIUnknownSt() { StrategyBasedComWrappers.DefaultIUnknownStrategy = null!; }
    // Purpose: Benchmark native-runtime performance of UniqueComInterfaceMarshaller.ConvertToManaged with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.UniqueComInterfaceMarshaller`1::ConvertToManaged:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_UniqueComInterfaceMarshaller_1_ConvertToManage() { UniqueComInterfaceMarshaller.ConvertToManaged(null!); }
    // Purpose: Benchmark native-runtime performance of UniqueComInterfaceMarshaller.ConvertToUnmanaged with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.UniqueComInterfaceMarshaller`1::ConvertToUnmanaged:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_UniqueComInterfaceMarshaller_1_ConvertToUnmana() { UniqueComInterfaceMarshaller.ConvertToUnmanaged(null!); }
    // Purpose: Benchmark native-runtime performance of UniqueComInterfaceMarshaller.Free with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.UniqueComInterfaceMarshaller`1::Free:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_UniqueComInterfaceMarshaller_1_Free_System_Voi() { UniqueComInterfaceMarshaller.Free(null!); }
    // Purpose: Benchmark native-runtime performance of Utf16StringMarshaller.ConvertToManaged with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.Utf16StringMarshaller::ConvertToManaged:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_Utf16StringMarshaller_ConvertToManaged_System_() { Utf16StringMarshaller.ConvertToManaged(null!); }
    // Purpose: Benchmark native-runtime performance of Utf16StringMarshaller.ConvertToUnmanaged with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.Utf16StringMarshaller::ConvertToUnmanaged:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_Utf16StringMarshaller_ConvertToUnmanaged_Syste() { Utf16StringMarshaller.ConvertToUnmanaged(null!); }
    // Purpose: Benchmark native-runtime performance of Utf16StringMarshaller.Free with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.Utf16StringMarshaller::Free:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_Utf16StringMarshaller_Free_System_Void_P0() { Utf16StringMarshaller.Free(null!); }
    // Purpose: Benchmark native-runtime performance of Utf16StringMarshaller.GetPinnableReference with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.Utf16StringMarshaller::GetPinnableReference:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_Utf16StringMarshaller_GetPinnableReference_Sys() { Utf16StringMarshaller.GetPinnableReference(null!); }
    // Purpose: Benchmark native-runtime performance of Utf8StringMarshaller.ConvertToManaged with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.Utf8StringMarshaller::ConvertToManaged:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_Utf8StringMarshaller_ConvertToManaged_System_V() { Utf8StringMarshaller.ConvertToManaged(null!); }
    // Purpose: Benchmark native-runtime performance of Utf8StringMarshaller.ConvertToUnmanaged with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.Utf8StringMarshaller::ConvertToUnmanaged:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_Utf8StringMarshaller_ConvertToUnmanaged_System() { Utf8StringMarshaller.ConvertToUnmanaged(null!); }
    // Purpose: Benchmark native-runtime performance of Utf8StringMarshaller.Free with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.Utf8StringMarshaller::Free:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_Utf8StringMarshaller_Free_System_Void_P0() { Utf8StringMarshaller.Free(null!); }
    // Purpose: Benchmark native-runtime performance of Utf8StringMarshaller+ManagedToUnmanagedIn.Free with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.Utf8StringMarshaller+ManagedToUnmanagedIn::Free:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_Utf8StringMarshaller_ManagedToUnmanagedIn_Free() { Utf8StringMarshaller+ManagedToUnmanagedIn.Free(); }
    // Purpose: Benchmark native-runtime performance of Utf8StringMarshaller+ManagedToUnmanagedIn.FromManaged with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.Utf8StringMarshaller+ManagedToUnmanagedIn::FromManaged:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_Utf8StringMarshaller_ManagedToUnmanagedIn_From() { Utf8StringMarshaller+ManagedToUnmanagedIn.FromManaged(null!, null!); }
    // Purpose: Benchmark native-runtime performance of Utf8StringMarshaller+ManagedToUnmanagedIn.ToUnmanaged with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.Utf8StringMarshaller+ManagedToUnmanagedIn::ToUnmanaged:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_Utf8StringMarshaller_ManagedToUnmanagedIn_ToUn() { Utf8StringMarshaller+ManagedToUnmanagedIn.ToUnmanaged(); }
    // Purpose: Benchmark native-runtime performance of Utf8StringMarshaller+ManagedToUnmanagedIn.get_BufferSize with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.Utf8StringMarshaller+ManagedToUnmanagedIn::get_BufferSize:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_Utf8StringMarshaller_ManagedToUnmanagedIn_get_() { Utf8StringMarshaller+ManagedToUnmanagedIn.BufferSize; }
    // Purpose: Benchmark native-runtime performance of Utf8StringMarshaller+ManagedToUnmanagedIn.set_BufferSize with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.Utf8StringMarshaller+ManagedToUnmanagedIn::set_BufferSize:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_Utf8StringMarshaller_ManagedToUnmanagedIn_set_() { Utf8StringMarshaller+ManagedToUnmanagedIn.BufferSize = null!; }
    // Purpose: Benchmark native-runtime performance of VirtualMethodTableInfo.VirtualMethodTableInfo: with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.VirtualMethodTableInfo::VirtualMethodTableInfo::Void(System.Object, System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_VirtualMethodTableInfo_VirtualMethodTableInfo_() { _ = VirtualMethodTableInfo.VirtualMethodTableInfo:(42, 42); }
    // Purpose: Benchmark native-runtime performance of VirtualMethodTableInfo.Deconstruct with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.VirtualMethodTableInfo::Deconstruct:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_VirtualMethodTableInfo_Deconstruct_System_Void() { VirtualMethodTableInfo.Deconstruct(null!, null!); }
    // Purpose: Benchmark native-runtime performance of VirtualMethodTableInfo.get_ThisPointer with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.VirtualMethodTableInfo::get_ThisPointer:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_VirtualMethodTableInfo_get_ThisPointer_System_() { VirtualMethodTableInfo.ThisPointer; }
    // Purpose: Benchmark native-runtime performance of VirtualMethodTableInfo.get_VirtualMethodTable with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.VirtualMethodTableInfo::get_VirtualMethodTable:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_VirtualMethodTableInfo_get_VirtualMethodTable_() { VirtualMethodTableInfo.VirtualMethodTable; }
    // Purpose: Benchmark native-runtime performance of VirtualMethodTableInfo.set_ThisPointer with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.VirtualMethodTableInfo::set_ThisPointer:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_VirtualMethodTableInfo_set_ThisPointer_System_() { VirtualMethodTableInfo.ThisPointer = null!; }
    // Purpose: Benchmark native-runtime performance of VirtualMethodTableInfo.set_VirtualMethodTable with typical input
    [BenchmarkSubjectId("System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.VirtualMethodTableInfo::set_VirtualMethodTable:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Runtime_InteropServices_System_Runtime_InteropServices_Marshalling_VirtualMethodTableInfo_set_VirtualMethodTable_() { VirtualMethodTableInfo.VirtualMethodTable = null!; }}
