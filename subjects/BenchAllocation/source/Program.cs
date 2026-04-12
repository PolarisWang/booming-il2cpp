namespace BenchAllocation;

internal sealed class WorkItem
{
    public int Id;
    public string? Name;
    public double Value;
}

internal static class Program
{
    private const int IterationCount = 1000;
    private static long s_lastChecksum;

    public static int Main()
    {
        s_lastChecksum = RunWorkload();
        return 0;
    }

    public static long RunWorkload()
    {
        long checksum = 0;
        for (int i = 0; i < IterationCount * 10; i++)
        {
            var item = new WorkItem { Id = i, Name = "item" + i, Value = i * 1.5 };
            checksum += item.Id;
        }

        return checksum % 10000;
    }
}
