using System.Runtime.InteropServices;

namespace PInvokeLiteDllImportMinimal;

internal static class NativeMethods
{
    [DllImport("kernel32.dll", ExactSpelling = true)]
    internal static extern int MulDiv(int number, int numerator, int denominator);
}

internal static class Program
{
    private static int Main()
    {
        return NativeMethods.MulDiv(6, 7, 3);
    }
}
