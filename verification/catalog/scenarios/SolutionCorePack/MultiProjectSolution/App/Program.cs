using GoldenMultiProject.Middle;

namespace GoldenMultiProject.App;

internal static class Program
{
    private static int Main()
    {
        Console.WriteLine(MessageComposer.Compose());
        return 0;
    }
}
