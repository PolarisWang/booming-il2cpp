using Chaos.IL2CPP.Contracts;

namespace Chaos.IL2CPP.Generator;

internal sealed class GenericCapabilityMatrixBuilder
{
    private sealed record FamilyBudgetTemplate(
        string FamilyId,
        string DisplayName,
        int BudgetLimit,
        Func<GenericCapabilityMatrixEntryArtifact, bool>? EntryMatch,
        Func<GenericCapabilityBoundaryCaseArtifact, bool>? BoundaryMatch);

    private sealed record GenericAuthorityObservation(
        string AuthoritySource,
        string? OpenDefinitionSubjectId,
        string? SharedGenericBodyId,
        string? InstantiationStubId,
        bool HasRuntimeGenericContextAuthority,
        string? StatusReasonCode,
        GenericSupportKind? SupportKindCode,
        GenericSpecializationKind? SpecializationKindCode);

    private static readonly IReadOnlyList<string> CanonicalHotUpdateModes =
    [
        "HotUpdateNone",
        "HotUpdateMetadataAugmented",
        "HotUpdateGenericSharing",
    ];

    private static readonly IReadOnlyList<FamilyBudgetTemplate> FamilyBudgetTemplates =
    [
        new("ClosedGenericType", "Closed Generic Type", 256, entry => entry.FamilyKindCode == GenericDemandFamilyKind.ClosedGenericType, null),
        new("ClosedMethodOnGenericType", "Closed Method On Generic Type", 512, entry => entry.FamilyKindCode == GenericDemandFamilyKind.ClosedMethodOnGenericType, null),
        new("ClosedGenericMethod", "Closed Generic Method", 512, entry => entry.FamilyKindCode == GenericDemandFamilyKind.ClosedGenericMethod, null),
        new("ClosedGenericMethodOnGenericType", "Closed Generic Method On Generic Type", 512, entry => entry.FamilyKindCode == GenericDemandFamilyKind.ClosedGenericMethodOnGenericType, null),
        new("CrossAssemblyBoundary", "Cross Assembly Boundary", 512, entry => entry.IsCrossAssembly, null),
        new("DispatchBoundary", "Dispatch Boundary", 128, null, boundaryCase => string.Equals(boundaryCase.BoundaryKind, "DispatchBoundary", StringComparison.Ordinal)),
        new("ReflectionBoundary", "Reflection Boundary", 64, null, boundaryCase => string.Equals(boundaryCase.BoundaryKind, "ReflectionBoundary", StringComparison.Ordinal)),
        new("HotUpdateBoundary", "HotUpdate Boundary", 64, null, boundaryCase => string.Equals(boundaryCase.BoundaryKind, "HotUpdateBoundary", StringComparison.Ordinal)),
        new("AsyncTaskFamily", "Async Task Family", 256, entry => ContainsAny(entry, "System.Threading.Tasks.Task<", "System.Threading.Tasks.Task`1"), null),
        new("AsyncValueTaskFamily", "Async ValueTask Family", 256, entry => ContainsAny(entry, "System.Threading.Tasks.ValueTask<", "System.Threading.Tasks.ValueTask`1"), null),
        new("AsyncBuilderFamily", "Async Builder Family", 128, entry => ContainsAny(entry, "AsyncTaskMethodBuilder<", "AsyncTaskMethodBuilder`1", "AsyncValueTaskMethodBuilder<", "AsyncValueTaskMethodBuilder`1"), null),
        new("AsyncAwaiterFamily", "Async Awaiter Family", 128, entry => ContainsAny(entry, "TaskAwaiter<", "ValueTaskAwaiter<", "TaskAwaiter`1", "ValueTaskAwaiter`1"), null),
        new("PointerByRefFamily", "Pointer And ByRef Family", 128, entry => ContainsAny(entry, "&", "*"), null),
        new("RefStructFamily", "Ref Struct Family", 128, entry => ContainsAny(entry, "System.Span<", "System.ReadOnlySpan<", "System.Span`1", "System.ReadOnlySpan`1"), null),
    ];

    public GenericCapabilityMatrixArtifact Build(
        string ownerSubjectId,
        string entrySubjectId,
        GenericInstantiationDemandGraphModel genericInstantiationDemandGraph,
        AotCoreIrArtifact aotCoreIr,
        SupplementalMetadataTemplateArtifact supplementalMetadataTemplate)
    {
        ArgumentException.ThrowIfNullOrWhiteSpace(ownerSubjectId);
        ArgumentNullException.ThrowIfNull(genericInstantiationDemandGraph);
        ArgumentNullException.ThrowIfNull(aotCoreIr);
        ArgumentNullException.ThrowIfNull(supplementalMetadataTemplate);

        var authorityLookup = BuildAuthorityLookup(aotCoreIr, supplementalMetadataTemplate);
        var leaks = new List<GenericCapabilityMatrixLeakArtifact>();
        var entries = genericInstantiationDemandGraph.Demands
            .Select(demand => BuildEntry(demand, authorityLookup, leaks))
            .OrderBy(entry => entry.OwningAssemblyName, StringComparer.Ordinal)
            .ThenBy(entry => entry.SubjectKind, StringComparer.Ordinal)
            .ThenBy(entry => entry.SubjectId, StringComparer.Ordinal)
            .ToList();
        var boundaryCases = BuildBoundaryCases(aotCoreIr);

        return new GenericCapabilityMatrixArtifact
        {
            OwnerSubjectId = ownerSubjectId,
            EntrySubjectId = entrySubjectId ?? string.Empty,
            HotUpdateModes = CanonicalHotUpdateModes,
            Gates = BuildGateStatus(entries, leaks),
            FamilyBudgets = BuildFamilyBudgets(entries, boundaryCases),
            BoundaryCases = boundaryCases,
            Entries = entries,
        };
    }

    private static GenericCapabilityMatrixEntryArtifact BuildEntry(
        GenericInstantiationDemandModel demand,
        IReadOnlyDictionary<string, GenericAuthorityObservation> authorityLookup,
        ICollection<GenericCapabilityMatrixLeakArtifact> leaks)
    {
        var expectedOpenDefinitionSubjectId = demand.InstantiationKey.DefinitionSubjectId;
        var expectedSharedGenericBodyId = ManagedNaming.CreateSharedGenericBodyId(demand.InstantiationKey).Value;
        var expectedInstantiationStubId = ManagedNaming.CreateInstantiationStubId(demand.InstantiationKey).Value;
        authorityLookup.TryGetValue(demand.SubjectId, out var observation);

        ValidateObservedAuthority(
            demand,
            observation,
            expectedOpenDefinitionSubjectId,
            expectedSharedGenericBodyId,
            expectedInstantiationStubId,
            leaks);

        if (observation is null)
        {
            return new GenericCapabilityMatrixEntryArtifact
            {
                SubjectKind = demand.SubjectKind,
                SubjectId = demand.SubjectId,
                DefinitionSubjectId = demand.DefinitionSubjectId,
                DemandSourceKind = demand.DemandSourceKind,
                RequestingAssemblyName = demand.RequestingAssemblyName,
                OwningAssemblyName = demand.OwningAssemblyName,
                FamilyKindCode = demand.FamilyKindCode,
                ContextKindCode = demand.InstantiationKey.ContextKind,
                SupportKindCode = demand.SupportKindCode,
                SpecializationKindCode = demand.SpecializationKindCode,
                IsCrossAssembly = demand.IsCrossAssembly,
                StatusReasonCode = $"loader-demand:{demand.DemandSourceKind}",
                AuthoritySource = "demand-derived",
                OpenDefinitionSubjectId = expectedOpenDefinitionSubjectId,
                SharedGenericBodyId = expectedSharedGenericBodyId,
                InstantiationStubId = expectedInstantiationStubId,
                HasOpenDefinitionAuthority = !string.IsNullOrWhiteSpace(expectedOpenDefinitionSubjectId),
                HasSharedBodyAuthority = !string.IsNullOrWhiteSpace(expectedSharedGenericBodyId),
                HasInstantiationStubAuthority = !string.IsNullOrWhiteSpace(expectedInstantiationStubId),
                HasRuntimeGenericContextAuthority = true,
            };
        }

        var statusReasonCode = observation.StatusReasonCode ?? string.Empty;
        var authoritySource = observation.AuthoritySource;
        var openDefinitionSubjectId = observation.OpenDefinitionSubjectId ?? string.Empty;
        var sharedGenericBodyId = observation.SharedGenericBodyId ?? string.Empty;
        var instantiationStubId = observation.InstantiationStubId ?? string.Empty;
        var hasOpenDefinitionAuthority = !string.IsNullOrWhiteSpace(observation.OpenDefinitionSubjectId);
        var hasSharedBodyAuthority = !string.IsNullOrWhiteSpace(observation.SharedGenericBodyId);
        var hasInstantiationStubAuthority = !string.IsNullOrWhiteSpace(observation.InstantiationStubId);
        var hasRuntimeGenericContextAuthority = observation.HasRuntimeGenericContextAuthority;

        return new GenericCapabilityMatrixEntryArtifact
        {
            SubjectKind = demand.SubjectKind,
            SubjectId = demand.SubjectId,
            DefinitionSubjectId = demand.DefinitionSubjectId,
            DemandSourceKind = demand.DemandSourceKind,
            RequestingAssemblyName = demand.RequestingAssemblyName,
            OwningAssemblyName = demand.OwningAssemblyName,
            FamilyKindCode = demand.FamilyKindCode,
            ContextKindCode = demand.InstantiationKey.ContextKind,
            SupportKindCode = demand.SupportKindCode,
            SpecializationKindCode = demand.SpecializationKindCode,
            IsCrossAssembly = demand.IsCrossAssembly,
            StatusReasonCode = statusReasonCode,
            AuthoritySource = authoritySource,
            OpenDefinitionSubjectId = openDefinitionSubjectId,
            SharedGenericBodyId = sharedGenericBodyId,
            InstantiationStubId = instantiationStubId,
            HasOpenDefinitionAuthority = hasOpenDefinitionAuthority,
            HasSharedBodyAuthority = hasSharedBodyAuthority,
            HasInstantiationStubAuthority = hasInstantiationStubAuthority,
            HasRuntimeGenericContextAuthority = hasRuntimeGenericContextAuthority,
        };
    }

    private static IReadOnlyList<GenericCapabilityFamilyBudgetArtifact> BuildFamilyBudgets(
        IReadOnlyList<GenericCapabilityMatrixEntryArtifact> entries,
        IReadOnlyList<GenericCapabilityBoundaryCaseArtifact> boundaryCases)
    {
        return FamilyBudgetTemplates
            .Select(template => new GenericCapabilityFamilyBudgetArtifact
            {
                FamilyId = template.FamilyId,
                DisplayName = template.DisplayName,
                BudgetLimit = template.BudgetLimit,
                ObservedCount =
                    (template.EntryMatch is null ? 0 : entries.Count(entry => template.EntryMatch(entry))) +
                    (template.BoundaryMatch is null ? 0 : boundaryCases.Count(boundaryCase => template.BoundaryMatch(boundaryCase))),
            })
            .ToList();
    }

    private static IReadOnlyList<GenericCapabilityBoundaryCaseArtifact> BuildBoundaryCases(AotCoreIrArtifact aotCoreIr)
    {
        var boundaryCases = new List<GenericCapabilityBoundaryCaseArtifact>();

        foreach (var method in aotCoreIr.Methods)
        {
            foreach (var instruction in method.Instructions)
            {
                if (TryCreateBoundaryCase("DispatchBoundary", method, instruction, out var dispatchBoundaryCase))
                {
                    boundaryCases.Add(dispatchBoundaryCase!);
                }

                if (TryCreateBoundaryCase("ReflectionBoundary", method, instruction, out var reflectionBoundaryCase))
                {
                    boundaryCases.Add(reflectionBoundaryCase!);
                }

                if (TryCreateBoundaryCase("HotUpdateBoundary", method, instruction, out var hotUpdateBoundaryCase))
                {
                    boundaryCases.Add(hotUpdateBoundaryCase!);
                }
            }
        }

        return boundaryCases
            .Distinct()
            .OrderBy(boundaryCase => boundaryCase.SourceMethodSubjectId, StringComparer.Ordinal)
            .ThenBy(boundaryCase => boundaryCase.IlOffset)
            .ThenBy(boundaryCase => boundaryCase.BoundaryKind, StringComparer.Ordinal)
            .ThenBy(boundaryCase => boundaryCase.TargetSubjectId, StringComparer.Ordinal)
            .ThenBy(boundaryCase => boundaryCase.EvidenceKind, StringComparer.Ordinal)
            .ToList();
    }

    private static bool TryCreateBoundaryCase(
        string boundaryKind,
        AotCoreIrMethodArtifact method,
        AotCoreIrInstructionArtifact instruction,
        out GenericCapabilityBoundaryCaseArtifact? boundaryCase)
    {
        ArgumentException.ThrowIfNullOrWhiteSpace(boundaryKind);
        ArgumentNullException.ThrowIfNull(method);
        ArgumentNullException.ThrowIfNull(instruction);

        boundaryCase = null;

        string? targetSubjectId;
        string? evidenceKind;

        switch (boundaryKind)
        {
            case "DispatchBoundary":
                if (!string.Equals(instruction.Op, "callvirt", StringComparison.Ordinal))
                {
                    return false;
                }

                evidenceKind = instruction.DispatchKindCode switch
                {
                    HybridDispatchKind.Virtual => "virtual-callvirt",
                    HybridDispatchKind.ExternalRuntime => "external-runtime-callvirt",
                    _ => null,
                };

                if (evidenceKind is null)
                {
                    return false;
                }

                targetSubjectId = instruction.TargetReference?.SubjectId ?? instruction.Callee;
                break;

            case "ReflectionBoundary":
                targetSubjectId = instruction.TargetReference?.SubjectId ?? instruction.Callee;
                if (!IsReflectionBoundaryTarget(targetSubjectId))
                {
                    return false;
                }

                evidenceKind = $"{instruction.Op}-reflection-target";
                break;

            case "HotUpdateBoundary":
                targetSubjectId = instruction.TargetReference?.SubjectId ?? instruction.Callee;
                if (!ContainsAny(targetSubjectId, "HotUpdate"))
                {
                    return false;
                }

                evidenceKind = $"{instruction.Op}-hotupdate-target";
                break;

            default:
                throw new NotSupportedException(
                    $"unsupported generic capability boundary kind '{boundaryKind}'.");
        }

        if (string.IsNullOrWhiteSpace(targetSubjectId))
        {
            return false;
        }

        boundaryCase = CreateBoundaryCase(
            boundaryKind,
            method.SubjectId,
            instruction.IlOffset,
            targetSubjectId,
            evidenceKind);
        return true;
    }

    private static GenericCapabilityBoundaryCaseArtifact CreateBoundaryCase(
        string boundaryKind,
        string sourceMethodSubjectId,
        int ilOffset,
        string targetSubjectId,
        string evidenceKind)
    {
        return new GenericCapabilityBoundaryCaseArtifact
        {
            BoundaryKind = boundaryKind,
            SourceMethodSubjectId = sourceMethodSubjectId,
            IlOffset = ilOffset,
            TargetSubjectId = targetSubjectId,
            EvidenceKind = evidenceKind,
            Status = "observed",
        };
    }

    private static bool IsReflectionBoundaryTarget(string? targetSubjectId)
    {
        return ContainsAny(
            targetSubjectId,
            "System.Reflection",
            "MethodInfo",
            "ConstructorInfo",
            "PropertyInfo",
            "FieldInfo",
            "System.Type::",
            "Activator");
    }

    private static GenericCapabilityMatrixGateStatus BuildGateStatus(
        IReadOnlyList<GenericCapabilityMatrixEntryArtifact> entries,
        IReadOnlyList<GenericCapabilityMatrixLeakArtifact> leaks)
    {
        var unsupportedLeakCount = leaks.Count(leak => string.Equals(leak.LeakKind, "unsupportedAuthorityLeak", StringComparison.Ordinal));
        var missingAuthorityCount = entries.Count(entry =>
            !entry.HasOpenDefinitionAuthority ||
            !entry.HasSharedBodyAuthority ||
            !entry.HasInstantiationStubAuthority ||
            !entry.HasRuntimeGenericContextAuthority);
        var nonCanonicalHotUpdateNameCount = CanonicalHotUpdateModes.Count(mode =>
            mode.Contains("HybridCLR", StringComparison.OrdinalIgnoreCase) ||
            !mode.StartsWith("HotUpdate", StringComparison.Ordinal));

        return new GenericCapabilityMatrixGateStatus
        {
            Status = unsupportedLeakCount == 0 && missingAuthorityCount == 0 && nonCanonicalHotUpdateNameCount == 0 && leaks.Count == 0
                ? "ok"
                : "fail",
            UnsupportedLeakCount = unsupportedLeakCount,
            MissingAuthorityCount = missingAuthorityCount,
            NonCanonicalHotUpdateNameCount = nonCanonicalHotUpdateNameCount,
            Leaks = leaks.ToList(),
        };
    }

    private static IReadOnlyDictionary<string, GenericAuthorityObservation> BuildAuthorityLookup(
        AotCoreIrArtifact aotCoreIr,
        SupplementalMetadataTemplateArtifact supplementalMetadataTemplate)
    {
        var authorityLookup = new Dictionary<string, GenericAuthorityObservation>(StringComparer.Ordinal);

        foreach (var method in aotCoreIr.Methods)
        {
            TryRegisterObservation(
                authorityLookup,
                method.SubjectId,
                new GenericAuthorityObservation(
                    "aot-core-ir",
                    method.OpenDefinitionSubjectId,
                    method.SharedGenericBodyId?.Value,
                    method.InstantiationStubId?.Value,
                    method.RuntimeGenericContext is not null,
                    method.RuntimeGenericContext?.StatusReasonCode,
                    method.RuntimeGenericContext?.SupportKindCode,
                    method.RuntimeGenericContext?.SpecializationKindCode));

            foreach (var instruction in method.Instructions)
            {
                if (instruction.TargetReference is null)
                {
                    continue;
                }

                TryRegisterObservation(
                    authorityLookup,
                    instruction.TargetReference.SubjectId,
                    new GenericAuthorityObservation(
                        "aot-core-ir",
                        instruction.TargetReference.OpenDefinitionSubjectId,
                        instruction.TargetReference.SharedGenericBodyId?.Value,
                        instruction.TargetReference.InstantiationStubId?.Value,
                        instruction.TargetReference.RuntimeGenericContext is not null,
                        instruction.TargetReference.RuntimeGenericContext?.StatusReasonCode,
                        instruction.TargetReference.RuntimeGenericContext?.SupportKindCode,
                        instruction.TargetReference.RuntimeGenericContext?.SpecializationKindCode));
            }
        }

        foreach (var typeEntry in supplementalMetadataTemplate.RegisteredTypes)
        {
            TryRegisterObservation(
                authorityLookup,
                typeEntry.SubjectId,
                CreateSupplementalMetadataObservation(typeEntry.RuntimeGenericContext));
        }

        foreach (var methodEntry in supplementalMetadataTemplate.RegisteredMethods)
        {
            TryRegisterObservation(
                authorityLookup,
                methodEntry.SubjectId,
                CreateSupplementalMetadataObservation(methodEntry.RuntimeGenericContext));
        }

        return authorityLookup;
    }

    private static void TryRegisterObservation(
        IDictionary<string, GenericAuthorityObservation> authorityLookup,
        string subjectId,
        GenericAuthorityObservation observation)
    {
        if (!HasAnyAuthority(observation))
        {
            return;
        }

        if (authorityLookup.TryGetValue(subjectId, out var existingObservation))
        {
            EnsureEquivalentObservation(subjectId, existingObservation, observation);
            if (ShouldPreferAdditionalObservation(existingObservation, observation))
            {
                authorityLookup[subjectId] = observation;
            }
            return;
        }

        authorityLookup[subjectId] = observation;
    }

    private static GenericAuthorityObservation CreateSupplementalMetadataObservation(
        RuntimeGenericContextArtifact? runtimeGenericContext)
    {
        return new GenericAuthorityObservation(
            "supplemental-metadata",
            runtimeGenericContext?.InstantiationKey.DefinitionSubjectId,
            runtimeGenericContext?.SharedGenericBodyId.Value,
            runtimeGenericContext?.InstantiationStubId.Value,
            runtimeGenericContext is not null,
            runtimeGenericContext?.StatusReasonCode,
            runtimeGenericContext?.SupportKindCode,
            runtimeGenericContext?.SpecializationKindCode);
    }

    private static bool ShouldPreferAdditionalObservation(
        GenericAuthorityObservation existingObservation,
        GenericAuthorityObservation additionalObservation)
    {
        return !string.Equals(existingObservation.AuthoritySource, "aot-core-ir", StringComparison.Ordinal) &&
               string.Equals(additionalObservation.AuthoritySource, "aot-core-ir", StringComparison.Ordinal);
    }

    private static bool HasAnyAuthority(GenericAuthorityObservation observation)
    {
        return observation.HasRuntimeGenericContextAuthority ||
               !string.IsNullOrWhiteSpace(observation.OpenDefinitionSubjectId) ||
               !string.IsNullOrWhiteSpace(observation.SharedGenericBodyId) ||
               !string.IsNullOrWhiteSpace(observation.InstantiationStubId);
    }

    private static void ValidateObservedAuthority(
        GenericInstantiationDemandModel demand,
        GenericAuthorityObservation? observation,
        string expectedOpenDefinitionSubjectId,
        string expectedSharedGenericBodyId,
        string expectedInstantiationStubId,
        ICollection<GenericCapabilityMatrixLeakArtifact> leaks)
    {
        if (observation is null)
        {
            return;
        }

        if (demand.SupportKindCode == GenericSupportKind.Forbidden)
        {
            leaks.Add(new GenericCapabilityMatrixLeakArtifact
            {
                SubjectId = demand.SubjectId,
                LeakKind = "unsupportedAuthorityLeak",
                Reason = $"forbidden generic demand '{demand.SubjectId}' surfaced execution authority",
            });
            return;
        }

        if (!string.Equals(observation.OpenDefinitionSubjectId, expectedOpenDefinitionSubjectId, StringComparison.Ordinal) ||
            !string.Equals(observation.SharedGenericBodyId, expectedSharedGenericBodyId, StringComparison.Ordinal) ||
            !string.Equals(observation.InstantiationStubId, expectedInstantiationStubId, StringComparison.Ordinal) ||
            observation.SupportKindCode != demand.SupportKindCode ||
            observation.SpecializationKindCode != demand.SpecializationKindCode)
        {
            leaks.Add(new GenericCapabilityMatrixLeakArtifact
            {
                SubjectId = demand.SubjectId,
                LeakKind = "authorityMismatch",
                Reason = $"observed authority for '{demand.SubjectId}' does not match loader demand authority",
            });
        }
    }

    private static void EnsureEquivalentObservation(
        string subjectId,
        GenericAuthorityObservation existingObservation,
        GenericAuthorityObservation additionalObservation)
    {
        if (string.Equals(existingObservation.OpenDefinitionSubjectId, additionalObservation.OpenDefinitionSubjectId, StringComparison.Ordinal) &&
            string.Equals(existingObservation.SharedGenericBodyId, additionalObservation.SharedGenericBodyId, StringComparison.Ordinal) &&
            string.Equals(existingObservation.InstantiationStubId, additionalObservation.InstantiationStubId, StringComparison.Ordinal) &&
            existingObservation.HasRuntimeGenericContextAuthority == additionalObservation.HasRuntimeGenericContextAuthority &&
            string.Equals(existingObservation.StatusReasonCode, additionalObservation.StatusReasonCode, StringComparison.Ordinal) &&
            existingObservation.SupportKindCode == additionalObservation.SupportKindCode &&
            existingObservation.SpecializationKindCode == additionalObservation.SpecializationKindCode)
        {
            return;
        }

        throw new InvalidOperationException(
            $"conflicting generic authority observations detected for '{subjectId}' while building generic capability matrix.");
    }

    private static bool ContainsAny(GenericCapabilityMatrixEntryArtifact entry, params string[] fragments)
    {
        return fragments.Any(fragment =>
            ContainsAny(entry.SubjectId, fragment) ||
            ContainsAny(entry.DefinitionSubjectId, fragment) ||
            ContainsAny(entry.OpenDefinitionSubjectId, fragment));
    }

    private static bool ContainsAny(string? value, params string[] fragments)
    {
        if (string.IsNullOrWhiteSpace(value))
        {
            return false;
        }

        return fragments.Any(fragment => value.Contains(fragment, StringComparison.Ordinal));
    }
}
