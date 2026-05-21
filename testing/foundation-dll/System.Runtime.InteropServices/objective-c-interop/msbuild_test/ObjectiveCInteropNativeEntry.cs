// Auto-generated native-AOT entry point
// Family: family/System.Runtime.InteropServices/objective-c-interop
// Assembly: System.Runtime.InteropServices
// Variant: benchmark

using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Runtime.InteropServices.ObjectiveC;

public static partial class ObjectiveCInteropNativeEntry
{
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    // [0] System.Runtime.InteropServices/System.Runtime.InteropServices.ObjectiveC.ObjectiveCMarshal::CreateReferenceTrackingHandle:System.Runtime.InteropServices.GCHandle(System.Object,System.Span{System.IntPtr}&)
    public static void Method0()
    {
    var refLocal_1 = null!;
        try { if ((int)(ObjectiveCMarshal.CreateReferenceTrackingHandle(42, ref refLocal_1)) != (int)(ObjectiveCMarshal.CreateReferenceTrackingHandle(42, ref refLocal_1))) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    // [1] System.Runtime.InteropServices/System.Runtime.InteropServices.ObjectiveC.ObjectiveCMarshal::Initialize:System.Void(System.Runtime.InteropServices.ObjectiveC.ObjectiveCMarshal+UnhandledExceptionPropagationHandler)
    public static void Method1()
    {
        try { ObjectiveCMarshal.Initialize(null!); }
        catch { _exitCode = 1; }
    }

    // [2] System.Runtime.InteropServices/System.Runtime.InteropServices.ObjectiveC.ObjectiveCMarshal::SetMessageSendCallback:System.Void(System.Runtime.InteropServices.ObjectiveC.ObjectiveCMarshal+MessageSendFunction,System.IntPtr)
    public static void Method2()
    {
        try { ObjectiveCMarshal.SetMessageSendCallback(null!, IntPtr.Zero); }
        catch { _exitCode = 1; }
    }

    // [3] System.Runtime.InteropServices/System.Runtime.InteropServices.ObjectiveC.ObjectiveCMarshal::SetMessageSendPendingException:System.Void(System.Exception)
    public static void Method3()
    {
        try { ObjectiveCMarshal.SetMessageSendPendingException(null!); }
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
            }
        }
        catch
        {
            _exitCode = 1;
        }
    }

}