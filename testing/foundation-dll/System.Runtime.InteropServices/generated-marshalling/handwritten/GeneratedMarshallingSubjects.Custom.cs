// Auto-generated handwritten test stubs for pre-verification audit.
// TODO: Replace each stub body with a real test invocation.

using System.Collections.Generic;
using System.Runtime.InteropServices;
using System;
using Chaos.TestFramework;

public static partial class GeneratedMarshallingSubjects
{
    // [0] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.ComExposedClassAttribute`1::GetComInterfaceEntries:System.Runtime.InteropServices.ComWrappers+ComInterfaceEntry*(System.Int32&)
    [Fact]
    public static void CustomEntrySubject_0()
    {        try { _ = ComExposedClassAttribute.GetComInterfaceEntries(out 42); } catch { }    }

    // [1] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.ComExposedClassAttribute`1::.ctor:System.Void()
    [Fact]
    public static void CustomEntrySubject_1()
    {        try { new ComExposedClassAttribute<byte>(); } catch { }    }

    // [2] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.ComInterfaceMarshaller`1::ConvertToManaged:T(System.Void*)
    [Fact]
    public static void CustomEntrySubject_2()
    {        try { _ = ComInterfaceMarshaller.ConvertToManaged(default); } catch { }    }

    // [3] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.ComInterfaceMarshaller`1::ConvertToUnmanaged:System.Void*(T)
    [Fact]
    public static void CustomEntrySubject_3()
    {        try { _ = ComInterfaceMarshaller.ConvertToUnmanaged(default); } catch { }    }

    // [4] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.ComInterfaceMarshaller`1::Free:System.Void(System.Void*)
    [Fact]
    public static void CustomEntrySubject_4()
    {        try { ComInterfaceMarshaller.Free(default); } catch { }    }

    // [5] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.ComObject::FinalRelease:System.Void()
    [Fact]
    public static void CustomEntrySubject_5()
    {        try { default(ComObject)!.FinalRelease(); } catch { }    }

    // [6] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.ComVariantMarshaller::ConvertToManaged:System.Object(System.Runtime.InteropServices.Marshalling.ComVariant)
    [Fact]
    public static void CustomEntrySubject_6()
    {        try { _ = ComVariantMarshaller.ConvertToManaged(default); } catch { }    }

    // [7] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.ComVariantMarshaller::ConvertToUnmanaged:System.Runtime.InteropServices.Marshalling.ComVariant(System.Object)
    [Fact]
    public static void CustomEntrySubject_7()
    {        try { _ = ComVariantMarshaller.ConvertToUnmanaged(null!); } catch { }    }

    // [8] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.ComVariantMarshaller::Free:System.Void(System.Runtime.InteropServices.Marshalling.ComVariant)
    [Fact]
    public static void CustomEntrySubject_8()
    {        try { ComVariantMarshaller.Free(default); } catch { }    }

    // [9] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.ComVariantMarshaller+RefPropagate::Free:System.Void()
    [Fact]
    public static void CustomEntrySubject_9()
    {        try { ComVariantMarshaller.RefPropagate.Free(); } catch { }    }

    // [10] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.ComVariantMarshaller+RefPropagate::FromManaged:System.Void(System.Object)
    [Fact]
    public static void CustomEntrySubject_10()
    {        try { ComVariantMarshaller.RefPropagate.FromManaged(null!); } catch { }    }

    // [11] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.ComVariantMarshaller+RefPropagate::FromUnmanaged:System.Void(System.Runtime.InteropServices.Marshalling.ComVariant)
    [Fact]
    public static void CustomEntrySubject_11()
    {        try { ComVariantMarshaller.RefPropagate.FromUnmanaged(default); } catch { }    }

    // [12] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.ComVariantMarshaller+RefPropagate::ToManaged:System.Object()
    [Fact]
    public static void CustomEntrySubject_12()
    {        try { _ = ComVariantMarshaller.RefPropagate.ToManaged(); } catch { }    }

    // [13] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.ComVariantMarshaller+RefPropagate::ToUnmanaged:System.Runtime.InteropServices.Marshalling.ComVariant()
    [Fact]
    public static void CustomEntrySubject_13()
    {        try { _ = ComVariantMarshaller.RefPropagate.ToUnmanaged(); } catch { }    }

    // [14] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.ExceptionAsDefaultMarshaller`1::ConvertToUnmanaged:T(System.Exception)
    [Fact]
    public static void CustomEntrySubject_14()
    {        try { _ = ExceptionAsDefaultMarshaller.ConvertToUnmanaged(default); } catch { }    }

    // [15] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.ExceptionAsHResultMarshaller`1::ConvertToUnmanaged:T(System.Exception)
    [Fact]
    public static void CustomEntrySubject_15()
    {        try { _ = ExceptionAsHResultMarshaller.ConvertToUnmanaged(default); } catch { }    }

    // [16] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.ExceptionAsNaNMarshaller`1::ConvertToUnmanaged:T(System.Exception)
    [Fact]
    public static void CustomEntrySubject_16()
    {        try { _ = ExceptionAsNaNMarshaller.ConvertToUnmanaged(default); } catch { }    }

    // [17] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.ExceptionAsVoidMarshaller::ConvertToUnmanaged:System.Void(System.Exception)
    [Fact]
    public static void CustomEntrySubject_17()
    {        try { ExceptionAsVoidMarshaller.ConvertToUnmanaged(default); } catch { }    }

    // [19] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.GeneratedComInterfaceAttribute::get_ExceptionToUnmanagedMarshaller:System.Type()
    [Fact]
    public static void CustomEntrySubject_19()
    {        try { _ = default(GeneratedComInterfaceAttribute)!.ExceptionToUnmanagedMarshaller; } catch { }    }

    // [20] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.GeneratedComInterfaceAttribute::get_Options:System.Runtime.InteropServices.Marshalling.ComInterfaceOptions()
    [Fact]
    public static void CustomEntrySubject_20()
    {        try { _ = default(GeneratedComInterfaceAttribute)!.Options; } catch { }    }

    // [21] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.GeneratedComInterfaceAttribute::get_StringMarshalling:System.Runtime.InteropServices.StringMarshalling()
    [Fact]
    public static void CustomEntrySubject_21()
    {        try { _ = default(GeneratedComInterfaceAttribute)!.StringMarshalling; } catch { }    }

    // [22] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.GeneratedComInterfaceAttribute::get_StringMarshallingCustomType:System.Type()
    [Fact]
    public static void CustomEntrySubject_22()
    {        try { _ = default(GeneratedComInterfaceAttribute)!.StringMarshallingCustomType; } catch { }    }

    // [23] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.GeneratedComInterfaceAttribute::set_ExceptionToUnmanagedMarshaller:System.Void(System.Type)
    [Fact]
    public static void CustomEntrySubject_23()
    {        try { default(GeneratedComInterfaceAttribute)!.ExceptionToUnmanagedMarshaller = typeof(byte); } catch { }    }

    // [24] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.GeneratedComInterfaceAttribute::set_Options:System.Void(System.Runtime.InteropServices.Marshalling.ComInterfaceOptions)
    [Fact]
    public static void CustomEntrySubject_24()
    {        try { default(GeneratedComInterfaceAttribute)!.Options = default; } catch { }    }

    // [25] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.GeneratedComInterfaceAttribute::set_StringMarshalling:System.Void(System.Runtime.InteropServices.StringMarshalling)
    [Fact]
    public static void CustomEntrySubject_25()
    {        try { default(GeneratedComInterfaceAttribute)!.StringMarshalling = default; } catch { }    }

    // [26] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.GeneratedComInterfaceAttribute::set_StringMarshallingCustomType:System.Void(System.Type)
    [Fact]
    public static void CustomEntrySubject_26()
    {        try { default(GeneratedComInterfaceAttribute)!.StringMarshallingCustomType = typeof(byte); } catch { }    }

    // [28] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.IComExposedClass::GetComInterfaceEntries:System.Runtime.InteropServices.ComWrappers+ComInterfaceEntry*(System.Int32&)
    [Fact]
    public static void CustomEntrySubject_28()
    {        try { _ = IComExposedClass.GetComInterfaceEntries(out 42); } catch { }    }

    // [29] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.IComExposedDetails::GetComInterfaceEntries:System.Runtime.InteropServices.ComWrappers+ComInterfaceEntry*(System.Int32&)
    [Fact]
    public static void CustomEntrySubject_29()
    {        try { _ = IComExposedDetails.GetComInterfaceEntries(out 42); } catch { }    }

    // [30] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.IIUnknownCacheStrategy::Clear:System.Void(System.Runtime.InteropServices.Marshalling.IIUnknownStrategy)
    [Fact]
    public static void CustomEntrySubject_30()
    {        try { default(IIUnknownCacheStrategy)!.Clear(default); } catch { }    }

    // [31] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.IIUnknownCacheStrategy::ConstructTableInfo:System.Runtime.InteropServices.Marshalling.IIUnknownCacheStrategy+TableInfo(System.RuntimeTypeHandle,System.Runtime.InteropServices.Marshalling.IIUnknownDerivedDetails,System.Void*)
    [Fact]
    public static void CustomEntrySubject_31()
    {        try { _ = default(IIUnknownCacheStrategy)!.ConstructTableInfo(default(System.RuntimeTypeHandle), default, default); } catch { }    }

    // [32] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.IIUnknownCacheStrategy::TryGetTableInfo:System.Boolean(System.RuntimeTypeHandle,System.Runtime.InteropServices.Marshalling.IIUnknownCacheStrategy+TableInfo&)
    [Fact]
    public static void CustomEntrySubject_32()
    {        try { _ = default(IIUnknownCacheStrategy)!.TryGetTableInfo(default(System.RuntimeTypeHandle), out default); } catch { }    }

    // [33] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.IIUnknownCacheStrategy::TrySetTableInfo:System.Boolean(System.RuntimeTypeHandle,System.Runtime.InteropServices.Marshalling.IIUnknownCacheStrategy+TableInfo)
    [Fact]
    public static void CustomEntrySubject_33()
    {        try { _ = default(IIUnknownCacheStrategy)!.TrySetTableInfo(default(System.RuntimeTypeHandle), default); } catch { }    }

    // [34] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.IIUnknownCacheStrategy+TableInfo::get_ManagedType:System.RuntimeTypeHandle()
    [Fact]
    public static void CustomEntrySubject_34()
    {        try { _ = IIUnknownCacheStrategy.TableInfo.ManagedType; } catch { }    }

    // [35] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.IIUnknownCacheStrategy+TableInfo::get_Table:System.Void**()
    [Fact]
    public static void CustomEntrySubject_35()
    {        try { _ = IIUnknownCacheStrategy.TableInfo.Table; } catch { }    }

    // [36] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.IIUnknownCacheStrategy+TableInfo::get_ThisPtr:System.Void*()
    [Fact]
    public static void CustomEntrySubject_36()
    {        try { _ = IIUnknownCacheStrategy.TableInfo.ThisPtr; } catch { }    }

    // [37] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.IIUnknownCacheStrategy+TableInfo::set_ManagedType:System.Void(System.RuntimeTypeHandle)
    [Fact]
    public static void CustomEntrySubject_37()
    {        try { IIUnknownCacheStrategy.TableInfo.ManagedType = default(System.RuntimeTypeHandle); } catch { }    }

    // [38] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.IIUnknownCacheStrategy+TableInfo::set_Table:System.Void(System.Void**)
    [Fact]
    public static void CustomEntrySubject_38()
    {        try { IIUnknownCacheStrategy.TableInfo.Table = default; } catch { }    }

    // [39] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.IIUnknownCacheStrategy+TableInfo::set_ThisPtr:System.Void(System.Void*)
    [Fact]
    public static void CustomEntrySubject_39()
    {        try { IIUnknownCacheStrategy.TableInfo.ThisPtr = default; } catch { }    }

    // [40] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.IIUnknownDerivedDetails::get_Iid:System.Guid()
    [Fact]
    public static void CustomEntrySubject_40()
    {        try { _ = default(IIUnknownDerivedDetails)!.Iid; } catch { }    }

    // [41] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.IIUnknownDerivedDetails::get_Implementation:System.Type()
    [Fact]
    public static void CustomEntrySubject_41()
    {        try { _ = default(IIUnknownDerivedDetails)!.Implementation; } catch { }    }

    // [42] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.IIUnknownDerivedDetails::get_ManagedVirtualMethodTable:System.Void**()
    [Fact]
    public static void CustomEntrySubject_42()
    {        try { _ = default(IIUnknownDerivedDetails)!.ManagedVirtualMethodTable; } catch { }    }

    // [43] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.IIUnknownInterfaceDetailsStrategy::GetComExposedTypeDetails:System.Runtime.InteropServices.Marshalling.IComExposedDetails(System.RuntimeTypeHandle)
    [Fact]
    public static void CustomEntrySubject_43()
    {        try { _ = IIUnknownInterfaceDetailsStrategy.GetComExposedTypeDetails(default(System.RuntimeTypeHandle)); } catch { }    }

    // [44] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.IIUnknownInterfaceDetailsStrategy::GetIUnknownDerivedDetails:System.Runtime.InteropServices.Marshalling.IIUnknownDerivedDetails(System.RuntimeTypeHandle)
    [Fact]
    public static void CustomEntrySubject_44()
    {        try { _ = IIUnknownInterfaceDetailsStrategy.GetIUnknownDerivedDetails(default(System.RuntimeTypeHandle)); } catch { }    }

    // [45] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.IIUnknownInterfaceType::get_Iid:System.Guid()
    [Fact]
    public static void CustomEntrySubject_45()
    {        try { _ = default(IIUnknownInterfaceType)!.Iid; } catch { }    }

    // [46] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.IIUnknownInterfaceType::get_ManagedVirtualMethodTable:System.Void**()
    [Fact]
    public static void CustomEntrySubject_46()
    {        try { _ = default(IIUnknownInterfaceType)!.ManagedVirtualMethodTable; } catch { }    }

    // [47] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.IIUnknownStrategy::CreateInstancePointer:System.Void*(System.Void*)
    [Fact]
    public static void CustomEntrySubject_47()
    {        try { _ = IIUnknownStrategy.CreateInstancePointer(default); } catch { }    }

    // [48] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.IIUnknownStrategy::QueryInterface:System.Int32(System.Void*,System.Guid&,System.Void*&)
    [Fact]
    public static void CustomEntrySubject_48()
    {        try { _ = IIUnknownStrategy.QueryInterface(default, out Guid.Empty, out default); } catch { }    }

    // [49] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.IIUnknownStrategy::Release:System.Int32(System.Void*)
    [Fact]
    public static void CustomEntrySubject_49()
    {        try { _ = IIUnknownStrategy.Release(default); } catch { }    }

    // [50] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.IUnknownDerivedAttribute`2::get_Iid:System.Guid()
    [Fact]
    public static void CustomEntrySubject_50()
    {        try { _ = default(IUnknownDerivedAttribute)!.Iid; } catch { }    }

    // [51] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.IUnknownDerivedAttribute`2::get_Implementation:System.Type()
    [Fact]
    public static void CustomEntrySubject_51()
    {        try { _ = default(IUnknownDerivedAttribute)!.Implementation; } catch { }    }

    // [52] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.IUnknownDerivedAttribute`2::get_ManagedVirtualMethodTable:System.Void**()
    [Fact]
    public static void CustomEntrySubject_52()
    {        try { _ = default(IUnknownDerivedAttribute)!.ManagedVirtualMethodTable; } catch { }    }

    // [53] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.IUnknownDerivedAttribute`2::.ctor:System.Void()
    [Fact]
    public static void CustomEntrySubject_53()
    {        try { new IUnknownDerivedAttribute<byte, byte>(); } catch { }    }

    // [54] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.IUnmanagedVirtualMethodTableProvider::GetVirtualMethodTableInfoForKey:System.Runtime.InteropServices.Marshalling.VirtualMethodTableInfo(System.Type)
    [Fact]
    public static void CustomEntrySubject_54()
    {        try { _ = IUnmanagedVirtualMethodTableProvider.GetVirtualMethodTableInfoForKey(typeof(byte)); } catch { }    }

    // [55] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.StrategyBasedComWrappers::get_DefaultIUnknownInterfaceDetailsStrategy:System.Runtime.InteropServices.Marshalling.IIUnknownInterfaceDetailsStrategy()
    [Fact]
    public static void CustomEntrySubject_55()
    {        try { _ = new StrategyBasedComWrappers().DefaultIUnknownInterfaceDetailsStrategy; } catch { }    }

    // [56] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.StrategyBasedComWrappers::get_DefaultIUnknownStrategy:System.Runtime.InteropServices.Marshalling.IIUnknownStrategy()
    [Fact]
    public static void CustomEntrySubject_56()
    {        try { _ = new StrategyBasedComWrappers().DefaultIUnknownStrategy; } catch { }    }

    // [58] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.UniqueComInterfaceMarshaller`1::ConvertToManaged:T(System.Void*)
    [Fact]
    public static void CustomEntrySubject_58()
    {        try { _ = UniqueComInterfaceMarshaller.ConvertToManaged(default); } catch { }    }

    // [59] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.UniqueComInterfaceMarshaller`1::ConvertToUnmanaged:System.Void*(T)
    [Fact]
    public static void CustomEntrySubject_59()
    {        try { _ = UniqueComInterfaceMarshaller.ConvertToUnmanaged(default); } catch { }    }

    // [60] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.UniqueComInterfaceMarshaller`1::Free:System.Void(System.Void*)
    [Fact]
    public static void CustomEntrySubject_60()
    {        try { UniqueComInterfaceMarshaller.Free(default); } catch { }    }

    // [61] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.VirtualMethodTableInfo::Deconstruct:System.Void(System.Void*&,System.Void**&)
    [Fact]
    public static void CustomEntrySubject_61()
    {        try { default(VirtualMethodTableInfo).Deconstruct(out default, out default); } catch { }    }

    // [62] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.VirtualMethodTableInfo::get_ThisPointer:System.Void*()
    [Fact]
    public static void CustomEntrySubject_62()
    {        try { _ = default(VirtualMethodTableInfo).ThisPointer; } catch { }    }

    // [63] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.VirtualMethodTableInfo::get_VirtualMethodTable:System.Void**()
    [Fact]
    public static void CustomEntrySubject_63()
    {        try { _ = default(VirtualMethodTableInfo).VirtualMethodTable; } catch { }    }

    // [64] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.VirtualMethodTableInfo::.ctor:System.Void(System.Void*,System.Void**)
    [Fact]
    public static void CustomEntrySubject_64()
    {        try { new VirtualMethodTableInfo(default, default); } catch { }    }

}
