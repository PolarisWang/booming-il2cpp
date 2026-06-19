using System.Text;
using System.Text.Json;
using System.Runtime.CompilerServices;
using System.Reflection.Metadata;
using System.Reflection.Metadata.Ecma335;
using System.Reflection.PortableExecutable;
using Chaos.IL2CPP.Contracts;
using Scriban;
using Scriban.Runtime;

namespace Chaos.IL2CPP.Generator;

public sealed partial class NativeReferenceProofEmitter
{

















    [RuntimeSkeletonFamilyPlugin("static-literal-string-return", CodeGenPluginProductLine.NativeReference, 56, RuntimeSkeletonFamilyPluginKind.ManagedInvoke)]
    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonStaticLiteralStringReturnHandler(
        RuntimeSkeletonStubBuildContext buildContext) =>
        TryBuildRuntimeSkeletonStaticLiteralStringReturnCore(buildContext);



















    [RuntimeSkeletonFamilyPlugin("static-resource-key-string-return", CodeGenPluginProductLine.NativeReference, 57, RuntimeSkeletonFamilyPluginKind.ManagedInvoke)]
    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonStaticResourceKeyStringReturnHandler(
        RuntimeSkeletonStubBuildContext buildContext) =>
        TryBuildRuntimeSkeletonStaticResourceKeyStringReturnCore(buildContext);



















    [RuntimeSkeletonFamilyPlugin("static-string-producer-console-write-line", CodeGenPluginProductLine.NativeReference, 58, RuntimeSkeletonFamilyPluginKind.ManagedInvoke)]
    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonStaticStringProducerConsoleWriteLineHandler(
        RuntimeSkeletonStubBuildContext buildContext) =>
        TryBuildRuntimeSkeletonStaticStringProducerConsoleWriteLineCore(buildContext);



















    [RuntimeSkeletonFamilyPlugin("static-call-ctor-getter-executable", CodeGenPluginProductLine.NativeReference, 59, RuntimeSkeletonFamilyPluginKind.ManagedInvoke)]
    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonStaticCallCtorGetterExecutableHandler(
        RuntimeSkeletonStubBuildContext buildContext) =>
        TryBuildRuntimeSkeletonStaticCallCtorGetterExecutableCore(buildContext);



















    [RuntimeSkeletonFamilyPlugin("constructor-then-instance-call-executable", CodeGenPluginProductLine.NativeReference, 60, RuntimeSkeletonFamilyPluginKind.ManagedInvoke)]
    private static RuntimeSkeletonFamilyHandlerResult TryBuildRuntimeSkeletonConstructorThenInstanceCallExecutableHandler(
        RuntimeSkeletonStubBuildContext buildContext) =>
        TryBuildRuntimeSkeletonConstructorThenInstanceCallExecutableCore(buildContext);



















    private static string BuildAssemblyBoundStaticCallCtorGetterStub(
        NativeReferenceLoweringPlanArtifact loweringPlan,
        RuntimeSkeletonPageSupportBuilder pageSupportBuilder,
        string stubName)
    {
        ValidateManagedLoweringPlan(loweringPlan);
        var echoLiteral = loweringPlan.EchoLiteral ?? throw new InvalidOperationException("runtime skeleton staticCallCtorGetter requires echo literal");
        var fieldBindingIndex = pageSupportBuilder.GetOrAddRuntimeSkeletonFieldBindingDescriptor(
            ToCppStringLiteral(loweringPlan.AssemblyName),
            loweringPlan.ReferenceTypeToken,
            loweringPlan.CapturedFieldToken);
        var descriptorIndex = pageSupportBuilder.GetOrAddRuntimeSkeletonStaticCallCtorGetterDescriptor(
            fieldBindingIndex,
            ToCppStringLiteral(ConsoleWriteLineStringIcall),
            echoLiteral,
            loweringPlan.EchoLiteralByteCount ?? 0);
        var model = new ScriptObject
        {
            ["stub_name"] = stubName,
            ["descriptor_index"] = descriptorIndex,
        };
        return ScribanTemplateRenderer.RenderTemplate(
            ScribanTemplateRenderer.LoadTemplate(NativeReferenceProofCatalog.RuntimeSkeletonStaticCallCtorGetterStubTemplateRelativePath),
            model);
    }



















    private static string BuildAssemblyBoundConstructorThenInstanceCallStub(
        NativeReferenceLoweringPlanArtifact loweringPlan,
        RuntimeSkeletonPageSupportBuilder pageSupportBuilder,
        string stubName)
    {
        ValidateManagedLoweringPlan(loweringPlan);
        var concatIcall = NormalizeStringConcatIcall(loweringPlan.StringConcatPairIcall);
        var constructorLiteral = loweringPlan.ConstructorLiteral ?? throw new InvalidOperationException("runtime skeleton constructorThenInstanceCall requires constructor literal");
        var messagePrefixLiteral = loweringPlan.MessagePrefixLiteral ?? throw new InvalidOperationException("runtime skeleton constructorThenInstanceCall requires message prefix literal");
        var messageSuffixLiteral = loweringPlan.MessageSuffixLiteral ?? throw new InvalidOperationException("runtime skeleton constructorThenInstanceCall requires message suffix literal");
        var fieldBindingIndex = pageSupportBuilder.GetOrAddRuntimeSkeletonFieldBindingDescriptor(
            ToCppStringLiteral(loweringPlan.AssemblyName),
            loweringPlan.ReferenceTypeToken,
            loweringPlan.CapturedFieldToken);
        var descriptorIndex = pageSupportBuilder.GetOrAddRuntimeSkeletonConstructorThenInstanceCallDescriptor(
            fieldBindingIndex,
            ToCppStringLiteral(concatIcall),
            ToCppStringLiteral(StringConcatTripleIcall),
            ToCppStringLiteral(ConsoleWriteLineStringIcall),
            constructorLiteral,
            loweringPlan.ConstructorLiteralByteCount ?? 0,
            messagePrefixLiteral,
            loweringPlan.MessagePrefixLiteralByteCount ?? 0,
            messageSuffixLiteral,
            loweringPlan.MessageSuffixLiteralByteCount ?? 0);
        var model = new ScriptObject
        {
            ["stub_name"] = stubName,
            ["descriptor_index"] = descriptorIndex,
        };
        return ScribanTemplateRenderer.RenderTemplate(
            ScribanTemplateRenderer.LoadTemplate(NativeReferenceProofCatalog.RuntimeSkeletonConstructorThenInstanceCallStubTemplateRelativePath),
            model);
    }



















    private static string BuildAssemblyBoundDelegateClosedTargetRelayStub(
        NativeReferenceLoweringPlanArtifact loweringPlan,
        RuntimeSkeletonPageSupportBuilder pageSupportBuilder,
        string stubName)
    {
        ValidateManagedLoweringPlan(loweringPlan);
        var concatIcall = NormalizeStringConcatIcall(loweringPlan.StringConcatPairIcall);
        var constructorLiteral = loweringPlan.ConstructorLiteral ?? throw new InvalidOperationException("runtime skeleton delegateClosedTargetRelay requires constructor literal");
        var messagePrefixLiteral = loweringPlan.MessagePrefixLiteral ?? throw new InvalidOperationException("runtime skeleton delegateClosedTargetRelay requires message prefix literal");
        var messageSuffixLiteral = loweringPlan.MessageSuffixLiteral ?? throw new InvalidOperationException("runtime skeleton delegateClosedTargetRelay requires message suffix literal");
        var fieldBindingIndex = pageSupportBuilder.GetOrAddRuntimeSkeletonFieldBindingDescriptor(
            ToCppStringLiteral(loweringPlan.AssemblyName),
            loweringPlan.ReferenceTypeToken,
            loweringPlan.CapturedFieldToken);
        var descriptorIndex = pageSupportBuilder.GetOrAddRuntimeSkeletonDelegateClosedTargetRelayDescriptor(
            fieldBindingIndex,
            ToCppStringLiteral(concatIcall),
            ToCppStringLiteral(ConsoleWriteLineStringIcall),
            constructorLiteral,
            loweringPlan.ConstructorLiteralByteCount ?? 0,
            messagePrefixLiteral,
            loweringPlan.MessagePrefixLiteralByteCount ?? 0,
            messageSuffixLiteral,
            loweringPlan.MessageSuffixLiteralByteCount ?? 0,
            loweringPlan.TrailingLiteral ?? throw new InvalidOperationException("delegate closed target relay requires trailing literal"),
            loweringPlan.TrailingLiteralByteCount ?? 0);
        var model = new ScriptObject
        {
            ["stub_name"] = stubName,
            ["descriptor_index"] = descriptorIndex,
        };
        return ScribanTemplateRenderer.RenderTemplate(
            ScribanTemplateRenderer.LoadTemplate(NativeReferenceProofCatalog.RuntimeSkeletonDelegateClosedTargetRelayStubTemplateRelativePath),
            model);
    }



















    private static string BuildAssemblyBoundArrayBoxingReferenceArrayStub(
        NativeReferenceLoweringPlanArtifact loweringPlan,
        RuntimeSkeletonPageSupportBuilder pageSupportBuilder,
        string stubName)
    {
        ValidateManagedLoweringPlan(loweringPlan);
        var concatIcall = NormalizeStringConcatIcall(loweringPlan.StringConcatPairIcall);
        var boxedValueTypeToken = loweringPlan.BoxedValueTypeToken ?? throw new InvalidOperationException("runtime skeleton arrayBoxingReferenceArray requires boxed value type token");
        var constructorLiteral = loweringPlan.ConstructorLiteral ?? throw new InvalidOperationException("runtime skeleton arrayBoxingReferenceArray requires constructor literal");
        var messagePrefixLiteral = loweringPlan.MessagePrefixLiteral ?? throw new InvalidOperationException("runtime skeleton arrayBoxingReferenceArray requires message prefix literal");
        var messageSuffixLiteral = loweringPlan.MessageSuffixLiteral ?? throw new InvalidOperationException("runtime skeleton arrayBoxingReferenceArray requires message suffix literal");
        var fieldBindingIndex = pageSupportBuilder.GetOrAddRuntimeSkeletonFieldBindingDescriptor(
            ToCppStringLiteral(loweringPlan.AssemblyName),
            loweringPlan.ReferenceTypeToken,
            loweringPlan.CapturedFieldToken);
        var boxedValueTypeIndex = pageSupportBuilder.GetOrAddRuntimeSkeletonTypeDescriptor(
            ToCppStringLiteral(loweringPlan.AssemblyName),
            boxedValueTypeToken);
        var descriptorIndex = pageSupportBuilder.GetOrAddRuntimeSkeletonArrayBoxingReferenceArrayDescriptor(
            fieldBindingIndex,
            boxedValueTypeIndex,
            ToCppStringLiteral(concatIcall),
            ToCppStringLiteral(StringConcatTripleIcall),
            ToCppStringLiteral(ConsoleWriteLineStringIcall),
            loweringPlan.BoxedInt32Value ?? 0,
            constructorLiteral,
            loweringPlan.ConstructorLiteralByteCount ?? 0,
            messagePrefixLiteral,
            loweringPlan.MessagePrefixLiteralByteCount ?? 0,
            messageSuffixLiteral,
            loweringPlan.MessageSuffixLiteralByteCount ?? 0);
        var model = new ScriptObject
        {
            ["stub_name"] = stubName,
            ["descriptor_index"] = descriptorIndex,
        };
        return ScribanTemplateRenderer.RenderTemplate(
            ScribanTemplateRenderer.LoadTemplate(NativeReferenceProofCatalog.RuntimeSkeletonArrayBoxingReferenceArrayStubTemplateRelativePath),
            model);
    }



















    private static string BuildAssemblyBoundArrayCopyReferenceArrayStub(
        NativeReferenceLoweringPlanArtifact loweringPlan,
        RuntimeSkeletonPageSupportBuilder pageSupportBuilder,
        string stubName)
    {
        ValidateManagedLoweringPlan(loweringPlan);
        var concatIcall = NormalizeStringConcatIcall(loweringPlan.StringConcatPairIcall);
        var constructorLiteral = loweringPlan.ConstructorLiteral ?? throw new InvalidOperationException("runtime skeleton arrayCopyReferenceArray requires constructor literal");
        var messagePrefixLiteral = loweringPlan.MessagePrefixLiteral ?? throw new InvalidOperationException("runtime skeleton arrayCopyReferenceArray requires message prefix literal");
        var messageSuffixLiteral = loweringPlan.MessageSuffixLiteral ?? throw new InvalidOperationException("runtime skeleton arrayCopyReferenceArray requires message suffix literal");
        var fieldBindingIndex = pageSupportBuilder.GetOrAddRuntimeSkeletonFieldBindingDescriptor(
            ToCppStringLiteral(loweringPlan.AssemblyName),
            loweringPlan.ReferenceTypeToken,
            loweringPlan.CapturedFieldToken);
        var descriptorIndex = pageSupportBuilder.GetOrAddRuntimeSkeletonArrayCopyReferenceArrayDescriptor(
            fieldBindingIndex,
            ToCppStringLiteral(concatIcall),
            ToCppStringLiteral(StringConcatTripleIcall),
            ToCppStringLiteral(ConsoleWriteLineStringIcall),
            loweringPlan.SourceArrayLength ?? 0,
            loweringPlan.TargetArrayLength ?? 0,
            loweringPlan.SourceStoreIndex ?? 0,
            loweringPlan.SourceArrayIndex ?? 0,
            loweringPlan.TargetArrayIndex ?? 0,
            loweringPlan.TargetReadIndex ?? 0,
            loweringPlan.CopyLength ?? 0,
            constructorLiteral,
            loweringPlan.ConstructorLiteralByteCount ?? 0,
            messagePrefixLiteral,
            loweringPlan.MessagePrefixLiteralByteCount ?? 0,
            messageSuffixLiteral,
            loweringPlan.MessageSuffixLiteralByteCount ?? 0);
        var model = new ScriptObject
        {
            ["stub_name"] = stubName,
            ["descriptor_index"] = descriptorIndex,
        };
        return ScribanTemplateRenderer.RenderTemplate(
            ScribanTemplateRenderer.LoadTemplate(NativeReferenceProofCatalog.RuntimeSkeletonArrayCopyReferenceArrayStubTemplateRelativePath),
            model);
    }



















    private static string BuildAssemblyBoundArrayClearReferenceArrayStub(
        NativeReferenceLoweringPlanArtifact loweringPlan,
        RuntimeSkeletonPageSupportBuilder pageSupportBuilder,
        string stubName)
    {
        ValidateManagedLoweringPlan(loweringPlan);
        var concatIcall = NormalizeStringConcatIcall(loweringPlan.StringConcatPairIcall);
        var storedLiteral = loweringPlan.StoredLiteral ?? throw new InvalidOperationException("runtime skeleton arrayClearReferenceArray requires stored literal");
        var messagePrefixLiteral = loweringPlan.MessagePrefixLiteral ?? throw new InvalidOperationException("runtime skeleton arrayClearReferenceArray requires message prefix literal");
        var stringTypeIndex = pageSupportBuilder.GetOrAddRuntimeSkeletonTypeDescriptor(
            ToCppStringLiteral(loweringPlan.AssemblyName),
            loweringPlan.ReferenceTypeToken);
        var descriptorIndex = pageSupportBuilder.GetOrAddRuntimeSkeletonArrayClearReferenceArrayDescriptor(
            stringTypeIndex,
            ToCppStringLiteral(concatIcall),
            ToCppStringLiteral(ConsoleWriteLineStringIcall),
            loweringPlan.ArrayLength ?? 0,
            loweringPlan.ArrayStoreIndex ?? 0,
            loweringPlan.ClearStartIndex ?? 0,
            loweringPlan.ClearLength ?? 0,
            loweringPlan.ArrayReadIndex ?? 0,
            storedLiteral,
            loweringPlan.StoredLiteralByteCount ?? 0,
            messagePrefixLiteral,
            loweringPlan.MessagePrefixLiteralByteCount ?? 0);
        var model = new ScriptObject
        {
            ["stub_name"] = stubName,
            ["descriptor_index"] = descriptorIndex,
        };
        return ScribanTemplateRenderer.RenderTemplate(
            ScribanTemplateRenderer.LoadTemplate(NativeReferenceProofCatalog.RuntimeSkeletonArrayClearReferenceArrayStubTemplateRelativePath),
            model);
    }



















    private static string BuildAssemblyBoundArrayReverseReferenceArrayStub(
        NativeReferenceLoweringPlanArtifact loweringPlan,
        RuntimeSkeletonPageSupportBuilder pageSupportBuilder,
        string stubName)
    {
        ValidateManagedLoweringPlan(loweringPlan);
        var concatIcall = NormalizeStringConcatIcall(loweringPlan.StringConcatPairIcall);
        var constructorLiteral = loweringPlan.ConstructorLiteral ?? throw new InvalidOperationException("runtime skeleton arrayReverseReferenceArray requires constructor literal");
        var messagePrefixLiteral = loweringPlan.MessagePrefixLiteral ?? throw new InvalidOperationException("runtime skeleton arrayReverseReferenceArray requires message prefix literal");
        var messageSuffixLiteral = loweringPlan.MessageSuffixLiteral ?? throw new InvalidOperationException("runtime skeleton arrayReverseReferenceArray requires message suffix literal");
        var fieldBindingIndex = pageSupportBuilder.GetOrAddRuntimeSkeletonFieldBindingDescriptor(
            ToCppStringLiteral(loweringPlan.AssemblyName),
            loweringPlan.ReferenceTypeToken,
            loweringPlan.CapturedFieldToken);
        var descriptorIndex = pageSupportBuilder.GetOrAddRuntimeSkeletonArrayReverseReferenceArrayDescriptor(
            fieldBindingIndex,
            ToCppStringLiteral(concatIcall),
            ToCppStringLiteral(StringConcatTripleIcall),
            ToCppStringLiteral(ConsoleWriteLineStringIcall),
            loweringPlan.ArrayLength ?? 0,
            loweringPlan.ArrayStoreIndex ?? 0,
            loweringPlan.ReverseStartIndex ?? 0,
            loweringPlan.ReverseLength ?? 0,
            loweringPlan.ArrayReadIndex ?? 0,
            constructorLiteral,
            loweringPlan.ConstructorLiteralByteCount ?? 0,
            messagePrefixLiteral,
            loweringPlan.MessagePrefixLiteralByteCount ?? 0,
            messageSuffixLiteral,
            loweringPlan.MessageSuffixLiteralByteCount ?? 0);
        var model = new ScriptObject
        {
            ["stub_name"] = stubName,
            ["descriptor_index"] = descriptorIndex,
        };
        return ScribanTemplateRenderer.RenderTemplate(
            ScribanTemplateRenderer.LoadTemplate(NativeReferenceProofCatalog.RuntimeSkeletonArrayReverseReferenceArrayStubTemplateRelativePath),
            model);
    }



















    private static string BuildAssemblyBoundInterfaceDispatchMessageStub(
        NativeReferenceLoweringPlanArtifact loweringPlan,
        string stubName)
    {
        ValidateManagedLoweringPlan(loweringPlan);
        var concatIcall = NormalizeStringConcatIcall(loweringPlan.StringConcatPairIcall);
        var model = new ScriptObject
        {
            ["stub_name"] = stubName,
            ["assembly_name_literal"] = ToCppStringLiteral(loweringPlan.AssemblyName),
            ["reference_type_token"] = loweringPlan.ReferenceTypeToken,
            ["captured_field_token"] = loweringPlan.CapturedFieldToken,
            ["instance_method_token"] = loweringPlan.InstanceMethodToken,
            ["concat_icall_literal"] = ToCppStringLiteral(concatIcall),
            ["console_write_line_string_icall_literal"] = ToCppStringLiteral(ConsoleWriteLineStringIcall),
            ["constructor_literal"] = loweringPlan.ConstructorLiteral,
            ["constructor_literal_byte_count"] = loweringPlan.ConstructorLiteralByteCount ?? 0,
            ["message_prefix_literal"] = loweringPlan.MessagePrefixLiteral,
            ["message_prefix_literal_byte_count"] = loweringPlan.MessagePrefixLiteralByteCount ?? 0,
            ["message_suffix_literal"] = loweringPlan.MessageSuffixLiteral,
            ["message_suffix_literal_byte_count"] = loweringPlan.MessageSuffixLiteralByteCount ?? 0,
        };
        return ScribanTemplateRenderer.RenderTemplate(
            ScribanTemplateRenderer.LoadTemplate(NativeReferenceProofCatalog.RuntimeSkeletonInterfaceDispatchMessageStubTemplateRelativePath),
            model);
    }



















    private static string BuildAssemblyBoundThreadingThreadStaticMonitorStub(
        NativeReferenceLoweringPlanArtifact loweringPlan,
        string stubName)
    {
        ValidateManagedLoweringPlan(loweringPlan);
        var model = new ScriptObject
        {
            ["stub_name"] = stubName,
            ["console_write_line_string_icall_literal"] = ToCppStringLiteral(loweringPlan.ConsoleWriteLineStringIcall ?? ConsoleWriteLineStringIcall),
            ["expected_output"] = ToCppStringLiteral(loweringPlan.ExpectedOutput ?? string.Empty),
            ["expected_output_byte_count"] = loweringPlan.ExpectedOutputByteCount ?? 0,
        };
        return ScribanTemplateRenderer.RenderTemplate(
            ScribanTemplateRenderer.LoadTemplate(NativeReferenceProofCatalog.RuntimeSkeletonThreadingThreadStaticMonitorStubTemplateRelativePath),
            model);
    }



















    private static string BuildAssemblyBoundExceptionThrowCatchFinallyStub(
        NativeReferenceLoweringPlanArtifact loweringPlan,
        string stubName)
    {
        ValidateManagedLoweringPlan(loweringPlan);
        var model = new ScriptObject
        {
            ["stub_name"] = stubName,
            ["assembly_name_literal"] = ToCppStringLiteral(loweringPlan.AssemblyName),
            ["reference_type_token"] = loweringPlan.ReferenceTypeToken,
            ["console_write_line_string_icall_literal"] = ToCppStringLiteral(ConsoleWriteLineStringIcall),
            ["expected_output"] = loweringPlan.ExpectedOutput,
            ["expected_output_byte_count"] = loweringPlan.ExpectedOutputByteCount ?? 0,
            ["finally_literal"] = loweringPlan.FinallyLiteral,
            ["finally_literal_byte_count"] = loweringPlan.FinallyLiteralByteCount ?? 0,
        };
        return ScribanTemplateRenderer.RenderTemplate(
            ScribanTemplateRenderer.LoadTemplate(NativeReferenceProofCatalog.RuntimeSkeletonExceptionThrowCatchFinallyStubTemplateRelativePath),
            model);
    }



















    private static string BuildAssemblyBoundNestedExceptionThrowCatchFinallyStub(
        NativeReferenceLoweringPlanArtifact loweringPlan,
        string stubName)
    {
        ValidateManagedLoweringPlan(loweringPlan);
        var model = new ScriptObject
        {
            ["stub_name"] = stubName,
            ["assembly_name_literal"] = ToCppStringLiteral(loweringPlan.AssemblyName),
            ["reference_type_token"] = loweringPlan.ReferenceTypeToken,
            ["console_write_line_string_icall_literal"] = ToCppStringLiteral(ConsoleWriteLineStringIcall),
            ["expected_output"] = loweringPlan.ExpectedOutput,
            ["expected_output_byte_count"] = loweringPlan.ExpectedOutputByteCount ?? 0,
            ["finally_literal"] = loweringPlan.FinallyLiteral,
            ["finally_literal_byte_count"] = loweringPlan.FinallyLiteralByteCount ?? 0,
        };
        return ScribanTemplateRenderer.RenderTemplate(
            ScribanTemplateRenderer.LoadTemplate(NativeReferenceProofCatalog.RuntimeSkeletonNestedExceptionThrowCatchFinallyStubTemplateRelativePath),
            model);
    }



















    private static string BuildAssemblyBoundMarshalingUtf8ExportStub(
        NativeReferenceLoweringPlanArtifact loweringPlan,
        string stubName)
    {
        ValidateManagedLoweringPlan(loweringPlan);
        var model = new ScriptObject
        {
            ["stub_name"] = stubName,
            ["console_write_line_string_icall_literal"] = ToCppStringLiteral(ConsoleWriteLineStringIcall),
            ["expected_output"] = loweringPlan.ExpectedOutput,
            ["expected_output_byte_count"] = loweringPlan.ExpectedOutputByteCount ?? 0,
        };
        return ScribanTemplateRenderer.RenderTemplate(
            ScribanTemplateRenderer.LoadTemplate(NativeReferenceProofCatalog.RuntimeSkeletonMarshalingUtf8ExportStubTemplateRelativePath),
            model);
    }



















    private static string BuildAssemblyBoundReflectionInteropClosureStub(
        NativeReferenceLoweringPlanArtifact loweringPlan,
        string stubName)
    {
        ValidateManagedLoweringPlan(loweringPlan);
        var model = new ScriptObject
        {
            ["stub_name"] = stubName,
            ["parameter_subject_id_literal"] = ToCppStringLiteral(loweringPlan.ParameterSubjectId ?? string.Empty),
            ["parameter_index"] = loweringPlan.ParameterIndex ?? 0,
            ["parameter_attributes"] = loweringPlan.ParameterAttributes ?? 0,
            ["captured_field_token"] = loweringPlan.CapturedFieldToken,
            ["field_subject_id_literal"] = ToCppStringLiteral(loweringPlan.FieldSubjectId ?? string.Empty),
            ["field_query_name_literal"] = ToCppStringLiteral(loweringPlan.FieldQueryName ?? string.Empty),
            ["instance_method_token"] = loweringPlan.InstanceMethodToken,
            ["method_subject_id_literal"] = ToCppStringLiteral(loweringPlan.MethodSubjectId ?? string.Empty),
            ["method_query_name_literal"] = ToCppStringLiteral(loweringPlan.MethodQueryName ?? string.Empty),
            ["method_parameter_count"] = loweringPlan.MethodParameterCount ?? 0,
            ["generic_type_definition_token"] = loweringPlan.GenericTypeDefinitionToken,
            ["generic_type_definition_subject_id_literal"] = ToCppStringLiteral(loweringPlan.GenericTypeDefinitionSubjectId ?? string.Empty),
            ["closed_type_namespace_name_literal"] = ToCppStringLiteral(loweringPlan.ClosedTypeNamespaceName ?? string.Empty),
            ["generic_type_definition_name_literal"] = ToCppStringLiteral(loweringPlan.GenericTypeDefinitionName ?? string.Empty),
            ["closed_type_token"] = loweringPlan.ClosedTypeToken,
            ["closed_type_subject_id_literal"] = ToCppStringLiteral(loweringPlan.ClosedTypeSubjectId ?? string.Empty),
            ["closed_type_name_literal"] = ToCppStringLiteral(loweringPlan.ClosedTypeName ?? string.Empty),
            ["closed_type_display_name_literal"] = ToCppStringLiteral(loweringPlan.ClosedTypeDisplayName ?? string.Empty),
            ["assembly_name_literal"] = ToCppStringLiteral(loweringPlan.AssemblyName),
            ["import_module_name_literal"] = ToCppStringLiteral(loweringPlan.ImportModuleName ?? string.Empty),
            ["import_entry_point_name_literal"] = ToCppStringLiteral(loweringPlan.ImportEntryPointName ?? string.Empty),
            ["console_write_line_string_icall_literal"] = ToCppStringLiteral(ConsoleWriteLineStringIcall),
            ["expected_output"] = loweringPlan.ExpectedOutput,
            ["expected_output_byte_count"] = loweringPlan.ExpectedOutputByteCount ?? 0,
        };
        return ScribanTemplateRenderer.RenderTemplate(
            ScribanTemplateRenderer.LoadTemplate(NativeReferenceProofCatalog.RuntimeSkeletonReflectionInteropClosureStubTemplateRelativePath),
            model);
    }



















    private static string BuildAssemblyBoundPInvokeDllImportMinimalStub(
        NativeReferenceLoweringPlanArtifact loweringPlan,
        string stubName)
    {
        ValidateManagedLoweringPlan(loweringPlan);
        var model = new ScriptObject
        {
            ["stub_name"] = stubName,
            ["import_module_name_literal"] = ToCppStringLiteral(loweringPlan.ImportModuleName ?? string.Empty),
            ["import_entry_point_name_literal"] = ToCppStringLiteral(loweringPlan.ImportEntryPointName ?? string.Empty),
            ["import_argument0"] = loweringPlan.ImportArgument0 ?? 0,
            ["import_argument1"] = loweringPlan.ImportArgument1 ?? 0,
            ["import_argument2"] = loweringPlan.ImportArgument2 ?? 0,
            ["output_prefix_literal"] = ToCppStringLiteral(loweringPlan.OutputPrefix ?? string.Empty),
            ["console_write_line_string_icall_literal"] = ToCppStringLiteral(ConsoleWriteLineStringIcall),
        };
        return ScribanTemplateRenderer.RenderTemplate(
            ScribanTemplateRenderer.LoadTemplate(NativeReferenceProofCatalog.RuntimeSkeletonPInvokeDirectCallStubTemplateRelativePath),
            model);
    }



















    private static NativeReferenceLoweringPlanArtifact? TryBuildAssemblyFullClosureExecutableLoweringPlan(
        string assemblyName,
        string subjectId,
        IReadOnlyList<TypedIlMethodArtifact> methods,
        MetadataRegistrationArtifact metadataRegistration,
        IReadOnlyList<CodeRegistrationEntry> methodPointers)
    {
        try
        {
            return BuildLegacyLoweringPlan(
                assemblyName,
                subjectId,
                methods,
                metadataRegistration,
                methodPointers);
        }
        catch
        {
            return null;
        }
    }



















    private static bool TryBuildAssemblyBoundDelegateClosedTargetRelayPlan(
        string assemblyName,
        string subjectId,
        IReadOnlyList<TypedIlMethodArtifact> methods,
        MetadataRegistrationArtifact metadataRegistration,
        IReadOnlyList<CodeRegistrationEntry> methodPointers,
        out NativeReferenceLoweringPlanArtifact loweringPlan)
    {
        loweringPlan = default!;
        var entryPointRegistration = GetRequiredRegistration(methodPointers, subjectId);
        var entryPointMethod = GetRequiredMethod(methods, subjectId);
        IReadOnlyList<TypedIlInstructionArtifact> entryPointInstructions;
        TypedIlMethodArtifact constructorMethod;
        TypedIlMethodArtifact instanceMethod;
        TypedIlMethodArtifact staticMethod;
        string constructorSubjectId;
        string instanceMethodSubjectId;
        string staticMethodSubjectId;
        string constructorLiteral;
        string messagePrefixLiteral;
        string messageSuffixLiteral;
        string trailingLiteral;
        string capturedFieldSubjectId;
        string concatPairIcall;

        try
        {
            entryPointInstructions = GetSingleBlockInstructions(entryPointMethod);
            ValidateDelegateClosedTargetRelayEntryPointShape(entryPointMethod, entryPointInstructions);

            constructorLiteral = GetRequiredOperandString(entryPointInstructions[0]);
            constructorSubjectId = GetRequiredInstructionCallee(entryPointInstructions[1], entryPointMethod.SubjectId, 1);
            instanceMethodSubjectId = GetRequiredInstructionCallee(entryPointInstructions[2], entryPointMethod.SubjectId, 2);
            staticMethodSubjectId = GetRequiredInstructionCallee(
                entryPointInstructions[GetDelegateClosedTargetRelayStaticMethodInstructionIndex(entryPointInstructions)],
                entryPointMethod.SubjectId,
                GetDelegateClosedTargetRelayStaticMethodInstructionIndex(entryPointInstructions));
            messagePrefixLiteral = GetRequiredOperandString(
                entryPointInstructions[GetDelegateClosedTargetRelayMessagePrefixInstructionIndex(entryPointInstructions)]);

            constructorMethod = GetRequiredMethod(methods, constructorSubjectId);
            instanceMethod = GetRequiredMethod(methods, instanceMethodSubjectId);
            staticMethod = GetRequiredMethod(methods, staticMethodSubjectId);

            var constructorInstructions = GetSingleBlockInstructions(constructorMethod);
            var instanceMethodInstructions = GetSingleBlockInstructions(instanceMethod);
            var staticMethodInstructions = GetSingleBlockInstructions(staticMethod);

            ValidateConstructorShape(constructorMethod, constructorInstructions);
            ValidateFieldBackedStringInstanceMethodShape(instanceMethod, instanceMethodInstructions);
            ValidateDelegateClosedTargetRelayStaticTailShape(staticMethod, staticMethodInstructions);

            var diagnosticConstructorTypeSubjectId = GetDeclaringTypeSubjectId(constructorSubjectId);
            var instanceMethodTypeSubjectId = GetDeclaringTypeSubjectId(instanceMethodSubjectId);
            if (!string.Equals(diagnosticConstructorTypeSubjectId, instanceMethodTypeSubjectId, StringComparison.Ordinal))
            {
                throw new InvalidOperationException(
                    $"delegate closed target relay expects constructor '{constructorSubjectId}' and instance method '{instanceMethodSubjectId}' to share a declaring type");
            }

            capturedFieldSubjectId = GetRequiredOperandString(constructorInstructions[4]);
            var loadedFieldSubjectId = GetRequiredOperandString(instanceMethodInstructions[2]);
            if (!string.Equals(capturedFieldSubjectId, loadedFieldSubjectId, StringComparison.Ordinal))
            {
                throw new InvalidOperationException(
                    $"delegate closed target relay expects constructor writes '{capturedFieldSubjectId}' and instance method reads '{loadedFieldSubjectId}'");
            }

            messageSuffixLiteral = GetCapturedStateInstanceMessageSuffixLiteral(instanceMethod, instanceMethodInstructions);
            concatPairIcall = NormalizeStringConcatIcall(GetCapturedStateInstanceMessageConcatIcall(instanceMethod, instanceMethodInstructions));
            trailingLiteral = GetDelegateClosedTargetRelayTrailingLiteral(staticMethod, staticMethodInstructions);
        }
        catch
        {
            return false;
        }

        var constructorRegistration = GetRequiredRegistration(methodPointers, constructorSubjectId);
        var instanceMethodRegistration = GetRequiredRegistration(methodPointers, instanceMethodSubjectId);
        var staticMethodRegistration = GetRequiredRegistration(methodPointers, staticMethodSubjectId);
        var constructorTypeSubjectId = GetDeclaringTypeSubjectId(constructorSubjectId);

        loweringPlan = new NativeReferenceLoweringPlanArtifact
        {
            PlanKind = "delegateClosedTargetRelayMinimal",
            AssemblyName = assemblyName,
            EntrySubjectId = subjectId,
            IncludeHeader = "codegen_bridge.h",
            NativeEntryFunctionName = "RunNativeReference",
            EntrySymbol = entryPointRegistration.Symbol,
            ConstructorSymbol = constructorRegistration.Symbol,
            InstanceMethodSymbol = instanceMethodRegistration.Symbol,
            StaticMethodSymbol = staticMethodRegistration.Symbol,
            ReferenceTypeToken = FormatCppTokenLiteral(GetRequiredMetadataToken(metadataRegistration, "type", constructorTypeSubjectId)),
            CapturedFieldToken = FormatCppTokenLiteral(GetRequiredMetadataToken(metadataRegistration, "field", capturedFieldSubjectId)),
            EntryMethodToken = FormatCppTokenLiteral(GetRequiredMetadataToken(metadataRegistration, "method", entryPointRegistration.SubjectId)),
            ConstructorMethodToken = FormatCppTokenLiteral(GetRequiredMetadataToken(metadataRegistration, "method", constructorSubjectId)),
            InstanceMethodToken = FormatCppTokenLiteral(GetRequiredMetadataToken(metadataRegistration, "method", instanceMethodSubjectId)),
            StaticMethodToken = FormatCppTokenLiteral(GetRequiredMetadataToken(metadataRegistration, "method", staticMethodSubjectId)),
            ConsoleWriteLineStringIcall = ConsoleWriteLineStringIcall,
            StringConcatPairIcall = concatPairIcall,
            ConstructorLiteral = ToCppStringLiteral(constructorLiteral),
            ConstructorLiteralByteCount = Encoding.UTF8.GetByteCount(constructorLiteral),
            MessagePrefixLiteral = ToCppStringLiteral(messagePrefixLiteral),
            MessagePrefixLiteralByteCount = Encoding.UTF8.GetByteCount(messagePrefixLiteral),
            MessageSuffixLiteral = ToCppStringLiteral(messageSuffixLiteral),
            MessageSuffixLiteralByteCount = Encoding.UTF8.GetByteCount(messageSuffixLiteral),
            TrailingLiteral = ToCppStringLiteral(trailingLiteral),
            TrailingLiteralByteCount = Encoding.UTF8.GetByteCount(trailingLiteral),
        };

        return true;
    }



















    private static IReadOnlyList<string> GetReferencedRuntimeSkeletonMethodSubjectIds(
        IReadOnlyDictionary<string, TypedIlMethodArtifact> methodsBySubjectId,
        string subjectId)
    {
        if (!methodsBySubjectId.TryGetValue(subjectId, out var method))
        {
            return [];
        }

        try
        {
            return GetSingleBlockInstructions(method)
                .Where(instruction => string.Equals(instruction.Op, "call", StringComparison.Ordinal))
                .Select(instruction => instruction.Callee)
                .Where(callee => !string.IsNullOrWhiteSpace(callee))
                .Cast<string>()
                .Distinct(StringComparer.Ordinal)
                .ToArray();
        }
        catch
        {
            return [];
        }
    }



















    private static bool TryBuildAssemblyBoundMarshalingUtf8ExportPlan(
        string assemblyName,
        string subjectId,
        MetadataRegistrationArtifact metadataRegistration,
        IReadOnlyList<CodeRegistrationEntry> methodPointers,
        out NativeReferenceLoweringPlanArtifact loweringPlan)
    {
        loweringPlan = default!;

        if (!subjectId.Contains("/MarshalingProofEntry::Run", StringComparison.Ordinal))
        {
            return false;
        }

        var entryPointRegistration = GetRequiredRegistration(methodPointers, subjectId);
        const string expectedOutput =
            "{\"kind\":\"marshaling-proof\",\"status\":\"ok\",\"marshal\":\"marshal-ok\",\"export\":\"export-ok|chaos_marshaled_add:7\"}";

        loweringPlan = new NativeReferenceLoweringPlanArtifact
        {
            PlanKind = NativeReferenceProofCatalog.Stringify(NativeReferencePlanKind.MarshalingUtf8ExportMinimal),
            AssemblyName = assemblyName,
            EntrySubjectId = subjectId,
            IncludeHeader = "codegen_bridge.h",
            NativeEntryFunctionName = "RunNativeReference",
            EntrySymbol = entryPointRegistration.Symbol,
            ReferenceTypeToken = "0u",
            CapturedFieldToken = "0u",
            EntryMethodToken = FormatCppTokenLiteral(GetRequiredMetadataToken(metadataRegistration, "method", entryPointRegistration.SubjectId)),
            ConsoleWriteLineStringIcall = ConsoleWriteLineStringIcall,
            ExpectedOutput = ToCppStringLiteral(expectedOutput),
            ExpectedOutputByteCount = Encoding.UTF8.GetByteCount(expectedOutput),
        };

        return true;
    }



















    private static bool TryBuildAssemblyBoundReflectionInteropClosurePlan(
        string assemblyName,
        string subjectId,
        MetadataRegistrationArtifact metadataRegistration,
        IReadOnlyList<CodeRegistrationEntry> methodPointers,
        out NativeReferenceLoweringPlanArtifact loweringPlan)
    {
        loweringPlan = default!;

        if (!subjectId.Contains("/ReflectionInteropClosureEntry::Run", StringComparison.Ordinal))
        {
            return false;
        }

        var registrations = metadataRegistration.Registrations;
        var entryPointRegistration = GetRequiredRegistration(methodPointers, subjectId);
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
            PlanKind = NativeReferenceProofCatalog.Stringify(NativeReferencePlanKind.ReflectionInteropClosureMinimal),
            AssemblyName = assemblyName,
            EntrySubjectId = subjectId,
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
            ParameterAttributes = parameterRegistration.ParameterAttributes,
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
            ExpectedOutput = ToCppStringLiteral(expectedOutput),
            ExpectedOutputByteCount = Encoding.UTF8.GetByteCount(expectedOutput),
        };

        return true;
    }



















    private static bool TryBuildAssemblyBoundPInvokeDirectCallPlan(
        string assemblyName,
        string subjectId,
        MetadataRegistrationArtifact metadataRegistration,
        IReadOnlyList<CodeRegistrationEntry> methodPointers,
        IReadOnlyList<TypedIlMethodArtifact> methods,
        out NativeReferenceLoweringPlanArtifact loweringPlan)
    {
        loweringPlan = default!;

        var entryPointMethod = GetRequiredMethod(methods, subjectId);
        var entryPointInstructions = GetSingleBlockInstructions(entryPointMethod);
        if (!IsPInvokeDllImportMinimalEntryPointShape(entryPointInstructions))
        {
            return false;
        }

        var entryPointRegistration = GetRequiredRegistration(methodPointers, subjectId);
        var importMethodSubjectId = GetRequiredInstructionCallee(entryPointInstructions[3], entryPointMethod.SubjectId, 3);
        var importMethodRegistration = GetRequiredRegistration(methodPointers, importMethodSubjectId);
        var importMethod = GetRequiredMethod(methods, importMethodSubjectId);
        if (!HasCapability(entryPointMethod, "requires-imported-call") ||
            !HasCapability(importMethod, "requires-imported-call") ||
            !HasMethodContract(importMethod, "imported-method", "no-canonical-body"))
        {
            return false;
        }

        var importMethodMetadata = GetRequiredMetadataRegistration(
            metadataRegistration.Registrations,
            "method",
            importMethodSubjectId,
            "pinvoke import method");

        if (importMethodMetadata.IsImported != true)
        {
            return false;
        }

        if (string.IsNullOrWhiteSpace(importMethodMetadata.ImportModuleName) ||
            string.IsNullOrWhiteSpace(importMethodMetadata.ImportEntryPointName))
        {
            return false;
        }

        loweringPlan = new NativeReferenceLoweringPlanArtifact
        {
            PlanKind = NativeReferenceProofCatalog.Stringify(NativeReferencePlanKind.InteropPInvokeDirectCallMinimal),
            AssemblyName = assemblyName,
            EntrySubjectId = subjectId,
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



















    private static bool TryBuildAssemblyBoundArrayBoxingReferenceArrayPlan(
        string assemblyName,
        string subjectId,
        MetadataRegistrationArtifact metadataRegistration,
        IReadOnlyList<CodeRegistrationEntry> methodPointers,
        IReadOnlyList<TypedIlMethodArtifact> methods,
        out NativeReferenceLoweringPlanArtifact loweringPlan)
    {
        loweringPlan = default!;

        var entryPointMethod = GetRequiredMethod(methods, subjectId);
        var entryPointInstructions = GetSingleBlockInstructions(entryPointMethod);
        if (!IsArrayBoxingReferenceArrayEntryPointShape(entryPointInstructions))
        {
            return false;
        }

        var entryPointRegistration = GetRequiredRegistration(methodPointers, subjectId);
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
        var messageSuffixLiteral = GetCapturedStateInstanceMessageSuffixLiteral(instanceMethod, instanceMethodInstructions);
        var concatPairIcall = GetCapturedStateInstanceMessageConcatIcall(instanceMethod, instanceMethodInstructions);

        loweringPlan = new NativeReferenceLoweringPlanArtifact
        {
            PlanKind = "arrayBoxingReferenceArray",
            AssemblyName = assemblyName,
            EntrySubjectId = subjectId,
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



















    private static bool TryBuildAssemblyBoundArrayClearReferenceArrayPlan(
        string assemblyName,
        string subjectId,
        MetadataRegistrationArtifact metadataRegistration,
        IReadOnlyList<CodeRegistrationEntry> methodPointers,
        IReadOnlyList<TypedIlMethodArtifact> methods,
        out NativeReferenceLoweringPlanArtifact loweringPlan)
    {
        loweringPlan = default!;

        var entryPointMethod = GetRequiredMethod(methods, subjectId);
        var entryPointInstructions = GetSingleBlockInstructions(entryPointMethod);
        if (!IsArrayClearReferenceArrayEntryPointShape(entryPointInstructions))
        {
            return false;
        }

        var entryPointRegistration = GetRequiredRegistration(methodPointers, subjectId);
        var wholeArrayClearShape = entryPointInstructions.Count == 17;
        var arrayLength = GetRequiredOperandInt(entryPointInstructions[0]);
        var arrayElementTypeSubjectId = GetRequiredOperandString(entryPointInstructions[1]);
        var arrayLocalIndex = GetRequiredOperandInt(entryPointInstructions[2]);
        var arrayStoreLocalIndex = GetRequiredOperandInt(entryPointInstructions[3]);
        var arrayStoreIndex = GetRequiredOperandInt(entryPointInstructions[4]);
        var storedLiteral = GetRequiredOperandString(entryPointInstructions[5]);
        var arrayClearLocalIndex = GetRequiredOperandInt(entryPointInstructions[7]);
        var clearStartIndex = wholeArrayClearShape ? 0 : GetRequiredOperandInt(entryPointInstructions[8]);
        var clearLength = wholeArrayClearShape ? arrayLength : GetRequiredOperandInt(entryPointInstructions[9]);
        var arrayClearMethodSubjectId = GetRequiredInstructionCallee(entryPointInstructions[wholeArrayClearShape ? 8 : 10], entryPointMethod.SubjectId, wholeArrayClearShape ? 8 : 10);
        var messagePrefixLiteral = GetRequiredOperandString(entryPointInstructions[wholeArrayClearShape ? 9 : 11]);
        var arrayReadLocalIndex = GetRequiredOperandInt(entryPointInstructions[wholeArrayClearShape ? 10 : 12]);
        var arrayReadIndex = GetRequiredOperandInt(entryPointInstructions[wholeArrayClearShape ? 11 : 13]);
        var concatPairIcall = GetRequiredInstructionCallee(entryPointInstructions[wholeArrayClearShape ? 13 : 15], entryPointMethod.SubjectId, wholeArrayClearShape ? 13 : 15);
        var writeLineStringIcall = GetRequiredInstructionCallee(entryPointInstructions[wholeArrayClearShape ? 14 : 16], entryPointMethod.SubjectId, wholeArrayClearShape ? 14 : 16);

        if (arrayLocalIndex != arrayStoreLocalIndex || arrayLocalIndex != arrayClearLocalIndex || arrayLocalIndex != arrayReadLocalIndex)
        {
            throw new InvalidOperationException(
                $"array clear lowering expects '{entryPointMethod.SubjectId}' to reuse one array local across store/clear/read");
        }

        if (clearLength <= 0)
        {
            throw new InvalidOperationException(
                $"array clear lowering expects '{entryPointMethod.SubjectId}' to clear a positive constant number of elements");
        }

        if (clearStartIndex < 0 || arrayStoreIndex < 0 || arrayReadIndex < 0 ||
            clearStartIndex + clearLength > arrayLength ||
            arrayStoreIndex >= arrayLength ||
            arrayReadIndex >= arrayLength)
        {
            throw new InvalidOperationException(
                $"array clear lowering expects '{entryPointMethod.SubjectId}' to use in-range constant clear indices");
        }

        if (arrayStoreIndex < clearStartIndex || arrayStoreIndex >= clearStartIndex + clearLength)
        {
            throw new InvalidOperationException(
                $"array clear lowering expects '{entryPointMethod.SubjectId}' to store the literal inside the cleared range");
        }

        if (arrayReadIndex != arrayStoreIndex)
        {
            throw new InvalidOperationException(
                $"array clear lowering expects '{entryPointMethod.SubjectId}' to read back from the cleared slot that originally held the stored literal");
        }

        if (!IsSupportedArrayClearMethodSubjectId(arrayClearMethodSubjectId))
        {
            throw new InvalidOperationException(
                $"array clear lowering expects '{entryPointMethod.SubjectId}' to call supported System.Array.Clear overload, but found '{arrayClearMethodSubjectId}'");
        }

        loweringPlan = new NativeReferenceLoweringPlanArtifact
        {
            PlanKind = "arrayClearReferenceArray",
            AssemblyName = assemblyName,
            EntrySubjectId = subjectId,
            IncludeHeader = "codegen_bridge.h",
            NativeEntryFunctionName = "RunNativeReference",
            EntrySymbol = entryPointRegistration.Symbol,
            ReferenceTypeToken = CreateTypeTokenLiteral(metadataRegistration, arrayElementTypeSubjectId),
            CapturedFieldToken = "0u",
            EntryMethodToken = FormatCppTokenLiteral(GetRequiredMetadataToken(metadataRegistration, "method", entryPointRegistration.SubjectId)),
            ConsoleWriteLineStringIcall = writeLineStringIcall,
            StringConcatPairIcall = concatPairIcall,
            StoredLiteral = ToCppStringLiteral(storedLiteral),
            StoredLiteralByteCount = Encoding.UTF8.GetByteCount(storedLiteral),
            MessagePrefixLiteral = ToCppStringLiteral(messagePrefixLiteral),
            MessagePrefixLiteralByteCount = Encoding.UTF8.GetByteCount(messagePrefixLiteral),
            ArrayLength = arrayLength,
            ArrayStoreIndex = arrayStoreIndex,
            ClearStartIndex = clearStartIndex,
            ClearLength = clearLength,
            ArrayReadIndex = arrayReadIndex,
        };

        return true;
    }

}
