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
}
