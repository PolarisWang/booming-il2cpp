// Hand-written native-AOT entry point for CCW basic verification
// Family: family/System.Private.CoreLib/ccw/basic
// Assembly: System.Private.CoreLib
// Variant: benchmark

using System;
using System.Runtime.InteropServices;

//
// ── COM interface definitions ───────────────────────────────────
//

// Simple calculator COM interface
// {7E4FCFC0-3A8F-4A0C-8C9D-1A2B3C4D5E6F}
[Guid("7E4FCFC0-3A8F-4A0C-8C9D-1A2B3C4D5E6F")]
public interface ISimpleMath
{
    int Add(int a, int b);
    int Multiply(int a, int b);
}

// COM interface with a single method returning a constant
// {9A8B7C6D-5E4F-3A2B-1C0D-9E8F7A6B5C4D}
[Guid("9A8B7C6D-5E4F-3A2B-1C0D-9E8F7A6B5C4D")]
public interface IConstantValue
{
    int GetValue();
}

//
// ── Implementations ────────────────────────────────────────────
//

public class SimpleMath : ISimpleMath
{
    public int Add(int a, int b) => a + b;
    public int Multiply(int a, int b) => a * b;
}

public class ConstantFortyTwo : IConstantValue
{
    public int GetValue() => 42;
}

// Class implementing two COM interfaces
public class DualInterfaceImpl : ISimpleMath, IConstantValue
{
    public int Add(int a, int b) => a + b + 10; // deliberately offset
    public int Multiply(int a, int b) => a * b * 2;
    public int GetValue() => 100;
}

//
// ── Entry point ────────────────────────────────────────────────
//

public static class CcwBasicNativeEntry
{
    // Test methods: each returns an int checksum
    public static int Run(int entryIndex)
    {
        switch (entryIndex)
        {
            case 0: return TestSimpleMathAdd();
            case 1: return TestSimpleMathMul();
            case 2: return TestConstantValue();
            case 3: return TestDualInterface();
            default: return -1;
        }
    }

    // [0] ISimpleMath.Add via managed dispatch
    public static int TestSimpleMathAdd()
    {
        ISimpleMath calc = new SimpleMath();
        return calc.Add(10, 5); // expected: 15
    }

    // [1] ISimpleMath.Multiply via managed dispatch
    public static int TestSimpleMathMul()
    {
        ISimpleMath calc = new SimpleMath();
        return calc.Multiply(3, 4); // expected: 12
    }

    // [2] IConstantValue.GetValue via managed dispatch
    public static int TestConstantValue()
    {
        IConstantValue cv = new ConstantFortyTwo();
        return cv.GetValue(); // expected: 42
    }

    // [3] Dual-interface implementation via managed dispatch
    public static int TestDualInterface()
    {
        var obj = new DualInterfaceImpl();
        ISimpleMath math = obj;
        IConstantValue cv = obj;
        int sum = math.Add(10, 5) + cv.GetValue(); // (10+5+10) + 100 = 125
        return sum; // expected: 125
    }
}
