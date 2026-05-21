using Xunit.Sdk;

namespace Chaos.IL2CPP.Generator.Tests.Infra;

/// <summary>
/// Custom assertion helpers for verifying generated C++ code patterns.
/// </summary>
internal static class AssertExtensions
{
    /// <summary>
    /// Asserts the generated C++ source contains the expected substring.
    /// </summary>
    public static void ContainsCode(string expectedSubstring, string generatedSource)
    {
        if (!generatedSource.Contains(expectedSubstring, StringComparison.Ordinal))
        {
            throw new XunitException(
                $"Expected generated code to contain:\n  {expectedSubstring}\n" +
                $"But it was not found.\n\n--- Generated source ---\n{generatedSource}");
        }
    }

    /// <summary>
    /// Asserts the generated C++ source does NOT contain the specified substring.
    /// </summary>
    public static void DoesNotContainCode(string unexpectedSubstring, string generatedSource)
    {
        if (generatedSource.Contains(unexpectedSubstring, StringComparison.Ordinal))
        {
            throw new XunitException(
                $"Expected generated code to NOT contain:\n  {unexpectedSubstring}\n" +
                $"But it was found.\n\n--- Generated source ---\n{generatedSource}");
        }
    }

    /// <summary>
    /// Asserts the generated C++ source matches the given regex pattern.
    /// </summary>
    public static void MatchesCode(System.Text.RegularExpressions.Regex pattern, string generatedSource)
    {
        var match = pattern.Match(generatedSource);
        if (!match.Success)
        {
            throw new XunitException(
                $"Expected generated code to match pattern:\n  {pattern}\n" +
                $"But no match was found.\n\n--- Generated source ---\n{generatedSource}");
        }
    }

    /// <summary>
    /// Asserts that the generated method body uses structured IR
    /// (i.e. contains `_s0{ }` or similar structured variable declarations)
    /// rather than eval-stack-based flat fallback.
    /// </summary>
    public static void UsesStructuredIR(string generatedSource)
    {
        if (!generatedSource.Contains("_s0{}", StringComparison.Ordinal) &&
            !generatedSource.Contains("_s0 {", StringComparison.Ordinal) &&
            !generatedSource.Contains("_s0(", StringComparison.Ordinal))
        {
            throw new XunitException(
                "Expected method to use structured IR (contain _s0), " +
                "but it appears to be flat fallback.\n\n--- Generated source ---\n" +
                generatedSource);
        }
    }

    /// <summary>
    /// Asserts the generated code for a method has a try-catch shape
    /// (structured exception handling with C++ try/catch keywords).
    /// </summary>
    public static void HasTryCatch(string generatedSource)
    {
        if (!generatedSource.Contains("try", StringComparison.Ordinal) ||
            !generatedSource.Contains("catch", StringComparison.Ordinal))
        {
            throw new XunitException(
                "Expected method to contain try/catch for structured EH, " +
                "but no try or catch keyword was found.\n\n--- Generated source ---\n" +
                generatedSource);
        }
    }

    /// <summary>
    /// Asserts the generated code uses DirectNativeSymbol call instead of
    /// external runtime table dispatch. A direct call has the form:
    ///   ChaosFunctionName(args);
    /// rather than a table lookup.
    /// </summary>
    public static void UsesDirectNativeCall(string directNativeSymbol, string generatedSource)
    {
        ContainsCode(directNativeSymbol, generatedSource);

        // A direct call should NOT contain chaos_external_runtime_ dispatch patterns
        // for this symbol. (It may still contain other extern calls.)
    }

    /// <summary>
    /// Asserts that the code uses external runtime dispatch table for a call
    /// (i.e. calls through kChaosExternalRuntimeFnTable[idx]).
    /// </summary>
    public static void UsesExternalDispatchTable(string generatedSource)
    {
        ContainsCode("kChaosExternalRuntimeFnTable", generatedSource);
    }
}
