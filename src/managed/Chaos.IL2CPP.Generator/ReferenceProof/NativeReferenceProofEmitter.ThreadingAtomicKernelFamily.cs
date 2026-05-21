using Chaos.IL2CPP.Contracts;
using Scriban.Runtime;

namespace Chaos.IL2CPP.Generator;

public sealed partial class NativeReferenceProofEmitter
{
    [RuntimeSkeletonFamilyPlugin("threading-atomic-kernel", CodeGenPluginProductLine.NativeReference, 410, RuntimeSkeletonFamilyPluginKind.Kernel)]
    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonThreadingAtomicKernelFamilyCore(
        RuntimeSkeletonStubBuildContext buildContext)
    {
        if (!buildContext.SubjectId.Contains("/System.Threading.Interlocked", StringComparison.Ordinal) &&
            !buildContext.SubjectId.Contains("/System.Threading.Volatile", StringComparison.Ordinal))
        {
            return RuntimeSkeletonFamilyHandlerResult.NoMatch;
        }

        return TryBuildRuntimeSkeletonThreadingAtomicKernelCore(buildContext);
    }

    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonThreadingAtomicKernelCore(
        RuntimeSkeletonStubBuildContext buildContext)
    {
        if (!buildContext.MethodsBySubjectId.TryGetValue(buildContext.SubjectId, out var method))
        {
            return RuntimeSkeletonFamilyHandlerResult.CreateUnsupported("threading-atomic-method-not-found");
        }

        if (RuntimeSkeletonThreadingAtomicCore.TryBuild(method, buildContext.StubName, out var stub))
        {
            return RuntimeSkeletonFamilyHandlerResult.CreateMatch(stub);
        }

        return RuntimeSkeletonFamilyHandlerResult.CreateUnsupported("threading-atomic-unsupported-shape");
    }
}
