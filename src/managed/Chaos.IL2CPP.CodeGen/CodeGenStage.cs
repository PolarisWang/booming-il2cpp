using Chaos.IL2CPP.Contracts;

namespace Chaos.IL2CPP.CodeGen;

public sealed class CodeGenStage
{
    public string Name => "CodeGen";

    public ManagedClosureResult Generate(
        ManagedClosureRequest request,
        LinkedWorldModel linkedWorld,
        MetadataWriterOutput metadataWriterOutput)
    {
        var methodShapes = linkedWorld.SemanticShapes.Methods
            .ToDictionary(shape => shape.SubjectId, StringComparer.Ordinal);
        var methodCapabilities = linkedWorld.CapabilityBundles.Methods
            .ToDictionary(bundle => bundle.SubjectId, bundle => bundle.Capabilities, StringComparer.Ordinal);
        var typedIl = new TypedIlIrArtifact
        {
            Methods = linkedWorld.Methods
                .Select(method => ToTypedIlMethodArtifact(method, methodShapes, methodCapabilities))
                .ToList(),
        };

        var codeRegistration = new CodeRegistrationArtifact
        {
            Modules =
            [
                new CodeRegistrationModule
                {
                    ModuleName = $"{linkedWorld.Assembly.Name}.dll",
                    Registrations = linkedWorld.Methods.Select((method, index) => new CodeRegistrationEntry
                    {
                        RegistrationKind = "methodPointer",
                        Slot = index,
                        Symbol = ManagedNaming.CreateMethodSymbol(method),
                        SubjectId = method.SubjectId,
                    }).ToList(),
                },
            ],
        };
        var loweringPlanner = new NativeReferenceLoweringPlanner();
        var nativeReferenceLoweringPlan = loweringPlanner.Create(
            linkedWorld,
            typedIl,
            metadataWriterOutput.MetadataRegistration,
            codeRegistration);

        var closureManifest = new ManagedClosureManifestArtifact
        {
            AssemblyName = linkedWorld.Assembly.Name,
            EntrySubjectId = linkedWorld.EntryPointSubjectId,
            InputAssemblyPath = ManagedNaming.NormalizePathForManifest(request.InputAssemblyPath, Environment.CurrentDirectory),
            InputModuleVersionId = linkedWorld.Assembly.ModuleVersionId.ToString(),
            Artifacts =
            [
                new ManagedClosureArtifactRef { Kind = "typedIlIr", Path = ManagedClosureArtifactNames.TypedIlIr },
                new ManagedClosureArtifactRef { Kind = "aotManifest", Path = ManagedClosureArtifactNames.AotManifest },
                new ManagedClosureArtifactRef { Kind = "metadataRegistration", Path = ManagedClosureArtifactNames.MetadataRegistration },
                new ManagedClosureArtifactRef { Kind = "codeRegistration", Path = ManagedClosureArtifactNames.CodeRegistration },
                new ManagedClosureArtifactRef { Kind = "optimizationFacts", Path = ManagedClosureArtifactNames.OptimizationFacts },
                new ManagedClosureArtifactRef { Kind = "nativeReferenceLoweringPlan", Path = ManagedClosureArtifactNames.NativeReferenceLoweringPlan },
            ],
        };

        return new ManagedClosureResult
        {
            OutputRootPath = request.OutputRootPath,
            TypedIlIr = typedIl,
            AotManifest = metadataWriterOutput.AotManifest,
            MetadataRegistration = metadataWriterOutput.MetadataRegistration,
            CodeRegistration = codeRegistration,
            OptimizationFacts = linkedWorld.OptimizationFacts,
            NativeReferenceLoweringPlan = nativeReferenceLoweringPlan,
            ClosureManifest = closureManifest,
        };
    }

    private static TypedIlMethodArtifact ToTypedIlMethodArtifact(
        ManagedMethodModel method,
        IReadOnlyDictionary<string, MethodShapeModel> methodShapes,
        IReadOnlyDictionary<string, IReadOnlyList<string>> methodCapabilities)
    {
        var methodShape = GetRequiredMethodShape(methodShapes, method.SubjectId);
        var capabilities = GetRequiredCapabilities(methodCapabilities, method.SubjectId);

        return new TypedIlMethodArtifact
        {
            MethodId = ManagedNaming.CreateMethodId(method),
            SubjectId = method.SubjectId,
            Signature = method.Signature,
            MethodRole = methodShape.MethodRole,
            BodyAvailability = methodShape.BodyAvailability,
            Capabilities = capabilities,
            Parameters = method.Parameters.Select(parameter => new TypedIlParameterArtifact
            {
                Name = parameter.Name,
                Type = parameter.Type,
            }).ToList(),
            Blocks = method.Body.Blocks.Select(block => new TypedIlBlockArtifact
            {
                BlockId = block.BlockId,
                Instructions = block.Instructions.Select(ToTypedIlInstructionArtifact).ToList(),
            }).ToList(),
        };
    }

    private static MethodShapeModel GetRequiredMethodShape(
        IReadOnlyDictionary<string, MethodShapeModel> methodShapes,
        string subjectId)
    {
        if (methodShapes.TryGetValue(subjectId, out var methodShape))
        {
            return methodShape;
        }

        throw new InvalidOperationException(
            $"missing semantic method shape for '{subjectId}' during typed-il generation");
    }

    private static IReadOnlyList<string> GetRequiredCapabilities(
        IReadOnlyDictionary<string, IReadOnlyList<string>> methodCapabilities,
        string subjectId)
    {
        if (methodCapabilities.TryGetValue(subjectId, out var capabilities))
        {
            return capabilities;
        }

        throw new InvalidOperationException(
            $"missing method capability bundle for '{subjectId}' during typed-il generation");
    }

    private static TypedIlInstructionArtifact ToTypedIlInstructionArtifact(ManagedInstructionModel instruction)
    {
        return new TypedIlInstructionArtifact
        {
            Op = instruction.Op,
            Operand = instruction.Operand,
            ResultType = instruction.ResultType,
            Callee = instruction.Callee,
        };
    }
}
