namespace MinimalTest;

public static class Subjects
{
    // ── Basic numeric operations (no dependencies, no special IL) ──
    public static int Add(int a, int b) => a + b;
    public static int Subtract(int a, int b) => a - b;
    public static bool IsPositive(int x) => x > 0;
    public static string Hello() => "Hello from IL2CPP!";
    public static double Divide(double a, double b) => a / b;
    public static long Factorial(int n)
    {
        long result = 1;
        for (int i = 2; i <= n; i++) result *= i;
        return result;
    }
}
