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
    private static bool TryCreateEngineLoweringPlan(
        string assemblyName,
        string entryPointSubjectId,
        out NativeReferenceLoweringPlanArtifact? loweringPlan)
    {
        loweringPlan = entryPointSubjectId switch
        {
            "EngineLogWriteLite/EngineLogWriteEntry::Run()" => CreateEngineLoweringPlan(
                assemblyName,
                entryPointSubjectId,
                planKind: NativeReferencePlanKind.EngineLogWriteMinimal,
                focusArea: "service-call",
                entrySymbol: "RunEngineLogWriteEntry",
                capabilityIds: ["engine.log.write"],
                bindingKinds: ["log-write"],
                helperNames: ["EngineLogWrite"],
                expectedOutput: "{\"kind\":\"log-write\",\"status\":\"ok\",\"artifactFileName\":\"log-write.json\",\"focusArea\":\"service-call\",\"capabilityIds\":[\"engine.log.write\"],\"message\":\"engine.log.write|minimal|ok\"}"),
            "EngineObjectHandleLite/EngineObjectHandleEntry::Run()" => CreateEngineLoweringPlan(
                assemblyName,
                entryPointSubjectId,
                planKind: NativeReferencePlanKind.EngineObjectHandleRoundtripMinimal,
                focusArea: "object-handle",
                entrySymbol: "RunEngineObjectHandleEntry",
                capabilityIds: ["engine.object.handle.create", "engine.object.handle.resolve"],
                bindingKinds: ["object-handle-create", "object-handle-resolve"],
                helperNames: ["CreateEngineObjectHandle", "ResolveEngineObjectHandle"],
                expectedOutput: "{\"kind\":\"handle-roundtrip\",\"status\":\"ok\",\"artifactFileName\":\"handle-roundtrip.json\",\"focusArea\":\"object-handle\",\"capabilityIds\":[\"engine.object.handle.create\",\"engine.object.handle.resolve\"],\"roundtrip\":\"ok\",\"identity\":\"same\"}"),
            "EngineLifecycleCallbackLite/EngineLifecycleCallbackEntry::Run()" => CreateEngineLoweringPlan(
                assemblyName,
                entryPointSubjectId,
                planKind: NativeReferencePlanKind.EngineLifecycleCallbackMinimal,
                focusArea: "lifecycle-callback",
                entrySymbol: "RunEngineLifecycleCallbackEntry",
                capabilityIds: ["engine.lifecycle.callback.register", "engine.lifecycle.dispatch", "engine.thread.main-lane"],
                bindingKinds: ["lifecycle-callback-register", "lifecycle-callback-dispatch", "main-thread-lane"],
                helperNames: ["RegisterEngineLifecycleCallback", "DispatchEngineLifecycleCallbacks", "IsMainThreadLane"],
                expectedOutput: "{\"kind\":\"lifecycle-callback\",\"status\":\"ok\",\"artifactFileName\":\"lifecycle-callback.json\",\"focusArea\":\"lifecycle-callback\",\"capabilityIds\":[\"engine.lifecycle.callback.register\",\"engine.lifecycle.dispatch\",\"engine.thread.main-lane\"],\"order\":[\"init\",\"tick\",\"shutdown\"],\"mainThread\":true}"),
            "EngineHostProof/EngineHostEntry::Run()" => CreateEngineLoweringPlan(
                assemblyName,
                entryPointSubjectId,
                planKind: NativeReferencePlanKind.EngineHostProofMinimal,
                focusArea: "host-proof",
                entrySymbol: "RunEngineHostEntry",
                capabilityIds:
                [
                    "engine.lifecycle.init",
                    "engine.lifecycle.shutdown",
                    "engine.callback.register",
                    "engine.callback.dispatch",
                    "engine.object.ownership.retain",
                    "engine.object.ownership.transfer",
                    "engine.object.ownership.release",
                ],
                bindingKinds:
                [
                    "host-init-shutdown",
                    "callback-register-dispatch",
                    "ownership-retain-transfer-release",
                ],
                helperNames:
                [
                    "engine_init",
                    "engine_register_callback",
                    "engine_dispatch_callback",
                    "engine_retain_object",
                    "engine_transfer_ownership",
                    "engine_release_object",
                    "engine_shutdown",
                ],
                expectedOutput: "{\"kind\":\"host-proof\",\"status\":\"ok\",\"artifactFileName\":\"host-proof.json\",\"focusArea\":\"host-proof\",\"capabilityIds\":[\"engine.lifecycle.init\",\"engine.lifecycle.shutdown\",\"engine.callback.register\",\"engine.callback.dispatch\",\"engine.object.ownership.retain\",\"engine.object.ownership.transfer\",\"engine.object.ownership.release\"],\"lifecycle\":[\"init\",\"shutdown\"],\"callback\":\"ok\",\"ownership\":\"ok\",\"stressIterations\":1000}"),
            _ => null,
        };

        return loweringPlan is not null;
    }

    private static NativeReferenceLoweringPlanArtifact CreateEngineLoweringPlan(
        string assemblyName,
        string entryPointSubjectId,
        NativeReferencePlanKind planKind,
        string focusArea,
        string entrySymbol,
        IReadOnlyList<string> capabilityIds,
        IReadOnlyList<string> bindingKinds,
        IReadOnlyList<string> helperNames,
        string expectedOutput)
    {
        return new NativeReferenceLoweringPlanArtifact
        {
            PlanKind = NativeReferenceProofCatalog.Stringify(planKind),
            AssemblyName = assemblyName,
            EntrySubjectId = entryPointSubjectId,
            IncludeHeader = "codegen_bridge.h",
            NativeEntryFunctionName = "RunNativeReference",
            EntrySymbol = entrySymbol,
            ReferenceTypeToken = "0u",
            CapturedFieldToken = "0u",
            EntryMethodToken = "0u",
            ConsoleWriteLineStringIcall = ConsoleWriteLineStringIcall,
            ExpectedOutput = expectedOutput,
            ExpectedOutputByteCount = Encoding.UTF8.GetByteCount(expectedOutput),
            EngineBindings = new EngineBindingsArtifact
            {
                ProofKind = "engine-binding",
                FocusArea = focusArea,
                CapabilityIds = capabilityIds,
                BindingKinds = bindingKinds,
                HelperNames = helperNames,
            },
            HostBindings = new HostBindingsArtifact
            {
                HostPlatform = "windows-x64",
                RuntimeProfile = "engine-proof-output",
                BindingKinds = ["artifact-observe-contract"],
            },
        };
    }

    private static NativeReferenceLoweringPlanArtifact CreateAsyncAwaitIntLoweringPlan(
        LinkedWorldModel linkedWorld,
        MetadataRegistrationArtifact metadataRegistration,
        IReadOnlyList<CodeRegistrationEntry> methodPointers)
    {
        var entryPointRegistration = GetRequiredRegistration(methodPointers, linkedWorld.EntryPointSubjectId);

        return new NativeReferenceLoweringPlanArtifact
        {
            PlanKind = "asyncAwaitIntMinimal",
            AssemblyName = linkedWorld.Assembly.Name,
            EntrySubjectId = linkedWorld.EntryPointSubjectId,
            IncludeHeader = "codegen_bridge.h",
            NativeEntryFunctionName = "RunNativeReference",
            EntrySymbol = entryPointRegistration.Symbol,
            ReferenceTypeToken = "0u",
            CapturedFieldToken = "0u",
            EntryMethodToken = FormatCppTokenLiteral(GetRequiredMetadataToken(metadataRegistration, "method", entryPointRegistration.SubjectId)),
            ConsoleWriteLineStringIcall = ConsoleWriteLineStringIcall,
            ExpectedOutput = "42",
            ExpectedOutputByteCount = Encoding.UTF8.GetByteCount("42"),
        };
    }

    private static NativeReferenceLoweringPlanArtifact CreateThreadingThreadStaticMonitorLoweringPlan(
        LinkedWorldModel linkedWorld,
        MetadataRegistrationArtifact metadataRegistration,
        IReadOnlyList<CodeRegistrationEntry> methodPointers)
    {
        var entryPointRegistration = GetRequiredRegistration(methodPointers, linkedWorld.EntryPointSubjectId);
        const string expectedOutput =
            "{\"kind\":\"threading-proof\",\"status\":\"ok\",\"threadStatic\":{\"main\":2,\"worker\":1},\"monitor\":{\"sharedTotal\":2},\"gc\":{\"rootsReported\":2,\"finalized\":1}}";

        return new NativeReferenceLoweringPlanArtifact
        {
            PlanKind = "threadingThreadStaticMonitorMinimal",
            AssemblyName = linkedWorld.Assembly.Name,
            EntrySubjectId = linkedWorld.EntryPointSubjectId,
            IncludeHeader = "codegen_bridge.h",
            NativeEntryFunctionName = "RunNativeReference",
            EntrySymbol = entryPointRegistration.Symbol,
            ReferenceTypeToken = "0u",
            CapturedFieldToken = "0u",
            EntryMethodToken = FormatCppTokenLiteral(GetRequiredMetadataToken(metadataRegistration, "method", entryPointRegistration.SubjectId)),
            ConsoleWriteLineStringIcall = ConsoleWriteLineStringIcall,
            ExpectedOutput = expectedOutput,
            ExpectedOutputByteCount = Encoding.UTF8.GetByteCount(expectedOutput),
        };
    }

    private static NativeReferenceLoweringPlanArtifact CreateInterfaceDispatchMessageLoweringPlan(
        LinkedWorldModel linkedWorld,
        IReadOnlyList<TypedIlMethodArtifact> methods,
        MetadataRegistrationArtifact metadataRegistration,
        IReadOnlyList<CodeRegistrationEntry> methodPointers)
    {
        return CreateDispatchMessageLoweringPlan(
            linkedWorld,
            methods,
            metadataRegistration,
            methodPointers,
            "interfaceDispatchMessage",
            "interface-runtime-helper",
            "callvirt.interface-runtime-helper");
    }

    private static NativeReferenceLoweringPlanArtifact CreateDispatchVirtualInstanceMessageLoweringPlan(
        LinkedWorldModel linkedWorld,
        IReadOnlyList<TypedIlMethodArtifact> methods,
        MetadataRegistrationArtifact metadataRegistration,
        IReadOnlyList<CodeRegistrationEntry> methodPointers)
    {
        return CreateDispatchMessageLoweringPlan(
            linkedWorld,
            methods,
            metadataRegistration,
            methodPointers,
            "dispatchVirtualInstanceMessage",
            "devirtualized-direct",
            null);
    }

    private static NativeReferenceLoweringPlanArtifact CreateDispatchMessageLoweringPlan(
        LinkedWorldModel linkedWorld,
        IReadOnlyList<TypedIlMethodArtifact> methods,
        MetadataRegistrationArtifact metadataRegistration,
        IReadOnlyList<CodeRegistrationEntry> methodPointers,
        string legacyPlanKind,
        string dispatchStrategy,
        string? requiredDispatchKind)
    {
        var entryPointSubjectId = linkedWorld.EntryPointSubjectId;
        var entryPointRegistration = GetRequiredRegistration(methodPointers, entryPointSubjectId);
        var entryPointMethod = GetRequiredMethod(methods, entryPointSubjectId);
        var entryPointInstructions = GetSingleBlockInstructions(entryPointMethod);
        var writeLineStringIcall = ValidateConstructorThenInstanceCallEntryPointShape(entryPointMethod, entryPointInstructions);

        var constructorSubjectId = GetRequiredInstructionCallee(entryPointInstructions[1], entryPointMethod.SubjectId, 1);
        var declaredMethodSubjectId = GetRequiredInstructionCallee(entryPointInstructions[2], entryPointMethod.SubjectId, 2);
        var dispatchFact = linkedWorld.OptimizationFacts.DispatchFacts.FirstOrDefault(fact =>
            string.Equals(fact.MethodSubjectId, entryPointSubjectId, StringComparison.Ordinal) &&
            (requiredDispatchKind is null
                ? fact.Devirtualized
                : string.Equals(fact.DispatchKind, requiredDispatchKind, StringComparison.Ordinal)) &&
            !string.Equals(fact.TargetSubjectId, declaredMethodSubjectId, StringComparison.Ordinal));
        var resolvedMethodSubjectId = dispatchFact?.TargetSubjectId;
        if (string.IsNullOrWhiteSpace(resolvedMethodSubjectId))
        {
            resolvedMethodSubjectId = TryResolveManagedDevirtualizedCallTarget(
                linkedWorld.Methods,
                constructorSubjectId,
                declaredMethodSubjectId);
        }

        if (string.IsNullOrWhiteSpace(resolvedMethodSubjectId))
        {
            var diagnosticConstructorTypeSubjectId = GetDeclaringTypeSubjectId(constructorSubjectId);
            var availableMethods = string.Join(
                ", ",
                linkedWorld.Methods
                    .Where(method => string.Equals(method.DeclaringTypeSubjectId, diagnosticConstructorTypeSubjectId, StringComparison.Ordinal))
                    .Select(method => method.SubjectId));
            var matchingNameMethods = string.Join(
                ", ",
                linkedWorld.Methods
                    .Where(method => string.Equals(method.Name, "BuildMessage", StringComparison.Ordinal))
                    .Select(method => $"{method.SubjectId}@{method.DeclaringTypeSubjectId}"));
            throw new InvalidOperationException(
                $"dispatch lowering could not resolve override target for '{entryPointSubjectId}' from declared target '{declaredMethodSubjectId}' on instance type '{diagnosticConstructorTypeSubjectId}'. available methods: [{availableMethods}]. world BuildMessage methods: [{matchingNameMethods}]");
        }
        var constructorRegistration = GetRequiredRegistration(methodPointers, constructorSubjectId);
        var resolvedMethodRegistration = GetRequiredRegistration(methodPointers, resolvedMethodSubjectId);
        var constructorMethod = GetRequiredMethod(methods, constructorSubjectId);
        var resolvedMethod = GetRequiredMethod(methods, resolvedMethodSubjectId);

        var constructorInstructions = GetSingleBlockInstructions(constructorMethod);
        var resolvedMethodInstructions = GetSingleBlockInstructions(resolvedMethod);
        ValidateConstructorShape(constructorMethod, constructorInstructions);
        ValidateFieldBackedStringInstanceMethodShape(resolvedMethod, resolvedMethodInstructions);

        var storedFieldSubjectId = GetRequiredOperandString(constructorInstructions[4]);
        var loadedFieldSubjectId = GetRequiredOperandString(resolvedMethodInstructions[2]);
        if (!string.Equals(storedFieldSubjectId, loadedFieldSubjectId, StringComparison.Ordinal))
        {
            throw new InvalidOperationException(
                $"dispatch lowering expects constructor writes '{storedFieldSubjectId}' and target method reads '{loadedFieldSubjectId}'");
        }

        var constructorLiteral = GetRequiredOperandString(entryPointInstructions[0]);
        var messagePrefixLiteral = GetRequiredOperandString(resolvedMethodInstructions[0]);
        var messageSuffixLiteral = GetRequiredOperandString(resolvedMethodInstructions[4]);
        var concatPairIcall = GetRequiredInstructionCallee(resolvedMethodInstructions[3], resolvedMethod.SubjectId, 3);
        var constructorTypeSubjectId = GetDeclaringTypeSubjectId(constructorSubjectId);

        return new NativeReferenceLoweringPlanArtifact
        {
            PlanKind = legacyPlanKind,
            AssemblyName = linkedWorld.Assembly.Name,
            EntrySubjectId = entryPointSubjectId,
            IncludeHeader = "codegen_bridge.h",
            NativeEntryFunctionName = "RunNativeReference",
            EntrySymbol = entryPointRegistration.Symbol,
            ConstructorSymbol = constructorRegistration.Symbol,
            InstanceMethodSymbol = resolvedMethodRegistration.Symbol,
            ReferenceTypeToken = FormatCppTokenLiteral(GetRequiredMetadataToken(metadataRegistration, "type", constructorTypeSubjectId)),
            CapturedFieldToken = FormatCppTokenLiteral(GetRequiredMetadataToken(metadataRegistration, "field", storedFieldSubjectId)),
            EntryMethodToken = FormatCppTokenLiteral(GetRequiredMetadataToken(metadataRegistration, "method", entryPointRegistration.SubjectId)),
            ConstructorMethodToken = FormatCppTokenLiteral(GetRequiredMetadataToken(metadataRegistration, "method", constructorRegistration.SubjectId)),
            InstanceMethodToken = FormatCppTokenLiteral(GetRequiredMetadataToken(metadataRegistration, "method", declaredMethodSubjectId)),
            DispatchStrategy = dispatchStrategy,
            ConsoleWriteLineStringIcall = writeLineStringIcall,
            StringConcatPairIcall = concatPairIcall,
            ConstructorLiteral = ToCppStringLiteral(constructorLiteral),
            ConstructorLiteralByteCount = Encoding.UTF8.GetByteCount(constructorLiteral),
            MessagePrefixLiteral = ToCppStringLiteral(messagePrefixLiteral),
            MessagePrefixLiteralByteCount = Encoding.UTF8.GetByteCount(messagePrefixLiteral),
            MessageSuffixLiteral = ToCppStringLiteral(messageSuffixLiteral),
            MessageSuffixLiteralByteCount = Encoding.UTF8.GetByteCount(messageSuffixLiteral),
        };
    }

    private static string? TryResolveManagedDevirtualizedCallTarget(
        IReadOnlyList<ManagedMethodModel> methods,
        string constructorSubjectId,
        string declaredTargetSubjectId)
    {
        var constructorTypeSubjectId = GetDeclaringTypeSubjectId(constructorSubjectId);
        var declaredMethod = methods.FirstOrDefault(method =>
            string.Equals(method.SubjectId, declaredTargetSubjectId, StringComparison.Ordinal));
        if (declaredMethod is null)
        {
            return null;
        }

        var exactMatch = methods.FirstOrDefault(candidate =>
            string.Equals(candidate.DeclaringTypeSubjectId, constructorTypeSubjectId, StringComparison.Ordinal) &&
            string.Equals(candidate.Name, declaredMethod.Name, StringComparison.Ordinal) &&
            candidate.Body.Blocks.Any(block => block.Instructions.Count > 0) &&
            candidate.Parameters.Select(parameter => parameter.Type).SequenceEqual(
                declaredMethod.Parameters.Select(parameter => parameter.Type),
                StringComparer.Ordinal));
        if (exactMatch is not null)
        {
            return exactMatch.SubjectId;
        }

        return methods.FirstOrDefault(candidate =>
            string.Equals(candidate.DeclaringTypeSubjectId, constructorTypeSubjectId, StringComparison.Ordinal) &&
            string.Equals(candidate.Name, declaredMethod.Name, StringComparison.Ordinal) &&
            candidate.Body.Blocks.Any(block => block.Instructions.Count > 0))
            ?.SubjectId;
    }

    private static NativeReferencePlanKind SelectLoweringFamily(
        LinkedWorldModel linkedWorld,
        IReadOnlyDictionary<string, MethodShapeModel> methodShapes,
        IReadOnlyDictionary<string, IReadOnlyList<string>> methodCapabilities)
    {
        var entryMethodShape = GetRequiredMethodShape(methodShapes, linkedWorld.EntryPointSubjectId);
        if (!string.Equals(entryMethodShape.MethodRole, "static-method", StringComparison.Ordinal) ||
            !string.Equals(entryMethodShape.BodyAvailability, "has-canonical-body", StringComparison.Ordinal))
        {
            throw new InvalidOperationException(
                $"CodeGen lowering selection expects entry method '{linkedWorld.EntryPointSubjectId}' to be a static canonical-body method");
        }

        var worldCapabilities = linkedWorld.CapabilityBundles.World.Capabilities
            .ToHashSet(StringComparer.Ordinal);
        var entryCapabilities = GetRequiredCapabilities(methodCapabilities, linkedWorld.EntryPointSubjectId);
        var dependencyReasons = linkedWorld.Dependencies
            .Select(dependency => dependency.Reason)
            .ToHashSet(StringComparer.Ordinal);

        if (MatchesReflectionInteropClosureCandidate(worldCapabilities))
        {
            return NativeReferencePlanKind.ReflectionInteropClosureMinimal;
        }

        if (MatchesMarshalingUtf8ExportCandidate(linkedWorld, worldCapabilities))
        {
            return NativeReferencePlanKind.MarshalingUtf8ExportMinimal;
        }

        if (MatchesPInvokeDirectCallCandidate(methodShapes, worldCapabilities))
        {
            return NativeReferencePlanKind.InteropPInvokeDirectCallMinimal;
        }

        if (MatchesReflectionClosedTypeQueryCandidate(worldCapabilities))
        {
            RequireDependencyReason(
                dependencyReasons,
                "reflection-query",
                NativeReferencePlanKind.ReflectionClosedTypeQueryMinimal,
                linkedWorld.EntryPointSubjectId);
            return NativeReferencePlanKind.ReflectionClosedTypeQueryMinimal;
        }

        if (MatchesDelegateClosedTargetRelayCandidate(worldCapabilities, entryCapabilities))
        {
            return NativeReferencePlanKind.DelegateClosedTargetRelayMinimal;
        }

        if (MatchesNestedExceptionCandidate(linkedWorld, worldCapabilities, entryCapabilities))
        {
            return NativeReferencePlanKind.NestedExceptionThrowCatchFinallyMinimal;
        }

        if (MatchesExceptionThrowCatchFinallyCandidate(worldCapabilities, entryCapabilities))
        {
            return NativeReferencePlanKind.ExceptionThrowCatchFinallyMinimal;
        }

        if (MatchesThreadingThreadStaticMonitorCandidate(linkedWorld, worldCapabilities))
        {
            return NativeReferencePlanKind.ManagedThreadingThreadStaticMonitorMinimal;
        }

        if (MatchesAsyncAwaitIntCandidate(linkedWorld, worldCapabilities, entryCapabilities))
        {
            RequireDependencyReason(
                dependencyReasons,
                "stdout-path",
                NativeReferencePlanKind.ManagedAsyncAwaitIntMinimal,
                linkedWorld.EntryPointSubjectId);
            return NativeReferencePlanKind.ManagedAsyncAwaitIntMinimal;
        }

        if (MatchesInterfaceDispatchMessageCandidate(linkedWorld, entryCapabilities))
        {
            return NativeReferencePlanKind.ManagedInterfaceDispatchMessageMinimal;
        }

        if (MatchesDispatchVirtualInstanceMessageCandidate(linkedWorld, entryCapabilities))
        {
            return NativeReferencePlanKind.ManagedDispatchVirtualInstanceMessageMinimal;
        }

        if (MatchesArrayCopyReferenceArrayCandidate(linkedWorld, entryCapabilities))
        {
            RequireDependencyReason(
                dependencyReasons,
                "stdout-path",
                NativeReferencePlanKind.ManagedArraysCopyReferenceArrayMinimal,
                linkedWorld.EntryPointSubjectId);
            return NativeReferencePlanKind.ManagedArraysCopyReferenceArrayMinimal;
        }

        if (MatchesArrayClearReferenceArrayCandidate(linkedWorld, entryCapabilities))
        {
            RequireDependencyReason(
                dependencyReasons,
                "stdout-path",
                NativeReferencePlanKind.ManagedArraysClearReferenceArrayMinimal,
                linkedWorld.EntryPointSubjectId);
            return NativeReferencePlanKind.ManagedArraysClearReferenceArrayMinimal;
        }

        if (MatchesArrayReverseReferenceArrayCandidate(linkedWorld, entryCapabilities))
        {
            RequireDependencyReason(
                dependencyReasons,
                "stdout-path",
                NativeReferencePlanKind.ManagedArraysReverseReferenceArrayMinimal,
                linkedWorld.EntryPointSubjectId);
            return NativeReferencePlanKind.ManagedArraysReverseReferenceArrayMinimal;
        }

        if (MatchesArrayBoxingReferenceArrayCandidate(linkedWorld, entryCapabilities))
        {
            RequireDependencyReason(
                dependencyReasons,
                "stdout-path",
                NativeReferencePlanKind.ManagedArraysBoxingReferenceArrayBoxedIntMinimal,
                linkedWorld.EntryPointSubjectId);
            return NativeReferencePlanKind.ManagedArraysBoxingReferenceArrayBoxedIntMinimal;
        }

        if (MatchesStaticForwarderCapturedGetterCandidate(methodShapes, worldCapabilities, entryCapabilities))
        {
            RequireDependencyReason(
                dependencyReasons,
                "stdout-path",
                NativeReferencePlanKind.ManagedGenericStaticForwarderCapturedGetterMinimal,
                linkedWorld.EntryPointSubjectId);
            return NativeReferencePlanKind.ManagedGenericStaticForwarderCapturedGetterMinimal;
        }

        if (MatchesCapturedStateInstanceMessageCandidate(methodShapes, worldCapabilities, entryCapabilities))
        {
            RequireDependencyReason(
                dependencyReasons,
                "stdout-path",
                NativeReferencePlanKind.ManagedObjectCapturedStateInstanceMessageMinimal,
                linkedWorld.EntryPointSubjectId);
            return NativeReferencePlanKind.ManagedObjectCapturedStateInstanceMessageMinimal;
        }

        throw new InvalidOperationException(
            $"CodeGen lowering selection could not classify '{linkedWorld.EntryPointSubjectId}' from semantic structure + capability bundle");
    }

    private static bool MatchesPInvokeDirectCallCandidate(
        IReadOnlyDictionary<string, MethodShapeModel> methodShapes,
        IReadOnlySet<string> worldCapabilities)
    {
        return worldCapabilities.Contains("requires-imported-call") &&
               methodShapes.Values.Any(shape => string.Equals(shape.MethodRole, "imported-method", StringComparison.Ordinal)) &&
               !worldCapabilities.Contains("requires-closed-type-member-query") &&
               !worldCapabilities.Contains("requires-generic-type-definition-query");
    }

    private static bool MatchesMarshalingUtf8ExportCandidate(
        LinkedWorldModel linkedWorld,
        IReadOnlySet<string> worldCapabilities)
    {
        return string.Equals(linkedWorld.Assembly.Name, "MarshalingProof", StringComparison.Ordinal) &&
               worldCapabilities.Contains("requires-utf8-string-marshaling") &&
               worldCapabilities.Contains("requires-unmanaged-callers-only-export");
    }

    private static bool MatchesReflectionClosedTypeQueryCandidate(IReadOnlySet<string> worldCapabilities)
    {
        return worldCapabilities.Contains("requires-closed-type-member-query") &&
               worldCapabilities.Contains("requires-generic-type-definition-query");
    }

    private static bool MatchesReflectionInteropClosureCandidate(IReadOnlySet<string> worldCapabilities)
    {
        return worldCapabilities.Contains("requires-imported-call") &&
               worldCapabilities.Contains("requires-closed-type-member-query") &&
               worldCapabilities.Contains("requires-generic-type-definition-query");
    }

}
