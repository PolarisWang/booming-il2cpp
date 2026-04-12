using System;

namespace CrossBoundaryExceptionProof;

/// Demonstrates exception propagation across AOT↔interpreter boundaries.
/// In AOT mode these are all native calls; in mixed mode some may run in the interpreter.
internal static class ProofMethods
{
    /// An AOT method that throws a known exception.
    public static void ThrowKnownException(string message)
    {
        throw new InvalidOperationException(message);
    }

    /// An AOT method that calls another method and catches exceptions.
    public static string CatchFromCallee(string tag)
    {
        try
        {
            ThrowKnownException("from-" + tag);
            return "unreachable";
        }
        catch (InvalidOperationException ex)
        {
            return "caught:" + ex.Message;
        }
    }

    /// AOT method with filter-like when() pattern.
    public static string CatchWithCondition(int value)
    {
        try
        {
            if (value < 0)
                throw new ArgumentOutOfRangeException(nameof(value), "negative");
            return "ok:" + value;
        }
        catch (ArgumentOutOfRangeException ex) when (ex.ParamName == "value")
        {
            return "filtered:" + ex.Message;
        }
    }

    /// Nested try/catch/finally.
    public static string NestedFinally(bool shouldThrow)
    {
        var log = "";
        try
        {
            try
            {
                log += "inner-try;";
                if (shouldThrow)
                    throw new Exception("nested");
                log += "no-throw;";
            }
            finally
            {
                log += "inner-finally;";
            }
        }
        catch (Exception ex)
        {
            log += "outer-catch:" + ex.Message + ";";
        }
        finally
        {
            log += "outer-finally;";
        }
        return log;
    }
}

internal static class CrossBoundaryExceptionProofEntry
{
    public static int Run()
    {
        // Scenario 1: simple throw and catch
        var r1 = ProofMethods.CatchFromCallee("aot");
        Console.WriteLine("cross-ex-catch=" + r1);

        // Scenario 2: filter-like catch when
        var r2a = ProofMethods.CatchWithCondition(5);
        Console.WriteLine("cross-ex-filter-ok=" + r2a);
        var r2b = ProofMethods.CatchWithCondition(-1);
        Console.WriteLine("cross-ex-filter-caught=" + r2b);

        // Scenario 3: nested finally (throw path)
        var r3a = ProofMethods.NestedFinally(true);
        Console.WriteLine("cross-ex-finally-throw=" + r3a);

        // Scenario 4: nested finally (no-throw path)
        var r3b = ProofMethods.NestedFinally(false);
        Console.WriteLine("cross-ex-finally-ok=" + r3b);

        return 0;
    }
}

internal static class Program
{
    public static int Main() => CrossBoundaryExceptionProofEntry.Run();
}
