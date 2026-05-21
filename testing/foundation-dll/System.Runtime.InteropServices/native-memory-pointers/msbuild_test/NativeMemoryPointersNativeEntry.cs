// Auto-generated native-AOT entry point
// Family: family/System.Runtime.InteropServices/native-memory-pointers
// Assembly: System.Runtime.InteropServices
// Variant: benchmark

using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Reflection;
using System.Runtime.InteropServices;
using System.Runtime.InteropServices.ComTypes;
using System.Security;

public static partial class NativeMemoryPointersNativeEntry
{
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    // [0] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::AddRef:System.Int32(System.IntPtr)
    public static void Method0()
    {
        try { if (Marshal.AddRef(IntPtr.Zero) != Marshal.AddRef(IntPtr.Zero)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [1] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::AllocCoTaskMem:System.IntPtr(System.Int32)
    public static void Method1()
    {
        try { if ((int)(Marshal.AllocCoTaskMem(42)) != (int)(Marshal.AllocCoTaskMem(42))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [2] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::AllocHGlobal:System.IntPtr(System.Int32)
    public static void Method2()
    {
        try { if ((int)(Marshal.AllocHGlobal(42)) != (int)(Marshal.AllocHGlobal(42))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [3] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::AllocHGlobal:System.IntPtr(System.IntPtr)
    public static void Method3()
    {
        try { if ((int)(Marshal.AllocHGlobal(IntPtr.Zero)) != (int)(Marshal.AllocHGlobal(IntPtr.Zero))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [4] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::AreComObjectsAvailableForCleanup:System.Boolean()
    public static void Method4()
    {
        try { if (((Marshal.AreComObjectsAvailableForCleanup()) ? 1 : 0) != ((Marshal.AreComObjectsAvailableForCleanup()) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [5] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::BindToMoniker:System.Object(System.String)
    public static void Method5()
    {
        try { if (((Marshal.BindToMoniker("hello")).GetHashCode()) != ((Marshal.BindToMoniker("hello")).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [6] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::ChangeWrapperHandleStrength:System.Void(System.Object,System.Boolean)
    public static void Method6()
    {
        try { Marshal.ChangeWrapperHandleStrength(42, true); }
        catch { _exitCode = 1; }
    }

    // [7] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::CleanupUnusedObjectsInCurrentContext:System.Void()
    public static void Method7()
    {
        try { Marshal.CleanupUnusedObjectsInCurrentContext(); }
        catch { _exitCode = 1; }
    }

    // [8] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::Copy:System.Void(System.Int32{},System.Int32,System.IntPtr,System.Int32)
    public static void Method8()
    {
        try { Marshal.Copy(null!, 42, IntPtr.Zero, 42); }
        catch { _exitCode = 1; }
    }

    // [9] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::Copy:System.Void(System.Char{},System.Int32,System.IntPtr,System.Int32)
    public static void Method9()
    {
        try { Marshal.Copy(null!, 42, IntPtr.Zero, 42); }
        catch { _exitCode = 1; }
    }

    // [10] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::Copy:System.Void(System.Int16{},System.Int32,System.IntPtr,System.Int32)
    public static void Method10()
    {
        try { Marshal.Copy(null!, 42, IntPtr.Zero, 42); }
        catch { _exitCode = 1; }
    }

    // [11] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::Copy:System.Void(System.Int64{},System.Int32,System.IntPtr,System.Int32)
    public static void Method11()
    {
        try { Marshal.Copy(null!, 42, IntPtr.Zero, 42); }
        catch { _exitCode = 1; }
    }

    // [12] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::Copy:System.Void(System.Single{},System.Int32,System.IntPtr,System.Int32)
    public static void Method12()
    {
        try { Marshal.Copy(null!, 42, IntPtr.Zero, 42); }
        catch { _exitCode = 1; }
    }

    // [13] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::Copy:System.Void(System.Double{},System.Int32,System.IntPtr,System.Int32)
    public static void Method13()
    {
        try { Marshal.Copy(null!, 42, IntPtr.Zero, 42); }
        catch { _exitCode = 1; }
    }

    // [14] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::Copy:System.Void(System.Byte{},System.Int32,System.IntPtr,System.Int32)
    public static void Method14()
    {
        try { Marshal.Copy(null!, 42, IntPtr.Zero, 42); }
        catch { _exitCode = 1; }
    }

    // [15] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::Copy:System.Void(System.IntPtr{},System.Int32,System.IntPtr,System.Int32)
    public static void Method15()
    {
        try { Marshal.Copy(null!, 42, IntPtr.Zero, 42); }
        catch { _exitCode = 1; }
    }

    // [16] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::Copy:System.Void(System.IntPtr,System.Int32{},System.Int32,System.Int32)
    public static void Method16()
    {
        try { Marshal.Copy(IntPtr.Zero, null!, 42, 42); }
        catch { _exitCode = 1; }
    }

    // [17] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::Copy:System.Void(System.IntPtr,System.Char{},System.Int32,System.Int32)
    public static void Method17()
    {
        try { Marshal.Copy(IntPtr.Zero, null!, 42, 42); }
        catch { _exitCode = 1; }
    }

    // [18] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::Copy:System.Void(System.IntPtr,System.Int16{},System.Int32,System.Int32)
    public static void Method18()
    {
        try { Marshal.Copy(IntPtr.Zero, null!, 42, 42); }
        catch { _exitCode = 1; }
    }

    // [19] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::Copy:System.Void(System.IntPtr,System.Int64{},System.Int32,System.Int32)
    public static void Method19()
    {
        try { Marshal.Copy(IntPtr.Zero, null!, 42, 42); }
        catch { _exitCode = 1; }
    }

    // [20] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::Copy:System.Void(System.IntPtr,System.Single{},System.Int32,System.Int32)
    public static void Method20()
    {
        try { Marshal.Copy(IntPtr.Zero, null!, 42, 42); }
        catch { _exitCode = 1; }
    }

    // [21] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::Copy:System.Void(System.IntPtr,System.Double{},System.Int32,System.Int32)
    public static void Method21()
    {
        try { Marshal.Copy(IntPtr.Zero, null!, 42, 42); }
        catch { _exitCode = 1; }
    }

    // [22] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::Copy:System.Void(System.IntPtr,System.Byte{},System.Int32,System.Int32)
    public static void Method22()
    {
        try { Marshal.Copy(IntPtr.Zero, null!, 42, 42); }
        catch { _exitCode = 1; }
    }

    // [23] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::Copy:System.Void(System.IntPtr,System.IntPtr{},System.Int32,System.Int32)
    public static void Method23()
    {
        try { Marshal.Copy(IntPtr.Zero, null!, 42, 42); }
        catch { _exitCode = 1; }
    }

    // [24] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::CreateAggregatedObject:System.IntPtr(System.IntPtr,System.Object)
    public static void Method24()
    {
        try { if ((int)(Marshal.CreateAggregatedObject(IntPtr.Zero, 42)) != (int)(Marshal.CreateAggregatedObject(IntPtr.Zero, 42))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [25] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::CreateAggregatedObject{T}:System.IntPtr(System.IntPtr,T)

    // [26] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::CreateWrapperOfType:System.Object(System.Object,System.Type)
    public static void Method26()
    {
        try { if (((Marshal.CreateWrapperOfType(42, typeof(byte))).GetHashCode()) != ((Marshal.CreateWrapperOfType(42, typeof(byte))).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [27] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::CreateWrapperOfType{T,TWrapper}:TWrapper(T)

    // [28] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::DestroyStructure:System.Void(System.IntPtr,System.Type)
    public static void Method28()
    {
        try { Marshal.DestroyStructure(IntPtr.Zero, typeof(byte)); }
        catch { _exitCode = 1; }
    }

    // [29] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::DestroyStructure{T}:System.Void(System.IntPtr)

    // [30] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::FinalReleaseComObject:System.Int32(System.Object)
    public static void Method30()
    {
        try { if (Marshal.FinalReleaseComObject(42) != Marshal.FinalReleaseComObject(42)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [31] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::FreeBSTR:System.Void(System.IntPtr)
    public static void Method31()
    {
        try { Marshal.FreeBSTR(IntPtr.Zero); }
        catch { _exitCode = 1; }
    }

    // [32] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::FreeCoTaskMem:System.Void(System.IntPtr)
    public static void Method32()
    {
        try { Marshal.FreeCoTaskMem(IntPtr.Zero); }
        catch { _exitCode = 1; }
    }

    // [33] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::FreeHGlobal:System.Void(System.IntPtr)
    public static void Method33()
    {
        try { Marshal.FreeHGlobal(IntPtr.Zero); }
        catch { _exitCode = 1; }
    }

    // [34] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::GenerateGuidForType:System.Guid(System.Type)
    public static void Method34()
    {
        try { if (((Marshal.GenerateGuidForType(typeof(byte))).GetHashCode()) != ((Marshal.GenerateGuidForType(typeof(byte))).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [35] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::GenerateProgIdForType:System.String(System.Type)
    public static void Method35()
    {
        try { if (((Marshal.GenerateProgIdForType(typeof(byte))).Length) != ((Marshal.GenerateProgIdForType(typeof(byte))).Length)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [36] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::GetComInterfaceForObject:System.IntPtr(System.Object,System.Type)
    public static void Method36()
    {
        try { if ((int)(Marshal.GetComInterfaceForObject(42, typeof(byte))) != (int)(Marshal.GetComInterfaceForObject(42, typeof(byte)))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [37] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::GetComInterfaceForObject{T,TInterface}:System.IntPtr(T)

    // [38] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::GetComInterfaceForObject:System.IntPtr(System.Object,System.Type,System.Runtime.InteropServices.CustomQueryInterfaceMode)
    public static void Method38()
    {
        try { if ((int)(Marshal.GetComInterfaceForObject(42, typeof(byte), null!)) != (int)(Marshal.GetComInterfaceForObject(42, typeof(byte), null!))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [39] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::GetComObjectData:System.Object(System.Object,System.Object)
    public static void Method39()
    {
        try { if (((Marshal.GetComObjectData(42, 42)).GetHashCode()) != ((Marshal.GetComObjectData(42, 42)).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [40] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::GetDelegateForFunctionPointer:System.Delegate(System.IntPtr,System.Type)
    public static void Method40()
    {
        try { if (((Marshal.GetDelegateForFunctionPointer(IntPtr.Zero, typeof(byte))).GetHashCode()) != ((Marshal.GetDelegateForFunctionPointer(IntPtr.Zero, typeof(byte))).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [41] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::GetDelegateForFunctionPointer{TDelegate}:TDelegate(System.IntPtr)

    // [42] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::GetEndComSlot:System.Int32(System.Type)
    public static void Method42()
    {
        try { if (Marshal.GetEndComSlot(typeof(byte)) != Marshal.GetEndComSlot(typeof(byte))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [43] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::GetExceptionCode:System.Int32()
    public static void Method43()
    {
        try { if (Marshal.GetExceptionCode() != Marshal.GetExceptionCode()) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [44] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::GetExceptionForHR:System.Exception(System.Int32)
    public static void Method44()
    {
        try { if (((Marshal.GetExceptionForHR(42)).GetHashCode()) != ((Marshal.GetExceptionForHR(42)).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [45] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::GetExceptionForHR:System.Exception(System.Int32,System.IntPtr)
    public static void Method45()
    {
        try { if (((Marshal.GetExceptionForHR(42, IntPtr.Zero)).GetHashCode()) != ((Marshal.GetExceptionForHR(42, IntPtr.Zero)).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [46] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::GetExceptionForHR:System.Exception(System.Int32,System.Guid&,System.IntPtr)
    public static void Method46()
    {
    var refLocal_1 = Guid.Empty;
        try { if (((Marshal.GetExceptionForHR(42, ref refLocal_1, IntPtr.Zero)).GetHashCode()) != ((Marshal.GetExceptionForHR(42, ref refLocal_1, IntPtr.Zero)).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [47] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::GetExceptionPointers:System.IntPtr()
    public static void Method47()
    {
        try { if ((int)(Marshal.GetExceptionPointers()) != (int)(Marshal.GetExceptionPointers())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [48] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::GetFunctionPointerForDelegate:System.IntPtr(System.Delegate)
    public static void Method48()
    {
        try { if ((int)(Marshal.GetFunctionPointerForDelegate(null!)) != (int)(Marshal.GetFunctionPointerForDelegate(null!))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [49] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::GetFunctionPointerForDelegate{TDelegate}:System.IntPtr(TDelegate)

    // [50] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::GetHINSTANCE:System.IntPtr(System.Reflection.Module)
    public static void Method50()
    {
        try { if ((int)(Marshal.GetHINSTANCE(null!)) != (int)(Marshal.GetHINSTANCE(null!))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [51] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::GetHRForException:System.Int32(System.Exception)
    public static void Method51()
    {
        try { if (Marshal.GetHRForException(null!) != Marshal.GetHRForException(null!)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [52] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::GetHRForLastWin32Error:System.Int32()
    public static void Method52()
    {
        try { if (Marshal.GetHRForLastWin32Error() != Marshal.GetHRForLastWin32Error()) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [53] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::GetIDispatchForObject:System.IntPtr(System.Object)
    public static void Method53()
    {
        try { if ((int)(Marshal.GetIDispatchForObject(42)) != (int)(Marshal.GetIDispatchForObject(42))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [54] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::GetIUnknownForObject:System.IntPtr(System.Object)
    public static void Method54()
    {
        try { if ((int)(Marshal.GetIUnknownForObject(42)) != (int)(Marshal.GetIUnknownForObject(42))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [55] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::GetLastPInvokeError:System.Int32()
    public static void Method55()
    {
        try { if (Marshal.GetLastPInvokeError() != Marshal.GetLastPInvokeError()) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [56] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::GetLastPInvokeErrorMessage:System.String()
    public static void Method56()
    {
        try { if (((Marshal.GetLastPInvokeErrorMessage()).Length) != ((Marshal.GetLastPInvokeErrorMessage()).Length)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [57] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::GetLastSystemError:System.Int32()
    public static void Method57()
    {
        try { if (Marshal.GetLastSystemError() != Marshal.GetLastSystemError()) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [58] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::GetLastWin32Error:System.Int32()
    public static void Method58()
    {
        try { if (Marshal.GetLastWin32Error() != Marshal.GetLastWin32Error()) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [59] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::GetNativeVariantForObject:System.Void(System.Object,System.IntPtr)
    public static void Method59()
    {
        try { Marshal.GetNativeVariantForObject(42, IntPtr.Zero); }
        catch { _exitCode = 1; }
    }

    // [60] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::GetNativeVariantForObject{T}:System.Void(T,System.IntPtr)

    // [61] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::GetObjectForIUnknown:System.Object(System.IntPtr)
    public static void Method61()
    {
        try { if (((Marshal.GetObjectForIUnknown(IntPtr.Zero)).GetHashCode()) != ((Marshal.GetObjectForIUnknown(IntPtr.Zero)).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [62] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::GetObjectForNativeVariant:System.Object(System.IntPtr)
    public static void Method62()
    {
        try { if (((Marshal.GetObjectForNativeVariant(IntPtr.Zero)).GetHashCode()) != ((Marshal.GetObjectForNativeVariant(IntPtr.Zero)).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [63] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::GetObjectForNativeVariant{T}:T(System.IntPtr)

    // [64] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::GetObjectsForNativeVariants:System.Object{}(System.IntPtr,System.Int32)
    public static void Method64()
    {
        try { if ((int)(Marshal.GetObjectsForNativeVariants(IntPtr.Zero, 42)) != (int)(Marshal.GetObjectsForNativeVariants(IntPtr.Zero, 42))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [65] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::GetObjectsForNativeVariants{T}:T{}(System.IntPtr,System.Int32)

    // [66] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::GetPInvokeErrorMessage:System.String(System.Int32)
    public static void Method66()
    {
        try { if (((Marshal.GetPInvokeErrorMessage(42)).Length) != ((Marshal.GetPInvokeErrorMessage(42)).Length)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [67] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::GetStartComSlot:System.Int32(System.Type)
    public static void Method67()
    {
        try { if (Marshal.GetStartComSlot(typeof(byte)) != Marshal.GetStartComSlot(typeof(byte))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [68] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::GetTypedObjectForIUnknown:System.Object(System.IntPtr,System.Type)
    public static void Method68()
    {
        try { if (((Marshal.GetTypedObjectForIUnknown(IntPtr.Zero, typeof(byte))).GetHashCode()) != ((Marshal.GetTypedObjectForIUnknown(IntPtr.Zero, typeof(byte))).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [69] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::GetTypeFromCLSID:System.Type(System.Guid)
    public static void Method69()
    {
        try { if (((Marshal.GetTypeFromCLSID(Guid.Empty)).GetHashCode()) != ((Marshal.GetTypeFromCLSID(Guid.Empty)).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [70] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::GetTypeInfoName:System.String(System.Runtime.InteropServices.ComTypes.ITypeInfo)
    public static void Method70()
    {
        try { if (((Marshal.GetTypeInfoName(null!)).Length) != ((Marshal.GetTypeInfoName(null!)).Length)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [71] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::GetUniqueObjectForIUnknown:System.Object(System.IntPtr)
    public static void Method71()
    {
        try { if (((Marshal.GetUniqueObjectForIUnknown(IntPtr.Zero)).GetHashCode()) != ((Marshal.GetUniqueObjectForIUnknown(IntPtr.Zero)).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [72] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::InitHandle:System.Void(System.Runtime.InteropServices.SafeHandle,System.IntPtr)
    public static void Method72()
    {
        try { Marshal.InitHandle(null!, IntPtr.Zero); }
        catch { _exitCode = 1; }
    }

    // [73] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::IsComObject:System.Boolean(System.Object)
    public static void Method73()
    {
        try { if (((Marshal.IsComObject(42)) ? 1 : 0) != ((Marshal.IsComObject(42)) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [74] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::IsTypeVisibleFromCom:System.Boolean(System.Type)
    public static void Method74()
    {
        try { if (((Marshal.IsTypeVisibleFromCom(typeof(byte))) ? 1 : 0) != ((Marshal.IsTypeVisibleFromCom(typeof(byte))) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [75] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::OffsetOf:System.IntPtr(System.Type,System.String)
    public static void Method75()
    {
        try { if ((int)(Marshal.OffsetOf(typeof(byte), "hello")) != (int)(Marshal.OffsetOf(typeof(byte), "hello"))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [76] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::OffsetOf{T}:System.IntPtr(System.String)

    // [77] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::Prelink:System.Void(System.Reflection.MethodInfo)
    public static void Method77()
    {
        try { Marshal.Prelink(null!); }
        catch { _exitCode = 1; }
    }

    // [78] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::PrelinkAll:System.Void(System.Type)
    public static void Method78()
    {
        try { Marshal.PrelinkAll(typeof(byte)); }
        catch { _exitCode = 1; }
    }

    // [79] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::PtrToStringAnsi:System.String(System.IntPtr)
    public static void Method79()
    {
        try { if (((Marshal.PtrToStringAnsi(IntPtr.Zero)).Length) != ((Marshal.PtrToStringAnsi(IntPtr.Zero)).Length)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [80] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::PtrToStringAnsi:System.String(System.IntPtr,System.Int32)
    public static void Method80()
    {
        try { if (((Marshal.PtrToStringAnsi(IntPtr.Zero, 42)).Length) != ((Marshal.PtrToStringAnsi(IntPtr.Zero, 42)).Length)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [81] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::PtrToStringAuto:System.String(System.IntPtr,System.Int32)
    public static void Method81()
    {
        try { if (((Marshal.PtrToStringAuto(IntPtr.Zero, 42)).Length) != ((Marshal.PtrToStringAuto(IntPtr.Zero, 42)).Length)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [82] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::PtrToStringAuto:System.String(System.IntPtr)
    public static void Method82()
    {
        try { if (((Marshal.PtrToStringAuto(IntPtr.Zero)).Length) != ((Marshal.PtrToStringAuto(IntPtr.Zero)).Length)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [83] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::PtrToStringBSTR:System.String(System.IntPtr)
    public static void Method83()
    {
        try { if (((Marshal.PtrToStringBSTR(IntPtr.Zero)).Length) != ((Marshal.PtrToStringBSTR(IntPtr.Zero)).Length)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [84] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::PtrToStringUni:System.String(System.IntPtr)
    public static void Method84()
    {
        try { if (((Marshal.PtrToStringUni(IntPtr.Zero)).Length) != ((Marshal.PtrToStringUni(IntPtr.Zero)).Length)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [85] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::PtrToStringUni:System.String(System.IntPtr,System.Int32)
    public static void Method85()
    {
        try { if (((Marshal.PtrToStringUni(IntPtr.Zero, 42)).Length) != ((Marshal.PtrToStringUni(IntPtr.Zero, 42)).Length)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [86] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::PtrToStringUTF8:System.String(System.IntPtr)
    public static void Method86()
    {
        try { if (((Marshal.PtrToStringUTF8(IntPtr.Zero)).Length) != ((Marshal.PtrToStringUTF8(IntPtr.Zero)).Length)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [87] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::PtrToStringUTF8:System.String(System.IntPtr,System.Int32)
    public static void Method87()
    {
        try { if (((Marshal.PtrToStringUTF8(IntPtr.Zero, 42)).Length) != ((Marshal.PtrToStringUTF8(IntPtr.Zero, 42)).Length)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [88] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::PtrToStructure:System.Object(System.IntPtr,System.Type)
    public static void Method88()
    {
        try { if (((Marshal.PtrToStructure(IntPtr.Zero, typeof(byte))).GetHashCode()) != ((Marshal.PtrToStructure(IntPtr.Zero, typeof(byte))).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [89] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::PtrToStructure:System.Void(System.IntPtr,System.Object)
    public static void Method89()
    {
        try { Marshal.PtrToStructure(IntPtr.Zero, 42); }
        catch { _exitCode = 1; }
    }

    // [90] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::PtrToStructure{T}:System.Void(System.IntPtr,T)

    // [91] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::PtrToStructure{T}:T(System.IntPtr)

    // [92] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::QueryInterface:System.Int32(System.IntPtr,System.Guid&,System.IntPtr&)
    public static void Method92()
    {
    var refLocal_1 = Guid.Empty;
    var refLocal_2 = IntPtr.Zero;
        try { if (Marshal.QueryInterface(IntPtr.Zero, ref refLocal_1, ref refLocal_2) != Marshal.QueryInterface(IntPtr.Zero, ref refLocal_1, ref refLocal_2)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [93] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::ReadByte:System.Byte(System.Object,System.Int32)
    public static void Method93()
    {
        try { if ((int)(Marshal.ReadByte(42, 42)) != (int)(Marshal.ReadByte(42, 42))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [94] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::ReadByte:System.Byte(System.IntPtr,System.Int32)
    public static void Method94()
    {
        try { if ((int)(Marshal.ReadByte(IntPtr.Zero, 42)) != (int)(Marshal.ReadByte(IntPtr.Zero, 42))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [95] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::ReadByte:System.Byte(System.IntPtr)
    public static void Method95()
    {
        try { if ((int)(Marshal.ReadByte(IntPtr.Zero)) != (int)(Marshal.ReadByte(IntPtr.Zero))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [96] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::ReadInt16:System.Int16(System.Object,System.Int32)
    public static void Method96()
    {
        try { if ((int)(Marshal.ReadInt16(42, 42)) != (int)(Marshal.ReadInt16(42, 42))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [97] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::ReadInt16:System.Int16(System.IntPtr,System.Int32)
    public static void Method97()
    {
        try { if ((int)(Marshal.ReadInt16(IntPtr.Zero, 42)) != (int)(Marshal.ReadInt16(IntPtr.Zero, 42))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [98] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::ReadInt16:System.Int16(System.IntPtr)
    public static void Method98()
    {
        try { if ((int)(Marshal.ReadInt16(IntPtr.Zero)) != (int)(Marshal.ReadInt16(IntPtr.Zero))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [99] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::ReadInt32:System.Int32(System.Object,System.Int32)
    public static void Method99()
    {
        try { if (Marshal.ReadInt32(42, 42) != Marshal.ReadInt32(42, 42)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [100] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::ReadInt32:System.Int32(System.IntPtr,System.Int32)
    public static void Method100()
    {
        try { if (Marshal.ReadInt32(IntPtr.Zero, 42) != Marshal.ReadInt32(IntPtr.Zero, 42)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [101] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::ReadInt32:System.Int32(System.IntPtr)
    public static void Method101()
    {
        try { if (Marshal.ReadInt32(IntPtr.Zero) != Marshal.ReadInt32(IntPtr.Zero)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [102] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::ReadInt64:System.Int64(System.Object,System.Int32)
    public static void Method102()
    {
        try { if ((int)(Marshal.ReadInt64(42, 42)) != (int)(Marshal.ReadInt64(42, 42))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [103] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::ReadInt64:System.Int64(System.IntPtr,System.Int32)
    public static void Method103()
    {
        try { if ((int)(Marshal.ReadInt64(IntPtr.Zero, 42)) != (int)(Marshal.ReadInt64(IntPtr.Zero, 42))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [104] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::ReadInt64:System.Int64(System.IntPtr)
    public static void Method104()
    {
        try { if ((int)(Marshal.ReadInt64(IntPtr.Zero)) != (int)(Marshal.ReadInt64(IntPtr.Zero))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [105] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::ReadIntPtr:System.IntPtr(System.Object,System.Int32)
    public static void Method105()
    {
        try { if ((int)(Marshal.ReadIntPtr(42, 42)) != (int)(Marshal.ReadIntPtr(42, 42))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [106] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::ReadIntPtr:System.IntPtr(System.IntPtr,System.Int32)
    public static void Method106()
    {
        try { if ((int)(Marshal.ReadIntPtr(IntPtr.Zero, 42)) != (int)(Marshal.ReadIntPtr(IntPtr.Zero, 42))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [107] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::ReadIntPtr:System.IntPtr(System.IntPtr)
    public static void Method107()
    {
        try { if ((int)(Marshal.ReadIntPtr(IntPtr.Zero)) != (int)(Marshal.ReadIntPtr(IntPtr.Zero))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [108] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::ReAllocCoTaskMem:System.IntPtr(System.IntPtr,System.Int32)
    public static void Method108()
    {
        try { if ((int)(Marshal.ReAllocCoTaskMem(IntPtr.Zero, 42)) != (int)(Marshal.ReAllocCoTaskMem(IntPtr.Zero, 42))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [109] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::ReAllocHGlobal:System.IntPtr(System.IntPtr,System.IntPtr)
    public static void Method109()
    {
        try { if ((int)(Marshal.ReAllocHGlobal(IntPtr.Zero, IntPtr.Zero)) != (int)(Marshal.ReAllocHGlobal(IntPtr.Zero, IntPtr.Zero))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [110] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::Release:System.Int32(System.IntPtr)
    public static void Method110()
    {
        try { if (Marshal.Release(IntPtr.Zero) != Marshal.Release(IntPtr.Zero)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [111] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::ReleaseComObject:System.Int32(System.Object)
    public static void Method111()
    {
        try { if (Marshal.ReleaseComObject(42) != Marshal.ReleaseComObject(42)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [112] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::SecureStringToBSTR:System.IntPtr(System.Security.SecureString)
    public static void Method112()
    {
        try { if ((int)(Marshal.SecureStringToBSTR(null!)) != (int)(Marshal.SecureStringToBSTR(null!))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [113] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::SecureStringToCoTaskMemAnsi:System.IntPtr(System.Security.SecureString)
    public static void Method113()
    {
        try { if ((int)(Marshal.SecureStringToCoTaskMemAnsi(null!)) != (int)(Marshal.SecureStringToCoTaskMemAnsi(null!))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [114] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::SecureStringToCoTaskMemUnicode:System.IntPtr(System.Security.SecureString)
    public static void Method114()
    {
        try { if ((int)(Marshal.SecureStringToCoTaskMemUnicode(null!)) != (int)(Marshal.SecureStringToCoTaskMemUnicode(null!))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [115] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::SecureStringToGlobalAllocAnsi:System.IntPtr(System.Security.SecureString)
    public static void Method115()
    {
        try { if ((int)(Marshal.SecureStringToGlobalAllocAnsi(null!)) != (int)(Marshal.SecureStringToGlobalAllocAnsi(null!))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [116] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::SecureStringToGlobalAllocUnicode:System.IntPtr(System.Security.SecureString)
    public static void Method116()
    {
        try { if ((int)(Marshal.SecureStringToGlobalAllocUnicode(null!)) != (int)(Marshal.SecureStringToGlobalAllocUnicode(null!))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [117] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::SetComObjectData:System.Boolean(System.Object,System.Object,System.Object)
    public static void Method117()
    {
        try { if (((Marshal.SetComObjectData(42, 42, 42)) ? 1 : 0) != ((Marshal.SetComObjectData(42, 42, 42)) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [118] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::SetLastPInvokeError:System.Void(System.Int32)
    public static void Method118()
    {
        try { Marshal.SetLastPInvokeError(42); }
        catch { _exitCode = 1; }
    }

    // [119] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::SetLastSystemError:System.Void(System.Int32)
    public static void Method119()
    {
        try { Marshal.SetLastSystemError(42); }
        catch { _exitCode = 1; }
    }

    // [120] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::SizeOf:System.Int32(System.Object)
    public static void Method120()
    {
        try { if (Marshal.SizeOf(42) != Marshal.SizeOf(42)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [121] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::SizeOf{T}:System.Int32(T)

    // [122] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::SizeOf:System.Int32(System.Type)
    public static void Method122()
    {
        try { if (Marshal.SizeOf(typeof(byte)) != Marshal.SizeOf(typeof(byte))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [123] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::SizeOf{T}:System.Int32()

    // [124] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::StringToBSTR:System.IntPtr(System.String)
    public static void Method124()
    {
        try { if ((int)(Marshal.StringToBSTR("hello")) != (int)(Marshal.StringToBSTR("hello"))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [125] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::StringToCoTaskMemAnsi:System.IntPtr(System.String)
    public static void Method125()
    {
        try { if ((int)(Marshal.StringToCoTaskMemAnsi("hello")) != (int)(Marshal.StringToCoTaskMemAnsi("hello"))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [126] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::StringToCoTaskMemAuto:System.IntPtr(System.String)
    public static void Method126()
    {
        try { if ((int)(Marshal.StringToCoTaskMemAuto("hello")) != (int)(Marshal.StringToCoTaskMemAuto("hello"))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [127] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::StringToCoTaskMemUni:System.IntPtr(System.String)
    public static void Method127()
    {
        try { if ((int)(Marshal.StringToCoTaskMemUni("hello")) != (int)(Marshal.StringToCoTaskMemUni("hello"))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [128] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::StringToCoTaskMemUTF8:System.IntPtr(System.String)
    public static void Method128()
    {
        try { if ((int)(Marshal.StringToCoTaskMemUTF8("hello")) != (int)(Marshal.StringToCoTaskMemUTF8("hello"))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [129] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::StringToHGlobalAnsi:System.IntPtr(System.String)
    public static void Method129()
    {
        try { if ((int)(Marshal.StringToHGlobalAnsi("hello")) != (int)(Marshal.StringToHGlobalAnsi("hello"))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [130] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::StringToHGlobalAuto:System.IntPtr(System.String)
    public static void Method130()
    {
        try { if ((int)(Marshal.StringToHGlobalAuto("hello")) != (int)(Marshal.StringToHGlobalAuto("hello"))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [131] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::StringToHGlobalUni:System.IntPtr(System.String)
    public static void Method131()
    {
        try { if ((int)(Marshal.StringToHGlobalUni("hello")) != (int)(Marshal.StringToHGlobalUni("hello"))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [132] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::StructureToPtr:System.Void(System.Object,System.IntPtr,System.Boolean)
    public static void Method132()
    {
        try { Marshal.StructureToPtr(42, IntPtr.Zero, true); }
        catch { _exitCode = 1; }
    }

    // [133] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::StructureToPtr{T}:System.Void(T,System.IntPtr,System.Boolean)

    // [134] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::ThrowExceptionForHR:System.Void(System.Int32)
    public static void Method134()
    {
        try { Marshal.ThrowExceptionForHR(42); }
        catch { _exitCode = 1; }
    }

    // [135] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::ThrowExceptionForHR:System.Void(System.Int32,System.IntPtr)
    public static void Method135()
    {
        try { Marshal.ThrowExceptionForHR(42, IntPtr.Zero); }
        catch { _exitCode = 1; }
    }

    // [136] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::ThrowExceptionForHR:System.Void(System.Int32,System.Guid&,System.IntPtr)
    public static void Method136()
    {
    var refLocal_1 = Guid.Empty;
        try { Marshal.ThrowExceptionForHR(42, ref refLocal_1, IntPtr.Zero); }
        catch { _exitCode = 1; }
    }

    // [137] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::UnsafeAddrOfPinnedArrayElement:System.IntPtr(System.Array,System.Int32)
    public static void Method137()
    {
        try { if ((int)(Marshal.UnsafeAddrOfPinnedArrayElement(Array.Empty<byte>(), 42)) != (int)(Marshal.UnsafeAddrOfPinnedArrayElement(Array.Empty<byte>(), 42))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [138] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::UnsafeAddrOfPinnedArrayElement{T}:System.IntPtr(T{},System.Int32)

    // [139] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::WriteByte:System.Void(System.Object,System.Int32,System.Byte)
    public static void Method139()
    {
        try { Marshal.WriteByte(42, 42, (byte)42); }
        catch { _exitCode = 1; }
    }

    // [140] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::WriteByte:System.Void(System.IntPtr,System.Int32,System.Byte)
    public static void Method140()
    {
        try { Marshal.WriteByte(IntPtr.Zero, 42, (byte)42); }
        catch { _exitCode = 1; }
    }

    // [141] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::WriteByte:System.Void(System.IntPtr,System.Byte)
    public static void Method141()
    {
        try { Marshal.WriteByte(IntPtr.Zero, (byte)42); }
        catch { _exitCode = 1; }
    }

    // [142] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::WriteInt16:System.Void(System.Object,System.Int32,System.Int16)
    public static void Method142()
    {
        try { Marshal.WriteInt16(42, 42, (short)42); }
        catch { _exitCode = 1; }
    }

    // [143] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::WriteInt16:System.Void(System.IntPtr,System.Int32,System.Int16)
    public static void Method143()
    {
        try { Marshal.WriteInt16(IntPtr.Zero, 42, (short)42); }
        catch { _exitCode = 1; }
    }

    // [144] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::WriteInt16:System.Void(System.IntPtr,System.Int16)
    public static void Method144()
    {
        try { Marshal.WriteInt16(IntPtr.Zero, (short)42); }
        catch { _exitCode = 1; }
    }

    // [145] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::WriteInt16:System.Void(System.IntPtr,System.Int32,System.Char)
    public static void Method145()
    {
        try { Marshal.WriteInt16(IntPtr.Zero, 42, 'A'); }
        catch { _exitCode = 1; }
    }

    // [146] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::WriteInt16:System.Void(System.Object,System.Int32,System.Char)
    public static void Method146()
    {
        try { Marshal.WriteInt16(42, 42, 'A'); }
        catch { _exitCode = 1; }
    }

    // [147] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::WriteInt16:System.Void(System.IntPtr,System.Char)
    public static void Method147()
    {
        try { Marshal.WriteInt16(IntPtr.Zero, 'A'); }
        catch { _exitCode = 1; }
    }

    // [148] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::WriteInt32:System.Void(System.Object,System.Int32,System.Int32)
    public static void Method148()
    {
        try { Marshal.WriteInt32(42, 42, 42); }
        catch { _exitCode = 1; }
    }

    // [149] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::WriteInt32:System.Void(System.IntPtr,System.Int32,System.Int32)
    public static void Method149()
    {
        try { Marshal.WriteInt32(IntPtr.Zero, 42, 42); }
        catch { _exitCode = 1; }
    }

    // [150] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::WriteInt32:System.Void(System.IntPtr,System.Int32)
    public static void Method150()
    {
        try { Marshal.WriteInt32(IntPtr.Zero, 42); }
        catch { _exitCode = 1; }
    }

    // [151] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::WriteInt64:System.Void(System.Object,System.Int32,System.Int64)
    public static void Method151()
    {
        try { Marshal.WriteInt64(42, 42, 42L); }
        catch { _exitCode = 1; }
    }

    // [152] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::WriteInt64:System.Void(System.IntPtr,System.Int32,System.Int64)
    public static void Method152()
    {
        try { Marshal.WriteInt64(IntPtr.Zero, 42, 42L); }
        catch { _exitCode = 1; }
    }

    // [153] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::WriteInt64:System.Void(System.IntPtr,System.Int64)
    public static void Method153()
    {
        try { Marshal.WriteInt64(IntPtr.Zero, 42L); }
        catch { _exitCode = 1; }
    }

    // [154] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::WriteIntPtr:System.Void(System.IntPtr,System.Int32,System.IntPtr)
    public static void Method154()
    {
        try { Marshal.WriteIntPtr(IntPtr.Zero, 42, IntPtr.Zero); }
        catch { _exitCode = 1; }
    }

    // [155] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::WriteIntPtr:System.Void(System.Object,System.Int32,System.IntPtr)
    public static void Method155()
    {
        try { Marshal.WriteIntPtr(42, 42, IntPtr.Zero); }
        catch { _exitCode = 1; }
    }

    // [156] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::WriteIntPtr:System.Void(System.IntPtr,System.IntPtr)
    public static void Method156()
    {
        try { Marshal.WriteIntPtr(IntPtr.Zero, IntPtr.Zero); }
        catch { _exitCode = 1; }
    }

    // [157] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::ZeroFreeBSTR:System.Void(System.IntPtr)
    public static void Method157()
    {
        try { Marshal.ZeroFreeBSTR(IntPtr.Zero); }
        catch { _exitCode = 1; }
    }

    // [158] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::ZeroFreeCoTaskMemAnsi:System.Void(System.IntPtr)
    public static void Method158()
    {
        try { Marshal.ZeroFreeCoTaskMemAnsi(IntPtr.Zero); }
        catch { _exitCode = 1; }
    }

    // [159] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::ZeroFreeCoTaskMemUnicode:System.Void(System.IntPtr)
    public static void Method159()
    {
        try { Marshal.ZeroFreeCoTaskMemUnicode(IntPtr.Zero); }
        catch { _exitCode = 1; }
    }

    // [160] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::ZeroFreeCoTaskMemUTF8:System.Void(System.IntPtr)
    public static void Method160()
    {
        try { Marshal.ZeroFreeCoTaskMemUTF8(IntPtr.Zero); }
        catch { _exitCode = 1; }
    }

    // [161] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::ZeroFreeGlobalAllocAnsi:System.Void(System.IntPtr)
    public static void Method161()
    {
        try { Marshal.ZeroFreeGlobalAllocAnsi(IntPtr.Zero); }
        catch { _exitCode = 1; }
    }

    // [162] System.Runtime.InteropServices/System.Runtime.InteropServices.Marshal::ZeroFreeGlobalAllocUnicode:System.Void(System.IntPtr)
    public static void Method162()
    {
        try { Marshal.ZeroFreeGlobalAllocUnicode(IntPtr.Zero); }
        catch { _exitCode = 1; }
    }

    // [163] System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::Dispose:System.Void()
    public static void Method163()
    {
        try { UnmanagedMemoryAccessor.Dispose(); }
        catch { _exitCode = 1; }
    }

    // [164] System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::get_CanRead:System.Boolean()
    public static void Method164()
    {
        try { if (((UnmanagedMemoryAccessor.CanRead) ? 1 : 0) != ((UnmanagedMemoryAccessor.CanRead) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [165] System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::get_CanWrite:System.Boolean()
    public static void Method165()
    {
        try { if (((UnmanagedMemoryAccessor.CanWrite) ? 1 : 0) != ((UnmanagedMemoryAccessor.CanWrite) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [166] System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::get_Capacity:System.Int64()
    public static void Method166()
    {
        try { if ((int)(UnmanagedMemoryAccessor.Capacity) != (int)(UnmanagedMemoryAccessor.Capacity)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [167] System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::Read{T}:System.Void(System.Int64,T&)

    // [168] System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::ReadArray{T}:System.Int32(System.Int64,T{},System.Int32,System.Int32)

    // [169] System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::ReadBoolean:System.Boolean(System.Int64)
    public static void Method169()
    {
        try { if (((UnmanagedMemoryAccessor.ReadBoolean(42L)) ? 1 : 0) != ((UnmanagedMemoryAccessor.ReadBoolean(42L)) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [170] System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::ReadByte:System.Byte(System.Int64)
    public static void Method170()
    {
        try { if ((int)(UnmanagedMemoryAccessor.ReadByte(42L)) != (int)(UnmanagedMemoryAccessor.ReadByte(42L))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [171] System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::ReadChar:System.Char(System.Int64)
    public static void Method171()
    {
        try { if ((int)(UnmanagedMemoryAccessor.ReadChar(42L)) != (int)(UnmanagedMemoryAccessor.ReadChar(42L))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [172] System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::ReadDecimal:System.Decimal(System.Int64)
    public static void Method172()
    {
        try { if ((int)(UnmanagedMemoryAccessor.ReadDecimal(42L)) != (int)(UnmanagedMemoryAccessor.ReadDecimal(42L))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [173] System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::ReadDouble:System.Double(System.Int64)
    public static void Method173()
    {
        try { if ((int)(UnmanagedMemoryAccessor.ReadDouble(42L)) != (int)(UnmanagedMemoryAccessor.ReadDouble(42L))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [174] System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::ReadInt16:System.Int16(System.Int64)
    public static void Method174()
    {
        try { if ((int)(UnmanagedMemoryAccessor.ReadInt16(42L)) != (int)(UnmanagedMemoryAccessor.ReadInt16(42L))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [175] System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::ReadInt32:System.Int32(System.Int64)
    public static void Method175()
    {
        try { if (UnmanagedMemoryAccessor.ReadInt32(42L) != UnmanagedMemoryAccessor.ReadInt32(42L)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [176] System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::ReadInt64:System.Int64(System.Int64)
    public static void Method176()
    {
        try { if ((int)(UnmanagedMemoryAccessor.ReadInt64(42L)) != (int)(UnmanagedMemoryAccessor.ReadInt64(42L))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [177] System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::ReadSByte:System.SByte(System.Int64)
    public static void Method177()
    {
        try { if ((int)(UnmanagedMemoryAccessor.ReadSByte(42L)) != (int)(UnmanagedMemoryAccessor.ReadSByte(42L))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [178] System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::ReadSingle:System.Single(System.Int64)
    public static void Method178()
    {
        try { if ((int)(UnmanagedMemoryAccessor.ReadSingle(42L)) != (int)(UnmanagedMemoryAccessor.ReadSingle(42L))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [179] System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::ReadUInt16:System.UInt16(System.Int64)
    public static void Method179()
    {
        try { if ((int)(UnmanagedMemoryAccessor.ReadUInt16(42L)) != (int)(UnmanagedMemoryAccessor.ReadUInt16(42L))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [180] System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::ReadUInt32:System.UInt32(System.Int64)
    public static void Method180()
    {
        try { if ((int)(UnmanagedMemoryAccessor.ReadUInt32(42L)) != (int)(UnmanagedMemoryAccessor.ReadUInt32(42L))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [181] System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::ReadUInt64:System.UInt64(System.Int64)
    public static void Method181()
    {
        try { if ((int)(UnmanagedMemoryAccessor.ReadUInt64(42L)) != (int)(UnmanagedMemoryAccessor.ReadUInt64(42L))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [182] System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::Write:System.Void(System.Int64,System.Boolean)
    public static void Method182()
    {
        try { UnmanagedMemoryAccessor.Write(42L, true); }
        catch { _exitCode = 1; }
    }

    // [183] System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::Write:System.Void(System.Int64,System.Byte)
    public static void Method183()
    {
        try { UnmanagedMemoryAccessor.Write(42L, (byte)42); }
        catch { _exitCode = 1; }
    }

    // [184] System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::Write:System.Void(System.Int64,System.Char)
    public static void Method184()
    {
        try { UnmanagedMemoryAccessor.Write(42L, 'A'); }
        catch { _exitCode = 1; }
    }

    // [185] System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::Write:System.Void(System.Int64,System.Int16)
    public static void Method185()
    {
        try { UnmanagedMemoryAccessor.Write(42L, (short)42); }
        catch { _exitCode = 1; }
    }

    // [186] System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::Write:System.Void(System.Int64,System.Int32)
    public static void Method186()
    {
        try { UnmanagedMemoryAccessor.Write(42L, 42); }
        catch { _exitCode = 1; }
    }

    // [187] System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::Write:System.Void(System.Int64,System.Int64)
    public static void Method187()
    {
        try { UnmanagedMemoryAccessor.Write(42L, 42L); }
        catch { _exitCode = 1; }
    }

    // [188] System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::Write:System.Void(System.Int64,System.Decimal)
    public static void Method188()
    {
        try { UnmanagedMemoryAccessor.Write(42L, 42m); }
        catch { _exitCode = 1; }
    }

    // [189] System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::Write:System.Void(System.Int64,System.Single)
    public static void Method189()
    {
        try { UnmanagedMemoryAccessor.Write(42L, 42.0f); }
        catch { _exitCode = 1; }
    }

    // [190] System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::Write:System.Void(System.Int64,System.Double)
    public static void Method190()
    {
        try { UnmanagedMemoryAccessor.Write(42L, 42.0); }
        catch { _exitCode = 1; }
    }

    // [191] System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::Write:System.Void(System.Int64,System.SByte)
    public static void Method191()
    {
        try { UnmanagedMemoryAccessor.Write(42L, (sbyte)42); }
        catch { _exitCode = 1; }
    }

    // [192] System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::Write:System.Void(System.Int64,System.UInt16)
    public static void Method192()
    {
        try { UnmanagedMemoryAccessor.Write(42L, (ushort)42); }
        catch { _exitCode = 1; }
    }

    // [193] System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::Write:System.Void(System.Int64,System.UInt32)
    public static void Method193()
    {
        try { UnmanagedMemoryAccessor.Write(42L, 42u); }
        catch { _exitCode = 1; }
    }

    // [194] System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::Write:System.Void(System.Int64,System.UInt64)
    public static void Method194()
    {
        try { UnmanagedMemoryAccessor.Write(42L, 42uL); }
        catch { _exitCode = 1; }
    }

    // [195] System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::Write{T}:System.Void(System.Int64,T&)

    // [196] System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::WriteArray{T}:System.Void(System.Int64,T{},System.Int32,System.Int32)

    // [197] System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::.ctor:System.Void(System.Runtime.InteropServices.SafeBuffer,System.Int64,System.Int64)
    public static void Method197()
    {
        try { new UnmanagedMemoryAccessor(null!, 42L, 42L); }
        catch { _exitCode = 1; }
    }

    // [198] System.Runtime.InteropServices/System.IO.UnmanagedMemoryAccessor::.ctor:System.Void(System.Runtime.InteropServices.SafeBuffer,System.Int64,System.Int64,System.IO.FileAccess)
    public static void Method198()
    {
        try { new UnmanagedMemoryAccessor(null!, 42L, 42L, null!); }
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
                case 25: CustomEntryMethod25(); break;
                case 26: Method26(); break;
                case 27: CustomEntryMethod27(); break;
                case 28: Method28(); break;
                case 29: CustomEntryMethod29(); break;
                case 30: Method30(); break;
                case 31: Method31(); break;
                case 32: Method32(); break;
                case 33: Method33(); break;
                case 34: Method34(); break;
                case 35: Method35(); break;
                case 36: Method36(); break;
                case 37: CustomEntryMethod37(); break;
                case 38: Method38(); break;
                case 39: Method39(); break;
                case 40: Method40(); break;
                case 41: CustomEntryMethod41(); break;
                case 42: Method42(); break;
                case 43: Method43(); break;
                case 44: Method44(); break;
                case 45: Method45(); break;
                case 46: Method46(); break;
                case 47: Method47(); break;
                case 48: Method48(); break;
                case 49: CustomEntryMethod49(); break;
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
                case 60: CustomEntryMethod60(); break;
                case 61: Method61(); break;
                case 62: Method62(); break;
                case 63: CustomEntryMethod63(); break;
                case 64: Method64(); break;
                case 65: CustomEntryMethod65(); break;
                case 66: Method66(); break;
                case 67: Method67(); break;
                case 68: Method68(); break;
                case 69: Method69(); break;
                case 70: Method70(); break;
                case 71: Method71(); break;
                case 72: Method72(); break;
                case 73: Method73(); break;
                case 74: Method74(); break;
                case 75: Method75(); break;
                case 76: CustomEntryMethod76(); break;
                case 77: Method77(); break;
                case 78: Method78(); break;
                case 79: Method79(); break;
                case 80: Method80(); break;
                case 81: Method81(); break;
                case 82: Method82(); break;
                case 83: Method83(); break;
                case 84: Method84(); break;
                case 85: Method85(); break;
                case 86: Method86(); break;
                case 87: Method87(); break;
                case 88: Method88(); break;
                case 89: Method89(); break;
                case 90: CustomEntryMethod90(); break;
                case 91: CustomEntryMethod91(); break;
                case 92: Method92(); break;
                case 93: Method93(); break;
                case 94: Method94(); break;
                case 95: Method95(); break;
                case 96: Method96(); break;
                case 97: Method97(); break;
                case 98: Method98(); break;
                case 99: Method99(); break;
                case 100: Method100(); break;
                case 101: Method101(); break;
                case 102: Method102(); break;
                case 103: Method103(); break;
                case 104: Method104(); break;
                case 105: Method105(); break;
                case 106: Method106(); break;
                case 107: Method107(); break;
                case 108: Method108(); break;
                case 109: Method109(); break;
                case 110: Method110(); break;
                case 111: Method111(); break;
                case 112: Method112(); break;
                case 113: Method113(); break;
                case 114: Method114(); break;
                case 115: Method115(); break;
                case 116: Method116(); break;
                case 117: Method117(); break;
                case 118: Method118(); break;
                case 119: Method119(); break;
                case 120: Method120(); break;
                case 121: CustomEntryMethod121(); break;
                case 122: Method122(); break;
                case 123: CustomEntryMethod123(); break;
                case 124: Method124(); break;
                case 125: Method125(); break;
                case 126: Method126(); break;
                case 127: Method127(); break;
                case 128: Method128(); break;
                case 129: Method129(); break;
                case 130: Method130(); break;
                case 131: Method131(); break;
                case 132: Method132(); break;
                case 133: CustomEntryMethod133(); break;
                case 134: Method134(); break;
                case 135: Method135(); break;
                case 136: Method136(); break;
                case 137: Method137(); break;
                case 138: CustomEntryMethod138(); break;
                case 139: Method139(); break;
                case 140: Method140(); break;
                case 141: Method141(); break;
                case 142: Method142(); break;
                case 143: Method143(); break;
                case 144: Method144(); break;
                case 145: Method145(); break;
                case 146: Method146(); break;
                case 147: Method147(); break;
                case 148: Method148(); break;
                case 149: Method149(); break;
                case 150: Method150(); break;
                case 151: Method151(); break;
                case 152: Method152(); break;
                case 153: Method153(); break;
                case 154: Method154(); break;
                case 155: Method155(); break;
                case 156: Method156(); break;
                case 157: Method157(); break;
                case 158: Method158(); break;
                case 159: Method159(); break;
                case 160: Method160(); break;
                case 161: Method161(); break;
                case 162: Method162(); break;
                case 163: Method163(); break;
                case 164: Method164(); break;
                case 165: Method165(); break;
                case 166: Method166(); break;
                case 167: CustomEntryMethod167(); break;
                case 168: CustomEntryMethod168(); break;
                case 169: Method169(); break;
                case 170: Method170(); break;
                case 171: Method171(); break;
                case 172: Method172(); break;
                case 173: Method173(); break;
                case 174: Method174(); break;
                case 175: Method175(); break;
                case 176: Method176(); break;
                case 177: Method177(); break;
                case 178: Method178(); break;
                case 179: Method179(); break;
                case 180: Method180(); break;
                case 181: Method181(); break;
                case 182: Method182(); break;
                case 183: Method183(); break;
                case 184: Method184(); break;
                case 185: Method185(); break;
                case 186: Method186(); break;
                case 187: Method187(); break;
                case 188: Method188(); break;
                case 189: Method189(); break;
                case 190: Method190(); break;
                case 191: Method191(); break;
                case 192: Method192(); break;
                case 193: Method193(); break;
                case 194: Method194(); break;
                case 195: CustomEntryMethod195(); break;
                case 196: CustomEntryMethod196(); break;
                case 197: Method197(); break;
                case 198: Method198(); break;
            }
        }
        catch
        {
            _exitCode = 1;
        }
    }

}