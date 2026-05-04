using System.Text.Json;
using Chaos.IL2CPP.Contracts;

namespace Chaos.IL2CPP.CodeGen;

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

    private static bool IsArrayCopyReferenceArrayEntryPointShape(IReadOnlyList<TypedIlInstructionArtifact> instructions)
    {
        return IsArrayCopyReferenceArrayLengthOnlyEntryPointShape(instructions) ||
               IsArrayCopyToReferenceArrayTargetOffsetEntryPointShape(instructions) ||
               IsArrayCopyReferenceArrayIndexedEntryPointShape(instructions);
    }

    private static bool IsArrayReverseReferenceArrayEntryPointShape(IReadOnlyList<TypedIlInstructionArtifact> instructions)
    {
        return IsArrayReverseRangeReferenceArrayEntryPointShape(instructions) ||
               IsArrayReverseWholeReferenceArrayEntryPointShape(instructions);
    }

    private static bool IsArrayReverseRangeReferenceArrayEntryPointShape(IReadOnlyList<TypedIlInstructionArtifact> instructions)
    {
        return instructions.Count == 17 &&
               string.Equals(instructions[0].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[1].Op, "newarr", StringComparison.Ordinal) &&
               string.Equals(instructions[2].Op, "dup", StringComparison.Ordinal) &&
               string.Equals(instructions[3].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[4].Op, "ldstr", StringComparison.Ordinal) &&
               string.Equals(instructions[5].Op, "newobj", StringComparison.Ordinal) &&
               string.Equals(instructions[6].Op, "stelem.ref", StringComparison.Ordinal) &&
               string.Equals(instructions[7].Op, "dup", StringComparison.Ordinal) &&
               string.Equals(instructions[8].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[9].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[10].Op, "call", StringComparison.Ordinal) &&
               string.Equals(instructions[11].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[12].Op, "ldelem.ref", StringComparison.Ordinal) &&
               string.Equals(instructions[13].Op, "callvirt", StringComparison.Ordinal) &&
               string.Equals(instructions[14].Op, "call", StringComparison.Ordinal) &&
               string.Equals(instructions[15].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[16].Op, "ret", StringComparison.Ordinal);
    }

    private static bool IsArrayReverseWholeReferenceArrayEntryPointShape(IReadOnlyList<TypedIlInstructionArtifact> instructions)
    {
        return instructions.Count == 15 &&
               string.Equals(instructions[0].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[1].Op, "newarr", StringComparison.Ordinal) &&
               string.Equals(instructions[2].Op, "dup", StringComparison.Ordinal) &&
               string.Equals(instructions[3].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[4].Op, "ldstr", StringComparison.Ordinal) &&
               string.Equals(instructions[5].Op, "newobj", StringComparison.Ordinal) &&
               string.Equals(instructions[6].Op, "stelem.ref", StringComparison.Ordinal) &&
               string.Equals(instructions[7].Op, "dup", StringComparison.Ordinal) &&
               string.Equals(instructions[8].Op, "call", StringComparison.Ordinal) &&
               string.Equals(instructions[9].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[10].Op, "ldelem.ref", StringComparison.Ordinal) &&
               string.Equals(instructions[11].Op, "callvirt", StringComparison.Ordinal) &&
               string.Equals(instructions[12].Op, "call", StringComparison.Ordinal) &&
               string.Equals(instructions[13].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[14].Op, "ret", StringComparison.Ordinal);
    }

    private static bool IsArrayClearReferenceArrayEntryPointShape(IReadOnlyList<TypedIlInstructionArtifact> instructions)
    {
        return IsArrayClearRangeReferenceArrayEntryPointShape(instructions) ||
               IsArrayClearWholeReferenceArrayEntryPointShape(instructions);
    }

    private static bool IsArrayClearRangeReferenceArrayEntryPointShape(IReadOnlyList<TypedIlInstructionArtifact> instructions)
    {
        return instructions.Count == 19 &&
               string.Equals(instructions[0].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[1].Op, "newarr", StringComparison.Ordinal) &&
               string.Equals(instructions[2].Op, "stloc", StringComparison.Ordinal) &&
               string.Equals(instructions[3].Op, "ldloc", StringComparison.Ordinal) &&
               string.Equals(instructions[4].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[5].Op, "ldstr", StringComparison.Ordinal) &&
               string.Equals(instructions[6].Op, "stelem.ref", StringComparison.Ordinal) &&
               string.Equals(instructions[7].Op, "ldloc", StringComparison.Ordinal) &&
               string.Equals(instructions[8].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[9].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[10].Op, "call", StringComparison.Ordinal) &&
               string.Equals(instructions[11].Op, "ldstr", StringComparison.Ordinal) &&
               string.Equals(instructions[12].Op, "ldloc", StringComparison.Ordinal) &&
               string.Equals(instructions[13].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[14].Op, "ldelem.ref", StringComparison.Ordinal) &&
               string.Equals(instructions[15].Op, "call", StringComparison.Ordinal) &&
               string.Equals(instructions[16].Op, "call", StringComparison.Ordinal) &&
               string.Equals(instructions[17].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[18].Op, "ret", StringComparison.Ordinal);
    }

    private static bool IsArrayClearWholeReferenceArrayEntryPointShape(IReadOnlyList<TypedIlInstructionArtifact> instructions)
    {
        return instructions.Count == 17 &&
               string.Equals(instructions[0].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[1].Op, "newarr", StringComparison.Ordinal) &&
               string.Equals(instructions[2].Op, "stloc", StringComparison.Ordinal) &&
               string.Equals(instructions[3].Op, "ldloc", StringComparison.Ordinal) &&
               string.Equals(instructions[4].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[5].Op, "ldstr", StringComparison.Ordinal) &&
               string.Equals(instructions[6].Op, "stelem.ref", StringComparison.Ordinal) &&
               string.Equals(instructions[7].Op, "ldloc", StringComparison.Ordinal) &&
               string.Equals(instructions[8].Op, "call", StringComparison.Ordinal) &&
               string.Equals(instructions[9].Op, "ldstr", StringComparison.Ordinal) &&
               string.Equals(instructions[10].Op, "ldloc", StringComparison.Ordinal) &&
               string.Equals(instructions[11].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[12].Op, "ldelem.ref", StringComparison.Ordinal) &&
               string.Equals(instructions[13].Op, "call", StringComparison.Ordinal) &&
               string.Equals(instructions[14].Op, "call", StringComparison.Ordinal) &&
               string.Equals(instructions[15].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[16].Op, "ret", StringComparison.Ordinal);
    }

    private static bool IsArrayCopyReferenceArrayLengthOnlyEntryPointShape(IReadOnlyList<TypedIlInstructionArtifact> instructions)
    {
        return IsArrayCopyReferenceArrayLengthOnlyInt32EntryPointShape(instructions) ||
               IsArrayCopyReferenceArrayLengthOnlyInt64EntryPointShape(instructions);
    }

    private static bool IsArrayCopyReferenceArrayLengthOnlyInt32EntryPointShape(IReadOnlyList<TypedIlInstructionArtifact> instructions)
    {
        return instructions.Count == 20 &&
               string.Equals(instructions[0].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[1].Op, "newarr", StringComparison.Ordinal) &&
               string.Equals(instructions[2].Op, "dup", StringComparison.Ordinal) &&
               string.Equals(instructions[3].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[4].Op, "ldstr", StringComparison.Ordinal) &&
               string.Equals(instructions[5].Op, "newobj", StringComparison.Ordinal) &&
               string.Equals(instructions[6].Op, "stelem.ref", StringComparison.Ordinal) &&
               string.Equals(instructions[7].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[8].Op, "newarr", StringComparison.Ordinal) &&
               string.Equals(instructions[9].Op, "stloc", StringComparison.Ordinal) &&
               string.Equals(instructions[10].Op, "ldloc", StringComparison.Ordinal) &&
               string.Equals(instructions[11].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[12].Op, "call", StringComparison.Ordinal) &&
               string.Equals(instructions[13].Op, "ldloc", StringComparison.Ordinal) &&
               string.Equals(instructions[14].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[15].Op, "ldelem.ref", StringComparison.Ordinal) &&
               string.Equals(instructions[16].Op, "callvirt", StringComparison.Ordinal) &&
               string.Equals(instructions[17].Op, "call", StringComparison.Ordinal) &&
               string.Equals(instructions[18].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[19].Op, "ret", StringComparison.Ordinal);
    }

    private static bool IsArrayCopyReferenceArrayLengthOnlyInt64EntryPointShape(IReadOnlyList<TypedIlInstructionArtifact> instructions)
    {
        return instructions.Count == 21 &&
               string.Equals(instructions[0].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[1].Op, "newarr", StringComparison.Ordinal) &&
               string.Equals(instructions[2].Op, "dup", StringComparison.Ordinal) &&
               string.Equals(instructions[3].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[4].Op, "ldstr", StringComparison.Ordinal) &&
               string.Equals(instructions[5].Op, "newobj", StringComparison.Ordinal) &&
               string.Equals(instructions[6].Op, "stelem.ref", StringComparison.Ordinal) &&
               string.Equals(instructions[7].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[8].Op, "newarr", StringComparison.Ordinal) &&
               string.Equals(instructions[9].Op, "stloc", StringComparison.Ordinal) &&
               string.Equals(instructions[10].Op, "ldloc", StringComparison.Ordinal) &&
               string.Equals(instructions[11].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[12].Op, "conv.i8", StringComparison.Ordinal) &&
               string.Equals(instructions[13].Op, "call", StringComparison.Ordinal) &&
               string.Equals(instructions[14].Op, "ldloc", StringComparison.Ordinal) &&
               string.Equals(instructions[15].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[16].Op, "ldelem.ref", StringComparison.Ordinal) &&
               string.Equals(instructions[17].Op, "callvirt", StringComparison.Ordinal) &&
               string.Equals(instructions[18].Op, "call", StringComparison.Ordinal) &&
               string.Equals(instructions[19].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[20].Op, "ret", StringComparison.Ordinal);
    }

    private static bool IsArrayCopyReferenceArrayIndexedEntryPointShape(IReadOnlyList<TypedIlInstructionArtifact> instructions)
    {
        return IsArrayCopyReferenceArrayIndexedInt32EntryPointShape(instructions) ||
               IsArrayCopyReferenceArrayIndexedInt64EntryPointShape(instructions);
    }

    private static bool IsArrayCopyReferenceArrayIndexedInt32EntryPointShape(IReadOnlyList<TypedIlInstructionArtifact> instructions)
    {
        return instructions.Count == 22 &&
               string.Equals(instructions[0].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[1].Op, "newarr", StringComparison.Ordinal) &&
               string.Equals(instructions[2].Op, "dup", StringComparison.Ordinal) &&
               string.Equals(instructions[3].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[4].Op, "ldstr", StringComparison.Ordinal) &&
               string.Equals(instructions[5].Op, "newobj", StringComparison.Ordinal) &&
               string.Equals(instructions[6].Op, "stelem.ref", StringComparison.Ordinal) &&
               string.Equals(instructions[7].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[8].Op, "newarr", StringComparison.Ordinal) &&
               string.Equals(instructions[9].Op, "stloc", StringComparison.Ordinal) &&
               string.Equals(instructions[10].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[11].Op, "ldloc", StringComparison.Ordinal) &&
               string.Equals(instructions[12].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[13].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[14].Op, "call", StringComparison.Ordinal) &&
               string.Equals(instructions[15].Op, "ldloc", StringComparison.Ordinal) &&
               string.Equals(instructions[16].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[17].Op, "ldelem.ref", StringComparison.Ordinal) &&
               string.Equals(instructions[18].Op, "callvirt", StringComparison.Ordinal) &&
               string.Equals(instructions[19].Op, "call", StringComparison.Ordinal) &&
               string.Equals(instructions[20].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[21].Op, "ret", StringComparison.Ordinal);
    }

    private static bool IsArrayCopyReferenceArrayIndexedInt64EntryPointShape(IReadOnlyList<TypedIlInstructionArtifact> instructions)
    {
        return instructions.Count == 25 &&
               string.Equals(instructions[0].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[1].Op, "newarr", StringComparison.Ordinal) &&
               string.Equals(instructions[2].Op, "dup", StringComparison.Ordinal) &&
               string.Equals(instructions[3].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[4].Op, "ldstr", StringComparison.Ordinal) &&
               string.Equals(instructions[5].Op, "newobj", StringComparison.Ordinal) &&
               string.Equals(instructions[6].Op, "stelem.ref", StringComparison.Ordinal) &&
               string.Equals(instructions[7].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[8].Op, "newarr", StringComparison.Ordinal) &&
               string.Equals(instructions[9].Op, "stloc", StringComparison.Ordinal) &&
               string.Equals(instructions[10].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[11].Op, "conv.i8", StringComparison.Ordinal) &&
               string.Equals(instructions[12].Op, "ldloc", StringComparison.Ordinal) &&
               string.Equals(instructions[13].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[14].Op, "conv.i8", StringComparison.Ordinal) &&
               string.Equals(instructions[15].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[16].Op, "conv.i8", StringComparison.Ordinal) &&
               string.Equals(instructions[17].Op, "call", StringComparison.Ordinal) &&
               string.Equals(instructions[18].Op, "ldloc", StringComparison.Ordinal) &&
               string.Equals(instructions[19].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[20].Op, "ldelem.ref", StringComparison.Ordinal) &&
               string.Equals(instructions[21].Op, "callvirt", StringComparison.Ordinal) &&
               string.Equals(instructions[22].Op, "call", StringComparison.Ordinal) &&
               string.Equals(instructions[23].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[24].Op, "ret", StringComparison.Ordinal);
    }

    private static bool IsArrayCopyToReferenceArrayTargetOffsetEntryPointShape(IReadOnlyList<TypedIlInstructionArtifact> instructions)
    {
        return IsArrayCopyToReferenceArrayTargetOffsetInt32EntryPointShape(instructions) ||
               IsArrayCopyToReferenceArrayTargetOffsetInt64EntryPointShape(instructions);
    }

    private static bool IsArrayCopyToReferenceArrayTargetOffsetInt32EntryPointShape(IReadOnlyList<TypedIlInstructionArtifact> instructions)
    {
        return instructions.Count == 20 &&
               string.Equals(instructions[0].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[1].Op, "newarr", StringComparison.Ordinal) &&
               string.Equals(instructions[2].Op, "dup", StringComparison.Ordinal) &&
               string.Equals(instructions[3].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[4].Op, "ldstr", StringComparison.Ordinal) &&
               string.Equals(instructions[5].Op, "newobj", StringComparison.Ordinal) &&
               string.Equals(instructions[6].Op, "stelem.ref", StringComparison.Ordinal) &&
               string.Equals(instructions[7].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[8].Op, "newarr", StringComparison.Ordinal) &&
               string.Equals(instructions[9].Op, "stloc", StringComparison.Ordinal) &&
               string.Equals(instructions[10].Op, "ldloc", StringComparison.Ordinal) &&
               string.Equals(instructions[11].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[12].Op, "callvirt", StringComparison.Ordinal) &&
               string.Equals(instructions[13].Op, "ldloc", StringComparison.Ordinal) &&
               string.Equals(instructions[14].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[15].Op, "ldelem.ref", StringComparison.Ordinal) &&
               string.Equals(instructions[16].Op, "callvirt", StringComparison.Ordinal) &&
               string.Equals(instructions[17].Op, "call", StringComparison.Ordinal) &&
               string.Equals(instructions[18].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[19].Op, "ret", StringComparison.Ordinal);
    }

    private static bool IsArrayCopyToReferenceArrayTargetOffsetInt64EntryPointShape(IReadOnlyList<TypedIlInstructionArtifact> instructions)
    {
        return instructions.Count == 21 &&
               string.Equals(instructions[0].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[1].Op, "newarr", StringComparison.Ordinal) &&
               string.Equals(instructions[2].Op, "dup", StringComparison.Ordinal) &&
               string.Equals(instructions[3].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[4].Op, "ldstr", StringComparison.Ordinal) &&
               string.Equals(instructions[5].Op, "newobj", StringComparison.Ordinal) &&
               string.Equals(instructions[6].Op, "stelem.ref", StringComparison.Ordinal) &&
               string.Equals(instructions[7].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[8].Op, "newarr", StringComparison.Ordinal) &&
               string.Equals(instructions[9].Op, "stloc", StringComparison.Ordinal) &&
               string.Equals(instructions[10].Op, "ldloc", StringComparison.Ordinal) &&
               string.Equals(instructions[11].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[12].Op, "conv.i8", StringComparison.Ordinal) &&
               string.Equals(instructions[13].Op, "callvirt", StringComparison.Ordinal) &&
               string.Equals(instructions[14].Op, "ldloc", StringComparison.Ordinal) &&
               string.Equals(instructions[15].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[16].Op, "ldelem.ref", StringComparison.Ordinal) &&
               string.Equals(instructions[17].Op, "callvirt", StringComparison.Ordinal) &&
               string.Equals(instructions[18].Op, "call", StringComparison.Ordinal) &&
               string.Equals(instructions[19].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[20].Op, "ret", StringComparison.Ordinal);
    }

    private static bool IsSupportedArrayCopyMethodSubjectId(string subjectId)
    {
        return (subjectId.Contains("/System.Array::Copy:", StringComparison.Ordinal) &&
                (subjectId.EndsWith("(System.Array,System.Array,System.Int32)", StringComparison.Ordinal) ||
                 subjectId.EndsWith("(System.Array,System.Array,System.Int64)", StringComparison.Ordinal) ||
                 subjectId.EndsWith("(System.Array,System.Int32,System.Array,System.Int32,System.Int32)", StringComparison.Ordinal) ||
                 subjectId.EndsWith("(System.Array,System.Int64,System.Array,System.Int64,System.Int64)", StringComparison.Ordinal))) ||
               (subjectId.Contains("/System.Array::ConstrainedCopy:", StringComparison.Ordinal) &&
                subjectId.EndsWith("(System.Array,System.Int32,System.Array,System.Int32,System.Int32)", StringComparison.Ordinal)) ||
               (subjectId.Contains("/System.Array::CopyTo:", StringComparison.Ordinal) &&
                (subjectId.EndsWith("(System.Array,System.Int32)", StringComparison.Ordinal) ||
                 subjectId.EndsWith("(System.Array,System.Int64)", StringComparison.Ordinal)));
    }

    private static bool IsSupportedArrayReverseMethodSubjectId(string subjectId)
    {
        return subjectId.Contains("/System.Array::Reverse", StringComparison.Ordinal);
    }

    private static bool IsSupportedArrayClearMethodSubjectId(string subjectId)
    {
        return subjectId.Contains("/System.Array::Clear:", StringComparison.Ordinal) &&
               (subjectId.EndsWith("(System.Array,System.Int32,System.Int32)", StringComparison.Ordinal) ||
                subjectId.EndsWith("(System.Array)", StringComparison.Ordinal));
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

    private static void ValidateRuntimeSkeletonBoxedIConvertibleCharEntryShape(
        TypedIlMethodArtifact method,
        IReadOnlyList<TypedIlInstructionArtifact> instructions)
    {
        RequireMethodContract(method, "static-method", "has-canonical-body");
        RequireInstructionCount(method, instructions, 5);
        RequireInstructionOp(instructions[0], "ldarg", method.SubjectId, 0);
        RequireInstructionOp(instructions[1], "box", method.SubjectId, 1);
        RequireInstructionOp(instructions[2], "ldnull", method.SubjectId, 2);
        RequireInstructionOp(instructions[3], "callvirt", method.SubjectId, 3);
        RequireInstructionOp(instructions[4], "ret", method.SubjectId, 4);
        if (GetRequiredOperandInt(instructions[0]) != 0)
        {
            throw new InvalidOperationException(
                $"native-reference emitter expects boxed iconvertible char entry '{method.SubjectId}' to load ldarg 0");
        }

        RequireInstructionCallee(
            instructions[3],
            "System.Private.CoreLib/System.IConvertible::ToChar:System.Char(System.IFormatProvider)",
            method.SubjectId,
            3);
    }

    private static void ValidateRuntimeSkeletonBoxedIConvertibleCharInvalidCastTargetShape(
        TypedIlMethodArtifact method,
        IReadOnlyList<TypedIlInstructionArtifact> instructions)
    {
        RequireMethodContract(method, "instance-method", "has-canonical-body");
        if (!string.Equals(GetMethodReturnType(method.SubjectId), "System.Char", StringComparison.Ordinal) ||
            method.Parameters.Count != 1 ||
            !string.Equals(method.Parameters[0].Type, "System.IFormatProvider", StringComparison.Ordinal))
        {
            throw new InvalidOperationException(
                $"native-reference emitter expects boxed iconvertible char invalid-cast target '{method.SubjectId}' to be an instance method returning char with a single IFormatProvider parameter");
        }

        RequireInstructionCount(method, instructions, 6);
        RequireInstructionOp(instructions[0], "call", method.SubjectId, 0);
        RequireInstructionOp(instructions[1], "ldstr", method.SubjectId, 1);
        RequireInstructionOp(instructions[2], "ldstr", method.SubjectId, 2);
        RequireInstructionOp(instructions[3], "call", method.SubjectId, 3);
        RequireInstructionOp(instructions[4], "newobj", method.SubjectId, 4);
        RequireInstructionOp(instructions[5], "throw", method.SubjectId, 5);
        RequireInstructionCallee(
            instructions[0],
            "System.Private.CoreLib/System.SR::get_InvalidCast_FromTo:System.String()",
            method.SubjectId,
            0);
        RequireInstructionCallee(
            instructions[3],
            "System.Private.CoreLib/System.SR::Format:System.String(System.String,System.Object,System.Object)",
            method.SubjectId,
            3);

        var exceptionConstructorSubjectId = GetRequiredInstructionCallee(instructions[4], method.SubjectId, 4);
        if (!string.Equals(
                exceptionConstructorSubjectId,
                "System.Private.CoreLib/System.InvalidCastException::.ctor:System.Void(System.String)",
                StringComparison.Ordinal))
        {
            throw new InvalidOperationException(
                $"native-reference emitter expects boxed iconvertible char invalid-cast target '{method.SubjectId}' to construct InvalidCastException(string), but found '{exceptionConstructorSubjectId}'");
        }
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
        var returnTypeSeparatorIndex = subjectId.LastIndexOf(':');
        var parameterSeparatorIndex = subjectId.IndexOf('(', methodSeparatorIndex + 2);
        if (methodSeparatorIndex <= 0 || parameterSeparatorIndex <= methodSeparatorIndex + 2)
        {
            throw new InvalidOperationException($"failed to extract method name from subject id '{subjectId}'");
        }

        var methodEndIndex = returnTypeSeparatorIndex > methodSeparatorIndex
            && returnTypeSeparatorIndex < parameterSeparatorIndex
            ? returnTypeSeparatorIndex
            : parameterSeparatorIndex;
        if (methodEndIndex <= methodSeparatorIndex + 2)
        {
            throw new InvalidOperationException($"failed to extract method name from subject id '{subjectId}'");
        }

        return subjectId[(methodSeparatorIndex + 2)..methodEndIndex];
    }

    private static IReadOnlyList<string> GetMethodParameterTypesFromSubjectId(string subjectId)
    {
        var startIndex = subjectId.IndexOf('(', StringComparison.Ordinal);
        var endIndex = subjectId.LastIndexOf(')');
        if (startIndex < 0 || endIndex < startIndex)
        {
            throw new InvalidOperationException($"failed to extract parameter types from subject id '{subjectId}'");
        }

        var parameterList = subjectId[(startIndex + 1)..endIndex];
        if (string.IsNullOrWhiteSpace(parameterList))
        {
            return Array.Empty<string>();
        }

        var parameters = new List<string>();
        var segmentStart = 0;
        var genericDepth = 0;
        for (var index = 0; index < parameterList.Length; index++)
        {
            switch (parameterList[index])
            {
                case '<':
                    genericDepth++;
                    break;
                case '>':
                    genericDepth--;
                    break;
                case ',' when genericDepth == 0:
                    parameters.Add(parameterList[segmentStart..index].Trim());
                    segmentStart = index + 1;
                    break;
            }
        }

        parameters.Add(parameterList[segmentStart..].Trim());
        return parameters;
    }

    private static string GetMethodReturnType(string subjectId)
    {
        var returnTypeSeparatorIndex = subjectId.LastIndexOf(':');
        var parameterSeparatorIndex = subjectId.IndexOf('(', StringComparison.Ordinal);
        if (returnTypeSeparatorIndex <= 0 || parameterSeparatorIndex <= returnTypeSeparatorIndex + 1)
        {
            throw new InvalidOperationException($"failed to extract return type from subject id '{subjectId}'");
        }

        return subjectId[(returnTypeSeparatorIndex + 1)..parameterSeparatorIndex];
    }

    private static bool IsCompilerGeneratedAsyncStateMachineMethodSubjectId(string subjectId)
    {
        var declaringTypeSubjectId = GetDeclaringTypeSubjectId(subjectId);
        return declaringTypeSubjectId.Contains("+<", StringComparison.Ordinal) &&
               declaringTypeSubjectId.Contains(">d__", StringComparison.Ordinal);
    }

    private static bool IsSupportedAsyncAwaitUnsafeOnCompletedCallee(string callee)
    {
        return IsSupportedAsyncBuilderCallee(callee, "::AwaitUnsafeOnCompleted<");
    }

    private static bool IsSupportedAsyncSetResultCallee(string callee)
    {
        return IsSupportedAsyncBuilderCallee(callee, "::SetResult:System.Void(");
    }

    private static bool IsSupportedAsyncSetExceptionCallee(string callee)
    {
        return IsSupportedAsyncBuilderCallee(callee, "::SetException:System.Void(System.Exception)");
    }

    private static bool IsSupportedAsyncSetStateMachineCallee(string callee)
    {
        return IsSupportedAsyncBuilderCallee(callee, "::SetStateMachine:System.Void(System.Runtime.CompilerServices.IAsyncStateMachine)");
    }

    private static bool IsSupportedAsyncTaskAwaiterGetResultCallee(string callee)
    {
        return IsSupportedAsyncAwaiterCallee(callee, "System.Private.CoreLib/System.Runtime.CompilerServices.TaskAwaiter<") ||
               IsSupportedAsyncAwaiterCallee(callee, "System.Private.CoreLib/System.Runtime.CompilerServices.ValueTaskAwaiter<");
    }

    private static bool IsSupportedAsyncBuilderCallee(string callee, string marker)
    {
        return (callee.StartsWith("System.Private.CoreLib/System.Runtime.CompilerServices.AsyncTaskMethodBuilder<", StringComparison.Ordinal) ||
                callee.StartsWith("System.Private.CoreLib/System.Runtime.CompilerServices.AsyncValueTaskMethodBuilder<", StringComparison.Ordinal)) &&
               callee.Contains(marker, StringComparison.Ordinal);
    }

    private static bool IsSupportedAsyncAwaiterCallee(string callee, string prefix)
    {
        return callee.StartsWith(prefix, StringComparison.Ordinal) &&
               callee.Contains("::GetResult:", StringComparison.Ordinal) &&
               callee.EndsWith("()", StringComparison.Ordinal);
    }

    private static bool TryParseClosedTaskResultType(string taskReturnType, out string managedResultType)
    {
        const string taskPrefix = "System.Threading.Tasks.Task<";
        managedResultType = string.Empty;
        if (!taskReturnType.StartsWith(taskPrefix, StringComparison.Ordinal) ||
            !taskReturnType.EndsWith(">", StringComparison.Ordinal) ||
            taskReturnType.Length <= taskPrefix.Length + 1)
        {
            return false;
        }

        managedResultType = taskReturnType[taskPrefix.Length..^1];
        return !string.IsNullOrWhiteSpace(managedResultType);
    }

    private static bool TryResolveAsyncManagedTypeShape(string managedType, out AsyncManagedTypeShape typeShape)
    {
        switch (managedType)
        {
            case "System.Boolean":
            case "System.Byte":
            case "System.SByte":
            case "System.Int16":
            case "System.UInt16":
            case "System.Int32":
            case "System.UInt32":
            case "System.Char":
                typeShape = new AsyncManagedTypeShape(managedType, "CHAOS_IL2CPP_INT32", "0", "ldc.i4", "integer-like literals");
                return true;
            case "System.Int64":
            case "System.UInt64":
                typeShape = new AsyncManagedTypeShape(managedType, "CHAOS_IL2CPP_INT64", "0", "ldc.i8", "int64 literals");
                return true;
            case "System.Single":
                typeShape = new AsyncManagedTypeShape(managedType, "float", "0.0f", "ldc.r4", "float literals");
                return true;
            case "System.Double":
                typeShape = new AsyncManagedTypeShape(managedType, "double", "0.0", "ldc.r8", "double literals");
                return true;
            case "System.String":
                typeShape = new AsyncManagedTypeShape(managedType, "void*", "nullptr", "ldstr", "string literals");
                return true;
            case "System.Object":
                typeShape = new AsyncManagedTypeShape(managedType, "void*", "nullptr", "ldnull", "null literals");
                return true;
            default:
                typeShape = null!;
                return false;
        }
    }

    private static bool TryResolveRuntimeSkeletonPrimitiveConvertShape(
        string inputManagedType,
        string outputManagedType,
        out string inputCppType,
        out string outputCppType,
        out string convertedValueExpression)
    {
        inputCppType = string.Empty;
        outputCppType = string.Empty;
        convertedValueExpression = string.Empty;

        if (string.Equals(inputManagedType, "System.Boolean", StringComparison.Ordinal) &&
            string.Equals(outputManagedType, "System.Byte", StringComparison.Ordinal))
        {
            inputCppType = "bool";
            outputCppType = "CHAOS_IL2CPP_UINT8";
            convertedValueExpression = "request->value ? static_cast<CHAOS_IL2CPP_UINT8>(1) : static_cast<CHAOS_IL2CPP_UINT8>(0)";
            return true;
        }

        if (string.Equals(inputManagedType, "System.Byte", StringComparison.Ordinal) &&
            string.Equals(outputManagedType, "System.Boolean", StringComparison.Ordinal))
        {
            inputCppType = "CHAOS_IL2CPP_UINT8";
            outputCppType = "bool";
            convertedValueExpression = "request->value != static_cast<CHAOS_IL2CPP_UINT8>(0)";
            return true;
        }

        if (string.Equals(inputManagedType, outputManagedType, StringComparison.Ordinal) &&
            TryResolveRuntimeSkeletonPrimitiveConvertInputCppType(inputManagedType, out inputCppType))
        {
            outputCppType = inputCppType;
            convertedValueExpression = "request->value";
            return true;
        }

        if (TryResolveRuntimeSkeletonPrimitiveConvertInputCppType(inputManagedType, out inputCppType) &&
            TryResolveRuntimeSkeletonPrimitiveConvertOutputCppType(outputManagedType, out outputCppType))
        {
            convertedValueExpression = "request->value";
            return true;
        }

        if (string.Equals(outputManagedType, "System.Boolean", StringComparison.Ordinal) &&
            TryResolveRuntimeSkeletonPrimitiveConvertInputCppType(inputManagedType, out inputCppType))
        {
            outputCppType = "bool";
            convertedValueExpression = "request->value != 0";
            return true;
        }

        return false;
    }

    private static bool MatchesRuntimeSkeletonPrimitiveConvertInstructionShape(
        IReadOnlyList<TypedIlInstructionArtifact> instructions,
        string inputManagedType,
        string outputManagedType)
    {
        if (instructions.Count == 2)
        {
            return string.Equals(instructions[0].Op, "ldarg", StringComparison.Ordinal) &&
                   GetRequiredOperandInt(instructions[0]) == 0 &&
                   string.Equals(instructions[1].Op, "ret", StringComparison.Ordinal) &&
                   TryResolveRuntimeSkeletonPrimitiveConvertInputCppType(inputManagedType, out _) &&
                   TryResolveRuntimeSkeletonPrimitiveConvertOutputCppType(outputManagedType, out _) &&
                   IsRuntimeSkeletonDirectPrimitiveValuePreservingReturn(inputManagedType, outputManagedType);
        }

        if (TryMatchRuntimeSkeletonPrimitiveConvRetInstructionShape(instructions, inputManagedType, outputManagedType))
        {
            return true;
        }

        if (instructions.Count == 6)
        {
            return string.Equals(outputManagedType, "System.Boolean", StringComparison.Ordinal) &&
                   (string.Equals(inputManagedType, "System.Single", StringComparison.Ordinal) ||
                    string.Equals(inputManagedType, "System.Double", StringComparison.Ordinal)) &&
                   string.Equals(instructions[0].Op, "ldarg", StringComparison.Ordinal) &&
                   GetRequiredOperandInt(instructions[0]) == 0 &&
                   string.Equals(instructions[1].Op, string.Equals(inputManagedType, "System.Single", StringComparison.Ordinal) ? "ldc.r4" : "ldc.r8", StringComparison.Ordinal) &&
                   IsZeroLiteralOperand(instructions[1]) &&
                   string.Equals(instructions[2].Op, "ceq", StringComparison.Ordinal) &&
                   string.Equals(instructions[3].Op, "ldc.i4", StringComparison.Ordinal) &&
                   GetRequiredOperandInt(instructions[3]) == 0 &&
                   string.Equals(instructions[4].Op, "ceq", StringComparison.Ordinal) &&
                   string.Equals(instructions[5].Op, "ret", StringComparison.Ordinal);
        }

        if (instructions.Count < 4 || instructions.Count > 5)
        {
            return false;
        }

        if (!string.Equals(instructions[0].Op, "ldarg", StringComparison.Ordinal) ||
            GetRequiredOperandInt(instructions[0]) != 0 ||
            !string.Equals(instructions[1].Op, "ldc.i4", StringComparison.Ordinal) ||
            GetRequiredOperandInt(instructions[1]) != 0)
        {
            return false;
        }

        var compareInstructionIndex = 2;
        if (instructions.Count == 5 &&
            string.Equals(instructions[2].Op, "conv.i8", StringComparison.Ordinal))
        {
            compareInstructionIndex = 3;
        }

        var compareOp = instructions[compareInstructionIndex].Op;
        if (!string.Equals(compareOp, "cgt", StringComparison.Ordinal) &&
            !string.Equals(compareOp, "cgt.un", StringComparison.Ordinal))
        {
            return false;
        }

        if (compareInstructionIndex == 3)
        {
            return string.Equals(instructions[4].Op, "ret", StringComparison.Ordinal) &&
                   string.Equals(outputManagedType, "System.Boolean", StringComparison.Ordinal) &&
                   (string.Equals(inputManagedType, "System.Int64", StringComparison.Ordinal) ||
                    string.Equals(inputManagedType, "System.UInt64", StringComparison.Ordinal)) &&
                   string.Equals(compareOp, "cgt.un", StringComparison.Ordinal);
        }

        if (instructions.Count == 4)
        {
            return string.Equals(instructions[3].Op, "ret", StringComparison.Ordinal) &&
                   string.Equals(outputManagedType, "System.Boolean", StringComparison.Ordinal) &&
                   (string.Equals(compareOp, "cgt", StringComparison.Ordinal) ||
                    string.Equals(compareOp, "cgt.un", StringComparison.Ordinal));
        }

        return string.Equals(instructions[3].Op, "conv.u1", StringComparison.Ordinal) &&
               string.Equals(instructions[4].Op, "ret", StringComparison.Ordinal) &&
               string.Equals(inputManagedType, "System.Boolean", StringComparison.Ordinal) &&
               string.Equals(outputManagedType, "System.Byte", StringComparison.Ordinal) &&
               string.Equals(compareOp, "cgt.un", StringComparison.Ordinal);
    }

    private static bool TryResolveRuntimeSkeletonCheckedByteConvertShape(
        string subjectId,
        string inputManagedType,
        IReadOnlyList<TypedIlInstructionArtifact> instructions,
        out string inputCppType,
        out string overflowConditionExpression,
        out string throwSubjectId)
    {
        inputCppType = string.Empty;
        overflowConditionExpression = string.Empty;
        throwSubjectId = string.Empty;

        if (!TryResolveRuntimeSkeletonPrimitiveConvertInputCppType(inputManagedType, out inputCppType))
        {
            return false;
        }

        if (string.Equals(inputManagedType, "System.UInt64", StringComparison.Ordinal))
        {
            if (instructions.Count != 8 ||
                !string.Equals(instructions[0].Op, "ldarg", StringComparison.Ordinal) ||
                GetRequiredOperandInt(instructions[0]) != 0 ||
                !string.Equals(instructions[1].Op, "ldc.i4", StringComparison.Ordinal) ||
                GetRequiredOperandInt(instructions[1]) != 255 ||
                !string.Equals(instructions[2].Op, "conv.i8", StringComparison.Ordinal) ||
                !string.Equals(instructions[3].Op, "ble.un", StringComparison.Ordinal) ||
                !string.Equals(instructions[4].Op, "call", StringComparison.Ordinal) ||
                !string.Equals(instructions[5].Op, "ldarg", StringComparison.Ordinal) ||
                GetRequiredOperandInt(instructions[5]) != 0 ||
                !string.Equals(instructions[6].Op, "conv.u1", StringComparison.Ordinal) ||
                !string.Equals(instructions[7].Op, "ret", StringComparison.Ordinal))
            {
                return false;
            }

            throwSubjectId = GetRequiredInstructionCallee(instructions[4], subjectId, 4);
            overflowConditionExpression = "request->value > static_cast<CHAOS_IL2CPP_UINT64>(255)";
            return true;
        }

        switch (inputManagedType)
        {
            case "System.SByte":
                if (instructions.Count != 7 ||
                    !string.Equals(instructions[0].Op, "ldarg", StringComparison.Ordinal) ||
                    GetRequiredOperandInt(instructions[0]) != 0 ||
                    !string.Equals(instructions[1].Op, "ldc.i4", StringComparison.Ordinal) ||
                    GetRequiredOperandInt(instructions[1]) != 0 ||
                    !string.Equals(instructions[2].Op, "bge", StringComparison.Ordinal) ||
                    !string.Equals(instructions[3].Op, "call", StringComparison.Ordinal) ||
                    !string.Equals(instructions[4].Op, "ldarg", StringComparison.Ordinal) ||
                    GetRequiredOperandInt(instructions[4]) != 0 ||
                    !string.Equals(instructions[5].Op, "conv.u1", StringComparison.Ordinal) ||
                    !string.Equals(instructions[6].Op, "ret", StringComparison.Ordinal))
                {
                    return false;
                }
                throwSubjectId = GetRequiredInstructionCallee(instructions[3], subjectId, 3);
                overflowConditionExpression = "request->value < static_cast<CHAOS_IL2CPP_INT8>(0)";
                return true;
            case "System.Int16":
                if (instructions.Count != 7 ||
                    !string.Equals(instructions[0].Op, "ldarg", StringComparison.Ordinal) ||
                    GetRequiredOperandInt(instructions[0]) != 0 ||
                    !string.Equals(instructions[1].Op, "ldc.i4", StringComparison.Ordinal) ||
                    GetRequiredOperandInt(instructions[1]) != 255 ||
                    !string.Equals(instructions[2].Op, "ble.un", StringComparison.Ordinal) ||
                    !string.Equals(instructions[3].Op, "call", StringComparison.Ordinal) ||
                    !string.Equals(instructions[4].Op, "ldarg", StringComparison.Ordinal) ||
                    GetRequiredOperandInt(instructions[4]) != 0 ||
                    !string.Equals(instructions[5].Op, "conv.u1", StringComparison.Ordinal) ||
                    !string.Equals(instructions[6].Op, "ret", StringComparison.Ordinal))
                {
                    return false;
                }
                throwSubjectId = GetRequiredInstructionCallee(instructions[3], subjectId, 3);
                overflowConditionExpression = "request->value < static_cast<CHAOS_IL2CPP_INT16>(0) || request->value > static_cast<CHAOS_IL2CPP_INT16>(255)";
                return true;
            case "System.UInt16":
                if (instructions.Count != 7 ||
                    !string.Equals(instructions[0].Op, "ldarg", StringComparison.Ordinal) ||
                    GetRequiredOperandInt(instructions[0]) != 0 ||
                    !string.Equals(instructions[1].Op, "ldc.i4", StringComparison.Ordinal) ||
                    GetRequiredOperandInt(instructions[1]) != 255 ||
                    !string.Equals(instructions[2].Op, "ble", StringComparison.Ordinal) ||
                    !string.Equals(instructions[3].Op, "call", StringComparison.Ordinal) ||
                    !string.Equals(instructions[4].Op, "ldarg", StringComparison.Ordinal) ||
                    GetRequiredOperandInt(instructions[4]) != 0 ||
                    !string.Equals(instructions[5].Op, "conv.u1", StringComparison.Ordinal) ||
                    !string.Equals(instructions[6].Op, "ret", StringComparison.Ordinal))
                {
                    return false;
                }
                throwSubjectId = GetRequiredInstructionCallee(instructions[3], subjectId, 3);
                overflowConditionExpression = "request->value > static_cast<CHAOS_IL2CPP_UINT16>(255)";
                return true;
            case "System.UInt32":
                if (instructions.Count != 7 ||
                    !string.Equals(instructions[0].Op, "ldarg", StringComparison.Ordinal) ||
                    GetRequiredOperandInt(instructions[0]) != 0 ||
                    !string.Equals(instructions[1].Op, "ldc.i4", StringComparison.Ordinal) ||
                    GetRequiredOperandInt(instructions[1]) != 255 ||
                    !string.Equals(instructions[2].Op, "ble.un", StringComparison.Ordinal) ||
                    !string.Equals(instructions[3].Op, "call", StringComparison.Ordinal) ||
                    !string.Equals(instructions[4].Op, "ldarg", StringComparison.Ordinal) ||
                    GetRequiredOperandInt(instructions[4]) != 0 ||
                    !string.Equals(instructions[5].Op, "conv.u1", StringComparison.Ordinal) ||
                    !string.Equals(instructions[6].Op, "ret", StringComparison.Ordinal))
                {
                    return false;
                }
                throwSubjectId = GetRequiredInstructionCallee(instructions[3], subjectId, 3);
                overflowConditionExpression = "request->value > static_cast<CHAOS_IL2CPP_UINT32>(255)";
                return true;
            case "System.Char":
                if (instructions.Count != 7 ||
                    !string.Equals(instructions[0].Op, "ldarg", StringComparison.Ordinal) ||
                    GetRequiredOperandInt(instructions[0]) != 0 ||
                    !string.Equals(instructions[1].Op, "ldc.i4", StringComparison.Ordinal) ||
                    GetRequiredOperandInt(instructions[1]) != 255 ||
                    !string.Equals(instructions[2].Op, "ble", StringComparison.Ordinal) ||
                    !string.Equals(instructions[3].Op, "call", StringComparison.Ordinal) ||
                    !string.Equals(instructions[4].Op, "ldarg", StringComparison.Ordinal) ||
                    GetRequiredOperandInt(instructions[4]) != 0 ||
                    !string.Equals(instructions[5].Op, "conv.u1", StringComparison.Ordinal) ||
                    !string.Equals(instructions[6].Op, "ret", StringComparison.Ordinal))
                {
                    return false;
                }
                throwSubjectId = GetRequiredInstructionCallee(instructions[3], subjectId, 3);
                overflowConditionExpression = "request->value > static_cast<CHAOS_IL2CPP_UINT16>(255)";
                return true;
            default:
                return false;
        }
    }

    private static bool TryResolveRuntimeSkeletonCheckedCharConvertShape(
        string subjectId,
        string inputManagedType,
        IReadOnlyList<TypedIlInstructionArtifact> instructions,
        out string inputCppType,
        out string overflowConditionExpression,
        out string throwSubjectId)
    {
        inputCppType = string.Empty;
        overflowConditionExpression = string.Empty;
        throwSubjectId = string.Empty;

        if (!TryResolveRuntimeSkeletonPrimitiveConvertInputCppType(inputManagedType, out inputCppType))
        {
            return false;
        }

        if (string.Equals(inputManagedType, "System.UInt64", StringComparison.Ordinal))
        {
            if (instructions.Count != 8 ||
                !string.Equals(instructions[0].Op, "ldarg", StringComparison.Ordinal) ||
                GetRequiredOperandInt(instructions[0]) != 0 ||
                !string.Equals(instructions[1].Op, "ldc.i4", StringComparison.Ordinal) ||
                GetRequiredOperandInt(instructions[1]) != 65535 ||
                !string.Equals(instructions[2].Op, "conv.i8", StringComparison.Ordinal) ||
                !string.Equals(instructions[3].Op, "ble.un", StringComparison.Ordinal) ||
                !string.Equals(instructions[4].Op, "call", StringComparison.Ordinal) ||
                !string.Equals(instructions[5].Op, "ldarg", StringComparison.Ordinal) ||
                GetRequiredOperandInt(instructions[5]) != 0 ||
                !string.Equals(instructions[6].Op, "conv.u2", StringComparison.Ordinal) ||
                !string.Equals(instructions[7].Op, "ret", StringComparison.Ordinal))
            {
                return false;
            }

            throwSubjectId = GetRequiredInstructionCallee(instructions[4], subjectId, 4);
            overflowConditionExpression = "request->value > static_cast<CHAOS_IL2CPP_UINT64>(65535)";
            return true;
        }

        switch (inputManagedType)
        {
            case "System.SByte":
                if (instructions.Count != 7 ||
                    !string.Equals(instructions[0].Op, "ldarg", StringComparison.Ordinal) ||
                    GetRequiredOperandInt(instructions[0]) != 0 ||
                    !string.Equals(instructions[1].Op, "ldc.i4", StringComparison.Ordinal) ||
                    GetRequiredOperandInt(instructions[1]) != 0 ||
                    !string.Equals(instructions[2].Op, "bge", StringComparison.Ordinal) ||
                    !string.Equals(instructions[3].Op, "call", StringComparison.Ordinal) ||
                    !string.Equals(instructions[4].Op, "ldarg", StringComparison.Ordinal) ||
                    GetRequiredOperandInt(instructions[4]) != 0 ||
                    !string.Equals(instructions[5].Op, "conv.u2", StringComparison.Ordinal) ||
                    !string.Equals(instructions[6].Op, "ret", StringComparison.Ordinal))
                {
                    return false;
                }
                throwSubjectId = GetRequiredInstructionCallee(instructions[3], subjectId, 3);
                overflowConditionExpression = "request->value < static_cast<CHAOS_IL2CPP_INT8>(0)";
                return true;
            case "System.Int16":
                if (instructions.Count != 7 ||
                    !string.Equals(instructions[0].Op, "ldarg", StringComparison.Ordinal) ||
                    GetRequiredOperandInt(instructions[0]) != 0 ||
                    !string.Equals(instructions[1].Op, "ldc.i4", StringComparison.Ordinal) ||
                    GetRequiredOperandInt(instructions[1]) != 0 ||
                    !string.Equals(instructions[2].Op, "bge", StringComparison.Ordinal) ||
                    !string.Equals(instructions[3].Op, "call", StringComparison.Ordinal) ||
                    !string.Equals(instructions[4].Op, "ldarg", StringComparison.Ordinal) ||
                    GetRequiredOperandInt(instructions[4]) != 0 ||
                    !string.Equals(instructions[5].Op, "conv.u2", StringComparison.Ordinal) ||
                    !string.Equals(instructions[6].Op, "ret", StringComparison.Ordinal))
                {
                    return false;
                }
                throwSubjectId = GetRequiredInstructionCallee(instructions[3], subjectId, 3);
                overflowConditionExpression = "request->value < static_cast<CHAOS_IL2CPP_INT16>(0)";
                return true;
            case "System.UInt32":
                if (instructions.Count != 7 ||
                    !string.Equals(instructions[0].Op, "ldarg", StringComparison.Ordinal) ||
                    GetRequiredOperandInt(instructions[0]) != 0 ||
                    !string.Equals(instructions[1].Op, "ldc.i4", StringComparison.Ordinal) ||
                    GetRequiredOperandInt(instructions[1]) != 65535 ||
                    !string.Equals(instructions[2].Op, "ble.un", StringComparison.Ordinal) ||
                    !string.Equals(instructions[3].Op, "call", StringComparison.Ordinal) ||
                    !string.Equals(instructions[4].Op, "ldarg", StringComparison.Ordinal) ||
                    GetRequiredOperandInt(instructions[4]) != 0 ||
                    !string.Equals(instructions[5].Op, "conv.u2", StringComparison.Ordinal) ||
                    !string.Equals(instructions[6].Op, "ret", StringComparison.Ordinal))
                {
                    return false;
                }
                throwSubjectId = GetRequiredInstructionCallee(instructions[3], subjectId, 3);
                overflowConditionExpression = "request->value > static_cast<CHAOS_IL2CPP_UINT32>(65535)";
                return true;
            default:
                return false;
        }
    }

    private static bool TryResolveRuntimeSkeletonCheckedPrimitiveConvertShape(
        string subjectId,
        string inputManagedType,
        string outputManagedType,
        IReadOnlyList<TypedIlInstructionArtifact> instructions,
        out string inputCppType,
        out string overflowConditionExpression,
        out string throwSubjectId)
    {
        inputCppType = string.Empty;
        overflowConditionExpression = string.Empty;
        throwSubjectId = string.Empty;

        if (!TryResolveRuntimeSkeletonPrimitiveConvertInputCppType(inputManagedType, out inputCppType))
        {
            return false;
        }

        if (string.Equals(outputManagedType, "System.Byte", StringComparison.Ordinal))
        {
            return TryResolveRuntimeSkeletonCheckedByteConvertShape(
                subjectId, inputManagedType, instructions,
                out inputCppType, out overflowConditionExpression, out throwSubjectId);
        }

        if (string.Equals(outputManagedType, "System.Char", StringComparison.Ordinal))
        {
            return TryResolveRuntimeSkeletonCheckedCharConvertShape(
                subjectId, inputManagedType, instructions,
                out inputCppType, out overflowConditionExpression, out throwSubjectId);
        }

        // For remaining output types, determine the type-specific constants
        string convOp;
        int maxValue;
        bool isSigned;
        if (string.Equals(outputManagedType, "System.SByte", StringComparison.Ordinal))
        {
            convOp = "conv.i1";
            maxValue = 127;
            isSigned = true;
        }
        else if (string.Equals(outputManagedType, "System.Int16", StringComparison.Ordinal))
        {
            convOp = "conv.i2";
            maxValue = 32767;
            isSigned = true;
        }
        else if (string.Equals(outputManagedType, "System.Int32", StringComparison.Ordinal))
        {
            convOp = "conv.i4";
            maxValue = 2147483647;
            isSigned = true;
        }
        else if (string.Equals(outputManagedType, "System.UInt32", StringComparison.Ordinal))
        {
            convOp = "conv.u4";
            maxValue = -1; // 4294967295 not representable as signed int, use uint expression
            isSigned = false;
        }
        else
        {
            return false;
        }

        // General pattern matching for checked primitive conversions
        string maxCppExpr;
        string maxBranchOp;
        if (string.Equals(outputManagedType, "System.UInt32", StringComparison.Ordinal))
        {
            maxCppExpr = "static_cast<CHAOS_IL2CPP_UINT64>(4294967295)";
            maxBranchOp = "ble.un";
        }
        else
        {
            maxCppExpr = $"static_cast<CHAOS_IL2CPP_INT32>({maxValue})";
            maxBranchOp = isSigned ? "ble" : "ble.un";
        }

        if (string.Equals(inputManagedType, "System.UInt64", StringComparison.Ordinal))
        {
            // UInt64: 8-instruction pattern with conv.i8
            if (instructions.Count != 8 ||
                !string.Equals(instructions[0].Op, "ldarg", StringComparison.Ordinal) ||
                GetRequiredOperandInt(instructions[0]) != 0 ||
                !string.Equals(instructions[1].Op, "ldc.i4", StringComparison.Ordinal) ||
                !string.Equals(instructions[2].Op, "conv.i8", StringComparison.Ordinal) ||
                !string.Equals(instructions[3].Op, "ble.un", StringComparison.Ordinal) ||
                !string.Equals(instructions[4].Op, "call", StringComparison.Ordinal) ||
                !string.Equals(instructions[5].Op, "ldarg", StringComparison.Ordinal) ||
                GetRequiredOperandInt(instructions[5]) != 0 ||
                !string.Equals(instructions[7].Op, "ret", StringComparison.Ordinal))
            {
                return false;
            }

            // Verify conv opcode at position 6
            if (!string.Equals(instructions[6].Op, convOp, StringComparison.Ordinal))
            {
                return false;
            }

            throwSubjectId = GetRequiredInstructionCallee(instructions[4], subjectId, 4);
            overflowConditionExpression = $"request->value > static_cast<CHAOS_IL2CPP_UINT64>({maxValue})";
            return true;
        }

        // 7-instruction patterns for non-UInt64 inputs
        if (instructions.Count != 7)
        {
            return false;
        }

        // Check common prefix: ldarg.0, ldc.i4, branch[, call, ldarg.0, conv.*, ret]
        if (!string.Equals(instructions[0].Op, "ldarg", StringComparison.Ordinal) ||
            GetRequiredOperandInt(instructions[0]) != 0 ||
            !string.Equals(instructions[1].Op, "ldc.i4", StringComparison.Ordinal) ||
            !string.Equals(instructions[4].Op, "ldarg", StringComparison.Ordinal) ||
            GetRequiredOperandInt(instructions[4]) != 0 ||
            !string.Equals(instructions[5].Op, convOp, StringComparison.Ordinal) ||
            !string.Equals(instructions[6].Op, "ret", StringComparison.Ordinal))
        {
            return false;
        }

        // Signed input types that need a dual (lower bound) check for signed output types
        // Use a sign check pattern via bge/bge.un with 0
        if (isSigned &&
            (string.Equals(inputManagedType, "System.Int16", StringComparison.Ordinal) ||
             string.Equals(inputManagedType, "System.Int32", StringComparison.Ordinal)))
        {
            // First check lower bound: ldarg.0, ldc.i4 <min>, ble/bge, call_throw
            // Then upper bound check: ... (if needed)
            // For Int16→SByte/Int32→SByte/Int32→Int16, the compiler generates the upper check pattern

            // For Int16→SByte: dual check (value < -128 || value > 127)
            // The IL pattern for this is: ble.un maxCheck which handles both directions
            // Actually the C# compiler uses a single ble.un for unsigned comparison
            // For Int16→SByte the comparison is actually against 127 with ble.un:
            //   ldarg.0, ldc.i4 127, ble.un, call_throw, ldarg.0, conv.i1, ret
            // This catches both < -128 and > 127 because negative Int16 wraps to > 65535 in unsigned

            // Check for single-comparison pattern: ldarg.0, ldc.i4 max, ble.un, call, ldarg.0, conv.*, ret
            if (GetRequiredOperandInt(instructions[1]) == maxValue &&
                string.Equals(instructions[2].Op, maxBranchOp, StringComparison.Ordinal))
            {
                throwSubjectId = GetRequiredInstructionCallee(instructions[3], subjectId, 3);
                overflowConditionExpression = $"request->value > {maxCppExpr}";
                // For signed inputs, also check lower bound
                int signedMin = 0;
                if (string.Equals(outputManagedType, "System.SByte", StringComparison.Ordinal))
                    signedMin = -128;
                else if (string.Equals(outputManagedType, "System.Int16", StringComparison.Ordinal))
                    signedMin = -32768;
                else if (string.Equals(outputManagedType, "System.Int32", StringComparison.Ordinal))
                    signedMin = -2147483648;
                overflowConditionExpression = $"request->value < static_cast<CHAOS_IL2CPP_INT32>({signedMin}) || {overflowConditionExpression}";
                return true;
            }

            return false;
        }

        if (string.Equals(inputManagedType, "System.SByte", StringComparison.Ordinal))
        {
            // SByte: negative check first
            if (GetRequiredOperandInt(instructions[1]) == 0 &&
                string.Equals(instructions[2].Op, "bge", StringComparison.Ordinal))
            {
                throwSubjectId = GetRequiredInstructionCallee(instructions[3], subjectId, 3);
                overflowConditionExpression = "request->value < static_cast<CHAOS_IL2CPP_INT8>(0)";
                return true;
            }
            return false;
        }

        if (string.Equals(inputManagedType, "System.Int16", StringComparison.Ordinal))
        {
            // For unsigned output types with unsigned comparison: upper check only
            if (!isSigned && GetRequiredOperandInt(instructions[1]) == maxValue &&
                string.Equals(instructions[2].Op, "ble", StringComparison.Ordinal))
            {
                throwSubjectId = GetRequiredInstructionCallee(instructions[3], subjectId, 3);
                overflowConditionExpression = $"request->value > {maxCppExpr}";
                return true;
            }
            // For signed output: already handled above
            return false;
        }

        if (string.Equals(inputManagedType, "System.UInt16", StringComparison.Ordinal))
        {
            if (GetRequiredOperandInt(instructions[1]) == maxValue &&
                string.Equals(instructions[2].Op, "ble", StringComparison.Ordinal))
            {
                throwSubjectId = GetRequiredInstructionCallee(instructions[3], subjectId, 3);
                overflowConditionExpression = $"request->value > {maxCppExpr}";
                return true;
            }
            return false;
        }

        if (string.Equals(inputManagedType, "System.UInt32", StringComparison.Ordinal))
        {
            if (GetRequiredOperandInt(instructions[1]) == maxValue &&
                string.Equals(instructions[2].Op, maxBranchOp, StringComparison.Ordinal))
            {
                throwSubjectId = GetRequiredInstructionCallee(instructions[3], subjectId, 3);
                overflowConditionExpression = $"request->value > {maxCppExpr}";
                return true;
            }
            return false;
        }

        if (string.Equals(inputManagedType, "System.Char", StringComparison.Ordinal))
        {
            if (GetRequiredOperandInt(instructions[1]) == maxValue &&
                string.Equals(instructions[2].Op, "ble", StringComparison.Ordinal))
            {
                throwSubjectId = GetRequiredInstructionCallee(instructions[3], subjectId, 3);
                overflowConditionExpression = $"request->value > {maxCppExpr}";
                return true;
            }
            return false;
        }

        if (string.Equals(inputManagedType, "System.Byte", StringComparison.Ordinal))
        {
            if (GetRequiredOperandInt(instructions[1]) == maxValue &&
                string.Equals(instructions[2].Op, "ble", StringComparison.Ordinal))
            {
                throwSubjectId = GetRequiredInstructionCallee(instructions[3], subjectId, 3);
                overflowConditionExpression = $"request->value > {maxCppExpr}";
                return true;
            }
            return false;
        }

        return false;
    }


    private static bool TryResolveRuntimeSkeletonPrimitiveConvertInputCppType(
        string inputManagedType,
        out string inputCppType)
    {
        inputCppType = string.Empty;
        switch (inputManagedType)
        {
            case "System.Boolean":
                inputCppType = "bool";
                return true;
            case "System.Byte":
                inputCppType = "CHAOS_IL2CPP_UINT8";
                return true;
            case "System.SByte":
                inputCppType = "CHAOS_IL2CPP_INT8";
                return true;
            case "System.Int16":
                inputCppType = "CHAOS_IL2CPP_INT16";
                return true;
            case "System.UInt16":
                inputCppType = "CHAOS_IL2CPP_UINT16";
                return true;
            case "System.Int32":
                inputCppType = "CHAOS_IL2CPP_INT32";
                return true;
            case "System.UInt32":
                inputCppType = "CHAOS_IL2CPP_UINT32";
                return true;
            case "System.Int64":
                inputCppType = "CHAOS_IL2CPP_INT64";
                return true;
            case "System.UInt64":
                inputCppType = "CHAOS_IL2CPP_UINT64";
                return true;
            case "System.Char":
                inputCppType = "CHAOS_IL2CPP_UINT16";
                return true;
            case "System.Single":
                inputCppType = "float";
                return true;
            case "System.Double":
                inputCppType = "double";
                return true;
            default:
                return false;
        }
    }

    private static bool TryResolveRuntimeSkeletonPrimitiveConvertOutputCppType(
        string outputManagedType,
        out string outputCppType)
    {
        return TryResolveRuntimeSkeletonPrimitiveConvertInputCppType(outputManagedType, out outputCppType);
    }

    private static bool TryResolveRuntimeSkeletonBoxedIConvertibleCharInvalidCastShape(
        TypedIlMethodArtifact method,
        IReadOnlyDictionary<string, TypedIlMethodArtifact> methodsBySubjectId,
        out string inputCppType,
        out string outputCppType,
        out string exceptionTypeSubjectId,
        out string sourceTypeName,
        out string targetTypeName)
    {
        inputCppType = string.Empty;
        outputCppType = string.Empty;
        exceptionTypeSubjectId = string.Empty;
        sourceTypeName = string.Empty;
        targetTypeName = string.Empty;

        if (!string.Equals(method.MethodRole, "static-method", StringComparison.Ordinal) ||
            !string.Equals(method.BodyAvailability, "has-canonical-body", StringComparison.Ordinal) ||
            method.Parameters.Count != 1 ||
            !string.Equals(GetMethodReturnType(method.SubjectId), "System.Char", StringComparison.Ordinal) ||
            !TryResolveRuntimeSkeletonPrimitiveConvertInputCppType(method.Parameters[0].Type, out inputCppType) ||
            !TryResolveRuntimeSkeletonPrimitiveConvertOutputCppType("System.Char", out outputCppType))
        {
            return false;
        }

        IReadOnlyList<TypedIlInstructionArtifact> instructions;
        try
        {
            instructions = GetSingleBlockInstructions(method);
            ValidateRuntimeSkeletonBoxedIConvertibleCharEntryShape(method, instructions);
        }
        catch
        {
            return false;
        }

        var boxedTypeSubjectId = GetRequiredOperandString(instructions[1]);
        var declaredTargetSubjectId = GetRequiredInstructionCallee(instructions[3], method.SubjectId, 3);
        var resolvedTargetSubjectId = TryResolveBoxedInterfaceCallTarget(
            methodsBySubjectId.Values.ToArray(),
            instructions,
            1,
            declaredTargetSubjectId);
        if (string.IsNullOrWhiteSpace(resolvedTargetSubjectId) ||
            !methodsBySubjectId.TryGetValue(resolvedTargetSubjectId, out var targetMethod))
        {
            return TryResolveRuntimeSkeletonBoxedIConvertibleCharInvalidCastFallback(
                method,
                boxedTypeSubjectId,
                out exceptionTypeSubjectId,
                out sourceTypeName,
                out targetTypeName);
        }

        IReadOnlyList<TypedIlInstructionArtifact> targetInstructions;
        try
        {
            targetInstructions = GetSingleBlockInstructions(targetMethod);
            ValidateRuntimeSkeletonBoxedIConvertibleCharInvalidCastTargetShape(targetMethod, targetInstructions);
        }
        catch
        {
            return false;
        }

        if (!string.Equals(GetDeclaringTypeSubjectId(targetMethod.SubjectId), boxedTypeSubjectId, StringComparison.Ordinal))
        {
            return false;
        }

        sourceTypeName = GetRequiredOperandString(targetInstructions[1]);
        targetTypeName = GetRequiredOperandString(targetInstructions[2]);
        if (!string.Equals(targetTypeName, "Char", StringComparison.Ordinal))
        {
            return false;
        }

        var exceptionConstructorSubjectId = GetRequiredInstructionCallee(targetInstructions[4], targetMethod.SubjectId, 4);
        exceptionTypeSubjectId = GetDeclaringTypeSubjectId(exceptionConstructorSubjectId);
        return string.Equals(exceptionTypeSubjectId, "System.Private.CoreLib/System.InvalidCastException", StringComparison.Ordinal);
    }

    private static bool TryResolveRuntimeSkeletonStringCharProviderShape(
        TypedIlMethodArtifact method,
        out string targetMethodSubjectId)
    {
        targetMethodSubjectId = string.Empty;

        if (!string.Equals(method.MethodRole, "static-method", StringComparison.Ordinal) ||
            !string.Equals(method.BodyAvailability, "has-canonical-body", StringComparison.Ordinal) ||
            !string.Equals(GetMethodReturnType(method.SubjectId), "System.Char", StringComparison.Ordinal))
        {
            return false;
        }

        IReadOnlyList<TypedIlInstructionArtifact> instructions;
        try
        {
            instructions = GetSingleBlockInstructions(method);
        }
        catch
        {
            return false;
        }

        if (method.Parameters.Count == 1 &&
            string.Equals(method.Parameters[0].Type, "System.String", StringComparison.Ordinal) &&
            instructions.Count == 4 &&
            string.Equals(instructions[0].Op, "ldarg", StringComparison.Ordinal) &&
            GetRequiredOperandInt(instructions[0]) == 0 &&
            string.Equals(instructions[1].Op, "ldnull", StringComparison.Ordinal) &&
            string.Equals(instructions[2].Op, "call", StringComparison.Ordinal) &&
            string.Equals(instructions[3].Op, "ret", StringComparison.Ordinal))
        {
            targetMethodSubjectId = GetRequiredInstructionCallee(instructions[2], method.SubjectId, 2);
            if (!string.Equals(GetMethodReturnType(targetMethodSubjectId), "System.Char", StringComparison.Ordinal))
            {
                return false;
            }

            var targetParameterTypes = GetMethodParameterTypesFromSubjectId(targetMethodSubjectId);
            return targetParameterTypes.Count == 2 &&
                   string.Equals(targetParameterTypes[0], "System.String", StringComparison.Ordinal) &&
                   string.Equals(targetParameterTypes[1], "System.IFormatProvider", StringComparison.Ordinal) &&
                   (string.Equals(targetMethodSubjectId, RuntimeSkeletonConvertStringCharProviderCoreLibSubjectId, StringComparison.Ordinal) ||
                    !string.IsNullOrWhiteSpace(targetMethodSubjectId));
        }

        if (method.Parameters.Count == 2 &&
            string.Equals(method.Parameters[0].Type, "System.String", StringComparison.Ordinal) &&
            string.Equals(method.Parameters[1].Type, "System.IFormatProvider", StringComparison.Ordinal) &&
            instructions.Count == 14 &&
            string.Equals(instructions[0].Op, "ldarg", StringComparison.Ordinal) &&
            GetRequiredOperandInt(instructions[0]) == 0 &&
            string.Equals(instructions[1].Op, "ldstr", StringComparison.Ordinal) &&
            string.Equals(instructions[2].Op, "call", StringComparison.Ordinal) &&
            string.Equals(instructions[3].Op, "ldarg", StringComparison.Ordinal) &&
            GetRequiredOperandInt(instructions[3]) == 0 &&
            string.Equals(instructions[4].Op, "callvirt", StringComparison.Ordinal) &&
            string.Equals(instructions[5].Op, "ldc.i4", StringComparison.Ordinal) &&
            GetRequiredOperandInt(instructions[5]) == 1 &&
            string.Equals(instructions[6].Op, "beq", StringComparison.Ordinal) &&
            (string.Equals(instructions[7].Op, "call", StringComparison.Ordinal) ||
             string.Equals(instructions[7].Op, "ldstr", StringComparison.Ordinal)) &&
            string.Equals(instructions[8].Op, "newobj", StringComparison.Ordinal) &&
            string.Equals(instructions[9].Op, "throw", StringComparison.Ordinal) &&
            string.Equals(instructions[10].Op, "ldarg", StringComparison.Ordinal) &&
            GetRequiredOperandInt(instructions[10]) == 0 &&
            string.Equals(instructions[11].Op, "ldc.i4", StringComparison.Ordinal) &&
            GetRequiredOperandInt(instructions[11]) == 0 &&
            string.Equals(instructions[12].Op, "callvirt", StringComparison.Ordinal) &&
            string.Equals(instructions[13].Op, "ret", StringComparison.Ordinal))
        {
            RequireInstructionCallee(
                instructions[2],
                "System.Private.CoreLib/System.ArgumentNullException::ThrowIfNull:System.Void(System.Object,System.String)",
                method.SubjectId,
                2);
            RequireInstructionCallee(
                instructions[4],
                "System.Private.CoreLib/System.String::get_Length:System.Int32()",
                method.SubjectId,
                4);
            if (string.Equals(instructions[7].Op, "call", StringComparison.Ordinal))
            {
                RequireInstructionCallee(
                    instructions[7],
                    "System.Private.CoreLib/System.SR::get_Format_NeedSingleChar:System.String()",
                    method.SubjectId,
                    7);
            }
            RequireInstructionCallee(
                instructions[8],
                "System.Private.CoreLib/System.FormatException::.ctor:System.Void(System.String)",
                method.SubjectId,
                8);
            RequireInstructionCallee(
                instructions[12],
                "System.Private.CoreLib/System.String::get_Chars:System.Char(System.Int32)",
                method.SubjectId,
                12);
            targetMethodSubjectId = method.SubjectId;
            return true;
        }

        return false;
    }

    private static bool TryResolveRuntimeSkeletonStringProviderPassthroughShape(
        TypedIlMethodArtifact method)
    {
        if (!string.Equals(method.MethodRole, "static-method", StringComparison.Ordinal) ||
            !string.Equals(method.BodyAvailability, "has-canonical-body", StringComparison.Ordinal) ||
            !string.Equals(GetMethodReturnType(method.SubjectId), "System.String", StringComparison.Ordinal) ||
            method.Parameters.Count != 2 ||
            !string.Equals(method.Parameters[0].Type, "System.String", StringComparison.Ordinal) ||
            !string.Equals(method.Parameters[1].Type, "System.IFormatProvider", StringComparison.Ordinal))
        {
            return false;
        }

        IReadOnlyList<TypedIlInstructionArtifact> instructions;
        try
        {
            instructions = GetSingleBlockInstructions(method);
        }
        catch
        {
            return false;
        }

        return instructions.Count == 2 &&
               string.Equals(instructions[0].Op, "ldarg", StringComparison.Ordinal) &&
               GetRequiredOperandInt(instructions[0]) == 0 &&
               string.Equals(instructions[1].Op, "ret", StringComparison.Ordinal) &&
               (string.Equals(method.SubjectId, RuntimeSkeletonConvertStringProviderPassthroughCoreLibSubjectId, StringComparison.Ordinal) ||
                !string.IsNullOrWhiteSpace(method.SubjectId));
    }

    private static bool TryResolveRuntimeSkeletonObjectCharProviderShape(
        TypedIlMethodArtifact method,
        out string targetMethodSubjectId)
    {
        targetMethodSubjectId = string.Empty;

        if (!string.Equals(method.MethodRole, "static-method", StringComparison.Ordinal) ||
            !string.Equals(method.BodyAvailability, "has-canonical-body", StringComparison.Ordinal) ||
            !string.Equals(GetMethodReturnType(method.SubjectId), "System.Char", StringComparison.Ordinal))
        {
            return false;
        }

        IReadOnlyList<TypedIlInstructionArtifact> instructions;
        try
        {
            instructions = GetSingleBlockInstructions(method);
        }
        catch
        {
            return false;
        }

        if (method.Parameters.Count == 1 &&
            string.Equals(method.Parameters[0].Type, "System.Object", StringComparison.Ordinal))
        {
            if (instructions.Count == 4 &&
                string.Equals(instructions[0].Op, "ldarg", StringComparison.Ordinal) &&
                GetRequiredOperandInt(instructions[0]) == 0 &&
                string.Equals(instructions[1].Op, "ldnull", StringComparison.Ordinal) &&
                string.Equals(instructions[2].Op, "call", StringComparison.Ordinal) &&
                string.Equals(instructions[3].Op, "ret", StringComparison.Ordinal))
            {
                targetMethodSubjectId = GetRequiredInstructionCallee(instructions[2], method.SubjectId, 2);
                var targetParameterTypes = GetMethodParameterTypesFromSubjectId(targetMethodSubjectId);
                return string.Equals(GetMethodReturnType(targetMethodSubjectId), "System.Char", StringComparison.Ordinal) &&
                       targetParameterTypes.Count == 2 &&
                       string.Equals(targetParameterTypes[0], "System.Object", StringComparison.Ordinal) &&
                       string.Equals(targetParameterTypes[1], "System.IFormatProvider", StringComparison.Ordinal);
            }

            if (instructions.Count == 9 &&
                string.Equals(instructions[0].Op, "ldarg", StringComparison.Ordinal) &&
                GetRequiredOperandInt(instructions[0]) == 0 &&
                string.Equals(instructions[1].Op, "brfalse", StringComparison.Ordinal) &&
                string.Equals(instructions[2].Op, "ldarg", StringComparison.Ordinal) &&
                GetRequiredOperandInt(instructions[2]) == 0 &&
                string.Equals(instructions[3].Op, "castclass", StringComparison.Ordinal) &&
                string.Equals(instructions[4].Op, "ldnull", StringComparison.Ordinal) &&
                string.Equals(instructions[5].Op, "callvirt", StringComparison.Ordinal) &&
                string.Equals(instructions[6].Op, "ret", StringComparison.Ordinal) &&
                string.Equals(instructions[7].Op, "ldc.i4", StringComparison.Ordinal) &&
                GetRequiredOperandInt(instructions[7]) == 0 &&
                string.Equals(instructions[8].Op, "ret", StringComparison.Ordinal))
            {
                targetMethodSubjectId = GetRequiredInstructionCallee(instructions[5], method.SubjectId, 5);
                var targetParameterTypes = GetMethodParameterTypesFromSubjectId(targetMethodSubjectId);
                return string.Equals(GetMethodReturnType(targetMethodSubjectId), "System.Char", StringComparison.Ordinal) &&
                       targetParameterTypes.Count == 1 &&
                       string.Equals(targetParameterTypes[0], "System.IFormatProvider", StringComparison.Ordinal);
            }

            if (instructions.Count == 9 &&
                string.Equals(instructions[0].Op, "ldarg", StringComparison.Ordinal) &&
                GetRequiredOperandInt(instructions[0]) == 0 &&
                string.Equals(instructions[1].Op, "brtrue", StringComparison.Ordinal) &&
                string.Equals(instructions[2].Op, "ldc.i4", StringComparison.Ordinal) &&
                GetRequiredOperandInt(instructions[2]) == 0 &&
                string.Equals(instructions[3].Op, "ret", StringComparison.Ordinal) &&
                string.Equals(instructions[4].Op, "ldarg", StringComparison.Ordinal) &&
                GetRequiredOperandInt(instructions[4]) == 0 &&
                string.Equals(instructions[5].Op, "castclass", StringComparison.Ordinal) &&
                string.Equals(instructions[6].Op, "ldnull", StringComparison.Ordinal) &&
                string.Equals(instructions[7].Op, "callvirt", StringComparison.Ordinal) &&
                string.Equals(instructions[8].Op, "ret", StringComparison.Ordinal))
            {
                targetMethodSubjectId = GetRequiredInstructionCallee(instructions[7], method.SubjectId, 7);
                var targetParameterTypes = GetMethodParameterTypesFromSubjectId(targetMethodSubjectId);
                return string.Equals(GetMethodReturnType(targetMethodSubjectId), "System.Char", StringComparison.Ordinal) &&
                       targetParameterTypes.Count == 1 &&
                       string.Equals(targetParameterTypes[0], "System.IFormatProvider", StringComparison.Ordinal);
            }
        }

        if (method.Parameters.Count == 2 &&
            string.Equals(method.Parameters[0].Type, "System.Object", StringComparison.Ordinal) &&
            string.Equals(method.Parameters[1].Type, "System.IFormatProvider", StringComparison.Ordinal) &&
            instructions.Count == 9 &&
            string.Equals(instructions[0].Op, "ldarg", StringComparison.Ordinal) &&
            GetRequiredOperandInt(instructions[0]) == 0)
        {
            if (string.Equals(instructions[1].Op, "brfalse", StringComparison.Ordinal) &&
                string.Equals(instructions[2].Op, "ldarg", StringComparison.Ordinal) &&
                GetRequiredOperandInt(instructions[2]) == 0 &&
                string.Equals(instructions[3].Op, "castclass", StringComparison.Ordinal) &&
                string.Equals(instructions[4].Op, "ldarg", StringComparison.Ordinal) &&
                GetRequiredOperandInt(instructions[4]) == 1 &&
                string.Equals(instructions[5].Op, "callvirt", StringComparison.Ordinal) &&
                string.Equals(instructions[6].Op, "ret", StringComparison.Ordinal) &&
                string.Equals(instructions[7].Op, "ldc.i4", StringComparison.Ordinal) &&
                GetRequiredOperandInt(instructions[7]) == 0 &&
                string.Equals(instructions[8].Op, "ret", StringComparison.Ordinal))
            {
                targetMethodSubjectId = GetRequiredInstructionCallee(instructions[5], method.SubjectId, 5);
                var targetParameterTypes = GetMethodParameterTypesFromSubjectId(targetMethodSubjectId);
                return string.Equals(GetMethodReturnType(targetMethodSubjectId), "System.Char", StringComparison.Ordinal) &&
                       targetParameterTypes.Count == 1 &&
                       string.Equals(targetParameterTypes[0], "System.IFormatProvider", StringComparison.Ordinal);
            }

            if (string.Equals(instructions[1].Op, "brtrue", StringComparison.Ordinal) &&
                string.Equals(instructions[2].Op, "ldc.i4", StringComparison.Ordinal) &&
                GetRequiredOperandInt(instructions[2]) == 0 &&
                string.Equals(instructions[3].Op, "ret", StringComparison.Ordinal) &&
                string.Equals(instructions[4].Op, "ldarg", StringComparison.Ordinal) &&
                GetRequiredOperandInt(instructions[4]) == 0 &&
                string.Equals(instructions[5].Op, "castclass", StringComparison.Ordinal) &&
                string.Equals(instructions[6].Op, "ldarg", StringComparison.Ordinal) &&
                GetRequiredOperandInt(instructions[6]) == 1 &&
                string.Equals(instructions[7].Op, "callvirt", StringComparison.Ordinal) &&
                string.Equals(instructions[8].Op, "ret", StringComparison.Ordinal))
            {
                targetMethodSubjectId = GetRequiredInstructionCallee(instructions[7], method.SubjectId, 7);
                var targetParameterTypes = GetMethodParameterTypesFromSubjectId(targetMethodSubjectId);
                return string.Equals(GetMethodReturnType(targetMethodSubjectId), "System.Char", StringComparison.Ordinal) &&
                       targetParameterTypes.Count == 1 &&
                       string.Equals(targetParameterTypes[0], "System.IFormatProvider", StringComparison.Ordinal);
            }
        }

        return false;
    }

    private static bool TryResolveRuntimeSkeletonBoxedValueTypeStringInstanceCallShape(
        TypedIlMethodArtifact method,
        IReadOnlyDictionary<string, TypedIlMethodArtifact> methodsBySubjectId,
        out string inputCppType,
        out string boxedValueTypeSubjectId,
        out string targetMethodSubjectId,
        out string targetAssemblyName,
        out int inputSize,
        out int argCount)
    {
        inputCppType = string.Empty;
        boxedValueTypeSubjectId = string.Empty;
        targetMethodSubjectId = string.Empty;
        targetAssemblyName = string.Empty;
        inputSize = 0;
        argCount = 0;

        if (!string.Equals(method.MethodRole, "static-method", StringComparison.Ordinal) ||
            !string.Equals(method.BodyAvailability, "has-canonical-body", StringComparison.Ordinal) ||
            !string.Equals(GetMethodReturnType(method.SubjectId), "System.String", StringComparison.Ordinal) ||
            method.Parameters.Count < 1 ||
            method.Parameters.Count > 2 ||
            !TryResolveRuntimeSkeletonValueTypeByValueInputCppType(method.Parameters[0].Type, out inputCppType, out inputSize))
        {
            return false;
        }

        if (method.Parameters.Count == 2 &&
            !string.Equals(method.Parameters[1].Type, "System.IFormatProvider", StringComparison.Ordinal))
        {
            return false;
        }

        IReadOnlyList<TypedIlInstructionArtifact> instructions;
        try
        {
            instructions = GetSingleBlockInstructions(method);
        }
        catch
        {
            return false;
        }

        if (method.Parameters.Count == 1)
        {
            if (instructions.Count != 3 ||
                !string.Equals(instructions[0].Op, "ldarga", StringComparison.Ordinal) ||
                GetRequiredOperandInt(instructions[0]) != 0 ||
                !string.Equals(instructions[1].Op, "call", StringComparison.Ordinal) ||
                !string.Equals(instructions[2].Op, "ret", StringComparison.Ordinal))
            {
                return false;
            }

            argCount = 0;
        }
        else
        {
            if (instructions.Count != 4 ||
                !string.Equals(instructions[0].Op, "ldarga", StringComparison.Ordinal) ||
                GetRequiredOperandInt(instructions[0]) != 0 ||
                !string.Equals(instructions[1].Op, "ldarg", StringComparison.Ordinal) ||
                GetRequiredOperandInt(instructions[1]) != 1 ||
                !string.Equals(instructions[2].Op, "call", StringComparison.Ordinal) ||
                !string.Equals(instructions[3].Op, "ret", StringComparison.Ordinal))
            {
                return false;
            }

            argCount = 1;
        }

        boxedValueTypeSubjectId = $"System.Private.CoreLib/{method.Parameters[0].Type}";
        targetMethodSubjectId = GetRequiredInstructionCallee(
            instructions[argCount == 0 ? 1 : 2],
            method.SubjectId,
            argCount == 0 ? 1 : 2);
        targetAssemblyName = GetAssemblyNameFromSubjectId(targetMethodSubjectId);

        if (!string.Equals(GetMethodReturnType(targetMethodSubjectId), "System.String", StringComparison.Ordinal) ||
            !string.Equals(GetDeclaringTypeSubjectId(targetMethodSubjectId), boxedValueTypeSubjectId, StringComparison.Ordinal))
        {
            return false;
        }

        var targetParameterTypes = GetMethodParameterTypesFromSubjectId(targetMethodSubjectId);
        if (argCount == 0)
        {
            if (targetParameterTypes.Count != 0)
            {
                return false;
            }
        }
        else if (targetParameterTypes.Count != 1 ||
                 !string.Equals(targetParameterTypes[0], "System.IFormatProvider", StringComparison.Ordinal))
        {
            return false;
        }

        if (methodsBySubjectId.TryGetValue(targetMethodSubjectId, out var targetMethod))
        {
            return string.Equals(targetMethod.MethodRole, "instance-method", StringComparison.Ordinal) &&
                   string.Equals(targetMethod.BodyAvailability, "has-canonical-body", StringComparison.Ordinal);
        }

        return true;
    }

    private static bool TryResolveRuntimeSkeletonBoxedValueTypeInstanceCallShape(
        TypedIlMethodArtifact method,
        IReadOnlyDictionary<string, TypedIlMethodArtifact> methodsBySubjectId,
        string outputManagedType,
        out string inputCppType,
        out string outputCppType,
        out string boxedValueTypeSubjectId,
        out string targetMethodSubjectId,
        out string targetAssemblyName,
        out int inputSize,
        out int argCount)
    {
        inputCppType = string.Empty;
        outputCppType = string.Empty;
        boxedValueTypeSubjectId = string.Empty;
        targetMethodSubjectId = string.Empty;
        targetAssemblyName = string.Empty;
        inputSize = 0;
        argCount = 0;

        if (!string.Equals(method.MethodRole, "static-method", StringComparison.Ordinal) ||
            !string.Equals(method.BodyAvailability, "has-canonical-body", StringComparison.Ordinal) ||
            !string.Equals(GetMethodReturnType(method.SubjectId), outputManagedType, StringComparison.Ordinal) ||
            method.Parameters.Count < 1 ||
            method.Parameters.Count > 2 ||
            !TryResolveRuntimeSkeletonValueTypeByValueInputCppType(method.Parameters[0].Type, out inputCppType, out inputSize) ||
            !TryResolveRuntimeSkeletonValueTypeByValueInputCppType(outputManagedType, out outputCppType, out _))
        {
            return false;
        }

        if (method.Parameters.Count == 2 &&
            !string.Equals(method.Parameters[1].Type, "System.IFormatProvider", StringComparison.Ordinal))
        {
            return false;
        }

        IReadOnlyList<TypedIlInstructionArtifact> instructions;
        try
        {
            instructions = GetSingleBlockInstructions(method);
        }
        catch
        {
            return false;
        }

        int callInstructionIndex;
        if (method.Parameters.Count == 1)
        {
            if (instructions.Count == 3 &&
                string.Equals(instructions[0].Op, "ldarga", StringComparison.Ordinal) &&
                GetRequiredOperandInt(instructions[0]) == 0 &&
                (string.Equals(instructions[1].Op, "call", StringComparison.Ordinal) ||
                 string.Equals(instructions[1].Op, "callvirt", StringComparison.Ordinal)) &&
                string.Equals(instructions[2].Op, "ret", StringComparison.Ordinal))
            {
                callInstructionIndex = 1;
            }
            else if (instructions.Count == 4 &&
                string.Equals(instructions[0].Op, "ldarga", StringComparison.Ordinal) &&
                GetRequiredOperandInt(instructions[0]) == 0 &&
                string.Equals(instructions[1].Op, "ldnull", StringComparison.Ordinal) &&
                (string.Equals(instructions[2].Op, "call", StringComparison.Ordinal) ||
                 string.Equals(instructions[2].Op, "callvirt", StringComparison.Ordinal)) &&
                string.Equals(instructions[3].Op, "ret", StringComparison.Ordinal))
            {
                callInstructionIndex = 2;
            }
            else
            {
                return false;
            }

            argCount = 0;
        }
        else
        {
            if (instructions.Count != 4 ||
                !string.Equals(instructions[0].Op, "ldarga", StringComparison.Ordinal) ||
                GetRequiredOperandInt(instructions[0]) != 0 ||
                !string.Equals(instructions[1].Op, "ldarg", StringComparison.Ordinal) ||
                GetRequiredOperandInt(instructions[1]) != 1 ||
                !(string.Equals(instructions[2].Op, "call", StringComparison.Ordinal) ||
                  string.Equals(instructions[2].Op, "callvirt", StringComparison.Ordinal)) ||
                !string.Equals(instructions[3].Op, "ret", StringComparison.Ordinal))
            {
                return false;
            }

            callInstructionIndex = 2;
            argCount = 1;
        }

        boxedValueTypeSubjectId = $"System.Private.CoreLib/{method.Parameters[0].Type}";
        targetMethodSubjectId = GetRequiredInstructionCallee(
            instructions[callInstructionIndex],
            method.SubjectId,
            callInstructionIndex);
        targetAssemblyName = GetAssemblyNameFromSubjectId(targetMethodSubjectId);

        if (!string.Equals(GetMethodReturnType(targetMethodSubjectId), outputManagedType, StringComparison.Ordinal))
        {
            return false;
        }

        var targetParameterTypes = GetMethodParameterTypesFromSubjectId(targetMethodSubjectId);
        if (argCount == 0)
        {
            if (targetParameterTypes.Count != 0 &&
                (targetParameterTypes.Count != 1 ||
                 !string.Equals(targetParameterTypes[0], "System.IFormatProvider", StringComparison.Ordinal)))
            {
                return false;
            }
        }
        else if (targetParameterTypes.Count != 1 ||
                 !string.Equals(targetParameterTypes[0], "System.IFormatProvider", StringComparison.Ordinal))
        {
            return false;
        }

        if (methodsBySubjectId.TryGetValue(targetMethodSubjectId, out var targetMethod) &&
            !string.Equals(targetMethod.MethodRole, "instance-method", StringComparison.Ordinal))
        {
            return false;
        }

        return true;
    }

    private static bool TryResolveRuntimeSkeletonStaticValueTypeCallShape(
        TypedIlMethodArtifact method,
        string outputManagedType,
        out string targetMethodSubjectId,
        out string targetAssemblyName,
        out List<string> argFieldDeclarations,
        out List<string> argRefs,
        out int argCount)
    {
        targetMethodSubjectId = string.Empty;
        targetAssemblyName = string.Empty;
        argFieldDeclarations = new List<string>();
        argRefs = new List<string>();
        argCount = 0;

        if (!string.Equals(method.MethodRole, "static-method", StringComparison.Ordinal) ||
            !string.Equals(method.BodyAvailability, "has-canonical-body", StringComparison.Ordinal) ||
            !string.Equals(GetMethodReturnType(method.SubjectId), outputManagedType, StringComparison.Ordinal) ||
            method.Parameters.Count < 1 ||
            method.Parameters.Count > 2)
        {
            return false;
        }

        IReadOnlyList<TypedIlInstructionArtifact> instructions;
        try
        {
            instructions = GetSingleBlockInstructions(method);
        }
        catch
        {
            return false;
        }

        int callIndex;
        if (method.Parameters.Count == 1)
        {
            if (instructions.Count != 3 ||
                !string.Equals(instructions[0].Op, "ldarg", StringComparison.Ordinal) ||
                GetRequiredOperandInt(instructions[0]) != 0 ||
                !string.Equals(instructions[1].Op, "call", StringComparison.Ordinal) ||
                !string.Equals(instructions[2].Op, "ret", StringComparison.Ordinal))
            {
                return false;
            }

            callIndex = 1;
        }
        else
        {
            if (instructions.Count != 4 ||
                !string.Equals(instructions[0].Op, "ldarg", StringComparison.Ordinal) ||
                GetRequiredOperandInt(instructions[0]) != 0 ||
                !string.Equals(instructions[1].Op, "ldarg", StringComparison.Ordinal) ||
                GetRequiredOperandInt(instructions[1]) != 1 ||
                !string.Equals(instructions[2].Op, "call", StringComparison.Ordinal) ||
                !string.Equals(instructions[3].Op, "ret", StringComparison.Ordinal))
            {
                return false;
            }

            callIndex = 2;
        }

        targetMethodSubjectId = GetRequiredInstructionCallee(
            instructions[callIndex], method.SubjectId, callIndex);
        if (string.Equals(targetMethodSubjectId, method.SubjectId, StringComparison.Ordinal))
        {
            return false;
        }

        if (!string.Equals(GetMethodReturnType(targetMethodSubjectId), outputManagedType, StringComparison.Ordinal))
        {
            return false;
        }

        targetAssemblyName = GetAssemblyNameFromSubjectId(targetMethodSubjectId);

        // Build arg field declarations and refs for each parameter
        for (var i = 0; i < method.Parameters.Count; i++)
        {
            var parameter = method.Parameters[i];
            if (TryResolveRuntimeSkeletonValueTypeByValueInputCppType(parameter.Type, out var cppType, out _))
            {
                argFieldDeclarations.Add($"{cppType} arg{i + 1};");
            }
            else
            {
                argFieldDeclarations.Add($"void* arg{i + 1};");
            }

            argRefs.Add($"const_cast<void*>(reinterpret_cast<const void*>(&request->arg{i + 1}))");
        }

        argCount = method.Parameters.Count;
        return true;
    }

    private static bool TryResolveRuntimeSkeletonInstanceValueTypeCallShape(
        TypedIlMethodArtifact method,
        string outputManagedType,
        out string targetMethodSubjectId,
        out string targetAssemblyName,
        out bool hasNullArg,
        out List<string> argFieldDeclarations,
        out List<string> argRefs,
        out int argCount)
    {
        targetMethodSubjectId = string.Empty;
        targetAssemblyName = string.Empty;
        hasNullArg = false;
        argFieldDeclarations = new List<string>();
        argRefs = new List<string>();
        argCount = 0;

        if (!string.Equals(method.MethodRole, "static-method", StringComparison.Ordinal) ||
            !string.Equals(method.BodyAvailability, "has-canonical-body", StringComparison.Ordinal) ||
            !string.Equals(GetMethodReturnType(method.SubjectId), outputManagedType, StringComparison.Ordinal) ||
            method.Parameters.Count < 1 ||
            method.Parameters.Count > 2)
        {
            return false;
        }

        IReadOnlyList<TypedIlInstructionArtifact> instructions;
        try
        {
            instructions = GetSingleBlockInstructions(method);
        }
        catch
        {
            return false;
        }

        // Pattern: ldarg.0, [ldnull or ldarg.1], callvirt, ret
        if (method.Parameters.Count == 1)
        {
            // ToDateTime(object value) → ((IConvertible)value).ToDateTime(null)
            // IL: ldarg.0, ldnull, callvirt IConvertible::ToDateTime, ret
            if (instructions.Count != 4 ||
                !string.Equals(instructions[0].Op, "ldarg", StringComparison.Ordinal) ||
                GetRequiredOperandInt(instructions[0]) != 0 ||
                !string.Equals(instructions[1].Op, "ldnull", StringComparison.Ordinal) ||
                !string.Equals(instructions[2].Op, "callvirt", StringComparison.Ordinal) ||
                !string.Equals(instructions[3].Op, "ret", StringComparison.Ordinal))
            {
                return false;
            }

            hasNullArg = true;
            argFieldDeclarations.Add("void* cdecl_this;");
            argRefs.Add("reinterpret_cast<void*>(nullptr)");
            argCount = 1;
            targetMethodSubjectId = GetRequiredInstructionCallee(instructions[2], method.SubjectId, 2);
        }
        else
        {
            // ToDateTime(object value, IFormatProvider provider) → ((IConvertible)value).ToDateTime(provider)
            // IL: ldarg.0, ldarg.1, callvirt IConvertible::ToDateTime, ret
            if (instructions.Count != 4 ||
                !string.Equals(instructions[0].Op, "ldarg", StringComparison.Ordinal) ||
                GetRequiredOperandInt(instructions[0]) != 0 ||
                !string.Equals(instructions[1].Op, "ldarg", StringComparison.Ordinal) ||
                GetRequiredOperandInt(instructions[1]) != 1 ||
                !string.Equals(instructions[2].Op, "callvirt", StringComparison.Ordinal) ||
                !string.Equals(instructions[3].Op, "ret", StringComparison.Ordinal))
            {
                return false;
            }

            hasNullArg = false;
            argFieldDeclarations.Add("void* arg1;");
            argFieldDeclarations.Add("void* cdecl_this;");
            argRefs.Add("const_cast<void*>(reinterpret_cast<const void*>(&request->arg1))");
            argCount = 1;
            targetMethodSubjectId = GetRequiredInstructionCallee(instructions[2], method.SubjectId, 2);
        }

        if (string.Equals(targetMethodSubjectId, method.SubjectId, StringComparison.Ordinal))
        {
            return false;
        }

        if (!string.Equals(GetMethodReturnType(targetMethodSubjectId), outputManagedType, StringComparison.Ordinal))
        {
            return false;
        }

        targetAssemblyName = GetAssemblyNameFromSubjectId(targetMethodSubjectId);
        return true;
    }

    private static bool TryResolveRuntimeSkeletonBoxedIConvertibleCharInvalidCastFallback(
        TypedIlMethodArtifact method,
        string boxedTypeSubjectId,
        out string exceptionTypeSubjectId,
        out string sourceTypeName,
        out string targetTypeName)
    {
        exceptionTypeSubjectId = string.Empty;
        sourceTypeName = string.Empty;
        targetTypeName = string.Empty;

        if (!TryResolveRuntimeSkeletonBoxedIConvertibleCharFallbackSourceTypeName(
                method.Parameters[0].Type,
                boxedTypeSubjectId,
                out sourceTypeName))
        {
            return false;
        }

        exceptionTypeSubjectId = "System.Private.CoreLib/System.InvalidCastException";
        targetTypeName = "Char";
        return true;
    }

    private static bool TryResolveRuntimeSkeletonBoxedIConvertibleCharFallbackSourceTypeName(
        string methodParameterType,
        string boxedTypeSubjectId,
        out string sourceTypeName)
    {
        sourceTypeName = string.Empty;
        if (!string.Equals(methodParameterType, "System.Boolean", StringComparison.Ordinal) &&
            !string.Equals(methodParameterType, "System.Single", StringComparison.Ordinal) &&
            !string.Equals(methodParameterType, "System.Double", StringComparison.Ordinal))
        {
            return false;
        }

        var expectedBoxedTypeSubjectId = $"System.Private.CoreLib/{methodParameterType}";
        if (!string.Equals(boxedTypeSubjectId, expectedBoxedTypeSubjectId, StringComparison.Ordinal))
        {
            return false;
        }

        sourceTypeName = GetTypeDisplayName(methodParameterType);
        return true;
    }

    private static bool TryResolveRuntimeSkeletonBoxedValueTypeCharInvalidCastShape(
        TypedIlMethodArtifact method,
        IReadOnlyDictionary<string, TypedIlMethodArtifact> methodsBySubjectId,
        out string inputCppType,
        out string outputCppType,
        out string boxedValueTypeSubjectId,
        out string exceptionTypeSubjectId,
        out string sourceTypeName,
        out string targetTypeName,
        out int inputSize)
    {
        inputCppType = string.Empty;
        outputCppType = string.Empty;
        boxedValueTypeSubjectId = string.Empty;
        exceptionTypeSubjectId = string.Empty;
        sourceTypeName = string.Empty;
        targetTypeName = string.Empty;
        inputSize = 0;

        if (!string.Equals(method.MethodRole, "static-method", StringComparison.Ordinal) ||
            !string.Equals(method.BodyAvailability, "has-canonical-body", StringComparison.Ordinal) ||
            method.Parameters.Count != 1 ||
            !string.Equals(GetMethodReturnType(method.SubjectId), "System.Char", StringComparison.Ordinal) ||
            !TryResolveRuntimeSkeletonValueTypeByValueInputCppType(method.Parameters[0].Type, out inputCppType, out inputSize) ||
            !TryResolveRuntimeSkeletonPrimitiveConvertOutputCppType("System.Char", out outputCppType))
        {
            return false;
        }

        IReadOnlyList<TypedIlInstructionArtifact> instructions;
        try
        {
            instructions = GetSingleBlockInstructions(method);
        }
        catch
        {
            return false;
        }

        if (instructions.Count != 5 ||
            !string.Equals(instructions[0].Op, "ldarg", StringComparison.Ordinal) ||
            GetRequiredOperandInt(instructions[0]) != 0 ||
            !string.Equals(instructions[1].Op, "box", StringComparison.Ordinal) ||
            !string.Equals(instructions[2].Op, "ldnull", StringComparison.Ordinal) ||
            !string.Equals(instructions[3].Op, "callvirt", StringComparison.Ordinal) ||
            !string.Equals(instructions[4].Op, "ret", StringComparison.Ordinal))
        {
            return false;
        }

        boxedValueTypeSubjectId = GetRequiredOperandString(instructions[1]);
        var declaredTargetSubjectId = GetRequiredInstructionCallee(instructions[3], method.SubjectId, 3);
        var resolvedTargetSubjectId = TryResolveBoxedInterfaceCallTarget(
            methodsBySubjectId.Values.ToArray(),
            instructions,
            1,
            declaredTargetSubjectId);
        if (string.IsNullOrWhiteSpace(resolvedTargetSubjectId) ||
            !methodsBySubjectId.TryGetValue(resolvedTargetSubjectId, out var targetMethod))
        {
            return TryResolveRuntimeSkeletonBoxedValueTypeCharInvalidCastFallback(
                method,
                boxedValueTypeSubjectId,
                out exceptionTypeSubjectId,
                out sourceTypeName,
                out targetTypeName);
        }

        IReadOnlyList<TypedIlInstructionArtifact> targetInstructions;
        try
        {
            targetInstructions = GetSingleBlockInstructions(targetMethod);
        }
        catch
        {
            return false;
        }

        return TryResolveRuntimeSkeletonBoxedValueTypeCharInvalidCastTargetShape(
            targetMethod,
            targetInstructions,
            out exceptionTypeSubjectId,
            out sourceTypeName,
            out targetTypeName);
    }

    private static bool TryResolveRuntimeSkeletonBoxedValueTypeCharInvalidCastFallback(
        TypedIlMethodArtifact method,
        string boxedValueTypeSubjectId,
        out string exceptionTypeSubjectId,
        out string sourceTypeName,
        out string targetTypeName)
    {
        exceptionTypeSubjectId = string.Empty;
        sourceTypeName = string.Empty;
        targetTypeName = string.Empty;

        if (!TryResolveRuntimeSkeletonBoxedValueTypeCharFallbackSourceTypeName(
                method.Parameters[0].Type,
                boxedValueTypeSubjectId,
                out sourceTypeName))
        {
            return false;
        }

        exceptionTypeSubjectId = "System.Private.CoreLib/System.InvalidCastException";
        targetTypeName = "Char";
        return true;
    }

    private static bool TryResolveRuntimeSkeletonBoxedValueTypeCharFallbackSourceTypeName(
        string methodParameterType,
        string boxedValueTypeSubjectId,
        out string sourceTypeName)
    {
        sourceTypeName = string.Empty;
        if (!string.Equals(methodParameterType, "System.Decimal", StringComparison.Ordinal) &&
            !string.Equals(methodParameterType, "System.DateTime", StringComparison.Ordinal))
        {
            return false;
        }

        var expectedBoxedTypeSubjectId = $"System.Private.CoreLib/{methodParameterType}";
        if (!string.Equals(boxedValueTypeSubjectId, expectedBoxedTypeSubjectId, StringComparison.Ordinal))
        {
            return false;
        }

        sourceTypeName = GetTypeDisplayName(methodParameterType);
        return true;
    }

    private static bool TryResolveRuntimeSkeletonBoxedValueTypeCharInvalidCastTargetShape(
        TypedIlMethodArtifact method,
        IReadOnlyList<TypedIlInstructionArtifact> instructions,
        out string exceptionTypeSubjectId,
        out string sourceTypeName,
        out string targetTypeName)
    {
        exceptionTypeSubjectId = string.Empty;
        sourceTypeName = string.Empty;
        targetTypeName = string.Empty;

        if (!string.Equals(method.MethodRole, "instance-method", StringComparison.Ordinal) ||
            !string.Equals(method.BodyAvailability, "has-canonical-body", StringComparison.Ordinal) ||
            !string.Equals(GetMethodReturnType(method.SubjectId), "System.Char", StringComparison.Ordinal) ||
            method.Parameters.Count != 1 ||
            !string.Equals(method.Parameters[0].Type, "System.IFormatProvider", StringComparison.Ordinal))
        {
            return false;
        }

        if (instructions.Count == 6 &&
            string.Equals(instructions[0].Op, "call", StringComparison.Ordinal) &&
            string.Equals(instructions[1].Op, "ldstr", StringComparison.Ordinal) &&
            string.Equals(instructions[2].Op, "ldstr", StringComparison.Ordinal) &&
            string.Equals(instructions[3].Op, "call", StringComparison.Ordinal) &&
            string.Equals(instructions[4].Op, "newobj", StringComparison.Ordinal) &&
            string.Equals(instructions[5].Op, "throw", StringComparison.Ordinal))
        {
            sourceTypeName = GetRequiredOperandString(instructions[1]);
            targetTypeName = GetRequiredOperandString(instructions[2]);
            var exceptionConstructorSubjectId = GetRequiredInstructionCallee(instructions[4], method.SubjectId, 4);
            exceptionTypeSubjectId = GetDeclaringTypeSubjectId(exceptionConstructorSubjectId);
            return string.Equals(exceptionTypeSubjectId, "System.Private.CoreLib/System.InvalidCastException", StringComparison.Ordinal) &&
                   string.Equals(targetTypeName, "Char", StringComparison.Ordinal);
        }

        if (instructions.Count == 3 &&
            string.Equals(instructions[0].Op, "ldstr", StringComparison.Ordinal) &&
            string.Equals(instructions[1].Op, "call", StringComparison.Ordinal) &&
            string.Equals(instructions[2].Op, "throw", StringComparison.Ordinal))
        {
            targetTypeName = GetRequiredOperandString(instructions[0]);
            sourceTypeName = GetTypeDisplayName(GetDeclaringTypeSubjectId(method.SubjectId));
            exceptionTypeSubjectId = "System.Private.CoreLib/System.InvalidCastException";
            return string.Equals(
                       GetRequiredInstructionCallee(instructions[1], method.SubjectId, 1),
                       "System.Private.CoreLib/System.DateTime::InvalidCast:System.InvalidCastException(System.String)",
                       StringComparison.Ordinal) &&
                   string.Equals(targetTypeName, "Char", StringComparison.Ordinal);
        }

        return false;
    }

    private static bool TryResolveRuntimeSkeletonValueTypeByValueInputCppType(
        string inputManagedType,
        out string inputCppType,
        out int inputSize)
    {
        inputCppType = string.Empty;
        inputSize = 0;
        switch (inputManagedType)
        {
            case "System.Boolean":
                inputCppType = "bool";
                inputSize = 1;
                return true;
            case "System.Byte":
                inputCppType = "CHAOS_IL2CPP_UINT8";
                inputSize = 1;
                return true;
            case "System.SByte":
                inputCppType = "CHAOS_IL2CPP_INT8";
                inputSize = 1;
                return true;
            case "System.Int16":
                inputCppType = "CHAOS_IL2CPP_INT16";
                inputSize = 2;
                return true;
            case "System.UInt16":
            case "System.Char":
                inputCppType = "CHAOS_IL2CPP_UINT16";
                inputSize = 2;
                return true;
            case "System.Int32":
                inputCppType = "CHAOS_IL2CPP_INT32";
                inputSize = 4;
                return true;
            case "System.UInt32":
                inputCppType = "CHAOS_IL2CPP_UINT32";
                inputSize = 4;
                return true;
            case "System.Int64":
                inputCppType = "CHAOS_IL2CPP_INT64";
                inputSize = 8;
                return true;
            case "System.UInt64":
                inputCppType = "CHAOS_IL2CPP_UINT64";
                inputSize = 8;
                return true;
            case "System.Single":
                inputCppType = "float";
                inputSize = 4;
                return true;
            case "System.Double":
                inputCppType = "double";
                inputSize = 8;
                return true;
            case "System.Decimal":
                inputCppType = "struct { CHAOS_IL2CPP_UINT32 flags; CHAOS_IL2CPP_UINT64 lo64; CHAOS_IL2CPP_UINT32 hi32; }";
                inputSize = 16;
                return true;
            case "System.DateTime":
                inputCppType = "CHAOS_IL2CPP_UINT64";
                inputSize = 8;
                return true;
            default:
                return false;
        }
    }

    private static string GetTypeDisplayName(string managedTypeOrSubjectId)
    {
        var separatorIndex = managedTypeOrSubjectId.LastIndexOfAny(['/', '.']);
        return separatorIndex >= 0 && separatorIndex < managedTypeOrSubjectId.Length - 1
            ? managedTypeOrSubjectId[(separatorIndex + 1)..]
            : managedTypeOrSubjectId;
    }

    private static bool IsRuntimeSkeletonDirectPrimitiveValuePreservingReturn(
        string inputManagedType,
        string outputManagedType)
    {
        if (string.Equals(inputManagedType, outputManagedType, StringComparison.Ordinal))
        {
            return true;
        }

        if ((string.Equals(inputManagedType, "System.UInt16", StringComparison.Ordinal) &&
             string.Equals(outputManagedType, "System.Char", StringComparison.Ordinal)) ||
            (string.Equals(inputManagedType, "System.Char", StringComparison.Ordinal) &&
             string.Equals(outputManagedType, "System.UInt16", StringComparison.Ordinal)))
        {
            return true;
        }

        return string.Equals(inputManagedType, "System.Byte", StringComparison.Ordinal) &&
               (string.Equals(outputManagedType, "System.Char", StringComparison.Ordinal) ||
                string.Equals(outputManagedType, "System.UInt16", StringComparison.Ordinal) ||
                string.Equals(outputManagedType, "System.Int16", StringComparison.Ordinal) ||
                string.Equals(outputManagedType, "System.Int32", StringComparison.Ordinal) ||
                string.Equals(outputManagedType, "System.UInt32", StringComparison.Ordinal) ||
                string.Equals(outputManagedType, "System.Int64", StringComparison.Ordinal) ||
                string.Equals(outputManagedType, "System.UInt64", StringComparison.Ordinal) ||
                string.Equals(outputManagedType, "System.Single", StringComparison.Ordinal) ||
                string.Equals(outputManagedType, "System.Double", StringComparison.Ordinal));
    }

    private static bool TryMatchRuntimeSkeletonPrimitiveConvRetInstructionShape(
        IReadOnlyList<TypedIlInstructionArtifact> instructions,
        string inputManagedType,
        string outputManagedType)
    {
        if (!TryResolveRuntimeSkeletonPrimitiveConvertInputCppType(inputManagedType, out _) ||
            !TryResolveRuntimeSkeletonPrimitiveConvertOutputCppType(outputManagedType, out _))
        {
            return false;
        }

        if (instructions.Count == 3 &&
            string.Equals(instructions[0].Op, "ldarg", StringComparison.Ordinal) &&
            GetRequiredOperandInt(instructions[0]) == 0 &&
            string.Equals(instructions[2].Op, "ret", StringComparison.Ordinal))
        {
            return IsExpectedPrimitiveConvOp(instructions[1].Op, inputManagedType, outputManagedType);
        }

        return instructions.Count == 4 &&
               string.Equals(instructions[0].Op, "ldarg", StringComparison.Ordinal) &&
               GetRequiredOperandInt(instructions[0]) == 0 &&
               string.Equals(instructions[1].Op, "conv.r.un", StringComparison.Ordinal) &&
               string.Equals(instructions[3].Op, "ret", StringComparison.Ordinal) &&
               IsExpectedPrimitiveConvOp(instructions[2].Op, inputManagedType, outputManagedType) &&
               (string.Equals(inputManagedType, "System.UInt32", StringComparison.Ordinal) ||
                string.Equals(inputManagedType, "System.UInt64", StringComparison.Ordinal)) &&
               (string.Equals(outputManagedType, "System.Single", StringComparison.Ordinal) ||
                string.Equals(outputManagedType, "System.Double", StringComparison.Ordinal));
    }

    private static bool IsExpectedPrimitiveConvOp(
        string op,
        string inputManagedType,
        string outputManagedType)
    {
        if (string.Equals(inputManagedType, outputManagedType, StringComparison.Ordinal))
        {
            return false;
        }

        if (string.Equals(outputManagedType, "System.Byte", StringComparison.Ordinal))
        {
            return string.Equals(op, "conv.u1", StringComparison.Ordinal);
        }

        if (string.Equals(outputManagedType, "System.SByte", StringComparison.Ordinal))
        {
            return string.Equals(op, "conv.i1", StringComparison.Ordinal);
        }

        if (string.Equals(outputManagedType, "System.Int16", StringComparison.Ordinal))
        {
            return string.Equals(op, "conv.i2", StringComparison.Ordinal);
        }

        if (string.Equals(outputManagedType, "System.UInt16", StringComparison.Ordinal) ||
            string.Equals(outputManagedType, "System.Char", StringComparison.Ordinal))
        {
            return string.Equals(op, "conv.u2", StringComparison.Ordinal);
        }

        if (string.Equals(outputManagedType, "System.Int32", StringComparison.Ordinal) ||
            string.Equals(outputManagedType, "System.UInt32", StringComparison.Ordinal))
        {
            return false;
        }

        if (string.Equals(outputManagedType, "System.Int64", StringComparison.Ordinal))
        {
            return string.Equals(op, "conv.i8", StringComparison.Ordinal) ||
                   string.Equals(op, "conv.u8", StringComparison.Ordinal);
        }

        if (string.Equals(outputManagedType, "System.UInt64", StringComparison.Ordinal))
        {
            return string.Equals(op, "conv.i8", StringComparison.Ordinal) ||
                   string.Equals(op, "conv.u8", StringComparison.Ordinal);
        }

        if (string.Equals(outputManagedType, "System.Single", StringComparison.Ordinal))
        {
            return string.Equals(op, "conv.r4", StringComparison.Ordinal);
        }

        if (string.Equals(outputManagedType, "System.Double", StringComparison.Ordinal))
        {
            return string.Equals(op, "conv.r8", StringComparison.Ordinal);
        }

        return false;
    }

    private static bool IsZeroLiteralOperand(TypedIlInstructionArtifact instruction)
    {
        return instruction.Operand switch
        {
            int value => value == 0,
            long value => value == 0L,
            float value => value == 0f,
            double value => value == 0d,
            JsonElement element when element.ValueKind == JsonValueKind.Number => element.GetDouble() == 0d,
            _ => false,
        };
    }

    private static bool TryCreateAsyncTaskFamilyShape(string taskReturnType, out AsyncTaskFamilyShape familyShape)
    {
        familyShape = null!;
        if (!TryParseClosedTaskResultType(taskReturnType, out var managedResultType) ||
            !TryResolveAsyncManagedTypeShape(managedResultType, out var resultTypeShape))
        {
            return false;
        }

        familyShape = new AsyncTaskFamilyShape(managedResultType, resultTypeShape);
        return true;
    }

    private static bool TryResolveAsyncTaskFactoryMethodShape(
        TypedIlMethodArtifact method,
        out AsyncTaskFactoryMethodShape methodShape)
    {
        methodShape = null!;
        if (method.Parameters.Count > 2 ||
            !TryCreateAsyncTaskFamilyShape(GetMethodReturnType(method.SubjectId), out var familyShape))
        {
            return false;
        }

        var parameterTypeShapes = new List<AsyncManagedTypeShape>(method.Parameters.Count);
        foreach (var parameter in method.Parameters)
        {
            if (!TryResolveAsyncManagedTypeShape(parameter.Type, out var parameterTypeShape))
            {
                return false;
            }

            parameterTypeShapes.Add(parameterTypeShape);
        }

        methodShape = new AsyncTaskFactoryMethodShape(familyShape, parameterTypeShapes);
        return true;
    }

    private static bool ResolveAsyncLiteralParameterShapes(
        IReadOnlyList<TypedIlParameterArtifact> producerParameters,
        out IReadOnlyList<AsyncManagedTypeShape> literalParameterShapes)
    {
        var resolvedShapes = new List<AsyncManagedTypeShape>(producerParameters.Count);
        foreach (var parameter in producerParameters)
        {
            if (!TryResolveAsyncManagedTypeShape(parameter.Type, out var parameterTypeShape) ||
                !parameterTypeShape.SupportsLiteralLoading)
            {
                literalParameterShapes = [];
                return false;
            }

            resolvedShapes.Add(parameterTypeShape);
        }

        literalParameterShapes = resolvedShapes;
        return true;
    }

    private static bool TryResolveAsyncGetResultWrapperShape(
        TypedIlMethodArtifact method,
        IReadOnlyList<TypedIlInstructionArtifact> instructions,
        IReadOnlyDictionary<string, TypedIlMethodArtifact> methodsBySubjectId,
        out AsyncGetResultWrapperShape wrapperShape)
    {
        wrapperShape = null!;
        if (!TryResolveAsyncManagedTypeShape(GetMethodReturnType(method.SubjectId), out var wrapperResultTypeShape))
        {
            return false;
        }

        for (var callInstructionIndex = 0; callInstructionIndex <= instructions.Count - 6; callInstructionIndex++)
        {
            if (!string.Equals(instructions[callInstructionIndex].Op, "call", StringComparison.Ordinal) ||
                string.IsNullOrWhiteSpace(instructions[callInstructionIndex].Callee))
            {
                continue;
            }

            var producedTaskMethod = instructions[callInstructionIndex].Callee!;
            if (string.Equals(producedTaskMethod, method.SubjectId, StringComparison.Ordinal) ||
                !methodsBySubjectId.TryGetValue(producedTaskMethod, out var producerMethod) ||
                !TryResolveAsyncTaskFactoryMethodShape(producerMethod, out var producerMethodShape) ||
                !ResolveAsyncLiteralParameterShapes(producerMethod.Parameters, out var expectedLiteralParameterShapes))
            {
                continue;
            }

            if (!string.Equals(producerMethodShape.Family.ManagedResultType, wrapperResultTypeShape.ManagedType, StringComparison.Ordinal) ||
                callInstructionIndex != expectedLiteralParameterShapes.Count ||
                instructions.Count != expectedLiteralParameterShapes.Count + 6)
            {
                continue;
            }

            var matchesLiteralPrefix = true;
            for (var instructionIndex = 0; instructionIndex < expectedLiteralParameterShapes.Count; instructionIndex++)
            {
                if (!string.Equals(
                        instructions[instructionIndex].Op,
                        expectedLiteralParameterShapes[instructionIndex].LiteralOp,
                        StringComparison.Ordinal))
                {
                    matchesLiteralPrefix = false;
                    break;
                }
            }

            if (!matchesLiteralPrefix)
            {
                continue;
            }

            wrapperShape = new AsyncGetResultWrapperShape(producerMethodShape.Family, expectedLiteralParameterShapes);
            return true;
        }

        return false;
    }

    private static void ValidateAsyncTaskFactoryShape(
        TypedIlMethodArtifact method,
        IReadOnlyList<TypedIlInstructionArtifact> instructions,
        AsyncTaskFactoryMethodShape methodShape)
    {
        RequireMethodContract(method, "static-method", "has-canonical-body");

        var argc = methodShape.ParameterTypeShapes.Count;
        if (argc > 2)
        {
            throw new InvalidOperationException(
                $"native-reference emitter expects async {methodShape.Family.DisplayName} factory '{method.SubjectId}' to take no more than two supported arguments");
        }

        var expectedInstructionCount = 14 + (argc * 3);
        RequireInstructionCount(method, instructions, expectedInstructionCount);
        RequireInstructionOp(instructions[0], "ldloca", method.SubjectId, 0);
        if (GetRequiredOperandInt(instructions[0]) != 0)
        {
            throw new InvalidOperationException(
                $"native-reference emitter expects '{method.SubjectId}' to initialize local state machine slot 0");
        }

        RequireInstructionOp(instructions[1], "call", method.SubjectId, 1);
        RequireInstructionCallee(
            instructions[1],
            methodShape.Family.BuilderCreateCallee,
            method.SubjectId,
            1);
        RequireInstructionOp(instructions[2], "stfld", method.SubjectId, 2);

        var instructionIndex = 3;
        for (var argIndex = 0; argIndex < argc; argIndex++)
        {
            RequireInstructionOp(instructions[instructionIndex], "ldloca", method.SubjectId, instructionIndex);
            if (GetRequiredOperandInt(instructions[instructionIndex]) != 0)
            {
                throw new InvalidOperationException(
                    $"native-reference emitter expects '{method.SubjectId}' to reload local state machine slot 0");
            }

            RequireInstructionOp(instructions[instructionIndex + 1], "ldarg", method.SubjectId, instructionIndex + 1);
            if (GetRequiredOperandInt(instructions[instructionIndex + 1]) != argIndex)
            {
                throw new InvalidOperationException(
                    $"native-reference emitter expects '{method.SubjectId}' to store async {methodShape.Family.DisplayName} wrapper argument {argIndex}");
            }

            RequireInstructionOp(instructions[instructionIndex + 2], "stfld", method.SubjectId, instructionIndex + 2);
            instructionIndex += 3;
        }

        RequireInstructionOp(instructions[instructionIndex], "ldloca", method.SubjectId, instructionIndex);
        RequireInstructionOp(instructions[instructionIndex + 1], "ldc.i4", method.SubjectId, instructionIndex + 1);
        RequireInstructionOp(instructions[instructionIndex + 2], "stfld", method.SubjectId, instructionIndex + 2);
        if (GetRequiredOperandInt(instructions[instructionIndex]) != 0 ||
            GetRequiredOperandInt(instructions[instructionIndex + 1]) != -1)
        {
            throw new InvalidOperationException(
                $"native-reference emitter expects '{method.SubjectId}' to set async state to -1 on local slot 0");
        }

        RequireInstructionOp(instructions[instructionIndex + 3], "ldloca", method.SubjectId, instructionIndex + 3);
        RequireInstructionOp(instructions[instructionIndex + 4], "ldflda", method.SubjectId, instructionIndex + 4);
        RequireInstructionOp(instructions[instructionIndex + 5], "ldloca", method.SubjectId, instructionIndex + 5);
        RequireInstructionOp(instructions[instructionIndex + 6], "call", method.SubjectId, instructionIndex + 6);
        RequireInstructionOp(instructions[instructionIndex + 7], "ldloca", method.SubjectId, instructionIndex + 7);
        RequireInstructionOp(instructions[instructionIndex + 8], "ldflda", method.SubjectId, instructionIndex + 8);
        RequireInstructionOp(instructions[instructionIndex + 9], "call", method.SubjectId, instructionIndex + 9);
        RequireInstructionOp(instructions[instructionIndex + 10], "ret", method.SubjectId, instructionIndex + 10);

        if (GetRequiredOperandInt(instructions[instructionIndex + 3]) != 0 ||
            GetRequiredOperandInt(instructions[instructionIndex + 5]) != 0 ||
            GetRequiredOperandInt(instructions[instructionIndex + 7]) != 0)
        {
            throw new InvalidOperationException(
                $"native-reference emitter expects '{method.SubjectId}' to use local async state machine slot 0");
        }

        RequireInstructionCalleePrefix(
            instructions[instructionIndex + 6],
            methodShape.Family.BuilderStartPrefix,
            method.SubjectId,
            instructionIndex + 6);
        RequireInstructionCallee(
            instructions[instructionIndex + 9],
            methodShape.Family.BuilderGetTaskCallee,
            method.SubjectId,
            instructionIndex + 9);
    }

    private static AsyncGetResultWrapperShape ValidateAsyncGetResultShape(
        TypedIlMethodArtifact method,
        IReadOnlyList<TypedIlInstructionArtifact> instructions,
        IReadOnlyDictionary<string, TypedIlMethodArtifact> methodsBySubjectId)
    {
        RequireMethodContract(method, "static-method", "has-canonical-body");
        if (method.Parameters.Count != 0)
        {
            throw new InvalidOperationException(
                $"native-reference emitter expects async get-result wrapper '{method.SubjectId}' to be parameterless");
        }

        if (!TryResolveAsyncGetResultWrapperShape(method, instructions, methodsBySubjectId, out var wrapperShape))
        {
            var literalExpectation = "supported literal arguments";
            if (instructions.Count >= 6 &&
                !string.IsNullOrWhiteSpace(instructions[0].Callee) &&
                methodsBySubjectId.TryGetValue(instructions[0].Callee!, out var producerMethod) &&
                ResolveAsyncLiteralParameterShapes(producerMethod.Parameters, out var literalParameterShapes))
            {
                literalExpectation = literalParameterShapes.Count == 0
                    ? "no literal arguments"
                    : string.Join(", ", literalParameterShapes.Select(shape => shape.LiteralExpectation));
            }

            throw new InvalidOperationException(
                $"native-reference emitter expects async get-result wrapper '{method.SubjectId}' to load {literalExpectation} before invoking the async producer");
        }

        var producerFamilyShape = wrapperShape.ProducerFamilyShape;
        var expectedLiteralParameterShapes = wrapperShape.ExpectedLiteralParameterShapes;
        var callInstructionIndex = expectedLiteralParameterShapes.Count;
        RequireInstructionOp(instructions[callInstructionIndex], "call", method.SubjectId, callInstructionIndex);
        RequireInstructionOp(instructions[callInstructionIndex + 1], "call", "callvirt", method.SubjectId, callInstructionIndex + 1);
        RequireInstructionOp(instructions[callInstructionIndex + 2], "stloc", method.SubjectId, callInstructionIndex + 2);
        RequireInstructionOp(instructions[callInstructionIndex + 3], "ldloca", method.SubjectId, callInstructionIndex + 3);
        RequireInstructionOp(instructions[callInstructionIndex + 4], "call", method.SubjectId, callInstructionIndex + 4);
        RequireInstructionOp(instructions[callInstructionIndex + 5], "ret", method.SubjectId, callInstructionIndex + 5);

        var producedTaskMethod = GetRequiredInstructionCallee(instructions[callInstructionIndex], method.SubjectId, callInstructionIndex);
        if (string.Equals(producedTaskMethod, method.SubjectId, StringComparison.Ordinal))
        {
            throw new InvalidOperationException(
                $"native-reference emitter expects '{method.SubjectId}' to consume an async producer, not recurse into itself");
        }

        RequireInstructionCallee(
            instructions[callInstructionIndex + 1],
            producerFamilyShape.TaskGetAwaiterCallee,
            method.SubjectId,
            callInstructionIndex + 1);
        RequireInstructionCallee(
            instructions[callInstructionIndex + 4],
            producerFamilyShape.AwaiterGetResultCallee,
            method.SubjectId,
            callInstructionIndex + 4);
        return wrapperShape;
    }

    private static void ValidateAsyncStateMachineMoveNextShape(
        TypedIlMethodArtifact method,
        IReadOnlyList<TypedIlInstructionArtifact> instructions)
    {
        RequireMethodContract(method, "instance-method", "has-canonical-body");
        if (instructions.Count < 20)
        {
            throw new InvalidOperationException(
                $"native-reference emitter expects async state machine MoveNext '{method.SubjectId}' to contain the canonical async body");
        }

        var callees = instructions
            .Where(instruction =>
                (string.Equals(instruction.Op, "call", StringComparison.Ordinal) ||
                 string.Equals(instruction.Op, "callvirt", StringComparison.Ordinal)) &&
                !string.IsNullOrWhiteSpace(instruction.Callee))
            .Select(instruction => instruction.Callee!)
            .ToList();

        if (!callees.Contains("System.Private.CoreLib/System.Threading.Tasks.Task::Yield:System.Runtime.CompilerServices.YieldAwaitable()", StringComparer.Ordinal))
        {
            throw new InvalidOperationException(
                $"native-reference emitter expects async state machine MoveNext '{method.SubjectId}' to call Task::Yield()");
        }

        if (!callees.Any(IsSupportedAsyncAwaitUnsafeOnCompletedCallee))
        {
            throw new InvalidOperationException(
                $"native-reference emitter expects async state machine MoveNext '{method.SubjectId}' to call AwaitUnsafeOnCompleted");
        }

        if (!callees.Any(IsSupportedAsyncSetResultCallee))
        {
            throw new InvalidOperationException(
                $"native-reference emitter expects async state machine MoveNext '{method.SubjectId}' to call SetResult");
        }

        if (!callees.Any(IsSupportedAsyncSetExceptionCallee))
        {
            throw new InvalidOperationException(
                $"native-reference emitter expects async state machine MoveNext '{method.SubjectId}' to call SetException");
        }

        if (!callees.Contains(
                "System.Private.CoreLib/System.Runtime.CompilerServices.YieldAwaitable+YieldAwaiter::GetResult:System.Void()",
                StringComparer.Ordinal) &&
            !callees.Any(IsSupportedAsyncTaskAwaiterGetResultCallee))
        {
            throw new InvalidOperationException(
                $"native-reference emitter expects async state machine MoveNext '{method.SubjectId}' to observe an awaiter GetResult call");
        }
    }

    private static void ValidateAsyncStateMachineSetStateMachineShape(
        TypedIlMethodArtifact method,
        IReadOnlyList<TypedIlInstructionArtifact> instructions)
    {
        RequireMethodContract(method, "instance-method", "has-canonical-body");
        if (method.Parameters.Count != 1 ||
            !string.Equals(method.Parameters[0].Type, "System.Runtime.CompilerServices.IAsyncStateMachine", StringComparison.Ordinal))
        {
            throw new InvalidOperationException(
                $"native-reference emitter expects async state machine SetStateMachine '{method.SubjectId}' to take IAsyncStateMachine");
        }

        RequireInstructionCount(method, instructions, 5);
        RequireInstructionOp(instructions[0], "ldarg", method.SubjectId, 0);
        RequireInstructionOp(instructions[1], "ldflda", method.SubjectId, 1);
        RequireInstructionOp(instructions[2], "ldarg", method.SubjectId, 2);
        RequireInstructionOp(instructions[3], "call", method.SubjectId, 3);
        RequireInstructionOp(instructions[4], "ret", method.SubjectId, 4);

        if (GetRequiredOperandInt(instructions[0]) != 0 ||
            GetRequiredOperandInt(instructions[2]) != 1)
        {
            throw new InvalidOperationException(
                $"native-reference emitter expects '{method.SubjectId}' to load ldarg 0 and ldarg 1");
        }

        var setStateMachineCallee = instructions[3].Callee;
        if (string.IsNullOrWhiteSpace(setStateMachineCallee) ||
            !IsSupportedAsyncSetStateMachineCallee(setStateMachineCallee))
        {
            throw new InvalidOperationException(
                $"native-reference emitter expects instruction 3 in '{method.SubjectId}' to call a supported AsyncTaskMethodBuilder<T>.SetStateMachine, but found '{setStateMachineCallee ?? "<null>"}'");
        }
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

    private static void ValidateDelegateClosedTargetRelayEntryPointShape(
        TypedIlMethodArtifact method,
        IReadOnlyList<TypedIlInstructionArtifact> instructions)
    {
        RequireMethodContract(method, "static-method", "has-canonical-body");

        if (!IsDelegateClosedTargetRelayEntryPointShape(instructions))
        {
            throw new InvalidOperationException(
                $"native-reference emitter expects '{method.SubjectId}' to follow delegate-closed-target-relay proof shape");
        }

        var writeLineInstructionIndex = GetDelegateClosedTargetRelayWriteLineInstructionIndex(instructions);
        if (!IsConsoleWriteLineStringTarget(GetRequiredInstructionCallee(instructions[writeLineInstructionIndex], method.SubjectId, writeLineInstructionIndex)))
        {
            throw new InvalidOperationException(
                $"native-reference emitter expects '{method.SubjectId}' to call Console.WriteLine(System.String)");
        }

        var returnValueInstructionIndex = GetDelegateClosedTargetRelayReturnValueInstructionIndex(instructions);
        if (GetRequiredOperandInt(instructions[returnValueInstructionIndex]) != 0)
        {
            throw new InvalidOperationException(
                $"native-reference emitter expects '{method.SubjectId}' to return ldc.i4 0 before ret");
        }
    }

    private static bool IsDelegateClosedTargetRelayEntryPointShape(IReadOnlyList<TypedIlInstructionArtifact> instructions)
    {
        return instructions.Count == 21 &&
               string.Equals(instructions[0].Op, "ldstr", StringComparison.Ordinal) &&
               string.Equals(instructions[1].Op, "newobj", StringComparison.Ordinal) &&
               string.Equals(instructions[2].Op, "ldftn", StringComparison.Ordinal) &&
               string.Equals(instructions[3].Op, "newobj", StringComparison.Ordinal) &&
               string.Equals(instructions[4].Op, "stloc", StringComparison.Ordinal) &&
               string.Equals(instructions[5].Op, "ldsfld", StringComparison.Ordinal) &&
               string.Equals(instructions[6].Op, "dup", StringComparison.Ordinal) &&
               string.Equals(instructions[7].Op, "brtrue", StringComparison.Ordinal) &&
               string.Equals(instructions[8].Op, "pop", StringComparison.Ordinal) &&
               string.Equals(instructions[9].Op, "ldnull", StringComparison.Ordinal) &&
               string.Equals(instructions[10].Op, "ldftn", StringComparison.Ordinal) &&
               string.Equals(instructions[11].Op, "newobj", StringComparison.Ordinal) &&
               string.Equals(instructions[12].Op, "dup", StringComparison.Ordinal) &&
               string.Equals(instructions[13].Op, "stsfld", StringComparison.Ordinal) &&
               string.Equals(instructions[14].Op, "ldloc", StringComparison.Ordinal) &&
               string.Equals(instructions[15].Op, "ldstr", StringComparison.Ordinal) &&
               string.Equals(instructions[16].Op, "callvirt", StringComparison.Ordinal) &&
               string.Equals(instructions[17].Op, "callvirt", StringComparison.Ordinal) &&
               string.Equals(instructions[18].Op, "call", StringComparison.Ordinal) &&
               string.Equals(instructions[19].Op, "ldc.i4", StringComparison.Ordinal) &&
               string.Equals(instructions[20].Op, "ret", StringComparison.Ordinal);
    }

    private static int GetDelegateClosedTargetRelayStaticMethodInstructionIndex(IReadOnlyList<TypedIlInstructionArtifact> instructions)
    {
        return instructions.Count == 21
            ? 10
            : throw new InvalidOperationException("unsupported delegate closed target relay entry-point shape");
    }

    private static int GetDelegateClosedTargetRelayMessagePrefixInstructionIndex(IReadOnlyList<TypedIlInstructionArtifact> instructions)
    {
        return instructions.Count == 21
            ? 15
            : throw new InvalidOperationException("unsupported delegate closed target relay entry-point shape");
    }

    private static int GetDelegateClosedTargetRelayWriteLineInstructionIndex(IReadOnlyList<TypedIlInstructionArtifact> instructions)
    {
        return instructions.Count == 21
            ? 18
            : throw new InvalidOperationException("unsupported delegate closed target relay entry-point shape");
    }

    private static int GetDelegateClosedTargetRelayReturnValueInstructionIndex(IReadOnlyList<TypedIlInstructionArtifact> instructions)
    {
        return instructions.Count == 21
            ? 19
            : throw new InvalidOperationException("unsupported delegate closed target relay entry-point shape");
    }

    private static void ValidateDelegateClosedTargetRelayStaticTailShape(
        TypedIlMethodArtifact method,
        IReadOnlyList<TypedIlInstructionArtifact> instructions)
    {
        RequireMethodContract(method, "static-method", "has-canonical-body");
        RequireInstructionCount(method, instructions, 4);
        RequireInstructionOp(instructions[0], "ldarg", method.SubjectId, 0);
        RequireInstructionOp(instructions[1], "ldstr", method.SubjectId, 1);
        RequireInstructionOp(instructions[2], "call", method.SubjectId, 2);
        RequireInstructionOp(instructions[3], "ret", method.SubjectId, 3);

        if (GetRequiredOperandInt(instructions[0]) != 0)
        {
            throw new InvalidOperationException(
                $"native-reference emitter expects '{method.SubjectId}' to load ldarg 0 before appending a trailing literal");
        }

        var concatIcall = NormalizeStringConcatIcall(GetRequiredInstructionCallee(instructions[2], method.SubjectId, 2));
        if (!string.Equals(concatIcall, StringConcatPairIcall, StringComparison.Ordinal))
        {
            throw new InvalidOperationException(
                $"native-reference emitter expects delegate static tail '{method.SubjectId}' to use pair string concat");
        }
    }

    private static string GetDelegateClosedTargetRelayTrailingLiteral(
        TypedIlMethodArtifact method,
        IReadOnlyList<TypedIlInstructionArtifact> instructions)
    {
        ValidateDelegateClosedTargetRelayStaticTailShape(method, instructions);
        return GetRequiredOperandString(instructions[1]);
    }

    private static void ValidateStaticExceptionThrowLiteralShape(
        TypedIlMethodArtifact method,
        IReadOnlyList<TypedIlInstructionArtifact> instructions)
    {
        RequireMethodContract(method, "static-method", "has-canonical-body");
        if (!string.Equals(GetMethodReturnType(method.SubjectId), "System.Void", StringComparison.Ordinal) ||
            method.Parameters.Count != 0)
        {
            throw new InvalidOperationException(
                $"native-reference emitter expects static exception throw literal '{method.SubjectId}' to be parameterless and return void");
        }

        RequireInstructionCount(method, instructions, 3);
        RequireInstructionOp(instructions[0], "ldstr", method.SubjectId, 0);
        RequireInstructionOp(instructions[1], "newobj", method.SubjectId, 1);
        RequireInstructionOp(instructions[2], "throw", method.SubjectId, 2);
        RequireInstructionCallee(
            instructions[1],
            "System.Private.CoreLib/System.InvalidOperationException::.ctor:System.Void(System.String)",
            method.SubjectId,
            1);
    }

    private static void ValidateStaticExceptionThrowStringProducerShape(
        TypedIlMethodArtifact method,
        IReadOnlyList<TypedIlInstructionArtifact> instructions)
    {
        RequireMethodContract(method, "static-method", "has-canonical-body");
        if (!string.Equals(GetMethodReturnType(method.SubjectId), "System.Void", StringComparison.Ordinal) ||
            method.Parameters.Count != 0)
        {
            throw new InvalidOperationException(
                $"native-reference emitter expects static exception throw string producer '{method.SubjectId}' to be parameterless and return void");
        }

        RequireInstructionCount(method, instructions, 3);
        RequireInstructionOp(instructions[0], "call", method.SubjectId, 0);
        RequireInstructionOp(instructions[1], "newobj", method.SubjectId, 1);
        RequireInstructionOp(instructions[2], "throw", method.SubjectId, 2);

        var exceptionConstructorSubjectId = GetRequiredInstructionCallee(instructions[1], method.SubjectId, 1);
        if (!exceptionConstructorSubjectId.EndsWith("::.ctor:System.Void(System.String)", StringComparison.Ordinal))
        {
            throw new InvalidOperationException(
                $"native-reference emitter expects static exception throw string producer '{method.SubjectId}' to construct a string-taking exception, but found '{exceptionConstructorSubjectId}'");
        }
    }

    private static void ValidateStaticExceptionCatchStringReturnShape(
        TypedIlMethodArtifact method,
        IReadOnlyList<TypedIlInstructionArtifact> instructions)
    {
        RequireMethodContract(method, "static-method", "has-canonical-body");
        if (!string.Equals(GetMethodReturnType(method.SubjectId), "System.String", StringComparison.Ordinal) ||
            method.Parameters.Count != 0)
        {
            throw new InvalidOperationException(
                $"native-reference emitter expects static exception catch string return '{method.SubjectId}' to be parameterless and return string");
        }

        RequireInstructionCount(method, instructions, 10);
        RequireInstructionOp(instructions[0], "call", method.SubjectId, 0);
        RequireInstructionOp(instructions[1], "ldstr", method.SubjectId, 1);
        RequireInstructionOp(instructions[2], "stloc", method.SubjectId, 2);
        RequireInstructionOp(instructions[3], "leave", method.SubjectId, 3);
        RequireInstructionOp(instructions[4], "pop", method.SubjectId, 4);
        RequireInstructionOp(instructions[5], "ldstr", method.SubjectId, 5);
        RequireInstructionOp(instructions[6], "stloc", method.SubjectId, 6);
        RequireInstructionOp(instructions[7], "leave", method.SubjectId, 7);
        RequireInstructionOp(instructions[8], "ldloc", method.SubjectId, 8);
        RequireInstructionOp(instructions[9], "ret", method.SubjectId, 9);
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

    private static string GetCapturedFieldSubjectIdForSupportedStringInstanceMethod(
        TypedIlMethodArtifact method,
        IReadOnlyList<TypedIlInstructionArtifact> instructions)
    {
        try
        {
            ValidateFieldGetterShape(method, instructions);
            return GetRequiredOperandString(instructions[1]);
        }
        catch
        {
        }

        ValidateFieldBackedStringInstanceMethodShape(method, instructions);
        return GetRequiredOperandString(instructions[2]);
    }

    private static bool HasMethodContract(
        TypedIlMethodArtifact method,
        string expectedMethodRole,
        string expectedBodyAvailability)
    {
        return string.Equals(method.MethodRole, expectedMethodRole, StringComparison.Ordinal) &&
               string.Equals(method.BodyAvailability, expectedBodyAvailability, StringComparison.Ordinal);
    }

    private static bool IsRuntimeSkeletonNonExecutableDeclaration(TypedIlMethodArtifact method)
    {
        return string.Equals(method.BodyAvailability, "no-canonical-body", StringComparison.Ordinal) &&
               !string.Equals(method.MethodRole, "imported-method", StringComparison.Ordinal) &&
               method.Blocks.All(block => block.Instructions.Count == 0);
    }

    private static bool HasCapability(
        TypedIlMethodArtifact method,
        string requiredCapability)
    {
        return method.Capabilities.Contains(requiredCapability, StringComparer.Ordinal);
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

    private static void RequireInstructionOp(
        TypedIlInstructionArtifact instruction,
        string expectedOp,
        string alternateExpectedOp,
        string subjectId,
        int instructionIndex)
    {
        if (!string.Equals(instruction.Op, expectedOp, StringComparison.Ordinal) &&
            !string.Equals(instruction.Op, alternateExpectedOp, StringComparison.Ordinal))
        {
            throw new InvalidOperationException(
                $"native-reference emitter expects instruction {instructionIndex} in '{subjectId}' to be '{expectedOp}' or '{alternateExpectedOp}', but found '{instruction.Op}'");
        }
    }

    private static void RequireInstructionCallee(
        TypedIlInstructionArtifact instruction,
        string expectedCallee,
        string subjectId,
        int instructionIndex)
    {
        RequireInstructionCallee(instruction, expectedCallee, null, subjectId, instructionIndex);
    }

    private static void RequireInstructionCallee(
        TypedIlInstructionArtifact instruction,
        string expectedCallee,
        string? alternateExpectedCallee,
        string subjectId,
        int instructionIndex)
    {
        if (!string.Equals(instruction.Callee, expectedCallee, StringComparison.Ordinal) &&
            !string.Equals(instruction.Callee, alternateExpectedCallee, StringComparison.Ordinal))
        {
            var expectedDescription = alternateExpectedCallee is null
                ? $"'{expectedCallee}'"
                : $"'{expectedCallee}' or '{alternateExpectedCallee}'";
            throw new InvalidOperationException(
                $"native-reference emitter expects instruction {instructionIndex} in '{subjectId}' to call {expectedDescription}, but found '{instruction.Callee ?? "<null>"}'");
        }
    }

    private static void RequireInstructionCalleePrefix(
        TypedIlInstructionArtifact instruction,
        string expectedPrefix,
        string subjectId,
        int instructionIndex)
    {
        if (string.IsNullOrWhiteSpace(instruction.Callee) ||
            !instruction.Callee.StartsWith(expectedPrefix, StringComparison.Ordinal))
        {
            throw new InvalidOperationException(
                $"native-reference emitter expects instruction {instructionIndex} in '{subjectId}' to call a method starting with '{expectedPrefix}', but found '{instruction.Callee ?? "<null>"}'");
        }
    }
}
