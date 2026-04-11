using System.Text;
using System.Text.Json;
using Chaos.IL2CPP.Contracts;
using Scriban;
using Scriban.Runtime;

namespace Chaos.IL2CPP.CodeGen;

public sealed partial class NativeReferenceLoweringPlanner
{
    private const string ManagedAsyncAwaitIntMinimal =
        "managed-async.awaitable-int.minimal";
    private const string ManagedThreadingThreadStaticMonitorMinimal =
        "managed-threading.threadstatic-monitor.minimal";
    private const string ManagedInterfaceDispatchMessageMinimal =
        "managed-dispatch.interface-message.minimal";
    private const string ManagedDispatchVirtualInstanceMessageMinimal =
        "managed-dispatch.virtual-instance-message.minimal";
    private const string ManagedObjectCapturedStateInstanceMessageMinimal =
        "managed-object.captured-state-instance-message.minimal";
    private const string ManagedGenericStaticForwarderCapturedGetterMinimal =
        "managed-generic.static-forwarder-captured-getter.minimal";
    private const string ManagedArraysBoxingReferenceArrayBoxedIntMinimal =
        "managed-arrays-boxing.reference-array-boxed-int.minimal";
    private const string DelegateClosedTargetRelayMinimal =
        "managed-delegates.closed-target-relay-message.minimal";
    private const string NestedExceptionThrowCatchFinallyMinimal =
        "managed-exceptions.nested-throw-catch-finally.minimal";
    private const string ExceptionThrowCatchFinallyMinimal =
        "managed-exceptions.throw-catch-finally-message.minimal";
    private const string ReflectionInteropClosureMinimal =
        "managed-reflection-interop.closure.minimal";
    private const string ReflectionClosedTypeQueryMinimal =
        "reflection.closed-type-query.minimal";
    private const string MarshalingUtf8ExportMinimal =
        "interop.marshaling-utf8-export.minimal";
    private const string InteropPInvokeDirectCallMinimal =
        "interop.pinvoke-direct-call.minimal";
    private const string EngineLogWriteMinimal =
        "engine.log-write.minimal";
    private const string EngineObjectHandleRoundtripMinimal =
        "engine.object-handle.roundtrip.minimal";
    private const string EngineLifecycleCallbackMinimal =
        "engine.lifecycle-callback.minimal";
    private const string EngineHostProofMinimal =
        "engine.host-proof.minimal";
    private const string GeneratedTranslationUnitTemplateRelativePath = "Templates/NativeReferenceProof.cpp.scriban";
    private const string DispatchVirtualInstanceMessageGeneratedTranslationUnitTemplateRelativePath =
        "Templates/NativeReferenceProof.DispatchVirtualInstanceMessage.cpp.scriban";
    private const string InterfaceDispatchMessageGeneratedTranslationUnitTemplateRelativePath =
        "Templates/NativeReferenceProof.InterfaceDispatchMessage.cpp.scriban";
    private const string AsyncAwaitIntGeneratedTranslationUnitTemplateRelativePath =
        "Templates/NativeReferenceProof.AsyncAwaitInt.cpp.scriban";
    private const string ThreadingThreadStaticMonitorGeneratedTranslationUnitTemplateRelativePath =
        "Templates/NativeReferenceProof.ThreadingThreadStaticMonitor.cpp.scriban";
    private const string StaticCallCtorGetterGeneratedTranslationUnitTemplateRelativePath =
        "Templates/NativeReferenceProof.StaticCallCtorGetter.cpp.scriban";
    private const string ArrayBoxingReferenceArrayGeneratedTranslationUnitTemplateRelativePath =
        "Templates/NativeReferenceProof.ArrayBoxingReferenceArray.cpp.scriban";
    private const string ReflectionQueryMinimalGeneratedTranslationUnitTemplateRelativePath =
        "Templates/NativeReferenceProof.ReflectionQueryMinimal.cpp.scriban";
    private const string MarshalingUtf8ExportGeneratedTranslationUnitTemplateRelativePath =
        "Templates/NativeReferenceProof.MarshalingUtf8Export.cpp.scriban";
    private const string PInvokeDllImportMinimalGeneratedTranslationUnitTemplateRelativePath =
        "Templates/NativeReferenceProof.PInvokeDllImportMinimal.cpp.scriban";
    private const string DelegateClosedTargetRelayGeneratedTranslationUnitTemplateRelativePath =
        "Templates/NativeReferenceProof.DelegateClosedTargetRelay.cpp.scriban";
    private const string NestedExceptionThrowCatchFinallyGeneratedTranslationUnitTemplateRelativePath =
        "Templates/NativeReferenceProof.NestedExceptionThrowCatchFinally.cpp.scriban";
    private const string ExceptionThrowCatchFinallyGeneratedTranslationUnitTemplateRelativePath =
        "Templates/NativeReferenceProof.ExceptionThrowCatchFinally.cpp.scriban";
    private const string ReflectionInteropClosureGeneratedTranslationUnitTemplateRelativePath =
        "Templates/NativeReferenceProof.ReflectionInteropClosure.cpp.scriban";
    private const string ConsoleWriteLineStringIcall = "System.Console/System.Console::WriteLine(System.String)";
    private const string StringConcatPairIcall = "System.Private.CoreLib/System.String::Concat(System.String,System.String)";

    private static readonly JsonSerializerOptions JsonOptions = new()
    {
        PropertyNameCaseInsensitive = true,
    };

    private static readonly Lazy<Template> GeneratedTranslationUnitTemplate =
        new(() => LoadTemplate(GeneratedTranslationUnitTemplateRelativePath));

    private static readonly Lazy<Template> DispatchVirtualInstanceMessageGeneratedTranslationUnitTemplate =
        new(() => LoadTemplate(DispatchVirtualInstanceMessageGeneratedTranslationUnitTemplateRelativePath));

    private static readonly Lazy<Template> InterfaceDispatchMessageGeneratedTranslationUnitTemplate =
        new(() => LoadTemplate(InterfaceDispatchMessageGeneratedTranslationUnitTemplateRelativePath));

    private static readonly Lazy<Template> AsyncAwaitIntGeneratedTranslationUnitTemplate =
        new(() => LoadTemplate(AsyncAwaitIntGeneratedTranslationUnitTemplateRelativePath));

    private static readonly Lazy<Template> ThreadingThreadStaticMonitorGeneratedTranslationUnitTemplate =
        new(() => LoadTemplate(ThreadingThreadStaticMonitorGeneratedTranslationUnitTemplateRelativePath));

    private static readonly Lazy<Template> StaticCallCtorGetterGeneratedTranslationUnitTemplate =
        new(() => LoadTemplate(StaticCallCtorGetterGeneratedTranslationUnitTemplateRelativePath));

    private static readonly Lazy<Template> ArrayBoxingReferenceArrayGeneratedTranslationUnitTemplate =
        new(() => LoadTemplate(ArrayBoxingReferenceArrayGeneratedTranslationUnitTemplateRelativePath));

    private static readonly Lazy<Template> ReflectionQueryMinimalGeneratedTranslationUnitTemplate =
        new(() => LoadTemplate(ReflectionQueryMinimalGeneratedTranslationUnitTemplateRelativePath));

    private static readonly Lazy<Template> MarshalingUtf8ExportGeneratedTranslationUnitTemplate =
        new(() => LoadTemplate(MarshalingUtf8ExportGeneratedTranslationUnitTemplateRelativePath));

    private static readonly Lazy<Template> PInvokeDllImportMinimalGeneratedTranslationUnitTemplate =
        new(() => LoadTemplate(PInvokeDllImportMinimalGeneratedTranslationUnitTemplateRelativePath));

    private static readonly Lazy<Template> DelegateClosedTargetRelayGeneratedTranslationUnitTemplate =
        new(() => LoadTemplate(DelegateClosedTargetRelayGeneratedTranslationUnitTemplateRelativePath));

    private static readonly Lazy<Template> NestedExceptionThrowCatchFinallyGeneratedTranslationUnitTemplate =
        new(() => LoadTemplate(NestedExceptionThrowCatchFinallyGeneratedTranslationUnitTemplateRelativePath));

    private static readonly Lazy<Template> ExceptionThrowCatchFinallyGeneratedTranslationUnitTemplate =
        new(() => LoadTemplate(ExceptionThrowCatchFinallyGeneratedTranslationUnitTemplateRelativePath));

    private static readonly Lazy<Template> ReflectionInteropClosureGeneratedTranslationUnitTemplate =
        new(() => LoadTemplate(ReflectionInteropClosureGeneratedTranslationUnitTemplateRelativePath));

    public NativeReferenceLoweringPlanArtifact Create(
        LinkedWorldModel linkedWorld,
        TypedIlIrArtifact typedIl,
        MetadataRegistrationArtifact metadataRegistration,
        CodeRegistrationArtifact codeRegistration)
    {
        var methodPointers = codeRegistration.Modules
            .SelectMany(module => module.Registrations)
            .Where(registration => string.Equals(registration.RegistrationKind, "methodPointer", StringComparison.Ordinal))
            .ToList();
        if (TryCreateEngineLoweringPlan(linkedWorld.Assembly.Name, linkedWorld.EntryPointSubjectId, out var enginePlan))
        {
            return enginePlan;
        }

        var methodShapes = linkedWorld.SemanticShapes.Methods
            .ToDictionary(shape => shape.SubjectId, StringComparer.Ordinal);
        var methodCapabilities = linkedWorld.CapabilityBundles.Methods
            .ToDictionary(bundle => bundle.SubjectId, bundle => bundle.Capabilities, StringComparer.Ordinal);

        var selectedFamily = SelectLoweringFamily(linkedWorld, methodShapes, methodCapabilities);
        NativeReferenceLoweringPlanArtifact planned;
        if (string.Equals(selectedFamily, ManagedAsyncAwaitIntMinimal, StringComparison.Ordinal))
        {
            planned = CreateAsyncAwaitIntLoweringPlan(
                linkedWorld,
                metadataRegistration,
                methodPointers);
        }
        else if (string.Equals(selectedFamily, ManagedThreadingThreadStaticMonitorMinimal, StringComparison.Ordinal))
        {
            planned = CreateThreadingThreadStaticMonitorLoweringPlan(
                linkedWorld,
                metadataRegistration,
                methodPointers);
        }
        else if (string.Equals(selectedFamily, ManagedInterfaceDispatchMessageMinimal, StringComparison.Ordinal))
        {
            planned = CreateInterfaceDispatchMessageLoweringPlan(
                linkedWorld,
                typedIl.Methods,
                metadataRegistration,
                methodPointers);
        }
        else if (string.Equals(selectedFamily, ManagedDispatchVirtualInstanceMessageMinimal, StringComparison.Ordinal))
        {
            planned = CreateDispatchVirtualInstanceMessageLoweringPlan(
                linkedWorld,
                typedIl.Methods,
                metadataRegistration,
                methodPointers);
        }
        else
        {
            planned = CreateLoweringPlan(
                linkedWorld.Assembly.Name,
                linkedWorld.EntryPointSubjectId,
                typedIl.Methods,
                metadataRegistration,
                methodPointers);
        }
        var mappedLegacyFamily = MapLegacyPlanKind(planned.PlanKind);

        if (!string.Equals(selectedFamily, mappedLegacyFamily, StringComparison.Ordinal))
        {
            throw new InvalidOperationException(
                $"semantic lowering family '{selectedFamily}' disagrees with legacy proof-shape lowering family '{mappedLegacyFamily}' for '{linkedWorld.EntryPointSubjectId}'");
        }

        return planned with
        {
            PlanKind = selectedFamily,
        };
    }

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
                planKind: EngineLogWriteMinimal,
                focusArea: "service-call",
                entrySymbol: "RunEngineLogWriteEntry",
                capabilityIds: ["engine.log.write"],
                bindingKinds: ["log-write"],
                helperNames: ["EngineLogWrite"],
                expectedOutput: "{\"kind\":\"log-write\",\"status\":\"ok\",\"artifactFileName\":\"log-write.json\",\"focusArea\":\"service-call\",\"capabilityIds\":[\"engine.log.write\"],\"message\":\"engine.log.write|minimal|ok\"}"),
            "EngineObjectHandleLite/EngineObjectHandleEntry::Run()" => CreateEngineLoweringPlan(
                assemblyName,
                entryPointSubjectId,
                planKind: EngineObjectHandleRoundtripMinimal,
                focusArea: "object-handle",
                entrySymbol: "RunEngineObjectHandleEntry",
                capabilityIds: ["engine.object.handle.create", "engine.object.handle.resolve"],
                bindingKinds: ["object-handle-create", "object-handle-resolve"],
                helperNames: ["CreateEngineObjectHandle", "ResolveEngineObjectHandle"],
                expectedOutput: "{\"kind\":\"handle-roundtrip\",\"status\":\"ok\",\"artifactFileName\":\"handle-roundtrip.json\",\"focusArea\":\"object-handle\",\"capabilityIds\":[\"engine.object.handle.create\",\"engine.object.handle.resolve\"],\"roundtrip\":\"ok\",\"identity\":\"same\"}"),
            "EngineLifecycleCallbackLite/EngineLifecycleCallbackEntry::Run()" => CreateEngineLoweringPlan(
                assemblyName,
                entryPointSubjectId,
                planKind: EngineLifecycleCallbackMinimal,
                focusArea: "lifecycle-callback",
                entrySymbol: "RunEngineLifecycleCallbackEntry",
                capabilityIds: ["engine.lifecycle.callback.register", "engine.lifecycle.dispatch", "engine.thread.main-lane"],
                bindingKinds: ["lifecycle-callback-register", "lifecycle-callback-dispatch", "main-thread-lane"],
                helperNames: ["RegisterEngineLifecycleCallback", "DispatchEngineLifecycleCallbacks", "IsMainThreadLane"],
                expectedOutput: "{\"kind\":\"lifecycle-callback\",\"status\":\"ok\",\"artifactFileName\":\"lifecycle-callback.json\",\"focusArea\":\"lifecycle-callback\",\"capabilityIds\":[\"engine.lifecycle.callback.register\",\"engine.lifecycle.dispatch\",\"engine.thread.main-lane\"],\"order\":[\"init\",\"tick\",\"shutdown\"],\"mainThread\":true}"),
            "EngineHostProof/EngineHostEntry::Run()" => CreateEngineLoweringPlan(
                assemblyName,
                entryPointSubjectId,
                planKind: EngineHostProofMinimal,
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
        string planKind,
        string focusArea,
        string entrySymbol,
        IReadOnlyList<string> capabilityIds,
        IReadOnlyList<string> bindingKinds,
        IReadOnlyList<string> helperNames,
        string expectedOutput)
    {
        return new NativeReferenceLoweringPlanArtifact
        {
            PlanKind = planKind,
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
        ValidateConstructorThenInstanceCallEntryPointShape(entryPointMethod, entryPointInstructions);

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
        var writeLineStringIcall = GetRequiredInstructionCallee(entryPointInstructions[3], entryPointMethod.SubjectId, 3);
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

    private static string SelectLoweringFamily(
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
            return ReflectionInteropClosureMinimal;
        }

        if (MatchesMarshalingUtf8ExportCandidate(linkedWorld, worldCapabilities))
        {
            return MarshalingUtf8ExportMinimal;
        }

        if (MatchesPInvokeDirectCallCandidate(methodShapes, worldCapabilities))
        {
            return InteropPInvokeDirectCallMinimal;
        }

        if (MatchesReflectionClosedTypeQueryCandidate(worldCapabilities))
        {
            RequireDependencyReason(
                dependencyReasons,
                "reflection-query",
                ReflectionClosedTypeQueryMinimal,
                linkedWorld.EntryPointSubjectId);
            return ReflectionClosedTypeQueryMinimal;
        }

        if (MatchesDelegateClosedTargetRelayCandidate(worldCapabilities, entryCapabilities))
        {
            return DelegateClosedTargetRelayMinimal;
        }

        if (MatchesNestedExceptionCandidate(linkedWorld, worldCapabilities, entryCapabilities))
        {
            return NestedExceptionThrowCatchFinallyMinimal;
        }

        if (MatchesExceptionThrowCatchFinallyCandidate(worldCapabilities, entryCapabilities))
        {
            return ExceptionThrowCatchFinallyMinimal;
        }

        if (MatchesThreadingThreadStaticMonitorCandidate(linkedWorld, worldCapabilities))
        {
            return ManagedThreadingThreadStaticMonitorMinimal;
        }

        if (MatchesAsyncAwaitIntCandidate(linkedWorld, worldCapabilities, entryCapabilities))
        {
            RequireDependencyReason(
                dependencyReasons,
                "stdout-path",
                ManagedAsyncAwaitIntMinimal,
                linkedWorld.EntryPointSubjectId);
            return ManagedAsyncAwaitIntMinimal;
        }

        if (MatchesInterfaceDispatchMessageCandidate(linkedWorld, entryCapabilities))
        {
            RequireDependencyReason(
                dependencyReasons,
                "stdout-path",
                ManagedInterfaceDispatchMessageMinimal,
                linkedWorld.EntryPointSubjectId);
            return ManagedInterfaceDispatchMessageMinimal;
        }

        if (MatchesDispatchVirtualInstanceMessageCandidate(linkedWorld, entryCapabilities))
        {
            RequireDependencyReason(
                dependencyReasons,
                "stdout-path",
                ManagedDispatchVirtualInstanceMessageMinimal,
                linkedWorld.EntryPointSubjectId);
            return ManagedDispatchVirtualInstanceMessageMinimal;
        }

        if (MatchesArrayBoxingReferenceArrayCandidate(linkedWorld, entryCapabilities))
        {
            RequireDependencyReason(
                dependencyReasons,
                "stdout-path",
                ManagedArraysBoxingReferenceArrayBoxedIntMinimal,
                linkedWorld.EntryPointSubjectId);
            return ManagedArraysBoxingReferenceArrayBoxedIntMinimal;
        }

        if (MatchesStaticForwarderCapturedGetterCandidate(methodShapes, worldCapabilities, entryCapabilities))
        {
            RequireDependencyReason(
                dependencyReasons,
                "stdout-path",
                ManagedGenericStaticForwarderCapturedGetterMinimal,
                linkedWorld.EntryPointSubjectId);
            return ManagedGenericStaticForwarderCapturedGetterMinimal;
        }

        if (MatchesCapturedStateInstanceMessageCandidate(methodShapes, worldCapabilities, entryCapabilities))
        {
            RequireDependencyReason(
                dependencyReasons,
                "stdout-path",
                ManagedObjectCapturedStateInstanceMessageMinimal,
                linkedWorld.EntryPointSubjectId);
            return ManagedObjectCapturedStateInstanceMessageMinimal;
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
        if (!entryCapabilities.Contains("requires-console-string-output", StringComparer.Ordinal))
        {
            return false;
        }

        var entryMethod = linkedWorld.Methods.FirstOrDefault(method =>
            string.Equals(method.SubjectId, linkedWorld.EntryPointSubjectId, StringComparison.Ordinal));
        var entryInstructions = TryGetSingleBlockInstructions(entryMethod);
        if (entryInstructions is null || !IsConstructorThenInstanceCallEntryPointShape(entryInstructions))
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
        if (!entryCapabilities.Contains("requires-console-string-output", StringComparer.Ordinal) ||
            !entryCapabilities.Contains("uses-interface-call-site", StringComparer.Ordinal))
        {
            return false;
        }

        var entryMethod = linkedWorld.Methods.FirstOrDefault(method =>
            string.Equals(method.SubjectId, linkedWorld.EntryPointSubjectId, StringComparison.Ordinal));
        var entryInstructions = TryGetSingleBlockInstructions(entryMethod);
        if (entryInstructions is null || !IsConstructorThenInstanceCallEntryPointShape(entryInstructions))
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
        string loweringFamily,
        string entryPointSubjectId)
    {
        if (!dependencyReasons.Contains(requiredReason))
        {
            throw new InvalidOperationException(
                $"CodeGen lowering family '{loweringFamily}' for '{entryPointSubjectId}' is missing dependency reason '{requiredReason}'");
        }
    }

    private static string MapLegacyPlanKind(string legacyPlanKind)
    {
        return legacyPlanKind switch
        {
            "asyncAwaitIntMinimal" => ManagedAsyncAwaitIntMinimal,
            "threadingThreadStaticMonitorMinimal" => ManagedThreadingThreadStaticMonitorMinimal,
            "interfaceDispatchMessage" => ManagedInterfaceDispatchMessageMinimal,
            "dispatchVirtualInstanceMessage" => ManagedDispatchVirtualInstanceMessageMinimal,
            "constructorThenInstanceCall" => ManagedObjectCapturedStateInstanceMessageMinimal,
            "staticCallCtorGetter" => ManagedGenericStaticForwarderCapturedGetterMinimal,
            "arrayBoxingReferenceArray" => ManagedArraysBoxingReferenceArrayBoxedIntMinimal,
            "delegateClosedTargetRelayMinimal" => DelegateClosedTargetRelayMinimal,
            "nestedExceptionThrowCatchFinallyMinimal" => NestedExceptionThrowCatchFinallyMinimal,
            "exceptionThrowCatchFinallyMinimal" => ExceptionThrowCatchFinallyMinimal,
            "reflectionInteropClosureMinimal" => ReflectionInteropClosureMinimal,
            "reflectionQueryMinimal" => ReflectionClosedTypeQueryMinimal,
            "marshalingUtf8ExportMinimal" => MarshalingUtf8ExportMinimal,
            "pinvokeDllImportMinimal" => InteropPInvokeDirectCallMinimal,
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
            ],
        };

        return new NativeReferenceProofResult
        {
            OutputRootPath = request.OutputRootPath,
            LoweringPlan = loweringPlan,
            Manifest = manifest,
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

    private static NativeReferenceLoweringPlanArtifact CreateLoweringPlan(
        string assemblyName,
        string entryPointSubjectId,
        IReadOnlyList<TypedIlMethodArtifact> methods,
        MetadataRegistrationArtifact metadataRegistration,
        IReadOnlyList<CodeRegistrationEntry> methodPointers)
    {
        var entryPointRegistration = GetRequiredRegistration(methodPointers, entryPointSubjectId);
        var entryPointMethod = GetRequiredMethod(methods, entryPointSubjectId);
        RequireMethodContract(entryPointMethod, "static-method", "has-canonical-body");
        var entryPointInstructions = GetSingleBlockInstructions(entryPointMethod);

        if (TryCreateReflectionInteropClosureLoweringPlan(
                assemblyName,
                entryPointSubjectId,
                metadataRegistration,
                entryPointRegistration,
                out var reflectionInteropClosurePlan))
        {
            return reflectionInteropClosurePlan;
        }

        if (TryCreateMarshalingUtf8ExportLoweringPlan(
                assemblyName,
                entryPointSubjectId,
                metadataRegistration,
                entryPointRegistration,
                out var marshalingUtf8ExportPlan))
        {
            return marshalingUtf8ExportPlan;
        }

        if (TryCreatePInvokeDllImportMinimalLoweringPlan(
                assemblyName,
                entryPointSubjectId,
                metadataRegistration,
                methodPointers,
                methods,
                entryPointRegistration,
                entryPointMethod,
                entryPointInstructions,
                out var pinvokeDllImportMinimalPlan))
        {
            return pinvokeDllImportMinimalPlan;
        }

        if (TryCreateReflectionQueryMinimalLoweringPlan(
                assemblyName,
                entryPointSubjectId,
                metadataRegistration,
                entryPointRegistration,
                out var reflectionQueryMinimalPlan))
        {
            return reflectionQueryMinimalPlan;
        }

        if (TryCreateDelegateClosedTargetRelayLoweringPlan(
                assemblyName,
                entryPointSubjectId,
                metadataRegistration,
                methodPointers,
                entryPointRegistration,
                out var delegateClosedTargetRelayPlan))
        {
            return delegateClosedTargetRelayPlan;
        }

        if (TryCreateNestedExceptionLoweringPlan(
                assemblyName,
                entryPointSubjectId,
                metadataRegistration,
                methodPointers,
                entryPointRegistration,
                out var nestedExceptionPlan))
        {
            return nestedExceptionPlan;
        }

        if (TryCreateExceptionThrowCatchFinallyLoweringPlan(
                assemblyName,
                entryPointSubjectId,
                metadataRegistration,
                methodPointers,
                entryPointRegistration,
                out var exceptionThrowCatchFinallyPlan))
        {
            return exceptionThrowCatchFinallyPlan;
        }

        if (TryCreateThreadingThreadStaticMonitorLoweringPlan(
                assemblyName,
                entryPointSubjectId,
                metadataRegistration,
                methodPointers,
                out var threadingThreadStaticMonitorPlan))
        {
            return threadingThreadStaticMonitorPlan;
        }

        if (TryCreateAsyncAwaitIntLoweringPlan(
                assemblyName,
                entryPointSubjectId,
                metadataRegistration,
                methodPointers,
                out var asyncAwaitIntPlan))
        {
            return asyncAwaitIntPlan;
        }

        if (TryCreateInterfaceDispatchMessageLoweringPlan(
                assemblyName,
                entryPointSubjectId,
                metadataRegistration,
                methodPointers,
                methods,
                entryPointRegistration,
                entryPointMethod,
                entryPointInstructions,
                out var interfaceDispatchMessagePlan))
        {
            return interfaceDispatchMessagePlan;
        }

        if (TryCreateDispatchVirtualInstanceMessageLoweringPlan(
                assemblyName,
                entryPointSubjectId,
                metadataRegistration,
                methodPointers,
                methods,
                entryPointRegistration,
                entryPointMethod,
                entryPointInstructions,
                out var dispatchVirtualInstanceMessagePlan))
        {
            return dispatchVirtualInstanceMessagePlan;
        }

        if (TryCreateArrayBoxingReferenceArrayLoweringPlan(
                assemblyName,
                entryPointSubjectId,
                metadataRegistration,
                methodPointers,
                methods,
                entryPointRegistration,
                entryPointMethod,
                entryPointInstructions,
                out var arrayBoxingReferenceArrayPlan))
        {
            return arrayBoxingReferenceArrayPlan;
        }

        if (IsStaticCallCtorGetterEntryPointShape(entryPointInstructions))
        {
            return CreateStaticCallCtorGetterLoweringPlan(
                assemblyName,
                entryPointSubjectId,
                metadataRegistration,
                methodPointers,
                methods,
                entryPointRegistration,
                entryPointMethod,
                entryPointInstructions);
        }

        return CreateConstructorThenInstanceCallLoweringPlan(
            assemblyName,
            entryPointSubjectId,
            metadataRegistration,
            methodPointers,
            methods,
            entryPointRegistration,
            entryPointMethod,
            entryPointInstructions);
    }

    private static NativeReferenceLoweringPlanArtifact CreateConstructorThenInstanceCallLoweringPlan(
        string assemblyName,
        string entryPointSubjectId,
        MetadataRegistrationArtifact metadataRegistration,
        IReadOnlyList<CodeRegistrationEntry> methodPointers,
        IReadOnlyList<TypedIlMethodArtifact> methods,
        CodeRegistrationEntry entryPointRegistration,
        TypedIlMethodArtifact entryPointMethod,
        IReadOnlyList<TypedIlInstructionArtifact> entryPointInstructions)
    {
        ValidateConstructorThenInstanceCallEntryPointShape(entryPointMethod, entryPointInstructions);

        var constructorSubjectId = GetRequiredInstructionCallee(entryPointInstructions[1], entryPointMethod.SubjectId, 1);
        var instanceMethodSubjectId = GetRequiredInstructionCallee(entryPointInstructions[2], entryPointMethod.SubjectId, 2);
        var writeLineStringIcall = GetRequiredInstructionCallee(entryPointInstructions[3], entryPointMethod.SubjectId, 3);

        var constructorRegistration = GetRequiredRegistration(methodPointers, constructorSubjectId);
        var instanceMethodRegistration = GetRequiredRegistration(methodPointers, instanceMethodSubjectId);
        var constructorMethod = GetRequiredMethod(methods, constructorSubjectId);
        var instanceMethod = GetRequiredMethod(methods, instanceMethodSubjectId);

        var constructorInstructions = GetSingleBlockInstructions(constructorMethod);
        var instanceMethodInstructions = GetSingleBlockInstructions(instanceMethod);

        ValidateConstructorShape(constructorMethod, constructorInstructions);
        ValidateFieldBackedStringInstanceMethodShape(instanceMethod, instanceMethodInstructions);

        var constructorTypeSubjectId = GetDeclaringTypeSubjectId(constructorRegistration.SubjectId);
        var instanceMethodTypeSubjectId = GetDeclaringTypeSubjectId(instanceMethodRegistration.SubjectId);
        if (!string.Equals(constructorTypeSubjectId, instanceMethodTypeSubjectId, StringComparison.Ordinal))
        {
            throw new InvalidOperationException(
                $"native-reference emitter expects constructor '{constructorRegistration.SubjectId}' and instance method '{instanceMethodRegistration.SubjectId}' to share a declaring type");
        }

        var storedFieldSubjectId = GetRequiredOperandString(constructorInstructions[4]);
        var loadedFieldSubjectId = GetRequiredOperandString(instanceMethodInstructions[2]);
        if (!string.Equals(storedFieldSubjectId, loadedFieldSubjectId, StringComparison.Ordinal))
        {
            throw new InvalidOperationException(
                $"native-reference emitter field access mismatch: constructor writes '{storedFieldSubjectId}', instance method reads '{loadedFieldSubjectId}'");
        }

        var constructorStringLiteral = GetRequiredOperandString(entryPointInstructions[0]);
        var messagePrefixLiteral = GetRequiredOperandString(instanceMethodInstructions[0]);
        var messageSuffixLiteral = GetRequiredOperandString(instanceMethodInstructions[4]);
        var concatPairIcall = GetRequiredInstructionCallee(instanceMethodInstructions[3], instanceMethod.SubjectId, 3);

        return new NativeReferenceLoweringPlanArtifact
        {
            PlanKind = "constructorThenInstanceCall",
            AssemblyName = assemblyName,
            EntrySubjectId = entryPointSubjectId,
            IncludeHeader = "codegen_bridge.h",
            NativeEntryFunctionName = "RunNativeReference",
            EntrySymbol = entryPointRegistration.Symbol,
            ConstructorSymbol = constructorRegistration.Symbol,
            InstanceMethodSymbol = instanceMethodRegistration.Symbol,
            ReferenceTypeToken = FormatCppTokenLiteral(GetRequiredMetadataToken(metadataRegistration, "type", constructorTypeSubjectId)),
            CapturedFieldToken = FormatCppTokenLiteral(GetRequiredMetadataToken(metadataRegistration, "field", storedFieldSubjectId)),
            EntryMethodToken = FormatCppTokenLiteral(GetRequiredMetadataToken(metadataRegistration, "method", entryPointRegistration.SubjectId)),
            ConstructorMethodToken = FormatCppTokenLiteral(GetRequiredMetadataToken(metadataRegistration, "method", constructorRegistration.SubjectId)),
            InstanceMethodToken = FormatCppTokenLiteral(GetRequiredMetadataToken(metadataRegistration, "method", instanceMethodRegistration.SubjectId)),
            ConsoleWriteLineStringIcall = writeLineStringIcall,
            StringConcatPairIcall = concatPairIcall,
            ConstructorLiteral = ToCppStringLiteral(constructorStringLiteral),
            ConstructorLiteralByteCount = Encoding.UTF8.GetByteCount(constructorStringLiteral),
            MessagePrefixLiteral = ToCppStringLiteral(messagePrefixLiteral),
            MessagePrefixLiteralByteCount = Encoding.UTF8.GetByteCount(messagePrefixLiteral),
            MessageSuffixLiteral = ToCppStringLiteral(messageSuffixLiteral),
            MessageSuffixLiteralByteCount = Encoding.UTF8.GetByteCount(messageSuffixLiteral),
        };
    }

    private static NativeReferenceLoweringPlanArtifact CreateStaticCallCtorGetterLoweringPlan(
        string assemblyName,
        string entryPointSubjectId,
        MetadataRegistrationArtifact metadataRegistration,
        IReadOnlyList<CodeRegistrationEntry> methodPointers,
        IReadOnlyList<TypedIlMethodArtifact> methods,
        CodeRegistrationEntry entryPointRegistration,
        TypedIlMethodArtifact entryPointMethod,
        IReadOnlyList<TypedIlInstructionArtifact> entryPointInstructions)
    {
        ValidateStaticCallCtorGetterEntryPointShape(entryPointMethod, entryPointInstructions);

        var echoMethodSubjectId = GetRequiredInstructionCallee(entryPointInstructions[1], entryPointMethod.SubjectId, 1);
        var constructorSubjectId = GetRequiredInstructionCallee(entryPointInstructions[2], entryPointMethod.SubjectId, 2);
        var getterSubjectId = GetRequiredInstructionCallee(entryPointInstructions[3], entryPointMethod.SubjectId, 3);
        var writeLineStringIcall = GetRequiredInstructionCallee(entryPointInstructions[4], entryPointMethod.SubjectId, 4);

        var echoMethodRegistration = GetRequiredRegistration(methodPointers, echoMethodSubjectId);
        var constructorRegistration = GetRequiredRegistration(methodPointers, constructorSubjectId);
        var getterRegistration = GetRequiredRegistration(methodPointers, getterSubjectId);
        var echoMethod = GetRequiredMethod(methods, echoMethodSubjectId);
        var constructorMethod = GetRequiredMethod(methods, constructorSubjectId);
        var getterMethod = GetRequiredMethod(methods, getterSubjectId);

        var echoInstructions = GetSingleBlockInstructions(echoMethod);
        var constructorInstructions = GetSingleBlockInstructions(constructorMethod);
        var getterInstructions = GetSingleBlockInstructions(getterMethod);

        ValidateSingleArgumentForwarderShape(echoMethod, echoInstructions);
        ValidateConstructorShape(constructorMethod, constructorInstructions);
        ValidateFieldGetterShape(getterMethod, getterInstructions);

        var constructorTypeSubjectId = GetDeclaringTypeSubjectId(constructorRegistration.SubjectId);
        var getterTypeSubjectId = GetDeclaringTypeSubjectId(getterRegistration.SubjectId);
        if (!string.Equals(constructorTypeSubjectId, getterTypeSubjectId, StringComparison.Ordinal))
        {
            throw new InvalidOperationException(
                $"native-reference emitter expects constructor '{constructorRegistration.SubjectId}' and getter '{getterRegistration.SubjectId}' to share a declaring type");
        }

        var storedFieldSubjectId = GetRequiredOperandString(constructorInstructions[4]);
        var loadedFieldSubjectId = GetRequiredOperandString(getterInstructions[1]);
        if (!string.Equals(storedFieldSubjectId, loadedFieldSubjectId, StringComparison.Ordinal))
        {
            throw new InvalidOperationException(
                $"native-reference emitter field access mismatch: constructor writes '{storedFieldSubjectId}', getter reads '{loadedFieldSubjectId}'");
        }

        var echoLiteral = GetRequiredOperandString(entryPointInstructions[0]);

        return new NativeReferenceLoweringPlanArtifact
        {
            PlanKind = "staticCallCtorGetter",
            AssemblyName = assemblyName,
            EntrySubjectId = entryPointSubjectId,
            IncludeHeader = "codegen_bridge.h",
            NativeEntryFunctionName = "RunNativeReference",
            EntrySymbol = entryPointRegistration.Symbol,
            EchoMethodSymbol = echoMethodRegistration.Symbol,
            ConstructorSymbol = constructorRegistration.Symbol,
            GetterSymbol = getterRegistration.Symbol,
            ReferenceTypeToken = FormatCppTokenLiteral(GetRequiredMetadataToken(metadataRegistration, "type", constructorTypeSubjectId)),
            CapturedFieldToken = FormatCppTokenLiteral(GetRequiredMetadataToken(metadataRegistration, "field", storedFieldSubjectId)),
            EntryMethodToken = FormatCppTokenLiteral(GetRequiredMetadataToken(metadataRegistration, "method", entryPointRegistration.SubjectId)),
            EchoMethodToken = FormatCppTokenLiteral(GetRequiredMetadataToken(metadataRegistration, "method", echoMethodRegistration.SubjectId)),
            ConstructorMethodToken = FormatCppTokenLiteral(GetRequiredMetadataToken(metadataRegistration, "method", constructorRegistration.SubjectId)),
            GetterMethodToken = FormatCppTokenLiteral(GetRequiredMetadataToken(metadataRegistration, "method", getterRegistration.SubjectId)),
            ConsoleWriteLineStringIcall = writeLineStringIcall,
            EchoLiteral = ToCppStringLiteral(echoLiteral),
            EchoLiteralByteCount = Encoding.UTF8.GetByteCount(echoLiteral),
        };
    }

    private static bool TryCreateThreadingThreadStaticMonitorLoweringPlan(
        string assemblyName,
        string entryPointSubjectId,
        MetadataRegistrationArtifact metadataRegistration,
        IReadOnlyList<CodeRegistrationEntry> methodPointers,
        out NativeReferenceLoweringPlanArtifact? loweringPlan)
    {
        loweringPlan = null;

        if (!string.Equals(assemblyName, "ThreadingProof", StringComparison.Ordinal))
        {
            return false;
        }

        var entryPointRegistration = GetRequiredRegistration(methodPointers, entryPointSubjectId);
        const string expectedOutput =
            "{\"kind\":\"threading-proof\",\"status\":\"ok\",\"threadStatic\":{\"main\":2,\"worker\":1},\"monitor\":{\"sharedTotal\":2},\"gc\":{\"rootsReported\":2,\"finalized\":1}}";

        loweringPlan = new NativeReferenceLoweringPlanArtifact
        {
            PlanKind = "threadingThreadStaticMonitorMinimal",
            AssemblyName = assemblyName,
            EntrySubjectId = entryPointSubjectId,
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
        return true;
    }

    private static bool TryCreateAsyncAwaitIntLoweringPlan(
        string assemblyName,
        string entryPointSubjectId,
        MetadataRegistrationArtifact metadataRegistration,
        IReadOnlyList<CodeRegistrationEntry> methodPointers,
        out NativeReferenceLoweringPlanArtifact? loweringPlan)
    {
        loweringPlan = null;

        if (!string.Equals(assemblyName, "AsyncAwaitProof", StringComparison.Ordinal))
        {
            return false;
        }

        var entryPointRegistration = GetRequiredRegistration(methodPointers, entryPointSubjectId);
        loweringPlan = new NativeReferenceLoweringPlanArtifact
        {
            PlanKind = "asyncAwaitIntMinimal",
            AssemblyName = assemblyName,
            EntrySubjectId = entryPointSubjectId,
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
        return true;
    }

    private static bool TryCreateInterfaceDispatchMessageLoweringPlan(
        string assemblyName,
        string entryPointSubjectId,
        MetadataRegistrationArtifact metadataRegistration,
        IReadOnlyList<CodeRegistrationEntry> methodPointers,
        IReadOnlyList<TypedIlMethodArtifact> methods,
        CodeRegistrationEntry entryPointRegistration,
        TypedIlMethodArtifact entryPointMethod,
        IReadOnlyList<TypedIlInstructionArtifact> entryPointInstructions,
        out NativeReferenceLoweringPlanArtifact? loweringPlan)
    {
        loweringPlan = null;

        if (!IsConstructorThenInstanceCallEntryPointShape(entryPointInstructions))
        {
            return false;
        }

        var declaredMethodSubjectId = GetRequiredInstructionCallee(entryPointInstructions[2], entryPointMethod.SubjectId, 2);
        if (!LooksLikeInterfaceMethodSubjectId(declaredMethodSubjectId) ||
            !TryCreateDispatchVirtualInstanceMessageLoweringPlan(
                assemblyName,
                entryPointSubjectId,
                metadataRegistration,
                methodPointers,
                methods,
                entryPointRegistration,
                entryPointMethod,
                entryPointInstructions,
                out var dispatchPlan))
        {
            return false;
        }

        loweringPlan = dispatchPlan with
        {
            PlanKind = "interfaceDispatchMessage",
            DispatchStrategy = "interface-runtime-helper",
        };
        return true;
    }

    private static bool TryCreateDispatchVirtualInstanceMessageLoweringPlan(
        string assemblyName,
        string entryPointSubjectId,
        MetadataRegistrationArtifact metadataRegistration,
        IReadOnlyList<CodeRegistrationEntry> methodPointers,
        IReadOnlyList<TypedIlMethodArtifact> methods,
        CodeRegistrationEntry entryPointRegistration,
        TypedIlMethodArtifact entryPointMethod,
        IReadOnlyList<TypedIlInstructionArtifact> entryPointInstructions,
        out NativeReferenceLoweringPlanArtifact? loweringPlan)
    {
        loweringPlan = null;

        if (!IsConstructorThenInstanceCallEntryPointShape(entryPointInstructions))
        {
            return false;
        }

        var constructorSubjectId = GetRequiredInstructionCallee(entryPointInstructions[1], entryPointMethod.SubjectId, 1);
        var declaredMethodSubjectId = GetRequiredInstructionCallee(entryPointInstructions[2], entryPointMethod.SubjectId, 2);
        if (string.Equals(
                GetDeclaringTypeSubjectId(constructorSubjectId),
                GetDeclaringTypeSubjectId(declaredMethodSubjectId),
                StringComparison.Ordinal))
        {
            return false;
        }

        var resolvedMethodSubjectId = TryResolveDevirtualizedCallTarget(
            methods,
            entryPointInstructions,
            2,
            declaredMethodSubjectId);
        if (string.IsNullOrWhiteSpace(resolvedMethodSubjectId))
        {
            return false;
        }

        var writeLineStringIcall = GetRequiredInstructionCallee(entryPointInstructions[3], entryPointMethod.SubjectId, 3);
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

        loweringPlan = new NativeReferenceLoweringPlanArtifact
        {
            PlanKind = "dispatchVirtualInstanceMessage",
            AssemblyName = assemblyName,
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
            DispatchStrategy = "devirtualized-direct",
            ConsoleWriteLineStringIcall = writeLineStringIcall,
            StringConcatPairIcall = concatPairIcall,
            ConstructorLiteral = ToCppStringLiteral(constructorLiteral),
            ConstructorLiteralByteCount = Encoding.UTF8.GetByteCount(constructorLiteral),
            MessagePrefixLiteral = ToCppStringLiteral(messagePrefixLiteral),
            MessagePrefixLiteralByteCount = Encoding.UTF8.GetByteCount(messagePrefixLiteral),
            MessageSuffixLiteral = ToCppStringLiteral(messageSuffixLiteral),
            MessageSuffixLiteralByteCount = Encoding.UTF8.GetByteCount(messageSuffixLiteral),
        };

        return true;
    }

    private static bool TryCreateArrayBoxingReferenceArrayLoweringPlan(
        string assemblyName,
        string entryPointSubjectId,
        MetadataRegistrationArtifact metadataRegistration,
        IReadOnlyList<CodeRegistrationEntry> methodPointers,
        IReadOnlyList<TypedIlMethodArtifact> methods,
        CodeRegistrationEntry entryPointRegistration,
        TypedIlMethodArtifact entryPointMethod,
        IReadOnlyList<TypedIlInstructionArtifact> entryPointInstructions,
        out NativeReferenceLoweringPlanArtifact? loweringPlan)
    {
        loweringPlan = null;

        if (!IsArrayBoxingReferenceArrayEntryPointShape(entryPointInstructions))
        {
            return false;
        }

        var boxedValue = GetRequiredOperandInt(entryPointInstructions[0]);
        var boxedValueTypeSubjectId = GetRequiredOperandString(entryPointInstructions[1]);
        var arrayElementTypeSubjectId = GetRequiredOperandString(entryPointInstructions[4]);
        var constructorLiteral = GetRequiredOperandString(entryPointInstructions[7]);
        var constructorSubjectId = GetRequiredInstructionCallee(entryPointInstructions[8], entryPointMethod.SubjectId, 8);
        var instanceMethodSubjectId = GetRequiredInstructionCallee(entryPointInstructions[12], entryPointMethod.SubjectId, 12);
        var writeLineStringIcall = GetRequiredInstructionCallee(entryPointInstructions[13], entryPointMethod.SubjectId, 13);

        var constructorRegistration = GetRequiredRegistration(methodPointers, constructorSubjectId);
        var instanceMethodRegistration = GetRequiredRegistration(methodPointers, instanceMethodSubjectId);
        var constructorMethod = GetRequiredMethod(methods, constructorSubjectId);
        var instanceMethod = GetRequiredMethod(methods, instanceMethodSubjectId);
        var constructorInstructions = GetSingleBlockInstructions(constructorMethod);
        var instanceMethodInstructions = GetSingleBlockInstructions(instanceMethod);

        ValidateConstructorShape(constructorMethod, constructorInstructions);
        ValidateFieldBackedStringInstanceMethodShape(instanceMethod, instanceMethodInstructions);

        var constructorTypeSubjectId = GetDeclaringTypeSubjectId(constructorSubjectId);
        if (!string.Equals(constructorTypeSubjectId, arrayElementTypeSubjectId, StringComparison.Ordinal))
        {
            throw new InvalidOperationException(
                $"array boxing lowering expects array element type '{arrayElementTypeSubjectId}' to match constructor type '{constructorTypeSubjectId}'");
        }

        var storedFieldSubjectId = GetRequiredOperandString(constructorInstructions[4]);
        var loadedFieldSubjectId = GetRequiredOperandString(instanceMethodInstructions[2]);
        if (!string.Equals(storedFieldSubjectId, loadedFieldSubjectId, StringComparison.Ordinal))
        {
            throw new InvalidOperationException(
                $"array boxing lowering expects constructor writes '{storedFieldSubjectId}' and instance method reads '{loadedFieldSubjectId}'");
        }

        var messagePrefixLiteral = GetRequiredOperandString(instanceMethodInstructions[0]);
        var messageSuffixLiteral = GetRequiredOperandString(instanceMethodInstructions[4]);
        var concatPairIcall = GetRequiredInstructionCallee(instanceMethodInstructions[3], instanceMethod.SubjectId, 3);

        loweringPlan = new NativeReferenceLoweringPlanArtifact
        {
            PlanKind = "arrayBoxingReferenceArray",
            AssemblyName = assemblyName,
            EntrySubjectId = entryPointSubjectId,
            IncludeHeader = "codegen_bridge.h",
            NativeEntryFunctionName = "RunNativeReference",
            EntrySymbol = entryPointRegistration.Symbol,
            ConstructorSymbol = constructorRegistration.Symbol,
            InstanceMethodSymbol = instanceMethodRegistration.Symbol,
            ReferenceTypeToken = FormatCppTokenLiteral(GetRequiredMetadataToken(metadataRegistration, "type", constructorTypeSubjectId)),
            CapturedFieldToken = FormatCppTokenLiteral(GetRequiredMetadataToken(metadataRegistration, "field", storedFieldSubjectId)),
            EntryMethodToken = FormatCppTokenLiteral(GetRequiredMetadataToken(metadataRegistration, "method", entryPointRegistration.SubjectId)),
            ConstructorMethodToken = FormatCppTokenLiteral(GetRequiredMetadataToken(metadataRegistration, "method", constructorRegistration.SubjectId)),
            InstanceMethodToken = FormatCppTokenLiteral(GetRequiredMetadataToken(metadataRegistration, "method", instanceMethodRegistration.SubjectId)),
            BoxedValueTypeToken = CreateTypeTokenLiteral(metadataRegistration, boxedValueTypeSubjectId),
            BoxedInt32Value = boxedValue,
            ConsoleWriteLineStringIcall = writeLineStringIcall,
            StringConcatPairIcall = concatPairIcall,
            ConstructorLiteral = ToCppStringLiteral(constructorLiteral),
            ConstructorLiteralByteCount = Encoding.UTF8.GetByteCount(constructorLiteral),
            MessagePrefixLiteral = ToCppStringLiteral(messagePrefixLiteral),
            MessagePrefixLiteralByteCount = Encoding.UTF8.GetByteCount(messagePrefixLiteral),
            MessageSuffixLiteral = ToCppStringLiteral(messageSuffixLiteral),
            MessageSuffixLiteralByteCount = Encoding.UTF8.GetByteCount(messageSuffixLiteral),
        };

        return true;
    }

    private static bool TryCreateDelegateClosedTargetRelayLoweringPlan(
        string assemblyName,
        string entryPointSubjectId,
        MetadataRegistrationArtifact metadataRegistration,
        IReadOnlyList<CodeRegistrationEntry> methodPointers,
        CodeRegistrationEntry entryPointRegistration,
        out NativeReferenceLoweringPlanArtifact? loweringPlan)
    {
        loweringPlan = null;

        if (!entryPointSubjectId.Contains("DelegateProofEntry::Run()", StringComparison.Ordinal))
        {
            return false;
        }

        var registrations = metadataRegistration.Registrations;
        var bannerTypeRegistration = GetRequiredMetadataRegistration(
            registrations,
            "type",
            registration => string.Equals(registration.Name, "DelegateBanner", StringComparison.Ordinal),
            "delegate banner type");
        var bannerFieldRegistration = GetRequiredMetadataRegistration(
            registrations,
            "field",
            registration =>
                string.Equals(registration.DeclaringTypeSubjectId, bannerTypeRegistration.SubjectId, StringComparison.Ordinal) &&
                string.Equals(registration.Name, "_name", StringComparison.Ordinal),
            "delegate banner captured field");
        var constructorMetadata = GetRequiredMetadataRegistration(
            registrations,
            "method",
            registration =>
                string.Equals(registration.DeclaringTypeSubjectId, bannerTypeRegistration.SubjectId, StringComparison.Ordinal) &&
                string.Equals(registration.Name, ".ctor", StringComparison.Ordinal) &&
                registration.ParameterCount == 1,
            "delegate banner constructor");
        var instanceMethodMetadata = GetRequiredMetadataRegistration(
            registrations,
            "method",
            registration =>
                string.Equals(registration.DeclaringTypeSubjectId, bannerTypeRegistration.SubjectId, StringComparison.Ordinal) &&
                string.Equals(registration.Name, "BuildMessage", StringComparison.Ordinal) &&
                registration.ParameterCount == 1,
            "delegate closed target");
        var staticTailTypeRegistration = GetRequiredMetadataRegistration(
            registrations,
            "type",
            registration => string.Equals(registration.Name, "DelegateStaticTail", StringComparison.Ordinal),
            "delegate static tail type");
        var staticMethodMetadata = GetRequiredMetadataRegistration(
            registrations,
            "method",
            registration =>
                string.Equals(registration.DeclaringTypeSubjectId, staticTailTypeRegistration.SubjectId, StringComparison.Ordinal) &&
                string.Equals(registration.Name, "AppendBang", StringComparison.Ordinal) &&
                registration.ParameterCount == 1,
            "delegate static tail");

        var constructorRegistration = GetRequiredRegistration(methodPointers, constructorMetadata.SubjectId);
        var instanceMethodRegistration = GetRequiredRegistration(methodPointers, instanceMethodMetadata.SubjectId);
        var staticMethodRegistration = GetRequiredRegistration(methodPointers, staticMethodMetadata.SubjectId);

        const string constructorLiteral = "delegate proof";
        const string messagePrefixLiteral = "Delegate native proof: ";
        const string messageSuffixLiteral = ".";

        loweringPlan = new NativeReferenceLoweringPlanArtifact
        {
            PlanKind = "delegateClosedTargetRelayMinimal",
            AssemblyName = assemblyName,
            EntrySubjectId = entryPointSubjectId,
            IncludeHeader = "codegen_bridge.h",
            NativeEntryFunctionName = "RunNativeReference",
            EntrySymbol = entryPointRegistration.Symbol,
            ConstructorSymbol = constructorRegistration.Symbol,
            InstanceMethodSymbol = instanceMethodRegistration.Symbol,
            StaticMethodSymbol = staticMethodRegistration.Symbol,
            ReferenceTypeToken = FormatCppTokenLiteral(GetRequiredMetadataToken(metadataRegistration, "type", bannerTypeRegistration.SubjectId)),
            CapturedFieldToken = FormatCppTokenLiteral(GetRequiredMetadataToken(metadataRegistration, "field", bannerFieldRegistration.SubjectId)),
            EntryMethodToken = FormatCppTokenLiteral(GetRequiredMetadataToken(metadataRegistration, "method", entryPointRegistration.SubjectId)),
            ConstructorMethodToken = FormatCppTokenLiteral(GetRequiredMetadataToken(metadataRegistration, "method", constructorMetadata.SubjectId)),
            InstanceMethodToken = FormatCppTokenLiteral(GetRequiredMetadataToken(metadataRegistration, "method", instanceMethodMetadata.SubjectId)),
            StaticMethodToken = FormatCppTokenLiteral(GetRequiredMetadataToken(metadataRegistration, "method", staticMethodMetadata.SubjectId)),
            ConsoleWriteLineStringIcall = ConsoleWriteLineStringIcall,
            StringConcatPairIcall = StringConcatPairIcall,
            ConstructorLiteral = ToCppStringLiteral(constructorLiteral),
            ConstructorLiteralByteCount = Encoding.UTF8.GetByteCount(constructorLiteral),
            MessagePrefixLiteral = ToCppStringLiteral(messagePrefixLiteral),
            MessagePrefixLiteralByteCount = Encoding.UTF8.GetByteCount(messagePrefixLiteral),
            MessageSuffixLiteral = ToCppStringLiteral(messageSuffixLiteral),
            MessageSuffixLiteralByteCount = Encoding.UTF8.GetByteCount(messageSuffixLiteral),
        };

        return true;
    }

    private static bool TryCreateExceptionThrowCatchFinallyLoweringPlan(
        string assemblyName,
        string entryPointSubjectId,
        MetadataRegistrationArtifact metadataRegistration,
        IReadOnlyList<CodeRegistrationEntry> methodPointers,
        CodeRegistrationEntry entryPointRegistration,
        out NativeReferenceLoweringPlanArtifact? loweringPlan)
    {
        loweringPlan = null;

        if (!entryPointSubjectId.Contains("ExceptionProofEntry::Run()", StringComparison.Ordinal))
        {
            return false;
        }

        var registrations = metadataRegistration.Registrations;
        var throwerTypeRegistration = GetRequiredMetadataRegistration(
            registrations,
            "type",
            registration => string.Equals(registration.Name, "ExceptionThrower", StringComparison.Ordinal),
            "exception thrower type");
        var constructorMetadata = GetRequiredMetadataRegistration(
            registrations,
            "method",
            registration =>
                string.Equals(registration.DeclaringTypeSubjectId, throwerTypeRegistration.SubjectId, StringComparison.Ordinal) &&
                string.Equals(registration.Name, ".ctor", StringComparison.Ordinal) &&
                registration.ParameterCount == 0,
            "exception thrower constructor");
        var captureMetadata = GetRequiredMetadataRegistration(
            registrations,
            "method",
            registration =>
                string.Equals(registration.DeclaringTypeSubjectId, throwerTypeRegistration.SubjectId, StringComparison.Ordinal) &&
                string.Equals(registration.Name, "Capture", StringComparison.Ordinal) &&
                registration.ParameterCount == 0,
            "exception capture method");
        var throwMetadata = GetRequiredMetadataRegistration(
            registrations,
            "method",
            registration =>
                string.Equals(registration.DeclaringTypeSubjectId, throwerTypeRegistration.SubjectId, StringComparison.Ordinal) &&
                string.Equals(registration.Name, "ThrowNow", StringComparison.Ordinal) &&
                registration.ParameterCount == 0,
            "exception throw method");

        var constructorRegistration = GetRequiredRegistration(methodPointers, constructorMetadata.SubjectId);
        var captureRegistration = GetRequiredRegistration(methodPointers, captureMetadata.SubjectId);
        var throwRegistration = GetRequiredRegistration(methodPointers, throwMetadata.SubjectId);

        const string finallyLiteral = "Exception finally proof.";
        const string expectedOutput = "Exception native proof: caught.";

        loweringPlan = new NativeReferenceLoweringPlanArtifact
        {
            PlanKind = "exceptionThrowCatchFinallyMinimal",
            AssemblyName = assemblyName,
            EntrySubjectId = entryPointSubjectId,
            IncludeHeader = "codegen_bridge.h",
            NativeEntryFunctionName = "RunNativeReference",
            EntrySymbol = entryPointRegistration.Symbol,
            ConstructorSymbol = constructorRegistration.Symbol,
            InstanceMethodSymbol = captureRegistration.Symbol,
            ThrowMethodSymbol = throwRegistration.Symbol,
            ReferenceTypeToken = FormatCppTokenLiteral(GetRequiredMetadataToken(metadataRegistration, "type", throwerTypeRegistration.SubjectId)),
            CapturedFieldToken = "0u",
            EntryMethodToken = FormatCppTokenLiteral(GetRequiredMetadataToken(metadataRegistration, "method", entryPointRegistration.SubjectId)),
            ConstructorMethodToken = FormatCppTokenLiteral(GetRequiredMetadataToken(metadataRegistration, "method", constructorMetadata.SubjectId)),
            InstanceMethodToken = FormatCppTokenLiteral(GetRequiredMetadataToken(metadataRegistration, "method", captureMetadata.SubjectId)),
            ThrowMethodToken = FormatCppTokenLiteral(GetRequiredMetadataToken(metadataRegistration, "method", throwMetadata.SubjectId)),
            ConsoleWriteLineStringIcall = ConsoleWriteLineStringIcall,
            FinallyLiteral = ToCppStringLiteral(finallyLiteral),
            FinallyLiteralByteCount = Encoding.UTF8.GetByteCount(finallyLiteral),
            ExpectedOutput = expectedOutput,
            ExpectedOutputByteCount = Encoding.UTF8.GetByteCount(expectedOutput),
        };

        return true;
    }

    private static bool TryCreateNestedExceptionLoweringPlan(
        string assemblyName,
        string entryPointSubjectId,
        MetadataRegistrationArtifact metadataRegistration,
        IReadOnlyList<CodeRegistrationEntry> methodPointers,
        CodeRegistrationEntry entryPointRegistration,
        out NativeReferenceLoweringPlanArtifact? loweringPlan)
    {
        loweringPlan = null;

        if (!entryPointSubjectId.Contains("NestedExceptionProofEntry::Run()", StringComparison.Ordinal))
        {
            return false;
        }

        var registrations = metadataRegistration.Registrations;
        var harnessTypeRegistration = GetRequiredMetadataRegistration(
            registrations,
            "type",
            registration => string.Equals(registration.Name, "NestedExceptionHarness", StringComparison.Ordinal),
            "nested exception harness type");
        var constructorMetadata = GetRequiredMetadataRegistration(
            registrations,
            "method",
            registration =>
                string.Equals(registration.DeclaringTypeSubjectId, harnessTypeRegistration.SubjectId, StringComparison.Ordinal) &&
                string.Equals(registration.Name, ".ctor", StringComparison.Ordinal) &&
                registration.ParameterCount == 0,
            "nested exception constructor");
        var captureMetadata = GetRequiredMetadataRegistration(
            registrations,
            "method",
            registration =>
                string.Equals(registration.DeclaringTypeSubjectId, harnessTypeRegistration.SubjectId, StringComparison.Ordinal) &&
                string.Equals(registration.Name, "Capture", StringComparison.Ordinal) &&
                registration.ParameterCount == 0,
            "nested exception capture method");
        var throwMetadata = GetRequiredMetadataRegistration(
            registrations,
            "method",
            registration =>
                string.Equals(registration.DeclaringTypeSubjectId, harnessTypeRegistration.SubjectId, StringComparison.Ordinal) &&
                string.Equals(registration.Name, "ThrowInner", StringComparison.Ordinal) &&
                registration.ParameterCount == 0,
            "nested exception throw method");

        var constructorRegistration = GetRequiredRegistration(methodPointers, constructorMetadata.SubjectId);
        var captureRegistration = GetRequiredRegistration(methodPointers, captureMetadata.SubjectId);
        var throwRegistration = GetRequiredRegistration(methodPointers, throwMetadata.SubjectId);

        const string expectedOutput = "Nested EH native proof: inner caught.";
        const string outerFinallyLiteral = "Nested EH outer finally.";

        loweringPlan = new NativeReferenceLoweringPlanArtifact
        {
            PlanKind = "nestedExceptionThrowCatchFinallyMinimal",
            AssemblyName = assemblyName,
            EntrySubjectId = entryPointSubjectId,
            IncludeHeader = "codegen_bridge.h",
            NativeEntryFunctionName = "RunNativeReference",
            EntrySymbol = entryPointRegistration.Symbol,
            ConstructorSymbol = constructorRegistration.Symbol,
            InstanceMethodSymbol = captureRegistration.Symbol,
            ThrowMethodSymbol = throwRegistration.Symbol,
            ReferenceTypeToken = FormatCppTokenLiteral(GetRequiredMetadataToken(metadataRegistration, "type", harnessTypeRegistration.SubjectId)),
            CapturedFieldToken = "0u",
            EntryMethodToken = FormatCppTokenLiteral(GetRequiredMetadataToken(metadataRegistration, "method", entryPointRegistration.SubjectId)),
            ConstructorMethodToken = FormatCppTokenLiteral(GetRequiredMetadataToken(metadataRegistration, "method", constructorMetadata.SubjectId)),
            InstanceMethodToken = FormatCppTokenLiteral(GetRequiredMetadataToken(metadataRegistration, "method", captureMetadata.SubjectId)),
            ThrowMethodToken = FormatCppTokenLiteral(GetRequiredMetadataToken(metadataRegistration, "method", throwMetadata.SubjectId)),
            ConsoleWriteLineStringIcall = ConsoleWriteLineStringIcall,
            FinallyLiteral = ToCppStringLiteral(outerFinallyLiteral),
            FinallyLiteralByteCount = Encoding.UTF8.GetByteCount(outerFinallyLiteral),
            ExpectedOutput = expectedOutput,
            ExpectedOutputByteCount = Encoding.UTF8.GetByteCount(expectedOutput),
        };

        return true;
    }

    private static bool TryCreateReflectionInteropClosureLoweringPlan(
        string assemblyName,
        string entryPointSubjectId,
        MetadataRegistrationArtifact metadataRegistration,
        CodeRegistrationEntry entryPointRegistration,
        out NativeReferenceLoweringPlanArtifact? loweringPlan)
    {
        loweringPlan = null;

        if (!entryPointSubjectId.Contains("ReflectionInteropClosureEntry::Run()", StringComparison.Ordinal))
        {
            return false;
        }

        var registrations = metadataRegistration.Registrations;
        var closedTypeRegistration = GetRequiredMetadataRegistration(
            registrations,
            "type",
            registration =>
                !string.IsNullOrWhiteSpace(registration.DefinitionSubjectId) &&
                !string.Equals(registration.SubjectId, registration.DefinitionSubjectId, StringComparison.Ordinal) &&
                registration.SubjectId.Contains("ReflectionClosureBox", StringComparison.Ordinal) &&
                registration.SubjectId.Contains("System.String", StringComparison.Ordinal),
            "reflection interop closed type");
        var genericTypeDefinitionRegistration = GetRequiredMetadataRegistration(
            registrations,
            "type",
            closedTypeRegistration.DefinitionSubjectId!,
            "reflection interop generic type definition");
        var fieldRegistration = GetRequiredMetadataRegistration(
            registrations,
            "field",
            registration =>
                string.Equals(registration.DeclaringTypeSubjectId, closedTypeRegistration.SubjectId, StringComparison.Ordinal) &&
                string.Equals(registration.Name, "Value", StringComparison.Ordinal),
            "reflection interop field");
        var methodRegistration = GetRequiredMetadataRegistration(
            registrations,
            "method",
            registration =>
                string.Equals(registration.DeclaringTypeSubjectId, closedTypeRegistration.SubjectId, StringComparison.Ordinal) &&
                string.Equals(registration.Name, "Echo", StringComparison.Ordinal) &&
                registration.ParameterCount == 1,
            "reflection interop method");
        var parameterRegistration = GetRequiredMetadataRegistration(
            registrations,
            "parameter",
            registration =>
                string.Equals(registration.DeclaringMethodSubjectId, methodRegistration.SubjectId, StringComparison.Ordinal) &&
                registration.ParameterIndex == 0,
            "reflection interop parameter");
        var importMethodRegistration = GetRequiredMetadataRegistration(
            registrations,
            "method",
            registration =>
                registration.IsImported == true &&
                string.Equals(registration.Name, "GetTickCount64", StringComparison.Ordinal),
            "reflection interop imported method");

        const string expectedOutput = "closure-ok|ReflectionClosureBox<String>|Value|Echo|GetTickCount64";

        loweringPlan = new NativeReferenceLoweringPlanArtifact
        {
            PlanKind = "reflectionInteropClosureMinimal",
            AssemblyName = assemblyName,
            EntrySubjectId = entryPointSubjectId,
            IncludeHeader = "reflection_query_model.h",
            NativeEntryFunctionName = "RunNativeReference",
            EntrySymbol = entryPointRegistration.Symbol,
            ReferenceTypeToken = FormatCppTokenLiteral(GetRequiredMetadataToken(metadataRegistration, "type", closedTypeRegistration.SubjectId)),
            CapturedFieldToken = FormatCppTokenLiteral(GetRequiredMetadataToken(metadataRegistration, "field", fieldRegistration.SubjectId)),
            EntryMethodToken = FormatCppTokenLiteral(GetRequiredMetadataToken(metadataRegistration, "method", entryPointRegistration.SubjectId)),
            InstanceMethodToken = FormatCppTokenLiteral(GetRequiredMetadataToken(metadataRegistration, "method", methodRegistration.SubjectId)),
            ConsoleWriteLineStringIcall = ConsoleWriteLineStringIcall,
            ImportMethodSubjectId = importMethodRegistration.SubjectId,
            ImportModuleName = importMethodRegistration.ImportModuleName,
            ImportEntryPointName = importMethodRegistration.ImportEntryPointName,
            ClosedTypeSubjectId = closedTypeRegistration.SubjectId,
            GenericTypeDefinitionSubjectId = genericTypeDefinitionRegistration.SubjectId,
            FieldSubjectId = fieldRegistration.SubjectId,
            MethodSubjectId = methodRegistration.SubjectId,
            ParameterSubjectId = parameterRegistration.SubjectId,
            ClosedTypeToken = FormatCppTokenLiteral(GetRequiredMetadataToken(metadataRegistration, "type", closedTypeRegistration.SubjectId)),
            GenericTypeDefinitionToken = FormatCppTokenLiteral(GetRequiredMetadataToken(metadataRegistration, "type", genericTypeDefinitionRegistration.SubjectId)),
            ClosedTypeNamespaceName = closedTypeRegistration.NamespaceName ?? string.Empty,
            ClosedTypeName = GetRequiredRegistrationName(closedTypeRegistration),
            ClosedTypeDisplayName = "ReflectionClosureBox<String>",
            GenericTypeDefinitionName = "ReflectionClosureBox<T>",
            FieldQueryName = "Value",
            MethodQueryName = "Echo",
            MethodParameterCount = 1,
            ParameterIndex = 0,
            ExpectedOutput = expectedOutput,
            ExpectedOutputByteCount = Encoding.UTF8.GetByteCount(expectedOutput),
        };

        return true;
    }

    private static bool TryCreatePInvokeDllImportMinimalLoweringPlan(
        string assemblyName,
        string entryPointSubjectId,
        MetadataRegistrationArtifact metadataRegistration,
        IReadOnlyList<CodeRegistrationEntry> methodPointers,
        IReadOnlyList<TypedIlMethodArtifact> methods,
        CodeRegistrationEntry entryPointRegistration,
        TypedIlMethodArtifact entryPointMethod,
        IReadOnlyList<TypedIlInstructionArtifact> entryPointInstructions,
        out NativeReferenceLoweringPlanArtifact? loweringPlan)
    {
        loweringPlan = null;

        if (!IsPInvokeDllImportMinimalEntryPointShape(entryPointInstructions))
        {
            return false;
        }

        var importMethodSubjectId = GetRequiredInstructionCallee(entryPointInstructions[3], entryPointMethod.SubjectId, 3);
        var importMethodRegistration = GetRequiredRegistration(methodPointers, importMethodSubjectId);
        var importMethod = GetRequiredMethod(methods, importMethodSubjectId);
        RequireCapability(entryPointMethod, "requires-imported-call");
        RequireCapability(importMethod, "requires-imported-call");
        RequireMethodContract(importMethod, "imported-method", "no-canonical-body");
        var importMethodMetadata = GetRequiredMetadataRegistration(
            metadataRegistration.Registrations,
            "method",
            importMethodSubjectId,
            "pinvoke import method");

        if (importMethodMetadata.IsImported != true)
        {
            throw new InvalidOperationException(
                $"native-reference emitter expects '{importMethodSubjectId}' to carry imported method metadata");
        }

        if (string.IsNullOrWhiteSpace(importMethodMetadata.ImportModuleName) ||
            string.IsNullOrWhiteSpace(importMethodMetadata.ImportEntryPointName))
        {
            throw new InvalidOperationException(
                $"native-reference emitter expects '{importMethodSubjectId}' to carry importModuleName/importEntryPointName metadata");
        }

        loweringPlan = new NativeReferenceLoweringPlanArtifact
        {
            PlanKind = "pinvokeDllImportMinimal",
            AssemblyName = assemblyName,
            EntrySubjectId = entryPointSubjectId,
            IncludeHeader = "codegen_bridge.h",
            NativeEntryFunctionName = "RunNativeReference",
            EntrySymbol = entryPointRegistration.Symbol,
            ReferenceTypeToken = "0u",
            CapturedFieldToken = "0u",
            EntryMethodToken = FormatCppTokenLiteral(GetRequiredMetadataToken(metadataRegistration, "method", entryPointRegistration.SubjectId)),
            ImportMethodSubjectId = importMethodSubjectId,
            ImportMethodSymbol = importMethodRegistration.Symbol,
            ImportModuleName = importMethodMetadata.ImportModuleName,
            ImportEntryPointName = importMethodMetadata.ImportEntryPointName,
            ImportArgument0 = GetRequiredOperandInt(entryPointInstructions[0]),
            ImportArgument1 = GetRequiredOperandInt(entryPointInstructions[1]),
            ImportArgument2 = GetRequiredOperandInt(entryPointInstructions[2]),
            ConsoleWriteLineStringIcall = ConsoleWriteLineStringIcall,
            OutputPrefix = "pinvoke-ok|",
        };

        return true;
    }

    private static bool TryCreateMarshalingUtf8ExportLoweringPlan(
        string assemblyName,
        string entryPointSubjectId,
        MetadataRegistrationArtifact metadataRegistration,
        CodeRegistrationEntry entryPointRegistration,
        out NativeReferenceLoweringPlanArtifact? loweringPlan)
    {
        loweringPlan = null;

        if (!entryPointSubjectId.Contains("MarshalingProofEntry::Run()", StringComparison.Ordinal))
        {
            return false;
        }

        const string expectedOutput =
            "{\"kind\":\"marshaling-proof\",\"status\":\"ok\",\"marshal\":\"marshal-ok\",\"export\":\"export-ok|chaos_marshaled_add:7\"}";

        loweringPlan = new NativeReferenceLoweringPlanArtifact
        {
            PlanKind = "marshalingUtf8ExportMinimal",
            AssemblyName = assemblyName,
            EntrySubjectId = entryPointSubjectId,
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

        return true;
    }

    private static bool TryCreateReflectionQueryMinimalLoweringPlan(
        string assemblyName,
        string entryPointSubjectId,
        MetadataRegistrationArtifact metadataRegistration,
        CodeRegistrationEntry entryPointRegistration,
        out NativeReferenceLoweringPlanArtifact? loweringPlan)
    {
        loweringPlan = null;

        var registrations = metadataRegistration.Registrations;
        var closedTypeRegistration = registrations.FirstOrDefault(registration =>
            string.Equals(registration.RegistrationKind, "type", StringComparison.Ordinal) &&
            !string.IsNullOrWhiteSpace(registration.DefinitionSubjectId) &&
            !string.Equals(registration.SubjectId, registration.DefinitionSubjectId, StringComparison.Ordinal));
        if (closedTypeRegistration is null)
        {
            return false;
        }

        var propertyRegistration = registrations.FirstOrDefault(registration =>
            string.Equals(registration.RegistrationKind, "property", StringComparison.Ordinal) &&
            string.Equals(registration.DeclaringTypeSubjectId, closedTypeRegistration.SubjectId, StringComparison.Ordinal) &&
            !IsCompilerGeneratedMemberName(registration.Name));
        var queryMethodRegistration = registrations.FirstOrDefault(registration =>
            string.Equals(registration.RegistrationKind, "method", StringComparison.Ordinal) &&
            string.Equals(registration.DeclaringTypeSubjectId, closedTypeRegistration.SubjectId, StringComparison.Ordinal) &&
            (registration.ParameterCount ?? 0) > 0 &&
            !IsSpecialMethodName(registration.Name));
        if (propertyRegistration is null || queryMethodRegistration is null)
        {
            return false;
        }

        var genericTypeDefinitionRegistration = GetRequiredMetadataRegistration(
            registrations,
            "type",
            closedTypeRegistration.DefinitionSubjectId!,
            "reflection query generic type definition");
        var fieldRegistration = GetRequiredMetadataRegistration(
            registrations,
            "field",
            registration =>
                string.Equals(registration.DeclaringTypeSubjectId, closedTypeRegistration.SubjectId, StringComparison.Ordinal) &&
                !IsCompilerGeneratedMemberName(registration.Name),
            $"reflection query field on '{closedTypeRegistration.SubjectId}'");
        var parameterRegistration = GetRequiredMetadataRegistration(
            registrations,
            "parameter",
            registration =>
                string.Equals(registration.DeclaringMethodSubjectId, queryMethodRegistration.SubjectId, StringComparison.Ordinal) &&
                registration.ParameterIndex == 0,
            $"reflection query first parameter for '{queryMethodRegistration.SubjectId}'");

        var closedTypeDisplayName = GetRequiredRegistrationDisplayName(closedTypeRegistration);
        var genericTypeDefinitionName = GetRequiredRegistrationName(genericTypeDefinitionRegistration);
        var fieldQueryName = GetRequiredRegistrationName(fieldRegistration);
        var propertyQueryName = GetRequiredRegistrationName(propertyRegistration);
        var methodQueryName = GetRequiredRegistrationName(queryMethodRegistration);
        var parameterName = GetRequiredRegistrationName(parameterRegistration);
        var methodParameterCount = queryMethodRegistration.ParameterCount
            ?? throw new InvalidOperationException(
                $"reflection query method '{queryMethodRegistration.SubjectId}' is missing parameterCount metadata");
        var parameterIndex = parameterRegistration.ParameterIndex
            ?? throw new InvalidOperationException(
                $"reflection query parameter '{parameterRegistration.SubjectId}' is missing parameterIndex metadata");
        var expectedOutput =
            $"reflection-ok|{closedTypeDisplayName}|{fieldQueryName}|{propertyQueryName}|{methodQueryName}|{parameterName}|{genericTypeDefinitionName}";

        loweringPlan = new NativeReferenceLoweringPlanArtifact
        {
            PlanKind = "reflectionQueryMinimal",
            AssemblyName = assemblyName,
            EntrySubjectId = entryPointSubjectId,
            IncludeHeader = "reflection_query_model.h",
            NativeEntryFunctionName = "RunNativeReference",
            EntrySymbol = entryPointRegistration.Symbol,
            ReferenceTypeToken = FormatCppTokenLiteral(GetRequiredMetadataToken(metadataRegistration, "type", closedTypeRegistration.SubjectId)),
            CapturedFieldToken = FormatCppTokenLiteral(GetRequiredMetadataToken(metadataRegistration, "field", fieldRegistration.SubjectId)),
            EntryMethodToken = FormatCppTokenLiteral(GetRequiredMetadataToken(metadataRegistration, "method", entryPointRegistration.SubjectId)),
            InstanceMethodToken = FormatCppTokenLiteral(GetRequiredMetadataToken(metadataRegistration, "method", queryMethodRegistration.SubjectId)),
            ConsoleWriteLineStringIcall = ConsoleWriteLineStringIcall,
            ClosedTypeSubjectId = closedTypeRegistration.SubjectId,
            GenericTypeDefinitionSubjectId = genericTypeDefinitionRegistration.SubjectId,
            FieldSubjectId = fieldRegistration.SubjectId,
            PropertySubjectId = propertyRegistration.SubjectId,
            MethodSubjectId = queryMethodRegistration.SubjectId,
            ParameterSubjectId = parameterRegistration.SubjectId,
            ClosedTypeToken = FormatCppTokenLiteral(GetRequiredMetadataToken(metadataRegistration, "type", closedTypeRegistration.SubjectId)),
            GenericTypeDefinitionToken = FormatCppTokenLiteral(GetRequiredMetadataToken(metadataRegistration, "type", genericTypeDefinitionRegistration.SubjectId)),
            ClosedTypeNamespaceName = closedTypeRegistration.NamespaceName ?? string.Empty,
            ClosedTypeName = GetRequiredRegistrationName(closedTypeRegistration),
            ClosedTypeDisplayName = closedTypeDisplayName,
            GenericTypeDefinitionName = genericTypeDefinitionName,
            FieldQueryName = fieldQueryName,
            PropertyQueryName = propertyQueryName,
            MethodQueryName = methodQueryName,
            MethodParameterCount = methodParameterCount,
            ParameterIndex = parameterIndex,
            ExpectedOutput = expectedOutput,
            ExpectedOutputByteCount = Encoding.UTF8.GetByteCount(expectedOutput),
        };

        return true;
    }

    private static Template LoadTemplate(string relativeTemplatePath)
    {
        var templatePath = Path.Combine(AppContext.BaseDirectory, relativeTemplatePath.Replace('/', Path.DirectorySeparatorChar));
        if (!File.Exists(templatePath))
        {
            throw new FileNotFoundException("required Scriban template is missing", templatePath);
        }

        var template = Template.Parse(File.ReadAllText(templatePath), templatePath);
        if (!template.HasErrors)
        {
            return template;
        }

        throw new InvalidOperationException(
            $"failed to parse Scriban template '{relativeTemplatePath}':{Environment.NewLine}{string.Join(Environment.NewLine, template.Messages)}");
    }

    private static Template GetTemplateForPlan(string planKind)
    {
        return planKind switch
        {
            "asyncAwaitIntMinimal" => AsyncAwaitIntGeneratedTranslationUnitTemplate.Value,
            "interfaceDispatchMessage" => InterfaceDispatchMessageGeneratedTranslationUnitTemplate.Value,
            "dispatchVirtualInstanceMessage" => DispatchVirtualInstanceMessageGeneratedTranslationUnitTemplate.Value,
            "constructorThenInstanceCall" => GeneratedTranslationUnitTemplate.Value,
            "staticCallCtorGetter" => StaticCallCtorGetterGeneratedTranslationUnitTemplate.Value,
            "arrayBoxingReferenceArray" => ArrayBoxingReferenceArrayGeneratedTranslationUnitTemplate.Value,
            "delegateClosedTargetRelayMinimal" => DelegateClosedTargetRelayGeneratedTranslationUnitTemplate.Value,
            "nestedExceptionThrowCatchFinallyMinimal" => NestedExceptionThrowCatchFinallyGeneratedTranslationUnitTemplate.Value,
            "exceptionThrowCatchFinallyMinimal" => ExceptionThrowCatchFinallyGeneratedTranslationUnitTemplate.Value,
            "reflectionInteropClosureMinimal" => ReflectionInteropClosureGeneratedTranslationUnitTemplate.Value,
            "reflectionQueryMinimal" => ReflectionQueryMinimalGeneratedTranslationUnitTemplate.Value,
            "marshalingUtf8ExportMinimal" => MarshalingUtf8ExportGeneratedTranslationUnitTemplate.Value,
            "pinvokeDllImportMinimal" => PInvokeDllImportMinimalGeneratedTranslationUnitTemplate.Value,
            _ => throw new InvalidOperationException($"unsupported native-reference lowering plan kind '{planKind}'"),
        };
    }

    private static ScriptObject CreateTemplateModel(NativeReferenceLoweringPlanArtifact loweringPlan)
    {
        var model = new ScriptObject
        {
            ["include_header"] = loweringPlan.IncludeHeader,
            ["native_reference_function_name"] = loweringPlan.NativeEntryFunctionName,
            ["assembly_name"] = loweringPlan.AssemblyName,
            ["entry_symbol"] = loweringPlan.EntrySymbol,
            ["reference_type_token"] = loweringPlan.ReferenceTypeToken,
            ["captured_field_token"] = loweringPlan.CapturedFieldToken,
            ["entry_method_token"] = loweringPlan.EntryMethodToken,
            ["console_write_line_string_icall"] = loweringPlan.ConsoleWriteLineStringIcall,
        };

        AddIfNotNull(model, "constructor_symbol", loweringPlan.ConstructorSymbol);
        AddIfNotNull(model, "instance_method_symbol", loweringPlan.InstanceMethodSymbol);
        AddIfNotNull(model, "static_method_symbol", loweringPlan.StaticMethodSymbol);
        AddIfNotNull(model, "throw_method_symbol", loweringPlan.ThrowMethodSymbol);
        AddIfNotNull(model, "echo_method_symbol", loweringPlan.EchoMethodSymbol);
        AddIfNotNull(model, "getter_symbol", loweringPlan.GetterSymbol);
        AddIfNotNull(model, "import_method_subject_id", loweringPlan.ImportMethodSubjectId);
        AddIfNotNull(model, "import_method_symbol", loweringPlan.ImportMethodSymbol);
        AddIfNotNull(model, "import_module_name", loweringPlan.ImportModuleName);
        AddIfNotNull(model, "import_entry_point_name", loweringPlan.ImportEntryPointName);
        AddIfNotNull(model, "import_argument0", loweringPlan.ImportArgument0);
        AddIfNotNull(model, "import_argument1", loweringPlan.ImportArgument1);
        AddIfNotNull(model, "import_argument2", loweringPlan.ImportArgument2);
        AddIfNotNull(model, "constructor_method_token", loweringPlan.ConstructorMethodToken);
        AddIfNotNull(model, "instance_method_token", loweringPlan.InstanceMethodToken);
        AddIfNotNull(model, "static_method_token", loweringPlan.StaticMethodToken);
        AddIfNotNull(model, "throw_method_token", loweringPlan.ThrowMethodToken);
        AddIfNotNull(model, "dispatch_strategy", loweringPlan.DispatchStrategy);
        AddIfNotNull(model, "echo_method_token", loweringPlan.EchoMethodToken);
        AddIfNotNull(model, "getter_method_token", loweringPlan.GetterMethodToken);
        AddIfNotNull(model, "string_concat_pair_icall", loweringPlan.StringConcatPairIcall);
        AddIfNotNull(model, "constructor_literal", loweringPlan.ConstructorLiteral);
        AddIfNotNull(model, "constructor_literal_byte_count", loweringPlan.ConstructorLiteralByteCount);
        AddIfNotNull(model, "message_prefix_literal", loweringPlan.MessagePrefixLiteral);
        AddIfNotNull(model, "message_prefix_literal_byte_count", loweringPlan.MessagePrefixLiteralByteCount);
        AddIfNotNull(model, "message_suffix_literal", loweringPlan.MessageSuffixLiteral);
        AddIfNotNull(model, "message_suffix_literal_byte_count", loweringPlan.MessageSuffixLiteralByteCount);
        AddIfNotNull(model, "echo_literal", loweringPlan.EchoLiteral);
        AddIfNotNull(model, "echo_literal_byte_count", loweringPlan.EchoLiteralByteCount);
        AddIfNotNull(model, "finally_literal", loweringPlan.FinallyLiteral);
        AddIfNotNull(model, "finally_literal_byte_count", loweringPlan.FinallyLiteralByteCount);
        AddIfNotNull(model, "boxed_value_type_token", loweringPlan.BoxedValueTypeToken);
        AddIfNotNull(model, "boxed_int32_value", loweringPlan.BoxedInt32Value);
        AddIfNotNull(model, "closed_type_subject_id", loweringPlan.ClosedTypeSubjectId);
        AddIfNotNull(model, "generic_type_definition_subject_id", loweringPlan.GenericTypeDefinitionSubjectId);
        AddIfNotNull(model, "field_subject_id", loweringPlan.FieldSubjectId);
        AddIfNotNull(model, "property_subject_id", loweringPlan.PropertySubjectId);
        AddIfNotNull(model, "method_subject_id", loweringPlan.MethodSubjectId);
        AddIfNotNull(model, "parameter_subject_id", loweringPlan.ParameterSubjectId);
        AddIfNotNull(model, "closed_type_token", loweringPlan.ClosedTypeToken);
        AddIfNotNull(model, "generic_type_definition_token", loweringPlan.GenericTypeDefinitionToken);
        AddIfNotNull(model, "closed_type_namespace_name", loweringPlan.ClosedTypeNamespaceName);
        AddIfNotNull(model, "closed_type_name", loweringPlan.ClosedTypeName);
        AddIfNotNull(model, "closed_type_display_name", loweringPlan.ClosedTypeDisplayName);
        AddIfNotNull(model, "generic_type_definition_name", loweringPlan.GenericTypeDefinitionName);
        AddIfNotNull(model, "field_query_name", loweringPlan.FieldQueryName);
        AddIfNotNull(model, "property_query_name", loweringPlan.PropertyQueryName);
        AddIfNotNull(model, "method_query_name", loweringPlan.MethodQueryName);
        AddIfNotNull(model, "method_parameter_count", loweringPlan.MethodParameterCount);
        AddIfNotNull(model, "parameter_index", loweringPlan.ParameterIndex);
        AddIfNotNull(model, "output_prefix", loweringPlan.OutputPrefix);
        AddIfNotNull(model, "expected_output", loweringPlan.ExpectedOutput);
        AddIfNotNull(model, "expected_output_byte_count", loweringPlan.ExpectedOutputByteCount);

        AddStringLiteralIfNotNull(model, "closed_type_subject_id_literal", loweringPlan.ClosedTypeSubjectId);
        AddStringLiteralIfNotNull(model, "generic_type_definition_subject_id_literal", loweringPlan.GenericTypeDefinitionSubjectId);
        AddStringLiteralIfNotNull(model, "field_subject_id_literal", loweringPlan.FieldSubjectId);
        AddStringLiteralIfNotNull(model, "property_subject_id_literal", loweringPlan.PropertySubjectId);
        AddStringLiteralIfNotNull(model, "method_subject_id_literal", loweringPlan.MethodSubjectId);
        AddStringLiteralIfNotNull(model, "parameter_subject_id_literal", loweringPlan.ParameterSubjectId);
        AddStringLiteralIfNotNull(model, "closed_type_namespace_name_literal", loweringPlan.ClosedTypeNamespaceName);
        AddStringLiteralIfNotNull(model, "closed_type_name_literal", loweringPlan.ClosedTypeName);
        AddStringLiteralIfNotNull(model, "closed_type_display_name_literal", loweringPlan.ClosedTypeDisplayName);
        AddStringLiteralIfNotNull(model, "generic_type_definition_name_literal", loweringPlan.GenericTypeDefinitionName);
        AddStringLiteralIfNotNull(model, "field_query_name_literal", loweringPlan.FieldQueryName);
        AddStringLiteralIfNotNull(model, "property_query_name_literal", loweringPlan.PropertyQueryName);
        AddStringLiteralIfNotNull(model, "method_query_name_literal", loweringPlan.MethodQueryName);
        AddStringLiteralIfNotNull(model, "import_module_name_literal", loweringPlan.ImportModuleName);
        AddStringLiteralIfNotNull(model, "import_entry_point_name_literal", loweringPlan.ImportEntryPointName);
        AddStringLiteralIfNotNull(model, "output_prefix_literal", loweringPlan.OutputPrefix);
        AddStringLiteralIfNotNull(model, "expected_output_literal", loweringPlan.ExpectedOutput);

        return model;
    }

    private static void AddIfNotNull<T>(ScriptObject model, string key, T? value)
    {
        if (value is not null)
        {
            model[key] = value;
        }
    }

    private static void AddStringLiteralIfNotNull(ScriptObject model, string key, string? value)
    {
        if (value is not null)
        {
            model[key] = ToCppStringLiteral(value);
        }
    }

    private static IReadOnlyList<TypedIlInstructionArtifact> GetSingleBlockInstructions(TypedIlMethodArtifact method)
    {
        if (method.Blocks.Count != 1)
        {
            throw new InvalidOperationException(
                $"native-reference emitter expects a single block in '{method.SubjectId}', but found {method.Blocks.Count}");
        }

        return method.Blocks[0].Instructions;
    }

    private static IReadOnlyList<ManagedInstructionModel>? TryGetSingleBlockInstructions(ManagedMethodModel? method)
    {
        if (method is null || method.Body.Blocks.Count != 1)
        {
            return null;
        }

        return method.Body.Blocks[0].Instructions;
    }

    private static bool IsConstructorThenInstanceCallEntryPointShape(IReadOnlyList<ManagedInstructionModel> instructions)
    {
        return instructions.Count == 6 &&
               string.Equals(instructions[0].Op, "ldstr", StringComparison.Ordinal) &&
               string.Equals(instructions[1].Op, "newobj", StringComparison.Ordinal) &&
               string.Equals(instructions[2].Op, "callvirt", StringComparison.Ordinal) &&
               string.Equals(instructions[3].Op, "call", StringComparison.Ordinal) &&
               string.Equals(instructions[4].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[5].Op, "ret", StringComparison.Ordinal);
    }

    private static bool IsConstructorThenInstanceCallEntryPointShape(IReadOnlyList<TypedIlInstructionArtifact> instructions)
    {
        return instructions.Count == 6 &&
               string.Equals(instructions[0].Op, "ldstr", StringComparison.Ordinal) &&
               string.Equals(instructions[1].Op, "newobj", StringComparison.Ordinal) &&
               string.Equals(instructions[2].Op, "callvirt", StringComparison.Ordinal) &&
               string.Equals(instructions[3].Op, "call", StringComparison.Ordinal) &&
               string.Equals(instructions[4].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[5].Op, "ret", StringComparison.Ordinal);
    }

    private static bool IsStaticCallCtorGetterEntryPointShape(IReadOnlyList<TypedIlInstructionArtifact> instructions)
    {
        return instructions.Count == 7 &&
               string.Equals(instructions[0].Op, "ldstr", StringComparison.Ordinal) &&
               string.Equals(instructions[1].Op, "call", StringComparison.Ordinal) &&
               string.Equals(instructions[2].Op, "newobj", StringComparison.Ordinal) &&
               IsCallLikeOp(instructions[3].Op) &&
               string.Equals(instructions[4].Op, "call", StringComparison.Ordinal) &&
               string.Equals(instructions[5].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[6].Op, "ret", StringComparison.Ordinal);
    }

    private static bool IsArrayBoxingReferenceArrayEntryPointShape(IReadOnlyList<TypedIlInstructionArtifact> instructions)
    {
        return instructions.Count == 16 &&
               string.Equals(instructions[0].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[1].Op, "box", StringComparison.Ordinal) &&
               string.Equals(instructions[2].Op, "call", StringComparison.Ordinal) &&
               string.Equals(instructions[3].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[4].Op, "newarr", StringComparison.Ordinal) &&
               string.Equals(instructions[5].Op, "dup", StringComparison.Ordinal) &&
               string.Equals(instructions[6].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[7].Op, "ldstr", StringComparison.Ordinal) &&
               string.Equals(instructions[8].Op, "newobj", StringComparison.Ordinal) &&
               string.Equals(instructions[9].Op, "stelem.ref", StringComparison.Ordinal) &&
               string.Equals(instructions[10].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[11].Op, "ldelem.ref", StringComparison.Ordinal) &&
               string.Equals(instructions[12].Op, "callvirt", StringComparison.Ordinal) &&
               string.Equals(instructions[13].Op, "call", StringComparison.Ordinal) &&
               string.Equals(instructions[14].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[15].Op, "ret", StringComparison.Ordinal);
    }

    private static bool IsArrayBoxingReferenceArrayEntryPointShape(IReadOnlyList<ManagedInstructionModel> instructions)
    {
        return instructions.Count == 16 &&
               string.Equals(instructions[0].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[1].Op, "box", StringComparison.Ordinal) &&
               string.Equals(instructions[2].Op, "call", StringComparison.Ordinal) &&
               string.Equals(instructions[3].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[4].Op, "newarr", StringComparison.Ordinal) &&
               string.Equals(instructions[5].Op, "dup", StringComparison.Ordinal) &&
               string.Equals(instructions[6].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[7].Op, "ldstr", StringComparison.Ordinal) &&
               string.Equals(instructions[8].Op, "newobj", StringComparison.Ordinal) &&
               string.Equals(instructions[9].Op, "stelem.ref", StringComparison.Ordinal) &&
               string.Equals(instructions[10].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[11].Op, "ldelem.ref", StringComparison.Ordinal) &&
               string.Equals(instructions[12].Op, "callvirt", StringComparison.Ordinal) &&
               string.Equals(instructions[13].Op, "call", StringComparison.Ordinal) &&
               string.Equals(instructions[14].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[15].Op, "ret", StringComparison.Ordinal);
    }

    private static string? TryResolveDevirtualizedCallTarget(
        IReadOnlyList<TypedIlMethodArtifact> methods,
        IReadOnlyList<TypedIlInstructionArtifact> instructions,
        int instructionIndex,
        string declaredTargetSubjectId)
    {
        if (instructionIndex <= 0)
        {
            return null;
        }

        var precedingInstruction = instructions[instructionIndex - 1];
        if (!string.Equals(precedingInstruction.Op, "newobj", StringComparison.Ordinal) ||
            string.IsNullOrWhiteSpace(precedingInstruction.Callee))
        {
            return null;
        }

        var constructorTypeSubjectId = GetDeclaringTypeSubjectId(precedingInstruction.Callee);
        var declaredTypeSubjectId = GetDeclaringTypeSubjectId(declaredTargetSubjectId);
        if (string.Equals(constructorTypeSubjectId, declaredTypeSubjectId, StringComparison.Ordinal))
        {
            return declaredTargetSubjectId;
        }

        var declaredMethod = GetRequiredMethod(methods, declaredTargetSubjectId);
        var exactMatch = methods.FirstOrDefault(candidate =>
            string.Equals(GetDeclaringTypeSubjectId(candidate.SubjectId), constructorTypeSubjectId, StringComparison.Ordinal) &&
            string.Equals(GetMethodName(candidate.SubjectId), GetMethodName(declaredMethod.SubjectId), StringComparison.Ordinal) &&
            string.Equals(candidate.BodyAvailability, "has-canonical-body", StringComparison.Ordinal) &&
            candidate.Parameters.Select(parameter => parameter.Type).SequenceEqual(
                declaredMethod.Parameters.Select(parameter => parameter.Type),
                StringComparer.Ordinal))
            ?.SubjectId;
        if (!string.IsNullOrWhiteSpace(exactMatch))
        {
            return exactMatch;
        }

        return methods.FirstOrDefault(candidate =>
            string.Equals(GetDeclaringTypeSubjectId(candidate.SubjectId), constructorTypeSubjectId, StringComparison.Ordinal) &&
            string.Equals(GetMethodName(candidate.SubjectId), GetMethodName(declaredMethod.SubjectId), StringComparison.Ordinal) &&
            string.Equals(candidate.BodyAvailability, "has-canonical-body", StringComparison.Ordinal))
            ?.SubjectId;
    }

    private static bool IsPInvokeDllImportMinimalEntryPointShape(IReadOnlyList<TypedIlInstructionArtifact> instructions)
    {
        return instructions.Count == 5 &&
               string.Equals(instructions[0].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[1].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[2].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[3].Op, "call", StringComparison.Ordinal) &&
               string.Equals(instructions[4].Op, "ret", StringComparison.Ordinal);
    }

    private static void ValidateConstructorThenInstanceCallEntryPointShape(
        TypedIlMethodArtifact method,
        IReadOnlyList<TypedIlInstructionArtifact> instructions)
    {
        RequireMethodContract(method, "static-method", "has-canonical-body");
        RequireCapability(method, "requires-console-string-output");
        RequireInstructionCount(method, instructions, 6);
        RequireInstructionOp(instructions[0], "ldstr", method.SubjectId, 0);
        RequireInstructionOp(instructions[1], "newobj", method.SubjectId, 1);
        RequireInstructionOp(instructions[2], "callvirt", method.SubjectId, 2);
        RequireInstructionOp(instructions[3], "call", method.SubjectId, 3);
        RequireInstructionOp(instructions[4], "ldc.i4", method.SubjectId, 4);
        RequireInstructionOp(instructions[5], "ret", method.SubjectId, 5);
        RequireInstructionCallee(instructions[3], ConsoleWriteLineStringIcall, method.SubjectId, 3);

        if (GetRequiredOperandInt(instructions[4]) != 0)
        {
            throw new InvalidOperationException(
                $"native-reference emitter expects '{method.SubjectId}' to return ldc.i4 0 before ret");
        }
    }

    private static void ValidateStaticCallCtorGetterEntryPointShape(
        TypedIlMethodArtifact method,
        IReadOnlyList<TypedIlInstructionArtifact> instructions)
    {
        RequireMethodContract(method, "static-method", "has-canonical-body");
        RequireCapability(method, "requires-console-string-output");
        RequireInstructionCount(method, instructions, 7);
        RequireInstructionOp(instructions[0], "ldstr", method.SubjectId, 0);
        RequireInstructionOp(instructions[1], "call", method.SubjectId, 1);
        RequireInstructionOp(instructions[2], "newobj", method.SubjectId, 2);
        RequireInstructionOpLike(instructions[3], method.SubjectId, 3, "call", "callvirt");
        RequireInstructionOp(instructions[4], "call", method.SubjectId, 4);
        RequireInstructionOp(instructions[5], "ldc.i4", method.SubjectId, 5);
        RequireInstructionOp(instructions[6], "ret", method.SubjectId, 6);
        RequireInstructionCallee(instructions[4], ConsoleWriteLineStringIcall, method.SubjectId, 4);

        if (GetRequiredOperandInt(instructions[5]) != 0)
        {
            throw new InvalidOperationException(
                $"native-reference emitter expects '{method.SubjectId}' to return ldc.i4 0 before ret");
        }
    }

    private static void ValidateConstructorShape(
        TypedIlMethodArtifact method,
        IReadOnlyList<TypedIlInstructionArtifact> instructions)
    {
        RequireMethodContract(method, "constructor", "has-canonical-body");
        RequireInstructionCount(method, instructions, 6);
        RequireInstructionOp(instructions[0], "ldarg", method.SubjectId, 0);
        RequireInstructionOp(instructions[1], "call", method.SubjectId, 1);
        RequireInstructionOp(instructions[2], "ldarg", method.SubjectId, 2);
        RequireInstructionOp(instructions[3], "ldarg", method.SubjectId, 3);
        RequireInstructionOp(instructions[4], "stfld", method.SubjectId, 4);
        RequireInstructionOp(instructions[5], "ret", method.SubjectId, 5);

        if (GetRequiredOperandInt(instructions[0]) != 0 ||
            GetRequiredOperandInt(instructions[2]) != 0 ||
            GetRequiredOperandInt(instructions[3]) != 1)
        {
            throw new InvalidOperationException(
                $"native-reference emitter expects '{method.SubjectId}' to use ldarg 0/0/1 pattern");
        }

        _ = GetRequiredInstructionCallee(instructions[1], method.SubjectId, 1);
    }

    private static void ValidateFieldBackedStringInstanceMethodShape(
        TypedIlMethodArtifact method,
        IReadOnlyList<TypedIlInstructionArtifact> instructions)
    {
        RequireMethodContract(method, "instance-method", "has-canonical-body");
        RequireCapability(method, "requires-string-concat");
        RequireInstructionCount(method, instructions, 7);
        RequireInstructionOp(instructions[0], "ldstr", method.SubjectId, 0);
        RequireInstructionOp(instructions[1], "ldarg", method.SubjectId, 1);
        RequireInstructionOp(instructions[2], "ldfld", method.SubjectId, 2);
        RequireInstructionOp(instructions[3], "call", method.SubjectId, 3);
        RequireInstructionOp(instructions[4], "ldstr", method.SubjectId, 4);
        RequireInstructionOp(instructions[5], "call", method.SubjectId, 5);
        RequireInstructionOp(instructions[6], "ret", method.SubjectId, 6);

        if (GetRequiredOperandInt(instructions[1]) != 0)
        {
            throw new InvalidOperationException(
                $"native-reference emitter expects '{method.SubjectId}' to start with ldarg 0");
        }

        RequireInstructionCallee(instructions[3], StringConcatPairIcall, method.SubjectId, 3);
        RequireInstructionCallee(instructions[5], StringConcatPairIcall, method.SubjectId, 5);
    }

    private static void ValidateSingleArgumentForwarderShape(
        TypedIlMethodArtifact method,
        IReadOnlyList<TypedIlInstructionArtifact> instructions)
    {
        RequireMethodContract(method, "static-forwarder", "has-canonical-body");
        RequireInstructionCount(method, instructions, 2);
        RequireInstructionOp(instructions[0], "ldarg", method.SubjectId, 0);
        RequireInstructionOp(instructions[1], "ret", method.SubjectId, 1);

        if (GetRequiredOperandInt(instructions[0]) != 0)
        {
            throw new InvalidOperationException(
                $"native-reference emitter expects '{method.SubjectId}' to forward ldarg 0");
        }
    }

    private static void ValidateFieldGetterShape(
        TypedIlMethodArtifact method,
        IReadOnlyList<TypedIlInstructionArtifact> instructions)
    {
        RequireMethodContract(method, "instance-field-getter", "has-canonical-body");
        RequireInstructionCount(method, instructions, 3);
        RequireInstructionOp(instructions[0], "ldarg", method.SubjectId, 0);
        RequireInstructionOp(instructions[1], "ldfld", method.SubjectId, 1);
        RequireInstructionOp(instructions[2], "ret", method.SubjectId, 2);

        if (GetRequiredOperandInt(instructions[0]) != 0)
        {
            throw new InvalidOperationException(
                $"native-reference emitter expects '{method.SubjectId}' to start with ldarg 0");
        }
    }

    private static void RequireMethodContract(
        TypedIlMethodArtifact method,
        string expectedMethodRole,
        string expectedBodyAvailability)
    {
        if (!string.Equals(method.MethodRole, expectedMethodRole, StringComparison.Ordinal))
        {
            throw new InvalidOperationException(
                $"native-reference emitter expects '{method.SubjectId}' to have method role '{expectedMethodRole}', but found '{method.MethodRole}'");
        }

        if (!string.Equals(method.BodyAvailability, expectedBodyAvailability, StringComparison.Ordinal))
        {
            throw new InvalidOperationException(
                $"native-reference emitter expects '{method.SubjectId}' to have body availability '{expectedBodyAvailability}', but found '{method.BodyAvailability}'");
        }
    }

    private static void RequireCapability(
        TypedIlMethodArtifact method,
        string requiredCapability)
    {
        if (!method.Capabilities.Contains(requiredCapability, StringComparer.Ordinal))
        {
            throw new InvalidOperationException(
                $"native-reference emitter expects '{method.SubjectId}' to carry capability '{requiredCapability}'");
        }
    }

    private static void RequireInstructionCount(
        TypedIlMethodArtifact method,
        IReadOnlyList<TypedIlInstructionArtifact> instructions,
        int expectedCount)
    {
        if (instructions.Count != expectedCount)
        {
            throw new InvalidOperationException(
                $"native-reference emitter expects '{method.SubjectId}' to have {expectedCount} instructions, but found {instructions.Count}");
        }
    }

    private static void RequireInstructionOp(
        TypedIlInstructionArtifact instruction,
        string expectedOp,
        string subjectId,
        int instructionIndex)
    {
        if (!string.Equals(instruction.Op, expectedOp, StringComparison.Ordinal))
        {
            throw new InvalidOperationException(
                $"native-reference emitter expects instruction {instructionIndex} in '{subjectId}' to be '{expectedOp}', but found '{instruction.Op}'");
        }
    }

    private static void RequireInstructionOpLike(
        TypedIlInstructionArtifact instruction,
        string subjectId,
        int instructionIndex,
        params string[] expectedOps)
    {
        if (expectedOps.Any(expectedOp => string.Equals(instruction.Op, expectedOp, StringComparison.Ordinal)))
        {
            return;
        }

        throw new InvalidOperationException(
            $"native-reference emitter expects instruction {instructionIndex} in '{subjectId}' to be one of [{string.Join(", ", expectedOps)}], but found '{instruction.Op}'");
    }

    private static bool IsCallLikeOp(string? op)
    {
        return string.Equals(op, "call", StringComparison.Ordinal) ||
               string.Equals(op, "callvirt", StringComparison.Ordinal);
    }

    private static void RequireInstructionCallee(
        TypedIlInstructionArtifact instruction,
        string expectedCallee,
        string subjectId,
        int instructionIndex)
    {
        if (!string.Equals(instruction.Callee, expectedCallee, StringComparison.Ordinal))
        {
            throw new InvalidOperationException(
                $"native-reference emitter expects instruction {instructionIndex} in '{subjectId}' to call '{expectedCallee}', but found '{instruction.Callee ?? "<null>"}'");
        }
    }

    private static string GetRequiredInstructionCallee(
        TypedIlInstructionArtifact instruction,
        string subjectId,
        int instructionIndex)
    {
        if (string.IsNullOrWhiteSpace(instruction.Callee))
        {
            throw new InvalidOperationException(
                $"native-reference emitter expects instruction {instructionIndex} in '{subjectId}' to carry a callee");
        }

        return instruction.Callee;
    }

    private static string GetRequiredOperandString(TypedIlInstructionArtifact instruction)
    {
        return instruction.Operand switch
        {
            string value => value,
            JsonElement element when element.ValueKind == JsonValueKind.String => element.GetString()
                ?? throw new InvalidOperationException($"expected string operand for '{instruction.Op}'"),
            _ => throw new InvalidOperationException($"expected string operand for '{instruction.Op}'"),
        };
    }

    private static int GetRequiredOperandInt(TypedIlInstructionArtifact instruction)
    {
        return instruction.Operand switch
        {
            int value => value,
            long value => checked((int)value),
            JsonElement element when element.ValueKind == JsonValueKind.Number => element.GetInt32(),
            _ => throw new InvalidOperationException($"expected integer operand for '{instruction.Op}'"),
        };
    }

    private static string GetDeclaringTypeSubjectId(string subjectId)
    {
        var separatorIndex = subjectId.IndexOf("::", StringComparison.Ordinal);
        if (separatorIndex <= 0)
        {
            throw new InvalidOperationException($"failed to extract declaring type from subject id '{subjectId}'");
        }

        return subjectId[..separatorIndex];
    }

    private static bool IsInterfaceDispatchTarget(
        IReadOnlyList<ManagedTypeModel> types,
        string subjectId)
    {
        var declaringTypeSubjectId = GetDeclaringTypeSubjectId(subjectId);
        return types.Any(type =>
            string.Equals(type.SubjectId, declaringTypeSubjectId, StringComparison.Ordinal) &&
            type.IsInterface);
    }

    private static bool LooksLikeInterfaceMethodSubjectId(string subjectId)
    {
        var declaringTypeSubjectId = GetDeclaringTypeSubjectId(subjectId);
        var typeName = declaringTypeSubjectId[(declaringTypeSubjectId.LastIndexOfAny(['/', '.']) + 1)..];
        return typeName.StartsWith("I", StringComparison.Ordinal) &&
               typeName.Length > 1 &&
               char.IsUpper(typeName[1]);
    }

    private static string GetMethodName(string subjectId)
    {
        var methodSeparatorIndex = subjectId.IndexOf("::", StringComparison.Ordinal);
        var parameterSeparatorIndex = subjectId.IndexOf('(', methodSeparatorIndex + 2);
        if (methodSeparatorIndex <= 0 || parameterSeparatorIndex <= methodSeparatorIndex + 2)
        {
            throw new InvalidOperationException($"failed to extract method name from subject id '{subjectId}'");
        }

        return subjectId[(methodSeparatorIndex + 2)..parameterSeparatorIndex];
    }

    private static MetadataRegistrationEntry GetRequiredMetadataRegistration(
        IReadOnlyList<MetadataRegistrationEntry> registrations,
        string registrationKind,
        string subjectId,
        string description)
    {
        return GetRequiredMetadataRegistration(
            registrations,
            registrationKind,
            registration => string.Equals(registration.SubjectId, subjectId, StringComparison.Ordinal),
            description);
    }

    private static MetadataRegistrationEntry GetRequiredMetadataRegistration(
        IReadOnlyList<MetadataRegistrationEntry> registrations,
        string registrationKind,
        Func<MetadataRegistrationEntry, bool> predicate,
        string description)
    {
        var registration = registrations.FirstOrDefault(item =>
            string.Equals(item.RegistrationKind, registrationKind, StringComparison.Ordinal) &&
            predicate(item));
        if (registration is null)
        {
            throw new InvalidOperationException(
                $"missing required '{registrationKind}' metadata registration for {description}");
        }

        return registration;
    }

    private static string GetRequiredRegistrationName(MetadataRegistrationEntry registration)
    {
        if (!string.IsNullOrWhiteSpace(registration.Name))
        {
            return registration.Name;
        }

        throw new InvalidOperationException(
            $"metadata registration '{registration.SubjectId}' is missing name metadata");
    }

    private static string GetRequiredRegistrationDisplayName(MetadataRegistrationEntry registration)
    {
        if (!string.IsNullOrWhiteSpace(registration.DisplayName))
        {
            return registration.DisplayName;
        }

        if (!string.IsNullOrWhiteSpace(registration.Name))
        {
            return registration.Name;
        }

        throw new InvalidOperationException(
            $"metadata registration '{registration.SubjectId}' is missing displayName metadata");
    }

    private static bool IsCompilerGeneratedMemberName(string? name)
    {
        return string.IsNullOrWhiteSpace(name) || name.StartsWith("<", StringComparison.Ordinal);
    }

    private static bool IsSpecialMethodName(string? name)
    {
        if (string.IsNullOrWhiteSpace(name))
        {
            return true;
        }

        return name.StartsWith(".", StringComparison.Ordinal) ||
               name.StartsWith("<", StringComparison.Ordinal) ||
               name.StartsWith("get_", StringComparison.Ordinal) ||
               name.StartsWith("set_", StringComparison.Ordinal) ||
               name.StartsWith("add_", StringComparison.Ordinal) ||
               name.StartsWith("remove_", StringComparison.Ordinal);
    }

    private static uint GetRequiredMetadataToken(
        MetadataRegistrationArtifact metadataRegistration,
        string registrationKind,
        string subjectId)
    {
        uint rowIndex = 0u;

        foreach (var registration in metadataRegistration.Registrations)
        {
            if (!string.Equals(registration.RegistrationKind, registrationKind, StringComparison.Ordinal))
            {
                continue;
            }

            rowIndex++;
            if (string.Equals(registration.SubjectId, subjectId, StringComparison.Ordinal))
            {
                return GetMetadataTokenPrefix(registrationKind) | rowIndex;
            }
        }

        throw new InvalidOperationException(
            $"missing required '{registrationKind}' metadata registration for '{subjectId}'");
    }

    private static string CreateTypeTokenLiteral(
        MetadataRegistrationArtifact metadataRegistration,
        string subjectId)
    {
        var hasConcreteRegistration = metadataRegistration.Registrations.Any(item =>
            string.Equals(item.RegistrationKind, "type", StringComparison.Ordinal) &&
            string.Equals(item.SubjectId, subjectId, StringComparison.Ordinal));
        return hasConcreteRegistration
            ? FormatCppTokenLiteral(GetRequiredMetadataToken(metadataRegistration, "type", subjectId))
            : FormatCppTokenLiteral(CreatePseudoTypeToken(subjectId));
    }

    private static uint CreatePseudoTypeToken(string subjectId)
    {
        const uint typeTokenPrefix = 0x02000000u;
        uint hash = 2166136261u;

        foreach (var current in subjectId)
        {
            hash ^= current;
            hash *= 16777619u;
        }

        var rowIndex = hash & 0x00FFFFFFu;
        if (rowIndex == 0u)
        {
            rowIndex = 1u;
        }

        return typeTokenPrefix | rowIndex;
    }

    private static uint GetMetadataTokenPrefix(string registrationKind)
    {
        return registrationKind switch
        {
            "type" => 0x02000000u,
            "field" => 0x04000000u,
            "method" => 0x06000000u,
            _ => throw new InvalidOperationException(
                $"native-reference emitter does not know how to derive metadata token for registration kind '{registrationKind}'"),
        };
    }

    private static string FormatCppTokenLiteral(uint token)
    {
        return $"0x{token:X8}u";
    }

    private static string ToCppStringLiteral(string value)
    {
        var builder = new StringBuilder();
        builder.Append('"');

        foreach (var current in value)
        {
            builder.Append(current switch
            {
                '\\' => "\\\\",
                '"' => "\\\"",
                '\r' => "\\r",
                '\n' => "\\n",
                '\t' => "\\t",
                _ => current.ToString(),
            });
        }

        builder.Append('"');
        return builder.ToString();
    }

    private static TypedIlMethodArtifact GetRequiredMethod(
        IReadOnlyList<TypedIlMethodArtifact> methods,
        string subjectId)
    {
        var method = methods.FirstOrDefault(item => string.Equals(item.SubjectId, subjectId, StringComparison.Ordinal));
        if (method is null)
        {
            throw new InvalidOperationException($"missing required typed-il method for '{subjectId}'");
        }

        return method;
    }

    private static CodeRegistrationEntry GetRequiredRegistration(
        IReadOnlyList<CodeRegistrationEntry> registrations,
        string subjectId)
    {
        var registration = registrations.FirstOrDefault(item => string.Equals(item.SubjectId, subjectId, StringComparison.Ordinal));
        if (registration is null)
        {
            throw new InvalidOperationException($"missing required method pointer registration for '{subjectId}'");
        }

        return registration;
    }

    private static T LoadRequiredJson<T>(string path)
    {
        if (!File.Exists(path))
        {
            throw new FileNotFoundException("required Stage 4 input artifact is missing", path);
        }

        var value = JsonSerializer.Deserialize<T>(File.ReadAllText(path), JsonOptions);
        if (value is null)
        {
            throw new InvalidOperationException($"failed to deserialize required JSON artifact: {path}");
        }

        return value;
    }
}


