namespace InterpreterArithmeticProof.AotBridge;

/// <summary>
/// External bridge exports for Phase 7 bridge call lowering proof.
/// This assembly is intentionally separate from the main InterpreterArithmeticProof
/// project so that cross-assembly CallBridge lowering can be exercised.
/// </summary>
public static class AotBridgeExports
{
    public static int Add(int a, int b) => a + b;
}
