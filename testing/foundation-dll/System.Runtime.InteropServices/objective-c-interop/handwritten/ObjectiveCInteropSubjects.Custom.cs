// Auto-generated handwritten test stubs for pre-verification audit.
// TODO: Replace each stub body with a real test invocation.

using System.Collections.Generic;
using System.Runtime.InteropServices;
using System;
using Chaos.TestFramework;

public static partial class ObjectiveCInteropSubjects
{
    // [1] System.Runtime.InteropServices/System.Runtime.InteropServices.ObjectiveC.ObjectiveCMarshal::Initialize:System.Void(System.Runtime.InteropServices.ObjectiveC.ObjectiveCMarshal+UnhandledExceptionPropagationHandler)
    public static void CustomEntrySubject_1()
    {
        try {
            ObjectiveCMarshal.Initialize(default);
        }
        catch { _exitCode = 1; }
    }

}
