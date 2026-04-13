using System.Reflection;

internal delegate void ValueChangedHandler<TValue>(object sender, TValue value);

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

    public event ValueChangedHandler<TValue>? Changed;

    public TValue Echo(TValue nextValue, int repeatCount = 1)
    {
        Value = nextValue;
        Changed?.Invoke(this, nextValue);
        return repeatCount > 0 ? nextValue : BackingField;
    }

    public Dictionary<string, TMetadata> Project<TMetadata>(TValue input, TMetadata metadata)
        where TMetadata : notnull
    {
        return new Dictionary<string, TMetadata>
        {
            ["value"] = metadata,
        };
    }
}

internal static class Program
{
    private static int Main()
    {
        Type closedType = typeof(ReflectionFixture<int>);
        FieldInfo field = closedType.GetField(nameof(ReflectionFixture<int>.BackingField))!;
        PropertyInfo property = closedType.GetProperty(nameof(ReflectionFixture<int>.Value))!;
        EventInfo eventInfo = closedType.GetEvent(nameof(ReflectionFixture<int>.Changed))!;
        MethodInfo method = closedType.GetMethod(nameof(ReflectionFixture<int>.Project))!;
        ParameterInfo[] parameters = method.GetParameters();
        MethodInfo closedMethod = method.MakeGenericMethod(typeof(string));

        Console.WriteLine($"type={closedType.FullName}");
        Console.WriteLine($"field={field.Name}:{field.FieldType.Name}");
        Console.WriteLine($"property={property.Name}:{property.PropertyType.Name}");
        Console.WriteLine($"event={eventInfo.Name}:{eventInfo.EventHandlerType?.Name}");
        Console.WriteLine($"method={method.Name}:{method.ReturnType.Name}");
        Console.WriteLine($"parameters={string.Join(",", parameters.Select(parameter => parameter.Name))}");
        Console.WriteLine($"generic-type={closedType.GetGenericTypeDefinition().Name}");
        Console.WriteLine($"generic-method={closedMethod.ReturnType.GetGenericArguments()[1].Name}");
        return 0;
    }
}
