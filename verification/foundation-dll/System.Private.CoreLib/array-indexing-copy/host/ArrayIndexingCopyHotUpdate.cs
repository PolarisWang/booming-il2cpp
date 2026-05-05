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

// Hot-update stubs for family/System.Private.CoreLib/array/indexing-copy
public static class ArrayIndexingCopyHotUpdate
{
    // Verify Array.Copy after hot-update (patch side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Array::Copy:System.Void(System.Array,System.Array,System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ArrayIndexingCopy)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Array_Copy_System_Void_System_Array_System_Array_System_Int32() { Array.Copy(new byte[4], new byte[4], 4); }
    // Verify Array.Copy after hot-update (patch side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Array::Copy:System.Void(System.Array,System.Int32,System.Array,System.Int32,System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ArrayIndexingCopy)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Array_Copy_System_Void_System_Array_System_Int32_System_Array_System_Int32_System_Int32() { Array.Copy(new byte[4], 0, new byte[4], 0, 4); }
    // Verify Array.Clear after hot-update (patch side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Array::Clear:System.Void(System.Array,System.Int32,System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ArrayIndexingCopy)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Array_Clear_System_Void_System_Array_System_Int32_System_Int32() { Array.Clear(new byte[4], 0, 4); }
    // Verify Array.Resize after hot-update (patch side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Array::Resize:System.Void(T[]&,System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ArrayIndexingCopy)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Array_Resize_System_Void_T_System_Int32() { }
    // Verify Array.Sort after hot-update (patch side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Array::Sort:System.Void(System.Array)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ArrayIndexingCopy)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Array_Sort_System_Void_System_Array() { Array.Sort(Array.Empty<byte>()); }
    // Verify Array.Sort after hot-update (patch side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Array::Sort:System.Void(System.Array,System.Collections.IComparer)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ArrayIndexingCopy)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Array_Sort_System_Void_System_Array_System_Collections_IComparer() { Array.Sort<byte>(new byte[1], (IComparer<byte>)null!); }
    // Verify Array.BinarySearch after hot-update (patch side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Array::BinarySearch:System.Int32(System.Array,System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ArrayIndexingCopy)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Array_BinarySearch_System_Int32_System_Array_System_Object() { _ = Array.BinarySearch(new byte[4], (byte)42); }
    // Verify Array.BinarySearch after hot-update (patch side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Array::BinarySearch:System.Int32(System.Array,System.Int32,System.Int32,System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ArrayIndexingCopy)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Array_BinarySearch_System_Int32_System_Array_System_Int32_System_Int32_System_Object() { _ = Array.BinarySearch(new byte[4], 0, 4, (byte)42); }
    // Verify Array.IndexOf after hot-update (patch side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Array::IndexOf:System.Int32(System.Array,System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ArrayIndexingCopy)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Array_IndexOf_System_Int32_System_Array_System_Object() { _ = Array.IndexOf(new byte[4], (byte)42); }
    // Verify Array.LastIndexOf after hot-update (patch side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Array::LastIndexOf:System.Int32(System.Array,System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ArrayIndexingCopy)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Array_LastIndexOf_System_Int32_System_Array_System_Object() { _ = Array.LastIndexOf(new byte[4], (byte)42); }
    // Verify Array.Reverse after hot-update (patch side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Array::Reverse:System.Void(System.Array)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ArrayIndexingCopy)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Array_Reverse_System_Void_System_Array() { Array.Reverse(Array.Empty<byte>()); }
    // Verify Array.GetLength after hot-update (patch side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Array::GetLength:System.Int32(System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ArrayIndexingCopy)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Array_GetLength_System_Int32_System_Int32() { _ = new int[4, 4].GetLength(0); }
    // Verify Array.GetValue after hot-update (patch side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Array::GetValue:System.Object(System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_ArrayIndexingCopy)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Array_GetValue_System_Object_System_Int32() { _ = new int[1].GetValue(0); }}
