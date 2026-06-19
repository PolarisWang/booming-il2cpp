using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Text.Json;
using Chaos.IL2CPP.Contracts;
using Scriban;
using Scriban.Runtime;

namespace Chaos.IL2CPP.Generator;

public sealed partial class NativeReferenceLoweringPlanner
{

    private static bool MatchesDelegateClosedTargetRelayCandidate(
        IReadOnlySet<string> worldCapabilities,
        IReadOnlyList<string> entryCapabilities)
    {
        return entryCapabilities.Contains("requires-console-string-output", StringComparer.Ordinal) &&
               worldCapabilities.Contains("requires-delegate-construction") &&
               worldCapabilities.Contains("requires-delegate-invoke");
    }



    private static bool MatchesExceptionThrowCatchFinallyCandidate(
        IReadOnlySet<string> worldCapabilities,
        IReadOnlyList<string> entryCapabilities)
    {
        return entryCapabilities.Contains("requires-console-string-output", StringComparer.Ordinal) &&
               worldCapabilities.Contains("requires-exception-throw") &&
               worldCapabilities.Contains("requires-exception-handler") &&
               worldCapabilities.Contains("requires-finally");
    }



    private static bool MatchesNestedExceptionCandidate(
        LinkedWorldModel linkedWorld,
        IReadOnlySet<string> worldCapabilities,
        IReadOnlyList<string> entryCapabilities)
    {
        return string.Equals(linkedWorld.Assembly.Name, "NestedExceptionProof", StringComparison.Ordinal) &&
               entryCapabilities.Contains("requires-console-string-output", StringComparer.Ordinal) &&
               worldCapabilities.Contains("requires-exception-throw") &&
               worldCapabilities.Contains("requires-exception-handler") &&
               worldCapabilities.Contains("requires-finally") &&
               worldCapabilities.Contains("requires-nested-exception-handler");
    }



    private static bool MatchesStaticForwarderCapturedGetterCandidate(
        IReadOnlyDictionary<string, MethodShapeModel> methodShapes,
        IReadOnlySet<string> worldCapabilities,
        IReadOnlyList<string> entryCapabilities)
    {
        return entryCapabilities.Contains("requires-console-string-output", StringComparer.Ordinal) &&
               worldCapabilities.Contains("uses-instance-field-state") &&
               methodShapes.Values.Any(shape => string.Equals(shape.MethodRole, "static-forwarder", StringComparison.Ordinal)) &&
               methodShapes.Values.Any(shape => string.Equals(shape.MethodRole, "constructor", StringComparison.Ordinal)) &&
               methodShapes.Values.Any(shape => string.Equals(shape.MethodRole, "instance-field-getter", StringComparison.Ordinal)) &&
               !worldCapabilities.Contains("requires-imported-call") &&
               !worldCapabilities.Contains("requires-closed-type-member-query");
    }



    private static bool MatchesAsyncAwaitIntCandidate(
        LinkedWorldModel linkedWorld,
        IReadOnlySet<string> worldCapabilities,
        IReadOnlyList<string> entryCapabilities)
    {
        return string.Equals(linkedWorld.Assembly.Name, "AsyncAwaitProof", StringComparison.Ordinal) &&
               entryCapabilities.Contains("requires-console-string-output", StringComparer.Ordinal) &&
               worldCapabilities.Contains("requires-async-state-machine") &&
               worldCapabilities.Contains("requires-task-awaiter");
    }



    private static bool MatchesThreadingThreadStaticMonitorCandidate(
        LinkedWorldModel linkedWorld,
        IReadOnlySet<string> worldCapabilities)
    {
        return string.Equals(linkedWorld.Assembly.Name, "ThreadingProof", StringComparison.Ordinal) &&
               worldCapabilities.Contains("requires-thread-static-storage") &&
               worldCapabilities.Contains("requires-monitor-enter-exit");
    }



    private static bool MatchesDispatchVirtualInstanceMessageCandidate(
        LinkedWorldModel linkedWorld,
        IReadOnlyList<string> entryCapabilities)
    {
        if (!entryCapabilities.Contains("uses-virtual-call-site", StringComparer.Ordinal))
        {
            return false;
        }

        var entryMethod = linkedWorld.Methods.FirstOrDefault(method =>
            string.Equals(method.SubjectId, linkedWorld.EntryPointSubjectId, StringComparison.Ordinal));
        var entryInstructions = TryGetSingleBlockInstructions(entryMethod);
        if (entryInstructions is null || !IsConstructorThenInstanceCallProofEntryPointShape(entryInstructions))
        {
            return false;
        }

        var constructorSubjectId = entryInstructions[1].Callee;
        var declaredMethodSubjectId = entryInstructions[2].Callee;
        return !string.IsNullOrWhiteSpace(constructorSubjectId)
            && !string.IsNullOrWhiteSpace(declaredMethodSubjectId)
            && !string.Equals(
                GetDeclaringTypeSubjectId(constructorSubjectId),
                GetDeclaringTypeSubjectId(declaredMethodSubjectId),
                StringComparison.Ordinal);
    }



    private static bool MatchesInterfaceDispatchMessageCandidate(
        LinkedWorldModel linkedWorld,
        IReadOnlyList<string> entryCapabilities)
    {
        if (!entryCapabilities.Contains("uses-interface-call-site", StringComparer.Ordinal))
        {
            return false;
        }

        var entryMethod = linkedWorld.Methods.FirstOrDefault(method =>
            string.Equals(method.SubjectId, linkedWorld.EntryPointSubjectId, StringComparison.Ordinal));
        var entryInstructions = TryGetSingleBlockInstructions(entryMethod);
        if (entryInstructions is null || !IsConstructorThenInstanceCallProofEntryPointShape(entryInstructions))
        {
            return false;
        }

        var declaredMethodSubjectId = entryInstructions[2].Callee;
        return !string.IsNullOrWhiteSpace(declaredMethodSubjectId) &&
               IsInterfaceDispatchTarget(linkedWorld.Types, declaredMethodSubjectId!);
    }



    private static bool MatchesArrayBoxingReferenceArrayCandidate(
        LinkedWorldModel linkedWorld,
        IReadOnlyList<string> entryCapabilities)
    {
        if (!entryCapabilities.Contains("requires-console-string-output", StringComparer.Ordinal) ||
            !entryCapabilities.Contains("requires-array-allocation", StringComparer.Ordinal) ||
            !entryCapabilities.Contains("requires-boxing", StringComparer.Ordinal))
        {
            return false;
        }

        var entryMethod = linkedWorld.Methods.FirstOrDefault(method =>
            string.Equals(method.SubjectId, linkedWorld.EntryPointSubjectId, StringComparison.Ordinal));
        var entryInstructions = TryGetSingleBlockInstructions(entryMethod);
        return entryInstructions is not null && IsArrayBoxingReferenceArrayEntryPointShape(entryInstructions);
    }



    private static bool MatchesArrayCopyReferenceArrayCandidate(
        LinkedWorldModel linkedWorld,
        IReadOnlyList<string> entryCapabilities)
    {
        if (!entryCapabilities.Contains("requires-console-string-output", StringComparer.Ordinal) ||
            !entryCapabilities.Contains("requires-array-allocation", StringComparer.Ordinal) ||
            entryCapabilities.Contains("requires-boxing", StringComparer.Ordinal))
        {
            return false;
        }

        var entryMethod = linkedWorld.Methods.FirstOrDefault(method =>
            string.Equals(method.SubjectId, linkedWorld.EntryPointSubjectId, StringComparison.Ordinal));
        var entryInstructions = TryGetSingleBlockInstructions(entryMethod);
        return entryInstructions is not null && IsArrayCopyReferenceArrayEntryPointShape(entryInstructions);
    }



    private static bool MatchesArrayReverseReferenceArrayCandidate(
        LinkedWorldModel linkedWorld,
        IReadOnlyList<string> entryCapabilities)
    {
        if (!entryCapabilities.Contains("requires-console-string-output", StringComparer.Ordinal) ||
            !entryCapabilities.Contains("requires-array-allocation", StringComparer.Ordinal) ||
            entryCapabilities.Contains("requires-boxing", StringComparer.Ordinal))
        {
            return false;
        }

        var entryMethod = linkedWorld.Methods.FirstOrDefault(method =>
            string.Equals(method.SubjectId, linkedWorld.EntryPointSubjectId, StringComparison.Ordinal));
        var entryInstructions = TryGetSingleBlockInstructions(entryMethod);
        return entryInstructions is not null && IsArrayReverseReferenceArrayEntryPointShape(entryInstructions);
    }



    private static bool MatchesArrayClearReferenceArrayCandidate(
        LinkedWorldModel linkedWorld,
        IReadOnlyList<string> entryCapabilities)
    {
        if (!entryCapabilities.Contains("requires-console-string-output", StringComparer.Ordinal) ||
            !entryCapabilities.Contains("requires-array-allocation", StringComparer.Ordinal) ||
            entryCapabilities.Contains("requires-boxing", StringComparer.Ordinal))
        {
            return false;
        }

        var entryMethod = linkedWorld.Methods.FirstOrDefault(method =>
            string.Equals(method.SubjectId, linkedWorld.EntryPointSubjectId, StringComparison.Ordinal));
        var entryInstructions = TryGetSingleBlockInstructions(entryMethod);
        return entryInstructions is not null && IsArrayClearReferenceArrayEntryPointShape(entryInstructions);
    }



    private static bool MatchesCapturedStateInstanceMessageCandidate(
        IReadOnlyDictionary<string, MethodShapeModel> methodShapes,
        IReadOnlySet<string> worldCapabilities,
        IReadOnlyList<string> entryCapabilities)
    {
        return entryCapabilities.Contains("requires-console-string-output", StringComparer.Ordinal) &&
               worldCapabilities.Contains("uses-instance-field-state") &&
               methodShapes.Values.Any(shape => string.Equals(shape.MethodRole, "constructor", StringComparison.Ordinal)) &&
               methodShapes.Values.Any(shape => string.Equals(shape.MethodRole, "instance-method", StringComparison.Ordinal)) &&
               !methodShapes.Values.Any(shape => string.Equals(shape.MethodRole, "static-forwarder", StringComparison.Ordinal)) &&
               !worldCapabilities.Contains("requires-imported-call") &&
               !worldCapabilities.Contains("requires-closed-type-member-query");
    }



    private static void RequireDependencyReason(
        IReadOnlySet<string> dependencyReasons,
        string requiredReason,
        NativeReferencePlanKind loweringFamily,
        string entryPointSubjectId)
    {
        if (!dependencyReasons.Contains(requiredReason))
        {
            throw new InvalidOperationException(
                $"CodeGen lowering family '{NativeReferenceProofCatalog.Stringify(loweringFamily)}' for '{entryPointSubjectId}' is missing dependency reason '{requiredReason}'");
        }
    }



    private static NativeReferencePlanKind MapLegacyPlanKind(string legacyPlanKind)
    {
        return legacyPlanKind switch
        {
            "asyncAwaitIntMinimal" => NativeReferencePlanKind.ManagedAsyncAwaitIntMinimal,
            "threadingThreadStaticMonitorMinimal" => NativeReferencePlanKind.ManagedThreadingThreadStaticMonitorMinimal,
            "interfaceDispatchMessage" => NativeReferencePlanKind.ManagedInterfaceDispatchMessageMinimal,
            "dispatchVirtualInstanceMessage" => NativeReferencePlanKind.ManagedDispatchVirtualInstanceMessageMinimal,
            "constructorThenInstanceCall" => NativeReferencePlanKind.ManagedObjectCapturedStateInstanceMessageMinimal,
            "staticCallCtorGetter" => NativeReferencePlanKind.ManagedGenericStaticForwarderCapturedGetterMinimal,
            "arrayReverseReferenceArray" => NativeReferencePlanKind.ManagedArraysReverseReferenceArrayMinimal,
            "arrayClearReferenceArray" => NativeReferencePlanKind.ManagedArraysClearReferenceArrayMinimal,
            "arrayCopyReferenceArray" => NativeReferencePlanKind.ManagedArraysCopyReferenceArrayMinimal,
            "arrayBoxingReferenceArray" => NativeReferencePlanKind.ManagedArraysBoxingReferenceArrayBoxedIntMinimal,
            "delegateClosedTargetRelayMinimal" => NativeReferencePlanKind.DelegateClosedTargetRelayMinimal,
            "nestedExceptionThrowCatchFinallyMinimal" => NativeReferencePlanKind.NestedExceptionThrowCatchFinallyMinimal,
            "exceptionThrowCatchFinallyMinimal" => NativeReferencePlanKind.ExceptionThrowCatchFinallyMinimal,
            "reflectionInteropClosureMinimal" => NativeReferencePlanKind.ReflectionInteropClosureMinimal,
            "reflectionQueryMinimal" => NativeReferencePlanKind.ReflectionClosedTypeQueryMinimal,
            "marshalingUtf8ExportMinimal" => NativeReferencePlanKind.MarshalingUtf8ExportMinimal,
            "pinvokeDllImportMinimal" => NativeReferencePlanKind.InteropPInvokeDirectCallMinimal,
            _ => throw new InvalidOperationException(
                $"unsupported legacy native-reference lowering plan kind '{legacyPlanKind}'"),
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
            $"missing semantic method shape for '{subjectId}' during lowering family selection");
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
            $"missing method capability bundle for '{subjectId}' during lowering family selection");
    }



    public NativeReferenceProofResult Generate(NativeReferenceProofRequest request)
    {
        var managedClosureRoot = Path.GetFullPath(request.ManagedClosureRootPath);
        var typedIlPath = Path.Combine(managedClosureRoot, ManagedClosureArtifactNames.TypedIlIr);
        var metadataRegistrationPath = Path.Combine(managedClosureRoot, ManagedClosureArtifactNames.MetadataRegistration);
        var closureManifestPath = Path.Combine(managedClosureRoot, ManagedClosureArtifactNames.ClosureManifest);
        var codeRegistrationPath = Path.Combine(managedClosureRoot, ManagedClosureArtifactNames.CodeRegistration);

        var typedIl = LoadRequiredJson<TypedIlIrArtifact>(typedIlPath);
        var metadataRegistration = LoadRequiredJson<MetadataRegistrationArtifact>(metadataRegistrationPath);
        var closureManifest = LoadRequiredJson<ManagedClosureManifestArtifact>(closureManifestPath);
        var codeRegistration = LoadRequiredJson<CodeRegistrationArtifact>(codeRegistrationPath);

        var methodPointers = codeRegistration.Modules
            .SelectMany(module => module.Registrations)
            .Where(registration => string.Equals(registration.RegistrationKind, "methodPointer", StringComparison.Ordinal))
            .ToList();

        var loweringPlan = CreateLoweringPlan(
            closureManifest.AssemblyName,
            closureManifest.EntrySubjectId,
            typedIl.Methods,
            metadataRegistration,
            methodPointers);
        var translationUnit = BuildGeneratedTranslationUnit(loweringPlan);
        var generatedSource = new NativeReferenceGeneratedSource
        {
            RelativePath = NativeReferenceArtifactNames.GeneratedTranslationUnit,
            Contents = translationUnit,
        };
        var codegenMetrics = NativeCodegenMetricsBuilder.Build(
            "native-reference",
            loweringPlan.PlanKind,
            [(generatedSource.RelativePath, generatedSource.Contents)]);

        var manifest = new NativeReferenceProofManifestArtifact
        {
            AssemblyName = closureManifest.AssemblyName,
            EntrySubjectId = closureManifest.EntrySubjectId,
            ManagedClosureRootPath = ManagedNaming.NormalizePathForManifest(managedClosureRoot, Environment.CurrentDirectory),
            PlanArtifactPath = NativeReferenceArtifactNames.LoweringPlan,
            GeneratedArtifacts =
            [
                new NativeReferenceGeneratedArtifactRef
                {
                    Kind = "generatedTranslationUnit",
                    Path = generatedSource.RelativePath,
                },
                new NativeReferenceGeneratedArtifactRef
                {
                    Kind = "codegenMetrics",
                    Path = NativeReferenceArtifactNames.CodegenMetrics,
                },
            ],
        };

        return new NativeReferenceProofResult
        {
            OutputRootPath = request.OutputRootPath,
            LoweringPlan = loweringPlan,
            Manifest = manifest,
            CodegenMetrics = codegenMetrics,
            GeneratedSources = [generatedSource],
        };
    }



    private static string BuildGeneratedTranslationUnit(
        NativeReferenceLoweringPlanArtifact loweringPlan)
    {
        var plan = CreateGeneratedTranslationUnitRenderPlan(loweringPlan);
        var context = new TemplateContext();
        context.PushGlobal(plan.Model);

        try
        {
            return plan.Template.Render(context);
        }
        finally
        {
            context.PopGlobal();
        }
    }



    private static GeneratedTranslationUnitRenderPlan CreateGeneratedTranslationUnitRenderPlan(
        NativeReferenceLoweringPlanArtifact loweringPlan)
    {
        return new GeneratedTranslationUnitRenderPlan(
            GetTemplateForPlan(loweringPlan.PlanKind),
            CreateTemplateModel(loweringPlan));
    }

}
