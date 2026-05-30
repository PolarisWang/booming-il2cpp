// Auto-generated handwritten test stubs for pre-verification audit.
// TODO: Replace each stub body with a real test invocation.

using System.Collections.Generic;
using System.Runtime.InteropServices;
using System;

public static partial class PipeCoreSubjects
{
    // [0] System.IO.Pipelines/System.IO.Pipelines.IDuplexPipe::get_Input:System.IO.Pipelines.PipeReader()
    public static void CustomEntrySubject_0()
    {        try { _ = default(IDuplexPipe)!.Input; } catch { _exitCode = 1; }    }

    // [1] System.IO.Pipelines/System.IO.Pipelines.IDuplexPipe::get_Output:System.IO.Pipelines.PipeWriter()
    public static void CustomEntrySubject_1()
    {        try { _ = default(IDuplexPipe)!.Output; } catch { _exitCode = 1; }    }

}
