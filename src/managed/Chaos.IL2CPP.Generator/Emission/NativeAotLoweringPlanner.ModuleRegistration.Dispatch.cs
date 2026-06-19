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
                $"{{ CHAOS_IL2CPP_UINT8 __v; std::memcpy(&__v, p, 1); p += 1; attr->{fieldName} = __v; }}",

            CustomAttributeLiteralKind.SByte =>
                $"{{ CHAOS_IL2CPP_INT8 __v; std::memcpy(&__v, p, 1); p += 1; attr->{fieldName} = __v; }}",

            CustomAttributeLiteralKind.Int16 =>
                $"{{ CHAOS_IL2CPP_INT16 __v; std::memcpy(&__v, p, 2); p += 2; attr->{fieldName} = __v; }}",

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
                $"{{ CHAOS_IL2CPP_UINT16 __len; std::memcpy(&__len, p, 2); p += 2; auto* __abi = chaos_runtime_get_abi_v0(); auto* __rt = __abi ? __abi->get_current_runtime_state() : nullptr; auto* __th = __abi ? __abi->get_current_thread_state() : nullptr; attr->{fieldName} = (__abi != nullptr && __abi->string_new_utf8 != nullptr) ? reinterpret_cast<CHAOS_IL2CPP_INTPTR>(__abi->string_new_utf8(__rt, __th, reinterpret_cast<const char*>(p), __len)) : 0; p += __len; }}",

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
            sb.AppendLine("    /* .custom_attribute_method_offset   = */ s_custom_attribute_method_offset,");
            sb.AppendLine("    /* .custom_attribute_field_offset    = */ s_custom_attribute_field_offset,");
            sb.AppendLine("    /* .custom_attribute_property_offset = */ s_custom_attribute_property_offset,");
            sb.AppendLine("    /* .custom_attribute_param_offset    = */ s_custom_attribute_param_offset,");
            sb.AppendLine("    /* .custom_attribute_method_count    = */ s_custom_attribute_method_count,");
            sb.AppendLine("    /* .custom_attribute_field_count     = */ s_custom_attribute_field_count,");
            sb.AppendLine("    /* .custom_attribute_property_count  = */ s_custom_attribute_property_count,");
            sb.Append("    /* .custom_attribute_param_count   = */ s_custom_attribute_param_count,");
        }
        else
        {
            sb.AppendLine("    /* .custom_attribute_blob       = */ nullptr,  // Tier 1 -- deferred");
            sb.AppendLine("    /* .custom_attribute_offset     = */ nullptr,");
            sb.AppendLine("    /* .custom_attribute_entity_count = */ 0u,");
            sb.AppendLine("    /* .custom_attribute_materializer = */ nullptr,");
            sb.AppendLine("    /* .custom_attribute_method_offset   = */ nullptr,");
            sb.AppendLine("    /* .custom_attribute_field_offset    = */ nullptr,");
            sb.AppendLine("    /* .custom_attribute_property_offset = */ nullptr,");
            sb.AppendLine("    /* .custom_attribute_param_offset    = */ nullptr,");
            sb.AppendLine("    /* .custom_attribute_method_count    = */ 0u,");
            sb.AppendLine("    /* .custom_attribute_field_count     = */ 0u,");
            sb.AppendLine("    /* .custom_attribute_property_count  = */ 0u,");
            sb.Append("    /* .custom_attribute_param_count   = */ 0u,");
        }
    }



    internal static string BuildAbiManifest(IReadOnlyList<AotCoreIrMethodArtifact> reachableMethods)
    {
        if (reachableMethods.Count == 0)
        {
            return string.Empty;
        }

        int totalParams = reachableMethods.Sum(m => m.ParameterAbis.Count);
        uint checksum = ComputeAbiManifestChecksum(reachableMethods);

        // Prefix-sum array
        var prefixSums = new int[reachableMethods.Count + 1];
        uint runningTotal = 0;
        for (int i = 0; i < reachableMethods.Count; i++)
        {
            prefixSums[i] = (int)runningTotal;
            runningTotal += (uint)reachableMethods[i].ParameterAbis.Count;
        }
        prefixSums[reachableMethods.Count] = (int)runningTotal;

        // Method entry models
        var methodModels = new List<ScriptObject>(reachableMethods.Count);
        for (int i = 0; i < reachableMethods.Count; i++)
        {
            var method = reachableMethods[i];
            methodModels.Add(new ScriptObject
            {
                ["return_carrier"] = (int)method.ReturnAbi.CarrierKindCode,
                ["param_count"] = method.ParameterAbis.Count,
                ["native_symbol"] = method.NativeSymbol,
            });
        }

        // Flat parameter carrier array
        int totalParamsActual = totalParams > 0 ? totalParams : 1;
        var paramCarriers = new int[totalParamsActual];
        if (totalParams > 0)
        {
            int idx = 0;
            foreach (var method in reachableMethods)
            {
                foreach (var abi in method.ParameterAbis)
                {
                    paramCarriers[idx++] = (int)abi.CarrierKindCode;
                }
            }
        }

        var model = new ScriptObject
        {
            ["methods_count"] = reachableMethods.Count,
            ["method_count_plus_one"] = reachableMethods.Count + 1,
            ["total_params"] = totalParams,
            ["total_params_actual"] = totalParamsActual,
            ["checksum"] = checksum,
            ["prefix_sums"] = prefixSums,
            ["methods"] = methodModels,
            ["param_carriers"] = paramCarriers,
        };

        return ScribanTemplateRenderer.RenderTemplate(
            NativeAotTemplateCatalog.GetAbiManifestTemplate(), model);
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
            emptySb.AppendLine("// --- External Runtime Dispatch Table (empty) ---");
            emptySb.AppendLine("extern \"C\" const char* kChaosExternalRuntimeSubjects[1] = { nullptr };");
            emptySb.AppendLine("extern \"C\" void* kChaosExternalRuntimeFnTable[1] = { nullptr };");
            emptySb.AppendLine("extern \"C\" CHAOS_IL2CPP_INT32 kChaosExternalRuntimeCount = 0;");
            return emptySb.ToString();
        }

        // Emit entries in dict-value index order so the table position matches
        // the ExternalRuntimeTableIndex used by call sites.  The dict is populated
        // deterministically (same input -> same scan order), so this is stable.
        var entriesByIndex = new KeyValuePair<string, int>[_externalRuntimeSubjects.Count];
        foreach (var kvp in _externalRuntimeSubjects)
        {
            int idx = kvp.Value;
            if (idx >= 0 && idx < entriesByIndex.Length)
                entriesByIndex[idx] = kvp;
        }

        // Build entry models
        var entryModels = new ScriptObject[entriesByIndex.Length];
        for (int i = 0; i < entriesByIndex.Length; i++)
        {
            string subjectId = entriesByIndex[i].Key;
            string? resolvedHelper = null;
            bool hasHelper = helperSymbolBySubjectId?.TryGetValue(subjectId, out resolvedHelper) == true;

            // If the helper has a DirectNativeSymbol, the chaos_external_runtime_*
            // function doesn't actually exist — only the DirectNativeSymbol stub does.
            // Use nullptr in the dispatch table to avoid unresolved linker symbols,
            // letting the TPG Scriban template fill in the entry at runtime via
            // subject-id string matching.
            //
            // Note: Do NOT add extra conditions here (e.g. checking h.Source).
            // DirectNativeSymbol alone is the reliable indicator that the wrapper
            // function body is NOT emitted as C++ code, regardless of Source content.
            if (hasHelper && _externalRuntimeHelpers is { } helpers)
            {
                foreach (var h in helpers)
                {
                    if (h.SubjectId == subjectId && h.DirectNativeSymbol != null)
                    {
                        hasHelper = false;
                        resolvedHelper = null;
                        break;
                    }
                }
            }

            var entryModel = new ScriptObject
            {
                ["subject_literal"] = EscapeCppStringLiteral(subjectId),
                ["has_helper_symbol"] = hasHelper,
            };
            if (hasHelper)
            {
                entryModel["helper_symbol"] = resolvedHelper!;
            }
            entryModels[i] = entryModel;
        }

        var model = new ScriptObject
        {
            ["subject_count"] = entriesByIndex.Length,
            ["entries"] = entryModels,
        };

        var tableResult = ScribanTemplateRenderer.RenderTemplate(
            NativeAotTemplateCatalog.GetExternalRuntimeDispatchTableTemplate(), model);

        // Append IL data table for interpreter fallback (Phase 3)
        var hasIlData = false;
        var ilBytes = new byte[entriesByIndex.Length][];
        for (int i = 0; i < entriesByIndex.Length; i++)
        {
            string sid = entriesByIndex[i].Key;
            try
            {
                int slashIdx = sid.IndexOf('/');
                if (slashIdx > 0)
                {
                    string assemblyName = sid.Substring(0, slashIdx);
                    string? rtDir = System.IO.Path.GetDirectoryName(typeof(object).Assembly.Location);
                    if (rtDir != null)
                    {
                        string dllPath = System.IO.Path.Combine(rtDir, assemblyName + ".dll");
                        if (System.IO.File.Exists(dllPath))
                        {
                            using var peReader = new System.Reflection.PortableExecutable.PEReader(
                                System.IO.File.OpenRead(dllPath));
                            if (peReader.HasMetadata)
                            {
                                var md = peReader.GetMetadataReader();
                                // Find type+method matching SubjectId
                                int methodSep = sid.IndexOf("::");
                                if (methodSep > 0)
                                {
                                    string typePart = sid.Substring(slashIdx + 1, methodSep - slashIdx - 1);
                                    string methodNameOnly = sid.Substring(methodSep + 2);
                                    int colon = methodNameOnly.IndexOf(':');
                                    if (colon > 0) methodNameOnly = methodNameOnly.Substring(0, colon);
                                    int paren = methodNameOnly.IndexOf('(');
                                    if (paren > 0) methodNameOnly = methodNameOnly.Substring(0, paren);

                                    foreach (var td in md.TypeDefinitions)
                                    {
                                        var tdef = md.GetTypeDefinition(td);
                                        if (md.GetString(tdef.Name) == "<Module>") continue;
                                        string ns = tdef.Namespace.IsNil ? "" : md.GetString(tdef.Namespace);
                                        string tn = md.GetString(tdef.Name);
                                        string fullName = string.IsNullOrEmpty(ns) ? tn : ns + "." + tn;
                                        // SubjectId may use simple type name (e.g. "AesCcm") or
                                        // full qualified name (e.g. "System.Security.Cryptography.AesCcm").
                                        // Try both to handle core crypto methods correctly.
                                        if (fullName != typePart && tn != typePart) continue;

                                        foreach (var mh in tdef.GetMethods())
                                        {
                                            var mdef = md.GetMethodDefinition(mh);
                                            string mn = md.GetString(mdef.Name);
                                            if (mn != methodNameOnly || mdef.RelativeVirtualAddress == 0) continue;
                                            var bodyBlock = peReader.GetMethodBody(mdef.RelativeVirtualAddress);
                                            var ilReader = bodyBlock.GetILReader();
                                            ilBytes[i] = ilReader.ReadBytes(ilReader.RemainingBytes);
                                            hasIlData = true;
                                            break;
                                        }
                                        break;
                                    }
                                }
                            }
                        }
                    }
                }
            }
            catch { }
        }

        if (hasIlData)
        {
            var ilSb = new System.Text.StringBuilder(65536);
            ilSb.AppendLine();
            ilSb.AppendLine("// ── Embedded IL Data for Interpreter Fallback ────────────────");
            ilSb.AppendLine("// ABI export: extern \"C\" for C-language linkage from managed/NativeAot code");
            ilSb.AppendLine("// ABI export: C-language struct to hold embedded IL + AotCoreIr JSON for interpreter fallback");
            ilSb.AppendLine("struct ChaosIlDataEntry {");
            ilSb.AppendLine("    const char* subject_id;");
            ilSb.AppendLine("    const CHAOS_IL2CPP_UINT8* il_data;");
            ilSb.AppendLine("    CHAOS_IL2CPP_INT32 il_size;");
            ilSb.AppendLine("    void* patch_method;");
            ilSb.AppendLine("    const char* json_data;");
            ilSb.AppendLine("};");
            ilSb.AppendLine();

            for (int i = 0; i < entriesByIndex.Length; i++)
            {
                if (ilBytes[i] == null) continue;
                var hex = System.Convert.ToHexString(ilBytes[i]);
                ilSb.AppendLine("static const CHAOS_IL2CPP_UINT8 s_il_" + i + "[] = {");
                for (int j = 0; j < hex.Length; j += 2)
                {
                    if (j % 48 == 0) ilSb.Append("    ");
                    ilSb.Append("0x" + hex.Substring(j, 2) + ", ");
                    if (j % 48 == 46) ilSb.AppendLine();
                }
                ilSb.AppendLine();
                ilSb.AppendLine("};");
            }

            ilSb.AppendLine();
            ilSb.AppendLine("extern \"C\" ChaosIlDataEntry kChaosExternalRuntimeIlData[] = {");
            for (int i = 0; i < entriesByIndex.Length; i++)
            {
                if (ilBytes[i] == null) continue;
                string sid = entriesByIndex[i].Key;
                string? jsonData = _externalRuntimeIlDataJson.TryGetValue(sid, out var jd) ? jd : null;
                string jsonField = jsonData != null ? $"\"{EscapeCppStringLiteral(jsonData)}\"" : "nullptr";
                ilSb.AppendLine($"    {{ \"{EscapeCppStringLiteral(sid)}\", s_il_{i}, sizeof(s_il_{i}), nullptr, {jsonField} }},");
            }
            ilSb.AppendLine("    { nullptr, nullptr, 0, nullptr, nullptr }");
            ilSb.AppendLine("};");
            ilSb.AppendLine();

            tableResult += ilSb.ToString();
        }

        return tableResult;
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



    /// <summary>
    /// Converts a standard GUID string (e.g., "ABCDEF01-2345-6789-ABCD-EF0123456789")
    /// to a 16-byte array. Returns null if the input is not a valid 36-char GUID.
    /// Handles both with and without braces.
    /// </summary>
    private static byte[]? ParseGuidStringToBytes(string guid)
    {
        // Strip optional braces.
        if (guid.StartsWith('{') && guid.EndsWith('}'))
            guid = guid.Substring(1, guid.Length - 2);

        if (guid.Length != 36) return null;
        if (guid[8] != '-' || guid[13] != '-' || guid[18] != '-' || guid[23] != '-')
            return null;

        try
        {
            var bytes = new byte[16];
            // GUID bytes are stored in a specific order in the binary representation:
            // Bytes 0-3: first group (little-endian uint32)
            // Bytes 4-5: second group (little-endian uint16)
            // Bytes 6-7: third group (little-endian uint16)
            // Bytes 8-15: remaining groups (big-endian)
            bytes[0] = Convert.ToByte(guid.Substring(0, 2), 16);
            bytes[1] = Convert.ToByte(guid.Substring(2, 2), 16);
            bytes[2] = Convert.ToByte(guid.Substring(4, 2), 16);
            bytes[3] = Convert.ToByte(guid.Substring(6, 2), 16);
            bytes[4] = Convert.ToByte(guid.Substring(9, 2), 16);
            bytes[5] = Convert.ToByte(guid.Substring(11, 2), 16);
            bytes[6] = Convert.ToByte(guid.Substring(14, 2), 16);
            bytes[7] = Convert.ToByte(guid.Substring(16, 2), 16);
            bytes[8] = Convert.ToByte(guid.Substring(19, 2), 16);
            bytes[9] = Convert.ToByte(guid.Substring(21, 2), 16);
            bytes[10] = Convert.ToByte(guid.Substring(24, 2), 16);
            bytes[11] = Convert.ToByte(guid.Substring(26, 2), 16);
            bytes[12] = Convert.ToByte(guid.Substring(28, 2), 16);
            bytes[13] = Convert.ToByte(guid.Substring(30, 2), 16);
            bytes[14] = Convert.ToByte(guid.Substring(32, 2), 16);
            bytes[15] = Convert.ToByte(guid.Substring(34, 2), 16);
            return bytes;
        }
        catch
        {
            return null;
        }
    }



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
    internal string BuildJitMethodRegistration(
        IReadOnlyList<AotCoreIrMethodArtifact> methodsForLowering,
        MetadataRegistrationArtifact metadataRegistration,
        string? jitDataOutputPath = null)
    {
        if (methodsForLowering == null || methodsForLowering.Count == 0)
            return string.Empty;

        var tokenLookup = new MetadataTokenLookup(metadataRegistration.Registrations);

        var sb = new StringBuilder(4096);
        sb.AppendLine("// --- JIT Method Entry Table ---");
        sb.AppendLine("// Auto-generated by chaos-il2cpp codegen for --mode jit.");
        sb.AppendLine("#include <cstdint>");
        sb.AppendLine("#include \"jit_registration.h\"");
        sb.AppendLine();

        // Serialize each method to JSON and emit as C++ string literal + JitEntry.
        var jsonOptions = new JsonSerializerOptions
        {
            PropertyNamingPolicy = JsonNamingPolicy.CamelCase,
            DefaultIgnoreCondition = System.Text.Json.Serialization.JsonIgnoreCondition.WhenWritingNull,
            NumberHandling = System.Text.Json.Serialization.JsonNumberHandling.AllowNamedFloatingPointLiterals,
        };

        // Pre-serialize all methods to JSON.
        var jsonStrings = new string[methodsForLowering.Count];
        for (int i = 0; i < methodsForLowering.Count; i++)
        {
            jsonStrings[i] = JsonSerializer.Serialize(methodsForLowering[i], jsonOptions);
        }

        if (jitDataOutputPath != null)
        {
            // Write binary .jdata file with concatenated UTF-8 JSON blobs.
            // Each JSON string is written sequentially; JitEntry entries use
            // (offset, length) pairs to reference the data.
            using var jdataStream = new FileStream(
                jitDataOutputPath, FileMode.Create, FileAccess.Write, FileShare.None);
            var offsets = new uint[methodsForLowering.Count];

            for (int i = 0; i < methodsForLowering.Count; i++)
            {
                offsets[i] = (uint)jdataStream.Position;
                byte[] utf8Bytes = System.Text.Encoding.UTF8.GetBytes(jsonStrings[i]);
                jdataStream.Write(utf8Bytes, 0, utf8Bytes.Length);
            }

            sb.AppendLine("// AotCoreIr JSON data is stored in the .jdata file.");
            sb.AppendLine("// Load at startup via ChaosJitDataLoad(), pass pointer to RegisterJitEntryMethods().");
            sb.AppendLine();

            // Emit JitEntry array with offset-based addressing
            sb.Append("extern \"C\" const JitEntry kChaosJitEntries[")
                .Append(methodsForLowering.Count)
                .AppendLine("] =");
            sb.AppendLine("{");
            for (int i = 0; i < methodsForLowering.Count; i++)
            {
                var method = methodsForLowering[i];
                uint token = tokenLookup.TryGetMethodToken(method.SubjectId);
                uint jsonLen = (uint)jsonStrings[i].Length;

                sb.Append("    { ")
                    .Append(offsets[i])
                    .Append("u, ")
                    .Append(jsonLen)
                    .Append("u, 0x")
                    .Append(token.ToString("X8"))
                    .Append("u, 0u }");
                if (i < methodsForLowering.Count - 1)
                    sb.Append(',');
                sb.AppendLine();
            }
            sb.AppendLine("};");
            sb.AppendLine();
        }
        else
        {
            // Legacy path: emit JSON as C++ string literals (backward compat).
            const int maxStringLiteralLen = 16000;
            for (int i = 0; i < methodsForLowering.Count; i++)
            {
                string escaped = EscapeCppStringLiteral(jsonStrings[i]);
                sb.Append("static const char kJitJson_")
                    .Append(i)
                    .Append("[] = ");
                if (escaped.Length <= maxStringLiteralLen)
                {
                    sb.Append("\"").Append(escaped).AppendLine("\";");
                }
                else
                {
                    sb.AppendLine();
                    for (int pos = 0; pos < escaped.Length;)
                    {
                        int chunkEnd = Math.Min(pos + maxStringLiteralLen, escaped.Length);
                        if (chunkEnd < escaped.Length)
                        {
                            int adjusted = chunkEnd;
                            while (adjusted > pos && escaped[adjusted - 1] == '\\')
                                adjusted--;
                            if (adjusted > pos)
                                chunkEnd = adjusted;
                        }
                        int chunkLen = chunkEnd - pos;
                        sb.Append("    \"").Append(escaped, pos, chunkLen).AppendLine("\"");
                        pos = chunkEnd;
                    }
                    sb.AppendLine("    ;");
                }
            }

            sb.AppendLine();

            // Emit JitEntry array with offset-based addressing (no .jdata file,
            // but the struct field is now uint32_t json_offset, so we emit
            // synthetic offsets that won't be used — jit_data is nullptr at runtime).
            // NOTE: The old format used pointer-based addressing (kJitJson_N).
            // With the struct field renamed to json_offset (uint32_t), the old
            // string-pointer entries are no longer supported. The legacy path
            // still works when jitDataOutputPath is provided, using the .jdata
            // file format. This fallback is kept for backward compat with tests
            // that don't pass a jitDataOutputPath.
            uint accumulatedOffset = 0;
            sb.Append("extern \"C\" const JitEntry kChaosJitEntries[")
                .Append(methodsForLowering.Count)
                .AppendLine("] =");
            sb.AppendLine("{");
            for (int i = 0; i < methodsForLowering.Count; i++)
            {
                var method = methodsForLowering[i];
                uint token = tokenLookup.TryGetMethodToken(method.SubjectId);
                uint jsonLen = (uint)jsonStrings[i].Length;

                sb.Append("    { ")
                    .Append(accumulatedOffset)
                    .Append("u, ")
                    .Append(jsonLen)
                    .Append("u, 0x")
                    .Append(token.ToString("X8"))
                    .Append("u, 0u }");
                accumulatedOffset += jsonLen;
                if (i < methodsForLowering.Count - 1)
                    sb.Append(',');
                sb.AppendLine();
            }
            sb.AppendLine("};");
            sb.AppendLine();
        }

        // Emit count symbol
        sb.Append("extern \"C\" const CHAOS_IL2CPP_UINT32 kChaosJitEntryCount = ")
            .Append(methodsForLowering.Count)
            .AppendLine("u;");

        return sb.ToString();
    }

}
