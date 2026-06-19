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

    private static bool TryBuildAssemblyBoundStaticBoxedIConvertibleCharInvalidCastStub(
        string assemblyName,
        string subjectId,
        MetadataRegistrationArtifact metadataRegistration,
        IReadOnlyDictionary<string, TypedIlMethodArtifact> methodsBySubjectId,
        string stubName,
        out string stub)
    {
        stub = string.Empty;
        if (!methodsBySubjectId.TryGetValue(subjectId, out var method))
        {
            return false;
        }

        if (!TryResolveRuntimeSkeletonBoxedIConvertibleCharInvalidCastShape(
                method,
                methodsBySubjectId,
                out var inputCppType,
                out var outputCppType,
                out var exceptionTypeSubjectId,
                out var sourceTypeName,
                out var targetTypeName))
        {
            return false;
        }

        var exceptionMessage = $"InvalidCast_FromTo: {sourceTypeName} -> {targetTypeName}";
        var model = new ScriptObject
        {
            ["stub_name"] = stubName,
            ["assembly_name_literal"] = ToCppStringLiteral(assemblyName),
            ["input_cpp_type"] = inputCppType,
            ["output_cpp_type"] = outputCppType,
            ["exception_type_token"] = CreateTypeTokenLiteral(metadataRegistration, exceptionTypeSubjectId),
            ["source_type_name_literal"] = ToCppStringLiteral(sourceTypeName),
            ["target_type_name_literal"] = ToCppStringLiteral(targetTypeName),
            ["exception_message_literal"] = ToCppStringLiteral(exceptionMessage),
            ["exception_message_byte_count"] = Encoding.UTF8.GetByteCount(exceptionMessage),
        };
        stub = ScribanTemplateRenderer.RenderTemplate(
            ScribanTemplateRenderer.LoadTemplate(NativeReferenceProofCatalog.RuntimeSkeletonStaticBoxedIConvertibleCharInvalidCastStubTemplateRelativePath),
            model);
        return true;
    }

    private static bool TryBuildAssemblyBoundStaticBoxedValueTypeCharInvalidCastStub(
        string assemblyName,
        string subjectId,
        MetadataRegistrationArtifact metadataRegistration,
        IReadOnlyDictionary<string, TypedIlMethodArtifact> methodsBySubjectId,
        string stubName,
        out string stub)
    {
        stub = string.Empty;
        if (!methodsBySubjectId.TryGetValue(subjectId, out var method))
        {
            return false;
        }

        if (!TryResolveRuntimeSkeletonBoxedValueTypeCharInvalidCastShape(
                method,
                methodsBySubjectId,
                out var inputCppType,
                out var outputCppType,
                out var boxedValueTypeSubjectId,
                out var exceptionTypeSubjectId,
                out var sourceTypeName,
                out var targetTypeName,
                out var inputSize))
        {
            return false;
        }

        var exceptionMessage = $"InvalidCast_FromTo: {sourceTypeName} -> {targetTypeName}";
        var model = new ScriptObject
        {
            ["stub_name"] = stubName,
            ["assembly_name_literal"] = ToCppStringLiteral(assemblyName),
            ["input_cpp_type"] = inputCppType,
            ["output_cpp_type"] = outputCppType,
            ["boxed_value_type_token"] = CreateTypeTokenLiteral(metadataRegistration, boxedValueTypeSubjectId),
            ["exception_type_token"] = CreateTypeTokenLiteral(metadataRegistration, exceptionTypeSubjectId),
            ["source_type_name_literal"] = ToCppStringLiteral(sourceTypeName),
            ["target_type_name_literal"] = ToCppStringLiteral(targetTypeName),
            ["exception_message_literal"] = ToCppStringLiteral(exceptionMessage),
            ["exception_message_byte_count"] = Encoding.UTF8.GetByteCount(exceptionMessage),
            ["input_size"] = inputSize,
        };
        stub = ScribanTemplateRenderer.RenderTemplate(
            ScribanTemplateRenderer.LoadTemplate(NativeReferenceProofCatalog.RuntimeSkeletonStaticBoxedValueTypeCharInvalidCastStubTemplateRelativePath),
            model);
        return true;
    }
}
