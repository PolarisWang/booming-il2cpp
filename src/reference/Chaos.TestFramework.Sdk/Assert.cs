namespace Chaos.TestFramework;

internal class AssertionException : System.Exception
{
    public AssertionException(string message) : base(message) { }
}

public static class Assert
{
    [ThreadStatic] private static int s_exitCode;

    internal static void Reset() => s_exitCode = 0;
    internal static void RecordFailure() => s_exitCode = 1;
    internal static int Complete() { int c = s_exitCode; s_exitCode = 0; return c; }
    internal static int ExitCode => s_exitCode;

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
        catch (AssertionException) { throw; }
        catch { }
    }

    public static void Throws<T>(System.Action action) where T : System.Exception
    {
        try { action(); Fail($"Expected {typeof(T).Name} but none thrown"); }
        catch (AssertionException) { throw; }
        catch (T) { }
        catch (System.Exception ex) { Fail($"Expected {typeof(T).Name} but got {ex.GetType().Name}: {ex.Message}"); }
    }

    public static void Fail(string message)
    {
        s_exitCode = 1;
        System.Console.Error.WriteLine($"[ASSERT FAIL] {message}");
        throw new AssertionException(message);
    }
}
