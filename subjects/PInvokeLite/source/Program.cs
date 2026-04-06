using System.Reflection;
using System.Runtime.InteropServices;

internal static class NativeMethods
{
    [DllImport("kernel32.dll", ExactSpelling = true)]
    internal static extern ulong GetTickCount64();
}

internal static class NativeExports
{
    internal const string AddEntryPoint = "chaos_smoke_add";

    [UnmanagedCallersOnly(EntryPoint = AddEntryPoint)]
    public static int Add(int left, int right)
    {
        return left + right;
    }
}

internal static class Program
{
    private static int Main()
    {
        string libraryName = ResolveLibraryName();
        string symbolName = ResolveSymbolName();
        string roundTripText = MarshalUtf8("interop-smoke");
        string exportSummary = ValidateExportContract();

        if (OperatingSystem.IsWindows())
        {
            Console.WriteLine($"tick={NativeMethods.GetTickCount64()}");
        }

        Console.WriteLine($"marshal={roundTripText}");
        Console.WriteLine($"export={exportSummary}");
        Console.WriteLine($"symbol={TryResolveSymbol(libraryName, symbolName)}");
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

        ValidateExportSignature(addMethod);

        delegate* unmanaged<int, int, int> export = &NativeExports.Add;
        return $"{exportAttribute.EntryPoint}:{export(3, 4)}";
    }

    private static void ValidateExportSignature(MethodInfo addMethod)
    {
        ParameterInfo[] parameters = addMethod.GetParameters();
        bool signatureMatches =
            addMethod.ReturnType == typeof(int)
            && parameters.Length == 2
            && parameters[0].ParameterType == typeof(int)
            && parameters[1].ParameterType == typeof(int);

        if (!signatureMatches)
        {
            throw new InvalidOperationException("native export signature drifted");
        }
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

    private static bool TryResolveSymbol(string libraryName, string symbolName)
    {
        if (!NativeLibrary.TryLoad(libraryName, out nint handle))
        {
            return false;
        }

        try
        {
            return NativeLibrary.TryGetExport(handle, symbolName, out _);
        }
        finally
        {
            NativeLibrary.Free(handle);
        }
    }

    private static string ResolveLibraryName()
    {
        if (OperatingSystem.IsWindows())
        {
            return "kernel32.dll";
        }

        if (OperatingSystem.IsLinux())
        {
            return "libc.so.6";
        }

        return "libSystem.B.dylib";
    }

    private static string ResolveSymbolName()
    {
        if (OperatingSystem.IsWindows())
        {
            return "GetTickCount64";
        }

        return "strlen";
    }
}
