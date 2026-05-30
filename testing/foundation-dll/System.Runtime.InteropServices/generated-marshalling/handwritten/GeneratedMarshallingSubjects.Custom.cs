// Auto-generated handwritten test stubs for pre-verification audit.
// TODO: Replace each stub body with a real test invocation.

using System.Collections.Generic;
using System.Runtime.InteropServices;
using System;

public static partial class GeneratedMarshallingSubjects
{
    // [0] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.ComExposedClassAttribute`1::GetComInterfaceEntries:System.Runtime.InteropServices.ComWrappers+ComInterfaceEntry*(System.Int32&)
    public static void CustomEntrySubject_0()
    {        try { _ = ComExposedClassAttribute.GetComInterfaceEntries(out 42); } catch { _exitCode = 1; }    }

    // [1] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.ComExposedClassAttribute`1::.ctor:System.Void()
    public static void CustomEntrySubject_1()
    {        try { new ComExposedClassAttribute<byte>(); } catch { _exitCode = 1; }    }

    // [2] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.ComInterfaceMarshaller`1::ConvertToManaged:T(System.Void*)
    public static void CustomEntrySubject_2()
    {        try { _ = ComInterfaceMarshaller.ConvertToManaged(default); } catch { _exitCode = 1; }    }

    // [3] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.ComInterfaceMarshaller`1::ConvertToUnmanaged:System.Void*(T)
    public static void CustomEntrySubject_3()
    {        try { _ = ComInterfaceMarshaller.ConvertToUnmanaged(default); } catch { _exitCode = 1; }    }

    // [4] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.ComInterfaceMarshaller`1::Free:System.Void(System.Void*)
    public static void CustomEntrySubject_4()
    {        try { ComInterfaceMarshaller.Free(default); } catch { _exitCode = 1; }    }

    // [5] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.ComObject::FinalRelease:System.Void()
    public static void CustomEntrySubject_5()
    {        try { default(ComObject)!.FinalRelease(); } catch { _exitCode = 1; }    }

    // [6] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.ComVariantMarshaller::ConvertToManaged:System.Object(System.Runtime.InteropServices.Marshalling.ComVariant)
    public static void CustomEntrySubject_6()
    {        try { _ = ComVariantMarshaller.ConvertToManaged(default); } catch { _exitCode = 1; }    }

    // [7] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.ComVariantMarshaller::ConvertToUnmanaged:System.Runtime.InteropServices.Marshalling.ComVariant(System.Object)
    public static void CustomEntrySubject_7()
    {        try { _ = ComVariantMarshaller.ConvertToUnmanaged(null!); } catch { _exitCode = 1; }    }

    // [8] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.ComVariantMarshaller::Free:System.Void(System.Runtime.InteropServices.Marshalling.ComVariant)
    public static void CustomEntrySubject_8()
    {        try { ComVariantMarshaller.Free(default); } catch { _exitCode = 1; }    }

    // [9] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.ComVariantMarshaller+RefPropagate::Free:System.Void()
    public static void CustomEntrySubject_9()
    {        try { ComVariantMarshaller.RefPropagate.Free(); } catch { _exitCode = 1; }    }

    // [10] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.ComVariantMarshaller+RefPropagate::FromManaged:System.Void(System.Object)
    public static void CustomEntrySubject_10()
    {        try { ComVariantMarshaller.RefPropagate.FromManaged(null!); } catch { _exitCode = 1; }    }

    // [11] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.ComVariantMarshaller+RefPropagate::FromUnmanaged:System.Void(System.Runtime.InteropServices.Marshalling.ComVariant)
    public static void CustomEntrySubject_11()
    {        try { ComVariantMarshaller.RefPropagate.FromUnmanaged(default); } catch { _exitCode = 1; }    }

    // [12] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.ComVariantMarshaller+RefPropagate::ToManaged:System.Object()
    public static void CustomEntrySubject_12()
    {        try { _ = ComVariantMarshaller.RefPropagate.ToManaged(); } catch { _exitCode = 1; }    }

    // [13] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.ComVariantMarshaller+RefPropagate::ToUnmanaged:System.Runtime.InteropServices.Marshalling.ComVariant()
    public static void CustomEntrySubject_13()
    {        try { _ = ComVariantMarshaller.RefPropagate.ToUnmanaged(); } catch { _exitCode = 1; }    }

    // [14] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.ExceptionAsDefaultMarshaller`1::ConvertToUnmanaged:T(System.Exception)
    public static void CustomEntrySubject_14()
    {        try { _ = ExceptionAsDefaultMarshaller.ConvertToUnmanaged(default); } catch { _exitCode = 1; }    }

    // [15] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.ExceptionAsHResultMarshaller`1::ConvertToUnmanaged:T(System.Exception)
    public static void CustomEntrySubject_15()
    {        try { _ = ExceptionAsHResultMarshaller.ConvertToUnmanaged(default); } catch { _exitCode = 1; }    }

    // [16] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.ExceptionAsNaNMarshaller`1::ConvertToUnmanaged:T(System.Exception)
    public static void CustomEntrySubject_16()
    {        try { _ = ExceptionAsNaNMarshaller.ConvertToUnmanaged(default); } catch { _exitCode = 1; }    }

    // [17] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.ExceptionAsVoidMarshaller::ConvertToUnmanaged:System.Void(System.Exception)
    public static void CustomEntrySubject_17()
    {        try { ExceptionAsVoidMarshaller.ConvertToUnmanaged(default); } catch { _exitCode = 1; }    }

    // [19] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.GeneratedComInterfaceAttribute::get_ExceptionToUnmanagedMarshaller:System.Type()
    public static void CustomEntrySubject_19()
    {        try { _ = default(GeneratedComInterfaceAttribute)!.ExceptionToUnmanagedMarshaller; } catch { _exitCode = 1; }    }

    // [20] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.GeneratedComInterfaceAttribute::get_Options:System.Runtime.InteropServices.Marshalling.ComInterfaceOptions()
    public static void CustomEntrySubject_20()
    {        try { _ = default(GeneratedComInterfaceAttribute)!.Options; } catch { _exitCode = 1; }    }

    // [21] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.GeneratedComInterfaceAttribute::get_StringMarshalling:System.Runtime.InteropServices.StringMarshalling()
    public static void CustomEntrySubject_21()
    {        try { _ = default(GeneratedComInterfaceAttribute)!.StringMarshalling; } catch { _exitCode = 1; }    }

    // [22] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.GeneratedComInterfaceAttribute::get_StringMarshallingCustomType:System.Type()
    public static void CustomEntrySubject_22()
    {        try { _ = default(GeneratedComInterfaceAttribute)!.StringMarshallingCustomType; } catch { _exitCode = 1; }    }

    // [23] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.GeneratedComInterfaceAttribute::set_ExceptionToUnmanagedMarshaller:System.Void(System.Type)
    public static void CustomEntrySubject_23()
    {        try { default(GeneratedComInterfaceAttribute)!.ExceptionToUnmanagedMarshaller = typeof(byte); } catch { _exitCode = 1; }    }

    // [24] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.GeneratedComInterfaceAttribute::set_Options:System.Void(System.Runtime.InteropServices.Marshalling.ComInterfaceOptions)
    public static void CustomEntrySubject_24()
    {        try { default(GeneratedComInterfaceAttribute)!.Options = default; } catch { _exitCode = 1; }    }

    // [25] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.GeneratedComInterfaceAttribute::set_StringMarshalling:System.Void(System.Runtime.InteropServices.StringMarshalling)
    public static void CustomEntrySubject_25()
    {        try { default(GeneratedComInterfaceAttribute)!.StringMarshalling = default; } catch { _exitCode = 1; }    }

    // [26] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.GeneratedComInterfaceAttribute::set_StringMarshallingCustomType:System.Void(System.Type)
    public static void CustomEntrySubject_26()
    {        try { default(GeneratedComInterfaceAttribute)!.StringMarshallingCustomType = typeof(byte); } catch { _exitCode = 1; }    }

    // [28] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.IComExposedClass::GetComInterfaceEntries:System.Runtime.InteropServices.ComWrappers+ComInterfaceEntry*(System.Int32&)
    public static void CustomEntrySubject_28()
    {        try { _ = IComExposedClass.GetComInterfaceEntries(out 42); } catch { _exitCode = 1; }    }

    // [29] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.IComExposedDetails::GetComInterfaceEntries:System.Runtime.InteropServices.ComWrappers+ComInterfaceEntry*(System.Int32&)
    public static void CustomEntrySubject_29()
    {        try { _ = IComExposedDetails.GetComInterfaceEntries(out 42); } catch { _exitCode = 1; }    }

    // [30] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.IIUnknownCacheStrategy::Clear:System.Void(System.Runtime.InteropServices.Marshalling.IIUnknownStrategy)
    public static void CustomEntrySubject_30()
    {        try { default(IIUnknownCacheStrategy)!.Clear(default); } catch { _exitCode = 1; }    }

    // [31] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.IIUnknownCacheStrategy::ConstructTableInfo:System.Runtime.InteropServices.Marshalling.IIUnknownCacheStrategy+TableInfo(System.RuntimeTypeHandle,System.Runtime.InteropServices.Marshalling.IIUnknownDerivedDetails,System.Void*)
    public static void CustomEntrySubject_31()
    {        try { _ = default(IIUnknownCacheStrategy)!.ConstructTableInfo(default(System.RuntimeTypeHandle), default, default); } catch { _exitCode = 1; }    }

    // [32] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.IIUnknownCacheStrategy::TryGetTableInfo:System.Boolean(System.RuntimeTypeHandle,System.Runtime.InteropServices.Marshalling.IIUnknownCacheStrategy+TableInfo&)
    public static void CustomEntrySubject_32()
    {        try { _ = default(IIUnknownCacheStrategy)!.TryGetTableInfo(default(System.RuntimeTypeHandle), out default); } catch { _exitCode = 1; }    }

    // [33] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.IIUnknownCacheStrategy::TrySetTableInfo:System.Boolean(System.RuntimeTypeHandle,System.Runtime.InteropServices.Marshalling.IIUnknownCacheStrategy+TableInfo)
    public static void CustomEntrySubject_33()
    {        try { _ = default(IIUnknownCacheStrategy)!.TrySetTableInfo(default(System.RuntimeTypeHandle), default); } catch { _exitCode = 1; }    }

    // [34] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.IIUnknownCacheStrategy+TableInfo::get_ManagedType:System.RuntimeTypeHandle()
    public static void CustomEntrySubject_34()
    {        try { _ = IIUnknownCacheStrategy.TableInfo.ManagedType; } catch { _exitCode = 1; }    }

    // [35] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.IIUnknownCacheStrategy+TableInfo::get_Table:System.Void**()
    public static void CustomEntrySubject_35()
    {        try { _ = IIUnknownCacheStrategy.TableInfo.Table; } catch { _exitCode = 1; }    }

    // [36] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.IIUnknownCacheStrategy+TableInfo::get_ThisPtr:System.Void*()
    public static void CustomEntrySubject_36()
    {        try { _ = IIUnknownCacheStrategy.TableInfo.ThisPtr; } catch { _exitCode = 1; }    }

    // [37] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.IIUnknownCacheStrategy+TableInfo::set_ManagedType:System.Void(System.RuntimeTypeHandle)
    public static void CustomEntrySubject_37()
    {        try { IIUnknownCacheStrategy.TableInfo.ManagedType = default(System.RuntimeTypeHandle); } catch { _exitCode = 1; }    }

    // [38] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.IIUnknownCacheStrategy+TableInfo::set_Table:System.Void(System.Void**)
    public static void CustomEntrySubject_38()
    {        try { IIUnknownCacheStrategy.TableInfo.Table = default; } catch { _exitCode = 1; }    }

    // [39] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.IIUnknownCacheStrategy+TableInfo::set_ThisPtr:System.Void(System.Void*)
    public static void CustomEntrySubject_39()
    {        try { IIUnknownCacheStrategy.TableInfo.ThisPtr = default; } catch { _exitCode = 1; }    }

    // [40] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.IIUnknownDerivedDetails::get_Iid:System.Guid()
    public static void CustomEntrySubject_40()
    {        try { _ = default(IIUnknownDerivedDetails)!.Iid; } catch { _exitCode = 1; }    }

    // [41] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.IIUnknownDerivedDetails::get_Implementation:System.Type()
    public static void CustomEntrySubject_41()
    {        try { _ = default(IIUnknownDerivedDetails)!.Implementation; } catch { _exitCode = 1; }    }

    // [42] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.IIUnknownDerivedDetails::get_ManagedVirtualMethodTable:System.Void**()
    public static void CustomEntrySubject_42()
    {        try { _ = default(IIUnknownDerivedDetails)!.ManagedVirtualMethodTable; } catch { _exitCode = 1; }    }

    // [43] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.IIUnknownInterfaceDetailsStrategy::GetComExposedTypeDetails:System.Runtime.InteropServices.Marshalling.IComExposedDetails(System.RuntimeTypeHandle)
    public static void CustomEntrySubject_43()
    {        try { _ = IIUnknownInterfaceDetailsStrategy.GetComExposedTypeDetails(default(System.RuntimeTypeHandle)); } catch { _exitCode = 1; }    }

    // [44] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.IIUnknownInterfaceDetailsStrategy::GetIUnknownDerivedDetails:System.Runtime.InteropServices.Marshalling.IIUnknownDerivedDetails(System.RuntimeTypeHandle)
    public static void CustomEntrySubject_44()
    {        try { _ = IIUnknownInterfaceDetailsStrategy.GetIUnknownDerivedDetails(default(System.RuntimeTypeHandle)); } catch { _exitCode = 1; }    }

    // [45] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.IIUnknownInterfaceType::get_Iid:System.Guid()
    public static void CustomEntrySubject_45()
    {        try { _ = default(IIUnknownInterfaceType)!.Iid; } catch { _exitCode = 1; }    }

    // [46] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.IIUnknownInterfaceType::get_ManagedVirtualMethodTable:System.Void**()
    public static void CustomEntrySubject_46()
    {        try { _ = default(IIUnknownInterfaceType)!.ManagedVirtualMethodTable; } catch { _exitCode = 1; }    }

    // [47] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.IIUnknownStrategy::CreateInstancePointer:System.Void*(System.Void*)
    public static void CustomEntrySubject_47()
    {        try { _ = IIUnknownStrategy.CreateInstancePointer(default); } catch { _exitCode = 1; }    }

    // [48] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.IIUnknownStrategy::QueryInterface:System.Int32(System.Void*,System.Guid&,System.Void*&)
    public static void CustomEntrySubject_48()
    {        try { _ = IIUnknownStrategy.QueryInterface(default, out Guid.Empty, out default); } catch { _exitCode = 1; }    }

    // [49] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.IIUnknownStrategy::Release:System.Int32(System.Void*)
    public static void CustomEntrySubject_49()
    {        try { _ = IIUnknownStrategy.Release(default); } catch { _exitCode = 1; }    }

    // [50] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.IUnknownDerivedAttribute`2::get_Iid:System.Guid()
    public static void CustomEntrySubject_50()
    {        try { _ = default(IUnknownDerivedAttribute)!.Iid; } catch { _exitCode = 1; }    }

    // [51] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.IUnknownDerivedAttribute`2::get_Implementation:System.Type()
    public static void CustomEntrySubject_51()
    {        try { _ = default(IUnknownDerivedAttribute)!.Implementation; } catch { _exitCode = 1; }    }

    // [52] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.IUnknownDerivedAttribute`2::get_ManagedVirtualMethodTable:System.Void**()
    public static void CustomEntrySubject_52()
    {        try { _ = default(IUnknownDerivedAttribute)!.ManagedVirtualMethodTable; } catch { _exitCode = 1; }    }

    // [53] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.IUnknownDerivedAttribute`2::.ctor:System.Void()
    public static void CustomEntrySubject_53()
    {        try { new IUnknownDerivedAttribute<byte, byte>(); } catch { _exitCode = 1; }    }

    // [54] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.IUnmanagedVirtualMethodTableProvider::GetVirtualMethodTableInfoForKey:System.Runtime.InteropServices.Marshalling.VirtualMethodTableInfo(System.Type)
    public static void CustomEntrySubject_54()
    {        try { _ = IUnmanagedVirtualMethodTableProvider.GetVirtualMethodTableInfoForKey(typeof(byte)); } catch { _exitCode = 1; }    }

    // [55] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.StrategyBasedComWrappers::get_DefaultIUnknownInterfaceDetailsStrategy:System.Runtime.InteropServices.Marshalling.IIUnknownInterfaceDetailsStrategy()
    public static void CustomEntrySubject_55()
    {        try { _ = new StrategyBasedComWrappers().DefaultIUnknownInterfaceDetailsStrategy; } catch { _exitCode = 1; }    }

    // [56] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.StrategyBasedComWrappers::get_DefaultIUnknownStrategy:System.Runtime.InteropServices.Marshalling.IIUnknownStrategy()
    public static void CustomEntrySubject_56()
    {        try { _ = new StrategyBasedComWrappers().DefaultIUnknownStrategy; } catch { _exitCode = 1; }    }

    // [58] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.UniqueComInterfaceMarshaller`1::ConvertToManaged:T(System.Void*)
    public static void CustomEntrySubject_58()
    {        try { _ = UniqueComInterfaceMarshaller.ConvertToManaged(default); } catch { _exitCode = 1; }    }

    // [59] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.UniqueComInterfaceMarshaller`1::ConvertToUnmanaged:System.Void*(T)
    public static void CustomEntrySubject_59()
    {        try { _ = UniqueComInterfaceMarshaller.ConvertToUnmanaged(default); } catch { _exitCode = 1; }    }

    // [60] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.UniqueComInterfaceMarshaller`1::Free:System.Void(System.Void*)
    public static void CustomEntrySubject_60()
    {        try { UniqueComInterfaceMarshaller.Free(default); } catch { _exitCode = 1; }    }

    // [61] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.VirtualMethodTableInfo::Deconstruct:System.Void(System.Void*&,System.Void**&)
    public static void CustomEntrySubject_61()
    {        try { default(VirtualMethodTableInfo).Deconstruct(out default, out default); } catch { _exitCode = 1; }    }

    // [62] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.VirtualMethodTableInfo::get_ThisPointer:System.Void*()
    public static void CustomEntrySubject_62()
    {        try { _ = default(VirtualMethodTableInfo).ThisPointer; } catch { _exitCode = 1; }    }

    // [63] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.VirtualMethodTableInfo::get_VirtualMethodTable:System.Void**()
    public static void CustomEntrySubject_63()
    {        try { _ = default(VirtualMethodTableInfo).VirtualMethodTable; } catch { _exitCode = 1; }    }

    // [64] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.VirtualMethodTableInfo::.ctor:System.Void(System.Void*,System.Void**)
    public static void CustomEntrySubject_64()
    {        try { new VirtualMethodTableInfo(default, default); } catch { _exitCode = 1; }    }

}
