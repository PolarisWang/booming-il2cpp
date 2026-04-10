using System.Text;
using System.Text.Json;
using Chaos.IL2CPP.Contracts;
using Scriban;
using Scriban.Runtime;

namespace Chaos.IL2CPP.CodeGen;

public sealed class NativeReferenceProofEmitter
{
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
    private const string ExceptionThrowCatchFinallyMinimal =
        "managed-exceptions.throw-catch-finally-message.minimal";
    private const string ReflectionInteropClosureMinimal =
        "managed-reflection-interop.closure.minimal";
    private const string ReflectionClosedTypeQueryMinimal =
        "reflection.closed-type-query.minimal";
    private const string InteropPInvokeDirectCallMinimal =
        "interop.pinvoke-direct-call.minimal";
    private const string EngineLogWriteMinimal =
        "engine.log-write.minimal";
    private const string EngineObjectHandleRoundtripMinimal =
        "engine.object-handle.roundtrip.minimal";
    private const string EngineLifecycleCallbackMinimal =
        "engine.lifecycle-callback.minimal";
    private const string GeneratedTranslationUnitTemplateRelativePath = "Templates/NativeReferenceProof.cpp.scriban";
    private const string DispatchVirtualInstanceMessageGeneratedTranslationUnitTemplateRelativePath =
        "Templates/NativeReferenceProof.DispatchVirtualInstanceMessage.cpp.scriban";
    private const string StaticCallCtorGetterGeneratedTranslationUnitTemplateRelativePath =
        "Templates/NativeReferenceProof.StaticCallCtorGetter.cpp.scriban";
    private const string ArrayBoxingReferenceArrayGeneratedTranslationUnitTemplateRelativePath =
        "Templates/NativeReferenceProof.ArrayBoxingReferenceArray.cpp.scriban";
    private const string ReflectionQueryMinimalGeneratedTranslationUnitTemplateRelativePath =
        "Templates/NativeReferenceProof.ReflectionQueryMinimal.cpp.scriban";
    private const string PInvokeDllImportMinimalGeneratedTranslationUnitTemplateRelativePath =
        "Templates/NativeReferenceProof.PInvokeDllImportMinimal.cpp.scriban";
    private const string DelegateClosedTargetRelayGeneratedTranslationUnitTemplateRelativePath =
        "Templates/NativeReferenceProof.DelegateClosedTargetRelay.cpp.scriban";
    private const string ExceptionThrowCatchFinallyGeneratedTranslationUnitTemplateRelativePath =
        "Templates/NativeReferenceProof.ExceptionThrowCatchFinally.cpp.scriban";
    private const string ReflectionInteropClosureGeneratedTranslationUnitTemplateRelativePath =
        "Templates/NativeReferenceProof.ReflectionInteropClosure.cpp.scriban";
    private const string EngineLogWriteGeneratedTranslationUnitTemplateRelativePath =
        "Templates/NativeReferenceProof.EngineLogWrite.cpp.scriban";
    private const string EngineObjectHandleGeneratedTranslationUnitTemplateRelativePath =
        "Templates/NativeReferenceProof.EngineObjectHandle.cpp.scriban";
    private const string EngineLifecycleCallbackGeneratedTranslationUnitTemplateRelativePath =
        "Templates/NativeReferenceProof.EngineLifecycleCallback.cpp.scriban";
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

    private static readonly Lazy<Template> StaticCallCtorGetterGeneratedTranslationUnitTemplate =
        new(() => LoadTemplate(StaticCallCtorGetterGeneratedTranslationUnitTemplateRelativePath));

    private static readonly Lazy<Template> ArrayBoxingReferenceArrayGeneratedTranslationUnitTemplate =
        new(() => LoadTemplate(ArrayBoxingReferenceArrayGeneratedTranslationUnitTemplateRelativePath));

    private static readonly Lazy<Template> ReflectionQueryMinimalGeneratedTranslationUnitTemplate =
        new(() => LoadTemplate(ReflectionQueryMinimalGeneratedTranslationUnitTemplateRelativePath));

    private static readonly Lazy<Template> PInvokeDllImportMinimalGeneratedTranslationUnitTemplate =
        new(() => LoadTemplate(PInvokeDllImportMinimalGeneratedTranslationUnitTemplateRelativePath));

    private static readonly Lazy<Template> DelegateClosedTargetRelayGeneratedTranslationUnitTemplate =
        new(() => LoadTemplate(DelegateClosedTargetRelayGeneratedTranslationUnitTemplateRelativePath));

    private static readonly Lazy<Template> ExceptionThrowCatchFinallyGeneratedTranslationUnitTemplate =
        new(() => LoadTemplate(ExceptionThrowCatchFinallyGeneratedTranslationUnitTemplateRelativePath));

    private static readonly Lazy<Template> ReflectionInteropClosureGeneratedTranslationUnitTemplate =
        new(() => LoadTemplate(ReflectionInteropClosureGeneratedTranslationUnitTemplateRelativePath));

    private static readonly Lazy<Template> EngineLogWriteGeneratedTranslationUnitTemplate =
        new(() => LoadTemplate(EngineLogWriteGeneratedTranslationUnitTemplateRelativePath));

    private static readonly Lazy<Template> EngineObjectHandleGeneratedTranslationUnitTemplate =
        new(() => LoadTemplate(EngineObjectHandleGeneratedTranslationUnitTemplateRelativePath));

    private static readonly Lazy<Template> EngineLifecycleCallbackGeneratedTranslationUnitTemplate =
        new(() => LoadTemplate(EngineLifecycleCallbackGeneratedTranslationUnitTemplateRelativePath));

    public NativeReferenceProofResult Generate(NativeReferenceProofRequest request)
    {
        var managedClosureRoot = Path.GetFullPath(request.ManagedClosureRootPath);
        var loweringPlanPath = Path.Combine(managedClosureRoot, ManagedClosureArtifactNames.NativeReferenceLoweringPlan);
        var loweringPlan = LoadRequiredJson<NativeReferenceLoweringPlanArtifact>(loweringPlanPath);
        ValidateManagedLoweringPlan(loweringPlan);
        var translationUnit = BuildGeneratedTranslationUnit(loweringPlan);
        var generatedSource = new NativeReferenceGeneratedSource
        {
            RelativePath = NativeReferenceArtifactNames.GeneratedTranslationUnit,
            Contents = translationUnit,
        };

        var manifest = new NativeReferenceProofManifestArtifact
        {
            AssemblyName = loweringPlan.AssemblyName,
            EntrySubjectId = loweringPlan.EntrySubjectId,
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
            ProofManifest = manifest,
            GeneratedSources = [generatedSource],
        };
    }

    private static string BuildGeneratedTranslationUnit(
        NativeReferenceLoweringPlanArtifact loweringPlan)
    {
        var context = new TemplateContext();
        context.PushGlobal(CreateTemplateModel(loweringPlan));

        try
        {
            return GetTemplateForPlan(loweringPlan.PlanKind).Render(context);
        }
        finally
        {
            context.PopGlobal();
        }
    }

    private static void ValidateManagedLoweringPlan(NativeReferenceLoweringPlanArtifact loweringPlan)
    {
        RequireStringField(loweringPlan.PlanKind, nameof(loweringPlan.PlanKind));
        RequireStringField(loweringPlan.AssemblyName, nameof(loweringPlan.AssemblyName));
        RequireStringField(loweringPlan.EntrySubjectId, nameof(loweringPlan.EntrySubjectId));
        RequireStringField(loweringPlan.IncludeHeader, nameof(loweringPlan.IncludeHeader));
        RequireStringField(loweringPlan.ProofFunctionName, nameof(loweringPlan.ProofFunctionName));
        RequireStringField(loweringPlan.EntrySymbol, nameof(loweringPlan.EntrySymbol));
        RequireStringField(loweringPlan.EntryMethodToken, nameof(loweringPlan.EntryMethodToken));
        RequireStringField(loweringPlan.ConsoleWriteLineStringIcall, nameof(loweringPlan.ConsoleWriteLineStringIcall));

        switch (loweringPlan.PlanKind)
        {
            case ManagedDispatchVirtualInstanceMessageMinimal:
                RequireStringField(loweringPlan.ConstructorSymbol, nameof(loweringPlan.ConstructorSymbol));
                RequireStringField(loweringPlan.InstanceMethodSymbol, nameof(loweringPlan.InstanceMethodSymbol));
                RequireStringField(loweringPlan.ReferenceTypeToken, nameof(loweringPlan.ReferenceTypeToken));
                RequireStringField(loweringPlan.CapturedFieldToken, nameof(loweringPlan.CapturedFieldToken));
                RequireStringField(loweringPlan.ConstructorMethodToken, nameof(loweringPlan.ConstructorMethodToken));
                RequireStringField(loweringPlan.InstanceMethodToken, nameof(loweringPlan.InstanceMethodToken));
                RequireStringField(loweringPlan.DispatchStrategy, nameof(loweringPlan.DispatchStrategy));
                RequireStringField(loweringPlan.StringConcatPairIcall, nameof(loweringPlan.StringConcatPairIcall));
                RequireStringField(loweringPlan.ConstructorLiteral, nameof(loweringPlan.ConstructorLiteral));
                RequireIntField(loweringPlan.ConstructorLiteralByteCount, nameof(loweringPlan.ConstructorLiteralByteCount));
                RequireStringField(loweringPlan.MessagePrefixLiteral, nameof(loweringPlan.MessagePrefixLiteral));
                RequireIntField(loweringPlan.MessagePrefixLiteralByteCount, nameof(loweringPlan.MessagePrefixLiteralByteCount));
                RequireStringField(loweringPlan.MessageSuffixLiteral, nameof(loweringPlan.MessageSuffixLiteral));
                RequireIntField(loweringPlan.MessageSuffixLiteralByteCount, nameof(loweringPlan.MessageSuffixLiteralByteCount));
                return;

            case ManagedObjectCapturedStateInstanceMessageMinimal:
                RequireStringField(loweringPlan.ConstructorSymbol, nameof(loweringPlan.ConstructorSymbol));
                RequireStringField(loweringPlan.InstanceMethodSymbol, nameof(loweringPlan.InstanceMethodSymbol));
                RequireStringField(loweringPlan.ReferenceTypeToken, nameof(loweringPlan.ReferenceTypeToken));
                RequireStringField(loweringPlan.CapturedFieldToken, nameof(loweringPlan.CapturedFieldToken));
                RequireStringField(loweringPlan.ConstructorMethodToken, nameof(loweringPlan.ConstructorMethodToken));
                RequireStringField(loweringPlan.InstanceMethodToken, nameof(loweringPlan.InstanceMethodToken));
                RequireStringField(loweringPlan.StringConcatPairIcall, nameof(loweringPlan.StringConcatPairIcall));
                RequireStringField(loweringPlan.ConstructorLiteral, nameof(loweringPlan.ConstructorLiteral));
                RequireIntField(loweringPlan.ConstructorLiteralByteCount, nameof(loweringPlan.ConstructorLiteralByteCount));
                RequireStringField(loweringPlan.MessagePrefixLiteral, nameof(loweringPlan.MessagePrefixLiteral));
                RequireIntField(loweringPlan.MessagePrefixLiteralByteCount, nameof(loweringPlan.MessagePrefixLiteralByteCount));
                RequireStringField(loweringPlan.MessageSuffixLiteral, nameof(loweringPlan.MessageSuffixLiteral));
                RequireIntField(loweringPlan.MessageSuffixLiteralByteCount, nameof(loweringPlan.MessageSuffixLiteralByteCount));
                return;

            case ManagedGenericStaticForwarderCapturedGetterMinimal:
                RequireStringField(loweringPlan.EchoMethodSymbol, nameof(loweringPlan.EchoMethodSymbol));
                RequireStringField(loweringPlan.ConstructorSymbol, nameof(loweringPlan.ConstructorSymbol));
                RequireStringField(loweringPlan.GetterSymbol, nameof(loweringPlan.GetterSymbol));
                RequireStringField(loweringPlan.ReferenceTypeToken, nameof(loweringPlan.ReferenceTypeToken));
                RequireStringField(loweringPlan.CapturedFieldToken, nameof(loweringPlan.CapturedFieldToken));
                RequireStringField(loweringPlan.EchoMethodToken, nameof(loweringPlan.EchoMethodToken));
                RequireStringField(loweringPlan.ConstructorMethodToken, nameof(loweringPlan.ConstructorMethodToken));
                RequireStringField(loweringPlan.GetterMethodToken, nameof(loweringPlan.GetterMethodToken));
                RequireStringField(loweringPlan.EchoLiteral, nameof(loweringPlan.EchoLiteral));
                RequireIntField(loweringPlan.EchoLiteralByteCount, nameof(loweringPlan.EchoLiteralByteCount));
                return;

            case ManagedArraysBoxingReferenceArrayBoxedIntMinimal:
                RequireStringField(loweringPlan.ConstructorSymbol, nameof(loweringPlan.ConstructorSymbol));
                RequireStringField(loweringPlan.InstanceMethodSymbol, nameof(loweringPlan.InstanceMethodSymbol));
                RequireStringField(loweringPlan.ReferenceTypeToken, nameof(loweringPlan.ReferenceTypeToken));
                RequireStringField(loweringPlan.CapturedFieldToken, nameof(loweringPlan.CapturedFieldToken));
                RequireStringField(loweringPlan.ConstructorMethodToken, nameof(loweringPlan.ConstructorMethodToken));
                RequireStringField(loweringPlan.InstanceMethodToken, nameof(loweringPlan.InstanceMethodToken));
                RequireStringField(loweringPlan.BoxedValueTypeToken, nameof(loweringPlan.BoxedValueTypeToken));
                RequireIntField(loweringPlan.BoxedInt32Value, nameof(loweringPlan.BoxedInt32Value));
                RequireStringField(loweringPlan.StringConcatPairIcall, nameof(loweringPlan.StringConcatPairIcall));
                RequireStringField(loweringPlan.ConstructorLiteral, nameof(loweringPlan.ConstructorLiteral));
                RequireIntField(loweringPlan.ConstructorLiteralByteCount, nameof(loweringPlan.ConstructorLiteralByteCount));
                RequireStringField(loweringPlan.MessagePrefixLiteral, nameof(loweringPlan.MessagePrefixLiteral));
                RequireIntField(loweringPlan.MessagePrefixLiteralByteCount, nameof(loweringPlan.MessagePrefixLiteralByteCount));
                RequireStringField(loweringPlan.MessageSuffixLiteral, nameof(loweringPlan.MessageSuffixLiteral));
                RequireIntField(loweringPlan.MessageSuffixLiteralByteCount, nameof(loweringPlan.MessageSuffixLiteralByteCount));
                return;

            case DelegateClosedTargetRelayMinimal:
                RequireStringField(loweringPlan.ConstructorSymbol, nameof(loweringPlan.ConstructorSymbol));
                RequireStringField(loweringPlan.InstanceMethodSymbol, nameof(loweringPlan.InstanceMethodSymbol));
                RequireStringField(loweringPlan.StaticMethodSymbol, nameof(loweringPlan.StaticMethodSymbol));
                RequireStringField(loweringPlan.ReferenceTypeToken, nameof(loweringPlan.ReferenceTypeToken));
                RequireStringField(loweringPlan.CapturedFieldToken, nameof(loweringPlan.CapturedFieldToken));
                RequireStringField(loweringPlan.ConstructorMethodToken, nameof(loweringPlan.ConstructorMethodToken));
                RequireStringField(loweringPlan.InstanceMethodToken, nameof(loweringPlan.InstanceMethodToken));
                RequireStringField(loweringPlan.StaticMethodToken, nameof(loweringPlan.StaticMethodToken));
                RequireStringField(loweringPlan.StringConcatPairIcall, nameof(loweringPlan.StringConcatPairIcall));
                RequireStringField(loweringPlan.ConstructorLiteral, nameof(loweringPlan.ConstructorLiteral));
                RequireIntField(loweringPlan.ConstructorLiteralByteCount, nameof(loweringPlan.ConstructorLiteralByteCount));
                RequireStringField(loweringPlan.MessagePrefixLiteral, nameof(loweringPlan.MessagePrefixLiteral));
                RequireIntField(loweringPlan.MessagePrefixLiteralByteCount, nameof(loweringPlan.MessagePrefixLiteralByteCount));
                RequireStringField(loweringPlan.MessageSuffixLiteral, nameof(loweringPlan.MessageSuffixLiteral));
                RequireIntField(loweringPlan.MessageSuffixLiteralByteCount, nameof(loweringPlan.MessageSuffixLiteralByteCount));
                return;

            case ExceptionThrowCatchFinallyMinimal:
                RequireStringField(loweringPlan.ConstructorSymbol, nameof(loweringPlan.ConstructorSymbol));
                RequireStringField(loweringPlan.InstanceMethodSymbol, nameof(loweringPlan.InstanceMethodSymbol));
                RequireStringField(loweringPlan.ThrowMethodSymbol, nameof(loweringPlan.ThrowMethodSymbol));
                RequireStringField(loweringPlan.ReferenceTypeToken, nameof(loweringPlan.ReferenceTypeToken));
                RequireStringField(loweringPlan.ConstructorMethodToken, nameof(loweringPlan.ConstructorMethodToken));
                RequireStringField(loweringPlan.InstanceMethodToken, nameof(loweringPlan.InstanceMethodToken));
                RequireStringField(loweringPlan.ThrowMethodToken, nameof(loweringPlan.ThrowMethodToken));
                RequireStringField(loweringPlan.FinallyLiteral, nameof(loweringPlan.FinallyLiteral));
                RequireIntField(loweringPlan.FinallyLiteralByteCount, nameof(loweringPlan.FinallyLiteralByteCount));
                RequireStringField(loweringPlan.ExpectedOutput, nameof(loweringPlan.ExpectedOutput));
                RequireIntField(loweringPlan.ExpectedOutputByteCount, nameof(loweringPlan.ExpectedOutputByteCount));
                return;

            case ReflectionInteropClosureMinimal:
                RequireStringField(loweringPlan.ClosedTypeSubjectId, nameof(loweringPlan.ClosedTypeSubjectId));
                RequireStringField(loweringPlan.GenericTypeDefinitionSubjectId, nameof(loweringPlan.GenericTypeDefinitionSubjectId));
                RequireStringField(loweringPlan.FieldSubjectId, nameof(loweringPlan.FieldSubjectId));
                RequireStringField(loweringPlan.MethodSubjectId, nameof(loweringPlan.MethodSubjectId));
                RequireStringField(loweringPlan.ParameterSubjectId, nameof(loweringPlan.ParameterSubjectId));
                RequireStringField(loweringPlan.ClosedTypeToken, nameof(loweringPlan.ClosedTypeToken));
                RequireStringField(loweringPlan.GenericTypeDefinitionToken, nameof(loweringPlan.GenericTypeDefinitionToken));
                RequireStringField(loweringPlan.ClosedTypeNamespaceName, nameof(loweringPlan.ClosedTypeNamespaceName));
                RequireStringField(loweringPlan.ClosedTypeName, nameof(loweringPlan.ClosedTypeName));
                RequireStringField(loweringPlan.ClosedTypeDisplayName, nameof(loweringPlan.ClosedTypeDisplayName));
                RequireStringField(loweringPlan.FieldQueryName, nameof(loweringPlan.FieldQueryName));
                RequireStringField(loweringPlan.MethodQueryName, nameof(loweringPlan.MethodQueryName));
                RequireIntField(loweringPlan.MethodParameterCount, nameof(loweringPlan.MethodParameterCount));
                RequireIntField(loweringPlan.ParameterIndex, nameof(loweringPlan.ParameterIndex));
                RequireStringField(loweringPlan.ImportModuleName, nameof(loweringPlan.ImportModuleName));
                RequireStringField(loweringPlan.ImportEntryPointName, nameof(loweringPlan.ImportEntryPointName));
                RequireStringField(loweringPlan.ExpectedOutput, nameof(loweringPlan.ExpectedOutput));
                RequireIntField(loweringPlan.ExpectedOutputByteCount, nameof(loweringPlan.ExpectedOutputByteCount));
                return;

            case ReflectionClosedTypeQueryMinimal:
                RequireStringField(loweringPlan.ClosedTypeSubjectId, nameof(loweringPlan.ClosedTypeSubjectId));
                RequireStringField(loweringPlan.GenericTypeDefinitionSubjectId, nameof(loweringPlan.GenericTypeDefinitionSubjectId));
                RequireStringField(loweringPlan.FieldSubjectId, nameof(loweringPlan.FieldSubjectId));
                RequireStringField(loweringPlan.PropertySubjectId, nameof(loweringPlan.PropertySubjectId));
                RequireStringField(loweringPlan.MethodSubjectId, nameof(loweringPlan.MethodSubjectId));
                RequireStringField(loweringPlan.ParameterSubjectId, nameof(loweringPlan.ParameterSubjectId));
                RequireStringField(loweringPlan.ClosedTypeToken, nameof(loweringPlan.ClosedTypeToken));
                RequireStringField(loweringPlan.GenericTypeDefinitionToken, nameof(loweringPlan.GenericTypeDefinitionToken));
                RequireStringField(loweringPlan.ClosedTypeNamespaceName, nameof(loweringPlan.ClosedTypeNamespaceName));
                RequireStringField(loweringPlan.ClosedTypeName, nameof(loweringPlan.ClosedTypeName));
                RequireStringField(loweringPlan.ClosedTypeDisplayName, nameof(loweringPlan.ClosedTypeDisplayName));
                RequireStringField(loweringPlan.GenericTypeDefinitionName, nameof(loweringPlan.GenericTypeDefinitionName));
                RequireStringField(loweringPlan.FieldQueryName, nameof(loweringPlan.FieldQueryName));
                RequireStringField(loweringPlan.PropertyQueryName, nameof(loweringPlan.PropertyQueryName));
                RequireStringField(loweringPlan.MethodQueryName, nameof(loweringPlan.MethodQueryName));
                RequireIntField(loweringPlan.MethodParameterCount, nameof(loweringPlan.MethodParameterCount));
                RequireIntField(loweringPlan.ParameterIndex, nameof(loweringPlan.ParameterIndex));
                RequireStringField(loweringPlan.ExpectedOutput, nameof(loweringPlan.ExpectedOutput));
                RequireIntField(loweringPlan.ExpectedOutputByteCount, nameof(loweringPlan.ExpectedOutputByteCount));
                return;

            case InteropPInvokeDirectCallMinimal:
                RequireStringField(loweringPlan.ImportMethodSymbol, nameof(loweringPlan.ImportMethodSymbol));
                RequireStringField(loweringPlan.ImportModuleName, nameof(loweringPlan.ImportModuleName));
                RequireStringField(loweringPlan.ImportEntryPointName, nameof(loweringPlan.ImportEntryPointName));
                RequireIntField(loweringPlan.ImportArgument0, nameof(loweringPlan.ImportArgument0));
                RequireIntField(loweringPlan.ImportArgument1, nameof(loweringPlan.ImportArgument1));
                RequireIntField(loweringPlan.ImportArgument2, nameof(loweringPlan.ImportArgument2));
                RequireStringField(loweringPlan.OutputPrefix, nameof(loweringPlan.OutputPrefix));
                return;

            case EngineLogWriteMinimal:
            case EngineObjectHandleRoundtripMinimal:
            case EngineLifecycleCallbackMinimal:
                RequireStringField(loweringPlan.ExpectedOutput, nameof(loweringPlan.ExpectedOutput));
                RequireIntField(loweringPlan.ExpectedOutputByteCount, nameof(loweringPlan.ExpectedOutputByteCount));
                if (loweringPlan.EngineBindings is null)
                {
                    throw new InvalidOperationException("managed lowering plan is missing required field 'EngineBindings'");
                }

                if (loweringPlan.HostBindings is null)
                {
                    throw new InvalidOperationException("managed lowering plan is missing required field 'HostBindings'");
                }

                return;

            default:
                throw new InvalidOperationException(
                    $"unsupported managed lowering plan kind '{loweringPlan.PlanKind}'");
        }
    }

    private static void RequireStringField(string? value, string fieldName)
    {
        if (string.IsNullOrWhiteSpace(value))
        {
            throw new InvalidOperationException(
                $"managed lowering plan is missing required field '{fieldName}'");
        }
    }

    private static void RequireIntField<T>(T? value, string fieldName)
        where T : struct
    {
        if (!value.HasValue)
        {
            throw new InvalidOperationException(
                $"managed lowering plan is missing required field '{fieldName}'");
        }
    }

    private static NativeReferenceLoweringPlanArtifact BuildLegacyLoweringPlan(
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

        if (TryBuildLegacyPInvokeDirectCallLoweringPlan(
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

        if (TryBuildLegacyReflectionClosedTypeQueryLoweringPlan(
                assemblyName,
                entryPointSubjectId,
                metadataRegistration,
                entryPointRegistration,
                out var reflectionQueryMinimalPlan))
        {
            return reflectionQueryMinimalPlan;
        }

        if (MatchesLegacyStaticForwarderCtorGetterEntryPointShape(entryPointInstructions))
        {
            return CreateLegacyStaticForwarderCapturedGetterLoweringPlan(
                assemblyName,
                entryPointSubjectId,
                metadataRegistration,
                methodPointers,
                methods,
                entryPointRegistration,
                entryPointMethod,
                entryPointInstructions);
        }

        return CreateLegacyCapturedStateInstanceMessageLoweringPlan(
            assemblyName,
            entryPointSubjectId,
            metadataRegistration,
            methodPointers,
            methods,
            entryPointRegistration,
            entryPointMethod,
            entryPointInstructions);
    }

    private static NativeReferenceLoweringPlanArtifact CreateLegacyCapturedStateInstanceMessageLoweringPlan(
        string assemblyName,
        string entryPointSubjectId,
        MetadataRegistrationArtifact metadataRegistration,
        IReadOnlyList<CodeRegistrationEntry> methodPointers,
        IReadOnlyList<TypedIlMethodArtifact> methods,
        CodeRegistrationEntry entryPointRegistration,
        TypedIlMethodArtifact entryPointMethod,
        IReadOnlyList<TypedIlInstructionArtifact> entryPointInstructions)
    {
        ValidateLegacyCapturedStateInstanceMessageEntryPointShape(entryPointMethod, entryPointInstructions);

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
            ProofFunctionName = "RunNativeReferenceProof",
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

    private static NativeReferenceLoweringPlanArtifact CreateLegacyStaticForwarderCapturedGetterLoweringPlan(
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
            ProofFunctionName = "RunNativeReferenceProof",
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

    private static bool TryBuildLegacyPInvokeDirectCallLoweringPlan(
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
            ProofFunctionName = "RunNativeReferenceProof",
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

    private static bool TryBuildLegacyReflectionClosedTypeQueryLoweringPlan(
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
            ProofFunctionName = "RunNativeReferenceProof",
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
            ManagedDispatchVirtualInstanceMessageMinimal => DispatchVirtualInstanceMessageGeneratedTranslationUnitTemplate.Value,
            ManagedObjectCapturedStateInstanceMessageMinimal => GeneratedTranslationUnitTemplate.Value,
            ManagedGenericStaticForwarderCapturedGetterMinimal => StaticCallCtorGetterGeneratedTranslationUnitTemplate.Value,
            ManagedArraysBoxingReferenceArrayBoxedIntMinimal => ArrayBoxingReferenceArrayGeneratedTranslationUnitTemplate.Value,
            DelegateClosedTargetRelayMinimal => DelegateClosedTargetRelayGeneratedTranslationUnitTemplate.Value,
            ExceptionThrowCatchFinallyMinimal => ExceptionThrowCatchFinallyGeneratedTranslationUnitTemplate.Value,
            ReflectionInteropClosureMinimal => ReflectionInteropClosureGeneratedTranslationUnitTemplate.Value,
            ReflectionClosedTypeQueryMinimal => ReflectionQueryMinimalGeneratedTranslationUnitTemplate.Value,
            InteropPInvokeDirectCallMinimal => PInvokeDllImportMinimalGeneratedTranslationUnitTemplate.Value,
            EngineLogWriteMinimal => EngineLogWriteGeneratedTranslationUnitTemplate.Value,
            EngineObjectHandleRoundtripMinimal => EngineObjectHandleGeneratedTranslationUnitTemplate.Value,
            EngineLifecycleCallbackMinimal => EngineLifecycleCallbackGeneratedTranslationUnitTemplate.Value,
            _ => throw new InvalidOperationException($"unsupported native-reference lowering plan kind '{planKind}'"),
        };
    }

    private static ScriptObject CreateTemplateModel(NativeReferenceLoweringPlanArtifact loweringPlan)
    {
        var model = new ScriptObject
        {
            ["include_header"] = loweringPlan.IncludeHeader,
            ["proof_function_name"] = loweringPlan.ProofFunctionName,
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

    private static bool MatchesLegacyStaticForwarderCtorGetterEntryPointShape(IReadOnlyList<TypedIlInstructionArtifact> instructions)
    {
        return instructions.Count == 7 &&
               string.Equals(instructions[0].Op, "ldstr", StringComparison.Ordinal) &&
               string.Equals(instructions[1].Op, "call", StringComparison.Ordinal) &&
               string.Equals(instructions[2].Op, "newobj", StringComparison.Ordinal) &&
               string.Equals(instructions[3].Op, "call", StringComparison.Ordinal) &&
               string.Equals(instructions[4].Op, "call", StringComparison.Ordinal) &&
               string.Equals(instructions[5].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[6].Op, "ret", StringComparison.Ordinal);
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

    private static void ValidateLegacyCapturedStateInstanceMessageEntryPointShape(
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
        RequireInstructionOp(instructions[3], "call", method.SubjectId, 3);
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
        RequireMethodContract(method, "static-method", "has-canonical-body");
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
        RequireMethodContract(method, "instance-method", "has-canonical-body");
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
