namespace Chaos.IL2CPP.Tools.TestProjectGenerator.Metadata;

public enum SubjectKind { Fact, Benchmark, HotUpdate }

public sealed class SubjectModel
{
    public SubjectKind Kind { get; init; }
    public string AssemblyName { get; init; } = "";
    public string FullTypeName { get; init; } = "";
    public string MethodName { get; init; } = "";
    public bool IsStatic { get; init; }
    public string ReturnTypeFullName { get; init; } = "";
    public List<ParameterModel> Parameters { get; init; } = [];
    public string SubjectId { get; init; } = "";
    public string? GeneratedMethodId { get; set; }  // AOT-suffix method name (e.g. "HtmlEncode_0_string_0")
}

public sealed class ParameterModel
{
    public string TypeFullName { get; init; } = "";
    public string Name { get; init; } = "";
}
