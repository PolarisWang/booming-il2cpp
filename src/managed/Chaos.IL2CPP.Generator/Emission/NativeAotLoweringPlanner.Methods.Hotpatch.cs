using System;
using System.Collections.Generic;
using System.Collections.Immutable;
using System.Diagnostics;
using System.Globalization;
using System.IO;
using System.Linq;
using System.Reflection;
using System.Reflection.Metadata;
using System.Reflection.Metadata.Ecma335;
using System.Reflection.PortableExecutable;
using System.Runtime.ExceptionServices;
using System.Text;
using System.Text.Json;
using System.Threading.Tasks;
using Chaos.IL2CPP.Contracts;
using Scriban.Runtime;

namespace Chaos.IL2CPP.Generator;

public sealed partial class NativeAotLoweringPlanner
{

    private static bool TryParseCustomAttributeQueryCallee(
        string? callee,
        out string? attributeDisplayName)
    {
        attributeDisplayName = null;
        const string declaringTypeSubjectId =
            "System.Private.CoreLib/System.Reflection.CustomAttributeExtensions";
        const string openGenericMethodPrefix =
            "System.Private.CoreLib/System.Reflection.CustomAttributeExtensions::GetCustomAttribute<";

        if (string.IsNullOrEmpty(callee) || callee.StartsWith("<", StringComparison.Ordinal) ||
            !string.Equals(GetMethodDeclaringTypeSubjectId(callee), declaringTypeSubjectId, StringComparison.Ordinal) ||
            !GetMethodName(callee).StartsWith("GetCustomAttribute<", StringComparison.Ordinal) ||
            !GetMethodParameterTypes(callee).SequenceEqual(["System.Reflection.MemberInfo"]) ||
            !TryReadSingleGenericTypeArgument(callee, openGenericMethodPrefix, out var attributeTypeName))
        {
            return false;
        }

        attributeDisplayName = GetTypeDisplayName(attributeTypeName);
        return !string.IsNullOrEmpty(attributeDisplayName);
    }



    private static bool TryParseAttributeGetterMethodSubjectId(
        string? subjectId,
        out string? attributeTypeSubjectId,
        out string? memberName)
    {
        attributeTypeSubjectId = null;
        memberName = null;
        if (string.IsNullOrEmpty(subjectId) ||
            !subjectId.EndsWith("()", StringComparison.Ordinal))
        {
            return false;
        }

        var separatorIndex = subjectId.IndexOf("::get_", StringComparison.Ordinal);
        if (separatorIndex <= 0)
        {
            return false;
        }

        attributeTypeSubjectId = subjectId[..separatorIndex];
        memberName = subjectId.Substring(separatorIndex + "::get_".Length, subjectId.Length - separatorIndex - "::get_".Length - 2);
        return !string.IsNullOrEmpty(attributeTypeSubjectId) &&
               !string.IsNullOrEmpty(memberName);
    }



    private static bool TryGetAttributeTypeIdentity(
        MetadataReader metadataReader,
        string currentAssemblyName,
        CustomAttributeHandle attributeHandle,
        out MetadataTypeIdentity attributeTypeIdentity)
    {
        var attribute = metadataReader.GetCustomAttribute(attributeHandle);
        return TryResolveTypeIdentity(metadataReader, currentAssemblyName, attribute.Constructor, out attributeTypeIdentity);
    }



    private static bool TryResolveTypeIdentity(
        MetadataReader metadataReader,
        string currentAssemblyName,
        EntityHandle constructorOrTypeHandle,
        out MetadataTypeIdentity typeIdentity)
    {
        switch (constructorOrTypeHandle.Kind)
        {
            case HandleKind.MethodDefinition:
                var methodDefinition = metadataReader.GetMethodDefinition((MethodDefinitionHandle)constructorOrTypeHandle);
                return TryResolveTypeDefinitionIdentity(
                    metadataReader,
                    currentAssemblyName,
                    methodDefinition.GetDeclaringType(),
                    out typeIdentity);
            case HandleKind.MemberReference:
                var memberReference = metadataReader.GetMemberReference((MemberReferenceHandle)constructorOrTypeHandle);
                return TryResolveTypeIdentity(metadataReader, currentAssemblyName, memberReference.Parent, out typeIdentity);
            case HandleKind.TypeDefinition:
                return TryResolveTypeDefinitionIdentity(
                    metadataReader,
                    currentAssemblyName,
                    (TypeDefinitionHandle)constructorOrTypeHandle,
                    out typeIdentity);
            case HandleKind.TypeReference:
                return TryResolveTypeReferenceIdentity(
                    metadataReader,
                    currentAssemblyName,
                    (TypeReferenceHandle)constructorOrTypeHandle,
                    out typeIdentity);
            default:
                typeIdentity = default;
                return false;
        }
    }



    private static bool TryResolveTypeDefinitionIdentity(
        MetadataReader metadataReader,
        string currentAssemblyName,
        TypeDefinitionHandle handle,
        out MetadataTypeIdentity typeIdentity)
    {
        var typeDefinition = metadataReader.GetTypeDefinition(handle);
        var typeName = metadataReader.GetString(typeDefinition.Name);
        var namespaceName = metadataReader.GetString(typeDefinition.Namespace);
        var declaringTypeHandle = typeDefinition.GetDeclaringType();
        if (!declaringTypeHandle.IsNil &&
            TryResolveTypeDefinitionIdentity(metadataReader, currentAssemblyName, declaringTypeHandle, out var declaringTypeIdentity))
        {
            typeIdentity = declaringTypeIdentity with
            {
                TypeName = $"{declaringTypeIdentity.TypeName}+{typeName}",
            };
            return true;
        }

        typeIdentity = new MetadataTypeIdentity(currentAssemblyName, namespaceName, typeName);
        return true;
    }



    private static bool TryResolveTypeReferenceIdentity(
        MetadataReader metadataReader,
        string currentAssemblyName,
        TypeReferenceHandle handle,
        out MetadataTypeIdentity typeIdentity)
    {
        var typeReference = metadataReader.GetTypeReference(handle);
        var typeName = metadataReader.GetString(typeReference.Name);
        var namespaceName = metadataReader.GetString(typeReference.Namespace);
        switch (typeReference.ResolutionScope.Kind)
        {
            case HandleKind.AssemblyReference:
                var assemblyReference = metadataReader.GetAssemblyReference((AssemblyReferenceHandle)typeReference.ResolutionScope);
                typeIdentity = new MetadataTypeIdentity(
                    metadataReader.GetString(assemblyReference.Name),
                    namespaceName,
                    typeName);
                return true;

            case HandleKind.TypeReference:
                if (TryResolveTypeReferenceIdentity(
                        metadataReader,
                        currentAssemblyName,
                        (TypeReferenceHandle)typeReference.ResolutionScope,
                        out var declaringTypeIdentity))
                {
                    typeIdentity = declaringTypeIdentity with
                    {
                        TypeName = $"{declaringTypeIdentity.TypeName}+{typeName}",
                    };
                    return true;
                }

                break;

            case HandleKind.ModuleDefinition:
            case HandleKind.AssemblyDefinition:
                typeIdentity = new MetadataTypeIdentity(currentAssemblyName, namespaceName, typeName);
                return true;
        }

        typeIdentity = default;
        return false;
    }


    // ── Shared hotpatch dispatch condition emission ───────────────────────
    // All dispatch decision points must use this helper so the condition
    // (flags check + kHotpatchKeepNative exclusion + acquire fence) is
    // consistent across all call sites.
    //
    // Emits:  if (HotpatchIsActive(entry) && !(entry.flags & kHotpatchKeepNative))
    //
    // With kHotpatchMakeActive and kHotpatchKeepNative:
    //   - kHotpatchActive (bit 0):  method has been patched, interpreter IR is ready
    //   - kHotpatchKeepNative (bit 1): method uses external runtime dispatch;
    //     interpreter cannot execute it, keep on native path even when patched
    private static void EmitHotpatchDispatchCondition(StringBuilder sb, string entryExpr, string indent = "    ")
    {
        sb.Append(indent).Append("if (chaos_runtime_get_abi_v0()->hotpatch_is_active(")
          .Append(entryExpr).Append(")")
          .Append(" && !chaos_runtime_get_abi_v0()->hotpatch_should_keep_native(")
          .Append(entryExpr).Append("))");
    }



    // ── Shared hotpatch entry reference emission ─────────────────────────
    private static void EmitHotpatchEntryRef(StringBuilder sb, string entryExpr, string indentation)
    {
        sb.Append(indentation).Append("auto& ").Append(entryExpr)
          .Append(" = s_hotpatch_entries[").Append(entryExpr).Append("];");
    }

}
