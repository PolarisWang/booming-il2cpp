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

// Auto-generated patch-side skeletons for FamilySystem.objectmodelObservableCollectionsPatch.
// Each method exercises a method from the patch side back to the host after hot-update.
public static class FamilySystem.objectmodelObservableCollectionsPatch
{
    // Purpose: Verify ObservableCollection.ObservableCollection`1: executes correctly from patch side back to host
    [HotUpdateSubjectId("System.ObjectModel/System.Collections.ObjectModel.ObservableCollection`1::ObservableCollection`1::Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_ObjectModel_System_Collections_ObjectModel_ObservableCollection_1_ObservableCollection_1_Void() { _ = ObservableCollection.ObservableCollection`1:(); }
    // Purpose: Verify ObservableCollection.ObservableCollection`1: executes correctly from patch side back to host
    [HotUpdateSubjectId("System.ObjectModel/System.Collections.ObjectModel.ObservableCollection`1::ObservableCollection`1::Void(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_ObjectModel_System_Collections_ObjectModel_ObservableCollection_1_ObservableCollection_1_Void_System_Object() { _ = ObservableCollection.ObservableCollection`1:(42); }
    // Purpose: Verify ObservableCollection.BlockReentrancy executes correctly from patch side back to host
    [HotUpdateSubjectId("System.ObjectModel/System.Collections.ObjectModel.ObservableCollection`1::BlockReentrancy:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_ObjectModel_System_Collections_ObjectModel_ObservableCollection_1_BlockReentrancy_System_Void() { ObservableCollection.BlockReentrancy(); }
    // Purpose: Verify ObservableCollection.CheckReentrancy executes correctly from patch side back to host
    [HotUpdateSubjectId("System.ObjectModel/System.Collections.ObjectModel.ObservableCollection`1::CheckReentrancy:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_ObjectModel_System_Collections_ObjectModel_ObservableCollection_1_CheckReentrancy_System_Void() { ObservableCollection.CheckReentrancy(); }
    // Purpose: Verify ObservableCollection.ClearItems executes correctly from patch side back to host
    [HotUpdateSubjectId("System.ObjectModel/System.Collections.ObjectModel.ObservableCollection`1::ClearItems:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_ObjectModel_System_Collections_ObjectModel_ObservableCollection_1_ClearItems_System_Void() { ObservableCollection.ClearItems(); }
    // Purpose: Verify ObservableCollection.InsertItem executes correctly from patch side back to host
    [HotUpdateSubjectId("System.ObjectModel/System.Collections.ObjectModel.ObservableCollection`1::InsertItem:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_ObjectModel_System_Collections_ObjectModel_ObservableCollection_1_InsertItem_System_Void_P0_P1() { ObservableCollection.InsertItem(null!, null!); }
    // Purpose: Verify ObservableCollection.Move executes correctly from patch side back to host
    [HotUpdateSubjectId("System.ObjectModel/System.Collections.ObjectModel.ObservableCollection`1::Move:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_ObjectModel_System_Collections_ObjectModel_ObservableCollection_1_Move_System_Void_P0_P1() { ObservableCollection.Move(null!, null!); }
    // Purpose: Verify ObservableCollection.MoveItem executes correctly from patch side back to host
    [HotUpdateSubjectId("System.ObjectModel/System.Collections.ObjectModel.ObservableCollection`1::MoveItem:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_ObjectModel_System_Collections_ObjectModel_ObservableCollection_1_MoveItem_System_Void_P0_P1() { ObservableCollection.MoveItem(null!, null!); }
    // Purpose: Verify ObservableCollection.OnCollectionChanged executes correctly from patch side back to host
    [HotUpdateSubjectId("System.ObjectModel/System.Collections.ObjectModel.ObservableCollection`1::OnCollectionChanged:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_ObjectModel_System_Collections_ObjectModel_ObservableCollection_1_OnCollectionChanged_System_Void_P0() { ObservableCollection.OnCollectionChanged(null!); }
    // Purpose: Verify ObservableCollection.OnPropertyChanged executes correctly from patch side back to host
    [HotUpdateSubjectId("System.ObjectModel/System.Collections.ObjectModel.ObservableCollection`1::OnPropertyChanged:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_ObjectModel_System_Collections_ObjectModel_ObservableCollection_1_OnPropertyChanged_System_Void_P0() { ObservableCollection.OnPropertyChanged(null!); }
    // Purpose: Verify ObservableCollection.RemoveItem executes correctly from patch side back to host
    [HotUpdateSubjectId("System.ObjectModel/System.Collections.ObjectModel.ObservableCollection`1::RemoveItem:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_ObjectModel_System_Collections_ObjectModel_ObservableCollection_1_RemoveItem_System_Void_P0() { ObservableCollection.RemoveItem(null!); }
    // Purpose: Verify ObservableCollection.SetItem executes correctly from patch side back to host
    [HotUpdateSubjectId("System.ObjectModel/System.Collections.ObjectModel.ObservableCollection`1::SetItem:System.Void(P0, P1)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_ObjectModel_System_Collections_ObjectModel_ObservableCollection_1_SetItem_System_Void_P0_P1() { ObservableCollection.SetItem(null!, null!); }
    // Purpose: Verify ObservableCollection.add_CollectionChanged executes correctly from patch side back to host
    [HotUpdateSubjectId("System.ObjectModel/System.Collections.ObjectModel.ObservableCollection`1::add_CollectionChanged:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_ObjectModel_System_Collections_ObjectModel_ObservableCollection_1_add_CollectionChanged_System_Void_P0() { ObservableCollection.add_CollectionChanged(null!); }
    // Purpose: Verify ObservableCollection.add_PropertyChanged executes correctly from patch side back to host
    [HotUpdateSubjectId("System.ObjectModel/System.Collections.ObjectModel.ObservableCollection`1::add_PropertyChanged:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_ObjectModel_System_Collections_ObjectModel_ObservableCollection_1_add_PropertyChanged_System_Void_P0() { ObservableCollection.add_PropertyChanged(null!); }
    // Purpose: Verify ObservableCollection.remove_CollectionChanged executes correctly from patch side back to host
    [HotUpdateSubjectId("System.ObjectModel/System.Collections.ObjectModel.ObservableCollection`1::remove_CollectionChanged:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_ObjectModel_System_Collections_ObjectModel_ObservableCollection_1_remove_CollectionChanged_System_Void_P0() { ObservableCollection.remove_CollectionChanged(null!); }
    // Purpose: Verify ObservableCollection.remove_PropertyChanged executes correctly from patch side back to host
    [HotUpdateSubjectId("System.ObjectModel/System.Collections.ObjectModel.ObservableCollection`1::remove_PropertyChanged:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_ObjectModel_System_Collections_ObjectModel_ObservableCollection_1_remove_PropertyChanged_System_Void_P0() { ObservableCollection.remove_PropertyChanged(null!); }
    // Purpose: Verify ReadOnlyObservableCollection.ReadOnlyObservableCollection`1: executes correctly from patch side back to host
    [HotUpdateSubjectId("System.ObjectModel/System.Collections.ObjectModel.ReadOnlyObservableCollection`1::ReadOnlyObservableCollection`1::Void(System.Object)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_ObjectModel_System_Collections_ObjectModel_ReadOnlyObservableCollection_1_ReadOnlyObservableCollection_1_Void_Sys() { _ = ReadOnlyObservableCollection.ReadOnlyObservableCollection`1:(42); }
    // Purpose: Verify ReadOnlyObservableCollection.OnCollectionChanged executes correctly from patch side back to host
    [HotUpdateSubjectId("System.ObjectModel/System.Collections.ObjectModel.ReadOnlyObservableCollection`1::OnCollectionChanged:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_ObjectModel_System_Collections_ObjectModel_ReadOnlyObservableCollection_1_OnCollectionChanged_System_Void_P0() { ReadOnlyObservableCollection.OnCollectionChanged(null!); }
    // Purpose: Verify ReadOnlyObservableCollection.OnPropertyChanged executes correctly from patch side back to host
    [HotUpdateSubjectId("System.ObjectModel/System.Collections.ObjectModel.ReadOnlyObservableCollection`1::OnPropertyChanged:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_ObjectModel_System_Collections_ObjectModel_ReadOnlyObservableCollection_1_OnPropertyChanged_System_Void_P0() { ReadOnlyObservableCollection.OnPropertyChanged(null!); }
    // Purpose: Verify ReadOnlyObservableCollection.add_CollectionChanged executes correctly from patch side back to host
    [HotUpdateSubjectId("System.ObjectModel/System.Collections.ObjectModel.ReadOnlyObservableCollection`1::add_CollectionChanged:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_ObjectModel_System_Collections_ObjectModel_ReadOnlyObservableCollection_1_add_CollectionChanged_System_Void_P0() { ReadOnlyObservableCollection.add_CollectionChanged(null!); }
    // Purpose: Verify ReadOnlyObservableCollection.add_PropertyChanged executes correctly from patch side back to host
    [HotUpdateSubjectId("System.ObjectModel/System.Collections.ObjectModel.ReadOnlyObservableCollection`1::add_PropertyChanged:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_ObjectModel_System_Collections_ObjectModel_ReadOnlyObservableCollection_1_add_PropertyChanged_System_Void_P0() { ReadOnlyObservableCollection.add_PropertyChanged(null!); }
    // Purpose: Verify ReadOnlyObservableCollection.get_Empty executes correctly from patch side back to host
    [HotUpdateSubjectId("System.ObjectModel/System.Collections.ObjectModel.ReadOnlyObservableCollection`1::get_Empty:System.Void()")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_ObjectModel_System_Collections_ObjectModel_ReadOnlyObservableCollection_1_get_Empty_System_Void() { ReadOnlyObservableCollection.Empty; }
    // Purpose: Verify ReadOnlyObservableCollection.remove_CollectionChanged executes correctly from patch side back to host
    [HotUpdateSubjectId("System.ObjectModel/System.Collections.ObjectModel.ReadOnlyObservableCollection`1::remove_CollectionChanged:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_ObjectModel_System_Collections_ObjectModel_ReadOnlyObservableCollection_1_remove_CollectionChanged_System_Void_P0() { ReadOnlyObservableCollection.remove_CollectionChanged(null!); }
    // Purpose: Verify ReadOnlyObservableCollection.remove_PropertyChanged executes correctly from patch side back to host
    [HotUpdateSubjectId("System.ObjectModel/System.Collections.ObjectModel.ReadOnlyObservableCollection`1::remove_PropertyChanged:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_ObjectModel_System_Collections_ObjectModel_ReadOnlyObservableCollection_1_remove_PropertyChanged_System_Void_P0() { ReadOnlyObservableCollection.remove_PropertyChanged(null!); }
    // Purpose: Verify ReadOnlyObservableCollection.set_Empty executes correctly from patch side back to host
    [HotUpdateSubjectId("System.ObjectModel/System.Collections.ObjectModel.ReadOnlyObservableCollection`1::set_Empty:System.Void(P0)")]
    [CapabilityFamilyId(CapabilityFamilyId.None)]
    [HotUpdateDirection(HotUpdateDirection.PatchToHost)]
    public static void Patch_System_ObjectModel_System_Collections_ObjectModel_ReadOnlyObservableCollection_1_set_Empty_System_Void_P0() { ReadOnlyObservableCollection.Empty = null!; }}
