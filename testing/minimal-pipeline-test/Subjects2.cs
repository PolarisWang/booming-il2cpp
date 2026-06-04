// Minimal test — only uses value types, no generics, no BCL dependencies beyond mscorlib basics.
// Compiled as net8.0 with only System.Runtime reference.
namespace Minimal;

public static class Subjects
{
    public static int Add(int a, int b) => a + b;
    public static int Subtract(int a, int b) => a - b;
    public static long Factorial(int n)
    {
        long r = 1;
        for (int i = 2; i <= n; i++) r *= i;
        return r;
    }
}
