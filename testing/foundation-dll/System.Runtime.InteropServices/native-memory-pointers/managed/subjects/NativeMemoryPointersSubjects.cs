// Auto-generated native-AOT entry point
// Family: family/System.Runtime.InteropServices/native/memory/pointers
// Assembly: System.Runtime.InteropServices
// Variant: subjects

using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Reflection;
using System.Runtime.InteropServices;
using System.Runtime.InteropServices.ComTypes;
using System.Security;

public static partial class NativeMemoryPointersSubjects
{
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    // [0] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::AddRef:System.Int32(System.IntPtr)
    public static void Subject_0()
    {
        try { if (Marshal.AddRef(IntPtr.Zero) != Marshal.AddRef(IntPtr.Zero)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [1] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::AllocCoTaskMem:System.IntPtr(System.Int32)
    public static void Subject_1()
    {
        try { if ((int)(Marshal.AllocCoTaskMem(42)) != (int)(Marshal.AllocCoTaskMem(42))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [2] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::AllocHGlobal:System.IntPtr(System.Int32)
    public static void Subject_2()
    {
        try { if ((int)(Marshal.AllocHGlobal(42)) != (int)(Marshal.AllocHGlobal(42))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [3] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::AllocHGlobal:System.IntPtr(System.IntPtr)
    public static void Subject_3()
    {
        try { if ((int)(Marshal.AllocHGlobal(IntPtr.Zero)) != (int)(Marshal.AllocHGlobal(IntPtr.Zero))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [4] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::AreComObjectsAvailableForCleanup:System.Boolean()
    public static void Subject_4()
    {
        try { if (((Marshal.AreComObjectsAvailableForCleanup()) ? 1 : 0) != ((Marshal.AreComObjectsAvailableForCleanup()) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [5] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::BindToMoniker:System.Object(System.String)
    public static void Subject_5()
    {
        try { if (((Marshal.BindToMoniker("hello")).GetHashCode()) != ((Marshal.BindToMoniker("hello")).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [6] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::ChangeWrapperHandleStrength:System.Void(System.Object,System.Boolean)
    public static void Subject_6()
    {
        try { Marshal.ChangeWrapperHandleStrength(42, true); }
        catch { _exitCode = 1; }
    }

    // [7] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::CleanupUnusedObjectsInCurrentContext:System.Void()
    public static void Subject_7()
    {
        try { Marshal.CleanupUnusedObjectsInCurrentContext(); }
        catch { _exitCode = 1; }
    }

    // [8] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::Copy:System.Void(System.Int32{},System.Int32,System.IntPtr,System.Int32)
    public static void Subject_8()
    {
        // TODO: System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::Copy:System.Void(System.Int32{},System.Int32,System.IntPtr,System.Int32) could not be auto-generated
    }

    // [9] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::Copy:System.Void(System.Char{},System.Int32,System.IntPtr,System.Int32)
    public static void Subject_9()
    {
        // TODO: System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::Copy:System.Void(System.Char{},System.Int32,System.IntPtr,System.Int32) could not be auto-generated
    }

    // [10] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::Copy:System.Void(System.Int16{},System.Int32,System.IntPtr,System.Int32)
    public static void Subject_10()
    {
        // TODO: System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::Copy:System.Void(System.Int16{},System.Int32,System.IntPtr,System.Int32) could not be auto-generated
    }

    // [11] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::Copy:System.Void(System.Int64{},System.Int32,System.IntPtr,System.Int32)
    public static void Subject_11()
    {
        // TODO: System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::Copy:System.Void(System.Int64{},System.Int32,System.IntPtr,System.Int32) could not be auto-generated
    }

    // [12] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::Copy:System.Void(System.Single{},System.Int32,System.IntPtr,System.Int32)
    public static void Subject_12()
    {
        // TODO: System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::Copy:System.Void(System.Single{},System.Int32,System.IntPtr,System.Int32) could not be auto-generated
    }

    // [13] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::Copy:System.Void(System.Double{},System.Int32,System.IntPtr,System.Int32)
    public static void Subject_13()
    {
        // TODO: System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::Copy:System.Void(System.Double{},System.Int32,System.IntPtr,System.Int32) could not be auto-generated
    }

    // [14] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::Copy:System.Void(System.Byte{},System.Int32,System.IntPtr,System.Int32)
    public static void Subject_14()
    {
        // TODO: System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::Copy:System.Void(System.Byte{},System.Int32,System.IntPtr,System.Int32) could not be auto-generated
    }

    // [15] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::Copy:System.Void(System.IntPtr{},System.Int32,System.IntPtr,System.Int32)
    public static void Subject_15()
    {
        // TODO: System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::Copy:System.Void(System.IntPtr{},System.Int32,System.IntPtr,System.Int32) could not be auto-generated
    }

    // [16] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::Copy:System.Void(System.IntPtr,System.Int32{},System.Int32,System.Int32)
    public static void Subject_16()
    {
        // TODO: System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::Copy:System.Void(System.IntPtr,System.Int32{},System.Int32,System.Int32) could not be auto-generated
    }

    // [17] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::Copy:System.Void(System.IntPtr,System.Char{},System.Int32,System.Int32)
    public static void Subject_17()
    {
        // TODO: System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::Copy:System.Void(System.IntPtr,System.Char{},System.Int32,System.Int32) could not be auto-generated
    }

    // [18] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::Copy:System.Void(System.IntPtr,System.Int16{},System.Int32,System.Int32)
    public static void Subject_18()
    {
        // TODO: System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::Copy:System.Void(System.IntPtr,System.Int16{},System.Int32,System.Int32) could not be auto-generated
    }

    // [19] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::Copy:System.Void(System.IntPtr,System.Int64{},System.Int32,System.Int32)
    public static void Subject_19()
    {
        // TODO: System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::Copy:System.Void(System.IntPtr,System.Int64{},System.Int32,System.Int32) could not be auto-generated
    }

    // [20] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::Copy:System.Void(System.IntPtr,System.Single{},System.Int32,System.Int32)
    public static void Subject_20()
    {
        // TODO: System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::Copy:System.Void(System.IntPtr,System.Single{},System.Int32,System.Int32) could not be auto-generated
    }

    // [21] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::Copy:System.Void(System.IntPtr,System.Double{},System.Int32,System.Int32)
    public static void Subject_21()
    {
        // TODO: System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::Copy:System.Void(System.IntPtr,System.Double{},System.Int32,System.Int32) could not be auto-generated
    }

    // [22] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::Copy:System.Void(System.IntPtr,System.Byte{},System.Int32,System.Int32)
    public static void Subject_22()
    {
        // TODO: System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::Copy:System.Void(System.IntPtr,System.Byte{},System.Int32,System.Int32) could not be auto-generated
    }

    // [23] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::Copy:System.Void(System.IntPtr,System.IntPtr{},System.Int32,System.Int32)
    public static void Subject_23()
    {
        // TODO: System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::Copy:System.Void(System.IntPtr,System.IntPtr{},System.Int32,System.Int32) could not be auto-generated
    }

    // [24] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::CreateAggregatedObject:System.IntPtr(System.IntPtr,System.Object)
    public static void Subject_24()
    {
        try { if ((int)(Marshal.CreateAggregatedObject(IntPtr.Zero, 42)) != (int)(Marshal.CreateAggregatedObject(IntPtr.Zero, 42))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [25] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::CreateAggregatedObject{T}:System.IntPtr(System.IntPtr,T)
    public static void CustomEntrySubject_25()
    {
    }

    // [26] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::CreateWrapperOfType:System.Object(System.Object,System.Type)
    public static void Subject_26()
    {
        try { if (((Marshal.CreateWrapperOfType(42, typeof(byte))).GetHashCode()) != ((Marshal.CreateWrapperOfType(42, typeof(byte))).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [27] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::CreateWrapperOfType{T,TWrapper}:TWrapper(T)
    public static void CustomEntrySubject_27()
    {
    }

    // [28] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::DestroyStructure:System.Void(System.IntPtr,System.Type)
    public static void Subject_28()
    {
        try { Marshal.DestroyStructure(IntPtr.Zero, typeof(byte)); }
        catch { _exitCode = 1; }
    }

    // [29] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::DestroyStructure{T}:System.Void(System.IntPtr)
    public static void CustomEntrySubject_29()
    {
    }

    // [30] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::FinalReleaseComObject:System.Int32(System.Object)
    public static void Subject_30()
    {
        try { if (Marshal.FinalReleaseComObject(42) != Marshal.FinalReleaseComObject(42)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [31] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::FreeBSTR:System.Void(System.IntPtr)
    public static void Subject_31()
    {
        try { Marshal.FreeBSTR(IntPtr.Zero); }
        catch { _exitCode = 1; }
    }

    // [32] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::FreeCoTaskMem:System.Void(System.IntPtr)
    public static void Subject_32()
    {
        try { Marshal.FreeCoTaskMem(IntPtr.Zero); }
        catch { _exitCode = 1; }
    }

    // [33] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::FreeHGlobal:System.Void(System.IntPtr)
    public static void Subject_33()
    {
        try { Marshal.FreeHGlobal(IntPtr.Zero); }
        catch { _exitCode = 1; }
    }

    // [34] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::GenerateGuidForType:System.Guid(System.Type)
    public static void Subject_34()
    {
        try { if (((Marshal.GenerateGuidForType(typeof(byte))).GetHashCode()) != ((Marshal.GenerateGuidForType(typeof(byte))).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [35] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::GenerateProgIdForType:System.String(System.Type)
    public static void Subject_35()
    {
        try { if (((Marshal.GenerateProgIdForType(typeof(byte))).Length) != ((Marshal.GenerateProgIdForType(typeof(byte))).Length)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [36] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::GetComInterfaceForObject:System.IntPtr(System.Object,System.Type)
    public static void Subject_36()
    {
        try { if ((int)(Marshal.GetComInterfaceForObject(42, typeof(byte))) != (int)(Marshal.GetComInterfaceForObject(42, typeof(byte)))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [37] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::GetComInterfaceForObject{T,TInterface}:System.IntPtr(T)
    public static void CustomEntrySubject_37()
    {
    }

    // [38] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::GetComInterfaceForObject:System.IntPtr(System.Object,System.Type,System.Runtime.InteropServices.CustomQueryInterfaceMode)
    public static void Subject_38()
    {
        try { if ((int)(Marshal.GetComInterfaceForObject(42, typeof(byte), default)) != (int)(Marshal.GetComInterfaceForObject(42, typeof(byte), default))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [39] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::GetComObjectData:System.Object(System.Object,System.Object)
    public static void Subject_39()
    {
        try { if (((Marshal.GetComObjectData(42, 42)).GetHashCode()) != ((Marshal.GetComObjectData(42, 42)).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [40] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::GetDelegateForFunctionPointer:System.Delegate(System.IntPtr,System.Type)
    public static void Subject_40()
    {
        try { if (((Marshal.GetDelegateForFunctionPointer(IntPtr.Zero, typeof(byte))).GetHashCode()) != ((Marshal.GetDelegateForFunctionPointer(IntPtr.Zero, typeof(byte))).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [41] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::GetDelegateForFunctionPointer{TDelegate}:TDelegate(System.IntPtr)
    public static void CustomEntrySubject_41()
    {
    }

    // [42] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::GetEndComSlot:System.Int32(System.Type)
    public static void Subject_42()
    {
        try { if (Marshal.GetEndComSlot(typeof(byte)) != Marshal.GetEndComSlot(typeof(byte))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [43] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::GetExceptionCode:System.Int32()
    public static void Subject_43()
    {
        try { if (Marshal.GetExceptionCode() != Marshal.GetExceptionCode()) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [44] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::GetExceptionForHR:System.Exception(System.Int32)
    public static void Subject_44()
    {
        try { if (((Marshal.GetExceptionForHR(42)).GetHashCode()) != ((Marshal.GetExceptionForHR(42)).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [45] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::GetExceptionForHR:System.Exception(System.Int32,System.IntPtr)
    public static void Subject_45()
    {
        try { if (((Marshal.GetExceptionForHR(42, IntPtr.Zero)).GetHashCode()) != ((Marshal.GetExceptionForHR(42, IntPtr.Zero)).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [46] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::GetExceptionForHR:System.Exception(System.Int32,System.Guid&,System.IntPtr)
    public static void Subject_46()
    {
        try { if (((Marshal.GetExceptionForHR(42, default(Guid), IntPtr.Zero)).GetHashCode()) != ((Marshal.GetExceptionForHR(42, default(Guid), IntPtr.Zero)).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [47] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::GetExceptionPointers:System.IntPtr()
    public static void Subject_47()
    {
        try { if ((int)(Marshal.GetExceptionPointers()) != (int)(Marshal.GetExceptionPointers())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [48] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::GetFunctionPointerForDelegate:System.IntPtr(System.Delegate)
    public static void Subject_48()
    {
        try { if ((int)(Marshal.GetFunctionPointerForDelegate(default)) != (int)(Marshal.GetFunctionPointerForDelegate(default))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [49] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::GetFunctionPointerForDelegate{TDelegate}:System.IntPtr(TDelegate)
    public static void CustomEntrySubject_49()
    {
    }

    // [50] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::GetHINSTANCE:System.IntPtr(System.Reflection.Module)
    public static void Subject_50()
    {
        try { if ((int)(Marshal.GetHINSTANCE(null!)) != (int)(Marshal.GetHINSTANCE(null!))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [51] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::GetHRForException:System.Int32(System.Exception)
    public static void Subject_51()
    {
        try { if (Marshal.GetHRForException(default) != Marshal.GetHRForException(default)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [52] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::GetHRForLastWin32Error:System.Int32()
    public static void Subject_52()
    {
        try { if (Marshal.GetHRForLastWin32Error() != Marshal.GetHRForLastWin32Error()) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [53] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::GetIDispatchForObject:System.IntPtr(System.Object)
    public static void Subject_53()
    {
        try { if ((int)(Marshal.GetIDispatchForObject(42)) != (int)(Marshal.GetIDispatchForObject(42))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [54] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::GetIUnknownForObject:System.IntPtr(System.Object)
    public static void Subject_54()
    {
        try { if ((int)(Marshal.GetIUnknownForObject(42)) != (int)(Marshal.GetIUnknownForObject(42))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [55] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::GetLastPInvokeError:System.Int32()
    public static void Subject_55()
    {
        try { if (Marshal.GetLastPInvokeError() != Marshal.GetLastPInvokeError()) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [56] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::GetLastPInvokeErrorMessage:System.String()
    public static void Subject_56()
    {
        try { if (((Marshal.GetLastPInvokeErrorMessage()).Length) != ((Marshal.GetLastPInvokeErrorMessage()).Length)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [57] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::GetLastSystemError:System.Int32()
    public static void Subject_57()
    {
        try { if (Marshal.GetLastSystemError() != Marshal.GetLastSystemError()) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [58] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::GetLastWin32Error:System.Int32()
    public static void Subject_58()
    {
        try { if (Marshal.GetLastWin32Error() != Marshal.GetLastWin32Error()) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [59] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::GetNativeVariantForObject:System.Void(System.Object,System.IntPtr)
    public static void Subject_59()
    {
        try { Marshal.GetNativeVariantForObject(42, IntPtr.Zero); }
        catch { _exitCode = 1; }
    }

    // [60] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::GetNativeVariantForObject{T}:System.Void(T,System.IntPtr)
    public static void CustomEntrySubject_60()
    {
    }

    // [61] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::GetObjectForIUnknown:System.Object(System.IntPtr)
    public static void Subject_61()
    {
        try { if (((Marshal.GetObjectForIUnknown(IntPtr.Zero)).GetHashCode()) != ((Marshal.GetObjectForIUnknown(IntPtr.Zero)).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [62] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::GetObjectForNativeVariant:System.Object(System.IntPtr)
    public static void Subject_62()
    {
        try { if (((Marshal.GetObjectForNativeVariant(IntPtr.Zero)).GetHashCode()) != ((Marshal.GetObjectForNativeVariant(IntPtr.Zero)).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [63] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::GetObjectForNativeVariant{T}:T(System.IntPtr)
    public static void CustomEntrySubject_63()
    {
    }

    // [64] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::GetObjectsForNativeVariants:System.Object{}(System.IntPtr,System.Int32)
    public static void Subject_64()
    {
        try { if (((Marshal.GetObjectsForNativeVariants(IntPtr.Zero, 42)).Length) != ((Marshal.GetObjectsForNativeVariants(IntPtr.Zero, 42)).Length)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [65] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::GetObjectsForNativeVariants{T}:T{}(System.IntPtr,System.Int32)
    public static void CustomEntrySubject_65()
    {
    }

    // [66] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::GetPInvokeErrorMessage:System.String(System.Int32)
    public static void Subject_66()
    {
        try { if (((Marshal.GetPInvokeErrorMessage(42)).Length) != ((Marshal.GetPInvokeErrorMessage(42)).Length)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [67] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::GetStartComSlot:System.Int32(System.Type)
    public static void Subject_67()
    {
        try { if (Marshal.GetStartComSlot(typeof(byte)) != Marshal.GetStartComSlot(typeof(byte))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [68] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::GetTypedObjectForIUnknown:System.Object(System.IntPtr,System.Type)
    public static void Subject_68()
    {
        try { if (((Marshal.GetTypedObjectForIUnknown(IntPtr.Zero, typeof(byte))).GetHashCode()) != ((Marshal.GetTypedObjectForIUnknown(IntPtr.Zero, typeof(byte))).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [69] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::GetTypeFromCLSID:System.Type(System.Guid)
    public static void Subject_69()
    {
        try { if (((Marshal.GetTypeFromCLSID(Guid.Empty)).GetHashCode()) != ((Marshal.GetTypeFromCLSID(Guid.Empty)).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [70] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::GetTypeInfoName:System.String(System.Runtime.InteropServices.ComTypes.ITypeInfo)
    public static void Subject_70()
    {
        try { if (((Marshal.GetTypeInfoName(default)).Length) != ((Marshal.GetTypeInfoName(default)).Length)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [71] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::GetUniqueObjectForIUnknown:System.Object(System.IntPtr)
    public static void Subject_71()
    {
        try { if (((Marshal.GetUniqueObjectForIUnknown(IntPtr.Zero)).GetHashCode()) != ((Marshal.GetUniqueObjectForIUnknown(IntPtr.Zero)).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [72] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::InitHandle:System.Void(System.Runtime.InteropServices.SafeHandle,System.IntPtr)
    public static void Subject_72()
    {
        try { Marshal.InitHandle(default, IntPtr.Zero); }
        catch { _exitCode = 1; }
    }

    // [73] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::IsComObject:System.Boolean(System.Object)
    public static void Subject_73()
    {
        try { if (((Marshal.IsComObject(42)) ? 1 : 0) != ((Marshal.IsComObject(42)) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [74] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::IsTypeVisibleFromCom:System.Boolean(System.Type)
    public static void Subject_74()
    {
        try { if (((Marshal.IsTypeVisibleFromCom(typeof(byte))) ? 1 : 0) != ((Marshal.IsTypeVisibleFromCom(typeof(byte))) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [75] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::OffsetOf:System.IntPtr(System.Type,System.String)
    public static void Subject_75()
    {
        try { if ((int)(Marshal.OffsetOf(typeof(byte), "hello")) != (int)(Marshal.OffsetOf(typeof(byte), "hello"))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [76] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::OffsetOf{T}:System.IntPtr(System.String)
    public static void CustomEntrySubject_76()
    {
    }

    // [77] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::Prelink:System.Void(System.Reflection.MethodInfo)
    public static void Subject_77()
    {
        try { Marshal.Prelink(null!); }
        catch { _exitCode = 1; }
    }

    // [78] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::PrelinkAll:System.Void(System.Type)
    public static void Subject_78()
    {
        try { Marshal.PrelinkAll(typeof(byte)); }
        catch { _exitCode = 1; }
    }

    // [79] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::PtrToStringAnsi:System.String(System.IntPtr)
    public static void Subject_79()
    {
        try { if (((Marshal.PtrToStringAnsi(IntPtr.Zero)).Length) != ((Marshal.PtrToStringAnsi(IntPtr.Zero)).Length)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [80] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::PtrToStringAnsi:System.String(System.IntPtr,System.Int32)
    public static void Subject_80()
    {
        try { if (((Marshal.PtrToStringAnsi(IntPtr.Zero, 42)).Length) != ((Marshal.PtrToStringAnsi(IntPtr.Zero, 42)).Length)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [81] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::PtrToStringAuto:System.String(System.IntPtr,System.Int32)
    public static void Subject_81()
    {
        try { if (((Marshal.PtrToStringAuto(IntPtr.Zero, 42)).Length) != ((Marshal.PtrToStringAuto(IntPtr.Zero, 42)).Length)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [82] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::PtrToStringAuto:System.String(System.IntPtr)
    public static void Subject_82()
    {
        try { if (((Marshal.PtrToStringAuto(IntPtr.Zero)).Length) != ((Marshal.PtrToStringAuto(IntPtr.Zero)).Length)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [83] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::PtrToStringBSTR:System.String(System.IntPtr)
    public static void Subject_83()
    {
        try { if (((Marshal.PtrToStringBSTR(IntPtr.Zero)).Length) != ((Marshal.PtrToStringBSTR(IntPtr.Zero)).Length)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [84] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::PtrToStringUni:System.String(System.IntPtr)
    public static void Subject_84()
    {
        try { if (((Marshal.PtrToStringUni(IntPtr.Zero)).Length) != ((Marshal.PtrToStringUni(IntPtr.Zero)).Length)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [85] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::PtrToStringUni:System.String(System.IntPtr,System.Int32)
    public static void Subject_85()
    {
        try { if (((Marshal.PtrToStringUni(IntPtr.Zero, 42)).Length) != ((Marshal.PtrToStringUni(IntPtr.Zero, 42)).Length)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [86] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::PtrToStringUTF8:System.String(System.IntPtr)
    public static void Subject_86()
    {
        try { if (((Marshal.PtrToStringUTF8(IntPtr.Zero)).Length) != ((Marshal.PtrToStringUTF8(IntPtr.Zero)).Length)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [87] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::PtrToStringUTF8:System.String(System.IntPtr,System.Int32)
    public static void Subject_87()
    {
        try { if (((Marshal.PtrToStringUTF8(IntPtr.Zero, 42)).Length) != ((Marshal.PtrToStringUTF8(IntPtr.Zero, 42)).Length)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [88] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::PtrToStructure:System.Object(System.IntPtr,System.Type)
    public static void Subject_88()
    {
        try { if (((Marshal.PtrToStructure(IntPtr.Zero, typeof(byte))).GetHashCode()) != ((Marshal.PtrToStructure(IntPtr.Zero, typeof(byte))).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [89] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::PtrToStructure:System.Void(System.IntPtr,System.Object)
    public static void Subject_89()
    {
        try { Marshal.PtrToStructure(IntPtr.Zero, 42); }
        catch { _exitCode = 1; }
    }

    // [90] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::PtrToStructure{T}:System.Void(System.IntPtr,T)
    public static void CustomEntrySubject_90()
    {
    }

    // [91] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::PtrToStructure{T}:T(System.IntPtr)
    public static void CustomEntrySubject_91()
    {
    }

    // [92] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::QueryInterface:System.Int32(System.IntPtr,System.Guid&,System.IntPtr&)
    public static void Subject_92()
    {
        // TODO: System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::QueryInterface:System.Int32(System.IntPtr,System.Guid&,System.IntPtr&) could not be auto-generated
    }

    // [93] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::ReadByte:System.Byte(System.Object,System.Int32)
    public static void Subject_93()
    {
        try { if ((int)(Marshal.ReadByte(42, 42)) != (int)(Marshal.ReadByte(42, 42))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [94] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::ReadByte:System.Byte(System.IntPtr,System.Int32)
    public static void Subject_94()
    {
        try { if ((int)(Marshal.ReadByte(IntPtr.Zero, 42)) != (int)(Marshal.ReadByte(IntPtr.Zero, 42))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [95] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::ReadByte:System.Byte(System.IntPtr)
    public static void Subject_95()
    {
        try { if ((int)(Marshal.ReadByte(IntPtr.Zero)) != (int)(Marshal.ReadByte(IntPtr.Zero))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [96] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::ReadInt16:System.Int16(System.Object,System.Int32)
    public static void Subject_96()
    {
        try { if ((int)(Marshal.ReadInt16(42, 42)) != (int)(Marshal.ReadInt16(42, 42))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [97] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::ReadInt16:System.Int16(System.IntPtr,System.Int32)
    public static void Subject_97()
    {
        try { if ((int)(Marshal.ReadInt16(IntPtr.Zero, 42)) != (int)(Marshal.ReadInt16(IntPtr.Zero, 42))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [98] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::ReadInt16:System.Int16(System.IntPtr)
    public static void Subject_98()
    {
        try { if ((int)(Marshal.ReadInt16(IntPtr.Zero)) != (int)(Marshal.ReadInt16(IntPtr.Zero))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [99] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::ReadInt32:System.Int32(System.Object,System.Int32)
    public static void Subject_99()
    {
        try { if (Marshal.ReadInt32(42, 42) != Marshal.ReadInt32(42, 42)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [100] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::ReadInt32:System.Int32(System.IntPtr,System.Int32)
    public static void Subject_100()
    {
        try { if (Marshal.ReadInt32(IntPtr.Zero, 42) != Marshal.ReadInt32(IntPtr.Zero, 42)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [101] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::ReadInt32:System.Int32(System.IntPtr)
    public static void Subject_101()
    {
        try { if (Marshal.ReadInt32(IntPtr.Zero) != Marshal.ReadInt32(IntPtr.Zero)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [102] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::ReadInt64:System.Int64(System.Object,System.Int32)
    public static void Subject_102()
    {
        try { if ((int)(Marshal.ReadInt64(42, 42)) != (int)(Marshal.ReadInt64(42, 42))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [103] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::ReadInt64:System.Int64(System.IntPtr,System.Int32)
    public static void Subject_103()
    {
        try { if ((int)(Marshal.ReadInt64(IntPtr.Zero, 42)) != (int)(Marshal.ReadInt64(IntPtr.Zero, 42))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [104] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::ReadInt64:System.Int64(System.IntPtr)
    public static void Subject_104()
    {
        try { if ((int)(Marshal.ReadInt64(IntPtr.Zero)) != (int)(Marshal.ReadInt64(IntPtr.Zero))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [105] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::ReadIntPtr:System.IntPtr(System.Object,System.Int32)
    public static void Subject_105()
    {
        try { if ((int)(Marshal.ReadIntPtr(42, 42)) != (int)(Marshal.ReadIntPtr(42, 42))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [106] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::ReadIntPtr:System.IntPtr(System.IntPtr,System.Int32)
    public static void Subject_106()
    {
        try { if ((int)(Marshal.ReadIntPtr(IntPtr.Zero, 42)) != (int)(Marshal.ReadIntPtr(IntPtr.Zero, 42))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [107] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::ReadIntPtr:System.IntPtr(System.IntPtr)
    public static void Subject_107()
    {
        try { if ((int)(Marshal.ReadIntPtr(IntPtr.Zero)) != (int)(Marshal.ReadIntPtr(IntPtr.Zero))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [108] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::ReAllocCoTaskMem:System.IntPtr(System.IntPtr,System.Int32)
    public static void Subject_108()
    {
        try { if ((int)(Marshal.ReAllocCoTaskMem(IntPtr.Zero, 42)) != (int)(Marshal.ReAllocCoTaskMem(IntPtr.Zero, 42))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [109] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::ReAllocHGlobal:System.IntPtr(System.IntPtr,System.IntPtr)
    public static void Subject_109()
    {
        try { if ((int)(Marshal.ReAllocHGlobal(IntPtr.Zero, IntPtr.Zero)) != (int)(Marshal.ReAllocHGlobal(IntPtr.Zero, IntPtr.Zero))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [110] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::Release:System.Int32(System.IntPtr)
    public static void Subject_110()
    {
        try { if (Marshal.Release(IntPtr.Zero) != Marshal.Release(IntPtr.Zero)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [111] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::ReleaseComObject:System.Int32(System.Object)
    public static void Subject_111()
    {
        try { if (Marshal.ReleaseComObject(42) != Marshal.ReleaseComObject(42)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [112] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::SecureStringToBSTR:System.IntPtr(System.Security.SecureString)
    public static void Subject_112()
    {
        try { if ((int)(Marshal.SecureStringToBSTR(null!)) != (int)(Marshal.SecureStringToBSTR(null!))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [113] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::SecureStringToCoTaskMemAnsi:System.IntPtr(System.Security.SecureString)
    public static void Subject_113()
    {
        try { if ((int)(Marshal.SecureStringToCoTaskMemAnsi(null!)) != (int)(Marshal.SecureStringToCoTaskMemAnsi(null!))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [114] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::SecureStringToCoTaskMemUnicode:System.IntPtr(System.Security.SecureString)
    public static void Subject_114()
    {
        try { if ((int)(Marshal.SecureStringToCoTaskMemUnicode(null!)) != (int)(Marshal.SecureStringToCoTaskMemUnicode(null!))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [115] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::SecureStringToGlobalAllocAnsi:System.IntPtr(System.Security.SecureString)
    public static void Subject_115()
    {
        try { if ((int)(Marshal.SecureStringToGlobalAllocAnsi(null!)) != (int)(Marshal.SecureStringToGlobalAllocAnsi(null!))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [116] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::SecureStringToGlobalAllocUnicode:System.IntPtr(System.Security.SecureString)
    public static void Subject_116()
    {
        try { if ((int)(Marshal.SecureStringToGlobalAllocUnicode(null!)) != (int)(Marshal.SecureStringToGlobalAllocUnicode(null!))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [117] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::SetComObjectData:System.Boolean(System.Object,System.Object,System.Object)
    public static void Subject_117()
    {
        try { if (((Marshal.SetComObjectData(42, 42, 42)) ? 1 : 0) != ((Marshal.SetComObjectData(42, 42, 42)) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [118] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::SetLastPInvokeError:System.Void(System.Int32)
    public static void Subject_118()
    {
        try { Marshal.SetLastPInvokeError(42); }
        catch { _exitCode = 1; }
    }

    // [119] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::SetLastSystemError:System.Void(System.Int32)
    public static void Subject_119()
    {
        try { Marshal.SetLastSystemError(42); }
        catch { _exitCode = 1; }
    }

    // [120] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::SizeOf:System.Int32(System.Object)
    public static void Subject_120()
    {
        try { if (Marshal.SizeOf(42) != Marshal.SizeOf(42)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [121] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::SizeOf{T}:System.Int32(T)
    public static void CustomEntrySubject_121()
    {
    }

    // [122] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::SizeOf:System.Int32(System.Type)
    public static void Subject_122()
    {
        try { if (Marshal.SizeOf(typeof(byte)) != Marshal.SizeOf(typeof(byte))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [123] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::SizeOf{T}:System.Int32()
    public static void CustomEntrySubject_123()
    {
    }

    // [124] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::StringToBSTR:System.IntPtr(System.String)
    public static void Subject_124()
    {
        try { if ((int)(Marshal.StringToBSTR("hello")) != (int)(Marshal.StringToBSTR("hello"))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [125] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::StringToCoTaskMemAnsi:System.IntPtr(System.String)
    public static void Subject_125()
    {
        try { if ((int)(Marshal.StringToCoTaskMemAnsi("hello")) != (int)(Marshal.StringToCoTaskMemAnsi("hello"))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [126] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::StringToCoTaskMemAuto:System.IntPtr(System.String)
    public static void Subject_126()
    {
        try { if ((int)(Marshal.StringToCoTaskMemAuto("hello")) != (int)(Marshal.StringToCoTaskMemAuto("hello"))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [127] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::StringToCoTaskMemUni:System.IntPtr(System.String)
    public static void Subject_127()
    {
        try { if ((int)(Marshal.StringToCoTaskMemUni("hello")) != (int)(Marshal.StringToCoTaskMemUni("hello"))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [128] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::StringToCoTaskMemUTF8:System.IntPtr(System.String)
    public static void Subject_128()
    {
        try { if ((int)(Marshal.StringToCoTaskMemUTF8("hello")) != (int)(Marshal.StringToCoTaskMemUTF8("hello"))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [129] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::StringToHGlobalAnsi:System.IntPtr(System.String)
    public static void Subject_129()
    {
        try { if ((int)(Marshal.StringToHGlobalAnsi("hello")) != (int)(Marshal.StringToHGlobalAnsi("hello"))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [130] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::StringToHGlobalAuto:System.IntPtr(System.String)
    public static void Subject_130()
    {
        try { if ((int)(Marshal.StringToHGlobalAuto("hello")) != (int)(Marshal.StringToHGlobalAuto("hello"))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [131] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::StringToHGlobalUni:System.IntPtr(System.String)
    public static void Subject_131()
    {
        try { if ((int)(Marshal.StringToHGlobalUni("hello")) != (int)(Marshal.StringToHGlobalUni("hello"))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [132] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::StructureToPtr:System.Void(System.Object,System.IntPtr,System.Boolean)
    public static void Subject_132()
    {
        try { Marshal.StructureToPtr(42, IntPtr.Zero, true); }
        catch { _exitCode = 1; }
    }

    // [133] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::StructureToPtr{T}:System.Void(T,System.IntPtr,System.Boolean)
    public static void CustomEntrySubject_133()
    {
    }

    // [134] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::ThrowExceptionForHR:System.Void(System.Int32)
    public static void Subject_134()
    {
        try { Marshal.ThrowExceptionForHR(42); }
        catch { _exitCode = 1; }
    }

    // [135] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::ThrowExceptionForHR:System.Void(System.Int32,System.IntPtr)
    public static void Subject_135()
    {
        try { Marshal.ThrowExceptionForHR(42, IntPtr.Zero); }
        catch { _exitCode = 1; }
    }

    // [136] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::ThrowExceptionForHR:System.Void(System.Int32,System.Guid&,System.IntPtr)
    public static void Subject_136()
    {
        // TODO: System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::ThrowExceptionForHR:System.Void(System.Int32,System.Guid&,System.IntPtr) could not be auto-generated
    }

    // [137] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::UnsafeAddrOfPinnedArrayElement:System.IntPtr(System.Array,System.Int32)
    public static void Subject_137()
    {
        try { if ((int)(Marshal.UnsafeAddrOfPinnedArrayElement(Array.Empty<byte>(), 42)) != (int)(Marshal.UnsafeAddrOfPinnedArrayElement(Array.Empty<byte>(), 42))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [138] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::UnsafeAddrOfPinnedArrayElement{T}:System.IntPtr(T{},System.Int32)
    public static void CustomEntrySubject_138()
    {
    }

    // [139] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::WriteByte:System.Void(System.Object,System.Int32,System.Byte)
    public static void Subject_139()
    {
        try { Marshal.WriteByte(42, 42, (byte)42); }
        catch { _exitCode = 1; }
    }

    // [140] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::WriteByte:System.Void(System.IntPtr,System.Int32,System.Byte)
    public static void Subject_140()
    {
        try { Marshal.WriteByte(IntPtr.Zero, 42, (byte)42); }
        catch { _exitCode = 1; }
    }

    // [141] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::WriteByte:System.Void(System.IntPtr,System.Byte)
    public static void Subject_141()
    {
        try { Marshal.WriteByte(IntPtr.Zero, (byte)42); }
        catch { _exitCode = 1; }
    }

    // [142] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::WriteInt16:System.Void(System.Object,System.Int32,System.Int16)
    public static void Subject_142()
    {
        try { Marshal.WriteInt16(42, 42, (short)42); }
        catch { _exitCode = 1; }
    }

    // [143] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::WriteInt16:System.Void(System.IntPtr,System.Int32,System.Int16)
    public static void Subject_143()
    {
        try { Marshal.WriteInt16(IntPtr.Zero, 42, (short)42); }
        catch { _exitCode = 1; }
    }

    // [144] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::WriteInt16:System.Void(System.IntPtr,System.Int16)
    public static void Subject_144()
    {
        try { Marshal.WriteInt16(IntPtr.Zero, (short)42); }
        catch { _exitCode = 1; }
    }

    // [145] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::WriteInt16:System.Void(System.IntPtr,System.Int32,System.Char)
    public static void Subject_145()
    {
        try { Marshal.WriteInt16(IntPtr.Zero, 42, 'A'); }
        catch { _exitCode = 1; }
    }

    // [146] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::WriteInt16:System.Void(System.Object,System.Int32,System.Char)
    public static void Subject_146()
    {
        try { Marshal.WriteInt16(42, 42, 'A'); }
        catch { _exitCode = 1; }
    }

    // [147] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::WriteInt16:System.Void(System.IntPtr,System.Char)
    public static void Subject_147()
    {
        try { Marshal.WriteInt16(IntPtr.Zero, 'A'); }
        catch { _exitCode = 1; }
    }

    // [148] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::WriteInt32:System.Void(System.Object,System.Int32,System.Int32)
    public static void Subject_148()
    {
        try { Marshal.WriteInt32(42, 42, 42); }
        catch { _exitCode = 1; }
    }

    // [149] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::WriteInt32:System.Void(System.IntPtr,System.Int32,System.Int32)
    public static void Subject_149()
    {
        try { Marshal.WriteInt32(IntPtr.Zero, 42, 42); }
        catch { _exitCode = 1; }
    }

    // [150] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::WriteInt32:System.Void(System.IntPtr,System.Int32)
    public static void Subject_150()
    {
        try { Marshal.WriteInt32(IntPtr.Zero, 42); }
        catch { _exitCode = 1; }
    }

    // [151] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::WriteInt64:System.Void(System.Object,System.Int32,System.Int64)
    public static void Subject_151()
    {
        try { Marshal.WriteInt64(42, 42, 42L); }
        catch { _exitCode = 1; }
    }

    // [152] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::WriteInt64:System.Void(System.IntPtr,System.Int32,System.Int64)
    public static void Subject_152()
    {
        try { Marshal.WriteInt64(IntPtr.Zero, 42, 42L); }
        catch { _exitCode = 1; }
    }

    // [153] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::WriteInt64:System.Void(System.IntPtr,System.Int64)
    public static void Subject_153()
    {
        try { Marshal.WriteInt64(IntPtr.Zero, 42L); }
        catch { _exitCode = 1; }
    }

    // [154] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::WriteIntPtr:System.Void(System.IntPtr,System.Int32,System.IntPtr)
    public static void Subject_154()
    {
        try { Marshal.WriteIntPtr(IntPtr.Zero, 42, IntPtr.Zero); }
        catch { _exitCode = 1; }
    }

    // [155] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::WriteIntPtr:System.Void(System.Object,System.Int32,System.IntPtr)
    public static void Subject_155()
    {
        try { Marshal.WriteIntPtr(42, 42, IntPtr.Zero); }
        catch { _exitCode = 1; }
    }

    // [156] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::WriteIntPtr:System.Void(System.IntPtr,System.IntPtr)
    public static void Subject_156()
    {
        try { Marshal.WriteIntPtr(IntPtr.Zero, IntPtr.Zero); }
        catch { _exitCode = 1; }
    }

    // [157] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::ZeroFreeBSTR:System.Void(System.IntPtr)
    public static void Subject_157()
    {
        try { Marshal.ZeroFreeBSTR(IntPtr.Zero); }
        catch { _exitCode = 1; }
    }

    // [158] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::ZeroFreeCoTaskMemAnsi:System.Void(System.IntPtr)
    public static void Subject_158()
    {
        try { Marshal.ZeroFreeCoTaskMemAnsi(IntPtr.Zero); }
        catch { _exitCode = 1; }
    }

    // [159] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::ZeroFreeCoTaskMemUnicode:System.Void(System.IntPtr)
    public static void Subject_159()
    {
        try { Marshal.ZeroFreeCoTaskMemUnicode(IntPtr.Zero); }
        catch { _exitCode = 1; }
    }

    // [160] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::ZeroFreeCoTaskMemUTF8:System.Void(System.IntPtr)
    public static void Subject_160()
    {
        try { Marshal.ZeroFreeCoTaskMemUTF8(IntPtr.Zero); }
        catch { _exitCode = 1; }
    }

    // [161] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::ZeroFreeGlobalAllocAnsi:System.Void(System.IntPtr)
    public static void Subject_161()
    {
        try { Marshal.ZeroFreeGlobalAllocAnsi(IntPtr.Zero); }
        catch { _exitCode = 1; }
    }

    // [162] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::ZeroFreeGlobalAllocUnicode:System.Void(System.IntPtr)
    public static void Subject_162()
    {
        try { Marshal.ZeroFreeGlobalAllocUnicode(IntPtr.Zero); }
        catch { _exitCode = 1; }
    }

    // [163] System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::Dispose:System.Void()
    public static void Subject_163()
    {
        try { default(UnmanagedMemoryAccessor)!.Dispose(); }
        catch { _exitCode = 1; }
    }

    // [164] System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::get_CanRead:System.Boolean()
    public static void Subject_164()
    {
        try { if (((default(UnmanagedMemoryAccessor)!.CanRead) ? 1 : 0) != ((default(UnmanagedMemoryAccessor)!.CanRead) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [165] System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::get_CanWrite:System.Boolean()
    public static void Subject_165()
    {
        try { if (((default(UnmanagedMemoryAccessor)!.CanWrite) ? 1 : 0) != ((default(UnmanagedMemoryAccessor)!.CanWrite) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [166] System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::get_Capacity:System.Int64()
    public static void Subject_166()
    {
        try { if ((int)(default(UnmanagedMemoryAccessor)!.Capacity) != (int)(default(UnmanagedMemoryAccessor)!.Capacity)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [167] System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::Read{T}:System.Void(System.Int64,T&)
    public static void CustomEntrySubject_167()
    {
    }

    // [168] System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::ReadArray{T}:System.Int32(System.Int64,T{},System.Int32,System.Int32)
    public static void CustomEntrySubject_168()
    {
    }

    // [169] System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::ReadBoolean:System.Boolean(System.Int64)
    public static void Subject_169()
    {
        try { if (((default(UnmanagedMemoryAccessor)!.ReadBoolean(42L)) ? 1 : 0) != ((default(UnmanagedMemoryAccessor)!.ReadBoolean(42L)) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [170] System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::ReadByte:System.Byte(System.Int64)
    public static void Subject_170()
    {
        try { if ((int)(default(UnmanagedMemoryAccessor)!.ReadByte(42L)) != (int)(default(UnmanagedMemoryAccessor)!.ReadByte(42L))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [171] System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::ReadChar:System.Char(System.Int64)
    public static void Subject_171()
    {
        try { if ((int)(default(UnmanagedMemoryAccessor)!.ReadChar(42L)) != (int)(default(UnmanagedMemoryAccessor)!.ReadChar(42L))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [172] System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::ReadDecimal:System.Decimal(System.Int64)
    public static void Subject_172()
    {
        try { if ((int)(default(UnmanagedMemoryAccessor)!.ReadDecimal(42L)) != (int)(default(UnmanagedMemoryAccessor)!.ReadDecimal(42L))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [173] System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::ReadDouble:System.Double(System.Int64)
    public static void Subject_173()
    {
        try { if ((int)(default(UnmanagedMemoryAccessor)!.ReadDouble(42L)) != (int)(default(UnmanagedMemoryAccessor)!.ReadDouble(42L))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [174] System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::ReadInt16:System.Int16(System.Int64)
    public static void Subject_174()
    {
        try { if ((int)(default(UnmanagedMemoryAccessor)!.ReadInt16(42L)) != (int)(default(UnmanagedMemoryAccessor)!.ReadInt16(42L))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [175] System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::ReadInt32:System.Int32(System.Int64)
    public static void Subject_175()
    {
        try { if (default(UnmanagedMemoryAccessor)!.ReadInt32(42L) != default(UnmanagedMemoryAccessor)!.ReadInt32(42L)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [176] System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::ReadInt64:System.Int64(System.Int64)
    public static void Subject_176()
    {
        try { if ((int)(default(UnmanagedMemoryAccessor)!.ReadInt64(42L)) != (int)(default(UnmanagedMemoryAccessor)!.ReadInt64(42L))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [177] System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::ReadSByte:System.SByte(System.Int64)
    public static void Subject_177()
    {
        try { if ((int)(default(UnmanagedMemoryAccessor)!.ReadSByte(42L)) != (int)(default(UnmanagedMemoryAccessor)!.ReadSByte(42L))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [178] System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::ReadSingle:System.Single(System.Int64)
    public static void Subject_178()
    {
        try { if ((int)(default(UnmanagedMemoryAccessor)!.ReadSingle(42L)) != (int)(default(UnmanagedMemoryAccessor)!.ReadSingle(42L))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [179] System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::ReadUInt16:System.UInt16(System.Int64)
    public static void Subject_179()
    {
        try { if ((int)(default(UnmanagedMemoryAccessor)!.ReadUInt16(42L)) != (int)(default(UnmanagedMemoryAccessor)!.ReadUInt16(42L))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [180] System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::ReadUInt32:System.UInt32(System.Int64)
    public static void Subject_180()
    {
        try { if ((int)(default(UnmanagedMemoryAccessor)!.ReadUInt32(42L)) != (int)(default(UnmanagedMemoryAccessor)!.ReadUInt32(42L))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [181] System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::ReadUInt64:System.UInt64(System.Int64)
    public static void Subject_181()
    {
        try { if ((int)(default(UnmanagedMemoryAccessor)!.ReadUInt64(42L)) != (int)(default(UnmanagedMemoryAccessor)!.ReadUInt64(42L))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [182] System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::Write:System.Void(System.Int64,System.Boolean)
    public static void Subject_182()
    {
        try { default(UnmanagedMemoryAccessor)!.Write(42L, true); }
        catch { _exitCode = 1; }
    }

    // [183] System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::Write:System.Void(System.Int64,System.Byte)
    public static void Subject_183()
    {
        try { default(UnmanagedMemoryAccessor)!.Write(42L, (byte)42); }
        catch { _exitCode = 1; }
    }

    // [184] System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::Write:System.Void(System.Int64,System.Char)
    public static void Subject_184()
    {
        try { default(UnmanagedMemoryAccessor)!.Write(42L, 'A'); }
        catch { _exitCode = 1; }
    }

    // [185] System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::Write:System.Void(System.Int64,System.Int16)
    public static void Subject_185()
    {
        try { default(UnmanagedMemoryAccessor)!.Write(42L, (short)42); }
        catch { _exitCode = 1; }
    }

    // [186] System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::Write:System.Void(System.Int64,System.Int32)
    public static void Subject_186()
    {
        try { default(UnmanagedMemoryAccessor)!.Write(42L, 42); }
        catch { _exitCode = 1; }
    }

    // [187] System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::Write:System.Void(System.Int64,System.Int64)
    public static void Subject_187()
    {
        try { default(UnmanagedMemoryAccessor)!.Write(42L, 42L); }
        catch { _exitCode = 1; }
    }

    // [188] System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::Write:System.Void(System.Int64,System.Decimal)
    public static void Subject_188()
    {
        try { default(UnmanagedMemoryAccessor)!.Write(42L, 42m); }
        catch { _exitCode = 1; }
    }

    // [189] System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::Write:System.Void(System.Int64,System.Single)
    public static void Subject_189()
    {
        try { default(UnmanagedMemoryAccessor)!.Write(42L, 42.0f); }
        catch { _exitCode = 1; }
    }

    // [190] System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::Write:System.Void(System.Int64,System.Double)
    public static void Subject_190()
    {
        try { default(UnmanagedMemoryAccessor)!.Write(42L, 42.0); }
        catch { _exitCode = 1; }
    }

    // [191] System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::Write:System.Void(System.Int64,System.SByte)
    public static void Subject_191()
    {
        try { default(UnmanagedMemoryAccessor)!.Write(42L, (sbyte)42); }
        catch { _exitCode = 1; }
    }

    // [192] System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::Write:System.Void(System.Int64,System.UInt16)
    public static void Subject_192()
    {
        try { default(UnmanagedMemoryAccessor)!.Write(42L, (ushort)42); }
        catch { _exitCode = 1; }
    }

    // [193] System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::Write:System.Void(System.Int64,System.UInt32)
    public static void Subject_193()
    {
        try { default(UnmanagedMemoryAccessor)!.Write(42L, 42u); }
        catch { _exitCode = 1; }
    }

    // [194] System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::Write:System.Void(System.Int64,System.UInt64)
    public static void Subject_194()
    {
        try { default(UnmanagedMemoryAccessor)!.Write(42L, 42uL); }
        catch { _exitCode = 1; }
    }

    // [195] System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::Write{T}:System.Void(System.Int64,T&)
    public static void CustomEntrySubject_195()
    {
    }

    // [196] System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::WriteArray{T}:System.Void(System.Int64,T{},System.Int32,System.Int32)
    public static void CustomEntrySubject_196()
    {
    }

    // [197] System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::.ctor:System.Void(System.Runtime.InteropServices.SafeBuffer,System.Int64,System.Int64)
    public static void Subject_197()
    {
        try { new UnmanagedMemoryAccessor(default, 42L, 42L); }
        catch { _exitCode = 1; }
    }

    // [198] System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::.ctor:System.Void(System.Runtime.InteropServices.SafeBuffer,System.Int64,System.Int64,System.IO.FileAccess)
    public static void Subject_198()
    {
        try { new UnmanagedMemoryAccessor(default, 42L, 42L, default); }
        catch { _exitCode = 1; }
    }

}