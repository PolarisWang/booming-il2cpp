using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text.RegularExpressions;

namespace Chaos.IL2CPP.Generator.Validation;

public sealed class NativeCodegenValidator
{
    public sealed class ValidationResult
    {
        public bool IsValid => Errors.Count == 0;
        public List<string> Errors { get; } = [];
        public List<string> Warnings { get; } = [];
    }

    private static readonly Regex UsingNamespaceRegex = new(
        @"using\s+namespace\s+(std|chaos)\s*;",
        RegexOptions.Compiled);

    private static readonly Regex NullPointerRegex = new(
        @"\bNULL\b",
        RegexOptions.Compiled);

    private static readonly Regex BareIntRegex = new(
        @"(?<!\w)(?<!std::)(?<!::)(?<!\w_)(int|long|unsigned\s+(int|long))(?!\w)(?!\s*_)",
        RegexOptions.Compiled | RegexOptions.IgnoreCase);

    private static readonly Regex IncludeGuardRegex = new(
        @"#ifndef\s+CHAOS_IL2CPP_\w+_H_\s*",
        RegexOptions.Compiled);

    private static readonly Regex ChaosPrefixInNamespaceRegex = new(
        @"\bchaos_\w+",
        RegexOptions.Compiled);

    private static readonly Regex RawNewDeleteRegex = new(
        @"\b(?:new|delete)\s+\w+",
        RegexOptions.Compiled);

    private static readonly Regex UsingDirectiveRegex = new(
        @"^using\s+namespace\s+(std|chaos)\s*;",
        RegexOptions.Compiled | RegexOptions.Multiline);

    private static readonly Regex RawStdTypeRegex = new(
        @"\bstd::(intptr_t|int\d+_t|uint\d+_t|size_t|ptrdiff_t|string|string_view|vector|array|unique_ptr|make_unique|shared_ptr|make_shared|unordered_map|pair|abort\s*\(|malloc\s*\(|free\s*\(|realloc\s*\(|memcpy|memcmp|memmove|strcmp|strlen|memset|mutex|lock_guard|recursive_timed_mutex|recursive_mutex|once_flag|call_once|thread|numeric_limits|atomic|to_string|printf|fwrite|fputc|fflush|chrono|move|nothrow)\b",
        RegexOptions.Compiled);

    private static readonly Regex BareCStdintTypeRegex = new(
        @"(?<!\w)(intptr_t|int\d+_t|uint\d+_t|size_t|ptrdiff_t)(?!\w)",
        RegexOptions.Compiled);

    public ValidationResult ValidateFile(string filePath)
    {
        var result = new ValidationResult();
        if (!File.Exists(filePath))
        {
            result.Errors.Add($"File not found: {filePath}");
            return result;
        }

        var lines = File.ReadAllLines(filePath);
        var content = string.Join("\n", lines);
        var fileName = Path.GetFileName(filePath);

        ValidateContent(content, fileName, result);
        return result;
    }

    public ValidationResult ValidateContent(string content, string fileName, ValidationResult? result = null)
    {
        result ??= new ValidationResult();

        // Check 1: No `using namespace std;` or `using namespace chaos;`
        var usingMatch = UsingNamespaceRegex.Match(content);
        if (usingMatch.Success)
        {
            result.Errors.Add($"'{fileName}': 'using namespace std/chaos' is forbidden — use explicit std:: prefix instead (found near line {EstimateLineNumber(content, usingMatch.Index)})");
        }

        // Check 2: No `using std::` or `using chaos::` directives
        var usingDirectiveMatch = UsingDirectiveRegex.Match(content);
        if (usingDirectiveMatch.Success)
        {
            result.Errors.Add($"'{fileName}': 'using std::...' or 'using chaos::...' is forbidden — use explicit prefixes instead (found near line {EstimateLineNumber(content, usingDirectiveMatch.Index)})");
        }

        // Check 3: Must use nullptr, not NULL or 0 as pointer
        var nullMatch = NullPointerRegex.Match(content);
        if (nullMatch.Success)
        {
            result.Errors.Add($"'{fileName}': 'NULL' is forbidden — use 'nullptr' instead (found near line {EstimateLineNumber(content, nullMatch.Index)})");
        }

        // Check 4: Must use <cstdint> types (CHAOS_IL2CPP_INT32 etc.), not bare int/long
        // Only flag in generated code, not in runtime library code
        if (!fileName.StartsWith("chaos/", StringComparison.Ordinal))
        {
            var bareIntMatch = BareIntRegex.Match(content);
            if (bareIntMatch.Success)
            {
                result.Warnings.Add($"'{fileName}': prefer <cstdint> types (CHAOS_IL2CPP_INT32, etc.) over '{bareIntMatch.Value}' (found near line {EstimateLineNumber(content, bareIntMatch.Index)})");
            }
        }

        // Check 5: Include guard format (only for header files)
        if (fileName.EndsWith(".h", StringComparison.Ordinal) && !fileName.StartsWith("chaos/", StringComparison.Ordinal))
        {
            var headerLines = content.Split('\n');
            var hasGuard = headerLines.Length > 0 && IncludeGuardRegex.IsMatch(headerLines[0]);
            if (!hasGuard)
            {
                result.Warnings.Add($"'{fileName}': header should have include guard matching 'CHAOS_IL2CPP_*_H_'");
            }
        }

        // Check 6: chaos_common namespace symbols must not use chaos_ prefix
        if (content.Contains("namespace ChaosIl2cpp::Common"))
        {
            var chaosPrefixMatch = ChaosPrefixInNamespaceRegex.Match(content);
            if (chaosPrefixMatch.Success)
            {
                result.Errors.Add($"'{fileName}': ChaosIl2cpp::Common symbols must not use 'chaos_' prefix — use plain names instead (found '{chaosPrefixMatch.Value}' near line {EstimateLineNumber(content, chaosPrefixMatch.Index)})");
            }
        }

        // Check 7: Generated code must include <chaos/common.h> instead of inline preludes
        if (fileName.EndsWith(".cpp", StringComparison.Ordinal) && !fileName.Contains("chaos/"))
        {
            if (content.Contains("chaos_finally_scope_guard") || content.Contains("chaos_wrap_add"))
            {
                result.Errors.Add($"'{fileName}': contains inline prelude definitions (chaos_finally_scope_guard, chaos_wrap_add) — must include <chaos/common.h> instead");
            }
        }

        // Check 8: Generated code must not contain raw std:: types — use CHAOS_IL2CPP_* macros
        if (!fileName.StartsWith("chaos/", StringComparison.Ordinal) &&
            !fileName.StartsWith("native_types", StringComparison.Ordinal))
        {
            var rawStdMatch = RawStdTypeRegex.Match(content);
            if (rawStdMatch.Success)
            {
                result.Errors.Add($"'{fileName}': raw std:: type '{rawStdMatch.Value}' found — use CHAOS_IL2CPP_* macros instead (found near line {EstimateLineNumber(content, rawStdMatch.Index)})");
            }
        }

        // Check 9: Generated code must not contain bare <cstdint> types (int32_t, uint32_t, etc.) — use CHAOS_IL2CPP_* macros
        if (!fileName.StartsWith("chaos/", StringComparison.Ordinal) &&
            !fileName.StartsWith("native_types", StringComparison.Ordinal))
        {
            var bareTypeMatch = BareCStdintTypeRegex.Match(content);
            if (bareTypeMatch.Success)
            {
                result.Errors.Add($"'{fileName}': bare type '{bareTypeMatch.Value}' found — use CHAOS_IL2CPP_* macro instead (found near line {EstimateLineNumber(content, bareTypeMatch.Index)})");
            }
        }

        return result;
    }

    private static int EstimateLineNumber(string content, int index)
    {
        if (index <= 0) return 1;
        var lineNumber = 1;
        for (var i = 0; i < index && i < content.Length; i++)
        {
            if (content[i] == '\n') lineNumber++;
        }
        return lineNumber;
    }

    public static bool ValidateGeneratedFile(string filePath, List<string>? errors = null)
    {
        var validator = new NativeCodegenValidator();
        var result = validator.ValidateFile(filePath);
        if (errors != null)
        {
            errors.AddRange(result.Errors);
        }

        if (!result.IsValid)
        {
            foreach (var error in result.Errors)
            {
                System.Console.Error.WriteLine($"[NativeCodegenValidator] ERROR: {error}");
            }
        }

        foreach (var warning in result.Warnings)
        {
            System.Console.Error.WriteLine($"[NativeCodegenValidator] WARNING: {warning}");
        }

        return result.IsValid;
    }
}
