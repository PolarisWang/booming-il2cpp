using System.Diagnostics;

namespace Chaos.Generated.ManagedTests.SolutionCorePack;

public static class SolutionCorePackBenchmarkWorkloads
{
    public static int InvokeWorkload(int entryIndex)
    {
        return entryIndex switch
        {
            0 => RunArithmeticInt(),
            1 => RunArithmeticDouble(),
            2 => RunStringConcat(),
            3 => RunArrayAccess(),
            4 => RunObjectAlloc(),
            5 => RunVirtualCall(),
            6 => RunLoopBranch(),
            _ => RunArithmeticInt(),
        };
    }

    public static int RunArithmeticInt()
    {
        long sum = 0;
        for (int i = 0; i < 10000; i++)
            sum += i * (i + 1) / (i | 1) - (i >> 1);
        return (int)(sum & 0x7FFFFFFF);
    }

    public static int RunArithmeticDouble()
    {
        double sum = 0.0;
        for (int i = 0; i < 10000; i++)
            sum += (double)i * 3.14159 / (i + 1.0);
        return (int)((long)(sum * 1000) % 10000);
    }

    public static int RunStringConcat()
    {
        string s = "";
        for (int i = 0; i < 500; i++)
            s = string.Concat(s, i.ToString());
        return s.Length;
    }

    public static int RunArrayAccess()
    {
        var arr = new int[1000];
        for (int i = 0; i < 1000; i++)
            arr[i] = i * 3;
        long sum = 0;
        for (int i = 0; i < 1000; i++)
            sum += arr[i];
        return (int)(sum & 0x7FFFFFFF);
    }

    public static int RunObjectAlloc()
    {
        long sum = 0;
        for (int i = 0; i < 5000; i++)
            sum += new SmallObj(i).Value;
        return (int)(sum & 0x7FFFFFFF);
    }

    public static int RunVirtualCall()
    {
        ICalc calc = new Adder();
        long sum = 0;
        for (int i = 0; i < 10000; i++)
            sum += calc.Compute(i);
        return (int)(sum & 0x7FFFFFFF);
    }

    public static int RunLoopBranch()
    {
        long sum = 0;
        for (int i = 0; i < 20000; i++)
        {
            if (i < 10000)
                sum += i;
            else
                sum -= i;
        }
        return (int)(sum & 0x7FFFFFFF);
    }

    private sealed class SmallObj
    {
        public int Value { get; }
        public SmallObj(int v) => Value = v;
    }

    private interface ICalc
    {
        long Compute(int x);
    }

    private sealed class Adder : ICalc
    {
        public long Compute(int x) => x + 1;
    }
}
