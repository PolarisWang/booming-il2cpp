using System.Text.Json;
using Chaos.IL2CPP.Contracts;

namespace Chaos.IL2CPP.Generator;

public sealed partial class NativeReferenceProofEmitter
{
    private static void ValidateManagedLoweringPlan(NativeReferenceLoweringPlanArtifact loweringPlan)
    {
        RequireStringField(loweringPlan.PlanKind, nameof(loweringPlan.PlanKind));
        RequireStringField(loweringPlan.AssemblyName, nameof(loweringPlan.AssemblyName));
        RequireStringField(loweringPlan.EntrySubjectId, nameof(loweringPlan.EntrySubjectId));
        RequireStringField(loweringPlan.IncludeHeader, nameof(loweringPlan.IncludeHeader));
        RequireStringField(loweringPlan.NativeEntryFunctionName, nameof(loweringPlan.NativeEntryFunctionName));
        RequireStringField(loweringPlan.EntrySymbol, nameof(loweringPlan.EntrySymbol));
        RequireStringField(loweringPlan.EntryMethodToken, nameof(loweringPlan.EntryMethodToken));
        RequireStringField(loweringPlan.ConsoleWriteLineStringIcall, nameof(loweringPlan.ConsoleWriteLineStringIcall));

        var planKind = NativeReferenceProofCatalog.Parse(loweringPlan.PlanKind);
        switch (planKind)
        {
            case NativeReferencePlanKind.ManagedAsyncAwaitIntMinimal:
            case NativeReferencePlanKind.ManagedThreadingThreadStaticMonitorMinimal:
            case NativeReferencePlanKind.MarshalingUtf8ExportMinimal:
                RequireStringField(loweringPlan.ExpectedOutput, nameof(loweringPlan.ExpectedOutput));
                RequireIntField(loweringPlan.ExpectedOutputByteCount, nameof(loweringPlan.ExpectedOutputByteCount));
                return;

            case NativeReferencePlanKind.ManagedInterfaceDispatchMessageMinimal:
            case NativeReferencePlanKind.ManagedDispatchVirtualInstanceMessageMinimal:
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

            case NativeReferencePlanKind.ManagedObjectCapturedStateInstanceMessageMinimal:
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

            case NativeReferencePlanKind.ManagedArraysCopyReferenceArrayMinimal:
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
                RequireIntField(loweringPlan.SourceArrayLength, nameof(loweringPlan.SourceArrayLength));
                RequireIntField(loweringPlan.TargetArrayLength, nameof(loweringPlan.TargetArrayLength));
                RequireIntField(loweringPlan.SourceStoreIndex, nameof(loweringPlan.SourceStoreIndex));
                RequireIntField(loweringPlan.SourceArrayIndex, nameof(loweringPlan.SourceArrayIndex));
                RequireIntField(loweringPlan.TargetArrayIndex, nameof(loweringPlan.TargetArrayIndex));
                RequireIntField(loweringPlan.TargetReadIndex, nameof(loweringPlan.TargetReadIndex));
                RequireIntField(loweringPlan.CopyLength, nameof(loweringPlan.CopyLength));
                return;

            case NativeReferencePlanKind.ManagedArraysReverseReferenceArrayMinimal:
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
                RequireIntField(loweringPlan.ArrayLength, nameof(loweringPlan.ArrayLength));
                RequireIntField(loweringPlan.ArrayStoreIndex, nameof(loweringPlan.ArrayStoreIndex));
                RequireIntField(loweringPlan.ReverseStartIndex, nameof(loweringPlan.ReverseStartIndex));
                RequireIntField(loweringPlan.ReverseLength, nameof(loweringPlan.ReverseLength));
                RequireIntField(loweringPlan.ArrayReadIndex, nameof(loweringPlan.ArrayReadIndex));
                return;

            case NativeReferencePlanKind.ManagedArraysClearReferenceArrayMinimal:
                RequireStringField(loweringPlan.ReferenceTypeToken, nameof(loweringPlan.ReferenceTypeToken));
                RequireStringField(loweringPlan.StringConcatPairIcall, nameof(loweringPlan.StringConcatPairIcall));
                RequireStringField(loweringPlan.StoredLiteral, nameof(loweringPlan.StoredLiteral));
                RequireIntField(loweringPlan.StoredLiteralByteCount, nameof(loweringPlan.StoredLiteralByteCount));
                RequireStringField(loweringPlan.MessagePrefixLiteral, nameof(loweringPlan.MessagePrefixLiteral));
                RequireIntField(loweringPlan.MessagePrefixLiteralByteCount, nameof(loweringPlan.MessagePrefixLiteralByteCount));
                RequireIntField(loweringPlan.ArrayLength, nameof(loweringPlan.ArrayLength));
                RequireIntField(loweringPlan.ArrayStoreIndex, nameof(loweringPlan.ArrayStoreIndex));
                RequireIntField(loweringPlan.ClearStartIndex, nameof(loweringPlan.ClearStartIndex));
                RequireIntField(loweringPlan.ClearLength, nameof(loweringPlan.ClearLength));
                RequireIntField(loweringPlan.ArrayReadIndex, nameof(loweringPlan.ArrayReadIndex));
                return;

            case NativeReferencePlanKind.ManagedGenericStaticForwarderCapturedGetterMinimal:
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

            case NativeReferencePlanKind.ManagedArraysBoxingReferenceArrayBoxedIntMinimal:
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

            case NativeReferencePlanKind.DelegateClosedTargetRelayMinimal:
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
                RequireStringField(loweringPlan.TrailingLiteral, nameof(loweringPlan.TrailingLiteral));
                RequireIntField(loweringPlan.TrailingLiteralByteCount, nameof(loweringPlan.TrailingLiteralByteCount));
                return;

            case NativeReferencePlanKind.NestedExceptionThrowCatchFinallyMinimal:
            case NativeReferencePlanKind.ExceptionThrowCatchFinallyMinimal:
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

            case NativeReferencePlanKind.ReflectionInteropClosureMinimal:
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

            case NativeReferencePlanKind.ReflectionClosedTypeQueryMinimal:
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

            case NativeReferencePlanKind.InteropPInvokeDirectCallMinimal:
                RequireStringField(loweringPlan.ImportMethodSymbol, nameof(loweringPlan.ImportMethodSymbol));
                RequireStringField(loweringPlan.ImportModuleName, nameof(loweringPlan.ImportModuleName));
                RequireStringField(loweringPlan.ImportEntryPointName, nameof(loweringPlan.ImportEntryPointName));
                RequireIntField(loweringPlan.ImportArgument0, nameof(loweringPlan.ImportArgument0));
                RequireIntField(loweringPlan.ImportArgument1, nameof(loweringPlan.ImportArgument1));
                RequireIntField(loweringPlan.ImportArgument2, nameof(loweringPlan.ImportArgument2));
                RequireStringField(loweringPlan.OutputPrefix, nameof(loweringPlan.OutputPrefix));
                return;

            case NativeReferencePlanKind.EngineLogWriteMinimal:
            case NativeReferencePlanKind.EngineObjectHandleRoundtripMinimal:
            case NativeReferencePlanKind.EngineLifecycleCallbackMinimal:
            case NativeReferencePlanKind.EngineHostProofMinimal:
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

    private static void ValidateAssemblyFullClosureAuditPlan(NativeReferenceLoweringPlanArtifact loweringPlan)
    {
        AssemblyFullClosureAuditEmitter.ValidatePlan(
            loweringPlan.PlanKind,
            loweringPlan.AssemblyName,
            loweringPlan.TranslationUnitMode,
            loweringPlan.TranslationUnitMethodSubjectIds,
            loweringPlan.TranslationUnitPageSize,
            loweringPlan.TranslationUnitPageCount,
            loweringPlan.TranslationUnitPages,
            AuditTranslationUnitPageSize);
    }

    private static void ValidateAssemblyFullClosureRuntimeSkeletonPlan(NativeReferenceLoweringPlanArtifact loweringPlan)
    {
        RequireStringField(loweringPlan.PlanKind, nameof(loweringPlan.PlanKind));
        RequireStringField(loweringPlan.AssemblyName, nameof(loweringPlan.AssemblyName));
        RequireStringField(loweringPlan.NativeEntryFunctionName, nameof(loweringPlan.NativeEntryFunctionName));

        if (!string.Equals(loweringPlan.PlanKind, "assembly-full-closure-runtime-skeleton", StringComparison.Ordinal))
        {
            throw new InvalidOperationException(
                $"assembly-full-closure runtime skeleton emitter expected plan kind 'assembly-full-closure-runtime-skeleton', but found '{loweringPlan.PlanKind}'");
        }

        if (!string.Equals(loweringPlan.TranslationUnitMode, "runtime-skeleton", StringComparison.Ordinal))
        {
            throw new InvalidOperationException(
                $"assembly-full-closure runtime skeleton emitter expected translation-unit mode 'runtime-skeleton', but found '{loweringPlan.TranslationUnitMode ?? "<null>"}'");
        }

        if (!string.Equals(loweringPlan.AuditStatus, "runtime-skeleton", StringComparison.Ordinal))
        {
            throw new InvalidOperationException(
                $"assembly-full-closure runtime skeleton emitter expected audit status 'runtime-skeleton', but found '{loweringPlan.AuditStatus ?? "<null>"}'");
        }

        if (loweringPlan.TranslationUnitMethodSubjectIds is null ||
            loweringPlan.TranslationUnitMethodSubjectIds.Count == 0)
        {
            throw new InvalidOperationException(
                "assembly-full-closure runtime skeleton emitter requires at least one translation-unit method subject id");
        }

        if (loweringPlan.TranslationUnitPageSize != AuditTranslationUnitPageSize)
        {
            throw new InvalidOperationException(
                $"assembly-full-closure runtime skeleton emitter expected translation-unit page size '{AuditTranslationUnitPageSize}', but found '{loweringPlan.TranslationUnitPageSize?.ToString() ?? "<null>"}'");
        }

        if (loweringPlan.TranslationUnitPageCount != GetAuditPageCount(loweringPlan.TranslationUnitMethodSubjectIds.Count))
        {
            throw new InvalidOperationException(
                $"assembly-full-closure runtime skeleton emitter expected translation-unit page count '{GetAuditPageCount(loweringPlan.TranslationUnitMethodSubjectIds.Count)}', but found '{loweringPlan.TranslationUnitPageCount?.ToString() ?? "<null>"}'");
        }

        if (loweringPlan.TranslationUnitPages is null ||
            loweringPlan.TranslationUnitPages.Count != loweringPlan.TranslationUnitPageCount)
        {
            throw new InvalidOperationException(
                "assembly-full-closure runtime skeleton emitter requires translation-unit page metadata matching the declared page count");
        }
    }

    private static int GetAuditPageCount(int totalMethodCount)
    {
        if (totalMethodCount <= 0)
        {
            return 0;
        }

        return (totalMethodCount + AuditTranslationUnitPageSize - 1) / AuditTranslationUnitPageSize;
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
    private static void ValidateLegacyCapturedStateInstanceMessageEntryPointShape(
        TypedIlMethodArtifact method,
        IReadOnlyList<TypedIlInstructionArtifact> instructions)
    {
        RequireMethodContract(method, "static-method", "has-canonical-body");
        RequireInstructionCount(method, instructions, 6);
        RequireInstructionOp(instructions[0], "ldstr", method.SubjectId, 0);
        RequireInstructionOp(instructions[1], "newobj", method.SubjectId, 1);
        RequireInstructionOp(instructions[2], "call", "callvirt", method.SubjectId, 2);
        RequireInstructionOp(instructions[3], "call", method.SubjectId, 3);
        RequireInstructionOp(instructions[4], "ldc.i4", method.SubjectId, 4);
        RequireInstructionOp(instructions[5], "ret", method.SubjectId, 5);
        RequireInstructionCallee(instructions[3], ConsoleWriteLineStringIcall, ConsoleWriteLineStringMethodSubjectId, method.SubjectId, 3);

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
        RequireInstructionCount(method, instructions, 7);
        RequireInstructionOp(instructions[0], "ldstr", method.SubjectId, 0);
        RequireInstructionOp(instructions[1], "call", method.SubjectId, 1);
        RequireInstructionOp(instructions[2], "newobj", method.SubjectId, 2);
        RequireInstructionOp(instructions[3], "call", method.SubjectId, 3);
        RequireInstructionOp(instructions[4], "call", method.SubjectId, 4);
        RequireInstructionOp(instructions[5], "ldc.i4", method.SubjectId, 5);
        RequireInstructionOp(instructions[6], "ret", method.SubjectId, 6);
        RequireInstructionCallee(instructions[4], ConsoleWriteLineStringIcall, ConsoleWriteLineStringMethodSubjectId, method.SubjectId, 4);

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
        if (instructions.Count == 7)
        {
            if (string.Equals(instructions[0].Op, "ldstr", StringComparison.Ordinal))
            {
                RequireInstructionOp(instructions[1], "ldarg", method.SubjectId, 1);
                if (GetRequiredOperandInt(instructions[1]) != 0)
                {
                    throw new InvalidOperationException(
                        $"native-reference emitter expects '{method.SubjectId}' literal-prefix shape to load ldarg 0 after the prefix literal");
                }
            }
            else
            {
                RequireInstructionOp(instructions[0], "ldarg", method.SubjectId, 0);
                RequireInstructionOp(instructions[1], "ldarg", method.SubjectId, 1);
                if (GetRequiredOperandInt(instructions[0]) != 1 ||
                    GetRequiredOperandInt(instructions[1]) != 0)
                {
                    throw new InvalidOperationException(
                        $"native-reference emitter expects '{method.SubjectId}' parameter-prefix shape to use ldarg 1 / ldarg 0 before the captured field load");
                }
            }

            RequireInstructionOp(instructions[2], "ldfld", method.SubjectId, 2);
            RequireInstructionOp(instructions[3], "call", method.SubjectId, 3);
            RequireInstructionOp(instructions[4], "ldstr", method.SubjectId, 4);
            RequireInstructionOp(instructions[5], "call", method.SubjectId, 5);
            RequireInstructionOp(instructions[6], "ret", method.SubjectId, 6);

            RequireInstructionCallee(
                instructions[3],
                StringConcatPairIcall,
                StringConcatPairMethodSubjectId,
                method.SubjectId,
                3);
            RequireInstructionCallee(
                instructions[5],
                StringConcatPairIcall,
                StringConcatPairMethodSubjectId,
                method.SubjectId,
                5);
            return;
        }

        if (instructions.Count == 6)
        {
            RequireInstructionOp(instructions[0], "ldstr", method.SubjectId, 0);
            RequireInstructionOp(instructions[1], "ldarg", method.SubjectId, 1);
            RequireInstructionOp(instructions[2], "ldfld", method.SubjectId, 2);
            RequireInstructionOp(instructions[3], "ldstr", method.SubjectId, 3);
            RequireInstructionOp(instructions[4], "call", method.SubjectId, 4);
            RequireInstructionOp(instructions[5], "ret", method.SubjectId, 5);

            if (GetRequiredOperandInt(instructions[1]) != 0)
            {
                throw new InvalidOperationException(
                    $"native-reference emitter expects '{method.SubjectId}' to start with ldarg 0");
            }

            RequireInstructionCallee(
                instructions[4],
                StringConcatTripleIcall,
                "System.Private.CoreLib/System.String::Concat:System.String(System.String,System.String,System.String)",
                method.SubjectId,
                4);
            return;
        }

        throw new InvalidOperationException(
            $"native-reference emitter expects '{method.SubjectId}' field-backed string instance method to use 6 or 7 canonical instructions");
    }

    private static string GetCapturedStateInstanceMessageSuffixLiteral(
        TypedIlMethodArtifact method,
        IReadOnlyList<TypedIlInstructionArtifact> instructions)
    {
        return instructions.Count switch
        {
            7 => GetRequiredOperandString(instructions[4]),
            6 => GetRequiredOperandString(instructions[3]),
            _ => throw new InvalidOperationException(
                $"native-reference emitter cannot extract suffix literal from '{method.SubjectId}'"),
        };
    }

    private static string GetCapturedStateInstanceMessageConcatIcall(
        TypedIlMethodArtifact method,
        IReadOnlyList<TypedIlInstructionArtifact> instructions)
    {
        return instructions.Count switch
        {
            7 => GetRequiredInstructionCallee(instructions[3], method.SubjectId, 3),
            6 => GetRequiredInstructionCallee(instructions[4], method.SubjectId, 4),
            _ => throw new InvalidOperationException(
                $"native-reference emitter cannot extract concat icall from '{method.SubjectId}'"),
        };
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
        return TryResolveConcreteTypeCallTarget(methods, constructorTypeSubjectId, declaredTargetSubjectId);
    }

    private static string? TryResolveBoxedInterfaceCallTarget(
        IReadOnlyList<TypedIlMethodArtifact> methods,
        IReadOnlyList<TypedIlInstructionArtifact> instructions,
        int instructionIndex,
        string declaredTargetSubjectId)
    {
        if (instructionIndex < 0 || instructionIndex >= instructions.Count)
        {
            return null;
        }

        var boxedInstruction = instructions[instructionIndex];
        if (!string.Equals(boxedInstruction.Op, "box", StringComparison.Ordinal))
        {
            return null;
        }

        var boxedTypeSubjectId = GetRequiredOperandString(boxedInstruction);
        return TryResolveConcreteTypeCallTarget(methods, boxedTypeSubjectId, declaredTargetSubjectId);
    }

    private static string? TryResolveConcreteTypeCallTarget(
        IReadOnlyList<TypedIlMethodArtifact> methods,
        string concreteTypeSubjectId,
        string declaredTargetSubjectId)
    {
        var declaredMethod = methods.FirstOrDefault(candidate =>
            string.Equals(candidate.SubjectId, declaredTargetSubjectId, StringComparison.Ordinal));
        var declaredMethodName = declaredMethod is null
            ? GetMethodName(declaredTargetSubjectId)
            : GetMethodName(declaredMethod.SubjectId);
        var declaredParameterTypes = declaredMethod is null
            ? GetMethodParameterTypesFromSubjectId(declaredTargetSubjectId)
            : declaredMethod.Parameters.Select(parameter => parameter.Type).ToArray();
        var exactMatch = methods.FirstOrDefault(candidate =>
            string.Equals(GetDeclaringTypeSubjectId(candidate.SubjectId), concreteTypeSubjectId, StringComparison.Ordinal) &&
            MethodNameMatchesConcreteTarget(GetMethodName(candidate.SubjectId), declaredMethodName) &&
            string.Equals(candidate.BodyAvailability, "has-canonical-body", StringComparison.Ordinal) &&
            candidate.Parameters.Select(parameter => parameter.Type).SequenceEqual(
                declaredParameterTypes,
                StringComparer.Ordinal))
            ?.SubjectId;
        if (!string.IsNullOrWhiteSpace(exactMatch))
        {
            return exactMatch;
        }

        return methods.FirstOrDefault(candidate =>
            string.Equals(GetDeclaringTypeSubjectId(candidate.SubjectId), concreteTypeSubjectId, StringComparison.Ordinal) &&
            MethodNameMatchesConcreteTarget(GetMethodName(candidate.SubjectId), declaredMethodName) &&
            string.Equals(candidate.BodyAvailability, "has-canonical-body", StringComparison.Ordinal))
            ?.SubjectId;
    }

    private static bool MethodNameMatchesConcreteTarget(
        string candidateMethodName,
        string declaredMethodName)
    {
        return string.Equals(candidateMethodName, declaredMethodName, StringComparison.Ordinal) ||
               candidateMethodName.EndsWith($".{declaredMethodName}", StringComparison.Ordinal);
    }

    // P3: cache for SubjectId -> parameter types
    private static readonly Dictionary<string, IReadOnlyList<string>> s_refProofParamTypeCache
        = new(StringComparer.Ordinal);
}
