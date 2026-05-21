// Auto-generated native-AOT entry point
// Family: family/System.IO.Pipelines/stream/adapters
// Assembly: System.IO.Pipelines
// Variant: subjects

using System;
using System.Collections.Generic;
using System.IO;
using System.IO.Pipelines;
using System.Linq;
using System.Threading;
using System.Threading.Tasks;

public static partial class StreamAdaptersSubjects
{
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    // [0] System.IO.Pipelines/System.IO.Pipelines.StreamPipeExtensions::CopyToAsync:System.Threading.Tasks.Task(System.IO.Stream,System.IO.Pipelines.PipeWriter,System.Threading.CancellationToken)
    public static void Subject_0()
    {
        try { StreamPipeExtensions.CopyToAsync(new MemoryStream(), default, default); _exitCode = 1; }
        catch (ArgumentNullException) { }
    }

}