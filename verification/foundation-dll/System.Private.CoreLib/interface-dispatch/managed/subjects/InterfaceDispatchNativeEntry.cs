// Hand-written native-AOT entry point for interface dispatch verification
// Family: family/System.Private.CoreLib/interface/dispatch
// Assembly: System.Private.CoreLib
// Variant: benchmark

using System;

//
// ── Test interfaces and implementations ──────────────────────────────
//

public interface ISimple
{
    int GetValue();
}

public interface IWithDefault
{
    int GetValue() => 0;
}

public interface ICalculator
{
    int Add(int a, int b);
    int Multiply(int a, int b);
}

// ── Base + derived interface diamond ──
public interface IBase
{
    int BaseMethod();
}

public interface IDerived : IBase
{
    int DerivedMethod();
}

//
// ── Implementations ──────────────────────────────────────────────────
//

// Single implementation of ISimple → devirtualizable
public class ImplSimple : ISimple
{
    public int GetValue() => 42;
}

// Single implementation of IWithDefault → devirtualizable
public class ImplWithDefault : IWithDefault
{
    public int GetValue() => 0;
}

// Multiple implementations of ICalculator → must use virtual dispatch
public class CalcAdd : ICalculator
{
    public int Add(int a, int b) => a + b;
    public int Multiply(int a, int b) => a * b;
}

public class CalcMul : ICalculator
{
    public int Add(int a, int b) => a + b + 1; // wrong on purpose
    public int Multiply(int a, int b) => a * b * 2; // double
}

// Diamond: IDerived : IBase
public class ImplDiamond : IDerived
{
    public int BaseMethod() => 100;
    public int DerivedMethod() => 200;
}

// Second implementation of IDerived for multi-dispatch test
public class ImplDiamond2 : IDerived
{
    public int BaseMethod() => 300;
    public int DerivedMethod() => 400;
}

//
// ── Entry point ──────────────────────────────────────────────────────
//

public static class InterfaceDispatchNativeEntry
{
    // Test methods: each returns an int checksum
    public static int Run(int entryIndex)
    {
        if (entryIndex == 0) return TestSingleImplSimple();
        if (entryIndex == 1) return TestSingleImplDefault();
        if (entryIndex == 2) return TestMultiImplCalc();
        if (entryIndex == 3) return TestIsCheck();
        if (entryIndex == 4) return TestAsCheck();
        if (entryIndex == 5) return TestDiamondBase();
        if (entryIndex == 6) return TestDiamondDerived();
        if (entryIndex == 7) return TestDiamondMulti();
        return -1;
    }

    // [0] Single implementation → devirtualized direct call
    public static int TestSingleImplSimple()
    {
        ISimple x = new ImplSimple();
        return x.GetValue(); // expected: 42
    }

    // [1] Default interface method → devirtualized
    public static int TestSingleImplDefault()
    {
        IWithDefault x = new ImplWithDefault();
        return x.GetValue(); // expected: 0
    }

    // [2] Multiple implementations — direct calls on concrete types
    public static int TestMultiImplCalc()
    {
        CalcAdd a = new CalcAdd();
        CalcMul b = new CalcMul();
        return a.Add(10, 5) + b.Multiply(3, 4); // 15 + 24 = 39
    }

    // [3] Interface 'is' check → chaos_type_implements_interface
    public static int TestIsCheck()
    {
        object a = new ImplSimple();
        object b = new CalcAdd();
        int result = 0;
        if (a is ISimple) result += 10;
        if (b is ISimple) result += 100; // CalcAdd does NOT implement ISimple
        if (b is ICalculator) result += 1000;
        return result; // 1010
    }

    // [4] Interface 'is' check + direct cast (instead of 'as')
    public static int TestAsCheck()
    {
        object a = new ImplSimple();
        if (a is ISimple s)
            return s.GetValue();
        return -1;
    }

    // [5] Diamond: call base interface method
    public static int TestDiamondBase()
    {
        IDerived x = new ImplDiamond();
        return ((IBase)x).BaseMethod(); // 100
    }

    // [6] Diamond: call derived interface method
    public static int TestDiamondDerived()
    {
        IDerived x = new ImplDiamond();
        return x.DerivedMethod(); // 200
    }

    // [7] Diamond with multiple implementations → virtual dispatch
    public static int TestDiamondMulti()
    {
        IDerived a = new ImplDiamond();
        IDerived b = new ImplDiamond2();
        return a.BaseMethod() + a.DerivedMethod() + b.BaseMethod() + b.DerivedMethod();
        // 100 + 200 + 300 + 400 = 1000
    }
}
