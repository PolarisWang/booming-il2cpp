// Auto-generated handwritten test stubs for pre-verification audit.
// TODO: Replace each stub body with a real test invocation.

using System.Collections.Generic;
using System.Runtime.InteropServices;
using System;
using Chaos.TestFramework;

public static partial class PipeCoreSubjects
{
    // [0] System.IO.Pipelines/System.IO.Pipelines.IDuplexPipe::get_Input:System.IO.Pipelines.PipeReader()
    [Fact]
    public static void CustomEntrySubject_0()
    {        try { _ = default(IDuplexPipe)!.Input; } catch { }    }

    // [1] System.IO.Pipelines/System.IO.Pipelines.IDuplexPipe::get_Output:System.IO.Pipelines.PipeWriter()
    [Fact]
    public static void CustomEntrySubject_1()
    {        try { _ = default(IDuplexPipe)!.Output; } catch { }    }

}
