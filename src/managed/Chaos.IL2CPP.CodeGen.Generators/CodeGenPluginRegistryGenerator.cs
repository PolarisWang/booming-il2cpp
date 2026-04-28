using System;
using System.Collections.Generic;
using System.Collections.Immutable;
using System.Linq;
using System.Text;
using Microsoft.CodeAnalysis;
using Microsoft.CodeAnalysis.CSharp.Syntax;
using Microsoft.CodeAnalysis.Text;

namespace Chaos.IL2CPP.CodeGen.Generators;

[Generator]
public sealed class CodeGenPluginRegistryGenerator : IIncrementalGenerator
{
    private const string FamilyAttributeMetadataName = "Chaos.IL2CPP.CodeGen.RuntimeSkeletonFamilyPluginAttribute";
    private const string LoweringAttributeMetadataName = "Chaos.IL2CPP.CodeGen.LoweringPlanPluginAttribute";
    private const string TemplateBundleAttributeMetadataName = "Chaos.IL2CPP.CodeGen.CodegenTemplateBundlePluginAttribute";
    private const string RuntimeProviderAttributeMetadataName = "Chaos.IL2CPP.CodeGen.RuntimeProviderPluginAttribute";

    public void Initialize(IncrementalGeneratorInitializationContext context)
    {
        context.RegisterPostInitializationOutput(static postContext =>
        {
            postContext.AddSource(
                "CodeGenPluginRegistryGenerator.Marker.g.cs",
                SourceText.From(
                    """
namespace Chaos.IL2CPP.CodeGen;

internal static class CodeGenPluginRegistryGeneratorMarker
{
    public const string Value = "active";
}
""",
                    Encoding.UTF8));
        });

        var familyMethods = context.SyntaxProvider.ForAttributeWithMetadataName(
                FamilyAttributeMetadataName,
                static (node, _) => node is MethodDeclarationSyntax,
                static (ctx, _) => MethodPluginDefinition.TryCreate(ctx, PluginCategory.Family))
            .Where(static definition => definition is not null)
            .Select(static (definition, _) => definition!);

        var loweringMethods = context.SyntaxProvider.ForAttributeWithMetadataName(
                LoweringAttributeMetadataName,
                static (node, _) => node is MethodDeclarationSyntax,
                static (ctx, _) => MethodPluginDefinition.TryCreate(ctx, PluginCategory.LoweringPlan))
            .Where(static definition => definition is not null)
            .Select(static (definition, _) => definition!);

        var templateBundleTypes = context.SyntaxProvider.ForAttributeWithMetadataName(
                TemplateBundleAttributeMetadataName,
                static (node, _) => node is ClassDeclarationSyntax,
                static (ctx, _) => TypePluginDefinition.TryCreate(ctx, PluginCategory.TemplateBundle))
            .Where(static definition => definition is not null)
            .Select(static (definition, _) => definition!);

        var runtimeProviderTypes = context.SyntaxProvider.ForAttributeWithMetadataName(
                RuntimeProviderAttributeMetadataName,
                static (node, _) => node is ClassDeclarationSyntax,
                static (ctx, _) => TypePluginDefinition.TryCreate(ctx, PluginCategory.RuntimeProvider))
            .Where(static definition => definition is not null)
            .Select(static (definition, _) => definition!);

        context.RegisterSourceOutput(familyMethods.Collect(), EmitFamilyRegistries);
        context.RegisterSourceOutput(loweringMethods.Collect(), EmitLoweringPlanRegistries);
        context.RegisterSourceOutput(templateBundleTypes.Collect(), EmitTemplateBundleRegistries);
        context.RegisterSourceOutput(runtimeProviderTypes.Collect(), EmitRuntimeProviderRegistries);
    }

    private static void EmitFamilyRegistries(SourceProductionContext context, ImmutableArray<MethodPluginDefinition> definitions)
    {
        EmitEmitterFamilyRegistry(context, definitions, "NativeReference");
        EmitEmitterFamilyRegistry(context, definitions, "NativeAot");
    }

    private static void EmitEmitterFamilyRegistry(SourceProductionContext context, ImmutableArray<MethodPluginDefinition> definitions, string productLineName)
    {
        var lineDefinitions = definitions
            .Where(definition => definition.ProductLineName == productLineName)
            .OrderBy(definition => definition.Order)
            .ThenBy(definition => definition.Id, StringComparer.Ordinal)
            .ToArray();
        if (lineDefinitions.Length == 0)
        {
            return;
        }

        var targetClass = productLineName == "NativeReference"
            ? "NativeReferenceProofEmitter"
            : "NativeAotEmitter";
        var registryClass = $"{productLineName}RuntimeSkeletonFamilyRegistry";
        var metadataArrayName = $"{productLineName}RuntimeSkeletonFamilyPlugins";
        var handlerArrayName = productLineName == "NativeReference"
            ? "RuntimeSkeletonFamilyHandlers"
            : "NativeAotFamilyHandlers";

        var source = $$"""
namespace Chaos.IL2CPP.CodeGen;

internal static partial class {{registryClass}}
{
    public static readonly GeneratedRuntimeSkeletonFamilyPluginMetadata[] Plugins =
    [
{{BuildFamilyMetadataEntries(lineDefinitions)}}
    ];
}

public sealed partial class {{targetClass}}
{
    private static readonly GeneratedRuntimeSkeletonFamilyPluginMetadata[] {{metadataArrayName}} = {{registryClass}}.Plugins;

    private static readonly RuntimeSkeletonFamilyHandler[] {{handlerArrayName}} =
    [
{{BuildMethodReferenceEntries(lineDefinitions)}}
    ];
}
""";
        context.AddSource($"{registryClass}.g.cs", SourceText.From(source, Encoding.UTF8));
    }

    private static void EmitLoweringPlanRegistries(SourceProductionContext context, ImmutableArray<MethodPluginDefinition> definitions)
    {
        var lineDefinitions = definitions
            .Where(definition => definition.ProductLineName == "NativeReference")
            .OrderBy(definition => definition.Order)
            .ThenBy(definition => definition.Id, StringComparer.Ordinal)
            .ToArray();

        var source = $$"""
namespace Chaos.IL2CPP.CodeGen;

internal static partial class NativeReferenceLoweringPlanRegistry
{
    public static readonly GeneratedLoweringPlanPluginMetadata[] Plugins =
    [
{{BuildLoweringMetadataEntries(lineDefinitions)}}
    ];
}

public sealed partial class NativeReferenceLoweringPlanner
{
    private static readonly GeneratedLoweringPlanPluginMetadata[] GeneratedLoweringPlanPlugins = NativeReferenceLoweringPlanRegistry.Plugins;

    private static readonly NativeReferenceLoweringPlanPlugin[] NativeReferenceLoweringPlanPlugins =
    [
{{BuildMethodReferenceEntries(lineDefinitions)}}
    ];
}
""";
        context.AddSource("NativeReferenceLoweringPlanRegistry.g.cs", SourceText.From(source, Encoding.UTF8));
    }

    private static void EmitTemplateBundleRegistries(SourceProductionContext context, ImmutableArray<TypePluginDefinition> definitions)
    {
        EmitTypeRegistry(
            context,
            definitions,
            "NativeReference",
            "NativeReferenceTemplateBundleRegistry",
            "ICodegenTemplateBundlePlugin",
            "GeneratedTemplateBundlePluginMetadata",
            static definition => $$"""new GeneratedTemplateBundlePluginMetadata("{{definition.Id}}", CodeGenPluginProductLine.{{definition.ProductLineName}}, "{{definition.DomainOrCapabilityFamily}}", {{definition.Order}}, "{{definition.TypeDisplayName}}")""");

        EmitTypeRegistry(
            context,
            definitions,
            "NativeAot",
            "NativeAotTemplateBundleRegistry",
            "ICodegenTemplateBundlePlugin",
            "GeneratedTemplateBundlePluginMetadata",
            static definition => $$"""new GeneratedTemplateBundlePluginMetadata("{{definition.Id}}", CodeGenPluginProductLine.{{definition.ProductLineName}}, "{{definition.DomainOrCapabilityFamily}}", {{definition.Order}}, "{{definition.TypeDisplayName}}")""");
    }

    private static void EmitRuntimeProviderRegistries(SourceProductionContext context, ImmutableArray<TypePluginDefinition> definitions)
    {
        EmitTypeRegistry(
            context,
            definitions,
            "NativeReference",
            "NativeReferenceRuntimeProviderRegistry",
            "IRuntimeProviderPlugin",
            "GeneratedRuntimeProviderPluginMetadata",
            static definition => $$"""new GeneratedRuntimeProviderPluginMetadata("{{definition.Id}}", CodeGenPluginProductLine.{{definition.ProductLineName}}, "{{definition.DomainOrCapabilityFamily}}", {{definition.Version}}, {{definition.Order}}, "{{definition.TypeDisplayName}}")""");

        EmitTypeRegistry(
            context,
            definitions,
            "NativeAot",
            "NativeAotRuntimeProviderRegistry",
            "IRuntimeProviderPlugin",
            "GeneratedRuntimeProviderPluginMetadata",
            static definition => $$"""new GeneratedRuntimeProviderPluginMetadata("{{definition.Id}}", CodeGenPluginProductLine.{{definition.ProductLineName}}, "{{definition.DomainOrCapabilityFamily}}", {{definition.Version}}, {{definition.Order}}, "{{definition.TypeDisplayName}}")""");
    }

    private static void EmitTypeRegistry(
        SourceProductionContext context,
        ImmutableArray<TypePluginDefinition> definitions,
        string productLineName,
        string registryClassName,
        string interfaceName,
        string metadataTypeName,
        Func<TypePluginDefinition, string> metadataFactory)
    {
        var lineDefinitions = definitions
            .Where(definition => definition.ProductLineName == productLineName)
            .OrderBy(definition => definition.Order)
            .ThenBy(definition => definition.Id, StringComparer.Ordinal)
            .ToArray();

        var metadataEntries = string.Join(
            "\n",
            lineDefinitions.Select(definition => $"        {metadataFactory(definition)},"));
        var pluginEntries = string.Join(
            "\n",
            lineDefinitions.Select(definition => $"        new {definition.TypeQualifiedName}(),"));

        var source = $$"""
namespace Chaos.IL2CPP.CodeGen;

internal static partial class {{registryClassName}}
{
    public static readonly {{metadataTypeName}}[] Metadata =
    [
{{metadataEntries}}
    ];

    public static readonly {{interfaceName}}[] Plugins =
    [
{{pluginEntries}}
    ];
}
""";
        context.AddSource($"{registryClassName}.g.cs", SourceText.From(source, Encoding.UTF8));
    }

    private static string BuildFamilyMetadataEntries(IReadOnlyList<MethodPluginDefinition> definitions)
    {
        return string.Join(
            "\n",
            definitions.Select(definition =>
                $"        new GeneratedRuntimeSkeletonFamilyPluginMetadata(\"{definition.Id}\", CodeGenPluginProductLine.{definition.ProductLineName}, {definition.Order}, RuntimeSkeletonFamilyPluginKind.{definition.KindName}, \"{definition.MethodName}\"),"));
    }

    private static string BuildLoweringMetadataEntries(IReadOnlyList<MethodPluginDefinition> definitions)
    {
        return string.Join(
            "\n",
            definitions.Select(definition =>
                $"        new GeneratedLoweringPlanPluginMetadata(\"{definition.Id}\", CodeGenPluginProductLine.{definition.ProductLineName}, {definition.Order}, \"{definition.MethodName}\"),"));
    }

    private static string BuildMethodReferenceEntries(IReadOnlyList<MethodPluginDefinition> definitions)
    {
        return string.Join(
            "\n",
            definitions.Select(definition => $"        {definition.MethodName},"));
    }

    private enum PluginCategory
    {
        Family,
        LoweringPlan,
        TemplateBundle,
        RuntimeProvider,
    }

    private sealed record MethodPluginDefinition(
        string Id,
        string ProductLineName,
        int Order,
        string MethodName,
        string KindName)
    {
        public static MethodPluginDefinition? TryCreate(GeneratorAttributeSyntaxContext context, PluginCategory category)
        {
            if (context.TargetSymbol is not IMethodSymbol methodSymbol ||
                context.Attributes.Length == 0)
            {
                return null;
            }

            var attributeData = context.Attributes[0];
            if (attributeData.ConstructorArguments.Length < 3)
            {
                return null;
            }

            var id = attributeData.ConstructorArguments[0].Value as string;
            var productLineName = GetEnumMemberName(attributeData.ConstructorArguments[1]);
            var order = attributeData.ConstructorArguments[2].Value as int?;
            var kindName = category == PluginCategory.Family && attributeData.ConstructorArguments.Length > 3
                ? GetEnumMemberName(attributeData.ConstructorArguments[3])
                : string.Empty;
            if (string.IsNullOrWhiteSpace(id) || string.IsNullOrWhiteSpace(productLineName) || order is null)
            {
                return null;
            }

            return new MethodPluginDefinition(
                id,
                productLineName,
                order.Value,
                methodSymbol.Name,
                kindName ?? string.Empty);
        }
    }

    private sealed record TypePluginDefinition(
        string Id,
        string ProductLineName,
        int Order,
        string DomainOrCapabilityFamily,
        int Version,
        string TypeDisplayName,
        string TypeQualifiedName)
    {
        public static TypePluginDefinition? TryCreate(GeneratorAttributeSyntaxContext context, PluginCategory category)
        {
            if (context.TargetSymbol is not INamedTypeSymbol typeSymbol ||
                context.Attributes.Length == 0)
            {
                return null;
            }

            var attributeData = context.Attributes[0];
            if (attributeData.ConstructorArguments.Length < 3)
            {
                return null;
            }

            var id = attributeData.ConstructorArguments[0].Value as string;
            var productLineName = GetEnumMemberName(attributeData.ConstructorArguments[1]);
            var third = attributeData.ConstructorArguments[2].Value?.ToString();
            var version = category == PluginCategory.RuntimeProvider && attributeData.ConstructorArguments.Length > 3
                ? attributeData.ConstructorArguments[3].Value as int? ?? 0
                : 0;
            var order = category == PluginCategory.RuntimeProvider
                ? attributeData.ConstructorArguments.Length > 4 ? attributeData.ConstructorArguments[4].Value as int? ?? 0 : 0
                : attributeData.ConstructorArguments.Length > 3 ? attributeData.ConstructorArguments[3].Value as int? ?? 0 : 0;
            if (string.IsNullOrWhiteSpace(id) || string.IsNullOrWhiteSpace(productLineName) || string.IsNullOrWhiteSpace(third))
            {
                return null;
            }

            return new TypePluginDefinition(
                id,
                productLineName,
                order,
                third,
                version,
                typeSymbol.Name,
                typeSymbol.ToDisplayString(SymbolDisplayFormat.FullyQualifiedFormat));
        }
    }

    private static string? GetEnumMemberName(TypedConstant constant)
    {
        if (constant.Type is not INamedTypeSymbol enumType ||
            constant.Value is null)
        {
            return null;
        }

        var rawValue = Convert.ToInt64(constant.Value);
        foreach (var member in enumType.GetMembers().OfType<IFieldSymbol>())
        {
            if (member.HasConstantValue &&
                Convert.ToInt64(member.ConstantValue) == rawValue)
            {
                return member.Name;
            }
        }

        return null;
    }
}
