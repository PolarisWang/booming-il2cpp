using Chaos.TestFramework;
using System.Reflection;
using System.Runtime.InteropServices;

namespace CoreRuntimeFeatures;

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
        CapabilityFamily = ChaosCapabilityFamily.ReflectionAndMetadata,
        Capability = ChaosCapabilityItem.MetadataClosure,
        Requires = ChaosRuntimeFeature.Reflection | ChaosRuntimeFeature.NativeInterop,
        Priority = 7)]
    public static int Run()
    {
        Type closedType = typeof(ReflectionClosureBox<string>);
        FieldInfo field = closedType.GetField(nameof(ReflectionClosureBox<string>.Value))!;
        MethodInfo method = closedType.GetMethod(nameof(ReflectionClosureBox<string>.Echo))!;
        Type genericTypeDefinition = closedType.GetGenericTypeDefinition();
        Assert.Equal("Value", field.Name);
        Assert.Equal("Echo", method.Name);
        Assert.Equal("ReflectionClosureBox`1", genericTypeDefinition.Name);
        _ = ClosureNativeMethods.GetTickCount64();
        return 0;
    }
}
