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

// Auto-generated skeleton for Array Indexing And Copy (family/System.Private.CoreLib/array/indexing-copy).
// Contains test, benchmark, and hotupdate methods merged into one partial class.
// No xunit dependency — [Fact] attributes belong in the test exe project.
public partial class ArrayIndexingCopy
{
    // Array.Copy
    [MethodSubjectId("System.Private.CoreLib/System.Array::Copy:System.Void(System.Array,System.Array,System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ArrayIndexingCopy)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Private_CoreLib_System_Array_Copy_System_Void_System_Array_System_Array_System_Int32()
    {
    Array.Copy(new byte[4], new byte[4], 4);
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.Array::Copy:System.Void(System.Array,System.Array,System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ArrayIndexingCopy)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Array_Copy_System_Void_System_Array_System_Array_System_Int32() { Array.Copy(new byte[4], new byte[4], 4); }

    [HotUpdateSubjectId("System.Private.CoreLib/System.Array::Copy:System.Void(System.Array,System.Array,System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ArrayIndexingCopy)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Array_Copy_System_Void_System_Array_System_Array_System_Int32() { Array.Copy(new byte[4], new byte[4], 4); }

    // Array.Copy
    [MethodSubjectId("System.Private.CoreLib/System.Array::Copy:System.Void(System.Array,System.Int32,System.Array,System.Int32,System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ArrayIndexingCopy)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Private_CoreLib_System_Array_Copy_System_Void_System_Array_System_Int32_System_Array_System_Int32_System_Int32()
    {
    Array.Copy(new byte[4], 0, new byte[4], 0, 4);
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.Array::Copy:System.Void(System.Array,System.Int32,System.Array,System.Int32,System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ArrayIndexingCopy)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Array_Copy_System_Void_System_Array_System_Int32_System_Array_System_Int32_System_Int32() { Array.Copy(new byte[4], 0, new byte[4], 0, 4); }

    [HotUpdateSubjectId("System.Private.CoreLib/System.Array::Copy:System.Void(System.Array,System.Int32,System.Array,System.Int32,System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ArrayIndexingCopy)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Array_Copy_System_Void_System_Array_System_Int32_System_Array_System_Int32_System_Int32() { Array.Copy(new byte[4], 0, new byte[4], 0, 4); }

    // Array.Clear
    [MethodSubjectId("System.Private.CoreLib/System.Array::Clear:System.Void(System.Array,System.Int32,System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ArrayIndexingCopy)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Private_CoreLib_System_Array_Clear_System_Void_System_Array_System_Int32_System_Int32()
    {
    Array.Clear(new byte[4], 0, 4);
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.Array::Clear:System.Void(System.Array,System.Int32,System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ArrayIndexingCopy)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Array_Clear_System_Void_System_Array_System_Int32_System_Int32() { Array.Clear(new byte[4], 0, 4); }

    [HotUpdateSubjectId("System.Private.CoreLib/System.Array::Clear:System.Void(System.Array,System.Int32,System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ArrayIndexingCopy)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Array_Clear_System_Void_System_Array_System_Int32_System_Int32() { Array.Clear(new byte[4], 0, 4); }

    // Array.Resize
    [MethodSubjectId("System.Private.CoreLib/System.Array::Resize:System.Void(T[]&,System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ArrayIndexingCopy)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Private_CoreLib_System_Array_Resize_System_Void_T_System_Int32()
    {
    // TODO: needs-manual — Resize with 2 params requires manual implementation
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.Array::Resize:System.Void(T[]&,System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ArrayIndexingCopy)]
    public static void Benchmark_System_Private_CoreLib_System_Array_Resize_System_Void_T_System_Int32() { }

    [HotUpdateSubjectId("System.Private.CoreLib/System.Array::Resize:System.Void(T[]&,System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ArrayIndexingCopy)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Array_Resize_System_Void_T_System_Int32() { }

    // Array.Sort
    [MethodSubjectId("System.Private.CoreLib/System.Array::Sort:System.Void(System.Array)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ArrayIndexingCopy)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Private_CoreLib_System_Array_Sort_System_Void_System_Array()
    {
    Array.Sort(Array.Empty<byte>());
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.Array::Sort:System.Void(System.Array)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ArrayIndexingCopy)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Array_Sort_System_Void_System_Array() { Array.Sort(Array.Empty<byte>()); }

    [HotUpdateSubjectId("System.Private.CoreLib/System.Array::Sort:System.Void(System.Array)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ArrayIndexingCopy)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Array_Sort_System_Void_System_Array() { Array.Sort(Array.Empty<byte>()); }

    // Array.Sort
    [MethodSubjectId("System.Private.CoreLib/System.Array::Sort:System.Void(System.Array,System.Collections.IComparer)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ArrayIndexingCopy)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Private_CoreLib_System_Array_Sort_System_Void_System_Array_System_Collections_IComparer()
    {
    Array.Sort<byte>(new byte[1], (IComparer<byte>)null!);
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.Array::Sort:System.Void(System.Array,System.Collections.IComparer)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ArrayIndexingCopy)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Array_Sort_System_Void_System_Array_System_Collections_IComparer() { Array.Sort<byte>(new byte[1], (IComparer<byte>)null!); }

    [HotUpdateSubjectId("System.Private.CoreLib/System.Array::Sort:System.Void(System.Array,System.Collections.IComparer)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ArrayIndexingCopy)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Array_Sort_System_Void_System_Array_System_Collections_IComparer() { Array.Sort<byte>(new byte[1], (IComparer<byte>)null!); }

    // Array.BinarySearch
    [MethodSubjectId("System.Private.CoreLib/System.Array::BinarySearch:System.Int32(System.Array,System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ArrayIndexingCopy)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Private_CoreLib_System_Array_BinarySearch_System_Int32_System_Array_System_Object()
    {
    var result = Array.BinarySearch(new byte[4], (byte)42);
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.Array::BinarySearch:System.Int32(System.Array,System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ArrayIndexingCopy)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Array_BinarySearch_System_Int32_System_Array_System_Object() { _ = Array.BinarySearch(new byte[4], (byte)42); }

    [HotUpdateSubjectId("System.Private.CoreLib/System.Array::BinarySearch:System.Int32(System.Array,System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ArrayIndexingCopy)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Array_BinarySearch_System_Int32_System_Array_System_Object() { _ = Array.BinarySearch(new byte[4], (byte)42); }

    // Array.BinarySearch
    [MethodSubjectId("System.Private.CoreLib/System.Array::BinarySearch:System.Int32(System.Array,System.Int32,System.Int32,System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ArrayIndexingCopy)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Private_CoreLib_System_Array_BinarySearch_System_Int32_System_Array_System_Int32_System_Int32_System_Object()
    {
    var result = Array.BinarySearch(new byte[4], 0, 4, (byte)42);
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.Array::BinarySearch:System.Int32(System.Array,System.Int32,System.Int32,System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ArrayIndexingCopy)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Array_BinarySearch_System_Int32_System_Array_System_Int32_System_Int32_System_Object() { _ = Array.BinarySearch(new byte[4], 0, 4, (byte)42); }

    [HotUpdateSubjectId("System.Private.CoreLib/System.Array::BinarySearch:System.Int32(System.Array,System.Int32,System.Int32,System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ArrayIndexingCopy)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Array_BinarySearch_System_Int32_System_Array_System_Int32_System_Int32_System_Object() { _ = Array.BinarySearch(new byte[4], 0, 4, (byte)42); }

    // Array.IndexOf
    [MethodSubjectId("System.Private.CoreLib/System.Array::IndexOf:System.Int32(System.Array,System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ArrayIndexingCopy)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Private_CoreLib_System_Array_IndexOf_System_Int32_System_Array_System_Object()
    {
    var result = Array.IndexOf(new byte[4], (byte)42);
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.Array::IndexOf:System.Int32(System.Array,System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ArrayIndexingCopy)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Array_IndexOf_System_Int32_System_Array_System_Object() { _ = Array.IndexOf(new byte[4], (byte)42); }

    [HotUpdateSubjectId("System.Private.CoreLib/System.Array::IndexOf:System.Int32(System.Array,System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ArrayIndexingCopy)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Array_IndexOf_System_Int32_System_Array_System_Object() { _ = Array.IndexOf(new byte[4], (byte)42); }

    // Array.LastIndexOf
    [MethodSubjectId("System.Private.CoreLib/System.Array::LastIndexOf:System.Int32(System.Array,System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ArrayIndexingCopy)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Private_CoreLib_System_Array_LastIndexOf_System_Int32_System_Array_System_Object()
    {
    var result = Array.LastIndexOf(new byte[4], (byte)42);
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.Array::LastIndexOf:System.Int32(System.Array,System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ArrayIndexingCopy)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Array_LastIndexOf_System_Int32_System_Array_System_Object() { _ = Array.LastIndexOf(new byte[4], (byte)42); }

    [HotUpdateSubjectId("System.Private.CoreLib/System.Array::LastIndexOf:System.Int32(System.Array,System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ArrayIndexingCopy)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Array_LastIndexOf_System_Int32_System_Array_System_Object() { _ = Array.LastIndexOf(new byte[4], (byte)42); }

    // Array.Reverse
    [MethodSubjectId("System.Private.CoreLib/System.Array::Reverse:System.Void(System.Array)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ArrayIndexingCopy)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Private_CoreLib_System_Array_Reverse_System_Void_System_Array()
    {
    Array.Reverse(Array.Empty<byte>());
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.Array::Reverse:System.Void(System.Array)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ArrayIndexingCopy)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Array_Reverse_System_Void_System_Array() { Array.Reverse(Array.Empty<byte>()); }

    [HotUpdateSubjectId("System.Private.CoreLib/System.Array::Reverse:System.Void(System.Array)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ArrayIndexingCopy)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Array_Reverse_System_Void_System_Array() { Array.Reverse(Array.Empty<byte>()); }

    // Array.GetLength
    [MethodSubjectId("System.Private.CoreLib/System.Array::GetLength:System.Int32(System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ArrayIndexingCopy)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Private_CoreLib_System_Array_GetLength_System_Int32_System_Int32()
    {
    var result = new int[4, 4].GetLength(0);
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.Array::GetLength:System.Int32(System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ArrayIndexingCopy)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Array_GetLength_System_Int32_System_Int32() { _ = new int[4, 4].GetLength(0); }

    [HotUpdateSubjectId("System.Private.CoreLib/System.Array::GetLength:System.Int32(System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ArrayIndexingCopy)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Array_GetLength_System_Int32_System_Int32() { _ = new int[4, 4].GetLength(0); }

    // Array.GetValue
    [MethodSubjectId("System.Private.CoreLib/System.Array::GetValue:System.Object(System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ArrayIndexingCopy)]
    [VerificationRoute(VerificationRoute.Native)]
    public void Method_System_Private_CoreLib_System_Array_GetValue_System_Object_System_Int32()
    {
    var result = new int[1].GetValue(0);
    }

    [BenchmarkSubjectId("System.Private.CoreLib/System.Array::GetValue:System.Object(System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ArrayIndexingCopy)]
    [BenchmarkRoute(BenchmarkRoute.Native)]
    [BenchmarkProfile(BenchmarkProfile.Default)]
    public static void Benchmark_System_Private_CoreLib_System_Array_GetValue_System_Object_System_Int32() { _ = new int[1].GetValue(0); }

    [HotUpdateSubjectId("System.Private.CoreLib/System.Array::GetValue:System.Object(System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ArrayIndexingCopy)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Array_GetValue_System_Object_System_Int32() { _ = new int[1].GetValue(0); }
}
