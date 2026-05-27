// Lightweight assertion helpers for foundation-dll Subject verification.
//
// Non-generic overloads only — ensures safe codegen without generic instantiation.
// Used by auto-generated Subject_N methods and handwritten CustomEntrySubject_N.
//
// On failure, throws AssertionException — the Subject_N try/catch wrapper
// (already present in all auto-generated subjects) catches it and sets
// _exitCode = 1.  This avoids a separate exit-code field that codegen would
// need to route to the native runner.
//
// Usage: subjects add `using Chaos.TestFramework;` and call directly:
//   Assert.AreEqual(42, result);
//   Assert.IsTrue(condition);
//   Assert.Throws(() => { risky_call(); });

namespace Chaos.TestFramework;

/// <summary>Thrown by Assert methods on failure. Caught by Subject_N try/catch
/// wrappers or by the managed proof runner.</summary>
internal class AssertionException : System.Exception
{
    public AssertionException(string message) : base(message) { }
}

public static class Assert
{
    public static void AreEqual(int expected, int actual, string? message = null)
    {
        if (expected != actual)
            Fail(message ?? $"Expected {expected}, got {actual}");
    }

    public static void AreEqual(uint expected, uint actual, string? message = null)
    {
        if (expected != actual)
            Fail(message ?? $"Expected {expected}, got {actual}");
    }

    public static void AreEqual(long expected, long actual, string? message = null)
    {
        if (expected != actual)
            Fail(message ?? $"Expected {expected}, got {actual}");
    }

    public static void AreEqual(ulong expected, ulong actual, string? message = null)
    {
        if (expected != actual)
            Fail(message ?? $"Expected {expected}, got {actual}");
    }

    public static void AreEqual(short expected, short actual, string? message = null)
    {
        if (expected != actual)
            Fail(message ?? $"Expected {expected}, got {actual}");
    }

    public static void AreEqual(ushort expected, ushort actual, string? message = null)
    {
        if (expected != actual)
            Fail(message ?? $"Expected {expected}, got {actual}");
    }

    public static void AreEqual(byte expected, byte actual, string? message = null)
    {
        if (expected != actual)
            Fail(message ?? $"Expected {expected}, got {actual}");
    }

    public static void AreEqual(sbyte expected, sbyte actual, string? message = null)
    {
        if (expected != actual)
            Fail(message ?? $"Expected {expected}, got {actual}");
    }

    public static void AreEqual(char expected, char actual, string? message = null)
    {
        if (expected != actual)
            Fail(message ?? $"Expected '{expected}', got '{actual}'");
    }

    public static void AreEqual(bool expected, bool actual, string? message = null)
    {
        if (expected != actual)
            Fail(message ?? $"Expected {expected}, got {actual}");
    }

    public static void AreEqual(float expected, float actual, string? message = null)
    {
        if (expected != actual)
            Fail(message ?? $"Expected {expected}, got {actual}");
    }

    public static void AreEqual(double expected, double actual, string? message = null)
    {
        if (expected != actual)
            Fail(message ?? $"Expected {expected}, got {actual}");
    }

    public static void AreEqual(decimal expected, decimal actual, string? message = null)
    {
        if (expected != actual)
            Fail(message ?? $"Expected {expected}, got {actual}");
    }

    public static void AreEqual(string expected, string actual, string? message = null)
    {
        if (expected != actual)
            Fail(message ?? $"Expected '{expected}', got '{actual}'");
    }

    public static void IsNull(object? value, string? message = null)
    {
        if (value != null)
            Fail(message ?? $"Expected null, got {value}");
    }

    public static void IsNotNull(object? value, string? message = null)
    {
        if (value == null)
            Fail(message ?? "Expected non-null");
    }

    public static void IsTrue(bool condition, string? message = null)
    {
        if (!condition)
            Fail(message ?? "Expected true");
    }

    public static void IsFalse(bool condition, string? message = null)
    {
        if (condition)
            Fail(message ?? "Expected false");
    }

    public static void Throws(System.Action action)
    {
        try { action(); Fail("Expected exception but none thrown"); }
        catch (AssertionException) { throw; }  // let assertion failures propagate
        catch { /* expected */ }
    }

    public static void Fail(string message)
    {
        System.Console.Error.WriteLine($"[ASSERT FAIL] {message}");
        throw new AssertionException(message);
    }
}
