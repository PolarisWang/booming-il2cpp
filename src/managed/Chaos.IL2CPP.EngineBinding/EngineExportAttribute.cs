namespace Chaos.IL2CPP.EngineBinding;

[AttributeUsage(AttributeTargets.Method, AllowMultiple = false, Inherited = false)]
public sealed class EngineExportAttribute(string exportName) : Attribute
{
    public string ExportName { get; } = string.IsNullOrWhiteSpace(exportName)
        ? throw new ArgumentException("exportName must not be empty.", nameof(exportName))
        : exportName;
}
