using ReferenceGreeter;

namespace GoldenReferenceAssembly.App;

internal static class Program
{
    private static int Main()
    {
        var message = Greeter.Compose("reference-assembly");
        Console.WriteLine(message);
        return string.Equals(message, "hello-reference-assembly", StringComparison.Ordinal) ? 0 : 1;
    }
}
