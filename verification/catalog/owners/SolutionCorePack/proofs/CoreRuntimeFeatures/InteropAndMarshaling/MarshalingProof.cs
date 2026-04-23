using Chaos.TestFramework;
using System.Reflection;
using System.Runtime.InteropServices;

namespace CoreRuntimeFeatures;

internal static class MarshalingNativeMethods
{
    [DllImport("kernel32.dll", ExactSpelling = true)]
    internal static extern ulong GetTickCount64();
}

internal static class MarshalingNativeExports
{
    internal const string AddEntryPoint = "chaos_marshaled_add";

    [UnmanagedCallersOnly(EntryPoint = AddEntryPoint)]
    public static int Add(int left, int right)
    {
        return left + right;
    }
}

internal static class MarshalingProofEntry
{
    [ChaosUnitTest(
        ChaosUnitCategory.InteropContract,
        Alias = "marshaling-proof",
        CapabilityFamily = ChaosCapabilityFamily.InteropAndMarshaling,
        Capability = ChaosCapabilityItem.StringAndUtf8Marshaling,
        Requires = ChaosRuntimeFeature.Reflection | ChaosRuntimeFeature.NativeInterop,
        Priority = 7)]
    public static unsafe int Run()
    {
        string roundTripText = MarshalUtf8("marshal-ok");
        string exportSummary = ValidateExportContract();
        Assert.Equal("marshal-ok", roundTripText);
        Assert.Equal($"{MarshalingNativeExports.AddEntryPoint}:7", exportSummary);

        if (OperatingSystem.IsWindows())
        {
            _ = MarshalingNativeMethods.GetTickCount64();
        }

        return 0;
    }

    private static unsafe string ValidateExportContract()
    {
        MethodInfo addMethod =
            typeof(MarshalingNativeExports).GetMethod(nameof(MarshalingNativeExports.Add), BindingFlags.Public | BindingFlags.Static)
            ?? throw new InvalidOperationException("native export method missing");
        UnmanagedCallersOnlyAttribute? exportAttribute = addMethod.GetCustomAttribute<UnmanagedCallersOnlyAttribute>();

        if (exportAttribute?.EntryPoint is not MarshalingNativeExports.AddEntryPoint)
        {
            throw new InvalidOperationException("native export entry point drifted");
        }

        delegate* unmanaged<int, int, int> export = &MarshalingNativeExports.Add;
        return $"{exportAttribute.EntryPoint}:{export(3, 4)}";
    }

    private static string MarshalUtf8(string input)
    {
        IntPtr buffer = Marshal.StringToCoTaskMemUTF8(input);
        try
        {
            return Marshal.PtrToStringUTF8(buffer) ?? string.Empty;
        }
        finally
        {
            Marshal.FreeCoTaskMem(buffer);
        }
    }
}
