using System.Reflection;

namespace Chaos.IL2CPP.Tools.AutoTestGenerator;

/// <summary>
/// Builds C# expressions for constructing instances of types.
/// Used to generate instance method calls with proper constructor initialization.
/// </summary>
public sealed class CSharpExpressionBuilder
{
    private readonly CSharpSerializer _serializer;

    public CSharpExpressionBuilder(CSharpSerializer serializer)
    {
        _serializer = serializer;
    }

    /// <summary>
    /// Get the best instance expression for a type.
    /// Static types → type name (static call prefix).
    /// Types with default ctor → "new TypeName()".
    /// Fallback → "default(TypeName)!".
    /// </summary>
    public string GetInstanceExpression(string typeFullName, bool isStatic)
    {
        if (isStatic)
            return CSharpSerializer.MapToCSharpType(typeFullName);

        var csType = CSharpSerializer.MapToCSharpType(typeFullName);

        // Try to find a default constructor via runtime reflection
        try
        {
            var type = Type.GetType(typeFullName, throwOnError: false);
            if (type is not null && !type.IsAbstract && !type.IsInterface && !type.IsSealed)
            {
                var ctors = type.GetConstructors(BindingFlags.Public | BindingFlags.Instance);
                if (ctors.Any(c => c.GetParameters().Length == 0))
                    return $"new {csType}()";
            }
        }
        catch
        {
            // Best-effort — fall through to default
        }

        return _serializer.DefaultExpression(typeFullName);
    }
}
