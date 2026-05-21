// Auto-generated native-AOT entry point
// Family: family/System.Runtime.InteropServices/generated-marshalling
// Assembly: System.Runtime.InteropServices
// Variant: benchmark

using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Runtime.InteropServices.Marshalling;

public static partial class GeneratedMarshallingNativeEntry
{
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    // [0] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.ComExposedClassAttribute`1::GetComInterfaceEntries:System.Runtime.InteropServices.ComWrappers+ComInterfaceEntry*(System.Int32&)
    public static void Method0()
    {
    var refLocal_0 = 42;
        try { if ((int)(ComExposedClassAttribute.GetComInterfaceEntries(ref refLocal_0)) != (int)(ComExposedClassAttribute.GetComInterfaceEntries(ref refLocal_0))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [1] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.ComExposedClassAttribute`1::.ctor:System.Void()
    public static void Method1()
    {
        try { new ComExposedClassAttribute(); }
        catch { _exitCode = 1; }
    }

    // [2] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.ComInterfaceMarshaller`1::ConvertToManaged:T(System.Void*)
    public static void Method2()
    {
        // TODO: System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.ComInterfaceMarshaller`1::ConvertToManaged:T(System.Void*) could not be auto-generated
    }

    // [3] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.ComInterfaceMarshaller`1::ConvertToUnmanaged:System.Void*(T)
    public static void Method3()
    {
        // TODO: System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.ComInterfaceMarshaller`1::ConvertToUnmanaged:System.Void*(T) could not be auto-generated
    }

    // [4] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.ComInterfaceMarshaller`1::Free:System.Void(System.Void*)
    public static void Method4()
    {
        // TODO: System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.ComInterfaceMarshaller`1::Free:System.Void(System.Void*) could not be auto-generated
    }

    // [5] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.ComObject::FinalRelease:System.Void()
    public static void Method5()
    {
        try { ComObject.FinalRelease(); }
        catch { _exitCode = 1; }
    }

    // [6] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.ComVariantMarshaller::ConvertToManaged:System.Object(System.Runtime.InteropServices.Marshalling.ComVariant)
    public static void Method6()
    {
        try { if (((ComVariantMarshaller.ConvertToManaged(null!)).GetHashCode()) != ((ComVariantMarshaller.ConvertToManaged(null!)).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [7] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.ComVariantMarshaller::ConvertToUnmanaged:System.Runtime.InteropServices.Marshalling.ComVariant(System.Object)
    public static void Method7()
    {
        try { if ((int)(ComVariantMarshaller.ConvertToUnmanaged(42)) != (int)(ComVariantMarshaller.ConvertToUnmanaged(42))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [8] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.ComVariantMarshaller::Free:System.Void(System.Runtime.InteropServices.Marshalling.ComVariant)
    public static void Method8()
    {
        try { ComVariantMarshaller.Free(null!); }
        catch { _exitCode = 1; }
    }

    // [9] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.ComVariantMarshaller+RefPropagate::Free:System.Void()
    public static void Method9()
    {
        try { ComVariantMarshaller+RefPropagate.Free(); }
        catch { _exitCode = 1; }
    }

    // [10] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.ComVariantMarshaller+RefPropagate::FromManaged:System.Void(System.Object)
    public static void Method10()
    {
        try { ComVariantMarshaller+RefPropagate.FromManaged(42); }
        catch { _exitCode = 1; }
    }

    // [11] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.ComVariantMarshaller+RefPropagate::FromUnmanaged:System.Void(System.Runtime.InteropServices.Marshalling.ComVariant)
    public static void Method11()
    {
        try { ComVariantMarshaller+RefPropagate.FromUnmanaged(null!); }
        catch { _exitCode = 1; }
    }

    // [12] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.ComVariantMarshaller+RefPropagate::ToManaged:System.Object()
    public static void Method12()
    {
        try { if (((ComVariantMarshaller+RefPropagate.ToManaged()).GetHashCode()) != ((ComVariantMarshaller+RefPropagate.ToManaged()).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [13] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.ComVariantMarshaller+RefPropagate::ToUnmanaged:System.Runtime.InteropServices.Marshalling.ComVariant()
    public static void Method13()
    {
        try { if ((int)(ComVariantMarshaller+RefPropagate.ToUnmanaged()) != (int)(ComVariantMarshaller+RefPropagate.ToUnmanaged())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [14] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.ExceptionAsDefaultMarshaller`1::ConvertToUnmanaged:T(System.Exception)
    public static void Method14()
    {
        try { if ((int)(ExceptionAsDefaultMarshaller.ConvertToUnmanaged(null!)) != (int)(ExceptionAsDefaultMarshaller.ConvertToUnmanaged(null!))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [15] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.ExceptionAsHResultMarshaller`1::ConvertToUnmanaged:T(System.Exception)
    public static void Method15()
    {
        try { if ((int)(ExceptionAsHResultMarshaller.ConvertToUnmanaged(null!)) != (int)(ExceptionAsHResultMarshaller.ConvertToUnmanaged(null!))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [16] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.ExceptionAsNaNMarshaller`1::ConvertToUnmanaged:T(System.Exception)
    public static void Method16()
    {
        try { if ((int)(ExceptionAsNaNMarshaller.ConvertToUnmanaged(null!)) != (int)(ExceptionAsNaNMarshaller.ConvertToUnmanaged(null!))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [17] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.ExceptionAsVoidMarshaller::ConvertToUnmanaged:System.Void(System.Exception)
    public static void Method17()
    {
        try { ExceptionAsVoidMarshaller.ConvertToUnmanaged(null!); }
        catch { _exitCode = 1; }
    }

    // [18] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.GeneratedComClassAttribute::.ctor:System.Void()
    public static void Method18()
    {
        try { new GeneratedComClassAttribute(); }
        catch { _exitCode = 1; }
    }

    // [19] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.GeneratedComInterfaceAttribute::get_ExceptionToUnmanagedMarshaller:System.Type()
    public static void Method19()
    {
        try { if (((GeneratedComInterfaceAttribute.ExceptionToUnmanagedMarshaller).GetHashCode()) != ((GeneratedComInterfaceAttribute.ExceptionToUnmanagedMarshaller).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [20] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.GeneratedComInterfaceAttribute::get_Options:System.Runtime.InteropServices.Marshalling.ComInterfaceOptions()
    public static void Method20()
    {
        try { if ((int)(GeneratedComInterfaceAttribute.Options) != (int)(GeneratedComInterfaceAttribute.Options)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [21] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.GeneratedComInterfaceAttribute::get_StringMarshalling:System.Runtime.InteropServices.StringMarshalling()
    public static void Method21()
    {
        try { if ((int)(GeneratedComInterfaceAttribute.StringMarshalling) != (int)(GeneratedComInterfaceAttribute.StringMarshalling)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [22] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.GeneratedComInterfaceAttribute::get_StringMarshallingCustomType:System.Type()
    public static void Method22()
    {
        try { if (((GeneratedComInterfaceAttribute.StringMarshallingCustomType).GetHashCode()) != ((GeneratedComInterfaceAttribute.StringMarshallingCustomType).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [23] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.GeneratedComInterfaceAttribute::set_ExceptionToUnmanagedMarshaller:System.Void(System.Type)
    public static void Method23()
    {
        try { GeneratedComInterfaceAttribute.ExceptionToUnmanagedMarshaller = typeof(byte); }
        catch { _exitCode = 1; }
    }

    // [24] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.GeneratedComInterfaceAttribute::set_Options:System.Void(System.Runtime.InteropServices.Marshalling.ComInterfaceOptions)
    public static void Method24()
    {
        try { GeneratedComInterfaceAttribute.Options = null!; }
        catch { _exitCode = 1; }
    }

    // [25] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.GeneratedComInterfaceAttribute::set_StringMarshalling:System.Void(System.Runtime.InteropServices.StringMarshalling)
    public static void Method25()
    {
        try { GeneratedComInterfaceAttribute.StringMarshalling = null!; }
        catch { _exitCode = 1; }
    }

    // [26] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.GeneratedComInterfaceAttribute::set_StringMarshallingCustomType:System.Void(System.Type)
    public static void Method26()
    {
        try { GeneratedComInterfaceAttribute.StringMarshallingCustomType = typeof(byte); }
        catch { _exitCode = 1; }
    }

    // [27] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.GeneratedComInterfaceAttribute::.ctor:System.Void()
    public static void Method27()
    {
        try { new GeneratedComInterfaceAttribute(); }
        catch { _exitCode = 1; }
    }

    // [28] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.IComExposedClass::GetComInterfaceEntries:System.Runtime.InteropServices.ComWrappers+ComInterfaceEntry*(System.Int32&)
    public static void Method28()
    {
    var refLocal_0 = 42;
        try { if ((int)(IComExposedClass.GetComInterfaceEntries(ref refLocal_0)) != (int)(IComExposedClass.GetComInterfaceEntries(ref refLocal_0))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [29] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.IComExposedDetails::GetComInterfaceEntries:System.Runtime.InteropServices.ComWrappers+ComInterfaceEntry*(System.Int32&)
    public static void Method29()
    {
    var refLocal_0 = 42;
        try { if ((int)(IComExposedDetails.GetComInterfaceEntries(ref refLocal_0)) != (int)(IComExposedDetails.GetComInterfaceEntries(ref refLocal_0))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [30] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.IIUnknownCacheStrategy::Clear:System.Void(System.Runtime.InteropServices.Marshalling.IIUnknownStrategy)
    public static void Method30()
    {
        try { IIUnknownCacheStrategy.Clear(null!); }
        catch { _exitCode = 1; }
    }

    // [31] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.IIUnknownCacheStrategy::ConstructTableInfo:System.Runtime.InteropServices.Marshalling.IIUnknownCacheStrategy+TableInfo(System.RuntimeTypeHandle,System.Runtime.InteropServices.Marshalling.IIUnknownDerivedDetails,System.Void*)
    public static void Method31()
    {
        // TODO: System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.IIUnknownCacheStrategy::ConstructTableInfo:System.Runtime.InteropServices.Marshalling.IIUnknownCacheStrategy+TableInfo(System.RuntimeTypeHandle,System.Runtime.InteropServices.Marshalling.IIUnknownDerivedDetails,System.Void*) could not be auto-generated
    }

    // [32] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.IIUnknownCacheStrategy::TryGetTableInfo:System.Boolean(System.RuntimeTypeHandle,System.Runtime.InteropServices.Marshalling.IIUnknownCacheStrategy+TableInfo&)
    public static void Method32()
    {
    var refLocal_1 = null!;
        try { if (((IIUnknownCacheStrategy.TryGetTableInfo(default(System.RuntimeTypeHandle), ref refLocal_1)) ? 1 : 0) != ((IIUnknownCacheStrategy.TryGetTableInfo(default(System.RuntimeTypeHandle), ref refLocal_1)) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [33] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.IIUnknownCacheStrategy::TrySetTableInfo:System.Boolean(System.RuntimeTypeHandle,System.Runtime.InteropServices.Marshalling.IIUnknownCacheStrategy+TableInfo)
    public static void Method33()
    {
        try { if (((IIUnknownCacheStrategy.TrySetTableInfo(default(System.RuntimeTypeHandle), null!)) ? 1 : 0) != ((IIUnknownCacheStrategy.TrySetTableInfo(default(System.RuntimeTypeHandle), null!)) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [34] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.IIUnknownCacheStrategy+TableInfo::get_ManagedType:System.RuntimeTypeHandle()
    public static void Method34()
    {
        try { if (((IIUnknownCacheStrategy+TableInfo.ManagedType).GetHashCode()) != ((IIUnknownCacheStrategy+TableInfo.ManagedType).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [35] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.IIUnknownCacheStrategy+TableInfo::get_Table:System.Void**()
    public static void Method35()
    {
        try { if ((int)(IIUnknownCacheStrategy+TableInfo.Table) != (int)(IIUnknownCacheStrategy+TableInfo.Table)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [36] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.IIUnknownCacheStrategy+TableInfo::get_ThisPtr:System.Void*()
    public static void Method36()
    {
        try { if ((int)(IIUnknownCacheStrategy+TableInfo.ThisPtr) != (int)(IIUnknownCacheStrategy+TableInfo.ThisPtr)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [37] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.IIUnknownCacheStrategy+TableInfo::set_ManagedType:System.Void(System.RuntimeTypeHandle)
    public static void Method37()
    {
        try { IIUnknownCacheStrategy+TableInfo.ManagedType = default(System.RuntimeTypeHandle); }
        catch { _exitCode = 1; }
    }

    // [38] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.IIUnknownCacheStrategy+TableInfo::set_Table:System.Void(System.Void**)
    public static void Method38()
    {
        // TODO: System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.IIUnknownCacheStrategy+TableInfo::set_Table:System.Void(System.Void**) could not be auto-generated
    }

    // [39] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.IIUnknownCacheStrategy+TableInfo::set_ThisPtr:System.Void(System.Void*)
    public static void Method39()
    {
        // TODO: System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.IIUnknownCacheStrategy+TableInfo::set_ThisPtr:System.Void(System.Void*) could not be auto-generated
    }

    // [40] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.IIUnknownDerivedDetails::get_Iid:System.Guid()
    public static void Method40()
    {
        try { if (((IIUnknownDerivedDetails.Iid).GetHashCode()) != ((IIUnknownDerivedDetails.Iid).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [41] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.IIUnknownDerivedDetails::get_Implementation:System.Type()
    public static void Method41()
    {
        try { if (((IIUnknownDerivedDetails.Implementation).GetHashCode()) != ((IIUnknownDerivedDetails.Implementation).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [42] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.IIUnknownDerivedDetails::get_ManagedVirtualMethodTable:System.Void**()
    public static void Method42()
    {
        try { if ((int)(IIUnknownDerivedDetails.ManagedVirtualMethodTable) != (int)(IIUnknownDerivedDetails.ManagedVirtualMethodTable)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [43] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.IIUnknownInterfaceDetailsStrategy::GetComExposedTypeDetails:System.Runtime.InteropServices.Marshalling.IComExposedDetails(System.RuntimeTypeHandle)
    public static void Method43()
    {
        try { if ((int)(IIUnknownInterfaceDetailsStrategy.GetComExposedTypeDetails(default(System.RuntimeTypeHandle))) != (int)(IIUnknownInterfaceDetailsStrategy.GetComExposedTypeDetails(default(System.RuntimeTypeHandle)))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [44] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.IIUnknownInterfaceDetailsStrategy::GetIUnknownDerivedDetails:System.Runtime.InteropServices.Marshalling.IIUnknownDerivedDetails(System.RuntimeTypeHandle)
    public static void Method44()
    {
        try { if ((int)(IIUnknownInterfaceDetailsStrategy.GetIUnknownDerivedDetails(default(System.RuntimeTypeHandle))) != (int)(IIUnknownInterfaceDetailsStrategy.GetIUnknownDerivedDetails(default(System.RuntimeTypeHandle)))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [45] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.IIUnknownInterfaceType::get_Iid:System.Guid()
    public static void Method45()
    {
        try { if (((IIUnknownInterfaceType.Iid).GetHashCode()) != ((IIUnknownInterfaceType.Iid).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [46] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.IIUnknownInterfaceType::get_ManagedVirtualMethodTable:System.Void**()
    public static void Method46()
    {
        try { if ((int)(IIUnknownInterfaceType.ManagedVirtualMethodTable) != (int)(IIUnknownInterfaceType.ManagedVirtualMethodTable)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [47] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.IIUnknownStrategy::CreateInstancePointer:System.Void*(System.Void*)
    public static void Method47()
    {
        // TODO: System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.IIUnknownStrategy::CreateInstancePointer:System.Void*(System.Void*) could not be auto-generated
    }

    // [48] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.IIUnknownStrategy::QueryInterface:System.Int32(System.Void*,System.Guid&,System.Void*&)
    public static void Method48()
    {
        // TODO: System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.IIUnknownStrategy::QueryInterface:System.Int32(System.Void*,System.Guid&,System.Void*&) could not be auto-generated
    }

    // [49] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.IIUnknownStrategy::Release:System.Int32(System.Void*)
    public static void Method49()
    {
        // TODO: System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.IIUnknownStrategy::Release:System.Int32(System.Void*) could not be auto-generated
    }

    // [50] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.IUnknownDerivedAttribute`2::get_Iid:System.Guid()
    public static void Method50()
    {
        try { if (((IUnknownDerivedAttribute.Iid).GetHashCode()) != ((IUnknownDerivedAttribute.Iid).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [51] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.IUnknownDerivedAttribute`2::get_Implementation:System.Type()
    public static void Method51()
    {
        try { if (((IUnknownDerivedAttribute.Implementation).GetHashCode()) != ((IUnknownDerivedAttribute.Implementation).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [52] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.IUnknownDerivedAttribute`2::get_ManagedVirtualMethodTable:System.Void**()
    public static void Method52()
    {
        try { if ((int)(IUnknownDerivedAttribute.ManagedVirtualMethodTable) != (int)(IUnknownDerivedAttribute.ManagedVirtualMethodTable)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [53] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.IUnknownDerivedAttribute`2::.ctor:System.Void()
    public static void Method53()
    {
        try { new IUnknownDerivedAttribute(); }
        catch { _exitCode = 1; }
    }

    // [54] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.IUnmanagedVirtualMethodTableProvider::GetVirtualMethodTableInfoForKey:System.Runtime.InteropServices.Marshalling.VirtualMethodTableInfo(System.Type)
    public static void Method54()
    {
        try { if ((int)(IUnmanagedVirtualMethodTableProvider.GetVirtualMethodTableInfoForKey(typeof(byte))) != (int)(IUnmanagedVirtualMethodTableProvider.GetVirtualMethodTableInfoForKey(typeof(byte)))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [55] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.StrategyBasedComWrappers::get_DefaultIUnknownInterfaceDetailsStrategy:System.Runtime.InteropServices.Marshalling.IIUnknownInterfaceDetailsStrategy()
    public static void Method55()
    {
        try { if ((int)(StrategyBasedComWrappers.DefaultIUnknownInterfaceDetailsStrategy) != (int)(StrategyBasedComWrappers.DefaultIUnknownInterfaceDetailsStrategy)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [56] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.StrategyBasedComWrappers::get_DefaultIUnknownStrategy:System.Runtime.InteropServices.Marshalling.IIUnknownStrategy()
    public static void Method56()
    {
        try { if ((int)(StrategyBasedComWrappers.DefaultIUnknownStrategy) != (int)(StrategyBasedComWrappers.DefaultIUnknownStrategy)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [57] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.StrategyBasedComWrappers::.ctor:System.Void()
    public static void Method57()
    {
        try { new StrategyBasedComWrappers(); }
        catch { _exitCode = 1; }
    }

    // [58] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.UniqueComInterfaceMarshaller`1::ConvertToManaged:T(System.Void*)
    public static void Method58()
    {
        // TODO: System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.UniqueComInterfaceMarshaller`1::ConvertToManaged:T(System.Void*) could not be auto-generated
    }

    // [59] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.UniqueComInterfaceMarshaller`1::ConvertToUnmanaged:System.Void*(T)
    public static void Method59()
    {
        // TODO: System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.UniqueComInterfaceMarshaller`1::ConvertToUnmanaged:System.Void*(T) could not be auto-generated
    }

    // [60] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.UniqueComInterfaceMarshaller`1::Free:System.Void(System.Void*)
    public static void Method60()
    {
        // TODO: System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.UniqueComInterfaceMarshaller`1::Free:System.Void(System.Void*) could not be auto-generated
    }

    // [61] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.VirtualMethodTableInfo::Deconstruct:System.Void(System.Void*&,System.Void**&)
    public static void Method61()
    {
    var refLocal_0 = null!;
    var refLocal_1 = null!;
        try { VirtualMethodTableInfo.Deconstruct(ref refLocal_0, ref refLocal_1); }
        catch { _exitCode = 1; }
    }

    // [62] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.VirtualMethodTableInfo::get_ThisPointer:System.Void*()
    public static void Method62()
    {
        try { if ((int)(VirtualMethodTableInfo.ThisPointer) != (int)(VirtualMethodTableInfo.ThisPointer)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [63] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.VirtualMethodTableInfo::get_VirtualMethodTable:System.Void**()
    public static void Method63()
    {
        try { if ((int)(VirtualMethodTableInfo.VirtualMethodTable) != (int)(VirtualMethodTableInfo.VirtualMethodTable)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [64] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.VirtualMethodTableInfo::.ctor:System.Void(System.Void*,System.Void**)
    public static void Method64()
    {
        // TODO: System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.VirtualMethodTableInfo::.ctor:System.Void(System.Void*,System.Void**) could not be auto-generated
    }

    // [65] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.CustomMarshallerAttribute::get_ManagedType:System.Type()
    public static void Method65()
    {
        try { if (((CustomMarshallerAttribute.ManagedType).GetHashCode()) != ((CustomMarshallerAttribute.ManagedType).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [66] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.CustomMarshallerAttribute::get_MarshallerType:System.Type()
    public static void Method66()
    {
        try { if (((CustomMarshallerAttribute.MarshallerType).GetHashCode()) != ((CustomMarshallerAttribute.MarshallerType).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [67] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.CustomMarshallerAttribute::get_MarshalMode:System.Runtime.InteropServices.Marshalling.MarshalMode()
    public static void Method67()
    {
        try { if ((int)(CustomMarshallerAttribute.MarshalMode) != (int)(CustomMarshallerAttribute.MarshalMode)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [68] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshalling.CustomMarshallerAttribute::.ctor:System.Void(System.Type,System.Runtime.InteropServices.Marshalling.MarshalMode,System.Type)
    public static void Method68()
    {
        try { new CustomMarshallerAttribute(typeof(byte), null!, typeof(byte)); }
        catch { _exitCode = 1; }
    }

    public static void Run(int entryIndex)
    {
        try
        {
            switch (entryIndex)
            {
                case 0: Method0(); break;
                case 1: Method1(); break;
                case 2: Method2(); break;
                case 3: Method3(); break;
                case 4: Method4(); break;
                case 5: Method5(); break;
                case 6: Method6(); break;
                case 7: Method7(); break;
                case 8: Method8(); break;
                case 9: Method9(); break;
                case 10: Method10(); break;
                case 11: Method11(); break;
                case 12: Method12(); break;
                case 13: Method13(); break;
                case 14: Method14(); break;
                case 15: Method15(); break;
                case 16: Method16(); break;
                case 17: Method17(); break;
                case 18: Method18(); break;
                case 19: Method19(); break;
                case 20: Method20(); break;
                case 21: Method21(); break;
                case 22: Method22(); break;
                case 23: Method23(); break;
                case 24: Method24(); break;
                case 25: Method25(); break;
                case 26: Method26(); break;
                case 27: Method27(); break;
                case 28: Method28(); break;
                case 29: Method29(); break;
                case 30: Method30(); break;
                case 31: Method31(); break;
                case 32: Method32(); break;
                case 33: Method33(); break;
                case 34: Method34(); break;
                case 35: Method35(); break;
                case 36: Method36(); break;
                case 37: Method37(); break;
                case 38: Method38(); break;
                case 39: Method39(); break;
                case 40: Method40(); break;
                case 41: Method41(); break;
                case 42: Method42(); break;
                case 43: Method43(); break;
                case 44: Method44(); break;
                case 45: Method45(); break;
                case 46: Method46(); break;
                case 47: Method47(); break;
                case 48: Method48(); break;
                case 49: Method49(); break;
                case 50: Method50(); break;
                case 51: Method51(); break;
                case 52: Method52(); break;
                case 53: Method53(); break;
                case 54: Method54(); break;
                case 55: Method55(); break;
                case 56: Method56(); break;
                case 57: Method57(); break;
                case 58: Method58(); break;
                case 59: Method59(); break;
                case 60: Method60(); break;
                case 61: Method61(); break;
                case 62: Method62(); break;
                case 63: Method63(); break;
                case 64: Method64(); break;
                case 65: Method65(); break;
                case 66: Method66(); break;
                case 67: Method67(); break;
                case 68: Method68(); break;
            }
        }
        catch
        {
            _exitCode = 1;
        }
    }

}