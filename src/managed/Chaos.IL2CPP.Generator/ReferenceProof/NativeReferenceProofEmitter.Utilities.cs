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
private static IReadOnlyDictionary<string, string> BuildAssemblyPathsByName(ManagedClosureManifestArtifact closureManifest)
    {
        var pathsByAssemblyName = new Dictionary<string, string>(StringComparer.Ordinal);
        foreach (var resolvedAssembly in closureManifest.ResolvedAssemblies ?? [])
        {
            if (!string.IsNullOrWhiteSpace(resolvedAssembly.AssemblyName) &&
                !string.IsNullOrWhiteSpace(resolvedAssembly.Path))
            {
                pathsByAssemblyName[resolvedAssembly.AssemblyName] = Path.GetFullPath(resolvedAssembly.Path);
            }
        }

        if (pathsByAssemblyName.Count > 0)
        {
            return pathsByAssemblyName;
        }

        pathsByAssemblyName[closureManifest.AssemblyName] = Path.GetFullPath(closureManifest.InputAssemblyPath);

        foreach (var additionalAssemblyPath in closureManifest.AdditionalAssemblyPaths ?? [])
        {
            var normalizedPath = Path.GetFullPath(additionalAssemblyPath);
            var assemblyName = Path.GetFileNameWithoutExtension(normalizedPath);
            if (!string.IsNullOrWhiteSpace(assemblyName))
            {
                pathsByAssemblyName[assemblyName] = normalizedPath;
            }
        }

        return pathsByAssemblyName;
    }

    private static bool TryCreateAssemblyMetadataCache(
        string assemblyName,
        string assemblyPath,
        out AssemblyMetadataCache cache)
    {
        cache = null!;
        var stream = File.OpenRead(assemblyPath);
        var peReader = new PEReader(stream);
        if (!peReader.HasMetadata)
        {
            peReader.Dispose();
            stream.Dispose();
            return false;
        }

        var metadataReader = peReader.GetMetadataReader();
        var typeDefinitionHandlesBySubjectId = new Dictionary<string, TypeDefinitionHandle>(StringComparer.Ordinal);
        foreach (var typeHandle in metadataReader.TypeDefinitions)
        {
            if (!TryResolveTypeDefinitionIdentity(metadataReader, assemblyName, typeHandle, out var typeIdentity))
            {
                continue;
            }

            typeDefinitionHandlesBySubjectId[typeIdentity.SubjectId] = typeHandle;
        }

        cache = new AssemblyMetadataCache(
            assemblyName,
            stream,
            peReader,
            metadataReader,
            typeDefinitionHandlesBySubjectId);
        return true;
    }

    private static bool TryResolveTypeDefinitionIdentity(
        MetadataReader metadataReader,
        string currentAssemblyName,
        TypeDefinitionHandle handle,
        out MetadataTypeIdentity typeIdentity)
    {
        typeIdentity = default!;
        var typeDefinition = metadataReader.GetTypeDefinition(handle);
        var namespaceName = metadataReader.GetString(typeDefinition.Namespace);
        var typeName = metadataReader.GetString(typeDefinition.Name);
        var declaringTypeHandle = typeDefinition.GetDeclaringType();
        if (!declaringTypeHandle.IsNil &&
            TryResolveTypeDefinitionIdentity(metadataReader, currentAssemblyName, declaringTypeHandle, out var declaringTypeIdentity))
        {
            var nestedTypeName = $"{declaringTypeIdentity.TypeName}+{typeName}";
            var nestedDisplayName = string.IsNullOrEmpty(namespaceName)
                ? nestedTypeName
                : $"{namespaceName}.{nestedTypeName}";
            typeIdentity = new MetadataTypeIdentity(
                currentAssemblyName,
                namespaceName,
                nestedTypeName,
                nestedDisplayName,
                $"{currentAssemblyName}/{nestedDisplayName}");
            return true;
        }

        var displayName = string.IsNullOrEmpty(namespaceName)
            ? typeName
            : $"{namespaceName}.{typeName}";
        typeIdentity = new MetadataTypeIdentity(
            currentAssemblyName,
            namespaceName,
            typeName,
            displayName,
            $"{currentAssemblyName}/{displayName}");
        return true;
    }

    private static int GetMethodParameterCount(MetadataReader metadataReader, MethodDefinition methodDefinition)
    {
        return methodDefinition.GetParameters()
            .Select(parameterHandle => metadataReader.GetParameter(parameterHandle))
            .Count(parameter => parameter.SequenceNumber > 0);
    }

    private static string GetMetadataMethodName(string methodSubjectId)
    {
        var methodName = GetMethodName(methodSubjectId);
        var genericArgumentIndex = methodName.IndexOf('<');
        return genericArgumentIndex >= 0
            ? methodName[..genericArgumentIndex]
            : methodName;
    }

    private static string GetAssemblyNameFromSubjectId(string subjectId)
    {
        var separatorIndex = subjectId.IndexOf('/');
        if (separatorIndex <= 0)
        {
            throw new InvalidOperationException($"failed to extract assembly name from subject id '{subjectId}'");
        }

        return subjectId[..separatorIndex];
    }

    private static bool TryGetMetadataTokenPrefix(string registrationKind, out uint tokenPrefix)
    {
        switch (registrationKind)
        {
            case "type":
                tokenPrefix = 0x02000000u;
                return true;
            case "field":
                tokenPrefix = 0x04000000u;
                return true;
            case "method":
                tokenPrefix = 0x06000000u;
                return true;
            default:
                tokenPrefix = 0u;
                return false;
        }
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
        var builder = new StringBuilder(value.Length + 2);
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

    private static bool TryRenderValueTypeManagedInvokeStub(
        Template template,
        string assemblyName,
        string subjectId,
        MetadataRegistrationArtifact metadataRegistration,
        string stubName,
        string contractId,
        string thisFieldDeclaration,
        string thisValidationStatement,
        string thisArgumentExpression,
        IReadOnlyList<RuntimeSkeletonManagedInvokePointerArgShape> argumentShapes,
        string argumentValidationStatements,
        int argumentCount,
        int argumentStorageSize,
        RuntimeSkeletonManagedInvokeReturnContract returnShape,
        out string stub)
    {
        stub = string.Empty;
        string targetMethodTokenLiteral;
        try
        {
            targetMethodTokenLiteral = FormatCppTokenLiteral(GetRequiredMetadataToken(metadataRegistration, "method", subjectId));
        }
        catch
        {
            return false;
        }

        var model = new ScriptObject
        {
            ["stub_name"] = stubName,
            ["contract_id"] = contractId,
            ["target_assembly_name_literal"] = ToCppStringLiteral(assemblyName),
            ["target_method_token"] = targetMethodTokenLiteral,
            ["this_field_declaration"] = thisFieldDeclaration,
            ["this_validation_statement"] = thisValidationStatement,
            ["this_argument_expression"] = thisArgumentExpression,
            ["arg_field_declarations"] = string.Join("\n    ", argumentShapes.Select(shape => shape.FieldDeclaration)),
            ["arg_validation_statements"] = argumentValidationStatements,
            ["arg_refs"] = string.Join(",\n        ", argumentShapes.Select(shape => shape.ArgReferenceExpression)),
            ["argc"] = argumentCount,
            ["arg_storage_size"] = argumentStorageSize,
            ["return_managed_type"] = returnShape.ManagedType,
            ["return_field_declarations"] = returnShape.FieldDeclarations,
            ["return_value_validation_statement"] = returnShape.ValidationStatement,
            ["return_value_declaration"] = returnShape.ReturnValueDeclaration,
            ["return_value_argument"] = returnShape.ReturnValueArgument,
            ["return_value_size"] = returnShape.ReturnValueSizeExpression,
            ["return_value_is_indirect"] = returnShape.ReturnValueIsIndirectExpression,
        };
        stub = ScribanTemplateRenderer.RenderTemplate(template, model);
        return true;
    }

    private static TypedIlMethodArtifact GetRequiredMethod(
        IReadOnlyList<TypedIlMethodArtifact> methods,
        string subjectId)
    {
        var methodLookup = GetMethodLookup(methods);
        if (methodLookup.TryGetValue(subjectId, out var method))
        {
            return method;
        }

        throw new InvalidOperationException($"missing required typed-il method for '{subjectId}'");
    }

    private static IReadOnlyDictionary<string, TypedIlMethodArtifact> GetMethodLookup(
        IReadOnlyList<TypedIlMethodArtifact> methods)
    {
        return MethodLookupCache.GetValue(
            methods,
            static items => items.ToDictionary(method => method.SubjectId, StringComparer.Ordinal));
    }

    private static IReadOnlyDictionary<string, string> BuildCanonicalSubjectIdLookup(IReadOnlyList<string> subjectIds)
    {
        return subjectIds
            .Where(subjectId => !string.IsNullOrWhiteSpace(subjectId))
            .Distinct(StringComparer.Ordinal)
            .ToDictionary(
                subjectId => subjectId,
                ManagedNaming.CanonicalizeSubjectId,
                StringComparer.Ordinal);
    }

    private static string ResolveCanonicalSubjectId(
        IReadOnlyDictionary<string, string> canonicalSubjectIds,
        string subjectId)
    {
        return canonicalSubjectIds.TryGetValue(subjectId, out var canonicalSubjectId)
            ? canonicalSubjectId
            : ManagedNaming.CanonicalizeSubjectId(subjectId);
    }

    private static IReadOnlyDictionary<string, CodeRegistrationEntry> GetCodeRegistrationLookup(
        IReadOnlyList<CodeRegistrationEntry> registrations)
    {
        return CodeRegistrationLookupCache.GetValue(
            registrations,
            static items => items.ToDictionary(registration => registration.SubjectId, StringComparer.Ordinal));
    }

    private static MetadataRegistrationLookup GetMetadataRegistrationLookup(
        IReadOnlyList<MetadataRegistrationEntry> registrations)
    {
        return MetadataRegistrationLookupCache.GetValue(
            registrations,
            static items => new MetadataRegistrationLookup(items));
    }

    private static IReadOnlyList<MetadataRegistrationEntry> GetMetadataRegistrationsForKind(
        IReadOnlyList<MetadataRegistrationEntry> registrations,
        string registrationKind)
    {
        return GetMetadataRegistrationLookup(registrations).GetRegistrationsForKind(registrationKind);
    }

    private static CodeRegistrationEntry GetRequiredRegistration(
        IReadOnlyList<CodeRegistrationEntry> registrations,
        string subjectId)
    {
        var registrationLookup = GetCodeRegistrationLookup(registrations);
        if (registrationLookup.TryGetValue(subjectId, out var registration))
        {
            return registration;
        }

        throw new InvalidOperationException($"missing required method pointer registration for '{subjectId}'");
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
