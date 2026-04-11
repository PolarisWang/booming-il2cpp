using Newtonsoft.Json;

namespace GoldenWithPackage.App;

internal static class Program
{
    private static int Main()
    {
        var json = JsonConvert.SerializeObject("phase1");
        Console.WriteLine(json);
        return 0;
    }
}
