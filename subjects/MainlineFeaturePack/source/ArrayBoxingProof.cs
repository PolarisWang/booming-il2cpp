namespace MainlineFeaturePack;

internal static class BoxingSink
{
    public static void Consume(object value)
    {
        _ = value;
    }
}

internal static class ArrayBoxingProofEntry
{
    public static int Run()
    {
        BoxingSink.Consume(42);
        Console.WriteLine(new FeatureBanner[] { new("array boxing proof") }[0].BuildMessage());
        return 0;
    }
}
