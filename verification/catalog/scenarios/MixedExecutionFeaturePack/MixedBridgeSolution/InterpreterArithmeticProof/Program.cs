using InterpreterArithmeticProof.AotBridge;

namespace InterpreterArithmeticProof;

public sealed class InstanceArithmetic
{
    public int AddOne(int value)
    {
        return value + 1;
    }
}

internal static class NativeExports
{
    public static int Add(int left, int right)
    {
        return left + right;
    }

    public static int AddTwentyOne(int value)
    {
        return value + 21;
    }

    public static int CallAotBridgeAdd(int left, int right)
    {
        return AotBridgeExports.Add(left, right);
    }

    public static int CallLocalAdd(int left, int right)
    {
        return Add(left, right);
    }

    public static int CallInstanceAddOne(InstanceArithmetic receiver, int value)
    {
        return receiver.AddOne(value);
    }

    public static int CallStringLength()
    {
        return "chaos".Length;
    }

    public static int DivideOrCatch(int divisor)
    {
        try
        {
            return 10 / divisor;
        }
        catch (DivideByZeroException)
        {
            return 42;
        }
    }

    public static int DivideOrRethrow(int divisor)
    {
        try
        {
            return 10 / divisor;
        }
        catch (DivideByZeroException)
        {
            throw;
        }
    }

    public static int AddWithFinally()
    {
        var result = 40;
        try
        {
            result += 0;
        }
        finally
        {
            result += 2;
        }

        return result;
    }
}

internal static class GenericExports
{
    public static T Echo<T>(T value)
    {
        return value;
    }

    public static int EchoStringLength(string value)
    {
        return Echo(value).Length;
    }
}

internal static class Program
{
    public static int Main()
    {
        return 0;
    }
}
