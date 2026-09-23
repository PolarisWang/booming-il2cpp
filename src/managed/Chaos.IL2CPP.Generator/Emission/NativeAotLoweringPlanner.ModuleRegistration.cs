using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Reflection.Metadata;
using System.Reflection.Metadata.Ecma335;
using System.Reflection.PortableExecutable;
using System.Text;
using System.Text.Json;
using Chaos.IL2CPP.Contracts;
using Scriban.Runtime;

namespace Chaos.IL2CPP.Generator;

public sealed partial class NativeAotLoweringPlanner
{
    internal string BuildModuleRegistration()
    {
        var assemblyName = _assemblyName;
        if (string.IsNullOrWhiteSpace(assemblyName))
        {
            return string.Empty;
        }

        bool hasTypeData = _moduleTypeCount > 0 && _moduleTypeFlags.Count == _moduleTypeCount;
        bool hasNestedTypes = _moduleNestedTypeChildren.Count > 0;
        bool hasConstraints = _moduleGenericParamConstraintData.Count > 0;

        // Pre-build custom attribute blob + materializer code (too complex for Scriban, stay in StringBuilder)
        string caCode = BuildCustomAttributeBlobAndMaterializer();

        // Pre-build ModuleDescriptor custom attribute fields
        var caFieldSb = new StringBuilder(256);
        EmitCustomAttributeModuleDescriptorFields(caFieldSb);
        string caFieldCode = caFieldSb.ToString();

        // Pre-build the reflection member descriptor tables (properties / fields /
        // events).  Built in C# rather than Scriban because the grouping into
        // per-type arrays with prefix indices is easier to get right here.
        string reflectionMemberCode = BuildReflectionMemberDescriptorTables();

        var model = new ScriptObject
        {
            ["indentation"] = ScribanTemplateRenderer.Indentation(1),
            ["assembly_name"] = EscapeCppStringLiteral(assemblyName),
            ["has_type_data"] = hasTypeData,
            ["type_count"] = _moduleTypeCount,
            ["custom_attribute_blob_code"] = caCode,
            ["module_descriptor_custom_attr_fields"] = caFieldCode,
            ["has_reflection_members"] = reflectionMemberCode.Length > 0,
            ["reflection_member_tables_code"] = reflectionMemberCode,
        };

        if (hasTypeData)
        {
            model["type_flags"] = _moduleTypeFlags.Select(f => (object)f).ToArray();
            model["type_names"] = _moduleTypeNames.Select(n => (object)EscapeCppStringLiteral(n)).ToArray();
            model["type_namespaces"] = _moduleTypeNamespaces.Select(ns => (object)EscapeCppStringLiteral(ns)).ToArray();
            model["type_parent_tokens"] = _moduleTypeParentTokens.Select(t => (object)t).ToArray();
            model["type_info_ptrs"] = _moduleTypeInfoSymbols
                .Select(s => (object)(s ?? "nullptr")).ToArray();

            model["has_nested_types"] = hasNestedTypes;
            if (hasNestedTypes)
            {
                model["nested_type_children"] = _moduleNestedTypeChildren.Select(c => (object)c).ToArray();
                model["nested_children_count"] = _moduleNestedTypeChildren.Count;
            }
            model["nested_type_offsets"] = _moduleNestedTypeOffsets.Select(o => (object)o).ToArray();
            model["nested_offsets_count"] = _moduleNestedTypeOffsets.Count;

            model["has_constraints"] = hasConstraints;
            if (hasConstraints)
            {
                model["generic_param_constraint_data"] = _moduleGenericParamConstraintData.Select(d => (object)d).ToArray();
                model["constraint_data_count"] = _moduleGenericParamConstraintData.Count;
            }
            model["generic_param_constraint_offsets"] = _moduleGenericParamConstraintOffsets.Select(o => (object)o).ToArray();
            model["constraint_offsets_count"] = _moduleGenericParamConstraintOffsets.Count;
        }

        return ScribanTemplateRenderer.RenderTemplate(
            NativeAotTemplateCatalog.GetModuleRegistrationTemplate(), model);
    }

    /// <summary>
    /// Emit hotpatch name index + dispatch table data.
    /// Generates flat .rodata arrays: type index, method index, token->slot map,
    /// dispatch table, and a HotpatchModuleV0 bundle with a static init caller.
    ///
    /// Uses ALL hotpatchable methods (not just reachable) so that hotpatch can
    /// target any method in the module. Methods without ECMA tokens get
    /// synthetic tokens (0x80000000 | syntheticIndex).
    ///
    /// The slot order MUST match BuildDispatchSlotMap (both use
    /// GetHotpatchableMethods() with the same ordering).
    ///
    /// Generated C++ pattern:
    /// <code>
    /// static constexpr HotpatchMethodEntryV0 s_hotpatch_methods[] = { ... };
    /// static constexpr HotpatchTypeEntryV0   s_hotpatch_types[] = { ... };
    /// static constexpr HotpatchSlotEntryV0   s_hotpatch_slots[] = { ... };
    /// static HotpatchEntryV0                 s_hotpatch_entries[] = { ... };
    /// static constexpr HotpatchModuleV0      s_hotpatch_module = { ... };
    /// static const CHAOS_IL2CPP_UINT32 s_hotpatch_registered = [](){
    ///     RegisterHotpatchModule(&amp;s_hotpatch_module);
    ///     return 1u;
    /// }();
    /// </code>
    /// </summary>
    internal string BuildHotpatchTable(
        IReadOnlyList<AotCoreIrMethodArtifact> reachableMethods,
        MetadataRegistrationArtifact metadataRegistration)
    {
        var allMethods = GetHotpatchableMethods();
        HotpatchEntryCount = allMethods.Count;
        if (allMethods.Count == 0)
        {
            // No methods — emit nullptr so the linker always resolves the symbol.
            var emptyModel = new ScriptObject
            {
                ["is_empty"] = true,
            };
            return ScribanTemplateRenderer.RenderTemplate(
                NativeAotTemplateCatalog.GetHotpatchTableTemplate(), emptyModel);
        }

        var tokenLookup = new MetadataTokenLookup(metadataRegistration.Registrations);

        // Collect (type_name, type_namespace, method_name, token, native_symbol, param_count, subject_id) tuples.
        int syntheticTokenCounter = 1;
        var entries = new List<(string TypeName, string TypeNamespace, string MethodName, uint Token, string NativeSymbol, int ParamCount, string SubjectId)>();
        foreach (var method in allMethods)
        {
            string typeSubjectId;
            try
            {
                typeSubjectId = GetMethodDeclaringTypeSubjectId(method.SubjectId);
            }
            catch
            {
                continue;
            }

            var typeName = GetTypeDisplayName(typeSubjectId);
            var typeNamespace = GetTypeNamespace(typeSubjectId);
            var methodName = GetMethodName(method.SubjectId);
            uint token = tokenLookup.TryGetMethodToken(method.SubjectId);
            if (token == 0)
            {
                // Assign synthetic token matching BuildDispatchSlotMap allocation.
                token = 0x80000000u | (uint)(syntheticTokenCounter++);
            }

            entries.Add((typeName, typeNamespace, methodName, token, method.NativeSymbol, method.ParameterCount, method.SubjectId));
        }

        if (entries.Count == 0)
            return string.Empty;

        // Group by (namespace, type_name) tuple and sort lexicographically.
        var grouped = entries
            .GroupBy(e => (e.TypeNamespace, e.TypeName))
            .OrderBy(g => g.Key.TypeNamespace, StringComparer.Ordinal)
            .ThenBy(g => g.Key.TypeName, StringComparer.Ordinal)
            .ToList();

        // Determine kHotpatchKeepNative flag per-method.
        var methodsCallingExternal = new HashSet<string>(StringComparer.Ordinal);
        foreach (var method in allMethods)
        {
            foreach (var instruction in method.Instructions)
            {
                if (!string.IsNullOrEmpty(instruction.Callee) &&
                    _externalRuntimeSubjects.ContainsKey(
                        ManagedNaming.NormalizeSubjectIdAssembly(instruction.Callee)))
                {
                    methodsCallingExternal.Add(method.SubjectId);
                    break;
                }
            }
        }

        // Token->Slot mapping (sorted by token for bsearch)
        var tokenSlotList = entries
            .Select((e, idx) => (Token: e.Token, Slot: (uint)idx))
            .OrderBy(ts => ts.Token)
            .ToList();

        // --- Build Scriban model ---

        // Type groups with nested method models
        var typeGroupModels = new ScriptObject[grouped.Count];
        uint methodIndex = 0;
        for (int gi = 0; gi < grouped.Count; gi++)
        {
            var group = grouped[gi];
            var methodModels = new ScriptObject[group.Count()];
            int mi = 0;
            foreach (var entry in group)
            {
                methodModels[mi] = new ScriptObject
                {
                    ["method_name_literal"] = EscapeCppStringLiteral(entry.MethodName),
                    ["token_hex"] = entry.Token.ToString("X8"),
                    ["param_count"] = (ushort)entry.ParamCount,
                };
                mi++;
            }
            typeGroupModels[gi] = new ScriptObject
            {
                ["type_name"] = group.Key.TypeName,
                ["type_name_literal"] = EscapeCppStringLiteral(group.Key.TypeName),
                ["type_namespace_literal"] = EscapeCppStringLiteral(group.Key.TypeNamespace),
                ["first_method_index"] = methodIndex,
                ["method_count"] = group.Count(),
                ["methods"] = methodModels,
            };
            methodIndex += (uint)group.Count();
        }

        // Flat dispatch entry models
        var entryModels = new ScriptObject[entries.Count];
        for (int i = 0; i < entries.Count; i++)
        {
            var entry = entries[i];
            // Build param_count from entry.ParamCount (set by GetHotpatchableMethods).
            int paramCount = Math.Max(0, entry.ParamCount);
            string flags = _codegenMode.HasFlag(CodegenMode.Jit)
                ? "0"
                : methodsCallingExternal.Contains(entry.SubjectId)
                    ? "kHotpatchKeepNative | HotpatchEncodeArgCount(" + paramCount + ")"
                    : "HotpatchEncodeArgCount(" + paramCount + ")";
            entryModels[i] = new ScriptObject
            {
                ["native_symbol"] = entry.NativeSymbol,
                ["type_name"] = entry.TypeName,
                ["method_name"] = entry.MethodName,
                ["flags"] = flags,
            };
        }

        // Token->Slot models
        var tokenSlotModels = new ScriptObject[tokenSlotList.Count];
        for (int i = 0; i < tokenSlotList.Count; i++)
        {
            tokenSlotModels[i] = new ScriptObject
            {
                ["token_hex"] = tokenSlotList[i].Token.ToString("X8"),
                ["slot"] = tokenSlotList[i].Slot,
            };
        }

        // Reverse P/Invoke entry models
        var reversePInvokeModels = new ScriptObject[_reversePInvokeEntries.Count];
        for (int i = 0; i < _reversePInvokeEntries.Count; i++)
        {
            reversePInvokeModels[i] = new ScriptObject
            {
                ["native_symbol"] = _reversePInvokeEntries[i].NativeSymbol,
            };
        }

        // --- CCW interface vtable data ---
        // Collect unique declaring type subject IDs that have a COM interface GUID.
        // V2 emits: GUID byte-array constant + vtable array + factory function.
        var ccwInterfaceModels = new List<ScriptObject>();
        if (_comInterfaceVtableData.Count > 0)
        {
            var seenTypes = new HashSet<string>(StringComparer.Ordinal);
            foreach (var method in allMethods)
            {
                string declaringTypeSubjectId;
                try { declaringTypeSubjectId = GetMethodDeclaringTypeSubjectId(method.SubjectId); }
                catch { continue; }

                if (!seenTypes.Add(declaringTypeSubjectId)) continue;

                if (_comInterfaceVtableData.TryGetValue(declaringTypeSubjectId, out var vtableInfo))
                {
                    // Convert "ABCDEF01-2345-6789-ABCD-EF0123456789" -> GUID bytes.
                    var guidBytes = ParseGuidStringToBytes(vtableInfo.Guid);
                    if (guidBytes != null)
                    {
                        string typeName = GetTypeDisplayName(declaringTypeSubjectId);
                        string typeNamespace = GetTypeNamespace(declaringTypeSubjectId);
                        string safeName = SanitizeCppIdentifier(typeName) + "_" +
                            SanitizeCppIdentifier(typeNamespace);

                        var methodModels = new List<ScriptObject>();
                        for (int mi = 0; mi < vtableInfo.Methods.Length; mi++)
                        {
                            var slot = vtableInfo.Methods[mi];
                            methodModels.Add(new ScriptObject
                            {
                                ["native_symbol"] = slot.NativeSymbol,
                                ["slot_index"] = mi,
                                ["param_count"] = slot.ParamCount,
                                ["token"] = "0x" + slot.Token.ToString("X8") + "u",
                            });
                        }

                        var methodNames = vtableInfo.Methods.Select(m => m.MethodName).ToArray();

                        ccwInterfaceModels.Add(new ScriptObject
                        {
                            ["guid_bytes"] = string.Join(", ", guidBytes.Select(b => $"0x{b:X2}u")),
                            ["guid_symbol_suffix"] = safeName,
                            ["type_name"] = typeName,
                            ["type_namespace"] = typeNamespace,
                            ["method_slot_count"] = vtableInfo.Methods.Length,
                            ["methods"] = methodModels,
                            ["stable_id"] = "CHAOS_IL2CPP_UINT64_C(0x" + vtableInfo.StableId.ToString("X16") + ")",
                            ["is_idispatch"] = vtableInfo.IsDispatch,
                            ["method_names"] = methodNames,
                        });
                    }
                }
            }
        }

        var model = new ScriptObject
        {
            ["is_empty"] = false,
            ["assembly_name_literal"] = EscapeCppStringLiteral(_assemblyName),
            ["total_method_count"] = entries.Count,
            ["type_groups"] = typeGroupModels,
            ["type_group_count"] = grouped.Count,
            ["entries"] = entryModels,
            ["token_slots"] = tokenSlotModels,
            ["slot_count"] = tokenSlotList.Count,
            ["reverse_pinvoke_count"] = _reversePInvokeEntries.Count,
            ["reverse_pinvoke_entries"] = reversePInvokeModels,
            ["ccw_interface_count"] = ccwInterfaceModels.Count,
            ["ccw_interfaces"] = ccwInterfaceModels,
        };

        return ScribanTemplateRenderer.RenderTemplate(
            NativeAotTemplateCatalog.GetHotpatchTableTemplate(), model);
    }

    /// <summary>
    /// Splits the hotpatch method-name index into bounded chunks so the emitted
    /// text stays within the per-translation-unit budget.
    ///
    /// <para>
    /// The hotpatch block is one coherent unit — <c>s_hotpatch_module</c> points at
    /// all of <c>s_hotpatch_methods</c>, <c>s_hotpatch_types</c>,
    /// <c>s_hotpatch_slots</c> and <c>s_hotpatch_entries</c> — and
    /// <c>s_hotpatch_types[i].first_method_index</c> indexes into
    /// <c>s_hotpatch_methods</c> <b>globally</b>. That is why chunks are emitted
    /// with an explicit base offset rather than being re-indexed: the global index
    /// space must be preserved exactly, or name lookups silently resolve to the
    /// wrong method.
    /// </para>
    ///
    /// <para>
    /// Returns <c>null</c> when the block is small enough to emit whole, so the
    /// common case keeps its existing single-file shape.
    /// </para>
    /// </summary>
    internal static (IReadOnlyList<string> ChunkNames, IReadOnlyList<int> ChunkOffsets, IReadOnlyList<int> ChunkCounts)?
        PlanHotpatchMethodChunks(int totalMethodCount, int estimatedCharsPerMethod, int budgetChars)
    {
        if (totalMethodCount <= 0 || estimatedCharsPerMethod <= 0 || budgetChars <= 0)
            return null;

        int totalChars = totalMethodCount * estimatedCharsPerMethod;
        if (totalChars <= budgetChars)
            return null;   // fits in one TU — nothing to do

        int perChunk = Math.Max(1, budgetChars / estimatedCharsPerMethod);
        int chunkCount = (totalMethodCount + perChunk - 1) / perChunk;

        var names = new List<string>(chunkCount);
        var offsets = new List<int>(chunkCount);
        var counts = new List<int>(chunkCount);

        for (int i = 0; i < chunkCount; i++)
        {
            int offset = i * perChunk;
            int count = Math.Min(perChunk, totalMethodCount - offset);
            names.Add($"s_hotpatch_methods_{i}");
            offsets.Add(offset);
            counts.Add(count);
        }

        return (names, offsets, counts);
    }

    // --- CustomAttribute blob data emission ---
    // Builds the binary blob, offset array, and materializer switch for
    // per-module CustomAttribute query support. Supports 5 entity kinds:
    // Type, Method, Field, Property, Param — all share a single blob stream
    // with separate prefix-sum offset arrays.

    private static uint CustomAttributeFieldSize(CustomAttributeLiteralKind kind)
    {
        return kind switch
        {
            CustomAttributeLiteralKind.Null => 0u,
            CustomAttributeLiteralKind.Boolean => 1u,
            CustomAttributeLiteralKind.Byte => 1u,
            CustomAttributeLiteralKind.SByte => 1u,
            CustomAttributeLiteralKind.Int16 => 2u,
            CustomAttributeLiteralKind.Int32 => 4u,
            CustomAttributeLiteralKind.Int64 => 8u,
            CustomAttributeLiteralKind.UInt16 => 2u,
            CustomAttributeLiteralKind.UInt32 => 4u,
            CustomAttributeLiteralKind.UInt64 => 8u,
            CustomAttributeLiteralKind.Single => 4u,
            CustomAttributeLiteralKind.Double => 8u,
            CustomAttributeLiteralKind.Char => 2u,
            CustomAttributeLiteralKind.String => 2u, // only the length prefix; data is variable
            CustomAttributeLiteralKind.Type => 2u,   // length prefix only in packed data
            CustomAttributeLiteralKind.Enum => 1u,   // underlying kind byte + value (deferred)
            _ => 0u,
        };
    }

    private byte[] EncodeCustomAttributeFieldValue(CustomAttributeLiteralValue value)
    {
        if (value.Kind == CustomAttributeLiteralKind.Null)
            return Array.Empty<byte>();

        return value.Kind switch
        {
            CustomAttributeLiteralKind.Boolean => new[] { (byte)(((bool)value.Value!) ? 1 : 0) },
            CustomAttributeLiteralKind.Byte => new[] { (byte)value.Value! },
            CustomAttributeLiteralKind.SByte => new[] { unchecked((byte)((sbyte)value.Value!)) },
            CustomAttributeLiteralKind.Int16 => BitConverter.GetBytes((short)value.Value!),
            CustomAttributeLiteralKind.Int32 => BitConverter.GetBytes((int)value.Value!),
            CustomAttributeLiteralKind.Int64 => BitConverter.GetBytes((long)value.Value!),
            CustomAttributeLiteralKind.UInt16 => BitConverter.GetBytes((ushort)value.Value!),
            CustomAttributeLiteralKind.UInt32 => BitConverter.GetBytes((uint)value.Value!),
            CustomAttributeLiteralKind.UInt64 => BitConverter.GetBytes((ulong)value.Value!),
            CustomAttributeLiteralKind.Single => BitConverter.GetBytes((float)value.Value!),
            CustomAttributeLiteralKind.Double => BitConverter.GetBytes((double)value.Value!),
            CustomAttributeLiteralKind.Char => BitConverter.GetBytes((char)value.Value!),
            CustomAttributeLiteralKind.String when value.Value != null
                => EncodePackedString((string)value.Value!),
            CustomAttributeLiteralKind.String => new byte[] { 0, 0 }, // empty string
            CustomAttributeLiteralKind.Type when value.Value is string typeSubjectId
                => BitConverter.GetBytes(GetTypeTokenForSubjectId(typeSubjectId)),
            CustomAttributeLiteralKind.Type => new byte[] { 0, 0, 0, 0 },
            CustomAttributeLiteralKind.Enum when value.Value != null
                => EncodeEnumValue(value.Value),
            CustomAttributeLiteralKind.Enum => new byte[] { 0 },
            _ => throw new NotSupportedException($"unknown CA literal kind {value.Kind}"),
        };
    }

    private static byte[] EncodeEnumValue(object value)
    {
        return value switch
        {
            int i => BitConverter.GetBytes(i),
            long l => BitConverter.GetBytes(l),
            short s => BitConverter.GetBytes(s),
            byte b => new[] { b },
            sbyte sb => new[] { unchecked((byte)sb) },
            ushort us => BitConverter.GetBytes(us),
            uint ui => BitConverter.GetBytes(ui),
            ulong ul => BitConverter.GetBytes(ul),
            _ => BitConverter.GetBytes(Convert.ToInt32(value)),
        };
    }

    private static byte[] EncodePackedString(string s)
    {
        var utf8 = System.Text.Encoding.UTF8.GetBytes(s);
        if (utf8.Length > 65535)
            throw new NotSupportedException("CA string field too long (>65535 UTF-8 bytes)");
        var result = new byte[2 + utf8.Length];
        result[0] = (byte)(utf8.Length & 0xFF);
        result[1] = (byte)((utf8.Length >> 8) & 0xFF);
        Buffer.BlockCopy(utf8, 0, result, 2, utf8.Length);
        return result;
    }

    internal string BuildCustomAttributeBlobAndMaterializer()
    {
        var materializations = _customAttributeSupport.Materializations;
        if (materializations.Count == 0 ||
            _moduleTypeCount == 0 ||
            _moduleTypeSubjectIds.Count != _moduleTypeCount)
            return string.Empty;

        // Group materializations by target kind
        var kindGroups = materializations
            .ToLookup(m => m.TargetKind);

        // Collect unique attribute types for the materializer switch (shared across all kinds)
        var uniqueAttrTypes = new List<(string SubjectId, uint Token, List<(string FieldSubjectId, CustomAttributeLiteralValue Value)> Fields)>();
        var attrTypeKeySet = new HashSet<string>(StringComparer.Ordinal);
        foreach (var plan in materializations)
        {
            if (attrTypeKeySet.Add(plan.AttributeTypeSubjectId))
            {
                var fieldSet = new Dictionary<string, CustomAttributeLiteralValue>(StringComparer.Ordinal);
                foreach (var a in plan.Assignments)
                    fieldSet[a.FieldSubjectId] = a.Value;
                uniqueAttrTypes.Add((plan.AttributeTypeSubjectId,
                    GetTypeTokenForSubjectId(plan.AttributeTypeSubjectId),
                    fieldSet.Select(kv => (kv.Key, kv.Value)).ToList()));
            }
        }

        // Read metadata table row counts (for offset array sizing of each entity kind).
        uint methodCount = 0, fieldCount = 0, propertyCount = 0, paramCount = 0;
        try
        {
            if (_closureAssemblyPathByName.TryGetValue(_assemblyName, out var assemblyPath) &&
                File.Exists(assemblyPath))
            {
                using var stream = File.OpenRead(assemblyPath);
                using var peReader = new PEReader(stream);
                if (peReader.HasMetadata)
                {
                    var metadataReader = peReader.GetMetadataReader();
                    methodCount = (uint)metadataReader.GetTableRowCount(TableIndex.MethodDef);
                    fieldCount = (uint)metadataReader.GetTableRowCount(TableIndex.Field);
                    propertyCount = (uint)metadataReader.GetTableRowCount(TableIndex.Property);
                    paramCount = (uint)metadataReader.GetTableRowCount(TableIndex.Param);
                }
            }
        }
        catch
        {
            // If metadata reading fails, counts stay 0 (offset arrays will be nullptr/0).
        }

        // Build per-kind entity index -> plans mapping
        var typeIndexToPlans = BuildKindEntityMap(kindGroups, CustomAttributeTargetKind.Type, _moduleTypeCount);
        var methodIndexToPlans = BuildKindEntityMap(kindGroups, CustomAttributeTargetKind.Method, (int)methodCount);
        var fieldIndexToPlans = BuildKindEntityMap(kindGroups, CustomAttributeTargetKind.Field, (int)fieldCount);
        var propertyIndexToPlans = BuildKindEntityMap(kindGroups, CustomAttributeTargetKind.Property, (int)propertyCount);
        var paramIndexToPlans = BuildKindEntityMap(kindGroups, CustomAttributeTargetKind.Param, (int)paramCount);

        // Build blob stream and offset arrays
        var blobStream = new MemoryStream();
        var typeOffsets = BuildKindOffsetArray(blobStream, typeIndexToPlans, _moduleTypeCount);
        var methodOffsets = methodCount > 0
            ? BuildKindOffsetArray(blobStream, methodIndexToPlans, (int)methodCount) : null;
        var fieldOffsets = fieldCount > 0
            ? BuildKindOffsetArray(blobStream, fieldIndexToPlans, (int)fieldCount) : null;
        var propertyOffsets = propertyCount > 0
            ? BuildKindOffsetArray(blobStream, propertyIndexToPlans, (int)propertyCount) : null;
        var paramOffsets = paramCount > 0
            ? BuildKindOffsetArray(blobStream, paramIndexToPlans, (int)paramCount) : null;

        var sb = new StringBuilder(4096);
        sb.AppendLine("// --- CustomAttribute blob ---");

        // Emit blob as constexpr CHAOS_IL2CPP_UINT8[]
        var blobBytes = blobStream.ToArray();
        if (blobBytes.Length > 0)
        {
            sb.Append("static constexpr CHAOS_IL2CPP_UINT8 s_custom_attribute_blob[")
                .Append(blobBytes.Length).AppendLine("] =");
            sb.AppendLine("{");
            for (int i = 0; i < blobBytes.Length; i++)
            {
                if (i > 0 && i % 24 == 0) sb.AppendLine();
                sb.Append("0x").Append(blobBytes[i].ToString("X2")).Append("u,");
            }
            sb.AppendLine();
            sb.AppendLine("};");
            sb.AppendLine();
        }

        // Emit type offset array (always present with _moduleTypeCount)
        EmitOffsetArray(sb, "s_custom_attribute_offset", typeOffsets, _moduleTypeCount + 1);

        // Emit method offset array
        if (methodOffsets != null)
            EmitOffsetArray(sb, "s_custom_attribute_method_offset", methodOffsets, (int)methodCount + 1);
        else if (blobBytes.Length > 0)
            EmitOffsetArray(sb, "s_custom_attribute_method_offset", new uint[(int)methodCount + 1], (int)methodCount + 1);

        // Emit field offset array
        if (fieldOffsets != null)
            EmitOffsetArray(sb, "s_custom_attribute_field_offset", fieldOffsets, (int)fieldCount + 1);
        else if (blobBytes.Length > 0)
            EmitOffsetArray(sb, "s_custom_attribute_field_offset", new uint[(int)fieldCount + 1], (int)fieldCount + 1);

        // Emit property offset array
        if (propertyOffsets != null)
            EmitOffsetArray(sb, "s_custom_attribute_property_offset", propertyOffsets, (int)propertyCount + 1);
        else if (blobBytes.Length > 0)
            EmitOffsetArray(sb, "s_custom_attribute_property_offset", new uint[(int)propertyCount + 1], (int)propertyCount + 1);

        // Emit param offset array
        if (paramOffsets != null)
            EmitOffsetArray(sb, "s_custom_attribute_param_offset", paramOffsets, (int)paramCount + 1);
        else if (blobBytes.Length > 0)
            EmitOffsetArray(sb, "s_custom_attribute_param_offset", new uint[(int)paramCount + 1], (int)paramCount + 1);

        // Emit count constants for ModuleDescriptor initialization
        sb.Append("static constexpr CHAOS_IL2CPP_UINT32 s_custom_attribute_method_count = ")
            .Append(methodCount).AppendLine("u;");
        sb.Append("static constexpr CHAOS_IL2CPP_UINT32 s_custom_attribute_field_count = ")
            .Append(fieldCount).AppendLine("u;");
        sb.Append("static constexpr CHAOS_IL2CPP_UINT32 s_custom_attribute_property_count = ")
            .Append(propertyCount).AppendLine("u;");
        sb.Append("static constexpr CHAOS_IL2CPP_UINT32 s_custom_attribute_param_count = ")
            .Append(paramCount).AppendLine("u;");
        sb.AppendLine();

        // Emit materializer switch (shared by all kinds)
        sb.AppendLine("static CHAOS_IL2CPP_INTPTR ModuleCustomAttributeMaterializer(");
        sb.AppendLine("    CHAOS_IL2CPP_UINT32 attr_type_token, const CHAOS_IL2CPP_UINT8* field_data)");
        sb.AppendLine("{");
        sb.AppendLine("    switch (attr_type_token)");
        sb.AppendLine("    {");

        foreach (var (attrSubjectId, attrToken, fields) in uniqueAttrTypes)
        {
            uint tokenForSwitch = GetTypeTokenForSubjectId(attrSubjectId);
            sb.Append("        case 0x").Append(tokenForSwitch.ToString("X8")).AppendLine("u:");
            sb.AppendLine("        {");
            sb.Append("            auto* attr = CHAOS_IL2CPP_NEW_GC(")
                .Append(GetNativeTypeSymbol(attrSubjectId)).AppendLine(");");
            sb.Append("            attr->header.type_info = ")
                .Append(GetNativeTypeInfoSymbol(attrSubjectId)).AppendLine(";");

            if (_vtableTypes?.Contains(attrSubjectId) == true)
            {
                sb.Append("            attr->header.vtable = ")
                    .Append(GetNativeVTableSymbol(attrSubjectId)).AppendLine(";");
            }

            if (fields.Count > 0)
            {
                sb.AppendLine("            const CHAOS_IL2CPP_UINT8* p = field_data;");
                foreach (var (fieldSubjectId, fieldValue) in fields)
                {
                    string fieldName = GetNativeFieldMemberName(fieldSubjectId);
                    string decodeCode = EmitFieldDecodeStmt(fieldName, fieldValue, attrSubjectId);
                    sb.Append("            ").AppendLine(decodeCode);
                }
            }

            sb.AppendLine("            return reinterpret_cast<CHAOS_IL2CPP_INTPTR>(attr);");
            sb.AppendLine("        }");
        }

        sb.AppendLine("        default:");
        sb.AppendLine("            return 0;");
        sb.AppendLine("    }");
        sb.AppendLine("}");

        // Cache for ModuleDescriptor emission
        _hasCustomAttributeBlob = true;

        return sb.ToString();
    }

    /// <summary>
    /// Build a map from entity index (TokenToIndex) to list of materialization plans
    /// for the given kind. Plans whose token maps to an index >= entityCount are excluded.
    /// </summary>
    private static Dictionary<int, List<CustomAttributeMaterializationPlan>> BuildKindEntityMap(
        ILookup<CustomAttributeTargetKind, CustomAttributeMaterializationPlan>? kindGroups,
        CustomAttributeTargetKind kind,
        int entityCount)
    {
        var result = new Dictionary<int, List<CustomAttributeMaterializationPlan>>();

        if (kindGroups == null)
            return result;

        var plans = kindGroups[kind];
        foreach (var plan in plans)
        {
            int entityIdx = (int)((plan.TargetMetadataToken & 0x00FFFFFFu) - 1);
            if (entityIdx < 0 || entityIdx >= entityCount)
                continue;

            if (!result.TryGetValue(entityIdx, out var list))
            {
                list = new List<CustomAttributeMaterializationPlan>();
                result[entityIdx] = list;
            }
            list.Add(plan);
        }

        return result;
    }

    /// <summary>
    /// Build a prefix-sum offset array for a given entity kind. Each entity index
    /// maps to a byte offset in the shared blob stream. Entities with no attributes
    /// get offset[i] == offset[i+1] (zero-length segment). Data is appended to the
    /// shared blobStream.
    /// </summary>
    private uint[] BuildKindOffsetArray(
        MemoryStream blobStream,
        Dictionary<int, List<CustomAttributeMaterializationPlan>> entityIndexToPlans,
        int entityCount)
    {
        if (entityCount <= 0)
            return [];

        var offsets = new uint[entityCount + 1];
        uint currentOffset = (uint)blobStream.Length;

        for (int ei = 0; ei < entityCount; ei++)
        {
            offsets[ei] = currentOffset;

            if (entityIndexToPlans.TryGetValue(ei, out var plans) && plans.Count > 0)
            {
                // Emit attr_count + per-attribute data
                var attrCountBytes = BitConverter.GetBytes((ushort)plans.Count);
                blobStream.Write(attrCountBytes, 0, 2);
                currentOffset += 2;

                foreach (var plan in plans)
                {
                    // For the attribute type token in the blob, use GetTypeTokenForSubjectId
                    // (resolves to the TypeDef token of the attribute type).
                    uint attrTypeToken = GetTypeTokenForSubjectId(plan.AttributeTypeSubjectId);
                    var tokenBytes = BitConverter.GetBytes(attrTypeToken);
                    blobStream.Write(tokenBytes, 0, 4);
                    currentOffset += 4;

                    // Encode field values and compute packed_size
                    using var fieldStream = new MemoryStream();
                    foreach (var assignment in plan.Assignments)
                    {
                        var fieldBytes = EncodeCustomAttributeFieldValue(assignment.Value);
                        fieldStream.Write(fieldBytes, 0, fieldBytes.Length);
                    }

                    var fieldData = fieldStream.ToArray();
                    ushort packedSize = (ushort)fieldData.Length;
                    var sizeBytes = BitConverter.GetBytes(packedSize);
                    blobStream.Write(sizeBytes, 0, 2);
                    blobStream.Write(fieldData, 0, fieldData.Length);
                    currentOffset += 2 + (uint)fieldData.Length;
                }
            }
        }
        offsets[entityCount] = currentOffset;

        return offsets;
    }

    /// <summary>
    /// Emit a constexpr offset array to the StringBuilder.
    /// </summary>
    private static void EmitOffsetArray(StringBuilder sb, string name, uint[] offsets, int count)
    {
        sb.Append("static constexpr CHAOS_IL2CPP_UINT32 ").Append(name)
            .Append("[").Append(count).AppendLine("] =");
        sb.AppendLine("{");
        for (int i = 0; i < count; i++)
        {
            sb.Append("    ").Append(offsets[i]).Append("u,");
            sb.AppendLine();
        }
        sb.AppendLine("};");
        sb.AppendLine();
    }

    private Dictionary<string, int>? _typeTokenCache;

    private uint GetTypeTokenForSubjectId(string subjectId)
    {
        // Build O(1) lookup cache on first access
        if (_typeTokenCache == null)
        {
            _typeTokenCache = new Dictionary<string, int>(_moduleTypeSubjectIds.Count, StringComparer.Ordinal);
            for (int i = 0; i < _moduleTypeSubjectIds.Count; i++)
                _typeTokenCache[_moduleTypeSubjectIds[i]] = i;
        }

        // Fast path: direct subjectId lookup
        if (_typeTokenCache.TryGetValue(subjectId, out int index))
            return 0x02000000u | (uint)(index + 1);

        // Fallback: look up via the name-based subject ID format
        // (for types not in this module, like CoreLib attribute types)
        var parts = subjectId.Split('/');
        if (parts.Length == 2)
        {
            string fullName = parts[1];
            for (int i = 0; i < _moduleTypeCount; i++)
            {
                string ns = _moduleTypeNamespaces[i];
                string name = _moduleTypeNames[i];
                string candidate = string.IsNullOrEmpty(ns) ? name : $"{ns}.{name}";
                if (candidate == fullName)
                    return 0x02000000u | (uint)(i + 1);
            }
        }
        return 0; // not found in this module
    }

    /// <summary>
    /// Emit the external runtime dispatch table: a startup-time-resolved function
    /// pointer table for cross-assembly calls that would otherwise fall through to
    /// chaos_external_runtime_* stub generation.
    ///
    /// Generated C++ pattern:
    /// <code>
    /// extern "C" const char* kChaosExternalRuntimeSubjects[] = { "subj1", "subj2", ... };
    /// extern "C" void* kChaosExternalRuntimeFnTable[2] = { nullptr, nullptr };
    /// extern "C" int32_t kChaosExternalRuntimeCount = 2;
    /// </code>
    ///
    /// For entries that have a corresponding chaos_external_runtime_* helper function
    /// (shaped helpers defined in the same TU), the function pointer is pre-filled
    /// at compile time.  Other entries remain nullptr and are resolved at startup
    /// by ChaosResolveExternalRuntimeFnTable() via the HotpatchNameRegistry.
    ///
    /// The interpreter's ResolveDirectFn uses kChaosExternalRuntimeFnTable as a
    /// third fallback (after AotDirectDispatch and HotpatchNameRegistry) so that
    /// patched methods can call cross-assembly functions correctly.
    /// </summary>

    /// <summary>
    /// Converts a standard GUID string (e.g., "ABCDEF01-2345-6789-ABCD-EF0123456789")
    /// to a 16-byte array. Returns null if the input is not a valid 36-char GUID.
    /// Handles both with and without braces.
    /// </summary>

    /// <summary>
    /// Emit JIT method entry table for JIT compilation dispatch.
    /// Generated when codegen mode is Jit. Produces:
    /// <code>
    /// extern "C" const JitEntry kChaosJitEntries[] = { ... };
    /// extern "C" const uint32_t kChaosJitEntryCount = N;
    /// </code>
    ///
    /// When jitDataOutputPath is non-null, the AotCoreIr JSON data is written
    /// to a binary .jdata file and the JitEntry uses offset-based addressing
    /// (json_offset, json_len). When null, the old C++ string literal format
    /// is used (kJitJson_N, backward compat).
    ///
    /// Each entry contains the full AotCoreIr JSON serialized as a C++ string literal,
    /// plus the metadata token and module_id for slot lookup.
    ///
    /// Unlike the old BuildJitMethodRegistration (which routed through interpreter dispatch),
    /// this builds entries for JIT compilation — first call triggers JitStubDispatchImpl
    /// → Compile() → direct_ptr patched to compiled native code.
    /// </summary>

    /// <summary>
    /// Build the reflection member descriptor tables (properties / fields / events)
    /// as a C++ block, plus the per-type <c>ReflectionQueryTypeDescriptor</c> array
    /// and the registration call that hands them to the runtime.
    ///
    /// These tables are what <c>Type::GetProperty</c> / <c>GetField</c> / <c>GetEvent</c>
    /// resolve against.  Without them those accessors are deliberate
    /// <c>CHAOS_IL2CPP_FAIL()</c> stubs (<c>RuntimeHelperShapeRegistry.CoreStubs.Part1.S15.cs</c>)
    /// and any member query aborts the process.
    ///
    /// Emitted per closure-reachable type (see
    /// <c>CollectReflectionMemberMetadataFromClosure</c>) rather than for the whole
    /// BCL — the library's full member set is props=5128 / fields=9011, while a
    /// chunk observes a few dozen types.
    ///
    /// Registration goes through the existing <c>ChaosRegisterExternalType</c>, whose
    /// hash-keyed dynamic table the type-resolution path already consults.  No ABI
    /// structure is extended, so ModuleDescriptor layout is untouched.
    /// </summary>
    private string BuildReflectionMemberDescriptorTables()
    {
        int total = _reflectionProperties.Count + _reflectionFields.Count
            + _reflectionEvents.Count + _reflectionMethods.Count;
        if (total == 0) return string.Empty;

        var sb = new StringBuilder(4096);
        string ind = ScribanTemplateRenderer.Indentation(1);
        string tab = ind + "    ";

        // Group members by declaring type so each type's descriptor can point at its
        // own contiguous slice.  Types are emitted in a stable order for reproducible
        // generated sources.
        var propsByType = _reflectionProperties.GroupBy(p => p.TypeSubjectId)
            .OrderBy(g => g.Key, StringComparer.Ordinal).ToList();
        var fieldsByType = _reflectionFields.GroupBy(f => f.TypeSubjectId)
            .OrderBy(g => g.Key, StringComparer.Ordinal).ToList();
        var eventsByType = _reflectionEvents.GroupBy(e => e.TypeSubjectId)
            .OrderBy(g => g.Key, StringComparer.Ordinal).ToList();

        // Methods + their parameter arrays: Type::GetMethod(name, Type[]) resolves
        // against the type's method table, and GetParameters()[i] reads the method's
        // parameter descriptors.  Both were absent (methods = nullptr / 0), so
        // GetMethod found nothing and GetParameters returned empty.
        var methodsByType = _reflectionMethods.GroupBy(m => m.TypeSubjectId)
            .OrderBy(g => g.Key, StringComparer.Ordinal).ToList();
        var paramsByMethod = _reflectionMethodParams.GroupBy(p => p.MethodSubjectId)
            .OrderBy(g => g.Key, StringComparer.Ordinal).ToList();

        // ── Per-method parameter arrays ──
        foreach (var g in paramsByMethod)
        {
            string sym = ReflectionMemberSymbol("params", g.Key);
            sb.AppendLine($"{ind}static const chaos::il2cpp::runtime_core::ReflectionQueryParameterDescriptor {sym}[] = {{");
            foreach (var m in g.OrderBy(x => x.ParamIndex))
                sb.AppendLine($"{tab}{{ \"{EscapeCppStringLiteral(m.ParamSubjectId)}\", \"{EscapeCppStringLiteral(m.ParamName)}\", {m.ParamIndex}u, \"{EscapeCppStringLiteral(m.ParamTypeName)}\", 0, 0u }},");
            sb.AppendLine($"{ind}}};");
        }

        // ── Per-type method arrays ──
        foreach (var g in methodsByType)
        {
            string sym = ReflectionMemberSymbol("methods", g.Key);
            sb.AppendLine($"{ind}static const chaos::il2cpp::runtime_core::ReflectionQueryMethodDescriptor {sym}[] = {{");
            foreach (var m in g.OrderBy(x => x.Token))
            {
                var paramSym = paramsByMethod.Any(pg => pg.Key == m.MethodSubjectId)
                    ? ReflectionMemberSymbol("params", m.MethodSubjectId)
                    : "nullptr";
                var paramCount = paramsByMethod.FirstOrDefault(pg => pg.Key == m.MethodSubjectId)?.Count() ?? 0;
                sb.AppendLine($"{tab}{{ {m.Token}u, \"{EscapeCppStringLiteral(m.MethodSubjectId)}\", \"{EscapeCppStringLiteral(m.Name)}\", \"{EscapeCppStringLiteral(m.ReturnTypeName)}\", {m.ParamCount}, {paramSym}, {paramCount}u, nullptr, {m.Flags}u }},");
            }
            sb.AppendLine($"{ind}}};");
        }

        // ── Per-type member arrays ──
        foreach (var g in propsByType)
        {
            string sym = ReflectionMemberSymbol("props", g.Key);
            sb.AppendLine($"{ind}static const chaos::il2cpp::runtime_core::ReflectionQueryPropertyDescriptor {sym}[] = {{");
            foreach (var m in g.OrderBy(x => x.Name, StringComparer.Ordinal))
                sb.AppendLine($"{tab}{{ \"{EscapeCppStringLiteral(m.TypeSubjectId)}\", \"{EscapeCppStringLiteral(m.Name)}\", \"{EscapeCppStringLiteral(m.MemberType)}\", {m.Flags}u }},");
            sb.AppendLine($"{ind}}};");
        }
        foreach (var g in fieldsByType)
        {
            string sym = ReflectionMemberSymbol("fields", g.Key);
            sb.AppendLine($"{ind}static const chaos::il2cpp::runtime_core::ReflectionQueryFieldDescriptor {sym}[] = {{");
            foreach (var m in g.OrderBy(x => x.Name, StringComparer.Ordinal))
                sb.AppendLine($"{tab}{{ 0u, \"{EscapeCppStringLiteral(m.TypeSubjectId)}\", \"{EscapeCppStringLiteral(m.Name)}\", \"{EscapeCppStringLiteral(m.MemberType)}\", {m.ConstantValue}LL, {m.Flags}u }},");
            sb.AppendLine($"{ind}}};");
        }
        foreach (var g in eventsByType)
        {
            string sym = ReflectionMemberSymbol("events", g.Key);
            sb.AppendLine($"{ind}static const chaos::il2cpp::runtime_core::ReflectionQueryEventDescriptor {sym}[] = {{");
            foreach (var m in g.OrderBy(x => x.Name, StringComparer.Ordinal))
                sb.AppendLine($"{tab}{{ \"{EscapeCppStringLiteral(m.TypeSubjectId)}\", \"{EscapeCppStringLiteral(m.Name)}\", \"{EscapeCppStringLiteral(m.MemberType)}\", {m.Flags}u }},");
            sb.AppendLine($"{ind}}};");
        }

        // ── Per-type descriptors + registration ──
        var allTypes = propsByType.Select(g => g.Key)
            .Concat(fieldsByType.Select(g => g.Key))
            .Concat(eventsByType.Select(g => g.Key))
            .Concat(methodsByType.Select(g => g.Key))
            .Distinct(StringComparer.Ordinal)
            .OrderBy(t => t, StringComparer.Ordinal)
            .ToList();

        sb.AppendLine($"{ind}static void ChaosRegisterReflectionMembers() {{");
        foreach (var typeId in allTypes)
        {
            var p = propsByType.FirstOrDefault(g => g.Key == typeId);
            var f = fieldsByType.FirstOrDefault(g => g.Key == typeId);
            var e = eventsByType.FirstOrDefault(g => g.Key == typeId);
            var m = methodsByType.FirstOrDefault(g => g.Key == typeId);

            string descSym = ReflectionMemberSymbol("desc", typeId);
            // Fill type_info_ptr with the same TypeInfoHot* expression the A2.7
            // typeof fold pushes — GetNativeTypeInfoSymbol(typeId) yields
            // chaos_mt_<Type>.AsTypeInfoHot(), byte-identical to the fold's
            // operand.  The runtime's ChaosRegisterExternalType builds a
            // TypeInfoHot* → descriptor reverse map from this field, so
            // typeof-fold receivers resolve through GetMethod/GetProperty/etc.
            // Without it, a folded typeof value matches no handle encoding and
            // every member query returns 0.
            //
            // ONLY for types whose MethodTable symbol has a DEFINITION in the
            // generated sources: reference types in
            // _referenceTypeBaseSubjectIds plus emitted value-type structs.
            // (_allEmittedTypeSubjectIds is broader — it also covers types that
            // get only an extern chaos_mt_ DECLARATION, e.g. MemberInfo,
            // TypeInfo, Decimal — and emitting AsTypeInfoHot() for those
            // produces LNK2001 × N.  The failure is silent at pipeline level:
            // the fact stage keeps running against a stale entry.exe.)
            // The typeof fold can only produce TypeInfoHot* for
            // MethodTable-defined types anyway, so this is exactly the fold's
            // output domain.
            bool hasMethodTable =
                _referenceTypeBaseSubjectIds.ContainsKey(typeId)
                || (_valueTypeStructSubjectIds?.Contains(typeId) == true);
            string typeInfoExpr = hasMethodTable
                ? GetNativeTypeInfoSymbol(typeId)
                : "nullptr";
            sb.AppendLine($"{tab}static const chaos::il2cpp::runtime_core::ReflectionQueryTypeDescriptor {descSym} = {{");
            sb.AppendLine($"{tab}    0u, \"{EscapeCppStringLiteral(typeId)}\", \"{EscapeCppStringLiteral(typeId)}\", nullptr, nullptr, nullptr,");
            sb.AppendLine($"{tab}    nullptr,");
            sb.AppendLine($"{tab}    {(f is null ? "nullptr" : ReflectionMemberSymbol("fields", typeId))}, {(f is null ? 0 : f.Count())}u,");
            sb.AppendLine($"{tab}    {(p is null ? "nullptr" : ReflectionMemberSymbol("props", typeId))}, {(p is null ? 0 : p.Count())}u,");
            sb.AppendLine($"{tab}    {(e is null ? "nullptr" : ReflectionMemberSymbol("events", typeId))}, {(e is null ? 0 : e.Count())}u,");
            sb.AppendLine($"{tab}    {(m is null ? "nullptr" : ReflectionMemberSymbol("methods", typeId))}, {(m is null ? 0 : m.Count())}u,");
            sb.AppendLine($"{tab}    nullptr, 0u,");                  // generic_parameters, generic_param_count
            sb.AppendLine($"{tab}    0u, {typeInfoExpr},");           // reserved_flags, type_info_ptr
            sb.AppendLine($"{tab}}};");
            sb.AppendLine($"{tab}ChaosRegisterExternalType({ReflectionMemberFnv24(typeId)}u, &{descSym});");
        }
        sb.AppendLine($"{ind}}}");
        // Self-registering static initializer: the table must be live before any
        // subject runs, and this TU is linked into every entry.exe.
        sb.AppendLine($"{ind}namespace {{ const bool s_reflection_members_registered = (ChaosRegisterReflectionMembers(), true); }}");

        // ── B3 fallback resolvers for the managed reflection object model ──
        //
        // The object-model switches (chaos_reflection_resolve_method_handle /
        // ChaosReflectionGetParameters) are emitted inside
        // EmitObjectModelDeclarations — BEFORE
        // CollectReflectionMemberMetadataFromClosure populates
        // _reflectionMethods/_reflectionMethodParams — and they only cover the
        // supplemental RegisteredMethods (test-subject types).  BCL types
        // reachable through the closure (System.String::IndexOf, …) therefore
        // miss: GetMethod returned 0 → NRE (B7 layer 3).  The definitions below
        // are emitted here, AFTER collection, and forward-declared in the object
        // model TU; all handle literals are compile-time constants.
        if (_reflectionMethods.Count > 0)
        {
            var b3ParamNames = _reflectionMethodParams
                .GroupBy(p => p.MethodSubjectId, StringComparer.Ordinal)
                .ToDictionary(
                    g => g.Key,
                    g => g.OrderBy(p => p.ParamIndex)
                        .Select(p => p.ParamName)
                        .ToArray(),
                    StringComparer.Ordinal);

            var b3Entries = _reflectionMethods
                .Select(m => new ReflectionMemberMethodEntry(
                    m.MethodSubjectId,
                    m.TypeSubjectId,
                    m.Name,
                    b3ParamNames.TryGetValue(m.MethodSubjectId, out var names)
                        ? names
                        : Array.Empty<string>(),
                    false,
                    (int)m.Token))
                .ToList();

            // Method-handle resolver: one case per declaring type.
            //
            // This is 3.09 MB / 15,756 STRCMPs across 190 cases on the system
            // chunk, and it is unpartitionable by the method pager — it is not a
            // method in that sense, so it stays whole inside the `modulereg`
            // section no matter how methods are paged, pinning page-0001 above
            // the size target. Split into `_partN` functions, each its own payload
            // section. Case blocks are the atomic unit (first-match-wins STRCMP
            // chains; see ReflectionDispatchPartitioner).
            {
                var caseBlocks = new List<(string CaseText, int BranchCount)>();
                foreach (var b3Group in b3Entries
                    .GroupBy(e => e.DeclaringTypeSubjectId, StringComparer.Ordinal)
                    .OrderBy(g => g.Key, (IComparer<string>)StringComparer.Ordinal))
                {
                    if (string.Equals(b3Group.Key, "System.Private.CoreLib/System.Type", StringComparison.Ordinal) ||
                        string.Equals(b3Group.Key, "System.Private.CoreLib/System.Reflection.MethodInfo", StringComparison.Ordinal))
                    {
                        continue;
                    }
                    var caseSb = new StringBuilder();
                    caseSb.AppendLine($"        case {GetTypeHandleLiteral(b3Group.Key)}:");
                    foreach (var b3Method in b3Group.OrderBy(e => e.MethodSubjectId, StringComparer.Ordinal))
                    {
                        caseSb.AppendLine($"            if (CHAOS_IL2CPP_STRCMP(chaos_method_name, {ToCppStringLiteral(b3Method.MethodName)}) == 0)");
                        caseSb.AppendLine("            {");
                        caseSb.AppendLine($"                return {GetMethodHandleLiteral(b3Method.MethodSubjectId)};");
                        caseSb.AppendLine("            }");
                        caseSb.AppendLine();
                    }
                    caseSb.AppendLine("            break;");
                    caseBlocks.Add((caseSb.ToString(), b3Group.Count()));
                }

                const string B3ResolveName = "chaos_reflection_resolve_method_handle_b3";
                const string B3ResolveParams = "CHAOS_IL2CPP_INTPTR chaos_type_handle, const char* chaos_method_name";

                var resolveParts = ReflectionDispatchPartitioner.SplitCaseBlocks(
                    B3ResolveName, "CHAOS_IL2CPP_INTPTR", B3ResolveParams, caseBlocks);

                sb.Append(ReflectionDispatchPartitioner.BuildFlatChainShell(
                    B3ResolveName, "CHAOS_IL2CPP_INTPTR", B3ResolveParams,
                    prologue: "    if (chaos_method_name == nullptr) return 0;\n",
                    resolveParts,
                    argumentList: "chaos_type_handle, chaos_method_name",
                    epilogue: "    return 0;\n"));
                sb.AppendLine();

                // Each part is declared in the shared header (page 0's object-model
                // shell calls the non-_b3 variant, but the module-registration TU
                // and any other caller reference these) — see BuildSharedHeader.
                foreach (var part in resolveParts)
                {
                    AddDeferredPayloadSection($"{B3ResolveName}_{part.NameSuffix}", part.Text);
                    RegisterCrossSectionSymbol(
                        $"{B3ResolveName}_{part.NameSuffix}",
                        $"extern \"C\" CHAOS_IL2CPP_INTPTR {B3ResolveName}_{part.NameSuffix}({B3ResolveParams}) noexcept;",
                        needsExternalLinkage: true);
                }
            }

            // Parameter array builder: if-chain over per-method parameter names,
            // mirroring EmitMethodParameterNameCase in the object model TU.
            //
            // EmitMethodParameterNameCase constructs a ParameterInfo[] and sets each
            // element's header.type_info, which references the ParameterInfo
            // MethodTable symbol.  ParameterInfo is a BCL reflection type that is
            // never a subject/closure type, so nothing else adds its MethodTable to
            // the emission set — without the registration below every chunk whose
            // B3 table contains a parameterised method fails to compile with
            // C2065 'chaos_mt_..._ParameterInfo': undeclared identifier.
            //
            // At 10.98 MB / 15,982 branches this is the single largest function in
            // the chunk, and unlike the _b3 resolver it has no prologue or epilogue
            // — just the if-chain and a trailing `return 0;`. The parts therefore
            // carry the whole body and each returns 0 on a miss.
            RegisterExtraMethodTableSymbol(ParameterInfoSubjectId);

            {
                const string B3ParamsName = "chaos_reflection_get_parameters_b3";
                const string B3ParamsParams = "CHAOS_IL2CPP_INTPTR chaos_method_handle";

                var branches = new List<string>();
                var emittedParamHandles = new HashSet<string>(StringComparer.Ordinal);
                foreach (var b3Method in b3Entries.OrderBy(e => e.MethodSubjectId, StringComparer.Ordinal))
                {
                    var handleKey = GetMethodHandleLiteral(b3Method.MethodSubjectId);
                    if (!emittedParamHandles.Add(handleKey))
                    {
                        continue;
                    }
                    var branchSb = new StringBuilder();
                    // Column 0: this branch is emitted into a standalone part
                    // function, not into the switch body of the unsplit original.
                    EmitMethodParameterNameCase(branchSb, b3Method, "if");
                    branches.Add(branchSb.ToString());
                }

                var paramParts = ReflectionDispatchPartitioner.SplitFlatChain(
                    B3ParamsName, "CHAOS_IL2CPP_INTPTR", B3ParamsParams, branches);

                sb.Append(ReflectionDispatchPartitioner.BuildFlatChainShell(
                    B3ParamsName, "CHAOS_IL2CPP_INTPTR", B3ParamsParams,
                    prologue: string.Empty,
                    paramParts,
                    argumentList: "chaos_method_handle",
                    epilogue: "    return 0;\n"));
                sb.AppendLine();

                foreach (var part in paramParts)
                {
                    AddDeferredPayloadSection($"{B3ParamsName}_{part.NameSuffix}", part.Text);
                    RegisterCrossSectionSymbol(
                        $"{B3ParamsName}_{part.NameSuffix}",
                        $"extern \"C\" CHAOS_IL2CPP_INTPTR {B3ParamsName}_{part.NameSuffix}({B3ParamsParams}) noexcept;",
                        needsExternalLinkage: true);
                }
            }
        }

        return sb.ToString();
    }

    /// <summary>
    /// Subject id of the BCL reflection type whose MethodTable the object-model
    /// parameter builders reference.
    /// </summary>
    private const string ParameterInfoSubjectId =
        "System.Private.CoreLib/System.Reflection.ParameterInfo";

    /// <summary>
    /// Add a chaos_mt_* symbol to the extra-symbol set that the shared header
    /// declares as an extern "C" MethodTable.
    ///
    /// Used by reflection-object emission paths that reference a BCL MethodTable
    /// from generated code which is NOT a lowered method body — the normal
    /// symbol scan in EmitMethodBodies only walks method bodies, so these
    /// references would otherwise be declared nowhere (C2065).
    /// </summary>
    private void RegisterExtraMethodTableSymbol(string subjectId)
    {
        _extraMethodTableSymbols ??= new HashSet<string>(StringComparer.Ordinal);
        _extraMethodTableSymbols.Add(GetNativeMethodTableSymbol(subjectId));
    }

    /// <summary>C++ identifier for a per-type member slice (subject id sanitised).</summary>
    private static string ReflectionMemberSymbol(string kind, string typeSubjectId)
    {
        var sb = new StringBuilder("kRefl_");
        sb.Append(kind).Append('_');
        foreach (char c in typeSubjectId)
            sb.Append(char.IsLetterOrDigit(c) ? c : '_');
        return sb.ToString();
    }

    /// <summary>
    /// FNV-1a 24-bit hash of a type subject id — must match the runtime's
    /// <c>ChaosRegisterExternalType</c> key derivation in type_resolve.cpp.
    /// </summary>
    private static uint ReflectionMemberFnv24(string typeSubjectId)
    {
        uint h = 2166136261u;
        foreach (char c in typeSubjectId)
        {
            h ^= (byte)c;
            h *= 16777619u;
        }
        return h & 0xFFFFFFu;
    }

}
