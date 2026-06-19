using Chaos.IL2CPP.Contracts;
using System.Reflection;

namespace Chaos.IL2CPP.Generator;

public sealed partial class CodeGenStage
{

    private static NativeAotLoweringPlanArtifact CreateAssemblyFullClosureNativeAotPlan(
        LinkedWorldModel linkedWorld,
        CodeRegistrationArtifact codeRegistration)
    {
        // Pick first method by SubjectId as synthetic entry
        var firstMethod = linkedWorld.Methods
            .OrderBy(m => m.SubjectId, StringComparer.Ordinal)
            .FirstOrDefault();
        var firstMethodSymbol = firstMethod is not null
            ? codeRegistration.Modules
                .SelectMany(module => module.Registrations)
                .FirstOrDefault(registration =>
                    string.Equals(registration.SubjectId, firstMethod.SubjectId, StringComparison.Ordinal))
                ?.Symbol
            : $"{linkedWorld.Assembly.Name}_synthetic_entry";

        // Enable TU paging when method count exceeds the NativeAotPageSize threshold.
        // This splits the generated C++ code across multiple .cpp files, each including
        // the shared header for type declarations. Page 0 contains TypeInfoV0 inline
        // definitions; subsequent pages reference them via extern declarations.
        var methodSubjectIds = linkedWorld.Methods
            .Select(m => m.SubjectId)
            .ToArray();
        int? pageSize = methodSubjectIds.Length > NativeAotPageSize ? NativeAotPageSize : null;
        IReadOnlyList<AuditTranslationUnitPageArtifact>? pages = pageSize.HasValue
            ? BuildAuditTranslationUnitPages(methodSubjectIds, "generated/native-aot")
            : null;

        return new NativeAotLoweringPlanArtifact
        {
            PlanKind = "full-assembly-entry",
            AssemblyName = linkedWorld.Assembly.Name,
            EntrySubjectId = firstMethod?.SubjectId ?? linkedWorld.EntryPointSubjectId,
            NativeEntryFunctionName = "RunNativeAot",
            EntrySymbol = firstMethodSymbol ?? "",
            EntryMethodToken = "0u",
            WorkloadAbi = "full-assembly",
            TranslationUnitPageSize = pageSize,
            TranslationUnitPageCount = pages?.Count,
            TranslationUnitPages = pages,
        };
    }



    private static IReadOnlyDictionary<string, string> BuildCanonicalSubjectIdLookup(
        IReadOnlyList<CanonicalSubjectModel> canonicalSubjects)
    {
        return canonicalSubjects
            .GroupBy(subject => subject.SubjectId, StringComparer.Ordinal)
            .ToDictionary(
                group => group.Key,
                group => group.Last().CanonicalSubjectId,
                StringComparer.Ordinal);
    }



    private static string ResolveCanonicalSubjectId(
        IReadOnlyDictionary<string, string> canonicalSubjectIds,
        string subjectId)
    {
        return canonicalSubjectIds.TryGetValue(subjectId, out var canonicalSubjectId)
            ? canonicalSubjectId
            : ManagedNaming.CanonicalizeSubjectId(subjectId);
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

        _ = token; // metadataRegistration is read for validation only; token literal is always "0u" for now
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
            ConstrainedTypeSubjectId = instruction.ConstrainedTypeSubjectId,
            DispatchKindCode = HybridDispatchResolver.ResolveInstruction(
                method.AssemblyName,
                internalAssemblyNames,
                instruction,
                methodsBySubjectId),
        };
    }



    private static StageLookups BuildStageLookups(LinkedWorldModel linkedWorld)
    {
        var methodShapes = new Dictionary<string, MethodShapeModel>(linkedWorld.SemanticShapes.Methods.Count, StringComparer.Ordinal);
        var methodCapabilities = new Dictionary<string, IReadOnlyList<string>>(linkedWorld.CapabilityBundles.Methods.Count, StringComparer.Ordinal);
        var internalAssemblyNames = new HashSet<string>(linkedWorld.Assemblies.Count, StringComparer.Ordinal);
        var methodsBySubjectId = new Dictionary<string, ManagedMethodModel>(linkedWorld.Methods.Count, StringComparer.Ordinal);

        foreach (var method in linkedWorld.Methods)
        {
            methodsBySubjectId[method.SubjectId] = method;
        }

        foreach (var shape in linkedWorld.SemanticShapes.Methods)
        {
            methodShapes[shape.SubjectId] = shape;
        }

        foreach (var bundle in linkedWorld.CapabilityBundles.Methods)
        {
            methodCapabilities[bundle.SubjectId] = bundle.Capabilities;
        }

        foreach (var assembly in linkedWorld.Assemblies)
        {
            internalAssemblyNames.Add(assembly.Name);
        }

        // Also include assembly names from methods — methods may be loaded from
        // additional assemblies (via AdditionalAssemblyPaths) even when those
        // assemblies are not tracked in linkedWorld.Assemblies.
        // Without this, calls to methods from these assemblies get
        // HybridDispatchKind.ExternalRuntime instead of Direct dispatch,
        // routing through chaos_external_runtime_* stubs that call
        // CHAOS_IL2CPP_FAIL().
        foreach (var method in linkedWorld.Methods)
        {
            if (!string.IsNullOrEmpty(method.AssemblyName))
                internalAssemblyNames.Add(method.AssemblyName);
        }

        return new StageLookups(methodShapes, methodCapabilities, internalAssemblyNames, methodsBySubjectId);
    }



    private static IReadOnlyList<CodeRegistrationTypeCapabilityEntry> BuildCodeRegistrationTypeCapabilities(
        MetadataRegistrationArtifact metadataRegistration)
    {
        static CodeRegistrationTypeCapabilityEntry? TryCreate(MetadataRegistrationEntry registration)
        {
            if (!string.Equals(registration.RegistrationKind, "type", StringComparison.Ordinal) ||
                string.IsNullOrWhiteSpace(registration.SubjectId) ||
                string.IsNullOrWhiteSpace(registration.DisplayName) ||
                string.IsNullOrWhiteSpace(registration.DefinitionSubjectId))
            {
                return null;
            }

            var displayName = registration.DisplayName!;
            uint valueSizeBytes;
            uint vectorLaneKind;
            uint scalarKind;
            switch (displayName)
            {
                case "System.Byte":
                    valueSizeBytes = 1u;
                    vectorLaneKind = 1u;
                    scalarKind = 2u;
                    break;
                case "System.SByte":
                    valueSizeBytes = 1u;
                    vectorLaneKind = 1u;
                    scalarKind = 1u;
                    break;
                case "System.Int16":
                    valueSizeBytes = 2u;
                    vectorLaneKind = 1u;
                    scalarKind = 1u;
                    break;
                case "System.UInt16":
                    valueSizeBytes = 2u;
                    vectorLaneKind = 1u;
                    scalarKind = 2u;
                    break;
                case "System.Int32":
                    valueSizeBytes = 4u;
                    vectorLaneKind = 1u;
                    scalarKind = 1u;
                    break;
                case "System.UInt32":
                    valueSizeBytes = 4u;
                    vectorLaneKind = 1u;
                    scalarKind = 2u;
                    break;
                case "System.Int64":
                    valueSizeBytes = 8u;
                    vectorLaneKind = 1u;
                    scalarKind = 1u;
                    break;
                case "System.UInt64":
                    valueSizeBytes = 8u;
                    vectorLaneKind = 1u;
                    scalarKind = 2u;
                    break;
                case "System.IntPtr":
                    valueSizeBytes = (uint)IntPtr.Size;
                    vectorLaneKind = 1u;
                    scalarKind = 3u;
                    break;
                case "System.UIntPtr":
                    valueSizeBytes = (uint)IntPtr.Size;
                    vectorLaneKind = 1u;
                    scalarKind = 4u;
                    break;
                case "System.Single":
                    valueSizeBytes = 4u;
                    vectorLaneKind = 2u;
                    scalarKind = 5u;
                    break;
                case "System.Double":
                    valueSizeBytes = 8u;
                    vectorLaneKind = 2u;
                    scalarKind = 5u;
                    break;
                default:
                    return null;
            }

            if (!uint.TryParse(registration.DefinitionSubjectId, out var typeToken))
            {
                var tokenText = registration.DefinitionSubjectId;
                if (tokenText.StartsWith("0x", StringComparison.OrdinalIgnoreCase))
                {
                    typeToken = Convert.ToUInt32(tokenText, 16);
                }
                else
                {
                    return null;
                }
            }

            return new CodeRegistrationTypeCapabilityEntry
            {
                SubjectId = registration.SubjectId,
                TypeToken = typeToken,
                CapabilityBits = 1u,
                ValueSizeBytes = valueSizeBytes,
                VectorWidthBytes = 16u,
                VectorLaneCount = valueSizeBytes == 0u ? 0u : (16u / valueSizeBytes),
                VectorLaneKind = vectorLaneKind,
                ScalarKind = scalarKind,
            };
        }

        return metadataRegistration.Registrations
            .Select(TryCreate)
            .Where(static item => item is not null)
            .Cast<CodeRegistrationTypeCapabilityEntry>()
            .GroupBy(static item => item.SubjectId, StringComparer.Ordinal)
            .Select(static group => group.First())
            .ToList();
    }

}
