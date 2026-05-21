// Auto-generated native-AOT entry point
// Family: family/System.IO.Pipelines/stream-adapters
// Assembly: System.IO.Pipelines
// Variant: benchmark

using System;
using System.Collections.Generic;
using System.IO;
using System.IO.Pipelines;
using System.Linq;
using System.Threading;
using System.Threading.Tasks;

public static partial class StreamAdaptersNativeEntry
{
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    // [0] System.IO.Pipelines/System.IO.Pipelines.StreamPipeExtensions::CopyToAsync:System.Threading.Tasks.Task(System.IO.Stream,System.IO.Pipelines.PipeWriter,System.Threading.CancellationToken)
    public static void Method0()
    {
        try { if (((StreamPipeExtensions.CopyToAsync(null!, null!, null!)).GetHashCode()) != ((StreamPipeExtensions.CopyToAsync(null!, null!, null!)).GetHashCode())) _exitCode = 1; }
        catch { _exitCode = 1; }
    }

    public static void Run(int entryIndex)
    {
        try
        {
            switch (entryIndex)
            {
                case 0: Method0(); break;
            }
        }
        catch
        {
            _exitCode = 1;
        }
    }

}