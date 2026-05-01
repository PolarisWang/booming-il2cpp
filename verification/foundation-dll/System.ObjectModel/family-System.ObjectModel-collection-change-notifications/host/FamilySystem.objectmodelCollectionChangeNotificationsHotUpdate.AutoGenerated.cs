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

// Auto-generated hot-update skeletons for FamilySystem.objectmodelCollectionChangeNotificationsHotUpdate.
// Each method exercises a method from the HostToPatch direction.
public static class FamilySystem.objectmodelCollectionChangeNotificationsHotUpdate
{
    // Purpose: Verify INotifyCollectionChanged.add_CollectionChanged executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.ObjectModel/System.Collections.Specialized.INotifyCollectionChanged::add_CollectionChanged:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_ObjectModel_System_Collections_Specialized_INotifyCollectionChanged_add_CollectionChanged_System_Void_P0() { INotifyCollectionChanged.add_CollectionChanged(null!); }
    // Purpose: Verify INotifyCollectionChanged.remove_CollectionChanged executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.ObjectModel/System.Collections.Specialized.INotifyCollectionChanged::remove_CollectionChanged:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_ObjectModel_System_Collections_Specialized_INotifyCollectionChanged_remove_CollectionChanged_System_Void_P0() { INotifyCollectionChanged.remove_CollectionChanged(null!); }
    // Purpose: Verify NotifyCollectionChangedEventArgs.NotifyCollectionChangedEventArgs: executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.ObjectModel/System.Collections.Specialized.NotifyCollectionChangedEventArgs::NotifyCollectionChangedEventArgs::Void(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_ObjectModel_System_Collections_Specialized_NotifyCollectionChangedEventArgs_NotifyCollectionChangedEventArgs_Void() { _ = NotifyCollectionChangedEventArgs.NotifyCollectionChangedEventArgs:(42); }
    // Purpose: Verify NotifyCollectionChangedEventArgs.NotifyCollectionChangedEventArgs: executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.ObjectModel/System.Collections.Specialized.NotifyCollectionChangedEventArgs::NotifyCollectionChangedEventArgs::Void(System.Object, System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_ObjectModel_System_Collections_Specialized_NotifyCollectionChangedEventArgs_NotifyCollectionChangedEventArgs_Void() { _ = NotifyCollectionChangedEventArgs.NotifyCollectionChangedEventArgs:(42, 42); }
    // Purpose: Verify NotifyCollectionChangedEventArgs.NotifyCollectionChangedEventArgs: executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.ObjectModel/System.Collections.Specialized.NotifyCollectionChangedEventArgs::NotifyCollectionChangedEventArgs::Void(System.Object, System.Object, System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_ObjectModel_System_Collections_Specialized_NotifyCollectionChangedEventArgs_NotifyCollectionChangedEventArgs_Void() { _ = NotifyCollectionChangedEventArgs.NotifyCollectionChangedEventArgs:(42, 42, 42); }
    // Purpose: Verify NotifyCollectionChangedEventArgs.NotifyCollectionChangedEventArgs: executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.ObjectModel/System.Collections.Specialized.NotifyCollectionChangedEventArgs::NotifyCollectionChangedEventArgs::Void(System.Object, System.Object, System.Object, System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_ObjectModel_System_Collections_Specialized_NotifyCollectionChangedEventArgs_NotifyCollectionChangedEventArgs_Void() { _ = NotifyCollectionChangedEventArgs.NotifyCollectionChangedEventArgs:(42, 42, 42, 42); }
    // Purpose: Verify NotifyCollectionChangedEventArgs.get_Action executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.ObjectModel/System.Collections.Specialized.NotifyCollectionChangedEventArgs::get_Action:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_ObjectModel_System_Collections_Specialized_NotifyCollectionChangedEventArgs_get_Action_System_Void() { NotifyCollectionChangedEventArgs.Action; }
    // Purpose: Verify NotifyCollectionChangedEventArgs.get_NewItems executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.ObjectModel/System.Collections.Specialized.NotifyCollectionChangedEventArgs::get_NewItems:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_ObjectModel_System_Collections_Specialized_NotifyCollectionChangedEventArgs_get_NewItems_System_Void() { NotifyCollectionChangedEventArgs.NewItems; }
    // Purpose: Verify NotifyCollectionChangedEventArgs.get_NewStartingIndex executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.ObjectModel/System.Collections.Specialized.NotifyCollectionChangedEventArgs::get_NewStartingIndex:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_ObjectModel_System_Collections_Specialized_NotifyCollectionChangedEventArgs_get_NewStartingIndex_System_Void() { NotifyCollectionChangedEventArgs.NewStartingIndex; }
    // Purpose: Verify NotifyCollectionChangedEventArgs.get_OldItems executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.ObjectModel/System.Collections.Specialized.NotifyCollectionChangedEventArgs::get_OldItems:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_ObjectModel_System_Collections_Specialized_NotifyCollectionChangedEventArgs_get_OldItems_System_Void() { NotifyCollectionChangedEventArgs.OldItems; }
    // Purpose: Verify NotifyCollectionChangedEventArgs.get_OldStartingIndex executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.ObjectModel/System.Collections.Specialized.NotifyCollectionChangedEventArgs::get_OldStartingIndex:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_ObjectModel_System_Collections_Specialized_NotifyCollectionChangedEventArgs_get_OldStartingIndex_System_Void() { NotifyCollectionChangedEventArgs.OldStartingIndex; }
    // Purpose: Verify NotifyCollectionChangedEventArgs.set_Action executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.ObjectModel/System.Collections.Specialized.NotifyCollectionChangedEventArgs::set_Action:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_ObjectModel_System_Collections_Specialized_NotifyCollectionChangedEventArgs_set_Action_System_Void_P0() { NotifyCollectionChangedEventArgs.Action = null!; }
    // Purpose: Verify NotifyCollectionChangedEventArgs.set_NewItems executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.ObjectModel/System.Collections.Specialized.NotifyCollectionChangedEventArgs::set_NewItems:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_ObjectModel_System_Collections_Specialized_NotifyCollectionChangedEventArgs_set_NewItems_System_Void_P0() { NotifyCollectionChangedEventArgs.NewItems = null!; }
    // Purpose: Verify NotifyCollectionChangedEventArgs.set_NewStartingIndex executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.ObjectModel/System.Collections.Specialized.NotifyCollectionChangedEventArgs::set_NewStartingIndex:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_ObjectModel_System_Collections_Specialized_NotifyCollectionChangedEventArgs_set_NewStartingIndex_System_Void_P0() { NotifyCollectionChangedEventArgs.NewStartingIndex = null!; }
    // Purpose: Verify NotifyCollectionChangedEventArgs.set_OldItems executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.ObjectModel/System.Collections.Specialized.NotifyCollectionChangedEventArgs::set_OldItems:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_ObjectModel_System_Collections_Specialized_NotifyCollectionChangedEventArgs_set_OldItems_System_Void_P0() { NotifyCollectionChangedEventArgs.OldItems = null!; }
    // Purpose: Verify NotifyCollectionChangedEventArgs.set_OldStartingIndex executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.ObjectModel/System.Collections.Specialized.NotifyCollectionChangedEventArgs::set_OldStartingIndex:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_ObjectModel_System_Collections_Specialized_NotifyCollectionChangedEventArgs_set_OldStartingIndex_System_Void_P0() { NotifyCollectionChangedEventArgs.OldStartingIndex = null!; }
    // Purpose: Verify NotifyCollectionChangedEventHandler.NotifyCollectionChangedEventHandler: executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.ObjectModel/System.Collections.Specialized.NotifyCollectionChangedEventHandler::NotifyCollectionChangedEventHandler::Void(System.Object, System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_ObjectModel_System_Collections_Specialized_NotifyCollectionChangedEventHandler_NotifyCollectionChangedEventHandle() { _ = NotifyCollectionChangedEventHandler.NotifyCollectionChangedEventHandler:(42, 42); }
    // Purpose: Verify NotifyCollectionChangedEventHandler.BeginInvoke executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.ObjectModel/System.Collections.Specialized.NotifyCollectionChangedEventHandler::BeginInvoke:System.Void(P0, P1, P2, P3)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_ObjectModel_System_Collections_Specialized_NotifyCollectionChangedEventHandler_BeginInvoke_System_Void_P0_P1_P2_P() { NotifyCollectionChangedEventHandler.BeginInvoke(null!, null!, null!, null!); }
    // Purpose: Verify NotifyCollectionChangedEventHandler.EndInvoke executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.ObjectModel/System.Collections.Specialized.NotifyCollectionChangedEventHandler::EndInvoke:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_ObjectModel_System_Collections_Specialized_NotifyCollectionChangedEventHandler_EndInvoke_System_Void_P0() { NotifyCollectionChangedEventHandler.EndInvoke(null!); }
    // Purpose: Verify NotifyCollectionChangedEventHandler.Invoke executes correctly after hot-update patch (host side)
    [HotUpdateSubjectId("System.ObjectModel/System.Collections.Specialized.NotifyCollectionChangedEventHandler::Invoke:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.HostToPatch)]
    public static void HotUpdate_System_ObjectModel_System_Collections_Specialized_NotifyCollectionChangedEventHandler_Invoke_System_Void_P0_P1() { NotifyCollectionChangedEventHandler.Invoke(null!, null!); }}
