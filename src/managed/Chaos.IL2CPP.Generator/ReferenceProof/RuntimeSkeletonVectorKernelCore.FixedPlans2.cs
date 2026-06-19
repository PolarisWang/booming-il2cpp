using Chaos.IL2CPP.Contracts;
using System.Collections.Concurrent;
using System.Collections.Generic;
using System.Linq;

namespace Chaos.IL2CPP.Generator;

internal static partial class RuntimeSkeletonVectorKernelCore
{
    private static bool TryCreateCapabilityQueryPlan(
        RuntimeSkeletonVectorKernelSemanticDescriptor descriptor,
        string subjectId,
        RuntimeSkeletonVectorKernelBackendKind backend,
        out RuntimeSkeletonVectorKernelEmissionPlan plan)
    {
        plan = null!;
        if (!TryResolveCapabilityScalarSelector(
                descriptor.SubjectIdPrefix,
                subjectId,
                out var methodSuffix,
                out var scalarTypeSubjectId,
                out var scalarResolutionKind,
                out var scalarGenericArgumentIndex) ||
            !TryGetFixedVectorWidthBytes(descriptor.SubjectIdPrefix, out var fixedVectorWidthBytes))
        {
            return false;
        }

        return methodSuffix switch
        {
            var current when current.StartsWith("get_Count:", StringComparison.Ordinal) =>
                TryCreatePlanCore(
                    descriptor,
                    "vector-capability-query",
                    backend,
                    new RuntimeSkeletonVectorKernelCarrierSchema(null, [], "CHAOS_IL2CPP_INT32"),
                    string.Empty,
                    scalarTypeSubjectId,
                    fixedVectorWidthBytes,
                    scalarResolutionKind,
                    scalarGenericArgumentIndex,
                    out plan),
            var current when current.StartsWith("get_IsSupported:", StringComparison.Ordinal) =>
                TryCreatePlanCore(
                    descriptor,
                    "vector-capability-query",
                    backend,
                    new RuntimeSkeletonVectorKernelCarrierSchema(null, [], "bool"),
                    string.Empty,
                    scalarTypeSubjectId,
                    fixedVectorWidthBytes,
                    scalarResolutionKind,
                    scalarGenericArgumentIndex,
                    out plan),
            var current when current.StartsWith("get_One:", StringComparison.Ordinal) =>
                TryCreatePlanCore(
                    descriptor,
                    "vector-capability-query",
                    backend,
                    new RuntimeSkeletonVectorKernelCarrierSchema(null, [], descriptor.CarrierCppType),
                    string.Empty,
                    scalarTypeSubjectId,
                    fixedVectorWidthBytes,
                    scalarResolutionKind,
                    scalarGenericArgumentIndex,
                    out plan),
            _ => false,
        };
    }

}
