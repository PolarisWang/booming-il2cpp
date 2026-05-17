using System.Collections.Immutable;
using System.Reflection;
using System.Reflection.Metadata;
using System.Reflection.Metadata.Ecma335;
using System.Reflection.PortableExecutable;
using System.Text;
using Chaos.IL2CPP.Contracts;

namespace Chaos.IL2CPP.Loader;

public sealed partial class LoaderStage
{
    private static MethodReferenceSummary ResolveMethodReference(
        MetadataReader metadataReader,
        MetadataTypeResolver typeResolver,
        IReadOnlyDictionary<TypeDefinitionHandle, ManagedTypeModel> typeModels,
        IReadOnlyDictionary<MethodDefinitionHandle, ManagedTypeModel> methodOwners,
        EntityHandle handle)
    {
        return handle.Kind switch
        {
            HandleKind.MethodDefinition => DescribeMethodDefinition(
                metadataReader,
                typeResolver,
                typeModels,
                methodOwners,
                typeResolver.CurrentAssemblyName,
                (MethodDefinitionHandle)handle),
            HandleKind.MemberReference => DescribeMemberReferenceMethod(metadataReader, typeResolver, (MemberReferenceHandle)handle),
            HandleKind.MethodSpecification => DescribeMethodSpecification(
                metadataReader,
                typeResolver,
                typeModels,
                methodOwners,
                (MethodSpecificationHandle)handle),
            _ => throw new NotSupportedException($"unsupported method reference handle kind: {handle.Kind}"),
        };
    }

    private static FieldReferenceSummary ResolveFieldReference(
        MetadataReader metadataReader,
        MetadataTypeResolver typeResolver,
        IReadOnlyDictionary<FieldDefinitionHandle, ManagedTypeModel> fieldOwners,
        EntityHandle handle)
    {
        return handle.Kind switch
        {
            HandleKind.FieldDefinition => DescribeFieldDefinition(
                metadataReader,
                typeResolver,
                fieldOwners,
                typeResolver.CurrentAssemblyName,
                (FieldDefinitionHandle)handle),
            HandleKind.MemberReference => DescribeMemberReferenceField(metadataReader, typeResolver, (MemberReferenceHandle)handle),
            _ => throw new NotSupportedException($"unsupported field reference handle kind: {handle.Kind}"),
        };
    }

    private static MethodSummary DescribeMethodDefinition(
        MetadataReader metadataReader,
        MetadataTypeResolver typeResolver,
        IReadOnlyDictionary<TypeDefinitionHandle, ManagedTypeModel> typeModels,
        IReadOnlyDictionary<MethodDefinitionHandle, ManagedTypeModel> methodOwners,
        string assemblyName,
        MethodDefinitionHandle handle)
    {
        var methodDefinition = metadataReader.GetMethodDefinition(handle);
        var declaringType = methodOwners.TryGetValue(handle, out var existingDeclaringType)
            ? existingDeclaringType
            : ResolveOwningTypeModel(metadataReader, typeResolver, typeModels, handle);
        var signature = methodDefinition.DecodeSignature(typeResolver.TypeNameProvider, null);
        var parameterTypes = signature.ParameterTypes.ToArray();
        var parameterNames = methodDefinition
            .GetParameters()
            .Select(parameterHandle => metadataReader.GetParameter(parameterHandle))
            .Where(parameter => parameter.SequenceNumber > 0)
            .OrderBy(parameter => parameter.SequenceNumber)
            .Select((parameter, index) =>
            {
                var name = metadataReader.GetString(parameter.Name);
                return string.IsNullOrEmpty(name) ? $"arg{index}" : name;
            })
            .ToArray();

        var parameterFlags = methodDefinition
            .GetParameters()
            .Select(parameterHandle => metadataReader.GetParameter(parameterHandle))
            .Where(parameter => parameter.SequenceNumber > 0)
            .OrderBy(parameter => parameter.SequenceNumber)
            .Select(parameter => (int)parameter.Attributes)
            .ToArray();

        var parameters = parameterTypes
            .Select((parameterType, index) => new ManagedParameterModel
            {
                Name = index < parameterNames.Length ? parameterNames[index] : $"arg{index}",
                Type = parameterType,
                Attributes = index < parameterFlags.Length ? parameterFlags[index] : 0,
            })
            .ToList();

        var methodName = metadataReader.GetString(methodDefinition.Name);
        var genericParameterCount = methodDefinition.GetGenericParameters().Count;
        var subjectId = ManagedNaming.CreateMethodSubjectId(
            declaringType.SubjectId,
            methodName,
            signature.ReturnType,
            parameterTypes,
            genericParameterCount);
        var import = TryDescribeMethodImport(metadataReader, methodDefinition);
        var isPreserved = HasPreserveAttribute(metadataReader, handle);
        var isUnmanagedCallersOnly = HasUnmanagedCallersOnlyAttribute(metadataReader, handle);
        var isPreserveSig = HasPreserveSigAttribute(metadataReader, handle);

        return new MethodSummary
        {
            AssemblyName = assemblyName,
            DeclaringTypeSubjectId = declaringType.SubjectId,
            DeclaringTypeDisplayName = declaringType.DisplayName,
            Name = methodName,
            GenericParameterCount = genericParameterCount,
            ReturnType = signature.ReturnType,
            SubjectId = subjectId,
            DefinitionSubjectId = subjectId,
            ParameterTypes = parameterTypes,
            Signature = ManagedNaming.CreateMethodSignature(signature.ReturnType, declaringType.DisplayName, methodName, parameterTypes),
            IsStatic = methodDefinition.Attributes.HasFlag(MethodAttributes.Static),
            IsVirtual = methodDefinition.Attributes.HasFlag(MethodAttributes.Virtual),
            IsFinal = methodDefinition.Attributes.HasFlag(MethodAttributes.Final),
            IsPreserved = isPreserved,
            IsUnmanagedCallersOnly = isUnmanagedCallersOnly,
            IsPreserveSig = isPreserveSig,
            MetadataToken = MetadataTokens.GetToken(handle),
            Parameters = parameters,
            Import = import,
            Substitutions = ImmutableDictionary<string, string>.Empty,
        };
    }

    private static ManagedImportModel? TryDescribeMethodImport(
        MetadataReader metadataReader,
        MethodDefinition methodDefinition)
    {
        if (!methodDefinition.Attributes.HasFlag(MethodAttributes.PinvokeImpl))
        {
            return null;
        }

        var import = methodDefinition.GetImport();
        var moduleReference = metadataReader.GetModuleReference(import.Module);
        var moduleName = metadataReader.GetString(moduleReference.Name);
        var entryPointName = metadataReader.GetString(import.Name);

        // __Internal: the symbol is expected to be linked statically at compile time,
        // so no LoadLibrary/GetProcAddress is needed at runtime. Signal this to the
        // codegen layer by clearing the module name.
        if (string.Equals(moduleName, "__Internal", StringComparison.Ordinal))
        {
            moduleName = "";
        }

        var attributes = (int)import.Attributes;
        var isSuppressGCTransition = HasSuppressGCTransitionAttribute(metadataReader, methodDefinition);

        return new ManagedImportModel
        {
            ModuleName = moduleName,
            EntryPointName = entryPointName,
            CallingConvention = attributes & (int)MethodImportAttributes.CallingConventionMask,
            CharSet = attributes & (int)MethodImportAttributes.CharSetMask,
            SetLastError = import.Attributes.HasFlag(MethodImportAttributes.SetLastError),
            IsSuppressGCTransition = isSuppressGCTransition,
        };
    }

    /// <summary>
    /// Checks whether the given method is annotated with
    /// <see cref="System.Runtime.InteropServices.SuppressGCTransitionAttribute"/>.
    /// </summary>
    private static bool HasSuppressGCTransitionAttribute(
        MetadataReader metadataReader,
        MethodDefinition methodDefinition)
    {
        const string suppressGCTransitionAttributeFullName =
            "System.Runtime.InteropServices.SuppressGCTransitionAttribute";

        foreach (var attributeHandle in methodDefinition.GetCustomAttributes())
        {
            if (TryGetAttributeTypeName(metadataReader, attributeHandle, out var namespaceName, out var typeName) &&
                string.Equals($"{namespaceName}.{typeName}", suppressGCTransitionAttributeFullName, StringComparison.Ordinal))
            {
                return true;
            }
        }

        return false;
    }

    private static MethodReferenceSummary DescribeMemberReferenceMethod(
        MetadataReader metadataReader,
        MetadataTypeResolver typeResolver,
        MemberReferenceHandle handle)
    {
        var memberReference = metadataReader.GetMemberReference(handle);
        var declaringType = ResolveMemberReferenceDeclaringType(metadataReader, typeResolver, memberReference);
        var signature = memberReference.DecodeMethodSignature(
            typeResolver.TypeNameProvider,
            typeResolver.CreateTypeNameContext(declaringType));
        var parameterTypes = signature.ParameterTypes.ToArray();
        var methodName = metadataReader.GetString(memberReference.Name);
        var definitionSignature = ResolveMemberReferenceDefinitionSignature(metadataReader, typeResolver, memberReference);
        var definitionParameterTypes = definitionSignature.ParameterTypes.ToArray();

        return new MethodReferenceSummary
        {
            AssemblyName = declaringType.AssemblyName,
            DeclaringTypeSubjectId = declaringType.SubjectId,
            DeclaringTypeDisplayName = declaringType.DisplayName,
            Name = methodName,
            GenericParameterCount = GetMemberReferenceGenericParameterCount(memberReference),
            SubjectId = ManagedNaming.CreateMethodSubjectId(
                declaringType.SubjectId,
                methodName,
                signature.ReturnType,
                parameterTypes,
                GetMemberReferenceGenericParameterCount(memberReference)),
            DefinitionSubjectId = ManagedNaming.CreateMethodSubjectId(
                declaringType.DefinitionSubjectId,
                methodName,
                definitionSignature.ReturnType,
                definitionParameterTypes,
                GetDefinitionGenericParameterCount(metadataReader, memberReference.Parent)),
            ReturnType = signature.ReturnType,
            ParameterTypes = parameterTypes,
            MetadataToken = MetadataTokens.GetToken(handle),
            Substitutions = CreateSubstitutionMap(declaringType.TypeArguments, []),
        };
    }

    private static TypeIdentity ResolveMemberReferenceDeclaringType(
        MetadataReader metadataReader,
        MetadataTypeResolver typeResolver,
        MemberReference memberReference)
    {
        if (memberReference.Parent.Kind == HandleKind.MethodDefinition)
        {
            var parentMethod = metadataReader.GetMethodDefinition((MethodDefinitionHandle)memberReference.Parent);
            return typeResolver.ResolveTypeIdentity(parentMethod.GetDeclaringType());
        }

        return typeResolver.ResolveTypeIdentity(memberReference.Parent);
    }

    private static MethodSignature<string> ResolveMemberReferenceDefinitionSignature(
        MetadataReader metadataReader,
        MetadataTypeResolver typeResolver,
        MemberReference memberReference)
    {
        if (memberReference.Parent.Kind == HandleKind.MethodDefinition)
        {
            return metadataReader
                .GetMethodDefinition((MethodDefinitionHandle)memberReference.Parent)
                .DecodeSignature(typeResolver.TypeNameProvider, null);
        }

        return memberReference.DecodeMethodSignature(typeResolver.TypeNameProvider, null);
    }

    private static MethodReferenceSummary DescribeMethodSpecification(
        MetadataReader metadataReader,
        MetadataTypeResolver typeResolver,
        IReadOnlyDictionary<TypeDefinitionHandle, ManagedTypeModel> typeModels,
        IReadOnlyDictionary<MethodDefinitionHandle, ManagedTypeModel> methodOwners,
        MethodSpecificationHandle handle)
    {
        var methodSpecification = metadataReader.GetMethodSpecification(handle);
        var methodArguments = methodSpecification.DecodeSignature(typeResolver.TypeNameProvider, null).ToImmutableArray();
        var baseReference = ResolveMethodReference(metadataReader, typeResolver, typeModels, methodOwners, methodSpecification.Method);
        var hasClosedMethodArguments = methodArguments.Any(argument => !argument.StartsWith("!", StringComparison.Ordinal));
        var methodName = hasClosedMethodArguments
            ? ManagedNaming.CreateGenericMethodName(baseReference.Name, methodArguments)
            : baseReference.Name;
        var substitutions = baseReference.Substitutions.SetItems(CreateSubstitutionMap([], methodArguments));

        MethodSignature<string> closedSignature = methodSpecification.Method.Kind switch
        {
            HandleKind.MethodDefinition => metadataReader
                .GetMethodDefinition((MethodDefinitionHandle)methodSpecification.Method)
                .DecodeSignature(typeResolver.TypeNameProvider, new SignatureContext<string>([], methodArguments)),
            HandleKind.MemberReference => metadataReader
                .GetMemberReference((MemberReferenceHandle)methodSpecification.Method)
                .DecodeMethodSignature(
                    typeResolver.TypeNameProvider,
                    new SignatureContext<string>(
                        substitutions
                            .Where(pair => pair.Key.StartsWith("!", StringComparison.Ordinal) && !pair.Key.StartsWith("!!", StringComparison.Ordinal))
                            .OrderBy(pair => pair.Key, StringComparer.Ordinal)
                            .Select(pair => pair.Value)
                            .ToImmutableArray(),
                        methodArguments)),
            _ => throw new NotSupportedException($"unsupported method specification target kind: {methodSpecification.Method.Kind}"),
        };

        var parameterTypes = closedSignature.ParameterTypes.ToArray();

        return new MethodReferenceSummary
        {
            AssemblyName = baseReference.AssemblyName,
            DeclaringTypeSubjectId = baseReference.DeclaringTypeSubjectId,
            DeclaringTypeDisplayName = baseReference.DeclaringTypeDisplayName,
            Name = methodName,
            GenericParameterCount = baseReference.GenericParameterCount,
            SubjectId = ManagedNaming.CreateMethodSubjectId(
                baseReference.DeclaringTypeSubjectId,
                methodName,
                closedSignature.ReturnType,
                parameterTypes,
                hasClosedMethodArguments ? 0 : baseReference.GenericParameterCount),
            DefinitionSubjectId = baseReference.DefinitionSubjectId,
            ReturnType = closedSignature.ReturnType,
            ParameterTypes = parameterTypes,
            MetadataToken = MetadataTokens.GetToken(handle),
            Substitutions = substitutions,
        };
    }

    private static int GetMemberReferenceGenericParameterCount(MemberReference memberReference)
    {
        return memberReference.Signature.IsNil
            ? 0
            : memberReference.DecodeMethodSignature(new GenericArityTypeProvider(), null).GenericParameterCount;
    }

    private static int GetDefinitionGenericParameterCount(
        MetadataReader metadataReader,
        EntityHandle parentHandle)
    {
        return parentHandle.Kind switch
        {
            HandleKind.MethodDefinition => metadataReader
                .GetMethodDefinition((MethodDefinitionHandle)parentHandle)
                .GetGenericParameters()
                .Count,
            _ => 0,
        };
    }

    private static FieldReferenceSummary DescribeFieldDefinition(
        MetadataReader metadataReader,
        MetadataTypeResolver typeResolver,
        IReadOnlyDictionary<FieldDefinitionHandle, ManagedTypeModel> fieldOwners,
        string assemblyName,
        FieldDefinitionHandle handle)
    {
        var fieldDefinition = metadataReader.GetFieldDefinition(handle);
        var declaringType = fieldOwners[handle];
        var fieldName = metadataReader.GetString(fieldDefinition.Name);

        return new FieldReferenceSummary
        {
            AssemblyName = assemblyName,
            DeclaringTypeSubjectId = declaringType.SubjectId,
            Name = fieldName,
            SubjectId = ManagedNaming.CreateFieldSubjectId(declaringType.SubjectId, fieldName),
            DefinitionSubjectId = ManagedNaming.CreateFieldSubjectId(declaringType.SubjectId, fieldName),
            FieldType = fieldDefinition.DecodeSignature(typeResolver.TypeNameProvider, null),
            IsStatic = fieldDefinition.Attributes.HasFlag(FieldAttributes.Static),
            IsThreadStatic = HasThreadStaticAttribute(metadataReader, handle),
            MetadataToken = MetadataTokens.GetToken(handle),
            Substitutions = ImmutableDictionary<string, string>.Empty,
        };
    }

    private static FieldReferenceSummary DescribeMemberReferenceField(
        MetadataReader metadataReader,
        MetadataTypeResolver typeResolver,
        MemberReferenceHandle handle)
    {
        var memberReference = metadataReader.GetMemberReference(handle);
        var declaringType = typeResolver.ResolveTypeIdentity(memberReference.Parent);
        var fieldName = metadataReader.GetString(memberReference.Name);
        var signatureContext = typeResolver.CreateTypeNameContext(declaringType);

        return new FieldReferenceSummary
        {
            AssemblyName = declaringType.AssemblyName,
            DeclaringTypeSubjectId = declaringType.SubjectId,
            Name = fieldName,
            SubjectId = ManagedNaming.CreateFieldSubjectId(declaringType.SubjectId, fieldName),
            DefinitionSubjectId = ManagedNaming.CreateFieldSubjectId(declaringType.DefinitionSubjectId, fieldName),
            FieldType = memberReference.DecodeFieldSignature(typeResolver.TypeNameProvider, signatureContext),
            MetadataToken = MetadataTokens.GetToken(handle),
            Substitutions = CreateSubstitutionMap(declaringType.TypeArguments, []),
        };
    }

    private static bool HasThreadStaticAttribute(
        MetadataReader metadataReader,
        FieldDefinitionHandle fieldHandle)
    {
        const string threadStaticAttributeFullName = "System.ThreadStaticAttribute";
        var fieldDefinition = metadataReader.GetFieldDefinition(fieldHandle);

        foreach (var attributeHandle in fieldDefinition.GetCustomAttributes())
        {
            if (TryGetAttributeTypeName(metadataReader, attributeHandle, out var namespaceName, out var typeName) &&
                string.Equals($"{namespaceName}.{typeName}", threadStaticAttributeFullName, StringComparison.Ordinal))
            {
                return true;
            }
        }

        return false;
    }

    private static bool HasUnmanagedCallersOnlyAttribute(
        MetadataReader metadataReader,
        MethodDefinitionHandle methodHandle)
    {
        const string unmanagedCallersOnlyAttributeFullName = "System.Runtime.InteropServices.UnmanagedCallersOnlyAttribute";
        var methodDefinition = metadataReader.GetMethodDefinition(methodHandle);

        foreach (var attributeHandle in methodDefinition.GetCustomAttributes())
        {
            if (TryGetAttributeTypeName(metadataReader, attributeHandle, out var namespaceName, out var typeName) &&
                string.Equals($"{namespaceName}.{typeName}", unmanagedCallersOnlyAttributeFullName, StringComparison.Ordinal))
            {
                return true;
            }
        }

        return false;
    }

    /// <summary>
    /// Checks whether the given method is annotated with
    /// <see cref="System.Runtime.InteropServices.PreserveSigAttribute"/>.
    /// In .NET 5+, COM interface methods default to PreserveSig=true; this attribute
    /// is only present when explicitly applied. Absence implies the default (true).
    /// </summary>
    private static bool HasPreserveSigAttribute(
        MetadataReader metadataReader,
        MethodDefinitionHandle methodHandle)
    {
        const string preserveSigAttributeFullName = "System.Runtime.InteropServices.PreserveSigAttribute";
        var methodDefinition = metadataReader.GetMethodDefinition(methodHandle);

        foreach (var attributeHandle in methodDefinition.GetCustomAttributes())
        {
            if (TryGetAttributeTypeName(metadataReader, attributeHandle, out var namespaceName, out var typeName) &&
                string.Equals($"{namespaceName}.{typeName}", preserveSigAttributeFullName, StringComparison.Ordinal))
            {
                return true;
            }
        }

        return false;
    }

    private static bool TryGetAttributeTypeName(
        MetadataReader metadataReader,
        CustomAttributeHandle attributeHandle,
        out string namespaceName,
        out string typeName)
    {
        var attribute = metadataReader.GetCustomAttribute(attributeHandle);
        return TryGetAttributeTypeName(metadataReader, attribute.Constructor, out namespaceName, out typeName);
    }

    private static bool TryGetAttributeTypeName(
        MetadataReader metadataReader,
        EntityHandle constructorHandle,
        out string namespaceName,
        out string typeName)
    {
        switch (constructorHandle.Kind)
        {
            case HandleKind.MemberReference:
                var memberReference = metadataReader.GetMemberReference((MemberReferenceHandle)constructorHandle);
                return TryGetTypeName(metadataReader, memberReference.Parent, out namespaceName, out typeName);
            case HandleKind.MethodDefinition:
                var methodDefinition = metadataReader.GetMethodDefinition((MethodDefinitionHandle)constructorHandle);
                return TryGetTypeName(metadataReader, methodDefinition.GetDeclaringType(), out namespaceName, out typeName);
            default:
                namespaceName = string.Empty;
                typeName = string.Empty;
                return false;
        }
    }

    private static bool TryGetTypeName(
        MetadataReader metadataReader,
        EntityHandle typeHandle,
        out string namespaceName,
        out string typeName)
    {
        switch (typeHandle.Kind)
        {
            case HandleKind.TypeReference:
                var typeReference = metadataReader.GetTypeReference((TypeReferenceHandle)typeHandle);
                namespaceName = metadataReader.GetString(typeReference.Namespace);
                typeName = metadataReader.GetString(typeReference.Name);
                return true;
            case HandleKind.TypeDefinition:
                var typeDefinition = metadataReader.GetTypeDefinition((TypeDefinitionHandle)typeHandle);
                namespaceName = metadataReader.GetString(typeDefinition.Namespace);
                typeName = metadataReader.GetString(typeDefinition.Name);
                return true;
            default:
                namespaceName = string.Empty;
                typeName = string.Empty;
                return false;
        }
    }

    /// <summary>
    /// Extracts the GUID string from a type's <see cref="System.Runtime.InteropServices.GuidAttribute"/>,
    /// if present. Returns the GUID string (e.g., "ABCDEF01-2345-6789-ABCD-EF0123456789") or null.
    /// </summary>
    internal static string? TryGetComInterfaceGuid(
        MetadataReader metadataReader,
        TypeDefinitionHandle typeHandle)
    {
        const string guidAttributeFullName = "System.Runtime.InteropServices.GuidAttribute";
        var typeDefinition = metadataReader.GetTypeDefinition(typeHandle);

        foreach (var attributeHandle in typeDefinition.GetCustomAttributes())
        {
            if (TryGetAttributeTypeName(metadataReader, attributeHandle, out var ns, out var name) &&
                string.Equals($"{ns}.{name}", guidAttributeFullName, StringComparison.Ordinal))
            {
                // Decode the single string constructor argument from the CA blob.
                var attribute = metadataReader.GetCustomAttribute(attributeHandle);
                var blob = metadataReader.GetBlobBytes(attribute.Value);
                return DecodeGuidAttributeString(blob);
            }
        }

        return null;
    }

    /// <summary>
    /// Decodes a GuidAttribute custom attribute blob and returns the GUID string.
    /// Blob format: 2-byte prolog (0x0001) + SerString (length-prefixed UTF8).
    /// </summary>
    private static string? DecodeGuidAttributeString(byte[] blob)
    {
        if (blob.Length < 3) return null;          // prolog + at least 1 byte of string header

        // Skip 2-byte prolog (always 0x01 0x00 for standard attributes).
        int pos = 2;

        // SerString: 0xFF is null; otherwise a packed unsigned length + UTF8.
        if (blob[pos] == 0xFF) return null;

        // Decode packed unsigned length (ECMA 335 II.23.2: compressed unsigned int).
        if (!TryDecodeCompressedUInt32(blob, ref pos, out uint length))
            return null;

        if (length == 0 || pos + length > blob.Length)
            return null;

        return System.Text.Encoding.UTF8.GetString(blob, pos, (int)length);
    }

    /// <summary>
    /// Decodes an ECMA 335 compressed unsigned integer at position pos.
    /// Advances pos past the encoded bytes.
    /// </summary>
    private static bool TryDecodeCompressedUInt32(byte[] blob, ref int pos, out uint value)
    {
        value = 0;
        if (pos >= blob.Length) return false;

        byte first = blob[pos];
        if ((first & 0x80) == 0)
        {
            // 1-byte encoding: 0xxx xxxx (7 bits)
            value = first;
            pos += 1;
            return true;
        }
        else if ((first & 0xC0) == 0x80)
        {
            // 2-byte encoding: 10xx xxxx xxxx xxxx (14 bits)
            if (pos + 2 > blob.Length) return false;
            value = (uint)((first & 0x3F) << 8) | blob[pos + 1];
            pos += 2;
            return true;
        }
        else if ((first & 0xE0) == 0xC0)
        {
            // 4-byte encoding: 110x xxxx xxxx xxxx xxxx xxxx xxxx xxxx (29 bits)
            if (pos + 4 > blob.Length) return false;
            value = (uint)((first & 0x1F) << 24) | (uint)(blob[pos + 1] << 16)
                  | (uint)(blob[pos + 2] << 8) | blob[pos + 3];
            pos += 4;
            return true;
        }

        return false;
    }

}
