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
        try { default(SafeHandle)!.Close(); _exitCode = 1; }
        catch (NullReferenceException) { }
    }

    // [1] System.Runtime.InteropServices/System.Runtime.InteropServices.SafeHandle::DangerousAddRef:System.Void(System.Boolean&)
    public static void Subject_1()
    {
        // TODO: System.Runtime.InteropServices/System.Runtime.InteropServices.SafeHandle::DangerousAddRef:System.Void(System.Boolean&) could not be auto-generated
    }

    // [2] System.Runtime.InteropServices/System.Runtime.InteropServices.SafeHandle::DangerousGetHandle:System.IntPtr()
    public static void Subject_2()
    {
        try { default(SafeHandle)!.DangerousGetHandle(); _exitCode = 1; }
        catch (NullReferenceException) { }
    }

    // [3] System.Runtime.InteropServices/System.Runtime.InteropServices.SafeHandle::DangerousRelease:System.Void()
    public static void Subject_3()
    {
        try { default(SafeHandle)!.DangerousRelease(); _exitCode = 1; }
        catch (NullReferenceException) { }
    }

    // [4] System.Runtime.InteropServices/System.Runtime.InteropServices.SafeHandle::Dispose:System.Void()
    public static void Subject_4()
    {
        try { default(SafeHandle)!.Dispose(); _exitCode = 1; }
        catch (NullReferenceException) { }
    }

    // [5] System.Runtime.InteropServices/System.Runtime.InteropServices.SafeHandle::get_IsClosed:System.Boolean()
    public static void Subject_5()
    {
        try { _ = default(SafeHandle)!.IsClosed; _exitCode = 1; }
        catch (NullReferenceException) { }
    }

    // [6] System.Runtime.InteropServices/System.Runtime.InteropServices.SafeHandle::get_IsInvalid:System.Boolean()
    public static void Subject_6()
    {
        try { _ = default(SafeHandle)!.IsInvalid; _exitCode = 1; }
        catch (NullReferenceException) { }
    }

    // [7] System.Runtime.InteropServices/System.Runtime.InteropServices.SafeHandle::SetHandleAsInvalid:System.Void()
    public static void Subject_7()
    {
        try { default(SafeHandle)!.SetHandleAsInvalid(); _exitCode = 1; }
        catch (NullReferenceException) { }
    }

    // [8] System.Runtime.InteropServices/System.Runtime.InteropServices.CriticalHandle::Close:System.Void()
    public static void Subject_8()
    {
        try { default(CriticalHandle)!.Close(); _exitCode = 1; }
        catch (NullReferenceException) { }
    }

    // [9] System.Runtime.InteropServices/System.Runtime.InteropServices.CriticalHandle::Dispose:System.Void()
    public static void Subject_9()
    {
        try { default(CriticalHandle)!.Dispose(); _exitCode = 1; }
        catch (NullReferenceException) { }
    }

    // [10] System.Runtime.InteropServices/System.Runtime.InteropServices.CriticalHandle::get_IsClosed:System.Boolean()
    public static void Subject_10()
    {
        try { _ = default(CriticalHandle)!.IsClosed; _exitCode = 1; }
        catch (NullReferenceException) { }
    }

    // [11] System.Runtime.InteropServices/System.Runtime.InteropServices.CriticalHandle::get_IsInvalid:System.Boolean()
    public static void Subject_11()
    {
        try { _ = default(CriticalHandle)!.IsInvalid; _exitCode = 1; }
        catch (NullReferenceException) { }
    }

    // [12] System.Runtime.InteropServices/System.Runtime.InteropServices.CriticalHandle::SetHandleAsInvalid:System.Void()
    public static void Subject_12()
    {
        try { default(CriticalHandle)!.SetHandleAsInvalid(); _exitCode = 1; }
        catch (NullReferenceException) { }
    }

    // [13] System.Runtime.InteropServices/System.Runtime.InteropServices.GCHandle::AddrOfPinnedObject:System.IntPtr()
    public static void Subject_13()
    {
        try { default(GCHandle).AddrOfPinnedObject(); _exitCode = 1; }
        catch (InvalidOperationException) { }
    }

    // [14] System.Runtime.InteropServices/System.Runtime.InteropServices.GCHandle::Alloc:System.Runtime.InteropServices.GCHandle(System.Object)
    public static void Subject_14()
    {
        if (((GCHandle.Alloc(42)).GetHashCode()) != 2130579834) _exitCode = 1;
    }

    // [15] System.Runtime.InteropServices/System.Runtime.InteropServices.GCHandle::Alloc:System.Runtime.InteropServices.GCHandle(System.Object,System.Runtime.InteropServices.GCHandleType)
    public static void Subject_15()
    {
        if (((GCHandle.Alloc(42, GCHandleType.Normal)).GetHashCode()) != 2130579722) _exitCode = 1;
    }

    // [16] System.Runtime.InteropServices/System.Runtime.InteropServices.GCHandle::Equals:System.Boolean(System.Object)
    public static void Subject_16()
    {
        if (((default(GCHandle).Equals(42)) ? 1 : 0) != 0) _exitCode = 1;
    }

    // [17] System.Runtime.InteropServices/System.Runtime.InteropServices.GCHandle::Equals:System.Boolean(System.Runtime.InteropServices.GCHandle)
    public static void Subject_17()
    {
        if (((default(GCHandle).Equals(default)) ? 1 : 0) != 1) _exitCode = 1;
    }

    // [18] System.Runtime.InteropServices/System.Runtime.InteropServices.GCHandle::Free:System.Void()
    public static void Subject_18()
    {
        try { default(GCHandle).Free(); _exitCode = 1; }
        catch (InvalidOperationException) { }
    }

    // [19] System.Runtime.InteropServices/System.Runtime.InteropServices.GCHandle::FromIntPtr:System.Runtime.InteropServices.GCHandle(System.IntPtr)
    public static void Subject_19()
    {
        try { GCHandle.FromIntPtr(IntPtr.Zero); _exitCode = 1; }
        catch (InvalidOperationException) { }
    }

    // [20] System.Runtime.InteropServices/System.Runtime.InteropServices.GCHandle::get_IsAllocated:System.Boolean()
    public static void Subject_20()
    {
        if (((default(GCHandle).IsAllocated) ? 1 : 0) != 0) _exitCode = 1;
    }

    // [21] System.Runtime.InteropServices/System.Runtime.InteropServices.GCHandle::get_Target:System.Object()
    public static void Subject_21()
    {
        try { _ = default(GCHandle).Target; _exitCode = 1; }
        catch (InvalidOperationException) { }
    }

    // [22] System.Runtime.InteropServices/System.Runtime.InteropServices.GCHandle::GetHashCode:System.Int32()
    public static void Subject_22()
    {
        if (default(GCHandle).GetHashCode() != 0) _exitCode = 1;
    }

    // [23] System.Runtime.InteropServices/System.Runtime.InteropServices.GCHandle::op_Equality:System.Boolean(System.Runtime.InteropServices.GCHandle,System.Runtime.InteropServices.GCHandle)
    public static void Subject_23()
    {
        if (((default(GCHandle) == default(GCHandle)) ? 1 : 0) != 1) _exitCode = 1;
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
        if (((default(GCHandle) != default(GCHandle)) ? 1 : 0) != 0) _exitCode = 1;
    }

    // [27] System.Runtime.InteropServices/System.Runtime.InteropServices.GCHandle::set_Target:System.Void(System.Object)
    public static void Subject_27()
    {
        // TODO: System.Runtime.InteropServices/System.Runtime.InteropServices.GCHandle::set_Target:System.Void(System.Object) could not be auto-generated
    }

    // [28] System.Runtime.InteropServices/System.Runtime.InteropServices.GCHandle::ToIntPtr:System.IntPtr(System.Runtime.InteropServices.GCHandle)
    public static void Subject_28()
    {
        if ((int)(GCHandle.ToIntPtr(default(GCHandle))) != 0) _exitCode = 1;
    }

}