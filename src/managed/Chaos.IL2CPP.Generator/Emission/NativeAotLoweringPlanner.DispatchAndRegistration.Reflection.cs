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



    // ── Step 3: ReflectionQueryImageDescriptor ──────────────────────────────────
    // Emits ReflectionQueryMethodDescriptor[] and ReflectionQueryTypeDescriptor[]
    // arrays, and a ReflectionQueryImageDescriptor that module.image points to.
    // This enables ResolveSubjectId to find call_target via reflection query model.
    private string EmitReflectionQueryImage(
        IReadOnlyList<AotCoreIrMethodArtifact> methods,
        MetadataRegistrationArtifact metadataRegistration)
    {
        if (methods.Count == 0 || _moduleTypeSubjectIds.Count == 0) return string.Empty;

        var tokenLookup = new MetadataTokenLookup(metadataRegistration.Registrations);

        // Build type -> methods map grouped by declaring type.
        // Preserve the AotCoreIrMethodArtifact to extract ReturnType/ParameterCount.
        var typeMethodMap = new Dictionary<string, List<(AotCoreIrMethodArtifact Artifact, string TypeNs, string TypeName)>>(StringComparer.Ordinal);
        foreach (var method in methods)
        {
            string declaringType;
            try { declaringType = GetMethodDeclaringTypeSubjectId(method.SubjectId); }
            catch { continue; }

            if (!typeMethodMap.TryGetValue(declaringType, out var list))
            {
                list = new List<(AotCoreIrMethodArtifact, string, string)>();
                typeMethodMap[declaringType] = list;
            }

            list.Add((method, GetTypeNamespace(declaringType), GetTypeDisplayName(declaringType)));
        }

        if (typeMethodMap.Count == 0) return string.Empty;

        var typeIndexToMethods = new List<(string TypeSubjectId, int Count, string SafeName, string Ns, string Name)>();
        // Pre-collect field data for all types in the type method map by assembly.
        // This ensures field descriptor arrays can be emitted in the reflection query image.
        var typeFieldMap = new Dictionary<string, List<(string SubjectId, string Name, string Type, long Value, uint Token, uint Flags)>>(StringComparer.Ordinal);
        var typeMethodAttrMap = new Dictionary<string, Dictionary<uint, uint>>(StringComparer.Ordinal); // typeSubjectId → {methodToken → flags}
        var typeEventMap = new Dictionary<string, List<(string SubjectId, string Name, string Type)>>(StringComparer.Ordinal);
        var assemblyTypeQueue = new Dictionary<string, HashSet<string>>(StringComparer.Ordinal);
        foreach (var typeSubjectId in typeMethodMap.Keys)
        {
            var slashIdx = typeSubjectId.IndexOf('/');
            var assemblyName = slashIdx >= 0 ? typeSubjectId.Substring(0, slashIdx) : _assemblyName;
            if (!assemblyTypeQueue.TryGetValue(assemblyName, out var typeSet))
            {
                typeSet = new HashSet<string>(StringComparer.Ordinal);
                assemblyTypeQueue[assemblyName] = typeSet;
            }
            typeSet.Add(typeSubjectId);
        }

        foreach (var (assemblyName, typeIds) in assemblyTypeQueue)
        {
            if (!_closureAssemblyPathByName.TryGetValue(assemblyName, out var assemblyPath))
                continue;

            try
            {
                using var stream = new FileStream(assemblyPath, FileMode.Open, FileAccess.Read, FileShare.Read);
                using var peReader = new PEReader(stream);
                if (!peReader.HasMetadata) continue;
                var metadataReader = peReader.GetMetadataReader();

                foreach (var typeHandle in metadataReader.TypeDefinitions)
                {
                    var typeDef = metadataReader.GetTypeDefinition(typeHandle);
                    var typeNs = metadataReader.GetString(typeDef.Namespace);
                    var typeName = metadataReader.GetString(typeDef.Name);
                    var nsTypeName = string.IsNullOrEmpty(typeNs) ? typeName : typeNs + "." + typeName;
                    var candidateId = assemblyName + "/" + nsTypeName;

                    if (!typeIds.Contains(candidateId))
                        continue;

                    var fields = new List<(string SubjectId, string Name, string Type, long Value, uint Token, uint Flags)>();
                    foreach (var fieldHandle in typeDef.GetFields())
                    {
                        var fieldDef = metadataReader.GetFieldDefinition(fieldHandle);
                        var fieldName = metadataReader.GetString(fieldDef.Name);
                        uint fieldToken = (uint)MetadataTokens.GetToken(fieldHandle);

                        long fieldValue = 0;
                        var constHandle = fieldDef.GetDefaultValue();
                        if (!constHandle.IsNil)
                        {
                            try
                            {
                                var constant = metadataReader.GetConstant(constHandle);
                                var blobReader = metadataReader.GetBlobReader(constant.Value);
                                switch ((PrimitiveTypeCode)constant.TypeCode)
                                {
                                    case PrimitiveTypeCode.Boolean: fieldValue = blobReader.ReadBoolean() ? 1L : 0L; break;
                                    case PrimitiveTypeCode.Byte: fieldValue = blobReader.ReadByte(); break;
                                    case PrimitiveTypeCode.SByte: fieldValue = blobReader.ReadSByte(); break;
                                    case PrimitiveTypeCode.Int16: fieldValue = blobReader.ReadInt16(); break;
                                    case PrimitiveTypeCode.UInt16: fieldValue = blobReader.ReadUInt16(); break;
                                    case PrimitiveTypeCode.Char: fieldValue = blobReader.ReadChar(); break;
                                    case PrimitiveTypeCode.Int32: fieldValue = blobReader.ReadInt32(); break;
                                    case PrimitiveTypeCode.UInt32: fieldValue = blobReader.ReadUInt32(); break;
                                    case PrimitiveTypeCode.Int64: fieldValue = blobReader.ReadInt64(); break;
                                    case PrimitiveTypeCode.UInt64: fieldValue = (long)blobReader.ReadUInt64(); break;
                                }
                            }
                            catch { }
                        }

                        // Compute field flags from PE FieldAttributes
                        var fa = fieldDef.Attributes;
                        uint fieldFlags = 0;
                        if ((fa & System.Reflection.FieldAttributes.FieldAccessMask) == System.Reflection.FieldAttributes.Public)
                            fieldFlags |= 1u << 0; // kFieldFlagIsPublic
                        if (fa.HasFlag(System.Reflection.FieldAttributes.Static))
                            fieldFlags |= 1u << 1; // kFieldFlagIsStatic
                        if (fa.HasFlag(System.Reflection.FieldAttributes.InitOnly))
                            fieldFlags |= 1u << 2; // kFieldFlagIsInitOnly
                        if (fa.HasFlag(System.Reflection.FieldAttributes.Literal))
                            fieldFlags |= 1u << 3; // kFieldFlagIsLiteral

                        var fieldSubjectId = candidateId + "::" + fieldName;
                        fields.Add((fieldSubjectId, fieldName, "System.Int32", fieldValue, fieldToken, fieldFlags));
                    }

                    typeFieldMap[candidateId] = fields;

                    // Collect method attributes for flag emission
                    var methodTokenFlags = new Dictionary<uint, uint>();
                    foreach (var methodHandle in typeDef.GetMethods())
                    {
                        var methodDef = metadataReader.GetMethodDefinition(methodHandle);
                        uint mToken = (uint)MetadataTokens.GetToken(methodHandle);
                        var ma = methodDef.Attributes;
                        uint methodFlags = 0;
                        if ((ma & System.Reflection.MethodAttributes.MemberAccessMask) == System.Reflection.MethodAttributes.Public)
                            methodFlags |= 1u << 0; // kMethodFlagIsPublic
                        if (ma.HasFlag(System.Reflection.MethodAttributes.Static))
                            methodFlags |= 1u << 1; // kMethodFlagIsStatic
                        if (ma.HasFlag(System.Reflection.MethodAttributes.Virtual))
                            methodFlags |= 1u << 2; // kMethodFlagIsVirtual
                        methodTokenFlags[mToken] = methodFlags;
                    }
                    typeMethodAttrMap[candidateId] = methodTokenFlags;

                    // Collect event data for this type
                    var events = new List<(string SubjectId, string Name, string Type)>();
                    foreach (var eventHandle in typeDef.GetEvents())
                    {
                        var eventDef = metadataReader.GetEventDefinition(eventHandle);
                        var eventName = metadataReader.GetString(eventDef.Name);
                        var eventSubjectId = candidateId + "::" + eventName;

                        // Resolve event handler type from the event's Type property
                        string eventType = "System.EventHandler";
                        try
                        {
                            var eventTypeHandle = eventDef.Type;
                            if (!eventTypeHandle.IsNil &&
                                TryResolveTypeIdentity(metadataReader, assemblyName, eventTypeHandle, out var typeIdentity))
                            {
                                eventType = typeIdentity.DisplayName;
                            }
                        }
                        catch { }

                        events.Add((eventSubjectId, eventName, eventType));
                    }
                    typeEventMap[candidateId] = events;
                }
            }
            catch { }
        }

        var typeGroups = new List<object>();
        foreach (var kvp in typeMethodMap)
        {
            var typeSubjectId = kvp.Key;
            var methodsInType = kvp.Value;
            var safeName = SanitizeCppIdentifier(typeSubjectId.Replace('/', '_').Replace(':', '_'));
            typeIndexToMethods.Add((typeSubjectId, methodsInType.Count, safeName, methodsInType[0].TypeNs, methodsInType[0].TypeName));

            uint typeToken = tokenLookup.TryGetTypeToken(typeSubjectId);

            var typeMethodAttrs = typeMethodAttrMap.TryGetValue(typeSubjectId, out var mta)
                ? mta : null;

            var methodEntries = methodsInType.Select(m =>
            {
                uint mToken = tokenLookup.TryGetMethodToken(m.Artifact.SubjectId);
                string returnType = m.Artifact.ReturnType ?? "System.Void";
                string methodName = GetMethodName(m.Artifact.SubjectId);
                uint methodFlags = (mToken > 0 && typeMethodAttrs != null && typeMethodAttrs.TryGetValue(mToken, out var mf)) ? mf : 0u;
                return new
                {
                    metadata_token_hex = mToken > 0 ? "0x" + mToken.ToString("X8") : "0",
                    subject_id_literal = EscapeCppStringLiteral(m.Artifact.SubjectId),
                    method_name_literal = EscapeCppStringLiteral(methodName),
                    return_type_literal = EscapeCppStringLiteral(returnType),
                    parameter_count = m.Artifact.ParameterCount,
                    flags = methodFlags,
                };
            }).ToArray();

            var fieldEntries = typeFieldMap.TryGetValue(typeSubjectId, out var tFields)
                ? tFields.Select(f => new
                {
                    metadata_token_hex = f.Token > 0 ? "0x" + f.Token.ToString("X8") : "0",
                    subject_id_literal = EscapeCppStringLiteral(f.SubjectId),
                    name_literal = EscapeCppStringLiteral(f.Name),
                    type_literal = EscapeCppStringLiteral(f.Type),
                    constant_value = f.Value,
                    flags = f.Flags,
                }).ToArray()
                : System.Array.Empty<object>();

            var eventEntries = typeEventMap.TryGetValue(typeSubjectId, out var tEvents)
                ? tEvents.Select(e => new
                {
                    subject_id_literal = EscapeCppStringLiteral(e.SubjectId),
                    name_literal = EscapeCppStringLiteral(e.Name),
                    type_literal = EscapeCppStringLiteral(e.Type),
                }).ToArray()
                : System.Array.Empty<object>();

            typeGroups.Add(new
            {
                metadata_token_hex = typeToken > 0 ? "0x" + typeToken.ToString("X8") : "0",
                safe_name = safeName,
                method_count = methodsInType.Count,
                subject_id_literal = EscapeCppStringLiteral(typeSubjectId),
                namespace_literal = EscapeCppStringLiteral(methodsInType[0].TypeNs),
                name_literal = EscapeCppStringLiteral(methodsInType[0].TypeName),
                methods = methodEntries,
                fields = fieldEntries,
                events = eventEntries,
                event_count = eventEntries.Length,
            });
        }

        var typeGroupIndices = Enumerable.Range(0, typeIndexToMethods.Count).ToArray();

        var model = new ScriptObject
        {
            ["type_groups"] = typeGroups,
            ["type_group_count"] = typeIndexToMethods.Count,
            ["type_group_indices"] = typeGroupIndices,
            ["assembly_name_literal"] = EscapeCppStringLiteral(_assemblyName),
        };

        return ScribanTemplateRenderer.RenderTemplate(
            NativeAotTemplateCatalog.GetReflectionQueryImageTemplate(), model);
    }





    /// <summary>
    /// Generate the C++ header for pre-computed enum metadata tables.
    /// Collects enum type data from ALL resolved assemblies (not just the entry
    /// assembly) so that SPC enum types (DayOfWeek, BindingFlags, etc.) are
    /// included alongside the entry assembly's own enum types.
    /// Returns empty string if no enum types or field data are available.
    /// </summary>
    private string GenerateEnumMetadataHeader()
    {
        // Start with entry assembly data (already collected in _moduleTypeFlags/SubjectIds).
        // Then augment with enum type data from all other resolved assemblies.
        // The seenSubjectIds set prevents duplicates when an assembly's types
        // overlap (e.g., when the entry assembly is also in resolved paths).
        var mergedFlags = new List<uint>(_moduleTypeFlags);
        var mergedSubjectIds = new List<string>(_moduleTypeSubjectIds);
        var seenSubjectIds = new HashSet<string>(_moduleTypeSubjectIds, StringComparer.Ordinal);
        // Collect field entries from PE metadata for all enum types (used when
        // _reflectionMemberSupport.FieldEntries is empty for stub-based families).
        var enumFieldEntries = new List<ReflectionMemberFieldEntry>();

        foreach (var assemblyPath in _cachedClosureAssemblyPaths)
        {
            try
            {
                CollectEnumTypesAndFieldsFromAssembly(assemblyPath, seenSubjectIds,
                    mergedFlags, mergedSubjectIds, enumFieldEntries);
            }
            catch
            {
                // Skip assemblies that can't be read (e.g., native images, missing files)
                continue;
            }
        }

        if (mergedFlags.Count == 0)
            return string.Empty;

        // Prefer reflection member field entries when available (they include data
        // from all closure assemblies); fall back to PE-metadata field entries.
        var fieldEntries = _reflectionMemberSupport.FieldEntries.Count > 0
            ? _reflectionMemberSupport.FieldEntries
            : enumFieldEntries;

        if (fieldEntries.Count == 0)
            return string.Empty;

        var header = EnumMetadataExtractor.GenerateHeader(
            mergedFlags,
            mergedSubjectIds,
            fieldEntries);

        // Post-process: if the header generator places compute_enum_hash24
        // inside the chaos::il2cpp::codegen namespace (as newer versions of
        // EnumMetadataExtractor do), but the registration code that calls it
        // is generated OUTSIDE the namespace, inject a using-declaration so
        // the unqualified call compiles.
        const string namespaceClose = "}}}  // namespace chaos::il2cpp::codegen";
        int nsCloseIdx = header.IndexOf(namespaceClose, StringComparison.Ordinal);
        if (nsCloseIdx >= 0)
        {
            int callIdx = header.IndexOf("compute_enum_hash24(", nsCloseIdx, StringComparison.Ordinal);
            if (callIdx >= 0)
            {
                // The hash function is called outside its defining namespace.
                // Insert a using-declaration right after the namespace close.
                int insertPos = nsCloseIdx + namespaceClose.Length;
                // Skip past any trailing whitespace/newline
                while (insertPos < header.Length &&
                       (header[insertPos] == '\r' || header[insertPos] == '\n'))
                    insertPos++;
                header = header[..insertPos] + "\n" +
                    "using chaos::il2cpp::codegen::compute_enum_hash24;" +
                    header[insertPos..];
            }
        }

        return header;
    }





    /// <summary>
    /// Read assembly PE metadata and collect enum type flags, subjectIds,
    /// AND field name/value entries for each enum type.
    /// Populates the provided lists, skipping types already in seenSubjectIds.
    /// </summary>
    private static void CollectEnumTypesAndFieldsFromAssembly(
        string assemblyPath,
        HashSet<string> seenSubjectIds,
        List<uint> mergedFlags,
        List<string> mergedSubjectIds,
        List<ReflectionMemberFieldEntry> enumFieldEntries)
    {
        if (string.IsNullOrEmpty(assemblyPath) || !File.Exists(assemblyPath))
            return;

        using var stream = File.OpenRead(assemblyPath);
        using var peReader = new System.Reflection.PortableExecutable.PEReader(stream);
        if (!peReader.HasMetadata)
            return;

        var metadataReader = peReader.GetMetadataReader();
        if (!metadataReader.IsAssembly)
            return;

        var assemblyName = metadataReader.GetString(metadataReader.GetAssemblyDefinition().Name);

        foreach (var handle in metadataReader.TypeDefinitions)
        {
            var typeDef = metadataReader.GetTypeDefinition(handle);
            var parentHandle = typeDef.BaseType;

            // Only interested in enum types
            if (parentHandle.IsNil)
                continue;

            var parentFullName = ResolveBaseTypeName(metadataReader, parentHandle);
            if (parentFullName == null ||
                !string.Equals(parentFullName, "System.Enum", StringComparison.Ordinal))
                continue;

            var subjectId = ComputeTypeDefSubjectId(metadataReader, handle, assemblyName);
            if (string.IsNullOrEmpty(subjectId) || !seenSubjectIds.Add(subjectId))
                continue;

            uint flags = ComputeTypeFlags(metadataReader, typeDef, parentHandle);
            mergedFlags.Add(flags);
            mergedSubjectIds.Add(subjectId);

            // Read field entries (names + constant values) from PE metadata
            foreach (var fieldHandle in typeDef.GetFields())
            {
                var fieldDef = metadataReader.GetFieldDefinition(fieldHandle);
                var fieldName = metadataReader.GetString(fieldDef.Name);

                // Skip the implicit "value__" instance field
                if (string.Equals(fieldName, "value__", StringComparison.Ordinal))
                    continue;

                long? constantValue = ReadFieldConstantValue(metadataReader, fieldDef);
                if (!constantValue.HasValue)
                    continue;

                enumFieldEntries.Add(new ReflectionMemberFieldEntry(
                    subjectId, fieldName, MetadataTokens.GetToken(fieldHandle), constantValue));
            }
        }
    }





    /// <summary>
    /// Fallback: scan PE metadata of all closure assemblies to collect enum field
    /// entries (name + constant value) when _reflectionMemberSupport.FieldEntries
    /// is empty (e.g. during foundation-dll codegen for stub-based families).
    /// </summary>
    private IReadOnlyList<ReflectionMemberFieldEntry> CollectEnumFieldEntriesFromMetadata()
    {
        var seenSubjectIds = new HashSet<string>(_moduleTypeSubjectIds, StringComparer.Ordinal);
        var enumFieldEntries = new List<ReflectionMemberFieldEntry>();
        var dummyFlags = new List<uint>();
        var dummySubjectIds = new List<string>();

        foreach (var assemblyPath in _cachedClosureAssemblyPaths)
        {
            try
            {
                CollectEnumTypesAndFieldsFromAssembly(assemblyPath, seenSubjectIds,
                    dummyFlags, dummySubjectIds, enumFieldEntries);
            }
            catch
            {
                continue;
            }
        }

        return enumFieldEntries;
    }





    /// <summary>
    /// Read the constant value from a field definition's Constant metadata.
    /// Returns null if the field has no constant or the type is unsupported.
    /// </summary>
    private static long? ReadFieldConstantValue(
        System.Reflection.Metadata.MetadataReader reader,
        System.Reflection.Metadata.FieldDefinition fieldDef)
    {
        try
        {
            var constHandle = fieldDef.GetDefaultValue();
            if (constHandle.IsNil)
                return null;

            var constant = reader.GetConstant(constHandle);
            var blobReader = reader.GetBlobReader(constant.Value);

            // PrimitiveTypeCode matches the ECMA 335 constant type codes
            // used by System.Reflection.Metadata (not to be confused with
            // the unrelated ConstantTypeCode enum from a different namespace).
            switch ((System.Reflection.Metadata.PrimitiveTypeCode)constant.TypeCode)
            {
                case System.Reflection.Metadata.PrimitiveTypeCode.Boolean:
                    return blobReader.ReadBoolean() ? 1L : 0L;
                case System.Reflection.Metadata.PrimitiveTypeCode.Byte:
                    return blobReader.ReadByte();
                case System.Reflection.Metadata.PrimitiveTypeCode.SByte:
                    return blobReader.ReadSByte();
                case System.Reflection.Metadata.PrimitiveTypeCode.Int16:
                    return blobReader.ReadInt16();
                case System.Reflection.Metadata.PrimitiveTypeCode.UInt16:
                    return blobReader.ReadUInt16();
                case System.Reflection.Metadata.PrimitiveTypeCode.Char:
                    return blobReader.ReadChar();
                case System.Reflection.Metadata.PrimitiveTypeCode.Int32:
                    return blobReader.ReadInt32();
                case System.Reflection.Metadata.PrimitiveTypeCode.UInt32:
                    return blobReader.ReadUInt32();
                case System.Reflection.Metadata.PrimitiveTypeCode.Int64:
                    return blobReader.ReadInt64();
                case System.Reflection.Metadata.PrimitiveTypeCode.UInt64:
                    return unchecked((long)blobReader.ReadUInt64());
                default:
                    return null;
            }
        }
        catch
        {
            return null;
        }
    }




    /// <summary>
    /// Collect enum type subject IDs from reflection member support data (preferred)
    /// or fall back to scanning PE metadata of closure assemblies.
    /// </summary>
    private static IReadOnlySet<string> CollectEnumTypeSubjectIds(
        ReflectionMemberSupportModel reflectionMemberSupport,
        IReadOnlyList<string> closureAssemblyPaths)
    {
        // Prefer reflection member field entries when available
        if (reflectionMemberSupport.FieldEntries.Count > 0)
        {
            return new HashSet<string>(
                reflectionMemberSupport.FieldEntries.Select(f => f.DeclaringTypeSubjectId),
                StringComparer.Ordinal);
        }

        // Fallback: scan PE metadata of closure assemblies for enum types
        var enumSubjectIds = new HashSet<string>(StringComparer.Ordinal);
        var seenSubjectIds = new HashSet<string>(StringComparer.Ordinal);

        foreach (var assemblyPath in closureAssemblyPaths)
        {
            try
            {
                CollectEnumTypeSubjectIdsFromAssembly(assemblyPath, seenSubjectIds, enumSubjectIds);
            }
            catch
            {
                // Skip assemblies that can't be read
                continue;
            }
        }

        return enumSubjectIds;
    }





    /// <summary>
    /// Build a type → {value → name} map for all enum types in the closure.
    /// Prefers FieldEntries from reflection member support data; falls back
    /// to scanning PE metadata of closure assemblies.
    /// Used by S2 (BoxToString switch) and other value→name lookups.
    /// </summary>
    private Dictionary<string, Dictionary<long, string>> BuildEnumValueToNameMap()
    {
        var map = new Dictionary<string, Dictionary<long, string>>(StringComparer.Ordinal);

        IReadOnlyList<ReflectionMemberFieldEntry> fieldEntries;
        if (_reflectionMemberSupport.FieldEntries.Count > 0)
        {
            fieldEntries = _reflectionMemberSupport.FieldEntries;
        }
        else
        {
            fieldEntries = CollectEnumFieldEntriesFromMetadata();
        }

        foreach (var field in fieldEntries)
        {
            if (field.ConstantValue == null)
                continue;

            if (!map.TryGetValue(field.DeclaringTypeSubjectId, out var valueToName))
            {
                valueToName = new Dictionary<long, string>();
                map[field.DeclaringTypeSubjectId] = valueToName;
            }

            // First field with this value wins (consistent with Enum.GetName behavior).
            if (!valueToName.ContainsKey(field.ConstantValue.Value))
                valueToName[field.ConstantValue.Value] = field.FieldName;
        }

        return map;
    }





    /// <summary>
    /// Scan a single assembly's PE metadata for enum type definitions and
    /// add their subject IDs to the provided set.
    /// </summary>
    private static void CollectEnumTypeSubjectIdsFromAssembly(
        string assemblyPath,
        HashSet<string> seenSubjectIds,
        HashSet<string> enumSubjectIds)
    {
        if (string.IsNullOrEmpty(assemblyPath) || !File.Exists(assemblyPath))
            return;

        using var stream = File.OpenRead(assemblyPath);
        using var peReader = new System.Reflection.PortableExecutable.PEReader(stream);
        if (!peReader.HasMetadata)
            return;

        var metadataReader = peReader.GetMetadataReader();
        if (!metadataReader.IsAssembly)
            return;

        var assemblyName = metadataReader.GetString(metadataReader.GetAssemblyDefinition().Name);

        foreach (var handle in metadataReader.TypeDefinitions)
        {
            var typeDef = metadataReader.GetTypeDefinition(handle);
            var parentHandle = typeDef.BaseType;

            if (parentHandle.IsNil)
                continue;

            var parentFullName = ResolveBaseTypeName(metadataReader, parentHandle);
            if (parentFullName == null ||
                !string.Equals(parentFullName, "System.Enum", StringComparison.Ordinal))
                continue;

            var subjectId = ComputeTypeDefSubjectId(metadataReader, handle, assemblyName);
            if (string.IsNullOrEmpty(subjectId) || !seenSubjectIds.Add(subjectId))
                continue;

            enumSubjectIds.Add(subjectId);
        }
    }





    private void LogHotpatchCoverage()
    {
        if (HotpatchEligibleMethodCount <= 0)
            return;

        var pct = (double)HotpatchEntryCount / HotpatchEligibleMethodCount * 100;
        Console.Error.WriteLine(
            $"[hotpatch] dispatch coverage: {HotpatchEntryCount}/{HotpatchEligibleMethodCount} ({pct:F1}%)");

        if (HotpatchEntryCount < HotpatchEligibleMethodCount)
        {
            var coveredSymbols = new HashSet<string>(
                GetHotpatchableMethods().Select(m => m.NativeSymbol),
                StringComparer.Ordinal);
            var eligibleSymbols = new HashSet<string>(StringComparer.Ordinal);
            var missing = _methodsBySubjectId.Values
                .Where(m => (m.Instructions.Count > 0 || m.IsPInvoke) && eligibleSymbols.Add(m.NativeSymbol))
                .Where(m => !coveredSymbols.Contains(m.NativeSymbol))
                .Select(m => m.SubjectId)
                .Take(10)
                .ToList();
            if (missing.Count > 0)
            {
                Console.Error.WriteLine($"[hotpatch] missing {HotpatchEligibleMethodCount - HotpatchEntryCount} methods from dispatch table (first {missing.Count}):");
                foreach (var m in missing)
                    Console.Error.WriteLine($"  {m}");
            }
        }
    }

}
