using GoldenSimpleLib.Library;

namespace GoldenSimpleLib.App;

internal static class Program
{
    private static int Main()
    {
        Console.WriteLine(Greeter.BuildMessage());
        return 0;
    }
}
