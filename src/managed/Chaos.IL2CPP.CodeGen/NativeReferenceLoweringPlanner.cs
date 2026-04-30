using System.Text;
using System.Text.Json;
using Chaos.IL2CPP.Contracts;
using Scriban;
using Scriban.Runtime;

namespace Chaos.IL2CPP.CodeGen;

public sealed partial class NativeReferenceLoweringPlanner
{
    private const string ManagedAsyncAwaitIntMinimal = NativeReferenceProofCatalog.ManagedAsyncAwaitIntMinimal;
    private const string ManagedThreadingThreadStaticMonitorMinimal = NativeReferenceProofCatalog.ManagedThreadingThreadStaticMonitorMinimal;
    private const string ManagedInterfaceDispatchMessageMinimal = NativeReferenceProofCatalog.ManagedInterfaceDispatchMessageMinimal;
    private const string ManagedDispatchVirtualInstanceMessageMinimal = NativeReferenceProofCatalog.ManagedDispatchVirtualInstanceMessageMinimal;
    private const string ManagedObjectCapturedStateInstanceMessageMinimal = NativeReferenceProofCatalog.ManagedObjectCapturedStateInstanceMessageMinimal;
    private const string ManagedGenericStaticForwarderCapturedGetterMinimal = NativeReferenceProofCatalog.ManagedGenericStaticForwarderCapturedGetterMinimal;
    private const string ManagedArraysReverseReferenceArrayMinimal = NativeReferenceProofCatalog.ManagedArraysReverseReferenceArrayMinimal;
    private const string ManagedArraysClearReferenceArrayMinimal = NativeReferenceProofCatalog.ManagedArraysClearReferenceArrayMinimal;
    private const string ManagedArraysCopyReferenceArrayMinimal = NativeReferenceProofCatalog.ManagedArraysCopyReferenceArrayMinimal;
    private const string ManagedArraysBoxingReferenceArrayBoxedIntMinimal = NativeReferenceProofCatalog.ManagedArraysBoxingReferenceArrayBoxedIntMinimal;
    private const string DelegateClosedTargetRelayMinimal = NativeReferenceProofCatalog.DelegateClosedTargetRelayMinimal;
    private const string NestedExceptionThrowCatchFinallyMinimal = NativeReferenceProofCatalog.NestedExceptionThrowCatchFinallyMinimal;
    private const string ExceptionThrowCatchFinallyMinimal = NativeReferenceProofCatalog.ExceptionThrowCatchFinallyMinimal;
    private const string ReflectionInteropClosureMinimal = NativeReferenceProofCatalog.ReflectionInteropClosureMinimal;
    private const string ReflectionClosedTypeQueryMinimal = NativeReferenceProofCatalog.ReflectionClosedTypeQueryMinimal;
    private const string MarshalingUtf8ExportMinimal = NativeReferenceProofCatalog.MarshalingUtf8ExportMinimal;
    private const string InteropPInvokeDirectCallMinimal = NativeReferenceProofCatalog.InteropPInvokeDirectCallMinimal;
    private const string EngineLogWriteMinimal = NativeReferenceProofCatalog.EngineLogWriteMinimal;
    private const string EngineObjectHandleRoundtripMinimal = NativeReferenceProofCatalog.EngineObjectHandleRoundtripMinimal;
    private const string EngineLifecycleCallbackMinimal = NativeReferenceProofCatalog.EngineLifecycleCallbackMinimal;
    private const string EngineHostProofMinimal = NativeReferenceProofCatalog.EngineHostProofMinimal;
    private const string GeneratedTranslationUnitTemplateRelativePath = NativeReferenceProofCatalog.GeneratedTranslationUnitTemplateRelativePath;
    private const string DispatchVirtualInstanceMessageGeneratedTranslationUnitTemplateRelativePath = NativeReferenceProofCatalog.DispatchVirtualInstanceMessageGeneratedTranslationUnitTemplateRelativePath;
    private const string InterfaceDispatchMessageGeneratedTranslationUnitTemplateRelativePath = NativeReferenceProofCatalog.InterfaceDispatchMessageGeneratedTranslationUnitTemplateRelativePath;
    private const string AsyncAwaitIntGeneratedTranslationUnitTemplateRelativePath = NativeReferenceProofCatalog.AsyncAwaitIntGeneratedTranslationUnitTemplateRelativePath;
    private const string ThreadingThreadStaticMonitorGeneratedTranslationUnitTemplateRelativePath = NativeReferenceProofCatalog.ThreadingThreadStaticMonitorGeneratedTranslationUnitTemplateRelativePath;
    private const string StaticCallCtorGetterGeneratedTranslationUnitTemplateRelativePath = NativeReferenceProofCatalog.StaticCallCtorGetterGeneratedTranslationUnitTemplateRelativePath;
    private const string ArrayBoxingReferenceArrayGeneratedTranslationUnitTemplateRelativePath = NativeReferenceProofCatalog.ArrayBoxingReferenceArrayGeneratedTranslationUnitTemplateRelativePath;
    private const string ReflectionQueryMinimalGeneratedTranslationUnitTemplateRelativePath = NativeReferenceProofCatalog.ReflectionQueryMinimalGeneratedTranslationUnitTemplateRelativePath;
    private const string MarshalingUtf8ExportGeneratedTranslationUnitTemplateRelativePath = NativeReferenceProofCatalog.MarshalingUtf8ExportGeneratedTranslationUnitTemplateRelativePath;
    private const string PInvokeDllImportMinimalGeneratedTranslationUnitTemplateRelativePath = NativeReferenceProofCatalog.PInvokeDllImportMinimalGeneratedTranslationUnitTemplateRelativePath;
    private const string DelegateClosedTargetRelayGeneratedTranslationUnitTemplateRelativePath = NativeReferenceProofCatalog.DelegateClosedTargetRelayGeneratedTranslationUnitTemplateRelativePath;
    private const string NestedExceptionThrowCatchFinallyGeneratedTranslationUnitTemplateRelativePath = NativeReferenceProofCatalog.NestedExceptionThrowCatchFinallyGeneratedTranslationUnitTemplateRelativePath;
    private const string ExceptionThrowCatchFinallyGeneratedTranslationUnitTemplateRelativePath = NativeReferenceProofCatalog.ExceptionThrowCatchFinallyGeneratedTranslationUnitTemplateRelativePath;
    private const string ReflectionInteropClosureGeneratedTranslationUnitTemplateRelativePath = NativeReferenceProofCatalog.ReflectionInteropClosureGeneratedTranslationUnitTemplateRelativePath;
    private const string ConsoleWriteLineStringIcall = "System.Console/System.Console::WriteLine(System.String)";
    private const string ConsoleWriteLineStringMethodSubjectId = "System.Console/System.Console::WriteLine:System.Void(System.String)";
    private const string StringConcatPairIcall = "System.Private.CoreLib/System.String::Concat(System.String,System.String)";
    private const string StringConcatTripleIcall = "System.Private.CoreLib/System.String::Concat(System.String,System.String,System.String)";
    private const string StringConcatPairMethodSubjectId = "System.Private.CoreLib/System.String::Concat:System.String(System.String,System.String)";
    private const string StringConcatTripleMethodSubjectId = "System.Private.CoreLib/System.String::Concat:System.String(System.String,System.String,System.String)";
    private static readonly JsonSerializerOptions JsonOptions = new()
    {
        PropertyNameCaseInsensitive = true,
    };

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
            return enginePlan!;
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
}
