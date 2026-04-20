using System;
using System.Reflection;
using System.Runtime.InteropServices;

namespace ReflectionInteropClosure.App;

internal static class ClosureNativeMethods
{
    [DllImport("kernel32.dll", ExactSpelling = true)]
    internal static extern ulong GetTickCount64();
}

internal sealed class ReflectionClosureBox<T>
{
    public T Value;

    public ReflectionClosureBox(T value)
    {
        Value = value;
    }

    public T Echo(T input)
    {
        return input;
    }
}

internal static class ReflectionInteropClosureEntry
{
    public static int Run()
    {
        Type closedType = typeof(ReflectionClosureBox<string>);
        FieldInfo field = closedType.GetField(nameof(ReflectionClosureBox<string>.Value))!;
        MethodInfo method = closedType.GetMethod(nameof(ReflectionClosureBox<string>.Echo))!;
        Type genericTypeDefinition = closedType.GetGenericTypeDefinition();
        if (OperatingSystem.IsWindows())
        {
            _ = ClosureNativeMethods.GetTickCount64();
        }

        if (genericTypeDefinition.Name != "ReflectionClosureBox`1")
        {
            return 1;
        }

        Console.WriteLine($"closure-ok|ReflectionClosureBox<String>|{field.Name}|{method.Name}|{nameof(ClosureNativeMethods.GetTickCount64)}");
        return 0;
    }
}

internal static class Program
{
    private static int Main()
    {
        return ReflectionInteropClosureEntry.Run();
    }
}
