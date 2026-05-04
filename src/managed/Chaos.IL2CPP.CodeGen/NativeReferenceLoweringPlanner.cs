using System.Text;
using System.Text.Json;
using Chaos.IL2CPP.Contracts;
using Scriban;
using Scriban.Runtime;

namespace Chaos.IL2CPP.CodeGen;

public sealed partial class NativeReferenceLoweringPlanner
{
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
        if (selectedFamily == NativeReferencePlanKind.ManagedAsyncAwaitIntMinimal)
        {
            planned = CreateAsyncAwaitIntLoweringPlan(
                linkedWorld,
                metadataRegistration,
                methodPointers);
        }
        else if (selectedFamily == NativeReferencePlanKind.ManagedThreadingThreadStaticMonitorMinimal)
        {
            planned = CreateThreadingThreadStaticMonitorLoweringPlan(
                linkedWorld,
                metadataRegistration,
                methodPointers);
        }
        else if (selectedFamily == NativeReferencePlanKind.ManagedInterfaceDispatchMessageMinimal)
        {
            planned = CreateInterfaceDispatchMessageLoweringPlan(
                linkedWorld,
                typedIl.Methods,
                metadataRegistration,
                methodPointers);
        }
        else if (selectedFamily == NativeReferencePlanKind.ManagedDispatchVirtualInstanceMessageMinimal)
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

        if (selectedFamily != mappedLegacyFamily)
        {
            throw new InvalidOperationException(
                $"semantic lowering family '{NativeReferenceProofCatalog.Stringify(selectedFamily)}' disagrees with legacy proof-shape lowering family '{NativeReferenceProofCatalog.Stringify(mappedLegacyFamily)}' for '{linkedWorld.EntryPointSubjectId}'");
        }

        return planned with
        {
            PlanKind = NativeReferenceProofCatalog.Stringify(selectedFamily),
        };
    }
}
