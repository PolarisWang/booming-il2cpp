using Chaos.TestFramework;
using System.Reflection;
using System.Runtime.InteropServices;

namespace MainlineFeaturePack;

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
    [ChaosUnitTest(
        ChaosUnitCategory.InteropContract,
        Alias = "reflection-interop-closure-proof",
        Requires = ChaosRuntimeFeature.Reflection | ChaosRuntimeFeature.NativeInterop,
        Evidence = ChaosEvidenceKind.Stdout,
        Priority = 7)]
    public static int Run()
    {
        Type closedType = typeof(ReflectionClosureBox<string>);
        FieldInfo field = closedType.GetField(nameof(ReflectionClosureBox<string>.Value))!;
        MethodInfo method = closedType.GetMethod(nameof(ReflectionClosureBox<string>.Echo))!;
        Type genericTypeDefinition = closedType.GetGenericTypeDefinition();
        _ = ClosureNativeMethods.GetTickCount64();
        _ = field;
        _ = method;
        _ = genericTypeDefinition;
        Console.WriteLine("closure-ok|ReflectionClosureBox<String>|Value|Echo|GetTickCount64");
        return 0;
    }
}
