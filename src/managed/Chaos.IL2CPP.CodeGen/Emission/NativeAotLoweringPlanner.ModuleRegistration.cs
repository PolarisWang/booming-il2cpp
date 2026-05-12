using System.Linq;
using System.Text;
using Chaos.IL2CPP.Contracts;
using Scriban.Runtime;

namespace Chaos.IL2CPP.CodeGen;

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
    /// Generates flat .rodata arrays: type index, method index, token→slot map,
    /// dispatch table, and a HotpatchModuleV0 bundle with a static init caller.
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
        if (reachableMethods == null || reachableMethods.Count == 0)
        {
            // No methods — emit nullptr so the linker always resolves the symbol.
            var emptySb = new StringBuilder(256);
            emptySb.AppendLine("// ── Hotpatch dispatch table (empty — no reachable methods) ──");
            emptySb.AppendLine("// No hotpatch data for this module");
            emptySb.AppendLine("extern \"C\" const HotpatchModuleV0* chaos_il2cpp_aot_hotpatch_module");
            emptySb.AppendLine("    = nullptr;");
            return emptySb.ToString();
        }

        var sb = new StringBuilder(4096);
        sb.AppendLine("// ── Hotpatch name index + dispatch table ────────────────────");

        // Build token lookup from metadata registration.
        var tokenLookup = new MetadataTokenLookup(metadataRegistration.Registrations);

        // Collect (type_name, type_namespace, method_name, token, native_symbol, param_count, subject_id) tuples.
        var entries = new List<(string TypeName, string TypeNamespace, string MethodName, uint Token, string NativeSymbol, int ParamCount, string SubjectId)>();
        foreach (var method in reachableMethods)
        {
            string typeSubjectId;
            try
            {
                typeSubjectId = GetMethodDeclaringTypeSubjectId(method.SubjectId);
            }
            catch
            {
                continue; // skip methods without declaring type
            }

            var typeName = GetTypeDisplayName(typeSubjectId);
            var typeNamespace = GetTypeNamespace(typeSubjectId);
            var methodName = GetMethodName(method.SubjectId);
            uint token = tokenLookup.TryGetMethodToken(method.SubjectId);
            if (token == 0)
                continue; // skip methods without metadata tokens

            entries.Add((typeName, typeNamespace, methodName, token, method.NativeSymbol, method.ParameterCount, method.SubjectId));
        }

        if (entries.Count == 0)
            return string.Empty;

        // Group by (namespace, type_name) tuple and sort lexicographically.
        // This mirrors the C++ CompareTypeName which compares namespace first, then type_name.
        var grouped = entries
            .GroupBy(e => (e.TypeNamespace, e.TypeName))
            .OrderBy(g => g.Key.TypeNamespace, StringComparer.Ordinal)
            .ThenBy(g => g.Key.TypeName, StringComparer.Ordinal)
            .ToList();

        // ── Method entries (grouped by type, in type-sorted order) ──
        sb.AppendLine("// Method name index entries");
        sb.Append("static constexpr HotpatchMethodEntryV0 s_hotpatch_methods[")
          .Append(entries.Count).AppendLine("] = {");
        foreach (var group in grouped)
        {
            foreach (var entry in group)
            {
                sb.Append("    { ").Append(ToCppStringLiteral(entry.MethodName)).Append(", ")
                  .Append("0x").Append(entry.Token.ToString("X8")).Append("u, ")
                  .Append((ushort)entry.ParamCount).Append("u },");
                sb.Append("  // ").Append(entry.TypeName).AppendLine();
            }
        }
        sb.AppendLine("};");
        sb.AppendLine();

        // ── Type entries with first_method_index ──
        sb.AppendLine("// Type name index entries (namespace, short_name)");
        sb.Append("static constexpr HotpatchTypeEntryV0 s_hotpatch_types[")
          .Append(grouped.Count).AppendLine("] = {");
        uint methodIndex = 0;
        foreach (var group in grouped)
        {
            sb.Append("    { ").Append(ToCppStringLiteral(group.Key.TypeName)).Append(", ")
              .Append(ToCppStringLiteral(group.Key.TypeNamespace)).Append(", ")
              .Append(methodIndex).Append("u, ")
              .Append((ushort)group.Count()).Append("u },");
            sb.AppendLine();
            methodIndex += (uint)group.Count();
        }
        sb.AppendLine("};");
        sb.AppendLine();

        // ── Token→Slot mapping (sorted by token for bsearch) ──
        var tokenSlotList = entries
            .Select((e, idx) => (Token: e.Token, Slot: (uint)idx))
            .OrderBy(ts => ts.Token)
            .ToList();

        sb.AppendLine("// Token→Slot mapping (sorted by token for binary search)");
        sb.Append("static constexpr HotpatchSlotEntryV0 s_hotpatch_slots[")
          .Append(tokenSlotList.Count).AppendLine("] = {");
        foreach (var ts in tokenSlotList)
        {
            sb.Append("    { 0x").Append(ts.Token.ToString("X8")).Append("u, ")
              .Append(ts.Slot).Append("u },");
            sb.AppendLine();
        }
        sb.AppendLine("};");
        sb.AppendLine();

        // Determine kHotpatchKeepNative flag per-method.
        // External-routable methods have cross-assembly calls handled by
        // the external runtime dispatch table (kChaosExternalRuntimeFnTable).
        // For these methods, the interpreter path is unnecessary — the dispatcher
        // already knows how to route them natively. Setting kHotpatchKeepNative
        // prevents the interpreter from attempting to execute these methods
        // (which it cannot handle for complex IL), keeping them on the native path.
        //
        // Note: _externalRuntimeSubjects stores callee SubjectIds (e.g.
        // "System.Array::Copy"), not the entry method SubjectIds. We must scan
        // each reachable method's instructions to detect whether it calls any
        // external runtime targets.
        var methodsCallingExternal = new HashSet<string>(StringComparer.Ordinal);
        foreach (var method in reachableMethods)
        {
            foreach (var instruction in method.Instructions)
            {
                if (!string.IsNullOrEmpty(instruction.Callee) &&
                    _externalRuntimeSubjects.ContainsKey(instruction.Callee))
                {
                    methodsCallingExternal.Add(method.SubjectId);
                    break;
                }
            }
        }

        // ── Dispatch table with function pointers ──
        // direct_ptr = &MethodNativeSymbol (extern "C" function emitted elsewhere)
        // interrupt_ptr = &InterpreterEntryDirect (hotpatch dispatch)
        sb.AppendLine("// Dispatch table (function pointers)");
        sb.Append("static HotpatchEntryV0 s_hotpatch_entries[")
          .Append(entries.Count).AppendLine("] = {");
        foreach (var entry in entries)
        {
            string flags = methodsCallingExternal.Contains(entry.SubjectId) ? "kHotpatchKeepNative" : "0";
            sb.Append("    { reinterpret_cast<void*>(&").Append(entry.NativeSymbol).Append("), ")
              .Append("reinterpret_cast<void*>(&InterpreterEntryDirect), 0ull, ")
              .Append(flags).Append(" },");
            sb.Append("  // ").Append(entry.TypeName).Append("::").Append(entry.MethodName).AppendLine();
        }
        sb.AppendLine("};");
        sb.AppendLine();

        // ── HotpatchModuleV0 bundle ──
        sb.AppendLine("// Module hotpatch bundle");
        sb.AppendLine("static constexpr HotpatchModuleV0 s_hotpatch_module = {");
        sb.Append("    ").Append(ToCppStringLiteral(_assemblyName)).AppendLine(",");
        sb.AppendLine("    s_hotpatch_types,");
        sb.Append("    ").Append(grouped.Count).AppendLine("u,");
        sb.AppendLine("    s_hotpatch_methods,");
        sb.Append("    ").Append(entries.Count).AppendLine("u,");
        sb.AppendLine("    s_hotpatch_slots,");
        sb.Append("    ").Append(tokenSlotList.Count).AppendLine("u,");
        sb.AppendLine("    s_hotpatch_entries,");
        sb.Append("    ").Append(entries.Count).AppendLine("u,");
        sb.AppendLine("};");
        sb.AppendLine();

        // ── Expose module to BootstrapRuntime via extern "C" ──
        sb.AppendLine("// Expose hotpatch module to BootstrapRuntime");
        sb.AppendLine("extern \"C\" const HotpatchModuleV0* chaos_il2cpp_aot_hotpatch_module");
        sb.AppendLine("    = &s_hotpatch_module;");

        // ── Reverse P/Invoke (UnmanagedCallersOnly) wrapper registration ──
        if (_reversePInvokeEntries.Count > 0)
        {
            sb.AppendLine();
            sb.AppendLine("// ── Reverse P/Invoke wrapper registration ──────────────────");
            sb.Append("static constexpr void* s_reverse_pinvoke_wrappers[")
              .Append(_reversePInvokeEntries.Count).AppendLine("] =");
            sb.AppendLine("{");
            for (int i = 0; i < _reversePInvokeEntries.Count; i++)
            {
                sb.Append("    reinterpret_cast<void*>(&")
                  .Append(_reversePInvokeEntries[i].NativeSymbol).AppendLine("),");
            }
            sb.AppendLine("};");
            sb.AppendLine();
            sb.AppendLine("static const CHAOS_IL2CPP_UINT32 s_reverse_pinvoke_registered = []()");
            sb.AppendLine("{");
            sb.AppendLine("    ::chaos::il2cpp::runtime_core::RegisterReversePInvokeWrappers(");
            sb.Append("        s_reverse_pinvoke_wrappers, ")
              .Append(_reversePInvokeEntries.Count).AppendLine("u);");
            sb.AppendLine("    return 1u;");
            sb.AppendLine("}();");
        }

        return sb.ToString();
    }

    // ── CustomAttribute blob data emission ──────────────────────────
    // Builds the binary blob, offset array, and materializer switch for
    // per-module CustomAttribute query support.

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

        // Filter to Type-kind only (method deferred)
        var typeMaterializations = materializations
            .Where(m => m.TargetKind == CustomAttributeTargetKind.Type)
            .ToList();
        if (typeMaterializations.Count == 0)
            return string.Empty;

        // Build subjectId → type index mapping
        var subjectIdToTypeIndex = new Dictionary<string, int>(_moduleTypeCount, StringComparer.Ordinal);
        for (int i = 0; i < _moduleTypeCount; i++)
            subjectIdToTypeIndex[_moduleTypeSubjectIds[i]] = i;

        // Group materializations by type index
        var typeIndexToPlans = new Dictionary<int, List<CustomAttributeMaterializationPlan>>();
        foreach (var plan in typeMaterializations)
        {
            if (subjectIdToTypeIndex.TryGetValue(plan.TargetSubjectId, out int ti))
            {
                if (!typeIndexToPlans.TryGetValue(ti, out var list))
                {
                    list = new List<CustomAttributeMaterializationPlan>();
                    typeIndexToPlans[ti] = list;
                }
                list.Add(plan);
            }
        }

        if (typeIndexToPlans.Count == 0)
            return string.Empty;

        // Collect unique attribute types for the materializer switch
        var uniqueAttrTypes = new List<(string SubjectId, uint Token, List<(string FieldSubjectId, CustomAttributeLiteralValue Value)> Fields)>();
        var attrTypeKeySet = new HashSet<string>(StringComparer.Ordinal);
        foreach (var plan in typeMaterializations)
        {
            if (attrTypeKeySet.Add(plan.AttributeTypeSubjectId))
            {
                // Deduplicate fields (same field can appear multiple times across entities)
                var fieldSet = new Dictionary<string, CustomAttributeLiteralValue>(StringComparer.Ordinal);
                foreach (var a in plan.Assignments)
                    fieldSet[a.FieldSubjectId] = a.Value;
                uniqueAttrTypes.Add((plan.AttributeTypeSubjectId,
                    GetTypeTokenForSubjectId(plan.AttributeTypeSubjectId),
                    fieldSet.Select(kv => (kv.Key, kv.Value)).ToList()));
            }
        }

        // Build per-entity blob segments
        var blobStream = new MemoryStream();
        var offsets = new uint[_moduleTypeCount + 1];
        uint currentOffset = 0;

        for (int ti = 0; ti < _moduleTypeCount; ti++)
        {
            offsets[ti] = currentOffset;

            if (typeIndexToPlans.TryGetValue(ti, out var plans))
            {
                // Emit attr_count + per-attribute data
                uint attrCount = (uint)plans.Count;
                var attrCountBytes = BitConverter.GetBytes((ushort)attrCount);
                blobStream.Write(attrCountBytes, 0, 2);
                currentOffset += 2;

                foreach (var plan in plans)
                {
                    // attr_type_token
                    uint token = GetTypeTokenForSubjectId(plan.AttributeTypeSubjectId);
                    var tokenBytes = BitConverter.GetBytes(token);
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
            // else: no attributes for this type → zero-byte segment
        }
        offsets[_moduleTypeCount] = currentOffset;

        var sb = new StringBuilder(4096);
        sb.AppendLine("// ── CustomAttribute blob ─────────────────────────────────────────");

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

        // Emit offset array
        sb.Append("static constexpr CHAOS_IL2CPP_UINT32 s_custom_attribute_offset[")
            .Append(_moduleTypeCount + 1).AppendLine("] =");
        sb.AppendLine("{");
        for (int i = 0; i <= _moduleTypeCount; i++)
        {
            sb.Append("    ").Append(offsets[i]).Append("u,");
            if (i <= _moduleTypeCount) sb.AppendLine();
        }
        sb.AppendLine("};");
        sb.AppendLine();

        // Emit materializer switch
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
            sb.Append("            attr->header.type_info = &")
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

    private string EmitFieldDecodeStmt(string fieldName, CustomAttributeLiteralValue value, string attrSubjectId)
    {
        var kind = value.Kind;
        return kind switch
        {
            CustomAttributeLiteralKind.Null =>
                $"attr->{fieldName} = 0;",

            CustomAttributeLiteralKind.Boolean =>
                $"{{ bool __v; std::memcpy(&__v, p, 1); p += 1; attr->{fieldName} = __v; }}",

            CustomAttributeLiteralKind.Byte =>
                $"{{ uint8_t __v; std::memcpy(&__v, p, 1); p += 1; attr->{fieldName} = __v; }}",

            CustomAttributeLiteralKind.SByte =>
                $"{{ int8_t __v; std::memcpy(&__v, p, 1); p += 1; attr->{fieldName} = __v; }}",

            CustomAttributeLiteralKind.Int16 =>
                $"{{ int16_t __v; std::memcpy(&__v, p, 2); p += 2; attr->{fieldName} = __v; }}",

            CustomAttributeLiteralKind.Int32 =>
                $"{{ CHAOS_IL2CPP_INT32 __v; std::memcpy(&__v, p, 4); p += 4; attr->{fieldName} = __v; }}",

            CustomAttributeLiteralKind.Int64 =>
                $"{{ CHAOS_IL2CPP_INT64 __v; std::memcpy(&__v, p, 8); p += 8; attr->{fieldName} = __v; }}",

            CustomAttributeLiteralKind.UInt16 =>
                $"{{ CHAOS_IL2CPP_UINT16 __v; std::memcpy(&__v, p, 2); p += 2; attr->{fieldName} = __v; }}",

            CustomAttributeLiteralKind.UInt32 =>
                $"{{ CHAOS_IL2CPP_UINT32 __v; std::memcpy(&__v, p, 4); p += 4; attr->{fieldName} = __v; }}",

            CustomAttributeLiteralKind.UInt64 =>
                $"{{ CHAOS_IL2CPP_UINT64 __v; std::memcpy(&__v, p, 8); p += 8; attr->{fieldName} = __v; }}",

            CustomAttributeLiteralKind.Single =>
                $"{{ float __v; std::memcpy(&__v, p, 4); p += 4; attr->{fieldName} = __v; }}",

            CustomAttributeLiteralKind.Double =>
                $"{{ double __v; std::memcpy(&__v, p, 8); p += 8; attr->{fieldName} = __v; }}",

            CustomAttributeLiteralKind.Char =>
                $"{{ CHAOS_IL2CPP_UINT16 __v; std::memcpy(&__v, p, 2); p += 2; attr->{fieldName} = __v; }}",

            CustomAttributeLiteralKind.String =>
                $"{{ CHAOS_IL2CPP_UINT16 __len; std::memcpy(&__len, p, 2); p += 2; auto* __rt = chaos::il2cpp::runtime_core::GetCurrentRuntimeState(); auto* __th = chaos::il2cpp::runtime_core::GetCurrentThreadState(); auto* __abi = chaos::il2cpp::runtime_core::GetRuntimeAbiV0(); attr->{fieldName} = (__abi != nullptr && __abi->string_new_utf8 != nullptr) ? reinterpret_cast<CHAOS_IL2CPP_INTPTR>(__abi->string_new_utf8(__rt, __th, reinterpret_cast<const char*>(p), __len)) : 0; p += __len; }}",

            CustomAttributeLiteralKind.Type when value.Value is string typeSubjectId =>
                $"{{ p += 4; attr->{fieldName} = reinterpret_cast<CHAOS_IL2CPP_INTPTR>({GetNativeTypeInfoSymbol(typeSubjectId)}); }}",

            CustomAttributeLiteralKind.Type =>
                $"{{ p += 4; attr->{fieldName} = 0; }}",

            CustomAttributeLiteralKind.Enum when value.Value != null =>
                $"attr->{fieldName} = {FormatEnumLiteral(value.Value)};",

            CustomAttributeLiteralKind.Enum =>
                $"attr->{fieldName} = 0;",

            _ => $"attr->{fieldName} = 0;",
        };
    }

    private static string FormatEnumLiteral(object value)
    {
        return value switch
        {
            int i => $"static_cast<CHAOS_IL2CPP_INTPTR>({i})",
            long l => $"static_cast<CHAOS_IL2CPP_INTPTR>({l})",
            short s => $"static_cast<CHAOS_IL2CPP_INTPTR>({s})",
            byte b => $"static_cast<CHAOS_IL2CPP_INTPTR>({b})",
            sbyte sb => $"static_cast<CHAOS_IL2CPP_INTPTR>({sb})",
            ushort us => $"static_cast<CHAOS_IL2CPP_INTPTR>({us})",
            uint ui => $"static_cast<CHAOS_IL2CPP_INTPTR>({ui})",
            ulong ul => $"static_cast<CHAOS_IL2CPP_INTPTR>({ul})",
            _ => $"static_cast<CHAOS_IL2CPP_INTPTR>({Convert.ToInt32(value)})",
        };
    }

    private void EmitCustomAttributeModuleDescriptorFields(StringBuilder sb)
    {
        if (_hasCustomAttributeBlob)
        {
            sb.AppendLine("    /* .custom_attribute_blob       = */ s_custom_attribute_blob,");
            sb.AppendLine("    /* .custom_attribute_offset     = */ s_custom_attribute_offset,");
            sb.Append("    /* .custom_attribute_entity_count = */ ").Append(_moduleTypeCount).AppendLine("u,");
            sb.AppendLine("    /* .custom_attribute_materializer = */ &ModuleCustomAttributeMaterializer,");
        }
        else
        {
            sb.AppendLine("    /* .custom_attribute_blob       = */ nullptr,  // Tier 1 — deferred");
            sb.AppendLine("    /* .custom_attribute_offset     = */ nullptr,");
            sb.AppendLine("    /* .custom_attribute_entity_count = */ 0u,");
            sb.AppendLine("    /* .custom_attribute_materializer = */ nullptr,");
        }
    }

    internal static string BuildAbiManifest(IReadOnlyList<AotCoreIrMethodArtifact> reachableMethods)
    {
        if (reachableMethods.Count == 0)
        {
            return string.Empty;
        }

        var sb = new StringBuilder(4096);
        sb.AppendLine("// ── ABI manifest ──────────────────────────────────────────────");
        sb.AppendLine("// Single contiguous struct: header + entries + params in same object");
        sb.AppendLine("// so CHAOS_ABI_MANIFEST_ENTRIES/CHAOS_ABI_MANIFEST_PARAMETERS find them by offset.");
        sb.AppendLine("// NOTE: reinterpret_cast is needed because MSVC rejects &anon_struct.header");

        int totalParams = reachableMethods.Sum(m => m.ParameterAbis.Count);
        uint checksum = ComputeAbiManifestChecksum(reachableMethods);

        // Prefix-sum array (separate constexpr so we can reference it in header initializer)
        sb.Append("// Param offset prefix-sum: [i] = cumulative parameter count before method i").AppendLine();
        sb.Append("static constexpr CHAOS_IL2CPP_UINT32 s_abi_manifest_prefix_sum[")
            .Append(reachableMethods.Count + 1).AppendLine("] = {");
        {
            uint runningTotal = 0;
            for (int i = 0; i < reachableMethods.Count; i++)
            {
                sb.Append("    ").Append(runningTotal).Append("u,");
                sb.AppendLine();
                runningTotal += (uint)reachableMethods[i].ParameterAbis.Count;
            }
            sb.Append("    ").Append(runningTotal).AppendLine("u");
        }
        sb.AppendLine("};");
        sb.AppendLine();

        sb.Append("static constexpr struct {").AppendLine();
        sb.Append("    ::ChaosAbiManifestV0 header;").AppendLine();
        sb.Append("    ::ChaosAbiMethodEntryV0 entries[").Append(reachableMethods.Count).AppendLine("];");
        sb.Append("    CHAOS_IL2CPP_UINT8 params[").Append(totalParams > 0 ? totalParams : 1).AppendLine("];");
        sb.Append("} s_abi_manifest_storage = {").AppendLine();
        sb.AppendLine("    {");
        sb.AppendLine("        CHAOS_ABI_MANIFEST_VERSION,");
        sb.Append("        ").Append(reachableMethods.Count).AppendLine("u,");
        sb.Append("        ").Append(totalParams).AppendLine("u,");
        sb.Append("        ").Append(checksum).AppendLine("u,  // FNV-1a over entries+params");
        sb.AppendLine("        s_abi_manifest_prefix_sum  // O(1) prefix-sum");
        sb.AppendLine("    },");
        sb.AppendLine("    {");
        for (int i = 0; i < reachableMethods.Count; i++)
        {
            var method = reachableMethods[i];
            sb.Append("        { ").Append((int)method.ReturnAbi.CarrierKindCode).Append("u, ")
                .Append(method.ParameterAbis.Count).Append("u },");
            sb.Append("  // ").Append(method.NativeSymbol);
            sb.AppendLine();
        }
        sb.AppendLine("    },");
        sb.AppendLine("    {");
        if (totalParams > 0)
        {
            int paramIndex = 0;
            foreach (var method in reachableMethods)
            {
                foreach (var abi in method.ParameterAbis)
                {
                    sb.Append("        ").Append((int)abi.CarrierKindCode).Append("u,");
                    paramIndex++;
                    if (paramIndex < totalParams)
                        sb.AppendLine();
                }
            }
            sb.AppendLine();
        }
        sb.AppendLine("    },");
        sb.AppendLine("};");
        sb.Append("static const ::ChaosAbiManifestV0* const s_abi_manifest =");
        sb.Append(" reinterpret_cast<const ::ChaosAbiManifestV0*>(&s_abi_manifest_storage);");
        sb.AppendLine();

        return sb.ToString();
    }

    private static string EscapeCppStringLiteral(string value)
    {
        return value
            .Replace("\\", "\\\\")
            .Replace("\"", "\\\"")
            .Replace("\n", "\\n")
            .Replace("\r", "\\r")
            .Replace("\t", "\\t");
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
    internal string BuildExternalRuntimeDispatchTable(
        Dictionary<string, string>? helperSymbolBySubjectId = null)
    {
        if (_externalRuntimeSubjects.Count == 0)
        {
            // Always emit the count symbol so the runtime can safely reference it.
            var emptySb = new StringBuilder(256);
            emptySb.AppendLine("// ── External Runtime Dispatch Table (empty) ─────────────────");
            emptySb.AppendLine("extern \"C\" const char* kChaosExternalRuntimeSubjects[1] = { nullptr };");
            emptySb.AppendLine("extern \"C\" void* kChaosExternalRuntimeFnTable[1] = { nullptr };");
            emptySb.AppendLine("extern \"C\" int32_t kChaosExternalRuntimeCount = 0;");
            return emptySb.ToString();
        }

        // Sort by subjectId for deterministic output
        var sorted = _externalRuntimeSubjects
            .OrderBy(kv => kv.Key, StringComparer.Ordinal)
            .ToArray();

        // Build a lookup: subjectId → chaos_external_runtime_* symbol (for shaped entries)
        // If caller passed helperSymbolBySubjectId, use it directly; otherwise build from optional IReadOnlyList<>.

        var sb = new StringBuilder(4096);
        sb.AppendLine("// ── External Runtime Dispatch Table ──────────────────────────");
        sb.AppendLine("// Startup-time-resolved function pointers for cross-assembly calls.");
        sb.AppendLine();

        // SubjectId strings (compiler can merge duplicates across TUs)
        sb.Append("extern \"C\" const char* kChaosExternalRuntimeSubjects[")
            .Append(sorted.Length).AppendLine("] =");
        sb.AppendLine("{");
        foreach (var kv in sorted)
        {
            sb.Append("    ").Append(ToCppStringLiteral(kv.Key)).AppendLine(",");
        }
        sb.AppendLine("};");
        sb.AppendLine();

        // Function pointer table — pre-filled for shaped entries, nullptr for others
        sb.Append("extern \"C\" void* kChaosExternalRuntimeFnTable[")
            .Append(sorted.Length).AppendLine("] =");
        sb.AppendLine("{");
        for (int i = 0; i < sorted.Length; i++)
        {
            string subjectId = sorted[i].Key;
            if (helperSymbolBySubjectId.TryGetValue(subjectId, out var helperSymbol))
            {
                // Shaped entry: pre-fill with the chaos_external_runtime_* function pointer
                sb.Append("    reinterpret_cast<void*>(&").Append(helperSymbol).AppendLine("),");
            }
            else
            {
                // Non-shaped entry: resolved at startup by ChaosResolveExternalRuntimeFnTable()
                sb.AppendLine("    nullptr,");
            }
        }
        sb.AppendLine("};");
        sb.AppendLine();

        // Count
        sb.Append("extern \"C\" int32_t kChaosExternalRuntimeCount = ")
            .Append(sorted.Length).AppendLine(";");
        sb.AppendLine();

        return sb.ToString();
    }

    private static uint ComputeAbiManifestChecksum(IReadOnlyList<AotCoreIrMethodArtifact> reachableMethods)
    {
        // FNV-1a hash over the entries+params payload layout that the native
        // ChaosAbiManifestValidate hashes (everything starting after the
        // checksum field): entries[] then parameters[].
        const uint fnvOffsetBasis = 2166136261u;
        const uint fnvPrime = 16777619u;

        uint hash = fnvOffsetBasis;

        // Hash entries: each ChaosAbiMethodEntryV0 = 2 bytes (return_carrier, parameter_count)
        foreach (var method in reachableMethods)
        {
            hash ^= (uint)method.ReturnAbi.CarrierKindCode;
            hash *= fnvPrime;
            hash ^= (uint)method.ParameterAbis.Count;
            hash *= fnvPrime;
        }

        // Hash parameters array
        foreach (var method in reachableMethods)
        {
            foreach (var abi in method.ParameterAbis)
            {
                hash ^= (uint)abi.CarrierKindCode;
                hash *= fnvPrime;
            }
        }

        return hash;
    }
}
