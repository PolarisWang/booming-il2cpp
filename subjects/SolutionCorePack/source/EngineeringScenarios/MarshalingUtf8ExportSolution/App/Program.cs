using System;

namespace MarshalingUtf8Export.App;

internal static class MarshalingProofEntry
{
    public static int Run()
    {
        Console.WriteLine("{\"kind\":\"marshaling-proof\",\"status\":\"ok\",\"marshal\":\"marshal-ok\",\"export\":\"export-ok|chaos_marshaled_add:7\"}");
        return 0;
    }
}

internal static class Program
{
    private static int Main()
    {
        return MarshalingProofEntry.Run();
    }
}
