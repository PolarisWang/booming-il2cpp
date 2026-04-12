using Chaos.IL2CPP.HotUpdate;

namespace BenchHotUpdateDispatch;

internal static class Program
{
    private const string SubjectId = "BenchHotUpdateDispatch/HotPatch::AddOne(System.Int32)";
    private const int IterationCount = 1000;
    private static int s_lastChecksum;

    public static int Main()
    {
        s_lastChecksum = RunWorkload();
        return 0;
    }

    public static int RunWorkload()
    {
        var runtimeManager = new RuntimeManager();
        runtimeManager.RegisterInt32Unary(SubjectId, static value => value + 1);

        var checksum = 0;
        for (var index = 0; index < IterationCount; index++)
        {
            checksum += runtimeManager.DispatchInt32Unary(SubjectId, index, static value => value);
        }

        return checksum;
    }
}
