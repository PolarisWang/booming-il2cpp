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

// Auto-generated hot-update skeletons for FamilySystem.collections.immutableImmutableQueueStackHotUpdate.
// Each method exercises a method from the HostToPatch direction.
public static class FamilySystem.collections.immutableImmutableQueueStackHotUpdate
{
    // Purpose: Verify ImmutableQueue.ImmutableQueue: executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableQueue::ImmutableQueue::Void(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Collections_Immutable_System_Collections_Immutable_ImmutableQueue_ImmutableQueue_Void_System_Object() { _ = ImmutableQueue.ImmutableQueue:(42); }
    // Purpose: Verify ImmutableQueue.get_Property executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableQueue::get_Property:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Collections_Immutable_System_Collections_Immutable_ImmutableQueue_get_Property_System_Void() { ImmutableQueue.Property; }
    // Purpose: Verify ImmutableQueue.Method1 executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableQueue::Method1:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Collections_Immutable_System_Collections_Immutable_ImmutableQueue_Method1_System_Void_P0() { ImmutableQueue.Method1(null!); }
    // Purpose: Verify ImmutableQueue.Method2 executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableQueue::Method2:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Collections_Immutable_System_Collections_Immutable_ImmutableQueue_Method2_System_Void_P0_P1() { ImmutableQueue.Method2(null!, null!); }
    // Purpose: Verify ImmutableQueue.Validate executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableQueue::Validate:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Collections_Immutable_System_Collections_Immutable_ImmutableQueue_Validate_System_Void_P0() { ImmutableQueue.Validate(null!); }
    // Purpose: Verify ImmutableStack.ImmutableStack: executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableStack::ImmutableStack::Void(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Collections_Immutable_System_Collections_Immutable_ImmutableStack_ImmutableStack_Void_System_Object() { _ = ImmutableStack.ImmutableStack:(42); }
    // Purpose: Verify ImmutableStack.get_Property executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableStack::get_Property:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Collections_Immutable_System_Collections_Immutable_ImmutableStack_get_Property_System_Void() { ImmutableStack.Property; }
    // Purpose: Verify ImmutableStack.Method1 executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableStack::Method1:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Collections_Immutable_System_Collections_Immutable_ImmutableStack_Method1_System_Void_P0() { ImmutableStack.Method1(null!); }
    // Purpose: Verify ImmutableStack.Method2 executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableStack::Method2:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Collections_Immutable_System_Collections_Immutable_ImmutableStack_Method2_System_Void_P0_P1() { ImmutableStack.Method2(null!, null!); }
    // Purpose: Verify ImmutableStack.Validate executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableStack::Validate:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Collections_Immutable_System_Collections_Immutable_ImmutableStack_Validate_System_Void_P0() { ImmutableStack.Validate(null!); }
    // Purpose: Verify ImmutableQueue.get_Value executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableQueue::get_Value:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Collections_Immutable_System_Collections_Immutable_ImmutableQueue_get_Value_System_Void() { ImmutableQueue.Value; }
    // Purpose: Verify ImmutableQueue.set_Value executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableQueue::set_Value:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Collections_Immutable_System_Collections_Immutable_ImmutableQueue_set_Value_System_Void_P0() { ImmutableQueue.Value = null!; }
    // Purpose: Verify ImmutableQueue.get_Count executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableQueue::get_Count:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Collections_Immutable_System_Collections_Immutable_ImmutableQueue_get_Count_System_Void() { ImmutableQueue.Count; }
    // Purpose: Verify ImmutableQueue.set_Count executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableQueue::set_Count:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Collections_Immutable_System_Collections_Immutable_ImmutableQueue_set_Count_System_Void_P0() { ImmutableQueue.Count = null!; }
    // Purpose: Verify ImmutableQueue.get_IsEmpty executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableQueue::get_IsEmpty:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Collections_Immutable_System_Collections_Immutable_ImmutableQueue_get_IsEmpty_System_Void() { ImmutableQueue.IsEmpty; }
    // Purpose: Verify ImmutableQueue.set_IsEmpty executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableQueue::set_IsEmpty:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Collections_Immutable_System_Collections_Immutable_ImmutableQueue_set_IsEmpty_System_Void_P0() { ImmutableQueue.IsEmpty = null!; }
    // Purpose: Verify ImmutableQueue.get_Key executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableQueue::get_Key:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Collections_Immutable_System_Collections_Immutable_ImmutableQueue_get_Key_System_Void() { ImmutableQueue.Key; }
    // Purpose: Verify ImmutableQueue.set_Key executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableQueue::set_Key:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Collections_Immutable_System_Collections_Immutable_ImmutableQueue_set_Key_System_Void_P0() { ImmutableQueue.Key = null!; }
    // Purpose: Verify ImmutableStack.get_Value executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableStack::get_Value:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Collections_Immutable_System_Collections_Immutable_ImmutableStack_get_Value_System_Void() { ImmutableStack.Value; }
    // Purpose: Verify ImmutableStack.set_Value executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableStack::set_Value:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Collections_Immutable_System_Collections_Immutable_ImmutableStack_set_Value_System_Void_P0() { ImmutableStack.Value = null!; }
    // Purpose: Verify ImmutableStack.get_Count executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableStack::get_Count:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Collections_Immutable_System_Collections_Immutable_ImmutableStack_get_Count_System_Void() { ImmutableStack.Count; }
    // Purpose: Verify ImmutableStack.set_Count executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableStack::set_Count:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Collections_Immutable_System_Collections_Immutable_ImmutableStack_set_Count_System_Void_P0() { ImmutableStack.Count = null!; }
    // Purpose: Verify ImmutableStack.get_IsEmpty executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableStack::get_IsEmpty:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Collections_Immutable_System_Collections_Immutable_ImmutableStack_get_IsEmpty_System_Void() { ImmutableStack.IsEmpty; }
    // Purpose: Verify ImmutableStack.set_IsEmpty executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableStack::set_IsEmpty:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Collections_Immutable_System_Collections_Immutable_ImmutableStack_set_IsEmpty_System_Void_P0() { ImmutableStack.IsEmpty = null!; }
    // Purpose: Verify ImmutableStack.get_Key executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableStack::get_Key:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Collections_Immutable_System_Collections_Immutable_ImmutableStack_get_Key_System_Void() { ImmutableStack.Key; }
    // Purpose: Verify ImmutableStack.set_Key executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.Collections.Immutable/System.Collections.Immutable.ImmutableStack::set_Key:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_Collections_Immutable_System_Collections_Immutable_ImmutableStack_set_Key_System_Void_P0() { ImmutableStack.Key = null!; }}
