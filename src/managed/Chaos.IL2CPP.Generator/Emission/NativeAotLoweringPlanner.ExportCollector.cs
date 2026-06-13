using System.Text;
using System.Text.RegularExpressions;

namespace Chaos.IL2CPP.Generator;

public sealed partial class NativeAotLoweringPlanner
{
    /// <summary>
    /// Scan generated C++ content for <c>chaos_external_runtime_*</c> function
    /// calls that lack any preceding declaration and prepend extern "C" declarations.
    /// This is a post-scan catch-all that ensures the shared header is self-contained
    /// — every external-runtime symbol referenced in page code has a declaration visible
    /// at translation-unit scope.
    /// </summary>
    private static string BuildAbiExportDeclarations(StringBuilder sb)
    {
        string text = sb.ToString();
        string decls = BuildAbiExportDeclarations(text);
        if (decls.Length == 0) return text;

        // Insert after the last #include directive so extern declarations
        // appear before any code that references them.
        int anchor = text.LastIndexOf("#include");
        if (anchor < 0) anchor = 0;
        else
        {
            int nl = text.IndexOf('\n', anchor);
            if (nl >= 0) anchor = nl + 1;
        }
        return text.Insert(anchor, decls);
    }

    /// <summary>Convenience overload accepting a plain string.</summary>
    private static string BuildAbiExportDeclarations(string text)
    {
        // 1. Collect ALL existing declaration forms.
        var declared = new HashSet<string>();
        foreach (Match m in s_declRx.Matches(text))
            declared.Add(m.Groups[1].Value);

        // 2. Collect function-call references.
        var referenced = new HashSet<string>();
        foreach (Match m in s_callRx.Matches(text))
            referenced.Add(m.Groups[1].Value);

        // 3. Difference — referenced but not declared.
        referenced.ExceptWith(declared);
        if (referenced.Count == 0) return string.Empty;

        // 4. Emit missing extern "C" declarations.
        var stub = new StringBuilder();
        stub.AppendLine("// ── External runtime ABI export declarations (auto-generated) ──");
        foreach (var sym in referenced.OrderBy(s => s))
        {
            stub.Append("extern \"C\" CHAOS_IL2CPP_INTPTR ");
            stub.Append(sym);
            stub.AppendLine("() noexcept;");
        }
        stub.AppendLine();
        return stub.ToString();
    }

    // Regex for declarations: extern / extern "C" / static inline
    // Matches symbol followed by ';', ')', or '(' (covers both
    // "Type name();" declarations and "static inline Type name() { ... }" definitions).
    private static readonly Regex s_declRx = new(
        @"\b(chaos_external_runtime_\w+)\s*(?=[\);\(])",
        RegexOptions.Compiled);

    // Regex for function-call references: symbol followed by '('
    private static readonly Regex s_callRx = new(
        @"\b(chaos_external_runtime_\w+)\(",
        RegexOptions.Compiled);
}
