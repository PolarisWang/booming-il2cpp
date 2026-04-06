using System.Reflection;

namespace ReflectionLiteQueryMinimal;

internal sealed class ReflectionFixture<TValue>
    where TValue : notnull
{
    public TValue BackingField;

    public ReflectionFixture(TValue initialValue)
    {
        BackingField = initialValue;
        Value = initialValue;
    }

    public TValue Value { get; private set; }

    public TValue Echo(TValue nextValue, int repeatCount = 1)
    {
        Value = nextValue;
        return nextValue;
    }
}

internal static class Program
{
    private static int Main()
    {
        Type closedType = typeof(ReflectionFixture<int>);
        FieldInfo field = closedType.GetField(nameof(ReflectionFixture<int>.BackingField))!;
        PropertyInfo property = closedType.GetProperty(nameof(ReflectionFixture<int>.Value))!;
        MethodInfo method = closedType.GetMethod(nameof(ReflectionFixture<int>.Echo))!;
        ParameterInfo parameter = method.GetParameters()[0];
        Type genericTypeDefinition = closedType.GetGenericTypeDefinition();

        _ = field;
        _ = property;
        _ = parameter;
        _ = genericTypeDefinition;
        Console.WriteLine("reflection-ok");
        return 0;
    }
}
