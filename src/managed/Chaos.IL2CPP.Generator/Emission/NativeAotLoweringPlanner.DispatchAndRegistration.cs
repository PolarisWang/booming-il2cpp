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
            ["assembly_suffix"] = _assemblySuffix ,
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



    /// <summary>
    /// Generate the C++ header for pre-computed enum metadata tables.
    /// Collects enum type data from ALL resolved assemblies (not just the entry
    /// assembly) so that SPC enum types (DayOfWeek, BindingFlags, etc.) are
    /// included alongside the entry assembly's own enum types.
    /// Returns empty string if no enum types or field data are available.
    /// </summary>



    /// <summary>
    /// Read assembly PE metadata and collect enum type flags, subjectIds,
    /// AND field name/value entries for each enum type.
    /// Populates the provided lists, skipping types already in seenSubjectIds.
    /// </summary>



    /// <summary>
    /// Fallback: scan PE metadata of all closure assemblies to collect enum field
    /// entries (name + constant value) when _reflectionMemberSupport.FieldEntries
    /// is empty (e.g. during foundation-dll codegen for stub-based families).
    /// </summary>



    /// <summary>
    /// Read the constant value from a field definition's Constant metadata.
    /// Returns null if the field has no constant or the type is unsupported.
    /// </summary>


    /// <summary>
    /// Collect enum type subject IDs from reflection member support data (preferred)
    /// or fall back to scanning PE metadata of closure assemblies.
    /// </summary>



    /// <summary>
    /// Build a type → {value → name} map for all enum types in the closure.
    /// Prefers FieldEntries from reflection member support data; falls back
    /// to scanning PE metadata of closure assemblies.
    /// Used by S2 (BoxToString switch) and other value→name lookups.
    /// </summary>



    /// <summary>
    /// Scan a single assembly's PE metadata for enum type definitions and
    /// add their subject IDs to the provided set.
    /// </summary>

}
