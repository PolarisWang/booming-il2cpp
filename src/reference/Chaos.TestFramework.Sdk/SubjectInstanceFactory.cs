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
    /// <remarks>
    /// The null check below is load-bearing, not defensive noise.  The cast used
    /// to carry a null-forgiving `!`, which tells the compiler the result can
    /// never be null — and the AOT codegen emits that as a real `ud2` trap after
    /// the call.  Under AOT, <c>GetUninitializedObject</c> returns null for any
    /// type the codegen could not give a native body (the
    /// "ExternalRuntimeFallback ... no native body" warning), so the trap is
    /// genuinely reachable: it kills the process with SIGILL mid-fact-loop,
    /// which truncates the streaming factResults JSON and reports the whole
    /// chunk as "0/0 passed" instead of failing one subject.
    ///
    /// Throwing instead makes the failure land where the harness can attribute
    /// it: PalTryCallNoExcept catches it and records just that subject as
    /// failed, so the remaining subjects in the chunk still run.
    /// </remarks>
    public static T Create<T>()
    {
        if (typeof(T).IsValueType)
            return default(T)!;
        var instance = RuntimeHelpers.GetUninitializedObject(typeof(T));
        if (instance is null)
        {
            throw new InvalidOperationException(
                $"SubjectInstanceFactory.Create<{typeof(T)}>: GetUninitializedObject returned null. "
                + "The type has no AOT-native body, so no instance can be constructed for it.");
        }
        return (T)(object)instance;
    }
}
