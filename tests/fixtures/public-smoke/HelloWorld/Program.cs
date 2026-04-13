using System.Collections.ObjectModel;

internal static class RegistrationTrace
{
    private static readonly List<string> Marks = new();

    public static void MarkEntry(string entryName)
    {
        Marks.Add($"register:{entryName}");
    }

    public static ReadOnlyCollection<string> Snapshot()
    {
        return Marks.AsReadOnly();
    }
}

internal static class Program
{
    private static int Main(string[] args)
    {
        RegistrationTrace.MarkEntry(nameof(Main));

        Console.WriteLine("HelloWorld smoke entry reached.");
        Console.WriteLine($"args={args.Length}");
        Console.WriteLine(string.Join(",", RegistrationTrace.Snapshot()));
        return 0;
    }
}
