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

    private string BuildDispatchEntryCode(IReadOnlyList<AotCoreIrMethodArtifact> methods)
    {
        if (methods.Count == 0) return string.Empty;
        System.Console.WriteLine("F_a64e93");

        ulong defaultStringId = _stringIdMapping is { Count: > 0 }
            ? _stringIdMapping.First().Value
            : 0UL;

        var methodEntries = new List<ScriptObject>(methods.Count);
        var subjectEntries = new List<ScriptObject>();

        // ── Phase 1 diagnostic: SubjectId match rate (fact-only, excluding Benchmark_) ──
        if (_subjectMethodSubjectIds is { Count: > 0 })
        {
            var subjectIdsInMethods = new HashSet<string>(methods.Select(m => m.SubjectId!),
                StringComparer.Ordinal);
            int matched = 0, missed = 0, benchmarkSkipped = 0;
            foreach (var sid in _subjectMethodSubjectIds)
            {
                // Skip Benchmark_ entries — they are excluded from subject entries
                // by IsSubjectMethod() at line 4837, so counting them as "missed" is
                // misleading. The diagnostic focuses on fact-wrapper match rate.
                if (sid.Contains("::Benchmark_", StringComparison.Ordinal))
                {
                    benchmarkSkipped++;
                    continue;
                }
                if (subjectIdsInMethods.Contains(sid))
                    matched++;
                else
                    missed++;
            }
            int totalConsidered = matched + missed;
            Console.Error.WriteLine($"[SUBJECT-MATCH] {matched} matched, {missed} missed " +
                $"(out of {totalConsidered} fact subject-methods + {benchmarkSkipped} benchmark skipped, " +
                $"{methods.Count} methods in dispatch)");
            if (missed > 0)
            {
                // Log first 10 missed SubjectIds as samples
                int sampleCount = 0;
                foreach (var sid in _subjectMethodSubjectIds)
                {
                    if (sid.Contains("::Benchmark_", StringComparison.Ordinal))
                        continue;
                    if (!subjectIdsInMethods.Contains(sid))
                    {
                        if (sampleCount < 10)
                            Console.Error.WriteLine($"  [SUBJECT-MISS] {sid}");
                        sampleCount++;
                        if (sampleCount == 10)
                        {
                            Console.Error.WriteLine($"  [SUBJECT-MISS] ... and {missed - 10} more");
                            break;
                        }
                    }
                }
            }
        }
        // Build set of SubjectIds that have a _0 variant (fact wrapper).
        // Used to skip _1/_2 variants when _0 covers the fact test.
        // Normalize the variant suffix _N before the return type (e.g., "_0:System.Int64()"
        // → ":System.Int64()") so that _0 and _1 variants of the same method match.
        var subjectIdsWithZero = new HashSet<string>(StringComparer.Ordinal);
        for (int vi = 0; vi < methods.Count; vi++)
        {
            var vm = methods[vi];
            if (IsSubjectMethod(vm.SubjectId) && vm.NativeSymbol != null
                && vm.NativeSymbol.EndsWith("_0", StringComparison.Ordinal))
                subjectIdsWithZero.Add(StripSubjectVariantSuffix(vm.SubjectId ?? string.Empty));
        }
        int skippedSubjectVariants = 0;
        for (int i = 0; i < methods.Count; i++)
        {
            var method = methods[i];
            var ac = method.ParameterCount;
            var isInstance = !method.IsStatic;

            var paramList = new List<ScriptObject>(ac + (isInstance ? 1 : 0));
            if (isInstance)
            {
                paramList.Add(new ScriptObject
                {
                    ["is_string"] = false,
                    ["is_this"] = true,
                });
            }
            for (int j = 0; j < ac; j++)
            {
                var abi = j < method.ParameterAbis.Count ? method.ParameterAbis[j] : null;
                var isString = abi != null && IsStringParameterSlot(abi);
                var isByRef = abi != null && (abi.CarrierKindCode == AotCoreIrAbiCarrierKind.ByRef
                    || abi.CarrierKindCode == AotCoreIrAbiCarrierKind.ByRefToValueType
                    || abi.CarrierKindCode == AotCoreIrAbiCarrierKind.MultiReturn);
                paramList.Add(new ScriptObject
                {
                    ["is_string"] = isString,
                    ["is_byref"] = isByRef,
                    ["is_this"] = false,
                });
            }

            var entry = new ScriptObject
            {
                ["index"] = i,
                ["native_symbol"] = method.NativeSymbol,
                ["param_count"] = ac + (isInstance ? 1 : 0),
                ["params"] = paramList,
                ["is_instance"] = isInstance,
            };

            methodEntries.Add(entry);

            if (IsSubjectMethod(method.SubjectId))
            {
                // Skip _1/_2 when _0 exists with same SubjectId AND calls external
                if (method.NativeSymbol != null
                    && (method.NativeSymbol.EndsWith("_1", StringComparison.Ordinal)
                        || method.NativeSymbol.EndsWith("_2", StringComparison.Ordinal))
                    && subjectIdsWithZero.Contains(StripSubjectVariantSuffix(method.SubjectId ?? string.Empty)))
                {
                    bool extCall = false;
                    foreach (var instr in method.Instructions)
                    {
                        string callee = instr.Callee ?? string.Empty;
                        if (callee.Length > 0 && _externalRuntimeSubjects.ContainsKey(
                                ManagedNaming.NormalizeSubjectIdAssembly(callee)))
                        { extCall = true; break; }
                    }
                    if (extCall) { skippedSubjectVariants++; continue; }
                }
                int subjectIdx = ExtractSubjectIndex(method.SubjectId!);
                if (subjectIdx < 0)
                    // Assign temporary unique index — will be sorted by subject_index
                    // and reassigned to sequential 0..N-1 after dedup+filter below.
                    subjectIdx = subjectEntries.Count;

                subjectEntries.Add(new ScriptObject
                {
                    ["subject_index"] = subjectIdx,
                    ["contract_index"] = subjectIdx,
                    ["method_index"] = i,
                });
            }
        }

        if (skippedSubjectVariants > 0)
            Console.Error.WriteLine($"[SUBJECT-VARIANT-SKIP] Skipped {skippedSubjectVariants} subject variant(s) (SubjectId-based)");
        // Deduplicate subject entries by subject_index — each unique subject
        // produces both a wrapper and the actual method body (2 entries) but
        // the slot map should expose only one entry per subject (the wrapper).
        // Keeping both causes the fact runner to iterate 2× expected subjects
        // and the wrappers lack EH catch blocks, so noexcept violations in the
        // actual method propagate to the fact runner's outer catch as abort().
        var seenSubjectIndices = new HashSet<int>();
        var deduped = new List<ScriptObject>(subjectEntries.Count);
        foreach (var se in subjectEntries)
        {
            if (seenSubjectIndices.Add((int)se["subject_index"]))
                deduped.Add(se);
        }
        if (deduped.Count < subjectEntries.Count)
        {
            Console.Error.WriteLine($"[SUBJECT-DEDUP] Removed {subjectEntries.Count - deduped.Count} duplicate subject entries (wrapper+actual → one per subject)");
            subjectEntries = deduped;
        }

        // Use methods.Count as the upper bound for kSubjectSlotMap entries since
        // it matches the kMethodTable[] dimension (kAotMethodCount at link time).
        // HotpatchEntryCount is now equal to methods.Count after removing the
        // NativeSymbol dedup from GetHotpatchableMethods(), but methods.Count
        // is the authoritative bound: kSubjectSlotMap stores kMethodTable indices,
        // and the dispatch loop passes them to ChaosDispatchMethodGetValue which
        // indexes into both kMethodTable (via default-arg thunks) and
        // s_hotpatch_entries (which is now the same length).
        int actualEntryCount = methods.Count;
        if (actualEntryCount > 0 && subjectEntries.Count > 0)
        {
            var filtered = new List<ScriptObject>(subjectEntries.Count);
            foreach (var se in subjectEntries)
            {
                if ((int)se["method_index"] < actualEntryCount)
                    filtered.Add(se);
            }
            if (filtered.Count < subjectEntries.Count)
            {
                Console.Error.WriteLine($"[SUBJECT-FILTER] Removed {subjectEntries.Count - filtered.Count} subject(s) with method_index >= {actualEntryCount}");
                subjectEntries = filtered;
            }
        }

        // Sort subject entries by metadata order so the slot map maps subject
        // indices correctly.  Build a metadata-position lookup from the ordered
        // _subjectMethodSubjectIds list (preserving TPG's ATG order), filtering out
        // benchmark entries (which IsSubjectMethod already excludes).  Use the
        // position in this filtered list as the sort key.
        if (_subjectMethodSubjectIds is { Count: > 0 })
        {
            // Build SubjectId → metadata-order-position map (fact only)
            var metadataOrder = new Dictionary<string, int>(StringComparer.Ordinal);
            int factPos = 0;
            foreach (var sid in _subjectMethodSubjectIds)
            {
                if (!sid.Contains("::Benchmark_", StringComparison.Ordinal))
                {
                    metadataOrder[sid] = factPos;
                    factPos++;
                }
            }
            subjectEntries = [.. subjectEntries.OrderBy(se =>
            {
                var methodIdx = (int)se["method_index"];
                var method = methods[methodIdx];
                if (method.SubjectId != null)
                {
                    // Exact match
                    if (metadataOrder.TryGetValue(method.SubjectId, out int order))
                        return order;
                    // Fuzzy match: try by suffix after ::
                    var methodName = method.SubjectId.Contains("::")
                        ? method.SubjectId.Substring(method.SubjectId.LastIndexOf("::") + 2)
                        : method.SubjectId;
                    foreach (var kvp in metadataOrder)
                    {
                        if (kvp.Key.EndsWith(methodName, StringComparison.Ordinal))
                            return kvp.Value;
                    }
                }
                return int.MaxValue; // Unknown methods go to the end
            })];
            // Reassign sequential subject_index and contract_index after sorting.
            // NOTE: kSubjectContractMap (rendered by Scriban from contract_index) is
            // intentionally sequential 0..N-1 matching subject_index — there are
            // currently NO downstream C++ consumers of kSubjectContractMap (confirmed
            // by codebase search). contract_index was previously set to the Subject_N
            // parsed number (which has gaps after dedup/filtering), but since nothing
            // reads it, sequential indexing is safe and cleaner for future use.
            for (int sei = 0; sei < subjectEntries.Count; sei++)
            {
                subjectEntries[sei]["subject_index"] = sei;
                subjectEntries[sei]["contract_index"] = sei;
            }
        }

        Console.Error.WriteLine($"[DISPATCH-DIAG] total methods: {methods.Count}, subject entries: {subjectEntries.Count}");
        if (subjectEntries.Count > 0)
        {
            Console.Error.WriteLine($"[DISPATCH-DIAG] first subject entry: method_index={subjectEntries[0]["method_index"]}, subject_index={subjectEntries[0]["subject_index"]}");
        }
        else if (methods.Count > 0)
        {
            // Show assembly distribution
            var assemblyCounts = methods
                .Select(m => m.SubjectId?.Split('/').FirstOrDefault() ?? "?")
                .GroupBy(a => a)
                .OrderByDescending(g => g.Count())
                .ToList();
            Console.Error.WriteLine($"[DISPATCH-DIAG] method SubjectId assembly distribution:");
            foreach (var g in assemblyCounts)
                Console.Error.WriteLine($"  {g.Key}: {g.Count()}");
            Console.Error.WriteLine($"[DISPATCH-DIAG] sample SubjectIds (first 5):");
            for (int si = 0; si < Math.Min(5, methods.Count); si++)
            {
                bool isSubj = IsSubjectMethod(methods[si].SubjectId);
                Console.Error.WriteLine($"  [{si}] subjectId={methods[si].SubjectId} isSubject={isSubj}");
            }
            // Check if ANY of the SubjectIds from the metadata match
            if (_subjectMethodSubjectIds is { Count: > 0 })
            {
                var matchedIds = methods
                    .Select(m => m.SubjectId)
                    .Where(id => _subjectMethodSubjectIds.Contains(id))
                    .ToList();
                Console.Error.WriteLine($"[DISPATCH-DIAG] SubjectId matches from metadata: {matchedIds.Count}");
                if (matchedIds.Count > 0)
                {
                    Console.Error.WriteLine($"[DISPATCH-DIAG] First match: {matchedIds[0]}");
                }
                else
                {
                    // Show one metadata SubjectId to compare
                    var sampleMetaId = _subjectMethodSubjectIds.First();
                    Console.Error.WriteLine($"[DISPATCH-DIAG] No matches. Sample metadata SubjectId: {sampleMetaId}");
                    Console.Error.WriteLine($"[DISPATCH-DIAG] Sample AOT Core IR SubjectId: {methods[0].SubjectId}");
                }
            }
        }

        var model = new ScriptObject
        {
            ["is_jit_mode"] = _codegenMode.HasFlag(CodegenMode.Jit),
            ["methods"] = methodEntries,
            ["methods_count"] = methods.Count,
            ["default_string_id"] = (long)defaultStringId,
            ["subject_entries"] = subjectEntries,
        };

        var template = NativeAotTemplateCatalog.GetDispatchEntryCodeTemplate();
        var cppCode = ScribanTemplateRenderer.RenderTemplate(template, model);

        // Build and store methods-manifest.json for Python verification dispatch generator.
        _manifestJson = BuildMethodsManifestJson(methods);

        return cppCode;
    }



    private string BuildMethodsManifestJson(IReadOnlyList<AotCoreIrMethodArtifact> methods)
    {
        ulong defaultStringId = _stringIdMapping is { Count: > 0 }
            ? _stringIdMapping.First().Value
            : 0UL;

        var manifestMethods = new List<Dictionary<string, object>>(methods.Count);
        for (int i = 0; i < methods.Count; i++)
        {
            var method = methods[i];
            var ac = method.ParameterCount;
            var isInstance = !method.IsStatic;

            var paramsList = new List<Dictionary<string, object>>(ac + (isInstance ? 1 : 0));
            if (isInstance)
            {
                paramsList.Add(new Dictionary<string, object>
                {
                    ["isString"] = false,
                    ["isThis"] = true,
                });
            }
            for (int j = 0; j < ac; j++)
            {
                var abi = j < method.ParameterAbis.Count ? method.ParameterAbis[j] : null;
                var isString = abi != null && IsStringParameterSlot(abi);
                paramsList.Add(new Dictionary<string, object>
                {
                    ["isString"] = isString,
                    ["isThis"] = false,
                });
            }

            int subjectIdx = -1;
            if (IsSubjectMethod(method.SubjectId))
                subjectIdx = ExtractSubjectIndex(method.SubjectId);
            var manifestMethod = new Dictionary<string, object>
            {
                ["index"] = i,
                ["nativeSymbol"] = method.NativeSymbol,
                ["subjectId"] = method.SubjectId,
                ["paramCount"] = ac + (isInstance ? 1 : 0),
                ["params"] = paramsList,
                ["isInstance"] = isInstance,
            };
            if (subjectIdx >= 0)
                manifestMethod["subjectIndex"] = subjectIdx;

            manifestMethods.Add(manifestMethod);
        }

        var manifest = new Dictionary<string, object>
        {
            ["schemaVersion"] = 2,
            ["assemblyName"] = _assemblyName,
            ["methodCount"] = methods.Count,
            ["defaultStringId"] = (long)defaultStringId,
            ["methods"] = manifestMethods,
        };

        return System.Text.Json.JsonSerializer.Serialize(manifest, new System.Text.Json.JsonSerializerOptions
        {
            WriteIndented = true,
            PropertyNamingPolicy = System.Text.Json.JsonNamingPolicy.CamelCase,
        });
    }



    /// <summary>
    /// Check whether a method is a subject method.
    /// When _subjectMethodSubjectIds is set (from --subject-methods), uses set-based
    /// lookup first (most specific); falls back to naming conventions for chunks
    /// where the constructed SubjectIds don't match AOT IR naming (e.g., when
    /// AutoTestGenerator's generatedMethodId differs from actual C# method names).
    /// </summary>
    private bool IsSubjectMethod(string subjectId)
    {
        // 0. Always exclude Benchmark_ wrappers — they call instance methods on
        //    null `this` without NullReferenceException handling, causing native
        //    crashes (STATUS_ACCESS_VIOLATION or STATUS_STACK_BUFFER_OVERRUN) in
        //    AOT dispatch.  The corresponding [Fact] variant wraps the same call
        //    in Assert.Throws<NullReferenceException> and is the correct
        //    correctness-verification entry point.
        if (subjectId.Contains("::Benchmark_", StringComparison.Ordinal))
            return false;

        // 1. When --subject-methods is provided, ONLY exact matches are valid.
        //    The CombinedSubjects prefix fallback below would also capture SDK
        //    infrastructure methods (Assert.AreEqual, ResultToLong, etc.) from
        //    the CombinedSubjects assembly, inflating kSubjectEntryCount to
        //    4000-5000 instead of the actual 200-300 Fact wrappers.
        if (_subjectMethodSubjectIds != null)
            return _subjectMethodSubjectIds.Contains(subjectId);

        // 2. CombinedSubjects methods: any method from the subject-assembly DLL
        //    (assembly name "CombinedSubjects") is a subject method by definition.
        //    Exclude constructors (.ctor, .cctor) and closures (<...>) — these are
        //    infrastructure methods generated by the compiler, not subject wrappers.
        if (subjectId.StartsWith("CombinedSubjects/", StringComparison.Ordinal))
        {
            if (subjectId.Contains(".ctor") || subjectId.Contains('<'))
                return false;
            return true;
        }

        // 3. Match ::Subject_N pattern (numbered subject wrappers).
        const string subjectPrefix = "::Subject_";
        int idx = subjectId.IndexOf(subjectPrefix, StringComparison.Ordinal);
        if (idx < 0) return false;
        int start = idx + subjectPrefix.Length;
        if (start >= subjectId.Length) return false;
        int end = start;
        while (end < subjectId.Length && char.IsAsciiDigit(subjectId[end]))
            end++;
        return end > start;
    }



    /// <summary>
    /// Extract subject index from a SubjectId string.
    /// Subject methods have SubjectIds ending like "::CustomEntrySubject_N:..." or "::Subject_N:...".
    /// Returns -1 if the SubjectId is not a subject method.
    /// </summary>
    private static int ExtractSubjectIndex(string subjectId)
    {
        const string customPrefix = "::CustomEntrySubject_";
        const string subjectPrefix = "::Subject_";

        // Try CustomEntrySubject_N first.
        int idx = subjectId.IndexOf(customPrefix, StringComparison.Ordinal);
        int prefixLen = customPrefix.Length;
        if (idx < 0)
        {
            idx = subjectId.IndexOf(subjectPrefix, StringComparison.Ordinal);
            prefixLen = subjectPrefix.Length;
        }
        if (idx < 0) return -1;

        int start = idx + prefixLen;
        if (start >= subjectId.Length) return -1;
        int end = start;
        while (end < subjectId.Length && char.IsAsciiDigit(subjectId[end]))
            end++;
        if (end == start) return -1;
        return int.Parse(subjectId.Substring(start, end - start), CultureInfo.InvariantCulture);
    }



    /// <summary>
    /// Strip the trailing _N variant suffix from a SubjectId before the return type.
    /// CombinedSubjects AutoGenerated methods and ::Subject_N methods both have
    /// _0/_1/_2 variant suffixes (e.g., "ForEachAsync_40_..._0:System.Int64()").
    /// Normalizing to ":System.Int64()" lets _0 and _1 variants of the same subject
    /// match in the subjectIdsWithZero dedup set.
    /// </summary>
    private static string StripSubjectVariantSuffix(string subjectId)
    {
        int lastColon = subjectId.LastIndexOf(':');
        if (lastColon > 1 && subjectId[lastColon - 2] == '_' && char.IsAsciiDigit(subjectId[lastColon - 1]))
        {
            return subjectId.Substring(0, lastColon - 2) + subjectId.Substring(lastColon);
        }
        return subjectId;
    }



    // ── Step 2: CodeRegistrationV0 + MetadataRegistrationV0 + CodegenRegistrationOptionsV0 ──
    // Emitted as extern "C" symbols for RegisterCodegen + BootstrapRuntime path.
    // References the generic registration arrays emitted by EmitGenericRegistration.

    private string EmitCodeRegistrationStructs(
    IReadOnlyList<AotCoreIrMethodArtifact> methods,
    MetadataRegistrationArtifact metadataRegistration,
    bool hasGcSlotMapSection = false)
    {
        _ = metadataRegistration; // unused — kept to avoid changing callers
        if (methods.Count == 0) return string.Empty;

        var model = new ScriptObject
        {
            ["methods"] = methods
                .Select(m => new ScriptObject { ["native_symbol"] = m.NativeSymbol })
                .ToArray(),
            ["methods_count"] = methods.Count,
            ["reverse_pinvoke_count"] = _reversePInvokeEntries.Count,
            ["reverse_pinvoke_entries"] = _reversePInvokeEntries.Count > 0
                ? _reversePInvokeEntries
                    .Select(e => new ScriptObject { ["native_symbol"] = e.NativeSymbol })
                    .ToArray()
                : Array.Empty<ScriptObject>(),
            ["assembly_name"] = EscapeCppStringLiteral(_assemblyName),
            ["has_gc_slot_map_section"] = hasGcSlotMapSection,
        };

        // ── VTable descriptors for BootstrapRuntime TypeVTable registration ──
        // Always set (even when empty) to avoid Scriban "function not found" error.
        if (_vtableDescriptors is { Count: > 0 })
        {
            model["vtable_descriptors"] = _vtableDescriptors
                .Select(d => new ScriptObject
                {
                    ["stable_id"] = "CHAOS_IL2CPP_UINT64_C(0x" + d.StableId.ToString("X16") + ")",
                    ["type_token_literal"] = d.TypeTokenLiteral,
                    ["base_token_literal"] = d.BaseTokenLiteral,
                    ["slot_count"] = d.Slots.Length,
                    ["slots_symbol"] = "kSlots_" + d.SanitizedId,
                    ["vtable_array_symbol"] = d.VTableArraySymbol,
                    ["vtable_length"] = d.VTableLength,
                    ["type_shape"] = d.TypeShape,
                    ["iface_map_symbol"] = d.IfaceMapSymbol ?? "nullptr",
                    ["iface_count"] = d.IfaceCount,
                })
                .ToArray();
            model["vtable_descriptor_count"] = _vtableDescriptors.Count;
        }
        else
        {
            model["vtable_descriptors"] = Array.Empty<ScriptObject>();
            model["vtable_descriptor_count"] = 0;
        }

        return ScribanTemplateRenderer.RenderTemplate(
            NativeAotTemplateCatalog.GetCodeRegistrationTemplate(), model);
    }



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
