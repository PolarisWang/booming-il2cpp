using Chaos.IL2CPP.HotUpdate;

namespace BenchHotUpdateLoad;

internal static class Program
{
    private const string CurrentAotVersion = "1.0.0";
    private const string SubjectId = "BenchHotUpdateLoad/HotPatch::GetValue()";
    private static readonly string s_packageRoot = Path.Combine(AppContext.BaseDirectory, "package");
    private static readonly IReadOnlyDictionary<string, int> s_subjectIdToConstantInt32 =
        new Dictionary<string, int>(StringComparer.Ordinal)
        {
            [SubjectId] = 1,
        };
    private static int s_lastLoadStatus;

    public static int Main()
    {
        s_lastLoadStatus = RunWorkload();
        return 0;
    }

    public static int RunWorkload()
    {
        var runtimeManager = new RuntimeManager();
        var loaded = runtimeManager.LoadPackage(
            s_packageRoot,
            CurrentAotVersion,
            subjectIdToConstantInt32: s_subjectIdToConstantInt32);
        return loaded ? 1 : 0;
    }
}
