using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using Chaos.IL2CPP.Contracts;

namespace Chaos.IL2CPP.Generator;

public sealed partial class NativeAotLoweringPlanner
{
    /// <summary>
    /// Collect GC reference field offsets for all emitted reference types and emit a
    /// ChaosRegisterGcLayouts() function that registers each type's GcTypeLayout with
    /// the GcLayoutRegistry.
    ///
    /// This enables precise (non-conservative) GC scanning for all managed types,
    /// eliminating false positives from raw C++ pointers (e.g. List&lt;T&gt;.items_array
    /// allocated via malloc) that were previously misinterpreted as GC references.
    ///
    /// Must be called after struct definitions have been emitted so that sizeof() and
    /// offsetof() compile correctly.
    /// </summary>
    private void EmitGcTypeLayoutRegistration(
        StringBuilder builder,
        IReadOnlySet<string> referenceTypeSubjectIds,
        IReadOnlyDictionary<string, string?> referenceTypeBaseSubjectIds,
        IReadOnlyDictionary<string, List<string>> fieldsByDeclaringType,
        IReadOnlyDictionary<string, string?> fieldTypeMap,
        IReadOnlySet<string> valueTypeSubjectIds)
    {
        // ── Skeleton type detection ──
        // Skeleton types (List, Dictionary, HashSet) embed native C++ pointers
        // (malloc'd) in CHAOS_IL2CPP_INTPTR fields that are NOT GC references.
        // Must be registered with pointer_count=0 so GC does not conservatively
        // scan the internal buffer pointer as a GC object reference.
        var skeletonTypeIds = new HashSet<string>(StringComparer.Ordinal);
        foreach (var typeId in referenceTypeSubjectIds)
        {
            var ns = ManagedNaming.NormalizeSubjectIdAssembly(typeId);
            if (ns.StartsWith("System.Private.CoreLib/System.Collections.Generic.List", StringComparison.Ordinal) ||
                ns.StartsWith("System.Private.CoreLib/System.Collections.Generic.Dictionary", StringComparison.Ordinal) ||
                ns.StartsWith("System.Private.CoreLib/System.Collections.Generic.HashSet", StringComparison.Ordinal))
            {
                skeletonTypeIds.Add(typeId);
            }
        }

        // ── Phase 1: Collect (subjectId, typeSymbol, stableId, gcMemberNames) ──
        var entries = new List<(string SubjectId, string TypeSymbol, ulong StableId, List<string> GcMemberNames)>();

        foreach (var typeId in GetReferenceTypeEmissionOrder(referenceTypeSubjectIds, referenceTypeBaseSubjectIds))
        {
            var ns = ManagedNaming.NormalizeSubjectIdAssembly(typeId);
            string typeSymbol = GetNativeTypeSymbol(typeId);
            ulong stableId = ComputeStableTypeId(typeId);

            // Skip Decimal — boxed value type with header-only struct, no GC refs.
            if (string.Equals(ns, "System.Private.CoreLib/System.Decimal", StringComparison.Ordinal))
                continue;

            bool isSkeleton = skeletonTypeIds.Contains(typeId);
            bool hasHardcodedRefs = false;
            var hardcodedMembers = new List<string>();
            if (!isSkeleton)
            {
                hasHardcodedRefs = HasHardcodedGcRefs(ns, out hardcodedMembers);
            }

            var gcMemberNames = new List<string>();

            // Add hardcoded GC refs (Exception, Delegate, reflection types).
            if (hasHardcodedRefs && hardcodedMembers.Count > 0)
                gcMemberNames.AddRange(hardcodedMembers);

            // Walk full inheritance chain collecting tracked GC ref fields.
            // offsetof(Derived, base_field) works correctly in C++ via inheritance,
            // so we can emit offsetof expressions for fields from any hierarchy level.
            var visitedMembers = new HashSet<string>(StringComparer.Ordinal);
            string? current = typeId;
            while (current != null && referenceTypeSubjectIds.Contains(current))
            {
                if (fieldsByDeclaringType.TryGetValue(current, out var fieldList))
                {
                    foreach (string fieldSubjectId in fieldList)
                    {
                        string memberName = GetNativeFieldMemberName(fieldSubjectId);
                        if (!visitedMembers.Add(memberName))
                            continue;

                        string? fieldType = fieldTypeMap.GetValueOrDefault(fieldSubjectId);
                        if (IsFieldAGcReference(fieldType, referenceTypeSubjectIds, valueTypeSubjectIds))
                            gcMemberNames.Add(memberName);
                    }
                }
                referenceTypeBaseSubjectIds.TryGetValue(current, out string? next);
                current = (next != null && referenceTypeSubjectIds.Contains(next)) ? next : null;
            }

            entries.Add((typeId, typeSymbol, stableId, gcMemberNames));
        }

        if (entries.Count == 0)
            return;

        // ── Phase 2: Emit offset arrays ──
        builder.AppendLine();
        builder.AppendLine("// ── GcTypeLayout pointer offset arrays (computed via offsetof) ──");

        foreach (var (_, TypeSymbol, _, GcMemberNames) in entries)
        {
            if (GcMemberNames.Count == 0)
                continue;

            string offsetsSym = $"kGcOffsets_{SanitizeSubjectId(TypeSymbol)}";
            builder.Append($"static constexpr uint16_t {offsetsSym}[] = {{");
            for (int i = 0; i < GcMemberNames.Count; i++)
            {
                if (i > 0) builder.Append(", ");
                builder.Append($"static_cast<uint16_t>(offsetof({TypeSymbol}, {GcMemberNames[i]}))");
            }
            builder.AppendLine("};");
        }

        // ── Phase 3: Emit ChaosRegisterGcLayouts() ──
        builder.AppendLine();
        builder.AppendLine("extern \"C\" void ChaosRegisterGcLayouts() {");
        builder.AppendLine("    auto& registry = chaos::il2cpp::runtime_core::GcLayoutRegistry::Instance();");
        builder.AppendLine();

        foreach (var (_, TypeSymbol, StableId, GcMemberNames) in entries)
        {
            if (GcMemberNames.Count > 0)
            {
                string offsetsSym = $"kGcOffsets_{SanitizeSubjectId(TypeSymbol)}";
                builder.AppendLine($"    registry.Register({StableId}ULL, sizeof({TypeSymbol}), {offsetsSym}, {GcMemberNames.Count});");
            }
            else
            {
                // Zero-pointer type — register explicitly so GC can skip it.
                builder.AppendLine($"    registry.Register({StableId}ULL, sizeof({TypeSymbol}), nullptr, 0);");
            }
        }

        // ── Phase 4: Register MethodTable address ranges ──
        // IsValidTypeInfoPointer() checks if a candidate TypeInfo* falls within
        // a registered address range. Without this, the young collector's Phase 2
        // candidate validation rejects valid codegen-emitted MethodTable pointers,
        // forcing a fallback to conservative scanning.
        builder.AppendLine();
        builder.AppendLine("    // Register MethodTable address ranges for IsValidTypeInfoPointer.");
        foreach (var (SubjectId, _, _, _) in entries)
        {
            string mtSymbol = GetNativeMethodTableSymbol(SubjectId);
            // MethodTable = chaos_mt_X (the struct itself). The first 32 bytes alias
            // as TypeInfoHot. Register the full struct range.
            builder.AppendLine($"    registry.RegisterTypeInfoRange(reinterpret_cast<uintptr_t>(&{mtSymbol}), reinterpret_cast<uintptr_t>(&{mtSymbol}) + sizeof({mtSymbol}));");
        }

        builder.AppendLine("}");
        builder.AppendLine();
    }

    /// <summary>
    /// Check whether a type has specially-emitted GC ref fields. Returns true and
    /// populates <paramref name="hardcodedMembers"/> with the C++ member names of
    /// those fields.
    /// </summary>
    private static bool HasHardcodedGcRefs(string ns, out List<string> hardcodedMembers)
    {
        hardcodedMembers = new List<string>();

        // Exception-derived: _message (String), _innerException (Exception),
        // _stackTrace (String) — all are GC references.
        if (ns.StartsWith("System.Private.CoreLib/", StringComparison.Ordinal) &&
            ns.EndsWith("Exception", StringComparison.Ordinal))
        {
            hardcodedMembers.Add("_message");
            hardcodedMembers.Add("_innerException");
            hardcodedMembers.Add("_stackTrace");
            return true;
        }

        // Delegate / MulticastDelegate
        if (string.Equals(ns, "System.Private.CoreLib/System.Delegate", StringComparison.Ordinal) ||
            string.Equals(ns, "System.Private.CoreLib/System.MulticastDelegate", StringComparison.Ordinal))
        {
            hardcodedMembers.Add("chaos_delegate_target");
            hardcodedMembers.Add("chaos_delegate_invocation_list");
            return true;
        }

        // System.Type
        if (string.Equals(ns, "System.Private.CoreLib/System.Type", StringComparison.Ordinal))
        {
            hardcodedMembers.Add("runtime_type_handle");
            hardcodedMembers.Add("runtime_name_value");
            return true;
        }

        // System.Reflection.MethodInfo / ConstructorInfo
        if (string.Equals(ns, "System.Private.CoreLib/System.Reflection.MethodInfo", StringComparison.Ordinal) ||
            string.Equals(ns, "System.Private.CoreLib/System.Reflection.ConstructorInfo", StringComparison.Ordinal))
        {
            hardcodedMembers.Add("declaring_type_handle");
            hardcodedMembers.Add("runtime_method_handle");
            hardcodedMembers.Add("generic_definition_method_handle");
            hardcodedMembers.Add("generic_argument_type_handle");
            hardcodedMembers.Add("runtime_name_value");
            return true;
        }

        // System.Reflection.FieldInfo
        if (string.Equals(ns, "System.Private.CoreLib/System.Reflection.FieldInfo", StringComparison.Ordinal))
        {
            hardcodedMembers.Add("declaring_type_handle");
            hardcodedMembers.Add("runtime_name_value");
            return true;
        }

        // System.Reflection.ParameterInfo
        if (string.Equals(ns, "System.Private.CoreLib/System.Reflection.ParameterInfo", StringComparison.Ordinal))
        {
            hardcodedMembers.Add("runtime_name_value");
            return true;
        }

        // System.Reflection.Assembly
        if (string.Equals(ns, "System.Private.CoreLib/System.Reflection.Assembly", StringComparison.Ordinal))
        {
            hardcodedMembers.Add("runtime_assembly_name_value");
            return true;
        }

        // System.Reflection.AssemblyName
        if (string.Equals(ns, "System.Private.CoreLib/System.Reflection.AssemblyName", StringComparison.Ordinal))
        {
            hardcodedMembers.Add("runtime_name_value");
            return true;
        }

        return false;
    }

    /// <summary>
    /// Determine whether a field of the given managed type is a GC object reference.
    ///
    /// Primitive types and structured value types stored inline are NOT GC refs.
    /// Types known to be reference types (appear in referenceTypeSubjectIds) ARE GC refs.
    /// Unknown types are conservatively treated as NOT GC refs to avoid moving-GC corruption
    /// (over-scanning a raw pointer during compaction would corrupt the value).
    /// </summary>
    private static bool IsFieldAGcReference(
        string? fieldTypeSubjectId,
        IReadOnlySet<string> referenceTypeSubjectIds,
        IReadOnlySet<string> valueTypeSubjectIds)
    {
        if (fieldTypeSubjectId == null)
            return false;

        // Primitive types are never GC references.
        if (PrimitiveValueTypeSubjectIds.Contains(fieldTypeSubjectId))
            return false;

        // Structured value types are stored inline — not pointers to GC heap.
        if (valueTypeSubjectIds.Contains(fieldTypeSubjectId))
            return false;

        // Known reference type → GC reference.
        if (referenceTypeSubjectIds.Contains(fieldTypeSubjectId))
            return true;

        // Unknown type: conservative — assume NOT a GC ref.
        return false;
    }
}
