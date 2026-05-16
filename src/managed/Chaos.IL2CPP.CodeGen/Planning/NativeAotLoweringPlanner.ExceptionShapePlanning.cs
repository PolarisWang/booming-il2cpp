using Chaos.IL2CPP.Contracts;

namespace Chaos.IL2CPP.CodeGen;

public sealed partial class NativeAotLoweringPlanner
{
    // These TryCreate* methods always return false — the pure structured IR path
    // in StructuredIR.cs handles all realistic exception shapes.
    private static bool TryCreateCatchOnlyExceptionMethodShape(AotCoreIrMethodArtifact method, out CatchOnlyExceptionMethodShape? catchOnlyShape)
    {
        catchOnlyShape = null;
        return false;
    }

    private static bool TryCreateFilterOnlyExceptionMethodShape(AotCoreIrMethodArtifact method, out FilterOnlyExceptionMethodShape? filterOnlyShape)
    {
        filterOnlyShape = null;
        return false;
    }

    private static bool TryCreateFinallyOnlyExceptionMethodShape(AotCoreIrMethodArtifact method, out FinallyOnlyExceptionMethodShape? finallyOnlyShape)
    {
        finallyOnlyShape = null;
        return false;
    }

    private static bool TryCreateCatchAndFinallyExceptionMethodShape(AotCoreIrMethodArtifact method, out CatchAndFinallyExceptionMethodShape? catchAndFinallyShape)
    {
        catchAndFinallyShape = null;
        return false;
    }

    private static bool TryCreateFilterAndFinallyExceptionMethodShape(AotCoreIrMethodArtifact method, out FilterAndFinallyExceptionMethodShape? filterAndFinallyShape)
    {
        filterAndFinallyShape = null;
        return false;
    }

    private static bool TryCreateFinallyHandlerEmissionPlan(FinallyHandlerShape handlerShape, out FinallyHandlerEmissionPlan? emissionPlan)
    {
        emissionPlan = null;
        IReadOnlyList<AotCoreIrInstructionArtifact> instructions = handlerShape.Instructions;
        if (instructions.Count == 0) return false;

        if (!string.Equals(instructions[instructions.Count - 1].Op, "endfinally", StringComparison.Ordinal))
            return false;

        var body = instructions.Take(instructions.Count - 1).ToArray();
        if (body.Length == 0)
        {
            emissionPlan = new FinallyHandlerEmissionPlan(null, Array.Empty<AotCoreIrInstructionArtifact>());
            return true;
        }

        // Look for a single brtrue/brfalse guard at the end of the body
        var guards = body
            .Select((instruction, index) => (instruction, index))
            .Where(entry => string.Equals(entry.instruction.Op, "brtrue", StringComparison.Ordinal) ||
                            string.Equals(entry.instruction.Op, "brfalse", StringComparison.Ordinal))
            .ToArray();

        if (guards.Length == 0)
        {
            emissionPlan = new FinallyHandlerEmissionPlan(null, body);
            return true;
        }

        if (guards.Length != 1)
            return false;

        var (guardInst, guardIdx) = guards[0];
        if (GetRequiredIntOperand(guardInst) != GetRequiredIlOffset(instructions[instructions.Count - 1]))
            return false;

        var condInstructions = body.Take(guardIdx).ToArray();
        var bodyInstructions = body.Skip(guardIdx + 1).ToArray();
        if (condInstructions.Length == 0)
            return false;

        emissionPlan = new FinallyHandlerEmissionPlan(
            new FinallyHandlerGuardShape(condInstructions, string.Equals(guardInst.Op, "brtrue", StringComparison.Ordinal)),
            bodyInstructions);
        return true;
    }
}
