using Chaos.IL2CPP.Contracts;

namespace Chaos.IL2CPP.CodeGen;

public sealed class CodeGenStage
{
    private const int AuditTranslationUnitPageSize = 1024;

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
        var internalAssemblyNames = linkedWorld.Assemblies
            .Select(assembly => assembly.Name)
            .ToHashSet(StringComparer.Ordinal);
        var methodsBySubjectId = linkedWorld.Methods
            .ToDictionary(method => method.SubjectId, StringComparer.Ordinal);
        var typedIl = new TypedIlIrArtifact
        {
            Methods = linkedWorld.Methods
                .Select(method => ToTypedIlMethodArtifact(
                    method,
                    methodShapes,
                    methodCapabilities,
                    internalAssemblyNames,
                    methodsBySubjectId))
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
        var aotCoreIr = new AotCoreIrLowering().Create(linkedWorld, typedIl, codeRegistration);
        var genericInstantiationDemandGraph = linkedWorld.GenericInstantiationDemandGraph
            ?? new GenericInstantiationDemandGraphModel
            {
                Demands = [],
            };
        var genericCapabilityMatrix = new GenericCapabilityMatrixBuilder().Build(
            ResolveOwnerSubjectId(request.InputAssemblyPath, linkedWorld.Assembly.Name),
            linkedWorld.EntryPointSubjectId,
            genericInstantiationDemandGraph,
            aotCoreIr,
            metadataWriterOutput.SupplementalMetadataTemplate);
        NativeReferenceLoweringPlanArtifact nativeReferenceLoweringPlan;
        if (linkedWorld.FullAssemblyClosure && string.IsNullOrWhiteSpace(linkedWorld.EntryPointSubjectId))
        {
            nativeReferenceLoweringPlan = CreateAssemblyFullClosureNativeReferenceRuntimeSkeletonPlan(linkedWorld, codeRegistration);
        }
        else
        {
            var loweringPlanner = new NativeReferenceLoweringPlanner();
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
            AdditionalAssemblyPaths = request.AdditionalAssemblyPaths?
                .Where(path => !string.IsNullOrWhiteSpace(path))
                .Select(path => ManagedNaming.NormalizePathForManifest(path, Environment.CurrentDirectory))
                .Distinct(StringComparer.OrdinalIgnoreCase)
                .ToList(),
            FullAssemblyClosure = request.FullAssemblyClosure,
            InputModuleVersionId = linkedWorld.Assembly.ModuleVersionId.ToString(),
            Artifacts =
            [
                new ManagedClosureArtifactRef { Kind = "typedIlIr", Path = ManagedClosureArtifactNames.TypedIlIr },
                new ManagedClosureArtifactRef { Kind = "aotCoreIr", Path = ManagedClosureArtifactNames.AotCoreIr },
                new ManagedClosureArtifactRef { Kind = "aotManifest", Path = ManagedClosureArtifactNames.AotManifest },
                new ManagedClosureArtifactRef { Kind = "metadataRegistration", Path = ManagedClosureArtifactNames.MetadataRegistration },
                new ManagedClosureArtifactRef { Kind = "supplementalMetadataTemplate", Path = ManagedClosureArtifactNames.SupplementalMetadataTemplate },
                new ManagedClosureArtifactRef { Kind = "codeRegistration", Path = ManagedClosureArtifactNames.CodeRegistration },
                new ManagedClosureArtifactRef { Kind = "genericInstantiationDemandGraph", Path = ManagedClosureArtifactNames.GenericInstantiationDemandGraph },
                new ManagedClosureArtifactRef { Kind = "genericCapabilityMatrix", Path = ManagedClosureArtifactNames.GenericCapabilityMatrix },
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
            AotCoreIr = aotCoreIr,
            AotManifest = metadataWriterOutput.AotManifest,
            MetadataRegistration = metadataWriterOutput.MetadataRegistration,
            SupplementalMetadataTemplate = metadataWriterOutput.SupplementalMetadataTemplate,
            CodeRegistration = codeRegistration,
            GenericInstantiationDemandGraph = genericInstantiationDemandGraph,
            GenericCapabilityMatrix = genericCapabilityMatrix,
            OptimizationFacts = linkedWorld.OptimizationFacts,
            PreserveDescriptor = linkedWorld.PreserveDescriptor,
            NativeReferenceLoweringPlan = nativeReferenceLoweringPlan,
            NativeAotLoweringPlan = nativeAotLoweringPlan,
            ClosureManifest = closureManifest,
        };
    }

    private static string ResolveOwnerSubjectId(string inputAssemblyPath, string fallbackOwnerSubjectId)
    {
        if (!string.IsNullOrWhiteSpace(inputAssemblyPath))
        {
            var normalizedPath = inputAssemblyPath
                .Replace(Path.AltDirectorySeparatorChar, Path.DirectorySeparatorChar)
                .Split(Path.DirectorySeparatorChar, StringSplitOptions.RemoveEmptyEntries);
            for (var index = 0; index < normalizedPath.Length - 1; index++)
            {
                if (string.Equals(normalizedPath[index], "subjects", StringComparison.OrdinalIgnoreCase))
                {
                    return normalizedPath[index + 1];
                }
            }
        }

        return fallbackOwnerSubjectId;
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

    private static NativeReferenceLoweringPlanArtifact CreateAssemblyFullClosureNativeReferenceRuntimeSkeletonPlan(
        LinkedWorldModel linkedWorld,
        CodeRegistrationArtifact codeRegistration)
    {
        var methodSubjectIds = linkedWorld.Methods
            .Select(method => method.SubjectId)
            .Distinct(StringComparer.Ordinal)
            .OrderBy(subjectId => subjectId, StringComparer.Ordinal)
            .ToList();
        var firstMethodSymbol = codeRegistration.Modules
            .SelectMany(module => module.Registrations)
            .FirstOrDefault(registration => string.Equals(registration.RegistrationKind, "methodPointer", StringComparison.Ordinal))
            ?.Symbol
            ?? $"{linkedWorld.Assembly.Name}_assembly_audit";

        return new NativeReferenceLoweringPlanArtifact
        {
            PlanKind = "assembly-full-closure-runtime-skeleton",
            AssemblyName = linkedWorld.Assembly.Name,
            EntrySubjectId = linkedWorld.EntryPointSubjectId,
            IncludeHeader = "codegen_bridge.h",
            NativeEntryFunctionName = "RunNativeReferenceAssembly",
            EntrySymbol = firstMethodSymbol,
            RuntimeExecutionKind = "assembly-bound-native-reference-skeleton",
            TranslationUnitMode = "runtime-skeleton",
            TranslationUnitMethodSubjectIds = methodSubjectIds,
            TranslationUnitMethodCount = methodSubjectIds.Count,
            TranslationUnitPageSize = AuditTranslationUnitPageSize,
            TranslationUnitPageCount = GetAuditPageCount(methodSubjectIds.Count),
            TranslationUnitPages = BuildAuditTranslationUnitPages(methodSubjectIds, "generated/runtime/native-reference.runtime-skeleton"),
            AuditStatus = "runtime-skeleton",
            AuditMessage = "assembly-bound full-closure native-reference runtime skeleton emits covered executable methods only; uncovered methods are reported separately",
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
        if (linkedWorld.FullAssemblyClosure && string.IsNullOrWhiteSpace(linkedWorld.EntryPointSubjectId))
        {
            return CreateAssemblyFullClosureNativeAotAuditPlan(linkedWorld, codeRegistration);
        }

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
            WorkloadAbi = "int(int32)",
        };
    }

    private static NativeAotLoweringPlanArtifact CreateAssemblyFullClosureNativeAotAuditPlan(
        LinkedWorldModel linkedWorld,
        CodeRegistrationArtifact codeRegistration)
    {
        var methodSubjectIds = linkedWorld.Methods
            .Select(method => method.SubjectId)
            .Distinct(StringComparer.Ordinal)
            .OrderBy(subjectId => subjectId, StringComparer.Ordinal)
            .ToList();
        var firstMethodSymbol = codeRegistration.Modules
            .SelectMany(module => module.Registrations)
            .FirstOrDefault(registration => string.Equals(registration.RegistrationKind, "methodPointer", StringComparison.Ordinal))
            ?.Symbol
            ?? $"{linkedWorld.Assembly.Name}_assembly_audit";

        return new NativeAotLoweringPlanArtifact
        {
            PlanKind = "assembly-full-closure-audit",
            AssemblyName = linkedWorld.Assembly.Name,
            EntrySubjectId = linkedWorld.EntryPointSubjectId,
            NativeEntryFunctionName = "RunNativeAotAudit",
            EntrySymbol = firstMethodSymbol,
            EntryMethodToken = "0u",
            WorkloadAbi = "audit-only",
            TranslationUnitMode = "audit-only",
            TranslationUnitMethodSubjectIds = methodSubjectIds,
            TranslationUnitMethodCount = methodSubjectIds.Count,
            TranslationUnitPageSize = AuditTranslationUnitPageSize,
            TranslationUnitPageCount = GetAuditPageCount(methodSubjectIds.Count),
            TranslationUnitPages = BuildAuditTranslationUnitPages(methodSubjectIds, "generated/audit/native-aot.audit", ".json"),
            AuditStatus = "not-yet-emittable",
            AuditMessage = "assembly-bound full-closure native-aot emission is not implemented",
        };
    }

    private static IReadOnlyList<AuditTranslationUnitPageArtifact> BuildAuditTranslationUnitPages(
        IReadOnlyList<string> methodSubjectIds,
        string pathPrefix,
        string extension = ".cpp")
    {
        var pages = new List<AuditTranslationUnitPageArtifact>();
        for (var pageIndex = 0; pageIndex * AuditTranslationUnitPageSize < methodSubjectIds.Count; pageIndex++)
        {
            var pageItems = methodSubjectIds
                .Skip(pageIndex * AuditTranslationUnitPageSize)
                .Take(AuditTranslationUnitPageSize)
                .ToList();
            pages.Add(new AuditTranslationUnitPageArtifact
            {
                PageNumber = pageIndex + 1,
                MethodCount = pageItems.Count,
                Path = $"{pathPrefix}.page-{pageIndex + 1:D4}{extension}",
                FirstMethodSubjectId = pageItems.FirstOrDefault(),
                LastMethodSubjectId = pageItems.LastOrDefault(),
            });
        }

        return pages;
    }

    private static int GetAuditPageCount(int totalMethodCount)
    {
        if (totalMethodCount <= 0)
        {
            return 0;
        }

        return (totalMethodCount + AuditTranslationUnitPageSize - 1) / AuditTranslationUnitPageSize;
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
        IReadOnlyDictionary<string, IReadOnlyList<string>> methodCapabilities,
        IReadOnlySet<string> internalAssemblyNames,
        IReadOnlyDictionary<string, ManagedMethodModel> methodsBySubjectId)
    {
        var methodShape = GetRequiredMethodShape(methodShapes, method.SubjectId);
        var capabilities = GetRequiredCapabilities(methodCapabilities, method.SubjectId);

        return new TypedIlMethodArtifact
        {
            MethodId = ManagedNaming.CreateMethodId(method),
            SubjectId = method.SubjectId,
            Signature = method.Signature,
            Identity = ManagedMethodIdentityResolver.Create(method),
            MethodRole = methodShape.MethodRole,
            BodyAvailability = methodShape.BodyAvailability,
            BodyAvailabilityCode = methodShape.BodyAvailabilityCode,
            Capabilities = capabilities,
            Parameters = method.Parameters.Select(parameter => new TypedIlParameterArtifact
            {
                Name = parameter.Name,
                Type = parameter.Type,
            }).ToList(),
            Blocks = method.Body.Blocks.Select(block => new TypedIlBlockArtifact
            {
                BlockId = block.BlockId,
                Instructions = block.Instructions
                    .Select(instruction => ToTypedIlInstructionArtifact(
                        method,
                        internalAssemblyNames,
                        methodsBySubjectId,
                        instruction))
                    .ToList(),
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

    private static TypedIlInstructionArtifact ToTypedIlInstructionArtifact(
        ManagedMethodModel method,
        IReadOnlySet<string> internalAssemblyNames,
        IReadOnlyDictionary<string, ManagedMethodModel> methodsBySubjectId,
        ManagedInstructionModel instruction)
    {
        return new TypedIlInstructionArtifact
        {
            Op = instruction.Op,
            Operand = instruction.Operand,
            ResultType = instruction.ResultType,
            Callee = instruction.Callee,
            CallSiteSignature = instruction.CallSiteSignature,
            Reference = instruction.Reference,
            DispatchKindCode = HybridDispatchResolver.ResolveInstruction(
                method.AssemblyName,
                internalAssemblyNames,
                instruction,
                methodsBySubjectId),
        };
    }
}
