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

        var model = new ScriptObject
        {
            ["indentation"] = ScribanTemplateRenderer.Indentation(1),
            ["assembly_name"] = EscapeCppStringLiteral(assemblyName),
            ["has_type_data"] = hasTypeData,
            ["type_count"] = _moduleTypeCount,
            ["custom_attribute_blob_code"] = caCode,
            ["module_descriptor_custom_attr_fields"] = caFieldCode,
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

}
