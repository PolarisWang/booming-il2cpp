// Auto-generated handwritten test stubs for pre-verification audit.
// TODO: Replace each stub body with a real test invocation.

using System.Collections.Generic;
using System.Runtime.InteropServices;
using System;
using Chaos.TestFramework;

public static partial class HandlesSafehandleGchandleSubjects
{
    // [0] System.Runtime.InteropServices/System.Runtime.InteropServices.SafeHandle::Close:System.Void()
    [Fact]
    public static void CustomEntrySubject_0()
    {        try { default(SafeHandle)!.Close(); } catch { }    }

    // [1] System.Runtime.InteropServices/System.Runtime.InteropServices.SafeHandle::DangerousAddRef:System.Void(System.Boolean&)
    [Fact]
    public static void CustomEntrySubject_1()
    {        try { default(SafeHandle)!.DangerousAddRef(out true); } catch { }    }

    // [2] System.Runtime.InteropServices/System.Runtime.InteropServices.SafeHandle::DangerousGetHandle:System.IntPtr()
    [Fact]
    public static void CustomEntrySubject_2()
    {        try { _ = default(SafeHandle)!.DangerousGetHandle(); } catch { }    }

    // [3] System.Runtime.InteropServices/System.Runtime.InteropServices.SafeHandle::DangerousRelease:System.Void()
    [Fact]
    public static void CustomEntrySubject_3()
    {        try { default(SafeHandle)!.DangerousRelease(); } catch { }    }

    // [4] System.Runtime.InteropServices/System.Runtime.InteropServices.SafeHandle::Dispose:System.Void()
    [Fact]
    public static void CustomEntrySubject_4()
    {        try { default(SafeHandle)!.Dispose(); } catch { }    }

    // [5] System.Runtime.InteropServices/System.Runtime.InteropServices.SafeHandle::get_IsClosed:System.Boolean()
    [Fact]
    public static void CustomEntrySubject_5()
    {        try { _ = default(SafeHandle)!.IsClosed; } catch { }    }

    // [6] System.Runtime.InteropServices/System.Runtime.InteropServices.SafeHandle::get_IsInvalid:System.Boolean()
    [Fact]
    public static void CustomEntrySubject_6()
    {        try { _ = default(SafeHandle)!.IsInvalid; } catch { }    }

    // [7] System.Runtime.InteropServices/System.Runtime.InteropServices.SafeHandle::SetHandleAsInvalid:System.Void()
    [Fact]
    public static void CustomEntrySubject_7()
    {        try { default(SafeHandle)!.SetHandleAsInvalid(); } catch { }    }

    // [8] System.Runtime.InteropServices/System.Runtime.InteropServices.CriticalHandle::Close:System.Void()
    [Fact]
    public static void CustomEntrySubject_8()
    {        try { default(CriticalHandle)!.Close(); } catch { }    }

    // [9] System.Runtime.InteropServices/System.Runtime.InteropServices.CriticalHandle::Dispose:System.Void()
    [Fact]
    public static void CustomEntrySubject_9()
    {        try { default(CriticalHandle)!.Dispose(); } catch { }    }

    // [10] System.Runtime.InteropServices/System.Runtime.InteropServices.CriticalHandle::get_IsClosed:System.Boolean()
    [Fact]
    public static void CustomEntrySubject_10()
    {        try { _ = default(CriticalHandle)!.IsClosed; } catch { }    }

    // [11] System.Runtime.InteropServices/System.Runtime.InteropServices.CriticalHandle::get_IsInvalid:System.Boolean()
    [Fact]
    public static void CustomEntrySubject_11()
    {        try { _ = default(CriticalHandle)!.IsInvalid; } catch { }    }

    // [12] System.Runtime.InteropServices/System.Runtime.InteropServices.CriticalHandle::SetHandleAsInvalid:System.Void()
    [Fact]
    public static void CustomEntrySubject_12()
    {        try { default(CriticalHandle)!.SetHandleAsInvalid(); } catch { }    }

    // [24] System.Runtime.InteropServices/System.Runtime.InteropServices.GCHandle::op_Explicit:System.Runtime.InteropServices.GCHandle(System.IntPtr)
    [Fact]
    public static void CustomEntrySubject_24()
    {        try { _ = default(GCHandle).op_Explicit(IntPtr.Zero); } catch { }    }

    // [25] System.Runtime.InteropServices/System.Runtime.InteropServices.GCHandle::op_Explicit:System.IntPtr(System.Runtime.InteropServices.GCHandle)
    [Fact]
    public static void CustomEntrySubject_25()
    {        try { _ = default(GCHandle).op_Explicit(default); } catch { }    }

    // [27] System.Runtime.InteropServices/System.Runtime.InteropServices.GCHandle::set_Target:System.Void(System.Object)
    [Fact]
    public static void CustomEntrySubject_27()
    {        try { default(GCHandle).Target = null!; } catch { }    }

}
