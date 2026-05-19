using Chaos.IL2CPP.Interpreter;

namespace InterpreterArithmeticProof;

/// <summary>
/// Proof methods for Phase 7 interpreter arithmetic lowering and execution.
/// Each method exercises a specific IL→IR lowering path.
/// </summary>
public static class ArithmeticProof
{
    // ── Pure arithmetic ──────────────────────────────────────────────────
    // IL: ldarg.0, ldarg.1, add, ret
    public static int Add(int a, int b) => a + b;

    // ── External bridge call ─────────────────────────────────────────────
    // Calls AotBridgeExports.Add from the AotBridge assembly.
    // This exercises cross-assembly CallBridge lowering.
    public static int CallAotBridgeAdd(int a, int b) =>
        AotBridge.AotBridgeExports.Add(a, b);

    // ── Same-assembly static call ────────────────────────────────────────
    // Calls ArithmeticProof.Add locally — exercises same-assembly Call lowering.
    public static int CallLocalAdd(int a, int b) => Add(a, b);

    // ── External callvirt lowering (String.get_Length) ───────────────────
    // Calls string.Length via callvirt — exercises CallBridge path for
    // external instance methods.
    public static int CallStringLength(string value) => value.Length;

    // ── Same-assembly instance callvirt ──────────────────────────────────
    // Calls InstanceArithmetic.AddOne via callvirt on a local receiver.
    // This exercises same-assembly CallVirt with real receiver execution.
    public static int CallInstanceAddOne(InstanceArithmetic instance, int value) =>
        instance.AddOne(value);

    // ── Real try/catch ───────────────────────────────────────────────────
    // try { return a / b; } catch { return -1; }
    // Exercises real ManagedExceptionRegionModel → IRExceptionRegion lowering
    // for catch with DivideByZeroException guard.
    public static int DivideOrCatch(int a, int b)
    {
        try
        {
            return a / b;
        }
        catch (System.DivideByZeroException)
        {
            return -1;
        }
    }

    // ── Real rethrow ─────────────────────────────────────────────────────
    // try { return a / b; } catch { if (b == 0) throw; else return -1; }
    // Exercises ManagedExceptionRegion rethrow lowering.
    public static int DivideOrRethrow(int a, int b)
    {
        try
        {
            return a / b;
        }
        catch
        {
            if (b == 0)
                throw;
            return -1;
        }
    }

    // ── Real try/finally ─────────────────────────────────────────────────
    // Exercises ManagedExceptionRegion finally lowering with leave/endfinally.
    // The result is a + b (finally doesn't change the return value in well-
    // behaved code, but its side effects are observable via the counter).
    private static int s_finallyCounter;

    public static int AddWithFinally(int a, int b)
    {
        s_finallyCounter = 0;
        try
        {
            return a + b;
        }
        finally
        {
            s_finallyCounter++;
        }
    }

    public static int GetFinallyCounter() => s_finallyCounter;
}

/// <summary>
/// Instance type for same-assembly callvirt proof.
/// </summary>
public sealed class InstanceArithmetic
{
    public int AddOne(int value) => value + 1;
}
