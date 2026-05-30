// Auto-generated handwritten test stubs for pre-verification audit.
// TODO: Replace each stub body with a real test invocation.

using System.Collections.Generic;
using System.Runtime.InteropServices;
using System;

public static partial class StreamAdaptersSubjects
{
    // [0] System.IO.Pipelines/System.IO.Pipelines.StreamPipeExtensions::CopyToAsync:System.Threading.Tasks.Task(System.IO.Stream,System.IO.Pipelines.PipeWriter,System.Threading.CancellationToken)
    public static void CustomEntrySubject_0()
    {        try { _ = StreamPipeExtensions.CopyToAsync(new MemoryStream(), default, default); } catch { _exitCode = 1; }    }

}
