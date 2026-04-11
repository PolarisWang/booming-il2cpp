using System.Reflection;
using System.Runtime.InteropServices;

namespace MarshalingProof;

internal static class NativeMethods
{
    [DllImport("kernel32.dll", ExactSpelling = true)]
    internal static extern ulong GetTickCount64();
}

internal static class NativeExports
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
    public static unsafe int Run()
    {
        string roundTripText = MarshalUtf8("marshal-ok");
        string exportSummary = ValidateExportContract();

        if (OperatingSystem.IsWindows())
        {
            _ = NativeMethods.GetTickCount64();
        }

        Console.WriteLine($"marshal={roundTripText}");
        Console.WriteLine($"export={exportSummary}");
        return 0;
    }

    private static unsafe string ValidateExportContract()
    {
        MethodInfo addMethod =
            typeof(NativeExports).GetMethod(nameof(NativeExports.Add), BindingFlags.Public | BindingFlags.Static)
            ?? throw new InvalidOperationException("native export method missing");
        UnmanagedCallersOnlyAttribute? exportAttribute = addMethod.GetCustomAttribute<UnmanagedCallersOnlyAttribute>();

        if (exportAttribute?.EntryPoint is not NativeExports.AddEntryPoint)
        {
            throw new InvalidOperationException("native export entry point drifted");
        }

        delegate* unmanaged<int, int, int> export = &NativeExports.Add;
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

internal static class Program
{
    public static int Main()
    {
        return MarshalingProofEntry.Run();
    }
}
