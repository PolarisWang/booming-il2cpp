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

// Auto-generated hot-update skeletons for FamilySystem.net.serversenteventsParserHotUpdate.
// Each method exercises a method from the HostToPatch direction.
public static class FamilySystem.net.serversenteventsParserHotUpdate
{
    // Purpose: Verify SseParser.SseParser: executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Net.ServerSentEvents/System.Net.ServerSentEvents.SseParser::SseParser::Void(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseParser_SseParser_Void_System_Object() { _ = SseParser.SseParser:(42); }
    // Purpose: Verify SseParser.get_Property executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Net.ServerSentEvents/System.Net.ServerSentEvents.SseParser::get_Property:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseParser_get_Property_System_Void() { SseParser.Property; }
    // Purpose: Verify SseParser.Method1 executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Net.ServerSentEvents/System.Net.ServerSentEvents.SseParser::Method1:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseParser_Method1_System_Void_P0() { SseParser.Method1(null!); }
    // Purpose: Verify SseParser.Method2 executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Net.ServerSentEvents/System.Net.ServerSentEvents.SseParser::Method2:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseParser_Method2_System_Void_P0_P1() { SseParser.Method2(null!, null!); }
    // Purpose: Verify SseParser.Validate executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Net.ServerSentEvents/System.Net.ServerSentEvents.SseParser::Validate:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseParser_Validate_System_Void_P0() { SseParser.Validate(null!); }
    // Purpose: Verify SseParser.get_Value executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Net.ServerSentEvents/System.Net.ServerSentEvents.SseParser::get_Value:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseParser_get_Value_System_Void() { SseParser.Value; }
    // Purpose: Verify SseParser.set_Value executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Net.ServerSentEvents/System.Net.ServerSentEvents.SseParser::set_Value:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseParser_set_Value_System_Void_P0() { SseParser.Value = null!; }
    // Purpose: Verify SseParser.get_Count executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Net.ServerSentEvents/System.Net.ServerSentEvents.SseParser::get_Count:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseParser_get_Count_System_Void() { SseParser.Count; }
    // Purpose: Verify SseParser.set_Count executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Net.ServerSentEvents/System.Net.ServerSentEvents.SseParser::set_Count:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseParser_set_Count_System_Void_P0() { SseParser.Count = null!; }
    // Purpose: Verify SseParser.get_IsEmpty executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Net.ServerSentEvents/System.Net.ServerSentEvents.SseParser::get_IsEmpty:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseParser_get_IsEmpty_System_Void() { SseParser.IsEmpty; }
    // Purpose: Verify SseParser.set_IsEmpty executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Net.ServerSentEvents/System.Net.ServerSentEvents.SseParser::set_IsEmpty:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseParser_set_IsEmpty_System_Void_P0() { SseParser.IsEmpty = null!; }
    // Purpose: Verify SseParser.get_Key executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Net.ServerSentEvents/System.Net.ServerSentEvents.SseParser::get_Key:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseParser_get_Key_System_Void() { SseParser.Key; }
    // Purpose: Verify SseParser.set_Key executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Net.ServerSentEvents/System.Net.ServerSentEvents.SseParser::set_Key:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Net_ServerSentEvents_System_Net_ServerSentEvents_SseParser_set_Key_System_Void_P0() { SseParser.Key = null!; }}
