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
        try { Marshal.AddRef(IntPtr.Zero); _exitCode = 1; }
        catch (ArgumentNullException) { }
    }

    // [1] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::AllocCoTaskMem:System.IntPtr(System.Int32)
    public static void Subject_1()
    {
        if ((int)(Marshal.AllocCoTaskMem(42)) != -1642479856) _exitCode = 1;
    }

    // [2] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::AllocHGlobal:System.IntPtr(System.Int32)
    public static void Subject_2()
    {
        if ((int)(Marshal.AllocHGlobal(42)) != -2134461472) _exitCode = 1;
    }

    // [3] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::AllocHGlobal:System.IntPtr(System.IntPtr)
    public static void Subject_3()
    {
        if ((int)(Marshal.AllocHGlobal(IntPtr.Zero)) != -1641522016) _exitCode = 1;
    }

    // [4] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::AreComObjectsAvailableForCleanup:System.Boolean()
    public static void Subject_4()
    {
        if (((Marshal.AreComObjectsAvailableForCleanup()) ? 1 : 0) != 0) _exitCode = 1;
    }

    // [5] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::BindToMoniker:System.Object(System.String)
    public static void Subject_5()
    {
        try { Marshal.BindToMoniker("hello"); _exitCode = 1; }
        catch (COMException) { }
    }

    // [6] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::ChangeWrapperHandleStrength:System.Void(System.Object,System.Boolean)
    public static void Subject_6()
    {
        Marshal.ChangeWrapperHandleStrength(42, true);
    }

    // [7] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::CleanupUnusedObjectsInCurrentContext:System.Void()
    public static void Subject_7()
    {
        Marshal.CleanupUnusedObjectsInCurrentContext();
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
        try { Marshal.CreateAggregatedObject(IntPtr.Zero, 42); _exitCode = 1; }
        catch (ArgumentNullException) { }
    }

    // [25] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::CreateAggregatedObject{T}:System.IntPtr(System.IntPtr,T)

    // [26] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::CreateWrapperOfType:System.Object(System.Object,System.Type)
    public static void Subject_26()
    {
        try { Marshal.CreateWrapperOfType(42, typeof(byte)); _exitCode = 1; }
        catch (ArgumentException) { }
    }

    // [27] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::CreateWrapperOfType{T,TWrapper}:TWrapper(T)

    // [28] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::DestroyStructure:System.Void(System.IntPtr,System.Type)
    public static void Subject_28()
    {
        try { Marshal.DestroyStructure(IntPtr.Zero, typeof(byte)); _exitCode = 1; }
        catch (ArgumentNullException) { }
    }

    // [29] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::DestroyStructure{T}:System.Void(System.IntPtr)

    // [30] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::FinalReleaseComObject:System.Int32(System.Object)
    public static void Subject_30()
    {
        try { Marshal.FinalReleaseComObject(42); _exitCode = 1; }
        catch (ArgumentException) { }
    }

    // [31] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::FreeBSTR:System.Void(System.IntPtr)
    public static void Subject_31()
    {
        Marshal.FreeBSTR(IntPtr.Zero);
    }

    // [32] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::FreeCoTaskMem:System.Void(System.IntPtr)
    public static void Subject_32()
    {
        Marshal.FreeCoTaskMem(IntPtr.Zero);
    }

    // [33] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::FreeHGlobal:System.Void(System.IntPtr)
    public static void Subject_33()
    {
        Marshal.FreeHGlobal(IntPtr.Zero);
    }

    // [34] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::GenerateGuidForType:System.Guid(System.Type)
    public static void Subject_34()
    {
        if (((Marshal.GenerateGuidForType(typeof(byte))).GetHashCode()) != -863221168) _exitCode = 1;
    }

    // [35] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::GenerateProgIdForType:System.String(System.Type)
    public static void Subject_35()
    {
        if (((Marshal.GenerateProgIdForType(typeof(byte))).Length) != 11) _exitCode = 1;
    }

    // [36] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::GetComInterfaceForObject:System.IntPtr(System.Object,System.Type)
    public static void Subject_36()
    {
        try { Marshal.GetComInterfaceForObject(42, typeof(byte)); _exitCode = 1; }
        catch (ArgumentException) { }
    }

    // [37] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::GetComInterfaceForObject{T,TInterface}:System.IntPtr(T)

    // [38] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::GetComInterfaceForObject:System.IntPtr(System.Object,System.Type,System.Runtime.InteropServices.CustomQueryInterfaceMode)
    public static void Subject_38()
    {
        try { Marshal.GetComInterfaceForObject(42, typeof(byte), default); _exitCode = 1; }
        catch (ArgumentException) { }
    }

    // [39] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::GetComObjectData:System.Object(System.Object,System.Object)
    public static void Subject_39()
    {
        try { Marshal.GetComObjectData(42, 42); _exitCode = 1; }
        catch (ArgumentException) { }
    }

    // [40] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::GetDelegateForFunctionPointer:System.Delegate(System.IntPtr,System.Type)
    public static void Subject_40()
    {
        try { Marshal.GetDelegateForFunctionPointer(IntPtr.Zero, typeof(byte)); _exitCode = 1; }
        catch (ArgumentNullException) { }
    }

    // [41] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::GetDelegateForFunctionPointer{TDelegate}:TDelegate(System.IntPtr)

    // [42] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::GetEndComSlot:System.Int32(System.Type)
    public static void Subject_42()
    {
        try { Marshal.GetEndComSlot(typeof(byte)); _exitCode = 1; }
        catch (ArgumentException) { }
    }

    // [43] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::GetExceptionCode:System.Int32()
    public static void Subject_43()
    {
        if (Marshal.GetExceptionCode() != 0) _exitCode = 1;
    }

    // [44] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::GetExceptionForHR:System.Exception(System.Int32)
    public static void Subject_44()
    {
        try { Marshal.GetExceptionForHR(42); _exitCode = 1; }
        catch (NullReferenceException) { }
    }

    // [45] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::GetExceptionForHR:System.Exception(System.Int32,System.IntPtr)
    public static void Subject_45()
    {
        try { Marshal.GetExceptionForHR(42, IntPtr.Zero); _exitCode = 1; }
        catch (NullReferenceException) { }
    }

    // [46] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::GetExceptionForHR:System.Exception(System.Int32,System.Guid&,System.IntPtr)
    public static void Subject_46()
    {
        try { Marshal.GetExceptionForHR(42, default(Guid), IntPtr.Zero); _exitCode = 1; }
        catch (NullReferenceException) { }
    }

    // [47] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::GetExceptionPointers:System.IntPtr()
    public static void Subject_47()
    {
        if ((int)(Marshal.GetExceptionPointers()) != 0) _exitCode = 1;
    }

    // [48] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::GetFunctionPointerForDelegate:System.IntPtr(System.Delegate)
    public static void Subject_48()
    {
        try { Marshal.GetFunctionPointerForDelegate(default); _exitCode = 1; }
        catch (ArgumentNullException) { }
    }

    // [49] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::GetFunctionPointerForDelegate{TDelegate}:System.IntPtr(TDelegate)

    // [50] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::GetHINSTANCE:System.IntPtr(System.Reflection.Module)
    public static void Subject_50()
    {
        try { Marshal.GetHINSTANCE(null!); _exitCode = 1; }
        catch (ArgumentNullException) { }
    }

    // [51] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::GetHRForException:System.Int32(System.Exception)
    public static void Subject_51()
    {
        if (Marshal.GetHRForException(default) != 0) _exitCode = 1;
    }

    // [52] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::GetHRForLastWin32Error:System.Int32()
    public static void Subject_52()
    {
        if (Marshal.GetHRForLastWin32Error() != -2147024693) _exitCode = 1;
    }

    // [53] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::GetIDispatchForObject:System.IntPtr(System.Object)
    public static void Subject_53()
    {
        try { Marshal.GetIDispatchForObject(42); _exitCode = 1; }
        catch (InvalidCastException) { }
    }

    // [54] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::GetIUnknownForObject:System.IntPtr(System.Object)
    public static void Subject_54()
    {
        if ((int)(Marshal.GetIUnknownForObject(42)) != -2095449832) _exitCode = 1;
    }

    // [55] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::GetLastPInvokeError:System.Int32()
    public static void Subject_55()
    {
        if (Marshal.GetLastPInvokeError() != 203) _exitCode = 1;
    }

    // [56] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::GetLastPInvokeErrorMessage:System.String()
    public static void Subject_56()
    {
        if (((Marshal.GetLastPInvokeErrorMessage()).Length) != 66) _exitCode = 1;
    }

    // [57] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::GetLastSystemError:System.Int32()
    public static void Subject_57()
    {
        if (Marshal.GetLastSystemError() != 0) _exitCode = 1;
    }

    // [58] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::GetLastWin32Error:System.Int32()
    public static void Subject_58()
    {
        if (Marshal.GetLastWin32Error() != 0) _exitCode = 1;
    }

    // [59] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::GetNativeVariantForObject:System.Void(System.Object,System.IntPtr)
    public static void Subject_59()
    {
        try { Marshal.GetNativeVariantForObject(42, IntPtr.Zero); _exitCode = 1; }
        catch (ArgumentNullException) { }
    }

    // [60] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::GetNativeVariantForObject{T}:System.Void(T,System.IntPtr)

    // [61] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::GetObjectForIUnknown:System.Object(System.IntPtr)
    public static void Subject_61()
    {
        try { Marshal.GetObjectForIUnknown(IntPtr.Zero); _exitCode = 1; }
        catch (ArgumentNullException) { }
    }

    // [62] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::GetObjectForNativeVariant:System.Object(System.IntPtr)
    public static void Subject_62()
    {
        try { Marshal.GetObjectForNativeVariant(IntPtr.Zero); _exitCode = 1; }
        catch (ArgumentNullException) { }
    }

    // [63] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::GetObjectForNativeVariant{T}:T(System.IntPtr)

    // [64] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::GetObjectsForNativeVariants:System.Object{}(System.IntPtr,System.Int32)
    public static void Subject_64()
    {
        try { Marshal.GetObjectsForNativeVariants(IntPtr.Zero, 42); _exitCode = 1; }
        catch (ArgumentNullException) { }
    }

    // [65] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::GetObjectsForNativeVariants{T}:T{}(System.IntPtr,System.Int32)

    // [66] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::GetPInvokeErrorMessage:System.String(System.Int32)
    public static void Subject_66()
    {
        if (((Marshal.GetPInvokeErrorMessage(42)).Length) != 20) _exitCode = 1;
    }

    // [67] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::GetStartComSlot:System.Int32(System.Type)
    public static void Subject_67()
    {
        try { Marshal.GetStartComSlot(typeof(byte)); _exitCode = 1; }
        catch (ArgumentException) { }
    }

    // [68] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::GetTypedObjectForIUnknown:System.Object(System.IntPtr,System.Type)
    public static void Subject_68()
    {
        try { Marshal.GetTypedObjectForIUnknown(IntPtr.Zero, typeof(byte)); _exitCode = 1; }
        catch (ArgumentNullException) { }
    }

    // [69] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::GetTypeFromCLSID:System.Type(System.Guid)
    public static void Subject_69()
    {
        if (((Marshal.GetTypeFromCLSID(Guid.Empty)).GetHashCode()) != 20054852) _exitCode = 1;
    }

    // [70] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::GetTypeInfoName:System.String(System.Runtime.InteropServices.ComTypes.ITypeInfo)
    public static void Subject_70()
    {
        try { Marshal.GetTypeInfoName(default); _exitCode = 1; }
        catch (ArgumentNullException) { }
    }

    // [71] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::GetUniqueObjectForIUnknown:System.Object(System.IntPtr)
    public static void Subject_71()
    {
        try { Marshal.GetUniqueObjectForIUnknown(IntPtr.Zero); _exitCode = 1; }
        catch (ArgumentNullException) { }
    }

    // [72] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::InitHandle:System.Void(System.Runtime.InteropServices.SafeHandle,System.IntPtr)
    public static void Subject_72()
    {
        try { Marshal.InitHandle(default, IntPtr.Zero); _exitCode = 1; }
        catch (NullReferenceException) { }
    }

    // [73] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::IsComObject:System.Boolean(System.Object)
    public static void Subject_73()
    {
        if (((Marshal.IsComObject(42)) ? 1 : 0) != 0) _exitCode = 1;
    }

    // [74] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::IsTypeVisibleFromCom:System.Boolean(System.Type)
    public static void Subject_74()
    {
        if (((Marshal.IsTypeVisibleFromCom(typeof(byte))) ? 1 : 0) != 0) _exitCode = 1;
    }

    // [75] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::OffsetOf:System.IntPtr(System.Type,System.String)
    public static void Subject_75()
    {
        try { Marshal.OffsetOf(typeof(byte), "hello"); _exitCode = 1; }
        catch (ArgumentException) { }
    }

    // [76] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::OffsetOf{T}:System.IntPtr(System.String)

    // [77] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::Prelink:System.Void(System.Reflection.MethodInfo)
    public static void Subject_77()
    {
        try { Marshal.Prelink(null!); _exitCode = 1; }
        catch (ArgumentNullException) { }
    }

    // [78] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::PrelinkAll:System.Void(System.Type)
    public static void Subject_78()
    {
        Marshal.PrelinkAll(typeof(byte));
    }

    // [79] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::PtrToStringAnsi:System.String(System.IntPtr)
    public static void Subject_79()
    {
        try { Marshal.PtrToStringAnsi(IntPtr.Zero); _exitCode = 1; }
        catch (NullReferenceException) { }
    }

    // [80] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::PtrToStringAnsi:System.String(System.IntPtr,System.Int32)
    public static void Subject_80()
    {
        try { Marshal.PtrToStringAnsi(IntPtr.Zero, 42); _exitCode = 1; }
        catch (ArgumentNullException) { }
    }

    // [81] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::PtrToStringAuto:System.String(System.IntPtr,System.Int32)
    public static void Subject_81()
    {
        try { Marshal.PtrToStringAuto(IntPtr.Zero, 42); _exitCode = 1; }
        catch (ArgumentNullException) { }
    }

    // [82] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::PtrToStringAuto:System.String(System.IntPtr)
    public static void Subject_82()
    {
        try { Marshal.PtrToStringAuto(IntPtr.Zero); _exitCode = 1; }
        catch (NullReferenceException) { }
    }

    // [83] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::PtrToStringBSTR:System.String(System.IntPtr)
    public static void Subject_83()
    {
        try { Marshal.PtrToStringBSTR(IntPtr.Zero); _exitCode = 1; }
        catch (ArgumentNullException) { }
    }

    // [84] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::PtrToStringUni:System.String(System.IntPtr)
    public static void Subject_84()
    {
        try { Marshal.PtrToStringUni(IntPtr.Zero); _exitCode = 1; }
        catch (NullReferenceException) { }
    }

    // [85] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::PtrToStringUni:System.String(System.IntPtr,System.Int32)
    public static void Subject_85()
    {
        try { Marshal.PtrToStringUni(IntPtr.Zero, 42); _exitCode = 1; }
        catch (ArgumentNullException) { }
    }

    // [86] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::PtrToStringUTF8:System.String(System.IntPtr)
    public static void Subject_86()
    {
        try { Marshal.PtrToStringUTF8(IntPtr.Zero); _exitCode = 1; }
        catch (NullReferenceException) { }
    }

    // [87] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::PtrToStringUTF8:System.String(System.IntPtr,System.Int32)
    public static void Subject_87()
    {
        try { Marshal.PtrToStringUTF8(IntPtr.Zero, 42); _exitCode = 1; }
        catch (ArgumentNullException) { }
    }

    // [88] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::PtrToStructure:System.Object(System.IntPtr,System.Type)
    public static void Subject_88()
    {
        try { Marshal.PtrToStructure(IntPtr.Zero, typeof(byte)); _exitCode = 1; }
        catch (NullReferenceException) { }
    }

    // [89] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::PtrToStructure:System.Void(System.IntPtr,System.Object)
    public static void Subject_89()
    {
        try { Marshal.PtrToStructure(IntPtr.Zero, 42); _exitCode = 1; }
        catch (ArgumentNullException) { }
    }

    // [90] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::PtrToStructure{T}:System.Void(System.IntPtr,T)

    // [91] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::PtrToStructure{T}:T(System.IntPtr)

    // [92] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::QueryInterface:System.Int32(System.IntPtr,System.Guid&,System.IntPtr&)
    public static void Subject_92()
    {
        // TODO: System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::QueryInterface:System.Int32(System.IntPtr,System.Guid&,System.IntPtr&) could not be auto-generated
    }

    // [93] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::ReadByte:System.Byte(System.Object,System.Int32)
    public static void Subject_93()
    {
        try { Marshal.ReadByte(42, 42); _exitCode = 1; }
        catch (AccessViolationException) { }
    }

    // [94] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::ReadByte:System.Byte(System.IntPtr,System.Int32)
    public static void Subject_94()
    {
        try { Marshal.ReadByte(IntPtr.Zero, 42); _exitCode = 1; }
        catch (AccessViolationException) { }
    }

    // [95] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::ReadByte:System.Byte(System.IntPtr)
    public static void Subject_95()
    {
        try { Marshal.ReadByte(IntPtr.Zero); _exitCode = 1; }
        catch (AccessViolationException) { }
    }

    // [96] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::ReadInt16:System.Int16(System.Object,System.Int32)
    public static void Subject_96()
    {
        try { Marshal.ReadInt16(42, 42); _exitCode = 1; }
        catch (AccessViolationException) { }
    }

    // [97] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::ReadInt16:System.Int16(System.IntPtr,System.Int32)
    public static void Subject_97()
    {
        try { Marshal.ReadInt16(IntPtr.Zero, 42); _exitCode = 1; }
        catch (AccessViolationException) { }
    }

    // [98] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::ReadInt16:System.Int16(System.IntPtr)
    public static void Subject_98()
    {
        try { Marshal.ReadInt16(IntPtr.Zero); _exitCode = 1; }
        catch (AccessViolationException) { }
    }

    // [99] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::ReadInt32:System.Int32(System.Object,System.Int32)
    public static void Subject_99()
    {
        try { Marshal.ReadInt32(42, 42); _exitCode = 1; }
        catch (AccessViolationException) { }
    }

    // [100] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::ReadInt32:System.Int32(System.IntPtr,System.Int32)
    public static void Subject_100()
    {
        try { Marshal.ReadInt32(IntPtr.Zero, 42); _exitCode = 1; }
        catch (AccessViolationException) { }
    }

    // [101] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::ReadInt32:System.Int32(System.IntPtr)
    public static void Subject_101()
    {
        try { Marshal.ReadInt32(IntPtr.Zero); _exitCode = 1; }
        catch (AccessViolationException) { }
    }

    // [102] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::ReadInt64:System.Int64(System.Object,System.Int32)
    public static void Subject_102()
    {
        try { Marshal.ReadInt64(42, 42); _exitCode = 1; }
        catch (AccessViolationException) { }
    }

    // [103] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::ReadInt64:System.Int64(System.IntPtr,System.Int32)
    public static void Subject_103()
    {
        try { Marshal.ReadInt64(IntPtr.Zero, 42); _exitCode = 1; }
        catch (AccessViolationException) { }
    }

    // [104] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::ReadInt64:System.Int64(System.IntPtr)
    public static void Subject_104()
    {
        try { Marshal.ReadInt64(IntPtr.Zero); _exitCode = 1; }
        catch (AccessViolationException) { }
    }

    // [105] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::ReadIntPtr:System.IntPtr(System.Object,System.Int32)
    public static void Subject_105()
    {
        try { Marshal.ReadIntPtr(42, 42); _exitCode = 1; }
        catch (AccessViolationException) { }
    }

    // [106] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::ReadIntPtr:System.IntPtr(System.IntPtr,System.Int32)
    public static void Subject_106()
    {
        try { Marshal.ReadIntPtr(IntPtr.Zero, 42); _exitCode = 1; }
        catch (AccessViolationException) { }
    }

    // [107] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::ReadIntPtr:System.IntPtr(System.IntPtr)
    public static void Subject_107()
    {
        try { Marshal.ReadIntPtr(IntPtr.Zero); _exitCode = 1; }
        catch (AccessViolationException) { }
    }

    // [108] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::ReAllocCoTaskMem:System.IntPtr(System.IntPtr,System.Int32)
    public static void Subject_108()
    {
        if ((int)(Marshal.ReAllocCoTaskMem(IntPtr.Zero, 42)) != -2134462240) _exitCode = 1;
    }

    // [109] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::ReAllocHGlobal:System.IntPtr(System.IntPtr,System.IntPtr)
    public static void Subject_109()
    {
        if ((int)(Marshal.ReAllocHGlobal(IntPtr.Zero, IntPtr.Zero)) != -1641521456) _exitCode = 1;
    }

    // [110] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::Release:System.Int32(System.IntPtr)
    public static void Subject_110()
    {
        try { Marshal.Release(IntPtr.Zero); _exitCode = 1; }
        catch (ArgumentNullException) { }
    }

    // [111] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::ReleaseComObject:System.Int32(System.Object)
    public static void Subject_111()
    {
        try { Marshal.ReleaseComObject(42); _exitCode = 1; }
        catch (ArgumentException) { }
    }

    // [112] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::SecureStringToBSTR:System.IntPtr(System.Security.SecureString)
    public static void Subject_112()
    {
        try { Marshal.SecureStringToBSTR(null!); _exitCode = 1; }
        catch (ArgumentNullException) { }
    }

    // [113] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::SecureStringToCoTaskMemAnsi:System.IntPtr(System.Security.SecureString)
    public static void Subject_113()
    {
        try { Marshal.SecureStringToCoTaskMemAnsi(null!); _exitCode = 1; }
        catch (ArgumentNullException) { }
    }

    // [114] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::SecureStringToCoTaskMemUnicode:System.IntPtr(System.Security.SecureString)
    public static void Subject_114()
    {
        try { Marshal.SecureStringToCoTaskMemUnicode(null!); _exitCode = 1; }
        catch (ArgumentNullException) { }
    }

    // [115] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::SecureStringToGlobalAllocAnsi:System.IntPtr(System.Security.SecureString)
    public static void Subject_115()
    {
        try { Marshal.SecureStringToGlobalAllocAnsi(null!); _exitCode = 1; }
        catch (ArgumentNullException) { }
    }

    // [116] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::SecureStringToGlobalAllocUnicode:System.IntPtr(System.Security.SecureString)
    public static void Subject_116()
    {
        try { Marshal.SecureStringToGlobalAllocUnicode(null!); _exitCode = 1; }
        catch (ArgumentNullException) { }
    }

    // [117] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::SetComObjectData:System.Boolean(System.Object,System.Object,System.Object)
    public static void Subject_117()
    {
        try { Marshal.SetComObjectData(42, 42, 42); _exitCode = 1; }
        catch (ArgumentException) { }
    }

    // [118] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::SetLastPInvokeError:System.Void(System.Int32)
    public static void Subject_118()
    {
        Marshal.SetLastPInvokeError(42);
    }

    // [119] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::SetLastSystemError:System.Void(System.Int32)
    public static void Subject_119()
    {
        Marshal.SetLastSystemError(42);
    }

    // [120] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::SizeOf:System.Int32(System.Object)
    public static void Subject_120()
    {
        if (Marshal.SizeOf(42) != 4) _exitCode = 1;
    }

    // [121] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::SizeOf{T}:System.Int32(T)

    // [122] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::SizeOf:System.Int32(System.Type)
    public static void Subject_122()
    {
        if (Marshal.SizeOf(typeof(byte)) != 1) _exitCode = 1;
    }

    // [123] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::SizeOf{T}:System.Int32()

    // [124] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::StringToBSTR:System.IntPtr(System.String)
    public static void Subject_124()
    {
        if ((int)(Marshal.StringToBSTR("hello")) != -2134492536) _exitCode = 1;
    }

    // [125] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::StringToCoTaskMemAnsi:System.IntPtr(System.String)
    public static void Subject_125()
    {
        if ((int)(Marshal.StringToCoTaskMemAnsi("hello")) != -2136817152) _exitCode = 1;
    }

    // [126] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::StringToCoTaskMemAuto:System.IntPtr(System.String)
    public static void Subject_126()
    {
        if ((int)(Marshal.StringToCoTaskMemAuto("hello")) != -2136817120) _exitCode = 1;
    }

    // [127] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::StringToCoTaskMemUni:System.IntPtr(System.String)
    public static void Subject_127()
    {
        if ((int)(Marshal.StringToCoTaskMemUni("hello")) != -2136817088) _exitCode = 1;
    }

    // [128] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::StringToCoTaskMemUTF8:System.IntPtr(System.String)
    public static void Subject_128()
    {
        if ((int)(Marshal.StringToCoTaskMemUTF8("hello")) != -2136814272) _exitCode = 1;
    }

    // [129] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::StringToHGlobalAnsi:System.IntPtr(System.String)
    public static void Subject_129()
    {
        if ((int)(Marshal.StringToHGlobalAnsi("hello")) != -2136815008) _exitCode = 1;
    }

    // [130] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::StringToHGlobalAuto:System.IntPtr(System.String)
    public static void Subject_130()
    {
        if ((int)(Marshal.StringToHGlobalAuto("hello")) != -2136814720) _exitCode = 1;
    }

    // [131] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::StringToHGlobalUni:System.IntPtr(System.String)
    public static void Subject_131()
    {
        if ((int)(Marshal.StringToHGlobalUni("hello")) != -2136814656) _exitCode = 1;
    }

    // [132] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::StructureToPtr:System.Void(System.Object,System.IntPtr,System.Boolean)
    public static void Subject_132()
    {
        try { Marshal.StructureToPtr(42, IntPtr.Zero, true); _exitCode = 1; }
        catch (ArgumentNullException) { }
    }

    // [133] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::StructureToPtr{T}:System.Void(T,System.IntPtr,System.Boolean)

    // [134] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::ThrowExceptionForHR:System.Void(System.Int32)
    public static void Subject_134()
    {
        Marshal.ThrowExceptionForHR(42);
    }

    // [135] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::ThrowExceptionForHR:System.Void(System.Int32,System.IntPtr)
    public static void Subject_135()
    {
        Marshal.ThrowExceptionForHR(42, IntPtr.Zero);
    }

    // [136] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::ThrowExceptionForHR:System.Void(System.Int32,System.Guid&,System.IntPtr)
    public static void Subject_136()
    {
        // TODO: System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::ThrowExceptionForHR:System.Void(System.Int32,System.Guid&,System.IntPtr) could not be auto-generated
    }

    // [137] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::UnsafeAddrOfPinnedArrayElement:System.IntPtr(System.Array,System.Int32)
    public static void Subject_137()
    {
        if ((int)(Marshal.UnsafeAddrOfPinnedArrayElement(Array.Empty<byte>(), 42)) != -2147445390) _exitCode = 1;
    }

    // [138] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::UnsafeAddrOfPinnedArrayElement{T}:System.IntPtr(T{},System.Int32)

    // [139] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::WriteByte:System.Void(System.Object,System.Int32,System.Byte)
    public static void Subject_139()
    {
        try { Marshal.WriteByte(42, 42, (byte)42); _exitCode = 1; }
        catch (AccessViolationException) { }
    }

    // [140] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::WriteByte:System.Void(System.IntPtr,System.Int32,System.Byte)
    public static void Subject_140()
    {
        try { Marshal.WriteByte(IntPtr.Zero, 42, (byte)42); _exitCode = 1; }
        catch (AccessViolationException) { }
    }

    // [141] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::WriteByte:System.Void(System.IntPtr,System.Byte)
    public static void Subject_141()
    {
        try { Marshal.WriteByte(IntPtr.Zero, (byte)42); _exitCode = 1; }
        catch (AccessViolationException) { }
    }

    // [142] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::WriteInt16:System.Void(System.Object,System.Int32,System.Int16)
    public static void Subject_142()
    {
        try { Marshal.WriteInt16(42, 42, (short)42); _exitCode = 1; }
        catch (AccessViolationException) { }
    }

    // [143] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::WriteInt16:System.Void(System.IntPtr,System.Int32,System.Int16)
    public static void Subject_143()
    {
        try { Marshal.WriteInt16(IntPtr.Zero, 42, (short)42); _exitCode = 1; }
        catch (AccessViolationException) { }
    }

    // [144] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::WriteInt16:System.Void(System.IntPtr,System.Int16)
    public static void Subject_144()
    {
        try { Marshal.WriteInt16(IntPtr.Zero, (short)42); _exitCode = 1; }
        catch (AccessViolationException) { }
    }

    // [145] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::WriteInt16:System.Void(System.IntPtr,System.Int32,System.Char)
    public static void Subject_145()
    {
        try { Marshal.WriteInt16(IntPtr.Zero, 42, 'A'); _exitCode = 1; }
        catch (AccessViolationException) { }
    }

    // [146] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::WriteInt16:System.Void(System.Object,System.Int32,System.Char)
    public static void Subject_146()
    {
        try { Marshal.WriteInt16(42, 42, 'A'); _exitCode = 1; }
        catch (AccessViolationException) { }
    }

    // [147] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::WriteInt16:System.Void(System.IntPtr,System.Char)
    public static void Subject_147()
    {
        try { Marshal.WriteInt16(IntPtr.Zero, 'A'); _exitCode = 1; }
        catch (AccessViolationException) { }
    }

    // [148] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::WriteInt32:System.Void(System.Object,System.Int32,System.Int32)
    public static void Subject_148()
    {
        try { Marshal.WriteInt32(42, 42, 42); _exitCode = 1; }
        catch (AccessViolationException) { }
    }

    // [149] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::WriteInt32:System.Void(System.IntPtr,System.Int32,System.Int32)
    public static void Subject_149()
    {
        try { Marshal.WriteInt32(IntPtr.Zero, 42, 42); _exitCode = 1; }
        catch (AccessViolationException) { }
    }

    // [150] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::WriteInt32:System.Void(System.IntPtr,System.Int32)
    public static void Subject_150()
    {
        try { Marshal.WriteInt32(IntPtr.Zero, 42); _exitCode = 1; }
        catch (AccessViolationException) { }
    }

    // [151] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::WriteInt64:System.Void(System.Object,System.Int32,System.Int64)
    public static void Subject_151()
    {
        try { Marshal.WriteInt64(42, 42, 42L); _exitCode = 1; }
        catch (AccessViolationException) { }
    }

    // [152] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::WriteInt64:System.Void(System.IntPtr,System.Int32,System.Int64)
    public static void Subject_152()
    {
        try { Marshal.WriteInt64(IntPtr.Zero, 42, 42L); _exitCode = 1; }
        catch (AccessViolationException) { }
    }

    // [153] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::WriteInt64:System.Void(System.IntPtr,System.Int64)
    public static void Subject_153()
    {
        try { Marshal.WriteInt64(IntPtr.Zero, 42L); _exitCode = 1; }
        catch (AccessViolationException) { }
    }

    // [154] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::WriteIntPtr:System.Void(System.IntPtr,System.Int32,System.IntPtr)
    public static void Subject_154()
    {
        try { Marshal.WriteIntPtr(IntPtr.Zero, 42, IntPtr.Zero); _exitCode = 1; }
        catch (AccessViolationException) { }
    }

    // [155] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::WriteIntPtr:System.Void(System.Object,System.Int32,System.IntPtr)
    public static void Subject_155()
    {
        try { Marshal.WriteIntPtr(42, 42, IntPtr.Zero); _exitCode = 1; }
        catch (AccessViolationException) { }
    }

    // [156] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::WriteIntPtr:System.Void(System.IntPtr,System.IntPtr)
    public static void Subject_156()
    {
        try { Marshal.WriteIntPtr(IntPtr.Zero, IntPtr.Zero); _exitCode = 1; }
        catch (AccessViolationException) { }
    }

    // [157] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::ZeroFreeBSTR:System.Void(System.IntPtr)
    public static void Subject_157()
    {
        Marshal.ZeroFreeBSTR(IntPtr.Zero);
    }

    // [158] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::ZeroFreeCoTaskMemAnsi:System.Void(System.IntPtr)
    public static void Subject_158()
    {
        Marshal.ZeroFreeCoTaskMemAnsi(IntPtr.Zero);
    }

    // [159] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::ZeroFreeCoTaskMemUnicode:System.Void(System.IntPtr)
    public static void Subject_159()
    {
        Marshal.ZeroFreeCoTaskMemUnicode(IntPtr.Zero);
    }

    // [160] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::ZeroFreeCoTaskMemUTF8:System.Void(System.IntPtr)
    public static void Subject_160()
    {
        Marshal.ZeroFreeCoTaskMemUTF8(IntPtr.Zero);
    }

    // [161] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::ZeroFreeGlobalAllocAnsi:System.Void(System.IntPtr)
    public static void Subject_161()
    {
        Marshal.ZeroFreeGlobalAllocAnsi(IntPtr.Zero);
    }

    // [162] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::ZeroFreeGlobalAllocUnicode:System.Void(System.IntPtr)
    public static void Subject_162()
    {
        Marshal.ZeroFreeGlobalAllocUnicode(IntPtr.Zero);
    }

    // [163] System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::Dispose:System.Void()
    public static void Subject_163()
    {
        try { default(UnmanagedMemoryAccessor)!.Dispose(); _exitCode = 1; }
        catch (NullReferenceException) { }
    }

    // [164] System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::get_CanRead:System.Boolean()
    public static void Subject_164()
    {
        try { _ = default(UnmanagedMemoryAccessor)!.CanRead; _exitCode = 1; }
        catch (NullReferenceException) { }
    }

    // [165] System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::get_CanWrite:System.Boolean()
    public static void Subject_165()
    {
        try { _ = default(UnmanagedMemoryAccessor)!.CanWrite; _exitCode = 1; }
        catch (NullReferenceException) { }
    }

    // [166] System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::get_Capacity:System.Int64()
    public static void Subject_166()
    {
        try { _ = default(UnmanagedMemoryAccessor)!.Capacity; _exitCode = 1; }
        catch (NullReferenceException) { }
    }

    // [167] System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::Read{T}:System.Void(System.Int64,T&)

    // [168] System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::ReadArray{T}:System.Int32(System.Int64,T{},System.Int32,System.Int32)

    // [169] System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::ReadBoolean:System.Boolean(System.Int64)
    public static void Subject_169()
    {
        try { default(UnmanagedMemoryAccessor)!.ReadBoolean(42L); _exitCode = 1; }
        catch (NullReferenceException) { }
    }

    // [170] System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::ReadByte:System.Byte(System.Int64)
    public static void Subject_170()
    {
        try { default(UnmanagedMemoryAccessor)!.ReadByte(42L); _exitCode = 1; }
        catch (NullReferenceException) { }
    }

    // [171] System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::ReadChar:System.Char(System.Int64)
    public static void Subject_171()
    {
        try { default(UnmanagedMemoryAccessor)!.ReadChar(42L); _exitCode = 1; }
        catch (NullReferenceException) { }
    }

    // [172] System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::ReadDecimal:System.Decimal(System.Int64)
    public static void Subject_172()
    {
        try { default(UnmanagedMemoryAccessor)!.ReadDecimal(42L); _exitCode = 1; }
        catch (NullReferenceException) { }
    }

    // [173] System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::ReadDouble:System.Double(System.Int64)
    public static void Subject_173()
    {
        try { default(UnmanagedMemoryAccessor)!.ReadDouble(42L); _exitCode = 1; }
        catch (NullReferenceException) { }
    }

    // [174] System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::ReadInt16:System.Int16(System.Int64)
    public static void Subject_174()
    {
        try { default(UnmanagedMemoryAccessor)!.ReadInt16(42L); _exitCode = 1; }
        catch (NullReferenceException) { }
    }

    // [175] System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::ReadInt32:System.Int32(System.Int64)
    public static void Subject_175()
    {
        try { default(UnmanagedMemoryAccessor)!.ReadInt32(42L); _exitCode = 1; }
        catch (NullReferenceException) { }
    }

    // [176] System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::ReadInt64:System.Int64(System.Int64)
    public static void Subject_176()
    {
        try { default(UnmanagedMemoryAccessor)!.ReadInt64(42L); _exitCode = 1; }
        catch (NullReferenceException) { }
    }

    // [177] System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::ReadSByte:System.SByte(System.Int64)
    public static void Subject_177()
    {
        try { default(UnmanagedMemoryAccessor)!.ReadSByte(42L); _exitCode = 1; }
        catch (NullReferenceException) { }
    }

    // [178] System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::ReadSingle:System.Single(System.Int64)
    public static void Subject_178()
    {
        try { default(UnmanagedMemoryAccessor)!.ReadSingle(42L); _exitCode = 1; }
        catch (NullReferenceException) { }
    }

    // [179] System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::ReadUInt16:System.UInt16(System.Int64)
    public static void Subject_179()
    {
        try { default(UnmanagedMemoryAccessor)!.ReadUInt16(42L); _exitCode = 1; }
        catch (NullReferenceException) { }
    }

    // [180] System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::ReadUInt32:System.UInt32(System.Int64)
    public static void Subject_180()
    {
        try { default(UnmanagedMemoryAccessor)!.ReadUInt32(42L); _exitCode = 1; }
        catch (NullReferenceException) { }
    }

    // [181] System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::ReadUInt64:System.UInt64(System.Int64)
    public static void Subject_181()
    {
        try { default(UnmanagedMemoryAccessor)!.ReadUInt64(42L); _exitCode = 1; }
        catch (NullReferenceException) { }
    }

    // [182] System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::Write:System.Void(System.Int64,System.Boolean)
    public static void Subject_182()
    {
        try { default(UnmanagedMemoryAccessor)!.Write(42L, true); _exitCode = 1; }
        catch (NullReferenceException) { }
    }

    // [183] System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::Write:System.Void(System.Int64,System.Byte)
    public static void Subject_183()
    {
        try { default(UnmanagedMemoryAccessor)!.Write(42L, (byte)42); _exitCode = 1; }
        catch (NullReferenceException) { }
    }

    // [184] System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::Write:System.Void(System.Int64,System.Char)
    public static void Subject_184()
    {
        try { default(UnmanagedMemoryAccessor)!.Write(42L, 'A'); _exitCode = 1; }
        catch (NullReferenceException) { }
    }

    // [185] System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::Write:System.Void(System.Int64,System.Int16)
    public static void Subject_185()
    {
        try { default(UnmanagedMemoryAccessor)!.Write(42L, (short)42); _exitCode = 1; }
        catch (NullReferenceException) { }
    }

    // [186] System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::Write:System.Void(System.Int64,System.Int32)
    public static void Subject_186()
    {
        try { default(UnmanagedMemoryAccessor)!.Write(42L, 42); _exitCode = 1; }
        catch (NullReferenceException) { }
    }

    // [187] System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::Write:System.Void(System.Int64,System.Int64)
    public static void Subject_187()
    {
        try { default(UnmanagedMemoryAccessor)!.Write(42L, 42L); _exitCode = 1; }
        catch (NullReferenceException) { }
    }

    // [188] System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::Write:System.Void(System.Int64,System.Decimal)
    public static void Subject_188()
    {
        try { default(UnmanagedMemoryAccessor)!.Write(42L, 42m); _exitCode = 1; }
        catch (NullReferenceException) { }
    }

    // [189] System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::Write:System.Void(System.Int64,System.Single)
    public static void Subject_189()
    {
        try { default(UnmanagedMemoryAccessor)!.Write(42L, 42.0f); _exitCode = 1; }
        catch (NullReferenceException) { }
    }

    // [190] System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::Write:System.Void(System.Int64,System.Double)
    public static void Subject_190()
    {
        try { default(UnmanagedMemoryAccessor)!.Write(42L, 42.0); _exitCode = 1; }
        catch (NullReferenceException) { }
    }

    // [191] System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::Write:System.Void(System.Int64,System.SByte)
    public static void Subject_191()
    {
        try { default(UnmanagedMemoryAccessor)!.Write(42L, (sbyte)42); _exitCode = 1; }
        catch (NullReferenceException) { }
    }

    // [192] System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::Write:System.Void(System.Int64,System.UInt16)
    public static void Subject_192()
    {
        try { default(UnmanagedMemoryAccessor)!.Write(42L, (ushort)42); _exitCode = 1; }
        catch (NullReferenceException) { }
    }

    // [193] System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::Write:System.Void(System.Int64,System.UInt32)
    public static void Subject_193()
    {
        try { default(UnmanagedMemoryAccessor)!.Write(42L, 42u); _exitCode = 1; }
        catch (NullReferenceException) { }
    }

    // [194] System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::Write:System.Void(System.Int64,System.UInt64)
    public static void Subject_194()
    {
        try { default(UnmanagedMemoryAccessor)!.Write(42L, 42uL); _exitCode = 1; }
        catch (NullReferenceException) { }
    }

    // [195] System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::Write{T}:System.Void(System.Int64,T&)

    // [196] System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::WriteArray{T}:System.Void(System.Int64,T{},System.Int32,System.Int32)

    // [197] System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::.ctor:System.Void(System.Runtime.InteropServices.SafeBuffer,System.Int64,System.Int64)
    public static void Subject_197()
    {
        try { new UnmanagedMemoryAccessor(default, 42L, 42L); _exitCode = 1; }
        catch (ArgumentNullException) { }
    }

    // [198] System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::.ctor:System.Void(System.Runtime.InteropServices.SafeBuffer,System.Int64,System.Int64,System.IO.FileAccess)
    public static void Subject_198()
    {
        try { new UnmanagedMemoryAccessor(default, 42L, 42L, default); _exitCode = 1; }
        catch (ArgumentNullException) { }
    }

}