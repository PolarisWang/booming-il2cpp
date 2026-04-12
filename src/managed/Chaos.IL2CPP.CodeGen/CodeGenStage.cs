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
            Modules = linkedWorld.Assemblies
                .Select(assembly => new CodeRegistrationModule
                {
                    ModuleName = $"{assembly.Name}.dll",
                    Registrations = linkedWorld.Methods
                        .Where(method => string.Equals(method.AssemblyName, assembly.Name, StringComparison.Ordinal))
                        .Select((method, index) => new CodeRegistrationEntry
                        {
                            RegistrationKind = "methodPointer",
                            Slot = index,
                            Symbol = ManagedNaming.CreateMethodSymbol(method),
                            SubjectId = method.SubjectId,
                        })
                        .ToList(),
                })
                .ToList(),
        };
        var loweringPlanner = new NativeReferenceLoweringPlanner();
        NativeReferenceLoweringPlanArtifact nativeReferenceLoweringPlan;
        try
        {
            nativeReferenceLoweringPlan = loweringPlanner.Create(
                linkedWorld,
                typedIl,
                metadataWriterOutput.MetadataRegistration,
                codeRegistration);
        }
        catch when (ShouldFallbackToGenericLoweringPlan(linkedWorld))
        {
            nativeReferenceLoweringPlan = CreateGenericLoweringPlan(linkedWorld, codeRegistration);
        }

        var nativeAotLoweringPlan = CreateNativeAotLoweringPlan(
            linkedWorld,
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
                new ManagedClosureArtifactRef { Kind = "supplementalMetadataTemplate", Path = ManagedClosureArtifactNames.SupplementalMetadataTemplate },
                new ManagedClosureArtifactRef { Kind = "codeRegistration", Path = ManagedClosureArtifactNames.CodeRegistration },
                new ManagedClosureArtifactRef { Kind = "optimizationFacts", Path = ManagedClosureArtifactNames.OptimizationFacts },
                new ManagedClosureArtifactRef { Kind = "preserveDescriptor", Path = ManagedClosureArtifactNames.PreserveDescriptor },
                new ManagedClosureArtifactRef { Kind = "nativeReferenceLoweringPlan", Path = ManagedClosureArtifactNames.NativeReferenceLoweringPlan },
                new ManagedClosureArtifactRef { Kind = "nativeAotLoweringPlan", Path = ManagedClosureArtifactNames.NativeAotLoweringPlan },
            ],
        };

        return new ManagedClosureResult
        {
            OutputRootPath = request.OutputRootPath,
            TypedIlIr = typedIl,
            AotManifest = metadataWriterOutput.AotManifest,
            MetadataRegistration = metadataWriterOutput.MetadataRegistration,
            SupplementalMetadataTemplate = metadataWriterOutput.SupplementalMetadataTemplate,
            CodeRegistration = codeRegistration,
            OptimizationFacts = linkedWorld.OptimizationFacts,
            PreserveDescriptor = linkedWorld.PreserveDescriptor,
            NativeReferenceLoweringPlan = nativeReferenceLoweringPlan,
            NativeAotLoweringPlan = nativeAotLoweringPlan,
            ClosureManifest = closureManifest,
        };
    }

    private static bool ShouldFallbackToGenericLoweringPlan(LinkedWorldModel linkedWorld)
    {
        if (linkedWorld.Assemblies.Count > 1 ||
            linkedWorld.PreserveDescriptor.Entries.Count > 0 ||
            linkedWorld.Dependencies.Any(dependency =>
                string.Equals(dependency.Reason, "external-call", StringComparison.Ordinal)))
        {
            return true;
        }

        var entryPointMethod = linkedWorld.Methods.FirstOrDefault(method =>
            string.Equals(method.SubjectId, linkedWorld.EntryPointSubjectId, StringComparison.Ordinal));
        if (entryPointMethod is null)
        {
            return false;
        }

        return entryPointMethod.IsStatic &&
               entryPointMethod.Parameters.Count == 0 &&
               !string.Equals(entryPointMethod.Name, "Run", StringComparison.Ordinal);
    }

    private static NativeReferenceLoweringPlanArtifact CreateGenericLoweringPlan(
        LinkedWorldModel linkedWorld,
        CodeRegistrationArtifact codeRegistration)
    {
        var entrySymbol = codeRegistration.Modules
            .SelectMany(module => module.Registrations)
            .FirstOrDefault(registration => string.Equals(registration.SubjectId, linkedWorld.EntryPointSubjectId, StringComparison.Ordinal))
            ?.Symbol
            ?? "analysis_only_entry";

        return new NativeReferenceLoweringPlanArtifact
        {
            PlanKind = "generic-analysis-only",
            AssemblyName = linkedWorld.Assembly.Name,
            EntrySubjectId = linkedWorld.EntryPointSubjectId,
            IncludeHeader = "codegen_bridge.h",
            NativeEntryFunctionName = "RunNativeReference",
            EntrySymbol = entrySymbol,
            ReferenceTypeToken = "0u",
            CapturedFieldToken = "0u",
            EntryMethodToken = "0u",
            ConsoleWriteLineStringIcall = "System.Console/System.Console::WriteLine(System.String)",
        };
    }

    private static NativeAotLoweringPlanArtifact CreateNativeAotLoweringPlan(
        LinkedWorldModel linkedWorld,
        MetadataRegistrationArtifact metadataRegistration,
        CodeRegistrationArtifact codeRegistration)
    {
        var entrySymbol = codeRegistration.Modules
            .SelectMany(module => module.Registrations)
            .FirstOrDefault(registration => string.Equals(registration.SubjectId, linkedWorld.EntryPointSubjectId, StringComparison.Ordinal))
            ?.Symbol
            ?? "analysis_only_entry";
        var entryMethodToken = metadataRegistration.Registrations
            .FirstOrDefault(registration =>
                string.Equals(registration.RegistrationKind, "method", StringComparison.Ordinal) &&
                string.Equals(registration.SubjectId, linkedWorld.EntryPointSubjectId, StringComparison.Ordinal))
            ?.SubjectId is null
                ? "0u"
                : FormatCppTokenLiteral(metadataRegistration, linkedWorld.EntryPointSubjectId);

        return new NativeAotLoweringPlanArtifact
        {
            PlanKind = "generic-managed-entry",
            AssemblyName = linkedWorld.Assembly.Name,
            EntrySubjectId = linkedWorld.EntryPointSubjectId,
            NativeEntryFunctionName = "RunNativeAot",
            EntrySymbol = entrySymbol,
            EntryMethodToken = entryMethodToken,
            WorkloadAbi = "int(void)",
        };
    }

    private static string FormatCppTokenLiteral(
        MetadataRegistrationArtifact metadataRegistration,
        string subjectId)
    {
        var token = metadataRegistration.Registrations
            .FirstOrDefault(registration =>
                string.Equals(registration.RegistrationKind, "method", StringComparison.Ordinal) &&
                string.Equals(registration.SubjectId, subjectId, StringComparison.Ordinal))
            ?.DefinitionSubjectId;

        if (string.IsNullOrWhiteSpace(token))
        {
            return "0u";
        }

        return "0u";
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
