// Auto-generated handwritten test stubs for pre-verification audit.
// TODO: Replace each stub body with a real test invocation.

using System.Collections.Generic;
using System.Runtime.InteropServices;
using System;
using Chaos.TestFramework;

public static partial class HandlesSafehandleGchandleSubjects
{
    // [1] System.Runtime.InteropServices/System.Runtime.InteropServices.SafeHandle::DangerousAddRef:System.Void(System.Boolean&)
    public static void CustomEntrySubject_1()
    {
        try {
            default(SafeHandle)!.DangerousAddRef(out true);
        }
        catch { _exitCode = 1; }
    }

    // [24] System.Runtime.InteropServices/System.Runtime.InteropServices.GCHandle::op_Explicit:System.Runtime.InteropServices.GCHandle(System.IntPtr)
    public static void CustomEntrySubject_24()
    {
        try {
            Assert.IsNotNull(default(GCHandle).op_Explicit(IntPtr.Zero));
        }
        catch { _exitCode = 1; }
    }

    // [25] System.Runtime.InteropServices/System.Runtime.InteropServices.GCHandle::op_Explicit:System.IntPtr(System.Runtime.InteropServices.GCHandle)
    public static void CustomEntrySubject_25()
    {
        try {
            Assert.IsNotNull(default(GCHandle).op_Explicit(default));
        }
        catch { _exitCode = 1; }
    }

    // [27] System.Runtime.InteropServices/System.Runtime.InteropServices.GCHandle::set_Target:System.Void(System.Object)
    public static void CustomEntrySubject_27()
    {
        try {
            default(GCHandle).Target = null!;
        }
        catch { _exitCode = 1; }
    }

}
