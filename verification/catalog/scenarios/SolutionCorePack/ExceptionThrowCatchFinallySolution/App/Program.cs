using System;

namespace ExceptionThrowCatchFinally.App;

internal sealed class ExceptionThrower
{
    public ExceptionThrower()
    {
    }

    public string Capture()
    {
        try
        {
            ThrowNow();
            return "unreachable";
        }
        catch (InvalidOperationException ex)
        {
            _ = ex;
            return "Exception native proof: caught.";
        }
        finally
        {
        }
    }

    private static void ThrowNow()
    {
        throw new InvalidOperationException("phase6-proof");
    }
}

internal static class ExceptionProofEntry
{
    public static int Run()
    {
        var thrower = new ExceptionThrower();
        Console.WriteLine(thrower.Capture());
        return 0;
    }
}

internal static class Program
{
    private static int Main()
    {
        return ExceptionProofEntry.Run();
    }
}
