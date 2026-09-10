// Patch for ArithmeticProof.Add — changes a+b to a*b
// The class must be in global namespace (matching the AOT type registration)
public static class ArithmeticProof
{
    public static int Add(int a, int b)
    {
        return a * b;
    }
}
