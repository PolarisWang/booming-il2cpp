// Auto-generated native-AOT entry point
// Family: family/System.Private.CoreLib/stream/adapters
// Assembly: System.Private.CoreLib
// Variant: patch

public static partial class StreamAdaptersPatchEntry
{
    // Inlined exit code — avoids SDK method call resolution in codegen
    public static int _exitCode;

    // [0] System.IO.Pipelines/System.IO.Pipelines.StreamPipeExtensions::CopyToAsync:System.Threading.Tasks.Task(System.IO.Stream,System.IO.Pipelines.PipeWriter,System.Threading.CancellationToken)
    public static int Subject_0()
    {
        return unchecked((int)(0xB0000000u + 0));
    }

    public static void Run(int entryIndex)
    {
        try
        {
            switch (entryIndex)
            {
                case 0: Subject_0(); break;
            }
        }
        catch
        {
            _exitCode = 1;
        }
    }

}