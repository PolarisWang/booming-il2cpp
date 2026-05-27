using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;

namespace Chaos.IL2CPP.Driver;

internal sealed class AsmCompareConfig
{
    public string AssemblyPath { get; init; } = "";
    public string MethodName { get; init; } = "";
    public IReadOnlyList<string> MethodNames { get; init; } = Array.Empty<string>();
    public IReadOnlyList<string> MethodSubjectIds { get; init; } = Array.Empty<string>();
    public string? OutputPath { get; init; }
    public HashSet<string> Sections { get; init; } = new(StringComparer.OrdinalIgnoreCase);
    public bool AllSections { get; init; } = true;
    public bool KeepTemp { get; init; }
    public bool IsHelp { get; init; }
    public string Format { get; init; } = "text"; // "text" or "json"

    public static readonly string[] AllSectionNames = new[]
    {
        "header", "il-dump", "raw-jit", "raw-cpp", "side-by-side",
        "ir-trace", "raw-aot", "inline-map", "metrics", "analysis"
    };

    public static AsmCompareConfig Parse(string[] args)
    {
        if (args.Length == 0)
        {
            PrintHelp();
            return new AsmCompareConfig { IsHelp = true };
        }

        string? assemblyPath = null;
        string? methodName = null;
        List<string>? methodNames = null;
        List<string>? methodSubjectIds = null;
        string? outputPath = null;
        HashSet<string>? sections = null;
        bool keepTemp = false;
        string format = "text";

        for (int i = 0; i < args.Length; i++)
        {
            switch (args[i])
            {
                case "--method" when i + 1 < args.Length:
                    methodName = args[++i];
                    break;
                case "--methods" when i + 1 < args.Length:
                    methodNames = new List<string>(
                        args[++i].Split(',', StringSplitOptions.RemoveEmptyEntries | StringSplitOptions.TrimEntries));
                    break;
                case "--method-subject-ids" when i + 1 < args.Length:
                    var rawIds = args[++i];
                    methodSubjectIds = SplitRespectingParens(rawIds);
                    break;
                case "--output" or "-o" when i + 1 < args.Length:
                    outputPath = args[++i];
                    break;
                case "--sections" when i + 1 < args.Length:
                    sections = new HashSet<string>(
                        args[++i].Split(',', StringSplitOptions.RemoveEmptyEntries | StringSplitOptions.TrimEntries),
                        StringComparer.OrdinalIgnoreCase);
                    break;
                case "--format" when i + 1 < args.Length:
                    format = args[++i].ToLowerInvariant();
                    break;
                case "--keep-temp":
                    keepTemp = true;
                    break;
                case "--help" or "-h":
                    PrintHelp();
                    return new AsmCompareConfig { IsHelp = true };
                default:
                    if (!args[i].StartsWith('-') && assemblyPath is null)
                        assemblyPath = args[i];
                    else
                    {
                        Console.Error.WriteLine($"Unknown argument: {args[i]}");
                        PrintHelp();
                        return new AsmCompareConfig { IsHelp = true };
                    }
                    break;
            }
        }

        if (assemblyPath is null)
        {
            Console.Error.WriteLine("Error: managed assembly path is required.");
            PrintHelp();
            return new AsmCompareConfig { IsHelp = true };
        }

        if (!File.Exists(assemblyPath))
        {
            Console.Error.WriteLine($"Error: assembly not found: {assemblyPath}");
            return new AsmCompareConfig { IsHelp = true };
        }

        if (!string.IsNullOrWhiteSpace(methodName))
        {
            methodNames = new List<string> { methodName };
        }
        else if (methodNames is null || methodNames.Count == 0)
        {
            Console.Error.WriteLine("Error: --method or --methods is required.");
            PrintHelp();
            return new AsmCompareConfig { IsHelp = true };
        }

        return new AsmCompareConfig
        {
            AssemblyPath = assemblyPath,
            MethodName = methodNames![0],
            MethodNames = methodNames,
            MethodSubjectIds = methodSubjectIds ?? new List<string>(),
            OutputPath = outputPath,
            Sections = sections ?? new HashSet<string>(AllSectionNames, StringComparer.OrdinalIgnoreCase),
            AllSections = sections is null,
            KeepTemp = keepTemp,
            Format = format,
        };
    }

    public bool HasSection(string name) => AllSections || Sections.Contains(name);

    /// Split a comma-separated list respecting balanced parentheses.
    /// MethodSubjectIds like "ToChar(System.Object,System.IFormatProvider)" contain
    /// commas inside parens that must NOT be treated as delimiters.
    private static List<string> SplitRespectingParens(string raw)
    {
        var result = new List<string>();
        int depth = 0;
        int start = 0;
        for (int i = 0; i < raw.Length; i++)
        {
            if (raw[i] == '(') depth++;
            else if (raw[i] == ')') depth--;
            else if (raw[i] == ',' && depth == 0)
            {
                var part = raw[start..i].Trim();
                if (part.Length > 0) result.Add(part);
                start = i + 1;
            }
        }
        var last = raw[start..].Trim();
        if (last.Length > 0) result.Add(last);
        return result;
    }

    private static void PrintHelp()
    {
        Console.WriteLine("chaos-il2cpp asm-compare — Compare JIT vs IL2CPP translation for a method");
        Console.WriteLine();
        Console.WriteLine("Usage:");
        Console.WriteLine("  chaos-il2cpp asm-compare <managed.dll|exe> --method <name> [options]");
        Console.WriteLine();
        Console.WriteLine("Options:");
        Console.WriteLine("  --method <name>               Single method to compare (e.g. TypeName::MethodName)");
        Console.WriteLine("  --methods <names>             Comma-separated method list for batch mode");
        Console.WriteLine("  --output, -o <file>           Output report file (default: stdout)");
        Console.WriteLine("  --sections <list>             Comma-separated sections (default: all)");
        Console.WriteLine("    Available: " + string.Join(", ", AllSectionNames));
        Console.WriteLine("  --format <fmt>                Output format: text (default) or json");
        Console.WriteLine("  --keep-temp                   Keep temporary pipeline output");
        Console.WriteLine("  --help, -h                    Show this help");
    }
}