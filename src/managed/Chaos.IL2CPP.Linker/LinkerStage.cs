using System.Globalization;
using Chaos.IL2CPP.Contracts;

namespace Chaos.IL2CPP.Linker;

public sealed partial class LinkerStage
{
    public string Name => "Linker";

    public PipelineResult<LinkedWorldModel> Link(SemanticWorldModel semanticWorld)
    {
        try
        {
            var reachableClosure = ComputeReachableClosure(semanticWorld);
            var orderedMethods = reachableClosure.Methods
                .OrderBy(method => string.IsNullOrWhiteSpace(semanticWorld.EntryPointSubjectId) ? 1 : string.Equals(method.SubjectId, semanticWorld.EntryPointSubjectId, StringComparison.Ordinal) ? 0 : 1)
                .ThenBy(method => method.MetadataToken)
                .ToList();
            var orderedTypes = OrderTypes(reachableClosure.Types, orderedMethods);
            var orderedFields = reachableClosure.Fields
                .OrderBy(field => field.MetadataToken)
                .ToList();
            var orderedProperties = reachableClosure.Properties
                .OrderBy(property => property.MetadataToken)
                .ToList();
            var dependencies = CollectExternalDependencies(semanticWorld, orderedMethods);
            var canonicalSubjects = FilterCanonicalSubjects(semanticWorld, orderedTypes, orderedFields, orderedProperties, orderedMethods);
            var semanticShapes = FilterSemanticShapes(semanticWorld, orderedTypes, orderedFields, orderedProperties, orderedMethods);
            var capabilityBundles = FilterCapabilityBundles(semanticWorld, orderedMethods);
            var optimizationFacts = BuildOptimizationFacts(semanticWorld, orderedTypes, orderedFields, orderedMethods);
            var preserveDescriptor = BuildPreserveDescriptor(
                semanticWorld.Assembly.Name,
                semanticWorld.FullAssemblyClosure,
                orderedTypes,
                orderedFields,
                orderedProperties,
                orderedMethods);

            return PipelineResult<LinkedWorldModel>.Ok(new LinkedWorldModel
            {
                InputAssemblyPath = semanticWorld.InputAssemblyPath,
                FullAssemblyClosure = semanticWorld.FullAssemblyClosure,
                Assembly = semanticWorld.Assembly,
                Assemblies = semanticWorld.Assemblies,
                EntryPointSubjectId = semanticWorld.EntryPointSubjectId,
                GenericInstantiationDemandGraph = semanticWorld.GenericInstantiationDemandGraph,
                Types = orderedTypes,
                Fields = orderedFields,
                Properties = orderedProperties,
                Methods = orderedMethods,
                Dependencies = dependencies,
                CanonicalSubjects = canonicalSubjects,
                SemanticShapes = semanticShapes,
                CapabilityBundles = capabilityBundles,
                OptimizationFacts = optimizationFacts,
                PreserveDescriptor = preserveDescriptor,
            });
        }
        catch (Exception ex)
        {
            return PipelineResult<LinkedWorldModel>.Fail("LINKER_LINK_FAILED",
                $"Linker stage failed: {ex.Message}", ex);
        }
    }

}

internal sealed record ReachableClosure(
    IReadOnlyList<ManagedTypeModel> Types,
    IReadOnlyList<ManagedFieldModel> Fields,
    IReadOnlyList<ManagedPropertyModel> Properties,
    IReadOnlyList<ManagedMethodModel> Methods);
