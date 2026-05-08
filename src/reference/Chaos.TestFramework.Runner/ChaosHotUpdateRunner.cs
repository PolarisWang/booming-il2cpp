namespace Chaos.TestFramework.Runner;

/// <summary>
/// HotUpdate runner: proof verification under hotpatch mode.
/// The entry methods are identical to proof — they use Assert.Equal etc.
/// Patch lifecycle (ApplyPatch/Unpatch) is managed externally by the native
/// runtime bootstrap before this runner is invoked.
/// </summary>
public static class ChaosHotUpdateRunner
{
    /// <summary>
    /// Run all proof methods under hotpatch mode.
    /// Delegates to ChaosProofRunner since entry Assert methods are identical.
    /// </summary>
    public static int RunAll(Action[] methods, int methodCount)
    {
        return ChaosProofRunner.RunAll(methods, methodCount);
    }
}