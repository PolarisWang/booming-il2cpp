using System.Globalization;
using Chaos.IL2CPP.Contracts;

namespace Chaos.IL2CPP.Linker;

public sealed partial class LinkerStage
{
    private static OptimizationFactsArtifact BuildOptimizationFacts(
        SemanticWorldModel semanticWorld,
        IReadOnlyList<ManagedTypeModel> orderedTypes,
        IReadOnlyList<ManagedFieldModel> orderedFields,
        IReadOnlyList<ManagedMethodModel> orderedMethods)
    {
        var methodMap = orderedMethods.ToDictionary(method => method.SubjectId, StringComparer.Ordinal);
        var closedWorldSpecializations = orderedTypes
            .Select(type => new ClosedWorldSpecializationFact
            {
                SubjectKind = "type",
                SubjectId = type.SubjectId,
                Reason = IsCompilerGeneratedAsyncStateMachine(type)
                    ? "compiler-generated-async-state-machine"
                    : "closed-world-type",
            })
            .Concat(
                orderedMethods.Select(method => new ClosedWorldSpecializationFact
                {
                    SubjectKind = "method",
                    SubjectId = method.SubjectId,
                    Reason = string.Equals(method.SubjectId, semanticWorld.EntryPointSubjectId, StringComparison.Ordinal)
                        ? "closed-world-entry"
                        : "closed-world-method",
                }))
            .ToList();

        var dispatchFacts = orderedMethods
            .SelectMany(method => method.Body.Blocks.SelectMany(block =>
                block.Instructions
                    .Select((instruction, index) => new { instruction, index, block.Instructions, method })
                    .Where(candidate =>
                        string.Equals(candidate.instruction.Op, "callvirt", StringComparison.Ordinal) &&
                        !string.IsNullOrEmpty(candidate.instruction.Callee))
                    .Select(candidate =>
                    {
                        var declaredTargetSubjectId = candidate.instruction.Callee!;
                        var resolvedTargetSubjectId = TryResolveDevirtualizedCallTarget(
                            methodMap,
                            candidate.Instructions,
                            candidate.index,
                            declaredTargetSubjectId) ?? declaredTargetSubjectId;
                        var interfaceDispatch = IsInterfaceDispatchTarget(semanticWorld.Types, declaredTargetSubjectId);
                        var devirtualized = !string.Equals(resolvedTargetSubjectId, declaredTargetSubjectId, StringComparison.Ordinal)
                            || IsInternalSubjectId(semanticWorld, declaredTargetSubjectId);

                        return new DispatchFact
                        {
                            MethodSubjectId = method.SubjectId,
                            DispatchKind = interfaceDispatch
                                ? "callvirt.interface-runtime-helper"
                                : devirtualized && !string.Equals(resolvedTargetSubjectId, declaredTargetSubjectId, StringComparison.Ordinal)
                                    ? "callvirt.devirtualized-direct"
                                    : "callvirt",
                            TargetSubjectId = resolvedTargetSubjectId,
                            Devirtualized = devirtualized,
                        };
                    })))
            .Concat(BuildDelegateDispatchFacts(orderedMethods))
            .ToList();

        var layoutFacts = orderedFields
            .Select(field => new LayoutFact
            {
                SubjectKind = "field",
                SubjectId = field.SubjectId,
                DataKind = ResolveLayoutDataKind(field.FieldType),
                ElementType = TryGetArrayElementType(field.FieldType),
                RequiresBoxing = RequiresBoxing(field.FieldType),
            })
            .Concat(
                orderedMethods.SelectMany(
                    method => method.Parameters
                        .Select(
                            (parameter, index) => new { parameter, index, method })
                        .Where(candidate => candidate.parameter.Type.EndsWith("[]", StringComparison.Ordinal))
                        .Select(candidate => new LayoutFact
                        {
                            SubjectKind = "parameter",
                            SubjectId = ManagedNaming.CreateParameterSubjectId(candidate.method.SubjectId, candidate.index, candidate.parameter.Name),
                            DataKind = "array",
                            ElementType = TryGetArrayElementType(candidate.parameter.Type),
                            RequiresBoxing = false,
                        })))
            .Concat(
                orderedTypes
                    .Where(type => !string.Equals(type.SubjectId, type.DefinitionSubjectId, StringComparison.Ordinal))
                    .Select(type => new LayoutFact
                    {
                        SubjectKind = "type",
                        SubjectId = type.SubjectId,
                        DataKind = "closed-generic-instance",
                        ElementType = type.DefinitionSubjectId,
                        RequiresBoxing = false,
                    }))
            .ToList();

        return new OptimizationFactsArtifact
        {
            ClosedWorldSpecializations = closedWorldSpecializations,
            DispatchFacts = dispatchFacts,
            LayoutFacts = layoutFacts,
            ExceptionFacts = BuildExceptionFacts(orderedMethods),
        };
    }

    private static PreserveDescriptorArtifact BuildPreserveDescriptor(
        string inputAssemblyName,
        bool fullAssemblyClosure,
        IReadOnlyList<ManagedTypeModel> orderedTypes,
        IReadOnlyList<ManagedFieldModel> orderedFields,
        IReadOnlyList<ManagedPropertyModel> orderedProperties,
        IReadOnlyList<ManagedMethodModel> orderedMethods)
    {
        var entries = orderedTypes
            .Where(type => type.IsPreserved)
            .Select(type => new PreserveDescriptorEntry
            {
                SubjectKind = "type",
                SubjectId = type.SubjectId,
                Preserve = "all",
                Reason = "preserve-attribute",
            })
            .Concat(
                orderedFields
                    .Where(field => field.IsPreserved)
                    .Select(field => new PreserveDescriptorEntry
                    {
                        SubjectKind = "field",
                        SubjectId = field.SubjectId,
                        Preserve = "members",
                        Reason = "preserve-attribute",
                    }))
            .Concat(
                orderedProperties
                    .Where(property => property.IsPreserved)
                    .Select(property => new PreserveDescriptorEntry
                    {
                        SubjectKind = "property",
                        SubjectId = property.SubjectId,
                        Preserve = "members",
                        Reason = "preserve-attribute",
                    }))
            .Concat(
                orderedMethods
                    .Where(method => method.IsPreserved)
                    .Select(method => new PreserveDescriptorEntry
                    {
                        SubjectKind = "method",
                        SubjectId = method.SubjectId,
                        Preserve = "signature",
                        Reason = "preserve-attribute",
                    }))
            .Concat(
                fullAssemblyClosure
                    ? orderedTypes
                        .Where(type => string.Equals(type.AssemblyName, inputAssemblyName, StringComparison.Ordinal))
                        .Select(type => new PreserveDescriptorEntry
                        {
                            SubjectKind = "type",
                            SubjectId = type.SubjectId,
                            Preserve = "all",
                            Reason = "full-assembly-closure",
                        })
                    : [])
            .Concat(
                fullAssemblyClosure
                    ? orderedFields
                        .Where(field => string.Equals(field.AssemblyName, inputAssemblyName, StringComparison.Ordinal))
                        .Select(field => new PreserveDescriptorEntry
                        {
                            SubjectKind = "field",
                            SubjectId = field.SubjectId,
                            Preserve = "members",
                            Reason = "full-assembly-closure",
                        })
                    : [])
            .Concat(
                fullAssemblyClosure
                    ? orderedProperties
                        .Where(property => string.Equals(property.AssemblyName, inputAssemblyName, StringComparison.Ordinal))
                        .Select(property => new PreserveDescriptorEntry
                        {
                            SubjectKind = "property",
                            SubjectId = property.SubjectId,
                            Preserve = "members",
                            Reason = "full-assembly-closure",
                        })
                    : [])
            .Concat(
                fullAssemblyClosure
                    ? orderedMethods
                        .Where(method => string.Equals(method.AssemblyName, inputAssemblyName, StringComparison.Ordinal))
                        .Select(method => new PreserveDescriptorEntry
                        {
                            SubjectKind = "method",
                            SubjectId = method.SubjectId,
                            Preserve = "signature",
                            Reason = "full-assembly-closure",
                        })
                    : [])
            .GroupBy(entry => $"{entry.SubjectKind}:{entry.SubjectId}", StringComparer.Ordinal)
            .Select(group => group.First())
            .ToList();

        return new PreserveDescriptorArtifact
        {
            Entries = entries,
        };
    }

    private static IReadOnlyList<DispatchFact> BuildDelegateDispatchFacts(
        IReadOnlyList<ManagedMethodModel> orderedMethods)
    {
        return orderedMethods
            .SelectMany(method => method.Body.Blocks.SelectMany(block =>
                block.Instructions
                    .Where(instruction =>
                        (string.Equals(instruction.Op, "ldftn", StringComparison.Ordinal) ||
                         string.Equals(instruction.Op, "ldvirtftn", StringComparison.Ordinal)) &&
                        !string.IsNullOrWhiteSpace(instruction.Callee))
                    .Select(instruction => new DispatchFact
                    {
                        MethodSubjectId = method.SubjectId,
                        DispatchKind = string.Equals(instruction.Op, "ldftn", StringComparison.Ordinal)
                            ? "delegate.exact-target-direct"
                            : "delegate.runtime-helper-fallback",
                        TargetSubjectId = instruction.Callee!,
                        Devirtualized = string.Equals(instruction.Op, "ldftn", StringComparison.Ordinal),
                    })))
            .ToList();
    }

    private static IReadOnlyList<ExceptionFact> BuildExceptionFacts(
        IReadOnlyList<ManagedMethodModel> orderedMethods)
    {
        return orderedMethods
            .SelectMany(method =>
            {
                _ = HasThrowCatchFinallyShape(method) ? "throw-catch-finally" : null;
                _ = HasNestedExceptionHandlerShape(method) ? "nested-throw-catch-finally" : null;
                _ = HasUtf8StringMarshalShape(method) ? "utf8-string-marshal" : null;
                _ = HasUnmanagedExportShape(method) ? "unmanaged-callers-only-export" : null;
                return method.Body.ExceptionRegions.Select(region => new ExceptionFact
                {
                    MethodSubjectId = method.SubjectId,
                    HandlingKind = region.HandlingKind,
                    CatchTypeSubjectId = region.CatchTypeSubjectId,
                });
            })
            .ToList();
    }

    private static bool HasThrowCatchFinallyShape(ManagedMethodModel method)
    {
        var instructions = method.Body.Blocks.SelectMany(block => block.Instructions);
        return instructions.Any(instruction => string.Equals(instruction.Op, "throw", StringComparison.Ordinal)) &&
               method.Body.ExceptionRegions.Any(region => string.Equals(region.HandlingKind, "catch", StringComparison.Ordinal)) &&
               method.Body.ExceptionRegions.Any(region => string.Equals(region.HandlingKind, "finally", StringComparison.Ordinal));
    }

    private static bool HasNestedExceptionHandlerShape(ManagedMethodModel method)
    {
        var exceptionRegions = method.Body.ExceptionRegions;
        for (var outerIndex = 0; outerIndex < exceptionRegions.Count; outerIndex++)
        {
            var outerRegion = exceptionRegions[outerIndex];
            var outerTryEnd = outerRegion.TryOffset + outerRegion.TryLength;

            for (var innerIndex = 0; innerIndex < exceptionRegions.Count; innerIndex++)
            {
                if (outerIndex == innerIndex)
                {
                    continue;
                }

                var innerRegion = exceptionRegions[innerIndex];
                var innerTryEnd = innerRegion.TryOffset + innerRegion.TryLength;
                if (outerRegion.TryOffset <= innerRegion.TryOffset &&
                    innerTryEnd <= outerTryEnd &&
                    (outerRegion.TryOffset != innerRegion.TryOffset || outerTryEnd != innerTryEnd))
                {
                    return true;
                }
            }
        }

        return false;
    }

    private static bool HasUtf8StringMarshalShape(ManagedMethodModel method)
    {
        return method.Body.Blocks
            .SelectMany(block => block.Instructions)
            .Any(instruction =>
                string.Equals(instruction.Callee, "System.Runtime.InteropServices.Marshal::StringToCoTaskMemUTF8(System.String)", StringComparison.Ordinal) ||
                string.Equals(instruction.Callee, "System.Runtime.InteropServices.Marshal::PtrToStringUTF8(System.IntPtr)", StringComparison.Ordinal) ||
                string.Equals(instruction.Callee, "System.Runtime.InteropServices.Marshal::FreeCoTaskMem(System.IntPtr)", StringComparison.Ordinal));
    }

    private static bool HasUnmanagedExportShape(ManagedMethodModel method)
    {
        return method.IsUnmanagedCallersOnly;
    }

}
