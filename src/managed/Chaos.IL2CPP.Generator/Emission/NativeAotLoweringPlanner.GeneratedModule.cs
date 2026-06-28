using System.Collections.Generic;
using System.Linq;
using Chaos.IL2CPP.Contracts;
using Scriban.Runtime;

namespace Chaos.IL2CPP.Generator;

public sealed partial class NativeAotLoweringPlanner
{
    /// <summary>
    /// Build the A1 typed dispatch table header (chaos_generated_module.h).
    /// Renders the NativeAot.GeneratedModule.h.scriban template with type group
    /// data from the methods for lowering.
    /// </summary>
	internal string BuildGeneratedModuleHeader(IReadOnlyList<AotCoreIrMethodArtifact> methodsForLowering, string objectModel, string assemblySuffix, IReadOnlySet<string>? extraValuetypes = null)
    {
        return ScribanTemplateRenderer.RenderTemplate(
            NativeAotTemplateCatalog.GetGeneratedModuleHeaderTemplate(),
            BuildGeneratedModuleModel(methodsForLowering, extraValuetypes));
    }

    /// <summary>
    /// Build the A1 dispatch wiring source (chaos_generated_module.cpp).
    /// Renders the NativeAot.GeneratedModule.cpp.scriban template with type group
    /// data and extern symbol declarations.
    /// </summary>
	internal string BuildGeneratedModuleSource(IReadOnlyList<AotCoreIrMethodArtifact> methodsForLowering, string objectModel, string assemblySuffix, IReadOnlySet<string>? extraValuetypes = null)
    {
        return ScribanTemplateRenderer.RenderTemplate(
            NativeAotTemplateCatalog.GetGeneratedModuleSourceTemplate(),
            BuildGeneratedModuleModel(methodsForLowering, extraValuetypes));
    }

    private ScriptObject BuildGeneratedModuleModel(IReadOnlyList<AotCoreIrMethodArtifact> methodsForLowering, IReadOnlySet<string>? extraValuetypes = null)
    {
        if (methodsForLowering.Count == 0)
        {
            return new ScriptObject
            {
                ["type_groups"] = System.Array.Empty<ScriptObject>(),
                ["k_aot_method_count_type"] = "CHAOS_IL2CPP_INT32",
                ["k_aot_method_count_value"] = 0,
            };
        }

        // Group methods by declaring type subject ID.
        var typeGroups = methodsForLowering
            .Where(m => !string.IsNullOrWhiteSpace(m.Identity.DeclaringTypeSubjectId))
            .GroupBy(m => m.Identity.DeclaringTypeSubjectId!, StringComparer.Ordinal)
            .OrderBy(g => g.Key, StringComparer.Ordinal)
            .ToList();

        // Compute total method count AFTER deduplication (used by k_aot_method_count_value
        // and kFunctionsFlat array sizing).  Dedup is applied per-type-group below, so we
        // pre-compute the global count here by the same rule.
        int totalDedupedCount = 0;
        {
            var globalDedupSet = new HashSet<string>(StringComparer.Ordinal);
            foreach (var m in methodsForLowering)
            {
                if (!string.IsNullOrWhiteSpace(m.Identity?.DeclaringTypeSubjectId) &&
                    globalDedupSet.Add(m.NativeSymbol))
                    totalDedupedCount++;
            }
        }

        var typeGroupModels = new ScriptObject[typeGroups.Count];
        for (int gi = 0; gi < typeGroups.Count; gi++)
        {
            var group = typeGroups[gi];
            var safeName = SanitizeCppIdentifier(group.Key);
            var safeNameLower = SanitizeCppIdentifierLowerFirst(group.Key);

            var methods = group
                .OrderBy(m => m.SubjectId, StringComparer.Ordinal)
                .ToArray();

            // Deduplicate by native symbol: when two managed methods map to the
            // same extern "C" symbol (e.g. overloads whose ABI slots collapse to
            // identical parameter types), keep only the first occurrence.  Duplicates
            // cause C2733 (extern "C" cannot be overloaded) and C2440 (function
            // pointer type mismatch in the typed s_functions struct).
            var emittedNativeSymbols = new HashSet<string>(StringComparer.Ordinal);
            methods = methods.Where(m => emittedNativeSymbols.Add(m.NativeSymbol)).ToArray();

            var methodModels = new ScriptObject[methods.Length];
            var methodNameCounts = new Dictionary<string, int>(StringComparer.Ordinal);
            var usedMethodNames = new HashSet<string>(StringComparer.Ordinal);
            for (int mi = 0; mi < methods.Length; mi++)
            {
                var method = methods[mi];
                var paramAbis = GetMethodAbiParameterSlots(method);
                var paramModels = new ScriptObject[paramAbis.Count];
                for (int pi = 0; pi < paramAbis.Count; pi++)
                {
                    paramModels[pi] = new ScriptObject
                    {
                        ["type"] = MapAbiSlotParameterType(paramAbis[pi]),
                        ["name"] = $"arg_{pi}",
                    };
                }

                var rawMethodName = GetMethodName(method.SubjectId);
                // Strip generic arity suffix (`1, `2, etc.) — the backtick-number
                // suffix is a CLR metadata convention, not part of the method identity
                // for C++ naming.  SanitizeCppIdentifier would turn `1 into _1,
                // which collides with the method-name uniquification counter.
                var backtickIdx = rawMethodName.IndexOf('`');
                if (backtickIdx > 0)
                    rawMethodName = rawMethodName[..backtickIdx];
                // C++ member names cannot start with '.' (e.g. ".ctor", ".cctor")
                // and must not contain special characters (e.g. '<', '>', '$', '|'
                // from compiler-generated names like "<<Main>$>g__Add|0_0").
                if (rawMethodName.Length > 0 && rawMethodName[0] == '.')
                    rawMethodName = rawMethodName.Substring(1);
                var safeMethodName = SanitizeCppIdentifier(rawMethodName);

                // Uniquify method names: when two managed overloads collapse to the
                // same C++ identifier at the ABI level, append _1, _2, ... suffix.
                if (methodNameCounts.TryGetValue(safeMethodName, out var methodCount))
                {
                    var baseName = safeMethodName;
                    safeMethodName = baseName + "_" + methodCount;
                    methodNameCounts[baseName] = methodCount + 1;
                }
                else
                {
                    methodNameCounts[safeMethodName] = 1;
                }

                // Guard against collisions between suffixed names and raw method
                // names (e.g. raw "AreEqual_14" collides with the 15th AreEqual
                // overload's suffixed "AreEqual_14").
                if (!usedMethodNames.Add(safeMethodName))
                {
                    var collisionBase = safeMethodName;
                    int disambiguator = 0;
                    do
                    {
                        disambiguator++;
                        safeMethodName = collisionBase + "_" + disambiguator;
                    } while (!usedMethodNames.Add(safeMethodName));
                }

                methodModels[mi] = new ScriptObject
                {
                    ["return_type"] = MapAbiSlotReturnType(method.ReturnAbi),
                    ["method_name"] = safeMethodName,
                    ["native_symbol"] = method.NativeSymbol,
                    ["param_count"] = paramAbis.Count,
                    ["params"] = paramModels,
                };
            }

            typeGroupModels[gi] = new ScriptObject
            {
                ["safe_name"] = safeName,
                ["safe_name_lower"] = safeNameLower,
                ["method_count"] = methods.Length,
                ["methods"] = methodModels,
            };
        }

        // Detect duplicate safe_names across type groups and uniquify by
        // appending the assembly prefix.  The converter may emit methods for
        // the same C# type from different assemblies, and GetTypeDisplayName()
        // strips the assembly prefix, causing C2011 struct redefinition errors.
        var safeNameGroups = new Dictionary<string, List<int>>(StringComparer.Ordinal);
        for (int gi = 0; gi < typeGroupModels.Length; gi++)
        {
            var name = (string)typeGroupModels[gi]["safe_name"];
            if (!safeNameGroups.TryGetValue(name, out var list))
            {
                list = new List<int>();
                safeNameGroups[name] = list;
            }
            list.Add(gi);
        }
        foreach (var kvp in safeNameGroups)
        {
            if (kvp.Value.Count <= 1) continue;
            foreach (var gi in kvp.Value)
            {
                var groupKey = typeGroups[gi].Key;
                var slashIdx = groupKey.IndexOf('/');
                var assemblyPart = slashIdx > 0 ? groupKey[..slashIdx] : groupKey;
                var assemblySafe = SanitizeCppIdentifier(assemblyPart);
                var oldName = (string)typeGroupModels[gi]["safe_name"];
                var newName = oldName + "_" + assemblySafe;
                typeGroupModels[gi]["safe_name"] = newName;
                typeGroupModels[gi]["safe_name_lower"] =
                    SanitizeCppIdentifierLowerFirst(newName);
            }
        }

        // ── Value type typedefs ──
        // The generated module header needs chaos_valuetype_* to be complete
        // types (typedef CHAOS_IL2CPP_INT32) for function pointer signatures
        // that return or accept value types by value.  The shared header has
        // these typedefs but also carries Python-added extern "C" declarations
        // that conflict with chaos_generated_module.cpp, so we emit the
        // typedefs directly here.
        var valueTypeTypedefs = "";
        _emittedValueTypeSubjectIds ??= new HashSet<string>(StringComparer.Ordinal);
        // Post-scan the lowered method artifacts for closure assembly value
        // type ABI slots.  The ObjectModelEmission phase only scans types in
            // the AOT IR's type metadata, which may not include value types from
            // closure assemblies (e.g., System.Data.ConnectionState,
            // System.Data.IsolationLevel).  Without these, the generated header
            // lacks chaos_valuetype_* typedefs, causing C2061.
            //
            // Use methodsForLowering (parameter, lowered artifacts with proper
            // CarrierKindCode=ValueTypeByValue and TypeSubjectId set) rather than
            // _methodsBySubjectId (original AOT IR JSON with no TypeSubjectId for
            // value types).
            for (int mi = 0; mi < methodsForLowering.Count; mi++)
            {
                var m = methodsForLowering[mi];
                // ValueTypeByValue ABI slots — exact value types
                if (m.ReturnAbi.CarrierKindCode == AotCoreIrAbiCarrierKind.ValueTypeByValue &&
                    !string.IsNullOrEmpty(m.ReturnAbi.TypeSubjectId))
                    _emittedValueTypeSubjectIds.Add(m.ReturnAbi.TypeSubjectId);
                if (m.ParameterAbis != null)
                {
                    foreach (var abi in m.ParameterAbis)
                    {
                        if (abi.CarrierKindCode == AotCoreIrAbiCarrierKind.ValueTypeByValue &&
                            !string.IsNullOrEmpty(abi.TypeSubjectId))
                            _emittedValueTypeSubjectIds.Add(abi.TypeSubjectId);
                    }
                }
                // NativeInt ABI slots with external type SubjectIds — these are external
                // value types (e.g. System.Data.CommandBehavior) that could not be resolved
                // at AOT IR lowering time (managedType == null).  The TypeSubjectId was
                // populated by BuildExternalTypeSubjectId in ResolveAbiSlot so that
                // GeneratedModule emits the necessary chaos_valuetype_* typedef.
                if (m.ReturnAbi.CarrierKindCode == AotCoreIrAbiCarrierKind.NativeInt &&
                    !string.IsNullOrEmpty(m.ReturnAbi.TypeSubjectId) &&
                    !m.ReturnAbi.TypeSubjectId.StartsWith("System.Private.CoreLib/", StringComparison.Ordinal))
                    _emittedValueTypeSubjectIds.Add(m.ReturnAbi.TypeSubjectId);
                if (m.ParameterAbis != null)
                {
                    foreach (var abi in m.ParameterAbis)
                    {
                        if (abi.CarrierKindCode == AotCoreIrAbiCarrierKind.NativeInt &&
                            !string.IsNullOrEmpty(abi.TypeSubjectId) &&
                            !abi.TypeSubjectId.StartsWith("System.Private.CoreLib/", StringComparison.Ordinal))
                            _emittedValueTypeSubjectIds.Add(abi.TypeSubjectId);
                    }
                }
            }

            // Include extra chaos_valuetype_ typedefs from method declarations
            // (extern "C" strings).  External value types used as ABI parameters
            // have CarrierKindCode=ValueTypeByValue with TypeSubjectId set from
            // the AOT IR, so FormatMethodDeclaration outputs chaos_valuetype_X
            // in the parameter list.  Scan _methodDeclarations for these names.
            var extraValuetypeNames = new HashSet<string>(StringComparer.Ordinal);
            if (_methodDeclarations != null)
            {
                foreach (var decl in _methodDeclarations)
                {
                    int idx = 0;
                    while ((idx = decl.IndexOf("chaos_valuetype_", idx, StringComparison.Ordinal)) >= 0)
                    {
                        int start = idx;
                        int end = decl.IndexOf(' ', idx + 16);
                        if (end < 0) end = decl.Length;
                        extraValuetypeNames.Add(decl.Substring(idx, end - idx));
                        idx = end;
                    }
                }
            }
            if (extraValuetypes != null)
                extraValuetypeNames.UnionWith(extraValuetypes);

            var vtBuilder = new System.Text.StringBuilder();
            vtBuilder.AppendLine("// chaos_valuetype_* typedefs (opaque 32-bit managed value types)");
            foreach (var typeId in _emittedValueTypeSubjectIds.OrderBy(id => id, StringComparer.Ordinal))
            {
                vtBuilder.Append("typedef CHAOS_IL2CPP_INT32 ");
                vtBuilder.Append(GetNativeValueTypeSymbol(typeId));
                vtBuilder.AppendLine(";");
            }
            foreach (var vtName in extraValuetypeNames.OrderBy(n => n, StringComparer.Ordinal))
            {
                if (_emittedValueTypeSubjectIds.Any(id => GetNativeValueTypeSymbol(id) == vtName))
                    continue; // already emitted from ABI slot scan
                vtBuilder.Append("typedef CHAOS_IL2CPP_INT32 ");
                vtBuilder.Append(vtName);
                vtBuilder.AppendLine(";");
            }
            vtBuilder.AppendLine();
            valueTypeTypedefs = vtBuilder.ToString();
        return new ScriptObject
        {
            ["type_groups"] = typeGroupModels,
            ["k_aot_method_count_type"] = "CHAOS_IL2CPP_INT32",
            ["k_aot_method_count_value"] = totalDedupedCount,
            ["value_type_typedefs"] = valueTypeTypedefs,
        };
    }

    private static string SanitizeCppIdentifierLowerFirst(string name)
    {
        var sanitized = SanitizeCppIdentifier(name);
        if (sanitized.Length > 0 && sanitized[0] >= 'A' && sanitized[0] <= 'Z')
        {
            var chars = sanitized.ToCharArray();
            chars[0] = char.ToLowerInvariant(chars[0]);
            return new string(chars);
        }
        return sanitized;
    }
}
