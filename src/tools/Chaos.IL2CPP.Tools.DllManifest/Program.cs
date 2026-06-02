using System.Reflection;
using System.Text.Json;
using System.Text.Json.Serialization;

// ── CLI ──
if (args.Length < 2 || args[0] != "--dll")
{
    Console.Error.WriteLine("Usage: DllManifest --dll <path-to-dll> [--output <dir>]");
    return 1;
}

var dllIndex = Array.IndexOf(args, "--dll");
var dllPath = args[dllIndex + 1];
var outputIdx = Array.IndexOf(args, "--output");
var outputDir = outputIdx >= 0 ? args[outputIdx + 1] : null;

if (!File.Exists(dllPath))
{
    Console.Error.WriteLine($"ERROR: DLL not found: {dllPath}");
    return 1;
}

// ── Setup MetadataLoadContext ──
var assemblyName = Path.GetFileNameWithoutExtension(dllPath);
var dllDir = Path.GetDirectoryName(Path.GetFullPath(dllPath))!;
var runtimeDir = Path.GetDirectoryName(typeof(object).Assembly.Location)!;
var probeDirs = new[] { dllDir, runtimeDir };

var resolver = new ManifestAssemblyResolver(probeDirs);
using var mlc = new MetadataLoadContext(resolver, "System.Private.CoreLib");
var assembly = mlc.LoadFromAssemblyPath(Path.GetFullPath(dllPath));

// ── Scan all types ──
var allMethods = new List<MethodInfoData>();
var namespaceCounts = new Dictionary<string, int>(StringComparer.Ordinal);

foreach (var type in assembly.GetTypes().OrderBy(t => t.FullName))
{
    var ns = type.Namespace ?? "";
    var typeName = type.FullName ?? type.Name;

    // Skip: enums, interfaces, non-public types (AutoTestGenerator only handles public types)
    if (type.IsEnum || type.IsInterface || !type.IsVisible) continue;

    // Skip abstract types but NOT static classes: static class = IsAbstract + IsSealed (has method bodies).
    if (type.IsAbstract && !type.IsSealed) continue;

    // Skip compiler-generated special types
    if (type.GetCustomAttributesData().Any(a =>
        a.AttributeType.Name is
            "CompilerGeneratedAttribute" or
            "SerializableAttribute")) continue;

    var methods = type.GetMethods(
        BindingFlags.Public | BindingFlags.Static |
        BindingFlags.Instance | BindingFlags.DeclaredOnly);

    foreach (var method in methods.OrderBy(m => m.Name))
    {
        // Skip property accessors, event add/remove, operators
        if (method.IsSpecialName && (
                method.Name.StartsWith("get_") ||
                method.Name.StartsWith("set_") ||
                method.Name.StartsWith("add_") ||
                method.Name.StartsWith("remove_") ||
                method.Name.StartsWith("op_")))
            continue;

        // Skip inherited object methods
        if (method.Name is "ToString" or "Equals" or "GetHashCode" or "Finalize" or "MemberwiseClone" or "GetType")
            continue;

        // Skip generic methods with unresolved parameters
        if (method.ContainsGenericParameters) continue;

        // Build parameter list
        var paramNames = new List<string>();
        foreach (var p in method.GetParameters())
        {
            var pType = GetTypeName(p.ParameterType).TrimEnd('&');
            if (!string.IsNullOrEmpty(pType))
                paramNames.Add(pType);
        }

        var returnType = GetTypeName(method.ReturnType).TrimEnd('&');
        if (string.IsNullOrEmpty(returnType)) continue;

        // Build method subject ID
        var paramList = string.Join(",", paramNames);
        var methodSubjectId = $"{assemblyName}/{typeName}::{method.Name}:{returnType}({paramList})";

        // Visibility
        var visibility = "public";
        if (method.IsFamily) visibility = "protected";
        else if (method.IsPrivate) visibility = "private";
        else if (method.IsAssembly) visibility = "internal";
        else if (method.IsFamilyOrAssembly) visibility = "protected-internal";

        allMethods.Add(new MethodInfoData(
            methodSubjectId, ns, typeName, method.Name,
            returnType, paramNames, visibility, method.IsStatic));

        namespaceCounts.TryGetValue(ns, out var count);
        namespaceCounts[ns] = count + 1;
    }
}

// ── Build output ──
var manifest = new ManifestData(
    SchemaVersion: 1,
    AssemblyName: assemblyName,
    GeneratedAt: DateTime.UtcNow.ToString("o"),
    TotalMethods: allMethods.Count,
    Methods: allMethods,
    Namespaces: namespaceCounts
        .OrderBy(kv => kv.Key)
        .ToDictionary(kv => kv.Key, kv => new NamespaceInfo(kv.Value))
);

var jsonOptions = new JsonSerializerOptions
{
    WriteIndented = true,
    PropertyNamingPolicy = JsonNamingPolicy.CamelCase,
    DefaultIgnoreCondition = JsonIgnoreCondition.WhenWritingNull
};

var json = JsonSerializer.Serialize(manifest, jsonOptions);

if (outputDir != null)
{
    Directory.CreateDirectory(outputDir);
    var path = Path.Combine(outputDir, "dll-method-manifest.json");
    File.WriteAllText(path, json);
    Console.WriteLine($"[Output] {path}  ({allMethods.Count} methods, {namespaceCounts.Count} namespaces)");
}
else
{
    Console.WriteLine(json);
}

return 0;

// ── Helpers ──
static string GetTypeName(Type type)
{
    if (type.IsGenericParameter) return "System.Object";
    if (type.IsByRef && type.GetElementType() is { } el)
        return GetTypeName(el);

    if (type.IsGenericType)
    {
        var defName = type.GetGenericTypeDefinition().FullName ?? type.Name;
        var tick = defName.IndexOf('`');
        if (tick >= 0) defName = defName[..tick];
        var args = string.Join(",", type.GetGenericArguments().Select(GetTypeName));
        return $"{defName}<{args}>";
    }

    return type.FullName ?? type.Name;
}
