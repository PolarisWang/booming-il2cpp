using System;
using System.IO;
using System.Threading;
using System.Threading.Tasks;
using System.Globalization;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Runtime.InteropServices;
using System.Collections.Generic;
using System.Reflection;
using Chaos.TestFramework;

// Hot-update stubs for family/System.Private.CoreLib/collections/generic-core
public static class CollectionsGenericCoreHotUpdate
{
    // Verify List.Add after hot-update (patch side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Collections.Generic.List`1::Add:System.Void(T)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_CollectionsGenericCore)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Collections_Generic_List_1_Add_System_Void_T() { }
    // Verify List.Clear after hot-update (patch side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Collections.Generic.List`1::Clear:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_CollectionsGenericCore)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Collections_Generic_List_1_Clear_System_Void() { new List<int>().Clear(); }
    // Verify List.Contains after hot-update (patch side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Collections.Generic.List`1::Contains:System.Boolean(T)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_CollectionsGenericCore)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Collections_Generic_List_1_Contains_System_Boolean_T() { }
    // Verify List.IndexOf after hot-update (patch side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Collections.Generic.List`1::IndexOf:System.Int32(T)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_CollectionsGenericCore)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Collections_Generic_List_1_IndexOf_System_Int32_T() { }
    // Verify List.Remove after hot-update (patch side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Collections.Generic.List`1::Remove:System.Boolean(T)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_CollectionsGenericCore)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Collections_Generic_List_1_Remove_System_Boolean_T() { }
    // Verify List.RemoveAt after hot-update (patch side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Collections.Generic.List`1::RemoveAt:System.Void(System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_CollectionsGenericCore)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Collections_Generic_List_1_RemoveAt_System_Void_System_Int32() { new System.Collections.Generic.List<int>{1,2,3}.RemoveAt(1); }
    // Verify List.Sort after hot-update (patch side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Collections.Generic.List`1::Sort:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_CollectionsGenericCore)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Collections_Generic_List_1_Sort_System_Void() { new List<int>().Sort(); }
    // Verify List.ToArray after hot-update (patch side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Collections.Generic.List`1::ToArray:T[]()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_CollectionsGenericCore)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Collections_Generic_List_1_ToArray_T() { _ = new List<int>().ToArray(); }
    // Verify Dictionary.Add after hot-update (patch side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Collections.Generic.Dictionary`2::Add:System.Void(TKey,TValue)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_CollectionsGenericCore)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Collections_Generic_Dictionary_2_Add_System_Void_TKey_TValue() { }
    // Verify Dictionary.get_Count after hot-update (patch side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Collections.Generic.Dictionary`2::get_Count:System.Int32()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_CollectionsGenericCore)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Collections_Generic_Dictionary_2_get_Count_System_Int32() { _ = new Dictionary<string, int>().Count; }
    // Verify Dictionary.TryGetValue after hot-update (patch side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Collections.Generic.Dictionary`2::TryGetValue:System.Boolean(TKey,TValue&)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_CollectionsGenericCore)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Collections_Generic_Dictionary_2_TryGetValue_System_Boolean_TKey_TValue() { }
    // Verify Dictionary.ContainsKey after hot-update (patch side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Collections.Generic.Dictionary`2::ContainsKey:System.Boolean(TKey)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_CollectionsGenericCore)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Collections_Generic_Dictionary_2_ContainsKey_System_Boolean_TKey() { }
    // Verify Dictionary.Remove after hot-update (patch side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Collections.Generic.Dictionary`2::Remove:System.Boolean(TKey)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_CollectionsGenericCore)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Collections_Generic_Dictionary_2_Remove_System_Boolean_TKey() { }
    // Verify HashSet.Add after hot-update (patch side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Collections.Generic.HashSet`1::Add:System.Boolean(T)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_CollectionsGenericCore)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Collections_Generic_HashSet_1_Add_System_Boolean_T() { }
    // Verify HashSet.Contains after hot-update (patch side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Collections.Generic.HashSet`1::Contains:System.Boolean(T)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_CollectionsGenericCore)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Collections_Generic_HashSet_1_Contains_System_Boolean_T() { }
    // Verify HashSet.Remove after hot-update (patch side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Collections.Generic.HashSet`1::Remove:System.Boolean(T)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_CollectionsGenericCore)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Collections_Generic_HashSet_1_Remove_System_Boolean_T() { }}
