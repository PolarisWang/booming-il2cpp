// Auto-generated handwritten test stubs for pre-verification audit.
// TODO: Replace each stub body with a real test invocation.

using System.Collections.Generic;
using System.Runtime.InteropServices;
using System;

public static partial class HandlesSafehandleGchandleSubjects
{
    // [0] System.Runtime.InteropServices/System.Runtime.InteropServices.SafeHandle::Close:System.Void()
    public static void CustomEntrySubject_0()
    {        try { default(SafeHandle)!.Close(); } catch { _exitCode = 1; }    }

    // [1] System.Runtime.InteropServices/System.Runtime.InteropServices.SafeHandle::DangerousAddRef:System.Void(System.Boolean&)
    public static void CustomEntrySubject_1()
    {        try { default(SafeHandle)!.DangerousAddRef(out true); } catch { _exitCode = 1; }    }

    // [2] System.Runtime.InteropServices/System.Runtime.InteropServices.SafeHandle::DangerousGetHandle:System.IntPtr()
    public static void CustomEntrySubject_2()
    {        try { _ = default(SafeHandle)!.DangerousGetHandle(); } catch { _exitCode = 1; }    }

    // [3] System.Runtime.InteropServices/System.Runtime.InteropServices.SafeHandle::DangerousRelease:System.Void()
    public static void CustomEntrySubject_3()
    {        try { default(SafeHandle)!.DangerousRelease(); } catch { _exitCode = 1; }    }

    // [4] System.Runtime.InteropServices/System.Runtime.InteropServices.SafeHandle::Dispose:System.Void()
    public static void CustomEntrySubject_4()
    {        try { default(SafeHandle)!.Dispose(); } catch { _exitCode = 1; }    }

    // [5] System.Runtime.InteropServices/System.Runtime.InteropServices.SafeHandle::get_IsClosed:System.Boolean()
    public static void CustomEntrySubject_5()
    {        try { _ = default(SafeHandle)!.IsClosed; } catch { _exitCode = 1; }    }

    // [6] System.Runtime.InteropServices/System.Runtime.InteropServices.SafeHandle::get_IsInvalid:System.Boolean()
    public static void CustomEntrySubject_6()
    {        try { _ = default(SafeHandle)!.IsInvalid; } catch { _exitCode = 1; }    }

    // [7] System.Runtime.InteropServices/System.Runtime.InteropServices.SafeHandle::SetHandleAsInvalid:System.Void()
    public static void CustomEntrySubject_7()
    {        try { default(SafeHandle)!.SetHandleAsInvalid(); } catch { _exitCode = 1; }    }

    // [8] System.Runtime.InteropServices/System.Runtime.InteropServices.CriticalHandle::Close:System.Void()
    public static void CustomEntrySubject_8()
    {        try { default(CriticalHandle)!.Close(); } catch { _exitCode = 1; }    }

    // [9] System.Runtime.InteropServices/System.Runtime.InteropServices.CriticalHandle::Dispose:System.Void()
    public static void CustomEntrySubject_9()
    {        try { default(CriticalHandle)!.Dispose(); } catch { _exitCode = 1; }    }

    // [10] System.Runtime.InteropServices/System.Runtime.InteropServices.CriticalHandle::get_IsClosed:System.Boolean()
    public static void CustomEntrySubject_10()
    {        try { _ = default(CriticalHandle)!.IsClosed; } catch { _exitCode = 1; }    }

    // [11] System.Runtime.InteropServices/System.Runtime.InteropServices.CriticalHandle::get_IsInvalid:System.Boolean()
    public static void CustomEntrySubject_11()
    {        try { _ = default(CriticalHandle)!.IsInvalid; } catch { _exitCode = 1; }    }

    // [12] System.Runtime.InteropServices/System.Runtime.InteropServices.CriticalHandle::SetHandleAsInvalid:System.Void()
    public static void CustomEntrySubject_12()
    {        try { default(CriticalHandle)!.SetHandleAsInvalid(); } catch { _exitCode = 1; }    }

    // [24] System.Runtime.InteropServices/System.Runtime.InteropServices.GCHandle::op_Explicit:System.Runtime.InteropServices.GCHandle(System.IntPtr)
    public static void CustomEntrySubject_24()
    {        try { _ = default(GCHandle).op_Explicit(IntPtr.Zero); } catch { _exitCode = 1; }    }

    // [25] System.Runtime.InteropServices/System.Runtime.InteropServices.GCHandle::op_Explicit:System.IntPtr(System.Runtime.InteropServices.GCHandle)
    public static void CustomEntrySubject_25()
    {        try { _ = default(GCHandle).op_Explicit(default); } catch { _exitCode = 1; }    }

    // [27] System.Runtime.InteropServices/System.Runtime.InteropServices.GCHandle::set_Target:System.Void(System.Object)
    public static void CustomEntrySubject_27()
    {        try { default(GCHandle).Target = null!; } catch { _exitCode = 1; }    }

}
