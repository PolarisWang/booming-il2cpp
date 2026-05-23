// Auto-generated native-AOT entry point
// Family: family/System.Runtime.InteropServices/objective/c/interop
// Assembly: System.Runtime.InteropServices
// Variant: subjects

using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Runtime.InteropServices.ObjectiveC;

public static partial class ObjectiveCInteropSubjects
{
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    // [0] System.Runtime.InteropServices/System.Runtime.InteropServices.ObjectiveC.ObjectiveCMarshal::CreateReferenceTrackingHandle:System.Runtime.InteropServices.GCHandle(System.Object,System.Span{System.IntPtr}&)
    public static void Subject_0()
    {
            System.Span<System.IntPtr> refLocal_1 = default;
        try { if (((ObjectiveCMarshal.CreateReferenceTrackingHandle(null!, out refLocal_1)).GetHashCode()) != ((ObjectiveCMarshal.CreateReferenceTrackingHandle(null!, out refLocal_1)).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [1] System.Runtime.InteropServices/System.Runtime.InteropServices.ObjectiveC.ObjectiveCMarshal::Initialize:System.Void(System.Runtime.InteropServices.ObjectiveC.ObjectiveCMarshal+UnhandledExceptionPropagationHandler)
    public static void Subject_1()
    {
        // needs-manual — Initialize with 1 params requires manual implementation: System.Runtime.InteropServices/System.Runtime.InteropServices.ObjectiveC.ObjectiveCMarshal::Initialize:System.Void(System.Runtime.InteropServices.ObjectiveC.ObjectiveCMarshal+UnhandledExceptionPropagationHandler)
    }

    // [2] System.Runtime.InteropServices/System.Runtime.InteropServices.ObjectiveC.ObjectiveCMarshal::SetMessageSendCallback:System.Void(System.Runtime.InteropServices.ObjectiveC.ObjectiveCMarshal+MessageSendFunction,System.IntPtr)
    public static void Subject_2()
    {
        try { ObjectiveCMarshal.SetMessageSendCallback(default, IntPtr.Zero); }
        catch { _exitCode = 1; }
    }

    // [3] System.Runtime.InteropServices/System.Runtime.InteropServices.ObjectiveC.ObjectiveCMarshal::SetMessageSendPendingException:System.Void(System.Exception)
    public static void Subject_3()
    {
        try { ObjectiveCMarshal.SetMessageSendPendingException(default); }
        catch { _exitCode = 1; }
    }

}