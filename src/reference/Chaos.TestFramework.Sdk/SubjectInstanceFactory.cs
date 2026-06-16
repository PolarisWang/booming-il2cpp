namespace Chaos.TestFramework;

using System.Runtime.CompilerServices;

/// <summary>
/// Creates subject instances for AutoTestGenerator-generated tests.
/// Replaces the default(T)! pattern that produced NullReferenceException
/// for reference types, making benchmark/fact methods executable.
/// </summary>
public static class SubjectInstanceFactory
{
    /// <summary>
    /// Create a subject instance for testing.
    /// For value types: returns default(T) (zero-initialized, valid).
    /// For reference types: returns an uninitialized instance (no constructor called).
    /// </summary>
    public static T Create<T>()
    {
        if (typeof(T).IsValueType)
            return default(T);
        return (T)RuntimeHelpers.GetUninitializedObject(typeof(T))!;
    }
}
