using System.Runtime.InteropServices;

namespace PInvokeDirectCall.App;

internal static class NativeMethods
{
    [DllImport("kernel32.dll", EntryPoint = "MulDiv", ExactSpelling = true)]
    internal static extern int MulDiv(int number, int numerator, int denominator);
}

internal static class PInvokeProofEntry
{
    public static int Run()
    {
        return NativeMethods.MulDiv(21, 2, 3);
    }
}

internal static class Program
{
    private static int Main()
    {
        return PInvokeProofEntry.Run();
    }
}
