using Chaos.IL2CPP.Contracts;

namespace Chaos.IL2CPP.CodeGen;

public sealed partial class NativeAotLoweringPlanner
{
    private static bool TryCreateCatchOnlyExceptionMethodShape(AotCoreIrMethodArtifact method, out CatchOnlyExceptionMethodShape? catchOnlyShape)
        => LegacyTryCreateCatchOnlyExceptionMethodShape(method, out catchOnlyShape);

    private static bool TryCreateFilterOnlyExceptionMethodShape(AotCoreIrMethodArtifact method, out FilterOnlyExceptionMethodShape? filterOnlyShape)
        => LegacyTryCreateFilterOnlyExceptionMethodShape(method, out filterOnlyShape);

    private static bool TryCreateFinallyOnlyExceptionMethodShape(AotCoreIrMethodArtifact method, out FinallyOnlyExceptionMethodShape? finallyOnlyShape)
        => LegacyTryCreateFinallyOnlyExceptionMethodShape(method, out finallyOnlyShape);

    private static bool TryCreateCatchAndFinallyExceptionMethodShape(AotCoreIrMethodArtifact method, out CatchAndFinallyExceptionMethodShape? catchAndFinallyShape)
        => LegacyTryCreateCatchAndFinallyExceptionMethodShape(method, out catchAndFinallyShape);

    private static bool TryCreateFilterAndFinallyExceptionMethodShape(AotCoreIrMethodArtifact method, out FilterAndFinallyExceptionMethodShape? filterAndFinallyShape)
        => LegacyTryCreateFilterAndFinallyExceptionMethodShape(method, out filterAndFinallyShape);

    private static bool TryCreateFinallyHandlerEmissionPlan(FinallyHandlerShape handlerShape, out FinallyHandlerEmissionPlan? emissionPlan)
        => LegacyTryCreateFinallyHandlerEmissionPlan(handlerShape, out emissionPlan);
}
