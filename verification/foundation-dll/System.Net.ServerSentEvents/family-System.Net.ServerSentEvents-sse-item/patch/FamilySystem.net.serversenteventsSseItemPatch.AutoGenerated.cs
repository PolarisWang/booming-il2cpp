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

// Auto-generated patch-side skeletons for FamilySystem.net.serversenteventsSseItemPatch.
// Each method exercises a method from the patch side back to the host after hot-update.
public static class FamilySystem.net.serversenteventsSseItemPatch
{
    // Purpose: Verify SseItem.SseItem: executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Net.ServerSentEvents/System.Net.ServerSentEvents.SseItem::SseItem::Void(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseItem_SseItem_Void_System_Object() { _ = SseItem.SseItem:(42); }
    // Purpose: Verify SseItem.get_Property executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Net.ServerSentEvents/System.Net.ServerSentEvents.SseItem::get_Property:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseItem_get_Property_System_Void() { SseItem.Property; }
    // Purpose: Verify SseItem.Method1 executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Net.ServerSentEvents/System.Net.ServerSentEvents.SseItem::Method1:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseItem_Method1_System_Void_P0() { SseItem.Method1(null!); }
    // Purpose: Verify SseItem.Method2 executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Net.ServerSentEvents/System.Net.ServerSentEvents.SseItem::Method2:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseItem_Method2_System_Void_P0_P1() { SseItem.Method2(null!, null!); }
    // Purpose: Verify SseItem.Validate executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Net.ServerSentEvents/System.Net.ServerSentEvents.SseItem::Validate:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseItem_Validate_System_Void_P0() { SseItem.Validate(null!); }
    // Purpose: Verify SseItem.get_Value executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Net.ServerSentEvents/System.Net.ServerSentEvents.SseItem::get_Value:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseItem_get_Value_System_Void() { SseItem.Value; }
    // Purpose: Verify SseItem.set_Value executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Net.ServerSentEvents/System.Net.ServerSentEvents.SseItem::set_Value:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseItem_set_Value_System_Void_P0() { SseItem.Value = null!; }
    // Purpose: Verify SseItem.get_Count executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Net.ServerSentEvents/System.Net.ServerSentEvents.SseItem::get_Count:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseItem_get_Count_System_Void() { SseItem.Count; }
    // Purpose: Verify SseItem.set_Count executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Net.ServerSentEvents/System.Net.ServerSentEvents.SseItem::set_Count:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseItem_set_Count_System_Void_P0() { SseItem.Count = null!; }
    // Purpose: Verify SseItem.get_IsEmpty executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Net.ServerSentEvents/System.Net.ServerSentEvents.SseItem::get_IsEmpty:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseItem_get_IsEmpty_System_Void() { SseItem.IsEmpty; }
    // Purpose: Verify SseItem.set_IsEmpty executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Net.ServerSentEvents/System.Net.ServerSentEvents.SseItem::set_IsEmpty:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseItem_set_IsEmpty_System_Void_P0() { SseItem.IsEmpty = null!; }
    // Purpose: Verify SseItem.get_Key executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Net.ServerSentEvents/System.Net.ServerSentEvents.SseItem::get_Key:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseItem_get_Key_System_Void() { SseItem.Key; }
    // Purpose: Verify SseItem.set_Key executes correctly from patch side back to host
    [HotUpdateSubjectId("System.Net.ServerSentEvents/System.Net.ServerSentEvents.SseItem::set_Key:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseItem_set_Key_System_Void_P0() { SseItem.Key = null!; }}
