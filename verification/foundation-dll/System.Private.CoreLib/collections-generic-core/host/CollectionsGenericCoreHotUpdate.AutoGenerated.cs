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

// Auto-generated hot-update skeletons for CollectionsGenericCoreHotUpdate.
// Each method exercises a method from the HostToPatch direction.
public static class CollectionsGenericCoreHotUpdate
{
    // Purpose: Verify List.Add executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Collections.Generic.List`1::Add:System.Void(T)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_CollectionsGenericCore)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Collections_Generic_List_1_Add_System_Void_T() { }
    // Purpose: Verify List.Clear executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Collections.Generic.List`1::Clear:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_CollectionsGenericCore)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Collections_Generic_List_1_Clear_System_Void() { new List<int>().Clear(); }
    // Purpose: Verify List.Contains executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Collections.Generic.List`1::Contains:System.Boolean(T)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_CollectionsGenericCore)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Collections_Generic_List_1_Contains_System_Boolean_T() { }
    // Purpose: Verify List.IndexOf executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Collections.Generic.List`1::IndexOf:System.Int32(T)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_CollectionsGenericCore)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Collections_Generic_List_1_IndexOf_System_Int32_T() { }
    // Purpose: Verify List.Remove executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Collections.Generic.List`1::Remove:System.Boolean(T)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_CollectionsGenericCore)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Collections_Generic_List_1_Remove_System_Boolean_T() { }
    // Purpose: Verify List.RemoveAt executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Collections.Generic.List`1::RemoveAt:System.Void(System.Int32)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_CollectionsGenericCore)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Collections_Generic_List_1_RemoveAt_System_Void_System_Int32() { }
    // Purpose: Verify List.Sort executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Collections.Generic.List`1::Sort:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_CollectionsGenericCore)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Collections_Generic_List_1_Sort_System_Void() { new List<int>().Sort(); }
    // Purpose: Verify List.ToArray executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Collections.Generic.List`1::ToArray:T[]()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_CollectionsGenericCore)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Collections_Generic_List_1_ToArray_T() { _ = new List<int>().ToArray(); }
    // Purpose: Verify Dictionary.Add executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Collections.Generic.Dictionary`2::Add:System.Void(TKey,TValue)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_CollectionsGenericCore)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Collections_Generic_Dictionary_2_Add_System_Void_TKey_TValue() { }
    // Purpose: Verify Dictionary.get_Count executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Collections.Generic.Dictionary`2::get_Count:System.Int32()")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_CollectionsGenericCore)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Collections_Generic_Dictionary_2_get_Count_System_Int32() { _ = new Dictionary<string, int>().Count; }
    // Purpose: Verify Dictionary.TryGetValue executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Collections.Generic.Dictionary`2::TryGetValue:System.Boolean(TKey,TValue&)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_CollectionsGenericCore)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Collections_Generic_Dictionary_2_TryGetValue_System_Boolean_TKey_TValue() { }
    // Purpose: Verify Dictionary.ContainsKey executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Collections.Generic.Dictionary`2::ContainsKey:System.Boolean(TKey)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_CollectionsGenericCore)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Collections_Generic_Dictionary_2_ContainsKey_System_Boolean_TKey() { }
    // Purpose: Verify Dictionary.Remove executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Collections.Generic.Dictionary`2::Remove:System.Boolean(TKey)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_CollectionsGenericCore)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Collections_Generic_Dictionary_2_Remove_System_Boolean_TKey() { }
    // Purpose: Verify HashSet.Add executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Collections.Generic.HashSet`1::Add:System.Boolean(T)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_CollectionsGenericCore)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Collections_Generic_HashSet_1_Add_System_Boolean_T() { }
    // Purpose: Verify HashSet.Contains executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Collections.Generic.HashSet`1::Contains:System.Boolean(T)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_CollectionsGenericCore)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Collections_Generic_HashSet_1_Contains_System_Boolean_T() { }
    // Purpose: Verify HashSet.Remove executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Private.CoreLib/System.Collections.Generic.HashSet`1::Remove:System.Boolean(T)")]
    [CapabilityFamilyId(CapabilityFamilyId.SystemPrivateCoreLib_CollectionsGenericCore)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Private_CoreLib_System_Collections_Generic_HashSet_1_Remove_System_Boolean_T() { }}
