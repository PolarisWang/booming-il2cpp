using Chaos.IL2CPP.Contracts;

namespace Chaos.IL2CPP.CodeGen;

internal static class RuntimeSkeletonBackendSelector
{
    public static RuntimeSkeletonExecutionBackendKind SelectBackend(
        TypedIlMethodArtifact method,
        RuntimeSkeletonSemanticFamilyDescriptor descriptor)
    {
        if (descriptor.SupportsImportedBridge &&
            string.Equals(method.MethodRole, "imported-method", StringComparison.Ordinal))
        {
            return RuntimeSkeletonExecutionBackendKind.ImportedBridge;
        }

        return descriptor.PreferredBackend;
    }
}
