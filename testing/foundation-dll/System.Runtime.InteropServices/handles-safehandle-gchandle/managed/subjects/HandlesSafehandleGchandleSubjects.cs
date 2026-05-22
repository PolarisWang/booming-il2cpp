// Auto-generated native-AOT entry point
// Family: family/System.Runtime.InteropServices/handles/safehandle/gchandle
// Assembly: System.Runtime.InteropServices
// Variant: subjects

using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;

public static partial class HandlesSafehandleGchandleSubjects
{
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    // [0] System.Runtime.InteropServices/System.Runtime.InteropServices.SafeHandle::Close:System.Void()
    public static void Subject_0()
    {
        try { default(SafeHandle)!.Close(); }
        catch { _exitCode = 1; }
    }

    // [1] System.Runtime.InteropServices/System.Runtime.InteropServices.SafeHandle::DangerousAddRef:System.Void(System.Boolean&)
    public static void Subject_1()
    {
        // TODO: System.Runtime.InteropServices/System.Runtime.InteropServices.SafeHandle::DangerousAddRef:System.Void(System.Boolean&) could not be auto-generated
    }

    // [2] System.Runtime.InteropServices/System.Runtime.InteropServices.SafeHandle::DangerousGetHandle:System.IntPtr()
    public static void Subject_2()
    {
        try { if ((int)(default(SafeHandle)!.DangerousGetHandle()) != (int)(default(SafeHandle)!.DangerousGetHandle())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [3] System.Runtime.InteropServices/System.Runtime.InteropServices.SafeHandle::DangerousRelease:System.Void()
    public static void Subject_3()
    {
        try { default(SafeHandle)!.DangerousRelease(); }
        catch { _exitCode = 1; }
    }

    // [4] System.Runtime.InteropServices/System.Runtime.InteropServices.SafeHandle::Dispose:System.Void()
    public static void Subject_4()
    {
        try { default(SafeHandle)!.Dispose(); }
        catch { _exitCode = 1; }
    }

    // [5] System.Runtime.InteropServices/System.Runtime.InteropServices.SafeHandle::get_IsClosed:System.Boolean()
    public static void Subject_5()
    {
        try { if (((default(SafeHandle)!.IsClosed) ? 1 : 0) != ((default(SafeHandle)!.IsClosed) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [6] System.Runtime.InteropServices/System.Runtime.InteropServices.SafeHandle::get_IsInvalid:System.Boolean()
    public static void Subject_6()
    {
        try { if (((default(SafeHandle)!.IsInvalid) ? 1 : 0) != ((default(SafeHandle)!.IsInvalid) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [7] System.Runtime.InteropServices/System.Runtime.InteropServices.SafeHandle::SetHandleAsInvalid:System.Void()
    public static void Subject_7()
    {
        try { default(SafeHandle)!.SetHandleAsInvalid(); }
        catch { _exitCode = 1; }
    }

    // [8] System.Runtime.InteropServices/System.Runtime.InteropServices.CriticalHandle::Close:System.Void()
    public static void Subject_8()
    {
        try { default(CriticalHandle)!.Close(); }
        catch { _exitCode = 1; }
    }

    // [9] System.Runtime.InteropServices/System.Runtime.InteropServices.CriticalHandle::Dispose:System.Void()
    public static void Subject_9()
    {
        try { default(CriticalHandle)!.Dispose(); }
        catch { _exitCode = 1; }
    }

    // [10] System.Runtime.InteropServices/System.Runtime.InteropServices.CriticalHandle::get_IsClosed:System.Boolean()
    public static void Subject_10()
    {
        try { if (((default(CriticalHandle)!.IsClosed) ? 1 : 0) != ((default(CriticalHandle)!.IsClosed) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [11] System.Runtime.InteropServices/System.Runtime.InteropServices.CriticalHandle::get_IsInvalid:System.Boolean()
    public static void Subject_11()
    {
        try { if (((default(CriticalHandle)!.IsInvalid) ? 1 : 0) != ((default(CriticalHandle)!.IsInvalid) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [12] System.Runtime.InteropServices/System.Runtime.InteropServices.CriticalHandle::SetHandleAsInvalid:System.Void()
    public static void Subject_12()
    {
        try { default(CriticalHandle)!.SetHandleAsInvalid(); }
        catch { _exitCode = 1; }
    }

    // [13] System.Runtime.InteropServices/System.Runtime.InteropServices.GCHandle::AddrOfPinnedObject:System.IntPtr()
    public static void Subject_13()
    {
        try { if ((int)(default(GCHandle).AddrOfPinnedObject()) != (int)(default(GCHandle).AddrOfPinnedObject())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [14] System.Runtime.InteropServices/System.Runtime.InteropServices.GCHandle::Alloc:System.Runtime.InteropServices.GCHandle(System.Object)
    public static void Subject_14()
    {
        try { if (((GCHandle.Alloc(42)).GetHashCode()) != ((GCHandle.Alloc(42)).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [15] System.Runtime.InteropServices/System.Runtime.InteropServices.GCHandle::Alloc:System.Runtime.InteropServices.GCHandle(System.Object,System.Runtime.InteropServices.GCHandleType)
    public static void Subject_15()
    {
        try { if (((GCHandle.Alloc(42, GCHandleType.Normal)).GetHashCode()) != ((GCHandle.Alloc(42, GCHandleType.Normal)).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [16] System.Runtime.InteropServices/System.Runtime.InteropServices.GCHandle::Equals:System.Boolean(System.Object)
    public static void Subject_16()
    {
        try { if (((default(GCHandle).Equals(42)) ? 1 : 0) != ((default(GCHandle).Equals(42)) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [17] System.Runtime.InteropServices/System.Runtime.InteropServices.GCHandle::Equals:System.Boolean(System.Runtime.InteropServices.GCHandle)
    public static void Subject_17()
    {
        try { if (((default(GCHandle).Equals(default)) ? 1 : 0) != ((default(GCHandle).Equals(default)) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [18] System.Runtime.InteropServices/System.Runtime.InteropServices.GCHandle::Free:System.Void()
    public static void Subject_18()
    {
        try { default(GCHandle).Free(); }
        catch { _exitCode = 1; }
    }

    // [19] System.Runtime.InteropServices/System.Runtime.InteropServices.GCHandle::FromIntPtr:System.Runtime.InteropServices.GCHandle(System.IntPtr)
    public static void Subject_19()
    {
        try { if (((GCHandle.FromIntPtr(IntPtr.Zero)).GetHashCode()) != ((GCHandle.FromIntPtr(IntPtr.Zero)).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [20] System.Runtime.InteropServices/System.Runtime.InteropServices.GCHandle::get_IsAllocated:System.Boolean()
    public static void Subject_20()
    {
        try { if (((default(GCHandle).IsAllocated) ? 1 : 0) != ((default(GCHandle).IsAllocated) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [21] System.Runtime.InteropServices/System.Runtime.InteropServices.GCHandle::get_Target:System.Object()
    public static void Subject_21()
    {
        try { if (((default(GCHandle).Target).GetHashCode()) != ((default(GCHandle).Target).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [22] System.Runtime.InteropServices/System.Runtime.InteropServices.GCHandle::GetHashCode:System.Int32()
    public static void Subject_22()
    {
        try { if (default(GCHandle).GetHashCode() != default(GCHandle).GetHashCode()) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [23] System.Runtime.InteropServices/System.Runtime.InteropServices.GCHandle::op_Equality:System.Boolean(System.Runtime.InteropServices.GCHandle,System.Runtime.InteropServices.GCHandle)
    public static void Subject_23()
    {
        try { if (((default(GCHandle) == default(GCHandle)) ? 1 : 0) != ((default(GCHandle) == default(GCHandle)) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [24] System.Runtime.InteropServices/System.Runtime.InteropServices.GCHandle::op_Explicit:System.Runtime.InteropServices.GCHandle(System.IntPtr)
    public static void Subject_24()
    {
        // TODO: System.Runtime.InteropServices/System.Runtime.InteropServices.GCHandle::op_Explicit:System.Runtime.InteropServices.GCHandle(System.IntPtr) could not be auto-generated
    }

    // [25] System.Runtime.InteropServices/System.Runtime.InteropServices.GCHandle::op_Explicit:System.IntPtr(System.Runtime.InteropServices.GCHandle)
    public static void Subject_25()
    {
        // TODO: System.Runtime.InteropServices/System.Runtime.InteropServices.GCHandle::op_Explicit:System.IntPtr(System.Runtime.InteropServices.GCHandle) could not be auto-generated
    }

    // [26] System.Runtime.InteropServices/System.Runtime.InteropServices.GCHandle::op_Inequality:System.Boolean(System.Runtime.InteropServices.GCHandle,System.Runtime.InteropServices.GCHandle)
    public static void Subject_26()
    {
        try { if (((default(GCHandle) != default(GCHandle)) ? 1 : 0) != ((default(GCHandle) != default(GCHandle)) ? 1 : 0)) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [27] System.Runtime.InteropServices/System.Runtime.InteropServices.GCHandle::set_Target:System.Void(System.Object)
    public static void Subject_27()
    {
        // TODO: System.Runtime.InteropServices/System.Runtime.InteropServices.GCHandle::set_Target:System.Void(System.Object) could not be auto-generated
    }

    // [28] System.Runtime.InteropServices/System.Runtime.InteropServices.GCHandle::ToIntPtr:System.IntPtr(System.Runtime.InteropServices.GCHandle)
    public static void Subject_28()
    {
        try { if ((int)(GCHandle.ToIntPtr(default(GCHandle))) != (int)(GCHandle.ToIntPtr(default(GCHandle)))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

}