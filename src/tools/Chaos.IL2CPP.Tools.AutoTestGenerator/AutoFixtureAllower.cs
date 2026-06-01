using AutoFixture;
using AutoFixture.Kernel;

namespace Chaos.IL2CPP.Tools.AutoTestGenerator;

/// <summary>
/// Wraps AutoFixture with a strict allowlist of types that can be meaningfully
/// serialized to C# expressions. Prevents AutoFixture from generating random
/// complex objects (e.g. IFormatProvider, Type) that can't be serialized.
/// </summary>
public sealed class AutoFixtureAllower
{
    private readonly Fixture _fixture;
    private readonly CSharpSerializer _serializer;

    private static readonly HashSet<string> AllowedTypes = new(StringComparer.Ordinal)
    {
        // Primitives
        "System.Boolean", "System.Byte", "System.SByte", "System.Int16", "System.UInt16",
        "System.Int32", "System.UInt32", "System.Int64", "System.UInt64",
        "System.Single", "System.Double", "System.Decimal", "System.Char",
        // Simple BCL types
        "System.String", "System.DateTime", "System.DateTimeOffset",
        "System.TimeSpan", "System.Guid", "System.Uri", "System.Version",
        // IntPtr/UIntPtr (limited)
        "System.IntPtr", "System.UIntPtr",
    };

    public AutoFixtureAllower(CSharpSerializer serializer)
    {
        _serializer = serializer;
        _fixture = new Fixture();

        // Replace default specimen builders with a delegate that checks the allowlist
        _fixture.Customizations.Insert(0, new TypeAllowlistBuilder());
    }

    /// <summary>
    /// Try to generate a random C# expression for the given type.
    /// Returns null if the type is not in the allowlist or serialization fails.
    /// </summary>
    public string? TryGenerateExpression(string typeName)
    {
        // Only allowed simple types — skip complex/interface/array types
        if (!AllowedTypes.Contains(typeName))
        {
            // Enums are allowed if they resolve at runtime
            if (!IsEnumType(typeName))
                return null;
        }

        try
        {
            var type = Type.GetType(typeName, throwOnError: false);
            if (type is null) return null;

            var value = _fixture.Create(type, new SpecimenContext(_fixture));
            return _serializer.Serialize(typeName, value);
        }
        catch
        {
            return null;
        }
    }

    /// <summary>
    /// Check if a type name refers to an enum via runtime reflection.
    /// </summary>
    private static bool IsEnumType(string typeName)
    {
        try
        {
            var type = Type.GetType(typeName, throwOnError: false);
            return type?.IsEnum == true;
        }
        catch
        {
            return false;
        }
    }

    /// <summary>
    /// Custom ISpecimenBuilder that only allows types in the allowlist or enums.
    /// All other types return NoSpecimen (defer to other builders / fall back).
    /// </summary>
    private sealed class TypeAllowlistBuilder : ISpecimenBuilder
    {
        public object Create(object request, ISpecimenContext context)
        {
            if (request is Type type)
            {
                var fullName = type.FullName;
                if (fullName is not null && AllowedTypes.Contains(fullName))
                    return context.Resolve(request);
                if (type.IsEnum)
                    return context.Resolve(request);
            }

            return new NoSpecimen();
        }
    }
}
