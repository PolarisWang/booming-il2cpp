using System.Globalization;

namespace ChaosPublishSmoke;

internal static class Program
{
    private static int Main(string[] args)
    {
        Console.WriteLine("ChaosPublishSmoke entry reached.");
        Console.WriteLine($"args.Length={args.Length}");
        Console.WriteLine($"culture={CultureInfo.CurrentCulture.Name}");
        return 0;
    }
}
