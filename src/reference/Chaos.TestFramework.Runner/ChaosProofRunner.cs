namespace Chaos.TestFramework.Runner;

/// <summary>
/// Proof runner: invokes each test method in sequence, reads ExitCode after each.
/// Equivalent to xUnit's TestClassRunner: Reset → Invoke → Read ExitCode → Accumulate.
/// </summary>
public static class ChaosProofRunner
{
    /// <summary>
    /// Run all proof methods and report Passed/Failed count.
    /// </summary>
    /// <param name="methods">Method dispatch table (Action[]).</param>
    /// <param name="methodCount">Number of methods to run.</param>
    /// <returns>Failure count (0 = all passed).</returns>
    public static int RunAll(Action[] methods, int methodCount)
    {
        int failures = 0;
        for (int i = 0; i < methodCount; i++)
        {
            ChaosAssertState.Reset();
            methods[i]();
            if (ChaosAssertState.ExitCode != 0)
            {
                Console.Error.WriteLine($"FAIL [{i}]: ExitCode={ChaosAssertState.ExitCode}");
                failures++;
            }
        }

        Console.WriteLine($"Passed: {methodCount - failures}/{methodCount}");
        return failures;
    }
}