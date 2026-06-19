using System;
using System.Collections.Generic;
using System.Collections.Immutable;
using System.Globalization;
using System.IO;
using System.Linq;
using System.Reflection;
using System.Reflection.Metadata;
using System.Reflection.Metadata.Ecma335;
using System.Reflection.PortableExecutable;
using System.Runtime.CompilerServices;
using System.Text;
using System.Text.Json;
using Chaos.IL2CPP.Contracts;

namespace Chaos.IL2CPP.Generator;

public sealed partial class NativeAotLoweringPlanner
{
    private static IReadOnlyList<string> GetReferenceTypeEmissionOrder(IReadOnlySet<string> referenceTypeSubjectIds, IReadOnlyDictionary<string, string?> referenceTypeBaseSubjectIds)
    {
        List<string> ordered = new List<string>();
        HashSet<string> visited = new HashSet<string>(StringComparer.Ordinal);
        HashSet<string> visiting = new HashSet<string>(StringComparer.Ordinal);
        foreach (string item in referenceTypeSubjectIds.OrderBy<string, string>((string value) => value, StringComparer.Ordinal))
        {
            Visit(item);
        }
        return ordered;
        void Visit(string typeSubjectId)
        {
            if (!visited.Contains(typeSubjectId))
            {
                if (!visiting.Add(typeSubjectId))
                {
                    throw new InvalidOperationException("reference type inheritance cycle detected for '" + typeSubjectId + "'.");
                }
                if (referenceTypeBaseSubjectIds.TryGetValue(typeSubjectId, out string? value) && !string.IsNullOrEmpty(value) && referenceTypeSubjectIds.Contains(value))
                {
                    Visit(value);
                }
                visiting.Remove(typeSubjectId);
                visited.Add(typeSubjectId);
                ordered.Add(typeSubjectId);
            }
        }
    }

    private static IReadOnlySet<string> CollectValueTypeSubjectIds(AotCoreIrArtifact aotCoreIr)
    {
        HashSet<string> hashSet = new HashSet<string>(StringComparer.Ordinal);
        foreach (AotCoreIrMethodArtifact method in aotCoreIr.Methods)
        {
            foreach (AotCoreIrInstructionArtifact instruction in method.Instructions)
            {
                AotCoreIrReferenceArtifact? targetReference = instruction.TargetReference;
                if (targetReference is not null)
                {
                    if (targetReference.Kind == AotCoreIrReferenceKind.Type && targetReference.TypeShape == AotCoreIrTypeShapeKind.ValueType)
                    {
                        hashSet.Add(targetReference.SubjectId);
                    }
                    AotCoreIrReferenceKind kind = targetReference.Kind;
                    bool flag = kind - 2 <= AotCoreIrReferenceKind.Type;
                    if (flag && targetReference.DeclaringTypeShape == AotCoreIrTypeShapeKind.ValueType && !string.IsNullOrEmpty(targetReference.DeclaringTypeSubjectId))
                    {
                        hashSet.Add(targetReference.DeclaringTypeSubjectId);
                    }
                }
            }
        }
        return hashSet;
    }

    private static readonly HashSet<string> NonDelegateInvokeTypeNames = new(StringComparer.Ordinal)
    {
        "System.Reflection.ConstructorInfo",
        "System.Reflection.MethodInfo",
        "System.Reflection.MethodBase",
        "System.Reflection.DynamicMethod",
    };

    private static IReadOnlyDictionary<string, string?> CollectReferenceTypeBaseSubjectIds(AotCoreIrArtifact aotCoreIr)
    {
        Dictionary<string, string?> dictionary = new Dictionary<string, string?>(StringComparer.Ordinal);
        foreach (AotCoreIrMethodArtifact method in aotCoreIr.Methods)
        {
            // Pass 1: Collect types from instruction target references
            foreach (AotCoreIrInstructionArtifact instruction in method.Instructions)
            {
                AotCoreIrReferenceArtifact? targetReference = instruction.TargetReference;
                if (targetReference is not null)
                {
                    if (targetReference.Kind == AotCoreIrReferenceKind.Type && targetReference.TypeShape == AotCoreIrTypeShapeKind.ReferenceType)
                    {
                        dictionary[targetReference.SubjectId] = targetReference.BaseTypeSubjectId;
                    }
                    if (!string.IsNullOrEmpty(targetReference.ArrayElementSubjectId) && targetReference.ArrayElementTypeShape == AotCoreIrTypeShapeKind.ReferenceType)
                    {
                        dictionary[targetReference.ArrayElementSubjectId] = targetReference.ArrayElementBaseTypeSubjectId;
                    }
                }
            }
            // Pass 1b: Collect types from exception region catch clauses.
            // Catch types reference exception types not visible to instruction scanning.
            foreach (var region in method.ExceptionRegions)
            {
                if (!string.IsNullOrEmpty(region.CatchTypeSubjectId) && region.CatchTypeSubjectId.IndexOf("::") > 0)
                {
                    var dc = region.CatchTypeSubjectId.IndexOf("::");
                    string typeSubjectId = region.CatchTypeSubjectId.Substring(0, dc);
                    if (!dictionary.ContainsKey(typeSubjectId))
                        dictionary[typeSubjectId] = null;
                }
            }
        }

        // Pass 2: Fill in missing base types for delegate types.
        // Delegate constructors always have signature ::.ctor:System.Void(System.Object,System.IntPtr).
        // When the AOT IR's targetReference for a delegate type lacks baseTypeSubjectId
        // (e.g., cross-assembly types like System.Threading.ThreadStart), detect it via the
        // unique callee pattern and set the missing base to MulticastDelegate.
        foreach (AotCoreIrMethodArtifact method in aotCoreIr.Methods)
        {
            foreach (AotCoreIrInstructionArtifact instruction in method.Instructions)
            {
                if (!string.IsNullOrEmpty(instruction.Callee) &&
                    instruction.Callee.Contains("::.ctor:", StringComparison.Ordinal) &&
                    instruction.Callee.EndsWith("(System.Object,System.IntPtr)", StringComparison.Ordinal))
                {
                    string declaringType = GetMethodDeclaringTypeSubjectId(instruction.Callee);
                    if (!dictionary.TryGetValue(declaringType, out var existingBase) || string.IsNullOrEmpty(existingBase))
                    {
                        dictionary[declaringType] = "System.Private.CoreLib/System.MulticastDelegate";
                    }
                }
            }
        }

        // Pass 3: Detect delegate types via callvirt Invoke pattern.
        // Delegate types that are only referenced as callee declaring types
        // (e.g., System.Action used as Assert.Throws<Action> parameter) don't
        // have TargetReference entries in Pass 1, and their constructors may
        // not be called directly (Pass 2).  The callvirt Invoke pattern
        // uniquely identifies delegate usage — any method named "Invoke"
        // called via callvirt whose declaring type is not yet in the
        // dictionary is a delegate type.
        foreach (AotCoreIrMethodArtifact method in aotCoreIr.Methods)
        {
            foreach (AotCoreIrInstructionArtifact instruction in method.Instructions)
            {
                if (!string.IsNullOrEmpty(instruction.Callee) &&
                    instruction.Op is "callvirt" and not null &&
                    string.Equals(GetMethodName(instruction.Callee), "Invoke", StringComparison.Ordinal))
                {
                    string declaringType = GetMethodDeclaringTypeSubjectId(instruction.Callee);
                    if ((!dictionary.TryGetValue(declaringType, out var existingBase) || string.IsNullOrEmpty(existingBase))
                        && !NonDelegateInvokeTypeNames.Contains(ManagedNaming.GetTypeDisplayNameFromSubjectId(declaringType)))
                    {
                        dictionary[declaringType] = "System.Private.CoreLib/System.MulticastDelegate";
                    }
                }
            }
        }
        return dictionary;
    }

    private static IReadOnlyDictionary<string, HashSet<string>> CollectReferenceTypeImplementedInterfaceSubjectIds(AotCoreIrArtifact aotCoreIr)
    {
        Dictionary<string, HashSet<string>> dictionary = new Dictionary<string, HashSet<string>>(StringComparer.Ordinal);
        foreach (AotCoreIrMethodArtifact method in aotCoreIr.Methods)
        {
            foreach (AotCoreIrInstructionArtifact instruction in method.Instructions)
            {
                AotCoreIrReferenceArtifact? targetReference = instruction.TargetReference;
                if (targetReference is not null)
                {
                    if (targetReference.Kind == AotCoreIrReferenceKind.Type && (targetReference.TypeShape == AotCoreIrTypeShapeKind.ReferenceType || targetReference.TypeShape == AotCoreIrTypeShapeKind.ValueType))
                    {
                        TrackImplementedInterfaces(dictionary, targetReference.SubjectId, targetReference.ImplementedInterfaceSubjectIds);
                    }
                    if (!string.IsNullOrEmpty(targetReference.ArrayElementSubjectId) && targetReference.ArrayElementTypeShape == AotCoreIrTypeShapeKind.ReferenceType)
                    {
                        TrackImplementedInterfaces(dictionary, targetReference.ArrayElementSubjectId, targetReference.ArrayElementImplementedInterfaceSubjectIds);
                    }
                }
            }
        }
        return dictionary;
        static void TrackImplementedInterfaces(IDictionary<string, HashSet<string>> implementedInterfaceSubjectIds, string? typeSubjectId, IReadOnlyList<string>? interfaceSubjectIds)
        {
            if (string.IsNullOrEmpty(typeSubjectId) || interfaceSubjectIds == null || interfaceSubjectIds.Count == 0)
            {
                return;
            }
            if (!implementedInterfaceSubjectIds.TryGetValue(typeSubjectId, out HashSet<string>? value))
            {
                value = (implementedInterfaceSubjectIds[typeSubjectId] = new HashSet<string>(StringComparer.Ordinal));
            }
            foreach (string interfaceSubjectId in interfaceSubjectIds)
            {
                if (!string.IsNullOrEmpty(interfaceSubjectId))
                {
                    value.Add(interfaceSubjectId);
                }
            }
        }
    }

    private static IReadOnlySet<string> CollectSealedTypeSubjectIds(AotCoreIrArtifact aotCoreIr)
    {
        var sealedTypes = new HashSet<string>(StringComparer.Ordinal);
        foreach (var method in aotCoreIr.Methods)
        {
            foreach (var instruction in method.Instructions)
            {
                AotCoreIrReferenceArtifact? targetReference = instruction.TargetReference;
                if (targetReference is not null)
                {
                    if (targetReference.Kind == AotCoreIrReferenceKind.Type && targetReference.IsSealed)
                    {
                        sealedTypes.Add(targetReference.SubjectId);
                    }
                }
            }
        }
        return sealedTypes;
    }

    private static IReadOnlySet<string> CollectInterfaceTypeSubjectIds(AotCoreIrArtifact aotCoreIr)
    {
        HashSet<string> hashSet = new HashSet<string>(StringComparer.Ordinal);
        foreach (AotCoreIrMethodArtifact method in aotCoreIr.Methods)
        {
            foreach (AotCoreIrInstructionArtifact instruction in method.Instructions)
            {
                AotCoreIrReferenceArtifact? targetReference = instruction.TargetReference;
                if (targetReference is not null)
                {
                    if (targetReference.Kind == AotCoreIrReferenceKind.Type && targetReference.TypeShape == AotCoreIrTypeShapeKind.InterfaceType)
                    {
                        hashSet.Add(targetReference.SubjectId);
                    }
                    AotCoreIrReferenceKind kind = targetReference.Kind;
                    bool flag = kind - 2 <= AotCoreIrReferenceKind.Type;
                    if (flag && targetReference.DeclaringTypeShape == AotCoreIrTypeShapeKind.InterfaceType && !string.IsNullOrEmpty(targetReference.DeclaringTypeSubjectId))
                    {
                        hashSet.Add(targetReference.DeclaringTypeSubjectId);
                    }
                    // Collect interface SubjectIds from ImplementedInterfaceSubjectIds on Type references.
                    // This is the primary way interface types appear in the AOT IR -- as interfaces
                    // implemented by concrete reference/value types. Without this, the interface
                    // devirtualization branch in InvocationPlanning.cs would be dead code.
                    if (targetReference.Kind == AotCoreIrReferenceKind.Type && targetReference.ImplementedInterfaceSubjectIds is { Count: > 0 })
                    {
                        foreach (string implementedInterfaceSubjectId in targetReference.ImplementedInterfaceSubjectIds)
                        {
                            if (!string.IsNullOrEmpty(implementedInterfaceSubjectId))
                            {
                                hashSet.Add(implementedInterfaceSubjectId);
                            }
                        }
                    }
                    // Also collect from array element ImplementedInterfaceSubjectIds.
                    if (targetReference.ArrayElementImplementedInterfaceSubjectIds is { Count: > 0 })
                    {
                        foreach (string arrayElementImplementedInterfaceSubjectId in targetReference.ArrayElementImplementedInterfaceSubjectIds)
                        {
                            if (!string.IsNullOrEmpty(arrayElementImplementedInterfaceSubjectId))
                            {
                                hashSet.Add(arrayElementImplementedInterfaceSubjectId);
                            }
                        }
                    }
                }
            }
        }
        return hashSet;
    }

    private static string GetDeclaringTypeSubjectId(string fieldSubjectId)
    {
        int num = fieldSubjectId.IndexOf("::", StringComparison.Ordinal);
        if (num <= 0)
        {
            throw new InvalidOperationException("field subject '" + fieldSubjectId + "' is missing declaring type information");
        }
        return fieldSubjectId.Substring(0, num);
    }

    private static string GetFieldName(string fieldSubjectId)
    {
        int num = fieldSubjectId.IndexOf("::", StringComparison.Ordinal);
        if (num <= 0 || num + 2 >= fieldSubjectId.Length)
        {
            throw new InvalidOperationException("field subject '" + fieldSubjectId + "' is missing field member information");
        }
        int num2 = num + 2;
        return fieldSubjectId.Substring(num2, fieldSubjectId.Length - num2);
    }

    private static string GetMethodSignatureSuffix(string subjectId)
    {
        int num = subjectId.IndexOf("::", StringComparison.Ordinal);
        if (num <= 0 || num + 2 >= subjectId.Length)
        {
            throw new InvalidOperationException("method subject '" + subjectId + "' is missing method signature information");
        }
        int num2 = num + 2;
        return subjectId.Substring(num2, subjectId.Length - num2);
    }

    private static string GetMethodDeclaringTypeSubjectId(string subjectId)
    {
        int num = subjectId.IndexOf("::", StringComparison.Ordinal);
        if (num <= 0)
        {
            throw new InvalidOperationException("method subject '" + subjectId + "' is missing declaring type information");
        }
        return subjectId.Substring(0, num);
    }

    private IReadOnlyList<AotCoreIrAbiSlotArtifact> ResolveDelegateInvokeParameterAbis(AotCoreIrInstructionArtifact instruction)
    {
        if (!string.IsNullOrEmpty(instruction.Callee) && _methodsBySubjectId.TryGetValue(instruction.Callee, out AotCoreIrMethodArtifact? value))
        {
            return value.ParameterAbis;
        }
        return GetMethodParameterTypes(instruction.Callee!).Select(CreateLegacyAbiSlot).ToArray();
    }

    private AotCoreIrAbiSlotArtifact ResolveDelegateInvokeReturnAbi(AotCoreIrInstructionArtifact instruction)
    {
        if (!string.IsNullOrEmpty(instruction.Callee) && _methodsBySubjectId.TryGetValue(instruction.Callee, out AotCoreIrMethodArtifact? value))
        {
            return value.ReturnAbi;
        }
        return CreateLegacyReturnAbiSlot(instruction.ResultType);
    }

    private static string GetRequiredDeclaringTypeSubjectId(AotCoreIrReferenceArtifact targetReference)
    {
        if (!string.IsNullOrEmpty(targetReference.DeclaringTypeSubjectId))
        {
            return targetReference.DeclaringTypeSubjectId;
        }
        return GetDeclaringTypeSubjectId(targetReference.SubjectId);
    }

    private bool IsDelegateInvokeInstruction(AotCoreIrInstructionArtifact instruction)
    {
        if (!string.Equals(instruction.Op, "callvirt", StringComparison.Ordinal) || string.IsNullOrEmpty(instruction.Callee) || !string.Equals(GetMethodName(instruction.Callee), "Invoke", StringComparison.Ordinal))
        {
            return false;
        }
        return IsDelegateTypeSubjectId(GetMethodDeclaringTypeSubjectId(instruction.Callee), _referenceTypeBaseSubjectIds);
    }

    private static string GetRequiredStringOperand(AotCoreIrInstructionArtifact instruction)
    {
        if (instruction.Operand is string result)
        {
            return result;
        }
        if (instruction.Operand is JsonElement { ValueKind: JsonValueKind.String } jsonElement)
        {
            return jsonElement.GetString() ?? throw new InvalidOperationException("opcode '" + instruction.Op + "' requires a non-null string operand.");
        }
        throw new InvalidOperationException("opcode '" + instruction.Op + "' requires a string operand.");
    }

    private static string GetFieldHandleLiteral(string subjectId)
    {
        return GetPseudoMetadataHandleLiteral(subjectId, 67108864u);
    }

    private static string GetTypeHandleLiteral(string subjectId)
    {
        return GetPseudoMetadataHandleLiteral(subjectId, 33554432u);
    }

    private static string GetMethodHandleLiteral(string subjectId)
    {
        return GetPseudoMetadataHandleLiteral(subjectId, 100663296u);
    }

    private static string GetNativeSymbol(string prefix, string subjectId)
    {
        return prefix + SanitizeSubjectId(subjectId);
    }

    private static string GetNativeTypeSymbol(string subjectId)
    {
        return GetNativeSymbol("chaos_type_", subjectId);
    }

    private static string GetNativeValueTypeSymbol(string subjectId)
    {
        return GetNativeSymbol("chaos_valuetype_", subjectId);
    }

    private static string GetNativeFieldMemberName(string subjectId)
    {
        return GetNativeSymbol("field_", subjectId);
    }

    private static string GetNativeStaticFieldSymbol(string subjectId)
    {
        return GetNativeSymbol("chaos_static_", subjectId);
    }

    private static string GetNativeTypeInitializationFunctionSymbol(string typeSubjectId)
    {
        return GetNativeSymbol("chaos_ensure_type_initialized_", typeSubjectId);
    }

    private static string GetNativeTypeInitializationOnceFlagSymbol(string typeSubjectId)
    {
        return GetNativeSymbol("chaos_type_init_once_", typeSubjectId);
    }

    private static string GetNativeBoxTypeSymbol(string subjectId)
    {
        return GetNativeSymbol("chaos_boxed_type_", subjectId);
    }

    private static string GetNativeTypeIdSymbol(string subjectId)
    {
        return GetNativeSymbol("chaos_type_id_", subjectId);
    }

    private static string GetNativeTypeInfoSymbol(string subjectId)
    {
        // MethodTable has no .hot member; use AsTypeInfoHot() which does reinterpret_cast.
        return GetNativeMethodTableSymbol(subjectId) + ".AsTypeInfoHot()";
    }

    private static string GetNativeTypeInfoV0Symbol(string subjectId)
    {
        return GetNativeMethodTableSymbol(subjectId);
    }

    private static string GetNativeMethodTableSymbol(string subjectId)
    {
        return GetNativeSymbol("chaos_mt_", subjectId);
    }

    private static string GetNativeVTableSymbol(string subjectId)
    {
        return GetNativeSymbol("chaos_vtable_", subjectId);
    }

    private static string GetNativeIfaceMapSymbol(string subjectId)
    {
        return GetNativeSymbol("chaos_iface_map_", subjectId);
    }

    private static string GetNativeBoxTypeIdSymbol(string subjectId)
    {
        return GetNativeSymbol("chaos_boxed_type_id_", subjectId);
    }

    private static string GetNativeTypeInfoWarmSymbol(string subjectId)
    {
        // MethodTable has warm fields inline at offset 32; use .warm member access.
        return GetNativeMethodTableSymbol(subjectId) + ".warm";
    }

    /// <summary>
    /// Compute FNV-1a 64-bit stable type ID from a subject ID.
    /// Must match chaos_compute_type_stable_id() in type_info.h.
    /// </summary>

    // Primitive value types that need inline INTPTR storage, not a struct definition.
    private static readonly System.Collections.Generic.HashSet<string> PrimitiveValueTypeSubjectIds = new(System.StringComparer.Ordinal)
    {
        "System.Private.CoreLib/System.Boolean",
        "System.Private.CoreLib/System.Byte",
        "System.Private.CoreLib/System.SByte",
        "System.Private.CoreLib/System.Int16",
        "System.Private.CoreLib/System.UInt16",
        "System.Private.CoreLib/System.Int32",
        "System.Private.CoreLib/System.UInt32",
        "System.Private.CoreLib/System.Int64",
        "System.Private.CoreLib/System.UInt64",
        "System.Private.CoreLib/System.Single",
        "System.Private.CoreLib/System.Double",
        "System.Private.CoreLib/System.Char",
        "System.Private.CoreLib/System.IntPtr",
        "System.Private.CoreLib/System.UIntPtr",
    };

    /// <summary>
    /// Collect COM interface vtable data from AotCoreIr references.
    /// Scans all instructions for type references with ComInterfaceGuid set,
    /// then counts the methods declared on each such COM interface type.
    /// </summary>
}
