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
            var typeDisplayName = GetTypeDisplayName(group.Key);
            var safeName = SanitizeCppIdentifier(typeDisplayName);
            var safeNameLower = SanitizeCppIdentifierLowerFirst(typeDisplayName);

            var methods = group
                .OrderBy(m => m.SubjectId, StringComparer.Ordinal)
                .ToArray();

            var methodModels = new ScriptObject[methods.Length];
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
                // C++ member names cannot start with '.' (e.g. ".ctor", ".cctor").
                var safeMethodName = rawMethodName.Length > 0 && rawMethodName[0] == '.'
                    ? rawMethodName.Substring(1)
                    : rawMethodName;

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

        return new ScriptObject
        {
            ["type_groups"] = typeGroupModels,
            ["k_aot_method_count_type"] = "const int",
            ["k_aot_method_count_value"] = methodsForLowering.Count,
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
