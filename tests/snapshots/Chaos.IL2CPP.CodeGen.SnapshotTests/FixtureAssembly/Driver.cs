// This assembly provides the real PE metadata that the emitter
// reads via System.Reflection.Metadata PEReader.

using System;

namespace SnapshotTestFixtures;

public static class ArithmeticOps
{
    // Used by fixture 01-simple-add
    public static int RunAdd() => 3 + 4;
}

public static class StringOps
{
    // Used by fixture 02-ldstr-return
    public static int UseString() { Helper.ConsumeString("Hello"); return 0; }
}

public static class VoidCaller
{
    // Used by fixture 03-call-static-void
    public static int DoNothing() { Helper.Nop(); return 0; }
}

public static class MathHelper
{
    // Used by fixture 04-call-static-ret
    public static int RunSquare()
    {
        var x = 5;
        return Helper.Square(x);
    }
}

public static class FlowControl
{
    // Used by fixture 05-br-condition
    public static int IsPositive()
    {
        var result = 5 > 0;
        return result ? 1 : 0;
    }
}

// --- P1: callvirt + newobj (06) ---
public class InstanceHelper
{
    private int _val;
    public InstanceHelper(int val) { _val = val; }
    public int GetValue() => _val;
}

public static class CallVirtHelper
{
    // Used by fixture 06-callvirt-newobj
    public static int CreateAndUse()
    {
        return new InstanceHelper(42).GetValue();
    }
}

// --- P1: box + unbox.any (07) ---
public static class BoxingHelper
{
    // Used by fixture 07-box-unbox
    public static int BoxAndUnbox()
    {
        object o = 42;
        return (int)o;
    }
}

// --- P1: static field access (08) ---
public static class FieldHelper
{
    private static int _counter;

    // Used by fixture 08-field-access
    public static int GetAndIncrement()
    {
        _counter++;
        return _counter;
    }
}

// --- P1: loop (09) ---
public static class LoopHelper
{
    // Used by fixture 09-loop
    public static int SumToFive()
    {
        int s = 0;
        for (int i = 0; i < 5; i++)
            s += i;
        return s;
    }
}

// --- P1: type conversions (10) ---
public static class ConversionHelper
{
    // Used by fixture 10-conv
    public static int ConvertToInt()
    {
        long a = 100L;
        int b = (int)a;
        return b;
    }
}

internal static class Helper
{
    public static void Nop() { }
    public static int Square(int x) => x * x;
    public static void ConsumeString(string s) { }
    public static int GetValue() => 42;
}

public static class SwitchHelper
{
    // Used by fixture 11-switch
    public static int Classify() => 0;
}

public static class LdftnHelper
{
    // Used by fixture 12-ldftn
    public static int GetFnPtr()
    {
        var fn = Helper.GetValue;
        return 0;
    }
}

public static class TargetHelper
{
    // Used by fixture 12-ldftn as ldftn target
    public static int GetValue() => 42;
}

public static class ArrayHelper
{
    // Used by fixture 13-newarr
    public static int MakeAndFill()
    {
        var arr = new int[3];
        arr[0] = 42;
        return arr[0];
    }
}

public static class EhHelper
{
    // Used by fixture 15-exception-handling
    public static int SafeDivide()
    {
        int result = 42;
        int zero = 0;
        try
        {
            result = 100 / zero;
        }
        catch
        {
            result = -1;
        }
        return result;
    }
}

public static class GenericHelper
{
    public static int UseGeneric()
    {
        var w = new Wrapper<int>(42);
        return w.GetValue();
    }
}

public class Wrapper<T>
{
    private T _val;
    public Wrapper(T val) { _val = val; }
    public T GetValue() => _val;
}

public static class DelegateHelper
{
    public static int RunDelegate()
    {
        var fn = new Func<int>(Helper.GetValue);
        return fn();
    }
}

public class BaseClass
{
    public virtual int Compute() => 1;
}

public class DerivedClass : BaseClass
{
    public override int Compute() => 2;
}

public static class VirtualHelper
{
    public static int UseVirtual()
    {
        BaseClass obj = new DerivedClass();
        return obj.Compute();
    }
}

// --- Fixture 18: isinst + castclass ---
public interface IMarker { }
public class MarkedClass : IMarker
{
    public int Value;
}

public static class TypeCheckHelper
{
    public static int CheckAndCast()
    {
        object obj = new MarkedClass();
        if (obj is MarkedClass marked)
        {
            return marked.Value;
        }
        return -1;
    }
}

// --- Fixture 19: sizeof ---
[System.Runtime.InteropServices.StructLayout(System.Runtime.InteropServices.LayoutKind.Sequential)]
public struct SmallStruct
{
    public int X;
    public int Y;
}

public static class SizeOfHelper
{
    public static unsafe int GetSize()
    {
        return sizeof(SmallStruct);
    }
}

// --- Fixture 20: ldind + stind (via ref parameter) ---
public static class IndirectHelper
{
    public static int ReadWriteRef()
    {
        int val = 42;
        ref int r = ref val;
        r = 100;
        return val;
    }
}
