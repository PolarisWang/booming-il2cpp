namespace Chaos.TestFramework;

using System.Diagnostics;

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

    [Conditional("VERIFY")]
    public static void AreEqual(int expected, int actual, string? message = null)
    {
        if (expected != actual)
            Fail(message ?? $"Expected {expected}, got {actual}");
    }

    [Conditional("VERIFY")]
    public static void AreEqual(uint expected, uint actual, string? message = null)
    {
        if (expected != actual)
            Fail(message ?? $"Expected {expected}, got {actual}");
    }

    [Conditional("VERIFY")]
    public static void AreEqual(long expected, long actual, string? message = null)
    {
        if (expected != actual)
            Fail(message ?? $"Expected {expected}, got {actual}");
    }

    [Conditional("VERIFY")]
    public static void AreEqual(ulong expected, ulong actual, string? message = null)
    {
        if (expected != actual)
            Fail(message ?? $"Expected {expected}, got {actual}");
    }

    [Conditional("VERIFY")]
    public static void AreEqual(short expected, short actual, string? message = null)
    {
        if (expected != actual)
            Fail(message ?? $"Expected {expected}, got {actual}");
    }

    [Conditional("VERIFY")]
    public static void AreEqual(ushort expected, ushort actual, string? message = null)
    {
        if (expected != actual)
            Fail(message ?? $"Expected {expected}, got {actual}");
    }

    [Conditional("VERIFY")]
    public static void AreEqual(byte expected, byte actual, string? message = null)
    {
        if (expected != actual)
            Fail(message ?? $"Expected {expected}, got {actual}");
    }

    [Conditional("VERIFY")]
    public static void AreEqual(sbyte expected, sbyte actual, string? message = null)
    {
        if (expected != actual)
            Fail(message ?? $"Expected {expected}, got {actual}");
    }

    [Conditional("VERIFY")]
    public static void AreEqual(char expected, char actual, string? message = null)
    {
        if (expected != actual)
            Fail(message ?? $"Expected '{expected}', got '{actual}'");
    }

    [Conditional("VERIFY")]
    public static void AreEqual(bool expected, bool actual, string? message = null)
    {
        if (expected != actual)
            Fail(message ?? $"Expected {expected}, got {actual}");
    }

    [Conditional("VERIFY")]
    public static void AreEqual(float expected, float actual, string? message = null)
    {
        if (expected != actual)
            Fail(message ?? $"Expected {expected}, got {actual}");
    }

    [Conditional("VERIFY")]
    public static void AreEqual(double expected, double actual, string? message = null)
    {
        if (expected != actual)
            Fail(message ?? $"Expected {expected}, got {actual}");
    }

    [Conditional("VERIFY")]
    public static void AreEqual(decimal expected, decimal actual, string? message = null)
    {
        if (expected != actual)
            Fail(message ?? $"Expected {expected}, got {actual}");
    }

    [Conditional("VERIFY")]
    public static void AreEqual(string expected, string actual, string? message = null)
    {
        if (expected != actual)
            Fail(message ?? $"Expected '{expected}', got '{actual}'");
    }

    [Conditional("VERIFY")]
    public static void IsNull(object? value, string? message = null)
    {
        if (value != null)
            Fail(message ?? $"Expected null, got {value}");
    }

    [Conditional("VERIFY")]
    public static void IsNotNull(object? value, string? message = null)
    {
        if (value == null)
            Fail(message ?? "Expected non-null");
    }

    [Conditional("VERIFY")]
    public static void IsTrue(bool condition, string? message = null)
    {
        if (!condition)
            Fail(message ?? "Expected true");
    }

    [Conditional("VERIFY")]
    public static void IsFalse(bool condition, string? message = null)
    {
        if (condition)
            Fail(message ?? "Expected false");
    }

    [Conditional("VERIFY")]
    public static void Throws(System.Action action)
    {
        try { action(); Fail("Expected exception but none thrown"); }
        catch (AssertionException) { throw; }
        catch { }
    }

    [Conditional("VERIFY")]
    public static void Throws<T>(System.Action action) where T : System.Exception
    {
        try { action(); Fail($"Expected {typeof(T).Name} but none thrown"); }
        catch (AssertionException) { throw; }
        catch (T) { }
        catch (System.Exception ex) { Fail($"Expected {typeof(T).Name} but got {ex.GetType().Name}: {ex.Message}"); }
    }

    [Conditional("VERIFY")]
    public static void AreEqual<T>(T expected, T actual, string? message = null)
    {
        // Null handling
        if (expected is null && actual is null) return;
        if (expected is null || actual is null)
        {
            Fail(message ?? $"Expected [{typeof(T).Name}]{expected}, got [{typeof(T).Name}]{actual}");
            return;
        }

        // Runtime collection structural comparison (List<T>, arrays, HashSet<T>, etc.)
        // Note: we cannot use IEnumerable<T> overloads — C# resolves the generic T=T overload
        // over IEnumerable<T> for concrete types like List<int>, so runtime detection is needed.
        // Exclude string (which implements IEnumerable<char>) to keep clear string-level error messages.
        if (expected is not string &&
            actual is not string &&
            expected is System.Collections.IEnumerable expectedEnum &&
            actual is System.Collections.IEnumerable actualEnum)
        {
            var eIter = expectedEnum.GetEnumerator();
            var aIter = actualEnum.GetEnumerator();
            using (eIter as IDisposable)
            using (aIter as IDisposable)
            {
                int index = 0;
                while (eIter.MoveNext())
                {
                    if (!aIter.MoveNext())
                    {
                        Fail(message ?? $"Expected longer collection at index {index}");
                        return;
                    }
                    if (!System.Collections.Generic.EqualityComparer<object?>.Default.Equals(eIter.Current, aIter.Current))
                    {
                        Fail(message ?? $"Element mismatch at index {index}: expected [{eIter.Current?.GetType().Name}]{eIter.Current}, got [{aIter.Current?.GetType().Name}]{aIter.Current}");
                        return;
                    }
                    index++;
                }
                if (aIter.MoveNext())
                {
                    Fail(message ?? $"Expected shorter collection — extra element at index {index}");
                }
            }
            return;
        }

        // Default equality (value types, strings, IEquatable<T> types)
        if (!System.Collections.Generic.EqualityComparer<T>.Default.Equals(expected, actual))
            Fail(message ?? $"Expected [{typeof(T).Name}]{expected}, got [{typeof(T).Name}]{actual}");
    }

    [Conditional("VERIFY")]
    public static void AreEqual(byte[] expected, byte[] actual, string? message = null)
    {
        if ((expected is null) != (actual is null)) { Fail(message ?? "Expected != actual (null mismatch)"); return; }
        if (expected is null) return;
        if (expected.Length != actual!.Length) { Fail(message ?? $"Expected length {expected.Length}, got {actual.Length}"); return; }
        for (int i = 0; i < expected.Length; i++)
            if (expected[i] != actual[i])
                Fail(message ?? $"Byte mismatch at {i}: expected {expected[i]}, got {actual[i]}");
    }

    [Conditional("VERIFY")]
    public static void AreNotEqual<T>(T expected, T actual, string? message = null)
    {
        if (System.Collections.Generic.EqualityComparer<T>.Default.Equals(expected, actual))
            Fail(message ?? $"Expected not-equal but both are [{typeof(T).Name}]{expected}");
    }

    [Conditional("VERIFY")]
    public static void Fail(string message)
    {
        s_exitCode = 1;
        System.Console.Error.WriteLine($"[ASSERT FAIL] {message}");
        throw new AssertionException(message);
    }
}
