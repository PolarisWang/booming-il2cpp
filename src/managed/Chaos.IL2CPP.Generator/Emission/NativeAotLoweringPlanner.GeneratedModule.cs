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
    internal string BuildGeneratedModuleHeader(IReadOnlyList<AotCoreIrMethodArtifact> methodsForLowering)
    {
        return ScribanTemplateRenderer.RenderTemplate(
            NativeAotTemplateCatalog.GetGeneratedModuleHeaderTemplate(),
            BuildGeneratedModuleModel(methodsForLowering));
    }

    /// <summary>
    /// Build the A1 dispatch wiring source (chaos_generated_module.cpp).
    /// Renders the NativeAot.GeneratedModule.cpp.scriban template with type group
    /// data and extern symbol declarations.
    /// </summary>
    internal string BuildGeneratedModuleSource(IReadOnlyList<AotCoreIrMethodArtifact> methodsForLowering)
    {
        return ScribanTemplateRenderer.RenderTemplate(
            NativeAotTemplateCatalog.GetGeneratedModuleSourceTemplate(),
            BuildGeneratedModuleModel(methodsForLowering));
    }

    private ScriptObject BuildGeneratedModuleModel(IReadOnlyList<AotCoreIrMethodArtifact> methodsForLowering)
    {
        if (methodsForLowering.Count == 0)
        {
            return new ScriptObject
            {
                ["type_groups"] = System.Array.Empty<ScriptObject>(),
                ["k_aot_method_count_type"] = "const int",
                ["k_aot_method_count_value"] = 0,
            };
        }

        // Group methods by declaring type subject ID.
        var typeGroups = methodsForLowering
            .Where(m => !string.IsNullOrWhiteSpace(m.Identity.DeclaringTypeSubjectId))
            .GroupBy(m => m.Identity.DeclaringTypeSubjectId!, StringComparer.Ordinal)
            .OrderBy(g => g.Key, StringComparer.Ordinal)
            .ToList();

        var typeGroupModels = new ScriptObject[typeGroups.Count];
        for (int gi = 0; gi < typeGroups.Count; gi++)
        {
            var group = typeGroups[gi];
            var safeName = SanitizeCppIdentifier(group.Key);
            var safeNameLower = SanitizeCppIdentifierLowerFirst(group.Key);

            var methods = group
                .OrderBy(m => m.SubjectId, StringComparer.Ordinal)
                .ToArray();

            var methodModels = new ScriptObject[methods.Length];
            var methodNameCounts = new Dictionary<string, int>(StringComparer.Ordinal);
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
        if (_emittedValueTypeSubjectIds is { Count: > 0 })
        {
            var vtBuilder = new System.Text.StringBuilder();
            vtBuilder.AppendLine("// chaos_valuetype_* typedefs (opaque 32-bit managed value types)");
            foreach (var typeId in _emittedValueTypeSubjectIds.OrderBy(id => id, StringComparer.Ordinal))
            {
                vtBuilder.Append("typedef CHAOS_IL2CPP_INT32 ");
                vtBuilder.Append(GetNativeValueTypeSymbol(typeId));
                vtBuilder.AppendLine(";");
            }
            vtBuilder.AppendLine();
            valueTypeTypedefs = vtBuilder.ToString();
        }

        return new ScriptObject
        {
            ["type_groups"] = typeGroupModels,
            ["k_aot_method_count_type"] = "const int",
            ["k_aot_method_count_value"] = methodsForLowering.Count,
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
