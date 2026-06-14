using System.Linq;

namespace Chaos.IL2CPP.Generator;

public sealed partial class NativeAotLoweringPlanner
{
    /// <summary>
    /// Extract a C++ parameter signature from the managed method subject ID.
    /// Format: Assembly/Type::Method:ReturnType(Param1,Param2,...)
    /// Returns "CHAOS_IL2CPP_INTPTR, CHAOS_IL2CPP_INTPTR, ..." for each parameter.
    /// </summary>
    private static string InferParameterSignatureFromSubjectId(string subjectId)
    {
        int open = subjectId.LastIndexOf('(');
        if (open < 0) return "";
        int close = subjectId.LastIndexOf(')');
        if (close <= open + 1) return "";
        string args = subjectId.Substring(open + 1, close - open - 1);
        if (string.IsNullOrWhiteSpace(args)) return "";
        int paramCount = args.Split(',').Length;
        return string.Join(", ", Enumerable.Repeat("CHAOS_IL2CPP_INTPTR", paramCount));
    }
}
