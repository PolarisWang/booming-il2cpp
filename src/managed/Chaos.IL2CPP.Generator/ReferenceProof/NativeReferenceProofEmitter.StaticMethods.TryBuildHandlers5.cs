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

}
