// Delegate subjects — delegate creation, invocation
using System;

namespace GenericsDelegates;

// Custom delegate type (non-generic)
public delegate int BinaryOp(int a, int b);

public static class DelegateSubjects
{
    public static int Add(int a, int b) => a + b;
    public static int Multiply(int a, int b) => a * b;

    // Test basic delegate creation and invocation
    public static int TestCustomDelegate()
    {
        var del = new BinaryOp(Add);
        return del(3, 4);
    }

    // Test delegate as parameter
    public static int ApplyFunction(BinaryOp op, int x, int y)
        => op(x, y);

    public static int TestDelegateAsParam()
        => ApplyFunction(Multiply, 5, 6);
}
